#include "stdafx.h"
#include "ConfigSenOption1Field.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigSenOption1Field : public ::testing::Test
	{
	public:
	};

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::CConfigSenOption1Field().
		/// </summary>
		TEST(CConfigSenOption1FieldTest, CConfigSenOption1FieldTest)
		{
			// Setup.
			const CConfigSenOption1Field configField;
			const FieldTypes typeExpected = SEN_OPTIONS1;

			// Test #1.
			const FieldTypes typeResult = configField.GetType();

			{
				// Report #1.
				bool areEqual = typeResult == typeExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED typeResult=%d expected=%d in %s"),
					(int)typeResult,
					(int)typeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::GetSenOptions()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, GetSenOptionsTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const DWORD m_senOptionsExpected;
			} TestDataType;

			const byte buffer1[] = { 0 };
			const byte buffer2[] = { 1 };
			const byte buffer3[] = { 0x7f };
			const byte buffer4[] = { 0x80 };
			const byte buffer5[] = { 0xfe };
			const byte buffer6[] = { 0xff };

			const byte buffer7[] = { 0, 0 };
			const byte buffer8[] = { 1, 0 };
			const byte buffer9[] = { 1, 2 };
			const byte buffer10[] = { 0xff, 0x7f };
			const byte buffer11[] = { 0x00, 0x80 };
			const byte buffer12[] = { 0xfe, 0xfd };
			const byte buffer13[] = { 0xfe, 0xff };
			const byte buffer14[] = { 0xff, 0xff };

			const byte buffer15[] = { 0, 0, 0 };
			const byte buffer16[] = { 1, 0, 0 };
			const byte buffer17[] = { 1, 2, 3 };
			const byte buffer18[] = { 0xff, 0xff, 0x7f };
			const byte buffer19[] = { 0x00, 0x00, 0x80 };
			const byte buffer20[] = { 0xfe, 0xfd, 0xfc };
			const byte buffer21[] = { 0xfe, 0xff, 0xff };
			const byte buffer22[] = { 0xff, 0xff, 0xff };

			const byte buffer23[] = { 0, 0, 0, 0 };
			const byte buffer24[] = { 1, 0, 0, 0 };
			const byte buffer25[] = { 1, 2, 3, 4 };
			const byte buffer26[] = { 0xff, 0xff, 0xff, 0x7f };
			const byte buffer27[] = { 0x00, 0x00, 0x00, 0x80 };
			const byte buffer28[] = { 0xfe, 0xfd, 0xfc, 0xfb };
			const byte buffer29[] = { 0xfe, 0xff, 0xff, 0xff };
			const byte buffer30[] = { 0xff, 0xff, 0xff, 0xff };

			const byte buffer31[] = { 0, 0, 0, 0, 0xff };
			const byte buffer32[] = { 1, 0, 0, 0, 0xfe };
			const byte buffer33[] = { 1, 2, 3, 4, 0xfd };
			const byte buffer34[] = { 0xff, 0xff, 0xff, 0x7f, 0x80 };
			const byte buffer35[] = { 0x00, 0x00, 0x00, 0x80, 0x7f };
			const byte buffer36[] = { 0xfe, 0xfd, 0xfc, 0xfb, 2 };
			const byte buffer37[] = { 0xfe, 0xff, 0xff, 0xff, 1 };
			const byte buffer38[] = { 0xff, 0xff, 0xff, 0xff, 0 };

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, 0 },

				{ buffer1, sizeof(buffer1), 0 },
				{ buffer2, sizeof(buffer2), 1 },
				{ buffer3, sizeof(buffer3), 0x7f },
				{ buffer4, sizeof(buffer4), 0x80 },
				{ buffer5, sizeof(buffer5), 0xfe },
				{ buffer6, sizeof(buffer6), 0xff },

				{ buffer7, sizeof(buffer7), 0 },
				{ buffer8, sizeof(buffer8), 1 },
				{ buffer9, sizeof(buffer9), 0x0201 },
				{ buffer10, sizeof(buffer10), 0x7fff },
				{ buffer11, sizeof(buffer11), 0x8000 },
				{ buffer12, sizeof(buffer12), 0xfdfe },
				{ buffer13, sizeof(buffer13), 0xfffe },
				{ buffer14, sizeof(buffer14), 0xffff },

				{ buffer15, sizeof(buffer15), 0 },
				{ buffer16, sizeof(buffer16), 1 },
				{ buffer17, sizeof(buffer17), 0x030201 },
				{ buffer18, sizeof(buffer18), 0x7fffff },
				{ buffer19, sizeof(buffer19), 0x800000 },
				{ buffer20, sizeof(buffer20), 0xfcfdfe },
				{ buffer21, sizeof(buffer21), 0xfffffe },
				{ buffer22, sizeof(buffer22), 0xffffff },

				{ buffer23, sizeof(buffer23), 0 },
				{ buffer24, sizeof(buffer24), 1 },
				{ buffer25, sizeof(buffer25), 0x04030201 },
				{ buffer26, sizeof(buffer26), 0x7fffffff },
				{ buffer27, sizeof(buffer27), 0x80000000 },
				{ buffer28, sizeof(buffer28), 0xfbfcfdfe },
				{ buffer29, sizeof(buffer29), 0xfffffffe },
				{ buffer30, sizeof(buffer30), 0xffffffff },

				{ buffer31, sizeof(buffer31), 0 },
				{ buffer32, sizeof(buffer32), 1 },
				{ buffer33, sizeof(buffer33), 0x04030201 },
				{ buffer34, sizeof(buffer34), 0x7fffffff },
				{ buffer35, sizeof(buffer35), 0x80000000 },
				{ buffer36, sizeof(buffer36), 0xfbfcfdfe },
				{ buffer37, sizeof(buffer37), 0xfffffffe },
				{ buffer38, sizeof(buffer38), 0xffffffff },

				{ NULL, 33, 0, },
				{ NULL, -33, 0, },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const DWORD senOptionsResult = configField.GetSenOptions();

				{
					// Report #1.
					bool areEqual = senOptionsResult == TEST_DATA[testIndex].m_senOptionsExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED senOptionsResult=%u expected=%u testIndex=%d in %s"),
						(unsigned)senOptionsResult,
						(unsigned)TEST_DATA[testIndex].m_senOptionsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::FillAmount()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, FillAmountTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_fillAmountExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x01, 0x00, 0x00, 0x00 };
			const byte buffer2[] = { 0xfe, 0xff, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool fillAmountResult = configField.FillAmount();

				{
					// Report #1.
					bool areEqual = fillAmountResult == TEST_DATA[testIndex].m_fillAmountExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED fillAmountResult=%d expected=%d testIndex=%d in %s"),
						(int)fillAmountResult,
						(int)TEST_DATA[testIndex].m_fillAmountExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::UseHopper()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, UseHopperTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_useHopperExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x02, 0x00, 0x00, 0x00 };
			const byte buffer2[] = { 0xfd, 0xff, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool useHopperResult = configField.UseHopper();

				{
					// Report #1.
					bool areEqual = useHopperResult == TEST_DATA[testIndex].m_useHopperExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED useHopperResult=%d expected=%d testIndex=%d in %s"),
						(int)useHopperResult,
						(int)TEST_DATA[testIndex].m_useHopperExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::DisplayNumericPointTotal()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, DisplayNumericPointTotalTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_displayNumericPointTotalExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x08, 0x00, 0x00, 0x00 };
			const byte buffer2[] = { 0xf7, 0xff, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool displayNumericPointTotalResult = configField.DisplayNumericPointTotal();

				{
					// Report #1.
					bool areEqual = displayNumericPointTotalResult == TEST_DATA[testIndex].m_displayNumericPointTotalExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED displayNumericPointTotalResult=%d expected=%d testIndex=%d in %s"),
						(int)displayNumericPointTotalResult,
						(int)TEST_DATA[testIndex].m_displayNumericPointTotalExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::DisplayPlayerName()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, DisplayPlayerNameTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_displayPlayerNameExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x10, 0x00, 0x00, 0x00 };
			const byte buffer2[] = { 0xef, 0xff, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool displayPlayerNameResult = configField.DisplayPlayerName();

				{
					// Report #1.
					bool areEqual = displayPlayerNameResult == TEST_DATA[testIndex].m_displayPlayerNameExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED displayPlayerNameResult=%d expected=%d testIndex=%d in %s"),
						(int)displayPlayerNameResult,
						(int)TEST_DATA[testIndex].m_displayPlayerNameExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::DisplayCountdownAndSession()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, DisplayCountdownAndSessionTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_displayCountdownAndSessionExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x20, 0x00, 0x00, 0x00 };
			const byte buffer2[] = { 0xdf, 0xff, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool displayCountdownAndSessionResult = configField.DisplayCountdownAndSession();

				{
					// Report #1.
					bool areEqual = displayCountdownAndSessionResult == TEST_DATA[testIndex].m_displayCountdownAndSessionExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED displayCountdownAndSessionResult=%d expected=%d testIndex=%d in %s"),
						(int)displayCountdownAndSessionResult,
						(int)TEST_DATA[testIndex].m_displayCountdownAndSessionExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::DisplayTotalPtsSessionPtsAsCash()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, DisplayTotalPtsSessionPtsAsCashTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_displayTotalPtsSessionPtsAsCashExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x40, 0x00, 0x00, 0x00 };
			const byte buffer2[] = { 0xbf, 0xff, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool displayTotalPtsSessionPtsAsCashResult = configField.DisplayTotalPtsSessionPtsAsCash();

				{
					// Report #1.
					bool areEqual = displayTotalPtsSessionPtsAsCashResult == TEST_DATA[testIndex].m_displayTotalPtsSessionPtsAsCashExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED displayTotalPtsSessionPtsAsCashResult=%d expected=%d testIndex=%d in %s"),
						(int)displayTotalPtsSessionPtsAsCashResult,
						(int)TEST_DATA[testIndex].m_displayTotalPtsSessionPtsAsCashExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::PBTCashEnable()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, PBTCashEnableTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_pbtCashEnableExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x01, 0x00, 0x00 };
			const byte buffer2[] = { 0xff, 0xfe, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool pbtCashEnableResult = configField.PBTCashEnable();

				{
					// Report #1.
					bool areEqual = pbtCashEnableResult == TEST_DATA[testIndex].m_pbtCashEnableExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pbtCashEnableResult=%d expected=%d testIndex=%d in %s"),
						(int)pbtCashEnableResult,
						(int)TEST_DATA[testIndex].m_pbtCashEnableExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::PBTPointsEnable()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, PBTPointsEnableTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_pbtPointsEnableExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x02, 0x00, 0x00 };
			const byte buffer2[] = { 0xff, 0xfd, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool pbtPointsEnable = configField.PBTPointsEnable();

				{
					// Report #1.
					bool areEqual = pbtPointsEnable == TEST_DATA[testIndex].m_pbtPointsEnableExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pbtPointsEnable=%d expected=%d testIndex=%d in %s"),
						(int)pbtPointsEnable,
						(int)TEST_DATA[testIndex].m_pbtPointsEnableExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::PBTPromoEnable()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, PBTPromoEnableTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_pbtPromoEnableExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x04, 0x00, 0x00 };
			const byte buffer2[] = { 0xff, 0xfb, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool pbtPromoEnableResult = configField.PBTPromoEnable();

				{
					// Report #1.
					bool areEqual = pbtPromoEnableResult == TEST_DATA[testIndex].m_pbtPromoEnableExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pbtPromoEnableResult=%d expected=%d testIndex=%d in %s"),
						(int)pbtPromoEnableResult,
						(int)TEST_DATA[testIndex].m_pbtPromoEnableExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::DisplayComp()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, DisplayCompTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_displayCompExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x08, 0x00, 0x00 };
			const byte buffer2[] = { 0xff, 0xf7, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool displayCompResult = configField.DisplayComp();

				{
					// Report #1.
					bool areEqual = displayCompResult == TEST_DATA[testIndex].m_displayCompExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED displayCompResult=%d expected=%d testIndex=%d in %s"),
						(int)displayCompResult,
						(int)TEST_DATA[testIndex].m_displayCompExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::PBTPointsAsPromo()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, PBTPointsAsPromoTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_pbtPointsAsPromoExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x10, 0x00, 0x00 };
			const byte buffer2[] = { 0xff, 0xef, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool pbtPointsAsPromoResult = configField.PBTPointsAsPromo();

				{
					// Report #1.
					bool areEqual = pbtPointsAsPromoResult == TEST_DATA[testIndex].m_pbtPointsAsPromoExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pbtPointsAsPromoResult=%d expected=%d testIndex=%d in %s"),
						(int)pbtPointsAsPromoResult,
						(int)TEST_DATA[testIndex].m_pbtPointsAsPromoExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::CoinsWonAccounting()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, CoinsWonAccountingTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_coinsWonAccountingExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x20, 0x00, 0x00 };
			const byte buffer2[] = { 0xff, 0xdf, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool coinsWonAccountingResult = configField.CoinsWonAccounting();

				{
					// Report #1.
					bool areEqual = coinsWonAccountingResult == TEST_DATA[testIndex].m_coinsWonAccountingExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED coinsWonAccountingResult=%d expected=%d testIndex=%d in %s"),
						(int)coinsWonAccountingResult,
						(int)TEST_DATA[testIndex].m_coinsWonAccountingExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::CoinsInAccounting()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, CoinsInAccountingTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_coinsInAccountingExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x40, 0x00, 0x00 };
			const byte buffer2[] = { 0xff, 0xbf, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool coinsInAccountingResult = configField.CoinsInAccounting();

				{
					// Report #1.
					bool areEqual = coinsInAccountingResult == TEST_DATA[testIndex].m_coinsInAccountingExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED coinsInAccountingResult=%d expected=%d testIndex=%d in %s"),
						(int)coinsInAccountingResult,
						(int)TEST_DATA[testIndex].m_coinsInAccountingExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::DisplayCongrats()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, DisplayCongratsTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_displayCongratsExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x80, 0x00, 0x00 };
			const byte buffer2[] = { 0xff, 0x7f, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool displayCongratsResult = configField.DisplayCongrats();

				{
					// Report #1.
					bool areEqual = displayCongratsResult == TEST_DATA[testIndex].m_displayCongratsExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED displayCongratsResult=%d expected=%d testIndex=%d in %s"),
						(int)displayCongratsResult,
						(int)TEST_DATA[testIndex].m_displayCongratsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::TheoreticalAccounting()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, TheoreticalAccountingTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_theoreticalAccountingExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x00, 0x01, 0x00 };
			const byte buffer2[] = { 0xff, 0xff, 0xfe, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool theoreticalAccountingResult = configField.TheoreticalAccounting();

				{
					// Report #1.
					bool areEqual = theoreticalAccountingResult == TEST_DATA[testIndex].m_theoreticalAccountingExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED theoreticalAccountingResult=%d expected=%d testIndex=%d in %s"),
						(int)theoreticalAccountingResult,
						(int)TEST_DATA[testIndex].m_theoreticalAccountingExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::CoinsOutAccounting()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, CoinsOutAccountingTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_coinsOutAccountingExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x00, 0x02, 0x00 };
			const byte buffer2[] = { 0xff, 0xff, 0xfd, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool coinsOutAccountingResult = configField.CoinsOutAccounting();

				{
					// Report #1.
					bool areEqual = coinsOutAccountingResult == TEST_DATA[testIndex].m_coinsOutAccountingExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED coinsOutAccountingResult=%d expected=%d testIndex=%d in %s"),
						(int)coinsOutAccountingResult,
						(int)TEST_DATA[testIndex].m_coinsOutAccountingExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::CardInPBT()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, CardInPBTTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_cardInPBTExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x00, 0x04, 0x00 };
			const byte buffer2[] = { 0xff, 0xff, 0xfb, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool cardInPBTResult = configField.CardInPBT();

				{
					// Report #1.
					bool areEqual = cardInPBTResult == TEST_DATA[testIndex].m_cardInPBTExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED cardInPBTResult=%d expected=%d testIndex=%d in %s"),
						(int)cardInPBTResult,
						(int)TEST_DATA[testIndex].m_cardInPBTExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::EmployeePinRequired()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, EmployeePinRequiredTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_employeePinRequiredExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x00, 0x08, 0x00 };
			const byte buffer2[] = { 0xff, 0xff, 0xf7, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool employeePinRequiredResult = configField.EmployeePinRequired();

				{
					// Report #1.
					bool areEqual = employeePinRequiredResult == TEST_DATA[testIndex].m_employeePinRequiredExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED employeePinRequiredResult=%d expected=%d testIndex=%d in %s"),
						(int)employeePinRequiredResult,
						(int)TEST_DATA[testIndex].m_employeePinRequiredExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::PartialBonusAward()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, PartialBonusAwardTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_partialBonusAwardExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x00, 0x20, 0x00 };
			const byte buffer2[] = { 0xff, 0xff, 0xdf, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool partialBonusAwardResult = configField.PartialBonusAward();

				{
					// Report #1.
					bool areEqual = partialBonusAwardResult == TEST_DATA[testIndex].m_partialBonusAwardExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED partialBonusAwardResult=%d expected=%d testIndex=%d in %s"),
						(int)partialBonusAwardResult,
						(int)TEST_DATA[testIndex].m_partialBonusAwardExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::DisplayPtsTotalAsCash()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, DisplayPtsTotalAsCashTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_displayPtsTotalAsCashExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x00, 0x40, 0x00 };
			const byte buffer2[] = { 0xff, 0xff, 0xbf, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool displayPtsTotalAsCashResult = configField.DisplayPtsTotalAsCash();

				{
					// Report #1.
					bool areEqual = displayPtsTotalAsCashResult == TEST_DATA[testIndex].m_displayPtsTotalAsCashExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED displayPtsTotalAsCashResult=%d expected=%d testIndex=%d in %s"),
						(int)displayPtsTotalAsCashResult,
						(int)TEST_DATA[testIndex].m_displayPtsTotalAsCashExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::DisplayGoodLuckAndSession()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, DisplayGoodLuckAndSessionTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_displayGoodLuckAndSessionExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x00, 0x80, 0x00 };
			const byte buffer2[] = { 0xff, 0xff, 0x7f, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool displayGoodLuckAndSessionResult = configField.DisplayGoodLuckAndSession();

				{
					// Report #1.
					bool areEqual = displayGoodLuckAndSessionResult == TEST_DATA[testIndex].m_displayGoodLuckAndSessionExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED displayGoodLuckAndSessionResult=%d expected=%d testIndex=%d in %s"),
						(int)displayGoodLuckAndSessionResult,
						(int)TEST_DATA[testIndex].m_displayGoodLuckAndSessionExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::CouponPromoInEnabled()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, CouponPromoInEnabledTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_couponPromoInEnabledExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x00, 0x00, 0x01 };
			const byte buffer2[] = { 0xff, 0xff, 0xff, 0xfe };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool couponPromoInEnabledResult = configField.CouponPromoInEnabled();

				{
					// Report #1.
					bool areEqual = couponPromoInEnabledResult == TEST_DATA[testIndex].m_couponPromoInEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED couponPromoInEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)couponPromoInEnabledResult,
						(int)TEST_DATA[testIndex].m_couponPromoInEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::VoucherPromoInEnabled()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, VoucherPromoInEnabledTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_voucherPromoInEnabledExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x00, 0x00, 0x02 };
			const byte buffer2[] = { 0xff, 0xff, 0xff, 0xfd };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool voucherPromoInEnabledResult = configField.VoucherPromoInEnabled();

				{
					// Report #1.
					bool areEqual = voucherPromoInEnabledResult == TEST_DATA[testIndex].m_voucherPromoInEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED voucherPromoInEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)voucherPromoInEnabledResult,
						(int)TEST_DATA[testIndex].m_voucherPromoInEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::VirtualSessionEnabled()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, VirtualSessionEnabledTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_virtualSessionEnabledExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x00, 0x00, 0x04 };
			const byte buffer2[] = { 0xff, 0xff, 0xff, 0xfb };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool virtualSessionEnabledResult = configField.VirtualSessionEnabled();

				{
					// Report #1.
					bool areEqual = virtualSessionEnabledResult == TEST_DATA[testIndex].m_virtualSessionEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED virtualSessionEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)virtualSessionEnabledResult,
						(int)TEST_DATA[testIndex].m_virtualSessionEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::TicketInEnabled()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, TicketInEnabledTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_ticketInEnabledExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x00, 0x00, 0x08 };
			const byte buffer2[] = { 0xff, 0xff, 0xff, 0xf7 };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool ticketInEnabledResult = configField.TicketInEnabled();

				{
					// Report #1.
					bool areEqual = ticketInEnabledResult == TEST_DATA[testIndex].m_ticketInEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED ticketInEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)ticketInEnabledResult,
						(int)TEST_DATA[testIndex].m_ticketInEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::UseNewPBMenus()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, UseNewPBMenusTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_useNewPBMenusExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x00, 0x00, 0x10 };
			const byte buffer2[] = { 0xff, 0xff, 0xff, 0xef };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool useNewPBMenusResult = configField.UseNewPBMenus();

				{
					// Report #1.
					bool areEqual = useNewPBMenusResult == TEST_DATA[testIndex].m_useNewPBMenusExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED useNewPBMenusResult=%d expected=%d testIndex=%d in %s"),
						(int)useNewPBMenusResult,
						(int)TEST_DATA[testIndex].m_useNewPBMenusExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::PBFromGameEnabled()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, PBFromGameEnabledTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_pbFromGameEnabledExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x00, 0x00, 0x20 };
			const byte buffer2[] = { 0xff, 0xff, 0xff, 0xdf };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool pbFromGameEnabledResult = configField.PBFromGameEnabled();

				{
					// Report #1.
					bool areEqual = pbFromGameEnabledResult == TEST_DATA[testIndex].m_pbFromGameEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pbFromGameEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)pbFromGameEnabledResult,
						(int)TEST_DATA[testIndex].m_pbFromGameEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption1Field::SetSenOptions()
		///     CConfigSenOption1Field::DisplayPlayerPtsTotal()
		/// </summary>
		TEST(CConfigSenOption1FieldTest, DisplayPlayerPtsTotalTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_displayPlayerPtsTotalExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x00, 0x00, 0x40 };
			const byte buffer2[] = { 0xff, 0xff, 0xff, 0xbf };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), true },
				{ buffer2, sizeof(buffer2), false },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption1Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption1Field &configField(varConfigField);

				// Test.
				const bool displayPlayerPtsTotalResult = configField.DisplayPlayerPtsTotal();

				{
					// Report #1.
					bool areEqual = displayPlayerPtsTotalResult == TEST_DATA[testIndex].m_displayPlayerPtsTotalExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED displayPlayerPtsTotalResult=%d expected=%d testIndex=%d in %s"),
						(int)displayPlayerPtsTotalResult,
						(int)TEST_DATA[testIndex].m_displayPlayerPtsTotalExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}
}
