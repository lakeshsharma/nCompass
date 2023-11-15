#include "stdafx.h"
#include "ConfigExtFile.h"

// Sentinel headers.
#include "DirectoryManager.h"
#include "Logging/LogString.h"

using namespace std;

// #pragma region common members
CConfigExtFile::CConfigExtFile(IOperatingSystem &operatingSystem) :
CConfigFile(0, operatingSystem)
{
	SetDefaults();
}


CConfigExtFile::~CConfigExtFile()
{
}
// #pragma endregion common members



// #pragma region configuration file members
const LPCTSTR CConfigExtFile::CONFIG_FILENAME("/ConfigExt.txt");

void CConfigExtFile::SetDefaults()
{
	FillConfigMaps();
}

void CConfigExtFile::FillConfigMaps()
{
	m_configData.FillDefaults();
}

bool CConfigExtFile::Start()
{
	LogString(POLLER_LOG, "%s: %d\n", __FUNCTION__, __LINE__);
	bool success(LoadConfigFile(CONFIG_FILENAME));
	return success;
}

bool CConfigExtFile::InterpretNameValue(const std::string& name, const std::string& value)
{
	bool ok(false);

	if (m_configData.HasConfigNameBool(name.c_str()))
	{
		bool interpretted;
		ok = InterpretBool(value, interpretted);
		if (ok)
		{
			m_configData.SetConfigValue(name.c_str(), interpretted, true);
		}
	}
	else if (m_configData.HasConfigNameWORD(name.c_str()))
	{
		WORD wInterpretted;
		ok = InterpretWORD(value, wInterpretted);
		
		LogString(POLLER_LOG, "%s %d: Key-%s, Value-%u\n", __FUNCTION__, __LINE__, name.c_str(), wInterpretted);
		if (ok)
		{
			m_configData.SetConfigValue(name.c_str(), wInterpretted, true);
		}
	}
	return ok;
}

void CConfigExtFile::LogConfigValues(LPCTSTR sourceDesc, bool valid)
{
	LogString(GENERAL, _T("%s: %s"),
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(valid ? _T("valid") : _T("invalid")));

	ConfigBoolMap boolMap = m_configData.GetBoolConfigs();

	for (ConfigBoolMap::iterator it = boolMap.begin(); it != boolMap.end(); ++it)
	{
		LogString(GENERAL, _T("%s: %s=%u fromFile=%s"),
			static_cast<LPCTSTR>(sourceDesc),
			static_cast<LPCTSTR>(it->second.name.c_str()),
			it->second.value,
			static_cast<LPCTSTR>(it->second.setFromFile ? TRUE_STRING : FALSE_STRING));
	}

	ConfigWordMap wordMap = m_configData.GetWordConfigs();

	for (ConfigWordMap::iterator it = wordMap.begin(); it != wordMap.end(); ++it)
	{
		LogString(GENERAL, _T("%s: %s=%u fromFile=%s"),
			static_cast<LPCTSTR>(sourceDesc),
			static_cast<LPCTSTR>(it->second.name.c_str()),
			it->second.value,
			static_cast<LPCTSTR>(it->second.setFromFile ? TRUE_STRING : FALSE_STRING));
	}
}

bool CConfigExtFile::DeleteConfigFile()
{
	string filePath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + CONFIG_FILENAME;
	bool ok(m_operatingSystem.DeleteFile(filePath.c_str()));
	return ok;
}
// #pragma endregion configuration file members



// #pragma region configuration extension members
CConfigExtFile::CConfigExtFile(const CConfig& config,
	IOperatingSystem &operatingSystem) :
	CConfigFile(0, operatingSystem)
{
	SetDefaults();

	m_configData.SetConfigValue(CConfigExtData::EMPLOYEE_OVER_PLAYER_IDLE_CARD_OUT, config.GetEmployeeOverPlayerIdleCardOut(), false);
	m_configData.SetConfigValue(CConfigExtData::PLAYER_RECARD_EMPLOYEE_CARD_OUT, config.GetPlayerRecardEmployeeCardOut(), false);
	m_configData.SetConfigValue(CConfigExtData::NEGLIGIBLE_CREDITS_THRESHOLD, config.GetNegligibleCreditsThreshold(), false);
	m_configData.SetConfigValue(CConfigExtData::EMPLOYEE_IDLE_CARD_OUT_SECONDS, config.GetEmployeeIdleCardOutSeconds(), false);

	m_configData.SetConfigValue(CConfigExtData::SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		config.GetSessionActiveNegligibleCreditsTimeoutSeconds(), false);
	m_configData.SetConfigValue(CConfigExtData::SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		config.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(), false);
	m_configData.SetConfigValue(CConfigExtData::NOTIFICATION_SHOW_PROMOTION_TIME, config.GetNotificationForShowPromotionTime(), false);
	m_configData.SetConfigValue(CConfigExtData::NOTIFICATION_PLAYER_TRACKING_TIME, config.GetNotificationForPlayerTrackingTime(), false);
	m_configData.SetConfigValue(CConfigExtData::RESORT_MENU_BUTTON_COUNT, config.GetResortMenuButtonCount(), false);
	m_configData.SetConfigValue(CConfigExtData::CUSTOM_BUTTON_1_ENABLE, config.GetCustomButton1Enabled(), false);
	m_configData.SetConfigValue(CConfigExtData::CUSTOM_MENU_BUTTON_1_COUNT, config.GetCustomMenuButton1Count(), false);
	m_configData.SetConfigValue(CConfigExtData::CUSTOM_BUTTON_2_ENABLE, config.GetCustomButton2Enabled(), false);
	m_configData.SetConfigValue(CConfigExtData::CUSTOM_MENU_BUTTON_2_COUNT, config.GetCustomMenuButton2Count(), false);
	m_configData.SetConfigValue(CConfigExtData::REBOOT_UI_RESTART_OVERRIDE_ENABLE, config.GetRebootUiRestartOverrideEnabled(), false);

	m_configData.SetConfigValue(CConfigExtData::BONUS_COUNTER_NRICH_LEVEL_PRIORITY, config.GetBonusCounternRichLevelPriority(), false);
	m_configData.SetConfigValue(CConfigExtData::BONUS_COUNTER_SPLASHDOWN_PRIORITY, config.GetBonusCounterSplashdownPriority(), false);
	m_configData.SetConfigValue(CConfigExtData::BONUS_COUNTER_PROGRESSIVE_ODOMETER_PRIORITY, config.GetBonusCounterProgressiveOdometerPriority(), false);
	m_configData.SetConfigValue(CConfigExtData::BONUS_COUNTER_PTE_PRIORITY, config.GetBonusCounterPtePriority(), false);
	m_configData.SetConfigValue(CConfigExtData::BONUS_COUNTER_SCATTER_COUNTDOWN_TIMER_PRIORITY, config.GetBonusCounterScatterCountdownTimerPriority(), false);
	m_configData.SetConfigValue(CConfigExtData::BONUS_COUNTER_PRIZE_PRIORITY, config.GetBonusCounterPrizePriority(), false);
	m_configData.SetConfigValue(CConfigExtData::BONUS_COUNTER_NOTIFICATION_BOX_PRIORITY, config.GetBonusCounterNotificationPriority(), false);
	m_configData.SetConfigValue(CConfigExtData::BONUS_COUNTER_PLAYXGETY_PRIORITY, config.GetBonusCounterPlayXGetYPriority(), false);
	m_configData.SetConfigValue(CConfigExtData::GAME_LIST_CHUNK_SIZE, config.GetGameListChunkSize(), false);
}

CConfigExtFile::CConfigExtFile(const CConfigExtFile &rhs) :
CConfigFile(0, rhs.m_operatingSystem)
{
	*this = rhs;
}

CConfigExtFile& CConfigExtFile::operator=(const CConfigExtFile &rhs)
{
	if (this != &rhs)
	{
		m_configData = rhs.m_configData;
	}

	return *this;
}

bool CConfigExtFile::GetNegligibleCreditsThreshold(WORD &negligibleCreditsThreshold) const
{
	return m_configData.GetConfigValue(CConfigExtData::NEGLIGIBLE_CREDITS_THRESHOLD, negligibleCreditsThreshold);
}

bool CConfigExtFile::GetEmployeeOverPlayerIdleCardOut(bool &employeeOverPlayerIdleCardOut) const
{
	return m_configData.GetConfigValue(CConfigExtData::EMPLOYEE_OVER_PLAYER_IDLE_CARD_OUT, employeeOverPlayerIdleCardOut);
}

bool CConfigExtFile::GetEmployeeIdleCardOutSeconds(WORD &employeeIdleCardOutSeconds) const
{
	return m_configData.GetConfigValue(CConfigExtData::EMPLOYEE_IDLE_CARD_OUT_SECONDS, employeeIdleCardOutSeconds);
}

bool CConfigExtFile::GetPlayerRecardEmployeeCardOut(bool &playerRecardEmployeeCardOut) const
{
	return m_configData.GetConfigValue(CConfigExtData::PLAYER_RECARD_EMPLOYEE_CARD_OUT, playerRecardEmployeeCardOut);
}

bool CConfigExtFile::GetSessionActiveNegligibleCreditsTimeoutSeconds(WORD &sessionActiveNegligibleCreditsTimeoutSeconds) const
{
	return m_configData.GetConfigValue(CConfigExtData::SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		sessionActiveNegligibleCreditsTimeoutSeconds);
}

bool CConfigExtFile::GetSessionActiveNonNegligibleCreditsTimeoutSeconds(WORD &sessionActiveNonNegligibleCreditsTimeoutSeconds) const
{
	return m_configData.GetConfigValue(CConfigExtData::SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS,
		sessionActiveNonNegligibleCreditsTimeoutSeconds);
}

bool CConfigExtFile::GetNotificationForShowPromotionTime(WORD &showPromotionTime) const
{
	return m_configData.GetConfigValue(CConfigExtData::NOTIFICATION_SHOW_PROMOTION_TIME, showPromotionTime);
}

bool CConfigExtFile::GetNotificationForPlayerTrackingTime(WORD &playerTrackingTime) const
{
	return m_configData.GetConfigValue(CConfigExtData::NOTIFICATION_PLAYER_TRACKING_TIME, playerTrackingTime);
}

bool CConfigExtFile::GetResortMenuButtonCount(WORD &count) const
{
	return m_configData.GetConfigValue(CConfigExtData::RESORT_MENU_BUTTON_COUNT, count);
}

bool CConfigExtFile::GetCustomButton1Enabled(bool &enable) const
{
	return m_configData.GetConfigValue(CConfigExtData::CUSTOM_BUTTON_1_ENABLE, enable);
}

bool CConfigExtFile::GetCustomButton2Enabled(bool &enable) const
{
	return m_configData.GetConfigValue(CConfigExtData::CUSTOM_BUTTON_2_ENABLE, enable);
}

bool CConfigExtFile::GetCustomMenuButton1Count(WORD &count) const
{
	return m_configData.GetConfigValue(CConfigExtData::CUSTOM_MENU_BUTTON_1_COUNT, count);
}

bool CConfigExtFile::GetCustomMenuButton2Count(WORD &count) const
{
	return m_configData.GetConfigValue(CConfigExtData::CUSTOM_MENU_BUTTON_2_COUNT, count);
}

bool CConfigExtFile::GetRebootUiRestartOverride(bool &enable) const
{
	return m_configData.GetConfigValue(CConfigExtData::REBOOT_UI_RESTART_OVERRIDE_ENABLE, enable);
}

bool CConfigExtFile::GetBonusCounterNotificationPriority(WORD& priority) const
{
	return m_configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_NOTIFICATION_BOX_PRIORITY, priority);
}

bool CConfigExtFile::GetBonusCounternRichLevelPriority(WORD& priority) const
{
	return m_configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_NRICH_LEVEL_PRIORITY, priority);
}

bool CConfigExtFile::GetBonusCounterSplashdownPriority(WORD& priority) const
{
	return m_configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_SPLASHDOWN_PRIORITY, priority);
}

bool CConfigExtFile::GetBonusCounterProgressiveOdometerPriority(WORD& priority) const
{
	return m_configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_PROGRESSIVE_ODOMETER_PRIORITY, priority);
}

bool CConfigExtFile::GetBonusCounterPtePriority(WORD& priority) const
{
	return m_configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_PTE_PRIORITY, priority);
}

bool CConfigExtFile::GetBonusCounterScatterCountdownTimerPriority(WORD& priority) const
{
	return m_configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_SCATTER_COUNTDOWN_TIMER_PRIORITY, priority);
}

bool CConfigExtFile::GetBonusCounterPrizePriority(WORD& priority) const
{
	return m_configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_PRIZE_PRIORITY, priority);
}

bool CConfigExtFile::GetBonusCounterPlayXGetYPriority(WORD& priority) const
{
	return m_configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_PLAYXGETY_PRIORITY, priority);
}

void CConfigExtFile::SetRebootUiRestartOverride(bool enable)
{
	m_configData.SetConfigValue(CConfigExtData::REBOOT_UI_RESTART_OVERRIDE_ENABLE, enable, true);
}

void CConfigExtFile::SetGameListChunkSize(WORD gameListChunkSize)
{
	m_configData.SetConfigValue(CConfigExtData::GAME_LIST_CHUNK_SIZE, gameListChunkSize, true);
}

bool CConfigExtFile::GetGameListChunkSize(WORD &gameListChunkSize) const
{
	return m_configData.GetConfigValue(CConfigExtData::GAME_LIST_CHUNK_SIZE, gameListChunkSize);
}

// #pragma endregion configuration extension members
