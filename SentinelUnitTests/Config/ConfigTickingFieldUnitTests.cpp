#include "stdafx.h"
#include "ConfigTicketingField.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigTicketingField : public ::testing::Test
	{
	public:
	};

		/// <summary>
		/// Test the implementation of:
		///     CConfigTicketingField::CConfigTicketingField().
		/// </summary>
		TEST(ConfigTicketingField, ConfigTicketingFieldTest)
		{
			// Setup.
			const CConfigTicketingField configField;
			const FieldTypes typeExpected = TICKETING;

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
		///     CConfigTicketingField::SetTicketingOptions(),
		///     CConfigTicketingField::GetTicketExpirationType(),
		///     CConfigTicketingField::GetTicketExpirationDuration(),
		///     CConfigTicketingField::TicketingEnabled(),
		///     CConfigTicketingField::SystemGeneratedTicketValidationNumbers(), and
		///     CConfigTicketingField::TicketBufferingEnabled().
		/// </summary>
		TEST(ConfigTicketingField, SetTicketingOptionsTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_buffer;
				const int m_length;

				// Expected results.
				const TicketExpirationType m_ticketExpirationTypeExpected;
				const WORD m_ticketExpirationDurationExpected;
				const short m_ticketingEnabledExpected;
				const short m_systemGeneratedTicketValidationNumbersExpected;
				const bool m_ticketBufferingEnabledExpected;
			} TestDataType;

			// Test buffer lengths of 1.
			const byte buffer1[] = { 0 };
			const byte buffer2[] = { 1 };
			const byte buffer3[] = { 2 };
			const byte buffer4[] = { 3 };
			const byte buffer5[] = { 4 };
			const byte buffer6[] = { 5 };
			const byte buffer7[] = { 6 };
			const byte buffer8[] = { 0x7f };
			const byte buffer9[] = { 0x80 };
			const byte buffer10[] = { 0xfe };
			const byte buffer11[] = { 0xff };

			// Test buffer lengths of 2.
			const byte buffer12[] = { 7, 0 };
			const byte buffer13[] = { 8, 1 };
			const byte buffer14[] = { 9, 2 };
			const byte buffer15[] = { 10, 0x7f };
			const byte buffer16[] = { 11, 0x80 };
			const byte buffer17[] = { 12, 0xfd };
			const byte buffer18[] = { 13, 0xfe };
			const byte buffer19[] = { 14, 0xff };

			// Test buffer lengths of 3.
			const byte buffer20[] = { 15, 3, 0 };
			const byte buffer21[] = { 16, 4, 1 };
			const byte buffer22[] = { 17, 5, 2 };
			const byte buffer23[] = { 18, 6, 0x7f };
			const byte buffer24[] = { 19, 7, 0x80 };
			const byte buffer25[] = { 20, 8, 0xfd };
			const byte buffer26[] = { 21, 9, 0xfe };
			const byte buffer27[] = { 22, 10, 0xff };

			// Test buffer lengths of 4.
			const byte buffer28[] = { 23, 11, 3, 0 };
			const byte buffer29[] = { 24, 12, 4, 1 };
			const byte buffer30[] = { 25, 13, 5, 2 };
			const byte buffer31[] = { 26, 14, 6, 0x7f };
			const byte buffer32[] = { 27, 15, 7, 0x80 };
			const byte buffer33[] = { 28, 16, 8, 0xfd };
			const byte buffer34[] = { 29, 17, 9, 0xfe };
			const byte buffer35[] = { 30, 18, 10, 0xff };

			// Test buffer lengths of 5.
			const byte buffer36[] = { 31, 19, 11, 3, 0 };
			const byte buffer37[] = { 32, 20, 12, 4, 1 };
			const byte buffer38[] = { 33, 21, 13, 5, 2 };
			const byte buffer39[] = { 34, 22, 14, 6, 0x7f };
			const byte buffer40[] = { 35, 23, 15, 7, 0x80 };
			const byte buffer41[] = { 36, 24, 16, 8, 0xfd };
			const byte buffer42[] = { 37, 25, 17, 9, 0xfe };
			const byte buffer43[] = { 38, 26, 18, 10, 0xff };

			// Test buffer lengths of 6.
			const byte buffer44[] = { 39, 27, 0, 0, 0, 0 };
			const byte buffer45[] = { 40, 28, 0, 0, 1, 1 };
			const byte buffer46[] = { 41, 29, 0, 1, 0, 2 };
			const byte buffer47[] = { 42, 30, 0, 1, 1, 3 };
			const byte buffer48[] = { 43, 31, 1, 0, 0, 4 };
			const byte buffer49[] = { 44, 32, 1, 0, 1, 5 };
			const byte buffer50[] = { 45, 33, 1, 1, 0, 6 };
			const byte buffer51[] = { 46, 34, 1, 1, 1, 7 };

			// Test buffer lengths of 7.
			const byte buffer52[] = { 47, 0, 0, 0, 0, 0, 0 };
			const byte buffer53[] = { 48, 1, 0, 0, 0, 1, 1 };
			const byte buffer54[] = { 49, 0, 1, 0, 1, 0, 2 };
			const byte buffer55[] = { 50, 0x0e, 0x27, 0, 1, 1, 3 };
			const byte buffer56[] = { 51, 0x0f, 0x27, 1, 0, 0, 4 };
			const byte buffer57[] = { 52, 0x10, 0x27, 1, 0, 1, 5 };
			const byte buffer58[] = { 53, 0xff, 0xfe, 1, 1, 0, 6 };
			const byte buffer59[] = { 54, 0xfe, 0xff, 1, 1, 1, 7 };
			const byte buffer60[] = { 55, 0xff, 0xff, 0, 0, 0, 8 };

			// Test buffer lengths of 5 with all 8 permutations for the 3 boolean values.
			const byte buffer61[] = { 56, 35, 0, 0, 0 };
			const byte buffer62[] = { 57, 36, 0, 0, 1 };
			const byte buffer63[] = { 58, 37, 0, 1, 0 };
			const byte buffer64[] = { 59, 38, 0, 1, 1 };
			const byte buffer65[] = { 60, 39, 1, 0, 0 };
			const byte buffer66[] = { 61, 40, 1, 0, 1 };
			const byte buffer67[] = { 62, 41, 1, 1, 0 };
			const byte buffer68[] = { 63, 42, 1, 1, 1 };

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, UNUSED, 0, false, false, false },
				{ buffer1, sizeof(buffer1), UNUSED, 0, false, false, false },
				{ buffer2, sizeof(buffer2), EXPIRES_SECONDS, 0, false, false, false },
				{ buffer3, sizeof(buffer3), EXPIRES_MINUTES, 0, false, false, false },
				{ buffer4, sizeof(buffer4), EXPIRES_HOURS, 0, false, false, false },
				{ buffer5, sizeof(buffer5), EXPIRES_DAYS, 0, false, false, false },
				{ buffer6, sizeof(buffer6), EXPIRES_YEARS, 0, false, false, false },
				{ buffer7, sizeof(buffer7), (TicketExpirationType)6, 0, false, false, false },
				{ buffer8, sizeof(buffer8), (TicketExpirationType)0x7f, 0, false, false, false },
				{ buffer9, sizeof(buffer9), (TicketExpirationType)0x80, 0, false, false, false },
				{ buffer10, sizeof(buffer10), (TicketExpirationType)0xfe, 0, false, false, false },
				{ buffer11, sizeof(buffer11), (TicketExpirationType)0xff, 0, false, false, false },

				{ buffer12, sizeof(buffer12), (TicketExpirationType)7, 0, false, false, false },
				{ buffer13, sizeof(buffer13), (TicketExpirationType)8, 0, false, false, false },
				{ buffer14, sizeof(buffer14), (TicketExpirationType)9, 0, false, false, false },
				{ buffer15, sizeof(buffer15), (TicketExpirationType)10, 0, false, false, false },
				{ buffer16, sizeof(buffer16), (TicketExpirationType)11, 0, false, false, false },
				{ buffer17, sizeof(buffer17), (TicketExpirationType)12, 0, false, false, false },
				{ buffer18, sizeof(buffer18), (TicketExpirationType)13, 0, false, false, false },
				{ buffer19, sizeof(buffer19), (TicketExpirationType)14, 0, false, false, false },

				{ buffer20, sizeof(buffer20), (TicketExpirationType)15, 3, false, false, false },
				{ buffer21, sizeof(buffer21), (TicketExpirationType)16, 260, false, false, false },
				{ buffer22, sizeof(buffer22), (TicketExpirationType)17, 517, false, false, false },
				{ buffer23, sizeof(buffer23), (TicketExpirationType)18, 32518, false, false, false },
				{ buffer24, sizeof(buffer24), (TicketExpirationType)19, 32775, false, false, false },
				{ buffer25, sizeof(buffer25), (TicketExpirationType)20, 64776, false, false, false },
				{ buffer26, sizeof(buffer26), (TicketExpirationType)21, 65033, false, false, false },
				{ buffer27, sizeof(buffer27), (TicketExpirationType)22, 65290, false, false, false },

				{ buffer28, sizeof(buffer28), (TicketExpirationType)23, 779, false, false, false },
				{ buffer29, sizeof(buffer29), (TicketExpirationType)24, 1036, true, false, false },
				{ buffer30, sizeof(buffer30), (TicketExpirationType)25, 1293, true, false, false },
				{ buffer31, sizeof(buffer31), (TicketExpirationType)26, 1550, true, false, false },
				{ buffer32, sizeof(buffer32), (TicketExpirationType)27, 1807, true, false, false },
				{ buffer33, sizeof(buffer33), (TicketExpirationType)28, 2064, true, false, false },
				{ buffer34, sizeof(buffer34), (TicketExpirationType)29, 2321, true, false, false },
				{ buffer35, sizeof(buffer35), (TicketExpirationType)30, 2578, true, false, false },

				{ buffer36, sizeof(buffer36), (TicketExpirationType)31, 19, true, true, false },
				{ buffer37, sizeof(buffer37), (TicketExpirationType)32, 20, true, true, true },
				{ buffer38, sizeof(buffer38), (TicketExpirationType)33, 21, true, true, true },
				{ buffer39, sizeof(buffer39), (TicketExpirationType)34, 22, true, true, true },
				{ buffer40, sizeof(buffer40), (TicketExpirationType)35, 23, true, true, true },
				{ buffer41, sizeof(buffer41), (TicketExpirationType)36, 24, true, true, true },
				{ buffer42, sizeof(buffer42), (TicketExpirationType)37, 25, true, true, true },
				{ buffer43, sizeof(buffer43), (TicketExpirationType)38, 26, true, true, true },

				{ buffer44, sizeof(buffer44), (TicketExpirationType)39, 27, false, false, false },
				{ buffer45, sizeof(buffer45), (TicketExpirationType)40, 28, false, true, true },
				{ buffer46, sizeof(buffer46), (TicketExpirationType)41, 29, true, false, true },
				{ buffer47, sizeof(buffer47), (TicketExpirationType)42, 30, true, true, true },
				{ buffer48, sizeof(buffer48), (TicketExpirationType)43, 287, false, false, true },
				{ buffer49, sizeof(buffer49), (TicketExpirationType)44, 288, false, true, true },
				{ buffer50, sizeof(buffer50), (TicketExpirationType)45, 289, true, false, true },
				{ buffer51, sizeof(buffer51), (TicketExpirationType)46, 290, true, true, true },

				{ buffer52, sizeof(buffer52), (TicketExpirationType)47, 0, false, false, false },
				{ buffer53, sizeof(buffer53), (TicketExpirationType)48, 1, false, false, true },
				{ buffer54, sizeof(buffer54), (TicketExpirationType)49, 256, false, true, false },
				{ buffer55, sizeof(buffer55), (TicketExpirationType)50, 9998, false, true, true },
				{ buffer56, sizeof(buffer56), (TicketExpirationType)51, 9999, true, false, false },
				{ buffer57, sizeof(buffer57), (TicketExpirationType)52, 10000, true, false, true },
				{ buffer58, sizeof(buffer58), (TicketExpirationType)53, 0xfeff, true, true, false },
				{ buffer59, sizeof(buffer59), (TicketExpirationType)54, 0xfffe, true, true, true },
				{ buffer60, sizeof(buffer60), (TicketExpirationType)55, 0xffff, false, false, false },

				{ buffer61, sizeof(buffer61), (TicketExpirationType)56, 35, false, false, false },
				{ buffer62, sizeof(buffer62), (TicketExpirationType)57, 36, false, false, true },
				{ buffer63, sizeof(buffer63), (TicketExpirationType)58, 37, false, true, false },
				{ buffer64, sizeof(buffer64), (TicketExpirationType)59, 38, false, true, true },
				{ buffer65, sizeof(buffer65), (TicketExpirationType)60, 39, true, false, false },
				{ buffer66, sizeof(buffer66), (TicketExpirationType)61, 40, true, false, true },
				{ buffer67, sizeof(buffer67), (TicketExpirationType)62, 41, true, true, false },
				{ buffer68, sizeof(buffer68), (TicketExpirationType)63, 42, true, true, true },

				{ NULL, 33, (const TicketExpirationType)UNUSED, 0, false, false, false },
				{ NULL, -33, (const TicketExpirationType)UNUSED, 0, false, false, false },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CConfigTicketingField varConfigField;

				// Perform operation(s) to be tested.
				varConfigField.SetTicketingOptions(TEST_DATA[testIndex].m_buffer, TEST_DATA[testIndex].m_length);
				const CConfigTicketingField &configField(varConfigField);

				// Test.
				const TicketExpirationType ticketExpirationTypeResult = configField.GetTicketExpirationType();
				const WORD ticketExpirationDurationResult = configField.GetTicketExpirationDuration();
				const short ticketingEnabledResult = configField.TicketingEnabled();
				const short systemGeneratedTicketValidationNumbersResult = configField.SystemGeneratedTicketValidationNumbers();
				const bool ticketBufferingEnabledResult = configField.TicketBufferingEnabled();

				{
					// Report #1.
					bool areEqual = ticketExpirationTypeResult == TEST_DATA[testIndex].m_ticketExpirationTypeExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED SetTicketingOptions() ticketExpirationTypeResult=%d expected=%d testIndex=%d in %s"),
						(int)ticketExpirationTypeResult,
						(int)TEST_DATA[testIndex].m_ticketExpirationTypeExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				{
					// Report #2.
					bool areEqual = ticketExpirationDurationResult == TEST_DATA[testIndex].m_ticketExpirationDurationExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED SetTicketingOptions() ticketExpirationDurationResult=%u expected=%u testIndex=%d in %s"),
						(int)ticketExpirationDurationResult,
						(int)TEST_DATA[testIndex].m_ticketExpirationDurationExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				{
					// Report #3.
					bool areEqual = ticketingEnabledResult == TEST_DATA[testIndex].m_ticketingEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED SetTicketingOptions() ticketingEnabledResult=%u expected=%u testIndex=%d in %s"),
						(int)ticketingEnabledResult,
						(int)TEST_DATA[testIndex].m_ticketingEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				{
					// Report #4.
					bool areEqual = systemGeneratedTicketValidationNumbersResult == TEST_DATA[testIndex].m_systemGeneratedTicketValidationNumbersExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED SetTicketingOptions() systemGeneratedTicketValidationNumbersResult=%u expected=%u testIndex=%d in %s"),
						(int)systemGeneratedTicketValidationNumbersResult,
						(int)TEST_DATA[testIndex].m_systemGeneratedTicketValidationNumbersExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				{
					// Report #5.
					bool areEqual = ticketBufferingEnabledResult == TEST_DATA[testIndex].m_ticketBufferingEnabledExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED SetTicketingOptions() ticketBufferingEnabledResult=%u expected=%u testIndex=%d in %s"),
						(int)ticketBufferingEnabledResult,
						(int)TEST_DATA[testIndex].m_ticketBufferingEnabledExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}
}
