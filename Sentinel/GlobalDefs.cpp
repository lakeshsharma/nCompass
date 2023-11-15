#include "stdafx.h"
#include "GlobalDefs.h"

using namespace std;

const int ONE_HUNDRED_PERCENT = 100;
const int SIZE_OF_PB_LOGS = 100; //TBD size of logs
const int SIZE_OF_LOGS = 100; //TBD size of logs
const int SIZE_OF_IPS = 15; //TBD size of logs
const int WEBSOCKET_MAX_LENGTH = 5120;
const int MAXIMUM_DISPLAY_DURATION = 300000;//5 minutes
const int NRICH_AWARD_PERCENTAGE_THRESHOLD = 100;
const int INVALID_AWARD_AMOUNT = -1;
const long INVALID_SLOT_MAST_ID = 0;
const long INVALID_SLOT_NUMBER = 0;
const int SIZE_OF_FILE_NAME = 100; //TBD size of image file names
const BYTE POLLER_PACKET_VERSION = 1;
const std::string STORAGE_CARD_1 = ("\\Storage Card");
const std::string STORAGE_CARD_2 = ("\\Storage Card2");
const std::string SERVICE_WINDOW_LOCATION = ("\\ServiceWindow");

const LPCTSTR PLAYLIST_FILENAME = "PlayList.json";	// Do not put a slash in this name
const LPCTSTR ERROR_FILE_NAME = DEF_ERROR_FILE_NAME;
const WORD INVALID_GAME_NUMBER = 0xffff;
const DWORD BYTES_PER_KB = 1024;
const DWORD KBS_PER_MB = 1024;
const DWORD BYTES_PER_MEG = 1048576;
const DWORD ONE_DOLLAR_IN_CENTS = 100;
const WORD INVALID_DENOM = 0;
const int TimeDriftThresholdSeconds = 5; //the threshold in seconds before applying a time change
const std::string BADREAD_SOUND_FILENAME = "/badcard.mp3";
const std::string UNMOUNT_USB_SOUND_FILENAME = "/unmountusb.mp3";
const LPCTSTR OS_IMAGE_VERSION_FILENAME = "/OSImageVersion.txt";
const LPCTSTR OS_NEEDED_SERVICES_FILENAME = "/OSNeededServices.txt";
const LPCTSTR FLV_EXTENSION = ".flv";
const LPCTSTR BMP_EXTENSION = ".bmp";
const LPCTSTR SMD_EXTENSION = ".smd";
const DWORD ATTRACTOR_QUEUE_STAVE_EXPIRE = 300000;
const UINT S7_RETRIES = 3;

const DWORD PBT_AUTO_UPLOAD_PIN = (DWORD)-1;

const int RADIX_DECIMAL = 10;
const int NUMBER_OF_PLACES_AFTER_DECIMAL_POINT = 2;

const SYSTEMTIME DEFAULT_SYSTEMTIME = { 0, 0, 0, 0, 0, 0, 0, 0 };

const int MICROSOFT_RAND_MAX(0x7fff);

//Hard Pins
const DWORD DEFAULT_NDI_SAMPLE_RATE = 50; //50 miliseconds
const DWORD PIN_DEBOUNCE_TOLLERANCE = 2;  //tollerance * sample rate is real tollerance

//Access Security / Chassis Door
const int CHASSIS_DOOR_WAIT_TIMEOUT = 10; //10 miliseconds

//Network adapter
const int NUM_OF_DNS = 2; //primary and secondary
const int SPACE_TO_LEAVE_FOR_EXTRA_TERMINATOR = 1;
const char NETWORK_ADAPTER_NAME_CHAR[] = "DM9CE1";
const int CONFIG_REQUEST_TIMEOUT = 15000;	//the time in miliseconds between config requests.

const int GLOBAL_CARD_PASSWORD_SIZE = 2;
const int MEMCMP_EQUAL = 0; // Return value for memcmp() indicating equality.

const __int64 DOOR_CLOSED_METER_VALUE = 0; //or any even number
const __int64 DOOR_OPEN_METER_VALUE = 1;   //or any odd number

const WORD NVRAM_MEMORY_BAD = 0;
const WORD NVRAM_MEMORY_OK = 1;

//Percentage that the Poller transaction resend list should be allowed
//to fill before telling internals to slow down
const int RESEND_LIST_FILLING_THESHOLD = 66;
const int RESEND_LIST_FULL_THRESHOLD = 90;

//Watchdog Timeouts
const int DUMP_MEMORY_THREAD_TIMEOUT = 120;
const int GCF_TO_INTERNALS_QUEUE_TIMEOUT = 120;
const int INTERNALS_TO_GCF_QUEUE_TIMEOUT = 120;
// 2012-03-06 Per Altera, worst case timing for the erase bulk cycle time for a EPCS64 device is 160 seconds.
// 2012-03-06 Per Braemac, The S5_Carrier_EraseVEBEPCSMemory function performs a bulk erase of a EPCS64 device.
const int UI_TO_INTERNALS_QUEUE_TIMEOUT = 180;
const int INTERNALS_TO_UI_QUEUE_TIMEOUT = 120;
const int SCF_TO_INTERNALS_QUEUE_TIMEOUT = 120;
const int INTERNALS_TO_SCF_QUEUE_TIMEOUT = 600;
const int PROGRESSIVE_TO_INTERNALS_QUEUE_TIMEOUT = 120;
const int ON_MESSAGE_TIMEOUT = 300;
const int INTERNALS_TO_PROGRESSIVE_QUEUE_TIMEOUT = 120;
const int FLOOR_TO_INTERNALS_QUEUE_TIMEOUT = 120;
const int THIRD_PARTY_LISTENING_QUEUE_TIMEOUT = 120;
const int SCF_MAIN_THREAD_TIMEOUT = 120;
const int UI_MAIN_THREAD_TIMEOUT = 120;
const int GCF_MAIN_THREAD_TIMEOUT = 120;
const int INTERNALS_MAIN_THREAD_TIMEOUT = 120;
const int SCF_RESEND_LIST_TIMEOUT = 120;
const int SAS_POLLING_LOOP_TIMEOUT = 120;
const int SAS_SCHEDULER_TIMEOUT = 120;
const int SEND_TO_EGM_THREAD_TIMEOUT = 120;
const int SEND_TO_POLLER_THREAD_TIMEOUT = 120;
const int SERVICE_WINDOW_LISTENER_THREAD_TIMEOUT = 120;
const int READ_FROM_POLLER_THREAD_TIMEOUT = 120;
const int CARD_READER_TIMEOUT = 120;
const int NDI_THREAD_TIMEOUT = 120;
const int APP_MAIN_THREAD_TIMEOUT = 120;
const int INTERNALS_PROGRESS_CACHE_TIMEOUT = 120;
const int KEEP_TIME_THREAD_TIMEOUT = 120;
const int UPDATE_MIXER_TIMEOUT = 1800;//30 minutes
const int FILE_DOWNLOADER_TIMEOUT = 120;

const int EVENT_WAIT_TIMEOUT = 5000; //miliseconds
const DWORD USER_EVENT_DELAY_MILLISECONDS = 50;
const DWORD SLEEP_YIELD_MILLISECONDS = 10;
const DWORD SHUTDOWN_LOOP_SLEEP_PERIOD = 10;

const DWORD FULL_CLEAN_CODE = 65001;
const DWORD PARTIAL_CLEAN_CODE = 65002;

const DWORD WATCHDOG_CHECKIN = 500;

const int WAIT_FOR_UI_TO_BE_READY = 100;//100 miliseconds

const bool DEFAULT_ONE_LINK_PROGRESSIVES_SEN_OPTION_SUPPORTED = false;

// Sen Options 1 Map
const DWORD SO1_FILL_AMOUNT						= 0x00000001;	//If set, the Sentinel will force the user to enter a fill amount, otherwise the system will use the fill amount setup in the database for that machine
const DWORD SO1_USE_HOPPER						= 0x00000002;	//Used with Spintek Weighted Hopper systems where the Sentinel configures and monitors the hopper level
const DWORD SO1_NOT_USED_1						= 0x00000004;	//Not used. Carry over from DOS
const DWORD SO1_DISPLAY_NUMERIC_POINT_TOTOAL	= 0x00000008;	//Point Total will display on Card-In as numeric
const DWORD SO1_DISPLAY_PLAYER_NAME				= 0x00000010;
const DWORD SO1_DISPLAY_COUNTDOWN_AND_SESSION	= 0x00000020;	//Countdown xxx / Session display.
const DWORD SO1_DISPLAY_TOTAL_POINTS_SESSION_POINTS_AS_CASH = 0x00000040;	// Total (points as Cash) / Session (points as Cash)
const DWORD SO1_NOT_USED_2						= 0x00000080;	//Not used. Carry over from DOS.
const DWORD SO1_PBT_CASH_ENABLE					= 0x00000100;	//When set, PersonalBanker Cash functionality is enabled at the Sentinel
const DWORD SO1_PBT_POINSTS_ENABLE				= 0x00000200;	//When set PersonalBanker Points as Cash functionality will be enabled at the Sentinel
const DWORD SO1_PBT_PROMO_ENABLE				= 0x00000400;	//When set, PersonalBanker Promo downloads are enabled at the Sentinel.
const DWORD SO1_DISPLAY_COMP					= 0x00000800;	//When set, the players comp total will be displayed on Card-In
const DWORD SO1_PBT_POINTS_AS_PROMO				= 0x00001000;	//When set, Points will be downloaded as Promo Credits instead of Cash at the Sentinel
const DWORD SO1_COINS_WON_ACCOUNTING			= 0x00002000;	//Not Used. OASIS system has never implemented Coins Won Accounting.
const DWORD SO1_COINS_IN_ACCOUNTING				= 0x00004000;	//When set, points will be earned based on the amount of Coin In played.
const DWORD SO1_DISPLAY_CONGRATS				= 0x00008000;	//When set, a congratulations message will be displayed when a point is earned.
const DWORD SO1_THEORETICAL_ACCOUNTING			= 0x00010000;	//When set, points will be earned based on the theoretical win.
const DWORD SO1_COINS_OUT_ACCOUNTING			= 0x00020000;	//When set, points will be earned based on the amount of Coin out.
const DWORD SO1_PBT_ON_CARD_IN					= 0x00040000;	//When set on Card-In immediately go to PBT menus
const DWORD SO1_EMPLOYEE_PIN_REQUIRED			= 0x00080000;	//When set all employee cards will require a PIN number to access menu options.
const DWORD SO1_NOT_USED_3						= 0x00100000;	//Not used. 
const DWORD SO1_PARTIAL_BONUS_AWARD				= 0x00200000;	//
const DWORD SO1_DISPLAY_POINTS_TOTAL_AS_CASH	= 0x00400000;	//Point Total will display on Card-In as Cash
const DWORD SO1_DISPLAY_GOOD_LUCK_AND_SESSION	= 0x00800000;	//Good Luck / Session (numeric)
const DWORD SO1_COUPON_PROMO_IN_ENABLED			= 0x01000000;	//When set, the Sentinel will redeem Promo Coupons 
const DWORD SO1_VOUCHER_PROMO_IN_ENABLED		= 0x02000000;	//When set Voucher Promos will be accepted and redeemed by the Sentinel
const DWORD SO1_VIRTUAL_SESSION_ENABLED			= 0x04000000;	//When set, Virtual Sessions are enabled. The Sentinel will keep the Card inserted until the end of the current game.
const DWORD SO1_TICKET_IN_ENABLED				= 0x08000000;	//When set, tickets can be inserted and redeemed.
const DWORD SO1_NOT_USED_4						= 0x10000000;	//Not used in PRIME. Used previously in 11.5.2NV and older.
const DWORD SO1_PB_FROM_GAME_ENABLED			= 0x20000000;	//When set, PersonalBanker uploads (From Game) will be enabled.
const DWORD SO1_DISPLAY_PLAYER_POINTS_TOTAL		= 0x40000000;	//Will display the players point total as numeric for the session display 
const DWORD SO1_NOT_USED_5						= 0x80000000;	//Not used.

// Sen Options 2 Map
const DWORD SO2_PBT_PIN_NOT_REQUIRED					= 0x00000001;	//When Enabled a player pin code is not required (European)
const DWORD SO2_COUPON_PROMO_OUT_ENABLED				= 0x00000002;	//Sentinel will attempt to enable/disable EGM option bit for Prom out. Will send to OASIS ticket info regardless
const DWORD SO2_VOUCHER_OUT_ENABLED						= 0x00000004;	//Sentinel will attempt to enable/disable EGM option bit for "Use printer as cashout device". Will send to OASIS ticket info regardless
const DWORD SO2_CEP_OUT_ENABLED							= 0x00000008;	//Cashable credits can be removed from the EGM
const DWORD SO2_NCEP_OUT_ENABLED						= 0x00000010;	//Non-restricted Promotional credtis can be removed from the EGM
const DWORD SO2_DISPLAY_GOOD_LUCK_AND_SESSION_POINTS 	= 0x00000020;	//Good Luck Session as points display
const DWORD SO2_DISPLAY_TOTAL_AND_SESSION_POINTS_NUM	= 0x00000040;	//Total (points as number) / Session(points as number)
const DWORD SO2_DISPLAY_POINTS_TOTAL_AS_CASH_SESSION 	= 0x00000080;	//Will display the players point total as Cash for the session display 
const DWORD SO2_NCEP_AUTO_UPLOAD						= 0X00000100;	//NCEP Auto
const DWORD SO2_UJP_ENABLED								= 0X00000200;	//Unattended jackpot enabled.
// If both SO1_VIRTUAL_SESSION_ENABLED and SO2_MINI_VIRTUAL_SESSION_ENABLED are set then SO2_MINI_VIRTUAL_SESSION_ENABLED is ignored.
const DWORD SO2_MINI_VIRTUAL_SESSION_ENABLED 			= 0X00000400;	// Mini-virtual player sessions after player card out.
const DWORD SO2_ACCRUE_POINTS_FOR_NCEP					= 0x00000800;	//Points can be accrued from NCEP play.
const DWORD SO2_ACCRUE_COMP_FOR_NCEP					= 0x00001000;	//Comp can be earned from NCEP play.
//  If Poller packet version is less than 8 then SO2_ONE_LINE_PROGRESSIVES_ENABLED is ignored.
const DWORD SO2_ONE_LINK_PROGRESSIVES_ENABLED 			= 0x00002000;	// ONE-LINK progressive protocol communications and features.
const DWORD SO2_MGMD_ENABLED							= 0x00004000;	//MGMD enabled.
const DWORD SO2_RCCL_CARDS_ENABLED						= 0x00010000;	//Allows the Sentinel to read RCCL cards.
const DWORD SO2_PROBE_PBT_LOCK							= 0X00020000;	//Attempt PBT lock before authorizing PBT funds
const DWORD SO2_EXTERNAL_CREDIT_TRANSFER_VERIFICATION	= 0X00040000;	//Display external credit verification screen even if there is no fee for transfer
const DWORD SO2_MOBILE_CONNECT_COMMUNICATION_ENABLED	= 0X00080000;	//Communication with MobileConnect
const DWORD SO2_REMOTE_DEBUGGING_ENABLED				= 0X00100000;	//Remote Debugging
const DWORD SO2_PB_PARTIAL_WAT_OUT_ENABLED              = 0x00200000;   //Partial WAT-Out from game
const DWORD SO2_CASHOUT_TO_HOST_ENABLED					= 0x00400000;   //Cashout to Host
const DWORD SO2_CEP_AUTOPLOAD							= 0x00800000;   //CEP AUTO
const DWORD SO2_WAT_AUTOPLOAD							= 0x01000000;   //WAT AUTO

// Feature Flags Map
const DWORD FF_ATOMIC_REDEMPTIONS           = 0x00000001; 
const DWORD FF_JACKPOT_EXTERNAL_BONUS		= 0x00000002;
const DWORD FF_EXTENDED_TICKET_EXPIRATION	= 0x00000004;
const DWORD FF_VOUCHER_SEPARATION			= 0x00000008;
const DWORD FF_MULTI_GAME_MULTI_DENOM		= 0x00000010;
const DWORD FF_EXTERNAL_CREDIT_TO_GAME		= 0x00000020;
const DWORD FF_CHUNKED_GAMES_LIST			= 0x00000040;
const DWORD FF_WALLET_2_0_SUPPORTED			= 0x00000080;

//Memory Thresholds
const DWORD DEFAULT_REBOOT_ASAP_THRESHOLD_MB(75);
const DWORD DEFAULT_REBOOT_WARNING_THRESHOLD_MB(125);
const DWORD DEFAULT_REBOOT_UI_ASAP_THRESHOLD_MB(175);
const DWORD DEFAULT_REBOOT_UI_AFTER_DROP_THRESHOLD_MB(225);

const DWORD DEFAULT_RESTART_UI_AFTER_X_DAYS(16);
const DWORD DEFAULT_RESTART_UI_DELAY_HOURS(24);

const DWORD  DEFAULT_SAS_READ_TIMEOUT_MULTIPLIER(5);

//We made this 50 because all the games we saw that had this LP53 failures are WMS games, and they all have 10 subgames or less
//We didn't want to pick a number too low to risk missing games that will report more than 10 and a number too high because it will increase getting games list duration.
const DWORD DEFAULT_NUMBER_OF_SUBGAMES_MISMATCH_CHECK = 50;

// default extended buckets countdown display interval in seconds
const DWORD DEFAULT_COUNTDOWN_DISPLAY_INTERVAL = 6;

const DWORD DEFAULT_AUTO_UPLOAD_DELAY_SECONDS = 4;

// Player Session Default Values
const WORD NVRAM_PLAYER_TRACKING_DIVISOR_DEFAULT = 30000;
const DWORD DEFAULT_MULTIPLIER = 1;
const DWORD DEFAULT_COUNTDOWN = NVRAM_PLAYER_TRACKING_DIVISOR_DEFAULT;
const DWORD INVALID_MULTIPLIER = (DWORD)-1;
const DWORD INVALID_DIVIDER = (DWORD)-1;
const DWORD MAXPOINTINC = 25000;

const int SESSION_ID_DEFAULT = 0;
const DWORD GAME_SESSION_ID_DEFAULT = 0;
const WORD DEFAULT_GAME_LIST_CHUNK_SIZE = 10;

// Touch Screen Protocol -- only types USB and SERIAL are valid.
const byte TOUCH_SCREEN_PROTOCOL_TYPE_INVALID = 0;
const byte TOUCH_SCREEN_PROTOCOL_TYPE_USB = 1;
const byte TOUCH_SCREEN_PROTOCOL_TYPE_SERIAL = 2;
const std::string TOUCH_SCREEN_PROTOCOL_VENDOR_ID_NOT_APPLICABLE = _T("N/A");
const std::string TOUCH_SCREEN_PROTOCOL_BAUD_NOT_APPLICABLE = _T("N/A");
const std::string TOUCH_SCREEN_PROTOCOL_NAME_SERIAL_DEF = _T("[Serial]");
const std::string TOUCH_SCREEN_PROTOCOL_NAME_USB_DEF = _T("[USB]");

const std::string TOUCH_SCREEN_PROTOCOLS_DAT = _T("\\TouchScreenProtocols.dat");

// Video Settings
const byte VIDEO_SIGNAL_TYPE_AUTO = 0;
const byte VIDEO_SIGNAL_TYPE_ANALOGUE = 1;
const byte VIDEO_SIGNAL_TYPE_DIGITAL = 2;
const byte VIDEO_SIGNAL_TYPE_BOTH = 3;
const byte VIDEO_SIGNAL_TYPE_ERROR = 255;

// Thread Names

// EGMProtocol Threads
extern const std::string THREAD_READ_EGM_INTERNALS_QUEUE("ReadInternalsQueueThread/EGMPRotocol");

// InternalsFramework Threads
const std::string THREAD_DUMP_MEMORY("DumpMemoryThread/InternalsFramework");
const std::string THREAD_POLICY_SERVER("PolicyServerThread/InternalsFramework");
const std::string THREAD_READ_CARD("ReadCardThread/InternalsFramework");
const std::string THREAD_READ_GCF_QUEUE("ReadGCFQueueThread/InternalsFramework");
const std::string THREAD_READ_NDI("ReadNDIThread/InternalsFramework");
const std::string THREAD_READ_SCF_QUEUE("ReadSCFQueueThread/InternalsFramework");
const std::string THREAD_READ_PROGRESSIVE_QUEUE("ReadProgressiveQueueThread/InternalsFramework");
const std::string THREAD_READ_UI_QUEUE("ReadUIQueueThread/InternalsFramework");
const std::string THREAD_READ_CARD_READER("ReadCardReaderThread/InternalsFramework");
const std::string THREAD_UPDATE_POLLER_WITH_SUBGAME_METERS("UpdatePollerWithSubgameMetersThread/InternalsFramework");
const std::string THREAD_PROGRESSIVE_CACHE_INTERNALS("ProgressiveCache/InternalsFramework");
const std::string THREAD_TIME_KEEPER("TimeKeeper/InternalsFramework");
const std::string THREAD_UPGRADE_MIXER("UpgradeMixerThread/InternalsFramework");
const std::string THREAD_FILE_DOWNLOADER("FileDownloaderThread/InternalsFramework");

// MediaList Threads
const std::string THREAD_PLAY_MEDIA("PlayMediaThread/MediaList");

// PollerProtocol Threads
const std::string THREAD_READ_FROM_POLLER("ReadFromPollerThread/PollerProtocol");
const std::string THREAD_POLLER_RECEIVE_FLASH("ReceiveFlashThread/PollerProtocol");
const std::string THREAD_SEND_TO_POLLER("SendToPollerThread/PollerProtocol");

// SASProtocol Threads
const std::string THREAD_SCHEDULED_SAS_TASK("ScheduledSASTaskThread/SASProtocol");
const std::string THREAD_SEND_TO_EGM("SendToEGMThread/SASProtocol");

// SentinelApp Threads
const std::string THREAD_MAIN("Main/SentinelApp");
const std::string THREAD_WATCHDOG("WatchdogThread/SentinelApp");

//MediaPackageManager Threads
const std::string THREAD_FILE_CLEANUP("FileCleanupThread/MediaPackageManager");

// SentinelDlg Threads
const std::string THREAD_READ_SENTINEL_INTERNALS_QUEUE("ReadInternalsQueueThread/UIProtocol");

// ServiceWindowManager Threads
const std::string THREAD_SERVICEWINDOW_LISTENER("ServiceWindowListenerThread/ServiceWindowManager");

// ThirdPartyProtocol Threads
const std::string THREAD_THIRDPARTY_LISTENER("ThirdPartyListenerThread/ThirdPartyUIProtocol");

// SystemProtocol Threads
const std::string THREAD_POLLER_INTERNALS_QUEUE("ReadFromInternals/PollerProtocol");
const std::string THREAD_PROGRESSIVE_INTERNALS_QUEUE("ReadFromInternals/ProgressiveProtocol");

// ProgressiveProtocol Threads
const std::string THREAD_PROGRESSIVE_TIMED_EVENTS("TimedEventsThread/ProgressiveProtocol");
const std::string THREAD_PROGRESSIVE_ON_MESSAGE("OnMessage/ProgressiveProtocol");
const std::string THREAD_PROGRESSIVE_PLAYLIST_CONTROLLER("PlaylistController/ProgressiveProtocol");
const std::string THREAD_MULTICAST_CHANNEL("ProgressiveMulticastChannel/ProgressiveProtocol");
const std::string THREAD_PROGRESSIVE_CONTENT_FILES("ProgressiveContentFiles/ProgressiveProtocol");

const USHORT DEFAULT_MIXER_PARAM_STATUS = 0;

const WORD COMMUNICATION_TYPE_EGM	= 0x01;
const WORD COMMUNICATION_TYPE_POLLER= 0x02;
const WORD COMMUNICATION_TYPE_MMT	= 0x04;
const WORD COMMUNICATION_TYPE_PMT	= 0x08;
const WORD COMMUNICATION_TYPE_TCP	= 0x10;

const BYTE PLAYER_PREFIX = 0xa5;
const BYTE GLOBAL_PREFIX = 0x69;
const BYTE MECHANIC_PREFIX = 0x5a;
const BYTE FLOOR_PREFIX = 0x3c;
const BYTE TEMP_PREFIX = 0x00;

const bool NCL_CARDS_ENABLED_DEFAULT = false;

const BYTE CARD_READER_TRACK_1 = 1;
const BYTE CARD_READER_TRACK_2 = 2;
const BYTE CARD_READER_TRACK_NUMBER_DEFAULT = CARD_READER_TRACK_2;

const std::string SUBSCRIPTION_WHERE_ALL_STRING = "ALL";

const std::string DISABLE_WATCHDOG = "/sdboot/DISABLE_WATCHDOG";

const int SHIFT_POLLINGSPEED = 8;
const int SHIFT_GAMEEND = 16;
const int SHIFT_GAMESTART = 24;

#ifdef MCR_LOGGING
const byte LOGGING_DIAG_VALUE = 0xcc;
#endif

const std::string DOWNLOAD_STATUS_INCOMPATIBLE_FILE_TYPE = "incompatibleFileType";
const std::string DOWNLOAD_STATUS_MEDIA_CARD_ERROR = "mediaCardMissingOrFull";
const std::string DOWNLOAD_STATUS_DOWNLOAD_ERROR = "downloadError";
const std::string DOWNLOAD_STATUS_CHECKSUM_VALIDATION = "validatingChecksum";
const std::string DOWNLOAD_STATUS_CHECKSUM_ERROR = "checksumMismatch";
const std::string DOWNLOAD_STATUS_EXTRACTING_CONTENTS = "extractingContents";
const std::string DOWNLOAD_STATUS_EXTRACTION_ERROR = "extractionError";
const std::string DOWNLOAD_STATUS_SUCCESS = "success";

const std::string PLATFORM_PT = "NCOMPASS-206";
const std::string PLATFORM_PT_306 = "NCOMPASS-306";
const std::string PLATFORM_LITE = "NCOMPASS-LITE";
const std::string PLATFORM_NEXUS = "NEXUS-PT";

const std::string FIRMWARE_BUNDLE_OS("os.fb");
const std::string FIRMWARE_BUNDLE_EXE("exe.fb");
const std::string FIRMWARE_BUNDLE_FPGA("fpga.fb");

const std::string ORIENTATION_LANDSCAPE("Landscape");
const std::string ORIENTATION_PORTRAIT("Portrait");

const DWORD LCD_RESOLUTION_WIDTH = 640;
const DWORD LCD_RESOLUTION_HEIGHT = 240;

const long HTTP_GET_TIMEOUT_IN_SECONDS = 60;
