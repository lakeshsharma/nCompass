#include "stdafx.h"
#include "ConfigExtData.h"

const LPCTSTR CConfigExtData::NEGLIGIBLE_CREDITS_THRESHOLD(_T("NegligibleCreditsThreshold"));
const LPCTSTR CConfigExtData::EMPLOYEE_OVER_PLAYER_IDLE_CARD_OUT(_T("EmployeeOverPlayerIdleCardOut"));
const LPCTSTR CConfigExtData::EMPLOYEE_IDLE_CARD_OUT_SECONDS(_T("EmployeeIdleCardOutSeconds"));
const LPCTSTR CConfigExtData::PLAYER_RECARD_EMPLOYEE_CARD_OUT(_T("PlayerRecardEmployeeCardOut"));
const LPCTSTR CConfigExtData::SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS(_T("SessionActiveNegligibleCreditsTimeoutSeconds"));
const LPCTSTR CConfigExtData::SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS(_T("SessionActiveNonNegligibleCreditsTimeoutSeconds"));

const LPCTSTR CConfigExtData::NOTIFICATION_SHOW_PROMOTION_TIME(_T("NotificationShowPromotionTime"));
const LPCTSTR CConfigExtData::NOTIFICATION_PLAYER_TRACKING_TIME(_T("NotificationPlayerTrackingTime"));

const LPCTSTR CConfigExtData::RESORT_MENU_BUTTON_COUNT(_T("ResortMenuButtonCount"));
const LPCTSTR CConfigExtData::CUSTOM_BUTTON_1_ENABLE(_T("EnableCustom1Button"));
const LPCTSTR CConfigExtData::CUSTOM_BUTTON_2_ENABLE(_T("EnableCustom2Button"));
const LPCTSTR CConfigExtData::CUSTOM_MENU_BUTTON_1_COUNT(_T("Custom1MenuButtonCount"));
const LPCTSTR CConfigExtData::CUSTOM_MENU_BUTTON_2_COUNT(_T("Custom2MenuButtonCount"));

const LPCTSTR CConfigExtData::REBOOT_UI_RESTART_OVERRIDE_ENABLE(_T("RebootUiRestartOverride"));
const LPCTSTR CConfigExtData::GAME_LIST_CHUNK_SIZE(_T("GameListChunkSize"));

const LPCTSTR CConfigExtData::BONUS_COUNTER_NRICH_LEVEL_PRIORITY(_T("BonusCounterNRichLevelPriority"));
const LPCTSTR CConfigExtData::BONUS_COUNTER_SPLASHDOWN_PRIORITY(_T("BonusCounterSplashdownPriority"));
const LPCTSTR CConfigExtData::BONUS_COUNTER_PROGRESSIVE_ODOMETER_PRIORITY(_T("BonusCounterProgressiveOdometerPriority"));
const LPCTSTR CConfigExtData::BONUS_COUNTER_PTE_PRIORITY(_T("BonusCounterPtePriority"));
const LPCTSTR CConfigExtData::BONUS_COUNTER_SCATTER_COUNTDOWN_TIMER_PRIORITY(_T("BonusCounterScatterCountdownTimerPriority"));
const LPCTSTR CConfigExtData::BONUS_COUNTER_PRIZE_PRIORITY(_T("BonusCounterPrizePriority"));
const LPCTSTR CConfigExtData::BONUS_COUNTER_NOTIFICATION_BOX_PRIORITY(_T("BonusCounterNotificationPriority"));
const LPCTSTR CConfigExtData::BONUS_COUNTER_PLAYXGETY_PRIORITY(_T("BonusCounterPlayXGetYPriority"));

CConfigExtData::CConfigExtData()
{
	FillDefaults();
}


CConfigExtData::~CConfigExtData()
{
}

void CConfigExtData::FillDefaults()
{
	m_boolConfigMap.clear();
	m_wordConfigMap.clear();

	AddConfig(EMPLOYEE_OVER_PLAYER_IDLE_CARD_OUT, DEFAULT_EMPLOYEE_OVER_PLAYER_IDLE_CARD_OUT);
	AddConfig(PLAYER_RECARD_EMPLOYEE_CARD_OUT, DEFAULT_PLAYER_RECARD_EMPLOYEE_CARD_OUT);
	AddConfig(CUSTOM_BUTTON_1_ENABLE, DEFAULT_CUSTOM_BUTTON_1_ENABLE);
	AddConfig(CUSTOM_BUTTON_2_ENABLE, DEFAULT_CUSTOM_BUTTON_2_ENABLE);
	
	AddConfig(GAME_LIST_CHUNK_SIZE, DEFAULT_GAME_LIST_CHUNK_SIZE, MINIMUM_GAME_LIST_CHUNK_SIZE, MAXIMUM_GAME_LIST_CHUNK_SIZE);
	AddConfig(NEGLIGIBLE_CREDITS_THRESHOLD, DEFAULT_NEGLIGIBLE_CREDITS_THRESHOLD, 0, WORD_MAX);
	AddConfig(EMPLOYEE_IDLE_CARD_OUT_SECONDS, DEFAULT_EMPLOYEE_IDLE_CARD_OUT_SECONDS, 0, WORD_MAX);
	AddConfig(SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		DEFAULT_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		MINIMUM_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		MAXIMUM_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
	AddConfig(SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		DEFAULT_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		MINIMUM_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		MAXIMUM_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);

	AddConfig(NOTIFICATION_SHOW_PROMOTION_TIME,
		DEFAULT_NOTIFICATION_SHOW_PROMOTION,
		MINIMUM_NOTIFICATION_SHOW_PROMOTION,
		MAXIMUM_NOTIFICATION_SHOW_PROMOTION);
	AddConfig(NOTIFICATION_PLAYER_TRACKING_TIME,
		DEFAULT_NOTIFICATION_PLAYER_TRACKING,
		MINIMUM_NOTIFICATION_PLAYER_TRACKING,
		MAXIMUM_NOTIFICATION_PLAYER_TRACKING);	
	AddConfig(RESORT_MENU_BUTTON_COUNT,
		DEFAULT_RESORT_MENU_BUTTON_COUNT,
		MINIMUM_RESORT_MENU_BUTTON_COUNT,
		MAXIMUM_RESORT_MENU_BUTTON_COUNT);
	AddConfig(CUSTOM_MENU_BUTTON_1_COUNT,
		DEFAULT_CUSTOM_MENU_BUTTON_1_COUNT,
		MINIMUM_CUSTOM_MENU_BUTTON_1_COUNT,
		MAXIMUM_CUSTOM_MENU_BUTTON_1_COUNT);
	AddConfig(CUSTOM_MENU_BUTTON_2_COUNT,
		DEFAULT_CUSTOM_MENU_BUTTON_2_COUNT,
		MINIMUM_CUSTOM_MENU_BUTTON_2_COUNT,
		MAXIMUM_CUSTOM_MENU_BUTTON_2_COUNT);

	AddConfig(REBOOT_UI_RESTART_OVERRIDE_ENABLE, DEFAULT_REBOOT_UI_RESTART_OVERRIDE_ENABLE);

	AddConfig(BONUS_COUNTER_NRICH_LEVEL_PRIORITY, DEFAULT_NRICH_PRIORITY, 0, WORD_MAX);
	AddConfig(BONUS_COUNTER_SPLASHDOWN_PRIORITY, DEFAULT_SPLASHDOWN_PRIORITY, 0, WORD_MAX);
	AddConfig(BONUS_COUNTER_PROGRESSIVE_ODOMETER_PRIORITY, DEFAULT_PROGRESSIVE_ODOMETER_PRIORITY, 0, WORD_MAX);
	AddConfig(BONUS_COUNTER_NOTIFICATION_BOX_PRIORITY, DEFAULT_NOTIFICATION_BOX_PRIORITY, 0, WORD_MAX);
	AddConfig(BONUS_COUNTER_PLAYXGETY_PRIORITY, DEFAULT_PTE_PRIORITY, 0, WORD_MAX);
	AddConfig(BONUS_COUNTER_PTE_PRIORITY, DEFAULT_PTE_PRIORITY, 0, WORD_MAX);
	AddConfig(BONUS_COUNTER_SCATTER_COUNTDOWN_TIMER_PRIORITY, DEFAULT_SCATTER_COUNTDOWN_TIMER_PRIORITY, 0, WORD_MAX);
	AddConfig(BONUS_COUNTER_PRIZE_PRIORITY, DEFAULT_PRIZE_ODOMETER_PRIORITY, 0, WORD_MAX);
}

CConfigExtData& CConfigExtData::operator=(const CConfigExtData &configExtData)
{
	if (this != &configExtData)
	{
		m_boolConfigMap = configExtData.m_boolConfigMap;
		m_wordConfigMap = configExtData.m_wordConfigMap;
	}

	return *this;
}

bool CConfigExtData::HasConfigNameBool(LPCTSTR name)
{
	return m_boolConfigMap.find(name) != m_boolConfigMap.end();
}

bool CConfigExtData::HasConfigNameWORD(LPCTSTR name)
{
	return m_wordConfigMap.find(name) != m_wordConfigMap.end();
}

ConfigBoolMap CConfigExtData::GetBoolConfigs() const
{
	return m_boolConfigMap;
}

ConfigWordMap CConfigExtData::GetWordConfigs() const
{
	return m_wordConfigMap;
}


void CConfigExtData::AddConfig(LPCTSTR name, bool bDefault)
{
	ConfigExtFileBoolInfo boolInfo;
	boolInfo.name = name;
	boolInfo.value = bDefault;
	boolInfo.setFromFile = false;
	m_boolConfigMap.insert(BoolKeyPair(name, boolInfo));
}

void CConfigExtData::AddConfig(LPCTSTR name, WORD wDefault, WORD minimumValue, WORD maximumValue)
{
	ConfigExtFileWORDInfo wordInfo;
	wordInfo.name = name;
	wordInfo.value = wDefault;
	wordInfo.m_defaultValue = wDefault;
	wordInfo.m_minimumValue = minimumValue;
	wordInfo.m_maximumValue = maximumValue;
	wordInfo.setFromFile = false;
	m_wordConfigMap.insert(WORDKeyPair(name, wordInfo));

}

bool CConfigExtData::GetConfigValue(LPCTSTR name, bool &bOut) const
{
	if (m_boolConfigMap.find(name) != m_boolConfigMap.end())
	{
		bOut = m_boolConfigMap.find(name)->second.value;
		return m_boolConfigMap.find(name)->second.setFromFile;
	}
	return false;
}

bool CConfigExtData::GetConfigValue(LPCTSTR name, WORD &wOut) const
{
	if (m_wordConfigMap.find(name) != m_wordConfigMap.end())
	{
		wOut = m_wordConfigMap.find(name)->second.value;
		return m_wordConfigMap.find(name)->second.setFromFile;
	}
	return false;
}

void CConfigExtData::SetConfigValue(LPCTSTR name, bool value, bool fromFile)
{
	if (m_boolConfigMap.find(name) != m_boolConfigMap.end())
	{
		m_boolConfigMap.find(name)->second.value = value;
		m_boolConfigMap.find(name)->second.setFromFile = fromFile;
	}
}

void CConfigExtData::SetConfigValue(LPCTSTR name, WORD value, bool fromFile)
{
	ConfigWordMap::iterator it(m_wordConfigMap.find(name));
	if (it != m_wordConfigMap.end())
	{
		if (it->second.m_minimumValue <= value && value <= it->second.m_maximumValue)
		{
			it->second.value = value;
		}
		else
		{
			it->second.value = it->second.m_defaultValue;
		}
		it->second.setFromFile = fromFile;
	}

}
