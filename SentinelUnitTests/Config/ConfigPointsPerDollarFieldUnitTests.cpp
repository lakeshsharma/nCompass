#include "stdafx.h"
#include "ConfigPointsPerDollarField.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigPointsPerDollarField : public ::testing::Test
	{
	public:
	};

		/// <summary>
		/// Test the implementation of:
		///     CConfigPointsPerDollarField::CConfigPointsPerDollarField().
		/// </summary>
		TEST(ConfigPointsPerDollarField, CConfigPointsPerDollarFieldTest)
		{
			// Setup.
			const CConfigPointsPerDollarField configField;
			const FieldTypes typeExpected = POINTS_PER_DOLLAR;

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
		///     CConfigPointsPerDollarField::SetPointsPerDollar()
		///     CConfigPointsPerDollarField::GetPointsPerDollar()
		/// </summary>
		TEST(ConfigPointsPerDollarField, SetPointsPerDollarTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_buffer;
				const int m_length;

				// Expected results.
				const DWORD m_pointsPerDollarTestExpected;
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
				{ buffer17, sizeof(buffer17), 0x0201 },
				{ buffer18, sizeof(buffer18), 0xffff },
				{ buffer19, sizeof(buffer19), 0x0000 },
				{ buffer20, sizeof(buffer20), 0xfdfe },
				{ buffer21, sizeof(buffer21), 0xfffe },
				{ buffer22, sizeof(buffer22), 0xffff },

				{ NULL, 33, 0, },
				{ NULL, -33, 0, },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigPointsPerDollarField varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetPointsPerDollar(TEST_DATA[testIndex].m_buffer, TEST_DATA[testIndex].m_length);
				const CConfigPointsPerDollarField &configField(varConfigField);

				// Test.
				const WORD pointsPerDollarResult = configField.GetPointsPerDollar();

				{
					// Report #1.
					bool areEqual = pointsPerDollarResult == TEST_DATA[testIndex].m_pointsPerDollarTestExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsPerDollarResult=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsPerDollarResult,
						(unsigned)TEST_DATA[testIndex].m_pointsPerDollarTestExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}
}
