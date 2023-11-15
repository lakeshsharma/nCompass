#include "stdafx.h"
#include "ConfigPointsForPromoField.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigPointsForPromoField : public ::testing::Test
	{
	public:
	};

		/// <summary>
		/// Test the implementation of:
		///     CConfigTimezoneField::CConfigTimezoneField().
		/// </summary>
		TEST(ConfigPointsForPromoField, CConfigPointsForPromoFieldTest)
		{
			// Setup.
			const CConfigPointsForPromoField configField;
			const FieldTypes typeExpected = POINTS_FOR_PROMO;

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
		///     CConfigTimezoneField::SetAccruePointsForPromo(),
		///     CConfigTimezoneField::GetAccruePointsForNCEP(), and
		///     CConfigTimezoneField::GetAccrueCompForNCEP().
		/// </summary>
		TEST(ConfigPointsForPromoField, SetAccruePointsForPromoTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_buffer;
				const int m_length;

				// Expected results.
				const bool m_accruePointsForNCEPExpected;
				const bool m_accrueCompForNCEPExpected;
			} TestDataType;

			const byte buffer1[] = { 0 };
			const byte buffer2[] = { 1 };
			const byte buffer3[] = { 2 };
			const byte buffer4[] = { 0xfe };
			const byte buffer5[] = { 0xff };

			const byte buffer6[] = { 0xff, 0 };
			const byte buffer7[] = { 0, 1 };
			const byte buffer8[] = { 1, 2 };
			const byte buffer9[] = { 2, 0xfe };
			const byte buffer10[] = { 0xfe, 0xff };

			const byte buffer11[] = { 0xfe, 0, 1 };
			const byte buffer12[] = { 0xff, 1, 2 };
			const byte buffer13[] = { 0, 2, 0xfe };
			const byte buffer14[] = { 1, 0xfe, 0xff };
			const byte buffer15[] = { 2, 0xf, 0 };

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, false },
				{ buffer1, sizeof(buffer1), false, false },
				{ buffer2, sizeof(buffer2), true, false },
				{ buffer3, sizeof(buffer3), false, false },
				{ buffer4, sizeof(buffer4), false, false },
				{ buffer5, sizeof(buffer5), false, false },
				{ buffer6, sizeof(buffer6), false, false },
				{ buffer7, sizeof(buffer7), false, true },
				{ buffer8, sizeof(buffer8), true, false },
				{ buffer9, sizeof(buffer9), false, false },
				{ buffer10, sizeof(buffer10), false, false },
				{ buffer11, sizeof(buffer11), false, false },
				{ buffer12, sizeof(buffer12), false, true },
				{ buffer13, sizeof(buffer13), false, false },
				{ buffer14, sizeof(buffer14), true, false },
				{ buffer15, sizeof(buffer15), false, false },
				{ NULL, 33, false, false },
				{ NULL, -33, false, false },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigPointsForPromoField varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetAccruePointsForPromo(TEST_DATA[testIndex].m_buffer, TEST_DATA[testIndex].m_length);
				const CConfigPointsForPromoField &configField(varConfigField);

				// Test #1.
				const bool accruePointsForNCEPResult = configField.GetAccruePointsForNCEP();

				{
					// Report #2.
					bool areEqual = accruePointsForNCEPResult == TEST_DATA[testIndex].m_accruePointsForNCEPExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED accruePointsForNCEPResult=%u expected=%u testIndex=%d in %s"),
						(int)accruePointsForNCEPResult,
						(int)TEST_DATA[testIndex].m_accruePointsForNCEPExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.
				const bool accrueCompForNCEPResult = configField.GetAccrueCompForNCEP();

				{
					// Report #2.
					bool areEqual = accrueCompForNCEPResult == TEST_DATA[testIndex].m_accrueCompForNCEPExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED accrueCompForNCEPResult=%u expected=%u testIndex=%d in %s"),
						(int)accrueCompForNCEPResult,
						(int)TEST_DATA[testIndex].m_accrueCompForNCEPExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}
}
