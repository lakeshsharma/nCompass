#include "stdafx.h"
#include "ConfigTimezoneField.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigTimezoneField : public ::testing::Test
	{
	public:
	};

		/// <summary>
		/// Test the implementation of:
		///     CConfigTimezoneField::CConfigTimezoneField().
		/// </summary>
		TEST(ConfigTimezoneField, ConfigTimezoneFieldTest)
		{
			// Setup.
			const CConfigTimezoneField configField;
			const FieldTypes typeExpected = TIMEZONE;

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
		///     CConfigTimezoneField::SetTimezoneOptions(),
		///     CConfigTimezoneField::GetTimezoneOffset(), and
		///     CConfigTimezoneField::UseDST().
		/// </summary>
		TEST(ConfigTimezoneField, SetTimezoneOptionsTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_buffer;
				const int m_length;

				// Expected results.
				const short m_timezoneOffsetExpected;
				const bool m_useDSTExpected;
			} TestDataType;

			const byte buffer1[] = { 0 };
			const byte buffer2[] = { 1 };
			const byte buffer3[] = { 0xff };
			const byte buffer4[] = { 0xfe };
			const byte buffer5[] = { 0, 0 };
			const byte buffer6[] = { 0, 1 };
			const byte buffer7[] = { 1, 0 };
			const byte buffer8[] = { 1, 1 };
			const byte buffer9[] = { 0xff, 0x7f };
			const byte buffer10[] = { 0x00, 0x80 };
			const byte buffer11[] = { 0x01, 0x80 };

			const byte buffer12[] = { 0xfe, 0xfe };
			const byte buffer13[] = { 0xfe, 0xff };
			const byte buffer14[] = { 0xff, 0xfe };
			const byte buffer15[] = { 0xff, 0xff };
			const byte buffer16[] = { 0x5c, 0xfe, 0 };
			const byte buffer17[] = { 0x5c, 0xfe, 1 };
			const byte buffer18[] = { 0x5c, 0xfe, 2 };
			const byte buffer19[] = { 0x5c, 0xfe, 0xff };
			const byte buffer20[] = { 0x5c, 0xfe, 0, 0x11 };
			const byte buffer21[] = { 0x5c, 0xfe, 1, 0x22 };
			const byte buffer22[] = { 0x5c, 0xfe, 2, 0x33 };
			const byte buffer23[] = { 0x5c, 0xfe, 0xff, 0x44 };

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, false },
				{ buffer1, sizeof(buffer1), 0, false },
				{ buffer2, sizeof(buffer2), 0, false },
				{ buffer3, sizeof(buffer3), 0, false },
				{ buffer4, sizeof(buffer4), 0, false },
				{ buffer5, sizeof(buffer5), 0, false },
				{ buffer6, sizeof(buffer6), 256, false },
				{ buffer7, sizeof(buffer7), 1, false },
				{ buffer8, sizeof(buffer8), 257, false },
				{ buffer9, sizeof(buffer9), 32767, false },
				{ buffer10, sizeof(buffer10), -32768, false },
				{ buffer11, sizeof(buffer11), -32767, false },
				{ buffer12, sizeof(buffer12), -258, false },
				{ buffer13, sizeof(buffer13), -2, false },
				{ buffer14, sizeof(buffer14), -257, false },
				{ buffer15, sizeof(buffer15), -1, false },
				{ buffer16, sizeof(buffer16), -420, false },
				{ buffer17, sizeof(buffer17), -420, true },
				{ buffer18, sizeof(buffer18), -420, true },
				{ buffer19, sizeof(buffer19), -420, true },
				{ buffer20, sizeof(buffer20), -420, false },
				{ buffer21, sizeof(buffer21), -420, true },
				{ buffer22, sizeof(buffer22), -420, true },
				{ buffer23, sizeof(buffer23), -420, true },
				{ NULL, 33, 0, false },
				{ NULL, -33, 0, false },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigTimezoneField varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetTimezoneOptions(TEST_DATA[testIndex].m_buffer, TEST_DATA[testIndex].m_length);
				const CConfigTimezoneField &configField(varConfigField);

				// Test #1.
				const short timezoneOffsetResult = configField.GetTimezoneOffset();

				{
					// Report #1.
					bool areEqual = timezoneOffsetResult == TEST_DATA[testIndex].m_timezoneOffsetExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED timezoneOffsetResult=%d expected=%d testIndex=%d in %s"),
						(int)timezoneOffsetResult,
						(int)TEST_DATA[testIndex].m_timezoneOffsetExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.
				const bool useDSTResult = configField.UseDST();

				{
					// Report #2.
					bool areEqual = useDSTResult == TEST_DATA[testIndex].m_useDSTExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED useDSTResult=%u expected=%u testIndex=%d in %s"),
						(int)useDSTResult,
						(int)TEST_DATA[testIndex].m_useDSTExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}
}
