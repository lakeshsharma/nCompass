#include "stdafx.h"
#include "ConfigSlotNumberField.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigSlotNumberField : public ::testing::Test
	{
	public:
	};

		/// <summary>
		/// Test the implementation of:
		///     CConfigSlotNumberField::CConfigSlotNumberField().
		/// </summary>
		TEST(ConfigSlotNumberField, ConfigSlotNumberFieldTest)
		{
			// Setup.
			const CConfigSlotNumberField configField;
			const FieldTypes typeExpected = SLOT_NUMBER;

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
		///     CConfigSlotNumberField::SetSlotNumber(), and
		///     CConfigSlotNumberField::GetSlotNumber().
		/// </summary>
		TEST(ConfigSlotNumberField, SetSlotNumberTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_slotNumber;
				const int m_length;

				// Expected results.
				const DWORD m_slotNumberExpected;
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
				CConfigSlotNumberField varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetSlotNumber(TEST_DATA[testIndex].m_slotNumber, TEST_DATA[testIndex].m_length);
				const CConfigSlotNumberField &configField(varConfigField);

				// Test.
				const DWORD slotNumberResult = configField.GetSlotNumber();

				{
					// Report #1.
					bool areEqual = slotNumberResult == TEST_DATA[testIndex].m_slotNumberExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED SetSlotNumber() slotNumberResult=%u expected=%u testIndex=%d in %s"),
						(unsigned)slotNumberResult,
						(unsigned)TEST_DATA[testIndex].m_slotNumberExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}
}
