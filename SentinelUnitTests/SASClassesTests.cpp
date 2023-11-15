#include "stdafx.h"
#include "SASProtocol/SASGameList.h"
#include "SASProtocol/SASMeter.h"
#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{

		TEST(SASClassesTest, SASGameListTest)
		{
			CSASGameList *gameList = new CSASGameList();
			gameList->SetGameEnabled(1);
			gameList->SetGameEnabled(2);
			gameList->SetCount(4);

			cout << "gameList->GetCount()=" << gameList->GetCount() << endl;

			ASSERT_TRUE(gameList->GetCount() == 4) << "Game list count did not return expected value.";
			ASSERT_TRUE(gameList->IsGameEnabled(1)) << "Game 1 should be enabled";
			ASSERT_TRUE(gameList->IsGameEnabled(2)) << "Game 2 should be enabled";
			ASSERT_TRUE(!gameList->IsGameEnabled(3)) << "Game 3 should be disabled";
			ASSERT_TRUE(!gameList->IsGameEnabled(4)) << "Game 4 should be disabled";

			gameList->SetGameEnabled(4);
			ASSERT_TRUE(gameList->GetCount() == 4) << "Game count was changed after game set enabled.";
			ASSERT_TRUE(gameList->IsGameEnabled(4)) << "Game 4 should now be enabled";

			gameList->SetCurrentGame(true, true, 2);
			gameList->SetFirstPass();
			bool needToSendInfo;
			ASSERT_TRUE(gameList->GetCurrentGame() == 2) << "Current game not set properly" ;
			ASSERT_TRUE(gameList->GetNextGame(needToSendInfo) == 1) << "Game 1 should be the 1st game retrieved" ;
			ASSERT_FALSE(gameList->IsLastEnabledGameOnFirstPass());
			ASSERT_TRUE(needToSendInfo) << "need-to-send flag should be true after 1st game retrieved" ;
			ASSERT_TRUE(gameList->GetNextGame(needToSendInfo) == 2) << "Game 2 should be the 2nd game retrieved" ;
			ASSERT_TRUE(needToSendInfo) << "need-to-send flag should be true after 2nd game retrieved" ;
			ASSERT_FALSE(gameList->IsLastEnabledGameOnFirstPass());
			ASSERT_TRUE(gameList->GetNextGame(needToSendInfo) == 4) << "Game 4 should be the 3rd game retrieved";
			ASSERT_TRUE(needToSendInfo) << "need-to-send flag should be true after 3rd game retrieved" ;
			ASSERT_TRUE(gameList->IsLastEnabledGameOnFirstPass());
			ASSERT_TRUE(gameList->GetNextGame(needToSendInfo) == 3) << "Game 3 should be the 4th game retrieved";
			ASSERT_TRUE(needToSendInfo) << "need-to-send flag should be true after 4th game retrieved" ;
			ASSERT_TRUE(gameList->IsLastEnabledGameOnFirstPass());
			ASSERT_TRUE(gameList->GetNextGame(needToSendInfo) == 2) << "Game 2 should be the 5th game retrieved";
			ASSERT_FALSE(needToSendInfo) << "need-to-send flag should be false after 5th game retrieved" ;
			ASSERT_TRUE(gameList->IsLastEnabledGameOnFirstPass());
			ASSERT_TRUE(gameList->GetNextGame(needToSendInfo) == 1) << "Game 1 should be the 6th game retrieved";
			ASSERT_FALSE(needToSendInfo) << "need-to-send flag should be false after 6th game retrieved" ;
			ASSERT_TRUE(gameList->IsLastEnabledGameOnFirstPass());
			ASSERT_TRUE(gameList->GetNextGame(needToSendInfo) == 2) << "Game 2 should be the 7th game retrieved";
			ASSERT_FALSE(needToSendInfo) << "need-to-send flag should be false after 7th game retrieved" ;
			ASSERT_TRUE(gameList->GetNextGame(needToSendInfo) == 2) << "Game 2 should be the 8th game retrieved" ;
			ASSERT_FALSE(needToSendInfo) << "need-to-send flag should be false after 8th game retrieved" ;
			ASSERT_TRUE(gameList->GetNextGame(needToSendInfo) == 2) << "Game 2 should be the 9th game retrieved" ;
			ASSERT_FALSE(needToSendInfo) << "need-to-send flag should be false after 9th game retrieved" ;
			ASSERT_TRUE(gameList->GetNextGame(needToSendInfo) == 4) << "Game 4 should be the 10th game retrieved" ;
			ASSERT_FALSE(needToSendInfo) << "need-to-send flag should be false after 10th game retrieved" ;
			ASSERT_TRUE(gameList->GetNextGame(needToSendInfo) == 2) << "Game 2 should be the 11th game retrieved" ;
			ASSERT_FALSE(needToSendInfo) << "need-to-send flag should be false after 11th game retrieved" ;
			ASSERT_TRUE(gameList->GetNextGame(needToSendInfo) == 3) << "Game 3 should be the 12th game retrieved" ;
			ASSERT_FALSE(needToSendInfo) << "need-to-send flag should be false after 12th game retrieved" ;

			delete gameList;
		}

		TEST(SASClassesTest, SASGameListErrorConditionTest)
		{
			CSASGameList gameList;

			ASSERT_FALSE(gameList.IsError());
			gameList.SetError();
			ASSERT_TRUE(gameList.IsError());
			gameList.ClearError();
			ASSERT_FALSE(gameList.IsError());
			gameList.SetError();
			gameList.Clear();
			ASSERT_FALSE(gameList.IsError());
		}

		TEST(SASClassesTest, SASMeterTest)
		{
			CSASMeter *sasMeter = new CSASMeter(100);

			ASSERT_TRUE(sasMeter->GetMeterNumber() == 100) << "Meter number should be 100";
			ASSERT_TRUE(sasMeter->GetLongPoll() == 0) << "Meter number should be 0 after constructor";
			ASSERT_TRUE(sasMeter->GetMeterCode() == 0) << "Meter code should be 0 after constructor";

			sasMeter->SetLongPoll(0x6f);
			sasMeter->SetMeterCode(0x02);
			ASSERT_TRUE(sasMeter->GetLongPoll() == 0x6f) << "Meter number should be 0x6f";
			ASSERT_TRUE(sasMeter->GetMeterCode() == 0x02) << "Meter code should be 0x02";


			delete sasMeter;
		}
}
