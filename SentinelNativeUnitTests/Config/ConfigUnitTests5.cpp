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
	TEST_CLASS(Config5)
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

// #pragma region UseHopperLevel
		/// <summary>
		/// Functional test for:
		///     CConfig::SetUseHopperLevel()
		///     CConfig::UseHopperLevel()
		/// Test setting a range of values.
		/// </summary>
		TEST_METHOD(SetUseHopperLevelTest)
		{
			// Data.
			typedef bool TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			m_testIndex = -1;
			CConfig varConfig(false, "");
			bool previousResult(true); // Prefer non-default.
			varConfig.SetUseHopperLevel(previousResult);
			AssertResult2(varConfig.UseHopperLevel(), previousResult);

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				AssertResult2(varConfig.SetUseHopperLevel(testValue), previousResult);
				const CConfig &config(varConfig);
				AssertResult2(config.UseHopperLevel(), expectedResult);

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
		///     CConfig::SetUseHopperLevel()
		///     CConfig::UseHopperLevel()
		/// Test setting a range of values.
		/// This normally tests saving and reading NVRAM values across restarts.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_METHOD(SetUseHopperLevel_NvRamTest)
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
				varConfig.SetUseHopperLevel(previousResult);
				AssertResult2(varConfig.UseHopperLevel(), previousResult);
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
				AssertResult2(varConfig.UseHopperLevel(), previousResult);
				AssertResult2(varConfig.SetUseHopperLevel(testValue), previousResult);
				const CConfig &config(varConfig);
				AssertResult2(config.UseHopperLevel(), expectedResult);

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
		///     CConfig::UseHopperLevel()
		///     CConfig::SetUseHopperLevel()
		///     CNVRAMConfig::Get()
		/// Test includes reading NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_METHOD(SetUseHopperLevel_NvRamGetTest)
		{
			// Data.
			const DWORD NVRAM_FIELD_OFFSET(43000);
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
				varConfig.SetUseHopperLevel(previousResult);
				AssertResult2(varConfig.UseHopperLevel(), previousResult);

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
				AssertResult2(config.UseHopperLevel(), previousResult);

				CNVRAMConfig nvramConfig;
				bool nvramResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramResult, NVRAM_FIELD_SIZE);
				AssertResult2(nvramResult, previousResult);

				// Perform operation(s) to be tested on next loop iteration.
				varConfig.SetUseHopperLevel(testValue);
				AssertResult2(varConfig.UseHopperLevel(), expectedResult);
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
		///     CConfig::UseHopperLevel()
		///     CNVRAMConfig::Set()
		/// Test includes writing NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_METHOD(SetUseHopperLevel_NvRamSetTest)
		{
			// Data.
			const DWORD NVRAM_FIELD_OFFSET(43000);
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
				AssertResult2(varConfig.UseHopperLevel(), previousResult);
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
				AssertResult2(config.UseHopperLevel(), previousResult);

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
		///     CConfig::SetUseHopperLevel()
		///     CConfig::UseHopperLevel()
		/// Test setting a range of values.
		/// This tests clearing NVRAM upon restart.
		/// </summary>
		TEST_METHOD(SetUseHopperLevel_ClearNvRamTest)
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
				varConfig1.SetUseHopperLevel(testValue);
				AssertResult2(varConfig1.UseHopperLevel(), expectedResult);

				// Verify setup NVRAM to non-reset state.
				CConfig varConfig2;
				varConfig2.BuildYourself();
				AssertResult2(varConfig2.UseHopperLevel(), expectedResult);

				// Set NVRAM invalid.
				CUnitTestingUtils::SetNvramInvalid();

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				AssertResult2(varConfig.UseHopperLevel(), defaultValue);
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
		///     CConfig::CheckForNVRAMUpgrade()
		///     CConfig::SetUseHopperLevel()
		///     CConfig::UseHopperLevel()
		/// </summary>
		/// <remarks>
		/// Verify that upgrading NVRAM has no effect on the configured value.
		/// </remarks>
		TEST_METHOD(SetUseHopperLevel_UpgradeNvRamTest)
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
				varConfig1.SetUseHopperLevel(testValue);
				AssertResult2(varConfig1.UseHopperLevel(), expectedResult);

				// Verify setup NVRAM to non-reset state.
				CConfig varConfig2;
				varConfig2.BuildYourself();
				AssertResult2(varConfig2.UseHopperLevel(), expectedResult);

				// Set NVRAM to need an maximum upgrade for all data that could be affected by an upgrade.
				CUnitTestingUtils::SetNvramNeedsUpgrade(NVRAM_ORIGINAL_VERSION);

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				AssertResult2(varConfig.UseHopperLevel(), expectedResult);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}
// #pragma endregion UseHopperLevel

// #pragma region VirtualSessionEnabled
		/// <summary>
		/// Functional test for:
		///     CConfig::SetVirtualSessionEnabled()
		///     CConfig::VirtualSessionEnabled()
		/// Test setting a range of values.
		/// </summary>
		TEST_METHOD(SetVirtualSessionEnabledTest)
		{
			// Data.
			typedef bool TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			m_testIndex = -1;
			CConfig varConfig(false, "");
			bool previousResult(true); // Prefer non-default.
			varConfig.SetVirtualSessionEnabled(previousResult);
			AssertResult2(varConfig.VirtualSessionEnabled(), previousResult);

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				AssertResult2(varConfig.SetVirtualSessionEnabled(testValue), previousResult);
				const CConfig &config(varConfig);
				AssertResult2(config.VirtualSessionEnabled(), expectedResult);

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
		///     CConfig::SetVirtualSessionEnabled()
		///     CConfig::VirtualSessionEnabled()
		/// Test setting a range of values.
		/// This normally tests saving and reading NVRAM values across restarts.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_METHOD(SetVirtualSessionEnabled_NvRamTest)
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
				varConfig.SetVirtualSessionEnabled(previousResult);
				AssertResult2(varConfig.VirtualSessionEnabled(), previousResult);
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
				AssertResult2(varConfig.VirtualSessionEnabled(), previousResult);
				AssertResult2(varConfig.SetVirtualSessionEnabled(testValue), previousResult);
				const CConfig &config(varConfig);
				AssertResult2(config.VirtualSessionEnabled(), expectedResult);

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
		///     CConfig::VirtualSessionEnabled()
		///     CConfig::SetVirtualSessionEnabled()
		///     CNVRAMConfig::Get()
		/// Test includes reading NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_METHOD(SetVirtualSessionEnabled_NvRamGetTest)
		{
			// Data.
			const DWORD NVRAM_FIELD_OFFSET(43001);
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
				varConfig.SetVirtualSessionEnabled(previousResult);
				AssertResult2(varConfig.VirtualSessionEnabled(), previousResult);

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
				AssertResult2(config.VirtualSessionEnabled(), previousResult);

				CNVRAMConfig nvramConfig;
				bool nvramResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramResult, NVRAM_FIELD_SIZE);
				AssertResult2(nvramResult, previousResult);

				// Perform operation(s) to be tested on next loop iteration.
				varConfig.SetVirtualSessionEnabled(testValue);
				AssertResult2(varConfig.VirtualSessionEnabled(), expectedResult);
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
		///     CConfig::VirtualSessionEnabled()
		///     CNVRAMConfig::Set()
		/// Test includes writing NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_METHOD(SetVirtualSessionEnabled_NvRamSetTest)
		{
			// Data.
			const DWORD NVRAM_FIELD_OFFSET(43001);
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
				AssertResult2(varConfig.VirtualSessionEnabled(), previousResult);
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
				AssertResult2(config.VirtualSessionEnabled(), previousResult);

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
		///     CConfig::SetVirtualSessionEnabled()
		///     CConfig::VirtualSessionEnabled()
		/// Test setting a range of values.
		/// This tests clearing NVRAM upon restart.
		/// </summary>
		TEST_METHOD(SetVirtualSessionEnabled_ClearNvRamTest)
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
				varConfig1.SetVirtualSessionEnabled(testValue);
				AssertResult2(varConfig1.VirtualSessionEnabled(), expectedResult);

				// Verify setup NVRAM to non-reset state.
				CConfig varConfig2;
				varConfig2.BuildYourself();
				AssertResult2(varConfig2.VirtualSessionEnabled(), expectedResult);

				// Set NVRAM invalid.
				CUnitTestingUtils::SetNvramInvalid();

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				AssertResult2(varConfig.VirtualSessionEnabled(), defaultValue);
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
		///     CConfig::CheckForNVRAMUpgrade()
		///     CConfig::SetVirtualSessionEnabled()
		///     CConfig::VirtualSessionEnabled()
		/// </summary>
		/// <remarks>
		/// Verify that upgrading NVRAM has no effect on the configured value.
		/// </remarks>
		TEST_METHOD(SetVirtualSessionEnabled_UpgradeNvRamTest)
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
				varConfig1.SetVirtualSessionEnabled(testValue);
				AssertResult2(varConfig1.VirtualSessionEnabled(), expectedResult);

				// Verify setup NVRAM to non-reset state.
				CConfig varConfig2;
				varConfig2.BuildYourself();
				AssertResult2(varConfig2.VirtualSessionEnabled(), expectedResult);

				// Set NVRAM to need an maximum upgrade for all data that could be affected by an upgrade.
				CUnitTestingUtils::SetNvramNeedsUpgrade(NVRAM_ORIGINAL_VERSION);

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				AssertResult2(varConfig.VirtualSessionEnabled(), expectedResult);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}
// #pragma endregion VirtualSessionEnabled

// #pragma region DisplayPlayerName
		/// <summary>
		/// Functional test for:
		///     CConfig::SetDisplayPlayerName()
		///     CConfig::DisplayPlayerName()
		/// Test setting a range of values.
		/// </summary>
		TEST_METHOD(SetDisplayPlayerNameTest)
		{
			// Data.
			typedef bool TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			m_testIndex = -1;
			CConfig varConfig(false, "");
			bool previousResult(true); // Prefer non-default.
			varConfig.SetDisplayPlayerName(previousResult);
			AssertResult2(varConfig.DisplayPlayerName(), previousResult);

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				AssertResult2(varConfig.SetDisplayPlayerName(testValue), previousResult);
				const CConfig &config(varConfig);
				AssertResult2(config.DisplayPlayerName(), expectedResult);

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
		///     CConfig::SetDisplayPlayerName()
		///     CConfig::DisplayPlayerName()
		/// Test setting a range of values.
		/// This normally tests saving and reading NVRAM values across restarts.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_METHOD(SetDisplayPlayerName_NvRamTest)
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
				varConfig.SetDisplayPlayerName(previousResult);
				AssertResult2(varConfig.DisplayPlayerName(), previousResult);
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
				AssertResult2(varConfig.DisplayPlayerName(), previousResult);
				AssertResult2(varConfig.SetDisplayPlayerName(testValue), previousResult);
				const CConfig &config(varConfig);
				AssertResult2(config.DisplayPlayerName(), expectedResult);

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
		///     CConfig::DisplayPlayerName()
		///     CConfig::SetDisplayPlayerName()
		///     CNVRAMConfig::Get()
		/// Test includes reading NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_METHOD(SetDisplayPlayerName_NvRamGetTest)
		{
			// Data.
			const DWORD NVRAM_FIELD_OFFSET(43002);
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
				varConfig.SetDisplayPlayerName(previousResult);
				AssertResult2(varConfig.DisplayPlayerName(), previousResult);

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
				AssertResult2(config.DisplayPlayerName(), previousResult);

				CNVRAMConfig nvramConfig;
				bool nvramResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramResult, NVRAM_FIELD_SIZE);
				AssertResult2(nvramResult, previousResult);

				// Perform operation(s) to be tested on next loop iteration.
				varConfig.SetDisplayPlayerName(testValue);
				AssertResult2(varConfig.DisplayPlayerName(), expectedResult);
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
		///     CConfig::DisplayPlayerName()
		///     CNVRAMConfig::Set()
		/// Test includes writing NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_METHOD(SetDisplayPlayerName_NvRamSetTest)
		{
			// Data.
			const DWORD NVRAM_FIELD_OFFSET(43002);
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
				AssertResult2(varConfig.DisplayPlayerName(), previousResult);
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
				AssertResult2(config.DisplayPlayerName(), previousResult);

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
		///     CConfig::SetDisplayPlayerName()
		///     CConfig::DisplayPlayerName()
		/// Test setting a range of values.
		/// This tests clearing NVRAM upon restart.
		/// </summary>
		TEST_METHOD(SetDisplayPlayerName_ClearNvRamTest)
		{
			// Data.
			typedef bool TestDataType;
			const TestDataType defaultValue(true);
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
				varConfig1.SetDisplayPlayerName(testValue);
				AssertResult2(varConfig1.DisplayPlayerName(), expectedResult);

				// Verify setup NVRAM to non-reset state.
				CConfig varConfig2;
				varConfig2.BuildYourself();
				AssertResult2(varConfig2.DisplayPlayerName(), expectedResult);

				// Set NVRAM invalid.
				CUnitTestingUtils::SetNvramInvalid();

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				AssertResult2(varConfig.DisplayPlayerName(), defaultValue);
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
		///     CConfig::CheckForNVRAMUpgrade()
		///     CConfig::SetDisplayPlayerName()
		///     CConfig::DisplayPlayerName()
		/// </summary>
		/// <remarks>
		/// Verify that upgrading NVRAM has no effect on the configured value.
		/// </remarks>
		TEST_METHOD(SetDisplayPlayerName_UpgradeNvRamTest)
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
				varConfig1.SetDisplayPlayerName(testValue);
				AssertResult2(varConfig1.DisplayPlayerName(), expectedResult);

				// Verify setup NVRAM to non-reset state.
				CConfig varConfig2;
				varConfig2.BuildYourself();
				AssertResult2(varConfig2.DisplayPlayerName(), expectedResult);

				// Set NVRAM to need an maximum upgrade for all data that could be affected by an upgrade.
				CUnitTestingUtils::SetNvramNeedsUpgrade(NVRAM_ORIGINAL_VERSION);

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				AssertResult2(varConfig.DisplayPlayerName(), expectedResult);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}
// #pragma endregion DisplayPlayerName
	};
}
#endif //TODO-PORT