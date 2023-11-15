#include "stdafx.h"
#include "RapidJsonHelper.h"
#include "ProgressiveConnectionTestHelpers.h"
#include "ProgressiveProtocol/ProgressiveJackpotAnnouncement.h"
#include "ProgressiveProtocol/ProgressiveTotalizerAnnouncement.h"
#include "UI/TotalizerAnnouncementCommand.h"
#include "UI/JackpotAnnouncementCommand.h"
#include "RapidJsonHelper.h"
#include "Util/Currency.h"

// Unit testing includes.
#include "gtest/gtest.h"

namespace SentinelUnitTests
{
	class ProgressiveAnnouncementTests : public ::testing::Test
	{
	protected:
		const std::string totalizerAnnouncementString = "{\"MetricName\": \"TestMetric\",\"MetricValue\" : 2019.0, \"Id\" : 15,	\"Name\" : \"TA\",	\"SourceDeviceId\" : 1,	\"SourceDeviceConfigVersion\" : 0,	\"SourceAsset\" : \"\",	\"Timestamp\" : 1682499013093}";
		const std::string totalizerAnnouncementNoIdString = "{\"MetricName\": \"TestMetric\",\"MetricValue\" : 2019.0, \"Name\" : \"TA\",	\"SourceDeviceId\" : 1,	\"SourceDeviceConfigVersion\" : 0,	\"SourceAsset\" : \"\",	\"Timestamp\" : 1682499013093}";
		const std::string totalizerAnnouncementNoNameString = "{\"MetricName\": \"TestMetric\",	\"MetricValue\" : 2019.0, \"Id\" : 15, \"SourceDeviceId\" : 1,	\"SourceDeviceConfigVersion\" : 0,	\"SourceAsset\" : \"\",	\"Timestamp\" : 1682499013093}";
		const std::string totalizerAnnouncementNoMetricNameString = "{\"MetricValue\" : 2019.0, \"Id\" : 15,	\"Name\" : \"TA\",	\"SourceDeviceId\" : 1,	\"SourceDeviceConfigVersion\" : 0,	\"SourceAsset\" : \"\",	\"Timestamp\" : 1682499013093}";
		const std::string totalizerAnnouncementNoMetricValueString = "{\"MetricName\": \"TestMetric\", \"Id\" : 15,	\"Name\" : \"TA\",	\"SourceDeviceId\" : 1,	\"SourceDeviceConfigVersion\" : 0,	\"SourceAsset\" : \"\",	\"Timestamp\" : 1682499013093}";

		const std::string jackpotAnnouncementString = "{\"PatronInformation\": {\"WinnerName\": \"Bobs\",\"WinnerFirstName\": \"Bob\",\"WinnerLastName\": \"Samson\",\"CardNumber\": \"5C7B0E3A1B\",\"PlayerId\": 941821,\"Tier\": \"Platinum\"},\"Amount\": 13.32,\"Venue\": \"Default Venue\",\"Bank\": \"Default Bank\",\"AssetNumber\": \"29063\",\"Id\": 5,\"Name\": \"JA\",\"SourceDeviceId\": 2,\"SourceDeviceConfigVersion\": 0,\"SourceAsset\": \"\",\"Timestamp\": 1682494931527}";
		const std::string jackpotAnnouncementNoOptionalString = "{\"Amount\": 13.32,\"Id\": 5,\"Name\": \"JA\",\"SourceDeviceId\": 2,\"SourceDeviceConfigVersion\": 0,\"SourceAsset\": \"\",\"Timestamp\": 1682494931527}";
		const std::string jackpotAnnouncementNoIdString = "{\"Amount\": 13.32,\"Name\": \"JA\",\"SourceDeviceId\": 2,\"SourceDeviceConfigVersion\": 0,\"SourceAsset\": \"\",\"Timestamp\": 1682494931527}";
		const std::string jackpotAnnouncementNoNameString = "{\"Amount\": 13.32,\"Id\": 5,\"SourceDeviceId\": 2,\"SourceDeviceConfigVersion\": 0,\"SourceAsset\": \"\",\"Timestamp\": 1682494931527}";
		const std::string jackpotAnnouncementNoAmountString = "{\"Id\": 5,\"Name\": \"JA\",\"SourceDeviceId\": 2,\"SourceDeviceConfigVersion\": 0,\"SourceAsset\": \"\",\"Timestamp\": 1682494931527}";
	
		FakeProgressiveConnection* TestConnection;
		FakeProgressiveProtocol* TestProgressiveProtocol;
		virtual void SetUp()
		{
			CPlatform::MockInstance("", 0, "", "");
			CProgressiveConfig pc;
			const std::string certLocation("/");
			const std::string firmwareVersion("1");

			//This creates a fake progressive protocol so we can reach in and grab
			// data manipulated from connection and fake data coming in.
			TestProgressiveProtocol = new FakeProgressiveProtocol();

			//This creats a fake connection which lets our tests call into
			// ProgressiveConnection to test the decryption logic.
			TestConnection = new FakeProgressiveConnection(
				*TestProgressiveProtocol, &pc, certLocation, firmwareVersion);
		}

		virtual void TearDown()
		{
			delete(TestConnection);
			delete(TestProgressiveProtocol);
		}

		//sends a message to Progressive connection so we can test without running
		void SendMessageToConnection(std::string MessageToSend, std::string HeaderType)
		{
			ActiveMQBytesMessage msg;
			msg.setStringProperty(OneLinkHeader::type, HeaderType);
			msg.setBodyBytes((const unsigned char*)MessageToSend.c_str(), (int)MessageToSend.size());
			msg.reset();
			TestConnection->onMessage(&msg);
		}
	};

	//Test the constructor of the totalizer announcement to parse the json
	TEST_F(ProgressiveAnnouncementTests, ProgressiveAnnouncementTests_Create_Totalizer)
	{
		std::vector<char> totalizerAnnouncementPacket(totalizerAnnouncementString.c_str(), totalizerAnnouncementString.c_str() + totalizerAnnouncementString.size() + 1);
		rapidjson::Document jsonDoc;
		
		jsonDoc.ParseInsitu(&totalizerAnnouncementPacket[0]);
		CProgressiveTotalizerAnnouncement totalizerAnnouncement(jsonDoc);
		
		ASSERT_EQ(totalizerAnnouncement.m_Id, 15);
		ASSERT_EQ(totalizerAnnouncement.m_Name, "TA");
		ASSERT_EQ(totalizerAnnouncement.m_MetricName, "TestMetric");
		ASSERT_EQ(totalizerAnnouncement.m_MetricValue, Currency::ToString(2019.0));
	}

	//Test the constructor of the jackpot announcement to parse the json
	TEST_F(ProgressiveAnnouncementTests, ProgressiveAnnouncementTests_Create_Jackpot)
	{
		std::vector<char> jackpotAnnouncementPacket(jackpotAnnouncementString.c_str(), jackpotAnnouncementString.c_str() + jackpotAnnouncementString.size() + 1);
		rapidjson::Document jsonDoc;
		jsonDoc.ParseInsitu(&jackpotAnnouncementPacket[0]);
		CProgressiveJackpotAnnouncement jackpotAnnouncement(jsonDoc);

		ASSERT_EQ(jackpotAnnouncement.m_Id, 5);
		ASSERT_EQ(jackpotAnnouncement.m_Name, "JA");
		ASSERT_EQ(jackpotAnnouncement.m_Amount, Currency::ToString(13.32));
		ASSERT_EQ(jackpotAnnouncement.m_CardNumber, "5C7B0E3A1B");
		ASSERT_EQ(jackpotAnnouncement.m_PlayerId, 941821);
		ASSERT_EQ(jackpotAnnouncement.m_Tier, "Platinum");
		ASSERT_EQ(jackpotAnnouncement.m_Venue, "Default Venue");
		ASSERT_EQ(jackpotAnnouncement.m_Bank, "Default Bank");
		ASSERT_EQ(jackpotAnnouncement.m_AssetNumber, "29063");
	}

	//Test the constructor of the totalizer announcement to parse the json with none of the optionals.
	TEST_F(ProgressiveAnnouncementTests, ProgressiveAnnouncementTests_Create_Jackpot_No_Optionals)
	{
		std::vector<char> jackpotAnnouncementPacket(jackpotAnnouncementNoOptionalString.c_str(), jackpotAnnouncementNoOptionalString.c_str() + jackpotAnnouncementNoOptionalString.size() + 1);
		rapidjson::Document jsonDoc;
		jsonDoc.ParseInsitu(&jackpotAnnouncementPacket[0]);
		CProgressiveJackpotAnnouncement jackpotAnnouncement(jsonDoc);

		ASSERT_EQ(jackpotAnnouncement.m_Id, 5);
		ASSERT_EQ(jackpotAnnouncement.m_Name, "JA");
		ASSERT_EQ(jackpotAnnouncement.m_Amount, Currency::ToString(13.32));
		ASSERT_EQ(jackpotAnnouncement.m_CardNumber, "");
		ASSERT_EQ(jackpotAnnouncement.m_PlayerId, -1);
		ASSERT_EQ(jackpotAnnouncement.m_Tier, "");
		ASSERT_EQ(jackpotAnnouncement.m_Venue, "");
		ASSERT_EQ(jackpotAnnouncement.m_Bank, "");
		ASSERT_EQ(jackpotAnnouncement.m_AssetNumber, "");
	}

	//Test the constructor of the jackpot announcement to parse the json and throw because the Id is required
	TEST_F(ProgressiveAnnouncementTests, ProgressiveAnnouncementTests_Create_Jackpot_No_Id)
	{
		EXPECT_THROW({
			std::vector<char> jackpotAnnouncementPacket(jackpotAnnouncementNoIdString.c_str(), jackpotAnnouncementNoIdString.c_str() + jackpotAnnouncementNoIdString.size() + 1);
			rapidjson::Document jsonDoc;
			jsonDoc.ParseInsitu(&jackpotAnnouncementPacket[0]);
			CProgressiveJackpotAnnouncement jackpotAnnouncement(jsonDoc);
		}, JsonException);
	}

	//Test the constructor of the jackpot announcement to parse the json and throw because the Name is required
	TEST_F(ProgressiveAnnouncementTests, ProgressiveAnnouncementTests_Create_Jackpot_No_Name)
	{
		EXPECT_THROW({
			std::vector<char> jackpotAnnouncementPacket(jackpotAnnouncementNoNameString.c_str(), jackpotAnnouncementNoNameString.c_str() + jackpotAnnouncementNoNameString.size() + 1);
			rapidjson::Document jsonDoc;
			jsonDoc.ParseInsitu(&jackpotAnnouncementPacket[0]);
			CProgressiveJackpotAnnouncement jackpotAnnouncement(jsonDoc);
			}, JsonException);
	}

	//Test the constructor of the jackpot announcement to parse the json and throw because the Amount is required
	TEST_F(ProgressiveAnnouncementTests, ProgressiveAnnouncementTests_Create_Jackpot_No_Amount)
	{
		EXPECT_THROW({
			std::vector<char> jackpotAnnouncementPacket(jackpotAnnouncementNoAmountString.c_str(), jackpotAnnouncementNoAmountString.c_str() + jackpotAnnouncementNoAmountString.size() + 1);
			rapidjson::Document jsonDoc;
			jsonDoc.ParseInsitu(&jackpotAnnouncementPacket[0]);
			CProgressiveJackpotAnnouncement jackpotAnnouncement(jsonDoc);
			}, JsonException);
	}

	//Test the onMessage of the Protocol to ensure it sends a correct jp announcement through to internals
	TEST_F(ProgressiveAnnouncementTests, ProgressiveJackpotAnnouncementTests_Through_Connection)
	{
		//Send the jackpot announcement through normal prog protocol sent directly from onelink
		SendMessageToConnection(jackpotAnnouncementString, "ICardJackpotAnnouncement");

		//This grabs the jp announcement from same mechanism that sends to internals.
		CProgressiveJackpotAnnouncement* announcement = TestProgressiveProtocol->GetJackpotAnnouncement();

		std::vector<char> jackpotAnnouncementPacket(jackpotAnnouncementString.c_str(), jackpotAnnouncementString.c_str() + jackpotAnnouncementString.size() + 1);
		rapidjson::Document jsonDoc;
		jsonDoc.ParseInsitu(&jackpotAnnouncementPacket[0]);
		CProgressiveJackpotAnnouncement jackpotAnnouncement(jsonDoc);

		ASSERT_EQ(jackpotAnnouncement.m_Id, announcement->m_Id);
		ASSERT_EQ(jackpotAnnouncement.m_Name, announcement->m_Name);
		ASSERT_EQ(jackpotAnnouncement.m_Amount, announcement->m_Amount);
		ASSERT_EQ(jackpotAnnouncement.m_CardNumber, announcement->m_CardNumber);
		ASSERT_EQ(jackpotAnnouncement.m_PlayerId, announcement->m_PlayerId);
		ASSERT_EQ(jackpotAnnouncement.m_Tier, announcement->m_Tier);
		ASSERT_EQ(jackpotAnnouncement.m_Venue, announcement->m_Venue);
		ASSERT_EQ(jackpotAnnouncement.m_Bank, announcement->m_Bank);
		ASSERT_EQ(jackpotAnnouncement.m_AssetNumber, announcement->m_AssetNumber);
	}

	//Test the constructor of the totalizer announcement to parse the json and throw because the Id is required
	TEST_F(ProgressiveAnnouncementTests, ProgressiveAnnouncementTests_Create_Totalizer_No_Id)
	{
		EXPECT_THROW({
			std::vector<char> totalizerAnnouncementPacket(totalizerAnnouncementNoIdString.c_str(), totalizerAnnouncementNoIdString.c_str() + totalizerAnnouncementNoIdString.size() + 1);
			rapidjson::Document jsonDoc;

			jsonDoc.ParseInsitu(&totalizerAnnouncementPacket[0]);
			CProgressiveTotalizerAnnouncement totalizerAnnouncement(jsonDoc);
		}, JsonException);
	}

	//Test the constructor of the totalizer announcement to parse the json and throw because the Name is required
	TEST_F(ProgressiveAnnouncementTests, ProgressiveAnnouncementTests_Create_Totalizer_No_Name)
	{
		EXPECT_THROW({
			std::vector<char> totalizerAnnouncementPacket(totalizerAnnouncementNoNameString.c_str(), totalizerAnnouncementNoNameString.c_str() + totalizerAnnouncementNoNameString.size() + 1);
			rapidjson::Document jsonDoc;

			jsonDoc.ParseInsitu(&totalizerAnnouncementPacket[0]);
			CProgressiveTotalizerAnnouncement totalizerAnnouncement(jsonDoc);
			}, JsonException);
	}

	//Test the constructor of the totalizer announcement to parse the json and throw because the MetricName is required
	TEST_F(ProgressiveAnnouncementTests, ProgressiveAnnouncementTests_Create_Totalizer_No_MetricName)
	{
		EXPECT_THROW({
			std::vector<char> totalizerAnnouncementPacket(totalizerAnnouncementNoMetricNameString.c_str(), totalizerAnnouncementNoMetricNameString.c_str() + totalizerAnnouncementNoMetricNameString.size() + 1);
			rapidjson::Document jsonDoc;

			jsonDoc.ParseInsitu(&totalizerAnnouncementPacket[0]);
			CProgressiveTotalizerAnnouncement totalizerAnnouncement(jsonDoc);
			}, JsonException);
	}

	//Test the constructor of the totalizer announcement to parse the json and throw because the MetricValue is required
	TEST_F(ProgressiveAnnouncementTests, ProgressiveAnnouncementTests_Create_Totalizer_No_MetricValue)
	{
		EXPECT_THROW({
			std::vector<char> totalizerAnnouncementPacket(totalizerAnnouncementNoMetricValueString.c_str(), totalizerAnnouncementNoMetricValueString.c_str() + totalizerAnnouncementNoMetricValueString.size() + 1);
			rapidjson::Document jsonDoc;

			jsonDoc.ParseInsitu(&totalizerAnnouncementPacket[0]);
			CProgressiveTotalizerAnnouncement totalizerAnnouncement(jsonDoc);
			}, JsonException);
	}

	//Test the onMessage of the Protocol to ensure it sends a correct totalizer announcement through to internals
	TEST_F(ProgressiveAnnouncementTests, ProgressiveTotalizerAnnouncementTests_Through_Connection)
	{
		//Send the totalizer announcement through normal prog protocol sent directly from onelink
		SendMessageToConnection(totalizerAnnouncementString, "ICardTotalizerAnnouncement");

		//This grabs the totalizer announcement from same mechanism that sends to internals.
		CProgressiveTotalizerAnnouncement* announcement = TestProgressiveProtocol->GetTotalizerAnnouncement();

		std::vector<char> totalizerAnnouncementPacket(totalizerAnnouncementString.c_str(), totalizerAnnouncementString.c_str() + totalizerAnnouncementString.size() + 1);
		rapidjson::Document jsonDoc;

		jsonDoc.ParseInsitu(&totalizerAnnouncementPacket[0]);
		CProgressiveTotalizerAnnouncement totalizerAnnouncement(jsonDoc);

		ASSERT_EQ(totalizerAnnouncement.m_Id, announcement->m_Id);
		ASSERT_EQ(totalizerAnnouncement.m_Name, announcement->m_Name);
		ASSERT_EQ(totalizerAnnouncement.m_MetricName, announcement->m_MetricName);
		ASSERT_EQ(totalizerAnnouncement.m_MetricValue, announcement->m_MetricValue);
	}

	//Test the totalizer announcement command and ensure that the json sent to the UI is correct
	TEST_F(ProgressiveAnnouncementTests, ProgressiveAnnouncementTests_Create_TotalizerCommand)
	{
		//vars for asserts below
		bool checkedId = false;
		bool checkedLevelName = false;
		bool checkedMetricName = false;
		bool checkedAwardDescription = false;
		bool checkedTotalizerLine1 = false;
		bool checkedTotalizerLine2 = false;
		bool checkedAwardCurrency = false;
		
		//json name value for dynamic tags
		const char* name = "name";
		const char* value = "value";

		//Create a totalizer announcement
		std::vector<char> totalizerAnnouncementPacket(totalizerAnnouncementString.c_str(), totalizerAnnouncementString.c_str() + totalizerAnnouncementString.size() + 1);
		rapidjson::Document jsonDoc;
		jsonDoc.ParseInsitu(&totalizerAnnouncementPacket[0]);
		CProgressiveTotalizerAnnouncement totalizerAnnouncement(jsonDoc);

		//Create the totalizer command that will be parsed and sent to UI
		CTotalizerAnnouncementCommand announcementCommand(30000, totalizerAnnouncement, "");
		auto announcementJSON = announcementCommand.getJSON();

		//Parse the JSON that is pulled from the command and sent to the UI
		std::vector<char> totalizerAnnouncementCommand(announcementJSON.c_str(), announcementJSON.c_str() + announcementJSON.size() + 1);
		rapidjson::Document doc;
		doc.ParseInsitu(&totalizerAnnouncementCommand[0]);

		//The root object is commandArgs. select this to find nodes.
		auto commandArgs = doc.FindMember("commandArgs");
		if (commandArgs != doc.MemberEnd() && commandArgs->value.IsObject())
		{
			//The dynamic Tags field is an array where each item is saved in a key value pair with name and value.
			auto dynamicTags = commandArgs->value.FindMember("dynamicTags");
			if (dynamicTags != commandArgs->value.MemberEnd() && dynamicTags->value.IsArray())
			{
				//Loop through each found object. Find the name then compare the value with the field.
				for (rapidjson::Value::ConstValueIterator itr = dynamicTags->value.Begin(); itr != dynamicTags->value.End(); ++itr) {
					if (itr->HasMember("name")) 
					{
						std::string nameNode(JsonGetStringEx(*itr, name));
						if (nameNode == "AnnouncementId")
						{
							auto valueNode(JsonGetInt64Ex(*itr, value));
							ASSERT_EQ(totalizerAnnouncement.m_Id, valueNode);
							checkedId = true;
						}
						else if (nameNode == "AnnouncementName")
						{
							auto valueNode(JsonGetStringEx(*itr, value));
							ASSERT_EQ(totalizerAnnouncement.m_Name, valueNode);
							checkedLevelName = true;
						}
						else if (nameNode == "MetricName")
						{
							auto valueNode(JsonGetStringEx(*itr, value));
							ASSERT_EQ(totalizerAnnouncement.m_MetricName, valueNode);
							checkedMetricName = true;
						}
						else if (nameNode == "AwardDescription")
						{
							auto valueNode(JsonGetStringEx(*itr, value));
							ASSERT_EQ(CUtilities::GetFormattedNumberString(totalizerAnnouncement.m_MetricValue),valueNode);
							checkedAwardDescription = true;
						}
						else if (nameNode == "TotalizerLine1")
						{
							std::string valueNode(JsonGetStringEx(*itr, value));
							ASSERT_EQ("TotalizerLine1", valueNode);
							checkedTotalizerLine1 = true;
						}
						else if (nameNode == "TotalizerLine2")
						{
							std::string valueNode(JsonGetStringEx(*itr, value));
							ASSERT_EQ("TotalizerLine2", valueNode);
							checkedTotalizerLine2 = true;
						}
						else if (nameNode == "AwardCurrency")
						{
							std::string valueNode(JsonGetStringEx(*itr, value));
							ASSERT_EQ("AwardCurrency", valueNode);
							checkedAwardCurrency = true;
						}
					}
				}
			}
		}
		//This makes sure it checked the 4 nodes.
		ASSERT_TRUE(checkedId);
		ASSERT_TRUE(checkedLevelName);
		ASSERT_TRUE(checkedMetricName);
		ASSERT_TRUE(checkedAwardDescription);
		ASSERT_TRUE(checkedTotalizerLine1);
		ASSERT_TRUE(checkedTotalizerLine2);
		ASSERT_TRUE(checkedAwardCurrency);
	}

	//Test the jackpot announcement command and ensure that the json sent to the UI is correct
	TEST_F(ProgressiveAnnouncementTests, ProgressiveAnnouncementTests_Create_JackpotCommand)
	{
		//vars for asserts below
		bool checkedId = false;
		bool checkedName = false;
		bool checkedAwardDescription = false;
		bool checkedVenue = false;
		bool checkedBank = false;
		bool checkedAssetNumber = false;
		bool checkedPlayerId = false;
		bool checkedCardNumber = false;
		bool checkedTier = false;
		bool checkedWinnerName = false;
		bool checkedWinnerFirstName = false;
		bool checkedWinnerLastName = false;
		bool checkedAwardCurrency = false;

		//json name value for dynamic tags
		const char* name = "name";
		const char* value = "value";

		//Create a jackpot announcement
		std::vector<char> jackpotAnnouncementPacket(jackpotAnnouncementString.c_str(), jackpotAnnouncementString.c_str() + jackpotAnnouncementString.size() + 1);
		rapidjson::Document jsonDoc;
		jsonDoc.ParseInsitu(&jackpotAnnouncementPacket[0]);
		CProgressiveJackpotAnnouncement jackpotAnnouncement(jsonDoc);

		//Create the jackpot announcement command that will be parsed and sent to UI
		CJackpotAnnouncementCommand announcementCommand(30000, jackpotAnnouncement, "");
		auto announcementJSON = announcementCommand.getJSON();

		//Parse the JSON that is pulled from the command and sent to the UI
		std::vector<char> jackpotAnnouncementCommand(announcementJSON.c_str(), announcementJSON.c_str() + announcementJSON.size() + 1);
		rapidjson::Document doc;
		doc.ParseInsitu(&jackpotAnnouncementCommand[0]);

		//The root object is commandArgs. select this to find nodes.
		auto commandArgs = doc.FindMember("commandArgs");
		if (commandArgs != doc.MemberEnd() && commandArgs->value.IsObject())
		{
			//The dynamic Tags field is an array where each item is saved in a key value pair with name and value.
			auto dynamicTags = commandArgs->value.FindMember("dynamicTags");
			if (dynamicTags != commandArgs->value.MemberEnd() && dynamicTags->value.IsArray())
			{
				//Loop through each found object. Find the name then compare the value with the field.
				for (rapidjson::Value::ConstValueIterator itr = dynamicTags->value.Begin(); itr != dynamicTags->value.End(); ++itr) {
					if (itr->HasMember("name")) 
					{
						std::string nameNode(JsonGetStringEx(*itr, name));
						if (nameNode == "AnnouncementId")
						{
							auto valueNode(JsonGetInt64Ex(*itr, value));
							ASSERT_EQ(jackpotAnnouncement.m_Id, valueNode);
							checkedId = true;
						}
						else if (nameNode == "AnnouncementName")
						{
							auto valueNode(JsonGetStringEx(*itr, value));
							ASSERT_EQ(jackpotAnnouncement.m_Name, valueNode);
							checkedName = true;
						}
						else if (nameNode == "AwardDescription")
						{
							auto valueNode(JsonGetStringEx(*itr, value));
							ASSERT_EQ(jackpotAnnouncement.m_Amount, valueNode);
							checkedAwardDescription = true;
						}
						else if (nameNode == "Venue")
						{
							auto valueNode(JsonGetOptionalStringEx(*itr, value));
							ASSERT_EQ(jackpotAnnouncement.m_Venue, valueNode);
							checkedVenue = true;
						}
						else if (nameNode == "Bank")
						{
							auto valueNode(JsonGetOptionalStringEx(*itr, value));
							ASSERT_EQ(jackpotAnnouncement.m_Bank, valueNode);
							checkedBank = true;
						}
						else if (nameNode == "AssetNumber")
						{
							auto valueNode(JsonGetOptionalStringEx(*itr, value));
							ASSERT_EQ(jackpotAnnouncement.m_AssetNumber, valueNode);
							checkedAssetNumber = true;
						}
						else if (nameNode == "WinnerPlayerId")
						{
							auto valueNode(JsonGetInt64Ex(*itr, value));
							ASSERT_EQ(jackpotAnnouncement.m_PlayerId, valueNode);
							checkedPlayerId = true;
						}
						else if (nameNode == "CardNumber")
						{
							auto valueNode(JsonGetOptionalStringEx(*itr, value));
							ASSERT_EQ(jackpotAnnouncement.m_CardNumber, valueNode);
							checkedCardNumber = true;
						}
						else if (nameNode == "WinnerTier")
						{
							auto valueNode(JsonGetOptionalStringEx(*itr, value));
							ASSERT_EQ(jackpotAnnouncement.m_Tier, valueNode);
							checkedTier = true;
						}
						else if (nameNode == "WinnerName")
						{
							auto valueNode(JsonGetOptionalStringEx(*itr, value));
							ASSERT_EQ(jackpotAnnouncement.m_WinnerName, valueNode);
							checkedWinnerName = true;
						}
						else if (nameNode == "WinnerFirstName")
						{
							auto valueNode(JsonGetOptionalStringEx(*itr, value));
							ASSERT_EQ(jackpotAnnouncement.m_WinnerFirstName, valueNode);
							checkedWinnerFirstName = true;
						}
						else if (nameNode == "WinnerLastName")
						{
							auto valueNode(JsonGetOptionalStringEx(*itr, value));
							ASSERT_EQ(jackpotAnnouncement.m_WinnerLastName, valueNode);
							checkedWinnerLastName = true;
						}
						else if (nameNode == "AwardCurrency")
						{
							std::string valueNode(JsonGetOptionalStringEx(*itr, value));
							ASSERT_EQ("AwardCurrency", valueNode);
							checkedAwardCurrency = true;
						}
					}
				}
			}
		}
		//This makes sure it checked the nodes.
		ASSERT_TRUE(checkedId);
		ASSERT_TRUE(checkedName);
		ASSERT_TRUE(checkedAwardDescription);
		ASSERT_TRUE(checkedVenue);
		ASSERT_TRUE(checkedBank);
		ASSERT_TRUE(checkedAssetNumber);
		ASSERT_TRUE(checkedPlayerId);
		ASSERT_TRUE(checkedCardNumber);
		ASSERT_TRUE(checkedTier);
		ASSERT_TRUE(checkedWinnerName);
		ASSERT_TRUE(checkedWinnerFirstName);
		ASSERT_TRUE(checkedWinnerLastName);
		ASSERT_TRUE(checkedAwardCurrency);
	}
}