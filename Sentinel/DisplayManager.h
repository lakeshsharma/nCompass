#pragma once

#include "DataSubscriptionProvider.h"
#include "DirectoryManager.h"
#include "DisplayManagerConfig.h"
#include "DisplayManagerPlayerState.h"
#include "UI/Logs.h"
#include "Config/Config.h"
#include "VideoModeConfig.h"
#include "VideoMixerConfigFile.h"
#include "VideoMixerDiagnostics.h"
#include "Util/Manipulators.h"
#include "Calibration.h"
#include "TouchScreenProtocols.h"

// The employee window height percentage for Media Window
const WORD EmployeeScreenHeightPercentage = 25;

typedef enum
{
	VMDO_TOP,
	VMDO_BOTTOM,
	VMDO_LEFT,
	VMDO_RIGHT,
	VMDO_CENTER,
} VideoMixerDisplayOrientation;

typedef enum
{
	CARD_INSERTED_NONE,
	CARD_INSERTED_PLAYER,
	CARD_INSERTED_EMPLOYEE,
} InsertedCardType;

typedef enum
{
	VMM_TFT,
	VMM_SERVICE_WINDOW,
	VMM_BANNER,
	VMM_EMPLOYEE_BANNER,
	VMM_FULLEGM,
	VMM_FULL_SERVICE_WINDOW,
	VMM_EMPLOYEE_SERVICE_WINDOW,
	VMM_SERVICE_WINDOW_CDM,
	VMM_BANNER_CDM,
	VMM_SHUTDOWN,
	NUMBER_OF_VM_MODES
} VideoMixerMode;

extern const char* VideoMixerModeStr[];

typedef enum
{
	CDMPLAY_NONE,
	CDMPLAY_CONTENT,
	CDMPLAY_POPUPTEXT,
} CDMPlayType;

enum MediaWindowPlayerStatePending
{
	MAINTAIN_PLAYER_STATE_PENDING = 0, // AKA nothing pending.
	OPEN_PLAYER_STATE_PENDING,
	OPEN_PLAYER_STATE_PENDING_IGNORE_CREDIT_METER,
	OPEN_PLAYER_STATE_PENDING_CREDIT_METER_ZERO_DELAY, // Need credit meter to stay zero during delay period.
	OPEN_PLAYER_STATE_PENDING_CARD_OUT_TIMEOUT,  // Need credit meter to become zero before timeout.
	OPEN_PLAYER_STATE_PENDING_GAME_PLAY_ENDED_DELAY, // Need EGM idle during delay period.
	CLOSE_PLAYER_STATE_PENDING,
	CLOSE_PLAYER_STATE_PENDING_IGNORE_CREDIT_METER,
	CLOSE_PLAYER_STATE_PENDING_CREDIT_METER_ZERO_DELAY, // Need credit meter to stay zero during delay period.
	CLOSE_PLAYER_STATE_PENDING_CARD_OUT_TIMEOUT,  // Need credit meter to become zero before timeout.
	CLOSE_PLAYER_STATE_PENDING_GAME_PLAY_ENDED_DELAY, // Need EGM idle during delay period.
};

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class DeviceInfoSet; }
namespace SentinelNativeUnitTests { class DisplayManagerUnitTests; }

const int VEBDATASIZE = 128;

class CDisplayManager: public CDataSubscriptionProvider
{
	// Grant "friend" access for testing.
	friend class CDisplayManagerWrapper;
	friend class SentinelNativeUnitTests::DeviceInfoSet;
	friend class SentinelNativeUnitTests::DisplayManagerUnitTests;

public:

	static CDisplayManager & Instance();

	static void CleanupInstance();
	
	// CDisplayManager::Instance
	// 
	
	/// <summary>
	/// CDisplayManager::Instance. FReturns the CDisplayManager instance after 
	/// performing updates as indicated in the parameters.. 
	/// </summary>
	/// <param name="tftMode"> If true then LCD type display.</param>
	/// <param name="serviceWindowOpen">Indicates if the current media window state
	/// is open.</param>
	/// <param name="mediaWindowConfig">Indicates parameters for the current media
	/// window configuration.</param>
	/// <param name="gameHasCredits">Indicates the last known state for whether the
	/// EGM has credits.</param>
	/// <param name="gameInProgress">Indicates the last known state for whether the
	/// EGM has a game in progress. </param>
	/// <returns>the CDisplayManager instance after updating the instance state to 
	/// match the parameters specified.</returns> 
	static CDisplayManager & Instance(bool serviceWindowOpen, const CMediaWindowConfig &mediaWindowConfig,
									  bool gameHasCredits, bool gameInProgress);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CDisplayManager"/> class.
	/// </summary>
	/// <remarks>
	/// Make destructor public to simplify creating derived classes for unit tests.
	/// </remarks>
	virtual ~CDisplayManager();

	/// <summary> 
	/// Starts up the display manger. 
	/// </summary>
	/// <param name="serviceWindowOpen">Indicates if to be started with the 
	/// service window open</param> 
	void Start(bool serviceWindowOpen);

	/// <summary>
	/// Determines if the hardware is tft (AKA LCD).
	/// </summary>
	/// <returns>true if hardware is tft. False if Video Mixer.</returns>
	static bool IsHardwareTFT( void );
	
	/// <summary>
	/// Determines if the hardware is compact (AKA 4.3).
	/// </summary>
	/// <returns>true if hardware is compact. False if Video Mixer or 206.</returns>
	static bool IsHardwareCompact(void);

	/// <summary>
	/// Gets the package Orientation based on ContentWindowOrientation.
	/// </summary>
	/// <param name="contentWindowOrientation">The content window Orientation.</param>
	PackageOrientation GetPackageOrientation(ContentWindowOrientation contentWindowOrientation) const;

	/// <summary>
	/// Determines true if we are running in tft (AKA LCD) mode.
	/// </summary>
	/// <returns>true if running in lcd mode. False if media window.</returns>
	bool IsTFTMode() const;

	/// <summary>
	/// Determines true if we are running in compact (AKA 4.3) mode.
	/// </summary>
	/// <returns>true if running in compact mode. False if media window or 206.</returns>
	bool IsCompactMode() const;
	
	DisplayPlatform GetDisplayPlatform();

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="topic">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

	/// <summary>
	/// Gets the firmware versions.
	/// </summary>
	/// <param name="mixerVersion">The mixer version.</param>
	/// <param name="touchVersion">The touch version.</param>
	void GetFirmwareVersions(std::string& mixerVersion, std::string& touchVersion);
	
	/// <summary> 
	/// Writes current media window config to the log file. 
	/// </summary>
	void LogMediaWindowConfig();

	/// <summary> 
	/// Opens the service (content) window. 
	/// </summary>
	void ShowServiceWindow();
	
	/// <summary> 
	/// Closes the service (content) window. 
	/// </summary>
	void HideServiceWindow();
	
	/// <summary> 
	/// Called when a valid player card is inserted. 
	/// </summary>
	/// <param name="pbtInProgress"> 
	/// IN - indicates whether PBT operations and/or PBT displays are pending or in 
	/// progress that affect CDisplayManager operation. 
	/// </param> 
	/// <param name="playerStatePending"> 
	/// OUT - determines if a media window player state change is pending. 
	/// </param> 
	/// <returns>whether a media window open/close change occurred.</returns>
	MediaWindowPlayerState PlayerCardInserted(bool pbtInProgress, MediaWindowPlayerStatePending &playerStatePending);
	
	/// <summary> 
	/// Called when a valid employee card is inserted. 
	/// </summary>
	/// <param name="closeServiceWindow">OUT - Set to true if the the 
	/// service window is to be closed. Set to true is already closed</param> 
	/// <param name="startCalibration">OUT - Set to true if a calibration is 
	/// to be started</param> 
	/// <returns>True if either of the output paramaters are set true</returns>
	bool EmployeeCardInserted(bool &closeServiceWindow, bool &startCalibration);

	/// Updates the video mixer, displayed windows, and/or state for a player card removed.
	/// </summary>
	/// <param name="pbtInProgress">true if PBT operations and/or PBT displays are pending or in progress that affect CDisplayManager operation, false otherwise.</param>
	/// <param name="playerStatePending">IN/OUT - The player state pending.</param>
	/// <returns>Instructions for the UI to maintain or change to a new Media Window player state.</returns>
	/// <remarks>
	/// The CDisplayManager instance internal state is updated to match the pbtInProgress parameter value.
	/// </remarks>
	MediaWindowPlayerState PlayerCardRemoved(bool pbtInProgress,
		MediaWindowPlayerStatePending &playerStatePending, bool bannedPlayerIsDisplayed = false);

	/// <summary> 
	/// Update state for employee card removed.
	/// </summary>
	/// <param name="restartUI">OUT - set to true if the UI needs to be 
	/// restarted as a result</param> 
	void EmployeeCardRemoved(bool &restartUI);
	
	/// <summary> 
	/// Set THE media window config. 
	/// </summary>
	/// <param name="mediaWindowConfig">config that THE media window config 
	/// is to be set from</param> 
	void SetMediaWindowConfig(const CMediaWindowConfig &mediaWindowConfig);

	/// <summary> 
	/// Get the current service (content) window orientation. 
    /// </summary> 
    /// <returns>The current content window orientation</returns> 
	ContentWindowOrientation GetServiceWindowOrientation();

    /// <summary> 
    /// Get the current desktop rotation setting. 
    /// </summary> 
    /// <returns>Returns the desktop orientation for the 
    /// current rotation</returns> 
    DesktopOrientationType GetDesktopRotation();

    /// <summary> 
    /// Sets the desktop rotation setting. 
    /// </summary> 
    /// <param name="desktopOrientation"> The new desktop 
    /// orientation</param> 
    /// <returns>Returns true if the rotation has been 
    /// changed</returns> 
    bool SetDesktopRotation(DesktopOrientationType desktopOrientation, bool makeFullScreenEGM = true);

    /// <summary> 
    /// Get the current touchscreen rotation setting. 
    /// </summary> 
    /// <returns>Returns the desktop orientation type for the 
    /// current touchscreen rotation</returns> 
    DesktopOrientationType GetTouchScreenRotation();

    /// <summary> 
    /// Sets the touchscreen rotation setting. 
    /// </summary> 
    /// <param name="touchscreenRotation"> The new touchscreen 
    /// rotation</param> 
    /// <returns>Returns true if the rotation has been 
    /// changed</returns> 
    bool SetTouchScreenRotation(DesktopOrientationType touchscreenRotation);
	
	/// <summary> 
	/// Sets the current video mixer display mode sizes based upon the current card 
	/// state and sizes and positions the video mixer windows based upon the updated
	/// mode. 
	/// </summary>
	/// <param name="forceUpdate">If true, this video mixer sizes will 
	/// always be updated regardless of a change in state</param> 
	void SetVideoMixerDisplay(bool forceUpdate=false);
	
	/// <summary> 
	/// Sizes and positions the video mixer windows based upon the current video 
	/// mixer mode. 
	/// </summary>
	void UpdateVideoMixerDisplay();
		
	/// <summary> Indicates that the game credits have gone from non-zero to zero. 
	/// </summary>
	/// <param name="playerStatePending"> 
	/// OUT playerStatePending - determines if a media window player state change is 
	/// pending. 
	/// </param> 
	void GameCreditsBecameZero(MediaWindowPlayerStatePending &playerStatePending);
	
	/// <summary> Indicates that the game credits have gone from zero to non-zero. 
	/// </summary>
	/// <param name="pbtInProgress"> 
	/// </param> 
	/// <param name="playerStatePending"> 
	/// OUT playerStatePending - determines if a media window player state change is 
	/// pending. 
	/// </param> 
	MediaWindowPlayerState GameCreditsBecameNonZero(bool pbtInProgress, MediaWindowPlayerStatePending &playerStatePending);
	
	/// <summary> 
	/// Called when an EGM game has started. 
	/// </summary>
	void GameInProgress();

	/// <summary> 
	/// Called when an EGM game has ended (or is not in progress). 
	/// </summary>
	/// <param name="pbtInProgress"> 
	/// Indicates whether PBT operations and/or PBT displays are pending or in 
	/// progress that affect CDisplayManager operation.
	/// </param> 
	/// <param name="playerStatePendingArg"> 
	/// IN/OUT - indicates if a media window player state change is pending.
	/// </param> 
	/// <returns>
	/// whether a media window open/close change occurred.
	/// </returns>
	/// <remarks> If this game end is for the first play (of a carded or uncarded
	/// player session) and it causes an immediate or pending state change then the
	/// incoming playerStatePendingArg value is ignored; otherwise the incoming
	/// playerStatePendingArg value is retained.
	/// </remarks>
	MediaWindowPlayerState GameNotInProgress(bool pbtInProgress, MediaWindowPlayerStatePending &playerStatePendingArg);
	
	/// <summary> 
	/// Indicates that EGM communications is down. 
	/// </summary>
	void GameCommDown();
	
	/// <summary> 
	/// Indicates that EGM communications is up. 
	/// </summary>
	void GameCommUp();
	
	/// <summary> 
	/// Checks to see if it is time to automatically open/close the content window.
	/// </summary>
	/// <param name="pbtInProgress"> 
	/// Indicates that a PBT is in progress. 
	/// </param> 
	/// <param name="playerStatePending"> 
	/// IN/OUT - determines if a media window player state change is waiting for
	/// state changes before performing an open or close operation.
	/// </param> 
	/// <returns>
	/// whether a media window open/close change occurred.
	/// </returns>
	/// <remarks> Automatically opens or closes the media window based on 
	/// playerStatePending and other internal state information. This routine should be called multiple times per second 
	/// while playerStatePending != MAINTAIN_PLAYER_STATE_PENDING in order to perform updates in a timely manner 
	/// -- otherwise, calling this routine at least once every few seconds should 
	///    be sufficient for satisfactory operation.
	/// </remarks>
	MediaWindowPlayerState AutoOpenCloseServiceWindowCheck(bool pbtInProgress, MediaWindowPlayerStatePending &playerStatePending);

	bool IsServiceWindowOpen()
	{
		return m_serviceWindowIsOpen;
	}
	
	bool IsInitialized()
	{
		return m_isInitialized;
	}
	void SetLastCardRead(const byte* cardID);
	void ClearLastCardRead( void );
    byte *GetLastCardRead();
	
	void MakeDisplayFullEGM();
	void MakeDisplayFullServiceWindow();
	
	/// <summary> 
	/// Notifies that the UI restart is complete. 
	/// </summary>
	/// <returns>The status of the video config change. This may result in the need
	/// to display the revert UI</returns> 
	VideoConfigChangeStatus UIRestartComplete();
	
	void PrepareDisplayForShutdown();

	/// <summary> 
	/// Sets video mode from the provided config object. 
	/// </summary>
	/// <param name="videoModeConfig">the video mode configuration to be set</param>
	/// <returns>The status of the change. This may result in the need to 
	/// restart the UI 
	/// </returns> 
	VideoConfigChangeStatus SetVideoModeConfig(CVideoModeConfig *videoModeConfig);

    /// <summary> Set touch protocol. 
    /// </summary>
    /// <param name="protocolName">the name of the protocol 
    /// to be set 
    /// </param> 
    void SetTouchProtocol(std::string protocolName);
	
    /// <summary> Indicates that the video mode changes are not 
    /// going to be reverted. 
	/// </summary>
	void KeepVideoModeChange();
	
	/// <summary> 
	/// Reverts video mode to the stored config if available. 
	/// </summary>
	/// <returns>The status of the change. This may result in the need to 
	/// restart the UI.
	/// </returns> 
	VideoConfigChangeStatus RevertVideoMode();
	
    /// <summary> This will check for the egm display if it was not 
    /// available at start-up. 
	/// </summary>
    void CheckForEGMDisplay();

	/// <summary> 
	/// Checks to see if the timer expired to revert the video mode. 
	/// </summary>
	/// <returns>The status of the change. This may result in the need to 
	/// restart the UI.
	/// </returns> 
	VideoConfigChangeStatus CheckForVideoModeRevert();
	
	/// <summary> 
	/// Checks to see if if there is an external video Mode Config file to be 
	/// applied. 
	/// </summary>
	/// <returns>The status of the change. This may result in the need to 
	/// restart the UI.
	/// </returns> 
	VideoConfigChangeStatus CheckForExternalVideoModeConfig();

	/// <summary> 
    /// Checks to see if if there is an external video mixer 
    /// diagnostics file to be populated. 
	/// </summary>
    /// <returns>True if diagnostics is dumped to external file.
    /// </returns> 
    bool CheckForExternalVideoMixerDiagnosticsFile();
	
	/// <summary> 
	/// Checks if DIsplayManger thinks it's okay to restart the UI 
	/// </summary>
	/// <returns>true if okay to restart the UI.
	/// </returns> 
	bool OKToRestartUI();
	
	/// <summary> 
	/// Starts the calibration process. 
	/// </summary>
	/// <returns> 
	/// The calibration state. 
	/// </returns> 
	Calibration::CalibrationState StartCalibration();
	
	/// <summary> Checks if calibration process is complete. 
	/// </summary>
	/// <returns> 
	/// The calibration state. 
	/// </returns> 
	Calibration::CalibrationState CheckForCalibrationComplete();
	
	/// <summary> Notifies that the Browser was killed. 
	/// </summary>
	void BrowserTerminated();

	/// <summary> Perform a clear on the mixer hardware 
	/// </summary>
    void ClearMixerHardware();

	/// <summary> Gets the resolution of the desktop. 
	/// </summary>
    /// <returns> The size of the desktop. 
	/// </returns> 
    SIZE GetDesktopResolution();

	/// <summary> 
	/// Checks if there was a calibratione error. 
	/// </summary>
	/// <returns> 
	/// true if there was calibration error, false otherwise. 
	/// </returns> 
	bool CalibrationError() const;

	/// <summary> Static function to convert the given desktop orientation to a string. 
	/// </summary>
	/// <param name="desktopOrientation">the desktop orientation to convert</param>
	/// <returns> 
	/// The string representation of the orientation. 
	/// </returns> 
    static std::string ConvertDesktopOrientationToString(DesktopOrientationType desktopOrientation);

	/// <summary> Static function to convert the given desktop orientation to a word value. 
	/// </summary>
	/// <param name="desktopOrientation">the desktop orientation to convert</param>
	/// <returns> 
	/// The numerical representation of the orientation. 
	/// </returns> 
	static WORD ConvertDesktopOrientationToWord(DesktopOrientationType desktopOrientation);

	/// <summary> Static function to convert the given string to a desktop orientation. 
	/// </summary>
	/// <param name="desktopOrientation">the string to convert</param>
	/// <returns> 
	/// The desktop orientation for the supplied string. 
	/// </returns> 
	static DesktopOrientationType ConvertDesktopOrientationFromString(const std::string& desktopOrientationString);

	/// <summary> Static function to convert the given word to a desktop orientation. 
	/// </summary>
	/// <param name="desktopOrientation">the number to convert</param>
	/// <returns> 
	/// The desktop orientation for the supplied number. 
	/// </returns> 
    static DesktopOrientationType ConvertDesktopOrientationFromWord(WORD desktopOrientationWord);
	
private:
	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CTicketingInfo"/> class.
	/// </summary>
	CDisplayManager();
	DisplayPlatform GetDisplayPlatform(bool isTFTMode, bool isCompactMode);

	// The singleton instance
	static CDisplayManager *m_instance;
	static std::recursive_mutex m_instanceCS;

	CMediaWindowConfig m_mediaWindowConfig;
	CDisplayManagerPlayerState m_playerState;
	enum MediaWindowPlayerStateCreditMeterMode
	{
		PLAYER_STATE_CREDIT_METER_IGNORE, // Ignore credit meter when determining whether to change state.
		PLAYER_STATE_CREDIT_METER_NORMAL, // Require normal credit meter when determining whether to change state.
			// 2013-01-10 Normal credit meter requirement is for credit meter to be zero for at least GAME_CHG_SECONDS_TO_AUTO_OPEN.
		PLAYER_STATE_CREDIT_METER_WAIT_FOR_ZERO, // Wait for credit meter to become zero when determining whether to change state.
	};

	std::recursive_mutex m_settingWindowsCS;
	
	// Persistent configuration as needed for internal implementation.
	CDisplayManagerConfig m_displayManagerConfig;
	bool m_tftMode;
	bool m_compactMode;
	bool m_lcdIsPresent;
	InsertedCardType m_insertedCard;
	bool m_serviceWindowIsOpen;
	bool m_isInitialized;
	time_t m_serviceWindowAutoOpenTimer;
	time_t m_serviceWindowDelayOpenTimer;
	WORD m_serviceWindowDelayOpenTimerPeriod;
	volatile DWORD m_gameCreditsBecameZeroTickCount; // Set before m_gameHasCredits to avoid race conditions.
	volatile bool m_gameHasCredits;
	bool m_gameCommUp;
	bool m_pbtInProgress;
	volatile DWORD m_playerCardOutTickCount;
	VideoMixerMode m_vmMode;
	VideoMixerMode m_vmModeLast;

	// Service window is NOT a Windows window; it is a portion of the service application
	// that used for nCompass content area that can be opened and closed.
	// m_serviceWindowSize is the openable/closeable Sentinel content area.
	// m_serviceWindowSize is typically about 25% of the service application area.
	WORD	m_serviceWindowSize;
	WORD	m_taskbarSize;
	
	// This is the video mode configuration we will revert to if the new configuration
	//  is not acknowledged by the user.
	std::recursive_mutex m_revertVideoModeConfigMutex;
	std::unique_ptr<CVideoModeConfig> m_revertVideoModeConfig;
	
	// This indicates that there is an external video mode config that is currenly being processed.
	std::unique_ptr<VideoMixerConfigFile> m_externalVideoMixerConfig;
    volatile DWORD m_externalVideoConfigCheckTick;

    VideoMixerDiagnostics m_videoMixerDiagnostics;

	DesktopOrientationType m_desktopOrientation;

	// The initial desktop (PT) resolution that comes from the video mixer API
	SIZE m_desktopResolution;
	
	// The initial EGM resolution that comes from the video mixer API
	SIZE m_EGMResolution;
    volatile DWORD m_EGMResolutionCheckTick;
	
	// The VideoMixer resolution that comes from the video mixer API
	SIZE m_mixerResolution;
	
	Calibration m_calibration;
	
	// Diagnostics hiding of windows
	bool m_egmWindowHidForDiagnostics;
	bool m_serviceWindowMinimizedForDiagnostics;
    
	CLogs *m_mediaWindowLog; 
	DWORD m_mediaWindowCode;
	byte m_lastCardRead[CARD_ID_LEN];
	// Elapsed period for logging Media Window config info again.
	static const DWORD LOG_MEDIA_WINDOW_CONFIG_TICK_COUNT_PERIOD = 30000; // 30000 TickCounts = 30 seconds.

	typedef struct {
		// Explicit state values related to Media Window Player State automatic open/close functionality.
		int m_serviceWindowIsOpen;
		int m_insertedCard;
		int m_gameCommUp;
		int m_gameHasCredits;
		int m_hasFirstPlay;
		int m_gameInProgress;
		int m_pbtInProgress;

		// Timer expirations.
		int m_isExpiredAutoSwitchWaitTime;
		int m_isExpiredDelayOpenTimer;
		int m_isExpiredCreditMeterZeroDelayOpenTimer;
		int m_isExpiredCreditMeterZeroBeginSessionDelay;
		int m_isExpiredCreditMeterZeroEndSessionDelay;
		int m_isExpiredCardOutCreditMeterZeroTimer;
		int m_isExpiredGameNotInProgressBeginSessionDelay;
		int m_isExpiredGameNotInProgressEndSessionDelay;
	} MediaWindowStateType;

	MediaWindowStateType m_oldMediaWindowState;

	std::string m_mixerFirmwareVersion;
	std::string m_touchFirmwareVersion;

	void GetVideoInput(std::string& signalTypeString, std::string& signalTypeDetectedString, bool& IsInputAuto, uint32_t& inputModeNumber);
	void GetVideoInput(VideoModeSignalType& signalType, bool& IsInputAuto, uint32_t& inputModeNumber);
	void GetVideoInput(uint8_t& signalFormat, uint8_t& signalDetectedFormat, bool& IsInputAuto, uint32_t& inputModeNumber);
	void GetVideoOutput(bool& isOutputAuto, uint32_t& outputModeNumber);
	
	std::string GetSignalTypeString();
	std::string GetDetectedSignalTypeString();
	std::string VideoFormatTypeToString(uint8_t formatType);
	VideoModeSignalType VideoFormatTypeToEnum(uint8_t formatType);
	
	TouchScreenProtocols *GetTouchscreenInfo();
	std::vector<struct video_mode> GetVideoModeTable();
	void GetVideoOffsets(int32_t &hOffsetEGM, int32_t &vOffsetEGM, int32_t &hOffsetMonitor, int32_t &vOffsetMonitor);
	std::string GetPolarityString(unsigned char);
		
	bool InitializeVideoMixer();
    SIZE GetDisplayResolution(uint8_t displayType);
    std::string GetDisplayTypeString(uint8_t displayType);
    void EnableDisplay(bool enableEGM);
	
	bool SetInputConfig(VideoModeSignalType signalType, bool inputModeIsAuto, int selectedInputModeIndex);
	bool SetOutputConfig(bool outputModeIsAuto, int selectedOutputModeIndex);
	bool SetVideoOffsets(int egmHorizontalOffset, int egmVerticalOffset, int monitorHorizontalOffset, int monitorVerticalOffset);

	bool ShowMouseCursor(bool show);
	
	bool IsExpiredAutoSwitchWaitTime();
	bool IsExpiredDelayOpenTimer();
	bool IsExpiredCreditMeterZeroDelayOpenTimer();
	bool IsExpiredCreditMeterZeroBeginSessionDelay();
	bool IsExpiredCreditMeterZeroEndSessionDelay();
	bool IsExpiredCardOutCreditMeterZeroTimer();
	void UpdateDelayOpenTimer(WORD secondsToDelay);
	void ResetDelayOpenTimer(WORD secondsToDelay);
	void ResetAutoOpenTimer();

	void UpdateServiceWindowSize();
	void UpdateTaskbarSize();

	void GetEgmPositionAndSize(POINT &position, SIZE &size);
	MediaWindowPlayerState CardRemoved(bool wasPlayerCard,
		MediaWindowPlayerStatePending &playerStatePending, bool bannedPlayerIsDisplayed = false);
	MediaWindowPlayerState UpdatePlayerState(MediaWindowPlayerStateCreditMeterMode creditMeterMode,
		MediaWindowPlayerState targetPlayerState,
		MediaWindowPlayerStatePending &playerStatePending);
	MediaWindowPlayerState UpdatePlayerStatePendingOpen();
	MediaWindowPlayerState UpdatePlayerStatePendingClose();
	MediaWindowPlayerState UpdatePlayerStatePending(bool close);
	MediaWindowPlayerState UpdatePlayerStatePending(MediaWindowPlayerStatePending &playerStatePending);
	MediaWindowPlayerState UpdateGameNotInProgress(bool pbtInProgress, MediaWindowPlayerStatePending &playerStatePendingArg);
	void LogMediaWindowState(LPCTSTR prefixStrArg,
		const MediaWindowPlayerStatePending *playerStatePending,
		const MediaWindowPlayerStateCreditMeterMode *creditMeterMode,
		LPCTSTR extraInfo);

	std::string ResponseForVideoConfig();
	std::string ResponseForForMediaWindowStatusLog();

    void SaveMediaWindowCode(DWORD mediaWindowCode);
    DWORD GetMediaWindowCode();
    void UpdateMediaWindowCode();

};
