#include "stdafx.h"
#include "Sentinel.h"
#include "NotificationBox.h"
#include "ScreenDefs.h"

using namespace std;

const int NOTIFICATION_DISPLAY_TIME = 3000;  // in milli-seconds
const DWORD MOBILE_LOAD_BALANCING_TIME_CHECK = 2500;//In milliseconds.
const DWORD MOBILE_CONNECT_ADDITIONAL_WELCOME_MSG_DELAY = 4000;//In milliseconds.

const string TOPIC_NOTIFICATIONBOX("NotificationBox");
const string MOBILE_CONNECT_TOPIC("MobileConnectSubscription");
const DWORD REBOOTED_ALERT_DURATION_MSECS = 5000; // 5 seconds
const WORD DEFAULT_TEXT_TOGGLE_INTERVAL = 3000; // in milli-seconds

CNotificationBox::CNotificationBox(IOperatingSystem &operatingSystem) :
CDataSubscriptionProvider(""),
m_bonusEventPlayerState(MAINTAIN_PLAYER_STATE),
m_downloadPending(false),
m_employeeActive(false),
m_notificationState(NotificationState::IDLE),
m_advanceStateAfterGuestText(false),
m_inPlayMultipleBucketDisplay(false),
m_isRFID(false),
m_notificationStartTickCount(0),
m_prevNotificationState(NotificationState::IDLE),
m_textPriority(CNotificationTextConfig::TextPriority::TEXT_PRIORITY_LAST),
m_isMultipleBucketDisplay(false),
m_startTickCount(0),
m_startTickCountForMobileText(0),
m_operatingSystem(operatingSystem),
m_mobileEnabled(false),
m_mobileState(MobileState::Disconnected),
m_isSubscriptionDataSent(false),
m_alertFlags(0),
m_meterSnapshotPercentage(0),
m_alertPriority(CNotificationTextConfig::TextPriority::TEXT_PRIORITY_LAST),
m_rebootedAlertStartTickCount(0),
m_textToggleIntervalInMilliSecs(DEFAULT_TEXT_TOGGLE_INTERVAL)
{
	AddTopic(TOPIC_NOTIFICATIONBOX);
	AddTopic(MOBILE_CONNECT_TOPIC);
	m_mobileLoadBalancingInProgress = false;
	m_mobileLoadBalancingInProgressStarted = CUtilities::GetTickCount();
	m_mobileConnectStarted = false;

	CalcText();
	CalcAlertText();
}

CNotificationBox::~CNotificationBox(void)
{
}

std::string CNotificationBox::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId)
{
	UNREFERENCED_PARAMETER(&whereId);
	
	lock_guard<recursive_mutex> lock(m_criticalSection);

	string dataResponse;

	if (TOPIC_NOTIFICATIONBOX == topic)
	{
		dataResponse = ResponseForNotificationBox();
	}
	else if(MOBILE_CONNECT_TOPIC == topic)
	{
		dataResponse = ResponseForMobileConnect();
	}
	
	return dataResponse;

}

/// <summary>
/// Determines whether a player is present, for the purpose of queuing and dequeuing player messages.
/// </summary>
/// <returns>true if a player is present, false otherwise</returns>
bool CNotificationBox::IsPlayerPresent() const
{
	bool playerPresent(false);

	switch (m_notificationState)
	{
	case NotificationState::UNCARDED:
	case NotificationState::GUEST_WELCOME:
	case NotificationState::PLAYER_WELCOME:
	case NotificationState::PLAYER_IN_PLAY:
		playerPresent = true;
		break;
	default:
		break;
	}

	return playerPresent;
}

bool CNotificationBox::CheckIfNotificationTextHasChanged()
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	bool changed(false);

	// Check timed events
	if (m_mobileLoadBalancingInProgress)
	{
		bool mobileLoadBalancingCheckExpired(CUtilities::GetTickCount() - m_mobileLoadBalancingInProgressStarted >= MOBILE_LOAD_BALANCING_TIME_CHECK);
		if (mobileLoadBalancingCheckExpired)
		{
			m_mobileLoadBalancingInProgress = false;
		}
	}


	// Timeout display messages even if there is a download pending.
	switch (m_notificationState)
	{
	case NotificationState::GUEST_WELCOME:
		changed = CheckGuestWelcomeTimer();
		break;
	case NotificationState::PLAYER_WELCOME:
		changed = CheckPlayerWelcomeTimer();
		break;
	case NotificationState::BAD_CARD_READ:
	case NotificationState::INELIGIBLE_PLAYER:
		if ((m_notificationState == NotificationState::BAD_CARD_READ || m_isRFID)
			&& m_notificationStartTickCount > 0
			&& CUtilities::GetTickCount() - m_notificationStartTickCount > NOTIFICATION_DISPLAY_TIME)
		{
			// expire bad read message
			m_notificationState = m_prevNotificationState;
			changed = CalcText();
		}
		break;
	default:
		break;
	}

	if (!changed && m_alertFlags && m_isSubscriptionDataSent)
	{
		changed = CheckAlertTimer();
	}

	return changed;
}

bool CNotificationBox::CheckGuestWelcomeTimer()
{
	bool changed(false);

	if (m_advanceStateAfterGuestText || m_guestWelcomeText.size() > 1)
	{
		DWORD extraDisplayTime = m_mobileConnectStarted ? MOBILE_CONNECT_ADDITIONAL_WELCOME_MSG_DELAY : 0;
		DWORD deltaTickCount(m_operatingSystem.GetTickCount32() - m_startTickCount);

		if (deltaTickCount >= (m_uiConfig.GetNotificationForPlayerTrackingTimeInMilliseconds() + extraDisplayTime))
		{
			m_mobileConnectStarted = false;
			m_startTickCount = m_operatingSystem.GetTickCount32();

			if (m_guestWelcomeText.size() > 0)
			{
				m_guestWelcomeText.erase(m_guestWelcomeText.begin());
			}

			if (m_guestWelcomeText.empty())
			{
				m_notificationState = NotificationState::PLAYER_IN_PLAY;
			}

			changed = CalcText();
		}
	}

	return changed;
}

bool CNotificationBox::CheckPlayerWelcomeTimer()
{
	bool changed(false);

	DWORD extraDisplayTime = m_mobileConnectStarted ? MOBILE_CONNECT_ADDITIONAL_WELCOME_MSG_DELAY : 0;
	DWORD deltaTickCount(m_operatingSystem.GetTickCount32() - m_startTickCount);

	if (deltaTickCount >= (m_uiConfig.GetNotificationForPlayerTrackingTimeInMilliseconds() + extraDisplayTime))
	{
		m_mobileConnectStarted = false;
		m_startTickCount = m_operatingSystem.GetTickCount32();

		if (!m_playerWelcomeText.empty())
		{
			m_playerWelcomeText.erase(m_playerWelcomeText.begin());

			if (!m_playerWelcomeTextArgs.empty())
			{
				m_playerWelcomeTextArgs.erase(m_playerWelcomeTextArgs.begin());
			}
		}

		if (m_playerWelcomeText.empty())
		{
			m_notificationState = NotificationState::PLAYER_IN_PLAY;
		}

		changed = CalcText();
	}

	return changed;
}

void CNotificationBox::CheckPlayerMessageState()
{
	if (!IsPlayerPresent())
	{
		if (m_notificationState != NotificationState::BAD_CARD_READ)
		{
			// Bad card read will need to return to previous text when using RFID
			m_inPlayText.clear();
			m_inPlayTextArgs.clear();
		}
	}
}

bool CNotificationBox::CalcText()
{
	std::string oldText(m_text);
	std::vector<std::string> oldTextArgs(m_textArgs);
	bool handleMobile = false;

	m_text.clear();
	m_textArgs.clear();
	m_textPriority = CNotificationTextConfig::TextPriority::TEXT_PRIORITY_LAST;
	m_isMultipleBucketDisplay = false;

	if (m_downloadPending)
	{
		m_text = m_notificationTextConfig.GetText(CNotificationTextConfig::DownloadPending);
	}
	else
	{
		switch (m_notificationState)
		{
		case NotificationState::SYSTEM_BUSY:
			m_text = m_notificationTextConfig.GetText(CNotificationTextConfig::SystemBusy);
			break;
		case NotificationState::BAD_CARD_READ:
			m_text = m_notificationTextConfig.GetText(CNotificationTextConfig::BadCardRead);
			break;
		case NotificationState::VIRTUAL_PLAYER:
			m_text = m_notificationTextConfig.GetText(CNotificationTextConfig::VirtualSession);
			break;
		case NotificationState::INELIGIBLE_PLAYER:
			m_text = m_notificationTextConfig.GetText(CNotificationTextConfig::BannedFromPlay);
			break;
		case NotificationState::MAX_CARD_LIMIT_EXCEEDED:
			m_text = m_notificationTextConfig.GetText(CNotificationTextConfig::MaxCardInExceeded);
			break;
		case NotificationState::GUEST_WELCOME:
			if (!m_guestWelcomeText.empty())
			{
				m_text = m_guestWelcomeText[0];
			}
			break;
		case NotificationState::PLAYER_WELCOME:
			if (!m_playerWelcomeText.empty())
			{
				m_text = m_playerWelcomeText[0];
				m_textArgs = m_playerWelcomeTextArgs[0];
			}
			break;
		case NotificationState::PLAYER_IN_PLAY:
			m_text = m_inPlayText;
			m_textArgs = m_inPlayTextArgs;
			m_isMultipleBucketDisplay = m_inPlayMultipleBucketDisplay;
			break;
		case NotificationState::UNCARDED:
			m_text = m_notificationTextConfig.GetText(CNotificationTextConfig::PleaseInsertCard);
			break;
		case NotificationState::IDLE:
			m_text = m_notificationTextConfig.GetText(CNotificationTextConfig::Idle);
			break;
		default:
			m_text = m_notificationTextConfig.GetText(CNotificationTextConfig::PleaseInsertCard);
			break;
		}
	}

	// update the text priority.
	m_textPriority = m_notificationTextConfig.GetTextPriority(m_text);

	return (oldText != m_text) || (oldTextArgs != m_textArgs);
}

std::string CNotificationBox::GetText()
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	return m_text;
}

bool CNotificationBox::SetDownloadPending(bool downloadPending)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	bool changed(false);
	if (m_downloadPending != downloadPending)
	{
		m_downloadPending = downloadPending;
		changed = CalcText();
	}

	return changed;
}

bool CNotificationBox::SetNotificationState(NotificationState notificationState)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	bool changed(false);

	if (m_notificationState == NotificationState::UNCARDED)
	{
		m_mobileState = MobileState::Disconnected;
	}

	if (m_notificationState != notificationState)
	{
		m_notificationState = notificationState;
		if (m_notificationState != NotificationState::UNCARDED)
		{
			m_mobileText.clear();
		}
		CheckPlayerMessageState();
		changed = CalcText();
	}

	m_notificationStartTickCount = 0;

	return changed;
}

NotificationState CNotificationBox::GetNotificationState() const
{
	return m_notificationState;
}

bool CNotificationBox::KeepNotificationBoxUpFront() const
{
	return NotificationState::GUEST_WELCOME == m_notificationState || NotificationState::PLAYER_WELCOME == m_notificationState;
}

bool CNotificationBox::SetNotificationStateByScreenId(int screenId, bool isCardInserted)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	bool changed(false);

	switch (screenId)
	{
	case INT_MSG_BAD_CARD_READ:
		if (NotificationState::BAD_CARD_READ != m_notificationState)
		{
			m_prevNotificationState = m_notificationState;
		}
		changed = SetNotificationState(NotificationState::BAD_CARD_READ);
		m_notificationStartTickCount = CUtilities::GetTickCount();
		break;
	case INT_MSG_DISPLAY_BANNED_PLAYER:
		if (isCardInserted)
		{
			changed = SetNotificationState(NotificationState::INELIGIBLE_PLAYER);
		}
		else
		{
			if (NotificationState::INELIGIBLE_PLAYER != m_notificationState)
			{
				m_prevNotificationState = m_notificationState;
			}
			changed = SetNotificationState(NotificationState::INELIGIBLE_PLAYER);
			m_notificationStartTickCount = CUtilities::GetTickCount();
		}
		break;
	case INT_MSG_CARDED_SESSION_IN_PROGRESS:
		changed = SetNotificationState(NotificationState::VIRTUAL_PLAYER);
		break;
	case INT_MSG_DOWNLOAD_CANCELED:
		changed = SetDownloadPending(false);
		break;
	case INT_MSG_DISPLAY_DOWNLOAD_PENDING:
		changed = SetDownloadPending(true);
		break;
	default:
		break;
	}

	return changed;
}

bool CNotificationBox::SetGuestWelcome(const vector<std::string>& guestWelcomeText,	bool advanceStateAfterGuestText,
	const string& inPlayText, const std::vector<std::string>& inPlayTextArgs)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	CheckPlayerMessageState();
	m_guestWelcomeText.clear();
	m_guestWelcomeText = guestWelcomeText;
	m_advanceStateAfterGuestText = advanceStateAfterGuestText;
	m_inPlayText = inPlayText;
	m_inPlayTextArgs = inPlayTextArgs;

	m_notificationState = NotificationState::GUEST_WELCOME; // Default.
	if (m_guestWelcomeText.empty() && advanceStateAfterGuestText)
	{
		m_notificationState = NotificationState::PLAYER_IN_PLAY;
	}
	m_startTickCount = m_operatingSystem.GetTickCount32();

	bool changed(CalcText());
	return changed;
}

bool CNotificationBox::SetPlayerWelcome(const vector<std::string>& playerWelcomeText, const std::vector<std::vector<std::string>>& playerWelcomeArgs,
	const string& inPlayText, const std::vector<std::string>& inPlayTextArgs)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	CheckPlayerMessageState();
	m_playerWelcomeText.clear();
	m_playerWelcomeText = playerWelcomeText;
	m_playerWelcomeTextArgs = playerWelcomeArgs;
	m_inPlayText = inPlayText;
	m_inPlayTextArgs = inPlayTextArgs;

	m_notificationState = NotificationState::PLAYER_WELCOME; // Default.
	if (m_playerWelcomeText.empty())
	{
		m_notificationState = NotificationState::PLAYER_IN_PLAY;
	}
	m_startTickCount = m_operatingSystem.GetTickCount32();

	bool changed(CalcText());
	return changed;
}

bool CNotificationBox::SetInPlayText(const string& inPlayText, const std::vector<std::string>& inPlayTextArgs, bool inPlayMultipleBucketDisplay)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	m_inPlayText = inPlayText;
	m_inPlayTextArgs = inPlayTextArgs;
	m_inPlayMultipleBucketDisplay = inPlayMultipleBucketDisplay;

	bool changed(CalcText());
	return changed;
}

bool CNotificationBox::Set4FieldPlayerText(const CPlayer *player)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	bool changed(false);
	if (player != nullptr)
	{
		string inPlayText, sTotalPoints, sSessionPoints;
		vector<string> inPlayTextArgs;
		bool inPlayMultipleBucketDisplay = false;

		DWORD totalPoints = player->GetTotalPoints();
		DWORD sessionPointsCalculated = player->GetSessionPoints();
		DWORD bonusPoints = player->GetBonusPoints();
		DWORD tierMultiplierPoints = player->GetTierMultiplierPoints();

		sTotalPoints = CUtilities::GetFormattedNumberString(totalPoints);
		sSessionPoints = CUtilities::GetFormattedNumberString(sessionPointsCalculated + bonusPoints + tierMultiplierPoints);

		LogString(SESSION_LOG, "%s:%d := Total Points: %u, Session Points calculated: %u, Bonus Points: %u, Tier Multiplier Points: %u, Final Session Points: %s",
			__FUNCTION__, __LINE__, totalPoints, sessionPointsCalculated, bonusPoints, tierMultiplierPoints, sSessionPoints.c_str());

		if (player->GetInPlayDisplayType() == IPDT_TOTAL_SESSION)
		{
			if (player->IsDisplayPlayerName())
			{
				inPlayText = m_notificationTextConfig.GetText(CNotificationTextConfig::TotalPoints_SessionPoints_WithName);
				inPlayTextArgs.push_back(player->GetName());
			}
			else
			{
				inPlayText = m_notificationTextConfig.GetText(CNotificationTextConfig::TotalPoints_SessionPoints);
			}
			inPlayTextArgs.push_back(sTotalPoints);
			inPlayTextArgs.push_back(sSessionPoints);
		}
		else if (player->GetInPlayDisplayType() == IPDT_TOTALCASH_SESSIONCASH)
		{
			if (player->IsDisplayPlayerName())
			{
				inPlayText = m_notificationTextConfig.GetText(CNotificationTextConfig::TotalCash_SessionCash_WithName);
				inPlayTextArgs.push_back(player->GetName());
			}
			else
			{
				inPlayText = m_notificationTextConfig.GetText(CNotificationTextConfig::TotalCash_SessionCash);
			}
			inPlayTextArgs.push_back(player->GetTotalPointsAsCashString());
			inPlayTextArgs.push_back(player->GetSessionPointsAsCashString());
		}
		else if (player->GetInPlayDisplayType() == IPDT_COUNTDOWN_SESSION)
		{
			DWORD countdown = 0;
			DWORD sessionPoints = 0;
			string sBucketName, sBucketCurrency, sBucketCountDown, sBucketSessionPoints;

			if (player->GetCurrentBucketDetails(sBucketName, sBucketCurrency, countdown, sessionPoints))
			{
				string sBucketCountDown = CUtilities::GetFormattedNumberString(countdown);
				string sBucketSessionPoints = CUtilities::GetFormattedNumberString(sessionPoints);

				if (player->IsDisplayPlayerName())
				{
					inPlayText = m_notificationTextConfig.GetText(CNotificationTextConfig::Countdown_Session_WithName);
					inPlayTextArgs.push_back(player->GetName());
				}
				else
				{
					inPlayText = m_notificationTextConfig.GetText(CNotificationTextConfig::Countdown_Session);
				}
				inPlayTextArgs.push_back(sBucketCountDown);
				inPlayTextArgs.push_back(sBucketCurrency + sBucketSessionPoints);
				inPlayTextArgs.push_back(sBucketName);
				inPlayMultipleBucketDisplay = (player->GetBucketCount() > 1) ? true : false;
			}
			else
			{
				inPlayText = m_notificationTextConfig.GetText(CNotificationTextConfig::NoCountdownAvailable);
			}
		}
		else if (player->GetInPlayDisplayType() == IPDT_GOODLUCK_SESSION)
		{
			if (player->IsDisplayPlayerName())
			{
				inPlayText = m_notificationTextConfig.GetText(CNotificationTextConfig::Goodluck_Session_WithName);
				inPlayTextArgs.push_back(player->GetName());
			}
			else
			{
				inPlayText = m_notificationTextConfig.GetText(CNotificationTextConfig::Goodluck_Session);
			}
			inPlayTextArgs.push_back(player->GetSessionPointsAsCashString());
		}
		else if (player->GetInPlayDisplayType() == IPDT_GOODLUCK_SESSIONPTS)
		{
			DWORD countdown = 0;
			DWORD sessionPoints = 0;
			string sBucketName, sBucketCurrency, sBucketSessionPoints;

			if (player->GetCurrentBucketDetails(sBucketName, sBucketCurrency, countdown, sessionPoints))
			{
				string sBucketSessionPoints = CUtilities::GetFormattedNumberString(sessionPoints);

				if (player->IsDisplayPlayerName())
				{
					inPlayText = m_notificationTextConfig.GetText(CNotificationTextConfig::Goodluck_SessionPoints_WithName);
					inPlayTextArgs.push_back(player->GetName());
				}
				else
				{
					inPlayText = m_notificationTextConfig.GetText(CNotificationTextConfig::Goodluck_SessionPoints);
				}
				inPlayTextArgs.push_back(sBucketCurrency + sBucketSessionPoints);
				inPlayTextArgs.push_back(sBucketName);
				inPlayMultipleBucketDisplay = (player->GetBucketCount() > 1) ? true : false;
			}
			else
			{
				inPlayText = m_notificationTextConfig.GetText(CNotificationTextConfig::NoCountdownAvailable);
			}
		}

		changed = SetInPlayText(inPlayText, inPlayTextArgs, inPlayMultipleBucketDisplay);
	}

	return changed;
}

void CNotificationBox::SetUIConfig(const CUIConfig &uiConfig)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	m_uiConfig = uiConfig;
}

void CNotificationBox::SetRFID(bool isRFID)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	m_isRFID = isRFID;
}

void CNotificationBox::SetMobileEnableFlag(bool value)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	if (m_mobileEnabled && !value)
	{
		// We are going from 'mobile connect' enabled to disabled
		// This is likely to be load balancing disconnect/reconnect
		m_mobileLoadBalancingInProgress = true;
		m_mobileLoadBalancingInProgressStarted = CUtilities::GetTickCount();
	}
	else
	{
		m_mobileLoadBalancingInProgress = false;
	}

	m_mobileEnabled = value;
}

bool CNotificationBox::IsMobileEnabled() const
{
	return m_mobileEnabled;
}

void CNotificationBox::SetMobileState(MobileState mobileState, const std::string& pairingCode)
{
	m_mobileState = mobileState;
	m_mobilePairingCode = pairingCode;

	if (m_mobileState == MobileState::Connected)
	{
		m_mobileConnectStarted = true;
	}
}

bool CNotificationBox::WaitingForMobileConnection() const
{
	return (MobileState::PleaseWait == m_mobileState || MobileState::PleaseEnterCode == m_mobileState);
}

void CNotificationBox::SetEmployeeCardFlag(bool value)
{
	m_employeeActive = value;
}

bool CNotificationBox::AddAlert(Alerts alert, WORD percentage)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	m_alertFlags = m_alertFlags | alert;
	m_meterSnapshotPercentage = percentage;

	return CalcAlertText();
}

bool CNotificationBox::RemoveAlert(Alerts alert)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	m_alertFlags = m_alertFlags & (~alert);

	return CalcAlertText();
}

void CNotificationBox::SetTextToggleInterval(WORD intervalInMilliSecs)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	m_textToggleIntervalInMilliSecs = intervalInMilliSecs;
}

bool CNotificationBox::CalcAlertText()
{
	std::string oldAlertText(m_alertText);
	std::vector<std::string> oldAlertTextArgs(m_alertTextArgs);

	m_alertText.clear();
	m_alertTextArgs.clear();
	m_alertPriority = CNotificationTextConfig::TextPriority::TEXT_PRIORITY_LAST;

	if (m_alertFlags & Alerts::ALERT_METER_SNAPSHOT_IN_PROGRESS)
	{
		m_alertText = "meter_snapshot_in_progress";
		m_alertTextArgs.push_back(std::to_string(m_meterSnapshotPercentage));
		m_alertPriority = CNotificationTextConfig::TextPriority::TEXT_PRIORITY_HIGH;
	}
	else if (m_alertFlags & Alerts::ALERT_REBOOT_IN_PROGRESS)
	{
		m_alertText = "reboot_in_progress";
		m_alertPriority = CNotificationTextConfig::TextPriority::TEXT_PRIORITY_HIGH;
	}
	else if (m_alertFlags & Alerts::ALERT_REBOOTED_MEMORY_ERROR)
	{
		m_alertText = "rebooted_memory_error";
		m_alertPriority = CNotificationTextConfig::TextPriority::TEXT_PRIORITY_HIGH;
	}
	else if (m_alertFlags & Alerts::ALERT_REBOOTED_MEMORY_OK)
	{
		m_alertText = "rebooted_memory_ok";
		m_alertPriority = CNotificationTextConfig::TextPriority::TEXT_PRIORITY_HIGH;
	}
	else if (m_alertFlags & Alerts::ALERT_PROGRESSIVE_DOWN)
	{
		m_alertText = "progressive_down";
		m_alertPriority = CNotificationTextConfig::TextPriority::TEXT_PRIORITY_LOW;
	}
	else if (m_alertFlags & Alerts::ALERT_PROGRESSIVE_INELIGIBILITY)
	{
		m_alertText = "progressive_ineligibility";
		m_alertPriority = CNotificationTextConfig::TextPriority::TEXT_PRIORITY_LOW;
	}
	else if (m_alertFlags & Alerts::ALERT_SYSTEM_BUSY)
	{
		m_alertText = "system_busy";
		m_alertPriority = CNotificationTextConfig::TextPriority::TEXT_PRIORITY_LOW;
	}

	return m_alertText != oldAlertText || m_alertTextArgs != oldAlertTextArgs;
}

bool CNotificationBox::CheckAlertTimer()
{
	bool changed(false);

	if (m_alertFlags & Alerts::ALERT_REBOOTED_MEMORY_OK)
	{
		DWORD deltaTickCount(m_operatingSystem.GetTickCount32() - m_rebootedAlertStartTickCount);
		if (deltaTickCount >= REBOOTED_ALERT_DURATION_MSECS)
		{
			RemoveAlert(Alerts::ALERT_REBOOTED_MEMORY_OK);
			changed = true;
		}
	}
	else if (m_alertFlags & Alerts::ALERT_REBOOTED_MEMORY_ERROR)
	{
		DWORD deltaTickCount(m_operatingSystem.GetTickCount32() - m_rebootedAlertStartTickCount);
		if (deltaTickCount >= REBOOTED_ALERT_DURATION_MSECS)
		{
			RemoveAlert(Alerts::ALERT_REBOOTED_MEMORY_ERROR);
			changed = true;
		}
	}

	return changed;
}

string CNotificationBox::ResponseForNotificationBox()
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	CalcText();
	string text(m_text);
	vector<string> textArgs(m_textArgs);
	WORD textPriority(m_textPriority);

	CalcAlertText();
	WORD alertFlags = m_alertFlags;
	string alertText(m_alertText);
	vector<string> alertTextArgs(m_alertTextArgs);
	WORD alertPriority(m_alertPriority);

	writer.StartObject();
	writer.Key("NotificationText");			writer.String(text);
	writer.Key("NotificationTextArgs");
	{
		writer.StartArray();
		for (int index = 0; index < textArgs.size(); index++)
		{
			writer.String(textArgs[index]);
		}
		writer.EndArray();
	}
	writer.Key("NotificationTextPriority");	writer.String(text.empty() ? "" : std::to_string(textPriority));

	writer.Key("AlertText");				writer.String(alertText);
	writer.Key("AlertTextArgs");
	{
		writer.StartArray();
		for (int index = 0; index < alertTextArgs.size(); index++)
		{
			writer.String(alertTextArgs[index]);
		}
		writer.EndArray();
	}
	writer.Key("AlertTextPriority");				writer.String(alertText.empty() ? "" : std::to_string(alertPriority));
	writer.Key("ShowAlertUntilNewNotification");	writer.Bool(m_isMultipleBucketDisplay);
	writer.Key("TextToggleIntervalInMilliSecs");	writer.Int(m_textToggleIntervalInMilliSecs);
	writer.EndObject();

	if (!m_isSubscriptionDataSent &&
		(m_alertFlags & Alerts::ALERT_REBOOTED_MEMORY_OK || m_alertFlags & Alerts::ALERT_REBOOTED_MEMORY_ERROR))
	{
		m_rebootedAlertStartTickCount = m_operatingSystem.GetTickCount32();
		m_isSubscriptionDataSent = true;
	}

	return writeStream.m_str;
}

string CNotificationBox::ResponseForMobileConnect() const
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key("IsMobileConnectEnabled");
	writer.Bool(m_mobileEnabled || m_mobileLoadBalancingInProgress);
	writer.Key("State");
	writer.Int((int)m_mobileState);
	writer.Key("Code");
	writer.String(m_mobilePairingCode);
	writer.EndObject();

	return writeStream.m_str;
}
