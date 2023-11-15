#pragma once
#include <stdafx.h>
#define NUM_METERS 91

//The certificate we used to make nCompass https compatible were created 05/30/2019 19:54:20.  
//If current system is older than Wed, 07 Sep 2021 00:00:00 GMT, we now set current system time to Wed, 07 Sep 2021 00:00:00 GMT.
const time_t LOCAL_CERT_TIME_ACTIVATION(1630972800);//Date and time(GMT) : Wed, 07 Sep 2021 00:00:00 GMT
const DWORD TIME_ZONE_OFFSET(42110);
const DWORD REMOTE_DEBUGGING_ENABLED_OFFSET(710693);
const DWORD SYSTEM_TIME_SPRINKLE_OFFSET(710716);
const DWORD SYSTEM_TIME_OFFSET(710720);
const BYTE INVALID_TIME(0xff);
extern const int ONE_HUNDRED_PERCENT;
extern const int SIZE_OF_PB_LOGS;
extern const int SIZE_OF_LOGS;
extern const int SIZE_OF_IPS;
extern const int WEBSOCKET_MAX_LENGTH;
#define HTTP_STATUS_OK 200
#define SIZE_OF_NAME_IP 30
#define SIZE_OF_LABEL 30
#define SIZE_OF_MESSAGES 127
#define SIZE_OF_CASINO_NAME 40
#define SIZE_OF_CASINO_ADDRESS1 41
#define SIZE_OF_CASINO_ADDRESS2 41
#define NUMBER_OF_VALID_CARDS_PREFIX_IN_NVRAM 33
#define SIZE_OF_VALID_PROP_CODE 2
#define SIZE_OF_EMPLOYEE_PASSWORD 20
#define PROGRESS_BAR_DEVISOR 2
#define PROGRESS_BAR_MIN 1
#define PROGRESS_BAR_MAX 51
#define PROGRESS_BAR_RANGE 46 // Matches media.xml layout.  2011-05-03
extern const int MAXIMUM_DISPLAY_DURATION;//5 minutes
extern const int NRICH_AWARD_PERCENTAGE_THRESHOLD;
extern const int INVALID_AWARD_AMOUNT;
extern const long INVALID_SLOT_MAST_ID;
extern const long INVALID_SLOT_NUMBER;
extern const int SIZE_OF_FILE_NAME;
#define SIZE_OF_MAC_ADDRESS 6
extern const BYTE POLLER_PACKET_VERSION;
extern const std::string XML_FILE_NAME;
extern const std::string STORAGE_CARD_1;
extern const std::string STORAGE_CARD_2;
extern const std::string SERVICE_WINDOW_LOCATION;

extern const LPCTSTR PLAYLIST_FILENAME;
#define DEF_ERROR_FILE_NAME "/Error.txt"
#define DEF_BADPOLLERDATA_FILE_NAME "/BADPOLLERDATA.sdf"
extern const LPCTSTR ERROR_FILE_NAME;
extern const WORD INVALID_GAME_NUMBER;
extern const DWORD BYTES_PER_KB;
extern const DWORD KBS_PER_MB;
extern const DWORD BYTES_PER_MEG;
extern const DWORD ONE_DOLLAR_IN_CENTS;
extern const COLORREF POLLER_STATUS_COLOR;
extern const COLORREF EGM_STATUS_COLOR;
extern const COLORREF DEFAULT_COLOR;
extern const WORD INVALID_DENOM;
extern const int TimeDriftThresholdSeconds;	//the threshold in seconds before applying a time change
extern const std::string BADREAD_SOUND_FILENAME;
extern const std::string UNMOUNT_USB_SOUND_FILENAME;
extern const LPCTSTR OS_IMAGE_VERSION_FILENAME;
extern const LPCTSTR OS_NEEDED_SERVICES_FILENAME;
extern const LPCTSTR FLV_EXTENSION;
extern const LPCTSTR BMP_EXTENSION;
extern const LPCTSTR SMD_EXTENSION;

extern const DWORD ATTRACTOR_QUEUE_STAVE_EXPIRE;

extern const UINT S7_RETRIES;

extern const DWORD PBT_AUTO_UPLOAD_PIN;

extern const int RADIX_DECIMAL; // Base 10.
extern const int NUMBER_OF_PLACES_AFTER_DECIMAL_POINT;

extern const SYSTEMTIME DEFAULT_SYSTEMTIME;
extern const int SESSION_ID_DEFAULT;
extern const WORD DEFAULT_GAME_LIST_CHUNK_SIZE;


// Define time constants so they can be used as either signed or unsigned values.
#define FILETIME_UNITS_PER_MILLISECOND (10 * 1000)
#define FILETIME_UNITS_PER_SECOND (FILETIME_UNITS_PER_MILLISECOND * MILLISECONDS_PER_SECOND)
#define MILLISECONDS_PER_HUNDRETH_SECOND (10)
#define TICK_COUNTS_PER_SECOND (1000)
#define MILLISECONDS_PER_SECOND (1000)
#define SECONDS_PER_MINUTE (60)
#define MINUTES_PER_HOUR (60)
#define HOURS_PER_DAY (24)
#define MILLISECONDS_PER_MINUTE (SECONDS_PER_MINUTE * MILLISECONDS_PER_SECOND)
#define MILLISECONDS_PER_HOUR (MINUTES_PER_HOUR * SECONDS_PER_MINUTE * MILLISECONDS_PER_SECOND)
#define MILLISECONDS_PER_DAY (HOURS_PER_DAY * MINUTES_PER_HOUR * SECONDS_PER_MINUTE * MILLISECONDS_PER_SECOND)
#define SECONDS_PER_HOUR (MINUTES_PER_HOUR * SECONDS_PER_MINUTE)
#define SECONDS_PER_DAY (HOURS_PER_DAY * SECONDS_PER_HOUR)
#define SIXTY_MINUTES_IN_MS (60 * MILLISECONDS_PER_MINUTE)
#define CARD_ID_LEN 5
#define MAX_PBID_LENGTH 20
#define IPADDRESS_STR_LENGTH 16
#define PB_FIXED_DATE_LEN 77

extern const int MICROSOFT_RAND_MAX;

//Hard Pins
#define NUM_HARD_PINS 8
extern const DWORD DEFAULT_NDI_SAMPLE_RATE;
#define NDI_BUFFER_SIZE 256
extern const DWORD PIN_DEBOUNCE_TOLLERANCE;

//Access Security / Chassis Door
extern const int CHASSIS_DOOR_WAIT_TIMEOUT;

//Network adapter
extern const int NUM_OF_DNS;
extern const int SPACE_TO_LEAVE_FOR_EXTRA_TERMINATOR;
extern const char NETWORK_ADAPTER_NAME_CHAR[];
extern const int CONFIG_REQUEST_TIMEOUT;

extern const int GLOBAL_CARD_PASSWORD_SIZE;
extern const int MEMCMP_EQUAL; // Return value for memcmp() indicating equality.
#define IP_ADDRESS_SIZE 4

#define SIZE_OF_READ_BUFFER 16384

#define HOST_PORT_DEFAULT 3027
#define NUDGE_PORT_DEFAULT 10551
#define ANNOUNCEMENT_PORT_DEFAULT 10550

extern const __int64 DOOR_CLOSED_METER_VALUE;
extern const __int64 DOOR_OPEN_METER_VALUE;

//Screen Calibration Stuff
#define EEPROM_CALIBRATION_WORDS	10
//#define EPR_IOCTL_STORE_TOUCHREGISTRY 0x10 //IOCTL call to store the touch calibration.
#define EEPROM_CALIBRATION_BYTES	(EEPROM_CALIBRATION_WORDS*2 + 4)
#define EEPROM_CALIBRATION_OFFSET	0

extern const WORD NVRAM_MEMORY_BAD;
extern const WORD NVRAM_MEMORY_OK;

//Percentage that the Poller transaction resend list should be allowed
//to fill before telling internals to slow down
extern const int RESEND_LIST_FILLING_THESHOLD;
extern const int RESEND_LIST_FULL_THRESHOLD;

// Disk space display parameters.
#define DRIVE_LETTER_MIN L'A' // Want a constant expression, not a "constant".
#define DRIVE_LETTER_MAX L'Z' // Want a constant expression, not a "constant".

#define SIZE_OF_TOUCH_SCREEN_PROTOCOL_NAME 31

//Watchdog Timeouts
extern const int DUMP_MEMORY_THREAD_TIMEOUT;
extern const int GCF_TO_INTERNALS_QUEUE_TIMEOUT;
extern const int INTERNALS_TO_GCF_QUEUE_TIMEOUT;
extern const int UI_TO_INTERNALS_QUEUE_TIMEOUT;
extern const int INTERNALS_TO_UI_QUEUE_TIMEOUT;
extern const int SCF_TO_INTERNALS_QUEUE_TIMEOUT;
extern const int INTERNALS_TO_SCF_QUEUE_TIMEOUT;
extern const int PROGRESSIVE_TO_INTERNALS_QUEUE_TIMEOUT;
extern const int ON_MESSAGE_TIMEOUT;
extern const int INTERNALS_TO_PROGRESSIVE_QUEUE_TIMEOUT;
extern const int THIRD_PARTY_LISTENING_QUEUE_TIMEOUT;
extern const int SCF_MAIN_THREAD_TIMEOUT;
extern const int UI_MAIN_THREAD_TIMEOUT;
extern const int GCF_MAIN_THREAD_TIMEOUT;
extern const int INTERNALS_MAIN_THREAD_TIMEOUT;
extern const int SCF_RESEND_LIST_TIMEOUT;
extern const int SAS_POLLING_LOOP_TIMEOUT;
extern const int SAS_SCHEDULER_TIMEOUT;
extern const int SEND_TO_EGM_THREAD_TIMEOUT;
extern const int SEND_TO_POLLER_THREAD_TIMEOUT;
extern const int SERVICE_WINDOW_LISTENER_THREAD_TIMEOUT;
extern const int READ_FROM_POLLER_THREAD_TIMEOUT;
extern const int CARD_READER_TIMEOUT;
extern const int NDI_THREAD_TIMEOUT;
extern const int APP_MAIN_THREAD_TIMEOUT;
extern const int INTERNALS_PROGRESS_CACHE_TIMEOUT;
extern const int KEEP_TIME_THREAD_TIMEOUT;
extern const int UPDATE_MIXER_TIMEOUT;
extern const int FILE_DOWNLOADER_TIMEOUT;

const int PROGRESSIVE_PLAYLIST_CONTROLLER_THREAD_TIMEOUT = 240;
const int THREAD_PROGRESSIVE_CONTENT_FILES_THREAD_TIMEOUT = 240;

extern const int EVENT_WAIT_TIMEOUT;
extern const DWORD USER_EVENT_DELAY_MILLISECONDS;
extern const DWORD SLEEP_YIELD_MILLISECONDS; // Sleep period to yield to other threads (of same priority).
extern const DWORD SHUTDOWN_LOOP_SLEEP_PERIOD; // Millisecond Sleep() period while waiting for other threads during shutdown.

extern const DWORD FULL_CLEAN_CODE;
extern const DWORD PARTIAL_CLEAN_CODE;

extern const DWORD WATCHDOG_CHECKIN;

extern const int WAIT_FOR_UI_TO_BE_READY;

extern const bool DEFAULT_ONE_LINK_PROGRESSIVES_SEN_OPTION_SUPPORTED;

// Sen Options 1 Map
extern const DWORD SO1_FILL_AMOUNT;
extern const DWORD SO1_USE_HOPPER;
extern const DWORD SO1_NOT_USED_1;
extern const DWORD SO1_DISPLAY_NUMERIC_POINT_TOTOAL;
extern const DWORD SO1_DISPLAY_PLAYER_NAME;
extern const DWORD SO1_DISPLAY_COUNTDOWN_AND_SESSION;
extern const DWORD SO1_DISPLAY_TOTAL_POINTS_SESSION_POINTS_AS_CASH;
extern const DWORD SO1_NOT_USED_2;
extern const DWORD SO1_PBT_CASH_ENABLE;
extern const DWORD SO1_PBT_POINSTS_ENABLE;
extern const DWORD SO1_PBT_PROMO_ENABLE;
extern const DWORD SO1_DISPLAY_COMP;
extern const DWORD SO1_PBT_POINTS_AS_PROMO;
extern const DWORD SO1_COINS_WON_ACCOUNTING;
extern const DWORD SO1_COINS_IN_ACCOUNTING;
extern const DWORD SO1_DISPLAY_CONGRATS;
extern const DWORD SO1_THEORETICAL_ACCOUNTING;
extern const DWORD SO1_COINS_OUT_ACCOUNTING;
extern const DWORD SO1_PBT_ON_CARD_IN;
extern const DWORD SO1_EMPLOYEE_PIN_REQUIRED;
extern const DWORD SO1_NOT_USED_3;
extern const DWORD SO1_PARTIAL_BONUS_AWARD;
extern const DWORD SO1_DISPLAY_POINTS_TOTAL_AS_CASH;
extern const DWORD SO1_DISPLAY_GOOD_LUCK_AND_SESSION;
extern const DWORD SO1_COUPON_PROMO_IN_ENABLED;
extern const DWORD SO1_VOUCHER_PROMO_IN_ENABLED;
extern const DWORD SO1_VIRTUAL_SESSION_ENABLED;
extern const DWORD SO1_TICKET_IN_ENABLED;
extern const DWORD SO1_NOT_USED_4;
extern const DWORD SO1_PB_FROM_GAME_ENABLED;
extern const DWORD SO1_DISPLAY_PLAYER_POINTS_TOTAL;
extern const DWORD SO1_NOT_USED_5;

// Sen Options 2 Map
extern const DWORD SO2_PBT_PIN_NOT_REQUIRED;
extern const DWORD SO2_COUPON_PROMO_OUT_ENABLED;
extern const DWORD SO2_VOUCHER_OUT_ENABLED;
extern const DWORD SO2_CEP_OUT_ENABLED;
extern const DWORD SO2_NCEP_OUT_ENABLED;
extern const DWORD SO2_DISPLAY_GOOD_LUCK_AND_SESSION_POINTS;
extern const DWORD SO2_DISPLAY_TOTAL_AND_SESSION_POINTS_NUM;
extern const DWORD SO2_DISPLAY_POINTS_TOTAL_AS_CASH_SESSION;
extern const DWORD SO2_NCEP_AUTO_UPLOAD;
extern const DWORD SO2_MINI_VIRTUAL_SESSION_ENABLED;
extern const DWORD SO2_ACCRUE_POINTS_FOR_NCEP;
extern const DWORD SO2_ACCRUE_COMP_FOR_NCEP;	
extern const DWORD SO2_ONE_LINK_PROGRESSIVES_ENABLED;
extern const DWORD SO2_RCCL_CARDS_ENABLED;
extern const DWORD SO2_UJP_ENABLED;
extern const DWORD SO2_PROBE_PBT_LOCK;
extern const DWORD SO2_MGMD_ENABLED;
extern const DWORD SO2_EXTERNAL_CREDIT_TRANSFER_VERIFICATION;
extern const DWORD SO2_MOBILE_CONNECT_COMMUNICATION_ENABLED;
extern const DWORD SO2_REMOTE_DEBUGGING_ENABLED;
extern const DWORD SO2_PB_PARTIAL_WAT_OUT_ENABLED;
extern const DWORD SO2_CASHOUT_TO_HOST_ENABLED;
extern const DWORD SO2_CEP_AUTOPLOAD;
extern const DWORD SO2_WAT_AUTOPLOAD;

// Feature Flags Map
extern const DWORD FF_ATOMIC_REDEMPTIONS;
extern const DWORD FF_JACKPOT_EXTERNAL_BONUS;
extern const DWORD FF_EXTENDED_TICKET_EXPIRATION;
extern const DWORD FF_VOUCHER_SEPARATION;
extern const DWORD FF_MULTI_GAME_MULTI_DENOM;
extern const DWORD FF_CHUNKED_GAMES_LIST;
extern const DWORD FF_WALLET_2_0_SUPPORTED;

//Memory Thresholds
extern const DWORD DEFAULT_REBOOT_ASAP_THRESHOLD_MB;
extern const DWORD DEFAULT_REBOOT_WARNING_THRESHOLD_MB;
extern const DWORD DEFAULT_REBOOT_UI_ASAP_THRESHOLD_MB;
extern const DWORD DEFAULT_REBOOT_UI_AFTER_DROP_THRESHOLD_MB;

extern const DWORD DEFAULT_RESTART_UI_AFTER_X_DAYS;
extern const DWORD DEFAULT_RESTART_UI_DELAY_HOURS;

extern const DWORD DEFAULT_SAS_READ_TIMEOUT_MULTIPLIER;

// Player Session Default Values
extern const WORD NVRAM_PLAYER_TRACKING_DIVISOR_DEFAULT;
extern const DWORD DEFAULT_MULTIPLIER;
extern const DWORD DEFAULT_COUNTDOWN;
extern const DWORD INVALID_MULTIPLIER;
extern const DWORD INVALID_DIVIDER;
extern const DWORD MAXPOINTINC;
extern const int SESSION_ID_DEFAULT;
extern const DWORD GAME_SESSION_ID_DEFAULT;
extern const DWORD FF_EXTERNAL_CREDIT_TO_GAME;
extern const DWORD DEFAULT_NUMBER_OF_SUBGAMES_MISMATCH_CHECK;
extern const DWORD DEFAULT_COUNTDOWN_DISPLAY_INTERVAL;
extern const DWORD DEFAULT_AUTO_UPLOAD_DELAY_SECONDS;

// Touch Screen Protocol

// Date Stamp Format is:
//           1
// 012345678901
// d YYYY-MM-DD
//
// Y - Year
// M - Month
// D - Day

#define TOUCH_SCREEN_PROTOCOL_DATESTAMP_LENGTH 12
#define TOUCH_SCREEN_PROTOCOL_YEAR_POSITION    2
#define TOUCH_SCREEN_PROTOCOL_MONTH_POSITION   7
#define TOUCH_SCREEN_PROTOCOL_DAY_POSITION     10

// Record Format is:
//           1         2         3         4         5         6
// 012345678901234567890123456789012345678901234567890123456789012
// t vvvvvv bbbbbbbbbb nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn
//
// t - Type
// v - Vendor ID
// b - Baud Rate
// n - Name

#define TOUCH_SCREEN_PROTOCOL_RECORD_LENGTH 62
#define TOUCH_SCREEN_PROTOCOL_TYPE_POSITION 0
#define TOUCH_SCREEN_PROTOCOL_ID_POSITION   2
#define TOUCH_SCREEN_PROTOCOL_BAUD_POSITION 9
#define TOUCH_SCREEN_PROTOCOL_NAME_POSITION 20

#define TOUCH_SCREEN_DEFAULT_VENDOR_ID  0x0596
#define TOUCH_SCREEN_DEFAULT_BAUD       2400
#define TOUCH_SCREEN_DEFAULT_BAUD_WS    9600

// Touch Screen Protocol Type -- only types USB and SERIAL are valid.
extern const byte TOUCH_SCREEN_PROTOCOL_TYPE_INVALID;
extern const byte TOUCH_SCREEN_PROTOCOL_TYPE_USB;
extern const byte TOUCH_SCREEN_PROTOCOL_TYPE_SERIAL;

extern const std::string TOUCH_SCREEN_PROTOCOL_VENDOR_ID_NOT_APPLICABLE;
extern const std::string TOUCH_SCREEN_PROTOCOL_BAUD_NOT_APPLICABLE;
extern const std::string TOUCH_SCREEN_PROTOCOL_NAME_SERIAL_DEF;
extern const std::string TOUCH_SCREEN_PROTOCOL_NAME_USB_DEF;

extern const std::string TOUCH_SCREEN_PROTOCOLS_DAT;

// Video Settings
#define AUTODETECT_VIDEOMODE 255

extern const byte VIDEO_SIGNAL_TYPE_AUTO;
extern const byte VIDEO_SIGNAL_TYPE_ANALOGUE;
extern const byte VIDEO_SIGNAL_TYPE_DIGITAL;
extern const byte VIDEO_SIGNAL_TYPE_BOTH;
extern const byte VIDEO_SIGNAL_TYPE_ERROR;

// Thread Names

// EGMProtocol Threads
extern const std::string THREAD_READ_EGM_INTERNALS_QUEUE;

// InternalsFramework Threads
extern const std::string THREAD_CALC_CHECKSUM;
extern const std::string THREAD_DUMP_MEMORY;
extern const std::string THREAD_GAT_CHECKSUM;
extern const std::string THREAD_POLICY_SERVER;
extern const std::string THREAD_READ_CARD;
extern const std::string THREAD_READ_GCF_QUEUE;
extern const std::string THREAD_READ_NDI;
extern const std::string THREAD_READ_SCF_QUEUE;
extern const std::string THREAD_READ_PROGRESSIVE_QUEUE;
extern const std::string THREAD_READ_UI_QUEUE;
extern const std::string THREAD_READ_CARD_READER;
extern const std::string THREAD_UPDATE_POLLER_WITH_SUBGAME_METERS;
extern const std::string THREAD_PROGRESSIVE_CACHE_INTERNALS;
extern const std::string THREAD_TIME_KEEPER;
extern const std::string THREAD_UPGRADE_MIXER;
extern const std::string THREAD_FILE_DOWNLOADER;

// MediaList Threads
extern const std::string THREAD_PLAY_MEDIA;

// PollerProtocol Threads
extern const std::string THREAD_READ_FROM_POLLER;
extern const std::string THREAD_POLLER_RECEIVE_FLASH;
extern const std::string THREAD_SEND_TO_POLLER;

// SASProtocol Threads
extern const std::string THREAD_SCHEDULED_SAS_TASK;
extern const std::string THREAD_SEND_TO_EGM;

// SentinelApp Threads
extern const std::string THREAD_MAIN;
extern const std::string THREAD_WATCHDOG;

// SentinelDlg Threads
extern const std::string THREAD_FILE_CLEANUP;
extern const std::string THREAD_READ_SENTINEL_INTERNALS_QUEUE;

// ServiceWindowManager Threads
extern const std::string THREAD_SERVICEWINDOW_LISTENER;

// ThirdPartyProtocol Threads
extern const std::string THREAD_THIRDPARTY_LISTENER;


// SystemProtocol Threads
extern const std::string THREAD_POLLER_INTERNALS_QUEUE;
extern const std::string THREAD_PROGRESSIVE_INTERNALS_QUEUE;

// ProgressiveProtocol Threads
extern const std::string THREAD_PROGRESSIVE_TIMED_EVENTS;
extern const std::string THREAD_PROGRESSIVE_ON_MESSAGE;
extern const std::string THREAD_PROGRESSIVE_PLAYLIST_CONTROLLER;
extern const std::string THREAD_MULTICAST_CHANNEL;
extern const std::string THREAD_PROGRESSIVE_CONTENT_FILES;


typedef enum
{
	VID_PARAMS_ARRAY_EGM_TO_SENTINEL   = 0,
	VID_PARAMS_ARRAY_SENTINEL_TO_MIXER = 1,
	VID_PARAMS_ARRAY_MIXER_TO_EGM      = 2,
	VID_PARAMS_ARRAY_SIZE              = 3,
} VideoParamsArray;

extern const USHORT DEFAULT_MIXER_PARAM_STATUS;

typedef enum  
{
	NO_EGM = 0,		//scNoSlot
	CUSTOM = 1,		//Now used to prevent GetGameList() from using B5 for progressive level info
	STANDARD = 2,	//scIGTSlot
	BALLY = 3,		//scBallySlot
	scUniversalSlot = 4,
	scSigmaSlot = 5,
	scWMSSlot = 6,
	WMS = 7,		//scWMSSlotSAS
	scUSGamesSlot = 8,
	scPMCoinSlot = 9,
	IGTVISION = 10,	//scVision
	scIGCASlot = 11,
	scAristoSlot = 12,
	scGameMaker = 13,
	scIGTWinner = 14,
	scGDAPSlot = 15,
	scBally6000 = 16,
	scAtronicFrance = 17, 
	scBessSlot = 18, 
	scUni8116Prog = 19,
	scIGTPulseNoBV = 20,
	scGamecraft = 21,
	scSASno71 = 22,  
	scAristoGamPro = 23,  
	IGTS2000 = 24,
	
} EGMType;

typedef enum 
{
	MS40,
	MS200,

} EGMPollingSpeed;

typedef enum 
{
	NO_EGM_PROTOCOL = 0,
	IGT_SAS = 1,
	BALLY_PROTOCOL = 2,
	WILLIAMS = 3,
	ARISTOCRAT = 4,
	CDS_GAMING_DEVICE_ACCOUNTING_PROTOCOL = 5,
	BALLY_EXTENDED_SIMPLE_SERIAL = 6,
	AUTOTOTE = 7,
	IGT_SAS_WITH_SLOW_POLLING = 8,
	IGT_SAS_3xx = 9,
	ARISTOCRAT_GAMPRO = 10,
	SAS5xx = 11,
	SAS6xx = 12,
} EGMProtocol;

typedef enum
{
	protocolStateEnabled,
	protocolStateDisabled,
	protocolStateDisableTransactionsFull,

} EGMProtocolState;

// Electronic transfer modes of EGM
typedef enum
{
	EGMTransferMode_PENDING,
	EGMTransferMode_AFT,           // AFT supported
	EGMTransferMode_EFT,           // EFT supported
	EGMTransferMode_NONE           // neither AFT nor EFT is supported
}EGMTransferMode;

typedef enum
{
	sentinelOffline,			// Sentinel is offline
	sentinelOfflineMove,		// Sentinel is offline because of a machine move

	sentinelComingOnline,		// Sentinel is in the process of coming online
	sentinelComingOnlineMove,	// Sentinel is in the process of completing a move

	sentinelOnline,				// Sentinel is currently online

	sentinelGoingOffline,		// Sentinel is in the process of going offline
	sentinelGoingOfflineMove,	// Sentinel is in the process of going offline becasue of a machine move
	sentinelGoingOfflineDownload,	// Sentinel is in the process of going offline becasue of a download

} SentinelOnlineState;


enum DenomCurrencyType 
{
	AUSRALIAN_DOLLARS,
	BRITISH_POUNDS,
	EUROS,
	US_DOLLARS,
};

typedef enum
{
	DISP_MSG_BAD_CARD_READ = 0,
	DISP_MSG_GOOD_CARD_READ = 1,
	DISP_MSG_CASINO_NAME = 2,
	DISP_MSG_ATTRACTOR = 3,
	DISP_MSG_CONGRATS = 4,
	DISP_MSG_BIRTHDAY = 5,
	DISP_MSG_ANNIVERSARY = 6,
	DISP_MSG_WELCOME = 7,
	DISP_MSG_ADDRESS1 = 8,
	DISP_MSG_ADDRESS2 = 9,
} DisplayMessageType;

typedef enum  
{
	LOG_ATT_PAID_PAYOUT,
	LOG_TICKET,
	LOG_PBT_LOG,
	LOG_PBT_EVENTS,
	LOG_TRANS,
	LOG_TILT,
	LOG_FAILURE,
	LOG_MEDIA_WINDOW,
	LOG_NONE,
} LogType;

typedef enum
{
	CHANGE_SUCCESS = 0,
	NO_SLOTMAST_ID = 1,
	WRONG_OASIS_ID = 2,
	PMT_DENIED = 3,
	RESPONSE_MISMATCH = 10,
	TIME_OUT = 99,
} SlotmastIDResponse;

typedef enum
{
	KE_TYPE_JACKPOT = 0,
	KE_TYPE_FILL = 1,
	KE_TYPE_PRINTER_MAINT = 16
} KeypadEntryType;

typedef enum
{
	RESEND_OK,
	RESEND_FILLING,
	RESEND_FULL,
} ResendState;

extern const WORD COMMUNICATION_TYPE_EGM;
extern const WORD COMMUNICATION_TYPE_POLLER;
extern const WORD COMMUNICATION_TYPE_MMT;
extern const WORD COMMUNICATION_TYPE_PMT;
extern const WORD COMMUNICATION_TYPE_TCP;

extern const BYTE PLAYER_PREFIX;
extern const BYTE GLOBAL_PREFIX;
extern const BYTE MECHANIC_PREFIX;
extern const BYTE FLOOR_PREFIX;
extern const BYTE TEMP_PREFIX;

extern const bool NCL_CARDS_ENABLED_DEFAULT;

extern const BYTE CARD_READER_TRACK_1;
extern const BYTE CARD_READER_TRACK_2;
extern const BYTE CARD_READER_TRACK_NUMBER_DEFAULT;

extern const std::string SUBSCRIPTION_WHERE_ALL_STRING;

#ifdef MCR_LOGGING
extern const byte LOGGING_DIAG_VALUE; // Value easily identified for diagnostic purposes.
#endif

typedef enum
{
	ccNone						= 0,
    ccMachineType				= 1001,
    ccSerialProtocol			= 1002,
    ccProgressiveReturn			= 1003,
    ccSAS3Override				= 1004,
    ccTicketPolling				= 1005,
    ccTicketdisableOverride		= 1006,
    ccPinInterface				= 1007,
    ccPinActiveState			= 1008,
    ccPinMeterType				= 1009,
    ccSlotNumber				= 1010,
    ccDenomination				= 1011,
    ccCasinoNumber				= 1012,
    ccScrollRate				= 1013,
    ccOption0					= 1014,
    ccMachineNumber				= 1015,
    ccMachineLocation			= 1016,
    ccCashDisplayIncrement		= 1017,
    ccIGTCardPrefix				= 1018,
    ccPar						= 1019,
    ccProgressiveDenomCode		= 1020,
    ccSlotMastID				= 1021,
    ccCommID					= 1022,
    ccDisplayType				= 1023,
    ccCardReaderType			= 1024,
    ccGlobalCardPrefix			= 1025,
    ccMechCardPrefix			= 1026,
    ccFloorCardPrefix			= 1027,
    ccTempCardPrefix			= 1028,
    ccTicketExpirationType		= 1029,
    ccTicketExpirationTime		= 1030,
    ccTicketEnabled				= 1031,
    ccSystemGeneratedValidationNumbers	= 1032,
    ccPBEnabledTransfers		= 1033,
    ccSendSessionDollars		= 1034,
    ccTicketBufferingEnabled	= 1035,
	ccDefaultMultiplier			= 1036,
    ccDefaultDivider			= 1037,
    ccPlayerCardTimeOut			= 1038,
    ccVirtualNoPlayTimeout		= 1039,
    ccVirtualSessionDuration	= 1040,
    ccOption1					= 1041,
    ccGlobalOptions				= 1042,
	ccSlotMastRev				= 1043,
	ccA8Enabled					= 1044,
	ccAccPointsForNCEP			= 1045,
	ccAccCompForNCEP			= 1046,
	ccUniverseEnabled			= 1047,
	ccMixedMode					= 1048,
	ccFeatureFlags              = 1049,
	ccNCLEnabled				= 1050,
	ccCardTrack					= 1051,
	ccPlayerProfileButton		= 1052,
	ccGlobalMGMD				= 1053,
	ccExcessiveVoucherOutEnabled		= 1054,
	ccExcessiveVoucherOutThreshold = 1055,
	ccTotalBillInThreshold		= 1056,
	ccCoinInThreshold			= 1057,
	ccCoinOutThreshold			= 1058,
	ccPartialWATOutEnabled      = 1059,
	ccAutoUploadCreditsEnabled  = 1060,
	ccCashoutToHostEnabled      = 1061,
	ccUJPTaxWithholdingEnabled	= 1062,
	ccPercentFederalTax			= 1063,
	ccPercentStateTax			= 1064,
	ccAutoCardOutSecToCountdownCredits = 1065,
	ccAutoCardOutSecToCountdownNoCrdts = 1066,
	ccAutoCardOutCountdownSeconds	   = 1067,
	ccAutoCardOutSecToAlert			   = 1068,
	ccNumberOfSubgamesMismatchCheck = 1069,	
	ccCountdownDisplayIntervalInSec = 1070,	
	ccPointsCountdownControlByUniverse = 1071,
	ccDisplayBonusCounterInAttractor = 1072, // unused in 15.1
	ccRebootAsapThresholdInMb = 1073,
	ccRebootWarningThresholdMb = 1074,
	ccRestartUIAsapThresholdMb = 1075,
	ccRestartUIAfterDropThresholdMb = 1076,
	ccRestartUIDelayHours = 1077,
	ccTicketInResend = 1078,
	ccRestartUIAfterxDays = 1079,
	ccReadTimeOutMultiplier = 1080,
	ccUseSDSCardFormat = 1081,
	ccEnableLocationServices = 1082,
}	ConfigChangeType;

typedef enum
{
	meterChangeRequestMajor		= 0x00,
	meterChangeRequestMinor		= 0x01,
	meterChangeRequestNone		= 0x80,
}	MeterChangeRequestType;

typedef enum
{
	transactionsFillOkay = 0,
	transactionsFillWarning,
	transactionsFull,
}	TransactionsFillStatus;

typedef enum
{
	batteryStatusUnknown = 0,
	batteryStatusOkay,
	batteryStatusLow,
	batteryStatusNoBattery,
}	BatteryStatus;

typedef enum
{
	CAPTURE_OK,
	CAPTURE_REBOOT,
	CAPTURE_CANCELED,
}	MeterCaptureStatus;

typedef enum
{
	NoTrigger = 0,
	CoinInTrigger = 1,
	CoinLostTrigger = 2,
	AllJackpots = 3,
	AttendantPaidJackpot = 4,
	AttendantPaidProgressiveJackpot = 5,
	CoinOut = 6,
	TheoreticalWin = 7,
	TimePlayed = 8,
} TriggerClassType;

typedef enum
{
	DesktopOrientationNoChange = -1,
	DesktopOrientationDefault,
	DesktopOrientation90,
	DesktopOrientation180,
	DesktopOrientation270,
} DesktopOrientationType;

typedef enum
{
	/// <summary>
    /// Content Load notification
    /// </summary>
    ContentLoad = 1,

    /// <summary>
    /// Content unload notification
    /// </summary>
    ContentUnload = 2,

    /// <summary>
    /// Content play notification
    /// </summary>
    ContentPlay = 3,

    /// <summary>
    /// Content stop notification
    /// </summary>
    ContentStop = 4,

    /// <summary>
    /// Blob message notification
    /// </summary>
    BlobMessage = 5,

    /// <summary>
    /// Pop-up text message that is to appear on sentinel.
    /// </summary>
    PopUpTextMessage = 6,

}SMAPINotificationType;

typedef enum
{
	Uninterruptable = 0,

	Interruptable = 1,

} PromotionPlayPriority;

typedef enum
{
	CWORIENTATION_LEFT = 0,
	CWORIENTATION_RIGHT = 1,
	CWORIENTATION_BOTTOM_OR_LCD = 2,
} ContentWindowOrientation;

typedef enum
{
	LcdPlatform,
	VideoMixerPlatform,
	CompactPlatform,
} DisplayPlatform;

// This must match SentinelDisplayOrientationType table in DB
typedef enum
{
	SentinelIII = 0,
	nCompassLCD = 1,
	nCompassBottom = 2,
	nCompassLeft = 3,
	nCompassRight = 4,
	nCompassCompact = 5,
} SentinelDisplayOrientation;

typedef enum
{
	PackageSentinelIII = 0,
	PackageNCompassLCD = 1,
	PackageNCompassBottom = 2,
	PackageNCompassSide = 3,
	PackageNCompassCompact = 4,
} PackageOrientation;

typedef enum
{
	MWCODE_RESERVED = 0, // Reserve for EGM full screen.
	MWCODE_BANNER = 1, // Banner, with MW closed.

	MWCODE_OPEN = 2, // Add ContentWindowOrientation to this value.
	MWCODE_BANNER_MWLEFT = MWCODE_OPEN + CWORIENTATION_LEFT, // 2 == Banner, plus MW open on left.
	MWCODE_BANNER_MWRIGHT = MWCODE_OPEN + CWORIENTATION_RIGHT, // 3 == Banner, plus MW open on right.
	MWCODE_MWBOTTOM = MWCODE_OPEN + CWORIENTATION_BOTTOM_OR_LCD, // 4 == MW open on bottom.
} MediaWindowCodeType;

extern const std::string DISABLE_WATCHDOG;

// Process and Thread Diagnositics
#define CALLSTACK_BUFFER_SIZE (8192)
#define MAX_ERROR_FILE_SIZE ((CALLSTACK_BUFFER_SIZE)*128)
#define MAX_ERROR_HTML_SIZE ((MAX_ERROR_FILE_SIZE) * 2)
#define THREAD_PRIORITY_ABOVE_NORMAL 14

extern const int SHIFT_POLLINGSPEED;
extern const int SHIFT_GAMEEND;
extern const int SHIFT_GAMESTART;

#define MAGIC_ENCRYPTION_NUMBER (7521);

typedef enum
{
	CASHOUT_ERROR_CASH_INACTIVE							= 0x0001,
	CASHOUT_ERROR_CASH_OVER_TRANSFER_LIMIT				= 0x0002,
	CASHOUT_ERROR_CASH_UNDER_TRANSFER_LIMIT				= 0x0004,
	CASHOUT_ERROR_CASH_FUNCTIONALITY_UNAVAILABLE		= 0x0008,
	CASHOUT_ERROR_PROMO_OVER_TRANSFER_LIMIT				= 0x0010,
	CASHOUT_ERROR_PROMO_UNDER_TRANSFER_LIMIT			= 0x0020,
	CASHOUT_ERROR_MAX_CARD_LIMIT						= 0x0040,
	CASHOUT_ERROR_MAX_CARD_LIMIT_CASH_ON_GAME			= 0x0080,
	CASHOUT_ERROR_GUEST_PLAYER							= 0x0100,
	CASHOUT_ERROR_GUEST_PLAYER_CASH_ON_GAME				= 0x0200,
	CASHOUT_ERROR_SAS_ERROR								= 0x0400,
}CashoutError;

typedef enum
{
	AUTOUPLOAD_ERROR_CASH_INACTIVE						= 0x0001,
	AUTOUPLOAD_ERROR_CASH_OVER_TRANSFER_LIMIT			= 0x0002,
	AUTOUPLOAD_ERROR_CASH_UNDER_TRANSFER_LIMIT			= 0x0004,
	AUTOUPLOAD_ERROR_CASH_FUNCTIONALITY_UNAVAILABLE		= 0x0008,
	AUTOUPLOAD_ERROR_PROMO_OVER_TRANSFER_LIMIT			= 0x0010,
	AUTOUPLOAD_ERROR_PROMO_UNDER_TRANSFER_LIMIT			= 0x0020,
	AUTOUPLOAD_ERROR_GUEST_PLAYER						= 0x0040,
}AutoUploadError;

typedef enum
{
	ALERT_METER_SNAPSHOT_IN_PROGRESS         = 0x0001,
	ALERT_REBOOT_IN_PROGRESS                 = 0x0002,
	ALERT_REBOOTED_MEMORY_ERROR              = 0x0004,
	ALERT_REBOOTED_MEMORY_OK                 = 0x0008,
	ALERT_PROGRESSIVE_DOWN                   = 0x0010,
	ALERT_PROGRESSIVE_INELIGIBILITY          = 0x0020,
	ALERT_SYSTEM_BUSY                        = 0x0040
}Alerts;

extern const std::string DOWNLOAD_STATUS_INCOMPATIBLE_FILE_TYPE;
extern const std::string DOWNLOAD_STATUS_MEDIA_CARD_ERROR;
extern const std::string DOWNLOAD_STATUS_DOWNLOAD_ERROR;
extern const std::string DOWNLOAD_STATUS_CHECKSUM_VALIDATION;
extern const std::string DOWNLOAD_STATUS_CHECKSUM_ERROR;
extern const std::string DOWNLOAD_STATUS_EXTRACTING_CONTENTS;
extern const std::string DOWNLOAD_STATUS_EXTRACTION_ERROR;
extern const std::string DOWNLOAD_STATUS_SUCCESS;

extern const std::string PLATFORM_PT;
extern const std::string PLATFORM_PT_306;
extern const std::string PLATFORM_LITE;
extern const std::string PLATFORM_NEXUS;

extern const std::string FIRMWARE_BUNDLE_OS;
extern const std::string FIRMWARE_BUNDLE_EXE;
extern const std::string FIRMWARE_BUNDLE_FPGA;

extern const std::string ORIENTATION_LANDSCAPE;
extern const std::string ORIENTATION_PORTRAIT;

extern const DWORD LCD_RESOLUTION_WIDTH;
extern const DWORD LCD_RESOLUTION_HEIGHT;

extern const long HTTP_GET_TIMEOUT_IN_SECONDS;
