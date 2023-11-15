#include "stdafx.h"
#include "Logging/LogString.h"
#include "SASProtocol.h"
#include "LongPoll3DMessage.h"
#include "LongPoll4CMessage.h"
#include "LongPoll4DMessage.h"
#include "LongPoll1BMessage.h"
#include "LongPoll83Message.h"
#include "LongPoll84Message.h"
#include "LongPoll85Message.h"
#include "LongPollA8Message.h"
#include "../Jackpot.h"
#include "LongPollB6Message.h"
#include "Diagnostics.h"
#include "../Sentinel.h"

using namespace std;

const DWORD MAX_GENERAL_POLL_TIME = 500;	// The maximum time to loop for general polls.
const DWORD PBT_SAS_TIMEOUT_TICK_COUNT = 30000; // Time in milliseconds to allow sas to complete a personal banker transfer before timing out.
const DWORD PBT_SAS_LOCK_REQUEST_TIMOUT_TICK_COUNT = 5000; // Time in milliseconds to wait for a pending lock request.
const DWORD MAX_AWARD_RETRY_TICKS = 20000; // Retry OneLink award for 20 seconds
const DWORD MAX_AWARD_FROM_PROMO_RETRY_TICKS = 30000; // Retry OneLink award from promo for 30 seconds

void CSASProtocol::DoGeneralPoll(void)
{
	lock_guard<recursive_mutex> lock(m_doGpMutex);
	CSASGeneralPoll *generalPollMsg = nullptr;

	DWORD currentTime = CUtilities::GetTickCount();
	DWORD generalPollMaxTime = currentTime + MAX_GENERAL_POLL_TIME;
	if ( generalPollMaxTime < currentTime )	// See if we tried to go past the end of current time
		generalPollMaxTime = 0xffffffff;		// Set it to the max value.

	bool bMoreExceptions = true;

	// We keep doing general polls until we have no more exceptions or we reached out time limit.
	while ( m_sas->GameComStatus() && currentTime < generalPollMaxTime && bMoreExceptions )
	{
		generalPollMsg = new CSASGeneralPoll(m_gameAddress);

		generalPollMsg->Fill();

		// Do the general poll.
		m_sasPollQueue->Add(generalPollMsg,QUEUE_PRIORITY_NORMAL);

		// Wait for a response from SAS
		generalPollMsg->WaitForResponse();

		// Get the exception value.
		GeneralPollException exceptionValue = generalPollMsg->GetExceptionValue();

		// Notify OneLink
		SendPollExceptionToInternals(exceptionValue);


		// See if there are no more exceptions
		if ( exceptionValue == GPE_NO_ACTIVITY || exceptionValue == GPE_NO_ACTIVITY_WAITING )
		{
			bMoreExceptions = false;
			CInternalMessage *msg = m_sas->ProcessReadQueueIsEmpty();
			if (msg != nullptr)
			{
				LogString("SASGENERAL", "SasStartupQueueEmpty true");
				msg->SetData(GetGameEndedMeters(), EGM_GAME_NUMBER);
				SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
			}
		}

		// Got an exception
		else
		{
			// See if we have a buffer overflow exception
			if ( exceptionValue == GPE_BUFFER_OVERFLOW )
			{
				ProcessGeneralPollBufferOverflow();
			}

			// Process this exception value
			else
			{
				ProcessGeneralPollException( exceptionValue );
			}
		}

		currentTime = CUtilities::GetTickCount();
		delete generalPollMsg;
	}

	DoGeneralPollTimers(currentTime);
}

void CSASProtocol::DoGeneralPollTimers(DWORD currentTime)
{
	// if pbt lock request has been pending for 5 seconds, try it again.
	if (m_sasState.GetLockPending() && m_sas->HasNoErrors() && (m_pbt != NULL || m_egmAward != NULL) && 
		currentTime - m_sasState.GetLastLockRequest() > PBT_SAS_LOCK_REQUEST_TIMOUT_TICK_COUNT)
	{
		bool pbtLockResent = false;
		{
			lock_guard<recursive_mutex> lock(m_pbtCritSection);
			if (m_pbt != NULL)
			{
				//TRACE(L"Resending lock request\n");
				ResendPBTLockRequest();
				pbtLockResent = true;
			}
		}

		// See if the award lock needs to be retried or canceled
		bool resendAwardLock = false;
		bool cancelAwardLock = false;		
		{
			lock_guard<recursive_mutex> lock(m_egmAwardCritSection);
			if (!pbtLockResent && m_egmAward != NULL && m_egmAward->InProgress())
			{
				DWORD awardRetryTicks(m_egmAward->IsFromPromoGame() ? MAX_AWARD_FROM_PROMO_RETRY_TICKS : MAX_AWARD_RETRY_TICKS);
				if (CUtilities::GetMillisecondsSinceTimestamp(m_egmAward->GetReceivedFromSystemTime()) < awardRetryTicks)
				{
					resendAwardLock = true;
				}
				else
				{
					cancelAwardLock = true;
				}
			}
		}

		if (resendAwardLock)
		{
			// Retry the award lock
			ResendAwardLockRequest();
		}
		else if (cancelAwardLock)
		{
			// Cancel lock request and mark award ineligible
			m_sasState.SetLockPending(false);
			CancelPBLock();
			ReturnAwardIneligible();
		}
	}

	// Check to see if there is a pbt transaction timed out, don't attempt this if game comm appears to be down
	if (m_sas->HasNoErrors() && m_pbt != NULL && currentTime - m_pbtStartTickCount > PBT_SAS_TIMEOUT_TICK_COUNT)
	{
		//TR 28746 check this inside the critical section because it was causing a race condition
		//we don't want to move this outside the above if because this thread fire frequently which would cause the lock to lock and unlock too often.
		lock_guard<recursive_mutex> lock(m_pbtCritSection);
		if(m_pbt != NULL)
		{
			CancelSASPBTransfer();
		}
	}
}

void CSASProtocol::SendPollExceptionToInternals(GeneralPollException exceptionValue)
{
	if (exceptionValue != GPE_NO_ACTIVITY &&
		exceptionValue != GPE_NO_ACTIVITY_WAITING &&
		exceptionValue != GPE_GAME_STARTED &&
		exceptionValue != GPE_GAME_ENDED &&
		exceptionValue != GPE_GAME_SELECTED)
	{
		CInternalMessage * msg = new CInternalMessage(INT_MSG_GENERAL_POLL_EXCEPTION, (WORD)exceptionValue);
		
		// FROM here to internals
		SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
	}
}


void CSASProtocol::ProcessGeneralPollException( GeneralPollException exceptionValue )
{
	CheckIfPendingAutoUploadNeedsBeCancelled(exceptionValue);

	switch ( exceptionValue )
	{
	case GPE_SLOTDOOR_OPENED:
		SendDoorOpenToInternals(mSDr);
		break;
	case GPE_DROPDOOR_OPENED:
		SendDoorOpenToInternals(mDDr);
		break;
	case GPE_CARDCAGE_OPENED:
		SendDoorOpenToInternals(mLDr);
		break;
	case GPE_CASHBOX_OPENED:
		SendDoorOpenToInternals(mCDr);
		break;
	case GPE_BELLYDOOR_OPENED:
		SendDoorOpenToInternals(mBDr);
		break;
	case GPE_SLOTDOOR_CLOSED:
		SendDoorClosedToInternals(mSDr);
		break;
	case GPE_DROPDOOR_CLOSED:
 		SendDoorClosedToInternals(mDDr);
		break;
    case GPE_CARDCAGE_CLOSED:
		SendDoorClosedToInternals(mLDr);
		break;
    case GPE_CASHBOX_CLOSED:
		SendDoorClosedToInternals(mCDr);
		break;
    case GPE_BELLYDOOR_CLOSED:
		SendDoorClosedToInternals(mBDr);
		break;
	case GPE_POWER_ON:
		SendTiltToInternals( tltGamePowerOn );
		break;
	case GPE_POWER_OFF:
		SendTiltToInternals( tltGamePowerOff );
		break;
	case GPE_GENERAL_TILT:
		SendTiltToInternals( tltGameUnknown );
		break;
	case GPE_COIN_IN_TILT:
		SendTiltToInternals( tltCiJam );
		break;
	case GPE_COIN_OUT_TILT:
		SendTiltToInternals( tltCoJam );
		break;
	case GPE_HOPPER_EMPTY:
		SendTiltToInternals( tltHopEmpty );
		break;
	case GPE_EXTRA_COIN_PAID:
		SendTiltToInternals( tltCoOverpay );
		break;
	case GPE_DIVERTER_MALFUNC:
		SendTiltToInternals( tltCiDiverter );
		break;
	case GPE_CASHBOX_FULL:
		SendTiltToInternals( tltBillFull );
		break;
	case GPE_CASHBOX_REMOVED:
		SendTiltToInternals( tltBillStackerOut );
		break;
	case GPE_CASHBOX_INSTALLED:
		SendTiltToInternals( tltBillStackerIn );
		break;
	case GPE_BILL_JAM:
		SendTiltToInternals( tltBillJam );
		break;
	case GPE_BILL_ACCEPTOR_HW_FAIL:
		SendTiltToInternals( tltBillHardFail );
		break;
	case GPE_REVERSE_BILL:
		SendTiltToInternals( tltBillUnknown );
		break;
	case GPE_BILL_REJECTED:
		SendTiltToInternals( tltBillUnknown );
		break;
	case GPE_COUNTERFEIT_BILL:
		SendTiltToInternals( tltBillUnknown );
		break;
	case GPE_REVERSE_COIN:
		SendTiltToInternals( tltCiYoyo );
		break;
	case GPE_RAM_ERR_DATA_RECOV:
		SendTiltToInternals( tltGameRamRecover );
		break;
	case GPE_RAM_ERR_NO_DATA_RECOV:
		SendTiltToInternals( tltGameRamIrrecover );
		break;
	case GPE_RAM_ERR_BAD_DEVICE:
		SendTiltToInternals( tltGameRamHardFail );
		break;
	case GPE_EEPROM_ERR_DATA:
		SendTiltToInternals( tltGameEepromRecover );
		break;
	case GPE_EEPROM_ERR_BAD_DEVICE:
		SendTiltToInternals( tltGameEepromHardFail );
		break;
	case GPE_EPROM_ERR_DIFF_CHKSM:
		SendTiltToInternals( tltGameRomWarning );
		break;
	case GPE_EPROM_ERR_BAD_CHKSM:
		SendTiltToInternals( tltGameRomError );
		break;
	case GPE_PEPROM_ERR_DIFF_CHKSM:
		SendTiltToInternals( tltGameRomWarning );
		break;
	case GPE_PEPROM_ERR_BAD_CHKSM:
		SendTiltToInternals( tltGameRomError );
		break;
	case GPE_MEMORY_ERR_RESET:
		SendTiltToInternals( tltGameUnknown );
		break;
	case GPE_LOW_BATTERY:
		SendTiltToInternals( tltGameRamBattery );
		break;
	case GPE_OPTIONS_CHANGED:
		LogString(GAMES, "GPE_OPTIONS_CHANGED");
		GetGameList(true, false);
		// Tracker # 22847
		SendTiltToInternals( tltOpChangedOption );
		break;
	case GPE_TICKET_CASHOUT:
		LogString(SASGENERAL, "[%s] GPE_TICKET_CASHOUT", __FUNCTION__);
		if (m_sasState.GetTicketingOutEnabled())
			GetPrintedTicketInfo(true);
		break;
	case GPE_HANDPAY_VALIDATED:
		GetPrintedTicketInfo(false);
		break;
	case GPE_VALIDATION_NO_CONFIG:
		ProcessInitTicketingRequest();
		break;
	case GPE_1_DOLLAR_ACCEPTED:
		GetBillMeters( mD1 );
		break;
    case GPE_5_DOLLAR_ACCEPTED:
		GetBillMeters( mD5 );
		break;
    case GPE_10_DOLLAR_ACCEPTED:
		GetBillMeters( mD10 );
		break;
    case GPE_20_DOLLAR_ACCEPTED:
		GetBillMeters( mD20 );
		break;
    case GPE_50_DOLLAR_ACCEPTED:
		GetBillMeters( mD50 );
		break;
    case GPE_100_DOLLAR_ACCEPTED:
		GetBillMeters( mD100 );
		break;
    case GPE_2_DOLLAR_ACCEPTED:
		GetBillMeters( mD2 );
		break;
    case GPE_500_DOLLAR_ACCEPTED:
		GetBillMeters( mD500 );
		break;
    case GPE_BILL_ACCEPTED:
		GetBillMeters();	// Get all the bill meters.
		break;
    case GPE_200_DOLLAR_ACCEPTED:
		GetBillMeters( mD200 );
		break;
	case GPE_REEL_TILT:
		SendTiltToInternals( tltReelUnknown );
		break;
	case GPE_REEL_1_TILT:
		SendTiltToInternals( tltReelJam1 );
		break;
	case GPE_REEL_2_TILT:
		SendTiltToInternals( tltReelJam2 );
		break;
	case GPE_REEL_3_TILT:
		SendTiltToInternals( tltReelJam3 );
		break;
	case GPE_REEL_4_TILT:
		SendTiltToInternals( tltReelJam4 );
		break;
	case GPE_REEL_5_TILT:
		SendTiltToInternals( tltReelJam5 );
		break;
	case GPE_REEL_DISCONNECT:
		SendTiltToInternals( tltReelRemoved );
		break;
	case GPE_HANDPAY_PENDING:
		LogString(SASGENERAL, "[%s] GPE_HANDPAY_PENDING", __FUNCTION__);
		ProcessGeneralPollHandpayPending();
		break;
	case GPE_PROG_WIN:
		ProcessGeneralPollProgressiveMachinePay();
		break;
	case GPE_SAS_PROG_LEVEL_HIT:
		ProcessGeneralPollProgressiveJackpot();
		break;
	case GPE_HANDPAY_RESET:
		SendTiltToInternals( tltJckpotReset );
		ProcessGeneralPollHandpayReset();
		break;
	case GPE_NO_PROG_5_SECS:
		SendTiltToInternals( tltProgUnknown );
		break;
	case GPE_PRINTER_COMM_ERR:
		SendTiltToInternals( tltPrnHardFail );
		break;
	case GPE_PRINTER_PAPER_OUT:
		SendTiltToInternals( tltPrnEmpty );
		break;
	case GPE_TICKET_INSERTED:
		LogString(TICKETING_LOG, "GPE_TICKET_INSERTED TicketingInEnabled=%u", m_sasState.GetTicketingInEnabled());
		if (m_sasState.GetTicketingInEnabled())
			ProcessTicketInsertedGP( );
		break;
	case GPE_TICKET_XFER_COMPLETE:
		LogString(TICKETING_LOG, "GPE_TICKET_XFER_COMPLETE");
		ProcessFinalTicketStatus( );
		break;
	case GPE_PRINTER_PAPER_LOW:
		SendTiltToInternals( tltPrnLow );
		break;
	case GPE_PRINTER_POWER_OFF:
		SendTiltToInternals( tltPrnHardFail );
		break;
	case GPE_PRINTER_POWER_ON:
		SendTiltToInternals( tltPrnHardFail );
		break;
	case GPE_REPLACE_PRINTER_RIBBON:
		SendTiltToInternals( tltPrnHardFail );
		break;
	case GPE_PRINTER_CARRIAGE_JAM:
		SendTiltToInternals( tltPrnJam );
		break;
	case GPE_GAME_STARTED:
		ProcessGeneralPollGameStarted();
		break;
	case GPE_GAME_ENDED:
		ProcessGeneralPollGameEnded();
		break;
	case GPE_GAME_SELECTED:
		ProcessGeneralPollGameSelected();
		break;
	case GPE_HOPPER_FULL:
		SendTiltToInternals( tltHopUnknown );
		break;
	case GPE_HOPPER_LEVEL_LOW:
		SendTiltToInternals( tltHopLow );
		break;
	case GPE_GAME_LOCKED:
		ProcessGeneralPollGameLocked();
		break;
	case GPE_AFT_XFER_COMPLETE:
		ProcessGeneralPollXferComplete();
		break;
	case GPE_DISP_MENU_ENTER:
		m_bInUserMenu = true;	
		break;
	case GPE_DISP_MENU_EXIT:
		m_bInUserMenu = false;	
		break;
	case GPE_SELF_TEST_ENTER:
		m_bInSelfTest = true;
		break;
	case GPE_SELF_TEST_EXIT:
		m_bInSelfTest = false;
		break;
	case GPE_METER_CHANGE_PENDING:
		ProcessMeterChangePending();
		break;
	case GPE_METER_CHANGE_CANCELLED:
		ProcessMeterChangeCancelled();
		break;
	case GPE_ENABLED_GAMES_DENOM_CHANGED:
	case GPE_SOFT_METERS_RESET_TO_ZERO:
		ProcessEnabledGamesDenomChanged();
		break;
	case GPE_CHANGE_LAMP_ON:
		ProcessChangeLightOn();
		break;
	case GPE_CHANGE_LAMP_OFF:
		ProcessChangeLightOff();
		break;
	case GPE_CASHOUT_PRESSED:
		LogString(SASGENERAL, "[%s] GPE_CASHOUT_PRESSED", __FUNCTION__);
		SendTiltToInternals(tltCashoutBtnPressed);
		break;
	case GPE_AFT_REQ_HOST_CASHOUT:
		ProcessGeneralPollCashoutToHost();
		break;
	case GPE_AFT_REQ_HOST_CASHOUT_WIN:
		LogString(SASGENERAL, "[%s] GPE_AFT_REQ_HOST_CASHOUT_WIN", __FUNCTION__);
		break;
	default:
		break;
	}
}


void CSASProtocol::ProcessGeneralPollBufferOverflow(void)
{
	// TBD.
	// We are supposed to do a long poll 30 to get the list of overflowed exceptions.
	// However, this is pre-SAS 5.0. Do we really need to do this?
}

void CSASProtocol::ProcessGeneralPollGameStarted(void)
{
	WORD gameNumber = m_gameList.GetCurrentGame();
	int denom = m_gameList.GetCurrentDenom();
	CGame game(MEMORY_NONE);
	game.SetGameNumber(gameNumber);
	__int64 maxBet = 0;
	if (GetInfoFromLongPollB5(game))
	{
		maxBet = game.GetMaxBet();
		if (maxBet == 0xFF)
		{
			// This means LP B5 failed, LP 53 was used to get maxbet and maxbet is >= 255.
			maxBet = 0;
		}
		maxBet = maxBet * denom;
	}
	LogString(GENERAL, "ProcessGeneralPollGameStarted() : GameNumber = %d, Denom = %d, MaxBet = %lld", gameNumber, denom, maxBet);

	// We want to get the coin in meter at this time.
	PokerHand *pokerHand = GetPokerHand(CLongPoll8EMessage::DealtHand);

	CMeters *meters = new CMeters(MEMORY_NONE);
	meters->SetMeter(gCI);

	// don't waste time trying to get these for SAS 3/4
	if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx)
	{
		meters->SetMeter(mRstrPlayd); // NCEP Played
	}


	GetMeters(*meters, EGM_GAME_NUMBER, false, true);

	// Update current player denomination and game selected as needed before sending game start message to Internals.
	CheckCurrentGameDenom(true);

	CInternalMessage *msg = new CInternalMessage(INT_MSG_NOTIFY_GAME_START, meters, EGM_GAME_NUMBER);
	msg->SetData((DWORD)m_sas->GetSasStartupQueueEmpty());
	msg->SetData(pokerHand);
	msg->SetData((__int64)maxBet);

	SendMessageToInternals( msg, QUEUE_PRIORITY_NORMAL );
}

CMeters *CSASProtocol::GetGameEndedMeters(void)
{
	// We have to get the credit meters at this time.
	CMeters *meters = new CMeters(MEMORY_NONE);
	meters->SetMeter(mCrd);
	meters->SetMeter(gCI);
	meters->SetMeter(gCO);
	meters->SetMeter(mBonusAtt);
	meters->SetMeter(mBonusMach);
	meters->SetMeter(gJP);
	meters->SetMeter(mPrgPayMac);
	meters->SetMeter(mPrgPayAtt);

	// don't waste time trying to get these for SAS 3/4
	if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx)
	{
		meters->SetMeter(mRstrPlayd); // NCEP Played
	}

	GetMeters(*meters, EGM_GAME_NUMBER, false, true);

	ProcessGameEndedAccountingOnlyLink(*meters);

	return meters;
}

/// <summary>
/// Process the game ended meters if there are accounting only links.
/// </summary>
/// <param name="meters">IN - The game ended meters.</param>
/// <remarks>
/// What we want is an efficient way to get the cumulative progressive wins meter (machine paid + attendant paid).
/// It is messy to add an oasisMeters enum that is not sent to Oasis FloorLogix.
/// It is even messier to use GetMeters() for an oasisMeters enum that has special requirements,
///     such as either being calculated from two existing meters or being obtained using an additional long poll (0x83).
/// So, handle the high level meter processing from here.
/// </remarks>
void CSASProtocol::ProcessGameEndedAccountingOnlyLink(const CMeters &meters)
{
	if (m_accountingOnlyLinks)
	{
		// Did we get the cumulative progressive wins amount with the normal game ended meters?
		const CMeter *machinePaidProgressiveWin(meters.GetMeterByNumber(mPrgPayMac));
		const CMeter *attendantPaidProgressiveWin(meters.GetMeterByNumber(mPrgPayAtt));
		if (machinePaidProgressiveWin != nullptr && machinePaidProgressiveWin->GetValue() != INVALID_METER_VALUE &&
			attendantPaidProgressiveWin != nullptr && attendantPaidProgressiveWin->GetValue() != INVALID_METER_VALUE)
		{
			__int64 amount(machinePaidProgressiveWin->GetValue() + attendantPaidProgressiveWin->GetValue());

			// Adjust the amount so it is within the range of long poll 0x83,
			// So, we don't have (large) deltas, in case we alternate between the sources for this meter.
			__int64 adjustedAmount = amount;
			if (m_denomMult != 0)
			{
                if (adjustedAmount > CLongPoll83Message::MAXIMUM_CUMULATIVE_PROGRESSIVE_WINS_AMOUNT_IN_CREDITS) 
                {
                    adjustedAmount %= (CLongPoll83Message::MAXIMUM_CUMULATIVE_PROGRESSIVE_WINS_AMOUNT_IN_CREDITS + 1);
                }
                adjustedAmount *= m_denomMult;
			}
			LogString(SASGENERAL, _T("GameEnded CumulativeProgressiveWin amount=%lld adjustedAmount=%lld"), (long long)amount, (long long)adjustedAmount);
			SendCumulativeProgressiveWinToInternals(adjustedAmount);
		}
		else
		{
			// Get the cumulative progressive wins amount (using long poll 0x83).
			GetCumulativeProgressiveWin();
		}
    }
}

void CSASProtocol::ProcessGeneralPollGameEnded(void)
{
	LogString(GAMES, "[%s] GameNumber: %d", __FUNCTION__,
		m_gameList.GetCurrentGame());

	// Ensure subgame meters are notified to the internals first
	SendSubGameMeters(m_gameList.GetCurrentGame(), false, false);

	PokerHand *pokerHand = GetPokerHand(CLongPoll8EMessage::FinalHand);
	CMeters *meters = GetGameEndedMeters();

	CInternalMessage *msg = new CInternalMessage( INT_MSG_NOTIFY_GAME_END , meters, EGM_GAME_NUMBER );
	msg->SetData((DWORD)m_sas->GetSasStartupQueueEmpty());
	msg->SetData(pokerHand);

	SendMessageToInternals( msg, QUEUE_PRIORITY_NORMAL );
}

PokerHand *CSASProtocol::GetPokerHand(CLongPoll8EMessage::PokerHandType pokerhandType)
{
	PokerHand *pPokerHand = new PokerHand;

	if (m_queryPokerhand)
	{
		CLongPoll8EMessage lp(m_gameAddress, pokerhandType);
		lp.Fill();

		m_sasPollQueue->Add(&lp, QUEUE_PRIORITY_HIGH);
		lp.WaitForResponse();

		if (lp.GetReturnValue() == SASPOLL_SUCCESS && lp.GetPokerHand().IsValid())
		{
			*pPokerHand = lp.GetPokerHand();
		}
	}

	return pPokerHand;
}

void CSASProtocol::ProcessGeneralPollGameSelected(void)
{
	SendCurrentSelectedGameToInternals(true);
	SendGameSessionIdIfRequiredToInternals();
}

void CSASProtocol::SendCurrentSelectedGameToInternals(bool gameSelected)
{
	UpdateCurrentGame(gameSelected);
	UpdateCurrentDenom(gameSelected);
}

void CSASProtocol::SendGameSessionIdIfRequiredToInternals()
{
	CInternalMessage *gameMsg = new CInternalMessage( INT_MSG_INCREMENT_GAME_SESSION_ID_IF_REQUIRED );
	SendMessageToInternals( gameMsg, QUEUE_PRIORITY_NORMAL );
}

/// <summary>
/// Gets the current game number and sends the information to Internals.
/// </summary>
/// <param name="gameSelected">IN - true if this update is in response to a Game Selected exception.</param>
void CSASProtocol::UpdateCurrentGame(bool gameSelected)
{
	int currentGame(EGM_GAME_NUMBER);
	bool isValid = GetSelectedGameNumber(currentGame);
	m_gameList.SetCurrentGame(isValid, gameSelected, currentGame);
	currentGame = m_gameList.GetCurrentGame();

	// Send the currently selected game to internals as a meter
	CMeters *selGameMeters = new CMeters(MEMORY_NONE);
	selGameMeters->SetMeter( mSelG, currentGame );	
	CInternalMessage *selGameMsg = new CInternalMessage( INT_MSG_SET_METERS, selGameMeters, EGM_GAME_NUMBER );
	SendMessageToInternals( selGameMsg, QUEUE_PRIORITY_NORMAL );
}

/// <summary>
/// Gets the current player denomination and sends the information to Internals.
/// </summary>
/// <param name="gameSelected">IN - true if this update is in response to a Game Selected exception.</param>
void CSASProtocol::UpdateCurrentDenom(bool gameSelected)
{
	byte denomCode(0);
	bool isValid = GetCurrentDenom(denomCode);
	m_gameList.SetCurrentDenomCode(isValid, gameSelected, denomCode);
	int currentGameDenom(m_gameList.GetCurrentDenom());

	// Send current game denom to internals
	CInternalMessage *denomMsg = new CInternalMessage(INT_MSG_SET_CURRENT_GAME_DENOM, currentGameDenom > 0 ? (DWORD)currentGameDenom : (DWORD)m_denomMult);
	SendMessageToInternals(denomMsg, QUEUE_PRIORITY_NORMAL);
}

/// <summary>
/// Updates the current game number and current player denomination as needed.
/// </summary>
/// <param name="gameStarted">IN - true if processing a Game Started exception, false otherwise.</param>
/// <remarks>
/// This method should not be called to process a Game Selected exception.
/// </remarks>
void CSASProtocol::CheckCurrentGameDenom(bool gameStarted)
{
	if (m_gameList.ShouldUpdateCurrentGame(gameStarted, m_sas->GetPollingSpeed()))
	{
		UpdateCurrentGame(false);
	}

	if (m_gameList.ShouldUpdateCurrentDenom(gameStarted, m_sas->GetPollingSpeed()))
	{
		UpdateCurrentDenom(false);
	}
}

void CSASProtocol::ProcessInitTicketingRequest( void )
{
	// If we don't have a valid SlotMastID then our configuration isn't good enough to update the EGM. 
	if (m_sasState.GetSlotmastID() != INVALID_SLOT_MAST_ID)
	{
		// Send info to be printed on tickets, before sending the seed information necessary for printing.
		SetTicketValidationParms();

		// query for initial sequence number to make sure that new sequence number is different
		CLongPoll4CMessage *lp4ci = new CLongPoll4CMessage( m_gameAddress );
		lp4ci->SetMachineValidationID( 0 ); // A validation ID of 0 indicates we are getting the validation ID
		lp4ci->SetValidationSequenceNumber(0);
		lp4ci->Fill();

		// Add to SAS poll queue at high priority
		// This is high priority because we might be getting this because or ticket data is out of sync.
		m_sasPollQueue->Add(lp4ci, QUEUE_PRIORITY_HIGH);
		lp4ci->WaitForResponse();
		if ( lp4ci->GetMachineValidationID() != 0 )
		{
            lp4ci->GetValidationSequenceNumber();
		}
	
		byte sequenceNumber[4]; // 4th byte unused, treat as DWORD for parameter

		// Init squence number to value returend by 4c poll.
		DWORD *dwSequence = (DWORD *)sequenceNumber;
		*dwSequence= lp4ci->GetValidationSequenceNumber();
				
		// set new values
		DWORD machineID = m_ticketSeed->GetMachineID(m_sasState.GetSlotmastID(), m_sasState.GetCasinoNumber());

		delete lp4ci;

		// Always reset value
		m_ticketSeed->IncAndRandSequence( sequenceNumber, true );

		CLongPoll4CMessage lp4c( m_gameAddress );

		lp4c.SetMachineValidationID( machineID ); 
		lp4c.SetValidationSequenceNumber( *(DWORD *)sequenceNumber );
		lp4c.Fill();

		// Add to SAS poll queue at high priority
		// This is high priority because we might be getting this because our ticket data is out of sync.
		m_sasPollQueue->Add(&lp4c, QUEUE_PRIORITY_HIGH);
		lp4c.WaitForResponse();

	}
}

void CSASProtocol::ProcessChangeLightOn()
{
	CInternalMessage * msg = new CInternalMessage(INT_MSG_CHANGE_LIGHT_ON);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CSASProtocol::ProcessChangeLightOff()
{
	CInternalMessage * msg = new CInternalMessage(INT_MSG_CHANGE_LIGHT_OFF);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CSASProtocol::GetPrintedTicketInfo(bool sendMeterUpdate304)
{
	LogString(TICKETING_LOG, "GetPrintedTicketInfo(), m_bIsEnhancedTicketing=%d sendMeterUpdate304=%d",
		m_sasState.GetIsEnhancedTicketing(), sendMeterUpdate304);

	if (m_sasState.GetIsEnhancedTicketing() && !m_sasState.GetIgnoreTicketPrinted())
	{
		m_sasState.SetIgnoreTicketPrinted(true);

		// Peek at first unread ticket
		CLongPoll4DMessage lp4d( m_gameAddress );
		lp4d.SetPeekOption( true );
		lp4d.Fill();
		// Add to SAS poll queue at high priority
		m_sasPollQueue->Add(&lp4d, QUEUE_PRIORITY_HIGH);
		lp4d.WaitForResponse();

		if (lp4d.GetReturnValue() == SASPOLL_SUCCESS)
		{
			CTicket *ticket = lp4d.GetResponseTicket();
			m_sasState.SetLastPrintedTicket(new CTicket(*ticket));
			if (ticket->ActionCode() != TicketActionCodeNoValue)
			{
				// Make sure ticket amount is less than max value
				if (!ticket->IsAmountLessThanMax())
				{
					// log and send tilt
					SendTiltToInternals(tltVoucherOvrLmtAftr);

					CDiagnostics::ReportPBTTicketDiagnosticToFile(CDirectoryManager::PBTTicketDiagnosticsFilePath(), CUtilities::LoadResourceString(IDS_TKT_PRINT_AMOUNT_OVER_LIMIT_AFTER), ticket->ToStringForLog(), tltVoucherOvrLmtAftr);
				}
				ticket->SetSequenceNumber(m_ticketSequenceNumber++);

				// New ticket, send INT_MSG_ADD_TICKET
				CInternalMessage *msg = new CInternalMessage(INT_MSG_SEND_TICKET,ticket);
				SendMessageToInternals( msg, QUEUE_PRIORITY_HIGHEST ); 

				// Tracker #19641 - get ticket meters here
				GetTicketMeters(sendMeterUpdate304);

			}
			else
			{
				// This ticket/receipt isn't something we need to keep
				m_sasState.SetIgnoreTicketPrinted(false);
				ClearTicketOut();
				delete ticket;
			}

			// cause reset of validation sequence number
			GetTicketValidationData( true );
	
		}
		else
			m_sasState.SetIgnoreTicketPrinted(false); // nothing going to internals, make sure future vouchers are not ignored
	}

	else if (!m_sasState.GetIgnoreTicketPrinted())
	{
		CLongPoll3DMessage lp3d( m_gameAddress );

		lp3d.Fill();

		// Add to SAS poll queue at normal priority
		m_sasPollQueue->Add(&lp3d, QUEUE_PRIORITY_NORMAL);

		// Wait for a response from SAS
		lp3d.WaitForResponse();
		if (lp3d.GetReturnValue() == SASPOLL_SUCCESS)
		{
			CTicket *ticket = lp3d.GetResponseTicket();

			// New ticket, send INT_MSG_ADD_TICKET
			CInternalMessage *msg = new CInternalMessage(INT_MSG_SEND_TICKET,ticket);
			SendMessageToInternals( msg, QUEUE_PRIORITY_HIGHEST ); 

			// Tracker #19641 - get ticket meters here
			GetTicketMeters(sendMeterUpdate304);

			// cause reset of validation sequence number
			GetTicketValidationData( true );
	
		}
	}
}

void CSASProtocol::SendDoorOpenToInternals(oasisMeters meterNumber)
{
	CInternalMessage * msg = new CInternalMessage(INT_MSG_DOOR_OPEN, (WORD)meterNumber);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}
	
void CSASProtocol::SendDoorClosedToInternals(oasisMeters meterNumber)
{
	CInternalMessage * msg = new CInternalMessage(INT_MSG_DOOR_CLOSED, (WORD)meterNumber);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CSASProtocol::ProcessHandpayPending()
{
	// get some meters
	CMeters *meters = new CMeters(MEMORY_NONE);
	meters->SetMeter(gCI);
	meters->SetMeter(gCO);
	meters->SetMeter(gJP);
	meters->SetMeter(mTHP);
	meters->SetMeter(mHCC);
	
	GetMeters(*meters,EGM_GAME_NUMBER, false, true);

	CLongPoll1BMessage lp1b(m_gameAddress);
	lp1b.Fill();
	m_sasPollQueue->Add(&lp1b,QUEUE_PRIORITY_HIGHEST);
	lp1b.WaitForResponse();

	if (lp1b.GetReturnValue() == SASPOLL_SUCCESS && !lp1b.isEmpty())
	{
		CJackpot *jackpot = new CJackpot(MEMORY_NONE);
		jackpot->SetProgressiveLevel(-1);
		switch(lp1b.m_progressiveLevel)
		{
		case 0x00:
		case 0x40:
			jackpot->SetJackpotAmount(lp1b.m_amount * m_denomMult);
			jackpot->SetJackpotType(JT_HANDPAY);
			if (lp1b.m_progressiveLevel == 0x40) // non-progressive top award
				jackpot->SetJackpotType(JT_TOPAWARD);
			break;
		case 0x80:
			jackpot->SetJackpotAmount(lp1b.m_amount * m_denomMult);
			jackpot->SetJackpotType(JT_CANCEL_CREDIT);
			break;
		}

		if (lp1b.m_progressiveLevel >= 0x01 && lp1b.m_progressiveLevel <= 0x20)
		{
			LogString(SASGENERAL, "CSASProtocol: 1B response received with progressive level between 0x01 and 0x20.");
			// progressive jackpot, amount is in pennies
			jackpot->SetJackpotType(JT_NONE);
			jackpot->SetJackpotAmount(lp1b.m_amount);
			jackpot->SetProgressiveLevel(lp1b.m_progressiveLevel);
		}

		jackpot->SetJackpotTransactionType(JTRANS_COMPOSITE);
		jackpot->SetKeyToCreditAvailable(lp1b.KeyToCreditAvailable());

		CInternalMessage *msg = new CInternalMessage(INT_MSG_JACKPOT_INFO,jackpot);
		msg->SetData(meters,EGM_GAME_NUMBER);
		SendMessageToInternals(msg,QUEUE_PRIORITY_NORMAL);

		// Enable reset to credit meter if appropriate
		if (lp1b.KeyToCreditAvailable() && m_resetToCredit)
		{
			ResetToCredit(true);
		}
		else
		{
			ResetToCredit(false);
		}
	}
}

void CSASProtocol::ProcessGeneralPollCashoutToHost()
{
	if (m_bIsAFTSupported)
	{
		LogString(SASGENERAL, "[%s] Sending LP74 to know the credits on the EGM...", __FUNCTION__);
		CLongPoll74Message lp74(m_gameAddress);
		lp74.SetLockCode(LOCK_CODE_QUERY_STATUS);
		lp74.SetLockTimeout(0);
		lp74.Fill();
		m_sasPollQueue->Add(&lp74, QUEUE_PRIORITY_HIGHEST);
		lp74.WaitForResponse();

		if (SASPOLL_SUCCESS != lp74.GetReturnValue())
		{
			LogString(SASGENERAL, "[%s] LP74 FAILED 0x%x, abort processing cashout to host", __FUNCTION__, lp74.GetReturnValue());
			return;
		}

		// Update asset number so that we can use it if cashout to host cancelation is needed going forward.
		m_assetNumber = lp74.m_assetNumber;

		DWORD cashBalance = lp74.m_currentCashableAmount;
		DWORD cashPromoBalance = lp74.m_currentNonrestrictedAmount;
		DWORD promoBalance = lp74.m_currentRestrictedAmount;

		CInternalMessage *msg = new CInternalMessage(INT_MSG_CASHOUT_TO_HOST, cashBalance, cashPromoBalance);
		msg->SetData((WORD)1);  // 1: process, 0: cancel processing
		msg->SetData((__int64)promoBalance);
		SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
	}
}

void CSASProtocol::ProcessProgressiveMachinePay()
{
	LogString(PROGRESSIVE, "ProcessProgressiveMachinePay()");
	LogString(SASGENERAL, "ProcessProgressiveMachinePay()");
	CLongPoll84Message lp84(m_gameAddress);
	lp84.Fill();
	m_sasPollQueue->Add(&lp84,QUEUE_PRIORITY_HIGHEST);
	lp84.WaitForResponse();

	if (lp84.GetReturnValue() == SASPOLL_SUCCESS)
	{
		//Send to Internals so it gets the award to the award manager.
		if(lp84.m_amount > 0 &&
			lp84.m_progressiveLevel >= 0x01 && lp84.m_progressiveLevel <= 0x20)
		{
			CJackpot *jackpot = new CJackpot(MEMORY_NONE);
			jackpot->SetJackpotType(JT_HANDPAY);
			jackpot->SetJackpotAmount(lp84.m_amount);
			jackpot->SetProgressiveLevel(lp84.m_progressiveLevel);
			jackpot->SetJackpotTransactionType(JTRANS_PROGRESSIVE_HOPPER_PAID);
			CInternalMessage *msg = new CInternalMessage(INT_MSG_PROGRESSIVE_HOPPER_PAID_INFO, jackpot);
			SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);

			CEGMAward *progressiveWin = new CEGMAward();

			SYSTEMTIME currentTime;
			CUtilities::GetUTCSystemTime(&currentTime);
			progressiveWin->SetReceivedFromEGMTime(currentTime);

			progressiveWin->SetAwardAmountHit(lp84.m_amount);
			progressiveWin->SetSASLevelId(lp84.m_progressiveLevel);
			progressiveWin->SetAwardState(ReceivedFromGame);
			progressiveWin->SetAwardPaidType(EGMAwardPaidtype_Machine);
			progressiveWin->setEgmPaidSignaled(true);
			CInternalMessage *msg2 = new CInternalMessage(INT_MSG_PROGRESSIVE_WIN, progressiveWin);
			SendMessageToInternals(msg2, QUEUE_PRIORITY_NORMAL);
		}
	}
}

void CSASProtocol::ProcessProgressiveJackpot()
{
	LogString(PROGRESSIVE, "ProcessProgressiveJackpot()");
	LogString(SASGENERAL, "ProcessProgressiveJackpot()");
	CLongPoll85Message lp85(m_gameAddress);
	lp85.Fill();
	m_sasPollQueue->Add(&lp85,QUEUE_PRIORITY_HIGHEST);
	lp85.WaitForResponse();

	CJackpot *jackpot = new CJackpot(MEMORY_NONE);
	jackpot->SetJackpotType(JT_HANDPAY);
	if (lp85.GetReturnValue() == SASPOLL_SUCCESS)
	{
		LogString(PROGRESSIVE, "ProcessProgressiveJackpot(), m_amount = %lld, m_progressiveLevel %d", (long long)lp85.m_amount, (int)lp85.m_progressiveLevel);
		LogString(SASGENERAL, "ProcessProgressiveJackpot(), m_amount = %lld, m_progressiveLevel %d", (long long)lp85.m_amount, (int)lp85.m_progressiveLevel);

		jackpot->SetJackpotAmount(lp85.m_amount);
		jackpot->SetProgressiveLevel(lp85.m_progressiveLevel);

		jackpot->SetJackpotTransactionType(JTRANS_PROGRESSIVE_SCANNER_ONLY);
		CInternalMessage *msg = new CInternalMessage(INT_MSG_PROGRESSIVE_WIN_INFO,jackpot);
		SendMessageToInternals(msg,QUEUE_PRIORITY_NORMAL);

		//Send to Internals so it gets the award to the award manager.
		CEGMAward *progressiveWin = new CEGMAward();

		SYSTEMTIME currentTime;
		CUtilities::GetUTCSystemTime(&currentTime);
		progressiveWin->SetReceivedFromEGMTime(currentTime);

		progressiveWin->SetAwardAmountHit(lp85.m_amount);
		progressiveWin->SetSASLevelId(lp85.m_progressiveLevel);
		progressiveWin->SetAwardState(ReceivedFromGame);
		progressiveWin->SetAwardPaidType(EGMAwardPaidType_Unknown);
		CInternalMessage *msg2 = new CInternalMessage(INT_MSG_PROGRESSIVE_WIN, progressiveWin);
		SendMessageToInternals(msg2, QUEUE_PRIORITY_NORMAL);
	}
}

void CSASProtocol::ProcessGeneralPollHandpayPending()
{
	LogString(PROGRESSIVE, "ProcessGeneralPollHandpayPending()");
	LogString(SASGENERAL, "ProcessGeneralPollHandpayPending()");
	CInternalMessage *msg = new CInternalMessage(INT_MSG_JACKPOT_HIT);
	SendMessageToInternals(msg,QUEUE_PRIORITY_NORMAL);
}

void CSASProtocol::ProcessGeneralPollProgressiveMachinePay()
{
	LogString(PROGRESSIVE, "ProcessGeneralPollProgressiveMachinePay()");
	LogString(SASGENERAL, "ProcessGeneralPollProgressiveMachinePay()");

	CInternalMessage *msg = new CInternalMessage(INT_MSG_PROGRESSIVE_HOPPER_PAID_HIT);
	SendMessageToInternals(msg,QUEUE_PRIORITY_NORMAL);
}

void CSASProtocol::ProcessGeneralPollProgressiveJackpot()
{
	LogString(PROGRESSIVE, "ProcessGeneralPollProgressiveJackpot()");
	LogString(SASGENERAL, "ProcessGeneralPollProgressiveJackpot()");

	CInternalMessage *msg = new CInternalMessage(INT_MSG_PROGRESSIVE_WIN_HIT);
	SendMessageToInternals(msg,QUEUE_PRIORITY_NORMAL);
}

void CSASProtocol::ProcessGeneralPollHandpayReset()
{
	LogString(PROGRESSIVE, "ProcessGeneralPollHandpayReset()");
	LogString(SASGENERAL, "ProcessGeneralPollHandpayReset()");

	CInternalMessage *msg = new CInternalMessage(INT_MSG_HANDPAY_RESET);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);

	CLongPollA8Message sasMsg(m_gameAddress);
	sasMsg.SetStandardHandpay();
	sasMsg.Fill();
	m_sasPollQueue->Add(&sasMsg, QUEUE_PRIORITY_HIGHEST);
	sasMsg.WaitForResponse();
}

void CSASProtocol::ProcessMeterChangePending()
{
	CLongPollB6Message lpb6(m_gameAddress);
	lpb6.SetAcknowledge();
	lpb6.Fill();
	m_sasPollQueue->Add(&lpb6,QUEUE_PRIORITY_NORMAL);

	lpb6.WaitForResponse();
	if (lpb6.GetReturnValue() == SASPOLL_SUCCESS)
	{
		CInternalMessage *msg = new CInternalMessage(INT_MSG_METER_CHANGE_REQUEST);
		msg->SetData((WORD)lpb6.GetStatus());
		SendMessageToInternals(msg,QUEUE_PRIORITY_NORMAL);
	}
}

void CSASProtocol::ProcessMeterChangeCancelled()
{
	// Send meter change canceled to internals.  Don't need to send anything back to EGM
	CInternalMessage *msg = new CInternalMessage(INT_MSG_METER_CHANGE_CANCEL);
	SendMessageToInternals(msg,QUEUE_PRIORITY_NORMAL);
}

void CSASProtocol::ProcessEnabledGamesDenomChanged()
{
	// Send meter change canceled to internals.  Don't need to send anything back to EGM
	CInternalMessage *msg = new CInternalMessage(INT_MSG_METER_CHANGE_COMPLETE);
	SendMessageToInternals(msg,QUEUE_PRIORITY_NORMAL);
}
