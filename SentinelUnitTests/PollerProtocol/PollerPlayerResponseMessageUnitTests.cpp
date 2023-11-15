#include "stdafx.h"
#include "PollerProtocol/PollerPlayerResponseMessage.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "UnitTestingUtils.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class PollerPlayerResponseMessage : public ::testing::Test
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
		static const WORD m_lossLimitBalanceExpected = 0;
		static const BYTE m_lossLimitStatusExpected = 0;
		static const DWORD m_dailyPointsExpected = (DWORD)-1;
		static const byte m_maxCardLimitIndicatorExpected = 0;
		static const DWORD m_pointsTillNextTierExpected = (DWORD)-1;
		static const DWORD m_tierMultiplierExpected = 0;
		static const DWORD m_rankIdExpected = (DWORD)-1;
		static const byte m_rankNameLengthExpected = 0;
		static const byte * const m_rankNameExpected;
		static const DWORD m_playerIdExpected = (DWORD)-1;
		static const DWORD m_tierPointsExpected = (DWORD)-1;
		static const DWORD m_countdownHighPrecisionExpected = (DWORD)-1;
		static const AccountingType m_accountingTypeExpected = AccountingType::ACCOUNTING_COINS_IN;
		static const DWORD m_ebPointsIdExpected = 0;
		static const DWORD m_ebCompsIdExpected = 0;
		static const DWORD m_ebPromosIdExpected = 0;
		static const DWORD m_ebTierPointsIdExpected = 0;
		static const bool m_ebIndicatorExpected = 0;
		static constexpr const char *m_ebJSONDataExpected = "";
		static const std::vector<CountdownDataType> m_ebCountdownDataExpected;

		void AssertEqual(const PlayerResponse &obj,
			const byte card_idExpected[CARD_ID_LEN],
			WORD multiplierExpected,
			WORD dividerExpected,
			bool birthdayIndicatorExpected,
			bool bannedFromPlayIndicatorExpected,
			bool anniversaryIndicatorExpected,
			DWORD totalPointsExpected,
			WORD countdownExpected,
			DWORD compsExpected,
			byte nameLengthExpected,
			const byte* nameExpected,
			byte respLengthExpected,
			const byte* responseExpected,
			WORD lossLimitBalanceExpected,
			BYTE lossLimitStatusExpected,
			DWORD dailyPointsExpected,
			byte maxCardLimitIndicatorExpected,
			DWORD pointsTillNextTierExpected,
			DWORD tierMultiplierExpected,
			DWORD rankIdExpected,
			byte rankNameLengthExpected,
			const byte* rankNameExpected,
			DWORD playerIdExpected,
			DWORD tierPointsExpected,
			DWORD countdownHighPrecisionExpected,
			DWORD accountingTypeExpected,
			DWORD ebPointsIdExpected,
			DWORD ebCompsIdExpected,
			DWORD ebPromosIdExpected,
			DWORD ebTierPointsIdExpected,
			bool ebIndicatorExpected,
			std::string ebJSONDataExpected,
			std::vector<CountdownDataType> ebCountdownDataExpected,
			int testIndex,
			LPCTSTR functionName,
			int lineNumber,
			const int* extraIndex = NULL)
		{
			CUnitTestingUtils::AssertEqual(obj.card_id, CARD_ID_LEN, card_idExpected, CARD_ID_LEN,
				"obj.card_id", &testIndex, functionName, lineNumber, extraIndex);
			ASSERT_EQ(obj.multiplier, multiplierExpected);
			ASSERT_EQ(obj.divider, dividerExpected);
			ASSERT_EQ(obj.birthdayIndicator, birthdayIndicatorExpected);
			ASSERT_EQ(obj.bannedFromPlayIndicator, bannedFromPlayIndicatorExpected);
			ASSERT_EQ(obj.anniversaryIndicator, anniversaryIndicatorExpected);
			ASSERT_EQ(obj.totalPoints, totalPointsExpected);
			ASSERT_EQ(obj.countdown, countdownExpected);
			ASSERT_EQ(obj.comps, compsExpected);

			ASSERT_EQ(obj.nameLength, nameLengthExpected);
			if (nameLengthExpected > 1)
			{
				// Verify trailing NUL character.
				CUnitTestingUtils::AssertEqual(obj.name, obj.nameLength + 1, nameExpected, nameLengthExpected + 1,
					"obj.name", &testIndex, functionName, lineNumber, extraIndex);
			}

			ASSERT_EQ(obj.respLength, respLengthExpected);
			if (nameLengthExpected > 1)
			{
				// Verify trailing NUL character.
				CUnitTestingUtils::AssertEqual(obj.response, obj.respLength + 1, responseExpected, respLengthExpected + 1,
					"obj.response", &testIndex, functionName, lineNumber, extraIndex);
			}

			ASSERT_EQ(obj.lossLimitBalance, lossLimitBalanceExpected);
			ASSERT_EQ(obj.lossLimitStatus, lossLimitStatusExpected);
			ASSERT_EQ(obj.dailyPoints, dailyPointsExpected);
			ASSERT_EQ(obj.maxCardLimitIndicator, maxCardLimitIndicatorExpected);
			ASSERT_EQ(obj.pointsTillNextTier, pointsTillNextTierExpected);
			ASSERT_EQ(obj.tierMultiplier, tierMultiplierExpected);
			ASSERT_EQ(obj.m_rankId, rankIdExpected);
			ASSERT_EQ(obj.m_rankNameLength, rankNameLengthExpected);
			if (nameLengthExpected > 1)
			{
				// Verify trailing NUL character.
				CUnitTestingUtils::AssertEqual(obj.m_rankName, obj.m_rankNameLength + 1, rankNameExpected, rankNameLengthExpected + 1,
					"obj.m_rankName", &testIndex, functionName, lineNumber, extraIndex);
			}

			ASSERT_EQ(obj.m_playerId, playerIdExpected);
			ASSERT_EQ(obj.m_tierPoints, tierPointsExpected);
			ASSERT_EQ(obj.countdownHighPrecision, countdownHighPrecisionExpected);
			ASSERT_EQ(obj.accountingType, accountingTypeExpected);

			ASSERT_EQ(obj.ebPointsId, ebPointsIdExpected);
			ASSERT_EQ(obj.ebCompsId, ebCompsIdExpected);
			ASSERT_EQ(obj.ebPromosId, ebPromosIdExpected);
			ASSERT_EQ(obj.ebTierPointsId, ebTierPointsIdExpected);

			ASSERT_EQ(obj.ebIndicator, ebIndicatorExpected);
			ASSERT_EQ(obj.ebJSONData, ebJSONDataExpected);

			ASSERT_EQ(obj.ebCountdowns.size(), ebCountdownDataExpected.size());
			for (int i = 0; i < obj.ebCountdowns.size(); i++)
			{
				ASSERT_EQ(obj.ebCountdowns[i]->id, ebCountdownDataExpected[i].m_id);
				ASSERT_EQ(obj.ebCountdowns[i]->displayName, ebCountdownDataExpected[i].m_displayName);
				ASSERT_EQ(obj.ebCountdowns[i]->bucketCurrency, ebCountdownDataExpected[i].m_bucketCurrency);
				ASSERT_EQ(obj.ebCountdowns[i]->countdown, ebCountdownDataExpected[i].m_countdown);
				ASSERT_EQ(obj.ebCountdowns[i]->multiplier, ebCountdownDataExpected[i].m_multiplier);
				ASSERT_EQ(obj.ebCountdowns[i]->divider, ebCountdownDataExpected[i].m_divider);
			}
		}
	};

		/// <summary>
		/// Functional test for:
		///     CPollerPlayerResponseMessage::CPollerPlayerResponseMessage()
		///     CPollerPlayerResponseMessage::GetPlayerResponse()
		/// Code coverage for:
		///     CPollerPlayerResponseMessage::~CPollerPlayerResponseMessage()
		TEST_F(PollerPlayerResponseMessage, CPollerPlayerResponseMessageTest_1)
		{
			static const DWORD RESPONSE_PADDING_SIZE(4); // Bytes of padding on test input.

			typedef struct
			{
				// Input(s).
				const WORD m_packetVersion; // Upgrade NVRAM from this version to the current version.
				const BYTE* const m_response;
				const WORD m_dataLength;

				// Expected result(s).
				const byte m_card_idExpected[CARD_ID_LEN];
				const WORD m_multiplierExpected;
				const WORD m_dividerExpected;
				const bool m_birthdayIndicatorExpected;
				const bool m_bannedFromPlayIndicatorExpected;
				const bool m_anniversaryIndicatorExpected;
				const DWORD m_totalPointsExpected;
				const WORD m_countdownExpected;
				const DWORD m_compsExpected;
				const byte m_nameLengthExpected;
				const byte* const m_nameExpected;
				const byte m_respLengthExpected;
				const byte* const m_responseExpected;
			} TestDataType;

			const BYTE response1[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const TestDataType TEST_DATA[] =
			{
				{ 0, response1, sizeof response1 - RESPONSE_PADDING_SIZE,
					{ 0x12, 0x34, 0x56, 0x78, 0x90 },
					11, 12, true, true, true, 55, 9, 66,
					0, NULL,
					0, NULL,
				},
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CPollerPlayerResponseMessage varObj(TEST_DATA[testIndex].m_response, TEST_DATA[testIndex].m_packetVersion, TEST_DATA[testIndex].m_dataLength);
				AssertEqual(varObj.GetPlayerResponse(),
					TEST_DATA[testIndex].m_card_idExpected,
					TEST_DATA[testIndex].m_multiplierExpected,
					TEST_DATA[testIndex].m_dividerExpected,
					TEST_DATA[testIndex].m_birthdayIndicatorExpected,
					TEST_DATA[testIndex].m_bannedFromPlayIndicatorExpected,
					TEST_DATA[testIndex].m_anniversaryIndicatorExpected,
					TEST_DATA[testIndex].m_totalPointsExpected,
					TEST_DATA[testIndex].m_countdownExpected,
					TEST_DATA[testIndex].m_compsExpected,
					TEST_DATA[testIndex].m_nameLengthExpected,
					TEST_DATA[testIndex].m_nameExpected,
					TEST_DATA[testIndex].m_respLengthExpected,
					TEST_DATA[testIndex].m_responseExpected,
					m_lossLimitBalanceExpected,
					m_lossLimitStatusExpected,
					m_dailyPointsExpected,
					m_maxCardLimitIndicatorExpected,
					m_pointsTillNextTierExpected,
					m_tierMultiplierExpected,
					m_rankIdExpected,
					m_rankNameLengthExpected,
					m_rankNameExpected,
					m_playerIdExpected,
					m_tierPointsExpected,
					m_countdownHighPrecisionExpected,
					m_accountingTypeExpected,
					m_ebPointsIdExpected,
					m_ebCompsIdExpected,
					m_ebPromosIdExpected,
					m_ebTierPointsIdExpected,
					m_ebIndicatorExpected,
					m_ebJSONDataExpected,
					m_ebCountdownDataExpected,
					testIndex, __TFUNCTION__, __LINE__, NULL);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CPollerPlayerResponseMessage::CPollerPlayerResponseMessage()
		///     CPollerPlayerResponseMessage::GetPlayerResponse()
		/// Code coverage for:
		///     CPollerPlayerResponseMessage::~CPollerPlayerResponseMessage()
		TEST_F(PollerPlayerResponseMessage, CPollerPlayerResponseMessageTest_2)
		{
			static const DWORD RESPONSE_PADDING_SIZE(4); // Bytes of padding on test input.

			typedef struct
			{
				// Input(s).
				const WORD m_packetVersion; // Upgrade NVRAM from this version to the current version.
				const BYTE* const m_response;
				const WORD m_dataLength;

				// Expected result(s).
				const byte m_card_idExpected[CARD_ID_LEN];
				const WORD m_multiplierExpected;
				const WORD m_dividerExpected;
				const bool m_birthdayIndicatorExpected;
				const bool m_bannedFromPlayIndicatorExpected;
				const bool m_anniversaryIndicatorExpected;
				const DWORD m_totalPointsExpected;
				const WORD m_countdownExpected;
				const DWORD m_compsExpected;
				const byte m_nameLengthExpected;
				const byte* const m_nameExpected;
				const byte m_respLengthExpected;
				const byte* const m_responseExpected;
				const WORD m_lossLimitBalanceExpected;
				const BYTE m_lossLimitStatusExpected;
				const DWORD m_dailyPointsExpected;
				const byte m_maxCardLimitIndicatorExpected;
				const DWORD m_pointsTillNextTierExpected;
				const DWORD m_tierMultiplierExpected;
			} TestDataType;

			const BYTE response1[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const BYTE response2[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.

			};

			const BYTE response3[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const TestDataType TEST_DATA[] =
			{
				// 2014-06-29 Reads all the standard fields, regardless of the length.
				{ 0, response1, 0,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0, 0,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ WORD_MAX, response1, 0,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0, 0,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},

				{ 0, response1, sizeof(response1 )- RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0, 0,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ WORD_MAX, response1, sizeof(response1) - RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0, 0,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},

				// Response length includes 1 byte of loss limit information.
				{ 0, response2, sizeof(response2) - RESPONSE_PADDING_SIZE - 2,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				m_lossLimitBalanceExpected, m_lossLimitStatusExpected,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ 1, response2, sizeof(response2)-RESPONSE_PADDING_SIZE - 2,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				m_lossLimitBalanceExpected, m_lossLimitStatusExpected,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ 2, response2, sizeof(response2)-RESPONSE_PADDING_SIZE - 2,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ 3, response2, sizeof(response2)-RESPONSE_PADDING_SIZE - 2,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ WORD_MAX, response2, sizeof(response2)-RESPONSE_PADDING_SIZE - 2,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},

				{ 0, response2, sizeof(response2)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				m_lossLimitBalanceExpected, m_lossLimitStatusExpected,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ 1, response2, sizeof(response2)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				m_lossLimitBalanceExpected, m_lossLimitStatusExpected,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ 2, response2, sizeof(response2)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ 3, response2, sizeof(response2)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ WORD_MAX, response2, sizeof(response2)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},

				// Response length includes 1 byte of highlander changes.
				{ 0, response3, sizeof(response3)-RESPONSE_PADDING_SIZE - 12,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				m_lossLimitBalanceExpected, m_lossLimitStatusExpected,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ 1, response3, sizeof(response3)-RESPONSE_PADDING_SIZE - 12,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				m_lossLimitBalanceExpected, m_lossLimitStatusExpected,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ 2, response3, sizeof(response3)-RESPONSE_PADDING_SIZE - 12,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ 3, response3, sizeof(response3)-RESPONSE_PADDING_SIZE - 12,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ 4, response3, sizeof(response3)-RESPONSE_PADDING_SIZE - 12,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				},
				{ 5, response3, sizeof(response3)-RESPONSE_PADDING_SIZE - 12,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				},
				{ WORD_MAX, response3, sizeof(response3)-RESPONSE_PADDING_SIZE - 12,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				},

				{ 0, response3, sizeof(response3)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				m_lossLimitBalanceExpected, m_lossLimitStatusExpected,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ 1, response3, sizeof(response3)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				m_lossLimitBalanceExpected, m_lossLimitStatusExpected,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ 2, response3, sizeof(response3)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ 3, response3, sizeof(response3)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				},
				{ 4, response3, sizeof(response3)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				},
				{ 5, response3, sizeof(response3)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				},
				{ WORD_MAX, response3, sizeof(response3)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				},
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CPollerPlayerResponseMessage varObj(TEST_DATA[testIndex].m_response, TEST_DATA[testIndex].m_packetVersion, TEST_DATA[testIndex].m_dataLength);
				AssertEqual(varObj.GetPlayerResponse(),
					TEST_DATA[testIndex].m_card_idExpected,
					TEST_DATA[testIndex].m_multiplierExpected,
					TEST_DATA[testIndex].m_dividerExpected,
					TEST_DATA[testIndex].m_birthdayIndicatorExpected,
					TEST_DATA[testIndex].m_bannedFromPlayIndicatorExpected,
					TEST_DATA[testIndex].m_anniversaryIndicatorExpected,
					TEST_DATA[testIndex].m_totalPointsExpected,
					TEST_DATA[testIndex].m_countdownExpected,
					TEST_DATA[testIndex].m_compsExpected,
					TEST_DATA[testIndex].m_nameLengthExpected,
					TEST_DATA[testIndex].m_nameExpected,
					TEST_DATA[testIndex].m_respLengthExpected,
					TEST_DATA[testIndex].m_responseExpected,
					TEST_DATA[testIndex].m_lossLimitBalanceExpected,
					TEST_DATA[testIndex].m_lossLimitStatusExpected,
					TEST_DATA[testIndex].m_dailyPointsExpected,
					TEST_DATA[testIndex].m_maxCardLimitIndicatorExpected,
					TEST_DATA[testIndex].m_pointsTillNextTierExpected,
					TEST_DATA[testIndex].m_tierMultiplierExpected,
					m_rankIdExpected,
					m_rankNameLengthExpected,
					m_rankNameExpected,
					m_playerIdExpected,
					m_tierPointsExpected,
					m_countdownHighPrecisionExpected,
					m_accountingTypeExpected,
					m_ebPointsIdExpected,
					m_ebCompsIdExpected,
					m_ebPromosIdExpected,
					m_ebTierPointsIdExpected,
					m_ebIndicatorExpected,
					m_ebJSONDataExpected,
					m_ebCountdownDataExpected,
					testIndex, __TFUNCTION__, __LINE__, NULL);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CPollerPlayerResponseMessage::CPollerPlayerResponseMessage()
		///     CPollerPlayerResponseMessage::GetPlayerResponse()
		/// Code coverage for:
		///     CPollerPlayerResponseMessage::~CPollerPlayerResponseMessage()
		// Test player tier rank fields with a rank name length of zero.
		TEST_F(PollerPlayerResponseMessage, CPollerPlayerResponseMessageTest_3_RankTest)
		{
			static const DWORD RESPONSE_PADDING_SIZE(4); // Bytes of padding on test input.

			typedef struct
			{
				// Input(s).
				const WORD m_packetVersion; // Upgrade NVRAM from this version to the current version.
				const BYTE* const m_response;
				const WORD m_dataLength;

				// Expected result(s).
				const byte m_card_idExpected[CARD_ID_LEN];
				const WORD m_multiplierExpected;
				const WORD m_dividerExpected;
				const bool m_birthdayIndicatorExpected;
				const bool m_bannedFromPlayIndicatorExpected;
				const bool m_anniversaryIndicatorExpected;
				const DWORD m_totalPointsExpected;
				const WORD m_countdownExpected;
				const DWORD m_compsExpected;
				const byte m_nameLengthExpected;
				const byte* const m_nameExpected;
				const byte m_respLengthExpected;
				const byte* const m_responseExpected;
				const WORD m_lossLimitBalanceExpected;
				const BYTE m_lossLimitStatusExpected;
				const DWORD m_dailyPointsExpected;
				const byte m_maxCardLimitIndicatorExpected;
				const DWORD m_pointsTillNextTierExpected;
				const DWORD m_tierMultiplierExpected;
				const DWORD m_rankIdExpected;
				const byte m_rankNameLengthExpected;
				const byte* const m_rankNameExpected;
			} TestDataType;

			const BYTE response4[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				0, // m_rankNameLength.
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const TestDataType TEST_DATA[] =
			{
				// Response length includes 1 byte of m_rankId.
				{ 6, response4, sizeof(response4)-RESPONSE_PADDING_SIZE - 4,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				m_rankIdExpected, 0, NULL,
				},
				// Response length includes 3 bytes of m_rankId.
				{ 6, response4, sizeof(response4)-RESPONSE_PADDING_SIZE - 3,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				m_rankIdExpected, 0, NULL,
				},
				// Response length includes m_rankId, but not m_rankNameLength.
				{ 6, response4, sizeof(response4)-RESPONSE_PADDING_SIZE - 1,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				m_rankIdExpected, 0, NULL,
				},

				{ 0, response4, sizeof(response4)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				m_lossLimitBalanceExpected, m_lossLimitStatusExpected,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				m_rankIdExpected, m_rankNameLengthExpected, m_rankNameExpected,
				},
				{ 1, response4, sizeof(response4)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				m_lossLimitBalanceExpected, m_lossLimitStatusExpected,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				m_rankIdExpected, m_rankNameLengthExpected, m_rankNameExpected,
				},
				{ 2, response4, sizeof(response4)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				m_rankIdExpected, m_rankNameLengthExpected, m_rankNameExpected,
				},
				{ 3, response4, sizeof(response4)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				m_dailyPointsExpected, m_maxCardLimitIndicatorExpected, m_pointsTillNextTierExpected, m_tierMultiplierExpected,
				m_rankIdExpected, m_rankNameLengthExpected, m_rankNameExpected,
				},
				{ 4, response4, sizeof(response4)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				m_rankIdExpected, m_rankNameLengthExpected, m_rankNameExpected,
				},
				{ 5, response4, sizeof(response4)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				m_rankIdExpected, m_rankNameLengthExpected, m_rankNameExpected,
				},
				{ 6, response4, sizeof(response4)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				0x6789abcd, 0, NULL,
				},
				{ 7, response4, sizeof(response4)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				0x6789abcd, 0, NULL,
				},
				{ WORD_MAX, response4, sizeof(response4)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				0x6789abcd, 0, NULL,
				},
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CPollerPlayerResponseMessage varObj(TEST_DATA[testIndex].m_response, TEST_DATA[testIndex].m_packetVersion, TEST_DATA[testIndex].m_dataLength);
				AssertEqual(varObj.GetPlayerResponse(),
					TEST_DATA[testIndex].m_card_idExpected,
					TEST_DATA[testIndex].m_multiplierExpected,
					TEST_DATA[testIndex].m_dividerExpected,
					TEST_DATA[testIndex].m_birthdayIndicatorExpected,
					TEST_DATA[testIndex].m_bannedFromPlayIndicatorExpected,
					TEST_DATA[testIndex].m_anniversaryIndicatorExpected,
					TEST_DATA[testIndex].m_totalPointsExpected,
					TEST_DATA[testIndex].m_countdownExpected,
					TEST_DATA[testIndex].m_compsExpected,
					TEST_DATA[testIndex].m_nameLengthExpected,
					TEST_DATA[testIndex].m_nameExpected,
					TEST_DATA[testIndex].m_respLengthExpected,
					TEST_DATA[testIndex].m_responseExpected,
					TEST_DATA[testIndex].m_lossLimitBalanceExpected,
					TEST_DATA[testIndex].m_lossLimitStatusExpected,
					TEST_DATA[testIndex].m_dailyPointsExpected,
					TEST_DATA[testIndex].m_maxCardLimitIndicatorExpected,
					TEST_DATA[testIndex].m_pointsTillNextTierExpected,
					TEST_DATA[testIndex].m_tierMultiplierExpected,
					TEST_DATA[testIndex].m_rankIdExpected,
					TEST_DATA[testIndex].m_rankNameLengthExpected,
					TEST_DATA[testIndex].m_rankNameExpected,
					m_playerIdExpected,
					m_tierPointsExpected,
					m_countdownHighPrecisionExpected,
					m_accountingTypeExpected,
					m_ebPointsIdExpected,
					m_ebCompsIdExpected,
					m_ebPromosIdExpected,
					m_ebTierPointsIdExpected,
					m_ebIndicatorExpected,
					m_ebJSONDataExpected,
					m_ebCountdownDataExpected,
					testIndex, __TFUNCTION__, __LINE__, NULL);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CPollerPlayerResponseMessage::CPollerPlayerResponseMessage()
		///     CPollerPlayerResponseMessage::GetPlayerResponse()
		/// Code coverage for:
		///     CPollerPlayerResponseMessage::~CPollerPlayerResponseMessage()
		// Test player tier rank fields with a non-zero rank name length.
		TEST_F(PollerPlayerResponseMessage, CPollerPlayerResponseMessageTest_4_RankNameTest)
		{
			static const DWORD RESPONSE_PADDING_SIZE(4); // Bytes of padding on test input.

			typedef struct
			{
				// Input(s).
				const WORD m_packetVersion; // Upgrade NVRAM from this version to the current version.
				const BYTE* const m_response;
				const WORD m_dataLength;

				// Expected result(s).
				const byte m_card_idExpected[CARD_ID_LEN];
				const WORD m_multiplierExpected;
				const WORD m_dividerExpected;
				const bool m_birthdayIndicatorExpected;
				const bool m_bannedFromPlayIndicatorExpected;
				const bool m_anniversaryIndicatorExpected;
				const DWORD m_totalPointsExpected;
				const WORD m_countdownExpected;
				const DWORD m_compsExpected;
				const byte m_nameLengthExpected;
				const byte* const m_nameExpected;
				const byte m_respLengthExpected;
				const byte* const m_responseExpected;
				const WORD m_lossLimitBalanceExpected;
				const BYTE m_lossLimitStatusExpected;
				const DWORD m_dailyPointsExpected;
				const byte m_maxCardLimitIndicatorExpected;
				const DWORD m_pointsTillNextTierExpected;
				const DWORD m_tierMultiplierExpected;
				const DWORD m_rankIdExpected;
				const byte m_rankNameLengthExpected;
				const byte* const m_rankNameExpected;
			} TestDataType;

			const BYTE response5[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				9, // m_rankNameLength.
				'R', 'a', 'n', 'k', ' ', 'N', 'a', 'm', 'e', // Missing m_rankName.
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const BYTE response5a[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				1, // m_rankNameLength.
				// Missing m_rankName.
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const BYTE response5b[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				1, // m_rankNameLength.
				'a', // m_rankName.
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const BYTE response5c[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				3, // m_rankNameLength.
				'a', 0, 'c', // m_rankName with embedded NUL character
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const BYTE response5d[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				14, // m_rankNameLength.
				' ', '#', '1', ' ', 'R', 'a', 'n', 'k', ' ', 'N', 'a', 'm', 'e', ' ',  // m_rankName.
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const BYTE response5e[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				95, // m_rankNameLength.
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, // m_rankName with displayable ASCII characters.
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e,
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const BYTE response5f[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				128, // m_rankNameLength.
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, // m_rankName with all ASCII characters.
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const BYTE response5g[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				255, // m_rankNameLength.
				'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', // m_rankName with 255 characters.
				'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',

				'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',

				'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5',
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const BYTE response5h[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				255, // m_rankNameLength.
				0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, // m_rankName with all 255 non-NUL characters.
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const TestDataType TEST_DATA[] =
			{
				{ 6, response5, sizeof(response5) - RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				0x6789abcd, 9, (byte*)"Rank Name",
				},

				{ 6, response5a, sizeof(response5a)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				m_rankIdExpected, m_rankNameLengthExpected, m_rankNameExpected,
				},

				{ 6, response5b, sizeof(response5b)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				0x6789abcd, 1, (byte*)"a",
				},

				{ 6, response5c, sizeof(response5c)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				0x6789abcd, 3, (byte*)("a\x00" "c"),
				},

				{ 6, response5d, sizeof(response5d)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				0x6789abcd, 14, (byte*)(" #1 Rank Name "),
				},

				{ 6, response5e, sizeof(response5e)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				0x6789abcd, 95, (byte*)" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~",
				},

				{ 6, response5f, sizeof(response5f)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				0x6789abcd, 128,
				(byte*)("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f" // No comma.
					"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f" // No comma.
					" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f"),
				},

				{ 6, response5g, sizeof(response5g)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				0x6789abcd, 255,
				(byte*)("12345678901234567890123456789012345678901234567890" // No comma.
					"12345678901234567890123456789012345678901234567890" // No comma.
					"12345678901234567890123456789012345678901234567890" // No comma.
					"12345678901234567890123456789012345678901234567890" // No comma.
					"12345678901234567890123456789012345678901234567890" // No comma.
					"12345"),
				},

				{ 6, response5h, sizeof(response5h)-RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				0x6789abcd, 255,
				(byte*)("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f" // No comma.
				"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f" // No comma.
				"\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f" // No comma.
				"\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f" // No comma.
				"\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f" // No comma.
				"\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f" // No comma.
				"\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f" // No comma.
				"\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f" // No comma.
				"\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f" // No comma.
				"\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f" // No comma.
				"\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf" // No comma.
				"\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf" // No comma.
				"\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf" // No comma.
				"\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf" // No comma.
				"\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef" // No comma.
				"\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"),
				},
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Perform operation(s) to be tested.
				CPollerPlayerResponseMessage varObj(TEST_DATA[testIndex].m_response, TEST_DATA[testIndex].m_packetVersion, TEST_DATA[testIndex].m_dataLength);
				AssertEqual(varObj.GetPlayerResponse(),
					TEST_DATA[testIndex].m_card_idExpected,
					TEST_DATA[testIndex].m_multiplierExpected,
					TEST_DATA[testIndex].m_dividerExpected,
					TEST_DATA[testIndex].m_birthdayIndicatorExpected,
					TEST_DATA[testIndex].m_bannedFromPlayIndicatorExpected,
					TEST_DATA[testIndex].m_anniversaryIndicatorExpected,
					TEST_DATA[testIndex].m_totalPointsExpected,
					TEST_DATA[testIndex].m_countdownExpected,
					TEST_DATA[testIndex].m_compsExpected,
					TEST_DATA[testIndex].m_nameLengthExpected,
					TEST_DATA[testIndex].m_nameExpected,
					TEST_DATA[testIndex].m_respLengthExpected,
					TEST_DATA[testIndex].m_responseExpected,
					TEST_DATA[testIndex].m_lossLimitBalanceExpected,
					TEST_DATA[testIndex].m_lossLimitStatusExpected,
					TEST_DATA[testIndex].m_dailyPointsExpected,
					TEST_DATA[testIndex].m_maxCardLimitIndicatorExpected,
					TEST_DATA[testIndex].m_pointsTillNextTierExpected,
					TEST_DATA[testIndex].m_tierMultiplierExpected,
					TEST_DATA[testIndex].m_rankIdExpected,
					TEST_DATA[testIndex].m_rankNameLengthExpected,
					TEST_DATA[testIndex].m_rankNameExpected,
					m_playerIdExpected,
					m_tierPointsExpected,
					m_countdownHighPrecisionExpected,
					m_accountingTypeExpected,
					m_ebPointsIdExpected,
					m_ebCompsIdExpected,
					m_ebPromosIdExpected,
					m_ebTierPointsIdExpected,
					m_ebIndicatorExpected,
					m_ebJSONDataExpected,
					m_ebCountdownDataExpected,
					testIndex, __TFUNCTION__, __LINE__, NULL);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CPollerPlayerResponseMessage::CPollerPlayerResponseMessage()
		///     CPollerPlayerResponseMessage::GetPlayerResponse()
		/// Code coverage for:
		///     CPollerPlayerResponseMessage::~CPollerPlayerResponseMessage()
		// Test for playerID, tierPoints, countdownHighPrecision and accountingType fields.
		TEST_F(PollerPlayerResponseMessage, CPollerPlayerResponseMessageTest_5)
		{
			static const DWORD RESPONSE_PADDING_SIZE(4); // Bytes of padding on test input.

			typedef struct
			{
				// Input(s).
				const WORD m_packetVersion; // Upgrade NVRAM from this version to the current version.
				const BYTE* const m_response;
				const WORD m_dataLength;

				// Expected result(s).
				const byte m_card_idExpected[CARD_ID_LEN];
				const WORD m_multiplierExpected;
				const WORD m_dividerExpected;
				const bool m_birthdayIndicatorExpected;
				const bool m_bannedFromPlayIndicatorExpected;
				const bool m_anniversaryIndicatorExpected;
				const DWORD m_totalPointsExpected;
				const WORD m_countdownExpected;
				const DWORD m_compsExpected;
				const byte m_nameLengthExpected;
				const byte* const m_nameExpected;
				const byte m_respLengthExpected;
				const byte* const m_responseExpected;
				const WORD m_lossLimitBalanceExpected;
				const BYTE m_lossLimitStatusExpected;
				const DWORD m_dailyPointsExpected;
				const byte m_maxCardLimitIndicatorExpected;
				const DWORD m_pointsTillNextTierExpected;
				const DWORD m_tierMultiplierExpected;
				const DWORD m_rankIdExpected;
				const byte m_rankNameLengthExpected;
				const byte* const m_rankNameExpected;
				const DWORD m_playerIdExpected;
				const DWORD m_tierPointsExpected;
				const DWORD m_countdownHighPrecisionExpected;
				const AccountingType m_accountingTypeExpected;
			} TestDataType;

			const BYTE response1[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				0, // m_rankNameLength.
				0x12, 0x34, 0x56, 0x78, // m_playerId
				10, 0, 0, 0, // m_tierPoints
				9, 0, 0, 0, // m_countdownHighPrecision
				5,// m_accountingType
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const TestDataType TEST_DATA[] =
			{
				// Response length includes 1 byte of m_rankId.
				{ 14, response1, sizeof(response1) - RESPONSE_PADDING_SIZE,
				{ 0x12, 0x34, 0x56, 0x78, 0x90 },
				11, 12, true, true, true, 55, 9, 66,
				0, NULL,
				0, NULL,
				0x2345, 0x67,
				0x3456789a, 0x78, 0x456789ab, 0x56789abc,
				0x6789abcd,
				0, NULL,
				0x78563412,
				10,	9, AccountingType::ACCOUNTING_COINS_OUT,
				}
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CPollerPlayerResponseMessage varObj(TEST_DATA[testIndex].m_response, TEST_DATA[testIndex].m_packetVersion, TEST_DATA[testIndex].m_dataLength);
				AssertEqual(varObj.GetPlayerResponse(),
					TEST_DATA[testIndex].m_card_idExpected,
					TEST_DATA[testIndex].m_multiplierExpected,
					TEST_DATA[testIndex].m_dividerExpected,
					TEST_DATA[testIndex].m_birthdayIndicatorExpected,
					TEST_DATA[testIndex].m_bannedFromPlayIndicatorExpected,
					TEST_DATA[testIndex].m_anniversaryIndicatorExpected,
					TEST_DATA[testIndex].m_totalPointsExpected,
					TEST_DATA[testIndex].m_countdownExpected,
					TEST_DATA[testIndex].m_compsExpected,
					TEST_DATA[testIndex].m_nameLengthExpected,
					TEST_DATA[testIndex].m_nameExpected,
					TEST_DATA[testIndex].m_respLengthExpected,
					TEST_DATA[testIndex].m_responseExpected,
					TEST_DATA[testIndex].m_lossLimitBalanceExpected,
					TEST_DATA[testIndex].m_lossLimitStatusExpected,
					TEST_DATA[testIndex].m_dailyPointsExpected,
					TEST_DATA[testIndex].m_maxCardLimitIndicatorExpected,
					TEST_DATA[testIndex].m_pointsTillNextTierExpected,
					TEST_DATA[testIndex].m_tierMultiplierExpected,
					TEST_DATA[testIndex].m_rankIdExpected,
					TEST_DATA[testIndex].m_rankNameLengthExpected,
					TEST_DATA[testIndex].m_rankNameExpected,
					TEST_DATA[testIndex].m_playerIdExpected,
					TEST_DATA[testIndex].m_tierPointsExpected,
					TEST_DATA[testIndex].m_countdownHighPrecisionExpected,
					TEST_DATA[testIndex].m_accountingTypeExpected,
					m_ebPointsIdExpected,
					m_ebCompsIdExpected,
					m_ebPromosIdExpected,
					m_ebTierPointsIdExpected,
					m_ebIndicatorExpected,
					m_ebJSONDataExpected,
					m_ebCountdownDataExpected,
					testIndex, __TFUNCTION__, __LINE__, NULL);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CPollerPlayerResponseMessage::CPollerPlayerResponseMessage()
		///     CPollerPlayerResponseMessage::GetPlayerResponse()
		/// Code coverage for:
		///     CPollerPlayerResponseMessage::~CPollerPlayerResponseMessage()
		// Test for extended buckets JSON data related fields.
		TEST_F(PollerPlayerResponseMessage, CPollerPlayerResponseMessageTest_6_EB_JsonData)
		{
			static const DWORD RESPONSE_PADDING_SIZE(4); // Bytes of padding on test input.

			typedef struct
			{
				// Input(s).
				const WORD m_packetVersion; // Upgrade NVRAM from this version to the current version.
				const BYTE* const m_response;
				const WORD m_dataLength;

				// Expected result(s).
				const byte m_card_idExpected[CARD_ID_LEN];
				const WORD m_multiplierExpected;
				const WORD m_dividerExpected;
				const bool m_birthdayIndicatorExpected;
				const bool m_bannedFromPlayIndicatorExpected;
				const bool m_anniversaryIndicatorExpected;
				const DWORD m_totalPointsExpected;
				const WORD m_countdownExpected;
				const DWORD m_compsExpected;
				const byte m_nameLengthExpected;
				const byte* const m_nameExpected;
				const byte m_respLengthExpected;
				const byte* const m_responseExpected;
				const WORD m_lossLimitBalanceExpected;
				const BYTE m_lossLimitStatusExpected;
				const DWORD m_dailyPointsExpected;
				const byte m_maxCardLimitIndicatorExpected;
				const DWORD m_pointsTillNextTierExpected;
				const DWORD m_tierMultiplierExpected;
				const DWORD m_rankIdExpected;
				const byte m_rankNameLengthExpected;
				const byte* const m_rankNameExpected;
				const DWORD m_playerIdExpected;
				const DWORD m_tierPointsExpected;
				const DWORD m_countdownHighPrecisionExpected;
				const AccountingType m_accountingTypeExpected;
				const DWORD m_ebPointsIdExpected;
				const DWORD m_ebCompsIdExpected;
				const DWORD m_ebPromosIdExpected;
				const DWORD m_ebTierPointsIdExpected;
				const bool m_ebIndicatorExpected;
				const std::string m_ebJSONDataExpected;
			} TestDataType;

			const BYTE response1[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				0, // m_rankNameLength.
				0x12, 0x34, 0x56, 0x78, // m_playerId
				10, 0, 0, 0, // m_tierPoints
				9, 0, 0, 0, // m_countdownHighPrecision
				0,// m_accountingType
				101, 0, 0, 0, // m_ebPointsId
				102, 0, 0, 0, // m_ebCompsId
				103, 0, 0, 0, // m_ebPromosId
				104, 0, 0, 0, // m_ebTierPointsId
				0, // countdown data count
				0, 0, 0, 0, // extended bucket data length
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const BYTE response2[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				0, // m_rankNameLength.
				0x12, 0x34, 0x56, 0x78, // m_playerId
				10, 0, 0, 0, // m_tierPoints
				9, 0, 0, 0, // m_countdownHighPrecision
				0,// m_accountingType
				101, 0, 0, 0, // m_ebPointsId
				102, 0, 0, 0, // m_ebCompsId
				103, 0, 0, 0, // m_ebPromosId
				104, 0, 0, 0, // m_ebTierPointsId
				0, // countdown data count
				9, 0, 0, 0, // extended bucket data length
				's', 'o', 'm', 'e', ' ', 'd', 'a', 't', 'a',
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const BYTE response3[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				11, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				0, // m_rankNameLength.
				0x12, 0x34, 0x56, 0x78, // m_playerId
				10, 0, 0, 0, // m_tierPoints
				9, 0, 0, 0, // m_countdownHighPrecision
				0,// m_accountingType
				101, 0, 0, 0, // m_ebPointsId
				102, 0, 0, 0, // m_ebCompsId
				103, 0, 0, 0, // m_ebPromosId
				104, 0, 0, 0, // m_ebTierPointsId
				0, // countdown data count
				0, 0, 0, 0, // extended bucket data length
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const BYTE response4[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				11, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				0, // m_rankNameLength.
				0x12, 0x34, 0x56, 0x78, // m_playerId
				10, 0, 0, 0, // m_tierPoints
				9, 0, 0, 0, // m_countdownHighPrecision
				0,// m_accountingType
				101, 0, 0, 0, // m_ebPointsId
				102, 0, 0, 0, // m_ebCompsId
				103, 0, 0, 0, // m_ebPromosId
				104, 0, 0, 0, // m_ebTierPointsId
				0, // countdown data count
				9, 0, 0, 0, // extended bucket data length
				's', 'o', 'm', 'e', ' ', 'd', 'a', 't', 'a',
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const TestDataType TEST_DATA[] =
			{
				{
					15, response1, sizeof(response1) - RESPONSE_PADDING_SIZE,
					{ 0x12, 0x34, 0x56, 0x78, 0x90 },
					11, 12, true, true, true, 55, 9, 66,
					0, NULL,
					0, NULL,
					0x2345, 0x67,
					0x3456789a, 0x78, 0x456789ab, 0x56789abc,
					0x6789abcd,
					0, NULL,
					0x78563412,
					10,	9, AccountingType::ACCOUNTING_COINS_IN,
					101, 102, 103, 104,
					0,
					"",
				},
				{
					15, response2, sizeof(response2) - RESPONSE_PADDING_SIZE,
					{ 0x12, 0x34, 0x56, 0x78, 0x90 },
					11, 12, true, true, true, 55, 9, 66,
					0, NULL,
					0, NULL,
					0x2345, 0x67,
					0x3456789a, 0x78, 0x456789ab, 0x56789abc,
					0x6789abcd,
					0, NULL,
					0x78563412,
					10,	9, AccountingType::ACCOUNTING_COINS_IN,
					101, 102, 103, 104,
					0,
					"",
				},
				{
					15, response3, sizeof(response3) - RESPONSE_PADDING_SIZE,
					{ 0x12, 0x34, 0x56, 0x78, 0x90 },
					11, 12, true, true, true, 55, 9, 66,
					0, NULL,
					0, NULL,
					0x2345, 0x67,
					0x3456789a, 0x78, 0x456789ab, 0x56789abc,
					0x6789abcd,
					0, NULL,
					0x78563412,
					10,	9, AccountingType::ACCOUNTING_COINS_IN,
					101, 102, 103, 104,
					1,
					"",
				},
				{
					15, response4, sizeof(response4) - RESPONSE_PADDING_SIZE,
					{ 0x12, 0x34, 0x56, 0x78, 0x90 },
					11, 12, true, true, true, 55, 9, 66,
					0, NULL,
					0, NULL,
					0x2345, 0x67,
					0x3456789a, 0x78, 0x456789ab, 0x56789abc,
					0x6789abcd,
					0, NULL,
					0x78563412,
					10,	9, AccountingType::ACCOUNTING_COINS_IN,
					101, 102, 103, 104,
					1,
					"some data",
				}
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CPollerPlayerResponseMessage varObj(TEST_DATA[testIndex].m_response, TEST_DATA[testIndex].m_packetVersion, TEST_DATA[testIndex].m_dataLength);
				AssertEqual(varObj.GetPlayerResponse(),
					TEST_DATA[testIndex].m_card_idExpected,
					TEST_DATA[testIndex].m_multiplierExpected,
					TEST_DATA[testIndex].m_dividerExpected,
					TEST_DATA[testIndex].m_birthdayIndicatorExpected,
					TEST_DATA[testIndex].m_bannedFromPlayIndicatorExpected,
					TEST_DATA[testIndex].m_anniversaryIndicatorExpected,
					TEST_DATA[testIndex].m_totalPointsExpected,
					TEST_DATA[testIndex].m_countdownExpected,
					TEST_DATA[testIndex].m_compsExpected,
					TEST_DATA[testIndex].m_nameLengthExpected,
					TEST_DATA[testIndex].m_nameExpected,
					TEST_DATA[testIndex].m_respLengthExpected,
					TEST_DATA[testIndex].m_responseExpected,
					TEST_DATA[testIndex].m_lossLimitBalanceExpected,
					TEST_DATA[testIndex].m_lossLimitStatusExpected,
					TEST_DATA[testIndex].m_dailyPointsExpected,
					TEST_DATA[testIndex].m_maxCardLimitIndicatorExpected,
					TEST_DATA[testIndex].m_pointsTillNextTierExpected,
					TEST_DATA[testIndex].m_tierMultiplierExpected,
					TEST_DATA[testIndex].m_rankIdExpected,
					TEST_DATA[testIndex].m_rankNameLengthExpected,
					TEST_DATA[testIndex].m_rankNameExpected,
					TEST_DATA[testIndex].m_playerIdExpected,
					TEST_DATA[testIndex].m_tierPointsExpected,
					TEST_DATA[testIndex].m_countdownHighPrecisionExpected,
					TEST_DATA[testIndex].m_accountingTypeExpected,
					TEST_DATA[testIndex].m_ebPointsIdExpected,
					TEST_DATA[testIndex].m_ebCompsIdExpected,
					TEST_DATA[testIndex].m_ebPromosIdExpected,
					TEST_DATA[testIndex].m_ebTierPointsIdExpected,
					TEST_DATA[testIndex].m_ebIndicatorExpected,
					TEST_DATA[testIndex].m_ebJSONDataExpected,
					m_ebCountdownDataExpected,
					testIndex, __TFUNCTION__, __LINE__, NULL);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CPollerPlayerResponseMessage::CPollerPlayerResponseMessage()
		///     CPollerPlayerResponseMessage::GetPlayerResponse()
		/// Code coverage for:
		///     CPollerPlayerResponseMessage::~CPollerPlayerResponseMessage()
		// Test for extended bucket countdown data.
		TEST_F(PollerPlayerResponseMessage, CPollerPlayerResponseMessageTest_6_EB_CountdownData)
		{
			static const DWORD RESPONSE_PADDING_SIZE(4); // Bytes of padding on test input.

			typedef struct
			{
				// Input(s).
				const WORD m_packetVersion; // Upgrade NVRAM from this version to the current version.
				const BYTE* const m_response;
				const WORD m_dataLength;

				// Expected result(s).
				const byte m_card_idExpected[CARD_ID_LEN];
				const WORD m_multiplierExpected;
				const WORD m_dividerExpected;
				const bool m_birthdayIndicatorExpected;
				const bool m_bannedFromPlayIndicatorExpected;
				const bool m_anniversaryIndicatorExpected;
				const DWORD m_totalPointsExpected;
				const WORD m_countdownExpected;
				const DWORD m_compsExpected;
				const byte m_nameLengthExpected;
				const byte* const m_nameExpected;
				const byte m_respLengthExpected;
				const byte* const m_responseExpected;
				const WORD m_lossLimitBalanceExpected;
				const BYTE m_lossLimitStatusExpected;
				const DWORD m_dailyPointsExpected;
				const byte m_maxCardLimitIndicatorExpected;
				const DWORD m_pointsTillNextTierExpected;
				const DWORD m_tierMultiplierExpected;
				const DWORD m_rankIdExpected;
				const byte m_rankNameLengthExpected;
				const byte* const m_rankNameExpected;
				const DWORD m_playerIdExpected;
				const DWORD m_tierPointsExpected;
				const DWORD m_countdownHighPrecisionExpected;
				const AccountingType m_accountingTypeExpected;
				const DWORD m_ebPointsIdExpected;
				const DWORD m_ebCompsIdExpected;
				const DWORD m_ebPromosIdExpected;
				const DWORD m_ebTierPointsIdExpected;
				const bool m_ebIndicatorExpected;
				const std::string m_ebJSONDataExpected;
				const std::vector<PollerPlayerResponseMessage::CountdownDataType> m_ebCountdownDataExpected;
			} TestDataType;

			const BYTE response1[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				0, // m_rankNameLength.
				0x12, 0x34, 0x56, 0x78, // m_playerId
				10, 0, 0, 0, // m_tierPoints
				9, 0, 0, 0, // m_countdownHighPrecision
				0,// m_accountingType
				0, 0, 0, 0, // m_ebPointsId
				0, 0, 0, 0, // m_ebCompsId
				0, 0, 0, 0, // m_ebPromosId
				0, 0, 0, 0, // m_ebTierPointsId
				1, // countdown data count
				101, 0, 0, 0, // countdown data : bucket id
				7, // countdown data : display name length
				'b', 'u', 'c', 'k', 'e', 't', '1', // countdown data : display name
				1, // countdown data : bucket currency length
				'$', // countdown data : bucket currency
				100, 0, 0, 0, // countdown data : countdown
				10, 0, 0, 0, // countdown data : multiplier
				100, 0, 0, 0, // countdown data : divider
				0, 0, 0, 0, // extended bucket data length
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const BYTE response2[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				0, // m_rankNameLength.
				0x12, 0x34, 0x56, 0x78, // m_playerId
				10, 0, 0, 0, // m_tierPoints
				9, 0, 0, 0, // m_countdownHighPrecision
				0,// m_accountingType
				0, 0, 0, 0, // m_ebPointsId
				0, 0, 0, 0, // m_ebCompsId
				0, 0, 0, 0, // m_ebPromosId
				0, 0, 0, 0, // m_ebTierPointsId
				1, // countdown data count
				101, 0, 0, 0, // countdown data : bucket id
				7, // countdown data : display name length
				'b', 'u', 'c', 'k', 'e', 't', '1', // countdown data : display name
				0, // countdown data : bucket currency length
				100, 0, 0, 0, // countdown data : countdown
				10, 0, 0, 0, // countdown data : multiplier
				100, 0, 0, 0, // countdown data : divider
				0, 0, 0, 0, // extended bucket data length
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const BYTE response3[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
				11, 0, // multiplier.
				12, 0, // divider.
				3, // playerOptionBitmask.
				1, // anniversaryIndicator.
				55, 0, 0, 0, // totalPoints.
				9, 0, // countdown.
				66, 0, 0, 0, // comps.
				0, // nameLength.
				0, // respLength.
				0x45, 0x23, // lossLimitBalance.
				0x67, // lossLimitStatus.
				0x9a, 0x78, 0x56, 0x34, // dailyPoints.
				0x78, // maxCardLimitIndicator.
				0xab, 0x89, 0x67, 0x45, // .pointsTillNextTier.
				0xbc, 0x9a, 0x78, 0x56, // tierMultiplier.
				0xcd, 0xab, 0x89, 0x67, // m_rankId.
				0, // m_rankNameLength.
				0x12, 0x34, 0x56, 0x78, // m_playerId
				10, 0, 0, 0, // m_tierPoints
				9, 0, 0, 0, // m_countdownHighPrecision
				0,// m_accountingType
				0, 0, 0, 0, // m_ebPointsId
				0, 0, 0, 0, // m_ebCompsId
				0, 0, 0, 0, // m_ebPromosId
				0, 0, 0, 0, // m_ebTierPointsId
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
				0, 0, 0, 0, // extended bucket data length
				0xcb, 0xcb, 0xcb, 0xcb, // RESPONSE_PADDING.
			};

			const TestDataType TEST_DATA[] =
			{
				{
					15, response1, sizeof(response1) - RESPONSE_PADDING_SIZE,
					{ 0x12, 0x34, 0x56, 0x78, 0x90 },
					11, 12, true, true, true, 55, 9, 66,
					0, NULL,
					0, NULL,
					0x2345, 0x67,
					0x3456789a, 0x78, 0x456789ab, 0x56789abc,
					0x6789abcd,
					0, NULL,
					0x78563412,
					10,	9, AccountingType::ACCOUNTING_COINS_IN,
					0, 0, 0, 0,
					0,
					"",
					{
						{101, "bucket1", "$", 100, 10, 100},
					},
				},
				{
					15, response2, sizeof(response2) - RESPONSE_PADDING_SIZE,
					{ 0x12, 0x34, 0x56, 0x78, 0x90 },
					11, 12, true, true, true, 55, 9, 66,
					0, NULL,
					0, NULL,
					0x2345, 0x67,
					0x3456789a, 0x78, 0x456789ab, 0x56789abc,
					0x6789abcd,
					0, NULL,
					0x78563412,
					10,	9, AccountingType::ACCOUNTING_COINS_IN,
					0, 0, 0, 0,
					0,
					"",
					{
						{101, "bucket1", "", 100, 10, 100},
					},
				},
				{
					15, response3, sizeof(response3) - RESPONSE_PADDING_SIZE,
					{ 0x12, 0x34, 0x56, 0x78, 0x90 },
					11, 12, true, true, true, 55, 9, 66,
					0, NULL,
					0, NULL,
					0x2345, 0x67,
					0x3456789a, 0x78, 0x456789ab, 0x56789abc,
					0x6789abcd,
					0, NULL,
					0x78563412,
					10,	9, AccountingType::ACCOUNTING_COINS_IN,
					0, 0, 0, 0,
					0,
					"",
					{
						{101, "bucket1", "$", 100, 10, 100},
						{102, "bucket2", "", 200, 20, 200},
						{103, "", "", 150, 15, 150},
					},
				}
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CPollerPlayerResponseMessage varObj(TEST_DATA[testIndex].m_response, TEST_DATA[testIndex].m_packetVersion, TEST_DATA[testIndex].m_dataLength);
				AssertEqual(varObj.GetPlayerResponse(),
					TEST_DATA[testIndex].m_card_idExpected,
					TEST_DATA[testIndex].m_multiplierExpected,
					TEST_DATA[testIndex].m_dividerExpected,
					TEST_DATA[testIndex].m_birthdayIndicatorExpected,
					TEST_DATA[testIndex].m_bannedFromPlayIndicatorExpected,
					TEST_DATA[testIndex].m_anniversaryIndicatorExpected,
					TEST_DATA[testIndex].m_totalPointsExpected,
					TEST_DATA[testIndex].m_countdownExpected,
					TEST_DATA[testIndex].m_compsExpected,
					TEST_DATA[testIndex].m_nameLengthExpected,
					TEST_DATA[testIndex].m_nameExpected,
					TEST_DATA[testIndex].m_respLengthExpected,
					TEST_DATA[testIndex].m_responseExpected,
					TEST_DATA[testIndex].m_lossLimitBalanceExpected,
					TEST_DATA[testIndex].m_lossLimitStatusExpected,
					TEST_DATA[testIndex].m_dailyPointsExpected,
					TEST_DATA[testIndex].m_maxCardLimitIndicatorExpected,
					TEST_DATA[testIndex].m_pointsTillNextTierExpected,
					TEST_DATA[testIndex].m_tierMultiplierExpected,
					TEST_DATA[testIndex].m_rankIdExpected,
					TEST_DATA[testIndex].m_rankNameLengthExpected,
					TEST_DATA[testIndex].m_rankNameExpected,
					TEST_DATA[testIndex].m_playerIdExpected,
					TEST_DATA[testIndex].m_tierPointsExpected,
					TEST_DATA[testIndex].m_countdownHighPrecisionExpected,
					TEST_DATA[testIndex].m_accountingTypeExpected,
					TEST_DATA[testIndex].m_ebPointsIdExpected,
					TEST_DATA[testIndex].m_ebCompsIdExpected,
					TEST_DATA[testIndex].m_ebPromosIdExpected,
					TEST_DATA[testIndex].m_ebTierPointsIdExpected,
					TEST_DATA[testIndex].m_ebIndicatorExpected,
					TEST_DATA[testIndex].m_ebJSONDataExpected,
					TEST_DATA[testIndex].m_ebCountdownDataExpected,
					testIndex, __TFUNCTION__, __LINE__, NULL);
			}
		}

	const byte * const PollerPlayerResponseMessage::m_rankNameExpected = NULL;
	const std::vector<PollerPlayerResponseMessage::CountdownDataType> PollerPlayerResponseMessage::m_ebCountdownDataExpected;
}
