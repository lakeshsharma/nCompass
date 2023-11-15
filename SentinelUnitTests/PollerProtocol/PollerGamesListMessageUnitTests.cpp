#include "stdafx.h"
#include "PollerProtocol/PollerGamesListMessage.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelUnitTests
{
    class PollerGamesListMessageUnitTests : public testing::Test
    {


 public:
        virtual void SetUp() 
        {
        }

        virtual void TearDown() 
        {
        } 
        CParseGameList * gameList;

        CPollerGamesListMessage * gamesListMessage;

        CGames *games;

        CGame *game;

		void ParseDenomGamesForPoller(CGames* gamesPtr)
        {
            gameList = new CParseGameList();

            gameList->ParseDenomGames(gamesPtr);

            ASSERT_EQ(gameList->GetDenoms().size(), 2);

            delete gameList;

         }

		void GetReadWriteBufferForGameList()
		{
			//set up
			gameList = new CParseGameList();

			games = new CGames(MEMORY_NONE);

			game = new CGame(MEMORY_NONE);
			games->AddGame(game);
			game->SetGameNumber(1);
			game->SetDenom((WORD)1, US_DOLLARS);
			game->SetMfgID("1234");
			game->SetPaytableID("xyzw");
			game->Enable();
			game->SetTheoreticalPayback(9492);
			game->SetGamenameLength((byte)5);
			game->SetGameName("xxxx");
			game->SetAdditionalID("ab");

			game = new CGame(MEMORY_NONE);
			games->AddGame(game);
			game->SetGameNumber(2);
			game->SetDenom((WORD)2, US_DOLLARS);
			game->SetMfgID("4556");
			game->SetPaytableID("abcd");
			game->Enable();
			game->SetTheoreticalPayback(8989);
			game->SetGamenameLength((byte)5);
			game->SetGameName("xxxx");
			game->SetAdditionalID("ab");

			gameList->ParseDenomGames(games);
			gameList->ParseGamesRange(games, 0, 2);
			time_t currentTime = CUtilities::GetCurrentTime();
			CPollerGamesListMessage *pollerGamesListMessage = new CPollerGamesListMessage(gameList, currentTime, 0, 3, 0, 0, 2, 2);

			DWORD bufferlen;

			byte *buffer = pollerGamesListMessage->GetReadWriteBuffer(bufferlen);

			DWORD dataLength = bufferlen - SIZE_OF_MESSAGE_HEADER;
			//Tests
			ASSERT_EQ(bufferlen, 129);

			ASSERT_EQ(dataLength, 107);

			delete gameList;
			delete pollerGamesListMessage;
		}


	};

    TEST_F(PollerGamesListMessageUnitTests, ParseDenomGamesUnitTest)
    {

        //set up
        games = new CGames(MEMORY_NONE);

        game = new CGame(MEMORY_NONE);
        games->AddGame(game);
        game->SetGameNumber(1);
        game->SetDenom((WORD)1, US_DOLLARS);
        game->SetMfgID("1234");
        game->SetPaytableID("xyzw");
        game->Enable();
        game->SetTheoreticalPayback(9492);
        game->SetGamenameLength((byte)5);
        game->SetGameName("xxxx");
        game->SetAdditionalID("ab");


        game = new CGame(MEMORY_NONE);
        games->AddGame(game);
        game->SetGameNumber(2);
        game->SetDenom((WORD)2, US_DOLLARS);
        game->SetMfgID("4556");
        game->SetPaytableID("abcd");
        game->Enable();
        game->SetTheoreticalPayback(8989);
        game->SetGamenameLength((byte)5);
        game->SetGameName("xxxx");
        game->SetAdditionalID("ab");


        //tests
        ParseDenomGamesForPoller(games);

        delete games;
    }

	TEST_F(PollerGamesListMessageUnitTests, GetReadWriteBufferUnitTest)
	{
		GetReadWriteBufferForGameList();
	}
}
