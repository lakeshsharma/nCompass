#pragma once
#include "../InternalMsgContent.h"
#include "EEPROMConfig.h"

#include "../GlobalDefs.h"
#include "../Hardware/NonVolatileMemoryMaps.h"
#include "EEPROMConfig.h"
#include "DataSubscriptionProvider.h"

// For each setting, the following needs to be implemented
// 1. A 'get' method
// 2. A 'set' method
// 3. Define 'local change pending' bit or id
// 4. The Default value
// 5. Initialize in constructors and assigment
// 6. Persistance in EEPROM (or potentially in the future NVRAM)
// 7. Text used to set from file

enum MediaWindowPlayerState
{
	MAINTAIN_PLAYER_STATE = 0,
	OPEN_PLAYER_STATE,
	CLOSE_PLAYER_STATE,
};

// This enumeration is used to identify posistions in a bit mask which is also saved to the EEPROM.
// Do not change the values of existing Ids. Otherwise, you will introduce backwards incompatibility.
enum MediaWindowConfigSettingId
{
	MWC_SETTING_MAINTAIN_ASPECT_RATIO = 0,
	MWC_SETTING_CARDIN_PLAYERSTATE = 1,
	MWC_SETTING_CARDOUT_PLAYERSTATE = 2,
	MWC_SETTING_FIRSTPLAY_PLAYERSTATE = 3,
	MWC_SETTING_AUTO_SWITCH = 4,
	MWC_SETTING_AUTOSWITCH_WAITTIME = 5,
	MWC_SETTING_TASKBARSIZE = 6,
	MWC_SETTING_CW_SIZE = 7,
	MWC_SETTING_CW_ORIENTATION = 8,
	MWC_SETTING_BONUSEVENT_PLAYERSTATE = 9,
	MWC_SETTING_LAST
};

class CMediaWindowConfig : public CDataSubscriptionProvider,
	public CInternalMsgContent<INT_MSG_CONTENT_MEDIAWINDOW_CONFIG>
{
public:
	static const byte AUTO_NONE_MASK = 0x0;
	static const byte AUTOCLOSE_MASK = 0x1;
	static const byte AUTOOPEN_MASK = 0x2;

	static const bool DEFAULT_MAINTAIN_ASPECT_RATIO;
	static const MediaWindowPlayerState DEFAULT_BONUSEVENT_PLAYERSTATE;
	static const MediaWindowPlayerState DEFAULT_CARDIN_PLAYERSTATE;
	static const MediaWindowPlayerState DEFAULT_CARDOUT_PLAYERSTATE;
	static const MediaWindowPlayerState DEFAULT_FIRSTPLAY_PLAYERSTATE;
	static const byte DEFAULT_AUTO_SWITCH_MAP;
	static const WORD DEFAULT_AUTOSWITCH_WAITTIME;
	static const WORD DEFAULT_TASKBARSIZE;
	static const WORD DEFAULT_CW_SIZE;
	static const ContentWindowOrientation DEFAULT_CW_ORIENTATION;

	static const LPCTSTR DESC_MAINTAIN_ASPECT_RATIO;
	static const LPCTSTR DESC_BONUSEVENT_PLAYERSTATE;
	static const LPCTSTR DESC_CARDIN_PLAYERSTATE;
	static const LPCTSTR DESC_CARDOUT_PLAYERSTATE;
	static const LPCTSTR DESC_FIRSTPLAY_PLAYERSTATE;
	static const LPCTSTR DESC_AUTOSWITCH;
	static const LPCTSTR DESC_AUTOSWITCH_WAITTIME;
	static const LPCTSTR DESC_TASKBARSIZE;
	static const LPCTSTR DESC_CW_SIZE;
	static const LPCTSTR DESC_CW_ORIENTATION;

	static CMediaWindowConfig* Instance();

protected:
	// Constructor and operators moved to 'protected' to prevent use outside MediaWindowConfig.h/cpp
	CMediaWindowConfig();
	CMediaWindowConfig &operator=(const CMediaWindowConfig& rhs);

	virtual void BuildYourself();

public:
	CMediaWindowConfig(const CMediaWindowConfig &rhs);
	virtual ~CMediaWindowConfig();

	bool operator==(const CMediaWindowConfig& rhs) const;

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

	bool IsPersistent() const;

	// Autoswitch helper functions
	bool IsAutoNone() const;
	bool IsAutoOpen() const;
	bool IsAutoClose() const;
	//----------------------------

	void SetFromConfigAlways(const CMediaWindowConfig& mediaWindowConfig);
	bool SetFromConfig(const CMediaWindowConfig& mediaWindowConfig, bool localChange);
	void SetFromFile(const std::string &szFilePath, bool localChange);

	bool GetLocalChangePending(MediaWindowConfigSettingId settingId) const;
	void ClearAllLocalChangePending();
	bool CheckLocalChange();

	bool GetMaintainAspectRatio() const;
	bool SetMaintainAspectRatio(bool state, bool localChange);

	/// <summary>
	/// Gets the media window auto open/close state for bonus events and announcements.
	/// </summary>
	/// <returns>The media window auto open/close state for bonus events and announcements.</returns>
	MediaWindowPlayerState GetBonusEventPlayerState() const;

	/// <summary>
	/// Sets the media window auto open/close state for bonus events and announcements.
	/// </summary>
	/// <param name="state">IN - The new state.</param>
	/// <param name="localChange">IN - true if if the change was made locally at the nCompass unit, false otherwise (such as remotely from SpeedMedia).</param>
	/// <returns>true if a conflict exists between the new value and a pending change, returns false otherwise.</returns>
	/// <remarks>
	///   Sets the state depending on localChange
	///     if localChange is true, setting is affected and local change pending is set to true if changed
	///     if localChange is false, setting is only changed if does not already have a local change pending
	///     if localChange is false and setting is already the same, local change pending is cleared.
	/// </remarks>
	bool SetBonusEventPlayerState(MediaWindowPlayerState state, bool localChange);

	std::string GetMediaWindowPlayerState(MediaWindowPlayerState mediaWindowPlayerState) const;
	MediaWindowPlayerState GetMediaWindowPlayerState(const std::string& value) const;

	MediaWindowPlayerState GetCardInPlayerState() const;
	bool SetCardInPlayerState(MediaWindowPlayerState state, bool localChange);

	MediaWindowPlayerState GetCardOutPlayerState() const;
	bool SetCardOutPlayerState(MediaWindowPlayerState state, bool localChange);

	MediaWindowPlayerState GetFirstPlayPlayerState() const;
	bool SetFirstPlayPlayerState(MediaWindowPlayerState state, bool localChange);

	byte GetAutoSwitch() const;
	bool SetAutoSwitch(byte state, bool localChange);

	WORD GetAutoSwitchWaitTime() const;
	bool SetAutoSwitchWaitTime(WORD time, bool localChange);

	WORD GetTaskbarSize() const;
	bool SetTaskbarSize(WORD size, bool localChange);

	WORD GetContentWindowSize() const;
	bool SetContentWindowSize(WORD size, bool localChange);

	ContentWindowOrientation GetContentWindowOrientation() const;
	bool SetContentWindowOrientation(ContentWindowOrientation orientation, bool localChange);

	DWORD GetGameCreditsBecameZeroBeginSessionDelayInMilliseconds() const;
	DWORD GetGameCreditsBecameZeroEndSessionDelayInMilliseconds() const;
	DWORD GetCardOutGameCreditsBecomeZeroTimeoutInMilliseconds() const;
	DWORD GetGameNotInProgressBeginSessionDelayInMilliseconds() const;
	DWORD GetGameNotInProgressEndSessionDelayInMilliseconds() const;
	bool IsOrientationChanged(ContentWindowOrientation newOrientation);
	bool IsServiceWindowChangePending();

private:
	static CMediaWindowConfig* m_instance;
	static std::recursive_mutex m_instanceCS;

	static const DWORD GAME_CREDITS_BECAME_ZERO_BEGIN_SESSION_DELAY_IN_MILLISECONDS;
	static const DWORD GAME_CREDITS_BECAME_ZERO_END_SESSION_DELAY_IN_MILLISECONDS;
	static const DWORD CARD_OUT_GAME_CREDITS_BECOME_ZERO_TIMEOUT_IN_MILLISECONDS;
	static const DWORD GAME_NOT_IN_PROGRESS_BEGIN_SESSION_DELAY_IN_MILLISECONDS;
	static const DWORD GAME_NOT_IN_PROGRESS_END_SESSION_DELAY_IN_MILLISECONDS;

	bool m_persistent;

	bool m_maintainAspectRatio;
	MediaWindowPlayerState m_bonusEventPlayerState;
	MediaWindowPlayerState m_cardInPlayerState;
	MediaWindowPlayerState m_cardOutPlayerState;
	MediaWindowPlayerState m_firstPlayPlayerState;
	byte m_autoSwitchMask;
	WORD m_autoSwitchWaitTime;	// In minutes
	WORD m_taskbarSize;
	WORD m_contentWindowSize;
	ContentWindowOrientation m_contentWindowOrientation;

	DWORD m_localChangePendingBitMap;

	CEEPROMConfig m_eepromConfig;

	void BuildMaintainAspectRatio();
	void BuildBonusEventState();
	void BuildCardInState();
	void BuildCardOutState();
	void BuildFirstPlayState();
	void BuildAutoSwitchMap();
	void BuildAutoSwitchTime();
	void BuildContentWindowOrientation();
	void BuildContentWindowSize();
	void BuildTaskbarSize();

	void SetLocalChangePending(MediaWindowConfigSettingId settingId, bool state);

	DWORD GetLocalChangePendingMask(MediaWindowConfigSettingId settingId) const;

	template<class ConfigSettingType> bool SetConfigSetting(ConfigSettingType& setting, ConfigSettingType value, MediaWindowConfigSettingId settingId, bool localChange, bool &bConflict);
};

template<class ConfigSettingType> bool CMediaWindowConfig::SetConfigSetting(ConfigSettingType& setting, ConfigSettingType value, MediaWindowConfigSettingId settingId, bool localChange, bool &bConflict)
{
	bool settingWasChanged = false;
	bConflict = false;

	if (setting == value && !localChange)
	{
		// Clear local change flag
		SetLocalChangePending(settingId, false);
	}
	else if (setting != value && (localChange || !GetLocalChangePending(settingId)))
	{
		setting = value;
		SetLocalChangePending(settingId, localChange);
		settingWasChanged = true;
	}
	else if (setting != value && !localChange && GetLocalChangePending(settingId))
	{
		bConflict = true;
	}

	return settingWasChanged;
}
