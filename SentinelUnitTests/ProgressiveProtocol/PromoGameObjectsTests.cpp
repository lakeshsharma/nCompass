#include "stdafx.h"
#include "ProgressiveProtocol/PromoGameRequest.h"
#include "ProgressiveProtocol/PromoGameAcceptanceStatus.h"
#include "ProgressiveProtocol/ProgressiveLevelTestHelpers.h"

// Unit testing includes.
#include "UnitTestingUtils.h"

#include "gtest/gtest.h"
using namespace std;

const string promoGameRequestString("{\"JackpotId\":2037,\"LevelId\":266,\"MinorAwardId\":164,\"Amount\":12,\"PaytableItemType\":\"Value\",\"PaytableItemValue\":30,\
		\"PromoGamePaytable\":{\"Name\":\"LouSidegame\",\"Items\":[{\"Type\":\"Value\",\"Value\":20,\"Probability\":0.200000000},\{\"Type\":\"Value\",\"Value\":30,\"Probability\":0.200000000},{\"Type\":\"Value\",\"Value\":40,\"Probability\":0.100000000},{\"Type\":\"Value\",\"Value\":50,\"Probability\":0.100000000},{\"Type\":\"Value\",\"Value\":75,\"Probability\":0.100000000},{\"Type\":\"Value\",\"Value\":100,\"Probability\":0.100000000},{\"Type\":\"Value\",\"Value\":250,\"Probability\":0.100000000},{\"Type\":\"Value\",\"Value\":500,\"Probability\":0.100000000}]},\
        \"JackpotTriggerType\":\"CoinInTrigger\",\"SourceDeviceId\":92,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1558022505311}");

const string promoGameRequestWithPayTableItemTagString("{\"JackpotId\":2037,\"LevelId\":266,\"MinorAwardId\":164,\"Amount\":12,\"PaytableItemTag\":3,\"PaytableItemType\":\"Value\",\"PaytableItemValue\":30,\
		\"PromoGamePaytable\":{\"Name\":\"LouSidegame\",\"Items\":[{\"Type\":\"Value\",\"Value\":20,\"Probability\":0.200000000},\{\"Type\":\"Value\",\"Value\":30,\"Probability\":0.200000000},{\"Type\":\"Value\",\"Value\":40,\"Probability\":0.100000000},{\"Type\":\"Value\",\"Value\":50,\"Probability\":0.100000000},{\"Type\":\"Value\",\"Value\":75,\"Probability\":0.100000000},{\"Type\":\"Value\",\"Value\":100,\"Probability\":0.100000000},{\"Type\":\"Value\",\"Value\":250,\"Probability\":0.100000000},{\"Type\":\"Value\",\"Value\":500,\"Probability\":0.100000000}]},\
        \"JackpotTriggerType\":\"CoinInTrigger\",\"SourceDeviceId\":92,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1558022505311}");

class PromoGameObjectsTests : public ::testing::Test
{
};

TEST_F(PromoGameObjectsTests, PromoGameRequestTest)
{
    rapidjson::Document jsonDoc;
    jsonDoc.Parse(promoGameRequestString.c_str()).HasParseError();
    CPromoGameRequest promoGameRequest(jsonDoc);
    ASSERT_TRUE(266 == promoGameRequest.GetLevelId());
    ASSERT_TRUE("164" == promoGameRequest.GetMinorAwardId());
    ASSERT_TRUE(2037 == promoGameRequest.GetJackpotId());
    ASSERT_TRUE(JackpotTrigger::CoinIn == promoGameRequest.GetJackpotTriggerType());
    ASSERT_TRUE("Value" == promoGameRequest.GetPaytableItemType());
	ASSERT_TRUE(30 == promoGameRequest.GetPaytableItemValue());
	ASSERT_FALSE(promoGameRequest.PaytableItemTagExists());
	ASSERT_TRUE(266 == promoGameRequest.GetLevelId());
	ASSERT_TRUE(JackpotTrigger::Type::CoinIn == promoGameRequest.GetJackpotTriggerType());
}

TEST_F(PromoGameObjectsTests, PromoGameAcceptanceStatusTest)
{
	int64_t jackpotId(123);
	CPromoGameAcceptanceStatus promoGameAcceptanceStatus(jackpotId);
	ASSERT_TRUE(jackpotId == promoGameAcceptanceStatus.GetJackpotId());
	ASSERT_TRUE("Accepted" == promoGameAcceptanceStatus.GetPromoGameStatus());
}

TEST_F(PromoGameObjectsTests, PromoGameRequestPaytableItemTagTest)
{
	rapidjson::Document jsonDoc;
	jsonDoc.Parse(promoGameRequestWithPayTableItemTagString.c_str()).HasParseError();
	CPromoGameRequest promoGameRequest(jsonDoc);
	ASSERT_TRUE(promoGameRequest.PaytableItemTagExists());
	ASSERT_TRUE(3 == promoGameRequest.m_paytableItemTag);
}
