#if 000//TODO-PORT
#include "stdafx.h"
#include "Config.h"

#include "UnitTestingUtils.h"
#include "UnitTestingXmlData.h"
#include "UnitTestingXmlLite.h"

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(Config8)
	{
		// Class member(s) used by test (data) methods.
		int m_testIndex;

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
		/// Functional test for:
		///     CConfig::SetGameInProgress()
		///     CConfig::GetGameInProgress()
		/// Test setting a range of values.
		/// </summary>
		TEST_METHOD(SetGameInProgressTest)
		{
			// Data.
			typedef bool TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			m_testIndex = -1;
			CConfig varConfig(false, "");
			bool previousResult(true); // Prefer non-default.
			varConfig.SetGameInProgress(previousResult);
			AssertResult2(varConfig.GetGameInProgress(), previousResult);

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				AssertResult2(varConfig.SetGameInProgress(testValue), previousResult);
				const CConfig &config(varConfig);
				AssertResult2(config.GetGameInProgress(), expectedResult);

				// Setup for next test.
				previousResult = expectedResult;
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::SetRestartValues()
		///     CConfig::SetGameInProgress()
		///     CConfig::GetGameInProgress()
		/// Test setting a range of values.
		/// This normally tests saving and reading NVRAM values across restarts.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_METHOD(SetGameInProgress_NvRamTest)
		{
			// Data.
			typedef bool TestDataType;
			const TestDataType defaultValue(false);
			TestDataType previousResult(!defaultValue); // Prefer non-default.
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			{
				m_testIndex = -1;
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CConfig varConfig(false, "");
				varConfig.SetGameInProgress(previousResult);
				AssertResult2(varConfig.GetGameInProgress(), previousResult);
			}

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				AssertResult2(varConfig.GetGameInProgress(), defaultValue);
				AssertResult2(varConfig.SetGameInProgress(testValue), defaultValue);
				const CConfig &config(varConfig);
				AssertResult2(config.GetGameInProgress(), expectedResult);

				// Setup for next test.
				previousResult = expectedResult;
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::SetRestartValues()
		///     CConfig::GetGameInProgress()
		///     CConfig::SetGameInProgress()
		///     CNVRAMConfig::Get()
		/// Test includes reading NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_METHOD(SetGameInProgress_NvRamGetTest)
		{
			// Data.
			const DWORD NVRAM_FIELD_OFFSET(43184);
			const UINT NVRAM_FIELD_SIZE(1);

			typedef bool TestDataType;
			Assert::AreEqual(sizeof(TestDataType), NVRAM_FIELD_SIZE);
			const TestDataType defaultValue(false);
			TestDataType previousResult(!defaultValue); // Prefer non-default.
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			{
				m_testIndex = -1;
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CConfig varConfig(false, "");
				varConfig.SetGameInProgress(previousResult);
				AssertResult2(varConfig.GetGameInProgress(), previousResult);

				CNVRAMConfig nvramConfig;
				bool resultValue;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&resultValue, NVRAM_FIELD_SIZE);
				AssertResult2(resultValue, previousResult);
			}

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				AssertResult2(config.GetGameInProgress(), defaultValue);

				CNVRAMConfig nvramConfig;
				bool nvramResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramResult, NVRAM_FIELD_SIZE);
				AssertResult2(nvramResult, defaultValue);

				// Perform operation(s) to be tested on next loop iteration.
				varConfig.SetGameInProgress(testValue);
				AssertResult2(varConfig.GetGameInProgress(), testValue);
				previousResult = expectedResult;
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::SetRestartValues()
		///     CConfig::GetGameInProgress()
		///     CNVRAMConfig::Set()
		/// Test includes writing NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_METHOD(SetGameInProgress_NvRamSetTest)
		{
			// Data.
			const DWORD NVRAM_FIELD_OFFSET(43184);
			const UINT NVRAM_FIELD_SIZE(1);

			typedef bool TestDataType;
			Assert::AreEqual(sizeof(TestDataType), NVRAM_FIELD_SIZE);
			const TestDataType defaultValue(false);
			TestDataType previousResult(!defaultValue); // Prefer non-default.
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			{
				m_testIndex = -1;
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CNVRAMConfig nvramConfig;
				bool nvramValue(previousResult); // CNVRAMConfig::Set() requires read/write data.
				nvramConfig.Set(NVRAM_FIELD_OFFSET, (BYTE*)&nvramValue, NVRAM_FIELD_SIZE);
				bool nvramResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramResult, NVRAM_FIELD_SIZE);
				AssertResult2(nvramResult, previousResult);

				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				AssertResult2(varConfig.GetGameInProgress(), defaultValue);
			}

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				AssertResult2(config.GetGameInProgress(), defaultValue);

				// Perform operation(s) to be tested on next loop iteration.
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CNVRAMConfig nvramConfig;
				bool nvramValue(testValue); // CNVRAMConfig::Set() requires read/write data.
				nvramConfig.Set(NVRAM_FIELD_OFFSET, (BYTE*)&nvramValue, NVRAM_FIELD_SIZE);
				bool nvramResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramResult, NVRAM_FIELD_SIZE);
				AssertResult2(nvramResult, expectedResult);

				// Perform operation(s) to be tested on next loop iteration.
				previousResult = expectedResult;
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::ResetConfigMemory()
		///     CConfig::BuildYourself()
		///     CConfig::SetRestartValues()
		///     CConfig::SetGameInProgress()
		///     CConfig::GetGameInProgress()
		/// Test setting a range of values.
		/// This tests clearing NVRAM upon restart.
		/// </summary>
		TEST_METHOD(SetGameInProgress_ClearNvRamTest)
		{
			// Data.
			typedef bool TestDataType;
			const TestDataType defaultValue(false);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Setup.
				// Setup NVRAM to non-reset state.
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CConfig varConfig1;
				varConfig1.SetGameInProgress(testValue);
				AssertResult2(varConfig1.GetGameInProgress(), expectedResult);

				// Set NVRAM invalid.
				CUnitTestingUtils::SetNvramInvalid();

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				AssertResult2(varConfig.GetGameInProgress(), defaultValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CMemoryStorage::UpgradeNVRAM()
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::SetRestartValues()
		///     CConfig::CheckForNVRAMUpgrade()
		///     CConfig::SetGameInProgress()
		///     CConfig::GetGameInProgress()
		/// </summary>
		/// <remarks>
		/// Verify that upgrading NVRAM has no effect on the configured value.
		/// </remarks>
		TEST_METHOD(SetGameInProgress_UpgradeNvRamTest)
		{
			// Data.
			typedef bool TestDataType;
			const TestDataType defaultValue(false);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Setup.
				// Setup NVRAM to non-reset state.
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CConfig varConfig1;
				varConfig1.SetGameInProgress(testValue);
				AssertResult2(varConfig1.GetGameInProgress(), expectedResult);

				// Set NVRAM to need an maximum upgrade for all data that could be affected by an upgrade.
				CUnitTestingUtils::SetNvramNeedsUpgrade(NVRAM_ORIGINAL_VERSION);

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				AssertResult2(varConfig.GetGameInProgress(), defaultValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}
	};
}
#endif //TODO-PORT