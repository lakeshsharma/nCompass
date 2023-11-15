#include "stdafx.h"
#include "Player.h"

// Sentinel includes.
#include "Config.h"
#include "resource.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "UnitTestingUtils.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class PlayerUnitTests3 : public ::testing::Test
	{
	};

		TEST_F(PlayerUnitTests3, PlayerGetInPlayTextGoodCardTest)
		{
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();
			
			// Set config to display 'Good Card'
			config.SetSessionPointsMessage(_T(""));
			config.SetDisplayPlayerPtsTotal(false);
			config.SetDisplayCountdownAndSession(false);
			config.SetDisplayGoodLuckAndSession(false);
			config.SetDisplayTotalAndSessionPtsNum(false);
			config.SetDisplayTotalPtsSessionPtsAsCash(false);
			config.SetDisplayGoodLuckAndSessionPts(false);
			config.SetDisplayPtsTotalAsCashSession(false);

			CPlayer player(config, egmConfig);

			string inPlayString;
			vector<string> inPlayArgs;
			player.GetInPlayText(inPlayString, inPlayArgs);
			ASSERT_EQ(player.GetGoodCardText(true), inPlayString) << L"Player not returning 'Good Card' text.";
		}

		TEST_F(PlayerUnitTests3, PlayerGetInPlayTextPointsDefaultTest)
		{
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();
			
			// Set config to display default 'points in play'
			config.SetDisplayPlayerPtsTotal(true);
			config.SetDisplayCountdownAndSession(false);
			config.SetDisplayGoodLuckAndSession(false);
			config.SetDisplayTotalAndSessionPtsNum(false);
			config.SetDisplayTotalPtsSessionPtsAsCash(false);
			config.SetDisplayGoodLuckAndSessionPts(false);
			config.SetDisplayPtsTotalAsCashSession(false);

			// Set config not to display name
			config.SetDisplayPlayerName(false);

			// CPlayerInfo has to be a pointer because CPlayer deletes it on deconstructon
			CPlayerInfo* playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(1, 1);
			playerInfo->SetPointsAndCountdown(0, 1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			CPlayer player(config, egmConfig);
			player.ResetSession(CUtilities::GetUTCTime()); // Make Session Points 0
			player.SetPlayerInfo(playerInfo);

			string result;
			string expected = "points";
			vector<string> args;
			vector<string> expectedArgs = {"0"};

			player.GetInPlayText(result, args);
			ASSERT_EQ(expected, result) << result + " vs expected: " + expected;
			ASSERT_EQ(args.size(), expectedArgs.size()) << "Incorrect argument count - default case";
			ASSERT_EQ(args[0], expectedArgs[0]) << "Incorrect argument value - default case";

			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 2, true, false);
			player.DecrementCountdown(meters, orig, false); // Make Session Points 1
			delete meters;
			delete orig;
			playerInfo->SetPointsAndCountdown(9, 0, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			result.clear();
			args.clear();
			expectedArgs = {"9"};
			player.GetInPlayText(result, args);

			// Ensure points are not stored in NVRAM
			player.ResetSession(CUtilities::GetUTCTime()); // Make Session Points 0
			playerInfo->SetPointsAndCountdown(0, 0, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			ASSERT_EQ(expected, result) << result + " vs expected: " + expected;
			ASSERT_EQ(args.size(), expectedArgs.size()) << "Incorrect argument count";
			ASSERT_EQ(args[0], expectedArgs[0]) << "Incorrect argument value";
		}

		TEST_F(PlayerUnitTests3, PlayerGetInPlayTextPointsDefaultWithMGMDDisabledTest)
		{
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();

			// Set config to display default 'points in play'
			config.SetDisplayPlayerPtsTotal(true);
			config.SetDisplayCountdownAndSession(false);
			config.SetDisplayGoodLuckAndSession(false);
			config.SetDisplayTotalAndSessionPtsNum(false);
			config.SetDisplayTotalPtsSessionPtsAsCash(false);
			config.SetDisplayGoodLuckAndSessionPts(false);
			config.SetDisplayPtsTotalAsCashSession(false);

			// Set config not to display name
			config.SetDisplayPlayerName(false);

			// CPlayerInfo has to be a pointer because CPlayer deletes it on deconstructon
			CPlayerInfo* playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(1, 1);
			playerInfo->SetPointsAndCountdown(0, 1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			CPlayer player(config, egmConfig);
			player.ResetSession(CUtilities::GetUTCTime()); // Make Session Points 0
			player.SetPlayerInfo(playerInfo);

			string result;
			string expected = "points";
			vector<string> args;
			vector<string> expectedArgs = {"0"};

			player.GetInPlayText(result, args);
			ASSERT_EQ(expected, result) << result + " vs expected: " + expected;
			ASSERT_EQ(args.size(), expectedArgs.size()) << "Incorrect argument count - default case";
			ASSERT_EQ(args[0], expectedArgs[0]) << "Incorrect argument value - default case";

			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 2, true, false);
			player.DecrementCountdown(meters, orig, false); // Make Session Points 1
			delete meters;
			delete orig;
			playerInfo->SetPointsAndCountdown(9, 0, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			result.clear();
			args.clear();
			expectedArgs = {"9"};
			player.GetInPlayText(result, args);

			// Ensure points are not stored in NVRAM
			player.ResetSession(CUtilities::GetUTCTime()); // Make Session Points 0
			playerInfo->SetPointsAndCountdown(0, 0, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			ASSERT_EQ(expected, result) << result + " vs expected: " + expected;
			ASSERT_EQ(args.size(), expectedArgs.size()) << "Incorrect argument count";
			ASSERT_EQ(args[0], expectedArgs[0]) << "Incorrect argument value";
		}

		TEST_F(PlayerUnitTests3, PlayerGetInPlayTextCashDefaultTest)
		{
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();

			// Set config to display default 'points in play'
			config.SetDisplayPlayerPtsTotal(false);
			config.SetDisplayCountdownAndSession(false);
			config.SetDisplayGoodLuckAndSession(false);
			config.SetDisplayTotalAndSessionPtsNum(false);
			config.SetDisplayTotalPtsSessionPtsAsCash(false);
			config.SetDisplayGoodLuckAndSessionPts(false);
			config.SetDisplayPtsTotalAsCashSession(true);

			// Set config not to display name
			config.SetDisplayPlayerName(false);

			// CPlayerInfo has to be a pointer because CPlayer deletes it on deconstructon
			CPlayerInfo* playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(1, 1);
			playerInfo->SetPointsAndCountdown(0, 1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			CPlayer player(config, egmConfig);
			player.ResetSession(CUtilities::GetUTCTime());
			player.SetPlayerInfo(playerInfo);

			string result;
			string expected = "points_as_cash";
			vector<string> args;
			vector<string> expectedArgs = {"$0.00"};

			player.GetInPlayText(result, args);
			ASSERT_EQ(expected, result) << result + " vs expected: " + expected;
			ASSERT_EQ(args.size(), expectedArgs.size()) << "Incorrect argument count - default case";
			ASSERT_EQ(args[0], expectedArgs[0]) << "Incorrect argument value - default case";

			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 2, true, false);
			player.DecrementCountdown(meters, orig, true); // Make Session Points 1
			delete meters;
			delete orig;
			playerInfo->SetPointsAndCountdown(9, 0, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			result.clear();
			args.clear();
			expectedArgs = {"$9.00"};
			player.GetInPlayText(result, args);

			// Ensure points are not stored in NVRAM
			player.ResetSession(CUtilities::GetUTCTime()); // Make Session Points 0
			playerInfo->SetPointsAndCountdown(0, 0, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			ASSERT_EQ(expected, result) << result + " vs expected: " + expected;
			ASSERT_EQ(args.size(), expectedArgs.size()) << "Incorrect argument count";
			ASSERT_EQ(args[0], expectedArgs[0]) << "Incorrect argument value";
		}

		TEST_F(PlayerUnitTests3, PlayerGetInPlayTextPointsWithNameDefaultTest)
		{
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();
			
			// Set config to display default 'points in play'
			config.SetDisplayPlayerPtsTotal(true);
			config.SetDisplayCountdownAndSession(false);
			config.SetDisplayGoodLuckAndSession(false);
			config.SetDisplayTotalAndSessionPtsNum(false);
			config.SetDisplayTotalPtsSessionPtsAsCash(false);
			config.SetDisplayGoodLuckAndSessionPts(false);
			config.SetDisplayPtsTotalAsCashSession(false);

			// Set config to display name
			config.SetDisplayPlayerName(true);

			string testName("Joe Joe");

			// CPlayerInfo has to be a pointer because CPlayer deletes it on deconstructon
			CPlayerInfo* playerInfo = new CPlayerInfo();
			playerInfo->SetName(testName);
			playerInfo->SetMultiplierDivider(1, 1);
			playerInfo->SetPointsAndCountdown(0, 1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			CPlayer player(config, egmConfig);
			player.ResetSession(CUtilities::GetUTCTime());
			player.SetPlayerInfo(playerInfo);

			string result;
			string expected = "points_with_name";;
			vector<string> args;
			vector<string> expectedArgs = {"Joe Joe", "0"};

			player.GetInPlayText(result, args);
			ASSERT_EQ(expected, result) << result + " vs expected: " + expected;
			ASSERT_EQ(args.size(), expectedArgs.size()) << "Incorrect argument count - default case";
			ASSERT_EQ(args[0], expectedArgs[0]) << "Incorrect argument value - default case";
			ASSERT_EQ(args[1], expectedArgs[1]) << "Incorrect argument value - default case";

			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 2, true, false);
			player.DecrementCountdown(meters, orig, true); // Make Session Points 1
			delete meters;
			delete orig;
			playerInfo->SetPointsAndCountdown(9, 0, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			result.clear();
			args.clear();
			expectedArgs = {"Joe Joe", "9"};
			player.GetInPlayText(result, args);

			// Ensure points are not stored in NVRAM
			player.ResetSession(CUtilities::GetUTCTime()); // Make Session Points 0
			playerInfo->SetPointsAndCountdown(0, 0, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			ASSERT_EQ(expected, result) << result + " vs expected: " + expected;
			ASSERT_EQ(args.size(), expectedArgs.size()) << "Incorrect argument count - case 1";
			ASSERT_EQ(args[0], expectedArgs[0]) << "Incorrect argument value - case 1";
			ASSERT_EQ(args[1], expectedArgs[1]) << "Incorrect argument value - case 1";
		}

		TEST_F(PlayerUnitTests3, PlayerGetInPlayTextCashWithNameDefaultTest)
		{
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();
			
			// Set config to display default 'points in play'
			config.SetDisplayPlayerPtsTotal(false);
			config.SetDisplayCountdownAndSession(false);
			config.SetDisplayGoodLuckAndSession(false);
			config.SetDisplayTotalAndSessionPtsNum(false);
			config.SetDisplayTotalPtsSessionPtsAsCash(false);
			config.SetDisplayGoodLuckAndSessionPts(false);
			config.SetDisplayPtsTotalAsCashSession(true);
			config.SetPointsPerDollar(1);

			// Set config to display name
			config.SetDisplayPlayerName(true);

			string testName("Joe Joe");

			// CPlayerInfo has to be a pointer because CPlayer deletes it on deconstructon
			CPlayerInfo* playerInfo = new CPlayerInfo();
			playerInfo->SetName(testName);
			playerInfo->SetMultiplierDivider(1, 1);
			playerInfo->SetPointsAndCountdown(0, 1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			CPlayer player(config, egmConfig);
			player.ResetSession(CUtilities::GetUTCTime()); // Make Session Points 0
			player.SetPlayerInfo(playerInfo);

			string quote(_T("\""));
			string result;
			string expected = "points_as_cash_with_name";
			vector<string> args;
			vector<string> expectedArgs = { "Joe Joe", "$0.00" };

			player.GetInPlayText(result, args);
			ASSERT_EQ(expected, result) << 
				quote + result + quote + " vs expected(1): " + quote + expected + quote;
			ASSERT_EQ(args.size(), expectedArgs.size()) << "Incorrect argument count - default case";
			ASSERT_EQ(args[0], expectedArgs[0]) << "Incorrect argument value - default case";
			ASSERT_EQ(args[1], expectedArgs[1]) << "Incorrect argument value - default case";

			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 2, true, false);
			player.DecrementCountdown(meters, orig, true); // Make Session Points 1
			delete meters;
			delete orig;
			playerInfo->SetPointsAndCountdown(9, 0, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			result.clear();
			args.clear();
			expectedArgs = {"Joe Joe", "$9.00"};
			player.GetInPlayText(result, args);

			// Ensure points are not stored in NVRAM
			player.ResetSession(CUtilities::GetUTCTime()); // Make Session Points 0
			playerInfo->SetPointsAndCountdown(0, 0, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			ASSERT_EQ(expected, result) << 
				quote + result + quote + " vs expected(2): " + quote + expected + quote;
			ASSERT_EQ(args.size(), expectedArgs.size()) << "Incorrect argument count - case 1";
			ASSERT_EQ(args[0], expectedArgs[0]) << "Incorrect argument value - case 1";
			ASSERT_EQ(args[1], expectedArgs[1]) << "Incorrect argument value - case 1";
		}

		TEST_F(PlayerUnitTests3, PlayerGetInPlayTextSessionPointsCustomTest)
		{
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();
			
			// Set config to display default 'points in play'
			config.SetDisplayPlayerPtsTotal(true);
			config.SetDisplayCountdownAndSession(false);
			config.SetDisplayGoodLuckAndSession(false);
			config.SetDisplayTotalAndSessionPtsNum(false);
			config.SetDisplayTotalPtsSessionPtsAsCash(false);
			config.SetDisplayGoodLuckAndSessionPts(false);
			config.SetDisplayPtsTotalAsCashSession(false);

			// Set config not to display name
			config.SetDisplayPlayerName(false);

			// CPlayerInfo has to be a pointer because CPlayer deletes it on deconstructon
			CPlayerInfo* playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(1, 1);
			playerInfo->SetPointsAndCountdown(0, 1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			CPlayer player(config, egmConfig);
			player.ResetSession(CUtilities::GetUTCTime()); // Make Session Points 0
			player.SetPlayerInfo(playerInfo);

			string result;
			string expected = "points";
			vector<string> args;
			vector<string> expectedArgs = {"0"};

			player.GetInPlayText(result, args);
			ASSERT_EQ(expected, result) << result + " vs expected: " + expected;
			ASSERT_EQ(args.size(), expectedArgs.size()) << "Incorrect argument count - default case";
			ASSERT_EQ(args[0], expectedArgs[0]) << "Incorrect argument value - default case";

			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 2, true, false);
			player.DecrementCountdown(meters, orig, true); // Make Session Points 1
			delete meters;
			delete orig;
			playerInfo->SetPointsAndCountdown(9, 0, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			result.clear();
			args.clear();
			expectedArgs = {"9"};
			player.GetInPlayText(result, args);

			// Ensure points are not stored in NVRAM
			player.ResetSession(CUtilities::GetUTCTime()); // Make Session Points 0
			playerInfo->SetPointsAndCountdown(0, 0, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			ASSERT_EQ(expected, result) << result + " vs expected: " + expected;
			ASSERT_EQ(args.size(), expectedArgs.size()) << "Incorrect argument count - case 1";
			ASSERT_EQ(args[0], expectedArgs[0]) << "Incorrect argument value - case 1";
		}

		TEST_F(PlayerUnitTests3, PlayerGetInPlayTextSessionPointsCustomWithNameTest)
		{
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();
			
			// Set config to display default 'points in play'
			config.SetDisplayPlayerPtsTotal(true);
			config.SetDisplayCountdownAndSession(false);
			config.SetDisplayGoodLuckAndSession(false);
			config.SetDisplayTotalAndSessionPtsNum(false);
			config.SetDisplayTotalPtsSessionPtsAsCash(false);
			config.SetDisplayGoodLuckAndSessionPts(false);
			config.SetDisplayPtsTotalAsCashSession(false);

			// Set config to display name
			config.SetDisplayPlayerName(true);

			string testName("Joe Joe");

			// CPlayerInfo has to be a pointer because CPlayer deletes it on deconstructon
			CPlayerInfo* playerInfo = new CPlayerInfo();
			playerInfo->SetName(testName);
			playerInfo->SetMultiplierDivider(1, 1);
			playerInfo->SetPointsAndCountdown(0, 1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			CPlayer player(config, egmConfig);
			player.ResetSession(CUtilities::GetUTCTime()); // Make Session Points 0
			player.SetPlayerInfo(playerInfo);

			string result;
			string expected = "points_with_name";
			vector<string> args;
			vector<string> expectedArgs = {"Joe Joe", "0"};

			player.GetInPlayText(result, args);
			ASSERT_EQ(expected, result) << result + " vs expected: " + expected;
			ASSERT_EQ(args.size(), expectedArgs.size()) << "Incorrect argument count - default case";
			ASSERT_EQ(args[0], expectedArgs[0]) << "Incorrect argument value - default case";
			ASSERT_EQ(args[1], expectedArgs[1]) << "Incorrect argument value - default case";

			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 2, true, false);
			player.DecrementCountdown(meters, orig, true); // Make Session Points 1
			delete meters;
			delete orig;
			playerInfo->SetPointsAndCountdown(9, 0, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			result.clear();
			args.clear();
			expectedArgs = {"Joe Joe", "9"};
			player.GetInPlayText(result, args);

			// Ensure points are not stored in NVRAM
			player.ResetSession(CUtilities::GetUTCTime()); // Make Session Points 0
			playerInfo->SetPointsAndCountdown(0, 0, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			ASSERT_EQ(expected, result) << result + " vs expected: " + expected;
			ASSERT_EQ(args.size(), expectedArgs.size()) << "Incorrect argument count - case 1";
			ASSERT_EQ(args[0], expectedArgs[0]) << "Incorrect argument value - case 1";
			ASSERT_EQ(args[1], expectedArgs[1]) << "Incorrect argument value - case 1";
		}
}
