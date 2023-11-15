#include "stdafx.h"
#include "ConfigPlayerTrackingField.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigPlayerTrackingField : public ::testing::Test
	{
	public:
	};

		/// <summary>
		/// Test the implementation of:
		///     CConfigPlayerTrackingField::CConfigPlayerTrackingField().
		/// </summary>
		TEST(ConfigPlayerTrackingField, CConfigPlayerTrackingFieldTest)
		{
			// Setup.
			const CConfigPlayerTrackingField configField;
			const FieldTypes typeExpected = PLAYER_TRACKING;

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
		///     CConfigPlayerTrackingField::SetPlayerTrackingOptions(),
		///     CConfigPlayerTrackingField::GetPlayerTrackingMultiplier(), and
		///     CConfigPlayerTrackingField::GetPlayerTrackingDivisor().
		/// </summary>
		TEST(ConfigPlayerTrackingField, SetPlayerTrackingOptionsTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_buffer;
				const int m_length;

				// Expected results.
				const WORD m_playerTrackingMultiplierExpected;
				const WORD m_playerTrackingDivisorExpected;
			} TestDataType;

			const byte buffer1[] = { 0 };
			const byte buffer2[] = { 1 };
			const byte buffer3[] = { 0xfe };
			const byte buffer4[] = { 0xff };

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

			const byte buffer16[] = { 0x4b, 0xdc, 0 };
			const byte buffer17[] = { 0x4b, 0xdc, 1 };
			const byte buffer18[] = { 0x4b, 0xdc, 0xfe };
			const byte buffer19[] = { 0x4b, 0xdc, 0xff };

			const byte buffer20[] = { 0x5c, 0xed, 0, 0 };
			const byte buffer21[] = { 0x5c, 0xed, 0, 1 };
			const byte buffer22[] = { 0x5c, 0xed, 1, 0 };
			const byte buffer23[] = { 0x5c, 0xed, 1, 1 };
			const byte buffer24[] = { 0x5c, 0xed, 0xff, 0x7f };
			const byte buffer25[] = { 0x5c, 0xed, 0x00, 0x80 };
			const byte buffer26[] = { 0x5c, 0xed, 0x01, 0x80 };
			const byte buffer27[] = { 0x5c, 0xed, 0xfe, 0xfe };
			const byte buffer28[] = { 0x5c, 0xed, 0xfe, 0xff };
			const byte buffer29[] = { 0x5c, 0xed, 0xff, 0xfe };
			const byte buffer30[] = { 0x5c, 0xed, 0xff, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, false },
				{ buffer1, sizeof(buffer1), 0, 0 },
				{ buffer2, sizeof(buffer2), 0, 0 },
				{ buffer3, sizeof(buffer3), 0, 0 },
				{ buffer4, sizeof(buffer4), 0, 0 },

				{ buffer5, sizeof(buffer5), 0, 0 },
				{ buffer6, sizeof(buffer6), 0x100, 0 },
				{ buffer7, sizeof(buffer7), 1, 0 },
				{ buffer8, sizeof(buffer8), 0x101, 0 },
				{ buffer9, sizeof(buffer9), 0x7fff, 0 },
				{ buffer10, sizeof(buffer10), 0x8000, 0 },
				{ buffer11, sizeof(buffer11), 0x8001, 0 },
				{ buffer12, sizeof(buffer12), 0xfefe, 0 },
				{ buffer13, sizeof(buffer13), 0xfffe, 0 },
				{ buffer14, sizeof(buffer14), 0xfeff, 0 },
				{ buffer15, sizeof(buffer15), 0xffff, 0 },

				{ buffer16, sizeof(buffer16), 0xdc4b, 0 },
				{ buffer17, sizeof(buffer17), 0xdc4b, 0 },
				{ buffer18, sizeof(buffer18), 0xdc4b, 0 },
				{ buffer19, sizeof(buffer19), 0xdc4b, 0 },

				{ buffer20, sizeof(buffer20), 0xed5c, 0 },
				{ buffer21, sizeof(buffer21), 0xed5c, 0x100 },
				{ buffer22, sizeof(buffer22), 0xed5c, 1 },
				{ buffer23, sizeof(buffer23), 0xed5c, 0x101 },
				{ buffer24, sizeof(buffer24), 0xed5c, 0x7fff },
				{ buffer25, sizeof(buffer25), 0xed5c, 0x8000 },
				{ buffer26, sizeof(buffer26), 0xed5c, 0x8001 },
				{ buffer27, sizeof(buffer27), 0xed5c, 0xfefe },
				{ buffer28, sizeof(buffer28), 0xed5c, 0xfffe },
				{ buffer29, sizeof(buffer29), 0xed5c, 0xfeff },
				{ buffer30, sizeof(buffer30), 0xed5c, 0xffff },

				{ NULL, 33, 0, false },
				{ NULL, -33, 0, false },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigPlayerTrackingField varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetPlayerTrackingOptions(TEST_DATA[testIndex].m_buffer, TEST_DATA[testIndex].m_length);
				const CConfigPlayerTrackingField &configField(varConfigField);

				// Test #1.
				{
					const WORD playerTrackingMultiplierResult = configField.GetPlayerTrackingMultiplier();

					bool areEqual = playerTrackingMultiplierResult == TEST_DATA[testIndex].m_playerTrackingMultiplierExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED playerTrackingMultiplierResult=%d expected=%d testIndex=%d in %s"),
						(unsigned)playerTrackingMultiplierResult,
						(unsigned)TEST_DATA[testIndex].m_playerTrackingMultiplierExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.
				{
					const WORD playerTrackingDivisorResult = configField.GetPlayerTrackingDivisor();

					bool areEqual = playerTrackingDivisorResult == TEST_DATA[testIndex].m_playerTrackingDivisorExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED playerTrackingDivisorResult=%u expected=%u testIndex=%d in %s"),
						(unsigned)playerTrackingDivisorResult,
						(unsigned)TEST_DATA[testIndex].m_playerTrackingDivisorExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}
}
