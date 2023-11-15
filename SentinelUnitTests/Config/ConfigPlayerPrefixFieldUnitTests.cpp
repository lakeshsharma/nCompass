#include "stdafx.h"
#include "ConfigPlayerPrefixField.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	static const byte MEMCMP_EQUAL = 0; // Return value for memcmp() indicating equality.

	class ConfigPlayerPrefixField : public ::testing::Test
	{
	public:
	};

		/// <summary>
		/// Test the implementation of:
		///     CConfigPlayerPrefixField::CConfigPlayerPrefixField().
		/// </summary>
		TEST(ConfigPlayerPrefixField, CConfigUniverseCardPrefixFieldTest)
		{
			// Setup.
			const CConfigPlayerPrefixField configField;
			const FieldTypes typeExpected = PLAYER_PREFIXES;

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
		/// <summary>
		/// Test the implementation of:
		///     CConfigUniverseCardPrefixField::SetPlayerPrefixes(),
		///     CConfigUniverseCardPrefixField::GetPlayerPrefixes(), and
		///     CConfigUniverseCardPrefixField::GetPrefixCount().
		/// </summary>
		TEST(ConfigPlayerPrefixField, SetPlayerPrefixesTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_prefixes;
				const int m_size;

				// Expected results.
				const DWORD m_cardPrefixCountExpected;
				const byte *m_cardPrefixesExpected;
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
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42
			};
			const byte buffer10[] = {
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43
			};
			// Test maximum size+1.
			const byte buffer11[] = {
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44
			};

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, 0, NULL },
				{ buffer1, sizeof(buffer1), sizeof(buffer1), buffer1 },
				{ buffer2, sizeof(buffer2), sizeof(buffer2), buffer2 },
				{ buffer3, sizeof(buffer3), sizeof(buffer3), buffer3 },
				{ buffer4, sizeof(buffer4), sizeof(buffer4), buffer4 },
				{ buffer5, sizeof(buffer5), sizeof(buffer5), buffer5 },
				{ buffer6, sizeof(buffer6), sizeof(buffer6), buffer6 },
				{ buffer7, sizeof(buffer7), sizeof(buffer7), buffer7 },
				{ buffer8, sizeof(buffer8), sizeof(buffer8), buffer8 },
				{ buffer9, sizeof(buffer9), sizeof(buffer9), buffer9 },
				{ buffer10, sizeof(buffer10), sizeof(buffer10), buffer10 },
				{ buffer11, sizeof(buffer11), SIZE_OF_VALID_CARDS_PREFIX, buffer11 },
				{ NULL, sizeof(buffer11), 0, NULL },
				{ NULL, -33, 0, NULL },
			};

			const int TEST_DATA_MEMCMP_EXPECTED = MEMCMP_EQUAL;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigPlayerPrefixField varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetPlayerPrefixes(TEST_DATA[testIndex].m_prefixes, TEST_DATA[testIndex].m_size);
				const CConfigPlayerPrefixField &configField(varConfigField);

				// Test #1.
				const int cardPrefixCountResult = configField.GetPrefixCount();

				// Assert that Test #1 is correct BEFORE performing Test #2.
				{
					// Report #1.
					bool areEqual = cardPrefixCountResult == TEST_DATA[testIndex].m_cardPrefixCountExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED countResult=%d expected=%d testIndex=%d in %s"),
						(int)cardPrefixCountResult,
						(int)TEST_DATA[testIndex].m_cardPrefixCountExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #3.
				const byte *cardPrefixesResult = configField.GetPlayerPrefixes();

				{
					// Report #2.
					string failedMsg;
					failedMsg = FormatString(_T("FAILED cardPrefixesResult=%p expected=<NOT NULL> testIndex=%d in %s"),
						(const void *)cardPrefixesResult,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE((void *)nullptr, (void *)cardPrefixesResult) << failedMsg;
				}

				// Test #3.
				const bool propCodesMemCmpResult = memcmp(cardPrefixesResult,
					TEST_DATA[testIndex].m_cardPrefixesExpected,
					cardPrefixCountResult);

				{
					// Report #3.
					bool areEqual = propCodesMemCmpResult == TEST_DATA_MEMCMP_EXPECTED;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED memcmp testIndex=%d in %s"),
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

        #endif//TODO-PORT
}
