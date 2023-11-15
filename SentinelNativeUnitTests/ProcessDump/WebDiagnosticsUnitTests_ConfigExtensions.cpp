#if 000//TODO-PORT
#include "stdafx.h"
#include "ProcessDump/WebDiagnostics.h"
#include "Config/ConfigExtFile.h"
#include "UnitTestingMemFile.h"
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingUtils.h"
#include "UnitTestingXmlData.h"
#include "UnitTestingXmlLite.h"

using namespace std;

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult5(result, resultLength, expected, expectedLength, resultName) \
	CUnitTestingUtils::AssertEqual(result, resultLength, expected, expectedLength, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult4(result, resultLength, expected, expectedLength) \
	AssertResult5(result, resultLength, expected, expectedLength, _T(#result))

#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

#define AssertResult4f(result, expected, expectedLength, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, expectedLength, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult3f(result, expected, expectedLength) AssertResult4f(result, expected, expectedLength, _T(#result))

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	/// <summary>
	/// Test web diagnostics related to CConfigExtFile.
	/// The GetConfigExtHtml...Test and SaveConfigExtData...Test methods are provided for ease of troubleshooting.
	/// The UpdateConfigExt...Test methods are sufficient for code coverage and functional coverage.
	/// </summary>
	TEST_CLASS(WebDiagnostics_ConfigExt)
	{
		// Class member constant(s).
		static const LPCTSTR CONFIG_EXTENSIONS_FILENAME; // When class under test choses the filename.
		static const LPCTSTR ALT_CONFIG_EXTENSIONS_FILENAME; // When we can choose any filename.
		static const int CONFIG_EXTENSIONS_REFRESH_SECONDS = 60;

		// Class member variable(s).
		int m_testIndex;

		// Web content simulator.  Compare actual results with simulator results.
		CString CalcPageHeaderHtml(int refreshSeconds)
		{
			CString html;
			html.Format(L"<html><META Http-equiv=\"refresh\" Content=\"%d\">", (int)refreshSeconds);

			return html;
		}

		// Web content simulator.  Compare actual results with simulator results.
		CString CalcPageTrailerHtml()
		{
			return _T("</html>");
		}

		// Web content simulator.  Compare actual results with simulator results.
		CString CalcCommonHeaderHtml(const CWebDiagnosticsCommonInfo &info)
		{
			CString html;
			html.AppendFormat(_T("Startup Time: %s<br>"), static_cast<LPCTSTR>(info.m_startupTime.Format(L"%m/%d/%Y %I:%M:%S %p")));
			html.AppendFormat(_T("Current Time: %s<br>"), static_cast<LPCTSTR>(info.m_currentTime.Format(L"%m/%d/%Y %I:%M:%S %p")));
			html.AppendFormat(_T("SlotMast ID: %ld<br>"), (long)info.m_slotMastID);
			html.AppendFormat(_T("Machine Number:%d<br>"), (long)info.m_machineNumber); // No space after ':' for backwards compatibility.
			html.AppendFormat(_T("Sentinel Version: %s<br>"), static_cast<LPCTSTR>(info.m_sentinelVersion));
			html.AppendFormat(_T("OS Image Version: %s<br>"), static_cast<LPCTSTR>(info.m_osImageVersion));
			html.Append(_T("<br>"));

			return html;
		}

		// Web content simulator.  Compare actual results with simulator results.
		CString CalcConfigExt1Html(const CConfigExtFile &configExt)
		{
			CString html;

			{
				WORD negligibleCreditsThreshold;
				configExt.GetNegligibleCreditsThreshold(negligibleCreditsThreshold);
				html.AppendFormat(L"Negligible Credits Threshold: %s<br>",
					static_cast<LPCTSTR>(CUnitTestingUtils::GetFormattedCashString((DWORD)negligibleCreditsThreshold)));
			}

			{
				bool employeeOverPlayerIdleCardOut;
				configExt.GetEmployeeOverPlayerIdleCardOut(employeeOverPlayerIdleCardOut);
				html.AppendFormat(L"Employee Over Player Idle Card Out: %s<br>",
					static_cast<LPCTSTR>(CUnitTestingUtils::GetBoolString(employeeOverPlayerIdleCardOut)));
			}

			{
				WORD employeeIdleCardOutSeconds;
				configExt.GetEmployeeIdleCardOutSeconds(employeeIdleCardOutSeconds);
				html.AppendFormat(L"Employee Idle Card Out Seconds: %u<br>", (unsigned)employeeIdleCardOutSeconds);
			}

			{
				bool playerRecardEmployeeCardOut;
				configExt.GetPlayerRecardEmployeeCardOut(playerRecardEmployeeCardOut);
				html.AppendFormat(L"Player Recard Employee Card Out: %s<br>",
					static_cast<LPCTSTR>(CUnitTestingUtils::GetBoolString(playerRecardEmployeeCardOut)));
			}

			html.Append(_T("<br>"));

			{
				WORD sessionActiveNegligibleCreditsTimeoutSeconds;
				configExt.GetSessionActiveNegligibleCreditsTimeoutSeconds(sessionActiveNegligibleCreditsTimeoutSeconds);
				html.AppendFormat(L"Session Active Negligible Credits Timeout: %u seconds<br>", (unsigned) sessionActiveNegligibleCreditsTimeoutSeconds);
			}

			{
				WORD sessionActiveNonNegligibleCreditsTimeoutSeconds;
				configExt.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(sessionActiveNonNegligibleCreditsTimeoutSeconds);
				html.AppendFormat(L"Session Active Non-negligible Credits Timeout: %u seconds<br>", (unsigned) sessionActiveNonNegligibleCreditsTimeoutSeconds);
			}

			html.Append(_T("<br>"));

			return html;
		}

		// Web content simulator.  Compare actual results with simulator results.
		CString CalcConfigExt2Html(const CConfigExtFile &configExt)
		{
			CString html;
			
			{
				WORD notificationForShowPromotionTime;
				configExt.GetNotificationForShowPromotionTime(notificationForShowPromotionTime);
				html.AppendFormat(L"Promotion Notification Seconds: %u<br>", (unsigned)notificationForShowPromotionTime);
			}

			{
				WORD notificationForPlayerTrackingTime;
				configExt.GetNotificationForPlayerTrackingTime(notificationForPlayerTrackingTime);
				html.AppendFormat(L"Player Tracking Notification Seconds: %u<br>", (unsigned)notificationForPlayerTrackingTime);
			}

			html.Append(_T("<br>"));

			{
				bool customButton1Enabled;
				configExt.GetCustomButton1Enabled(customButton1Enabled);
				html.AppendFormat(L"Custom Button 1: %s<br>",
					static_cast<LPCTSTR>(CUnitTestingUtils::GetEnabledDisabledString(customButton1Enabled)));
			}

			{
				WORD customMenuButton1Count;
				configExt.GetCustomMenuButton1Count(customMenuButton1Count);
				html.AppendFormat(L"Custom 1 Menu Items: %u<br>", (unsigned)customMenuButton1Count);
			}

			{
				bool customButton2Enabled;
				configExt.GetCustomButton2Enabled(customButton2Enabled);
				html.AppendFormat(L"Custom Button 2: %s<br>",
					static_cast<LPCTSTR>(CUnitTestingUtils::GetEnabledDisabledString(customButton2Enabled)));
			}

			{
				WORD customMenuButton2Count;
				configExt.GetCustomMenuButton2Count(customMenuButton2Count);
				html.AppendFormat(L"Custom 2 Menu Items: %u<br>", (unsigned)customMenuButton2Count);
			}

			{
				WORD resortMenuButtonCount;
				configExt.GetResortMenuButtonCount(resortMenuButtonCount);
				html.AppendFormat(L"Resort Menu Items: %u<br>", (unsigned)resortMenuButtonCount);
			}

			html.Append(_T("<br>"));

			{
				bool rebootSwfRestartOverride;
				configExt.GetRebootSwfRestartOverride(rebootSwfRestartOverride);
				html.AppendFormat(L"Reboot SWF Restart Override: %s<br>",
					static_cast<LPCTSTR>(CUnitTestingUtils::GetEnabledDisabledString(rebootSwfRestartOverride)));
			}

			html.Append(_T("<br>"));

			return html;
		}

		/// <summary>
		/// Web content simulator for the configuration extensions info.
		/// Compare actual results with simulator results.
		/// </summary>
		/// <param name="info">IN - The web diagnostics common information.</param>
		/// <param name="configExt">IN - The configuration extensions.</param>
		/// <returns>A string with the expected content for the configuration extensions info.</returns>
		CString CalcConfigExtHtml(const CConfigExtFile &configExt)
		{
			CString html;
			html += CalcConfigExt1Html(configExt);
			html += CalcConfigExt2Html(configExt);

			return html;
		}

		/// <summary>
		/// Web content simulator for the entire configuration extensions page.
		/// Compare actual results with simulator results.
		/// </summary>
		/// <param name="info">IN - The web diagnostics common information.</param>
		/// <param name="configExt">IN - The configuration extensions.</param>
		/// <returns>A string with the expected content for the entire configuration extensions page.</returns>
		CString CalcConfigExtPage(const CWebDiagnosticsCommonInfo &commonInfo, const CConfigExtFile &configExt)
		{
			CString html;
			html += CalcPageHeaderHtml(CONFIG_EXTENSIONS_REFRESH_SECONDS);
			html += CalcCommonHeaderHtml(commonInfo);
			html += CalcConfigExtHtml(configExt);
			html += CalcPageTrailerHtml();

			return html;
		}

		/// <summary>
		/// Gets web diagnostics common information, initialized for general use.
		/// </summary>
		/// <param name="seed">IN - A seed used to vary the information provided.</param>
		/// <returns>A pointer to a new CWebDiagnosticsCommonInfo instance.</returns>
		/// <remarks>
		/// The caller is responsible for deleting the instance.
		/// </remarks>
		CWebDiagnosticsCommonInfo *GetWebDiagnosticsCommonInfo(int seed)
		{
			seed &= 0x7fff; // Force seed to have a small non-negative value.

			const bool hasSpeedMediaInfo((bool)((seed >> 0) & 1));
			const CSpeedMediaInfo speedMediaInfo;
			LPCTSTR const speedMediaInfoCacheList(_T(""));

			CWebDiagnosticsCommonInfo *commonInfo = new CWebDiagnosticsCommonInfo(
				CTime(2000 + (14 + seed) % 99, 1 + seed % 12, 1 + (26 + seed) % 28, (10 + seed) % 24, (11 + seed) % 60, (12 + seed) % 60),
				CTime(2000 + (14 + seed) % 99, 1 + seed % 12, 1 + (26 + seed) % 28, (13 + seed) % 24, (14 + seed) % 60, (15 + seed) % 60),
				941 + seed,
				5141 + seed,
				_T("1.12.2.0"),
				_T("1.21.0"),
				hasSpeedMediaInfo,
				speedMediaInfo,
				speedMediaInfoCacheList,
				(161028 + seed) * 1024,
				seed,
				8 + seed);
			return commonInfo;
		}

		/// <summary>
		/// Gets an operating system instance, initialized for testing CWebDiagnostics::UpdateConfigExt().
		/// </summary>
		/// <returns>A pointer to a new COperatingSystemTest instance.</returns>
		/// <remarks>
		/// The caller is responsible for deleting the instance.
		/// </remarks>
		COperatingSystemTest *GetOperatingSystemTest()
		{
			// Input(s).
			const CString dumpFileName(CONFIG_EXTENSIONS_FILENAME);
			const CFile::OpenFlags dumpnOpenFlags((CFile::OpenFlags)(CFile::modeCreate | CFile::modeWrite));

			// Setup.
			// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
			COperatingSystemTest *operatingSystem = new COperatingSystemTest(dumpFileName,
				dumpnOpenFlags,
				NULL,
				0);

			return operatingSystem;
		}

		/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		TEST_METHOD_INITIALIZE(Initialize)
		{
			// Test method initialization code.
			m_testIndex = 0;
		}

		/// <summary>
		/// Cleanup class member(s) before each test method is run.
		/// </summary>
		TEST_METHOD_CLEANUP(Cleanup)
		{
			// Test method cleanup code.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::GetConfigExtHtml()
		/// Test with default values.
		/// </summary>
		TEST_METHOD(GetConfigExtHtml_DefaultTest)
		{
			// Setup.
			const CConfigExtFile configExt;

			// Test(s).
			AssertResult2(CWebDiagnostics::GetConfigExtHtml(configExt), CalcConfigExtHtml(configExt));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::Set...()
		///     CWebDiagnostics::GetConfigExtHtml()
		/// </summary>
		/// <param name="setValue">IN - The Set...() function for varying test data.</param>
		/// <param name="extraThresholdA">IN - If non-zero then (extra) values (threshold-1, threshold, threshold+1) are included.</param>
		/// <param name="extraThresholdB">IN - If non-zero then (extra) values (threshold-1, threshold, threshold+1) are included.</param>
		template<typename T>
		void DoGetConfigExtHtml_SetTest(void (CConfigExtFile::*setValue)(T), T minimumThreshold = 0, T maximumThreshold = 0)
		{
			// Data.
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(minimumThreshold, maximumThreshold), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				T value;
				testReader.GetNextAttribute(value, CUnitTestingXmlData::TestData_ValueName);

				// Setup.
				CConfigExtFile varConfigExt;
				(varConfigExt.*setValue)(value);
				const CConfigExtFile &configExt(varConfigExt);

				// Test(s).
				AssertResult2(CWebDiagnostics::GetConfigExtHtml(configExt), CalcConfigExtHtml(configExt));
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::Set...()
		///     CWebDiagnostics::GetConfigExtHtml()
		/// </summary>
		TEST_METHOD(GetConfigExtHtml_SetTest)
		{
			DoGetConfigExtHtml_SetTest(&CConfigExtFile::SetNegligibleCreditsThreshold);
			DoGetConfigExtHtml_SetTest(&CConfigExtFile::SetEmployeeOverPlayerIdleCardOut);
			DoGetConfigExtHtml_SetTest(&CConfigExtFile::SetEmployeeIdleCardOutSeconds);
			DoGetConfigExtHtml_SetTest(&CConfigExtFile::SetPlayerRecardEmployeeCardOut);
			DoGetConfigExtHtml_SetTest(&CConfigExtFile::SetSessionActiveNegligibleCreditsTimeoutSeconds,
				CConfigExtData::MINIMUM_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
				CConfigExtData::MAXIMUM_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			DoGetConfigExtHtml_SetTest(&CConfigExtFile::SetSessionActiveNonNegligibleCreditsTimeoutSeconds,
				CConfigExtData::MINIMUM_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
				CConfigExtData::MAXIMUM_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			DoGetConfigExtHtml_SetTest(&CConfigExtFile::SetNotificationForShowPromotionTime,
				CConfigExtData::MINIMUM_NOTIFICATION_SHOW_PROMOTION,
				CConfigExtData::MAXIMUM_NOTIFICATION_SHOW_PROMOTION);
			DoGetConfigExtHtml_SetTest(&CConfigExtFile::SetNotificationForPlayerTrackingTime,
				CConfigExtData::MINIMUM_NOTIFICATION_PLAYER_TRACKING,
				CConfigExtData::MAXIMUM_NOTIFICATION_PLAYER_TRACKING);
			DoGetConfigExtHtml_SetTest(&CConfigExtFile::SetCustomButton1Enabled);
			DoGetConfigExtHtml_SetTest(&CConfigExtFile::SetCustomButton2Enabled);
			DoGetConfigExtHtml_SetTest(&CConfigExtFile::SetCustomMenuButton1Count,
				CConfigExtData::MINIMUM_CUSTOM_MENU_BUTTON_1_COUNT,
				CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_1_COUNT);
			DoGetConfigExtHtml_SetTest(&CConfigExtFile::SetCustomMenuButton2Count,
				CConfigExtData::MINIMUM_CUSTOM_MENU_BUTTON_2_COUNT,
				CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_2_COUNT);
			DoGetConfigExtHtml_SetTest(&CConfigExtFile::SetRebootSwfRestartOverride);
		}

		/// <summary>
		/// Asserts the save file results.
		/// </summary>
		/// <param name="file">IN/OUT - The file.  Seeking & Reading requires read/write access.</param>
		/// <param name="configExt">IN - The configuration extensions.</param>
		/// <param name="commonInfo">IN - The web diagnostics common information.</param>
		/// <param name="dumpFileName">IN - Name of the dump file.</param>
		void AssertSaveFileResults(CMemFileTest &file,
			const CConfigExtFile &configExt,
			const CWebDiagnosticsCommonInfo &commonInfo,
			const CString &dumpFileName)
		{
			// Expected results.
			CStringA expected(CalcConfigExtPage(commonInfo, configExt));

			// Test(s).
			AssertResult2(file.m_fileName, dumpFileName);
			AssertResult2(file.m_nOpenFlags, (UINT)(CFile::modeCreate | CFile::modeWrite));
			AssertResult2(file.m_fileLengthOnOpen, (ULONGLONG)0);
			AssertResult2(file.m_openCount, (DWORD)1);
			AssertResult2(file.m_closeCount, (DWORD)1);
			AssertResult2(file, (DWORD)1);

			AssertResult3f(file, reinterpret_cast<const BYTE *>(static_cast<LPCSTR>(expected)), expected.GetLength());
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::SaveConfigExtData()
		/// Test with default values.
		/// </summary>
		TEST_METHOD(SaveConfigExtData_DefaultTest)
		{
			// Setup.
			unique_ptr<CWebDiagnosticsCommonInfo> commonInfo(GetWebDiagnosticsCommonInfo(m_testIndex));
			const CString dumpFileName(ALT_CONFIG_EXTENSIONS_FILENAME);
			CMemFileTest file;

			// Perform operation(s) to be tested.
			const CConfigExtFile configExt;
			CWebDiagnostics::SaveConfigExtData(file, dumpFileName, *commonInfo, configExt);

			// Test(s).
			AssertSaveFileResults(file, configExt, *commonInfo, dumpFileName);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::Set...()
		///     CWebDiagnostics::SaveConfigExtData()
		/// </summary>
		/// <param name="setValue">IN - The Set...() function for varying test data.</param>
		/// <param name="extraThresholdA">IN - If non-zero then (extra) values (threshold-1, threshold, threshold+1) are included.</param>
		/// <param name="extraThresholdB">IN - If non-zero then (extra) values (threshold-1, threshold, threshold+1) are included.</param>
		template<typename T>
		void DoSaveConfigExtData_SetTest(void (CConfigExtFile::*setValue)(T), T extraThresholdA = 0, T extraThresholdB = 0)
		{
			// Data.
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(extraThresholdA, extraThresholdB), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				T value;
				testReader.GetNextAttribute(value, CUnitTestingXmlData::TestData_ValueName);

				// Setup.
				unique_ptr<CWebDiagnosticsCommonInfo> commonInfo(GetWebDiagnosticsCommonInfo(m_testIndex));
				const CString dumpFileName(ALT_CONFIG_EXTENSIONS_FILENAME);
				CMemFileTest file;

				// Perform operation(s) to be tested.
				CConfigExtFile varConfigExt;
				(varConfigExt.*setValue)(value);
				const CConfigExtFile &configExt(varConfigExt);
				CWebDiagnostics::SaveConfigExtData(file, dumpFileName, *commonInfo, configExt);

				// Test(s).
				AssertSaveFileResults(file, configExt, *commonInfo, dumpFileName);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::Set...()
		///     CWebDiagnostics::SaveConfigExtData()
		/// </summary>
		TEST_METHOD(SaveConfigExtData_SetTest)
		{
			DoSaveConfigExtData_SetTest(&CConfigExtFile::SetNegligibleCreditsThreshold);
			DoSaveConfigExtData_SetTest(&CConfigExtFile::SetEmployeeOverPlayerIdleCardOut);
			DoSaveConfigExtData_SetTest(&CConfigExtFile::SetEmployeeIdleCardOutSeconds);
			DoSaveConfigExtData_SetTest(&CConfigExtFile::SetPlayerRecardEmployeeCardOut);
			DoSaveConfigExtData_SetTest(&CConfigExtFile::SetNotificationForShowPromotionTime,
				CConfigExtData::MINIMUM_NOTIFICATION_SHOW_PROMOTION,
				CConfigExtData::MAXIMUM_NOTIFICATION_SHOW_PROMOTION);
			DoSaveConfigExtData_SetTest(&CConfigExtFile::SetNotificationForPlayerTrackingTime,
				CConfigExtData::MINIMUM_NOTIFICATION_PLAYER_TRACKING,
				CConfigExtData::MAXIMUM_NOTIFICATION_PLAYER_TRACKING);
			DoSaveConfigExtData_SetTest(&CConfigExtFile::SetCustomButton1Enabled);
			DoSaveConfigExtData_SetTest(&CConfigExtFile::SetCustomButton2Enabled);
			DoSaveConfigExtData_SetTest(&CConfigExtFile::SetCustomMenuButton1Count,
				CConfigExtData::MINIMUM_CUSTOM_MENU_BUTTON_1_COUNT,
				CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_1_COUNT);
			DoSaveConfigExtData_SetTest(&CConfigExtFile::SetCustomMenuButton2Count,
				CConfigExtData::MINIMUM_CUSTOM_MENU_BUTTON_2_COUNT,
				CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_2_COUNT);
			DoSaveConfigExtData_SetTest(&CConfigExtFile::SetResortMenuButtonCount,
				CConfigExtData::MINIMUM_RESORT_MENU_BUTTON_COUNT,
				CConfigExtData::MAXIMUM_RESORT_MENU_BUTTON_COUNT);
			DoSaveConfigExtData_SetTest(&CConfigExtFile::SetRebootSwfRestartOverride);
		}

		void AssertUpdateResults(COperatingSystemTest &operatingSystem,
			const CConfigExtFile &configExt,
			const CWebDiagnosticsCommonInfo &commonInfo,
			const CString &dumpFileName)
		{
			CStringA expected(CalcConfigExtPage(commonInfo, configExt));

			// Test(s).
			AssertResult2(operatingSystem.m_file1.m_fileName, dumpFileName);
			AssertResult2(operatingSystem.m_file1.m_openCount, (DWORD)1);
			AssertResult2(operatingSystem.m_file1.m_closeCount, (DWORD)1);
			AssertResult2(operatingSystem.m_file1.m_finalizeCount, (DWORD)1);
			AssertResult2(operatingSystem.m_file1.m_fileName, dumpFileName);

			// Verify that the output file content is as expected.
			AssertResult4(operatingSystem.m_file1.m_finalFileContents, operatingSystem.m_file1.m_finalFileLength,
				static_cast<LPCSTR>(expected), (UINT)expected.GetLength());

			// Check for unexpected accesses to the operating system.
			// Total file counts: One for trying to read the non-existent error file, one for the output file.
			const DWORD totalFileCreateCountExpected(2); // Expect that number of creates == opens == finalizes.
			const int totalGetCurrentTimeCount(0); // Expect no accesses.

			AssertResult2(operatingSystem.m_totalFileCreateCount, totalFileCreateCountExpected);
			AssertResult2(operatingSystem.m_totalFileOpenCount, totalFileCreateCountExpected);
			AssertResult2(operatingSystem.m_totalFileCloseCount, (DWORD)1);
			AssertResult2(operatingSystem.m_totalFileFinalizeCount, totalFileCreateCountExpected);
			AssertResult2(operatingSystem.m_totalGetCurrentTimeCount, totalGetCurrentTimeCount);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::UpdateConfigExt()
		/// Test with default values.
		/// </summary>
		TEST_METHOD(UpdateConfigExt_DefaultTest)
		{
			// Setup.
			unique_ptr<CWebDiagnosticsCommonInfo> commonInfo(GetWebDiagnosticsCommonInfo(m_testIndex));
			unique_ptr<COperatingSystemTest> operatingSystem(GetOperatingSystemTest());

			// Perform operation(s) to be tested.
			const CConfigExtFile configExt;
			CWebDiagnostics wb(*operatingSystem);
			wb.Start();
			wb.UpdateConfigExt(*commonInfo, configExt);


			// Test(s).
			AssertUpdateResults(*operatingSystem, configExt, *commonInfo, CONFIG_EXTENSIONS_FILENAME);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::Set...()
		///     CWebDiagnostics::UpdateConfigExt()
		/// </summary>
		/// <param name="setValue">IN - The Set...() function for varying test data.</param>
		/// <param name="extraThresholdA">IN - If non-zero then (extra) values (threshold-1, threshold, threshold+1) are included.</param>
		/// <param name="extraThresholdB">IN - If non-zero then (extra) values (threshold-1, threshold, threshold+1) are included.</param>
		template<typename T>
		void DoUpdateConfigExt_SetTest(void (CConfigExtFile::*setValue)(T), T extraThresholdA = 0, T extraThresholdB = 0)
		{
			// Data.
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(extraThresholdA, extraThresholdB), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				T value;
				testReader.GetNextAttribute(value, CUnitTestingXmlData::TestData_ValueName);

				// Setup.
				unique_ptr<CWebDiagnosticsCommonInfo> commonInfo(GetWebDiagnosticsCommonInfo(m_testIndex));
				unique_ptr<COperatingSystemTest> operatingSystem(GetOperatingSystemTest());

				// Perform operation(s) to be tested.
				CConfigExtFile varConfigExt;
				(varConfigExt.*setValue)(value);
				const CConfigExtFile &configExt(varConfigExt);
				CWebDiagnostics wb(*operatingSystem);
				wb.Start();
				wb.UpdateConfigExt(*commonInfo, configExt);

				// Test(s).
				AssertUpdateResults(*operatingSystem, configExt, *commonInfo, CONFIG_EXTENSIONS_FILENAME);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::Set...()
		///     CWebDiagnostics::SaveConfigExtData()
		/// </summary>
		TEST_METHOD(UpdateConfigExt_SetTest)
		{
			DoUpdateConfigExt_SetTest(&CConfigExtFile::SetNegligibleCreditsThreshold);
			DoUpdateConfigExt_SetTest(&CConfigExtFile::SetEmployeeOverPlayerIdleCardOut);
			DoUpdateConfigExt_SetTest(&CConfigExtFile::SetEmployeeIdleCardOutSeconds);
			DoUpdateConfigExt_SetTest(&CConfigExtFile::SetPlayerRecardEmployeeCardOut);
			DoUpdateConfigExt_SetTest(&CConfigExtFile::SetNotificationForShowPromotionTime,
				CConfigExtData::MINIMUM_NOTIFICATION_SHOW_PROMOTION,
				CConfigExtData::MAXIMUM_NOTIFICATION_SHOW_PROMOTION);
			DoUpdateConfigExt_SetTest(&CConfigExtFile::SetNotificationForPlayerTrackingTime,
				CConfigExtData::MINIMUM_NOTIFICATION_PLAYER_TRACKING,
				CConfigExtData::MAXIMUM_NOTIFICATION_PLAYER_TRACKING);
			DoUpdateConfigExt_SetTest(&CConfigExtFile::SetCustomButton1Enabled);
			DoUpdateConfigExt_SetTest(&CConfigExtFile::SetCustomButton2Enabled);
			DoUpdateConfigExt_SetTest(&CConfigExtFile::SetCustomMenuButton1Count,
				CConfigExtData::MINIMUM_CUSTOM_MENU_BUTTON_1_COUNT,
				CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_1_COUNT);
			DoUpdateConfigExt_SetTest(&CConfigExtFile::SetCustomMenuButton2Count,
				CConfigExtData::MINIMUM_CUSTOM_MENU_BUTTON_2_COUNT,
				CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_2_COUNT);
			DoUpdateConfigExt_SetTest(&CConfigExtFile::SetCustomMenuButton2Count,
				CConfigExtData::MINIMUM_CUSTOM_MENU_BUTTON_2_COUNT,
				CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_2_COUNT);
			DoUpdateConfigExt_SetTest(&CConfigExtFile::SetRebootSwfRestartOverride);
		}
	};

	const LPCTSTR WebDiagnostics_ConfigExt::CONFIG_EXTENSIONS_FILENAME(_T("C:\\Inetpub\\wwwroot\\ConfigExtensions.html"));
	const LPCTSTR WebDiagnostics_ConfigExt::ALT_CONFIG_EXTENSIONS_FILENAME(_T("C:/Inetpub/wwwroot/ConfigExtensionsAlt.html"));
}
#endif //TODO-PORT