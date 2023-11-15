#include "stdafx.h"
#include "Player.h"
#include "InternalMessage.h"
#include "MultiplierBonusEvent.h"
#include "UI/ScreenDefs.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "UnitTestingUtils.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	static const time_t TIME_DEFAULT_VALUE(0); // Arbitrary value when any valid value is okay.
	static const byte MEMSET_DEFAULT_VALUE = 0xcd; // Value unlikely to be used for other purposes.

	class Player6 : public ::testing::Test
	{
	};

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetFormattedWelcome()
		/// </summary>
		TEST(Player6, GetFormattedWelcomeTest)
		{
			// Test data -- inputs.
			const bool displayPlayerName(false);

			// Test data -- expected results.
			const string formattedWelcomeExpected(_T("welcome"));

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPlayerName(displayPlayerName);
			const CConfig &config(varConfig);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string formattedWelcomeResult;
			vector<string> welcomeArgs;
			player.GetFormattedWelcome(formattedWelcomeResult, welcomeArgs);

			// Test #1.
			{
				bool areEqual(formattedWelcomeResult == formattedWelcomeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED formattedWelcomeResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(formattedWelcomeResult.c_str()),
					static_cast<LPCTSTR>(formattedWelcomeExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(welcomeArgs.size(), 0) << "Incorrect argument count";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetFormattedWelcome()
		/// </summary>
		TEST(Player6, GetFormattedWelcomeWithNameTest)
		{
			// Test data -- inputs.
			const bool displayPlayerName(true);
			const string name("Mark");

			// Test data -- expected results.
			const string formattedWelcomeExpected(_T("welcome_with_name"));
			const string nameExpected(name);

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPlayerName(displayPlayerName);
			const CConfig &config(varConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetName(name);
			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(playerInfo);
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string formattedWelcomeResult;
			vector<string> welcomeArgs;
			player.GetFormattedWelcome(formattedWelcomeResult, welcomeArgs);

			// Test #1.
			{
				bool areEqual(formattedWelcomeResult == formattedWelcomeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED formattedWelcomeResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(formattedWelcomeResult.c_str()),
					static_cast<LPCTSTR>(formattedWelcomeExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(welcomeArgs.size(), 1) << "Incorrect argument count";

				ASSERT_EQ(welcomeArgs[0], nameExpected) << "Incorrect argument";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetGoodCardText()
		/// </summary>
		TEST(Player6, GetGoodCardTextTest)
		{
			// Test data -- expected results.
			string const goodCardTextExpected(_T("good_card_read"));

			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string const goodCardTextResult(player.GetGoodCardText(false));

			// Test #1.
			{
				bool areEqual(goodCardTextResult == goodCardTextExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED goodCardTextResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(goodCardTextResult.c_str()),
					static_cast<LPCTSTR>(goodCardTextExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetFormattedBalanceRetrievalText()
		/// </summary>
		TEST(Player6, GetFormattedBalanceRetrievalText0Test)
		{
			// Test data -- expected results.
			const string formattedBalanceRetrievalTextExpected(_T("balance_retrieval"));

			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const string goodCardTextResult(player.GetFormattedBalanceRetrievalText());

			// Test #1.
			{
				bool areEqual(goodCardTextResult == formattedBalanceRetrievalTextExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED goodCardTextResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(goodCardTextResult.c_str()),
					static_cast<LPCTSTR>(formattedBalanceRetrievalTextExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetFormattedDailyPointsText()
		/// Test path(s) with:
		///     m_playerInfo == NULL
		/// </summary>
		TEST(Player6, GetFormattedDailyPointsText0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string formattedDailyPointsTextExpected;

			// Setup.
			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(NULL);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string formattedDailyPointsTextResult;
			vector<string> args;
			player.GetFormattedDailyPointsText(formattedDailyPointsTextResult, args);

			// Test #1.
			{
				bool areEqual(formattedDailyPointsTextResult == formattedDailyPointsTextExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED formattedDailyPointsTextResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(formattedDailyPointsTextResult.c_str()),
					static_cast<LPCTSTR>(formattedDailyPointsTextExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetFormattedDailyPointsText()
		/// Test path(s) with:
		///     m_playerInfo != NULL
		///     m_playerInfo->GetDailyPoints() == (DWORD)-1
		/// </summary>
		TEST(Player6, GetFormattedDailyPointsText1Test)
		{
			// Test data -- inputs.
			const DWORD dailyPoints((DWORD)-1);

			// Test data -- expected results.
			const string formattedDailyPointsTextExpected;

			// Setup.
			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->SetDailyPoints(dailyPoints);

			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string formattedDailyPointsTextResult;
			vector<string> args;
			player.GetFormattedDailyPointsText(formattedDailyPointsTextResult, args);

			// Test #1.
			{
				bool areEqual(formattedDailyPointsTextResult == formattedDailyPointsTextExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED formattedDailyPointsTextResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(formattedDailyPointsTextResult.c_str()),
					static_cast<LPCTSTR>(formattedDailyPointsTextExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetFormattedDailyPointsText()
		///     CPlayer::GetFormattedStringWithOptionalName(UINT ...)
		///     CPlayer::GetFormattedStringWithOptionalName(LPCTSTR ...)
		/// Test path(s) with:
		///     m_playerInfo != NULL
		///     m_playerInfo->GetDailyPoints() != (DWORD)-1
		///     m_bDisplayName == false -- AKA config.DisplayPlayerName() == false
		///     m_dailyPointsDisplaySuffixMessage == "tiered points today"
		/// </summary>
		TEST(Player6, GetFormattedDailyTieredPointsTextTest)
		{
			// Test data -- inputs.
			const DWORD dailyPoints((DWORD)-2);
			const bool displayPlayerName(false);

			// Test data -- expected results.
			const string formattedDailyPointsTextExpected(_T("daily_earned_points"));
			const string formattedDailyPointsArgsExpected(_T("-2"));

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.BuildYourself();
			varConfig.SetDisplayPlayerName(displayPlayerName);
			const CConfig &config(varConfig);

			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->SetDailyPoints(dailyPoints);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string formattedPointsToNextTierResult;
			vector<string> formattedPointsToNextTierArgsResult;
			player.GetFormattedDailyPointsText(formattedPointsToNextTierResult, formattedPointsToNextTierArgsResult);

			// Test #1.
			{
				bool areEqual(formattedPointsToNextTierResult == formattedDailyPointsTextExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED formattedPointsToNextTierResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(formattedPointsToNextTierResult.c_str()),
					static_cast<LPCTSTR>(formattedDailyPointsTextExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(formattedPointsToNextTierArgsResult.size(), 1) << "Incorrect arguments count";

				ASSERT_EQ(formattedPointsToNextTierArgsResult[0], formattedDailyPointsArgsExpected) << "Incorrect arguments";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetFormattedDailyPointsText()
		///     CPlayer::GetFormattedStringWithOptionalName(UINT ...)
		///     CPlayer::GetFormattedStringWithOptionalName(LPCTSTR ...)
		/// Test path(s) with:
		///     m_playerInfo != NULL
		///     m_playerInfo->GetDailyPoints() != (DWORD)-1
		///     m_bDisplayName == false -- AKA config.DisplayPlayerName() == false
		/// </summary>
		TEST(Player6, GetFormattedDailyPointsText2Test)
		{
			// Test data -- inputs.
			const DWORD dailyPoints((DWORD)-2);
			const bool displayPlayerName(false);

			// Test data -- expected results.
			const string formattedDailyPointsTextExpected(_T("daily_earned_points"));
			const string formattedDailyPointsArgsExpected(_T("-2"));

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.BuildYourself();
			varConfig.SetDisplayPlayerName(displayPlayerName);
			const CConfig &config(varConfig);

			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->SetDailyPoints(dailyPoints);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string formattedPointsToNextTierResult;
			vector<string> formattedPointsToNextTierArgsResult;
			player.GetFormattedDailyPointsText(formattedPointsToNextTierResult, formattedPointsToNextTierArgsResult);

			// Test #1.
			{
				bool areEqual(formattedPointsToNextTierResult == formattedDailyPointsTextExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED formattedPointsToNextTierResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(formattedPointsToNextTierResult.c_str()),
					static_cast<LPCTSTR>(formattedDailyPointsTextExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(formattedPointsToNextTierArgsResult.size(), 1) << "Incorrect arguments count";

				ASSERT_EQ(formattedPointsToNextTierArgsResult[0], formattedDailyPointsArgsExpected) << "Incorrect arguments";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetFormattedDailyPointsText()
		///     CPlayer::GetFormattedStringWithOptionalName(UINT ...)
		///     CPlayer::GetFormattedStringWithOptionalName(LPCTSTR ...)
		/// Test path(s) with:
		///     m_playerInfo != NULL
		///     m_playerInfo->GetDailyPoints() != (DWORD)-1
		///     m_bDisplayName == false -- AKA config.DisplayPlayerName() == false
		/// </summary>
		TEST(Player6, GetFormattedDailyPointsText3Test)
		{
			// Test data -- inputs.
			const DWORD dailyPoints((DWORD)-2);
			const bool displayPlayerName(true);
			LPCTSTR const name = _T("Playre");

			// Test data -- expected results.
			const string formattedDailyPointsTextExpected(_T("daily_earned_points"));
			const string formattedDailyPointsArgsExpected(_T("-2"));

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.BuildYourself();
			varConfig.SetDisplayPlayerName(displayPlayerName);
			const CConfig &config(varConfig);

			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->SetDailyPoints(dailyPoints);
			playerInfo->SetName(name);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string formattedPointsToNextTierResult;
			vector<string> formattedPointsToNextTierArgsResult;
			player.GetFormattedDailyPointsText(formattedPointsToNextTierResult, formattedPointsToNextTierArgsResult);

			// Test #1.
			{
				bool areEqual(formattedPointsToNextTierResult == formattedDailyPointsTextExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED formattedPointsToNextTierResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(formattedPointsToNextTierResult.c_str()),
					static_cast<LPCTSTR>(formattedDailyPointsTextExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(formattedPointsToNextTierArgsResult.size(), 1) << "Incorrect arguments count";

				ASSERT_EQ(formattedPointsToNextTierArgsResult[0], formattedDailyPointsArgsExpected) << "Incorrect arguments";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetFormattedPointsToNextTierText()
		/// Test path(s) with:
		///     m_playerInfo == NULL
		/// </summary>
		TEST(Player6, GetFormattedPointsToNextTierText0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string formattedPointsToNextTierExpected;

			// Setup.
			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(NULL);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string formattedPointsToNextTierResult;
			vector<string> args;
			player.GetFormattedPointsToNextTierText(formattedPointsToNextTierResult, args);

			// Test #1.
			{
				bool areEqual(formattedPointsToNextTierResult == formattedPointsToNextTierExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED formattedPointsToNextTierResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(formattedPointsToNextTierResult.c_str()),
					static_cast<LPCTSTR>(formattedPointsToNextTierExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}
		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetFormattedPointsToNextTierText()
		/// Test path(s) with:
		///     m_playerInfo != NULL
		///     m_playerInfo->GetDailyPoints() == (DWORD)-1
		/// </summary>
		TEST(Player6, GetFormattedPointsToNextTierText1Test)
		{
			// Test data -- inputs.
			const DWORD pointsTillNextTier((DWORD)-1);

			// Test data -- expected results.
			const string formattedPointsToNextTierExpected;

			// Setup.
			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->SetPointsTillNextTier(pointsTillNextTier);

			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string formattedPointsToNextTierResult;
			vector<string> args;
			player.GetFormattedPointsToNextTierText(formattedPointsToNextTierResult, args);

			// Test #1.
			{
				bool areEqual(formattedPointsToNextTierResult == formattedPointsToNextTierExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED formattedPointsToNextTierResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(formattedPointsToNextTierResult.c_str()),
					static_cast<LPCTSTR>(formattedPointsToNextTierExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetFormattedPointsToNextTierText()
		/// Test path(s) with:
		///     m_playerInfo != NULL
		///     m_playerInfo->GetDailyPoints() != (DWORD)-1
		/// </summary>
		TEST(Player6, GetFormattedPointsToNextTierText2Test)
		{
			// Test data -- inputs.
			const DWORD pointsTillNextTier((DWORD)-2);
			const bool displayPlayerName(false);

			// Test data -- expected results.
			const string formattedPointsToNextTierExpected(_T("points_needed_for_next_tier"));
			const string formattedPointsToNextTierArgsExpected(_T("-2"));

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPlayerName(displayPlayerName);
			const CConfig &config(varConfig);

			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->SetPointsTillNextTier(pointsTillNextTier);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string formattedPointsToNextTierResult;
			vector<string> formattedPointsToNextTierArgsResult;
			player.GetFormattedPointsToNextTierText(formattedPointsToNextTierResult, formattedPointsToNextTierArgsResult);

			// Test #1.
			{
				bool areEqual(formattedPointsToNextTierResult == formattedPointsToNextTierExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED formattedPointsToNextTierResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(formattedPointsToNextTierResult.c_str()),
					static_cast<LPCTSTR>(formattedPointsToNextTierExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(formattedPointsToNextTierArgsResult.size(), 1) << "Incorrect argument count";

				ASSERT_EQ(formattedPointsToNextTierArgsResult[0], formattedPointsToNextTierArgsExpected) << "Incorrect argument count";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::SetSessionStartMeters()
		///     CPlayer::GetSessionStartMetersCopy()
		/// Test path(s) with:
		///     meters == NULL
		/// </summary>
		TEST(Player6, SetSessionStartMeters0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.

			// Setup.
			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(NULL);

			// Perform operation(s) to be tested.
			varPlayer.SetSessionStartMeters(NULL);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				CMeters *metersResult(player.GetSessionStartMetersCopy());

				string failedMsg;
				failedMsg = FormatString(_T("FAILED metersResult=%p expected=NULL in %s"),
					(const void *)metersResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_EQ((void *)nullptr, (void *)metersResult) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::SetSessionStartMeters()
		///     CPlayer::GetSessionStartMetersCopy()
		/// Test path(s) with:
		///     meters != NULL -- 1 call
		/// </summary>
		TEST(Player6, SetSessionStartMeters1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.

			// Setup.
			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(NULL);

			// Perform operation(s) to be tested.
			CMeters *meters(new CMeters(MEMORY_NONE));
			varPlayer.SetSessionStartMeters(meters);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				CMeters *metersResult(player.GetSessionStartMetersCopy());

				string failedMsg;
				failedMsg = FormatString(_T("FAILED metersResult=%p expected=NOT NULL in %s"),
					(const void *)metersResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)metersResult) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::SetSessionStartMeters()
		/// Test path(s) with:
		///     meters != NULL -- 2 calls
		/// </summary>
		TEST(Player6, SetSessionStartMeters2Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.

			// Setup.
			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(NULL);

			// Perform operation(s) to be tested.
			CMeters *meters0(new CMeters(MEMORY_NONE));
			varPlayer.SetSessionStartMeters(meters0);
			CMeters *meters(new CMeters(MEMORY_NONE));
			varPlayer.SetSessionStartMeters(meters);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				CMeters *metersResult(player.GetSessionStartMetersCopy());

				string failedMsg;
				failedMsg = FormatString(_T("FAILED metersResult=%p expected=NOT NULL in %s"),
					(const void *)metersResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)metersResult) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ResetLastUpdateSentTime()
		///     CPlayer::GetLastUpdateSentTime()
		/// </summary>
		TEST(Player6, ResetLastUpdateSentTimeTest)
		{
			// Test data -- inputs.
			const time_t utcTime(12321);

			// Test data -- expected results.
			const time_t lastUpdateSentTimeExpected(12321);

			// Setup.
			CPlayer varPlayer;

			// Perform operation(s) to be tested.
			varPlayer.ResetLastUpdateSentTime(utcTime);
			const CPlayer &player(varPlayer);
			const time_t lastUpdateSentTimeResult(player.GetLastUpdateSentTime());

			// Test #1.
			{
				bool areEqual(lastUpdateSentTimeResult == lastUpdateSentTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED lastUpdateSentTimeResult=%lld expected=%lld in %s"),
					(long long)lastUpdateSentTimeResult,
					(long long)lastUpdateSentTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetLossLimitSessionBalance()
		///     CPlayer::GetLossLimitSessionBalance()
		/// </summary>
		TEST(Player6, SetLossLimitSessionBalanceTest)
		{
			// Test data -- inputs.
			const WORD newBalance(321);

			// Test data -- expected results.
			const WORD lossLimitSessionBalanceExpected(321);

			// Setup.
			CPlayer varPlayer;

			// Perform operation(s) to be tested.
			varPlayer.SetLossLimitSessionBalance(newBalance);
			const CPlayer &player(varPlayer);
			const WORD lossLimitSessionBalanceResult(player.GetLossLimitSessionBalance());

			// Test #1.
			{
				bool areEqual(lossLimitSessionBalanceResult == lossLimitSessionBalanceExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED lossLimitSessionBalanceResult=%u expected=%u in %s"),
					(unsigned)lossLimitSessionBalanceResult,
					(unsigned)lossLimitSessionBalanceExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetHasPlayed()
		///     CPlayer::GetHasPlayed()
		/// </summary>
		TEST(Player6, SetHasPlayedTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const WORD hasPlayedExpected(true);

			// Setup.
			CPlayer varPlayer;

			// Perform operation(s) to be tested.
			varPlayer.SetHasPlayed();
			const CPlayer &player(varPlayer);
			const WORD hasPlayedResult(player.GetHasPlayed());

			// Test #1.
			{
				bool areEqual(hasPlayedResult == hasPlayedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED hasPlayedResult=%d expected=%u in %s"),
					(int)hasPlayedResult,
					(int)hasPlayedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::IsMaxCardLimitReached()
		/// Test path(s) with:
		///     m_playerInfo == NULL
		/// </summary>
		TEST(Player6, IsMaxCardLimitReached0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool maxCardLimitReachedExpected(false);

			// Setup.
			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(NULL);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const bool maxCardLimitReachedResult(player.IsMaxCardLimitReached());

			// Test #1.
			{
				bool areEqual(maxCardLimitReachedResult == maxCardLimitReachedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED maxCardLimitReachedResult=%d expected=%d in %s"),
					(int)maxCardLimitReachedResult,
					(int)maxCardLimitReachedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::IsMaxCardLimitReached()
		/// Test path(s) with:
		///     m_playerInfo != NULL
		///     m_playerInfo->GetDailyPoints() == (DWORD)-1
		/// </summary>
		TEST(Player6, IsMaxCardLimitReached1Test)
		{
			// Test data -- inputs.
			const bool bMaxCardLimitReached(true);

			// Test data -- expected results.
			const bool maxCardLimitReachedExpected(true);

			// Setup.
			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->MaxCardLimitReached(bMaxCardLimitReached);

			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const bool maxCardLimitReachedResult(player.IsMaxCardLimitReached());

			// Test #1.
			{
				bool areEqual(maxCardLimitReachedResult == maxCardLimitReachedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED maxCardLimitReachedResult=%d expected=%d in %s"),
					(int)maxCardLimitReachedResult,
					(int)maxCardLimitReachedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetMaxCardLimitText()
		/// Test with:
		///     playerInfo == NULL
		/// </summary>
		TEST(Player6, GetMaxCardLimitText0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string maxCardLimitTextExpected;

			// Setup.
			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(NULL);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const string maxCardLimitTextResult = player.GetMaxCardLimitText();

			// Test #1.
			{
				bool areEqual(maxCardLimitTextResult == maxCardLimitTextExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED maxCardLimitTextResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(maxCardLimitTextResult.c_str()),
					static_cast<LPCTSTR>(maxCardLimitTextExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetMaxCardLimitText()
		/// Test with:
		///     playerInfo != NULL
		/// </summary>
		TEST(Player6, GetMaxCardLimitText1Test)
		{
			// Test data -- inputs.
			LPCTSTR const text(_T("respo"));

			// Test data -- expected results.
			const string maxCardLimitTextExpected(_T("respo"));

			// Setup.
			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->SetResponseText(text );

			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const string maxCardLimitTextResult = player.GetMaxCardLimitText();

			// Test #1.
			{
				bool areEqual(maxCardLimitTextResult == maxCardLimitTextExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED maxCardLimitTextResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(maxCardLimitTextResult.c_str()),
					static_cast<LPCTSTR>(maxCardLimitTextExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetTierMultiplierPoints()
		/// Test with:
		///     playerInfo == NULL
		/// </summary>
		TEST(Player6, GetTierMultiplierPoints0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const DWORD tierMultiplierPointsExpected(0);

			// Setup.
			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(NULL);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const DWORD tierMultiplierPointsResult = player.GetTierMultiplierPoints();

			// Test #1.
			{
				bool areEqual(tierMultiplierPointsResult == tierMultiplierPointsExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED tierMultiplierPointsResult=%u expected=%u in %s"),
					(unsigned)tierMultiplierPointsResult,
					(unsigned)tierMultiplierPointsExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetTierMultiplierPoints()
		/// Test with:
		///     playerInfo != NULL
		///     m_playerInfo->GetTierMultiplier() == 0
		/// </summary>
		TEST(Player6, GetTierMultiplierPoints1Test)
		{
			// Test data -- inputs.
			const DWORD tierMultiplier(0);

			// Test data -- expected results.
			const DWORD tierMultiplierPointsExpected(0);

			// Setup.
			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->SetTierMultiplier(tierMultiplier);

			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const DWORD tierMultiplierPointsResult = player.GetTierMultiplierPoints();

			// Test #1.
			{
				bool areEqual(tierMultiplierPointsResult == tierMultiplierPointsExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED tierMultiplierPointsResult=%u expected=%u in %s"),
					(unsigned)tierMultiplierPointsResult,
					(unsigned)tierMultiplierPointsExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetTierMultiplierPoints()
		/// Test with:
		///     playerInfo != NULL
		///     m_playerInfo->GetTierMultiplier() > 0
		/// </summary>
		TEST(Player6, GetTierMultiplierPoints2Test)
		{
			// Test data -- inputs.
			const DWORD tierMultiplier(300); // We can have tier multipliers with decimals now.
			const DWORD multiplier(1);
			const DWORD divider(10);
			const DWORD points(0);
			const DWORD countdown(2);
			const DWORD decrement(countdown);

			// Test data -- expected results.
			const DWORD tierMultiplierPointsExpected(0);

			// Setup.
			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->SetTierMultiplier(tierMultiplier);
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(playerInfo);
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + decrement, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const DWORD tierMultiplierPointsResult = player.GetTierMultiplierPoints();

			// Test #1.
			{
				bool areEqual(tierMultiplierPointsResult == tierMultiplierPointsExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED tierMultiplierPointsResult=%u expected=%u in %s"),
					(unsigned)tierMultiplierPointsResult,
					(unsigned)tierMultiplierPointsExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetFormattedTierPointsLabel()
		/// Test path(s) with:
		///     m_tierPointsLabel is empty
		/// </summary>
		TEST(Player6, GetFormattedTierPointsLabelTest)
		{
			// Test data -- expected results.
			const string formattedTierPointsLabelExpected(_T("Tier Points:"));

			// Setup.
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();

			CPlayerInfo *playerInfo(new CPlayerInfo);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const string formattedTierPointsLabelResult(player.GetFormattedTierPointsLabel());

			// Test
			{
				bool areEqual(formattedTierPointsLabelResult == formattedTierPointsLabelExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED formattedTierPointsLabelResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(formattedTierPointsLabelResult.c_str()),
					static_cast<LPCTSTR>(formattedTierPointsLabelExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::GetTieredPointsAsString()
		/// </summary>
		TEST(Player6, GetTieredPointsAsStringTest)
		{
			// Test data -- expected results.
			const DWORD TIERED_POINTS = 969;
			const string formattedTierPointsExpected(_T("969"));

			// Setup.
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();
			config.SetUniverseEnabled(true);

			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->SetTierPoints(TIERED_POINTS);

			CPlayer varPlayer;
			varPlayer.ConfigSettings(config, egmConfig);
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const string formattedTierPointsResult(player.GetTieredPointsAsString());

			// Test
			{
				bool areEqual(formattedTierPointsResult == formattedTierPointsExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED formattedTierPointsResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(formattedTierPointsResult.c_str()),
					static_cast<LPCTSTR>(formattedTierPointsExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

// #pragma region GetRankId
		/// <summary>
		/// Functional test for static const member(s).
		/// </summary>
		TEST(Player6, GetRankId_StaticConst)
		{
			// Verify forwards and backwards compatibility for value(s) that can be persisted.
			ASSERT_EQ((DWORD)-1, CPlayerInfo::INVALID_RANK_ID) << L"CPlayerInfo::INVALID_RANK_ID";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayer::CPlayer()
		///     CPlayer::GetRankId()
		/// Test the default constructor for default value(s).
		/// </summary>
		TEST(Player6, GetRankId_Default)
		{
			// Data.
			const DWORD rankIdExpected(CPlayerInfo::INVALID_RANK_ID);

			// Perform operation(s) and test(s).
			const CPlayer player;
			ASSERT_EQ(rankIdExpected, player.GetRankId());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayer::CPlayer(CCard *, ...)
		///     CPlayer::GetRankId()
		/// Test the CCard constructor with nullptr playerInfo for default value(s).
		/// </summary>
		TEST(Player6, GetRankId_Card_PlayerInfoNull)
		{
			// Data.
			const DWORD rankIdExpected(CPlayerInfo::INVALID_RANK_ID);

			// Setup.
			CCard *card(new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE));
			CPlayerInfo *playerInfo(nullptr);
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const time_t utcTime(TIME_DEFAULT_VALUE);
			const bool goodbyeEnabled(false);
            CGames games;
			// Perform operation(s) and test(s).
			const CPlayer player(card, playerInfo, config, egmConfig, utcTime, goodbyeEnabled, &games);
			ASSERT_EQ(rankIdExpected, player.GetRankId());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayer::CPlayer(CCard *, ...)
		///     CPlayer::GetRankId()
		/// Test the CCard constructor with non-nullptr playerInfo for default value(s).
		/// </summary>
		TEST(Player6, GetRankId_Card_PlayerInfo)
		{
			// Data.
			const DWORD rankIdExpected(5);

			// Setup.
			CCard *card(new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE));
			CPlayerInfo *playerInfo(new CPlayerInfo);
			playerInfo->SetRankId(rankIdExpected);
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const time_t utcTime(TIME_DEFAULT_VALUE);
			const bool goodbyeEnabled(false);
            CGames games;
			// Perform operation(s) and test(s).
			const CPlayer player(card, playerInfo, config, egmConfig, utcTime, goodbyeEnabled, &games);
			ASSERT_EQ(rankIdExpected, player.GetRankId());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayer::CPlayer(const CConfig &)
		///     CPlayer::GetRankId()
		/// Test CConfig constructor with player info restored from NVRAM.
		/// </summary>
		TEST(Player6, GetRankId_Config_NvramRestored)
		{
			// Data.
			const DWORD rankIdExpected(17);

			// Setup.
			CUnitTestingUtils::SetNvramValidAndUpgraded();
			{
				CCard *card(new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE));
				CPlayerInfo *playerInfo(new CPlayerInfo);
				playerInfo->SetRankId(rankIdExpected);
				const CConfig config(false, "");
				const CEGMConfig egmConfig(MEMORY_NONE);
				const time_t utcTime(TIME_DEFAULT_VALUE);
				const bool goodbyeEnabled(false);
                CGames games;
				const CPlayer player(card, playerInfo, config, egmConfig, utcTime, goodbyeEnabled, &games);
				ASSERT_EQ(rankIdExpected, player.GetRankId()); // Verify setup.
			}

			// Perform operation(s) and test(s).
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CPlayer player(config, egmConfig);
			ASSERT_EQ(rankIdExpected, player.GetRankId());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayer::SetPlayerInfo()
		///     CPlayer::GetRankId()
		/// Test copy constructor using non-default value(s).
		/// </summary>
		TEST(Player6, GetRankId_Copy)
		{
			// Data.
			const DWORD rankIdExpected(5);

			// Setup.
			CPlayer sourcePlayer;
			CPlayerInfo *playerInfo(new CPlayerInfo());
			playerInfo->SetRankId(rankIdExpected);
			sourcePlayer.SetPlayerInfo(playerInfo);

			// Perform operation(s) to be tested.
			CPlayer player(sourcePlayer);
			ASSERT_EQ(rankIdExpected, player.GetRankId());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayer::SetPlayerInfo()
		///     CPlayer::GetRankId()
		/// Test assignment operator using non-default value(s).
		/// </summary>
		TEST(Player6, GetRankId_Assignment)
		{
			// Data.
			const DWORD rankIdSetup(7);
			const DWORD rankIdExpected(5);

			// Setup.
			CPlayer sourcePlayer;
			CPlayerInfo *sourcePlayerInfo(new CPlayerInfo());
			sourcePlayerInfo->SetRankId(rankIdExpected);
			sourcePlayer.SetPlayerInfo(sourcePlayerInfo);

			CPlayer player;
			CPlayerInfo *playerInfo(new CPlayerInfo());
			playerInfo->SetRankId(rankIdSetup);
			player.SetPlayerInfo(playerInfo);
			ASSERT_EQ(rankIdSetup, player.GetRankId()); // Verify setup.

			// Perform operation(s) to be tested.
			player = sourcePlayer;
			ASSERT_EQ(rankIdExpected, player.GetRankId());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayer::SetPlayerInfo()
		///     CPlayer::GetRankId()
		/// Test for non-default value(s) after setting player information.
		/// </summary>
		TEST(Player6, GetRankId_SetPlayerInfo)
		{
			// Data.
			const DWORD rankIdExpected(5);

			// Setup.
			CPlayer player;
			CPlayerInfo *playerInfo(new CPlayerInfo());
			playerInfo->SetRankId(rankIdExpected);

			// Perform operation(s) to be tested.
			player.SetPlayerInfo(playerInfo);
			ASSERT_EQ(rankIdExpected, player.GetRankId());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayer::SetPlayerInfo()
		///     CPlayer::GetRankId()
		/// Test for default value(s) when player information is set to nullptr.
		/// </summary>
		TEST(Player6, GetRankId_SetPlayerInfoNull)
		{
			// Data.
			const DWORD rankIdSetup(5);
			const DWORD rankIdExpected(CPlayerInfo::INVALID_RANK_ID);

			// Setup.
			CPlayer player;
			CPlayerInfo *playerInfo(new CPlayerInfo());
			playerInfo->SetRankId(rankIdSetup);
			player.SetPlayerInfo(playerInfo);
			ASSERT_EQ(rankIdSetup, player.GetRankId()); // Verity setup.

			// Perform operation(s) to be tested.
			player.SetPlayerInfo(nullptr);
			ASSERT_EQ(rankIdExpected, player.GetRankId());
		}
// #pragma endregion GetRankId

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::IsUniverseEnabled()
		/// </summary>
		TEST(Player6, UniverseEnabledTest)
		{
			// Setup.
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();
			config.SetUniverseEnabled(true);

			CPlayer varPlayer_UniverseEnabled;
			varPlayer_UniverseEnabled.ConfigSettings(config, egmConfig);
			const CPlayer &player1(varPlayer_UniverseEnabled);

			config.SetUniverseEnabled(false);
			CPlayer varPlayer_UniverseDisabled;
			varPlayer_UniverseDisabled.ConfigSettings(config, egmConfig);
			const CPlayer &player2(varPlayer_UniverseDisabled);

			// Tests
			{
				ASSERT_TRUE(player1.IsUniverseEnabled()) << L"FAILED IsUniverseEnabled should be true";
				ASSERT_FALSE(player2.IsUniverseEnabled()) << L"FAILED IsUniverseEnabled should be false";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::DisplayProfileButton()
		/// </summary>
		TEST(Player6, DisplayProfileButtonTest)
		{
			// Setup.
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();
			config.SetDisplayPlayerProfileButton(true);

			CPlayer varPlayer_DisplayPlayerProfileButtonEnabled;
			varPlayer_DisplayPlayerProfileButtonEnabled.ConfigSettings(config, egmConfig);
			const CPlayer &player1(varPlayer_DisplayPlayerProfileButtonEnabled);

			config.SetDisplayPlayerProfileButton(false);
			CPlayer varPlayer_DisplayPlayerProfileButtonDisabled;
			varPlayer_DisplayPlayerProfileButtonDisabled.ConfigSettings(config, egmConfig);
			const CPlayer &player2(varPlayer_DisplayPlayerProfileButtonDisabled);

			// Tests
			{
				ASSERT_TRUE(player1.DisplayProfileButton()) << L"FAILED DisplayProfileButton should be true";
				ASSERT_FALSE(player2.DisplayProfileButton()) << L"FAILED DisplayProfileButton should be false";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path coverage test for:
		///     CPlayer::ConvertPlayerTxTypeToInternalMsgType()
		/// </summary>
		TEST(Player6, ConvertPlayerTxTypeToInternalMsgTypeTest)
		{
			// Setup.
			CPlayer player;

			// Tests
			{
				ASSERT_TRUE(INT_MSG_TRANSACTION_PLAYER_UPDATE == player.ConvertPlayerTxTypeToInternalMsgType(PTT_UPDATE));
				ASSERT_TRUE(INT_MSG_TRANSACTION_PLAYER_CARD_IN == player.ConvertPlayerTxTypeToInternalMsgType(PTT_CARD_IN));
				ASSERT_TRUE(INT_MSG_TRANSACTION_VIRTUAL_SESSION_STARTED == player.ConvertPlayerTxTypeToInternalMsgType(PTT_VIRTUAL_SESSION_START));
				ASSERT_TRUE(INT_MSG_TRANSACTION_PLAYER_CARD_OUT == player.ConvertPlayerTxTypeToInternalMsgType(PTT_CARD_OUT));
				ASSERT_TRUE(INT_MSG_TRANSACTION_PLAYER_CARD_OUT == player.ConvertPlayerTxTypeToInternalMsgType(PTT_VIRTUAL_SESSION_STOP));
			}

			// Cleanup.
		}
}
