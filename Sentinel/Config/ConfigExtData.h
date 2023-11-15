#pragma once

#include "GlobalDefs.h"
#include "InternalMsgContent.h"

// Data encapsualtion for config setting map for bool values
struct ConfigExtFileBoolInfo
{
	std::string name; // corresponds to key name
	bool setFromFile; // true when loaded from file
	bool value; // the current value
};

// Data encapsualtion for config setting map for WORD values
struct ConfigExtFileWORDInfo
{
	std::string name; // corresponds to key name
	bool setFromFile; // true when loaded from file
	WORD value; // the current value
	WORD m_defaultValue;
	WORD m_minimumValue; // The minimum valid value.
	WORD m_maximumValue; // The maximum valid value.
};

/// <summary>
/// STL collection for bool configuration name-value pairs
/// </summary>
typedef std::map<std::string, ConfigExtFileBoolInfo> ConfigBoolMap;

/// <summary>
/// STL collection for bool configuration name-value pairs
/// </summary>
typedef std::map<std::string, ConfigExtFileWORDInfo> ConfigWordMap;

class CConfigExtData
{
public:
	/// <summary>
	/// Default Ctor
	/// </summary>
	CConfigExtData();

	/// <summary>
	/// Default Dtor
	/// </summary>
	~CConfigExtData();

	ConfigBoolMap GetBoolConfigs() const;
	ConfigWordMap GetWordConfigs() const;

	/// <summary>
	/// Assigment operator.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	/// <returns>A reference to the target of the assignment.</returns>
	CConfigExtData& operator=(const CConfigExtData &configExtData);

	/// <summary>
	/// Is there a Key of this name in the Bool Configs?
	/// </summary>
	/// <returns>True if there is a key by this name in the Map</returns>
	bool HasConfigNameBool(LPCTSTR name);

	/// <summary>
	/// Is there a Key of this name in the WORD Configs?
	/// </summary>
	/// <returns>True if there is a key by this name in the Map</returns>
	bool HasConfigNameWORD(LPCTSTR name);

	/// <summary>
	/// Adds a new key-value pair to the bool configuration map
	/// </summary>
	/// <param name="name">the key name</param>
	/// <param name="bDefault">default value</param>
	void AddConfig(LPCTSTR name, bool bDefault);

	/// <summary>
	/// Adds a new key-value pair to the WORD configuration map
	/// </summary>
	/// <param name="name">the key name</param>
	/// <param name="wDefault">default value</param>
	void AddConfig(LPCTSTR name, WORD wDefault, WORD minimumValue, WORD maximumValue);

	/// <summary>
	/// Gets the value for the key from the bool configuration map
	/// </summary>
	/// <param name="name">the key name</param>
	/// <param name="bOut">IN/OUT the value for the key, unchanged if the key is not found</param>
	/// <returns>bool flag if this was loaded from the config file, false if the key is not found</returns>
	bool GetConfigValue(LPCTSTR name, bool &bOut) const;

	/// <summary>
	/// Gets the value for the key from the WORD configuration map
	/// </summary>
	/// <param name="name">the key name</param>
	/// <param name="wOut">IN/OUT the value for the key, unchanged if the key is not found</param>
	/// <returns>bool flag if this was loaded from the config file, false if the key is not found</returns>
	bool GetConfigValue(LPCTSTR name, WORD &wOut) const;

	/// <summary>
	/// Sets the value for the key in the bool configuration map
	/// </summary>
	/// <param name="name">the key name</param>
	/// <param name="value">the value for the key</param>
	/// <param name="fromFile">IN - true if value is being set from a file, false otherwise.</param>
	void SetConfigValue(LPCTSTR name, bool value, bool fromFile);

	/// <summary>
	/// Sets the value for the key in the WORD configuration map
	/// </summary>
	/// <param name="name">the key name</param>
	/// <param name="value">the value for the key</param>
	/// <param name="fromFile">IN - true if value is being set from a file, false otherwise.</param>
	void SetConfigValue(LPCTSTR name, WORD value, bool fromFile);

	/// <summary>
	/// Fill the Maps with default data (Can only be done once per instance)
	/// </summary>
	void FillDefaults();

	// Parameter names used in the configuration file.
	static const LPCTSTR NEGLIGIBLE_CREDITS_THRESHOLD;
	static const LPCTSTR EMPLOYEE_OVER_PLAYER_IDLE_CARD_OUT;
	static const LPCTSTR EMPLOYEE_IDLE_CARD_OUT_SECONDS;
	static const LPCTSTR PLAYER_RECARD_EMPLOYEE_CARD_OUT;
	static const LPCTSTR SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS; // Non-carded.
	static const LPCTSTR SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS; // Non-carded.
	static const LPCTSTR GAME_LIST_CHUNK_SIZE;
	
	// Notification names
	static const LPCTSTR NOTIFICATION_SHOW_PROMOTION_TIME;
	static const LPCTSTR NOTIFICATION_PLAYER_TRACKING_TIME;

	static const LPCTSTR REBOOT_UI_RESTART_OVERRIDE_ENABLE;

	static const LPCTSTR BONUS_COUNTER_NRICH_LEVEL_PRIORITY;
	static const LPCTSTR BONUS_COUNTER_SPLASHDOWN_PRIORITY;
	static const LPCTSTR BONUS_COUNTER_PROGRESSIVE_ODOMETER_PRIORITY;
	static const LPCTSTR BONUS_COUNTER_PTE_PRIORITY;
	static const LPCTSTR BONUS_COUNTER_SCATTER_COUNTDOWN_TIMER_PRIORITY;
	static const LPCTSTR BONUS_COUNTER_PRIZE_PRIORITY;
	static const LPCTSTR BONUS_COUNTER_NOTIFICATION_BOX_PRIORITY;
	static const LPCTSTR BONUS_COUNTER_PLAYXGETY_PRIORITY;	

	// Custom config names
	static const LPCTSTR RESORT_MENU_BUTTON_COUNT;
	static const LPCTSTR CUSTOM_BUTTON_1_ENABLE;
	static const LPCTSTR CUSTOM_BUTTON_2_ENABLE;
	static const LPCTSTR CUSTOM_MENU_BUTTON_1_COUNT;
	static const LPCTSTR CUSTOM_MENU_BUTTON_2_COUNT;

	// Default values
	static const WORD DEFAULT_NEGLIGIBLE_CREDITS_THRESHOLD = 0;
	static const bool DEFAULT_EMPLOYEE_OVER_PLAYER_IDLE_CARD_OUT = false;
	static const WORD DEFAULT_EMPLOYEE_IDLE_CARD_OUT_SECONDS = 120;
	static const bool DEFAULT_PLAYER_RECARD_EMPLOYEE_CARD_OUT = false;

	static const WORD DEFAULT_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS = 10; // Non-carded.
	static const WORD MINIMUM_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS = 0; // Non-carded.
	static const WORD MAXIMUM_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS = WORD_MAX; // Non-carded.

	static const WORD DEFAULT_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS = 300; // Non-carded.
	static const WORD MINIMUM_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS = 10; // Non-carded.
	static const WORD MAXIMUM_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS = WORD_MAX; // Non-carded.

	static const bool DEFAULT_REBOOT_UI_RESTART_OVERRIDE_ENABLE = false;

	static const bool DEFAULT_CUSTOM_BUTTON_1_ENABLE = false;
	static const bool DEFAULT_CUSTOM_BUTTON_2_ENABLE = false;
	
	static const WORD DEFAULT_RESORT_MENU_BUTTON_COUNT = 1;
	static const WORD MINIMUM_RESORT_MENU_BUTTON_COUNT = 1;
	static const WORD MAXIMUM_RESORT_MENU_BUTTON_COUNT = 6;

	static const WORD DEFAULT_CUSTOM_MENU_BUTTON_1_COUNT = 0;
	static const WORD MINIMUM_CUSTOM_MENU_BUTTON_1_COUNT = 0;
	static const WORD MAXIMUM_CUSTOM_MENU_BUTTON_1_COUNT = 6;
	
	static const WORD DEFAULT_CUSTOM_MENU_BUTTON_2_COUNT = 0;
	static const WORD MINIMUM_CUSTOM_MENU_BUTTON_2_COUNT = 0;
	static const WORD MAXIMUM_CUSTOM_MENU_BUTTON_2_COUNT = 6;

	static const WORD DEFAULT_NOTIFICATION_SHOW_PROMOTION = 30;
	static const WORD MINIMUM_NOTIFICATION_SHOW_PROMOTION = 1;
	static const WORD MAXIMUM_NOTIFICATION_SHOW_PROMOTION = 60;

	static const WORD DEFAULT_NOTIFICATION_PLAYER_TRACKING = 3;
	static const WORD MINIMUM_NOTIFICATION_PLAYER_TRACKING = 1;
	static const WORD MAXIMUM_NOTIFICATION_PLAYER_TRACKING = 60;

	static const bool DEFAULT_UJP_ENABLED = false;
	static const bool DEFAULT_MINI_VIRTUAL_SESSION_ENABLED = false;
	static const bool DEFAULT_ONE_LINK_PROGRESSIVES_ENABLED = false;

	static const WORD DEFAULT_NOTIFICATION_BOX_PRIORITY = 200;
	static const WORD DEFAULT_NRICH_PRIORITY = 200;
	static const WORD DEFAULT_PROGRESSIVE_ODOMETER_PRIORITY = 200;
	static const WORD DEFAULT_PLAYXGETY_PRIORITY = 200;
	static const WORD DEFAULT_PTE_PRIORITY = 200;
	static const WORD DEFAULT_SPLASHDOWN_PRIORITY = 100;
	static const WORD DEFAULT_SCATTER_COUNTDOWN_TIMER_PRIORITY = 200;
	static const WORD DEFAULT_PRIZE_ODOMETER_PRIORITY = 200;

	static const WORD MINIMUM_GAME_LIST_CHUNK_SIZE = 0;
	static const WORD MAXIMUM_GAME_LIST_CHUNK_SIZE = 1000;  //ToDo: Min max valyes neeed to be confirmed

private:

	/// <summary>
	/// Name/Value pair for bool map
	/// </summary>
	typedef std::pair<std::string, ConfigExtFileBoolInfo> BoolKeyPair;

	/// <summary>
	/// Name/Value pair for WORD map
	/// </summary>
	typedef std::pair<std::string, ConfigExtFileWORDInfo> WORDKeyPair;

	/// <summary>
	/// Map (name/value) for bool configurations
	/// </summary>
	ConfigBoolMap m_boolConfigMap;

	/// <summary>
	/// Map for WORD configuration name-value pairs
	/// </summary>
	ConfigWordMap m_wordConfigMap;
};

