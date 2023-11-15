#include "stdafx.h"
#include "DisplayManager.h"
#include "UI/MediaWindowLog.h"
#include "Diagnostics.h"
#include "DirectoryManager.h"
#include "Hardware/WatchDog.h"
#include "Logging/LogString.h"
#include "Util/String.h"
#include "Platform.h"
#include "RapidJsonHelper.h"
#include "UI/ScreenDefs.h"
#include "TouchScreenProtocols.h"

#include "axhybrid.h"

using namespace std;

const DWORD WAIT_TIME = 1000;

CDisplayManager *CDisplayManager::m_instance = nullptr;
recursive_mutex CDisplayManager::m_instanceCS;

const WORD DEFAULT_SECONDS_TO_AUTO_OPEN_CLOSE = 600;
const WORD COMM_UP_SECONDS_TO_AUTO_OPEN = 15;
const WORD GAME_CHG_SECONDS_TO_AUTO_OPEN = 5;
const DWORD GAME_CHG_MILLISECONDS_TO_AUTO_OPEN = GAME_CHG_SECONDS_TO_AUTO_OPEN * MILLISECONDS_PER_SECOND;
const bool CENTER_EMPLOYEE_SERVICE_WINDOW = true;

const int32_t MixerAnimationTime = 500;

const uint32_t AutoConfigMode = 255;

const uint8_t EnableDisplayNone 	= 0x00;
const uint8_t EnableDisplayEGM 		= 0x01;
const uint8_t EnableDisplayDesktop 	= 0x02;
const uint8_t EnableDisplayAll	 	= 0x03;

// Choose arbitrary defaults that allow the arithmetic to work.
const POINT DEFAULT_POINT(0, 0);

const int DEFAULT_WIDTH = 1024;
const int DEFAULT_HEIGHT = 1024;
const SIZE DEFAULT_SIZE(DEFAULT_WIDTH, DEFAULT_HEIGHT);
const SIZE ZERO_SIZE(0, 0);

const RECT DEFAULT_RECT(DEFAULT_POINT, DEFAULT_SIZE);
const SIZE DEFAULT_LCD_SIZE(640, 240);
const SIZE DEFAULT_COMPACT_SIZE(480, 272);
const RECT DEFAULT_LCD_RECT_WITH_VEB(POINT(0, 0), DEFAULT_LCD_SIZE);
const RECT DEFAULT_LCD_RECT_WITHOUT_VEB(POINT(0, 0), DEFAULT_LCD_SIZE);

// These are the positions to put the service window screen for LCD.
const POINT LCD_FULL_ATTRACT_POINT(0, 0);
const SIZE LCD_FULL_ATTRACT_SIZE(640, 240);

const RECT LCD_FULL_ATTRACT_RECT(LCD_FULL_ATTRACT_POINT, LCD_FULL_ATTRACT_SIZE);

const string CmdStringHideMouse = "axpt-hidecursor";
const string CmdStringRotation = "axpt-rotation";

const string RotationString0 = "0";
const string RotationString90 = "90";
const string RotationString180 = "180";
const string RotationString270 = "270";


// Maintain the "native" Sentinel aspect ratio even if the video mixer and/or service window desktop display device
// are configured for different resolutions than the monitor.
const bool DISPLAYWINDOW_MAINTAIN_SENTINEL_ASPECT_RATIO = false;


const string VIDEO_CONFIGURATION_TOPIC("VideoConfig");
const string MEDIA_WINDOW_STATUS_LOG_TOPIC("MediaWindowStatusLog");

const char* VideoMixerModeStr[] =
{
	"VMM_TFT",
	"VMM_SERVICE_WINDOW",
	"VMM_BANNER",
	"VMM_EMPLOYEE_BANNER",
	"VMM_FULLEGM",
	"VMM_FULL_SERVICE_WINDOW",
	"VMM_EMPLOYEE_SERVICE_WINDOW",
	"VMM_SERVICE_WINDOW_CDM",
	"VMM_BANNER_CDM",
	"VMM_SHUTDOWN",
};
static_assert(NUMBER_OF_VM_MODES == _countof(VideoMixerModeStr),
	"VideoMixerModeStr has to be updated to match all the entries in VideoMixerMode");

CDisplayManager & CDisplayManager::Instance()
{
	lock_guard<recursive_mutex> lock(m_instanceCS);
	if (m_instance == nullptr)
	{
		m_instance = new CDisplayManager();
	}
	return *m_instance;
}

void CDisplayManager::CleanupInstance()
{
	lock_guard<recursive_mutex> lock(m_instanceCS);
	if (m_instance != nullptr)
	{
		delete m_instance;
		m_instance = nullptr;
	}
}


CDisplayManager & CDisplayManager::Instance(bool serviceWindowOpen, const CMediaWindowConfig &mediaWindowConfig,
	bool gameHasCredits, bool gameInProgress)
{
	CDisplayManager *dmInstance = &Instance();

	dmInstance->m_serviceWindowIsOpen = serviceWindowOpen;
	dmInstance->SetMediaWindowConfig(mediaWindowConfig);
	dmInstance->m_gameHasCredits = gameHasCredits;
	DWORD currentTickCount = CUtilities::GetTickCount();
	dmInstance->m_playerState.SetGameInProgress(gameInProgress, currentTickCount);

	return *dmInstance;
}

CDisplayManager::CDisplayManager() :
	CDataSubscriptionProvider(""),
	m_mediaWindowConfig(*CMediaWindowConfig::Instance()),
	m_playerState(m_mediaWindowConfig)
{
	AddTopic(VIDEO_CONFIGURATION_TOPIC);
	AddTopic(MEDIA_WINDOW_STATUS_LOG_TOPIC);

	m_isInitialized = false;
	m_tftMode = IsHardwareTFT();
	m_compactMode = IsHardwareCompact();
	m_lcdIsPresent = m_tftMode; // Useful default until actual status is determined.
	m_vmMode = VMM_TFT;
	m_vmModeLast = m_vmMode;
	
	m_revertVideoModeConfig = nullptr;
	m_externalVideoMixerConfig = nullptr;
	
	m_mediaWindowCode = (DWORD)-1; // Default until last Media Window log entry is available.
	m_mediaWindowLog = new CLogs(MEMORY_NVRAM, LOG_MEDIA_WINDOW, "");
	ZeroMemory(&m_oldMediaWindowState, sizeof(m_oldMediaWindowState));

	// Get Last Media Window code from log, if we can.
	CMediaWindowLog *log = (CMediaWindowLog *)m_mediaWindowLog->GetLastLog();
	if (log)
	{
		m_mediaWindowCode = log->GetMediaWindowCode();
		switch (m_mediaWindowCode)
		{
		case MWCODE_MWBOTTOM:
			m_serviceWindowIsOpen = true;
			break;
		case MWCODE_BANNER_MWLEFT:
			m_serviceWindowIsOpen = true;
			break;
		case MWCODE_BANNER_MWRIGHT:
			m_serviceWindowIsOpen = true;
			break;
		default:
			m_serviceWindowIsOpen = false;
		}
	}
	DisplayPlatform platformType = GetDisplayPlatform();
    if (platformType == LcdPlatform)
    {
        m_desktopResolution = DEFAULT_LCD_SIZE;
    }
    else if (platformType == VideoMixerPlatform)
    {
        m_desktopResolution = ZERO_SIZE;
    }
	else
	{
		m_desktopResolution = DEFAULT_COMPACT_SIZE;
	}
	m_EGMResolution = ZERO_SIZE;
	m_mixerResolution = ZERO_SIZE;

	m_insertedCard = CARD_INSERTED_NONE;

	UpdateServiceWindowSize();

	UpdateTaskbarSize();

	m_egmWindowHidForDiagnostics = false;
	m_serviceWindowMinimizedForDiagnostics = false;

	DWORD currentTickCount = CUtilities::GetTickCount();
	m_gameCreditsBecameZeroTickCount = 0; // Not used when m_gameHasCredits == true.
	m_gameHasCredits = true; // Default to prevent unwanted timer-based changes.
	m_playerState.SetGameInProgress(true, currentTickCount); // Default to prevent unwanted timer-based changes.
	m_gameCommUp = false; // Default to prevent unwanted timer-based changes.
	m_pbtInProgress = true; // Default to prevent unwanted timer-based changes.
	m_playerCardOutTickCount = 0;
    m_externalVideoConfigCheckTick = currentTickCount;
	m_serviceWindowIsOpen = false;

	ResetAutoOpenTimer();
	ResetDelayOpenTimer(0);
}

CDisplayManager::~CDisplayManager()
{
	if (m_mediaWindowLog != nullptr)
	{
		delete m_mediaWindowLog;
	}
}

// Initialize all the windows
void CDisplayManager::Start(bool serviceWindowOpen)
{
	lock_guard<recursive_mutex> lock(m_settingWindowsCS);
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == LcdPlatform)
	{
		LogString(DISPLAY_LOG, "DisplayManager has started for LCD mode");
	}
	else if (platformType == VideoMixerPlatform)
	{
		LogString(DISPLAY_LOG, "DisplayManager has started for Media Window mode");

		// TEMP_MEDIA_WINDOW_SUPPORT : commenting the video mixer call until proper hardware/OS support is available
		if (!CUtilities::DoesFileExist("/sdboot/MEDIA_WINDOW_SUPPORT"))
		{
			InitializeVideoMixer();

			// Set the positions of the windows in the video mixer 
			SetVideoMixerDisplay(true);
		}
	}
	else
	{
		LogString(DISPLAY_LOG, "DisplayManager has started for Compact mode");
	}

	m_serviceWindowIsOpen = serviceWindowOpen;

	m_isInitialized = true;
}

bool CDisplayManager::IsHardwareTFT()
{	
	bool bHardwareIsTFT = true;

	// TEMP_MEDIA_WINDOW_SUPPORT : should be true for MW display connected over USB
	const bool isMediaWindowDisplay = true;
	
	const string platformName = CPlatform::Instance().GetPlatformName();
	
	if (PLATFORM_PT_306 == platformName ||
		(PLATFORM_NEXUS == platformName && isMediaWindowDisplay))
	{
		bHardwareIsTFT = false;
	}

	return bHardwareIsTFT;
}

bool CDisplayManager::IsHardwareCompact()
{
	bool bHardwareIsCompact = false;
	const string CompactPlatform = "204";

	string platformName = CPlatform::Instance().GetPlatformName();

	if (platformName == CompactPlatform)
	{
		bHardwareIsCompact = true;
	}

	return bHardwareIsCompact;
}

PackageOrientation CDisplayManager::GetPackageOrientation(ContentWindowOrientation contentWindowOrientation) const
{
	PackageOrientation orientation(PackageSentinelIII);

	if (m_compactMode)
	{
		orientation = PackageNCompassCompact;
	}
	else if (m_tftMode)
	{
		orientation = PackageNCompassLCD;
	}
	else
	{
		switch (contentWindowOrientation)
		{
		case CWORIENTATION_LEFT:
		case CWORIENTATION_RIGHT:
			orientation = PackageNCompassSide;
			break;
		case CWORIENTATION_BOTTOM_OR_LCD:
		default:
			orientation = PackageNCompassBottom;
			break;
		}
	}

	return orientation;
}

bool CDisplayManager::IsTFTMode() const
{
	return m_tftMode;
}

bool CDisplayManager::IsCompactMode() const
{
	return m_compactMode;
}

DisplayPlatform CDisplayManager::GetDisplayPlatform()
{
	return GetDisplayPlatform(m_tftMode, m_compactMode);
}

DisplayPlatform CDisplayManager::GetDisplayPlatform(bool isTFTMode, bool isCompactMode)
{
	DisplayPlatform platformType = LcdPlatform;

	if (!isTFTMode)
	{
		platformType = VideoMixerPlatform;
	}
	else if (isCompactMode)
	{
		platformType = CompactPlatform;
	}

	return platformType;

}

string CDisplayManager::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId)
{
    UNREFERENCED_PARAMETER(&whereId); 

	string dataResponse;
	
	if (VIDEO_CONFIGURATION_TOPIC == topic)
	{
		dataResponse = ResponseForVideoConfig();
	}
	else if (MEDIA_WINDOW_STATUS_LOG_TOPIC == topic)
	{
		dataResponse = ResponseForForMediaWindowStatusLog();
	}

	return dataResponse;
}

void CDisplayManager::GetFirmwareVersions(std::string& mixerVersion, std::string& touchVersion)
{
	mixerVersion = m_mixerFirmwareVersion;
	touchVersion = m_touchFirmwareVersion;
}

// CDisplayManager::LogMediaWindowConfig
// Log information Media Window configuration information related to Media Window player states.
// --
// Convert millisecond timer values to seconds, rounding positive values towards zero,
// so comparisions with threshold values should be appropriate.
// Timeout thresholds should be multiples of one second.
void CDisplayManager::LogMediaWindowConfig()
{
	// Use "mwdCFG" (for Media Window DisplayManager.cpp) as a prefix to help identify output when using filters to process logs.
		LogString(_T("MWPLAYERCONFIG"),
		_T("mwdCFG Init=%d TFT=%d") // No comma.
		_T(" TIMEOUTS AutoOpenClose=%d CardOutCrdMtrToZero=%u CrdMtrZeroDly/BeginSess/EndSess=%u/%u/%u GameEndToBeginSess/EndSess=%u/%u") // No comma.
		_T(" MWCONFIG CardIn=%d CardOut=%d 1stPlay=%d AutoOpen/Close=%d/%d"),
		(int)m_isInitialized,
		(int)GetDisplayPlatform(),

		(int)m_mediaWindowConfig.GetAutoSwitchWaitTime(),
		(unsigned)(m_mediaWindowConfig.GetCardOutGameCreditsBecomeZeroTimeoutInMilliseconds() / MILLISECONDS_PER_SECOND),
		(unsigned)(GAME_CHG_MILLISECONDS_TO_AUTO_OPEN / MILLISECONDS_PER_SECOND),
		(unsigned)(m_mediaWindowConfig.GetGameCreditsBecameZeroBeginSessionDelayInMilliseconds() / MILLISECONDS_PER_SECOND),
		(unsigned)(m_mediaWindowConfig.GetGameCreditsBecameZeroEndSessionDelayInMilliseconds() / MILLISECONDS_PER_SECOND),
		(unsigned)(m_mediaWindowConfig.GetGameNotInProgressBeginSessionDelayInMilliseconds() / MILLISECONDS_PER_SECOND),
		(unsigned)(m_mediaWindowConfig.GetGameNotInProgressEndSessionDelayInMilliseconds() / MILLISECONDS_PER_SECOND),

		(int)m_mediaWindowConfig.GetCardInPlayerState(),
		(int)m_mediaWindowConfig.GetCardOutPlayerState(),
		(int)m_mediaWindowConfig.GetFirstPlayPlayerState(),
		(int)m_mediaWindowConfig.IsAutoOpen(),
		(int)m_mediaWindowConfig.IsAutoClose());
}

void CDisplayManager::ShowServiceWindow()
{
	// We will not show the service window for TFT mode until it can show all player screens.
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
		ResetAutoOpenTimer();
		m_serviceWindowIsOpen = true;
		SetVideoMixerDisplay();
	}
}

void CDisplayManager::HideServiceWindow()
{
	// This needs to be revisted when auto-hide stuff is put back in.
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
		ResetAutoOpenTimer();
	}

	m_serviceWindowIsOpen = false;
	SetVideoMixerDisplay();
}

MediaWindowPlayerState CDisplayManager::PlayerCardInserted(bool pbtInProgress, MediaWindowPlayerStatePending &playerStatePending)
{
	MediaWindowPlayerState returnValue = MAINTAIN_PLAYER_STATE;
	playerStatePending = MAINTAIN_PLAYER_STATE_PENDING; // Clear any prior pending change.

	m_pbtInProgress = pbtInProgress;

	lock_guard<recursive_mutex> lock(m_settingWindowsCS);

	m_insertedCard = CARD_INSERTED_PLAYER;
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform && m_gameCommUp)
	{
		LogString(DISPLAY_LOG, "DisplayManager::PlayerCardInserted. Not tftMode and gameCommUp");
		if (!m_pbtInProgress && !m_playerState.GetGameInProgress())
		{
			LogString(DISPLAY_LOG, "DisplayManager::PlayerCardInserted. Not pbtInProgress and Not GameInProgress");
			DWORD currentTickCount = CUtilities::GetTickCount();
			if (!m_playerState.GetGameInProgress() && m_playerState.IsExpiredGameNotInProgressBeginSessionDelay(currentTickCount) &&
				!m_gameHasCredits && IsExpiredCreditMeterZeroBeginSessionDelay())
			{
				LogString(DISPLAY_LOG, "DisplayManager::PlayerCardInserted. Not pbtInProgress and Not GameInProgress");
				if (IsExpiredDelayOpenTimer())
				{
					m_playerState.SetHasFirstPlay(false);
					MediaWindowPlayerStateCreditMeterMode creditMeterMode = PLAYER_STATE_CREDIT_METER_NORMAL;
					returnValue = UpdatePlayerState(creditMeterMode, m_mediaWindowConfig.GetCardInPlayerState(), playerStatePending);
				}
			}
		}
	}
	
	LogString(DISPLAY_LOG, "DisplayManager::PlayerCardInserted is returning a player state of %d", returnValue);

	return returnValue;
}


bool CDisplayManager::EmployeeCardInserted(bool &closeServiceWindow, bool &startCalibration)
{
	closeServiceWindow = false;
	startCalibration = false;

    m_insertedCard = CARD_INSERTED_EMPLOYEE;

	if (m_calibration.CheckCalibCardCount())
	{
		startCalibration = true;
	}
	else
	{
		lock_guard<recursive_mutex> lock(m_settingWindowsCS);
	
		if (m_serviceWindowIsOpen)
		{
			m_serviceWindowIsOpen = false;
			closeServiceWindow = true;
		}
		SetVideoMixerDisplay();

        ShowMouseCursor(true);
	}
	
	return (closeServiceWindow || startCalibration);
}

MediaWindowPlayerState CDisplayManager::PlayerCardRemoved(bool pbtInProgress,
	MediaWindowPlayerStatePending &playerStatePending, bool bannedPlayerIsDisplayed)
{
	MediaWindowPlayerState returnValue;
	
	m_pbtInProgress = pbtInProgress;

	m_playerCardOutTickCount = CUtilities::GetTickCount();

	returnValue = CardRemoved(true, playerStatePending, bannedPlayerIsDisplayed);
	
	LogString(DISPLAY_LOG, "DisplayManager::PlayerCardRemoved is returning a player state of %d", returnValue);
		
	return returnValue;
}

void CDisplayManager::EmployeeCardRemoved(bool &restartUI)
{ 
	restartUI = false;
	MediaWindowPlayerStatePending playerStatePending = MAINTAIN_PLAYER_STATE_PENDING; // Discard result.
	CardRemoved(false, playerStatePending);

    ShowMouseCursor(false);
	
	if (m_calibration.EmployeeCardRemoved() == Calibration::WaitUIRestart)
	{
		restartUI = true;
	}
	
	if (m_calibration.InCalibrationProcess())
	{
		LogString(DISPLAY_LOG, "Calibration Aborted by card removal. Setting display to full screen EGM");
		MakeDisplayFullEGM();
	}
}

void CDisplayManager::SetMediaWindowConfig(const CMediaWindowConfig &mediaWindowConfig)
{
	lock_guard<recursive_mutex> lock(m_settingWindowsCS);

	m_mediaWindowConfig.SetFromConfigAlways(mediaWindowConfig);
	m_playerState.SetMediaWindowConfig(m_mediaWindowConfig);
	UpdateServiceWindowSize();
	UpdateTaskbarSize();

	LogMediaWindowConfig(); // Log new values.
}

ContentWindowOrientation CDisplayManager::GetServiceWindowOrientation()
{
	return m_mediaWindowConfig.GetContentWindowOrientation();
}

DesktopOrientationType CDisplayManager::GetDesktopRotation()
{
    string currentRotationString = CUtilities::GetSystemResultString(CmdStringRotation);
    CUtilities::Trim(currentRotationString);

    LogString(DISPLAY_LOG, "Current desktop rotation query result is %s", currentRotationString.c_str());

    m_desktopOrientation = ConvertDesktopOrientationFromString(currentRotationString);

    LogString(DISPLAY_LOG, "Current desktop orientation is %d", m_desktopOrientation);

    return m_desktopOrientation;
}

bool CDisplayManager::SetDesktopRotation(DesktopOrientationType desktopOrientation, bool makeFullScreenEGM)
{
    bool displayWasRotated = false;

    if ( GetDesktopRotation() != desktopOrientation )
    {
        if (makeFullScreenEGM) 
        {
            LogString(DISPLAY_LOG, "Setting display to full screen EGM before rotating display");
            MakeDisplayFullEGM();
        }

        string setRotationString = CmdStringRotation + " -s " + ConvertDesktopOrientationToString(desktopOrientation);

        LogString(DISPLAY_LOG, "Making system call to rotate display: %s", setRotationString.c_str());

        if (system(setRotationString.c_str()) == 0)
        {
            displayWasRotated = true;
            m_desktopOrientation = desktopOrientation;
            LogString(DISPLAY_LOG, "Display rotation successful %s", setRotationString.c_str());
        }
        else
        {
            LogString(ERROR_LOG, "Error rotating display with command %s", setRotationString.c_str());
        }
    }

    return displayWasRotated;
}

DesktopOrientationType CDisplayManager::GetTouchScreenRotation()
{
    DesktopOrientationType touchscreenRotation = DesktopOrientationDefault;

	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
        
        uint8_t touchRotationCCW = TS_ROTATION_0;

        int32_t vmResult = AXVM_GetTSRotation(&touchRotationCCW);

        if (vmResult == VM_Success) 
        {
           // Translate from the hybrid library orientation
           // NOTE: The touch rotation from the hybrid library is counter-clockwise
           switch (touchRotationCCW) 
           {
           case TS_ROTATION_90:
             touchscreenRotation = DesktopOrientation270;
             break;
           case TS_ROTATION_180:
             touchscreenRotation = DesktopOrientation180;
             break;
           case TS_ROTATION_270:
             touchscreenRotation = DesktopOrientation90;
             break;
           case TS_ROTATION_0:
           default:
             touchscreenRotation = DesktopOrientationDefault;
             break;
           }

           LogString(DISPLAY_LOG, "Current touchscreen rotation is %s", ConvertDesktopOrientationToString(touchscreenRotation).c_str());
        }
        else
        {
            LogString(ERROR_LOG, _T("AXVM_GetTSRotation failure %d"), vmResult);
        }
    }
    
    return touchscreenRotation;
}

bool CDisplayManager::SetTouchScreenRotation(DesktopOrientationType touchscreenRotation)
{
    bool touchWasRotated = false;

	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform && GetTouchScreenRotation() != touchscreenRotation)
	{
        // Translate to the hybrid library orientation
        // NOTE: The touch rotation in the hybrid library is counter-clockwise
        uint8_t touchRotationCCW = TS_ROTATION_0;
        switch (touchscreenRotation)
        {
        case DesktopOrientation90:
          touchRotationCCW = TS_ROTATION_270;
          break;
        case DesktopOrientation180:
          touchRotationCCW = TS_ROTATION_180;
          break;
        case DesktopOrientation270:
          touchRotationCCW = TS_ROTATION_90;
          break;
        case DesktopOrientationDefault:
        default:
          touchRotationCCW = TS_ROTATION_0;
          break;
        }
        int32_t vmResult = AXVM_SetTSRotation(touchRotationCCW);

        if (vmResult == VM_Success)
        {
			LogString(DISPLAY_LOG, "Touchscreen rotation is set to %s", ConvertDesktopOrientationToString(touchscreenRotation).c_str());
            touchWasRotated = true;
        }
        else
        {
            LogString(ERROR_LOG, _T("AXVM_SetTSRotation failure %d"), vmResult);
        }
    }
    
    return touchWasRotated;
}

void CDisplayManager::SetVideoMixerDisplay(bool forceUpdate)
{
	VideoMixerMode oldVmMode = m_vmMode;

	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType != VideoMixerPlatform)
	{
		m_vmMode = VMM_TFT;
	}
	else if (CARD_INSERTED_EMPLOYEE == m_insertedCard)
	{		
		m_vmMode = VMM_EMPLOYEE_BANNER;
		m_serviceWindowIsOpen = false;
	}
	else
	{
		m_vmMode = m_serviceWindowIsOpen ? VMM_SERVICE_WINDOW : VMM_BANNER;
	}

	if (forceUpdate || oldVmMode != m_vmMode)
	{
		LogString(DISPLAY_LOG, "SetVideoMixerDisplay: Setting the video mixer mode to %s", VideoMixerModeStr[m_vmMode]);
		UpdateVideoMixerDisplay();
	}
}

void CDisplayManager::UpdateVideoMixerDisplay()
{
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
		int vmResult = VM_Success;
		POINT egmPosition;
		SIZE egmSize;

		GetEgmPositionAndSize(egmPosition, egmSize);
		
		if (egmSize.cx > 0 && egmSize.cy > 0)
		{
			LogString(DISPLAY_LOG, "Setting EGM window to position %ld,%ld. Size: %ld x %ld", egmPosition.x, egmPosition.y, egmSize.cx, egmSize.cy);

			vmResult = AXVM_BeginAnimation();
			if (vmResult != VM_Success)
			{
			LogString(ERROR_LOG, _T("AXVM_BeginAnimation failure %d"), vmResult);
			}
			
			vmResult = AXVM_SetPosition(DISPLAY_EGM, egmPosition.x, egmPosition.y);
			if (vmResult != VM_Success)
			{
				LogString(ERROR_LOG, _T("AXVM_SetPosition failure %d"), vmResult);
			}
			
			vmResult = AXVM_SetSize(DISPLAY_EGM, egmSize.cx, egmSize.cy);
			if (vmResult != VM_Success)
			{
				LogString(ERROR_LOG, _T("AXVM_SetSize failure %d"), vmResult);
			}
			
			vmResult = AXVM_RunAnimation(UNIT_TIME_MS, MixerAnimationTime);
			if (vmResult != VM_Success)
			{
				LogString(ERROR_LOG, _T("AXVM_RunAnimation failure %d"), vmResult);
			}
		}
		else
		{
			LogString(DISPLAY_LOG, "No EGM video input detected. Enabling only the desktop.");
			
			// We don't seem to have an EGM display. So only enable the desktop so we can see it.
			EnableDisplay(false);
		}

        // Set the media window logging state
        UpdateMediaWindowCode();
	}
}

void CDisplayManager::GameCreditsBecameZero(MediaWindowPlayerStatePending &playerStatePending)
{
	LogString(DISPLAY_LOG, "Display Manager credits became ZERO");
	
	ResetAutoOpenTimer();
	m_gameCreditsBecameZeroTickCount = CUtilities::GetTickCount();
	m_gameHasCredits = false;

	switch (playerStatePending)
	{
	case OPEN_PLAYER_STATE_PENDING_CARD_OUT_TIMEOUT:
	case CLOSE_PLAYER_STATE_PENDING_CARD_OUT_TIMEOUT:
		if (!IsExpiredCardOutCreditMeterZeroTimer())
		{
			// Retain current pending state and let AutoOpenCloseServiceWindowCheck() handle the pending change.
			break;
		}
		// Else fall thru.

	default:
		// Set a new pending state if condition(s) are met.
		playerStatePending = MAINTAIN_PLAYER_STATE_PENDING;  // Clear any prior pending change.
		if (CARD_INSERTED_NONE == m_insertedCard)
		{
			// m_insertedCard is already checked, other requirements are checked by AutoOpenCloseServiceWindowCheck().
			switch (m_mediaWindowConfig.GetCardOutPlayerState())
			{
			case OPEN_PLAYER_STATE:
				playerStatePending = OPEN_PLAYER_STATE_PENDING_CREDIT_METER_ZERO_DELAY;
				break;

			case CLOSE_PLAYER_STATE:
				playerStatePending = CLOSE_PLAYER_STATE_PENDING_CREDIT_METER_ZERO_DELAY;
				break;

			case MAINTAIN_PLAYER_STATE:
			default:
				break;
			}
		}
	}
}

MediaWindowPlayerState CDisplayManager::GameCreditsBecameNonZero(bool pbtInProgress,
	MediaWindowPlayerStatePending &playerStatePending)
{
	LogString(DISPLAY_LOG, "Display Manager credits became NON zero");
	
	MediaWindowPlayerState returnValue = MAINTAIN_PLAYER_STATE;
	playerStatePending = MAINTAIN_PLAYER_STATE_PENDING;

	m_pbtInProgress = pbtInProgress;

	lock_guard<recursive_mutex> lock(m_settingWindowsCS);

	if (!m_gameHasCredits)
	{
		m_gameHasCredits = true;
		DisplayPlatform platformType = GetDisplayPlatform();
		if (platformType == VideoMixerPlatform && m_gameCommUp && IsExpiredDelayOpenTimer())
		{
			if (CARD_INSERTED_NONE == m_insertedCard)
			{
				DWORD currentTickCount = CUtilities::GetTickCount();
				if (!m_playerState.GetGameInProgress() && m_playerState.IsExpiredGameNotInProgressBeginSessionDelay(currentTickCount) &&
					IsExpiredCreditMeterZeroBeginSessionDelay())
				{
					m_playerState.SetHasFirstPlay(false);
					MediaWindowPlayerStateCreditMeterMode creditMeterMode = PLAYER_STATE_CREDIT_METER_IGNORE;
					returnValue = UpdatePlayerState(creditMeterMode, m_mediaWindowConfig.GetCardInPlayerState(), playerStatePending);
				}
			}
		}
	}

	return returnValue;
}

// Called when an EGM game is started (or is in progress).
void CDisplayManager::GameInProgress()
{
	DWORD currentTickCount = CUtilities::GetTickCount();
	m_playerState.SetGameInProgress(true, currentTickCount);
}

MediaWindowPlayerState CDisplayManager::GameNotInProgress(bool pbtInProgress,
	MediaWindowPlayerStatePending &playerStatePendingArg)
{
	MediaWindowPlayerState returnValue = MAINTAIN_PLAYER_STATE;

	lock_guard<recursive_mutex> lock(m_settingWindowsCS);
	{
		returnValue = UpdateGameNotInProgress(pbtInProgress, playerStatePendingArg);
	}

	return returnValue;
}

// Called when communications to the EGM are up.
void CDisplayManager::GameCommUp()
{
	UpdateDelayOpenTimer(COMM_UP_SECONDS_TO_AUTO_OPEN);
	m_gameCommUp = true;
}

// Called when communications to the EGM are down.
void CDisplayManager::GameCommDown()
{
	m_gameCommUp = false;
}

MediaWindowPlayerState CDisplayManager::AutoOpenCloseServiceWindowCheck(bool pbtInProgress,
	MediaWindowPlayerStatePending &playerStatePending)
{
	MediaWindowPlayerState returnValue = MAINTAIN_PLAYER_STATE;
	m_pbtInProgress = pbtInProgress;

	lock_guard<recursive_mutex> lock(m_settingWindowsCS);

	if (playerStatePending != MAINTAIN_PLAYER_STATE_PENDING)
	{
		// Try to resolve pending media window player state.
		returnValue = UpdatePlayerStatePending(playerStatePending);
	}

	if (MAINTAIN_PLAYER_STATE == returnValue && MAINTAIN_PLAYER_STATE_PENDING == playerStatePending)
	{
		// Check to see if we need to auto-open/close the service window
		if (CARD_INSERTED_NONE == m_insertedCard)
		{
			if (IsExpiredAutoSwitchWaitTime())
			{
				MediaWindowPlayerStateCreditMeterMode creditMeterMode = PLAYER_STATE_CREDIT_METER_NORMAL;
				MediaWindowPlayerStatePending tempPlayerStatePending; // Discard result.
				if (m_serviceWindowIsOpen && m_mediaWindowConfig.IsAutoClose())
				{
					returnValue = UpdatePlayerState(creditMeterMode, CLOSE_PLAYER_STATE, tempPlayerStatePending);
				}
				else if ((!m_serviceWindowIsOpen) && m_mediaWindowConfig.IsAutoOpen())
				{
					returnValue = UpdatePlayerState(creditMeterMode, OPEN_PLAYER_STATE, tempPlayerStatePending);
				}
			}
		}
	}
	
	return returnValue;
}

void CDisplayManager::SetLastCardRead(const byte* cardID)
{
	memcpy(m_lastCardRead, cardID, CARD_ID_LEN);
}

void CDisplayManager::ClearLastCardRead( void )
{
	memset( m_lastCardRead, 0, CARD_ID_LEN );
}

byte * CDisplayManager::GetLastCardRead()
{
    return m_lastCardRead;
}


void CDisplayManager::MakeDisplayFullEGM()
{
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
		LogString(DISPLAY_LOG, "Making display full EGM");
		
		lock_guard<recursive_mutex> lock(m_settingWindowsCS);
		if ( VMM_FULLEGM != m_vmMode )
		{
			m_vmMode = VMM_FULLEGM;
			
			UpdateVideoMixerDisplay();
		}
	}
}

void CDisplayManager::MakeDisplayFullServiceWindow()
{
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
		LogString(DISPLAY_LOG, "Making display full Service Window");
		
		lock_guard<recursive_mutex> lock(m_settingWindowsCS);
		if ( VMM_FULL_SERVICE_WINDOW != m_vmMode )
		{
			m_vmMode = VMM_FULL_SERVICE_WINDOW;
			
			UpdateVideoMixerDisplay();
		}
	}
}

VideoConfigChangeStatus CDisplayManager::UIRestartComplete()
{
	LogString(DISPLAY_LOG, "Display Manager received UI Restart Complete. Re-initializing device");
	
	m_calibration.UIRestarted();
	
	InitializeVideoMixer();
	
	SetVideoMixerDisplay(true);

	lock_guard<recursive_mutex> lock(m_revertVideoModeConfigMutex);
	
	VideoConfigChangeStatus videoConfigStatusChange = VIDEO_CHANGE_NONE;
	if (m_revertVideoModeConfig != nullptr)
	{
		videoConfigStatusChange = m_revertVideoModeConfig->UIRestarted();
	}
	
	return videoConfigStatusChange;
}

void CDisplayManager::PrepareDisplayForShutdown()
{
	DisplayPlatform platformType = GetDisplayPlatform();
	if (m_isInitialized && platformType == VideoMixerPlatform)
	{
		lock_guard<recursive_mutex> lock(m_settingWindowsCS);
		if ( VMM_SHUTDOWN != m_vmMode )
		{
			m_vmMode = VMM_SHUTDOWN;
			UpdateVideoMixerDisplay();
		}
	}
}

VideoConfigChangeStatus CDisplayManager::SetVideoModeConfig(CVideoModeConfig *videoModeConfig)
{
	VideoConfigChangeStatus changeStatus = VIDEO_CHANGE_NONE;

    LogString(DISPLAY_LOG, "CDisplayManager::Setting Video Config");
	
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform && videoModeConfig != nullptr)
	{
		if (videoModeConfig->IsInputConfig() || videoModeConfig->IsOutputConfig())
		{
			// Set full screen egm
			MakeDisplayFullEGM();
			
			// Get the current settings .
            LogString(DISPLAY_LOG, "CDisplayManager::Getting the current video config in case we have to revert back");
			VideoModeSignalType currentSignalType = VIDEO_MODE_SIGNAL_AUTO; 
			bool currentInputModeIsAuto = true;
			uint32_t currentSelectedInputModeIndex = 0;
			GetVideoInput(currentSignalType, currentInputModeIsAuto, currentSelectedInputModeIndex);
			bool currentOutputModeIsAuto = true;  
			uint32_t currentSelectedOutputModeIndex = 0;
			GetVideoOutput(currentOutputModeIsAuto, currentSelectedOutputModeIndex);
			
			// Set the input or output config to the VM
			bool setSuccess = false;
			if (videoModeConfig->IsInputConfig())
			{
				setSuccess = SetInputConfig(videoModeConfig->GetInputSignalType(), videoModeConfig->IsAutoInputMode(), videoModeConfig->GetInputMode());
			}
			else
			{
				setSuccess = SetOutputConfig(videoModeConfig->IsAutoOutputMode(), videoModeConfig->GetOutputMode());
			}
			
			// If the VM set was a success...
			if (setSuccess)
			{
				lock_guard<recursive_mutex> lock(m_revertVideoModeConfigMutex);

                LogString(DISPLAY_LOG, "Storing the revert video config. Input %s %s Mode #: %d. Output %s Mode #: %d", VideoFormatTypeToString(currentSignalType).c_str(), currentInputModeIsAuto ? "Auto" : "Manual", currentSelectedInputModeIndex, currentOutputModeIsAuto ? "Auto" : "Manual", currentSelectedOutputModeIndex);

				// Set the revert config object in case we have to revert back
				m_revertVideoModeConfig.reset(new CVideoModeConfig(currentSignalType, currentInputModeIsAuto, currentSelectedInputModeIndex, currentOutputModeIsAuto, currentSelectedOutputModeIndex));

				// Return status that we are to restart the UI
				changeStatus = VIDEO_CHANGE_RESTARTING_UI;
			}
            else
            {
                LogString(DISPLAY_LOG, "Could not save the revert video config because the Set function did not succeed.");
            }
		}
		
		else if (videoModeConfig->IsOffsetConfig())
		{
			// The offsets need to be set in full screen EGM mode.
			MakeDisplayFullEGM();
			
			// Now adjust the offsets
			int egmHorizontalOffset;
			int egmVerticalOffset;
			int monitorHorizontalOffset; 
			int monitorVerticalOffset;
			videoModeConfig->GetOffsets(egmHorizontalOffset, egmVerticalOffset, monitorHorizontalOffset, monitorVerticalOffset);
			SetVideoOffsets(egmHorizontalOffset, egmVerticalOffset, monitorHorizontalOffset, monitorVerticalOffset);
			
			// Now go back to normal display.
			SetVideoMixerDisplay();
		}
	}
	
	return changeStatus;
}

void CDisplayManager::SetTouchProtocol(string protocolName)
{
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
        TouchScreenProtocols *touchProtocols = GetTouchscreenInfo();

        if (nullptr != touchProtocols) 
        {
            LogString(DISPLAY_LOG, "Setting new touch screen protocol %s.", protocolName.c_str() );

            TouchScreenProtocol *currentProtocol = touchProtocols->GetCurrentProtocol();
            if (nullptr == currentProtocol || currentProtocol->GetName() != protocolName) 
            {
                TouchScreenProtocol *newTouchProtocol = touchProtocols->GetProtocol(protocolName);

                if (nullptr != newTouchProtocol) 
                {
                    int32_t vmResult = AXVM_SetTSVendor(newTouchProtocol->GetVendorID());
                    if (vmResult != VM_Success)
                    {
                        LogString(ERROR_LOG, "AXVM_SetTSVendor failure %d", vmResult);
                    }

                    vmResult = AXVM_SetTSBaudRate(newTouchProtocol->GetBaud());
                    if (vmResult != VM_Success)
                    {
                        LogString(ERROR_LOG, "AXVM_SetTSBaud failure %d", vmResult);
                    }
                }
                else
                {
                    LogString(ERROR_LOG, "Could not find %s in list of available touch screen protocols", protocolName.c_str());
                }
            }
            else
            {
                if (nullptr == currentProtocol) 
				{
					LogString(DISPLAY_LOG, "Touch screen protocol %s is not the name of a valid protocol.", protocolName.c_str() );
					LogString(ERROR_LOG, "Touch screen protocol %s is not the name of a valid protocol.", protocolName.c_str() );
                }
				else
				{
					LogString(DISPLAY_LOG, "Touch screen protocol is already set to %s.", protocolName.c_str());
				}
            }

            delete touchProtocols;
        }
    }
}

void CDisplayManager::CheckForEGMDisplay()
{
    const DWORD EGMResolutionCheckInterval = 10000;

	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform && m_isInitialized &&
		(m_EGMResolution.cx == 0 || m_EGMResolution.cy == 0) &&
		(CUtilities::GetTickCount() - m_EGMResolutionCheckTick) >= EGMResolutionCheckInterval)
	{
        LogString(DISPLAY_LOG, "Checking for an EGM display" );
        SIZE egmResolution = GetDisplayResolution(DISPLAY_EGM);

        if (egmResolution.cx > 0 && egmResolution.cy > 0) 
        {
            m_EGMResolution = egmResolution;

            LogString(DISPLAY_LOG, "We now have an EGM display. Updating all the displays" );
			EnableDisplay(true);
            UpdateVideoMixerDisplay();
        }
    }
}

VideoConfigChangeStatus CDisplayManager::RevertVideoMode()
{
	VideoConfigChangeStatus changeStatus = VIDEO_CHANGE_NONE;

	lock_guard<recursive_mutex> lock(m_revertVideoModeConfigMutex);
	
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform && m_revertVideoModeConfig != nullptr)
	{
		LogString(DISPLAY_LOG, "Reverting video modes");
		bool inputSetSuccess = SetInputConfig(m_revertVideoModeConfig->GetInputSignalType(), m_revertVideoModeConfig->IsAutoInputMode(), m_revertVideoModeConfig->GetInputMode());
		bool outputSetSuccess = SetOutputConfig(m_revertVideoModeConfig->IsAutoOutputMode(), m_revertVideoModeConfig->GetOutputMode());

		// If either the input or the output mode was set, we need to restart the UI
		if (inputSetSuccess || outputSetSuccess)
		{
			changeStatus = VIDEO_CHANGE_RESTARTING_UI;
		}
		
		m_revertVideoModeConfig.reset();
	}
	
	return changeStatus;
}

VideoConfigChangeStatus CDisplayManager::CheckForVideoModeRevert()
{
	VideoConfigChangeStatus changeStatus = VIDEO_CHANGE_NONE;

	lock_guard<recursive_mutex> lock(m_revertVideoModeConfigMutex);
	
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform && m_revertVideoModeConfig != nullptr && m_revertVideoModeConfig->HasRevertTimeExpired())
	{
		LogString(DISPLAY_LOG, "Reverting video mode timer has expired");
		
		changeStatus = RevertVideoMode();
	}
	
	return changeStatus;
}

VideoConfigChangeStatus CDisplayManager::CheckForExternalVideoModeConfig()
{
	VideoConfigChangeStatus changeStatus = VIDEO_CHANGE_NONE;

    const DWORD ExternalVideoConfigCheckInterval = 10000;

	if ( (CUtilities::GetTickCount() - m_externalVideoConfigCheckTick) >= ExternalVideoConfigCheckInterval )
    {
        bool configFilePresent = VideoMixerConfigFile::CheckExists();
        
        if (m_externalVideoMixerConfig != nullptr && !configFilePresent)
        {
            LogString(DISPLAY_LOG, "External video mixer config file removed");
            m_externalVideoMixerConfig.reset();
        }
        
        else if (m_externalVideoMixerConfig == nullptr && configFilePresent)
        {
            LogString(DISPLAY_LOG, "Loading video mixer config from external file");
            m_externalVideoMixerConfig.reset(new VideoMixerConfigFile(CUtilities::GetTickCount()));
            
            if (m_externalVideoMixerConfig->Start())
            {
                // Set the touch screen protocol from the external config file
                string touchProtocolName;
                if (m_externalVideoMixerConfig->GetTouchProtocol(touchProtocolName)) 
                {
                    SetTouchProtocol(touchProtocolName);
                }
				else
				{
					LogString(DISPLAY_LOG, "No touch protocol specified in external mixer config file");
				}
                
                // Get the input configuration from the external config file
                string inputSignalType;
                bool isInputAuto;
                int inputMode;
                bool hasInputConfig = m_externalVideoMixerConfig->GetVideoInput(inputSignalType, isInputAuto, inputMode);

                // Get the output configuration from the external config file
                bool isOutputAuto;
                int outputMode;
                bool hasOutputConfig = m_externalVideoMixerConfig->GetVideoOutput(isOutputAuto, outputMode);

                if (hasInputConfig || hasOutputConfig) 
                {
                    // Make display full screen EGM
                    MakeDisplayFullEGM();
                    
                    if (hasInputConfig) 
                    {
                        hasInputConfig = SetInputConfig(CVideoModeConfig::ConvertStringToSignalType(inputSignalType), isInputAuto, inputMode);
                    }
					else
					{
						LogString(DISPLAY_LOG, "No video input specified in external mixer config file");
					}

                    if (hasOutputConfig) 
                    {
                        hasOutputConfig = SetOutputConfig(isOutputAuto, outputMode);
                    }
					else
					{
						LogString(DISPLAY_LOG, "No video output specified in external mixer config file");
					}

                    // If either the input or the output mode was set, we need to restart the UI
                    if (hasInputConfig || hasOutputConfig)
                    {
                        changeStatus = VIDEO_CHANGE_RESTARTING_UI;
                    }
                }
				else
				{
					LogString(DISPLAY_LOG, "No video input or output specified in external mixer config file");
				}
                
                // Remove the revert config in case there is one.
				lock_guard<recursive_mutex> lock(m_revertVideoModeConfigMutex);
                m_revertVideoModeConfig.reset();
            }
        }

        m_externalVideoConfigCheckTick = CUtilities::GetTickCount();
    }
	
	return changeStatus;
}

bool CDisplayManager::CheckForExternalVideoMixerDiagnosticsFile()
{
    return m_videoMixerDiagnostics.CheckForExternalVideoMixerDiagnosticsFile();
}

bool CDisplayManager::OKToRestartUI()
{
	lock_guard<recursive_mutex> lock(m_revertVideoModeConfigMutex);
	return (!m_calibration.InCalibrationProcess() && m_revertVideoModeConfig == nullptr);
}

Calibration::CalibrationState CDisplayManager::StartCalibration()
{
	Calibration::CalibrationState calibrationState = m_calibration.StartCalibration();
	
	if (calibrationState == Calibration::WaitBrowserTermination)
	{
		// Set to a full screen EGM display until the calibration program is started
		LogString(DISPLAY_LOG, "Calibration started. Setting display to full screen EGM");
		MakeDisplayFullEGM();
	}
	
	return calibrationState;
}

Calibration::CalibrationState CDisplayManager::CheckForCalibrationComplete()
{
	Calibration::CalibrationState calibrationState = m_calibration.CheckCalibrationComplete();

	if (calibrationState == Calibration::CalibrationComplete)
	{
		// Set to a full screen EGM display until the UI is restarted
		LogString(DISPLAY_LOG, "Calibration complete. Setting display to full screen EGM");
		MakeDisplayFullEGM();
	}

	return calibrationState;
}

void CDisplayManager::BrowserTerminated()
{
	if (m_calibration.BrowserTerminated() == Calibration::InCalibration)
	{
		LogString(DISPLAY_LOG, "Setting display to full service window to show calibration");
		MakeDisplayFullServiceWindow();
	}
}


void CDisplayManager::ClearMixerHardware()
{
	int32_t vmResult = AXVM_FactoryReset();

	if (vmResult != VM_Success)
	{
		LogString(ERROR_LOG, "AXVM_FactoryReset failure %d", vmResult);
	}
}

SIZE CDisplayManager::GetDesktopResolution()
{
    return m_desktopResolution;
}

bool CDisplayManager::CalibrationError() const
{
	return m_calibration.CalibrationError();
}

string CDisplayManager::ResponseForVideoConfig()
{
    LogString(DISPLAY_LOG, "Getting subscription data for video config");

	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	writer.StartObject();

	string signalTypeString;
	string signalTypeDetectedString;
	bool isInputAuto;
	uint32_t inputModeNumber;
	GetVideoInput(signalTypeString, signalTypeDetectedString, isInputAuto, inputModeNumber);
	writer.Key("InputType");					writer.String(signalTypeString);
	writer.Key("InputTypeDetected");			writer.String(signalTypeDetectedString);
	writer.Key("InputResolutionHorizontal");	writer.Int(m_EGMResolution.cx);
	writer.Key("InputResolutionVertical");		writer.Int(m_EGMResolution.cy);
	writer.Key("InputModeAuto");				writer.Bool(isInputAuto);
	writer.Key("InputModeNumber");				writer.Int(inputModeNumber);

	bool isOutputAuto;
	uint32_t outputModeNumber;
	GetVideoOutput(isOutputAuto, outputModeNumber);
	writer.Key("OutputResolutionHorizontal");	writer.Int(m_mixerResolution.cx);
	writer.Key("OutputResolutionVertical");		writer.Int(m_mixerResolution.cy);
	writer.Key("OutputModeAuto");				writer.Bool(isOutputAuto);
	writer.Key("OutputModeNumber");				writer.Int(outputModeNumber);

    TouchScreenProtocols *touchProtocols = GetTouchscreenInfo();
    if (nullptr != touchProtocols) 
    {
        writer.Key("USBTouchscreenDetected");   writer.Bool(touchProtocols->USBDetected() ? true : false);

        string touchProtocolName;
        string vendorIDString;
        string baudRateString;
        touchProtocols->GetCurrentInfo(touchProtocolName, vendorIDString, baudRateString);
        writer.Key("TouchscreenProtocolName");	writer.String(touchProtocolName);
        writer.Key("TouchscreenVendorID");      writer.String(vendorIDString);
        writer.Key("TouchscreenBaud");          writer.String(baudRateString);
        writer.Key("TouchscreenProtocols");
        writer.StartArray();
        std::vector<TouchScreenProtocol *>protocolList = touchProtocols->GetAllProtocols();
        for (vector<TouchScreenProtocol *>::iterator iter = protocolList.begin(); iter != protocolList.end(); ++iter)
        {
            writer.StartObject();
            if ((*iter) != nullptr) 
            {
                writer.Key("Name");		        writer.String((*iter)->GetName());
                writer.Key("VendorID");		    writer.String((*iter)->GetVendorIDString());
                writer.Key("Baud");		        writer.String((*iter)->GetBaudString());
            }
            writer.EndObject();
        }
        writer.EndArray();
        delete touchProtocols;
    }

	int32_t hOffsetEGM;
	int32_t vOffsetEGM;
	int32_t hOffsetMonitor;
	int32_t vOffsetMonitor;
	GetVideoOffsets(hOffsetEGM, vOffsetEGM, hOffsetMonitor, vOffsetMonitor);
	writer.Key("EGMOffsetHorizontal");			writer.Int(hOffsetEGM);
	writer.Key("EGMOffsetVertical");			writer.Int(vOffsetEGM);
	writer.Key("MonitorOffsetHorizontal");		writer.Int(hOffsetMonitor);
    writer.Key("MonitorOffsetVertical");		writer.Int(vOffsetMonitor);

    writer.Key("DisplayRotation");              writer.Uint(ConvertDesktopOrientationToWord(m_desktopOrientation));
    writer.Key("TouchRotation");                writer.Uint(ConvertDesktopOrientationToWord(GetTouchScreenRotation()));

	writer.Key("Modes");
	writer.StartArray();
	vector<struct video_mode> videoModeTable = GetVideoModeTable();
	int modeNumber = 0;
	for (vector<struct video_mode>::iterator iter = videoModeTable.begin(); iter != videoModeTable.end(); ++iter)
	{
		writer.StartObject();
		writer.Key("ModeNumber");			writer.Int(modeNumber++);
		writer.Key("PixelClockkHz");	writer.Int((*iter).pixel_clock_khz);
		writer.Key("HSyncActive");		writer.Int((*iter).hsync_active);
		writer.Key("VSyncActive");		writer.Int((*iter).vsync_active);
		writer.Key("HSyncFrontPorch");	writer.Int((*iter).hsync_front_porch);
		writer.Key("HSync");			writer.Int((*iter).hsync);
		writer.Key("HSyncBlank");		writer.Int((*iter).hsync_blank);
		writer.Key("VSyncFrontPorch");	writer.Int((*iter).vsync_front_porch);
		writer.Key("VSync");			writer.Int((*iter).vsync);
		writer.Key("VSyncBlank");		writer.Int((*iter).vsync_blank);
		writer.Key("RefreshRate");		writer.Int((*iter).refresh_rate);
		writer.Key("HSyncPolarity");	writer.String(GetPolarityString((*iter).hsync_polarity));
		writer.Key("VSyncPolarity");	writer.String(GetPolarityString((*iter).vsync_polarity));
		writer.EndObject();
	}
	writer.EndArray();

	writer.EndObject();

	return writeStream.m_str;
}

string CDisplayManager::ResponseForForMediaWindowStatusLog()
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	writer.StartObject();

	writer.Key("Logs");
	writer.StartArray();
	{
		int numLogs(m_mediaWindowLog->GetNumItems());
		CMediaWindowLog* log((CMediaWindowLog*)m_mediaWindowLog->GetFirst());
		for (int index = 0; index < numLogs; index++)
		{
			if (log)
			{
				writer.StartObject();
				string egmDisplay;
				switch (log->GetMediaWindowCode())
				{
				case MWCODE_BANNER:
					egmDisplay = TXT_MECHANIC_MWCODE_TASK_BAR;
					break;
				case MWCODE_BANNER_MWLEFT:
					egmDisplay = TXT_MECHANIC_MWCODE_BANNER_MWLEFT;
					break;
				case MWCODE_BANNER_MWRIGHT:
					egmDisplay = TXT_MECHANIC_MWCODE_BANNER_MWRIGHT;
					break;
				case MWCODE_MWBOTTOM:
					egmDisplay = TXT_MECHANIC_MWCODE_MWBOTTOM;
					break;
				case MWCODE_RESERVED:
					egmDisplay = TXT_MECHANIC_MWCODE_EGMFULL;
					break;
				default:
					// Try to display useful diagnostic information.
					egmDisplay = TXT_NA;
					LogString(ERROR_LOG, "Invalid Media Window log code 0x%x", log->GetMediaWindowCode());
					break;
				}

				writer.Key("TimeStamp");	writer.String(FormatString("(%d) %s", index + 1, log->GetDateTimeAsString().c_str()));
				writer.Key("EgmDisplay");	writer.String(egmDisplay);
				writer.Key("Ocr");			writer.String(CUtilities::MakeStringFromCardID(log->GetCardID()));
				writer.EndObject();
				log = (CMediaWindowLog *)m_mediaWindowLog->GetNext();
			}
			else
			{
				break;
			}
		}
	}
	writer.EndArray();

	writer.EndObject();

	return writeStream.m_str;
}

void CDisplayManager::GetVideoInput(string &signalTypeString, string &signalTypeDetectedString, bool &isInputAuto, uint32_t &inputModeNumber)
{
	uint8_t signalFormat;
	uint8_t signalDetectedFormat;
	
	GetVideoInput(signalFormat, signalDetectedFormat, isInputAuto, inputModeNumber);
	
	signalTypeString = VideoFormatTypeToString(signalFormat);
	signalTypeDetectedString = VideoFormatTypeToString(signalDetectedFormat);
}

void CDisplayManager::GetVideoInput(VideoModeSignalType& signalType, bool& isInputAuto, uint32_t& inputModeNumber)
{
	uint8_t signalFormat;
	uint8_t signalDetectedFormat;

	GetVideoInput(signalFormat, signalDetectedFormat, isInputAuto, inputModeNumber);

	signalType = VideoFormatTypeToEnum(signalFormat);
}

void CDisplayManager::GetVideoInput(uint8_t& signalFormat, uint8_t& signalDetectedFormat, bool& isInputAuto, uint32_t& inputModeNumber)
{
	signalFormat = EE_VIFORMAT_NOT_CONNECTED;
	signalDetectedFormat = EE_VIFORMAT_NOT_CONNECTED;
	isInputAuto = true;
	inputModeNumber = 0;
	
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
		int32_t vmResult = AXVM_GetVideoFormat(DISPLAY_EGM, &signalFormat);
		if (vmResult != VM_Success)
		{
			LogString(ERROR_LOG, "AXVM_GetVideoFormat failure %d", vmResult);
			signalFormat = EE_VIFORAMT_UNKNOWN_ERROR;
		}
		
		vmResult = AXVM_GetDetectedVideoFormat(DISPLAY_EGM, &signalDetectedFormat);
		if (vmResult != VM_Success)
		{
			LogString(ERROR_LOG, "AXVM_GetDetectedVideoFormat failure %d", vmResult);
		}
		
		vmResult = AXVM_GetVIAnalogMode(&inputModeNumber);
		if (vmResult == VM_Success)
		{
			if (inputModeNumber != AutoConfigMode)
			{
				isInputAuto = false;
			}
		}
		else
		{
			LogString(ERROR_LOG, "AXVM_GetVIAnalogMode failure %d", vmResult);
		}
	}
	
	LogString(DISPLAY_LOG, "GetVideoInput - %s - %s - Mode#: %d", VideoFormatTypeToString(signalFormat).c_str(), isInputAuto ? "Auto" : "Manual", inputModeNumber);
}

void CDisplayManager::GetVideoOutput(bool& isOutputAuto, uint32_t& outputModeNumber)
{
	isOutputAuto = true;
	outputModeNumber = 0;
	
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
		int32_t vmResult = AXVM_GetVOMode(&outputModeNumber);
		if (vmResult == VM_Success)
		{
			if (outputModeNumber != AutoConfigMode)
			{
				isOutputAuto = false;
			}
		}
		else
		{
			LogString(ERROR_LOG, "AXVM_GetVOMode failure %d", vmResult);
		}
	}
	
	LogString(DISPLAY_LOG, "GetVideoOutput - %s - Mode#: %d", isOutputAuto ? "Auto" : "Manual", outputModeNumber);
}

std::string CDisplayManager::GetSignalTypeString()
{
	uint8_t signalFormat = EE_VIFORMAT_NOT_CONNECTED;
	
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
		int32_t vmResult = AXVM_GetVideoFormat(DISPLAY_EGM, &signalFormat);
		
		if (vmResult != VM_Success)
		{
			LogString(ERROR_LOG, "AXVM_GetVideoFormat failure %d", vmResult);
			signalFormat = EE_VIFORAMT_UNKNOWN_ERROR;
		}
	}
	
	return VideoFormatTypeToString(signalFormat);
}

std::string CDisplayManager::GetDetectedSignalTypeString()
{
	uint8_t signalFormat = EE_VIFORMAT_NOT_CONNECTED;
	
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
		int32_t vmResult = AXVM_GetDetectedVideoFormat(DISPLAY_EGM, &signalFormat);
		
		if (vmResult != VM_Success)
		{
			LogString(ERROR_LOG, "AXVM_GetDetectedVideoFormat failure %d", vmResult);
			signalFormat = EE_VIFORAMT_UNKNOWN_ERROR;
		}
	}
	
	return VideoFormatTypeToString(signalFormat);
}

std::string CDisplayManager::VideoFormatTypeToString(uint8_t formatType)
{
	string formatTypeString = CVideoModeConfig::UnknownTypeString;
	
	switch (formatType)
	{
	case EE_VIFORMAT_AUTO:
		formatTypeString = CVideoModeConfig::AutoTypeString;
		break;
	case EE_VIFORMAT_DIGITAL_DVI:
		formatTypeString = CVideoModeConfig::DVITypeString;
		break;
	case EE_VIFORMAT_ANALOGUE:
		formatTypeString = CVideoModeConfig::AnalogTypeString;
		break;
	case EE_VIFORMAT_DIGITAL_DP:
		formatTypeString = CVideoModeConfig::DisplayPortTypeString;
		break;
	case EE_VIFORMAT_NOT_CONNECTED:
		formatTypeString = CVideoModeConfig::NotConnectedTypeString;
		break;
	case EE_VIFORAMT_UNKNOWN_ERROR:
		formatTypeString = CVideoModeConfig::ErrorTypeString;
		break;
	default:
	case EE_VIFORMAT_COUNT:
		formatTypeString = CVideoModeConfig::UnknownTypeString;
		break;
	}
		
	return formatTypeString;
}

VideoModeSignalType CDisplayManager::VideoFormatTypeToEnum(uint8_t formatType)
{
	VideoModeSignalType videoModeSignalType = VIDEO_MODE_SIGNAL_AUTO;
	
	switch (formatType)
	{
	case EE_VIFORMAT_DIGITAL_DVI:
		videoModeSignalType = VIDEO_MODE_SIGNAL_DVI;
		break;
	case EE_VIFORMAT_ANALOGUE:
		videoModeSignalType = VIDEO_MODE_SIGNAL_ANALOG;
		break;
	case EE_VIFORMAT_DIGITAL_DP:
		videoModeSignalType = VIDEO_MODE_SIGNAL_DP;
		break;
	default:
	case EE_VIFORMAT_AUTO:
		videoModeSignalType = VIDEO_MODE_SIGNAL_AUTO;
		break;
	}
	
	return videoModeSignalType;
}

TouchScreenProtocols * CDisplayManager::GetTouchscreenInfo()
{
	TouchScreenProtocols *touchScreenProtocols = nullptr;

	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
        uint32_t touchscreenType;
        uint32_t touchscreenVendor;
        uint32_t touchscreenProduct;
		int32_t vmResult = AXVM_GetTSDetected(&touchscreenType, &touchscreenVendor, &touchscreenProduct);
		if (vmResult != VM_Success)
		{
			LogString(ERROR_LOG, "AXVM_GetTSDetected failure %d", vmResult);
		}

        uint32_t baudRate = 0;
        vmResult = AXVM_GetTSBaudRate(&baudRate);
		if (vmResult != VM_Success)
		{
			LogString(ERROR_LOG, "AXVM_GetTSBaudRate failure %d", vmResult);
		}

        uint32_t vendorID;
        vmResult = AXVM_GetTSVendor(&vendorID);
		if (vmResult != VM_Success)
		{
			LogString(ERROR_LOG, "AXVM_GetTSVendor failure %d", vmResult);
		}

        LogString(DISPLAY_LOG, "Current touch screen is %s. Vendor ID: 0x%04x, Baudrate: %d",touchscreenType == EE_TSTYPE_USB ? "USB" : "Serial", vendorID, baudRate );
        touchScreenProtocols = new TouchScreenProtocols(touchscreenType == EE_TSTYPE_USB, vendorID, baudRate);

        uint16_t size = 0;
        struct ts_info *list = NULL;
        vmResult = AXVM_GetTSSuppotList(&size, &list);
        if (vmResult == VM_Success)
        {
            for (int iVendor = 0; iVendor < size; iVendor++)
            {
                string vendorName(list[iVendor].name);
                touchScreenProtocols->AddVendor(list[iVendor].id, vendorName);
                LogString(DISPLAY_LOG, "Vendor ID 0x%04X, Name %s", list[iVendor].id, list[iVendor].name);
            }

            if (list)
            {
                 free(list);
            }
        }
        else
        {
			LogString(ERROR_LOG, "AXVM_GetTSSuppotList failure %d", vmResult);
        }

	}

    return touchScreenProtocols;
}

std::vector<struct video_mode> CDisplayManager::GetVideoModeTable()
{
	std::vector<struct video_mode> videoModeTable;
	
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
		uint32_t numVideoModes = 0;
		int32_t vmResult = AXVM_GetNumberOfVideoTableEntries(&numVideoModes);
		if (vmResult != VM_Success)
		{   
            numVideoModes = 0;
			LogString(ERROR_LOG, _T("AXVM_GetNumberOfVideoTableEntries failure: %d"), vmResult);
		}
		else if (numVideoModes == 0)
		{
			LogString(ERROR_LOG, _T("AXVM_GetNumberOfVideoTableEntries returned no entries"));
		}
		else
		{
			for (int videoModeNumber = 0; videoModeNumber < (int)numVideoModes; videoModeNumber++)
			{
				struct video_mode videoMode;
				vmResult = AXVM_GetVideoTableEntry(videoModeNumber, &videoMode);
				if (vmResult == VM_Success)
				{
					videoModeTable.push_back(videoMode);
				}
			}
		}
	}
	
	return videoModeTable;
}

void CDisplayManager::GetVideoOffsets(int32_t &hOffsetEGM, int32_t &vOffsetEGM, int32_t &hOffsetMonitor, int32_t &vOffsetMonitor)
{
	hOffsetEGM = 0;
	vOffsetEGM = 0;
	hOffsetMonitor = 0;
	vOffsetMonitor = 0;
	
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
		int32_t vmResult = AXVM_GetVIAnalogPos(&hOffsetEGM, &vOffsetEGM);
		if (vmResult != VM_Success)
		{
            hOffsetEGM = 0;
            vOffsetEGM = 0;
			LogString(ERROR_LOG, "AXVM_GetVIAnalogPos failure %d", vmResult);
		}
		vmResult = AXVM_GetVOPos(&hOffsetMonitor, &vOffsetMonitor);
		if (vmResult != VM_Success)
		{
            hOffsetMonitor = 0;
            vOffsetMonitor = 0;
			LogString(ERROR_LOG, "AXVM_GetVOPos failure %d", vmResult);
		}
	}
}

std::string CDisplayManager::GetPolarityString(unsigned char polarity)
{
	const string PositivePolarityString = "Positive";
	const string NegativePolarityString = "Negative";
	
	return (polarity ? NegativePolarityString : PositivePolarityString);
}

bool CDisplayManager::SetInputConfig(VideoModeSignalType signalType, bool inputModeIsAuto, int selectedInputModeIndex)
{
    int32_t vmResult = VM_Success;

	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
        uint8_t format = EE_VIFORMAT_AUTO;
        switch (signalType)
        {
        case VIDEO_MODE_SIGNAL_ANALOG:
            format = EE_VIFORMAT_ANALOGUE;
            break;
        case VIDEO_MODE_SIGNAL_DVI:
            format = EE_VIFORMAT_DIGITAL_DVI;
            break;
        case VIDEO_MODE_SIGNAL_DP:
            format = EE_VIFORMAT_DIGITAL_DP;
            break;
        case VIDEO_MODE_SIGNAL_AUTO:	
        default:
            break;
        }
        
        LogString(DISPLAY_LOG, "Set Video Input Config %s %s Mode #: %d", VideoFormatTypeToString(format).c_str(), inputModeIsAuto ? "Auto" : "Manual", selectedInputModeIndex);
        
        int32_t vmResult = AXVM_SetVideoFormat(DISPLAY_EGM, format);
        if (vmResult != VM_Success)
        {
            LogString(ERROR_LOG, "AXVM_SetVideoFormat failure %d", vmResult);
        }
        else
        {
            if (inputModeIsAuto)
            {
                selectedInputModeIndex = AutoConfigMode;
            }
            vmResult = AXVM_SetVIAnalogMode(selectedInputModeIndex);
            if (vmResult != VM_Success)
            {
                LogString(ERROR_LOG, "AXVM_SetVIAnalogMode failure %d", vmResult);
            }
        }
    }
	
	return (vmResult == VM_Success);
}

bool CDisplayManager::SetOutputConfig(bool outputModeIsAuto, int selectedOutputModeIndex)
{
    int32_t vmResult = VM_Success;

	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
        LogString(DISPLAY_LOG, "Set Video Output %s Mode #: %d", outputModeIsAuto ? "Auto" : "Manual", selectedOutputModeIndex);
        
        if (outputModeIsAuto)
        {
            selectedOutputModeIndex = AutoConfigMode;
        }
        vmResult = AXVM_SetVOMode(selectedOutputModeIndex);
        if (vmResult != VM_Success)
        {
            LogString(ERROR_LOG, "AXVM_SetVOMode failure %d", vmResult);
        }
    }
	
	return (vmResult == VM_Success);
}

bool CDisplayManager::SetVideoOffsets(int egmHorizontalOffset, int egmVerticalOffset, int monitorHorizontalOffset, int monitorVerticalOffset)
{
    int32_t vmResult1 = VM_Success;
    int32_t vmResult2 = VM_Success;

	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
        LogString(DISPLAY_LOG, "Set Video Offset- egm horizontal: %d, vertical: %d. Monitor horizonal: %d, vertical: %d", egmHorizontalOffset, egmVerticalOffset, monitorHorizontalOffset, monitorVerticalOffset);
	
        vmResult1 = AXVM_SetVIAnalogPos(egmHorizontalOffset, egmVerticalOffset);
        if (vmResult1 != VM_Success)
        {
            LogString(ERROR_LOG, "AXVM_SetVIAnalogPos failure %d", vmResult1);
        }
        
        vmResult2 = AXVM_SetVOPos(monitorHorizontalOffset, monitorVerticalOffset);
        if (vmResult2 != VM_Success)
        {
            LogString(ERROR_LOG, "AXVM_SetVOPos failure %d", vmResult2);
        }
    }
	
	return (vmResult1 == VM_Success && vmResult2 == VM_Success);
}


bool CDisplayManager::ShowMouseCursor(bool show)
{
	string cmd;
	if (show)
	{
		LogString(DISPLAY_LOG, "Showing mouse cursor");
		cmd = CmdStringHideMouse + " 0";
	}
	else
	{
		LogString(DISPLAY_LOG, "Hiding mouse cursor");
		cmd = CmdStringHideMouse + " 1";
	}
	return system(cmd.c_str()) == 0;
}

void CDisplayManager::KeepVideoModeChange()
{
	lock_guard<recursive_mutex> lock(m_revertVideoModeConfigMutex);
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform && m_revertVideoModeConfig != nullptr)
	{
		m_revertVideoModeConfig.reset();
	}
}

bool CDisplayManager::InitializeVideoMixer()
{
	int vmResult = VM_Success;
	
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform)
	{
		//uint16_t width = 0;
		//uint16_t height = 0;
		
		LogString(DISPLAY_LOG, "Opening Video Mixer device");
		vmResult = AXVM_OpenDevice();
		if (vmResult != VM_Success)
		{
			LogString(ERROR_LOG, _T("AXVM_OpenDevice failure %d"), vmResult);
		}
		else
		{
			LogString(DISPLAY_LOG, "Video Mixer device opened successfully");
		}
		
        // Enable the display
        EnableDisplay(true);
	
		struct axvm_version vmVersion;
		vmResult = AXVM_GetFirmwareVersion(&vmVersion);
		if (vmResult != VM_Success)
		{
			LogString(ERROR_LOG, _T("AXVM_GetFirmwareVersion failure %d"), vmResult);
		}
		else
		{
			LogString(GENERAL, _T("VM Firmware: Core Version: %d.%d.%d. Touch Version: %d.%d.%d"), 
					vmVersion.core_major, vmVersion.core_minor, vmVersion.core_build, 
					vmVersion.touch_major, vmVersion.touch_minor, vmVersion.touch_build);
			
			LogString(DISPLAY_LOG, _T("VM Firmware: Core Version: %d.%d.%d. Touch Version: %d.%d.%d"), 
					vmVersion.core_major, vmVersion.core_minor, vmVersion.core_build, 
					vmVersion.touch_major, vmVersion.touch_minor, vmVersion.touch_build);

			m_mixerFirmwareVersion = FormatString("%d.%d.%d", vmVersion.core_major, vmVersion.core_minor, vmVersion.core_build);
			m_touchFirmwareVersion = FormatString("%d.%d.%d", vmVersion.touch_major, vmVersion.touch_minor, vmVersion.touch_build);
		}
		
		/* Get the resolution of the desktop*/
        m_desktopResolution = GetDisplayResolution(DISPLAY_PT);
	
		/* Get the resolution of Video Mixer display*/
        m_mixerResolution = GetDisplayResolution(DISPLAY_MIXER);
	
		/* Get the resolution of EGM display */
        m_EGMResolution = GetDisplayResolution(DISPLAY_EGM);
		
		// Get the input and output just so they can be logged.
		uint8_t signalFormat;
		uint8_t signalDetectedFormat;
		bool isAuto;
		uint32_t modeNumber;
		GetVideoInput(signalFormat, signalDetectedFormat, isAuto, modeNumber);
		GetVideoOutput(isAuto, modeNumber);
	}
	
	return (vmResult == VM_Success);
}

void CDisplayManager::EnableDisplay(bool enableEGMDisplay)
{
    uint8_t displayTypes = enableEGMDisplay ? EnableDisplayAll : EnableDisplayDesktop;

    int vmResult = AXVM_EnableDisplay(displayTypes);
    if (vmResult != VM_Success)
    {
        LogString(ERROR_LOG, _T("AXVM_EnableDisplay failure %d"), vmResult);
    }
    else
    {
        LogString(DISPLAY_LOG, "All displays enabled successfully");
    }
}

SIZE CDisplayManager::GetDisplayResolution(uint8_t displayType)
{
    uint16_t width = 0;
    uint16_t height = 0;
    SIZE resolution = ZERO_SIZE;
    int vmResult = AXVM_GetDisplayResolution(displayType, &width, &height);
    if (vmResult != VM_Success)
    {
        LogString(ERROR_LOG, _T("AXVM_GetDisplayResolution failure for %s: %d"), GetDisplayTypeString(displayType).c_str(), vmResult);
    }
    else
    {
        resolution = SIZE(width, height);
        LogString(DISPLAY_LOG, "Display Resolution for %s: %d x %d", GetDisplayTypeString(displayType).c_str(), width, height);
    }

    if (displayType == DISPLAY_EGM) 
    {
        m_EGMResolutionCheckTick = CUtilities::GetTickCount();
    }
    
    return resolution;
}

std::string CDisplayManager::GetDisplayTypeString(uint8_t displayType)
{
    const string DisplayTypeUnknown = "Unknown";
    const string DisplayTypeEGM = "EGM";
    const string DisplayTypeMixer = "MIXER";
    const string DisplayTypePlayerTracking = "PT";

    string displayTypeString = DisplayTypeUnknown;
    switch (displayType) 
    {
    case DISPLAY_EGM:
        displayTypeString = DisplayTypeEGM;
        break;
    case DISPLAY_MIXER:
        displayTypeString = DisplayTypeMixer;
        break;
    case DISPLAY_PT:
        displayTypeString = DisplayTypePlayerTracking;
        break;
    default:
        displayTypeString = DisplayTypeUnknown;
        break;
    }

    return displayTypeString;
}

// CDisplayManager::CardRemoved
//   OUT playerStatePending - determines if a media window player state change is pending.
//   RETURN - whether a media window open/close change occurred.
// Indicates that a card (player or employee) was removed.
MediaWindowPlayerState CDisplayManager::CardRemoved(bool wasPlayerCard,
	MediaWindowPlayerStatePending &playerStatePending, bool bannedPlayerIsDisplayed)
{
	MediaWindowPlayerState returnValue = MAINTAIN_PLAYER_STATE;
	playerStatePending = MAINTAIN_PLAYER_STATE_PENDING;

	// Reset timer before changing state.
	ResetAutoOpenTimer();

	lock_guard<recursive_mutex> lock(m_settingWindowsCS);

	m_insertedCard = CARD_INSERTED_NONE;
	SetVideoMixerDisplay();

	if ( m_isInitialized )
	{
		DisplayPlatform platformType = GetDisplayPlatform();
		if (platformType == VideoMixerPlatform && wasPlayerCard)
		{
			MediaWindowPlayerStateCreditMeterMode creditMeterMode = PLAYER_STATE_CREDIT_METER_WAIT_FOR_ZERO;

			MediaWindowPlayerState mediaWindowPlayerState(m_mediaWindowConfig.GetCardOutPlayerState());
			if (bannedPlayerIsDisplayed)
			{
				mediaWindowPlayerState = MAINTAIN_PLAYER_STATE;
			}
			returnValue = UpdatePlayerState(creditMeterMode, mediaWindowPlayerState, playerStatePending);
		}
	}

	return returnValue;
}

// CDisplayManager::UpdatePlayerState
//   IN creditMeterMode - indicates the credit meter requirements for achieving the targetPlayerState.
//   IN targetPlayerState - indicates the desired media window open or close state, if required conditions can be met.
//   OUT playerStatePending - determines if a media window player state change is pending.
//   RETURN - whether a media window open/close change occurred.
// Updates the current and pending media window open/closed state depending on parameters and internal state.
MediaWindowPlayerState CDisplayManager::UpdatePlayerState(MediaWindowPlayerStateCreditMeterMode creditMeterMode,
	MediaWindowPlayerState targetPlayerState,
	MediaWindowPlayerStatePending &playerStatePending)
{
	string targStr = FormatString(_T("TargPlayState=%d"), (int)targetPlayerState);
	
	MediaWindowPlayerState returnValue = MAINTAIN_PLAYER_STATE;
	playerStatePending = MAINTAIN_PLAYER_STATE_PENDING; // Clear any prior pending change.

	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform && m_gameCommUp)
	{
		if (creditMeterMode != PLAYER_STATE_CREDIT_METER_NORMAL ||
			(!m_gameHasCredits && IsExpiredCreditMeterZeroDelayOpenTimer()))
		{
			if (IsExpiredDelayOpenTimer())
			{
				switch (targetPlayerState)
				{
				case OPEN_PLAYER_STATE:
					if (!m_serviceWindowIsOpen)
					{
						if (creditMeterMode == PLAYER_STATE_CREDIT_METER_WAIT_FOR_ZERO && m_gameHasCredits)
						{
							playerStatePending = OPEN_PLAYER_STATE_PENDING_CARD_OUT_TIMEOUT;
						}
						else if (m_playerState.GetGameInProgress())
						{
							// Wait until game is not in progress to open window.
							playerStatePending = (creditMeterMode == PLAYER_STATE_CREDIT_METER_IGNORE) ?
								OPEN_PLAYER_STATE_PENDING_IGNORE_CREDIT_METER :
								OPEN_PLAYER_STATE_PENDING;
						}
						else
						{
							// It is okay to open window now.
							ShowServiceWindow();
							returnValue = OPEN_PLAYER_STATE;
						}
					}
					break;
				case CLOSE_PLAYER_STATE:
					if (m_serviceWindowIsOpen)
					{
						if (creditMeterMode == PLAYER_STATE_CREDIT_METER_WAIT_FOR_ZERO && m_gameHasCredits)
						{
							playerStatePending = CLOSE_PLAYER_STATE_PENDING_CARD_OUT_TIMEOUT;
						}
						else if (m_pbtInProgress || m_playerState.GetGameInProgress())
						{
							// Wait until PBT and game are not in progress to close window.
							playerStatePending = (creditMeterMode == PLAYER_STATE_CREDIT_METER_IGNORE) ?
								CLOSE_PLAYER_STATE_PENDING_IGNORE_CREDIT_METER :
								CLOSE_PLAYER_STATE_PENDING;
						}
						else
						{
							// It is okay to close window now.
							HideServiceWindow();
							returnValue = CLOSE_PLAYER_STATE;
						}
					}
					break;
				case MAINTAIN_PLAYER_STATE:
				default:
					break;
				}
			}
		}
	}

	return returnValue;
}

// CDisplayManager::UpdatePlayerStatePendingOpen
//   RETURN - whether a media window open/close change occurred.
// Open media window if it is not already open.
MediaWindowPlayerState CDisplayManager::UpdatePlayerStatePendingOpen()
{
	MediaWindowPlayerState returnValue = MAINTAIN_PLAYER_STATE;

	if (!m_serviceWindowIsOpen)
	{
		ShowServiceWindow();
		returnValue = OPEN_PLAYER_STATE;
	}

	return returnValue;
}

// CDisplayManager::UpdatePlayerStatePendingClose
//   RETURN - whether a media window open/close change occurred.
// Close media window if it is not already closed.
MediaWindowPlayerState CDisplayManager::UpdatePlayerStatePendingClose()
{
	MediaWindowPlayerState returnValue = MAINTAIN_PLAYER_STATE;

	if (m_serviceWindowIsOpen)
	{
		HideServiceWindow();
		returnValue = CLOSE_PLAYER_STATE;
	}

	return returnValue;
}

// CDisplayManager::UpdatePlayerStatePending
//   IN close - indicates whether target media window state is closed or open.
//   RETURN - whether a media window open/close change occurred.
// If (close) then close media window if it is not already closed.
// Else open media window if it is not already open.
MediaWindowPlayerState CDisplayManager::UpdatePlayerStatePending(bool close)
{
	MediaWindowPlayerState returnValue = MAINTAIN_PLAYER_STATE;

	if (close)
	{
		returnValue = UpdatePlayerStatePendingClose();
	}
	else
	{
		returnValue = UpdatePlayerStatePendingOpen();
	}

	return returnValue;
}

// CDisplayManager::UpdatePlayerStatePending
//   IN/OUT playerStatePending - determines if a media window player state change is waiting for state changes
//      before performing an open or close operation.
//   RETURN - whether a media window open/close change occurred.
// Updates the current and pending media window open/closed state depending on parameters and internal state.
// This routine should be called multiple times per second while playerStatePending != MAINTAIN_PLAYER_STATE_PENDING
// in order to perform updates in a timely manner.
MediaWindowPlayerState CDisplayManager::UpdatePlayerStatePending(MediaWindowPlayerStatePending &playerStatePending)
{
	MediaWindowPlayerState returnValue = MAINTAIN_PLAYER_STATE;

	bool clearPlayerStatePending = true;
	bool close = false; // If not close then treat m_pbtInProgress as false -- i.e., ignore m_pbtInProgress when an open is pending.
	DisplayPlatform platformType = GetDisplayPlatform();
	if (platformType == VideoMixerPlatform && m_gameCommUp && IsExpiredDelayOpenTimer())
	{
		switch (playerStatePending)
		{
		case CLOSE_PLAYER_STATE_PENDING:
			close = true;
			// Fall thru
		case OPEN_PLAYER_STATE_PENDING:
			if ((close && m_pbtInProgress) || m_playerState.GetGameInProgress())
			{
				// Wait until PBT and game are not in progress to clear pending state.
				clearPlayerStatePending = false;
			}
			else if (!m_gameHasCredits)
			{
				// Okay to change.
				returnValue = UpdatePlayerStatePending(close);
			}
			break;

		case CLOSE_PLAYER_STATE_PENDING_IGNORE_CREDIT_METER:
			close = true;
			// Fall thru
		case OPEN_PLAYER_STATE_PENDING_IGNORE_CREDIT_METER:
			if ((close && m_pbtInProgress) || m_playerState.GetGameInProgress())
			{
				// Wait until PBT and game are not in progress to clear pending state.
				clearPlayerStatePending = false;
			}
			else
			{
				// Okay to change.
				returnValue = UpdatePlayerStatePending(close);
			}
			break;

		case CLOSE_PLAYER_STATE_PENDING_CREDIT_METER_ZERO_DELAY:
			close = true;
			// Fall thru
		case OPEN_PLAYER_STATE_PENDING_CREDIT_METER_ZERO_DELAY:
			if (!m_gameHasCredits && !IsExpiredCreditMeterZeroEndSessionDelay())
			{
				// Wait until credit meter non-zero or delay period expired to clear pending state.
				clearPlayerStatePending = false;
			}
			else if (!(close && m_pbtInProgress) && !m_playerState.GetGameInProgress() && !m_gameHasCredits)
			{
				// Okay to change.
				returnValue = UpdatePlayerStatePending(close);
			}
			break;

		case CLOSE_PLAYER_STATE_PENDING_CARD_OUT_TIMEOUT:
			close = true;
			// Fall thru
		case OPEN_PLAYER_STATE_PENDING_CARD_OUT_TIMEOUT:
			if ((close && m_pbtInProgress) || m_playerState.GetGameInProgress() || m_gameHasCredits)
			{
				if (!IsExpiredCardOutCreditMeterZeroTimer())
				{
					// Wait for timeout to clear pending state.
					clearPlayerStatePending = false;
				}
			}
			else
			{
				// Okay to change.
				returnValue = UpdatePlayerStatePending(close);
			}
			break;

		case CLOSE_PLAYER_STATE_PENDING_GAME_PLAY_ENDED_DELAY:
			close = true;
			// Fall thru
		case OPEN_PLAYER_STATE_PENDING_GAME_PLAY_ENDED_DELAY:
			// Elsewhere game start will clear pending state.
			// Elsewhere player card in will clear pending state.
			// Elsewhere credit meter non-zero will clear pending state.
			{
				DWORD currentTickCount = CUtilities::GetTickCount();
				if (!m_playerState.IsExpiredGameNotInProgressEndSessionDelay(currentTickCount))
				{
					// Wait for timeout to clear pending state.
					clearPlayerStatePending = false;
				}
				else
				{
					if (close && m_pbtInProgress)
					{
						// Let pending state be cleared.
					}
					else
					{
						// Okay to change.
						returnValue = UpdatePlayerStatePending(close);
					}
				}
			}
			break;

		case MAINTAIN_PLAYER_STATE_PENDING:
		default:
			break;
		}
	}

	if (clearPlayerStatePending)
	{
		playerStatePending = MAINTAIN_PLAYER_STATE_PENDING;
	}

	return returnValue;
}


bool CDisplayManager::IsExpiredAutoSwitchWaitTime()
{
	DWORD autoOpenTimeSpan = (CUtilities::GetUTCTime() - m_serviceWindowAutoOpenTimer) / SECONDS_PER_MINUTE;
	bool retVal = autoOpenTimeSpan >= m_mediaWindowConfig.GetAutoSwitchWaitTime();
	return retVal;
}

// ResetDelayOpenTimer:
// Set the timer to expire after the specified delay.
// Currently, timing is not critical, so a lock is not needed to make timer updates atomic.  2010-11-03
void CDisplayManager::ResetDelayOpenTimer(WORD secondsToDelay)
{
	m_serviceWindowDelayOpenTimerPeriod = secondsToDelay;
	m_serviceWindowDelayOpenTimer = CUtilities::GetUTCTime();
}

// UpdateDelayOpenTimer:
// Ensure the specified delay before timer expires.
// The timer is not modified if it would not expire until after the specified delay.
void CDisplayManager::UpdateDelayOpenTimer(WORD secondsToDelay)
{
	time_t delayOpenTimeSpan = CUtilities::GetUTCTime() - m_serviceWindowDelayOpenTimer;
	time_t secondsRemaining = 0;
	if (m_serviceWindowDelayOpenTimerPeriod > delayOpenTimeSpan)
	{
		secondsRemaining = m_serviceWindowDelayOpenTimerPeriod - delayOpenTimeSpan;
	}

	if (secondsRemaining < secondsToDelay)
	{
		ResetDelayOpenTimer(secondsToDelay);
	}
}

bool CDisplayManager::IsExpiredDelayOpenTimer()
{
	return CUtilities::GetUTCTime() - m_serviceWindowDelayOpenTimer >= (time_t)m_serviceWindowDelayOpenTimerPeriod;
}

// CDisplayManager::IsExpiredCreditMeterZeroDelayOpenTimer
//   RETURN - true if the normal period for delaying changes since the credit changed to zero has expired.
//      - returns false otherwise.
// Returns whether the timer indicated above has expired.
bool CDisplayManager::IsExpiredCreditMeterZeroDelayOpenTimer()
{
	DWORD millisecondsSinceGameCreditsBecameZero = CUtilities::GetTickCount() - m_gameCreditsBecameZeroTickCount;
	return millisecondsSinceGameCreditsBecameZero >= GAME_CHG_MILLISECONDS_TO_AUTO_OPEN;
}

// CDisplayManager::IsExpiredCreditMeterZeroBeginSessionDelay
//   RETURN - true if the period that game credits need to be zero before starting an (uncarded) player session has elapsed.
//      - returns false otherwise.
// Returns whether the timer indicated above has expired.
bool CDisplayManager::IsExpiredCreditMeterZeroBeginSessionDelay()
{
	DWORD millisecondsSinceGameCreditsBecameZero = CUtilities::GetTickCount() - m_gameCreditsBecameZeroTickCount;
	bool returnValue = millisecondsSinceGameCreditsBecameZero >= m_mediaWindowConfig.GetGameCreditsBecameZeroBeginSessionDelayInMilliseconds();
	return returnValue;
}

// CDisplayManager::IsExpiredCreditMeterZeroEndSessionDelay
//   RETURN - true if the delay period expired for an uncarded player session end after the credit meter became zero.
//      - returns false otherwise.
// Returns whether the timer indicated above has expired.
// Note that the delay period expires after the period elapses, regardless of the credit meter state.
bool CDisplayManager::IsExpiredCreditMeterZeroEndSessionDelay()
{
	DWORD millisecondsSinceGameCreditsBecameZero = CUtilities::GetTickCount() - m_gameCreditsBecameZeroTickCount;
	bool returnValue = millisecondsSinceGameCreditsBecameZero >= m_mediaWindowConfig.GetGameCreditsBecameZeroEndSessionDelayInMilliseconds();
	return returnValue;
}

// CDisplayManager::IsExpiredCardOutCreditMeterZeroTimer
//   RETURN - true if the timer expired for waiting for the credit meter to become zero after a player card out
//      in order to perform a player state change due to player card out
//      - returns false otherwise.
// Returns whether the timer indicated above has expired.
// Note that the timer expires after the period elapses, regardless of the credit meter state.
bool CDisplayManager::IsExpiredCardOutCreditMeterZeroTimer()
{
	DWORD millisecondsSincePlayerCardOut = CUtilities::GetTickCount() - m_playerCardOutTickCount;
	bool returnValue = millisecondsSincePlayerCardOut >= m_mediaWindowConfig.GetCardOutGameCreditsBecomeZeroTimeoutInMilliseconds();
	return returnValue;
}

// TBD. This needs to be revisted when when service window opening and closing is re-implemented
void CDisplayManager::ResetAutoOpenTimer()
{
	m_serviceWindowAutoOpenTimer = CUtilities::GetUTCTime();
}

void CDisplayManager::UpdateServiceWindowSize()
{
	lock_guard<recursive_mutex> lock(m_settingWindowsCS);

	m_serviceWindowSize = m_mediaWindowConfig.GetContentWindowSize();
}

void CDisplayManager::UpdateTaskbarSize()
{
	lock_guard<recursive_mutex> lock(m_settingWindowsCS);
	
	m_taskbarSize = m_mediaWindowConfig.GetTaskbarSize();
}

// Calculate (the position and size of) the EGM and CDM rectangles,
// using either the service desktop coordinates or the VEB video mixer buffer/output coordinates.
// When the service desktop coordinates are used, it is expected that the EGM rectangle
// will not be used by the caller, even though it is needed as an intermediate result
// for calculating the CDM rectangle.
void CDisplayManager::GetEgmPositionAndSize(POINT &position, SIZE &size)
{	
	LogString(DISPLAY_LOG, "GetEgmPositionAndSize: Service Window:%d. Taskbar:%d. vm_mode:%s. orientation:%d. rotation:%d", m_serviceWindowSize, m_taskbarSize, VideoMixerModeStr[m_vmMode], m_mediaWindowConfig.GetContentWindowOrientation(), m_desktopOrientation);

	// Start with the position and size as if full screen EGM
	position = DEFAULT_POINT;
	size = m_EGMResolution;

	// If we are not to show the EGM display at all, set EGM to zero size 
	if (m_vmMode == VMM_FULL_SERVICE_WINDOW || m_vmMode == VMM_TFT)
	{
		size = ZERO_SIZE;
	}
	
	// If not in full screen EGM
	else if (m_vmMode != VMM_FULLEGM && m_vmMode != VMM_SHUTDOWN)
	{
		long heightReductionPercentage = m_taskbarSize;
		ContentWindowOrientation orientation = m_mediaWindowConfig.GetContentWindowOrientation();

        // If we are rotated 90 or 270 we need to swap the width and height of the resolution.
        // Otherwise use the resolution as-is
        SIZE egmResolution = m_EGMResolution;
        if (m_desktopOrientation == DesktopOrientation90 || m_desktopOrientation == DesktopOrientation270) 
        {
            egmResolution.cx = m_EGMResolution.cy;
            egmResolution.cy = m_EGMResolution.cx;
        }

        // This will be our working position and size.
        POINT egmPosition = DEFAULT_POINT;
        SIZE egmSize = egmResolution;
        
		if (m_vmMode == VMM_EMPLOYEE_SERVICE_WINDOW || m_vmMode == VMM_SERVICE_WINDOW)
		{
			// Add the bottom window height to the height reduction percentage
			if (orientation == CWORIENTATION_BOTTOM_OR_LCD)
			{
				heightReductionPercentage += m_serviceWindowSize;
			}

			// Side Window
			else
			{
				// Adjust the EGM width if side window
				egmSize.cx = (egmResolution.cx * (100 - m_serviceWindowSize)) / 100;
				
				// Adjust the x position if left window
				if (orientation == CWORIENTATION_LEFT)
				{
					egmPosition.x = (egmResolution.cx * m_serviceWindowSize) / 100;
				}
			}
		}
		
		// Add the employee window to the height reduction percentage
		if (m_vmMode == VMM_EMPLOYEE_BANNER || m_vmMode == VMM_EMPLOYEE_SERVICE_WINDOW)
		{
			heightReductionPercentage += EmployeeScreenHeightPercentage;
		}
	
		// Reduce the height of the egm window
		egmSize.cy = (egmResolution.cy * (100 - heightReductionPercentage)) / 100;
		
		// Adjust the calculated EGM display size for maintaining the EGM display aspect ratio
		if (m_mediaWindowConfig.GetMaintainAspectRatio())
		{
			long originalEGMDisplayAspectRatio = (egmResolution.cx * 100) / egmResolution.cy;
			long newEGMDisplayAspectRatio = (egmSize.cx * 100) / egmSize.cy;
			
			// Adjust the width to maintain EGM aspect ratio
			if (newEGMDisplayAspectRatio > originalEGMDisplayAspectRatio)
			{
				long preadjustedWidth = egmSize.cx;
				
				// Adjust the width
				egmSize.cx = (egmResolution.cx * egmSize.cy) / egmResolution.cy;
				
				// Center the EGM display horizontally to account for the reduction in width.
				if (preadjustedWidth > egmSize.cx)
				{
					egmPosition.x += ((preadjustedWidth - egmSize.cx) / 2);
				}
			}
			// Adjust the height to maintain EGM aspect ratio
			else
			{
				long preadjustedHeight = egmSize.cy;
				
				// Adjust the height
				egmSize.cy = (egmResolution.cy * egmSize.cx) / egmResolution.cx;
				
				// Center the EGM display vertically to account for the reduction in height.
				if (preadjustedHeight > egmSize.cy)
				{
					egmPosition.y += ((preadjustedHeight - egmSize.cy) / 2);
				}
			}
		}

        LogString(DISPLAY_LOG, "GetEgmPositionAndSize - before rotation: Resolution: %ld x %ld. Position: %ld x %ld. Size: %ld x %ld", egmResolution.cx, egmResolution.cy, egmPosition.x, egmPosition.y, egmSize.cx, egmSize.cy);
		
		// Adjust the position and sizes for rotation	
		switch (m_desktopOrientation)
		{
		case DesktopOrientation90:
            LogString(DISPLAY_LOG, "GetEgmPositionAndSize: rotating 270" );
			position.x = egmResolution.cy - egmSize.cy - egmPosition.y;
            position.y = egmPosition.x;
            size.cx = egmSize.cy;
            size.cy = egmSize.cx;
			break;
			
		case DesktopOrientation180:
            LogString(DISPLAY_LOG, "GetEgmPositionAndSize: rotating 180" );
			position.x = egmResolution.cx - egmSize.cx - egmPosition.x;
            position.y = egmResolution.cy - egmSize.cy - egmPosition.y;
            size = egmSize;
			break;
			
        case DesktopOrientation270:
            LogString(DISPLAY_LOG, "GetEgmPositionAndSize: rotating 90" );
			position.x = egmPosition.y;
            position.y = egmResolution.cx - egmSize.cx - egmPosition.x;
            size.cx = egmSize.cy;
            size.cy = egmSize.cx;
			break;

		case DesktopOrientationDefault:
        default:
            LogString(DISPLAY_LOG, "GetEgmPositionAndSize: no rotation" );
            position = egmPosition;
            size = egmSize;
			break;
		}

        LogString(DISPLAY_LOG, "GetEgmPositionAndSize - after rotation: Resolution: %ld x %ld. Position: %ld x %ld. Size: %ld x %ld", m_EGMResolution.cx, m_EGMResolution.cy, position.x, position.y, size.cx, size.cy);
	}
}

// CDisplayManager::UpdateGameNotInProgress
//   IN pbtInProgress - indicates whether PBT operations and/or PBT displays are pending or in progress that affect CDisplayManager operation.
//      - The CDisplayManager instance internal state is updated to match the pbtInProgress parameter value.
//   IN/OUT playerStatePendingArg - indicates if a media window player state change is pending.
//   RETURN - whether a media window open/close change occurred.
// Called when an EGM game has ended (or is not in progress).
// If this game end is for the first play (of a carded or uncarded player session) and it causes an immediate or pending state change
// then the incoming playerStatePendingArg value is ignored; otherwise the incoming playerStatePendingArg value is retained.
MediaWindowPlayerState CDisplayManager::UpdateGameNotInProgress(bool pbtInProgress,
	MediaWindowPlayerStatePending &playerStatePendingArg)
{
	MediaWindowPlayerState returnValue = MAINTAIN_PLAYER_STATE;
	MediaWindowPlayerStatePending playerStatePending = MAINTAIN_PLAYER_STATE_PENDING; // Clear any prior pending change.

	m_pbtInProgress = pbtInProgress;

	ResetAutoOpenTimer();
	DWORD currentTickCount = CUtilities::GetTickCount();
	m_playerState.SetGameInProgress(false, currentTickCount);

	if (!m_playerState.GetHasFirstPlay())
	{
		m_playerState.SetHasFirstPlay(true);
		MediaWindowPlayerStateCreditMeterMode creditMeterMode = PLAYER_STATE_CREDIT_METER_IGNORE;
		returnValue = UpdatePlayerState(creditMeterMode, m_mediaWindowConfig.GetFirstPlayPlayerState(), playerStatePending);
	}

	if (returnValue == MAINTAIN_PLAYER_STATE && playerStatePending == MAINTAIN_PLAYER_STATE_PENDING)
	{
		if (!m_gameHasCredits)
		{
			// Set a new pending state if condition(s) are met.
			if (CARD_INSERTED_NONE == m_insertedCard)
			{
				// m_insertedCard is already checked, other requirements are checked by AutoOpenCloseServiceWindowCheck().
				switch (m_mediaWindowConfig.GetCardOutPlayerState())
				{
				case OPEN_PLAYER_STATE:
					playerStatePending = OPEN_PLAYER_STATE_PENDING_GAME_PLAY_ENDED_DELAY;
					break;

				case CLOSE_PLAYER_STATE:
					playerStatePending = CLOSE_PLAYER_STATE_PENDING_GAME_PLAY_ENDED_DELAY;
					break;

				case MAINTAIN_PLAYER_STATE:
				default:
					break;
				}
			}
		}
	}

	// Check if we need to clear or override the incoming playerStatePendingArg value.
	if (returnValue != MAINTAIN_PLAYER_STATE || playerStatePending != MAINTAIN_PLAYER_STATE_PENDING)
	{
		// First play changed the state, replace the incoming playerStatePendingArg value.
		playerStatePendingArg = playerStatePending;
	}

	return returnValue;
}


// CDisplayManager::SaveMediaWindowCode
//     IN mediaWindowCode - code specifying the current state.
// If the state code has changed then log the new state code.
void CDisplayManager::SaveMediaWindowCode(DWORD mediaWindowCode)
{
	if (m_mediaWindowCode != mediaWindowCode)
	{
		// Create log entry first and then change state, so we don't have state changes without log entries.
		CMediaWindowLog *log = new CMediaWindowLog(mediaWindowCode, GetLastCardRead());
		m_mediaWindowLog->Add(log);

		m_mediaWindowCode = mediaWindowCode;
	}
}

// CDisplayManager::GetMediaWindowCode 
// Return a code describing the current Media Window status for logging.
// Extend as needed, bit flags, etc.
DWORD CDisplayManager::GetMediaWindowCode()
{
	if (m_serviceWindowIsOpen)
	{
		switch (m_mediaWindowConfig.GetContentWindowOrientation())
		{
		case CWORIENTATION_BOTTOM_OR_LCD:
			return MWCODE_MWBOTTOM;
			break;
		case CWORIENTATION_LEFT:
			return MWCODE_BANNER_MWLEFT;
			break;
		default:
			return MWCODE_BANNER_MWRIGHT;
			break;
		}
	}
	else if (m_vmMode == VMM_FULLEGM)
		return MWCODE_RESERVED;

	return MWCODE_BANNER;
}

// CDisplayManager::UpdateMediaWindowCode
// Determine the media window code for the current state and then save that state.
void CDisplayManager::UpdateMediaWindowCode()
{
	SaveMediaWindowCode(GetMediaWindowCode());
}


string CDisplayManager::ConvertDesktopOrientationToString(DesktopOrientationType desktopOrientation)
{
    string rotationString = RotationString0;

    switch (desktopOrientation) 
    {
    case DesktopOrientation90:
        rotationString = RotationString90;
        break;
    case DesktopOrientation180:
        rotationString = RotationString180;
        break;
    case DesktopOrientation270:
        rotationString = RotationString270;
        break;
    default:
    case DesktopOrientationDefault:
        rotationString = RotationString0;
        break;
    }

    return rotationString;
}

WORD CDisplayManager::ConvertDesktopOrientationToWord(DesktopOrientationType desktopOrientation)
{
    WORD retWord = 0;
    switch( desktopOrientation )
    {
    case DesktopOrientation90:
        retWord = 90;
        break;
    case DesktopOrientation180:
        retWord = 180;
        break;
    case DesktopOrientation270:
        retWord = 270;
        break;
    case DesktopOrientationDefault:
    default:
        retWord = 0;
        break;
    }

    return retWord;
}

DesktopOrientationType CDisplayManager::ConvertDesktopOrientationFromString(const string& desktopOrientationString)
{
    DesktopOrientationType desktopOrientation = DesktopOrientationDefault;

    if (desktopOrientationString == RotationString90) 
    {
        desktopOrientation = DesktopOrientation90;
    }
    else if (desktopOrientationString == RotationString180) 
    {
        desktopOrientation = DesktopOrientation180;
    }
    else if (desktopOrientationString == RotationString270) 
    {
        desktopOrientation = DesktopOrientation270;
    }

    return desktopOrientation;
}

DesktopOrientationType CDisplayManager::ConvertDesktopOrientationFromWord(WORD desktopOrientationWord)
{
    DesktopOrientationType desktopOrientation = DesktopOrientationDefault;

    switch( desktopOrientationWord )
    {
    case 90:
        desktopOrientation = DesktopOrientation90;
        break;
    case 180:
        desktopOrientation = DesktopOrientation180;
        break;
    case 270:
        desktopOrientation = DesktopOrientation270;
        break;
    case 0:
    default:
        desktopOrientation = DesktopOrientationDefault;
        break;
    }

    return desktopOrientation;
}