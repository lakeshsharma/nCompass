#include "stdafx.h"
#include "gtest/gtest.h"
#include "../Sentinel/Card.h"

const int SDS_CARD_DATA_LENGTH = 16;

namespace SentinelNativeUnitTests
{
	class CardTest : public ::testing::Test
	{
	protected:
		virtual void SetUp()
		{
		}

		virtual void TearDown()
		{
		}

		virtual void TestBody()
		{
		}

	public:
		void SetCardInfoFromRawSdsDataTest(byte* rawData, byte* expectedCardID)
		{
			CCard card(time_t(0), time_t(0));
			card.SetCardInfoFromRawSdsData(rawData, SDS_CARD_DATA_LENGTH);

			for (int i = 0; i < CARD_ID_LEN; i++)
			{
				ASSERT_EQ(card.m_cardID[i], expectedCardID[i]) << "Incorrect Card ID value at position " << i;
			}
		}
	};

	TEST_F(CardTest, CardConstructorTests)
	{
		const time_t utcTime = time(nullptr);
		const time_t localTime = utcTime - 100;
		CCard card(localTime, utcTime);

		int cardIDLen;
		EXPECT_EQ(card.GetCardID(cardIDLen), nullptr);
		EXPECT_EQ(cardIDLen, 0);
		EXPECT_EQ(card.GetCardType(), CARD_UNKNOWN);
		EXPECT_EQ(card.GetCardInTime(), localTime);
		EXPECT_EQ(card.GetUTCCardInTime(), utcTime);
		EXPECT_EQ(card.GetCardOutTime(), localTime);
		EXPECT_FALSE(card.GetIsUniversalCard());
		EXPECT_TRUE(card.CheckCommID(0));
	}

	TEST_F(CardTest, CardCopyConstructorTests)
	{
		const time_t utcTime = time(nullptr);
		const time_t localTime = utcTime - (480 * 60);
		const byte testByteCardNum[5] = { 0x12, 0x34, 0x56, 0x78, 0x90 };
		CCard originalCard(localTime, utcTime);
		originalCard.SetCardInfo(CARD_PLAYER, testByteCardNum, 5);
		originalCard.SetAsValidPlayer();

		{
			CCard* newCard1 = new CCard(originalCard);
			int cardIDLen;
			const byte* cardID = newCard1->GetCardID(cardIDLen);
			ASSERT_EQ(cardIDLen, 5);
			for (int ci = 0; ci < cardIDLen; ci++)
			{
				ASSERT_EQ(*(cardID + ci), testByteCardNum[ci]);
			}

			delete newCard1;
		}

		{
			CCard newCard2(time_t(0), time_t(0));
			newCard2 = originalCard;
			int cardIDLen;
			const byte* cardID = newCard2.GetCardID(cardIDLen);
			ASSERT_EQ(cardIDLen, 5);
			for (int ci = 0; ci < cardIDLen; ci++)
			{
				ASSERT_EQ(*(cardID + ci), testByteCardNum[ci]);
			}
		}

	}

	TEST_F(CardTest, CardIDTests)
	{
		const time_t utcTime = time(nullptr);
		const time_t localTime = utcTime - 100;
		CCard card(localTime, utcTime);

		const char* testCardNum = "1234567890";
		const byte testByteCardNum[5] = { 0x12, 0x34, 0x56, 0x78, 0x90 };
		card.SetCardID(testCardNum);
		int cardIDLen;
		const byte* cardID = card.GetCardID(cardIDLen);
		ASSERT_EQ(cardIDLen, 5);
		for (int ci = 0; ci < cardIDLen; ci++)
		{
			ASSERT_EQ(*(cardID + ci), testByteCardNum[ci]);
		}

		card.SetAsInvalidRead();
		EXPECT_EQ(card.GetCardID(cardIDLen), nullptr);
		EXPECT_EQ(cardIDLen, 0);
	}

	TEST_F(CardTest, CardTimeTests)
	{
		CCard card(time_t(0), time_t(0));

		const time_t utcTime = time(nullptr);
		const time_t localTime = utcTime - (480 * 60);
		card.SetCardInTime(localTime, utcTime);
		EXPECT_EQ(card.GetCardInTime(), localTime);
		EXPECT_EQ(card.GetUTCCardInTime(), utcTime);
		EXPECT_EQ(card.GetCardOutTime(), localTime);

		const time_t cardOutTime = localTime + 500;
		card.SetCardOutTime(cardOutTime);
		EXPECT_EQ(card.GetCardInTime(), localTime);
		EXPECT_EQ(card.GetUTCCardInTime(), utcTime);
		EXPECT_EQ(card.GetCardOutTime(), cardOutTime);
	}

	TEST_F(CardTest, CardMemoryTests)
	{
#if 000//TODO-PORT
		// TODO after NVRAM stuff done
#endif//TODO-PORT
	}

	TEST_F(CardTest, CardFloorTypeTests)
	{
		CCard card(time_t(0), time_t(0));

		card.SetCardID("0000000000");
		ASSERT_EQ(card.DetermineFloorCardType(), CARD_FLOOR);
		card.SetCardID("0000100000");
		ASSERT_EQ(card.DetermineFloorCardType(), CARD_CURRENCY);
		card.SetCardID("0000100001");
		ASSERT_EQ(card.DetermineFloorCardType(), CARD_METER);
		card.SetCardID("0000100011");
		ASSERT_EQ(card.DetermineFloorCardType(), CARD_METER);
	}


	TEST_F(CardTest, CheckCommIDTests)
	{
		CCard card(time_t(0), time_t(0));

		card.SetCardID("0000000000");
		ASSERT_FALSE(card.CheckCommID(0));
		card.SetCardID("0000100000");
		ASSERT_TRUE(card.CheckCommID(1));
		card.SetCardID("0000100000");
		ASSERT_FALSE(card.CheckCommID(2));
		card.SetCardID("0000100001");
		ASSERT_FALSE(card.CheckCommID(255));
		card.SetCardID("6909ff0001");
		ASSERT_TRUE(card.CheckCommID(0));
		card.SetCardID("6909ff0001");
		ASSERT_TRUE(card.CheckCommID(255));
	}

	TEST_F(CardTest, ClearLastCardReadIfNecessaryTest)
	{
		const time_t utcTime = time(nullptr);
		const time_t localTime = utcTime - 100;
		CCard card(localTime, utcTime);
		const int GLOBAL_GLOBAL_CARD_LEN = 5;
		const byte GLOBAL_GLOBAL_CARD_NUM[GLOBAL_GLOBAL_CARD_LEN] = { 0x69, 0x09, 0xff, 0x00, 0x01 };
		CCard tempCard(CUtilities::GetCurrentTime(), CUtilities::GetUTCTime());
		CConfig config(false, "");
		config.SetGlobalCardPrefix(0x69);
		config.SetLastCardRead(GLOBAL_GLOBAL_CARD_NUM);
		byte* lastCardRead(config.GetLastCardRead());
		for (int ci = 0; ci < GLOBAL_GLOBAL_CARD_LEN; ci++)
		{
			ASSERT_TRUE(lastCardRead[ci] == GLOBAL_GLOBAL_CARD_NUM[ci]);
		}
		tempCard.ClearLastCardReadIfNecessary(config);
		lastCardRead = config.GetLastCardRead();
		for (int ci = 0; ci < GLOBAL_GLOBAL_CARD_LEN; ci++)
		{
			ASSERT_TRUE(lastCardRead[ci] == 0);
		}
	}

	TEST_F(CardTest, SetCardInfoFromRawSdsDataTest1)
	{
		byte rawData[SDS_CARD_DATA_LENGTH];
		memcpy(rawData, "%31200000007777?", SDS_CARD_DATA_LENGTH);
		byte expectedCardID[CARD_ID_LEN] = { 0x38, 0x40, 0x00, 0x1e, 0x61 };

		CardTest obj;
		obj.SetCardInfoFromRawSdsDataTest(rawData, expectedCardID);

	}

	TEST_F(CardTest, SetCardInfoFromRawSdsDataTest2)
	{
		byte rawData[SDS_CARD_DATA_LENGTH];
		memcpy(rawData, "%24500100007777?", SDS_CARD_DATA_LENGTH);
		byte expectedCardID[CARD_ID_LEN] = { 0xf5, 0x05, 0xf5, 0xff, 0x61 };

		CardTest obj;
		obj.SetCardInfoFromRawSdsDataTest(rawData, expectedCardID);
	}

	TEST_F(CardTest, SetCardInfoFromRawSdsDataTest3)
	{
		byte rawData[SDS_CARD_DATA_LENGTH];
		memcpy(rawData, "%10100000000004?", SDS_CARD_DATA_LENGTH);
		byte expectedCardID[CARD_ID_LEN] = { 0x65, 0x00, 0x00, 0x00, 0x04 };

		CardTest obj;
		obj.SetCardInfoFromRawSdsDataTest(rawData, expectedCardID);
	}

	TEST_F(CardTest, SetCardInfoFromRawSdsDataTest4)
	{
		byte rawData[SDS_CARD_DATA_LENGTH];
		memcpy(rawData, "%99900999999999?", SDS_CARD_DATA_LENGTH);
		byte expectedCardID[CARD_ID_LEN] = { 0xe7, 0xfb, 0x9a, 0xc9, 0xff };

		CardTest obj;
		obj.SetCardInfoFromRawSdsDataTest(rawData, expectedCardID);
	}
}