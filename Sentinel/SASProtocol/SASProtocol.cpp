#include "stdafx.h"
#include "SASProtocol.h"

// Sentinel includes.
#include "Hardware/WatchDog.h"
#include "SASPollMessage.h"
#include "SASMeterCollection.h"
#include "LongPoll01Message.h"
#include "LongPoll02Message.h"
#include "LongPoll0EMessage.h"
#include "LongPoll10Message.h"
#include "LongPoll19Message.h"
#include "LongPoll1AMessage.h"
#include "LongPoll1CMessage.h"
#include "LongPoll1DMessage.h"
#include "LongPoll1EMessage.h"
#include "LongPoll1FMessage.h"
#include "LongPoll20Message.h"
#include "LongPoll27Message.h"
#include "LongPoll2AMessage.h"
#include "LongPoll2BMessage.h"
#include "LongPoll2DMessage.h"
#include "LongPoll2EMessage.h"
#include "LongPoll2FMessage.h"
#include "LongPoll31Message.h"
#include "LongPoll32Message.h"
#include "LongPoll33Message.h"
#include "LongPoll34Message.h"
#include "LongPoll35Message.h"
#include "LongPoll36Message.h"
#include "LongPoll37Message.h"
#include "LongPoll38Message.h"
#include "LongPoll39Message.h"
#include "LongPoll3AMessage.h"
#include "LongPoll46Message.h"
#include "LongPoll4CMessage.h"
#include "LongPoll50Message.h"
#include "LongPoll52Message.h"
#include "LongPoll6FMessage.h"
#include "LongPoll7FMessage.h"
#include "LongPoll80Message.h"
#include "LongPoll86Message.h"
#include "LongPoll8AMessage.h"
#include "LongPoll9AMessage.h"
#include "LongPollB7Message.h"
#include "../InternalMessage.h"
#include "SASGeneralPoll.h"
#include "SASLimits.h"
#include "../Sentinel.h"
#include "Logging/LoggerTypes.h"
#include "Logging/LogString.h"
#include "Utilities.h"

using namespace std;

// Some juristictions (i.e. West Virginia) do not allow the enable/disable
// Need to comment out the following line to not allow enable/disable.
#define ALLOW_ENABLE_DISABLE
#define SAS_SEND_COUNT 10
#define SAS_GET_METERS_CHECKIN_COUNT 5

const int SAS_SCHEDULER_SLEEP_TIME = 128;
const short SAS_GAME_DELAY = 0;
const int SEQUENCENUMBERSIZE = 3;
const int CASHOUT_TO_HOST_CONFIG_MAX_TRIES = 10;

LPCTSTR const CSASProtocol::ATIBONUS_TRANSACTION_ID_PREFIX(_T("ATIBONUS"));

// Use below transaction ID for SAS LP72 transactions whose responses needs to be ignored
// from PBT perspective (like transactions to configure cashout to host feature)
LPCTSTR const CSASProtocol::ATIGENERIC_TRANSACTION_ID_PREFIX(_T("ATIGENERIC"));

const int HIGHPRIOIRTYMETERDELAY = 200;

static UINT AFX_CDECL SendToEGMThread(PVOID threadParam);

static UINT AFX_CDECL ScheduledSASTaskThread(PVOID threadParam);

CSASProtocol::CSASProtocol(CQueueList& gcfToInternalsQueue,
									IOperatingSystem& operatingSystem)
	:
	  CSystemProtocol(gcfToInternalsQueue,
							_T(THREAD_READ_EGM_INTERNALS_QUEUE),
							INT2GCF_MessageQueueLogIdentifier,
							EVENT_WAIT_TIMEOUT,
							operatingSystem),
	  m_sasPollQueue(nullptr),
	  m_sasBills(gcfToInternalsQueue)
{
	m_PBTEnabled = false;
	m_sasState.SetAllowStepdownResetForWMS(false);
	m_sasState.SetSlotmastID(INVALID_SLOT_MAST_ID);
	m_sasState.SetAllowableEnableDisable(true);
	m_sasState.SetAllowableGameDelayPoll(true);
	m_sasState.SetIsEnhancedTicketing(false);
	m_bIsAFTSupported = false;
	m_bIsEFTSupported = false;
	m_egmTransferMode = EGMTransferMode_PENDING;
	m_bEFTCashFromGame = false;
	m_bEFTCashToGame = false;
	m_lastPollValue = 0;
	m_pbt = NULL;
	m_lp74 = NULL;
	m_bInSelfTest = false;
	m_bInUserMenu = false;
	m_bInitialized = false;
	m_state = PBStateUnknown;
	m_accountingOnlyLinks = false;

	PopulateMeterStepdowns();

	m_sendToEGMThread.reset();
	m_scheduleThread.reset();

	m_bShutdownSendToEGMThread = false;

	m_SASSendLoopCount = 0;
	m_SASGetMetersCount = 0;
	m_resetToCredit = false;
	m_totalRetryCount = 0;
	m_lastEGMEnableTickCount = 0;
	m_sasInfo = new CGames();
	m_egmAward = nullptr;
	m_egmPrizeAward = nullptr;
	 
	m_denomMult = INVALID_DENOM;
	m_billMeterSuccess = false;
	m_startSnapshot = false;
	m_snapshotAllGames = false;

	// Tracker 23188
	m_bGameDelaySent = false;
	m_queryPokerhand = false;
	m_denomGames = nullptr;

	m_ticketSeed = new CTicketSeed(CUtilities::GetTickCount());
	m_sas = nullptr;
	m_slotNumber = 0;
	m_assetNumber = 0;
	m_numberOfSubgamesMismatchCheck = DEFAULT_NUMBER_OF_SUBGAMES_MISMATCH_CHECK;
}

CSASProtocol::~CSASProtocol(void)
{
	Shutdown();

	delete m_sasPollQueue;
	if (m_ticketSeed) delete m_ticketSeed;
	delete m_sasInfo;
	delete m_egmAward;
	delete m_sas;
}

CSASInterface* CSASProtocol::GetSASInterface(CSASConsumer *sasProtocol,
															CQueueList& gcfToInternalsQueue,
															CQueueList& internalsToGCFQueue,
															IOperatingSystem& operatingSystem)
{
	CSASSerialWrapper *serial(new CSASSerialWrapper(operatingSystem));
	CSASInterface *sas(new CSASInterface(serial, sasProtocol, &gcfToInternalsQueue, &internalsToGCFQueue));
	return sas;
}

bool CSASProtocol::IsInitialized(void)
{
	return m_bInitialized;
}

void CSASProtocol::ProcessMessageFromInternals(CInternalMessage *msg)
{
	bool bDeleteMsg = true;
	//LogString(SASGENERAL, "SASProtocol: processing message %d.", msg->GetMessageType());
	try
	{
		switch (msg->GetMessageType())
		{
		case INT_MSG_GET_GAME_INFO:
			ProcessGetGameInfoFromInternals(msg);
			break;
		case INT_MSG_SET_SLOTMASTID:
			ProcessSetSlotMastIDFromInternals(msg);
			break;
		case INT_MSG_SET_TIME:
			SetGameTime();
			break;
		case INT_MSG_SET_EGM_SETTINGS:
			ProcessEGMSettingsFromInternals(msg);
			break;
		case INT_MSG_ENABLE_GAME:
			ProcessEnableGame(msg);
			break;
		case INT_MSG_ENABLE_CASHOUT_TO_HOST:
			ProcessEnableCashoutToHost(msg);
			break;
		case INT_MSG_SET_TICKETING_INFO:
			ProcessSetTicketingInfo(msg);
			break;
		case INT_MSG_TICKET_STORED:
			ProcessTicketStored();
			break;
		case INT_MSG_ALLOW_TICKET_PRINTED:
			m_sasState.SetIgnoreTicketPrinted(false);
			// Tracker #19946

			ClearTicketOut();
			break;
		case INT_MSG_PROCESS_TICKET_IN:
			ProcessTicketIn();
			break;
		case INT_MSG_TICKET_RESPONSE:
			ProcessTicketResponse(msg);
			break;
		case INT_MSG_GET_TICKET_STATUS:
			LogString(TICKETING_LOG, "INT_MSG_GET_TICKET_STATUS");
			ProcessFinalTicketStatus();
			break;
		case INT_MSG_PB_TRANSFER:
			if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx && m_sasState.GetEGMConfig().GetEGMProtocol() != NO_EGM_PROTOCOL)
			{
				ProcessPBTransfer(msg);
			}
			break;
		case INT_MSG_PB_STATUS:
			if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx && m_sasState.GetEGMConfig().GetEGMProtocol() != NO_EGM_PROTOCOL)
			{
				ProcessPBStatus(msg);
			}
			break;
		case INT_MSG_START_CAPTURING_METERS:
			ProcessMeterSnapshot(msg);
			break;
		case INT_MSG_GET_JACKPOT_INFO:
			ProcessHandpayPending();
			break;
		case INT_MSG_GET_PROGRESSIVE_HOPPER_PAID_INFO:
			ProcessProgressiveMachinePay();
			break;
		case INT_MSG_GET_PROGRESSIVE_WIN_INFO:
			ProcessProgressiveJackpot();
			break;
		case INT_MSG_METER_CHANGE_ALLOW:
			if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx && m_sasState.GetEGMConfig().GetEGMProtocol() != NO_EGM_PROTOCOL)
			{
				ProcessMeterChangeAllow(msg);
			}
			break;
		case INT_MSG_SET_SLOT_NUMBER:
			ProcessSetSlotNumber(msg);
			break;
		case INT_MSG_GET_EGM_CAPS:
		{
			GetEGMCapabilities();

			// make offline play check active
			CInternalMessage* offlineMsg = new CInternalMessage(INT_MSG_FINISHED_OFFLINE_PASS, (WORD)false);
			SendMessageToInternals(offlineMsg, QUEUE_PRIORITY_NORMAL);
			m_gameList.SetFirstPass();
		}
		break;
		case INT_MSG_GET_DENOM:
			GetGameDenomInfo();
			break;
		case INT_MSG_GET_BILL_METERS:
			LossLimitBillMeters();
			break;
		case INT_MSG_GET_PB_BALANCES:
			if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx && m_sasState.GetEGMConfig().GetEGMProtocol() != NO_EGM_PROTOCOL)
			{
				GetPBBalances();
			}
			break;
		case INT_MSG_SET_ALLOW_FLOOR_LOCKOUT:
			m_sasState.SetAllowableEnableDisable((bool)msg->GetWordData());
			break;
		case INT_MSG_SET_JACKPOT_RESET:
			if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx && m_sasState.GetEGMConfig().GetEGMProtocol() != NO_EGM_PROTOCOL)
			{
				m_resetToCredit = (bool)msg->GetWordData();
			}
			break;
		case INT_MSG_EGM_AWARD:
			if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx && m_sasState.GetEGMConfig().GetEGMProtocol() != NO_EGM_PROTOCOL)
			{
				ProcessEGMAward(msg);
			}
			break;
		case INT_MSG_GET_PROGRESSIVE_GAMES_INFO:
			if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx && m_sasState.GetEGMConfig().GetEGMProtocol() != NO_EGM_PROTOCOL)
			{
				m_gameList.SetFirstPass();
			}
			break;
		case INT_MSG_PROGRESSIVE_LEVEL_FORWARD:
			if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx && m_sasState.GetEGMConfig().GetEGMProtocol() != NO_EGM_PROTOCOL)
			{
				ProcessProgressiveData(msg);
			}
			break;
		case INT_MSG_PROGRESSIVE_ACCOUNTING_ONLY:
			m_accountingOnlyLinks = (bool)msg->GetWordData();
			break;
		case INT_MSG_QUERY_POKER_HAND:
			if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx && m_sasState.GetEGMConfig().GetEGMProtocol() != NO_EGM_PROTOCOL)
			{
				m_queryPokerhand = (bool)msg->GetWordData();
			}
			break;
		case INT_MSG_GET_GAME_LIST:
			LogString(GAMES, "INT_MSG_GET_GAME_LIST");
			GetGameList(false, false);
			break;
		case INT_MSG_RESET_JACKPOT:
			bDeleteMsg = false;
			ResetJackpot(msg);
			break;
		case INT_MSG_PROBE_PBT:
			ProbePBT(msg->GetWordData() == 1);
			break;
		case INT_MSG_PRIZE_LOCK_CLEAR:
			LockClearForEGMPrizeAward(msg);
			bDeleteMsg = false;
			break;
		case INT_MSG_CASHOUT_TO_HOST:
			if (!msg->GetWordData() /* process cashout to host cancelation */)
			{
				CancelCashoutToHost();
			}
			break;
		case INT_MSG_SET_NUMBER_OF_SUBGAMES_MISMATCH_CHECK:
			m_numberOfSubgamesMismatchCheck = msg->GetDWordData();
			LogString(GAMES, "INT_MSG_SET_NUMBER_OF_SUBGAMES_MISMATCH_CHECK-m_numberOfSubgamesMismatchCheck=%u", m_numberOfSubgamesMismatchCheck);
			break;
		case INT_MSG_CLEAR_PBT_PROBE:
			ResetPBTProbe();
			break;
		case INT_MSG_UPDATE_SAS_READ_TIMEOUT:
			if (m_sas)
			{
				m_sas->UpdateSasReadTimeout();
			}
			break;
		default:
			break;
		}
	}
	catch (...)
	{
	}

	if (bDeleteMsg) delete msg;

	CWatchDog::Instance().Checkin();
}

void CSASProtocol::ProcessSetSlotNumber(CInternalMessage *msg)
{
	WORD slotNumber = (WORD)msg->GetDWordData();
	
	if (0 != slotNumber && slotNumber != m_slotNumber)
	{
		m_slotNumber = slotNumber;
		CMeters *meters = new CMeters(MEMORY_NONE);
		meters->SetMeter(mCrd); // Credit Meter
		GetMeters(*meters, EGM_GAME_NUMBER, false, true);
		
	 	if (0 == meters->GetMeterByNumber(mCrd)->GetValue())
		{
			CLongPollB7Message *lpB7 = new CLongPollB7Message(m_gameAddress);
			lpB7->SetAssetNumber(msg->GetDWordData());
		
			lpB7->Fill();
			m_sasPollQueue->Add(lpB7, QUEUE_PRIORITY_HIGHEST);
			lpB7->WaitForResponse();

			if (lpB7->GetReturnValue() == SASPOLL_SUCCESS)
			{
				if (!lpB7->ResponseHostControlAssetNumber())
					LogString(ERROR_LOG, "LP B7: EGM doesn't support update of Asset Number.");
			 	else if(lpB7->GetAssetNumber() == msg->GetDWordData())
					LogString("SASGENERAL", "LP B7: successfully executed.");
				else
					LogString(ERROR_LOG, "LP B7: Asset Number not updated in EGM.");
			}
			else
			{
			 	LogString(SASGENERAL, "LP B7 not accepted by EGM.");
			}
			delete lpB7;
		}
		else
		{
			LogString(SASGENERAL, "Cannot set Asset number as meter value is either invalid or non-zero.");
		}
		delete meters;
	}
}

/// <summary>
/// If game is initialized, Enable/Disable command will be sent to the EGM.
/// Is Game is not initialized, EstablishEGMCommunications
/// </summary>
/// <param name="msg">Contains enable EGM flag.</param>
void CSASProtocol::ProcessEnableGame(CInternalMessage *msg)
{
	m_sasState.SetEnableEGM(msg->GetWordData() == 1 ? true : false, msg->GetWordData(true) == 1 ? true : false);
	if (!m_bInitialized)
	{
		EstablishEGMCommunications();
		m_bInitialized = true;
	}
	else
	{
		EnableDisableEGM();
	}
}

/// <summary>
/// ProcessEnableCashoutToHost
/// </summary>
/// <param name="msg">Contains enable host cashout flag.</param>
void CSASProtocol::ProcessEnableCashoutToHost(CInternalMessage *msg)
{
	LogString(SASGENERAL, "[CSASProtocol::ProcessEnableCashoutToHost]");
	m_sasState.SetEnableCashoutToHost(msg->GetWordData() == 1 ? true : false);
	m_sasState.SetCashoutToHostConfigTries(0);
	EnableDisableCashoutToHost();
}

void CSASProtocol::ProcessGetGameInfoFromInternals(CInternalMessage *msg)
{
	WORD gameNumber = msg->GetWordData();
	LogString(GAMES, "ProcessGetGameInfoFromInternals-SendSubGameMeters");
	SendSubGameInfo(gameNumber, false);
}

void CSASProtocol::ProcessSetSlotMastIDFromInternals(CInternalMessage *msg)
{
	m_sasState.SetSlotmastID((long)msg->GetDWordData());
}

void CSASProtocol::ProcessEGMSettingsFromInternals(CInternalMessage *msg)
{
	CEGMConfig *egmConfig = msg->GetEGMConfigData();

	if (egmConfig != NULL)
	{
		m_PBTEnabled = msg->GetWordData();
		if ((m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx && egmConfig->GetEGMProtocol() == IGT_SAS_3xx && egmConfig->GetEGMType() != BALLY) ||
			(egmConfig->GetEGMProtocol() == IGT_SAS_3xx && m_sasState.GetEGMConfig().GetEGMType() == BALLY && egmConfig->GetEGMType() != BALLY))
		{
			// config changed to SAS 3/4 and type not CUSTOM or config is SAS 3/4 and type changed from custom
			LogString(SASGENERAL, "SASProtocol: Setting SAS 3.");
			m_mainGameMeterStepdown.DeactivateForSAS3x();
			m_subGameMeterStepdown.DeactivateForSAS3x();
			m_PBTEnabled = false;
		}
		else if ((m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx && egmConfig->GetEGMProtocol() == IGT_SAS_3xx && egmConfig->GetEGMType() == BALLY) ||
			(egmConfig->GetEGMProtocol() == IGT_SAS_3xx && m_sasState.GetEGMConfig().GetEGMType() != BALLY && egmConfig->GetEGMType() == BALLY))
		{
			// config changed to SAS 3/4 and type is CUSTOM or config is SAS 3/4 and type changed to custom
			LogString(SASGENERAL, "SASProtocol: Setting SAS 4.");
			m_mainGameMeterStepdown.DeactivateForSAS4x();
			m_subGameMeterStepdown.DeactivateForSAS4x();
			m_PBTEnabled = false;
		}
		else if (m_sasState.GetEGMConfig().GetEGMProtocol() == IGT_SAS_3xx && egmConfig->GetEGMProtocol() != IGT_SAS_3xx)
		{
			// Tracker #22679 -- Added PBT Enabled to message word data.
			m_mainGameMeterStepdown.ResetAllStepdowns();
			m_subGameMeterStepdown.ResetAllStepdowns();

			// Adjust for SAS 5 when necessary
			if (m_sas->GetMajorVersion() == SASVERSION5)
			{
				m_mainGameMeterStepdown.DeactivateForSAS5x();
				m_subGameMeterStepdown.DeactivateForSAS5x();
			}
		}

		m_sasState.SetEGMConfig(*egmConfig);

		// Set the polling speed.
		switch (egmConfig->GetEGMPollingSpeed())
		{
		case MS200:
			m_sas->SetPollingSpeed(CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS);
			break;
		case MS40:
		default:
			m_sas->SetPollingSpeed(CSASSerialWrapper::MS40_POLLING_SPEED_MILLISECONDS);
			break;
		}


		m_sasState.SetAllowStepdownResetForWMS(false);
		if (egmConfig->GetEGMProtocol() != IGT_SAS_3xx)
		{
			// Hijacked EGM Type for SAS 3/4 use. Only do this stuff when not SAS 3/4.
			if (egmConfig->GetEGMType() == WMS)
			{
				m_sasState.SetAllowableEnableDisable(false);
				m_sasState.SetAllowStepdownResetForWMS(true);
			}
			else if (egmConfig->GetEGMType() == BALLY)
			{
				m_sasState.SetAllowableGameDelayPoll(false);
			}
		}

		LogString(SASGENERAL,
					 "SAS EGMSettings: pollingSpeed=%d allowEnableDisable=%d allowGameDelayPoll=%d wmsStepdown=%d IGTS2000=%d protocol=%d pbtEnabled=%d",
					 (int)m_sas->GetPollingSpeed(),
					 (int)m_sasState.GetAllowableEnableDisable(),
					 (int)m_sasState.GetAllowableGameDelayPoll(),
					 (int)m_sasState.GetAllowStepdownResetForWMS(),
					 (int)egmConfig->GetEGMType() == IGTS2000,
					 (int)egmConfig->GetEGMProtocol(),
					 (int)m_PBTEnabled);
	}

	m_sasBills.SetBillDropCreditsFromInternals(msg->GetInt64Data());
}

void CSASProtocol::SendMessageToSAS(CSASPollMessage *msg)
{
	TryToSendMessageToSAS(msg);

	// Tell who ever is waiting that we are done or have given up trying
	if (SASPOLL_SUCCESS == msg->GetReturnValue())
	{
		msg->ResponseReceived();
	}
	else
	{
		msg->CSASPollMessage::ResponseReceived();
	}

	// watchdog checkin
	CWatchDog::Instance().Checkin();
}

void CSASProtocol::TryToSendMessageToSAS(CSASPollMessage *msg, byte retryCount)
{
	// Check if the message contains time-sensitive information that needs to be discarded if stale or expired.
	if (msg->FillWithTickCountBeforeSending())
	{
		msg->Fill(CUtilities::GetTickCount());
	}

	if (0 == msg->GetSendBufferSize())
	{
		msg->SetReturnValue(SASPOLL_INCOMPLETE_DATA);
		msg->SetResponseBuffer(msg->GetResponseBuffer(), 0);
	}
	else
	{
        if (m_sas != nullptr)
			m_sas->SendToSAS(msg, retryCount);
        else
            LogString(ERROR_LOG, "CSASProtocol::TryToSendMessageToSAS : CSASInterface is null");
	}
}

void CSASProtocol::PerformScheduledTask(SAS_SCHEDULED_TASK taskID)
{
	try
	{
		// If meter capture needs to be done, do it here
		if (m_startSnapshot)
		{
			DoMeterSnapshot();
			m_startSnapshot = false;
			m_snapshotAllGames = false;
		}

		switch (taskID)
		{
		case SST_SEND_SUBGAME_METERS:
			SendSubGameMeters();
			break;
		case SST_DO_GENERAL_POLL:
			DoGeneralPoll();
			break;
		case SST_GET_HIGH_PRIORITY_METERS:
			GetHighPriorityMeters();
			break;
		case SST_GET_PBT_METERS:
			GetPBTMeters();
			break;
		case SST_GET_BILL_METERS:
			GetBillMeters();
			break;
		case SST_GET_TICKET_METERS:
			if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx && m_sasState.GetEGMConfig().GetEGMProtocol() != NO_EGM_PROTOCOL)
			{
				GetTicketMeters(false);
			}
			break;
		case SST_GET_MEDIUM_PRIORITY_GAME_METERS:
			GetMediumPriorityMeters();
			break;
		case SST_ENABLE_DISABLE_EGM_AND_FEATURES:
			EnableDisableEGM();
			EnableDisableCashoutToHost();
			break;
		case SST_SET_GAME_DELAY_UPDATE_CURRENT_GAME_DENOM:
			SetGameDelayAndUpdateCurrentGameDenom();
			break;
		case SST_SEND_DISABLE_REAL_TIME_EVENTS:
			DisableRealTimeEvents();
			break;
		case SST_DETERMINE_CAPS_OF_EGM:
			GetEGMCapabilities();
			break;
		case SST_SET_TIME:
			SetGameTime();              // AKA SetTime
			break;
		case SST_SET_TICKET_DATA:
			GetTicketValidationData(false);   // AKA SetTicketData
			break;
		case SST_GET_CURRENT_GAME_DENOM:
			SendCurrentSelectedGameToInternals(false);
			SendGameSessionIdIfRequiredToInternals();
			break;
		default:
			LogString(ERROR_LOG, "CSASProtocol::PerformScheduledTask unknown taskID=%d", (int)taskID);
			break;
		}
	}
	catch (...)
	{
	}
}

WORD CSASProtocol::GetMeters(CMeters& meters, WORD gameNumber, bool bInSnapshot, bool tryOnce)
{
	m_meterMutex.lock();

	CMeterStepdown *stepdown = gameNumber == EGM_GAME_NUMBER ? &m_mainGameMeterStepdown : &m_subGameMeterStepdown;
	CSASMeterCollection collect(m_sas, &meters, gameNumber, m_denomMult, bInSnapshot, tryOnce, stepdown, m_sasState.GetEGMConfig().GetEGMType() == IGTS2000);
	int acquiredMeterCount = collect.CollectMeters([this](CSASPollMessage *msg, int priority) {
		//if we've been here a while, check in with the watchdog
		m_SASGetMetersCount++;
		if (m_SASGetMetersCount > SAS_GET_METERS_CHECKIN_COUNT)
		{
			CWatchDog::Instance().Checkin();
			m_SASGetMetersCount = 0;
		}

		// Ananymous function to send to sas
		m_sasPollQueue->Add(msg, priority);
		msg->WaitForResponse();

		return;
	},
	[this](CInternalMessage *msg, int priority) {
		// Anonymous function to send to internals
		SendMessageToInternals(msg, priority);
		return;
	});

	//SPS tracker # 26399
	//if we did not find any more stepdowns for wms games reset the stepdowns
	// Before refactoring, acquiredMetercount was always 0 unless this is a snapshot.
	// I don't think that was the intended behavior. Now updating all the time.
	if (acquiredMeterCount == 0 && EGM_GAME_NUMBER != gameNumber && m_sasState.GetAllowStepdownResetForWMS())
	{
		LogString(ERROR_LOG, "SASProtocol: Reseting stepdown for WMS Sub Game.  Game Number %d", gameNumber);
		m_subGameMeterStepdown.ResetAllStepdowns();

		if (m_sas->GetMajorVersion() == SASVERSION5)
		{
			LogString(ERROR_LOG, "SASProtocol: Deactivating SAS 5x stepdown for WMS Sub Game. Game Number %d", gameNumber);
			m_subGameMeterStepdown.DeactivateForSAS5x();
		}
		else if (m_sasState.GetEGMConfig().GetEGMProtocol() == IGT_SAS_3xx && m_sasState.GetEGMConfig().GetEGMProtocol() != NO_EGM_PROTOCOL)
		{
			m_subGameMeterStepdown.DeactivateForSAS3x();
		}
	}

	m_meterMutex.unlock();

	return acquiredMeterCount;
}

void CSASProtocol::GetHighPriorityMeters(void)
{
	CMeters *meters = GetHighPriorityMetersAll();
	if (meters != nullptr)
	{
		GetHighPriorityMetersTickets(meters);
		GetHighPriorityMetersPbt(meters);
		SendHighPriorityMetersResults(meters);
	}

	// If calculating individual bills from total bills then get the total bills value frequently.
	if (!m_billMeterSuccess)
	{
		GetBillMeter();
	}
}

CMeters* CSASProtocol::GetNewHighPriorityMetersList()
{
	// Get these meters necessary for calculating 'no points for promo'
	// gCI was moved into this call to ensure that all meters used to calculate promo play are made in
	// a single call

	CMeters *meters = new CMeters(MEMORY_NONE);
	meters->SetMeter(gCI); // Coin in
	meters->SetMeter(mCTktPV); // Promo Ticket In
	meters->SetMeter(mCPromoOutVal); // Promo Ticket Out
	meters->SetMeter(gCO); // Coin out
	meters->SetMeter(mCrd); // Credit Meter
	meters->SetMeter(gJP); // Jackpot
	meters->SetMeter(gGS); // Games played
	meters->SetMeter(mBonusMach); // Machine-paid bonus for final bonus award meters.
	meters->SetMeter(mBonusAtt); // Machine-paid bonus for final bonus award meters.

	// don't waste time trying to get these for SAS 3/4
	if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx)
	{
		meters->SetMeter(mPbAR2G); // PBT NCEP In
		meters->SetMeter(mPbAR2H); // PBT NCEP Out
		meters->SetMeter(gBW); //Legacy Bonus meter
		meters->SetMeter(mNcepCredits); // NCEP Credits on game
		meters->SetMeter(mRstrPlayd); // NCEP Played
	}
	return meters;
}

// Do not send meters gathered before a general poll to Internals after performing the general poll, as this can deliver
// meters out of order to Internals, causing meter deltas (last coins won, etc.) to be calculated incorrectly.
CMeters* CSASProtocol::GetHighPriorityMetersAll()
{
	// sas_34 is to protect against the possibility that config is changed from SAS 3/4 to SAS 5 or 6 after the meters collection object is populated (which crashes)
	bool sas_34 = m_sasState.GetEGMConfig().GetEGMProtocol() == IGT_SAS_3xx;
	CMeters *meters = GetNewHighPriorityMetersList();
	GetMeters(*meters, EGM_GAME_NUMBER, false, true);

	// Try to get two consecutive tries where all meters involved in No Point For Promo calculations did not change.
	bool metersChanged = true;
	short numberToTry = HIGH_PRIORITY_CHANGED_METER_RETRIES + 1; // Plus one for initial try to determine if meters changed.
																					 // only try this numberToTry times, after that let meters go through
	while (metersChanged && numberToTry--)
	{
		// Do general poll to take up time to allow time for aristocrat's asynch meter updates to occur
		DWORD start = CUtilities::GetTickCount();
		do
		{
			DoGeneralPoll();
		}
		while (CUtilities::GetTickCount() - start < HIGHPRIOIRTYMETERDELAY);

		CMeters *metersAgain = GetNewHighPriorityMetersList();
		GetMeters(*metersAgain, EGM_GAME_NUMBER, false, true);

		if (meters->GetMeterByNumber(gCI)->GetValue() == metersAgain->GetMeterByNumber(gCI)->GetValue() &&
			 meters->GetMeterByNumber(mCTktPV)->GetValue() == metersAgain->GetMeterByNumber(mCTktPV)->GetValue() &&
			 meters->GetMeterByNumber(mCPromoOutVal)->GetValue() == metersAgain->GetMeterByNumber(mCPromoOutVal)->GetValue() &&
			 ((sas_34 || m_sasState.GetEGMConfig().GetEGMProtocol() == IGT_SAS_3xx) // This looks weird but need to guarantee that it wasn't SAS 3/4 at start of method and still isn't SAS 3/4
				 || ( // if SAS 3/4 don't check next 4 values
																								 meters->GetMeterByNumber(mPbAR2G)->GetValue() == metersAgain->GetMeterByNumber(mPbAR2G)->GetValue() &&
																								 meters->GetMeterByNumber(mPbAR2H)->GetValue() == metersAgain->GetMeterByNumber(mPbAR2H)->GetValue() &&
																								 meters->GetMeterByNumber(mNcepCredits)->GetValue() == metersAgain->GetMeterByNumber(mNcepCredits)->GetValue() &&
																								 meters->GetMeterByNumber(mRstrPlayd)->GetValue() == metersAgain->GetMeterByNumber(mRstrPlayd)->GetValue())))
		{
			metersChanged = false;
		}

		delete meters;
		meters = metersAgain;
	}

	if (numberToTry == 0 && metersChanged)
	{
		// Meters never stabalized give up on this attempt
		delete meters;
		meters = nullptr;
	}

	return meters;
}

void CSASProtocol::GetHighPriorityMetersTickets(CMeters *meters)
{
	if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx && m_sasState.GetEGMConfig().GetEGMProtocol() != NO_EGM_PROTOCOL)
	{
		// If we have gotten this far and ticket meters are still invalid, use long poll 50's to get them
		CLongPoll50Message *lp50 = NULL;
		CMeters *lp50Meters = NULL;
		if (meters->GetMeterByNumber(mCPromoOutVal)->GetValue() == INVALID_METER_VALUE)
		{
			lp50Meters = new CMeters(MEMORY_NONE);
			lp50Meters->SetMeter(mCPromoOutVal);
			lp50 = new CLongPoll50Message(m_gameAddress, lp50Meters);
			lp50->Fill();
			m_sasPollQueue->Add(lp50, QUEUE_PRIORITY_NORMAL);
			lp50->WaitForResponse();
			meters->GetMeterByNumber(mCPromoOutVal)->SetValue(lp50Meters->GetMeterByNumber(mCPromoOutVal)->GetValue());
			delete lp50Meters;
			delete lp50;
		}
		if (meters->GetMeterByNumber(mCTktPV)->GetValue() == INVALID_METER_VALUE)
		{
			lp50Meters = new CMeters(MEMORY_NONE);
			lp50Meters->SetMeter(mCTktPV);
			lp50 = new CLongPoll50Message(m_gameAddress, lp50Meters);
			lp50->Fill();
			m_sasPollQueue->Add(lp50, QUEUE_PRIORITY_NORMAL);
			lp50->WaitForResponse();
			meters->GetMeterByNumber(mCTktPV)->SetValue(lp50Meters->GetMeterByNumber(mCTktPV)->GetValue());
			delete lp50Meters;
			delete lp50;
		}
	}
}

void CSASProtocol::GetHighPriorityMetersPbt(CMeters *meters)
{
	if (m_sasState.GetEGMConfig().GetEGMProtocol() != IGT_SAS_3xx && m_sasState.GetEGMConfig().GetEGMProtocol() != NO_EGM_PROTOCOL)
	{
		// SAS 5 games may not have a way to get mPbAR2H which is required for NPFP calculations
		// If SAS 5 game and mPbAR2H is invalid, default to 0.
		if (m_sas->GetMajorVersion() <= SASVERSION5 && meters->GetMeterByNumber(mPbAR2H)->GetValue() == INVALID_METER_VALUE)
		{
			meters->GetMeterByNumber(mPbAR2H)->SetValue(0);
		}
	}
}

void CSASProtocol::SendHighPriorityMetersResults(CMeters *meters)
{
	CInternalMessage *msg = NULL;
	LogString(NOPTS_LOG, "High Priority Meter Values*******************");
	LogString(NOPTS_LOG, "meter #: %d, value: %lld", gCI, (long long)(meters->GetMeterByNumber(gCI) != NULL ? meters->GetMeterByNumber(gCI)->GetValue() : -1));
	LogString(NOPTS_LOG, "meter #: %d, value: %lld", mCPromoOutVal, (long long)(meters->GetMeterByNumber(mCPromoOutVal) != NULL ? meters->GetMeterByNumber(mCPromoOutVal)->GetValue() : -1));
	LogString(NOPTS_LOG, "meter #: %d, value: %lld", mPbAR2G, (long long)(meters->GetMeterByNumber(mPbAR2G) != NULL ? meters->GetMeterByNumber(mPbAR2G)->GetValue() : -1));
	LogString(NOPTS_LOG, "meter #: %d, value: %lld", mCTktPV, (long long)(meters->GetMeterByNumber(mCTktPV) != NULL ? meters->GetMeterByNumber(mCTktPV)->GetValue() : -1));
	LogString(NOPTS_LOG, "meter #: %d, value: %lld", mPbAR2H, (long long)(meters->GetMeterByNumber(mPbAR2H) != NULL ? meters->GetMeterByNumber(mPbAR2H)->GetValue() : -1));
	LogString(NOPTS_LOG, "meter #: %d, value: %lld", mNcepCredits, (long long)(meters->GetMeterByNumber(mNcepCredits) != NULL ? meters->GetMeterByNumber(mNcepCredits)->GetValue() : -1));

	msg = new CInternalMessage(INT_MSG_SET_METERS, meters, EGM_GAME_NUMBER);

	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CSASProtocol::GetMediumPriorityMeters(void)
{
	CMeters *meters = new CMeters(MEMORY_NONE);

	// Add the meters we need for medium priority meters
	meters->SetMeter(gGW);    // Games won
	meters->SetMeter(mTotalDrop); // Total Drop
	meters->SetMeter(mHCC);   // Hand-paid canceled credits
	meters->SetMeter(gCC);    // Total canceled credits
	meters->SetMeter(mBD);    // Bill drop credits
	meters->SetMeter(mTHP);   // Total Hand Pay
	meters->SetMeter(mActualCD);  // Actual coin drop
	meters->SetMeter(mTValueCI);  // Physical Coin In
	meters->SetMeter(mTValueCO);  // Physical Coin Out
	meters->SetMeter(mGameWTPP);  // Weighted Theoretical Payback Percentage
	meters->SetMeter(mCD);    // Coin drop
	meters->SetMeter(mTCi);   // True Coin In
	meters->SetMeter(mTCo);   // True Coin Out
	meters->SetMeter(mURstrPlayd);    // NonRestricted Played
	meters->SetMeter(mPrgPayAtt); //
	meters->SetMeter(mPrgPayMac); //

	GetMeters(*meters, EGM_GAME_NUMBER, false, true);
	if (INVALID_DENOM == m_denomMult && 0 == meters->GetMeterByNumber(mBD)->GetValue())
	{
		// Set the mBD meter back to INVALID_METER_VALUE, as the value is untrustworthy.
		CMeter *bdMeter(meters->GetMeterByNumber(mBD));
		if (bdMeter != nullptr)
		{
			bdMeter->SetValue(INVALID_METER_VALUE);
		}
		GetGameDenomInfo(); // A valid game denom is critical for the fallback to long poll 20 for mBD.
	}

	CInternalMessage *msg = new CInternalMessage(INT_MSG_SET_METERS, meters, EGM_GAME_NUMBER);

	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);

	// mActualSlotDr moved into a seperate call so that smaller meters from long poll 1c can't mess up other meters
	meters = new CMeters(MEMORY_NONE);
	meters->SetMeter(mActualSlotDr);  // Slot Door
	GetMeters(*meters, EGM_GAME_NUMBER, false, true);
	msg = new CInternalMessage(INT_MSG_SET_METERS, meters, EGM_GAME_NUMBER);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CSASProtocol::GetBillMeters(int billMeterNumber)
{
	CMeters *meters = new CMeters(MEMORY_NONE);

	meters->SetBillMeters(billMeterNumber);

	GetMeters(*meters, EGM_GAME_NUMBER, false, true);

	if (INVALID_DENOM == m_denomMult && 0 == meters->GetMeterByNumber(mBD)->GetValue())
	{
		// Set the mBD meter back to INVALID_METER_VALUE, as the value is untrustworthy.
		CMeter* bdMeter(meters->GetMeterByNumber(mBD));
		if (bdMeter != nullptr)
		{
			bdMeter->SetValue(INVALID_METER_VALUE);
		}
		GetGameDenomInfo(); // A valid game denom is critical for the fallback to long poll 20 for mBD.
	}

	// If there was a comm issue at startup we may bounce between individual bill meter and calculated bill meter.
	// Prevent this by resetting flag if individual bill meter successfully returned
	if (!m_billMeterSuccess && meters->GetMeterByNumber(mD1)->GetValue() != INVALID_METER_VALUE)
	{
		m_billMeterSuccess = true;
	}

	// Some of the denominations we set to zero if we could not get them
	for (int mi = 0; mi < meters->GetMeterCount(); mi++)
	{
		CMeter *meter = meters->GetMeterByIndex(mi);
		if (meter->GetValue() == INVALID_METER_VALUE)
		{
			switch (meter->GetMeterNumber())
			{
			case mD2:
			case mD200:
			case mD500:
			case mD1000:
				meter->SetValue(0);
				break;
			}
		}
	}

	CInternalMessage *msg = new CInternalMessage(INT_MSG_SET_METERS, meters, EGM_GAME_NUMBER);

	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CSASProtocol::ProcessMeterSnapshot(CInternalMessage *msg)
{	
	SnapshotType snapShotType((SnapshotType)msg->GetWordData());
	if (MachineRollover == snapShotType)
	{
		ProcessRolloverMeterSnapshot();
	}
	else
	{
		m_startSnapshot = true;
		m_sasState.SetSnapshotType(snapShotType);
		m_snapshotAllGames = (SnapshotType)msg->GetWordData(true);
	}
}

void CSASProtocol::ProcessRolloverMeterSnapshot()
{
	// Tell internals that snapshot is starting
	SendMessageToInternals(new CInternalMessage(INT_MSG_START_CAPTURING_METERS, (WORD)MachineRollover), QUEUE_PRIORITY_HIGHEST);

	// Define the meters we are going to try to get
	CMeters* meters = new CMeters(MEMORY_NONE);
	for (byte meterNum = mNone + 1; meterNum < mMeterLast; meterNum++)
	{
		meters->SetMeter(meterNum);
	}

	// Now get these meters. The last parameter indicates that we are sending the progress to internals
	WORD acquiredMeterCount = GetMeters(*meters, EGM_GAME_NUMBER, true);

	// See if we are going to cancel the snapshot.
	WORD bCancel(acquiredMeterCount == 0);

	if (!bCancel)
	{
		// Send the egm meters.
		SendMessageToInternals(new CInternalMessage(INT_MSG_SET_METERS, meters, EGM_GAME_NUMBER, true), QUEUE_PRIORITY_HIGHEST);

		int currentGameNumber(EGM_GAME_NUMBER);
		if (GetSelectedGameNumber(currentGameNumber))
		{
			if (EGM_GAME_NUMBER != currentGameNumber)
			{
				SendSubGameMeters(currentGameNumber, false, true);
			}
		}
	}

	// Tell internals we are done with getting snapshot.
	SendMessageToInternals(new CInternalMessage(INT_MSG_END_CAPTURING_METERS, (WORD)(bCancel)), QUEUE_PRIORITY_HIGHEST);
}

void CSASProtocol::DoMeterSnapshot()
{
	// Tell internals that snapshot is starting
	CInternalMessage *msg = new CInternalMessage(INT_MSG_START_CAPTURING_METERS);
	msg->SetData((WORD)m_sasState.GetSnapshotType());
	SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);

	// Define the meters we are going to try to get
	CMeters *meters = new CMeters(MEMORY_NONE);
	for (byte meterNum = mNone + 1; meterNum < mMeterLast; meterNum++)
	{
		meters->SetMeter(meterNum);
	}

	// Now get these meters. The last parameter indicates that we are sending the progress to internals
	WORD acquiredMeterCount = GetMeters(*meters, EGM_GAME_NUMBER, true);

	// See if we are going to cancel the snapshot.
	bool bCancel = false;
	if (acquiredMeterCount == 0) bCancel = true;

	// This takes a while. Check into the watchdog
	CWatchDog::Instance().Checkin();

	if (!bCancel)
	{
		// Send the egm meters.
		SendMessageToInternals(new CInternalMessage(INT_MSG_SET_METERS, meters, EGM_GAME_NUMBER, true), QUEUE_PRIORITY_HIGHEST);


		// If we previously were in a the first pass, we will need to send info
		//  on every getting of the subgames
		bool bPreviouslyFirstPass = m_gameList.IsFirstPass();

		// Set the game list back to it's "first pass" state.
		// This will make the game list go in order for one pass
		m_gameList.SetFirstPass();

		// Tracker #20956 - don't get subgame meters for all snapshot types
		if (m_sasState.GetSnapshotType() != MpCashDoor && m_sasState.GetSnapshotType() != MpDropDoor && m_sasState.GetSnapshotType() != MpCurrencyCard)
		{
			DWORD lastGameNumber(EGM_GAME_NUMBER);
			WORD subGameCount(m_gameList.GetCount());
			if (m_snapshotAllGames)
			{
				LogString(GAMES, "SnapshotType=%u All Games subGameCount=%u", m_sasState.GetSnapshotType(), subGameCount);

				// Get the next game number we have to get meters for
				bool sendInfo = false;
				WORD gameNumber = m_gameList.GetNextGame(sendInfo);

				// Cycle thru one pass of the egm game.
				WORD subGameProgress = 0;
				while (gameNumber != EGM_GAME_NUMBER && m_gameList.IsFirstPass())
				{
					LogString(GAMES, "DoMeterSnapshot for gameNumber %u", gameNumber);
					SendSubGameMeters(gameNumber, bPreviouslyFirstPass, true);

					CInternalMessage *msg = new CInternalMessage(INT_MSG_SNAPSHOT_PROGRESS, subGameProgress++, subGameCount);
					msg->SetData((DWORD)gameNumber);
					SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);

					gameNumber = m_gameList.GetNextGame(sendInfo);

					// This takes a while. Check into the watchdog
					CWatchDog::Instance().Checkin();

					CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);

					lastGameNumber = gameNumber;
				}
			}
			else
			{
				LogString(GAMES, "SnapshotType=%u Enabled and non-zero coinin subgames", m_sasState.GetSnapshotType());
				// Cycle thru all the enabled games and previously enabled games
				vector<DWORD> subgamesToMeterSnapShot(m_gameList.GetEnabledGames());
				subgamesToMeterSnapShot.insert(subgamesToMeterSnapShot.end(), m_gameList.GetPreviouslyEnabledGames().begin(), m_gameList.GetPreviouslyEnabledGames().end());
				subGameCount = (WORD)(subgamesToMeterSnapShot.size());
				WORD subGameProgress(0);
				for (const DWORD gameNumber : subgamesToMeterSnapShot)
				{
					if (gameNumber != EGM_GAME_NUMBER)
					{
						LogString(GAMES, "DoMeterSnapshot for gameNumber %u", gameNumber);
						SendSubGameMeters(gameNumber, bPreviouslyFirstPass, true);
						CInternalMessage *msg = new CInternalMessage(INT_MSG_SNAPSHOT_PROGRESS, subGameProgress++, subGameCount);
						msg->SetData((DWORD)gameNumber);
						SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);

						// This takes a while. Check into the watchdog
						CWatchDog::Instance().Checkin();

						CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
					}
					lastGameNumber = gameNumber;
				}
			}

			LogString(GAMES, "Meter Snapshot is finished subGameCount=%u lastGameNumber=%u", subGameCount, lastGameNumber);
			if (0 == subGameCount)
			{
				subGameCount = 1;
			}
			CInternalMessage *msg = new CInternalMessage(INT_MSG_SNAPSHOT_PROGRESS, subGameCount, subGameCount);
			msg->SetData(lastGameNumber);
			SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);
		}
	}
	else
	{
		delete meters;
	}

	// Tell internals we are done with getting snapshot.
	CInternalMessage *endMsg = new CInternalMessage(INT_MSG_END_CAPTURING_METERS);
	endMsg->SetData((WORD)(bCancel ? 1 : 0));
	SendMessageToInternals(endMsg, QUEUE_PRIORITY_HIGHEST);
}

// AKA SetTicketData.

/// <summary>
/// Gets and conditionally sets the ticket seed values. If game machine id is 0 ticket seed will be reset regardless of bAlwaysUpdate value.
/// </summary>
/// <param name="bAlwaysUpdate">Specifies if ticket seed should be always reset</param>
/// <returns></returns>
void CSASProtocol::GetTicketValidationData(bool bAlwaysUpdate)
{
	// reseed RNG
	srand(CUtilities::GetTickCount());

	if (m_sasState.GetSlotmastID() != INVALID_SLOT_MAST_ID && m_sasState.GetTicketingOutEnabled())
	{

		// Only do this if the time is >= 2001
		if (CUtilities::GetCurrentTime() >= CUtilities::GetTimeFromFields(2001, 1, 1, 0, 0, 0) &&
			 m_sas->GetMajorVersion() > SASVERSION3 // not supported in sas 3.x
			)
		{
			CLongPoll4CMessage *lp4c = new CLongPoll4CMessage(m_gameAddress);

			lp4c->SetMachineValidationID(0); // A validation ID of 0 indicates we are getting the validation ID
			lp4c->SetValidationSequenceNumber(0);
			lp4c->Fill();
			// Add to SAS poll queue at high priority
			// This is high priority because we might be getting this because or ticket data is out of sync.
			m_sasPollQueue->Add(lp4c, QUEUE_PRIORITY_HIGH);
			lp4c->WaitForResponse();
			DWORD machineID = m_ticketSeed->GetMachineID(m_sasState.GetSlotmastID(), m_sasState.GetCasinoNumber());
			if (lp4c->GetMachineValidationID() != machineID || bAlwaysUpdate)
			{
				// game reset, reset ticket seed info
				byte sequenceNumber[sizeof(DWORD)]; // 4th byte unused, treat as DWORD for parameter

				// Init squence number to value returend by 4c poll.
				DWORD *dwSequence = (DWORD *)sequenceNumber;
				*dwSequence = lp4c->GetValidationSequenceNumber();

				m_ticketSeed->IncAndRandSequence(sequenceNumber, lp4c->GetMachineValidationID() != machineID);

				// set new values
				CLongPoll4CMessage *lp4cUpdate = new CLongPoll4CMessage(m_gameAddress);

				DWORD dwSeqNumber = 0;
				memcpy(&dwSeqNumber, sequenceNumber, SEQUENCENUMBERSIZE);
				lp4cUpdate->SetMachineValidationID(machineID);
				lp4cUpdate->SetValidationSequenceNumber(dwSeqNumber);
				lp4cUpdate->Fill();

				// Add to SAS poll queue at high priority
				// This is high priority because we might be getting this because or ticket data is out of sync.
				m_sasPollQueue->Add(lp4cUpdate, QUEUE_PRIORITY_HIGH);
				lp4cUpdate->WaitForResponse();

				delete lp4cUpdate;
			}
			delete lp4c;

		}
	}
}

/// <summary>
/// Squeeze an update for current game number and player denomination into the polling cycle,
/// trying to minimize the impact (delay) to the polling cycle at the same or lower priority.
/// </summary>
void CSASProtocol::SetGameDelayAndUpdateCurrentGameDenom()
{
	SetGameDelay();
	CheckCurrentGameDenom(false);
}

void CSASProtocol::SetGameDelay(void)
{
	if (m_sasState.GetAllowableGameDelayPoll() && !m_bGameDelaySent)
	{
		CLongPoll2EMessage lp2e(m_gameAddress);

		lp2e.SetGameDelay(SAS_GAME_DELAY);

		lp2e.Fill();

		// Add to SAS poll queue at high priority
		m_sasPollQueue->Add(&lp2e, QUEUE_PRIORITY_HIGHEST);

		// Wait for a response from SAS
		lp2e.WaitForResponse();

		//Tracker 21388
		m_bGameDelaySent = true; // cause gamedelay to be sent one time only
	}
}

void CSASProtocol::DisableRealTimeEvents(void)
{
	CLongPoll0EMessage lp0e(m_gameAddress);
	lp0e.EnableRealTimeReporting(false);
	lp0e.Fill();

	// Add to SAS poll queue at high priority
	m_sasPollQueue->Add(&lp0e, QUEUE_PRIORITY_HIGHEST);

	// Wait for a response from SAS
	lp0e.WaitForResponse();
}

void CSASProtocol::EnableDisableEGM()
{
#ifdef ALLOW_ENABLE_DISABLE

	if (m_sasState.GetAllowableEnableDisable())
	{
		CSASPollMessage *pollMsg(nullptr);
		bool enableGame(m_sasState.GetEnableEGM());
		if (enableGame)
		{
			pollMsg = new CLongPoll02Message(m_gameAddress);
			m_lastEGMEnableTickCount = CUtilities::GetTickCount();
		}
		else
		{
			pollMsg = new CLongPoll01Message(m_gameAddress);
		}

		// Add to SAS poll queue at high priority
		pollMsg->Fill();
		m_sasPollQueue->Add(pollMsg, QUEUE_PRIORITY_HIGHEST);

		// Wait for a response from SAS
		pollMsg->WaitForResponse();
		bool ackedSuccessfully(pollMsg->GetReturnValue() == SASPOLL_SUCCESS);
		LogString(SASGENERAL, "EGM %s complete status=%d", enableGame ? "Enable" : "Disable", (int)(ackedSuccessfully));
		TransactionType transactionType(NONE_SPECIFIED);
		if (m_sasState.CheckIfTransactionNeedsToBeSentForLockUnlockEgm(enableGame, ackedSuccessfully, transactionType))
		{
			LogString(SASGENERAL, "EnableDisableEGM transaction=%d", (int)transactionType);
			SendMessageToInternals(new CInternalMessage(INT_MSG_EGM_LOCK_UNLOCK_STATUS, (WORD)transactionType), QUEUE_PRIORITY_NORMAL);
		}	

		delete pollMsg;
	}

#endif
}

// Below function gets called after every 25 secs approx. from SAS scheduled tasks
void CSASProtocol::EnableDisableCashoutToHost()
{
	if (!m_bIsAFTSupported || 
		(m_sasState.GetCashoutToHostEnabledOnEGM() == m_sasState.GetEnableCashoutToHost()) ||
		(m_sasState.GetCashoutToHostConfigTries() >= CASHOUT_TO_HOST_CONFIG_MAX_TRIES))
	{
		return;
	}

	LogString(SASGENERAL, "[%s] CashoutToHostEnabledOnEgm %d GetEnableCashoutToHost %d NumTries: %d",
		__FUNCTION__, m_sasState.GetCashoutToHostEnabledOnEGM(),
		m_sasState.GetEnableCashoutToHost(), m_sasState.GetCashoutToHostConfigTries());

	CLongPoll72Message lp72(m_gameAddress);
	lp72.ClearStatusQuery();
	lp72.SetCashableAmount(0);
	lp72.SetNonrestrictedAmount(0);
	lp72.SetRestrictedAmount(0);
	lp72.SetTransferType(SAS_TRANSFER_TYPE_CASH_TO_GAME);
	lp72.SetTransferFlag(TRANSFER_FLAG_ACCEPT_TRANSFER_ONLY_IF_LOCKED, false);
	lp72.SetTransferFlag(TRANSFER_FLAG_HOST_CASHOUT_ENABLE_CONTROL, true);
	lp72.SetTransferFlag(TRANSFER_FLAG_HOST_CASHOUT_ENABLE, m_sasState.GetEnableCashoutToHost());
	lp72.SetTransferFlag(TRANSFER_FLAG_HOST_CASHOUT_MODE, false); // cashout mode soft
	lp72.SetAssetNumber(m_assetNumber);

	// Generate transactionId = ATIGENERIC-XXX (where XXX is random number between 0 and 65535)
	string transactionId(CUtilities::GetPrefixIdString(ATIGENERIC_TRANSACTION_ID_PREFIX, (LONG64)rand() % 0xffff, MAX_PBID_LENGTH));
	lp72.SetTransactionID((byte *)transactionId.c_str(), transactionId.size());

	LogString(SASGENERAL, "[%s] %s cashout to host on EGM", __FUNCTION__,
		m_sasState.GetEnableCashoutToHost() ? "Enabling" : "Disabling");

	// Add to SAS poll queue at high priority
	lp72.Fill();
	m_sasPollQueue->Add(&lp72, QUEUE_PRIORITY_HIGHEST);

	// Wait for a response from SAS
	lp72.WaitForResponse();

	if (SASPOLL_SUCCESS == lp72.GetReturnValue())
	{
		if (lp72.m_transferStatus == 0x00) // successful
		{
			LogString(SASGENERAL, "[%s] Enabling/disabling cashout to host succeeded. "
				"LP72 TransferFlags: 0x%x", __FUNCTION__, lp72.m_transferFlags);
			m_sasState.SetCashoutToHostEnabledOnEGM(lp72.GetTransferFlag(TRANSFER_FLAG_HOST_CASHOUT_ENABLE));

			// Send host cashout status (first 3 bits of transfer flags) to internals for web diagnostics display
			byte hostCashoutStatus = (lp72.m_transferFlags & 0x7);
			SendMessageToInternals(new CInternalMessage(INT_MSG_CASHOUT_TO_HOST_STATUS,
				(WORD)hostCashoutStatus), QUEUE_PRIORITY_NORMAL);
		}
		else if (lp72.m_transferStatus == TRANSFER_STATUS_PENDING)
		{
			LogString(SASGENERAL, "[%s] Enabling/disabling cashout to host accepted. TransferStatus: 0x%x.",
				__FUNCTION__, lp72.m_transferStatus);
		}
		else
		{
			LogString(SASGENERAL, "[%s] Enabling/disabling cashout to host returned error. TransferStatus: 0x%x.", 
				__FUNCTION__, lp72.m_transferStatus);

			// LP72 generated error, cashout to host configuration may or may not be changed (varies based on error
			// code generated and EGM differences). Query using LP74 and update the configuration.
			CLongPoll74Message lp74(m_gameAddress);
			lp74.SetLockCode(LOCK_CODE_QUERY_STATUS);
			lp74.SetLockTimeout(0);
			lp74.Fill();
			m_sasPollQueue->Add(&lp74, QUEUE_PRIORITY_HIGHEST);
			lp74.WaitForResponse();
			if (SASPOLL_SUCCESS == lp74.GetReturnValue())
			{
				bool isCthEnabled = lp74.ResponseCashoutToHostEnabled();
				LogString(SASGENERAL, "[%s] CashoutToHostEnabledOnEgm: %d HostCashoutStatus: 0x%x", 
					__FUNCTION__, isCthEnabled, lp74.m_hostCashoutStatus);
				m_sasState.SetCashoutToHostEnabledOnEGM(isCthEnabled);

				SendMessageToInternals(new CInternalMessage(INT_MSG_CASHOUT_TO_HOST_STATUS,
					(WORD)lp74.m_hostCashoutStatus), QUEUE_PRIORITY_NORMAL);

				// Update asset number also, this will fix the issue next time if asset number is changed.
				m_assetNumber = lp74.m_assetNumber;
			}
			else
			{
				LogString(SASGENERAL, "[%s] LP74 to query cashout to host status failed. LP74 returned: %d",
					__FUNCTION__, lp74.GetReturnValue());
			}
			m_sasState.SetCashoutToHostConfigTries(m_sasState.GetCashoutToHostConfigTries() + 1);
		}
	}
	else
	{
		LogString(SASGENERAL, "[%s] Enabling/disabling cashout to host failed/timeout. LP72 returned: %d.",
			__FUNCTION__, lp72.GetReturnValue());
	}
}

void CSASProtocol::CancelCashoutToHost(void)
{
	LogString(SASGENERAL, "[%s]", __FUNCTION__);

	if (!m_bIsAFTSupported)
	{
		LogString(SASGENERAL, "[%s] AFT not supported on EGM, returning.", __FUNCTION__);
		return;
	}

	// Frame LP72 request with 0 credits to get EGM out of cashout to host wait mode
	CLongPoll72Message lp72(m_gameAddress);
	lp72.ClearStatusQuery();
	lp72.SetCashableAmount(0);
	lp72.SetNonrestrictedAmount(0);
	lp72.SetRestrictedAmount(0);
	lp72.SetTransferType(SAS_TRANSFER_TYPE_CASH_TO_HOST);
	lp72.SetTransferFlag(TRANSFER_FLAG_ACCEPT_TRANSFER_ONLY_IF_LOCKED, false);
	lp72.SetAssetNumber(m_assetNumber);

	// Generate transactionId = ATIGENERIC-XXX (where XXX is random number between 0 and 65535)
	string transactionId(CUtilities::GetPrefixIdString(ATIGENERIC_TRANSACTION_ID_PREFIX, (LONG64)rand() % 0xffff, MAX_PBID_LENGTH));
	lp72.SetTransactionID((byte *)transactionId.c_str(), transactionId.size());

	// Add to SAS poll queue at high priority
	lp72.Fill();
	m_sasPollQueue->Add(&lp72, QUEUE_PRIORITY_HIGHEST);

	// Wait for a response from SAS
	lp72.WaitForResponse();

	if ((SASPOLL_SUCCESS == lp72.GetReturnValue()) && (TRANSFER_STATUS_PENDING >= lp72.m_transferStatus))
	{
		LogString(SASGENERAL, "[%s] LP72 to initiate canceling cashout to host accepted. TransferStatus: 0x%x.",
			__FUNCTION__, lp72.m_transferStatus);
	}
	else
	{
		LogString(SASGENERAL, "[%s] LP72 to initiate canceling cashout to host failed. LP72 returned: 0x%x, TransferStatus: 0x%x.",
			__FUNCTION__, lp72.GetReturnValue(), lp72.m_transferStatus);
	}
}

void CSASProtocol::SetGameTime(void)
{
	CLongPoll7FMessage lp7f(m_gameAddress);
	lp7f.Fill();

	// Add to SAS poll queue at high priority
	m_sasPollQueue->Add(&lp7f, QUEUE_PRIORITY_HIGHEST);

	// Wait for a response from SAS
	lp7f.WaitForResponse();


	SetGameDelay();
}

void CSASProtocol::SendTiltToInternals(TiltCode tiltCode)
{
	CInternalMessage *msg = new CInternalMessage(INT_MSG_SEND_TILT, (WORD)tiltCode);

	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}


void CSASProtocol::PopulateMeterStepdowns(void)
{

	// Populate main game meter stepdown map here
	m_mainGameMeterStepdown.AddMeterItem(mCD,	0x6F,	0x2F,	0x1C,	0x19);  // Coin Drop
																							  //m_mainGameMeterStepdown.AddMeterItem(mSDr,	0x1C,	0,		0,		0 );	// Slot Door
	m_mainGameMeterStepdown.AddMeterItem(mActualSlotDr,	0x1C,	0,		0,		0);    // Slot Door
	m_mainGameMeterStepdown.AddMeterItem(mD1,	0x6F,	0x2F,	0x1e,	0x31);      //{Ones}
	m_mainGameMeterStepdown.AddMeterItem(mD2,	0x6F,	0x2F,	0x32,	0);     //{Twos}
	m_mainGameMeterStepdown.AddMeterItem(mD5,	0x6F,	0x2F,	0x1e,	0x33);      //{Fives}
	m_mainGameMeterStepdown.AddMeterItem(mD10,	0x6F,	0x2F,	0x1e,	0x34);      //{Tens}
	m_mainGameMeterStepdown.AddMeterItem(mD20,	0x6F,	0x2F,	0x1e,	0x35);      //{Twenties}
	m_mainGameMeterStepdown.AddMeterItem(mD50,	0x6F,	0x2F,	0x1e,	0x36);      //{Fifties}
	m_mainGameMeterStepdown.AddMeterItem(mD100,	0x6F,	0x2F,	0x1e,	0x37);      //{Hundreds}
	m_mainGameMeterStepdown.AddMeterItem(mCrd,	0x6F,	0x2F,	0x1A,	0);    // Credit Meter
	m_mainGameMeterStepdown.AddMeterItem(mTCi,	0x2A,	0,		0,		0);    // True Coin In
	m_mainGameMeterStepdown.AddMeterItem(mTCo,	0x2B,	0,		0,		0);    // True Coin Out
	m_mainGameMeterStepdown.AddMeterItem(mBD,	0x6F,	0x2F,	0x46,	0x20); // Bill drop credits
	m_mainGameMeterStepdown.AddMeterItem(mTHP,	0x6F,	0x2F,	0,		0);    // Total Hand Pay
	m_mainGameMeterStepdown.AddMeterItem(mActualCD, 0x6F, 0x2F,	0,		0);    // Actual coin drop
	m_mainGameMeterStepdown.AddMeterItem(gCI,	0x6F,	0x2F,	0x1C,	0x19);  // Coin in
	m_mainGameMeterStepdown.AddMeterItem(gCO,	0x6F,	0x2F,	0x1C,	0x19);  // Coin out
	m_mainGameMeterStepdown.AddMeterItem(gGS,	0x6F,	0x2F,	0x1C,	0x19);  // Games played
	m_mainGameMeterStepdown.AddMeterItem(gGW,	0x6F,	0x2F,	0x1C,	0);    // Games won
	m_mainGameMeterStepdown.AddMeterItem(gCC,	0x6F,	0x2F,	0x10,	0);     // Total canceled credits
	m_mainGameMeterStepdown.AddMeterItem(gBW,	0x9A,	0,		0,		0);     // Bonus Won (Legacy Bonus meter)
	m_mainGameMeterStepdown.AddMeterItem(gJP,	0x6F,	0x2F,	0x1C,	0x19);  // Jackpot
	m_mainGameMeterStepdown.AddMeterItem(mHCC,	0x6F,	0x2F,	0x2D,	0);    // Hand-paid canceled credits
	m_mainGameMeterStepdown.AddMeterItem(mD200,	0x6F,	0x2F,	0x3A,	0);     //{Two Hundreds}
	m_mainGameMeterStepdown.AddMeterItem(mD500,	0x6F,	0x2F,	0x38,	0);     //{Five Hundreds}
	m_mainGameMeterStepdown.AddMeterItem(mD1000, 0x6F,	0x2F,	0x39,	0);     //{One Thousands}
	m_mainGameMeterStepdown.AddMeterItem(mTValueCI, 0x6F, 0x2F,	0,		0);    // Physical Coin In
	m_mainGameMeterStepdown.AddMeterItem(mTValueCO, 0x6F, 0x2F,	0,		0);    // Physical Coin Out
	m_mainGameMeterStepdown.AddMeterItem(mGameWTPP, 0x6F, 0x2F,	0,		0);    // Weighted Theoretical Payback Percentage
	m_mainGameMeterStepdown.AddMeterItem(mTotalDrop, 0x6F, 0x2F,	0x1C,	0);    // Total Drop
	m_mainGameMeterStepdown.AddMeterItem(mBonusMach,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mBonusAtt,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mPrgPayAtt,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mPrgPayMac,	0x6F,	0x2F,	0,	0);
	// Ticketing meters
	m_mainGameMeterStepdown.AddMeterItem(mTVal,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mTO,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mCPromoOutVal,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mCPromoOutCnt,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mHPCshRcptVal,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mHPCshRcptCnt,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mHPWinRcptCnt,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mHPWinRcptVal,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mCoupV,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mVchrTktDrop,	0x6F,	0x2F,	00,	0);
	m_mainGameMeterStepdown.AddMeterItem(mCoup,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mCTktPV,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mCTktPN,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mCTktCV,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mCTktCN,	0x6F,	0x2F,	0,	0);
	// PBT Meters
	m_mainGameMeterStepdown.AddMeterItem(mPbAR2G,	0x6F,	0x2F,	0x1d,	0);
	m_mainGameMeterStepdown.AddMeterItem(mPbAN2G,	0x6F,	0x2F,	0x1d,	0);
	m_mainGameMeterStepdown.AddMeterItem(mPbAC2H,	0x6F,	0x2F,	0x1d,	0);
	m_mainGameMeterStepdown.AddMeterItem(mPbAC2G,	0x6F,	0x2F,	0x1d,	0);
	m_mainGameMeterStepdown.AddMeterItem(mPbAR2H,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mPbAN2H,	0x6F,	0x2F,	0,	0);
	//m_mainGameMeterStepdown.AddMeterItem(mPtR,	0x1d,	0,	0,	0);
	//m_mainGameMeterStepdown.AddMeterItem(mPBn,	0x1d,	0,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mRstrPlayd,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mURstrPlayd,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mPbAD2G,	0x6F,	0x2F,	0,	0);
	m_mainGameMeterStepdown.AddMeterItem(mNcepCredits,	0x6F,	0x2F,	0x27,	0);


	// Populate sub game meter stepdown map here
	m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_GAMES_PLAYED,		0x6F,	0x2F,	0x52, 0);   // Coin in
	m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_COIN_IN,			0x6F,	0x2F,	0x52, 0);   // Games played
	m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_COIN_OUT,			0x6F,	0x2F,	0, 0);  // Games won
	m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_ATT_PAID_JACKPOT,	0x6F,	0x2F,	0x52, 0);   // Coin Out
	m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_GAMES_WON,		0x6F,	0x2F,	0, 0);  // Machine Pay Progressive payout
	m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_ATT_PAID_PROG,	0x6F,	0x2F,	0, 0);  // External Bonusing machine payout
	m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_ATT_PAID_EXT_BONUS, 0x6F,	0x2F,	0x52, 0);   // Jackpot
	m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_MACH_PAID_PROG,	0x6F,	0x2F,	0, 0);  // Attendant Pay Progressive payout
	m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_MACH_PAID_EXT_BONUS, 0x6F,	0x2F,	0, 0);  // External Bonusing Attendant payout
	m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_WTPP,				0x6F,	0x2F,	0, 0);  // Weighted Theoretical Payback Percentage
}

void CSASProtocol::Init()
{
	m_sas = GetSASInterface(this, m_scfToInternalsQueue, m_internalsToSCFQueue, m_operatingSystem);

	// seed random number generator
	srand(CUtilities::GetTickCount());

	// Set up random ticket sequence number starting point
	m_ticketSequenceNumber = rand();

	// initialize some ticketing fields for testing
	m_sasState.SetIgnoreTicketPrinted(true);

	// Set up the SAS message queue
	m_sasPollQueue = new CSASQueueList();

	// Start the send to EGM queue thread.
	m_sendToEGMThread = unique_ptr<std::thread>(new std::thread(SendToEGMThread, this));
	CWatchDog::Instance().RegisterThread(THREAD_SEND_TO_EGM, m_sendToEGMThread->get_id(), SEND_TO_EGM_THREAD_TIMEOUT, m_sendToEGMThread->native_handle());
}

void CSASProtocol::Start(void)
{
	// Start the scheduled tasks thread
	m_scheduleThread = unique_ptr<std::thread>(new std::thread(ScheduledSASTaskThread, this));
	CWatchDog::Instance().RegisterThread(THREAD_SCHEDULED_SAS_TASK, m_scheduleThread->get_id(), SAS_SCHEDULER_TIMEOUT, m_scheduleThread->native_handle());

	// Notify Internals that SAS Protocol is up and running
	CInternalMessage *msg = new CInternalMessage(INT_MSG_GCF_STARTED);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);

	CSystemProtocol::Start();

	m_sendToEGMThread->detach();
	m_scheduleThread->detach();
}

void CSASProtocol::Shutdown(void)
{
	// Set IsTimeToClose() == true and wait for the base class thread to shutdown.
	Stop();

	// First wait for the scheduler thread to shut down
	while (m_scheduleThread != NULL)
	{
		CUtilities::Sleep(SHUTDOWN_SCHEDULER_SLEEP_PERIOD);
		CWatchDog::Instance().Checkin();
	}

	// Now wait until the set to EGM thread closes
	m_bShutdownSendToEGMThread = true;
	while ((m_sendToEGMThread != NULL))
	{
		// Nudge the SendToEGM thread to read the shutting down flag.
		binsem& hEvent = m_sasPollQueue->GetAddedItemEventHandle();
		hEvent.signal(false);

		CUtilities::Sleep(SHUTDOWN_LOOP_SLEEP_PERIOD);
	}
}

void CSASProtocol::SendToEGMThreadClosed(void)
{
	m_sendToEGMThread = NULL;
}

void CSASProtocol::ScheduledSASTaskThreadClosed(void)
{
	m_scheduleThread = NULL;
}

static void SendToEGMThreadInner(CSASProtocol *sasProtocol, CQueueList *queueList, binsem& itemAddedEventHandle)
{
	CUtilities::ShowThreadId(_T(THREAD_SEND_TO_EGM));

	// This loop never ends
	while (!sasProtocol->AreWeShuttingDownSendToEGMThread())
	{
		// Wait for an item to be added to the queue
		itemAddedEventHandle.try_wait_for(EVENT_WAIT_TIMEOUT);
		CWatchDog::Instance().Checkin();

		// Remove the messages from the queue and give them to the SAS interface.
		//  until there are no more.
		CSASPollMessage *msg = (CSASPollMessage *)queueList->Remove();
		while (msg != NULL && !sasProtocol->AreWeShuttingDownSendToEGMThread())       //Tracker #25893
		{
			// Give this message to the SAS interface to process.
			sasProtocol->SendMessageToSAS(msg);

			// polling cycle.
			CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);

			// Get the next message (if any)
			msg = (CSASPollMessage *)queueList->Remove();
		}
	}
}

static UINT AFX_CDECL SendToEGMThread(PVOID threadParam)
{
	CSASProtocol *sasProtocol = (CSASProtocol *)threadParam;

	CUtilities::SetThreadName(std::this_thread::get_id(), THREAD_SEND_TO_EGM);

	// Get the queue of messages from the GCF
	CQueueList *queueList = sasProtocol->GetSASPollQueue();

	// Get the handle to the event that gets called when an item is put in the queue
	binsem& itemAddedEventHandle = queueList->GetAddedItemEventHandle();

	try
	{
		SendToEGMThreadInner(sasProtocol, queueList, itemAddedEventHandle);
	}
	catch (...)
	{
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_SEND_TO_EGM);
	}

	sasProtocol->SendToEGMThreadClosed();
	CWatchDog::Instance().DeregisterThread();

	return 0;
}


static void ScheduledSASTaskThreadInner2(CSASProtocol *sasProtocol, CSASScheduler *scheduler, DWORD statsTime)
{
	CUtilities::ShowThreadId(_T(THREAD_SCHEDULED_SAS_TASK));

	// This loop never ends
	while (sasProtocol != NULL && !sasProtocol->IsTimeToClose())
	{
		SAS_SCHEDULED_TASK taskID = scheduler->GetNextScheduledTask();
		CWatchDog::Instance().Checkin();

		sasProtocol->PerformScheduledTask(taskID);
		CWatchDog::Instance().Checkin();

		DWORD curTime = CUtilities::GetTickCount();
		if (curTime >= statsTime + 60000 || curTime < statsTime)
		{
			LogString(SASGENERAL, "Last Highest priority time interval(%d)", scheduler->GetLastIntervalForPriority(SAS_PRIORITY_HIGHEST));
			LogString(SASGENERAL, "Last High priority time interval(%d)", scheduler->GetLastIntervalForPriority(SAS_PRIORITY_HIGH));
			LogString(SASGENERAL, "Last Above normal priority time interval(%d)", scheduler->GetLastIntervalForPriority(SAS_PRIORITY_ABOVE_NORMAL));
			LogString(SASGENERAL, "Last normal priority time interval(%d)", scheduler->GetLastIntervalForPriority(SAS_PRIORITY_NORMAL));
			LogString(SASGENERAL, "Last below normal priority time interval(%d)", scheduler->GetLastIntervalForPriority(SAS_PRIORITY_BELOW_NORMAL));
			LogString(SASGENERAL, "Last low priority time interval(%d)", scheduler->GetLastIntervalForPriority(SAS_PRIORITY_LOW));
			statsTime = curTime;
		}

		CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
	}
}

static void ScheduledSASTaskThreadInner(CSASProtocol *sasProtocol, CSASScheduler *scheduler, DWORD statsTime)
{
	try
	{
		ScheduledSASTaskThreadInner2(sasProtocol, scheduler, statsTime);
	}
	catch (...)
	{
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_SCHEDULED_SAS_TASK);
	}
}

static UINT AFX_CDECL ScheduledSASTaskThread(PVOID threadParam)
{
	CSASProtocol *sasProtocol = (CSASProtocol *)threadParam;

	CUtilities::SetThreadName(std::this_thread::get_id(), THREAD_SCHEDULED_SAS_TASK);

	// Wait until we are initialized
	while (!sasProtocol->IsInitialized() && !sasProtocol->IsTimeToClose())
	{
		CWatchDog::Instance().Checkin();
		CUtilities::Sleep(1000);
	}

	// GlowCode incorrectly reports this CSASScheduler allocation as a leak while it is in use.
	CSASScheduler *scheduler = new CSASScheduler();

	DWORD statsTime = CUtilities::GetTickCount();

	ScheduledSASTaskThreadInner(sasProtocol, scheduler, statsTime);

	delete scheduler;

	sasProtocol->ScheduledSASTaskThreadClosed();
	CWatchDog::Instance().DeregisterThread();

	return 0;
}

void CSASProtocol::GetTicketMeters(bool sendMeterUpdate304)
{
	CMeters *meters = new CMeters(MEMORY_NONE);

	//Cancel Credit
	meters->SetMeter(gCC);

	// metercodes: (2C or 0E) and 12
	meters->SetMeter(mTVal);
	meters->SetMeter(mTO);

	// metercodes: (2D or 10) and 14
	meters->SetMeter(mCPromoOutVal);
	meters->SetMeter(mCPromoOutCnt);

	// get each of these
	meters->SetMeter(mHPCshRcptVal);
	meters->SetMeter(mHPCshRcptCnt);
	meters->SetMeter(mHPWinRcptCnt);
	meters->SetMeter(mHPWinRcptVal);

	// need to get mCoupV (0d or 28)  and 0x11
	meters->SetMeter(mCoupV);
	meters->SetMeter(mCoup);

	// need to get mVchrTktDrop (29)
	meters->SetMeter(mVchrTktDrop);

	// need mCTktPV (2A or 0f) and mCTktPN (0x13)
	meters->SetMeter(mCTktPV);
	meters->SetMeter(mCTktPN);

	// need mCTktCV (2B or 84) and mCTktCN (85)
	meters->SetMeter(mCTktCV);
	meters->SetMeter(mCTktCN);

	GetMeters(*meters, EGM_GAME_NUMBER, false, true);

	CLongPoll50Message *lp50 = NULL;
	CMeters *lp50Meters;

	// check missing values
	if (meters->GetMeterByNumber(mTVal)->GetValue() == INVALID_METER_VALUE || meters->GetMeterByNumber(mTO)->GetValue() == INVALID_METER_VALUE)
	{
		// Long Poll 0x50 to get meter values
		lp50Meters = new CMeters(MEMORY_NONE);
		lp50Meters->SetMeter(mTVal);
		lp50Meters->SetMeter(mTO);
		lp50 = new CLongPoll50Message(m_gameAddress, lp50Meters);
		lp50->Fill();
		m_sasPollQueue->Add(lp50, QUEUE_PRIORITY_NORMAL);
		lp50->WaitForResponse();
		if (meters->GetMeterByNumber(mTVal)->GetValue() == INVALID_METER_VALUE) meters->GetMeterByNumber(mTVal)->SetValue(lp50Meters->GetMeterByNumber(mTVal)->GetValue());
		if (meters->GetMeterByNumber(mTO)->GetValue() == INVALID_METER_VALUE) meters->GetMeterByNumber(mTO)->SetValue(lp50Meters->GetMeterByNumber(mTO)->GetValue());
		delete lp50Meters;
		delete lp50;

		CWatchDog::Instance().Checkin();
	}

	if (meters->GetMeterByNumber(mCPromoOutVal)->GetValue() == INVALID_METER_VALUE || meters->GetMeterByNumber(mCPromoOutCnt)->GetValue() == INVALID_METER_VALUE)
	{
		// Long Poll 0x50 to get meter values
		lp50Meters = new CMeters(MEMORY_NONE);
		lp50Meters->SetMeter(mCPromoOutVal);
		lp50Meters->SetMeter(mCPromoOutCnt);
		lp50 = new CLongPoll50Message(m_gameAddress, lp50Meters);
		lp50->Fill();
		m_sasPollQueue->Add(lp50, QUEUE_PRIORITY_NORMAL);
		lp50->WaitForResponse();
		if (meters->GetMeterByNumber(mCPromoOutVal)->GetValue() == INVALID_METER_VALUE) meters->GetMeterByNumber(mCPromoOutVal)->SetValue(lp50Meters->GetMeterByNumber(mCPromoOutVal)->GetValue());
		if (meters->GetMeterByNumber(mCPromoOutCnt)->GetValue() == INVALID_METER_VALUE) meters->GetMeterByNumber(mCPromoOutCnt)->SetValue(lp50Meters->GetMeterByNumber(mCPromoOutCnt)->GetValue());
		delete lp50Meters;
		delete lp50;

		CWatchDog::Instance().Checkin();
	}


	if (meters->GetMeterByNumber(mHPCshRcptVal)->GetValue() == INVALID_METER_VALUE || meters->GetMeterByNumber(mHPCshRcptCnt)->GetValue() == INVALID_METER_VALUE)
	{
		// Long Poll 0x50 to get meter values
		lp50Meters = new CMeters(MEMORY_NONE);
		lp50Meters->SetMeter(mHPCshRcptVal);
		lp50Meters->SetMeter(mHPCshRcptCnt);
		lp50 = new CLongPoll50Message(m_gameAddress, lp50Meters);
		lp50->Fill();
		m_sasPollQueue->Add(lp50, QUEUE_PRIORITY_NORMAL);
		lp50->WaitForResponse();
		if (meters->GetMeterByNumber(mHPCshRcptVal)->GetValue() == INVALID_METER_VALUE) meters->GetMeterByNumber(mHPCshRcptVal)->SetValue(lp50Meters->GetMeterByNumber(mHPCshRcptVal)->GetValue());
		if (meters->GetMeterByNumber(mHPCshRcptCnt)->GetValue() == INVALID_METER_VALUE) meters->GetMeterByNumber(mHPCshRcptCnt)->SetValue(lp50Meters->GetMeterByNumber(mHPCshRcptCnt)->GetValue());
		delete lp50Meters;
		delete lp50;

		CWatchDog::Instance().Checkin();
	}


	if (meters->GetMeterByNumber(mHPWinRcptVal)->GetValue() == INVALID_METER_VALUE || meters->GetMeterByNumber(mHPWinRcptCnt)->GetValue() == INVALID_METER_VALUE)
	{
		// Long Poll 0x50 to get meter values
		lp50Meters = new CMeters(MEMORY_NONE);
		lp50Meters->SetMeter(mHPWinRcptVal);
		lp50Meters->SetMeter(mHPWinRcptCnt);
		lp50 = new CLongPoll50Message(m_gameAddress, lp50Meters);
		lp50->Fill();
		m_sasPollQueue->Add(lp50, QUEUE_PRIORITY_NORMAL);
		lp50->WaitForResponse();
		if (meters->GetMeterByNumber(mHPWinRcptVal)->GetValue() == INVALID_METER_VALUE) meters->GetMeterByNumber(mHPWinRcptVal)->SetValue(lp50Meters->GetMeterByNumber(mHPWinRcptVal)->GetValue());
		if (meters->GetMeterByNumber(mHPWinRcptCnt)->GetValue() == INVALID_METER_VALUE) meters->GetMeterByNumber(mHPWinRcptCnt)->SetValue(lp50Meters->GetMeterByNumber(mHPWinRcptCnt)->GetValue());
		delete lp50Meters;
		delete lp50;

		CWatchDog::Instance().Checkin();
	}


	if (meters->GetMeterByNumber(mCoupV)->GetValue() == INVALID_METER_VALUE || meters->GetMeterByNumber(mCoup)->GetValue() == INVALID_METER_VALUE)
	{
		// Long Poll 0x50 to get meter values
		lp50Meters = new CMeters(MEMORY_NONE);
		lp50Meters->SetMeter(mCoupV);
		lp50Meters->SetMeter(mCoup);
		lp50 = new CLongPoll50Message(m_gameAddress, lp50Meters);
		lp50->Fill();
		m_sasPollQueue->Add(lp50, QUEUE_PRIORITY_NORMAL);
		lp50->WaitForResponse();
		if (meters->GetMeterByNumber(mCoupV)->GetValue() == INVALID_METER_VALUE)
		{
			meters->GetMeterByNumber(mCoupV)->SetValue(lp50Meters->GetMeterByNumber(mCoupV)->GetValue());
			if (meters->GetMeterByNumber(mCoup)->GetValue() == INVALID_METER_VALUE) meters->GetMeterByNumber(mCoup)->SetValue(lp50Meters->GetMeterByNumber(mCoup)->GetValue());
		}
		delete lp50Meters;
		delete lp50;

		CWatchDog::Instance().Checkin();
	}

	if (meters->GetMeterByNumber(mVchrTktDrop)->GetValue() == INVALID_METER_VALUE)
	{

		// Long Poll 0x50 to get meter values
		lp50Meters = new CMeters(MEMORY_NONE);
		lp50Meters->SetMeter(mCoupV);
		lp50 = new CLongPoll50Message(m_gameAddress, lp50Meters);
		lp50->Fill();
		m_sasPollQueue->Add(lp50, QUEUE_PRIORITY_NORMAL);
		lp50->WaitForResponse();
		meters->GetMeterByNumber(mVchrTktDrop)->SetValue(lp50Meters->GetMeterByNumber(mCoupV)->GetValue());
		delete lp50Meters;
		delete lp50;

		CWatchDog::Instance().Checkin();
	}

	if (meters->GetMeterByNumber(mCTktPV)->GetValue() == INVALID_METER_VALUE || meters->GetMeterByNumber(mCTktPN)->GetValue() == INVALID_METER_VALUE)
	{
		// Long Poll 0x50 to get meter values
		lp50Meters = new CMeters(MEMORY_NONE);
		lp50Meters->SetMeter(mCTktPV);
		lp50Meters->SetMeter(mCTktPN);
		lp50 = new CLongPoll50Message(m_gameAddress, lp50Meters);
		lp50->Fill();
		m_sasPollQueue->Add(lp50, QUEUE_PRIORITY_NORMAL);
		lp50->WaitForResponse();
		if (meters->GetMeterByNumber(mCTktPV)->GetValue() == INVALID_METER_VALUE) meters->GetMeterByNumber(mCTktPV)->SetValue(lp50Meters->GetMeterByNumber(mCTktPV)->GetValue());
		if (meters->GetMeterByNumber(mCTktPN)->GetValue() == INVALID_METER_VALUE) meters->GetMeterByNumber(mCTktPN)->SetValue(lp50Meters->GetMeterByNumber(mCTktPN)->GetValue());
		delete lp50Meters;
		delete lp50;

		CWatchDog::Instance().Checkin();
	}

	if (meters->GetMeterByNumber(mCTktCV)->GetValue() == INVALID_METER_VALUE || meters->GetMeterByNumber(mCTktCN)->GetValue() == INVALID_METER_VALUE)
	{
		// Long Poll 0x50 to get meter values
		lp50Meters = new CMeters(MEMORY_NONE);
		lp50Meters->SetMeter(mCTktCV);
		lp50Meters->SetMeter(mCTktCN);
		lp50 = new CLongPoll50Message(m_gameAddress, lp50Meters);
		lp50->Fill();
		m_sasPollQueue->Add(lp50, QUEUE_PRIORITY_NORMAL);
		lp50->WaitForResponse();
		if (meters->GetMeterByNumber(mCTktCV)->GetValue() == INVALID_METER_VALUE) meters->GetMeterByNumber(mCTktCV)->SetValue(lp50Meters->GetMeterByNumber(mCTktCV)->GetValue());
		if (meters->GetMeterByNumber(mCTktCN)->GetValue() == INVALID_METER_VALUE) meters->GetMeterByNumber(mCTktCN)->SetValue(lp50Meters->GetMeterByNumber(mCTktCN)->GetValue());
		delete lp50Meters;
		delete lp50;

		CWatchDog::Instance().Checkin();
	}

	CInternalMessage *msg = new CInternalMessage(INT_MSG_SET_METERS, meters, EGM_GAME_NUMBER);
	u_SetMeterFlags setMeterFlags;
	setMeterFlags.flags.sendMeterUpdate304 = sendMeterUpdate304;
	msg->SetData((WORD)EGM_GAME_NUMBER, (WORD)setMeterFlags.uValue);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);

}

void CSASProtocol::GetPBTMeters()
{
	CMeters *meters = new CMeters(MEMORY_NONE);

	meters->SetMeter(mPbAR2G);
	meters->SetMeter(mPbAN2G);
	meters->SetMeter(mPbAC2H);
	meters->SetMeter(mPbAC2G);
	meters->SetMeter(mPbAR2H);
	meters->SetMeter(mPbAN2H);
	meters->SetMeter(mPbAD2G);

	GetMeters(*meters, EGM_GAME_NUMBER, false, true);

	CInternalMessage *msg = new CInternalMessage(INT_MSG_SET_METERS, meters, EGM_GAME_NUMBER);

	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CSASProtocol::ProcessEGMAward(CInternalMessage *msg)
{
	LogString("SASGENERAL", "EGM award processing");
	CEGMAward *egmAward = msg->GetEGMAward();

	if (egmAward)	
	{
		egmAward->LogInfo(_T("ss SAS received EGM award info from Internals"));

		if (AwardType::Type::Prize == egmAward->GetAwardType())
		{
			lock_guard<recursive_mutex> lock(m_egmAwardPrizeCritSection);
			m_egmPrizeAward.reset(new CEGMAward(*egmAward));			
			if (SentToGame == m_egmPrizeAward->GetAwardState() ||
				Signaled == m_egmPrizeAward->GetAwardState())
			{
				LockForEGMPrizeAward();
			}
		}
		else
		{
			// Copy the passed-in EGMAward into our local protocol copy
			lock_guard<recursive_mutex> lock(m_egmAwardCritSection);
			if (m_egmAward != NULL)
			{
				delete m_egmAward;
			}
			m_egmAward = new CEGMAward(*egmAward);
			AwardBonus();
		}
	}
}

void CSASProtocol::AwardBonus()
{
	EGMAwardState startingAwardState = NoAward;
	EGMAwardState newAwardState = NoAward;
	EGMAwardTransferType awardTransferType = EGMAwardTransferType_None;
	string statusInfo;
	string logStr;
	bool bProcessAward = false;

	// Get the arriving award state and transfer type
	{
		lock_guard<recursive_mutex> lock(m_egmAwardCritSection);
		if (m_egmAward != NULL)
		{
			bProcessAward = true;
			startingAwardState = m_egmAward->GetAwardState();
			awardTransferType = m_egmAward->GetAwardTransferType();
		}
	}

	// If this award came to us as ineligible or error, just send it back to internals.
	if (bProcessAward && (startingAwardState == AwardToGameError || startingAwardState == AwardToGameIneligible))
	{
		bProcessAward = false;
		newAwardState = startingAwardState;
		logStr = _T("ss SAS protocol received error or ineligible award");
	}

	// See if this EGM can process awards.
	if (bProcessAward && !m_sasInfo->GetLegacyBonusEnabled() && !m_sasInfo->GetAftBonusEnabled())
	{
		bProcessAward = false;
		LogString(ERROR_LOG, "No bonusing is available on this EGM");
		newAwardState = AwardToGameIneligible;
		statusInfo = CEGMAward::TEXT_SAS_NO_BONUSING_IS_AVAILABLE_ON_THIS_EGM;
		logStr = _T("[SAS Member]");
	}

	// Make sure we are not in a pbt
	if (bProcessAward)
	{
		lock_guard<recursive_mutex> lock(m_pbtCritSection);
		if (m_pbt != NULL)
		{
			bProcessAward = false;
			newAwardState = GameBusy;
			logStr = _T("ss SAS busy with PBT");
		}
	}

	// See if we just need to get the oringinal meters for the award
	if (bProcessAward && startingAwardState == WaitForMetersInitial)
	{
		bProcessAward = false;
		GetOriginalBonusingMeters();
		logStr = _T("ss SAS got original bonusing meters");
	}

	// If we are not trying to process the award then just ignore this
	if (bProcessAward && startingAwardState != SentToGame)
	{
		bProcessAward = false;
		logStr = _T("ss SAS ignored award state");
	}

	// Check to make sure we do not have a denom mismatch
	if (bProcessAward && !CheckAwardDenom())
	{
		bProcessAward = false;
		newAwardState = AwardToGameIneligible;
		statusInfo = CEGMAward::TEXT_SAS_DENOM_CHECK_FAILED;
		logStr = _T("[SAS Member]");
	}

	// Process the award
	if (bProcessAward)
	{
		// Then we need to use the same type of transfer that Internals persisted, for restart recovery to work as expected.
		if (awardTransferType == EGMAwardTransferType_SASAFTBonusingCash && m_sasInfo->GetAftBonusEnabled())
		{
			AwardAFTBonus();
		}
		else if (awardTransferType == EGMAwardTransferType_SASLegacyBonusing && m_sasInfo->GetLegacyBonusEnabled())
		{
			AwardLegacyBonus();
		}
		else
		{
			newAwardState = AwardToGameIneligible;
			statusInfo = CEGMAward::TEXT_EGM_DOES_NOT_CURRENTLY_SUPPORT_TRANSFER_TYPE;
			logStr = _T("[SAS Member]");
		}
	}

	lock_guard<recursive_mutex> lock(m_egmAwardCritSection);

	if (m_egmAward != NULL)
	{
		// See if we have a new award state
		if (newAwardState != NoAward)
		{
			m_egmAward->SetAwardState(newAwardState);
		}

		m_egmAward->SetStatusInfo(statusInfo);

		// Log now if we made a log string.
		if (!logStr.empty())
		{
			m_egmAward->LogInfo(logStr);
		}

		// Log again to say we are sending status back to internals
		m_egmAward->LogInfo(_T("ss SAS return EGM award info to Internals"));

		// Make a copy of the award to send to internals
		CEGMAward *award = new CEGMAward(*m_egmAward);

		// Send the award state back to internals
		CInternalMessage *awardMsg = new CInternalMessage(INT_MSG_EGM_AWARD, award);
		SendMessageToInternals(awardMsg, QUEUE_PRIORITY_HIGHEST);
	}
}

void CSASProtocol::AwardLegacyBonus()
{
	long long awardAmount = 0;
	long long awardAmountCredits = 0;

	lock_guard<recursive_mutex> lock(m_egmAwardCritSection);
	if (m_egmAward != NULL)
	{
		m_egmAward->LogInfo(_T("ss SAS try to award using legacy bonusing"));

		// If the amount we want to award is not a multiple of the denom, round up.
		awardAmount = m_egmAward->GetAwardAmountHit();
		if (awardAmount % m_denomMult > 0)
		{
			awardAmount += (m_denomMult - (awardAmount % m_denomMult));
		}

		awardAmountCredits = awardAmount / m_denomMult;
		if (awardAmountCredits <= 0 || CLongPoll8AMessage::MAXIMUM_LEGACY_BONUS_AMOUNT < awardAmountCredits)
		{
			LogString(ERROR_LOG, "awardAmount=%lld awardAmountCredits=%lld > MAX_LEGACY_BONUS=%u",
						 (long long)awardAmount, (long long)awardAmountCredits, (unsigned)CLongPoll8AMessage::MAXIMUM_LEGACY_BONUS_AMOUNT);
			m_egmAward->SetAwardState(AwardToGameIneligible);
			m_egmAward->SetStatusInfo(CEGMAward::TEXT_SAS_LEGACY_BONUS_AWARD_AMOUNT_INVALID);
			m_egmAward->LogInfo(_T("[SAS Member]"));
		}
		else
		{
			// Set up the poll to do the transfer
			CLongPoll8AMessage lp8a(m_gameAddress);

			// award paid amount set above to account for possible round up from cents to accounting denom
			lp8a.m_legacyBonusAmount = (DWORD)awardAmountCredits;
			lp8a.m_legacyBonusTaxStatus = 0;
			lp8a.m_bWaitForSASResponse = false;
			lp8a.Fill();

			LogString("SASGENERAL", "Legacy bonus added to queue");

			// Add the transfer poll mesaage to the SAS poll queue at high priority
			m_sasPollQueue->Add(&lp8a, QUEUE_PRIORITY_HIGHEST);

			// Wait for a response from SAS
			lp8a.WaitForResponse();
			if (lp8a.GetReturnValue() == SASPOLL_SUCCESS)
			{
				LogString("SASGENERAL", "Legacy bonus sent.");
				m_egmAward->SetAwardState(Signaled);
				m_egmAward->SetStatusInfo(CEGMAward::TEXT_SAS_LEGACY_BONUS_SIGNALED);
				m_egmAward->LogInfo(_T("[SAS Member"));
			}
			else
			{
				// Since no response is expected, this is unlikely to occur.
				LogString(ERROR_LOG, "Legacy bonus response : failure");
				m_egmAward->SetAwardState(AwardToGameIneligible);
				m_egmAward->SetStatusInfo(CEGMAward::TEXT_SAS_LEGACY_BONUS_POLL_FAILED);
				m_egmAward->LogInfo(_T("[SAS Member]"));
			}

			// Set amount paid after rounding
			m_egmAward->SetAwardAmountPaid(awardAmount);

			m_egmAward->SetSentToGameCount(m_egmAward->GetSentToGameCount() + 1);
			SYSTEMTIME systime;
			CUtilities::GetLocalSystemTime(&systime);
			m_egmAward->SetSentToGameTime(systime);
		}
	}
}

void CSASProtocol::AwardAFTBonus()
{
	bool attemptAFTBonus = false;
	EGMAwardState newAwardState = NoAward;
	LPCTSTR awardLogStr = NULL;

	if (m_egmAward != NULL)
	{
		lock_guard<recursive_mutex> lock(m_egmAwardCritSection);
		m_egmAward->LogInfo(_T("ss SAS try to award using AFT bonusing"));
		attemptAFTBonus = true;
	}

	if (attemptAFTBonus)
	{
		// Lock game
		CLongPoll74Message *lp74 = new CLongPoll74Message(m_gameAddress);
		lp74->SetLockCode(LOCK_CODE_CANCEL_PENDING);
		lp74->Fill();
		m_sasPollQueue->Add(lp74, QUEUE_PRIORITY_HIGHEST);
		lp74->WaitForResponse();
		delete lp74;

		lp74 = new CLongPoll74Message(m_gameAddress);
		lp74->SetLockCode(LOCK_CODE_REQUEST_LOCK);
		lp74->SetLockTimeout(LOCK_DURATION);
		lp74->SetBonusToGame(true);
		lp74->Fill();
		m_sasPollQueue->Add(lp74, QUEUE_PRIORITY_HIGHEST);
		lp74->WaitForResponse();
		m_sasState.SetLastLockRequest(CUtilities::GetTickCount());
		m_sasState.SetLockPending(true);

		if (lp74->GetReturnValue() == SASPOLL_SUCCESS)
		{
			switch (lp74->m_gameLockStatus)
			{
			case LOCK_CODE_GAME_LOCK_PENDING:
				// We don't know whether we will be in "SentToGame" or not.
				newAwardState = AftLockPending;
				awardLogStr = _T("ss SAS AFT Bonus lock pending");
				break;

			case LOCK_CODE_GAME_NOT_LOCKED:
				// lock has been rejected
				m_sasState.SetLockPending(false);
				LogString(ERROR_LOG, "Could not obtain lock while trying to process bonus award");
				newAwardState = GameBusy;
				awardLogStr = _T("ss SAS AFT Bonus lock rejected");
				break;

			case LOCK_CODE_GAME_LOCKED:
				// lock has been granted, start process.
				m_sasState.SetLockPending(false);
				ExecuteAFTBonusAward(lp74->m_assetNumber);
				break;
			}
		}

		delete lp74;

		lock_guard<recursive_mutex> lock(m_egmAwardCritSection);
		if (newAwardState != NoAward && m_egmAward->InProgress())
		{
			m_egmAward->SetAwardState(newAwardState);
			if (awardLogStr != NULL)
			{
				m_egmAward->LogInfo(awardLogStr);
			}
		}
	}
}

void CSASProtocol::GetOriginalBonusingMeters()
{
	// Get the current bonusing meters
	__int64 orginalMachineMeter = INVALID_METER_VALUE;
	__int64 orginalAttendantMeter = INVALID_METER_VALUE;
	__int64 orginalLegacyMeter = INVALID_METER_VALUE;

	CMeters *meters = new CMeters(MEMORY_NONE);
	meters->SetMeter(mBonusMach);
	meters->SetMeter(mBonusAtt);
	meters->SetMeter(gBW);
	GetMeters(*meters, EGM_GAME_NUMBER);
	CMeter *tempMeter = meters->GetMeterByNumber(mBonusMach);
	if (tempMeter != NULL)
	{
		orginalMachineMeter = tempMeter->GetValue();
	}
	tempMeter = meters->GetMeterByNumber(mBonusAtt);
	if (tempMeter != NULL)
	{
		orginalAttendantMeter = tempMeter->GetValue();
	}
	tempMeter = meters->GetMeterByNumber(gBW);
	if (tempMeter != NULL)
	{
		orginalLegacyMeter = tempMeter->GetValue();
	}


	// Keep these in the award object for later
	lock_guard<recursive_mutex> lock(m_egmAwardCritSection);
	if (m_egmAward != NULL)
	{
		m_egmAward->SetOriginalAttendantBonusingMeterValue(orginalAttendantMeter);
		m_egmAward->SetOriginalMachineBonusMeterValue(orginalMachineMeter);
		m_egmAward->SetOriginalLegacyBonusMeterValue(orginalLegacyMeter);
	}

	delete meters;
}

bool CSASProtocol::CheckAwardDenom()
{
	bool denomOK = true;
	string statusInfo;
	string awardLogStr;

	CLongPoll1FMessage *lp1f = new CLongPoll1FMessage(m_gameAddress);

	lp1f->Fill();
	m_sasPollQueue->Add(lp1f, QUEUE_PRIORITY_HIGHEST);
	lp1f->WaitForResponse();
	LogData("SASPBDATA", DATA_WRITE | DATA_COM1, lp1f->GetSendBuffer(), lp1f->GetSendBufferSize());
	LogData("SASPBDATA", DATA_WRITE | DATA_COM1, lp1f->GetResponseBuffer(), lp1f->GetResponseBufferSize());
	if (lp1f->GetReturnValue() == SASPOLL_SUCCESS)
	{
		WORD newDenom = (WORD)denomCodeMap[lp1f->m_denomination].multiplier;
		if (m_denomMult != newDenom)
		{
			SetDenomMult(newDenom);
			CInternalMessage *msg = new CInternalMessage(INT_MSG_SET_DENOM, (WORD)denomCodeMap[lp1f->m_denomination].multiplier);

			// Send message back to internals to adjust the denom
			SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);

			// Denom mismatch error.
			LogString(ERROR_LOG, "Denom mismatch while trying to process bonus award");
			statusInfo = CEGMAward::TEXT_SAS_DENOM_MISMATCH_ERROR;
			awardLogStr = _T("[SAS Member]");
			denomOK = false;
		}
	}
	else
	{
		LogString(ERROR_LOG, "1F long poll error while trying to process bonus award");
		statusInfo = CEGMAward::TEXT_SAS_MACHINE_INFO_LONG_POLL_ERROR__DENOM_CHECK_FAILED;
		awardLogStr = _T("[SAS Member]");
		denomOK = false;
	}

	delete lp1f;

	if (!denomOK)
	{
		lock_guard<recursive_mutex> lock(m_egmAwardCritSection);
		if (m_egmAward != NULL)
		{
			m_egmAward->SetAwardState(AwardToGameIneligible);
			m_egmAward->SetStatusInfo(statusInfo);
			if (!awardLogStr.empty())
			{
				m_egmAward->LogInfo(awardLogStr);
			}
		}
	}
	return denomOK;
}

void CSASProtocol::ExecuteAFTBonusAward(DWORD assetNumber)
{
	LogString("SASGENERAL", "Game locked, begin SAS AFT Bonus");

	LONG64 jackpotID = CEGMAward::INVALID_JACKPOT_ID;
	DWORD awardAmountInPennies = 0;
	byte transactionIDBytes[MAX_PBID_LENGTH + 1];
	int transactionIDLength = 0;

	{
		lock_guard<recursive_mutex> lock(m_egmAwardCritSection);

		// Add small sleep to give game a chance to really lock itself
		CUtilities::Sleep(PBTLOCKSLEEP);

		if (m_egmAward != NULL && (m_egmAward->GetAwardState() == SentToGame || m_egmAward->GetAwardState() == AftLockPending)) /* not really sent to game yet */
		{
			jackpotID = m_egmAward->GetJackpotId();
			string transactionID(GetTransactionId(jackpotID));
			// Check if award amount will fit in our variable and is greater than zero.
			if (m_egmAward->GetAwardAmountHit() <= 0 || DWORD_MAX < m_egmAward->GetAwardAmountHit())
			{
				LogString(ERROR_LOG, "awardAmount=%lld > DWORD_MAX=%u",
							 (long long)m_egmAward->GetAwardAmountHit(), (unsigned)DWORD_MAX);
				m_egmAward->SetAwardState(AwardToGameIneligible);
				m_egmAward->SetStatusInfo(CEGMAward::TEXT_SAS_AFT_BONUS_AWARD_AMOUNT_INVALID);
				m_egmAward->LogInfo(_T("[SAS Member]"));
				jackpotID = CEGMAward::INVALID_JACKPOT_ID;
			}
			else
			{
				awardAmountInPennies = (DWORD)m_egmAward->GetAwardAmountHit();
			}
			transactionIDLength = transactionID.size();
			strcpy((char *)transactionIDBytes, transactionID.c_str());
		}
	}

	if (CEGMAward::IsValidJackpotId(jackpotID))
	{

		lock_guard<recursive_mutex> lock2(m_egmAwardCritSection);
		CLongPoll72Message *lp72 = new CLongPoll72Message(m_gameAddress);
		lp72->ClearStatusQuery();
		lp72->SetTransferType(MethodOfPayment::PayToGame == m_egmAward->GetMethodOfPayment() ?
									 SAS_TRANSFER_TYPE_COIN_OUT_BONUS :
									 SAS_TRANSFER_TYPE_JACKPOT_BONUS);
		lp72->SetCashableAmount(awardAmountInPennies);

		// asset number
		lp72->SetAssetNumber(assetNumber);

		// set the transactionID
		lp72->SetTransactionID(transactionIDBytes, transactionIDLength);

		// send it
		lp72->Fill();
		m_sasPollQueue->Add(lp72, QUEUE_PRIORITY_HIGHEST);

		m_egmAward->SetSentToGameCount(m_egmAward->GetSentToGameCount() + 1);
		SYSTEMTIME systime;
		CUtilities::GetLocalSystemTime(&systime);
		m_egmAward->SetSentToGameTime(systime);
		m_egmAward->LogInfo(_T("MM SAS AFT award transfer increment send count"));

		// too late to turn back, we must get final completion status from EGM
		lp72->WaitForResponse();
		if (lp72->GetReturnValue() == SASPOLL_SUCCESS)
		{
			LogString(SASGENERAL, "SAS AFT bonusing transfer complete");

			LPCTSTR statusInfo;
			m_egmAward->SetAwardState(GetAwardResultFromTransferStatus(lp72->m_transferStatus, statusInfo));
			if (m_egmAward->GetAwardState() == Signaled)
			{
				m_egmAward->SetAwardAmountPaid(lp72->GetCashableAmount());
			}
			m_egmAward->SetStatusInfo(statusInfo);
			m_egmAward->LogInfo(_T("MM SAS AFT award transfer result"));
		}
		else
		{
			// something is wrong, egm responded with incomplete response or no response at all.
			LogString(ERROR_LOG, "SAS AFT ERROR getting Bonus Transfer status");
			m_egmAward->SetAwardState(AwardToGameIneligible);
			m_egmAward->SetStatusInfo(CEGMAward::TEXT_SAS_AFT_ERROR_GETTING_BONUS_TRANSFER_STATUS);
			m_egmAward->LogInfo(_T("[SAS Member]"));
		}

		delete lp72;
	}

}
// The return value is limited to MAX_PBID_LENGTH characters.
string CSASProtocol::GetTransactionId(LONG64 jackpotId)
{
	string transactionId(CUtilities::GetPrefixIdString(ATIBONUS_TRANSACTION_ID_PREFIX, jackpotId, MAX_PBID_LENGTH));
	return transactionId;
}

// Set statusInfo for EGMAwardState(s) that can be sent to One Link.
EGMAwardState CSASProtocol::GetAwardResultFromTransferStatus(byte transferStatus, LPCTSTR& statusInfo)
{
	EGMAwardState result = AwardToGameIneligible;
	switch (transferStatus)
	{
	case TRANSFER_STATUS_FULL_SUCCESSFUL:
	case TRANSFER_STATUS_PARTIAL_SUCCESSFUL:
		// transfer successful, send info to internals
		LogString(PROGRESSIVE, "AWARD: Transfer status result: Transaction successful");
		statusInfo = CEGMAward::TEXT_AFT_TRANSACTION_SUCCESSFUL;
		result = Signaled;
		break;
	case TRANSFER_STATUS_PENDING:
		LogString(PROGRESSIVE, "AWARD: TRANSFER_STATUS_PENDING in GetAwardResultFromTransferStatus");
		statusInfo = CEGMAward::TEXT_AFT_TRANSACTION_SUCCESSFUL;
		result = Signaled;
		break;
	case 0x80:
		LogString(ERROR_LOG, "Transfer status error: Transfer canceled by host");
		statusInfo = CEGMAward::TEXT_AFT_TRANSFER_CANCELED_BY_HOST;
		break;
	case 0x81:
		// This is a successful result, if we restarted during a first attempt.
		LogString(PROGRESSIVE, "AWARD: Transfer status (resend) result: Transaction ID not unique");
		statusInfo = CEGMAward::TEXT_AFT_TRANSACTION_ID_NOT_UNIQUE__SUCCESS_AFTER_RESTART;
		result = Signaled;
		break;
	case 0x82:
		LogString(ERROR_LOG, "Transfer status error: Not a valid transfer function");
		statusInfo = CEGMAward::TEXT_AFT_NOT_A_VALID_TRANSFER_FUNCTION;
		break;
	case 0x83:
		LogString(ERROR_LOG, "Transfer status error: Not a valid transfer amount or expiration");
		statusInfo = CEGMAward::TEXT_AFT_NOT_A_VALID_TRANSFER_AMOUNT_OR_EXPIRATION;
		break;
	case 0x84:
		LogString(ERROR_LOG, "Transfer status error: Transfer amount exceeds limits");
		statusInfo = CEGMAward::TEXT_AFT_TRANSFER_AMOUNT_EXCEEDS_LIMITS;
		break;
	case 0x85:
		LogString(ERROR_LOG, "Transfer status error: Transfer amount not even multiple of denom");
		statusInfo = CEGMAward::TEXT_AFT_TRANSFER_AMOUNT_NOT_EVEN_MULTIPLE_OF_DENOM;
		break;
	case 0x86:
		LogString(ERROR_LOG, "Transfer status error: cannot perform partial transfers.");
		statusInfo = CEGMAward::TEXT_AFT_CANNOT_PERFORM_PARTIAL_TRANSFERS;
		break;
	case 0x87:
		LogString(ERROR_LOG, "Transfer status error: unable to perform transfer at this time");
		result = GameBusy;
		break;
	case 0x88:
		LogString(ERROR_LOG, "Transfer status error: machine not registered.");
		statusInfo = CEGMAward::TEXT_AFT_MACHINE_NOT_REGISTERED;
		break;
	case 0x89:
		LogString(ERROR_LOG, "Transfer status error: registration key does not match");
		statusInfo = CEGMAward::TEXT_AFT_REGISTRATION_KEY_DOES_NOT_MATCH;
		break;
	case 0x8A:
		LogString(ERROR_LOG, "Transfer status error: No POS ID");
		statusInfo = CEGMAward::TEXT_AFT_NO_POS_ID;
		break;
	case 0x8B:
		// Should not occur for an award transfer to the EGM.
		LogString(ERROR_LOG, "Transfer status error: No won credits for cashout");
		statusInfo = CEGMAward::TEXT_AFT_NO_WON_CREDITS_FOR_CASHOUT;
		break;
	case 0x8C:
		LogString(ERROR_LOG, "Transfer status error: No denom set");
		statusInfo = CEGMAward::TEXT_AFT_NO_DENOM_SET;
		break;
	case 0x8D:
		LogString(ERROR_LOG, "Transfer status error: Expiration not valid");
		statusInfo = CEGMAward::TEXT_AFT_EXPIRATION_NOT_VALID;
		break;
	case 0x8E:
		LogString(ERROR_LOG, "Transfer status error: ticker device not available");
		statusInfo = CEGMAward::TEXT_AFT_TICKET_DEVICE_NOT_AVAILABLE;
		break;
	case 0x8F:
		LogString(ERROR_LOG, "Transfer status error: 8f");
		statusInfo = CEGMAward::TEXT_AFT_RESTRICTED_AMOUNTS_FROM_DIFFERENT_POOLS;
		break;
	case 0x90:
		LogString(ERROR_LOG, "Transfer status error: 90");
		statusInfo = CEGMAward::TEXT_AFT_UNABLE_TO_PRINT_TRANSACTION_RECEIPT_DEVICE_UNAVAILABLE;
		break;
	case 0x91:
		LogString(ERROR_LOG, "Transfer status error: 91");
		statusInfo = CEGMAward::TEXT_AFT_INSUFFICIENT_DATA_TO_PRINT_TRANSACTION_RECEIPT;
		break;
	case 0x92:
		LogString(ERROR_LOG, "Transfer status error: 92");
		statusInfo = CEGMAward::TEXT_AFT_TRANSACTION_RECEIPT_NOT_ALLOWED_FOR_TRANSFER_TYPE;
		break;
	case 0x93:
		LogString(ERROR_LOG, "Transfer status error: Asset number 0 or does not match.");
		statusInfo = CEGMAward::TEXT_AFT_ASSET_NUMBER_0_OR_DOES_NOT_MATCH;
		break;
	case 0x94:
		LogString(ERROR_LOG, "Transfer status error: EGM not locked");
		result = GameBusy;
		break;
	case 0x95:
		LogString(ERROR_LOG, "Transfer status error: Transaction ID not valid");
		statusInfo = CEGMAward::TEXT_AFT_TRANSACTION_ID_NOT_VALID;
		break;
	case 0x9F:
		LogString(ERROR_LOG, "Transfer status error: Unexpected error");
		statusInfo = CEGMAward::TEXT_AFT_UNEXPECTED_ERROR;
		break;
	case 0xC0:
		LogString(ERROR_LOG, "Transfer status error: Not compatible with current transfer in progress");
		statusInfo = CEGMAward::TEXT_AFT_NOT_COMPATIBLE_WITH_CURRENT_TRANSFER_IN_PROGRESS;
		break;
	case 0xC1:
		LogString(ERROR_LOG, "Transfer status error: Unsupported transfer code");
		statusInfo = CEGMAward::TEXT_AFT_UNSUPPORTED_TRANSFER_CODE;
		break;
	case 0xFF:
		LogString(ERROR_LOG, "Transfer status error: No transfer info available");
		statusInfo = CEGMAward::TEXT_AFT_NO_TRANSFER_INFO_AVAILABLE;
		break;
	default:
		LogString(ERROR_LOG, "Transfer status error: Unknown error");
		statusInfo = CEGMAward::TEXT_AFT_UNKNOWN_ERROR;
		break;
	}

	return result;
}

void CSASProtocol::GetBillMeter(int queuePriority)
{
	// Use lp 20 to get total dollar value of bills meter
	CLongPoll20Message lp20(m_gameAddress);
	lp20.Fill();
	m_sasPollQueue->Add(&lp20, queuePriority);
	lp20.WaitForResponse();
	if (lp20.GetReturnValue() == SASPOLL_SUCCESS)
	{
		m_sasBills.SetTotalBills(lp20.GetTotalBills());
	}
}

void CSASProtocol::SetDenomMult(WORD denomMult)
{
	m_denomMult = denomMult;
	m_sasBills.SetDenomMult(m_denomMult);
}

void CSASProtocol::ProcessProgressiveData(CInternalMessage *msg)
{
	CProgressiveLevelForward *levelInfo(msg->GetProgressiveLevelForward());
	if (levelInfo != NULL)
	{
		// Validate progressive level info first, then check if there is only one level to send.
		DWORD tickCount = CUtilities::GetTickCount();
		CSASLimits::ValidateSasProgressiveLevels(*levelInfo, tickCount);
		byte gameAddress = m_sasState.GetEGMConfig().GetEGMType() != CUSTOM ? m_gameAddress : 0;

		// Use long poll 80 if there is only one level to send, otherwise use long poll 86.
		if (CLongPoll80Message::LEVEL_COUNT == levelInfo->GetNumLevels())
		{
			auto it = levelInfo->cbegin();
			CLongPoll80Message lp80(gameAddress, levelInfo->GetProgressiveGroupId(), *it);
			lp80.Fill();
			m_sasPollQueue->Add(&lp80, QUEUE_PRIORITY_HIGHEST);
			if (gameAddress == m_gameAddress)
			{
				lp80.WaitForResponse();
			}
			else
			{
				lp80.m_bWaitForSASResponse = false;
			}
		}
		else if (levelInfo->GetNumLevels() > 0)
		{
			CLongPoll86Message lp86(gameAddress, *levelInfo);
			lp86.Fill();
			m_sasPollQueue->Add(&lp86, QUEUE_PRIORITY_HIGHEST);
			if (gameAddress == m_gameAddress)
			{
				lp86.WaitForResponse();
			}
			else
			{
				lp86.m_bWaitForSASResponse = false;
			}
		}
	}
}

void CSASProtocol::ResetStepdowns()
{
	// Reset all stepbacks
	m_subGameMeterStepdown.ResetAllStepdowns();
	m_mainGameMeterStepdown.ResetAllStepdowns();

	if (m_sasState.GetEGMConfig().GetEGMProtocol() == IGT_SAS_3xx)
	{
		m_mainGameMeterStepdown.DeactivateForSAS3x();
		m_subGameMeterStepdown.DeactivateForSAS3x();
	}
	// only do SAS version 5 check if game com has been fully initialized
	else if (m_bInitialized && m_sas->GetMajorVersion() <= SASVERSION5)
	{
		// SAS version 5 something, cause 6F's to not happen
		m_mainGameMeterStepdown.DeactivateForSAS5x();
		m_subGameMeterStepdown.DeactivateForSAS5x();
	}
}

void CSASProtocol::LockForEGMPrizeAward()
{
	lock_guard<recursive_mutex> lock(m_egmAwardPrizeCritSection);
	if (m_egmPrizeAward)
	{
		if (m_bIsAFTSupported)
		{
			AFTLockForEGMPrizeAward();
		}
		else
		{
			LegacyLockForEGMPrizeAward();
		}
	}
}

void CSASProtocol::LockClearForEGMPrizeAward(CInternalMessage* msg)
{
	LogString(SASGENERAL, _T("Clearing lock on game for prize award"));
	lock_guard<recursive_mutex> lock(m_egmAwardPrizeCritSection);
	int64_t jackpotId(msg->GetInt64Data());
	bool unlocked(false);
	if (m_egmPrizeAward)
	{
		if (m_egmPrizeAward->GetJackpotId() == jackpotId)
		{
			// Mark award paid
			ReportPrizeAwardStatus(AwardToGameSuccess);
			if (m_bIsAFTSupported)
			{
				AFTLockClearForEGMPrizeAward();
			}
			else
			{
				LegacyLockClearForEGMPrizeAward();
			}
			m_egmPrizeAward.reset();
			unlocked = true;
		}
		else
		{
			LogString(SASGENERAL, _T("LockClearForEGMPrizeAward - different jackpot id m_egmPrizeAward jackpotId=%lld egmAward jackpotId=%lld"), m_egmPrizeAward->GetJackpotId(), jackpotId);
		}
	}
	else
	{
		LogString(SASGENERAL, _T("LockClearForEGMPrizeAward - No active jackpot"));
	}
	
	msg->SetData((WORD)unlocked);
	SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);
}

void CSASProtocol::AFTLockForEGMPrizeAward()
{
	// Make sure PBT isn't already in progress
	if (m_pbt == nullptr)
	{
		CLongPoll74Message *lp74 = new CLongPoll74Message(m_gameAddress);
		lp74->SetLockCode(LOCK_CODE_REQUEST_LOCK);
		lp74->SetLockTimeout(PRIZE_LOCK_DURATION);
		lp74->SetXferToGame(true);
		lp74->Fill();
		m_sasPollQueue->Add(lp74,QUEUE_PRIORITY_HIGHEST);
		m_sasState.SetLastLockRequest(CUtilities::GetTickCount());
		lp74->WaitForResponse();

		if (lp74->GetReturnValue() == SASPOLL_SUCCESS)
		{
			// Check return values
			m_sasState.SetLockPending(false);
			switch (lp74->m_gameLockStatus)
			{
			case LOCK_CODE_GAME_LOCK_PENDING:
				LogString(PROGRESSIVE, "AFTLockForEGMPrizeAward-LOCK_CODE_GAME_LOCK_PENDING");
				// Do nothing, wait for the general exception x6f
				m_sasState.SetLockPending(true);
				break;
			case LOCK_CODE_GAME_NOT_LOCKED:
				LogString(PROGRESSIVE, "AFTLockForEGMPrizeAward-LOCK_CODE_GAME_NOT_LOCKED");
				ReportPrizeAwardStatus(AwardToGameError);
				break;
			case LOCK_CODE_GAME_LOCKED:
				LogString(PROGRESSIVE, "AFTLockForEGMPrizeAward-LOCK_CODE_GAME_LOCKED");
				ReportPrizeAwardStatus(Signaled);
				break;
			}
		}
		else
		{
			// lock poll failed
			ReportPrizeAwardStatus(AwardToGameError);
			LogString(SASGENERAL, _T("Prize award AFT Lock failed."));
		}

		delete lp74;
	}
	else
	{
		// PBT in process, lock not tried
		LogString(SASGENERAL, _T("Prize award AFT lock not tried because PBT active."));
		ReportPrizeAwardStatus(AwardToGameError);
	}
}

void CSASProtocol::AFTLockClearForEGMPrizeAward()
{
	// Make sure PBT isn't already in progress
	if (m_pbt == nullptr)
	{
		LogString(SASGENERAL, _T("AFTLockClearForEGMPrizeAward"));
		CLongPoll74Message *lp74 = new CLongPoll74Message(m_gameAddress);
		lp74->SetLockCode(LOCK_CODE_CANCEL_PENDING);
		lp74->SetLockTimeout(0);
		lp74->SetXferToGame(true);
		lp74->Fill();
		m_sasPollQueue->Add(lp74,QUEUE_PRIORITY_HIGHEST);
		m_sasState.SetLastLockRequest(CUtilities::GetTickCount());
		lp74->WaitForResponse();
		delete lp74;
	}
}

void CSASProtocol::LegacyLockForEGMPrizeAward()
{
	if (m_sasState.GetAllowableGameDelayPoll())
	{
		CLongPoll2EMessage *lp2E = new CLongPoll2EMessage(m_gameAddress);
		lp2E->SetGameDelay(PRIZE_LEGACY_LOCK_DURATION);
		lp2E->Fill();
		m_sasPollQueue->Add(lp2E, QUEUE_PRIORITY_HIGHEST);
		lp2E->WaitForResponse();
		if (lp2E->GetReturnValue() != SASPOLL_SUCCESS)
		{
			LogString(SASGENERAL, _T("Prize award Lecacy lock failed."));
			ReportPrizeAwardStatus(AwardToGameError);
		}
		else
		{
			ReportPrizeAwardStatus(Signaled);
		}

		delete lp2E;
	}
	else
	{
		// Game delay not supported, lock poll failed
		LogString(SASGENERAL, _T("Prize award legacy game delay not allowed."));
		ReportPrizeAwardStatus(AwardToGameError);
	}
}

void CSASProtocol::LegacyLockClearForEGMPrizeAward()
{
	if (m_sasState.GetAllowableGameDelayPoll())
	{
		CLongPoll2EMessage *lp2E = new CLongPoll2EMessage(m_gameAddress);
		lp2E->SetGameDelay(0);
		lp2E->Fill();
		m_sasPollQueue->Add(lp2E, QUEUE_PRIORITY_HIGHEST);
		lp2E->WaitForResponse();
		delete lp2E;
	}
}

void CSASProtocol::ReportPrizeAwardStatus(EGMAwardState status)
{
	if (m_egmPrizeAward && m_egmPrizeAward->GetAwardState() != status)
	{
		LogString(PROGRESSIVE, "ReportPrizeAwardStatus %d", status);
		m_egmPrizeAward->SetAwardState(status);
		SendMessageToInternals(new CInternalMessage(INT_MSG_EGM_AWARD, new CEGMAward(*m_egmPrizeAward.get())), QUEUE_PRIORITY_HIGHEST);
	}
}
