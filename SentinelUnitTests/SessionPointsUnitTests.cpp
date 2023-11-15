#include "stdafx.h"
#include "SessionPoints.h"
#include "../Hardware/MemoryStorage.h"
#include "OneTimeBonusEvent.h"
#include "MultiplierBonusEvent.h"
#include "SubgameSessionParameters.h"
#include "SubgameSession.h"
#include "Player.h"
#include "Config.h"

#include "gtest/gtest.h"


namespace SentinelUnitTests
{
        class SessionPointsUnitTests : public ::testing::Test
        {
        protected:
        	virtual void SetUp()
        	{
        	}

        	virtual void TearDown()
        	{
        	}
			
			bool CreateNewSubgameSession (CSessionPoints& obj, SubgameSessionParameters &params)
			{
				return obj.CreateNewSubgameSession(params);
			}

			void PlayerDecrementCountdown(CPlayer &player, DWORD gameSessionSequenceNumber, bool setMultiplierDivider, DWORD multiplier, 
				DWORD divider, DWORD decrement, bool isNewPlayer,  DWORD points, DWORD startingCountdown, bool &bonusPointsAwardedResult, CMeters *meters = nullptr, 
				CMeters *orig = nullptr)
			{				
				// Setup.
				if (isNewPlayer)
				{
					const DWORD fallbackMultiplier(DEFAULT_MULTIPLIER + 1);
					const DWORD fallbackDivider(DEFAULT_COUNTDOWN + 1);
								
					CPlayerInfo *playerInfo = new CPlayerInfo();
					playerInfo->SetPointsAndCountdown(points, startingCountdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

					if (setMultiplierDivider)
						playerInfo->SetMultiplierDivider(multiplier, divider);
					
					player.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);
				}
				
				// Perform operation(s) to be tested.
				SubgameSessionParameters subgameSessionParams;
				subgameSessionParams.m_gameSessionSequenceNumber = gameSessionSequenceNumber;

				if(setMultiplierDivider)
				{
					subgameSessionParams.m_divider = divider;	
					subgameSessionParams.m_multiplier = multiplier; 

					SubgameSelectionResponse subgameSelectionResponse;
					subgameSelectionResponse.m_subgameSessionParams = subgameSessionParams;
                    ASSERT_TRUE (player.SetGameSession(subgameSessionParams));
                    ASSERT_TRUE (player.ApplySubgameSelectionResponse(&subgameSelectionResponse));
				}
				
				if (meters == nullptr)
				{
					meters = new CMeters(MEMORY_NONE);
				}

				if (orig == nullptr) 
				{
					orig = new CMeters(MEMORY_NONE);
				}

				orig->SetMeter(gCI, 1, true, false);
				meters->SetMeter(gCI, 1 + decrement, true, false);
				bonusPointsAwardedResult = player.DecrementCountdown(meters, orig, true);
				delete meters;
				delete orig;

			}

			void TestPlayerResult(bool bonusPointsAwardedResult, bool bonusPointsAwardedExpected, 
				DWORD currentCountdownResult, DWORD currentCountdownExpected,
				DWORD currentSessionpointsResult, DWORD currentSessionpointsExpected,
				DWORD lineNumber)
			{
				// Test #1.
				{
					bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
					string failedMsg;
					failedMsg = FormatString(_T("(FAILED at line %u) bonusPointsAwardedResult=%d expected=%d in %s"),
						lineNumber, 
						(int)bonusPointsAwardedResult,
						(int)bonusPointsAwardedExpected,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}
				
				// Test #2.
				{
					bool areEqual(currentCountdownResult == currentCountdownExpected);
					string failedMsg;
					failedMsg = FormatString(_T("(FAILED at line %u) currentCountdownResult=%u expected=%u in %s"),
						lineNumber, 
						(unsigned)currentCountdownResult,
						(unsigned)currentCountdownExpected,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}
				
				// Test #3.
				{
					bool areEqual(currentSessionpointsResult == currentSessionpointsExpected);
					string failedMsg;
					failedMsg = FormatString(_T("(FAILED at line %u) currentSessionpointsResult=%u expected=%u in %s"),
						lineNumber, 
						(unsigned)currentSessionpointsResult,
						(unsigned)currentSessionpointsExpected,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}
			
			}
			
        };

		TEST_F(SessionPointsUnitTests, SessionPointsUnitTests_ConstTest)
		{
            CSessionPoints obj;
			
            ASSERT_EQ(obj.GetSessionPoints(), 0) << "Constructor m_sessionPoints";
            ASSERT_EQ(obj.GetSessionPointsFlushed(), 0) << "Constructor m_sessionPointsFlushed";
            ASSERT_EQ(NULL, obj.GetBonusEvents()) << "Constructor m_bonusEvents";
        	ASSERT_EQ(NULL, obj.GetSessionStartMeters()) << "Constructor m_sessionStartMeters";
        	ASSERT_EQ(NULL, obj.GetSessionStartMetersCurrent()) << "Constructor m_sessionStartMetersCurrent";
		}

        TEST_F(SessionPointsUnitTests, SessionPointsUnitTests_CopyConstTest)
		{
            CSessionPoints obj;
			obj.SetSessionPoints(5);
            obj.SetSessionPointsFlushed(2);
            obj.SetBonusEvents(new CBonusEvents());

            CSessionPoints obj1(obj);

            ASSERT_EQ(obj1.GetSessionPoints(), obj.GetSessionPoints()) << "Copy Constructor m_sessionPoints";
            ASSERT_EQ(obj1.GetSessionPointsFlushed(), obj.GetSessionPointsFlushed()) << "Copy Constructor m_sessionPointsFlushed";
            ASSERT_EQ(obj1.GetBonusEvents()->GetBonusPoints(), obj.GetBonusEvents()->GetBonusPoints()) << "Copy Constructor m_bonusEvents->GetBonusPoints()";
            ASSERT_EQ(obj1.GetBonusEvents()->GetBonusPointsCurrent(), obj.GetBonusEvents()->GetBonusPointsCurrent()) << "Copy Constructor m_bonusEvents->GetBonusPointsCurrent()";
        	ASSERT_EQ(NULL, obj1.GetSessionStartMeters()) << "Copy Constructor m_sessionStartMeters";
        	ASSERT_EQ(NULL, obj1.GetSessionStartMetersCurrent()) << "Copy Constructor m_sessionStartMetersCurrent";
		}

        TEST_F(SessionPointsUnitTests, SessionPointsUnitTests_AssignmentTest)
		{
            CSessionPoints obj;
			obj.SetSessionPoints(5);
            obj.SetSessionPointsFlushed(2);
            obj.SetBonusEvents(new CBonusEvents());

            CSessionPoints obj1 = obj;

            ASSERT_EQ(obj1.GetSessionPoints(), obj.GetSessionPoints()) << "Assignment m_sessionPoints";
            ASSERT_EQ(obj1.GetSessionPointsFlushed(), obj.GetSessionPointsFlushed()) << "Assignment m_sessionPointsFlushed";
            ASSERT_EQ(obj1.GetBonusEvents()->GetBonusPoints(), obj.GetBonusEvents()->GetBonusPoints()) << "Assignment m_bonusEvents->GetBonusPoints()";
            ASSERT_EQ(obj1.GetBonusEvents()->GetBonusPointsCurrent(), obj.GetBonusEvents()->GetBonusPointsCurrent()) << "Assignment m_bonusEvents->GetBonusPointsCurrent()";
        	ASSERT_EQ(NULL, obj1.GetSessionStartMeters()) << "Assignment m_sessionStartMeters";
        	ASSERT_EQ(NULL, obj1.GetSessionStartMetersCurrent()) << "Assignment m_sessionStartMetersCurrent";
		}

        TEST_F(SessionPointsUnitTests, SessionPointsUnitTests_SetSessionPoints)
		{
            CSessionPoints obj;
            obj.SetSessionPoints(5);
            
            ASSERT_EQ(5, obj.GetSessionPoints()) << "SetSessionPoints m_sessionPoints";
         }

        TEST_F(SessionPointsUnitTests, SessionPointsUnitTests_SetSessionPointsFlushed)
		{
            CSessionPoints obj;
            obj.SetSessionPointsFlushed(2);
            
            ASSERT_EQ(2, obj.GetSessionPointsFlushed()) << "SetSessionPointsFlushed m_sessionPointsFlushed";
         }

        TEST_F(SessionPointsUnitTests, SessionPointsUnitTests_SetBonusEvents)
		{
            CSessionPoints obj;
            obj.SetBonusEvents(new CBonusEvents());
            
            ASSERT_FALSE(obj.GetBonusEvents() == NULL) << "SetBonusEvents";
         }

		TEST_F(SessionPointsUnitTests, SessionPointsUnitTests_BuildSessionStartMeters)
		{
			//setup
			CMemoryStorage::SetSprinkles();

			{
				CMemoryStorage::SetInactiveLocation(NVRAM_PLAYER_START_METERS_OFFSET, (BYTE)ACTIVE_LOCATION);
				CMemoryStorage::SetInactiveLocation(NVRAM_PLAYER_START_METERS_CURRENT_OFFSET, (BYTE)ACTIVE_LOCATION);
				CSessionPoints obj;
				obj.BuildSessionStartMeters();
				ASSERT_FALSE(obj.GetSessionStartMeters() == NULL) << "BuildSessionStartMeters1";
				ASSERT_FALSE(obj.GetSessionStartMetersCurrent() == NULL) << "BuildSessionStartMeters2";
			}

			{
				CMemoryStorage::SetInactiveLocation(NVRAM_PLAYER_START_METERS_OFFSET, (BYTE)INACTIVE_LOCATION);
				CMemoryStorage::SetInactiveLocation(NVRAM_PLAYER_START_METERS_CURRENT_OFFSET, (BYTE)INACTIVE_LOCATION);
				CSessionPoints obj;
				obj.BuildSessionStartMeters();
				ASSERT_EQ(NULL, obj.GetSessionStartMeters()) << "BuildSessionStartMeters3";
				ASSERT_EQ(NULL, obj.GetSessionStartMetersCurrent()) << "BuildSessionStartMeters4";
			}

			{
				CMemoryStorage::SetInactiveLocation(NVRAM_PLAYER_START_METERS_OFFSET, (BYTE)ACTIVE_LOCATION);
				CMemoryStorage::SetInactiveLocation(NVRAM_PLAYER_START_METERS_CURRENT_OFFSET, (BYTE)INACTIVE_LOCATION);
				CSessionPoints obj;
				obj.BuildSessionStartMeters();
				ASSERT_FALSE(obj.GetSessionStartMeters() == NULL) << "BuildSessionStartMeters5";
				ASSERT_EQ(NULL, obj.GetSessionStartMetersCurrent()) << "BuildSessionStartMeters6";
			}

			{
				CMemoryStorage::SetInactiveLocation(NVRAM_PLAYER_START_METERS_OFFSET, (BYTE)INACTIVE_LOCATION);
				CMemoryStorage::SetInactiveLocation(NVRAM_PLAYER_START_METERS_CURRENT_OFFSET, (BYTE)ACTIVE_LOCATION);
				CSessionPoints obj;
				obj.BuildSessionStartMeters();
				ASSERT_EQ(NULL, obj.GetSessionStartMeters()) << "BuildSessionStartMeters7";
				ASSERT_FALSE(obj.GetSessionStartMetersCurrent() == NULL) << "BuildSessionStartMeters8";
			}
		}

		TEST_F(SessionPointsUnitTests, SessionPointsUnitTests_GetSessionStartMetersCopy)
		{
            {
                CSessionPoints obj;
                CMeters* meters = new CMeters(MEMORY_NONE);
                meters->SetMeter(gCO, 10);
                meters->SetMeter(mCDr, 1000);
                obj.SetSessionStartMeters(meters);

                CMeters* metersCopy = obj.GetSessionStartMetersCopy();
                ASSERT_FALSE(metersCopy == NULL);
                ASSERT_EQ(1000, meters->GetMeterByNumber(mCDr)->GetValue()) << "GetSessionStartMetersCopy mCDr";
                ASSERT_EQ(10, meters->GetMeterByNumber(gCO)->GetValue()) << "GetSessionStartMetersCopy gCO";
            }
            
            {
                CSessionPoints obj;

                CMeters* metersCopy = obj.GetSessionStartMetersCopy();
                ASSERT_EQ(NULL, metersCopy) << "GetSessionStartMetersCopy m_sessionStartMeters is NULL";
            }
         }

        TEST_F(SessionPointsUnitTests, SessionPointsUnitTests_ResetCurrentBonusData)
		{
            CSessionPoints obj;
            obj.SetBonusEvents(new CBonusEvents());
            CBonusEvents* bonusEvents = obj.GetBonusEvents();
            ASSERT_FALSE(bonusEvents == NULL) << "ResetCurrentBonusData bonusEvents is NULL";

            long id = 1234;
            WORD points = 100;
            time_t start = CUtilities::GetCurrentTime() - 60;
            time_t stop = CUtilities::GetCurrentTime() + 600;
            COneTimeBonusEvent *event = new COneTimeBonusEvent(id, start, stop, points);
            WORD multiplier = 10;
            CMultiplierBonusEvent *mBonus = new CMultiplierBonusEvent(id, start, stop, multiplier);
            bonusEvents->AddEvent(event);
            bonusEvents->AddEvent(mBonus);
            bonusEvents->AwardBonusPoints(10);
            ASSERT_EQ(200, obj.GetBonusPointsCurrent()) << "ResetCurrentBonusData GetBonusPointsCurrent1";
            ASSERT_EQ(200, obj.GetBonusPoints()) << "ResetCurrentBonusData GetBonusPoints1";

            obj.ResetCurrentBonusData();
            ASSERT_EQ(0, obj.GetBonusPointsCurrent()) << "ResetCurrentBonusData GetBonusPointsCurrent2";
            ASSERT_EQ(200, obj.GetBonusPoints()) << "ResetCurrentBonusData GetBonusPoints2";
         }

	/// <summary>
	/// Code coverage test for:
	/// 	CSessionPoints::CalculateSessionPointsAndCountdown()
	/// Test for new countdown value when:
	/// 	 decrement less than current countdown
	/// 	 and starting divider is set
	/// </summary>
	TEST_F(SessionPointsUnitTests, CalculateSessionPointsAndCountdown_Test01)
	{
        const DWORD gameSessionSequenceNumber(40);
		const DWORD decrement(3);
		const DWORD points(0);
		const DWORD countdown(7);
		const DWORD multiplier(1);
		const DWORD divider(270);
		bool bonusPointsAwardedResult(false);
		
		// Test data -- expected results.
		const bool bonusPointsAwardedExpected(false);
		const DWORD currentCountdownExpected(countdown - decrement);
		const DWORD currentSessionpointsExpected((countdown - decrement)/divider);
		
        CPlayer player;
        PlayerDecrementCountdown(player, gameSessionSequenceNumber, true, multiplier, divider, decrement, true, points, countdown, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected, player.GetSessionPoints(), currentSessionpointsExpected , __LINE__);

	}

	/// <summary>
	/// Code coverage test for:
	/// 	CSessionPoints::CalculateSessionPointsAndCountdown()
	/// Test for new countdown value wraparound case when:
	/// 	 decrement is greater than than current countdown+divider
	/// 	 and starting divider is set
	/// </summary>
	TEST_F(SessionPointsUnitTests, CalculateSessionPointsAndCountdown_Test02)
	{
		const DWORD gameSessionSequenceNumber(40);
		const DWORD decrement(600);
		const DWORD points(0);
		const DWORD countdown(7);
		const DWORD multiplier(1);
		const DWORD divider(270);
		bool bonusPointsAwardedResult(false);

		// Test data -- expected results.
		const bool bonusPointsAwardedExpected(false);
		const DWORD currentCountdownExpected((divider + countdown - decrement % divider) % divider);
		const DWORD currentSessionpointsExpected ((decrement + divider - countdown) / divider);
		
        CPlayer player;
        PlayerDecrementCountdown(player, gameSessionSequenceNumber, true, multiplier, divider, decrement, true, points, countdown, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected, player.GetSessionPoints(), currentSessionpointsExpected , __LINE__);

		// Cleanup.
	}


	/// <summary>
	/// Code coverage test for:
	/// 	CSessionPoints::CalculateSessionPointsAndCountdown()
	/// Test for new countdown value wraparound case with:
	/// 	 multiple subgame changes     
	/// 	 One subgame with wraparound case
	/// </summary>
	TEST_F(SessionPointsUnitTests, CalculateSessionPointsAndCountdown_Test03)
	{
		/*Game 1*/
		const DWORD gameSessionSequenceNumber1(40);
		const DWORD decrement1(5);
		const DWORD points(0);
		const DWORD countdown1(50);
		const DWORD multiplier1(1);
		const DWORD divider1(100);
		bool bonusPointsAwardedResult(false);

		// Test data -- expected results.
		const bool bonusPointsAwardedExpected(false);
		DWORD currentCountdownExpected(countdown1 - decrement1);
		DWORD currentSessionpointsExpected ((decrement1 + divider1 - countdown1) / divider1);
		
		// Setup.
		CPlayer player;
		PlayerDecrementCountdown(player, gameSessionSequenceNumber1, true, multiplier1, divider1, decrement1, true, points, countdown1, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected, player.GetSessionPoints(), currentSessionpointsExpected , __LINE__);

		/*Game 2*/
		const DWORD gameSessionSequenceNumber2(41);
		const DWORD decrement2(2);
		const DWORD multiplier2(1);
		const DWORD divider2(10);
		bonusPointsAwardedResult = false;

		// Test data -- expected results.
		currentCountdownExpected = divider2 - decrement1 % divider2 - decrement2;
        currentSessionpointsExpected = ((divider2 - decrement1 % divider2 + decrement2) / divider2);
		
		// Setup.
		PlayerDecrementCountdown(player, gameSessionSequenceNumber2, true, multiplier2, divider2, decrement2, false, 0, 0, bonusPointsAwardedResult);
		ASSERT_EQ (player.GetCurrentCountdown(), currentCountdownExpected);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected, player.GetSessionPoints(), currentSessionpointsExpected , __LINE__);

        //Now make it wrap around
        const DWORD decrement3(5);
 
        currentCountdownExpected = divider2 + (divider2 - decrement1 % divider2 - decrement2 - decrement3);
        currentSessionpointsExpected = ((divider2 - decrement1 % divider2 + decrement2 + decrement3) / divider2);

		CMeters *meters = new CMeters(MEMORY_NONE);
		CMeters *orig = new CMeters(MEMORY_NONE);
		orig->SetMeter(gCI, 1, true, false);
		meters->SetMeter(gCI, 1 + decrement3, true, false);
        bonusPointsAwardedResult = player.DecrementCountdown(meters, orig, true);
		delete meters;
		delete orig;
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected, player.GetSessionPoints(), currentSessionpointsExpected , __LINE__);
	
		// Cleanup.
	}

    /// <summary>
    /// Code coverage test for:
    /// 	CSessionPoints::CalculateSessionPointsAndCountdown()
    /// Test for new countdown value wraparound case with:
    /// 	 multiple subgame changes     
    ///      first subgame with delta less than countdown
    ///      second subgame with wraparound
    ///      third subgame: back to first subgame
    /// </summary>
    TEST_F(SessionPointsUnitTests, CalculateSessionPointsAndCountdown_Test04)
    {
        /*Game 1*/
        const DWORD gameSessionSequenceNumber1(40);
        const DWORD decrement1(5);
        const DWORD points(0);
        const DWORD countdown1(50);
        const DWORD multiplier1(1);
        const DWORD divider1(100);
        bool bonusPointsAwardedResult(false);

        // Test data -- expected results.
        const bool bonusPointsAwardedExpected(false);
        DWORD currentCountdownExpected(countdown1 - decrement1);
        DWORD currentSessionpointsExpected ((decrement1 + divider1 - countdown1) / divider1);

        // Setup.
        CPlayer player;
        PlayerDecrementCountdown(player, gameSessionSequenceNumber1, true, multiplier1, divider1, decrement1, true, points, countdown1, bonusPointsAwardedResult);
        TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected, player.GetSessionPoints(), currentSessionpointsExpected , __LINE__);

        /*Game 2*/
        const DWORD gameSessionSequenceNumber2(41);
        const DWORD decrement2(8);
        const DWORD multiplier2(1);
        const DWORD divider2(10);
        bonusPointsAwardedResult = false;

        // Test data -- expected results.
        currentCountdownExpected = divider2 + (divider2 - decrement1 % divider2 - decrement2);
        currentSessionpointsExpected = ((divider2 - decrement1 % divider2 + decrement2) / divider2);

        // Setup.
        PlayerDecrementCountdown(player, gameSessionSequenceNumber2, true, multiplier2, divider2, decrement2, false, 0, 0, bonusPointsAwardedResult);
        TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected, player.GetSessionPoints(), currentSessionpointsExpected , __LINE__);

		/*Game 3: Back to Game1 and play for 5*/
        const DWORD gameSessionSequenceNumber3(42);		
        const DWORD decrement3(5);

        currentCountdownExpected = 60;  //based on calculation
        currentSessionpointsExpected = ((divider2 - decrement1 % divider2 + decrement2 + decrement3) / divider2);

        PlayerDecrementCountdown(player, gameSessionSequenceNumber3, true, multiplier1, divider1, decrement3, false, 0, 0, bonusPointsAwardedResult);
        TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected, player.GetSessionPoints(), currentSessionpointsExpected , __LINE__);

        // Cleanup.
    }

	/// <summary>
	/// Code coverage test for:
	/// 	CSessionPoints::CalculateSessionPointsAndCountdown()
	/// Test for new countdown value wraparound case with:
	/// 	 multiple subgame changes before player changes	  
	/// 	 first subgame with delta less than countdown
	/// 	 second subgame with wraparound
	/// 	 New Player enters and session points and countdown comes as per new player
	/// </summary>
	TEST_F(SessionPointsUnitTests, CalculateSessionPointsAndCountdown_Test05)
	{
		/*Game 1*/
		const DWORD gameSessionSequenceNumber1(40);
		const DWORD decrement1(5);
		const DWORD points(0);
		const DWORD countdown1(50);
		const DWORD multiplier1(1);
		const DWORD divider1(100);
		bool bonusPointsAwardedResult(false);

		// Test data -- expected results.
		const bool bonusPointsAwardedExpected(false);
		DWORD currentCountdownExpected(countdown1 - decrement1);
		DWORD currentSessionpointsExpected ((decrement1 + divider1 - countdown1) / divider1);

		// Setup.
		CPlayer player;
		PlayerDecrementCountdown(player, gameSessionSequenceNumber1, true, multiplier1, divider1, decrement1, true, points, countdown1, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected, player.GetSessionPoints(), currentSessionpointsExpected , __LINE__);

		/*Game 2*/
		const DWORD gameSessionSequenceNumber2(41);
		const DWORD decrement2(8);
		const DWORD multiplier2(1);
		const DWORD divider2(10);
		bonusPointsAwardedResult = false;

		// Test data -- expected results.
		currentCountdownExpected = divider2 + (divider2 - decrement1 % divider2 - decrement2);
		currentSessionpointsExpected = ((divider2 - decrement1 % divider2 + decrement2) / divider2);

		// Setup.
		PlayerDecrementCountdown(player, gameSessionSequenceNumber2, true, multiplier2, divider2, decrement2, false, 0, 0, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected, player.GetSessionPoints(), currentSessionpointsExpected , __LINE__);

		/*New Player*/
		const DWORD gameSessionSequenceNumber3(40);
		const DWORD decrement3(5);
		const DWORD points3(0);
		const DWORD countdown3(55);
		const DWORD multiplier3(1);
		const DWORD divider3(200);
		CPlayer player2;

		currentCountdownExpected = (countdown3 - decrement3);
		currentSessionpointsExpected = ((decrement3 + divider3 - countdown3) / divider3);

		PlayerDecrementCountdown(player2, gameSessionSequenceNumber3, true, multiplier3, divider3, decrement3, true, points3, countdown3, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player2.GetCurrentCountdown(), currentCountdownExpected, player2.GetSessionPoints(), currentSessionpointsExpected , __LINE__);

		// Cleanup.
	}


	/// <summary>
	/// Code coverage test for:
	/// 	CSessionPoints::CalculateSessionPointsAndCountdown()
	/// Test for new countdown value when:
	///		 no multiplier and divider (default values will be used)
	/// 	 decrement less than current countdown
	/// </summary>
	TEST_F(SessionPointsUnitTests, CalculateSessionPointsAndCountdown_Test06)
	{
		const DWORD gameSessionSequenceNumber(40);
		const DWORD decrement(3);
		const DWORD points(0);
		const DWORD countdown(7);
		bool bonusPointsAwardedResult(false);
		
		// Test data -- expected results.
		const bool bonusPointsAwardedExpected(false);
		const DWORD currentCountdownExpected(countdown - decrement);
		const DWORD currentSessionpointsExpected((countdown - decrement)/DEFAULT_COUNTDOWN);
		
		CPlayer player;
		PlayerDecrementCountdown(player, gameSessionSequenceNumber, true, DEFAULT_MULTIPLIER, DEFAULT_COUNTDOWN, decrement, true, points, countdown, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected, player.GetSessionPoints(), currentSessionpointsExpected , __LINE__);

	}

	/// <summary>
	/// Code coverage test for:
	/// 	CSessionPoints::CalculateSessionPointsAndCountdown()
	/// Test for new countdown value when:
	///		 no multiplier and divider (default values will be used)
	///  	 decrement more than current countdown
	/// </summary>
	TEST_F(SessionPointsUnitTests, CalculateSessionPointsAndCountdown_Test07)
	{
		const DWORD gameSessionSequenceNumber(40);
		const DWORD decrement(13);
		const DWORD points(0);
		const DWORD countdown(7);
		bool bonusPointsAwardedResult(false);
		
		// Test data -- expected results.
		const bool bonusPointsAwardedExpected(false);
		const DWORD currentCountdownExpected((DEFAULT_COUNTDOWN + countdown - decrement % DEFAULT_COUNTDOWN) % DEFAULT_COUNTDOWN);
		const DWORD currentSessionpointsExpected ((decrement + DEFAULT_COUNTDOWN - countdown) / DEFAULT_COUNTDOWN);
		
		CPlayer player;
		PlayerDecrementCountdown(player, gameSessionSequenceNumber, true, DEFAULT_MULTIPLIER, DEFAULT_COUNTDOWN, decrement, true, points, countdown, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected, player.GetSessionPoints(), currentSessionpointsExpected , __LINE__);

	}


	TEST_F(SessionPointsUnitTests, CalculateSessionPointsAndCountdown_WeightedMutilplier01)
	{
		const DWORD gameSessionSequenceNumber1 = 1;
		const DWORD decrement1 = 78;
		const DWORD points = 0;
		const DWORD countdown = 100;
        const DWORD multiplier1 = 2;
        const DWORD divider1 = 100;
		bool bonusPointsAwardedResult = false;
		
		// Test data -- expected results.
		const bool bonusPointsAwardedExpected = false;
		const DWORD currentCountdownExpected1 = (countdown - decrement1);
		const DWORD currentSessionpointsExpected1 = decrement1 / countdown;
		
		CPlayer player;
		PlayerDecrementCountdown(player, gameSessionSequenceNumber1, true, multiplier1, divider1, decrement1, true, points, countdown, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected1, player.GetSessionPoints(), currentSessionpointsExpected1 , __LINE__);

		const DWORD gameSessionSequenceNumber2 = 2;
		const DWORD decrement2 = 10;
        const DWORD multiplier2 = 10;
        const DWORD divider2 = 30;

		const DWORD currentCountdownExpected2 = 27;
		const DWORD currentSessionpointsExpected2 = 3;

		PlayerDecrementCountdown(player, gameSessionSequenceNumber2, true, multiplier2, divider2, decrement2, false, points, countdown, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected2, player.GetSessionPoints(), currentSessionpointsExpected2 , __LINE__);
	}

	TEST_F(SessionPointsUnitTests, CalculateSessionPointsAndCountdown_WeightedMutilplier02)
	{
		const DWORD gameSessionSequenceNumber1 = 1;
		const DWORD decrement1 = 14;
		const DWORD points = 0;
		const DWORD countdown = 100;
        const DWORD multiplier1 = 6;
        const DWORD divider1 = 100;
		bool bonusPointsAwardedResult = false;
		
		CPlayer player;
		PlayerDecrementCountdown(player, gameSessionSequenceNumber1, true, multiplier1, divider1, decrement1, true, points, countdown, bonusPointsAwardedResult);

		const DWORD gameSessionSequenceNumber2 = 2;
		const DWORD decrement2 = 8;
        const DWORD multiplier2 = 3;
        const DWORD divider2 = 10;

		PlayerDecrementCountdown(player, gameSessionSequenceNumber2, true, multiplier2, divider2, decrement2, false, points, countdown, bonusPointsAwardedResult);

		const DWORD gameSessionSequenceNumber3 = 3;
		const DWORD decrement3 = 15;
        const DWORD multiplier3 = 30;
        const DWORD divider3 = 15;

		// Test data -- expected results.
		const bool bonusPointsAwardedExpected = false;
		const DWORD currentCountdownExpected3 = 1;
		const DWORD currentSessionpointsExpected3 = 5;

		PlayerDecrementCountdown(player, gameSessionSequenceNumber3, true, multiplier3, divider3, decrement3, false, points, countdown, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected3, player.GetSessionPoints(), currentSessionpointsExpected3, __LINE__);

        int decrement4 = 3;
		const DWORD currentCountdownExpected4 = 13;
		const DWORD currentSessionpointsExpected4 = 35;

		PlayerDecrementCountdown(player, gameSessionSequenceNumber3, false, 0, 0, decrement4, false, points, countdown, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), currentCountdownExpected4, player.GetSessionPoints(), currentSessionpointsExpected4, __LINE__);
	}

	TEST_F(SessionPointsUnitTests, CalculateSessionPointsAndCountdown_WeightedMutilplier03)
	{
		const bool bonusPointsAwardedExpected = false;
		const DWORD gameSessionSequenceNumber1 = 1;
		const DWORD decrement1 = 10;
		const DWORD points = 0;
		const DWORD countdown = 39;
        const DWORD multiplier1 = 3;
        const DWORD divider1 = 39;
		bool bonusPointsAwardedResult = false;
		const DWORD countdownExpected1 = 30;
		const DWORD currentSessionpointsExpected1 = 0;
		
		CPlayer player;
		PlayerDecrementCountdown(player, gameSessionSequenceNumber1, true, multiplier1, divider1, decrement1, true, points, countdown, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), countdownExpected1, player.GetSessionPoints(), currentSessionpointsExpected1, __LINE__);

		const DWORD gameSessionSequenceNumber2 = 2;
        const DWORD multiplier2 = 2;
        const DWORD divider2 = 35;
		const DWORD decrement2 = 50;
		const DWORD countdownExpected2 = 12;
		const DWORD currentSessionpointsExpected2 = 2;

		PlayerDecrementCountdown(player, gameSessionSequenceNumber2, true, multiplier2, divider2, decrement2, false, points, countdownExpected1, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), countdownExpected2, player.GetSessionPoints(), currentSessionpointsExpected2, __LINE__);

		const DWORD gameSessionSequenceNumber3 = 3;
        const DWORD multiplier3 = 4;
        const DWORD divider3 = 45;
		const DWORD decrement3 = 20;
		const DWORD countdownExpected3 = 40;
		const DWORD currentSessionpointsExpected3 = 4;
		
		PlayerDecrementCountdown(player, gameSessionSequenceNumber3, true, multiplier3, divider3, decrement3, false, points, countdownExpected2, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), countdownExpected3, player.GetSessionPoints(), currentSessionpointsExpected3, __LINE__);

		const DWORD gameSessionSequenceNumber4 = 4;
        const DWORD multiplier4 = 1;
        const DWORD divider4 = 50;
		const DWORD decrement4 = 50;
		const DWORD countdownExpected4 = 44;
		const DWORD currentSessionpointsExpected4 = 6;
		
		PlayerDecrementCountdown(player, gameSessionSequenceNumber4, true, multiplier4, divider4, decrement4, false, points, countdownExpected3, bonusPointsAwardedResult);
		TestPlayerResult(bonusPointsAwardedResult, bonusPointsAwardedExpected, player.GetCurrentCountdown(), countdownExpected4, player.GetSessionPoints(), currentSessionpointsExpected4, __LINE__);
	}


	/// <summary>
	/// GetCurrentSubgameSession should retrun null and GetSubgameSessionCount should also be zero 
	/// as no subgame session exists initially
	/// </summary>
    TEST_F(SessionPointsUnitTests, GetCurrentSubgameSession_NoSubgameSession)
	{
        CSessionPoints obj;
		
		ASSERT_EQ(obj.GetCurrentSubgameSession(), nullptr) << "No SubgameSession should exist initially";
		ASSERT_EQ(obj.GetSubgameSessionCount(), 0) << "SubgameSession count should be zero initially";
    }
	
	/// <summary>
	/// On successful CSessionPoints::CreateNewSubgameSession,  GetCurrentSubgameSession should not be null 
	/// and GetSubgameSessionCount should also be one 
	/// </summary>
    TEST_F(SessionPointsUnitTests, CreateSubgameSession_NoExistingSubgameSession)
	{
		CSessionPoints obj;
		DWORD expected_gameSessionSequenceNumber = 2; 
		
		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_gameSessionSequenceNumber  = expected_gameSessionSequenceNumber;

		ASSERT_TRUE(CreateNewSubgameSession(obj, subgameSessionParams));
		
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";
		ASSERT_EQ(obj.GetSubgameSessionCount(), 1) << "SubgameSession count should be one";

		//Test that last one added is the current active game session
		DWORD actual_gameSessionSequenceNumber = obj.GetCurrentSubgameSession()->GetSubgameSessionParameters().m_gameSessionSequenceNumber;
		
		ASSERT_EQ(actual_gameSessionSequenceNumber, expected_gameSessionSequenceNumber);
		ASSERT_EQ(obj.GetCurrentSubgameSession()->GetStatus(), SubgameSessionStatus::ActiveWaitingForParameters);
    }
	
	/// <summary>
	/// On multiple CSessionPoints::CreateNewSubgameSession function invocation, CurrentSubgameSession
	/// should point to last one created
	/// </summary>
    TEST_F(SessionPointsUnitTests, CreateNewSubgameSession_ChangeCurrentSubgameSession)
	{
		CSessionPoints obj;
		
		SubgameSessionParameters subgameSessionParams1;
		SubgameSessionParameters subgameSessionParams2;
		
		subgameSessionParams1.m_gameSessionSequenceNumber  = 2;
		subgameSessionParams2.m_gameSessionSequenceNumber  = 12;
		
		ASSERT_TRUE(CreateNewSubgameSession(obj, subgameSessionParams1));
		ASSERT_TRUE(CreateNewSubgameSession(obj, subgameSessionParams2));
		
		ASSERT_EQ(obj.GetSubgameSessionCount(), 2) << "SubgameSession count should be two";			
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";

		//Test that last one added is the current active game session
		DWORD actual_gameSessionSequenceNumber = obj.GetCurrentSubgameSession()->GetSubgameSessionParameters().m_gameSessionSequenceNumber;
		DWORD expected_gameSessionSequenceNumber = subgameSessionParams2.m_gameSessionSequenceNumber;

		ASSERT_EQ(actual_gameSessionSequenceNumber, expected_gameSessionSequenceNumber);
		ASSERT_EQ(obj.GetCurrentSubgameSession()->GetStatus(), SubgameSessionStatus::ActiveWaitingForParameters);

		
    }
	
	/// <summary>
	/// On successful CSessionPoints::SetGameSession,  GetCurrentSubgameSession should not be null 
	/// and GetSubgameSessionCount should also be one 
	/// </summary>
    TEST_F(SessionPointsUnitTests, SetGameSession_NoExistingSubgameSession)
	{
		CSessionPoints obj;
		DWORD expected_gameSessionSequenceNumber = 2; 
		
		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_gameSessionSequenceNumber  = expected_gameSessionSequenceNumber;
	
		
		ASSERT_TRUE(obj.SetGameSession(subgameSessionParams));
		
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";
		ASSERT_EQ(obj.GetSubgameSessionCount(), 1) << "SubgameSession count should be one";
	
		DWORD actual_gameSessionSequenceNumber = obj.GetCurrentSubgameSession()->GetSubgameSessionParameters().m_gameSessionSequenceNumber;
		
		//Test that last one added is the current active game session
		ASSERT_EQ(actual_gameSessionSequenceNumber, expected_gameSessionSequenceNumber);
		ASSERT_EQ(obj.GetCurrentSubgameSession()->GetStatus(), SubgameSessionStatus::ActiveWaitingForParameters);
	}		
	
	/// <summary>
	/// On multiple CSessionPoints::SetGameSession function invocation, CurrentSubgameSession
	/// should point to last one created and it's initial status should be 
	/// SubgameSessionStatus::ActiveWaitingForParameters
	/// </summary>
	TEST_F(SessionPointsUnitTests, SetGameSession_MultipleSetGameSessionTest)
	{
		CSessionPoints obj;
		
		SubgameSessionParameters subgameSessionParams1;
		SubgameSessionParameters subgameSessionParams2;
		
		subgameSessionParams1.m_gameSessionSequenceNumber  = 2;
		subgameSessionParams2.m_gameSessionSequenceNumber  = 12;
		
		ASSERT_TRUE(obj.SetGameSession(subgameSessionParams1));
		ASSERT_TRUE(obj.SetGameSession(subgameSessionParams2));
		
		ASSERT_EQ(obj.GetSubgameSessionCount(), 2) << "SubgameSession count should be two";			
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";

		DWORD actual_gameSessionSequenceNumber = obj.GetCurrentSubgameSession()->GetSubgameSessionParameters().m_gameSessionSequenceNumber;
		DWORD expected_gameSessionSequenceNumber = subgameSessionParams2.m_gameSessionSequenceNumber;
		
		//Test that last one added is the current active game session
		ASSERT_EQ(actual_gameSessionSequenceNumber, expected_gameSessionSequenceNumber);
		ASSERT_EQ(obj.GetCurrentSubgameSession()->GetStatus(), SubgameSessionStatus::ActiveWaitingForParameters);
    }
	
	/// <summary>
		/// Multiple CSessionPoints::SetGameSession function call for 
		/// same GameSessionSequenceNumber
	/// </summary>
	TEST_F(SessionPointsUnitTests, SetGameSession_SameGameSessionSequenceNumber)
	{
		CSessionPoints obj;
		
		SubgameSessionParameters subgameSessionParams1;
		SubgameSessionParameters subgameSessionParams2;
		
		subgameSessionParams1.m_gameSessionSequenceNumber  = 2;
		subgameSessionParams2.m_gameSessionSequenceNumber  = 2;
		
		ASSERT_TRUE(obj.SetGameSession(subgameSessionParams1));
		ASSERT_TRUE(obj.SetGameSession(subgameSessionParams2));
		
		ASSERT_EQ(obj.GetSubgameSessionCount(), 1) << "SubgameSession count should be one for same GameSessionSequenceNumber";			
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";
    }

	/// <summary>
	/// CSessionPoints::ApplySubgameSessionParams should fail if called on non-existing 
	/// GameSessionSequenceNumber
	/// </summary>
    TEST_F(SessionPointsUnitTests, ApplySubgameSessionParams_NonExistentGameSessionSequenceNumber)
	{
		CSessionPoints obj;
		
		SubgameSessionParameters subgameSessionParams1;
		SubgameSessionParameters subgameSessionParams2;
		
		subgameSessionParams1.m_gameSessionSequenceNumber  = 2;
		subgameSessionParams2.m_gameSessionSequenceNumber  = 12;
	
		ASSERT_TRUE(obj.SetGameSession(subgameSessionParams1));
		
        bool pointsAdded(false);
		ASSERT_FALSE(obj.ApplySubgameSessionParams(subgameSessionParams2, false, pointsAdded)) 
			<< "No SubgameSession object should exist for GameSessionSequenceNumber 12";
	}		

	/// <summary>
	/// CSessionPoints::ApplySubgameSessionParams should pass if called on existing GameSessionSequenceNumber.
	/// Also test that respective variables are modified on it's invocation.
	/// </summary>
	TEST_F(SessionPointsUnitTests, ApplySubgameSessionParams_ExistentGameSessionSequenceNumber)
	{
		CSessionPoints obj;
		
		SubgameSessionParameters subgameSessionParams1;
		subgameSessionParams1.m_gameSessionSequenceNumber  = 2;

		SubgameSessionParameters subgameSessionParams2;
		subgameSessionParams2.m_gameSessionSequenceNumber  = 2;
		subgameSessionParams2.m_multiplier = 22;  //try changing any other field just to verify that applying sesssion params is working

		ASSERT_TRUE(obj.SetGameSession(subgameSessionParams1));

		DWORD actual_multiplier = obj.GetCurrentSubgameSession()->GetSubgameSessionParameters().m_multiplier;
		DWORD expected_multiplier = subgameSessionParams1.m_multiplier;
		ASSERT_EQ(actual_multiplier, expected_multiplier);
		
        bool pointsAdded(false);
		ASSERT_TRUE(obj.ApplySubgameSessionParams(subgameSessionParams2, false, pointsAdded)) 
			<< "SubgameSession object should exist for GameSessionSequenceNumber 12";

		actual_multiplier = obj.GetCurrentSubgameSession()->GetSubgameSessionParameters().m_multiplier;
		expected_multiplier = subgameSessionParams2.m_multiplier;
		ASSERT_EQ(actual_multiplier, expected_multiplier);
	}	
	
	/// <summary>
	/// CSessionPoints::GameStart should retrun false when no subgame session exists
	/// </summary>
	TEST_F(SessionPointsUnitTests, GameStart_NoExistingSubgameSession)
	{
		CSessionPoints obj;
	
		ASSERT_FALSE(obj.GameStart());
	}

	/// <summary>
	/// CSessionPoints::GameStart should retrun return true on first game start and then false on subsequent calls
	/// </summary>
	TEST_F(SessionPointsUnitTests, GameStart_MultipleGameStart)
	{
		CSessionPoints obj;
		
		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_gameSessionSequenceNumber  = 5;
        subgameSessionParams.m_gameNumber = 10;

		ASSERT_TRUE(CreateNewSubgameSession(obj, subgameSessionParams));
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";

		ASSERT_TRUE(obj.GameStart()) << "First Gamestart should be true";
		ASSERT_FALSE(obj.GameStart()) << "Second onwards Gamestart should return false";
	}
	
	/// <summary>
	/// CSessionPoints::ApplyMeterChange should retrun false when no subgame session exists
	/// </summary>
	TEST_F(SessionPointsUnitTests, ApplyMeterChange_NoExistingSubgameSession)
	{
		CSessionPoints obj;
		DWORD meterDelta = 23;
		bool isBonusPointsAwarded = false;
	
		CMeters *meters = new CMeters(MEMORY_NONE);
		CMeters *orig = new CMeters(MEMORY_NONE);
		orig->SetMeter(gCI, 1, true, false);
		meters->SetMeter(gCI, 1 + meterDelta, true, false);
        bool pointsAdded(false);
		ASSERT_FALSE(obj.ApplyMeterChange(meters, orig, false, false, isBonusPointsAwarded, pointsAdded));
		delete meters;
		delete orig;
	}

	/// <summary>
    /// CSessionPoints::ApplyMeterChange should retrun return true 
    /// if  MGMD is disabled
	/// </summary>
	TEST_F(SessionPointsUnitTests, ApplyMeterChange_MGMDDiabled)
	{
		CSessionPoints obj;
		
		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_gameSessionSequenceNumber  = 5;
		subgameSessionParams.m_divider = 2;

		ASSERT_TRUE(CreateNewSubgameSession(obj, subgameSessionParams));
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";

		DWORD meterDelta = 23;
		bool isBonusPointsAwarded = false;
	
		CMeters *meters = new CMeters(MEMORY_NONE);
		CMeters *orig = new CMeters(MEMORY_NONE);
		orig->SetMeter(gCI, 1, true, false);
		meters->SetMeter(gCI, 1 + meterDelta, true, false);
        bool pointsAdded(false);
		ASSERT_TRUE(obj.ApplyMeterChange(meters, orig, false, false, isBonusPointsAwarded, pointsAdded));
		delete meters;
		delete orig;
	}

    /// <summary>
    /// CSessionPoints::ApplyMeterChange should retrun return true 
    /// if  MGMD is enabled
    /// </summary>
    TEST_F(SessionPointsUnitTests, ApplyMeterChange_MGMDEnabled)
    {
        CSessionPoints obj;

        SubgameSessionParameters subgameSessionParams;
        subgameSessionParams.m_gameSessionSequenceNumber  = 5;
        subgameSessionParams.m_divider = 2;

        ASSERT_TRUE(CreateNewSubgameSession(obj, subgameSessionParams));
        ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";

        DWORD meterDelta = 23;
        bool isBonusPointsAwarded = false;

		CMeters *meters = new CMeters(MEMORY_NONE);
		CMeters *orig = new CMeters(MEMORY_NONE);
		orig->SetMeter(gCI, 1, true, false);
		meters->SetMeter(gCI, 1 + meterDelta, true, false);
        bool pointsAdded(false);
        ASSERT_TRUE(obj.ApplyMeterChange(meters, orig, false, true, isBonusPointsAwarded, pointsAdded));
		delete meters;
		delete orig;
    }

	/// <summary>
	/// CSessionPoints::ApplyMeterChange with divider as zero
	/// </summary>
	TEST_F(SessionPointsUnitTests, ApplyMeterChange_MultipleGameStart_zerodivider)
	{
		CSessionPoints obj;
		
		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_gameSessionSequenceNumber  = 5;
		subgameSessionParams.m_divider = 0;  

		ASSERT_TRUE(CreateNewSubgameSession(obj, subgameSessionParams));
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";

		DWORD meterDelta = 23;
		bool isBonusPointsAwarded = false;
	
		CMeters *meters = new CMeters(MEMORY_NONE);
		CMeters *orig = new CMeters(MEMORY_NONE);
		orig->SetMeter(gCI, 1, true, false);
		meters->SetMeter(gCI, 1 + meterDelta, true, false);
        bool pointsAdded(false);
		ASSERT_TRUE(obj.ApplyMeterChange(meters, orig, false, false, isBonusPointsAwarded, pointsAdded));
		delete meters;
		delete orig;
	}

	/// <summary>
	/// CSessionPoints::ApplyMeterChange with original meter missing
	/// </summary>
	TEST_F(SessionPointsUnitTests, ApplyMeterChange_missingOrigMeter)
	{
		CSessionPoints obj;

		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_gameSessionSequenceNumber  = 5;
		subgameSessionParams.m_divider = 0;  

		ASSERT_TRUE(CreateNewSubgameSession(obj, subgameSessionParams));
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";

		DWORD meterDelta = 23;
		bool isBonusPointsAwarded = false;

		CMeters *meters = new CMeters(MEMORY_NONE);
		CMeters *orig = new CMeters(MEMORY_NONE);
		orig->SetMeter(gCO, 1, true, false);
		meters->SetMeter(gCI, 1 + meterDelta, true, false);
        bool pointsAdded(false);
		ASSERT_FALSE(obj.ApplyMeterChange(meters, orig, false, false, isBonusPointsAwarded, pointsAdded));
		delete meters;
		delete orig;
	}

	/// <summary>
	/// CSessionPoints::ApplyMeterChange with only unimportant 
	/// meters incrementing 
	/// </summary>
	TEST_F(SessionPointsUnitTests, ApplyMeterChange_IncrementOther)
	{
		CSessionPoints obj;

		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_gameSessionSequenceNumber  = 5;
		subgameSessionParams.m_divider = 0;  

		ASSERT_TRUE(CreateNewSubgameSession(obj, subgameSessionParams));
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";

		DWORD meterDelta = 23;
		bool isBonusPointsAwarded = false;

		CMeters *meters = new CMeters(MEMORY_NONE);
		CMeters *orig = new CMeters(MEMORY_NONE);
		orig->SetMeter(gCI, 1, true, false);
		orig->SetMeter(gCO, 1, true, false);
		meters->SetMeter(gCI, 1, true, false);
		meters->SetMeter(gCO, 1 + meterDelta, true, false);
        bool pointsAdded(false);
		ASSERT_FALSE(obj.ApplyMeterChange(meters, orig, false, false, isBonusPointsAwarded, pointsAdded));
		delete meters;
		delete orig;
	}

	/// <summary>
	/// CSessionPoints::ApplyMeterChange with only unimportant 
	/// meters incrementing 
	/// </summary>
	TEST_F(SessionPointsUnitTests, ApplyMeterChange_CoinOutSessionPoint)
	{
		CSessionPoints obj;

		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_accountingType = ACCOUNTING_COINS_OUT;
		subgameSessionParams.m_gameSessionSequenceNumber  = 5;
		subgameSessionParams.m_divider = 10;  

		ASSERT_TRUE(CreateNewSubgameSession(obj, subgameSessionParams));
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";

		DWORD meterDelta = 23;
		bool isBonusPointsAwarded = false;

		CMeters *meters = new CMeters(MEMORY_NONE);
		CMeters *orig = new CMeters(MEMORY_NONE);
		orig->SetMeter(gCI, 1, true, false);
		orig->SetMeter(gCO, 1, true, false);
		meters->SetMeter(gCI, 1, true, false);
		meters->SetMeter(gCO, 1 + meterDelta, true, false);
        bool pointsAdded(false);
		ASSERT_TRUE(obj.ApplyMeterChange(meters, orig, false, false, isBonusPointsAwarded, pointsAdded));
		delete meters;
		delete orig;
	}

	/// <summary>
	/// CSessionPoints::ApplyMeterChange
	/// This tests that session points does not overflow and go back in 0 or 1
	/// Previously, dividerAdjustedSubgameDelta overflowed because it was only a DWORD
	/// a meter delta value of more than 42949 cuz it to go back to zero or 1 
	/// because we multiply it by COUNTDOWN_PRECISION_MULTIPLIER (100000)
	/// </summary>
	TEST_F(SessionPointsUnitTests, ApplyMeterChange_DividerAdjustedSubgameDeltaMorethanDword)
	{
		CSessionPoints obj;
		obj.SetStartingMultiplierDividerCountdown(1, 1, 1, 1, 1);
		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_gameSessionSequenceNumber = 5;
		subgameSessionParams.m_divider = 2;
		subgameSessionParams.m_ebSessionParams[obj.GetBucketId()] = ExtendedBucketSessionParameters(DEFAULT_MULTIPLIER, 2);

		ASSERT_TRUE(CreateNewSubgameSession(obj, subgameSessionParams));
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";

		DWORD meterDelta = 43000;
		bool isBonusPointsAwarded = false;

		CMeters *meters = new CMeters(MEMORY_NONE);
		CMeters *orig = new CMeters(MEMORY_NONE);
		orig->SetMeter(gCI, 1, true, false);
		meters->SetMeter(gCI, 1 + meterDelta, true, false);
        bool pointsAdded(true);
		ASSERT_TRUE(obj.ApplyMeterChange(meters, orig, false, false, isBonusPointsAwarded, pointsAdded));
		ASSERT_TRUE(obj.GetSessionPoints() == 21500);
		delete meters;
		delete orig;
	}

	TEST_F(SessionPointsUnitTests, GetCurrentBucketDetailsTest)
	{
		// Setup.
		const std::vector<ExtendedBucketsCountdownData> countdownData = {
			{101, "Bucket 1", "$", 100, 10, 100},
			{102, "Bucket 2", "", 200, 20, 200},
		};

		CSessionPoints obj;
		obj.SetStartingMultiplierDividerCountdown(1, 1, 1, 1, 1);
		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_gameSessionSequenceNumber = 5;
		subgameSessionParams.m_divider = 2;
		subgameSessionParams.m_gameNumber = 5;
		subgameSessionParams.m_denom = 1;
		subgameSessionParams.m_ebSessionParams[obj.GetBucketId()] = ExtendedBucketSessionParameters(DEFAULT_MULTIPLIER, 2);
		for (int i = 0; i < countdownData.size(); i++)
		{
			obj.InitializeExtendedBucketCountdownTracking(&countdownData[i]);
			subgameSessionParams.m_ebSessionParams[countdownData[i].id] = ExtendedBucketSessionParameters(countdownData[i].multiplier, countdownData[i].divider);
		}

		ASSERT_TRUE(CreateNewSubgameSession(obj, subgameSessionParams));
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";

		// Perform test
		DWORD id = 0;
		std::string bucketName, bucketCurrency;
		DWORD countdown = 0;
		DWORD sessionPoints = 0;
		obj.GetCurrentBucketDetails(id, bucketName, bucketCurrency, countdown, sessionPoints);

		// verify the legacy points bucket details.
		ASSERT_EQ(id, 0) << "Legacy Points Bucket : ID incorrect";
		ASSERT_TRUE(bucketName.empty()) << "Legacy Points Bucket : bucket name incorrect";
		ASSERT_TRUE(bucketCurrency.empty()) << "Legacy Points Bucket : bucket currency incorrect";

		// test and verify the extended bucket details
		for (int i = 0; i < countdownData.size(); i++)
		{
			obj.MoveToNextBucket();
			obj.GetCurrentBucketDetails(id, bucketName, bucketCurrency, countdown, sessionPoints);

			ASSERT_EQ(id, countdownData[i].id) << "Extended Bucket : ID incorrect";
			ASSERT_EQ(bucketName, countdownData[i].displayName) << "Extended Bucket : bucket name incorrect";
			ASSERT_EQ(bucketCurrency, countdownData[i].bucketCurrency) << "Extended Bucket : bucket currency incorrect";
		}

		// perform test to reset the current bucket
		obj.MoveToNextBucket();
		obj.GetCurrentBucketDetails(id, bucketName, bucketCurrency, countdown, sessionPoints);

		// verify the legacy points bucket details again.
		ASSERT_EQ(id, 0) << "Legacy Points Bucket : ID incorrect";
		ASSERT_TRUE(bucketName.empty()) << "Legacy Points Bucket : bucket name incorrect";
		ASSERT_TRUE(bucketCurrency.empty()) << "Legacy Points Bucket : bucket currency incorrect";
	}

	TEST_F(SessionPointsUnitTests, GetBucketsDisplayDataTest)
	{
		// Setup.
		const std::vector<ExtendedBucketsCountdownData> countdownData = {
			{101, "Bucket 1", "$", 100, 10, 100},
			{102, "Bucket 2", "", 200, 20, 200},
		};

		CSessionPoints obj;
		obj.SetStartingMultiplierDividerCountdown(1, 1, 1, 1, 1);
		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_gameSessionSequenceNumber = 5;
		subgameSessionParams.m_divider = 2;
		subgameSessionParams.m_gameNumber = 5;
		subgameSessionParams.m_denom = 1;
		subgameSessionParams.m_ebSessionParams[obj.GetBucketId()] = ExtendedBucketSessionParameters(DEFAULT_MULTIPLIER, 2);
		for (int i = 0; i < countdownData.size(); i++)
		{
			obj.InitializeExtendedBucketCountdownTracking(&countdownData[i]);
			subgameSessionParams.m_ebSessionParams[countdownData[i].id] = ExtendedBucketSessionParameters(countdownData[i].multiplier, countdownData[i].divider);
		}

		ASSERT_TRUE(CreateNewSubgameSession(obj, subgameSessionParams));
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";

		// Perform test
		std::vector<std::unique_ptr<BucketDisplayData>> displayData;
		DWORD gameNumber = 0;
		int denom = 0;
		obj.GetBucketsDisplayData(displayData, gameNumber, denom);

		// verify
		ASSERT_EQ(displayData.size(), countdownData.size());
		for (int i = 0; i < displayData.size(); i++)
		{
			ASSERT_EQ(displayData[i]->id, countdownData[i].id);
			ASSERT_EQ(displayData[i]->displayName, countdownData[i].displayName);
			ASSERT_EQ(displayData[i]->bucketName, countdownData[i].displayName);
			ASSERT_EQ(displayData[i]->currency, countdownData[i].bucketCurrency);
			ASSERT_EQ(displayData[i]->countdown, countdownData[i].countdown);
		}
		ASSERT_EQ(gameNumber, 5);
		ASSERT_EQ(denom, 1);
	}

	TEST_F(SessionPointsUnitTests, GetEBSessionPointsAndCountdownTest)
	{
		// Setup.
		const std::vector<ExtendedBucketsCountdownData> countdownData = {
			{101, "Bucket 1", "$", 100, 10, 100},
			{102, "Bucket 2", "", 200, 20, 200},
		};

		CSessionPoints obj;
		obj.SetStartingMultiplierDividerCountdown(1, 1, 1, 1, 1);
		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_gameSessionSequenceNumber = 5;
		subgameSessionParams.m_divider = 2;
		subgameSessionParams.m_gameNumber = 5;
		subgameSessionParams.m_denom = 1;
		subgameSessionParams.m_ebSessionParams[obj.GetBucketId()] = ExtendedBucketSessionParameters(DEFAULT_MULTIPLIER, 2);
		for (int i = 0; i < countdownData.size(); i++)
		{
			obj.InitializeExtendedBucketCountdownTracking(&countdownData[i]);
			subgameSessionParams.m_ebSessionParams[countdownData[i].id] = ExtendedBucketSessionParameters(countdownData[i].multiplier, countdownData[i].divider);
		}

		ASSERT_TRUE(CreateNewSubgameSession(obj, subgameSessionParams));
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";

		DWORD meterDelta = 120;
		bool isBonusPointsAwarded = false;

		CMeters *meters = new CMeters(MEMORY_NONE);
		CMeters *orig = new CMeters(MEMORY_NONE);
		orig->SetMeter(gCI, 1, true, false);
		meters->SetMeter(gCI, 1 + meterDelta, true, false);
		bool pointsAdded(true);
		ASSERT_TRUE(obj.ApplyMeterChange(meters, orig, false, false, isBonusPointsAwarded, pointsAdded));

		// perform test and verify
		ASSERT_TRUE(obj.GetEBSessionPoints(101) == 10);
		ASSERT_TRUE(obj.GetEBSessionCountdown(101) == 80);

		ASSERT_TRUE(obj.GetEBSessionPoints(102) == 0);
		ASSERT_TRUE(obj.GetEBSessionCountdown(102) == 80);

		delete meters;
		delete orig;
	}

	TEST_F(SessionPointsUnitTests, GetBucketCountTest)
	{
		// Setup.
		const std::vector<ExtendedBucketsCountdownData> countdownData = {
			{101, "Bucket 1", "$", 100, 10, 100},
			{102, "Bucket 2", "", 200, 20, 200},
		};
		const WORD bucketCount = countdownData.size() + 1;  // One legacy bucket by default

		CSessionPoints obj;
		obj.SetStartingMultiplierDividerCountdown(1, 1, 1, 1, 1);
		SubgameSessionParameters subgameSessionParams;
		subgameSessionParams.m_gameSessionSequenceNumber = 5;
		subgameSessionParams.m_divider = 2;
		subgameSessionParams.m_gameNumber = 5;
		subgameSessionParams.m_denom = 1;
		subgameSessionParams.m_ebSessionParams[obj.GetBucketId()] = ExtendedBucketSessionParameters(DEFAULT_MULTIPLIER, 2);
		for (int i = 0; i < countdownData.size(); i++)
		{
			obj.InitializeExtendedBucketCountdownTracking(&countdownData[i]);
			subgameSessionParams.m_ebSessionParams[countdownData[i].id] = ExtendedBucketSessionParameters(countdownData[i].multiplier, countdownData[i].divider);
		}

		ASSERT_TRUE(CreateNewSubgameSession(obj, subgameSessionParams));
		ASSERT_NE(obj.GetCurrentSubgameSession(), nullptr) << "Current SubgameSession should not be NULL";

		// perform test and verify
		ASSERT_EQ(obj.GetBucketCount(), bucketCount) << "Incorrect bucket count.";
	}
}
