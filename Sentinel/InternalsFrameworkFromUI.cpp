#include "stdafx.h"

#include "InternalsFramework.h"

// Sentinel includes.
#include "Hardware/WatchDog.h"
#include "Logging/Logger.h"
#include "Logging/LogString.h"
#include "UI/MediaWindowLog.h"
#include "UI/RepairLog.h"
#include "UI/AttendantPaidPayoutLog.h"
#include "FileList.h"
#include "VideoModeConfig.h"
#include "VideoMixerUpgrade.h"
#include "CarrierUpgrade.h"
#include "Diagnostics.h"
#include "Config/ConfigExtData.h"
#include "ProgressiveProtocol/ProgressiveWinnerPick.h"
#include "ProgressiveProtocol/ProgressiveWinnerPicked.h"

using namespace std;

static UINT AFX_CDECL UpdateMixerThread(PVOID threadParam);

const LPCTSTR UPGRADE_FILENAME = _T("/s5-update.txt");
const LPCTSTR UPGRADE_FILENAME_TMP = _T("/s5-update.txt.tmp");

const LPCTSTR RESTORE_SYS_FILES_COMMAND = _T("\\windows\\system32\\xcopy.exe /y /i /h /e c:\\restore e:\\update");
const unsigned RESTORE_SYS_FILES_COMMAND_WAIT = 30000;

void CInternalsFramework::ProcessMessageFromUI( CInternalMessage *msg )
{
	bool bDeleteMsg = true;

	switch (msg->GetMessageType())
	{
	case INT_MSG_SET_POLLER_SETTINGS:
		ProcessSetPollerSettings(msg);
		break;
	case INT_MSG_SET_DHCP_SETTING:
		ProcessSetDhcpSettings(msg);
		break;
	case INT_MSG_SET_IP_ADDRESS:
		ProcessSetSentinelIpAddress(msg);
		break;
	case INT_MSG_SET_SUBNET_MASK:
		ProcessSetSubnetMask(msg);
		break;
	case INT_MSG_SET_DEFAULT_GATEWAY:
		ProcessSetDefaultGateway(msg);
		break;
	case INT_MSG_SET_PREFERRED_DNS:
		ProcessSetPreferredDnsServer(msg);
		break;
	case INT_MSG_SET_ALTERNATE_DNS:
		ProcessSetAlternateDnsServer(msg);
		break;		
	case INT_MSG_GET_ACCOUNT_BALANCE:
		ProcessGetAccountBalance();
		break;
	case INT_MSG_CHECK_EMPLOYEE_PIN:
		ProcessCheckEmployeePin(msg);
		break;
	case INT_MSG_PB_CHECK_PIN:
		ProcessPBCheckPinFromUI(msg);
		break;
	case INT_MSG_UJP_CHECK_PIN:
		ProcessUJPCheckPinFromUI(msg);
		break;
	case INT_MSG_PB_CANCEL_SESSION:
		ProcessCancelPersonalBankerSession(msg);
		break;
	case INT_MSG_PB_RESTORE_SESSION:
		ProcessRestorePersonalBankerSession();
		break;
	case INT_MSG_PB_REQUEST_TRANSFER:
		ProcessRequestPersonalBankerTransferFromUI(msg);
		break;
	case INT_MSG_SET_EGM_SETTINGS:
		ProcessChangeEGMTypeFromUI(msg);
		break;
	case INT_MSG_START_CAPTURING_METERS:
		ProcessMeterSnapshot(msg);
		break;
	case INT_MSG_DISPLAY_EMPLOYEE_SCREEN:
		// UI_REWRITE_CLEANUP: Validate if we ar missing any use-case of employee screen launch this way, or we want to keep this around for media window use case reference.
		SendEmployeeDisplayStart(false, false, false);
		break;
	case INT_MSG_SET_VOLUME:
		ProcessSetVolumeFromUI(msg);
		break;
	case INT_MSG_SET_BACKLIGHT:
		ProcessSetBacklightFromUI(msg);
		break;
	case INT_MSG_SET_VIDEO_MODE_CONFIG:
		ProcessSetVideoModeConfigFromUI(msg);
		bDeleteMsg = false;
		break;
	case INT_MSG_ACCEPT_VIDEO_MODE_CONFIG:
		ProcessAcceptVideoModeFromUI(msg);
		break;
	case INT_MSG_SET_DESKTOP_ORIENTATION_TYPE:
		ProcessSetDesktopOrientationTypeFromUI(msg);
		break;
	case INT_MSG_MACHINE_ONLINE:
		ProcessMachineOnlineFromUI(msg);
		break;
	case INT_MSG_MACHINE_OFFLINE:
		ProcessMachineOfflineFromUI();
		break;
	case INT_MSG_MACHINE_MOVE_BEGIN:
		ProcessBeginMachineMoveFromUI();
		break;
	case INT_MSG_MACHINE_MOVE_COMPLETE:
		ProcessCompleteMachineMoveFromUI();
		break;
	case INT_MSG_SEND_PRINTER_MAINTENANCE:
		ProcessPrinterMaintenanceFromUI();
		break;
	case INT_MSG_SEND_REPAIR_CODE:
		ProcessSendRepairCodeFromUI(msg);
		break;
	case INT_MSG_ATTENDANT_PAID_PAYOUT:
		ProcessAttendantPayoutFromUI(msg);
		break;
	case INT_MSG_SEND_HOPPER_FILL:
		ProcessHopperFillFromUI(msg);
		break;
	case INT_MSG_SEND_WITNESS:
		ProcessSendWitnessFromUI();
		break;
	case INT_MSG_SEND_MAINTENANCE:
		ProcessSendMaintenanceFromUI(msg);
		break;
	case INT_MSG_FORCE_CARD_OUT:
		ProcessForceCardOutFromUI();
		break;
	case INT_MSG_PERFORM_BATTERY_TEST:
		PerformBatteryTest();
		break;
	case INT_MSG_BILL_VALIDATOR_DISABLE:
		// Loss limit no longer supported
		break;
	case INT_MSG_BILL_VALIDATOR_ENABLE:
		// Loss limit no longer supported
		break;
	case INT_MSG_SET_JACKPOT_RESET:
		// going to send msg on to sas protocol, do not delete it
		bDeleteMsg = false;
		ProcessSetJackpotResetSettingFromUI(msg);
		break;
    case INT_MSG_SET_TOUCH:
        ProcessSetTouchFromUI(msg);
        break;
	case INT_MSG_PREVIEW_SW_SIZE:
		ProcessPreviewSWSizeFromUI();
		break;
	case INT_MSG_PREVIEW_TASKBAR_SIZE:
		ProcessPreviewTaskbarSizeFromUI();
		break;
	case INT_MSG_PREVIEW_SW_LOCATION:
		ProcessPreviewSWLocationFromUI();
		break;
	case INT_MSG_SET_SERVICE_WINDOW:
		ProcessSetServiceWindowFromUI(msg);
		break;
	case INT_MSG_SET_TASKBAR:
		ProcessSetTaskbarFromUI(msg);
		break;
	// UI_REWRITE_CLEANUP: looks like we can remove below 2 messages, above msgs also to be scanned...
	case INT_MSG_SHOW_SERVICE_WINDOW:
		m_displayManager->ShowServiceWindow();
		SaveServiceWindowOpen(true);
		break;
	case INT_MSG_HIDE_SERVICE_WINDOW:
		ProcessHideServiceWindow();
		break;
	case INT_MSG_CLEAR_PERSISTENT_MEMORY:
		m_restartManager.SetProcessingUiHeartbeats(false);
		ProcessClearPersistentMemory(msg);
		break;
	case INT_MSG_SUBSCRIBE_REQUEST:
		ProcessUiSubscribeRequest(msg);
		break;
	case INT_MSG_UNSUBSCRIBE_REQUEST:
		ProcessUiUnsubscribeRequest(msg);
		break;
	case INT_MSG_MOBILE_CONNECT_PRESS:
		ProcessMobileConnectPressed();
		break;
	case INT_MSG_TERMINATE_BROWSER:
		ProcessBrowserTerminationFromUI();
		break;
	case INT_MSG_RESTART_SERVICEWINDOW:
		ProcessRestartMediaWindowFromUI(msg);
		break;
	// UI_REWRITE_CLEANUP: below 5 messages must not be needed (related to media window)
	case INT_MSG_GET_EGM_DISPLAY:
		ProcessGetEgmDisplaySetting(msg);
		break;
	case INT_MSG_UPDATE_EGM_DISPLAY_SETTING:
		ProcessUpdateEgmDisplaySettings(msg);
		break;
	case INT_MSG_UPDATE_PLAYER_STATE_CLOSE_SETTING:
		ProcessUpdatePlayerStateCloseSettings(msg);
		break;
	case INT_MSG_UPDATE_PLAYER_STATE_OPEN_SETTING:
		ProcessUpdatePlayerStateOpenSettings(msg);
		break;
	case INT_MSG_UPDATE_AUTO_CLOSE_SETTING:
		ProcessUpdateAutoCloseSetting(msg);
		break;
	case INT_MSG_HEARTBEAT:
		m_restartManager.SetUiHeartbeatReceived();
		break;
	case INT_MSG_EMPLOYEE_GOODBYE:
		ProcessEmployeeGoodbye();
		break;
	case INT_MSG_PLAYER_GOODBYE:
		ProcessPlayerGoodbye();
		break;
	case INT_MSG_BUTTON_ACTIVITY:
		ProcessButtonActivity(msg);
		break;
	case INT_MSG_PB_SET_NEW_PIN:
		ProcessSetNewPinFromUI(msg);
		break;
	case INT_MSG_UJP_CALLHOST:
		ProcessUJPCallHost(false);
		break;
	case INT_MSG_UJP_TIMED_OUT:
		ProcessUJPPinpadTimeout();
		break;
	case INT_MSG_CALIBRATE_SENTINEL_SCREEN:
		StartCalibration();
		break;
	case INT_MSG_UPGRADE_MIXER:
		ProcessUpgradeMixer();
		break;
	case INT_MSG_UPGRADE_CARRIER:
		ProcessUpgradeCarrier();
		break;
	case INT_MSG_FLUSH_PLAYER_UPDATE:
		ProcessFlushPlayerUpdate(msg);
		break;
	case INT_MSG_PROBE_PBT:
		ProcessProbePBTFromUI(msg->GetWordData() == 1);
		break;
	case INT_MSG_STATUS_REBOOTING:
	{
		bool rebootDueToShellUiRestart(msg->GetWordData() == 1);
		if (rebootDueToShellUiRestart)
		{
			CInternalMessage* controlRebootMessage(PrepareTransactionMessage(INT_MSG_TRANSACTION_CONTROLLED_REBOOT, IsPlayerVirtual()));
			if (controlRebootMessage)
			{
				SendTransactionMessageToSCF(controlRebootMessage, QUEUE_PRIORITY_NORMAL);
			}
		}
		break;
	}		
    case INT_MSG_UPDATE_UI_CONFIG:
		ProcessUpdateUIConfigFromUI(msg);
		break;
	case INT_MSG_ATTRACTOR_SEQUENCE_COMPLETE:
		m_notificationBox.SetMobileState(MobileState::Disconnected);
		break;
	case INT_MSG_PROMOGAME_COMPLETE:
		SendMessageToProgressive(msg, QUEUE_PRIORITY_HIGHEST);
		bDeleteMsg = false;
		break;
	case INT_MSG_PRIZE_LOCK_CLEAR:
		ProcessPrizeLockClearFromUI();
		break;
	case INT_MSG_PROGRESSIVE_PROMO_GAME:
	{
		std::lock_guard<std::recursive_mutex> lock(m_egmAwardCriticalSection);
		m_egmAwardManager.AddPendingPromoJackpotId(msg->GetInt64Data());
		break;
	}
	case INT_MSG_LIST_OF_CERTS:
	{
		vector<string>* listOfCerts(msg->GetVectorOfStrings());
		if (listOfCerts)
		{
			std::lock_guard<std::recursive_mutex> lock(m_listOfCertsCriticalSection);
			m_listOfCerts = *listOfCerts;
		}		
		break;
	}
	case INT_MSG_SET_CARD_BEZEL:
		McrSetBezelStyle((MACHINE_STATE)msg->GetWordData());
		break;
	case INT_MSG_AUTO_CARDOUT_TIMED_OUT:
	case INT_MSG_AUTO_CARDOUT_END_SESSION:
		LogString(SESSION_LOG, "ProcessMessageFromUI=%s", msg->GetMessageTypeStr().c_str());
		PerformPlayerCardTimedEvents(msg->GetMessageType());
		break;
	case INT_MSG_AUTO_CARDOUT_CONTINUE_SESSION:
		LogString(SESSION_LOG, "ProcessMessageFromUI=%s", msg->GetMessageTypeStr().c_str());
		m_playerSession.PlayerActivityOccurred();
		if (m_player)
		{
			GenerateTilt(tltAutoCardOutTimerReset);
		}
		break;
	case INT_MSG_SEND_TILT:
		ProcessTilt(msg);
		break;
	case INT_MSG_SUBSCRIPTION_UPDATES:
		SendSubscriptionUpdateIfNecessary();
		break;
	case INT_MSG_UI_ACTIVITY_OCCURRED:
		m_playerSession.PlayerActivityOccurred();
		ResetPersonalBankerInactivityTimer();
		break;
	case INT_MSG_START_PB_STARVATION:
	{
		bDeleteMsg = false;
		int priority(QUEUE_PRIORITY_NORMAL);
		msg->SetMessagePersist(m_msgPriority.PrioritizeMessage(msg, priority));
		SendMessageToUI(msg, priority);
		break;
	}
	default:
		break;
	}

	if ( bDeleteMsg )
		delete msg;

	CWatchDog::Instance().Checkin();

}

void CInternalsFramework::ProcessSendMaintenanceFromUI(  CInternalMessage *msg )
{
	DWORD repairCode = msg->GetDWordData();

	m_repairLog->Add(new CRepairLog(repairCode));

	CInternalMessage *transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_MAINTENANCE, IsPlayerVirtual());
	transMsg->SetData( (DWORD)m_config.GetSlotNumber() );
	transMsg->SetData( (__int64)msg->GetDWordData() );
	SendTransactionMessageToSCF( transMsg, QUEUE_PRIORITY_NORMAL );
}

void CInternalsFramework::ProcessForceCardOutFromUI()
{
	ForcePlayerCardOut();
}

void CInternalsFramework::ProcessMachineOnlineFromUI( CInternalMessage *msg )
{
	if ( m_EGMConfig.CanPerformMachineOnline() )
	{
		// Indicate that we are in the process of trying to get the EGM on line.
		m_EGMConfig.SetSentinelOnlineState( sentinelComingOnline );

		// The first thing we have to do is issue a slotmast request from the SCF.
		m_config.SetPendingSlotmast(msg->GetDWordData());
		SendSlotmastChangeToSCF();

		SendSubscriptionUpdateIfNecessary();
	}
}

void CInternalsFramework::ProcessMachineOfflineFromUI( void )
{
	if ( m_sentinelState.TransactionFillStatusOkay() )
	{
		if ( m_EGMConfig.CanPerformMachineOffline() )
		{
			m_EGMConfig.SetSentinelOnlineState( sentinelGoingOffline );

			SendSubscriptionUpdateIfNecessary();
			
			BeginMeterCapture(FinalMeters, true, false);
		}
	}
}

void CInternalsFramework::ProcessBeginMachineMoveFromUI( void )
{
	if ( m_sentinelState.TransactionFillStatusOkay() )
	{
		if ( m_EGMConfig.CanBeginMachineMove() )
		{
			m_EGMConfig.SetSentinelOnlineState( sentinelGoingOfflineMove );

			BeginMeterCapture(MachineMoveBegin, true, false);
		}
	}
}

void CInternalsFramework::ProcessCompleteMachineMoveFromUI( void )
{
	if ( m_EGMConfig.CanCompleteMachineMove() )
	{
		// Indicate that we are in the process of trying to complete the EGM move.
			m_EGMConfig.SetSentinelOnlineState( sentinelComingOnlineMove );

		// Start up the GCF. This will capture the meters when started.
		StartGCF();
	}
}

void  CInternalsFramework::ProcessSetConnection()
{
	{
		lock_guard<recursive_mutex> lock(m_adapterCriticalSection);
		ConfigureNetworkAdapter();
	}

	CInternalMessage *connDataMsgToPoller = CreateConnectionDataMessage();
	SendMessageToSCF(connDataMsgToPoller, QUEUE_PRIORITY_NORMAL);
	m_sentinelIp = connDataMsgToPoller->GetConnectionData()->GetActualIP().GetIP();
	LogString(NETWORKING_LOG, "[CInternalsFramework::ProcessSetConnection] [From UI] Sentinel IP sent to SCF: [%s]", m_sentinelIp.c_str());

	if (m_sentinelState.m_timeOfLastFloorLogixConfigAvailable)
	{
		// Send the connection data to the progressive once we have a config.
		SendMessageToProgressive(CreateConnectionDataMessage(), QUEUE_PRIORITY_NORMAL);
	}
}

void CInternalsFramework::ProcessSetPollerSettings(CInternalMessage *msg)
{
	m_config.SetHostAddressOrName(msg->GetStringData());
	m_config.SetHostPort(_tcstol(msg->GetStringData(true).c_str(), NULL, RADIX_DECIMAL));

	ProcessSetConnection();
}

void CInternalsFramework::ProcessSetDhcpSettings(CInternalMessage *msg)
{
	m_config.SetDHCPConfig((bool)msg->GetWordData());

	if (m_config.GetDHCPConfig())
	{
		ProcessSetConnection();
	}
}

void CInternalsFramework::ProcessSetSentinelIpAddress(CInternalMessage *msg)
{
	if (m_config.GetDHCPConfig())
	{
		//if we're setting to use dhcp, then set the static ip to 0.0.0.0
		CIPAddress ip;
		ip.SetIP("0.0.0.0");
		m_config.SetMyIP(ip);
	}
	else
	{
		LogString(UI_LOG, "%s %d: Setting IP Address: %s", __FUNCTION__, __LINE__,
			msg->GetStringData().c_str());		
		m_config.SetMyIP(msg->GetStringData());
	}
	ProcessSetConnection();
}

void CInternalsFramework::ProcessSetSubnetMask(CInternalMessage *msg)
{
	m_config.SetSubnetMask(msg->GetStringData());
}

void CInternalsFramework::ProcessSetDefaultGateway(CInternalMessage *msg)
{
	m_config.SetDefaultGateway(msg->GetStringData());
}

void CInternalsFramework::ProcessSetPreferredDnsServer(CInternalMessage *msg)
{
	m_config.SetPreferredDNSServer(msg->GetStringData());
}

void CInternalsFramework::ProcessSetAlternateDnsServer(CInternalMessage *msg)
{
	m_config.SetAlternateDNSServer(msg->GetStringData());
}

void CInternalsFramework::ProcessCheckEmployeePin( CInternalMessage *msg )
{
	ValidateEnteredEmployeePin( msg->GetStringData() );
}

void CInternalsFramework::ProcessPBCheckDenom( void )
{
	m_personalBanker->SetWaitingForDenomCheck(true);

	CInternalMessage *msg = new CInternalMessage( INT_MSG_GET_DENOM );
	SendMessageToGCF( msg, QUEUE_PRIORITY_NORMAL );
}

void CInternalsFramework::ProcessPBCheckPinFromUI( CInternalMessage *msg )
{
	BeginPersonalBankerSession(msg);
}

void CInternalsFramework::ProcessRequestPersonalBankerTransferFromUI( CInternalMessage *msg )
{
	BeginPersonalBankerTransaction( (PersonalBankerTransferType)msg->GetDWordData(), msg->GetDWordData( true ) );
}

void CInternalsFramework::SetDoorState()
{
	m_sentinelState.SetDoorOpen(IsDoorOpen(mSDr));
}

void CInternalsFramework::SendServiceWindowShowToUI( bool bShow )
{
	CInternalMessage *msg = new CInternalMessage( INT_MSG_SERVICE_WINDOW_SHOW, (WORD)(bShow ? 1 : 0)  );
	SendMessageToUI( msg, QUEUE_PRIORITY_HIGHEST );	
	SaveServiceWindowOpen(bShow);
}

void CInternalsFramework::ProcessMeterSnapshot(CInternalMessage *msg)
{
	if ( m_sentinelState.TransactionFillStatusOkay() )
	{
		BeginMeterCapture((SnapshotType)msg->GetWordData(), true, false);
	}
}

void CInternalsFramework::ProcessSetVolumeFromUI( CInternalMessage *msg )
{	
	m_config.SetSpeakerVolume( msg->GetWordData() > 100 ? 100 : (byte)msg->GetWordData(),
		msg->GetWordData() > 100 ? 100 : (byte)msg->GetWordData() );

	AdjustSpeakerVolume();
}

void CInternalsFramework::ProcessSetBacklightFromUI(CInternalMessage *msg)
{
	m_config.SetScreenBrightness( msg->GetWordData() > 100 ? 100 : (byte)msg->GetWordData() );

	AdjustScreenBrightness();

}

void CInternalsFramework::ProcessSendRepairCodeFromUI( CInternalMessage *msg )
{
	DWORD repairCode(msg->GetDWordData());
	m_repairLog->Add(new CRepairLog(repairCode));

	CInternalMessage *transMsg = PrepareTransactionMessage( INT_MSG_TRANSACTION_FAILURE_CODE, IsPlayerVirtual() );
	transMsg->SetData( (DWORD)m_config.GetSlotNumber() );
	transMsg->SetData( (__int64)repairCode );

	SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::ProcessSetVideoModeConfigFromUI( CInternalMessage *msg  )
{
    LogString(DISPLAY_LOG, "Processing video config from the UI");

	CVideoModeConfig *videoModeConfig = msg->GetVideoModeConfig();

	VideoConfigChangeStatus changeStatus = m_displayManager->SetVideoModeConfig(videoModeConfig);
	
	if (changeStatus == VIDEO_CHANGE_RESTARTING_UI)
	{
		SendMessageToUI(new CInternalMessage(INT_MSG_RESTART_SERVICEWINDOW), QUEUE_PRIORITY_HIGHEST);
	}
}

void CInternalsFramework::ProcessAcceptVideoModeFromUI(CInternalMessage *msg)
{
	bool keepVideoMode = msg->GetWordData(false);
	
	if (keepVideoMode)
	{
		m_displayManager->KeepVideoModeChange();
	}
	else
	{
		VideoConfigChangeStatus changeStatus = m_displayManager->RevertVideoMode();
		if (changeStatus == VIDEO_CHANGE_RESTARTING_UI)
		{
			m_sentinelState.SetUIRestartInProgress(true);
			SendMessageToUI(new CInternalMessage(INT_MSG_RESTART_SERVICEWINDOW), QUEUE_PRIORITY_HIGHEST);
		}
	}
}

void CInternalsFramework::ProcessSetDesktopOrientationTypeFromUI( CInternalMessage *msg  )
{
	DesktopOrientationType displayOrientation = (DesktopOrientationType)(msg->GetWordData());
	DesktopOrientationType touchOrientation = (DesktopOrientationType)(msg->GetWordData(true));

	m_config.SetOrientationType(displayOrientation);

    if (m_displayManager->SetDesktopRotation(displayOrientation))
    {
		m_sentinelState.SetUIRestartInProgress(true);
		SendMessageToUI(new CInternalMessage(INT_MSG_RESTART_SERVICEWINDOW), QUEUE_PRIORITY_HIGHEST);
	}

    m_displayManager->SetTouchScreenRotation(touchOrientation);
}

void CInternalsFramework::ProcessAttendantPayoutFromUI( CInternalMessage *msg )
{
	__int64 amount = msg->GetInt64Data();

	CInternalMessage *transMsg = PrepareTransactionMessage( INT_MSG_TRANSACTION_JACKPOT_FILL, IsPlayerVirtual() );
	transMsg->SetData( amount );
	transMsg->SetData( (DWORD) m_config.GetSlotNumber() );
	transMsg->SetData( (WORD)KE_TYPE_JACKPOT );

	SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::ProcessHopperFillFromUI( CInternalMessage *msg )
{
	__int64 amount = msg->GetInt64Data();

	CInternalMessage *transMsg = PrepareTransactionMessage( INT_MSG_TRANSACTION_JACKPOT_FILL, IsPlayerVirtual() );
	transMsg->SetData( amount );
	transMsg->SetData( (DWORD) m_config.GetSlotNumber() );
	transMsg->SetData( (WORD)KE_TYPE_FILL );

	SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);

	m_repairLog->Add(new CRepairLog(KE_TYPE_FILL));
}

void CInternalsFramework::ProcessPrinterMaintenanceFromUI()
{
	CInternalMessage *transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_JACKPOT_FILL, IsPlayerVirtual());
	transMsg->SetData((__int64)0);		// The amount is zero for printer maintenance
	transMsg->SetData((DWORD)m_config.GetSlotNumber());
	transMsg->SetData((WORD)KE_TYPE_PRINTER_MAINT);

	SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);

	m_repairLog->Add(new CRepairLog(KE_TYPE_PRINTER_MAINT));
}

void CInternalsFramework::ProcessSendWitnessFromUI( )
{
	CInternalMessage *transMsg = PrepareTransactionMessage( INT_MSG_SEND_WITNESS, IsPlayerVirtual());
	SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::PerformBatteryTest()
{
	m_config.SetLastBatteryCheckTime(CUtilities::GetCurrentTime());
	m_config.SetBatteryStatus(GetBatteryStatus());
}

bool CInternalsFramework::FileValid(const string& path, const string& filename, int checksum)
{
	bool retVal = false;

	DWORD retChecksum;
	
	std::string filePath = path + filename;

	if ( CalcFileChecksum( filePath, retChecksum, true ) && ((int)retChecksum == checksum) )
	{
		retVal = true;
	}
	else
	{
		LogString(ERROR_LOG, "File Check Failure %s - expected checksum %d, actual %d", filePath.c_str(), checksum, retChecksum);
	}

	return retVal;
}

void CInternalsFramework::ProcessSetJackpotResetSettingFromUI( CInternalMessage *msg )
{
	// Don't allow A8 reset if ujp is enabled
	bool resetToCredit = !m_config.GetUJPEnabled() && msg->GetWordData(false);
	m_config.SetJackpotA8Reset(resetToCredit);
	msg->SetData((WORD)resetToCredit);

	// send msg on to SAS Protocol
	SendMessageToGCF(msg,QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::ProcessSetTouchFromUI(CInternalMessage* msg)
{
	if (msg != NULL)
	{
        m_displayManager->SetTouchProtocol(msg->GetStringData());
    }
}

void CInternalsFramework::ProcessPreviewSWSizeFromUI()
{
	// If previewing setting, do it here
}

void CInternalsFramework::ProcessPreviewTaskbarSizeFromUI()
{
	// If previewing setting, do it here
}

void CInternalsFramework::ProcessPreviewSWLocationFromUI()
{
	// If previewing setting, do it here
}

void CInternalsFramework::ProcessSetServiceWindowFromUI( CInternalMessage *msg )
{
	LogString(DISPLAY_LOG, _T("Message received from UI to set the content window to %d, orientation %d"), msg->GetWordData(true), msg->GetWordData(false));
	
	// Persist new settings and update display
	m_mwConfig->SetContentWindowSize(msg->GetWordData(true), true);
	m_mwConfig->SetContentWindowOrientation((ContentWindowOrientation)msg->GetWordData(false), true);

	m_displayManager->SetMediaWindowConfig(*m_mwConfig);
	m_displayManager->UpdateVideoMixerDisplay();

	LogString(UI_LOG, "%s %d", __FUNCTION__, __LINE__);
	SendDisplayAttractorToUI();
}

void CInternalsFramework::ProcessSetTaskbarFromUI(CInternalMessage *msg)
{
	LogString(DISPLAY_LOG, _T("Message received from UI to set the taskbar to %d"), msg->GetWordData(false));
	
	// Persist new settings and update display
	m_mwConfig->SetTaskbarSize(msg->GetWordData(false), true);

	m_displayManager->SetMediaWindowConfig(*m_mwConfig);
	m_displayManager->UpdateVideoMixerDisplay();
}

void CInternalsFramework::ProcessMobileConnectPressed()
{
	m_notificationBox.SetMobileState(MobileState::PleaseWait);
	SendSubscriptionUpdateIfNecessary();
	LogString(UI_LOG, "%s %d: Sending Pairing Request to Internals" , __FUNCTION__, __LINE__);
	RequestPairing();
}

void CInternalsFramework::ProcessUiSubscribeRequest(CInternalMessage *msg)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	CSubscription* subscriptionData(msg->GetSubscriptionData());
	m_dataSubscriptionManager.Subscribe(subscriptionData);
	if (ArchiveLogsProgress == subscriptionData->GetTopic())
	{
		TiltCode tiltCode(CSmartDataLogger::Instance().ArchiveLogs(msg->GetStringData()));
		if (tltUnknown != tiltCode)
		{
			GenerateTilt(tiltCode);
		}
	}

	SendSubscriptionUpdateIfNecessary();
}

void CInternalsFramework::ProcessUiUnsubscribeRequest(CInternalMessage *msg)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	m_dataSubscriptionManager.Unsubscribe(msg->GetSubscriptionData());
}

void CInternalsFramework::SaveServiceWindowOpen(bool serviceWindowOpen)
{
	//m_notificationBox.SetContentWindowOpen(serviceWindowOpen, m_mwConfig->GetBonusEventPlayerState());

	// Save window status to NvRam.
	m_config.SetServiceWindowOpen(serviceWindowOpen);
}

void CInternalsFramework::ProcessHideServiceWindow()
{
	m_notificationBox.SetMobileState(MobileState::Disconnected);
	m_displayManager->HideServiceWindow();
	SaveServiceWindowOpen(false);
	m_protocolManager->ClearInboundQueueStarvation(IProtocolManager::UIProtocol, PERSIST_CONTENT_WINDOW);

	ProcessUJPCallHost(true);
}

void CInternalsFramework::ProcessClearPersistentMemory(CInternalMessage *msg)
{
	if (msg)
	{
		const int NVRAMCHECKBYTEOFFSET = SPRINKLE1_OFFSET;
		const int EEPROMCHECKBYTEOFFSET = EEPROM_CHECKBYTE_OFFSET;
		string memoryType(msg->GetStringData());
		LogString(GENERAL, "Clearing persistant memory type: %s", memoryType.c_str());
		BYTE zero(0);
		bool clearNvram(false), clearEeprom(false), clearHardware(false);
		if (TXT_GLOBAL_CLEAR_NVRAM == memoryType)
		{
			clearNvram = true;
		}
		else if (TXT_GLOBAL_CLEAR_EEPROM == memoryType)
		{
			clearEeprom = true;
		}
		else if (TXT_GLOBAL_CLEAR_HARDWARE == memoryType)
		{
			clearHardware = true;
		}
		else if (TXT_GLOBAL_CLEAR_ALL == memoryType)
		{
			clearNvram = true;
			clearEeprom = true;
		}
        else
        {
            LogString(ERROR_LOG, "Unknown persistant memory clear type of %s", memoryType.c_str() );
        }

		if (clearNvram)
		{
			//Create CLEAR_MEMORY
			string createClearMemorFile(FormatString("touch %s", CDirectoryManager::CLEAR_MEMORY));
			CUtilities::GetSystemResultString(createClearMemorFile);

			CNVRAMinator *nvram = new CNVRAMinator();
			//Make sure PBT object isn't restored after nvram clear.
			CPersonalBankerTransfer::SetInactiveLocation();
			CEGMAward::SetInactiveLocation();
			StandardProgressiveWins::SetInactiveLocation();
			nvram->SetData(NVRAMCHECKBYTEOFFSET, &zero, sizeof(zero)); // Mark NVRAM invalid by setting SPRINKLE1 to an invalid value.
			delete nvram;
		}

		if (clearEeprom)
		{
			CEEPROMinator *eeprom = new CEEPROMinator();
			eeprom->SetData(EEPROMCHECKBYTEOFFSET, &zero, sizeof(zero)); // Mark EEPROM invalid by setting not equal to EEPROM_CHECKBYTE_VALUE.
			delete eeprom;
		}

		if (clearHardware)
		{
			CDisplayManager::Instance().ClearMixerHardware();
		}

		m_sentinelState.MemoryClearSuccessful();

		m_notificationBox.AddAlert(Alerts::ALERT_REBOOT_IN_PROGRESS);
		SendSubscriptionUpdateIfNecessary();

		CDiagnostics::Instance().PersistReboot(tltRebootMemoryClear, "MEMORY");
		CWatchDog::Instance().Checkin();
		CUtilities::Sleep(POPUPDISPLAYTIME);
		CWatchDog::Instance().Checkin();
		CWatchDog::Instance().BeginShutdown(CWebDiagnosticsStatusInfo::TiltCodeToString(tltRebootMemoryClear));
	}
}

void CInternalsFramework::ProcessRestartMediaWindowFromUI(CInternalMessage *msg)
{
	LogString(DISPLAY_LOG, "Message received from UI that the the UI has restarted" );

	m_sentinelState.ClearCurrentActivity(CurrentActivity::RestartingUi);
	SendDashboardStatusToProgressive();

	m_sentinelState.m_lastUIRestartTime = CUtilities::GetCurrentTime();
	++m_sentinelState.m_uiRestartCount;

	// Tell the display manager that the UI has been restarted
	VideoConfigChangeStatus videoStatus = CDisplayManager::Instance().UIRestartComplete();
	
	// See if the video revert screen nees to be displayed
	bool showVideoRevertScreen = (videoStatus == VIDEO_CHANGE_REVERT_TIMER);
	
	//Need to call this function, because Weston reset causes volume to go at some higher default value.
	AdjustSpeakerVolume();

	m_sentinelState.SetUIRestartInProgress(false);
}

void CInternalsFramework::ProcessBrowserTerminationFromUI()
{
	LogString(DISPLAY_LOG, "Browser terminated message received from UI" );
	CDisplayManager::Instance().BrowserTerminated();
}


void CInternalsFramework::ProcessUpgradeCarrier()
{
	CVideoMixerUpgrade *vmUpgrade = nullptr;
	CCarrierUpgrade *cUpgrade = nullptr;
	FindFirmware(vmUpgrade, cUpgrade);

	//S5_Result result;
	bool upgradePerformed = false;

	// determine if something should be upgraded
	// scracker #7146, don't attempt on LCDs
	if (cUpgrade && !cUpgrade->IsCurrent())
	{
		upgradePerformed = cUpgrade->Upgrade();
	}

	// kill watchdog if an upgrade was performed, prevent video mixer reset
	if (upgradePerformed)
	{
		CVideoMixerUpgrade *tmpvmUpgrade = nullptr;
		CCarrierUpgrade *tmpcUpgrade = nullptr;
		string path = FindFirmware(tmpvmUpgrade, tmpcUpgrade);
		if ((!tmpcUpgrade || tmpcUpgrade->IsCurrent()) &&
			 (!tmpvmUpgrade || (CDisplayManager::Instance().GetDisplayPlatform() != VideoMixerPlatform) || (tmpvmUpgrade && tmpvmUpgrade->IsCurrent()))
			 )
		{
			// Remove FPGA folder from meidadisk
			// Don't want to remove FPGA folder from USB stick so make sure path doesn't contain "sd"
			if (!path.empty() && path.find("sd") == std::string::npos && !CUtilities::DeleteDirectory(path))
			{
				// Log error
				LogString(ERROR_LOG, "ProcessUpgradeCarrier-Could not remove FPGA folder %s.", path.c_str());
			}

		}
		delete tmpvmUpgrade;
		delete tmpcUpgrade;

		CDiagnostics::Instance().PersistReboot(tltRebootFirmUpgrade, "FLASHING");
		CWatchDog::Instance().BeginShutdown(CWebDiagnosticsStatusInfo::TiltCodeToString(tltRebootFirmUpgrade));
	}
	delete vmUpgrade;
	delete cUpgrade;
}

void CInternalsFramework::ProcessUpgradeMixer()
{
	m_updateMixerThread.reset(new thread(UpdateMixerThread, this));
	CWatchDog::Instance().RegisterThread(THREAD_UPGRADE_MIXER, m_updateMixerThread->get_id(), UPDATE_MIXER_TIMEOUT, m_updateMixerThread->native_handle());
	m_updateMixerThread->detach();
}

string CInternalsFramework::FindFirmware(CVideoMixerUpgrade *&vmUpgrade, CCarrierUpgrade *&cUpgrade)
{
	const string fpga = "/FPGA";
	const string fpgaXml = "/FPGA/FPGA.xml";
	FPGAUpgradeLocationType locationType = LocationNone;

	// Start with USB drive so a manual update is always found
	bool moreUSBPaths = true;
	bool fileFound = false;
	int usbIndex = 0;
	string path("");
	while (!fileFound && moreUSBPaths)
	{
		path = CDirectoryManager::Instance().USBPath(usbIndex);
		if (!path.empty())
		{
			fileFound = CUtilities::FileExists(path + fpgaXml);
		}
		else
		{
			moreUSBPaths = false;
		}
		usbIndex++;
	}

	// Get the OS path
	if (!fileFound)
	{
		path = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_MEDIADISK);
		fileFound = CUtilities::FileExists(path + fpgaXml);
	}

	if (fileFound)
	{
		if (path == CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_MEDIADISK))
		{
			locationType = DownloadLocation;
		}
		else
		{
			locationType = ExternalLocation;
		}

		// process xml
		vmUpgrade = new CVideoMixerUpgrade(path, locationType);
		cUpgrade = new CCarrierUpgrade(path, locationType);
	}
	else
	{
		// process without xml
		vmUpgrade = new CVideoMixerUpgrade("", locationType);
		cUpgrade = new CCarrierUpgrade("", locationType);
	}
	
	return path + fpga;
}

void CInternalsFramework::SetUiStateObject(CSentinelDlgState* uiState)
{
	m_playerSession.SetUiStateObject(uiState);
}

void CInternalsFramework::ProcessGetEgmDisplaySetting(CInternalMessage *msg )
{
	bool maintainAspectRatio = (bool)msg->GetWordData();
	
	LogString(DISPLAY_LOG, _T("Message received from UI to set the egm display to %s"), maintainAspectRatio ? "maintain apect ratio" : "fit to size" );
	
	// Persist new settings and update display
	m_mwConfig->SetMaintainAspectRatio(maintainAspectRatio, true);

	m_displayManager->SetMediaWindowConfig(*m_mwConfig);
	m_displayManager->UpdateVideoMixerDisplay();
}

void CInternalsFramework::ProcessUpdateEgmDisplaySettings( CInternalMessage *msg )
{
	if ( msg )
	{
		bool newValue = (bool) msg->GetWordData( false );
		m_mwConfig->SetMaintainAspectRatio(newValue, true);

		m_displayManager->SetMediaWindowConfig(*m_mwConfig);
		m_displayManager->UpdateVideoMixerDisplay();
	}
}

void CInternalsFramework::ProcessUpdatePlayerStateCloseSettings( CInternalMessage *msg )
{
	if ( msg )
	{
		m_mwConfig->SetCardOutPlayerState(m_mwConfig->GetMediaWindowPlayerState(msg->GetStringData()), true);
		m_mwConfig->SetFirstPlayPlayerState(m_mwConfig->GetMediaWindowPlayerState(msg->GetStringData(true)), true);

		m_displayManager->SetMediaWindowConfig(*m_mwConfig);
	}
}

void CInternalsFramework::ProcessUpdatePlayerStateOpenSettings(CInternalMessage *msg)
{
	if (msg != NULL)
	{
		m_mwConfig->SetCardInPlayerState(m_mwConfig->GetMediaWindowPlayerState(msg->GetStringData()), true);
		m_mwConfig->SetBonusEventPlayerState(m_mwConfig->GetMediaWindowPlayerState(msg->GetStringData(true)), true);

		m_displayManager->SetMediaWindowConfig(*m_mwConfig);
	}
}

void CInternalsFramework::ProcessUpdateAutoCloseSetting( CInternalMessage *msg )
{
	if ( msg )
	{
		byte autoValue = (byte) msg->GetWordData( false );
		WORD minutes = msg->GetWordData( true );
		m_mwConfig->SetAutoSwitch(autoValue, true);
		m_mwConfig->SetAutoSwitchWaitTime(minutes, true);

		m_displayManager->SetMediaWindowConfig(*m_mwConfig);
	}
}

void CInternalsFramework::ProcessEmployeeGoodbye()
{
	LogString(GENERAL, _T("DEBUG: Employee Goodbye pressed"));
	m_employeeGoodbyeRequested = true;
}

void CInternalsFramework::ProcessPlayerGoodbye()
{
	LogString(GENERAL, _T("DEBUG: Player Goodbye pressed"));
	SetPlayerGoodbyeRequested(true);
}

void CInternalsFramework::ProcessButtonActivity(CInternalMessage *msg)
{
	UNREFERENCED_PARAMETER(msg);
	ProcessEmployeeButtonActivity();
}

void CInternalsFramework::ProcessSetNewPinFromUI(CInternalMessage *msg)
{
	LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	bool isVirtualPlayer = m_player->IsVirtual();
	if (m_player != NULL && !isVirtualPlayer)
	{
		WORD newPin = msg->GetWordData();
		m_personalBanker->SetPlayerIdWaitingPinReset(m_player->GetPlayerInfo()->GetPlayerId());
		m_personalBanker->SetNewPin(newPin);
		m_personalBanker->ClearResetPinResult();  // clear the previous reset-pin result if any

		CInternalMessage *msg = PrepareTransactionMessage(INT_MSG_PB_SET_NEW_PIN, isVirtualPlayer);
		msg->SetData((WORD)newPin);
		msg->SetData(m_personalBanker->GetCurrentPin());
		int priority = QUEUE_PRIORITY_HIGH;
		SendMessageToSCF(msg, priority);
	}
}

void CInternalsFramework::ProcessUJPCheckPinFromUI(CInternalMessage *msg)
{
	lock_guard<recursive_mutex> lock(m_jackpotCriticalSection);

	CJackpot::LogInfo(m_jackpot, _T("Processing unattended jackpot check pin from UI"));
	if (m_jackpot != nullptr)
	{
		LogString(JACKPOT, _T("Unattended jackpot current state = %s"), m_jackpot->GetUnattendedStateStr());

		if (m_jackpot->GetUnattendedState() == UnattendedJackpotState::UJPPinEntry)
		{
			if (NULL != msg && m_player != NULL)
			{
				string pin(msg->GetStringData());
				DWORD federalTaxWithheld = msg->GetDWordData();
				DWORD stateTaxWithheld = msg->GetDWordData(true);
				bool federalTaxOverridden = (1 == msg->GetWordData());
				bool stateTaxOverridden = (1 == msg->GetWordData(true));

				m_jackpot->SetTaxWithholding(federalTaxWithheld, stateTaxWithheld, federalTaxOverridden, stateTaxOverridden);

				LogString(JACKPOT, "ProcessUJPCheckPinFromUI Pin=xxxx federalTaxWithheld=%u stateTaxWithheld=%u federalTaxOverridden=%u stateTaxOverridden=%u",
					federalTaxWithheld, stateTaxWithheld, federalTaxOverridden, stateTaxOverridden);

				m_jackpot->SetPlayerPin(pin);
				LogString(JACKPOT, _T("Unattended jackpot send check pin to poller"));

				// Send PIN to poller
				CInternalMessage *transMsg = PrepareTransactionMessage(INT_MSG_UJP_CHECK_PIN, IsPlayerVirtual());

				SetPlayerCardIDToUJPTransaction(transMsg);

				std::string::size_type sz;
				int intPin(std::stoi(pin, &sz));
				transMsg->SetData((WORD)intPin);
				m_jackpot->SetUnattendedState(UnattendedJackpotState::UJPPinSent);
				SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);
			}
		}
		else
		{
			// ignore this call from UI
			LogString(JACKPOT, "UTJ pin error scenario, so ignore this call.");
		}
	}

	CJackpot::LogInfo(m_jackpot, _T("ProcessUJPCheckPinFromUI done"));
}

void CInternalsFramework::ProcessUJPCallHost(bool closeMediaWindow)
{
	lock_guard<recursive_mutex> lock(m_jackpotCriticalSection);
	if (m_jackpot != nullptr && m_jackpot->IsUnattendedJackpotInProgress())
	{
		LogString(JACKPOT, "%s %d", __FUNCTION__, __LINE__);
		m_jackpot->SetUnattendedState(closeMediaWindow ? UnattendedJackpotState::UJPCanceledCloseMediaWindow : UnattendedJackpotState::UJPCanceledCallHost);
		SendUJPToPoller(new CJackpot(*m_jackpot));
	}
}

void CInternalsFramework::ProcessUJPPinpadTimeout()
{
	lock_guard<recursive_mutex> lock(m_jackpotCriticalSection);
	CJackpot::LogInfo(m_jackpot, _T("ProcessUJPPinpadTimeout enter"));

	if (m_jackpot != nullptr && UnattendedJackpotState::UJPPinEntry == m_jackpot->GetUnattendedState())
	{
		// UJP needs to be canceled
		m_jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledTimedOut);
		SendSubscriptionUpdateIfNecessary();
		SendUJPToPoller(new CJackpot(*m_jackpot));
	}

	CJackpot::LogInfo(m_jackpot, _T("ProcessUJPPinpadTimeout done"));
}

void CInternalsFramework::ProcessChangeEGMTypeFromUI(CInternalMessage * msg)
{
	if (msg != nullptr)
	{
		CEGMConfig *egmConfig = msg->GetEGMConfigData();
		if (egmConfig != nullptr)
		{
			// Send transaction if the machine type was changed.
			if (m_EGMConfig.GetEGMType() != egmConfig->GetEGMType())
			{
				SendConfigChangeTransactionToSCF(ccMachineType, m_EGMConfig.GetEGMType(), egmConfig->GetEGMType());
			}

			// Send transaction if protocol was changed.
			if (m_EGMConfig.GetEGMProtocol() != egmConfig->GetEGMProtocol())
			{
				SendConfigChangeTransactionToSCF(ccSerialProtocol, m_EGMConfig.GetEGMProtocol(), egmConfig->GetEGMProtocol());
			}

			m_EGMConfig.SetEGMType(egmConfig->GetEGMType());
			m_EGMConfig.SetEGMProtocol(egmConfig->GetEGMProtocol());
			m_EGMConfig.SetEGMPollingSpeed(egmConfig->GetEGMPollingSpeed());

			// If an EGM protocol is assigned but the protocol state is disabled due to not protocol set, start up the EGM protcol.
			if (m_EGMConfig.GetEGMProtocol() != NO_EGM_PROTOCOL && m_EGMConfig.GetEGMProtocolState() == protocolStateDisabled)
			{
				StartGCF();
			}

			// If a protcol is not assigned but we are currently running, shut down the EGM protocol. 
			else if (m_EGMConfig.GetEGMProtocol() == NO_EGM_PROTOCOL && m_EGMConfig.GetEGMProtocolState() == protocolStateEnabled)
			{
				m_EGMConfig.SetEGMProtocolState(protocolStateDisabled);
				m_config.SetEGMProtocolState(protocolStateDisabled);
				UninitializeGCF();
			}

			// Otherwise, send these settings to the GCF.
			else
			{
				SendEGMSettingsToGCF();
			}

			SendOneLinkProgressivesSupportToProgressive();
		}
	}
}

void CInternalsFramework::ProcessFlushPlayerUpdate(CInternalMessage *msg)
{
	int playerId = msg->GetIntData();
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	{
		// if the player update was recently sent with timed event, we may not flush now.
		if (NULL != m_player && (m_player->GetPlayerInfo()->GetPlayerId() == (DWORD)playerId) && m_playerUpdateManager.CheckIfPlayerBeFlushed(m_player->GetLastUpdateSentTime()))
		{
			LogString(GENERAL, "Flushing player update to poller");
			// Store a snapshot of the meters when the session is flushed.
			CMeters *sessionMeters = GetTransactionMeters(MEMORY_NVRAM, NVRAM_PLAYER_START_METERS_CURRENT_OFFSET);
			m_player->SetSessionStartMetersCurrent(sessionMeters);

			// Make a copy of meters for the player update transaction and send the transaction.
			CMeters *flushMeters = new CMeters(*sessionMeters);

			SendPlayerTransactionToSCF(PTT_UPDATE, flushMeters);

			// Reset current values of the session
			m_player->ResetCurrentSessionData();

			m_player->ResetLastUpdateSentTime(CUtilities::GetCurrentTime());
		}
	}
}

void CInternalsFramework::ProcessProbePBTFromUI(bool isTransferToGame)
{
	if (m_config.GetProbePBTLock())
	{
		CInternalMessage *gcfMsg = new CInternalMessage(INT_MSG_PROBE_PBT, (WORD)isTransferToGame);
		SendMessageToGCF(gcfMsg, QUEUE_PRIORITY_HIGHEST);
	}
	else
	{
		if (!m_mobilePBTInfo.IsMobileFundTransferRequest())
		{
			// return success back to UI to show requested screen
			CInternalMessage *uiMsg = new CInternalMessage(INT_MSG_PROBE_PBT_RESULT, (WORD)true /*ok to proceed */, (WORD)false /* no error */);
			SendMessageToUI(uiMsg, QUEUE_PRIORITY_HIGHEST);
		}
		if (m_pairingInformation.IsMobileCardedSession() && m_mobilePBTInfo.IsMobileFundTransferRequest())
		{
			HandlePBTProbeMobile();
		}
	}
}

void CInternalsFramework::ProcessPrizeLockClearFromUI()
{
	lock_guard<recursive_mutex> lock(m_egmAwardCriticalSection);
	int64_t jackpotId(0), clearedByOneLink(0);
	WORD enableEgm(1);
	if (m_egmAwardManager.IsPrizeAwardActive(jackpotId) && !m_egmAwardManager.UnlockInProgress())
	{
		LogString(PROGRESSIVE, _T("ProcessPrizeLockClearFromUI jackpotId=%lld clearedByOneLink=%lld"), jackpotId, clearedByOneLink);
		CInternalMessage* msg(new CInternalMessage(INT_MSG_EGM_STATE, jackpotId, clearedByOneLink));
		msg->SetData(enableEgm);
		ProcessEgmStateRequest(msg);
		delete msg;
	}
	else
	{
		LogString(PROGRESSIVE, _T("ProcessPrizeLockClearFromUI jackpotId=%lld is not active or already in progress."), jackpotId);
	}
}

void CInternalsFramework::ProcessUpdateUIConfigFromUI(CInternalMessage* msg)
{
	const CUIConfig *taskBarConfig(msg->GetUIConfig());
	if (taskBarConfig != nullptr)
	{
		m_notificationBox.SetUIConfig(*taskBarConfig);
		m_config.SetCustomButton1Enabled(taskBarConfig->GetCustomButton1Enabled());
		m_config.SetCustomButton2Enabled(taskBarConfig->GetCustomButton2Enabled());
		m_config.SetResortMenuButtonCount(taskBarConfig->GetResortMenuButtonCount());
		m_config.SetCustomMenuButton1Count(taskBarConfig->GetCustomMenuButton1Count());
		m_config.SetCustomMenuButton2Count(taskBarConfig->GetCustomMenuButton2Count());
	}
}

static UINT UpdateMixerThread(PVOID threadParam)
{
	auto internals(static_cast<CInternalsFramework*>(threadParam));

	CUtilities::SetThreadName(std::this_thread::get_id(), THREAD_UPGRADE_MIXER);
	CUtilities::ShowThreadId(_T(THREAD_UPGRADE_MIXER));

	try
	{
		CVideoMixerUpgrade* vmUpgrade(nullptr);
		CCarrierUpgrade* cUpgrade(nullptr);
		internals->FindFirmware(vmUpgrade, cUpgrade);

		//S5_Result result;
		bool upgradePerformed = false;

        LogString(FPGA_LOG, "UpdateMixerThread");

		// determine if something should be upgraded
		if ((CDisplayManager::Instance().GetDisplayPlatform() == VideoMixerPlatform) && vmUpgrade)
		{
			upgradePerformed = vmUpgrade->Upgrade();
		}

		// kill watchdog if an upgrade was performed, prevent video mixer reset
		if (upgradePerformed)
		{
			CVideoMixerUpgrade* tmpvmUpgrade(nullptr);
			CCarrierUpgrade* tmpcUpgrade(nullptr);
			string path = internals->FindFirmware(tmpvmUpgrade, tmpcUpgrade);
			if (((CDisplayManager::Instance().GetDisplayPlatform() != VideoMixerPlatform) || (tmpvmUpgrade && tmpvmUpgrade->IsCurrent())) &&
				(!tmpcUpgrade || tmpcUpgrade->IsCurrent()))
			{
				// Remove FPGA folder from mediadisk
				if (!path.empty() && path.find("sd") == std::string::npos)
				{
					CUtilities::DeleteDirectory(path);
				}

			}
			delete tmpvmUpgrade;
			delete tmpcUpgrade;
		}

		CUtilities::FlushVolume();

		CWatchDog::Instance().DeregisterThread();

		CDiagnostics::Instance().PersistReboot(tltRebootFirmUpgrade, "FLASHING");
		CWatchDog::Instance().BeginShutdown(CWebDiagnosticsStatusInfo::TiltCodeToString(tltRebootFirmUpgrade));
		delete vmUpgrade;
		delete cUpgrade;
	}
	catch (...)
	{
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_UPGRADE_MIXER);
	}

	return 0;
}
