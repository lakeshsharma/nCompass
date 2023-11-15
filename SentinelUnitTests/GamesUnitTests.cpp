
#include "stdafx.h"
#include "Game.h"
#include "Games.h"

#include "gtest/gtest.h"

#include "UnitTestingOperatingSystem.h"

using namespace std;

namespace SentinelUnitTests
{
		// Expected value(s).
		static const byte m_nonSasProgressiveGroup = 0;
		static const byte m_progressiveGroupDefault = m_nonSasProgressiveGroup;

		class GamesTest : public ::testing::Test
		{
			public:
				// Class member variable(s).
				int m_testIndex;
			protected:
				virtual void SetUp() 
				{
					// Test method initialization code.
					m_testIndex = 0;
				}

				virtual void TearDown() 
				{
				}
		};
		
		TEST_F(GamesTest, GameBasicTest)
		{
			CGame *game = new CGame(MEMORY_NONE, true);

			ASSERT_TRUE(game->GetMemoryType() == MEMORY_NONE) << "Game memory type incorrect after constructor" ;
			ASSERT_TRUE(game->IsEGM()) << "EGM indicator should be true after constructor" ;
			ASSERT_TRUE(game->IsEnabled()) << "Game should be enabled after constructor" ;
			ASSERT_FALSE(game->IsCurrentGame()) << "Game should not be current game after constructor" ;
			ASSERT_TRUE(game->GetDenomType() == US_DOLLARS) << "Game denom type should be US_DOLLARS after constructor" ;
			ASSERT_TRUE(game->GetDenomMultiplier() == 1) << "Game denom multiplier should be 1 after constructor" ;
			ASSERT_TRUE(game->GetTheoreticalPayback() == 0) << "Theo payback should be 0 after constructor" ;
			ASSERT_TRUE(game->GetGameNumber() == EGM_GAME_NUMBER) << "game number should be EGM_GAME_NUMBER after constructor" ;

			game->SetDenom(10);
			ASSERT_TRUE(game->GetDenomType() == US_DOLLARS) << "Game denom type should be US_DOLLARS after set" ;
			ASSERT_TRUE(game->GetDenomMultiplier() == 10) << "Game denom multiplier should be 10 after set" ;

			game->SetEGMIndicator(false);
			game->SetGameNumber(5);
			ASSERT_FALSE(game->IsEGM()) << "EGM indicator should be false after set" ;
			ASSERT_TRUE(game->GetGameNumber() == 5) << "game number should be 5 after set" ;
		
			delete game;
		}

		TEST_F(GamesTest, GameTheoPaybackTest)
		{
			CGame *game = new CGame(MEMORY_NONE, true);

			ASSERT_TRUE(game->GetTheoreticalPayback() == 0) << "Theo payback should be 0 after constructor" ;

			game->SetTheoreticalPayback(9000);
			ASSERT_TRUE(game->GetTheoreticalPayback() == 9000) << "Theo payback incorrect after WORD set" ;

			char largeTheo[] = "8800";
			game->SetTheoreticalPayback(largeTheo);
			ASSERT_TRUE(game->GetTheoreticalPayback() == 8800) << "Theo payback incorrect after large char set" ;

			char smallTheo[] = "3200";
			game->SetTheoreticalPayback(smallTheo);
			ASSERT_TRUE(game->GetTheoreticalPayback() == 13200) << "Theo payback incorrect after small char set" ;

			delete game;
		}

		TEST_F(GamesTest, GameMeterTest)
		{
			CGame *game = new CGame(MEMORY_NONE, true);

			ASSERT_TRUE(game->GetMeter(0) == INVALID_METER_VALUE) << "No meters should be present after constructor" ;

			game->SetMeter(0, 1234);
			ASSERT_TRUE(game->GetMeter(0) == 1234) << "Meter 0 incorrect after set";
			ASSERT_TRUE(game->GetMeter(1) == INVALID_METER_VALUE) << "Meter 1 should be invalid after set of meter 0";

			game->AddToMeter(0, 6);
			ASSERT_TRUE(game->GetMeter(0) == 1240) << "Meter 0 incorrect after AddToMeter";

			CMeter meter(0, 1240);
			ASSERT_TRUE( game->CompareMeter(&meter) == 0) << "Meter compare incorrect" ;

			delete game;
		}

		TEST_F(GamesTest, GameMaxBetTest)
		{
			CGame *game = new CGame(MEMORY_NONE, true);
			ASSERT_TRUE(game->GetMaxBet() == 0);
			game->SetMaxBet(100);

			ASSERT_TRUE(game->GetMaxBet() == 100);

			CGame *copy = new CGame(*game);
			ASSERT_TRUE(copy->GetMaxBet() == 100);

			delete game;
			delete copy;
		}

		TEST_F(GamesTest, GamesAddGetRemoveTest)
		{
			CGames *games = new CGames(MEMORY_NONE);
			ASSERT_TRUE(games->GetGameCount() == 0) << "Should be no games after constructor" ;

			CGame *game1 = new CGame( MEMORY_NONE, true );
			game1->SetGameNumber(1);
			CGame *game2 = new CGame( MEMORY_NONE, false );
			game2->SetGameNumber(2);
			CGame *game3 = new CGame( MEMORY_NONE, false );
			game3->SetGameNumber(3);
			CGame *game4 = new CGame( MEMORY_NONE, false );
			game4->SetGameNumber(4);

			games->AddGame(game1);
			games->AddGame(game2);
			games->AddGame(game3);
			games->AddGame(game4);

			ASSERT_TRUE(games->GetGameCount() == 4) << "Should be 4 games after set" ;
			ASSERT_TRUE(games->GetEGM() == game1) << "EGM game not found" ;
			ASSERT_TRUE(games->GetGame(3) == game3) << "Game 3 not found" ;

			games->RemoveGame(3);
			ASSERT_TRUE(games->GetGame(3) == NULL) << "Game 3 should not be found" ;
			ASSERT_TRUE(games->GetGameCount() == 3) << "Should be 3 games after remove" ;

			delete games;
		}

		TEST_F(GamesTest, GamesConvertCreditsTest)
		{
			CGames *games = new CGames(MEMORY_NONE);

			CGame *game1 = new CGame( MEMORY_NONE, true );
			game1->SetGameNumber(1);
			game1->SetDenom(5);
			CGame *game2 = new CGame( MEMORY_NONE, false );
			game2->SetGameNumber(2);
			game2->SetDenom(25);

			games->AddGame(game1);
			games->AddGame(game2);

			ASSERT_TRUE(games->ConvertCreditsForSelectedDenom(100) == 500) << "ConverCreditsSelectedGame incorrect before current game selected" ;

			game2->ChangeCurrentGameStatus();
			ASSERT_TRUE(games->ConvertCreditsForSelectedDenom(100) == 2500) << "ConverCreditsSelectedGame incorrect after current game selected" ;
			ASSERT_TRUE(games->ConvertCredits(100) == 2500) << "ConverCredits incorrect after current game selected" ;

			delete games;
		}

		TEST_F(GamesTest, GamesCopyConstructor)
		{
			CGames *games = new CGames(MEMORY_NONE);
			games->SetSASVersion("601");
			games->SetGameId("AT");
			games->SetAddress(1);
			games->SetSerialNumber("001");
			games->SetAssetNumber("111");
			games->SetAccountingDenom(1);
			games->SetMultiDenomEnabled(true);
			games->SetExtendedMetersEnabled(true);
			games->SetAftEnabled(true);
			games->SetAftBonusEnabled(true);
			games->SetAftInHouseEnabled(true);
			games->SetLegacyBonusEnabled(true);
			games->SetEftTransferFromEnabled(true);
			games->SetEftTransferToEnabled(true);
			games->SetValidationExtensionsEnabled(true);


			CGame *game1 = new CGame(MEMORY_NONE, true);
			game1->SetGameNumber(1);
			game1->SetDenom(5);
			CGame *game2 = new CGame(MEMORY_NONE, false);
			game2->SetGameNumber(2);
			game2->SetDenom(25);

			games->AddGame(game1);
			games->AddGame(game2);

			CGames *copy = new CGames(*games);
			ASSERT_TRUE(games->GetGameId() == copy->GetGameId());
			ASSERT_TRUE(games->GetAddress() == copy->GetAddress());
			ASSERT_TRUE(games->GetSerialNumber() == copy->GetSerialNumber());
			ASSERT_TRUE(games->GetAssetNumber() == copy->GetAssetNumber());
			ASSERT_TRUE(games->GetAccountingDenom() == copy->GetAccountingDenom());
			ASSERT_TRUE(games->GetMultiDenomEnabled() == copy->GetMultiDenomEnabled());
			ASSERT_TRUE(games->GetExtendedMetersEnabled() == copy->GetExtendedMetersEnabled());
			ASSERT_TRUE(games->GetAftEnabled() == copy->GetAftEnabled());
			ASSERT_TRUE(games->GetAftBonusEnabled() == copy->GetAftBonusEnabled());
			ASSERT_TRUE(games->GetAftInHouseEnabled() == copy->GetAftInHouseEnabled());
			ASSERT_TRUE(games->GetLegacyBonusEnabled() == copy->GetLegacyBonusEnabled());
			ASSERT_TRUE(games->GetEftTransferToEnabled() == copy->GetEftTransferToEnabled());
			ASSERT_TRUE(games->GetEftTransferFromEnabled() == copy->GetEftTransferFromEnabled());
			ASSERT_TRUE(games->GetValidationExtensionsEnabled() == copy->GetValidationExtensionsEnabled());
			ASSERT_TRUE(games->GetGameCount() == copy->GetGameCount());

			for (int i = 0; i < games->GetGameCount(); i++)
			{
				CGame *game = games->GetGameByIndex(i);
				CGame *gameCopy = copy->GetGameByIndex(i);

				ASSERT_TRUE(game->GetGameNumber() == gameCopy->GetGameNumber());
				ASSERT_TRUE(game->GetDenomMultiplier() == gameCopy->GetDenomMultiplier());
			}
			delete games;
			delete copy;
		}

		TEST_F(GamesTest, GamesGetEnabledGamesEmpty)
		{
			CGames *games = new CGames(MEMORY_NONE);
			games->SetSASVersion("601");
			games->SetGameId("AT");
			games->SetAddress(1);
			games->SetSerialNumber("001");
			games->SetAssetNumber("111");
			games->SetAccountingDenom(1);
			games->SetMultiDenomEnabled(true);
			games->SetExtendedMetersEnabled(true);
			games->SetAftEnabled(true);
			games->SetAftBonusEnabled(true);
			games->SetAftInHouseEnabled(true);
			games->SetLegacyBonusEnabled(true);
			games->SetEftTransferFromEnabled(true);
			games->SetEftTransferToEnabled(true);
			games->SetValidationExtensionsEnabled(true);


			CGame *game1 = new CGame(MEMORY_NONE, true);
			game1->SetGameNumber(1);
			game1->SetDenom(5);
			game1->Disable();
			CGame *game2 = new CGame(MEMORY_NONE, false);
			game2->SetGameNumber(2);
			game2->SetDenom(25);
			game2->Disable();

			games->AddGame(game1);
			games->AddGame(game2);

			CGames *enabledGames = games->GetEnabledGames();
			ASSERT_TRUE(games->GetGameId() == enabledGames->GetGameId());
			ASSERT_TRUE(games->GetAddress() == enabledGames->GetAddress());
			ASSERT_TRUE(games->GetSerialNumber() == enabledGames->GetSerialNumber());
			ASSERT_TRUE(games->GetAssetNumber() == enabledGames->GetAssetNumber());
			ASSERT_TRUE(games->GetAccountingDenom() == enabledGames->GetAccountingDenom());
			ASSERT_TRUE(games->GetMultiDenomEnabled() == enabledGames->GetMultiDenomEnabled());
			ASSERT_TRUE(games->GetExtendedMetersEnabled() == enabledGames->GetExtendedMetersEnabled());
			ASSERT_TRUE(games->GetAftEnabled() == enabledGames->GetAftEnabled());
			ASSERT_TRUE(games->GetAftBonusEnabled() == enabledGames->GetAftBonusEnabled());
			ASSERT_TRUE(games->GetAftInHouseEnabled() == enabledGames->GetAftInHouseEnabled());
			ASSERT_TRUE(games->GetLegacyBonusEnabled() == enabledGames->GetLegacyBonusEnabled());
			ASSERT_TRUE(games->GetEftTransferToEnabled() == enabledGames->GetEftTransferToEnabled());
			ASSERT_TRUE(games->GetEftTransferFromEnabled() == enabledGames->GetEftTransferFromEnabled());
			ASSERT_TRUE(games->GetValidationExtensionsEnabled() == enabledGames->GetValidationExtensionsEnabled());
			ASSERT_TRUE(enabledGames->GetGameCount() == 0);

			delete games;
			delete enabledGames;
		}

		TEST_F(GamesTest, GamesGetEnabledGames)
		{
			CGames* games = new CGames(MEMORY_NONE);
			games->SetSASVersion("601");
			games->SetGameId("AT");
			games->SetAddress(1);
			games->SetSerialNumber("001");
			games->SetAssetNumber("111");
			games->SetAccountingDenom(1);
			games->SetMultiDenomEnabled(true);
			games->SetExtendedMetersEnabled(true);
			games->SetAftEnabled(true);
			games->SetAftBonusEnabled(true);
			games->SetAftInHouseEnabled(true);
			games->SetLegacyBonusEnabled(true);
			games->SetEftTransferFromEnabled(true);
			games->SetEftTransferToEnabled(true);
			games->SetValidationExtensionsEnabled(true);


			CGame* game1 = new CGame(MEMORY_NONE, true);
			game1->SetGameNumber(1);
			game1->SetDenom(5);
			game1->Enable();
			CGame* game2 = new CGame(MEMORY_NONE, false);
			game2->SetGameNumber(2);
			game2->SetDenom(25);
			game2->Enable();

			games->AddGame(game1);
			games->AddGame(game2);

			CGames* enabledGames = games->GetEnabledGames();
			ASSERT_TRUE(games->GetGameId() == enabledGames->GetGameId());
			ASSERT_TRUE(games->GetAddress() == enabledGames->GetAddress());
			ASSERT_TRUE(games->GetSerialNumber() == enabledGames->GetSerialNumber());
			ASSERT_TRUE(games->GetAssetNumber() == enabledGames->GetAssetNumber());
			ASSERT_TRUE(games->GetAccountingDenom() == enabledGames->GetAccountingDenom());
			ASSERT_TRUE(games->GetMultiDenomEnabled() == enabledGames->GetMultiDenomEnabled());
			ASSERT_TRUE(games->GetExtendedMetersEnabled() == enabledGames->GetExtendedMetersEnabled());
			ASSERT_TRUE(games->GetAftEnabled() == enabledGames->GetAftEnabled());
			ASSERT_TRUE(games->GetAftBonusEnabled() == enabledGames->GetAftBonusEnabled());
			ASSERT_TRUE(games->GetAftInHouseEnabled() == enabledGames->GetAftInHouseEnabled());
			ASSERT_TRUE(games->GetLegacyBonusEnabled() == enabledGames->GetLegacyBonusEnabled());
			ASSERT_TRUE(games->GetEftTransferToEnabled() == enabledGames->GetEftTransferToEnabled());
			ASSERT_TRUE(games->GetEftTransferFromEnabled() == enabledGames->GetEftTransferFromEnabled());
			ASSERT_TRUE(games->GetValidationExtensionsEnabled() == enabledGames->GetValidationExtensionsEnabled());
			ASSERT_TRUE(enabledGames->GetGameCount() == 2);

			delete games;
			delete enabledGames;
		}

		TEST_F(GamesTest, GamesGetEnabledGamesPartial)
		{
			CGames* games = new CGames(MEMORY_NONE);
			games->SetSASVersion("601");
			games->SetGameId("AT");
			games->SetAddress(1);
			games->SetSerialNumber("001");
			games->SetAssetNumber("111");
			games->SetAccountingDenom(1);
			games->SetMultiDenomEnabled(true);
			games->SetExtendedMetersEnabled(true);
			games->SetAftEnabled(true);
			games->SetAftBonusEnabled(true);
			games->SetAftInHouseEnabled(true);
			games->SetLegacyBonusEnabled(true);
			games->SetEftTransferFromEnabled(true);
			games->SetEftTransferToEnabled(true);
			games->SetValidationExtensionsEnabled(true);


			CGame* game1 = new CGame(MEMORY_NONE, true);
			game1->SetGameNumber(1);
			game1->SetDenom(5);
			game1->Enable();
			CGame* game2 = new CGame(MEMORY_NONE, false);
			game2->SetGameNumber(2);
			game2->SetDenom(25);
			game2->Disable();

			games->AddGame(game1);
			games->AddGame(game2);

			CGames* enabledGames = games->GetEnabledGames();
			ASSERT_TRUE(games->GetGameId() == enabledGames->GetGameId());
			ASSERT_TRUE(games->GetAddress() == enabledGames->GetAddress());
			ASSERT_TRUE(games->GetSerialNumber() == enabledGames->GetSerialNumber());
			ASSERT_TRUE(games->GetAssetNumber() == enabledGames->GetAssetNumber());
			ASSERT_TRUE(games->GetAccountingDenom() == enabledGames->GetAccountingDenom());
			ASSERT_TRUE(games->GetMultiDenomEnabled() == enabledGames->GetMultiDenomEnabled());
			ASSERT_TRUE(games->GetExtendedMetersEnabled() == enabledGames->GetExtendedMetersEnabled());
			ASSERT_TRUE(games->GetAftEnabled() == enabledGames->GetAftEnabled());
			ASSERT_TRUE(games->GetAftBonusEnabled() == enabledGames->GetAftBonusEnabled());
			ASSERT_TRUE(games->GetAftInHouseEnabled() == enabledGames->GetAftInHouseEnabled());
			ASSERT_TRUE(games->GetLegacyBonusEnabled() == enabledGames->GetLegacyBonusEnabled());
			ASSERT_TRUE(games->GetEftTransferToEnabled() == enabledGames->GetEftTransferToEnabled());
			ASSERT_TRUE(games->GetEftTransferFromEnabled() == enabledGames->GetEftTransferFromEnabled());
			ASSERT_TRUE(games->GetValidationExtensionsEnabled() == enabledGames->GetValidationExtensionsEnabled());
			ASSERT_TRUE(enabledGames->GetGameCount() == 1);

			delete games;
			delete enabledGames;
		}

		TEST_F(GamesTest, GamesUpdateSubgameData)
		{
			CGames *games = new CGames(MEMORY_NONE);
			games->SetSASVersion("601");
			games->SetGameId("AT");
			games->SetAddress(1);
			games->SetSerialNumber("001");
			games->SetAssetNumber("111");
			games->SetAccountingDenom(1);
			games->SetMultiDenomEnabled(true);
			games->SetExtendedMetersEnabled(true);
			games->SetAftEnabled(true);
			games->SetAftBonusEnabled(true);
			games->SetAftInHouseEnabled(true);
			games->SetLegacyBonusEnabled(true);
			games->SetEftTransferFromEnabled(true);
			games->SetEftTransferToEnabled(true);
			games->SetValidationExtensionsEnabled(true);


			CGame *game1 = new CGame(MEMORY_NONE, true);
			game1->SetGameNumber(1);
			game1->SetDenom(1);
			game1->Disable();
			game1->SetPaytableID("game1");
            game1->SetGamenameLength(5);
            game1->SetGameName("abcd");
            game1->SetAdditionalInfo("ab");
			CGame *game2 = new CGame(MEMORY_NONE, false);
			game2->SetGameNumber(2);
			game2->SetDenom(1);
			game2->Enable();
			game2->SetPaytableID("game2");
            game1->SetGamenameLength(5);
            game1->SetGameName("bcde");
            game1->SetAdditionalInfo("cd");

			games->AddGame(game1);
			games->AddGame(game2);


			// Create 2 denoms for each game
			CGames *denomGames = new CGames(MEMORY_NONE);
			CGame *singleGame = new CGame(MEMORY_NONE);
			singleGame->SetGameNumber(1);
			singleGame->SetPaytableID("game1");
			singleGame->SetDenom(5);
			denomGames->AddGame(singleGame);

			singleGame = new CGame(MEMORY_NONE);
			singleGame->SetGameNumber(1);
			singleGame->SetPaytableID("game2");
			singleGame->SetDenom(25);
			denomGames->AddGame(singleGame);

			singleGame = new CGame(MEMORY_NONE);
			singleGame->SetGameNumber(2);
			singleGame->SetPaytableID("game3");
			singleGame->SetDenom(5);
			denomGames->AddGame(singleGame);

			singleGame = new CGame(MEMORY_NONE);
			singleGame->SetGameNumber(2);
			singleGame->SetPaytableID("game4");
			singleGame->SetDenom(25);
			denomGames->AddGame(singleGame);

			// Copy games/subgame data from "games" into denomGames
			denomGames->UpdateSubGameData(games);
			ASSERT_TRUE(games->GetGameId() == denomGames->GetGameId());
			ASSERT_TRUE(games->GetAddress() == denomGames->GetAddress());
			ASSERT_TRUE(games->GetSerialNumber() == denomGames->GetSerialNumber());
			ASSERT_TRUE(games->GetAssetNumber() == denomGames->GetAssetNumber());
			ASSERT_TRUE(games->GetAccountingDenom() == denomGames->GetAccountingDenom());
			ASSERT_TRUE(games->GetMultiDenomEnabled() == denomGames->GetMultiDenomEnabled());
			ASSERT_TRUE(games->GetExtendedMetersEnabled() == denomGames->GetExtendedMetersEnabled());
			ASSERT_TRUE(games->GetAftEnabled() == denomGames->GetAftEnabled());
			ASSERT_TRUE(games->GetAftBonusEnabled() == denomGames->GetAftBonusEnabled());
			ASSERT_TRUE(games->GetAftInHouseEnabled() == denomGames->GetAftInHouseEnabled());
			ASSERT_TRUE(games->GetLegacyBonusEnabled() == denomGames->GetLegacyBonusEnabled());
			ASSERT_TRUE(games->GetEftTransferToEnabled() == denomGames->GetEftTransferToEnabled());
			ASSERT_TRUE(games->GetEftTransferFromEnabled() == denomGames->GetEftTransferFromEnabled());
			ASSERT_TRUE(games->GetValidationExtensionsEnabled() == denomGames->GetValidationExtensionsEnabled());
			ASSERT_TRUE(denomGames->GetGameCount() == 4);

			CGame *subGame = denomGames->GetGameByIndex(0);
			ASSERT_TRUE(strcmp(subGame->GetPaytableID(), "game1") == 0);
			ASSERT_TRUE(subGame->GetDenomMultiplier() == 5);

			subGame = denomGames->GetGameByIndex(1);
			ASSERT_TRUE(strcmp(subGame->GetPaytableID(), "game2") == 0);
			ASSERT_TRUE(subGame->GetDenomMultiplier() == 25);

			subGame = denomGames->GetGameByIndex(2);
			ASSERT_TRUE(strcmp(subGame->GetPaytableID(), "game3") == 0);
			ASSERT_TRUE(subGame->GetDenomMultiplier() == 5);

			subGame = denomGames->GetGameByIndex(3);
			ASSERT_TRUE(strcmp(subGame->GetPaytableID(), "game4") == 0);
			ASSERT_TRUE(subGame->GetDenomMultiplier() == 25);

			delete games;
			delete denomGames;
		}

// #pragma region CGames::GetProgressiveGroup()
		/// <summary>
		/// Code coverage and functional test for:
		///     CGames::CGames()
		///     CGames::GetProgressiveGroup()
		/// Test default value.
		/// </summary>
		TEST_F(GamesTest, GetProgressiveGroup_ConstructorTest)
		{
			const CGames obj;
			ASSERT_EQ(m_progressiveGroupDefault, obj.GetProgressiveGroup()) << "m_progressiveGroupDefault";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CGames::SetProgressiveGroup()
		///     CGames::GetProgressiveGroup()
		/// Test setting a range of values.
		/// </summary>
		TEST_F(GamesTest, SetProgressiveGroupTest)
		{
			vector<BYTE> testData{0,1,2,123,126,127,128,129,210,254,255};

			CGames varObjA;
			for(BYTE testValue : testData)
			{
				varObjA.SetProgressiveGroup(testValue);
				CGames &objA(varObjA);
				ASSERT_EQ(testValue, objA.GetProgressiveGroup()) << "objA.GetProgressiveGroup()";

				// Setup.  Use a different object for each test sequence.
				CGames varObjB;

				// Perform operation(s) and test(s).
				varObjB.SetProgressiveGroup(testValue);
				CGames &objB(varObjB);
				ASSERT_EQ(testValue, objB.GetProgressiveGroup()) << "objB.GetProgressiveGroup()";
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CGames::SetProgressiveGroup()
		///     CGames::CGames(const CGames &)
		///     CGames::GetProgressiveGroup()
		/// Test copy constructor after setting a range of values.
		/// </summary>
		TEST_F(GamesTest, SetProgressiveGroup_CopyTest)
		{
			vector<BYTE> testData{0,1,2,123,126,127,128,129,210,254,255};

			for (auto testValue : testData)
			{
				// Setup.
				CGames varObjA;
				varObjA.SetProgressiveGroup(testValue);
				const CGames &objA(varObjA);

				// Perform operation(s) and test(s).
				const CGames objB(objA);
				ASSERT_EQ(testValue, objB.GetProgressiveGroup()) << "objB.GetProgressiveGroup()";
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CGames::SetProgressiveGroup()
		///     CGames::operator=(const CGames &)
		///     CGames::GetProgressiveGroup()
		/// Test assigment operator after setting a range of values.
		/// </summary>
		TEST_F(GamesTest, SetProgressiveGroup_AssignmentTest)
		{
			vector<BYTE> testData{0,1,2,123,126,127,128,129,210,254,255};

			for (auto testValue : testData)
			{
				// Setup.
				CGames varObjA;
				varObjA.SetProgressiveGroup(testValue);
				const CGames &objA(varObjA);
				CGames objB;
				objB.SetProgressiveGroup((BYTE)~testValue);
				ASSERT_EQ((BYTE)~testValue, objB.GetProgressiveGroup()) << "Setup objB.GetProgressiveGroup()"; // Verify setup to a different value.

				// Perform operation(s) and test(s).
				objB = objA;
				ASSERT_EQ(testValue, objB.GetProgressiveGroup()) << "objB.GetProgressiveGroup()";
			}
		}

// #pragma endregion CGames::GetProgressiveGroup()

// #pragma region CGames::CalcProgressiveGroup()
		/// <summary>
		/// Code coverage and functional test for:
		///     CGames::CalcProgressiveGroup()
		/// Test within initial non-SAS value and no games.
		/// </summary>
		TEST_F(GamesTest, CalcProgressiveGroup0Test)
		{
			ASSERT_EQ(m_nonSasProgressiveGroup, (byte)0) << "m_nonSasProgressiveGroup"; // Verify test conditions.
			const byte progressiveGroupExpected(0);

			CGames obj;
			obj.SetProgressiveGroup(0);
			obj.CalcProgressiveGroup();
			ASSERT_EQ(progressiveGroupExpected, obj.GetProgressiveGroup()) << "obj.GetProgressiveGroup()";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CGames::CalcProgressiveGroup()
		/// Test with initial SAS value and no games.
		/// </summary>
		TEST_F(GamesTest, CalcProgressiveGroup1Test)
		{
			ASSERT_EQ(m_nonSasProgressiveGroup, (byte)0) << "m_nonSasProgressiveGroup"; // Verify test conditions.
			const byte progressiveGroupExpected(5);

			CGames obj;
			obj.SetProgressiveGroup(progressiveGroupExpected);
			obj.CalcProgressiveGroup();
			ASSERT_EQ(progressiveGroupExpected, obj.GetProgressiveGroup()) << "obj.GetProgressiveGroup()";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CGames::CalcProgressiveGroup()
		/// Test initial non-SAS value and one game with a non-SAS value.
		/// </summary>
		TEST_F(GamesTest, CalcProgressiveGroup00Test)
		{
			ASSERT_EQ(m_nonSasProgressiveGroup, (byte)0) << "m_nonSasProgressiveGroup"; // Verify test conditions.
			const byte progressiveGroupExpected(0);

			CGames obj;

			CGame *game0(new CGame(MEMORY_NONE, true));
			game0->SetGameNumber(0);
			game0->SetProgressiveGroup(0);
			obj.AddGame(game0);

			obj.SetProgressiveGroup(0);
			obj.CalcProgressiveGroup();
			ASSERT_EQ(progressiveGroupExpected, obj.GetProgressiveGroup()) << "obj.GetProgressiveGroup()";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CGames::CalcProgressiveGroup()
		/// Test initial non-SAS value and one game with a SAS value.
		/// </summary>
		TEST_F(GamesTest, CalcProgressiveGroup01Test)
		{
			ASSERT_EQ(m_nonSasProgressiveGroup, (byte)0) << "m_nonSasProgressiveGroup"; // Verify test conditions.
			const byte progressiveGroupExpected(1);
	
			CGames obj;

			CGame *game0(new CGame(MEMORY_NONE, true));
			game0->SetGameNumber(0);
			game0->SetProgressiveGroup(progressiveGroupExpected);
			obj.AddGame(game0);

			obj.SetProgressiveGroup(0);
			obj.CalcProgressiveGroup();
			ASSERT_EQ(progressiveGroupExpected, obj.GetProgressiveGroup()) << "obj.GetProgressiveGroup()";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CGames::CalcProgressiveGroup()
		/// Test initial SAS value and one game with a non-SAS value.
		/// </summary>
		TEST_F(GamesTest, CalcProgressiveGroup10Test)
		{
			ASSERT_EQ(m_nonSasProgressiveGroup, (byte)0) << "m_nonSasProgressiveGroup"; // Verify test conditions.
			const byte progressiveGroupExpected(7);

			CGames obj;

			CGame *game0(new CGame(MEMORY_NONE, true));
			game0->SetGameNumber(0);
			game0->SetProgressiveGroup(0);
			obj.AddGame(game0);

			obj.SetProgressiveGroup(progressiveGroupExpected);
			obj.CalcProgressiveGroup();
			ASSERT_EQ(progressiveGroupExpected, obj.GetProgressiveGroup()) << "obj.GetProgressiveGroup()";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CGames::CalcProgressiveGroup()
		/// Test initial SAS value and one game with a SAS value.
		/// </summary>
		TEST_F(GamesTest, CalcProgressiveGroup11Test)
		{
			ASSERT_EQ(m_nonSasProgressiveGroup, (byte)0) << "m_nonSasProgressiveGroup"; // Verify test conditions.
			const byte progressiveGroupExpected(2);

			CGames obj;

			CGame *game0(new CGame(MEMORY_NONE, true));
			game0->SetGameNumber(0);
			game0->SetProgressiveGroup(1);
			obj.AddGame(game0);

			obj.SetProgressiveGroup(progressiveGroupExpected);
			obj.CalcProgressiveGroup();
			ASSERT_EQ(progressiveGroupExpected, obj.GetProgressiveGroup()) << "obj.GetProgressiveGroup()";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CGames::CalcProgressiveGroup()
		/// Test with initial non-SAS value and multiple games.  First game has a SAS progressive group.
		/// </summary>
		TEST_F(GamesTest, CalcProgressiveGroup3aTest)
		{
			ASSERT_EQ(m_nonSasProgressiveGroup, (byte)0) << "m_nonSasProgressiveGroup"; // Verify test conditions.
			const byte progressiveGroupExpected(10);

			CGames obj;

			{
				CGame *game0(new CGame(MEMORY_NONE, true));
				game0->SetGameNumber(0);
				game0->SetProgressiveGroup(progressiveGroupExpected);
				obj.AddGame(game0);
			}

			{
				CGame *game1(new CGame(MEMORY_NONE, true));
				game1->SetGameNumber(1);
				game1->SetProgressiveGroup(progressiveGroupExpected);
				obj.AddGame(game1);
			}

			{
				CGame *game2(new CGame(MEMORY_NONE, true));
				game2->SetGameNumber(2);
				game2->SetProgressiveGroup(0);
				obj.AddGame(game2);
			}

			obj.SetProgressiveGroup(progressiveGroupExpected);
			obj.CalcProgressiveGroup();
			ASSERT_EQ(progressiveGroupExpected, obj.GetProgressiveGroup()) << "obj.GetProgressiveGroup()";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CGames::CalcProgressiveGroup()
		/// Test with initial non-SAS value and multiple games.  Second game has a SAS progressive group.
		/// </summary>
		TEST_F(GamesTest, CalcProgressiveGroup3bTest)
		{
			ASSERT_EQ(m_nonSasProgressiveGroup, (byte)0) << "m_nonSasProgressiveGroup"; // Verify test conditions.
			const byte progressiveGroupExpected(12);

			CGames obj;

			{
				CGame *game0(new CGame(MEMORY_NONE, true));
				game0->SetGameNumber(0);
				game0->SetProgressiveGroup(0);
				obj.AddGame(game0);
			}

			{
				CGame *game1(new CGame(MEMORY_NONE, true));
				game1->SetGameNumber(1);
				game1->SetProgressiveGroup(progressiveGroupExpected);
				obj.AddGame(game1);
			}

			{
				CGame *game2(new CGame(MEMORY_NONE, true));
				game2->SetGameNumber(2);
				game2->SetProgressiveGroup(0);
				obj.AddGame(game2);
			}

			obj.SetProgressiveGroup(progressiveGroupExpected);
			obj.CalcProgressiveGroup();
			ASSERT_EQ(progressiveGroupExpected, obj.GetProgressiveGroup()) << "obj.GetProgressiveGroup()";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CGames::CalcProgressiveGroup()
		/// Test with initial non-SAS value and multiple games.  Third game has a SAS progressive group.
		/// </summary>
		TEST_F(GamesTest, CalcProgressiveGroup3cTest)
		{
			ASSERT_EQ(m_nonSasProgressiveGroup, (byte)0) << "m_nonSasProgressiveGroup"; // Verify test conditions.
			const byte progressiveGroupExpected(13);

			CGames obj;

			{
				CGame *game0(new CGame(MEMORY_NONE, true));
				game0->SetGameNumber(0);
				game0->SetProgressiveGroup(0);
				obj.AddGame(game0);
			}

			{
				CGame *game1(new CGame(MEMORY_NONE, true));
				game1->SetGameNumber(1);
				game1->SetProgressiveGroup(0);
				obj.AddGame(game1);
			}

			{
				CGame *game2(new CGame(MEMORY_NONE, true));
				game2->SetGameNumber(2);
				game2->SetProgressiveGroup(progressiveGroupExpected);
				obj.AddGame(game2);
			}

			obj.SetProgressiveGroup(progressiveGroupExpected);
			obj.CalcProgressiveGroup();
			ASSERT_EQ(progressiveGroupExpected, obj.GetProgressiveGroup()) << "obj.GetProgressiveGroup()";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CGames::CalcProgressiveGroup()
		/// Test with initial non-SAS value and multiple games.  All three games have non-SAS progressive groups.
		/// </summary>
		TEST_F(GamesTest, CalcProgressiveGroup3abcTest)
		{
			ASSERT_EQ(m_nonSasProgressiveGroup, (byte)0) << "m_nonSasProgressiveGroup"; // Verify test conditions.
			const byte progressiveGroupExpected(20);

			CGames obj;

			{
				CGame *game0(new CGame(MEMORY_NONE, true));
				game0->SetGameNumber(0);
				game0->SetProgressiveGroup(progressiveGroupExpected);
				obj.AddGame(game0);
			}

			{
				CGame *game1(new CGame(MEMORY_NONE, true));
				game1->SetGameNumber(1);
				game1->SetProgressiveGroup(11);
				obj.AddGame(game1);
			}

			{
				CGame *game2(new CGame(MEMORY_NONE, true));
				game2->SetGameNumber(2);
				game2->SetProgressiveGroup(32);
				obj.AddGame(game2);
			}

			obj.SetProgressiveGroup(progressiveGroupExpected);
			obj.CalcProgressiveGroup();
			ASSERT_EQ(progressiveGroupExpected, obj.GetProgressiveGroup()) << "obj.GetProgressiveGroup()";
		}
// #pragma endregion CGames::CalcProgressiveGroup()

// #pragma region SetGame
		/// <summary>
		/// Functional test for:
		///     CGames::SetGame()
		/// Test adding a nullptr sourceGame.
		/// </summary>
		TEST_F(GamesTest, SetGame_Null)
		{
			// Setup.
			CGames games;
			CGame *sourceGame(nullptr);

			// Perform operation(s) and test(s).
			ASSERT_FALSE(games.SetGame(sourceGame)) << "SetGame(sourceGame)";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::SetGame()
		/// Test adding a sourceGame.
		/// </summary>
		TEST_F(GamesTest, SetGame_Add)
		{
			// Data.
			const int denomMultiplier(15);

			// Setup.
			CGames games;
			CGame sourceGame(MEMORY_NONE);
			sourceGame.SetDenom(denomMultiplier);

			// Perform operation(s) and test(s).
			ASSERT_EQ(true, games.SetGame(&sourceGame)) << "SetGame(&sourceGame)";
			ASSERT_EQ(denomMultiplier, games.GetGameByIndex(0)->GetDenomMultiplier()) << "GetGameByIndex(0)->GetDenomMultiplier()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::SetGame()
		/// Test updating an existing game number.
		/// </summary>
		TEST_F(GamesTest, SetGame_Update)
		{
			// Data.
			const DWORD gameNumber(2);
			const int denomMultiplier(15);

			// Setup.
			CGames games;
			CGame *targetGame(new CGame(MEMORY_NONE));
			targetGame->SetGameNumber(gameNumber);
			targetGame->SetDenom(denomMultiplier - 5);
			games.AddGame(targetGame);

			CGame sourceGame(MEMORY_NONE);
			sourceGame.SetGameNumber(gameNumber);
			sourceGame.SetDenom(denomMultiplier);

			// Perform operation(s) and test(s).
			ASSERT_FALSE(games.SetGame(&sourceGame)) << "SetGame(&sourceGame)";
			ASSERT_EQ(denomMultiplier, games.GetGame(gameNumber)->GetDenomMultiplier()) << "GetGameByIndex(gameNumber)->GetDenomMultiplier()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::SetGame()
		/// Test changing the EGM game number.
		/// </summary>
		TEST_F(GamesTest, SetGame_ChangeEgmGameNumber)
		{
			// Data.
			const DWORD oldEgmGameNumber(0);
			const DWORD newEgmGameNumber(1);

			// Setup.
			CGames games;
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetEGMIndicator(true);
			egmGame->SetGameNumber(oldEgmGameNumber);
			games.AddGame(egmGame);

			CGame sourceGame(MEMORY_NONE);
			sourceGame.SetEGMIndicator(true);
			sourceGame.SetGameNumber(newEgmGameNumber);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(games.SetGame(&sourceGame)) << "SetGame(&sourceGame)";
			ASSERT_EQ(oldEgmGameNumber, games.GetEGM()->GetGameNumber()) << "GetEGM()->GetGameNumber()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::SetGame()
		/// Test adding a sourceGame that is the current game.
		/// </summary>
		TEST_F(GamesTest, SetGame_AddCurrentGame)
		{
			// Setup.
			CGames games;
			CGame sourceGame(MEMORY_NONE);
			sourceGame.ChangeCurrentGameStatus(true);

			// Perform operation(s) and test(s).
			ASSERT_EQ(true, games.SetGame(&sourceGame)) << "SetGame(&sourceGame)";
			ASSERT_EQ(true, games.GetGameByIndex(0)->IsCurrentGame()) << "GetGameByIndex(0)->IsCurrentGame()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::SetGame()
		/// Test adding a sourceGame that changes the current game.
		/// </summary>
		TEST_F(GamesTest, SetGame_ChangeCurrentGame)
		{
			// Data.
			const DWORD gameNumberA(0);
			const DWORD gameNumberB(2);

			// Setup.
			CGames games;
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetGameNumber(gameNumberA);
			egmGame->ChangeCurrentGameStatus(true);
			games.AddGame(egmGame);

			CGame sourceGame(MEMORY_NONE);
			sourceGame.SetGameNumber(gameNumberB);
			sourceGame.ChangeCurrentGameStatus(true);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(games.SetGame(&sourceGame)) << "SetGame(&sourceGame)";
			ASSERT_FALSE(games.GetGame(gameNumberA)->IsCurrentGame()) << "GetGame(0)->IsCurrentGame()";
			ASSERT_TRUE(games.GetGame(gameNumberB)->IsCurrentGame()) << "GetGame(2)->IsCurrentGame()";
		}
// #pragma endregion SetGame

// #pragma region SetCurrentGame
		/// <summary>
		/// Functional test for:
		///     CGames::SetCurrentGame()
		/// Test when there is no prior current game and the new current game is not present.
		/// </summary>
		TEST_F(GamesTest, SetCurrentGame_NeitherPresent)
		{
			// Setup.
			CGames games;

			// Perform operation(s) and test(s).
			games.SetCurrentGame(3);
			ASSERT_TRUE(games.GetCurrentGame() == NULL) << "GetCurrentGame()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::SetCurrentGame()
		/// Test when there is no prior current game, but the new game is present.
		/// </summary>
		TEST_F(GamesTest, SetCurrentGame_NoPriorCurrentGame)
		{
			// Data.
			const DWORD gameNumberB(3);

			// Setup.
			CGames games;

			CGame *gameB(new CGame(MEMORY_NONE));
			gameB->SetGameNumber(gameNumberB);
			games.AddGame(gameB);

			// Perform operation(s) and test(s).
			games.SetCurrentGame(gameNumberB);
			ASSERT_EQ(true, games.GetGame(gameNumberB)->IsCurrentGame()) << "GetGame(gameNumberB).IsCurrentGame()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::SetCurrentGame()
		/// Test when there is a prior current game, but the new game is not present.
		/// </summary>
		TEST_F(GamesTest, SetCurrentGame_NoNewGame)
		{
			// Data.
			const DWORD gameNumberA(2);
			const DWORD gameNumberB(3);

			// Setup.
			CGames games;

			CGame *gameA(new CGame(MEMORY_NONE));
			gameA->SetGameNumber(gameNumberA);
			games.AddGame(gameA);

			// Perform operation(s) and test(s).
			games.SetCurrentGame(gameNumberB);
			ASSERT_FALSE(games.GetGame(gameNumberA)->IsCurrentGame()) << "GetGame(gameNumberA).IsCurrentGame()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::SetCurrentGame()
		/// Test when there is a prior current game and the new game is present.
		/// </summary>
		TEST_F(GamesTest, SetCurrentGame_BothPresent)
		{
			// Data.
			const DWORD gameNumberA(2);
			const DWORD gameNumberB(3);

			// Setup.
			CGames games;

			CGame *gameA(new CGame(MEMORY_NONE));
			gameA->SetGameNumber(gameNumberA);
			games.AddGame(gameA);

			CGame *gameB(new CGame(MEMORY_NONE));
			gameB->SetGameNumber(gameNumberB);
			games.AddGame(gameB);

			// Perform operation(s) and test(s).
			games.SetCurrentGame(gameNumberB);
			ASSERT_FALSE(games.GetGame(gameNumberA)->IsCurrentGame()) << "GetGame(gameNumberA).IsCurrentGame()";
			ASSERT_TRUE(games.GetGame(gameNumberB)->IsCurrentGame()) << "GetGame(gameNumberB).IsCurrentGame()";
		}
// #pragma endregion SetCurrentGame

// #pragma region GetGame
		/// <summary>
		/// Functional test for:
		///     CGames::GetGame() const
		/// Test a const instance when there are no games present.
		/// </summary>
		TEST_F(GamesTest, GetGame_Const_NonePresent)
		{
			// Setup.
			const CGames games;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(games.GetGame(EGM_GAME_NUMBER) == NULL) << "GetGame(EGM_GAME_NUMBER)";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetGame() const
		/// Test a const instance when the requested game is not present.
		/// </summary>
		TEST_F(GamesTest, GetGame_Const_NotPresent)
		{
			// Data.
			const DWORD gameNumberPresent(3);
			const DWORD gameNumberRequested(4);

			// Setup.
			CGames varGames;

			CGame *game(new CGame(MEMORY_NONE));
			game->SetGameNumber(gameNumberPresent);
			varGames.AddGame(game);
			const CGames &games(varGames);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(games.GetGame(gameNumberRequested) == NULL) << "GetGame(gameNumber)";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetGame() const
		/// Test a const instance when the requested game is present.
		/// </summary>
		TEST_F(GamesTest, GetGame_Const_Present)
		{
			// Data.
			const DWORD gameNumberPresent(3);
			const DWORD gameNumberRequested(gameNumberPresent);

			// Setup.
			CGames varGames;

			CGame *game(new CGame(MEMORY_NONE));
			game->SetGameNumber(gameNumberPresent);
			varGames.AddGame(game);
			const CGames &games(varGames);

			// Perform operation(s) and test(s).
			ASSERT_EQ((void *)game, (void *)games.GetGame(gameNumberRequested)) << "GetGame(gameNumberRequested)";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetGame() const
		/// Test a non-const instance when there are no games present.
		/// </summary>
		TEST_F(GamesTest, GetGame_NonePresent)
		{
			// Setup.
			CGames games;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(games.GetGame(EGM_GAME_NUMBER) == NULL) << "GetGame(EGM_GAME_NUMBER)";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetGame() const
		/// Test a non-const instance when the requested game is not present.
		/// </summary>
		TEST_F(GamesTest, GetGame_NotPresent)
		{
			// Data.
			const DWORD gameNumberPresent(3);
			const DWORD gameNumberRequested(4);

			// Setup.
			CGames games;

			CGame *game(new CGame(MEMORY_NONE));
			game->SetGameNumber(gameNumberPresent);
			games.AddGame(game);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(games.GetGame(gameNumberRequested) == NULL) << "GetGame(gameNumber)";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetGame() const
		/// Test a non-const instance when the requested game is present.
		/// </summary>
		TEST_F(GamesTest, GetGame_Present)
		{
			// Data.
			const DWORD gameNumberPresent(3);
			const DWORD gameNumberRequested(gameNumberPresent);

			// Setup.
			CGames games;

			CGame *game(new CGame(MEMORY_NONE));
			game->SetGameNumber(gameNumberPresent);
			games.AddGame(game);

			// Perform operation(s) and test(s).
			ASSERT_EQ((void *)game, (void *)games.GetGame(gameNumberRequested)) << "GetGame(gameNumberRequested)";
		}
// #pragma endregion GetGame

// #pragma region GetCurrentGame
		/// <summary>
		/// Functional test for:
		///     CGames::GetCurrentGame() const
		/// Test a const instance when there is no current game.
		/// </summary>
		TEST_F(GamesTest, GetCurrentGame_Const_NotPresent)
		{
			// Setup.
			const CGames games;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(games.GetCurrentGame() == NULL) << "GetCurrentGame()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetCurrentGame() const
		/// Test a const instance when there is a current game.
		/// </summary>
		TEST_F(GamesTest, GetCurrentGame_Const_Present)
		{
			// Data.
			const DWORD gameNumber(3);

			// Setup.
			CGames varGames;

			CGame *game(new CGame(MEMORY_NONE));
			game->ChangeCurrentGameStatus(true);
			varGames.AddGame(game);
			const CGames &games(varGames);

			// Perform operation(s) and test(s).
			ASSERT_EQ((void *)game, (void *)games.GetCurrentGame()) << "GetCurrentGame()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetCurrentGame()
		/// Test a non-const instance when there is no current game.
		/// </summary>
		TEST_F(GamesTest, GetCurrentGame_NotPresent)
		{
			// Setup.
			CGames games;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(games.GetCurrentGame() == NULL) << "GetCurrentGame()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetCurrentGame()
		/// Test a non-const instance when there is a current game.
		/// </summary>
		TEST_F(GamesTest, GetCurrentGame_Present)
		{
			// Data.
			const DWORD gameNumber(3);

			// Setup.
			CGames games;

			CGame *game(new CGame(MEMORY_NONE));
			game->ChangeCurrentGameStatus(true);
			games.AddGame(game);

			// Perform operation(s) and test(s).
			ASSERT_EQ((void *)game, (void *)games.GetCurrentGame()) << "GetCurrentGame()";
		}
// #pragma endregion GetCurrentGame

// #pragma region GetAdjustedCurrentGameCopy
		/// <summary>
		/// Functional test for:
		///     CGames::GetAdjustedCurrentGameCopy()
		/// Test when there are no games.
		/// </summary>
		TEST_F(GamesTest, GetAdjustedCurrentGameCopy_NoGames)
		{
			// Setup.
			const CGames games;

			// Perform operation(s) and test(s).
			unique_ptr<CGame> currentGame(games.GetAdjustedCurrentGameCopy());
			ASSERT_TRUE(currentGame.get() == NULL) << "get()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetAdjustedCurrentGameCopy()
		/// Test when there is an EGM game with game number 0.
		/// </summary>
		TEST_F(GamesTest, GetAdjustedCurrentGameCopy_EgmGame0)
		{
			// Data.
			const DWORD egmGameNumber(EGM_GAME_NUMBER);
			const DWORD egmGameDenom(15);

			const bool isEgmGameExpected(false);
			const DWORD gameNumberExpected(1);
			const int denomExpected(egmGameDenom);

			// Setup.
			CGames games;
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetGameNumber(egmGameNumber);
			egmGame->SetDenom(denomExpected);
			games.AddGame(egmGame);

			// Perform operation(s) and test(s).
			unique_ptr<CGame> currentGame(games.GetAdjustedCurrentGameCopy());
			ASSERT_EQ(isEgmGameExpected, currentGame->IsEGM()) << "IsEGM()";
			ASSERT_EQ(gameNumberExpected, currentGame->GetGameNumber()) << "GetGameNumber()";
			ASSERT_EQ(denomExpected, currentGame->GetDenomMultiplier()) << "GetDenomMultiplier()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetAdjustedCurrentGameCopy()
		/// Test when there is an EGM game with game number 1.
		/// </summary>
		TEST_F(GamesTest, GetAdjustedCurrentGameCopy_EgmGame1)
		{
			// Data.
			const DWORD egmGameNumber(1);
			const DWORD egmGameDenom(15);

			const bool isEgmGameExpected(false);
			const DWORD gameNumberExpected(EGM_GAME_NUMBER == egmGameNumber ? 1 : egmGameNumber);
			const int denomExpected(egmGameDenom);

			// Setup.
			CGames games;
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetEGMIndicator(true);
			egmGame->SetGameNumber(egmGameNumber);
			egmGame->SetDenom(egmGameDenom);
			games.AddGame(egmGame);

			// Perform operation(s) and test(s).
			unique_ptr<CGame> currentGame(games.GetAdjustedCurrentGameCopy());
			ASSERT_EQ(isEgmGameExpected, currentGame->IsEGM()) << "IsEGM()";
			ASSERT_EQ(gameNumberExpected, currentGame->GetGameNumber()) << "GetGameNumber()";
			ASSERT_EQ(denomExpected, currentGame->GetDenomMultiplier()) << "GetDenomMultiplier()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetAdjustedCurrentGameCopy()
		/// Test when there is an EGM game with game number 2.
		/// </summary>
		TEST_F(GamesTest, GetAdjustedCurrentGameCopy_EgmGame2)
		{
			// Data.
			const DWORD egmGameNumber(2);
			const DWORD egmGameDenom(15);

			const bool isEgmGameExpected(false);
			const DWORD gameNumberExpected(EGM_GAME_NUMBER == egmGameNumber ? 1 : egmGameNumber);
			const int denomExpected(egmGameDenom);

			// Setup.
			CGames games;
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetEGMIndicator(true);
			egmGame->SetGameNumber(egmGameNumber);
			egmGame->SetDenom(egmGameDenom);
			games.AddGame(egmGame);

			// Perform operation(s) and test(s).
			unique_ptr<CGame> currentGame(games.GetAdjustedCurrentGameCopy());
			ASSERT_EQ(isEgmGameExpected, currentGame->IsEGM()) << "IsEGM()";
			ASSERT_EQ(gameNumberExpected, currentGame->GetGameNumber()) << "GetGameNumber()";
			ASSERT_EQ(denomExpected, currentGame->GetDenomMultiplier()) << "GetDenomMultiplier()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetAdjustedCurrentGameCopy()
		/// Test when there is an non-EGM game with game number 2.
		/// </summary>
		TEST_F(GamesTest, GetAdjustedCurrentGameCopy_NonEgmGame2)
		{
			// Data.
			const DWORD subGameNumber(2);
			const DWORD subGameDenom(15);

			const bool isEgmGameExpected(false);
			const DWORD gameNumberExpected(subGameNumber);
			const int denomExpected(subGameDenom);

			// Setup.
			CGames games;
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetEGMIndicator(false);
			egmGame->SetGameNumber(subGameNumber);
			egmGame->SetDenom(subGameDenom);
			games.AddGame(egmGame);

			// Perform operation(s) and test(s).
			unique_ptr<CGame> currentGame(games.GetAdjustedCurrentGameCopy());
			ASSERT_EQ(isEgmGameExpected, currentGame->IsEGM()) << "IsEGM()";
			ASSERT_EQ(gameNumberExpected, currentGame->GetGameNumber()) << "GetGameNumber()";
			ASSERT_EQ(denomExpected, currentGame->GetDenomMultiplier()) << "GetDenomMultiplier()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetAdjustedCurrentGameCopy()
		/// Test when there is an EGM game and a disabled sub game where neither game is the current game.
		/// </summary>
		TEST_F(GamesTest, GetAdjustedCurrentGameCopy_DisabledSubGame_NoCurrent)
		{
			// Data.
			DWORD egmGameNumber(0);
			const int egmGameDenom(15);

			const DWORD subGameNumber(6);
			const DWORD subGameDenom(egmGameDenom * 5);

			const bool isEgmGameExpected(false);
			const DWORD gameNumberExpected(1);
			const int denomExpected(egmGameDenom);

			// Setup.
			CGames games;
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetEGMIndicator(true);
			egmGame->SetGameNumber(egmGameNumber);
			egmGame->SetDenom(egmGameDenom);
			games.AddGame(egmGame);

			CGame *subGame(new CGame(MEMORY_NONE));
			subGame->Disable();
			subGame->SetGameNumber(subGameNumber);
			subGame->SetDenom(subGameDenom);
			games.AddGame(subGame);

			// Perform operation(s) and test(s).
			unique_ptr<CGame> currentGame(games.GetAdjustedCurrentGameCopy());
			ASSERT_EQ(isEgmGameExpected, currentGame->IsEGM()) << "IsEGM()";
			ASSERT_EQ(gameNumberExpected, currentGame->GetGameNumber()) << "GetGameNumber()";
			ASSERT_EQ(denomExpected, currentGame->GetDenomMultiplier()) << "GetDenomMultiplier()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetAdjustedCurrentGameCopy()
		/// Test when there is an EGM game and a disabled sub game and the EGM game is the current game.
		/// </summary>
		TEST_F(GamesTest, GetAdjustedCurrentGameCopy_DisabledSubGame_EgmGameCurrent)
		{
			// Data.
			DWORD egmGameNumber(0);
			const int egmGameDenom(15);

			const DWORD subGameNumber(6);
			const DWORD subGameDenom(egmGameDenom * 5);

			const bool isEgmGameExpected(false);
			const DWORD gameNumberExpected(1);
			const int denomExpected(egmGameDenom);

			// Setup.
			CGames games;
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetEGMIndicator(true);
			egmGame->SetGameNumber(egmGameNumber);
			egmGame->SetDenom(egmGameDenom);
			egmGame->ChangeCurrentGameStatus(true);
			games.AddGame(egmGame);

			CGame *subGame(new CGame(MEMORY_NONE));
			subGame->Disable();
			subGame->SetGameNumber(subGameNumber);
			subGame->SetDenom(subGameDenom);
			games.AddGame(subGame);

			// Perform operation(s) and test(s).
			unique_ptr<CGame> currentGame(games.GetAdjustedCurrentGameCopy());
			ASSERT_EQ(isEgmGameExpected, currentGame->IsEGM()) << "IsEGM()";
			ASSERT_EQ(gameNumberExpected, currentGame->GetGameNumber()) << "GetGameNumber()";
			ASSERT_EQ(denomExpected, currentGame->GetDenomMultiplier()) << "GetDenomMultiplier()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetAdjustedCurrentGameCopy()
		/// Test when there is an EGM game and an enabled sub game where neither game is the current game.
		/// </summary>
		TEST_F(GamesTest, GetAdjustedCurrentGameCopy_EnabledSubGame_NoCurrent)
		{
			// Data.
			DWORD egmGameNumber(0);
			const int egmGameDenom(15);

			const DWORD subGameNumber(6);
			const DWORD subGameDenom(egmGameDenom * 5);

			const bool isEgmGameExpected(false);
			const DWORD gameNumberExpected(subGameNumber);
			const int denomExpected(subGameDenom);

			// Setup.
			CGames games;
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetEGMIndicator(true);
			egmGame->SetGameNumber(egmGameNumber);
			egmGame->SetDenom(egmGameDenom);
			games.AddGame(egmGame);

			CGame *subGame(new CGame(MEMORY_NONE));
			subGame->SetGameNumber(subGameNumber);
			subGame->SetDenom(subGameDenom);
			games.AddGame(subGame);

			// Perform operation(s) and test(s).
			unique_ptr<CGame> currentGame(games.GetAdjustedCurrentGameCopy());
			ASSERT_EQ(isEgmGameExpected, currentGame->IsEGM()) << "IsEGM()";
			ASSERT_EQ(gameNumberExpected, currentGame->GetGameNumber()) << "GetGameNumber()";
			ASSERT_EQ(denomExpected, currentGame->GetDenomMultiplier()) << "GetDenomMultiplier()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetAdjustedCurrentGameCopy()
		/// Test when there is an EGM game and an enabled sub game and the EGM game is the current game.
		/// </summary>
		TEST_F(GamesTest, GetAdjustedCurrentGameCopy_EnabledSubGame_EgmGameCurrent)
		{
			// Data.
			DWORD egmGameNumber(0);
			const int egmGameDenom(15);

			const DWORD subGameNumber(6);
			const DWORD subGameDenom(egmGameDenom * 5);

			const bool isEgmGameExpected(false);
			const DWORD gameNumberExpected(subGameNumber);
			const int denomExpected(subGameDenom);

			// Setup.
			CGames games;
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetEGMIndicator(true);
			egmGame->SetGameNumber(egmGameNumber);
			egmGame->SetDenom(egmGameDenom);
			egmGame->ChangeCurrentGameStatus(true);
			games.AddGame(egmGame);

			CGame *subGame(new CGame(MEMORY_NONE));
			subGame->SetGameNumber(subGameNumber);
			subGame->SetDenom(subGameDenom);
			games.AddGame(subGame);

			// Perform operation(s) and test(s).
			unique_ptr<CGame> currentGame(games.GetAdjustedCurrentGameCopy());
			ASSERT_EQ(isEgmGameExpected, currentGame->IsEGM()) << "IsEGM()";
			ASSERT_EQ(gameNumberExpected, currentGame->GetGameNumber()) << "GetGameNumber()";
			ASSERT_EQ(denomExpected, currentGame->GetDenomMultiplier()) << "GetDenomMultiplier()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetAdjustedCurrentGameCopy()
		/// Test when there is an EGM game and an enabled sub game and the sub-game is the current game.
		/// </summary>
		TEST_F(GamesTest, GetAdjustedCurrentGameCopy_EnabledSubGame_SubGameCurrent)
		{
			// Data.
			DWORD egmGameNumber(0);
			const int egmGameDenom(15);

			const DWORD subGameNumber(6);
			const DWORD subGameDenom(egmGameDenom * 5);

			const bool isEgmGameExpected(false);
			const DWORD gameNumberExpected(subGameNumber);
			const int denomExpected(subGameDenom);

			// Setup.
			CGames games;
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetEGMIndicator(true);
			egmGame->SetGameNumber(egmGameNumber);
			egmGame->SetDenom(egmGameDenom);
			games.AddGame(egmGame);

			CGame *subGame(new CGame(MEMORY_NONE));
			subGame->SetGameNumber(subGameNumber);
			subGame->SetDenom(subGameDenom);
			subGame->ChangeCurrentGameStatus(true);
			games.AddGame(subGame);

			// Perform operation(s) and test(s).
			unique_ptr<CGame> currentGame(games.GetAdjustedCurrentGameCopy());
			ASSERT_EQ(isEgmGameExpected, currentGame->IsEGM()) << "IsEGM()";
			ASSERT_EQ(gameNumberExpected, currentGame->GetGameNumber()) << "GetGameNumber()";
			ASSERT_EQ(denomExpected, currentGame->GetDenomMultiplier()) << "GetDenomMultiplier()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetAdjustedCurrentGameCopy()
		/// Test when there is an EGM game and two enabled sub-games where none of the games is the current game.
		/// </summary>
		TEST_F(GamesTest, GetAdjustedCurrentGameCopy_TwoEnabledSubGames_NoCurrent)
		{
			// Data.
			DWORD egmGameNumber(0);
			const int egmGameDenom(15);

			const DWORD subGame1Number(6);
			const DWORD subGame1Denom(egmGameDenom * 5);
			const DWORD subGame2Number(7);
			const DWORD subGame2Denom(egmGameDenom * 25);

			const bool isEgmGameExpected(true);
			const DWORD gameNumberExpected(egmGameNumber);
			const int denomExpected(egmGameDenom);

			// Setup.
			CGames games;
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetEGMIndicator(true);
			egmGame->SetGameNumber(egmGameNumber);
			egmGame->SetDenom(egmGameDenom);
			games.AddGame(egmGame);

			CGame *subGame1(new CGame(MEMORY_NONE));
			subGame1->SetGameNumber(subGame1Number);
			subGame1->SetDenom(subGame1Denom);
			games.AddGame(subGame1);

			CGame *subGame2(new CGame(MEMORY_NONE));
			subGame2->SetGameNumber(subGame2Number);
			subGame2->SetDenom(subGame2Denom);
			games.AddGame(subGame2);

			// Perform operation(s) and test(s).
			unique_ptr<CGame> currentGame(games.GetAdjustedCurrentGameCopy());
			ASSERT_EQ(isEgmGameExpected, currentGame->IsEGM()) << "IsEGM()";
			ASSERT_EQ(gameNumberExpected, currentGame->GetGameNumber()) << "GetGameNumber()";
			ASSERT_EQ(denomExpected, currentGame->GetDenomMultiplier()) << "GetDenomMultiplier()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetAdjustedCurrentGameCopy()
		/// Test when there is an EGM game and two enabled sub-games and the EGM game is the current game.
		/// </summary>
		TEST_F(GamesTest, GetAdjustedCurrentGameCopy_TwoEnabledSubGame_EgmGameCurrent)
		{
			// Data.
			DWORD egmGameNumber(0);
			const int egmGameDenom(15);

			const DWORD subGame1Number(6);
			const DWORD subGame1Denom(egmGameDenom * 5);
			const DWORD subGame2Number(7);
			const DWORD subGame2Denom(egmGameDenom * 25);

			const bool isEgmGameExpected(true);
			const DWORD gameNumberExpected(egmGameNumber);
			const int denomExpected(egmGameDenom);

			// Setup.
			CGames games;
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetEGMIndicator(true);
			egmGame->SetGameNumber(egmGameNumber);
			egmGame->SetDenom(egmGameDenom);
			egmGame->ChangeCurrentGameStatus(true);
			games.AddGame(egmGame);

			CGame *subGame1(new CGame(MEMORY_NONE));
			subGame1->SetGameNumber(subGame1Number);
			subGame1->SetDenom(subGame1Denom);
			games.AddGame(subGame1);

			CGame *subGame2(new CGame(MEMORY_NONE));
			subGame2->SetGameNumber(subGame2Number);
			subGame2->SetDenom(subGame2Denom);
			games.AddGame(subGame2);

			// Perform operation(s) and test(s).
			unique_ptr<CGame> currentGame(games.GetAdjustedCurrentGameCopy());
			ASSERT_EQ(isEgmGameExpected, currentGame->IsEGM()) << "IsEGM()";
			ASSERT_EQ(gameNumberExpected, currentGame->GetGameNumber()) << "GetGameNumber()";
			ASSERT_EQ(denomExpected, currentGame->GetDenomMultiplier()) << "GetDenomMultiplier()";
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetAdjustedCurrentGameCopy()
		/// Test when there is an EGM game and two enabled sub-games and the second sub-game is the current game.
		/// </summary>
		TEST_F(GamesTest, GetAdjustedCurrentGameCopy_TwoEnabledSubGame_SecondSubGameCurrent)
		{
			// Data.
			DWORD egmGameNumber(0);
			const int egmGameDenom(15);

			const DWORD subGame1Number(6);
			const DWORD subGame1Denom(egmGameDenom * 5);
			const DWORD subGame2Number(7);
			const DWORD subGame2Denom(egmGameDenom * 25);

			const bool isEgmGameExpected(false);
			const DWORD gameNumberExpected(subGame2Number);
			const int denomExpected(subGame2Denom);

			// Setup.
			CGames games;
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetEGMIndicator(true);
			egmGame->SetGameNumber(egmGameNumber);
			egmGame->SetDenom(egmGameDenom);
			games.AddGame(egmGame);

			CGame *subGame1(new CGame(MEMORY_NONE));
			subGame1->SetGameNumber(subGame1Number);
			subGame1->SetDenom(subGame1Denom);
			games.AddGame(subGame1);

			CGame *subGame2(new CGame(MEMORY_NONE));
			subGame2->SetGameNumber(subGame2Number);
			subGame2->SetDenom(subGame2Denom);
			subGame2->ChangeCurrentGameStatus(true);
			games.AddGame(subGame2);

			// Perform operation(s) and test(s).
			unique_ptr<CGame> currentGame(games.GetAdjustedCurrentGameCopy());
			ASSERT_EQ(isEgmGameExpected, currentGame->IsEGM()) << "IsEGM()";
			ASSERT_EQ(gameNumberExpected, currentGame->GetGameNumber()) << "GetGameNumber()";
			ASSERT_EQ(denomExpected, currentGame->GetDenomMultiplier()) << "GetDenomMultiplier()";
		}
// #pragma endregion GetAdjustedCurrentGameCopy

// #pragma region GetCurrentSessionGame

		/// Functional test for:
		///     CGames::SetCurrentSessionGame()
		/// Test when there is no prior current game and the new current game is not present.
		/// </summary>
		TEST_F(GamesTest, SetCurrentSessionGame_NeitherPresent)
		{
			// Setup.
			CGames games;

			// Perform operation(s) and test(s).
			games.SetCurrentGame(3);
			ASSERT_TRUE(games.GetCurrentSessionGame() == NULL);
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetCurrentSessionGame()
		/// Test when there is no current game.
		/// </summary>
		TEST_F(GamesTest, GetCurrentSessionGame_NotPresent)
		{
			// Setup.
			CGames games;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(games.GetCurrentSessionGame() == NULL);
		}

		/// <summary>
		/// Functional test for:
		///     CGames::GetCurrentSessionGame()
		/// Test when there is only single game present.
		/// </summary>
		TEST_F(GamesTest, GetCurrentSessionGame_SingleGame)
		{
			// Data.
			const DWORD gameNumber(3);

			// Setup.
			CGames games;

			CGame *game(new CGame(MEMORY_NONE));
			game->ChangeCurrentGameStatus(true);
			games.AddGame(game);

			// Perform operation(s) and test(s).
			ASSERT_EQ((void *)game, (void *)games.GetCurrentSessionGame());
		}

	/// <summary>
	/// Functional test for:
	/// 	CGames::GetCurrentSessionGame()
	/// Test adding multiple games and expecting the right current game.
	/// </summary>
	TEST_F(GamesTest, GetCurrentSessionGame_MultipleGames)
	{
		// Data.
		const DWORD gameNumberA(0);
		const DWORD gameNumberB(2);

		// Setup.
		CGames games;
		CGame *gameA(new CGame(MEMORY_NONE));
		gameA->SetGameNumber(gameNumberA);
		gameA->ChangeCurrentGameStatus(true);
		games.AddGame(gameA);

		CGame gameB(MEMORY_NONE);
		gameB.SetGameNumber(gameNumberB);
		gameB.ChangeCurrentGameStatus(true);

		// Perform operation(s) and test(s).
		ASSERT_TRUE(games.SetGame(&gameB)) << "Setting game B as current game failed";
		ASSERT_TRUE(games.GetGame(gameNumberB)->IsCurrentGame());

		CGame *currSessionGame = games.GetCurrentSessionGame();
		
		ASSERT_EQ(gameNumberB, currSessionGame->GetGameNumber()) << "Current Session Game number should be gameNumberB(2)" ;
	}

    TEST_F(GamesTest, SendEGMMeters_NonNFDPoller)
    {
        COperatingSystemTest os;

        const DWORD StartingTick = 1000;
		const DWORD maxEgmMetersSendTriggerOffset = 10 * MILLISECONDS_PER_MINUTE;
        const DWORD BeforeSendEGMMetersTick = StartingTick + 60 * MILLISECONDS_PER_MINUTE - 1;
        const DWORD AfterSendEGMMetersTick = StartingTick + 60 * MILLISECONDS_PER_MINUTE + 
			maxEgmMetersSendTriggerOffset + 1;

        os.m_tickCount32 = StartingTick;

		CGames games(MEMORY_NONE, os);
		
		// Set meters send frequency for non-NFD pollers
		games.RefreshMetersSendFrequency(false);

        os.m_tickCount32++;

        ASSERT_FALSE(games.IsTimeToSendMeters(true));
     
        os.m_tickCount32 = BeforeSendEGMMetersTick;   

        ASSERT_FALSE(games.IsTimeToSendMeters(true));

        os.m_tickCount32 = AfterSendEGMMetersTick;

        ASSERT_TRUE(games.IsTimeToSendMeters(true));

        games.UpdateLastMetersSentTime(true);

        ASSERT_FALSE(games.IsTimeToSendMeters(true));

		games.ForceEgmMetersUpdate();

		ASSERT_TRUE(games.IsTimeToSendMeters(true));

		games.UpdateLastMetersSentTime(true);

		ASSERT_FALSE(games.IsTimeToSendMeters(true));
    }

    TEST_F(GamesTest, SendSubgameMeters_NonNFDPoller)
    {
        COperatingSystemTest os;

        const DWORD StartingTick = 1000;
		const DWORD maxSubgameMetersSendTriggerOffset = 10 * MILLISECONDS_PER_MINUTE;
        const DWORD BeforeSendSubgameMetersTick = StartingTick + 60 * MILLISECONDS_PER_MINUTE - 1;
        const DWORD AfterSendSubgameMetersTick = StartingTick + 60 * MILLISECONDS_PER_MINUTE + 
			maxSubgameMetersSendTriggerOffset + 1;

        os.m_tickCount32 = StartingTick;

		CGames games(MEMORY_NONE, os);
		
		// Set meters send frequency for non-NFD pollers
		games.RefreshMetersSendFrequency(false);
		
        os.m_tickCount32++;

        ASSERT_FALSE(games.IsTimeToSendMeters(false));
     
        os.m_tickCount32 = BeforeSendSubgameMetersTick;   

        ASSERT_FALSE(games.IsTimeToSendMeters(false));

        os.m_tickCount32 = AfterSendSubgameMetersTick;

        ASSERT_TRUE(games.IsTimeToSendMeters(false));

        games.UpdateLastMetersSentTime(false);

        ASSERT_FALSE(games.IsTimeToSendMeters(false));
    }
	
    TEST_F(GamesTest, SendEGMMeters_NFDPoller)
    {
        COperatingSystemTest os;

        const DWORD StartingTick = 1000;
		const DWORD maxEgmMetersSendTriggerOffset = MILLISECONDS_PER_MINUTE;
        const DWORD BeforeSendEGMMetersTick = StartingTick + (5 * MILLISECONDS_PER_MINUTE) - 1;
        const DWORD AfterSendEGMMetersTick = StartingTick + (5 * MILLISECONDS_PER_MINUTE) + 
			maxEgmMetersSendTriggerOffset + 1;

        os.m_tickCount32 = StartingTick;

		CGames games(MEMORY_NONE, os);
		
		// Set meters send frequency for NFD pollers
		games.RefreshMetersSendFrequency(true);

        os.m_tickCount32++;

        ASSERT_FALSE(games.IsTimeToSendMeters(true));
     
        os.m_tickCount32 = BeforeSendEGMMetersTick;   

        ASSERT_FALSE(games.IsTimeToSendMeters(true));

        os.m_tickCount32 = AfterSendEGMMetersTick;

        ASSERT_TRUE(games.IsTimeToSendMeters(true));

        games.UpdateLastMetersSentTime(true);

        ASSERT_FALSE(games.IsTimeToSendMeters(true));

		games.ForceEgmMetersUpdate();

		ASSERT_TRUE(games.IsTimeToSendMeters(true));

		games.UpdateLastMetersSentTime(true);

		ASSERT_FALSE(games.IsTimeToSendMeters(true));
    }

    TEST_F(GamesTest, SendSubgameMeters_NFDPoller)
    {
        COperatingSystemTest os;

        const DWORD StartingTick = 1000;
		const DWORD maxSubgameMetersSendTriggerOffset = 10 * MILLISECONDS_PER_MINUTE;
        const DWORD BeforeSendSubgameMetersTick = StartingTick + (0.5 * MILLISECONDS_PER_DAY) - 1;
        const DWORD AfterSendSubgameMetersTick = StartingTick + (0.5 * MILLISECONDS_PER_DAY) + 
			maxSubgameMetersSendTriggerOffset + 1;

        os.m_tickCount32 = StartingTick;

		CGames games(MEMORY_NONE, os);
		
		// Set meters send frequency for NFD pollers
		games.RefreshMetersSendFrequency(true);

        os.m_tickCount32++;

        ASSERT_FALSE(games.IsTimeToSendMeters(false));
     
        os.m_tickCount32 = BeforeSendSubgameMetersTick;   

        ASSERT_FALSE(games.IsTimeToSendMeters(false));

        os.m_tickCount32 = AfterSendSubgameMetersTick;

        ASSERT_TRUE(games.IsTimeToSendMeters(false));

        games.UpdateLastMetersSentTime(false);

        ASSERT_FALSE(games.IsTimeToSendMeters(false));
    }
	
	/// <summary>
	/// Functional test for:
	/// 	CGame::CGame()
	/// Test initial game length and paytable id length.
	/// </summary>	
	TEST_F(GamesTest, InitGameInstance)
    {
    	CGame game(MEMORY_NONE);
		ASSERT_EQ(GAME_NAME_LENGTH - 1, game.GetGamenameLength());
		ASSERT_EQ(PAY_TABLE_ID_LEN - 1, game.GetPaytableIDLength());
	}
	
	/// <summary>
	/// Functional test for:
	/// 	CGame::SetPaytableIDLength()
	/// Test  paytable id length.
	/// </summary>	
	TEST_F(GamesTest, TestPaytableIDLength)
    {
    	CGame game(MEMORY_NONE);
		DWORD actualPaytableIdLength = PAY_TABLE_ID_LEN + 1;
		game.SetPaytableIDLength(actualPaytableIdLength);
		ASSERT_EQ(PAY_TABLE_ID_LEN - 1, game.GetPaytableIDLength());
	}

	
	/// <summary>
	/// Functional test for:
	/// 	CGame::SetPaytableID()
	/// Test longer  paytable id and length.
	/// </summary>	
	TEST_F(GamesTest, TestPaytableID)
    {
    	CGame game(MEMORY_NONE);
		const char* actualPaytableId = "123456789012345678901";
		const char* expectPaytableId = "12345678901234567890";
		DWORD actualPaytableIdLength = PAY_TABLE_ID_LEN + 1;
        DWORD expectedPaytableIDLength = PAY_TABLE_ID_LEN - 1;

		game.SetPaytableID(actualPaytableId);
		ASSERT_EQ(expectedPaytableIDLength, game.GetPaytableIDLength());

		ASSERT_EQ(strncmp(game.GetPaytableID(), expectPaytableId, expectedPaytableIDLength), 0);
	}
	
	/// <summary>
	/// Functional test for:
	/// 	CGame::SetGamenameLength()
	/// Test  gamename length.
	/// </summary>	
	TEST_F(GamesTest, TestGamenameLength)
    {
    	CGame game(MEMORY_NONE);
		DWORD actualGamenameLength = GAME_NAME_LENGTH + 1;
		game.SetPaytableIDLength(actualGamenameLength);
		ASSERT_EQ(GAME_NAME_LENGTH - 1, game.GetGamenameLength());
	}

	
	/// <summary>
	/// Functional test for:
	/// 	CGame::SetGamenameName()
	/// Test longer gamename and length.
	/// </summary>	
	TEST_F(GamesTest, TestGamename)
    {
    	CGame game(MEMORY_NONE);
		DWORD actualGamenameLength = GAME_NAME_LENGTH + 1;
		const char* actualGamename = "123456789012345678901";
		const char* expectGamename = "12345678901234567890";
			
		game.SetGameName(actualGamename);
		ASSERT_EQ(GAME_NAME_LENGTH - 1, game.GetGamenameLength());
		ASSERT_STREQ(expectGamename, game.GetGameName());
	}
	
	/// <summary>
	/// Functional test for:
	/// 	CGames::UpdateLastGamelistTimestamp()
	/// Test adding multiple games and expecting the right current time.
	/// </summary>
	TEST_F(GamesTest, UpdateLastGamelistTimestamp_Test)
	{
		// Data.
		const DWORD gameNumberB(1);
		const DWORD gameNumberC(2);

		// Setup.
		CGames games;
		CGame *gameA(new CGame(MEMORY_NONE));
		gameA->SetGameNumber(EGM_GAME_NUMBER);
		gameA->ChangeCurrentGameStatus(true);
		games.AddGame(gameA);

		CGame *gameB(new CGame(MEMORY_NONE));
		gameB->SetGameNumber(gameNumberB);
		gameB->ChangeCurrentGameStatus(true);
		games.AddGame(gameB);
		
		CGame *gameC(new CGame(MEMORY_NONE));
		gameB->SetGameNumber(gameNumberB);
		games.AddGame(gameC);

		ASSERT_EQ(3, games.GetGameCount());

		games.SetNumberOfImplementedGames(3);

		gameA->SetInfoFetchedFlag(true);
		gameB->SetInfoFetchedFlag(true);
		gameC->SetInfoFetchedFlag(true);
		
		time_t curTimestamp = CUtilities::GetCurrentTime();			
		games.UpdateLastGamelistTimestamp(curTimestamp); //All Games fetched. 
		ASSERT_TRUE(games.IsCompleteGamelistFetched());
		ASSERT_EQ(curTimestamp, games.GetLastGamelistTimestamp());

		games.UpdateLastGamelistTimestamp(curTimestamp);
		ASSERT_TRUE(games.IsCompleteGamelistFetched());
		ASSERT_EQ(curTimestamp, games.GetLastGamelistTimestamp());

		games.ResetIsCompleteGamelistFetched();
		ASSERT_FALSE(games.IsCompleteGamelistFetched());

		games.UpdateLastGamelistTimestamp(curTimestamp);//Update again
		ASSERT_FALSE(games.IsCompleteGamelistFetched());
	}

	/// <summary>
	/// Functional test for:
	/// 	CGames::ResetIsCompleteGamelistFetched()
	/// ResetIsCompleteGamelistFetched after adding multiple games.
	/// </summary>
	TEST_F(GamesTest, ResetGameCountsFetched_Test)
	{
		// Data.
		const DWORD gameNumberB(1);
		const DWORD gameNumberC(2);

		// Setup.
		CGames games;
		CGame *gameA(new CGame(MEMORY_NONE));
		games.AddGame(gameA);

		CGame *gameB(new CGame(MEMORY_NONE));
		games.AddGame(gameB);
		
		CGame *gameC(new CGame(MEMORY_NONE));
		games.AddGame(gameC);

		ASSERT_EQ(3, games.GetGameCount());
		
		games.SetNumberOfImplementedGames(3);

		gameA->SetInfoFetchedFlag(true);
		gameB->SetInfoFetchedFlag(true);
		gameC->SetInfoFetchedFlag(true);

		time_t curTimestamp = CUtilities::GetCurrentTime();
		games.UpdateLastGamelistTimestamp(curTimestamp); //pass games fetched count as one
		ASSERT_TRUE(games.IsCompleteGamelistFetched());
			
		games.ResetIsCompleteGamelistFetched();
		ASSERT_FALSE(games.IsCompleteGamelistFetched());

	}	

	TEST_F(GamesTest, SyncTheEnabledGamesTest)
	{
		CGames *games = new CGames(MEMORY_NONE);
		ASSERT_TRUE(games->GetGameCount() == 0) << "Should be no games after constructor";

		CGame *game1 = new CGame(MEMORY_NONE, true);
		game1->SetGameNumber(1);
		game1->Enable();
		CGame *game2 = new CGame(MEMORY_NONE, false);
		game2->SetGameNumber(2);
		game2->Enable();
		CGame *game3 = new CGame(MEMORY_NONE, false);
		game3->SetGameNumber(3);
		game3->Enable();
		CGame *game4 = new CGame(MEMORY_NONE, false);
		game4->SetGameNumber(4);
		game4->Enable();
		
		games->AddGame(game1);
		games->AddGame(game2);
		games->AddGame(game3);
		games->AddGame(game4);

		ASSERT_TRUE(games->GetGameCount() == 4) << "Should be 4 games after set";
		ASSERT_TRUE(games->GetGame(1)->GetGameNumber() == game1->GetGameNumber());
		ASSERT_TRUE(games->GetGame(2)->GetGameNumber() == game2->GetGameNumber());
		ASSERT_TRUE(games->GetGame(3)->GetGameNumber() == game3->GetGameNumber());
		ASSERT_TRUE(games->GetGame(4)->GetGameNumber() == game4->GetGameNumber());

		vector<DWORD> vectorOfEnabledGames{1, 2, 4};
		games->SyncTheEnabledGames(&vectorOfEnabledGames);//Game number 3 should be deleted

		ASSERT_TRUE(games->GetGame(1)->GetGameNumber() == game1->GetGameNumber());
		ASSERT_TRUE(games->GetGame(2)->GetGameNumber() == game2->GetGameNumber());
		ASSERT_TRUE(games->GetGame(3) == nullptr);
		ASSERT_TRUE(games->GetGame(4)->GetGameNumber() == game4->GetGameNumber());

		delete games;
	}

	TEST_F(GamesTest, GetBonusPaymentMethodTest)
	{
		CGames *games = new CGames(MEMORY_NONE);
		ASSERT_TRUE("None" == games->GetBonusPaymentMethod());
		games->SetAftBonusEnabled(true);
		ASSERT_TRUE("SAS_AFT_BONUS" == games->GetBonusPaymentMethod());
		games->SetAftBonusEnabled(false);
		games->SetLegacyBonusEnabled(true);
		ASSERT_TRUE("SAS_LEGACY_BONUS" == games->GetBonusPaymentMethod());
		delete games;
	}

	TEST_F(GamesTest, IsDenomValidTest)
	{
		ASSERT_FALSE(CGames::IsDenomValid(69));
		WORD arrayOfValidDenom[] = {
			1,
			5,
			10,
			25,
			50,
			100,
			500,
			1000,
			2000,
			10000,
			20,
			200,
			250,
			2500,
			5000,
			20000,
			25000,
			50000,
			2,
			3,
			15,
			40};
		for (WORD denom : arrayOfValidDenom)
		{
			ASSERT_TRUE(CGames::IsDenomValid(denom));
		}		
	}
}

