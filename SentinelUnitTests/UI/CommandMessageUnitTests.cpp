#include "stdafx.h"
#include "UI/FullScreenAttractorCommand.h"
#include "UI/PopupTextCommand.h"
#include "UI/MassMarketingCommand.h"
#include "UI/WinnerPickAndPickedCommand.h"
#include "UI/PromoGameCommand.h"
#include "UI/ScheduleEndCommand.h"
#include "ProgressiveProtocol/ProgressiveWinnerPicked.h"

#include "GlobalDefs.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingUtils.h"
#include "gtest/gtest.h"

using namespace std;

namespace SentinelUnitTests
{
	class CommandMessageUnitTests : public ::testing::Test
	{
	public:
		virtual void SetUp()
		{
		}

		virtual void TearDown()
		{
		}
	};

	TEST_F(CommandMessageUnitTests, CommandMessageTest)
	{
		string expectedJSONData;
		expectedJSONData = R"({"type":"CommandMessage",)";
		expectedJSONData += R"("commandName":"ListenToPBTProgress",)";
		expectedJSONData += R"("commandArgs":{)";
		expectedJSONData += R"("displayDuration":0}})";

		CCommandMessage obj("ListenToPBTProgress");

		std::string jsonData = obj.getJSON();
		ASSERT_EQ(jsonData, expectedJSONData) << "Incorrect JSON data";
	}

	TEST_F(CommandMessageUnitTests, FullScreenAttractorCommandTest)
	{
		string expectedJSONData;
		expectedJSONData = R"({"type":"CommandMessage",)";
		expectedJSONData += R"("commandName":"ShowFullScreenAttractor",)";
		expectedJSONData += R"("commandArgs":{)";
		expectedJSONData += R"("displayDuration":3000,)";
		expectedJSONData += R"("assetTag":"Birthday"}})";

		CFullScreenAttractorCommand obj(3000, "Birthday", "");

		std::string jsonData = obj.getJSON();
		ASSERT_EQ(jsonData, expectedJSONData) << "Incorrect JSON data";
	}

	TEST_F(CommandMessageUnitTests, PopupTextCommandTest)
	{
		string expectedJSONData;
		expectedJSONData = R"({"type":"CommandMessage",)";
		expectedJSONData += R"("commandName":"ShowPopupText",)";
		expectedJSONData += R"("commandArgs":{)";
		expectedJSONData += R"("displayDuration":3000,)";
		expectedJSONData += R"("textTagType":"CashoutErrorFlags",)";
		expectedJSONData += R"("textTag":"1",)";
		expectedJSONData += R"("popupType":5}})";

		CPopupTextCommand obj("CashoutErrorFlags", "1", PopupTextType::Popup_Failure, 3000);

		std::string jsonData = obj.getJSON();
		ASSERT_EQ(jsonData, expectedJSONData) << "Incorrect JSON data";
	}

	TEST_F(CommandMessageUnitTests, MassMarketingCommandTest)
	{
		const char* JSON_VALUE =
		"{"
		"\"Amount\":1.23,"
		"\"ClaimableAcceptanceUrl\":\"URL\","
		"\"ClaimableExpirationMillis\":3100,"
		"\"ClaimableSamePlayer\":true"
		"}";

		string expectedJSONData;
		expectedJSONData = R"({"type":"CommandMessage",)";
		expectedJSONData += R"("commandName":"ShowMassMarketingScreen",)";
		expectedJSONData += R"("commandArgs":{)";
		expectedJSONData += R"("displayDuration":3000,)";
		expectedJSONData += R"("AwardAmount":"1.23",)";
		expectedJSONData += R"("Url":"URL",)";
		expectedJSONData += R"("Expiration":3100,)";
		expectedJSONData += R"("SamePlayer":true}})";

		rapidjson::Document jsonDoc;
		ASSERT_FALSE(jsonDoc.Parse(JSON_VALUE).HasParseError());

		CProgressiveClaimableAward award(1000, jsonDoc);
		CMassMarketingCommand obj(&award, 3000);

		ASSERT_EQ(obj.getJSON(), expectedJSONData) << "Incorrect JSON data";
	}

	TEST_F(CommandMessageUnitTests, WinnerPickAndPickedCommandDefaultTest)
	{
		CWinnerPickAndPickedCommand obj(
			3000,
			"WinnerPicked", 
			"",
			"LevelName",
			"1000",
			"PrizeDescription",
			"PaymentMethod",
			"AwardType",
			"ExternalAwardName",
			"ExternalAwardValue",
			"ExternalAwardCurrency");

		string expectedJSONData("{\"type\":\"CommandMessage\",\"commandName\":\"ShowFullScreenAttractor\",\"commandArgs\":{\"displayDuration\":3000,\"assetTag\":\"WinnerPicked\",\"dynamicTags\":[{\"name\":\"LevelName\",\"value\":\"LevelName\"},{\"name\":\"PaymentMethod\",\"value\":\"PaymentMethod\"},{\"name\":\"AwardCurrency\",\"value\":\"AwardCurrency\"},{\"name\":\"AwardDescription\",\"value\":\"1000\"},{\"name\":\"PaymentModePrefix\",\"value\":\"\"},{\"name\":\"PaymentMode\",\"value\":\"\"},{\"name\":\"PaymentBucket\",\"value\":\"ExternalAwardName\"}]}}");

		ASSERT_EQ(obj.getJSON(), expectedJSONData) << "Incorrect JSON data";
	}

	TEST_F(CommandMessageUnitTests, WinnerPickAndPickedCommandExtendedBucketTest)
	{
		CWinnerPickAndPickedCommand obj(
			3000,
			"WinnerPicked",
			"",
			"Louie's Extended Bucket",
			"1000",
			"",
			"PayToPlayerAccount",
			"ExtendedBucket",
			"shr_newbucket",
			"1000.75569875",
			"PHP");

		string expectedJSONData("{\"type\":\"CommandMessage\",\"commandName\":\"ShowFullScreenAttractor\",\"commandArgs\":{\"displayDuration\":3000,\"assetTag\":\"WinnerPickedExtendedBucket\",\"dynamicTags\":[{\"name\":\"LevelName\",\"value\":\"Louie's Extended Bucket\"},{\"name\":\"PaymentMethod\",\"value\":\"PayToPlayerAccount\"},{\"name\":\"AwardCurrency\",\"value\":\"PHP\"},{\"name\":\"AwardDescription\",\"value\":\"1000.76\"},{\"name\":\"PaymentModePrefix\",\"value\":\"ExtendedBucketToAccountPrefix\"},{\"name\":\"PaymentMode\",\"value\":\"ExtendedBucketToAccount\"},{\"name\":\"PaymentBucket\",\"value\":\"shr_newbucket\"}]}}");

		ASSERT_EQ(obj.getJSON(), expectedJSONData) << "Incorrect JSON data";
	}
	
	TEST_F(CommandMessageUnitTests, WinnerPickAndPickedCommandOfferTest)
	{
		CWinnerPickAndPickedCommand obj(
			3000,
			"WinnerPicked",
			"",
			"LevelName",
			"1000",
			"PrizeDescription",
			"PayToPlayerAccount",
			"Offer",
			"ExternalAwardName",
			"ExternalAwardValue",
			"PHP");

		string expectedJSONData("{\"type\":\"CommandMessage\",\"commandName\":\"ShowFullScreenAttractor\",\"commandArgs\":{\"displayDuration\":3000,\"assetTag\":\"WinnerPickedOffer\",\"dynamicTags\":[{\"name\":\"LevelName\",\"value\":\"LevelName\"},{\"name\":\"PaymentMethod\",\"value\":\"PayToPlayerAccount\"},{\"name\":\"AwardCurrency\",\"value\":\"\"},{\"name\":\"AwardDescription\",\"value\":\"ExternalAwardName\"},{\"name\":\"PaymentModePrefix\",\"value\":\"\"},{\"name\":\"PaymentMode\",\"value\":\"OfferToAccount\"},{\"name\":\"PaymentBucket\",\"value\":\"ExternalAwardName\"}]}}");

		ASSERT_EQ(obj.getJSON(), expectedJSONData) << "Incorrect JSON data";
	}

	TEST_F(CommandMessageUnitTests, WinnerPickAndPickedCommandPrizeTest)
	{
		CWinnerPickAndPickedCommand obj(
			3000,
			"WinnerPicked",
			"",
			"LevelName",
			"1000",
			"PrizeDescription",
			"PayToGame",
			"Prize",
			"ExternalAwardName",
			"ExternalAwardValue",
			"PHP");

		string expectedJSONData("{\"type\":\"CommandMessage\",\"commandName\":\"ShowFullScreenAttractor\",\"commandArgs\":{\"displayDuration\":3000,\"assetTag\":\"WinnerPickedPrize\",\"dynamicTags\":[{\"name\":\"LevelName\",\"value\":\"LevelName\"},{\"name\":\"PaymentMethod\",\"value\":\"PayToGame\"},{\"name\":\"AwardCurrency\",\"value\":\"\"},{\"name\":\"AwardDescription\",\"value\":\"PrizeDescription\"},{\"name\":\"PaymentModePrefix\",\"value\":\"\"},{\"name\":\"PaymentMode\",\"value\":\"PrizeToAccount\"},{\"name\":\"PaymentBucket\",\"value\":\"ExternalAwardName\"}]}}");

		ASSERT_EQ(obj.getJSON(), expectedJSONData) << "Incorrect JSON data";
	}

	TEST_F(CommandMessageUnitTests, WinnerPickAndPickedCommandCashableCreditsTest)
	{
		CWinnerPickAndPickedCommand obj(
			3000,
			"WinnerPicked",
			"",
			"LevelName",
			"1000",
			"PrizeDescription",
			"PayToGame",
			"CashableCredits",
			"ExternalAwardName",
			"ExternalAwardValue",
			"PHP");

		string expectedJSONData("{\"type\":\"CommandMessage\",\"commandName\":\"ShowFullScreenAttractor\",\"commandArgs\":{\"displayDuration\":3000,\"assetTag\":\"WinnerPicked\",\"dynamicTags\":[{\"name\":\"LevelName\",\"value\":\"LevelName\"},{\"name\":\"PaymentMethod\",\"value\":\"PayToGame\"},{\"name\":\"AwardCurrency\",\"value\":\"AwardCurrency\"},{\"name\":\"AwardDescription\",\"value\":\"1000\"},{\"name\":\"PaymentModePrefix\",\"value\":\"\"},{\"name\":\"PaymentMode\",\"value\":\"CashableCreditsToGame\"},{\"name\":\"PaymentBucket\",\"value\":\"ExternalAwardName\"}]}}");

		ASSERT_EQ(obj.getJSON(), expectedJSONData) << "Incorrect JSON data";
	}

	TEST_F(CommandMessageUnitTests, WinnerPickAndPickedCommandFreePlayToGameTest)
	{
		CWinnerPickAndPickedCommand obj(
			3000,
			"WinnerPicked",
			"",
			"LevelName",
			"1000",
			"PrizeDescription",
			"PayToGame",
			"Freeplay",
			"ExternalAwardName",
			"ExternalAwardValue",
			"PHP");

		string expectedJSONData("{\"type\":\"CommandMessage\",\"commandName\":\"ShowFullScreenAttractor\",\"commandArgs\":{\"displayDuration\":3000,\"assetTag\":\"WinnerPicked\",\"dynamicTags\":[{\"name\":\"LevelName\",\"value\":\"LevelName\"},{\"name\":\"PaymentMethod\",\"value\":\"PayToGame\"},{\"name\":\"AwardCurrency\",\"value\":\"AwardCurrency\"},{\"name\":\"AwardDescription\",\"value\":\"1000\"},{\"name\":\"PaymentModePrefix\",\"value\":\"\"},{\"name\":\"PaymentMode\",\"value\":\"FreePlayToGame\"},{\"name\":\"PaymentBucket\",\"value\":\"ExternalAwardName\"}]}}");

		ASSERT_EQ(obj.getJSON(), expectedJSONData) << "Incorrect JSON data";
	}

	TEST_F(CommandMessageUnitTests, WinnerPickAndPickedCommandRestrictedCreditsToGameTest)
	{
		CWinnerPickAndPickedCommand obj(
			3000,
			"WinnerPicked",
			"",
			"LevelName",
			"1000",
			"PrizeDescription",
			"PayToGame",
			"RestrictedCredits",
			"ExternalAwardName",
			"ExternalAwardValue",
			"PHP");

		string expectedJSONData("{\"type\":\"CommandMessage\",\"commandName\":\"ShowFullScreenAttractor\",\"commandArgs\":{\"displayDuration\":3000,\"assetTag\":\"WinnerPicked\",\"dynamicTags\":[{\"name\":\"LevelName\",\"value\":\"LevelName\"},{\"name\":\"PaymentMethod\",\"value\":\"PayToGame\"},{\"name\":\"AwardCurrency\",\"value\":\"AwardCurrency\"},{\"name\":\"AwardDescription\",\"value\":\"1000\"},{\"name\":\"PaymentModePrefix\",\"value\":\"\"},{\"name\":\"PaymentMode\",\"value\":\"RestrictedCreditsToGame\"},{\"name\":\"PaymentBucket\",\"value\":\"ExternalAwardName\"}]}}");

		ASSERT_EQ(obj.getJSON(), expectedJSONData) << "Incorrect JSON data";
	}

	TEST_F(CommandMessageUnitTests, WinnerPickAndPickedCommandCashableCreditsHandpayToGameTest)
	{
		CWinnerPickAndPickedCommand obj(
			3000,
			"WinnerPicked",
			"",
			"LevelName",
			"1000",
			"PrizeDescription",
			"PayToGameWithHandPay",
			"CashableCredits",
			"ExternalAwardName",
			"ExternalAwardValue",
			"PHP");

		string expectedJSONData("{\"type\":\"CommandMessage\",\"commandName\":\"ShowFullScreenAttractor\",\"commandArgs\":{\"displayDuration\":3000,\"assetTag\":\"WinnerPicked\",\"dynamicTags\":[{\"name\":\"LevelName\",\"value\":\"LevelName\"},{\"name\":\"PaymentMethod\",\"value\":\"PayToGameWithHandPay\"},{\"name\":\"AwardCurrency\",\"value\":\"AwardCurrency\"},{\"name\":\"AwardDescription\",\"value\":\"1000\"},{\"name\":\"PaymentModePrefix\",\"value\":\"\"},{\"name\":\"PaymentMode\",\"value\":\"CashableCreditsHandpayToGame\"},{\"name\":\"PaymentBucket\",\"value\":\"ExternalAwardName\"}]}}");

		ASSERT_EQ(obj.getJSON(), expectedJSONData) << "Incorrect JSON data";
	}

	TEST_F(CommandMessageUnitTests, WinnerPickAndPickedCommandCashableCreditsToAccountTest)
	{
		CWinnerPickAndPickedCommand obj(
			3000,
			"WinnerPicked",
			"",
			"LevelName",
			"1000",
			"PrizeDescription",
			"PayToPlayerAccount",
			"CashableCredits",
			"ExternalAwardName",
			"ExternalAwardValue",
			"PHP");

		string expectedJSONData("{\"type\":\"CommandMessage\",\"commandName\":\"ShowFullScreenAttractor\",\"commandArgs\":{\"displayDuration\":3000,\"assetTag\":\"WinnerPicked\",\"dynamicTags\":[{\"name\":\"LevelName\",\"value\":\"LevelName\"},{\"name\":\"PaymentMethod\",\"value\":\"PayToPlayerAccount\"},{\"name\":\"AwardCurrency\",\"value\":\"AwardCurrency\"},{\"name\":\"AwardDescription\",\"value\":\"1000\"},{\"name\":\"PaymentModePrefix\",\"value\":\"\"},{\"name\":\"PaymentMode\",\"value\":\"CashableCreditsToAccount\"},{\"name\":\"PaymentBucket\",\"value\":\"ExternalAwardName\"}]}}");

		ASSERT_EQ(obj.getJSON(), expectedJSONData) << "Incorrect JSON data";
	}

	TEST_F(CommandMessageUnitTests, WinnerPickAndPickedCommandCompsToAccountTest)
	{
		CWinnerPickAndPickedCommand obj(
			3000,
			"WinnerPicked",
			"",
			"LevelName",
			"1000",
			"PrizeDescription",
			"PayToPlayerAccount",
			"Comps",
			"ExternalAwardName",
			"ExternalAwardValue",
			"PHP");

		string expectedJSONData("{\"type\":\"CommandMessage\",\"commandName\":\"ShowFullScreenAttractor\",\"commandArgs\":{\"displayDuration\":3000,\"assetTag\":\"WinnerPicked\",\"dynamicTags\":[{\"name\":\"LevelName\",\"value\":\"LevelName\"},{\"name\":\"PaymentMethod\",\"value\":\"PayToPlayerAccount\"},{\"name\":\"AwardCurrency\",\"value\":\"AwardCurrency\"},{\"name\":\"AwardDescription\",\"value\":\"1000\"},{\"name\":\"PaymentModePrefix\",\"value\":\"\"},{\"name\":\"PaymentMode\",\"value\":\"CompsToAccount\"},{\"name\":\"PaymentBucket\",\"value\":\"ExternalAwardName\"}]}}");

		ASSERT_EQ(obj.getJSON(), expectedJSONData) << "Incorrect JSON data";
	}

	TEST_F(CommandMessageUnitTests, WinnerPickAndPickedCommandFreePlayToAccountTest)
	{
		CWinnerPickAndPickedCommand obj(
			3000,
			"WinnerPicked",
			"",
			"LevelName",
			"1000",
			"PrizeDescription",
			"PayToPlayerAccount",
			"Freeplay",
			"ExternalAwardName",
			"ExternalAwardValue",
			"PHP");

		string expectedJSONData("{\"type\":\"CommandMessage\",\"commandName\":\"ShowFullScreenAttractor\",\"commandArgs\":{\"displayDuration\":3000,\"assetTag\":\"WinnerPicked\",\"dynamicTags\":[{\"name\":\"LevelName\",\"value\":\"LevelName\"},{\"name\":\"PaymentMethod\",\"value\":\"PayToPlayerAccount\"},{\"name\":\"AwardCurrency\",\"value\":\"AwardCurrency\"},{\"name\":\"AwardDescription\",\"value\":\"1000\"},{\"name\":\"PaymentModePrefix\",\"value\":\"\"},{\"name\":\"PaymentMode\",\"value\":\"FreePlayToAccount\"},{\"name\":\"PaymentBucket\",\"value\":\"ExternalAwardName\"}]}}");

		ASSERT_EQ(obj.getJSON(), expectedJSONData) << "Incorrect JSON data";
	}

	TEST_F(CommandMessageUnitTests, WinnerPickAndPickedCommandPointsToAccountTest)
	{
		CWinnerPickAndPickedCommand obj(
			3000,
			"WinnerPicked",
			"/opt/ncompass/media/assets/dynamicContent/57-CelebrationContent-LcdContent",
			"LevelName",
			"1000",
			"PrizeDescription",
			"PayToPlayerAccount",
			"Points",
			"ExternalAwardName",
			"ExternalAwardValue",
			"PHP");

		string expectedJSONData("{\"type\":\"CommandMessage\",\"commandName\":\"ShowFullScreenAttractor\",\"commandArgs\":{\"displayDuration\":3000,\"dynamicContentPath\":\"/opt/ncompass/media/assets/dynamicContent/57-CelebrationContent-LcdContent\",\"assetTag\":\"WinnerPicked\",\"dynamicTags\":[{\"name\":\"LevelName\",\"value\":\"LevelName\"},{\"name\":\"PaymentMethod\",\"value\":\"PayToPlayerAccount\"},{\"name\":\"AwardCurrency\",\"value\":\"AwardCurrency\"},{\"name\":\"AwardDescription\",\"value\":\"1000\"},{\"name\":\"PaymentModePrefix\",\"value\":\"\"},{\"name\":\"PaymentMode\",\"value\":\"PointsToAccount\"},{\"name\":\"PaymentBucket\",\"value\":\"ExternalAwardName\"}]}}");

		ASSERT_EQ(obj.getJSON(), expectedJSONData) << "Incorrect JSON data";
	}	

	TEST_F(CommandMessageUnitTests, ScheduleEndCommandTest)
	{
		CScheduleEndCommand obj(3000, "/opt/ncompass/media/assets/dynamicContent/57-ScheduleEndContent-LcdContent", "Loutest");

		string expectedJSONData("{\"type\":\"CommandMessage\",\"commandName\":\"ShowFullScreenAttractor\",\"commandArgs\":{\"displayDuration\":3000,\"dynamicContentPath\":\"/opt/ncompass/media/assets/dynamicContent/57-ScheduleEndContent-LcdContent\",\"assetTag\":\"ScheduleEnd\",\"dynamicTags\":[{\"name\":\"LevelName\",\"value\":\"Loutest\"}]}}");

		ASSERT_EQ(obj.getJSON(), expectedJSONData) << "Incorrect JSON data";
	}

	TEST_F(CommandMessageUnitTests, PromoGameCommandTest)
	{
		string expectedJSONData;
		expectedJSONData = R"({"type":"CommandMessage",)";
		expectedJSONData += R"("commandName":"ShowPromoGame",)";
		expectedJSONData += R"("commandArgs":{)";
		expectedJSONData += R"("displayDuration":0,)";
		expectedJSONData += R"("dynamicContentPath":"/opt/ncompass/media/assets/dynamicContent/57-PromoGameContent-LcdContent",)";
		expectedJSONData += R"("PaytableItemType":"ItemType",)";
		expectedJSONData += R"("PaytableItemValue":10,)";
		expectedJSONData += R"("JackpotId":101,)";
		expectedJSONData += R"("Amount":1000}})";
		
		CPromoGameCommand obj("ItemType", 10, 101, 1000, "/opt/ncompass/media/assets/dynamicContent/57-PromoGameContent-LcdContent");

		std::string jsonData = obj.getJSON();
		ASSERT_EQ(jsonData, expectedJSONData) << "Incorrect JSON data";
	}
}
