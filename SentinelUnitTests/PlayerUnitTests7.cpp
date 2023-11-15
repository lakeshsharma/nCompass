#include "stdafx.h"
#include "Player.h"
#include "SubgameSessionParameters.h"
#include "SubgameSession.h"
#include "Config.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	static const time_t TIME_DEFAULT_VALUE(0); // Arbitrary value when any valid value is okay.
	static const byte MEMSET_DEFAULT_VALUE = 0xcd; // Value unlikely to be used for other purposes.

	class Player7 : public ::testing::Test
	{
	
	private:
		CPlayer *m_player;
		
	public:
		
		//Setup
		Player7()
		{					
			//No need to set this time to do basic tests on class instances.
			time_t transTime; 
			
			// Setup CPlayer for virtual card out.
			CCard *card(new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE));
			card->SetCardID(_T("1234567890"));
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			
			CGames *games = new CGames();
			m_player = new CPlayer(card, new CPlayerInfo, config, egmConfig, TIME_DEFAULT_VALUE, false, games);
		}

		CPlayer* GetPlayer()
		{
			return m_player;
		}
		
        __int64 GetMeterDeltafullTest(CPlayer &obj, byte meterNumber)
        {
            return obj.GetMeterDeltafull(meterNumber);
        }
        __int64 GetMeterDeltaUnflushedTest(CPlayer &obj, byte meterNumber)
        {
            return obj.GetMeterDeltaUnflushed(meterNumber);
        }
        __int64 GetCurrentMeterValueTest(CPlayer &obj, byte meterNumber)
        {
            return obj.GetCurrentMeterValue(meterNumber);
        }
        string ResponseForPlayerSessionFullSessionTest(CPlayer &obj)
        {
            return obj.ResponseForPlayerSessionFullSession();
        }
        string ResponseForPlayerSessionUnFlushedSessionTest(CPlayer &obj)
        {
            return obj.ResponseForPlayerSessionUnFlushedSession();
        }
        string ResponseForPlayerSessionExtUnFlushedSessionTest(CPlayer &obj)
        {
            return obj.ResponseForPlayerSessionExtUnFlushedSession();
        }
        string ResponseForPlayerSessionExtFullSessionTest(CPlayer &obj)
        {
            return obj.ResponseForPlayerSessionExtFullSession();
        }
		
		void PlayerDecrementCountdown(CPlayer &player, DWORD gameSessionSequenceNumber, DWORD multiplier, 
			DWORD divider, DWORD decrement, bool isNewPlayer,  DWORD points, DWORD startingCountdown, bool &bonusPointsAwardedResult)
		{				
			// Setup.
			if (isNewPlayer)
			{
				const DWORD fallbackMultiplier(DEFAULT_MULTIPLIER + 1);
				const DWORD fallbackDivider(DEFAULT_COUNTDOWN + 1);
							
				CPlayerInfo *playerInfo = new CPlayerInfo();
				playerInfo->SetPointsAndCountdown(points, startingCountdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			
				playerInfo->SetMultiplierDivider(multiplier, divider);
				player.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);
			}
			
			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			subgameSelectionResponse.m_subgameSessionParams.m_gameSessionSequenceNumber = gameSessionSequenceNumber;
			subgameSelectionResponse.m_subgameSessionParams.m_multiplier = multiplier;	//Keep it unchanged
			subgameSelectionResponse.m_subgameSessionParams.m_divider = divider;	//Keep it unchanged
			ASSERT_TRUE (player.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE (player.ApplySubgameSelectionResponse(&subgameSelectionResponse));
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + decrement, true, false);
			bonusPointsAwardedResult = player.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
		
		}
		
		void SetInitialSessionPoints (CPlayer &obj, DWORD initialSessionPts)
		{
			obj.m_sessionInfo.SetInitialSessionPoints(initialSessionPts);
		}	
	};

		TEST_F(Player7, PlayerPointCalcLargeDecrement)
		{
			const DWORD fallbackMultiplier(DEFAULT_MULTIPLIER + 1);
			const DWORD fallbackDivider(DEFAULT_COUNTDOWN + 1);

            const DWORD multiplier(1);
            const DWORD divider(1);
			const DWORD points (0);
			const DWORD countdown (1);
			
            CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			bool tilt(false);
			CPlayerInfo *pi = new CPlayerInfo();
			pi->SetMultiplierDivider(multiplier, divider);
			pi->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			config.SetDenomination(1);
			config.SetCoinsInAccounting(true);

			CPlayer player(config, egmConfig);
 			player.SetPlayerInfoAndMultiplierDividerCountdown(pi, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);

			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			subgameSelectionResponse.m_subgameSessionParams.m_multiplier = multiplier;;
			subgameSelectionResponse.m_subgameSessionParams.m_divider = divider;
			player.SetCurrentCountdown(1);

			ASSERT_TRUE(player.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE(player.ApplySubgameSelectionResponse(&subgameSelectionResponse));
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 11, true, false);
			player.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
			ASSERT_EQ(player.GetSessionPoints(), 10);
		}

        TEST_F(Player7, GetMeterDeltafull)
		{
			{
                // Setup.
                CCard *card = NULL;
                CPlayerInfo *playerInfo = NULL;
                const CConfig config(false, "");
                const CEGMConfig egmConfig(MEMORY_NONE);

                CGames *games = new CGames();
                CGame *game = new CGame(MEMORY_NONE, true);
                game->SetMeter(mCD, 60L);
                game->SetMeter(mD1, 50L);
                game->SetMeter(mTotalDrop, 10L);

                games->AddGame(game);

                CMeters *meters = new CMeters(MEMORY_NONE);
                meters->SetMeter(mCD, 50L);
                meters->SetMeter(mD1, 40L);
            
                // Perform operation(s) to be tested.
                CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, false, games);
                obj.SetSessionStartMeters(meters);
                ASSERT_EQ(10, GetMeterDeltafullTest(obj, mCD));
                ASSERT_EQ(10, GetMeterDeltafullTest(obj, mD1));
                ASSERT_EQ(0, GetMeterDeltafullTest(obj, mVchrTktDrop));
                ASSERT_EQ(11, GetMeterDeltafullTest(obj, mTotalDrop));

                delete games;
            }
            {
                // Setup.
                CCard *card = NULL;
                CPlayerInfo *playerInfo = NULL;
                const CConfig config(false, "");
                const CEGMConfig egmConfig(MEMORY_NONE);
                       
                // Perform operation(s) to be tested.
                CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, false, NULL);
                ASSERT_EQ(0, GetMeterDeltafullTest(obj, mCD));
            }
            {
                // Setup.
                CCard *card = NULL;
                CPlayerInfo *playerInfo = NULL;
                const CConfig config(false, "");
                const CEGMConfig egmConfig(MEMORY_NONE);

                CGames *games = new CGames();
                // Perform operation(s) to be tested.
                CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, false, games);
                ASSERT_EQ(0, GetMeterDeltafullTest(obj, mCD));

                delete games;
            }
		}

        TEST_F(Player7, GetMeterDeltaUnflushed)
		{
			{
                // Setup.
                CCard *card = NULL;
                CPlayerInfo *playerInfo = NULL;
                const CConfig config(false, "");
                const CEGMConfig egmConfig(MEMORY_NONE);

                CGames *games = new CGames();
                CGame *game = new CGame(MEMORY_NONE, true);
                game->SetMeter(mCD, 60L);
                game->SetMeter(mD1, 50L);
                game->SetMeter(mTotalDrop, 10L);

                games->AddGame(game);

                CMeters *meters = new CMeters(MEMORY_NONE);
                meters->SetMeter(mCD, 50L);
                meters->SetMeter(mD1, 40L);
            
                // Perform operation(s) to be tested.
                CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, false, games);
                obj.SetSessionStartMetersCurrent(meters);
                ASSERT_EQ(10, GetMeterDeltaUnflushedTest(obj, mCD));
                ASSERT_EQ(10, GetMeterDeltaUnflushedTest(obj, mD1));
                ASSERT_EQ(0, GetMeterDeltaUnflushedTest(obj, mVchrTktDrop));
                ASSERT_EQ(11, GetMeterDeltaUnflushedTest(obj, mTotalDrop));

                delete games;
            }
            {
                // Setup.
                CCard *card = NULL;
                CPlayerInfo *playerInfo = NULL;
                const CConfig config(false, "");
                const CEGMConfig egmConfig(MEMORY_NONE);
                       
                // Perform operation(s) to be tested.
                CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, false, NULL);
                ASSERT_EQ(0, GetMeterDeltaUnflushedTest(obj, mCD));
            }
            {
                // Setup.
                CCard *card = NULL;
                CPlayerInfo *playerInfo = NULL;
                const CConfig config(false, "");
                const CEGMConfig egmConfig(MEMORY_NONE);

                CGames *games = new CGames();
                // Perform operation(s) to be tested.
                CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, false, games);
                ASSERT_EQ(0, GetMeterDeltaUnflushedTest(obj, mCD));

                delete games;
            }
		}

        TEST_F(Player7, GetCurrentMeterValue)
		{
			{
                // Setup.
                CCard *card = NULL;
                CPlayerInfo *playerInfo = NULL;
                const CConfig config(false, "");
                const CEGMConfig egmConfig(MEMORY_NONE);

                CGames *games = new CGames();
                CGame *game = new CGame(MEMORY_NONE, true);
                game->SetMeter(mCD, 60L);
                game->SetMeter(mD1, 50L);
                game->SetMeter(mTotalDrop, 10L);

                games->AddGame(game);

                // Perform operation(s) to be tested.
                CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, false, games);
                ASSERT_EQ(60, GetCurrentMeterValueTest(obj, mCD));
                ASSERT_EQ(50, GetCurrentMeterValueTest(obj, mD1));
                ASSERT_EQ(-1, GetCurrentMeterValueTest(obj, mVchrTktDrop));
                ASSERT_EQ(10, GetCurrentMeterValueTest(obj, mTotalDrop));

                delete games;
            }
            {
                // Setup.
                CCard *card = NULL;
                CPlayerInfo *playerInfo = NULL;
                const CConfig config(false, "");
                const CEGMConfig egmConfig(MEMORY_NONE);
                       
                // Perform operation(s) to be tested.
                CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, false, NULL);
                ASSERT_EQ(-1, GetCurrentMeterValueTest(obj, mCD));
            }
            {
                // Setup.
                CCard *card = NULL;
                CPlayerInfo *playerInfo = NULL;
                const CConfig config(false, "");
                const CEGMConfig egmConfig(MEMORY_NONE);

                CGames *games = new CGames();
                // Perform operation(s) to be tested.
                CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, false, games);
                ASSERT_EQ(-1, GetCurrentMeterValueTest(obj, mCD));

                delete games;
            }
		}

        TEST_F(Player7, ResponseForPlayerSessionFullSession)
		{
			//Setup
			CMeters *meters = new CMeters(MEMORY_NONE);
			CGames *games = new CGames(MEMORY_NONE);
			
			CGame *game = new CGame(MEMORY_NONE);
			CCard *card(new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE));

			game->GetMeters()->SetMeter(gCI, 2L);
			game->GetMeters()->SetMeter(gCO, 2L);
			game->GetMeters()->SetMeter(gGS, 2L);
            game->GetMeters()->SetMeter(gGW, 2L);
			game->GetMeters()->SetMeter(mCrd, 2L);
			game->GetMeters()->SetMeter(mBonusMach, 2L);
			game->GetMeters()->SetMeter(mBonusAtt, 2L);
            game->GetMeters()->SetMeter(mPrgPayAtt, 2L);
			game->GetMeters()->SetMeter(mPrgPayMac, 2L);
			game->GetMeters()->SetMeter(gBW, 2L);
			game->GetMeters()->SetMeter(gJP, 2L);

			games->AddGame(game);
			CPlayerInfo *playerInfo = new CPlayerInfo();

			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const bool goodbyeEnabled(false);

			playerInfo->SetName("ABC");
			playerInfo->SetPlayerId(12345);

			CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, goodbyeEnabled, games);

			meters->SetMeter(gCI, 1L);
			meters->SetMeter(gCO, 1L);
			meters->SetMeter(gGS, 2L);
            meters->SetMeter(gGW, 2L);
			meters->SetMeter(mCrd, 3L);
			meters->SetMeter(mBonusMach, 4L);
			meters->SetMeter(mBonusAtt, 5L);
            meters->SetMeter(mPrgPayAtt, 6L);
			meters->SetMeter(mPrgPayMac, 6L);
			meters->SetMeter(gBW, 7L);
			meters->SetMeter(gJP, 7L);
			
			obj.SetSessionStartMeters(meters); 
			//Tests

			string fullSessionExpected = "{\"PlayerID\":12345,\"CardNumber\":\"\",\"DisplayName\":\"ABC\",\"SessionPoints\":0,\"BonusPoints\":0,\"Countdown\":30000,\"SessionMeters\":{\"CoinInDelta\":1,\"CoinOutDelta\":1,\"GameStarts\":0,\"GamesWon\":0,\"CreditMeter\":2,\"ExternalBonusMachinePay\":2,\"ExternalBonusAttendantPay\":3,\"ProgressiveAttendantPay\":4,\"ProgressiveMachinePay\":4,\"LegacyBonus\":5,\"Jackpot Handpay\":5}}";
			string fullSession = ResponseForPlayerSessionFullSessionTest(obj);

			ASSERT_STREQ(fullSessionExpected.c_str(), fullSession.c_str());
            			
			delete games;			
		}

        TEST_F(Player7, ResponseForPlayerSessionUnFlushedSession)
		{
			//Setup
			CMeters *meters = new CMeters(MEMORY_NONE);
			CGames *games = new CGames(MEMORY_NONE);
			
			CGame *game = new CGame(MEMORY_NONE);
			CCard *card(new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE));

			game->GetMeters()->SetMeter(gCI, 2L);
			game->GetMeters()->SetMeter(gCO, 2L);
			game->GetMeters()->SetMeter(gGS, 2L);
            game->GetMeters()->SetMeter(gGW, 2L);
			game->GetMeters()->SetMeter(mCrd, 2L);
			game->GetMeters()->SetMeter(mBonusMach, 2L);
			game->GetMeters()->SetMeter(mBonusAtt, 2L);
            game->GetMeters()->SetMeter(mPrgPayAtt, 2L);
			game->GetMeters()->SetMeter(mPrgPayMac, 2L);
			game->GetMeters()->SetMeter(gBW, 2L);
			game->GetMeters()->SetMeter(gJP, 2L);

			games->AddGame(game);
			CPlayerInfo *playerInfo = new CPlayerInfo();

			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const bool goodbyeEnabled(false);

			playerInfo->SetName("ABC");
			playerInfo->SetPlayerId(12345);

			CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, goodbyeEnabled, games);

			meters->SetMeter(gCI, 1L);
			meters->SetMeter(gCO, 1L);
			meters->SetMeter(gGS, 2L);
            meters->SetMeter(gGW, 2L);
			meters->SetMeter(mCrd, 3L);
			meters->SetMeter(mBonusMach, 4L);
			meters->SetMeter(mBonusAtt, 5L);
            meters->SetMeter(mPrgPayAtt, 6L);
			meters->SetMeter(mPrgPayMac, 6L);
			meters->SetMeter(gBW, 7L);
			meters->SetMeter(gJP, 7L);
			
			obj.SetSessionStartMetersCurrent(meters); 
			//Tests

			string fullSessionExpected = "{\"PlayerID\":12345,\"CardNumber\":\"\",\"DisplayName\":\"ABC\",\"SessionPoints\":0,\"BonusPoints\":0,\"Countdown\":30000,\"SessionMeters\":{\"CoinInDelta\":1,\"CoinOutDelta\":1,\"GameStarts\":0,\"GamesWon\":0,\"CreditMeter\":2,\"ExternalBonusMachinePay\":2,\"ExternalBonusAttendantPay\":3,\"ProgressiveAttendantPay\":4,\"ProgressiveMachinePay\":4,\"LegacyBonus\":5,\"Jackpot Handpay\":5}}";
			string fullSession = ResponseForPlayerSessionUnFlushedSessionTest(obj);

			ASSERT_STREQ(fullSessionExpected.c_str(), fullSession.c_str());
			
			delete games;			
		}

        TEST_F(Player7, ResponseForPlayerSessionExtUnFlushedSession)
		{
			//Setup
			CMeters *meters = new CMeters(MEMORY_NONE);
			CGames *games = new CGames(MEMORY_NONE);
			
			CGame *game = new CGame(MEMORY_NONE);
			CCard *card(new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE));

			game->GetMeters()->SetMeter(mCD, 20L);
			game->GetMeters()->SetMeter(mD1, 5L);
			game->GetMeters()->SetMeter(mD2, 15L);
			game->GetMeters()->SetMeter(mD5, 10L);

			games->AddGame(game);
			CPlayerInfo *playerInfo = new CPlayerInfo();

			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const bool goodbyeEnabled(false);

			playerInfo->SetName("ABC");
			playerInfo->SetPlayerId(12345);

			CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, goodbyeEnabled, games);

			meters->SetMeter(mCD, 10L);
			meters->SetMeter(mD1, 10L);
			meters->SetMeter(mD2, 10L);
			meters->SetMeter(mD5, 5L);
			
			obj.SetSessionStartMetersCurrent(meters); 
			//Tests

			string fullSessionExpected = "{\"SessionMeters\":{\"CoinDropDelta\":10,\"OneBillDelta\":5,\"TwoBillDelta\":5,\"FiveBillDelta\":5,\"TenBillDelta\":0,\"TwentyBillDelta\":0,\"HundredBillDelta\":0,\"TicketInDelta\":0,\"TicketOutDelta\":0,\"TrueCoinInDelta\":0,\"BillDropDelta\":0,\"TotalHandPayDelta\":0,\"TicketPromoInDelta\":0,\"HandPayJackpotReceiptDelta\":0,\"ActualCoinDropDelta\":0,\"TicketPromoOutDelta\":0,\"CancelCreditDelta\":0,\"HandPayCancelCreditDelta\":0,\"SlotDoorCountDelta\":0,\"AFTCashableToGameDelta\":0,\"AFTRestrictedToGameDelta\":0,\"AFTNonRestrictedToGameDelta\":0,\"AFTCashableToHostDelta\":0,\"AFTRestrictedToHostDelta\":0,\"AFTNonRestrictedToHostDelta\":0,\"AFTDebitToGameDelta\":0,\"PhysicalCoinInDelta\":0,\"PhysicalCoinOutDelta\":0,\"RestrictedAmountPlayedDelta\":0,\"WeightedAverageTheoPayPercent\":-1,\"TotalDropDelta\":0,\"CashableTicketInCreditsDelta\":0}}";;
			string fullSession = ResponseForPlayerSessionExtUnFlushedSessionTest(obj);

			ASSERT_STREQ(fullSessionExpected.c_str(), fullSession.c_str());
			
			delete games;			
		}

        TEST_F(Player7, ResponseForPlayerSessionExtFullSession)
		{
			//Setup
			CMeters *meters = new CMeters(MEMORY_NONE);
			CGames *games = new CGames(MEMORY_NONE);
			
			CGame *game = new CGame(MEMORY_NONE);
			CCard *card(new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE));

			game->GetMeters()->SetMeter(mCD, 20L);
			game->GetMeters()->SetMeter(mD1, 5L);
			game->GetMeters()->SetMeter(mD2, 15L);
			game->GetMeters()->SetMeter(mD5, 10L);

			games->AddGame(game);
			CPlayerInfo *playerInfo = new CPlayerInfo();

			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const bool goodbyeEnabled(false);

			playerInfo->SetName("ABC");
			playerInfo->SetPlayerId(12345);

			CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, goodbyeEnabled, games);

			meters->SetMeter(mCD, 10L);
			meters->SetMeter(mD1, 10L);
			meters->SetMeter(mD2, 10L);
			meters->SetMeter(mD5, 5L);
			
			obj.SetSessionStartMeters(meters); 
			//Tests

			string fullSessionExpected = "{\"SessionMeters\":{\"CoinDropDelta\":10,\"OneBillDelta\":5,\"TwoBillDelta\":5,\"FiveBillDelta\":5,\"TenBillDelta\":0,\"TwentyBillDelta\":0,\"HundredBillDelta\":0,\"TicketInDelta\":0,\"TicketOutDelta\":0,\"TrueCoinInDelta\":0,\"BillDropDelta\":0,\"TotalHandPayDelta\":0,\"TicketPromoInDelta\":0,\"HandPayJackpotReceiptDelta\":0,\"ActualCoinDropDelta\":0,\"TicketPromoOutDelta\":0,\"CancelCreditDelta\":0,\"HandPayCancelCreditDelta\":0,\"SlotDoorCountDelta\":0,\"AFTCashableToGameDelta\":0,\"AFTRestrictedToGameDelta\":0,\"AFTNonRestrictedToGameDelta\":0,\"AFTCashableToHostDelta\":0,\"AFTRestrictedToHostDelta\":0,\"AFTNonRestrictedToHostDelta\":0,\"AFTDebitToGameDelta\":0,\"PhysicalCoinInDelta\":0,\"PhysicalCoinOutDelta\":0,\"RestrictedAmountPlayedDelta\":0,\"WeightedAverageTheoPayPercent\":-1,\"TotalDropDelta\":0,\"CashableTicketInCreditsDelta\":0}}";;
			string fullSession = ResponseForPlayerSessionExtFullSessionTest(obj);

			ASSERT_STREQ(fullSessionExpected.c_str(), fullSession.c_str());
			
			delete games;			
		}

		
		/// <summary>
		/// Code coverage test for:
		/// 	CSessionPoints::CalculateSessionPointsAndCountdown()
		/// Test for new countdown value wraparound case when:
		/// 	 decrement is less than countdown
		/// 	 and initial session points exists
		/// </summary>
		TEST_F(Player7, CalculateSessionPointsAndCountdown_InitialSessionPointsSet01)
		{
			const DWORD gameSessionSequenceNumber(40);
			const DWORD decrement(10);
			const DWORD initialSessionPts(2);
			const DWORD countdown(50);
			const DWORD multiplier(1);
			const DWORD divider(200);
			bool bonusPointsAwardedResult(false);
		
			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);
			const DWORD currentCountdownExpected(countdown - decrement);
			const DWORD currentSessionpointsExpected (initialSessionPts + (countdown - decrement) / divider);
			
			CPlayer player;
			SetInitialSessionPoints (player, initialSessionPts);
			PlayerDecrementCountdown(player, gameSessionSequenceNumber, multiplier, divider, decrement, true, 0, countdown, bonusPointsAwardedResult);
			ASSERT_EQ(player.GetCurrentCountdown(), currentCountdownExpected);
			ASSERT_EQ(player.GetSessionPoints(), currentSessionpointsExpected);
		
			// Cleanup.
		}
		
		/// <summary>
		/// Code coverage test for:
		/// 	CSessionPoints::CalculateSessionPointsAndCountdown()
		/// Test for new countdown value wraparound case when:
		/// 	 decrement is more than countdown+divider
		/// 	 and initial session points exists
		/// </summary>
		TEST_F(Player7, CalculateSessionPointsAndCountdown_InitialSessionPointsSet02)
		{
			const DWORD gameSessionSequenceNumber(40);
			const DWORD decrement(160);
			const DWORD initialSessionPts(2);
			const DWORD countdown(50);
			const DWORD multiplier(1);
			const DWORD divider(200);
			bool bonusPointsAwardedResult(false);
		
			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);
			const DWORD currentCountdownExpected((divider + countdown - decrement % divider) % divider);
			const DWORD currentSessionpointsExpected (initialSessionPts + (decrement + divider - countdown) / divider);
			
			CPlayer player;
			SetInitialSessionPoints (player, initialSessionPts);
			PlayerDecrementCountdown(player, gameSessionSequenceNumber, multiplier, divider, decrement, true, 0, countdown, bonusPointsAwardedResult);
			ASSERT_EQ(player.GetCurrentCountdown(), currentCountdownExpected);
			ASSERT_EQ(player.GetSessionPoints(), currentSessionpointsExpected);
		
			// Cleanup.
		}
		
		/// <summary>
		/// Code coverage test for:
		/// 	CSessionPoints::GameStart()
		/// Test for Gamestart return value when:
		///		four subgame changes with different gamenumber/denom combinations
		/// </summary>
		TEST_F(Player7, GameStart_Test01)
		{
			const DWORD gameSessionSequenceNumber1(40);
			const DWORD subgameGamenumber1 (8);
			const DWORD subgameDenom1(10);

			CPlayer player;

			SubgameSessionParameters subgameSessionParams1;
			subgameSessionParams1.m_gameSessionSequenceNumber = gameSessionSequenceNumber1;
			subgameSessionParams1.m_gameNumber = subgameGamenumber1;	
			subgameSessionParams1.m_denom = subgameDenom1;
			ASSERT_TRUE (player.SetGameSession(subgameSessionParams1));
			ASSERT_TRUE(player.GameStart()) << "First Game Start on first play should return true";
			ASSERT_FALSE(player.GameStart()) << "First Game Start on second play game should return false";
			
			const DWORD gameSessionSequenceNumber2(41);
			const DWORD subgameGamenumber2 (6);	// game number changed
			const DWORD subgameDenom2(100);		// denom changed

			SubgameSessionParameters subgameSessionParams2;
			subgameSessionParams2.m_gameSessionSequenceNumber = gameSessionSequenceNumber2;
			subgameSessionParams2.m_gameNumber = subgameGamenumber2;	
			subgameSessionParams2.m_denom = subgameDenom2;
			ASSERT_TRUE (player.SetGameSession(subgameSessionParams2));
			ASSERT_TRUE(player.GameStart()) << "First Game Start on first play should return true";
			ASSERT_FALSE(player.GameStart()) << "First Game Start on second play game should return false";
		
			const DWORD gameSessionSequenceNumber3(42);
			const DWORD subgameGamenumber3 (8);	// Changed game number
			const DWORD subgameDenom3(100);		// denom unchanged
			
			SubgameSessionParameters subgameSessionParams3;
			subgameSessionParams3.m_gameSessionSequenceNumber = gameSessionSequenceNumber3;
			subgameSessionParams3.m_gameNumber = subgameGamenumber3;	
			subgameSessionParams3.m_denom = subgameDenom3;
			ASSERT_TRUE (player.SetGameSession(subgameSessionParams3));
			ASSERT_TRUE(player.GameStart()) << "First Game Start on first play should return true";
			ASSERT_FALSE(player.GameStart()) << "First Game Start on second play game should return false";

			const DWORD gameSessionSequenceNumber4(43);
			const DWORD subgameGamenumber4 (8);	//game number unchanged
			const DWORD subgameDenom4(10);		//denom changed
			
			SubgameSessionParameters subgameSessionParams4;
			subgameSessionParams4.m_gameSessionSequenceNumber = gameSessionSequenceNumber4;
			subgameSessionParams4.m_gameNumber = subgameGamenumber4;	
			subgameSessionParams4.m_denom = subgameDenom4;
			ASSERT_TRUE (player.SetGameSession(subgameSessionParams4));
			ASSERT_TRUE(player.GameStart()) << "First Game Start on first play should return true";
			ASSERT_FALSE(player.GameStart()) << "First Game Start on second play game should return false";
		
			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		/// 	CSessionPoints::GameStart()
		/// Test for Gamestart return value when:
		///		three subgame changes with third one back to first subgame
		/// </summary>
		TEST_F(Player7, GameStart_Test02)
		{
			const DWORD gameSessionSequenceNumber1(40);
			const DWORD subgameGamenumber1 (8);
			const DWORD subgameDenom1(10);

			CPlayer player;

			SubgameSessionParameters subgameSessionParams1;
			subgameSessionParams1.m_gameSessionSequenceNumber = gameSessionSequenceNumber1;
			subgameSessionParams1.m_gameNumber = subgameGamenumber1;	
			subgameSessionParams1.m_denom = subgameDenom1;
			ASSERT_TRUE (player.SetGameSession(subgameSessionParams1));
			ASSERT_TRUE(player.GameStart()) << "First Game Start on first play should return true";
			ASSERT_FALSE(player.GameStart()) << "First Game Start on second play game should return false";
			
			const DWORD gameSessionSequenceNumber2(41);
			const DWORD subgameGamenumber2 (6);	// game number changed
			const DWORD subgameDenom2(100);		// denom changed

			SubgameSessionParameters subgameSessionParams2;
			subgameSessionParams2.m_gameSessionSequenceNumber = gameSessionSequenceNumber2;
			subgameSessionParams2.m_gameNumber = subgameGamenumber2;	
			subgameSessionParams2.m_denom = subgameDenom2;
			ASSERT_TRUE (player.SetGameSession(subgameSessionParams2));
		
			const DWORD gameSessionSequenceNumber3(42);
			const DWORD subgameGamenumber3 (8);	// Changed game number
			const DWORD subgameDenom3(10);		// denom unchanged
			
			SubgameSessionParameters subgameSessionParams3;
			subgameSessionParams3.m_gameSessionSequenceNumber = gameSessionSequenceNumber3;
			subgameSessionParams3.m_gameNumber = subgameGamenumber3;	
			subgameSessionParams3.m_denom = subgameDenom3;
			ASSERT_TRUE (player.SetGameSession(subgameSessionParams3));
			ASSERT_FALSE(player.GameStart()) << "As back to first one without play so First Game Start should return false";
			ASSERT_FALSE(player.GameStart()) << "First Game Start on second play game should return false";
		
			// Cleanup.
		}

	/// <summary>
	/// Code coverage test for:
	/// 	CSessionPoints::GameStart()
	/// Test for Gamestart return value when:
	/// 	four subgame changes with fourth one back to first subgame with no play in between
	/// </summary>
	TEST_F(Player7, GameStart_Test03)
	{
		const DWORD gameSessionSequenceNumber1(40);
		const DWORD subgameGamenumber1 (8);
		const DWORD subgameDenom1(10);
	
		CPlayer player;
	
		SubgameSessionParameters subgameSessionParams1;
		subgameSessionParams1.m_gameSessionSequenceNumber = gameSessionSequenceNumber1;
		subgameSessionParams1.m_gameNumber = subgameGamenumber1;	
		subgameSessionParams1.m_denom = subgameDenom1;
		ASSERT_TRUE (player.SetGameSession(subgameSessionParams1));
		ASSERT_TRUE(player.GameStart()) << "First Game Start on first play should return true";
		ASSERT_FALSE(player.GameStart()) << "First Game Start on second play game should return false";
		
		const DWORD gameSessionSequenceNumber2(41);
		const DWORD subgameGamenumber2 (6); // game number changed
		const DWORD subgameDenom2(100); 	// denom changed
	
		SubgameSessionParameters subgameSessionParams2;
		subgameSessionParams2.m_gameSessionSequenceNumber = gameSessionSequenceNumber2;
		subgameSessionParams2.m_gameNumber = subgameGamenumber2;	
		subgameSessionParams2.m_denom = subgameDenom2;
		ASSERT_TRUE (player.SetGameSession(subgameSessionParams2));

		const DWORD gameSessionSequenceNumber3(42);
		const DWORD subgameGamenumber3 (16); // game number changed
		const DWORD subgameDenom3(10); 	// denom changed
	
		SubgameSessionParameters subgameSessionParams3;
		subgameSessionParams3.m_gameSessionSequenceNumber = gameSessionSequenceNumber3;
		subgameSessionParams3.m_gameNumber = subgameGamenumber3;	
		subgameSessionParams3.m_denom = subgameDenom3;
		ASSERT_TRUE (player.SetGameSession(subgameSessionParams3));
	
	
		const DWORD gameSessionSequenceNumber4(43);
		const DWORD subgameGamenumber4 (8); // Changed game number
		const DWORD subgameDenom4(10);		// denom unchanged
		
		SubgameSessionParameters subgameSessionParams4;
		subgameSessionParams4.m_gameSessionSequenceNumber = gameSessionSequenceNumber4;
		subgameSessionParams4.m_gameNumber = subgameGamenumber4;	
		subgameSessionParams4.m_denom = subgameDenom4;
		ASSERT_TRUE (player.SetGameSession(subgameSessionParams4));
		ASSERT_FALSE(player.GameStart()) << "As back to first one without play so First Game Start should return false";
		ASSERT_FALSE(player.GameStart()) << "First Game Start on second play game should return false";
	
		// Cleanup.
	}

	/// <summary>
	/// Code coverage test for:
	/// 	CSessionPoints::GameStart()
	/// Test for Gamestart return value when:
	/// 	four subgame changes with fourth one back to first subgame with play in second game in between
	/// </summary>
	TEST_F(Player7, GameStart_Test04)
	{
		const DWORD gameSessionSequenceNumber1(40);
		const DWORD subgameGamenumber1 (8);
		const DWORD subgameDenom1(10);
	
		CPlayer player;
	
		SubgameSessionParameters subgameSessionParams1;
		subgameSessionParams1.m_gameSessionSequenceNumber = gameSessionSequenceNumber1;
		subgameSessionParams1.m_gameNumber = subgameGamenumber1;	
		subgameSessionParams1.m_denom = subgameDenom1;
		ASSERT_TRUE (player.SetGameSession(subgameSessionParams1));
		ASSERT_TRUE(player.GameStart()) << "First Game Start on first play should return true";
		ASSERT_FALSE(player.GameStart()) << "First Game Start on second play game should return false";
		
		const DWORD gameSessionSequenceNumber2(41);
		const DWORD subgameGamenumber2 (6); // game number changed
		const DWORD subgameDenom2(100); 	// denom changed
	
		SubgameSessionParameters subgameSessionParams2;
		subgameSessionParams2.m_gameSessionSequenceNumber = gameSessionSequenceNumber2;
		subgameSessionParams2.m_gameNumber = subgameGamenumber2;	
		subgameSessionParams2.m_denom = subgameDenom2;
		ASSERT_TRUE (player.SetGameSession(subgameSessionParams2));
		ASSERT_TRUE(player.GameStart()) << "First Game Start on first play should return true";
		ASSERT_FALSE(player.GameStart()) << "First Game Start on second play game should return false";
		
		const DWORD gameSessionSequenceNumber3(42);
		const DWORD subgameGamenumber3 (16); // game number changed
		const DWORD subgameDenom3(10); 	// denom changed
	
		SubgameSessionParameters subgameSessionParams3;
		subgameSessionParams3.m_gameSessionSequenceNumber = gameSessionSequenceNumber3;
		subgameSessionParams3.m_gameNumber = subgameGamenumber3;	
		subgameSessionParams3.m_denom = subgameDenom3;
		ASSERT_TRUE (player.SetGameSession(subgameSessionParams3));
	
	
		const DWORD gameSessionSequenceNumber4(43);
		const DWORD subgameGamenumber4 (8); // Changed game number
		const DWORD subgameDenom4(10);		// denom unchanged
		
		SubgameSessionParameters subgameSessionParams4;
		subgameSessionParams4.m_gameSessionSequenceNumber = gameSessionSequenceNumber4;
		subgameSessionParams4.m_gameNumber = subgameGamenumber4;	
		subgameSessionParams4.m_denom = subgameDenom4;
		ASSERT_TRUE (player.SetGameSession(subgameSessionParams4));
		ASSERT_TRUE(player.GameStart()) << "As back to first one without play so First Game Start should return false";
		ASSERT_FALSE(player.GameStart()) << "First Game Start on second play game should return false";
	
		// Cleanup.
	}

	
	TEST_F(Player7, GameStart_Test05)
	{
		const DWORD gameSessionSequenceNumber1(40);
		const DWORD subgameGamenumber1 (8);
		const DWORD subgameDenom1(10);

		CPlayer player;

		SubgameSessionParameters subgameSessionParams1;
		subgameSessionParams1.m_gameSessionSequenceNumber = gameSessionSequenceNumber1;
		subgameSessionParams1.m_gameNumber = subgameGamenumber1;	
		subgameSessionParams1.m_denom = subgameDenom1;
		ASSERT_TRUE (player.SetGameSession(subgameSessionParams1));
		ASSERT_TRUE(player.GameStart()) << "First Game Start on first play should return true";
		ASSERT_FALSE(player.GameStart()) << "First Game Start on second play game should return false";

	}

	TEST_F(Player7, GameStart_Test06)
	{
		// gameSessionSequenceNumber 0 indicates default info coming from player response
		const DWORD gameSessionSequenceNumber1(0);
		const DWORD subgameGamenumber1 (8);
		const DWORD subgameDenom1(10);

		CPlayer player;

		SubgameSessionParameters subgameSessionParams1;
		subgameSessionParams1.m_gameSessionSequenceNumber = gameSessionSequenceNumber1;
		subgameSessionParams1.m_gameNumber = subgameGamenumber1;	
		subgameSessionParams1.m_denom = subgameDenom1;
		ASSERT_TRUE (player.SetGameSession(subgameSessionParams1));
		ASSERT_FALSE(player.GameStart()) << "First Game Start on first play should return false";
		ASSERT_FALSE(player.GameStart()) << "First Game Start on second play game should return false";

	}

	/// <summary>
	/// CPlayer::SetGameSession when no existing SubgameSession exists 
	/// </summary>
	TEST_F(Player7, SetGameSession_NoExistingSubgameSession)
	{
		ASSERT_NE(GetPlayer(), nullptr);
		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_gameSessionSequenceNumber  = 2;
			
		ASSERT_TRUE(GetPlayer()->SetGameSession(subgameSessionParams));
	}		
	
	/// <summary>
	/// CPlayer::SetGameSession when there exists atleast one SubgameSession 
	/// </summary>
	TEST_F(Player7, SetGameSession_MultipleSetGameSessionTest)
	{
		ASSERT_NE(GetPlayer(), nullptr);
		
		SubgameSessionParameters subgameSessionParams1;
		SubgameSessionParameters subgameSessionParams2;
		
		subgameSessionParams1.m_gameSessionSequenceNumber  = 2;
		subgameSessionParams2.m_gameSessionSequenceNumber  = 12;
		
		ASSERT_TRUE(GetPlayer()->SetGameSession(subgameSessionParams1));
		ASSERT_TRUE(GetPlayer()->SetGameSession(subgameSessionParams2));
		
	}

	/// <summary>
	/// CPlayer::ApplySubgameSelectionResponse should fail if called on non-existing 
	/// GameSessionSequenceNumber
	/// </summary>
	TEST_F(Player7, ApplySubgameSelectionResponse_NonExistentGameSessionSequenceNumber)
	{
		ASSERT_NE(GetPlayer(), nullptr);
		
		SubgameSessionParameters subgameSessionParams;
		SubgameSelectionResponse subgameSelectionResponse;
		
		subgameSessionParams.m_gameSessionSequenceNumber  = 2;
		subgameSelectionResponse.m_subgameSessionParams.m_gameSessionSequenceNumber  = 12;
	
		ASSERT_TRUE(GetPlayer()->SetGameSession(subgameSessionParams));
		
		ASSERT_FALSE(GetPlayer()->ApplySubgameSelectionResponse(&subgameSelectionResponse))
			<< "No SubgameSession object should exist for GameSessionSequenceNumber 12";
	}		
	
	/// <summary>
	/// CPlayer::ApplySubgameSelectionResponse should pass if called on existing GameSessionSequenceNumber.
	/// </summary>
	TEST_F(Player7, ApplySubgameSelectionResponse_ExistentGameSessionSequenceNumber)
	{
		ASSERT_NE(GetPlayer(), nullptr);
					
		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_gameSessionSequenceNumber  = 2;
	
		SubgameSelectionResponse subgameSelectionResponse;
		subgameSelectionResponse.m_subgameSessionParams.m_gameSessionSequenceNumber  = 2;
	
		ASSERT_TRUE(GetPlayer()->SetGameSession(subgameSessionParams));
		
		ASSERT_TRUE(GetPlayer()->ApplySubgameSelectionResponse(&subgameSelectionResponse))
			<< "SubgameSession object should exist for GameSessionSequenceNumber 2";
	}	
	
	/// <summary>
	/// CPlayer::GameStart should retrun false when no subgame session exists
	/// </summary>
	TEST_F(Player7, GameStart_NoExistingSubgameSession)
	{
		ASSERT_NE(GetPlayer(), nullptr);
	
		ASSERT_FALSE(GetPlayer()->GameStart());
	}
		
	/// <summary>
	/// CPlayer::GameStart test after multiple CPlayer::SetGameSession. In every case, it should 
	/// retrun return true on first game start and then false on subsequent calls
	/// </summary>
	TEST_F(Player7, GameStart_MultipleSetGameSessionTest)
	{
		ASSERT_NE(GetPlayer(), nullptr);
		
		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_denom = 1;
		subgameSessionParams.m_gameSessionSequenceNumber  = 5;
	
		ASSERT_TRUE(GetPlayer()->SetGameSession(subgameSessionParams));
	
		ASSERT_TRUE(GetPlayer()->GameStart()) << "Test 1: First Gamestart should be true";
		ASSERT_FALSE(GetPlayer()->GameStart()) << "Second onwards Gamestart should return false";

		SubgameSessionParameters subgameSessionParams2;
		subgameSessionParams2.m_denom = 2;
		subgameSessionParams2.m_gameSessionSequenceNumber  = 15;
		
		ASSERT_TRUE(GetPlayer()->SetGameSession(subgameSessionParams2));
		
		ASSERT_TRUE(GetPlayer()->GameStart()) << "Test 2: First Gamestart should be true";
		ASSERT_FALSE(GetPlayer()->GameStart()) << "Second onwards Gamestart should return false";
	}
	
	/// <summary>
	/// Test for sequence SetGameSession -> GameStart -> ApplySubgameSelectionResponse
	/// </summary>
	TEST_F(Player7, SetGameSession_GameStart_ApplySubgameSelectionResponse)
	{
		ASSERT_NE(GetPlayer(), nullptr);
		
		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_denom = 1;
		subgameSessionParams.m_gameSessionSequenceNumber  = 5;
	
		ASSERT_TRUE(GetPlayer()->SetGameSession(subgameSessionParams));
	
		ASSERT_TRUE(GetPlayer()->GameStart()) << "First Gamestart should be true";
		ASSERT_FALSE(GetPlayer()->GameStart()) << "Second onwards Gamestart should return false";

		SubgameSelectionResponse subgameSelectionResponse;
		subgameSelectionResponse.m_subgameSessionParams.m_denom = 2;
		subgameSelectionResponse.m_subgameSessionParams.m_gameSessionSequenceNumber  = 5;
		subgameSessionParams.m_multiplier = 3;
		
		ASSERT_TRUE(GetPlayer()->ApplySubgameSelectionResponse(&subgameSelectionResponse))
			<< "SubgameSession object should exist for GameSessionSequenceNumber 12";
		
		ASSERT_FALSE(GetPlayer()->GameStart()) << "It's third Gamestart, should be false";
		ASSERT_FALSE(GetPlayer()->GameStart()) << "Second onwards Gamestart should return false";
	}

	TEST_F(Player7, GetBucketDisplayNameTest)
	{
		ASSERT_NE(GetPlayer(), nullptr);

		// Setup.
		const std::string legacyPointsText("Points");
		const std::string legacyPromosText("Promos");
		const std::string legacyCompsText("Comps");
		const std::string legacyTierPointsText("Tier Points");

		const DWORD legacyPointsId = 101;
		const DWORD legacyPromosId = 102;
		const DWORD legacyCompsId = 103;
		const DWORD legacyTierPointsId = 104;
		 
		PlayerResponse playerReponse = { { 1, 2, 3, 4, 5 }, 6, 7, false, false, false, 8, 9, 10, 0, (byte*)NULL, 0, (byte*)NULL, 
			13, 14, 15, 0, 17, 18, 19, 0, (byte*)NULL, 100, 0, 10, AccountingType::ACCOUNTING_COINS_IN, 0,
			legacyPointsId, legacyCompsId, legacyPromosId, legacyTierPointsId, {}, "", true};

		CPlayerInfo *playerInfo = new CPlayerInfo(playerReponse);
		GetPlayer()->SetPlayerInfo(playerInfo);

		// Perform tests & verify result.
		std::string displayName;
		GetPlayer()->GetBucketDisplayName(0, displayName);
		ASSERT_EQ(displayName, legacyPointsText);

		GetPlayer()->GetBucketDisplayName(legacyPointsId, displayName);
		ASSERT_EQ(displayName, legacyPointsText);

		GetPlayer()->GetBucketDisplayName(legacyPromosId, displayName);
		ASSERT_EQ(displayName, legacyPromosText);

		GetPlayer()->GetBucketDisplayName(legacyCompsId, displayName);
		ASSERT_EQ(displayName, legacyCompsText);

		GetPlayer()->GetBucketDisplayName(legacyTierPointsId, displayName);
		ASSERT_EQ(displayName, legacyTierPointsText);
	}

	TEST_F(Player7, LeftoverCashTransferStatus_Get_Set_Test)
	{
		// default value check.
		ASSERT_EQ(GetPlayer()->GetLeftoverCashTransferStatus(), LeftoverCashTransferStatus::LEFTOVER_CASH_TRANSFER_NA);

		GetPlayer()->SetLeftoverCashTransferStatus(LeftoverCashTransferStatus::LEFTOVER_CASH_TRANSFER_SUCCEED);
		ASSERT_EQ(GetPlayer()->GetLeftoverCashTransferStatus(), LeftoverCashTransferStatus::LEFTOVER_CASH_TRANSFER_SUCCEED);

		GetPlayer()->SetLeftoverCashTransferStatus(LeftoverCashTransferStatus::LEFTOVER_CASH_TRANSFER_FAILED);
		ASSERT_EQ(GetPlayer()->GetLeftoverCashTransferStatus(), LeftoverCashTransferStatus::LEFTOVER_CASH_TRANSFER_FAILED);
	}

	TEST_F(Player7, GetSubscriptionDataResponse_PlayerInfo_Test1)
	{
		ASSERT_NE(GetPlayer(), nullptr);

		// Setup.
		const byte cardNumber[] = { 'a', 'b', 'c', 'd', 'e' };
		const int cardNumberLen = (int)sizeof(cardNumber);
		CPlayerInfo *playerInfo = new CPlayerInfo();
		playerInfo->SetName("James");
		playerInfo->SetPlayerId(101);
		playerInfo->SetPointsAndCountdown(50, 100, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
		playerInfo->SetTierPoints(12);
		playerInfo->SetCompValue(2200);
		playerInfo->SetCardNumber(cardNumber, cardNumberLen);

		CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);

		config.SetUniverseEnabled(true);
		config.SetDisplayComps(true);
		config.SetDisplayPtsTotalAsCash(true);
		config.SetPBTCashEnabled_Global(true);
		config.SetPBTCashEnabled_local(true);
		config.SetPointsPerDollar(1);
		config.SetShowPointsVsAccountBalance(true);
		GetPlayer()->ConfigSettings(config, egmConfig);
		GetPlayer()->SetPlayerInfo(playerInfo);

		string expectedResponse;
		expectedResponse = R"({"Name":"James",)";
		expectedResponse += R"("IsGuest":false,)";
		expectedResponse += R"("PlayerId":101,)";
		expectedResponse += R"("PBEnabled":true,)";
		expectedResponse += R"("TierPoints":"12",)";
		expectedResponse += R"("CashValueOfPoints":"$50.00",)";
		expectedResponse += R"("CompDollars":"$22.00",)";
		expectedResponse += R"("ExtendedBucketsAvailable":false})";

		// Perform tests & verify result.
		std::string response = GetPlayer()->GetSubscriptionDataResponse("PlayerInfo", "");
		ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
	}

	TEST_F(Player7, GetSubscriptionDataResponse_PlayerInfo_Test2)
	{
		ASSERT_NE(GetPlayer(), nullptr);

		// Setup.
		const byte cardNumber[] = { 'a', 'b', 'c', 'd', 'e' };
		const int cardNumberLen = (int)sizeof(cardNumber);
		CPlayerInfo *playerInfo = new CPlayerInfo();
		playerInfo->SetName("James");
		playerInfo->SetPlayerId(101);
		playerInfo->SetPointsAndCountdown(50, 100, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
		playerInfo->SetTierPoints(12);
		playerInfo->SetCompValue(2200);
		playerInfo->SetCardNumber(cardNumber, cardNumberLen);

		CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);

		config.SetUniverseEnabled(true);
		config.SetDisplayComps(true);
		config.SetDisplayPtsTotalAsCash(true);
		config.SetPointsPerDollar(1);
		config.SetShowPointsVsAccountBalance(false);
		GetPlayer()->ConfigSettings(config, egmConfig);
		GetPlayer()->SetPlayerInfo(playerInfo);

		string expectedResponse;
		expectedResponse = R"({"Name":"James",)";
		expectedResponse += R"("IsGuest":false,)";
		expectedResponse += R"("PlayerId":101,)";
		expectedResponse += R"("PBEnabled":false,)";
		expectedResponse += R"("TierPoints":"12",)";
		expectedResponse += R"("PointBalance":"50",)";
		expectedResponse += R"("CompDollars":"$22.00",)";
		expectedResponse += R"("ExtendedBucketsAvailable":false})";

		// Perform tests & verify result.
		std::string response = GetPlayer()->GetSubscriptionDataResponse("PlayerInfo", "");
		ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
	}

	TEST_F(Player7, GetSubscriptionDataResponse_PlayerInfo_Test3)
	{
		ASSERT_NE(GetPlayer(), nullptr);

		// Setup.
		CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);

		config.SetUniverseEnabled(true);
		config.SetDisplayComps(false);
		config.SetDisplayPtsTotalAsCash(true);
		config.SetPointsPerDollar(1);
		config.SetShowPointsVsAccountBalance(false);
		GetPlayer()->ConfigSettings(config, egmConfig);

		string expectedResponse;
		expectedResponse = R"({"Name":"Guest",)";
		expectedResponse += R"("IsGuest":true,)";
		expectedResponse += R"("PlayerId":-1,)";
		expectedResponse += R"("PBEnabled":false,)";
		expectedResponse += R"("TierPoints":"",)";
		expectedResponse += R"("PointBalance":"0",)";
		expectedResponse += R"("CompDollars":"",)";
		expectedResponse += R"("ExtendedBucketsAvailable":false})";

		// Perform tests & verify result.
		std::string response = GetPlayer()->GetSubscriptionDataResponse("PlayerInfo", "");
		ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
	}
}
