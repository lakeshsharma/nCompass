#include "stdafx.h"
#include "PersonalBanker.h"
#include "UI/PersonalBankerLog.h"
#include "Logging/LogString.h"
#include "RapidJsonHelper.h"
#include "GlobalDefs.h"

using namespace std;

const int PROBEPBTDELAY = 1000;
const int PROBEPBTMAXTICKS = 8000;
const WORD DEFAULTRESETPINRESPONSE = 0xffff;

const string PB_INFO_SUBSCRIPTION("PBInfo");
const string PB_TRANSFER_SUBSCRIPTION("PBTransfer");
const string PB_RESET_PIN_RESPONSE("PinResetResponse");

const char* PersonalBankerActionStr[] =
{
	"PBActionNone",
	"PBActionGetEGMBalances",
	"PBActionGetEGMBalancesWithMessage",
	"PBActionSendTransferTransaction",
	"PBActionReleaseSession",
	"PBActionSendCardSessionInProgressToUI",
	"PBActionStartTransferOnly",
	"PBActionSendPersonalBankerNcepOutFail",
	"PBActionSendDisplayPBAnimationToUI",
	"PBActionSendDisplayPBAnimationToUIAndTransferToEGM",
	"PBActionSendPersonalBankerInfoToUI",
	"PBActionProcessRequestPlayerResume",
	"PBActionSendAutoTransferCancelledToUI",
	"PBActionSendCompleteToUI",
	"PBActionSendAutoUploadCancelledMessage",
	"PBActionProcessAuthorizationCanceled",
	"PBActionProcessRequestPlayerResumeDueToCashInactive",
	"PBActionProcessRequestPlayerResumeDueToGuestPlayer",
	"PBActionSendCancelCashoutToHostToEGM",
	"PBActionCancelAutoUploadDueToCashErrorSessionRelease",
	"PBActionSendTaxDeductionRequest",
};

static_assert(NUMBER_OF_PB_ACTIONS == _countof(PersonalBankerActionStr),
	"PersonalBankerActionStr has to be updated to match all the entries in PersonalBankerAction");

/// <summary>
/// Constructs a personal banker from default or NVRAM
/// </summary>
/// <param name="memoryType">Type of memory used for storing and retrieving.</param>
/// <remarks>
/// If the NVRAM location is not active then a default instance is constructed.
/// This method does not recover the CPersonalBankerTransfer and CPersonalBankerInfo instances from NVRAM.
/// ... Call ProcessInitialPersonalBanker() to recover these members from NVRAM.
/// </remarks>
CPersonalBanker::CPersonalBanker(MEMORY_TYPE memoryType) : 
CDataSubscriptionProvider(""),
CMemoryStorageEx(memoryType, NVRAM_DATA_OFFSET, NVRAM_DATA_SIZE, NVRAM_ACTIVE_OFFSET, NVRAM_ACTIVE_VALUE),
m_pbLog(MEMORY_NVRAM, LOG_PBT_EVENTS, PB_TRANSACTION_LOGS),
m_pbSuccessLog(MEMORY_NVRAM, LOG_PBT_LOG, PB_LOGS)
{
	m_waitingForTransferToBeCompleted = false;
	m_waitingForDenomCheck = false;

	m_previousTransferId = nullptr;
	m_previousTransferIdLength = 0;

	m_pbInfo = nullptr;
	m_pbTransfer = nullptr;
	m_readyAction = PBActionNone;
	m_state = PBStateUnknown;
	m_pbStateForUi = PBStateUnknown;
	m_ReleaseSessionDueToTimeout = false;
	m_InvalidTransIdReceived = false;
	m_pbtAuthorizationInProcess = DEFAULT_PBT_AUTHORIZATION_IN_PROCESS;
	m_cashInactive = false;
	m_guestPlayer = true;
	m_receivedBalancesFromEGM = false;
	m_isUploadableCashIgnored = false;
	m_playerIdWaitingPinReset = 0;
	m_newPin = 0;
	m_pendingAutoUploadTickCount = 0;
	m_autoUploadDelaySeconds = DEFAULT_AUTO_UPLOAD_DELAY_SECONDS;
	m_resetPinResult = DEFAULTRESETPINRESPONSE;

	BuildYourself();
	StoreYourself();
	SetEligibleCreditsToUpload(0, 0);

	m_lastSessionInProgressToUI = 0;
	LogPBT("PBT CPersonalBanker constructor");

	AddTopic(PB_PENDING_LOGS);
	AddTopic(PB_TRANSACTION_LOGS);
	AddTopic(PB_LOGS);
	AddTopic(PB_INFO_SUBSCRIPTION);
	AddTopic(PB_TRANSFER_SUBSCRIPTION);
	AddTopic(PB_RESET_PIN_RESPONSE);
}

/// <summary>
/// Destructor
/// </summary>
CPersonalBanker::~CPersonalBanker(void)
{
	CleanUp();
}

string CPersonalBanker::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId)
{
    UNREFERENCED_PARAMETER(&whereId); 

	string dataResponse;

	if (PB_TRANSACTION_LOGS == topic || PB_LOGS == topic)
	{
		dataResponse = ResponseForPbLogs(topic);
	}
	else if (PB_PENDING_LOGS == topic)
	{
		dataResponse = ResponseForPbPendingLogs();
	}
	else if (PB_INFO_SUBSCRIPTION == topic)
	{
		dataResponse = ResponseForPBInfo();  // it acquires critical section lock
	}
	else if (PB_TRANSFER_SUBSCRIPTION == topic)
	{
		dataResponse = ResponseForPBTransfer();  // it acquires critical section lock
	}
	else if (PB_RESET_PIN_RESPONSE == topic)
	{
		dataResponse = ResponseForResetPin();
	}
	return dataResponse;
}

void CPersonalBanker::SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize)
{
	size_t offset(0);
	bool ok(true);

	CUtilities::MemCopyZero_FromOffset(&ok, &m_state, sizeof(m_state), buffer, bufferSize, &offset);

	byte checkByte2;
	CUtilities::MemCopyZero_FromOffset(&ok, &checkByte2, sizeof(checkByte2), buffer, bufferSize, &offset);
	if (NVRAM_PB_CHECKBYTE2_VALUE == checkByte2)
	{
		CUtilities::MemCopyZero_FromOffset(&ok, &m_pbtAuthorizationInProcess, sizeof(m_pbtAuthorizationInProcess), buffer, bufferSize, &offset);
		LogPBT(_T("PBT SetFromNvramBuffer checkByte2"));
	}
	else
	{
		// Set defaults for remaining members.
		m_pbtAuthorizationInProcess = DEFAULT_PBT_AUTHORIZATION_IN_PROCESS;
		LogPBT(_T("PBT SetFromNvramBuffer default2"));
	}
}

/// <summary>
/// Deletes allocated objects used by this class.
/// </summary>
void CPersonalBanker::CleanUp()
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	{
		DeleteInfo();

		DeleteTransfer();

		DeletePreviousTransferId();

		if ( m_type == MEMORY_NVRAM )
		{
			SetActiveLocation(false);
		}
	}
}

UINT CPersonalBanker::GetNvramBuffer(BYTE *buffer, UINT bufferSize, bool& active) const
{
	size_t offset(0);
	bool ok(true);

	active = true;
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_state, sizeof(m_state));

	const byte checkByte2(NVRAM_PB_CHECKBYTE2_VALUE);
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &checkByte2, sizeof(checkByte2));
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_pbtAuthorizationInProcess, sizeof(m_pbtAuthorizationInProcess));
	LogPBT("PBT GetNvramBuffer");

	return (UINT)offset;
}

void CPersonalBanker::LogPBT(LPCTSTR descriptionFormat, ...) const
{
	string descriptionFormatEx(descriptionFormat);
	descriptionFormatEx += FormatString(_T(" State=%s AuthInProcess=%d"), CPersonalBankerTransfer::GetStateString(m_state), (int)m_pbtAuthorizationInProcess);

	va_list args;
	va_start(args, descriptionFormat);
	CPersonalBankerTransfer::LogPBTV(m_pbTransfer, descriptionFormatEx.c_str(), args);
	va_end(args);
}

void CPersonalBanker::DoneWithTransfer()
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	{
		DeleteTransfer();
	}
}

CPersonalBankerTransfer* CPersonalBanker::GetTransferCopy() const
{
	CPersonalBankerTransfer* transfer(nullptr);

	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if (m_pbTransfer != nullptr)
		{
			transfer = new CPersonalBankerTransfer(*m_pbTransfer);
		}
	}

	return transfer;
}

void CPersonalBanker::CreateTransfer(DWORD tickCount, MEMORY_TYPE memoryType, bool bBuildFromNVRAM)
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	{
		DeleteTransfer();
		m_pbTransfer = new CPersonalBankerTransfer(tickCount, memoryType, bBuildFromNVRAM);
		if (bBuildFromNVRAM && m_state == PersonalBankerState::PBStateWaitingForUploadToStart)
		{
			m_autoUpload.SetStartUploadOver(true);
		}
	}
}

/// <summary>
/// Deletes the Transfer object.
/// </summary>
void CPersonalBanker::DeleteTransfer()
{
	if(m_pbTransfer != nullptr)
	{
		delete m_pbTransfer;
		m_pbTransfer = nullptr;
		SetPbtAuthorizationInProcess(false);
		LogString("PBT", "PBT transfer deleted");
	}

	// resetting the session specific flags.
	m_ReleaseSessionDueToTimeout = false;
	m_InvalidTransIdReceived = false;
}

bool CPersonalBanker::HasTransfer()
{
	bool returnValue(false);

	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if (m_pbTransfer != nullptr)
		{
			returnValue = true;
		}
	}

	return returnValue;
}

bool CPersonalBanker::HasTransferForDifferentCardNumber(const CCard &card)
{
	bool returnValue = false;

	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if ( m_pbTransfer != nullptr )
		{
			int cardIDLen;
			const byte *cardID = card.GetCardID( cardIDLen );
			if ( m_pbTransfer->CompareCardNumber( cardID, cardIDLen ) != 0 )
			{
				returnValue = true;
			}
		}
	}

	return returnValue;
}

CPersonalBankerInfo* CPersonalBanker::GetInfoCopy() const
{
	CPersonalBankerInfo* info(nullptr);
	
	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if (m_pbInfo)
		{
			info = new CPersonalBankerInfo(*m_pbInfo);
		}
	}

	return info;
}

/// <summary>
/// Creates the Info object.
/// </summary>
/// <param name="memoryType">Type of memory used for storing and retrieving the object.</param>
/// <param name="bBuildFromNVRAM">If false, Personal Banker Info object is to be built from NVRAM, otherwise store it in NVRAM.</param>
void CPersonalBanker::CreateInfo(MEMORY_TYPE memoryType, bool bBuildFromNVRAM)
{
	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		DeleteInfo();
		m_pbInfo = new CPersonalBankerInfo(memoryType, bBuildFromNVRAM);
	}
}

/// <summary>
/// Deletes the Info object.
/// </summary>
void CPersonalBanker::DeleteInfo()
{
	if(m_pbInfo != nullptr)
	{
		delete m_pbInfo;
		m_pbInfo = nullptr;
	}
}

std::string CPersonalBanker::GetInfoPersonalBankerID() const
{
	std::string returnValue;

	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if(m_pbInfo != nullptr)
		{
			// The conversion from char* to cstring leaves characters at the end.
			returnValue = m_pbInfo->GetPersonalBankerID();
		}
	}

	return returnValue;
}

#ifdef _DEBUG
void CPersonalBanker::DeleteTransferAndInfo()
{
	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		DeleteTransfer();
		DeleteInfo();
	}
}
#endif // _DEBUG.

CLogs* CPersonalBanker::GetLogCopy()
{
	return new CLogs(m_pbLog);
}

CLogs* CPersonalBanker::GetSuccessLogCopy()
{
	return new CLogs(m_pbSuccessLog);
}

CPersonalBankerLog* CPersonalBanker::GetLastLogCopy()
{
	CPersonalBankerLog* log(nullptr);
	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if((CPersonalBankerLog*)m_pbLog.GetLastLog() != nullptr)
		{
			log = new CPersonalBankerLog(*(CPersonalBankerLog*)m_pbLog.GetLastLog());
		}
	}

	return log;
}

/// <summary>
/// Creates the Previous Transfer ID.
/// </summary>
void CPersonalBanker::CreatePreviousTransferId()
{
	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if(m_pbTransfer != nullptr && m_pbTransfer->GetPBTIDLength() > 0 && m_pbTransfer->GetID() != nullptr)
		{
			DeletePreviousTransferId();
			m_previousTransferIdLength = m_pbTransfer->GetPBTIDLength();
			m_previousTransferId = new char [m_previousTransferIdLength];
			memcpy(m_previousTransferId, m_pbTransfer->GetID(), m_previousTransferIdLength);
		}
	}
}

/// <summary>
/// Deletes the Previous Transfer ID.
/// </summary>
void CPersonalBanker::DeletePreviousTransferId()
{
	if(m_previousTransferId != nullptr)
	{
		delete[] m_previousTransferId;
		m_previousTransferIdLength = 0;
		m_previousTransferId = nullptr;
	}
}

void CPersonalBanker::SetWaitingForTransferToBeCompleted(bool waitingForTransferToBeCompleted)
{
	m_waitingForTransferToBeCompleted = waitingForTransferToBeCompleted;
}

bool CPersonalBanker::IsWaitingForTransferToBeCompleted() const
{
	return m_waitingForTransferToBeCompleted;
}

void CPersonalBanker::SetWaitingForDenomCheck(bool waitingForDenomCheck)
{
	m_waitingForDenomCheck = waitingForDenomCheck;
}

bool CPersonalBanker::IsWaitingForDenomCheck() const
{
	return m_waitingForDenomCheck;
}

void CPersonalBanker::SetState(PersonalBankerState state)
{
	m_state = state;
	StoreYourself();
}

PersonalBankerState CPersonalBanker::GetState() const
{
	return m_state;
}

PersonalBankerState CPersonalBanker::GetPbStateForUi() const
{
	return m_pbStateForUi;
}

void CPersonalBanker::SetPbtAuthorizationInProcess(bool pbtAuthorizationInProcess)
{
	m_pbtAuthorizationInProcess = pbtAuthorizationInProcess;
	StoreYourself();
	LogPBT("PBT SetPbtAuthorizationInProcess");
}

/// <summary>
/// Sets the personal banker action.
/// </summary>
/// <param name="action">Personal Bankers Action(PBActionGetEGMBalances, PBActionStartSession, PBActionSendInfo, etc...)</param>
/// <remarks>
/// If the previously set action was != PBActionNone then it should be set to PBActionNone when the action has been processed.
/// Any previously set action (except PBActionNone), should have already been processed before setting another action.
/// It is probably an error to set a new action != PBActionNone unless the current action is PBActionNone.
/// Organize the code so that each action is processed before another action is needed.
/// </remarks>
void CPersonalBanker::SetPersonalBankerAction(PersonalBankerAction action)
{
	// Assert that any previous action != PBActionNone has been completed before setting another action != PBActionNone.
	// I.e., assert that the previous action == PBActionNone or that the new action == PBActionNone.

	if (m_readyAction != PBActionNone && action != PBActionNone)
	{
		LogString(ERROR_LOG, "m_readyAction=%d action=%d in %s",
			(int)m_readyAction,
			(int)action,
			static_cast<LPCTSTR>(__TFUNCTION__));
	}

	m_readyAction = action;
}

void CPersonalBanker::ClearPersonalBankerAction()
{
	if(PBActionSendCompleteToUI == m_readyAction)
	{
		m_pbStateForUi = PBStateUnknown;
	}
	SetPersonalBankerAction(PBActionNone);
}

PersonalBankerAction CPersonalBanker::GetAction() const
{
	return m_readyAction;
}

bool CPersonalBanker::IsPbtInProgress() const
{
	bool pbtInProgress(false);
	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		pbtInProgress = (m_pbTransfer != nullptr || m_autoUpload.IsAutoUploadInProgress());
	}

	return pbtInProgress;
}

void CPersonalBanker::RemoveSession()
{
	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
		SetEligibleCreditsToUpload(0, 0);
		m_autoUpload.SetPendingAutoWatAmount(0);
		m_autoUpload.SetPendingAutoCepAmount(0);
		m_autoUpload.SetPendingAutoNcepAmount(0);
		DeleteTransfer();
		DeleteInfo();
		if(m_autoUpload.IsStartUploadOver())
		{
			LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
			m_autoUpload.SetUploadDelay(CUtilities::GetTickCount(), PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND);
			m_autoUpload.SetAutoUploadPending(true);
			m_autoUpload.SetAutoInProcess(false);
			m_autoUpload.SetStartUploadOver(false);
		}
	}
}

void CPersonalBanker::SetTransferCardNumber(const CCard* card, time_t time)
{
	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if(card != nullptr && m_pbTransfer != nullptr)
		{
			int cardLen(0);
			const byte* cardId = card->GetCardID(cardLen);
			m_pbTransfer->SetCardNumber(cardId, cardLen);
			m_pbTransfer->SetUTCCardInTime(time);
		}
	}
}

void CPersonalBanker::DeleteCard()
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	m_autoUpload.DeleteCard();
}

void CPersonalBanker::RebuildCard()
{
	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		CCard* card = m_autoUpload.CreateCard();
		int cardlen = 0;
		const byte *cardID = nullptr;

		if (m_pbTransfer)
		{
			cardID = m_pbTransfer->GetCardNumber(cardlen);
		}

		card->SetCardInfo(CARD_PLAYER, cardID, cardlen);
	}
}

void CPersonalBanker::RebuildCard(const CCard* card)
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	
	m_autoUpload.CreateCard(card);
}

bool CPersonalBanker::BeginPersonalBankerAutoSession(bool sendToPoller, const CConfig& config, DWORD autoUploadPin,
DWORD tickCount)
{
	bool bNeedToSendToPoller(false);

	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if( m_pbTransfer != nullptr )
		{
			LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
			m_autoUpload.SetAutoInProcess(false);
			m_autoUpload.SetAutoUploadPending(true);
			m_autoUpload.SetUploadDelay(tickCount, PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND);
		}
		else
		{
			LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
			CreateTransfer(tickCount, MEMORY_NVRAM);

			// set up the transfer object
			m_pbTransfer->SetPersonalBankerType(SessionRequest);
			m_pbTransfer->SetCancelledType(_Unknown);
			SetState(sendToPoller ? PBStateInquireSent : PBStateWaitingForUploadToStart);
			m_pbTransfer->SetPIN( autoUploadPin ); // No pin for auto transfers, set to -1 so PMT knows not to verify it.
			m_pbTransfer->SetRestrictedPointRedemption(config.PBTPointsAsPromo());
			m_pbTransfer->SetCEPOutEnabled(config.CEPOutEnabled());
			m_pbTransfer->SetDenomination(config.GetDenomination());

			// Set the card number in the transfer object
			SetTransferCardNumber(m_autoUpload.GetCard(), CUtilities::GetUTCTime());

			if(sendToPoller)
			{
				LogString(PBT_LOG, "%s %d sendToPoller", __FUNCTION__, __LINE__);
				bNeedToSendToPoller = true;
			}
		}

	}

	return bNeedToSendToPoller;
}

void CPersonalBanker::PreparePersonalBankerTransfer(WORD& pbSequence)
{
	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);

		if(m_pbTransfer != nullptr)
		{
			// Set the sequence number for the transaction.
			if(pbSequence == 0)
				pbSequence++;

			m_pbTransfer->SetSequenceNumber( pbSequence++ );

			if ( m_pbTransfer->GetRestrictedAmount() > 0 )
			{
				m_pbTransfer->SetRestrictedSequenceNumber( pbSequence++ );
			}

			if ( m_pbTransfer->GetNonrestrictedAmount() > 0 )
			{
				m_pbTransfer->SetNonrestrictedSequenceNumber( pbSequence++ );
			}

			// Keep copy of this ID if session is already established
			if(m_state >= PBStateTransferingFromToGame)
			{
				CreatePreviousTransferId();
			}
		}
	}
}

bool CPersonalBanker::IsPersonalBankerTransferInProgress() const
{
	bool bIsTransfer = false;

	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if ( m_pbTransfer != nullptr && m_state == PBStateTransferingFromToGame )
		{
			bIsTransfer = true;
		}
	}

	return bIsTransfer;
}

bool CPersonalBanker::IsPersonalBankerTransferToGame() const
{
	bool bIsTransfer = false;

	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if ( m_pbTransfer != nullptr && m_pbTransfer->IsTransferTypeToGame() )
		{
			bIsTransfer = true;
		}
	}

	return bIsTransfer;
}

bool CPersonalBanker::IsPersonalBankerTransferFromGame() const
{
	bool bIsTransfer = false;

	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if(m_pbTransfer != nullptr)
		{
			bIsTransfer = m_pbTransfer->GetPersonalBankerTransferType() == CashFromGame || 
							m_pbTransfer->GetPersonalBankerTransferType() == PointsFromGame || 
							m_pbTransfer->GetPersonalBankerTransferType() == PromoDollarsFromGame ||
							m_pbTransfer->GetPersonalBankerTransferType() == AllFromGame;
		}
	}

	return bIsTransfer;
}

bool CPersonalBanker::BeginPersonalBankerSession(CPlayer* player, const string& pbPin, CConfig& config, bool cashoutToHostSession, 
	CJackpot* jackpot)
{
	bool bSendToPoller = false;

	lock_guard<recursive_mutex> lock(m_CriticalSection);
	if (player != nullptr)
	{
		if (m_pbTransfer != nullptr)
		{
			if (m_pbInfo && m_state == PBStateInfoReceived)
			{
				if (m_pbInfo->HasError())
				{
					// We are at received info state with error in PBInfo that should mean we had a 
					// wrong pin and in this case it is okay to delete the existing transfer object.
					if (m_pbtAuthorizationInProcess)
					{
						// Not okay to delete the transfer, let the authorization continue.
						LogString(PBT_LOG, "[%s] Canceling new session request since older session's authorization is in progress.", __FUNCTION__);
					}
					else
					{
						LogString(PBT_LOG, "[%s] Accepting new session request and removing an older session in error state (wrong PIN).", __FUNCTION__);
						RemoveSession();
					}
				}
				else if (!m_pbTransfer->IsCashoutToHostTransfer() && cashoutToHostSession)
				{
					// We got cashout to host PBT session request while we are in the regular PBT session with no 
					// transfer initiated yet. Cashout to host should take precedence and hence we need to end
					// the current PBT session. We still do not want to initiate the cashout to host session yet 
					// until current PBT session ends gracefully.
					// Note: If we get here we already knew player's cash account inactive state and it is 
					// guaranteed here that will be doing transfer of some credits via cashout to host
					LogString(PBT_LOG, "[%s] Canceling new cashout to host session request but initiating releasing of an earlier valid session so that next time request can be picked.", __FUNCTION__);
					SetPersonalBankerAction(PBActionReleaseSession);
				}
				else
				{
					LogString(PBT_LOG, "[%s] Canceling new session request since we are already in a valid session.", __FUNCTION__);
				}
			}
			else
			{
				LogString(PBT_LOG, "[%s] Canceling new session request since current session in state: %s cannot be terminated.", __FUNCTION__, CPersonalBankerTransfer::GetStateString(m_state));
			}
		}

		// If we do not have a transfer object, create a new one and send it to the SCF.
		// If we do have a transfer object, then we are in a state where we cannot send a new inquire.
		if ( m_pbTransfer == nullptr )
		{
			LogString(PBT_LOG, "[%s] Starting new session and creating new transfer object...", __FUNCTION__);

			bSendToPoller = true;

			DWORD pin = cashoutToHostSession ? PBT_AUTO_UPLOAD_PIN : strtoul(pbPin.c_str(), nullptr, 10);

			LogString (POLLER_LOG, "%s %d cashoutToHostSession: %u", __FUNCTION__, __LINE__, cashoutToHostSession);
			
			// Create transfer object, cashout to host transfer session should not be NVRAM backed-up because  
			// such transfers needs to complete within a small timeframe window and otherwise EGM goes into 
			// handpay mode and transfer is no longer valid to be done (like if nCompass reboots in between).
			CreateTransfer(CUtilities::GetTickCount(), cashoutToHostSession ? MEMORY_NONE : MEMORY_NVRAM);

			// set up the transfer object
			m_pbTransfer->SetCashoutToHostTransfer(cashoutToHostSession);
			m_pbTransfer->SetPersonalBankerType( SessionRequest );
			m_pbTransfer->SetCancelledType( _Unknown );
			SetState(PBStateInquireSent);
			m_pbTransfer->SetPIN(pin);
			m_pbTransfer->SetRestrictedPointRedemption(config.PBTPointsAsPromo());
			m_pbTransfer->SetCEPOutEnabled(config.CEPOutEnabled());
			m_pbTransfer->SetDenomination(config.GetDenomination());
			if (jackpot)
			{
				m_pbTransfer->SetTaxInformation(true, jackpot->IsFederalTaxOverriden(), jackpot->IsStateTaxOverriden(), 
					jackpot->GetJackpotID(), jackpot->GetFederalTaxWithholding());
			}
			else
			{
				m_pbTransfer->SetTaxInformation(false, false, false, 0, 0);
			}
			// Set the card number in the transfer object
			SetTransferCardNumber(player->GetCard(), player->GetCard()->GetCardInTime());
		}
	}

	return bSendToPoller;
}

void CPersonalBanker::PerformTimedEvents(bool delayAutoNcepOutWhenNotDefaultedPlayer,
	bool haveEmployee1,
	const CConfig& config,
	DWORD tickCount,
	DWORD sessionTimeoutValueInMilliseconds,
	DWORD authorizationTimeoutValueInMilliseconds)
{
	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		PerformTimedEventsTransfer(config,
			tickCount,
			sessionTimeoutValueInMilliseconds,
			authorizationTimeoutValueInMilliseconds);

		// If no pending action so far, check for other timed events.
		if (PBActionNone == m_readyAction)
		{
			PerformTimedEventsAuto(delayAutoNcepOutWhenNotDefaultedPlayer,
				haveEmployee1,
				config,
				tickCount);
		}
	}
}

// Private function. Critical section is already locked.
void CPersonalBanker::PerformTimedEventsTransfer(const CConfig& config,
	DWORD tickCount,
	DWORD sessionTimeoutValueInMilliseconds,
	DWORD authorizationTimeoutValueInMilliseconds)
{
	if ( m_pbTransfer != nullptr )
	{
		const DWORD timeoutValueInMilliseconds(
			m_pbtAuthorizationInProcess ? authorizationTimeoutValueInMilliseconds : sessionTimeoutValueInMilliseconds);

		// See if the personal banker session has expired.
		if((m_pbTransfer->GetPersonalBankerType() == CreditTransfer || m_state == PBStateInquireSent || m_state == PBStateInfoReceived) && 
			m_pbTransfer->HasSessionExpired(tickCount, timeoutValueInMilliseconds))
		{
			LogString(PBT_LOG, "[%s] PBT Session expired, initiating session cleanup...", __FUNCTION__);

			if (m_autoUpload.IsAutoInProcess())
			{
				// cancel
				m_pbTransfer->SetCancelledType(AutomaticUploadCanceledBySystem);
				LogString(PBT_LOG, "PerformTimedEventsTransfer AutomaticUploadCanceledBySystem type=%s amount=%u NonrestrictedAmount=%u RestrictedAmount=%u ",
					m_pbTransfer->GetPBTypeText(m_pbTransfer->GetPersonalBankerType()).c_str(), m_autoUpload.GetPendingAutoWatAmount(), m_autoUpload.GetPendingAutoCepAmount(), m_autoUpload.GetPendingAutoNcepAmount());
				m_pbTransfer->SetAmount(m_autoUpload.GetPendingAutoWatAmount());
				m_pbTransfer->SetNonrestrictedAmount(m_autoUpload.GetPendingAutoCepAmount());
				m_pbTransfer->SetRestrictedAmount(m_autoUpload.GetPendingAutoNcepAmount());
				m_autoUpload.SetAutoInProcess(false);
				// Send transaction to poller
				SetPersonalBankerAction(PBActionSendTransferTransaction);

				//We need to restart the transfer no matter what
				m_autoUpload.SetUploadDelay(tickCount, PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND);
				m_autoUpload.SetAutoUploadPending(true);
			}
			else
			{
				m_ReleaseSessionDueToTimeout = true;
				if (m_pbTransfer->GetPersonalBankerType() == CreditTransfer)
				{
					m_pbTransfer->SetCancelledType(CreditTransferErrorTimeout);
					SetState(PBStateUnknown);

					// Clear any action that got us here.
					if (m_readyAction == PBActionReleaseSession)
					{
						ClearPersonalBankerAction();
					}
					m_pbStateForUi = m_state;
					SetPersonalBankerAction(PBActionSendCompleteToUI);

					CPersonalBankerLog *log = new CPersonalBankerLog( m_pbTransfer, m_pbTransfer->GetPersonalBankerTransferType(), m_pbTransfer->GetAmount(), CUtilities::GetCurrentTime() );
					m_pbLog.Add( log );
				}
				else
				{
					LogPBT("PerformTimedEventsTransfer() PBActionReleaseSession");
					SetPersonalBankerAction(PBActionReleaseSession);
				}
			}
		}
	}
	else
	{
		// waiting for bpt to end before displaying a upload cancelled message?
		if (IsWaitingForTransferToBeCompleted())
		{
			SetWaitingForTransferToBeCompleted(false);
			CreateTransfer(tickCount, MEMORY_NONE);
			m_pbTransfer->SetCancelledType(AutomaticUploadCanceledByUser);
			LogString(PBT_LOG, "PerformTimedEventsTransfer2 type=%s amount=%u NonrestrictedAmount=%u RestrictedAmount=%u ",
				m_pbTransfer->GetPBTypeText(m_pbTransfer->GetPersonalBankerType()).c_str(), m_autoUpload.GetPendingAutoWatAmount(), m_autoUpload.GetPendingAutoCepAmount(), m_autoUpload.GetPendingAutoNcepAmount());
			m_pbTransfer->SetAmount(m_autoUpload.GetPendingAutoWatAmount());
			m_pbTransfer->SetNonrestrictedAmount(m_autoUpload.GetPendingAutoCepAmount());
			m_pbTransfer->SetRestrictedAmount(m_autoUpload.GetPendingAutoNcepAmount());
			m_autoUpload.SetAutoInProcess(false);
			// set up the transfer object
			SetState(PBStateTransferComplete);
			m_pbTransfer->SetPIN( PBT_AUTO_UPLOAD_PIN ); // No pin for auto transfers, set to -1 so PMT knows not to verify it.
			m_pbTransfer->SetDenomination( config.GetDenomination() );
 
			// Set the card number in the transfer object
			SetTransferCardNumber(m_autoUpload.GetCard(), CUtilities::GetUTCTime());

			// Send transaction to poller
			SetPersonalBankerAction(PBActionSendPersonalBankerNcepOutFail);
		}
	}
}

// Private function. Critical section is already locked.
void CPersonalBanker::PerformTimedEventsAutoNotDefaulted(bool haveEmployee1,
	const CConfig& config, DWORD tickCount)
{
	LogPBT("PerformTimedEventsAutoNotDefaulted: Current Time %d IsAutoInProcess=%d IsAutoUploadPending=%d IsAutoStartedOnce=%d IsStartUploadOver=%d", 
		(int)CUtilities::GetCurrentTime(),
		m_autoUpload.IsAutoInProcess(),
		m_autoUpload.IsAutoUploadPending(),
		m_autoUpload.IsAutoStartedOnce(),
		m_autoUpload.IsStartUploadOver());

	// Start Auto Upload Credits process, only do this once
	if(m_pbTransfer == nullptr)
	{
		//we need to create a pbt transfer object but not send it to the poller
		//This is incase we reboot before we get game comm again.
		// 2013-06-26 BeginPersonalBankerAutoSession(sendToPoller=false, ...) always returns false.
		BeginPersonalBankerAutoSession(false, config, PBT_AUTO_UPLOAD_PIN, tickCount);
	}

	//we must be waiting for a download to end lets wait and try in 3 seconds.
	//added employee to this if for tracker 27446
	int uploadDelaySeconds = PBT_UPLOAD_DELAY;

	if ( !m_pbTransfer->IsAutoUploadPIN() || haveEmployee1) 
	{
		LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
		m_autoUpload.SetAutoInProcess(false);
		m_autoUpload.SetAutoUploadPending(true);
	}
	else
	{
		bool delayAutoUpload(DelayAutoUpload());
		LogString(PBT_LOG, _T("%s %d DelayAutoUpload=%u"), __TFUNCTION__, __LINE__, delayAutoUpload);
		m_autoUpload.SetAutoInProcess(true);
		m_autoUpload.SetAutoUploadPending(false);

		// Tell GCF to get balances
		SetPersonalBankerAction(delayAutoUpload ? PBActionGetEGMBalancesWithMessage : PBActionGetEGMBalances);
				
		uploadDelaySeconds = PBT_UPLOAD_DELAY_PLAYER_ACTIVE;
	}

	m_autoUpload.SetUploadDelay(tickCount, uploadDelaySeconds * MILLISECONDS_PER_SECOND);
}

// Private function. Critical section is already locked.
void CPersonalBanker::PerformTimedEventsAuto(bool delayAutoNcepOutWhenNotDefaultedPlayer,
	bool haveEmployee1,
	const CConfig& config,
	DWORD tickCount)
{
	int uploadDelaySeconds = 0;

	if (DelayAutoUpload() &&
		m_autoUploadPendingFlags.m_flags.m_cancelPendingAutoUpload &&
		m_autoUploadPendingFlags.m_flags.m_cancellable)
	{
		LogString(PBT_LOG, "%s %d cancelling auto upload", __FUNCTION__, __LINE__);
		m_autoUploadPendingFlags.m_flags.m_cancelPendingAutoUpload = false;
		m_autoUploadPendingFlags.m_flags.m_delayAutoUpload = false;
		m_autoUploadPendingFlags.m_flags.m_cancellable = false;		
		m_autoUpload.SetAutoUploadPending(false);
		m_autoUpload.SetStartUploadOver(false);
		m_autoUpload.SetAutoInProcess(false);
		m_autoUpload.SetAutoStartedOnce(false);
		RemoveSession();

		// cause PBT transfering message or animation to clear.
		SetPersonalBankerAction(PBActionSendAutoTransferCancelledToUI);
	}

	if (m_autoUpload.IsAutoUploadPending() &&
		m_autoUpload.IsUploadTimerExpired(tickCount))
	{
		if (!delayAutoNcepOutWhenNotDefaultedPlayer)
		{
			LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
			PerformTimedEventsAutoNotDefaulted(haveEmployee1, config, tickCount);
		}
		else
		{
			LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
			// for this case only we want to delay the start of the upload this allows GCF to come up
			// This might happen at startup
			uploadDelaySeconds = PBT_UPLOAD_DELAY;
			SetPersonalBankerAction(PBActionSendCardSessionInProgressToUI);
		}
	}
	else if (m_pbInfo == nullptr &&
			 m_autoUpload.IsAutoInProcess() &&
		 	 m_autoUpload.IsUploadTimerExpired(tickCount))
	{
		LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
		SetPersonalBankerAction(PBActionSendCardSessionInProgressToUI);
		uploadDelaySeconds = PBT_UPLOAD_DELAY;
	}

	if (uploadDelaySeconds != 0)
	{
		m_autoUpload.SetUploadDelay(tickCount, uploadDelaySeconds * MILLISECONDS_PER_SECOND);
	}
}

void CPersonalBanker::ProcessBalancesFromEGM(DWORD wat,
	DWORD cep,
	DWORD ncep,
	bool canTransferOff,
	bool callToEGMFailed,
	const CConfig& config,
	bool eftMode)
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);

	DWORD uploadableWAT((config.IsPoller_12_11_6000_or_Higher() && config.WatAutoUpload() && config.PBFromGameEnabled()) ? wat : 0),
		  uploadableCEP((config.IsPoller_12_11_6000_or_Higher() && config.CepAutoUpload() && config.PBFromGameEnabled() && config.CEPOutEnabled()) ? cep : 0),
		  uploadableNCEP(config.NcepAutoUpload() && config.NCEPOutEnabled() ? ncep : 0);

	// While reboot m_guestPlayer is true, which eventually here is restricting the auto upload of WAT and CEP.
	m_isUploadableCashIgnored = false;
	if (m_guestPlayer || IsCashInactive())
	{
		if (uploadableWAT > 0)
		{
			uploadableWAT = 0;
			m_isUploadableCashIgnored = true;
		}
		if (uploadableCEP > 0)
		{
			uploadableCEP = 0;
			m_isUploadableCashIgnored = true;
		}
	}

	LogStringNoFormat(PBT_LOG, FormatString("%s IsAutoInProcess=%d m_guestPlayer=%d, IsPoller_12_11_6000_or_Higher=%d WatAuto=%d CepAuto=%d NcepAuto=%d callToEGMFailed=%d canTransferOff=%d eftMode=%d isCashInactive=%d uploadableWAT=%u uploadableCEP=%d uploadableNCEP=%u wat=%u cep=%u ncep=%u",
		__FUNCTION__, m_autoUpload.IsAutoInProcess(), m_guestPlayer, config.IsPoller_12_11_6000_or_Higher(), config.WatAutoUpload(), config.CepAutoUpload(), config.NcepAutoUpload(),
		callToEGMFailed, canTransferOff, eftMode, IsCashInactive(), uploadableWAT, uploadableCEP, uploadableNCEP, wat, cep, ncep).c_str());
	
	m_autoUploadPendingFlags.m_flags.m_cancellable = uploadableNCEP == 0;
	if (!m_autoUploadPendingFlags.m_flags.m_cancellable)
	{
		//Cancel pending auto upload becase we have promo credits
		LogString(PBT_LOG, "%s %d StopDelayAutoUpload because we have promo credits.", __FUNCTION__, __LINE__);
		StopDelayAutoUpload();
	}
	bool cashOnEgm(uploadableWAT > 0 || uploadableCEP > 0);

	if (canTransferOff && !eftMode && cashOnEgm && DelayAutoUpload())
	{
		LogString(PBT_LOG, "%s %d delaying auto upload", __FUNCTION__, __LINE__);
		m_autoUpload.SetPendingAutoWatAmount(uploadableWAT);
		m_autoUpload.SetPendingAutoCepAmount(uploadableCEP);
		m_autoUpload.SetPendingAutoNcepAmount(uploadableNCEP);
		m_autoUpload.SetUploadDelay(CUtilities::GetTickCount(), PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND);
		m_autoUpload.SetAutoUploadPending(true);
		m_autoUpload.SetAutoInProcess(false);
		m_autoUpload.SetAutoStartedOnce(true);
	}
	else if (m_autoUpload.IsAutoInProcess() && canTransferOff && !eftMode &&
		(uploadableWAT > 0 || uploadableCEP > 0 || uploadableNCEP > 0))
	{
		LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
		m_autoUpload.SetPendingAutoWatAmount(uploadableWAT);
		m_autoUpload.SetPendingAutoCepAmount(uploadableCEP);
		m_autoUpload.SetPendingAutoNcepAmount(uploadableNCEP);
		m_autoUpload.SetAutoStartedOnce(true);
		DeleteTransfer();
		if (BeginPersonalBankerAutoSession(true, config, PBT_AUTO_UPLOAD_PIN, CUtilities::GetTickCount()))
		{
			LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
			// Start the transfer
			SetPersonalBankerAction(PBActionStartTransferOnly);
		}
	}
	else if ((m_autoUpload.IsAutoInProcess() ||
		m_autoUpload.IsAutoStartedOnce() ||
		m_autoUpload.IsAutoUploadPending()) &&
		!eftMode &&
		!canTransferOff &&
		(uploadableWAT > 0 || uploadableCEP > 0 || uploadableNCEP > 0 || callToEGMFailed))
	{
		LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);

		// Pending auto transfer and ( can't communicate with game or there was promo on game )
		SetPersonalBankerAction(PBActionSendCardSessionInProgressToUI);

		// game probably can not be locked.  In-play or door open likely
		// report floorlogix and ui		
		m_autoUpload.SetPendingAutoWatAmount(uploadableWAT);
		m_autoUpload.SetPendingAutoCepAmount(uploadableCEP);
		m_autoUpload.SetPendingAutoNcepAmount(uploadableNCEP);
		m_autoUpload.SetUploadDelay(CUtilities::GetTickCount(), PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND);
		m_autoUpload.SetAutoUploadPending(true);
		m_autoUpload.SetAutoInProcess(false);
		m_autoUpload.SetAutoStartedOnce(true);
	}
	// Tracker #22978 - m_pbtAutoInProcess state variable needs to be reset when promo on game is 0 so
	// that new pbt session doesn't get handled as a promo up transaction
	else if (m_autoUpload.IsAutoInProcess() &&
		m_autoUpload.IsAutoStartedOnce() &&
		!eftMode &&
		!callToEGMFailed)
	{
		LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
		m_autoUpload.SetAutoInProcess(false);
		m_autoUpload.SetAutoStartedOnce(false);
		RemoveSession();

		// cause PBT transfering message or animation to clear.
		SetPersonalBankerAction(PBActionSendAutoTransferCancelledToUI);
	}
	else if (m_autoUpload.IsAutoInProcess())
	{
		LogString(PBT_LOG, "%s %d eftMode=%d callToEGMFailed=%d canTransferOff=%d",
			__FUNCTION__, __LINE__, eftMode, callToEGMFailed, canTransferOff);
		m_autoUpload.SetStartUploadOver(false);
		m_autoUpload.SetAutoInProcess(false);
		RemoveSession();

		PersonalBankerAction action = PBActionProcessRequestPlayerResume;
		if (config.IsPoller_12_11_6000_or_Higher() &&
			config.PBFromGameEnabled() &&
			((config.WatAutoUpload() && wat > 0) || (config.CepAutoUpload() && config.CEPOutEnabled() && cep > 0)))
		{
			if (IsCashInactive())
			{
				action = PBActionProcessRequestPlayerResumeDueToCashInactive;
			}
			else if (m_guestPlayer)
			{
				action = PBActionProcessRequestPlayerResumeDueToGuestPlayer;
			}
		}
		SetPersonalBankerAction(action);
	}
	else
	{
		LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
		if (m_pbInfo != nullptr && !m_autoUpload.IsAutoUploadPending())
		{
			LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
			//Load the balances
			if (config.CEPOutEnabled())
			{
				wat += cep;
			}
			m_pbInfo->SetCashBalanceOnGame(wat);
			m_pbInfo->SetPromoBalanceOnGame(ncep);

			//Send the message to the UI
			SetPersonalBankerAction(PBActionSendPersonalBankerInfoToUI);
		}
	}

	m_receivedBalancesFromEGM = true;
}

void CPersonalBanker::ProcessPersonalBankerTransferComplete(const CPersonalBankerTransfer *pbTransfer,
	CGames& games,
	byte& meterNumberToAddTo,
	bool& isDuplicate,
	bool isEFT)
{
	meterNumberToAddTo = mNone;
	isDuplicate = false;

	bool wasTransferCompleteExpected(false);

	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if(pbTransfer != nullptr)
		{
			m_autoUpload.SetAutoInProcess(false);
			m_autoUpload.SetAutoStartedOnce(false);

			// If we do not have a transfer object, we need to create one.
			if(m_pbTransfer == nullptr)
			{
				CreateTransfer(CUtilities::GetTickCount(), MEMORY_NVRAM);
				m_pbTransfer->SetID( pbTransfer->GetID(), pbTransfer->GetPBTIDLength() );
			}

			// Determine if we expected this transfer complete.
			// NOTE: IF we created a new object above, this will be unexpected.
			wasTransferCompleteExpected = m_state == PBStateTransferingFromToGame;

			CheckIfTransferMatchWhatWeHaveInMemory(pbTransfer);

			//If this is a transfer from the game and it failed and the amount is 0, use the credit meter as the amount (tracker 16890)
			if(!m_pbTransfer->IsTransferTypeToGame() && m_pbTransfer->IsTransferRequestCanceled() && !m_pbTransfer->HasAmount())
			{
				LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
				m_pbTransfer->SetAmount( games.ConvertCredits( m_pbTransfer->GetOriginalCreditMeter() ) );
				m_pbTransfer->SetRestrictedAmount(m_autoUpload.GetPendingAutoNcepAmount());
			}
			 
			LogString(PBT_LOG, "ProcessPersonalBankerTransferComplete m_state=%s Amount=%lld Nonrestricted=%lld Restricted=%lld",
				CPersonalBankerTransfer::GetStateString(m_state), m_pbTransfer->GetAmount(), m_pbTransfer->GetNonrestrictedAmount(), m_pbTransfer->GetRestrictedAmount());
			PersonalBankerType originalPBType = GetOriginalPBType();

			GetMeterToUpdate(wasTransferCompleteExpected, 
				isEFT, 
				meterNumberToAddTo, 
				originalPBType);

			CheckIfPersonalBankerTypeHasToBeChanged(originalPBType, wasTransferCompleteExpected, isDuplicate);
		}
	}
}

void CPersonalBanker::CheckIfTransferMatchWhatWeHaveInMemory(const CPersonalBankerTransfer *pbTransfer)
{
	if(m_pbTransfer != nullptr && pbTransfer != nullptr)
	{
		// The received tranfer matches the one we had in memory
		if(*m_pbTransfer == *pbTransfer)
		{
			// get the original transfer amount
			__int64 originalAmount = m_pbTransfer->GetAmount();

			// Copy the transfer data over to our object.
			*m_pbTransfer = *pbTransfer;

			// If this was a transfer to the game and it failed, maintain the orginal amount.
			if ( m_pbTransfer->IsTransferTypeToGame() && m_pbTransfer->IsTransferRequestCanceled() )
			{
				LogString(PBT_LOG, "%s %d originalAmount=%lld", __FUNCTION__, __LINE__, originalAmount);
				m_pbTransfer->SetAmount(originalAmount);
			}

			else if ((PromoDollarsFromGame == m_pbTransfer->GetPromoPersonalBankerTransferType()  || 
				      CashFromGame == m_pbTransfer->GetPersonalBankerTransferType()) &&
					 m_pbTransfer->GetPIN() == PBT_AUTO_UPLOAD_PIN)
			{
				LogString(PBT_LOG, "%s %d originalAmount=%lld", __FUNCTION__, __LINE__, originalAmount);

				RebuildCard();

				//now that we have the card rebuilt lets start the upload process again
				m_autoUpload.SetUploadDelay(CUtilities::GetTickCount(), PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND);
				m_autoUpload.SetAutoUploadPending(true);
				m_autoUpload.SetAutoInProcess(false);
			}		
		}
		// If the transfer we received was not the same as the one we have in memory, get rid of the one we received.
		//  ... it has to be an old one.
		// The one we have is memory must not have made it to the game.
		else
		{
			LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
			//if at startup the Auto upload transfer failed after we sent it to the game we need to try again.
			if ((PromoDollarsFromGame == m_pbTransfer->GetPromoPersonalBankerTransferType() ||
				 CashFromGame == m_pbTransfer->GetPersonalBankerTransferType()) &&
			    m_pbTransfer->GetPIN() == PBT_AUTO_UPLOAD_PIN)
			{
				LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);

				RebuildCard();

				//now that we have the card rebuilt lets start the upload process again
				m_autoUpload.SetUploadDelay(CUtilities::GetTickCount(), PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND);
				m_autoUpload.SetAutoUploadPending(true);
				m_autoUpload.SetAutoInProcess(false);
			}		
			m_pbTransfer->ChangePersonalBankerTypeToCanceled();
			m_pbTransfer->SetCancelledType( MismatchTransID );
		}
	}
}

TiltCode CPersonalBanker::VerifyTransferedAmounts(CPersonalBankerTransfer *transfer)
{
	// Check transfer amounts against expected amounts (and hardcoded amounts) if transfer was successful
	lock_guard<recursive_mutex> lock(m_CriticalSection);

	TiltCode tilt = tltUnknown;

	if (transfer != nullptr && m_pbTransfer != nullptr)
	{
		if (!transfer->IsTransferRequestCanceled())
		{
			if (!transfer->VerifyMaxAmounts())
			{
				tilt = tltTransferOvrLmtAftr;
			}
			else if (transfer->IsTransferTypeToGame() && ( transfer->GetAmount() != m_pbTransfer->GetAmount() ||
				transfer->GetRestrictedAmount() != m_pbTransfer->GetRestrictedAmount() ||
				transfer->GetNonrestrictedAmount() != m_pbTransfer->GetNonrestrictedAmount() ) )
			{
				// Only doing this check for money transferred to game
				tilt = tltInvalidPbAmount;
			}
		}
	}

	return tilt;
}

PersonalBankerType CPersonalBanker::GetOriginalPBType()
{
	PersonalBankerType originalPBType = m_pbTransfer->GetPersonalBankerType();
	if(originalPBType == Unspecified)
	{
		// could be promo upload
		originalPBType = m_pbTransfer->GetPromoPersonalBankerType();
	}

	return originalPBType;
}

void CPersonalBanker::GetMeterToUpdate(bool wasTransferCompleteExpected, 
	bool isEFT,
	byte& meterNumberToAddTo,
	PersonalBankerType originalPBType)
{
	meterNumberToAddTo = mNone;

	// If we are adding points or promo credits to the game, we have to add this to meters
	if(wasTransferCompleteExpected && 
		(originalPBType == PromoCreditsToCash || (originalPBType == PointRedemptionToCash && isEFT) || originalPBType == PointRedemptionToPromoCredits))
	{
		if ( originalPBType == PromoCreditsToCash )
		{
			meterNumberToAddTo = mPBn;
		}
		else
		{
			meterNumberToAddTo = mPtR;
		}
	}
}

void CPersonalBanker::CheckIfPersonalBankerTypeHasToBeChanged(PersonalBankerType originalPBType,
	bool wasTransferCompleteExpected, bool& isDuplicate)
{
	// If the transfer complete was not expected, we have to change the type.
	if(!wasTransferCompleteExpected)
	{
		//TR 28746 use only the success log since we are checking this against the last transaction in the EGM
		CPersonalBankerLog *testLog = (CPersonalBankerLog *)m_pbSuccessLog.GetLastLog();
		if (testLog != nullptr)
		{
			// Check for duplicate
			int length = 0;
			char *transID = testLog->GetTransID(length);
			const char *transID2 = m_pbTransfer->GetID( );
			if(memcmp( transID, transID2, length) == 0)
			{
				isDuplicate = true;
			}
			else
			{
				switch( originalPBType )
				{
				case CashUpload:
				case CashUploadAuto:
				case PointCashUploadAuto:
					m_pbTransfer->SetPersonalBankerType(UnexpectedCashFromGame);
					break;
				case CanceledCashUploadAuto:
				case CanceledPointCashUploadAuto:
				case CanceledCashUpload:
					m_pbTransfer->SetPersonalBankerType(UnexpectedCashFromGameErr);
					break;
				case CashDownload:
					m_pbTransfer->SetPersonalBankerType(UnexpectedCashToGame);
					break;
				case CanceledCashDownload:
					m_pbTransfer->SetPersonalBankerType(UnexpectedCashToGameErr);
					break;
				case PromoCreditsToCash:
					m_pbTransfer->SetPersonalBankerType(UnexpectedRestrictedToGame);
					break;
				case CanceledPromoCreditsToCash:
					m_pbTransfer->SetPersonalBankerType(UnexpectedRestrictedToGameErr);
					break;
				case PointRedemptionToCash:
					m_pbTransfer->SetPersonalBankerType(UnexpectedUnRestrictedToGame);
					break;
				case CanceledPointRedemptionToPromoCredits:
					m_pbTransfer->SetPersonalBankerType(UnexpectedRestrictedToGameErr);
					break;
				case PointRedemptionToPromoCredits:
					m_pbTransfer->SetPersonalBankerType(UnexpectedRestrictedToGame);
					break;
				case CanceledPointRedemption:
					m_pbTransfer->SetPersonalBankerType(UnexpectedUnrestrictedToGameErr);
					break;
				case PromoCreditsUpload:
					m_pbTransfer->SetPersonalBankerType(UnexpectedRestrictedFromGame);
					break;
				case CanceledPromoCreditsUpload:
					m_pbTransfer->SetPersonalBankerType(UnexpectedRestrictedFromGame);
					break;
				case PromoCreditsUploadAuto:
					m_pbTransfer->SetPersonalBankerType(UnexpectedRestrictedFromGame);
					break;
				case CanceledPromoCreditsUploadAuto:
					m_pbTransfer->SetPersonalBankerType(UnexpectedRestrictedFromGame);
					break;
				case CancelExternalCreditsToGame: 
					m_pbTransfer->SetPersonalBankerType(CancelExternalCreditsToGame);
					break;
				default:
					break;
				}
			}
		}
	}
}

void CPersonalBanker::LogTransfer(CPlayer* player, const CConfig& config, const time_t currentTime)
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	if(m_pbTransfer != nullptr)
	{
		// Log the pb transfer - successful or failed transfer.
		// If the transfer was successful, only log if we had a non-zero amount.
		if ( m_pbTransfer->HasAmount() || m_pbTransfer->IsTransferRequestCanceled() )
		{
			LogString(PBT_LOG, "%s %d m_pbTransfer->GetPersonalBankerTransferType(): %u", __FUNCTION__, __LINE__, (WORD)m_pbTransfer->GetPersonalBankerTransferType());
			if (!IsCreditTransfer())
			{
				// Transfers TO the game should always have an amount.
				// Transfers FROM the game may not have an amount if there was a failure or there was only non-restricted amount.
				if ( m_pbTransfer->GetAmount() > 0 || m_pbTransfer->IsTransferRequestCanceled() )
				{
					CPersonalBankerLog *log = new CPersonalBankerLog( m_pbTransfer, m_pbTransfer->IsTransferTypeToGame() ? m_pbTransfer->GetPersonalBankerTransferType() : CashFromGame, 
						m_pbTransfer->GetAmount(), currentTime, m_pbTransfer->IsPartialWATOutTransfer() );
					m_pbLog.Add( log );
				}

				// Transfers FROM the game could have a nonrestricted and restricted amount.
				// NOTE: There could be 2 log entries for transfers FROM the game.
				if ( m_pbTransfer->GetNonrestrictedAmount() > 0 && !m_pbTransfer->IsTransferTypeToGame() )
				{
					CPersonalBankerLog *log = new CPersonalBankerLog( m_pbTransfer, PointsFromGame, m_pbTransfer->GetNonrestrictedAmount(), 
						currentTime, m_pbTransfer->IsPartialCEPOutTransfer() );
					m_pbLog.Add( log );
				}
				if ( m_pbTransfer->GetRestrictedAmount() > 0 && !m_pbTransfer->IsTransferTypeToGame() )
				{
					CPersonalBankerLog *log = new CPersonalBankerLog( m_pbTransfer, PromoDollarsFromGame, m_pbTransfer->GetRestrictedAmount(), currentTime );
					m_pbLog.Add( log );
				}
			}
		}

		// Log the successful pb transfer.
		if (  m_pbTransfer->HasAmount() && !m_pbTransfer->IsTransferRequestCanceled() )
		{
			if(!IsCreditTransfer())
			{
				// Transfers TO the game should always have an amount.
				// Transfers FROM the game may not have an amount if there was only non-restricted amount.
				if ( m_pbTransfer->GetAmount() > 0 )
				{
					CPersonalBankerLog *successLog = new CPersonalBankerLog( m_pbTransfer, m_pbTransfer->IsTransferTypeToGame() ? m_pbTransfer->GetPersonalBankerTransferType() : CashFromGame, 
						m_pbTransfer->GetAmount(), currentTime, m_pbTransfer->IsPartialWATOutTransfer() );
					m_pbSuccessLog.Add( successLog );

					if (player != nullptr && m_pbTransfer->GetPersonalBankerTransferType() == PointsToGame)
					{
						CPlayerInfo * info = player->GetPlayerInfo();

						__int64 amountTransfered = m_pbTransfer->GetAmount() + m_pbTransfer->GetRestrictedAmount() + m_pbTransfer->GetNonrestrictedAmount();
						__int64 pointsTransfered = (config.GetPointsPerDollar() * amountTransfered) / ONE_DOLLAR_IN_CENTS;
						if (0 != pointsTransfered)
						{
							if (info == nullptr)
							{
								LogString(ERROR_LOG, "info=nullptr type=PointsToGame amountTransfered=%lld pointsTransfered=%lld in %s",
									(long long)amountTransfered,
									(long long)pointsTransfered,
									static_cast<LPCTSTR>(__TFUNCTION__));
							}
							else
							{
								DWORD newPoints = 0;
								if (0 <= pointsTransfered && pointsTransfered < info->GetPoints())
								{
									newPoints = info->GetPoints() - (DWORD)pointsTransfered;
								}

                                info->SetPointsAndCountdown(newPoints, info->GetCountdown(), info->GetCountdownHighPrecision());
							}
						}
					}
				}
			}

			// Transfers FROM the game could have a nonrestricted amount.
			// NOTE: There could be 2 log entries for transfers FROM the game.
			if ( m_pbTransfer->GetNonrestrictedAmount() > 0 && !m_pbTransfer->IsTransferTypeToGame() )
			{
				CPersonalBankerLog *successLog = new CPersonalBankerLog( m_pbTransfer, PointsFromGame, 
					m_pbTransfer->GetNonrestrictedAmount(), currentTime,
					m_pbTransfer->IsPartialCEPOutTransfer());
				m_pbSuccessLog.Add( successLog );
			}

			if ( m_pbTransfer->GetRestrictedAmount() > 0 && !m_pbTransfer->IsTransferTypeToGame() )
			{

				CPersonalBankerLog *successLog = new CPersonalBankerLog( m_pbTransfer, PromoDollarsFromGame, m_pbTransfer->GetRestrictedAmount(), currentTime );
				m_pbSuccessLog.Add( successLog );
			}
		}
	}
}

void CPersonalBanker::ProcessPersonalBankerCreditTransfer(const CPersonalBankerInfo *pbInfo)
{
	if ( pbInfo != nullptr && m_pbTransfer != nullptr && m_pbTransfer->GetPersonalBankerType() != SessionRelease )
	{
		if (m_pbTransfer->GetPersonalBankerType() == CreditTransfer)
		{
			if (pbInfo->GetErrorCode() != NoError)
			{
				switch (pbInfo->GetErrorCode())
				{
				case CreditTransferCommserror:
					m_pbTransfer->SetCancelledType(CreditTransferErrorComms);
					break;
				case CreditTransferInsufficientCredits:
					m_pbTransfer->SetCancelledType(CreditTransferErrorInsufficientCredits);
					break;
				case CreditTransferOtherIssue:
					m_pbTransfer->SetCancelledType(CreditTransferErrorOther);
					break;
				default:
					m_pbTransfer->SetCancelledType(CreditTransferErrorUnknown);
					break;
				}
				SetState(PBStateTransferComplete);
			}
			else
			{
				CPersonalBankerLog *successLog = new CPersonalBankerLog( m_pbTransfer, m_pbTransfer->GetPersonalBankerTransferType(), m_pbTransfer->GetAmount(), CUtilities::GetCurrentTime() );
				m_pbSuccessLog.Add( successLog );
			}

			CPersonalBankerLog *log = new CPersonalBankerLog( m_pbTransfer, m_pbTransfer->GetPersonalBankerTransferType(), m_pbTransfer->GetAmount(), CUtilities::GetCurrentTime() );
			m_pbLog.Add( log );

			// Clear any action that got us here.
			if (m_readyAction == PBActionReleaseSession)
			{
				ClearPersonalBankerAction();
			}
			m_pbStateForUi = m_state;
			SetPersonalBankerAction(PBActionSendCompleteToUI);
		}
	}
}

void CPersonalBanker::ProcessPersonalBankerInfo(const CPersonalBankerInfo *pbInfo,
	const CConfig &config,
	bool gameOnline,
	bool bIsEFT)
{
	LogPBT("PBT ProcessPersonalBankerInfo");
	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		LogString(PBT_LOG, "[%s] : State=%s", __FUNCTION__, CPersonalBankerTransfer::GetStateString(m_state));
		if (pbInfo == nullptr)
		{
			LogString(ERROR_LOG, "pbInfo == nullptr in %s", static_cast<LPCTSTR>(__TFUNCTION__));
		}
		else
		{
			LogPBT("PBT ProcessPersonalBankerInfo pbInfo.ErrCode: %d", pbInfo->GetErrorCode());
#ifdef _DEBUG
			LogPBT("%s %d: pbInfo: PBT Cash Balance = %u, Cash upload limit = %u, Cash download limit = %u.",
				__FUNCTION__, __LINE__, pbInfo->GetCashBalance(), pbInfo->GetMaximumCashUp(), pbInfo->GetMaximumCashDown());
#endif
			// Note: If we are in the process of authorizing then we do not want the state 
			// to be set to InfoReceived until after we actually transfer to the game
			// 2016-06-03 If we are in process of authorizing then the state is probably already PBStateInfoReceived.
			// 2016-06-03 After the authorization process completes, m_pbtAuthorizationInProcess is still true.
			if(m_pbtAuthorizationInProcess)
			{
				LogPBT("PersonalBanker info received while authorization in process");
			}
			else if ( m_pbTransfer != nullptr && m_pbTransfer->GetPersonalBankerType() != SessionRelease )
			{
				LogString(PBT_LOG, "%s m_pbTransfer != nullptr && m_pbTransfer->GetPersonalBankerType() != SessionRelease", __FUNCTION__);
				// TV -- Added ID check
				// compare against previous ID.  If prev id doesn't exist, or new ID is different, then use it.  If it is the same, using this data may overwrite
				// new NCEP Out Auto session.
				if (m_previousTransferIdLength == 0 ||
					m_previousTransferId == nullptr || 
					m_previousTransferIdLength != pbInfo->GetPBIDLength() || 
					(m_previousTransferIdLength == pbInfo->GetPBIDLength() && 
					0 != memcmp(m_previousTransferId, pbInfo->GetPersonalBankerID(), m_previousTransferIdLength)))
				{

					int cardNumberLen;
					if ( pbInfo->CompareCardNumber( m_pbTransfer->GetCardNumber( cardNumberLen ) ) == 0 )
					{
						// Make an info object
						if ( m_pbInfo == nullptr )
						{
							CreateInfo(MEMORY_NVRAM);
						}

						// Copy the info object to the persistant object
						*m_pbInfo = *pbInfo;
						m_cashInactive = m_pbInfo->IsCashInactive();

						// Add the config data to the info 
						//added isEFT for TR #19313
						//((CSentinelApp *)AfxGetApp())->LogString(L"PBT", L"**** Not pbtAutoInProcess ****" );
						m_pbInfo->SetConfigData( config, gameOnline, bIsEFT );

						if (m_autoUpload.IsAutoInProcess())
						{
							if (gameOnline)
							{
								LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
								ProcessPersonalBankerInfoAuto(pbInfo, config);
							}
							else//Delete m_pbInfo to allow to repeat the auto upload process
							{
								LogString(PBT_LOG, "%s %d DeleteInfo", __FUNCTION__, __LINE__);
								DeleteInfo();
							}
						}
						else
						{
							LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
							ProcessPersonalBankerInfoNotAuto(pbInfo);
						}
					}
					else
					{
					}
				}
			}
		}
	}
}

// Private function. Critical section is already locked.
void CPersonalBanker::ProcessPersonalBankerInfoNotAuto(const CPersonalBankerInfo *pbInfo)
{
	SetState(PBStateInfoReceived);
	LogString(PBT_LOG, "[%s] : State=PBStateInfoReceived, setting max/min cashup/promoup", __FUNCTION__);
#ifdef _DEBUG
	LogString(PBT_LOG, "%s %d: pbInfo: PBT Cash Balance = %u, Cash upload limit = %u, Cash download limit = %u.",
		__FUNCTION__, __LINE__, pbInfo->GetCashBalance(), pbInfo->GetMaximumCashUp(), pbInfo->GetMaximumCashDown());
#endif

	m_pbTransfer->SetID( pbInfo->GetPersonalBankerID(), pbInfo->GetPBIDLength() );

	m_pbTransfer->SetMaxCashUp( pbInfo->GetMaximumCashUp() );
	m_pbTransfer->SetMinCashUp( pbInfo->GetMinimumCashUp() );
	m_pbTransfer->SetMinPromoUp( pbInfo->GetMinimumPromoUp());
	m_pbTransfer->SetMaxPromoUp( pbInfo->GetMaximumPromoUp());

	if (m_pbTransfer->IsProcessTaxDeduction())
	{
		LogString(JACKPOT, "PBActionSendTaxDeductionRequest");
		SetPersonalBankerAction(PBActionSendTaxDeductionRequest);
	}
	else if (m_pbTransfer->IsCashoutToHostTransfer())
	{
		bool proceedWithCashUpload = false;
		bool proceedWithPromoUpload = false;

		if (m_eligibleCashToUpload)
		{
			WORD cashoutErrorFlags = 0;

			if (m_cashInactive)
			{
				LogString(PBT_LOG, "[%s] Cashout error : Cash inactive", __FUNCTION__);
				cashoutErrorFlags = CashoutError::CASHOUT_ERROR_CASH_INACTIVE;
			}
			else if (pbInfo->IsUnableToMoveLeftoverCash())
			{
				LogString(PBT_LOG, "[%s] Cashout error : Cash functionality unavailable", __FUNCTION__);
				cashoutErrorFlags = CashoutError::CASHOUT_ERROR_CASH_FUNCTIONALITY_UNAVAILABLE;
			}
			else if (m_eligibleCashToUpload < m_pbTransfer->GetMinCashUp() * ONE_DOLLAR_IN_CENTS)
			{
				LogString(PBT_LOG, "[%s] Cashout error : Cash under transfer limit", __FUNCTION__);
				cashoutErrorFlags = CashoutError::CASHOUT_ERROR_CASH_UNDER_TRANSFER_LIMIT;
			}
			else if (m_eligibleCashToUpload > m_pbTransfer->GetMaxCashUp() * ONE_DOLLAR_IN_CENTS)
			{
				LogString(PBT_LOG, "[%s] Cashout error : Cash over transfer limit", __FUNCTION__);
				cashoutErrorFlags = CashoutError::CASHOUT_ERROR_CASH_OVER_TRANSFER_LIMIT;
			}
			else
			{
				LogString(PBT_LOG, "[%s] proceedWithCashUpload = true", __FUNCTION__);
				proceedWithCashUpload = true;
			}

			if (cashoutErrorFlags)
			{
				LogString(PBT_LOG, "[%s] Setting cashout error [0x%x] in personal banker object. Cash upload won't occur.", __FUNCTION__, cashoutErrorFlags);
				m_pbTransfer->AppendCashoutErrorFlags(cashoutErrorFlags);
			}
		}

		if (m_eligiblePromosToUpload)
		{
			WORD cashoutErrorFlags = 0;

			if (m_eligiblePromosToUpload < m_pbTransfer->GetMinPromoUp() * ONE_DOLLAR_IN_CENTS)
			{
				LogString(PBT_LOG, "[%s] Cashout error : Promo under transfer limit", __FUNCTION__);
				cashoutErrorFlags = CashoutError::CASHOUT_ERROR_PROMO_UNDER_TRANSFER_LIMIT;
			}
			else if (m_eligiblePromosToUpload > m_pbTransfer->GetMaxPromoUp() * ONE_DOLLAR_IN_CENTS)
			{
				LogString(PBT_LOG, "[%s] Cashout error : Promo over transfer limit", __FUNCTION__);
				cashoutErrorFlags = CashoutError::CASHOUT_ERROR_PROMO_OVER_TRANSFER_LIMIT;
			}
			else
			{
				LogString(POLLER_LOG, "[%s] proceedWithPromoUpload = true", __FUNCTION__);
				proceedWithPromoUpload = true;
			}

			if (cashoutErrorFlags)
			{
				LogString(PBT_LOG, "[%s] Setting cashout error [0x%x] in personal banker object. Promo upload won't occur.", __FUNCTION__, cashoutErrorFlags);
				m_pbTransfer->AppendCashoutErrorFlags(cashoutErrorFlags);
			}
		}

		LogString(PBT_LOG, "[%s] : CashoutToHost Scenario: EligibleCashToUpload: %d EligiblePromosToUpload: %d "
			"CashInactive: %d MinCashUp: %d MaxCashUp: %d MinPromoUp: %d MaxPromoUp: %d ProceedWithCashUpload: %d "
			"ProceedWithPromoUpload: %d", __FUNCTION__, m_eligibleCashToUpload, m_eligiblePromosToUpload,
			m_cashInactive, m_pbTransfer->GetMinCashUp(), m_pbTransfer->GetMaxCashUp(), m_pbTransfer->GetMinPromoUp(),
			m_pbTransfer->GetMaxPromoUp(), proceedWithCashUpload, proceedWithPromoUpload);

		PersonalBankerTransferType pbTransferType;
		if (proceedWithCashUpload && proceedWithPromoUpload)
		{
			pbTransferType = AllFromGame;
		}
		else if (proceedWithCashUpload)
		{
			pbTransferType = CashFromGame;
		}
		else if (proceedWithPromoUpload)
		{
			pbTransferType = PromoDollarsFromGame;
		}

		if (proceedWithCashUpload || proceedWithPromoUpload)
		{
			// We are all set to do the transfer
			m_pbTransfer->SetPersonalBankerTransferType(pbTransferType);
			SetState(PBStateTransferingFromToGame);

			// Send message to UI to display 'transfer in progress'
			// Also send message to GCF to initiate transfer to EGM.
			SetPersonalBankerAction(PBActionSendDisplayPBAnimationToUIAndTransferToEGM);
		}
		else
		{
			// Send message to GCF to abort cashout to host
			SetPersonalBankerAction(PBActionSendCancelCashoutToHostToEGM);
		}
	}
	else
	{
		// Send message to GCF to get balances on game for NCEPOut stuff
		// We will inform the UI only after we recieve a response.
#ifdef _DEBUG
		LogString(POLLER_LOG, "%s: SetPersonalBankerAction(PBActionGetEGMBalances)", __FUNCTION__);
#endif
		m_receivedBalancesFromEGM = false;
		SetPersonalBankerAction(PBActionGetEGMBalances);
	}
}

// Private function. Critical section is already locked.
void CPersonalBanker::ProcessPersonalBankerInfoAuto(const CPersonalBankerInfo *pbInfo, const CConfig &config)
{
	if (!pbInfo->HasError())
	{
		LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);

		// Must filter out multiple PBInfo responses so it doesn't cause multiple get PBT balance messages going into CSASProtocol
		if(m_state == PBStateInquireSent && m_pbInfo->GetPBIDLength() > 0)
		{
			LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
			SetState(PBStateTransferingFromToGame);
						
			m_pbTransfer->SetID( pbInfo->GetPersonalBankerID(), pbInfo->GetPBIDLength() );

			m_pbTransfer->SetMaxCashUp( pbInfo->GetMaximumCashUp() );
			m_pbTransfer->SetMinCashUp( pbInfo->GetMinimumCashUp() );
			m_pbTransfer->SetMinPromoUp( pbInfo->GetMinimumPromoUp());
			m_pbTransfer->SetMaxPromoUp( pbInfo->GetMaximumPromoUp());

			bool sendToGCF(false);
			WORD autoUploadErrorFlags_Cash = 0;

			if (config.IsPoller_12_11_6000_or_Higher())
			{
				bool promoIsNotWithinLimits(false);
				const DWORD pendingAutoNcepAmount = m_autoUpload.GetPendingAutoNcepAmount();

				if (pendingAutoNcepAmount > 0)
				{
					WORD autoUploadErrorFlags_Promo = 0;
					std::string strError;

					if (pendingAutoNcepAmount < m_pbTransfer->GetMinPromoUp() * ONE_DOLLAR_IN_CENTS)
					{
						strError = "Promo under transfer limit";
						autoUploadErrorFlags_Promo = AutoUploadError::AUTOUPLOAD_ERROR_PROMO_UNDER_TRANSFER_LIMIT;
						promoIsNotWithinLimits = true;
					}
					else if (pendingAutoNcepAmount > m_pbTransfer->GetMaxPromoUp() * ONE_DOLLAR_IN_CENTS)
					{
						strError = "Promo over transfer limit";
						autoUploadErrorFlags_Promo = AutoUploadError::AUTOUPLOAD_ERROR_PROMO_OVER_TRANSFER_LIMIT;
						promoIsNotWithinLimits = true;
					}

					if (config.NcepAutoUpload() && m_pbInfo->IsTransferPromoFromGameEnabled() && autoUploadErrorFlags_Promo)
					{
						LogString(PBT_LOG, "[%s] AutoUpload error : %s", __FUNCTION__, strError.c_str());
						LogString(PBT_LOG, "[%s] Setting autoupload error [0x%x] in personal banker object. Promo upload won't occur.", __FUNCTION__, autoUploadErrorFlags_Promo);
						m_pbTransfer->AppendAutoUploadErrorFlags(autoUploadErrorFlags_Promo);
					}
				}

				LogString(PBT_LOG, "ProcessPersonalBankerInfoAuto: CashFromGameEnabled:%d PromoFromGameEnabled:%d "
					"m_guestPlayer=%d CashInactive:%d EligibleCashToUpload:%u EligiblePromosToUpload:%d "
					"MinCashUp:%d MaxCashUp:%d MinPromoUp:%d MaxPromoUp:%d "
					"PromoIsNotWithinLimits:%d", 
					m_pbInfo->IsTransferCashFromGameEnabled(), m_pbInfo->IsTransferPromoFromGameEnabled(),
					m_guestPlayer, m_cashInactive, m_autoUpload.GetPendingCashToAutoUpload(), pendingAutoNcepAmount,
					m_pbTransfer->GetMinCashUp(), m_pbTransfer->GetMaxCashUp(), m_pbTransfer->GetMinPromoUp(), m_pbTransfer->GetMaxPromoUp(), 
					promoIsNotWithinLimits);

				const DWORD pendingAutoCashAmount = ((config.WatAutoUpload() && m_autoUpload.GetPendingAutoWatAmount() > 0) ? m_autoUpload.GetPendingAutoWatAmount() : 0) +
							((config.CepAutoUpload() && m_autoUpload.GetPendingAutoCepAmount() > 0) ? m_autoUpload.GetPendingAutoCepAmount() : 0);

				if (m_pbInfo->IsTransferCashFromGameEnabled() &&
					pendingAutoCashAmount > 0 &&
					pendingAutoCashAmount >= m_pbTransfer->GetMinCashUp() * ONE_DOLLAR_IN_CENTS &&
					pendingAutoCashAmount <= m_pbTransfer->GetMaxCashUp() * ONE_DOLLAR_IN_CENTS &&
					!m_guestPlayer && 
					!m_cashInactive &&
					!m_pbInfo->IsUnableToMoveLeftoverCash())
				{
					LogString(PBT_LOG, "ProcessPersonalBankerInfoAuto CashUploadAuto");
					sendToGCF = true;
					m_pbTransfer->SetPersonalBankerTransferType(CashFromGame);
					m_pbTransfer->SetPersonalBankerType(CashUploadAuto);
					m_pbTransfer->SetCepAutoUpload(m_autoUpload.GetPendingAutoCepAmount() > 0);
					m_pbTransfer->SetWatAutoUpload(m_autoUpload.GetPendingAutoWatAmount() > 0);
				}
				else
				{
					LogString(PBT_LOG, "ProcessPersonalBankerInfoAuto CashUploadAuto cancelled");
					m_pbTransfer->SetPersonalBankerTransferType(Undeclared);
					m_pbTransfer->SetPersonalBankerType(Unspecified);

					if (m_pbInfo->IsTransferCashFromGameEnabled() &&
						(pendingAutoCashAmount > 0 || m_isUploadableCashIgnored))
					{
						if (m_guestPlayer)
						{
							LogString(PBT_LOG, "[%s] AutoUpload error : Guest player", __FUNCTION__);
							autoUploadErrorFlags_Cash = AutoUploadError::AUTOUPLOAD_ERROR_GUEST_PLAYER;
						}
						else if (m_cashInactive)
						{
							LogString(PBT_LOG, "[%s] AutoUpload error : Cash inactive", __FUNCTION__);
							autoUploadErrorFlags_Cash = AutoUploadError::AUTOUPLOAD_ERROR_CASH_INACTIVE;
						}
						else if (m_pbInfo->IsUnableToMoveLeftoverCash())
						{
							LogString(PBT_LOG, "[%s] AutoUpload error : Cash functionality unavailable", __FUNCTION__);
							autoUploadErrorFlags_Cash = AutoUploadError::AUTOUPLOAD_ERROR_CASH_FUNCTIONALITY_UNAVAILABLE;
						}
						else if (pendingAutoCashAmount < m_pbTransfer->GetMinCashUp() * ONE_DOLLAR_IN_CENTS)
						{
							LogString(PBT_LOG, "[%s] AutoUpload error : Cash under transfer limit", __FUNCTION__);
							autoUploadErrorFlags_Cash = AutoUploadError::AUTOUPLOAD_ERROR_CASH_UNDER_TRANSFER_LIMIT;
						}
						else if (pendingAutoCashAmount > m_pbTransfer->GetMaxCashUp() * ONE_DOLLAR_IN_CENTS)
						{
							LogString(PBT_LOG, "[%s] AutoUpload error : Cash over transfer limit", __FUNCTION__);
							autoUploadErrorFlags_Cash = AutoUploadError::AUTOUPLOAD_ERROR_CASH_OVER_TRANSFER_LIMIT;
						}

						if (autoUploadErrorFlags_Cash)
						{
							LogString(PBT_LOG, "[%s] Setting autoupload error [0x%x] in personal banker object. Cash upload won't occur.", __FUNCTION__, autoUploadErrorFlags_Cash);
							m_pbTransfer->AppendAutoUploadErrorFlags(autoUploadErrorFlags_Cash);
						}
					}
				}

				if (config.NcepAutoUpload() && m_pbInfo->IsTransferPromoFromGameEnabled() && pendingAutoNcepAmount > 0)
				{
					m_pbTransfer->SetNcepAutoUpload(m_autoUpload.GetPendingAutoNcepAmount() > 0);
					LogString(PBT_LOG, "ProcessPersonalBankerInfoAuto PromoCreditsUploadAuto");
					sendToGCF = true;
					m_pbTransfer->SetPromoPersonalBankerTransferType(PromoDollarsFromGame);
					m_pbTransfer->SetPromoPersonalBankerType(PromoCreditsUploadAuto);
					if (promoIsNotWithinLimits)
					{
						//We need to skip cash in this case because, 
						//proper 90tx will not be reported properly.
						LogString(PBT_LOG, "ProcessPersonalBankerInfoAuto skipping cash because promo is not within transfer limits.");
						m_pbTransfer->SetPersonalBankerTransferType(Undeclared);
						m_pbTransfer->SetPersonalBankerType(Unspecified);
					}
				}
			}
			else
			{
				LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);

				sendToGCF = true;

				m_pbTransfer->SetPersonalBankerTransferType(Undeclared);
				m_pbTransfer->SetPersonalBankerType(Unspecified);

				m_pbTransfer->SetPromoPersonalBankerTransferType(PromoDollarsFromGame);
				m_pbTransfer->SetPromoPersonalBankerType(PromoCreditsUploadAuto);
			}

			if (sendToGCF)
			{
				LogString(PBT_LOG, "ProcessPersonalBankerInfoAuto PBActionSendDisplayPBAnimationToUIAndTransferToEGM");
				// Send message to GCF to get balances on game for auto upload stuff
				// We will inform the UI only after we recieve a response.
				SetPersonalBankerAction(PBActionSendDisplayPBAnimationToUIAndTransferToEGM);
			}
			else
			{				
				LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
				m_autoUpload.SetAutoUploadPending(false);
				m_autoUpload.SetAutoInProcess(false);
				m_autoUpload.SetAutoStartedOnce(false);
				m_autoUpload.SetStartUploadOver(false);
				if (autoUploadErrorFlags_Cash)
				{
					LogString(PBT_LOG, "ProcessPersonalBankerInfoAuto PBActionCancelAutoUploadDueToCashErrorSessionRelease");
					SetPersonalBankerAction(PBActionCancelAutoUploadDueToCashErrorSessionRelease);
				}
				else
				{
					LogString(PBT_LOG, "ProcessPersonalBankerInfoAuto PBActionProcessRequestPlayerResume");
					SetPersonalBankerAction(PBActionProcessRequestPlayerResume);
				}
			}
		}
		else
		{
			LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);

			//we don't want to send this if we have an employee card in
			SetPersonalBankerAction(PBActionSendDisplayPBAnimationToUI);
		}
 	}
	else
	{
		LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);

		// Had session request error, notify UI
		m_autoUpload.SetAutoInProcess(false);
		m_autoUpload.SetAutoUploadPending(true);

		// Tell the UI of failed PB.
		// TV -- Now, only start UI in process animation if Session request was successful. No longer need to turn it off.
		// Kill existing Personal Banker Transfer
		SetPersonalBankerAction(LimitSessionInProgressToUIExpiredAction());
		DeleteTransfer(); 
	}
}

void CPersonalBanker::EndActivePlayerSession(const CConfig &config)
{
	// If we have a PB transfer object but the transfer has not started, release the transfer.
	// lock this section to prevent potential conflicting events
	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if ( m_pbTransfer == nullptr && CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM() )
		{
			CreateTransfer( CUtilities::GetTickCount(), MEMORY_NVRAM, true );

			if ( CPersonalBankerInfo::DoesPersonalBankerInfoExistInNVRAM() )
			{
				CreateInfo( MEMORY_NVRAM, true );
				m_pbInfo->SetConfigData( config, true, false ); //added false for TR #19313

				m_pbTransfer->SetMinCashUp(m_pbInfo->GetMinimumCashUp());
				m_pbTransfer->SetMaxCashUp(m_pbInfo->GetMaximumCashUp());
				m_pbTransfer->SetMinPromoUp(m_pbInfo->GetMinimumPromoUp());
				m_pbTransfer->SetMaxPromoUp(m_pbInfo->GetMaximumPromoUp());
			}
		}

		if ( m_pbTransfer != nullptr )
		{
			if ( m_state == PBStateInquireSent || m_state == PBStateInfoReceived )
			{
				LogPBT("EndActivePlayerSession() PBActionReleaseSession");
				SetPersonalBankerAction(PBActionReleaseSession);
			}
		}
	}
}

void CPersonalBanker::ProcessInitialPersonalBanker(const CConfig& config)
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	if (CPersonalBankerTransfer::DoesPersonalBankerTransferExistInNVRAM())
	{
		CreateTransfer(CUtilities::GetTickCount(), MEMORY_NVRAM, true);
		if(CPersonalBankerInfo::DoesPersonalBankerInfoExistInNVRAM())
		{
			CreateInfo(MEMORY_NVRAM, true);
			m_pbInfo->SetConfigData(config, true, false); 
			m_pbTransfer->SetMinCashUp(m_pbInfo->GetMinimumCashUp());
			m_pbTransfer->SetMaxCashUp(m_pbInfo->GetMaximumCashUp());
			m_pbTransfer->SetMinPromoUp(m_pbInfo->GetMinimumPromoUp());
			m_pbTransfer->SetMaxPromoUp(m_pbInfo->GetMaximumPromoUp());
		}
	}
}

bool CPersonalBanker::CheckIfAuthorizationIsNeeded(PersonalBankerTransferType type, DWORD amountInPennies, bool atomicRedemptions)
{
	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		bool pbtAuthorizationInProcess(false);
		// We go through an authorization process before we transfer to the game. Added case for adding external credit from External Credit provider RCCL to game
		if((m_pbTransfer != nullptr && m_state == PBStateInfoReceived && atomicRedemptions) && (type == PointsToGame || type == CashToGame || type == PromoDollarsToGame || (type == ExternalCredit && m_pbInfo && m_pbInfo->ExternalCreditTransferType() != TransferToAccount)))
		{
			LogString(PBT_LOG, "%s : External credit to game enabled : %d  , External Credit Transfer Type : %d authorization in process", __FUNCTION__, m_pbInfo->ExternalCreditProviderEnabled(), m_pbInfo->ExternalCreditTransferType());
			pbtAuthorizationInProcess = true;
			m_pbTransfer->SetPersonalBankerTransferType( type );
			LogString(PBT_LOG, "CheckIfAuthorizationIsNeeded-SetAmount1=%u", amountInPennies);
			m_pbTransfer->SetAmount(amountInPennies);
			m_pbTransfer->ChangePersonalBankerTypeToAuthorization();
		}

		SetPbtAuthorizationInProcess(pbtAuthorizationInProcess);
	}

	LogPBT("PBT CheckIfAuthorizationIsNeeded result");
	return m_pbtAuthorizationInProcess;
}

bool CPersonalBanker::BeginPersonalBankerTransfer(CGames& games, PersonalBankerTransferType type, DWORD amountInPennies, byte& pbEFTNumber)
{
	bool success(false);

	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if(m_pbTransfer != nullptr && m_state == PBStateInfoReceived)
		{
			SetState(PBStateTransferingFromToGame);	// This will keep the timer from going off.

			// Set the type and amount that came from the UI.
			m_pbTransfer->SetPersonalBankerTransferType( type );
			LogString(PBT_LOG, "BeginPersonalBankerTransfer-SetAmount1=%u", amountInPennies);
			m_pbTransfer->SetAmount(amountInPennies );

			// Set the current credit meter. This will be reset by the GCF right before the transfer.
			CGame *game = games.GetGame(EGM_GAME_NUMBER);
			if(game != nullptr)
			{
				m_pbTransfer->SetOriginalCreditMeter( game->GetMeter( mCrd ) );
			}

			// Send and increment the EFT number.
			if ( pbEFTNumber == EFT_TRANS_ID_DEFAULT )
			{
				pbEFTNumber = EFT_TRANS_ID_MINIMUM;	// can't be 0
			}
			m_pbTransfer->SetEFTNumber( pbEFTNumber++ );

			//Handle the external credit to game from RCCL card 
			if ( type == ExternalCredit )
			{
				if (m_pbInfo->ExternalCreditTransferType() == TransferToAccount)
				{
					LogString(PBT_LOG, "%s : External credit to game enabled : %d  , External Credit Transfer Type : %d PBT started", __FUNCTION__, m_pbInfo->ExternalCreditProviderEnabled(), m_pbInfo->ExternalCreditTransferType());
					m_pbTransfer->SetPersonalBankerType(CreditTransfer);
					m_pbTransfer->SetCancelledType(_Unknown);
				}
				else
				{
					LogString(PBT_LOG, "%s :RCCL Card  External credit to game enabled : %d  , External Credit Transfer Type : %d PBT started", __FUNCTION__, m_pbInfo->ExternalCreditProviderEnabled(), m_pbInfo->ExternalCreditTransferType());
                    switch (m_pbInfo->ExternalCreditTransferType()) 
					{
                    case DownloadCashToGame :
                            m_pbTransfer->SetPersonalBankerType(CashDownload);
                            success = true;
                            break;
                        
                    case DownloadPromoToGame:
                            m_pbTransfer->SetPersonalBankerType(PromoCreditsToCash);
                            success = true;
                            break;

                    default:
                            break;
                    }
				}
			}
			else
			{
				success = true;
			}
		}
	}

	return success;
}

void CPersonalBanker::ReleasePersonalBankerSession(const CConfig& config, 
	bool canceledFromUI)
{	
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	bool sendToUI = true;
	if(m_pbTransfer != nullptr) 
	{
		ReleasePersonalBankerSessionAutoUpload(canceledFromUI, sendToUI);
		ReleasePersonalBankerSessionAction(config, canceledFromUI, sendToUI);
	}

	// Any action that got us here is not consistent with the new/current state.
	if (m_readyAction == PBActionReleaseSession)
	{
		ClearPersonalBankerAction();
	}
}

// Private function. Critical section is already locked. m_pbTransfer != nullptr
void CPersonalBanker::ReleasePersonalBankerSessionAutoUpload(bool canceledFromUI, bool &sendToUI)
{
	LogPBT("PBT ReleasePersonalBankerSessionAutoUpload canceledFromUI=%d sendToUI=%d IsAutoInProcess=%d IsAutoUploadPending=%d IsAutoStartedOnce=%d IsStartUploadOver=%d",
		canceledFromUI,
		sendToUI,
		m_autoUpload.IsAutoInProcess(),
		m_autoUpload.IsAutoUploadPending(),
		m_autoUpload.IsAutoStartedOnce(),
		m_autoUpload.IsStartUploadOver());

	//If in a auto upload, we don't want to send the message to the UI if it is from game and the transfer failed
	//because we are going to try again and again.
	if((m_autoUpload.IsAutoUploadPending() || m_autoUpload.IsAutoInProcess()) &&
		!m_pbTransfer->IsTransferTypeToGame() && 
		(m_pbTransfer->IsTransferRequestCanceled() ||
		 AutomaticUploadCanceledBySystem == m_pbTransfer->GetCancelledType() ||
		 AutomaticUploadCanceledByUser == m_pbTransfer->GetCancelledType()))
	{
		LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
		sendToUI = false;
		m_autoUpload.SetStartUploadOver(true);
	}
	else if (!m_pbTransfer->IsCashoutToHostTransfer() &&
		(m_pbTransfer->GetPIN() == PBT_AUTO_UPLOAD_PIN) && 
		(m_state == PBStateInquireSent || m_state == PBStateWaitingForUploadToStart) && 
		!canceledFromUI)
		//This will happen at startup if the card was removed right before the reboot
	{
		LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
		LogString(ERROR_LOG, "Auto upload found at startup restarting the transfer.");
		sendToUI = false;

		//since this occurs at startup and we don't keep the orig card around we need to rebuild it.
		RebuildCard();

		//now that we have the card rebuilt lets start the upload process again
		m_autoUpload.SetAutoUploadPending(true);
		m_autoUpload.SetAutoInProcess(false);
	}
}

// Private function. Critical section is already locked. m_pbTransfer != nullptr
void CPersonalBanker::ReleasePersonalBankerSessionAction(const CConfig& config,
	bool canceledFromUI,
	bool sendToUI)
{
	LogPBT("PBT ReleasePersonalBankerSessionAction canceledFromUI=%d sendToUI=%d IsAutoInProcess=%d IsAutoUploadPending=%d IsAutoStartedOnce=%d IsStartUploadOver=%d", 
		    canceledFromUI, 
		    sendToUI,
			m_autoUpload.IsAutoInProcess(),
			m_autoUpload.IsAutoUploadPending(),
			m_autoUpload.IsAutoStartedOnce(),
			m_autoUpload.IsStartUploadOver()
	);

	// If we are in the authorization phase then we need to rollback the transfer.
	if (m_pbtAuthorizationInProcess && m_state == PBStateInfoReceived)
	{
		LogPBT("Release session while in authorization.  Rolling back transfer.");
		// To cancel a PersonalBanker authorization, prior to receiving a granted or denied response,
		// ... we need to start a transfer and then run the process to roll it back.

		// Clear any action that got us here.
		if (m_readyAction == PBActionReleaseSession)
		{
			ClearPersonalBankerAction();
		}
		SetPersonalBankerAction(PBActionProcessAuthorizationCanceled);
		SetState(PBStateTransferingFromToGame);
		m_pbTransfer->SetPersonalBankerTransferType(m_pbTransfer->GetPersonalBankerTransferType());
	}
	else if (canceledFromUI && m_state == PBStateWaitingForUploadToStart)
	{
		// Clear any action that got us here.
		if (m_readyAction == PBActionReleaseSession)
		{
			ClearPersonalBankerAction();
		}
		SetPersonalBankerAction(PBActionSendAutoUploadCancelledMessage);

		m_autoUpload.SetAutoUploadPending(false);
		m_autoUpload.SetAutoInProcess(false);
		m_autoUpload.SetStartUploadOver(false);
		m_autoUpload.SetAutoStartedOnce(false);

		LogString(PBT_LOG, "ReleasePersonalBankerSessionAction type=%s amount=%u NonrestrictedAmount=%u RestrictedAmount=%u ",
			m_pbTransfer->GetPBTypeText(m_pbTransfer->GetPersonalBankerType()).c_str(), m_autoUpload.GetPendingAutoWatAmount(), m_autoUpload.GetPendingAutoCepAmount(), m_autoUpload.GetPendingAutoNcepAmount());
		m_pbTransfer->SetCancelledType(AutomaticUploadCanceledByUser);
		m_pbTransfer->SetAmount(m_autoUpload.GetPendingAutoWatAmount());
		m_pbTransfer->SetNonrestrictedAmount(m_autoUpload.GetPendingAutoCepAmount());
		m_pbTransfer->SetRestrictedAmount(m_autoUpload.GetPendingAutoNcepAmount());
		m_autoUpload.SetAutoInProcess(false);
		// set up the transfer object
		SetState(PBStateTransferComplete);
		m_pbTransfer->SetPIN( PBT_AUTO_UPLOAD_PIN ); // No pin for auto transfers, set to -1 so PMT knows not to verify it.
		m_pbTransfer->SetDenomination(config.GetDenomination());
	}
	else if(sendToUI && !IsCreditTransfer())
	{
		PersonalBankerAction action(PBActionSendCompleteToUI);
		if (m_autoUpload.IsAutoUploadPending() && 
			(PBStateTransferComplete == m_state))
		{
			if (m_pbTransfer->GetAutoUploadErrorFlags())
			{
				m_autoUpload.SetAutoUploadPending(false);
			}
		}

		m_autoUpload.SetStartUploadOver(false);

		// Clear any action that got us here.
		if (m_readyAction == PBActionReleaseSession)
		{
			ClearPersonalBankerAction();
		}
		m_pbStateForUi = m_state;
		SetPersonalBankerAction(action);
	}
}

void CPersonalBanker::SetToReleaseTransferSession()
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	
	if(m_pbTransfer != nullptr)
	{
		m_pbTransfer->SetPersonalBankerType( SessionRelease );

		// Need to reset PromoPersonalBankerType also to prevent second upload transaction from going to poller
		m_pbTransfer->SetPromoPersonalBankerType( SessionRelease );
		m_pbStateForUi = m_state;
		SetState(PBStateReleaseSent);
	}
}

bool CPersonalBanker::ProcessCancelPersonalBankerSession(CPersonalBankerTransfer *pbTransfer, bool displayCancelSession)
{
	bool releasePersonalBankerSession(false);
	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		if (pbTransfer != nullptr) 
		{
			if (pbTransfer->GetPersonalBankerTransferType() == ExternalCredit)
			{
				SetState(PBStateInfoReceived);

				DeleteTransfer();

				m_pbTransfer = new CPersonalBankerTransfer(*pbTransfer);
			}
		}

		if ( m_pbTransfer != nullptr )
		{
			PersonalBankerState state = m_state;

			//TR #26940 add PBStateWaitingForUploadToStart because we need to cancel the auto uploads.
			if ( state == PBStateInquireSent || 
				 state == PBStateInfoReceived || 
				 state == PBStateWaitingForUploadToStart ||
				 m_pbTransfer->GetPersonalBankerType() == CanceledCreditTransfer)
			{
				if (displayCancelSession) // false everywhere except when credit transfer is canceled
				{
					m_pbTransfer->SetPersonalBankerType(CreditTransfer);
				}
				releasePersonalBankerSession = true;
			}
		}
		else if(m_autoUpload.IsAutoUploadPending() || m_autoUpload.IsAutoInProcess() == true)
		{		
			m_autoUpload.SetAutoInProcess(false);
			m_autoUpload.SetAutoUploadPending(false);
			SetWaitingForTransferToBeCompleted(true);
		}
	}

	return releasePersonalBankerSession;
}

void CPersonalBanker::ProcessRestorePersonalBankerSession()
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	
	if (m_pbTransfer != nullptr)
	{
		SetState(PBStateInfoReceived);
		m_pbTransfer->SetPersonalBankerType(SessionRequest);
		m_pbTransfer->SetPersonalBankerTransferType(Undeclared);
	}
}

void CPersonalBanker::ProcessPersonalBankerTransactionAcked(WORD sequenceNumber)
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	
	CPersonalBankerLog *tempPBLog(nullptr);
	int logCount = 0;

	// Update the pb log to show we are no longer pending
	tempPBLog = (CPersonalBankerLog *)m_pbLog.GetLastLog();
	logCount = 0;
	while ( tempPBLog != nullptr && logCount <m_pbLog.GetNumItems() && tempPBLog->GetSequenceNumber() != sequenceNumber )
	{
		tempPBLog = (CPersonalBankerLog *)m_pbLog.GetPreviousLog();
		logCount++;
	}
	if( tempPBLog != nullptr && logCount < m_pbLog.GetNumItems() )
	{
		tempPBLog->SetIsPending( false );
		m_pbLog.UpdateLastLog( tempPBLog );
	}


	// Update the pb success log to show we are no longer pending
	tempPBLog = (CPersonalBankerLog *)m_pbSuccessLog.GetLastLog();
	logCount = 0;
	while ( tempPBLog != nullptr && logCount < m_pbSuccessLog.GetNumItems() && tempPBLog->GetSequenceNumber() != sequenceNumber )
	{
		tempPBLog = (CPersonalBankerLog *)m_pbSuccessLog.GetPreviousLog();
		logCount++;
	}
	if( tempPBLog != nullptr && logCount < m_pbSuccessLog.GetNumItems() )
	{
		tempPBLog->SetIsPending( false );
		m_pbSuccessLog.UpdateLastLog( tempPBLog );
	}
}

bool CPersonalBanker::GetUnlockEgmDuringTransfer() const
{
	bool unlockEgmDuringTransfer(false);

	{
		lock_guard<recursive_mutex> lock(m_CriticalSection);
		
		if(m_pbTransfer != nullptr)
		{
			if((IsPersonalBankerTransferFromGame() && m_state == PBStateTransferingFromToGame) ||
				(m_autoUpload.IsAutoStartedOnce() && (m_state == PBStateWaitingForUploadToStart || m_state == PBStateInquireSent)))
			{
				unlockEgmDuringTransfer = true;
			}
		}
	}

	return unlockEgmDuringTransfer;
}

PersonalBankerTransferType CPersonalBanker::GetPersonalBankerTransferType() const
{
	PersonalBankerTransferType transferType(Undeclared);
	if(m_pbTransfer != nullptr)
	{
		transferType = m_pbTransfer->GetPersonalBankerTransferType();
	}
	return transferType;
}

__int64 CPersonalBanker::GetTransferAmount()
{
	__int64 amount(0);
	if(m_pbTransfer != nullptr)
	{
		amount = m_pbTransfer->GetAmount();
	}
	return amount;
}

void CPersonalBanker::SetPersonalBankerType(PersonalBankerType type)
{
	if(m_pbTransfer != nullptr)
	{
		m_pbTransfer->SetPersonalBankerType(type);
	}
}

void CPersonalBanker::SetPersonalBankerTransferType(PersonalBankerTransferType type)
{
	if (m_pbTransfer != nullptr)
	{
		m_pbTransfer->SetPersonalBankerTransferType(type);
	}
}

bool CPersonalBanker::TransferHasAmount()
{
	bool hasAmount(false);

	if(m_pbTransfer != nullptr)
	{
		hasAmount = m_pbTransfer->HasAmount();
	}

	return hasAmount;
}

bool CPersonalBanker::IsTransferRequestCanceled() const
{
	bool transferIsCancelled(false);

	if(m_pbTransfer != nullptr)
	{
		transferIsCancelled = m_pbTransfer->IsTransferRequestCanceled();
	}

	return transferIsCancelled;
}

void CPersonalBanker::ResetTransferSessionStartTime(DWORD tickCount)
{
	if(m_pbTransfer != nullptr)
	{
		m_pbTransfer->ResetSessionStartTime(tickCount);
	}
}

void CPersonalBanker::SetAutoInProcess(bool autoInProcess)
{
	m_autoUpload.SetAutoInProcess(autoInProcess);
}

bool CPersonalBanker::IsAutoInProcess() const
{
	return m_autoUpload.IsAutoInProcess();
}

void CPersonalBanker::MakeAutoUploadPending()
{
	LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
	m_autoUpload.SetAutoUploadPending(true);
}

bool CPersonalBanker::IsAutoTransferPending()
{
	bool transferIsPending(false);

	LogString(PBT_LOG, "IsAutoTransferPending IsAutoInProcess=%u IsAutoUploadPending=%u m_autoUpload.GetCard()!=nullptr=%u",
		m_autoUpload.IsAutoInProcess(), m_autoUpload.IsAutoUploadPending(), m_autoUpload.GetCard() != nullptr);
	if ((m_autoUpload.IsAutoInProcess() || m_autoUpload.IsAutoUploadPending()) && m_autoUpload.GetCard() != nullptr)
	{
		transferIsPending = true;
	}

	return transferIsPending;
}

void CPersonalBanker::SetAutoUploadTimerExpired()
{
	m_autoUpload.SetUploadTimerExpired();
}

CCard* CPersonalBanker::GetAutoUploadCard()
{
	return m_autoUpload.GetCard();
}

bool CPersonalBanker::IsAutoUploadPending() const
{
	return m_autoUpload.IsAutoUploadPending();
}

bool CPersonalBanker::IsSessionReleasedDueToTimeout() const
{
	return m_ReleaseSessionDueToTimeout;
}

bool CPersonalBanker::IsInvalidTransactionIDReceived() const
{
	return m_InvalidTransIdReceived;
}

PersonalBankerAction CPersonalBanker::LimitSessionInProgressToUIExpiredAction()
{
	PersonalBankerAction retVal = CUtilities::GetTickCount() - m_lastSessionInProgressToUI > TELL_UI_OF_FAILED_PB_SLEEP_MILLISECONDS ? PBActionSendCardSessionInProgressToUI : PBActionNone;

	return retVal;
}

void CPersonalBanker::SetLastCardedSessionInProgressToCurrent(DWORD custom)
{
	if (custom == 0)
	{
		m_lastSessionInProgressToUI = CUtilities::GetTickCount();
	}
	else
	{
		m_lastSessionInProgressToUI = custom;
	}

	return;
}

bool CPersonalBanker::IsCreditTransfer() const
{
	bool isCreditTransfer(false);

	if(m_pbTransfer != nullptr)
	{
		isCreditTransfer = m_pbTransfer->GetPersonalBankerType() == CreditTransfer;
	}

	return isCreditTransfer;	
}

bool CPersonalBanker::ProcessInvalidTransIdIfNecessary(CPersonalBankerInfo *pbInfo, long slotMastId, string& personalBankerId)
{
	m_InvalidTransIdReceived = false;

	// The error code InvalidID is not a error, just a message routing convention used internally within nCompass for IsBannedFromRedemptions().
	// Even when a PB session IsBannedFromRedemptions(), we still need IsValidID() for automatic transfers.  
	if (pbInfo &&
		m_state == PBStateInquireSent &&
		(!pbInfo->HasError() || pbInfo->GetErrorCode() == InvalidID) &&
		!CPersonalBankerTransfer::IsValidID(slotMastId, pbInfo->GetPersonalBankerID(), pbInfo->GetPBIDLength()))
	{
		m_InvalidTransIdReceived = true;

		// Invalid PBT ID returned from poller
		// Send tilt transaction and Set pbt error code
		if (m_pbTransfer != nullptr)
		{
			m_pbTransfer->SetCancelledType(InvalidTransID);
			m_pbTransfer->SetPersonalBankerType(CanceledSessionRequest);
			SetState(PBStateTransferComplete);
			m_pbStateForUi = m_state;
		}

		//Check for null PB ID
		if (pbInfo->GetPBIDLength() > 0)
		{
			personalBankerId = string(pbInfo->GetPersonalBankerID(), pbInfo->GetPBIDLength());
		}
	}

	return m_InvalidTransIdReceived;
}

bool CPersonalBanker::CheckValidTransferAmount(PersonalBankerTransferType type, DWORD amount)
{
	bool retVal = true;
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	if (m_pbInfo)
	{
		switch (type)
		{
		case CashToGame:
			if (amount < m_pbInfo->GetMinimumCashDown())
			{
				retVal = false;
			}
			else if (amount > m_pbInfo->GetMaximumCashDown())
			{
				retVal = false;
			}
			else if (amount > m_pbInfo->GetCashBalance() / ONE_DOLLAR_IN_CENTS)
			{
				retVal = false;
			}
			else if (amount <= 0)
			{
				retVal = false;
			}
			break;
		case PointsToGame:
			if (amount < m_pbInfo->GetMinimumPointsDollarValueDownload())
			{
				retVal = false;
			}
			else if (amount > m_pbInfo->GetMaximumPointsDollarValueDownload())
			{
				retVal = false;
			}
			else if (amount > (m_pbInfo->GetPointsAvailable() / m_pbInfo->GetPointsPerDollar()))
			{
				retVal = false;
			}
			else if (amount <= 0)
			{
				retVal = false;
			}
			break;
		case PromoDollarsToGame:
			if (amount < m_pbInfo->GetMinimumPromoDown())
			{
				retVal = false;
			}
			else if (amount > m_pbInfo->GetMaximumPromoDown())
			{
				retVal = false;
			}
			else if (amount > m_pbInfo->GetPromo2Balance() / ONE_DOLLAR_IN_CENTS)
			{
				retVal = false;
			}
			else if (amount <= 0)
			{
				retVal = false;
			}
			break;
		case CashFromGame:
			if (amount < m_pbInfo->GetMinimumCashUp())
			{
				retVal = false;
			}
			else if (amount > min(m_pbInfo->GetCashBalanceOnGame()/ ONE_DOLLAR_IN_CENTS, m_pbInfo->GetMaximumCashUp()))
			{
				retVal = false;
			}
			else if (amount <= 0)
			{
				retVal = false;
			}
			break;
		case PromoDollarsFromGame:
			if (amount < m_pbInfo->GetMinimumPromoUp())
			{
				retVal = false;
			}
			else if (amount > m_pbInfo->GetMaximumPromoUp())
			{
				retVal = false;
			}
			else if (amount <= 0)
			{
				retVal = false;
			}
			break;
		case ExternalCredit:
			if (amount < 1)
			{
				retVal = false;
			}
			break;
		default:
			retVal = true;
		}
	}
	else
	{
		LogString(PBT_LOG, "[%s] Warning: PBInfo object found NULL.", __FUNCTION__);
	}

	return retVal;

}

bool CPersonalBanker::IsPinValidLength(const std::string &pin)
{
	return pin.length() == VALID_PIN_LENGTH;
}

string CPersonalBanker::ResponseForPbPendingLogs()
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key(PB_LOGS.c_str());
	{
		writer.StartArray();
		{
			m_CriticalSection.lock();
			unique_ptr<CLogs> logs(GetLogCopy());
			m_CriticalSection.unlock();
			int numLogs(logs->GetNumItems());
			string timeStamp, amount, result, source, ocr, transactionId, sequenceNumber;

			CPersonalBankerLog *log = (CPersonalBankerLog *)logs->GetLastLog();

			//Check to see if last log is pending
			int index(0);
			while (log != nullptr && log->IsPending() && index < numLogs)
			{
				writer.StartObject();
				{
					timeStamp = log->GetDateTimeAsString();
					amount = FormatString("$%d.%02d", (int)(log->GetAmount() / 
						(__int64)ONE_DOLLAR_IN_CENTS), (int)(log->GetAmount() % (__int64)ONE_DOLLAR_IN_CENTS));
					result = log->IsSuccessful() == true ? "Successful" : "Failed";
					source = log->GetSourceString();

					int length;
					byte *OCR = log->GetCardID(length);
					ocr = CUtilities::MakeStringFromBCD(OCR, length);

					transactionId = string(log->GetTransID(length));
					sequenceNumber = FormatString("%d", log->GetSequenceNumber());

					writer.Key("TimeStamp");		writer.String(timeStamp);
					writer.Key("Amount");			writer.String(amount);
					writer.Key("Result");			writer.String(result);
					writer.Key("Source");			writer.String(source);
					writer.Key("PlayerOcr");		writer.String(ocr);
					writer.Key("TransactionId");	writer.String(transactionId);
					writer.Key("SequenceNumber");	writer.String(sequenceNumber);
				}
				writer.EndObject();

				++index;

				log = (CPersonalBankerLog *)logs->GetPreviousLog();
			}
		}
		writer.EndArray();
	}
	writer.EndObject();

	return writeStream.m_str;
}

string CPersonalBanker::ResponseForPbLogs(const string& topic)
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key(PB_LOGS.c_str());
	{
		writer.StartArray();
		{
			m_CriticalSection.lock();
			unique_ptr<CLogs> logs(topic == PB_TRANSACTION_LOGS ? GetLogCopy() : GetSuccessLogCopy());
			m_CriticalSection.unlock();
			int numLogs(logs->GetNumItems());
			string timeStamp, amount, result, source, ocr, transactionId, sequenceNumber, errorCode;

			CPersonalBankerLog *log = (CPersonalBankerLog *)logs->GetFirst();

			for (int index = 0; index < numLogs; index++)
			{
				if (log)
				{
					writer.StartObject();
					{
						timeStamp = FormatString("(%d) %s", index + 1, log->GetDateTimeAsString().c_str());

						amount = FormatString("$%s.%02d", CUtilities::GetFormattedNumberString((DWORD)(log->GetAmount() /
							(__int64)ONE_DOLLAR_IN_CENTS)).c_str(), (int)(log->GetAmount() % (__int64)ONE_DOLLAR_IN_CENTS));

						// For credit transfers that time out, we show 'Unknown' in the log since we don't really know
						// at the Sentinel level whether the transfer failed or we just didn't receive the response.
						if (log->GetCancellationType() == CreditTransferErrorTimeout)
						{
							result = "Unknown";
						}
						else
						{
							result = log->IsSuccessful() ? "Successful" : "Failed";
						}

						source = log->GetSourceString();

						int length;
						byte *OCR = log->GetCardID(length);
						ocr = CUtilities::MakeStringFromBCD(OCR, length);

						transactionId = string(log->GetTransID(length));
						sequenceNumber = FormatString("%d", log->GetSequenceNumber());

						if (log->GetCancellationType() > TransInfoUnavailable ||
							log->GetCancellationType() <= (PollerCancellationType)_Unknown)
						{
							errorCode = "N/A";
						}
						else
						{
							errorCode = PB_CANCELLATION_CODES[log->GetCancellationType()];
							errorCode += " ";
							errorCode += PB_CANCELLATION_TOKEN[log->GetCancellationType()];
						}

						writer.Key("TimeStamp");		writer.String(timeStamp);
						writer.Key("Amount");			writer.String(amount);
						writer.Key("Result");			writer.String(result);
						writer.Key("Source");			writer.String(source);
						writer.Key("PlayerOcr");		writer.String(ocr);
						writer.Key("TransactionId");	writer.String(transactionId);
						writer.Key("SequenceNumber");	writer.String(sequenceNumber);
						writer.Key("ErrorCode");		writer.String(errorCode);

					}
					writer.EndObject();

					log = (CPersonalBankerLog *)logs->GetNext();
				}
				else
				{
					break;
				}
			}
		}
		writer.EndArray();
	}
	writer.EndObject();

	return writeStream.m_str;
}

std::string CPersonalBanker::ResponseForPBInfo()
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	if (IsSessionReleasedDueToTimeout())
	{
		// Generate response in case of pin_validation or pbt_session timeout
		writer.StartObject();
		writer.Key("Successful");		writer.Bool(false);
		writer.Key("Error");			writer.String(m_pbInfo ? "pbtSessionTimeout" : "pinValidationTimeout");
		writer.EndObject();
	}
	else if (IsInvalidTransactionIDReceived())
	{
		// Generate response in case of invalid transaction ID received from poller
		writer.StartObject();
		writer.Key("Successful");		writer.Bool(false);
		writer.Key("Error");			writer.String("pbtInvalidTransactionId");
		writer.EndObject();
	}
	else if (m_pbInfo)
	{
		// Force exit is used for mobile session exit and any existing PBT session during cashout
		if (m_pbInfo->IsSessionForceExit())
		{
			writer.StartObject();
			writer.Key("Successful");		writer.Bool(false);
			writer.Key("Error");			writer.String("pbtSessionForceExit");
			writer.EndObject();
		}
		else if (m_pbInfo->HasError())
		{
			string error;
			switch (m_pbInfo->GetErrorCode())
			{
			case PBError::AccessDenied:
				error = "accessDenied";
				break;
			case PBError::AccessError:
				error = "accessError";
				break;
			case PBError::AccountInUse:
				error = "accountInUse";
				break;
			case PBError::AccountLocked:
				error = "accountLocked";
				break;
			case PBError::AcctHasNoPin:
				error = "acctHasNoPin";
				break;
			case PBError::BannedPinEntered:
				error = "bannedPinEntered";
				break;
			case PBError::CreditTransferCommserror:
				error = "creditTransferCommserror";
				break;
			case PBError::CreditTransferInsufficientCredits:
				error = "creditTransferInsufficientCredits";
				break;
			case PBError::CreditTransferOtherIssue:
				error = "creditTransferOtherIssue";
				break;
			case PBError::InvalidID:
				error = "invalidID";
				break;
			case PBError::InvalidPin:
				error = "invalidPin";
				break;
			case PBError::NewPinRequired:
				error = "newPinRequired";
				break;
			default:
				error = "unknownError";
				break;
			}

			writer.StartObject();
			writer.Key("Successful");		writer.Bool(false);
			writer.Key("Error");			writer.String(error);
			writer.EndObject();
		}
		else if (m_receivedBalancesFromEGM)
		{
			writer.StartObject();
			writer.Key("Successful");		writer.Bool(true);
			writer.Key("Error");			writer.String("");

			writer.Key("CashUnavailable");	writer.Bool(m_pbInfo->IsUnableToMoveLeftoverCash());

			m_pbInfo->SetShowFromGameAndToGameButtonFlags();
			writer.Key("IsTransferToGameEnabled");			writer.Bool(m_pbInfo->GetShowToGameButton());
			writer.Key("IsTransferFromGameEnabled");		writer.Bool(m_pbInfo->GetShowFromGameButton());

			writer.Key("ShowPoints");						writer.Bool(!m_pbInfo->GetShowPointsVsAccountBalance());
			writer.Key("Points");							writer.Int64(m_pbInfo->GetPointsAvailable());
			writer.Key("PointsAsCash");						writer.Double(m_pbInfo->GetPointsAvailable() / m_pbInfo->GetPointsPerDollar());
			writer.Key("Cash");								writer.Double(m_pbInfo->GetCashBalance() / ONE_DOLLAR_IN_CENTS);
			writer.Key("Promo");							writer.Double(m_pbInfo->GetPromo2Balance() / ONE_DOLLAR_IN_CENTS);

			writer.Key("CashOnGame");						writer.Double(m_pbInfo->GetCashBalanceOnGame() / (double)ONE_DOLLAR_IN_CENTS);
			writer.Key("PromoOnGame");						writer.Double(m_pbInfo->GetPromoBalanceOnGame() / (double)ONE_DOLLAR_IN_CENTS);
			writer.Key("IsTransferCashFromGame");			writer.Bool(m_pbInfo->DisplayTransferCashFromGame());
			writer.Key("IsTransferPromoFromGame");			writer.Bool(m_pbInfo->DisplayTransferPromoFromGame());
			writer.Key("IsTransferCashToGame");				writer.Bool(m_pbInfo->DisplayTransferCashToGame());
			writer.Key("IsTransferPromoToGame");			writer.Bool(m_pbInfo->DisplayTransferPromoToGame());
			writer.Key("IsTransferPointsToGame");			writer.Bool(m_pbInfo->DisplayTransferPointsToGame());
			writer.Key("IsCashInactive");					writer.Bool(m_pbInfo->IsCashInactive());
			writer.Key("IsPartialTransferCash");			writer.Bool(m_pbInfo->IsPartialTransferCashFromGameEnabled());

			writer.Key("MinCashUpload");					writer.Int64(m_pbInfo->GetMinimumCashUp());
			writer.Key("MaxCashUpload");					writer.Int64(m_pbInfo->GetMaximumCashUp());
			writer.Key("MinCashDownload");					writer.Int64(m_pbInfo->GetMinimumCashDown());
			writer.Key("MaxCashDownload");					writer.Int64(m_pbInfo->GetMaximumCashDown());
			writer.Key("MinPromoDownload");					writer.Int64(m_pbInfo->GetMinimumPromoDown());
			writer.Key("MaxPromoDownload");					writer.Int64(m_pbInfo->GetMaximumPromoDown());
			writer.Key("MinPointsDownload");				writer.Int64(m_pbInfo->GetMinimumPointsDollarValueDownload());
			writer.Key("MaxPointsDownload");				writer.Int64(m_pbInfo->GetMaximumPointsDollarValueDownload());

			writer.Key("IsExternalCreditProviderEnabled");	writer.Bool(m_pbInfo->ExternalCreditProviderEnabled());
			writer.Key("IsCreditFeeFixed");					writer.Bool(m_pbInfo->CreditFeeFixed());
			writer.Key("CreditFee");						writer.Uint(m_pbInfo->CreditFee());

			writer.EndObject();
		}
	}

	return writeStream.m_str;
}

std::string CPersonalBanker::ResponseForPBTransfer()
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	if (IsSessionReleasedDueToTimeout())
	{
		// Generate response in case of pbt_session timeout
		writer.StartObject();
		writer.Key("Successful");		writer.Bool(false);
		writer.Key("Error");			writer.String("pbtSessionTimeout");
		writer.EndObject();
	}
	else if (m_pbTransfer != nullptr)
	{
		if (m_readyAction == PBActionProcessAuthorizationCanceled)
		{
			writer.StartObject();
			writer.Key("Successful");		writer.Bool(false);
			writer.Key("Error");			writer.String("authorizationCancelled");
			writer.EndObject();
		}
		else if (m_readyAction == PBActionSendCompleteToUI)
		{
			writer.StartObject();

			if (m_pbStateForUi != PBStateTransferComplete)
			{
				std::string error;
				if (m_pbStateForUi == PBStateAuthDenied)
				{
					error = "authorizationDenied"; // Authorization Failed, See Attendant
				}
				else if ((m_pbTransfer->GetPersonalBankerTransferType() == ExternalCredit))
				{
					// to do
				}

				writer.Key("Successful");		writer.Bool(false);
				writer.Key("Error");			writer.String(error);
			}
			else if (m_pbTransfer->IsTransferRequestCanceled())
			{
				writer.Key("Successful");		writer.Bool(false);
				writer.Key("Error");			writer.String(m_pbTransfer->GetCancelledToken());
			}
			else
			{
				std::string transferTypeStr;
				switch (m_pbTransfer->GetPersonalBankerTransferType())
				{
				case PersonalBankerTransferType::Undeclared:
					transferTypeStr = "Undeclared";
					break;
				case PersonalBankerTransferType::CashToGame:
					transferTypeStr = "CashToGame";
					break;
				case PersonalBankerTransferType::PointsToGame:
					transferTypeStr = "PointsToGame";
					break;
				case PersonalBankerTransferType::PromoDollarsToGame:
					transferTypeStr = "PromoDollarsToGame";
					break;
				case PersonalBankerTransferType::CashFromGame:
					transferTypeStr = "CashFromGame";
					break;
				case PersonalBankerTransferType::PointsFromGame:
					transferTypeStr = "PointsFromGame";
					break;
				case PersonalBankerTransferType::PromoDollarsFromGame:
					transferTypeStr = "PromoDollarsFromGame";
					break;
				case PersonalBankerTransferType::ExternalCredit:
					transferTypeStr = "ExternalCredit";
					break;
				case PersonalBankerTransferType::AllFromGame:
					transferTypeStr = "AllFromGame";
					break;
				}

				__int64 cash = m_pbTransfer->GetAmount() + m_pbTransfer->GetNonrestrictedAmount();
				__int64 promo = m_pbTransfer->GetRestrictedAmount();

				writer.Key("Successful");				writer.Bool(true);
				writer.Key("Error");					writer.String("");
				writer.Key("TransferType");				writer.String(transferTypeStr);
				writer.Key("Cash");						writer.Double(cash / (double)ONE_DOLLAR_IN_CENTS);
				writer.Key("Promo");					writer.Double(promo / (double)ONE_DOLLAR_IN_CENTS);
				writer.Key("CashoutErrorFlags");		writer.Int(m_pbTransfer->GetCashoutErrorFlags());
				writer.Key("AutoUploadErrorFlags");		writer.Int(m_pbTransfer->GetAutoUploadErrorFlags());
			}

			writer.EndObject();
		}
		else if (m_pbTransfer->IsTransferRequestCanceled() && m_pbTransfer->GetAutoUploadErrorFlags() > 0)
		{
			// This is the case when we have auto-upload error for promos.
			// There may or may not be any cash related error.
			writer.StartObject();
			writer.Key("Successful");				writer.Bool(false);
			writer.Key("Error");					writer.String("autoUploadError");
			writer.Key("AutoUploadErrorFlags");		writer.Int(m_pbTransfer->GetAutoUploadErrorFlags());
			writer.EndObject();
		}
	}

	return writeStream.m_str;
}

std::string CPersonalBanker::ResponseForResetPin()
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	if (m_resetPinResult != DEFAULTRESETPINRESPONSE)
	{
		writer.StartObject();
		writer.Key("PinResetResult");	writer.Int(m_resetPinResult);
		writer.EndObject();
	}

	return writeStream.m_str;
}



bool CPersonalBanker::IsAuthorisationInProcess()
{
	return m_pbtAuthorizationInProcess;
}

bool CPersonalBanker::IsToGame(PersonalBankerTransferType type)
{
	return (type == CashToGame || type == PointsToGame || type == PromoDollarsToGame);
}

bool CPersonalBanker::IsTransferSuccess()
{
	return(!IsTransferRequestCanceled() && 
		GetInfoCopy() && !GetInfoCopy()->HasError());

}

void CPersonalBanker::SetPlayerIdWaitingPinReset(DWORD playerId)
{
	m_playerIdWaitingPinReset = playerId;
}

bool CPersonalBanker::IsPlayerIdWaitingPinReset(DWORD playerId)
{
	return (playerId == m_playerIdWaitingPinReset);
}

void CPersonalBanker::SetNewPin(DWORD pin)
{
	m_newPin = pin;
}

DWORD CPersonalBanker::GetNewPin()
{
	return m_newPin;
}

DWORD CPersonalBanker::GetCurrentPin()
{
	if (m_pbTransfer)
	{
		return m_pbTransfer->GetPIN();
	}
	else
	{
		LogString(PBT_LOG, "[%s] Warning: Transfer object is NULL, returning -1 as PIN.", __FUNCTION__);
		return (DWORD)-1;
	}
}

bool CPersonalBanker::IsCashInactive() const
{
	return m_cashInactive;
}

void CPersonalBanker::ClearInactiveAccountsFlag()
{
	m_cashInactive = false;
}

void CPersonalBanker::SetEligibleCreditsToUpload(DWORD cash, DWORD promos)
{
	m_eligibleCashToUpload = cash;
	m_eligiblePromosToUpload = promos;
}

void CPersonalBanker::SetGuestPlayer(bool guest)
{
	m_guestPlayer = guest;
}

const char* CPersonalBanker::GetActionString(PersonalBankerAction action)
{
	if (action < NUMBER_OF_PB_ACTIONS)
	{
		return PersonalBankerActionStr[action];
	}
	else
	{
		return "unknown";
	}
}

void CPersonalBanker::SetToDelayAutoUpload()
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	m_autoUploadPendingFlags.m_flags.m_delayAutoUpload = true;
	m_autoUploadPendingFlags.m_flags.m_cancelPendingAutoUpload = false;
	m_autoUploadPendingFlags.m_flags.m_cancellable = false;
	m_pendingAutoUploadTickCount = CUtilities::GetTickCount();
	LogString(PBT_LOG, "SetToDelayAutoUpload tickcount=%u", m_pendingAutoUploadTickCount);
}

bool CPersonalBanker::DelayAutoUpload()
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);

	if (m_autoUploadPendingFlags.m_flags.m_delayAutoUpload)
	{
		DWORD autoUploadDelayTickCount(m_autoUploadDelaySeconds * MILLISECONDS_PER_SECOND);
		DWORD pendingAutoUploadTickCount(CUtilities::GetTickCount() - m_pendingAutoUploadTickCount);
		if (pendingAutoUploadTickCount > autoUploadDelayTickCount)
		{
			LogString(PBT_LOG, "DelayAutoUpload-expired");
			m_autoUploadPendingFlags.m_flags.m_delayAutoUpload = false;
		}
	}

	return m_autoUploadPendingFlags.m_flags.m_delayAutoUpload;
}

void CPersonalBanker::SetAutoUploadDelayValue(DWORD seconds)
{
	m_autoUploadDelaySeconds = seconds;
}

void CPersonalBanker::CancelPendingAutoUploadIfNecessary(WORD pollException)
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	if (m_autoUploadPendingFlags.m_flags.m_delayAutoUpload)
	{
		LogString(PBT_LOG, "CancelPendingAutoUpload pollexception=%u", pollException);
		m_autoUploadPendingFlags.m_flags.m_cancelPendingAutoUpload = true;
	}
}

void CPersonalBanker::AppendCashoutErrorFlags(WORD error)
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);

	if (m_pbTransfer != nullptr)
	{
		m_pbTransfer->AppendCashoutErrorFlags(error);
	}
}

WORD CPersonalBanker::GetCashoutErrorFlags() const
{
	WORD cashoutErrorFlags = 0;
	lock_guard<recursive_mutex> lock(m_CriticalSection);

	if (m_pbTransfer != nullptr)
	{
		cashoutErrorFlags = m_pbTransfer->GetCashoutErrorFlags();
	}
	return cashoutErrorFlags;
}

WORD CPersonalBanker::GetAutoUploadErrorFlags() const
{
	WORD autoUploadErrorFlags = 0;
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	
	if (m_pbTransfer != nullptr)
	{
		autoUploadErrorFlags = m_pbTransfer->GetAutoUploadErrorFlags();
	}
	return autoUploadErrorFlags;
}

WORD CPersonalBanker::ValidateCreditsForCashoutToHost(const CPlayer *player, DWORD &eligibleCashToUpload, DWORD &eligiblePromosToUpload) const
{
	WORD cashoutErrorFlags = 0;

	if (player != nullptr)
	{
		if (player->IsMaxCardLimitReached())
		{
			LogString(PBT_LOG, "[%s] Cashout error : Max card limit reached", __FUNCTION__);
			cashoutErrorFlags = eligibleCashToUpload ? CashoutError::CASHOUT_ERROR_MAX_CARD_LIMIT_CASH_ON_GAME : CashoutError::CASHOUT_ERROR_MAX_CARD_LIMIT;
			eligibleCashToUpload = eligiblePromosToUpload = 0;
		}
		else if (player->IsGuest())
		{
			LogString(PBT_LOG, "[%s] Cashout error : Guest player", __FUNCTION__);
			cashoutErrorFlags = eligibleCashToUpload ? CashoutError::CASHOUT_ERROR_GUEST_PLAYER_CASH_ON_GAME : CashoutError::CASHOUT_ERROR_GUEST_PLAYER;
			eligibleCashToUpload = eligiblePromosToUpload = 0;
		}
		else if (IsCashInactive() && eligibleCashToUpload)
		{
			LogString(PBT_LOG, "[%s] Cashout error : Cash inactive", __FUNCTION__);
			cashoutErrorFlags = CashoutError::CASHOUT_ERROR_CASH_INACTIVE;
			eligibleCashToUpload = 0;
		}
	}

	return cashoutErrorFlags;
}

void CPersonalBanker::SetResetPinResult(WORD resetPinResult)
{
	m_resetPinResult = resetPinResult;
}

void CPersonalBanker::ClearResetPinResult()
{
	m_resetPinResult = DEFAULTRESETPINRESPONSE;
}

void CPersonalBanker::SetSessionForceExit()
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);
	if (m_pbInfo)
	{
		m_pbInfo->SetSessionForceExit();
	}
}

bool CPersonalBanker::IsSessionForceExit() const
{
	lock_guard<recursive_mutex> lock(m_CriticalSection);

	return (m_pbInfo && m_pbInfo->IsSessionForceExit());
}
