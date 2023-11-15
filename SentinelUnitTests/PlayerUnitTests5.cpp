#include "stdafx.h"
#include "Player.h"

#include "MultiplierBonusEvent.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "UnitTestingUtils.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	static const time_t TIME_DEFAULT_VALUE(0); // Arbitrary value when any valid value is okay.
	static const byte MEMSET_DEFAULT_VALUE = 0xcd; // Value unlikely to be used for other purposes.

	class Player5 : public ::testing::Test
	{
	};

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCardInCompSingleLineString()
		/// Test with:
		///     m_showCardInCompDisplay true -- AKA config.DisplayComps() true
		///     m_bDisplayName true -- AKA config.DisplayPlayerName() true
		///     m_playerInfo != NULL
		/// </summary>
		TEST(Player5, GetCardInCompSingleLineString0Test)
		{
			// Test data -- inputs.
			const DWORD compVal(123);
			const LPCTSTR name(_T("Dee"));

			// Test data -- expected results.
			const string cardInCompSingleLineStringExpected(_T("comp"));
			const string cardInCompSingleLineArgsExpected(_T("$1.23"));

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayComps(true);
			varConfig.SetDisplayPlayerName(true);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetCompValue(compVal);
			playerInfo->SetName(name);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string cardInCompSingleLineStringResult;
			vector<string> cardInCompSingleLineArgsResult;
			player.GetCardInCompSingleLineString(cardInCompSingleLineStringResult, cardInCompSingleLineArgsResult);

			// Test #1.
			{
				bool areEqual(cardInCompSingleLineStringResult == cardInCompSingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardInCompSingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cardInCompSingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(cardInCompSingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(cardInCompSingleLineArgsResult.size(), 1) << "Argument count is invalid";

				ASSERT_EQ(cardInCompSingleLineArgsResult[0], cardInCompSingleLineArgsExpected) << "Wrong argument";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCardInCompSingleLineString()
		/// Test with:
		///     m_showCardInCompDisplay true -- AKA config.DisplayComps() true
		///     m_bDisplayName true -- AKA config.DisplayPlayerName() true
		///     m_playerInfo == NULL
		/// </summary>
		TEST(Player5, GetCardInCompSingleLineString1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string cardInCompSingleLineStringExpected(_T("comp"));
			const string cardInCompSingleLineArgsExpected(_T("$0.00"));

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayComps(true);
			varConfig.SetDisplayPlayerName(true);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = NULL;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string cardInCompSingleLineStringResult;
			vector<string> cardInCompSingleLineArgsResult;
			player.GetCardInCompSingleLineString(cardInCompSingleLineStringResult, cardInCompSingleLineArgsResult);

			// Test #1.
			{
				bool areEqual(cardInCompSingleLineStringResult == cardInCompSingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardInCompSingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cardInCompSingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(cardInCompSingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(cardInCompSingleLineArgsResult.size(), 1) << "Argument count is invalid";

				ASSERT_EQ(cardInCompSingleLineArgsResult[0], cardInCompSingleLineArgsExpected) << "Wrong argument";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCardInCompSingleLineString()
		/// Test with:
		///     m_showCardInCompDisplay true -- AKA config.DisplayComps() true
		///     m_bDisplayName false -- AKA config.DisplayPlayerName() false
		///     m_playerInfo != NULL
		/// </summary>
		TEST(Player5, GetCardInCompSingleLineString2Test)
		{
			// Test data -- inputs.
			const DWORD compVal(123);
			const LPCTSTR name(_T("Dee"));

			// Test data -- expected results.
			const string cardInCompSingleLineStringExpected(_T("comp"));
			const string cardInCompSingleLineArgsExpected(_T("$1.23"));

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayComps(true);
			varConfig.SetDisplayPlayerName(false);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetCompValue(compVal);
			playerInfo->SetName(name);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string cardInCompSingleLineStringResult;
			vector<string> cardInCompSingleLineArgsResult;
			player.GetCardInCompSingleLineString(cardInCompSingleLineStringResult, cardInCompSingleLineArgsResult);

			// Test #1.
			{
				bool areEqual(cardInCompSingleLineStringResult == cardInCompSingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardInCompSingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cardInCompSingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(cardInCompSingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(cardInCompSingleLineArgsResult.size(), 1) << "Argument count is invalid";

				ASSERT_EQ(cardInCompSingleLineArgsResult[0], cardInCompSingleLineArgsExpected) << "Wrong argument";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCardInCompSingleLineString()
		/// Test with:
		///     m_showCardInCompDisplay true -- AKA config.DisplayComps() true
		///     m_bDisplayName false -- AKA config.DisplayPlayerName() false
		///     m_playerInfo == NULL
		/// </summary>
		TEST(Player5, GetCardInCompSingleLineString3Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string cardInCompSingleLineStringExpected(_T("comp"));
			const string cardInCompSingleLineArgsExpected(_T("$0.00"));

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayComps(true);
			varConfig.SetDisplayPlayerName(false);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = NULL;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string cardInCompSingleLineStringResult;
			vector<string> cardInCompSingleLineArgsResult;
			player.GetCardInCompSingleLineString(cardInCompSingleLineStringResult, cardInCompSingleLineArgsResult);

			// Test #1.
			{
				bool areEqual(cardInCompSingleLineStringResult == cardInCompSingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardInCompSingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cardInCompSingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(cardInCompSingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(cardInCompSingleLineArgsResult.size(), 1) << "Argument count is invalid";

				ASSERT_EQ(cardInCompSingleLineArgsResult[0], cardInCompSingleLineArgsExpected) << "Wrong argument";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCardInCompSingleLineString()
		/// Test with:
		///     m_showCardInCompDisplay false -- AKA config.DisplayComps() false
		/// </summary>
		TEST(Player5, GetCardInCompSingleLineString4Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string cardInCompSingleLineStringExpected;

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayComps(false);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = NULL;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string cardInCompSingleLineStringResult;
			vector<string> cardInCompSingleLineArgsResult;
			player.GetCardInCompSingleLineString(cardInCompSingleLineStringResult, cardInCompSingleLineArgsResult);

			// Test #1.
			{
				bool areEqual(cardInCompSingleLineStringResult == cardInCompSingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardInCompSingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cardInCompSingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(cardInCompSingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(cardInCompSingleLineArgsResult.size(), 0) << "Argument count is invalid";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCardInPointsSingleLineString()
		/// Test with:
		///     m_cardInPointsDisplayType == CIDT_TOTALCASH -- AKA config.DisplayPtsTotalAsCash() true
		///     m_bDisplayName true -- AKA config.DisplayPlayerName() true
		/// </summary>
		TEST(Player5, GetCardInPointsSingleLineString0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string cardInPointsSingleLineStringExpected(_T("points_as_cash"));
			const string cardInPointsSingleLineArgsExpected(_T("$0.00"));

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(true);
			varConfig.SetDisplayPlayerName(true);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = NULL;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string cardInPointsSingleLineStringResult;
			vector<string> cardInPointsSingleLineArgsResult;
			player.GetCardInPointsSingleLineString(cardInPointsSingleLineStringResult, cardInPointsSingleLineArgsResult);

			// Test #1.
			{
				bool areEqual(cardInPointsSingleLineStringResult == cardInPointsSingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardInPointsSingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(cardInPointsSingleLineArgsResult.size(), 1) << "Argument count is invalid";

				ASSERT_EQ(cardInPointsSingleLineArgsResult[0], cardInPointsSingleLineArgsExpected) << "Wrong argument";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCardInPointsSingleLineString()
		/// Test with:
		///     m_cardInPointsDisplayType == CIDT_TOTALCASH -- AKA config.DisplayPtsTotalAsCash() true
		///     m_universeEnabled false && m_showPointsVsAccountBalance false
		///     m_bDisplayName false -- AKA config.DisplayPlayerName() false
		/// </summary>
		TEST(Player5, GetCardInPointsSingleLineString_CashTrueUniverseFalseTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string cardInPointsSingleLineStringExpected(_T("points_as_cash"));
			const string cardInPointsSingleLineArgsExpected(_T("$0.00"));

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(true);
			varConfig.SetDisplayPlayerName(false);
			varConfig.SetUniverseEnabled(false);
			varConfig.SetShowPointsVsAccountBalance(false);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = NULL;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string cardInPointsSingleLineStringResult;
			vector<string> cardInPointsSingleLineArgsResult;
			player.GetCardInPointsSingleLineString(cardInPointsSingleLineStringResult, cardInPointsSingleLineArgsResult);

			// Test #1.
			{
				bool areEqual(cardInPointsSingleLineStringResult == cardInPointsSingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardInPointsSingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(cardInPointsSingleLineArgsResult.size(), 1) << "Argument count is invalid";

				ASSERT_EQ(cardInPointsSingleLineArgsResult[0], cardInPointsSingleLineArgsExpected) << "Wrong argument";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCardInPointsSingleLineString()
		/// Test with:
		///     m_cardInPointsDisplayType == CIDT_NONE -- AKA config.DisplayPtsTotalAsCash() false && config.DisplayNumericPointTotal() false
		///     m_universeEnabled true && m_showPointsVsAccountBalance true
		///     m_bDisplayName false -- AKA config.DisplayPlayerName() false
		/// </summary>
		TEST(Player5, GetCardInPointsSingleLineString_PointsFalseUniverseTrueTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string cardInPointsSingleLineStringExpected;

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(false);
			varConfig.SetDisplayNumericPointTotal(false);
			varConfig.SetDisplayPlayerName(false);
			varConfig.SetUniverseEnabled(true);
			varConfig.SetShowPointsVsAccountBalance(true);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = NULL;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string cardInPointsSingleLineStringResult;
			vector<string> args;
			player.GetCardInPointsSingleLineString(cardInPointsSingleLineStringResult, args);

			// Test #1.
			{
				bool areEqual(cardInPointsSingleLineStringResult == cardInPointsSingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardInPointsSingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCardInPointsSingleLineString()
		/// Test with:
		///     m_cardInPointsDisplayType == CIDT_TOTAL -- AKA config.DisplayPtsTotalAsCash() false && config.DisplayNumericPointTotal() true
		///     m_universeEnabled true && m_showPointsVsAccountBalance true
		///     m_bDisplayName false -- AKA config.DisplayPlayerName() false
		/// </summary>
		TEST(Player5, GetCardInPointsSingleLineString_CashFalseUniverseTrueTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string cardInPointsSingleLineStringExpected(_T("points_as_cash"));
			const string cardInPointsSingleLineArgsExpected(_T("$0.00"));

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(false);
			varConfig.SetDisplayNumericPointTotal(true);
			varConfig.SetDisplayPlayerName(false);
			varConfig.SetUniverseEnabled(true);
			varConfig.SetShowPointsVsAccountBalance(true);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = NULL;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string cardInPointsSingleLineStringResult;
			vector<string> cardInPointsSingleLineArgsResult;
			player.GetCardInPointsSingleLineString(cardInPointsSingleLineStringResult, cardInPointsSingleLineArgsResult);

			// Test #1.
			{
				bool areEqual(cardInPointsSingleLineStringResult == cardInPointsSingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardInPointsSingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(cardInPointsSingleLineArgsResult.size(), 1) << "Argument count is invalid";

				ASSERT_EQ(cardInPointsSingleLineArgsResult[0], cardInPointsSingleLineArgsExpected) << "Wrong argument";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCardInPointsSingleLineString()
		/// Test with:
		///     m_cardInPointsDisplayType == CIDT_TOTALCASH -- AKA config.DisplayPtsTotalAsCash() true
		///     m_universeEnabled true && m_showPointsVsAccountBalance true
		///     m_bDisplayName false -- AKA config.DisplayPlayerName() false
		/// </summary>
		TEST(Player5, GetCardInPointsSingleLineString_CashTrueUniverseTrueTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string cardInPointsSingleLineStringExpected(_T("points_as_cash"));
			const string cardInPointsSingleLineArgsExpected(_T("$0.00"));

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(true);
			varConfig.SetDisplayPlayerName(false);
			varConfig.SetUniverseEnabled(true);
			varConfig.SetShowPointsVsAccountBalance(true);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = NULL;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string cardInPointsSingleLineStringResult;
			vector<string> cardInPointsSingleLineArgsResult;
			player.GetCardInPointsSingleLineString(cardInPointsSingleLineStringResult, cardInPointsSingleLineArgsResult);

			// Test #1.
			{
				bool areEqual(cardInPointsSingleLineStringResult == cardInPointsSingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardInPointsSingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(cardInPointsSingleLineArgsResult.size(), 1) << "Argument count is invalid";

				ASSERT_EQ(cardInPointsSingleLineArgsResult[0], cardInPointsSingleLineArgsExpected) << "Wrong argument";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCardInPointsSingleLineString()
		/// Test with:
		///     m_cardInPointsDisplayType == CIDT_TOTAL -- AKA config.DisplayPtsTotalAsCash() false && config.DisplayNumericPointTotal() true
		///     m_bDisplayName true -- AKA config.DisplayPlayerName() true
		/// </summary>
		TEST(Player5, GetCardInPointsSingleLineString2Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string cardInPointsSingleLineStringExpected(_T("points"));
			const string cardInPointsSingleLineArgsExpected(_T("0"));

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(false);
			varConfig.SetDisplayNumericPointTotal(true);
			varConfig.SetDisplayPlayerName(true);
			varConfig.SetUniverseEnabled(false);
			varConfig.SetShowPointsVsAccountBalance(false);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = NULL;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string cardInPointsSingleLineStringResult;
			vector<string> cardInPointsSingleLineArgsResult;
			player.GetCardInPointsSingleLineString(cardInPointsSingleLineStringResult, cardInPointsSingleLineArgsResult);

			// Test #1.
			{
				bool areEqual(cardInPointsSingleLineStringResult == cardInPointsSingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardInPointsSingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(cardInPointsSingleLineArgsResult.size(), 1) << "Argument count is invalid";

				ASSERT_EQ(cardInPointsSingleLineArgsResult[0], cardInPointsSingleLineArgsExpected) << "Wrong argument";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCardInPointsSingleLineString()
		/// Test with:
		///     m_cardInPointsDisplayType == CIDT_TOTAL -- AKA config.DisplayPtsTotalAsCash() false && config.DisplayNumericPointTotal() true
		///     m_bDisplayName false -- AKA config.DisplayPlayerName() false
		/// </summary>
		TEST(Player5, GetCardInPointsSingleLineString3Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string cardInPointsSingleLineStringExpected(_T("points"));
			const string cardInPointsSingleLineArgsExpected(_T("0"));

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(false);
			varConfig.SetDisplayNumericPointTotal(true);
			varConfig.SetDisplayPlayerName(false);
			varConfig.SetUniverseEnabled(false);
			varConfig.SetShowPointsVsAccountBalance(false);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = NULL;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string cardInPointsSingleLineStringResult;
			vector<string> cardInPointsSingleLineArgsResult;
			player.GetCardInPointsSingleLineString(cardInPointsSingleLineStringResult, cardInPointsSingleLineArgsResult);

			// Test #1.
			{
				bool areEqual(cardInPointsSingleLineStringResult == cardInPointsSingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardInPointsSingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(cardInPointsSingleLineArgsResult.size(), 1) << "Argument count is invalid";

				ASSERT_EQ(cardInPointsSingleLineArgsResult[0], cardInPointsSingleLineArgsExpected) << "Wrong argument";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCardInPointsSingleLineString()
		/// Test with:
		///     m_cardInPointsDisplayType == CIDT_NONE -- AKA config.DisplayPtsTotalAsCash() false && config.DisplayNumericPointTotal() false
		/// </summary>
		TEST(Player5, GetCardInPointsSingleLineString4Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string cardInPointsSingleLineStringExpected;

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(false);
			varConfig.SetDisplayNumericPointTotal(false);
			varConfig.SetUniverseEnabled(false);
			varConfig.SetShowPointsVsAccountBalance(false);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string cardInPointsSingleLineStringResult;
			vector<string> args;
			player.GetCardInPointsSingleLineString(cardInPointsSingleLineStringResult, args);

			// Test #1.
			{
				bool areEqual(cardInPointsSingleLineStringResult == cardInPointsSingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardInPointsSingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(cardInPointsSingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::DisplayPointsOnBalanceScreen()
		/// Test with:
		///     m_cardInPointsDisplayType == CIDT_NONE -- AKA config.DisplayPtsTotalAsCash() false && config.DisplayNumericPointTotal() false
		/// </summary>
		TEST(Player5, DisplayPointsOnBalanceScreen0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool displayPointsOnBalanceScreenExpected(false);

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(false);
			varConfig.SetDisplayNumericPointTotal(false);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const bool displayPointsOnBalanceScreenResult = player.DisplayPointsOnBalanceScreen();

			// Test #1.
			{
				bool areEqual(displayPointsOnBalanceScreenResult == displayPointsOnBalanceScreenExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED displayPointsOnBalanceScreenResult=%d expected=%d in %s"),
					(int)displayPointsOnBalanceScreenResult,
					(int)displayPointsOnBalanceScreenExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::DisplayPointsOnBalanceScreen()
		/// Test with:
		///     m_cardInPointsDisplayType == CIDT_TOTALCASH -- AKA config.DisplayPtsTotalAsCash() true
		/// </summary>
		TEST(Player5, DisplayPointsOnBalanceScreen1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool displayPointsOnBalanceScreenExpected(true);

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(true);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const bool displayPointsOnBalanceScreenResult = player.DisplayPointsOnBalanceScreen();

			// Test #1.
			{
				bool areEqual(displayPointsOnBalanceScreenResult == displayPointsOnBalanceScreenExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED displayPointsOnBalanceScreenResult=%d expected=%d in %s"),
					(int)displayPointsOnBalanceScreenResult,
					(int)displayPointsOnBalanceScreenExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::DisplayAccountBalanceButton()
		/// Test with:
		///     m_bIsVirtual == true
		/// </summary>
		TEST(Player5, DisplayAccountBalanceButton0Test)
		{
			// Test data -- inputs.
			const time_t removalTime(123);
			const time_t utcRemovalTime(12345);
			const bool bMakeVirtual(true);
			CMeters * virtualSessionStartMeters = NULL;

			// Test data -- expected results.
			const bool displayAccountBalanceButtonExpected(false);

			// Setup.
			CPlayer varPlayer;
			varPlayer.CardRemoved(removalTime, utcRemovalTime, bMakeVirtual);
			CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const bool displayAccountBalanceButtonResult = player.DisplayAccountBalanceButton();

			// Test #1.
			{
				bool areEqual(displayAccountBalanceButtonResult == displayAccountBalanceButtonExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED displayAccountBalanceButtonResult=%d expected=%d in %s"),
					(int)displayAccountBalanceButtonResult,
					(int)displayAccountBalanceButtonExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::DisplayAccountBalanceButton()
		/// Test with:
		///     m_bIsVirtual == false
		///     m_cardInPointsDisplayType == CIDT_NONE -- AKA config.DisplayPtsTotalAsCash() false && config.DisplayNumericPointTotal() false
		///     config.DisplayComps() == false
		/// </summary>
		TEST(Player5, DisplayAccountBalanceButton1Test)
		{
			// Test data -- inputs.
			// Test data -- expected results.
			const bool displayAccountBalanceButtonExpected(false);

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(false);
			varConfig.SetDisplayNumericPointTotal(false);
			varConfig.SetDisplayComps(false);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			varPlayer.VirtualSessionEnd(TIME_DEFAULT_VALUE);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const bool displayAccountBalanceButtonResult = player.DisplayAccountBalanceButton();

			// Test #1.
			{
				bool areEqual(displayAccountBalanceButtonResult == displayAccountBalanceButtonExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED displayAccountBalanceButtonResult=%d expected=%d in %s"),
					(int)displayAccountBalanceButtonResult,
					(int)displayAccountBalanceButtonExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::DisplayAccountBalanceButton()
		/// Test with:
		///     m_bIsVirtual == false
		///     m_cardInPointsDisplayType == CIDT_NONE -- AKA config.DisplayPtsTotalAsCash() false && config.DisplayNumericPointTotal() false
		///     config.DisplayComps() == true
		/// </summary>
		TEST(Player5, DisplayAccountBalanceButton2Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool displayAccountBalanceButtonExpected(true);

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(false);
			varConfig.SetDisplayNumericPointTotal(false);
			varConfig.SetDisplayComps(true);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			varPlayer.VirtualSessionEnd(TIME_DEFAULT_VALUE);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const bool displayAccountBalanceButtonResult = player.DisplayAccountBalanceButton();

			// Test #1.
			{
				bool areEqual(displayAccountBalanceButtonResult == displayAccountBalanceButtonExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED displayAccountBalanceButtonResult=%d expected=%d in %s"),
					(int)displayAccountBalanceButtonResult,
					(int)displayAccountBalanceButtonExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::DisplayAccountBalanceButton()
		/// Test with:
		///     m_bIsVirtual == false
		///     m_cardInPointsDisplayType == CIDT_TOTALCASH -- AKA config.DisplayPtsTotalAsCash() true
		/// </summary>
		TEST(Player5, DisplayAccountBalanceButton3Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool displayAccountBalanceButtonExpected(true);

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(true);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			varPlayer.VirtualSessionEnd(TIME_DEFAULT_VALUE);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const bool displayAccountBalanceButtonResult = player.DisplayAccountBalanceButton();

			// Test #1.
			{
				bool areEqual(displayAccountBalanceButtonResult == displayAccountBalanceButtonExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED displayAccountBalanceButtonResult=%d expected=%d in %s"),
					(int)displayAccountBalanceButtonResult,
					(int)displayAccountBalanceButtonExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::IsGuest()
		/// Test with:
		///     playerInfo == NULL
		/// </summary>
		TEST(Player5, IsGuest0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool isGuestExpected(true);

			// Setup.
			CPlayer varPlayer;

			CPlayerInfo *playerInfo(NULL);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const bool isGuestResult = player.IsGuest();

			// Test #1.
			{
				bool areEqual(isGuestResult == isGuestExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED isGuestResult=%d expected=%d in %s"),
					(int)isGuestResult,
					(int)isGuestExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::IsGuest()
		/// Test with:
		///     m_playerInfo =! NULL
		///     m_playerInfo.GetCardNumber == NULL
		/// </summary>
		TEST(Player5, IsGuest1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool isGuestExpected(true);

			// Setup.
			CPlayer varPlayer;

			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->SetCardNumber(NULL, 0);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const bool isGuestResult = player.IsGuest();

			// Test #1.
			{
				bool areEqual(isGuestResult == isGuestExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED isGuestResult=%d expected=%d in %s"),
					(int)isGuestResult,
					(int)isGuestExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::IsGuest()
		/// Test with:
		///     m_playerInfo =! NULL
		///     m_playerInfo.GetCardNumber != NULL
		///     m_playerInfo->GetName() == "Guest"
		/// </summary>
		TEST(Player5, IsGuest2Test)
		{
			// Test data -- inputs.
			const byte cardNumber[] = { 0 };
			const int cardNumberLen = (int)sizeof(cardNumber);
			LPCTSTR const name = _T("Guest");

			// Test data -- expected results.
			const bool isGuestExpected(true);

			// Setup.
			CPlayer varPlayer;

			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->SetCardNumber(cardNumber, cardNumberLen);
			playerInfo->SetName(name);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const bool isGuestResult = player.IsGuest();

			// Test #1.
			{
				bool areEqual(isGuestResult == isGuestExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED isGuestResult=%d expected=%d in %s"),
					(int)isGuestResult,
					(int)isGuestExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::IsGuest()
		/// Test with:
		///     m_playerInfo =! NULL
		///     m_playerInfo.GetCardNumber != NULL
		///     m_playerInfo->GetName() == "Guests"
		/// </summary>
		TEST(Player5, IsGuest3Test)
		{
			// Test data -- inputs.
			const byte cardNumber[] = { 0 };
			const int cardNumberLen = (int)sizeof(cardNumber);
			LPCTSTR const name = _T("Guests");

			// Test data -- expected results.
			const bool isGuestExpected(false);

			// Setup.
			CPlayer varPlayer;

			CPlayerInfo * playerInfo(new CPlayerInfo);
			playerInfo->SetCardNumber(cardNumber, cardNumberLen);
			playerInfo->SetName(name);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const bool isGuestResult = player.IsGuest();

			// Test #1.
			{
				bool areEqual(isGuestResult == isGuestExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED isGuestResult=%d expected=%d in %s"),
					(int)isGuestResult,
					(int)isGuestExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::IsGuest()
		/// Test with:
		///     m_playerInfo =! NULL
		///     m_playerInfo.GetCardNumber != NULL
		///     m_playerInfo->GetName() == "Guest"
		///     m_universeEnabled = true.
		/// </summary>
		TEST(Player5, IsGuest4Test)
		{
			// Test data -- inputs.
			const byte cardNumber[] = { 0 };
			const int cardNumberLen = (int)sizeof(cardNumber);
			LPCTSTR const name = _T("Guest");

			// Test data -- expected results.
			const bool isGuestExpected(true);

			// Setup.
			CConfig config(true, "1.2.3.4");
			config.SetUniverseEnabled(true);
			CEGMConfig egmConfig(MEMORY_NONE);
			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo * playerInfo(new CPlayerInfo);
			playerInfo->SetCardNumber(cardNumber, cardNumberLen);
			playerInfo->SetName(name);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const bool isGuestResult = player.IsGuest();

			// Test #1.
			{
				bool areEqual(isGuestResult == isGuestExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED isGuestResult=%d expected=%d in %s"),
					(int)isGuestResult,
					(int)isGuestExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::IsGuest()
		/// Test with:
		///     m_playerInfo =! NULL
		///     m_playerInfo.GetCardNumber != NULL
		///     m_playerInfo->GetName() == "Guests"
		///     m_universeEnabled = true.
		/// </summary>
		TEST(Player5, IsGuest5Test)
		{
			// Test data -- inputs.
			const byte cardNumber[] = { 0 };
			const int cardNumberLen = (int)sizeof(cardNumber);
			LPCTSTR const name = _T("Guests");

			// Test data -- expected results.
			const bool isGuestExpected(false);

			// Setup.
			CConfig config(true, "1.2.3.4");
			config.SetUniverseEnabled(true);
			CEGMConfig egmConfig(MEMORY_NONE);
			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo * playerInfo(new CPlayerInfo);
			playerInfo->SetCardNumber(cardNumber, cardNumberLen);
			playerInfo->SetName(name);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const bool isGuestResult = player.IsGuest();

			// Test #1.
			{
				bool areEqual(isGuestResult == isGuestExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED isGuestResult=%d expected=%d in %s"),
					(int)isGuestResult,
					(int)isGuestExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetVirtualFlag()
		///     CPlayer::IsVirtual()
		/// </summary>
		TEST(Player5, SetVirtualFlagTest)
		{
			// Test data -- inputs.
			const bool virtualFlag(true);

			// Test data -- expected results.
			const bool isVirtualExpected(true);

			// Setup.
			CPlayer varPlayer;

			// Perform operation(s) to be tested.
			varPlayer.SetVirtualFlag(virtualFlag);
			const CPlayer &player(varPlayer);
			const bool isVirtualResult(player.IsVirtual());

			// Test #1.
			{
				bool areEqual(isVirtualResult == isVirtualExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED isVirtualResult=%d expected=%d in %s"),
					(int)isVirtualResult,
					(int)isVirtualExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::CompareCard()
		/// Test with:
		///     m_card == NULL
		/// </summary>
		TEST(Player5, CompareCard0Test)
		{
			// Test data -- inputs.
			CCard * const card(NULL);

			// Test data -- expected results.
			const bool compareCardExpected(false);

			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
            CGames *games = new CGames();
			const CPlayer player(card, NULL, config, egmConfig, TIME_DEFAULT_VALUE, false, games);

			// Perform operation(s) to be tested.
			const bool compareCardResult = player.CompareCard(card);

			// Test #1.
			{
				bool areEqual(compareCardResult == compareCardExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED compareCardResult=%d expected=%d in %s"),
					(int)compareCardResult,
					(int)compareCardExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::CompareCard()
		/// Test with:
		///     m_card != NULL
		/// </summary>
		TEST(Player5, CompareCard1Test)
		{
			// Test data -- inputs.
			CCard * const card(new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE));

			// Test data -- expected results.
			const bool compareCardExpected(false);

			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
            CGames *games = new CGames();
			const CPlayer player(card, NULL, config, egmConfig, TIME_DEFAULT_VALUE, false, games);

			// Perform operation(s) to be tested.
			const bool compareCardResult = player.CompareCard(card);

			// Test #1.
			{
				bool areEqual(compareCardResult == compareCardExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED compareCardResult=%d expected=%d in %s"),
					(int)compareCardResult,
					(int)compareCardExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCard()
		/// Test with:
		///     m_card == NULL
		/// </summary>
		TEST(Player5, GetCardTest)
		{
			// Test data -- inputs.
			CCard * const card(NULL);

			// Test data -- expected results.
			const CCard * const cardExpected(card);

			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
            CGames *games = new CGames();
			const CPlayer player(card, NULL, config, egmConfig, TIME_DEFAULT_VALUE, false, games);

			// Perform operation(s) to be tested.
			const CCard * cardResult = player.GetCard();

			// Test #1.
			{
				bool areEqual(cardResult == cardExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=%p expected=%p in %s"),
					(const void *)cardResult,
					(const void *)cardExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetName()
		/// Test with:
		///     m_playerInfo == NULL
		/// </summary>
		TEST(Player5, GetName0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			string const nameExpected(_T(""));

			// Setup.
			CPlayer varPlayer;

			CPlayerInfo * playerInfo(NULL);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string const nameResult = player.GetName();

			// Test #1.
			{
				bool areEqual = nameResult == nameExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(nameResult.c_str()),
					static_cast<LPCTSTR>(nameExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetName()
		/// Test with:
		///     m_playerInfo != NULL
		/// </summary>
		TEST(Player5, GetName1Test)
		{
			// Test data -- inputs.
			LPCTSTR const name(_T("$"));

			// Test data -- expected results.
			string const nameExpected(_T("$"));

			// Setup.
			CPlayer varPlayer;

			CPlayerInfo * playerInfo(new CPlayerInfo);
			playerInfo->SetName(name);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string const nameResult = player.GetName();

			// Test #1.
			{
				bool areEqual = nameResult == nameExpected;
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(nameResult.c_str()),
					static_cast<LPCTSTR>(nameExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetPlayerInfo()
		/// Test with:
		///     m_playerInfo == NULL
		/// </summary>
		TEST(Player5, GetPlayerInfoTest)
		{
			// Test data -- inputs.
			CCard * card(NULL);
			CPlayerInfo *playerInfo(NULL);

			// Test data -- expected results.
			const CPlayerInfo *playerInfoExpected(playerInfo);

			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
            CGames *games = new CGames();
			CPlayer varPlayer(card, NULL, config, egmConfig, TIME_DEFAULT_VALUE, false, games);

			// Perform operation(s) to be tested.
			const CPlayerInfo * playerInfoResult = varPlayer.GetPlayerInfo();

			// Test #1.
			{
				bool areEqual(playerInfoResult == playerInfoExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=%p in %s"),
					(const void *)playerInfoResult,
					(const void *)playerInfoExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetBonusPoints()
		/// Test with:
		///     default m_bonusEvents value == NULL
		/// </summary>
		TEST(Player5, GetBonusPoints0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const DWORD bonusPointsExpected(0);

			// Setup.
			const CPlayer player;

			// Perform operation(s) to be tested.
			const DWORD bonusPointsResult = player.GetBonusPoints();

			// Test #1.
			{
				bool areEqual(bonusPointsResult == bonusPointsExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=%u expected=%u in %s"),
					(unsigned)bonusPointsResult,
					(unsigned)bonusPointsExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetBonusPoints()
		/// Test with:
		///     m_bonusEvents != NULL
		/// </summary>
		TEST(Player5, GetBonusPoints1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const DWORD bonusPointsExpected(0);

			// Setup.
			CPlayer varPlayer;
			const CBonusEvents bonusEvents;
			varPlayer.SetBonusEvents(&bonusEvents);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const DWORD bonusPointsResult = player.GetBonusPoints();

			// Test #1.
			{
				bool areEqual(bonusPointsResult == bonusPointsExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=%u expected=%u in %s"),
					(unsigned)bonusPointsResult,
					(unsigned)bonusPointsExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetVirtualCard()
		/// Test with:
		///     m_virtualCard == NULL
		/// </summary>
		TEST(Player5, GetVirtualCardTest)
		{
			// Test data -- inputs.
			CCard * card(NULL);
			const bool bMakeVirtual(true);
			CMeters * virtualSessionStartMeters(NULL);

			// Test data -- expected results.
			const CCard * virtualCardExpected(NULL);

			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
            CGames *games = new CGames();
			CPlayer varPlayer(card, NULL, config, egmConfig, TIME_DEFAULT_VALUE, false, games);
			varPlayer.CardRemoved( TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE, bMakeVirtual);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const CCard * virtualCardResult = player.GetVirtualCard();

			// Test #1.
			{
				bool areEqual(virtualCardResult == virtualCardExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED virtualCardResult=%p expected=%p in %s"),
					(const void *)virtualCardResult,
					(const void *)virtualCardExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetLastPlayTime()
		/// Test with:
		///     m_virtualCard == NULL
		/// </summary>
		TEST(Player5, GetLastPlayTimeTest)
		{
			// Test data -- inputs.
			const time_t playTime(1234);

			// Test data -- expected results.
			const time_t lastPlayTimeExpected(1234);

			// Setup.
			CPlayer varPlayer;
			varPlayer.SetLastPlayTime(playTime);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const time_t lastPlayTimeResult = player.GetLastPlayTime();

			// Test #1.
			{
				bool areEqual(lastPlayTimeResult == lastPlayTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED lastPlayTimeResult=%lld expected=%lld in %s"),
					(long long)lastPlayTimeResult,
					(long long)lastPlayTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetVirtualStartTime()
		/// Test with:
		///     m_virtualCard == NULL
		/// </summary>
		TEST(Player5, GetVirtualStartTimeTest)
		{
			// Test data -- inputs.
			const time_t utcRemovalTime(7890);
			const bool bMakeVirtual(true);
			CMeters * virtualSessionStartMeters(NULL);

			// Test data -- expected results.
			const time_t virtualStartTimeExpected(7890);

			// Setup.
			CPlayer varPlayer;
			varPlayer.CardRemoved(utcRemovalTime, utcRemovalTime, bMakeVirtual);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const time_t virtualSessionStartResult = player.GetVirtualStartTime();

			// Test #1.
			{
				bool areEqual(virtualSessionStartResult == virtualStartTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED virtualSessionStartResult=%lld expected=%lld in %s"),
					(long long)virtualSessionStartResult,
					(long long)virtualStartTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetCurrentCountdown()
		/// </summary>
		TEST(Player5, GetCurrentCountdownTest)
		{
			// Test data -- inputs.
			const DWORD currentCountdown(369);

			// Test data -- expected results.
			const DWORD currentCountdownExpected(369);

			// Setup.
			CPlayer varPlayer;
			varPlayer.SetCurrentCountdown(currentCountdown);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const DWORD currentCountdownResult = player.GetCurrentCountdown();

			// Test #1.
			{
				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetSessionPoints()
		/// </summary>
		TEST(Player5, GetSessionPointsTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const DWORD sessionPointsExpected(0);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo * playerInfo(new CPlayerInfo);
			varPlayer.SetPlayerInfo(playerInfo);
			varPlayer.ResetSession(TIME_DEFAULT_VALUE);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const DWORD sessionPointsResult = player.GetSessionPoints();

			// Test #1.
			{
				bool areEqual(sessionPointsResult == sessionPointsExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED sessionPointsResult=%u expected=%u in %s"),
					(unsigned)sessionPointsResult,
					(unsigned)sessionPointsExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::HasPointsBeenAdded()
		/// </summary>
		TEST(Player5, HasPointsBeenAddedTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pointsBeenAddedExpected(false);

			// Setup.
			const CPlayer player;

			// Perform operation(s) to be tested.
			const bool pointsBeenAddedResult = player.HasPointsBeenAdded();

			// Test #1.
			{
				bool areEqual(pointsBeenAddedResult == pointsBeenAddedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pointsBeenAddedResult=%d expected=%d in %s"),
					(int)pointsBeenAddedResult,
					(int)pointsBeenAddedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::IsPBEnabled()
		/// Test path(s) with:
		///     m_bPBEnabled == false -- AKA
		///          config.PBTCashEnabled() false
		///          config.PBTPointsEnabled() false
		///          config.PBTPointsAsPromo() false
		///          config.PBTPromoEnabled() false
		///          config.PBFromGameEnabled() false
		/// </summary>
		TEST(Player5, IsPBEnabled0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			varConfig.SetPBTCashEnabled_local(false);
			varConfig.SetPBTPointsEnabled_local(false);
			varConfig.SetPBTPointsAsPromo(false);
			varConfig.SetPBTPromoEnabled_local(false);
			varConfig.SetPBFromGameEnabled(false);
			const CConfig &config(varConfig);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer player(varPlayer);


			// Perform operation(s) to be tested.
			const bool pbEnabledResult = player.IsPBEnabled();

			// Test #1.
			{
				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::IsPBEnabled()
		/// Test path(s) with:
		///     m_bPBEnabled == true -- AKA config.PBTCashEnabled() true
		///     m_playerInfo == NULL
		/// </summary>
		TEST(Player5, IsPBEnabled1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPBTCashEnabled_local(true);
			varConfig.SetPBTCashEnabled_Global(true);
			const CConfig &config(varConfig);

			CPlayerInfo * playerInfo(NULL);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer player(varPlayer);

			// Perform operation(s) to be tested.
			const bool pbEnabledResult = player.IsPBEnabled();

			// Test #1.
			{
				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::IsPBEnabled()
		/// Test path(s) with:
		///     m_bPBEnabled == true -- AKA config.PBTCashEnabled() true
		///     m_playerInfo != NULL
		///     m_playerInfo->IsPBTAllowed() == false
		/// </summary>
		TEST(Player5, IsPBEnabled2Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPBTCashEnabled_local(true);
			varConfig.SetPBTCashEnabled_Global(true);
			const CConfig &config(varConfig);

			CPlayerInfo * playerInfo(new CPlayerInfo);
			playerInfo->AllowPBT(false);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer player(varPlayer);

			// Perform operation(s) to be tested.
			const bool pbEnabledResult = player.IsPBEnabled();

			// Test #1.
			{
				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::IsPBEnabled()
		/// Test path(s) with:
		///     m_bPBEnabled == true -- AKA config.PBTCashEnabled() true
		///     m_playerInfo != NULL
		///     m_playerInfo->IsPBTAllowed() == true
		///     IsVirtual() == true
		/// </summary>
		TEST(Player5, IsPBEnabled3Test)
		{
			// Test data -- inputs.
			const time_t removalTime(TIME_DEFAULT_VALUE);
			const time_t utcRemovalTime(TIME_DEFAULT_VALUE);
			const bool bMakeVirtual(true);
			CMeters * virtualSessionStartMeters(NULL);

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPBTCashEnabled_local(true);
			varConfig.SetPBTCashEnabled_Global(true);
			const CConfig &config(varConfig);

			CPlayerInfo * playerInfo(new CPlayerInfo);
			playerInfo->AllowPBT(true);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			varPlayer.SetPlayerInfo(playerInfo);
			varPlayer.CardRemoved(removalTime, utcRemovalTime, bMakeVirtual);
			const CPlayer player(varPlayer);

			// Perform operation(s) to be tested.
			const bool pbEnabledResult = player.IsPBEnabled();

			// Test #1.
			{
				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::IsPBEnabled()
		/// Test path(s) with:
		///     m_bPBEnabled == true -- AKA config.PBTCashEnabled() true
		///     m_playerInfo != NULL
		///     m_playerInfo->IsPBTAllowed() == true
		///     IsVirtual() == false
		/// </summary>
		TEST(Player5, IsPBEnabled4Test)
		{
			// Test data -- inputs.
			const time_t virtualEndTime(TIME_DEFAULT_VALUE);

			// Test data -- expected results.
			const bool pbEnabledExpected(true);

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPBTCashEnabled_local(true);
			varConfig.SetPBTCashEnabled_Global(true);
			const CConfig &config(varConfig);

			CPlayerInfo * playerInfo(new CPlayerInfo);
			playerInfo->AllowPBT(true);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			varPlayer.SetPlayerInfo(playerInfo);
			varPlayer.VirtualSessionEnd(virtualEndTime);
			const CPlayer player(varPlayer);

			// Perform operation(s) to be tested.
			const bool pbEnabledResult(player.IsPBEnabled());

			// Test #1.
			{
				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::DoPBOnCardIn()
		/// Test path(s) with:
		///     m_bPBOnCardIn == false -- AKAK config.CardInPBT() == false
		/// </summary>
		TEST(Player5, DoPBOnCardIn0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool doPBOnCardInExpected(false);

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetCardInPBT(false);
			const CConfig &config(varConfig);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer player(varPlayer);


			// Perform operation(s) to be tested.
			const bool doPBOnCardInResult(player.DoPBOnCardIn());

			// Test #1.
			{
				bool areEqual(doPBOnCardInResult == doPBOnCardInExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED doPBOnCardInResult=%d expected=%d in %s"),
					(int)doPBOnCardInResult,
					(int)doPBOnCardInExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::DoPBOnCardIn()
		/// Test path(s) with:
		///     m_bPBOnCardIn == true -- AKAK config.CardInPBT() == true
		///     IsPBEnabled() == false -- AKA
		///         m_bPBEnabled == false -- AKA
		///              config.PBTCashEnabled() false
		///              config.PBTPointsEnabled() false
		///              config.PBTPointsAsPromo() false
		///              config.PBTPromoEnabled() false
		///              config.PBFromGameEnabled() false
		/// </summary>
		TEST(Player5, DoPBOnCardIn1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool doPBOnCardInExpected(false);

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetCardInPBT(true);
			varConfig.SetPBTCashEnabled_local(false);
			varConfig.SetPBTPointsEnabled_local(false);
			varConfig.SetPBTPointsAsPromo(false);
			varConfig.SetPBTPromoEnabled_local(false);
			varConfig.SetPBFromGameEnabled(false);
			const CConfig &config(varConfig);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer player(varPlayer);

			// Perform operation(s) to be tested.
			const bool doPBOnCardInResult(player.DoPBOnCardIn());

			// Test #1.
			{
				bool areEqual(doPBOnCardInResult == doPBOnCardInExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED doPBOnCardInResult=%d expected=%d in %s"),
					(int)doPBOnCardInResult,
					(int)doPBOnCardInExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::DoPBOnCardIn()
		/// Test path(s) with:
		///     m_bPBOnCardIn == true -- AKAK config.CardInPBT() == true
		///     IsPBEnabled() == true -- AKA
		///         m_bPBEnabled == true -- AKA config.PBTCashEnabled() true
		///         m_playerInfo != NULL
		///         m_playerInfo->IsPBTAllowed() == true
		///         IsVirtual() == false
		/// </summary>
		TEST(Player5, DoPBOnCardIn2Test)
		{
			// Test data -- inputs.
			const time_t virtualEndTime(TIME_DEFAULT_VALUE);

			// Test data -- expected results.
			const bool doPBOnCardInExpected(true);

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetCardInPBT(true);
			varConfig.SetPBTCashEnabled_local(true);
			varConfig.SetPBTCashEnabled_Global(true);
			const CConfig &config(varConfig);

			CPlayerInfo * playerInfo(new CPlayerInfo);
			playerInfo->AllowPBT(true);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			varPlayer.SetPlayerInfo(playerInfo);
			varPlayer.VirtualSessionEnd(virtualEndTime);
			const CPlayer player(varPlayer);

			// Perform operation(s) to be tested.
			const bool doPBOnCardInResult(player.DoPBOnCardIn());

			// Test #1.
			{
				bool areEqual(doPBOnCardInResult == doPBOnCardInExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED doPBOnCardInResult=%d expected=%d in %s"),
					(int)doPBOnCardInResult,
					(int)doPBOnCardInExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

// #pragma region GetLastSessionStartTime
		/// <summary>
		/// Functional test for:
		///     CPlayer::CPlayer()
		///     CPlayer::GetLastSessionStartTime()
		/// Test the default constructor for default value(s).
		/// </summary>
		TEST(Player5, GetLastSessionStartTime_Default)
		{
			// Data.
			const time_t lastSessionStartTime(0);

			// Perform operation(s) and test(s).
			const CPlayer player;
			ASSERT_EQ(lastSessionStartTime, player.GetLastSessionStartTime());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayer::CPlayer(const CConfig &)
		///     CPlayer::GetLastSessionStartTime()
		/// Test the CConfig constructor for default value(s).
		/// </summary>
		TEST(Player5, GetLastSessionStartTime_CConfigDefault)
		{
			// Data.
			const time_t lastSessionStartTime(0);

			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) and test(s).
			const CPlayer player(config, egmConfig);
			ASSERT_EQ(lastSessionStartTime, player.GetLastSessionStartTime());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayer::CPlayer(CCard *, ...)
		///     CPlayer::GetLastSessionStartTime()
		/// Test the CCard constructor for default value(s).
		/// </summary>
		TEST(Player5, GetLastSessionStartTime_CCardDefault)
		{
			// Data.
			const time_t lastSessionStartTime(0);

			// Setup.
			CCard *card(new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE));
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
            CGames *games = new CGames();
			// Perform operation(s) and test(s).
			const CPlayer player(card, nullptr, config, egmConfig, TIME_DEFAULT_VALUE, false, games);
			ASSERT_EQ(lastSessionStartTime, player.GetLastSessionStartTime());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayer::CPlayer(const CPlayer &)
		///     CPlayer::GetLastSessionStartTime()
		/// Test the copy constructor with non-default value(s).
		/// </summary>
		TEST(Player5, GetLastSessionStartTime_Copy)
		{
			// Data.
			const time_t lastSessionStartTime(CUtilities::GetTimeFromFields(2015, 10, 20, 12, 23, 34));

			// Setup.
			CCard *card(new CCard(lastSessionStartTime, lastSessionStartTime));
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
            CGames *games = new CGames();
			CPlayer sourcePlayer(card, nullptr, config, egmConfig, TIME_DEFAULT_VALUE, false, games);

			const time_t utcRemovalTime(lastSessionStartTime + 60);
			const bool makeVirtual(false);
			CMeters *virtualStartMeters(nullptr);
			sourcePlayer.CardRemoved(utcRemovalTime, utcRemovalTime, makeVirtual);
			ASSERT_EQ(lastSessionStartTime, sourcePlayer.GetLastSessionStartTime()); // Verify setup.

			// Perform operation(s) and test(s).
			const CPlayer player(sourcePlayer);
			ASSERT_EQ(lastSessionStartTime, player.GetLastSessionStartTime());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayer::operator=()
		///     CPlayer::GetLastSessionStartTime()
		/// Test the assignment operator with non-default value(s).
		/// </summary>
		TEST(Player5, GetLastSessionStartTime_Assignment)
		{
			// Data.
			const time_t lastSessionStartTimeDefault(0);
			const time_t lastSessionStartTime(CUtilities::GetTimeFromFields(2015, 10, 20, 12, 23, 34));

			// Setup.
			CCard *card(new CCard(lastSessionStartTime, lastSessionStartTime));
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
            CGames *games = new CGames();
			CPlayer sourcePlayer(card, nullptr, config, egmConfig, TIME_DEFAULT_VALUE, false, games);

			const time_t utcRemovalTime(lastSessionStartTime + 60);
			const bool makeVirtual(false);
			CMeters *virtualStartMeters(nullptr);
			sourcePlayer.CardRemoved(utcRemovalTime, utcRemovalTime, makeVirtual);
			ASSERT_EQ(lastSessionStartTime, sourcePlayer.GetLastSessionStartTime()); // Verify source setup.

			CPlayer player;
			ASSERT_EQ(lastSessionStartTimeDefault, player.GetLastSessionStartTime()); // Verify target setup.
			
			// Perform operation(s) and test(s).
			player = sourcePlayer;
			ASSERT_EQ(lastSessionStartTime, player.GetLastSessionStartTime());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayer::CPlayer(CCard *, ...)
		///     CPlayer::CardRemoved()
		///     CPlayer::GetLastSessionStartTime()
		/// Test physical card out with no virtual session for correct non-default value(s).
		/// </summary>
		TEST(Player5, GetLastSessionStartTime_EndSessionNoVirtual)
		{
			// Data.
			const time_t lastSessionStartTime(CUtilities::GetTimeFromFields(2015, 10, 20, 12, 23, 34));

			// Setup.
			CCard *card(new CCard(lastSessionStartTime, lastSessionStartTime));
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			const time_t utcRemovalTime(lastSessionStartTime + 60);
			const bool makeVirtual(false);
			CMeters *virtualStartMeters(nullptr);
            CGames *games = new CGames();
			// Perform operation(s) and test(s).
			CPlayer player(card, nullptr, config, egmConfig, TIME_DEFAULT_VALUE, false, games);
			player.CardRemoved(utcRemovalTime, utcRemovalTime, makeVirtual);
			ASSERT_EQ(lastSessionStartTime, player.GetLastSessionStartTime());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayer::CPlayer(CCard *, ...)
		///     CPlayer::CardRemoved()
		///     CPlayer::GetLastSessionStartTime()
		/// Test physical card out with make virtual session for correct non-default value(s).
		/// </summary>
		TEST(Player5, GetLastSessionStartTime_EndSessionMakeVirtual)
		{
			// Data.
			const time_t lastSessionStartTime(CUtilities::GetTimeFromFields(2015, 10, 20, 12, 23, 34));

			// Setup.
			CCard *card(new CCard(lastSessionStartTime, lastSessionStartTime));
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			const time_t utcRemovalTime(lastSessionStartTime + 60);
			const bool makeVirtual(true);
			CMeters *virtualStartMeters(new CMeters(MEMORY_NONE));
            CGames *games = new CGames();
			// Perform operation(s) and test(s).
			CPlayer player(card, nullptr, config, egmConfig, TIME_DEFAULT_VALUE, false, games);
			player.CardRemoved(utcRemovalTime, utcRemovalTime, makeVirtual);
			ASSERT_EQ(lastSessionStartTime, player.GetLastSessionStartTime());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayer::VirtualSessionEnd()
		///     CPlayer::CardRemoved()
		///     CPlayer::GetLastSessionStartTime()
		/// Test end of virtual session for correct non-default value(s).
		/// </summary>
		TEST(Player5, GetLastSessionStartTime_VirtualSessionEnd)
		{
			// Data.
			const time_t lastSessionStartTime(CUtilities::GetTimeFromFields(2015, 10, 20, 12, 23, 34));

			// Setup.
			CCard *card(new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE));
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
            CGames *games = new CGames();
			CPlayer player(card, nullptr, config, egmConfig, TIME_DEFAULT_VALUE, false, games);

			const time_t utcRemovalTime(lastSessionStartTime);
			const bool makeVirtual(true);
			CMeters *virtualStartMeters(new CMeters(MEMORY_NONE));
			player.CardRemoved(utcRemovalTime, utcRemovalTime, makeVirtual);

			const time_t virtualEndTime(utcRemovalTime + 120);

			// Perform operation(s) and test(s).
			player.VirtualSessionEnd(virtualEndTime);
			ASSERT_EQ(lastSessionStartTime, player.GetLastSessionStartTime());
		}
// #pragma endregion GetLastSessionStartTime

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::IsBannedFromPlay()
		/// Test path(s) with:
		///     m_playerInfo == NULL
		/// </summary>
		TEST(Player5, IsBannedFromPlay0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool bannedFromPlayExpected(false);

			// Setup.
			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(NULL);
			const CPlayer player(varPlayer);

			// Perform operation(s) to be tested.
			const bool bannedFromPlayResult(player.IsBannedFromPlay());

			// Test #1.
			{
				bool areEqual(bannedFromPlayResult == bannedFromPlayExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bannedFromPlayResult=%d expected=%d in %s"),
					(int)bannedFromPlayResult,
					(int)bannedFromPlayExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::IsBannedFromPlay()
		/// Test path(s) with:
		///     m_playerInfo != NULL
		/// </summary>
		TEST(Player5, IsBannedFromPlay1Test)
		{
			// Test data -- inputs.
			const bool bBirthdayToday(false);
			const bool bAnniversaryToday(false);
			const bool bBannedFromPlay(true);

			// Test data -- expected results.
			const bool bannedFromPlayExpected(true);

			// Setup.
			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->SetFlags(bBirthdayToday, bAnniversaryToday, bBannedFromPlay);

			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer player(varPlayer);

			// Perform operation(s) to be tested.
			const bool bannedFromPlayResult(player.IsBannedFromPlay());

			// Test #1.
			{
				bool areEqual(bannedFromPlayResult == bannedFromPlayExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bannedFromPlayResult=%d expected=%d in %s"),
					(int)bannedFromPlayResult,
					(int)bannedFromPlayExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetInPlayDisplayType()
		/// </summary>
		TEST(Player5, GetInPlayDisplayTypeTest)
		{
			// Test data -- inputs.
			LPCTSTR const sessionPointsmessage(_T(""));

			// Test data -- expected results.
			const InPlayDisplayType inPlayDisplayTypeExpected(IPDT_TOTAL);

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetSessionPointsMessage(sessionPointsmessage);
			varConfig.SetDisplayPlayerPtsTotal(true);
			const CConfig &config(varConfig);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer player(varPlayer);

			// Perform operation(s) to be tested.
			const InPlayDisplayType inPlayDisplayTypeResult(player.GetInPlayDisplayType());

			// Test #1.
			{
				bool areEqual(inPlayDisplayTypeResult == inPlayDisplayTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED inPlayDisplayTypeResult=%d expected=%d in %s"),
					(int)inPlayDisplayTypeResult,
					(int)inPlayDisplayTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}
}
