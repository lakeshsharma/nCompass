#include "stdafx.h"
#include "ProcessDump/WebDiagnosticsStatusInfo.h"

// Sentinel includes.
#include "GlobalDefs.h"
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	static const DWORD CURRENT_MULTIPLIER = 23;
	static const DWORD CURRENT_DIVIDER = 34;
	
	class WebDiagnosticsStatusInfo : public ::testing::Test
	{
	public:
	};

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnosticsStatusInfo::CWebDiagnosticsStatusInfo()
		/// Code coverage for:
		///     CWebDiagnosticsStatusInfo::~CWebDiagnosticsStatusInfo()
		/// </summary>
		TEST(WebDiagnosticsStatusInfo, CWebDiagnosticsStatusInfoTest)
		{
			// Perform operation(s) to be tested.
			const CFlashStatus downloadStatus(FlashType::FLASH_NONE, 0, "", "", "", "");
			const CWebDiagnosticsStatusInfo statusInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND,
				true,
				1000000,
				true,
				CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03),
				-480,
				(TiltCode)0x900,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13),
				1,
				_T("20-6F-EC-00-08-09"),
				_T("20-6F-EC-00-08-08"),
				125,
				75,
				225,
				175,
				3000 * 100,
				7000 * 100,
				_T("Kyocera"),
				_T("Action"),
				_T("602"),
				true,
				false,
                CURRENT_MULTIPLIER,
                CURRENT_DIVIDER,
                2,
				1,
                0,
                false,
				EGMTransferMode_EFT,
				"None",
                2,
                3,
                false,
				downloadStatus);

			// Test(s).
			ASSERT_TRUE(statusInfo.m_currentTime == CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)) << _T("m_currentTime");
			ASSERT_TRUE(statusInfo.m_currentTickCount == (DWORD)(1000000 + (1 * SECONDS_PER_MINUTE + 2) * TICK_COUNTS_PER_SECOND)) << _T("m_currentTickCount");
			ASSERT_TRUE(statusInfo.m_tickCountOfLastFloorLogixTimeUpdateAvailable == true) << _T("m_tickCountOfLastFloorLogixTimeUpdateAvailable");
			ASSERT_TRUE(statusInfo.m_tickCountOfLastFloorLogixTimeUpdate == (DWORD)1000000) << _T("m_tickCountOfLastFloorLogixTimeUpdate");
			ASSERT_TRUE(statusInfo.m_timeOfLastFloorLogixConfigAvailable == true) << _T("m_timeOfLastFloorLogixConfigAvailable");
			ASSERT_TRUE(statusInfo.m_timeOfLastFloorLogixConfig == CUtilities::GetTimeFromFields(2014, 1, 27, 10, 01, 03)) << _T("m_timeOfLastFloorLogixConfig");
			ASSERT_TRUE(statusInfo.m_timeZoneOffsetInMinutes == (short)-480) << _T("m_timeZoneOffsetInMinutes");
			ASSERT_TRUE(statusInfo.m_tiltCodeForLastShutdown == (TiltCode)0x900) << _T("m_tiltCodeForLastShutdown");
			ASSERT_TRUE(statusInfo.m_lastUIRestartTime == CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13)) << _T("m_lastUIRestartTime");
			ASSERT_TRUE(statusInfo.m_uiRestartCount == (DWORD)1) << _T("m_uiRestartCount");
			ASSERT_TRUE(statusInfo.m_lastUIRefreshTime == CUtilities::GetTimeFromFields(2014, 1, 27, 9, 11, 13)) << _T("m_lastUIRefreshTime");
			ASSERT_TRUE(statusInfo.m_uiRefreshCount == (DWORD)1) << _T("m_uiRefreshCount");
			ASSERT_TRUE(statusInfo.m_macAddress == _T("20-6F-EC-00-08-09")) << _T("m_macAddress");
			ASSERT_TRUE(statusInfo.m_macAddress2 == _T("20-6F-EC-00-08-08")) << _T("m_macAddress2");
			ASSERT_TRUE(statusInfo.m_rebootAfterDropThreshold == 125) << _T("m_rebootAfterDropThreshold");
			ASSERT_TRUE(statusInfo.m_rebootASAPThreshold == 75) << _T("m_rebootASAPThreshold");
			ASSERT_TRUE(statusInfo.m_uiRestartAfterDropThreshold == 225) << _T("m_uiRestartAfterDropThreshold");
			ASSERT_TRUE(statusInfo.m_uiRestartAsapThreshold == 175) << _T("m_uiRestartAsapThreshold");
			ASSERT_TRUE(statusInfo.m_maxPBTAmount == 3000 * 100) << _T("m_maxPBTAmount");
			ASSERT_TRUE(statusInfo.m_maxTicketAmount == 7000 * 100) << _T("m_maxTicketAmount");
			ASSERT_TRUE(statusInfo.m_lcdModel == _T("Kyocera")) << _T("m_lcdModel");			
			ASSERT_TRUE(statusInfo.m_pendingAction == _T("Action")) << _T("m_pendingAction");
			ASSERT_TRUE(statusInfo.m_reportedEgmProtocolVersion == _T("602")) << _T("m_reportedEgmProtocolVersion");
			ASSERT_TRUE(statusInfo.m_egmSupportsLP7B_Available == true) << _T("m_egmSupportsLP7B_Available");
			ASSERT_TRUE(statusInfo.m_egmSupportsLP7B == false) << _T("m_egmSupportsLP7B");
			ASSERT_TRUE(statusInfo.m_currentMultiplier == CURRENT_MULTIPLIER) << _T("m_currentMultiplier");
			ASSERT_TRUE(statusInfo.m_currentDivider == CURRENT_DIVIDER) << _T("m_currentDivider");
			ASSERT_TRUE(statusInfo.m_numberSubgames == 2) << _T("m_numberSubgames");
			ASSERT_TRUE(statusInfo.m_numberSubgamesEnabled == 1) << _T("m_numberSubgamesEnabled");
			ASSERT_FALSE(statusInfo.m_isAllGamesFetched) << _T("m_isAllGamesFetched");
			ASSERT_FALSE(statusInfo.m_jackpotA8Reset) << _T("m_jackpotA8Reset");
			ASSERT_TRUE(statusInfo.m_egmTransferMode == EGMTransferMode_EFT) << _T("m_egmTransferMode");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnosticsStatusInfo::TiltCodeToString()
		/// </summary>
		TEST(WebDiagnosticsStatusInfo, TiltCodeToStringTest)
		{
			// Test(s).
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x8ff) == _T("N/A")) << _T("0x8ff");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x900) == _T("Unknown")) << _T("0x900");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x901) == _T("Employee")) << _T("0x901");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x902) == _T("Software Upgrade")) << _T("0x902");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x903) == _T("Firmware Upgrade")) << _T("0x903");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x904) == _T("Memory Clear")) << _T("0x904");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x905) == _T("Skin Change")) << _T("0x905");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x906) == _T("Display Change")) << _T("0x906");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x907) == _T("Low Memory Warning")) << _T("0x907");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x908) == _T("Low Memory")) << _T("0x908");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x909) == _T("Exception")) << _T("0x909");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x90a) == _T("Watchdog")) << _T("0x90a");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x90b) == _T("Memory Error")) << _T("0x90b");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x90c) == _T("Reboot Offline")) << _T("0x90c");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x90d) == _T("Remote Reboot")) << _T("0x90d");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x90e) == _T("UI Restart")) << _T("0x90e");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x90f) == _T("Remote Request")) << _T("0x90f");
			ASSERT_TRUE(CWebDiagnosticsStatusInfo::TiltCodeToString((TiltCode)0x910) == _T("Remote Pending")) << _T("0x910");
		}
}
