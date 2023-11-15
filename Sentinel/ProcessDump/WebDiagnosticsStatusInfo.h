#pragma once
#include "TiltsDef.h"
#include "GlobalDefs.h"
#include "FlashStatus.h"

union u_HostCashoutStatus
{
	// first representation (member of union) 
	struct tHostCashoutStatus
	{
		bool m_cashoutControlledByHost : 1; //Bit0
		bool m_cashoutEnabled : 1; //Bit1
		bool m_cashoutModeHard : 1; //Bit2
		byte m_unused : 5; //Bit 3-7  
	} m_hostCashoutStatus;

	// second representation (member of union) 
	byte m_uValue;

	u_HostCashoutStatus(byte status)
	{
		m_uValue = status;
	}
};

/// <summary>
/// Status info for display on web diagnostics pages.
/// Keep strings in wide characters until the last possible moment.
/// </summary>
class CWebDiagnosticsStatusInfo
{
public:
	/// <summary>
	/// Constructor. Initializes a new instance of the <see cref="CWebDiagnosticsStatusInfo"/> class.
	/// </summary>
	/// <param name="currentTime">IN - The current time.</param>
	/// <param name="currentTickCount">IN - The current tick count.</param>
	/// <param name="tickCountOfLastFloorLogixTimeUpdateAvailable">IN - Is the tick count of the last floor logix time update available?</param>
	/// <param name="tickCountOfLastFloorLogixTimeUpdate">IN - The tick count of the last date/time update from floor logix.</param>
	/// <param name="timeOfLastFloorLogixConfigAvailable">IN - Is the time of the last floor logix configuration available?</param>
	/// <param name="timeOfLastFloorLogixConfig">IN - The time of the last (full) configuration received from loor logix.</param>
	/// <param name="timeZoneOffsetInMinutes">IN - The time zone offset in minutes.</param>
	/// <param name="tiltCodeForLastShutdown">The tilt code for the last shutdown.</param>
	/// <param name="lastUIRestartTime">IN - local time that the UI was last restarted.</param>
	/// <param name="uiRestartCount">IN - Number of UI restarts.</param>
	/// <param name="lastUIRefreshTime">IN - local time that the UI was last refreshed.</param>
	/// <param name="uiRefreshCount">IN - Number of UI refreshes.</param>
	/// <param name="macAddress">IN - (primary) MAC address of Sentinel.</param>
	/// <param name="macAddress2">IN - (secondary) MAC address of Sentinel.</param>
	/// <param name="rebootAfterDropThreshold">IN - The threshold used to determine if nCompass should reboot after a drop.</param>
	/// <param name="rebootASAPThreshold">IN - The threshold used to determine if nCompass should reboot after as soon as practical.</param>
	/// <param name="uiRestartAfterDropThreshold">IN - The threshold used to determine if nCompass should restart the UI after the next drop.</param>	
	/// <param name="uiRestartAsapThreshold">IN - The threshold used to determine if nCompass should restart the UI ASAP.</param>	
	/// <param name="maxPBTAmount">IN - The maximum PBT amount.</param>
	/// <param name="maxTicketAmount">IN - The maximum ticket amount.</param>
	/// <param name="lcdModel">IN - LCD Model inserted.</param>
	/// <param name="pendingAction">IN - The pending action.</param>
	/// <param name="reportedEgmProtocolVersion">IN - The reported EGM protocol version.</param>
	/// <param name="egmSupportsLP7B_Available">IN - Sets whether the EGM supports long poll 7B information is available.</param>
	/// <param name="egmSupportsLP7B">IN - Sets whether the EGM supports long poll 7B.</param>
	/// <param name="currentMultiplier">IN - the current multiplier.</param>
	/// <param name="currentDivider">IN - the current divider.</param>
	/// <param name="numberSubgames">IN - number of subgames.</param>
	/// <param name="numberSubgamesEnabled">IN - number of subgames enabled.</param>
	/// <param name="lastGamelistTimestamp">IN - last game time stamp.</param>
	/// <param name="isAllGamesFetched">IN - all games fetched or not.</param>
	/// <param name="egmTransferMode">IN - EGM transfer mode.</param>
	/// <param name="bonusPaymentMethod">IN - bonus payment method.</param>
	/// <param name="currentGameChunkIndex">IN - current game chunk index.</param>
	/// <param name="totalGameListChunks">IN - total game list chunks.</param>
	/// <param name="jackpotA8Reset">IN - Display jackpot A8 reset status.</param>
	/// <param name="downloadStatus">IN - download status.</param>
	/// <param name="cashoutToHostEnabled">IN - cashout to host enabled from FloorLogix.</param>
	/// <param name="cashoutToHostStatusOnEGM">IN - cashout to host status on EGM.</param>
	/// <param name="cashoutToHostRequestsAccepted">IN - cashout to host requests accepted.</param>
	/// <param name="cashoutToHostRequestsCanceled">IN - cashout to host requests canceled.</param>
	/// <param name="maintenanceMode">IN - maintenance mode.</param>
	CWebDiagnosticsStatusInfo(time_t currentTime,
		DWORD currentTickCount,
		bool tickCountOfLastFloorLogixTimeUpdateAvailable,
		DWORD tickCountOfLastFloorLogixTimeUpdate,
		bool timeOfLastFloorLogixConfigAvailable,
		time_t timeOfLastFloorLogixConfig,
		short timeZoneOffsetInMinutes,
		TiltCode tiltCodeForLastShutdown,
		time_t lastUIRestartTime,
		DWORD uiRestartCount,
		time_t lastUIRefreshTime,
		DWORD uiRefreshCount,
		const std::string &macAddress,
		const std::string &macAddress2,
		DWORD rebootAfterDropThreshold,
		DWORD rebootASAPThreshold,
		DWORD uiRestartAfterDropThreshold,
		DWORD uiRestartAsapThreshold,
		long maxPBTAmount,
		long maxTicketAmount,
		const std::string &lcdModel,
		const std::string &pendingAction,
		const std::string &reportedEgmProtocolVersion,
		bool egmSupportsLP7B_Available,
		bool egmSupportsLP7B,
		DWORD currentMultiplier,
		DWORD currentDivider, 
		DWORD numberSubgames,
		DWORD numberSubgamesEnabled,
		time_t lastGamelistTimestamp,
		bool isAllGamesFetched,
		EGMTransferMode egmTransferMode,
		const std::string& bonusPaymentMethod,
		WORD currentGameChunkIndex,
		WORD totalGameListChunks,
		bool jackpotA8Reset,
		const CFlashStatus &downloadStatus,
		bool cashoutToHostEnabled = false,
		byte cashoutToHostStatusOnEGM = 0,
		DWORD cashoutToHostRequestsAccepted = 0,
		DWORD cashoutToHostRequestsCanceled = 0,
		bool maintenanceMode = false);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CWebDiagnosticsStatusInfo"/> class.
	/// </summary>
	~CWebDiagnosticsStatusInfo(void);

	static std::string TiltCodeToString(TiltCode code);

	time_t m_currentTime;
	DWORD m_currentTickCount;
	bool m_tickCountOfLastFloorLogixTimeUpdateAvailable;
	DWORD m_tickCountOfLastFloorLogixTimeUpdate;
	bool m_timeOfLastFloorLogixConfigAvailable;
	time_t m_timeOfLastFloorLogixConfig;
	short m_timeZoneOffsetInMinutes;
	TiltCode m_tiltCodeForLastShutdown;
	time_t m_lastUIRestartTime;
	DWORD m_uiRestartCount; // Restarts.  Does not include first start.
	time_t m_lastUIRefreshTime;
	DWORD m_uiRefreshCount;
	std::string m_macAddress;
	std::string m_macAddress2;
	DWORD m_rebootAfterDropThreshold;
	DWORD m_rebootASAPThreshold;
	DWORD m_uiRestartAfterDropThreshold;
	DWORD m_uiRestartAsapThreshold;
	long m_maxPBTAmount;
	long m_maxTicketAmount;
	std::string m_lcdModel;
	std::string m_pendingAction;
	std::string m_reportedEgmProtocolVersion;
	bool m_egmSupportsLP7B_Available; // Set true if m_egmSupportsLP7B is available, set false otherwise.
	bool m_egmSupportsLP7B;
	DWORD m_currentMultiplier;
	DWORD m_currentDivider;
	DWORD m_numberSubgames;
	DWORD m_numberSubgamesEnabled;
	time_t m_lastGamelistTimestamp;
	bool m_isAllGamesFetched;
	EGMTransferMode m_egmTransferMode;
	std::string m_bonusPaymentMethod;
	WORD m_currentGameChunkIndex;
	WORD m_totalGameListChunks;
	bool m_jackpotA8Reset;
	CFlashStatus m_downloadStatus;
	bool m_cashoutToHostEnabled;
	u_HostCashoutStatus m_cashoutToHostStatusOnEGM;
	DWORD m_cashoutToHostRequestsAccepted;
	DWORD m_cashoutToHostRequestsCanceled;
	bool m_maintenanceMode;

private:
	// Disable default constructor, default copy constructor, and default assignment operator.
	CWebDiagnosticsStatusInfo();
	CWebDiagnosticsStatusInfo(const CWebDiagnosticsStatusInfo &rhs);
	CWebDiagnosticsStatusInfo& operator=(const CWebDiagnosticsStatusInfo &rhs);
};
