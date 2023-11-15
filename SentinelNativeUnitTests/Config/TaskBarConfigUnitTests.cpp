#if 000//TODO-PORT
#include "stdafx.h"
#include "Config/UIConfig.h"

#include "Config/ConfigExtData.h"

// Unit testing includes.
#include "UnitTestingUtils.h"
#include "UnitTestingXmlData.h"
#include "UnitTestingXmlLite.h"

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a class initialization method.
#define AssertResult3i(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, 0, __TFUNCTION__, __LINE__);
#define AssertResult2i(result, expected) AssertResult3i(result, expected, _T(#result))

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	/// <summary>
	/// Test CUIConfig.
	/// </summary>
	TEST_CLASS(UIConfig)
	{
		// Expected value(s).
		static const WORD m_notificationForShowPromotionTimeInSecondsDefault = 30;
		static const WORD m_notificationForShowPromotionTimeInSecondsMinimum = 1;
		static const WORD m_notificationForShowPromotionTimeInSecondsMaximum = 60;
		static const DWORD m_notificationForShowPromotionTimeInMillisecondsDefault = 1000 * m_notificationForShowPromotionTimeInSecondsDefault;

		static const WORD m_notificationForPlayerTrackingTimeInSecondsDefault = 3;
		static const WORD m_notificationForPlayerTrackingTimeInSecondsMinimum = 1;
		static const WORD m_notificationForPlayerTrackingTimeInSecondsMaximum = 60;
		static const DWORD m_notificationForPlayerTrackingTimeInMillisecondsDefault = 1000 * m_notificationForPlayerTrackingTimeInSecondsDefault;

		static const bool m_customButton1EnabledDefault = false;
		static const bool m_customButton2EnabledDefault = false;

		static const WORD m_resortMenuButtonCountDefault = 1;
		static const WORD m_resortMenuButtonCountMinimum = 1;
		static const WORD m_resortMenuButtonCountMaximum = 6;

		static const WORD m_customMenuButton1CountDefault = 0;
		static const WORD m_customMenuButton1CountMinimum = 0;
		static const WORD m_customMenuButton1CountMaximum = 6;

		static const WORD m_customMenuButton2CountDefault = 0;
		static const WORD m_customMenuButton2CountMinimum = 0;
		static const WORD m_customMenuButton2CountMaximum = 6;

		// Class member variable(s).
		int m_testIndex;

		/// <summary>
		/// Initialize and/or test static class members.
		/// </summary>
		TEST_CLASS_INITIALIZE(ClassInitialize)
		{
			// Test static default value(s).
			AssertResult2i(m_notificationForShowPromotionTimeInSecondsDefault,
				CConfigExtData::DEFAULT_NOTIFICATION_SHOW_PROMOTION);
			AssertResult2i(m_notificationForShowPromotionTimeInSecondsMinimum,
				CConfigExtData::MINIMUM_NOTIFICATION_SHOW_PROMOTION);
			AssertResult2i(m_notificationForShowPromotionTimeInSecondsMaximum,
				CConfigExtData::MAXIMUM_NOTIFICATION_SHOW_PROMOTION);
			AssertResult2i(m_notificationForShowPromotionTimeInMillisecondsDefault,
				CConfigExtData::DEFAULT_NOTIFICATION_SHOW_PROMOTION * (DWORD)1000);

			AssertResult2i(m_notificationForPlayerTrackingTimeInSecondsDefault,
				CConfigExtData::DEFAULT_NOTIFICATION_PLAYER_TRACKING);
			AssertResult2i(m_notificationForPlayerTrackingTimeInSecondsMinimum,
				CConfigExtData::MINIMUM_NOTIFICATION_PLAYER_TRACKING);
			AssertResult2i(m_notificationForPlayerTrackingTimeInSecondsMaximum,
				CConfigExtData::MAXIMUM_NOTIFICATION_PLAYER_TRACKING);
			AssertResult2i(m_notificationForPlayerTrackingTimeInMillisecondsDefault,
				CConfigExtData::DEFAULT_NOTIFICATION_PLAYER_TRACKING * (DWORD)1000);

			AssertResult2i(m_customButton1EnabledDefault, CConfigExtData::DEFAULT_CUSTOM_BUTTON_1_ENABLE);
			AssertResult2i(m_customButton2EnabledDefault, CConfigExtData::DEFAULT_CUSTOM_BUTTON_2_ENABLE);

			AssertResult2i(m_customMenuButton1CountDefault, CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_1_COUNT);
			AssertResult2i(m_customMenuButton1CountMinimum, CConfigExtData::MINIMUM_CUSTOM_MENU_BUTTON_1_COUNT);
			AssertResult2i(m_customMenuButton1CountMaximum, CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_1_COUNT);

			AssertResult2i(m_customMenuButton2CountDefault, CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_2_COUNT);
			AssertResult2i(m_customMenuButton2CountMinimum, CConfigExtData::MINIMUM_CUSTOM_MENU_BUTTON_2_COUNT);
			AssertResult2i(m_customMenuButton2CountMaximum, CConfigExtData::MAXIMUM_CUSTOM_MENU_BUTTON_2_COUNT);
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
		///     CUIConfig::CUIConfig()
		/// Test with default values.
		/// </summary>
		TEST_METHOD(CUIConfigTest)
		{
			// Setup.
			const CUIConfig obj;

			// Test(s).
			AssertResult2(obj.GetNotificationForShowPromotionTimeInSeconds(), m_notificationForShowPromotionTimeInSecondsDefault);
			AssertResult2(obj.GetNotificationForShowPromotionTimeInMilliseconds(), m_notificationForShowPromotionTimeInMillisecondsDefault);
			AssertResult2(obj.GetNotificationForPlayerTrackingTimeInSeconds(), m_notificationForPlayerTrackingTimeInSecondsDefault);
			AssertResult2(obj.GetNotificationForPlayerTrackingTimeInMilliseconds(), m_notificationForPlayerTrackingTimeInMillisecondsDefault);
			AssertResult2(obj.GetCustomButton1Enabled(), m_customButton1EnabledDefault);
			AssertResult2(obj.GetCustomButton2Enabled(), m_customButton2EnabledDefault);
			AssertResult2(obj.GetCustomMenuButton1Count(), m_customMenuButton1CountDefault);
			AssertResult2(obj.GetCustomMenuButton2Count(), m_customMenuButton2CountDefault);
			AssertResult2(obj.GetResortMenuButtonCount(), m_resortMenuButtonCountDefault);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUIConfig::CUIConfig(CConfig)
		///     CUIConfig::GetNotificationForShowPromotionTimeInSeconds()
		///     CUIConfig::GetNotificationForShowPromotionTimeInMilliseconds()
		/// </summary>
		TEST_METHOD(CUIConfig_CConfig_GetNotificationForShowPromotionTimeTest)
		{
			// Data.
			WORD defaultResult(m_notificationForShowPromotionTimeInSecondsDefault);
			WORD minimumResult(m_notificationForShowPromotionTimeInSecondsMinimum);
			WORD maximumResult(m_notificationForShowPromotionTimeInSecondsMaximum);
			auto expectedResult = [=](WORD data) { return minimumResult <= data && data <= maximumResult ? data : defaultResult; };

			CXmlReaderTest testReader(CUnitTestingXmlData::GetWordData(minimumResult, maximumResult), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				WORD testData(testReader.GetNextAttributeAsWORD(CUnitTestingXmlData::TestData_ValueName));

				// Setup.
				CConfig varConfig(false, "");
				varConfig.SetNotificationForShowPromotionTime(testData);
				const CConfig &config(varConfig);

				// Perform operation(s) to be tested.
				CUIConfig obj(config);

				// Test(s).
				AssertResult2(obj.GetNotificationForShowPromotionTimeInSeconds(), expectedResult(testData));
				AssertResult2(obj.GetNotificationForShowPromotionTimeInMilliseconds(), expectedResult(testData) * (DWORD)1000);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUIConfig::operator=()
		///     CUIConfig::GetNotificationForShowPromotionTimeInSeconds()
		///     CUIConfig::GetNotificationForShowPromotionTimeInMilliseconds()
		/// </summary>
		TEST_METHOD(AssignmentOperator_GetNotificationForShowPromotionTimeTest)
		{
			// Data.
			WORD defaultResult(m_notificationForShowPromotionTimeInSecondsDefault);
			WORD minimumResult(m_notificationForShowPromotionTimeInSecondsMinimum);
			WORD maximumResult(m_notificationForShowPromotionTimeInSecondsMaximum);
			auto expectedResult = [=](WORD data) { return minimumResult <= data && data <= maximumResult ? data : defaultResult; };

			CXmlReaderTest testReader(CUnitTestingXmlData::GetWordData(minimumResult, maximumResult), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				WORD testData(testReader.GetNextAttributeAsWORD(CUnitTestingXmlData::TestData_ValueName));

				// Setup.
				CConfig varConfig(false, "");
				varConfig.SetNotificationForShowPromotionTime(testData);
				const CConfig &config(varConfig);
				CUIConfig testUIConfig(config);

				// Perform operation(s) to be tested.
				CUIConfig obj;
				obj = testUIConfig;

				// Test(s).
				AssertResult2(obj.GetNotificationForShowPromotionTimeInSeconds(), expectedResult(testData));
				AssertResult2(obj.GetNotificationForShowPromotionTimeInMilliseconds(), expectedResult(testData) * (DWORD)1000);

				// Restore defaults.
				CUIConfig defaultUIConfig;
				obj = defaultUIConfig;

				// Test(s).
				AssertResult2(obj.GetNotificationForShowPromotionTimeInSeconds(), m_notificationForShowPromotionTimeInSecondsDefault);
				AssertResult2(obj.GetNotificationForShowPromotionTimeInMilliseconds(), m_notificationForShowPromotionTimeInMillisecondsDefault);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUIConfig::CUIConfig(CConfig)
		///     CUIConfig::GetNotificationForPlayerTrackingTimeInSeconds()
		///     CUIConfig::GetNotificationForPlayerTrackingTimeInMilliseconds()
		/// </summary>
		TEST_METHOD(CUIConfig_CConfig_GetNotificationForPlayerTrackingTimeTest)
		{
			// Data.
			WORD defaultResult(m_notificationForPlayerTrackingTimeInSecondsDefault);
			WORD minimumResult(m_notificationForPlayerTrackingTimeInSecondsMinimum);
			WORD maximumResult(m_notificationForPlayerTrackingTimeInSecondsMaximum);
			auto expectedResult = [=](WORD data) { return minimumResult <= data && data <= maximumResult ? data : defaultResult; };

			CXmlReaderTest testReader(CUnitTestingXmlData::GetWordData(minimumResult, maximumResult), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				WORD testData(testReader.GetNextAttributeAsWORD(CUnitTestingXmlData::TestData_ValueName));

				// Setup.
				CConfig varConfig(false, "");
				varConfig.SetNotificationForPlayerTrackingTime(testData);
				const CConfig &config(varConfig);

				// Perform operation(s) to be tested.
				CUIConfig obj(config);

				// Test(s).
				AssertResult2(obj.GetNotificationForPlayerTrackingTimeInSeconds(), expectedResult(testData));
				AssertResult2(obj.GetNotificationForPlayerTrackingTimeInMilliseconds(), expectedResult(testData) * (DWORD)1000);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUIConfig::operator=()
		///     CUIConfig::GetNotificationForPlayerTrackingTimeInSeconds()
		///     CUIConfig::GetNotificationForShowPromotionTimeInMilliseconds()
		/// </summary>
		TEST_METHOD(AssignmentOperator_GetNotificationForPlayerTrackingTimeTest)
		{
			// Data.
			WORD defaultResult(m_notificationForPlayerTrackingTimeInSecondsDefault);
			WORD minimumResult(m_notificationForPlayerTrackingTimeInSecondsMinimum);
			WORD maximumResult(m_notificationForPlayerTrackingTimeInSecondsMaximum);
			auto expectedResult = [=](WORD data) { return minimumResult <= data && data <= maximumResult ? data : defaultResult; };

			CXmlReaderTest testReader(CUnitTestingXmlData::GetWordData(minimumResult, maximumResult), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				WORD testData(testReader.GetNextAttributeAsWORD(CUnitTestingXmlData::TestData_ValueName));

				// Setup.
				CConfig varConfig(false, "");
				varConfig.SetNotificationForPlayerTrackingTime(testData);
				const CConfig &config(varConfig);
				CUIConfig testUIConfig(config);

				// Perform operation(s) to be tested.
				CUIConfig obj;
				obj = testUIConfig;

				// Test(s).
				AssertResult2(obj.GetNotificationForPlayerTrackingTimeInSeconds(), expectedResult(testData));
				AssertResult2(obj.GetNotificationForPlayerTrackingTimeInMilliseconds(), expectedResult(testData) * (DWORD)1000);

				// Restore defaults.
				CUIConfig defaultUIConfig;
				obj = defaultUIConfig;

				// Test(s).
				AssertResult2(obj.GetNotificationForPlayerTrackingTimeInSeconds(), m_notificationForPlayerTrackingTimeInSecondsDefault);
				AssertResult2(obj.GetNotificationForPlayerTrackingTimeInMilliseconds(), m_notificationForPlayerTrackingTimeInMillisecondsDefault);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUIConfig::CUIConfig(CConfig)
		///     CUIConfig::GetCustomButton1Enabled()
		/// </summary>
		TEST_METHOD(CUIConfig_CConfig_GetCustomButton1EnabledTest)
		{
			// Data.
			CXmlReaderTest testReader(CUnitTestingXmlData::GetBoolData(), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				bool testData(testReader.GetNextAttributeAsBool(CUnitTestingXmlData::TestData_ValueName));

				// Setup.
				CConfig varConfig(false, "");
				varConfig.SetCustomButton1Enabled(testData);
				const CConfig &config(varConfig);

				// Perform operation(s) to be tested.
				CUIConfig obj(config);

				// Test(s).
				AssertResult2(obj.GetCustomButton1Enabled(), testData);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUIConfig::operator=()
		///     CUIConfig::GetCustomButton1Enabled()
		/// </summary>
		TEST_METHOD(AssignmentOperator_GetCustomButton1EnabledTest)
		{
			// Data.
			CXmlReaderTest testReader(CUnitTestingXmlData::GetBoolData(), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				bool testData(testReader.GetNextAttributeAsBool(CUnitTestingXmlData::TestData_ValueName));

				// Setup.
				CConfig varConfig(false, "");
				varConfig.SetCustomButton1Enabled(testData);
				const CConfig &config(varConfig);
				CUIConfig testUIConfig(config);

				// Perform operation(s) to be tested.
				CUIConfig obj;
				obj = testUIConfig;

				// Test(s).
				AssertResult2(obj.GetCustomButton1Enabled(), testData);

				// Restore defaults.
				CUIConfig defaultUIConfig;
				obj = defaultUIConfig;

				// Test(s).
				AssertResult2(obj.GetCustomButton1Enabled(), m_customButton1EnabledDefault);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUIConfig::CUIConfig(CConfig)
		///     CUIConfig::GetCustomButton2Enabled()
		/// </summary>
		TEST_METHOD(CUIConfig_CConfig_GetCustomButton2EnabledTest)
		{
			// Data.
			CXmlReaderTest testReader(CUnitTestingXmlData::GetBoolData(), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				bool testData(testReader.GetNextAttributeAsBool(CUnitTestingXmlData::TestData_ValueName));

				// Setup.
				CConfig varConfig(false, "");
				varConfig.SetCustomButton2Enabled(testData);
				const CConfig &config(varConfig);

				// Perform operation(s) to be tested.
				CUIConfig obj(config);

				// Test(s).
				AssertResult2(obj.GetCustomButton2Enabled(), testData);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUIConfig::operator=()
		///     CUIConfig::GetCustomButton2Enabled()
		/// </summary>
		TEST_METHOD(AssignmentOperator_GetCustomButton2EnabledTest)
		{
			// Data.
			CXmlReaderTest testReader(CUnitTestingXmlData::GetBoolData(), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				bool testData(testReader.GetNextAttributeAsBool(CUnitTestingXmlData::TestData_ValueName));

				// Setup.
				CConfig varConfig(false, "");
				varConfig.SetCustomButton2Enabled(testData);
				const CConfig &config(varConfig);
				CUIConfig testUIConfig(config);

				// Perform operation(s) to be tested.
				CUIConfig obj;
				obj = testUIConfig;

				// Test(s).
				AssertResult2(obj.GetCustomButton2Enabled(), testData);

				// Restore defaults.
				CUIConfig defaultUIConfig;
				obj = defaultUIConfig;

				// Test(s).
				AssertResult2(obj.GetCustomButton2Enabled(), m_customButton2EnabledDefault);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		
		/// <summary>
		/// Code coverage and functional test for:
		///     CUIConfig::CUIConfig(CConfig)
		///     CUIConfig::GetResortMenuButtonCount()
		/// </summary>
		TEST_METHOD(CUIConfig_CConfig_GetResortMenuButtonCountTest)
		{
			// Data.
			WORD defaultResult(m_resortMenuButtonCountDefault);
			WORD minimumResult(m_resortMenuButtonCountMinimum);
			WORD maximumResult(m_resortMenuButtonCountMaximum);
			auto expectedResult = [=](WORD data) { return minimumResult <= data && data <= maximumResult ? data : defaultResult; };

			CXmlReaderTest testReader(CUnitTestingXmlData::GetWordData(minimumResult, maximumResult), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				WORD testData(testReader.GetNextAttributeAsWORD(CUnitTestingXmlData::TestData_ValueName));

				// Setup.
				CConfig varConfig(false, "");
				varConfig.SetResortMenuButtonCount(testData);
				const CConfig &config(varConfig);

				// Perform operation(s) to be tested.
				CUIConfig obj(config);

				// Test(s).
				AssertResult2(obj.GetResortMenuButtonCount(), expectedResult(testData));
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUIConfig::CUIConfig(CConfig)
		///     CUIConfig::GetCustomMenuButton1Count()
		/// </summary>
		TEST_METHOD(CUIConfig_CConfig_GetCustomMenuButton1CountTest)
		{
			// Data.
			WORD defaultResult(m_customMenuButton1CountDefault);
			WORD minimumResult(m_customMenuButton1CountMinimum);
			WORD maximumResult(m_customMenuButton1CountMaximum);
			auto expectedResult = [=](WORD data) { return minimumResult <= data && data <= maximumResult ? data : defaultResult; };

			CXmlReaderTest testReader(CUnitTestingXmlData::GetWordData(minimumResult, maximumResult), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				WORD testData(testReader.GetNextAttributeAsWORD(CUnitTestingXmlData::TestData_ValueName));

				// Setup.
				CConfig varConfig(false, "");
				varConfig.SetCustomMenuButton1Count(testData);
				const CConfig &config(varConfig);

				// Perform operation(s) to be tested.
				CUIConfig obj(config);

				// Test(s).
				AssertResult2(obj.GetCustomMenuButton1Count(), expectedResult(testData));
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUIConfig::operator=()
		///     CUIConfig::GetResortMenuButton1Count()
		/// </summary>
		TEST_METHOD(AssignmentOperator_GetResortMenuButtonCountTest)
		{
			// Data.
			WORD defaultResult(m_resortMenuButtonCountDefault);
			WORD minimumResult(m_resortMenuButtonCountMinimum);
			WORD maximumResult(m_resortMenuButtonCountMaximum);
			auto expectedResult = [=](WORD data) { return minimumResult <= data && data <= maximumResult ? data : defaultResult; };

			CXmlReaderTest testReader(CUnitTestingXmlData::GetWordData(minimumResult, maximumResult), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				WORD testData(testReader.GetNextAttributeAsWORD(CUnitTestingXmlData::TestData_ValueName));

				// Setup.
				CConfig varConfig(false, "");
				varConfig.SetResortMenuButtonCount(testData);
				const CConfig &config(varConfig);
				CUIConfig testUIConfig(config);

				// Perform operation(s) to be tested.
				CUIConfig obj;
				obj = testUIConfig;

				// Test(s).
				AssertResult2(obj.GetResortMenuButtonCount(), expectedResult(testData));

				// Restore defaults.
				CUIConfig defaultUIConfig;
				obj = defaultUIConfig;

				// Test(s).
				AssertResult2(obj.GetResortMenuButtonCount(), m_resortMenuButtonCountDefault);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUIConfig::operator=()
		///     CUIConfig::GetCustomMenuButton1Count()
		/// </summary>
		TEST_METHOD(AssignmentOperator_GetCustomMenuButton1CountTest)
		{
			// Data.
			WORD defaultResult(m_customMenuButton1CountDefault);
			WORD minimumResult(m_customMenuButton1CountMinimum);
			WORD maximumResult(m_customMenuButton1CountMaximum);
			auto expectedResult = [=](WORD data) { return minimumResult <= data && data <= maximumResult ? data : defaultResult; };

			CXmlReaderTest testReader(CUnitTestingXmlData::GetWordData(minimumResult, maximumResult), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				WORD testData(testReader.GetNextAttributeAsWORD(CUnitTestingXmlData::TestData_ValueName));

				// Setup.
				CConfig varConfig(false, "");
				varConfig.SetCustomMenuButton1Count(testData);
				const CConfig &config(varConfig);
				CUIConfig testUIConfig(config);

				// Perform operation(s) to be tested.
				CUIConfig obj;
				obj = testUIConfig;

				// Test(s).
				AssertResult2(obj.GetCustomMenuButton1Count(), expectedResult(testData));

				// Restore defaults.
				CUIConfig defaultUIConfig;
				obj = defaultUIConfig;

				// Test(s).
				AssertResult2(obj.GetCustomMenuButton1Count(), m_customMenuButton1CountDefault);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUIConfig::CUIConfig(CConfig)
		///     CUIConfig::GetCustomMenuButton2Count()
		/// </summary>
		TEST_METHOD(CUIConfig_CConfig_GetCustomMenuButton2CountTest)
		{
			// Data.
			WORD defaultResult(m_customMenuButton2CountDefault);
			WORD minimumResult(m_customMenuButton2CountMinimum);
			WORD maximumResult(m_customMenuButton2CountMaximum);
			auto expectedResult = [=](WORD data) { return minimumResult <= data && data <= maximumResult ? data : defaultResult; };

			CXmlReaderTest testReader(CUnitTestingXmlData::GetWordData(minimumResult, maximumResult), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				WORD testData(testReader.GetNextAttributeAsWORD(CUnitTestingXmlData::TestData_ValueName));

				// Setup.
				CConfig varConfig(false, "");
				varConfig.SetCustomMenuButton2Count(testData);
				const CConfig &config(varConfig);

				// Perform operation(s) to be tested.
				CUIConfig obj(config);

				// Test(s).
				AssertResult2(obj.GetCustomMenuButton2Count(), expectedResult(testData));
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUIConfig::operator=()
		///     CUIConfig::GetCustomMenuButton2Count()
		/// </summary>
		TEST_METHOD(AssignmentOperator_GetCustomMenuButton2CountTest)
		{
			// Data.
			WORD defaultResult(m_customMenuButton2CountDefault);
			WORD minimumResult(m_customMenuButton2CountMinimum);
			WORD maximumResult(m_customMenuButton2CountMaximum);
			auto expectedResult = [=](WORD data) { return minimumResult <= data && data <= maximumResult ? data : defaultResult; };

			CXmlReaderTest testReader(CUnitTestingXmlData::GetWordData(minimumResult, maximumResult), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				WORD testData(testReader.GetNextAttributeAsWORD(CUnitTestingXmlData::TestData_ValueName));

				// Setup.
				CConfig varConfig(false, "");
				varConfig.SetCustomMenuButton2Count(testData);
				const CConfig &config(varConfig);
				CUIConfig testUIConfig(config);

				// Perform operation(s) to be tested.
				CUIConfig obj;
				obj = testUIConfig;

				// Test(s).
				AssertResult2(obj.GetCustomMenuButton2Count(), expectedResult(testData));

				// Restore defaults.
				CUIConfig defaultUIConfig;
				obj = defaultUIConfig;

				// Test(s).
				AssertResult2(obj.GetCustomMenuButton2Count(), m_customMenuButton2CountDefault);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		TEST_METHOD(UIConfigCopyConstructor)
		{
			// Data.
			WORD defaultResult(m_notificationForPlayerTrackingTimeInSecondsDefault);
			WORD minimumResult(m_notificationForPlayerTrackingTimeInSecondsMinimum);
			WORD maximumResult(m_notificationForPlayerTrackingTimeInSecondsMaximum);
			auto expectedResult = [=](WORD data) { return minimumResult <= data && data <= maximumResult ? data : defaultResult; };

			CXmlReaderTest testReader(CUnitTestingXmlData::GetWordData(minimumResult, maximumResult), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				WORD testData(testReader.GetNextAttributeAsWORD(CUnitTestingXmlData::TestData_ValueName));

				// Setup.
				CConfig varConfig(false, "");
				varConfig.SetNotificationForPlayerTrackingTime(testData);
				const CConfig &config(varConfig);
				CUIConfig testUIConfig(config);

				// Perform operation(s) to be tested.
				CUIConfig obj(testUIConfig);
				//obj = testUIConfig;

				// Test(s).
				AssertResult2(obj.GetNotificationForPlayerTrackingTimeInSeconds(), expectedResult(testData));
				AssertResult2(obj.GetNotificationForPlayerTrackingTimeInMilliseconds(), expectedResult(testData) * (DWORD)1000);

				// Restore defaults.
				CUIConfig defaultUIConfig;
				obj = defaultUIConfig;

				// Test(s).
				AssertResult2(obj.GetNotificationForPlayerTrackingTimeInSeconds(), m_notificationForPlayerTrackingTimeInSecondsDefault);
				AssertResult2(obj.GetNotificationForPlayerTrackingTimeInMilliseconds(), m_notificationForPlayerTrackingTimeInMillisecondsDefault);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}
	};
}
#endif //TODO-PORT