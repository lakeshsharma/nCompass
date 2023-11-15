#include "stdafx.h"
#include "ScreenDefs.h"

const std::string DATE_TIME_SECONDS_MASK = "%m/%d/%Y %I:%M:%S %p";

const int DISPLAY_GOAL_PERCENTAGE = 100;

//Screen Defines
#include "GlobalDefs.h"
#include "metersdef.h"

const RECT SCREEN_SIZE (0,0,640,240);
const COLORREF TEXT_COLOR = 0x00000000;
const COLORREF ERROR_TEXT_COLOR = 0x000000FF;
const COLORREF TRANSPARENT_COLOR = 0x00FF00FF;
const std::string CLICK_SOUND_FILENAME = "/clickerx.wav";
const int MAX_PIN_METERS = 12;
const WORD BACKLIGHT_INCREMENT = 5;
const DWORD FIVE = 5;
const DWORD TEN = 10;
const DWORD TWENTY = 20;
const DWORD FIFTY = 50;
const DWORD HUNDRED = 100;
const DWORD FOURTY = 40;
const DWORD	SIXTY = 60;
const DWORD EIGHTY = 80;
const std::string DATE_TIME_MASK = _T("%m/%d/%Y %I:%M %p");
const std::string MILITARY_DATE_TIME_MASK = _T("%m/%d/%Y %H:%M:%S");
const WORD SW_MIN_SIZE = 3;
const WORD SW_MAX_SIZE = 75;
const WORD TASKBAR_MIN_SIZE = 1;
const WORD TASKBAR_MAX_SIZE = 50;
const WORD AUTOCLOSE_MIN_MINUTES = 1;
const WORD AUTOCLOSE_MAX_MINUTES = 120;
const int VGA_SAMPLING_PHASE_MIN = 0;
const int VGA_SAMPLING_PHASE_MAX = 31;
const int VGA_SAMPLING_PHASE_RANGE = VGA_SAMPLING_PHASE_MAX - VGA_SAMPLING_PHASE_MIN;
const int VGA_SAMPLING_PHASE_NA = VGA_SAMPLING_PHASE_MIN - 1; // Outside of valid range.
const int VGA_SAMPLING_PHASE_DEF = VGA_SAMPLING_PHASE_NA;
const std::string ANNOUNCEMENT_SCREEN = _T("Announcement");
const std::string CARDIN_ELIGIBILITY_SCREEN = _T("CardInEligibility");
const std::string PROXIMITY_SCREEN = _T("Proximity");
const std::string JPACARDED_SCREEN = _T("JPACarded");
const std::string JPANONCARDED_SCREEN = _T("JPANonCarded");
const std::string TOTALIZER_SCREEN = _T("Totalizer");
const std::string PNW_END_SCREEN = _T("PnWEnd");
const std::string CARD_IN_PROMOTION = _T("CardIn");
const std::string PLAYER_ANNOUNCMENT = _T("PlayerMessage");
const std::string NONWINNER_NOTIFICATION_SCREEN = _T("NonWinnerNotification");
const std::string EVENT_TRIGGERED_SCREEN = _T("EventTriggered");

const std::string SPLASHDOWN_START_SCREEN(_T("SplashdownStart"));
const std::string RICOCHET_START_SCREEN(_T("RicochetStart"));
const std::string CASINO_CHALLENGE_START_SCREEN(_T("CasinoChallengetStart"));

const std::string SPLASHDOWN_AWARD_SCREEN(_T("SplashdownAward"));
const std::string RICOCHET_CELEBRATION_SCREEN(_T("RicochetCelebration"));
const std::string CASINO_CHALLENGE_AWARD1_SCREEN(_T("CasinoChallengeAward1"));
const std::string CASINO_CHALLENGE_AWARD2_SCREEN(_T("CasinoChallengeAward2"));
const std::string CASINO_CHALLENGE_AWARD3_SCREEN(_T("CasinoChallengeAward3"));

const std::string CASINO_CHALLENGE_FIVE_MINUTE_WARNING_END_SCREEN(_T("CasinoChallengeFiveMinuteWarning"));

const std::string SPLASHDOWN_END_SCREEN(_T("SplashdownEnd"));
const std::string SPLASHDOWN_WINNER_END_SCREEN(_T("SplashdownWinnerEnd"));

const std::string CASINO_CHALLENGE_END_SCREEN(_T("CasinoChallengeEnd"));

/////////////////////////////////////
//     Mechanic Card Text Items
/////////////////////////////////////

const std::string TXT_MECHANIC_CARD = _T("Mechanic Card");
const std::string TXT_MECHANIC_GLOBAL_OPTIONS = _T("Global Options");
const std::string TXT_MECHANIC_SENTINEL_OPTIONS = _T("Sentinel Options");
const std::string TXT_MECHANIC_BATTERY_STATUS = _T("Battery Status");
const std::string TXT_MECHANIC_BATTERY_UKNOWN = _T("Unknown");
const std::string TXT_MECHANIC_BATTERY_OKAY = _T("OK");
const std::string TXT_MECHANIC_BATTERY_LOW = _T("Low/no battery");
const std::string TXT_MECHANIC_BATTERY_NO = _T("No battery");
const std::string TXT_MECHANIC_EGM_DOWNLOAD = _T("EGM download");

const std::string TXT_MECHANIC_VIEW = _T("View");

//Mechanic card versions screen
const std::string TXT_MECHANIC_VERSIONS = _T("Versions");
const std::string TXT_MECHANIC_OS_VERSION = _T("OS:");
const std::string TXT_MECHANIC_SAS_VERSION = _T("SAS:");

//Mechanic card checksums screen
const std::string TXT_MECHANIC_OS_APPS_CHECKSUM = _T("OS Apps:");
const std::string TXT_MECHANIC_BOOT_LOADER_CHECKSUM = _T("Boot loader:");

//Mechanic card network information screen
const std::string TXT_MECHANIC_NETWORK_INFORMATION = _T("Network Information");
const std::string TXT_MECHANIC_HOST_NAME1 = _T("Poller IP:");
const std::string TXT_MECHANIC_DHCP_STATUS = _T("DHCP status:");
const std::string TXT_MECHANIC_IP_ADDRESS = _T("IP address:");
const std::string TXT_MECHANIC_SUBNET_MASK = _T("Subnet mask:");
const std::string TXT_MECHANIC_DEFAULT_GATEWAY = _T("Default gateway:");
const std::string TXT_MECHANIC_PREFERRED_DNS1 = _T("Preferred DNS:");
const std::string TXT_MECHANIC_ALTERNATE_DNS1 = _T("Alternate DNS:");
const std::string TXT_MECHANIC_MAC_ADDRESS = _T("MAC address:");

//Mechanic card view meters screen
const std::string TXT_MECHANIC_VIEW_METERS = _T("Meters");

//Mechanic card EGM meters screen
const std::string TXT_MECHANIC_EGM_METERS = _T("EGM Meters");

//Mechanic card Pins screen
const std::string TXT_MECHANIC_VIEW_PINS = _T("Pins");
const std::string TXT_MECHANIC_PIN1 = _T("Pin 1");
const std::string TXT_MECHANIC_PIN2 = _T("Pin 2");
const std::string TXT_MECHANIC_PIN3 = _T("Pin 3");
const std::string TXT_MECHANIC_PIN4 = _T("Pin 4");
const std::string TXT_MECHANIC_PIN5 = _T("Pin 5");
const std::string TXT_MECHANIC_PIN6 = _T("Pin 6");
const std::string TXT_MECHANIC_PIN7 = _T("Pin 7");
const std::string TXT_MECHANIC_PIN8 = _T("Pin 8");

//Mechanic card view recent transactions screen
const std::string TXT_MECHANIC_VIEW_RECENT_TRANS = _T("Recent Transactions");

//Mechanic card attendant paid payouts screen
const std::string TXT_MECHANIC_ATTENDANT_PAID = _T("Attendant Paid Payouts");
const std::string TXT_MECHANIC_ENTER_ATTENDANT_PAID = _T("Enter attendant paid payout:");
const std::string TXT_MECHANIC_HANDPAY = _T("Handpay");
const std::string TXT_MECHANIC_CC = _T("Cancel credit");
const std::string TXT_MECHANIC_TOP_AWARD = _T("Top award");

const std::string TXT_MECHANIC_SLOTMAST = _T("Set SlotMast ID");

//Mechanic card voucher log screen
const std::string TXT_MECHANIC_VOUCHER_LOG = _T("Voucher Log");

//Mechanic card PersonalBanker Transactions screen
const std::string TXT_MECHANIC_PB_TRANS = _T("PersonalBanker™ Transactions");

//Mechanic card PersonalBanker Log screen
const std::string TXT_MECHANIC_PB_LOG = _T("PersonalBanker™ Log");

//Mechanic card Transaction Log screen
const std::string TXT_MECHANIC_TRANSACTION_LOG = _T("Transaction Log");

//Mechanic card Composite Tilt Log screen
const std::string TXT_MECHANIC_TILT_LOG = _T("Composite Tilt Log");

//Mechanic card Repair Log screen
const std::string TXT_MECHANIC_REPAIR_LOG = _T("Failure/Repair Codes");

//Mechanic card Diagnostics screen
const std::string TXT_MECHANIC_DIAGNOSTICS = _T("Diagnostics");

//Mechanic card More Diagnostics screen
const std::string TXT_MECHANIC_MORE_DIAGNOSTICS = _T("More Diagnostics");

//Mechanic card PersonalBanker Pending screen
const std::string TXT_MECHANIC_PB_PENDING = _T("PersonalBanker™ Pending");

//Mechanic card Repair Log screen
const std::string TXT_MECHANIC_MEDIA_WINDOW_STATUS_LOG = _T("Media Window Status Log");

//Mechanic card Comm Status screen
const std::string TXT_MECHANIC_COMM_POLLER = _T("Poller");
const std::string TXT_MECHANIC_COMM_EGM = _T("EGM");
const std::string TXT_MECHANIC_COMM_TCPIP = _T("TCPIP");
const std::string TXT_MECHANIC_COMM_ERRORS = _T("Errors");
const std::string TXT_MECHANIC_COMM_MMT = _T("MMT");
const std::string TXT_MECHANIC_COMM_MMT_NA = _T("N/A");
const std::string TXT_MECHANIC_COMM_PMT = _T("PMT");
const std::string TXT_MECHANIC_COMM_PMT_NA = _T("N/A");
const std::string TXT_MECHANIC_COMM_CONNECTED = _T("Connected");
const std::string TXT_MECHANIC_COMM_NOT_CONNECTED = _T("Not connected");
const std::string TXT_MECHANIC_COMM_VOUCHER_SERVICE = _T("VS");
const std::string TXT_MECHANIC_COMM_VOUCHER_SERVICE_NA = _T("N/A");
const std::string TXT_MECHANIC_DISPLAY_YELLOW_COMM_DOWN = _T("DisplayYellowCommDown");
const std::string TXT_MECHANIC_DISPLAY_RED_COMM_DOWN = _T("DisplayRedCommDown");

//Mechanic card View Disk Space screen
const std::string TXT_MECHANIC_DISK_SPACE = _T("Disk Space");
const std::string TXT_MECHANIC_DRIVE = _T("Drive");
const std::string TXT_MECHANIC_TOTAL_SIZE = _T("Total size");
const std::string TXT_MECHANIC_FREE = _T("Free");

//Mechanic card Flash Download Status screen
const std::string TXT_MECHANIC_FLASH_STATUS = _T("Download Status");
const std::string TXT_MECHANIC_DOWNLOAD_STATUS = _T("Download status:");
const std::string TXT_MECHANIC_DOWNLOAD_CHECKSUM = _T("Checksum:");
const std::string TXT_MECHANIC_DOWNLOADED = _T("Downloaded:");
const std::string TXT_MECHANIC_DOWNLOAD_TYPE = _T("Type:");

const std::string TXT_MECHANIC_DOWNLOAD_NK = _T("NK.nb0");
const std::string TXT_MECHANIC_DOWNLOAD_DEFAULTMEDIA = _T("Default media");
const std::string TXT_MECHANIC_DOWNLOAD_SYSTEM = _T("System");

//Mechanic card View Voucher Limits screen
const std::string TXT_MECHANIC_VOUCHER_LIMITS = _T("Voucher Limits");
const std::string TXT_MECHANIC_VOUCHER_BUFFERING = _T("Voucher buffering:");
const std::string TXT_MECHANIC_TOTAL_VOUCHERS = _T("Total vouchers:");
const std::string TXT_MECHANIC_DISABLED = _T("Disabled");
const std::string TXT_MECHANIC_ENABLED = _T("Enabled");
const std::string TEXT_SIZE_TEXT = _T("MB");

//Mechanic card Test Lockout screen
const std::string TXT_MECHANIC_TEST_LOCKOUT = _T("Test Lockout");

//Mechanic card Denomination screen
const std::string TXT_MECHANIC_DENOMINATION = _T("Denominations");
const std::string TXT_MECHANIC_METER_CALC = _T("Meter calc:");
const std::string TXT_MECHANIC_PB = _T("PersonalBanker™:");
const std::string TXT_MECHANIC_EGM = _T("EGM:");

//Mechanic card Loss Limit screen
const std::string TXT_MECHANIC_LOSS_LIMIT = _T("Loss Limit");
const std::string TXT_MECHANIC_AT_EGM = _T("Loss Limit Enabled");
const std::string TXT_MECHANIC_SESSION_NUM = _T("Session Number");
const std::string TXT_MECHANIC_BILL_OFF = _T("Bill Validator Off Amount");

//Mechanic card Send Messages screen
const std::string TXT_MECHANIC_SEND_MESSAGES = _T("Send Messages");

//Mechanic card Send Repair Codes screen
const std::string TXT_MECHANIC_SEND_REPAIR = _T("Failure and Repair Codes");
const std::string TXT_MECHANIC_SENDING_MESSAGE = _T("Sending message");
const std::string TXT_MECHANIC_SEND_MAINTENANCE_CODE = _T("Maintenance codes");

//Mechanic card Capture Meters screen
const std::string TXT_MECHANIC_REBOOT_IN_PROGRESS("Reboot in progress...");
const std::string TXT_MECHANIC_CAPTURED_SUB_GAMES = _T("Captured %u of %u sub-games");
const std::string TXT_FINISHED_METERS = _T("Finished capturing game meters");
const std::string TXT_FINISHED_METERS_SUB = _T("Finished capturing game and sub game meters");
const std::string TXT_FAILED_METERS = _T("Meter capture failed");
const std::string TXT_MECHANIC_RAMCLEAR_YESNO_1 = _T("Proceed with EGM RAM");
const std::string TXT_MECHANIC_RAMCLEAR_YESNO_2 = _T("Clear meter capture?");

//Mechanic card Log Screen text
const std::string TXT_MECHANIC_DATE_TIME = _T("Time stamp");
const std::string TXT_MECHANIC_AMOUNT = _T("Amount");
const std::string TXT_MECHANIC_STATUS = _T("Status");
const std::string TXT_MECHANIC_VALIDATION = _T("Validation number");
const std::string TXT_MECHANIC_SEQUENCE = _T("Sequence number");
const std::string TXT_MECHANIC_ERROR_CODE = _T("Error code: ");
const std::string TXT_MECHANIC_SOURCE = _T("Source");
const std::string TXT_MECHANIC_TYPE = _T("Type");
const std::string TXT_MECHANIC_RESULT = _T("Result");
const std::string TXT_MECHANIC_TRANS_ID = _T("Transaction ID: ");
const std::string TXT_MECHANIC_CODE = _T("Code");
const std::string TXT_MECHANIC_EGM_DISPLAY = _T("EGM Display");
const std::string TXT_MECHANIC_OCR = _T("OCR");
const std::string TXT_MECHANIC_MWCODE_TASK_BAR = (_T("Task Bar"));
const std::string TXT_MECHANIC_MWCODE_BANNER_MWLEFT = _T("MW Left");
const std::string TXT_MECHANIC_MWCODE_BANNER_MWRIGHT = _T("MW Right");
const std::string TXT_MECHANIC_MWCODE_MWBOTTOM = _T("MW Bottom");
const std::string TXT_MECHANIC_MWCODE_EGMFULL = _T("Full EGM");
const std::string TXT_NA = "N/A";

//Mechanic card FPGA upgrade screen
const std::string TXT_MECHANIC_FIRMWARE_UPGRADE = _T("Firmware Upgrade");
const std::string TXT_MECHANIC_FPGA_UPGRADE_YESNO_1 = _T("This may take several");
const std::string TXT_MECHANIC_FPGA_UPGRADE_YESNO_2 = _T("minutes. Press Yes");
const std::string TXT_MECHANIC_FPGA_UPGRADE_YESNO_3 = _T("to continue.");

const std::string TXT_MECHANIC_FPGA_UPGRADE_TEXT_1 = _T("Warning! Do not turn");
const std::string TXT_MECHANIC_FPGA_UPGRADE_TEXT_2 = _T("off power. Firmware");
const std::string TXT_MECHANIC_FPGA_UPGRADE_TEXT_3 = _T("upgrade in progress...");


/////////////////////////////////////
//     Global Card Text Items
/////////////////////////////////////
const std::string TXT_GLOBAL_CARD = _T("Global Card");

//Global card Calibration Settings screen
const std::string TXT_GLOBAL_CALIBRATION = _T("Calibration Settings");

//Global card Calibration screen
const std::string TXT_GLOBAL_CALIBRATING = _T("Calibrating Touch Screen");
const std::string TXT_GLOBAL_CALIBRATING_TEXT1 = _T("Removing Card EXITS calibration");

//Global card Touch Screen Protocol current settings screen
const std::string TXT_GLOBAL_TOUCH_SCREEN_PROTOCOL = _T("Touch Screen Protocol");
const std::string TXT_GLOBAL_SELECT_TOUCH_SCREEN_PROTOCOL = _T("Select Touch Screen Protocol");
const std::string TXT_GLOBAL_PROTOCOL_NAME = _T("Protocol Name:");
const std::string TXT_GLOBAL_VENDOR_ID = _T("Vendor ID:");
const std::string TXT_GLOBAL_BAUD = _T("Baud:");
const std::string TXT_GLOBAL_USB_STATUS = _T("USB Status:");
const std::string TXT_GLOBAL_DETECTED = _T("Detected");
const std::string TXT_GLOBAL_NOT_DETECTED = _T("Not detected");

//Global card Touch Screen RTS / CTS Configuration screen
const std::string TXT_GLOBAL_TOUCH_SCREEN_RTS_CTS_CONFIGURATION = _T("Touch Screen RTS / CTS Configuration");
const std::string TXT_GLOBAL_HANDSHAKING_DISABLED = _T("Handshaking Disabled");
const std::string TXT_GLOBAL_HANDSHAKING_ENABLED = _T("Handshaking Enabled");
const std::string TXT_GLOBAL_RTS_SET_TO_MATCH_CTS = _T("RTS Set to Match CTS");
const std::string TXT_GLOBAL_RTS_ALWAYS_OFF = _T("RTS Always Off");
const std::string TXT_GLOBAL_RTS_ALWAYS_ON = _T("RTS Always On");

//Global card Set VGA Sampling Phase screen
const std::string TXT_GLOBAL_VGA_SAMPLING_PHASE = _T("VGA Sampling Phase");
const std::string TXT_GLOBAL_PHASE = _T("Phase");
const std::string TXT_GLOBAL_PHASE_NA = _T("N/A");

//Global card Video Configuration screen
const std::string TXT_GLOBAL_VIDEO_CONFIGURATION = _T("Video Configuration");

//Global card Video Mode Configuration screen
const std::string TXT_GLOBAL_VIDEO_INPUT_CONFIGURATION = _T("Video Input Configuration");
const std::string TXT_GLOBAL_VIDEO_OUTPUT_CONFIGURATION = _T("Video Output Configuration");
const std::string TXT_GLOBAL_VIDEO_SIGNAL_TYPE = _T("Video Signal");
const std::string TXT_GLOBAL_VIDEO_SIGNAL_AUTO = _T("Auto");
const std::string TXT_GLOBAL_VIDEO_SIGNAL_ANALOG = _T("Analog");
const std::string TXT_GLOBAL_VIDEO_SIGNAL_DIGITAL = _T("Digital");
const std::string TXT_GLOBAL_VIDEO_MODE_CONFIG = _T("Video Mode:");
const std::string TXT_GLOBAL_VIDEO_OUTPUT_MODE_AUTO_CONFIG_TRUE = _T("Auto Configure");
const std::string TXT_GLOBAL_VIDEO_OUTPUT_MODE_AUTO_CONFIG_FALSE = _T("Manual Override");
const std::string TXT_GLOBAL_VIDEO_OUTPUT_MODE_NATIVE = _T("Native");
const std::string TXT_GLOBAL_SAVE_VIDEO_CONFIG_YESNO_1 = _T("Restart required.");
const std::string TXT_GLOBAL_SAVE_VIDEO_CONFIG_YESNO_2 = _T("Press Yes to continue.");
const std::string TXT_GLOBAL_SAVE_VIDEO_ROTATION_YESNO_1 = _T("20-30 seconds required.");
const std::string TXT_GLOBAL_SAVE_VIDEO_ROTATION_YESNO_2 = _T("Press Yes to continue.");

//Global card Video Mode selection screen
const std::string TXT_GLOBAL_SELECT_VIDEO_MODE = _T("Select Video Mode");
const std::string TXT_GLOBAL_VIDEO_MODE_NUMBER = _T("Mode#:");
const std::string TXT_GLOBAL_VIDEO_MODE_DESCRIPTION = _T("Description:");

//Global card Video Rotation screen
const std::string TXT_GLOBAL_VIDEO_ROTATION = _T("Video Rotation");
const std::string TXT_GLOBAL_VIDEO_ROTATION_NO_ROTATION = _T("No Rotation");
const std::string TXT_GLOBAL_VIDEO_ROTATION_90_DEGREES = _T("90 Degrees");
const std::string TXT_GLOBAL_VIDEO_ROTATION_180_DEGREES = _T("180 Degrees");
const std::string TXT_GLOBAL_VIDEO_ROTATION_270_DEGREES = _T("270 Degrees");
const std::string TXT_GLOBAL_CURRENT_ROTATION = _T("Current Rotation:");

//Global PIN Settings screen
const std::string TXT_GLOBAL_PIN_SETTINGS = _T("Pin Interface and Active States");

//Global card Set Volume Backlight screen
const std::string TXT_GLOBAL_BACK_LEVEL = _T("Backlight Level");
const std::string TXT_GLOBAL_VOL_LEVEL = _T("Volume Level");
const std::string TXT_GLOBAL_VOL = _T("Volume");
const std::string TXT_GLOBAL_BACK = _T("Backlight");

//Global card Service Window screens
const std::string TXT_GLOBAL_SERVICE_WINDOW_SETTINGS = _T("Media Window Settings");
const std::string TXT_GLOBAL_SIZE = _T("Size");
const std::string TXT_GLOBAL_LEFT = _T("Left");
const std::string TXT_GLOBAL_RIGHT = _T("Right");
const std::string TXT_GLOBAL_BOTTOM = _T("Bottom");
const std::string TXT_GLOBAL_CLEAR_NVRAM("NVRAM");
const std::string TXT_GLOBAL_CLEAR_EEPROM("EEPROM");
const std::string TXT_GLOBAL_CLEAR_HARDWARE("Hardware");
const std::string TXT_GLOBAL_CLEAR_ALL("All");
const std::string TXT_GLOBAL_EGM_DISPLAY = _T("Media Window EGM Display");
const std::string TXT_GLOBAL_AR_HEAD = _T("EGM Display");
const std::string TXT_GLOBAL_ASPECT_RATIO = _T("Maintain Aspect Ratio");
const std::string TXT_GLOBAL_SIZE_TO_FIT = _T("Size To Fit");
const std::string TXT_GLOBAL_AUTO_MEDIAWINDOW = _T("Auto Close");
const std::string TXT_GLOBAL_AUTO_MEDIAWINDOW_MINUTES = _T("Auto Close Minutes");
const std::string TXT_GLOBAL_AUTO_MEDIAWINDOW_MAINTAIN = _T("Maintain");

//Global Card Video Offsets screens
const std::string TXT_GLOBAL_VIDEO_OFFSETS = _T("Video Offsets");

/////////////////////////////////////
//     Player Card Text Items
/////////////////////////////////////
const std::string TXT_OPEN = _T("Open");
const std::string TXT_CLOSED = _T("Closed");
const std::string TXT_UNKNOWN_ERROR = _T("Unknown error");
const std::string TXT_CALCULATING_CHECKSUMS = _T("Calculating checksums");
const std::string TXT_REBOOT_IN_PROGRESS = "Reboot in progress...";
const std::string TXT_REBOOTED = _T("Rebooted...");
const std::string TXT_MEMORY_ERROR = _T("Memory error.");
const std::string TXT_MEMORY_OK = _T("Memory OK.");
const std::string TXT_METER_CARD = _T("Meter Card: ");
const std::string TXT_COLON_SPACE = ": ";

const std::string TXT_PLAYER_TIER_POINTS = _T("Tier points:");
const std::string TXT_PLAYER_CASH_FROM_GAME = _T("Cash from game");
const std::string TXT_PLAYER_POINTS_FROM_GAME = _T("Points from game");
const std::string TXT_PLAYER_CASH_TO_GAME = _T("Cash to game");
const std::string TXT_PLAYER_POINTS_TO_GAME = _T("Points to game");
const std::string TXT_PLAYER_PROMO_TO_GAME = _T("Promo to game");
const std::string TXT_PLAYER_PROMO_FROM_GAME = _T("Promo from game");
const std::string TXT_CREDIT_TRANSFER = _T("Credit transfer");
const std::string TXT_PLAYER_PARTIAL_CASH_FROM_GAME = _T("Partial Cash from game");
const std::string TXT_PLAYER_PARTIAL_POINTS_FROM_GAME = _T("Partial Points from game");
const std::string TXT_PLAYER_CASHOUT_CASH_FROM_GAME = _T("Cashout Cash from game");
const std::string TXT_PLAYER_CASHOUT_POINTS_FROM_GAME = _T("Cashout Points from game");
const std::string TXT_PLAYER_CASHOUT_PROMOS_FROM_GAME = _T("Cashout Promos from game");

const std::string TXT_METER_NAMES[] = 
{
	"None",
	"1-Coin Drop",
	"2-Slot Door",
	"3-Drop Door",
	"4-Logic Door",
	"5-Cash Door",
	"6-Fill Door",
	"7-N/A",
	"8-PBT Pts to Promo Dn",
	"9-N/A",
	"10-N/A",
	"11-PBT Promo $ Down",
	"12-Ones",
	"13-Twos",
	"14-Fives",
	"15-Tens",
	"16-Twenties",
	"17-Fifties",
	"18-Hundreds",
	"19-Voucher In Count",
	"20-Voucher Out Count",
	"21-Hopper Level",
	"22-Voucher In",
	"23-Voucher Out",
	"24-Door",
	"25-Credit Meter",
	"26-True Coin In",
	"27-True Coin Out",
	"28-Bill In",
	"29-Att Pd Payouts",
	"30-Vouch Promo In Cnt",
	"31-Voucher Promo",
	"32-Coup Promo In Cnt",
	"33-Coupon Promo In",
	"34-HPCC Receipt Count",
	"35-HPCC Receipt",
	"36-HPJP Receipt Count",
	"37-HPJP Receipt",
	"38-True Coin Drop",
	"39-Coupon Promo Out",
	"40-Coup Promo Out Cnt",
	"41-N/A",
	"42-N/A",
	"43-N/A",
	"44-N/A",
	"45-Cur Game #",
	"46-Num Games",
	"47-Belly Door",
	"48-N/A",
	"49-N/A",
	"50-N/A",
	"51-N/A",
	"52-N/A",
	"53-Coin In",
	"54-Coin Out",
	"55-Game Start",
	"56-Games Won",
	"57-Max Coin Bet",
	"58-Cancel Credit",
	"59-Legacy Bonus",
	"60-Att Pd JPs",
	"61-Prog Coin In",
	"62-N/A",
	"63-Black Out",
	"64-N/A",
	"65-N/A",
	"66-Att Pd CC",
	"67-Slot Door Open",
	"68-Two Hundreds",
	"69-Five Hundreds",
	"70-One Thousands",
	"71-WAT In",
	"72-NCEP In",
	"73-CEP In",
	"74-WAT Out",
	"75-NCEP Out",
	"76-CEP Out",
	"77-EFT In",
	"78-Mach Paid Ext Bonus",
	"79-Att Pd Ext Bonus",
	"80-Att Paid Prog",
	"81-Mach Paid Prog",
	"82-Physical Coin In",
	"83-Physical Coin Out",
	"84-Noncash Promo Plyd",
	"85-Cash Promo Played",
	"86-Game WTPP",
	"87-Total Drop",
	"88-Voucher Ticket",
	"89-Access Security Door",
	"90-NCEP Credits",
	"91-Non-NCEP Played"
};
