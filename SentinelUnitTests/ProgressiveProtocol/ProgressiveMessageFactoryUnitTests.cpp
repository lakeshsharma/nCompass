
#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveMessageFactory.h"
#include "SASProtocol/SASGeneralException.h"
#include "Util/Convert.h"
#include "RapidJsonTestHelper.h"
#include "ProgressiveLevelTestHelpers.h"
#include "TestUtil.h"

#include "gtest/gtest.h"
using namespace std;

string OUTPUTDEBUG(string jsonStr)
{
	string OutputDebugString("### pJson ###\n");
	OutputDebugString += jsonStr.c_str();
	OutputDebugString += "\n### pJson ###\n";
	return OutputDebugString;
}

namespace SentinelNativeUnitTests
{
	class ProgressiveMessageFactoryUnitTests : public ::testing::Test
	{
	public:

		void f_ProgressiveMessageFactoryConstructorUnitTest(void)
		{
			CConnection connection;
			connection.SetMacAddress("AA-BB-CC-DD-EE-FF");
			CProgressiveMessageFactory factory(connection);

			string s = factory.GetMacAddressWithColon();
			ASSERT_STREQ("AA:BB:CC:DD:EE:FF", s.c_str()) << "GetMacAddressWithColon";

			s = factory.GetMacAddressWithoutColon();
			ASSERT_STREQ("AABBCCDDEEFF", s.c_str()) << "GetMacAddressWithoutColon";
			ASSERT_FALSE(factory.IsPopulatedFromDiscovery()) << "IsPopulatedFromDiscovery";
		}

		void f_CreateDeviceAcknowledgementUnitTest(void)
		{
			CConnection connection;
			connection.SetSlotNumber(1);
			connection.SetMacAddress("AA-BB-CC-DD-EE-FF");
			CProgressiveMessageFactory factory(connection);

			factory.m_isPopulatedFromDiscovery = true;
			factory.m_sourceDeviceID = 1;
			factory.m_sourceDevice = "2";
			factory.m_sourceAsset = "4";

			struct
			{
				int line;
				const char* pMessageId;
				bool success;
				const char* pDescription;
			}
			static data[] =
			{
				{ __LINE__, "5",	 false, nullptr },
				{ __LINE__, nullptr, false, nullptr },
				{ __LINE__, "5",	 true,	nullptr },
				{ __LINE__, nullptr, true,	nullptr },
				{ __LINE__, "5",	 false, "6"		},
				{ __LINE__, nullptr, false, "6"		},
				{ __LINE__, "5",	 true,	"6"		},
				{ __LINE__, nullptr, true,	"6"		}
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string s(factory.CreateDeviceAcknowledgement(data[i].pMessageId, data[i].success, data[i].pDescription));

				rapidjson::Document jsonDoc;
				bool result = jsonDoc.Parse(s.c_str()).HasParseError();
				ASSERT_FALSE(result) << AssertMsg::Format(data[i].line, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

				JsonGetStringTest(jsonDoc, "MessageRefId", data[i].pMessageId, data[i].line);
				JsonGetBoolTest(jsonDoc, "Success", data[i].success, true, data[i].line);
				JsonGetStringTest(jsonDoc, "Description", data[i].pDescription, data[i].line);
				JsonGetInt64Test(jsonDoc, "SourceDeviceId", factory.m_sourceDeviceID, true, data[i].line);
				JsonGetStringTest(jsonDoc, "SourceDevice", factory.m_sourceDevice.c_str(), data[i].line);
				JsonGetStringTest(jsonDoc, "SourceAsset", factory.m_sourceAsset.c_str(), data[i].line);
				JsonGetInt64Test(jsonDoc, "Timestamp", 0, true, data[i].line, false);
				JsonGetStringTest(jsonDoc, "MessageId", "", data[i].line, false);
			}
		}


		void f_ProgressiveMessageFactory_WritePatronSessionInfo_nullptr(void)
		{
			CConnection connection;
			CProgressiveMessageFactory factory(connection);

			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);
			factory.WritePatronInformation(writer, nullptr);
			ASSERT_TRUE(writeStream.m_str.empty()) << "WritePatronInformation(writer, nullptr)";
		}

		void f_ProgressiveMessageFactory_WritePatronSessionInfo_empty(void)
		{
			CConnection connection;
			CProgressiveMessageFactory factory(connection);

			CPlayer *player = new CPlayer();

			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);
			factory.WritePatronInformation(writer, player);
			ASSERT_TRUE(writeStream.m_str.empty()) << L"WritePatronInformation(writer, &player)";
		}

		void f_ProgressiveMessageFactory_WritePatronSessionInfo_playerinfoNoId(void)
		{
			CConnection connection;
			CProgressiveMessageFactory factory(connection);

			CPlayer *player = new CPlayer();
			player->SetPlayerInfo(new CPlayerInfo);
			player->GetPlayerInfo()->SetName("Test");
			player->GetPlayerInfo()->SetRankName("rAnK");
			player->GetPlayerInfo()->SetPlayerId(CPlayerInfo::INVALID_PLAYER_ID);

			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);

			writer.StartObject();
			factory.WritePatronInformation(writer, player);
			writer.EndObject();

			rapidjson::Document jsonDoc;
			bool result = jsonDoc.Parse(writeStream.m_str.c_str()).HasParseError();
			ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

			const rapidjson::Value& value = JsonGetObjectTest(jsonDoc, "PatronInformation");

			JsonGetStringTest(value, "Name", "Test");
			JsonGetStringTest(value, "Tier", "rAnK");
			JsonGetUintTest(value, "PlayerId", 0, false);
			JsonGetStringTest(value, "CardNumber", nullptr);
			JsonGetStringTest(value, "SessionPts", nullptr);
		}

		void f_ProgressiveMessageFactory_WritePatronSessionInfo_playerinfoId(void)
		{
			CConnection connection;
			CProgressiveMessageFactory factory(connection);

			CPlayer *player = new CPlayer();
			player->SetPlayerInfo(new CPlayerInfo);
			player->GetPlayerInfo()->SetName("Test");
			player->GetPlayerInfo()->SetRankName("rAnK");
			player->GetPlayerInfo()->SetPlayerId(0xfffffffe);

			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);

			writer.StartObject();
			factory.WritePatronInformation(writer, player);
			writer.EndObject();

			rapidjson::Document jsonDoc;
			bool result = jsonDoc.Parse(writeStream.m_str.c_str()).HasParseError();
			ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

			const rapidjson::Value& value = JsonGetObjectTest(jsonDoc, "PatronInformation");

			JsonGetStringTest(value, "Name", "Test");
			JsonGetStringTest(value, "Tier", "rAnK");
			JsonGetUintTest(value, "PlayerId", 0xfffffffe);
			JsonGetStringTest(value, "CardNumber", nullptr);
			JsonGetStringTest(value, "SessionPts", nullptr);
		}

		void f_ProgressiveMessageFactory_WritePatronSessionInfo_nocardid_noplayerinfo(void)
		{
			CConnection connection;
			CProgressiveMessageFactory factory(connection);

			CPlayer *player = new CPlayer();

			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);
			factory.WritePatronInformation(writer, player);
			ASSERT_TRUE(writeStream.m_str.empty()) << "WritePatronInformation(writer, &player)";
		}

		void f_ProgressiveMessageFactory_WritePatronSessionInfo_card_noplayerinfo(void)
		{
			CConnection connection;
			CProgressiveMessageFactory factory(connection);

			time_t cardInTime(0);

			CPlayer *player = new CPlayer();
			player->m_card = new CCard(cardInTime, cardInTime);
			player->m_card->SetCardID("12345");

			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);

			writer.StartObject();
			factory.WritePatronInformation(writer, player);
			writer.EndObject();

			rapidjson::Document jsonDoc;
			bool result = jsonDoc.Parse(writeStream.m_str.c_str()).HasParseError();
			ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

			const rapidjson::Value& value = JsonGetObjectTest(jsonDoc, "PatronInformation");

			JsonGetStringTest(value, "Name", nullptr);
			JsonGetStringTest(value, "Tier", nullptr);
			JsonGetUintTest(value, "PlayerId", 0, false);
			JsonGetStringTest(value, "CardNumber", "1234050000");
			JsonGetStringTest(value, "SessionPts", nullptr);
		}

		void f_ProgressiveMessageFactory_WritePatronSessionInfo(void)
		{
			CConnection connection;
			CProgressiveMessageFactory factory(connection);

			CPlayer *player = new CPlayer();
			player->SetPlayerInfo(new CPlayerInfo);
			player->GetPlayerInfo()->SetName("Test");
			player->GetPlayerInfo()->SetRankName("rAnK");
			player->GetPlayerInfo()->SetPlayerId(0xfffffffe);

			time_t cardInTime(0);
			player->m_card = new CCard(cardInTime, cardInTime);
			player->m_card->SetCardID("12345");

			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);

			writer.StartObject();
			factory.WritePatronInformation(writer, player);
			writer.EndObject();

			rapidjson::Document jsonDoc;
			bool result = jsonDoc.Parse(writeStream.m_str.c_str()).HasParseError();
			ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

			const rapidjson::Value& value = JsonGetObjectTest(jsonDoc, "PatronInformation");

			JsonGetStringTest(value, "Name", "Test");
			JsonGetStringTest(value, "Tier", "rAnK");
			JsonGetUintTest(value, "PlayerId", 0xfffffffe);
			JsonGetStringTest(value, "CardNumber", "1234050000");
			JsonGetStringTest(value, "SessionPts", nullptr);
		}

		void f_ProgressiveMessageFactory_WriteGameData(void)
		{
			CConnection connection;
			CProgressiveMessageFactory factory(connection);

			CGame game(MEMORY_NONE, false);

			game.SetDenom(2, DenomCurrencyType::US_DOLLARS);
			game.SetMaxBet(5);
			game.SetTheoreticalPayback(6);
			game.SetMfgID("ab");
			game.SetPaytableID("12345678");

			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);

			factory.WriteGameData(&game, writer, 5);
			string s = writeStream.m_str;

			const char* pExpected = R"({"GameNumber":5,"Denomination":0.02,"MaxBetCredits":5,"TheoPercentage":0.06,"GameName":"ab","Paytable":"12345678","DiscoveredLevelIds":""})";
			ASSERT_STREQ(pExpected, s.c_str()) << "Game data not same";
		}

		void f_ProgressiveMessageFactory_CreateICardConfigurationRequest5Test(void)
		{
			CConnection connection;
			CProgressiveMessageFactory factory(connection);
			factory.m_slotNumber = 12345;
			factory.m_macAddress = "112233445566";
			factory.m_sentinelVersion = "1.2.3.4";
			factory.m_actualIP = "10.147.0.209";
			factory.m_firmwareVersion = "1.0.106";
			factory.m_osVersion = "1.1.2";

			CGames games;
			CGame* currentGame(new CGame(MEMORY_NONE, true));
			currentGame->SetGameNumber(1);
			currentGame->SetDenom(1);
			currentGame->SetMaxBet(1);
			currentGame->SetGameNumber(1);
			currentGame->SetTheoreticalPayback(1);
			currentGame->SetMfgID("Id1");
			currentGame->SetPaytableID("Pay1");
			games.AddGame(currentGame);

			currentGame = new CGame(MEMORY_NONE, false);
			currentGame->SetGameNumber(2);
			currentGame->SetDenom(2);
			currentGame->SetMaxBet(2);
			currentGame->SetGameNumber(2);
			currentGame->SetTheoreticalPayback(2);
			currentGame->SetMfgID("Id2");
			currentGame->SetPaytableID("Pay2");
			games.AddGame(currentGame);

			games.SetAddress(1);
			games.SetGameId("GameId");
			games.SetSASVersion("SASVersion");
			games.SetSerialNumber("SerialNumber");
			games.SetMultiDenomEnabled(true);
			games.SetExtendedMetersEnabled(false);
			games.SetAftEnabled(true);
			games.SetAftBonusEnabled(false);
			games.SetAftInHouseEnabled(true);
			games.SetLegacyBonusEnabled(false);
			games.SetEftTransferFromEnabled(true);
			games.SetEftTransferToEnabled(false);
			games.SetValidationExtensionsEnabled(true);
			games.SetProgressiveGroup(7);

			string jsonStr = factory.CreateICardConfigurationRequest5(&games);
			string str = OUTPUTDEBUG(jsonStr);

			rapidjson::Document jsonDoc;
			bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
			ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

			const rapidjson::Value& keyGames = jsonDoc[pKeyGames];
			if (keyGames.IsArray())
			{
				for (auto it = keyGames.Begin(); it != keyGames.End(); ++it)
				{
					CGame* game(games.GetGame(JsonGetUint(*it, pKeyGameNumber)));
					if (game != NULL)
					{
						JsonGetDoubleTest(*it, pKeyDenomination, (double)game->GetDenomMultiplier() / ONE_DOLLAR_IN_CENTS);
						JsonGetInt64Test(*it, pKeyMaxBetCredits, game->GetMaxBet());
						JsonGetDoubleTest(*it, pKeyTheoPercentage, (double)game->GetTheoreticalPayback() / ONE_DOLLAR_IN_CENTS);
						JsonGetStringTest(*it, pKeyGameName, (const char*)(game->GetMfgID()));
						JsonGetStringTest(*it, pKeyPaytable, (const char*)(game->GetPaytableID()));
					}
				}
			}

			const rapidjson::Value& valueSasDiscovery = JsonGetObjectTest(jsonDoc, pKeySasDiscovery);
			JsonGetUintTest(valueSasDiscovery, pKeyAddress, games.GetAddress());
			JsonGetStringTest(valueSasDiscovery, pKeyGameId, games.GetGameId().c_str());
			JsonGetStringTest(valueSasDiscovery, pKeySasVersion, games.GetSASVersion().c_str());
			JsonGetStringTest(valueSasDiscovery, pKeySerialNumber, games.GetSerialNumber().c_str());
			JsonGetStringTest(valueSasDiscovery, pKeyAssetNumber, to_string(factory.m_slotNumber).c_str());
			JsonGetCurrencyTest(valueSasDiscovery, pKeyAccountingDenomination, games.GetAccountingDenom());
			JsonGetBoolTest(valueSasDiscovery, pKeyMultiDenomEnabled, games.GetMultiDenomEnabled());
			JsonGetBoolTest(valueSasDiscovery, pKeyExtendedMetersEnabled, games.GetExtendedMetersEnabled());
			JsonGetBoolTest(valueSasDiscovery, pKeyAftEnabled, games.GetAftEnabled());
			JsonGetBoolTest(valueSasDiscovery, pKeyAftBonusEnabled, games.GetAftBonusEnabled());
			JsonGetBoolTest(valueSasDiscovery, pKeyAftInHouseEnabled, games.GetAftInHouseEnabled());
			JsonGetBoolTest(valueSasDiscovery, pKeyLegacyBonusEnabled, games.GetLegacyBonusEnabled());
			JsonGetBoolTest(valueSasDiscovery, pKeyEftTransferToEnabled, games.GetEftTransferFromEnabled());
			JsonGetBoolTest(valueSasDiscovery, pKeyEftTransferFromEnabled, games.GetEftTransferToEnabled());
			JsonGetBoolTest(valueSasDiscovery, pKeyValidationExtensionsSupported, games.GetValidationExtensionsEnabled());
			JsonGetUintTest(valueSasDiscovery, pKeyProgressiveGroup, games.GetProgressiveGroup());

			const rapidjson::Value& value = JsonGetObjectTest(jsonDoc, pKeyFeatures);
			JsonGetBoolTest(value, pKeyPlaylists, true);

			JsonGetStringTest(jsonDoc, pKeyDeviceMAC, factory.GetMacAddressWithColon().c_str());
			JsonGetStringTest(jsonDoc, pKeyImplementationVersion, factory.m_sentinelVersion.c_str());
			JsonGetStringTest(jsonDoc, pKeyIpAddress, factory.m_actualIP.c_str());
			JsonGetStringTest(jsonDoc, pKeyManagementURL, string("http://" + factory.m_actualIP).c_str());

			JsonGetStringTest(jsonDoc, pKeyOSVersion, factory.m_osVersion.c_str());
		}

		void f_ProgressiveMessageFactory_WriteGameData_empty(void)
		{
			CConnection connection;
			CProgressiveMessageFactory factory(connection);

			CGame game(MEMORY_NONE, false);

			game.SetDenom(0, DenomCurrencyType::US_DOLLARS);
			game.SetMaxBet(0);
			game.SetTheoreticalPayback((WORD)0);
			game.SetMfgID("");
			game.SetPaytableID("");

			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);

			factory.WriteGameData(&game, writer, 0);
			string s = writeStream.m_str;

			const char* pExpected = R"({"GameNumber":0,"Denomination":0.0,"MaxBetCredits":0,"TheoPercentage":0.0,"GameName":"","Paytable":"","DiscoveredLevelIds":""})";
			ASSERT_STREQ(pExpected, s.c_str()) << "Game data not same";
		}

		void f_ProgressiveMessageFactory_CreateICardConfigurationRequest5_empty(void)
		{
			CConnection connection;
			CProgressiveMessageFactory factory(connection);
			factory.m_sentinelVersion = "12.0.30501.0";
			factory.m_actualIP = "10.147.0.209";
			CGames *games = new CGames();

			const char* pExpected = R"({"Games":[],"SASDiscovery":{"Address":1,"GameId":"","SASVersion":"","SerialNumber":"","AssetNumber":"0","AccountingDenomination":0.0,"MultiDenomEnabled":false,"ExtendedMetersEnabled":false,"AFTEnabled":false,"AFTBonusEnabled":false,"AFTInHouseEnabled":false,"LegacyBonusEnabled":false,"EFTTransferToEnabled":false,"EFTTransferFromEnabled":false,"ValidationExtensionsSupported":false,"ProgressiveGroup":0},"Features":{"Playlists":true,"RemoteReboot":true,"PromoGames":true,"PrizeJackpot":true,"SlotsContent":true,"DownloadableFirmware":true,"DownloadableConfigFiles":true,"Campaigns":true,"Skins":true},"DeviceMAC":"","ImplementationVersion":"12.0.30501.0","OSVersion":"","Platform":"NCOMPASS-PT206","PlatformFirmware":[{"Name":"os.fb","Version":""},{"Name":"exe.fb","Version":"12.0.30501.0"},{"Name":"fpga.fb","Version":""}],"IPAddress":"10.147.0.209","ManagementURL":"http://10.147.0.209","SourceDeviceConfigVersion":0,"SourceDeviceId":0,"SourceDevice":"","SourceAsset":"","Timestamp":)";
			const char* pExpectedEnd = strstr(pExpected, R"("Timestamp":)");

			string rval = factory.CreateICardConfigurationRequest5(games);
			auto rvalEnd = rval.find(R"("Timestamp":)");

			ASSERT_EQ((size_t)(pExpectedEnd - pExpected), (size_t)rvalEnd) << "Length";
			ASSERT_EQ(0, strncmp(pExpected, rval.c_str(), rvalEnd)) << "strncmp";

		}

		void f_ProgressiveMessageFactory_CreateConfigFileDownloadAck(void)
		{
			CConnection connection;
			CProgressiveMessageFactory factory(connection);
			factory.m_sourceAsset = "123456";
			string messageRefId("101");
			string filename("config.zip");

			const char* pExpected = R"({"SourceAsset":"123456","FileName":"config.zip","MessageRefId":"101","Timestamp":)";
			const char* pExpectedEnd = strstr(pExpected, R"("Timestamp":)");

			string rval = factory.CreateConfigFileDownloadAck(messageRefId, filename);
			auto rvalEnd = rval.find(R"("Timestamp":)");

			ASSERT_EQ((size_t)(pExpectedEnd - pExpected), (size_t)rvalEnd) << "Length";
			ASSERT_EQ(0, strncmp(pExpected, rval.c_str(), rvalEnd)) << "strncmp";
		}
	};


	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactoryConstructorUnitTest)
	{
		f_ProgressiveMessageFactoryConstructorUnitTest();
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, CreateDeviceAcknowledgementUnitTest)
	{
		f_CreateDeviceAcknowledgementUnitTest();
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_WritePatronSessionInfo_nullptr)
	{
		f_ProgressiveMessageFactory_WritePatronSessionInfo_nullptr();
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_WritePatronSessionInfo_empty)
	{
		f_ProgressiveMessageFactory_WritePatronSessionInfo_empty();
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_WritePatronSessionInfo_playerinfoNoId)
	{
		f_ProgressiveMessageFactory_WritePatronSessionInfo_playerinfoNoId();
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_WritePatronSessionInfo_playerinfoId)
	{
		f_ProgressiveMessageFactory_WritePatronSessionInfo_playerinfoId();
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_WritePatronSessionInfo_nocardid_noplayerinfo)
	{
		f_ProgressiveMessageFactory_WritePatronSessionInfo_nocardid_noplayerinfo();
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_WritePatronSessionInfo_card_noplayerinfo)
	{
		f_ProgressiveMessageFactory_WritePatronSessionInfo_card_noplayerinfo();
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_WritePatronSessionInfo)
	{
		f_ProgressiveMessageFactory_WritePatronSessionInfo();
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateICardConfigurationRequest5Test)
	{
		f_ProgressiveMessageFactory_CreateICardConfigurationRequest5Test();
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateGameEnd5)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		CGame* currentGame(new CGame(MEMORY_NONE, true));
		currentGame->SetDenom(5);
		currentGame->SetGameNumber(2);
		currentGame->SetTheoreticalPayback(75);

		CGameEndData gameData(currentGame, 25, 1000, 2, NULL, 5, 10);
		string jsonStr = factory.CreateGameEnd5(gameData, nullptr, 12345);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetUintTest(jsonDoc, "GameNumber", 2);
		JsonGetCurrencyTest(jsonDoc, "Denomination", 5);
		JsonGetCurrencyTest(jsonDoc, "Win", 10);
		JsonGetIntTest(jsonDoc, "SessionId", 12345);
		JsonGetStringTest(jsonDoc, "PokerHand", nullptr);
		JsonGetObjectTest(jsonDoc, "PatronInformation", false);
	}

	/// <summary>
	/// Functional test for:
	///     CProgressiveMessageFactory::CreateCoinIn5()
	/// Test where the accounting denomination is different than the current player denomination.
	/// </summary>
	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateGameEnd5_MultiDenom)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		CGame* currentGame(new CGame(MEMORY_NONE, true));
		int accountingDenom(5);
		int currentPlayerDenom(10 * accountingDenom);
		currentGame->SetDenom(accountingDenom);
		currentGame->SetGameNumber(2);
		currentGame->SetTheoreticalPayback(75);

		const long lastWinAmount = 3;

		CGameEndData gameData(currentGame, 25, 1000, lastWinAmount, NULL, currentPlayerDenom, 10);
		string jsonStr = factory.CreateGameEnd5(gameData, nullptr, 12345);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetUintTest(jsonDoc, "GameNumber", 2);
		JsonGetCurrencyTest(jsonDoc, "Denomination", currentPlayerDenom);
		JsonGetCurrencyTest(jsonDoc, "Win", lastWinAmount * accountingDenom);
		JsonGetIntTest(jsonDoc, "SessionId", 12345);
		JsonGetStringTest(jsonDoc, "PokerHand", nullptr);
		JsonGetObjectTest(jsonDoc, "PatronInformation", false);
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreatePayout)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		CPayout * payout = new CPayout(200, AttendantPaidProgressiveJackpot);

		
		string jsonStr = factory.CreatePayout(payout, nullptr, 12345);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetCurrencyTest(jsonDoc, "JackpotAmount", 200);
		JsonGetIntTest(jsonDoc, "SessionId", 12345);
		JsonGetStringTest(jsonDoc, "JackpotType", "AttendantPaidProgressiveJackpot");
		JsonGetObjectTest(jsonDoc, "PatronInformation", false);
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreatePayoutMisc)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		CPayout* payout = new CPayout(200, AllJackpots);


		string jsonStr = factory.CreatePayout(payout, nullptr, 12345);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetCurrencyTest(jsonDoc, "JackpotAmount", 200);
		JsonGetIntTest(jsonDoc, "SessionId", 12345);
		JsonGetStringTest(jsonDoc, "JackpotType", "MiscJackpot");
		JsonGetObjectTest(jsonDoc, "PatronInformation", false);
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreatePayoutNull)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		
		string jsonStr = factory.CreatePayout(nullptr, nullptr, 12345);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_TRUE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateCardStatus)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		ProgressiveLevelTestHelpers::_ICardConfig5 config5;
		rapidjson::Document jsonDoc;
		config5.CreateJsonDoc(jsonDoc);

		factory.UpdateCardConfiguration(jsonDoc);

		CPlayer *player = new CPlayer();

		int sessionId = 456789;
		int *gameNumber(nullptr);
		__int64 *denomination(nullptr);
		string cardStatusString = factory.CreateICardStatus(player, sessionId, true, true, true, gameNumber, denomination, 0, 0, 0, 0, "");

		bool result = jsonDoc.Parse(cardStatusString.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetBoolTest(jsonDoc, pKeyPollingActive, true);
		JsonGetBoolTest(jsonDoc, pKeySessionActive, true);
		JsonGetBoolTest(jsonDoc, pKeyMulticastActive, true);
		JsonGetIntTest(jsonDoc, pKeySessionId, sessionId);
		JsonGetIntTest(jsonDoc, pKeyGameNumber, gameNumber);
		JsonGetCurrencyTest(jsonDoc, pKeyDenomination, denomination);
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateCardStatus_PollingActiveFalse)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		ProgressiveLevelTestHelpers::_ICardConfig5 config5;
		rapidjson::Document jsonDoc;
		config5.CreateJsonDoc(jsonDoc);

		factory.UpdateCardConfiguration(jsonDoc);

		CPlayer *player = new CPlayer();

		int sessionId = 456789;
		int *gameNumber(nullptr);
		__int64 *denomination(nullptr);
		string cardStatusString = factory.CreateICardStatus(player, sessionId, false, true, true, gameNumber, denomination, 0, 0, 0, 0, "");

		bool result = jsonDoc.Parse(cardStatusString.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetBoolTest(jsonDoc, pKeyPollingActive, false);
		JsonGetBoolTest(jsonDoc, pKeySessionActive, true);
		JsonGetBoolTest(jsonDoc, pKeyMulticastActive, true);
		JsonGetIntTest(jsonDoc, pKeySessionId, sessionId);
		JsonGetIntTest(jsonDoc, pKeyGameNumber, gameNumber);
		JsonGetCurrencyTest(jsonDoc, pKeyDenomination, denomination);
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateCardStatus_SessionActiveFalse)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		ProgressiveLevelTestHelpers::_ICardConfig5 config5;
		rapidjson::Document jsonDoc;
		config5.CreateJsonDoc(jsonDoc);

		factory.UpdateCardConfiguration(jsonDoc);

		CPlayer *player = new CPlayer();

		int sessionId = 456789;
		int *gameNumber(nullptr);
		__int64 *denomination(nullptr);
		string cardStatusString = factory.CreateICardStatus(player, sessionId, true, false, true, gameNumber, denomination, 0, 0, 0, 0, "");

		bool result = jsonDoc.Parse(cardStatusString.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetBoolTest(jsonDoc, pKeyPollingActive, true);
		JsonGetBoolTest(jsonDoc, pKeySessionActive, false);
		JsonGetBoolTest(jsonDoc, pKeyMulticastActive, true);
		JsonGetIntTest(jsonDoc, pKeySessionId, sessionId);
		JsonGetIntTest(jsonDoc, pKeyGameNumber, gameNumber);
		JsonGetCurrencyTest(jsonDoc, pKeyDenomination, denomination);
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateCardStatus_MulticastActiveFalse)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		ProgressiveLevelTestHelpers::_ICardConfig5 config5;
		rapidjson::Document jsonDoc;
		config5.CreateJsonDoc(jsonDoc);

		factory.UpdateCardConfiguration(jsonDoc);

		CPlayer *player = new CPlayer();

		int sessionId = 456789;
		int *gameNumber(nullptr);
		__int64 *denomination(nullptr);
		string cardStatusString = factory.CreateICardStatus(player, sessionId, true, true, false, gameNumber, denomination, 0, 0, 0, 0, "");

		bool result = jsonDoc.Parse(cardStatusString.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetBoolTest(jsonDoc, pKeyPollingActive, true);
		JsonGetBoolTest(jsonDoc, pKeySessionActive, true);
		JsonGetBoolTest(jsonDoc, pKeyMulticastActive, false);
		JsonGetIntTest(jsonDoc, pKeySessionId, sessionId);
		JsonGetIntTest(jsonDoc, pKeyGameNumber, gameNumber);
		JsonGetCurrencyTest(jsonDoc, pKeyDenomination, denomination);
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateCardStatus_GameNumber)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		ProgressiveLevelTestHelpers::_ICardConfig5 config5;
		rapidjson::Document jsonDoc;
		config5.CreateJsonDoc(jsonDoc);

		factory.UpdateCardConfiguration(jsonDoc);

		CPlayer *player = new CPlayer();

		int sessionId = 456789;
		int gameNumberValue(3456);
		int *gameNumber(&gameNumberValue);
		__int64 *denomination(nullptr);
		string cardStatusString = factory.CreateICardStatus(player, sessionId, true, true, true, gameNumber, denomination, 0, 0, 0, 0, "");

		bool result = jsonDoc.Parse(cardStatusString.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetBoolTest(jsonDoc, pKeyPollingActive, true);
		JsonGetBoolTest(jsonDoc, pKeySessionActive, true);
		JsonGetBoolTest(jsonDoc, pKeyMulticastActive, true);
		JsonGetIntTest(jsonDoc, pKeySessionId, sessionId);
		JsonGetIntTest(jsonDoc, pKeyGameNumber, gameNumber);
		JsonGetCurrencyTest(jsonDoc, pKeyDenomination, denomination);
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateCardStatus_Denomination)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		ProgressiveLevelTestHelpers::_ICardConfig5 config5;
		rapidjson::Document jsonDoc;
		config5.CreateJsonDoc(jsonDoc);

		factory.UpdateCardConfiguration(jsonDoc);

		CPlayer *player = new CPlayer();

		int sessionId = 456789;
		int *gameNumber(nullptr);
		__int64 denominationValue(1);
		__int64 *denomination(&denominationValue);
		string cardStatusString = factory.CreateICardStatus(player, sessionId, true, true, true, gameNumber, denomination, 0, 0, 0, 0, "");

		bool result = jsonDoc.Parse(cardStatusString.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetBoolTest(jsonDoc, pKeyPollingActive, true);
		JsonGetBoolTest(jsonDoc, pKeySessionActive, true);
		JsonGetBoolTest(jsonDoc, pKeyMulticastActive, true);
		JsonGetIntTest(jsonDoc, pKeySessionId, sessionId);
		JsonGetIntTest(jsonDoc, pKeyGameNumber, gameNumber);
		JsonGetCurrencyTest(jsonDoc, pKeyDenomination, denomination);
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateCardStatus_GameNumberDenomination)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		ProgressiveLevelTestHelpers::_ICardConfig5 config5;
		rapidjson::Document jsonDoc;
		config5.CreateJsonDoc(jsonDoc);

		factory.UpdateCardConfiguration(jsonDoc);

		CPlayer *player = new CPlayer();

		int sessionId = 456789;
		int gameNumberValue(0);
		int *gameNumber(&gameNumberValue);
		__int64 denominationValue(50000);
		__int64 *denomination(&denominationValue);
		string cardStatusString = factory.CreateICardStatus(player, sessionId, true, true, true, gameNumber, denomination, 0, 0, 0, 0, "");

		bool result = jsonDoc.Parse(cardStatusString.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetBoolTest(jsonDoc, pKeyPollingActive, true);
		JsonGetBoolTest(jsonDoc, pKeySessionActive, true);
		JsonGetBoolTest(jsonDoc, pKeyMulticastActive, true);
		JsonGetIntTest(jsonDoc, pKeySessionId, sessionId);
		JsonGetIntTest(jsonDoc, pKeyGameNumber, gameNumber);
		JsonGetCurrencyTest(jsonDoc, pKeyDenomination, denomination);
	}

	/// <summary>
	/// Functional test for:
	///     CProgressiveMessageFactory::CreateJackpotPaid()
	/// Test with default StatusInfo value.
	/// </summary>
	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateJackpotPaid)
	{
		CConnection connection;
		connection.SetSlotNumber(9);
		CProgressiveMessageFactory factory(connection);

		CEGMAward egmAward;
		egmAward.SetLevelId(1);
		egmAward.SetJackpotId(2);
		egmAward.SetRemoteJackpotId(3);
		egmAward.SetAwardAmountPaid(4);
		egmAward.SetAwardState(AwardToGameSuccess);
		egmAward.SetStatusInfo(_T(""));

		string jsonStr = factory.CreateJackpotPaid(egmAward);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetIntTest(jsonDoc, "LevelId", 1);
		JsonGetInt64Test(jsonDoc, "JackpotId", 2);
		JsonGetInt64Test(jsonDoc, "RemoteJackpotId", 3);
		JsonGetStringTest(jsonDoc, "Status", "Paid");
		JsonGetCurrencyTest(jsonDoc, "Amount", 4);
		JsonGetIntTest(jsonDoc, "WinningAssetNumber", 9);
	}

	/// <summary>
	/// Functional test for:
	///     CProgressiveMessageFactory::CreateJackpotPaid()
	/// Test with non-default StatusInfo value.
	/// </summary>
	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateJackpotPaid_StatusInfo)
	{

		CConnection connection;
		connection.SetSlotNumber(9);
		CProgressiveMessageFactory factory(connection);

		CEGMAward egmAward;
		egmAward.SetLevelId(1);
		egmAward.SetJackpotId(2);
		egmAward.SetRemoteJackpotId(3);
		egmAward.SetAwardAmountPaid(4);
		egmAward.SetAwardState(AwardToGameSuccess);
		egmAward.SetStatusInfo(_T("Transaction successful"));

		string jsonStr = factory.CreateJackpotPaid(egmAward);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetIntTest(jsonDoc, "LevelId", 1);
		JsonGetInt64Test(jsonDoc, "JackpotId", 2);
		JsonGetInt64Test(jsonDoc, "RemoteJackpotId", 3);
		JsonGetStringTest(jsonDoc, "Status", "Paid");
		JsonGetStringTest(jsonDoc, "StatusInfo", "Transaction successful");
		JsonGetCurrencyTest(jsonDoc, "Amount", 4);
		JsonGetIntTest(jsonDoc, "WinningAssetNumber", 9);
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateCoinIn5)
	{
		CGame* pGame = new CGame(MEMORY_NONE, true);
		pGame->SetDenom(13);
		pGame->SetGameNumber(27);
		pGame->SetTheoreticalPayback(75);

		const long LastPlayedAmount = 25;
		const __int64 CoinIn = 1000;

		CGameEndData gameData(pGame, LastPlayedAmount, CoinIn, 0, NULL, 13, 10);

		CPlayer somePlayer;

		CConnection connection;
		connection.SetSlotNumber(9);
		CProgressiveMessageFactory factory(connection);

		string jsonStr = factory.CreateCoinIn5(gameData, &somePlayer, 456789);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetUintTest(jsonDoc, "GameNumber", 27);
		JsonGetCurrencyTest(jsonDoc, "Wager", LastPlayedAmount * 13);
		JsonGetCurrencyTest(jsonDoc, "Restricted", CoinIn * 13);
		JsonGetCurrencyTest(jsonDoc, "Denomination", 13);
		JsonGetDoubleTest(jsonDoc, "BasePointsEarned", 10);
		JsonExistsTest(jsonDoc, "PokerHand", false);
		JsonGetIntTest(jsonDoc, "SessionId", 456789);
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateCoinIn5_JokerHand)
	{
		CGame* pGame = new CGame(MEMORY_NONE, true);
		pGame->SetDenom(13);
		pGame->SetGameNumber(27);
		pGame->SetTheoreticalPayback(75);

		const long LastPlayedAmount = 25;
		const __int64 CoinIn = 1000;

		CGameEndData gameData(pGame, LastPlayedAmount, CoinIn, 0, NULL, 13, 10);

		PokerHand& pokerHand = (PokerHand&)gameData.GetPokerHand();

		static byte cards[PokerHand::CardsInHand] = { 0x08, 0x09, 0x0a, 0x0b, 0x0c };
		pokerHand.Set(cards);

		CPlayer somePlayer;

		CConnection connection;
		connection.SetSlotNumber(9);
		CProgressiveMessageFactory factory(connection);

		set<DWORD> queryCards;
		queryCards.insert(pGame->GetGameNumber());
		factory.SetQueryPokerCardsSet(queryCards);

		string jsonStr = factory.CreateCoinIn5(gameData, &somePlayer, 456789);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetUintTest(jsonDoc, "GameNumber", 27);
		JsonGetCurrencyTest(jsonDoc, "Wager", LastPlayedAmount * 13);
		JsonGetCurrencyTest(jsonDoc, "Restricted", CoinIn * 13);
		JsonGetCurrencyTest(jsonDoc, "Denomination", 13);
		JsonGetStringTest(jsonDoc, "PokerHand", "TS JS QS KS AS");
		JsonGetIntTest(jsonDoc, "SessionId", 456789);
	}

	/// <summary>
	/// Functional test for:
	///     CProgressiveMessageFactory::CreateCoinIn5()
	/// Test where the accounting denomination is different than the current player denomination.
	/// </summary>
	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateCoinIn5_MultiDenom)
	{
		CGame* pGame = new CGame(MEMORY_NONE, true);
		int accountingDenom(5);
		int currentPlayerDenom(10 * accountingDenom);
		pGame->SetDenom(accountingDenom);
		pGame->SetGameNumber(27);
		pGame->SetTheoreticalPayback(75);

		const long LastPlayedAmount = 25;
		const __int64 CoinIn = 1000;

		CGameEndData gameData(pGame, LastPlayedAmount, CoinIn, 0, NULL, currentPlayerDenom, 10);

		CPlayer somePlayer;

		CConnection connection;
		connection.SetSlotNumber(9);
		CProgressiveMessageFactory factory(connection);

		string jsonStr = factory.CreateCoinIn5(gameData, &somePlayer, 456789);
		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetUintTest(jsonDoc, "GameNumber", 27);
		JsonGetCurrencyTest(jsonDoc, "Wager", LastPlayedAmount * accountingDenom);
		JsonGetCurrencyTest(jsonDoc, "Restricted", CoinIn * accountingDenom);
		JsonGetCurrencyTest(jsonDoc, "Denomination", currentPlayerDenom);
		JsonGetDoubleTest(jsonDoc, "BasePointsEarned", 10);
		JsonExistsTest(jsonDoc, "PokerHand", false);
		JsonGetIntTest(jsonDoc, "SessionId", 456789);
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateICardEvent)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		const CSASGeneralException SASGeneralException;
		const std::string egmEventCodeExpected(to_string(GPE_SLOTDOOR_OPENED));
		const std::string eventDescriptionExpected(SASGeneralException.GetDescriptionFor(GPE_SLOTDOOR_OPENED));

		string jsonStr = factory.CreateICardEvent(GPE_SLOTDOOR_OPENED);

		string str = OUTPUTDEBUG(jsonStr);
		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetStringTest(jsonDoc, "EGMProtocolType", pValSAS);
		JsonGetInt64Test(jsonDoc, "EGMEventCode", (int64_t)GPE_SLOTDOOR_OPENED);
		JsonGetStringTest(jsonDoc, "EventDescription", eventDescriptionExpected.c_str());
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateProgressiveLevelHitTest)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);
		CEGMAward egmAward;
		egmAward.SetLevelId(3);
		egmAward.SetAwardAmountHit(4);

		string jsonStr = factory.CreateProgressiveLevelHit(egmAward);
		string str = OUTPUTDEBUG(jsonStr);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetInt64Test(jsonDoc, pKeyLevelId, egmAward.GetLevelId());
		JsonGetCurrencyTest(jsonDoc, pKeyHitValue, egmAward.GetAwardAmountHit());
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateOfferAcceptanceResponseTest)
	{
		string OfferRequestJsonStr("{\"JackpotId\":2951, \"OfferId\":\"6max\",\"OfferName\":\"DCFreeBuffet\",\"PlayerId\":163807,\"LevelName\":\"LouMajorOffer3\",\"IsMajor\":true,\"TransactionId\":\"368\",\"SourceDeviceId\":92,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1577389505929}");
		string expectedOfferAcceptanceResponse("{\"JackpotId\":2951,\"PlayerId\":163807,\"OfferId\":\"6max\",\"Accepted\":true,\"TransactionId\":\"368\"");

		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(OfferRequestJsonStr.c_str()).HasParseError();

		CProgressiveOfferRequest progressiveOfferRequest(jsonDoc);
		string actualAcceptanceResponse(factory.CreateOfferAcceptanceResponse(progressiveOfferRequest));



		ASSERT_TRUE(actualAcceptanceResponse.find(expectedOfferAcceptanceResponse) != string::npos);
	}	

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateOfflineCoinInEvent)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		__int64 COIN_IN_VALUE(969);
		std::stringstream stream;
		stream << COIN_IN_VALUE;
		std::string strValue(stream.str());
		string jsonStr = factory.CreateOfflineCoinInEvent(COIN_IN_VALUE);

		string str = OUTPUTDEBUG(jsonStr);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		JsonGetStringTest(jsonDoc, "Type", pValOfflineCoinInType);
		JsonGetStringTest(jsonDoc, "Data", strValue.c_str());
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_SetActualIP)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		const string actualIP("1.2.3.4");

		bool changed = factory.SetActualIP(actualIP);
		ASSERT_TRUE(changed) << "Ip Didnt change ";

		changed = factory.SetActualIP(actualIP);
		ASSERT_FALSE(changed) << "Ip changed";
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_GetActualIP)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		const string actualIP("1.2.3.4");
		factory.SetActualIP(actualIP);
		auto rval = factory.GetActualIP();
		ASSERT_STREQ("1.2.3.4", rval.c_str()) << "Ip not the same";
	}

	// These tests are very specific to how the message factory writes the JSON string.

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_WriteGameData)
	{
		f_ProgressiveMessageFactory_WriteGameData();
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_WriteGameData_empty)
	{
		f_ProgressiveMessageFactory_WriteGameData_empty();
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateICardConfigurationRequest5_empty)
	{
		f_ProgressiveMessageFactory_CreateICardConfigurationRequest5_empty();

	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateConfigFileDownloadAck)
	{
		f_ProgressiveMessageFactory_CreateConfigFileDownloadAck();
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateICardConfigurationRequest5_1game)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		CGames games;

		CGame* pGame = new CGame(MEMORY_NONE, false);
		pGame->SetGameNumber(1);
		pGame->SetDenom(1, DenomCurrencyType::US_DOLLARS);
		pGame->SetMaxBet(250);
		pGame->SetTheoreticalPayback((WORD)8782);
		pGame->SetMfgID("AT");
		pGame->SetPaytableID("906   ");
		games.AddGame(pGame);

		const char* pExpected = R"({"Games":[{"GameNumber":1,"Denomination":0.01,"MaxBetCredits":250,"TheoPercentage":87.82,"GameName":"AT","Paytable":"906   ","DiscoveredLevelIds":""}],"SASDiscovery":)";
		const char* pExpectedEnd = strstr(pExpected, R"("SASDiscovery":)");

		string rval = factory.CreateICardConfigurationRequest5(&games);
		auto rvalEnd = rval.find(R"("SASDiscovery":)");

		ASSERT_EQ(0, strncmp(pExpected, rval.c_str(), rvalEnd)) << "strncmp";
		ASSERT_EQ((size_t)(pExpectedEnd - pExpected), (size_t)rvalEnd) << "Length";
	}

	TEST_F(ProgressiveMessageFactoryUnitTests, ProgressiveMessageFactory_CreateICardConfigurationRequest5_2games)
	{
		CConnection connection;
		CProgressiveMessageFactory factory(connection);

		CGames games;

		CGame* pGame1 = new CGame(MEMORY_NONE, false);
		pGame1->SetGameNumber(1);
		pGame1->SetDenom(1, DenomCurrencyType::US_DOLLARS);
		pGame1->SetMaxBet(250);
		pGame1->SetTheoreticalPayback((WORD)8782);
		pGame1->SetMfgID("AT");
		pGame1->SetPaytableID("906   ");
		games.AddGame(pGame1);

		CGame* pGame2 = new CGame(MEMORY_NONE, false);
		pGame2->SetGameNumber(3);
		pGame2->SetDenom(5, DenomCurrencyType::US_DOLLARS);
		pGame2->SetMaxBet(250);
		pGame2->SetTheoreticalPayback((WORD)8782);
		pGame2->SetMfgID("AT");
		pGame2->SetPaytableID("906   ");
		games.AddGame(pGame2);

		string expected(R"({"Games":[{"GameNumber":1,"Denomination":0.01,"MaxBetCredits":250,"TheoPercentage":87.82,"GameName":"AT","Paytable":"906   ","DiscoveredLevelIds":""},{"GameNumber":3,"Denomination":0.05,"MaxBetCredits":250,"TheoPercentage":87.82,"GameName":"AT","Paytable":"906   ","DiscoveredLevelIds":""}],"SASDiscovery":{"Address":1,"GameId":"","SASVersion":"","SerialNumber":"","AssetNumber":"0","AccountingDenomination":0.0,"MultiDenomEnabled":false,"ExtendedMetersEnabled":false,"AFTEnabled":false,"AFTBonusEnabled":false,"AFTInHouseEnabled":false,"LegacyBonusEnabled":false,"EFTTransferToEnabled":false,"EFTTransferFromEnabled":false,"ValidationExtensionsSupported":false,"ProgressiveGroup":0},"Features":{"Playlists":true,"RemoteReboot":true,"PromoGames":true,"PrizeJackpot":true,"SlotsContent":true,"DownloadableFirmware":true,"DownloadableConfigFiles":true,"Campaigns":true,"Skins":true},"DeviceMAC":"","ImplementationVersion":"0.0.0.0","OSVersion":"","Platform":"NCOMPASS-PT206","PlatformFirmware":[{"Name":"os.fb","Version":""},{"Name":"exe.fb","Version":"0.0.0.0"},{"Name":"fpga.fb","Version":""}],"IPAddress":"0.0.0.0","ManagementURL":"http://0.0.0.0","SourceDeviceConfigVersion":0,"SourceDeviceId":0,"SourceDevice":"","SourceAsset":"")");
		string rval = factory.CreateICardConfigurationRequest5(&games);
		string PATTERN_TO_DELETE(R"(,"Timestamp")");
		size_t posToDeleteFrom = rval.find(PATTERN_TO_DELETE);
		rval.erase(posToDeleteFrom, rval.size() - posToDeleteFrom);
		ASSERT_TRUE(expected == rval) << "Game data not same";
	}
}
