#pragma once

#include "Employee.h"
#include "Game.h"
#include "Player.h"
#include "InternalMessage.h"

// 4 Minutes
#define FOURMINUTESTIMESPAN (4 * SECONDS_PER_MINUTE)
#define MINUSONEMINUTETIMESPAN (-1 * SECONDS_PER_MINUTE)

const DWORD UINOHEARTBEATRUNNING = 100000; // In milliseconds.
const DWORD UINOHEARTBEATRESTART = 115000; // In milliseconds.

typedef enum
{
	REBOOT_NO_ACTIVE_SESSION = 0,
	REBOOT_ASAP = 1
} RemoteRebootType;

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class RestartManagerUnitTests; }

class CRestartManager
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CRestartManager"/> class.
	/// </summary>
	CRestartManager();

	/// <summary>
	/// Finalizes an instance of the <see cref="CRestartManager"/> class.
	/// </summary>
	~CRestartManager();

/// <summary>
	/// Returns the send reboot warning transaction flag.
	/// </summary>
	/// <returns>true if reboot warning transaction needs be sent, false otherwise.</returns>
	bool NeedToSendRebootWarningTransaction() const;

	/// <summary>
	/// Returns the perform controlled reboot after next drop flag.
	/// </summary>
	/// <returns>true if controlled reboot after next drop is needed, false otherwise.</returns>
	bool NeedToPerformControlledRebootAfterNextDrop() const;

	/// <summary>
	/// Returns the perform controlled reboot asap flag.
	/// </summary>
	/// <returns>true if controlled reboot asap is needed, false otherwise.</returns>
	bool NeedToPerformControlledRebootAsap() const;

	/// <summary>
	/// Returns the perform controlled remote reboot flag.
	/// </summary>
	/// <returns>true if controlled reboot asap is needed, false otherwise.</returns>
	bool NeedToPerformControlledRemoteReboot() const;

	/// <summary>
	/// Determines whether [is shutdown in process].
	/// </summary>
	/// <returns>true if shut down is in process, false otherwise.</returns>
	bool IsShutdownInProcess() const;

	/// <summary>
	/// Returns the perform controlled reboot for os upgrade flag.
	/// </summary>
	/// <returns></returns>
	bool NeedToPerformControlledRebootForOsUpgrade() const;

	/// <summary>
	/// Determines whether it is okay to do reboot or UI restart.
	/// </summary>
	/// <param name="lastCreditMeterZeroTime">The last credit meter zero time.</param>
	/// <param name="hasSignificantCredits">if set to <c>true</c> [has significant credits].</param>
	/// <param name="utcTime">The (current) UTC time.</param>
	/// <returns>true if okay to do reboot or UI restart, false otherwise.</returns>
	bool IsOkayToDoRebootOrUiRestart(const time_t& lastCreditMeterZeroTime,
		bool hasSignificantCredits,
		const time_t &utcTime) const;

	/// <summary>
	/// Performs the post drop tasks.
	/// </summary>
	void PerformPostDropTasks();

	/// <summary>
	/// Set to perform the reboot after next drop.
	/// </summary>
	void SetToPerformRebootAfterNextDrop();

	/// <summary>
	/// Set to perform controlled reboot asap.
	/// </summary>
	void SetToPerformControlledRebootAsap();

	/// <summary>
	/// Set varibale to perform controlled remote reboot.
	/// </summary>
	void SetToPerformControlledRemoteReboot();

	/// <summary>
	/// Sets to perform controlled reboot for os upgrade.
	/// </summary>
	void SetToPerformControlledRebootForOsUpgrade();

	/// <summary>
	/// Performs the controlled UI restart check.
	/// </summary>
	/// <param name="player">The player.</param>
	/// <param name="employee">The employee.</param>
	/// <param name="lastCreditMeterZeroTime">The last credit meter zero time.</param>
	/// <param name="hasSignificantCredits">if set to <c>true</c> [has significant credits].</param>
	/// <param name="meterCaptureInProgress">if set to <c>true</c> [meter capture in progress].</param>
	/// <param name="uiRestartOverride">if set to true, the UI will restart regardless no matter what.</param>
	/// <returns>true to restart UI, false otherwise.</returns>
	bool PerformControlledUIRestartCheck(CPlayer* player, CEmployee* employee,
		const time_t& lastCreditMeterZeroTime, bool hasSignificantCredits, bool meterCaptureInProgress, 
		bool uiRestartOverride);

	/// <summary>
	/// Performs the controlled UI refresh check.
	/// </summary>
	/// <param name="player">The player.</param>
	/// <param name="employee">The employee.</param>
	/// <param name="lastCreditMeterZeroTime">The last credit meter zero time.</param>
	/// <param name="hasSignificantCredits">if set to <c>true</c> [has significant credits].</param>
	/// <param name="meterCaptureInProgress">if set to <c>true</c> [meter capture in progress].</param>
	/// <returns>true to refresh UI, false otherwise.</returns>
	bool PerformControlledUIRefreshCheck(CPlayer* player, CEmployee* employee,
		const time_t& lastCreditMeterZeroTime, bool hasSignificantCredits, bool meterCaptureInProgress);

	/// <summary>
	/// Performs the controlled reboot check.
	/// </summary>
	/// <param name="game">The game.</param>
	/// <param name="player">The player.</param>
	/// <param name="employee">The employee.</param>
	/// <param name="lastCreditMeterZeroTime">The last credit meter zero time.</param>
	/// <param name="isEgmDoorOpen">if set to <c>true</c> [door is open].</param>
	/// <param name="hasSignificantCredits">if set to <c>true</c> [has significant credits].</param>
	/// <param name="meterCaptureInProgress">if set to <c>true</c> [meter capture in progress].</param>
	/// <param name="rebootOverride">if set to true, reboot will happen even if there are credits in the game.</param>
	/// <param name="isOSReboot">output variable indicates if reboot is for os upgrade</param>
	/// <returns>true to reboot, false otherwise.</returns>
	bool PerformControlledRebootCheck(CPlayer* player, CEmployee* employee, const time_t& lastCreditMeterZeroTime, 
		bool isEgmDoorOpen, bool hasSignificantCredits, bool meterCaptureInProgress, bool rebootOrUiRestartOverride, bool &isOSReboot);

	/// <summary>
	/// Sets the UI heartbeat received.
	/// </summary>
	void SetUiHeartbeatReceived();

	/// <summary>
	/// Sets the processing UI heartbeats.
	/// </summary>
	/// <param name="processingUiHeartbeats">if set to <c>true</c> [processing UI heartbeats].</param>
	void SetProcessingUiHeartbeats(bool processingUiHeartbeats);

    static void ReportThreadExceptionAndRequestAReboot(const std::string &threadName);
	
	/// <summary>
	/// Sets the remote reboot asap flag.
	/// </summary>
	/// <param name="val">set to true when reboot asap is rececived from FL</c>
	void SetRemoteRebootASAP(bool val);

	/// <summary>
	/// Get the remote reboot asap value.
	/// </summary>
	bool GetRemoteRebootASAP();

	/// <summary>
	/// Sets the firstTimeAfterRemoteRebootRequest flag.
	/// </summary>
	/// <param name="val">set to true when remote reboot NO active session is received.</c>
	void SetFirstTimeAfterRemoteRebootRequest(bool val);

	/// <summary>
	/// Get firstTimeAfterRemoteRebootRequest value.
	/// </summary>
	bool IsFirstTimeAfterRemoteRebootRequest();

	/// <summary>
	/// Sets the sendRemoteRebootPendingTilt flag.
	/// </summary>
	/// <param name="val">set to true when pending tilt for remote reboot is required to be sent.</c>
	void SetSendRemoteRebootPendingTilt(bool val);
	
	/// <summary>
	/// Get sendRemoteRebootPendingTilt value.
	/// </summary>
	bool IsSendRemoteRebootPendingTilt();

	/// <summary>
	/// Checks to see if is allowed to be restarted, nCompass doesn't allow UI restart unless it has been over a day.
	/// </summary>
	bool IsAbleToRestartUi() const;

	/// <summary>
	/// Checks to see if UI needs to be restarted after the drop regardless of memory.
	/// </summary>
	/// <returns>true if UI needs to restart after next drop, false otherwise.</returns>
	bool IsRestartUiAfterxDays() const;

	/// <summary>
	/// Set to perform the UI restart after next drop.
	/// </summary>
	/// <param name="value">true if UI needs to restart after next drop, false otherwise.</param>
	void SetUiRestartAfterNextDropFlag(bool value);

	/// <summary>
	/// Returns the restart UI after next drop flag.
	/// </summary>
	/// <returns>true if UI needs to restart after next drop, false otherwise.</returns>
	bool GetUiRestartAfterNextDropFlag() const;

	/// <summary>
	/// Set to perform UI restart asap flag.
	/// </summary>
	/// <param name="value">true if UI needs to restart ASAP, false otherwise.</param>
	void SetPerformUiRestartAsapFlag(bool value);

	/// <summary>
	/// Determines if it is okay to do UI restart, false otherwise.
	/// </summary>
	/// <returns>true if it is okay to do UI restart, false otherwise.</returns>
	bool GetPerformUiRestartAsapFlag() const;

	/// <summary>
	/// Set to perform UI refresh asap flag.
	/// </summary>
	/// <param name="value">true if UI needs to refresh ASAP, false otherwise.</param>
	void SetPerformUiRefreshAsapFlag(bool value);

	/// <summary>
	/// Determines if it is okay to do UI refresh, false otherwise.
	/// </summary>
	/// <returns>true if it is okay to do UI refresh, false otherwise.</returns>
	bool GetPerformUiRefreshAsapFlag() const;

	/// <summary>
	/// Sets the restart UI after x days value.
	/// </summary>
	/// <param name="value">restart UI after x days value</param>
	void SetRestartUIAfterxDays(DWORD value);

	/// <summary>
	/// Sets the restart UI delay hours.
	/// </summary>
	/// <param name="value">restart UI delay hours</param>
	void SetRestartUIDelayHours(DWORD value);

	/// <summary>
	/// Checks if heartbeat from UI has been received.
	/// </summary>
	/// <returns>true if heartbeat from UI has been received, false otherwise.</returns>
	bool GetHeartbeatReceivedFromUI() const;

private:
	bool m_controlledRebootAfterNextDrop;
	bool m_controlledRebootAsap;
	bool m_controlledRemoteReboot;
	bool m_controlledRebootForOsUpgrade;
	bool m_shutdownInProcess;
	bool m_heartbeatReceivedFromUI;
	volatile bool m_processingUiHeartbeats; // Is processing enabled, regardless of receiving them.
	bool m_restartUiAfterNextDrop;
	bool m_restartUiAsap;
	bool m_refreshUiAsap;
	DWORD m_restartUIAfterxDays;
	DWORD m_restartUIDelayHours;	

	DWORD m_lastUiRestartTime;
	DWORD m_uiHeartbeatReceived;

	time_t m_creditZeroTimeSpan; // Maximum for no timeout.
	time_t m_minCreditZeroTimeSpan; // Minimum for no timeout.

	bool m_remoteRebootASAP; // for remote reboot
	bool m_firstTimeAfterRemoteRebootRequest; // for remote reboot no player session
	bool m_sendRemoteRebootPendingTilt;
	
	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::RestartManagerUnitTests;
	FRIEND_TEST(RestartManagerUnitTests, RestartManagerConstructorUnitTest);
	FRIEND_TEST(RestartManagerUnitTests, ShutdownInProcessUnitTest);
	FRIEND_TEST(RestartManagerUnitTests, IsOkayToDoRebootOrUiRestartUnitTest);
	FRIEND_TEST(RestartManagerUnitTests, IsOkayToDoRebootOrUiRestartUnitTest_Default);
	FRIEND_TEST(RestartManagerUnitTests, IsOkayToDoRebootOrUiRestartUnitTest_Min);
	FRIEND_TEST(RestartManagerUnitTests, IsOkayToDoRebootOrUiRestartUnitTest_Max);
	FRIEND_TEST(RestartManagerUnitTests, PerformPostDropRebootUnitTest);
	FRIEND_TEST(RestartManagerUnitTests, PerformPostDropUiRestartUnitTest);
	FRIEND_TEST(RestartManagerUnitTests, PerformControlledRebootAsapCheckRebootOverrideFalseUnitTest);
	FRIEND_TEST(RestartManagerUnitTests, PerformControlledRebootAsapRebootCheckOverrideTrueUnitTest);
	FRIEND_TEST(RestartManagerUnitTests, PerformControlledRebootAsapForOsUpgradeCheckOverrideTrueUnitTest);
	FRIEND_TEST(RestartManagerUnitTests, PerformControlledRebootAsapCheckForOsUpgradeOverrideFalseUnitTest);
	FRIEND_TEST(RestartManagerUnitTests, PerformControlledUiRestartCheckOverrideFalseUnitTest);
	FRIEND_TEST(RestartManagerUnitTests, PerformControlledUiRestartCheckOverrideTrueUnitTest);
	FRIEND_TEST(RestartManagerUnitTests, PerformControlledRemoteRebootCheckOverrideFalseUnitTest);
	FRIEND_TEST(RestartManagerUnitTests, PerformControlledRemoteRebootCheckOverrideTrueUnitTest);
};

