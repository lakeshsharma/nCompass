#include "stdafx.h"


#include "InternalsFramework.h"
#include "Logging/LogString.h"
#include "Sentinel.h"
#include "Ticket.h"
#include "UI/PersonalBankerLog.h"
#include "MessagePersist.h"
#include "GameEndData.h"
#include "Meters.h"
#include "PollerProtocol/PollerMGMDData.h"
#include "SASProtocol/SASProtocol.h"

using namespace std;

// Time we should delay accepting next cashout to host session request, 10 secs.
const DWORD MIN_NEXT_CASHOUT_TO_HOST_SESSION_DELAY_MSECS = 10000;

void CInternalsFramework::ProcessMessageFromGCF( CInternalMessage *msg )
{
	bool bDeleteMsg = true;

	switch ( msg->GetMessageType() )
	{
	case INT_MSG_SET_METERS:
		ProcessSetMetersFromGCF( msg );
		break;
	case INT_MSG_SET_ENABLED_GAMES:
	{
		vector<DWORD>* vectorOfEnabledGames(msg->GetVectorOfWords());
		m_games.SetNumberOfEnabledGames(vectorOfEnabledGames ? vectorOfEnabledGames->size() : 0);
		SendMessageToSCF(msg, QUEUE_PRIORITY_HIGHEST);
		bDeleteMsg = false;
		break;
	}
	case INT_MSG_SET_GAME_LIST:
		ProcessSetGameListFromGCF( msg );
		break;
	case INT_MSG_SNAPSHOT_PROGRESS:
		ProcessSnapshotProgressFromGCF( msg );
		break;
	case INT_MSG_SET_DENOM:
		ProcessSetDenomFromGCF( msg );
		break;
	case INT_MSG_GAME_COMM_STATUS:
		ProcessGameCommStatusFromGCF(msg);
		break;
	case INT_MSG_GAME_COMM_ERROR:
		ProcessGameCommErrorFromGCF( msg );
		break;
	case INT_MSG_NOTIFY_GAME_START:
		ProcessGameStartFromGCF(msg);
		break;
	case INT_MSG_NOTIFY_GAME_END:
		ProcessGameEndFromGCF( msg );
		break;
	case INT_MSG_GENERAL_POLL_EXCEPTION:
		ProcessGeneralPollExceptionFromGCF(msg);
		bDeleteMsg = false;
		break;
	case INT_MSG_DOOR_OPEN:
		ProcessDoorOpenFromGCF( msg );
		break;
	case INT_MSG_DOOR_CLOSED:
		ProcessDoorClosedFromGCF( msg );
		break;
	case INT_MSG_SEND_TILT:
		ProcessTilt(msg);
		break;
	case INT_MSG_SEND_TICKET:
		ProcessSendTicketFromGCF( msg );
		break;
	case INT_MSG_TICKET_INSERTED:
		ProcessTicketInserted();
		break;
	case INT_MSG_TICKET_STATUS:
		ProcessTicketStatusFromGCF( msg );
		break;
	case INT_MSG_PB_TRANSFER:
		ProcessTransferCompleteFromGCF( msg );
		break;
	case INT_MSG_EFT_SEED:
		ProcessEFTSeedFromGCF( msg );
		break;
	case INT_MSG_END_CAPTURING_METERS:
		ProcessSnapshotEndFromGCF( msg );
		break;
	case INT_MSG_HANDPAY_RESET:
		ProcessHandPayReset(msg);
		break;
	case INT_MSG_JACKPOT_HIT:
	case INT_MSG_PROGRESSIVE_HOPPER_PAID_HIT:
	case INT_MSG_PROGRESSIVE_WIN_HIT:
	case INT_MSG_PROGRESSIVE_WIN:
		JackpotHit(msg);
		break;
	case INT_MSG_JACKPOT_INFO:
	case INT_MSG_PROGRESSIVE_WIN_INFO:
	case INT_MSG_PROGRESSIVE_HOPPER_PAID_INFO:
		ProcessJackpotInfoFromGCF( msg );
		break;
	case INT_MSG_METER_CHANGE_REQUEST:
		ProcessMeterChangeRequestFromGCF( msg );
		break;
	case INT_MSG_METER_CHANGE_ALLOW_SENT:
		ProcessMeterChangeAllowSentFromGCF( msg );
		break;
	case INT_MSG_METER_CHANGE_COMPLETE:
		// Just ignore this message. We no longer wait for the EGM to tell us that the download is complete (tracker 17428)
		//ProcessMeterChangeCompleteFromGCF( msg );
		break;
	case INT_MSG_METER_CHANGE_CANCEL:
		ProcessMeterChangeCancelFromGCF();
		break;
	case INT_MSG_EGM_PROTOCOL_VERSION:
		ProcessEgmVersionFromGCF( msg );
		break;
	case INT_MSG_EGM_SUPPORTS_LP_7B:
		ProcessEgmSupportsLP7B(msg);
		break;
	case INT_MSG_CHANGE_LIGHT_ON:
		ProcessChangeLightOnFromGCF();
		break;
	case INT_MSG_CHANGE_LIGHT_OFF:
		ProcessChangeLightOffFromGCF();
		break;
	case INT_MSG_START_CAPTURING_METERS:
		ProcessStartCapturingMetersFromCGF( msg );
		break;
	case INT_MSG_BILL_METERS_COMPLETE:
		ProcessBillMetersCompleteFromGCF();
		break;
	case INT_MSG_SET_PB_BALANCES:
		ProcessPBBalancesFromGCF( msg );
		break;
	case INT_MSG_GCF_STARTED:
		ProcessGCFStarted();
		break;
	case INT_MSG_FINISHED_GAMESLIST_FIRSTPASS:
		ProcessFinishedFirstPass(msg);
		break;
	case INT_MSG_UPDATE_SAS_INFO:
		ProcessUpdateSasInfo(msg);
		break;
	case INT_MSG_EGM_AWARD:
		ProcessEgmAwardMessage(msg, mqGCF);
		break;
	case INT_MSG_CUMULATIVE_PROGRESSIVE_WIN:
		ProcessCumulativeProgressiveWin(msg, mqGCF);
		break;
	case INT_MSG_UPDATE_BONUS_TYPES:
		ProcessBonusCapabilities(msg);
		break;
	case INT_MSG_SET_CURRENT_GAME_DENOM:
		ProcessCurrentDenom(msg);
		break;
	case INT_MSG_RESET_JACKPOT:
		ProcessJackpotResetResult(msg);
		break;
	case INT_MSG_INC_BILL_METER:
		ProcessIncBillMeter(msg);
		break;
	case INT_MSG_PROBE_PBT_RESULT:
	  bDeleteMsg = ProcessProbePBTFromGCF(msg);
	  break;
    case INT_MSG_INCREMENT_GAME_SESSION_ID_IF_REQUIRED:
		m_playerSession.IncrementGameSessionIdIfRequired(m_games);
		if (m_playerSession.IsGameSessionIdIncremented())
		{
			SendSubgameSelectToPoller();
		}
		break;
	case INT_MSG_PRIZE_LOCK_CLEAR:
	{
		if (msg->GetWordData()/*game unlocked*/)
		{
			int64_t jackpotId(msg->GetInt64Data());
			int64_t clearedByOneLink(msg->GetInt64Data(true));
			ProcessPrizeLockClear(jackpotId, (bool)clearedByOneLink);
		}

		break;
	}
	case INT_MSG_EGM_LOCK_UNLOCK_STATUS:
	{
		CInternalMessage *transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_GENERIC, IsPlayerVirtual());
		transMsg->SetData((WORD)msg->GetWordData());
		SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);
		break;
	}
	case INT_MSG_FINISHED_OFFLINE_PASS:
		ProcessFinishedOfflinePlayPass(msg);
		break;

	case INT_MSG_CASHOUT_TO_HOST:
		if (msg->GetWordData() /* process cashout to host*/)
		{
			bDeleteMsg = ProcessCashoutToHostFromGCF(msg);
		}
		break;

	case INT_MSG_CASHOUT_TO_HOST_STATUS:
		m_sentinelState.SetEGMCashoutToHostStatus((byte)msg->GetWordData());
		break;

	case INT_MSG_CANCEL_PENDING_AUTO_UPLOAD:
		m_personalBanker->CancelPendingAutoUploadIfNecessary(msg->GetWordData());
		break;

	default:
		break;
	}

	if ( bDeleteMsg )
		delete msg;
}

void CInternalsFramework::ProcessChangeLightOnFromGCF()
{
	CInternalMessage * transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_CHANGE_LIGHT_ON, IsPlayerVirtual());
	SendMessageToSCF( transMsg, QUEUE_PRIORITY_NORMAL );
}

void CInternalsFramework::ProcessChangeLightOffFromGCF()
{
	CInternalMessage * transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_CHANGE_LIGHT_OFF, IsPlayerVirtual());
	SendMessageToSCF( transMsg, QUEUE_PRIORITY_NORMAL );
}

void CInternalsFramework::ProcessSetMetersFromGCF( CInternalMessage *msg )
{	
	// This is the source meters object from the message
	WORD gameNumber;
	CMeters *meters = msg->GetMetersData( gameNumber );
	u_SetMeterFlags setMeterFlags;
	setMeterFlags.uValue = msg->GetWordData(true);
	ProcessSetMeters( meters, gameNumber, setMeterFlags.flags.meterSnapshop, setMeterFlags.flags.sendMeterUpdate304);

	if (gameNumber == EGM_GAME_NUMBER)
	{
		ProcessEgmAwardMessage(msg, mqGCF);
	}
}

// CInternalsFramework::TellDisplayManagerCreditMeterNonZero
// This can be called multiple times for a single credit meter transition from zero to non-zero.
// Tell CDisplayManager that the credit meter has become non-zero.
// Tell UI (CUIProtocol and CMediaWindowManager) if media window opened or closed.
void CInternalsFramework::TellDisplayManagerCreditMeterNonZero()
{
	MediaWindowPlayerState playerState;
	playerState = m_displayManager->GameCreditsBecameNonZero(GetPbtInProgress(), m_mediaWindowPlayerStatePending);
	TellUIMediaWindowPlayerState(playerState);
}

// Called when vouchers and money has been accepted, but meters have not been incremented.
void CInternalsFramework::CheckMoneyInServiceWindow()
{
	CGame* game = m_games.GetEGM();
	if (game != NULL)
	{
		if (m_playerSession.HasSignificantCredits())
		{
			// tell the displayManager that the credit meter now has a value
			TellDisplayManagerCreditMeterNonZero();
		}
	}
}

void CInternalsFramework::ProcessSetMeters( CMeters *meters, WORD gameNumber, bool isSnapshot, bool sendMeterUpdate304 )
{
	bool bMetersChanged = false;
	bool bPointCalcMetersChanged = false; // keep track if any meters involved in No Point For Promo calculations have changed
	CMeters *origMeters = NULL;
	bool needSendMCPlayerSessionInfoToPollerProtocol = false;  // Keep track of when we need to player session updates for mobile connect

	// TBD - This function is too long and needs to be split into smaller functions

	// It is possible that the coinIn meter will come in following a "game end" and get processed before the high priority meters get processed.
	// If this happens, the new coinIn will get processed before all of the other meters that go into NPFP calculations.
	// When the High Priority meters do show up, the new coinIn will have already been processed and won't look like it has changed causing the
	// points that should be awarded to be missed.  Need to keep a flag of an unprocessed coinIn change and what the original coinIn value is.
	// if a coinin change is pending, the NPFP calcs need to use the original coinIn value instead of the value passed in "meters"

	// This is the target game that will have its meters set.
	CGame *game = m_games.GetGame( gameNumber );

	// Need to track when m_pendingCoinIn is set so that points are not calculated on same update
	bool thisUpdateSetPending = false;
	bool metersInitForNewSubgame = false;

	if ( game != NULL && meters != NULL )
	{
		if ((gameNumber != EGM_GAME_NUMBER) &&
			(game->GetMeters()->GetMeterCount() == 0))
		{
			// This is the call to set meters of a freshly discovered subgame
			// Enabled games usually won't fall here since they have game 
			// object with meters already updated from NVRAM.
			metersInitForNewSubgame = true;
		}

		// Go thru all of the source meters
		for ( int mi = 0; mi < meters->GetMeterCount(); mi++ )
		{
			// Get the source meter.
			CMeter *meter = meters->GetMeterByIndex(mi);

			// Set the target game with the source meter
			if ( meter != NULL && meter->GetValue() != INVALID_METER_VALUE )
			{
				// See if the meter value we received is different from the one in memory.
				long lDifference = (long)game->CompareMeter( meter );
				if ( lDifference != 0 )
				{
					if (!origMeters)
					{
						// keep temporary copy of game meters for no points for promo calculations later.
						// temporary copy will be made only once per meters object
						CMeters *gameMeters = game->GetMeters();
						origMeters = new CMeters(*gameMeters);
						//((CSentinel3App *)AfxGetApp())->LogString(L"GENERAL", L"Start meter update" );
					}
					// Set the new meter value and mark it as changed.
					byte meterNumber = meter->GetMeterNumber();
					game->SetMeter( meterNumber, meter->GetValue() );
					bMetersChanged = true;

					// The player object will process some of the meter changes.
					// Don't do it here if No Points for Promo is active
					LogString(GENERAL, "[%s] [GameNumber: %d] Setting meter #: %d, value: %lld, difference: %ld",
						__FUNCTION__, gameNumber, meterNumber, meter->GetValue(), lDifference);

					// Now do processing unique to the meter.
					switch ( meterNumber )
					{
					case mTVal:
					{
						ProcessVoucherOut(lDifference);
						break;
					}
					case mCrd:
						ProcessSetCreditMeter(*meter);
						needSendMCPlayerSessionInfoToPollerProtocol = true;
						break;
					case mSelG:
						SetCurrentGame( (WORD)meter->GetValue() );
						break;
					case mD1:	//{Ones}
						if ( lDifference > 0 )
						{
							ProcessBillAccepted( 1, lDifference, meter->GetValue(), game, meters->GetMeterByNumber(mBD));
						}
						break;
					case mD2:	//{Twos}
						if ( lDifference > 0 )
						{
							ProcessBillAccepted( 2, lDifference, meter->GetValue(), game, meters->GetMeterByNumber(mBD));
						}
						break;
					case mD5:	//{Fives}
						if ( lDifference > 0 )
						{
							ProcessBillAccepted( 5, lDifference, meter->GetValue(), game, meters->GetMeterByNumber(mBD));
						}
						break;
					case mD10:	//{Tens}
						if ( lDifference > 0 )
						{
							ProcessBillAccepted( 10, lDifference, meter->GetValue(), game, meters->GetMeterByNumber(mBD));
						}
						break;
					case mD20:	//{Twenties}
						if ( lDifference > 0 )
						{
							ProcessBillAccepted( 20, lDifference, meter->GetValue(), game, meters->GetMeterByNumber(mBD));
						}
						break;
					case mD50:	//{Fifties}
						if ( lDifference > 0 )
						{
							ProcessBillAccepted( 50, lDifference, meter->GetValue(), game, meters->GetMeterByNumber(mBD));
						}
						break;
					case mD100:	//{Hundreds}
						if ( lDifference > 0 )
						{
							ProcessBillAccepted( 100, lDifference, meter->GetValue(), game, meters->GetMeterByNumber(mBD));
						}
						break;
					case mD200: //{Two Hundreds}
						if ( lDifference > 0 )
						{
							ProcessBillAccepted( 200, lDifference, meter->GetValue(), game, meters->GetMeterByNumber(mBD));
						}
						break;
					case mD500: //{Five Hundreds}
						if ( lDifference > 0 )
						{
							ProcessBillAccepted( 500, lDifference, meter->GetValue(), game, meters->GetMeterByNumber(mBD));
						}
						break;
					case mD1000: //{One Thousands}
						if ( lDifference > 0 )
						{
							ProcessBillAccepted( 1000, lDifference, meter->GetValue(), game, meters->GetMeterByNumber(mBD));
						}
						break;
					case gCI:
					case mCTktPV:
					case mPbAR2G:
					case mCPromoOutVal:
					case mPbAR2H:
					case mNcepCredits:
					case mRstrPlayd:
						// Add gCO because all pointcalcs occur where it used to be only NPFP CoinIn pointcalcs
					case gCO:
						if (lDifference > 0)
						{
							if (meterNumber == gCI)
							{
								m_excessiveVoucherOut.ProcessCoinIn(lDifference);
							}
							if (meterNumber == gCO)
							{
								m_excessiveVoucherOut.ProcessCoinOut(lDifference);
							}
						}

						if (gameNumber == EGM_GAME_NUMBER)
						{
							m_pendingCoinIn = true;
							thisUpdateSetPending = true;
						}

						// if there isn't already a copy of original meters, get one.
						if (!m_origMeters)
						{
							m_origMeters = new CMeters(*origMeters);
						}

						if (mCTktPV == meterNumber)
						{
							CheckMoneyInServiceWindow();
						}

						break;
					case mCoupV:
					case mCTktCV:
					case mTValueCI:
					case mTCi:
						CheckMoneyInServiceWindow();
						break;
					case SUBGAME_METER_COIN_IN:
						if (lDifference > 0)
						{
							// Check if necessary to send "offline" coin in to OneLink
							if (gameNumber != EGM_GAME_NUMBER)
							{
								if (!m_finishedOfflineCheckForSubgames)
								{
									// Filter out "first time" coin in
									if (lDifference != meter->GetValue())
									{
										// Send offline CoinIn5
										SendOfflineCoinInToProgressive(new CGame(*game), lDifference);
									}
								}
							}
						}


						break;
					}

					if (lDifference < 0 && (meterNumber == gCI || meterNumber == gCO))
					{
						GenerateTilt(tltNegativeCoinInOutMeterDelta);
					}

					// Tracker #20839 -- mark the meter as changed so it will later be sent to scf
					meter->SetMeterChanged();
				}
				else
				{
					// check if pending coinIn needs to be processed
					if (m_pendingCoinIn && gameNumber == EGM_GAME_NUMBER)
					{
						switch ( meter->GetMeterNumber() )
						{
						case gCI:
						case mCTktPV:
						case mPbAR2G:
						case mCPromoOutVal:
						case mPbAR2H:
						case mNcepCredits:
						case mRstrPlayd:
						case gCO:
							bPointCalcMetersChanged = true;
							if (!origMeters)
							{
								// keep temporary copy of game meters for no points for promo calculations later.
								// temporary copy will be made only once per meters object
								CMeters* gameMeters = game->GetMeters();
								origMeters = new CMeters(*gameMeters);
							}
							break;
						}
					}
				}
			}
		}

		if (metersInitForNewSubgame)
		{
			if (!IsNFDPoller() || game->IsEnabled() ||
				(game->GetMeter(SUBGAME_METER_COIN_IN) > 0))
			{
				LogString(GAMES, "[%s]: Sending 305 Subgame meters for newly discovered subgame [%u].",
					__FUNCTION__, game->GetGameNumber());
				SendGameMetersToSCF(game);
			}
		}
	}

	// If any of these meters have changed, send the meters to SCF
	/// NOTE: Don't do this if we are in a snapshot.
	LogString(NOPTS_LOG, "bMetersChanged = %d", bMetersChanged);
	if ((bMetersChanged || bPointCalcMetersChanged) && !m_sentinelState.IsMeterCaptureInProgress())
	{
		// Calculate No Points For Promo meters if necessary
		bool bUpdatePlayerCountdownRequired = false; // When SAS's GetTicketMeters() GetPbtMeters() run, meters object won't contain all required meters so skip update.
		LogString(NOPTS_LOG, "bPointCalcMetersChanged = %d", bPointCalcMetersChanged);
		if (bPointCalcMetersChanged && !thisUpdateSetPending && gameNumber == EGM_GAME_NUMBER)
		{
			//We skipped updating SpeedMedia regarding session points because of
			//a pending coin in and CalculateNcepMeters will set m_pendingCoinIn to false.
			//We need to cache original value to update SpeedMedia properly.
			bool oldPendingCoinIn(m_pendingCoinIn);

			CMeter *coMeter = meters->GetMeterByNumber(gCO);
			bUpdatePlayerCountdownRequired = CalculateNcepMeters(meters, game, origMeters) && coMeter != nullptr && coMeter->GetValue() != INVALID_METER_VALUE;
			LogString(NOPTS_LOG, "bUpdatePlayerCountdownRequired = %d", bUpdatePlayerCountdownRequired);
			LogString(NOPTS_LOG, "!m_config.GetAccruePointsForNCEP() = %d", !m_config.GetAccruePointsForNCEP());
			LogString(NOPTS_LOG, "m_config.GetAccountingType() = %d", m_config.GetAccountingType());

			// Cause current countdown to be updated
			if (bUpdatePlayerCountdownRequired)
			{
				ApplyMeterChangeToPlayer( meters, m_origMeters );
				if (m_origMeters)
				{
					delete m_origMeters;
					m_origMeters = NULL;
				}

				needSendMCPlayerSessionInfoToPollerProtocol = true;

				m_pendingCoinIn = false;

			}
		}

		if (bMetersChanged && (gameNumber != EGM_GAME_NUMBER))
		{
			m_currentSubgameMetersChanged = true;
		}

		// Eliminate sending incremental meter changes to poller for NFD pollers.
		// We may see gaps in subgame meter values until next 304 is sent within 
		// 5 mins or game/denom change is done.
		if (bMetersChanged)
		{			
			if (IsNFDPoller())
			{
				if (sendMeterUpdate304)
				{
					m_games.ForceEgmMetersUpdate();
				}
			}
			else
			{
				// The SCF needs a games object with a single game that contains the meters.
				CGames *scfGames = new CGames();
				CGame *scfGame = new CGame( *game );

				// For the egm, we don't want to send all the meters.
				if ( game->IsEGM() )
				{
					CMeters *egmMeters = scfGame->GetMeters();
					CMeters *changedMeters = meters->DuplicateChangedMeters(m_config.GetGameInProgress());
					*egmMeters = *changedMeters;
					delete changedMeters;
				}

				scfGames->AddGame( scfGame );
				//don't send meters to scf if there is no meter change
				if (scfGame->GetMeters()->GetMeterCount() != 0)
				{
					LogString(GAMES, "[%s]: Sending Incremental 304/305 meters from ProcessSetMeters [Game: %u]...",
						__FUNCTION__, scfGame->GetGameNumber());
					CInternalMessage *scfMsg = new CInternalMessage(INT_MSG_SET_METERS, scfGames);

					// Send the meters update message to the SCF
					SendMessageToSCF(scfMsg, QUEUE_PRIORITY_NORMAL);
				}
				else
				{
					delete scfGames;
				}
			}
		}
	}
	else
	{
		// Send the snapshot transaction to the SCF
		if ( game != NULL && m_sentinelState.IsMeterCaptureInProgress() && isSnapshot )
		{
			if (game->IsEGM())
			{
				SendSnapshotTransactionToSCF( gameNumber );
			}
			else
			{
				//only send subgame snapshot if thee is a non-zero meter
				bool nonZeroFound = false;
				for (int i = 0; i < game->GetMeters()->GetMeterCount(); i++)
				{
					if (game->IsEnabled() || game->GetMeter((byte)i) != 0)
					{
						nonZeroFound = true;
						break;
					}
				}
				if (nonZeroFound)
				{
					SendSnapshotTransactionToSCF( gameNumber );
				}
			}
		}

		// If we didn't find this game then our games must be out of sync.
		// Send a request to get the game list from the GCF.
		if (game == NULL && !m_sentinelState.IsMeterCaptureInProgress())
		{
			LogString(GAMES, "Sending INT_MSG_GET_GAME_INFO gameNumber=%u", gameNumber);
			SendMessageToGCF(new CInternalMessage(INT_MSG_GET_GAME_INFO, (WORD)gameNumber), QUEUE_PRIORITY_ABOVE_NORMAL);
		}
	}
	
    if (needSendMCPlayerSessionInfoToPollerProtocol) 
    {
        SendMCPlayerSessionInfoToPollerProtocol();
    }

	// delete temporary meters if present
	if (origMeters)
		delete origMeters;

}

void CInternalsFramework::ProcessSetCreditMeter(const CMeter &meter)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	bool sessionActiveInfoChanged(false), creditsIncreased(false);
	__int64 meterValueInt64(meter.GetValue());
	DWORD meterValue(meterValueInt64 == INVALID_METER_VALUE ? 0 : DWORD(meterValueInt64));
	bool bChanged = m_playerSession.SetCreditsInPennies(meterValue * m_config.GetDenomination(), sessionActiveInfoChanged, creditsIncreased);


	if (creditsIncreased)
	{
		LogString(SESSION_LOG, "ProcessSetCreditMeter credits increased");
		m_playerSession.PlayerActivityOccurred();
	}

	if (bChanged)
	{
		if (m_playerSession.HasSignificantCredits())
		{
			// tell the displayManager that the credit meter now has a value
			TellDisplayManagerCreditMeterNonZero();
		}
		else
		{
			// tell the display manager that the credit meter was zero-ed
			m_displayManager->GameCreditsBecameZero(m_mediaWindowPlayerStatePending);
		}
	}

	if (sessionActiveInfoChanged)
	{
		// If SetCreditsInPennies() returned sessionActiveInfoChanged=true, it means session has changed to active due to 
		// addition of credits to THE EGM and this can only happen if its not due to carded or mobile player, so its the 
		// beginning of cardless session
		SendCardlessSessionTransactionsToSCF(/*sessionStart*/ true);
		DistributePlayerData(true, false, false);
	}
}

bool CInternalsFramework::CalculateNcepMeters(CMeters *meters, CGame *game, CMeters *origMeters)
{
	__int64 nonNcepPlayed = INVALID_METER_VALUE;
	bool bUpdatePlayerCountdownRequired = false;

	if (m_EGMConfig.GetEGMProtocol() != IGT_SAS_3xx)
	{
		LogString(NOPTS_LOG, "Calculating NCEP Played");
		if (meters && game && origMeters)
		{
			// Determine which method to use for calculating mNonNcepPlayed
			CMeter *ncepPlayed = NULL, *coinIn = NULL, *ticketPromoOut = NULL, *pbtNcepIn = NULL, *ticketPromoIn = NULL, *pbtNcepOut = NULL, *ncepCredits = NULL;

			ncepPlayed = meters->GetMeterByNumber(mRstrPlayd);
			coinIn = meters->GetMeterByNumber(gCI);
			if (ncepPlayed && ncepPlayed->GetValue() != INVALID_METER_VALUE &&
				coinIn && coinIn->GetValue() != INVALID_METER_VALUE)
			{
				nonNcepPlayed = CalculateNonNCEPPlayed(coinIn->GetValue(), ncepPlayed->GetValue());

				// to make all changed meters available in meters variable, to be used later on if needed.
				meters->SetMeter(mNonNcepPlayed, nonNcepPlayed);

				if (game->GetMeter(mNonNcepPlayed) != nonNcepPlayed)
				{
					// Don't set meter value unless less it has changed in order to avoid unnecessary NVRAM writes.
					game->SetMeter(mNonNcepPlayed, nonNcepPlayed);
				}
				bUpdatePlayerCountdownRequired = true;

				// reset pending coinin flag if this is the EGM_GAME_NUMBER 
				if (game->GetGameNumber() == EGM_GAME_NUMBER)
				{
					LogString(NOPTS_LOG, "Clearing m_pendingCoinIn");
					m_pendingCoinIn = false;
					m_origCoinIn = INVALID_METER_VALUE;
				}
			}
			else
			{
				// If ncepPlayed wasn't available, try longer calculation if all values are valid
				// If the meters were sent but invalid, use nvram value
				ticketPromoOut = meters->GetMeterByNumber(mCPromoOutVal);
				if (origMeters->GetMeterByNumber(mCPromoOutVal) == NULL)
					origMeters->SetMeter(mCPromoOutVal);
				if (ticketPromoOut && ticketPromoOut->GetValue() == INVALID_METER_VALUE)
					ticketPromoOut->SetValue(origMeters->GetMeterByNumber(mCPromoOutVal)->GetValue());
				pbtNcepIn = meters->GetMeterByNumber(mPbAR2G);
				if (origMeters->GetMeterByNumber(mPbAR2G) == NULL)
					origMeters->SetMeter(mPbAR2G);
				if (pbtNcepIn && pbtNcepIn->GetValue() == INVALID_METER_VALUE)
					pbtNcepIn->SetValue(origMeters->GetMeterByNumber(mPbAR2G)->GetValue());
				ticketPromoIn = meters->GetMeterByNumber(mCTktPV);
				if (origMeters->GetMeterByNumber(mCTktPV) == NULL)
					origMeters->SetMeter(mCTktPV);
				if (ticketPromoIn && ticketPromoIn->GetValue() == INVALID_METER_VALUE)
					ticketPromoIn->SetValue(origMeters->GetMeterByNumber(mCTktPV)->GetValue());
				pbtNcepOut = meters->GetMeterByNumber(mPbAR2H);
				if (pbtNcepOut)
				{
					LogString(NOPTS_LOG, "NCEP Out value before fiddling: %lld", pbtNcepOut->GetValue());
				}
				else
				{
					LogString(NOPTS_LOG, "NCEP Out value before fiddling is NULL");
				}
			if (origMeters->GetMeterByNumber(mPbAR2H) == NULL)
				origMeters->SetMeter(mPbAR2H);
			if (pbtNcepOut && pbtNcepOut->GetValue() == INVALID_METER_VALUE)
				pbtNcepOut->SetValue(origMeters->GetMeterByNumber(mPbAR2H)->GetValue());
			ncepCredits = meters->GetMeterByNumber(mNcepCredits);
			if (origMeters->GetMeterByNumber(mNcepCredits) == NULL)
				origMeters->SetMeter(mNcepCredits);
			if (ncepCredits && ncepCredits->GetValue() == INVALID_METER_VALUE)
				ncepCredits->SetValue(origMeters->GetMeterByNumber(mNcepCredits)->GetValue());

			if (coinIn && coinIn->GetValue() != INVALID_METER_VALUE &&
				ticketPromoOut && ticketPromoOut->GetValue() != INVALID_METER_VALUE &&
				pbtNcepIn && pbtNcepIn->GetValue() != INVALID_METER_VALUE &&
				ticketPromoIn && ticketPromoIn->GetValue() != INVALID_METER_VALUE &&
				pbtNcepOut && pbtNcepOut->GetValue() != INVALID_METER_VALUE &&
					ncepCredits && ncepCredits->GetValue() != INVALID_METER_VALUE)
				{
					// need to convert two pbt values to credits ****Ignoring fractional credits****
					__int64 ticketPromoInCredits = ticketPromoIn->GetValue() / game->GetDenomMultiplier();
					__int64 ticketPromoOutCredits = ticketPromoOut->GetValue() / game->GetDenomMultiplier();
					// calculate ncep played
					__int64 iNcepPlayed = pbtNcepIn->GetValue() + ticketPromoInCredits - pbtNcepOut->GetValue() - ticketPromoOutCredits - ncepCredits->GetValue();
					meters->SetMeter(mRstrPlayd, iNcepPlayed);
					game->SetMeter(mRstrPlayd, iNcepPlayed);

					// calculate non-ncep played
					__int64 iNonNcepPlayed = CalculateNonNCEPPlayed(coinIn->GetValue(),iNcepPlayed);
					meters->SetMeter(mNonNcepPlayed, iNonNcepPlayed);
                    if (game->GetMeter(mNonNcepPlayed) != iNonNcepPlayed) 
                    {
                        // Don't set meter value unless less it has changed in order to avoid unnecessary NVRAM writes.
                        game->SetMeter(mNonNcepPlayed, iNonNcepPlayed);
                    }
					bUpdatePlayerCountdownRequired = true;

					// reset pending coinin flag if this is the EGM_GAME_NUMBER
					if (game->GetGameNumber() == EGM_GAME_NUMBER)
					{
						LogString(NOPTS_LOG, "Clearing m_pendingCoinIn");
						m_pendingCoinIn = false;
						m_origCoinIn = INVALID_METER_VALUE;
					}

					LogString(NOPTS_LOG, "Calculated meter #: %d, value: %lld", 84, iNcepPlayed);
					if (pbtNcepOut)
					{
						LogString(NOPTS_LOG, "NCEP Out value: %lld", pbtNcepOut->GetValue());
					}
					LogString(NOPTS_LOG, "NCEP In value =  %lld", pbtNcepIn->GetValue());
					if (iNcepPlayed == 0)
					{
						// Calculate 0 value, how?
						LogString(NOPTS_LOG, "Calculated NCEP Played 0");
					}
				}
				else
				{
					// log info to figure out why skipped
				
					LogString(NOPTS_LOG, "Skipped NPFP update");
					LogString(NOPTS_LOG, "meter #: %d, value: %lld", gCI, coinIn != NULL ? coinIn->GetValue() : -1);
					LogString(NOPTS_LOG, "meter #: %d, value: %lld", mCPromoOutVal, ticketPromoOut != NULL ? ticketPromoOut->GetValue() : -1);
					LogString(NOPTS_LOG, "meter #: %d, value: %lld", mPbAR2G, pbtNcepIn != NULL ? pbtNcepIn->GetValue() : -1);
					LogString(NOPTS_LOG, "meter #: %d, value: %lld", mCTktPV, ticketPromoIn != NULL ? ticketPromoIn->GetValue() : -1);
					LogString(NOPTS_LOG, "meter #: %d, value: %lld", mPbAR2H, pbtNcepOut != NULL ? pbtNcepOut->GetValue() : -1);
					LogString(NOPTS_LOG, "meter #: %d, value: %lld", mNcepCredits, ncepCredits != NULL ? ncepCredits->GetValue() : -1);
					LogString(NOPTS_LOG, "meter #: %d, value: %lld", mRstrPlayd, ncepPlayed != NULL ? ncepPlayed->GetValue() : -1);
					LogString(NOPTS_LOG, "End of NPFP Meter Dump");
				}
			}
		}
	}
	else
	{
		bUpdatePlayerCountdownRequired = true;
	}

	return bUpdatePlayerCountdownRequired;
}

void CInternalsFramework::ProcessBillMetersCompleteFromGCF()
{
}

/// <summary>
/// Process a cashout to host request from GCF.
/// </summary>
bool CInternalsFramework::ProcessCashoutToHostFromGCF(CInternalMessage *msg)
{
	if ((CUtilities::GetTickCount() - m_lastCashoutToHostSessionTickCount) < MIN_NEXT_CASHOUT_TO_HOST_SESSION_DELAY_MSECS)
	{
		LogString(PBT_LOG, "[%s] Cashout to host request coming within < 10 secs of previous accepted, skipping...",
			__FUNCTION__);
		return true;
	}

	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	DWORD cashOnGame = msg->GetDWordData();
	DWORD cashPromoOnGame = msg->GetDWordData(true);
	DWORD promoOnGame = (DWORD)msg->GetInt64Data();

	LogString(PBT_LOG, "[%s] EGM Balances: [Cash: %d CashablePromos: %d Promos: %d] CashoutToHostEnabled: %d "
		"Player: 0x%p, IsBannedFromPlay: %d, IsMaxCardLimitReached: %d, "
		"IsGuest: %d, PBFromGameEnabled: %d, CEPOutEnabled: %d, NCEPOutEnabled: %d IsCashAccountInactive: %d",
		__FUNCTION__, cashOnGame, cashPromoOnGame, promoOnGame,	m_config.CashoutToHostEnabled(), (void *)m_player,
		(m_player ? m_player->IsBannedFromPlay() : 0), (m_player ? m_player->IsMaxCardLimitReached() : 0),
		(m_player ? m_player->IsGuest() : 0), m_config.PBFromGameEnabled(), m_config.CEPOutEnabled(), 
		m_config.NCEPOutEnabled(), m_personalBanker->IsCashInactive());

	DWORD eligibleCashToUpload = 0;
	DWORD eligiblePromosToUpload = 0;
	WORD cashoutErrorFlags = 0;

	if (m_config.CashoutToHostEnabled() &&
		nullptr != m_player &&
		!m_player->IsBannedFromPlay())
	{
		// Force exit of existing PBT session (if any) at UI side
		m_personalBanker->SetSessionForceExit();
		SendSubscriptionUpdateIfNecessary();

		if (m_config.PBFromGameEnabled())
		{
			eligibleCashToUpload = cashOnGame + (m_config.CEPOutEnabled() ? cashPromoOnGame : 0);
		}

		if (m_config.NCEPOutEnabled())
		{
			eligiblePromosToUpload = promoOnGame;
		}

		if (eligibleCashToUpload || eligiblePromosToUpload)
		{
			// We have some stuff that is "expected" to be cashed-out, check if we can do so. If not, generate an error.
			cashoutErrorFlags = m_personalBanker->ValidateCreditsForCashoutToHost(m_player, eligibleCashToUpload, eligiblePromosToUpload);
		}
	}

	bool processTransfer = ((eligibleCashToUpload > 0) || (eligiblePromosToUpload > 0));
	if (processTransfer)
	{
		LogString(PBT_LOG, "[%s] Starting PB session for cashout to host [eligibleCashToUpload: %d eligiblePromosToUpload: %d] ...",
			__FUNCTION__, eligibleCashToUpload, eligiblePromosToUpload);

		if (m_personalBanker->BeginPersonalBankerSession(m_player, "", m_config, true))
		{
			if (cashoutErrorFlags)
			{
				LogString(PBT_LOG, "[%s] Setting cashout error [0x%x] in personal banker object.", __FUNCTION__, cashoutErrorFlags);
				m_personalBanker->AppendCashoutErrorFlags(cashoutErrorFlags);
			}

			m_personalBanker->SetEligibleCreditsToUpload(eligibleCashToUpload, eligiblePromosToUpload);

			m_sentinelState.IncrementCashoutToHostRequestsAccepted();

			// Update tickCount when last cashout to host PBT session is started
			m_lastCashoutToHostSessionTickCount = CUtilities::GetTickCount();
			
			LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);

			// Send Tx90 (SessionRequest) to poller
			SendPersonalBankerTransaction();
		}

		// Perform PBActions that CPersonalBanker object might have  
		// requested like SessionRelease of an earlier session
		PerformPersonalBankerActions();
	}
	else
	{
		m_sentinelState.IncrementCashoutToHostRequestsCanceled();

		LogString(PBT_LOG, "[%s] PB transfer conditions not fulfilled, canceling cashout to host request...", __FUNCTION__);

		// We need to tell SAS to cancel cashout to host request so that
		// EGM stops wait of 8 secs to get cash transfer to host and
		// proceed to next available cashout device to use.
		msg->SetData((WORD)0); // tell SAS to cancel cashout to host request
		SendMessageToGCF(msg, QUEUE_PRIORITY_NORMAL);

		if (cashoutErrorFlags)
		{
			LogString(PBT_LOG, "[%s] Sending cashout error [0x%x] to UI for display.", __FUNCTION__, cashoutErrorFlags);
			CInternalMessage *msg1 = new CInternalMessage(INT_MSG_DISPLAY_CASHOUT_PBT_ERROR, cashoutErrorFlags);
			SetMessagePersistenceAndSendMessageToUI(msg1);

			SendMCPlayerSessionInfoToPollerProtocol(SESSION_CHANGE_EVENT_CASHOUT_ERROR, cashoutErrorFlags);
		}
	}

	return processTransfer;
}

/// <summary>
/// Process a PersonalBanker balances message from GCF.
/// </summary>
/// <param name="msg">IN/OUT - a PersonalBanker balances message from GCF.</param>
void CInternalsFramework::ProcessPBBalancesFromGCF( CInternalMessage *msg )
{
	DWORD cashOnGame = 0;
	DWORD cashPromoOnGame = 0;
	DWORD promoOnGame = 0;
	bool bCanTransferOff = false;
	bool bCallToGameFailed = false;
	bool eftMode(false);
	CGame *game = m_games.GetEGM();

	if(msg != NULL)
	{
		cashOnGame = msg->GetDWordData();
		promoOnGame = msg->GetDWordData(true);
		cashPromoOnGame = (DWORD)msg->GetInt64Data();
		eftMode = msg->GetInt64Data(true) == 1;
		bCanTransferOff = (bool)msg->GetWordData(false);
		bCallToGameFailed = (bool)msg->GetWordData(true);
	}
	if (bCallToGameFailed)
	{
		// use stored meter
		promoOnGame = (DWORD)game->GetMeter(mNcepCredits);
	}
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	m_personalBanker->ProcessBalancesFromEGM(cashOnGame,
		cashPromoOnGame,
		promoOnGame,
		bCanTransferOff,
		bCallToGameFailed,
		m_config, 
		eftMode);

	SendSubscriptionUpdateIfNecessary();

	CheckIfEgmNeedsToBeLockedOrUnlocked();

	PerformPersonalBankerActions();

	CPersonalBankerInfo* pbInfo = m_personalBanker->GetInfoCopy();
	if ((m_personalBanker->GetState() == PBStateInfoReceived) &&
		(pbInfo != nullptr) && pbInfo->HasError())
	{
		// Poller returned an error on PBT session initiation, so remove
		// the PBT session. Do not do this when poller asks for PIN reset 
		// since existing PBT session objects are used in that case.
		m_personalBanker->RemoveSession();
	}

	// Release session for mobile, based on the PBInfo error. 
	// Doing it here after balances received from SAS, since here the PbInfo goes to UI.
	if (m_pairingInformation.IsMobileCardedSession() && 
		m_mobilePBTInfo.IsMobileFundsTransferBalanceRequest() && 
		m_personalBanker->GetState() == PBStateInfoReceived)
	{
		if (pbInfo != nullptr && pbInfo->HasError())
		{
			m_mobilePBTInfo.SetError(PBErrorString[pbInfo->GetErrorCode()]);
			SendMCFundsTransferBalanceToPollerProtocol();
		}
		else
		{
			m_mobilePBTInfo.SetBalances(cashOnGame, cashPromoOnGame, promoOnGame);

			ProcessPBCheckDenom();
		}
	}
	delete pbInfo;
}

void CInternalsFramework::ProcessSetGameListFromGCF( CInternalMessage *msg )
{
	// This is the source games object
	CGames *games = msg->GetGamesData();
	WORD numberOfImplementedGames(msg->GetWordData());
	if(numberOfImplementedGames > 0)
	{
		m_games.SetNumberOfImplementedGames(numberOfImplementedGames);
		WORD chunkSize(CPollerMGMDData::Instance().GetGameListChunkSizeOption());
		if (chunkSize == 0)
		{
			chunkSize = 1;
		}
		WORD totalNumberOfChunks((numberOfImplementedGames + chunkSize -1) / chunkSize);
		m_sentinelState.SetTotalNumberOfGameChunks(totalNumberOfChunks);

		vector<DWORD>* vEnabledGameNumbers(msg->GetVectorOfWords());
		if (vEnabledGameNumbers)
		{
			if (!m_games.SyncTheEnabledGames(vEnabledGameNumbers))
			{
				GenerateTilt(tltEnabledSubGameCountChanged);
			}
			msg->SetVectorOfWords(nullptr);
		}
	}
	// Iterate thru all of the source games and see if there is a matching game
	//  in internals game list. If not add it.
	for ( int iSourceGame = 0; iSourceGame < games->GetGameCount(); iSourceGame++ )
	{
		CGame *sourceGame = NULL;

		// Get the next source game from the list of games in the message
		if ( (sourceGame = games->GetGameByIndex( iSourceGame )) != NULL )
		{
			bool subgameAdded = m_games.SetGame(sourceGame);
			LogString(GAMES, "[%s] Updated [Game # %u] to the game list. SubgameAdded: %d",
				__FUNCTION__, sourceGame->GetGameNumber(), subgameAdded);
		}
	}

	CInternalMessage *scmsg = new CInternalMessage(INT_MSG_SEND_ALL_GAMES_LIST, &m_games);
	m_games.UpdateLastGamelistTimestamp(CUtilities::GetCurrentTime());
	scmsg->SetData(numberOfImplementedGames);
	SendMessageToSCF(scmsg, QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::ProcessSetDenomFromGCF( CInternalMessage *msg )
{
	WORD denom = msg->GetWordData();

	if ( denom != INVALID_DENOM )
	{
		m_config.SetGameDenom( denom );

		// Compare this to the denom we got from the system.
		// If it is different, send denom mismatch tilt.
		if ( denom != m_config.GetDenomination() )
		{
			GenerateTilt(tltEepromDenomErr);
		}
	}

	if ( m_personalBanker->IsWaitingForDenomCheck() )
	{
		m_personalBanker->SetWaitingForDenomCheck(false);

		if (nullptr != m_player)
		{
			LogString(PBT_LOG, "ProcessSetDenomFromGCF() pollerdenom=%u gameDenom=%u", m_config.GetDenomination(), denom);
			bool bDenomMatches = denom != INVALID_DENOM && denom == m_config.GetDenomination();
			if (m_pairingInformation.IsMobileCardedSession() && m_mobilePBTInfo.IsMobileFundsTransferBalanceRequest())
			{
				if (bDenomMatches)
				{
					// All good, send balances to mobile
					SendMCFundsTransferBalanceToPollerProtocol();
				}
				else
				{
					if (m_sentinelState.IsGameOnline())
					{// SAS connected
						m_mobilePBTInfo.SetError(DENOM_MISMATCH);
						m_mobilePBTInfo.SetMobileDenomMismatch(true);
					}
					else
					{
						m_mobilePBTInfo.SetError(SAS_ERROR);
						m_mobilePBTInfo.SetMobileSASError(true);
						LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
						ReleasePersonalBankerSession();// SAS not connected, release PBTsession
					}
					SendMCFundsTransferBalanceToPollerProtocol();
				}
			}
		}
	}
}

void CInternalsFramework::ProcessGameCommErrorFromGCF( CInternalMessage *msg )
{
	m_sentinelState.SetEgmErrorCount(msg->GetWordData());

}

void CInternalsFramework::ProcessGameStartFromGCF(CInternalMessage *msg)
{
	// TBD. Activity diagrams say we have to "set cardInTimeFlag = 100 if gameEndedEventsSupported is true"
	// Right now we are not sure what his means. We think it has something to do with virtual sessions.

	EndMiniVirtualSession(); // Before making any updates for the game start.

	SetPlayerLastPlay();

	// Save game status to NvRam and tell display manager.
	m_config.SetGameInProgress(true);
	m_config.SetGameStartExceptions(true);

	{
		// Use m_playerCriticalSection to synchronize m_playerSession 
		// access and subsequent result processing.
		lock_guard<recursive_mutex> lock(m_playerCriticalSection);
		m_playerSession.SetLastPlayedGameSessionGameNumberAndDenom(m_games);

		// It makes more sense to send CardlessSessionStart Tx before 
		// GameSessionChange Tx so that latter transaction is considered
		// to be the part of Cardless session (have Play Session ID same
		// as in the cardless session start Tx)
		if (m_playerSession.GameStarted())
		{
			// If GameStarted() returned true, it means session has changed to 
			// active due to GameStart event (GameInProgress=true) and this can
			// can only happen if its not due to carded or mobile player, so it
			// is the beginning of cardless session
			SendCardlessSessionTransactionsToSCF(/*sessionStart*/ true);
			DistributePlayerData(true, false, false);
		}

		m_playerSession.PlayerActivityOccurred();
	}

	// Sending Tx 127 (Game session change) at game start when a new subgame
	// is played or a new denom. We will send it for game plays in uncarded 
	// sessions also when talking to NFD compatible pollers.
	// Irrespective of NFD or non-NFD pollers, we will still send these Tx 
	// only when MGMD is enabled.

	bool sendGameSessionChangeForCardlessSession = (!m_player &&
		IsNFDPoller() &&
		/* whether MGMD is enabled at OASIS */
		IsGlobalMGMDOptionEnabled() &&		
		/* whether MGMD Senoption is enabled and EGM supports MGMD */
		CPollerMGMDData::Instance().GetMGMDAvailable() &&
		/* whether Poller supports MGMD via FeatureFlags */
		m_config.MultiGameMultiDenom() &&
		m_games.GetCurrentGame() &&
		((m_games.GetCurrentGame()->GetGameNumber() != m_playerSession.GetPrevGameSessionGameNumber()) ||
		(m_games.GetCurrentDenom() != m_playerSession.GetPrevGameSessionGameDenom())));

	bool sendGameSessionChangeForCardedSession = (m_player &&
		!m_player->IsVirtual() && 
		m_player->GameStart() && 
		IsGlobalMGMDOptionEnabled() && 
		CPollerMGMDData::Instance().GetMGMDAvailable() && 
		m_config.MultiGameMultiDenom());

	LogString(SESSION_LOG, "[%s] Checking whether to send Game Session Change (127) msg to Poller "
		"[sendGameSessionChangeForCardlessSession %d sendGameSessionChangeForCardedSession %d",
		__FUNCTION__, sendGameSessionChangeForCardlessSession, sendGameSessionChangeForCardedSession);

	if (sendGameSessionChangeForCardlessSession || sendGameSessionChangeForCardedSession)
	{
		CInternalMessage* gsmsg = PrepareGameSessionChangeTransactionMsg();
		
		if(gsmsg != NULL)
		{			
			LogString(SESSION_LOG, "Sending Game Session Change message (127) to Poller Protocol");
			SendTransactionMessageToSCF(gsmsg, QUEUE_PRIORITY_NORMAL);
			
			m_playerSession.IncrementPlayTransactionSequenceNumber();
		}
	}

	m_displayManager->GameInProgress();

	if (msg != nullptr)
	{
		// Process the meters at the time of game start.
		WORD gameNumber;
		CMeters *meters = msg->GetMetersData(gameNumber);

		if (m_playerSession.IsSasStartupQueueEmpty())
		{
			CMeter* coinIn = meters->GetMeterByNumber(gCI);
			__int64 maxBet = msg->GetInt64Data();
			if (coinIn != nullptr)
			{
				if (coinIn->GetValue() == 0 || coinIn->GetValue() == INVALID_METER_VALUE)
				{
					// Coin In meter is invalid, use m_games coin in instead
					coinIn = m_games.GetGame(EGM_GAME_NUMBER)->GetMeters()->GetMeterByNumber(gCI);
				}
			}
			SetLastCoinsPlayedAndUpdateMaxCoinBetMeter(coinIn, maxBet);

			LogString(GENERAL, "Game start notification, sending gamedata m_games currrent game = %d", 
				(m_games.GetCurrentGame()) ? m_games.GetCurrentGame()->GetGameNumber() : -1);

			SendGameDataToProgressive(INT_MSG_NOTIFY_GAME_START, 0, msg->GetPokerHand());
		}

		ProcessSasStartupQueueEmptyFromGCF((bool)msg->GetDWordData());

		msg->SetData((WORD)m_playerSession.CheckIfHandPayResetProcessingIsNeededAfterGameStart());

		// Let EGMAwardManager know that a game started
		ProcessEgmAwardMessage(msg, mqGCF);
	}

}

CInternalMessage* CInternalsFramework::PrepareGameSessionChangeTransactionMsg()
{
	LogString(SESSION_LOG, "%s", __FUNCTION__);
	CInternalMessage *gsmsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_GAME_SESSION_CHANGE, IsPlayerVirtual());

	if(gsmsg != NULL)
	{
		if(m_player)
		{
			// Make a copy of the player object and add it to the message.
			CPlayer *player = new CPlayer( *m_player );
			gsmsg->SetData(player);
		}

		//Fetching in sepeatare vars instead of storing directly, just for logging purpose
		DWORD ptsn 		= m_playerSession.GetPlayTransactionSequenceNumber();
		int denom 		= m_playerSession.GetPrevGameSessionGameDenom();
		int gameNumber 	= m_playerSession.GetPrevGameSessionGameNumber();
		bool isMGMD 	= CPollerMGMDData::Instance().GetMGMDAvailable();
		DWORD psid 		= m_playerSession.GetSessionId();

		if (isMGMD)
			LogString(SESSION_LOG, "In %s := isMGMD: Yes", __FUNCTION__);
		else
			LogString(SESSION_LOG, "In %s := isMGMD: No", __FUNCTION__);

		LogString( SESSION_LOG, "Sending %s (%d) := PlayTx SN: %u, denom: %d, gamenum: %d, psid: %d", 
			__FUNCTION__, __LINE__, ptsn, denom, gameNumber, psid);
		
		gsmsg->SetData((__int64)ptsn, (__int64)psid);
		gsmsg->SetData(gameNumber, denom);				
		gsmsg->SetData((WORD) isMGMD);
		
		// Reset previous denom/gameNumber for playerSession
		m_playerSession.SetPrevDenomAndGameNumber();		

	}
	else
	{
		LogString(SESSION_LOG, "Failure while preparing GameSessionChange message" );
	}
	
	return gsmsg;
}

void CInternalsFramework::ProcessGeneralPollExceptionFromGCF(CInternalMessage *msg)
{
	if (GPE_OPTIONS_CHANGED == msg->GetWordData())
	{
		LogString(GAMES, "%s %d GPE_OPTIONS_CHANGED Received", __FUNCTION__, __LINE__);
		m_games.ResetIsCompleteGamelistFetched();
		m_sentinelState.SetGameChunkIndexSent(0);
	}

	SendMessageToProgressive(msg, QUEUE_PRIORITY_NORMAL);
}

// Process all meters (and events) associated with this condition BEFORE processing this condition.
// When the condition changes from false to true, we need the coin in (wager in) and coin won meters up-to-date.
// For example, don't process game start and/or game end events after processing this condition (for a change from false to true)
// ... as the meter deltas should be zero.
// TODO: waiting on playerSession gamePlay and progressives
void CInternalsFramework::ProcessSasStartupQueueEmptyFromGCF(bool sasStartupQueueEmpty)
{
	bool initializeLastCoinsPlayedAndWon;
	bool sendOfflineCoinIn;
	m_playerSession.UpdateSasStartupQueueEmpty(sasStartupQueueEmpty, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);

	if (initializeLastCoinsPlayedAndWon)
	{
		InitializeLastCoinsPlayedAndWon();
	}

	if (sendOfflineCoinIn)
	{
		__int64 coinIn(m_config.GetCoinInAtLastGameStart());
		SendMessageToProgressive(new CInternalMessage(INT_MSG_OFFLINE_COIN_IN, coinIn), QUEUE_PRIORITY_NORMAL);
	}

	ProcessSasStartupQueueEmptyUpdate();
}

void CInternalsFramework::ProcessGameCommStatusFromGCF(CInternalMessage *msg)
{
	if (msg != NULL)
	{
		WORD gameNumber;
		CMeters *meters = msg->GetMetersData(gameNumber);
		ProcessSetMeters(meters, gameNumber, false, false);

		bool gameComStatus = (bool)msg->GetDWordData();
		bool sasStartupQueueEmpty = (bool)msg->GetDWordData(true);

		if (gameComStatus)
		{
			ProcessGameCommUpFromGCF();
		}
		else
		{
			ProcessGameCommDownFromGCF();
		}

		ProcessSasStartupQueueEmptyFromGCF(sasStartupQueueEmpty);
	}
}

void CInternalsFramework::ProcessGameCommDownFromGCF()
{
	LogString(GENERAL, _T("ProcessGameCommDownFromGCF"));
	
	CInternalMessage * transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_SERIAL_COMM_DOWN, IsPlayerVirtual());
	transMsg->SetData((WORD)m_EGMConfig.GetEGMProtocol());
	SendMessageToSCF(transMsg, QUEUE_PRIORITY_HIGH);
	m_sentinelState.SetGameOnline(false);
	m_displayManager->GameCommDown();
	ProcessSystemChangeForProgressive();

	EndMiniVirtualSession(); // For diagnostics, update statuses before ending mini-virtual session.
}

void CInternalsFramework::ProcessGameCommUpFromGCF()
{
	LogString(GENERAL, _T("ProcessGameCommUpFromGCF"));
	if (!m_sentinelState.IsGameOnline())
	{
		//In a loss limit environment we need to ensure that we have
		//all bill meters before turning on the BV
		if (m_config.GetLossLimitActive())
		{
			CInternalMessage *bmMsg = new CInternalMessage(INT_MSG_GET_BILL_METERS);
			SendMessageToGCF(bmMsg, QUEUE_PRIORITY_NORMAL);
		}


		CInternalMessage * transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_SERIAL_COMM_UP, IsPlayerVirtual());
		transMsg->SetData((WORD)m_EGMConfig.GetEGMProtocol());
		SendMessageToSCF(transMsg, QUEUE_PRIORITY_HIGH);
		m_sentinelState.SetGameOnline(true);
	}
	m_displayManager->GameCommUp();
	ProcessSystemChangeForProgressive();
}

void CInternalsFramework::ProcessGameEndFromGCF( CInternalMessage *msg )
{
	LogString(SESSION_LOG, "[%s]", __FUNCTION__);

	// Process the meters at the time of game end.
	WORD gameNumber;
	CMeters *meters = msg->GetMetersData( gameNumber );
	m_config.SetGameEndExceptions(true);

	// Get the credit meter and reset excessive voucher out tracking values 
	// credit meter is equal or less than the neglibile credits threshold
	CMeter *creditMeter(meters->GetMeterByIndex(mCrd));
	if (creditMeter != nullptr)
	{
		if (creditMeter->GetValue() <= m_config.GetNegligibleCreditsThreshold())
		{
			m_excessiveVoucherOut.GameCreditsBecameZero();
		}
	}	

	ProcessSetMeters(meters, EGM_GAME_NUMBER, false, false);

	if (!IsNFDPoller())
	{
		/* Get Game End meters specified in CMeters::ignoreDuringGameProgress from games object and
		pass them to SCF*/
		LogString(GAMES, "[%s]: Sending Incremental 304 EGM meters at GameEnd event.", __FUNCTION__);

		CGame *game = m_games.GetGame(gameNumber);

		CGames *scfGames = new CGames();
		CGame *scfGame = new CGame(*game);
		CMeters *egmMeters = scfGame->GetMeters();
		CMeters *gameendMeters = new CMeters(MEMORY_NONE);
		for (auto meterNum : CMeters::ignoreDuringGameProgress)
		{
			//Set game end meters by games object.
			gameendMeters->SetMeter(meterNum, (m_games.GetGame(gameNumber))->GetMeter(meterNum));
		}
		*egmMeters = *gameendMeters;
		delete gameendMeters;
		scfGames->AddGame(scfGame);
		CInternalMessage *scfMsg = new CInternalMessage(INT_MSG_SET_METERS, scfGames);
		SendMessageToSCF(scfMsg, QUEUE_PRIORITY_NORMAL);
	}
	else if (m_maintenanceMode)
	{
		// Send 304 EGM meters message with current subgame meters at game end when
		// communicating with NFD compatible pollers in maintenance mode
		LogString(GAMES, "[%s]: Sending 304 EGM meters at GameEnd event.", __FUNCTION__);
		SendGameMetersToSCF(m_games.GetGame(EGM_GAME_NUMBER));
	}

	//reset the players last played time
	SetPlayerLastPlay();

	// Save game status to NvRam.
	m_config.SetGameInProgress(false);

	{
		// Use m_playerCriticalSection to synchronize m_playerSession access and subsequent result processing.
		lock_guard<recursive_mutex> lock(m_playerCriticalSection);
		m_playerSession.SetLastDenom(m_games.GetCurrentDenom());
		if (m_playerSession.GameEnded())
		{
			DistributePlayerData();
		}
	}

	EndMiniVirtualSession(); // After meter and NVRAM updates for the game end, but before SpeedMedia and progressive bonusing updates.

	// Tell display manager about game status.
	MediaWindowPlayerState playerState;
	playerState = m_displayManager->GameNotInProgress(GetPbtInProgress(), m_mediaWindowPlayerStatePending);
	TellUIMediaWindowPlayerState(playerState);

	if (m_playerSession.IsSasStartupQueueEmpty())
	{
		SendGameDataToProgressive(INT_MSG_NOTIFY_GAME_END, CalcLastCoinsWon(), msg->GetPokerHand());
	}

	ProcessSasStartupQueueEmptyFromGCF((bool)msg->GetDWordData());
}

CGameEndData *CInternalsFramework::GetGameData(long lastCoinsWon, PokerHand *pokerHand, bool gameStart)
{
	CGame *currentGameCopy(m_games.GetAdjustedCurrentGameCopy());
	double sessionPoints = 0;

	if (gameStart)
	{
		lock_guard<recursive_mutex> lock(m_playerCriticalSection);
		if (m_player != nullptr)
		{
			sessionPoints = (double)m_player->GetBasepointsEarned();
		}
	}

	CGameEndData* gameEndData(new CGameEndData(currentGameCopy, m_config.GetLastCoinsPlayed(), 0, lastCoinsWon, pokerHand, m_games.GetCurrentDenom(), sessionPoints));
	return gameEndData;
}

void CInternalsFramework::SendGameDataToProgressive(InternalMsgType messageType, long lastCoinsWon, PokerHand *pokerHand)
{
	lock_guard<recursive_mutex> lock(m_progressiveCriticalSection); // Get a lock, so value(s) are queued in the order acquired.

	CGameEndData* gameEndData(GetGameData(lastCoinsWon, pokerHand, INT_MSG_NOTIFY_GAME_START == messageType));
	if (INT_MSG_NOTIFY_GAME_START == messageType && gameEndData != nullptr)
	{
		const CGame *pGame = gameEndData->GetCurrentGame();
		if (pGame != nullptr)
		{
			LogString(GENERAL, "CInternalsFramework::SendGameDataToProgressive() current game number = %d", pGame->GetGameNumber());
			m_playerSession.SetLastGameNumber(pGame->GetGameNumber());
		}
	}
	SendMessageToProgressive(new CInternalMessage(messageType, gameEndData), QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::SendOfflineCoinInToProgressive(CGame *gameData, long lastCoinsPlayed) 
{
	double sessionPoints = 0;
	if (m_player != nullptr)
	{
		sessionPoints = (double)m_player->GetSessionPoints();
	}
	CGameEndData* gameEndData = new CGameEndData(gameData, lastCoinsPlayed, 0, 0, nullptr, m_config.GetDenomination(), sessionPoints, true);
	SendMessageToProgressive(new CInternalMessage(INT_MSG_NOTIFY_GAME_START, gameEndData), QUEUE_PRIORITY_NORMAL);
}


void CInternalsFramework::SetLastCoinsPlayedAndUpdateMaxCoinBetMeter(CMeter *coinInMeter, __int64 maxBet)
{
	if(coinInMeter != nullptr)
	{
		//set the last coins played
		__int64 coinIn = coinInMeter->GetValue();
		__int64 lastCoinsPlayed = coinIn - m_config.GetCoinInAtLastGameStart();

		// Recover automatically if the Coin In meter changes unexpectedly.
		// Treat rollover as an unexpected change.
		if (lastCoinsPlayed < MINIMUM_LAST_COINS_PLAYED || MAXIMUM_LAST_COINS_PLAYED < lastCoinsPlayed)
		{
			lastCoinsPlayed = 0;
		}

		// Block for locking player session
		{
			lock_guard<recursive_mutex> lock(m_playerCriticalSection);
			m_playerSession.SetLastCoinsPlayed((long)lastCoinsPlayed);
		}

		m_config.SetLastCoinsPlayed((int)lastCoinsPlayed);
		m_sentinelState.SetLastCoinsPlayed((int)lastCoinsPlayed);
		m_config.SetCoinInAtLastGameStart(coinIn);
		LogString(GENERAL, _T("LastCoinIn = %lld LastCoinsPlayed = %lld"), (long long)coinIn, (long long)lastCoinsPlayed);

		// check for max bet played.
		if (lastCoinsPlayed == maxBet)
		{
			LogString(GENERAL, "Max Bet Played.");
			CGame *egmGame = m_games.GetEGM();
			if (egmGame)
			{
				__int64 meterValue = egmGame->GetMeter(gMC);
				if (meterValue == INVALID_METER_VALUE)
				{
					meterValue = 1;
				}
				else
				{
					meterValue++;
				}
				LogString(GENERAL, "MaxCoinBet : MeterNumber = %d, MeterValue = %lld", gMC, meterValue);
				egmGame->SetMeter(gMC, meterValue);
			}
		}
	}
}

// Calculate the credits won for the last game played. Does not include attendant-paid bonus win.
long CInternalsFramework::CalcLastCoinsWon()
{
	long lastCoinsWon(0);
	CGame *egm(m_games.GetEGM());

	if (egm != nullptr)
	{
		__int64 coinsWon(egm->GetCreditsWon());
		__int64 coinsWonAtLastGameEnd(m_config.GetCoinsWonAtLastGameEnd());

		// Individual meters from SAS do not exceed 18 digits.
		// Don't calculate the coins won delta for the last game if the sum of the individual coins won meters is negative.
		if (coinsWon >= 0 && coinsWonAtLastGameEnd >= 0)
		{
			__int64 delta(coinsWon - m_config.GetCoinsWonAtLastGameEnd());
			if (MINIMUM_LAST_COINS_WON <= delta && delta <= MAXIMUM_LAST_COINS_WON)
			{
				lastCoinsWon = (long)delta;
			}
		}

		m_config.SetCoinsWonAtLastGameEnd(coinsWon);
		LogString(GENERAL, _T("SetCoinsWon=%lld LastCoinsWon=%lld"), (long long)coinsWon, (long long)lastCoinsWon);
	}

	return lastCoinsWon;
}

void CInternalsFramework::InitializeLastCoinsPlayedAndWon(void)
{
	// Set a better initial coin in value at last game start. Will still misrepresent last coins played when restarting 
	// nCompass after clearing NVRAM and when game is played while nCompass is shutdown.
	CGame * egm = m_games.GetEGM();

	if (egm != NULL)
	{
		__int64 coinIn = egm->GetMeter(gCI);
		m_config.SetCoinInAtLastGameStart(coinIn);

		__int64 coinsWon(egm->GetCreditsWon());
		m_config.SetCoinsWonAtLastGameEnd(coinsWon);
		LogString(GENERAL, _T("LastCoinIn=%lld LastCoinsWon=%lld (re)initialized"), (long long)coinIn, (long long)coinsWon);
	}

}

void CInternalsFramework::ProcessStartCapturingMetersFromCGF( CInternalMessage *msg )
{
	// We need to set the snapshot in progress when the GCF has started the snapshot. Tracker 19064
	m_sentinelState.SetMeterCaptureStartTime((SnapshotType)msg->GetWordData());
}

void CInternalsFramework::SetCurrentGame( WORD newCurrentGameNumber )
{
	if (IsNFDPoller())
	{
		// Send 305 subgame meters for older current game if game/denom 
		// is changed and any subgame meters of previous game changed
		CGame *currentGame = m_games.GetCurrentGame();
		if (currentGame && m_currentSubgameMetersChanged &&
			(newCurrentGameNumber != (WORD) currentGame->GetGameNumber()))
		{
			LogString(GAMES, "[%s]: Current subgame is being changed from %d to %d. Sending "
				"subgame meters to poller for older subgame since they have changed.",
				__FUNCTION__, currentGame->GetGameNumber(), newCurrentGameNumber);
			SendGameMetersToSCF(currentGame);
		}
	}

	m_currentSubgameMetersChanged = false;
	m_games.SetCurrentGame(newCurrentGameNumber);
	SendGameDataToProgressive(INT_MSG_GAME_DATA, 0, NULL);
}

void CInternalsFramework::ProcessEgmVersionFromGCF(CInternalMessage *msg)
{
	m_sentinelState.SetReportedEgmProtocolVersion(msg->GetStringData().c_str());
}

void CInternalsFramework::ProcessEgmSupportsLP7B(CInternalMessage *msg)
{
	m_sentinelState.SetEgmSupportsLP7B((bool)msg->GetWordData());
	m_sentinelState.SetEgmSupportsLP7B_Available(true);
}

void CInternalsFramework::ProcessSendTicketFromGCF(CInternalMessage *msg)
{
	CTicket *ticket = msg->GetTicketData();

	if ( ticket != NULL )
	{
		LogString(TICKETING_LOG, "ProcessSendTicketFromGCF.");
		ProcessTicketFromGame( *ticket );
	}
}

void CInternalsFramework::ProcessTransferCompleteFromGCF( CInternalMessage *msg )
{
	LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);

	CPersonalBankerTransfer *pbTransfer = msg->GetPBTransferData();
	
	// Store EGM cashout to host status part of AFT transfer flags (first 3 bits) 
	// from the last valid transaction for web diagnostics purpose
	if (pbTransfer != nullptr &&
		pbTransfer->GetPBTIDLength() > 0)
	{
		m_sentinelState.SetEGMCashoutToHostStatus(
			pbTransfer->GetAftTransferFlags() & 0x7);
	}

	ProcessPersonalBankerTransferComplete( pbTransfer );

}

void CInternalsFramework::ProcessTicketStatusFromGCF( CInternalMessage *msg )
{
	CTicket *ticket = msg->GetTicketData();

	TicketInStatus status = (TicketInStatus)msg->GetWordData();

	msg->SetData((CTicket *)NULL);

	ProcessTicketStatus( ticket, status );

}

// Includes check for log values in the range 1..255.
void CInternalsFramework::ProcessEFTSeedFromGCF( CInternalMessage *msg )
{
	m_bIsEFT = true;

	// Get the eft number from the last PB log.
	int eftNumberFromLog = 0;

	// Get the last PB log
	CPersonalBankerLog* lastPBLog(m_personalBanker->GetLastLogCopy());
	if ( lastPBLog != NULL )
	{
		// Get the transaction number from the log.
		int transIDLen;
		char *transID = lastPBLog->GetTransID(transIDLen);

		// It has to be 1-3 digits in length.
		if ( EFT_TRANS_ID_LEN_MINIMUM <= transIDLen && transIDLen <= EFT_TRANS_ID_LEN_MAXIMUM )
		{
			// Get the the numerical value from the string.
			// If there are any characters in the number, don't use it.
			int multiplier = 1;
			while ( transIDLen > 0 )
			{
				if ( isdigit(transID[--transIDLen]) )
				{
					eftNumberFromLog += (transID[transIDLen] - '0') * multiplier;
				}
				else
				{
					eftNumberFromLog = 0;
					transIDLen = 0;
				}
				multiplier *= RADIX_DECIMAL;
			}

			// Only valid if in the range 1..255 -- use 0 if it is not in that range.
			if (eftNumberFromLog < CPersonalBanker::EFT_TRANS_ID_MINIMUM || CPersonalBanker::EFT_TRANS_ID_MAXIMUM < eftNumberFromLog)
			{
				eftNumberFromLog = CPersonalBanker::EFT_TRANS_ID_DEFAULT;
			}
		}

		delete lastPBLog;
	}

	// If the eft number could not be gotten from the log, use the seed we got from SAS
	if ( eftNumberFromLog == 0 )
	{
		m_pbEFTNumber = (byte)msg->GetWordData() + 1;
	}

	// If it was found from the PB log, use it.
	else
	{
		m_pbEFTNumber = (byte)(eftNumberFromLog + 1);
	}
}


void CInternalsFramework::ProcessSnapshotProgressFromGCF( CInternalMessage *msg )
{
	DWORD gameNum(msg->GetDWordData(false));
	WORD totalMeters(msg->GetWordData(true));
	WORD finishedMeters(msg->GetWordData(false));
	m_sentinelState.SetMeterCapturePercentage(gameNum, totalMeters, finishedMeters);
	SendSubscriptionUpdateIfNecessary();
}

void CInternalsFramework::ProcessSnapshotEndFromGCF( CInternalMessage *msg )
{
	bool bCanceled = msg->GetWordData() == 0 ? false : true;

	MeterSnapshotEnd( bCanceled );
}

void CInternalsFramework::MeterSnapshotEnd( bool bCanceled )
{	
	MeterCaptureStatus captureStatus = bCanceled ? CAPTURE_CANCELED : CAPTURE_OK;

	switch ( m_EGMConfig.GetSentinelOnlineState() )
	{
            // If we are coming on line, the meter capture is the last step
    	    case sentinelComingOnline:
    	    case sentinelComingOnlineMove:
				SendSentinelChangeTransactionToSCF();
				m_EGMConfig.SetSentinelOnlineState( sentinelOnline );	// We are online!
    			// Now that we are online, ask the pending ticket-out count.
    			// This will eventually allow ticket printing when the count is received by the SCF. (tracker 21622)
				SendTicketOutPendingRequestToSCF();
		break;

	// If we are going offline, Do final offline stuff. 
	// NOTE: This will start the reboot process.
	case sentinelGoingOfflineMove:
	case sentinelGoingOffline:
		captureStatus = CAPTURE_REBOOT;
		m_sentinelState.SetMeterCaptureEnd(captureStatus, m_games.GetGameCount());
		SendSubscriptionUpdateIfNecessary();
		PerformFinalOfflineStep();
		break;
	// If we are going offline because of a download, tell the GCF.
	// NOTE: When we get a response back from the GCF we will do the final offline step.
	case sentinelGoingOfflineDownload:
		// Do this to let the persistent memory think we are done with the offline
		m_EGMConfig.SetSentinelOnlineState( sentinelOffline );
		m_EGMConfig.SetSentinelOnlineState( sentinelGoingOfflineDownload, false );

		SendDownloadAllowToGCF( true );
		break;
	default:
		break;
	}

	m_sentinelState.SetMeterCaptureEnd(captureStatus, m_games.GetGameCount());
	SendSubscriptionUpdateIfNecessary();

	//if we no longer have a card in, the UI should return
	//to attractor mode after finishing the capture
	//  ... but do not send to UI if it a background snapshot 19064.
	if (!DoesEmployeeExist()&& !DoesPlayerExist() && m_EGMConfig.GetSentinelOnlineState() == sentinelOnline)
	{
		LogString(UI_LOG, "%s %d", __FUNCTION__, __LINE__);
		SendDisplayAttractorToUI();
	}
}

void CInternalsFramework::PerformFinalOfflineStep( void )
{
	// Send transaction indicating we are going offline.
	SendSentinelChangeTransactionToSCF();

	// Shutdown the GCF
	// Moved this to below - Tracker #25893
	//UninitializeGCF();

	SentinelOnlineState currentOnlineState = m_EGMConfig.GetSentinelOnlineState();

	// Set the EGM protocol back to NONE and clear the slotmast if this not a machine move
	if ( currentOnlineState == sentinelGoingOfflineMove )
	{
		// Do this to let the persistent memory think we are done with the offline
		m_EGMConfig.SetSentinelOnlineState( sentinelOfflineMove );
	}
	else
	{
		// Do this to let the persistent memory think we are done with the offline
		m_EGMConfig.SetSentinelOnlineState( sentinelOffline );

		m_EGMConfig.SetEGMType( NO_EGM );
		m_config.SetSlotMastID( INVALID_SLOT_MAST_ID );
		m_EGMConfig.SetEGMProtocolState( protocolStateDisabled );

		// Remove subgames from m_games
		m_games.ResetGames(); // It is okay that this also removes the main egm game.
	}

	// Retain our state but don't set in persistent memory.
	m_EGMConfig.SetSentinelOnlineState( currentOnlineState, false );

	// Display the reboot message if we are doing a download offline (the other offline's will display reboot with the meter capture).
	if (currentOnlineState == sentinelGoingOfflineDownload)
	{
		m_notificationBox.AddAlert(Alerts::ALERT_REBOOT_IN_PROGRESS);
		SendSubscriptionUpdateIfNecessary();
	}

	// Shutdown the GCF
	//Moved this down so that we set our online state before shutting down GCF - Tracker #25893
	UninitializeGCF();

	// Start the Reboot process
	BeginRebootProcess();
}

void CInternalsFramework::ProcessDoorOpenFromGCF( CInternalMessage *msg )
{
	WORD oasisMeter = msg->GetWordData();
	ProcessDoorOpen(oasisMeter);
}

void CInternalsFramework::ProcessDoorClosedFromGCF( CInternalMessage *msg )
{
	WORD oasisMeter = msg->GetWordData();
	ProcessDoorClosed(oasisMeter);
}

void CInternalsFramework::ProcessJackpotInfoFromGCF( CInternalMessage * msg )
{
	CJackpot *jackpot = msg->GetJackpotData();

	if ( jackpot != NULL )
	{
		// Since SAS 3 games may not send Game End events Speedmedia events won't be process. Don't want to process them here either.
		if (jackpot->GetJackpotType() != JT_CANCEL_CREDIT && m_EGMConfig.GetEGMProtocol() != IGT_SAS_3xx)
		{
			SendPayoutToOneLink( jackpot );
		}

		{
			lock_guard<recursive_mutex> lock(m_egmAwardCriticalSection); 
	
			// Let egm award manager see the jackpot
			CInternalMessage* targetMsg = nullptr;
			MessageQueueId target = mqNone;
			SYSTEMTIME systemTime;
			CUtilities::GetUTCSystemTime(&systemTime);
	
			std::vector<MsgTargetPair*> *returnMessages = new std::vector<MsgTargetPair*>();
	
			m_egmAwardManager.ProcessMessage(CUtilities::GetTickCount(), m_games,
				m_progressiveCriticalText.IsDown(),
				msg,
				mqGCF,
				targetMsg,
				target,
				returnMessages,
				m_playerSession.IsSasStartupQueueEmpty());

			if (targetMsg != NULL)
			{
				switch (target)
				{
				case mqProgressive:
					SendMessageToProgressive(targetMsg, QUEUE_PRIORITY_NORMAL);
					break;
				case mqGCF:
					SendMessageToGCF(targetMsg, QUEUE_PRIORITY_NORMAL);
					break;
				default:
					if (targetMsg != nullptr)
					{
						delete targetMsg;
						targetMsg = nullptr;
					}
					break;
				}
			}

			ProcessReturnedMessages(returnMessages);
			delete returnMessages; //delete the vector created
		}

		// Now process the jackpot
		ProcessJackpot(jackpot);
	}

}

void CInternalsFramework::ProcessMeterChangeRequestFromGCF( CInternalMessage * msg )
{
	// ignore a request if we are in one
	if (!m_sentinelState.IsMeterChangeInProgress())
	{
		m_sentinelState.SetMeterChangeRequestType((MeterChangeRequestType)msg->GetWordData());

		if (m_sentinelState.IsMeterChangeInProgress())
		{
			GenerateTilt(tltMeterChangeRequest);

			if (!DoesPlayerExist() && m_sentinelState.IsPollerOnline())
			{
				// If this is a major request, we need to get the okay from the UI.
				if (m_sentinelState.IsMeterChangeMajor())
				{
					if ( m_sentinelState.TransactionFillStatusOkay() && m_EGMConfig.CanPerformMachineOffline() )
					{
						SendMessageToUI(new CInternalMessage(INT_MSG_DISPLAY_DOWNLOAD_PENDING), QUEUE_PRIORITY_HIGHEST);
					}

					else
					{
						SendDownloadAllowToGCF( false );
						m_sentinelState.SetMeterChangeRequestType(meterChangeRequestNone);
					}
				}

				// For a minor type, just go ahead and do it.
				else
				{
					SendDownloadAllowToGCF( true );
				}
			}

			// We cannot allow a meter meter change at this time 
			else
			{
				SendDownloadAllowToGCF( false );
				m_sentinelState.SetMeterChangeRequestType(meterChangeRequestNone);
			}
		}
	}
}

void CInternalsFramework::ProcessMeterChangeAllowSentFromGCF( CInternalMessage * msg )
{
	bool bMeterChangeAllowed = (msg->GetWordData() != 0);

	// If we told the EGM to go ahead with the download, do it now (tracker 17428)
	if ( bMeterChangeAllowed )
	{
		ProcessMeterChangeCompleteFromGCF();
	}

	// Once we told the EGM that we are not allowing this change, we are done with the meter change in progress.
	else
	{
		m_sentinelState.SetMeterChangeRequestType(meterChangeRequestNone);
	}

}

void CInternalsFramework::SendDownloadAllowToGCF( bool bAllowDownload )
{
	CInternalMessage *allowMsg = new CInternalMessage( INT_MSG_METER_CHANGE_ALLOW, bAllowDownload ? (WORD)1 : (WORD)0 );
	SendMessageToGCF( allowMsg, QUEUE_PRIORITY_NORMAL ); 
}

void CInternalsFramework::ProcessMeterChangeCompleteFromGCF()
{
	m_sentinelState.SetMeterChangeRequestType(meterChangeRequestNone);
	GenerateTilt(tltMeterChangeComplete);

	if ( m_EGMConfig.GetSentinelOnlineState() == sentinelGoingOfflineDownload )
	{
		PerformFinalOfflineStep();
	}
}

void CInternalsFramework::ProcessMeterChangeCancelFromGCF()
{
	m_sentinelState.SetMeterChangeRequestType(meterChangeRequestNone);

	CInternalMessage *uiMsg = new CInternalMessage( INT_MSG_DOWNLOAD_CANCELED );
	SendMessageToUI( uiMsg, QUEUE_PRIORITY_NORMAL );

	GenerateTilt(tltMeterChangeCancelled);

	if ( m_EGMConfig.GetSentinelOnlineState() == sentinelGoingOfflineDownload )
	{
		m_EGMConfig.SetSentinelOnlineState( sentinelOnline );

		// Now that we are online, ask the pending ticket-out count.
		// This will eventually allow ticket printing when the count is received by the SCF. (tracker 21622)
		SendTicketOutPendingRequestToSCF();
	}
}

CInternalMessage * CInternalsFramework::GetStackerOutMessage()
{
	CInternalMessage *msg;

	//Tell the SCF it needs to send a transaction
	msg = PrepareTransactionMessage(INT_MSG_SEND_STACKER_OUT, IsPlayerVirtual());

	return msg;
}

CInternalMessage * CInternalsFramework::GetStackerInMessage()
{
	CInternalMessage *msg;

	//Tell the SCF it needs to send a transaction
	msg = PrepareTransactionMessage(INT_MSG_SEND_STACKER_IN, IsPlayerVirtual());

	return msg;
}

CInternalMessage * CInternalsFramework::GetTiltMessage(TiltCode tilt)
{
	CInternalMessage *msg;

	//Tell the SCF it needs to send a transaction
	msg = PrepareTransactionMessage(INT_MSG_SEND_TILT, IsPlayerVirtual());
	msg->SetData((WORD)tilt);
	msg->SetData((DWORD)m_config.GetSlotNumber());

	return msg;

}

void CInternalsFramework::ProcessGCFStarted()
{
	// Kick start ticketing, even when no config is received because poller is down.
	SetTicketsConfig();
	SendTicketingInfoToGCF();
	SendTicketOutPendingRequestToSCF();
}

void CInternalsFramework::GenerateTilt(TiltCode code)
{
	InternalMsgType additionalMsg = INT_MSG_NONE;
	bool sendTilt = false;

	LogString(POLLER_LOG, "Processing tilt code %d", code);

	// Only send tltB0B5Failure tilts if onelink is enabled
	if (NULL != m_tiltManager && (tltSubgameCaptureFailed != code || m_config.OneLinkProgressivesEnabled()))
	{
		sendTilt = m_tiltManager->ProcessTilt(code, additionalMsg);
	}

	if (INT_MSG_NONE != additionalMsg)
	{
		if (INT_MSG_SEND_STACKER_IN == additionalMsg)
		{
			m_restartManager.PerformPostDropTasks();
		}
		CInternalMessage *addedMsg = PrepareTransactionMessage(additionalMsg, IsPlayerVirtual());
		SendTransactionMessageToSCF(addedMsg, QUEUE_PRIORITY_NORMAL);
	}

	if (sendTilt)
	{
		CInternalMessage *tiltMsg = GetTiltMessage(code);
		SendTransactionMessageToSCF(tiltMsg, QUEUE_PRIORITY_NORMAL);
	}
}

void CInternalsFramework::ProcessFinishedFirstPass(CInternalMessage *msg)
{
	
	// if coMeters was supplied, use its totCoinOutCredits value for new games collection (to be sent to the progressive protocol).
	WORD gameNumber = EGM_GAME_NUMBER;
	bool gameListError = msg->GetWordData();
	if (!gameListError)
	{	
		LogString(GAMES, "%s %d", __FUNCTION__, __LINE__);

		CMeters *coMeters = msg->GetMetersData(gameNumber);
		if (coMeters != nullptr)
		{
			__int64 totCoinOut = coMeters->GetMeterByNumber(gCO)->GetValue();
			m_games.SetLastTotalCoinOutMeterValue(totCoinOut);
		}
	
		m_finishedFirstPassForSubGames = true;

		CGames * games(msg->GetGamesData());
		if (games)
		{
			LogString(GAMES, "%s %d gameCount=%u", __FUNCTION__, __LINE__, games->GetGameCount());
			std::lock_guard< std::recursive_mutex > lock(m_progressiveCriticalSection);
			m_sentinelState.SetProgGames(games);
			msg->SetData((CGames *)nullptr); //keeping games data
			CheckProgressiveWaitingForSubGames();
			SendMessageToSCF(new CInternalMessage(INT_MSG_SEND_ENABLED_DENOMGAMES_LIST, games), QUEUE_PRIORITY_NORMAL);
		}
		else
		{
			LogString(GAMES, "%s %d no games available.", __FUNCTION__, __LINE__);
		}
	}
	else
	{
		LogString(GAMES, "%s %d", __FUNCTION__, __LINE__);

		// Error while retrieving games list.
		if (m_progressiveWaitingForSubGames)
		{
			m_finishedFirstPassForSubGames = false;
		}

		// Tell GCF to get game list again
		CInternalMessage *gcfMsg = new CInternalMessage(INT_MSG_GET_GAME_LIST);
		SendMessageToGCF(gcfMsg, QUEUE_PRIORITY_NORMAL);
	}
	
}


void CInternalsFramework::ProcessFinishedOfflinePlayPass(CInternalMessage* msg)
{
	bool newValue = (bool)msg->GetWordData();
	if (m_finishedOfflineCheckForSubgames != newValue)
	{
		m_finishedOfflineCheckForSubgames = newValue;
		LogString(GAMES, "Setting m_finishedOfflineCheckForSubgames = %d", newValue);
	}
}

void CInternalsFramework::ProcessUpdateSasInfo(CInternalMessage* msg)
{
	CGames *games = msg->GetGamesData();

	if (games)
	{
		// merge SAS Info with m_games;
		m_games.SetSASVersion(games->GetSASVersion());
		m_config.SetSASVersion(games->GetSASVersion());
		m_games.SetGameId(games->GetGameId());
		m_games.SetProgressiveGroup(games->GetProgressiveGroup());
		m_games.SetAddress(games->GetAddress());
		m_games.SetSerialNumber(games->GetSerialNumber());
		m_games.SetAssetNumber(games->GetAssetNumber());
		m_games.SetAccountingDenom(games->GetAccountingDenom());
		m_games.SetMultiDenomEnabled(games->GetMultiDenomEnabled());
		m_games.SetExtendedMetersEnabled(games->GetExtendedMetersEnabled());
		m_games.SetAftEnabled(games->GetAftEnabled());
		m_games.SetAftBonusEnabled(games->GetAftBonusEnabled());
		m_games.SetAftInHouseEnabled(games->GetAftInHouseEnabled());
		m_games.SetLegacyBonusEnabled(games->GetLegacyBonusEnabled());
		m_games.SetEGMTransferMode(games->GetEGMTransferMode());
		m_games.SetEftTransferToEnabled(games->GetEftTransferToEnabled());
		m_games.SetEftTransferFromEnabled(games->GetEftTransferFromEnabled());
		m_games.SetValidationExtensionsEnabled(games->GetValidationExtensionsEnabled());
	}
}

void CInternalsFramework::ProcessBonusCapabilities(CInternalMessage *msg)
{
	CGames *games = msg->GetGamesData();

	if (games)
	{
		m_games.SetAftBonusEnabled(games->GetAftBonusEnabled());
		m_games.SetLegacyBonusEnabled(games->GetLegacyBonusEnabled());
	}
}

void CInternalsFramework::ProcessCurrentDenom(CInternalMessage *msg)
{
	int denom = msg->GetDWordData();
	m_games.SetCurrentDenom(denom);
    LogString(GENERAL, _T("ProcessCurrentDenom: denom: %d"), denom);
	m_playerSession.IncrementGameSessionIdIfRequired(m_games);
	if (m_playerSession.IsGameSessionIdIncremented())
	{
		SendSubgameSelectToPoller();
		SendCurrentDenomToProgressive();
	}
}

void CInternalsFramework::SendSubgameSelectToPoller()
{
	if (m_config.GetGlobalMGMDEnabled() &&
		CPollerMGMDData::Instance().GetMGMDAvailable())
	{
		SubgameSessionParameters *params = new SubgameSessionParameters();
		params->m_denom = m_games.GetCurrentDenom();
		params->m_gameNumber = m_games.GetCurrentGame()->GetGameNumber();
		params->m_gameSessionSequenceNumber = m_playerSession.GetGameSessionId();
		params->m_playSessionID = m_playerSession.GetSessionId();

		// if npfp then default to ACCOUNTING_CASHABLE_PLAYED
		if (!m_config.GetAccruePointsForNCEP())
		{
			params->m_accountingType = ACCOUNTING_CASHABLE_PLAYED;
		}

		LogString(POLLER_LOG, "%s %d Sending 323 message. gameNumber: %u denom: %u", __FUNCTION__, __LINE__, params->m_gameNumber, params->m_denom);

		CInternalMessage *pollMsg = new CInternalMessage(INT_MSG_SUBGAME_SELECTION);

		if (m_player != nullptr)
		{
			params->m_multiplier = m_player->GetPlayerInfo()->GetMultiplier();
			params->m_divider = m_player->GetPlayerInfo()->GetDivider();

			pollMsg->SetData(new CPlayer(*m_player));
		}
		else
		{
			params->m_multiplier = m_config.GetPlayerTrackingMultiplier();
			params->m_divider = m_config.GetPlayerTrackingDivisor();
		}

		// keep a copy of params
		SubgameSessionParameters *copyParams = new SubgameSessionParameters(*params);

		pollMsg->SetData(params);
		pollMsg->SetData((WORD)CPollerMGMDData::Instance().GetMGMDAvailable());
		SendMessageToSCF(pollMsg, QUEUE_PRIORITY_NORMAL);

		if (m_player != nullptr)
		{
			m_player->SetGameSession(*copyParams);
		}

		delete copyParams;
	}
}

void CInternalsFramework::SendCurrentDenomToProgressive()
{
	lock_guard<recursive_mutex> lock(m_progressiveCriticalSection); // Get a lock, so value(s) are queued in the order acquired.

	CInternalMessage *progMsg = new CInternalMessage(INT_MSG_SET_DENOM, (DWORD)m_games.GetCurrentDenom());
	SendMessageToProgressive(progMsg, QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::ProcessJackpotResetResult(CInternalMessage *msg)
{	
	lock_guard<recursive_mutex> jackpotLock(m_jackpotCriticalSection); // Get a lock, so value(s) are queued in the order acquired.
	lock_guard<recursive_mutex> playerLock(m_playerCriticalSection); // Get a lock, so value(s) are queued in the order acquired.
	
	CJackpot::LogInfo(m_jackpot, _T("ProcessJackpotResetResult prior"));

	if (m_jackpot != nullptr)
	{
		CJackpot *jackpot = msg->GetJackpotData();
		CJackpot::LogInfo(jackpot, _T("ProcessJackpotResetResult msg"));
		if (jackpot != nullptr)
		{
			LogString(JACKPOT, "%s %d", __FUNCTION__, __LINE__);
			m_jackpot->SetUnattendedState(jackpot->GetUnattendedState());
			if (m_jackpot->IsTaxWithholdingEnabled() && UnattendedJackpotState::UJPCreditAndRemoteReset == m_jackpot->GetUnattendedState())
			{
				//Send the jackpot info to poller but don't update the UJP UI just yet.  
				SendUJPToPoller(new CJackpot(*m_jackpot));
				if(m_personalBanker->BeginPersonalBankerSession(m_player, jackpot->GetPlayerPin(), m_config, false, m_jackpot))
				{
					LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
					SendPersonalBankerTransaction();
				}
			}
			else
			{
				SendSubscriptionUpdateIfNecessary();
				SendUJPToPoller(new CJackpot(*m_jackpot));
			}
		}
	}
	CJackpot::LogInfo(m_jackpot, _T("ProcessJackpotResetResult done"));
}

void CInternalsFramework::ProcessIncBillMeter(CInternalMessage *msg)
{
	if (msg != nullptr)
	{
		int oasisMeter = (int)msg->GetDWordData();
		int incAmount = (int)msg->GetDWordData(true);

		// Get current meter value
		__int64 currentValue = m_games.GetEGM()->GetMeter(oasisMeter);

		if (currentValue != INVALID_METER_VALUE)
		{
			currentValue += incAmount;
		}
		else
		{
			currentValue = incAmount;
		}
		CMeters *meters = new CMeters(MEMORY_NONE);
		meters->SetMeter(oasisMeter, currentValue);
		ProcessSetMeters(meters, EGM_GAME_NUMBER, false, false);
		delete meters;
	}
}


void CInternalsFramework::ProcessDoorOpen( WORD oasisMeter, bool sendTransaction)
{
	if( oasisMeter != mNone)
	{
		CGame * egm = m_games.GetEGM();

		if( egm != NULL)
		{
			__int64 currentValue = egm->GetMeter((BYTE)oasisMeter);

			if((CUtilities::IsEven(currentValue)) || (currentValue == INVALID_METER_VALUE))
			{
				switch(m_runaway.GetSendAction(oasisMeter))
				{
				case RUNAWAY_ACTION_SEND:
					{
						SnapshotType snapshotToPerform = SnapshotNone;

						// if we think it's closed
						// set it as open and send the transaction
						if (currentValue == INVALID_METER_VALUE)
						{
							currentValue = 1; //otherwise it would be set to zero
						}
						else
						{
							currentValue++; //increment
						}

						egm->SetMeter((BYTE)oasisMeter, currentValue);

						//if it's the slot door, tell the ui.
						if (oasisMeter == mSDr)
						{
							SetDoorState();
						}

						//if we need to send the transaction, do it.
						if (sendTransaction)
						{
							CInternalMessage *msg;
							if (( oasisMeter == mSDr ) && !DoesEmployeeExist() )
							{
								msg = PrepareTransactionMessage(INT_MSG_TRANSACTION_ILLEGAL_DOOR_OPEN, IsPlayerVirtual());
							}
							else
							{		
								msg = PrepareTransactionMessage(INT_MSG_DOOR_OPEN, IsPlayerVirtual());
								msg->SetData(oasisMeter);

								//Perform meter capture nly if we recived config from poller
								if (m_sentinelState.IsPollerOnline() && m_config.GetSlotMastID() != INVALID_SLOT_MAST_ID)
								{
									// Cashdoor open and drop door open need to capture meters.
									if (oasisMeter == mCDr)
										snapshotToPerform = MpCashDoor;
									else if (oasisMeter == mDDr)
										snapshotToPerform = MpDropDoor;
								}
							}
							SendTransactionMessageToSCF(msg, QUEUE_PRIORITY_NORMAL);
						}

						if (!IsNFDPoller())
						{
							// Send this single meter update to the SCF
							LogString(GAMES, "[%s]: Sending Incremental 304 EGM meters at DoorOpen [%d] event.",
								__FUNCTION__, oasisMeter);
							CGames *scfGames = new CGames();
							CGame *scfGame = new CGame(*(m_games.GetEGM()));
							CMeters * scfMeters = scfGame->GetMeters();
							scfMeters->ClearAllMeters();
							scfMeters->SetMeter((byte)oasisMeter, currentValue, true, false);
							scfGames->AddGame(scfGame);
							CInternalMessage *scfMsg = new CInternalMessage(INT_MSG_SET_METERS, scfGames);
							SendMessageToSCF(scfMsg, QUEUE_PRIORITY_NORMAL);
						}

						// See if we need to do a meter capture.
						if (snapshotToPerform != SnapshotNone)
							BeginMeterCapture(snapshotToPerform, false, false);
					}
					break;
				case RUNAWAY_ACTION_DONT_SEND:
					break;
				case RUNAWAY_ACTION_SEND_RUNAWAY:
					CInternalMessage *msg;
					msg = PrepareTransactionMessage(INT_MSG_TRANSACTION_METER_RUNAWAY, IsPlayerVirtual());
					SendTransactionMessageToSCF(msg, QUEUE_PRIORITY_NORMAL);
					break;
				default: 
					break;
				}
			}
		}
	}
}

void CInternalsFramework::ProcessDoorClosed( WORD oasisMeter, bool sendTransaction )
{
	if( oasisMeter != mNone)
	{
		CGame * egm = m_games.GetEGM();
		if( egm != NULL)
		{
			__int64 currentValue = egm->GetMeter((BYTE)oasisMeter);

			if(!CUtilities::IsEven(currentValue))
			{
				switch(m_runaway.GetSendAction(oasisMeter))
				{
				case RUNAWAY_ACTION_SEND:
					{	
						// if we it's open, set the meter closed and send the transaction
						currentValue++; //increment to even

						egm->SetMeter((BYTE)oasisMeter, currentValue);

						if (oasisMeter == mSDr)
							SetDoorState();

						if (sendTransaction)
						{
							CInternalMessage *msg;
							//Tell the SCF it needs to send a transaction
							msg = PrepareTransactionMessage(INT_MSG_DOOR_CLOSED, IsPlayerVirtual());
							msg->SetData(oasisMeter);
							SendTransactionMessageToSCF(msg, QUEUE_PRIORITY_NORMAL);
						}

						if (!IsNFDPoller())
						{
							// Send this single meter update to the SCF
							LogString(GAMES, "[%s]: Sending Incremental 304 EGM meters at DoorClose [%d] event.",
								__FUNCTION__, oasisMeter);
							CGames *scfGames = new CGames();
							CGame *scfGame = new CGame(*(m_games.GetEGM()));
							CMeters * scfMeters = scfGame->GetMeters();
							scfMeters->ClearAllMeters();
							scfMeters->SetMeter((byte)oasisMeter, currentValue, true, false);
							scfGames->AddGame(scfGame);
							CInternalMessage *scfMsg = new CInternalMessage(INT_MSG_SET_METERS, scfGames);
							SendMessageToSCF(scfMsg, QUEUE_PRIORITY_NORMAL);
						}
					}
					break;
				case RUNAWAY_ACTION_DONT_SEND:
					break;
				case RUNAWAY_ACTION_SEND_RUNAWAY:
					CInternalMessage *msg;
					msg = PrepareTransactionMessage(INT_MSG_TRANSACTION_METER_RUNAWAY, IsPlayerVirtual());
					SendTransactionMessageToSCF(msg, QUEUE_PRIORITY_NORMAL);
					break;
				default: 
					break;
				}
			}
		}
	}
}

bool CInternalsFramework::ProcessProbePBTFromGCF(CInternalMessage *msg)
{
	bool deleteMsg = true;
	WORD bLock = msg->GetWordData();
	msg->SetData(bLock, (WORD)true); // if lock not available, show error
	int priority = QUEUE_PRIORITY_NORMAL;
	DWORD persistTicks = POPUPDISPLAYTIME;
	LogString(PBT_LOG, "ProcessProbePBTFromGCF() bLock = %d", bLock);

	if (!m_mobilePBTInfo.IsMobileFundTransferRequest())
	{
		if (!bLock)
		{
			CMessagePersist *mp = m_msgPriority.PrioritizeMessage(msg, priority);
			mp->SetMainStarveTicks(persistTicks);
			msg->SetMessagePersist(mp);
		}

		SendMessageToUI(msg, QUEUE_PRIORITY_HIGHEST);
		deleteMsg = false;
	}
	
	if (m_pairingInformation.IsMobileCardedSession() && m_mobilePBTInfo.IsMobileFundTransferRequest())
	{
		if(bLock)
		{
			HandlePBTProbeMobile();
		}	
		else
		{
			// lock not available
			SendMCFundsTransferToPollerProtocol(false, LOCK_NOT_AVAILABLE, true);
		}
	}
	
	return deleteMsg;
}


void CInternalsFramework::SendMCFundsTransferBalanceToPollerProtocol()
{
	if (!(m_config.MobileConnectCommunicationEnabled()))
		return

	LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);
	CPersonalBankerInfo* pbInfo = m_personalBanker->GetInfoCopy();
	// Edge case during error condition
	if (nullptr == pbInfo)
		pbInfo = new CPersonalBankerInfo();

	if (m_player != nullptr && 
		m_player->GetPlayerInfo() != nullptr && 
		m_player->GetPlayerInfo()->IsEnrolledInWallet_2_0() &&
		!m_mobilePBTInfo.IsWallet_2_0_CompatiblePDS())
	{
		LogString(POLLER_LOG, "%s %d: PDS is not compatible with wallet 2.0, so sending Cash Balance and Cash upload/download limits as 0.", __FUNCTION__, __LINE__);
		pbInfo->SetCashBalance(0);
		pbInfo->SetMaximumCashDown(0);
		pbInfo->SetMaximumCashUp(0);
	}

#ifdef _DEBUG
	LogString(POLLER_LOG, "%s %d: Creating CFundsTransferBalanceEvent, cash balance = %d, DisplayTransferCashToGame = %d, DisplayTransferCashFromGame = %d", 
		__FUNCTION__, __LINE__, pbInfo->GetCashBalance(), pbInfo->DisplayTransferCashToGame(), pbInfo->DisplayTransferCashFromGame());
#endif

	CFundsTransferBalanceEvent *pFundsTransferBalanceEvent = new CFundsTransferBalanceEvent(m_mobilePBTInfo.GetCashableBalance(),
		(m_config.CEPOutEnabled() ? m_mobilePBTInfo.GetCashPromoBalance() : 0),
		m_mobilePBTInfo.GetNonCashablePromoBalance(),
		pbInfo,
		m_mobilePBTInfo.GetError(),
		m_pairingInformation.GetCorrelationId(RequestFundsTransferBalance),
		pbInfo->IsUnableToMoveLeftoverCash());

	CInternalMessage *pollerMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_MOBILE_CONNECT_PROXIMITY, IsPlayerVirtual());
	pollerMsg->SetData((WORD) ToPlayerDeviceService::EventFundsTransferBalance);
	pollerMsg->SetData(pFundsTransferBalanceEvent);
	SendTransactionMessageToSCF( pollerMsg, QUEUE_PRIORITY_NORMAL );

	m_mobilePBTInfo.SetMobileFundsTransferBalanceRequest(false);
	m_mobilePBTInfo.ClearBalances();
	m_mobilePBTInfo.ClearError();
}

void CInternalsFramework::HandlePBTProbeMobile()
{
	bool valid = true;
	PersonalBankerTransferType type = m_mobilePBTInfo.GetMobileTransferType();
	DWORD amount = m_mobilePBTInfo.GetMobileAmountInPennies();
	
	if (m_personalBanker->IsToGame(type))
		valid = m_personalBanker->CheckValidTransferAmount(type, (DWORD)(amount / ONE_DOLLAR_IN_CENTS));
			
	if (valid)
	{
		// initiate transfer
		BeginPersonalBankerTransaction( type, amount);

		bool isTransferToGame = type == PersonalBankerTransferType::CashToGame ||
			type == PersonalBankerTransferType::PointsToGame ||
			type == PersonalBankerTransferType::PromoDollarsToGame;
		SendMessageToUI(new CInternalMessage(isTransferToGame ? INT_MSG_LISTEN_TO_PBT_PROGRESS_TO_GAME : INT_MSG_LISTEN_TO_PBT_PROGRESS_FROM_GAME),
			QUEUE_PRIORITY_NORMAL);
	}
	else
	{
		SendMCFundsTransferToPollerProtocol(false, INVALID_AMOUNT, true);

		CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
		LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
		ReleasePersonalBankerSession();
	}
}

__int64 CInternalsFramework::CalculateNonNCEPPlayed(__int64 coinIn, __int64 NCEPPLayed)
{
	return coinIn > NCEPPLayed ? coinIn - NCEPPLayed : 0;
}
