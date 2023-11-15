#include "stdafx.h"
#include "UIConfig.h"
#include "ConfigExtData.h"

CUIConfig::CUIConfig() :
	m_notificationForShowPromotionTime(CConfigExtData::DEFAULT_NOTIFICATION_SHOW_PROMOTION),
	m_notificationForPlayerTrackingTime(CConfigExtData::DEFAULT_NOTIFICATION_PLAYER_TRACKING),
	m_resortMenuButtonCount(CConfigExtData::DEFAULT_RESORT_MENU_BUTTON_COUNT),
	m_customButton1Enabled(CConfigExtData::DEFAULT_CUSTOM_BUTTON_1_ENABLE),
	m_customButton2Enabled(CConfigExtData::DEFAULT_CUSTOM_BUTTON_2_ENABLE),
	m_customMenuButton1Count(CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_1_COUNT),
	m_customMenuButton2Count(CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_2_COUNT),
	m_mobileEnabled(false),
	m_bonusCounterNotificationPriority(CConfigExtData::DEFAULT_NOTIFICATION_BOX_PRIORITY),
	m_bonusCounternRichLevelPriority(CConfigExtData::DEFAULT_NRICH_PRIORITY),
	m_bonusCounterSplashdownPriority(CConfigExtData::DEFAULT_SPLASHDOWN_PRIORITY),
	m_bonusCounterProgressiveOdometerPriority(CConfigExtData::DEFAULT_PROGRESSIVE_ODOMETER_PRIORITY),
	m_bonusCounterPtePriority(CConfigExtData::DEFAULT_PTE_PRIORITY),
	m_bonusCounterScatterCountdownTimerPriority(CConfigExtData::DEFAULT_SCATTER_COUNTDOWN_TIMER_PRIORITY),
	m_bonusCounterPrizePriority(CConfigExtData::DEFAULT_PRIZE_ODOMETER_PRIORITY),
	m_bonusCounterPlayXGetYPriority(CConfigExtData::DEFAULT_PLAYXGETY_PRIORITY)
{
}

CUIConfig::CUIConfig(const CConfig& config, bool mobileEnabled) :
m_notificationForShowPromotionTime(config.GetNotificationForShowPromotionTime()),
m_notificationForPlayerTrackingTime(config.GetNotificationForPlayerTrackingTime()),
m_resortMenuButtonCount(config.GetResortMenuButtonCount()),
m_customButton1Enabled(config.GetCustomButton1Enabled()),
m_customButton2Enabled(config.GetCustomButton2Enabled()),
m_customMenuButton1Count(config.GetCustomMenuButton1Count()),
m_customMenuButton2Count(config.GetCustomMenuButton2Count()),
m_mobileEnabled(mobileEnabled),
m_bonusCounterNotificationPriority(config.GetBonusCounterNotificationPriority()),
m_bonusCounternRichLevelPriority(config.GetBonusCounternRichLevelPriority()),
m_bonusCounterSplashdownPriority(config.GetBonusCounterSplashdownPriority()),
m_bonusCounterProgressiveOdometerPriority(config.GetBonusCounterProgressiveOdometerPriority()),
m_bonusCounterPtePriority(config.GetBonusCounterPtePriority()),
m_bonusCounterScatterCountdownTimerPriority(config.GetBonusCounterScatterCountdownTimerPriority()),
m_bonusCounterPrizePriority(config.GetBonusCounterPrizePriority()),
m_bonusCounterPlayXGetYPriority(config.GetBonusCounterPlayXGetYPriority())
{
}

CUIConfig::~CUIConfig()
{
}

WORD CUIConfig::GetNotificationForShowPromotionTimeInSeconds() const
{
	return m_notificationForShowPromotionTime;
}

DWORD CUIConfig::GetNotificationForShowPromotionTimeInMilliseconds() const
{
	return m_notificationForShowPromotionTime * (DWORD)MILLISECONDS_PER_SECOND;
}

WORD CUIConfig::GetNotificationForPlayerTrackingTimeInSeconds() const
{
	return m_notificationForPlayerTrackingTime;
}

DWORD CUIConfig::GetNotificationForPlayerTrackingTimeInMilliseconds() const
{
	return m_notificationForPlayerTrackingTime * (DWORD)MILLISECONDS_PER_SECOND;
}

WORD CUIConfig::GetResortMenuButtonCount() const
{
	return m_resortMenuButtonCount;
}

bool CUIConfig::GetCustomButton1Enabled() const
{
	return m_customButton1Enabled;
}

bool CUIConfig::GetCustomButton2Enabled() const
{
	return m_customButton2Enabled;
}

WORD CUIConfig::GetCustomMenuButton1Count() const
{
	return m_customMenuButton1Count;
}

WORD CUIConfig::GetCustomMenuButton2Count() const
{
	return m_customMenuButton2Count;
}

bool CUIConfig::IsMobileEnabled() const
{
	return m_mobileEnabled;
}

WORD CUIConfig::GetBonusCounterNotificationPriority() const
{
	return m_bonusCounterNotificationPriority;
}

WORD CUIConfig::GetBonusCounternRichLevelPriority() const
{
	return m_bonusCounternRichLevelPriority;
}

WORD CUIConfig::GetBonusCounterSplashdownPriority() const
{
	return m_bonusCounterSplashdownPriority;
}

WORD CUIConfig::GetBonusCounterProgressiveOdometerPriority() const
{
	return m_bonusCounterProgressiveOdometerPriority;
}

WORD CUIConfig::GetBonusCounterPtePriority() const
{
	return m_bonusCounterPtePriority;
}

WORD CUIConfig::GetBonusCounterScatterCountdownTimerPriority() const
{
	return m_bonusCounterScatterCountdownTimerPriority;
}

WORD CUIConfig::GetBonusCounterPrizePriority() const
{
	return m_bonusCounterPrizePriority;
}

WORD CUIConfig::GetBonusCounterPlayXGetYPriority() const
{
	return m_bonusCounterPlayXGetYPriority;
}

void CUIConfig::SetResortMenuButtonCount(WORD buttonCount)
{
	m_resortMenuButtonCount = buttonCount;
}

void CUIConfig::SetCustomButton1Enabled(bool enable)
{
	m_customButton1Enabled = enable;
}

void CUIConfig::SetCustomButton2Enabled(bool enable)
{
	m_customButton2Enabled = enable;
}

void CUIConfig::SetCustomMenuButton1Count(WORD buttonCount)
{
	m_customMenuButton1Count = buttonCount;
}

void CUIConfig::SetCustomMenuButton2Count(WORD buttonCount)
{
	m_customMenuButton2Count = buttonCount;
}

