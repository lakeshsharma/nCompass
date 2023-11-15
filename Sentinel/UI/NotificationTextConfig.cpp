#include "stdafx.h"
#include "NotificationTextConfig.h"

using namespace std;

CNotificationTextConfig::CNotificationTextConfig()
{
	m_texts[TextType::Idle] = "";
	m_texts[TextType::PleaseInsertCard] = "insert_card";
	m_texts[TextType::BalanceRetrieval] = "balance_retrieval";
	m_texts[TextType::GoodCardRead] = "good_card_read";
	m_texts[TextType::GoodCardRead_InPlay] = "good_card_read_inplay";
	m_texts[TextType::BadCardRead] = "bad_card_read";

	m_texts[TextType::VirtualSession] = "virtual_session";
	m_texts[TextType::BannedFromPlay] = "";
	m_texts[TextType::DownloadPending] = "download_pending";
	m_texts[TextType::SystemBusy] = "system_busy";
	m_texts[TextType::MaxCardInExceeded] = "max_card_in_exceeded";

	m_texts[TextType::MobileSessionAccepted] = "mobile_session_accepted";

	m_texts[TextType::Welcome] = "welcome";
	m_texts[TextType::WelcomeWithName] = "welcome_with_name";

	m_texts[TextType::DailyEarnedPoints] = "daily_earned_points";
	m_texts[TextType::PointsNeededForNextTier] = "points_needed_for_next_tier";

	m_texts[TextType::Comp] = "comp";

	m_texts[TextType::Points] = "points";
	m_texts[TextType::PointsWithName] = "points_with_name";
	m_texts[TextType::PointsAsCash] = "points_as_cash";
	m_texts[TextType::PointsAsCashWithName] = "points_as_cash_with_name";

	m_texts[TextType::TotalPoints_SessionPoints] = "totalpoints_sessionpoints";
	m_texts[TextType::TotalPoints_SessionPoints_WithName] = "totalpoints_sessionpoints_with_name";
	m_texts[TextType::TotalCash_SessionCash] = "totalcash_sessioncash";
	m_texts[TextType::TotalCash_SessionCash_WithName] = "totalcash_sessioncash_with_name";

	m_texts[TextType::Countdown_Session] = "countdown_session";
	m_texts[TextType::Countdown_Session_WithName] = "countdown_session_with_name";

	m_texts[TextType::Goodluck_Session] = "goodluck_session";
	m_texts[TextType::Goodluck_Session_WithName] = "goodluck_session_with_name";
	m_texts[TextType::Goodluck_SessionPoints] = "goodluck_session_points";
	m_texts[TextType::Goodluck_SessionPoints_WithName] = "goodluck_session_points_with_name";

	m_texts[TextType::NoCountdownAvailable] = "no_countdown_available";

	GenerateTextPriorityMap();
}

CNotificationTextConfig::CNotificationTextConfig(const CNotificationTextConfig &rhs)
{
	*this = rhs;
}

CNotificationTextConfig::~CNotificationTextConfig()
{
}

string CNotificationTextConfig::GetText(TextType index) const
{
	return m_texts[index];
}

CNotificationTextConfig::TextPriority CNotificationTextConfig::GetTextPriority(const std::string text) const
{
	TextPriority priority = TextPriority::TEXT_PRIORITY_LAST;
	std::map<std::string, TextPriority>::const_iterator itr = m_textPriorityMap.find(text);
	if (itr != m_textPriorityMap.end())
	{
		priority = itr->second;
	}
	return priority;
}

void CNotificationTextConfig::GenerateTextPriorityMap()
{
	m_textPriorityMap[m_texts[TextType::Idle]] = TextPriority::TEXT_PRIORITY_NORMAL;
	m_textPriorityMap[m_texts[TextType::PleaseInsertCard]] = TextPriority::TEXT_PRIORITY_LOW;
	m_textPriorityMap[m_texts[TextType::BalanceRetrieval]] = TextPriority::TEXT_PRIORITY_LOW;
	m_textPriorityMap[m_texts[TextType::GoodCardRead]] = TextPriority::TEXT_PRIORITY_NORMAL;
	m_textPriorityMap[m_texts[TextType::GoodCardRead_InPlay]] = TextPriority::TEXT_PRIORITY_LOW;
	m_textPriorityMap[m_texts[TextType::BadCardRead]] = TextPriority::TEXT_PRIORITY_NORMAL;

	m_textPriorityMap[m_texts[TextType::VirtualSession]] = TextPriority::TEXT_PRIORITY_LOW;
	m_textPriorityMap[m_texts[TextType::BannedFromPlay]] = TextPriority::TEXT_PRIORITY_NORMAL;
	m_textPriorityMap[m_texts[TextType::DownloadPending]] = TextPriority::TEXT_PRIORITY_NORMAL;
	m_textPriorityMap[m_texts[TextType::SystemBusy]] = TextPriority::TEXT_PRIORITY_NORMAL;
	m_textPriorityMap[m_texts[TextType::MaxCardInExceeded]] = TextPriority::TEXT_PRIORITY_NORMAL;

	m_textPriorityMap[m_texts[TextType::MobileSessionAccepted]] = TextPriority::TEXT_PRIORITY_NORMAL;

	m_textPriorityMap[m_texts[TextType::Welcome]] = TextPriority::TEXT_PRIORITY_NORMAL;
	m_textPriorityMap[m_texts[TextType::WelcomeWithName]] = TextPriority::TEXT_PRIORITY_NORMAL;

	m_textPriorityMap[m_texts[TextType::DailyEarnedPoints]] = TextPriority::TEXT_PRIORITY_NORMAL;
	m_textPriorityMap[m_texts[TextType::PointsNeededForNextTier]] = TextPriority::TEXT_PRIORITY_NORMAL;

	m_textPriorityMap[m_texts[TextType::Comp]] = TextPriority::TEXT_PRIORITY_NORMAL;

	m_textPriorityMap[m_texts[TextType::Points]] = TextPriority::TEXT_PRIORITY_NORMAL;
	m_textPriorityMap[m_texts[TextType::PointsWithName]] = TextPriority::TEXT_PRIORITY_NORMAL;
	m_textPriorityMap[m_texts[TextType::PointsAsCash]] = TextPriority::TEXT_PRIORITY_NORMAL;
	m_textPriorityMap[m_texts[TextType::PointsAsCashWithName]] = TextPriority::TEXT_PRIORITY_NORMAL;

	m_textPriorityMap[m_texts[TextType::TotalPoints_SessionPoints]] = TextPriority::TEXT_PRIORITY_LOW;
	m_textPriorityMap[m_texts[TextType::TotalPoints_SessionPoints_WithName]] = TextPriority::TEXT_PRIORITY_LOW;
	m_textPriorityMap[m_texts[TextType::TotalCash_SessionCash]] = TextPriority::TEXT_PRIORITY_LOW;
	m_textPriorityMap[m_texts[TextType::TotalCash_SessionCash_WithName]] = TextPriority::TEXT_PRIORITY_LOW;

	m_textPriorityMap[m_texts[TextType::Countdown_Session]] = TextPriority::TEXT_PRIORITY_LOW;
	m_textPriorityMap[m_texts[TextType::Countdown_Session_WithName]] = TextPriority::TEXT_PRIORITY_LOW;

	m_textPriorityMap[m_texts[TextType::Goodluck_Session]] = TextPriority::TEXT_PRIORITY_LOW;
	m_textPriorityMap[m_texts[TextType::Goodluck_Session_WithName]] = TextPriority::TEXT_PRIORITY_LOW;
	m_textPriorityMap[m_texts[TextType::Goodluck_SessionPoints]] = TextPriority::TEXT_PRIORITY_LOW;
	m_textPriorityMap[m_texts[TextType::Goodluck_SessionPoints_WithName]] = TextPriority::TEXT_PRIORITY_LOW;

	m_textPriorityMap[m_texts[TextType::NoCountdownAvailable]] = TextPriority::TEXT_PRIORITY_LOW;
}
