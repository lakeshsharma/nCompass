#if 000//TODO-PORT
#include "stdafx.h"
#include "Config/ConfigExtFile.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingToString.h"
#include "UnitTestingUtils.h"
#include "UnitTestingXmlData.h"
#include "UnitTestingXmlLite.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a test method using the member test index.
#define AssertResult3m(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2m(result, expected) AssertResult3m(result, expected, _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a helper method.
#define AssertResult3h(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, functionName, lineNumber, extraIndex);
#define AssertResult2h(result, expected) AssertResult3h(result, expected, _T(#result))

// AssertResult macro for calling CUnitTestingUtils (with an extra index) from a test method.
#define AssertResult3x(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__, &extraIndex);
#define AssertResult2x(result, expected) AssertResult3x(result, expected, _T(#result))

	TEST_CLASS(ConfigExtFile)
	{
		// Default expected result(s).
		static const WORD m_negligibleCreditsThresholdExpected = 0;
		static const bool m_negligibleCreditsThresholdFromFileExpected = false;

		static const bool m_employeeOverPlayerIdleCardOutExpected = false;
		static const bool m_employeeOverPlayerIdleCardOutFromFileExpected = false;

		static const WORD m_employeeIdleCardOutSecondsExpected = 120;
		static const bool m_employeeIdleCardOutSecondsFromFileExpected = false;

		static const bool m_playerRecardEmployeeCardOutExpected = false;
		static const bool m_playerRecardEmployeeCardOutFromFileExpected = false;

		static const WORD m_notificationShowPromotionExpected = 30;
		static const bool m_notificationShowPromotionFromFileExpected = false;

		static const WORD m_notificationPlayerTrackingExpected = 3;
		static const bool m_notificationPlayerTrackingFromFileExpected = false;

		static const bool m_customButton1EnableExpected = false;
		static const bool m_customButton1EnableFromFileExpected = false;

		static const bool m_customButton2EnableExpected = false;
		static const bool m_customButton2EnableFromFileExpected = false;

		static const WORD m_customMenuButton1CountDefaultExpected = 0;
		static const WORD m_customMenuButton1CountMinimumExpected = 0;
		static const WORD m_customMenuButton1CountMaximumExpected = 6;
		static const WORD m_customMenuButton1CountExpected = 0;
		static const bool m_customMenuButton1CountFromFileExpected = false;

		static const WORD m_customMenuButton2CountDefaultExpected = 0;
		static const WORD m_customMenuButton2CountMinimumExpected = 0;
		static const WORD m_customMenuButton2CountMaximumExpected = 6;
		static const WORD m_customMenuButton2CountExpected = 0;
		static const bool m_customMenuButton2CountFromFileExpected = false;

		static const WORD m_resortMenuButtonCountDefaultExpected = 1;
		static const WORD m_resortMenuButtonCountMinimumExpected = 1;
		static const WORD m_resortMenuButtonCountMaximumExpected = 6;
		static const WORD m_resortMenuButtonCountExpected = 1;
		static const bool m_resortMenuButtonCountFromFileExpected = false;

		// Class member(s) used by test (data) methods.
		int m_testIndex;

		void AssertEqual(const CConfigExtFile &obj,
			WORD negligibleCreditsThresholdExpected,
			bool negligibleCreditsThresholdFromFileExpected,
			bool employeeOverPlayerIdleCardOutExpected,
			bool employeeOverPlayerIdleCardOutFromFileExpected,
			WORD employeeIdleCardOutSecondsExpected,
			bool employeeIdleCardOutSecondsFromFileExpected,
			bool playerRecardEmployeeCardOutExpected,
			bool playerRecardEmployeeCardOutFromFileExpected,
			WORD notificationShowPromotionExpected,
			bool notificationShowPromotionFromFileExpected,
			WORD notificationPlayerTrackingExpected,
			bool notificationPlayerTrackingFromFileExpected,
			bool customButton1EnableExpected,
			bool customButton1EnableFromFileExpected,
			bool customButton2EnableExpected,
			bool customButton2EnableFromFileExpected,
			WORD customMenuButton1CountExpected,
			bool customMenuButton1CountFromFileExpected,
			WORD customMenuButton2CountExpected,
			bool customMenuButton2CountFromFileExpected,
			WORD resortMenuButtonCountExpected,
			bool resortMenuButtonCountFromFileExpected,
			int testIndex,
			LPCTSTR functionName,
			int lineNumber,
			const int* extraIndex = NULL)
		{
			WORD negligibleCreditsThresholdResult;
			bool negligibleCreditsThresholdFromFileResult(obj.GetNegligibleCreditsThreshold(negligibleCreditsThresholdResult));
			AssertResult2h(negligibleCreditsThresholdResult, negligibleCreditsThresholdExpected);
			AssertResult2h(negligibleCreditsThresholdFromFileResult, negligibleCreditsThresholdFromFileExpected);

			bool employeeOverPlayerIdleCardOutResult;
			bool employeeOverPlayerIdleCardOutFromFileResult(obj.GetEmployeeOverPlayerIdleCardOut(employeeOverPlayerIdleCardOutResult));
			AssertResult2h(employeeOverPlayerIdleCardOutResult, employeeOverPlayerIdleCardOutExpected);
			AssertResult2h(employeeOverPlayerIdleCardOutFromFileResult, employeeOverPlayerIdleCardOutFromFileExpected);

			WORD employeeIdleCardOutSecondsResult;
			bool employeeIdleCardOutSecondsFromFileResult(obj.GetEmployeeIdleCardOutSeconds(employeeIdleCardOutSecondsResult));
			AssertResult2h(employeeIdleCardOutSecondsResult, employeeIdleCardOutSecondsExpected);
			AssertResult2h(employeeIdleCardOutSecondsFromFileResult, employeeIdleCardOutSecondsFromFileExpected);

			bool playerRecardEmployeeCardOutResult;
			bool playerRecardEmployeeCardOutFromFileResult(obj.GetPlayerRecardEmployeeCardOut(playerRecardEmployeeCardOutResult));
			AssertResult2h(playerRecardEmployeeCardOutResult, playerRecardEmployeeCardOutExpected);
			AssertResult2h(playerRecardEmployeeCardOutFromFileResult, playerRecardEmployeeCardOutFromFileExpected);

			WORD notificationShowPromotionResult;
			bool notificationShowPromotionFromFileResult(obj.GetNotificationForShowPromotionTime(notificationShowPromotionResult));
			AssertResult2h(notificationShowPromotionResult, notificationShowPromotionExpected);
			AssertResult2h(notificationShowPromotionFromFileResult, notificationShowPromotionFromFileExpected);

			WORD notificationPlayerTrackingResult;
			bool notificationPlayerTrackingFromFileResult(obj.GetNotificationForPlayerTrackingTime(notificationPlayerTrackingResult));
			AssertResult2h(notificationPlayerTrackingResult, notificationPlayerTrackingExpected);
			AssertResult2h(notificationPlayerTrackingFromFileResult, notificationPlayerTrackingFromFileExpected);

			bool customButton1EnableResult;
			bool customButton1EnableFromFileResult(obj.GetCustomButton1Enabled(customButton1EnableResult));
			AssertResult2h(customButton1EnableResult, customButton1EnableExpected);
			AssertResult2h(customButton1EnableFromFileResult, customButton1EnableFromFileExpected);

			bool customButton2EnableResult;
			bool customButton2EnableFromFileResult(obj.GetCustomButton2Enabled(customButton2EnableResult));
			AssertResult2h(customButton2EnableResult, customButton2EnableExpected);
			AssertResult2h(customButton2EnableFromFileResult, customButton2EnableFromFileExpected);

			WORD customMenuButton1CountResult;
			bool customMenuButton1CountFromFileResult(obj.GetCustomMenuButton1Count(customMenuButton1CountResult));
			AssertResult2h(customMenuButton1CountResult, customMenuButton1CountExpected);
			AssertResult2h(customMenuButton1CountFromFileResult, customMenuButton1CountFromFileExpected);

			WORD customMenuButton2CountResult;
			bool customMenuButton2CountFromFileResult(obj.GetCustomMenuButton2Count(customMenuButton2CountResult));
			AssertResult2h(customMenuButton2CountResult, customMenuButton2CountExpected);
			AssertResult2h(customMenuButton2CountFromFileResult, customMenuButton2CountFromFileExpected);

			WORD resortMenuButtonCountResult;
			bool resortMenuButtonCountFromFileResult(obj.GetResortMenuButtonCount(resortMenuButtonCountResult));
			AssertResult2h(resortMenuButtonCountResult, resortMenuButtonCountExpected);
			AssertResult2h(resortMenuButtonCountFromFileResult, resortMenuButtonCountFromFileExpected);
		}

		/// <summary>
		/// Initialize and/or test static class members.
		/// </summary>
		TEST_CLASS_INITIALIZE(ClassInitialize)
		{
			// Test static default value(s).
			Assert::IsTrue(m_customMenuButton1CountDefaultExpected == CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_1_COUNT, _T("m_customMenuButton1CountDefaultExpected"));
			Assert::IsTrue(m_customMenuButton1CountMinimumExpected == CConfigExtData::MINIMUM_CUSTOM_MENU_BUTTON_1_COUNT, _T("m_customMenuButton1CountMinimumExpected"));
			Assert::IsTrue(m_customMenuButton1CountMaximumExpected == CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_1_COUNT, _T("m_customMenuButton1CountMaximumExpected"));

			Assert::IsTrue(m_customMenuButton2CountDefaultExpected == CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_2_COUNT, _T("m_customMenuButton2CountDefaultExpected"));
			Assert::IsTrue(m_customMenuButton2CountMinimumExpected == CConfigExtData::MINIMUM_CUSTOM_MENU_BUTTON_2_COUNT, _T("m_customMenuButton2CountMinimumExpected"));
			Assert::IsTrue(m_customMenuButton2CountMaximumExpected == CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_2_COUNT, _T("m_customMenuButton2CountMaximumExpected"));

			Assert::IsTrue(m_resortMenuButtonCountDefaultExpected == CConfigExtData::DEFAULT_RESORT_MENU_BUTTON_COUNT, _T("m_resortMenuButtonCountDefaultExpected"));
			Assert::IsTrue(m_resortMenuButtonCountMinimumExpected == CConfigExtData::MINIMUM_RESORT_MENU_BUTTON_COUNT, _T("m_resortMenuButtonCountMinimumExpected"));
			Assert::IsTrue(m_resortMenuButtonCountMaximumExpected == CConfigExtData::MAXIMUM_RESORT_MENU_BUTTON_COUNT, _T("m_resortMenuButtonCountMaximumExpected"));
		}

		/// <summary>
		/// Cleanup static class member(s).
		/// </summary>
		TEST_CLASS_CLEANUP(ClassCleanup)
		{
			// Class cleanup code.
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
		///     CConfigExtFile::CConfigExtFile()
		///     CConfigExtFile::GetNegligibleCreditsThreshold()
		///     CConfigExtFile::GetEmployeeOverPlayerIdleCardOut()
		///     CConfigExtFile::GetEmployeeIdleCardOutSeconds()
		///     CConfigExtFile::GetPlayerRecardEmployeeCardOut()
		/// Code coverage:
		///     CConfigExtFile::~CConfigExtFile()
		/// </summary>
		TEST_METHOD(ConfigExtFileTest)
		{
			// Setup.
			const int testIndex(0);

			// Perform operation(s) to be tested.
			const CConfigExtFile obj;

			// Test(s).
			AssertEqual(obj,
				m_negligibleCreditsThresholdExpected,
				m_negligibleCreditsThresholdFromFileExpected,
				m_employeeOverPlayerIdleCardOutExpected,
				m_employeeOverPlayerIdleCardOutFromFileExpected,
				m_employeeIdleCardOutSecondsExpected,
				m_employeeIdleCardOutSecondsFromFileExpected,
				m_playerRecardEmployeeCardOutExpected,
				m_playerRecardEmployeeCardOutFromFileExpected,
				m_notificationShowPromotionExpected,
				m_notificationShowPromotionFromFileExpected,
				m_notificationPlayerTrackingExpected,
				m_notificationPlayerTrackingFromFileExpected,
				m_customButton1EnableExpected,
				m_customButton1EnableFromFileExpected,
				m_customButton2EnableExpected,
				m_customButton2EnableFromFileExpected,
				m_customMenuButton1CountExpected,
				m_customMenuButton1CountFromFileExpected,
				m_customMenuButton2CountExpected,
				m_customMenuButton2CountFromFileExpected,
				m_resortMenuButtonCountExpected,
				m_resortMenuButtonCountFromFileExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::CProgressiveConfigFile()
		///     CConfigExtFile::SetDefaults()
		///     CConfigExtFile::Start()
		///     CConfigExtFile::InterpretNameValue()
		///     CConfigFile::::InterpretWORD()
		///     CConfigExtFile::SetNegligibleCreditsThreshold()
		///     CConfigExtFile::GetNegligibleCreditsThreshold()
		/// Code coverage:
		///     CConfigExtFile::~CConfigExtFile()
		/// Test with fileName "D:\\SentinelData\\ConfigExt.txt"
		/// </summary>
		TEST_METHOD(Start_NegligibleCreditsThresholdTest)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ConfigExt.txt"));
			const CFile::OpenFlags nOpenFlags(CFile::modeRead);

			typedef struct
			{
				// Input(s).
				const BYTE* const m_fileContent;
				const UINT m_fileLength;

				// Expected result(s).
				const WORD m_negligibleCreditsThresholdExpected;
				const bool m_negligibleCreditsThresholdFromFileExpected;
				const DWORD m_totalFileCreateCountExpected; // Expect number of creates == opens == finalizes.
			} TestDataType;

			// Content that isn't even close.
			const char content1[] = "ab";
			const char content2[] = "ab=cd";
			const char content3[] = "NegligibleCreditsThreshold=cd";
			const char content4[] = "NegligibleCreditsThreshold=";
			const char content5[] = "NegligibleCreditsThreshold= ";
			const char content6[] = "NegligibleCreditsThreshold=   ";

			// Content that is close to being valid or are valid.
			const char content7[] = "NegligibleCreditsThreshold=0x9999";
			const char content8[] = "NegligibleCreditsThreshold=9999.";
			const char content9[] = "NegligibleCreditsThreshold=9999.0";
			const char content10[] = "NegligibleCreditsThreshold=9999e0";
			const char content11[] = "NegligibleCreditsThreshold=+9999";
			const char content12[] = "NegligibleCreditsThreshold=1001";
			const char content13[] = " NegligibleCreditsThreshold = 1002 ";
			const char content14[] = "   NegligibleCreditsThreshold   =   1003   ";
			const char content15[] = "NegligibleCreditsThreshold=1004\n";
			const char content16[] = "NegligibleCreditsThreshold=1005\n" "NegligibleCreditsThreshold=1005\n";
			const char content17[] = "   NegligibleCreditsThreshold   =   1006   \n" "   NegligibleCreditsThreshold   =   1007   \n";
			const char content18[] = "   NegligibleCreditsThreshold   =   1008   \n" "   NegligibleCreditsThreshold   =   1009.   \n";
			const char content19[] = "   NegligibleCreditsThreshold   =   1010   \n" "   NegligibleCreditsThreshold   =   1011 0   \n";
			const char content20[] = "   NegligibleCreditsThreshold   =   1012   \n" "   NegligibleCreditsThreshold   =   1013   NegligibleCreditsThreshold   =   1014  \n";

			// Non-negative values that are valid WORD/DWORD values.
			const char content21[] = "NegligibleCreditsThreshold=0";
			const char content22[] = "NegligibleCreditsThreshold=1";
			const char content23[] = "NegligibleCreditsThreshold=2";
			const char content24[] = "NegligibleCreditsThreshold=999";
			const char content25[] = "NegligibleCreditsThreshold=1000";
			const char content26[] = "NegligibleCreditsThreshold=1001";
			const char content26a[] = "NegligibleCreditsThreshold=12345";
			const char content26b[] = "NegligibleCreditsThreshold=32767"; // Maximum unsigned 15-bit value.
			const char content26c[] = "NegligibleCreditsThreshold=32768"; // // One more than maximum unsigned 15-bit value.
			const char content26d[] = "NegligibleCreditsThreshold=65535"; // Maximum unsigned 16-bit value.
			const char content27[] = "NegligibleCreditsThreshold=1234567890";
			const char content28[] = "NegligibleCreditsThreshold=2147483647"; // Maximum unsigned 31-bit value.
			const char content29[] = "NegligibleCreditsThreshold=2147483648"; // // One more than maximum unsigned 31-bit value.
			const char content30[] = "NegligibleCreditsThreshold=4294967295"; // Maximum unsigned 32-bit value.

			// Non-negative values that are not valid WORD/DWORD values.
			const char content30a[] = "NegligibleCreditsThreshold=65536"; // One more than maximum unsigned 16-bit value.
			const char content30b[] = "NegligibleCreditsThreshold=99999";
			const char content31[] = "NegligibleCreditsThreshold=4294967296"; // One more than maximum unsigned 32-bit value.
			const char content32[] = "NegligibleCreditsThreshold=9999999999";
			const char content33[] = "NegligibleCreditsThreshold=18446744073709551615"; // Maximum unsigned 64-bit value.
			const char content34[] = "NegligibleCreditsThreshold=99999999999999999999";
			const char content35[] = "NegligibleCreditsThreshold=9999999999999999999999999999999999999999";

			// "Negative" values that are valid WORD/DWORD values.
			// Except that WORD values are parsed as DWORD values, so negative values are generally large DWORD values and are out of the range of WORD values.
			const char content36[] = "NegligibleCreditsThreshold=-0";
			const char content37[] = "NegligibleCreditsThreshold=-1";
			const char content38[] = "NegligibleCreditsThreshold=-2";
			const char content39[] = "NegligibleCreditsThreshold=-999";
			const char content40[] = "NegligibleCreditsThreshold=-1000";
			const char content41[] = "NegligibleCreditsThreshold=-1001";
			const char content41a[] = "NegligibleCreditsThreshold=-12345";
			const char content41b[] = "NegligibleCreditsThreshold=-32767"; // One more than minimum signed 15-bit value.
			const char content41c[] = "NegligibleCreditsThreshold=-32768"; // Minimum signed 15-bit value.
			const char content41d[] = "NegligibleCreditsThreshold=-32769"; // One less than maximum unsigned 15-bit value.
			const char content41e[] = "NegligibleCreditsThreshold=-64535"; // Equivalent to 1001.
			const char content41f[] = "NegligibleCreditsThreshold=-64536"; // Equivalent to 1000.
			const char content41g[] = "NegligibleCreditsThreshold=-64537"; // Equivalent to 999.
			const char content41h[] = "NegligibleCreditsThreshold=-65535"; // Maximum unsigned 16-bit value, negated.

			const char content42[] = "NegligibleCreditsThreshold=-1234567890";
			const char content43[] = "NegligibleCreditsThreshold=-2147483647"; // One more than minimum signed 31-bit value.
			const char content44[] = "NegligibleCreditsThreshold=-2147483648"; // Minimum signed 31-bit value.
			const char content45[] = "NegligibleCreditsThreshold=-2147483649"; // One less than maximum unsigned 31-bit value.
			const char content45a[] = "NegligibleCreditsThreshold=-4294901760"; // Equivalent to 65536.
			const char content45b[] = "NegligibleCreditsThreshold=-4294901761"; // Equivalent to 65535.
			const char content45c[] = "NegligibleCreditsThreshold=-4294934527"; // Equivalent to 32769.
			const char content45d[] = "NegligibleCreditsThreshold=-4294934528"; // Equivalent to 32767.
			const char content45e[] = "NegligibleCreditsThreshold=-4294934529"; // Equivalent to 32768.
			const char content46[] = "NegligibleCreditsThreshold=-4294966295"; // Equivalent to 1001.
			const char content47[] = "NegligibleCreditsThreshold=-4294966296"; // Equivalent to 1000.
			const char content48[] = "NegligibleCreditsThreshold=-4294966297"; // Equivalent to 999.
			const char content49[] = "NegligibleCreditsThreshold=-4294967295"; // Maximum unsigned 32-bit value, negated.

			// Negative values that are not valid DWORD values.
			const char content50[] = "NegligibleCreditsThreshold=-4294967296"; // One more than maximum unsigned 32-bit value, negated.
			const char content51[] = "NegligibleCreditsThreshold=-9999999999";
			const char content52[] = "NegligibleCreditsThreshold=-18446744073709551615"; // Maximum unsigned 64-bit value.
			const char content53[] = "NegligibleCreditsThreshold=-99999999999999999999";
			const char content54[] = "NegligibleCreditsThreshold=-9999999999999999999999999999999999999999";

			// Content that sets one value and then maybe to another.
			const char content55[] = "NegligibleCreditsThreshold=12345\n" "NegligibleCreditsThreshold=23456\n";
			const char content56[] = "NegligibleCreditsThreshold=12346\n" "NegligibleCreditsThreshold=23457.\n";
			const char content57[] = "NegligibleCreditsThreshold=12347\n" "NegligibleCreditsThreshold=23458 0\n";
			const char content58[] = "NegligibleCreditsThreshold=12348\n" "NegligibleCreditsThreshold=23459\n";

			// More content.
			const char content59[] = "NegligibleCreditsThreshold=01234"; // Test that a value with a leading zero is not treated as a valid octal value.
			const char content60[] = "NegligibleCreditsThreshold=09999"; // Test that a value with a leading zero is not treated as an invalid octal value.

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content1), sizeof(content1)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content2), sizeof(content2)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3), sizeof(content3)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content4), sizeof(content4)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content5), sizeof(content5)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content6), sizeof(content6)-1, 0, false, 2 },

				{ reinterpret_cast<const BYTE*>(content7), sizeof(content7)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content8), sizeof(content8)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content9), sizeof(content9)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content10), sizeof(content10)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content11), sizeof(content11)-1, 9999, true, 1 },
				{ reinterpret_cast<const BYTE*>(content12), sizeof(content12)-1, 1001, true, 1 },
				{ reinterpret_cast<const BYTE*>(content13), sizeof(content13)-1, 1002, true, 1 },
				{ reinterpret_cast<const BYTE*>(content14), sizeof(content14)-1, 1003, true, 1 },
				{ reinterpret_cast<const BYTE*>(content15), sizeof(content15)-1, 1004, true, 1 },
				{ reinterpret_cast<const BYTE*>(content16), sizeof(content16)-1, 1005, true, 1 },
				{ reinterpret_cast<const BYTE*>(content17), sizeof(content17)-1, 1007, true, 1 },
				{ reinterpret_cast<const BYTE*>(content18), sizeof(content18)-1, 1008, true, 1 },
				{ reinterpret_cast<const BYTE*>(content19), sizeof(content19)-1, 1010, true, 1 },
				{ reinterpret_cast<const BYTE*>(content20), sizeof(content20)-1, 1012, true, 1 },

				{ reinterpret_cast<const BYTE*>(content21), sizeof(content21)-1, 0, true, 1 },
				{ reinterpret_cast<const BYTE*>(content22), sizeof(content22)-1, 1, true, 1 },
				{ reinterpret_cast<const BYTE*>(content23), sizeof(content23)-1, 2, true, 1 },
				{ reinterpret_cast<const BYTE*>(content24), sizeof(content24)-1, 999, true, 1 },
				{ reinterpret_cast<const BYTE*>(content25), sizeof(content25)-1, 1000, true, 1 },
				{ reinterpret_cast<const BYTE*>(content26), sizeof(content26)-1, 1001, true, 1 },
				{ reinterpret_cast<const BYTE*>(content26a), sizeof(content26a)-1, 12345, true, 1 },
				{ reinterpret_cast<const BYTE*>(content26b), sizeof(content26b)-1, 32767, true, 1 },
				{ reinterpret_cast<const BYTE*>(content26c), sizeof(content26c)-1, 32768, true, 1 },
				{ reinterpret_cast<const BYTE*>(content26d), sizeof(content26d)-1, 65535, true, 1 },
				{ reinterpret_cast<const BYTE*>(content27), sizeof(content27)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content28), sizeof(content28)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content29), sizeof(content29)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content30), sizeof(content30)-1, 0, false, 2 },

				{ reinterpret_cast<const BYTE*>(content30a), sizeof(content30a)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content30b), sizeof(content30b)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content31), sizeof(content31)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content32), sizeof(content32)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content33), sizeof(content33)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content34), sizeof(content34)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content35), sizeof(content35)-1, 0, false, 2 },

				{ reinterpret_cast<const BYTE*>(content36), sizeof(content36)-1, 0, true, 1 },
				{ reinterpret_cast<const BYTE*>(content37), sizeof(content37)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content38), sizeof(content38)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content39), sizeof(content39)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content40), sizeof(content40)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41), sizeof(content41)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41a), sizeof(content41a)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41b), sizeof(content41b)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41c), sizeof(content41c)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41d), sizeof(content41d)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41e), sizeof(content41e)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41f), sizeof(content41f)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41g), sizeof(content41g)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41h), sizeof(content41h)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content42), sizeof(content42)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content43), sizeof(content43)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content44), sizeof(content44)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content45), sizeof(content45)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content45a), sizeof(content45a)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content45b), sizeof(content45b)-1, 65535, true, 1 },
				{ reinterpret_cast<const BYTE*>(content45c), sizeof(content45c)-1, 32769, true, 1 },
				{ reinterpret_cast<const BYTE*>(content45d), sizeof(content45d)-1, 32768, true, 1 },
				{ reinterpret_cast<const BYTE*>(content45e), sizeof(content45e)-1, 32767, true, 1 },
				{ reinterpret_cast<const BYTE*>(content46), sizeof(content46)-1, 1001, true, 1 },
				{ reinterpret_cast<const BYTE*>(content47), sizeof(content47)-1, 1000, true, 1 },
				{ reinterpret_cast<const BYTE*>(content48), sizeof(content48)-1, 999, true, 1 },
				{ reinterpret_cast<const BYTE*>(content49), sizeof(content49)-1, 1, true, 1 },

				{ reinterpret_cast<const BYTE*>(content50), sizeof(content50)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content51), sizeof(content51)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content52), sizeof(content52)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content53), sizeof(content53)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content54), sizeof(content54)-1, 0, false, 2 },

				{ reinterpret_cast<const BYTE*>(content55), sizeof(content55)-1, 23456, true, 1 },
				{ reinterpret_cast<const BYTE*>(content56), sizeof(content56)-1, 12346, true, 1 },
				{ reinterpret_cast<const BYTE*>(content57), sizeof(content57)-1, 12347, true, 1 },
				{ reinterpret_cast<const BYTE*>(content58), sizeof(content58)-1, 23459, true, 1 },

				{ reinterpret_cast<const BYTE*>(content59), sizeof(content59)-1, 1234, true, 1 },
				{ reinterpret_cast<const BYTE*>(content60), sizeof(content60)-1, 9999, true, 1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
				COperatingSystemTest operatingSystem(fileName,
					nOpenFlags,
					TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength);
				CConfigExtFile obj(operatingSystem);

				// Perform operation(s) to be tested.
				// See note above about the ASSERT for lpMemSource.
				obj.Start();

				// Test(s).
				AssertEqual(obj,
					TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected,
					TEST_DATA[testIndex].m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Verify that the file was accessed as expected.
				AssertResult2(operatingSystem.m_file1.m_isOpen, false);
				AssertResult2(operatingSystem.m_file1.m_openCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_closeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalizeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

				// Verify that the file was not changed.
				Assert::IsTrue(
					CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength,
					operatingSystem.m_file1.m_finalFileContents,
					operatingSystem.m_file1.m_finalFileLength),
					_T("IsEqual - error"));

				// Check for unexpected accesses.
				AssertResult2(operatingSystem.m_totalFileCreateCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileOpenCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileCloseCount, (DWORD)1);
				AssertResult2(operatingSystem.m_totalFileFinalizeCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::CProgressiveConfigFile()
		///     CConfigExtFile::SetDefaults()
		///     CConfigExtFile::Start()
		///     CConfigExtFile::InterpretNameValue()
		///     CConfigFile::::InterpretWORD()
		///     CConfigExtFile::SetNegligibleCreditsThreshold()
		///     CConfigExtFile::GetNegligibleCreditsThreshold()
		/// Code coverage:
		///     CConfigExtFile::~CConfigExtFile()
		/// Test with fileName "C:\\ConfigExt.txt"
		/// </summary>
		TEST_METHOD(Start_NegligibleCreditsThreshold2Test)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("C:\\ConfigExt.txt"));
			const CFile::OpenFlags nOpenFlags(CFile::modeRead);

			typedef struct
			{
				// Input(s).
				const BYTE* const m_fileContent;
				const UINT m_fileLength;

				// Expected result(s).
				const WORD m_negligibleCreditsThresholdExpected;
				const bool m_negligibleCreditsThresholdFromFileExpected;
				const DWORD m_totalFileCreateCountExpected; // Expect number of creates == opens == finalizes.
			} TestDataType;

			// Content that isn't even close.
			const char content1[] = "ab";
			const char content2[] = "ab=cd";
			const char content3[] = "NegligibleCreditsThreshold=cd";
			const char content4[] = "NegligibleCreditsThreshold=";
			const char content5[] = "NegligibleCreditsThreshold= ";
			const char content6[] = "NegligibleCreditsThreshold=   ";

			// Content that is close to being valid or are valid.
			const char content7[] = "NegligibleCreditsThreshold=0x9999";
			const char content8[] = "NegligibleCreditsThreshold=9999.";
			const char content9[] = "NegligibleCreditsThreshold=9999.0";
			const char content10[] = "NegligibleCreditsThreshold=9999e0";
			const char content11[] = "NegligibleCreditsThreshold=+9999";
			const char content12[] = "NegligibleCreditsThreshold=1001";
			const char content13[] = " NegligibleCreditsThreshold = 1002 ";
			const char content14[] = "   NegligibleCreditsThreshold   =   1003   ";
			const char content15[] = "NegligibleCreditsThreshold=1004\n";
			const char content16[] = "NegligibleCreditsThreshold=1005\n" "NegligibleCreditsThreshold=1005\n";
			const char content17[] = "   NegligibleCreditsThreshold   =   1006   \n" "   NegligibleCreditsThreshold   =   1007   \n";
			const char content18[] = "   NegligibleCreditsThreshold   =   1008   \n" "   NegligibleCreditsThreshold   =   1009.   \n";
			const char content19[] = "   NegligibleCreditsThreshold   =   1010   \n" "   NegligibleCreditsThreshold   =   1011 0   \n";
			const char content20[] = "   NegligibleCreditsThreshold   =   1012   \n" "   NegligibleCreditsThreshold   =   1013   NegligibleCreditsThreshold   =   1014  \n";

			// Non-negative values that are valid WORD/DWORD values.
			const char content21[] = "NegligibleCreditsThreshold=0";
			const char content22[] = "NegligibleCreditsThreshold=1";
			const char content23[] = "NegligibleCreditsThreshold=2";
			const char content24[] = "NegligibleCreditsThreshold=999";
			const char content25[] = "NegligibleCreditsThreshold=1000";
			const char content26[] = "NegligibleCreditsThreshold=1001";
			const char content26a[] = "NegligibleCreditsThreshold=12345";
			const char content26b[] = "NegligibleCreditsThreshold=32767"; // Maximum unsigned 15-bit value.
			const char content26c[] = "NegligibleCreditsThreshold=32768"; // // One more than maximum unsigned 15-bit value.
			const char content26d[] = "NegligibleCreditsThreshold=65535"; // Maximum unsigned 16-bit value.
			const char content27[] = "NegligibleCreditsThreshold=1234567890";
			const char content28[] = "NegligibleCreditsThreshold=2147483647"; // Maximum unsigned 31-bit value.
			const char content29[] = "NegligibleCreditsThreshold=2147483648"; // // One more than maximum unsigned 31-bit value.
			const char content30[] = "NegligibleCreditsThreshold=4294967295"; // Maximum unsigned 32-bit value.

			// Non-negative values that are not valid WORD/DWORD values.
			const char content30a[] = "NegligibleCreditsThreshold=65536"; // One more than maximum unsigned 16-bit value.
			const char content30b[] = "NegligibleCreditsThreshold=99999";
			const char content31[] = "NegligibleCreditsThreshold=4294967296"; // One more than maximum unsigned 32-bit value.
			const char content32[] = "NegligibleCreditsThreshold=9999999999";
			const char content33[] = "NegligibleCreditsThreshold=18446744073709551615"; // Maximum unsigned 64-bit value.
			const char content34[] = "NegligibleCreditsThreshold=99999999999999999999";
			const char content35[] = "NegligibleCreditsThreshold=9999999999999999999999999999999999999999";

			// "Negative" values that are valid WORD/DWORD values.
			// Except that WORD values are parsed as DWORD values, so negative values are generally large DWORD values and are out of the range of WORD values.
			const char content36[] = "NegligibleCreditsThreshold=-0";
			const char content37[] = "NegligibleCreditsThreshold=-1";
			const char content38[] = "NegligibleCreditsThreshold=-2";
			const char content39[] = "NegligibleCreditsThreshold=-999";
			const char content40[] = "NegligibleCreditsThreshold=-1000";
			const char content41[] = "NegligibleCreditsThreshold=-1001";
			const char content41a[] = "NegligibleCreditsThreshold=-12345";
			const char content41b[] = "NegligibleCreditsThreshold=-32767"; // One more than minimum signed 15-bit value.
			const char content41c[] = "NegligibleCreditsThreshold=-32768"; // Minimum signed 15-bit value.
			const char content41d[] = "NegligibleCreditsThreshold=-32769"; // One less than maximum unsigned 15-bit value.
			const char content41e[] = "NegligibleCreditsThreshold=-64535"; // Equivalent to 1001.
			const char content41f[] = "NegligibleCreditsThreshold=-64536"; // Equivalent to 1000.
			const char content41g[] = "NegligibleCreditsThreshold=-64537"; // Equivalent to 999.
			const char content41h[] = "NegligibleCreditsThreshold=-65535"; // Maximum unsigned 16-bit value, negated.

			const char content42[] = "NegligibleCreditsThreshold=-1234567890";
			const char content43[] = "NegligibleCreditsThreshold=-2147483647"; // One more than minimum signed 31-bit value.
			const char content44[] = "NegligibleCreditsThreshold=-2147483648"; // Minimum signed 31-bit value.
			const char content45[] = "NegligibleCreditsThreshold=-2147483649"; // One less than maximum unsigned 31-bit value.
			const char content45a[] = "NegligibleCreditsThreshold=-4294901760"; // Equivalent to 65536.
			const char content45b[] = "NegligibleCreditsThreshold=-4294901761"; // Equivalent to 65535.
			const char content45c[] = "NegligibleCreditsThreshold=-4294934527"; // Equivalent to 32769.
			const char content45d[] = "NegligibleCreditsThreshold=-4294934528"; // Equivalent to 32767.
			const char content45e[] = "NegligibleCreditsThreshold=-4294934529"; // Equivalent to 32768.
			const char content46[] = "NegligibleCreditsThreshold=-4294966295"; // Equivalent to 1001.
			const char content47[] = "NegligibleCreditsThreshold=-4294966296"; // Equivalent to 1000.
			const char content48[] = "NegligibleCreditsThreshold=-4294966297"; // Equivalent to 999.
			const char content49[] = "NegligibleCreditsThreshold=-4294967295"; // Maximum unsigned 32-bit value, negated.

			// Negative values that are not valid DWORD values.
			const char content50[] = "NegligibleCreditsThreshold=-4294967296"; // One more than maximum unsigned 32-bit value, negated.
			const char content51[] = "NegligibleCreditsThreshold=-9999999999";
			const char content52[] = "NegligibleCreditsThreshold=-18446744073709551615"; // Maximum unsigned 64-bit value.
			const char content53[] = "NegligibleCreditsThreshold=-99999999999999999999";
			const char content54[] = "NegligibleCreditsThreshold=-9999999999999999999999999999999999999999";

			// Content that sets one value and then maybe to another.
			const char content55[] = "NegligibleCreditsThreshold=12345\n" "NegligibleCreditsThreshold=23456\n";
			const char content56[] = "NegligibleCreditsThreshold=12346\n" "NegligibleCreditsThreshold=23457.\n";
			const char content57[] = "NegligibleCreditsThreshold=12347\n" "NegligibleCreditsThreshold=23458 0\n";
			const char content58[] = "NegligibleCreditsThreshold=12348\n" "NegligibleCreditsThreshold=23459\n";

			// More content.
			const char content59[] = "NegligibleCreditsThreshold=01234"; // Test that a value with a leading zero is not treated as a valid octal value.
			const char content60[] = "NegligibleCreditsThreshold=09999"; // Test that a value with a leading zero is not treated as an invalid octal value.

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content1), sizeof(content1)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content2), sizeof(content2)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3), sizeof(content3)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content4), sizeof(content4)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content5), sizeof(content5)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content6), sizeof(content6)-1, 0, false, 2 },

				{ reinterpret_cast<const BYTE*>(content7), sizeof(content7)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content8), sizeof(content8)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content9), sizeof(content9)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content10), sizeof(content10)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content11), sizeof(content11)-1, 9999, true, 2 },
				{ reinterpret_cast<const BYTE*>(content12), sizeof(content12)-1, 1001, true, 2 },
				{ reinterpret_cast<const BYTE*>(content13), sizeof(content13)-1, 1002, true, 2 },
				{ reinterpret_cast<const BYTE*>(content14), sizeof(content14)-1, 1003, true, 2 },
				{ reinterpret_cast<const BYTE*>(content15), sizeof(content15)-1, 1004, true, 2 },
				{ reinterpret_cast<const BYTE*>(content16), sizeof(content16)-1, 1005, true, 2 },
				{ reinterpret_cast<const BYTE*>(content17), sizeof(content17)-1, 1007, true, 2 },
				{ reinterpret_cast<const BYTE*>(content18), sizeof(content18)-1, 1008, true, 2 },
				{ reinterpret_cast<const BYTE*>(content19), sizeof(content19)-1, 1010, true, 2 },
				{ reinterpret_cast<const BYTE*>(content20), sizeof(content20)-1, 1012, true, 2 },

				{ reinterpret_cast<const BYTE*>(content21), sizeof(content21)-1, 0, true, 2 },
				{ reinterpret_cast<const BYTE*>(content22), sizeof(content22)-1, 1, true, 2 },
				{ reinterpret_cast<const BYTE*>(content23), sizeof(content23)-1, 2, true, 2 },
				{ reinterpret_cast<const BYTE*>(content24), sizeof(content24)-1, 999, true, 2 },
				{ reinterpret_cast<const BYTE*>(content25), sizeof(content25)-1, 1000, true, 2 },
				{ reinterpret_cast<const BYTE*>(content26), sizeof(content26)-1, 1001, true, 2 },
				{ reinterpret_cast<const BYTE*>(content26a), sizeof(content26a)-1, 12345, true, 2 },
				{ reinterpret_cast<const BYTE*>(content26b), sizeof(content26b)-1, 32767, true, 2 },
				{ reinterpret_cast<const BYTE*>(content26c), sizeof(content26c)-1, 32768, true, 2 },
				{ reinterpret_cast<const BYTE*>(content26d), sizeof(content26d)-1, 65535, true, 2 },
				{ reinterpret_cast<const BYTE*>(content27), sizeof(content27)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content28), sizeof(content28)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content29), sizeof(content29)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content30), sizeof(content30)-1, 0, false, 2 },

				{ reinterpret_cast<const BYTE*>(content30a), sizeof(content30a)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content30b), sizeof(content30b)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content31), sizeof(content31)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content32), sizeof(content32)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content33), sizeof(content33)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content34), sizeof(content34)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content35), sizeof(content35)-1, 0, false, 2 },

				{ reinterpret_cast<const BYTE*>(content36), sizeof(content36)-1, 0, true, 2 },
				{ reinterpret_cast<const BYTE*>(content37), sizeof(content37)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content38), sizeof(content38)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content39), sizeof(content39)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content40), sizeof(content40)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41), sizeof(content41)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41a), sizeof(content41a)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41b), sizeof(content41b)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41c), sizeof(content41c)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41d), sizeof(content41d)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41e), sizeof(content41e)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41f), sizeof(content41f)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41g), sizeof(content41g)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41h), sizeof(content41h)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content42), sizeof(content42)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content43), sizeof(content43)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content44), sizeof(content44)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content45), sizeof(content45)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content45a), sizeof(content45a)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content45b), sizeof(content45b)-1, 65535, true, 2 },
				{ reinterpret_cast<const BYTE*>(content45c), sizeof(content45c)-1, 32769, true, 2 },
				{ reinterpret_cast<const BYTE*>(content45d), sizeof(content45d)-1, 32768, true, 2 },
				{ reinterpret_cast<const BYTE*>(content45e), sizeof(content45e)-1, 32767, true, 2 },
				{ reinterpret_cast<const BYTE*>(content46), sizeof(content46)-1, 1001, true, 2 },
				{ reinterpret_cast<const BYTE*>(content47), sizeof(content47)-1, 1000, true, 2 },
				{ reinterpret_cast<const BYTE*>(content48), sizeof(content48)-1, 999, true, 2 },
				{ reinterpret_cast<const BYTE*>(content49), sizeof(content49)-1, 1, true, 2 },

				{ reinterpret_cast<const BYTE*>(content50), sizeof(content50)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content51), sizeof(content51)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content52), sizeof(content52)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content53), sizeof(content53)-1, 0, false, 2 },
				{ reinterpret_cast<const BYTE*>(content54), sizeof(content54)-1, 0, false, 2 },

				{ reinterpret_cast<const BYTE*>(content55), sizeof(content55)-1, 23456, true, 2 },
				{ reinterpret_cast<const BYTE*>(content56), sizeof(content56)-1, 12346, true, 2 },
				{ reinterpret_cast<const BYTE*>(content57), sizeof(content57)-1, 12347, true, 2 },
				{ reinterpret_cast<const BYTE*>(content58), sizeof(content58)-1, 23459, true, 2 },

				{ reinterpret_cast<const BYTE*>(content59), sizeof(content59)-1, 1234, true, 2 },
				{ reinterpret_cast<const BYTE*>(content60), sizeof(content60)-1, 9999, true, 2 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
				COperatingSystemTest operatingSystem(fileName,
					nOpenFlags,
					TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength);
				CConfigExtFile obj(operatingSystem);

				// Perform operation(s) to be tested.
				// See note above about the ASSERT for lpMemSource.
				obj.Start();

				// Test(s).
				AssertEqual(obj,
					TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected,
					TEST_DATA[testIndex].m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Verify that the file was accessed as expected.
				AssertResult2(operatingSystem.m_file1.m_isOpen, false);
				AssertResult2(operatingSystem.m_file1.m_openCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_closeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalizeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

				// Verify that the file was not changed.
				Assert::IsTrue(
					CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength,
					operatingSystem.m_file1.m_finalFileContents,
					operatingSystem.m_file1.m_finalFileLength),
					_T("IsEqual - error"));

				// Check for unexpected accesses.
				AssertResult2(operatingSystem.m_totalFileCreateCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileOpenCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileCloseCount, (DWORD)1);
				AssertResult2(operatingSystem.m_totalFileFinalizeCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::CProgressiveConfigFile()
		///     CConfigExtFile::SetDefaults()
		///     CConfigExtFile::Start()
		///     CConfigExtFile::InterpretNameValue()
		///     CConfigFile::::InterpretBool()
		///     CConfigExtFile::SetEmployeeOverPlayerIdleCardOut()
		///     CConfigExtFile::GetEmployeeOverPlayerIdleCardOut()
		/// Code coverage:
		///     CConfigExtFile::~CConfigExtFile()
		/// Test with fileName "D:\\SentinelData\\ConfigExt.txt"
		/// </summary>
		TEST_METHOD(Start_EmployeeOverPlayerIdleCardOutTest)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ConfigExt.txt"));
			const CFile::OpenFlags nOpenFlags(CFile::modeRead);

			typedef struct
			{
				// Input(s).
				const BYTE* const m_fileContent;
				const UINT m_fileLength;

				// Expected result(s).
				const bool m_employeeOverPlayerIdleCardOutExpected;
				const bool m_employeeOverPlayerIdleCardOutFromFileExpected;
				const DWORD m_totalFileCreateCountExpected; // Expect number of creates == opens == finalizes.
			} TestDataType;

			// Content that does not set RequestButtonEnabled.

			// Content that isn't even close.
			const char content1[] = "ab";
			const char content2[] = "ab=cd";
			const char content3[] = "EmployeeOverPlayerIdleCardOut=cd";
			const char content3a[] = "EmployeeOverPlayerIdleCardOut=";
			const char content3b[] = "EmployeeOverPlayerIdleCardOut= ";
			const char content3c[] = "EmployeeOverPlayerIdleCardOut=   ";

			// Content that is close or equivalent to "IgnoreUnmappedPackages=false\n".
			const char content4[] = "EmployeeOverPlayerIdleCardOut=0";
			const char content5[] = "EmployeeOverPlayerIdleCardOut=F";
			const char content6[] = "EmployeeOverPlayerIdleCardOut=False";
			const char content7[] = "EmployeeOverPlayerIdleCardOut=fals";
			const char content8[] = "EmployeeOverPlayerIdleCardOut=falsE";
			const char content9[] = "EmployeeOverPlayerIdleCardOut=falsex";
			const char content10[] = "EmployeeOverPlayerIdleCardOut=false";
			const char content11[] = " EmployeeOverPlayerIdleCardOut = false ";
			const char content12[] = "   EmployeeOverPlayerIdleCardOut   =   false   ";
			const char content13[] = "EmployeeOverPlayerIdleCardOut=false\n";
			const char content14[] = "EmployeeOverPlayerIdleCardOut=false\n" "EmployeeOverPlayerIdleCardOut=false\n";

			// Content that is close or equivalent to "EmployeeOverPlayerIdleCardOut=true\n".
			const char content15[] = "EmployeeOverPlayerIdleCardOut=1";
			const char content16[] = "EmployeeOverPlayerIdleCardOut=T";
			const char content17[] = "EmployeeOverPlayerIdleCardOut=True";
			const char content18[] = "EmployeeOverPlayerIdleCardOut=tru";
			const char content19[] = "EmployeeOverPlayerIdleCardOut=truE";
			const char content20[] = "EmployeeOverPlayerIdleCardOut=truex";
			const char content21[] = "EmployeeOverPlayerIdleCardOut=true";
			const char content22[] = " EmployeeOverPlayerIdleCardOut = true ";
			const char content23[] = "   EmployeeOverPlayerIdleCardOut   =   true   ";
			const char content24[] = "EmployeeOverPlayerIdleCardOut=true\n";
			const char content25[] = "EmployeeOverPlayerIdleCardOut=true\n" "EmployeeOverPlayerIdleCardOut=true\n";

			// Content that sets EmployeeOverPlayerIdleCardOut to one value and then to another.
			const char content26[] = "EmployeeOverPlayerIdleCardOut=false\n" "EmployeeOverPlayerIdleCardOut=true\n";
			const char content27[] = "EmployeeOverPlayerIdleCardOut=true\n" "EmployeeOverPlayerIdleCardOut=false\n";
			const char content28[] = "EmployeeOverPlayerIdleCardOut=false\n" "EmployeeOverPlayerIdleCardOut=tru\n";
			const char content29[] = "EmployeeOverPlayerIdleCardOut=true\n" "EmployeeOverPlayerIdleCardOut=fals\n";

			// Content that sets EmployeeOverPlayerIdleCardOut to false and then test values that are close or equivalent to "true".
			const char content30[] = "EmployeeOverPlayerIdleCardOut=false\n" "EmployeeOverPlayerIdleCardOut=1";
			const char content31[] = "EmployeeOverPlayerIdleCardOut=false\n" "EmployeeOverPlayerIdleCardOut=T";
			const char content32[] = "EmployeeOverPlayerIdleCardOut=false\n" "EmployeeOverPlayerIdleCardOut=True";
			const char content33[] = "EmployeeOverPlayerIdleCardOut=false\n" "EmployeeOverPlayerIdleCardOut=tru";
			const char content34[] = "EmployeeOverPlayerIdleCardOut=false\n" "EmployeeOverPlayerIdleCardOut=truE";
			const char content35[] = "EmployeeOverPlayerIdleCardOut=false\n" "EmployeeOverPlayerIdleCardOut=truex";
			const char content36[] = "EmployeeOverPlayerIdleCardOut=false\n" "EmployeeOverPlayerIdleCardOut=true";
			const char content37[] = "EmployeeOverPlayerIdleCardOut=false\n" " EmployeeOverPlayerIdleCardOut = true ";
			const char content38[] = "EmployeeOverPlayerIdleCardOut=false\n" "   EmployeeOverPlayerIdleCardOut   =   true   ";
			const char content39[] = "EmployeeOverPlayerIdleCardOut=false\n" "EmployeeOverPlayerIdleCardOut=true\n";

			// Content that sets EmployeeOverPlayerIdleCardOut to true and then test values that are close or equivalent to "false".
			const char content40[] = "EmployeeOverPlayerIdleCardOut=true\n" "EmployeeOverPlayerIdleCardOut=0";
			const char content41[] = "EmployeeOverPlayerIdleCardOut=true\n" "EmployeeOverPlayerIdleCardOut=F";
			const char content42[] = "EmployeeOverPlayerIdleCardOut=true\n" "EmployeeOverPlayerIdleCardOut=False";
			const char content43[] = "EmployeeOverPlayerIdleCardOut=true\n" "EmployeeOverPlayerIdleCardOut=fals";
			const char content44[] = "EmployeeOverPlayerIdleCardOut=true\n" "EmployeeOverPlayerIdleCardOut=falsE";
			const char content45[] = "EmployeeOverPlayerIdleCardOut=true\n" "EmployeeOverPlayerIdleCardOut=falsex";
			const char content46[] = "EmployeeOverPlayerIdleCardOut=true\n" "EmployeeOverPlayerIdleCardOut=false";
			const char content47[] = "EmployeeOverPlayerIdleCardOut=true\n" " EmployeeOverPlayerIdleCardOut = false ";
			const char content48[] = "EmployeeOverPlayerIdleCardOut=true\n" "   EmployeeOverPlayerIdleCardOut   =   false   ";
			const char content49[] = "EmployeeOverPlayerIdleCardOut=true\n" "EmployeeOverPlayerIdleCardOut=false\n";

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content1), sizeof(content1)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content2), sizeof(content2)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3), sizeof(content3)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3a), sizeof(content3a)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3b), sizeof(content3b)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3c), sizeof(content3c)-1, false, false, 2 },

				{ reinterpret_cast<const BYTE*>(content4), sizeof(content4)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content5), sizeof(content5)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content6), sizeof(content6)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content7), sizeof(content7)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content8), sizeof(content8)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content9), sizeof(content9)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content10), sizeof(content10)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content11), sizeof(content11)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content12), sizeof(content12)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content13), sizeof(content13)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content14), sizeof(content14)-1, false, true, 1 },

				{ reinterpret_cast<const BYTE*>(content15), sizeof(content15)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content16), sizeof(content16)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content17), sizeof(content17)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content18), sizeof(content18)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content19), sizeof(content19)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content20), sizeof(content20)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content21), sizeof(content21)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content22), sizeof(content22)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content23), sizeof(content23)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content24), sizeof(content24)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content25), sizeof(content25)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content26), sizeof(content26)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content27), sizeof(content27)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content28), sizeof(content28)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content29), sizeof(content29)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content30), sizeof(content30)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content31), sizeof(content31)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content32), sizeof(content32)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content33), sizeof(content33)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content34), sizeof(content34)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content35), sizeof(content35)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content36), sizeof(content36)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content37), sizeof(content37)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content38), sizeof(content38)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content39), sizeof(content39)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content40), sizeof(content40)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content41), sizeof(content41)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content42), sizeof(content42)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content43), sizeof(content43)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content44), sizeof(content44)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content45), sizeof(content45)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content46), sizeof(content46)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content47), sizeof(content47)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content48), sizeof(content48)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content49), sizeof(content49)-1, false, true, 1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
				COperatingSystemTest operatingSystem(fileName,
					nOpenFlags,
					TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength);
				CConfigExtFile obj(operatingSystem);

				// Perform operation(s) to be tested.
				// See note above about the ASSERT for lpMemSource.
				obj.Start();

				// Test(s).
				AssertEqual(obj,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutExpected,
					TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Verify that the file was accessed as expected.
				AssertResult2(operatingSystem.m_file1.m_isOpen, false);
				AssertResult2(operatingSystem.m_file1.m_openCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_closeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalizeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

				// Verify that the file was not changed.
				Assert::IsTrue(
					CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength,
					operatingSystem.m_file1.m_finalFileContents,
					operatingSystem.m_file1.m_finalFileLength),
					_T("IsEqual - error"));

				// Check for unexpected accesses.
				AssertResult2(operatingSystem.m_totalFileCreateCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileOpenCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileCloseCount, (DWORD)1);
				AssertResult2(operatingSystem.m_totalFileFinalizeCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::CProgressiveConfigFile()
		///     CConfigExtFile::SetDefaults()
		///     CConfigExtFile::Start()
		///     CConfigExtFile::InterpretNameValue()
		///     CConfigFile::::InterpretWORD()
		///     CConfigExtFile::SetEmployeeIdleCardOutSeconds()
		///     CConfigExtFile::GetEmployeeIdleCardOutSeconds()
		/// Code coverage:
		///     CConfigExtFile::~CConfigExtFile()
		/// Test with fileName "D:\\SentinelData\\ConfigExt.txt"
		/// </summary>
		TEST_METHOD(Start_EmployeeIdleCardOutSecondsTest)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ConfigExt.txt"));
			const CFile::OpenFlags nOpenFlags(CFile::modeRead);

			typedef struct
			{
				// Input(s).
				const BYTE* const m_fileContent;
				const UINT m_fileLength;

				// Expected result(s).
				const WORD m_employeeIdleCardOutSecondsExpected;
				const bool m_employeeIdleCardOutSecondsFromFileExpected;
				const DWORD m_totalFileCreateCountExpected; // Expect number of creates == opens == finalizes.
			} TestDataType;

			// Content that isn't even close.
			const char content1[] = "ab";
			const char content2[] = "ab=cd";
			const char content3[] = "EmployeeIdleCardOutSeconds=cd";
			const char content4[] = "EmployeeIdleCardOutSeconds=";
			const char content5[] = "EmployeeIdleCardOutSeconds= ";
			const char content6[] = "EmployeeIdleCardOutSeconds=   ";

			// Content that is close to being valid or are valid.
			const char content7[] = "EmployeeIdleCardOutSeconds=0x9999";
			const char content8[] = "EmployeeIdleCardOutSeconds=9999.";
			const char content9[] = "EmployeeIdleCardOutSeconds=9999.0";
			const char content10[] = "EmployeeIdleCardOutSeconds=9999e0";
			const char content11[] = "EmployeeIdleCardOutSeconds=+9999";
			const char content12[] = "EmployeeIdleCardOutSeconds=1001";
			const char content13[] = " EmployeeIdleCardOutSeconds = 1002 ";
			const char content14[] = "   EmployeeIdleCardOutSeconds   =   1003   ";
			const char content15[] = "EmployeeIdleCardOutSeconds=1004\n";
			const char content16[] = "EmployeeIdleCardOutSeconds=1005\n" "EmployeeIdleCardOutSeconds=1005\n";
			const char content17[] = "   EmployeeIdleCardOutSeconds   =   1006   \n" "   EmployeeIdleCardOutSeconds   =   1007   \n";
			const char content18[] = "   EmployeeIdleCardOutSeconds   =   1008   \n" "   EmployeeIdleCardOutSeconds   =   1009.   \n";
			const char content19[] = "   EmployeeIdleCardOutSeconds   =   1010   \n" "   EmployeeIdleCardOutSeconds   =   1011 0   \n";
			const char content20[] = "   EmployeeIdleCardOutSeconds   =   1012   \n" "   EmployeeIdleCardOutSeconds   =   1013   EmployeeIdleCardOutSeconds   =   1014  \n";

			// Non-negative values that are valid WORD/DWORD values.
			const char content21[] = "EmployeeIdleCardOutSeconds=0";
			const char content22[] = "EmployeeIdleCardOutSeconds=1";
			const char content23[] = "EmployeeIdleCardOutSeconds=2";
			const char content24[] = "EmployeeIdleCardOutSeconds=999";
			const char content25[] = "EmployeeIdleCardOutSeconds=1000";
			const char content26[] = "EmployeeIdleCardOutSeconds=1001";
			const char content26a[] = "EmployeeIdleCardOutSeconds=12345";
			const char content26b[] = "EmployeeIdleCardOutSeconds=32767"; // Maximum unsigned 15-bit value.
			const char content26c[] = "EmployeeIdleCardOutSeconds=32768"; // // One more than maximum unsigned 15-bit value.
			const char content26d[] = "EmployeeIdleCardOutSeconds=65535"; // Maximum unsigned 16-bit value.
			const char content27[] = "EmployeeIdleCardOutSeconds=1234567890";
			const char content28[] = "EmployeeIdleCardOutSeconds=2147483647"; // Maximum unsigned 31-bit value.
			const char content29[] = "EmployeeIdleCardOutSeconds=2147483648"; // // One more than maximum unsigned 31-bit value.
			const char content30[] = "EmployeeIdleCardOutSeconds=4294967295"; // Maximum unsigned 32-bit value.

			// Non-negative values that are not valid WORD/DWORD values.
			const char content30a[] = "EmployeeIdleCardOutSeconds=65536"; // One more than maximum unsigned 16-bit value.
			const char content30b[] = "EmployeeIdleCardOutSeconds=99999";
			const char content31[] = "EmployeeIdleCardOutSeconds=4294967296"; // One more than maximum unsigned 32-bit value.
			const char content32[] = "EmployeeIdleCardOutSeconds=9999999999";
			const char content33[] = "EmployeeIdleCardOutSeconds=18446744073709551615"; // Maximum unsigned 64-bit value.
			const char content34[] = "EmployeeIdleCardOutSeconds=99999999999999999999";
			const char content35[] = "EmployeeIdleCardOutSeconds=9999999999999999999999999999999999999999";

			// "Negative" values that are valid WORD/DWORD values.
			// Except that WORD values are parsed as DWORD values, so negative values are generally large DWORD values and are out of the range of WORD values.
			const char content36[] = "EmployeeIdleCardOutSeconds=-0";
			const char content37[] = "EmployeeIdleCardOutSeconds=-1";
			const char content38[] = "EmployeeIdleCardOutSeconds=-2";
			const char content39[] = "EmployeeIdleCardOutSeconds=-999";
			const char content40[] = "EmployeeIdleCardOutSeconds=-1000";
			const char content41[] = "EmployeeIdleCardOutSeconds=-1001";
			const char content41a[] = "EmployeeIdleCardOutSeconds=-12345";
			const char content41b[] = "EmployeeIdleCardOutSeconds=-32767"; // One more than minimum signed 15-bit value.
			const char content41c[] = "EmployeeIdleCardOutSeconds=-32768"; // Minimum signed 15-bit value.
			const char content41d[] = "EmployeeIdleCardOutSeconds=-32769"; // One less than maximum unsigned 15-bit value.
			const char content41e[] = "EmployeeIdleCardOutSeconds=-64535"; // Equivalent to 1001.
			const char content41f[] = "EmployeeIdleCardOutSeconds=-64536"; // Equivalent to 1000.
			const char content41g[] = "EmployeeIdleCardOutSeconds=-64537"; // Equivalent to 999.
			const char content41h[] = "EmployeeIdleCardOutSeconds=-65535"; // Maximum unsigned 16-bit value, negated.

			const char content42[] = "EmployeeIdleCardOutSeconds=-1234567890";
			const char content43[] = "EmployeeIdleCardOutSeconds=-2147483647"; // One more than minimum signed 31-bit value.
			const char content44[] = "EmployeeIdleCardOutSeconds=-2147483648"; // Minimum signed 31-bit value.
			const char content45[] = "EmployeeIdleCardOutSeconds=-2147483649"; // One less than maximum unsigned 31-bit value.
			const char content45a[] = "EmployeeIdleCardOutSeconds=-4294901760"; // Equivalent to 65536.
			const char content45b[] = "EmployeeIdleCardOutSeconds=-4294901761"; // Equivalent to 65535.
			const char content45c[] = "EmployeeIdleCardOutSeconds=-4294934527"; // Equivalent to 32769.
			const char content45d[] = "EmployeeIdleCardOutSeconds=-4294934528"; // Equivalent to 32767.
			const char content45e[] = "EmployeeIdleCardOutSeconds=-4294934529"; // Equivalent to 32768.
			const char content46[] = "EmployeeIdleCardOutSeconds=-4294966295"; // Equivalent to 1001.
			const char content47[] = "EmployeeIdleCardOutSeconds=-4294966296"; // Equivalent to 1000.
			const char content48[] = "EmployeeIdleCardOutSeconds=-4294966297"; // Equivalent to 999.
			const char content49[] = "EmployeeIdleCardOutSeconds=-4294967295"; // Maximum unsigned 32-bit value, negated.

			// Negative values that are not valid DWORD values.
			const char content50[] = "EmployeeIdleCardOutSeconds=-4294967296"; // One more than maximum unsigned 32-bit value, negated.
			const char content51[] = "EmployeeIdleCardOutSeconds=-9999999999";
			const char content52[] = "EmployeeIdleCardOutSeconds=-18446744073709551615"; // Maximum unsigned 64-bit value.
			const char content53[] = "EmployeeIdleCardOutSeconds=-99999999999999999999";
			const char content54[] = "EmployeeIdleCardOutSeconds=-9999999999999999999999999999999999999999";

			// Content that sets one value and then maybe to another.
			const char content55[] = "EmployeeIdleCardOutSeconds=12345\n" "EmployeeIdleCardOutSeconds=23456\n";
			const char content56[] = "EmployeeIdleCardOutSeconds=12346\n" "EmployeeIdleCardOutSeconds=23457.\n";
			const char content57[] = "EmployeeIdleCardOutSeconds=12347\n" "EmployeeIdleCardOutSeconds=23458 0\n";
			const char content58[] = "EmployeeIdleCardOutSeconds=12348\n" "EmployeeIdleCardOutSeconds=23459\n";

			// More content.
			const char content59[] = "EmployeeIdleCardOutSeconds=01234"; // Test that a value with a leading zero is not treated as a valid octal value.
			const char content60[] = "EmployeeIdleCardOutSeconds=09999"; // Test that a value with a leading zero is not treated as an invalid octal value.

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content1), sizeof(content1)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content2), sizeof(content2)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3), sizeof(content3)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content4), sizeof(content4)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content5), sizeof(content5)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content6), sizeof(content6)-1, 120, false, 2 },

				{ reinterpret_cast<const BYTE*>(content7), sizeof(content7)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content8), sizeof(content8)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content9), sizeof(content9)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content10), sizeof(content10)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content11), sizeof(content11)-1, 9999, true, 1 },
				{ reinterpret_cast<const BYTE*>(content12), sizeof(content12)-1, 1001, true, 1 },
				{ reinterpret_cast<const BYTE*>(content13), sizeof(content13)-1, 1002, true, 1 },
				{ reinterpret_cast<const BYTE*>(content14), sizeof(content14)-1, 1003, true, 1 },
				{ reinterpret_cast<const BYTE*>(content15), sizeof(content15)-1, 1004, true, 1 },
				{ reinterpret_cast<const BYTE*>(content16), sizeof(content16)-1, 1005, true, 1 },
				{ reinterpret_cast<const BYTE*>(content17), sizeof(content17)-1, 1007, true, 1 },
				{ reinterpret_cast<const BYTE*>(content18), sizeof(content18)-1, 1008, true, 1 },
				{ reinterpret_cast<const BYTE*>(content19), sizeof(content19)-1, 1010, true, 1 },
				{ reinterpret_cast<const BYTE*>(content20), sizeof(content20)-1, 1012, true, 1 },

				{ reinterpret_cast<const BYTE*>(content21), sizeof(content21)-1, 0, true, 1 },
				{ reinterpret_cast<const BYTE*>(content22), sizeof(content22)-1, 1, true, 1 },
				{ reinterpret_cast<const BYTE*>(content23), sizeof(content23)-1, 2, true, 1 },
				{ reinterpret_cast<const BYTE*>(content24), sizeof(content24)-1, 999, true, 1 },
				{ reinterpret_cast<const BYTE*>(content25), sizeof(content25)-1, 1000, true, 1 },
				{ reinterpret_cast<const BYTE*>(content26), sizeof(content26)-1, 1001, true, 1 },
				{ reinterpret_cast<const BYTE*>(content26a), sizeof(content26a)-1, 12345, true, 1 },
				{ reinterpret_cast<const BYTE*>(content26b), sizeof(content26b)-1, 32767, true, 1 },
				{ reinterpret_cast<const BYTE*>(content26c), sizeof(content26c)-1, 32768, true, 1 },
				{ reinterpret_cast<const BYTE*>(content26d), sizeof(content26d)-1, 65535, true, 1 },
				{ reinterpret_cast<const BYTE*>(content27), sizeof(content27)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content28), sizeof(content28)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content29), sizeof(content29)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content30), sizeof(content30)-1, 120, false, 2 },

				{ reinterpret_cast<const BYTE*>(content30a), sizeof(content30a)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content30b), sizeof(content30b)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content31), sizeof(content31)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content32), sizeof(content32)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content33), sizeof(content33)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content34), sizeof(content34)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content35), sizeof(content35)-1, 120, false, 2 },

				{ reinterpret_cast<const BYTE*>(content36), sizeof(content36)-1, 0, true, 1 },
				{ reinterpret_cast<const BYTE*>(content37), sizeof(content37)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content38), sizeof(content38)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content39), sizeof(content39)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content40), sizeof(content40)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41), sizeof(content41)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41a), sizeof(content41a)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41b), sizeof(content41b)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41c), sizeof(content41c)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41d), sizeof(content41d)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41e), sizeof(content41e)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41f), sizeof(content41f)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41g), sizeof(content41g)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content41h), sizeof(content41h)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content42), sizeof(content42)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content43), sizeof(content43)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content44), sizeof(content44)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content45), sizeof(content45)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content45a), sizeof(content45a)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content45b), sizeof(content45b)-1, 65535, true, 1 },
				{ reinterpret_cast<const BYTE*>(content45c), sizeof(content45c)-1, 32769, true, 1 },
				{ reinterpret_cast<const BYTE*>(content45d), sizeof(content45d)-1, 32768, true, 1 },
				{ reinterpret_cast<const BYTE*>(content45e), sizeof(content45e)-1, 32767, true, 1 },
				{ reinterpret_cast<const BYTE*>(content46), sizeof(content46)-1, 1001, true, 1 },
				{ reinterpret_cast<const BYTE*>(content47), sizeof(content47)-1, 1000, true, 1 },
				{ reinterpret_cast<const BYTE*>(content48), sizeof(content48)-1, 999, true, 1 },
				{ reinterpret_cast<const BYTE*>(content49), sizeof(content49)-1, 1, true, 1 },

				{ reinterpret_cast<const BYTE*>(content50), sizeof(content50)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content51), sizeof(content51)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content52), sizeof(content52)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content53), sizeof(content53)-1, 120, false, 2 },
				{ reinterpret_cast<const BYTE*>(content54), sizeof(content54)-1, 120, false, 2 },

				{ reinterpret_cast<const BYTE*>(content55), sizeof(content55)-1, 23456, true, 1 },
				{ reinterpret_cast<const BYTE*>(content56), sizeof(content56)-1, 12346, true, 1 },
				{ reinterpret_cast<const BYTE*>(content57), sizeof(content57)-1, 12347, true, 1 },
				{ reinterpret_cast<const BYTE*>(content58), sizeof(content58)-1, 23459, true, 1 },

				{ reinterpret_cast<const BYTE*>(content59), sizeof(content59)-1, 1234, true, 1 },
				{ reinterpret_cast<const BYTE*>(content60), sizeof(content60)-1, 9999, true, 1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
				COperatingSystemTest operatingSystem(fileName,
					nOpenFlags,
					TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength);
				CConfigExtFile obj(operatingSystem);

				// Perform operation(s) to be tested.
				// See note above about the ASSERT for lpMemSource.
				obj.Start();

				// Test(s).
				AssertEqual(obj,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					TEST_DATA[testIndex].m_employeeIdleCardOutSecondsExpected,
					TEST_DATA[testIndex].m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Verify that the file was accessed as expected.
				AssertResult2(operatingSystem.m_file1.m_isOpen, false);
				AssertResult2(operatingSystem.m_file1.m_openCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_closeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalizeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

				// Verify that the file was not changed.
				Assert::IsTrue(
					CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength,
					operatingSystem.m_file1.m_finalFileContents,
					operatingSystem.m_file1.m_finalFileLength),
					_T("IsEqual - error"));

				// Check for unexpected accesses.
				AssertResult2(operatingSystem.m_totalFileCreateCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileOpenCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileCloseCount, (DWORD)1);
				AssertResult2(operatingSystem.m_totalFileFinalizeCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::CProgressiveConfigFile()
		///     CConfigExtFile::SetDefaults()
		///     CConfigExtFile::Start()
		///     CConfigExtFile::InterpretNameValue()
		///     CConfigFile::::InterpretBool()
		///     CConfigExtFile::SetPlayerRecardEmployeeCardOut()
		///     CConfigExtFile::GetPlayerRecardEmployeeCardOut()
		/// Code coverage:
		///     CConfigExtFile::~CConfigExtFile()
		/// Test with fileName "D:\\SentinelData\\ConfigExt.txt"
		/// </summary>
		TEST_METHOD(Start_PlayerRecardEmployeeCardOutTest)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ConfigExt.txt"));
			const CFile::OpenFlags nOpenFlags(CFile::modeRead);

			typedef struct
			{
				// Input(s).
				const BYTE* const m_fileContent;
				const UINT m_fileLength;

				// Expected result(s).
				const bool m_playerRecardEmployeeCardOutExpected;
				const bool m_playerRecardEmployeeCardOutFromFileExpected;
				const DWORD m_totalFileCreateCountExpected; // Expect number of creates == opens == finalizes.
			} TestDataType;

			// Content that does not set RequestButtonEnabled.

			// Content that isn't even close.
			const char content1[] = "ab";
			const char content2[] = "ab=cd";
			const char content3[] = "PlayerRecardEmployeeCardOut=cd";
			const char content3a[] = "PlayerRecardEmployeeCardOut=";
			const char content3b[] = "PlayerRecardEmployeeCardOut= ";
			const char content3c[] = "PlayerRecardEmployeeCardOut=   ";

			// Content that is close or equivalent to "IgnoreUnmappedPackages=false\n".
			const char content4[] = "PlayerRecardEmployeeCardOut=0";
			const char content5[] = "PlayerRecardEmployeeCardOut=F";
			const char content6[] = "PlayerRecardEmployeeCardOut=False";
			const char content7[] = "PlayerRecardEmployeeCardOut=fals";
			const char content8[] = "PlayerRecardEmployeeCardOut=falsE";
			const char content9[] = "PlayerRecardEmployeeCardOut=falsex";
			const char content10[] = "PlayerRecardEmployeeCardOut=false";
			const char content11[] = " PlayerRecardEmployeeCardOut = false ";
			const char content12[] = "   PlayerRecardEmployeeCardOut   =   false   ";
			const char content13[] = "PlayerRecardEmployeeCardOut=false\n";
			const char content14[] = "PlayerRecardEmployeeCardOut=false\n" "PlayerRecardEmployeeCardOut=false\n";

			// Content that is close or equivalent to "PlayerRecardEmployeeCardOut=true\n".
			const char content15[] = "PlayerRecardEmployeeCardOut=1";
			const char content16[] = "PlayerRecardEmployeeCardOut=T";
			const char content17[] = "PlayerRecardEmployeeCardOut=True";
			const char content18[] = "PlayerRecardEmployeeCardOut=tru";
			const char content19[] = "PlayerRecardEmployeeCardOut=truE";
			const char content20[] = "PlayerRecardEmployeeCardOut=truex";
			const char content21[] = "PlayerRecardEmployeeCardOut=true";
			const char content22[] = " PlayerRecardEmployeeCardOut = true ";
			const char content23[] = "   PlayerRecardEmployeeCardOut   =   true   ";
			const char content24[] = "PlayerRecardEmployeeCardOut=true\n";
			const char content25[] = "PlayerRecardEmployeeCardOut=true\n" "PlayerRecardEmployeeCardOut=true\n";

			// Content that sets PlayerRecardEmployeeCardOut to one value and then to another.
			const char content26[] = "PlayerRecardEmployeeCardOut=false\n" "PlayerRecardEmployeeCardOut=true\n";
			const char content27[] = "PlayerRecardEmployeeCardOut=true\n" "PlayerRecardEmployeeCardOut=false\n";
			const char content28[] = "PlayerRecardEmployeeCardOut=false\n" "PlayerRecardEmployeeCardOut=tru\n";
			const char content29[] = "PlayerRecardEmployeeCardOut=true\n" "PlayerRecardEmployeeCardOut=fals\n";

			// Content that sets PlayerRecardEmployeeCardOut to false and then test values that are close or equivalent to "true".
			const char content30[] = "PlayerRecardEmployeeCardOut=false\n" "PlayerRecardEmployeeCardOut=1";
			const char content31[] = "PlayerRecardEmployeeCardOut=false\n" "PlayerRecardEmployeeCardOut=T";
			const char content32[] = "PlayerRecardEmployeeCardOut=false\n" "PlayerRecardEmployeeCardOut=True";
			const char content33[] = "PlayerRecardEmployeeCardOut=false\n" "PlayerRecardEmployeeCardOut=tru";
			const char content34[] = "PlayerRecardEmployeeCardOut=false\n" "PlayerRecardEmployeeCardOut=truE";
			const char content35[] = "PlayerRecardEmployeeCardOut=false\n" "PlayerRecardEmployeeCardOut=truex";
			const char content36[] = "PlayerRecardEmployeeCardOut=false\n" "PlayerRecardEmployeeCardOut=true";
			const char content37[] = "PlayerRecardEmployeeCardOut=false\n" " PlayerRecardEmployeeCardOut = true ";
			const char content38[] = "PlayerRecardEmployeeCardOut=false\n" "   PlayerRecardEmployeeCardOut   =   true   ";
			const char content39[] = "PlayerRecardEmployeeCardOut=false\n" "PlayerRecardEmployeeCardOut=true\n";

			// Content that sets PlayerRecardEmployeeCardOut to true and then test values that are close or equivalent to "false".
			const char content40[] = "PlayerRecardEmployeeCardOut=true\n" "PlayerRecardEmployeeCardOut=0";
			const char content41[] = "PlayerRecardEmployeeCardOut=true\n" "PlayerRecardEmployeeCardOut=F";
			const char content42[] = "PlayerRecardEmployeeCardOut=true\n" "PlayerRecardEmployeeCardOut=False";
			const char content43[] = "PlayerRecardEmployeeCardOut=true\n" "PlayerRecardEmployeeCardOut=fals";
			const char content44[] = "PlayerRecardEmployeeCardOut=true\n" "PlayerRecardEmployeeCardOut=falsE";
			const char content45[] = "PlayerRecardEmployeeCardOut=true\n" "PlayerRecardEmployeeCardOut=falsex";
			const char content46[] = "PlayerRecardEmployeeCardOut=true\n" "PlayerRecardEmployeeCardOut=false";
			const char content47[] = "PlayerRecardEmployeeCardOut=true\n" " PlayerRecardEmployeeCardOut = false ";
			const char content48[] = "PlayerRecardEmployeeCardOut=true\n" "   PlayerRecardEmployeeCardOut   =   false   ";
			const char content49[] = "PlayerRecardEmployeeCardOut=true\n" "PlayerRecardEmployeeCardOut=false\n";

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content1), sizeof(content1)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content2), sizeof(content2)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3), sizeof(content3)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3a), sizeof(content3a)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3b), sizeof(content3b)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3c), sizeof(content3c)-1, false, false, 2 },

				{ reinterpret_cast<const BYTE*>(content4), sizeof(content4)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content5), sizeof(content5)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content6), sizeof(content6)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content7), sizeof(content7)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content8), sizeof(content8)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content9), sizeof(content9)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content10), sizeof(content10)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content11), sizeof(content11)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content12), sizeof(content12)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content13), sizeof(content13)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content14), sizeof(content14)-1, false, true, 1 },

				{ reinterpret_cast<const BYTE*>(content15), sizeof(content15)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content16), sizeof(content16)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content17), sizeof(content17)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content18), sizeof(content18)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content19), sizeof(content19)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content20), sizeof(content20)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content21), sizeof(content21)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content22), sizeof(content22)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content23), sizeof(content23)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content24), sizeof(content24)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content25), sizeof(content25)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content26), sizeof(content26)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content27), sizeof(content27)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content28), sizeof(content28)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content29), sizeof(content29)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content30), sizeof(content30)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content31), sizeof(content31)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content32), sizeof(content32)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content33), sizeof(content33)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content34), sizeof(content34)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content35), sizeof(content35)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content36), sizeof(content36)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content37), sizeof(content37)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content38), sizeof(content38)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content39), sizeof(content39)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content40), sizeof(content40)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content41), sizeof(content41)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content42), sizeof(content42)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content43), sizeof(content43)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content44), sizeof(content44)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content45), sizeof(content45)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content46), sizeof(content46)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content47), sizeof(content47)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content48), sizeof(content48)-1, false, true, 1 },
				{ reinterpret_cast<const BYTE*>(content49), sizeof(content49)-1, false, true, 1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
				COperatingSystemTest operatingSystem(fileName,
					nOpenFlags,
					TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength);
				CConfigExtFile obj(operatingSystem);

				// Perform operation(s) to be tested.
				// See note above about the ASSERT for lpMemSource.
				obj.Start();

				// Test(s).
				AssertEqual(obj,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					TEST_DATA[testIndex].m_playerRecardEmployeeCardOutExpected,
					TEST_DATA[testIndex].m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Verify that the file was accessed as expected.
				AssertResult2(operatingSystem.m_file1.m_isOpen, false);
				AssertResult2(operatingSystem.m_file1.m_openCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_closeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalizeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

				// Verify that the file was not changed.
				Assert::IsTrue(
					CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength,
					operatingSystem.m_file1.m_finalFileContents,
					operatingSystem.m_file1.m_finalFileLength),
					_T("IsEqual - error"));

				// Check for unexpected accesses.
				AssertResult2(operatingSystem.m_totalFileCreateCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileOpenCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileCloseCount, (DWORD)1);
				AssertResult2(operatingSystem.m_totalFileFinalizeCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::CConfigExtFile()
		///     CConfigExtFile::DeleteConfigFile()
		/// Code coverage:
		///     CConfigExtFile::~CConfigExtFile()
		/// </summary>
		TEST_METHOD(DeleteConfigFileTest)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ConfigExt.txt"));
			const CFile::OpenFlags nOpenFlags(CFile::modeRead);
			const BYTE* const fileContent(NULL);
			const UINT fileLength(0);

			// Expected result(s).
			DWORD totalFileDeleteCountExpected(1);
			CString lastFilenameDeletedExpected(_T("D:\\SentinelData\\ConfigExt.txt"));

			// Setup.
			const int testIndex(0);
			// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
			COperatingSystemTest operatingSystem(fileName,
				nOpenFlags,
				fileContent,
				fileLength);
			CConfigExtFile obj(operatingSystem);

			// Perform operation(s) to be tested.
			obj.DeleteConfigFile();

			// Test(s).
			AssertResult2(operatingSystem.m_totalFileDeleteCount, totalFileDeleteCountExpected);
			AssertResult2(operatingSystem.m_lastFilenameDeleted, lastFilenameDeletedExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::CConfigExtFile(const CConfig&)
		///     CConfigExtFile::GetNegligibleCreditsThreshold()
		///     CConfigExtFile::GetEmployeeOverPlayerIdleCardOut()
		///     CConfigExtFile::GetEmployeeIdleCardOutSeconds()
		///     CConfigExtFile::GetPlayerRecardEmployeeCardOut()
		/// Code coverage:
		///     CConfigExtFile::~CConfigExtFile()
		/// Test with a default CConfig instance;
		/// </summary>
		TEST_METHOD(CConfigExtFileCConfig_DefaultTest)
		{
			// Setup.
			const int testIndex(0);
			CUnitTestingUtils::SetNvramInvalid();
			const CConfig config(false, "");

			// Perform operation(s) to be tested.
			CConfigExtFile obj(config);

			// Test(s).
			AssertEqual(obj,
				m_negligibleCreditsThresholdExpected,
				m_negligibleCreditsThresholdFromFileExpected,
				m_employeeOverPlayerIdleCardOutExpected,
				m_employeeOverPlayerIdleCardOutFromFileExpected,
				m_employeeIdleCardOutSecondsExpected,
				m_employeeIdleCardOutSecondsFromFileExpected,
				m_playerRecardEmployeeCardOutExpected,
				m_playerRecardEmployeeCardOutFromFileExpected,
				m_notificationShowPromotionExpected,
				m_notificationShowPromotionFromFileExpected,
				m_notificationPlayerTrackingExpected,
				m_notificationPlayerTrackingFromFileExpected,
				m_customButton1EnableExpected,
				m_customButton1EnableFromFileExpected,
				m_customButton2EnableExpected,
				m_customButton2EnableFromFileExpected,
				m_customMenuButton1CountExpected,
				m_customMenuButton1CountFromFileExpected,
				m_customMenuButton2CountExpected,
				m_customMenuButton2CountFromFileExpected,
				m_resortMenuButtonCountExpected,
				m_resortMenuButtonCountFromFileExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::CConfigExtFile(const CConfig&)
		///     CConfigExtFile::GetNegligibleCreditsThreshold()
		///     CConfigExtFile::GetEmployeeOverPlayerIdleCardOut()
		///     CConfigExtFile::GetEmployeeIdleCardOutSeconds()
		///     CConfigExtFile::GetPlayerRecardEmployeeCardOut()
		/// Code coverage:
		///     CConfigExtFile::~CConfigExtFile()
		/// </summary>
		TEST_METHOD(CConfigExtFileCConfigTest)
		{
			static const WORD negligibleCreditsThreshold(1234);
			static const bool employeeOverPlayerIdleCardOut(true);
			static const WORD employeeIdleCardOutSeconds(5678);
			static const bool playerRecardEmployeeCardOut(true);

			// Expected result(s).
			static const WORD negligibleCreditsThresholdExpected(1234);
			static const bool employeeOverPlayerIdleCardOutExpected(false);
			static const WORD employeeIdleCardOutSecondsExpected(56789);
			static const bool playerRecardEmployeeCardOutExpected(true);

			// Setup.
			const int testIndex(0);
			CUnitTestingUtils::SetNvramInvalid();
			CConfig varConfig(false, "");
			varConfig.SetNegligibleCreditsThreshold(negligibleCreditsThreshold);
			varConfig.SetEmployeeOverPlayerIdleCardOut(employeeOverPlayerIdleCardOutExpected);
			varConfig.SetEmployeeIdleCardOutSeconds(employeeIdleCardOutSecondsExpected);
			varConfig.SetPlayerRecardEmployeeCardOut(playerRecardEmployeeCardOutExpected);
			CConfig& config(varConfig);


			// Perform operation(s) to be tested.
			CConfigExtFile obj(config);

			// Test(s).
			AssertEqual(obj,
				negligibleCreditsThresholdExpected,
				m_negligibleCreditsThresholdFromFileExpected,
				employeeOverPlayerIdleCardOutExpected,
				m_employeeOverPlayerIdleCardOutFromFileExpected,
				employeeIdleCardOutSecondsExpected,
				m_employeeIdleCardOutSecondsFromFileExpected,
				playerRecardEmployeeCardOutExpected,
				m_playerRecardEmployeeCardOutFromFileExpected,
				m_notificationShowPromotionExpected,
				m_notificationShowPromotionFromFileExpected,
				m_notificationPlayerTrackingExpected,
				m_notificationPlayerTrackingFromFileExpected,
				m_customButton1EnableExpected,
				m_customButton1EnableFromFileExpected,
				m_customButton2EnableExpected,
				m_customButton2EnableFromFileExpected,
				m_customMenuButton1CountExpected,
				m_customMenuButton1CountFromFileExpected,
				m_customMenuButton2CountExpected,
				m_customMenuButton2CountFromFileExpected,
				m_resortMenuButtonCountExpected,
				m_resortMenuButtonCountFromFileExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::CConfigExtFile(const CConfigExtFile&)
		///     CConfigExtFile::GetNegligibleCreditsThreshold()
		///     CConfigExtFile::GetEmployeeOverPlayerIdleCardOut()
		///     CConfigExtFile::GetEmployeeIdleCardOutSeconds()
		///     CConfigExtFile::GetPlayerRecardEmployeeCardOut()
		/// Code coverage:
		///     CConfigExtFile::~CConfigExtFile()
		/// Test with a default CConfigExt instance;
		/// </summary>
		TEST_METHOD(CopyConstructor_DefaultTest)
		{
			// Setup.
			const int testIndex(0);
			const CConfigExtFile configExt1;

			// Perform operation(s) to be tested.
			CConfigExtFile obj(configExt1);

			// Test(s).
			AssertEqual(obj,
				m_negligibleCreditsThresholdExpected,
				m_negligibleCreditsThresholdFromFileExpected,
				m_employeeOverPlayerIdleCardOutExpected,
				m_employeeOverPlayerIdleCardOutFromFileExpected,
				m_employeeIdleCardOutSecondsExpected,
				m_employeeIdleCardOutSecondsFromFileExpected,
				m_playerRecardEmployeeCardOutExpected,
				m_playerRecardEmployeeCardOutFromFileExpected,
				m_notificationShowPromotionExpected,
				m_notificationShowPromotionFromFileExpected,
				m_notificationPlayerTrackingExpected,
				m_notificationPlayerTrackingFromFileExpected,
				m_customButton1EnableExpected,
				m_customButton1EnableFromFileExpected,
				m_customButton2EnableExpected,
				m_customButton2EnableFromFileExpected,
				m_customMenuButton1CountExpected,
				m_customMenuButton1CountFromFileExpected,
				m_customMenuButton2CountExpected,
				m_customMenuButton2CountFromFileExpected,
				m_resortMenuButtonCountExpected,
				m_resortMenuButtonCountFromFileExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::CConfigExtFile(const CConfigExtFile&)
		///     CConfigExtFile::GetNegligibleCreditsThreshold()
		///     CConfigExtFile::GetEmployeeOverPlayerIdleCardOut()
		///     CConfigExtFile::GetEmployeeIdleCardOutSeconds()
		///     CConfigExtFile::GetPlayerRecardEmployeeCardOut()
		/// Code coverage:
		///     CConfigExtFile::~CConfigExtFile()
		/// </summary>
		TEST_METHOD(CopyConstructorTest)
		{
			static const WORD negligibleCreditsThreshold(1234);
			static const bool employeeOverPlayerIdleCardOut(true);
			static const WORD employeeIdleCardOutSeconds(5678);
			static const bool playerRecardEmployeeCardOut(true);

			// Expected result(s).
			static const WORD negligibleCreditsThresholdExpected(1234);
			static const bool negligibleCreditsThresholdFromFileExpected(true);

			static const bool employeeOverPlayerIdleCardOutExpected(true);
			static const bool employeeOverPlayerIdleCardOutFromFileExpected(true);

			static const WORD employeeIdleCardOutSecondsExpected(5678);
			static const bool employeeIdleCardOutSecondsFromFileExpected(true);

			static const bool playerRecardEmployeeCardOutExpected(true);
			static const bool playerRecardEmployeeCardOutFromFileExpected(true);

			// Setup.
			const int testIndex(0);
			CConfigExtFile varConfigExt1;
			varConfigExt1.SetNegligibleCreditsThreshold(negligibleCreditsThreshold);
			varConfigExt1.SetEmployeeOverPlayerIdleCardOut(employeeOverPlayerIdleCardOutExpected);
			varConfigExt1.SetEmployeeIdleCardOutSeconds(employeeIdleCardOutSecondsExpected);
			varConfigExt1.SetPlayerRecardEmployeeCardOut(playerRecardEmployeeCardOutExpected);
			const CConfigExtFile& configExt1(varConfigExt1);

			// Perform operation(s) to be tested.
			CConfigExtFile obj(configExt1);

			// Test(s).
			AssertEqual(obj,
				negligibleCreditsThresholdExpected,
				negligibleCreditsThresholdFromFileExpected,
				employeeOverPlayerIdleCardOutExpected,
				employeeOverPlayerIdleCardOutFromFileExpected,
				employeeIdleCardOutSecondsExpected,
				employeeIdleCardOutSecondsFromFileExpected,
				playerRecardEmployeeCardOutExpected,
				playerRecardEmployeeCardOutFromFileExpected,
				m_notificationShowPromotionExpected,
				m_notificationShowPromotionFromFileExpected,
				m_notificationPlayerTrackingExpected,
				m_notificationPlayerTrackingFromFileExpected,
				m_customButton1EnableExpected,
				m_customButton1EnableFromFileExpected,
				m_customButton2EnableExpected,
				m_customButton2EnableFromFileExpected,
				m_customMenuButton1CountExpected,
				m_customMenuButton1CountFromFileExpected,
				m_customMenuButton2CountExpected,
				m_customMenuButton2CountFromFileExpected,
				m_resortMenuButtonCountExpected,
				m_resortMenuButtonCountFromFileExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::operator=(const CConfigExtFile &)
		///     CConfigExtFile::GetNegligibleCreditsThreshold()
		///     CConfigExtFile::GetEmployeeOverPlayerIdleCardOut()
		///     CConfigExtFile::GetEmployeeIdleCardOutSeconds()
		///     CConfigExtFile::GetPlayerRecardEmployeeCardOut()
		/// Code coverage:
		///     CConfigExtFile::~CConfigExtFile()
		/// Test with a default CConfigExt instance;
		/// </summary>
		TEST_METHOD(AssignmentOperator_DefaultTest)
		{
			// Input(s).
			static const WORD negligibleCreditsThreshold(1234);
			static const bool employeeOverPlayerIdleCardOut(true);
			static const WORD employeeIdleCardOutSeconds(5678);
			static const bool playerRecardEmployeeCardOut(true);

			// Expected result(s).
			static const WORD negligibleCreditsThresholdExpected(1234);
			static const bool negligibleCreditsThresholdFromFileExpected(true);

			static const bool employeeOverPlayerIdleCardOutExpected(true);
			static const bool employeeOverPlayerIdleCardOutFromFileExpected(true);

			static const WORD employeeIdleCardOutSecondsExpected(5678);
			static const bool employeeIdleCardOutSecondsFromFileExpected(true);

			static const bool playerRecardEmployeeCardOutExpected(true);
			static const bool playerRecardEmployeeCardOutFromFileExpected(true);

			// Setup.
			const int testIndex(0);
			CConfigExtFile obj;
			obj.SetNegligibleCreditsThreshold(negligibleCreditsThreshold);
			obj.SetEmployeeOverPlayerIdleCardOut(employeeOverPlayerIdleCardOut);
			obj.SetEmployeeIdleCardOutSeconds(employeeIdleCardOutSeconds);
			obj.SetPlayerRecardEmployeeCardOut(playerRecardEmployeeCardOut);

			// Verify non-default setup.
			AssertEqual(obj,
				negligibleCreditsThresholdExpected,
				negligibleCreditsThresholdFromFileExpected,
				employeeOverPlayerIdleCardOutExpected,
				employeeOverPlayerIdleCardOutFromFileExpected,
				employeeIdleCardOutSecondsExpected,
				employeeIdleCardOutSecondsFromFileExpected,
				playerRecardEmployeeCardOutExpected,
				playerRecardEmployeeCardOutFromFileExpected,
				m_notificationShowPromotionExpected,
				m_notificationShowPromotionFromFileExpected,
				m_notificationPlayerTrackingExpected,
				m_notificationPlayerTrackingFromFileExpected,
				m_customButton1EnableExpected,
				m_customButton1EnableFromFileExpected,
				m_customButton2EnableExpected,
				m_customButton2EnableFromFileExpected,
				m_customMenuButton1CountExpected,
				m_customMenuButton1CountFromFileExpected,
				m_customMenuButton2CountExpected,
				m_customMenuButton2CountFromFileExpected,
				m_resortMenuButtonCountExpected,
				m_resortMenuButtonCountFromFileExpected,
				testIndex, __TFUNCTION__, __LINE__);

			const CConfigExtFile configExt1;

			// Perform operation(s) to be tested.
			obj = configExt1;

			// Test(s).
			AssertEqual(obj,
				m_negligibleCreditsThresholdExpected,
				m_negligibleCreditsThresholdFromFileExpected,
				m_employeeOverPlayerIdleCardOutExpected,
				m_employeeOverPlayerIdleCardOutFromFileExpected,
				m_employeeIdleCardOutSecondsExpected,
				m_employeeIdleCardOutSecondsFromFileExpected,
				m_playerRecardEmployeeCardOutExpected,
				m_playerRecardEmployeeCardOutFromFileExpected,
				m_notificationShowPromotionExpected,
				m_notificationShowPromotionFromFileExpected,
				m_notificationPlayerTrackingExpected,
				m_notificationPlayerTrackingFromFileExpected,
				m_customButton1EnableExpected,
				m_customButton1EnableFromFileExpected,
				m_customButton2EnableExpected,
				m_customButton2EnableFromFileExpected,
				m_customMenuButton1CountExpected,
				m_customMenuButton1CountFromFileExpected,
				m_customMenuButton2CountExpected,
				m_customMenuButton2CountFromFileExpected,
				m_resortMenuButtonCountExpected,
				m_resortMenuButtonCountFromFileExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::operator=(const CConfigExtFile &)
		///     CConfigExtFile::GetNegligibleCreditsThreshold()
		///     CConfigExtFile::GetEmployeeOverPlayerIdleCardOut()
		///     CConfigExtFile::GetEmployeeIdleCardOutSeconds()
		///     CConfigExtFile::GetPlayerRecardEmployeeCardOut()
		/// Code coverage:
		///     CConfigExtFile::~CConfigExtFile()
		/// </summary>
		TEST_METHOD(AssignmentOperatorTest)
		{
			// Input(s).
			static const WORD negligibleCreditsThreshold(1234);
			static const bool employeeOverPlayerIdleCardOut(true);
			static const WORD employeeIdleCardOutSeconds(5678);
			static const bool playerRecardEmployeeCardOut(true);

			static const WORD negligibleCreditsThreshold1(11234);
			static const bool employeeOverPlayerIdleCardOut1(false);
			static const WORD employeeIdleCardOutSeconds1(15678);

			// Expected result(s).
			static const WORD negligibleCreditsThresholdExpected(1234);
			static const bool negligibleCreditsThresholdFromFileExpected(true);

			static const bool employeeOverPlayerIdleCardOutExpected(true);
			static const bool employeeOverPlayerIdleCardOutFromFileExpected(true);

			static const WORD employeeIdleCardOutSecondsExpected(5678);
			static const bool employeeIdleCardOutSecondsFromFileExpected(true);

			static const bool playerRecardEmployeeCardOutExpected(true);
			static const bool playerRecardEmployeeCardOutFromFileExpected(true);

			static const WORD negligibleCreditsThresholdExpected1(11234);
			static const bool negligibleCreditsThresholdFromFileExpected1(true);

			static const bool employeeOverPlayerIdleCardOutExpected1(false);
			static const bool employeeOverPlayerIdleCardOutFromFileExpected1(true);

			static const WORD employeeIdleCardOutSecondsExpected1(15678);
			static const bool employeeIdleCardOutSecondsFromFileExpected1(true);

			static const bool playerRecardEmployeeCardOutExpected1(false);
			static const bool playerRecardEmployeeCardOutFromFileExpected1(false);

			// Setup.
			const int testIndex(0);
			CConfigExtFile obj;
			obj.SetNegligibleCreditsThreshold(negligibleCreditsThreshold);
			obj.SetEmployeeOverPlayerIdleCardOut(employeeOverPlayerIdleCardOut);
			obj.SetEmployeeIdleCardOutSeconds(employeeIdleCardOutSeconds);
			obj.SetPlayerRecardEmployeeCardOut(playerRecardEmployeeCardOut);

			// Verify non-default setup.
			AssertEqual(obj,
				negligibleCreditsThresholdExpected,
				negligibleCreditsThresholdFromFileExpected,
				employeeOverPlayerIdleCardOutExpected,
				employeeOverPlayerIdleCardOutFromFileExpected,
				employeeIdleCardOutSecondsExpected,
				employeeIdleCardOutSecondsFromFileExpected,
				playerRecardEmployeeCardOutExpected,
				playerRecardEmployeeCardOutFromFileExpected,
				m_notificationShowPromotionExpected,
				m_notificationShowPromotionFromFileExpected,
				m_notificationPlayerTrackingExpected,
				m_notificationPlayerTrackingFromFileExpected,
				m_customButton1EnableExpected,
				m_customButton1EnableFromFileExpected,
				m_customButton2EnableExpected,
				m_customButton2EnableFromFileExpected,
				m_customMenuButton1CountExpected,
				m_customMenuButton1CountFromFileExpected,
				m_customMenuButton2CountExpected,
				m_customMenuButton2CountFromFileExpected,
				m_resortMenuButtonCountExpected,
				m_resortMenuButtonCountFromFileExpected,
				testIndex, __TFUNCTION__, __LINE__);

			CConfigExtFile varConfigExt1;
			varConfigExt1.SetNegligibleCreditsThreshold(negligibleCreditsThreshold1);
			varConfigExt1.SetEmployeeOverPlayerIdleCardOut(employeeOverPlayerIdleCardOutExpected1);
			varConfigExt1.SetEmployeeIdleCardOutSeconds(employeeIdleCardOutSecondsExpected1);
			const CConfigExtFile& configExt1(varConfigExt1);

			// Perform operation(s) to be tested.
			obj = configExt1;

			// Test(s).
			AssertEqual(obj,
				negligibleCreditsThresholdExpected1,
				negligibleCreditsThresholdFromFileExpected1,
				employeeOverPlayerIdleCardOutExpected1,
				employeeOverPlayerIdleCardOutFromFileExpected1,
				employeeIdleCardOutSecondsExpected1,
				employeeIdleCardOutSecondsFromFileExpected1,
				playerRecardEmployeeCardOutExpected1,
				playerRecardEmployeeCardOutFromFileExpected1,
				m_notificationShowPromotionExpected,
				m_notificationShowPromotionFromFileExpected,
				m_notificationPlayerTrackingExpected,
				m_notificationPlayerTrackingFromFileExpected,
				m_customButton1EnableExpected,
				m_customButton1EnableFromFileExpected,
				m_customButton2EnableExpected,
				m_customButton2EnableFromFileExpected,
				m_customMenuButton1CountExpected,
				m_customMenuButton1CountFromFileExpected,
				m_customMenuButton2CountExpected,
				m_customMenuButton2CountFromFileExpected,
				m_resortMenuButtonCountExpected,
				m_resortMenuButtonCountFromFileExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::CConfigExtFile()
		///     CConfigExtFile::SetNegligibleCreditsThreshold()
		///     CConfigExtFile::GetNegligibleCreditsThreshold()
		///     CConfigExtFile::SetEmployeeOverPlayerIdleCardOut()
		///     CConfigExtFile::GetEmployeeOverPlayerIdleCardOut()
		///     CConfigExtFile::SetEmployeeIdleCardOutSeconds()
		///     CConfigExtFile::GetEmployeeIdleCardOutSeconds()
		///     CConfigExtFile::SetPlayerRecardEmployeeCardOut()
		///     CConfigExtFile::GetPlayerRecardEmployeeCardOut()
		///     CConfigExtFile::SetDefaults()
		/// Code coverage:
		///     CConfigExtFile::~CConfigExtFile()
		/// </summary>
		TEST_METHOD(SetDefaultsTest)
		{
			// Input(s).
			static const WORD negligibleCreditsThreshold(1234);
			static const bool employeeOverPlayerIdleCardOut(true);
			static const WORD employeeIdleCardOutSeconds(5678);
			static const bool playerRecardEmployeeCardOut(true);

			// Expected result(s).
			static const WORD negligibleCreditsThresholdExpected(1234);
			static const bool negligibleCreditsThresholdFromFileExpected(true);

			static const bool employeeOverPlayerIdleCardOutExpected(true);
			static const bool employeeOverPlayerIdleCardOutFromFileExpected(true);

			static const WORD employeeIdleCardOutSecondsExpected(5678);
			static const bool employeeIdleCardOutSecondsFromFileExpected(true);

			static const bool playerRecardEmployeeCardOutExpected(true);
			static const bool playerRecardEmployeeCardOutFromFileExpected(true);



			// Setup.
			const int testIndex(0);
			CConfigExtFile obj;
			obj.SetNegligibleCreditsThreshold(negligibleCreditsThreshold);
			obj.SetEmployeeOverPlayerIdleCardOut(employeeOverPlayerIdleCardOut);
			obj.SetEmployeeIdleCardOutSeconds(employeeIdleCardOutSeconds);
			obj.SetPlayerRecardEmployeeCardOut(playerRecardEmployeeCardOut);

			// Verify non-default setup.
			AssertEqual(obj,
				negligibleCreditsThresholdExpected,
				negligibleCreditsThresholdFromFileExpected,
				employeeOverPlayerIdleCardOutExpected,
				employeeOverPlayerIdleCardOutFromFileExpected,
				employeeIdleCardOutSecondsExpected,
				employeeIdleCardOutSecondsFromFileExpected,
				playerRecardEmployeeCardOutExpected,
				playerRecardEmployeeCardOutFromFileExpected,
				m_notificationShowPromotionExpected,
				m_notificationShowPromotionFromFileExpected,
				m_notificationPlayerTrackingExpected,
				m_notificationPlayerTrackingFromFileExpected,
				m_customButton1EnableExpected,
				m_customButton1EnableFromFileExpected,
				m_customButton2EnableExpected,
				m_customButton2EnableFromFileExpected,
				m_customMenuButton1CountExpected,
				m_customMenuButton1CountFromFileExpected,
				m_customMenuButton2CountExpected,
				m_customMenuButton2CountFromFileExpected,
				m_resortMenuButtonCountExpected,
				m_resortMenuButtonCountFromFileExpected,
				testIndex, __TFUNCTION__, __LINE__);

			// Perform operation(s) to be tested.
			obj.SetDefaults();

			// Test(s).
			AssertEqual(obj,
				m_negligibleCreditsThresholdExpected,
				m_negligibleCreditsThresholdFromFileExpected,
				m_employeeOverPlayerIdleCardOutExpected,
				m_employeeOverPlayerIdleCardOutFromFileExpected,
				m_employeeIdleCardOutSecondsExpected,
				m_employeeIdleCardOutSecondsFromFileExpected,
				m_playerRecardEmployeeCardOutExpected,
				m_playerRecardEmployeeCardOutFromFileExpected,
				m_notificationShowPromotionExpected,
				m_notificationShowPromotionFromFileExpected,
				m_notificationPlayerTrackingExpected,
				m_notificationPlayerTrackingFromFileExpected,
				m_customButton1EnableExpected,
				m_customButton1EnableFromFileExpected,
				m_customButton2EnableExpected,
				m_customButton2EnableFromFileExpected,
				m_customMenuButton1CountExpected,
				m_customMenuButton1CountFromFileExpected,
				m_customMenuButton2CountExpected,
				m_customMenuButton2CountFromFileExpected,
				m_resortMenuButtonCountExpected,
				m_resortMenuButtonCountFromFileExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::SetNegligibleCreditsThreshold()
		///     CConfigExtFile::GetNegligibleCreditsThreshold()
		///     CConfigExtFile::SetDefaults()
		/// </summary>
		TEST_METHOD(SetNegligibleCreditsThresholdTest)
		{
			// Expected result(s).
			static const bool negligibleCreditsThresholdFromFileExpected(true);

			typedef struct
			{
				// Input(s).
				const WORD m_negligibleCreditsThreshold;

				// Expected result(s).
				const WORD m_negligibleCreditsThresholdExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (WORD)-1, (WORD)-1 },
				{ 0, 0 },
				{ 1, 1 },
				{ 2, 2 },
				{ 12345, 12345 },
				{ 0x1234, 0x1234 },

				{ (WORD)(SHRT_MAX - 1), (WORD)(SHRT_MAX -1) },
				{ SHRT_MAX, SHRT_MAX },
				{ (WORD)SHRT_MIN, (WORD)SHRT_MIN },
				{ (WORD)(SHRT_MIN + 1), (WORD)(SHRT_MIN + 1) },
				{ (WORD)(WORD_MAX - 1), (WORD)(WORD_MAX - 1) },
				{ SHRT_MAX, SHRT_MAX },
			};

			// Setup. Perform each test with the same instance.
			CConfigExtFile objA;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new object.
				CConfigExtFile objB;

				// Perform operation(s) to be tested.
				objA.SetNegligibleCreditsThreshold(TEST_DATA[testIndex].m_negligibleCreditsThreshold);
				objB.SetNegligibleCreditsThreshold(TEST_DATA[testIndex].m_negligibleCreditsThreshold);

				// Test(s).
				AssertEqual(objA,
					TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected,
					negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(objB,
					TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected,
					negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);

				objB.SetDefaults();
				AssertEqual(objB,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}

			objA.SetDefaults();
			AssertEqual(objA,
				m_negligibleCreditsThresholdExpected,
				m_negligibleCreditsThresholdFromFileExpected,
				m_employeeOverPlayerIdleCardOutExpected,
				m_employeeOverPlayerIdleCardOutFromFileExpected,
				m_employeeIdleCardOutSecondsExpected,
				m_employeeIdleCardOutSecondsFromFileExpected,
				m_playerRecardEmployeeCardOutExpected,
				m_playerRecardEmployeeCardOutFromFileExpected,
				m_notificationShowPromotionExpected,
				m_notificationShowPromotionFromFileExpected,
				m_notificationPlayerTrackingExpected,
				m_notificationPlayerTrackingFromFileExpected,
				m_customButton1EnableExpected,
				m_customButton1EnableFromFileExpected,
				m_customButton2EnableExpected,
				m_customButton2EnableFromFileExpected,
				m_customMenuButton1CountExpected,
				m_customMenuButton1CountFromFileExpected,
				m_customMenuButton2CountExpected,
				m_customMenuButton2CountFromFileExpected,
				m_resortMenuButtonCountExpected,
				m_resortMenuButtonCountFromFileExpected,
				0, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::SetEmployeeOverPlayerIdleCardOut()
		///     CConfigExtFile::GetEmployeeOverPlayerIdleCardOut()
		///     CConfigExtFile::SetDefaults()
		/// </summary>
		TEST_METHOD(SetEmployeeOverPlayerIdleCardOutTest)
		{
			// Expected result(s).
			static const bool employeeOverPlayerIdleCardOutFromFileExpected(true);

			typedef struct
			{
				// Input(s).
				const bool m_employeeOverPlayerIdleCardOut;

				// Expected result(s).
				const bool m_employeeOverPlayerIdleCardOutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ true, true },
				{ true, true },
				{ false, false },
				{ false, false },
				{ true, true },
			};

			// Setup. Perform each test with the same instance.
			CConfigExtFile objA;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new object.
				CConfigExtFile objB;

				// Perform operation(s) to be tested.
				objA.SetEmployeeOverPlayerIdleCardOut(TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOut);
				objB.SetEmployeeOverPlayerIdleCardOut(TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOut);

				// Test(s).
				AssertEqual(objA,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutExpected,
					employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(objB,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutExpected,
					employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);

				objB.SetDefaults();
				AssertEqual(objB,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}

			objA.SetDefaults();
			AssertEqual(objA,
				m_negligibleCreditsThresholdExpected,
				m_negligibleCreditsThresholdFromFileExpected,
				m_employeeOverPlayerIdleCardOutExpected,
				m_employeeOverPlayerIdleCardOutFromFileExpected,
				m_employeeIdleCardOutSecondsExpected,
				m_employeeIdleCardOutSecondsFromFileExpected,
				m_playerRecardEmployeeCardOutExpected,
				m_playerRecardEmployeeCardOutFromFileExpected,
				m_notificationShowPromotionExpected,
				m_notificationShowPromotionFromFileExpected,
				m_notificationPlayerTrackingExpected,
				m_notificationPlayerTrackingFromFileExpected,
				m_customButton1EnableExpected,
				m_customButton1EnableFromFileExpected,
				m_customButton2EnableExpected,
				m_customButton2EnableFromFileExpected,
				m_customMenuButton1CountExpected,
				m_customMenuButton1CountFromFileExpected,
				m_customMenuButton2CountExpected,
				m_customMenuButton2CountFromFileExpected,
				m_resortMenuButtonCountExpected,
				m_resortMenuButtonCountFromFileExpected,
				0, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::SetEmployeeIdleCardOutSeconds()
		///     CConfigExtFile::GetEmployeeIdleCardOutSeconds()
		///     CConfigExtFile::SetDefaults()
		/// </summary>
		TEST_METHOD(SetEmployeeIdleCardOutSecondsTest)
		{
			// Expected result(s).
			static const bool employeeIdleCardOutSecondsFromFileExpected(true);

			typedef struct
			{
				// Input(s).
				const WORD m_employeeIdleCardOutSeconds;

				// Expected result(s).
				const WORD m_employeeIdleCardOutSecondsExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (WORD)-1, (WORD)-1 },
				{ 0, 0 },
				{ 1, 1 },
				{ 2, 2 },
				{ 12345, 12345 },
				{ 0x1234, 0x1234 },

				{ (WORD)(SHRT_MAX - 1), (WORD)(SHRT_MAX - 1) },
				{ SHRT_MAX, SHRT_MAX },
				{ (WORD)SHRT_MIN, (WORD)SHRT_MIN },
				{ (WORD)(SHRT_MIN + 1), (WORD)(SHRT_MIN + 1) },
				{ (WORD)(WORD_MAX - 1), (WORD)(WORD_MAX - 1) },
				{ SHRT_MAX, SHRT_MAX },
			};

			// Setup. Perform each test with the same instance.
			CConfigExtFile objA;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new object.
				CConfigExtFile objB;

				// Perform operation(s) to be tested.
				objA.SetEmployeeIdleCardOutSeconds(TEST_DATA[testIndex].m_employeeIdleCardOutSeconds);
				objB.SetEmployeeIdleCardOutSeconds(TEST_DATA[testIndex].m_employeeIdleCardOutSeconds);

				// Test(s).
				AssertEqual(objA,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					TEST_DATA[testIndex].m_employeeIdleCardOutSecondsExpected,
					employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(objB,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					TEST_DATA[testIndex].m_employeeIdleCardOutSecondsExpected,
					employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);

				objB.SetDefaults();
				AssertEqual(objB,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}

			objA.SetDefaults();
			AssertEqual(objA,
				m_negligibleCreditsThresholdExpected,
				m_negligibleCreditsThresholdFromFileExpected,
				m_employeeOverPlayerIdleCardOutExpected,
				m_employeeOverPlayerIdleCardOutFromFileExpected,
				m_employeeIdleCardOutSecondsExpected,
				m_employeeIdleCardOutSecondsFromFileExpected,
				m_playerRecardEmployeeCardOutExpected,
				m_playerRecardEmployeeCardOutFromFileExpected,
				m_notificationShowPromotionExpected,
				m_notificationShowPromotionFromFileExpected,
				m_notificationPlayerTrackingExpected,
				m_notificationPlayerTrackingFromFileExpected,
				m_customButton1EnableExpected,
				m_customButton1EnableFromFileExpected,
				m_customButton2EnableExpected,
				m_customButton2EnableFromFileExpected,
				m_customMenuButton1CountExpected,
				m_customMenuButton1CountFromFileExpected,
				m_customMenuButton2CountExpected,
				m_customMenuButton2CountFromFileExpected,
				m_resortMenuButtonCountExpected,
				m_resortMenuButtonCountFromFileExpected,
				0, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::SetPlayerRecardEmployeeCardOut()
		///     CConfigExtFile::GetPlayerRecardEmployeeCardOut()
		///     CConfigExtFile::SetDefaults()
		/// </summary>
		TEST_METHOD(SetPlayerRecardEmployeeCardOutTest)
		{
			// Expected result(s).
			static const bool playerRecardEmployeeCardOutFromFileExpected(true);

			typedef struct
			{
				// Input(s).
				const bool m_playerRecardEmployeeCardOut;

				// Expected result(s).
				const bool m_playerRecardEmployeeCardOutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ true, true },
				{ true, true },
				{ false, false },
				{ false, false },
				{ true, true },
			};

			// Setup. Perform each test with the same instance.
			CConfigExtFile objA;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new object.
				CConfigExtFile objB;

				// Perform operation(s) to be tested.
				objA.SetPlayerRecardEmployeeCardOut(TEST_DATA[testIndex].m_playerRecardEmployeeCardOut);
				objB.SetPlayerRecardEmployeeCardOut(TEST_DATA[testIndex].m_playerRecardEmployeeCardOut);

				// Test(s).
				AssertEqual(objA,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					TEST_DATA[testIndex].m_playerRecardEmployeeCardOutExpected,
					playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(objB,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					TEST_DATA[testIndex].m_playerRecardEmployeeCardOutExpected,
					playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);

				objB.SetDefaults();
				AssertEqual(objB,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}

			objA.SetDefaults();
			AssertEqual(objA,
				m_negligibleCreditsThresholdExpected,
				m_negligibleCreditsThresholdFromFileExpected,
				m_employeeOverPlayerIdleCardOutExpected,
				m_employeeOverPlayerIdleCardOutFromFileExpected,
				m_employeeIdleCardOutSecondsExpected,
				m_employeeIdleCardOutSecondsFromFileExpected,
				m_playerRecardEmployeeCardOutExpected,
				m_playerRecardEmployeeCardOutFromFileExpected,
				m_notificationShowPromotionExpected,
				m_notificationShowPromotionFromFileExpected,
				m_notificationPlayerTrackingExpected,
				m_notificationPlayerTrackingFromFileExpected,
				m_customButton1EnableExpected,
				m_customButton1EnableFromFileExpected,
				m_customButton2EnableExpected,
				m_customButton2EnableFromFileExpected,
				m_customMenuButton1CountExpected,
				m_customMenuButton1CountFromFileExpected,
				m_customMenuButton2CountExpected,
				m_customMenuButton2CountFromFileExpected,
				m_resortMenuButtonCountExpected,
				m_resortMenuButtonCountFromFileExpected,
				0, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::SetDefaults()
		///     CConfigExtFile::SetCustomButton1Enable()
		///     CConfigExtFile::GetCustomButton1Enable()
		/// </summary>
		TEST_METHOD(SetCustomButton1EnableTest)
		{
			typedef struct
			{
				// Expected result(s).
				const bool m_Expected;
				const bool m_FromFileExpected;
			} TestDataType;


			const TestDataType TEST_DATA[] =
			{
				{ true, true },
				{ false, true }
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				CConfigExtFile obj;

				obj.SetCustomButton1Enabled(TEST_DATA[testIndex].m_Expected);

				// Test Set method
				AssertEqual(obj,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					TEST_DATA[testIndex].m_Expected,
					TEST_DATA[testIndex].m_FromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					0, __TFUNCTION__, __LINE__);

				// Test Set Defaults
				obj.SetDefaults();

				AssertEqual(obj,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					0, __TFUNCTION__, __LINE__);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::SetDefaults()
		///     CConfigExtFile::SetCustomButton2Enable()
		///     CConfigExtFile::GetCustomButton2Enable()
		/// </summary>
		TEST_METHOD(SetCustomButton2EnableTest)
		{
			typedef struct
			{
				// Expected result(s).
				const bool m_Expected;
				const bool m_FromFileExpected;
			} TestDataType;


			const TestDataType TEST_DATA[] =
			{
				{ true, true },
				{ false, true }
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				CConfigExtFile obj;

				obj.SetCustomButton2Enabled(TEST_DATA[testIndex].m_Expected);

				// Test Set method
				AssertEqual(obj,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					TEST_DATA[testIndex].m_Expected,
					TEST_DATA[testIndex].m_FromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					0, __TFUNCTION__, __LINE__);

				// Test Set Defaults
				obj.SetDefaults();

				AssertEqual(obj,
					m_negligibleCreditsThresholdExpected,
					m_negligibleCreditsThresholdFromFileExpected,
					m_employeeOverPlayerIdleCardOutExpected,
					m_employeeOverPlayerIdleCardOutFromFileExpected,
					m_employeeIdleCardOutSecondsExpected,
					m_employeeIdleCardOutSecondsFromFileExpected,
					m_playerRecardEmployeeCardOutExpected,
					m_playerRecardEmployeeCardOutFromFileExpected,
					m_notificationShowPromotionExpected,
					m_notificationShowPromotionFromFileExpected,
					m_notificationPlayerTrackingExpected,
					m_notificationPlayerTrackingFromFileExpected,
					m_customButton1EnableExpected,
					m_customButton1EnableFromFileExpected,
					m_customButton2EnableExpected,
					m_customButton2EnableFromFileExpected,
					m_customMenuButton1CountExpected,
					m_customMenuButton1CountFromFileExpected,
					m_customMenuButton2CountExpected,
					m_customMenuButton2CountFromFileExpected,
					m_resortMenuButtonCountExpected,
					m_resortMenuButtonCountFromFileExpected,
					0, __TFUNCTION__, __LINE__);
			}
		}

// #pragma region GetSessionActiveNegligibleCreditsTimeoutSeconds
		/// <summary>
		/// Test that member-related constants have the correct values.
		/// </summary>
		TEST_METHOD(GetSessionActiveNegligibleCreditsTimeoutSeconds_ConstTest)
		{
			// Perform test(s).
			Assert::AreEqual((WORD)10, CConfigExtData::DEFAULT_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			Assert::AreEqual((WORD)0, CConfigExtData::MINIMUM_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			Assert::AreEqual((WORD)WORD_MAX, CConfigExtData::MAXIMUM_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
		}

		/// <summary>
		/// Functional test for:
		///     CConfigExtFile::CConfigExtFile()
		///     CConfigExtFile::GetSessionActiveNegligibleCreditsTimeoutSeconds()
		/// Test getter for default value(s) from the default constructor.
		/// </summary>
		TEST_METHOD(GetSessionActiveNegligibleCreditsTimeoutSeconds_DefaultTest)
		{
			const bool setFromFileExpected(false);
			const WORD defaultExpected(CConfigExtData::DEFAULT_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);

			// Setup.
			WORD resultValue;

			// Perform operation(s) and test(s).
			CConfigExtFile obj;
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds defaultExpected"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue defaultExpected"));
		}

		/// <summary>
		/// Functional test for:
		///     CConfigExtFile::CConfigExtFile(const CConfig &)
		///     CConfigExtFile::GetSessionActiveNegligibleCreditsTimeoutSeconds()
		/// Test getter for configured value(s) from the CConfig constructor.
		/// </summary>
		TEST_METHOD(GetSessionActiveNegligibleCreditsTimeoutSeconds_ConfigTest)
		{
			const bool setFromFileExpected(false);
			const WORD resultExpected(567);

			// Setup.
			WORD resultValue;
			CConfig config;
			config.SetSessionActiveNegligibleCreditsTimeoutSeconds(resultExpected);

			// Perform operation(s) and test(s).
			CConfigExtFile obj(config);
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds defaultExpected"));
			Assert::AreEqual(resultExpected, resultValue, _T("resultValue defaultExpected"));
		}

		/// <summary>
		/// Functional test for:
		///     CConfigExtFile::SetSessionActiveNegligibleCreditsTimeoutSeconds()
		///     CConfigExtFile::GetSessionActiveNegligibleCreditsTimeoutSeconds()
		///     CConfigExtFile::SetDefaults()
		/// Test setter and getter for threshold values, an average value, and default value(s) from SetDefaults().
		/// </summary>
		TEST_METHOD(GetSessionActiveNegligibleCreditsTimeoutSecondsTest)
		{
			// Data.
			const bool setFromFileExpected(true);
			const WORD defaultExpected(CConfigExtData::DEFAULT_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			const WORD minimumExpected(CConfigExtData::MINIMUM_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			const WORD maximumExpected(CConfigExtData::MAXIMUM_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);

			// Setup.
			WORD resultValue;
			CConfigExtFile obj;

			// Perform operation(s) and test(s).

			// Test minimum threshold.
			obj.SetSessionActiveNegligibleCreditsTimeoutSeconds(minimumExpected);
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds minimumExpected"));
			Assert::AreEqual(minimumExpected, resultValue, _T("resultValue minimumExpected"));

			// Test one-off minimum threshold.
			obj.SetSessionActiveNegligibleCreditsTimeoutSeconds((WORD)(minimumExpected + 1));
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds minimumExpected + 1"));
			Assert::AreEqual((WORD)(minimumExpected + 1), resultValue, _T("resultValue minimumExpected + 1"));

			// Test average value(s).
			obj.SetSessionActiveNegligibleCreditsTimeoutSeconds((WORD)((minimumExpected + maximumExpected) / 2));
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds (min+max)/2"));
			Assert::AreEqual((WORD)((minimumExpected + maximumExpected) / 2), resultValue, _T("resultValue (min+max)/2"));

			// Test default value(s).
			obj.SetDefaults();
			Assert::AreEqual(false, obj.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds default"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue default"));

			// Test one-off maximum threshold.
			obj.SetSessionActiveNegligibleCreditsTimeoutSeconds((WORD)(maximumExpected - 1));
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds maximumExpected - 1"));
			Assert::AreEqual((WORD)(maximumExpected - 1), resultValue, _T("resultValue maximumExpected - 1"));

			// Test maximum threshold.
			obj.SetSessionActiveNegligibleCreditsTimeoutSeconds(maximumExpected);
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds maximumExpected"));
			Assert::AreEqual(maximumExpected, resultValue, _T("resultValue maximumExpected"));
		}

		/// <summary>
		/// Functional test for:
		///     CConfigExtFile::CConfigExtFile(const CConfigExtFile &)
		///     CConfigExtFile::GetSessionActiveNegligibleCreditsTimeoutSeconds()
		/// Test getter for non-default value(s) from the copy constructor.
		/// </summary>
		TEST_METHOD(GetSessionActiveNegligibleCreditsTimeoutSeconds_CopyTest)
		{
			const bool setFromFileExpected(true);
			const WORD resultExpected(555);

			// Setup.
			WORD resultValue;
			CConfigExtFile objA;
			objA.SetSessionActiveNegligibleCreditsTimeoutSeconds(resultExpected);
			Assert::AreEqual(setFromFileExpected, objA.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds maximumExpected")); // Verify setup.
			Assert::AreEqual(resultExpected, resultValue, _T("resultValue maximumExpected"));

			// Perform operation(s) and test(s).
			CConfigExtFile obj(objA);
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds defaultExpected"));
			Assert::AreEqual(resultExpected, resultValue, _T("resultValue defaultExpected"));
		}

		/// <summary>
		/// Functional test for:
		///     CConfigExtFile::operator=()
		///     CConfigExtFile::GetSessionActiveNegligibleCreditsTimeoutSeconds()
		/// Test getter for non-default value(s) from the assignment operator.
		/// Note that the setFromFile return value is different for default and non-default values.
		/// </summary>
		TEST_METHOD(GetSessionActiveNegligibleCreditsTimeoutSeconds_AssignmentNonDefaultTest)
		{
			const bool setFromFileSetupExpected(false);
			const bool setFromFileExpected(true);
			const WORD defaultExpected(CConfigExtData::DEFAULT_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			const WORD resultExpected(555);

			// Setup.
			WORD resultValue;
			CConfigExtFile objA;
			objA.SetSessionActiveNegligibleCreditsTimeoutSeconds(resultExpected);
			Assert::AreEqual(setFromFileExpected, objA.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds maximumExpected")); // Verify setup.
			Assert::AreEqual(resultExpected, resultValue, _T("resultValue maximumExpected"));

			CConfigExtFile obj;
			Assert::AreEqual(setFromFileSetupExpected, obj.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds maximumExpected")); // Verify setup.
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue maximumExpected"));

			// Perform operation(s) and test(s).
			obj = objA;
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds defaultExpected"));
			Assert::AreEqual(resultExpected, resultValue, _T("resultValue defaultExpected"));
		}

		/// <summary>
		/// Functional test for:
		///     CConfigExtFile::operator=()
		///     CConfigExtFile::GetSessionActiveNegligibleCreditsTimeoutSeconds()
		/// Test getter for default value(s) from the assignment operator.
		/// Note that the setFromFile return value is different for default and non-default values.
		/// </summary>
		TEST_METHOD(GetSessionActiveNegligibleCreditsTimeoutSeconds_AssignmentDefaultTest)
		{
			const bool setFromFileSetupExpected(true);
			const bool setFromFileExpected(false);
			const WORD defaultExpected(CConfigExtData::DEFAULT_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			const WORD setupExpected(555);

			// Setup.
			WORD resultValue;
			CConfigExtFile objA;
			Assert::AreEqual(setFromFileExpected, objA.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds maximumExpected")); // Verify setup.
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue maximumExpected"));

			CConfigExtFile obj;
			obj.SetSessionActiveNegligibleCreditsTimeoutSeconds(setupExpected);
			Assert::AreEqual(setFromFileSetupExpected, obj.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds maximumExpected")); // Verify setup.
			Assert::AreEqual(setupExpected, resultValue, _T("resultValue maximumExpected"));

			// Perform operation(s) and test(s).
			obj = objA;
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNegligibleCreditsTimeoutSeconds defaultExpected"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue defaultExpected"));
		}
// #pragma endregion GetSessionActiveNegligibleCreditsTimeoutSeconds

// #pragma region GetSessionActiveNonNegligibleCreditsTimeoutSeconds
		/// <summary>
		/// Test that member-related constants have the correct values.
		/// </summary>
		TEST_METHOD(GetSessionActiveNonNegligibleCreditsTimeoutSeconds_ConstTest)
		{
			// Perform test(s).
			Assert::AreEqual((WORD)300, CConfigExtData::DEFAULT_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			Assert::AreEqual((WORD)10, CConfigExtData::MINIMUM_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			Assert::AreEqual((WORD)WORD_MAX, CConfigExtData::MAXIMUM_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
		}

		/// <summary>
		/// Functional test for:
		///     CConfigExtFile::CConfigExtFile()
		///     CConfigExtFile::GetSessionActiveNonNegligibleCreditsTimeoutSeconds()
		/// Test getter for default value(s) from the default constructor.
		/// </summary>
		TEST_METHOD(GetSessionActiveNonNegligibleCreditsTimeoutSeconds_DefaultTest)
		{
			const bool setFromFileExpected(false);
			const WORD defaultExpected(CConfigExtData::DEFAULT_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);

			// Setup.
			WORD resultValue;

			// Perform operation(s) and test(s).
			CConfigExtFile obj;
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds defaultExpected"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue defaultExpected"));
		}

		/// <summary>
		/// Functional test for:
		///     CConfigExtFile::CConfigExtFile(const CConfig &)
		///     CConfigExtFile::GetSessionActiveNonNegligibleCreditsTimeoutSeconds()
		/// Test getter for configured value(s) that are within the valid range from the CConfig constructor.
		/// </summary>
		TEST_METHOD(GetSessionActiveNonNegligibleCreditsTimeoutSeconds_ConfigInRangeTest)
		{
			const bool setFromFileExpected(false);
			const WORD resultExpected(567);

			// Setup.
			WORD resultValue;
			CConfig config;
			config.SetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultExpected);

			// Perform operation(s) and test(s).
			CConfigExtFile obj(config);
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds defaultExpected"));
			Assert::AreEqual(resultExpected, resultValue, _T("resultValue defaultExpected"));
		}

		/// <summary>
		/// Functional test for:
		///     CConfigExtFile::CConfigExtFile(const CConfig &)
		///     CConfigExtFile::GetSessionActiveNonNegligibleCreditsTimeoutSeconds()
		/// Test getter for configured value(s) that are outside the valid range from the CConfig constructor.
		/// </summary>
		TEST_METHOD(GetSessionActiveNonNegligibleCreditsTimeoutSeconds_ConfigOutOfRangeTest)
		{
			const bool setFromFileExpected(false);
			const WORD defaultExpected(CConfigExtData::DEFAULT_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			const WORD minimumExpected(CConfigExtData::MINIMUM_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);

			// Setup.
			WORD resultValue;
			CConfig config;
			config.SetSessionActiveNonNegligibleCreditsTimeoutSeconds((WORD)(minimumExpected - 1));

			// Perform operation(s) and test(s).
			CConfigExtFile obj(config);
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds defaultExpected"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue defaultExpected"));
		}

		/// <summary>
		/// Functional test for:
		///     CConfigExtFile::SetSessionActiveNonNegligibleCreditsTimeoutSeconds()
		///     CConfigExtFile::GetSessionActiveNonNegligibleCreditsTimeoutSeconds()
		///     CConfigExtFile::SetDefaults()
		/// Test setter and getter for threshold values, an average value, and default value(s) from SetDefaults().
		/// </summary>
		TEST_METHOD(GetSessionActiveNonNegligibleCreditsTimeoutSecondsTest)
		{
			// Data.
			const bool setFromFileExpected(true);
			const WORD defaultExpected(CConfigExtData::DEFAULT_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			const WORD minimumExpected(CConfigExtData::MINIMUM_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			const WORD maximumExpected(CConfigExtData::MAXIMUM_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);

			// Setup.
			WORD resultValue;
			CConfigExtFile obj;

			// Perform operation(s) and test(s).

			// Test one-off minimum threshold.
			obj.SetSessionActiveNonNegligibleCreditsTimeoutSeconds((WORD)(minimumExpected - 1));
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds minimumExpected - 1"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue minimumExpected - 1"));

			// Test minimum threshold.
			obj.SetSessionActiveNonNegligibleCreditsTimeoutSeconds(minimumExpected);
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds minimumExpected"));
			Assert::AreEqual(minimumExpected, resultValue, _T("resultValue minimumExpected"));

			// Test one-off minimum threshold.
			obj.SetSessionActiveNonNegligibleCreditsTimeoutSeconds((WORD)(minimumExpected + 1));
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds minimumExpected + 1"));
			Assert::AreEqual((WORD)(minimumExpected + 1), resultValue, _T("resultValue minimumExpected + 1"));

			// Test average value(s).
			obj.SetSessionActiveNonNegligibleCreditsTimeoutSeconds((WORD)((minimumExpected + maximumExpected) / 2));
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds (min+max)/2"));
			Assert::AreEqual((WORD)((minimumExpected + maximumExpected) / 2), resultValue, _T("resultValue (min+max)/2"));

			// Test default value(s).
			obj.SetDefaults();
			Assert::AreEqual(false, obj.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds default"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue default"));

			// Test one-off maximum threshold.
			obj.SetSessionActiveNonNegligibleCreditsTimeoutSeconds((WORD)(maximumExpected - 1));
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds maximumExpected - 1"));
			Assert::AreEqual((WORD)(maximumExpected - 1), resultValue, _T("resultValue maximumExpected - 1"));

			// Test maximum threshold.
			obj.SetSessionActiveNonNegligibleCreditsTimeoutSeconds(maximumExpected);
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds maximumExpected"));
			Assert::AreEqual(maximumExpected, resultValue, _T("resultValue maximumExpected"));
		}

		/// <summary>
		/// Functional test for:
		///     CConfigExtFile::CConfigExtFile(const CConfigExtFile &)
		///     CConfigExtFile::GetSessionActiveNonNegligibleCreditsTimeoutSeconds()
		/// Test getter for non-default value(s) from the copy constructor.
		/// </summary>
		TEST_METHOD(GetSessionActiveNonNegligibleCreditsTimeoutSeconds_CopyTest)
		{
			const bool setFromFileExpected(true);
			const WORD resultExpected(555);

			// Setup.
			WORD resultValue;
			CConfigExtFile objA;
			objA.SetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultExpected);
			Assert::AreEqual(setFromFileExpected, objA.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds maximumExpected")); // Verify setup.
			Assert::AreEqual(resultExpected, resultValue, _T("resultValue maximumExpected"));

			// Perform operation(s) and test(s).
			CConfigExtFile obj(objA);
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds defaultExpected"));
			Assert::AreEqual(resultExpected, resultValue, _T("resultValue defaultExpected"));
		}

		/// <summary>
		/// Functional test for:
		///     CConfigExtFile::operator=()
		///     CConfigExtFile::GetSessionActiveNonNegligibleCreditsTimeoutSeconds()
		/// Test getter for non-default value(s) from the assignment operator.
		/// Note that the setFromFile return value is different for default and non-default values.
		/// </summary>
		TEST_METHOD(GetSessionActiveNonNegligibleCreditsTimeoutSeconds_AssignmentNonDefaultTest)
		{
			const bool setFromFileSetupExpected(false);
			const bool setFromFileExpected(true);
			const WORD defaultExpected(CConfigExtData::DEFAULT_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			const WORD resultExpected(555);

			// Setup.
			WORD resultValue;
			CConfigExtFile objA;
			objA.SetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultExpected);
			Assert::AreEqual(setFromFileExpected, objA.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds maximumExpected")); // Verify setup.
			Assert::AreEqual(resultExpected, resultValue, _T("resultValue maximumExpected"));

			CConfigExtFile obj;
			Assert::AreEqual(setFromFileSetupExpected, obj.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds maximumExpected")); // Verify setup.
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue maximumExpected"));

			// Perform operation(s) and test(s).
			obj = objA;
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds defaultExpected"));
			Assert::AreEqual(resultExpected, resultValue, _T("resultValue defaultExpected"));
		}

		/// <summary>
		/// Functional test for:
		///     CConfigExtFile::operator=()
		///     CConfigExtFile::GetSessionActiveNonNegligibleCreditsTimeoutSeconds()
		/// Test getter for default value(s) from the assignment operator.
		/// Note that the setFromFile return value is different for default and non-default values.
		/// </summary>
		TEST_METHOD(GetSessionActiveNonNegligibleCreditsTimeoutSeconds_AssignmentDefaultTest)
		{
			const bool setFromFileSetupExpected(true);
			const bool setFromFileExpected(false);
			const WORD defaultExpected(CConfigExtData::DEFAULT_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			const WORD setupExpected(555);

			// Setup.
			WORD resultValue;
			CConfigExtFile objA;
			Assert::AreEqual(setFromFileExpected, objA.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds maximumExpected")); // Verify setup.
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue maximumExpected"));

			CConfigExtFile obj;
			obj.SetSessionActiveNonNegligibleCreditsTimeoutSeconds(setupExpected);
			Assert::AreEqual(setFromFileSetupExpected, obj.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds maximumExpected")); // Verify setup.
			Assert::AreEqual(setupExpected, resultValue, _T("resultValue maximumExpected"));

			// Perform operation(s) and test(s).
			obj = objA;
			Assert::AreEqual(setFromFileExpected, obj.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(resultValue),
				_T("GetSessionActiveNonNegligibleCreditsTimeoutSeconds defaultExpected"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue defaultExpected"));
		}
// #pragma endregion GetSessionActiveNonNegligibleCreditsTimeoutSeconds

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::SetDefaults()
		///     CConfigExtFile::SetNotificationForShowPromotionTime()
		///     CConfigExtFile::GetNotificationForShowPromotionTime()
		/// </summary>
		TEST_METHOD(SetNotificationForShowPromotionTimeTest)
		{
			// Data.
			const bool setFromFileExpected(true);
			const WORD defaultExpected(30);
			const WORD minimumExpected(1);
			const WORD maximumExpected(60);
			Assert::AreEqual(defaultExpected, CConfigExtData::DEFAULT_NOTIFICATION_SHOW_PROMOTION);
			Assert::AreEqual(minimumExpected, CConfigExtData::MINIMUM_NOTIFICATION_SHOW_PROMOTION);
			Assert::AreEqual(maximumExpected, CConfigExtData::MAXIMUM_NOTIFICATION_SHOW_PROMOTION);

			// Setup.
			WORD resultValue;
			CConfigExtFile obj;

			// Perform operation(s) and test(s).
			obj.SetNotificationForShowPromotionTime((WORD)(minimumExpected - 1));
			Assert::AreEqual(setFromFileExpected, obj.GetNotificationForShowPromotionTime(resultValue),
				_T("GetNotificationForShowPromotionTime minimumExpected - 1"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue minimumExpected - 1"));

			obj.SetNotificationForShowPromotionTime(minimumExpected);
			Assert::AreEqual(setFromFileExpected, obj.GetNotificationForShowPromotionTime(resultValue),
				_T("GetNotificationForShowPromotionTime minimumExpected"));
			Assert::AreEqual((WORD)(minimumExpected), resultValue, _T("resultValue minimumExpected"));

			obj.SetNotificationForShowPromotionTime((WORD)(minimumExpected + 1));
			Assert::AreEqual(setFromFileExpected, obj.GetNotificationForShowPromotionTime(resultValue),
				_T("GetNotificationForShowPromotionTime minimumExpected + 1"));
			Assert::AreEqual((WORD)(minimumExpected + 1), resultValue, _T("resultValue minimumExpected + 1"));

			obj.SetNotificationForShowPromotionTime((WORD)((minimumExpected + maximumExpected) / 2));
			Assert::AreEqual(setFromFileExpected, obj.GetNotificationForShowPromotionTime(resultValue),
				_T("GetNotificationForShowPromotionTime (min+max)/2"));
			Assert::AreEqual((WORD)((minimumExpected + maximumExpected) / 2), resultValue, _T("resultValue (min+max)/2"));

			obj.SetNotificationForShowPromotionTime((WORD)(maximumExpected - 1));
			Assert::AreEqual(setFromFileExpected, obj.GetNotificationForShowPromotionTime(resultValue),
				_T("GetNotificationForShowPromotionTime maximumExpected - 1"));
			Assert::AreEqual((WORD)(maximumExpected - 1), resultValue, _T("resultValue maximumExpected - 1"));

			obj.SetNotificationForShowPromotionTime(maximumExpected);
			Assert::AreEqual(setFromFileExpected, obj.GetNotificationForShowPromotionTime(resultValue),
				_T("GetNotificationForShowPromotionTime maximumExpected"));
			Assert::AreEqual((WORD)(maximumExpected), resultValue, _T("resultValue maximumExpected"));

			obj.SetNotificationForShowPromotionTime((WORD)(maximumExpected + 1));
			Assert::AreEqual(setFromFileExpected, obj.GetNotificationForShowPromotionTime(resultValue),
				_T("GetNotificationForShowPromotionTime maximumExpected + 1"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue maximumExpected + 1"));
		}



		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::SetDefaults()
		///     CConfigExtFile::SetNotificationForPlayerTrackingTime()
		///     CConfigExtFile::GetNotificationForPlayerTrackingTime()
		/// </summary>
		TEST_METHOD(SetNotificationForPlayerTrackingTimeTest)
		{
			// Data.
			const bool setFromFileExpected(true);
			const WORD defaultExpected(3);
			const WORD minimumExpected(1);
			const WORD maximumExpected(60);
			Assert::AreEqual(defaultExpected, CConfigExtData::DEFAULT_NOTIFICATION_PLAYER_TRACKING);
			Assert::AreEqual(minimumExpected, CConfigExtData::MINIMUM_NOTIFICATION_PLAYER_TRACKING);
			Assert::AreEqual(maximumExpected, CConfigExtData::MAXIMUM_NOTIFICATION_PLAYER_TRACKING);

			// Setup.
			WORD resultValue;
			CConfigExtFile obj;

			// Perform operation(s) and test(s).
			obj.SetNotificationForPlayerTrackingTime((WORD)(minimumExpected - 1));
			Assert::AreEqual(setFromFileExpected, obj.GetNotificationForPlayerTrackingTime(resultValue),
				_T("GetNotificationForPlayerTrackingTime minimumExpected - 1"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue minimumExpected - 1"));

			obj.SetNotificationForPlayerTrackingTime(minimumExpected);
			Assert::AreEqual(setFromFileExpected, obj.GetNotificationForPlayerTrackingTime(resultValue),
				_T("GetNotificationForPlayerTrackingTime minimumExpected"));
			Assert::AreEqual((WORD)(minimumExpected), resultValue, _T("resultValue minimumExpected"));

			obj.SetNotificationForPlayerTrackingTime((WORD)(minimumExpected + 1));
			Assert::AreEqual(setFromFileExpected, obj.GetNotificationForPlayerTrackingTime(resultValue),
				_T("GetNotificationForPlayerTrackingTime minimumExpected + 1"));
			Assert::AreEqual((WORD)(minimumExpected + 1), resultValue, _T("resultValue minimumExpected + 1"));

			obj.SetNotificationForPlayerTrackingTime((WORD)((minimumExpected + maximumExpected) / 2));
			Assert::AreEqual(setFromFileExpected, obj.GetNotificationForPlayerTrackingTime(resultValue),
				_T("GetNotificationForPlayerTrackingTime (min+max)/2"));
			Assert::AreEqual((WORD)((minimumExpected + maximumExpected) / 2), resultValue, _T("resultValue (min+max)/2"));

			obj.SetNotificationForPlayerTrackingTime((WORD)(maximumExpected - 1));
			Assert::AreEqual(setFromFileExpected, obj.GetNotificationForPlayerTrackingTime(resultValue),
				_T("GetNotificationForPlayerTrackingTime maximumExpected - 1"));
			Assert::AreEqual((WORD)(maximumExpected - 1), resultValue, _T("resultValue maximumExpected - 1"));

			obj.SetNotificationForPlayerTrackingTime(maximumExpected);
			Assert::AreEqual(setFromFileExpected, obj.GetNotificationForPlayerTrackingTime(resultValue),
				_T("GetNotificationForPlayerTrackingTime maximumExpected"));
			Assert::AreEqual((WORD)(maximumExpected), resultValue, _T("resultValue maximumExpected"));

			obj.SetNotificationForPlayerTrackingTime((WORD)(maximumExpected + 1));
			Assert::AreEqual(setFromFileExpected, obj.GetNotificationForPlayerTrackingTime(resultValue),
				_T("GetNotificationForPlayerTrackingTime maximumExpected + 1"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue maximumExpected + 1"));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::SetDefaults()
		///     CConfigExtFile::SetCustomMenuButton1Count()
		///     CConfigExtFile::GetCustomMenuButton1Count()
		/// </summary>
		TEST_METHOD(GetCustomMenuButton1CountTest)
		{
			// Data.
			const bool setFromFileExpected(true);
			const WORD defaultExpected(0);
			const WORD minimumExpected(0);
			const WORD maximumExpected(6);
			Assert::AreEqual(defaultExpected, CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_1_COUNT);
			Assert::AreEqual(minimumExpected, CConfigExtData::MINIMUM_CUSTOM_MENU_BUTTON_1_COUNT);
			Assert::AreEqual(maximumExpected, CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_1_COUNT);

			// Setup.
			WORD resultValue;
			CConfigExtFile obj;

			// Perform operation(s) and test(s).
			obj.SetCustomMenuButton1Count((WORD)(minimumExpected - 1));
			Assert::AreEqual(setFromFileExpected, obj.GetCustomMenuButton1Count(resultValue),
				_T("GetCustomMenuButton1Count minimumExpected - 1"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue minimumExpected - 1"));

			obj.SetCustomMenuButton1Count(minimumExpected);
			Assert::AreEqual(setFromFileExpected, obj.GetCustomMenuButton1Count(resultValue),
				_T("GetCustomMenuButton1Count minimumExpected"));
			Assert::AreEqual((WORD)(minimumExpected), resultValue, _T("resultValue minimumExpected"));

			obj.SetCustomMenuButton1Count((WORD)(minimumExpected + 1));
			Assert::AreEqual(setFromFileExpected, obj.GetCustomMenuButton1Count(resultValue),
				_T("GetCustomMenuButton1Count minimumExpected + 1"));
			Assert::AreEqual((WORD)(minimumExpected + 1), resultValue, _T("resultValue minimumExpected + 1"));

			obj.SetCustomMenuButton1Count((WORD)((minimumExpected + maximumExpected) / 2));
			Assert::AreEqual(setFromFileExpected, obj.GetCustomMenuButton1Count(resultValue),
				_T("GetCustomMenuButton1Count (min+max)/2"));
			Assert::AreEqual((WORD)((minimumExpected + maximumExpected) / 2), resultValue, _T("resultValue (min+max)/2"));

			obj.SetCustomMenuButton1Count((WORD)(maximumExpected - 1));
			Assert::AreEqual(setFromFileExpected, obj.GetCustomMenuButton1Count(resultValue),
				_T("GetCustomMenuButton1Count maximumExpected - 1"));
			Assert::AreEqual((WORD)(maximumExpected - 1), resultValue, _T("resultValue maximumExpected - 1"));

			obj.SetCustomMenuButton1Count(maximumExpected);
			Assert::AreEqual(setFromFileExpected, obj.GetCustomMenuButton1Count(resultValue),
				_T("GetCustomMenuButton1Count maximumExpected"));
			Assert::AreEqual((WORD)(maximumExpected), resultValue, _T("resultValue maximumExpected"));

			obj.SetCustomMenuButton1Count((WORD)(maximumExpected + 1));
			Assert::AreEqual(setFromFileExpected, obj.GetCustomMenuButton1Count(resultValue),
				_T("GetCustomMenuButton1Count maximumExpected + 1"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue maximumExpected + 1"));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CConfigExtFile::SetDefaults()
		///     CConfigExtFile::SetCustomMenuButton2Count()
		///     CConfigExtFile::GetCustomMenuButton2Count()
		/// </summary>
		TEST_METHOD(GetCustomMenuButton2CountTest)
		{
			// Data.
			const bool setFromFileExpected(true);
			const WORD defaultExpected(0);
			const WORD minimumExpected(0);
			const WORD maximumExpected(6);
			Assert::AreEqual(defaultExpected, CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_2_COUNT);
			Assert::AreEqual(minimumExpected, CConfigExtData::MINIMUM_CUSTOM_MENU_BUTTON_2_COUNT);
			Assert::AreEqual(maximumExpected, CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_2_COUNT);

			// Setup.
			WORD resultValue;
			CConfigExtFile obj;

			// Perform operation(s) and test(s).
			obj.SetCustomMenuButton2Count((WORD)(minimumExpected - 1));
			Assert::AreEqual(setFromFileExpected, obj.GetCustomMenuButton2Count(resultValue),
				_T("GetCustomMenuButton2Count minimumExpected - 1"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue minimumExpected - 1"));

			obj.SetCustomMenuButton2Count(minimumExpected);
			Assert::AreEqual(setFromFileExpected, obj.GetCustomMenuButton2Count(resultValue),
				_T("GetCustomMenuButton2Count minimumExpected"));
			Assert::AreEqual((WORD)(minimumExpected), resultValue, _T("resultValue minimumExpected"));

			obj.SetCustomMenuButton2Count((WORD)(minimumExpected + 1));
			Assert::AreEqual(setFromFileExpected, obj.GetCustomMenuButton2Count(resultValue),
				_T("GetCustomMenuButton2Count minimumExpected + 1"));
			Assert::AreEqual((WORD)(minimumExpected + 1), resultValue, _T("resultValue minimumExpected + 1"));

			obj.SetCustomMenuButton2Count((WORD)((minimumExpected + maximumExpected) / 2));
			Assert::AreEqual(setFromFileExpected, obj.GetCustomMenuButton2Count(resultValue),
				_T("GetCustomMenuButton2Count (min+max)/2"));
			Assert::AreEqual((WORD)((minimumExpected + maximumExpected) / 2), resultValue, _T("resultValue (min+max)/2"));

			obj.SetCustomMenuButton2Count((WORD)(maximumExpected - 1));
			Assert::AreEqual(setFromFileExpected, obj.GetCustomMenuButton2Count(resultValue),
				_T("GetCustomMenuButton2Count maximumExpected - 1"));
			Assert::AreEqual((WORD)(maximumExpected - 1), resultValue, _T("resultValue maximumExpected - 1"));

			obj.SetCustomMenuButton2Count(maximumExpected);
			Assert::AreEqual(setFromFileExpected, obj.GetCustomMenuButton2Count(resultValue),
				_T("GetCustomMenuButton2Count maximumExpected"));
			Assert::AreEqual((WORD)(maximumExpected), resultValue, _T("resultValue maximumExpected"));

			obj.SetCustomMenuButton2Count((WORD)(maximumExpected + 1));
			Assert::AreEqual(setFromFileExpected, obj.GetCustomMenuButton2Count(resultValue),
				_T("GetCustomMenuButton2Count maximumExpected + 1"));
			Assert::AreEqual(defaultExpected, resultValue, _T("resultValue maximumExpected + 1"));
		}

	};
}
#endif //TODO-PORT