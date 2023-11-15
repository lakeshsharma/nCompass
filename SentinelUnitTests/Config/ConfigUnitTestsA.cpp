
#include "stdafx.h"
#include "Config.h"
#include "UnitTestingUtils.h"
#if 000//TODO-PORT
//#include "UnitTestingXmlData.h"
//#include "UnitTestingXmlLite.h"
#endif

// Unit testing includes.
#include "gtest/gtest.h"

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

#define AssertResult5(result, resultLength, expected, expectedLength, resultName) \
	CUnitTestingUtils::AssertEqual(result, resultLength, expected, expectedLength, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult4(result, resultLength, expected, expectedLength) AssertResult5(result, resultLength, expected, expectedLength, _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a helper method.
#define AssertResult3h(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, functionName, lineNumber, extraIndex);
#define AssertResult2h(result, expected) AssertResult3h(result, expected, _T(#result))

// AssertResult macro for calling CUnitTestingUtils (with an extra index) from a test method.
#define AssertResult3x(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__, &extraIndex);
#define AssertResult2x(result, expected) AssertResult3x(result, expected, _T(#result))

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigAUnitTest : public ::testing::Test
	{
		// Class member(s) used by test (data) methods.
		int m_testIndex;

		virtual void SetUp()
		{
			// Test method initialization code.
			m_testIndex = 0;
		}

		virtual void TearDown()
		{
		}
	};

		/// <summary>
		/// Code coverage and functional test for:
		/// CConfig::SetNegligibleCreditsThreshold()
		/// CConfig::GetNegligibleCreditsThreshold()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetNegligibleCreditsThresholdTest)
		{
			const WORD initialNegligibleCreditsThreshold(12345);

			typedef struct
			{
				// Inputs.
				const WORD m_negligibleCreditsThreshold;

				// Expected results.
				const WORD m_oldNegligibleCreditsThresholdExpected;
				const WORD m_negligibleCreditsThresholdExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (WORD)-1, 12345, (WORD)-1 },

				{ 0, (WORD)-1, 0 },
				{ 1, 0, 1 },
				{ 2, 1, 2 },
				{ 12345, 2, 12345 },
				{ 0x1234, 12345, 0x1234 },
				{ (WORD)(SHRT_MAX - 1), 0x1234, (WORD)(SHRT_MAX - 1) },

				{ SHRT_MAX, (WORD)(SHRT_MAX - 1), SHRT_MAX },
				{ (WORD)SHRT_MIN, SHRT_MAX, (WORD)SHRT_MIN },
				{ (WORD)(SHRT_MIN + 1), (WORD)SHRT_MIN, (WORD)(SHRT_MIN + 1) },
				{ (WORD)-12345, (WORD)(SHRT_MIN + 1), (WORD)-12345 },

				{ (WORD)(WORD_MAX - 1), (WORD)-12345, (WORD)(WORD_MAX - 1) },
				{ WORD_MAX, (WORD)(WORD_MAX - 1), WORD_MAX },
				{ 0, WORD_MAX, 0 },
			};

			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetNegligibleCreditsThreshold(initialNegligibleCreditsThreshold);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				const WORD oldNegligibleCreditsThresholdResult = varConfig.SetNegligibleCreditsThreshold(TEST_DATA[testIndex].m_negligibleCreditsThreshold);
				const CConfig &config(varConfig);
				const WORD negligibleCreditsThresholdResult = config.GetNegligibleCreditsThreshold();

				// Test #1.
				{
					const bool areEqual(oldNegligibleCreditsThresholdResult == TEST_DATA[testIndex].m_oldNegligibleCreditsThresholdExpected);

					string failedMsg;
					failedMsg = FormatString(_T("oldNegligibleCreditsThresholdResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)oldNegligibleCreditsThresholdResult,
						(int)TEST_DATA[testIndex].m_oldNegligibleCreditsThresholdExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.
				{
					const bool areEqual(negligibleCreditsThresholdResult == TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected);

					string failedMsg;
					failedMsg = FormatString(_T("negligibleCreditsThresholdResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)negligibleCreditsThresholdResult,
						(int)TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::SetNegligibleCreditsThreshold()
		///     CConfig::GetNegligibleCreditsThreshold()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetNegligibleCreditsThreshold_NvRamTest)
		{
			const WORD initialNegligibleCreditsThreshold(12345);

			typedef struct
			{
				// Inputs.
				const WORD m_negligibleCreditsThreshold;

				// Expected results.
				const WORD m_negligibleCreditsThresholdExpected;
				const WORD m_oldNegligibleCreditsThresholdExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (WORD)-1, 12345, 12345 },

				{ 0, (WORD)-1, (WORD)-1 },
				{ 1, 0, 0 },
				{ 2, 1, 1 },
				{ 12345, 2, 2 },
				{ 0x1234, 12345, 12345 },
				{ (WORD)(SHRT_MAX - 1), 0x1234, 0x1234 },

				{ SHRT_MAX, (WORD)(SHRT_MAX - 1), (WORD)(SHRT_MAX - 1) },
				{ (WORD)SHRT_MIN, SHRT_MAX, SHRT_MAX },
				{ (WORD)(SHRT_MIN + 1), (WORD)SHRT_MIN, (WORD)SHRT_MIN },
				{ (WORD)-12345, (WORD)(SHRT_MIN + 1), (WORD)(SHRT_MIN + 1) },

				{ (WORD)(WORD_MAX - 1), (WORD)-12345, (WORD)-12345 },
				{ WORD_MAX, (WORD)(WORD_MAX - 1), (WORD)(WORD_MAX - 1) },
				{ 0, WORD_MAX, WORD_MAX },
			};

			// Setup.
			CMemoryStorage::SetSprinkles();
			CMemoryStorage::UpgradeNVRAM();
			CMemoryStorage::UpgradeNVRAM(); // Set memory storage to already upgraded.
			CConfig varConfig(false, "");
			varConfig.SetNegligibleCreditsThreshold(initialNegligibleCreditsThreshold);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const WORD negligibleCreditsThresholdResult = config.GetNegligibleCreditsThreshold();
				const WORD oldNegligibleCreditsThresholdResult = varConfig.SetNegligibleCreditsThreshold(TEST_DATA[testIndex].m_negligibleCreditsThreshold);

				// Test #1.
				{
					const bool areEqual(negligibleCreditsThresholdResult == TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected);

					string failedMsg;
					failedMsg = FormatString(_T("negligibleCreditsThresholdResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)negligibleCreditsThresholdResult,
						(int)TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.
				{
					const bool areEqual(oldNegligibleCreditsThresholdResult == TEST_DATA[testIndex].m_oldNegligibleCreditsThresholdExpected);

					string failedMsg;
					failedMsg = FormatString(_T("oldNegligibleCreditsThresholdResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)oldNegligibleCreditsThresholdResult,
						(int)TEST_DATA[testIndex].m_oldNegligibleCreditsThresholdExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::SetNegligibleCreditsThreshold()
		///     CConfig::GetNegligibleCreditsThreshold()
		///     CNVRAMConfig::Get()
		/// Test includes reading NVRAM contents.
		/// This tests forward and backward compatibility across versions.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetNegligibleCreditsThreshold_NvRamGetTest)
		{
			// Test data -- input(s).
			const DWORD NVRAM_FIELD_OFFSET(42685);
			const UINT NVRAM_FIELD_SIZE(2);
			const WORD negligibleCreditsThresholdInitial(12345);
			const WORD negligibleCreditsThresholdInitialExpected(12345);
			ASSERT_TRUE(NVRAM_FIELD_SIZE == sizeof(negligibleCreditsThresholdInitial));

			// Test data.
			typedef struct
			{
				// Input(s).
				const WORD m_negligibleCreditsThreshold;

				// Expected result(s).
				const WORD m_negligibleCreditsThresholdExpected;
				const WORD m_nvramNegligibleCreditsThresholdExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (WORD)-1, 12345, 12345 },

				{ 0, (WORD)-1, (WORD)-1 },
				{ 1, 0, 0 },
				{ 2, 1, 1 },
				{ 12345, 2, 2 },
				{ 0x1234, 12345, 12345 },
				{ (WORD)(SHRT_MAX - 1), 0x1234, 0x1234 },

				{ SHRT_MAX, (WORD)(SHRT_MAX - 1), (WORD)(SHRT_MAX - 1) },
				{ (WORD)SHRT_MIN, SHRT_MAX, SHRT_MAX },
				{ (WORD)(SHRT_MIN + 1), (WORD)SHRT_MIN, (WORD)SHRT_MIN },

				{ WORD_MAX, (WORD)(SHRT_MIN + 1), (WORD)(SHRT_MIN + 1) },
				{ 0, WORD_MAX, WORD_MAX },
			};

			// Setup.
			{
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CConfig varConfig(false, "");
				varConfig.SetNegligibleCreditsThreshold(negligibleCreditsThresholdInitial);

				CNVRAMConfig nvramConfig;
				WORD negligibleCreditsThresholdResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&negligibleCreditsThresholdResult, NVRAM_FIELD_SIZE);
				ASSERT_TRUE(negligibleCreditsThresholdResult == negligibleCreditsThresholdInitialExpected);
			}

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const WORD negligibleCreditsThresholdResult = config.GetNegligibleCreditsThreshold();

				CNVRAMConfig nvramConfig;
				WORD nvramNegligibleCreditsThresholdResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramNegligibleCreditsThresholdResult, NVRAM_FIELD_SIZE);

				// Test #1.
				{
					const bool equal(negligibleCreditsThresholdResult == TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected);
					if (!equal)
					{
						string failedMsg;
						failedMsg = FormatString(_T("negligibleCreditsThresholdResult=%d expected=%d testIndex=%d in %s(%d)"),
							(int)negligibleCreditsThresholdResult,
							(int)TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
						ASSERT_TRUE(false) << failedMsg;
					}
				}

				// Test #2.
				{
					const bool equal(nvramNegligibleCreditsThresholdResult == TEST_DATA[testIndex].m_nvramNegligibleCreditsThresholdExpected);
					if (!equal)
					{
						string failedMsg;
						failedMsg = FormatString(_T("nvramNegligibleCreditsThresholdResult=%d expected=%d testIndex=%d in %s(%d)"),
							(int)nvramNegligibleCreditsThresholdResult,
							(int)TEST_DATA[testIndex].m_nvramNegligibleCreditsThresholdExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
						ASSERT_TRUE(false) << failedMsg;
					}
				}

				// Perform operation(s) to be tested on next loop iteration.
				varConfig.SetNegligibleCreditsThreshold(TEST_DATA[testIndex].m_negligibleCreditsThreshold);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::GetNegligibleCreditsThreshold()
		///     CNVRAMConfig::Set()
		/// Test includes writing NVRAM contents.
		/// This tests forward and backward compatibility across versions.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetNegligibleCreditsThreshold_NvRamSetTest)
		{
			// Test data -- input(s).
			const DWORD NVRAM_FIELD_OFFSET(42685);
			const UINT NVRAM_FIELD_SIZE(2);
			const WORD negligibleCreditsThresholdInitial(12345);
			const WORD negligibleCreditsThresholdInitialExpected(12345);
			ASSERT_TRUE(NVRAM_FIELD_SIZE == sizeof(negligibleCreditsThresholdInitial));

			// Test data.
			typedef struct
			{
				// Input(s).
				const WORD m_negligibleCreditsThreshold;

				// Expected result(s).
				const WORD m_negligibleCreditsThresholdExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (WORD)-1, 12345 },

				{ 0, (WORD)-1 },
				{ 1, 0 },
				{ 2, 1 },
				{ 12345, 2 },
				{ 0x1234, 12345 },
				{ (WORD)(SHRT_MAX - 1), 0x1234 },

				{ SHRT_MAX, (WORD)(SHRT_MAX - 1) },
				{ (WORD)SHRT_MIN, SHRT_MAX },
				{ (WORD)(SHRT_MIN + 1), (WORD)SHRT_MIN },

				{ WORD_MAX, (WORD)(SHRT_MIN + 1) },
				{ 0, WORD_MAX },
			};

			// Setup.
			{
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CNVRAMConfig nvramConfig;
				WORD negligibleCreditsThreshold(negligibleCreditsThresholdInitial); // CNVRAMConfig::Set() requires read/write data.
				nvramConfig.Set(NVRAM_FIELD_OFFSET, (BYTE*)&negligibleCreditsThreshold, NVRAM_FIELD_SIZE);

				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				ASSERT_TRUE(varConfig.GetNegligibleCreditsThreshold() == negligibleCreditsThresholdInitialExpected) <<  "Setup";
			}

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const WORD negligibleCreditsThresholdResult = config.GetNegligibleCreditsThreshold();

				// Test #1.
				{
					const bool equal(negligibleCreditsThresholdResult == TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected);
					if (!equal)
					{
						string failedMsg;
						failedMsg = FormatString(_T("negligibleCreditsThresholdResult=%d expected=%d testIndex=%d in %s(%d)"),
							(int)negligibleCreditsThresholdResult,
							(int)TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
						ASSERT_TRUE(false) << failedMsg;
					}
				}

				// Perform operation(s) to be tested on next loop iteration.
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CNVRAMConfig nvramConfig;
				WORD negligibleCreditsThreshold(TEST_DATA[testIndex].m_negligibleCreditsThreshold); // CNVRAMConfig::Set() requires read/write data.
				nvramConfig.Set(NVRAM_FIELD_OFFSET, (BYTE*)&negligibleCreditsThreshold, NVRAM_FIELD_SIZE);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::ResetConfigMemory()
		///     CConfig::BuildYourself()
		///     CConfig::SetNegligibleCreditsThreshold()
		///     CConfig::GetNegligibleCreditsThreshold()
		/// Note that the configuration value is uninitialized after NVRAM is cleared or lost.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetNegligibleCreditsThreshold_ClearNvRamTest)
		{
			const WORD negligibleCreditsThresholdResetValue(0); // Expected value after NVRAM is cleared or lost.

			typedef struct
			{
				// Input(s).
				const WORD m_negligibleCreditsThreshold;

				// Expected result(s).
				const WORD m_negligibleCreditsThresholdExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (WORD)-1, negligibleCreditsThresholdResetValue },

				{ 0, negligibleCreditsThresholdResetValue },
				{ 1, negligibleCreditsThresholdResetValue },
				{ 2, negligibleCreditsThresholdResetValue },
				{ 12345, negligibleCreditsThresholdResetValue },
				{ 0x1234, negligibleCreditsThresholdResetValue },
				{ (WORD)(SHRT_MAX - 1), negligibleCreditsThresholdResetValue },

				{ (WORD)SHRT_MAX, negligibleCreditsThresholdResetValue },
				{ (WORD)SHRT_MIN, negligibleCreditsThresholdResetValue },
				{ (WORD)(SHRT_MIN + 1), negligibleCreditsThresholdResetValue },

				{ (WORD)(WORD_MAX - 1), negligibleCreditsThresholdResetValue },
				{ (WORD)WORD_MAX, negligibleCreditsThresholdResetValue },
				{ 0, negligibleCreditsThresholdResetValue },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Setup NVRAM to non-reset state.
				CMemoryStorage::SetSprinkles();
				CMemoryStorage::UpgradeNVRAM();
				CMemoryStorage::UpgradeNVRAM(); // Set memory storage to already upgraded.
				CConfig varConfig1(false, "");
				varConfig1.SetNegligibleCreditsThreshold(TEST_DATA[testIndex].m_negligibleCreditsThreshold);
				ASSERT_TRUE(varConfig1.GetNegligibleCreditsThreshold() == TEST_DATA[testIndex].m_negligibleCreditsThreshold);

				// Verify setup NVRAM to non-reset state.
				CConfig varConfig2(false, "");
				varConfig2.BuildYourself();
				ASSERT_TRUE(varConfig2.GetNegligibleCreditsThreshold() == TEST_DATA[testIndex].m_negligibleCreditsThreshold);

				// Set NVRAM invalid.
				CUnitTestingUtils::SetNvramInvalid();

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const WORD negligibleCreditsThresholdResult = config.GetNegligibleCreditsThreshold();
				const WORD& negligibleCreditsThresholdExpected(TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected);

				// Test #1.
				{
					const bool areEqual(negligibleCreditsThresholdResult == negligibleCreditsThresholdExpected);

					string failedMsg;
					failedMsg = FormatString(_T("negligibleCreditsThresholdResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)negligibleCreditsThresholdResult,
						(int)TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CMemoryStorage::UpgradeNVRAM()
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::CheckForNVRAMUpgrade()
		///     CConfig::SetNegligibleCreditsThreshold()
		///     CConfig::GetNegligibleCreditsThreshold()
		/// </summary>
		/// <remarks>
		/// Verify that upgrading NVRAM has no effect on the time zone offset configuration.
		/// </remarks>
		TEST_F(ConfigAUnitTest, SetNegligibleCreditsThreshold_UpgradeNvRamTest)
		{
			const WORD negligibleCreditsThresholdUpgradeValue(0); // Expected value after NVRAM is upgraded.

			typedef struct
			{
				// Input(s).
				const DWORD m_fromNvramVersion; // Upgrade NVRAM from this version to the current version.
				const WORD m_negligibleCreditsThreshold;

				// Expected result(s).
				const WORD m_negligibleCreditsThresholdExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Upgrade.
				{ NVRAM_ORIGINAL_VERSION, 12345, negligibleCreditsThresholdUpgradeValue },
				{ NVRAM_CONFIG_EXT_VERSION - 2, 12345, negligibleCreditsThresholdUpgradeValue },

				{ NVRAM_CONFIG_EXT_VERSION - 1, 0, negligibleCreditsThresholdUpgradeValue },
				{ NVRAM_CONFIG_EXT_VERSION - 1, 12345, negligibleCreditsThresholdUpgradeValue },
				{ NVRAM_CONFIG_EXT_VERSION - 1, WORD_MAX, negligibleCreditsThresholdUpgradeValue },

				// No upgrade.
				{ NVRAM_CONFIG_EXT_VERSION, (WORD)-2, (WORD)-2 },
				{ NVRAM_CONFIG_EXT_VERSION, (WORD)-1, (WORD)-1 },

				{ NVRAM_CONFIG_EXT_VERSION, 0, 0 },
				{ NVRAM_CONFIG_EXT_VERSION, 1, 1 },
				{ NVRAM_CONFIG_EXT_VERSION, 2, 2 },
				{ NVRAM_CONFIG_EXT_VERSION, 12345, 12345 },
				{ NVRAM_CONFIG_EXT_VERSION, 0x1234, 0x1234 },
				{ NVRAM_CONFIG_EXT_VERSION, (WORD)(SHRT_MAX - 1), (WORD)(SHRT_MAX - 1) },
				{ NVRAM_CONFIG_EXT_VERSION, (WORD)SHRT_MAX, (WORD)SHRT_MAX },

				{ NVRAM_CONFIG_EXT_VERSION, (WORD)SHRT_MIN, (WORD)SHRT_MIN },
				{ NVRAM_CONFIG_EXT_VERSION, (WORD)(SHRT_MIN + 1), (WORD)(SHRT_MIN + 1) },
				{ NVRAM_CONFIG_EXT_VERSION, (WORD)-12345, (WORD)-12345 },
				{ NVRAM_CONFIG_EXT_VERSION, (WORD)(WORD_MAX - 1), (WORD)(WORD_MAX - 1) },
				{ NVRAM_CONFIG_EXT_VERSION, WORD_MAX, WORD_MAX },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Setup NVRAM to non-reset state.
				CMemoryStorage::SetSprinkles();
				CMemoryStorage::UpgradeNVRAM();
				CMemoryStorage::UpgradeNVRAM(); // Set memory storage to already upgraded.
				CConfig varConfig1(false, "");
				varConfig1.SetNegligibleCreditsThreshold(TEST_DATA[testIndex].m_negligibleCreditsThreshold);
				ASSERT_TRUE(varConfig1.GetNegligibleCreditsThreshold() == TEST_DATA[testIndex].m_negligibleCreditsThreshold);

				// Verify setup NVRAM to non-reset state.
				CConfig varConfig2(false, "");
				varConfig2.BuildYourself();
				ASSERT_TRUE(varConfig2.GetNegligibleCreditsThreshold() == TEST_DATA[testIndex].m_negligibleCreditsThreshold);

				// Set NVRAM version to be upgraded from.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(TEST_DATA[testIndex].m_fromNvramVersion);

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const WORD negligibleCreditsThresholdResult = config.GetNegligibleCreditsThreshold();

				// Test #1.
				{
					const bool areEqual(negligibleCreditsThresholdResult == TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected);

					string failedMsg;
					failedMsg = FormatString(_T("negligibleCreditsThresholdResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)negligibleCreditsThresholdResult,
						(int)TEST_DATA[testIndex].m_negligibleCreditsThresholdExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}



		/// <summary>
		/// Code coverage and functional test for:
		///     CConfig::SetEmployeeOverPlayerIdleCardOut()
		///     CConfig::GetEmployeeOverPlayerIdleCardOut()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetEmployeeOverPlayerIdleCardOutTest)
		{
			const bool initialEmployeeOverPlayerIdleCardOut(false);

			typedef struct
			{
				// Inputs.
				const bool m_employeeOverPlayerIdleCardOut;

				// Expected results.
				const bool m_oldEmployeeOverPlayerIdleCardOutExpected;
				const bool m_employeeOverPlayerIdleCardOutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false, false },

				{ true, false, true },
				{ true, true, true },
				{ true, true, true },
				{ false, true, false },
				{ false, false, false },
				{ false, false, false },

				{ true, false, true },
				{ true, true, true },
				{ false, true, false },
				{ false, false, false },

				{ true, false, true },
				{ false, true, false },
				{ true, false, true },
				{ false, true, false },
			};

			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetEmployeeOverPlayerIdleCardOut(initialEmployeeOverPlayerIdleCardOut);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				const bool oldEmployeeOverPlayerIdleCardOut = varConfig.SetEmployeeOverPlayerIdleCardOut(TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOut);
				const CConfig &config(varConfig);
				const bool employeeOverPlayerIdleCardOutResult = config.GetEmployeeOverPlayerIdleCardOut();

				// Test #1.
				{
					const bool areEqual(oldEmployeeOverPlayerIdleCardOut == TEST_DATA[testIndex].m_oldEmployeeOverPlayerIdleCardOutExpected);

					string failedMsg;
					failedMsg = FormatString(_T("oldEmployeeOverPlayerIdleCardOut=%d expected=%d testIndex=%d in %s(%d)"),
						(int)oldEmployeeOverPlayerIdleCardOut,
						(int)TEST_DATA[testIndex].m_oldEmployeeOverPlayerIdleCardOutExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.
				{
					const bool areEqual(employeeOverPlayerIdleCardOutResult == TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutExpected);

					string failedMsg;
					failedMsg = FormatString(_T("employeeOverPlayerIdleCardOutResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)employeeOverPlayerIdleCardOutResult,
						(int)TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::SetEmployeeOverPlayerIdleCardOut()
		///     CConfig::GetEmployeeOverPlayerIdleCardOut()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetEmployeeOverPlayerIdleCardOut_NvRamTest)
		{
			const bool initialEmployeeOverPlayerIdleCardOut(false);

			typedef struct
			{
				// Inputs.
				const bool m_employeeOverPlayerIdleCardOut;

				// Expected results.
				const bool m_employeeOverPlayerIdleCardOutExpected;
				const bool m_oldEmployeeOverPlayerIdleCardOutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false, false },

				{ true, false, false },
				{ true, true, true },
				{ true, true, true },
				{ false, true, true },
				{ false, false, false },
				{ false, false, false },

				{ true, false, false },
				{ true, true, true },
				{ false, true, true },
				{ false, false, false },

				{ true, false, false },
				{ false, true, true },
				{ true, false, false },
				{ false, true, true },
			};

			// Setup.
			CMemoryStorage::SetSprinkles();
			CMemoryStorage::UpgradeNVRAM();
			CMemoryStorage::UpgradeNVRAM(); // Set memory storage to already upgraded.
			CConfig varConfig0(false, "");
			varConfig0.SetEmployeeOverPlayerIdleCardOut(initialEmployeeOverPlayerIdleCardOut);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const bool employeeOverPlayerIdleCardOutResult = config.GetEmployeeOverPlayerIdleCardOut();
				const bool oldEmployeeOverPlayerIdleCardOut = varConfig.SetEmployeeOverPlayerIdleCardOut(TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOut);

				// Test #1.
				{
					const bool areEqual(employeeOverPlayerIdleCardOutResult == TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutExpected);

					string failedMsg;
					failedMsg = FormatString(_T("employeeOverPlayerIdleCardOutResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)employeeOverPlayerIdleCardOutResult,
						(int)TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.
				{
					const bool areEqual(oldEmployeeOverPlayerIdleCardOut == TEST_DATA[testIndex].m_oldEmployeeOverPlayerIdleCardOutExpected);

					string failedMsg;
					failedMsg = FormatString(_T("oldEmployeeOverPlayerIdleCardOut=%d expected=%d testIndex=%d in %s(%d)"),
						(int)oldEmployeeOverPlayerIdleCardOut,
						(int)TEST_DATA[testIndex].m_oldEmployeeOverPlayerIdleCardOutExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::EmployeeOverPlayerIdleCardOut()
		///     CConfig::SetEmployeeOverPlayerIdleCardOut()
		///     CNVRAMConfig::Get()
		/// Test includes reading NVRAM contents.
		/// This tests forward and backward compatibility across versions.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetEmployeeOverPlayerIdleCardOut_NvRamGetTest)
		{
			// Test data -- input(s).
			const DWORD NVRAM_FIELD_OFFSET(42687);
			const UINT NVRAM_FIELD_SIZE(1);
			const bool employeeOverPlayerIdleCardOutInitial(true);
			const bool employeeOverPlayerIdleCardOutInitialExpected(true);
			ASSERT_TRUE(NVRAM_FIELD_SIZE == sizeof(employeeOverPlayerIdleCardOutInitial));

			// Test data.
			typedef struct
			{
				// Input(s).
				const bool m_employeeOverPlayerIdleCardOut;

				// Expected result(s).
				const bool m_employeeOverPlayerIdleCardOutExpected;
				const bool m_nvramEmployeeOverPlayerIdleCardOutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, true, true },
				{ true, false, false },
				{ false, true, true },
				{ true, false, false },
			};

			// Setup.
			{
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CConfig varConfig(false, "");
				varConfig.SetEmployeeOverPlayerIdleCardOut(employeeOverPlayerIdleCardOutInitial);

				CNVRAMConfig nvramConfig;
				bool employeeOverPlayerIdleCardOutResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&employeeOverPlayerIdleCardOutResult, NVRAM_FIELD_SIZE);
				ASSERT_TRUE(employeeOverPlayerIdleCardOutResult == employeeOverPlayerIdleCardOutInitialExpected);
			}

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const bool employeeOverPlayerIdleCardOutResult = config.GetEmployeeOverPlayerIdleCardOut();

				CNVRAMConfig nvramConfig;
				bool nvramEmployeeOverPlayerIdleCardOutResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramEmployeeOverPlayerIdleCardOutResult, NVRAM_FIELD_SIZE);

				// Test #1.
				{
					const bool equal(employeeOverPlayerIdleCardOutResult == TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutExpected);
					if (!equal)
					{
						string failedMsg;
						failedMsg = FormatString(_T("employeeOverPlayerIdleCardOutResult=%d expected=%d testIndex=%d in %s(%d)"),
							(int)employeeOverPlayerIdleCardOutResult,
							(int)TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
						ASSERT_TRUE(false) << failedMsg;
					}
				}

				// Test #2.
				{
					const bool equal(nvramEmployeeOverPlayerIdleCardOutResult == TEST_DATA[testIndex].m_nvramEmployeeOverPlayerIdleCardOutExpected);
					if (!equal)
					{
						string failedMsg;
						failedMsg = FormatString(_T("nvramEmployeeOverPlayerIdleCardOutResult=%d expected=%d testIndex=%d in %s(%d)"),
							(int)nvramEmployeeOverPlayerIdleCardOutResult,
							(int)TEST_DATA[testIndex].m_nvramEmployeeOverPlayerIdleCardOutExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
						ASSERT_TRUE(false) << failedMsg;
					}
				}

				// Perform operation(s) to be tested on next loop iteration.
				varConfig.SetEmployeeOverPlayerIdleCardOut(TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOut);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::EmployeeOverPlayerIdleCardOut()
		///     CNVRAMConfig::Set()
		/// Test includes writing NVRAM contents.
		/// This tests forward and backward compatibility across versions.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetEmployeeOverPlayerIdleCardOut_NvRamSetTest)
		{
			// Test data -- input(s).
			const DWORD NVRAM_FIELD_OFFSET(42687);
			const UINT NVRAM_FIELD_SIZE(1);
			const bool employeeOverPlayerIdleCardOutInitial(true);
			const bool employeeOverPlayerIdleCardOutInitialExpected(true);
			ASSERT_TRUE(NVRAM_FIELD_SIZE == sizeof(employeeOverPlayerIdleCardOutInitial));

			// Test data.
			typedef struct
			{
				// Input(s).
				const bool m_employeeOverPlayerIdleCardOut;

				// Expected result(s).
				const bool m_employeeOverPlayerIdleCardOutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, true },
				{ true, false },
				{ false, true },
				{ true, false },
			};

			// Setup.
			{
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CNVRAMConfig nvramConfig;
				bool employeeOverPlayerIdleCardOut(employeeOverPlayerIdleCardOutInitial); // CNVRAMConfig::Set() requires read/write data.
				nvramConfig.Set(NVRAM_FIELD_OFFSET, (BYTE*)&employeeOverPlayerIdleCardOut, NVRAM_FIELD_SIZE);

				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				ASSERT_TRUE(varConfig.GetEmployeeOverPlayerIdleCardOut() == employeeOverPlayerIdleCardOutInitialExpected) << "Setup";
			}

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const bool employeeOverPlayerIdleCardOutResult = config.GetEmployeeOverPlayerIdleCardOut();

				// Test #1.
				{
					const bool equal(employeeOverPlayerIdleCardOutResult == TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutExpected);
					if (!equal)
					{
						string failedMsg;
						failedMsg = FormatString(_T("employeeOverPlayerIdleCardOutResult=%d expected=%d testIndex=%d in %s(%d)"),
							(int)employeeOverPlayerIdleCardOutResult,
							(int)TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
						ASSERT_TRUE(false) << failedMsg;
					}
				}

				// Perform operation(s) to be tested on next loop iteration.
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CNVRAMConfig nvramConfig;
				bool employeeOverPlayerIdleCardOut(TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOut); // CNVRAMConfig::Set() requires read/write data.
				nvramConfig.Set(NVRAM_FIELD_OFFSET, (BYTE*)&employeeOverPlayerIdleCardOut, NVRAM_FIELD_SIZE);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::ResetConfigMemory()
		///     CConfig::BuildYourself()
		///     CConfig::SetEmployeeOverPlayerIdleCardOut()
		///     CConfig::EmployeeOverPlayerIdleCardOut()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetEmployeeOverPlayerIdleCardOut_ClearNvRamTest)
		{
			typedef struct
			{
				// Input(s).
				const bool m_employeeOverPlayerIdleCardOut;

				// Expected result(s).
				const bool m_oldEmployeeOverPlayerIdleCardOutExpected;
				const bool m_employeeOverPlayerIdleCardOutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, true, false },
				{ true, false, false },
				{ false, true, false },
				{ true, false, false },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Setup NVRAM to non-reset state.
				CMemoryStorage::SetSprinkles();
				CMemoryStorage::UpgradeNVRAM();
				CMemoryStorage::UpgradeNVRAM(); // Set memory storage to already upgraded.
				CConfig varConfig1(false, "");
				varConfig1.SetEmployeeOverPlayerIdleCardOut(TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOut);
				ASSERT_TRUE(varConfig1.GetEmployeeOverPlayerIdleCardOut() == TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOut);

				// Verify setup NVRAM to non-reset state.
				CConfig varConfig2(false, "");
				varConfig2.BuildYourself();
				ASSERT_TRUE(varConfig2.GetEmployeeOverPlayerIdleCardOut() == TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOut);

				// Set NVRAM invalid.
				CUnitTestingUtils::SetNvramInvalid();

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const bool employeeOverPlayerIdleCardOutResult = config.GetEmployeeOverPlayerIdleCardOut();

				// Test #1.
				{
					const bool areEqual(employeeOverPlayerIdleCardOutResult == TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutExpected);

					string failedMsg;
					failedMsg = FormatString(_T("employeeOverPlayerIdleCardOutResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)employeeOverPlayerIdleCardOutResult,
						(int)TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CMemoryStorage::UpgradeNVRAM()
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::CheckForNVRAMUpgrade()
		///     CConfig::SetEmployeeOverPlayerIdleCardOut()
		///     CConfig::EmployeeOverPlayerIdleCardOut()
		/// </summary>
		/// <remarks>
		/// Verify that upgrading NVRAM has no effect on the allow floor lockout configuration.
		/// </remarks>
		TEST_F(ConfigAUnitTest, SetEmployeeOverPlayerIdleCardOut_UpgradeNvRamTest)
		{
			const bool employeeOverPlayerIdleCardOutUpgradeValue(false); // Expected value after NVRAM is upgraded.

			typedef struct
			{
				// Input(s).
				const DWORD m_fromNvramVersion; // Upgrade NVRAM from this version to the current version.
				const bool m_employeeOverPlayerIdleCardOut;

				// Expected result(s).
				const bool m_employeeOverPlayerIdleCardOutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Upgrade.
				{ NVRAM_ORIGINAL_VERSION, true, employeeOverPlayerIdleCardOutUpgradeValue },
				{ NVRAM_CONFIG_EXT_VERSION - 2, true, employeeOverPlayerIdleCardOutUpgradeValue },

				{ NVRAM_CONFIG_EXT_VERSION - 1, false, employeeOverPlayerIdleCardOutUpgradeValue },
				{ NVRAM_CONFIG_EXT_VERSION - 1, true, employeeOverPlayerIdleCardOutUpgradeValue },
				{ NVRAM_CONFIG_EXT_VERSION - 1, false, employeeOverPlayerIdleCardOutUpgradeValue },
				{ NVRAM_CONFIG_EXT_VERSION - 1, true, employeeOverPlayerIdleCardOutUpgradeValue },

				// No upgrade.
				{ NVRAM_CONFIG_EXT_VERSION, false, false },
				{ NVRAM_CONFIG_EXT_VERSION, true, true },
				{ NVRAM_CONFIG_EXT_VERSION, false, false },
				{ NVRAM_CONFIG_EXT_VERSION, true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Setup NVRAM to non-reset state.
				CMemoryStorage::SetSprinkles();
				CMemoryStorage::UpgradeNVRAM();
				CMemoryStorage::UpgradeNVRAM(); // Set memory storage to already upgraded.
				CConfig varConfig1(false, "");
				varConfig1.SetEmployeeOverPlayerIdleCardOut(TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOut);
				ASSERT_TRUE(varConfig1.GetEmployeeOverPlayerIdleCardOut() == TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOut);

				// Verify setup NVRAM to non-reset state.
				CConfig varConfig2(false, "");
				varConfig2.BuildYourself();
				ASSERT_TRUE(varConfig2.GetEmployeeOverPlayerIdleCardOut() == TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOut);

				// Set NVRAM version to be upgraded from.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(TEST_DATA[testIndex].m_fromNvramVersion);

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const bool employeeOverPlayerIdleCardOutResult = config.GetEmployeeOverPlayerIdleCardOut();

				// Test #1.
				{
					const bool areEqual(employeeOverPlayerIdleCardOutResult == TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutExpected);

					string failedMsg;
					failedMsg = FormatString(_T("employeeOverPlayerIdleCardOutResult=%d expected=%d in %s(%d)"),
						(int)employeeOverPlayerIdleCardOutResult,
						(int)TEST_DATA[testIndex].m_employeeOverPlayerIdleCardOutExpected,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}



		/// <summary>
		/// Code coverage and functional test for:
		/// CConfig::SetEmployeeIdleCardOutSeconds()
		/// CConfig::GetEmployeeIdleCardOutSeconds()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetEmployeeIdleCardOutSecondsTest)
		{
			const WORD initialEmployeeIdleCardOutSeconds(12345);

			typedef struct
			{
				// Inputs.
				const WORD m_employeeIdleCardOutSeconds;

				// Expected results.
				const WORD m_oldEmployeeIdleCardOutSecondsExpected;
				const WORD m_employeeIdleCardOutSecondsExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (WORD)-1, 12345, (WORD)-1 },

				{ 0, (WORD)-1, 0 },
				{ 1, 0, 1 },
				{ 2, 1, 2 },
				{ 12345, 2, 12345 },
				{ 0x1234, 12345, 0x1234 },
				{ (WORD)(SHRT_MAX - 1), 0x1234, (WORD)(SHRT_MAX - 1) },

				{ SHRT_MAX, (WORD)(SHRT_MAX - 1), SHRT_MAX },
				{ (WORD)SHRT_MIN, SHRT_MAX, (WORD)SHRT_MIN },
				{ (WORD)(SHRT_MIN + 1), (WORD)SHRT_MIN, (WORD)(SHRT_MIN + 1) },
				{ (WORD)-12345, (WORD)(SHRT_MIN + 1), (WORD)-12345 },

				{ (WORD)(WORD_MAX - 1), (WORD)-12345, (WORD)(WORD_MAX - 1) },
				{ WORD_MAX, (WORD)(WORD_MAX - 1), WORD_MAX },
				{ 0, WORD_MAX, 0 },
			};

			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetEmployeeIdleCardOutSeconds(initialEmployeeIdleCardOutSeconds);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				const WORD oldEmployeeIdleCardOutSecondsResult = varConfig.SetEmployeeIdleCardOutSeconds(TEST_DATA[testIndex].m_employeeIdleCardOutSeconds);
				const CConfig &config(varConfig);
				const WORD employeeIdleCardOutSecondsResult = config.GetEmployeeIdleCardOutSeconds();

				// Test #1.
				{
					const bool areEqual(oldEmployeeIdleCardOutSecondsResult == TEST_DATA[testIndex].m_oldEmployeeIdleCardOutSecondsExpected);

					string failedMsg;
					failedMsg = FormatString(_T("oldEmployeeIdleCardOutSecondsResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)oldEmployeeIdleCardOutSecondsResult,
						(int)TEST_DATA[testIndex].m_oldEmployeeIdleCardOutSecondsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.
				{
					const bool areEqual(employeeIdleCardOutSecondsResult == TEST_DATA[testIndex].m_employeeIdleCardOutSecondsExpected);

					string failedMsg;
					failedMsg = FormatString(_T("employeeIdleCardOutSecondsResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)employeeIdleCardOutSecondsResult,
						(int)TEST_DATA[testIndex].m_employeeIdleCardOutSecondsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::SetEmployeeIdleCardOutSeconds()
		///     CConfig::GetEmployeeIdleCardOutSeconds()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetEmployeeIdleCardOutSeconds_NvRamTest)
		{
			const WORD initialEmployeeIdleCardOutSeconds(12345);

			typedef struct
			{
				// Inputs.
				const WORD m_employeeIdleCardOutSeconds;

				// Expected results.
				const WORD m_employeeIdleCardOutSecondsExpected;
				const WORD m_oldEmployeeIdleCardOutSecondsExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (WORD)-1, 12345, 12345 },

				{ 0, (WORD)-1, (WORD)-1 },
				{ 1, 0, 0 },
				{ 2, 1, 1 },
				{ 12345, 2, 2 },
				{ 0x1234, 12345, 12345 },
				{ (WORD)(SHRT_MAX - 1), 0x1234, 0x1234 },

				{ SHRT_MAX, (WORD)(SHRT_MAX - 1), (WORD)(SHRT_MAX - 1) },
				{ (WORD)SHRT_MIN, SHRT_MAX, SHRT_MAX },
				{ (WORD)(SHRT_MIN + 1), (WORD)SHRT_MIN, (WORD)SHRT_MIN },
				{ (WORD)-12345, (WORD)(SHRT_MIN + 1), (WORD)(SHRT_MIN + 1) },

				{ (WORD)(WORD_MAX - 1), (WORD)-12345, (WORD)-12345 },
				{ WORD_MAX, (WORD)(WORD_MAX - 1), (WORD)(WORD_MAX - 1) },
				{ 0, WORD_MAX, WORD_MAX },
			};

			// Setup.
			CMemoryStorage::SetSprinkles();
			CMemoryStorage::UpgradeNVRAM();
			CMemoryStorage::UpgradeNVRAM(); // Set memory storage to already upgraded.
			CConfig varConfig0(false, "");
			varConfig0.SetEmployeeIdleCardOutSeconds(initialEmployeeIdleCardOutSeconds);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const WORD employeeIdleCardOutSecondsResult = config.GetEmployeeIdleCardOutSeconds();
				const WORD oldEmployeeIdleCardOutSecondsResult = varConfig.SetEmployeeIdleCardOutSeconds(TEST_DATA[testIndex].m_employeeIdleCardOutSeconds);

				// Test #1.
				{
					const bool areEqual(employeeIdleCardOutSecondsResult == TEST_DATA[testIndex].m_employeeIdleCardOutSecondsExpected);

					string failedMsg;
					failedMsg = FormatString(_T("employeeIdleCardOutSecondsResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)employeeIdleCardOutSecondsResult,
						(int)TEST_DATA[testIndex].m_employeeIdleCardOutSecondsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.
				{
					const bool areEqual(oldEmployeeIdleCardOutSecondsResult == TEST_DATA[testIndex].m_oldEmployeeIdleCardOutSecondsExpected);

					string failedMsg;
					failedMsg = FormatString(_T("oldEmployeeIdleCardOutSecondsResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)oldEmployeeIdleCardOutSecondsResult,
						(int)TEST_DATA[testIndex].m_oldEmployeeIdleCardOutSecondsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::SetEmployeeIdleCardOutSeconds()
		///     CConfig::GetEmployeeIdleCardOutSeconds()
		///     CNVRAMConfig::Get()
		/// Test includes reading NVRAM contents.
		/// This tests forward and backward compatibility across versions.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetEmployeeIdleCardOutSeconds_NvRamGetTest)
		{
			// Test data -- input(s).
			const DWORD NVRAM_FIELD_OFFSET(42688);
			const UINT NVRAM_FIELD_SIZE(2);
			const WORD employeeIdleCardOutSecondsInitial(12345);
			const WORD employeeIdleCardOutSecondsInitialExpected(12345);
			ASSERT_TRUE(NVRAM_FIELD_SIZE == sizeof(employeeIdleCardOutSecondsInitial));

			// Test data.
			typedef struct
			{
				// Input(s).
				const WORD m_employeeIdleCardOutSeconds;

				// Expected result(s).
				const WORD m_employeeIdleCardOutSecondsExpected;
				const WORD m_nvramEmployeeIdleCardOutSecondsExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (WORD)-1, 12345, 12345 },

				{ 0, (WORD)-1, (WORD)-1 },
				{ 1, 0, 0 },
				{ 2, 1, 1 },
				{ 12345, 2, 2 },
				{ 0x1234, 12345, 12345 },
				{ (WORD)(SHRT_MAX - 1), 0x1234, 0x1234 },

				{ SHRT_MAX, (WORD)(SHRT_MAX - 1), (WORD)(SHRT_MAX - 1) },
				{ (WORD)SHRT_MIN, SHRT_MAX, SHRT_MAX },
				{ (WORD)(SHRT_MIN + 1), (WORD)SHRT_MIN, (WORD)SHRT_MIN },
				{ (WORD)-12345, (WORD)(SHRT_MIN + 1), (WORD)(SHRT_MIN + 1) },

				{ (WORD)(WORD_MAX - 1), (WORD)-12345, (WORD)-12345 },
				{ WORD_MAX, (WORD)(WORD_MAX - 1), (WORD)(WORD_MAX - 1) },
				{ 0, WORD_MAX, WORD_MAX },
			};

			// Setup.
			{
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CConfig varConfig(false, "");
				varConfig.SetEmployeeIdleCardOutSeconds(employeeIdleCardOutSecondsInitial);

				CNVRAMConfig nvramConfig;
				WORD employeeIdleCardOutSecondsResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&employeeIdleCardOutSecondsResult, NVRAM_FIELD_SIZE);
				ASSERT_TRUE(employeeIdleCardOutSecondsResult == employeeIdleCardOutSecondsInitialExpected);
			}

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const WORD employeeIdleCardOutSecondsResult = config.GetEmployeeIdleCardOutSeconds();

				CNVRAMConfig nvramConfig;
				WORD nvramEmployeeIdleCardOutSecondsResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramEmployeeIdleCardOutSecondsResult, NVRAM_FIELD_SIZE);

				// Test #1.
				{
					const bool equal(employeeIdleCardOutSecondsResult == TEST_DATA[testIndex].m_employeeIdleCardOutSecondsExpected);
					if (!equal)
					{
						string failedMsg;
						failedMsg = FormatString(_T("employeeIdleCardOutSecondsResult=%d expected=%d testIndex=%d in %s(%d)"),
							(int)employeeIdleCardOutSecondsResult,
							(int)TEST_DATA[testIndex].m_employeeIdleCardOutSecondsExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
						ASSERT_TRUE(false) << failedMsg;
					}
				}

				// Test #2.
				{
					const bool equal(nvramEmployeeIdleCardOutSecondsResult == TEST_DATA[testIndex].m_nvramEmployeeIdleCardOutSecondsExpected);
					if (!equal)
					{
						string failedMsg;
						failedMsg = FormatString(_T("nvramEmployeeIdleCardOutSecondsResult=%d expected=%d testIndex=%d in %s(%d)"),
							(int)nvramEmployeeIdleCardOutSecondsResult,
							(int)TEST_DATA[testIndex].m_nvramEmployeeIdleCardOutSecondsExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
						ASSERT_TRUE(false) << failedMsg;
					}
				}

				// Perform operation(s) to be tested on next loop iteration.
				varConfig.SetEmployeeIdleCardOutSeconds(TEST_DATA[testIndex].m_employeeIdleCardOutSeconds);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::GetEmployeeIdleCardOutSeconds()
		///     CNVRAMConfig::Set()
		/// Test includes writing NVRAM contents.
		/// This tests forward and backward compatibility across versions.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetEmployeeIdleCardOutSeconds_NvRamSetTest)
		{
			// Test data -- input(s).
			const DWORD NVRAM_FIELD_OFFSET(42688);
			const UINT NVRAM_FIELD_SIZE(2);
			const WORD employeeIdleCardOutSecondsInitial(12345);
			const WORD employeeIdleCardOutSecondsInitialExpected(12345);
			ASSERT_TRUE(NVRAM_FIELD_SIZE == sizeof(employeeIdleCardOutSecondsInitial));

			// Test data.
			typedef struct
			{
				// Input(s).
				const WORD m_employeeIdleCardOutSeconds;

				// Expected result(s).
				const WORD m_employeeIdleCardOutSecondsExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (WORD)-1, 12345 },

				{ 0, (WORD)-1 },
				{ 1, 0 },
				{ 2, 1 },
				{ 12345, 2 },
				{ 0x1234, 12345 },
				{ (WORD)(SHRT_MAX - 1), 0x1234 },

				{ SHRT_MAX, (WORD)(SHRT_MAX - 1) },
				{ (WORD)SHRT_MIN, SHRT_MAX },
				{ (WORD)(SHRT_MIN + 1), (WORD)SHRT_MIN },
				{ (WORD)-12345, (WORD)(SHRT_MIN + 1) },

				{ (WORD)(WORD_MAX - 1), (WORD)-12345 },
				{ (WORD)WORD_MAX, (WORD)(WORD_MAX - 1) },
				{ 0, WORD_MAX },
			};

			// Setup.
			{
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CNVRAMConfig nvramConfig;
				WORD employeeIdleCardOutSeconds(employeeIdleCardOutSecondsInitial); // CNVRAMConfig::Set() requires read/write data.
				nvramConfig.Set(NVRAM_FIELD_OFFSET, (BYTE*)&employeeIdleCardOutSeconds, NVRAM_FIELD_SIZE);

				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				ASSERT_TRUE(varConfig.GetEmployeeIdleCardOutSeconds() == employeeIdleCardOutSecondsInitialExpected) << "Setup";
			}

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const WORD employeeIdleCardOutSecondsResult = config.GetEmployeeIdleCardOutSeconds();

				// Test #1.
				{
					const bool equal(employeeIdleCardOutSecondsResult == TEST_DATA[testIndex].m_employeeIdleCardOutSecondsExpected);
					if (!equal)
					{
						string failedMsg;
						failedMsg = FormatString(_T("employeeIdleCardOutSecondsResult=%d expected=%d testIndex=%d in %s(%d)"),
							(int)employeeIdleCardOutSecondsResult,
							(int)TEST_DATA[testIndex].m_employeeIdleCardOutSecondsExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
						ASSERT_TRUE(false) << failedMsg;
					}
				}

				// Perform operation(s) to be tested on next loop iteration.
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CNVRAMConfig nvramConfig;
				WORD employeeIdleCardOutSeconds(TEST_DATA[testIndex].m_employeeIdleCardOutSeconds); // CNVRAMConfig::Set() requires read/write data.
				nvramConfig.Set(NVRAM_FIELD_OFFSET, (BYTE*)&employeeIdleCardOutSeconds, NVRAM_FIELD_SIZE);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::ResetConfigMemory()
		///     CConfig::BuildYourself()
		///     CConfig::SetEmployeeIdleCardOutSeconds()
		///     CConfig::GetEmployeeIdleCardOutSeconds()
		/// Note that the configuration value is uninitialized after NVRAM is cleared or lost.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetEmployeeIdleCardOutSeconds_ClearNvRamTest)
		{
			const WORD employeeIdleCardOutSecondsResetValue(120); // Expected value after NVRAM is cleared or lost.

			typedef struct
			{
				// Input(s).
				const WORD m_employeeIdleCardOutSeconds;

				// Expected result(s).
				const WORD m_employeeIdleCardOutSecondsExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (WORD)-1, employeeIdleCardOutSecondsResetValue },

				{ 0, employeeIdleCardOutSecondsResetValue },
				{ 1, employeeIdleCardOutSecondsResetValue },
				{ 2, employeeIdleCardOutSecondsResetValue },
				{ 12345, employeeIdleCardOutSecondsResetValue },
				{ 0x1234, employeeIdleCardOutSecondsResetValue },
				{ (WORD)(SHRT_MAX - 1), employeeIdleCardOutSecondsResetValue },

				{ (WORD)SHRT_MAX, employeeIdleCardOutSecondsResetValue },
				{ (WORD)SHRT_MIN, employeeIdleCardOutSecondsResetValue },
				{ (WORD)(SHRT_MIN + 1), employeeIdleCardOutSecondsResetValue },
				{ (WORD)-12345, employeeIdleCardOutSecondsResetValue },

				{ (WORD)(WORD_MAX - 1), employeeIdleCardOutSecondsResetValue },
				{ (WORD)WORD_MAX, employeeIdleCardOutSecondsResetValue },
				{ (WORD)0, employeeIdleCardOutSecondsResetValue },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Setup NVRAM to non-reset state.
				CMemoryStorage::SetSprinkles();
				CMemoryStorage::UpgradeNVRAM();
				CMemoryStorage::UpgradeNVRAM(); // Set memory storage to already upgraded.
				CConfig varConfig1(false, "");
				varConfig1.SetEmployeeIdleCardOutSeconds(TEST_DATA[testIndex].m_employeeIdleCardOutSeconds);
				ASSERT_TRUE(varConfig1.GetEmployeeIdleCardOutSeconds() == TEST_DATA[testIndex].m_employeeIdleCardOutSeconds);

				// Verify setup NVRAM to non-reset state.
				CConfig varConfig2(false, "");
				varConfig2.BuildYourself();
				ASSERT_TRUE(varConfig2.GetEmployeeIdleCardOutSeconds() == TEST_DATA[testIndex].m_employeeIdleCardOutSeconds);

				// Set NVRAM invalid.
				CUnitTestingUtils::SetNvramInvalid();

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const WORD employeeIdleCardOutSecondsResult = config.GetEmployeeIdleCardOutSeconds();
				const WORD& employeeIdleCardOutSecondsExpected(TEST_DATA[testIndex].m_employeeIdleCardOutSecondsExpected);

				// Test #1.
				{
					const bool areEqual(employeeIdleCardOutSecondsResult == employeeIdleCardOutSecondsExpected);

					string failedMsg;
					failedMsg = FormatString(_T("employeeIdleCardOutSecondsResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)employeeIdleCardOutSecondsResult,
						(int)TEST_DATA[testIndex].m_employeeIdleCardOutSecondsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CMemoryStorage::UpgradeNVRAM()
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::CheckForNVRAMUpgrade()
		///     CConfig::SetEmployeeIdleCardOutSeconds()
		///     CConfig::GetEmployeeIdleCardOutSeconds()
		/// </summary>
		/// <remarks>
		/// Verify that upgrading NVRAM has no effect on the time zone offset configuration.
		/// </remarks>
		TEST_F(ConfigAUnitTest, SetEmployeeIdleCardOutSeconds_UpgradeNvRamTest)
		{
			const WORD employeeIdleCardOutSecondsUpgradeValue(120); // Expected value after NVRAM is upgraded.

			typedef struct
			{
				// Input(s).
				const DWORD m_fromNvramVersion; // Upgrade NVRAM from this version to the current version.
				const WORD m_employeeIdleCardOutSeconds;

				// Expected result(s).
				const WORD m_employeeIdleCardOutSecondsExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Upgrade.
				{ NVRAM_ORIGINAL_VERSION, 12345, employeeIdleCardOutSecondsUpgradeValue },
				{ NVRAM_CONFIG_EXT_VERSION - 2, 12345, employeeIdleCardOutSecondsUpgradeValue },

				{ NVRAM_CONFIG_EXT_VERSION - 1, 0, employeeIdleCardOutSecondsUpgradeValue },
				{ NVRAM_CONFIG_EXT_VERSION - 1, 12345, employeeIdleCardOutSecondsUpgradeValue },
				{ NVRAM_CONFIG_EXT_VERSION - 1, WORD_MAX, employeeIdleCardOutSecondsUpgradeValue },

				// No upgrade.
				{ NVRAM_CONFIG_EXT_VERSION, (WORD)-2, (WORD)-2 },
				{ NVRAM_CONFIG_EXT_VERSION, (WORD)-1, (WORD)-1 },

				{ NVRAM_CONFIG_EXT_VERSION, 0, 0 },
				{ NVRAM_CONFIG_EXT_VERSION, 1, 1 },
				{ NVRAM_CONFIG_EXT_VERSION, 2, 2 },
				{ NVRAM_CONFIG_EXT_VERSION, 12345, 12345 },
				{ NVRAM_CONFIG_EXT_VERSION, 0x1234, 0x1234 },
				{ NVRAM_CONFIG_EXT_VERSION, (WORD)(SHRT_MAX - 1), (WORD)(SHRT_MAX - 1) },
				{ NVRAM_CONFIG_EXT_VERSION, SHRT_MAX, SHRT_MAX },

				{ NVRAM_CONFIG_EXT_VERSION, (WORD)SHRT_MIN, (WORD)SHRT_MIN },
				{ NVRAM_CONFIG_EXT_VERSION, (WORD)(SHRT_MIN + 1), (WORD)(SHRT_MIN + 1) },
				{ NVRAM_CONFIG_EXT_VERSION, (WORD)-12345, (WORD)-12345 },
				{ NVRAM_CONFIG_EXT_VERSION, (WORD)(WORD_MAX - 1), (WORD)(WORD_MAX - 1) },
				{ NVRAM_CONFIG_EXT_VERSION, WORD_MAX, WORD_MAX },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Setup NVRAM to non-reset state.
				CMemoryStorage::SetSprinkles();
				CMemoryStorage::UpgradeNVRAM();
				CMemoryStorage::UpgradeNVRAM(); // Set memory storage to already upgraded.
				CConfig varConfig1(false, "");
				varConfig1.SetEmployeeIdleCardOutSeconds(TEST_DATA[testIndex].m_employeeIdleCardOutSeconds);
				ASSERT_TRUE(varConfig1.GetEmployeeIdleCardOutSeconds() == TEST_DATA[testIndex].m_employeeIdleCardOutSeconds);

				// Verify setup NVRAM to non-reset state.
				CConfig varConfig2(false, "");
				varConfig2.BuildYourself();
				ASSERT_TRUE(varConfig2.GetEmployeeIdleCardOutSeconds() == TEST_DATA[testIndex].m_employeeIdleCardOutSeconds);

				// Set NVRAM version to be upgraded from.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(TEST_DATA[testIndex].m_fromNvramVersion);

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const WORD employeeIdleCardOutSecondsResult = config.GetEmployeeIdleCardOutSeconds();

				// Test #1.
				{
					const bool areEqual(employeeIdleCardOutSecondsResult == TEST_DATA[testIndex].m_employeeIdleCardOutSecondsExpected);

					string failedMsg;
					failedMsg = FormatString(_T("employeeIdleCardOutSecondsResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)employeeIdleCardOutSecondsResult,
						(int)TEST_DATA[testIndex].m_employeeIdleCardOutSecondsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}



		/// <summary>
		/// Code coverage and functional test for:
		///     CConfig::SetPlayerRecardEmployeeCardOut()
		///     CConfig::GetPlayerRecardEmployeeCardOut()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetPlayerRecardEmployeeCardOutTest)
		{
			const bool initialPlayerRecardEmployeeCardOut(false);

			typedef struct
			{
				// Inputs.
				const bool m_playerRecardEmployeeCardOut;

				// Expected results.
				const bool m_oldPlayerRecardEmployeeCardOutExpected;
				const bool m_playerRecardEmployeeCardOutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false, false },

				{ true, false, true },
				{ true, true, true },
				{ true, true, true },
				{ false, true, false },
				{ false, false, false },
				{ false, false, false },

				{ true, false, true },
				{ true, true, true },
				{ false, true, false },
				{ false, false, false },

				{ true, false, true },
				{ false, true, false },
				{ true, false, true },
				{ false, true, false },
			};

			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetPlayerRecardEmployeeCardOut(initialPlayerRecardEmployeeCardOut);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				const bool oldPlayerRecardEmployeeCardOut = varConfig.SetPlayerRecardEmployeeCardOut(TEST_DATA[testIndex].m_playerRecardEmployeeCardOut);
				const CConfig &config(varConfig);
				const bool playerRecardEmployeeCardOutResult = config.GetPlayerRecardEmployeeCardOut();

				// Test #1.
				{
					const bool areEqual(oldPlayerRecardEmployeeCardOut == TEST_DATA[testIndex].m_oldPlayerRecardEmployeeCardOutExpected);

					string failedMsg;
					failedMsg = FormatString(_T("oldPlayerRecardEmployeeCardOut=%d expected=%d testIndex=%d in %s(%d)"),
						(int)oldPlayerRecardEmployeeCardOut,
						(int)TEST_DATA[testIndex].m_oldPlayerRecardEmployeeCardOutExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.
				{
					const bool areEqual(playerRecardEmployeeCardOutResult == TEST_DATA[testIndex].m_playerRecardEmployeeCardOutExpected);

					string failedMsg;
					failedMsg = FormatString(_T("playerRecardEmployeeCardOutResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)playerRecardEmployeeCardOutResult,
						(int)TEST_DATA[testIndex].m_playerRecardEmployeeCardOutExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::SetPlayerRecardEmployeeCardOut()
		///     CConfig::GetPlayerRecardEmployeeCardOut()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetPlayerRecardEmployeeCardOut_NvRamTest)
		{
			const bool initialPlayerRecardEmployeeCardOut(false);

			typedef struct
			{
				// Inputs.
				const bool m_playerRecardEmployeeCardOut;

				// Expected results.
				const bool m_playerRecardEmployeeCardOutExpected;
				const bool m_oldPlayerRecardEmployeeCardOutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false, false },

				{ true, false, false },
				{ true, true, true },
				{ true, true, true },
				{ false, true, true },
				{ false, false, false },
				{ false, false, false },

				{ true, false, false },
				{ true, true, true },
				{ false, true, true },
				{ false, false, false },

				{ true, false, false },
				{ false, true, true },
				{ true, false, false },
				{ false, true, true },
			};

			// Setup.
			CMemoryStorage::SetSprinkles();
			CMemoryStorage::UpgradeNVRAM();
			CMemoryStorage::UpgradeNVRAM(); // Set memory storage to already upgraded.
			CConfig varConfig0(false, "");
			varConfig0.SetPlayerRecardEmployeeCardOut(initialPlayerRecardEmployeeCardOut);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const bool playerRecardEmployeeCardOutResult = config.GetPlayerRecardEmployeeCardOut();
				const bool oldPlayerRecardEmployeeCardOut = varConfig.SetPlayerRecardEmployeeCardOut(TEST_DATA[testIndex].m_playerRecardEmployeeCardOut);

				// Test #1.
				{
					const bool areEqual(playerRecardEmployeeCardOutResult == TEST_DATA[testIndex].m_playerRecardEmployeeCardOutExpected);

					string failedMsg;
					failedMsg = FormatString(_T("playerRecardEmployeeCardOutResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)playerRecardEmployeeCardOutResult,
						(int)TEST_DATA[testIndex].m_playerRecardEmployeeCardOutExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.
				{
					const bool areEqual(oldPlayerRecardEmployeeCardOut == TEST_DATA[testIndex].m_oldPlayerRecardEmployeeCardOutExpected);

					string failedMsg;
					failedMsg = FormatString(_T("oldPlayerRecardEmployeeCardOut=%d expected=%d testIndex=%d in %s(%d)"),
						(int)oldPlayerRecardEmployeeCardOut,
						(int)TEST_DATA[testIndex].m_oldPlayerRecardEmployeeCardOutExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::PlayerRecardEmployeeCardOut()
		///     CConfig::SetPlayerRecardEmployeeCardOut()
		///     CNVRAMConfig::Get()
		/// Test includes reading NVRAM contents.
		/// This tests forward and backward compatibility across versions.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetPlayerRecardEmployeeCardOut_NvRamGetTest)
		{
			// Test data -- input(s).
			const DWORD NVRAM_FIELD_OFFSET(42690);
			const UINT NVRAM_FIELD_SIZE(1);
			const bool playerRecardEmployeeCardOutInitial(true);
			const bool playerRecardEmployeeCardOutInitialExpected(true);
			ASSERT_TRUE(NVRAM_FIELD_SIZE == sizeof(playerRecardEmployeeCardOutInitial));

			// Test data.
			typedef struct
			{
				// Input(s).
				const bool m_playerRecardEmployeeCardOut;

				// Expected result(s).
				const bool m_playerRecardEmployeeCardOutExpected;
				const bool m_nvramPlayerRecardEmployeeCardOutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, true, true },
				{ true, false, false },
				{ false, true, true },
				{ true, false, false },
			};

			// Setup.
			{
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CConfig varConfig(false, "");
				varConfig.SetPlayerRecardEmployeeCardOut(playerRecardEmployeeCardOutInitial);

				CNVRAMConfig nvramConfig;
				bool playerRecardEmployeeCardOutResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&playerRecardEmployeeCardOutResult, NVRAM_FIELD_SIZE);
				ASSERT_TRUE(playerRecardEmployeeCardOutResult == playerRecardEmployeeCardOutInitialExpected);
			}

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const bool playerRecardEmployeeCardOutResult = config.GetPlayerRecardEmployeeCardOut();

				CNVRAMConfig nvramConfig;
				bool nvramPlayerRecardEmployeeCardOutResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramPlayerRecardEmployeeCardOutResult, NVRAM_FIELD_SIZE);

				// Test #1.
				{
					const bool equal(playerRecardEmployeeCardOutResult == TEST_DATA[testIndex].m_playerRecardEmployeeCardOutExpected);
					if (!equal)
					{
						string failedMsg;
						failedMsg = FormatString(_T("playerRecardEmployeeCardOutResult=%d expected=%d testIndex=%d in %s(%d)"),
							(int)playerRecardEmployeeCardOutResult,
							(int)TEST_DATA[testIndex].m_playerRecardEmployeeCardOutExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
						ASSERT_TRUE(false) << failedMsg;
					}
				}

				// Test #2.
				{
					const bool equal(nvramPlayerRecardEmployeeCardOutResult == TEST_DATA[testIndex].m_nvramPlayerRecardEmployeeCardOutExpected);
					if (!equal)
					{
						string failedMsg;
						failedMsg = FormatString(_T("nvramPlayerRecardEmployeeCardOutResult=%d expected=%d testIndex=%d in %s(%d)"),
							(int)nvramPlayerRecardEmployeeCardOutResult,
							(int)TEST_DATA[testIndex].m_nvramPlayerRecardEmployeeCardOutExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
						ASSERT_TRUE(false) << failedMsg;
					}
				}

				// Perform operation(s) to be tested on next loop iteration.
				varConfig.SetPlayerRecardEmployeeCardOut(TEST_DATA[testIndex].m_playerRecardEmployeeCardOut);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::PlayerRecardEmployeeCardOut()
		///     CNVRAMConfig::Set()
		/// Test includes writing NVRAM contents.
		/// This tests forward and backward compatibility across versions.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetPlayerRecardEmployeeCardOut_NvRamSetTest)
		{
			// Test data -- input(s).
			const DWORD NVRAM_FIELD_OFFSET(42690);
			const UINT NVRAM_FIELD_SIZE(1);
			const bool playerRecardEmployeeCardOutInitial(true);
			const bool playerRecardEmployeeCardOutInitialExpected(true);
			ASSERT_TRUE(NVRAM_FIELD_SIZE == sizeof(playerRecardEmployeeCardOutInitial));

			// Test data.
			typedef struct
			{
				// Input(s).
				const bool m_playerRecardEmployeeCardOut;

				// Expected result(s).
				const bool m_playerRecardEmployeeCardOutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, true },
				{ true, false },
				{ false, true },
				{ true, false },
			};

			// Setup.
			{
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CNVRAMConfig nvramConfig;
				bool playerRecardEmployeeCardOut(playerRecardEmployeeCardOutInitial); // CNVRAMConfig::Set() requires read/write data.
				nvramConfig.Set(NVRAM_FIELD_OFFSET, (BYTE*)&playerRecardEmployeeCardOut, NVRAM_FIELD_SIZE);

				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				ASSERT_TRUE(varConfig.GetPlayerRecardEmployeeCardOut() == playerRecardEmployeeCardOutInitialExpected) << "Setup";
			}

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const bool playerRecardEmployeeCardOutResult = config.GetPlayerRecardEmployeeCardOut();

				// Test #1.
				{
					const bool equal(playerRecardEmployeeCardOutResult == TEST_DATA[testIndex].m_playerRecardEmployeeCardOutExpected);
					if (!equal)
					{
						string failedMsg;
						failedMsg = FormatString(_T("playerRecardEmployeeCardOutResult=%d expected=%d testIndex=%d in %s(%d)"),
							(int)playerRecardEmployeeCardOutResult,
							(int)TEST_DATA[testIndex].m_playerRecardEmployeeCardOutExpected,
							(int)testIndex,
							static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
						ASSERT_TRUE(false) << failedMsg;
					}
				}

				// Perform operation(s) to be tested on next loop iteration.
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CNVRAMConfig nvramConfig;
				bool playerRecardEmployeeCardOut(TEST_DATA[testIndex].m_playerRecardEmployeeCardOut); // CNVRAMConfig::Set() requires read/write data.
				nvramConfig.Set(NVRAM_FIELD_OFFSET, (BYTE*)&playerRecardEmployeeCardOut, NVRAM_FIELD_SIZE);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::ResetConfigMemory()
		///     CConfig::BuildYourself()
		///     CConfig::SetPlayerRecardEmployeeCardOut()
		///     CConfig::PlayerRecardEmployeeCardOut()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetPlayerRecardEmployeeCardOut_ClearNvRamTest)
		{
			typedef struct
			{
				// Input(s).
				const bool m_playerRecardEmployeeCardOut;

				// Expected result(s).
				const bool m_oldPlayerRecardEmployeeCardOutExpected;
				const bool m_playerRecardEmployeeCardOutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, true, false },
				{ true, false, false },
				{ false, true, false },
				{ true, false, false },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Setup NVRAM to non-reset state.
				CMemoryStorage::SetSprinkles();
				CMemoryStorage::UpgradeNVRAM();
				CMemoryStorage::UpgradeNVRAM(); // Set memory storage to already upgraded.
				CConfig varConfig1(false, "");
				varConfig1.SetPlayerRecardEmployeeCardOut(TEST_DATA[testIndex].m_playerRecardEmployeeCardOut);
				ASSERT_TRUE(varConfig1.GetPlayerRecardEmployeeCardOut() == TEST_DATA[testIndex].m_playerRecardEmployeeCardOut);

				// Verify setup NVRAM to non-reset state.
				CConfig varConfig2(false, "");
				varConfig2.BuildYourself();
				ASSERT_TRUE(varConfig2.GetPlayerRecardEmployeeCardOut() == TEST_DATA[testIndex].m_playerRecardEmployeeCardOut);

				// Set NVRAM invalid.
				CUnitTestingUtils::SetNvramInvalid();

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const bool playerRecardEmployeeCardOutResult = config.GetPlayerRecardEmployeeCardOut();

				// Test #1.
				{
					const bool areEqual(playerRecardEmployeeCardOutResult == TEST_DATA[testIndex].m_playerRecardEmployeeCardOutExpected);

					string failedMsg;
					failedMsg = FormatString(_T("playerRecardEmployeeCardOutResult=%d expected=%d testIndex=%d in %s(%d)"),
						(int)playerRecardEmployeeCardOutResult,
						(int)TEST_DATA[testIndex].m_playerRecardEmployeeCardOutExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CMemoryStorage::UpgradeNVRAM()
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::CheckForNVRAMUpgrade()
		///     CConfig::SetPlayerRecardEmployeeCardOut()
		///     CConfig::PlayerRecardEmployeeCardOut()
		/// </summary>
		/// <remarks>
		/// Verify that upgrading NVRAM has no effect on the allow floor lockout configuration.
		/// </remarks>
		TEST_F(ConfigAUnitTest, SetPlayerRecardEmployeeCardOut_UpgradeNvRamTest)
		{
			const bool playerRecardEmployeeCardOutUpgradeValue(false); // Expected value after NVRAM is upgraded.

			typedef struct
			{
				// Input(s).
				const DWORD m_fromNvramVersion; // Upgrade NVRAM from this version to the current version.
				const bool m_playerRecardEmployeeCardOut;

				// Expected result(s).
				const bool m_playerRecardEmployeeCardOutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Upgrade.
				{ NVRAM_ORIGINAL_VERSION, true, playerRecardEmployeeCardOutUpgradeValue },
				{ NVRAM_CONFIG_EXT_VERSION - 2, true, playerRecardEmployeeCardOutUpgradeValue },

				{ NVRAM_CONFIG_EXT_VERSION - 1, false, playerRecardEmployeeCardOutUpgradeValue },
				{ NVRAM_CONFIG_EXT_VERSION - 1, true, playerRecardEmployeeCardOutUpgradeValue },
				{ NVRAM_CONFIG_EXT_VERSION - 1, false, playerRecardEmployeeCardOutUpgradeValue },
				{ NVRAM_CONFIG_EXT_VERSION - 1, true, playerRecardEmployeeCardOutUpgradeValue },

				// No upgrade.
				{ NVRAM_CONFIG_EXT_VERSION, false, false },
				{ NVRAM_CONFIG_EXT_VERSION, true, true },
				{ NVRAM_CONFIG_EXT_VERSION, false, false },
				{ NVRAM_CONFIG_EXT_VERSION, true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Setup NVRAM to non-reset state.
				CMemoryStorage::SetSprinkles();
				CMemoryStorage::UpgradeNVRAM();
				CMemoryStorage::UpgradeNVRAM(); // Set memory storage to already upgraded.
				CConfig varConfig1(false, "");
				varConfig1.SetPlayerRecardEmployeeCardOut(TEST_DATA[testIndex].m_playerRecardEmployeeCardOut);
				ASSERT_TRUE(varConfig1.GetPlayerRecardEmployeeCardOut() == TEST_DATA[testIndex].m_playerRecardEmployeeCardOut);

				// Verify setup NVRAM to non-reset state.
				CConfig varConfig2(false, "");
				varConfig2.BuildYourself();
				ASSERT_TRUE(varConfig2.GetPlayerRecardEmployeeCardOut() == TEST_DATA[testIndex].m_playerRecardEmployeeCardOut);

				// Set NVRAM version to be upgraded from.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(TEST_DATA[testIndex].m_fromNvramVersion);

				// Perform operation(s) to be tested.
				CConfig varConfig(false, "");
				varConfig.BuildYourself();
				const CConfig &config(varConfig);
				const bool playerRecardEmployeeCardOutResult = config.GetPlayerRecardEmployeeCardOut();

				// Test #1.
				{
					const bool areEqual(playerRecardEmployeeCardOutResult == TEST_DATA[testIndex].m_playerRecardEmployeeCardOutExpected);

					string failedMsg;
					failedMsg = FormatString(_T("playerRecardEmployeeCardOutResult=%d expected=%d in %s(%d)"),
						(int)playerRecardEmployeeCardOutResult,
						(int)TEST_DATA[testIndex].m_playerRecardEmployeeCardOutExpected,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}
// #pragma region GetUJPEnabled
		/// <summary>
		/// Functional test for:
		///     CConfig::SetUJPEnabled()
		///     CConfig::GetUJPEnabled()
		/// Test setting a range of values.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetUJPEnabledTest)
		{
			// Data.
			vector<bool> testData{true, true, false, false, true};
			
			// Setup.
			CConfig config(false, "");
			bool previousResult(true); // Prefer non-default.
			config.SetUJPEnabled(previousResult);
			ASSERT_EQ(previousResult, config.GetUJPEnabled()) << "GetUJPEnabled setup";

			for (auto testValue : testData)
			{
				// Get the next test value.
				bool expectedResult(testValue);

				// Perform operation(s) and test(s).
				ASSERT_EQ(previousResult, config.SetUJPEnabled(testValue)) << "SetUJPEnabled";
				ASSERT_EQ(expectedResult, config.GetUJPEnabled()) << "GetUJPEnabled get";

				// Setup for next test.
				previousResult = expectedResult;
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::SetUJPEnabled()
		///     CConfig::GetUJPEnabled()
		/// Test setting a range of values.
		/// This normally tests saving and reading NVRAM values across restarts.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetUJPEnabled_NvRamTest)
		{
			// Data.
			vector<bool> testData{true, true, false, false, true};
			const bool defaultValue(false);
			bool previousResult(!defaultValue); // Prefer non-default.

			// Setup.
			{
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CConfig config(false, "");
				config.SetUJPEnabled(previousResult);
				ASSERT_EQ(previousResult, config.GetUJPEnabled()) << "GetUJPEnabled setup";
			}

			for (auto testValue : testData)
			{
				// Get the next test value.
				const bool expectedResult(testValue);

				// Perform operation(s) and test(s).
				CConfig config(false, "");
				config.BuildYourself();
				ASSERT_EQ(previousResult, config.GetUJPEnabled()) <<  "GetUJPEnabled previous";
				ASSERT_EQ(previousResult, config.SetUJPEnabled(testValue)) << "SetUJPEnabled";
				ASSERT_EQ(expectedResult, config.GetUJPEnabled()) << "GetUJPEnabled expected";

				// Setup for next test.
				previousResult = expectedResult;
			}
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::GetUJPEnabled()
		///     CConfig::SetUJPEnabled()
		///     CNVRAMConfig::Get()
		/// Test includes reading NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetUJPEnabled_NvRamGetTest)
		{
			// Data.
			vector<bool> testData{true, true, false, false, true};

			const DWORD NVRAM_FIELD_OFFSET(710796);
			const UINT NVRAM_FIELD_SIZE(1);

			ASSERT_EQ(sizeof(bool), NVRAM_FIELD_SIZE) << "NVRAM_FIELD_SIZE";
			const bool defaultValue(false);
			bool previousResult(!defaultValue); // Prefer non-default.

			// Setup.
			{
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CConfig config(false, "");
				config.SetUJPEnabled(previousResult);
				ASSERT_EQ(previousResult, config.GetUJPEnabled()) << "GetUJPEnabled setup";
			}

			for (auto testValue : testData)
			{
				// Get the next test value.
				const bool expectedResult(testValue);

				// Perform operation(s) and test(s).
				CConfig config(false, "");
				config.BuildYourself();
				ASSERT_EQ(previousResult, config.GetUJPEnabled()) << "GetUJPEnabled previous";

				// Perform operation(s) to be tested on next loop iteration.
				config.SetUJPEnabled(testValue);
				ASSERT_EQ(expectedResult, config.GetUJPEnabled()) << "GetUJPEnabled expected";
				previousResult = expectedResult;
			}
		}
#if 000//TODO-PORT

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::GetUJPEnabled()
		///     CNVRAMConfig::Set()
		/// Test includes writing NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetUJPEnabled_NvRamSetTest)
		{
			// Data.
			const DWORD NVRAM_FIELD_OFFSET(710796);
			const UINT NVRAM_FIELD_SIZE(1);

			typedef bool TestDataType;
			Assert::AreEqual(sizeof(TestDataType), NVRAM_FIELD_SIZE, L"NVRAM_FIELD_SIZE");
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
				Assert::AreEqual(previousResult, nvramResult, L"nvramResult setup");

				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(previousResult, config.GetUJPEnabled(), L"GetUJPEnabled setup");
			}

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(previousResult, config.GetUJPEnabled(), L"GetUJPEnabled previous");

				// Perform operation(s) to be tested on next loop iteration.
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CNVRAMConfig nvramConfig;
				bool nvramValue(testValue); // CNVRAMConfig::Set() requires read/write data.
				nvramConfig.Set(NVRAM_FIELD_OFFSET, (BYTE*)&nvramValue, NVRAM_FIELD_SIZE);
				bool nvramResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramResult, NVRAM_FIELD_SIZE);
				Assert::AreEqual(expectedResult, nvramResult, L"nvramResult expected");

				// Perform operation(s) to be tested on next loop iteration.
				previousResult = expectedResult;
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::ResetConfigMemory()
		///     CConfig::BuildYourself()
		///     CConfig::SetUJPEnabled()
		///     CConfig::GetUJPEnabled()
		/// Test setting a range of values.
		/// This tests clearing NVRAM upon restart.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetUJPEnabled_ClearNvRamTest)
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
				CConfig config1;
				config1.SetUJPEnabled(testValue);
				Assert::AreEqual(expectedResult, config1.GetUJPEnabled(), L"GetUJPEnabled setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.GetUJPEnabled(), L"GetUJPEnabled setup2");

				// Set NVRAM invalid.
				CUnitTestingUtils::SetNvramInvalid();

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.GetUJPEnabled(), L"GetUJPEnabled default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetUJPEnabled()
		///     CConfig::GetUJPEnabled()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetUJPEnabled_UpgradeNvRamTest_CheckByteValid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710797);
			const BYTE VALID_CHECK_BYTE(0xc9);
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
				CConfig config1;
				config1.SetUJPEnabled(testValue);
				Assert::AreEqual(expectedResult, config1.GetUJPEnabled(), L"GetUJPEnabled setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.GetUJPEnabled(), L"GetUJPEnabled setup2");

				// Set NVRAM check byte valid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, VALID_CHECK_BYTE);

				// Set NVRAM to the maximum version to need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_CONFIG_EXT3_VERSION - 1);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.GetUJPEnabled(), L"GetUJPEnabled default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetUJPEnabled()
		///     CConfig::GetUJPEnabled()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetUJPEnabled_UpgradeNvRamTest_CheckByteInvalid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710797);
			const BYTE INVALID_CHECK_BYTE(0x00);
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
				CConfig config1;
				config1.SetUJPEnabled(testValue);
				Assert::AreEqual(expectedResult, config1.GetUJPEnabled(), L"GetUJPEnabled setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.GetUJPEnabled(), L"GetUJPEnabled setup2");

				// Set NVRAM check byte invalid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, INVALID_CHECK_BYTE);

				// Set NVRAM to the maximum version to need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_CONFIG_EXT3_VERSION - 1);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.GetUJPEnabled(), L"GetUJPEnabled default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetUJPEnabled()
		///     CConfig::GetUJPEnabled()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetUJPEnabled_NotUpgradeNvRamTest_CheckByteValid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710797);
			const BYTE VALID_CHECK_BYTE(0xc9);
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
				CConfig config1;
				config1.SetUJPEnabled(testValue);
				Assert::AreEqual(expectedResult, config1.GetUJPEnabled(), L"GetUJPEnabled setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.GetUJPEnabled(), L"GetUJPEnabled setup2");

				// Set NVRAM check byte valid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, VALID_CHECK_BYTE);

				// Set NVRAM to the minimum version to not need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_CONFIG_EXT3_VERSION);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(expectedResult, config.GetUJPEnabled(), L"GetUJPEnabled expected");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetUJPEnabled()
		///     CConfig::GetUJPEnabled()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetUJPEnabled_NotUpgradeNvRamTest_CheckByteInvalid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710797);
			const BYTE INVALID_CHECK_BYTE(0x00);
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
				CConfig config1;
				config1.SetUJPEnabled(testValue);
				Assert::AreEqual(expectedResult, config1.GetUJPEnabled(), L"GetUJPEnabled setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.GetUJPEnabled(), L"GetUJPEnabled setup2");

				// Set NVRAM check byte invalid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, INVALID_CHECK_BYTE);

				// Set NVRAM to the minimum version to not need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_CONFIG_EXT3_VERSION);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.GetUJPEnabled(), L"GetUJPEnabled default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}
// #pragma endregion GetUJPEnabled

// #pragma region MiniVirtualSessionEnabled
		/// <summary>
		/// Functional test for:
		///     CConfig::SetMiniVirtualSessionEnabled()
		///     CConfig::MiniVirtualSessionEnabled()
		/// Test setting a range of values.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetMiniVirtualSessionEnabledTest)
		{
			// Data.
			typedef bool TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			m_testIndex = -1;
			CConfig config;
			bool previousResult(true); // Prefer non-default.
			config.SetMiniVirtualSessionEnabled(previousResult);
			Assert::AreEqual(previousResult, config.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled setup");

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				Assert::AreEqual(previousResult, config.SetMiniVirtualSessionEnabled(testValue), L"SetMiniVirtualSessionEnabled");
				const CConfig &config(config);
				Assert::AreEqual(expectedResult, config.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled get");

				// Setup for next test.
				previousResult = expectedResult;
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::SetMiniVirtualSessionEnabled()
		///     CConfig::MiniVirtualSessionEnabled()
		/// Test setting a range of values.
		/// This normally tests saving and reading NVRAM values across restarts.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetMiniVirtualSessionEnabled_NvRamTest)
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
				CConfig config;
				config.SetMiniVirtualSessionEnabled(previousResult);
				Assert::AreEqual(previousResult, config.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled setup");
			}

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(previousResult, config.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled previous");
				Assert::AreEqual(previousResult, config.SetMiniVirtualSessionEnabled(testValue), L"SetMiniVirtualSessionEnabled");
				Assert::AreEqual(expectedResult, config.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled expected");

				// Setup for next test.
				previousResult = expectedResult;
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::MiniVirtualSessionEnabled()
		///     CConfig::SetMiniVirtualSessionEnabled()
		///     CNVRAMConfig::Get()
		/// Test includes reading NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetMiniVirtualSessionEnabled_NvRamGetTest)
		{
			// Data.
			const DWORD NVRAM_FIELD_OFFSET(710798);
			const UINT NVRAM_FIELD_SIZE(1);

			typedef bool TestDataType;
			Assert::AreEqual(sizeof(TestDataType), NVRAM_FIELD_SIZE, L"NVRAM_FIELD_SIZE");
			const TestDataType defaultValue(false);
			TestDataType previousResult(!defaultValue); // Prefer non-default.
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			{
				m_testIndex = -1;
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CConfig config;
				config.SetMiniVirtualSessionEnabled(previousResult);
				Assert::AreEqual(previousResult, config.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled setup");

				CNVRAMConfig nvramConfig;
				bool resultValue;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&resultValue, NVRAM_FIELD_SIZE);
				Assert::AreEqual(previousResult, resultValue, L"resultValue setup");
			}

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(previousResult, config.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled previous");

				CNVRAMConfig nvramConfig;
				bool nvramResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramResult, NVRAM_FIELD_SIZE);
				Assert::AreEqual(previousResult, nvramResult, L"nvramResult");

				// Perform operation(s) to be tested on next loop iteration.
				config.SetMiniVirtualSessionEnabled(testValue);
				Assert::AreEqual(expectedResult, config.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled expected");
				previousResult = expectedResult;
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::MiniVirtualSessionEnabled()
		///     CNVRAMConfig::Set()
		/// Test includes writing NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetMiniVirtualSessionEnabled_NvRamSetTest)
		{
			// Data.
			const DWORD NVRAM_FIELD_OFFSET(710798);
			const UINT NVRAM_FIELD_SIZE(1);

			typedef bool TestDataType;
			Assert::AreEqual(sizeof(TestDataType), NVRAM_FIELD_SIZE, L"NVRAM_FIELD_SIZE");
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
				Assert::AreEqual(previousResult, nvramResult, L"nvramResult setup");

				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(previousResult, config.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled setup");
			}

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(previousResult, config.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled previous");

				// Perform operation(s) to be tested on next loop iteration.
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CNVRAMConfig nvramConfig;
				bool nvramValue(testValue); // CNVRAMConfig::Set() requires read/write data.
				nvramConfig.Set(NVRAM_FIELD_OFFSET, (BYTE*)&nvramValue, NVRAM_FIELD_SIZE);
				bool nvramResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramResult, NVRAM_FIELD_SIZE);
				Assert::AreEqual(expectedResult, nvramResult, L"nvramResult expected");

				// Perform operation(s) to be tested on next loop iteration.
				previousResult = expectedResult;
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::ResetConfigMemory()
		///     CConfig::BuildYourself()
		///     CConfig::SetMiniVirtualSessionEnabled()
		///     CConfig::MiniVirtualSessionEnabled()
		/// Test setting a range of values.
		/// This tests clearing NVRAM upon restart.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetMiniVirtualSessionEnabled_ClearNvRamTest)
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
				CConfig config1;
				config1.SetMiniVirtualSessionEnabled(testValue);
				Assert::AreEqual(expectedResult, config1.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled setup2");

				// Set NVRAM invalid.
				CUnitTestingUtils::SetNvramInvalid();

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetMiniVirtualSessionEnabled()
		///     CConfig::MiniVirtualSessionEnabled()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetMiniVirtualSessionEnabled_UpgradeNvRamTest_CheckByteValid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710797);
			const BYTE VALID_CHECK_BYTE(0xc9);
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
				CConfig config1;
				config1.SetMiniVirtualSessionEnabled(testValue);
				Assert::AreEqual(expectedResult, config1.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled setup2");

				// Set NVRAM check byte valid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, VALID_CHECK_BYTE);

				// Set NVRAM to the maximum version to need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_CONFIG_EXT3_VERSION - 1);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetMiniVirtualSessionEnabled()
		///     CConfig::MiniVirtualSessionEnabled()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetMiniVirtualSessionEnabled_UpgradeNvRamTest_CheckByteInvalid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710797);
			const BYTE INVALID_CHECK_BYTE(0x00);
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
				CConfig config1;
				config1.SetMiniVirtualSessionEnabled(testValue);
				Assert::AreEqual(expectedResult, config1.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled setup2");

				// Set NVRAM check byte invalid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, INVALID_CHECK_BYTE);

				// Set NVRAM to the maximum version to need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_CONFIG_EXT3_VERSION - 1);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetMiniVirtualSessionEnabled()
		///     CConfig::MiniVirtualSessionEnabled()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetMiniVirtualSessionEnabled_NotUpgradeNvRamTest_CheckByteValid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710797);
			const BYTE VALID_CHECK_BYTE(0xc9);
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
				CConfig config1;
				config1.SetMiniVirtualSessionEnabled(testValue);
				Assert::AreEqual(expectedResult, config1.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled setup2");

				// Set NVRAM check byte valid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, VALID_CHECK_BYTE);

				// Set NVRAM to the minimum version to not need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_CONFIG_EXT3_VERSION);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(expectedResult, config.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled expected");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetMiniVirtualSessionEnabled()
		///     CConfig::MiniVirtualSessionEnabled()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetMiniVirtualSessionEnabled_NotUpgradeNvRamTest_CheckByteInvalid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710797);
			const BYTE INVALID_CHECK_BYTE(0x00);
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
				CConfig config1;
				config1.SetMiniVirtualSessionEnabled(testValue);
				Assert::AreEqual(expectedResult, config1.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled setup2");

				// Set NVRAM check byte invalid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, INVALID_CHECK_BYTE);

				// Set NVRAM to the minimum version to not need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_CONFIG_EXT3_VERSION);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.MiniVirtualSessionEnabled(), L"MiniVirtualSessionEnabled default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}
// #pragma endregion MiniVirtualSessionEnabled

// #pragma region OneLinkProgressivesEnabled
		/// <summary>
		/// Functional test for:
		///     CConfig::SetOneLinkProgressivesEnabled()
		///     CConfig::OneLinkProgressivesEnabled()
		/// Test setting a range of values.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetOneLinkProgressivesEnabledTest)
		{
			// Data.
			typedef bool TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			m_testIndex = -1;
			CConfig config;
			bool previousResult(true); // Prefer non-default.
			config.SetOneLinkProgressivesEnabled(previousResult);
			Assert::AreEqual(previousResult, config.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled setup");

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				Assert::AreEqual(previousResult, config.SetOneLinkProgressivesEnabled(testValue), L"SetOneLinkProgressivesEnabled");
				const CConfig &config(config);
				Assert::AreEqual(expectedResult, config.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled get");

				// Setup for next test.
				previousResult = expectedResult;
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::SetOneLinkProgressivesEnabled()
		///     CConfig::OneLinkProgressivesEnabled()
		/// Test setting a range of values.
		/// This normally tests saving and reading NVRAM values across restarts.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetOneLinkProgressivesEnabled_NvRamTest)
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
				CConfig config;
				config.SetOneLinkProgressivesEnabled(previousResult);
				Assert::AreEqual(previousResult, config.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled setup");
			}

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(previousResult, config.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled previous");
				Assert::AreEqual(previousResult, config.SetOneLinkProgressivesEnabled(testValue), L"SetOneLinkProgressivesEnabled");
				Assert::AreEqual(expectedResult, config.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled expected");

				// Setup for next test.
				previousResult = expectedResult;
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::OneLinkProgressivesEnabled()
		///     CConfig::SetOneLinkProgressivesEnabled()
		///     CNVRAMConfig::Get()
		/// Test includes reading NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetOneLinkProgressivesEnabled_NvRamGetTest)
		{
			// Data.
			const DWORD NVRAM_FIELD_OFFSET(710799);
			const UINT NVRAM_FIELD_SIZE(1);

			typedef bool TestDataType;
			Assert::AreEqual(sizeof(TestDataType), NVRAM_FIELD_SIZE, L"NVRAM_FIELD_SIZE");
			const TestDataType defaultValue(false);
			TestDataType previousResult(!defaultValue); // Prefer non-default.
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			{
				m_testIndex = -1;
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CConfig config;
				config.SetOneLinkProgressivesEnabled(previousResult);
				Assert::AreEqual(previousResult, config.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled setup");

				CNVRAMConfig nvramConfig;
				bool resultValue;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&resultValue, NVRAM_FIELD_SIZE);
				Assert::AreEqual(previousResult, resultValue, L"resultValue setup");
			}

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(previousResult, config.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled previous");

				CNVRAMConfig nvramConfig;
				bool nvramResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramResult, NVRAM_FIELD_SIZE);
				Assert::AreEqual(previousResult, nvramResult, L"nvramResult");

				// Perform operation(s) to be tested on next loop iteration.
				config.SetOneLinkProgressivesEnabled(testValue);
				Assert::AreEqual(expectedResult, config.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled expected");
				previousResult = expectedResult;
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::OneLinkProgressivesEnabled()
		///     CNVRAMConfig::Set()
		/// Test includes writing NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetOneLinkProgressivesEnabled_NvRamSetTest)
		{
			// Data.
			const DWORD NVRAM_FIELD_OFFSET(710799);
			const UINT NVRAM_FIELD_SIZE(1);

			typedef bool TestDataType;
			Assert::AreEqual(sizeof(TestDataType), NVRAM_FIELD_SIZE, L"NVRAM_FIELD_SIZE");
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
				Assert::AreEqual(previousResult, nvramResult, L"nvramResult setup");

				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(previousResult, config.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled setup");
			}

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(previousResult, config.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled previous");

				// Perform operation(s) to be tested on next loop iteration.
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CNVRAMConfig nvramConfig;
				bool nvramValue(testValue); // CNVRAMConfig::Set() requires read/write data.
				nvramConfig.Set(NVRAM_FIELD_OFFSET, (BYTE*)&nvramValue, NVRAM_FIELD_SIZE);
				bool nvramResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramResult, NVRAM_FIELD_SIZE);
				Assert::AreEqual(expectedResult, nvramResult, L"nvramResult expected");

				// Perform operation(s) to be tested on next loop iteration.
				previousResult = expectedResult;
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::ResetConfigMemory()
		///     CConfig::BuildYourself()
		///     CConfig::SetOneLinkProgressivesEnabled()
		///     CConfig::OneLinkProgressivesEnabled()
		/// Test setting a range of values.
		/// This tests clearing NVRAM upon restart.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetOneLinkProgressivesEnabled_ClearNvRamTest)
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
				CConfig config1;
				config1.SetOneLinkProgressivesEnabled(testValue);
				Assert::AreEqual(expectedResult, config1.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled setup2");

				// Set NVRAM invalid.
				CUnitTestingUtils::SetNvramInvalid();

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetOneLinkProgressivesEnabled()
		///     CConfig::OneLinkProgressivesEnabled()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetOneLinkProgressivesEnabled_UpgradeNvRamTest_CheckByteValid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710797);
			const BYTE VALID_CHECK_BYTE(0xc9);
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
				CConfig config1;
				config1.SetOneLinkProgressivesEnabled(testValue);
				Assert::AreEqual(expectedResult, config1.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled setup2");

				// Set NVRAM check byte valid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, VALID_CHECK_BYTE);

				// Set NVRAM to the maximum version to need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_CONFIG_EXT3_VERSION - 1);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetOneLinkProgressivesEnabled()
		///     CConfig::OneLinkProgressivesEnabled()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetOneLinkProgressivesEnabled_UpgradeNvRamTest_CheckByteInvalid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710797);
			const BYTE INVALID_CHECK_BYTE(0x00);
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
				CConfig config1;
				config1.SetOneLinkProgressivesEnabled(testValue);
				Assert::AreEqual(expectedResult, config1.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled setup2");

				// Set NVRAM check byte invalid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, INVALID_CHECK_BYTE);

				// Set NVRAM to the maximum version to need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_CONFIG_EXT3_VERSION - 1);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetOneLinkProgressivesEnabled()
		///     CConfig::OneLinkProgressivesEnabled()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetOneLinkProgressivesEnabled_NotUpgradeNvRamTest_CheckByteValid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710797);
			const BYTE VALID_CHECK_BYTE(0xc9);
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
				CConfig config1;
				config1.SetOneLinkProgressivesEnabled(testValue);
				Assert::AreEqual(expectedResult, config1.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled setup2");

				// Set NVRAM check byte valid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, VALID_CHECK_BYTE);

				// Set NVRAM to the minimum version to not need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_CONFIG_EXT3_VERSION);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(expectedResult, config.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled expected");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetOneLinkProgressivesEnabled()
		///     CConfig::OneLinkProgressivesEnabled()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetOneLinkProgressivesEnabled_NotUpgradeNvRamTest_CheckByteInvalid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710797);
			const BYTE INVALID_CHECK_BYTE(0x00);
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
				CConfig config1;
				config1.SetOneLinkProgressivesEnabled(testValue);
				Assert::AreEqual(expectedResult, config1.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled setup2");

				// Set NVRAM check byte invalid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, INVALID_CHECK_BYTE);

				// Set NVRAM to the minimum version to not need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_CONFIG_EXT3_VERSION);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.OneLinkProgressivesEnabled(), L"OneLinkProgressivesEnabled default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}
// #pragma endregion OneLinkProgressivesEnabled

// #pragma region DisplayPlayerProfileButton
		/// <summary>
		/// Functional test for:
		///     CConfig::SetDisplayPlayerProfileButton()
		///     CConfig::DisplayPlayerProfileButton()
		/// Test setting a range of values.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetDisplayPlayerProfileButtonTest)
		{
			// Data.
			typedef bool TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			m_testIndex = -1;
			CConfig config;
			bool previousResult(false); // Prefer non-default.
			config.SetDisplayPlayerProfileButton(previousResult);
			Assert::AreEqual(previousResult, config.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton setup");

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				Assert::AreEqual(previousResult, config.SetDisplayPlayerProfileButton(testValue), L"SetDisplayPlayerProfileButton");
				const CConfig &config(config);
				Assert::AreEqual(expectedResult, config.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton get");

				// Setup for next test.
				previousResult = expectedResult;
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::SetDisplayPlayerProfileButton()
		///     CConfig::DisplayPlayerProfileButton()
		/// Test setting a range of values.
		/// This normally tests saving and reading NVRAM values across restarts.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetDisplayPlayerProfileButton_NvRamTest)
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
				CConfig config;
				config.SetDisplayPlayerProfileButton(previousResult);
				Assert::AreEqual(previousResult, config.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton setup");
			}

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(previousResult, config.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton previous");
				Assert::AreEqual(previousResult, config.SetDisplayPlayerProfileButton(testValue), L"SetDisplayPlayerProfileButton");
				Assert::AreEqual(expectedResult, config.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton expected");

				// Setup for next test.
				previousResult = expectedResult;
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::DisplayPlayerProfileButton()
		///     CConfig::SetDisplayPlayerProfileButton()
		///     CNVRAMConfig::Get()
		/// Test includes reading NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetDisplayPlayerProfileButton_NvRamGetTest)
		{
			// Data.
			const DWORD NVRAM_FIELD_OFFSET(710803);
			const UINT NVRAM_FIELD_SIZE(1);

			typedef bool TestDataType;
			Assert::AreEqual(sizeof(TestDataType), NVRAM_FIELD_SIZE, L"NVRAM_FIELD_SIZE");
			const TestDataType defaultValue(false);
			TestDataType previousResult(!defaultValue); // Prefer non-default.
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			// Setup.
			{
				m_testIndex = -1;
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CConfig config;
				config.SetDisplayPlayerProfileButton(previousResult);
				Assert::AreEqual(previousResult, config.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton setup");

				CNVRAMConfig nvramConfig;
				bool resultValue;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&resultValue, NVRAM_FIELD_SIZE);
				Assert::AreEqual(previousResult, resultValue, L"resultValue setup");
			}

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(previousResult, config.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton previous");

				CNVRAMConfig nvramConfig;
				bool nvramResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramResult, NVRAM_FIELD_SIZE);
				Assert::AreEqual(previousResult, nvramResult, L"nvramResult");

				// Perform operation(s) to be tested on next loop iteration.
				config.SetDisplayPlayerProfileButton(testValue);
				Assert::AreEqual(expectedResult, config.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton expected");
				previousResult = expectedResult;
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::BuildYourself()
		///     CConfig::DisplayPlayerProfileButton()
		///     CNVRAMConfig::Set()
		/// Test includes writing NVRAM contents.
		/// This normally tests forward and backward compatibility across versions.
		/// Note that the NVRAM value is set to the default value on startup.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetDisplayPlayerProfileButton_NvRamSetTest)
		{
			// Data.
			const DWORD NVRAM_FIELD_OFFSET(710803);
			const UINT NVRAM_FIELD_SIZE(1);

			typedef bool TestDataType;
			Assert::AreEqual(sizeof(TestDataType), NVRAM_FIELD_SIZE, L"NVRAM_FIELD_SIZE");
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
				Assert::AreEqual(previousResult, nvramResult, L"nvramResult setup");

				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(previousResult, config.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton setup");
			}

			for (m_testIndex = 0; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the next test value.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);
				const TestDataType expectedResult(testValue);

				// Perform operation(s) and test(s).
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(previousResult, config.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton previous");

				// Perform operation(s) to be tested on next loop iteration.
				CUnitTestingUtils::SetNvramValidAndUpgraded();
				CNVRAMConfig nvramConfig;
				bool nvramValue(testValue); // CNVRAMConfig::Set() requires read/write data.
				nvramConfig.Set(NVRAM_FIELD_OFFSET, (BYTE*)&nvramValue, NVRAM_FIELD_SIZE);
				bool nvramResult;
				nvramConfig.Get(NVRAM_FIELD_OFFSET, (BYTE*)&nvramResult, NVRAM_FIELD_SIZE);
				Assert::AreEqual(expectedResult, nvramResult, L"nvramResult expected");

				// Perform operation(s) to be tested on next loop iteration.
				previousResult = expectedResult;
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles();
		///     CConfig::CConfig()
		///     CConfig::ConfigIntegrityCheck()
		///     CConfig::ResetConfigMemory()
		///     CConfig::BuildYourself()
		///     CConfig::SetDisplayPlayerProfileButton()
		///     CConfig::DisplayPlayerProfileButton()
		/// Test setting a range of values.
		/// This tests clearing NVRAM upon restart.
		/// </summary>
		TEST_F(ConfigAUnitTest, SetDisplayPlayerProfileButton_ClearNvRamTest)
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
				CConfig config1;
				config1.SetDisplayPlayerProfileButton(testValue);
				Assert::AreEqual(expectedResult, config1.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton setup2");

				// Set NVRAM invalid.
				CUnitTestingUtils::SetNvramInvalid();

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetDisplayPlayerProfileButton()
		///     CConfig::DisplayPlayerProfileButton()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetDisplayPlayerProfileButton_UpgradeNvRamTest_CheckByteValid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710802);
			const BYTE VALID_CHECK_BYTE(0xd3);
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
				CConfig config1;
				config1.SetDisplayPlayerProfileButton(testValue);
				Assert::AreEqual(expectedResult, config1.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton setup2");

				// Set NVRAM check byte valid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, VALID_CHECK_BYTE);

				// Set NVRAM to the maximum version to need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_DISPLAY_PLAYER_PROFILE_BUTTON_VERSION - 1);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetDisplayPlayerProfileButton()
		///     CConfig::DisplayPlayerProfileButton()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetDisplayPlayerProfileButton_UpgradeNvRamTest_CheckByteInvalid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710802);
			const BYTE INVALID_CHECK_BYTE(0x00);
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
				CConfig config1;
				config1.SetDisplayPlayerProfileButton(testValue);
				Assert::AreEqual(expectedResult, config1.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton setup2");

				// Set NVRAM check byte invalid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, INVALID_CHECK_BYTE);

				// Set NVRAM to the maximum version to need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_DISPLAY_PLAYER_PROFILE_BUTTON_VERSION - 1);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetDisplayPlayerProfileButton()
		///     CConfig::DisplayPlayerProfileButton()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetDisplayPlayerProfileButton_NotUpgradeNvRamTest_CheckByteValid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710802);
			const BYTE VALID_CHECK_BYTE(0xd3);
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
				CConfig config1;
				config1.SetDisplayPlayerProfileButton(testValue);
				Assert::AreEqual(expectedResult, config1.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton setup2");

				// Set NVRAM check byte valid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, VALID_CHECK_BYTE);

				// Set NVRAM to the minimum version to not need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_DISPLAY_PLAYER_PROFILE_BUTTON_VERSION);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(expectedResult, config.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton expected");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
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
		///     CConfig::SetDisplayPlayerProfileButton()
		///     CConfig::DisplayPlayerProfileButton()
		/// </summary>
		TEST_F(ConfigAUnitTest, SetDisplayPlayerProfileButton_NotUpgradeNvRamTest_CheckByteInvalid)
		{
			// Data.
			const DWORD CHECK_BYTE_OFFSET(710802);
			const BYTE INVALID_CHECK_BYTE(0x00);
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
				CConfig config1;
				config1.SetDisplayPlayerProfileButton(testValue);
				Assert::AreEqual(expectedResult, config1.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton setup");

				// Verify setup NVRAM to non-reset state.
				CConfig config2;
				config2.BuildYourself();
				Assert::AreEqual(expectedResult, config2.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton setup2");

				// Set NVRAM check byte invalid for the data under test.
				CUnitTestingUtils::NvramWrite(CHECK_BYTE_OFFSET, INVALID_CHECK_BYTE);

				// Set NVRAM to the minimum version to not need an upgrade for the data under test.
				CUnitTestingUtils::SetNvramVersionAndSprinkles(NVRAM_DISPLAY_PLAYER_PROFILE_BUTTON_VERSION);

				// Perform operation(s) to be tested.
				CConfig config;
				config.BuildYourself();
				Assert::AreEqual(defaultValue, config.DisplayPlayerProfileButton(), L"DisplayPlayerProfileButton default");
			}

			ASSERT_TRUE(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}
// #pragma endregion DisplayPlayerProfileButton
#endif //TODO-PORT
}
