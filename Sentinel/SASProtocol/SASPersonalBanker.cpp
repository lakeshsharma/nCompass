#include "stdafx.h"

#include "../InternalMessage.h"
#include "SASProtocol.h"
#include "SASPollMessage.h"
#include "SASSerialWrapper.h"
#include "LongPoll1FMessage.h"
#include "../Meters.h"
#include "../PersonalBankerTransfer.h"
#include "LongPollDMessage.h"
#include "LongPollUMessage.h"
#include "LongPoll28Message.h"
#include "../Sentinel.h"
#include "Diagnostics.h"
#include "../Logging/LogString.h"

const int LOCK_GRANTED_MAX_TIME = 10; // max seconds to wait for game to lock

const byte EFTACKPHASE1 = 0;
const byte EFTACKPHASE2 = 1;

const byte EFTPROMODOWN = 0X63;
const byte EFTCREDITSUP = 0X64;
const byte EFTCASHDOWN = 0X69;
const int EGMENABLEWAITTICKS = 250;
const int LENGTHOFEFTSEEDARRRAY = 4;
const int PROBEPBTLOCKTIME = 2000;

#define UPLOAD 0
#define DOWNLOAD 1

using namespace std;

void CSASProtocol::ProcessPBTransfer(CInternalMessage *msg)
{
	CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ProcessPBTransfer()");

	m_pbtStartTickCount = CUtilities::GetTickCount();

	// Check to see if enough time has elapsed since last EGM Enable poll was sent
	LogString(PBT_LOG, "Checking EnableEGM time.");
	if (m_pbtStartTickCount - m_lastEGMEnableTickCount < EGMENABLEWAITTICKS)
	{
		LogString("PBT", "EnableEGM time %dms", m_pbtStartTickCount - m_lastEGMEnableTickCount);
		CUtilities::Sleep( EGMENABLEWAITTICKS - ( m_pbtStartTickCount - m_lastEGMEnableTickCount ) );
	}

	lock_guard<recursive_mutex> lock(m_pbtCritSection);

	// check if previous pbt in process
	if (m_pbt == NULL)
	{

		m_pbt = msg->GetPBTransferData();
		m_state = (PersonalBankerState)msg->GetWordData();
		msg->SetData((CPersonalBankerTransfer *)NULL); // save data

		bool denomGood = false;
		bool gameComGood = true;

		// check pbt ID
		if (m_pbt->IsValidID(m_sasState.GetSlotmastID(), m_pbt->GetID(), m_pbt->GetPBTIDLength()))
		{
			if (m_pbt->VerifyMaxAmounts())
			{
				CheckDenomForPB(denomGood, gameComGood);

				if (denomGood)
				{
					if (m_bIsAFTSupported)
					{
						if (m_pbt->IsCashoutToHostTransfer())
						{
							CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ProcessPBTransfer(): Initiating cashout to host transfer on SAS...");

							// Game is already implictly in locked state for cashout to host
							// We cannot send lock requests right now since it would be rejected
							// so just get normal LP74 queued to get the credits on the EGM
							// and directly start the transfer
							CLongPoll74Message *lp74 = new CLongPoll74Message(m_gameAddress);
							lp74->SetLockCode(LOCK_CODE_QUERY_STATUS);
							lp74->SetLockTimeout(0);
							lp74->Fill();
							m_sasPollQueue->Add(lp74, QUEUE_PRIORITY_HIGHEST);
							lp74->WaitForResponse();

							if (SASPOLL_SUCCESS != lp74->GetReturnValue())
							{
								LogString(SASGENERAL, "[%s] LP74 FAILED 0x%x", __FUNCTION__, lp74->GetReturnValue());
								return;
							}

							LogString(PBT_LOG, "[%s] LP74 succeeded Credits: [%d %d %d] GameLockStatus: 0x%x AvailableTransfers: 0x%x", __FUNCTION__,
								lp74->m_currentCashableAmount, lp74->m_currentRestrictedAmount, lp74->m_currentNonrestrictedAmount,
								lp74->m_gameLockStatus, lp74->m_availableTransfers);

							m_lp74 = lp74;
							ExecutePBTransfer();
						}
						else
						{
							CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ProcessPBTransfer(): Sending AFT lock request");

							// begin by locking game
							LockGameForAFTTransfer();
						}
					}
					else if (m_bIsEFTSupported)
					{
						PerformEFT();
					}
					else
					{
						CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, EFT GeneralError: EFT or AFT not supported by EGM");
						// EFT/AFT not supported
						CancelEFT(GeneralError);
					}
				}
				else
				{
					PollerCancellationType cancelType;
					if (gameComGood)
					{
						cancelType = BartGameDenomMismatch;
					}
					else
					{
						cancelType = SASTimeout;
					}

					// denom is bad
					if (m_bIsAFTSupported)
					{
						m_pbt->ChangePersonalBankerTypeToCanceled();
						CancelPBTransfer(cancelType);
					}
					else
						CancelEFT(cancelType);
				}
			}
			else
			{
				// Received invalid amounts, cancel transfer and log it
				m_pbt->ChangePersonalBankerPromoTypeToCanceled();
				CDiagnostics::ReportPBTTicketDiagnosticToFile(CDirectoryManager::PBTTicketDiagnosticsFilePath(), CUtilities::LoadResourceString(IDS_PBT_AMOUNT_OVER_LIMIT_BEFORE), m_pbt->ToStringForLog(), tltTransferOvrLmtBef);
				CancelPBTransfer(OverTransferLimit);

				// Send tilt transaction 
				SendTiltToInternals(tltTransferOvrLmtBef);
			}
		}
		else
		{
			// Received invalid ID, cancel transfer and log it
			m_pbt->ChangePersonalBankerPromoTypeToCanceled();
			CDiagnostics::ReportPBTTicketDiagnosticToFile(CDirectoryManager::PBTTicketDiagnosticsFilePath(), CUtilities::LoadResourceString(IDS_PBT_ID_INVALIDE_BEFORE), m_pbt->ToStringForLog(), tltInvalidPbIdBef);
			CancelPBTransfer(InvalidTransID);

			// Send tilt transaction 
			SendTiltToInternals(tltInvalidPbIdBef);
		}
	}			
	else
	{
		// previous pbt in process, clear m_pbt and report error
		m_pbt->ChangePersonalBankerTypeToCanceled();
		CancelPBTransfer(GeneralError);
	}
}

void CSASProtocol::LockGameForAFTTransfer()
{
	bool deletelp74 = true;

	CLongPoll74Message *lp74 = new CLongPoll74Message(m_gameAddress);
	lp74->SetLockCode(LOCK_CODE_REQUEST_LOCK);
	lp74->SetLockTimeout(LOCK_DURATION);

	// set desired transfer condition
	switch(m_pbt->GetPersonalBankerType())
	{
	case CashUpload:
	case CashUploadAuto:
		lp74->SetXferFromGame(true);
		break;
	case CashDownload:
	case PromoCreditsToCash:
	case PointRedemptionToCash:
	case PointRedemptionToPromoCredits:
		lp74->SetXferToGame(true);
		break;
	default:
		LogString("PBT", "Unexpected personal banker type %d: AFT 74",
			(int)m_pbt->GetPersonalBankerType());
		break;

	}

	// look for ncep out
	if (m_pbt->GetPromoPersonalBankerType() == PromoCreditsUpload ||
		m_pbt->GetPromoPersonalBankerType() == PromoCreditsUploadAuto)
	{
		lp74->SetXferFromGame(true);
	}

	lp74->Fill();
	m_sasPollQueue->Add(lp74,QUEUE_PRIORITY_HIGHEST);
	m_sasState.SetLastLockRequest(CUtilities::GetTickCount());
	lp74->WaitForResponse();

	if (lp74->GetReturnValue() == SASPOLL_SUCCESS)
	{

		// Check return values
		switch(lp74->m_gameLockStatus)
		{
		case LOCK_CODE_GAME_LOCK_PENDING:
			// Do nothing, wait for the general exception x6f
			CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ProcessPBTransfer(): Lock pending waiting for 6F Exception");
			m_sasState.SetLockPending(true);
			break;
		case LOCK_CODE_GAME_NOT_LOCKED:
			// lock has been rejected, clear m_pbt and report error
			CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ProcessPBTransfer(): Game not locked canceling PBT");
			m_pbt->ChangePersonalBankerTypeToCanceled();
			CancelPBTransfer(LockReject);
			break;
		case LOCK_CODE_GAME_LOCKED:
			// lock has been granted, start process.
			deletelp74 = false;
			m_lp74 = lp74;

			CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ProcessPBTransfer(): Game locked starting PBT");
			ExecutePBTransfer();
			break;
		default:
			LogString(PBT_LOG, "Unexpected lock status");
			break;

		}
	}
	else
	{
		// lock poll failed, cancel request, clear m_pbt and report error
		m_pbt->ChangePersonalBankerTypeToCanceled();
		CancelPBTransfer(LockReject);
					
	}
				
	if (deletelp74)
		delete lp74;
}

void CSASProtocol::CheckDenomForPB( bool &denomGood, bool &gameComGood )
{
	// do 1f poll, make sure denom hasn't changed
	CLongPoll1FMessage *lp1f = new CLongPoll1FMessage(m_gameAddress);
	lp1f->Fill();
	m_sasPollQueue->Add(lp1f,QUEUE_PRIORITY_HIGHEST);
	lp1f->WaitForResponse();
	//LogData("SASPBDATA", DATA_WRITE | DATA_COM1, lp1f->GetSendBuffer(), lp1f->GetSendBufferSize());
	//LogData("SASPBDATA", DATA_WRITE | DATA_COM1, lp1f->GetResponseBuffer(), lp1f->GetResponseBufferSize());

	if ( lp1f->GetReturnValue() == SASPOLL_SUCCESS )
	{
			
		WORD newDenom = (WORD)denomCodeMap[lp1f->m_denomination].multiplier;
		if (m_pbt->GetDenomination() != newDenom)
		{
			SetDenomMult(newDenom);
			CInternalMessage *msg = new CInternalMessage( INT_MSG_SET_DENOM, (WORD)denomCodeMap[lp1f->m_denomination].multiplier);

			SendMessageToInternals( msg, QUEUE_PRIORITY_NORMAL );
		}
		else
			denomGood = true;
	}
	else
	{
		gameComGood = false;
	}

	// delete 1f poll
	delete lp1f;

}

void CSASProtocol::PerformEFT()
{

	CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, PerformEFT(): Starting EFT");
	// perfrom EFT transfer
	bool isSupported = true;
	byte direction = 0;
	CSASPollMessage *lp = NULL;
	CLongPollDMessage *lpD = NULL;
	CLongPollUMessage *lpU = NULL;

	GetCurrentCreditMeter(m_pbt);

	lp = SetupEFTPoll( isSupported, direction );

	if (isSupported)
	{
		CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, PerformEFT(): Starting first EFT Long Poll");
		lp->Fill();

		// Set starvation so no polls interrupt EFT process
		m_sasPollQueue->SetQueueStarvation(QUEUE_PRIORITY_HIGHEST);
		m_sasPollQueue->Add(lp,QUEUE_PRIORITY_HIGHEST);
		lp->WaitForResponse();

		// resolve the EGM transfer mode if required, based in the response of long poll.
		if (m_egmTransferMode == EGMTransferMode_PENDING)
		{
			if (lp->GetReturnValue() == SASPOLL_SUCCESS)
			{
				LogString(PBT_LOG, "Long poll success, setting EGM transfer mode from PENDING to EFT");
				m_egmTransferMode = EGMTransferMode_EFT;
			}
			else
			{
				LogString(PBT_LOG, "Long poll failure, setting EGM transfer mode from PENDING to NONE");
				m_egmTransferMode = EGMTransferMode_NONE;
			}
			m_sasInfo->SetEGMTransferMode(m_egmTransferMode);

			// Send updated m_sasInfo to internals to get merged into main games object
			LogString(PBT_LOG, "EGM transfer mode is resolved. Sending updated SAS info to internals...");
			CGames *sasInfo = new CGames(*m_sasInfo);
			CInternalMessage *sasInfoMsg = new CInternalMessage(INT_MSG_UPDATE_SAS_INFO, sasInfo);
			SendMessageToInternals(sasInfoMsg, QUEUE_PRIORITY_HIGH);
		}

		CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ProcessPBTransfer(): Returned from first EFT Long Poll");
		DWORD amount;
		byte status;
		if (direction == UPLOAD)
		{
			lpU = (CLongPollUMessage *) lp;
			amount = lpU->GetAmount();
			status = lpU->GetStatus();

			// set cashable amount as pbt amount
			// TR# 017551
			if (amount * m_denomMult < m_pbt->GetMinCashUp() * ONE_DOLLAR_IN_CENTS)
			{
				CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, PerformEFT(): Canceling EFT Under Transfer Limit");
				// cancel
				m_pbt->SetCancelledType(UnderTransferLimit);
				status = EFTErrorUnderTransferLimit; // made up value used EFT error handling method
			}
			else if (amount * m_denomMult > m_pbt->GetMaxCashUp() * ONE_DOLLAR_IN_CENTS)
			{
				CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, PerformEFT(): Canceling EFT Over Transfer Limit");
				// cancel
				m_pbt->SetCancelledType(OverTransferLimit);
				status = EFTErorrOverTransferLimit; // used in switch below
			}
			else
			{
				m_pbt->SetAmount(amount * m_denomMult);
			}
		}
		else
		{
			lpD = (CLongPollDMessage *) lp;
			amount = lpD->GetAmount();
			status = lpD->GetStatus();
		}
					
		CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, EFT Amount %u, m_pbt Amount %lld, status %d",
			(unsigned)(amount * m_denomMult),
			(long long)m_pbt->GetAmount(), status);
		// check response amount
		if (lp->GetReturnValue() == SASPOLL_SUCCESS && m_pbt->GetAmount() == amount * m_denomMult  && status == EFTSuccess)
		{
			CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, EFT Amount is good, setting Ack to 1");
			// amount ok, continue with transfer
			// set ack to 1
			if (direction == UPLOAD)
			{
				lpU->SetAck(EFTACKPHASE2);
			}
			else
			{
				lpD->SetAck(EFTACKPHASE2);
			}
			lp->ResetResponse();
			lp->Fill();
					

			m_sasPollQueue->Add(lp,QUEUE_PRIORITY_HIGHEST);
			lp->WaitForResponse();
			CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, EFT Ack long poll returned");
			
			// if response status = 0, then transfer complete
			m_sasPollQueue->ClearQueueStarvation();	

			// default in case long poll fails
			status = EFTErrorGameCommDown; 
			if (lp->GetReturnValue() == SASPOLL_SUCCESS)
			{
				if (direction == UPLOAD)
				{
					status = lpU->GetStatus();
				}
				else
				{
					status = lpD->GetStatus();
				}
			}			
			if (status == EFTSuccess)
			{

				CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, EFT Status returned of zero canceling EFT");
				m_pbt->SetCancelledType(_Unknown);

				// send to internals
				GetFinalCreditMeter();
				GetPBTMeters();

				//ASSERT( m_pbt != NULL );

				CInternalMessage *msg = new CInternalMessage(INT_MSG_PB_TRANSFER, m_pbt, m_state);

				m_pbt = NULL;
				SendMessageToInternals( msg, QUEUE_PRIORITY_HIGHEST ); 
			}

			else
			{
				HandleEFTErrorStatus(status);
			}

		}
		else
		{
						
			if (lp->GetReturnValue() != SASPOLL_SUCCESS)
			{
				// made up value used in EFT error handling method
				status = EFTErrorSASTimeout; 
			}

			// problem with transfer, cancel
			HandleEFTErrorStatus( status );
		}
	}
	else
	{

		CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, EFT SASTimeout");
		CancelEFT(SASTimeout);
	}
	// clean up EFT
	if (lp != NULL)
	{
		delete lp;
		lp = NULL;
		lpD = NULL;
		lpU = NULL;
	}
}

CSASPollMessage *CSASProtocol::SetupEFTPoll( bool &isEftSupported, byte &direction )
{
	CSASPollMessage *lp = NULL;
	CLongPollDMessage *lpD = NULL;
	CLongPollUMessage *lpU = NULL;
	

	switch(m_pbt->GetPersonalBankerType())
	{
	case CashUpload:
	case CashUploadAuto:
		// Need to get the current credits to determine the amount to upload
		// set pbt amount to current credit meter.
		// for any non aristocrat machine, disable game
		if (!m_bEFTCashFromGame)
		{
			// this operation not supported on this game
			isEftSupported = false;
		}
		else
		{
			m_pbt->SetAmount(m_pbt->GetOriginalCreditMeter());
			lpU = new CLongPollUMessage(m_gameAddress);
			lpU->SetPollValue(EFTCREDITSUP);
			lpU->SetTransactionNumber(m_pbt->GetEFTNumber());
			lpU->SetAck(EFTACKPHASE1);
			direction = UPLOAD;
			lp = lpU;
		}
		break;
	case CashDownload: // download as cashable
		if (!m_bEFTCashToGame)
		{
			// this operation not supported on this game
			isEftSupported = false;
		}
		else
		{
			lpD = new CLongPollDMessage(m_gameAddress);
			lpD->SetPollValue(EFTCASHDOWN);
			lpD->SetTransactionNumber(m_pbt->GetEFTNumber());
			lpD->SetAck(EFTACKPHASE1);
			lpD->SetAmount((DWORD)m_pbt->GetAmount()/m_denomMult);
			direction = DOWNLOAD;
			lp = lpD;
		}
		break;
	case PromoCreditsToCash: // download as promo
		if (!m_bEFTCashToGame)
		{
			// this operation not supported on this game
			isEftSupported = false;
		}
		else
		{
			lpD = new CLongPollDMessage(m_gameAddress);
			lpD->SetPollValue(EFTPROMODOWN);
			lpD->SetTransactionNumber(m_pbt->GetEFTNumber());
			lpD->SetAck(EFTACKPHASE1);
			lpD->SetAmount((DWORD)m_pbt->GetAmount()/m_denomMult);
			direction = DOWNLOAD;
			lp = lpD;
		}
		break;
	case PointRedemptionToCash: // download as nonrestricted or restricted
	case PointRedemptionToPromoCredits:
		if (!m_bEFTCashToGame)
		{
			// this operation not supported on this game
			isEftSupported = false;
		}
		else
		{
			lpD = new CLongPollDMessage(m_gameAddress);
			lpD->SetTransactionNumber(m_pbt->GetEFTNumber());
			lpD->SetAck(EFTACKPHASE1);
			lpD->SetAmount((DWORD)m_pbt->GetAmount()/m_denomMult);
			if (m_pbt->GetPersonalBankerType() == PointRedemptionToPromoCredits)
				lpD->SetPollValue(EFTPROMODOWN);
			else
				lpD->SetPollValue(EFTCASHDOWN);
			direction = DOWNLOAD;
			lp = lpD;
		}
		break;
	default:
		LogString("PBT", "Unexpected EFT personal banker type");
		break;
	}

	return lp;
}

void CSASProtocol::HandleEFTErrorStatus(byte status)
{
	PollerCancellationType cancelCode = _Unknown;
	
	switch(status)
	{
	case EFTErrorDoorOpen:
		cancelCode = DoorOpen;
		break;
	case EFTErorrOverTransferLimit:
		cancelCode = OverTransferLimit;
		break;
	case EFTErrorSASInvalidTransactionNm:
		cancelCode = SASInvalidTransactionNum;
		break;
	case EFTErrrorNonCreditMode:
		cancelCode = NonCreditMode;
		break;
	case EFTErrorMachineTilt:
		cancelCode = MachineTilt;
		break;
	case EFTErrorSASInvalidAckMessage:
		cancelCode = SASInvalidAckMessage;
		break;
	case EFTErrorGameInProgress:
		cancelCode = GameInProgress;
		break;
	case EFTErrorNonBCDDataError:
		cancelCode = SASNonBCDDataError;
		break;
	case EFTErrorSASDuplicateTransferNum:
		cancelCode = SASDuplicateTransferNum;
		break;
	case EFTErrorMachineDisabledViaSAS:
		cancelCode = MachineDisabledViaSAS;
		break;
	case EFTErrorMachineOutOfService:
		cancelCode = MachineOutOfService;
		break;
	case EFTErrorSASMachineBusy:
		cancelCode = SASMachineBusy;
		break;
	case EFTErrorUnderTransferLimit:
		cancelCode = UnderTransferLimit;
		break;
	case EFTErrorSASTimeout:
		cancelCode = SASTimeout;
		break;
	case EFTErrorGameCommDown:
		cancelCode = GameCommDown;
		break;
	default:
		cancelCode = _Unknown;
		break;
	}
	CancelEFT(cancelCode);
}

void CSASProtocol::ProcessPersonalBankerGameLocked()
{
	CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ProcessAFTPersonalBankerGameLocked()");

	LogString("PBT", "Processing PBT after game locked");
	
	lock_guard<recursive_mutex> lock(m_pbtCritSection);
	m_sasState.SetLockPending(false);
	if (m_pbt != NULL && !m_pbt->GetPBTSent())
	{
		// make sure that game locked in a reasonable amount of time
		DWORD now = CUtilities::GetTickCount();
		DWORD ts;

		// timespan needs to measure time since lock request, not first lock attempt.
		ts = now - m_sasState.GetLastLockRequest();
		if (ts <= LOCK_GRANTED_MAX_TIME * MILLISECONDS_PER_SECOND)
		{
			// lock granted in reasonable time, query lock status
			CLongPoll74Message *lp74 = new CLongPoll74Message(m_gameAddress);
			lp74->SetLockCode(LOCK_CODE_QUERY_STATUS);
			// Tracker #018406
			lp74->SetLockTimeout(LOCK_DURATION);
			lp74->Fill();
			m_sasPollQueue->Add(lp74, QUEUE_PRIORITY_HIGHEST);
			lp74->WaitForResponse();

			if (lp74->GetReturnValue() == SASPOLL_SUCCESS)
			{
				m_lp74 = lp74;
				if (lp74->m_gameLockStatus == LOCK_CODE_GAME_LOCKED)
				{
					CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ProcessAFTPersonalBankerGameLocked(): Game Locked Starting Transfer");
					// do pb transfer now
					ExecutePBTransfer();
				}
				else
				{
					CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ProcessAFTPersonalBankerGameLocked(): Game Not Locked Canceling Transfer");
					// game not really locked.  This should not happen
					m_pbt->ChangePersonalBankerTypeToCanceled();
					CancelPBTransfer(LockReject);
				}
			}
			else
			{
				CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ProcessAFTPersonalBankerGameLocked(): Lock Query Failed Canceling Transfer");
				// lock query failed, cancel pbt
				delete lp74;
				m_pbt->ChangePersonalBankerTypeToCanceled();
				CancelPBTransfer(LockReject);
			}

		}
		else
		{
			CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ProcessAFTPersonalBankerGameLocked(): Lock Query Took Too Long Canceling Transfer");
			// took too long, cancel
			CancelPBTransfer(LockReject);
		}
	}
}

void CSASProtocol::ProcessProbePbtGameLocked()
{
	lock_guard<recursive_mutex> lock(m_pbtCritSection);
	m_sasState.SetLockPending(false);
	CLongPoll74Message *lp74 = new CLongPoll74Message(m_gameAddress);
	lp74->SetLockCode(LOCK_CODE_QUERY_STATUS);
	// Tracker #018406
	lp74->SetLockTimeout(PROBEPBTLOCKTIME);
	lp74->Fill();
	m_sasPollQueue->Add(lp74, QUEUE_PRIORITY_HIGHEST);
	lp74->WaitForResponse();

	// Default true so PBT can proceed if lp74 fails
	bool bCanLock = true;
	if (lp74->GetReturnValue() == SASPOLL_SUCCESS)
	{
		if (lp74->m_gameLockStatus == LOCK_CODE_GAME_NOT_LOCKED)
		{
			bCanLock = false;
		}
		else
		{
			// Cancel lock
			CancelPBLock();
		}
	}
	delete lp74;

	m_sasState.SetProbingPBTLock(false);

	// Send result to internals
	CInternalMessage *msg = new CInternalMessage(INT_MSG_PROBE_PBT_RESULT, (WORD) bCanLock);
	SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);
}

void CSASProtocol::ProcessAFTBonusGameLocked()
{
	LONG64 jackpotID = CEGMAward::INVALID_JACKPOT_ID;
	bool sendResult = false;
	m_sasState.SetLockPending(false);

	lock_guard<recursive_mutex> lock(m_egmAwardCritSection);
	if (m_egmAward != NULL)
	{
		m_egmAward->LogInfo(_T("AFT Bonus Game Locked"));
		jackpotID = m_egmAward->GetJackpotId();
	}

	if (CEGMAward::IsValidJackpotId(jackpotID))
	{
		// lock granted in reasonable time, query lock status
		CLongPoll74Message *lp74 = new CLongPoll74Message(m_gameAddress);
		lp74->SetLockCode(LOCK_CODE_QUERY_STATUS);
		// Tracker #018406
		lp74->SetLockTimeout(LOCK_DURATION);
		lp74->Fill();
		m_sasPollQueue->Add(lp74, QUEUE_PRIORITY_HIGHEST);
		lp74->WaitForResponse();

		if (lp74->GetReturnValue() == SASPOLL_SUCCESS)
		{
			if (lp74->m_gameLockStatus == LOCK_CODE_GAME_LOCKED)
			{
				// do award transfer now
				ExecuteAFTBonusAward(lp74->m_assetNumber);
				sendResult = true;
			}
			else
			{
				// game not really locked.  This should not happen
				LogString(ERROR_LOG, "Could not obtain lock while trying to process bonus award");

				if (m_egmAward != NULL)
				{
					sendResult = true;
					m_egmAward->SetAwardState(GameBusy);
					m_egmAward->LogInfo(_T("MM SAS PB AFT game not really locked"));
				}
			}
		}
		else
		{
			// lock query failed
			LogString(ERROR_LOG, "Lock querry failed while trying to process bonus award");
			if (m_egmAward != NULL)
			{
				m_egmAward->SetAwardState(GameBusy);
				m_egmAward->LogInfo(_T("MM SAS PB AFT lock query failed"));
			}
		}

		if (sendResult)
		{
			if (m_egmAward != NULL)
			{
				m_egmAward->LogInfo(_T("MM SAS PB AFT send result to internals from ProcessAFTBonusGameLocked"));
				CEGMAward *copyAward = new CEGMAward(*m_egmAward);
				CInternalMessage *awardMsg = new CInternalMessage(INT_MSG_EGM_AWARD, copyAward);
				SendMessageToInternals(awardMsg, QUEUE_PRIORITY_HIGHEST);
			}
		}
		delete lp74;
	}
}

void CSASProtocol::ProcessGeneralPollGameLocked()
{
	bool bProcessPB = false;
	LONG64 bonusAwardID = CEGMAward::INVALID_JACKPOT_ID;
	LogString("PBT", "Process game locked");
	if (m_sasState.GetProbingPBTLock())
	{
		// Lock is to pre-determine PBT lock success
		ProcessProbePbtGameLocked();
	}
	else if (m_egmPrizeAward)
	{
		CheckAFTLockForPrizeAward();
	}
	else
	{
		{
			lock_guard<recursive_mutex> lock(m_pbtCritSection);
			if (m_pbt != NULL)
			{
				bProcessPB = true;
			}
		}
		
		// Get the bonus award ID of any AFT bonus award in progress
		{
			lock_guard<recursive_mutex> lock(m_egmAwardCritSection);
			if (m_egmAward != NULL)
			{
				m_egmAward->LogInfo(_T("Processing game locked"));
		
				if (m_egmAward->GetAwardState() == AftLockPending)
				{
					bonusAwardID = m_egmAward->GetJackpotId();
				}
			}
		}

		// We have to process the PBT Lock
		if (bProcessPB)
		{
			// Process the PBT
			ProcessPersonalBankerGameLocked();

			// Delay aft bonus award
			if (CEGMAward::IsValidJackpotId(bonusAwardID))
			{
				lock_guard<recursive_mutex> lock(m_egmAwardCritSection);
				if (m_egmAward != NULL)
				{
					// Send to internals
					m_egmAward->SetAwardState(GameBusy);
					m_egmAward->LogInfo(_T("MM SAS PB AFT delay bonus award, send to internals from ProcessGeneralPollGameLocked"));
					CEGMAward *copyAward = new CEGMAward(*m_egmAward);
					CInternalMessage *awardMsg = new CInternalMessage(INT_MSG_EGM_AWARD, copyAward);
					SendMessageToInternals(awardMsg, QUEUE_PRIORITY_HIGHEST);
				}
			}
		}
		// Process bonus award
		else if (CEGMAward::IsValidJackpotId(bonusAwardID))
		{
			ProcessAFTBonusGameLocked();
		}
	}
}

void CSASProtocol::SetupAFT72PollPersonalBankerType( CLongPoll72Message *lp72, CLongPoll74Message *lp74, bool &proceedWithCashTransfer  )
{
	switch(m_pbt->GetPersonalBankerType())
	{
	case CashUpload:
	case CashUploadAuto:
		{
			DWORD amountToTransfer = 0;
			DWORD nonRestrictedAmountToTransfer = 0;

			DWORD amountAvailableToTransfer = lp74->m_currentCashableAmount;
			DWORD nonRestrictedAmountAvailableToTransfer = m_pbt->IsCEPOutEnabled() ? lp74->m_currentNonrestrictedAmount : 0;

			if (m_pbt->GetAmount() == 0)
			{
				amountToTransfer = amountAvailableToTransfer;
				nonRestrictedAmountToTransfer = nonRestrictedAmountAvailableToTransfer;
			}
			else
			{
				if (m_pbt->IsProcessTaxDeduction())
				{
					amountToTransfer = (DWORD)m_pbt->GetAmount();
					//To Prevent false reporting of CanceledPointCashUpload we must set nonRestrictedAmountToTransfer to 0
					nonRestrictedAmountToTransfer = 0;
					LogString(JACKPOT, "SetupAFT72PollPersonalBankerType TaxDeduction=%u", amountToTransfer);
				}
				else
				{
					// split amount to transfer into cash bucket and cashable promos bucket to take-off
					amountToTransfer = min((DWORD)m_pbt->GetAmount(), amountAvailableToTransfer);
					nonRestrictedAmountToTransfer = m_pbt->GetAmount() - amountToTransfer;
				}
			}

			if (CashUploadAuto == m_pbt->GetPersonalBankerType())
			{
				amountToTransfer = m_pbt->IsWatAutoUpload() ? amountToTransfer : 0;
				nonRestrictedAmountToTransfer = m_pbt->IsCepAutoUpload() ? nonRestrictedAmountToTransfer : 0;
			}

			m_pbt->SetAmount(amountToTransfer);

			if (!m_pbt->IsProcessTaxDeduction())
			{
				m_pbt->SetPartialWATOutTransfer((amountToTransfer != 0) &&
					(amountToTransfer < lp74->m_currentCashableAmount));
				m_pbt->SetPartialCEPOutTransfer((nonRestrictedAmountToTransfer != 0) &&
					(nonRestrictedAmountToTransfer < lp74->m_currentNonrestrictedAmount));
			}
			m_pbt->SetNonrestrictedAmount(nonRestrictedAmountToTransfer);
			CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, SetupAFT72PollPersonalBankerType(): LP74 (get credits) result: Cashable: %d "
				"Non-restricted %d restricted %d. MinCashUp %u MaxCashUp %u CepOutEnabled %d, amountToTransfer %u nonRestrictedAmountToTransfer %u "
				"PartialWATOut %d PartialCEPOut %d",
				lp74->m_currentCashableAmount, lp74->m_currentNonrestrictedAmount, 
				lp74->m_currentRestrictedAmount, m_pbt->GetMinCashUp(), m_pbt->GetMaxCashUp(), m_pbt->IsCEPOutEnabled(),
				amountToTransfer, nonRestrictedAmountToTransfer, m_pbt->IsPartialWATOutTransfer(),
				m_pbt->IsPartialCEPOutTransfer());

			if (amountToTransfer + nonRestrictedAmountToTransfer < m_pbt->GetMinCashUp() * ONE_DOLLAR_IN_CENTS)
			{
				// amount too small
				m_pbt->SetCancelledType(UnderTransferLimit);
				lp72->SetCashableAmount(0);
				proceedWithCashTransfer = false;
			}
			//If there is no amount to transfer and this is tax deduction, consider it over transfer limit.
			//It is over the transfer limit because, because lp74->m_currentCashableAmount is 0.
			//We are trying to takeout x amount of $ from the game but the game has zero cashable amount.
			else if ((m_pbt->IsProcessTaxDeduction() && amountToTransfer == 0) ||
				amountToTransfer + nonRestrictedAmountToTransfer >
				min((amountAvailableToTransfer + nonRestrictedAmountAvailableToTransfer),
				(m_pbt->GetMaxCashUp() * ONE_DOLLAR_IN_CENTS)))
			{
				if ((m_pbt->IsProcessTaxDeduction() && amountToTransfer == 0) || (!m_pbt->IsProcessTaxDeduction()))
				{
					// amount too large
					m_pbt->SetCancelledType(OverTransferLimit);
					lp72->SetCashableAmount(0);
					proceedWithCashTransfer = false;
				}
				//We don't need to abide by WAT out limit for tax deductions
				else if(m_pbt->IsProcessTaxDeduction() && amountToTransfer > 0)
				{
					lp72->SetTransferType(SAS_TRANSFER_TYPE_CASH_TO_HOST);
					lp72->SetCashableAmount(amountToTransfer);
					proceedWithCashTransfer = true;
				}
			}
			else
			{				
				lp72->SetTransferType(SAS_TRANSFER_TYPE_CASH_TO_HOST);
				lp72->SetCashableAmount(amountToTransfer);
				lp72->SetNonrestrictedAmount(nonRestrictedAmountToTransfer);
				proceedWithCashTransfer = true;
			}
		}
		break;
	case CashDownload: // download as cashable
		lp72->SetTransferType(SAS_TRANSFER_TYPE_CASH_TO_GAME);
		lp72->SetCashableAmount((DWORD)m_pbt->GetAmount());
		proceedWithCashTransfer = true;
		break;
	case PromoCreditsToCash: // download as restricted
		lp72->SetTransferType(SAS_TRANSFER_TYPE_CASH_TO_GAME);
		lp72->SetRestrictedAmount((DWORD)m_pbt->GetAmount());
		proceedWithCashTransfer = true;
		break;
	case PointRedemptionToCash: // download as nonrestricted
		lp72->SetTransferType(SAS_TRANSFER_TYPE_CASH_TO_GAME);
		lp72->SetNonrestrictedAmount((DWORD)m_pbt->GetAmount());
		proceedWithCashTransfer = true;
		break;
	case PointRedemptionToPromoCredits:	// download as restricted
		lp72->SetTransferType(SAS_TRANSFER_TYPE_CASH_TO_GAME);
		lp72->SetRestrictedAmount((DWORD)m_pbt->GetAmount());
		proceedWithCashTransfer = true;
		break;
	default:
		LogString("PBT", "Unexpected personal banker type %d: AFT 72",
			(int)m_pbt->GetPersonalBankerType());
		proceedWithCashTransfer = false;
		break;
	}
}

void CSASProtocol::ExecutePBTransfer()
{
	LogString("PBT","SAS PBT, Game locked, begin SAS AFT Transfer");

	// Add small sleep to give game a chance to really lock itself
	CUtilities::Sleep(PBTLOCKSLEEP);

	CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ExecutePBTransfer(): Game locked, begin SAS AFT Transfer");
	if (m_pbt && !m_pbt->GetPBTSent())
	{
		// make sure that m_lp74 is valid
		//ASSERT(m_lp74 != NULL);

		bool proceedWithTransfer = true;
		CLongPoll72Message *lp72 = SetupAFT72Poll(proceedWithTransfer);
		
		if (proceedWithTransfer)
		{
			if (m_pbt->IsCashoutToHostTransfer())
			{
				lp72->SetTransferFlag(TRANSFER_FLAG_ACCEPT_TRANSFER_ONLY_IF_LOCKED, false);
				lp72->SetTransferFlag(TRANSFER_FLAG_CASHOUT_FROM_EGM_REQUEST, true);
			}

			// send it
			lp72->Fill();
			m_pbt->SetPBTSent(true);

			m_sasPollQueue->Add(lp72,QUEUE_PRIORITY_HIGHEST);
			lp72->WaitForResponse();

			// too late to turn back, we must get final completion status from EGM
			if (lp72->GetReturnValue() == SASPOLL_SUCCESS)
			{
				// look at status, if not pending then handle response
				if (lp72->m_transferStatus != TRANSFER_STATUS_PENDING)
				{
					UpdatePBTWithTransferStatus(lp72->m_transferStatus);

					CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ExecutePBTransfer(): Download Complete");
					LogString("PBT","SAS PBT Complete without pending");

					GetFinalCreditMeter();
					GetPBTMeters();

					//ASSERT( m_pbt != NULL );

					CInternalMessage *iMsg = new CInternalMessage(INT_MSG_PB_TRANSFER, m_pbt, m_state);
					m_pbt = NULL;
					SendMessageToInternals( iMsg, QUEUE_PRIORITY_HIGHEST ); 
				}
				else
				{
					CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ExecutePBTransfer(): Download pending status reported by EGM");
				}
			}
			else
			{
				CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ExecutePBTransfer(): EGM Returned Invalid Response.");
				// something is wrong, egm responded with incomplete response or no response at all.
				LogString(ERROR_LOG, "Incomplete or no response to Long Poll 72");

			}
			
			// clean up
			delete lp72;

			if (m_lp74 != NULL)
			{
				delete m_lp74;
				m_lp74 = NULL;
			}

		}
		else
		{
			// cancel transfer
			delete lp72;
			CancelPBTransfer(m_pbt->GetCancelledType());
		}
	}
	else
	{
		LogString("PBT", "ExecutePBTransfer(), PBT object NULL or PBT already sent");
		if (m_lp74 != NULL)
		{
			delete m_lp74;
			m_lp74 = NULL;
		}
	}
}

void CSASProtocol::SetupAFT72PollPromoPersonalBankerType( CLongPoll72Message *lp72, CLongPoll74Message *lp74, bool &proceedWithPromoTransfer  )
{
	switch (m_pbt->GetPromoPersonalBankerType())
	{
	case PromoCreditsUpload:
	case PromoCreditsUploadAuto:
		// Add NCEP Out check here
		if (lp74->m_currentRestrictedAmount < m_pbt->GetMinPromoUp()* ONE_DOLLAR_IN_CENTS)
		{
			// amount too small
			m_pbt->SetPromoCancelledType(UnderTransferLimit);
			lp72->SetRestrictedAmount(0);
			proceedWithPromoTransfer = false;
		}
		else if (lp74->m_currentRestrictedAmount > m_pbt->GetMaxPromoUp()* ONE_DOLLAR_IN_CENTS)
		{
			// amount too large
			m_pbt->SetPromoCancelledType(OverTransferLimit);
			m_pbt->SetRestrictedAmount(lp74->m_currentRestrictedAmount);
			lp72->SetRestrictedAmount(0);
			proceedWithPromoTransfer = false;
		}
		else
		{
			proceedWithPromoTransfer = true;
			m_pbt->SetRestrictedAmount(lp74->m_currentRestrictedAmount);
			lp72->SetTransferType(SAS_TRANSFER_TYPE_CASH_TO_HOST);
			lp72->SetRestrictedAmount(lp74->m_currentRestrictedAmount);
		}
		break;
	default:
		LogString("PBT", "Unexpected personal banker promo type %d", 
			(int)m_pbt->GetPromoPersonalBankerType());
		proceedWithPromoTransfer = false;
		break;
	}
}

void CSASProtocol::Setup72AFTPollOtherInfo( CLongPoll72Message *lp72, CLongPoll74Message *lp74 )
{
		// asset number
		lp72->SetAssetNumber(lp74->m_assetNumber);

		// transactionID
		byte *id = (byte *)m_pbt->GetID();
		byte idLength = m_pbt->GetPBTIDLength(); 
		lp72->SetTransactionID(id, idLength);

		// Set Receipt data to player card number
		int cardLen;
		const byte *cardNum = m_pbt->GetCardNumber(cardLen);
			
		char *cardStr = CUtilities::BigEndianToHexCharArray( cardNum, cardLen );
			
		lp72->SetReceiptData((byte *)cardStr,cardLen*CUtilities::HEX_CHARS_PER_BYTE);
		delete [] cardStr;
}

CLongPoll72Message *CSASProtocol::SetupAFT72Poll( bool &proceedWithTransfer )
{
	// lock is active, lock data in m_lp74 member variable
		
	bool proceedWithCashTransfer = true;
	bool proceedWithPromoTransfer = true;

	CLongPoll74Message *lp74 = (CLongPoll74Message *)m_lp74;
	CLongPoll72Message *lp72 = new CLongPoll72Message(m_gameAddress);
	lp72->ClearStatusQuery();
	GetCurrentCreditMeter(m_pbt);
	SetupAFT72PollPersonalBankerType( lp72, lp74, proceedWithCashTransfer );
	SetupAFT72PollPromoPersonalBankerType( lp72, lp74, proceedWithPromoTransfer );


	proceedWithTransfer = proceedWithCashTransfer || proceedWithPromoTransfer;
	if (proceedWithTransfer)
	{
		LogString(PBT_LOG, "%s %d: proceedWithCashTransfer=%u proceedWithPromoTransfer=%u", __FUNCTION__, __LINE__, 
			proceedWithCashTransfer, proceedWithPromoTransfer);
		Setup72AFTPollOtherInfo( lp72, lp74 );
	}

	return lp72;
}

void CSASProtocol::CancelPBTransfer(PollerCancellationType cancelType)
{
	CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, CancelPBTransfer(): Cancelling pbt with cancellation type of %d",
		(int)cancelType);
	m_sasState.SetLockPending(false);

	// does lock data need to be deleted?
	if (m_lp74 != NULL)
	{
		delete m_lp74;
		m_lp74 = NULL;
	}

	if (m_pbt != NULL) 
	{
		// cancel lock
		CancelPBLock();

		m_pbt->SetCancelledType(cancelType);
		GetFinalCreditMeter();

		//ASSERT( m_pbt != NULL );

		CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, m_bpt session = %d", (int)m_pbt->GetPersonalBankerType());
		CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, m_bpt card id length = %d", (int)m_pbt->GetCardNumberLen());
		CInternalMessage *iMsg = new CInternalMessage(INT_MSG_PB_TRANSFER, m_pbt, m_state);
		m_pbt = NULL;
		SendMessageToInternals( iMsg, QUEUE_PRIORITY_HIGHEST ); 
	}
}

void CSASProtocol::UpdatePBTWithTransferStatus(byte transferStatus)
{

	CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, UpdatePBT(): Transfer Status of %x", transferStatus);

	PollerCancellationType cancelType = _Unknown;

	switch (transferStatus)
	{
	case TRANSFER_STATUS_FULL_SUCCESSFUL:
	case TRANSFER_STATUS_PARTIAL_SUCCESSFUL:
		// transfer successful, send info to internals
		cancelType = _Unknown;
		break;
	default:
		// there was an error
		switch(transferStatus)
		{
		case AFTErrorCanceledBySystem:
			cancelType = CanceledBySystem;
			break;
		case AFTErrorSASDuplicateTransferNum:
			cancelType = SASDuplicateTransferNum;
			break;
		case AFTErrorUnsupportedTransCode:
			cancelType = UnsupportedTransCode;
			break;
		case AFTErrorAllowableRangeExceeded:
			cancelType = AllowableRangeExceeded;
			break;
		case AFTErrorOverTransferLimit:
			cancelType = OverTransferLimit;
			break;
		case AFTErrorAmountNotEvenMult:
			cancelType = AmountNotEvenMult;
			break;
		case AFTErrorGeneralError:
			cancelType = GeneralError;
			break;
		case AFTErrorMachineTilt:
			cancelType = MachineTilt;
			break;
		case AFTErrorGameNotRegistered:
			cancelType = GameNotRegistered;
			break;
		case AFTErrorRegistrationKeyNotMatched:
			cancelType = GameNotRegistered;
			break;
		case AFTErrorNoPosID:
			cancelType = NoPosID;
			break;
		case AFTErrorNoCredits:
			cancelType = NoCredits;
			break;
		case AFTErrorBartGameDenomMismatch:
			cancelType = BartGameDenomMismatch;
			break;
		case AFTErrorTicketAlreadyExpired:
			cancelType = TicketReceiptError;
			break;
		case AFTErrorTicketDeviceNotAvailable:
			cancelType = TicketReceiptError;
			break;
		case AFTErrorRestrictedPoolIDMismatch:
			cancelType = RestrictedPoolIDMismatch;
			break;
		case AFTErrorReceitDeviceNotAvailable:
			cancelType = TicketReceiptError;
			break;
		case AFTErrorInsuficientReceiptData:
			cancelType = TicketReceiptError;
			break;
		case AFTErrorReceiptNotAllowed:
			cancelType = TicketReceiptError;
			break;
		case AFTErrorAssetNumberNoMatch:
			cancelType = GameAssetNumError;
			break;
		case AFTErrorGameNotLocked:
			cancelType = GameNotLocked;
			break;
		case AFTErrorInvalidTransID:
			cancelType = InvalidTransID;
			break;
		case AFTErrorUnexpectedError:
			cancelType = GeneralError;
			break;
		case AFTErrorNotCompatable:
			cancelType = IncompatiblePoll;
			break;
		case AFTErrorUsupportedTransfercode:
			cancelType = UnsupportedTransCode;
			break;
		case AFTErrorNoTransferInfoAvailable:
			cancelType = TransInfoUnavailable;
			break;
		default:
			cancelType = _Unknown;
			break;
		}
		m_pbt->SetCancelledType(cancelType);
		m_pbt->SetPersonalBankerType(m_pbt->GetCancelledCode(m_pbt->GetPersonalBankerType()));
	}
}

void CSASProtocol::ProcessGeneralPollXferComplete()
{
	// transfer has been completed

	LogString("SASGENERAL", "Process General Poll Transfer Complete");

	// Make a 72 poll to do a transfer querry.
	CLongPoll72Message *lp72 = new CLongPoll72Message(m_gameAddress);
	lp72->SetStatusQuery();
	lp72->Fill();
	m_sasPollQueue->Add(lp72, QUEUE_PRIORITY_HIGHEST);
	lp72->WaitForResponse();

	LogString("SASGENERAL", "Most recent LP72 transaction results: TransactionID [%s] ReturnValue [0x%x] TransferStatus [0x%x]", 
		string((const char*)lp72->m_transactionID, lp72->m_transactionIDLength).c_str(), lp72->GetReturnValue(), lp72->m_transferStatus);

	if (0 == strncmp((char *)lp72->m_transactionID, ATIGENERIC_TRANSACTION_ID_PREFIX, strlen(ATIGENERIC_TRANSACTION_ID_PREFIX)))
	{
		// Update cashout to host SAS state since we may have just completed the LP72 to enable/disable the feature
		// SAS documentation: Response to LP72 always indicated current host cashout state unless the response data
		// is from the history buffer (not in our case).
		bool cashoutToHostEnabled = lp72->GetTransferFlag(TRANSFER_FLAG_HOST_CASHOUT_ENABLE);
		m_sasState.SetCashoutToHostEnabledOnEGM(cashoutToHostEnabled);

		LogString("SASGENERAL", "SAS Generic AFT transaction completed. CashoutToHostEnabledOnEgm: %d TransferFlags: 0x%x", 
			cashoutToHostEnabled, lp72->m_transferFlags);

		// Send host cashout status (first 3 bits of transfer flags) to internals for web diagnostics display
		SendMessageToInternals(new CInternalMessage(INT_MSG_CASHOUT_TO_HOST_STATUS,
			(WORD)(lp72->m_transferFlags & 0x7)), QUEUE_PRIORITY_NORMAL);
		return;
	}

	// look at status, if not pending then handle response
	if (lp72->GetReturnValue() == SASPOLL_SUCCESS && lp72->m_transferStatus != TRANSFER_STATUS_PENDING)
	{
		// Determine which kind of transfer was complete based upon the transfer type
		if (lp72->GetTransferType() == SAS_TRANSFER_TYPE_COIN_OUT_BONUS || lp72->GetTransferType() == SAS_TRANSFER_TYPE_JACKPOT_BONUS)
		{
			ProcessAFTBonusXferComplete(lp72);
		}
		else
		{
			ProcessPBTransferComplete(lp72);
		}
	}


	// clean up
	delete lp72;

}

void CSASProtocol::ProcessPBTransferComplete(CLongPoll72Message *lp72)
{
	// transfer has been completed

	lock_guard<recursive_mutex> lock(m_pbtCritSection);
	CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ProcessPBTransferComplete(): Transfer Complete");

	// do long poll 0x72 to get final status
	bool newPbt = false; 
	if (m_pbt == NULL)
	{
		// probably responding to a completed transfer on fresh boot, make new pbt to send to internals
		m_pbt = new CPersonalBankerTransfer(CUtilities::GetTickCount());
		newPbt = true;
	}

	// look at status, if not pending then handle response
	if (lp72->GetReturnValue() == SASPOLL_SUCCESS && lp72->m_transferStatus != TRANSFER_STATUS_PENDING)
	{
		if (newPbt)
		{
			// need to fill in amount data from long poll response
			if (lp72->GetTransferType() != SAS_TRANSFER_TYPE_CASH_TO_GAME)
			{
				m_pbt->SetAmount(lp72->m_cashableAmount);
				m_pbt->SetNonrestrictedAmount(lp72->m_nonrestrictedAmount);
				m_pbt->SetRestrictedAmount(lp72->m_restrictedAmount);
			}
			else
			{
				// find which amount was transferred
				if (lp72->m_cashableAmount > 0)
					m_pbt->SetAmount(lp72->m_cashableAmount);
				else if (lp72->m_nonrestrictedAmount > 0)
					m_pbt->SetAmount(lp72->m_nonrestrictedAmount);
				else
					m_pbt->SetAmount(lp72->m_restrictedAmount);
			}
			if (lp72->GetCashableAmount() > 0)
			{
				if (lp72->GetTransferType() == SAS_TRANSFER_TYPE_CASH_TO_GAME)
				{
					// cash download
					m_pbt->SetPersonalBankerType(CashDownload);
					m_pbt->SetPersonalBankerTransferType(CashToGame);
				}
				else if (lp72->GetTransferType() == SAS_TRANSFER_TYPE_CASH_TO_HOST)
				{
					// cash upload
					m_pbt->SetPersonalBankerType(CashUpload);
					m_pbt->SetPersonalBankerTransferType(CashFromGame);
				}
			}
			else if (lp72->GetRestrictedAmount() > 0)
			{
				if (lp72->GetTransferType() == SAS_TRANSFER_TYPE_CASH_TO_GAME)
				{
					// restricted download
					m_pbt->SetPersonalBankerType(PromoCreditsToCash);
					m_pbt->SetPersonalBankerTransferType(PromoDollarsToGame);
				}
				else if (lp72->GetTransferType() == SAS_TRANSFER_TYPE_CASH_TO_HOST)
				{
					// restricted upload
					m_pbt->SetPersonalBankerType(PromoCreditsUpload);
					m_pbt->SetPersonalBankerTransferType(PromoDollarsFromGame);
				}
			}
			else if (lp72->GetNonrestrictedAmount() > 0)
			{
				if (lp72->GetTransferType() == SAS_TRANSFER_TYPE_CASH_TO_GAME)
				{
					// nonrestricted download
					m_pbt->SetPersonalBankerType(PointRedemptionToCash);
					m_pbt->SetPersonalBankerTransferType(PointsToGame);
				}
				else if (lp72->GetTransferType() == SAS_TRANSFER_TYPE_CASH_TO_HOST)
				{
					// nonrestricted upload
					m_pbt->SetPersonalBankerType(CashUpload);
					m_pbt->SetPersonalBankerTransferType(CashFromGame);
				}
			}
			else
			{
				// failed attempt, need to use type to decide what the transaction was
				if (lp72->GetTransferType() == SAS_TRANSFER_TYPE_CASH_TO_GAME)
				{
					// TBD: assuming failed cash download, could have been points or promo download
					m_pbt->SetPersonalBankerType(CashDownload);
					m_pbt->SetPersonalBankerTransferType(CashToGame);
				}
				else
				{
					m_pbt->SetPersonalBankerType(CashUpload);
					m_pbt->SetPersonalBankerTransferType(CashFromGame);
				}
			}
		}
		UpdatePBTWithTransferStatus(lp72->m_transferStatus);
		m_pbt->SetID((char *)lp72->m_transactionID,lp72->m_transactionIDLength);
		m_pbt->SetAftTransferFlags(lp72->m_transferFlags);

		// Check Transaction ID returned from game
		if (!m_pbt->IsValidID(m_sasState.GetSlotmastID(), m_pbt->GetID(), m_pbt->GetPBTIDLength()))
		{
			// Received invalid ID, log it
			// Send tilt transaction 
			SendTiltToInternals(tltInvalidPbIdAfter);

			CDiagnostics::ReportPBTTicketDiagnosticToFile(CDirectoryManager::PBTTicketDiagnosticsFilePath(), CUtilities::LoadResourceString(IDS_PBT_ID_INVALID_AFTER), m_pbt->ToStringForLog(), tltInvalidPbIdAfter);
		}
		// Send it
		GetFinalCreditMeter();
		GetPBTMeters();

		//ASSERT( m_pbt != NULL );
		
		CInternalMessage *iMsg = new CInternalMessage(INT_MSG_PB_TRANSFER, m_pbt, m_state);
		m_pbt = NULL;
		SendMessageToInternals( iMsg, QUEUE_PRIORITY_HIGHEST ); 
	}
}

void CSASProtocol::ProcessAFTBonusXferComplete(CLongPoll72Message *lp72)
{
	LogString("SASGENERAL", "SAS AFT bonusing complete.");

	// Get the transaction that is in the poll response
	char transIDStr[MAX_PBID_LENGTH + 1];
	int transIDLen = lp72->m_transactionIDLength;
	if (transIDLen > MAX_PBID_LENGTH)
		transIDLen = MAX_PBID_LENGTH;
	memcpy(transIDStr, lp72->m_transactionID, transIDLen);
	transIDStr[transIDLen] = '\0';
	string transactionIDFromPoll(transIDStr);
	
	lock_guard<recursive_mutex> lock(m_egmAwardCritSection);
	if (m_egmAward != NULL)
	{
		string transactionIDFromAward = GetTransactionId(m_egmAward->GetJackpotId());
		if (transactionIDFromPoll == transactionIDFromAward)
		{
			LPCTSTR statusInfo(nullptr);
			m_egmAward->SetAwardState(GetAwardResultFromTransferStatus(lp72->m_transferStatus, statusInfo));
			if (m_egmAward->GetAwardState() == Signaled)
			{
				m_egmAward->SetAwardAmountPaid(lp72->GetCashableAmount());
			}

			m_egmAward->SetStatusInfo(statusInfo);
		}

		// Send result to Internals
		m_egmAward->LogInfo(_T("MM SAS PB AFT send result to internals from ProcessAFTBonusXferComplete"));
		CEGMAward *copyAward = new CEGMAward(*m_egmAward);
		CInternalMessage *awardMsg = new CInternalMessage(INT_MSG_EGM_AWARD, copyAward);
		SendMessageToInternals(awardMsg, QUEUE_PRIORITY_HIGHEST);
	}

}

void CSASProtocol::CancelPBLock()
{

	CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, CancelPBLock()");
	// this will send a cancel lock request to the machine
	CLongPoll74Message lp74(m_gameAddress);
	lp74.SetLockCode(LOCK_CODE_CANCEL_PENDING);
	lp74.Fill();
	m_sasPollQueue->Add(&lp74, QUEUE_PRIORITY_HIGHEST);
	lp74.WaitForResponse();
}

void CSASProtocol::CancelEFT(PollerCancellationType type)
{
	CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, CancelEFT() cancelling with cancellation type of %d", (int)type);
	// this will send a cancelled pbt
	m_sasPollQueue->ClearQueueStarvation();
	//m_pbtStartTickCount = 0;
	m_pbt->SetCancelledType(type);

	// set cancel codes for bptransfer, send to internals
	m_pbt->ChangePersonalBankerTypeToCanceled();

	//m_pbtStartTickCount = 0;
	GetFinalCreditMeter();

	//ASSERT( m_pbt != NULL );

	LogString("SASGENERAL", "Cancel EFT");
	CInternalMessage *iMsg = new CInternalMessage(INT_MSG_PB_TRANSFER, m_pbt, m_state);
	m_pbt = NULL;
	SendMessageToInternals( iMsg, QUEUE_PRIORITY_HIGHEST ); 
}

void CSASProtocol::GetCurrentCreditMeter(CPersonalBankerTransfer *pbt)
{
	// Add current credit meter into pbt object
	CMeters *meters = new CMeters(MEMORY_NONE);
	meters->SetMeter( mCrd );	// Current Credits Meter
	GetMeters( *meters, EGM_GAME_NUMBER );
	if (meters->GetMeterByNumber(mCrd)->GetValue() != INVALID_METER_VALUE)
	{
		pbt->SetOriginalCreditMeter(meters->GetMeterByNumber(mCrd)->GetValue());
	}
	delete meters;
}

void CSASProtocol::GetFinalCreditMeter()
{
	CMeters *meters = new CMeters(MEMORY_NONE);
	meters->SetMeter( mCrd );	// Current Credits Meter
	GetMeters( *meters, EGM_GAME_NUMBER );

	// send to internals
	//m_pbtStartTickCount = 0;
	CInternalMessage *iMsg = new CInternalMessage(INT_MSG_SET_METERS,meters, EGM_GAME_NUMBER);
	SendMessageToInternals( iMsg, QUEUE_PRIORITY_HIGHEST ); 

}

void CSASProtocol::ProcessPBStatus(CInternalMessage *msg)
{
	lock_guard<recursive_mutex> lock(m_pbtCritSection);

	CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ProcessPBStatus()");
	m_pbt = msg->GetPBTransferData();
	m_state = (PersonalBankerState)msg->GetWordData();
	msg->SetData((CPersonalBankerTransfer *)NULL);


	if (m_bIsAFTSupported)
	{
		CLongPoll72Message *lp72 = new CLongPoll72Message(m_gameAddress);
		lp72->SetStatusQuery();

		// send it
		lp72->Fill();
		m_sasPollQueue->Add(lp72,QUEUE_PRIORITY_HIGHEST);
		lp72->WaitForResponse();
		
		// look at status, if not pending then handle response
		if (lp72->GetReturnValue() == SASPOLL_SUCCESS && lp72->m_transferStatus != TRANSFER_STATUS_PENDING)
		{
			
			// fill in transfer amounts
			if ((lp72->GetTransferType() == SAS_TRANSFER_TYPE_CASH_TO_GAME))
			{
				// find which amount was transferred
				if (lp72->m_cashableAmount > 0)
				{					
					m_pbt->SetAmount(lp72->m_cashableAmount);
				}
				else if (lp72->m_nonrestrictedAmount > 0)
				{
					m_pbt->SetAmount(lp72->m_nonrestrictedAmount);
				}
				else
				{
					m_pbt->SetAmount(lp72->m_restrictedAmount);
				}				
			}
			else
			{
				m_pbt->SetAmount(lp72->m_cashableAmount);
				m_pbt->SetNonrestrictedAmount(lp72->m_nonrestrictedAmount);
				m_pbt->SetRestrictedAmount(lp72->m_restrictedAmount);
			}


			UpdatePBTWithTransferStatus(lp72->m_transferStatus);
			m_pbt->SetID((char *)lp72->m_transactionID,lp72->m_transactionIDLength);

			// Send it
			GetFinalCreditMeter();
			GetPBTMeters();

			//ASSERT( m_pbt != NULL );

			CInternalMessage *iMsg = new CInternalMessage(INT_MSG_PB_TRANSFER, m_pbt, m_state);
			m_pbt = NULL;
			SendMessageToInternals( iMsg, QUEUE_PRIORITY_HIGHEST ); 
		}

		// clean up
		delete lp72;
	}
	else if (m_bIsEFTSupported)
	{
		// try lp 28 to see what the last pb transfer was
		CLongPoll28Message lp28(m_gameAddress);
		lp28.Fill();
		m_sasPollQueue->Add(&lp28, QUEUE_PRIORITY_NORMAL);
		lp28.WaitForResponse();
		if (lp28.GetReturnValue() == SASPOLL_SUCCESS)
		{
			// Compare results with m_pbt passed from internals
			if(m_pbt->GetEFTNumber() == lp28.GetEFTSeed())
			{
				// this was the same pbt, report result
				std::stringstream ss;
				ss << lp28.GetEFTSeed();
				const char *id = ss.str().c_str();
				m_pbt->SetID(id, strlen(id));
				if ( lp28.m_lastAck == 1 && lp28.m_lastStatus == 0)
				{
					// last eft was successful
					m_pbt->SetCancelledType(_Unknown);

					if (lp28.m_lastAmount*m_denomMult != m_pbt->GetAmount())
					{
						// this shouldn't happen
						//ASSERT(0);
					}
					// send to internals
					GetFinalCreditMeter();
					GetPBTMeters();

					//ASSERT( m_pbt != NULL );

					CInternalMessage *msg = new CInternalMessage(INT_MSG_PB_TRANSFER, m_pbt, m_state);

					m_pbt = NULL;
					SendMessageToInternals( msg, QUEUE_PRIORITY_HIGHEST ); 
				}
				else if (lp28.m_lastAck == 0)
				{
					// Last EFT attempt failed before final ack was sent to EGM
					CancelEFT(SASInvalidAckMessage);
				}
				else
				{
					// not successful
					HandleEFTErrorStatus(lp28.m_lastStatus);
				}
			}
			else
			{
				// last completed eft was different from what internals asked about, this would only happen if last eft failed before hitting EGM.
				CancelEFT(GeneralError);
			}
		}
		else
		{
			// if long poll 28 fails, then cancel
			//ASSERT(0);
			CancelEFT(GeneralError); // Shouldn't get this far
		}
	}
}

void CSASProtocol::CancelSASPBTransfer()
{
	CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, CancelSASPBTransfer()");
	if (m_bIsAFTSupported)
	{
		LogString("SASGENERAL", "Cancel SAS PBT Transfer: tell SAS");
		// Try to tell SAS to cancel pending transfer.  
		// check for pending or current game lock.
		// if pending, cancel it and cancel pbt
		// if current game lock, check for pending transfer
		// if not pending, cancel pbt and send transfer status to internals
		// if pending, do lp 72 cancel and check result.
		CLongPoll74Message lp74(m_gameAddress);
		lp74.SetLockCode(LOCK_CODE_QUERY_STATUS);
		lp74.Fill();
		m_sasPollQueue->Add(&lp74,QUEUE_PRIORITY_HIGHEST);
		lp74.WaitForResponse();

		if (lp74.GetReturnValue() == SASPOLL_SUCCESS)
		{
			if (lp74.GetLockCode() == LOCK_CODE_GAME_LOCKED)
			{
				// Game is currently locked, try to cancel pbt
				// check status of last pbt
				CLongPoll72Message lp72(m_gameAddress);
				lp72.SetStatusQuery();
				lp72.Fill();
				m_sasPollQueue->Add(&lp72, QUEUE_PRIORITY_HIGHEST);
				lp72.WaitForResponse();

				if (lp72.GetReturnValue() == SASPOLL_SUCCESS)
				{
					if (lp72.m_transferStatus == TRANSFER_STATUS_PENDING)
					{
						// still pending, try to cancel
						CLongPoll72Message cancelPbt(m_gameAddress);
						cancelPbt.SetCashableAmount(0);
						cancelPbt.SetNonrestrictedAmount(0);
						cancelPbt.SetRestrictedAmount(0);
						cancelPbt.SetTransferCode(TRANSFER_REQUEST_CANCEL_REQUEST);
						cancelPbt.SetTransactionID((byte *)m_pbt->GetID(),m_pbt->GetPBTIDLength());
						cancelPbt.Fill();
						m_sasPollQueue->Add(&cancelPbt,QUEUE_PRIORITY_HIGHEST);
						cancelPbt.WaitForResponse();
						if (cancelPbt.GetReturnValue() == SASPOLL_SUCCESS)
						{
							if (cancelPbt.m_transferStatus != TRANSFER_STATUS_PENDING)
							{
								// update final status
								UpdatePBTWithTransferStatus(cancelPbt.m_transferStatus);
								GetFinalCreditMeter();
								
								//ASSERT( m_pbt != NULL );

								LogString("SASGENERAL", "Cancel SAS PBT Transfer: pending canceled");
								CInternalMessage *iMsg = new CInternalMessage(INT_MSG_PB_TRANSFER, m_pbt, m_state);
								m_pbt = NULL;
								SendMessageToInternals( iMsg, QUEUE_PRIORITY_HIGHEST ); 

							}
						}
					}
					else
					{
						// not pending, report current/final status to internals
						UpdatePBTWithTransferStatus(lp72.m_transferStatus);
						GetFinalCreditMeter();

						//ASSERT( m_pbt != NULL );

						LogString("SASGENERAL", "Cancel SAS PBT Transfer: not pending");
						CInternalMessage *iMsg = new CInternalMessage(INT_MSG_PB_TRANSFER, m_pbt, m_state);
						m_pbt = NULL;
						SendMessageToInternals( iMsg, QUEUE_PRIORITY_HIGHEST ); 
					}
				}
			}
			// Tracker #28746 only do this if we are not waiting for a lock
			else if (lp74.GetLockCode() != LOCK_CODE_GAME_LOCK_PENDING && !m_sasState.GetLockPending())
			{
				// Tracker #23163 - No longer pending, but no status.  Need to get status of last transfer
				// Same functionality as ProcessGeneralPollXFerComplete(), so use that
				ProcessGeneralPollXferComplete();
			}
			else
			{
				// lock still pending, cancel it
				CancelPBLock();

				// cancel pbt
				if (m_pbt)
					m_pbt->ChangePersonalBankerTypeToCanceled();
				CancelPBTransfer(SASTimeout);
			}
		}
	}
}

void CSASProtocol::GetPBBalances()
{
	CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, GetPBBalances()");
	CInternalMessage *msg = NULL;
	if (m_bIsAFTSupported)
	{

		// query values on game
		CLongPoll74Message *lp74 = new CLongPoll74Message(m_gameAddress);
		lp74->SetLockCode(LOCK_CODE_QUERY_STATUS);

		lp74->Fill();
		m_sasPollQueue->Add(lp74,QUEUE_PRIORITY_HIGHEST);
		lp74->WaitForResponse();

		if (lp74->GetReturnValue() == SASPOLL_SUCCESS)
		{
			DWORD cashBalance, promoBalance, cashablePromo;
			cashBalance = lp74->m_currentCashableAmount ;
			cashablePromo = lp74->m_currentNonrestrictedAmount;
			promoBalance = lp74->m_currentRestrictedAmount;
			
			// Tracker #23130 - Try to detect if transfer from game would succeed or not
			bool canTransferOff = lp74->ResponseXferFromGame();
			msg = new CInternalMessage(INT_MSG_SET_PB_BALANCES, cashBalance, promoBalance);
			msg->SetData((__int64)cashablePromo);
			msg->SetData((WORD)canTransferOff);
			LogString(PBT_LOG, "GetPBBalances AFT cashBalance=%u promoBalance=%u cashablePromo=%u canTransferOff=%d", 
				cashBalance, promoBalance, cashablePromo, canTransferOff);
		}
		else
		{
			msg = new CInternalMessage(INT_MSG_SET_PB_BALANCES, (DWORD)0, (DWORD)0);
			msg->SetData((__int64)0);
			msg->SetData((WORD)0, (WORD)1);
		}
		delete lp74;
	}
	else if (m_bIsEFTSupported)
	{
		// send current credits as cashable amount.  Might not be all cashable, but gives an estimation without starting a new transaction that won't get completed.
		CPersonalBankerTransfer *tmpPbt = new CPersonalBankerTransfer(CUtilities::GetTickCount(), MEMORY_NONE);
		GetCurrentCreditMeter(tmpPbt);
		msg = new CInternalMessage(INT_MSG_SET_PB_BALANCES,(DWORD)tmpPbt->GetOriginalCreditMeter(),(DWORD)0);
		LogString(PBT_LOG, "GetPBBalances EFT cashBalance=%u", (DWORD)tmpPbt->GetOriginalCreditMeter());
		bool eftMode(true);
		msg->SetData((__int64)0, (__int64)eftMode);
		delete tmpPbt;
	}
	else
	{
		// no pb supported, send 0's
		msg = new CInternalMessage(INT_MSG_SET_PB_BALANCES,(DWORD)0,(DWORD)0);
		msg->SetData((__int64)0);
	}

	SendMessageToInternals(msg,QUEUE_PRIORITY_NORMAL);

}

void CSASProtocol::ResendPBTLockRequest()
{
	// resend lock request
	
	CPersonalBankerTransfer::LogPBT(m_pbt, "SAS PBT, ResendLockRequest()");
	bool deletelp74 = true;
	CLongPoll74Message *lp74 = new CLongPoll74Message(m_gameAddress);
	lp74->SetLockCode(LOCK_CODE_REQUEST_LOCK);
	lp74->SetLockTimeout(LOCK_DURATION);

	// set desired transfer condition
	switch(m_pbt->GetPersonalBankerType())
	{
	case CashUpload:
	case CashUploadAuto:
		lp74->SetXferFromGame(true);
		break;
	case CashDownload:
	case PromoCreditsToCash:
	case PointRedemptionToCash:
	case PointRedemptionToPromoCredits:
		lp74->SetXferToGame(true);
		break;
	default:
		break;
	}

	// look for ncep out
	if (m_pbt->GetPromoPersonalBankerType() == PromoCreditsUpload ||
		m_pbt->GetPromoPersonalBankerType() == PromoCreditsUploadAuto)
	{
		lp74->SetXferFromGame(true);
	}

	lp74->Fill();
	m_sasPollQueue->Add(lp74,QUEUE_PRIORITY_HIGHEST);
	m_sasState.SetLastLockRequest(CUtilities::GetTickCount());
	lp74->WaitForResponse();

	if (lp74->GetReturnValue() == SASPOLL_SUCCESS)
	{
		// if cash from game, get values
		if (m_pbt->GetPersonalBankerType() == CashUpload ||
			m_pbt->GetPersonalBankerType() == CashUploadAuto)
		{
			// fill in the two buckets that will be taken off of the game... Restricted money stays on game.
			m_pbt->SetAmount(lp74->m_currentCashableAmount);
			m_pbt->SetNonrestrictedAmount(lp74->m_currentNonrestrictedAmount);
		}

		// Check return values
		m_sasState.SetLockPending(false);
		switch (lp74->m_gameLockStatus)
		{
		case LOCK_CODE_GAME_LOCK_PENDING:
			// Do nothing, wait for the general exception x6f
			m_sasState.SetLockPending(true);
			break;
		case LOCK_CODE_GAME_NOT_LOCKED:
			// lock has been rejected, clear m_pbt and report error
			m_pbt->ChangePersonalBankerTypeToCanceled();
			CancelPBTransfer(LockReject);
			break;
		case LOCK_CODE_GAME_LOCKED:
			// lock has been granted, start process.
			deletelp74 = false;
			m_lp74 = lp74;
			ExecutePBTransfer();
			break;
		}
	}
	else
	{
		// lock poll failed, cancel request, clear m_pbt and report error
		m_pbt->ChangePersonalBankerTypeToCanceled();
		CancelPBTransfer(LockReject);
		
	}
	
	if (deletelp74)
		delete lp74;
}

void CSASProtocol::ResendAwardLockRequest()
{
	AwardAFTBonus();

	if (!m_sasState.GetLockPending())
	{
		// We no longer think the lock is pending. Therefore send the status back to internals
		lock_guard<recursive_mutex> lock(m_egmAwardCritSection);
		if (NULL != m_egmAward)
		{
			m_egmAward->LogInfo(_T("ss lock no longer pending so send result to internals from ResendAwardLockRequest"));
			CEGMAward *copyAward = new CEGMAward(*m_egmAward);
			CInternalMessage *awardMsg = new CInternalMessage(INT_MSG_EGM_AWARD, copyAward);
			SendMessageToInternals(awardMsg, QUEUE_PRIORITY_HIGHEST);
		}
	}
}

void CSASProtocol::ReturnAwardIneligible()
{
	lock_guard<recursive_mutex> lock(m_egmAwardCritSection);	
	m_egmAward->SetAwardState(m_egmAward->IsFromPromoGame() ? AwardToGameError : AwardToGameIneligible);
	m_egmAward->SetStatusInfo(CEGMAward::TEXT_SAS_AFT_ERROR_TIMED_OUT_WAITING_FOR_LOCK);
	m_egmAward->LogInfo(_T("[SAS Member]"));
	CInternalMessage *msg = new CInternalMessage(INT_MSG_EGM_AWARD, new CEGMAward(*m_egmAward));

	SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);
}

void CSASProtocol::ProbePBT(bool isTransferToGame)
{
	bool bCanLock = true;
	bool bShouldSend = true;
	LogString(PBT_LOG, "isTransferToGame = %d", isTransferToGame);
	if (m_bIsAFTSupported)
	{
		LogString(PBT_LOG, "Begin PB Lock probe on AFT EGM");

		// Try to get a AFT Lock
		CLongPoll74Message lp74(m_gameAddress);
		lp74.SetLockCode(LOCK_CODE_REQUEST_LOCK);
		if (isTransferToGame)
		{
			lp74.SetXferToGame(true);
		}
		else
		{
			lp74.SetXferFromGame(true);
		}
		lp74.SetLockTimeout(PROBEPBTLOCKTIME);
		lp74.Fill();

		m_sasPollQueue->Add(&lp74, QUEUE_PRIORITY_HIGHEST);
		m_sasState.SetLastLockRequest(CUtilities::GetTickCount());
		lp74.WaitForResponse();

		if (lp74.GetReturnValue() == SASPOLL_SUCCESS)
		{
			// Check return values
			m_sasState.SetLockPending(false);
			switch (lp74.m_gameLockStatus)
			{
			case LOCK_CODE_GAME_LOCK_PENDING:
				m_sasState.SetProbingPBTLock(true);
				bShouldSend = false;
				break;
			case LOCK_CODE_GAME_NOT_LOCKED:
				// Clear probing lock flag just in case it was set
				m_sasState.SetProbingPBTLock(false);
				bCanLock = false;
				LogString(PBT_LOG, "ProbePBT : Lock Status = Game Not Locked. Sending tilt %d to internals.", tltProbePBTAccessError);
				SendTiltToInternals(tltProbePBTAccessError);
				break;
			case LOCK_CODE_GAME_LOCKED:
				// Clear probing lock flag just in case it was set
				m_sasState.SetProbingPBTLock(false);

				// Don't keep lock
				CancelPBLock();
				break;
			}
		}
	}
	else
	{
		LogString(PBT_LOG, "Cannot perform PB Lock probe on AFT. Returning successful lock");
	}
	
	if (bShouldSend) 
	{
		CInternalMessage *msg = new CInternalMessage(INT_MSG_PROBE_PBT_RESULT, (WORD) bCanLock);
		SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);
	}
}

void CSASProtocol::CheckAFTLockForPrizeAward()
{
	lock_guard<recursive_mutex> lock(m_egmAwardPrizeCritSection);
	if (m_egmPrizeAward)
	{
		// Try to get a AFT Lock
		CLongPoll74Message lp74(m_gameAddress);
		lp74.SetLockCode(LOCK_CODE_QUERY_STATUS);
		lp74.SetLockTimeout(0);
		lp74.Fill();

		m_sasPollQueue->Add(&lp74,QUEUE_PRIORITY_HIGHEST);
		lp74.WaitForResponse();

		if (lp74.GetReturnValue() == SASPOLL_SUCCESS)
		{
			// Check return values
			m_sasState.SetLockPending(false);
			switch (lp74.m_gameLockStatus)
			{
			case LOCK_CODE_GAME_NOT_LOCKED:
				LogString(PROGRESSIVE, "CheckAFTLockForPrizeAward-LOCK_CODE_GAME_NOT_LOCKED");
				ReportPrizeAwardStatus(AwardToGameError);
				break;
			case LOCK_CODE_GAME_LOCKED:
				LogString(PROGRESSIVE, "CheckAFTLockForPrizeAward-LOCK_CODE_GAME_LOCKED");
				ReportPrizeAwardStatus(Signaled);
				break;
			}
		}
	}
}


void CSASProtocol::CheckIfPendingAutoUploadNeedsBeCancelled(GeneralPollException exceptionValue)
{
	bool sendCancelAutoUpload(false);

	switch (exceptionValue)
	{
	case GPE_GAME_STARTED:
	case GPE_CASHOUT_PRESSED:
	case GPE_TICKET_INSERTED:
	case GPE_TICKET_CASHOUT:
	case GPE_1_DOLLAR_ACCEPTED:
	case GPE_5_DOLLAR_ACCEPTED:
	case GPE_10_DOLLAR_ACCEPTED:
	case GPE_20_DOLLAR_ACCEPTED:
	case GPE_50_DOLLAR_ACCEPTED:
	case GPE_100_DOLLAR_ACCEPTED:
	case GPE_2_DOLLAR_ACCEPTED:
	case GPE_500_DOLLAR_ACCEPTED:
	case GPE_BILL_ACCEPTED:
	case GPE_200_DOLLAR_ACCEPTED:
		SendMessageToInternals(new CInternalMessage(INT_MSG_CANCEL_PENDING_AUTO_UPLOAD, WORD(exceptionValue)), QUEUE_PRIORITY_HIGHEST);
		break;
	default:
		break;
	}
}

void CSASProtocol::ResetPBTProbe()
{
	LogString(PBT_LOG, "Resetting PBT Probe status.");
	CLongPoll74Message lp74(m_gameAddress);
	lp74.SetLockCode(LOCK_CODE_CANCEL_PENDING);
	lp74.SetLockTimeout(0);
	lp74.Fill();

	m_sasPollQueue->Add(&lp74, QUEUE_PRIORITY_NORMAL);
	lp74.WaitForResponse();

	if (lp74.GetReturnValue() != SASPOLL_SUCCESS)
	{
		LogString(PBT_LOG, "Error cancelling PBT Probe.");
	}

	m_sasState.SetProbingPBTLock(false);
	m_sasState.SetLockPending(false);
}
