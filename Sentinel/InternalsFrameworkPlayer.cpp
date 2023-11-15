#include "stdafx.h"

#include "InternalsFramework.h"

#include "Hardware/NonVolatileMemoryMaps.h"
#include "OneTimeBonusEvent.h"
#include "MultiplierBonusEvent.h"
#include "UI/PersonalBankerLog.h"
#include "Logging/Logger.h"
#include "Logging/LogString.h"
#include "PollerProtocol/PollerMGMDData.h"

using namespace std;

const int PLAYER_UPDATE_INTERVAL = 3600;	// Interval to send player updates in seconds.
const DWORD SEND_CARD_IN_TO_SPEED_MEDIA_MILLISECONDS_TIMEOUT = 10000;

// Initilaize player at start-up
void CInternalsFramework::InitializePlayer( void )
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	if ( CPlayer::DoesPlayerExistInNVRAM() )
	{
		// This will build the player from NVRAM
		// Use a volatile temporary variable to ensure that m_player does not point to the new memory until
		// after the constructor completes.
		CPlayer * volatile player = new CPlayer(m_config, m_EGMConfig);
		m_player = player;

		// If the player is virtual at startup, close out the virtual session properly.
		if ( m_player->IsVirtual() )
		{
			EndVirtualPlayerSession( CUtilities::GetCurrentTime() );
			LogString(UI_LOG, "%s %d", __FUNCTION__, __LINE__);
			SendDisplayAttractorToUI();
		}

		// End active player session
 		else
		{
			EndActivePlayerSession( CUtilities::GetCurrentTime(), CUtilities::GetUTCTime(), false );
			LogString(UI_LOG, "%s %d", __FUNCTION__, __LINE__);
			SendDisplayAttractorToUI();
		}
	}
	else
	{
		CPlayer::MarkSessionMetersInactiveInNVRAM();
		LogString(UI_LOG, "%s %d", __FUNCTION__, __LINE__);
		SendDisplayAttractorToUI();
	}

	m_playerSession.SetPlayerState(m_player, (m_mcr ? m_mcr->IsCardInserted() : false));
	m_playerSession.SetPrevPlayerType(m_playerSession.GetPrevPlayerType());
	DistributePlayerData(true, true);

	// Seed the sequence number and EFT number.
	m_pbSequence = (WORD)CUtilities::GetTickCount();
}

#ifdef _DEBUG
/// <summary>
/// Remove player at shutdown
/// </summary>
/// <remarks>
/// This method should only be called when testing testing for memory leaks on nice shutdown.
/// Do NOT call this method during normal operation, because it will clear this information from NVRAM (tracker 20165).
/// </remarks>
void CInternalsFramework::UninitializePlayer( void )
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	if ( m_player != NULL )
	{
		m_dataSubscriptionManager.UnregisterProvider(m_player);
		delete m_player;
		m_player = NULL;
	}

	m_personalBanker->DeleteTransferAndInfo();
}
#endif // _DEBUG.

/// <summary>
/// This gets called when a player card is inserted.
/// Returns true if the card was accepted.
/// <para/>
/// The caller MUST ensure that the cardID byte array for the card number provided is NOT NULL.
/// <para/>
/// The caller MUST ensure that the length of the card number provided is EXACTLY the same
/// length as the card number for any transaction already in progress.
/// </summary>
/// <param name="card">IN/OUT - information about the card inserted.</param>
/// <param name="goodbyeEnabled">IN - true if the a goodbye button should be enabled for this player session, false otherwise.</param>
/// <returns>true if if the card was accepted, false otherwise.</returns>
/// <remarks>
/// Caller MUST NOT access "card" after calling if this method returns true.
/// Caller is responsible for deleting "card" if this method returns false.
/// </remarks>
bool CInternalsFramework::PlayerCardInserted(CCard *card, bool goodbyeEnabled)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	bool bCardAccepted = true;

	// This will close out any open player sessions just in case.
	// NOTE: this wont close out a virtual session 
	EndActivePlayerSession( card->GetCardInTime(), CUtilities::GetUTCTime(), false );

	// See if we currently have a player.
	if ( m_player != nullptr )
	{
		LogString(GENERAL, "PlayerCardInserted - there is an existing player");
		// If this is the current player, end the virtual session.
		if ( m_player->CompareCard( card ) )
		{
			LogString(GENERAL, "PlayerCardInserted end the virtual session");
			EndVirtualPlayerSession( card->GetCardInTime(), true );	// This will delete the player.
			if (m_player && m_pairingInformation.IsMobileCardedSession())
				m_player->SetMobilePlayer();
		}
		
		// If the current player is in a virual session, we cannot accept this card.
		else if ( m_player->IsVirtual() )
		{
			LogString(GENERAL, "PlayerCardInserted card ignored 1");
			bCardAccepted = false;
		}
	}

	// If we don't have a player but there is still a pb session, 
	//  we can only accept this player if it is the player as the transfer.
	else if ( card != NULL )
	{
		if ( m_personalBanker->HasTransferForDifferentCardNumber(*card) )
		{
			LogString(GENERAL, "PlayerCardInserted card ignored 2");
			bCardAccepted = false;
		}
	}

	//If there is a meter change in progress, disregard a player card.
	//If we are in an offline state, disregard a player card.
	//If transaction fill status is not Okay
	else if (m_sentinelState.IsMeterChangeInProgress() || 
			 m_EGMConfig.GetSentinelOnlineState() != sentinelOnline ||
			 !m_sentinelState.TransactionFillStatusOkay())
	{
		LogString(GENERAL, "PlayerCardInserted card ignored 3");
		bCardAccepted = false;
	}

	CUtilities::Sleep( SLEEP_YIELD_MILLISECONDS );

	bool stateChanged = false;
	
	if ( bCardAccepted )
	{
		if (!m_pairingInformation.IsMobileCardedSession())
		{
			m_protocolManager->ClearInboundQueueStarvation(IProtocolManager::UIProtocol, PERSIST_CONTENT_WINDOW);
		}
		
		// If we do not have a player object at this time, make a new one.
		if ( m_player == NULL )
		{
			//We want to delete this auto upload card to prevent auto upload of credits
			//to a previous stored OCR when the next player does a fast cardin/cardout
			//without waiting for a player response
			m_personalBanker->DeleteCard();
			m_personalBanker->RemoveSession();
			m_personalBanker->SetGuestPlayer(true);
			LogString(GENERAL, "PlayerSession: cardNumber=\"%s\" -- Starting carded player session",
				CCard::MakeCStringCardNumber(card).c_str());

			m_player = new CPlayer(card, MakeDefaultPlayerInfo(), m_config, m_EGMConfig, CUtilities::GetCurrentTime(), goodbyeEnabled, &m_games);

			if (m_pairingInformation.IsMobileCardedSession())
				m_player->SetMobilePlayer();

			m_dataSubscriptionManager.RegisterProvider(m_player);
			SendSubscriptionUpdateIfNecessary();
		}
		else
		{
			m_personalBanker->SetGuestPlayer(m_player->IsGuest());
			LogString(GENERAL, "PlayerSession: cardNumber=\"%s\" -- Continuing player session as carded",
				CCard::MakeCStringCardNumber(card).c_str());
		}

		// Store a snapshot of the meters when the session is started.
		CMeters *sessionStartMeters = GetTransactionMeters( MEMORY_NVRAM, NVRAM_PLAYER_START_METERS_OFFSET );
		m_player->SetSessionStartMeters( sessionStartMeters );

		//Set current meters as copy of start meters at this time.
		CMeters *startMeters = m_player->GetSessionStartMetersCopy();
		startMeters->SetMemoryType(MEMORY_NVRAM);
		startMeters->SetOffset(NVRAM_PLAYER_START_METERS_CURRENT_OFFSET);
		startMeters->SetAllowedSize(NVRAM_PLAYER_START_METERS_CURRENT_SIZE);
		/* NOTE : passing false here because we are handling player session meters and 
		   we want to maintain backward compatibility with offsets on which these 
		   meters were being written in the NVRAM earlier. */
		startMeters->InitialWriteToPermanentStore(false);
		m_player->SetSessionStartMetersCurrent(startMeters);

		// Make a copy of the session start meters for the card-in transaction and send the transaction.
		CMeters *cardInMeters = new CMeters( *sessionStartMeters );
		
		
		LogString(POLLER_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);

		// Check if we are in the cardless session right now, if yes we have to end it before sending Card-In
		PlayerTypeState oldPlayerType = m_playerSession.GetPlayerTypeEnum();
		if (oldPlayerType == PLAYERTYPEUNCARDED)
		{
			SendCardlessSessionTransactionsToSCF(/*sessionStart*/ false);
		}

		stateChanged = m_playerSession.SetPlayerState(m_player, (m_mcr ? m_mcr->IsCardInserted() : false));
		m_playerSession.SetLastPlayedGameSessionGameNumberAndDenom(m_games);
		
		SendPlayerTransactionToSCF( PTT_CARD_IN, cardInMeters );

		// Clear previous Media Window card out player state for PBT in progress or pending.

		// The display manager will make sure the correct window isdisplayed for the player card.
		MediaWindowPlayerState playerState;
		playerState = m_displayManager->PlayerCardInserted(GetPbtInProgress(), m_mediaWindowPlayerStatePending);
		TellUIMediaWindowPlayerState(playerState);
		
		if (m_pairingInformation.IsMobileCardedSession() && m_notificationBox.WaitingForMobileConnection())
		{
			m_notificationBox.SetMobileState(MobileState::Connected);
			SendSubscriptionUpdateIfNecessary();
		}
	}

	if (stateChanged || ( !bCardAccepted && 
		m_playerSession.SetPlayerState(m_player, (m_mcr ? m_mcr->IsCardInserted() : false)))) // PlayerCardInserted.
	{
		m_playerSession.SetCardInSentToProgressiveProtocol(false);

		// If FloorLogix transaction processing is down, send card in to One Link immediately.
		DistributePlayerData(!m_sentinelState.IsPollerOnline() || !m_sentinelState.GetPmtUp(), true);
	}
	
	//Allows the Social Distancing UI to be able to receive some player session update 
	//in case a player cards out quickly after cardin
	CUtilities::Sleep(ONE_SECOND);

	if (bCardAccepted)
	{
		m_personalBanker->ClearInactiveAccountsFlag();
		m_playerSession.PlayerActivityOccurred();
	}

	return bCardAccepted;
}

// Returns true if a physical player card was able to be removed.
bool CInternalsFramework::PlayerCardRemoved(time_t &removalTime, time_t &utcRemovalTime, bool& bannedPlayerCardIsRemoved)
{

	//ensure that we have a player
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	bannedPlayerCardIsRemoved = m_bannedPlayerCardInserted;
	m_bannedPlayerCardInserted = false;

	m_playerSession.PlayerActivityOccurred();

	bool wasPlayerCardRemoved = EndActivePlayerSession( removalTime, utcRemovalTime, true );	

	return wasPlayerCardRemoved;
}

bool CInternalsFramework::IsPlayerVirtual( void )
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	bool bIsVirtual = false;
	
	if ( m_player != NULL )
	{
		bIsVirtual = m_player->IsVirtual();
	}

	return bIsVirtual;
}

bool CInternalsFramework::IsPlayerVirtualFromEmployeeCardIn()
{
	bool retval(false);

	{
		lock_guard<recursive_mutex> lock(m_playerCriticalSection);
		if (m_player != NULL)
		{
			if (m_player->IsVirtual())
			{
				retval = m_player->WasCardedOutOnEmployeeCardIn();
			}
		}
	}

	return retval;
}

bool CInternalsFramework::DoesPlayerExist( void )
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	bool bDoesPlayerExist = ( m_player != NULL );

	return bDoesPlayerExist;

}

// Returns true if the player was recarded.
bool CInternalsFramework::RecardPlayer(bool employeeCardRemoved)
{
	bool playerRecarded(false);

	if (employeeCardRemoved)
	{
		CCard *virtualPlayerCard(NULL);

		{
			lock_guard<recursive_mutex> lock(m_playerCriticalSection);
			if (m_player != NULL)
			{
				if (m_playerSession.GetPlayerRecardEmployeeCardOut())
				{
					if (IsPlayerVirtualFromEmployeeCardIn())
					{
						const CCard *playerCard(m_player->GetCard());
						if (playerCard != NULL)
						{
							virtualPlayerCard = (new CCard(*playerCard));
						}
					}
				}

				// Clear flag for next subsequent employee card activity.
				m_player->SetCardedOutOnEmployeeCardIn(false);
			}
		}

		if (virtualPlayerCard != NULL)
		{
			virtualPlayerCard->SetAsValidPlayer();
			HandleCardInserted(virtualPlayerCard, !m_player->IsGoodbyeEnabled());
			playerRecarded = true;
		}
	}

	return playerRecarded;
}

void CInternalsFramework::ForcePlayerCardOut(bool bannedPlayerIsDisplayed)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	if ( m_player != NULL )
	{
		time_t currentTime = CUtilities::GetCurrentTime();
		time_t utcTime = CUtilities::GetUTCTime();

		if(  m_player->IsVirtual() )
		{
			// For a force card out, end a virtual player session even if an employee session is in progress.
			EndVirtualPlayerSession( currentTime );
		}
		else
		{

			EndActivePlayerSession(currentTime, utcTime, false, bannedPlayerIsDisplayed);
		}
	}
}

/// <summary>
/// Performe timed events:
///  1) Determine if the player virtual session is to end.
///  2) Determine if the player active session is to end.
///  3) Determine if player update is to be sent.
/// Reurns true if the player was removed;
/// </summary>
/// <returns>true if the player was removed, false otherwise.</returns>
bool CInternalsFramework::PlayerTimedEvents(InternalMsgType msgType)
{
	bool bPlayerWasRemoved = false;
	bool isVirtualSession  = false;

	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	{
		if ( m_player != NULL )
		{
			time_t currentTime = CUtilities::GetCurrentTime();
			time_t utcTime = CUtilities::GetUTCTime();

			if(m_player->IsVirtual())
			{
				isVirtualSession = true;
				
				// See if the virtual session should end
				bPlayerWasRemoved = CheckIfVirtualSessionNeedsToBeRemoved(currentTime, utcTime);
			}
			else
			{
				// See if the active session should end.
				bPlayerWasRemoved = CheckIfActiveSessionNeedsToBeRemoved(msgType, currentTime, utcTime);
			}

			CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
			
			if (NULL != m_player && !isVirtualSession && m_playerUpdateManager.CheckIfPlayerUpdateNeedsToBeSent(m_player->GetLastUpdateSentTime()))
			{
				LogString(GENERAL, "Sending player update to poller");
				SendPlayerTransactionToSCF(PTT_UPDATE, GetTransactionMeters());
				m_player->ResetLastUpdateSentTime(CUtilities::GetCurrentTime());
			}

		}

		CheckIfAutoOpenCloseInServiceWindowIsNeeded();

		CheckIfMediaWindowConfigNeedsToBeSent();

		if (m_notificationBox.CheckIfNotificationTextHasChanged())
		{
			SendSubscriptionUpdateIfNecessary();
		}
	}

	return bPlayerWasRemoved;
}

void CInternalsFramework::PerformPlayerSessionTimedEvents()
{
	// Use m_playerCriticalSection to synchronize m_playerSession access and subsequent processing.
	lock_guard<recursive_mutex> csLockEgmAward(m_egmAwardCriticalSection);
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	int64_t jackpotId(0);

	PlayerTypeState oldPlayerType = m_playerSession.GetPlayerTypeEnum();

	// If we are ending a cardless session, we would need session ID and transaction sequence 
	// before the session ID is incremented by below call to PerformSessionActiveTimedEvents().
	DWORD psid = m_playerSession.GetSessionId();
	DWORD ptsn = m_playerSession.GetPlayTransactionSequenceNumber();

	bool bSessionActiveChanged = m_playerSession.PerformSessionActiveTimedEvents();
	if (bSessionActiveChanged)
	{
		if (oldPlayerType == PLAYERTYPEUNCARDED)
		{
			// If Session active changed during performing timed events and cardless session was
			// in progress, it means cardless session has timed-out. Since we already came out
			// cardless session logically while PerformSessionActiveTimedEvents, we would have
			// play session ID already incremented and that will reflect in Tx we send here.
			SendCardlessSessionTransactionsToSCF(/*sessionStart*/ false, psid, ptsn);
		}

		if (!m_egmAwardManager.IsPrizeAwardActive(jackpotId))
		{
			DistributePlayerData();
		}
	}

	if (m_playerSession.PerformSessionIdMatchTimedEvents())
	{
		// Session ID match timed out. Send card in to One Link if not already sent.
		if (!m_playerSession.GetCardInSentToProgressiveProtocol())
		{
			DistributePlayerDataToProgressiveProtocol();
		}

		if (m_progressiveCriticalText.ProcessSessionIdMatchStatus(m_playerSession.GetSessionIdMatchStatus()))
		{
			// There was something changed for the critical text, so send it to the UI
			SendProgressiveCriticalTextToUI();
		}
	}

	TiltCode tilt(m_playerSession.PerformCardedInactivityTimedEvents(m_config, IsPlayerVirtual()));
	if (tltUnknown != tilt)
	{
		GenerateTilt(tilt);
	}
}

// Caller is responsible for locking m_playerCriticalSection.
bool CInternalsFramework::CheckIfVirtualSessionNeedsToBeRemoved(time_t currentTime, time_t utcTime)
{
	bool bEndVirtual(false);

	// Don't end virtual player sessions while an employee session is in progress.
	if (m_employee1 == NULL)
	{
		if (m_config.VirtualSessionEnabled())
		{
			// Determine if a normal virtual session is to end.

			// Check to see if we don't have any credits left
			// ... and no game in progress.
			if ( !m_playerSession.HasSignificantCredits() && !m_config.GetGameInProgress())
			{
				bEndVirtual = true;
			}

			// Check if the max virtual session duration expired
			chrono::duration<int> span;
			if ( !bEndVirtual )
			{
				span = CUtilities::GetTimeSpanSeconds(m_player->GetVirtualStartTime(), currentTime);
				if ( m_config.GetVirtualSessionMaxDuration() > 0 && span.count() > m_config.GetVirtualSessionMaxDuration() )
				{
					bEndVirtual = true;
				}
			}

			// Check if the no-play virtual session duration expired
			if ( !bEndVirtual  && !m_config.GetGameInProgress() )
			{
				span = CUtilities::GetTimeSpanSeconds(m_player->GetLastPlayTime(), currentTime);
				if ( span.count() > m_config.GetVirtualSessionNoPlayTimeout() )
				{
					bEndVirtual = true;
				}
			}
		}
		else
		{
			// For ending virtual sessions, ignore m_config.MiniVirtualSessionEnabled().
			// Determine if a mini-virtual session is to end.
			if (!m_config.GetGameInProgress())
			{
				bEndVirtual = true;
			}
		}

		// If we have determined that the virtual session is to end, let's do it!
		if(bEndVirtual)
		{
			EndVirtualPlayerSession( currentTime );
		}
	}

	return bEndVirtual;
}

bool CInternalsFramework::CheckIfActiveSessionNeedsToBeRemoved(InternalMsgType msgType, time_t currentTime, time_t utcTime)
{
	bool activeSessionEnded(false);
	chrono::duration<int> span = CUtilities::GetTimeSpanSeconds(m_player->GetLastPlayTime(), currentTime);

	if ((INT_MSG_NONE != msgType) || (span.count() > m_config.GetCardTimeout() && !m_playerSession.GetGameInProgress()))
	{		
		if (INT_MSG_NONE != msgType)
		{
			if (INT_MSG_AUTO_CARDOUT_END_SESSION == msgType)
			{
				GenerateTilt(tltAutoCardOutTimerEndedByPlayer);
			}
			else if(INT_MSG_AUTO_CARDOUT_TIMED_OUT == msgType)
			{
				GenerateTilt(tltAutoCardOutTimerExpired);
			}
			m_playerSession.PlayerActivityOccurred();
		}

		if (m_pairingInformation.IsMobileCardedSession() && !DoesEmployeeExist())
		{
			McrSetBezelStyle(CARD_BAD_READ);
			CUtilities::Sleep(DISPLAY_PLAYER_OUT_EMPLOYEE_IN_PERIOD);
		}
		EndActivePlayerSession( currentTime, utcTime, false );
		activeSessionEnded = true;
	}

	return activeSessionEnded;
}

void CInternalsFramework::CheckIfAutoOpenCloseInServiceWindowIsNeeded()
{
	// Check to see if we need to auto-open/close the service window
	// Try to resolve pending media window player state, if any.
	MediaWindowPlayerState playerState;
	playerState = m_displayManager->AutoOpenCloseServiceWindowCheck(GetPbtInProgress(), m_mediaWindowPlayerStatePending);
	TellUIMediaWindowPlayerState(playerState);
}

void CInternalsFramework::CheckIfMediaWindowConfigNeedsToBeSent()
{
	// Check if MediaWindowConfig change is pending
	if ((CDisplayManager::Instance().GetDisplayPlatform() == VideoMixerPlatform) && m_pendingMWConfigChange &&
		!GetPbtInProgress() && m_mediaWindowPlayerStatePending == MAINTAIN_PLAYER_STATE_PENDING)
	{
		// check credit meter
		if ( m_games.GetGame( EGM_GAME_NUMBER )->GetMeter(mCrd) == 0 && !m_bCardInserted)
		{
			// Okay to send media window config to UI.
			CMediaWindowConfig *mwConfig = new CMediaWindowConfig( *m_mwConfig );

			m_displayManager->SetMediaWindowConfig(*m_mwConfig);
			m_displayManager->UpdateVideoMixerDisplay();

			m_pendingMWConfigChange = false;
		}
	}
}

/// <summary>
/// Sets time of the player's last play activity to the current time.
/// If hasPlayed is true then set the player instance to indicate that play occurred.
/// </summary>
/// <param name="hasPlayed">IN - true if play has played, false otherwise.</param>
void CInternalsFramework::SetPlayerLastPlay( bool hasPlayed )
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	if ( m_player != nullptr )
	{
		time_t playTime = CUtilities::GetCurrentTime();

		m_player->SetLastPlayTime( playTime );

		if (hasPlayed)
		{
			//don't set this if not needed cos it writes to NVRAM.
			if (!m_player->GetHasPlayed())
				m_player->SetHasPlayed();
		}
	}
}

void CInternalsFramework::ProcessPlayerInfo( CPlayerInfo *playerInfo)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	if ( m_player != NULL && playerInfo != NULL )
	{
		// Make sure this player info is for the same player that is in-play
		const CCard *card = m_player->GetCard();
		if ( card != NULL )
		{
			int cardIDLen;
			const byte *cardID = playerInfo->GetCardNumber( cardIDLen );

			// Compare the card numbers
			if ( card->Compare( cardID, cardIDLen ) )
			{

				DWORD gameNumber = EGM_GAME_NUMBER;
				CGame *currentGame = m_games.GetCurrentGame();
				if (currentGame != nullptr)
				{
					gameNumber = currentGame->GetGameNumber();
				}

				// Set the player info for this player and the starting multiplier, divider and countdown
				m_player->SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, m_config.GetPlayerTrackingMultiplier(), m_config.GetPlayerTrackingDivisor(), m_config.GetGlobalMGMDEnabled() && CPollerMGMDData::Instance().GetMGMDAvailable(),
																	 m_games.GetCurrentDenom(), gameNumber, m_config.GetAccountingType(), !m_config.GetAccruePointsForNCEP());
				m_personalBanker->SetGuestPlayer(m_player->IsGuest());
				playerInfo = NULL;	// So we don't delete it below.
				if (!m_player->IsVirtual())
				{
					McrSetBezelStyle(CARD_STYLE_PLAYER, m_player->GetRankId());
				}
				
				LogString(POLLER_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
				// Already locked m_playerCriticalSection.
				m_playerSession.SetPlayerState(m_player, (m_mcr ? m_mcr->IsCardInserted() : false)); // ProcessPlayerInfo.
				DistributePlayerData(); // Send player data even if other state did not change.
				
				//Send event.playerChange
				if (m_sendPlayerChangeEvent) 
				{
					SendMCPlayerDataToPollerProtocol(true, m_playerSession.GetSessionId());
					m_sendPlayerChangeEvent = false;
				}
				
				// Send the player data to the UI
				if (!m_player->IsVirtual())
				{
					SendPlayerDataToUI(INT_MSG_PLAYER_DATA);
					
					SendMCPlayerSessionInfoToPollerProtocol();
				}		
				
				bool ignoreSendingGoodBye = m_mcr->IsCardInserted() || m_pairingInformation.IsMobileCardedSession();
				// If DAP, Underage player, max card limit reached card out.
				// No need to send banned player or max card limit, if we are starting virtual session.
				if (!m_player->IsVirtual() &&
					(m_player->IsBannedFromPlay() || m_player->IsMaxCardLimitReached()))
				{
					m_bannedPlayerCardInserted = true;
					if (m_mcr->IsCardInserted())
					{
						if (m_player->IsBannedFromPlay())
						{
							// Force card out and reset bezel
							ForcePlayerCardOut(true);
						}
					}
					else
					{
						ForcePlayerCardOut(true);
						ShowRFIDCardRemoved(true);
					}

					CInternalMessage *msg = new CInternalMessage(INT_MSG_DISPLAY_BANNED_PLAYER, (DWORD)ignoreSendingGoodBye);
					int priority(QUEUE_PRIORITY_NORMAL);
					CMessagePersist* mp(m_msgPriority.PrioritizeMessage(msg, priority));
					mp->SetMainStarveTicks(DISPLAY_BANNED_PLAYER_FOR_RFID_TIMEOUT);
					msg->SetMessagePersist(mp);
					SendMessageToUI(msg, priority);
				}
			}
		}
	}

	if ( playerInfo != NULL )
		delete playerInfo;
}

void CInternalsFramework::ProcessPlayerGlobalBalance( CPlayerGlobalBalance *playerGlobalBal )
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	if( m_player != NULL && playerGlobalBal != NULL )
	{
		const CCard *card = m_player->GetCard();

		if( card != NULL )
		{
			int cardIDLen;
			byte *cardID = playerGlobalBal->GetCardNumber( cardIDLen );

			// is this the same player?
			if( card->Compare( cardID, cardIDLen ) )
			{
                m_player->SetPlayerGlobalBalance( playerGlobalBal );
				playerGlobalBal = NULL;

				SendPlayerDataToUI(INT_MSG_PLAYER_GLOBAL_BALANCE);
            }
        }
    }

	delete playerGlobalBal;
}

void CInternalsFramework::SetPlayerBonusEvents( CBonusEvents *bonusEvents )
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	if ( m_player != NULL && bonusEvents != NULL )
	{
		// Make sure these bonus events is for the same player that is in-play
		const CCard *card = m_player->GetCard();
		if ( card != NULL )
		{
			int cardIDLen;
			const byte *cardID = bonusEvents->GetCardNumber( cardIDLen );

			// Compare the card numbers
			if ( card->Compare( cardID, cardIDLen ) )
			{
				// Set the player info for this player
				m_player->SetBonusEvents( bonusEvents );
				bonusEvents = NULL;	// So we don't delete it below.
			}
		}
	}

	if ( bonusEvents != NULL )
		delete bonusEvents;
}

void CInternalsFramework::ApplyMeterChangeToPlayer( CMeters *meters, CMeters *origMeters )
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	// If there is a player, apply the meter change properly for the player.
	
	//LogString(GENERAL, "Entering ApplyMeterChangetoPlayer()" );

	// calculate change for each meter
	__int64 meterChange = 0;
	CGame *game = m_games.GetGame(EGM_GAME_NUMBER);
	if (game != NULL && origMeters)
	{
		LogString(NOPTS_LOG, "Calculated countdown meterChange %lld", (long long)meterChange );
		LogString(NOPTS_LOG, "Used OrigMeters CoinIn Value %lld", (long long)(origMeters->GetMeterByNumber(gCI) ? origMeters->GetMeterByNumber(gCI)->GetValue() : INVALID_METER_VALUE));
		LogString(NOPTS_LOG, "Used Meters CoinIn Value %lld", (long long)(meters->GetMeterByNumber(gCI) ? meters->GetMeterByNumber(gCI)->GetValue() : INVALID_METER_VALUE));
		LogString(NOPTS_LOG, "Used OrigMeters PromoPlayed Value %lld", (long long)(origMeters->GetMeterByNumber(mRstrPlayd) ? origMeters->GetMeterByNumber(mRstrPlayd)->GetValue() : INVALID_METER_VALUE));
		LogString(NOPTS_LOG, "Used Meters CoinIn PromoPlayed %lld", (long long)(meters->GetMeterByNumber(mRstrPlayd) ? meters->GetMeterByNumber(mRstrPlayd)->GetValue() : INVALID_METER_VALUE));
		if ( m_player != nullptr)
		{
			// Decrement the meter countdown.
			// If this function returns true, bonus points were awarded
			bool triggerTilt(false);
			if (m_player->DecrementCountdown(meters, origMeters, CPollerMGMDData::Instance().GetMGMDAvailable() && m_config.GetGlobalMGMDEnabled()))
			{
				// Send the one-time bonus transaction(s) to SCF. 
				SendOutstandingBonusTransactionsToSCF(false, GetTransactionMeters());
			}

			// Update player screen on UI.
			SendPlayerDataToUI(INT_MSG_PLAYER_UPDATE);

			// if the coin-in meter has changed, we need to reset the last play time for the player. Tracker 17884
			SetPlayerLastPlay();
		}
	}
}

void CInternalsFramework::SetPlayerConfig( void )
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	if ( m_player != NULL )
	{
		m_player->ConfigSettings(m_config, m_EGMConfig);
	}
}

/// <summary>
/// This gets called to process process personal banker activity.
/// It will reset session start time.
/// </summary>
void CInternalsFramework::ProcessPersonalBankerUIActivity(void)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	{
		m_personalBanker->ResetTransferSessionStartTime(CUtilities::GetTickCount());
	}
}

/// <summary>
/// This gets called to process process personal banker activity.
/// It will reset session start time.
/// </summary>
void CInternalsFramework::ResetPersonalBankerInactivityTimer( void )
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	{
		m_personalBanker->ResetTransferSessionStartTime(CUtilities::GetTickCount());
	}
}

void CInternalsFramework::ProcessGetAccountBalance()
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	if (m_player != NULL)
	{
		SendMessageToUI(new CInternalMessage(INT_MSG_DISPLAY_ACCOUNT_BALANCE, new CPlayer(*m_player)), QUEUE_PRIORITY_HIGHEST);
	}
}

void CInternalsFramework::ProcessRequestPlayerResume(PersonalBankerAction personalBankerAction)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	WORD autoUploadErrorFlags = 0;

	if (PBActionProcessRequestPlayerResumeDueToCashInactive == personalBankerAction)
	{
		LogString(PBT_LOG, "[%s] AutoUpload error : Cash inactive", __FUNCTION__);
		autoUploadErrorFlags = AutoUploadError::AUTOUPLOAD_ERROR_CASH_INACTIVE;
	}
	else if (PBActionProcessRequestPlayerResumeDueToGuestPlayer == personalBankerAction)
	{
		LogString(PBT_LOG, "[%s] AutoUpload error : Guest Player", __FUNCTION__);
		autoUploadErrorFlags = AutoUploadError::AUTOUPLOAD_ERROR_GUEST_PLAYER;
	}
	else if (PBActionCancelAutoUploadDueToCashErrorSessionRelease == personalBankerAction)
	{
		autoUploadErrorFlags = m_personalBanker->GetAutoUploadErrorFlags();

		// Send Tx90 ReleaseSession and delete transfer object
		m_personalBanker->SetToReleaseTransferSession();
		LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
		SendPersonalBankerTransaction();
	}

	if (autoUploadErrorFlags)
	{
		SetMessagePersistenceAndSendMessageToUI(new CInternalMessage(INT_MSG_DISPLAY_AUTOUPLOAD_ERROR, autoUploadErrorFlags));
	}

	SetMessagePersistenceAndSendMessageToUI(new CInternalMessage(INT_MSG_CLEAR_PBT));

	// If we get a request for a player resume, first see if we have a player
	if ( m_player != NULL )
	{
		// If the player is virtual, send the in-progress screen
		if (m_player->IsVirtual())
		{
			SendCardSessionInProgressToUI(false);
		}
	}

	// If we don't have a player, we probably were completing a pb transfer.
	// In this case, just display the attractor or a bad card read.
	else if (!m_bannedPlayerCardInserted)
	{
		if ((m_mcr->IsCardInserted() || m_playerSession.IsVirtualBadCardRead()) && (m_employee1 == NULL && m_mcr->IsCardReaderReady()))
		{
			SendBadCardReadToUI();
		}
		else
		{
			LogString(UI_LOG, "%s %d", __FUNCTION__, __LINE__);
			SendDisplayAttractorToUI();
		}
	}
}

/// <summary>
/// End an active player session.
/// </summary>
/// <param name="removalTime">IN - removal time, according to local time zone configuration.</param>
/// <param name="utcRemovalTime">IN - removal time, according to UTC configuration.</param>
/// <param name="bCanMakeVirtual">IN - true if can make virtual, false otherwise.</param>
/// <returns>true if player card removed, false otherwise.</returns>
bool CInternalsFramework::EndActivePlayerSession( time_t const removalTime, time_t const utcRemovalTime, bool bCanMakeVirtual,
	bool bannedPlayerIsDisplayed)
{
	bool playerCardRemoved = false;
	m_personalBanker->SetAutoUploadTimerExpired(); // card inserts won't be delayed for PBT Upload purposes.
	if ( m_player != NULL && !m_player->IsVirtual() )
	{
		// Indicate that the card removed was a player card.
		playerCardRemoved = true;

		// See if this player is to be made virtual.
		bool bMakeVirtual = false;
		if ( bCanMakeVirtual )
		{
			if (m_config.VirtualSessionEnabled())
			{
				// If there is still credits or a game in progress, we want to make the player virtual.
				bMakeVirtual = m_playerSession.HasSignificantCredits() || m_config.GetGameInProgress();
			}
			else if (m_config.MiniVirtualSessionEnabled())
			{
				// If virtual sessions are disabled and EGM communications are up then start a mini-virtual session if there is a game in progress.
				// But only start a mini-virtual session if the EGM is also sending game end events.
				if (m_sentinelState.IsGameOnline() && m_config.GetGameInProgress() && m_config.GetGameEndExceptions())
				{
					bMakeVirtual = true;
				}
			}
		}

		LogString(GENERAL, _T("PlayerSession: cardNumber=\"%s\" -- Ending carded player session -- makeVirtual=%s"),
			static_cast<LPCTSTR>(CCard::MakeCStringCardNumber(m_player->GetCard()).c_str()),
			static_cast<LPCTSTR>(CSmartDataLogger::GetBoolString(bMakeVirtual)));

		EndPersonalBankerActivePlayerSession();

		// Tell the player that the card was removed and whether to make virtual
		m_player->CardRemoved( removalTime, utcRemovalTime, bMakeVirtual );
		if (bMakeVirtual)
		{
			m_playerSession.SetIsVirtualSession(true);

			// Report start and type of virtual session here, so CPlayer does not have to know about the type of virtual session.
			LogString(GENERAL, _T("PlayerSession: cardNumber=\"%s\" -- Started %svirtual player session"),
				static_cast<LPCTSTR>(CCard::MakeCStringCardNumber(m_player->GetVirtualCard()).c_str()),
				m_config.VirtualSessionEnabled() ? _T("") : _T("mini-"));

			SendPlayerTransactionToSCF(PTT_VIRTUAL_SESSION_START, GetTransactionMeters());
		}
		else// If we are not becoming virtual, delete the player.
		{
			// Get a new snapshot of transaction meters
			CMeters *cardOutTransMeters(GetTransactionMeters());

			// Check to start the auto upload process if we are endabled for ncep auto uplaod and PB is enabled for this player.
			if (m_config.AutoUploadCredits() && m_EGMConfig.GetEGMProtocol() != IGT_SAS_3xx)
			{
				StartAutoUpload();
			}
			else
			{
				LogString(PBT_LOG, "EndActivePlayerSession: No Auto Upload. Not enabled for auto or EGM is SAS 3");
			}

			// Make a copy of the card-out transactions to use in cash transfer transaction.
			SendCashTransferTransactionsIfNecessary(cardOutTransMeters);

			// Make a copy of the card-out meters use in bonus transaction and send transaction
			SendOutstandingBonusTransactionsToSCF(true, new CMeters(*cardOutTransMeters));

			// Now send the card out transaction to the SCF
			SendPlayerTransactionToSCF(PTT_CARD_OUT, cardOutTransMeters);

			m_playerUpdateManager.ClearPlayerUpdateTimes();

			m_dataSubscriptionManager.UnregisterProvider(m_player);
			delete m_player;
			m_player = NULL;
			m_pairingInformation.SetMobileCardedSession(false);

			//Unlock EGM if needed
			CheckIfEgmNeedsToBeLockedOrUnlocked();
		}

		// Caller is responsible for locking m_playerCriticalSection.
		LogString(POLLER_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
		if (m_playerSession.SetPlayerState(m_player)) // EndActivePlayerSession.
		{
			DistributePlayerData();
		}

		if (!DoesEmployeeExist()) // for mobilePlayer, if employee card is already inserted 
		{	// Tell the display manager that the card was removed.
			MediaWindowPlayerState playerState;
			playerState = m_displayManager->PlayerCardRemoved(GetPbtInProgress(), 
				m_mediaWindowPlayerStatePending, bannedPlayerIsDisplayed);
			TellUIMediaWindowPlayerState(playerState);
		}
		if (m_mcr->IsCardInserted())
		{
			if (!DoesEmployeeExist()) // for mobilePlayer, if employee card is already inserted
				McrSetBezelStyle(CARD_BAD_READ);
			}
		else
		{
			ShowRFIDCardRemoved(false);
		}

		if (!DoesEmployeeExist()) // for mobilePlayer, if employee card is already inserted
		{
			m_config.ClearLastCardRead();
			m_displayManager->ClearLastCardRead();
		}

		// Set Idle ID, either card "Remove' or auto-cardout
		if (!bMakeVirtual && !m_pairingInformation.GetPairingType().compare(BEACON) && m_beaconManager->GetBeacon() && m_beaconManager->GetBeacon()->IsConnected())
		{
            if (m_beaconManager->SetEddystoneFrameData(m_pairingInformation.GetIdlePairingId(), m_pairingInformation.GetIdlePairingChecksum(), m_pairingInformation.GetMeasuredPower()))
				SendMCPairingStatusToPollerProtocol(true, BEACON_IDLE_SET);
			else
				SendMCPairingStatusToPollerProtocol(false, BEACON_IDLE_NOT_SET);
		}
	}

	PerformPersonalBankerTimedEvents();

	return playerCardRemoved;
}

// If (normal) virtual sessions are disabled and a virtual session is in progress then end the virtual session.
// Ignore m_config.MiniVirtualSessionEnabled() when ending a virtual session.
void CInternalsFramework::EndMiniVirtualSession()
{
	if (!m_config.VirtualSessionEnabled())
	{
		// Don't end mini-virtual player sessions automatically while an employee session is in progress.
		if (m_employee1 == NULL)
		{
			lock_guard<recursive_mutex> lock(m_playerCriticalSection);

			if (m_player != nullptr && m_player->IsVirtual())
			{
				EndVirtualPlayerSession(CUtilities::GetCurrentTime());

				SendDisplayPlayerRemovedToUI();
			}
		}
	}
}

void CInternalsFramework::StartAutoUpload()
{
	// If the player is enabled for PB, start the auto upload process. Even do this is it is guest player since a guest player can be enabled for PB.
	// Also if game is locked and auto transfer can't happen, the player can card back in and card out before new player
	// response is received.  If this happens, try to use original card OCR for promo transfer
	if (m_player->IsPBEnabled() || (m_player->IsGuest() && m_personalBanker->GetAutoUploadCard() != NULL))
	{
		LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
		m_personalBanker->SetToDelayAutoUpload();		
		m_personalBanker->MakeAutoUploadPending();
		if (m_player->IsPBEnabled())
		{
			LogString(PBT_LOG, "StartAutoUpload: Begin Auto Upload for card %s", static_cast<LPCTSTR>(CCard::MakeCStringCardNumber(m_player->GetCard()).c_str()));
			// if original card exists, delete it
			m_personalBanker->RebuildCard(m_player->GetCard());
			BeginPersonalBankerAutoSession(false);
		}
		// Do not rebuild the Autoupload card. Use the one that is currently there
		else
		{
			LogString(PBT_LOG, "StartAutoUpload: Begin Auto Upload upon guest player card removal. Using card %s", static_cast<LPCTSTR>(CCard::MakeCStringCardNumber(m_personalBanker->GetAutoUploadCard()).c_str()));
		}
	}
	else
	{
		LogString(PBT_LOG, "StartAutoUpload: No Auto Upload. Player is not Enabled for PB.");
	}
}

// The caller is responsible for locking m_playerCriticalSection.
void CInternalsFramework::EndVirtualPlayerSession( time_t const sessionEndTime, bool bSamePlayer )
{
	if ( m_player != nullptr && m_player->IsVirtual() )
	{
		CUtilities::Sleep(ONE_SECOND);
		LogString(GENERAL, "PlayerSession: cardNumber=\"%s\" -- Ending %svirtual player session",
			CCard::MakeCStringCardNumber(m_player->GetVirtualCard()).c_str(),
			m_config.VirtualSessionEnabled() ? "" : "mini-");

		// Indicate in the player that the virtual session ended.
		//Added CTimeSpan because if the virtual session starts and ends at the exact same time as the player session 
		//The PMT treats this card out as a duplicate transaction
		m_player->VirtualSessionEnd( sessionEndTime + VIRTUAL_SESSION_END_ADD_SECONDS );

		// Send a card out message to the SCF
		if ( m_player->GetVirtualCard() != NULL )
		{
			// Get the meters for the card out transaction. 
			CMeters *cardOutTransMeters = GetTransactionMeters();

			// Make a copy of the card-out transactions to use in cash transfer transaction.
			SendCashTransferTransactionsIfNecessary(cardOutTransMeters);

			// Make a copy of the card-out meters use in bonus transaction and send transaction
			CMeters *bonusTransMeters = new CMeters( *cardOutTransMeters );
			SendOutstandingBonusTransactionsToSCF( true, bonusTransMeters );

			// get credit meter for later
			__int64 creditMeterVal = INVALID_METER_VALUE;
			if (m_EGMConfig.GetEGMProtocol() != IGT_SAS_3xx && m_config.AutoUploadCredits() && !bSamePlayer)
			{
				CMeter *creditMeter = cardOutTransMeters->GetMeterByNumber(mCrd);
				if (creditMeter != NULL)
				{
					creditMeterVal = creditMeter->GetValue();
				}
			}

			// Send the card out transaction to SCF
			SendPlayerTransactionToSCF(PTT_VIRTUAL_SESSION_STOP, cardOutTransMeters);

			// if ncep out enabled and credits remain, remove any existing promo.
			// this should only get called if a virtual session times out.
			if (m_EGMConfig.GetEGMProtocol() != IGT_SAS_3xx && m_config.AutoUploadCredits() && !bSamePlayer && creditMeterVal > 0)
			{
				m_personalBanker->SetAutoInProcess(false);
				StartAutoUpload();
			}
		}
		
		m_dataSubscriptionManager.UnregisterProvider(m_player);
		delete m_player;
		m_player = nullptr;
		m_pairingInformation.SetMobileCardedSession(false);	
		m_playerSession.SetIsVirtualSession(false);

		// Caller is responsible for locking m_playerCriticalSection.
		LogString(POLLER_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
		if (m_playerSession.SetPlayerState(m_player)) // EndVirtualPlayerSession.
		{
			DistributePlayerData(true, false, bSamePlayer);
		}

		//Unlock EGM if needed
		CheckIfEgmNeedsToBeLockedOrUnlocked();
		
		// Set Idle ID, either card "Remove' or auto-cardout
		if (!m_pairingInformation.GetPairingType().compare(BEACON) && m_beaconManager->GetBeacon() && m_beaconManager->GetBeacon()->IsConnected())
		{
            if (m_beaconManager->SetEddystoneFrameData(m_pairingInformation.GetIdlePairingId(), m_pairingInformation.GetIdlePairingChecksum(), m_pairingInformation.GetMeasuredPower()))
            {
				SendMCPairingStatusToPollerProtocol(true, BEACON_IDLE_SET);
            }
			else
            {
				SendMCPairingStatusToPollerProtocol(false, BEACON_IDLE_NOT_SET);
            }
		}

		PerformPersonalBankerActions();
	}
}

CPlayerInfo * CInternalsFramework::MakeDefaultPlayerInfo( void )
{
	CPlayerInfo *playerInfo = new CPlayerInfo();

	playerInfo->SetMultiplierDivider( m_config.GetPlayerTrackingMultiplier(), m_config.GetPlayerTrackingDivisor() );
	playerInfo->SetPointsAndCountdown(0,playerInfo->GetDivider(), CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
//	playerInfo->SetMultiplierDivider( m_config.GetPlayerTrackingMultiplier(), NVRAM_PLAYER_TRACKING_DIVISOR_DEFAULT );

	playerInfo->AllowPBT( false );

	return playerInfo;
}

void CInternalsFramework::SendPlayerTransactionToSCF( PlayerTransactionType transactionType, CMeters *transactionMeters )
{
	// Make a copy of the player object
	CPlayer *player = new CPlayer( *m_player );

	// For a card in, clear out the current session points we are sending to the poller.
	if ( transactionType == PTT_CARD_IN )
		player->SetCurrentCountdown( 0 );

	// Determine what card we should use
	const CCard *card = player->GetCard();
	if (PTT_VIRTUAL_SESSION_START == transactionType || PTT_VIRTUAL_SESSION_STOP == transactionType)
	{
		card = m_player->GetVirtualCard();
	}

	// Get the transaction time.
	time_t *transactionTime = new time_t();
	switch( transactionType )
	{
	case PTT_VIRTUAL_SESSION_START:
		player->SetVirtualFlag(true);
		// Fall thru
	case PTT_CARD_IN:
		*transactionTime = card->GetCardInTime();
		break;
	case PTT_CARD_OUT:
	case PTT_VIRTUAL_SESSION_STOP:
		*transactionTime = card->GetCardOutTime();
		break;
	case PTT_UPDATE:
	default:
		*transactionTime = CUtilities::GetCurrentTime();
		break;
	}

	// prepare the transaction message.
	CInternalMessage *msg = PrepareTransactionMessage(
		(InternalMsgType)m_player->ConvertPlayerTxTypeToInternalMsgType(transactionType),
		m_player->IsVirtual(), 
		transactionTime, 
		transactionMeters);

	if ( msg != NULL )
	{
		// Add Player to the message.
		msg->SetData(player);

		//Fetching in sepeatare vars instead of storing directly just for logging purpose
		DWORD ptsn 		= m_playerSession.GetPlayTransactionSequenceNumber();
		int denom 		= m_playerSession.GetLastPlayedGameSessionGameDenom();
		int gameNumber	= m_playerSession.GetLastPlayedGameSessionGameNumber();
		DWORD psid		= m_playerSession.GetSessionId();

		LogString( POLLER_LOG, "%s (%d) := PlayTx SN: %u, denom: %d, gamenum: %d, psid: %d", 
			__FUNCTION__, __LINE__, ptsn, denom, gameNumber, psid);
		
		msg->SetData(gameNumber, denom);				
		msg->SetData((__int64)ptsn,(__int64)psid);
		msg->SetData((WORD)IsGlobalMGMDOptionEnabled(), (WORD)CPollerMGMDData::Instance().GetMGMDAvailable());

		SendTransactionMessageToSCF( msg, QUEUE_PRIORITY_NORMAL );
	}

	else
	{
		delete player;
		delete transactionTime;
	}

	//We don't need to increment PlayTransactionSequenceNumber to prevent badtx
	if (PTT_VIRTUAL_SESSION_START != transactionType)
	{
		m_playerSession.IncrementPlayTransactionSequenceNumber();
	}
}

void CInternalsFramework::SendCardlessSessionTransactionsToSCF(bool sessionStart, DWORD psid, DWORD ptsn) 
{
	CInternalMessage *transMsg = PrepareTransactionMessage(sessionStart ? 
		INT_MSG_TRANSACTION_CARDLESS_SESSION_START : INT_MSG_TRANSACTION_CARDLESS_SESSION_END,
		IsPlayerVirtual());

	psid = (psid == (DWORD)-1) ? m_playerSession.GetSessionId() : psid;
	ptsn = (ptsn == (DWORD)-1) ? m_playerSession.GetPlayTransactionSequenceNumber() : ptsn;
	int gameNumber = m_playerSession.GetLastPlayedGameSessionGameNumber();
	int gameDenom = m_playerSession.GetLastPlayedGameSessionGameDenom();

	transMsg->SetData(gameNumber, gameDenom);
	transMsg->SetData((__int64)ptsn, (__int64)psid);
	transMsg->SetData((WORD)IsGlobalMGMDOptionEnabled(), (WORD)CPollerMGMDData::Instance().GetMGMDAvailable());

	LogString(POLLER_LOG, "[%s] [%s] PlayTxSeq: %d PlaySessionId: %d GameNumber: %d Denom: %d "
		"IsGlobalMGMDOptionEnabled: %d" , __FUNCTION__, 
		sessionStart ? "CardlessSessionStart" : "CardlessSessionEnd",
		ptsn, psid, gameNumber, gameDenom, IsGlobalMGMDOptionEnabled());

	SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);

	// We should increment play transaction sequence number here also.
	m_playerSession.IncrementPlayTransactionSequenceNumber();
}

void CInternalsFramework::SendOutstandingBonusTransactionsToSCF( bool bIsCardOutTime, CMeters *transactionMeters )
{
	CBonusEvent *bonusEvent =  m_player->GetOutstandingBonusEvent( bIsCardOutTime );
	if ( bonusEvent != NULL )
	{
		// Build the bonus events object to send to the SCF
		CBonusEvents *bonusEventsToSend = new CBonusEvents();
		while ( bonusEvent != NULL )
		{
			if ( bonusEvent->GetBonusEventType() == ONE_TIME_BONUS )
			{
				COneTimeBonusEvent *oneTimeBonus = new COneTimeBonusEvent( *(COneTimeBonusEvent *)bonusEvent );
				bonusEventsToSend->AddEvent( oneTimeBonus );
			}
			else
			{
				CMultiplierBonusEvent *multiplierBonus = new CMultiplierBonusEvent( *(CMultiplierBonusEvent *)bonusEvent );
				bonusEventsToSend->AddEvent( multiplierBonus );
			}

			bonusEvent =  m_player->GetOutstandingBonusEvent( bIsCardOutTime );
		}

		// Make a copy of the player object
		CPlayer *player = new CPlayer( *m_player );

		CInternalMessage *msg = PrepareTransactionMessage( INT_MSG_TRANSACTION_BONUS_EVENTS, m_player->IsVirtual(), NULL, transactionMeters);

		// Add the player and the bonus events to the message.
		msg->SetData( player );
		msg->SetData( bonusEventsToSend );

		// Send the message.
		SendTransactionMessageToSCF( msg, QUEUE_PRIORITY_NORMAL );
	}
	else
	{
		delete transactionMeters;
	}
}

void CInternalsFramework::SendCashTransferTransactionsIfNecessary( CMeters *transactionMeters )
{
	if (m_config.GetCashTransferTracking())
	{
		// Make a copy of the session start meters.
		CMeters *sessionStartMeters = m_player->GetSessionStartMetersCopy();
		if (sessionStartMeters == NULL)
		{
			sessionStartMeters = GetTransactionMeters();
			LogString(ERROR_LOG, "ERROR - Expected start of session meters not present");
		}
		// Make a copy of the player object
		CPlayer *player = new CPlayer(*m_player);

		CInternalMessage *msg = PrepareTransactionMessage(INT_MSG_TRANSACTION_CASH_TRANSFER, m_player->IsVirtual(), NULL, new CMeters(*transactionMeters));

		// Add the player and the session start meters to the message.
		msg->SetData(player);
		msg->SetData(sessionStartMeters, EGM_GAME_NUMBER);

		// Send the message.
		SendTransactionMessageToSCF(msg, QUEUE_PRIORITY_NORMAL);
	}
}

/// <summary>
/// Send a previously prepared PersonalBanker transfer.
/// <para/>
/// The caller MUST NOT access (or delete) pbTransfer after calling this method.
/// </summary>
/// <param name="transactionTime">IN/OUT - transaction time.</param>
/// <param name="pbTransfer">IN/OUT - PersonalBanker transfer instance to sent.</param>
void CInternalsFramework::SendPersonalBankerTransfer(CPersonalBankerTransfer *pbTransfer, time_t *transactionTime)
{
	LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);

	CInternalMessage *msg = PrepareTransactionMessage( INT_MSG_TRANSACTION_PB, m_player == NULL ? false : m_player->IsVirtual(), transactionTime );

	// Add the trnasfer object to the transaction message
	msg->SetData( pbTransfer );

	// Send the message.
	SendTransactionMessageToSCF( msg, QUEUE_PRIORITY_NORMAL );
}

void CInternalsFramework::SendPlayerDataToUI( InternalMsgType aMsgType, bool displayCongrats )
{
	CPlayer *player(new CPlayer(*m_player));
	CInternalMessage *msg(new CInternalMessage(aMsgType, player));

	if (INT_MSG_PLAYER_DATA == msg->GetMessageType())
	{
		if (player)
		{
			if (player->IsUniverseEnabled() && player->IsGuest())
			{
				vector<string> textArray;
				textArray.push_back(player->GetFormattedBalanceRetrievalText());
				string inPlayText;
				vector<string> inPlayTextArgs;
				player->GetInPlayText(inPlayText, inPlayTextArgs);
				m_notificationBox.SetGuestWelcome(textArray, !player->IsUniverseEnabled(), inPlayText, inPlayTextArgs);
			}
			else if (!player->IsBannedFromPlay() && !player->IsMaxCardLimitReached())
			{
				vector<string> textArray;
				vector<vector<string>> argsArray;

				string welcomeStr;
				vector<string> welcomeArgs;
				player->GetFormattedWelcome(welcomeStr, welcomeArgs);
				textArray.push_back(welcomeStr);
				argsArray.push_back(welcomeArgs);

				string comp;
				vector<string> compArgs;
				//Get card in comp display
				player->GetCardInCompSingleLineString(comp, compArgs);

				if (!comp.empty())
				{
					textArray.push_back(comp);
					argsArray.push_back(compArgs);
				}

				string points;
				vector<string> pointsArgs;
				//Get Card in points
				player->GetCardInPointsSingleLineString(points, pointsArgs);
				LogString(UI_LOG, "Total Points: %s", points.c_str());
				if (!points.empty())
				{
					textArray.push_back(points);
					argsArray.push_back(pointsArgs);
				}

				// Added for tracker 26925
				string dailyPoints;
				vector<string> dailyPointsArgs;
				//Get Daily points
				player->GetFormattedDailyPointsText(dailyPoints, dailyPointsArgs);

				if (!dailyPoints.empty())
				{
					textArray.push_back(dailyPoints);
					argsArray.push_back(dailyPointsArgs);
				}

				// Added for tracker 26928
				string pointsToNextTier;
				vector<string> pointsToNextTierArgs;
				//Get Daily points
				player->GetFormattedPointsToNextTierText(pointsToNextTier, pointsToNextTierArgs);

				if (!pointsToNextTier.empty())
				{
					textArray.push_back(pointsToNextTier);
					argsArray.push_back(pointsToNextTierArgs);
				}

				if (!player->IsUniverseEnabled() || !player->IsGuest())
				{
					CInternalMessage* msgAttractor = nullptr;

					if (player->IsAnniversaryToday() && player->IsBirthdayToday())
					{
						msgAttractor = new CInternalMessage(INT_MSG_SHOW_BIRTHDAY_AND_ANNIVERSARY);
					}
					else if (player->IsAnniversaryToday())
					{
						msgAttractor = new CInternalMessage(INT_MSG_SHOW_ANNIVERSARY);
					}
					else if (player->IsBirthdayToday())
					{
						msgAttractor = new CInternalMessage(INT_MSG_SHOW_BIRTHDAY);
					}

					if (msgAttractor)
					{
						int priority(QUEUE_PRIORITY_HIGHEST);
						msgAttractor->SetMessagePersist(m_msgPriority.PrioritizeMessage(msgAttractor, priority));
						m_protocolManager->SendMessageToProtocol(IProtocolManager::UIProtocol, msgAttractor, priority);
					}
				}

				string inPlayText;
				vector<string> inPlayTextArgs;
				player->GetInPlayText(inPlayText, inPlayTextArgs);
				m_notificationBox.SetPlayerWelcome(textArray, argsArray, inPlayText, inPlayTextArgs);

				if (inPlayText.empty())
				{
					m_notificationBox.Set4FieldPlayerText(player);
				}
			}
		}
	}
	else if (INT_MSG_PLAYER_UPDATE == msg->GetMessageType())
	{
		if (player &&
			!player->IsMaxCardLimitReached() &&
			(!player->IsUniverseEnabled() || !player->IsGuest()))
		{
			string inPlayText;
			vector<string> inPlayTextArgs;
			player->GetInPlayText(inPlayText, inPlayTextArgs);
			if (inPlayText.empty())
			{
				LogString(SESSION_LOG, "%s:%d :=", __FUNCTION__, __LINE__);
				m_notificationBox.Set4FieldPlayerText(player);
			}
			else 
			{
				LogString(SESSION_LOG, "%s:%d :=", __FUNCTION__, __LINE__);
				m_notificationBox.SetInPlayText(inPlayText, inPlayTextArgs);
			}

			if (player->HasPointsBeenAdded() && m_config.DisplayCongrats() && displayCongrats)
			{
				CInternalMessage* msgCongrats(new CInternalMessage(INT_MSG_PLAY_CONGRATS));
				int priority(QUEUE_PRIORITY_HIGHEST);
				msgCongrats->SetMessagePersist(m_msgPriority.PrioritizeMessage(msgCongrats, priority));
				m_protocolManager->SendMessageToProtocol(IProtocolManager::UIProtocol, msgCongrats, priority);
			}
		}
	}
	
	SendSubscriptionUpdateIfNecessary();

	SendMessageToUI(msg, QUEUE_PRIORITY_HIGHEST);
}

void CInternalsFramework::DistributePlayerData(bool sendToProgressiveProtocol, bool attachPlayer, bool samePlayer)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	bool sessionActive;
	bool cardedPlayer;
	int sessionId;
	m_playerSession.GetSessionActiveInfo(sessionActive, cardedPlayer, sessionId);
	m_progressiveCriticalText.SetPlayerStatus(cardedPlayer);
	m_progressiveLevelManager.SetSessionActiveInfo(sessionActive, sessionId);

	if (sessionId != m_lastSessionID)
	{
		LogString(SESSION_LOG, "DistributePlayerData sessionId=%d", sessionId);

		m_lastSessionID = sessionId;

		//Don't flush the queue for the same player twice
		if (!m_playerSession.IsNamedPlayer())
		{
			m_protocolManager->FlushAllPersistedButTypeFromInboundQueue(IProtocolManager::UIProtocol, PBFlushType);
		}

		if (!samePlayer)
		{
			CInternalMessage* msg(new CInternalMessage(INT_MSG_ACTIVE_SESSION, WORD(cardedPlayer && (m_player && m_player->IsVirtual())), WORD(sessionActive)));
			bool isBannedFromPlay(m_player ? (m_player->IsBannedFromPlay() || m_player->IsMaxCardLimitReached()) : false);
			if (m_player != nullptr && (attachPlayer || isBannedFromPlay))
			{
				if (isBannedFromPlay)
				{
					m_progressiveLevelManager.DeleteCardedBonusCounterItems();
				}

				msg->SetData(new CPlayer(*GetPlayerData()));
				vector<string> textArray;
				textArray.push_back(m_player->GetGoodCardText(false));
				textArray.push_back(m_player->GetFormattedBalanceRetrievalText());
				m_notificationBox.SetRFID(m_player->IsGoodbyeEnabled());

				string inPlayText;
				vector<string> inPlayTextArgs;
				m_player->GetInPlayText(inPlayText, inPlayTextArgs);
				if (m_notificationBox.SetGuestWelcome(textArray, !m_player->IsUniverseEnabled(), inPlayText, inPlayTextArgs))
				{
					SendSubscriptionUpdateIfNecessary();
				}
				if ((!m_player->IsUniverseEnabled() || !m_player->IsGuest()) && inPlayText.empty())
				{
					if (m_notificationBox.Set4FieldPlayerText(m_player))
					{
						SendSubscriptionUpdateIfNecessary();
					}
				}
			}
			else if (!cardedPlayer)
			{
				if (m_playerSession.GetPlayerTypeEnum() == PlayerTypeState::PLAYERTYPEUNCARDED)
				{
					LogString(UI_LOG, "%s %d : Setting notification state as uncarded.", __FUNCTION__, __LINE__);
					m_notificationBox.SetNotificationState(NotificationState::UNCARDED);
				}
				else if (m_player == nullptr || !m_player->IsVirtual())
				{
					// Since it is not carded player & it is not a virtual session, set notification state as IDLE.
					LogString(UI_LOG, "%s %d : Setting notification state as idle.", __FUNCTION__, __LINE__);
					m_notificationBox.SetNotificationState(NotificationState::IDLE);
				}
				m_progressiveLevelManager.DeleteCardedBonusCounterItems();
			}

			int priority(QUEUE_PRIORITY_HIGHEST);
			msg->SetMessagePersist(m_msgPriority.PrioritizeMessage(msg, priority));
			SendMessageToUI(msg, priority);
		}
		
		// update to FP, only when session changes
		if (cardedPlayer)
		{
			// Set flag and send when player info is received
			m_sendPlayerChangeEvent = true;
		}
		else
			SendMCPlayerDataToPollerProtocol(cardedPlayer, sessionId);
	}
	else
	{
		LogString(SESSION_LOG, "DistributePlayerData sessionId=%d did not change.", sessionId);
	}

	if (sendToProgressiveProtocol)
	{
		SendPlayerDataToProgressiveProtocol(sessionActive, cardedPlayer, sessionId);
	}

	SendSubscriptionUpdateIfNecessary();
}

void CInternalsFramework::DistributePlayerDataToProgressiveProtocol()
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	bool sessionActive;
	bool cardedPlayer;
	int sessionId;
	m_playerSession.GetSessionActiveInfo(sessionActive, cardedPlayer, sessionId);
	SendPlayerDataToProgressiveProtocol(sessionActive, cardedPlayer, sessionId);
}

void CInternalsFramework::SendPlayerDataToProgressiveProtocol(bool sessionActive, bool cardedPlayer, int sessionId)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	// Send CPlayer data, SessionActive flag, and sessionId at the same time,
	// so these changes are synchronized when received by the progressive protocol.
	CPlayer *player(cardedPlayer && m_player != NULL ? new CPlayer(*m_player) : NULL);
	CInternalMessage *msg = new CInternalMessage(INT_MSG_PLAYER_DATA, player);
	msg->SetData((WORD)sessionActive);
	msg->SetData((DWORD)sessionId); // Pass as DWORD because the size matches.

	SendMessageToProgressive(msg, QUEUE_PRIORITY_NORMAL);

	if (cardedPlayer)
	{
		m_playerSession.SetCardInSentToProgressiveProtocol(true);
	}
}

void CInternalsFramework::SendMCPlayerDataToPollerProtocol(bool cardedPlayer, int sessionId)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	if (m_config.MobileConnectCommunicationEnabled())
	{
		// Send CPlayer data and sessionId at the same time,
		// so these changes are synchronized when received by the poller protocol.
		CPlayer *player(cardedPlayer && m_player != NULL ? new CPlayer(*m_player) : NULL);
		CPlayerChangeEvent *evntPoller =  new  CPlayerChangeEvent(player, sessionId, m_playerSession.GetPrevPlayerType(), m_playerSession.GetPlayerType());
		CInternalMessage *pollerMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_MOBILE_CONNECT_PROXIMITY, IsPlayerVirtual());
		pollerMsg->SetData( (WORD) ToPlayerDeviceService::EventPlayerChange );
		pollerMsg->SetData(evntPoller);

		SendTransactionMessageToSCF( pollerMsg, QUEUE_PRIORITY_NORMAL );

		//Set previous player state as current
		m_playerSession.SetPrevPlayerType(m_playerSession.GetPlayerType());
	}
}

void CInternalsFramework::SendMCPlayerSessionInfoToPollerProtocol(PlayerSessionChangeEventType evtType, WORD cashoutErrorFlags, std::string sasErrorText)
{
	LogString(POLLER_LOG, "%s:%d", __FUNCTION__, __LINE__);
	if (m_player && m_config.MobileConnectCommunicationEnabled())
	{
		lock_guard<recursive_mutex> lock(m_playerCriticalSection);

		if (m_player && m_player->IsMobilePlayer()) 
		{
			CPlayer *player(new CPlayer(*m_player));
			DWORD jackpotAmount = 0;
			bool isEgmLocked = false;

			if (m_jackpot != nullptr)
			{
				// If current jackpot got locked-up in handpay mode, EGM will be in locked  
				// state if jackpot has not been reset so far - manually or via UTJ flow
				if ((m_jackpot->GetJackpotTransactionType() == JTRANS_COMPOSITE) &&
					!m_jackpot->GetHandPayReset())
				{
					isEgmLocked = true;
				}

				if (evtType == SESSION_CHANGE_EVENT_JACKPOT_LOCKUP)
				{
					jackpotAmount = m_jackpot->GetJackpotAmount();
				}
			}

			// Note: We are mapping isPbtEnabled flag of Floor message right now just to tell if EGM 
			// is locked-up or notso that mobile app can use it to enable/disable PBT buttons. 
			// We may extend this functionality later-on to suit our need
			CPlayerSessionChangeEvent *evnt = new  CPlayerSessionChangeEvent(player, evtType, m_playerSession.GetSessionId(), m_playerSession.GetCreditsInPennies(), 
				jackpotAmount, !isEgmLocked, cashoutErrorFlags, sasErrorText);

			CInternalMessage *pollerMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_MOBILE_CONNECT_PROXIMITY, IsPlayerVirtual());
			pollerMsg->SetData((WORD) ToPlayerDeviceService::EventPlayerSessionChange);
			pollerMsg->SetData(evnt);

			SendTransactionMessageToSCF(pollerMsg, QUEUE_PRIORITY_NORMAL);
		}
	}
}

void CInternalsFramework::SendPersonalBankerMessageToUI( CInternalMessage *msg, bool lowerPriority, int priority)
{
	if (priority)
	{
		SendMessageToUI(msg, priority);
	}
	else
	{
		SendMessageToUI(msg, lowerPriority ? QUEUE_PRIORITY_ABOVE_NORMAL : QUEUE_PRIORITY_HIGHEST);
	}
}

/// <summary>
/// Returns true if player is banned from play, returns false otherwise.
/// </summary>
bool CInternalsFramework::IsPlayerBannedFromPlay()
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	
	bool bannedFromPlay = false;

	{
		if(m_player != NULL)
		{
			bannedFromPlay = m_player->IsBannedFromPlay();                            
		}
	}

	return bannedFromPlay;
}


CPlayer* CInternalsFramework::GetPlayerData()
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	return m_player;
}

PackageOrientation CInternalsFramework::GetPackageOrientation() const
{
	return CDisplayManager::Instance().GetPackageOrientation(m_mwConfig->GetContentWindowOrientation());
}
