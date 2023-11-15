#include "stdafx.h"
#include "UIProtocol.h"
#include "Logging/Logger.h"

// Sentinel includes.
#include "Config/ConfigExtData.h"
#include "../Sentinel.h"
#include "../GlobalDefs.h"
#include "Hardware/WatchDog.h"
#include "../FlashStatus.h"
#include "../IPItem.h"
#include "ScreenDefs.h"
#include "../DirectoryManager.h"
#include "../DisplayManager.h"
#include "../VideoMixerUpgrade.h"
#include "../CarrierUpgrade.h"
#include "ServiceWindowDefs.h"
#include "Diagnostics.h"
#include "UI/ProgressiveBonusCounterItem.h"
#include "Logging/LogString.h"
#include "RapidJsonHelper.h"
#include "../MediaPackageManager.h"
#include "ServiceWindowManager.h"
#include "PollerProtocol/MobileConnectProtocol/PairingEvent.h"
#include "UI/FullScreenAttractorCommand.h"
#include "UI/PopupTextCommand.h"

using namespace std;

const DWORD TIMED_EVENTS_INTERVAL = 100;
static UINT AFX_CDECL ReadInternalsQueueThread(PVOID threadParam);
const DWORD waitForServiceWindowListenerThreadToExit(1000);
const string HIDEFULLSCREENATTRACTOR_COMMAND = "HideFullScreenAttractor";

// CUIProtocol dialog
CUIProtocol::CUIProtocol(CQueueList *uiToInternalsQueue) : m_uiState(new CSentinelDlgState())
{
	m_serviceWindow = nullptr;
	
	//Copy Hosts File To etc folder
	CUtilities::GetSystemResultString(FormatString("cp %s/hosts /etc", CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_APPLICATIONS).c_str()));
	m_internalsToUIQueue = NULL;
	m_uiToInternalsQueue = uiToInternalsQueue;
	m_uiReadThread = nullptr;
	m_restartingMW = false;

	// Need to get CDirectoryManager instantiated before CServiceWindowManager intantiation.
	CDirectoryManager::Instance();

	Initialize();
}

CUIProtocol::~CUIProtocol()
{
	m_uiState->SetTimeToClose(true);

	//wait for the read thread to end
	while (m_uiReadThread)
	{
		CUtilities::Sleep(10);
	}

	if (m_internalsToUIQueue != NULL)
		delete m_internalsToUIQueue;

	while (!m_serviceWindow->ServiceWindowListenerThreadExited())
	{
		CUtilities::Sleep(waitForServiceWindowListenerThreadToExit);
	}
	if (m_serviceWindow != nullptr)
	{
		delete m_serviceWindow;
	}

    if (m_uiState != nullptr) 
    {
        delete m_uiState;
    }
}

// CUIProtocol message handlers
bool CUIProtocol::Initialize()
{
	//Get current Time for our employee card screens and set the time span for 1 minute
	m_t1 = CUtilities::GetCurrentTime();

	// Set up the incoming message queue. Set logging to go to UI_LOG.
	m_internalsToUIQueue = new CInternalMessageQueueList(UI_LOG);

	// When internals adds a message to the queue, we do not want to flush all messages in the queue
	//  that are a lower priority than the messages being added,
	m_internalsToUIQueue->FlushLowerPriorityItemsUponAdd(false);

	// Start the incoming queue thread.
	m_uiReadThread = unique_ptr<thread>(new thread(ReadInternalsQueueThread, this));
    CWatchDog::Instance().RegisterThread(THREAD_READ_SENTINEL_INTERNALS_QUEUE, m_uiReadThread->get_id(), INTERNALS_TO_UI_QUEUE_TIMEOUT, m_uiReadThread->native_handle());
	m_uiReadThread->detach();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//This processes all messages from internals
void CUIProtocol::ProcessMessageFromInternals(CInternalMessage *msg)
{
	if (!ProcessServiceWindowManagerDependentMessages(msg))
	{
		LogString(UI_LOG, "ProcessMessageFromInternals message=%s", msg->GetMessageTypeStr().c_str());
		switch (msg->GetMessageType())
		{
		case INT_MSG_RESTART_SERVICEWINDOW:
			ProcessRestartServiceWindow(msg);
			break;
		case INT_MSG_TERMINATE_BROWSER:
			ProcessTerminateBrowser();
			break;
		default:
			break;
		}
		
		delete msg;
	}
}


bool CUIProtocol::ProcessServiceWindowManagerDependentMessages(CInternalMessage *msg)
{
	bool messageProcessed = true;

	// Don't process any of these messages if service window manager is not present
	if (m_serviceWindow == nullptr)
	{
		LogString(ERROR_LOG, "m_serviceWindow not populated message=%s", msg->GetMessageTypeStr().c_str());
		messageProcessed = false;
	}
	else
	{
		if (INT_MSG_SUBSCRIPTION_UPDATES != msg->GetMessageType())
		{
			LogString(UI_LOG, "ProcessServiceWindowManagerDependentMessages message=%s", msg->GetMessageTypeStr().c_str());
		}
		bool bDeleteMsg = true;
		
		switch (msg->GetMessageType())
		{
		case INT_MSG_SUBSCRIPTION_UPDATES:
			ProcessSubscriptionUpdates(msg);
			break;
		case INT_MSG_UPDATE_UI_CONFIG:
			ProcessUIConfig(msg);
			break;
		case INT_MSG_SHOW_ANNIVERSARY:
			ProcessDisplayAnniversary(msg);
			break;
		case INT_MSG_SHOW_BIRTHDAY:
			ProcessDisplayBirthday(msg);
			break;
		case INT_MSG_SHOW_BIRTHDAY_AND_ANNIVERSARY:
			ProcessDisplayBirthdayAndAnniversary(msg);
			break;
		case INT_MSG_PLAY_CONGRATS:
			ProcessDisplayCongrats(msg);
			break;
		case INT_MSG_BAD_CARD_READ:
			ProcessDisplayBadCardRead(msg);
			break;
		case INT_MSG_DISPLAY_BANNED_PLAYER:
			ProcessDisplayBannedPlayerMessage(msg);
			break;
		case INT_MSG_DISPLAY_CASHOUT_PBT_ERROR:
			ProcessDisplayCashoutPBTError(msg);
			break;
		case INT_MSG_DISPLAY_AUTOUPLOAD_ERROR:
			ProcessDisplayAutoUploadError(msg);
			break;
		case INT_MSG_LISTEN_TO_PBT_PROGRESS_FROM_GAME:
			ProcessListenToPBTProgressFromGame(msg);
			break;
		case INT_MSG_LISTEN_TO_PBT_PROGRESS_TO_GAME:
			ProcessListenToPBTProgressToGame(msg);
			break;
		case INT_MSG_CHECK_UPLOADABLE_CREDITS:
			ProcessCheckUploadableCredits(msg);
			break;
		case INT_MSG_CLEAR_PBT:
			ProcessClearPBT(msg);
			break;
		case INT_MSG_AUTO_TRANSFER_CANCELLED:
			ProcessDisplayAutoTransferCancelled(msg);
			break;
		case INT_MSG_MONITOR_MOBILE_PB_SESSION:
			ProcessMonitorMobilePBSession(msg);
			break;
		case INT_MSG_DISPLAY_PB_TRANSFER_ERROR:
			ProcessDisplayPBTransferError(msg);
			break;
		case INT_MSG_DISPLAY_EMPLOYEE_SCREEN:
			ProcessDisplayEmployeeScreen(msg);
			break;
		case INT_MSG_EMPLOYEE_PIN_RESULT:
			ProcessEmployeePinResult(msg);
			break;
		case INT_MSG_ACTIVE_SESSION:
			m_uiState->SetActiveSession(/*cardedPlayer*/msg->GetWordData() || /*sessionActive*/msg->GetWordData(true));
			break;
		case INT_MSG_EMPLOYEE_CARD_IN:
			ProcessEmployeeCardIn(msg);
			break;
		case INT_MSG_CAMPAIGN_AVAILABLE:
			ProcessCampaignAvailable(msg);
			break;
		case INT_MSG_REFRESH_UI:
			ProcessRefreshUI(msg);
			break;
		case INT_MSG_SHUTDOWN:
			PerformShutdown(msg);
			break;
		case INT_MSG_PROGRESSIVE_JACKPOT_ANNOUNCEMENT:
		case INT_MSG_PROGRESSIVE_TOTALIZER_ANNOUNCEMENT:
		case INT_MSG_DISPLAY_UJP:
		case INT_MSG_PROGRESSIVE_CLAIMABLE_AWARD:
		case INT_MSG_PROGRESSIVE_SCHEDULE_END:
		case INT_MSG_PROGRESSIVE_WINNER_PICK:
		case INT_MSG_PROGRESSIVE_WINNER_PICKED:
		case INT_MSG_PROGRESSIVE_PRIZE:
		case INT_MSG_PROGRESSIVE_PROMO_GAME:
			m_serviceWindow->SendMessageToAngularUI(msg->GetStringData());
			break;
		case INT_MSG_CARD_REMOVED:
			if (m_uiState->GetClearOnCardRemove())
			{
				CCommandMessage hideFullScreenCmd(HIDEFULLSCREENATTRACTOR_COMMAND);
				m_serviceWindow->SendMessageToAngularUI(hideFullScreenCmd.getJSON());
				m_uiState->SetClearOnCardRemove(false);
			}
			break;
		case INT_MSG_PROBE_PBT_RESULT:
			{
				bool bLock = msg->GetWordData();
				CCommandMessage pbtProbeResultCmd("PbtProbeResult");
				pbtProbeResultCmd.AddStringArg("ProbeResult",
					bLock ? "true" : "false");
				m_serviceWindow->SendMessageToAngularUI(pbtProbeResultCmd.getJSON());
			}
			break;
		case INT_MSG_EGM_AWARD_STATUS:
			ProcessEgmAwardFailed(msg);
			break;
		default:
			messageProcessed = false;
			bDeleteMsg = false;
			break;
		}
		
		if (bDeleteMsg)
		{
			delete msg;
		}
	}
	
	return messageProcessed;
}

//called to check any timed events that may need to be proccesed
void CUIProtocol::CheckTimedEvents(void)
{
	m_t1 = CUtilities::GetCurrentTime();

	CWatchDog::Instance().Checkin();
	TiltCode tiltCode(CSmartDataLogger::Instance().TimedEvents(m_uiState->IsActiveSession()));
	if (tltUnknown != tiltCode)
	{
		SendMessageToInternals(new CInternalMessage(INT_MSG_SEND_TILT, (WORD)tiltCode), QUEUE_PRIORITY_NORMAL);
	}

	if (CSmartDataLogger::Instance().NeedToRequestForSubscriptionUpdate())
	{
		SendMessageToInternals(new CInternalMessage(INT_MSG_SUBSCRIPTION_UPDATES), QUEUE_PRIORITY_HIGHEST);
	}	

	CWatchDog::Instance().Checkin();
}

//called to shut down the read thread to internals during a shut down
void CUIProtocol::SetReadThreadClosed()
{
	m_uiReadThread.reset();
}

void CUIProtocol::ProcessAngularUIMessage(const string& message)
{
	// UI_REWRITE_CLEANUP, need to clean last 3 arguments.
	CInternalMessage* msg(m_uiState->CreateAngularUIMessageForInternals(message));

	if (msg)
	{
		if (INT_MSG_ADMIN_MESSAGE == msg->GetMessageType())
		{
			rapidjson::Document jsonDoc;
			string jsonString(msg->GetStringData());
			try
			{
				if (jsonDoc.Parse(jsonString.c_str()).HasParseError())
				{
					LogString(ERROR_LOG, "[CUIProtocol::ProcessAngularUIMessage] parsing %s failed.", jsonString.c_str());
				}
				else
				{
					m_uiState->ProcessAdminMessageFromAngularUI(msg, JsonGetObjectEx(jsonDoc, PARAMS.c_str()));
				}
			}
			catch (JsonException& e)
			{
				LogString(ERROR_LOG, "CUIProtocol::ProcessAngularUIMessage] JSON exception: %s: '%s'", 
					e.codestring(), e.what());
			}
			catch (std::out_of_range& e)
			{
				LogString(ERROR_LOG, "[CUIProtocol::ProcessAngularUIMessage] out_of_range exception: '%s'", e.what());
			}
			catch (std::invalid_argument& e)
			{
				LogString(ERROR_LOG, "[CUIProtocol::ProcessAngularUIMessage] invalid_argument exception: '%s'", e.what());
			}
		}

		if (INT_MSG_CLEAR_DISPLAY_STARVATION == msg->GetMessageType())
		{
			LogString(UI_LOG, "[CUIProtocol::ProcessAngularUIMessage] ClearDisplayStarvation received from UI");
			m_internalsToUIQueue->ClearQueueStarvation(PERSIST_CONTENT_WINDOW);
			delete msg;
		}
		else if (INT_MSG_START_CAPTURING_METERS == msg->GetMessageType())
		{
			SendMessageToInternals(new CInternalMessage(INT_MSG_START_CAPTURING_METERS, msg->GetWordData()), QUEUE_PRIORITY_HIGHEST);
			delete msg;
		}
		else if (INT_MSG_ADMIN_MESSAGE == msg->GetMessageType())
		{
			delete msg;
		}
		else if (INT_MSG_DISPLAY_EMPLOYEE_SCREEN == msg->GetMessageType())
		{
			SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);
		}		
		else if (INT_MSG_SHUTDOWN == msg->GetMessageType())
		{
            PerformShutdown(msg);
			delete msg;
        }
		else if (INT_MSG_ARCHIVE_LOGS == msg->GetMessageType())
		{
			delete msg;
		}
		else if (INT_MSG_TEST_URL == msg->GetMessageType())
		{
			CSubscription* subscription(msg->GetSubscriptionData());
			m_serviceWindow->SendMessageToAngularUI(subscription->GetSubscriptionData());
			delete msg;
		}
		// UI_REWRITE_CLEANUP: button defines have been removed
		//else if (INT_MSG_PROCESS_BUTTON_PRESS == msg->GetMessageType() && (DWORD)buttonPlayerPersonalBankerFromSwf == msg->GetDWordData())
		//{
		//	SendMessageToUI(msg, QUEUE_PRIORITY_HIGHEST);
		//}
		else if (INT_MSG_EXTENDED_BUCKETS_EXIT == msg->GetMessageType() )
		{
			delete msg;
		}
		else
		{
			if (INT_MSG_PROMOGAME_COMPLETE == msg->GetMessageType() && /*isWinner==false*/ !msg->GetWordData())
			{
				SendMessageToUI(new CInternalMessage(INT_MSG_ATTRACTOR_SEQUENCE_COMPLETE, /*isMainAttactor*/(DWORD)true), QUEUE_PRIORITY_HIGHEST);
			}
			else if (INT_MSG_SET_CARD_BEZEL == msg->GetMessageType())
			{}
			SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);
		}
	}
}

//called to send messages to internals
void CUIProtocol::SendMessageToInternals(CInternalMessage *msg, int priorityLevel)
{
	if (m_uiToInternalsQueue != NULL)
		m_uiToInternalsQueue->Add(msg, priorityLevel);
	else
		delete msg;
}

//called to send messages to ourself
bool CUIProtocol::PushMessageToOurQueue(CInternalMessage *msg, int priorityLevel)
{
	bool pushed = false;

	if (m_internalsToUIQueue != NULL)
	{
		m_internalsToUIQueue->Add(msg, priorityLevel);
		pushed = true;
	}

	return pushed;
}

// Called from child objects (service window) to push messages on the UI queue as if they came from internals.
void CUIProtocol::SendMessageToUI(CInternalMessage *msg, int priorityLevel)
{
	bool messagePushed = false;
	while (!messagePushed)
	{
		messagePushed = PushMessageToOurQueue(msg, priorityLevel);
		CUtilities::Sleep(0);
	}
}

//called to determine if it is time to close the UI
bool  CUIProtocol::IsTimeToClose()
{
	return m_uiState->IsTimeToClose();
}

bool CUIProtocol::IsTimeToRestartMWThread()
{
	return m_restartingMW;
}

//Read internals queue thread
static void ReadInternalsQueueThreadInner(CUIProtocol *uiProtocol, CQueueList *queueList, binsem& itemAddedEventHandle)
{
	CUtilities::ShowThreadId(_T(THREAD_READ_SENTINEL_INTERNALS_QUEUE));
	
	// This loop never ends
	while (!uiProtocol->IsTimeToClose())
	{
		// Wait for an item to be added to the queue
		CWatchDog::Instance().Checkin();
		itemAddedEventHandle.try_wait_for(TIMED_EVENTS_INTERVAL);
		CWatchDog::Instance().Checkin();

		// Remove the messages from the queue and give them to the internals object
		//  until there are no more.
		CInternalMessage *msg = (CInternalMessage *)queueList->Remove();

		while (msg != nullptr && !uiProtocol->IsTimeToClose())		//Tracker #25893
		{
			// Give this message to internals to process
			uiProtocol->ProcessMessageFromInternals(msg);
			CWatchDog::Instance().Checkin();

			// Just in case this loops for a long time, let other threads have a go.
			CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);

			// Get the next message (if any)
			msg = (CInternalMessage *)queueList->Remove();
		}

		if (!uiProtocol->IsTimeToClose())		//Tracker #25893
		{
			uiProtocol->CheckTimedEvents();
		}
		CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
	}
}

static UINT AFX_CDECL ReadInternalsQueueThread(PVOID threadParam)
{
	CUIProtocol *uiProtocol = (CUIProtocol *)threadParam;

	CUtilities::SetThreadName(std::this_thread::get_id(), THREAD_READ_SENTINEL_INTERNALS_QUEUE);

	// Get the queue of messages from Internals
	CQueueList *queueList = uiProtocol->GetInternalsQueue();

	// Get the handle to the event that gets called when an item is put in the queue
	binsem& itemAddedEventHandle = queueList->GetAddedItemEventHandle();

	try
	{
		ReadInternalsQueueThreadInner(uiProtocol, queueList, itemAddedEventHandle);
	}
    catch (...) 
    {
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_READ_SENTINEL_INTERNALS_QUEUE);
    }

	CWatchDog::Instance().DeregisterThread();
	uiProtocol->SetReadThreadClosed();

	return 0;
}

void CUIProtocol::ProcessEmployeeCardIn(CInternalMessage* msg)
{
	bool employeeCardIn = (bool)(msg->GetWordData(false));

	m_uiState->SetEmployeeCardIn(employeeCardIn);

	if (!employeeCardIn)
	{
		SendEmployeeCardEventSubscriptionData();
	}
}

void CUIProtocol::ProcessCampaignAvailable(CInternalMessage *msg)
{
	UNREFERENCED_PARAMETER(&msg);

	CCommandMessage cmdMsg("RefreshCampaign");
	m_serviceWindow->SendMessageToAngularUI(cmdMsg.getJSON());
}

void CUIProtocol::ProcessRefreshUI(CInternalMessage *msg)
{
	UNREFERENCED_PARAMETER(&msg);

	CCommandMessage cmdMsg("RefreshUI");
	m_serviceWindow->SendMessageToAngularUI(cmdMsg.getJSON());
}

void CUIProtocol::ProcessRestartServiceWindow(CInternalMessage *msg)
{
	RestartServiceWindow((bool)msg->GetWordData(), false);
}

void CUIProtocol::ProcessTerminateBrowser()
{
	ShutdownServiceWindow();
}

void CUIProtocol::ShutdownServiceWindow()
{	
	LogString(UI_LOG, "Unsubscribing to all before service window shutdown");
	
	CWatchDog::Instance().Checkin();
	
	LogString(UI_LOG, "Deleting Service Window");
	
	if (m_serviceWindow != nullptr)
	{
		CServiceWindowManager* serviceWindow(m_serviceWindow);
		m_serviceWindow = nullptr;
		m_restartingMW = true;
		while (!serviceWindow->ServiceWindowListenerThreadExited())
		{
			CUtilities::Sleep(waitForServiceWindowListenerThreadToExit);
		}
		delete serviceWindow;
	}
	
	LogString(UI_LOG, "Service Window deleted");
	
	CWatchDog::Instance().Checkin();
	
	LogString(UI_LOG, "Sending message notifying UI the browser has been terminated.");
	SendMessageToInternals(new CInternalMessage(INT_MSG_TERMINATE_BROWSER), QUEUE_PRIORITY_NORMAL);
}

void CUIProtocol::RestartServiceWindow(bool calibrationError, bool startup)
{
	LogString(UI_LOG, "Shutting down the UI before restart");

	ShutdownServiceWindow();

	CDirectoryManager::Instance().RenameTempDirectories();
	CDirectoryManager::Instance().DetermineWorkingMediaPaths();

	// Restart the UI before service window shell is re-instantiated.

	const DWORD RestartUIDelay(1000);

	if (calibrationError)
	{
		LogString(UI_LOG, "Sleeping for %d milliseconds before restarting Weston because there was a calibration error.", RestartUIDelay);
		CUtilities::Sleep(RestartUIDelay);
	}

	LogString(UI_LOG, "Restarting Weston");
	const string RestartUICmdLine = "/etc/init.d/weston restart";
	if (system(RestartUICmdLine.c_str()) == 0)
	{
		LogString(UI_LOG, "UI command successful for %s", RestartUICmdLine.c_str());
	}
	else
	{
		LogString(ERROR_LOG, "Error restarting UI with command %s", RestartUICmdLine.c_str());
	}

	LogString(UI_LOG, "Reinstantiating Service Window Manager");

	m_serviceWindow = new CServiceWindowManager();

	CWatchDog::Instance().Checkin();

	m_restartingMW = false;
	m_serviceWindow->Start(this);
	if (m_serviceWindow->ServiceWindowListenerThreadExited())
	{
		CWatchDog::Instance().Checkin();
	}
	else
	{
		CWatchDog::Instance().Checkin();

		if (!startup)
		{
			// Send message to internals that service window was restarted
			SendMessageToInternals(new CInternalMessage(INT_MSG_RESTART_SERVICEWINDOW), QUEUE_PRIORITY_NORMAL);
		}
	}
}

void CUIProtocol::ProcessSubscriptionUpdates(CInternalMessage* msg)
{
	std::vector<CSubscription>* subscriptions(msg->GetSubscriptions());
	if (NULL != subscriptions)
	{
		for (const CSubscription& subscription : *subscriptions)
		{
			m_serviceWindow->SendMessageToAngularUI(subscription.GetSubscriptionData());
		}
	}
}

void CUIProtocol::ProcessUIConfig(CInternalMessage *msg)
{
	if (msg != nullptr)
	{
		CUIConfig *uiConfig(msg->GetUIConfig());
		if (uiConfig != nullptr)
		{
			m_uiConfig = *uiConfig;
			m_serviceWindow->SetUIConfig(m_uiConfig);
		}
	}
}

void CUIProtocol::ProcessDisplayAnniversary(CInternalMessage *msg)
{	
	UNREFERENCED_PARAMETER(&msg);

	unique_ptr<CFullScreenAttractorCommand> cmdMsg(m_serviceWindow->CreateFullScreenAttractor(screenPlayerAnniversary));
	m_serviceWindow->SendMessageToAngularUI(cmdMsg->getJSON());
}

void CUIProtocol::ProcessDisplayBirthday(CInternalMessage *msg)
{
	UNREFERENCED_PARAMETER(&msg);

	unique_ptr<CFullScreenAttractorCommand> cmdMsg(m_serviceWindow->CreateFullScreenAttractor(screenPlayerBirthday));
	m_serviceWindow->SendMessageToAngularUI(cmdMsg->getJSON());
}

void CUIProtocol::ProcessDisplayBirthdayAndAnniversary(CInternalMessage *msg)
{
	UNREFERENCED_PARAMETER(&msg);

	unique_ptr<CFullScreenAttractorCommand> cmdMsg(m_serviceWindow->CreateFullScreenAttractor(screenPlayerBirthdayAndAnniversary));
	m_serviceWindow->SendMessageToAngularUI(cmdMsg->getJSON());
}

void CUIProtocol::ProcessDisplayCongrats(CInternalMessage* msg)
{
	UNREFERENCED_PARAMETER(&msg);

	unique_ptr<CFullScreenAttractorCommand> cmdMsg(m_serviceWindow->CreateFullScreenAttractor(screenPlayerCongratulations));
	m_serviceWindow->SendMessageToAngularUI(cmdMsg->getJSON());
}

void CUIProtocol::ProcessDisplayBadCardRead(CInternalMessage* msg)
{
	m_uiState->SetClearOnCardRemove(true);
	DWORD displayDuration = 0;
	const bool isRFID = (bool)msg->GetWordData();
	if (isRFID)
	{
		displayDuration = POPUPDISPLAYTIME;
	}

	unique_ptr<CFullScreenAttractorCommand> cmdMsg(m_serviceWindow->CreateFullScreenAttractor(screenPlayerBadCard, "", displayDuration));
	m_serviceWindow->SendMessageToAngularUI(cmdMsg->getJSON());
}

void CUIProtocol::ProcessDisplayBannedPlayerMessage(CInternalMessage* msg)
{
	UNREFERENCED_PARAMETER(&msg);

	m_uiState->SetClearOnCardRemove(true);

	unique_ptr<CFullScreenAttractorCommand> cmdMsg(m_serviceWindow->CreateFullScreenAttractor(screenBannedFromPlayMessage));
	m_serviceWindow->SendMessageToAngularUI(cmdMsg->getJSON());
}

void CUIProtocol::ProcessDisplayCashoutPBTError(CInternalMessage *msg)
{
	WORD errorFlag = msg->GetWordData();
	if (errorFlag)
	{
		CPopupTextCommand cmdMsg(TEXT_TAG_TYPE_CASHOUT_ERROR_FLAGS, std::to_string(errorFlag), PopupTextType::Popup_Failure, POPUPDISPLAYTIME);
		m_serviceWindow->SendMessageToAngularUI(cmdMsg.getJSON());
	}
	else
	{
		LogString(ERROR_LOG, "[%s:%d] Cashout error flag is empty.", __FUNCTION__, __LINE__);
		LogString(PBT_LOG, "[%s:%d] Cashout error flag is empty.", __FUNCTION__, __LINE__);
	}
}

void CUIProtocol::ProcessDisplayAutoUploadError(CInternalMessage* msg)
{
	WORD errorFlag = msg->GetWordData();
	if (errorFlag)
	{
		CPopupTextCommand cmdMsg(TEXT_TAG_TYPE_AUTOUPLOAD_ERROR_FLAGS, std::to_string(errorFlag), PopupTextType::Popup_Failure, POPUPDISPLAYTIME);
		m_serviceWindow->SendMessageToAngularUI(cmdMsg.getJSON());
	}
	else
	{
		LogString(ERROR_LOG, "[%s:%d] Auto upload error flag is empty.", __FUNCTION__, __LINE__);
		LogString(PBT_LOG, "[%s:%d] Auto upload error flag is empty.", __FUNCTION__, __LINE__);
	}
}

void CUIProtocol::ProcessListenToPBTProgressFromGame(CInternalMessage *msg)
{
	UNREFERENCED_PARAMETER(&msg);

	CCommandMessage cmdMsg("ListenToPBTProgressFromGame");
	m_serviceWindow->SendMessageToAngularUI(cmdMsg.getJSON());
}

void CUIProtocol::ProcessListenToPBTProgressToGame(CInternalMessage *msg)
{
	UNREFERENCED_PARAMETER(&msg);

	CCommandMessage cmdMsg("ListenToPBTProgressToGame");
	m_serviceWindow->SendMessageToAngularUI(cmdMsg.getJSON());
}

void CUIProtocol::ProcessCheckUploadableCredits(CInternalMessage *msg)
{
	UNREFERENCED_PARAMETER(&msg);

	CPopupTextCommand cmdMsg(TEXT_TAG_TYPE_GENERIC, "CheckUploadableCredits", PopupTextType::Popup_Info, 0);
	m_serviceWindow->SendMessageToAngularUI(cmdMsg.getJSON());
}

void CUIProtocol::ProcessClearPBT(CInternalMessage *msg)
{
	UNREFERENCED_PARAMETER(&msg);

	CCommandMessage cmdMsg("HidePopupText");
	m_serviceWindow->SendMessageToAngularUI(cmdMsg.getJSON());
}

void CUIProtocol::ProcessDisplayAutoTransferCancelled(CInternalMessage *msg)
{
	UNREFERENCED_PARAMETER(&msg);

	CPopupTextCommand cmdMsg(TEXT_TAG_TYPE_GENERIC, "AutoTransferCancelled", PopupTextType::Popup_Failure, POPUPDISPLAYTIME);
	m_serviceWindow->SendMessageToAngularUI(cmdMsg.getJSON());
}

void CUIProtocol::ProcessMonitorMobilePBSession(CInternalMessage *msg)
{
	UNREFERENCED_PARAMETER(&msg);

	CCommandMessage cmdMsg("MonitorMobilePBSession");
	m_serviceWindow->SendMessageToAngularUI(cmdMsg.getJSON());
}

void CUIProtocol::ProcessDisplayPBTransferError(CInternalMessage *msg)
{
	CPopupTextCommand cmdMsg(TEXT_TAG_TYPE_PB_TRANSFER_ERROR, msg->GetStringData(), PopupTextType::Popup_Failure, POPUPDISPLAYTIME);
	m_serviceWindow->SendMessageToAngularUI(cmdMsg.getJSON());
}

void CUIProtocol::PerformShutdown(CInternalMessage* msg)
{
	LogString(UI_LOG, "PerformShutdown");

	TiltCode tiltCode(msg->GetWordData() ? tltRebootUiRestart : tltRebootButton);
	WORD rebootDueToShellUiRestart(tltRebootUiRestart == tiltCode);
	SendMessageToInternals(new CInternalMessage(INT_MSG_STATUS_REBOOTING, rebootDueToShellUiRestart), QUEUE_PRIORITY_HIGHEST);

	// Sleep display time then kill watchdog
	CDiagnostics::Instance().PersistReboot(tiltCode, POLLER_LOG);

	CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);

	CWatchDog::Instance().BeginShutdown(CWebDiagnosticsStatusInfo::TiltCodeToString(tiltCode));
}

void CUIProtocol::ProcessDisplayEmployeeScreen(CInternalMessage* msg)
{
	if (msg != NULL)
	{
		bool startCalibration(msg->GetWordData(true));
		bool showApplyRevertScreen(msg->GetDWordData(false));
		bool pinRequired(msg->GetDWordData(true));
		if (startCalibration)
		{
			SendMessageToInternals(new CInternalMessage(INT_MSG_CALIBRATE_SENTINEL_SCREEN), QUEUE_PRIORITY_HIGHEST);
		}
		else
		{
			m_uiState->SetCardEventSubscription((CardType)msg->GetWordData(), showApplyRevertScreen, pinRequired);
			SendEmployeeCardEventSubscriptionData();
			if (CARD_CURRENCY == msg->GetWordData() || CARD_METER == msg->GetWordData())
			{
				SendMessageToInternals(new CInternalMessage(INT_MSG_START_CAPTURING_METERS, 
					msg->GetWordData() == CARD_METER ?
					(WORD)MeterCardIn : (WORD)MpCurrencyCard), QUEUE_PRIORITY_HIGHEST);
			}
		}
	}
}

void CUIProtocol::ProcessEmployeePinResult(CInternalMessage* msg)
{
	CSubscription* subscription(m_uiState->ProcessEmployeePinResult(msg));

	if (subscription)
	{
		m_serviceWindow->SendMessageToAngularUI(subscription->GetSubscriptionData());
	}
}

void CUIProtocol::SendEmployeeCardEventSubscriptionData()
{
	string subscriptionData;
	m_uiState->GetEmployeeCardEventSubscriptionData(subscriptionData);
	if (!subscriptionData.empty())
	{
		m_serviceWindow->SendMessageToAngularUI(subscriptionData);
	}
}

void CUIProtocol::ProcessEgmAwardFailed(CInternalMessage *msg)
{
	int64_t amount = msg->GetInt64Data();
	CCommandMessage cmdMsg("AwardFailed");
	cmdMsg.AddStringArg("Amount", std::to_string(amount));
	m_serviceWindow->SendMessageToAngularUI(cmdMsg.getJSON());
}
