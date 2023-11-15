#include "stdafx.h"
#include "ConfigUniversePropCodesField.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigUniversePropCodesField : public ::testing::Test
	{
	public:
	};

		// ConfigUniversePropCodesFieldTest
		// Test the implementation of:
		//     CConfigUniversePropCodesField::CConfigUniversePropCodesField().
		TEST(ConfigUniversePropCodesField, ConfigUniversePropCodesFieldTest)
		{
			// Setup.
			const CConfigUniversePropCodesField configField;
			const FieldTypes typeExpected = UNIVERSE_PROP_CODES;

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

        #if 000//TODO-PORT
		// SetUniversePropCodesTest
		// Test the implementation of:
		//     CConfigUniversePropCodesField::SetUniversePropCodes().
		//     CConfigUniversePropCodesField::GetUniversePropCodes().
		//     CConfigUniversePropCodesField::GetUniversePropCodeCount().
		TEST(ConfigUniversePropCodesField, SetUniversePropCodesTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_propCodes;
				const int m_size;

				// Expected results.
				const DWORD m_propCodeCountExpected;
				const byte *m_propCodesExpected;
				const bool m_propCodesMemCmpExpected;
			} TestDataType;

			const byte buffer1[] = { 0, };
			const byte buffer2[] = { 0, 1};
			const byte buffer3[] = { 0, 1, 2 };
			const byte buffer4[] = { 0, 0, 1, 2 };
			const byte buffer5[] = { 0xff };
			const byte buffer6[] = { 0xff, 0xfe };
			const byte buffer7[] = { 0xff, 0xfe, 0xfd };
			const byte buffer8[] = { 0xff, 0xfe, 0xfd, 0xfc };
			const byte buffer9[] = {
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
				51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 70, 71, 72,
			};
			const byte buffer10[] = {
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
				51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 70, 71, 72, 73
			};
			// Test maximum size+1.
			const byte buffer11[] = {
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
				51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 70, 71, 72, 73, 74
			};

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, 0, NULL, MEMCMP_EQUAL },
				{ buffer1, sizeof(buffer1), sizeof(buffer1) / SIZE_OF_VALID_PROP_CODE, buffer1, MEMCMP_EQUAL },
				{ buffer2, sizeof(buffer2), sizeof(buffer2) / SIZE_OF_VALID_PROP_CODE, buffer2, MEMCMP_EQUAL },
				{ buffer3, sizeof(buffer3), sizeof(buffer3) / SIZE_OF_VALID_PROP_CODE, buffer3, MEMCMP_EQUAL },
				{ buffer4, sizeof(buffer4), sizeof(buffer4) / SIZE_OF_VALID_PROP_CODE, buffer4, MEMCMP_EQUAL },
				{ buffer5, sizeof(buffer5), sizeof(buffer5) / SIZE_OF_VALID_PROP_CODE, buffer5, MEMCMP_EQUAL },
				{ buffer6, sizeof(buffer6), sizeof(buffer6) / SIZE_OF_VALID_PROP_CODE, buffer6, MEMCMP_EQUAL },
				{ buffer7, sizeof(buffer7), sizeof(buffer7) / SIZE_OF_VALID_PROP_CODE, buffer7, MEMCMP_EQUAL },
				{ buffer8, sizeof(buffer8), sizeof(buffer8) / SIZE_OF_VALID_PROP_CODE, buffer8, MEMCMP_EQUAL },
				{ buffer9, sizeof(buffer9), sizeof(buffer9) / SIZE_OF_VALID_PROP_CODE, buffer9, MEMCMP_EQUAL },
				{ buffer10, sizeof(buffer10), sizeof(buffer10) / SIZE_OF_VALID_PROP_CODE, buffer10, MEMCMP_EQUAL },
				{ buffer11, sizeof(buffer11), SIZE_OF_VALID_CARDS_PREFIX, buffer11, MEMCMP_EQUAL },
				{ NULL, sizeof(buffer11), 0, NULL, MEMCMP_EQUAL },
				{ NULL, -33, 0, NULL, MEMCMP_EQUAL },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigUniversePropCodesField varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetUniversePropCodes(TEST_DATA[testIndex].m_propCodes, TEST_DATA[testIndex].m_size);
				const CConfigUniversePropCodesField &configField(varConfigField);

				// Test #1.
				const int propCodeCountResult = configField.GetUniversePropCodeCount();

				// Assert that Test #1 is correct BEFORE performing Test #2.
				{
					// Report #1.
					bool areEqual = propCodeCountResult == TEST_DATA[testIndex].m_propCodeCountExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED SetUniversePropCodes() countResult=%d expected=%d testIndex=%d in %s"),
						(int)propCodeCountResult,
						(int)TEST_DATA[testIndex].m_propCodeCountExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2.
				const byte *propCodesResult = configField.GetUniversePropCodes();
				const bool propCodesMemCmpResult = memcmp(propCodesResult,
					TEST_DATA[testIndex].m_propCodesExpected,
					propCodeCountResult * SIZE_OF_VALID_PROP_CODE);

				{
					// Report #2.
					bool areEqual = propCodesMemCmpResult == TEST_DATA[testIndex].m_propCodesMemCmpExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED SetUniversePropCodes() memcmpResult=%d expected=%d testIndex=%d in %s"),
						(int)propCodesMemCmpResult,
						(int)TEST_DATA[testIndex].m_propCodesMemCmpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

#endif//TODO-PORT
}
