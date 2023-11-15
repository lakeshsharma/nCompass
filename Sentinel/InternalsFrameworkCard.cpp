#include "stdafx.h"
#include "InternalsFramework.h"
#include "Hardware/WatchDog.h"
#include "Logging/LogString.h"
#include "Logging/Logger.h"
#include "MediaPackageManager.h"

using namespace std;

const DWORD CARDOUTMINTIME = 1000;
const DWORD CARDINMINTIME = 500;

// CInternalsFramework::CardReaderProcessingLoop()
// Monitor the card reader for state changes and update member variables accordingly.
// m_mcr transforms raw card data and reports Oasis OCR values.
void CInternalsFramework::CardReaderProcessingLoop( void )
{
	m_mcr->CardReaderProcessingLoop( &m_config );
}

void CInternalsFramework::McrSetBezelStyle(CARD_STYLE style, DWORD rankId)
{
	lock_guard<recursive_mutex> lock(m_csReadCardReader);

	if (CARD_REMOVED == style)
	{
		style = GetBezelStyle(m_machineState);
	}

	try
	{
		if (m_mcr)
		{
			m_mcr->SetBezelStyle(style, rankId);
		}
	}
	catch(...)
	{
	}
}

CARD_STYLE CInternalsFramework::GetBezelStyle(MACHINE_STATE machineState)
{
	CARD_STYLE style(CARD_STYLE_INVALID);

	switch (m_machineState)
	{
	case IN_THE_CLEAR:
		style = CARD_REMOVED;
		break;
	case SOCIAL_DISTANCE:
		style = CARD_STYLE_SOCIAL_DISTANCE;
		break;
	case CLEANING_IN_PROGRESS:
		style = CARD_STYLE_CLEANING_IN_PROGRESS;
		break;
	default:
		break;
	}

	return style;
}

void CInternalsFramework::McrSetBezelStyle(MACHINE_STATE machineState)
{
	MACHINE_STATE oldMachineState(m_machineState);
	if (machineState != oldMachineState)
	{
		m_machineState = machineState;
		CARD_STYLE style(GetBezelStyle(m_machineState));

		if (!m_mcr->IsCardInserted())
		{
			McrSetBezelStyle(style);
		}
	}
}

void CInternalsFramework::CardProcessingLoop( void )
{
	bool bIsCardInserted = m_mcr->IsCardInserted(); // Must be synchronized with mcr.Read() and mcr.WaitForCardRemoval() updates.
	bool bInserted = false;
	bool bRemoved = false;
	bool showBadCard(false);
	DWORD cardInTicks = 0;
	DWORD cardRemovedTicks = 0;

	m_adapterCheckTimeout = CUtilities::GetTickCount();
	m_adapterCheckTimeout += ADAPTER_CHECK_STARTUP_TIMEOUT; //at startup I want this to start 2 mins after startup

    // On reboot if card is inserted and player has been initalised from NVRAM then set bezel as per player's rank
	if (bIsCardInserted &&	DoesPlayerExist() && !IsPlayerVirtual() )
	{
		McrSetBezelStyle(CARD_STYLE_PLAYER, m_player->GetRankId());
	} 


	// Loop forever!
	while ( !m_shutdownReadCard )
	{
		CWatchDog::Instance().Checkin();		
		m_badCardCounter.SetBadCardReadReminderTick( 0 );
		m_badCardCounter.SetBadCardReadReminders( 0 );

		// Wait here until a card is inserted or removed.
		CCard *card(nullptr);
		bool bTimeout = true; // Output from WaitForCardEvent() is ignored.
		bInserted = false;
		bRemoved = false;
		showBadCard = false;

		WaitForCardEvent(bInserted, bRemoved, bTimeout, bIsCardInserted, cardRemovedTicks, card);

		m_sentinelState.CheckIfCardInsertionIsToBeIgnored(card);

		bInserted = (card != nullptr);

		// if card has been inserted, figure out what to do with it
		if (bInserted)
		{
			DetermineCardType(card);
			
			InsertTranslation translate = Ignored;
			{
				lock_guard<recursive_mutex> lock(m_playerCriticalSection);
				translate = m_playerSession.TransformInsertedCard(bIsCardInserted, m_employee1, m_player, card);
			}

			switch (translate)
			{
				case Ignored:
					bInserted = false;
					bRemoved = false;
					showBadCard = true;
					break;
				case Inserted:
					bInserted = true;
					bRemoved = false;
					break;
				case Removed:
					bInserted = false;
					bRemoved = true;
					break;
				case RemovedInserted:
					bInserted = true;
					bRemoved = true;
					break;
				case StayCarded:
					bInserted = false;
					bRemoved = false;
					showBadCard = false;
					GenerateTilt(tltCardReaderError);
					break;
			}
		}
		

		// Postpone a (potential) virtual player session recard, if an employee is carding in over another employee.
		bool postponePlayerRecard(false);
		if (bInserted && bRemoved && m_employee1 != NULL && card->IsEmployee())
		{
			postponePlayerRecard = true;
		}

		if (bRemoved)
			CardRemoved(cardInTicks, postponePlayerRecard);

		if (bInserted)
		{
			if (bRemoved)
			{
				// Device indicator will turn Blue momentarily indicating existing session end and then turns green to indicate Employee session in progress.
				CUtilities::Sleep(DISPLAY_PLAYER_OUT_EMPLOYEE_IN_PERIOD);
			}

			HandleCardInserted(card, bIsCardInserted);
			card = nullptr;
		}
		else if(showBadCard)
		{
			McrSetBezelStyle(CARD_BAD_READ);
			SendBadCardReadToUI();
			m_badCardCounter.SetBadCardReadReminderTick(CUtilities::GetTickCount() + BAD_CARD_READ_REMINDER_INTERVAL);
			m_badCardCounter.SetBadCardReadReminders(BAD_CARD_READ_MAX_REMINDERS - 1);
			m_playerSession.SetVirtualBadCardRead(m_mcr->IsCardInserted());
		}

		// It is unlikely, but if an employee card in (over another employee) failed
		// then there may be an virtual player session that we need to be recarded.
		if (m_employee1 == nullptr && IsPlayerVirtualFromEmployeeCardIn())
		{
			RecardPlayer(true);
		}

		//if we've done too many consecutive bad card reads,
		//send the transaction to poller.
		if (m_badCardCounter.IsTooManyBadCardReads())
		{
			m_badCardCounter.ResetBadReadCount();
			SendBadCardReadToSCF();
		}

		if (card)
		{
			delete card;
			card = nullptr;
		}

		CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
	}

	m_mcr->ShutdownCardReader();
}

void CInternalsFramework::CardRemoved(DWORD cardInTicks, bool postponePlayerRecard)
{
	// mobile session, card removed, if it was employee card
	if (m_pairingInformation.IsMobileCardedSession() && !IsPlayerGoodbyeRequested())
	{
		m_bCardInserted = false;
		if (DoesEmployeeExist())
		{
			EmployeeCardRemoved(CUtilities::GetCurrentTime());
		}
		if (m_player && m_player->IsVirtual())
		{
			McrSetBezelStyle(CARD_REMOVED);
			m_config.ClearLastCardRead();
			m_displayManager->ClearLastCardRead();
		}
		
		if (m_player && !(m_player->IsVirtual()) && !DoesEmployeeExist()) // for mobilePlayer, if player session is in progress but employee card is removed
			McrSetBezelStyle(CARD_STYLE_PLAYER);
		
		m_protocolManager->ClearInboundQueueStarvation(IProtocolManager::UIProtocol, PERSIST_CONTENT_WINDOW);
	}
	//	Process for player goodbye button too, employee goodbye will be implcitly handled.
	//	Process for mobile session when player is virtual.
	else if (
		!m_pairingInformation.IsMobileCardedSession() ||
		IsPlayerGoodbyeRequested() ||
		(m_pairingInformation.IsMobileCardedSession() && m_player->IsVirtual())
		)
	{
		if (IsPlayerGoodbyeRequested())
			SetPlayerGoodbyeRequested(false);

		HandleCardRemoved(cardInTicks, postponePlayerRecard);
	}

}

void CInternalsFramework::CheckIfBackgroundSnapshotIsInProgress()
{
	bool snapshotInProgressMessageSent(false);

	int meterCapturePercentage(0);
	while (m_sentinelState.IsBackgroundSnapshotInProgress(meterCapturePercentage) && !m_shutdownReadCard)
	{
		CWatchDog::Instance().Checkin();
		LogString(GAMES, "Adding ALERT_METER_SNAPSHOT_IN_PROGRESS, percentage=%d", meterCapturePercentage);
		m_notificationBox.AddAlert(Alerts::ALERT_METER_SNAPSHOT_IN_PROGRESS, (WORD)meterCapturePercentage);
		SendSubscriptionUpdateIfNecessary();
		snapshotInProgressMessageSent = true;
		CUtilities::Sleep(ONE_SECOND);
	}

	if (snapshotInProgressMessageSent)
	{
		LogString(GAMES, "Removing ALERT_METER_SNAPSHOT_IN_PROGRESS");
		m_notificationBox.RemoveAlert(Alerts::ALERT_METER_SNAPSHOT_IN_PROGRESS);
		SendSubscriptionUpdateIfNecessary();
		CUtilities::Sleep(ONE_SECOND);
	}
}

void CInternalsFramework::HandleCardRemoved(DWORD cardInTicks, bool postponePlayerRecard )
{
	DWORD cardInsertedTicks = CUtilities::GetTickCount() - cardInTicks;
	if ( cardInsertedTicks < CARDOUTMINTIME && cardInsertedTicks > 0 )
	{
		// Card out came within 1 second of card in, add a delay so UI can catch up.
		// cardInsertedTicks < 0 means ticks rolled over to 0.
		CUtilities::Sleep(CARDOUTMINTIME - cardInsertedTicks);
	}

	LogString( "GENERAL", "DEBUG: Some Card Removed." );
	// If here, Card Removed.

	CheckIfBackgroundSnapshotIsInProgress();

	// If there is PBT in progress (especially promo to EGM) then wait a while before determining whether to start a virtual session.
	//Tracker 27219
	//only do this check if normal virtual sessions are enabled 
	if ( m_config.VirtualSessionEnabled())
	{
		int loopCount = 0;
		//if we are in the middle of a download to the game we want to hold of on checking the credit meter
		//for virtual sessions until the download completes.
		while(m_personalBanker->IsPersonalBankerTransferToGame() && loopCount < 30)
		{
			CUtilities::Sleep(1000);
			loopCount++;
		}

		//only do this if we actually looped.
		if(loopCount > 0)
		{
			//I put in this delay so that the transfer message could stay on screen for a while.
			CUtilities::Sleep(2000);
		}
	}

	time_t removalTime = CUtilities::GetCurrentTime();
	time_t utcRemovalTime = CUtilities::GetUTCTime();

	m_bCardInserted = false;
	bool playerRecarded(false);
	bool bannedPlayerCardIsRemoved(false);

	// Process the removal of a player card or employee card
	if (!PlayerCardRemoved(removalTime, utcRemovalTime, bannedPlayerCardIsRemoved))
	{
		bool employeeCardRemoved = EmployeeCardRemoved( removalTime );
		if (!postponePlayerRecard)
		{
			playerRecarded = RecardPlayer(employeeCardRemoved);
		}

		if (employeeCardRemoved)
		{
			m_protocolManager->ClearInboundQueueStarvation(IProtocolManager::UIProtocol, PERSIST_CONTENT_WINDOW);
		}
	}
	
	if (!playerRecarded)
	{
		if ( IsPlayerVirtual() || IsEmployeeVirtual() || m_personalBanker->IsPersonalBankerTransferInProgress())
		{
			SendCardSessionInProgressToUI(m_personalBanker->IsPersonalBankerTransferInProgress());
		}
		else
		{
			if (bannedPlayerCardIsRemoved)
			{
				m_protocolManager->ClearInboundQueueStarvation(IProtocolManager::UIProtocol, PERSIST_CONTENT_WINDOW);
			}	
		}
	
		if ((!m_player && !DoesEmployeeExist()) || (m_player && m_player->IsVirtual() && !DoesEmployeeExist())) // for mobilePlayer, if employee card is already inserted
		{
			McrSetBezelStyle(CARD_REMOVED);
			m_config.ClearLastCardRead();

			m_displayManager->ClearLastCardRead();
		}
		
	}

	// Tell UI card removed
	CInternalMessage* msg = new CInternalMessage(INT_MSG_CARD_REMOVED);
	SendMessageToUI(msg, QUEUE_PRIORITY_HIGHEST);
}

bool CInternalsFramework::HandleCardInserted( CCard *card, bool bIsCardInserted)
{
	m_bCardInserted = true;
	bool bCardAccepted = false;
	CARD_STYLE style(CARD_BAD_READ);
	int IDLength;

	if ( card->IsValidRead() )
	{
		m_badCardCounter.ResetBadReadCount();
		m_config.SetLastCardRead(card->GetCardID(IDLength));
		m_displayManager->SetLastCardRead(card->GetCardID(IDLength));

		// Determine the card type
		CardType cardType = DetermineCardType( card );

		switch( cardType )
		{
		case CARD_PLAYER:
		case CARD_TEMP:
			bCardAccepted = PlayerCardInserted( card, true);
			if (bCardAccepted)
			{
				style = CARD_STYLE_PLAYER;			
			}
			break;
		case CARD_MECHANIC:
		case CARD_GLOBAL:
		case CARD_FLOOR:
		case CARD_CURRENCY:
		case CARD_METER:
				bCardAccepted = EmployeeCardInserted( card, true);
				if (bCardAccepted)
				{
					style = CARD_STYLE_EMPLOYEE;
				}
				break;
		default:
			bCardAccepted = false;
			break;
		}
		// Only start bad card read and timer when no card inserted and card not accepted, otherwise clear bad card read and timer (ie true)
		m_playerSession.SetVirtualBadCardRead(bIsCardInserted || bCardAccepted);
		McrSetBezelStyle(style);

		// Tracker #22680 -- if valid card read has unknown prefix, show Bad card read on UI
		// Scracker #1834 -- don't show bad card read if transaction log is full
		if (!bCardAccepted && m_sentinelState.TransactionFillStatusOkay())
		{
			SendBadCardReadToUI();
			m_badCardCounter.SetBadCardReadReminderTick( CUtilities::GetTickCount() + BAD_CARD_READ_REMINDER_INTERVAL );
			m_badCardCounter.SetBadCardReadReminders( BAD_CARD_READ_MAX_REMINDERS - 1 );
		}
	}
	else
	{
		m_playerSession.SetVirtualBadCardRead(bIsCardInserted);
		m_badCardCounter.IncBadReadCount();
		McrSetBezelStyle(CARD_BAD_READ);

		// Per BRD if employee session active bad card read should not interrupt current session
		if (m_employee1 == NULL)
		{
			SendBadCardReadToUI();
		}
		m_badCardCounter.SetBadCardReadReminderTick( CUtilities::GetTickCount() + BAD_CARD_READ_REMINDER_INTERVAL );
		m_badCardCounter.SetBadCardReadReminders( BAD_CARD_READ_MAX_REMINDERS - 1 );
		bCardAccepted = false;
	}

	if ( !bCardAccepted )
	{
		delete card; 
	}
	
	return bCardAccepted;
}

void CInternalsFramework::ShowRFIDCardRemoved(bool bSendAttractorToUI)
{
	LogString(GENERAL, "DEBUG: RFID Card Removed.");

	m_bCardInserted = false;

	if (bSendAttractorToUI)
	{
		LogString(UI_LOG, "%s %d", __FUNCTION__, __LINE__);
		SendDisplayAttractorToUI();
	}

	McrSetBezelStyle(CARD_REMOVED);
	m_config.ClearLastCardRead();
	m_displayManager->ClearLastCardRead();
}

void CInternalsFramework::WaitForCardEvent(bool &bInserted, bool &bRemoved, bool &bTimeout, bool &bIsCardInserted, DWORD cardRemovedTicks, CCard *&retCard)
{
	CCard *card = nullptr;

	while (bTimeout && !m_shutdownReadCard)
	{
		CheckIfBackgroundSnapshotIsInProgress();

		if (m_mcr->IsOpenErrorAndCardInserted())
		{
			ForcePlayerCardOut();
		}

		card = m_mcr->McrWaitForCardEvent(bInserted, bRemoved, bTimeout, bIsCardInserted);

		if (bTimeout && (m_tcpMcr != nullptr))
		{
			// We do not have any new card in/out from hardware MCR, 
			// check if network emulator has any event
			card = m_tcpMcr->TcpMcrWaitForCardEvent(bInserted, bRemoved, bTimeout);
		}

		CWatchDog::Instance().Checkin();

		if (!bTimeout)
		{
			if (bInserted && card != NULL)
			{
				bTimeout = false;

				if (card->IsValidRead())
				{
					// Looks like a valid card
					DWORD ticks = CUtilities::GetTickCount() - cardRemovedTicks;

					// Tracker #21732 - Compare commID with card's site id.
					//also compare card in timeout.
					if ((ticks < CARDINMINTIME) || (card != NULL && !card->CheckCommID(m_config.GetCommID())))
					{
						//ignore this read so that we do not back up the UI with quick card in and outs
						//or ignore for a invalid comm ID
						bInserted = false;
						bTimeout = true;
						delete card;
						card = NULL;
						// for mobile session ignore bad card
						McrSetBezelStyle(CARD_BAD_READ);
						SendBadCardReadToUI();
						m_badCardCounter.SetBadCardReadReminderTick(CUtilities::GetTickCount() + BAD_CARD_READ_REMINDER_INTERVAL);
						m_badCardCounter.SetBadCardReadReminders(BAD_CARD_READ_MAX_REMINDERS - 1);
						m_playerSession.SetVirtualBadCardRead(m_mcr->IsCardInserted());
					}

					// Make sure card in delay is past, only do this if transfer is pending and card read wasn't timed out.
					if (m_personalBanker->IsAutoTransferPending() && !bTimeout)
					{
						CardType cardType = CARD_UNKNOWN;
						// Determine the card type
						if (card != nullptr)
						{
							cardType = DetermineCardType(card);
						}

						if (cardType != CARD_MECHANIC && cardType != CARD_GLOBAL && cardType != CARD_FLOOR)
						{
							//Since we are now retrying the PBT upload transfers we need to allow employee cards through
							// Not same card, disallow card and act as if timeout.
							bInserted = false;
							bTimeout = true;
							if (card != NULL)
							{
								delete card;
								card = nullptr;
							}
							McrSetBezelStyle(CARD_BAD_READ);
							SendBadCardReadToUI();
							m_badCardCounter.SetBadCardReadReminderTick(CUtilities::GetTickCount() + BAD_CARD_READ_REMINDER_INTERVAL);
							m_badCardCounter.SetBadCardReadReminders(BAD_CARD_READ_MAX_REMINDERS - 1);
							m_playerSession.SetVirtualBadCardRead(m_mcr->IsCardInserted());
						}
					}

				}
			}
		}
		else
		{
			NoCardActivity(bRemoved, bTimeout);
			if (bTimeout && !bIsCardInserted)
			{
				card = AutoCardActivity(bInserted, bRemoved, bTimeout);
			}
		}
	}

	retCard = card; // Set output to the current value of card.
}


void CInternalsFramework::NoCardActivity(bool &bRemoved, bool &bTimeout)
{
	if (m_employeeGoodbyeRequested)
	{
		m_employeeGoodbyeRequested = false;

		// See if there is an eligible employee to remove the card for.
		if ((m_employee2 != NULL && m_employee2->IsGoodbyeEnabled()) ||
			(m_employee1 != NULL && m_employee1->IsGoodbyeEnabled() && !m_employee1->IsVirtual()))
		{
			LogString("GENERAL", "DEBUG: Employee Goodbye -- carding out.");
			bRemoved = true;
			bTimeout = false;
		}
		else
		{
			LogString("GENERAL", "DEBUG: Employee Goodbye -- no card.");
		}
	}
	else if (IsPlayerGoodbyeRequested())
	{
		// See if there is an eligible player to remove the card for.
		lock_guard<recursive_mutex> lock(m_playerCriticalSection);
		if (m_player != NULL)
		{
			if (m_player->IsGoodbyeEnabled() && !m_player->IsVirtual())
			{
				LogString("GENERAL", "DEBUG: Player Goodbye -- carding out.");
				bRemoved = true;
				bTimeout = false;
			}
			else
			{
				LogString("GENERAL", "DEBUG: Player Goodbye -- no eligible card.");
			}
		}
	}

	// Check if still no activity.
	if (bTimeout)
	{
		// No card activity
		// See if we need to send a bad card reminder to the UI.
		// This wil replay the sound
		if (m_badCardCounter.IsBadCardReminderRequired())
		{
			McrSetBezelStyle(CARD_BAD_READ);
			SendBadCardReadToUI();
			m_badCardCounter.SetBadCardReadReminderTick(CUtilities::GetTickCount() + BAD_CARD_READ_REMINDER_INTERVAL);
			m_badCardCounter.DecBadCardReadReminders();
			m_playerSession.SetVirtualBadCardRead(m_mcr->IsCardInserted());
		}
		PerformCardTimedEvents();

		// Sleep here -- another reader thread now does the sleep on mcr.WaitForCardRemoval().
		CUtilities::Sleep(CARD_READER_LOOP_SLEEP_PERIOD);
	}

}

CCard *CInternalsFramework::AutoCardActivity(bool &bInserted, bool &bRemoved, bool &bTimeout)
{
	CCard *card(m_mcrAuto->GetCardEvent(bInserted, bRemoved, bTimeout, CUtilities::GetTickCount()));
	return card;
}



// Tell GCF to get balances
void CInternalsFramework::TellGCFToGetBalances()
{
	CInternalMessage *msg = new CInternalMessage(INT_MSG_GET_PB_BALANCES);
	this->SendMessageToGCF(msg, QUEUE_PRIORITY_NORMAL);
}

/// <summary>
/// Performs card timed events for players.
/// </summary>
void CInternalsFramework::PerformPlayerCardTimedEvents(InternalMsgType msgType)
{
	// Peform PersonalBanker timed events before general player timed events, in case UI updates are affected.
	PerformPersonalBankerTimedEvents();

	bool bPlayerWasRemoved = PlayerTimedEvents(msgType);

	// TODO: Defect 91621:nCompass: Bad card read screen not displayed when player card times out.
	bool sentDisplayAttractorToUI = false;
	if ( bPlayerWasRemoved && !DoesEmployeeExist() )
	{
		sentDisplayAttractorToUI = true;
		SendDisplayPlayerRemovedToUI();
	}

	if (m_playerSession.HasVirtualBadCardReadExpired())
	{
		PerformVirtualBadCardReadExpired(sentDisplayAttractorToUI);
	}
}

void CInternalsFramework::PerformVirtualBadCardReadExpired(bool sentDisplayAttractorToUI)
{
	if (m_employee1 != NULL)
	{
		m_mcr->SetBezelStyle(CARD_STYLE_EMPLOYEE);
	}
	else if (m_player != NULL && !m_player->IsVirtual())
	{
		m_mcr->SetBezelStyle(CARD_STYLE_PLAYER, m_player->GetRankId());
	}
	else if (m_player != NULL && m_player->IsVirtual())
	{
		SendCardSessionInProgressToUI(m_personalBanker->IsPersonalBankerTransferInProgress());
		m_mcr->SetBezelStyle(CARD_REMOVED);
	}
	else if (!sentDisplayAttractorToUI)
	{
		LogString(UI_LOG, "%s %d", __FUNCTION__, __LINE__);
		SendDisplayAttractorToUI();
		m_mcr->SetBezelStyle(CARD_REMOVED);
	}
}

/// <summary>
/// Performs card timed events.
/// </summary>
void CInternalsFramework::PerformCardTimedEvents( void )
{
	PerformPlayerCardTimedEvents(INT_MSG_NONE);

	CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);

	TicketTimedEvents();

	CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);

	EmployeeTimedEvents();

	CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);

	PerformAdapterTimedEvents();

	CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);

	PerformProgressiveTimedEvents();

	CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);

	PerformPlayerSessionTimedEvents();

	CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
}

void CInternalsFramework::PerformAdapterTimedEvents( void )
{
	ULONGLONG currentTick;
	currentTick = CUtilities::GetTickCount();
	//handle a wrap around
	if (currentTick < m_adapterCheckTimeout)
	{
		currentTick += 0xffffffff; //the tickcount wraps at DWORD max
	}
	//check our IP address
	if (currentTick - m_adapterCheckTimeout > ADAPTER_CHECK_TIMEOUT)
	{
		string ip;
		lock_guard<recursive_mutex> lock(m_adapterCriticalSection);
		{
			m_config.GetActualIP().GetIP(ip);
			//If network cable unplugged and then plugged again, then restart network service to discover new ip
			if(ip == "0.0.0.0" || (ip.find("169.") == 0) || NetworkUtils::Instance().GetNetworkCableToggleState())
			{
				ConfigureNetworkAdapter();
				m_config.GetActualIP().GetIP(ip);
			}

			if (m_sentinelIp != ip)
			{
				LogString(NETWORKING_LOG, "[CInternalsFramework::PerformAdapterTimedEvents] Sentinel IP change detected. "
					"Old: [%s] New: [%s]", m_sentinelIp.c_str(), ip.c_str());
				m_sentinelIp = ip;
				SendMessageToSCF(new CInternalMessage(INT_MSG_SET_STATUS, m_sentinelIp), QUEUE_PRIORITY_NORMAL);
			}

			m_adapterCheckTimeout = CUtilities::GetTickCount();
		}
	}
}

CardType CInternalsFramework::DetermineCardType( CCard *card )
{
	CardType cardType = card->GetCardType();

    if (cardType == CARD_UNKNOWN) 
    {
        byte cardTypeNumber = card->GetCardTypeNumber();

        if ( m_config.GetGlobalCardPrefix() == cardTypeNumber )
        {
            cardType = CARD_GLOBAL;
        }
        else if ( m_config.GetMechanicCardPrefix() == cardTypeNumber )
        {
            cardType = CARD_MECHANIC;
        }
        else if ( m_config.GetFloorCardPrefix() == cardTypeNumber )
        {
            // This can be a floor, meter, or currency card
            cardType = card->DetermineFloorCardType();
        }
        else if ( m_config.GetTempCardPrefix() == cardTypeNumber )
        {
            cardType = CARD_TEMP;
        }
        else if ( m_config.IsPlayerCardPrefix( cardTypeNumber, card->GetIsUniversalCard() ) )
        {
            cardType = CARD_PLAYER;
        }
        else if (m_config.NclCardsEnabled())
        {
            cardType = CARD_PLAYER;
        }
		else if (m_config.GetUniverseEnabled() && m_config.GetUseSDSCardFormat())
		{
			cardType = CARD_PLAYER;
		}

        card->SetCardType( cardType );
    }
    else
    {
        LogString( GENERAL, _T("Card type predetermined. Skip determining type based upon OCR") );
    }

	return cardType;
}

void CInternalsFramework::SendEmployeeCardMessageToSCF( bool bCardIn )
{
	// Make a copy of the employee object
	CEmployee *employee = NULL;
	if ( m_employee2 != NULL )
		employee = new CEmployee( *m_employee2 );
	else
		employee = new CEmployee( *m_employee1 );

	// Get the card
	const CCard *card = employee->GetCard();

	// Get the transaction time.
	time_t *transactionTime = new time_t();
	*transactionTime = bCardIn ?  card->GetCardInTime() : card->GetCardOutTime();

	// prepare the transaction message.
	CInternalMessage *msg = PrepareTransactionMessage( bCardIn ? INT_MSG_TRANSACTION_EMPLOYEE_CARD_IN : INT_MSG_TRANSACTION_EMPLOYEE_CARD_OUT, IsPlayerVirtual(), transactionTime, NULL);

	if ( msg != NULL )
	{
		// Add Player to the message.
		msg->SetData(employee);

		SendTransactionMessageToSCF( msg, QUEUE_PRIORITY_NORMAL );
	}
	else
	{
		delete employee;
	}

}

// #UI_REWRITE_CLEANUP This function can be removed as the msgs are not being processed by UIProtocol
void CInternalsFramework::SendCardSessionInProgressToUI(bool bPBTransferInProgress)
{	
	CInternalMessage *msg = new CInternalMessage(bPBTransferInProgress ? INT_MSG_TRANSFER_PENDING : INT_MSG_CARDED_SESSION_IN_PROGRESS);
	int priority(QUEUE_PRIORITY_HIGHEST);
	if (bPBTransferInProgress)
	{
		msg->SetMessagePersist(m_msgPriority.PrioritizeMessage(msg, priority));
	}	
	SendMessageToUI(msg, priority);
}

void CInternalsFramework::SendSystemBusyToUI(bool enable)
{
	if (enable)
	{
		m_notificationBox.AddAlert(Alerts::ALERT_SYSTEM_BUSY);
	}
	else
	{
		m_notificationBox.RemoveAlert(Alerts::ALERT_SYSTEM_BUSY);
	}
	SendSubscriptionUpdateIfNecessary();
}

// UI_REWRITE_CLEANUP: We need to understand whether this function has any significance left
void CInternalsFramework::SendDisplayAttractorToUI(bool uncarded)
{
	if (m_personalBanker->DelayAutoUpload())
	{
		LogString(PBT_LOG, "DelayAutoUpload - skipping INT_MSG_DISPLAY_ATTRACTOR");
	}
	else
	{
		LogString(UI_LOG, "DEBUG: Send Display Attractor to UI.");

		if (uncarded)
		{
			if (m_playerSession.GetPlayerTypeEnum() == PlayerTypeState::PLAYERTYPEUNCARDED)
			{
				LogString(UI_LOG, "%s %d : Setting notification state as uncarded.", __FUNCTION__, __LINE__);
				m_notificationBox.SetNotificationState(NotificationState::UNCARDED);
			}
			else if (m_player == nullptr || !m_player->IsVirtual())
			{
				LogString(UI_LOG, "%s %d : Setting notification state as idle.", __FUNCTION__, __LINE__);
				m_notificationBox.SetNotificationState(NotificationState::IDLE);
				SendSubscriptionUpdateIfNecessary();
			}
		}
	}
}

// The caller is responsible for ensuring that neither a carded player session nor a carded employee session is in progress.
void CInternalsFramework::SendDisplayPlayerRemovedToUI()
{
	 if (m_personalBanker->IsPersonalBankerTransferInProgress())
	 {
		 SendCardSessionInProgressToUI(true);
	 }
	else if (IsPlayerVirtual() || IsEmployeeVirtual())
	{
		SendCardSessionInProgressToUI(false);
	}
	else 
	if (m_bCardInserted || m_playerSession.IsVirtualBadCardRead())
	{
		SendBadCardReadToUI();
	}
	else
	{
		LogString(UI_LOG, "%s %d", __FUNCTION__, __LINE__);
		SendDisplayAttractorToUI();
	}

}
void CInternalsFramework::SendBadCardReadToUI( void )
{
	// Per BRD if employee session active bad card read should not interrupt current session
	// To meet the BRD employee session requirement for bad reads, it needs to have precedence over
	// the BRD player session requirements.
	if (m_employee1 == NULL)
	{
		LogString(UI_LOG, "DEBUG: Send Bad Card Read to UI");
		CInternalMessage *msg = new CInternalMessage( INT_MSG_BAD_CARD_READ );
		int priority = QUEUE_PRIORITY_NORMAL;
		CMessagePersist *mp(m_msgPriority.PrioritizeMessage(msg, priority));
		if (!m_mcr->IsCardInserted())
		{
			// RFID reader, change persist to shorter duration
			mp->SetMainStarveTicks(POPUPDISPLAYTIME);
			const bool isRFID = true;
			msg->SetData((WORD)isRFID);
		}
		msg->SetMessagePersist(mp);
		SendMessageToUI( msg, priority );
	}
}

void CInternalsFramework::SendBadCardReadToSCF( void )
{
	CInternalMessage *msg = PrepareTransactionMessage( INT_MSG_BAD_CARD_READ, IsPlayerVirtual() );
	SendTransactionMessageToSCF( msg, QUEUE_PRIORITY_ABOVE_NORMAL );
}

bool CInternalsFramework::IsPlayerGoodbyeRequested()
{
	return m_playerGoodbyeRequested;
}

void CInternalsFramework::SetPlayerGoodbyeRequested(bool val)
{
	m_playerGoodbyeRequested = val;
}

