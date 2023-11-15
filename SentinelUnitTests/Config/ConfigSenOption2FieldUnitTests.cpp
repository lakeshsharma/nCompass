#include "stdafx.h"
#include "ConfigSenOption2Field.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigSenOption2Field : public ::testing::Test
	{
	public:
	};

		/// Test the implementation of:
		///     CConfigSenOption2Field::CConfigSenOption2Field().
		/// </summary>
		TEST(CConfigSenOption2FieldTest, CConfigSenOption2FieldTest)
		{
			// Setup.
			const CConfigSenOption2Field configField;
			const FieldTypes typeExpected = SEN_OPTIONS2;

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
		///     CConfigSenOption2Field::SetSenOptions()
		///     CConfigSenOption2Field::GetSenOptions()
		/// </summary>
		TEST(CConfigSenOption2FieldTest, GetSenOptionsTest)
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
				CConfigSenOption2Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption2Field &configField(varConfigField);

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
		///     CConfigSenOption2Field::SetSenOptions()
		///     CConfigSenOption2Field::PBTPinNotRequired()
		/// </summary>
		TEST(CConfigSenOption2FieldTest, PBTPinNotRequiredTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_pbtPinNotRequiredExpected;
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
				CConfigSenOption2Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption2Field &configField(varConfigField);

				// Test.
				const bool pbtPinNotRequiredResult = configField.PBTPinNotRequired();

				{
					// Report #1.
					bool areEqual = pbtPinNotRequiredResult == TEST_DATA[testIndex].m_pbtPinNotRequiredExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pbtPinNotRequiredResult=%d expected=%d testIndex=%d in %s"),
						(int)pbtPinNotRequiredResult,
						(int)TEST_DATA[testIndex].m_pbtPinNotRequiredExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption2Field::SetSenOptions()
		///     CConfigSenOption2Field::CouponPromoOutEnabled()
		/// </summary>
		TEST(CConfigSenOption2FieldTest, CouponPromoOutEnabledTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_couponPromoOutEnabledExpected;
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
				CConfigSenOption2Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption2Field &configField(varConfigField);

				// Test.
				const bool couponPromoOutEnabledResult = configField.CouponPromoOutEnabled();

				{
					// Report #1.
					bool areEqual = couponPromoOutEnabledResult == TEST_DATA[testIndex].m_couponPromoOutEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED couponPromoOutEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)couponPromoOutEnabledResult,
						(int)TEST_DATA[testIndex].m_couponPromoOutEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption2Field::SetSenOptions()
		///     CConfigSenOption2Field::VoucherOutEnabled()
		/// </summary>
		TEST(CConfigSenOption2FieldTest, VoucherOutEnabledTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_voucherOutEnabledExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x04, 0x00, 0x00, 0x00 };
			const byte buffer2[] = { 0xfb, 0xff, 0xff, 0xff };
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
				CConfigSenOption2Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption2Field &configField(varConfigField);

				// Test.
				const bool voucherOutEnabledResult = configField.VoucherOutEnabled();

				{
					// Report #1.
					bool areEqual = voucherOutEnabledResult == TEST_DATA[testIndex].m_voucherOutEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED voucherOutEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)voucherOutEnabledResult,
						(int)TEST_DATA[testIndex].m_voucherOutEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption2Field::SetSenOptions()
		///     CConfigSenOption2Field::DisplayGoodLuckAndSessionPts()
		/// </summary>
		TEST(CConfigSenOption2FieldTest, DisplayGoodLuckAndSessionPtsTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_displayGoodLuckAndSessionPtsExpected;
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
				CConfigSenOption2Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption2Field &configField(varConfigField);

				// Test.
				const bool displayGoodLuckAndSessionPtsResult = configField.DisplayGoodLuckAndSessionPts();

				{
					// Report #1.
					bool areEqual = displayGoodLuckAndSessionPtsResult == TEST_DATA[testIndex].m_displayGoodLuckAndSessionPtsExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED displayGoodLuckAndSessionPtsResult=%d expected=%d testIndex=%d in %s"),
						(int)displayGoodLuckAndSessionPtsResult,
						(int)TEST_DATA[testIndex].m_displayGoodLuckAndSessionPtsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption2Field::SetSenOptions()
		///     CConfigSenOption2Field::DisplayTotalAndSessionPtsNum()
		/// </summary>
		TEST(CConfigSenOption2FieldTest, DisplayTotalAndSessionPtsNumTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_displayTotalAndSessionPtsNumExpected;
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
				CConfigSenOption2Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption2Field &configField(varConfigField);

				// Test.
				const bool displayTotalAndSessionPtsNumResult = configField.DisplayTotalAndSessionPtsNum();

				{
					// Report #1.
					bool areEqual = displayTotalAndSessionPtsNumResult == TEST_DATA[testIndex].m_displayTotalAndSessionPtsNumExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED displayTotalAndSessionPtsNumResult=%d expected=%d testIndex=%d in %s"),
						(int)displayTotalAndSessionPtsNumResult,
						(int)TEST_DATA[testIndex].m_displayTotalAndSessionPtsNumExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption2Field::SetSenOptions()
		///     CConfigSenOption2Field::DisplayPtsTotalAsCashSession()
		/// </summary>
		TEST(CConfigSenOption2FieldTest, DisplayPtsTotalAsCashSessionTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_displayPtsTotalAsCashSessionExpected;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x80, 0x00, 0x00, 0x00 };
			const byte buffer2[] = { 0x7f, 0xff, 0xff, 0xff };
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
				CConfigSenOption2Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption2Field &configField(varConfigField);

				// Test.
				const bool displayPtsTotalAsCashSessionResult = configField.DisplayPtsTotalAsCashSession();

				{
					// Report #1.
					bool areEqual = displayPtsTotalAsCashSessionResult == TEST_DATA[testIndex].m_displayPtsTotalAsCashSessionExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED displayTotalAndSessionPtsNumResult=%d expected=%d testIndex=%d in %s"),
						(int)displayPtsTotalAsCashSessionResult,
						(int)TEST_DATA[testIndex].m_displayPtsTotalAsCashSessionExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption2Field::SetSenOptions()
		///     CConfigSenOption2Field::RcclCardsEnabled()
		/// </summary>
		TEST(CConfigSenOption2FieldTest, RcclCardsEnabledTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_rcclCardsEnabledExpected;
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
				CConfigSenOption2Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption2Field &configField(varConfigField);

				// Test.
				const bool rcclCardsEnabledResult = configField.RcclCardsEnabled();

				{
					// Report #1.
					bool areEqual = rcclCardsEnabledResult == TEST_DATA[testIndex].m_rcclCardsEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED rcclCardsEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)rcclCardsEnabledResult,
						(int)TEST_DATA[testIndex].m_rcclCardsEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}
		
		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption2Field::SetSenOptions()
		///     CConfigSenOption2Field::MiniVirtualSessionEnabled()
		/// </summary>
		TEST(CConfigSenOption2FieldTest, MiniVirtualSessionEnabledTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_miniVirtualSessionEnabledExpected;
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
				CConfigSenOption2Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption2Field &configField(varConfigField);

				// Test.
				const bool miniVirtualSessionEnabledResult = configField.MiniVirtualSessionEnabled();

				{
					// Report #1.
					bool areEqual = miniVirtualSessionEnabledResult == TEST_DATA[testIndex].m_miniVirtualSessionEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED miniVirtualSessionEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)miniVirtualSessionEnabledResult,
						(int)TEST_DATA[testIndex].m_miniVirtualSessionEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption2Field::SetSenOptions()
		///     CConfigSenOption2Field::AccruePointsForNCEP()
		/// </summary>
		TEST(CConfigSenOption2FieldTest, AccruePointsForNCEPTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_accruePointsForNCEPExpected;
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
				CConfigSenOption2Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption2Field &configField(varConfigField);

				// Test.
				const bool accruePointsForNCEPResult = configField.AccruePointsForNCEP();

				{
					// Report #1.
					bool areEqual = accruePointsForNCEPResult == TEST_DATA[testIndex].m_accruePointsForNCEPExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED accruePointsForNCEPResult=%d expected=%d testIndex=%d in %s"),
						(int)accruePointsForNCEPResult,
						(int)TEST_DATA[testIndex].m_accruePointsForNCEPExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption2Field::SetSenOptions()
		///     CConfigSenOption2Field::AccrueCompForNCEP()
		/// </summary>
		TEST(CConfigSenOption2FieldTest, AccrueCompForNCEPTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_accrueCompForNCEPExpected;
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
				CConfigSenOption2Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption2Field &configField(varConfigField);

				// Test.
				const bool accrueCompForNCEPResult = configField.AccrueCompForNCEP();

				{
					// Report #1.
					bool areEqual = accrueCompForNCEPResult == TEST_DATA[testIndex].m_accrueCompForNCEPExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED accrueCompForNCEPResult=%d expected=%d testIndex=%d in %s"),
						(int)accrueCompForNCEPResult,
						(int)TEST_DATA[testIndex].m_accrueCompForNCEPExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption2Field::SetSenOptions()
		///     CConfigSenOption2Field::OneLinkProgressivesEnabled()
		/// </summary>
		TEST(CConfigSenOption2FieldTest, OneLinkProgressivesEnabledTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_oneLinkProgressiveEnabledExpected;
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
				CConfigSenOption2Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption2Field &configField(varConfigField);

				// Test.
				const bool oneLinkProgressiveEnabledResult = configField.OneLinkProgressivesEnabled();

				{
					// Report #1.
					bool areEqual = oneLinkProgressiveEnabledResult == TEST_DATA[testIndex].m_oneLinkProgressiveEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED oneLinkProgressiveEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)oneLinkProgressiveEnabledResult,
						(int)TEST_DATA[testIndex].m_oneLinkProgressiveEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigSenOption2Field::SetSenOptions()
		///     CConfigSenOption2Field::UJPEnabled()
		/// </summary>
		TEST(CConfigSenOption2FieldTest, UJPEnabledTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_ujpEnabled;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x10, 0x00, 0x00 };
			const byte buffer2[] = { 0xff, 0xef, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), false },
				{ buffer2, sizeof(buffer2), true },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption2Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption2Field &configField(varConfigField);

				// Test.
				const bool ujpEnabledResult = configField.UJPEnabled();

				{
					// Report #1.
					bool areEqual = ujpEnabledResult == TEST_DATA[testIndex].m_ujpEnabled;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED ujpEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)ujpEnabledResult,
						(int)TEST_DATA[testIndex].m_ujpEnabled,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

        /// <summary>
		/// Test the implementation of:
		///     CConfigSenOption2Field::SetSenOptions()
		///     CConfigSenOption2Field::MGMDEnabled()
		/// </summary>
		TEST(CConfigSenOption2FieldTest, MGMDEnabledTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_senOption;
				const int m_length;

				// Expected results.
				const bool m_mgmdEnabled;
			} TestDataType;

			const byte buffer0[] = { 0x00, 0x00, 0x00, 0x00 };
			const byte buffer1[] = { 0x00, 0x10, 0x00, 0x00 };
			const byte buffer2[] = { 0xff, 0xef, 0xff, 0xff };
			const byte buffer3[] = { 0xff, 0xff, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ buffer0, sizeof(buffer0), false },
				{ buffer1, sizeof(buffer1), false },
				{ buffer2, sizeof(buffer2), true },
				{ buffer3, sizeof(buffer3), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigSenOption2Field varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSenOptions(TEST_DATA[testIndex].m_senOption, TEST_DATA[testIndex].m_length);
				const CConfigSenOption2Field &configField(varConfigField);

				// Test.
				const bool MGMDEnabledResult = configField.MGMDEnabled();

				{
					// Report #1.
					bool areEqual = MGMDEnabledResult == TEST_DATA[testIndex].m_mgmdEnabled;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED MGMDEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)MGMDEnabledResult,
						(int)TEST_DATA[testIndex].m_mgmdEnabled,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}
}
