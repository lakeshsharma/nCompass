
#include "stdafx.h"
#include "gtest/gtest.h"
#include "../Sentinel/Game.h"
#include "../Sentinel/metersdef.h"

class GameTest : public ::testing::Test
{
    protected:
        virtual void SetUp() 
        {
            _egmGame = new CGame(MEMORY_NONE, true);
            _subGame = new CGame(MEMORY_NONE, false);
        }

        virtual void TearDown() 
        {
            delete _egmGame;
            delete _subGame;
        }
        
    CGame *_egmGame;
    CGame *_subGame;
};

TEST_F(GameTest, GameConstructorTestIsEGMTrue)
{
    EXPECT_TRUE(_egmGame->IsEGM());
    EXPECT_EQ(_egmGame->GetGameNumber(), 0);
    EXPECT_TRUE(_egmGame->IsEnabled());
    EXPECT_FALSE(_egmGame->IsCurrentGame());
    EXPECT_EQ(_egmGame->GetDenomMultiplier(), 1);
    EXPECT_EQ(_egmGame->GetMaxBet(), 0);
    EXPECT_EQ(_egmGame->GetProgressiveGroup(), 0);
    EXPECT_EQ(_egmGame->GetCreditsWon(), 0);
    EXPECT_STREQ(_egmGame->GetPaytableID(), "");
    EXPECT_STREQ(_egmGame->GetAdditionalID(), "");
    EXPECT_EQ(_egmGame->GetTheoreticalPayback(), 0);
}

TEST_F(GameTest, GameConstructorTestIsEGMFalse)
{
    EXPECT_FALSE(_subGame->IsEGM());
    EXPECT_NE(_subGame->GetGameNumber(), 0);
    EXPECT_TRUE(_subGame->IsEnabled());
    EXPECT_FALSE(_subGame->IsCurrentGame());
    EXPECT_EQ(_subGame->GetDenomMultiplier(), 1);
    EXPECT_EQ(_subGame->GetMaxBet(), 0);
    EXPECT_EQ(_subGame->GetProgressiveGroup(), 0);
    EXPECT_EQ(_subGame->GetCreditsWon(), 0);
    EXPECT_STREQ(_subGame->GetPaytableID(), "");
    EXPECT_STREQ(_subGame->GetAdditionalID(), "");
    EXPECT_EQ(_subGame->GetTheoreticalPayback(), 0);
}

TEST_F(GameTest, GameCopyConstructorTest)
{
    CGame *copyGame = new CGame(*_egmGame);
    
    EXPECT_TRUE(_egmGame->IsEGM());
    EXPECT_EQ(_egmGame->GetGameNumber(), 0);
    EXPECT_TRUE(_egmGame->IsEnabled());
    EXPECT_FALSE(_egmGame->IsCurrentGame());
    EXPECT_EQ(_egmGame->GetDenomMultiplier(), 1);
    EXPECT_EQ(_egmGame->GetMaxBet(), 0);
    EXPECT_EQ(_egmGame->GetProgressiveGroup(), 0);
    EXPECT_EQ(_egmGame->GetCreditsWon(), 0);
    EXPECT_STREQ(_egmGame->GetPaytableID(), "");
    EXPECT_STREQ(_egmGame->GetAdditionalID(), "");
    EXPECT_EQ(_egmGame->GetTheoreticalPayback(), 0);
    
    // Add some meters into the copied game.
    copyGame->SetMeter(gCI, 1000);
    copyGame->SetMeter(gCO, 2000);
    
    // Copy game again and make sure the meters are copied
    CGame *copyGame2 = new CGame(*copyGame);
    EXPECT_EQ(copyGame2->GetMeter(gCI), 1000);
    EXPECT_EQ(copyGame2->GetMeter(gCO), 2000);
    EXPECT_EQ(copyGame2->GetMeter(mCrd), INVALID_METER_VALUE );
}

TEST_F(GameTest, GameMetersTest)
{
    CGame *copyGame = new CGame(*_egmGame);
    copyGame->SetMeter(gCI, 1000);
    copyGame->SetMeter(gCO, 2000);
    EXPECT_EQ(copyGame->GetMeter(gCI), 1000);
    EXPECT_EQ(copyGame->GetMeter(gCO), 2000);
    EXPECT_EQ(copyGame->GetMeter(mCrd), INVALID_METER_VALUE );
    
    // Copy game again and make sure the meters are copied
    CGame *copyGame2 = new CGame(*copyGame);
    EXPECT_EQ(copyGame2->GetMeter(gCI), 1000);
    EXPECT_EQ(copyGame2->GetMeter(gCO), 2000);
    EXPECT_EQ(copyGame2->GetMeter(mCrd), INVALID_METER_VALUE );

    // Add to a meter value
    copyGame->AddToMeter(gCI, 234);
    EXPECT_EQ(copyGame->GetMeter(gCI), 1234);
    
    // Try to set a meter past the allowable range of subgames    
    CGame *copySubGame = new CGame(*_subGame);
    copySubGame->SetMeter(SUBGAME_METER_LAST, 1000);
    EXPECT_EQ(copyGame2->GetMeter(SUBGAME_METER_LAST), INVALID_METER_VALUE );
}

TEST_F(GameTest, GameSetTest)
{
    CGame *copyGame = new CGame(*_subGame);
    
    copyGame->SetGameNumber(1234);
    EXPECT_EQ(copyGame->GetGameNumber(), 1234);
    
    copyGame->SetDenom(25);
    EXPECT_EQ(copyGame->GetDenomMultiplier(), 25);
    
    copyGame->SetPaytableID("100");
    EXPECT_STREQ(copyGame->GetPaytableID(), "100");
    
    copyGame->SetMfgID("AT");
    EXPECT_STREQ(copyGame->GetMfgID(), "AT");
    
    copyGame->SetAdditionalID("1T");
    EXPECT_STREQ(copyGame->GetAdditionalID(), "1T");

    copyGame->Disable();
    EXPECT_FALSE(copyGame->IsEnabled());

    copyGame->Enable();
    EXPECT_TRUE(copyGame->IsEnabled());

    copyGame->ChangeCurrentGameStatus();
    EXPECT_TRUE(copyGame->IsCurrentGame());

    copyGame->ChangeCurrentGameStatus(false);
    EXPECT_FALSE(copyGame->IsCurrentGame());
};

TEST_F(GameTest, GameTheoTest)
{
    CGame *copyGame = new CGame(*_subGame);
    
    copyGame->SetTheoreticalPayback(9877);
    EXPECT_EQ(copyGame->GetTheoreticalPayback(),9877);
    
    copyGame->SetTheoreticalPayback(const_cast<char *>("9799"));
    EXPECT_EQ(copyGame->GetTheoreticalPayback(),9799);
    
    copyGame->SetTheoreticalPayback(const_cast<char *>("19"));
    EXPECT_EQ(copyGame->GetTheoreticalPayback(),10019);
}

TEST_F(GameTest, GameLevelIdsTest)
{
    CGame* game = new CGame(*_subGame);
    game->SetLevelIds(0);
    EXPECT_EQ(game->GetLevelIds(), 0);
    EXPECT_STREQ(game->GetLevelIdsString().c_str(), "");

    game->SetLevelIds(4);
    EXPECT_EQ(game->GetLevelIds(), 4);
    EXPECT_STREQ(game->GetLevelIdsString().c_str(), "3");

    game->SetLevelIds(5);
    EXPECT_EQ(game->GetLevelIds(), 5);
    EXPECT_STREQ(game->GetLevelIdsString().c_str(), "1,3");
}
