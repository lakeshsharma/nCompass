#include "stdafx.h"
#include "InternalMessage.h"

// Sentinel includes.
#include "PokerHand.h"
#include "ProgressiveProtocol/EGMAward.h"
#include "ProgressiveProtocol/ProgressiveAnnouncementsInfo.h"
#include "ProgressiveProtocol/ProgressiveJackpotAnnouncement.h"
#include "ProgressiveProtocol/ProgressiveTotalizerAnnouncement.h"
#include "ProgressiveProtocol/ProgressiveBonusingAvailable.h"
#include "ProgressiveProtocol/ProgressiveClaimableAward.h"
#include "ProgressiveProtocol/ProgressiveConfig.h"
#include "ProgressiveProtocol/ProgressiveConnectionInfo.h"
#include "ProgressiveProtocol/ProgressiveHost.h"
#include "ProgressiveProtocol/ProgressiveLevelBroadcast.h"
#include "ProgressiveProtocol/ProgressiveLevelForward.h"
#include "ProgressiveProtocol/ProgressiveLevelsInfo.h"
#include "ProgressiveProtocol/ProgressiveWinnerPick.h"
#include "ProgressiveProtocol/ProgressiveWinnerPicked.h"
#include "Utilities.h"
#include <Payout.h>

using namespace std;

const char* InternalMsgTypeStr[] = 
{
	"INT_MSG_NONE",
	"INT_MSG_ACCEPT_VIDEO_MODE_CONFIG",
	"INT_MSG_ALLOW_TICKET_PRINTED",
	"INT_MSG_ATTENDANT_PAID_PAYOUT",
	"INT_MSG_ATTRACTOR_SEQUENCE_COMPLETE",
	"INT_MSG_AUTHORIZE_PIN",
	"INT_MSG_AUTO_TRANSFER_CANCELLED",
	"INT_MSG_BAD_CARD_READ",
	"INT_MSG_BILL_METERS_COMPLETE",
	"INT_MSG_BILL_VALIDATOR_DISABLE",
	"INT_MSG_BILL_VALIDATOR_ENABLE",
	"INT_MSG_BUTTON_ACTIVITY",
	"INT_MSG_CARD_REMOVED",
	"INT_MSG_CASINO_CHALLENGE_AWARD",
	"INT_MSG_CASINO_CHALLENGE_END",
	"INT_MSG_CASINO_CHALLENGE_COUNTER",
	"INT_MSG_CASINO_CHALLENGE_FIVE_MINUTE_END",
	"INT_MSG_CASINO_CHALLENGE_INCREMENT",
	"INT_MSG_CASINO_CHALLENGE_STOP_PROGRESS_FROM_UI",
	"INT_MSG_CASINO_CHALLENGE_TRIGGER",
	"INT_MSG_CDM_ANNOUNCEMENT",
	"INT_MSG_CHANGE_EGM_TYPE",
	"INT_MSG_CHANGE_LIGHT_ON",
	"INT_MSG_CHANGE_LIGHT_OFF",
	"INT_MSG_CHANGE_SLOTMASTID",
	"INT_MSG_CHECK_EMPLOYEE_PIN",
	"INT_MSG_CHECK_OTHER_PBT_AMOUNT",
	"INT_MSG_CARD_INFO",
	"INT_MSG_CARDED_SESSION_IN_PROGRESS",
	"INT_MSG_TRANSFER_PENDING",
	"INT_MSG_CLEAR_PBT",
	"INT_MSG_CLEAR_PBT_PROBE",
	"INT_MSG_CLEAR_PERSISTENT_MEMORY",
	"INT_MSG_CLOSE_PTE_LEADERBOARD",
	"INT_MSG_DEFAULT_DATA_DIRECTORY_UPDATED",
	"INT_MSG_DISPLAY_ACCESSING_ACCOUNT",
	"INT_MSG_DISPLAY_ACCOUNT_BALANCE",
	"INT_MSG_DISPLAY_ATTRACTOR",
	"INT_MSG_SET_CARD_BEZEL",
	"INT_MSG_DISPLAY_BANNED_PLAYER",
	"INT_MSG_DISPLAY_DOWNLOAD_PENDING",
	"INT_MSG_DISPLAY_INFORMATION",
	"INT_MSG_DISPLAY_LOSS_LIMIT_BALANCE",
	"INT_MSG_DISPLAY_PTE_ANTICIPATION",
	"INT_MSG_DISPLAY_PTE_AWARD",
	"INT_MSG_DISPLAY_PTE_ENDCOUNTDOWN",
	"INT_MSG_DISPLAY_PTE_LEADERBOARD",
	"INT_MSG_DISPLAY_PTE_LEADERBOARD_PLAYER_INITIATED",
	"INT_MSG_DISPLAY_PTE_LEVELUP",
	"INT_MSG_DISPLAY_PTE_RESULTS",
	"INT_MSG_DISPLAY_UJP",
	"INT_MSG_DISPLAY_UJP_INVALID_PIN",
	"INT_MSG_DISPLAY_AUTOCARDOUT",
	"INT_MSG_AUTO_CARDOUT_TIMED_OUT",
	"INT_MSG_AUTO_CARDOUT_END_SESSION",
	"INT_MSG_AUTO_CARDOUT_CONTINUE_SESSION",
	"INT_MSG_DISPLAY_EXTENDED_BUCKETS",
	"INT_MSG_DOLLAR_BUTTON_SUCCESS",
	"INT_MSG_DOLLAR_BUTTON_ERROR",
	"INT_MSG_DOOR_CLOSED",
	"INT_MSG_DOOR_OPEN",
	"INT_MSG_DOWNLOAD_CANCELED",
	"INT_MSG_EFT_SEED",
	"INT_MSG_EGM_AWARD",
	"INT_MSG_EGM_AWARD_STATUS",
	"INT_MSG_CUMULATIVE_PROGRESSIVE_WIN",
	"INT_MSG_EGM_PROTOCOL_VERSION",
	"INT_MSG_EGM_STATE",
	"INT_MSG_EGM_SUPPORTS_LP_7B",
	"INT_MSG_EMPLOYEE_CARD_IN",
	"INT_MSG_EMPLOYEE_GOODBYE",
	"INT_MSG_EMPLOYEE_RESPONSE",
	"INT_MSG_END_CAPTURING_METERS",
	"INT_MSG_ENABLE_GAME",
	"INT_MSG_FINISHED_GAMESLIST_FIRSTPASS",
	"INT_MSG_FINISHED_OFFLINE_PASS",
	"INT_MSG_FIRST_PLAY",
	"INT_MSG_FORCE_CARD_OUT",
	"INT_MSG_GAME_COMM_ERROR",
	"INT_MSG_GAME_COMM_STATUS",
	"INT_MSG_GAME_DATA",
	"INT_MSG_GCF_STARTED",
	"INT_MSG_GET_ACCOUNT_BALANCE",
	"INT_MSG_PERFORM_BATTERY_TEST",
	"INT_MSG_GET_BILL_METERS",
	"INT_MSG_GET_BONUS_METERS",
	"INT_MSG_GET_CONFIG_FROM_CACHE",
	"INT_MSG_GET_DENOM",
	"INT_MSG_GET_EGM_CAPS",
	"INT_MSG_GET_EGM_DISPLAY",
	"INT_MSG_GET_EGM_SETTINGS",
	"INT_MSG_GET_GAME_INFO",
	"INT_MSG_GET_GAME_LIST",
	"INT_MSG_GET_PROGRESSIVE_GAMES_INFO",
	"INT_MSG_GET_IP",
	"INT_MSG_GET_JACKPOT_INFO",
	"INT_MSG_GET_LOSS_LIMIT_BALANCE_UPDATE",
	"INT_MSG_GET_METERS",
	"INT_MSG_GET_NETWORK_INFO",
	"INT_MSG_GET_PB_BALANCES",
	"INT_MSG_GET_PROGRESSIVE_HOPPER_PAID_INFO",
	"INT_MSG_GET_PROGRESSIVE_WIN_INFO",
	"INT_MSG_GET_STATUS",
	"INT_MSG_GET_SUBGAME_METERS",
	"INT_MSG_GET_TICKET_STATUS",
	"INT_MSG_GET_VIDEO_MODE_CONFIG",
	"INT_MSG_HANDPAY_RESET",
	"INT_MSG_HEARTBEAT",
	"INT_MSG_HIDE_SERVICE_WINDOW",
	"INT_MSG_IN_EVENT_TRIGGER_REJECTED",
	"INT_MSG_IN_TRIGGERED_EVENT_TRIGGER_REJECTED",
	"INT_MSG_INC_BILL_METER",
	"INT_MSG_EMPLOYEE_PIN_RESULT",
	"INT_MSG_JACKPOT_ANNOUNCEMENT",
	"INT_MSG_JACKPOT_HIT",
	"INT_MSG_JACKPOT_INFO",
	"INT_MSG_JACKPOT_STORED",
	"INT_MSG_LCD_DETECT",
	"INT_MSG_LOAD_EVENT_XML",
	"INT_MSG_LOAD_EVENT_PACKAGE",
	"INT_MSG_LOSS_LIMIT_BALANCE_UPDATE",
	"INT_MSG_OS_UPGRADE",
	"INT_MSG_MAC_ADDR",
	"INT_MSG_MACHINE_MOVE_BEGIN",
	"INT_MSG_MACHINE_MOVE_COMPLETE",
	"INT_MSG_MACHINE_OFFLINE",
	"INT_MSG_MACHINE_ONLINE",
	"INT_MSG_METER_CHANGE_ALLOW",
	"INT_MSG_METER_CHANGE_ALLOW_SENT",
	"INT_MSG_METER_CHANGE_CANCEL",
	"INT_MSG_METER_CHANGE_COMPLETE",
	"INT_MSG_METER_CHANGE_REQUEST",
	"INT_MSG_NOTIFY_GAME_END",
	"INT_MSG_NOTIFY_GAME_START",
	"INT_MSG_NOTIFY_RESEND_EMPTY",
	"INT_MSG_NVRAM_VALID",
	"INT_MSG_OFFLINE_COIN_IN",
	"INT_MSG_PB_ACKED",
	"INT_MSG_PB_CANCEL_SESSION",
	"INT_MSG_PB_COMPLETE",
	"INT_MSG_PB_PIN_RESET_RESULT",
	"INT_MSG_PB_CHECK_DENOM",
	"INT_MSG_PB_CHECK_PIN",
	"INT_MSG_PB_INFO",
	"INT_MSG_PB_NCEPOUT_FAIL",
	"INT_MSG_PB_REQUEST_TRANSFER",
	"INT_MSG_PB_RESPONSE",
	"INT_MSG_PB_STATUS",
	"INT_MSG_PB_STORED",
	"INT_MSG_PB_TRANSFER",
	"INT_MSG_PB_RESTORE_SESSION",
	"INT_MSG_PB_SET_NEW_PIN",
	"INT_MSG_PLAY_CONGRATS",
	"INT_MSG_PLAYER_BONUS_EVENTS",
	"INT_MSG_PLAYER_DATA",
	"INT_MSG_PLAYER_DATA_PROCESSED",
	"INT_MSG_PLAYER_GLOBAL_BALANCE",
	"INT_MSG_PLAYER_GOODBYE",
	"INT_MSG_PLAYER_TRACKING_DURATION",
	"INT_MSG_PLAYER_UPDATE",
	"INT_MSG_PLAYER_UPDATE_TIME",
	"INT_MSG_CONNECTIVITY_STATUS",
	"INT_MSG_POLLERID",
	"INT_MSG_POLLER_LIST_FULL",
	"INT_MSG_POLLER_LIST_OK",
	"INT_MSG_POLLER_LIST_WARNING",
	"INT_MSG_POLLER_PACKET_SENT_VERIFY_ERRORS",
	"INT_MSG_PREVIEW_TASKBAR_SIZE",
	"INT_MSG_PREVIEW_SW_LOCATION",
	"INT_MSG_PREVIEW_SW_SIZE",
	"INT_MSG_PRIZE_LOCK_CLEAR",
	"INT_MSG_PROBE_PBT",
	"INT_MSG_PROBE_PBT_RESULT",
	"INT_MSG_PROCESS_TICKET_IN",
	"INT_MSG_PROGRESSIVE_BONUSING_AVAILABLE",
	"INT_MSG_PROGRESSIVE_DASHBOARD_STATUS",
	"INT_MSG_PROGRESSIVE_JACKPOT_ANNOUNCEMENT",
	"INT_MSG_PROGRESSIVE_TOTALIZER_ANNOUNCEMENT",
	"INT_MSG_PROGRESSIVE_CLAIMABLE_AWARD",
	"INT_MSG_PROGRESSIVE_EXTERNAL_COMMUNICATIONS_STATUS",
	"INT_MSG_PROGRESSIVE_FORWARD",
	"INT_MSG_PROGRESSIVE_HOPPER_PAID_HIT",
	"INT_MSG_PROGRESSIVE_HOPPER_PAID_INFO",
	"INT_MSG_PROGRESSIVE_WIN_HIT",
	"INT_MSG_PROGRESSIVE_WIN_INFO",
	"INT_MSG_PROGRESSIVE_WIN",
	"INT_MSG_PROGRESSIVE_CONFIGURATION",
	"INT_MSG_PROGRESSIVE_LEVEL_BROADCAST",
	"INT_MSG_PROGRESSIVE_LEVEL_FORWARD",
	"INT_MSG_PROGRESSIVE_SCHEDULE_END",
	"INT_MSG_PROGRESSIVE_WINNER_PICK",
	"INT_MSG_PROGRESSIVE_WINNER_PICKED",
	"INT_MSG_PROGRESSIVE_ACCOUNTING_ONLY",
	"INT_MSG_PROGRESSIVE_PROMO_GAME",
	"INT_MSG_PROGRESSIVE_PRIZE",
	"INT_MSG_PROCESS_BUTTON_PRESS",
	"INT_MSG_PROCESS_BUTTON_PRESS_FOR_SCREEN",
	"INT_MSG_PROMOTION_DURATION",
	"INT_MSG_QUERY_POKER_HAND",
	"INT_MSG_RECONNECT",
	"INT_MSG_RESET_JACKPOT",
	"INT_MSG_RESTART_SERVICEWINDOW",
	"INT_MSG_RICOCHET_TRIGGER",
	"INT_MSG_RICOCHET_AWARD",
	"INT_MSG_SCF_PROTOCOL_VERSION",
	"INT_MSG_SCF_CARDOUT",
	"INT_MSG_SEND_CARD_IN",
	"INT_MSG_SEND_CARD_OUT",
	"INT_MSG_SEND_EXTERNAL_BONUS",
	"INT_MSG_SEND_HOPPER_FILL",
	"INT_MSG_SEND_MAINTENANCE",
	"INT_MSG_SEND_PRINTER_MAINTENANCE",
	"INT_MSG_SEND_PROGRESSIVE_LEVEL_FORWARD",
	"INT_MSG_SEND_REPAIR_CODE",
	"INT_MSG_SEND_STACKER_IN",
	"INT_MSG_SEND_STACKER_OUT",
	"INT_MSG_SEND_TICKET",
	"INT_MSG_SEND_TILT",
	"INT_MSG_SEND_WITNESS",
	"INT_MSG_SERVICE_WINDOW_SHOW",
	"INT_MSG_SERVICE_WINDOW_STATE",
	"INT_MSG_SET_ALLOW_FLOOR_LOCKOUT",
	"INT_MSG_SET_ALTERNATE_DNS",
	"INT_MSG_SET_BACKLIGHT",
	"INT_MSG_SET_TASKBAR",
	"INT_MSG_SET_CDM_WINDOW",
	"INT_MSG_SET_COMM_STATUS",
	"INT_MSG_SET_CONNECTION_DATA",
	"INT_MSG_SET_POLLER_SETTINGS",
	"INT_MSG_SET_IP_ADDRESS",
	"INT_MSG_SET_CURRENT_GAME_DENOM",
	"INT_MSG_SET_DENOM",
	"INT_MSG_SET_DEFAULT_GATEWAY",
	"INT_MSG_SET_DESKTOP_ORIENTATION_TYPE",
	"INT_MSG_SET_DHCP_SETTING",
	"INT_MSG_SET_DISPLAY_MESSAGE",
	"INT_MSG_SET_EGM_SETTINGS",
	"INT_MSG_SET_FIRMWARE_UPGRADE_AVAILABLE",
	"INT_MSG_SET_GAME_LIST",
	"INT_MSG_SET_ENABLED_GAMES",
	"INT_MSG_SET_GLOBAL_PASSWORD",
	"INT_MSG_SET_GLOBAL_BALANCE",
	"INT_MSG_SET_IMAGE_CHECKSUM",
	"INT_MSG_SET_IP",
	"INT_MSG_SET_JACKPOT_RESET",
	"INT_MSG_SET_METER",
	"INT_MSG_SET_METERS",
	"INT_MSG_SET_MOBILE_CONNECT_COMMUNICATION_SUPPORT",
	"INT_MSG_SET_MULTICAST_DATA",
	"INT_MSG_SET_ONE_LINK_PROGRESSIVES_SEN_OPTION_SUPPORT",
	"INT_MSG_SET_ONE_LINK_PROGRESSIVES_SUPPORT",
	"INT_MSG_SET_SLOT_NUMBER",
	"INT_MSG_SET_PAR",
	"INT_MSG_SET_PB_BALANCES",
	"INT_MSG_SET_PLAYER_INFO",
	"INT_MSG_SET_PREFERRED_DNS",
	"INT_MSG_SET_PROGRESSIVE_CONFIG",
	"INT_MSG_SET_PROGRESSIVE_CONNECTION_INFO",
	"INT_MSG_SET_PROGRESSIVE_DOWN",
	"INT_MSG_SET_PROGRESSIVE_GAMES_INFO",
	"INT_MSG_SET_PROGRESSIVE_HOST",
	"INT_MSG_SET_PROGRESSIVE_HIT_SENT",
	"INT_MSG_SET_PROGRESSIVE_PAID_SENT",
	"INT_MSG_SET_SERVICE_WINDOW",
	"INT_MSG_SET_SERVICE_WINDOW_AFTER_SHOW_OR_HIDE",
	"INT_MSG_SET_SLOTMAST_REVISION",
	"INT_MSG_SET_SLOTMASTID",
	"INT_MSG_SET_STATUS",
	"INT_MSG_SET_SYSTEM_CONFIG",
	"INT_MSG_SET_SUBNET_MASK",
	"INT_MSG_SET_TICKET_EXP_DAYS",
	"INT_MSG_SET_TICKETING_INFO",
	"INT_MSG_SET_TIME",
	"INT_MSG_SET_TOUCH",
	"INT_MSG_SET_VERSION",
	"INT_MSG_SET_VIDEO_MODE_CONFIG",
	"INT_MSG_SET_VIDEO_OFFSETS",
	"INT_MSG_SET_VOLUME",
	"INT_MSG_SHOW_ANNIVERSARY",
	"INT_MSG_SHOW_ATTRACTOR",
	"INT_MSG_SHOW_BIRTHDAY",
	"INT_MSG_SHOW_BIRTHDAY_AND_ANNIVERSARY",
	"INT_MSG_SHOW_CREDIT_TRANSFER_ENTRY_SCREEN",
	"INT_MSG_SHOW_NON_CREDIT_TRANSFER_ENTRY_SCREEN",
	"INT_MSG_SHOW_OTHER_BUTTON_ERROR",
	"INT_MSG_SHOW_SERVICE_WINDOW",
	"INT_MSG_SHUTDOWN",
	"INT_MSG_SNAPSHOT_PROGRESS",
	"INT_MSG_SPLASHDOWN_START",
	"INT_MSG_SPLASHDOWN_START_TIMER",
	"INT_MSG_SPLASHDOWN_AWARD",
	"INT_MSG_SPLASHDOWN_END",
	"INT_MSG_SPLASHDOWN_WINNER_END",
	"INT_MSG_SPLASHDOWN_END_TIMER",
	"INT_MSG_START_CAPTURING_METERS",
	"INT_MSG_START_DEBUG_LOG",
	"INT_MSG_START_UI",
	"INT_MSG_START_VIDEO",
	"INT_MSG_STATUS_REBOOTING",
	"INT_MSG_STOP_DEBUG_LOG",
	"INT_MSG_STOP_UI",
	"INT_MSG_STOP_VIDEO",
	"INT_MSG_SUBGAME_SELECTION",
	"INT_MSG_SYSTEM_ONLINE",
	"INT_MSG_SUBSCRIBE_REQUEST",
	"INT_MSG_UNSUBSCRIBE_REQUEST",
	"INT_MSG_SUBSCRIPTION_UPDATES",
	"INT_MSG_TERMINATE_BROWSER",
	"INT_MSG_TICKET_INSERTED",
	"INT_MSG_TICKET_OUT_PENDING_COUNT",
	"INT_MSG_TICKET_RESPONSE",
	"INT_MSG_TICKET_RESPONSE_RECEIVED",
	"INT_MSG_TICKET_STATUS",
	"INT_MSG_TICKET_STORED",
	"INT_MSG_TRANSACTION_AUTO_UPLOAD_CANCELED",
	"INT_MSG_TRANSACTION_BILL_ACCEPTED",
	"INT_MSG_TRANSACTION_BONUS_EVENTS",
	"INT_MSG_TRANSACTION_CASH_TRANSFER",
	"INT_MSG_TRANSACTION_CHANGE_LIGHT_ON",
	"INT_MSG_TRANSACTION_CHANGE_LIGHT_OFF",
	"INT_MSG_TRANSACTION_CONFIG_CHANGE",
	"INT_MSG_TRANSACTION_CONTROLLED_REBOOT_WARNING",
	"INT_MSG_TRANSACTION_CONTROLLED_REBOOT",
	"INT_MSG_TRANSACTION_EMPLOYEE_CARD_IN",
	"INT_MSG_TRANSACTION_EMPLOYEE_CARD_OUT",
	"INT_MSG_TRANSACTION_FAILURE_CODE",
	"INT_MSG_TRANSACTION_GAME_INFO",
	"INT_MSG_TRANSACTION_ILLEGAL_DOOR_OPEN",
	"INT_MSG_TRANSACTION_JACKPOT",
	"INT_MSG_TRANSACTION_JACKPOT_FILL",
	"INT_MSG_TRANSACTION_MACHINE_ONLINE",
	"INT_MSG_TRANSACTION_MACHINE_OFFLINE",
	"INT_MSG_TRANSACTION_MACHINE_MOVE_BEGIN",
	"INT_MSG_TRANSACTION_MACHINE_MOVE_COMPLETE",
	"INT_MSG_TRANSACTION_MAINTENANCE",
	"INT_MSG_TRANSACTION_METER_RUNAWAY",
	"INT_MSG_TRANSACTION_PB",
	"INT_MSG_TRANSACTION_PD_CHASSIS_OPEN",
	"INT_MSG_TRANSACTION_VIRTUAL_SESSION_STARTED",
	"INT_MSG_TRANSACTION_PLAYER_CARD_IN",
	"INT_MSG_TRANSACTION_PLAYER_CARD_OUT",
	"INT_MSG_TRANSACTION_PLAYER_UPDATE",
	"INT_MSG_TRANSACTION_SENTINEL_OFF",
	"INT_MSG_TRANSACTION_SENTINEL_ON",
	"INT_MSG_TRANSACTION_SERIAL_COMM_DOWN",
	"INT_MSG_TRANSACTION_SERIAL_COMM_UP",
	"INT_MSG_TRANSACTION_SLOT_POWER_OFF",
	"INT_MSG_TRANSACTION_SLOT_POWER_ON",
	"INT_MSG_TRANSACTION_SNAPSHOT",
	"INT_MSG_TRANSACTION_TICKET",
	"INT_MSG_TRANSACTION_UJP_RESULT",
	"INT_MSG_TRANSACTION_ONELINK",
	"INT_MSG_TRANSACTION_GENERIC",
	"INT_MSG_UI_ACTIVITY_OCCURRED",
	"INT_MSG_START_PB_STARVATION",
	"INT_MSG_CLEAR_DISPLAY_STARVATION",
	"INT_MSG_UJP_CALLHOST",
	"INT_MSG_UJP_CHECK_PIN",
	"INT_MSG_UJP_TIMED_OUT",
	"INT_MSG_UNATTENDED_JACKPOT",
	"INT_MSG_UNLOAD_EVENT_PACKAGE",
	"INT_MSG_UPDATE_AUTO_CLOSE_SETTING",
	"INT_MSG_UPDATE_BONUS_TYPES",
	"INT_MSG_UPDATE_EGM_DISPLAY_SETTING",
	"INT_MSG_UPDATE_MEDIAWINDOW_CONFIG",
	"INT_MSG_UPDATE_PLAYER_STATE_CLOSE_SETTING",
	"INT_MSG_UPDATE_PLAYER_STATE_OPEN_SETTING",
	"INT_MSG_UPDATE_SAS_INFO",
	"INT_MSG_UPDATE_UI_CONFIG",
	"INT_MSG_UPGRADE_AVAILABLE",
	"INT_MSG_UPGRADE_MIXER",
	"INT_MSG_UPGRADE_CARRIER",
	"INT_MSG_USER_SELECTED_SCREEN",
	"INT_MSG_CALIBRATE_SENTINEL_SCREEN",
	"INT_MSG_MWCONFIG_FILE_AVAILABLE",
	"INT_MSG_TRANSACTION_PB_AUTHORIZE",
	"INT_MSG_AUTHORIZATION_GRANTED",
	"INT_MSG_AUTHORIZATION_DENIED",
	"INT_SHOW_POINTS_VS_ACCOUNT_BALANCE",
	"INT_MSG_POINTS_OR_CASH_LABEL",
	"INT_MSG_ACTIVE_SESSION",
	"INT_MSG_GENERAL_POLL_EXCEPTION",
	"INT_DMSG_PROGRESSIVE_QUEUE_TO_ONELINK",
	"INT_MSG_ADMIN_MESSAGE",
	"INT_MSG_DISPLAY_EMPLOYEE_SCREEN",
	"INT_MSG_TEST_URL",
	"INT_MSG_UPDATE_ATTRACTOR",
	"INT_MSG_ARCHIVE_LOGS",
	"INT_MSG_PLAY_UNMOUNT_SOUND",
    "INT_MSG_FEATURE_FLAG_UPDATE",
	"INT_MSG_FLUSH_PLAYER_UPDATE",
	"INT_MSG_TRANSACTION_GAME_SESSION_CHANGE",
   	"INT_MSG_INCREMENT_GAME_SESSION_ID_IF_REQUIRED",
	"INT_MSG_SEND_ALL_GAMES_LIST",
	"INT_MSG_SEND_ENABLED_DENOMGAMES_LIST",
	"INT_MSG_TRYSENDGAMELIST_GLOBAL_MGMD",
	"INT_MSG_TRYSENDGAMELIST_POLLER_ONLINE",
	"INT_MSG_REMOTE_REBOOT",
	"INT_MSG_SEND_STATUS_AFTER_PARSING_CONFIG",
	"INT_MSG_PAIRING_SETTINGS_EVENT",
	"INT_MSG_PAIRING_REQUEST",
	"INT_MSG_PAIRING_EVENT",
    "INT_MSG_PAIRING_STATUS_EVENT",
	"INT_MSG_CARD_CHANGE_REQUEST",
	"INT_MSG_CARD_CHANGE_EVENT",
	"INT_MSG_PLAYER_SESSION_INFO",
    "INT_MSG_CREDIT_TOTALS_REQUEST",
    "INT_MSG_FUNDS_TRANSFER_REQUEST",
    "INT_MSG_FUNDS_TRANSFER_EVENT",
	"INT_MSG_GAME_LIST_CHUNKS_SENT",
	"INT_MSG_NEW_PIN_REQUIRED",
	"INT_MSG_PROMOGAME_REQUEST",
	"INT_MSG_PROMOGAME_COMPLETE",
	"INT_MSG_LIST_OF_CERTS",
	"INT_MSG_TRANSACTION_CARDLESS_SESSION_START",
	"INT_MSG_TRANSACTION_CARDLESS_SESSION_END",
	"INT_MSG_EGM_LOCK_UNLOCK_STATUS",
	"INT_MSG_ENABLE_CASHOUT_TO_HOST",
	"INT_MSG_CASHOUT_TO_HOST",
	"INT_MSG_CASHOUT_TO_HOST_STATUS",
	"INT_MSG_SET_POLLER_IP",
	"INT_MSG_PERFORM_LOAD_BALANCING",
	"INT_MSG_SET_NFD_POLLER",
	"INT_MSG_EXTENDED_BUCKETS_EXIT",
	"INT_MSG_SET_NUMBER_OF_SUBGAMES_MISMATCH_CHECK",
	"INT_MSG_PROCESS_TAX_DEDUCTION_COMPLETE",
	"INT_MSG_CANCEL_PENDING_AUTO_UPLOAD",
	"INT_MSG_BAD_PACKET_RECEIVED_FROM_POLLER",
	"INT_MSG_DISPLAY_CASHOUT_PBT_ERROR",
	"INT_MSG_DISPLAY_AUTOUPLOAD_ERROR",
	"INT_MSG_EXTENSIBLE_SETTINGS",
	"INT_MSG_UPDATE_SAS_READ_TIMEOUT",
	"INT_MSG_REQUEST_CONFIG",
	"INT_MSG_LISTEN_TO_PBT_PROGRESS_FROM_GAME",
	"INT_MSG_LISTEN_TO_PBT_PROGRESS_TO_GAME",
	"INT_MSG_CHECK_UPLOADABLE_CREDITS",
	"INT_MSG_MONITOR_MOBILE_PB_SESSION",
	"INT_MSG_DISPLAY_PB_TRANSFER_ERROR",
	"INT_MSG_PAYOUT",
	"INT_MSG_CAMPAIGN_DOWNLOAD_REQUEST",
	"INT_MSG_SKIN_DOWNLOAD_REQUEST",
	"INT_MSG_FIRMWARE_PACKAGE_AVAILABLE",
	"INT_MSG_CONFIG_FILE_AVAILABLE",
	"INT_MSG_CAMPAIGN_AVAILABLE",
	"INT_MSG_SKIN_AVAILABLE",
	"INT_MSG_REFRESH_UI",
	"INT_MSG_MOBILE_CONNECT_PRESS",
	"INT_MSG_TRANSACTION_MOBILE_CONNECT_PROXIMITY",
	"INT_MSG_CALL_MC_PAIRING_SETTINGS_REQUEST",
	"INT_MSG_TRANSACTION_PROXIMITY_REQUEST_BEACON_SCAN_LIST", // Proximity-related request
	"INT_MSG_PROXIMITY_BEACON_SCAN_LIST_RESPONSE", // Proximity-related response
	"INT_MSG_CALL_MCP_PROXIMITY_SETTINGS_REQUEST", // Proximity-related request
	"INT_MSG_PROXIMITY_SETTINGS_EVENT", // Proximity-related event
};

static_assert(NUMBER_OF_INT_MSG == _countof(InternalMsgTypeStr),
	"InternalMsgTypeStr has to be updated to match all the entries in InternalMsgType");

class CTransactionMetersContent :
	public CInternalMsgContentWrapper<CMeters, INT_MSG_CONTENT_TRANSACTIONMETERS>
{
public:
	CTransactionMetersContent(CMeters *data) :
		CInternalMsgContentWrapper<CMeters, INT_MSG_CONTENT_TRANSACTIONMETERS>(data) {}
};

class CTimeContent :
	public CInternalMsgContentWrapper<time_t, INT_MSG_CONTENT_TIME>
{
public:
	CTimeContent(time_t *data) :
		CInternalMsgContentWrapper<time_t, INT_MSG_CONTENT_TIME>(data) {}
};

CInternalMessage::CInternalMessage(InternalMsgType type)
{
	InitMembers(type);
}

CInternalMessage::CInternalMessage(InternalMsgType type, IInternalMsgContent* content, InternalMsgContentType contentType)
{
	InitMembers(type);
	SetData(content, contentType);
}

CInternalMessage::CInternalMessage(InternalMsgType type, IInternalMsgContent& content)
{
	InitMembers(type);
	SetData(content);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CMeters *metersData, WORD gameNumber, bool bMeterSnapshot)
{
	InitMembers(type);
	SetData(metersData, gameNumber, bMeterSnapshot);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CTicket *ticketData)
{
	InitMembers(type);
	SetData(ticketData);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CGames *gamesData)
{
	InitMembers(type);
	SetData(gamesData);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CConfigFields *configData)
{
	InitMembers(type);
	SetData(configData, CConfigFields::MsgContentType());
}

CInternalMessage::CInternalMessage(InternalMsgType type, const string& stringData, const string& stringDataAdditional)
{
	InitMembers(type);
	SetData(stringData, stringDataAdditional);
}

CInternalMessage::CInternalMessage(InternalMsgType type, std::vector<CSubscription>* subscriptions)
{
	InitMembers(type);
	m_subcriptions = subscriptions;
}

CInternalMessage::CInternalMessage(InternalMsgType type, time_t *timeData)
{
	InitMembers(type);
	SetData(timeData);
}

CInternalMessage::CInternalMessage(InternalMsgType type, WORD wordData, WORD additionalWordData)
{
	InitMembers(type);
	m_wordData = wordData;
	m_wordDataAdditional = additionalWordData;
}

CInternalMessage::CInternalMessage(InternalMsgType type, DWORD dwordData, DWORD additionalDWordData)
{
	InitMembers(type);
	m_dwordData = dwordData;
	m_dwordDataAdditional = additionalDWordData;
}

CInternalMessage::CInternalMessage(InternalMsgType type, int64_t int64Data, int64_t additionalInt64Data)
{
	InitMembers(type);
	m_int64Data = int64Data;
	m_int64DataAdditional = additionalInt64Data;
}

CInternalMessage::CInternalMessage(InternalMsgType type, int32_t intData, int32_t additionalIntData)
{
	InitMembers(type);
	m_intData = intData;
	m_intDataAdditional = additionalIntData;
}

CInternalMessage::CInternalMessage(InternalMsgType type, const BYTE* ptrBytes, size_t size)
{
	InitMembers(type);
	m_bytesData = ptrBytes;
	m_size = size;
}

CInternalMessage::CInternalMessage(InternalMsgType type, CConnection* connection)
{
	InitMembers(type);
	SetData(connection);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CPlayerInfo *playerInfo)
{
	InitMembers(type);
	SetData(playerInfo);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CPlayerGlobalBalance *globalBalance)
{
	InitMembers(type);
	SetData(globalBalance);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CPlayer *player)
{
	InitMembers(type);
	SetData(player);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CEmployee *employee)
{
	InitMembers(type);
	SetData(employee);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CBonusEvents *bonusEventsData)
{
	InitMembers(type);
	SetData(bonusEventsData);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CTicketingInfo *ticketInfoData)
{
	InitMembers(type);
	SetData(ticketInfoData);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CLogs *log)
{
	InitMembers(type);
	SetData(log);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CPersonalBankerInfo *pbInfoData)
{
	InitMembers(type);
	SetData(pbInfoData);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CSubscription* subscription)
{
	InitMembers(type);
	SetData(subscription);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CEGMAward* egmAward)
{
	InitMembers(type);
	SetData(egmAward);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CProgressiveConfig* progressiveConfig)
{
	InitMembers(type);
	SetData(progressiveConfig);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CProgressiveConnectionInfo* progressiveConnectionInfo)
{
	InitMembers(type);
	SetData(progressiveConnectionInfo);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CGameEndData *gameEndData)
{
	InitMembers(type);
	SetData(gameEndData);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CPayout* payoutData)
{
	InitMembers(type);
	SetData(payoutData);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CProgressiveHost* progressiveHost)
{
	InitMembers(type);
	SetData(progressiveHost);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CPersonalBankerTransfer *pbTransferData, PersonalBankerState state, bool showTimedOut)
{
	InitMembers(type);
	SetData(pbTransferData);
	m_wordData = state;
	m_wordDataAdditional = showTimedOut ? 1 : 0;
}

CInternalMessage::CInternalMessage(InternalMsgType type, CEGMConfig * EGMConfigData)
{
	InitMembers(type);
	SetData(EGMConfigData);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CJackpot *jackpotData)
{
	InitMembers(type);
	SetData(jackpotData);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CFlashStatus *flashStatusData)
{
	InitMembers(type);
	SetData(flashStatusData);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CFileList *fileList)
{
	InitMembers(type);
	SetData(fileList);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CThirdPartyMessage *thirdPartyMessage)
{
	InitMembers(type);
	SetThirdPartyMessage(thirdPartyMessage);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CMessagePersist *messagePersist)
{
	InitMembers(type);
	SetMessagePersist(messagePersist);
}

CInternalMessage::CInternalMessage(InternalMsgType type, std::vector<string>* vectorOfStrings)
{
	InitMembers(type);
	m_vectorOfStrings = vectorOfStrings;
}


CInternalMessage::CInternalMessage(InternalMsgType type, std::vector<DWORD>* vectorOfWords)
{
	InitMembers(type);
	m_vectorOfWords = vectorOfWords;
}

CInternalMessage::CInternalMessage(InternalMsgType type, CProgressiveJackpotAnnouncement* pAnnouncement)
{
	InitMembers(type);
	SetData(pAnnouncement);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CProgressiveTotalizerAnnouncement* pAnnouncement)
{
	InitMembers(type);
	SetData(pAnnouncement);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CProgressiveClaimableAward* pClaimableAward)
{
	InitMembers(type);
	SetData(pClaimableAward);
}

CInternalMessage::CInternalMessage(InternalMsgType type, PokerHand* pPokerHand)
{
	InitMembers(type);
	SetData(pPokerHand);
}

CInternalMessage::CInternalMessage(InternalMsgType type, SubgameSessionParameters *pParams)
{
	InitMembers(type);
	SetData(pParams);
}

CInternalMessage::CInternalMessage(InternalMsgType type, SubgameSelectionResponse *pParams)
{
	InitMembers(type);
	SetData(pParams);
}

void CInternalMessage::SetData(WORD wordData, WORD additionalWordData)
{
	m_wordData = wordData;
	m_wordDataAdditional = additionalWordData;
}

void CInternalMessage::SetData(DWORD dWordData, DWORD additionalDWordData)
{
	m_dwordData = dWordData;
	m_dwordDataAdditional = additionalDWordData;
}

void CInternalMessage::SetDoubleData(double doubleData, double additionalDoubleData)
{
	m_doubleData = doubleData;
	m_doubleDataAdditional = additionalDoubleData;
}

void CInternalMessage::SetData(int64_t int64Data, int64_t additionalInt64Data)
{
	m_int64Data = int64Data;
	m_int64DataAdditional = additionalInt64Data;
}

void CInternalMessage::SetData(int32_t intData, int32_t additionalIntData)
{
	m_intData = intData;
	m_intDataAdditional = additionalIntData;
}

void CInternalMessage::SetData(IInternalMsgContent* content, InternalMsgContentType contentType)
{
	m_contentStore.SetContent(content, contentType);
}

// We need to get the InternalMsgContentType for the content object, so a NULL pointer cannot be passed.
// To make this impossible we've made the function parameter a reference and not a pointer.
void CInternalMessage::SetData(IInternalMsgContent& content)
{
	SetData(&content, content.GetContentType());
}

void CInternalMessage::SetData(CMeters *meters, WORD gameNumber, bool bMeterSnapshot)
{
	SetData((IInternalMsgContent*) meters, CMeters::MsgContentType());
	m_wordData = gameNumber;
	bMeterSnapshot ? m_wordDataAdditional = 1 : 0;
}

void CInternalMessage::SetData(PokerHand* pPokerHand)
{
	SetData(pPokerHand, PokerHand::MsgContentType());
}

PokerHand* CInternalMessage::GetPokerHand() const
{
	return static_cast<PokerHand*>(GetContent(PokerHand::MsgContentType()));
}

void CInternalMessage::SetData(CProgressiveJackpotAnnouncement* pAnnouncement)
{
	SetData(pAnnouncement, CProgressiveJackpotAnnouncement::MsgContentType());
}

CProgressiveJackpotAnnouncement* CInternalMessage::GetProgressiveJackpotAnnouncement() const
{
	return static_cast<CProgressiveJackpotAnnouncement*>(GetContent(CProgressiveJackpotAnnouncement::MsgContentType()));
}

void CInternalMessage::SetData(CProgressiveTotalizerAnnouncement* pAnnouncement)
{
	SetData(pAnnouncement, CProgressiveTotalizerAnnouncement::MsgContentType());
}

CProgressiveTotalizerAnnouncement* CInternalMessage::GetProgressiveTotalizerAnnouncement() const
{
	return static_cast<CProgressiveTotalizerAnnouncement*>(GetContent(CProgressiveTotalizerAnnouncement::MsgContentType()));
}

void CInternalMessage::SetData(CCampaignConfig* pConfig)
{
	SetData(pConfig, CCampaignConfig::MsgContentType());
}

void CInternalMessage::SetData(CSkinConfig* pConfig)
{
	SetData(pConfig, CSkinConfig::MsgContentType());
}

CCampaignConfig* CInternalMessage::GetCampaignConfig() const
{
	return static_cast<CCampaignConfig*>(GetContent(CCampaignConfig::MsgContentType()));
}

CSkinConfig* CInternalMessage::GetSkinConfig() const
{
	return static_cast<CSkinConfig*>(GetContent(CSkinConfig::MsgContentType()));
}

void CInternalMessage::SetData(CProgressiveClaimableAward* pClaimableAward)
{
	SetData(pClaimableAward, CProgressiveClaimableAward::MsgContentType());
}

CProgressiveClaimableAward* CInternalMessage::GetProgressiveClaimableAward() const
{
	return static_cast<CProgressiveClaimableAward*>(GetContent(CProgressiveClaimableAward::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CProgressiveLevelBroadcast* pLevelBroadcast)
{
	InitMembers(type);
	SetData(pLevelBroadcast);
}

void CInternalMessage::SetData(CProgressiveLevelBroadcast* pLevelBroadcast)
{
	SetData(pLevelBroadcast, CProgressiveLevelBroadcast::MsgContentType());
}

CProgressiveLevelBroadcast* CInternalMessage::GetProgressiveLevelBroadcast() const
{
	return static_cast<CProgressiveLevelBroadcast*>(GetContent(CProgressiveLevelBroadcast::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, ProgressiveLevelsInfo* pProgressiveLevelsInfo, CProgressiveAnnouncementsInfo* pAnnouncmentsConfiguration, CFirmwarePackage* pFirmwarePackage, CCampaignConfig* pCampaignConfig, CSkinConfig* pSkinConfig)
{
	InitMembers(type);
	SetData(pProgressiveLevelsInfo, ProgressiveLevelsInfo::MsgContentType());
	SetData(pAnnouncmentsConfiguration, CProgressiveAnnouncementsInfo::MsgContentType());
	SetData(pFirmwarePackage, CFirmwarePackage::MsgContentType());
	SetData(pCampaignConfig, CCampaignConfig::MsgContentType());
	SetData(pSkinConfig, CSkinConfig::MsgContentType());
}

ProgressiveLevelsInfo* CInternalMessage::GetProgressiveLevelsInfo() const
{
	return static_cast<ProgressiveLevelsInfo*>(GetContent(ProgressiveLevelsInfo::MsgContentType()));
}

CProgressiveAnnouncementsInfo* CInternalMessage::GetProgressiveAnnouncementsInfo() const
{
	return static_cast<CProgressiveAnnouncementsInfo*>(GetContent(CProgressiveAnnouncementsInfo::MsgContentType()));
}

void CInternalMessage::SetData(CProgressiveLevelForward* pProgressiveLevelForward)
{
	SetData(pProgressiveLevelForward, CProgressiveLevelForward::MsgContentType());
}

CProgressiveLevelForward* CInternalMessage::GetProgressiveLevelForward() const
{
	return static_cast<CProgressiveLevelForward*>(GetContent(CProgressiveLevelForward::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CProgressiveLevelForward* pProgressiveLevelForward)
{
	InitMembers(type);
	SetData(pProgressiveLevelForward);
}

CInternalMessage::CInternalMessage(InternalMsgType type, CProgressiveWinnerPick* pWinnerPick)
{
	InitMembers(type);
	SetData(pWinnerPick);
}

void CInternalMessage::SetData(CProgressiveWinnerPick* pWinnerPick)
{
	SetData(pWinnerPick, CProgressiveWinnerPick::MsgContentType());
}

CProgressiveWinnerPick* CInternalMessage::GetProgressiveWinnerPick() const
{
	return static_cast<CProgressiveWinnerPick*>(GetContent(CProgressiveWinnerPick::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CProgressiveWinnerPicked* pWinnerPicked)
{
	InitMembers(type);
	SetData(pWinnerPicked);
}

void CInternalMessage::SetData(CProgressiveWinnerPicked* pWinnerPicked)
{
	SetData(pWinnerPicked, CProgressiveWinnerPicked::MsgContentType());
}

CProgressiveWinnerPicked* CInternalMessage::GetProgressiveWinnerPicked() const
{
	return static_cast<CProgressiveWinnerPicked*>(GetContent(CProgressiveWinnerPicked::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CProgressiveBonusingAvailable *progresiveBonusingAvailable)
{
	InitMembers(type);
	SetData(progresiveBonusingAvailable);
}

void CInternalMessage::SetData(CProgressiveBonusingAvailable *progresiveBonusingAvailable)
{
	SetData(progresiveBonusingAvailable, CProgressiveBonusingAvailable::MsgContentType());
}

CProgressiveBonusingAvailable *CInternalMessage::GetProgressiveBonusingAvailable() const
{
	return static_cast<CProgressiveBonusingAvailable *>(GetContent(CProgressiveBonusingAvailable::MsgContentType()));
}

void CInternalMessage::SetData(ProgressivePatronSessionInfo *progressivePatronSessionInfo)
{
	SetData(progressivePatronSessionInfo, ProgressivePatronSessionInfo::MsgContentType());
}

ProgressivePatronSessionInfo *CInternalMessage::GetProgressivePatronSessionInfo() const
{
	return static_cast<ProgressivePatronSessionInfo *>(GetContent(ProgressivePatronSessionInfo::MsgContentType()));
}

void CInternalMessage::SetData(CProgressiveEligibleLevelsInfo *progressiveEligibleLevelsInfo)
{
	SetData(progressiveEligibleLevelsInfo, CProgressiveEligibleLevelsInfo::MsgContentType());
}

void CInternalMessage::SetData(SubgameSessionParameters *params)
{
	SetData(params, SubgameSessionParameters::MsgContentType());
}

void CInternalMessage::SetData(SubgameSelectionResponse *params)
{
	SetData(params, SubgameSelectionResponse::MsgContentType());
}

CProgressiveEligibleLevelsInfo *CInternalMessage::GetProgressiveEligibleLevelsInfo() const
{
	return static_cast<CProgressiveEligibleLevelsInfo *>(GetContent(CProgressiveEligibleLevelsInfo::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CUIConfig *uiConfig)
{
	InitMembers(type);
	SetData(uiConfig);
}

void CInternalMessage::SetData(CUIConfig *uiConfig)
{
	SetData(uiConfig, CUIConfig::MsgContentType());
}

CUIConfig *CInternalMessage::GetUIConfig() const
{
	return static_cast<CUIConfig*>(GetContent(CUIConfig::MsgContentType()));
}

CInternalMessage::~CInternalMessage()
{	
	if (m_bytesData != NULL)
		delete[] m_bytesData;

	m_contentStore.ClearAllContent();

	if (m_subcriptions != nullptr)
	{
		delete m_subcriptions;
	}

	if (m_vectorOfStrings != nullptr)
	{
		delete m_vectorOfStrings;
	}	

	if (m_vectorOfWords != nullptr)
	{
		delete m_vectorOfWords;
	}
}

void CInternalMessage::InitMembers(InternalMsgType msgType)
{
	m_msgType = msgType;
	m_wordData = 0;
	m_wordDataAdditional = 0;
	m_dwordData = 0;
	m_dwordDataAdditional = 0;
	m_doubleData = 0.0;
	m_doubleDataAdditional = 0.0;
	m_intData = 0;
	m_intDataAdditional = 0;
	m_int64Data = 0;
	m_int64DataAdditional = 0;
	m_size = 0;
	m_bytesData = NULL;
	m_subcriptions = nullptr;
	m_vectorOfStrings = nullptr;
	m_vectorOfWords = nullptr;
	m_createTickCount = 0;
	m_ttlTickCount = 0;

}

void CInternalMessage::SetData(CEGMAward* egmAward)
{
	SetData(egmAward, CEGMAward::MsgContentType());
}

CEGMAward* CInternalMessage::GetEGMAward() const
{
	return static_cast<CEGMAward*>(GetContent(CEGMAward::MsgContentType()));
}

void CInternalMessage::SetData(CProgressiveConfig* progressiveConfig)
{
	SetData(progressiveConfig, CProgressiveConfig::MsgContentType());
}

CProgressiveConfig* CInternalMessage::GetProgressiveConfig() const
{
	return static_cast<CProgressiveConfig*>(GetContent(CProgressiveConfig::MsgContentType()));
}

void CInternalMessage::SetData(CProgressiveConnectionInfo* progressiveConnectionInfo)
{
	SetData(progressiveConnectionInfo, CProgressiveConnectionInfo::MsgContentType());
}

CProgressiveConnectionInfo* CInternalMessage::GetProgressiveConnectionInfo() const
{
	return static_cast<CProgressiveConnectionInfo*>(GetContent(CProgressiveConnectionInfo::MsgContentType()));
}

void CInternalMessage::SetData(CGameEndData *gameEndData)
{
	SetData(gameEndData, CGameEndData::MsgContentType());
}

CGameEndData *CInternalMessage::GetGameEndData() const
{
	return static_cast<CGameEndData *>(GetContent(CGameEndData::MsgContentType()));
}


void CInternalMessage::SetData(CPayout* payout)
{
	SetData(payout, CPayout::MsgContentType());
}


CPayout* CInternalMessage::GetPayout() const
{
	return static_cast<CPayout *>(GetContent(CPayout::MsgContentType()));
}

void CInternalMessage::SetData(CProgressiveHost* progressiveHost)
{
	SetData(progressiveHost, CProgressiveHost::MsgContentType());
}

CProgressiveHost* CInternalMessage::GetProgressiveHost() const
{
	return static_cast<CProgressiveHost*>(GetContent(CProgressiveHost::MsgContentType()));
}

InternalMsgType CInternalMessage::GetMessageType() const
{
	return m_msgType;
}

WORD CInternalMessage::GetWordData(bool bAdditional) const
{
	return (bAdditional ? m_wordDataAdditional : m_wordData);
}

void CInternalMessage::SetData(CTicket *ticket)
{
	SetData(ticket, CTicket::MsgContentType());
}

void CInternalMessage::SetData(CConnection *connection)
{
	SetData(connection, CConnection::MsgContentType());
}

void CInternalMessage::SetData(CPlayer *player)
{
	SetData(player, CPlayer::MsgContentType());
}

void CInternalMessage::SetData(CPlayerInfo *playerInfo)
{
	SetData(playerInfo, CPlayerInfo::MsgContentType());
}

void CInternalMessage::SetData(CPlayerGlobalBalance *globalBalance)
{
	SetData(globalBalance, CPlayerGlobalBalance::MsgContentType());
}

void CInternalMessage::SetData(CBonusEvents *bonusEvents)
{
	SetData(bonusEvents, CBonusEvents::MsgContentType());
}

void CInternalMessage::SetData(CEmployee *employee)
{
	SetData(employee, CEmployee::MsgContentType());
}

void CInternalMessage::SetData(time_t *timeData)
{
	SetData(new CTimeContent(timeData), CTimeContent::MsgContentType());
}

void CInternalMessage::SetData(const BYTE* ptrBytes, size_t size)
{
	m_bytesData = ptrBytes;
	m_size = size;
}

void CInternalMessage::SetData(CTicketingInfo *ticketingInfo)
{
	SetData(ticketingInfo, CTicketingInfo::MsgContentType());
}

void CInternalMessage::SetData(CLogs *log)
{
	SetData(log, CLogs::MsgContentType());
}

void CInternalMessage::SetData(CPersonalBankerInfo *pbInfo)
{
	SetData(pbInfo, CPersonalBankerInfo::MsgContentType());
}

void CInternalMessage::SetData(CSubscription* subscription)
{
	SetData(subscription, CSubscription::MsgContentType());
}

void CInternalMessage::SetData(CPersonalBankerTransfer *pbTransferData)
{
	SetData(pbTransferData, CPersonalBankerTransfer::MsgContentType());
}

void CInternalMessage::SetData(CEGMConfig *EGMConfigData)
{
	SetData(EGMConfigData, CEGMConfig::MsgContentType());
}

void CInternalMessage::SetData(CJackpot *jackpot)
{
	SetData(jackpot, CJackpot::MsgContentType());
}

void CInternalMessage::SetData(CFlashStatus *flashStatus)
{
	SetData(flashStatus, CFlashStatus::MsgContentType());
}

void CInternalMessage::SetData(CGames *gamesData)
{
	SetData(gamesData, CGames::MsgContentType());
}

void CInternalMessage::SetData(CFileList *fileList)
{
	SetData(fileList, CFileList::MsgContentType());
}

void CInternalMessage::SetData(const string& stringData, const string& stringDataAdditional)
{
	m_strData = stringData;
	m_strDataAdditional = stringDataAdditional;
}

void CInternalMessage::SetTransactionMetersData(CMeters *transactionMeters)
{
	SetData(new CTransactionMetersContent(transactionMeters), CTransactionMetersContent::MsgContentType());
}

void CInternalMessage::SetThirdPartyMessage(CThirdPartyMessage *thirdPartyMessage)
{
	SetData(thirdPartyMessage, CThirdPartyMessage::MsgContentType());
}

void CInternalMessage::SetMessagePersist(CMessagePersist * messagePersist)
{
	SetData(messagePersist, CMessagePersist::MsgContentType());
}

void CInternalMessage::SetVideoModeConfig(CVideoModeConfig *videoModeConfig)
{
	SetData(videoModeConfig, CVideoModeConfig::MsgContentType());
}

void CInternalMessage::SetMediaWindowConfig(CMediaWindowConfig *mwConfig)
{
	SetData(mwConfig, CMediaWindowConfig::MsgContentType());
}

void CInternalMessage::SetMessageType(InternalMsgType type)
{
	m_msgType = type;
}

IInternalMsgContent* CInternalMessage::GetContent(InternalMsgContentType type) const
{
	return (IInternalMsgContent*) m_contentStore.GetContent(type);
}

CMessagePersist* CInternalMessage::GetMessagePersist() const
{
	return static_cast<CMessagePersist*>(GetContent(CMessagePersist::MsgContentType()));
}

CMeters *CInternalMessage::GetMetersData(WORD &gameNumber) const
{
	gameNumber = m_wordData;
	return static_cast<CMeters*>(GetContent(CMeters::MsgContentType()));
}

CTicket *CInternalMessage::GetTicketData() const
{
	return static_cast<CTicket*>(GetContent(CTicket::MsgContentType()));
}

CPersonalBankerInfo *CInternalMessage::GetPBInfoData() const
{
	return static_cast<CPersonalBankerInfo*>(GetContent(CPersonalBankerInfo::MsgContentType()));
}

CPersonalBankerTransfer *CInternalMessage::GetPBTransferData() const
{
	return static_cast<CPersonalBankerTransfer*>(GetContent(CPersonalBankerTransfer::MsgContentType()));
}

CGames *CInternalMessage::GetGamesData() const
{
	return static_cast<CGames*>(GetContent(CGames::MsgContentType()));
}

CConfigFields *CInternalMessage::GetConfigData() const
{
	return static_cast<CConfigFields*>(GetContent(CConfigFields::MsgContentType()));
}

std::string CInternalMessage::GetStringData(bool bAdditional) const
{
	return (bAdditional ? m_strDataAdditional : m_strData);
}

time_t *CInternalMessage::GetTimeData() const
{
	CTimeContent *content =
		static_cast<CTimeContent*>(GetContent(CTimeContent::MsgContentType()));
	return content->GetData();
}

CLogs *CInternalMessage::GetLogData() const
{
	return static_cast<CLogs*>(GetContent(CLogs::MsgContentType()));
}

CJackpot *CInternalMessage::GetJackpotData() const
{
	return static_cast<CJackpot*>(GetContent(CJackpot::MsgContentType()));
}

SubgameSessionParameters* CInternalMessage::GetSubgameSessionParameters() const
{
	return static_cast<SubgameSessionParameters *>(GetContent(SubgameSessionParameters::MsgContentType()));
}

SubgameSelectionResponse* CInternalMessage::GetSubgameSelectionResponse() const
{
	return static_cast<SubgameSelectionResponse *>(GetContent(SubgameSelectionResponse::MsgContentType()));
}

DWORD CInternalMessage::GetDWordData(bool bAdditional) const
{
	return (bAdditional ? m_dwordDataAdditional : m_dwordData);
}

double CInternalMessage::GetDoubleData(bool bAdditional) const
{
	return (bAdditional ? m_doubleDataAdditional : m_doubleData);
}

long  CInternalMessage::GetIntData(bool bAdditional) const
{
	return (bAdditional ? m_intDataAdditional : m_intData);
}

int64_t CInternalMessage::GetInt64Data(bool bAdditional) const
{
	return (bAdditional ? m_int64DataAdditional : m_int64Data);
}
const BYTE *CInternalMessage::GetBytesData(size_t &size) const
{
	size = m_size;
	return m_bytesData;
}

CConnection *CInternalMessage::GetConnectionData() const
{
	return static_cast<CConnection*>(GetContent(CConnection::MsgContentType()));
}

CPlayerInfo *CInternalMessage::GetPlayerInfoData() const
{
	return static_cast<CPlayerInfo*>(GetContent(CPlayerInfo::MsgContentType()));
}

CPlayerGlobalBalance *CInternalMessage::GetPlayerGlobalBalanceData() const
{
	return static_cast<CPlayerGlobalBalance*>(GetContent(CPlayerGlobalBalance::MsgContentType()));
}

CPlayer *CInternalMessage::GetPlayerData() const
{
	return static_cast<CPlayer*>(GetContent(CPlayer::MsgContentType()));
}

CFlashStatus *CInternalMessage::GetFlashStatus() const
{
	return static_cast<CFlashStatus*>(GetContent(CFlashStatus::MsgContentType()));
}

CBonusEvents *CInternalMessage::GetBonusEventsData() const
{
	return static_cast<CBonusEvents*>(GetContent(CBonusEvents::MsgContentType()));
}

CEmployee *CInternalMessage::GetEmployeeData() const
{
	return static_cast<CEmployee*>(GetContent(CEmployee::MsgContentType()));
}

CMeters *CInternalMessage::GetTransactionMetersData() const
{
	CTransactionMetersContent *content
		= static_cast<CTransactionMetersContent*>(GetContent(CTransactionMetersContent::MsgContentType()));

    return (content) ? content->GetData() : nullptr;
}

CTicketingInfo *CInternalMessage::GetTicketingInfoData() const
{
	return static_cast<CTicketingInfo*>(GetContent(CTicketingInfo::MsgContentType()));
}

CEGMConfig *CInternalMessage::GetEGMConfigData() const
{
	return static_cast<CEGMConfig*>(GetContent(CEGMConfig::MsgContentType()));
}

CFileList *CInternalMessage::GetFileList() const
{
	return static_cast<CFileList*>(GetContent(CFileList::MsgContentType()));
}

CSubscription* CInternalMessage::GetSubscriptionData() const
{
	return static_cast<CSubscription*>(GetContent(CSubscription::MsgContentType()));
}

CThirdPartyMessage *CInternalMessage::GetThirdPartyMessage() const
{
	return static_cast<CThirdPartyMessage*>(GetContent(CThirdPartyMessage::MsgContentType()));
}

CVideoModeConfig * CInternalMessage::GetVideoModeConfig() const
{
	return static_cast<CVideoModeConfig*>(GetContent(CVideoModeConfig::MsgContentType()));
}

std::vector<CSubscription>* CInternalMessage::GetSubscriptions() const
{
	return m_subcriptions;
}

std::vector<std::string>* CInternalMessage::GetVectorOfStrings() const
{
	return m_vectorOfStrings;
}

void CInternalMessage::SetVectorOfWords(std::vector<DWORD>* vectorOfWords)
{
	m_vectorOfWords = vectorOfWords;
}

std::vector<DWORD>* CInternalMessage::GetVectorOfWords() const
{
	return m_vectorOfWords;
}
CMediaWindowConfig *CInternalMessage::GetMediaWindowConfig() const
{
	return static_cast<CMediaWindowConfig*>(GetContent(CMediaWindowConfig::MsgContentType()));
}

void CInternalMessage::SetMessageTTL(DWORD ttl)
{
	m_createTickCount = CUtilities::GetTickCount();
	m_ttlTickCount = ttl;
}

bool CInternalMessage::IsTTLExpired() const
{
	bool retVal = false;
	if (m_ttlTickCount != 0)
	{
		retVal = CUtilities::GetTickCount() - m_createTickCount > m_ttlTickCount;
	}
	return retVal;
}

const string CInternalMessage::GetMessageTypeStr() const
{
    if (m_msgType >= _countof(InternalMsgTypeStr)) {
		return "ERROR: InternalMsgTypeStr has to be updated to match InternalMsgType";
    }

	string retVal(InternalMsgTypeStr[m_msgType]);

	if (NUMBER_OF_INT_MSG != _countof(InternalMsgTypeStr))
	{
		retVal = "ERROR: InternalMsgTypeStr has to be updated to match InternalMsgType";
	}

    return retVal;
}

CInternalMessage::CInternalMessage(InternalMsgType type, CPairingRequest *pPairingRequestData)
{
	InitMembers(type);
	SetData(pPairingRequestData);
}

void CInternalMessage::SetData(CPairingRequest *pairingRequestData)
{
	SetData(pairingRequestData, CPairingRequest::MsgContentType());
}

CPairingRequest *CInternalMessage::GetPairingRequestData() const
{
	return static_cast<CPairingRequest *>(GetContent(CPairingRequest::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CPairingSettingsEvent *pPairingSettingsEvent)
{
	InitMembers(type);
	SetData(pPairingSettingsEvent);
}

void CInternalMessage::SetData(CPairingSettingsEvent *pairingSettingsEventData)
{
	SetData(pairingSettingsEventData, CPairingSettingsEvent::MsgContentType());
}

CPairingSettingsEvent *CInternalMessage::GetPairingSettingsEventData() const
{
	return static_cast<CPairingSettingsEvent *>(GetContent(CPairingSettingsEvent::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CPairingEvent *pPairingEvent)
{
	InitMembers(type);
	SetData(pPairingEvent);
}

void CInternalMessage::SetData(CPairingEvent *pairingEventData)
{
	SetData(pairingEventData, CPairingEvent::MsgContentType());
}

CPairingEvent *CInternalMessage::GetPairingEventData() const
{
	return static_cast<CPairingEvent *>(GetContent(CPairingEvent::MsgContentType()));
}


CInternalMessage::CInternalMessage(InternalMsgType type, CPairingStatusEvent *pPairingStatusEvent)
{
	InitMembers(type);
	SetData(pPairingStatusEvent);
}

void CInternalMessage::SetData(CPairingStatusEvent *pairingStatusEventData)
{
	SetData(pairingStatusEventData, CPairingStatusEvent::MsgContentType());
}

CPairingStatusEvent *CInternalMessage::GetPairingStatusEventData() const
{
	return static_cast<CPairingStatusEvent *>(GetContent(CPairingStatusEvent::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CProximitySettingsEvent *pProximitySettingsEvent)
{
	InitMembers(type);
	SetData(pProximitySettingsEvent);
}

void CInternalMessage::SetData(CProximitySettingsEvent *proximitySettingsEventData)
{
	SetData(proximitySettingsEventData, CProximitySettingsEvent::MsgContentType());
}

CProximitySettingsEvent *CInternalMessage::GetProximitySettingsEventData() const
{
	return static_cast<CProximitySettingsEvent *>(GetContent(CProximitySettingsEvent::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CProximityStatusEvent *pProximityStatusEvent)
{
	InitMembers(type);
	SetData(pProximityStatusEvent);
}

void CInternalMessage::SetData(CProximityStatusEvent *proximityStatusEventData)
{
	SetData(proximityStatusEventData, CProximityStatusEvent::MsgContentType());
}

CProximityStatusEvent *CInternalMessage::GetProximityStatusEventData() const
{
	return static_cast<CProximityStatusEvent *>(GetContent(CProximityStatusEvent::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CCardChangeRequest *pCardChangeRequest)
{
	InitMembers(type);
	SetData(pCardChangeRequest);
}

void CInternalMessage::SetData(CCardChangeRequest *pCardChangeRequestData)
{
	SetData(pCardChangeRequestData, CCardChangeRequest::MsgContentType());
}

CCardChangeRequest *CInternalMessage::GetCardChangeRequestData() const
{
	return static_cast<CCardChangeRequest *>(GetContent(CCardChangeRequest::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CCardChangeEvent *pCardChangeEvent)
{
	InitMembers(type);
	SetData(pCardChangeEvent);
}

void CInternalMessage::SetData(CCardChangeEvent *pCardChangeEventData)
{
	SetData(pCardChangeEventData, CCardChangeEvent::MsgContentType());
}

CCardChangeEvent *CInternalMessage::GetCardChangeEventData() const
{
	return static_cast<CCardChangeEvent *>(GetContent(CCardChangeEvent::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CPlayerChangeEvent *pPlayerChangeEvent)
{
	InitMembers(type);
	SetData(pPlayerChangeEvent);
}

void CInternalMessage::SetData(CPlayerChangeEvent *pPlayerChangeEventData)
{
	SetData(pPlayerChangeEventData, CPlayerChangeEvent::MsgContentType());
}

CPlayerChangeEvent *CInternalMessage::GetPlayerChangeEventData() const
{
	return static_cast<CPlayerChangeEvent *>(GetContent(CPlayerChangeEvent::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CPlayerSessionChangeEvent *pPlayerSessionChangeEvent)
{
	InitMembers(type);
	SetData(pPlayerSessionChangeEvent);
}

void CInternalMessage::SetData(CPlayerSessionChangeEvent *pPlayerSessionChangeEventData)
{
	SetData(pPlayerSessionChangeEventData, CPlayerSessionChangeEvent::MsgContentType());
}

CPlayerSessionChangeEvent *CInternalMessage::GetPlayerSessionChangeEventData() const
{
	return static_cast<CPlayerSessionChangeEvent *>(GetContent(CPlayerSessionChangeEvent::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CFundsTransferBalanceRequest *pCreditTotalsRequest)
{
	InitMembers(type);
	SetData(pCreditTotalsRequest);
}

void CInternalMessage::SetData(CFundsTransferBalanceRequest *pCreditTotalsRequestData)
{
	SetData(pCreditTotalsRequestData, CFundsTransferBalanceRequest::MsgContentType());
}

CFundsTransferBalanceRequest *CInternalMessage::GetFundsTransferBalanceRequestData() const
{
	return static_cast<CFundsTransferBalanceRequest *>(GetContent(CFundsTransferBalanceRequest::MsgContentType()));
}


CInternalMessage::CInternalMessage(InternalMsgType type, CFundsTransferBalanceEvent *pFundsTransferBalanceEvent)
{
	InitMembers(type);
	SetData(pFundsTransferBalanceEvent);
}
	
void CInternalMessage::SetData(CFundsTransferBalanceEvent *pFundsTransferBalanceEventData)
{
	SetData(pFundsTransferBalanceEventData, CFundsTransferBalanceEvent::MsgContentType());
}
	
CFundsTransferBalanceEvent *CInternalMessage::GetFundsTransferBalanceEventData() const
{
	return static_cast<CFundsTransferBalanceEvent *>(GetContent(CFundsTransferBalanceEvent::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CFundsTransferRequest *pFundsTransferRequest)
{
	InitMembers(type);
	SetData(pFundsTransferRequest);
}
	
void CInternalMessage::SetData(CFundsTransferRequest *pFundsTransferRequestData)
{
	SetData(pFundsTransferRequestData, CFundsTransferRequest::MsgContentType());
}
	
CFundsTransferRequest *CInternalMessage::GetFundsTransferRequestData() const
{
	return static_cast<CFundsTransferRequest *>(GetContent(CFundsTransferRequest::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CFundsTransferEvent *pFundsTransferEvent)
{
	InitMembers(type);
	SetData(pFundsTransferEvent);
}
		
void CInternalMessage::SetData(CFundsTransferEvent *pFundsTransferEventData)
{
	SetData(pFundsTransferEventData, CFundsTransferEvent::MsgContentType());
}
		
CFundsTransferEvent *CInternalMessage::GetFundsTransferEventData() const
{
	return static_cast<CFundsTransferEvent *>(GetContent(CFundsTransferEvent::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CPromoGameRequest* promoGameRequest)
{
	InitMembers(type);
	SetData(promoGameRequest);
}

void CInternalMessage::SetData(CPromoGameRequest* promoGameRequest)
{
	SetData(promoGameRequest, CPromoGameRequest::MsgContentType());
}

CPromoGameRequest* CInternalMessage::GetPromoGameRequest() const
{
	return static_cast<CPromoGameRequest*>(GetContent(CPromoGameRequest::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CFirmwarePackage* firmwarePackage)
{
	InitMembers(type);
	SetData(firmwarePackage);
}

void CInternalMessage::SetData(CFirmwarePackage* firmwarePackage)
{
	SetData(firmwarePackage, CFirmwarePackage::MsgContentType());
}

CFirmwarePackage* CInternalMessage::GetFirmwarePackage() const
{
	return static_cast<CFirmwarePackage*>(GetContent(CFirmwarePackage::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CConfigFilePackage* configFilePackage)
{
	InitMembers(type);
	SetData(configFilePackage);
}

void CInternalMessage::SetData(CConfigFilePackage* configFilePackage)
{
	SetData(configFilePackage, CConfigFilePackage::MsgContentType());
}

CConfigFilePackage* CInternalMessage::GetConfigFilePackage() const
{
	return static_cast<CConfigFilePackage*>(GetContent(CConfigFilePackage::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CCampaignPackage* campaignPackage)
{
	InitMembers(type);
	SetData(campaignPackage);
}

void CInternalMessage::SetData(CCampaignPackage* campaignPackage)
{
	SetData(campaignPackage, CCampaignPackage::MsgContentType());
}

CCampaignPackage* CInternalMessage::GetCampaignPackage() const
{
	return static_cast<CCampaignPackage*>(GetContent(CCampaignPackage::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CSkinPackage* skinPackage)
{
	InitMembers(type);
	SetData(skinPackage);
}

void CInternalMessage::SetData(CSkinPackage* skinPackage)
{
	SetData(skinPackage, CSkinPackage::MsgContentType());
}

CSkinPackage* CInternalMessage::GetSkinPackage() const
{
	return static_cast<CSkinPackage*>(GetContent(CSkinPackage::MsgContentType()));
}

CInternalMessage::CInternalMessage(InternalMsgType type, CPromoGameAcceptanceStatus* promoGameAcceptanceStatus)
{
	InitMembers(type);
	SetData(promoGameAcceptanceStatus);
}

void CInternalMessage::SetData(CPromoGameAcceptanceStatus* promoGameAcceptanceStatus)
{
	SetData(promoGameAcceptanceStatus, CPromoGameAcceptanceStatus::MsgContentType());
}

CPromoGameAcceptanceStatus* CInternalMessage::GetPromoGameAcceptanceStatus() const
{
	return static_cast<CPromoGameAcceptanceStatus*>(GetContent(CPromoGameAcceptanceStatus::MsgContentType()));
}
