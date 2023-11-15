#include "stdafx.h"
#include "SASProtocol/SASGameList.h"

// Sentinel includes.
#include "SASProtocol/SASSerialWrapper.h"

// Unit testing includes.
#include "gtest/gtest.h"

namespace SentinelUnitTests
{
    static const int DENOM_UNKNOWN = 0;
    class SASGameListTests : public ::testing::Test
	{    
		public:   
			virtual void SetUp()  
			{
            } 
			virtual void TearDown()
			{
			}
            void GetCurrentGame_NoChange();
            void GetCurrentGame_Change();
            void GetCurrentGame_Set_1Game();
            void GetCurrentGame_Set_2Games();
            void ShouldUpdateCurrentGame_2Games();
            void ShouldUpdateCurrentGame_1Game();
    };
		/// <summary>
		/// Functional test for:
		///     CSASGameList
		/// Test static const members.
		/// </summary>
		TEST_F(SASGameListTests, GetCurrentGame_StaticConst)
		{
			ASSERT_EQ((WORD)0, (WORD)EGM_GAME_NUMBER);
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::CSASGameList()
		///     CSASGameList::GetCurrentGame()
		/// Test constructor for default value(s).
		/// </summary>
		TEST_F(SASGameListTests, GetCurrentGame_Default)
		{
			// Perform operation(s) and test(s).
			CSASGameList obj;
			ASSERT_EQ((WORD)EGM_GAME_NUMBER, obj.GetCurrentGame());
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::SetCurrentGame()
		///     CSASGameList::GetCurrentGame()
		/// Test setting non-default value(s).
		/// </summary>
		TEST_F(SASGameListTests, GetCurrentGame_Set)
		{
			// Data.
			const WORD gameNumber(1234);

			// Setup.
			CSASGameList obj;

			// Perform operation(s) and test(s).
			obj.SetCurrentGame(true, true, gameNumber);
			ASSERT_EQ(gameNumber, obj.GetCurrentGame());
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::Clear()
		///     CSASGameList::GetCurrentGame()
		/// Test for Clear() changing non-default value(s) to default value(s).
		/// </summary>
		TEST_F(SASGameListTests, GetCurrentGame_Clear)
		{
			// Data.
			const WORD gameNumber(1234);

			// Setup.
			CSASGameList obj;
			obj.SetCurrentGame(true, true, gameNumber);

			// Perform operation(s) and test(s).
			obj.Clear();
			ASSERT_EQ((WORD)EGM_GAME_NUMBER, obj.GetCurrentGame());
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::SetCurrentGame()
		/// Test for m_bCurrentGameUsedLast changing from true to false when current game number is changed.
		/// </summary>
		TEST_F(SASGameListTests, GetCurrentGame_Change)
		{
            GetCurrentGame_Change();
        }
        void SASGameListTests::GetCurrentGame_Change()
        {
			// Data.
			const WORD gameNumber(1234);

			// Setup.
			CSASGameList obj;
			obj.SetGameEnabled(gameNumber);
			obj.SetCurrentGame(true, true, gameNumber);
			bool needToSendInfo;
			obj.GetNextGame(needToSendInfo);

			ASSERT_EQ(gameNumber, obj.GetCurrentGame()); // Verify setup.
			ASSERT_EQ(false, obj.m_bCurrentGameUsedLast);

			// Perform operation(s) and test(s).
			obj.SetCurrentGame(true, true, gameNumber + 1);
			ASSERT_FALSE(obj.m_bCurrentGameUsedLast);
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::SetCurrentGame()
		/// Test for m_bCurrentGameUsedLast staying true when current game number is not changed.
		/// </summary>
		TEST_F(SASGameListTests, GetCurrentGame_NoChange)
		{
            GetCurrentGame_NoChange();
        }
        void SASGameListTests::GetCurrentGame_NoChange()
        {
			// Data.
			const WORD gameNumber(1234);

			// Setup.
			CSASGameList obj;
			obj.SetGameEnabled(gameNumber);
			obj.SetCurrentGame(true, true, gameNumber);
			bool needToSendInfo;
			obj.GetNextGame(needToSendInfo);

			ASSERT_EQ(gameNumber, obj.GetCurrentGame()); // Verify setup.
			ASSERT_EQ(false, obj.m_bCurrentGameUsedLast);

			// Perform operation(s) and test(s).
			obj.SetCurrentGame(true, true, gameNumber);
			ASSERT_EQ(false, obj.m_bCurrentGameUsedLast);
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::SetCurrentGame()
		///     CSASGameList::GetCurrentGame()
		/// Test SetCurrentGame() when there is 1 enabled game.
		/// </summary>
		TEST_F(SASGameListTests, GetCurrentGame_Set_1Game)
		{
            GetCurrentGame_Set_1Game();
        }
        void SASGameListTests::GetCurrentGame_Set_1Game()
        {
			// Data.
			const WORD gameNumber1(1234);
			const WORD gameNumber2(gameNumber1 + 1);

			// Setup.
			CSASGameList obj;
			obj.SetGameEnabled(gameNumber1);
			ASSERT_EQ((int)1, obj.m_enabledGames.size()); // Verify setup.

			// Perform operation(s) and test(s).
			// Try to set EGM_GAME_NUMBER.
			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(false, false, EGM_GAME_NUMBER); // Perform operation.
			ASSERT_EQ(gameNumber1, obj.GetCurrentGame()); // Test.

			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(false, true, EGM_GAME_NUMBER); // Perform operation.
			ASSERT_EQ(gameNumber1, obj.GetCurrentGame()); // Test.

			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(true, false, EGM_GAME_NUMBER); // Perform operation.
			ASSERT_EQ(gameNumber1, obj.GetCurrentGame()); // Test.

			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(true, true, EGM_GAME_NUMBER); // Perform operation.
			ASSERT_EQ(gameNumber1, obj.GetCurrentGame()); // Test.

			// Try to set a non-EGM_GAME_NUMBER.
			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(false, false, gameNumber2); // Perform operation.
			ASSERT_EQ(gameNumber1, obj.GetCurrentGame()); // Test.

			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(false, true, gameNumber2); // Perform operation.
			ASSERT_EQ(gameNumber1, obj.GetCurrentGame()); // Test.

			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(true, false, gameNumber2); // Perform operation.
			ASSERT_EQ(gameNumber2, obj.GetCurrentGame()); // Test.

			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(true, true, gameNumber2); // Perform operation.
			ASSERT_EQ(gameNumber2, obj.GetCurrentGame()); // Test.
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::SetCurrentGame()
		///     CSASGameList::GetCurrentGame()
		/// Test SetCurrentGame() when there are 2 enabled games.
		/// </summary>
		TEST_F(SASGameListTests, GetCurrentGame_Set_2Games)
		{
            GetCurrentGame_Set_2Games();
        }
        void SASGameListTests::GetCurrentGame_Set_2Games()
        {
			// Data.
			const WORD gameNumber1(1234);
			const WORD gameNumber2(gameNumber1 + 1);

			// Setup.
			CSASGameList obj;
			obj.SetGameEnabled(gameNumber1);
			obj.SetGameEnabled(gameNumber2);
			ASSERT_EQ((int)2, obj.m_enabledGames.size()); // Verify setup.

			// Perform operation(s) and test(s).
			// Try to set EGM_GAME_NUMBER.
			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(false, false, EGM_GAME_NUMBER); // Perform operation.
			ASSERT_EQ(gameNumber1, obj.GetCurrentGame()); // Test.

			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(false, true, EGM_GAME_NUMBER); // Perform operation.
			ASSERT_EQ((WORD)EGM_GAME_NUMBER, obj.GetCurrentGame()); // Test.

			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(true, false, EGM_GAME_NUMBER); // Perform operation.
			ASSERT_EQ((WORD)EGM_GAME_NUMBER, obj.GetCurrentGame()); // Test.

			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(true, true, EGM_GAME_NUMBER); // Perform operation.
			ASSERT_EQ((WORD)EGM_GAME_NUMBER, obj.GetCurrentGame()); // Test.

			// Try to set a non-EGM_GAME_NUMBER.
			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(false, false, gameNumber2); // Perform operation.
			ASSERT_EQ(gameNumber1, obj.GetCurrentGame()); // Test.

			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(false, true, gameNumber2); // Perform operation.
			ASSERT_EQ((WORD)EGM_GAME_NUMBER, obj.GetCurrentGame()); // Test.

			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(true, false, gameNumber2); // Perform operation.
			ASSERT_EQ(gameNumber2, obj.GetCurrentGame()); // Test.

			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			obj.SetCurrentGame(true, true, gameNumber2); // Perform operation.
			ASSERT_EQ(gameNumber2, obj.GetCurrentGame()); // Test.
		}
// #pragma endregion GetCurrentGame

// #pragma region GetEnabledDenomCount
		/// <summary>
		/// Functional test for:
		///     CSASGameList::CSASGameList()
		///     CSASGameList::GetEnabledDenomCount()
		/// Test constructor for default value(s).
		/// </summary>
		TEST_F(SASGameListTests, GetEnabledDenomCount_Default)
		{
			// Perform operation(s) and test(s).
			CSASGameList obj;
			ASSERT_EQ((byte)0, obj.GetEnabledDenomCount());
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::SetEnabledDenomCount()
		///     CSASGameList::GetEnabledDenomCount()
		/// Test setting non-default value(s).
		/// </summary>
		TEST_F(SASGameListTests, GetEnabledDenomCount_Set)
		{
			// Data.
			const byte enabledDenomCount(5);

			// Setup.
			CSASGameList obj;

			// Perform operation(s) and test(s).
			obj.SetEnabledDenomCount(enabledDenomCount);
			ASSERT_EQ(enabledDenomCount, obj.GetEnabledDenomCount());
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::Clear()
		///     CSASGameList::GetEnabledDenomCount()
		/// Test for Clear() changing non-default value(s) to default value(s).
		/// </summary>
		TEST_F(SASGameListTests, GetEnabledDenomCount_Clear)
		{
			// Data.
			const byte enabledDenomCount(5);

			// Setup.
			CSASGameList obj;
			obj.SetEnabledDenomCount(enabledDenomCount);

			// Perform operation(s) and test(s).
			obj.Clear();
			ASSERT_EQ((byte)0, obj.GetEnabledDenomCount());
		}
// #pragma endregion GetEnabledDenomCount

// #pragma region GetDenomFromDenomCode
		/// <summary>
		/// Functional test for:
		///     CSASGameList::GetDenomFromDenomCode()
		/// Test constructor for default value(s).
		/// </summary>
		TEST_F(SASGameListTests, GetDenomFromDenomCode)
		{
			// Data.
			byte denomCode0(0); // Minimum valid code.
			int denominationExpected0(CSASGameList::DENOM_UNKNOWN);

			byte denomCode4(4);
			int denominationExpected4(25);

			byte denomCodeMax(_countof(denomCodeMap) - 1); // Maximum valid code.
			int denominationExpectedMax(40);

			byte denomCodeMaxPlus1(_countof(denomCodeMap)); // Maximum valid code, plus 1
			int denominationExpectedMaxPlus1(CSASGameList::DENOM_UNKNOWN);

			byte denomCodeFF(UCHAR_MAX); // Maximum invalid code value.
			int denominationExpectedFF(CSASGameList::DENOM_UNKNOWN);

			// Perform operation(s) and test(s).
			ASSERT_EQ(denominationExpected0, CSASGameList::GetDenomFromDenomCode(denomCode0));
			ASSERT_EQ(denominationExpected4, CSASGameList::GetDenomFromDenomCode(denomCode4));
			ASSERT_EQ(denominationExpectedMax, CSASGameList::GetDenomFromDenomCode(denomCodeMax));
			ASSERT_EQ(denominationExpectedMaxPlus1, CSASGameList::GetDenomFromDenomCode(denomCodeMaxPlus1));
			ASSERT_EQ(denominationExpectedFF, CSASGameList::GetDenomFromDenomCode(denomCodeFF));
		}
// #pragma endregion GetDenomFromDenomCode


// #pragma region SetCurrentDenomCode
		/// <summary>
		/// Functional test for:
		///     CSASGameList::SetCurrentDenomCode()
		///     CSASGameList::GetCurrentDenom()
		/// Test setting non-default value(s).
		/// </summary>
		TEST_F(SASGameListTests, SetCurrentDenomCode)
		{
			// Data.
			byte denomCode4(4);
			int denominationExpected4(25);

			// Setup.
			CSASGameList obj;

			// Perform operation(s) and test(s).
			obj.SetCurrentDenomCode(true, true, denomCode4);
			ASSERT_EQ(denominationExpected4, obj.GetCurrentDenom());
		}
// #pragma endregion GetEnabledDenomCount

// #pragma region GetCurrentDenom
		/// <summary>
		/// Functional test for:
		///     CSASGameList
		/// Test static const members.
		/// </summary>
		TEST_F(SASGameListTests, GetCurrentDenom_StaticConst)
		{
			ASSERT_EQ((int)0, DENOM_UNKNOWN);
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::CSASGameList()
		///     CSASGameList::GetCurrentDenom()
		/// Test constructor for default value(s).
		/// </summary>
		TEST_F(SASGameListTests, GetCurrentDenom_Default)
		{
			// Perform operation(s) and test(s).
			CSASGameList obj;
			ASSERT_EQ(DENOM_UNKNOWN, obj.GetCurrentDenom());
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::SetCurrentDenom()
		///     CSASGameList::GetCurrentDenom()
		/// Test setting non-default value(s).
		/// </summary>
		TEST_F(SASGameListTests, GetCurrentDenom_Set)
		{
			// Data.
			const int currentDenom(1234);

			// Setup.
			CSASGameList obj;

			// Perform operation(s) and test(s).
			obj.SetCurrentDenom(true, true, currentDenom);
			ASSERT_EQ(currentDenom, obj.GetCurrentDenom());
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::Clear()
		///     CSASGameList::GetCurrentDenom()
		/// Test for Clear() changing non-default value(s) to default value(s).
		/// </summary>
		TEST_F(SASGameListTests, GetCurrentDenom_Clear)
		{
			// Data.
			const int currentDenom(1234);

			// Setup.
			CSASGameList obj;
			obj.SetCurrentDenom(true, true, currentDenom);

			// Perform operation(s) and test(s).
			obj.Clear();
			ASSERT_EQ(DENOM_UNKNOWN, obj.GetCurrentDenom());
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::SetCurrentDenom()
		///     CSASGameList::GetCurrentDenom()
		/// Test SetCurrentGame() when there is 1 enabled denom.
		/// </summary>
		TEST_F(SASGameListTests, GetCurrentDenom_Set_1Denom)
		{
			// Data.
			const int currentDenom1(1234);
			const int currentDenom2(currentDenom1 + 1);

			// Setup.
			CSASGameList obj;
			obj.SetEnabledDenomCount(1);

			// Perform operation(s) and test(s).
			// Try to set CSASGameList::DENOM_UNKNOWN.
			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(false, false, CSASGameList::DENOM_UNKNOWN); // Perform operation.
			ASSERT_EQ(currentDenom1, obj.GetCurrentDenom()); // Test.

			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(false, true, CSASGameList::DENOM_UNKNOWN); // Perform operation.
			ASSERT_EQ(currentDenom1, obj.GetCurrentDenom()); // Test.

			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(true, false, CSASGameList::DENOM_UNKNOWN); // Perform operation.
			ASSERT_EQ(currentDenom1, obj.GetCurrentDenom()); // Test.

			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(true, true, CSASGameList::DENOM_UNKNOWN); // Perform operation.
			ASSERT_EQ(currentDenom1, obj.GetCurrentDenom()); // Test.

			// Try to set a non-EGM_GAME_NUMBER.
			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(false, false, currentDenom2); // Perform operation.
			ASSERT_EQ(currentDenom1, obj.GetCurrentDenom()); // Test.

			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(false, true, currentDenom2); // Perform operation.
			ASSERT_EQ(currentDenom1, obj.GetCurrentDenom()); // Test.

			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(true, false, currentDenom2); // Perform operation.
			ASSERT_EQ(currentDenom2, obj.GetCurrentDenom()); // Test.

			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(true, true, currentDenom2); // Perform operation.
			ASSERT_EQ(currentDenom2, obj.GetCurrentDenom()); // Test.
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::SetCurrentDenom()
		///     CSASGameList::GetCurrentDenom()
		/// Test SetCurrentGame() when there are 2 enabled denominations.
		/// </summary>
		TEST_F(SASGameListTests, GetCurrentDenom_Set_2Denoms)
		{
			// Data.
			const int currentDenom1(1234);
			const int currentDenom2(currentDenom1 + 1);

			// Setup.
			CSASGameList obj;
			obj.SetEnabledDenomCount(2);

			// Perform operation(s) and test(s).
			// Try to set EGM_GAME_NUMBER.
			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(false, false, CSASGameList::DENOM_UNKNOWN); // Perform operation.
			ASSERT_EQ(currentDenom1, obj.GetCurrentDenom()); // Test.

			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(false, true, CSASGameList::DENOM_UNKNOWN); // Perform operation.
			ASSERT_EQ(DENOM_UNKNOWN, obj.GetCurrentDenom()); // Test.

			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(true, false, CSASGameList::DENOM_UNKNOWN); // Perform operation.
			ASSERT_EQ(DENOM_UNKNOWN, obj.GetCurrentDenom()); // Test.

			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(true, true, CSASGameList::DENOM_UNKNOWN); // Perform operation.
			ASSERT_EQ(DENOM_UNKNOWN, obj.GetCurrentDenom()); // Test.

			// Try to set a non-EGM_GAME_NUMBER.
			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(false, false, currentDenom2); // Perform operation.
			ASSERT_EQ(currentDenom1, obj.GetCurrentDenom()); // Test.

			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(false, true, currentDenom2); // Perform operation.
			ASSERT_EQ(DENOM_UNKNOWN, obj.GetCurrentDenom()); // Test.

			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(true, false, currentDenom2); // Perform operation.
			ASSERT_EQ(currentDenom2, obj.GetCurrentDenom()); // Test.

			obj.SetCurrentDenom(true, true, currentDenom1); // Setup.
			obj.SetCurrentDenom(true, true, currentDenom2); // Perform operation.
			ASSERT_EQ(currentDenom2, obj.GetCurrentDenom()); // Test.
		}
// #pragma endregion GetCurrentDenom

// #pragma region ShouldUpdateCurrentGame
		/// <summary>
		/// Functional test for:
		///     CSASGameList::ShouldUpdateCurrentGame()
		/// Test ShouldUpdateCurrentGame() when there is 1 enabled game.
		/// </summary>
		TEST_F(SASGameListTests, ShouldUpdateCurrentGame_1Game)
		{
            ShouldUpdateCurrentGame_1Game();
        }
        void SASGameListTests::ShouldUpdateCurrentGame_1Game()
        {
			// Data.
			const short fastPollingSpeed(CSASSerialWrapper::MS40_POLLING_SPEED_MILLISECONDS);
			const WORD gameNumber1(1234);

			// Setup.
			CSASGameList obj;
			obj.SetGameEnabled(gameNumber1);
			ASSERT_EQ((int)1, obj.m_enabledGames.size()); // Verify setup.
			ASSERT_EQ((WORD)EGM_GAME_NUMBER, obj.GetCurrentGame());

			// Perform operation(s) and test(s).
			ASSERT_FALSE(obj.ShouldUpdateCurrentGame(false, fastPollingSpeed)); // Test.
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::ShouldUpdateCurrentGame()
		/// Test ShouldUpdateCurrentGame() when there are 2 enabled games.
		/// </summary>
		TEST_F(SASGameListTests, ShouldUpdateCurrentGame_2Games)
		{
            ShouldUpdateCurrentGame_2Games();
        }
        void SASGameListTests::ShouldUpdateCurrentGame_2Games()
        {
			// Data.
			const short fastPollingSpeed(CSASSerialWrapper::MS40_POLLING_SPEED_MILLISECONDS);
			const short slowPollingSpeed(CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS);
			const WORD gameNumber1(1234);
			const WORD gameNumber2(gameNumber1 + 1);

			// Setup.
			CSASGameList obj;
			obj.SetGameEnabled(gameNumber1);
			obj.SetGameEnabled(gameNumber2);
			ASSERT_EQ((int)2, obj.m_enabledGames.size()); // Verify setup.

			// Perform operation(s) and test(s).
			// Try with gameStarted, slowPollingSpeed, m_currentGame != EGM_GAME_NUMBER.
			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			ASSERT_FALSE(obj.ShouldUpdateCurrentGame(true, slowPollingSpeed)); // Test.

			// Try with !gameStarted, slowPollingSpeed, m_currentGame != EGM_GAME_NUMBER.
			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			ASSERT_EQ(true, obj.ShouldUpdateCurrentGame(false, slowPollingSpeed)); // Test.

			// Try with gameStarted, fastPollingSpeed, m_currentGame != EGM_GAME_NUMBER.
			obj.SetCurrentGame(true, true, gameNumber1); // Setup.
			ASSERT_EQ(true, obj.ShouldUpdateCurrentGame(true, fastPollingSpeed)); // Test.

			// Try with gameStarted, slowPollingSpeed, m_currentGame == EGM_GAME_NUMBER.
			obj.SetCurrentGame(true, true, EGM_GAME_NUMBER); // Setup.
			ASSERT_EQ(true, obj.ShouldUpdateCurrentGame(true, slowPollingSpeed)); // Test.
		}
// #pragma endregion ShouldUpdateCurrentGame

// #pragma region ShouldUpdateCurrentDenom
		/// <summary>
		/// Functional test for:
		///     CSASGameList::ShouldUpdateCurrentDenom()
		/// Test ShouldUpdateCurrentDenom() when there is 1 enabled denomination.
		/// </summary>
		TEST_F(SASGameListTests, ShouldUpdateCurrentDenom_1Denom)
		{
			// Data.
			const short fastPollingSpeed(CSASSerialWrapper::MS40_POLLING_SPEED_MILLISECONDS);
			const WORD gameNumber1(1234);

			// Setup.
			CSASGameList obj;
			obj.SetEnabledDenomCount(1);
			ASSERT_EQ((WORD)EGM_GAME_NUMBER, obj.GetCurrentGame());

			// Perform operation(s) and test(s).
			ASSERT_FALSE(obj.ShouldUpdateCurrentDenom(false, fastPollingSpeed)); // Test.
		}

		/// <summary>
		/// Functional test for:
		///     CSASGameList::ShouldUpdateCurrentDenom()
		/// Test ShouldUpdateCurrentDenom() when there are 2 enabled denominations.
		/// </summary>
		TEST_F(SASGameListTests, ShouldUpdateCurrentDenom_2Denoms)
		{
			// Data.
			const short fastPollingSpeed(CSASSerialWrapper::MS40_POLLING_SPEED_MILLISECONDS);
			const short slowPollingSpeed(CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS);
			const WORD denom1(25);

			// Setup.
			CSASGameList obj;
			obj.SetEnabledDenomCount(2);

			// Perform operation(s) and test(s).
			// Try with gameStarted, slowPollingSpeed, m_currentDenom != DENOM_UNKNOWN.
			obj.SetCurrentDenom(true, true, denom1); // Setup.
			ASSERT_FALSE(obj.ShouldUpdateCurrentDenom(true, slowPollingSpeed)); // Test.

			// Try with !gameStarted, slowPollingSpeed, m_currentDenom != DENOM_UNKNOWN.
			obj.SetCurrentDenom(true, true, denom1); // Setup.
			ASSERT_EQ(true, obj.ShouldUpdateCurrentDenom(false, slowPollingSpeed)); // Test.

			// Try with gameStarted, fastPollingSpeed, m_currentDenom != DENOM_UNKNOWN.
			obj.SetCurrentDenom(true, true, denom1); // Setup.
			ASSERT_EQ(true, obj.ShouldUpdateCurrentDenom(true, fastPollingSpeed)); // Test.

			// Try with gameStarted, slowPollingSpeed, m_currentDenom == DENOM_UNKNOWN.
			obj.SetCurrentDenom(true, true, CSASGameList::DENOM_UNKNOWN); // Setup.
			ASSERT_EQ(true, obj.ShouldUpdateCurrentDenom(true, slowPollingSpeed)); // Test.
		}

		
		/// <summary>
		/// Functional test for:
		///     CSASGameList::IsEnabledGameCountsReached()
		/// Test IsEnabledGameCountsReached() when there is 1 enabled game.
		/// </summary>
		TEST_F(SASGameListTests, IsEnabledGameCountsReached_Test)
		{
			// Data.
			const WORD gameNumber(1234);

			// Setup.
			CSASGameList obj;
			obj.SetGameEnabled(gameNumber);
			bool needToSendInfo;
			ASSERT_FALSE(obj.IsEnabledGameCountsReached());

			obj.SetFirstPass();
			obj.GetNextGame(needToSendInfo);
			ASSERT_TRUE(obj.IsEnabledGameCountsReached());

			//Now first pass must be false but enabled game counts status should remain true	
			obj.GetNextGame(needToSendInfo);
			ASSERT_TRUE(obj.IsEnabledGameCountsReached());
			
        }
// #pragma endregion ShouldUpdateCurrentDenom
}
