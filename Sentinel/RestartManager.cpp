#include "stdafx.h"
#include "RestartManager.h"
#include "Logging/Logger.h"
#include "Logging/LogString.h"
#include "DisplayManager.h"
#include "Hardware/WatchDog.h"
#include "Diagnostics.h"
#include "ProcessDump/WebDiagnosticsStatusInfo.h"

// If we do not want to do the status checking before UI RESTART for DEMO PURPOSES ONLY, Comment out the next line
#define UIRESTARTSTATUSCHECK

CRestartManager::CRestartManager() :
m_controlledRebootAfterNextDrop(false),
m_controlledRebootAsap(false),
m_controlledRemoteReboot(false),
m_controlledRebootForOsUpgrade(false),
m_shutdownInProcess(false),
m_heartbeatReceivedFromUI(false),
m_processingUiHeartbeats(false),
m_restartUiAfterNextDrop(false),
m_restartUiAsap(false),
m_refreshUiAsap(false),
m_restartUIAfterxDays(DEFAULT_RESTART_UI_AFTER_X_DAYS),
m_restartUIDelayHours(DEFAULT_RESTART_UI_DELAY_HOURS),
m_lastUiRestartTime(0),
m_uiHeartbeatReceived(CUtilities::GetTickCount()),
m_creditZeroTimeSpan(FOURMINUTESTIMESPAN),
m_minCreditZeroTimeSpan(MINUSONEMINUTETIMESPAN),
m_remoteRebootASAP(false),
m_firstTimeAfterRemoteRebootRequest(false),
m_sendRemoteRebootPendingTilt(false)
{
}

CRestartManager::~CRestartManager()
{
}

void CRestartManager::ReportThreadExceptionAndRequestAReboot(const std::string &threadName)
{
    if (!CWatchDog::Instance().IsShutdownInProcess())
	{
		std::cout << "ReportThreadExceptionAndRequestAReboot\n";
		//Cause shutdown.
		//We want to do this so we know if the app has an error.
		CDiagnostics::Instance().PersistReboot(tltRebootException, ERROR_LOG);

		CDisplayManager::Instance().PrepareDisplayForShutdown();

		CWatchDog::Instance().ReportThreadCallstackToDisk(threadName, false);

		CWatchDog::Instance().BeginShutdown(CWebDiagnosticsStatusInfo::TiltCodeToString(tltRebootException));
	}
}

bool CRestartManager::NeedToSendRebootWarningTransaction() const
{
	return !m_controlledRebootAfterNextDrop && !m_controlledRebootAsap;
}

bool CRestartManager::NeedToPerformControlledRebootAfterNextDrop() const
{
	return m_controlledRebootAfterNextDrop;
}

bool CRestartManager::NeedToPerformControlledRebootAsap() const
{
	return m_controlledRebootAsap;
}

bool CRestartManager::NeedToPerformControlledRemoteReboot() const
{
	return m_controlledRemoteReboot;
}

bool CRestartManager::IsShutdownInProcess() const
{
	return m_shutdownInProcess;
}

bool CRestartManager::NeedToPerformControlledRebootForOsUpgrade() const
{
	return m_controlledRebootForOsUpgrade;
}

bool CRestartManager::IsOkayToDoRebootOrUiRestart(const time_t& lastCreditMeterZeroTime,
	bool hasSignificantCredits,
	const time_t &utcTime) const
{
	time_t creditMeterZeroTimeSpan = utcTime - lastCreditMeterZeroTime;
	bool timeSpanOk(creditMeterZeroTimeSpan < m_minCreditZeroTimeSpan || creditMeterZeroTimeSpan > m_creditZeroTimeSpan);

	bool creditMeterOk(!hasSignificantCredits && timeSpanOk);
	LogString(GENERAL, _T("creditsOk=%d sigCredits=%d lastCreditsZero=%s utc=%s") // No comma.
		_T(" creditsZeroSeconds=%lld maxSeconds=%lld timeSpanOk=%d"),
		(int)creditMeterOk,
		(int)hasSignificantCredits,
		static_cast<LPCTSTR>(CUtilities::FormatDateTimeString(lastCreditMeterZeroTime).c_str()),
		static_cast<LPCTSTR>(CUtilities::FormatDateTimeString(utcTime).c_str()),
		(long long)creditMeterZeroTimeSpan,
		(long long)m_creditZeroTimeSpan,
		(int)timeSpanOk);

	LogString(UI_LOG, "IsOkayToDoRebootOrUiRestart-IsAbleToRestartUi=%u", IsAbleToRestartUi());

	return creditMeterOk;
}

void CRestartManager::PerformPostDropTasks()
{
	LogString(GENERAL, "Performing post-drop tasks");

	DWORD currentTickCount = CUtilities::GetTickCount();

	// If we are set to reboot after stacker insert, set the flag to do it as soon as we can
	if (NeedToPerformControlledRebootAfterNextDrop())
	{
		if (!NeedToPerformControlledRebootAsap())
		{
			LogString(GENERAL, "Setting flag to reboot ASAP");
		}
		SetToPerformControlledRebootAsap();
	}

	// If we are not about to reboot, this is a good time to restart the UI.
	if (!NeedToPerformControlledRebootAsap() && GetUiRestartAfterNextDropFlag())
	{
		LogString(GENERAL, "Setting flag to restart UI ASAP");

		SetPerformUiRestartAsapFlag(true);

		//Reset m_restartUiAfterNextDrop flag back to false
		SetUiRestartAfterNextDropFlag(false);
	}
}

void CRestartManager::SetToPerformRebootAfterNextDrop()
{
	m_controlledRebootAfterNextDrop = true;
}

void CRestartManager::SetToPerformControlledRebootAsap()
{
	m_controlledRebootAsap = true;
}

void CRestartManager::SetToPerformControlledRemoteReboot()
{
	m_controlledRemoteReboot = true;
}

void CRestartManager::SetToPerformControlledRebootForOsUpgrade()
{
	m_controlledRebootForOsUpgrade = true;
}

bool CRestartManager::PerformControlledUIRestartCheck(CPlayer* player, CEmployee* employee,
	const time_t& lastCreditMeterZeroTime, bool hasSignificantCredits, bool meterCaptureInProgress, bool uiRestartOverride)
{
	bool restartUi(false);
	bool okayToDoRestart(true);
	
	// Make sure there is no player card in, employee card-in, or we are not in the middle of a meter capture.
	if (player != NULL || employee != NULL || meterCaptureInProgress)
	{
#ifdef UIRESTARTSTATUSCHECK
		okayToDoRestart = false || uiRestartOverride;
#endif
	}

	if (okayToDoRestart)
	{
		okayToDoRestart = GetPerformUiRestartAsapFlag();
	}

	// Make sure that the credit meter has been zero for more than 4 minutes.
	if (okayToDoRestart)
	{
#ifdef UIRESTARTSTATUSCHECK
		okayToDoRestart =
			IsOkayToDoRebootOrUiRestart(lastCreditMeterZeroTime, hasSignificantCredits, CUtilities::GetUTCTime()) ||
			uiRestartOverride;
#endif
	}

	// If the UI is not responding then restart it no matter what.
	DWORD ticksSinceLastHeartbeat = CUtilities::GetTickCount() - m_uiHeartbeatReceived;
	if ((ticksSinceLastHeartbeat >= UINOHEARTBEATRESTART) ||
		(m_heartbeatReceivedFromUI &&
		(ticksSinceLastHeartbeat >= UINOHEARTBEATRUNNING) &&
		(CUtilities::GetTickCount() - m_lastUiRestartTime >= UINOHEARTBEATRESTART)))
	{
		if (m_processingUiHeartbeats)
		{
			LogString(ERROR_LOG,
				_T("%s SW lastHeartbeatTicks=%u haveHeartbeats=%d -- restarting Service Window"),
				static_cast<LPCTSTR>(__TFUNCTION__),
				(unsigned)ticksSinceLastHeartbeat,
				(int)m_heartbeatReceivedFromUI);
#ifndef DEBUG
			okayToDoRestart = true;
#endif
		}
	}

	if (okayToDoRestart)
	{
		DWORD ticksSinceLastRestart = CUtilities::GetTickCount() - m_lastUiRestartTime;
		LogString(_T("GENERAL"),
			_T("%s SW ticksSinceLastRestart=%u -- restarting Service Window"),
			static_cast<LPCTSTR>(__TFUNCTION__),
			(unsigned)ticksSinceLastRestart);

		SetPerformUiRestartAsapFlag(false);

		m_lastUiRestartTime = CUtilities::GetTickCount();

		// Reset UI keep alive to allow maximum duration before next timeout.
		// Reset after restarting ServiceWindowManager to avoid unwanted race condition with SetServiceWindowLastKeepAlive.
		m_uiHeartbeatReceived = CUtilities::GetTickCount();
		m_heartbeatReceivedFromUI = false;

		// Tell UI to send restart to ServiceWindowManager
		restartUi = true;
	}

	return restartUi;
}

bool CRestartManager::PerformControlledUIRefreshCheck(CPlayer* player, CEmployee* employee,
	const time_t& lastCreditMeterZeroTime, bool hasSignificantCredits, bool meterCaptureInProgress)
{
	bool refreshUI(false);
	bool okayToDoRefresh(true);

	// Make sure there is no player card in, employee card-in, or we are not in the middle of a meter capture.
	if (player != NULL || employee != NULL || meterCaptureInProgress)
	{
		okayToDoRefresh = false;
	}

	if (okayToDoRefresh)
	{
		okayToDoRefresh = GetPerformUiRefreshAsapFlag();
	}

	// Make sure that the credit meter has been zero for more than 4 minutes.
	if (okayToDoRefresh)
	{
		okayToDoRefresh = IsOkayToDoRebootOrUiRestart(lastCreditMeterZeroTime, hasSignificantCredits, CUtilities::GetUTCTime());
	}

	if (okayToDoRefresh)
	{
		LogString(GENERAL, "%s : refreshing UI", __FUNCTION__);
		SetPerformUiRefreshAsapFlag(false);
		refreshUI = true;
	}

	return refreshUI;
}

bool CRestartManager::PerformControlledRebootCheck(CPlayer* player, CEmployee* employee,
	const time_t& lastCreditMeterZeroTime, bool isEgmDoorOpen, bool hasSignificantCredits, 
	bool meterCaptureInProgress, bool rebootOverride, bool &isOSUpgrade)
{
	bool reboot(false);

	if (NeedToPerformControlledRebootForOsUpgrade() || NeedToPerformControlledRebootAsap() || NeedToPerformControlledRemoteReboot())
	{
		bool creditsOk(!hasSignificantCredits);
		bool okayToDoReboot(true);

		// Make sure there is no player card in, employee card-in, or we are not in the middle of a meter capture.
		if (player != NULL || employee != NULL || meterCaptureInProgress)
		{
			okayToDoReboot = false;
		}

		// Make sure the slot door is closed
		if (okayToDoReboot)
		{
			if (isEgmDoorOpen)
			{
				okayToDoReboot = false;	// Do not do reboot with slot door is open.
			}
		}

		if (okayToDoReboot || rebootOverride)
		{
			creditsOk =  rebootOverride ||
				IsOkayToDoRebootOrUiRestart(lastCreditMeterZeroTime, hasSignificantCredits, CUtilities::GetUTCTime());
			okayToDoReboot = creditsOk ||
							 (NeedToPerformControlledRebootForOsUpgrade() && rebootOverride);
		}

		// We are set to do the reboot.
		if (okayToDoReboot)
		{
			isOSUpgrade = NeedToPerformControlledRebootForOsUpgrade();
			m_shutdownInProcess = true;
			reboot = true;
		}
		
		// Send pending tilt for remote reboot
		if (IsFirstTimeAfterRemoteRebootRequest() && !okayToDoReboot)
		{
			SetSendRemoteRebootPendingTilt(true);
			SetFirstTimeAfterRemoteRebootRequest(false);
		}
		
		LogString(GENERAL, _T("rebootForUpgrade=%d rebootAsap=%d remoteReboot=%d player=%d employee=%d meterCapture=%d") // No comma.
			_T(" doorOpen=%d sigCredits=%d creditsOk=%d override=%d okayToReboot=%d shuttingDown=%d reboot=%d"),
			(int)NeedToPerformControlledRebootForOsUpgrade(),
			(int)NeedToPerformControlledRebootAsap(),
			(int)NeedToPerformControlledRemoteReboot(),
			(int)(player != nullptr),
			(int)(employee != nullptr),
			(int)meterCaptureInProgress,
			// doorOpen ...
			(int)isEgmDoorOpen,
			(int)hasSignificantCredits,
			(int)creditsOk,
			(int)rebootOverride,
			(int)okayToDoReboot,
			(int)m_shutdownInProcess,
			(int)reboot);
	}

	return reboot;
}

void CRestartManager::SetUiHeartbeatReceived()
{
	// heartbeat received from UI.
	m_uiHeartbeatReceived = CUtilities::GetTickCount();
	if (!m_heartbeatReceivedFromUI)
	{
		LogString(PBT_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
		LogString(SOCKETSERVER_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
		m_heartbeatReceivedFromUI = true;
	}
}

void CRestartManager::SetProcessingUiHeartbeats(bool processingUiHeartbeats)
{
	m_processingUiHeartbeats = processingUiHeartbeats;
}

void CRestartManager::SetRemoteRebootASAP(bool val)
{
	m_remoteRebootASAP = val;
}

bool CRestartManager::GetRemoteRebootASAP()
{
	return m_remoteRebootASAP;
}

void CRestartManager::SetFirstTimeAfterRemoteRebootRequest(bool val)
{
	m_firstTimeAfterRemoteRebootRequest = val;
}

bool CRestartManager::IsFirstTimeAfterRemoteRebootRequest()
{
	return m_firstTimeAfterRemoteRebootRequest;
}

void CRestartManager::SetSendRemoteRebootPendingTilt(bool val)
{
	m_sendRemoteRebootPendingTilt = val;
}
	
bool CRestartManager::IsSendRemoteRebootPendingTilt()
{
	return m_sendRemoteRebootPendingTilt;
}

bool CRestartManager::IsAbleToRestartUi() const
{	
	return ((CUtilities::GetTickCount() - m_lastUiRestartTime) >= (m_restartUIDelayHours * MILLISECONDS_PER_HOUR)) ||
		   (0 == m_lastUiRestartTime);
}

bool CRestartManager::IsRestartUiAfterxDays() const
{
	return ((CUtilities::GetTickCount() - m_lastUiRestartTime) / MILLISECONDS_PER_DAY) >= m_restartUIAfterxDays;
}

void CRestartManager::SetUiRestartAfterNextDropFlag(bool value)
{
	m_restartUiAfterNextDrop = value;
}

bool CRestartManager::GetUiRestartAfterNextDropFlag() const
{
	return m_restartUiAfterNextDrop;
}

void CRestartManager::SetPerformUiRestartAsapFlag(bool value)
{
	m_restartUiAsap = value;
}

bool CRestartManager::GetPerformUiRestartAsapFlag() const
{
	return m_restartUiAsap;
}

void CRestartManager::SetPerformUiRefreshAsapFlag(bool value)
{
	m_refreshUiAsap = value;
}

bool CRestartManager::GetPerformUiRefreshAsapFlag() const
{
	return m_refreshUiAsap;
}

void CRestartManager::SetRestartUIAfterxDays(DWORD value)
{
	m_restartUIAfterxDays = value;
}

void CRestartManager::SetRestartUIDelayHours(DWORD value)
{
	m_restartUIDelayHours = value;
}

bool CRestartManager::GetHeartbeatReceivedFromUI() const
{
	return m_heartbeatReceivedFromUI;
}