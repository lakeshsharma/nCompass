#pragma once

////Screen Defines
#include "../GlobalDefs.h"
#include "../metersdef.h"

extern const int DISPLAY_GOAL_PERCENTAGE;

#define ONE_SECOND			1000 //one second in milliseconds

extern const RECT SCREEN_SIZE;
extern const COLORREF TEXT_COLOR;
extern const COLORREF ERROR_TEXT_COLOR;
extern const COLORREF TRANSPARENT_COLOR;
extern const std::string CLICK_SOUND_FILENAME;
extern const int MAX_PIN_METERS;
extern const WORD BACKLIGHT_INCREMENT;
extern const std::string DATE_TIME_MASK;
extern const DWORD FIVE;
extern const DWORD TEN;
extern const DWORD TWENTY;
extern const DWORD FIFTY;
extern const DWORD HUNDRED;
extern const DWORD FOURTY;
extern const DWORD	SIXTY;
extern const DWORD EIGHTY;
extern const std::string DATE_TIME_SECONDS_MASK;
extern const std::string MILITARY_DATE_TIME_MASK;
extern const WORD SW_MIN_SIZE;
extern const WORD SW_MAX_SIZE;
extern const WORD TASKBAR_MIN_SIZE;
extern const WORD TASKBAR_MAX_SIZE;
extern const WORD AUTOCLOSE_MIN_MINUTES;
extern const WORD AUTOCLOSE_MAX_MINUTES;
extern const int VGA_SAMPLING_PHASE_MIN;
extern const int VGA_SAMPLING_PHASE_MAX;
extern const int VGA_SAMPLING_PHASE_RANGE;
extern const int VGA_SAMPLING_PHASE_NA;
extern const int VGA_SAMPLING_PHASE_DEF;
extern const std::string ANNOUNCEMENT_SCREEN;
extern const std::string CARDIN_ELIGIBILITY_SCREEN;
extern const std::string PROXIMITY_SCREEN;
extern const std::string JPACARDED_SCREEN; 
extern const std::string JPANONCARDED_SCREEN;
extern const std::string TOTALIZER_SCREEN;
extern const std::string PNW_END_SCREEN;
extern const std::string CARD_IN_PROMOTION;
extern const std::string PLAYER_ANNOUNCMENT;
extern const std::string NONWINNER_NOTIFICATION_SCREEN;
extern const std::string EVENT_TRIGGERED_SCREEN;
extern const std::string SPLASHDOWN_START_SCREEN;
extern const std::string RICOCHET_START_SCREEN;
extern const std::string CASINO_CHALLENGE_START_SCREEN;
extern const std::string SPLASHDOWN_AWARD_SCREEN;
extern const std::string RICOCHET_CELEBRATION_SCREEN;
extern const std::string CASINO_CHALLENGE_AWARD1_SCREEN;
extern const std::string CASINO_CHALLENGE_AWARD2_SCREEN;
extern const std::string CASINO_CHALLENGE_AWARD3_SCREEN;
extern const std::string CASINO_CHALLENGE_FIVE_MINUTE_WARNING_END_SCREEN;
extern const std::string SPLASHDOWN_END_SCREEN;
extern const std::string SPLASHDOWN_WINNER_END_SCREEN;
extern const std::string CASINO_CHALLENGE_END_SCREEN;

////////////////////////////////////////////////////////
//		Screen ID's 
//		NOTE: only add screens to the bottom of this 
//      so that it matches the default XML file
////////////////////////////////////////////////////////

enum class BonusCounterType
{
	Logo,
	NotificationBox,
	nRichLevel,	
	PlayXGetY,
	ProgressiveOdometer,
	ProgressivePrize,
	PTE,
	SplashdownCountdownTimer,
	ScatterTimer,	
	InvalidCounterType,
};

enum TriggerLessType
{
	CardinPromotion,
	PlayerAnnouncement,
	Totalizer,
	InvalidTriggerLessType
};

enum screenID
{
	screenAttractorScreen,
	screenCardedSessionInProgress,

////////////////////////////////////////////////////////
//		Mechanic Card Items
////////////////////////////////////////////////////////

	screenMechanicPin,				
	screenMechanicEntry,

	//View
	screenMechanicView,				
	screenMechanicViewVersions,		
	screenMechanicViewChecksums,
	screenMechanicViewNetwork,
	
	//Meters
	screenMechanicViewMeters,		
	screenMechanicMetersEGMMeters,
	screenMechanicMetersBills,	
	screenMechanicMetersGame,		
	screenMechanicMetersMeters,		
	screenMechanicPins,

	//Transactions
	screenMechanicViewTransactions,		
	screenMechanicTransactionsAttendantPaid,
	screenMechanicTransactionsVoucherLog,		
	screenMechanicTransactionsPBFailGood,
	screenMechanicTransactionsPBLog,		
	screenMechanicTransactionsTransLog,
	screenMechanicTransactionsTiltLog,
	screenMechanicTransactionsRepairCodes,		
	screenMechanicTransactionsLastGame,

	//Diagnostics
	screenMechanicViewDiagnostics,	
	screenMechanicDiagnosticsPBPending,
	screenMechanicDiagnosticsConnectivity,
	screenMechanicDiagnosticsDiskSpace,
	screenMechanicDiagnosticsFlashStatus,
	screenMechanicDiagnosticsVoucherLimits,
	screenMechanicDiagnosticsLockout,
	screenMechanicDiagnosticsDenomination,
	screenMechanicDiagnosticsLossLimit,

	//Send Messages
	screenMechanicMessages,
	screenMechanicMessagesRepairCodes,

	//Capture Meters
	screenMechanicCaptureMeters,
	screenMechanicCapturingMeters,

////////////////////////////////////////////////////////
//		Global Card Items
////////////////////////////////////////////////////////

	screenGlobalPin,
	screenGlobalEntry,
	
	screenGlobalSetSlotMastID, //SlotMast ID
	
	//Network Settings
	screenGlobalNetworkSettings,
	screenGlobalPollerIP,
	screenGlobalDHCPConfig,
	screenGlobalIPAddress, //My IP
	screenGlobalSubnetMask,
	screenGlobalDefaultGateway,
	screenGlobalPreferredDNS,
	screenGlobalAlternateDNS,

	//EGM Settings
	screenGlobalEGMSettings,
	
	//Pin Settings
	screenGlobalPinSettings,

	//Calibration settings
	screenGlobalCalibration,
	screenGlobalCalibratingScreen,
	screenGlobalBacklight,

	//Utility
	screenGlobalUtility,

////////////////////////////////////////////////////////
//		Floor Card Items
////////////////////////////////////////////////////////

	screenFloorPin,
	screenFloorEntry,
	screenFloorAttendantPaidPayout,
	screenFloorMessages,
	screenFloorRepairCodes,
	screenFloorViewFailures,

////////////////////////////////////////////////////////
//		Meter Card Items
////////////////////////////////////////////////////////

	screenMeterCard,

////////////////////////////////////////////////////////
//		Currency Card Items
////////////////////////////////////////////////////////

	screenCurrencyCard,

////////////////////////////////////////////////////////
//		Player Card Items
////////////////////////////////////////////////////////
	
	screenPlayerBadCard,

	//player screen used to show one line message
	screenPlayer1Field,
	
	//player screen used to show 4 field message
	screenPlayer4Field,

	//player accoutn balance screen
	screenPlayerAccountBalance,

	//PB Menu
	screenPlayerPBPin,
	screenPlayerPBMain,
	screenPlayerPBBalance,
	screenPlayerPBToGame,
	screenPlayerPBTransferToGame,
	screenPlayerPBTransferToGameOther,
	screenPlayerPBFromGame, //DISPLAY_TIME
	screenPlayerPBFromGameCashPromo,
	screenPlayerPBCanceled, //DISPLAY_TIME
	screenPlayerPBNotAvailable, //DISPLAY_TIME
	screenPopUp, //DISPLAY_TIME

	//player screen used to show one line message without PB
	screenPlayer1FieldWOPB,
	
	//player screen used to show 4 field message without PB
	screenPlayer4FieldWOPB,

	screenPlayerCardedSessionInProgress,

	//player screen used to show one line message for guest
	screenPlayer1FieldGuest,
	//player screen used to show 4 field message for guest
	screenPlayer4FieldGuest,

	screenTempDebugLog,

	screenMechanicEGMChange,

	screenPlayerPBCashToGame,
	screenPlayerPBPointsToGame,
	screenPlayerPBPromoToGame,
	screenPlayerPBCashFromGame,

	screenPlayerPBOtherCashToGame,
	screenPlayerPBOtherPointsToGame,
	screenPlayerPBOtherPromoToGame,
	screenPlayerPBOtherCashFromGame,

	screenPlayerPBTransferAnimation,
	screenPlayerPBLogPopUp,
	screenPlayerPBCompleted,
	screenPlayerPendingPBLogPopUp,
	screenPlayerPBTransferFromGame,

	screenMechanicPinPopUp,
	screenGlobalPinPopUp,
	screenFloorPinPopUp,

	screenMechanicEGMDownloadPending,
	screenMechanicEGMDownload,

	screenPlayerWelcome,
	screenPlayerCongratulations,
	screenPlayerAnniversary,
	screenPlayerBirthday,
	screenPlayerBirthdayAndAnniversary,

	screenPlayerPinPopUp,
	
	screenSystemBusy,
	screenMechanicGlobalOptions,
	screenMechanicHopperFill,
	screenMechanicBatteryStatus,

	screenPlayerPersonalBankerInProgress,
	screenPlayerGoodCardXXX, // No longer used.
	screenPlayerTotalDisplay,

////////////////////////////////////////////////////////
//		Splashdown Countdown event Items
////////////////////////////////////////////////////////
	
	screenSplashdownStart,
	screenSplashdownTimer,
	screenSplashdownAward,
	screenSplashdownEnd,
	screenSplashdownWinnerEnd,
	
	//view loss Limit
	screenMechanicViewLossLimit,

	screenLostEGMCommsXXX, // Loss limit only. No longer used.

////////////////////////////////////////////////////////
//		Ricochet Rewards event Items
////////////////////////////////////////////////////////
	
	screenRicochetStart,
	screenRicochetCelebration,

////////////////////////////////////////////////////////
//		Jackpot Announcer event Screens
////////////////////////////////////////////////////////
	screenJackpotAnnouncerCarded,
	screenJackpotAnnouncerNonCarded,

	screenYesNo,

	screenJackpotReset,
	
	screenTriggerlessAnnouncer,

	// Media Window setup screens
	screenMediaWindowSettings,
	screenContentWindow,
	screenTaskBar,
	screenGlobalPersistentMemClear,

	screenGlobalVolume,

	screenFullEGM,
	screenGlobalTouchScreenProtocolShow,
	screenGlobalTouchScreenProtocolSelect,
	screenMechanicMediaWindowLog,
	screenMechanicViewMoreDiagnostics,
	screenGlobalVGASamplingPhase,
	screenGlobalVideoModeSelect,
	screenGlobalVideoInputModeConfiguration,
	screenGlobalVideoOffsetsSelect,
	screenGlobalTouchScreenRtsCtsConfiguration,
	screenGlobalVideoConfiguration,
	screenGlobalVideoRotation,
	screenGlobalVideoOutputModeConfiguration,
	screenMechanicFirmwareUpgrade,
	screenTriggerlessAnnouncerNonCarded,

	//Casino Challenge screens
	screenCasinoChallengeStart,
	screenThermometer1,
	screenThermometer2,
	screenThermometer3,
	screenCasinoChallengeAward1,
	screenCasinoChallengeAward2,
	screenCasinoChallengeAward3,
	screenScheduleEnd,
	screenCasinoChallengeFiveMinuteWarningEnd,
	screenGlobalEGMDisplay,
	screenGlobalPlayerState,
	screenGlobalAutoClose,
	screenProximity,
	screenCardInEligibility,
	
	screenBannedFromPlayMessage,

	screenPBAcknowledgePointsUnavailable,

	// screens for Credit Transfer feature
	screenPlayerPBMainWithCreditTransfer,
	screenPlayerPBCreditTransfer,
	screenPlayerPBCreditTransferSelectRequestAmount,
	screenPlayerPBCreditTransferFeeVerification,
	screenPlayerPBCreditTransferSelectRequestAmountOther,
	screenPlayerPBCreditTransferAnimation,

	// Proceed with Reboot Popup
	screenShutdown,

	// screens for Floor Wide Mystery Bonus
	screenEventTriggered,
	screenNonWinnerMessage,

	// Screens for (Paltronics) Progressive.
	screenProgressiveConfig,
	screenOneLinkConnectionInfo,
	screenProgressiveHostList,
	screenProgressiveHostAddresses,

	// Facelift screens.
	screenGlobalPlayerStateClose,
	screenGlobalPlayerStateOpen,
	screenAttractorBackground,
	screenThirdParty,

	// screens specific for ServiceWindow
	screenPlayerPBHomeWithToGame = 1000,
	screenPlayerPBHomeWithFromGame,
	screenPlayerPBHomeWithBalance,
	screenPlayerPBToGameCash,
	screenPlayerPBToGamePoints,
	screenPlayerPBToGamePromo,
	screenPlayerPBToGameCashPoints,
	screenPlayerPBToGameCashPromo,
	screenPlayerPBToGamePointsPromo,
	screenPlayerPBFromGameCash,
	screenPlayerPBFromGamePromo,
	screenPBTPopup,
	screenPBTBackExit,
	screenPBTAvailBalance,
	screenPlayerPBTNone,	// used for removing all buttons

	screenResortMenu,

	// Need to refresh the state of the service window (usually after open/close of service window).
	screenServiceWindowRefresh,

    screenCardInPromotion,

	screenPlayerMessageAnnouncement,

	// Credit Transfer Media Window specific screens
	screenPlayerPBCreditTransfersWithFromToGame,
	screenPlayerPBCreditTransfersWithFromGame,
	screenPlayerPBCreditTransfersWithToGame,
	screenPlayerPBCreditTransfersWithOutGame,
	screenCustom,
	screenAccount,
	screenAccountAccess,
	screenCustom1Menu,
	screenCustom2Menu,
	screenWinnerPick,
	screenWinnerPicked,
	screenClaimableAward,
	screenUJPPinEntry,
	screenUJPPinVerify,
	screenUJPCardedTeaser,
	screenUJPUncardedTeaser,
	screenUJPSuccess,
	screenUJPError,
	screenAttractorScreenUpdate,
	screenInvitedGroupsAnticipation,
	screenInvitedGroupsAward,
	screenInvitedGroupsEnd,
	screenInvitedGroupsResults,
	screenInvitedGroupsLevelUp,
	screenInvitedGroupsLeaderboard,

	screenGenericExternal,

	screenAccessPlayer,

	screenEmployee,

	screenPlayerSelection,

	screenMobile,

	screenProgressiveOffer,
	screenPromoGame,
	screenProgressivePrize,
	screenProgressivePrizeOdometer,
	screenProgressiveOdometer,
	screenPteOdometer,
	screenPlayXGetYOdometer,
	screenScatterCountdownTimer,
		
	screenPlayerPBNewPinMessageOkay,

	screenPlayerPBEnterNewPin,
	screenPlayerPBReenterNewPin,

	screenEgmAwardFailedOkay,
	screenUJP,

	screenAutoCardout,

	screenExtendedBuckets,
	screenNone, //Keep below all other screens

};


//Mechanic card Log Screen text
extern const std::string TXT_MECHANIC_DATE_TIME;
extern const std::string TXT_MECHANIC_AMOUNT;
extern const std::string TXT_MECHANIC_STATUS;
extern const std::string TXT_MECHANIC_VALIDATION;
extern const std::string TXT_MECHANIC_ERROR_CODE;
extern const std::string TXT_MECHANIC_SEQUENCE;
extern const std::string TXT_MECHANIC_SOURCE;
extern const std::string TXT_MECHANIC_TYPE;
extern const std::string TXT_MECHANIC_RESULT;
extern const std::string TXT_MECHANIC_TRANS_ID;
extern const std::string TXT_MECHANIC_CODE;
extern const std::string TXT_MECHANIC_EGM_DISPLAY;
extern const std::string TXT_MECHANIC_OCR;
extern const std::string TXT_MECHANIC_MWCODE_TASK_BAR;
extern const std::string TXT_MECHANIC_MWCODE_BANNER_MWLEFT;
extern const std::string TXT_MECHANIC_MWCODE_BANNER_MWRIGHT;
extern const std::string TXT_MECHANIC_MWCODE_MWBOTTOM;
extern const std::string TXT_MECHANIC_MWCODE_EGMFULL;
extern const std::string TXT_NA;

/////////////////////////////////////
//     Mechanic Card Text Items
/////////////////////////////////////

extern const std::string TXT_MECHANIC_CARD;
extern const std::string TXT_MECHANIC_GLOBAL_OPTIONS;
extern const std::string TXT_MECHANIC_SENTINEL_OPTIONS;
extern const std::string TXT_MECHANIC_BATTERY_STATUS;
extern const std::string TXT_MECHANIC_BATTERY_UKNOWN;
extern const std::string TXT_MECHANIC_BATTERY_OKAY;
extern const std::string TXT_MECHANIC_BATTERY_LOW;
extern const std::string TXT_MECHANIC_BATTERY_NO;
extern const std::string TXT_MECHANIC_EGM_DOWNLOAD;

extern const std::string TXT_MECHANIC_VIEW;

//Mechanic card versions screen
extern const std::string TXT_MECHANIC_VERSIONS;
extern const std::string TXT_MECHANIC_OS_VERSION;
extern const std::string TXT_MECHANIC_SAS_VERSION;

//Mechanic card checksums screen
extern const std::string TXT_MECHANIC_OS_APPS_CHECKSUM;
extern const std::string TXT_MECHANIC_BOOT_LOADER_CHECKSUM;

//Mechanic card network information screen
extern const std::string TXT_MECHANIC_NETWORK_INFORMATION;
extern const std::string TXT_MECHANIC_HOST_NAME1;
extern const std::string TXT_MECHANIC_DHCP_STATUS;
extern const std::string TXT_MECHANIC_IP_ADDRESS;
extern const std::string TXT_MECHANIC_SUBNET_MASK;
extern const std::string TXT_MECHANIC_DEFAULT_GATEWAY;
extern const std::string TXT_MECHANIC_PREFERRED_DNS1;
extern const std::string TXT_MECHANIC_ALTERNATE_DNS1;
extern const std::string TXT_MECHANIC_MAC_ADDRESS;

//Mechanic card view meters screen
extern const std::string TXT_MECHANIC_VIEW_METERS;

//Mechanic card EGM meters screen
extern const std::string TXT_MECHANIC_EGM_METERS;

//Mechanic card Pins screen
extern const std::string TXT_MECHANIC_VIEW_PINS;
extern const std::string TXT_MECHANIC_PIN1;
extern const std::string TXT_MECHANIC_PIN2;
extern const std::string TXT_MECHANIC_PIN3;
extern const std::string TXT_MECHANIC_PIN4;
extern const std::string TXT_MECHANIC_PIN5;
extern const std::string TXT_MECHANIC_PIN6;
extern const std::string TXT_MECHANIC_PIN7;
extern const std::string TXT_MECHANIC_PIN8;

//Mechanic card view recent transactions screen
extern const std::string TXT_MECHANIC_VIEW_RECENT_TRANS;

//Mechanic card attendant paid payouts screen
extern const std::string TXT_MECHANIC_ATTENDANT_PAID;
extern const std::string TXT_MECHANIC_ENTER_ATTENDANT_PAID;
extern const std::string TXT_MECHANIC_HANDPAY;
extern const std::string TXT_MECHANIC_CC;
extern const std::string TXT_MECHANIC_TOP_AWARD;

extern const std::string TXT_MECHANIC_SLOTMAST;

//Mechanic card voucher log screen
extern const std::string TXT_MECHANIC_VOUCHER_LOG;

//Mechanic card PersonalBanker Transactions screen
extern const std::string TXT_MECHANIC_PB_TRANS;

//Mechanic card PersonalBanker Log screen
extern const std::string TXT_MECHANIC_PB_LOG;

//Mechanic card Transaction Log screen
extern const std::string TXT_MECHANIC_TRANSACTION_LOG;

//Mechanic card Composite Tilt Log screen
extern const std::string TXT_MECHANIC_TILT_LOG;

//Mechanic card Repair Log screen
extern const std::string TXT_MECHANIC_REPAIR_LOG;

//Mechanic card Media Window Log screen
extern const std::string TXT_MECHANIC_MEDIA_WINDOW_STATUS_LOG;

//Mechanic card Diagnostics screen
extern const std::string TXT_MECHANIC_DIAGNOSTICS;

//Mechanic card More Diagnostics screen
extern const std::string TXT_MECHANIC_MORE_DIAGNOSTICS;

//Mechanic card PersonalBanker Pending screen
extern const std::string TXT_MECHANIC_PB_PENDING;

//Mechanic card Comm Status screen
extern const std::string TXT_MECHANIC_COMM_STATUS;
extern const std::string TXT_MECHANIC_COMM_POLLER;
extern const std::string TXT_MECHANIC_COMM_EGM;
extern const std::string TXT_MECHANIC_COMM_TCPIP;
extern const std::string TXT_MECHANIC_COMM_ERRORS;
extern const std::string TXT_MECHANIC_COMM_MMT;
extern const std::string TXT_MECHANIC_COMM_MMT_NA;
extern const std::string TXT_MECHANIC_COMM_PMT;
extern const std::string TXT_MECHANIC_COMM_PMT_NA;
extern const std::string TXT_MECHANIC_COMM_CONNECTED;
extern const std::string TXT_MECHANIC_COMM_NOT_CONNECTED;
extern const std::string TXT_MECHANIC_COMM_VOUCHER_SERVICE;
extern const std::string TXT_MECHANIC_COMM_VOUCHER_SERVICE_NA;
extern const std::string TXT_MECHANIC_DISPLAY_YELLOW_COMM_DOWN ;
extern const std::string TXT_MECHANIC_DISPLAY_RED_COMM_DOWN;

//Mechanic card View Disk Space screen
extern const std::string TXT_MECHANIC_DISK_SPACE;
extern const std::string TXT_MECHANIC_DRIVE;
extern const std::string TXT_MECHANIC_TOTAL_SIZE;
extern const std::string TXT_MECHANIC_FREE;

//Mechanic card Flash Download Status screen
extern const std::string TXT_MECHANIC_FLASH_STATUS;
extern const std::string TXT_MECHANIC_DOWNLOAD_STATUS;
extern const std::string TXT_MECHANIC_DOWNLOAD_CHECKSUM;
extern const std::string TXT_MECHANIC_DOWNLOADED;
extern const std::string TXT_MECHANIC_DOWNLOAD_TYPE;
extern const std::string TXT_MECHANIC_DOWNLOAD_NK;
extern const std::string TXT_MECHANIC_DOWNLOAD_DEFAULTMEDIA;
extern const std::string TXT_MECHANIC_DOWNLOAD_SYSTEM;

//Mechanic card View Voucher Limits screen
extern const std::string TXT_MECHANIC_VOUCHER_LIMITS;
extern const std::string TXT_MECHANIC_VOUCHER_BUFFERING;
extern const std::string TXT_MECHANIC_TOTAL_VOUCHERS;
extern const std::string TXT_MECHANIC_DISABLED;
extern const std::string TXT_MECHANIC_ENABLED;

extern const std::string TEXT_SIZE_TEXT;

//Mechanic card Test Lockout screen
extern const std::string TXT_MECHANIC_TEST_LOCKOUT;

//Mechanic card Denomination screen
extern const std::string TXT_MECHANIC_DENOMINATION;
extern const std::string TXT_MECHANIC_METER_CALC;
extern const std::string TXT_MECHANIC_PB;
extern const std::string TXT_MECHANIC_EGM;

//Mechanic card Loss Limit screen
extern const std::string TXT_MECHANIC_LOSS_LIMIT;
extern const std::string TXT_MECHANIC_AT_EGM;
extern const std::string TXT_MECHANIC_SESSION_NUM;
extern const std::string TXT_MECHANIC_BILL_OFF;

//Mechanic card Send Messages screen
extern const std::string TXT_MECHANIC_SEND_MESSAGES;

//Mechanic card Send Repair Codes screen
extern const std::string TXT_MECHANIC_SEND_REPAIR;
extern const std::string TXT_MECHANIC_SENDING_MESSAGE;
extern const std::string TXT_MECHANIC_SEND_MAINTENANCE_CODE;

//Mechanic card Capture Meters screen
extern const std::string TXT_MECHANIC_REBOOT_IN_PROGRESS;
extern const std::string TXT_MECHANIC_CAPTURED_SUB_GAMES;
extern const std::string TXT_FINISHED_METERS;
extern const std::string TXT_FINISHED_METERS_SUB;
extern const std::string TXT_FAILED_METERS;
extern const std::string TXT_MECHANIC_RAMCLEAR_YESNO_1;
extern const std::string TXT_MECHANIC_RAMCLEAR_YESNO_2;

//Mechanic card FPGA Upgrade text
extern const std::string TXT_MECHANIC_FIRMWARE_UPGRADE;
extern const std::string TXT_MECHANIC_FPGA_UPGRADE_YESNO_1;
extern const std::string TXT_MECHANIC_FPGA_UPGRADE_YESNO_2;
extern const std::string TXT_MECHANIC_FPGA_UPGRADE_YESNO_3;
extern const std::string TXT_MECHANIC_FPGA_UPGRADE_TEXT_1;
extern const std::string TXT_MECHANIC_FPGA_UPGRADE_TEXT_2;
extern const std::string TXT_MECHANIC_FPGA_UPGRADE_TEXT_3;

/////////////////////////////////////
//     Global Card Text Items
/////////////////////////////////////
extern const std::string TXT_GLOBAL_CARD;

//Global card Calibration Settings screen
extern const std::string TXT_GLOBAL_CALIBRATION;

//Global card Calibration screen
extern const std::string TXT_GLOBAL_CALIBRATING;
extern const std::string TXT_GLOBAL_CALIBRATING_TEXT1;

//Global card Touch Screen Protocol current settings screen
extern const std::string TXT_GLOBAL_TOUCH_SCREEN_PROTOCOL;
extern const std::string TXT_GLOBAL_SELECT_TOUCH_SCREEN_PROTOCOL;
extern const std::string TXT_GLOBAL_PROTOCOL_NAME;
extern const std::string TXT_GLOBAL_VENDOR_ID;
extern const std::string TXT_GLOBAL_BAUD;
extern const std::string TXT_GLOBAL_USB_STATUS;
extern const std::string TXT_GLOBAL_DETECTED;
extern const std::string TXT_GLOBAL_NOT_DETECTED;

//Global card Touch Screen RTS / CTS Configuration screen
extern const std::string TXT_GLOBAL_TOUCH_SCREEN_RTS_CTS_CONFIGURATION;
extern const std::string TXT_GLOBAL_HANDSHAKING_DISABLED;
extern const std::string TXT_GLOBAL_HANDSHAKING_ENABLED;
extern const std::string TXT_GLOBAL_RTS_SET_TO_MATCH_CTS;
extern const std::string TXT_GLOBAL_RTS_ALWAYS_OFF;
extern const std::string TXT_GLOBAL_RTS_ALWAYS_ON;

//Global card Set VGA Sampling Phase screen
extern const std::string TXT_GLOBAL_VGA_SAMPLING_PHASE;
extern const std::string TXT_GLOBAL_PHASE;
extern const std::string TXT_GLOBAL_PHASE_NA;

//Global card Video Configuration screen
extern const std::string TXT_GLOBAL_VIDEO_CONFIGURATION;

//Global card Video Mode Configuration screen
extern const std::string TXT_GLOBAL_VIDEO_INPUT_CONFIGURATION;
extern const std::string TXT_GLOBAL_VIDEO_OUTPUT_CONFIGURATION;
extern const std::string TXT_GLOBAL_VIDEO_SIGNAL_TYPE;
extern const std::string TXT_GLOBAL_VIDEO_SIGNAL_AUTO;
extern const std::string TXT_GLOBAL_VIDEO_SIGNAL_ANALOG;
extern const std::string TXT_GLOBAL_VIDEO_SIGNAL_DIGITAL;
extern const std::string TXT_GLOBAL_VIDEO_MODE_CONFIG;
extern const std::string TXT_GLOBAL_VIDEO_OUTPUT_MODE_AUTO_CONFIG_TRUE;
extern const std::string TXT_GLOBAL_VIDEO_OUTPUT_MODE_AUTO_CONFIG_FALSE;
extern const std::string TXT_GLOBAL_VIDEO_OUTPUT_MODE_NATIVE;
extern const std::string TXT_GLOBAL_SAVE_VIDEO_CONFIG_YESNO_1;
extern const std::string TXT_GLOBAL_SAVE_VIDEO_CONFIG_YESNO_2;
extern const std::string TXT_GLOBAL_SAVE_VIDEO_ROTATION_YESNO_1;
extern const std::string TXT_GLOBAL_SAVE_VIDEO_ROTATION_YESNO_2;
extern const std::string TXT_GLOBAL_EGM_DISPLAY;
extern const std::string TXT_GLOBAL_AR_HEAD;
extern const std::string TXT_GLOBAL_ASPECT_RATIO;
extern const std::string TXT_GLOBAL_SIZE_TO_FIT;
extern const std::string TXT_GLOBAL_AUTO_MEDIAWINDOW;
extern const std::string TXT_GLOBAL_AUTO_MEDIAWINDOW_MINUTES;
extern const std::string TXT_GLOBAL_AUTO_MEDIAWINDOW_MAINTAIN;

//Global card Video Mode selection screen
extern const std::string TXT_GLOBAL_SELECT_VIDEO_MODE;
extern const std::string TXT_GLOBAL_VIDEO_MODE_NUMBER;
extern const std::string TXT_GLOBAL_VIDEO_MODE_DESCRIPTION;

//Global card Video Rotation screen
extern const std::string TXT_GLOBAL_VIDEO_ROTATION;
extern const std::string TXT_GLOBAL_VIDEO_ROTATION_NO_ROTATION;
extern const std::string TXT_GLOBAL_VIDEO_ROTATION_90_DEGREES;
extern const std::string TXT_GLOBAL_VIDEO_ROTATION_180_DEGREES;
extern const std::string TXT_GLOBAL_VIDEO_ROTATION_270_DEGREES;
extern const std::string TXT_GLOBAL_CURRENT_ROTATION;

//Global PIN Settings screen
extern const std::string TXT_GLOBAL_PIN_SETTINGS;

//Global card Set Volume Backlight screen
extern const std::string TXT_GLOBAL_BACK_LEVEL;
extern const std::string TXT_GLOBAL_VOL_LEVEL;
extern const std::string TXT_GLOBAL_VOL;
extern const std::string TXT_GLOBAL_BACK;

//Global card Service Window screens
extern const std::string TXT_GLOBAL_SERVICE_WINDOW_SETTINGS;

//Global Card NVRAM clear screens
extern const std::string TXT_GLOBAL_CLEAR_NVRAM;
extern const std::string TXT_GLOBAL_CLEAR_EEPROM;
extern const std::string TXT_GLOBAL_CLEAR_HARDWARE;
extern const std::string TXT_GLOBAL_CLEAR_ALL;

//Global Card Video Offsets screens
extern const std::string TXT_GLOBAL_VIDEO_OFFSETS;

/////////////////////////////////////
//     Player Card Text Items
/////////////////////////////////////
extern const std::string TXT_OPEN;
extern const std::string TXT_CLOSED;
extern const std::string TXT_UNKNOWN_ERROR;
extern const std::string TXT_CALCULATING_CHECKSUMS;
extern const std::string TXT_REBOOT_IN_PROGRESS;
extern const std::string TXT_REBOOTED;
extern const std::string TXT_MEMORY_ERROR;
extern const std::string TXT_MEMORY_OK;
extern const std::string TXT_METER_CARD;
extern const std::string TXT_COLON_SPACE;
extern const std::string TXT_PLAYER_TIER_POINTS;
extern const std::string TXT_GLOBAL_LEFT;
extern const std::string TXT_GLOBAL_RIGHT;
extern const std::string TXT_GLOBAL_BOTTOM;
extern const std::string TXT_GLOBAL_SIZE;
extern const std::string TXT_PLAYER_CASH_FROM_GAME;
extern const std::string TXT_PLAYER_POINTS_FROM_GAME;
extern const std::string TXT_PLAYER_PARTIAL_CASH_FROM_GAME;
extern const std::string TXT_PLAYER_PARTIAL_POINTS_FROM_GAME;
extern const std::string TXT_PLAYER_CASHOUT_CASH_FROM_GAME;
extern const std::string TXT_PLAYER_CASHOUT_POINTS_FROM_GAME;
extern const std::string TXT_PLAYER_CASHOUT_PROMOS_FROM_GAME;
extern const std::string TXT_PLAYER_CASH_TO_GAME;
extern const std::string TXT_PLAYER_POINTS_TO_GAME;
extern const std::string TXT_PLAYER_PROMO_TO_GAME;
extern const std::string TXT_PLAYER_PROMO_FROM_GAME;
extern const std::string TXT_CREDIT_TRANSFER;
extern const std::string TXT_METER_NAMES[NUM_METERS + 1];
extern const std::string TXT_GLOBAL_OPTIONS[];
