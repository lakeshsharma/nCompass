#include "stdafx.h"
#include "ProcessDump/WebDiagnosticsStatusInfo.h"

using namespace std;

CWebDiagnosticsStatusInfo::CWebDiagnosticsStatusInfo(time_t currentTime,
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
	const string& bonusPaymentMethod,
	WORD currentGameChunkIndex,
	WORD totalGameListChunks,
	bool jackpotA8Reset,
	const CFlashStatus &downloadStatus,
	bool cashoutToHostEnabled,
	byte cashoutToHostStatusOnEGM,
	DWORD cashoutToHostRequestsAccepted,
	DWORD cashoutToHostRequestsCanceled,
	bool maintenanceMode):
	m_currentTime(currentTime),
	m_currentTickCount(currentTickCount),
	m_tickCountOfLastFloorLogixTimeUpdateAvailable(tickCountOfLastFloorLogixTimeUpdateAvailable),
	m_tickCountOfLastFloorLogixTimeUpdate(tickCountOfLastFloorLogixTimeUpdate),
	m_timeOfLastFloorLogixConfigAvailable(timeOfLastFloorLogixConfigAvailable),
	m_timeOfLastFloorLogixConfig(timeOfLastFloorLogixConfig),
	m_timeZoneOffsetInMinutes(timeZoneOffsetInMinutes),
	m_tiltCodeForLastShutdown(tiltCodeForLastShutdown),
	m_lastUIRestartTime(lastUIRestartTime),
	m_uiRestartCount(uiRestartCount),
	m_lastUIRefreshTime(lastUIRefreshTime),
	m_uiRefreshCount(uiRefreshCount),
	m_macAddress(macAddress),
	m_macAddress2(macAddress2),
	m_rebootAfterDropThreshold(rebootAfterDropThreshold),
	m_rebootASAPThreshold(rebootASAPThreshold),
	m_uiRestartAfterDropThreshold(uiRestartAfterDropThreshold),
	m_uiRestartAsapThreshold(uiRestartAsapThreshold),
	m_maxPBTAmount(maxPBTAmount),
	m_maxTicketAmount(maxTicketAmount),
	m_lcdModel(lcdModel),
	m_pendingAction(pendingAction),
	m_reportedEgmProtocolVersion(reportedEgmProtocolVersion),
	m_egmSupportsLP7B_Available(egmSupportsLP7B_Available),
	m_egmSupportsLP7B(egmSupportsLP7B),
    m_currentMultiplier(currentMultiplier),
    m_currentDivider(currentDivider),    
	m_numberSubgames(numberSubgames),
	m_numberSubgamesEnabled(numberSubgamesEnabled),
	m_lastGamelistTimestamp(lastGamelistTimestamp) ,
	m_isAllGamesFetched(isAllGamesFetched),
	m_egmTransferMode(egmTransferMode),
	m_bonusPaymentMethod(bonusPaymentMethod),
	m_currentGameChunkIndex(currentGameChunkIndex),
	m_totalGameListChunks(totalGameListChunks),
	m_jackpotA8Reset(jackpotA8Reset),
	m_downloadStatus(downloadStatus),
	m_cashoutToHostEnabled(cashoutToHostEnabled),
	m_cashoutToHostStatusOnEGM(cashoutToHostStatusOnEGM),
	m_cashoutToHostRequestsAccepted(cashoutToHostRequestsAccepted),
	m_cashoutToHostRequestsCanceled(cashoutToHostRequestsCanceled),
	m_maintenanceMode(maintenanceMode)
{
}

CWebDiagnosticsStatusInfo::~CWebDiagnosticsStatusInfo(void)
{
}

std::string CWebDiagnosticsStatusInfo::TiltCodeToString(TiltCode code)
{
	typedef struct
	{
		TiltCode m_code;
		LPCTSTR m_string;
	} CodeString;

	static CodeString CodeStringArray[] = {
		{ tltRebootUnknown, _T("Unknown") },
		{ tltRebootButton, _T("Employee") },
		{ tltRebootSoftUpgrade, _T("Software Upgrade") },
		{ tltRebootFirmUpgrade, _T("Firmware Upgrade") },
		{ tltRebootMemoryClear, _T("Memory Clear") },
		{ tltRebootSkinChange, _T("Skin Change") },
		{ tltRebootDispChange, _T("Display Change") },
		{ tltRebootWarning, _T("Low Memory Warning") },
		{ tltRebootAsap, _T("Low Memory") },
		{ tltRebootException, _T("Exception") },
		{ tltRebootThreadTimeout, _T("Watchdog") },
		{ tltRebootMemoryError, _T("Memory Error") },
		{ tltRebootOffline, _T("Reboot Offline") },
		{ tltRebootRemote, _T("Remote Reboot") },
		{ tltRebootUiRestart, _T("UI Restart") },
		{ tltRebootRemoteRequest, _T("Remote Request") },
		{ tltRebootRemotePending, _T("Remote Pending") },
	};

	static_assert((int)(tltRebootLastCode - tltRebootUnknown) == _countof(CodeStringArray),
		"CodeStringArray has to be updated to include all the reboot TiltCodes");

	string tiltString(_T("N/A"));

	for (size_t index = 0; index < _countof(CodeStringArray); ++index)
	{
		if (CodeStringArray[index].m_code == code)
		{
			tiltString = CodeStringArray[index].m_string;
			break; // Out of "for" loop.
		}
	}

	return tiltString;
}

