#include "stdafx.h"

#include "MediaWindowConfig.h"
#include "Logging/LogString.h"
#include "../UI/ScreenDefs.h"
#include "RapidJsonHelper.h"

using namespace std;

const string MEDIA_WINDOW_CONFIG = "MediaWindowConfig";

const bool CMediaWindowConfig::DEFAULT_MAINTAIN_ASPECT_RATIO = false;
const MediaWindowPlayerState CMediaWindowConfig::DEFAULT_BONUSEVENT_PLAYERSTATE = OPEN_PLAYER_STATE;
const MediaWindowPlayerState CMediaWindowConfig::DEFAULT_CARDIN_PLAYERSTATE = OPEN_PLAYER_STATE;
const MediaWindowPlayerState CMediaWindowConfig::DEFAULT_CARDOUT_PLAYERSTATE = CLOSE_PLAYER_STATE;
const MediaWindowPlayerState CMediaWindowConfig::DEFAULT_FIRSTPLAY_PLAYERSTATE = CLOSE_PLAYER_STATE;
const byte CMediaWindowConfig::DEFAULT_AUTO_SWITCH_MAP = AUTOCLOSE_MASK;
const WORD CMediaWindowConfig::DEFAULT_AUTOSWITCH_WAITTIME = EEPROM_MW_AUTO_MINUTES_DEFAULT;
const WORD CMediaWindowConfig::DEFAULT_TASKBARSIZE = EEPROM_TASKBAR_SIZE_DEFAULT;
const WORD CMediaWindowConfig::DEFAULT_CW_SIZE = EEPROM_SERVICE_WINDOW_SIZE_DEFAULT;
const ContentWindowOrientation CMediaWindowConfig::DEFAULT_CW_ORIENTATION = (ContentWindowOrientation)EEPROM_SERVICE_WINDOW_LOCATION_DEFAULT;

const LPCTSTR CMediaWindowConfig::DESC_MAINTAIN_ASPECT_RATIO = "MaintainAspectRatio";
const LPCTSTR CMediaWindowConfig::DESC_BONUSEVENT_PLAYERSTATE = "BonusEventPlayerState";
const LPCTSTR CMediaWindowConfig::DESC_CARDIN_PLAYERSTATE = "CardInPlayerState";
const LPCTSTR CMediaWindowConfig::DESC_CARDOUT_PLAYERSTATE = "CardOutPlayerState";
const LPCTSTR CMediaWindowConfig::DESC_FIRSTPLAY_PLAYERSTATE = "FirstPlayPlayerState";
const LPCTSTR CMediaWindowConfig::DESC_AUTOSWITCH = "AutoSwitch";
const LPCTSTR CMediaWindowConfig::DESC_AUTOSWITCH_WAITTIME = "AutoSwitchWaitTime";
const LPCTSTR CMediaWindowConfig::DESC_TASKBARSIZE = "TaskbarSize";
const LPCTSTR CMediaWindowConfig::DESC_CW_SIZE = "ContentWindowSize";
const LPCTSTR CMediaWindowConfig::DESC_CW_ORIENTATION = "ContentWindowOrientation";

// 2013-01-11 Constant expressions like these need to be outside the class definition (in VS 2012).
const DWORD CMediaWindowConfig::GAME_CREDITS_BECAME_ZERO_BEGIN_SESSION_DELAY_IN_MILLISECONDS = 20 * MILLISECONDS_PER_SECOND; // 20 seconds.
const DWORD CMediaWindowConfig::GAME_CREDITS_BECAME_ZERO_END_SESSION_DELAY_IN_MILLISECONDS = 20 * MILLISECONDS_PER_SECOND; // 20 seconds.
const DWORD CMediaWindowConfig::CARD_OUT_GAME_CREDITS_BECOME_ZERO_TIMEOUT_IN_MILLISECONDS = 20 * MILLISECONDS_PER_SECOND; // 20 seconds.
const DWORD CMediaWindowConfig::GAME_NOT_IN_PROGRESS_BEGIN_SESSION_DELAY_IN_MILLISECONDS = 20 * MILLISECONDS_PER_SECOND; // 20 seconds.
const DWORD CMediaWindowConfig::GAME_NOT_IN_PROGRESS_END_SESSION_DELAY_IN_MILLISECONDS = 20 * MILLISECONDS_PER_SECOND; // 20 seconds.

CMediaWindowConfig* CMediaWindowConfig::m_instance = NULL;
std::recursive_mutex CMediaWindowConfig::m_instanceCS;

// Instance
//   Returns the CMediaWindowConfig singleton and creates if not already existing
CMediaWindowConfig* CMediaWindowConfig::Instance()
{
	lock_guard<recursive_mutex> lock(m_instanceCS);
	if (NULL == m_instance)
	{
		m_instance = new CMediaWindowConfig();
	}
	return m_instance;
}

CMediaWindowConfig::CMediaWindowConfig() : CDataSubscriptionProvider(MEDIA_WINDOW_CONFIG)
{
	m_persistent = true;
	m_maintainAspectRatio = DEFAULT_MAINTAIN_ASPECT_RATIO;
	m_bonusEventPlayerState = DEFAULT_BONUSEVENT_PLAYERSTATE;
	m_cardInPlayerState = DEFAULT_CARDIN_PLAYERSTATE;
	m_cardOutPlayerState = DEFAULT_CARDOUT_PLAYERSTATE;
	m_firstPlayPlayerState = DEFAULT_FIRSTPLAY_PLAYERSTATE;
	m_autoSwitchMask = DEFAULT_AUTO_SWITCH_MAP;
	m_autoSwitchWaitTime = DEFAULT_AUTOSWITCH_WAITTIME;
	m_taskbarSize = DEFAULT_TASKBARSIZE;
	m_contentWindowSize = DEFAULT_CW_SIZE;
	m_contentWindowOrientation = DEFAULT_CW_ORIENTATION;
	m_localChangePendingBitMap = 0;

	BuildYourself();
}

// Constructor
//   Copy constructor. Since default constructor is protected, all CMediaWindowConfig objects
//   will be copies of either other objects or the singleton createdin by Instance
// NOTE: defined so that normal constructor isn't called
CMediaWindowConfig::CMediaWindowConfig(const CMediaWindowConfig &rhs) : CDataSubscriptionProvider(MEDIA_WINDOW_CONFIG)
{
	m_persistent = false;
	m_localChangePendingBitMap = 0;
	SetFromConfig(rhs, false);
}

// Destructor
//   Exist to make sure subclasses are properly destructed
CMediaWindowConfig::~CMediaWindowConfig()
{
}

CMediaWindowConfig& CMediaWindowConfig::operator=(const CMediaWindowConfig& rhs)
{
	SetFromConfig(rhs, true);
	return *this;
}

// Operator==
//     IN rhs - CMediaWindowConfig to be compared to
//   returns true if all settings are the same, but ignores operation attributes such as 'persistent' and 'local change pending map'
bool CMediaWindowConfig::operator==(const CMediaWindowConfig& rhs) const
{
	if (m_maintainAspectRatio != rhs.m_maintainAspectRatio
		|| m_bonusEventPlayerState != rhs.m_bonusEventPlayerState
		|| m_cardInPlayerState != rhs.m_cardInPlayerState
		|| m_cardOutPlayerState != rhs.m_cardOutPlayerState
		|| m_firstPlayPlayerState != rhs.m_firstPlayPlayerState
		|| m_autoSwitchMask != rhs.m_autoSwitchMask
		|| m_autoSwitchWaitTime != rhs.m_autoSwitchWaitTime
		|| m_taskbarSize != rhs.m_taskbarSize
		|| m_contentWindowSize != rhs.m_contentWindowSize
		|| m_contentWindowOrientation != rhs.m_contentWindowOrientation)
		return false;

	return true;
}

void CMediaWindowConfig::BuildYourself()
{
	BuildMaintainAspectRatio();

	BuildBonusEventState();

	BuildCardInState();

	BuildCardOutState();

	BuildFirstPlayState();

	BuildAutoSwitchMap();

	BuildAutoSwitchTime();

	BuildContentWindowOrientation();

	BuildContentWindowSize();

	BuildTaskbarSize();

}

void CMediaWindowConfig::BuildMaintainAspectRatio()
{
	BYTE tempByte;

	// Set the maintain aspect ratio from EEPROM setting
	m_eepromConfig.Get(EEPROM_MW_EGM_ASPECT_RATIO_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	if (tempByte != EEPROM_MW_EGM_ASPECT_MAINTAIN && tempByte != EEPROM_MW_EGM_ASPECT_NO_MAINTAIN)
	{
		// Not valid in EEPROM. Set to default
		tempByte = EEPROM_MW_EGM_ASPECT_DEFAULT;
		m_eepromConfig.Set(EEPROM_MW_EGM_ASPECT_RATIO_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	}
	if (tempByte == EEPROM_MW_EGM_ASPECT_MAINTAIN)
	{
		m_maintainAspectRatio = true;
	}
	else if (tempByte == EEPROM_MW_EGM_ASPECT_NO_MAINTAIN)
	{
		m_maintainAspectRatio = false;
	}
}

void CMediaWindowConfig::BuildBonusEventState()
{
	BYTE tempByte;

	// Set the bonuse-event state from the EEPROM setting
	m_eepromConfig.Get(EEPROM_MW_BONUS_EVENT_PLAYER_STATE_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	if (tempByte != EEPROM_MW_BONUS_EVENT_PLAYER_STATE_MAINTAIN && tempByte != EEPROM_MW_BONUS_EVENT_PLAYER_STATE_OPEN && tempByte != EEPROM_MW_BONUS_EVENT_PLAYER_STATE_CLOSE)
	{
		// Not valid in EEPROM. Set to default
		tempByte = EEPROM_MW_BONUS_EVENT_PLAYER_STATE_DEFAULT;
		m_eepromConfig.Set(EEPROM_MW_BONUS_EVENT_PLAYER_STATE_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	}
	if (tempByte == EEPROM_MW_BONUS_EVENT_PLAYER_STATE_MAINTAIN)
	{
		m_bonusEventPlayerState = MAINTAIN_PLAYER_STATE;
	}
	else if (tempByte == EEPROM_MW_BONUS_EVENT_PLAYER_STATE_OPEN)
	{
		m_bonusEventPlayerState = OPEN_PLAYER_STATE;
	}
	else if (tempByte == EEPROM_MW_BONUS_EVENT_PLAYER_STATE_CLOSE)
	{
		m_bonusEventPlayerState = CLOSE_PLAYER_STATE;
	}
}

void CMediaWindowConfig::BuildCardInState()
{
	BYTE tempByte;

	// Set the card-in state from the EEPROM setting
	m_eepromConfig.Get(EEPROM_MW_CARD_IN_PLAYER_STATE_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	if (tempByte != EEPROM_MW_CARD_IN_PLAYER_STATE_MAINTAIN && tempByte != EEPROM_MW_CARD_IN_PLAYER_STATE_OPEN && tempByte != EEPROM_MW_CARD_IN_PLAYER_STATE_CLOSE)
	{
		// Not valid in EEPROM. Set to default
		tempByte = EEPROM_MW_CARD_IN_PLAYER_STATE_DEFAULT;
		m_eepromConfig.Set(EEPROM_MW_CARD_IN_PLAYER_STATE_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	}
	if (tempByte == EEPROM_MW_CARD_IN_PLAYER_STATE_MAINTAIN)
	{
		m_cardInPlayerState = MAINTAIN_PLAYER_STATE;
	}
	else if (tempByte == EEPROM_MW_CARD_IN_PLAYER_STATE_OPEN)
	{
		m_cardInPlayerState = OPEN_PLAYER_STATE;
	}
	else if (tempByte == EEPROM_MW_CARD_IN_PLAYER_STATE_CLOSE)
	{
		m_cardInPlayerState = CLOSE_PLAYER_STATE;
	}
}

void CMediaWindowConfig::BuildCardOutState()
{
	BYTE tempByte;

	// Set the card-out state from the EEPROM setting
	m_eepromConfig.Get(EEPROM_MW_CARD_OUT_PLAYER_STATE_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	if (tempByte != EEPROM_MW_CARD_OUT_PLAYER_STATE_MAINTAIN && tempByte != EEPROM_MW_CARD_OUT_PLAYER_STATE_OPEN && tempByte != EEPROM_MW_CARD_OUT_PLAYER_STATE_CLOSE)
	{
		// Not valid in EEPROM. Set to default
		tempByte = EEPROM_MW_CARD_OUT_PLAYER_STATE_DEFAULT;
		m_eepromConfig.Set(EEPROM_MW_CARD_OUT_PLAYER_STATE_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	}
	if (tempByte == EEPROM_MW_CARD_OUT_PLAYER_STATE_MAINTAIN)
	{
		m_cardOutPlayerState = MAINTAIN_PLAYER_STATE;
	}
	else if (tempByte == EEPROM_MW_CARD_OUT_PLAYER_STATE_OPEN)
	{
		m_cardOutPlayerState = OPEN_PLAYER_STATE;
	}
	else if (tempByte == EEPROM_MW_CARD_OUT_PLAYER_STATE_CLOSE)
	{
		m_cardOutPlayerState = CLOSE_PLAYER_STATE;
	}
	// Else use the default.

}

void CMediaWindowConfig::BuildFirstPlayState()
{
	BYTE tempByte;

	// Set the card-out state from the EEPROM setting
	m_eepromConfig.Get(EEPROM_MW_FIRST_PLAY_PLAYER_STATE_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	if (tempByte != EEPROM_MW_FIRST_PLAY_PLAYER_STATE_MAINTAIN && tempByte != EEPROM_MW_FIRST_PLAY_PLAYER_STATE_OPEN && tempByte != EEPROM_MW_FIRST_PLAY_PLAYER_STATE_CLOSE)
	{
		// Not valid in EEPROM. Set to default
		tempByte = EEPROM_MW_FIRST_PLAY_PLAYER_STATE_DEFAULT;
		m_eepromConfig.Set(EEPROM_MW_FIRST_PLAY_PLAYER_STATE_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	}
	if (tempByte == EEPROM_MW_FIRST_PLAY_PLAYER_STATE_MAINTAIN)
	{
		m_firstPlayPlayerState = MAINTAIN_PLAYER_STATE;
	}
	else if (tempByte == EEPROM_MW_FIRST_PLAY_PLAYER_STATE_OPEN)
	{
		m_firstPlayPlayerState = OPEN_PLAYER_STATE;
	}
	else if (tempByte == EEPROM_MW_FIRST_PLAY_PLAYER_STATE_CLOSE)
	{
		m_firstPlayPlayerState = CLOSE_PLAYER_STATE;
	}
	// Else use the default.

}

void CMediaWindowConfig::BuildAutoSwitchMap()
{
	BYTE tempByte;

	// Zero the auto switch map. Will be built below.
	m_autoSwitchMask = AUTO_NONE_MASK;

	// Set the auto close mask in the auto switch map from the EEPROM setting
	m_eepromConfig.Get(EEPROM_MW_AUTO_CLOSE_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	if (tempByte != EEPROM_MW_AUTO_CLOSE_YES && tempByte != EEPROM_MW_AUTO_CLOSE_NO)
	{
		// Not valid in EEPROM. Set to default
		tempByte = EEPROM_MW_AUTO_CLOSE_DEFAULT;
		m_eepromConfig.Set(EEPROM_MW_AUTO_CLOSE_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	}
	if (tempByte == EEPROM_MW_AUTO_CLOSE_YES)
	{
		m_autoSwitchMask |= AUTOCLOSE_MASK;
	}

	// Set the auto open mask in the auto switch map from the EEPROM setting
	m_eepromConfig.Get(EEPROM_MW_AUTO_OPEN_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	if (tempByte != EEPROM_MW_AUTO_OPEN_YES && tempByte != EEPROM_MW_AUTO_OPEN_NO)
	{
		// Not valid in EEPROM. Set to default
		tempByte = EEPROM_MW_AUTO_OPEN_DEFAULT;
		m_eepromConfig.Set(EEPROM_MW_AUTO_OPEN_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	}
	if (tempByte == EEPROM_MW_AUTO_OPEN_YES)
	{
		m_autoSwitchMask |= AUTOOPEN_MASK;
	}
}

void CMediaWindowConfig::BuildAutoSwitchTime()
{
	BYTE tempByte;

	// Set the auto switch time
	m_eepromConfig.Get(EEPROM_MW_AUTO_MINUTES_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	if (tempByte == 0)
	{
		tempByte = EEPROM_MW_AUTO_MINUTES_DEFAULT;
		m_eepromConfig.Set(EEPROM_MW_AUTO_MINUTES_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	}
	m_autoSwitchWaitTime = (WORD)tempByte;
}

void CMediaWindowConfig::BuildContentWindowOrientation()
{
	BYTE tempByte;

	m_eepromConfig.Get(EEPROM_SERVICE_WINDOW_LOCATION_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	if (tempByte != CWORIENTATION_LEFT && tempByte != CWORIENTATION_RIGHT && tempByte != CWORIENTATION_BOTTOM_OR_LCD)
	{
		tempByte = (byte)EEPROM_SERVICE_WINDOW_LOCATION_DEFAULT;
		m_eepromConfig.Set(EEPROM_SERVICE_WINDOW_LOCATION_OFFSET, (BYTE *)&tempByte, sizeof(tempByte));
	}
	m_contentWindowOrientation = (ContentWindowOrientation)tempByte;
}

void CMediaWindowConfig::BuildContentWindowSize()
{
	WORD tempWord;

	m_eepromConfig.Get(EEPROM_SERVICE_WINDOW_SIZE_OFFSET, (BYTE *)&tempWord, sizeof(tempWord));
	if (tempWord < SW_MIN_SIZE || tempWord > SW_MAX_SIZE)
	{
		tempWord = (WORD)EEPROM_SERVICE_WINDOW_SIZE_DEFAULT;
		m_eepromConfig.Set(EEPROM_SERVICE_WINDOW_SIZE_OFFSET, (BYTE *)&tempWord, sizeof(tempWord));
	}
	m_contentWindowSize = tempWord;
}

void CMediaWindowConfig::BuildTaskbarSize()
{
	WORD tempWord;

	m_eepromConfig.Get(EEPROM_TASKBAR_SIZE_OFFSET, (BYTE *)&tempWord, sizeof(tempWord));
	if (tempWord < TASKBAR_MIN_SIZE || tempWord > TASKBAR_MAX_SIZE)
	{
		tempWord = (WORD)EEPROM_TASKBAR_SIZE_DEFAULT;
		m_eepromConfig.Set(EEPROM_TASKBAR_SIZE_OFFSET, (BYTE *)&tempWord, sizeof(tempWord));
	}
	m_taskbarSize = tempWord;
}

string CMediaWindowConfig::GetSubscriptionDataResponse(const string& topic, const string& whereId)
{
	UNREFERENCED_PARAMETER(&topic);
	UNREFERENCED_PARAMETER(&whereId);

	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key("Location");								writer.Int(m_contentWindowOrientation);
	writer.Key("ContentWindowMinimumSize");				writer.Int(SW_MIN_SIZE);
	writer.Key("ContentWindowMaximumSize");				writer.Int(SW_MAX_SIZE);
	writer.Key("ContentWindowSize");					writer.Int(m_contentWindowSize);
	writer.Key("TaskbarMinimumSize");					writer.Int(TASKBAR_MIN_SIZE);
	writer.Key("TaskbarMaximumSize");					writer.Int(TASKBAR_MAX_SIZE);
	writer.Key("TaskbarSize");							writer.Int(m_taskbarSize);
	writer.Key("CardInOpenWindowOrMaintainState");		writer.String(GetMediaWindowPlayerState(m_cardInPlayerState));
	writer.Key("BonusEventOpenWindowOrMaintainState");	writer.String(GetMediaWindowPlayerState(m_bonusEventPlayerState));
	writer.Key("CardOutCloseWindowOrMaintainState");	writer.String(GetMediaWindowPlayerState(m_cardOutPlayerState));
	writer.Key("FirstPlayCloseWindowOrMaintainState");	writer.String(GetMediaWindowPlayerState(m_firstPlayPlayerState));
	bool autoClose(m_autoSwitchMask & CMediaWindowConfig::AUTOCLOSE_MASK);
	writer.Key("MediaWindowAutoCloseOrMaintain");		writer.String(autoClose ? "AutoClose" : "Maintain");
	writer.Key("MediaWindowAutoCloseMinimumMinutes");	writer.Int(AUTOCLOSE_MIN_MINUTES);
	writer.Key("MediaWindowAutoCloseMaximumMinutes");	writer.Int(AUTOCLOSE_MAX_MINUTES);
	writer.Key("MediaWindowAutoCloseMinutes");			writer.Int(m_autoSwitchWaitTime);
	writer.Key("MaintainEGMDisplayAspectRatio");		writer.Bool(m_maintainAspectRatio);
	writer.EndObject();

	return writeStream.m_str;

}

// IsPersistent
//   returns true if settings are perserved in EEPROM
bool CMediaWindowConfig::IsPersistent() const
{
	return m_persistent;
}

// CMediaWindowConfig::SetFromConfigAlways
//   IN mediaWindowConfig - source parameter used to update this instance.
// Set all media window configuration values from the mediaWindowConfig parameter.
// And leave this instance in a state where additional Set*() calls can work consistently.
void CMediaWindowConfig::SetFromConfigAlways(const CMediaWindowConfig& mediaWindowConfig)
{
	SetFromConfig(mediaWindowConfig, true);
	ClearAllLocalChangePending();
}

// SetFromConfig
//     IN mediaWindowConfig - mediaWindowConfig object to set configuration
//     IN localChange - whether settings are being done at the nCompass unit or remotely such as from SpeedMedia
//   Makes all setting match mediaWindowConfig depending on localChange
// return bool conflictFound - true if there was a conflict on any of the settings
//     if localChange is true all settings will match and local change pending is set to true if changed
//     if localChange is false, setting is only made to match if does not already have a local change pending
//     if localChange is false and setting already matches, local change pending is cleared.
bool CMediaWindowConfig::SetFromConfig(const CMediaWindowConfig& mediaWindowConfig, bool localChange)
{
	bool conflictFound = false;
	if (!SetMaintainAspectRatio(mediaWindowConfig.m_maintainAspectRatio, localChange))
		conflictFound = true;
	if (!SetBonusEventPlayerState(mediaWindowConfig.m_bonusEventPlayerState, localChange))
		conflictFound = true;
	if (!SetCardInPlayerState(mediaWindowConfig.m_cardInPlayerState, localChange))
		conflictFound = true;
	if (!SetCardOutPlayerState(mediaWindowConfig.m_cardOutPlayerState, localChange))
		conflictFound = true;
	if (!SetFirstPlayPlayerState(mediaWindowConfig.m_firstPlayPlayerState, localChange))
		conflictFound = true;
	if (!SetAutoSwitch(mediaWindowConfig.m_autoSwitchMask, localChange))
		conflictFound = true;
	if (!SetAutoSwitchWaitTime(mediaWindowConfig.m_autoSwitchWaitTime, localChange))
		conflictFound = true;
	if (!SetTaskbarSize(mediaWindowConfig.m_taskbarSize, localChange))
		conflictFound = true;
	if (!SetContentWindowSize(mediaWindowConfig.m_contentWindowSize, localChange))
		conflictFound = true;
	if (!SetContentWindowOrientation(mediaWindowConfig.m_contentWindowOrientation, localChange))
		conflictFound = true;
	return conflictFound;
}

// SetFromFile
//     IN szFilePath - path of properly formatted file used to set configuration
//     IN localChange - whether settings are being done at the nCompass unit or remotely such as from SpeedMedia
//   Set configuration thru a properly formatted file depending on localChange
//     if localChange is true all settings are affected and local change pending is set to true if changed
//     if localChange is false, setting is only changed if does not already have a local change pending
//     if localChange is false and setting already done, local change pending is cleared.
void CMediaWindowConfig::SetFromFile(const std::string &szFilePath, bool localChange)
{
	ifstream file(szFilePath, ios_base::in);
	if (file)
	{
		string line;
		while (getline(file, line))
		{
			bool lineIgnored = true;

			int eqIndex = 0;
			if ((eqIndex = line.find('=')) > 0)
			{
				TCHAR *remaining = NULL;

				string desc = line.substr(0, eqIndex);
				CUtilities::Trim(desc);
				string value = line.substr(eqIndex + 1);
				CUtilities::Trim(value);

				if (CUtilities::CompareStringsNoCase(desc, DESC_MAINTAIN_ASPECT_RATIO) == 0)
				{
					long flag = strtol(value.c_str(), &remaining, 10);
					if (L'\0' == *remaining)
					{
						SetMaintainAspectRatio((flag > 0), localChange);
						lineIgnored = false;
					}
				}
				else if (CUtilities::CompareStringsNoCase(desc, DESC_BONUSEVENT_PLAYERSTATE) == 0)
				{
					long state = strtol(value.c_str(), &remaining, 10);
					if (L'\0' == *remaining)
					{
						SetBonusEventPlayerState((MediaWindowPlayerState)state, localChange);
						lineIgnored = false;
					}
				}
				else if (CUtilities::CompareStringsNoCase(desc, DESC_CARDIN_PLAYERSTATE) == 0)
				{
					long state = strtol(value.c_str(), &remaining, 10);
					if (L'\0' == *remaining)
					{
						SetCardInPlayerState((MediaWindowPlayerState)state, localChange);
						lineIgnored = false;
					}
				}
				else if (CUtilities::CompareStringsNoCase(desc, DESC_CARDOUT_PLAYERSTATE) == 0)
				{
					long state = strtol(value.c_str(), &remaining, 10);
					if (L'\0' == *remaining)
					{
						SetCardOutPlayerState((MediaWindowPlayerState)state, localChange);
						lineIgnored = false;
					}
				}
				else if (CUtilities::CompareStringsNoCase(desc, DESC_FIRSTPLAY_PLAYERSTATE) == 0)
				{
					long state = strtol(value.c_str(), &remaining, 10);
					if (L'\0' == *remaining)
					{
						SetFirstPlayPlayerState((MediaWindowPlayerState)state, localChange);
						lineIgnored = false;
					}
				}
				else if (CUtilities::CompareStringsNoCase(desc, DESC_AUTOSWITCH) == 0)
				{
					long state = strtol(value.c_str(), &remaining, 10);
					if (L'\0' == *remaining)
					{
						SetAutoSwitch((byte)state, localChange);
						lineIgnored = false;
					}
				}
				else if (CUtilities::CompareStringsNoCase(desc, DESC_AUTOSWITCH_WAITTIME) == 0)
				{
					long waitTime = strtol(value.c_str(), &remaining, 10);
					if (L'\0' == *remaining)
					{
						SetAutoSwitchWaitTime((WORD)waitTime, localChange);
						lineIgnored = false;
					}
				}
				else if (CUtilities::CompareStringsNoCase(desc, DESC_TASKBARSIZE) == 0)
				{
					long size = strtol(value.c_str(), &remaining, 10);
					if (L'\0' == *remaining)
					{
						SetTaskbarSize((WORD)size, localChange);
						lineIgnored = false;
					}
				}
				else if (CUtilities::CompareStringsNoCase(desc, DESC_CW_SIZE) == 0)
				{
					long size = strtol(value.c_str(), &remaining, 10);
					if (L'\0' == *remaining)
					{
						SetContentWindowSize((WORD)size, localChange);
						lineIgnored = false;
					}
				}
				else if (CUtilities::CompareStringsNoCase(desc, DESC_CW_ORIENTATION) == 0)
				{
					long orientation = strtol(value.c_str(), &remaining, 10);
					if (L'\0' == *remaining)
					{
						SetContentWindowOrientation((ContentWindowOrientation)orientation, localChange);
						lineIgnored = false;
					}
				}
			}

			if (lineIgnored)
			{
				LogString(ERROR_LOG, "CMediaWindowConfig::SetFromFile: %s in %s not correctly formatted", line.c_str(), szFilePath.c_str());
			}
		}
		file.close();
	}
}

DWORD CMediaWindowConfig::GetLocalChangePendingMask(MediaWindowConfigSettingId settingId) const
{
	// Because settingIds are enumerated in order, the id value also matches the position in the bitmask.
	return (((DWORD)1) << settingId);
}

// GetLocalChangePending
//     IN settingId - id enum that corresponds to the given setting
//   returns whether changes have been made at the nCompass unit but not yet is sync with SpeedMedia
bool CMediaWindowConfig::GetLocalChangePending(MediaWindowConfigSettingId settingId) const
{
	return (0 != (m_localChangePendingBitMap & GetLocalChangePendingMask(settingId)));
}


// ClearAllLocalChangePending
//   Sets all local change pending flags to false
void CMediaWindowConfig::ClearAllLocalChangePending()
{
	m_localChangePendingBitMap = 0;
}

void CMediaWindowConfig::SetLocalChangePending(MediaWindowConfigSettingId settingId, bool state)
{
	DWORD bitMap = 0;

	if (state)
	{
		bitMap = (m_localChangePendingBitMap | GetLocalChangePendingMask(settingId));
	}
	else
	{
		bitMap = (m_localChangePendingBitMap & ~GetLocalChangePendingMask(settingId));
	}

	if (bitMap != m_localChangePendingBitMap)
	{
		m_localChangePendingBitMap = bitMap;
		if (IsPersistent())
		{
			if (!m_eepromConfig.Set(EEPROM_MW_LOCAL_CHANGE_PENDING_MAP_OFFSET, (BYTE *)&m_localChangePendingBitMap, EEPROM_MW_LOCAL_CHANGE_PENDING_MAP_SIZE))
			{
				LogString(ERROR_LOG, "Could not wrtie local change pending bit map to EEPROM");
			}
		}
	}
}

// GetMaintainAspectRatio
//   returns whether or the the EGM aspect ratio needs to be perserved
bool CMediaWindowConfig::GetMaintainAspectRatio() const
{
	return m_maintainAspectRatio;
}

// SetMaintainAspectRatio
//     IN state - whether or the the EGM aspect ratio needs to be perserved
//     IN localChange - whether settings are being done at the nCompass unit or remotely such as from SpeedMedia
//   Sets where EGM aspect ratio is perserved depending on localChange
//     if localChange is true all setting is affected and local change pending is set to true if changed
//     if localChange is false, setting is only changed if does not already have a local change pending
//     if localChange is false and setting already done, local change pending is cleared.
//	returns false if conflict exists between new value and pending change
bool CMediaWindowConfig::SetMaintainAspectRatio(bool state, bool localChange)
{
	bool conflict = false;
	bool result = SetConfigSetting<bool>(m_maintainAspectRatio, state, MWC_SETTING_MAINTAIN_ASPECT_RATIO, localChange, conflict);
	if (result && IsPersistent())
	{
		byte stateByte = m_maintainAspectRatio ? EEPROM_MW_EGM_ASPECT_MAINTAIN : EEPROM_MW_EGM_ASPECT_NO_MAINTAIN;
		if (!m_eepromConfig.Set(EEPROM_MW_EGM_ASPECT_RATIO_OFFSET, &stateByte, EEPROM_MW_EGM_ASPECT_RATIO_SIZE))
		{
			LogString(ERROR_LOG, "Could not write maintain aspect ratio to EEPROM");
		}
	}
	return !conflict;
}

MediaWindowPlayerState CMediaWindowConfig::GetBonusEventPlayerState() const
{
	return m_bonusEventPlayerState;
}

bool CMediaWindowConfig::SetBonusEventPlayerState(MediaWindowPlayerState state, bool localChange)
{
	bool conflict = false;
	bool result = SetConfigSetting(m_bonusEventPlayerState, state, MWC_SETTING_BONUSEVENT_PLAYERSTATE, localChange, conflict);
	if (result && IsPersistent())
	{
		byte stateByte = EEPROM_MW_BONUS_EVENT_PLAYER_STATE_DEFAULT;

		switch (state)
		{
		case MAINTAIN_PLAYER_STATE:
			stateByte = EEPROM_MW_BONUS_EVENT_PLAYER_STATE_MAINTAIN;
			break;
		case OPEN_PLAYER_STATE:
			stateByte = EEPROM_MW_BONUS_EVENT_PLAYER_STATE_OPEN;
			break;
		case CLOSE_PLAYER_STATE:
			stateByte = EEPROM_MW_BONUS_EVENT_PLAYER_STATE_CLOSE;
			break;
		default:
			;
		}

		if (!m_eepromConfig.Set(EEPROM_MW_BONUS_EVENT_PLAYER_STATE_OFFSET, &stateByte, EEPROM_MW_BONUS_EVENT_PLAYER_STATE_SIZE))
		{
			LogString(ERROR_LOG, "Could not write bonus event player state to EEPROM");
		}
	}

	return !conflict;
}

std::string CMediaWindowConfig::GetMediaWindowPlayerState(MediaWindowPlayerState mediaWindowPlayerState) const
{
	string mediaWindowPlayerStateString;

	switch (mediaWindowPlayerState)
	{
	case MAINTAIN_PLAYER_STATE:
		mediaWindowPlayerStateString = "MaintainState";
		break;
	case OPEN_PLAYER_STATE:
		mediaWindowPlayerStateString = "OpenWindow";
		break;
	case CLOSE_PLAYER_STATE:
		mediaWindowPlayerStateString = "CloseWindow";
		break;
	default:
		break;
	}

	return mediaWindowPlayerStateString;
}

MediaWindowPlayerState CMediaWindowConfig::GetMediaWindowPlayerState(const string& value) const
{
	MediaWindowPlayerState state;

	if (value == "MaintainState")
	{
		state = MAINTAIN_PLAYER_STATE;
	}
	else if (value == "OpenWindow")
	{
		state = OPEN_PLAYER_STATE;
	}
	else if (value == "CloseWindow")
	{
		state = CLOSE_PLAYER_STATE;
	}
	else
	{
		// should not get here
		state = MAINTAIN_PLAYER_STATE;
	}

	return state;
}

// GetCardInPlayerState
//   returns player state for Card In or Coin In
MediaWindowPlayerState CMediaWindowConfig::GetCardInPlayerState() const
{
	return m_cardInPlayerState;
}

// SetCardInPlayerState
//     IN state - whate the player state is for Card In or Coin In
//     IN localChange - whether settings are being done at the nCompass unit or remotely such as from SpeedMedia
//   Sets player state for Card In or Coin depending on localChange
//     if localChange is true all setting is affected and local change pending is set to true if changed
//     if localChange is false, setting is only changed if does not already have a local change pending
//     if localChange is false and setting already done, local change pending is cleared.
//	returns false if conflict exists between new value and pending change
bool CMediaWindowConfig::SetCardInPlayerState(MediaWindowPlayerState state, bool localChange)
{
	bool conflict = false;
	bool result = SetConfigSetting(m_cardInPlayerState, state, MWC_SETTING_CARDIN_PLAYERSTATE, localChange, conflict);
	if (result && IsPersistent())
	{
		byte stateByte = EEPROM_MW_CARD_IN_PLAYER_STATE_DEFAULT;

		switch (state)
		{
		case MAINTAIN_PLAYER_STATE:
			stateByte = EEPROM_MW_CARD_IN_PLAYER_STATE_MAINTAIN;
			break;
		case OPEN_PLAYER_STATE:
			stateByte = EEPROM_MW_CARD_IN_PLAYER_STATE_OPEN;
			break;
		case CLOSE_PLAYER_STATE:
			stateByte = EEPROM_MW_CARD_IN_PLAYER_STATE_CLOSE;
			break;
		default:
			;
		}

		if (!m_eepromConfig.Set(EEPROM_MW_CARD_IN_PLAYER_STATE_OFFSET, &stateByte, EEPROM_MW_CARD_IN_PLAYER_STATE_SIZE))
		{
			LogString(ERROR_LOG, "Could not write card in player state to EEPROM");
		}
	}

	return !conflict;
}

// GetCardOutPlayerState
//   returns player state for Card Out
MediaWindowPlayerState CMediaWindowConfig::GetCardOutPlayerState() const
{
	return m_cardOutPlayerState;
}

// SetCardOutPlayerState
//     IN state - whate the player state is for Card Out
//     IN localChange - whether settings are being done at the nCompass unit or remotely such as from SpeedMedia
//   Sets player state for Card Out depending on localChange
//     if localChange is true all setting is affected and local change pending is set to true if changed
//     if localChange is false, setting is only changed if does not already have a local change pending
//     if localChange is false and setting already done, local change pending is cleared.
//	returns false if conflict exists between new value and pending change
bool CMediaWindowConfig::SetCardOutPlayerState(MediaWindowPlayerState state, bool localChange)
{
	bool conflict = false;
	bool result = SetConfigSetting(m_cardOutPlayerState, state, MWC_SETTING_CARDOUT_PLAYERSTATE, localChange, conflict);
	if (result && IsPersistent())
	{
		byte stateByte = EEPROM_MW_CARD_OUT_PLAYER_STATE_DEFAULT;

		switch (state)
		{
		case MAINTAIN_PLAYER_STATE:
			stateByte = EEPROM_MW_CARD_OUT_PLAYER_STATE_MAINTAIN;
			break;
		case OPEN_PLAYER_STATE:
			stateByte = EEPROM_MW_CARD_OUT_PLAYER_STATE_OPEN;
			break;
		case CLOSE_PLAYER_STATE:
			stateByte = EEPROM_MW_CARD_OUT_PLAYER_STATE_CLOSE;
			break;
		default:
			;
		}

		if (!m_eepromConfig.Set(EEPROM_MW_CARD_OUT_PLAYER_STATE_OFFSET, &stateByte, EEPROM_MW_CARD_OUT_PLAYER_STATE_SIZE))
		{
			LogString(ERROR_LOG, "Could not write card out player state to EEPROM");
		}
	}

	return !conflict;
}

// GetFirstPlayPlayerState
//   returns player state for First Play
MediaWindowPlayerState CMediaWindowConfig::GetFirstPlayPlayerState() const
{
	return m_firstPlayPlayerState;
}

// SetFirstPlayPlayerState
//     IN state - whate the player state is for First Play
//     IN localChange - whether settings are being done at the nCompass unit or remotely such as from SpeedMedia
//   Sets player state for First Play depending on localChange
//     if localChange is true all setting is affected and local change pending is set to true if changed
//     if localChange is false, setting is only changed if does not already have a local change pending
//     if localChange is false and setting already done, local change pending is cleared.
//	returns false if conflict exists between new value and pending change
bool CMediaWindowConfig::SetFirstPlayPlayerState(MediaWindowPlayerState state, bool localChange)
{
	bool conflict = false;
	bool result = SetConfigSetting(m_firstPlayPlayerState, state, MWC_SETTING_FIRSTPLAY_PLAYERSTATE, localChange, conflict);
	if (result && IsPersistent())
	{
		byte stateByte = EEPROM_MW_FIRST_PLAY_PLAYER_STATE_DEFAULT;

		switch (state)
		{
		case MAINTAIN_PLAYER_STATE:
			stateByte = EEPROM_MW_FIRST_PLAY_PLAYER_STATE_MAINTAIN;
			break;
		case OPEN_PLAYER_STATE:
			stateByte = EEPROM_MW_FIRST_PLAY_PLAYER_STATE_OPEN;
			break;
		case CLOSE_PLAYER_STATE:
			stateByte = EEPROM_MW_FIRST_PLAY_PLAYER_STATE_CLOSE;
			break;
		default:
			;
		}

		if (!m_eepromConfig.Set(EEPROM_MW_FIRST_PLAY_PLAYER_STATE_OFFSET, &stateByte, EEPROM_MW_FIRST_PLAY_PLAYER_STATE_SIZE))
		{
			LogString(ERROR_LOG, "Could not write first play player state to EEPROM");
		}
	}
	return !conflict;
}

// GetAutoSwitch
//   returns the behavior when EGM is not in play for a given time 
byte CMediaWindowConfig::GetAutoSwitch() const
{
	return m_autoSwitchMask;
}

// Autoswitch helper functions

// IsAutoNone
//   returns true if AUTO_NONE_MASK is set true
bool CMediaWindowConfig::IsAutoNone() const
{
	return AUTO_NONE_MASK == m_autoSwitchMask;
}

// IsAutoOpen
//   returns true if AUTOOPEN_MASK is set true
bool CMediaWindowConfig::IsAutoOpen() const
{
	return 0 != (m_autoSwitchMask & AUTOOPEN_MASK);
}

// IsAutoClose
//   returns true if AUTOCLOSE_MASK is set true
bool CMediaWindowConfig::IsAutoClose() const
{
	return 0 != (m_autoSwitchMask & AUTOCLOSE_MASK);
}

//----------------------------

// SetAutoSwitch
//     IN state - mask describing behavior when EGM is not in play for a given time
//     IN localChange - whether settings are being done at the nCompass unit or remotely such as from SpeedMedia
//   Sets behavior depending on localChange
//     if localChange is true all setting is affected and local change pending is set to true if changed
//     if localChange is false, setting is only changed if does not already have a local change pending
//     if localChange is false and setting already done, local change pending is cleared.
//	returns false if conflict exists between new value and pending change
bool CMediaWindowConfig::SetAutoSwitch(byte mask, bool localChange)
{
	bool conflict = false;
	bool result = SetConfigSetting(m_autoSwitchMask, mask, MWC_SETTING_AUTO_SWITCH, localChange, conflict);
	if (result && IsPersistent())
	{
		byte autoSwitch = 0;

		if (IsAutoNone())
		{
			autoSwitch = EEPROM_MW_AUTO_OPEN_NO;
			if (!m_eepromConfig.Set(EEPROM_MW_AUTO_OPEN_OFFSET, &autoSwitch, EEPROM_MW_AUTO_OPEN_SIZE))
			{
				LogString(ERROR_LOG, "Could not write auto open to EEPROM");
			}

			autoSwitch = EEPROM_MW_AUTO_CLOSE_NO;
			if (!m_eepromConfig.Set(EEPROM_MW_AUTO_CLOSE_OFFSET, &autoSwitch, EEPROM_MW_AUTO_CLOSE_SIZE))
			{
				LogString(ERROR_LOG, "Could not write auto close to EEPROM");
			}
		}
		else
		{
			if (IsAutoClose())
			{
				autoSwitch = EEPROM_MW_AUTO_OPEN_YES;
				if (!m_eepromConfig.Set(EEPROM_MW_AUTO_OPEN_OFFSET, &autoSwitch, EEPROM_MW_AUTO_OPEN_SIZE))
				{
					LogString(ERROR_LOG, "Could not write auto open to EEPROM");
				}
			}

			if (IsAutoOpen())
			{
				autoSwitch = EEPROM_MW_AUTO_OPEN_YES;
				if (!m_eepromConfig.Set(EEPROM_MW_AUTO_CLOSE_OFFSET, &autoSwitch, EEPROM_MW_AUTO_CLOSE_SIZE))
				{
					LogString(ERROR_LOG, "Could not auto close to EEPROM");
				}
			}
		}
	}
	return !conflict;
}

// GetAutoSwitchWaitTime
//   returns time EGM is not in play before AutoSwitch behavior is done
WORD CMediaWindowConfig::GetAutoSwitchWaitTime() const
{
	return m_autoSwitchWaitTime;
}

// SetAutoSwitchWaitTime
//     IN time - time EGM is not in play before AutoSwitch behavior is done
//     IN localChange - whether settings are being done at the nCompass unit or remotely such as from SpeedMedia
//   Sets time depending on localChange
//     if localChange is true all setting is affected and local change pending is set to true if changed
//     if localChange is false, setting is only changed if does not already have a local change pending
//     if localChange is false and setting already done, local change pending is cleared.
//	returns false if conflict exists between new value and pending change
bool CMediaWindowConfig::SetAutoSwitchWaitTime(WORD time, bool localChange)
{
	bool conflict = false;
	bool result = SetConfigSetting(m_autoSwitchWaitTime, time, MWC_SETTING_AUTOSWITCH_WAITTIME, localChange, conflict);
	if (result && IsPersistent())
	{
		if (!m_eepromConfig.Set(EEPROM_MW_AUTO_MINUTES_OFFSET, (BYTE *)&m_autoSwitchWaitTime, EEPROM_MW_AUTO_MINUTES_SIZE))
		{
			LogString(ERROR_LOG, "Could not write banner size to EEPROM");
		}
	}
	return !conflict;
}

// GetTaskbarSize
//   returns size of banner when Media Window is closed
WORD CMediaWindowConfig::GetTaskbarSize() const
{
	return m_taskbarSize;
}

// SetTaskbarSize
//     IN size - size of banner when Media Window is closed
//     IN localChange - whether settings are being done at the nCompass unit or remotely such as from SpeedMedia
//   Sets size depending on localChange
//     if localChange is true all setting is affected and local change pending is set to true if changed
//     if localChange is false, setting is only changed if does not already have a local change pending
//     if localChange is false and setting already done, local change pending is cleared.
//	returns false if conflict exists between new value and pending change
bool CMediaWindowConfig::SetTaskbarSize(WORD size, bool localChange)
{
	bool conflict = false;
	bool result = SetConfigSetting(m_taskbarSize, size, MWC_SETTING_TASKBARSIZE, localChange, conflict);
	if (result && IsPersistent())
	{
		if (!m_eepromConfig.Set(EEPROM_TASKBAR_SIZE_OFFSET, (BYTE *)&m_taskbarSize, EEPROM_TASKBAR_SIZE))
		{
			LogString(ERROR_LOG, "Could not write banner size to EEPROM");
		}
	}
	return !conflict;
}

// GetContentWindowSize
//   returns size of Media Window when it is open
WORD CMediaWindowConfig::GetContentWindowSize() const
{
	return m_contentWindowSize;
}

// SetContentWindowSize
//     IN size - size of Media Window when it is open
//     IN localChange - whether settings are being done at the nCompass unit or remotely such as from SpeedMedia
//   Sets size depending on localChange
//     if localChange is true all setting is affected and local change pending is set to true if changed
//     if localChange is false, setting is only changed if does not already have a local change pending
//     if localChange is false and setting already done, local change pending is cleared.
//	returns false if conflict exists between new value and pending change
bool CMediaWindowConfig::SetContentWindowSize(WORD size, bool localChange)
{
	bool conflict = false;
	bool result = SetConfigSetting(m_contentWindowSize, size, MWC_SETTING_CW_SIZE, localChange, conflict);
	if (result && IsPersistent())
	{
		if (!m_eepromConfig.Set(EEPROM_SERVICE_WINDOW_SIZE_OFFSET, (BYTE *)&m_contentWindowSize, EEPROM_SERVICE_WINDOW_SIZE_SIZE))
		{
			LogString(ERROR_LOG, "Could not write media window size to EEPROM");
		}
	}
	return !conflict;
}

// GetContentWindowOrientation
//   returns whether Media Window when open is positioned right, left, or bottom
ContentWindowOrientation CMediaWindowConfig::GetContentWindowOrientation() const
{
	return m_contentWindowOrientation;
}

// SetContentWindowOrientation
//     IN orientation - whether Media Window when open is positioned right, left, or bottom
//     IN localChange - whether settings are being done at the nCompass unit or remotely such as from SpeedMedia
//   Sets orientation depending on localChange
//     if localChange is true all setting is affected and local change pending is set to true if changed
//     if localChange is false, setting is only changed if does not already have a local change pending
//     if localChange is false and setting already done, local change pending is cleared.
//	returns false if conflict exists between new value and pending change
bool CMediaWindowConfig::SetContentWindowOrientation(ContentWindowOrientation orientation, bool localChange)
{
	bool conflict = false;
	bool result = SetConfigSetting(m_contentWindowOrientation, orientation, MWC_SETTING_CW_ORIENTATION, localChange, conflict);
	if (result && IsPersistent())
	{
		if (!m_eepromConfig.Set(EEPROM_SERVICE_WINDOW_LOCATION_OFFSET, (BYTE *)&m_contentWindowOrientation, EEPROM_SERVICE_WINDOW_LOCATION_SIZE))
		{
			LogString(ERROR_LOG, "Could not write media window orientation to EEPROM");
		}
	}
	return !conflict;
}

// CMediaWindowConfig::GetGameCreditsBecameZeroBeginSessionDelayInMilliseconds
//   RETURN - the delay in milliseconds that game credits need to be zero before starting an (uncarded) player session.
DWORD CMediaWindowConfig::GetGameCreditsBecameZeroBeginSessionDelayInMilliseconds() const
{
	return GAME_CREDITS_BECAME_ZERO_BEGIN_SESSION_DELAY_IN_MILLISECONDS;
}

// CMediaWindowConfig::GetGameCreditsBecameZeroEndSessionDelayInMilliseconds
//   RETURN - the delay in milliseconds that game credits need to be zero before ending an (uncarded) player session.
DWORD CMediaWindowConfig::GetGameCreditsBecameZeroEndSessionDelayInMilliseconds() const
{
	return GAME_CREDITS_BECAME_ZERO_END_SESSION_DELAY_IN_MILLISECONDS;
}

// CMediaWindowConfig::GetGameNotInProgressBeginSessionDelayInMilliseconds
//   RETURN - the timeout period in milliseconds after player card out for game credits to become zero
//      for a player state change due to player card out.
DWORD CMediaWindowConfig::GetGameNotInProgressBeginSessionDelayInMilliseconds() const
{
	return GAME_NOT_IN_PROGRESS_BEGIN_SESSION_DELAY_IN_MILLISECONDS;
}

// CMediaWindowConfig::GetGameNotInProgressEndSessionDelayInMilliseconds
//     RETURN - the timeout period in milliseconds after EGM game ended before
//         performing the Media Window Player State Card Out behavior.
DWORD CMediaWindowConfig::GetGameNotInProgressEndSessionDelayInMilliseconds() const
{
	return GAME_NOT_IN_PROGRESS_END_SESSION_DELAY_IN_MILLISECONDS;
}

// CMediaWindowConfig::GetCardOutGameCreditsBecomeZeroTimeoutInMilliseconds
//   RETURN - the timeout period in milliseconds after player card out for game credits to become zero
//      for a player state change due to player card out.
DWORD CMediaWindowConfig::GetCardOutGameCreditsBecomeZeroTimeoutInMilliseconds() const
{
	return CARD_OUT_GAME_CREDITS_BECOME_ZERO_TIMEOUT_IN_MILLISECONDS;
}

// CMediaWindowConfig::CheckLocalChange
// Return bool value specifying if this media window config contains changes
bool CMediaWindowConfig::CheckLocalChange()
{
	return m_localChangePendingBitMap == 0 ? false : true;
}

// CMediaWindowConfig::IsOrientationChanged
// IN
//	 CMediaWindowConfig *newConfig - settings to compare current orientation agianst
// RETURN
// bool - True if the orientation moved from side to bottom or bottom to side. Left to right will return false.
bool CMediaWindowConfig::IsOrientationChanged(ContentWindowOrientation newOrientation)
{
	bool retVal = false;
	ContentWindowOrientation curOrientation = GetContentWindowOrientation();

	if ((curOrientation == CWORIENTATION_BOTTOM_OR_LCD && (newOrientation == CWORIENTATION_LEFT || newOrientation == CWORIENTATION_RIGHT)) ||
		(newOrientation == CWORIENTATION_BOTTOM_OR_LCD && (curOrientation == CWORIENTATION_LEFT || curOrientation == CWORIENTATION_RIGHT)))
	{
		retVal = true;
	}

	return retVal;
}

// CMediaWindowConfig::IsServiceWindowChange
// RETURN
//    bool - True if the media window orientation, size, or banner size change is pending
bool CMediaWindowConfig::IsServiceWindowChangePending()
{
	bool retVal = false;
	if (GetLocalChangePending(MWC_SETTING_CW_SIZE) ||
		GetLocalChangePending(MWC_SETTING_TASKBARSIZE) ||
		GetLocalChangePending(MWC_SETTING_CW_ORIENTATION))
	{
		retVal = true;
	}
	return retVal;
}
