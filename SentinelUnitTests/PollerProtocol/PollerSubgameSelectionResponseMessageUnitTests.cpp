#include "stdafx.h"
#include "PollerProtocol/PollerSubgameSelectionResponseMessage.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "UnitTestingUtils.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class PollerSubgameSelectionResponseMessage : public ::testing::Test
	{
	public:

		typedef struct
		{
			DWORD m_id;
			std::string m_displayName;
			std::string m_bucketCurrency;
			DWORD m_countdown;
			DWORD m_multiplier;
			DWORD m_divider;
		}CountdownDataType;

		// Default expected values.
		static const DWORD m_gameSessionId = 0;
		static const DWORD m_playSessionId = 0;
		static const WORD m_multiplier = 0;
		static const WORD m_divider = 0;
		static const AccountingType m_accountingTypeExpected = AccountingType::ACCOUNTING_COINS_IN;
		static const std::vector<CountdownDataType> m_ebCountdownDataExpected;

		void AssertEqual(const SubgameSelectionResponse *obj,
			DWORD gameSessionIdExpected,
			DWORD playSessionIdExpected,
			WORD multiplierExpected,
			WORD dividerExpected,
			DWORD accountingTypeExpected,
			std::vector<CountdownDataType> ebCountdownDataExpected)
		{
			ASSERT_EQ(obj->m_subgameSessionParams.m_gameSessionSequenceNumber, gameSessionIdExpected);
			ASSERT_EQ(obj->m_subgameSessionParams.m_playSessionID, playSessionIdExpected);
			ASSERT_EQ(obj->m_subgameSessionParams.m_multiplier, multiplierExpected);
			ASSERT_EQ(obj->m_subgameSessionParams.m_divider, dividerExpected);
			ASSERT_EQ(obj->m_subgameSessionParams.m_accountingType, accountingTypeExpected);

			ASSERT_EQ(obj->m_ebCountdowns.size(), ebCountdownDataExpected.size());
			for (int i = 0; i < obj->m_ebCountdowns.size(); i++)
			{
				ASSERT_EQ(obj->m_ebCountdowns[i]->id, ebCountdownDataExpected[i].m_id);
				ASSERT_EQ(obj->m_ebCountdowns[i]->displayName, ebCountdownDataExpected[i].m_displayName);
				ASSERT_EQ(obj->m_ebCountdowns[i]->bucketCurrency, ebCountdownDataExpected[i].m_bucketCurrency);
				ASSERT_EQ(obj->m_ebCountdowns[i]->countdown, ebCountdownDataExpected[i].m_countdown);
				ASSERT_EQ(obj->m_ebCountdowns[i]->multiplier, ebCountdownDataExpected[i].m_multiplier);
				ASSERT_EQ(obj->m_ebCountdowns[i]->divider, ebCountdownDataExpected[i].m_divider);
			}
		}
	};

		/// <summary>
		/// Functional test for:
		///     CPollerSubgameSelectionResponseMessage::CPollerSubgameSelectionResponseMessage()
		///     CPollerSubgameSelectionResponseMessage::GetSubgameSelectionResponse()
		/// Code coverage for:
		///     CPollerSubgameSelectionResponseMessage::~CPollerSubgameSelectionResponseMessage()
		TEST_F(PollerSubgameSelectionResponseMessage, PollerSubgameSelectionResponseMessageTest_1)
		{
			static const DWORD RESPONSE_PADDING_SIZE(4); // Bytes of padding on test input.

			typedef struct
			{
				// Input(s).
				const WORD m_packetVersion; // Upgrade NVRAM from this version to the current version.
				const BYTE* const m_response;
				const WORD m_dataLength;

				// Expected result(s).
				const DWORD m_gameSessionIdExpected;
				const DWORD m_playSessionIdExpected;
				const WORD m_multiplierExpected;
				const WORD m_dividerExpected;
				const AccountingType m_accountingTypeExpected;
			} TestDataType;

			const BYTE response1[] = {
				1, 0, 0, 0, // game session id.
				11, 0, 0, 0, // play session id.
				100, 0, // multiplier.
				10, 0, // divider.
				5,// m_accountingType
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const TestDataType TEST_DATA[] =
			{
				{ 14, response1, sizeof(response1) - RESPONSE_PADDING_SIZE,
					1, 11, 100, 10, AccountingType::ACCOUNTING_COINS_OUT,
				},
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CPollerSubgameSelectionResponseMessage varObj(TEST_DATA[testIndex].m_response, TEST_DATA[testIndex].m_packetVersion, TEST_DATA[testIndex].m_dataLength);
				AssertEqual(varObj.GetSubgameSelectionResponse(),
					TEST_DATA[testIndex].m_gameSessionIdExpected,
					TEST_DATA[testIndex].m_playSessionIdExpected,
					TEST_DATA[testIndex].m_multiplierExpected,
					TEST_DATA[testIndex].m_dividerExpected,
					TEST_DATA[testIndex].m_accountingTypeExpected,
					m_ebCountdownDataExpected);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CPollerSubgameSelectionResponseMessage::CPollerSubgameSelectionResponseMessage()
		///     CPollerSubgameSelectionResponseMessage::GetSubgameSelectionResponse()
		/// Code coverage for:
		///     CPollerSubgameSelectionResponseMessage::~CPollerSubgameSelectionResponseMessage()
		// Test for extended buckets countdown data.
		TEST_F(PollerSubgameSelectionResponseMessage, PollerSubgameSelectionResponseMessageTest_2)
		{
			static const DWORD RESPONSE_PADDING_SIZE(4); // Bytes of padding on test input.

			typedef struct
			{
				// Input(s).
				const WORD m_packetVersion; // Upgrade NVRAM from this version to the current version.
				const BYTE* const m_response;
				const WORD m_dataLength;

				// Expected result(s).
				const DWORD m_gameSessionIdExpected;
				const DWORD m_playSessionIdExpected;
				const WORD m_multiplierExpected;
				const WORD m_dividerExpected;
				const AccountingType m_accountingTypeExpected;
				const std::vector<PollerSubgameSelectionResponseMessage::CountdownDataType> m_ebCountdownDataExpected;
			} TestDataType;

			const BYTE response1[] = {
				1, 0, 0, 0, // game session id.
				11, 0, 0, 0, // play session id.
				100, 0, // multiplier.
				10, 0, // divider.
				5,// m_accountingType
				1, // countdown data count
				101, 0, 0, 0, // countdown data : bucket id
				7, // countdown data : display name length
				'b', 'u', 'c', 'k', 'e', 't', '1', // countdown data : display name
				1, // countdown data : bucket currency length
				'$', // countdown data : bucket currency
				100, 0, 0, 0, // countdown data : countdown
				10, 0, 0, 0, // countdown data : multiplier
				100, 0, 0, 0, // countdown data : divider
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const BYTE response2[] = {
				1, 0, 0, 0, // game session id.
				11, 0, 0, 0, // play session id.
				100, 0, // multiplier.
				10, 0, // divider.
				5,// m_accountingType
				3, // countdown data count
				101, 0, 0, 0, // countdown data : bucket id
				7, // countdown data : display name length
				'b', 'u', 'c', 'k', 'e', 't', '1', // countdown data : display name
				1, // countdown data : bucket currency length
				'$', // countdown data : bucket currency
				100, 0, 0, 0, // countdown data : countdown
				10, 0, 0, 0, // countdown data : multiplier
				100, 0, 0, 0, // countdown data : divider
				102, 0, 0, 0, // countdown data : bucket id
				7, // countdown data : display name length
				'b', 'u', 'c', 'k', 'e', 't', '2', // countdown data : display name
				0, // countdown data : bucket currency length
				200, 0, 0, 0, // countdown data : countdown
				20, 0, 0, 0, // countdown data : multiplier
				200, 0, 0, 0, // countdown data : divider
				103, 0, 0, 0, // countdown data : bucket id
				0, // countdown data : display name length
				0, // countdown data : bucket currency length
				150, 0, 0, 0, // countdown data : countdown
				15, 0, 0, 0, // countdown data : multiplier
				150, 0, 0, 0, // countdown data : divider
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const TestDataType TEST_DATA[] =
			{
				{ 15, response1, sizeof(response1) - RESPONSE_PADDING_SIZE,
					1, 11, 100, 10, AccountingType::ACCOUNTING_COINS_OUT,
					{
						{101, "bucket1", "$", 100, 10, 100},
					},
				},
				{ 15, response2, sizeof(response2) - RESPONSE_PADDING_SIZE,
					1, 11, 100, 10, AccountingType::ACCOUNTING_COINS_OUT,
					{
						{101, "bucket1", "$", 100, 10, 100},
						{102, "bucket2", "", 200, 20, 200},
						{103, "", "", 150, 15, 150},
					},
				},
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CPollerSubgameSelectionResponseMessage varObj(TEST_DATA[testIndex].m_response, TEST_DATA[testIndex].m_packetVersion, TEST_DATA[testIndex].m_dataLength);
				SubgameSelectionResponse *response = varObj.GetSubgameSelectionResponse();
				AssertEqual(response,
					TEST_DATA[testIndex].m_gameSessionIdExpected,
					TEST_DATA[testIndex].m_playSessionIdExpected,
					TEST_DATA[testIndex].m_multiplierExpected,
					TEST_DATA[testIndex].m_dividerExpected,
					TEST_DATA[testIndex].m_accountingTypeExpected,
					TEST_DATA[testIndex].m_ebCountdownDataExpected);

				// Cleanup.
				delete response;
			}
		}

		const std::vector<PollerSubgameSelectionResponseMessage::CountdownDataType> PollerSubgameSelectionResponseMessage::m_ebCountdownDataExpected;
}
