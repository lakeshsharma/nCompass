#if 000//TODO-PORT
#include "stdafx.h"
#include "ProgressiveConnectionMock.h"
#include "ProgressiveDataConsumerMock.h"
#include "ProgressiveLevelTestHelpers.h"
#include "Sentinel.h"
#include "UnitTestingToString.h"
#include "UnitTestingUtils.h"
#include "TestUtil.h"
#include "RapidJsonHelper.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace activemq::commands;
using namespace cms;
using ::testing::Return;
using ::testing::Mock;
using namespace TestUtil;
using namespace SentinelNativeUnitTests::ProgressiveLevelTestHelpers;

const CString paltronicsCertificatePath("Paltronics.crt");
const LPCTSTR ENDPOINT1 = L"1.1.1.1";
const LPCTSTR ENDPOINT2 = L"1.1.1.2";
const DWORD PING_INTERVAL = 54321;
const long VALID_SLOT_MAST_ID = 1;

namespace SentinelNativeUnitTests
{
	static unique_ptr<DeleteOnCloseDir> playlistDir;
	static CStringA playlist_Url;
	static CString  playlist_PathUrl;
	static CString  playlist_PathContent;
	static CString  playlist_PathTempContent;

	TEST_CLASS(ProgressiveConnectionUnitTests)
	{
		static const char* pICardConfig5_GameKing;

		/// <summary>
		/// Classes the setup once.
		/// </summary>
		TEST_CLASS_INITIALIZE(ProgressiveConnectionUnitTests_Init)
		{
			int argc = 0;
			char ** argv = 0;
			::testing::InitGoogleMock(&argc, argv);
			::testing::GTEST_FLAG(throw_on_failure) = true;

			CDirectoryManager::MockInstance(NULL);
			playlistDir.reset(new DeleteOnCloseDir(L"~PlayList"));

			playlist_PathUrl += playlistDir->GetName() + L"\\URL";
			::CreateDirectory(playlist_PathUrl, nullptr);

			for (int i = 1; i <= 7; i++)
			{
				CString filename;
				filename.Format(L"File%d.txt", i);

				CString pathname(playlist_PathUrl + L'\\' + filename);
				CFile file(pathname, CFile::modeCreate | CFile::modeWrite);

				CStringA filenameA(filename);
				file.Write((LPCSTR) filenameA, filenameA.GetLength());
			}

			playlist_PathContent = playlistDir->GetName() + L"Content";
			CDirectoryManager::MockSetPath(DIRECTORY_PLCONTENT, playlist_PathContent);
			::CreateDirectory(playlist_PathContent, nullptr);

			playlist_PathTempContent = playlistDir->GetName() + L"TempContent";
			CDirectoryManager::MockSetPath(DIRECTORY_PLCONTENTTEMP, playlist_PathTempContent);
			::CreateDirectory(playlist_PathTempContent, nullptr);

			playlist_Url = "file:///";
			playlist_Url += playlist_PathUrl;
			playlist_Url.Replace("\\", "/");
		}

		TEST_CLASS_CLEANUP(ProgressiveConnectionUnitTests_Deinit)
		{
			playlistDir.reset();
		}

		/// <summary>
		/// Setups this instance.
		/// </summary>
		TEST_METHOD_INITIALIZE(setup)
		{
		}

		/// <summary>
		/// ProgressiveConnectionConstructorConnectionStateWaitingForInfoUnitTest
		/// </summary>
		TEST_METHOD(ProgressiveConnectionConstructorConnectionStateWaitingForInfoUnitTest)
		{
			//Setup
			CProgressiveConfig progressiveConfig;
			progressiveConfig.SetActive(true);
			progressiveConfig.AddEndpoint(ENDPOINT1);
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			progressiveConnection.LoadProgressiveConfig(&progressiveConfig);

			//Test
			Assert::IsTrue(WaitingForInfo == progressiveConnection.GetConnectionState());
		}

		/// <summary>
		/// ProgressiveConnectionConstructorConnectionStateDisabledUnitTest
		/// </summary>
		TEST_METHOD(ProgressiveConnectionConstructorConnectionStateDisabledUnitTest)
		{
			//Setup
			CProgressiveConfig progressiveConfig;
			progressiveConfig.SetActive(false);
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			progressiveConnection.LoadProgressiveConfig(&progressiveConfig);

			//Test
			Assert::IsTrue(Disabled == progressiveConnection.GetConnectionState());
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&dataConsumer));
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		/// <summary>
		/// ProgressiveConnectionLoadProgressiveConfigUnitTest
		/// </summary>
		TEST_METHOD(ProgressiveConnectionLoadProgressiveConfigUnitTest)
		{
			//Setup
			CProgressiveConfig progressiveConfig;
			progressiveConfig.SetPingInterval(PING_INTERVAL);
			progressiveConfig.AddEndpoint(ENDPOINT1);
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			progressiveConnection.LoadProgressiveConfig(&progressiveConfig);

			//Tests
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			Assert::IsTrue((PING_INTERVAL / 1000) * 1000 == progressiveConnection.m_connectionInfo.m_pingInterval);
		}

		/// <summary>
		/// ProgressiveConnectionLoadDeviceConnectionDataUnitTest
		/// </summary>
		TEST_METHOD(ProgressiveConnectionLoadDeviceConnectionDataUnitTest)
		{
			//Setup
			CConnection connection;
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			
			//Tests
			Assert::IsFalse(progressiveConnection.IsMessageFactoryCreated());
			progressiveConnection.LoadDeviceConnectionData(&connection);
			Assert::IsTrue(progressiveConnection.IsMessageFactoryCreated());
		}

		/// <summary>
		/// ProgressiveConnectionSetGamesUnitTest
		/// </summary>
		TEST_METHOD(ProgressiveConnectionSetGamesUnitTest)
		{
			long long COIN_OUT_CREDITS = 969;
			//Setup
			CGames games;
			CGame* game(new CGame(MEMORY_NONE, true));
			game->SetMeter(gCO, COIN_OUT_CREDITS);
			games.AddGame(game);
			CConnection connection;
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			progressiveConnection.LoadDeviceConnectionData(&connection);			

			//Tests
			Assert::IsFalse(progressiveConnection.IsGamesCreated());
			progressiveConnection.SetGames(&games, nullptr);
			Assert::IsTrue(progressiveConnection.IsGamesCreated());
			progressiveConnection.SetGames(&games, nullptr);
			Assert::IsTrue(progressiveConnection.IsGamesCreated());
		}
		
		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetGames()
		/// Test that m_haveGameData is set correctly.
		/// Test that m_currentGameNumber is set correctly.
		/// Test that m_currentDenomination is set correctly.
		/// </summary>
		TEST_METHOD(ProgressiveConnectionSetGamesGameDataUnitTest)
		{
			long long COIN_OUT_CREDITS = 969;
			//Setup
			CGames games;
			CGame* game(new CGame(MEMORY_NONE, true));
			game->SetMeter(gCO, COIN_OUT_CREDITS);
			games.AddGame(game);
			CConnection connection;
			CProgressiveConfig progressiveConfig;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			progressiveConnection.LoadDeviceConnectionData(&connection);

			const int currentGameNumber(789);
			const __int64 currentDenomination(15);
			CGame *currentGame(new CGame(MEMORY_NONE, true));
			currentGame->SetGameNumber((DWORD)currentGameNumber);
			currentGame->SetDenom((int)currentDenomination);
			unique_ptr<CGameEndData> gameEndData(new CGameEndData(currentGame, 0, 0, 0, nullptr, 1));

			//Tests
			progressiveConnection.SetGames(&games, gameEndData.get());
			Assert::IsTrue(progressiveConnection.m_haveGameData, L"m_haveGameData");
			Assert::AreEqual(currentGameNumber, progressiveConnection.m_currentGameNumber, L"m_gameNumber,");
		}

		/// <summary>
		/// ProgressiveConnectionTimedEventsUnitTest
		/// </summary>
		TEST_METHOD(ProgressiveConnectionTimedEventsUnitTest)
		{
			//Setup
			CProgressiveConfig progressiveConfig;
			progressiveConfig.AddEndpoint(ENDPOINT1);
			progressiveConfig.AddEndpoint(ENDPOINT2);
			CGames games;
			CConnection connection;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			
			//Tests
			//(m_messageFactory == NULL || m_games == NULL) == true
			Assert::IsTrue(Disabled == progressiveConnection.GetConnectionState());
			progressiveConnection.TimedEvents();
			Assert::IsTrue(WaitingForInfo == progressiveConnection.GetConnectionState());

			//(m_connectionInfo.GetState() == WaitingForInfo && m_messageFactory != NULL && m_games != NULL) == true
			progressiveConnection.LoadDeviceConnectionData(&connection);
			progressiveConnection.SetGames(&games, nullptr);
			ON_CALL(progressiveConnection, IsTimeForConnectionAttempt()).WillByDefault(Return(false));			
			progressiveConnection.TimedEvents();
			Assert::IsTrue(Disconnected == progressiveConnection.GetConnectionState());

			//Time to make a connection
			ON_CALL(progressiveConnection, IsTimeForConnectionAttempt()).WillByDefault(Return(true));
			EXPECT_CALL(progressiveConnection, WatchdogCheckIn()).Times(5);
			progressiveConnection.TimedEvents();
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			Assert::IsTrue(InDiscovery == progressiveConnection.GetConnectionState());

			//Receive ICardConfiguration5 as a result of sending ICardConfigurationRequest5

			ProgressiveLevelTestHelpers::_ICardConfig5 config5;
			std::string messageBody = config5.CreateJson();

			ON_CALL(progressiveConnection, CreateBytesMessage()).WillByDefault(Return(new ActiveMQBytesMessage()));
			BytesMessage* bytesMessage(progressiveConnection.CreateMockBytesMessage(FromOneLink::pICardConfiguration5, messageBody));
			ON_CALL(progressiveConnection, GetMessageBody(bytesMessage)).WillByDefault(Return(messageBody));
			bytesMessage->reset();
			progressiveConnection.onMessage(bytesMessage);
			delete bytesMessage;
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
			Assert::AreEqual((int) Connected, (int) progressiveConnection.GetConnectionState());

			//SendPing and Time For Address Check 
			ON_CALL(progressiveConnection, IsTimeForConnectionAttempt()).WillByDefault(Return(false));
			ON_CALL(progressiveConnection, IsTimeForRediscovery()).WillByDefault(Return(false));		
			ON_CALL(progressiveConnection, IsTimeForAddressCheck()).WillByDefault(Return(true));
			EXPECT_CALL(progressiveConnection, AddressCheck()).Times(1);
			ON_CALL(progressiveConnection, CreateBytesMessage()).WillByDefault(Return(new ActiveMQBytesMessage()));
			EXPECT_CALL(progressiveConnection, WatchdogCheckIn()).Times(5);
			progressiveConnection.TimedEvents();
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));

			// Set to disabled so connection shuts down
			progressiveConnection.UpdateConnectionState(Disabled);
		}

		/// <summary>
		/// ProgressiveConnectionOnMessageUnitTest
		/// </summary>
		TEST_METHOD(ProgressiveConnectionOnMessageUnitTest)
		{
			//Setup
			string messageBody;
			BytesMessage* byteMessage(NULL);
			CProgressiveConfig progressiveConfig;
			progressiveConfig.AddEndpoint(ENDPOINT1);
			progressiveConfig.AddEndpoint(ENDPOINT2);
			CGames games;
			CConnection connection;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			progressiveConnection.LoadDeviceConnectionData(&connection);

			//Tests Receive Messages 

			//DiscoveryEvent

			messageBody.clear();
			ON_CALL(progressiveConnection, CreateBytesMessage()).WillByDefault(Return(new ActiveMQBytesMessage()));
			byteMessage = progressiveConnection.CreateMockBytesMessage(FromOneLink::pDiscoveryEvent, messageBody);
			ON_CALL(progressiveConnection, GetMessageBody(byteMessage)).WillByDefault(Return(messageBody));
			progressiveConnection.onMessage(byteMessage);
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));

			//JackpotHit

			messageBody.clear();
			ON_CALL(progressiveConnection, CreateBytesMessage()).WillByDefault(Return(new ActiveMQBytesMessage()));
			byteMessage = progressiveConnection.CreateMockBytesMessage(FromOneLink::pJackpotHit, messageBody);
			ON_CALL(progressiveConnection, GetMessageBody(byteMessage)).WillByDefault(Return(messageBody));
			progressiveConnection.onMessage(byteMessage);
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		TEST_METHOD(ProgressiveConnectionOnMessageUnitTest_ICardConfiguration)
		{
			//Setup
			string messageBody;
			BytesMessage* byteMessage(NULL);
			CProgressiveConfig progressiveConfig;
			progressiveConfig.AddEndpoint(ENDPOINT1);
			progressiveConfig.AddEndpoint(ENDPOINT2);
			CGames games;
			CConnection connection;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			progressiveConnection.LoadDeviceConnectionData(&connection);

			//Tests Receive Messages 

			//DiscoveryEvent

			messageBody.clear();
			ON_CALL(progressiveConnection, CreateBytesMessage()).WillByDefault(Return(new ActiveMQBytesMessage()));
			byteMessage = progressiveConnection.CreateMockBytesMessage(FromOneLink::pDiscoveryEvent, messageBody);
			ON_CALL(progressiveConnection, GetMessageBody(byteMessage)).WillByDefault(Return(messageBody));
			progressiveConnection.onMessage(byteMessage);
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));

			//JackpotHit

			messageBody.clear();
			ON_CALL(progressiveConnection, CreateBytesMessage()).WillByDefault(Return(new ActiveMQBytesMessage()));
			byteMessage = progressiveConnection.CreateMockBytesMessage(FromOneLink::pICardConfiguration5, pICardConfig5_GameKing);
			ON_CALL(progressiveConnection, GetMessageBody(byteMessage)).WillByDefault(Return(messageBody));
			progressiveConnection.onMessage(byteMessage);
			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		/// <summary>
		/// ProgressiveConnectionSendMessagesUnitTest
		/// </summary>
		TEST_METHOD(ProgressiveConnectionSendMessagesUnitTest)
		{
			//Setup
			const ToOneLink::Type messageType = ToOneLink::ICardStatus;
			const string OCR = "A531000002";
			CGame* currentGame(new CGame(MEMORY_NONE, true));
			CGameEndData gameData(currentGame, 100, 200, 300, NULL, 1);
			CEGMAward egmAward;
			CPlayer player;

			string messageBody;
			BytesMessage* byteMessage(NULL);
			CProgressiveConfig progressiveConfig;
			CConnection connection;
			CProgressiveDataConsumerMock dataConsumer;
			CProgressiveConnectionMock progressiveConnection(dataConsumer, &progressiveConfig, paltronicsCertificatePath);
			progressiveConnection.LoadDeviceConnectionData(&connection);

			//Code coverage tests
			progressiveConnection.QueueToOneLink(new ProgressiveQueueToOneLink(messageType, "Body"), QUEUE_PRIORITY_HIGH);
			progressiveConnection.QueueToOneLink(new ProgressiveQueueToOneLink(messageType, "Body"), QUEUE_PRIORITY_HIGH);

			progressiveConnection.SendCoinIn(&gameData);
			progressiveConnection.SendGameEnd(&gameData);
			progressiveConnection.SendJackpotPaidRequest(egmAward);
			progressiveConnection.SendICardEvent(GPE_SLOTDOOR_OPENED);

			progressiveConnection.SetPlayer(&player, true, 0);
			progressiveConnection.SendCardStatus();

			Assert::IsTrue(Mock::VerifyAndClearExpectations(&progressiveConnection));
		}

		// This test exists to test specific json messages saved as files.
		// As such, it's usefulness as an automated test is limited and it
		// is normally commented out.
		/*
		TEST_METHOD(ProgressiveConnection_ProcessICardConfiguration5ish)
		{
			CStdioFile file(L"C:\\Temp\\Script1.json", CFile::modeRead);
			CStringA frame;
			UINT len = (UINT) file.GetLength();
			char* pFrame = frame.GetBuffer(len + 1);
			file.Read(pFrame, (UINT) file.GetLength());
			pFrame[len] = 0;

			rapidjson::Document jsonDoc;
			bool result = jsonDoc.ParseInsitu(pFrame).HasParseError();
			Assert::IsFalse(result, AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError())));

			const char* pMessageId = nullptr;

			try
			{
				pMessageId = JsonGetStringEx(jsonDoc, pKeyMessageId, nullptr);

				CConnection connection;
				CProgressiveMessageFactory factory(connection);

				factory.UpdateCardConfiguration(jsonDoc);

				vector<string> urls;
				unique_ptr<CProgressiveLevels> levels(new CProgressiveLevels(GetTickCount(), jsonDoc, &urls));
				
				const char* pOneLinkSystemId = JsonGetStringEx(jsonDoc, OneLinkHeader::pOneLinkSystemId);

				try
				{
					unique_ptr<CProgressivePlaylist> playlist(new CProgressivePlaylist(jsonDoc));
				}
				catch (JsonException& e)
				{
					Assert::Fail(AssertMsg::FormatOnly(L"Unexpected exception: %S: new CProgressivePlaylist: %S", pMessageId, e.what()));
				}
			}
			catch (SentinelException& e)
			{
				Assert::Fail(AssertMsg::FormatOnly(L"Unexpected exception: %S: %S", pMessageId ? pMessageId : "<pMessageId == nullptr>", e.what()));
			}
		}
		*/
	};

 const char* ProgressiveConnectionUnitTests::pICardConfig5_GameKing =
"{"
    "\"Games\": ["
        "{"
            "\"GameNumber\": 9,"
            "\"Denomination\": 0.05,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.17,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1003A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 2,"
            "\"Denomination\": 0.01,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.54,"
            "\"GameName\": \"Jacks Or Better\","
            "\"Paytable\": \"P1041A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 52,"
            "\"Denomination\": 0.03,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.35,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-L  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 52,"
            "\"Denomination\": 5.0,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.35,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-L  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 83,"
            "\"Denomination\": 1.0,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 96.16,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"001556\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 46,"
            "\"Denomination\": 0.25,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 31,"
            "\"Denomination\": 25.0,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.73,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1029A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 46,"
            "\"Denomination\": 0.5,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 52,"
            "\"Denomination\": 0.5,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.35,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-L  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 31,"
            "\"Denomination\": 0.03,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.73,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1029A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 73,"
            "\"Denomination\": 0.05,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 96.22,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"001555\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 73,"
            "\"Denomination\": 1.0,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 96.22,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"001555\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 31,"
            "\"Denomination\": 1.0,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.73,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1029A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 83,"
            "\"Denomination\": 0.25,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 96.16,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"001556\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 16,"
            "\"Denomination\": 25.0,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.11,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1035A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 83,"
            "\"Denomination\": 5.0,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 96.16,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"001556\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 83,"
            "\"Denomination\": 0.03,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 96.16,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"001556\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 9,"
            "\"Denomination\": 0.25,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.17,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1003A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 46,"
            "\"Denomination\": 5.0,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 31,"
            "\"Denomination\": 0.05,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.73,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1029A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 52,"
            "\"Denomination\": 0.02,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.35,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-L  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 62,"
            "\"Denomination\": 0.05,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 62,"
            "\"Denomination\": 0.25,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 73,"
            "\"Denomination\": 0.25,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 96.22,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"001555\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 24,"
            "\"Denomination\": 0.03,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 98.98,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1011A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 24,"
            "\"Denomination\": 5.0,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 98.98,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1011A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 16,"
            "\"Denomination\": 0.05,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.11,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1035A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 52,"
            "\"Denomination\": 0.25,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.35,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-L  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 73,"
            "\"Denomination\": 0.03,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 96.22,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"001555\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 52,"
            "\"Denomination\": 0.05,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.35,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-L  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 83,"
            "\"Denomination\": 0.01,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 96.16,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"001556\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 68,"
            "\"Denomination\": 0.02,"
            "\"MaxBetCredits\": 10,"
            "\"TheoPercentage\": 95.36,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"95-C  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 68,"
            "\"Denomination\": 0.05,"
            "\"MaxBetCredits\": 10,"
            "\"TheoPercentage\": 95.36,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"95-C  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 9,"
            "\"Denomination\": 0.5,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.17,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1003A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 2,"
            "\"Denomination\": 0.02,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.54,"
            "\"GameName\": \"Jacks Or Better\","
            "\"Paytable\": \"P1041A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 52,"
            "\"Denomination\": 25.0,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.35,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-L  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 16,"
            "\"Denomination\": 0.02,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.11,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1035A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 68,"
            "\"Denomination\": 1.0,"
            "\"MaxBetCredits\": 10,"
            "\"TheoPercentage\": 95.36,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"95-C  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 46,"
            "\"Denomination\": 0.05,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 62,"
            "\"Denomination\": 0.03,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 57,"
            "\"Denomination\": 0.01,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 95.25,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"94-V  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 24,"
            "\"Denomination\": 0.05,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 98.98,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1011A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 2,"
            "\"Denomination\": 1.0,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.54,"
            "\"GameName\": \"Jacks Or Better\","
            "\"Paytable\": \"P1041A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 24,"
            "\"Denomination\": 1.0,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 98.98,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1011A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 46,"
            "\"Denomination\": 0.01,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 16,"
            "\"Denomination\": 1.0,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.11,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1035A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 24,"
            "\"Denomination\": 25.0,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 98.98,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1011A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 68,"
            "\"Denomination\": 0.03,"
            "\"MaxBetCredits\": 10,"
            "\"TheoPercentage\": 95.36,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"95-C  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 31,"
            "\"Denomination\": 0.01,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.73,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1029A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 62,"
            "\"Denomination\": 5.0,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 9,"
            "\"Denomination\": 0.01,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.17,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1003A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 16,"
            "\"Denomination\": 0.25,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.11,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1035A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 24,"
            "\"Denomination\": 0.02,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 98.98,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1011A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 68,"
            "\"Denomination\": 5.0,"
            "\"MaxBetCredits\": 10,"
            "\"TheoPercentage\": 95.36,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"95-C  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 31,"
            "\"Denomination\": 0.25,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.73,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1029A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 68,"
            "\"Denomination\": 25.0,"
            "\"MaxBetCredits\": 10,"
            "\"TheoPercentage\": 95.36,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"95-C  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 57,"
            "\"Denomination\": 5.0,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 95.25,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"94-V  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 2,"
            "\"Denomination\": 5.0,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.54,"
            "\"GameName\": \"Jacks Or Better\","
            "\"Paytable\": \"P1041A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 73,"
            "\"Denomination\": 0.02,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 96.22,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"001555\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 68,"
            "\"Denomination\": 0.01,"
            "\"MaxBetCredits\": 10,"
            "\"TheoPercentage\": 95.36,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"95-C  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 31,"
            "\"Denomination\": 0.02,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.73,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1029A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 2,"
            "\"Denomination\": 0.05,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.54,"
            "\"GameName\": \"Jacks Or Better\","
            "\"Paytable\": \"P1041A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 16,"
            "\"Denomination\": 0.03,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.11,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1035A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 46,"
            "\"Denomination\": 1.0,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 24,"
            "\"Denomination\": 0.25,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 98.98,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1011A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 57,"
            "\"Denomination\": 0.25,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 95.25,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"94-V  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 83,"
            "\"Denomination\": 0.05,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 96.16,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"001556\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 16,"
            "\"Denomination\": 5.0,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.11,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1035A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 52,"
            "\"Denomination\": 1.0,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.35,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-L  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 9,"
            "\"Denomination\": 0.03,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.17,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1003A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 46,"
            "\"Denomination\": 0.03,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 16,"
            "\"Denomination\": 0.01,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.11,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1035A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 57,"
            "\"Denomination\": 0.03,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 95.25,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"94-V  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 57,"
            "\"Denomination\": 0.05,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 95.25,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"94-V  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 16,"
            "\"Denomination\": 0.5,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.11,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1035A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 9,"
            "\"Denomination\": 0.02,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.17,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1003A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 57,"
            "\"Denomination\": 0.02,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 95.25,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"94-V  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 24,"
            "\"Denomination\": 0.5,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 98.98,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1011A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 83,"
            "\"Denomination\": 0.5,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 96.16,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"001556\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 2,"
            "\"Denomination\": 0.5,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.54,"
            "\"GameName\": \"Jacks Or Better\","
            "\"Paytable\": \"P1041A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 9,"
            "\"Denomination\": 5.0,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.17,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1003A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 62,"
            "\"Denomination\": 0.01,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 73,"
            "\"Denomination\": 0.5,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 96.22,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"001555\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 73,"
            "\"Denomination\": 0.01,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 96.22,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"001555\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 52,"
            "\"Denomination\": 0.01,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.35,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-L  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 46,"
            "\"Denomination\": 0.02,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 24,"
            "\"Denomination\": 0.01,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 98.98,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1011A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 9,"
            "\"Denomination\": 1.0,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.17,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1003A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 2,"
            "\"Denomination\": 0.03,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.54,"
            "\"GameName\": \"Jacks Or Better\","
            "\"Paytable\": \"P1041A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 62,"
            "\"Denomination\": 0.5,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 9,"
            "\"Denomination\": 25.0,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.17,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1003A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 62,"
            "\"Denomination\": 1.0,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 46,"
            "\"Denomination\": 25.0,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 57,"
            "\"Denomination\": 1.0,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 95.25,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"94-V  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 2,"
            "\"Denomination\": 25.0,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.54,"
            "\"GameName\": \"Jacks Or Better\","
            "\"Paytable\": \"P1041A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 57,"
            "\"Denomination\": 0.5,"
            "\"MaxBetCredits\": 8,"
            "\"TheoPercentage\": 95.25,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"94-V  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 83,"
            "\"Denomination\": 0.02,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 96.16,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"001556\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 62,"
            "\"Denomination\": 0.02,"
            "\"MaxBetCredits\": 80,"
            "\"TheoPercentage\": 94.99,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"93-P  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 31,"
            "\"Denomination\": 0.5,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.73,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1029A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 31,"
            "\"Denomination\": 5.0,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.73,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"P1029A\","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 2,"
            "\"Denomination\": 0.25,"
            "\"MaxBetCredits\": 5,"
            "\"TheoPercentage\": 99.54,"
            "\"GameName\": \"Jacks Or Better\","
            "\"Paytable\": \"P1041A\","
            "\"QueryCards\": false,"
            "\"Levels\": ["
                "{"
                    "\"LevelId\": 632,"
                    "\"LevelSequence\": 3"
                "},"
                "{"
                    "\"LevelId\": 627,"
                    "\"LevelSequence\": 1"
                "},"
                "{"
                    "\"LevelId\": 638,"
                    "\"LevelSequence\": 6"
                "},"
                "{"
                    "\"LevelId\": 629,"
                    "\"LevelSequence\": 2"
                "},"
                "{"
                    "\"LevelId\": 634,"
                    "\"LevelSequence\": 4"
                "},"
                "{"
                    "\"LevelId\": 636,"
                    "\"LevelSequence\": 5"
                "}"
            "]"
        "},"
        "{"
            "\"GameNumber\": 68,"
            "\"Denomination\": 0.25,"
            "\"MaxBetCredits\": 10,"
            "\"TheoPercentage\": 95.36,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"95-C  \","
            "\"QueryCards\": false"
        "},"
        "{"
            "\"GameNumber\": 68,"
            "\"Denomination\": 0.5,"
            "\"MaxBetCredits\": 10,"
            "\"TheoPercentage\": 95.36,"
            "\"GameName\": \"GK\","
            "\"Paytable\": \"95-C  \","
            "\"QueryCards\": false"
        "}"
    "],"
    "\"Levels\": ["
        "{"
            "\"Id\": 627,"
            "\"Version\": 250,"
            "\"Type\": \"Standard\","
            "\"Channel\": {"
                "\"Destination\": \"@239.1.1.1:61613\","
                "\"MulticastDestination\": \"stomp://239.1.1.1:61613\""
            "},"
            "\"Info\": \"\","
            "\"Name\": \"Johnny_Jacks or Better_Level 1\","
            "\"MediaMinSeconds\": 0,"
            "\"MediaMaxSeconds\": 0"
        "},"
        "{"
            "\"Id\": 629,"
            "\"Version\": 251,"
            "\"Type\": \"Standard\","
            "\"Channel\": {"
                "\"Destination\": \"@239.1.1.1:61613\","
                "\"MulticastDestination\": \"stomp://239.1.1.1:61613\""
            "},"
            "\"Info\": \"\","
            "\"Name\": \"Johnny_Jacks or Better_Level 2\","
            "\"MediaMinSeconds\": 0,"
            "\"MediaMaxSeconds\": 0"
        "},"
        "{"
            "\"Id\": 634,"
            "\"Version\": 253,"
            "\"Type\": \"Standard\","
            "\"Channel\": {"
                "\"Destination\": \"@239.1.1.1:61613\","
                "\"MulticastDestination\": \"stomp://239.1.1.1:61613\""
            "},"
            "\"Info\": \"\","
            "\"Name\": \"Johnny_Jacks or Better_Level 4\","
            "\"MediaMinSeconds\": 0,"
            "\"MediaMaxSeconds\": 0"
        "},"
        "{"
            "\"Id\": 632,"
            "\"Version\": 252,"
            "\"Type\": \"Standard\","
            "\"Channel\": {"
                "\"Destination\": \"@239.1.1.1:61613\","
                "\"MulticastDestination\": \"stomp://239.1.1.1:61613\""
            "},"
            "\"Info\": \"\","
            "\"Name\": \"Johnny_Jacks or Better_Level 3\","
            "\"MediaMinSeconds\": 0,"
            "\"MediaMaxSeconds\": 0"
        "},"
        "{"
            "\"Id\": 638,"
            "\"Version\": 255,"
            "\"Type\": \"Standard\","
            "\"Channel\": {"
                "\"Destination\": \"@239.1.1.1:61613\","
                "\"MulticastDestination\": \"stomp://239.1.1.1:61613\""
            "},"
            "\"Info\": \"\","
            "\"Name\": \"Johnny_Jacks or Better_Level 6\","
            "\"MediaMinSeconds\": 0,"
            "\"MediaMaxSeconds\": 0"
        "},"
        "{"
            "\"Id\": 636,"
            "\"Version\": 254,"
            "\"Type\": \"Standard\","
            "\"Channel\": {"
                "\"Destination\": \"@239.1.1.1:61613\","
                "\"MulticastDestination\": \"stomp://239.1.1.1:61613\""
            "},"
            "\"Info\": \"\","
            "\"Name\": \"Johnny_Jacks or Better_Level 5\","
            "\"MediaMinSeconds\": 0,"
            "\"MediaMaxSeconds\": 0"
        "}"
    "],"
    "\"ProtocolType\": \"sas\","
    "\"ProgressiveGroupId\": 1,"
    "\"PaymentMethod\": \"SasAFTBonus\","
    "\"UseAFTLockOnTransfers\": false,"
    "\"MeterBankLevel\": false,"
    "\"MeterMachineOnly\": false,"
    "\"ContentHttpUrl\": \"http://10.138.98.70:8080/onelink/content/original/\","
    "\"Playlist\": {"
        "\"Name\": \"SLOT_PLAYLIST\","
        "\"Items\": ["
            "{"
                "\"Content\": {"
                    "\"FileName\": \"ZMbanana.gif\","
                    "\"MD5\": \"f30e21da146bd3501555eec943a8898e\""
                "},"
                "\"Enabled\": true,"
                "\"BeginDate\": \"2015-06-18T00:00:00.000-0700\","
                "\"BeginTime\": \"00:00\","
                "\"EndTime\": \"00:00\","
                "\"ActiveSunday\": true,"
                "\"ActiveMonday\": true,"
                "\"ActiveTuesday\": true,"
                "\"ActiveWednesday\": true,"
                "\"ActiveThursday\": true,"
                "\"ActiveFriday\": true,"
                "\"ActiveSaturday\": true,"
                "\"SequenceId\": 1,"
                "\"AudioAdjustment\": 1.0,"
                "\"Mute\": false,"
                "\"Image\": true,"
                "\"Duration\": 10000"
            "},"
            "{"
                "\"Content\": {"
                    "\"FileName\": \"diningicon_1.jpg\","
                    "\"MD5\": \"563177e561c8d7596e83c943be844cf3\""
                "},"
                "\"Enabled\": true,"
                "\"BeginDate\": \"2015-06-18T00:00:00.000-0700\","
                "\"BeginTime\": \"00:00\","
                "\"EndTime\": \"00:00\","
                "\"ActiveSunday\": true,"
                "\"ActiveMonday\": true,"
                "\"ActiveTuesday\": true,"
                "\"ActiveWednesday\": true,"
                "\"ActiveThursday\": true,"
                "\"ActiveFriday\": true,"
                "\"ActiveSaturday\": true,"
                "\"SequenceId\": 2,"
                "\"AudioAdjustment\": 1.0,"
                "\"Mute\": false,"
                "\"Image\": true,"
                "\"Duration\": 10000"
            "},"
            "{"
                "\"Content\": {"
                    "\"FileName\": \"zmSummitBottom.flv\","
                    "\"MD5\": \"75cd87cd5a8160c0d9114e30b5b893cc\""
                "},"
                "\"Enabled\": true,"
                "\"BeginDate\": \"2015-06-22T00:00:00.000-0700\","
                "\"BeginTime\": \"00:00\","
                "\"EndTime\": \"00:00\","
                "\"ActiveSunday\": true,"
                "\"ActiveMonday\": true,"
                "\"ActiveTuesday\": true,"
                "\"ActiveWednesday\": true,"
                "\"ActiveThursday\": true,"
                "\"ActiveFriday\": true,"
                "\"ActiveSaturday\": true,"
                "\"SequenceId\": 3,"
                "\"AudioAdjustment\": 1.0,"
                "\"Mute\": false,"
                "\"Image\": false,"
                "\"Duration\": 188660"
            "},"
            "{"
                "\"Content\": {"
                    "\"FileName\": \"640x360_floorwide2.png\","
                    "\"MD5\": \"d84c74df38b65b9bc067251875d292b1\""
                "},"
                "\"Enabled\": true,"
                "\"BeginDate\": \"2015-06-23T00:00:00.000-0700\","
                "\"BeginTime\": \"00:00\","
                "\"EndTime\": \"00:00\","
                "\"ActiveSunday\": true,"
                "\"ActiveMonday\": true,"
                "\"ActiveTuesday\": true,"
                "\"ActiveWednesday\": true,"
                "\"ActiveThursday\": true,"
                "\"ActiveFriday\": true,"
                "\"ActiveSaturday\": true,"
                "\"SequenceId\": 4,"
                "\"AudioAdjustment\": 1.0,"
                "\"Mute\": false,"
                "\"Image\": true,"
                "\"Duration\": 10000"
            "}"
        "],"
        "\"Shuffle\": false,"
        "\"Description\": \"\""
    "},"
    "\"DeviceId\": 663,"
    "\"DeviceConfigVersion\": 0,"
    "\"DeviceAddress\": \"@device.ICARD.206FEC01068D\","
    "\"EventChannel\": {"
        "\"Destination\": \"@239.3.0.1:61613\","
        "\"MulticastDestination\": \"stomp://239.3.0.1:61613\""
    "},"
    "\"AssetNumber\": \"42731\","
    "\"BankName\": \"Default Bank\","
    "\"TimeZone\": \"America/Los_Angeles\","
    "\"SourceDeviceId\": 742,"
    "\"SourceDeviceConfigVersion\": 0,"
    "\"SourceAsset\": \"\","
    "\"Timestamp\": 1438721358764,"
    "\"MessageId\": \"discovery1438721358764\","
    "\"OneLinkSystemId\": \"23951469-c3cd-41be-8d79-2780e7de9291\""
"}";

}
#endif //TODO-PORT