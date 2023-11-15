#include "stdafx.h"
#include "ConfigPBTField.h"

#include "GlobalDefs.h"
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigPBTField : public ::testing::Test
	{
	public:
	};

		/// <summary>
		/// Test the implementation of:
		///     CConfigPBTField::CConfigPBTField().
		/// </summary>
		TEST(ConfigPBTField, CConfigPBTFieldTest)
		{
			// Setup.
			const CConfigPBTField configField;
			const FieldTypes typeExpected = PBT;

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
		///     CConfigPBTField::SetPBTOptions(),
		///     CConfigPBTField::PromoEnabled(),
		///     CConfigPBTField::PointsEnabled(), and
		///     CConfigPBTField::CashEnabled().
		/// </summary>
		TEST(ConfigPBTField, SetPBTOptionsTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_buffer;
				const int m_length;

				// Expected results.
				const bool m_pbtEnabledExpected;
				const bool m_pointsEnabledExpected;
				const bool m_cashEnabledExpected;
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

			const byte buffer11[] = { 0xfe, 0xff, 0 };
			const byte buffer12[] = { 0xff, 0, 1 };
			const byte buffer13[] = { 0, 1, 2 };
			const byte buffer14[] = { 1, 2, 0xfe };
			const byte buffer15[] = { 2, 0xfe, 0xff };

			const byte buffer16[] = { 2, 0xfe, 0xff, 0 };
			const byte buffer17[] = { 0xfe, 0xff, 0, 1 };
			const byte buffer18[] = { 0xff, 0, 1, 2 };
			const byte buffer19[] = { 0, 1, 2, 0xfe };
			const byte buffer20[] = { 1, 2, 0xfe, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, false, false },
				{ buffer1, sizeof(buffer1), false, false, false },
				{ buffer2, sizeof(buffer2), false, false, false },
				{ buffer3, sizeof(buffer3), false, false, false },
				{ buffer4, sizeof(buffer4), false, false, false },
				{ buffer5, sizeof(buffer5), false, false, false },

				{ buffer6, sizeof(buffer6), false, false, false },
				{ buffer7, sizeof(buffer7), false, false, false },
				{ buffer8, sizeof(buffer8), false, false, false },
				{ buffer9, sizeof(buffer9), false, false, false },
				{ buffer10, sizeof(buffer10), false, false, false },

				{ buffer11, sizeof(buffer11), false, false, false },
				{ buffer12, sizeof(buffer12), false, false, true },
				{ buffer13, sizeof(buffer13), false, true, false },
				{ buffer14, sizeof(buffer14), true, false, false },
				{ buffer15, sizeof(buffer15), false, false, false },

				{ buffer16, sizeof(buffer16), false, false, false },
				{ buffer17, sizeof(buffer17), false, false, false },
				{ buffer18, sizeof(buffer18), false, false, true },
				{ buffer19, sizeof(buffer19), false, true, false },
				{ buffer20, sizeof(buffer20), true, false, false },

				{ buffer20, 0, false, false, false },
				{ buffer20, -33, false, false, false },
				{ NULL, 33, false, false, false },
				{ NULL, -33, false, false, false },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigPBTField varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetPBTOptions(TEST_DATA[testIndex].m_buffer, TEST_DATA[testIndex].m_length);
				const CConfigPBTField &configField(varConfigField);

				// Test #1.
				{
					const bool pbtEnabledResult = configField.PromoEnabled();

					bool areEqual = pbtEnabledResult == TEST_DATA[testIndex].m_pbtEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pbtEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)pbtEnabledResult,
						(int)TEST_DATA[testIndex].m_pbtEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.
				{
					const bool pointsEnabledResult = configField.PointsEnabled();

					bool areEqual = pointsEnabledResult == TEST_DATA[testIndex].m_pointsEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)pointsEnabledResult,
						(int)TEST_DATA[testIndex].m_pointsEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #3.
				{
					const bool cashEnabledResult = configField.CashEnabled();

					bool areEqual = cashEnabledResult == TEST_DATA[testIndex].m_cashEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED cashEnabledResult=%d expected=%d testIndex=%d in %s"),
						(int)cashEnabledResult,
						(int)TEST_DATA[testIndex].m_cashEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CConfigPBTField::SetPBTOptionsBuffer(),
		/// </summary>
		TEST(ConfigPBTField, SetPBTOptionsBufferTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_buffer;
				const size_t m_bufferSize;
				const int m_length;
				const bool m_pbtEnabled;
				const bool m_pointsEnabled;
				const bool m_cashEnabled;

				// Expected results.
				const byte *m_bufferExpected;
			} TestDataType;

			const byte buffer1[] = { 0xcd };
			const byte buffer2[] = { 0xcd, 0xcd };

			const byte buffer3[] = { 0xcd, 0xcd, 0xcd };
			const byte buffer3a[] = { 0, 0, 0 };
			const byte buffer3b[] = { 0, 0, 1 };
			const byte buffer3c[] = { 0, 1, 0 };
			const byte buffer3d[] = { 0, 1, 1 };
			const byte buffer3e[] = { 1, 0, 0 };
			const byte buffer3f[] = { 1, 0, 1 };
			const byte buffer3g[] = { 1, 1, 0 };
			const byte buffer3h[] = { 1, 1, 1 };

			const byte buffer4[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte buffer4a[] = { 0, 0, 0, 0xcd };
			const byte buffer4b[] = { 0, 0, 1, 0xcd };
			const byte buffer4c[] = { 0, 1, 0, 0xcd };
			const byte buffer4d[] = { 0, 1, 1, 0xcd };
			const byte buffer4e[] = { 1, 0, 0, 0xcd };
			const byte buffer4f[] = { 1, 0, 1, 0xcd };
			const byte buffer4g[] = { 1, 1, 0, 0xcd };
			const byte buffer4h[] = { 1, 1, 1, 0xcd };

			const byte buffer5[] = { 0xcd, 0xcd, 0xcd, 0xcd, 0xcd };
			const byte buffer5a[] = { 0, 0, 0, 0xcd, 0xcd };
			const byte buffer5b[] = { 0, 0, 1, 0xcd, 0xcd };
			const byte buffer5c[] = { 0, 1, 0, 0xcd, 0xcd };
			const byte buffer5d[] = { 0, 1, 1, 0xcd, 0xcd };
			const byte buffer5e[] = { 1, 0, 0, 0xcd, 0xcd };
			const byte buffer5f[] = { 1, 0, 1, 0xcd, 0xcd };
			const byte buffer5g[] = { 1, 1, 0, 0xcd, 0xcd };
			const byte buffer5h[] = { 1, 1, 1, 0xcd, 0xcd };

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, 0, false, false, false, NULL },

				{ buffer1, sizeof(buffer1), 0, true, true, true, buffer1 },
				{ buffer1, sizeof(buffer1), sizeof(buffer1), false, false, false, buffer1 },
				{ buffer1, sizeof(buffer1), -33, true, true, true, buffer1 },

				{ buffer2, sizeof(buffer2), 0, true, true, true, buffer2 },
				{ buffer2, sizeof(buffer2), sizeof(buffer2), false, false, false, buffer2 },
				{ buffer2, sizeof(buffer2), -33, true, true, true, buffer2 },

				{ buffer3, sizeof(buffer3), 0, false, false, false, buffer3 },
				{ buffer3, sizeof(buffer3), sizeof(buffer3), false, false, false, buffer3a },
				{ buffer3, sizeof(buffer3), sizeof(buffer3), false, false, true, buffer3b },
				{ buffer3, sizeof(buffer3), sizeof(buffer3), false, true, false, buffer3c },
				{ buffer3, sizeof(buffer3), sizeof(buffer3), false, true, true, buffer3d },
				{ buffer3, sizeof(buffer3), sizeof(buffer3), true, false, false, buffer3e },
				{ buffer3, sizeof(buffer3), sizeof(buffer3), true, false, true, buffer3f },
				{ buffer3, sizeof(buffer3), sizeof(buffer3), true, true, false, buffer3g },
				{ buffer3, sizeof(buffer3), sizeof(buffer3), true, true, true, buffer3h },
				{ buffer3, sizeof(buffer3), -33, true, true, true, buffer3 },

				{ buffer4, sizeof(buffer4), 0, false, false, false, buffer4 },
				{ buffer4, sizeof(buffer4), sizeof(buffer4), false, false, false, buffer4a },
				{ buffer4, sizeof(buffer4), sizeof(buffer4), false, false, true, buffer4b },
				{ buffer4, sizeof(buffer4), sizeof(buffer4), false, true, false, buffer4c },
				{ buffer4, sizeof(buffer4), sizeof(buffer4), false, true, true, buffer4d },
				{ buffer4, sizeof(buffer4), sizeof(buffer4), true, false, false, buffer4e },
				{ buffer4, sizeof(buffer4), sizeof(buffer4), true, false, true, buffer4f },
				{ buffer4, sizeof(buffer4), sizeof(buffer4), true, true, false, buffer4g },
				{ buffer4, sizeof(buffer4), sizeof(buffer4), true, true, true, buffer4h },
				{ buffer4, sizeof(buffer4), -33, true, true, true, buffer4 },

				{ buffer5, sizeof(buffer5), 0, false, false, false, buffer5 },
				{ buffer5, sizeof(buffer5), sizeof(buffer5), false, false, false, buffer5a },
				{ buffer5, sizeof(buffer5), sizeof(buffer5), false, false, true, buffer5b },
				{ buffer5, sizeof(buffer5), sizeof(buffer5), false, true, false, buffer5c },
				{ buffer5, sizeof(buffer5), sizeof(buffer5), false, true, true, buffer5d },
				{ buffer5, sizeof(buffer5), sizeof(buffer5), true, false, false, buffer5e },
				{ buffer5, sizeof(buffer5), sizeof(buffer5), true, false, true, buffer5f },
				{ buffer5, sizeof(buffer5), sizeof(buffer5), true, true, false, buffer5g },
				{ buffer5, sizeof(buffer5), sizeof(buffer5), true, true, true, buffer5h },
				{ buffer5, sizeof(buffer5), -33, true, true, true, buffer5 },

				{ NULL, 0, 33, false, false, false, NULL },
				{ NULL, 0, -33, false, false, false, NULL },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigPBTField varConfigField;
				byte *buffer = NULL;
				if (TEST_DATA[testIndex].m_buffer != NULL)
				{
					buffer = new byte[TEST_DATA[testIndex].m_bufferSize];
					memcpy(buffer, TEST_DATA[testIndex].m_buffer, TEST_DATA[testIndex].m_bufferSize);
				}

				// Perform operation(s) to be tested.
				varConfigField.SetPBTOptionsBuffer(buffer,
					TEST_DATA[testIndex].m_length,
					TEST_DATA[testIndex].m_pbtEnabled,
					TEST_DATA[testIndex].m_pointsEnabled,
					TEST_DATA[testIndex].m_cashEnabled);

				// Test #1.
				{
					const int memcmpResult = memcmp(buffer, TEST_DATA[testIndex].m_bufferExpected, TEST_DATA[testIndex].m_bufferSize);
					string bufferStr = CUtilities::ToHexString(buffer, TEST_DATA[testIndex].m_bufferSize);
					string bufferExpectedStr = CUtilities::ToHexString(TEST_DATA[testIndex].m_bufferExpected, TEST_DATA[testIndex].m_bufferSize);

					bool areEqual = memcmpResult == MEMCMP_EQUAL;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED memcmpResult=%d expected=%d buffer={%s} bufferExpected=[%s] testIndex=%d in %s"),
						(int)memcmpResult,
						(int)MEMCMP_EQUAL,
						static_cast<LPCTSTR>(bufferStr.c_str()),
						static_cast<LPCTSTR>(bufferExpectedStr.c_str()),
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
				delete[] buffer;
			}
		}
}
