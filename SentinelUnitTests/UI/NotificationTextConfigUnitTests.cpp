#include "stdafx.h"
#include "UI/NotificationTextConfig.h"

// Unit testing includes.
#include "UnitTestingUtils.h"
#include "gtest/gtest.h"

using std::string;

namespace SentinelUnitTests
{
	TEST(NotificationTextConfig, GetTextTest)
	{
		// Expected Data Setup
		std::array<std::string, CNotificationTextConfig::NumberOfTextEnums> expectedTexts;
		expectedTexts[CNotificationTextConfig::Idle] = "";
		expectedTexts[CNotificationTextConfig::PleaseInsertCard] = "insert_card";
		expectedTexts[CNotificationTextConfig::BalanceRetrieval] = "balance_retrieval";
		expectedTexts[CNotificationTextConfig::GoodCardRead] = "good_card_read";
		expectedTexts[CNotificationTextConfig::GoodCardRead_InPlay] = "good_card_read_inplay";
		expectedTexts[CNotificationTextConfig::BadCardRead] = "bad_card_read";
		expectedTexts[CNotificationTextConfig::VirtualSession] = "virtual_session";
		expectedTexts[CNotificationTextConfig::BannedFromPlay] = "";
		expectedTexts[CNotificationTextConfig::DownloadPending] = "download_pending";
		expectedTexts[CNotificationTextConfig::SystemBusy] = "system_busy";
		expectedTexts[CNotificationTextConfig::MaxCardInExceeded] = "max_card_in_exceeded";
		expectedTexts[CNotificationTextConfig::MobileSessionAccepted] = "mobile_session_accepted";
		expectedTexts[CNotificationTextConfig::Welcome] = "welcome";
		expectedTexts[CNotificationTextConfig::WelcomeWithName] = "welcome_with_name";
		expectedTexts[CNotificationTextConfig::DailyEarnedPoints] = "daily_earned_points";
		expectedTexts[CNotificationTextConfig::PointsNeededForNextTier] = "points_needed_for_next_tier";
		expectedTexts[CNotificationTextConfig::Comp] = "comp";
		expectedTexts[CNotificationTextConfig::Points] = "points";
		expectedTexts[CNotificationTextConfig::PointsWithName] = "points_with_name";
		expectedTexts[CNotificationTextConfig::PointsAsCash] = "points_as_cash";
		expectedTexts[CNotificationTextConfig::PointsAsCashWithName] = "points_as_cash_with_name";
		expectedTexts[CNotificationTextConfig::TotalPoints_SessionPoints] = "totalpoints_sessionpoints";
		expectedTexts[CNotificationTextConfig::TotalPoints_SessionPoints_WithName] = "totalpoints_sessionpoints_with_name";
		expectedTexts[CNotificationTextConfig::TotalCash_SessionCash] = "totalcash_sessioncash";
		expectedTexts[CNotificationTextConfig::TotalCash_SessionCash_WithName] = "totalcash_sessioncash_with_name";
		expectedTexts[CNotificationTextConfig::Countdown_Session] = "countdown_session";
		expectedTexts[CNotificationTextConfig::Countdown_Session_WithName] = "countdown_session_with_name";
		expectedTexts[CNotificationTextConfig::Goodluck_Session] = "goodluck_session";
		expectedTexts[CNotificationTextConfig::Goodluck_Session_WithName] = "goodluck_session_with_name";
		expectedTexts[CNotificationTextConfig::Goodluck_SessionPoints] = "goodluck_session_points";
		expectedTexts[CNotificationTextConfig::Goodluck_SessionPoints_WithName] = "goodluck_session_points_with_name";
		expectedTexts[CNotificationTextConfig::NoCountdownAvailable] = "no_countdown_available";

		const int minimumIndex(0);
		const int maximumIndex(CNotificationTextConfig::NumberOfTextEnums - 1);

		// Setup.
		CNotificationTextConfig obj;

		// Test
		for (int index = minimumIndex; index <= maximumIndex; index++)
		{
			ASSERT_EQ(obj.GetText((CNotificationTextConfig::TextType)index), expectedTexts[index]) << "Text mismatch for index = " << index;
		}
	}
}
