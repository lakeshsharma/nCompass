#pragma once

#include "QueueItem.h"
#include "Meters.h"
#include "Ticket.h"
#include "GameEndData.h"
#include "Games.h"
#include "Config/ConfigFields.h"
#include "Connection.h"
#include "Player.h"
#include "PlayerGlobalBalance.h"
#include "Employee.h"
#include "BonusEvent.h"
#include "GlobalDefs.h"
#include "TicketingInfo.h"
#include "PersonalBankerInfo.h"
#include "PersonalBankerTransfer.h"
#include "UI/Logs.h"
#include "Config/EGMConfig.h"
#include "Jackpot.h"
#include "FlashStatus.h"
#include "FileList.h"
#include "ContentDeliverySystem/ThirdPartyMessage.h"
#include "MessagePersist.h"
#include "VideoModeConfig.h"
#include "Config/MediaWindowConfig.h"
#include "Config/UIConfig.h"
#include "InternalMsgContent.h"
#include "ProgressiveProtocol/ProgressiveJackpotAnnouncement.h"
#include "ProgressiveProtocol/ProgressiveTotalizerAnnouncement.h"
#include "ProgressiveProtocol/EGMAward.h"
#include "ProgressiveProtocol/ProgressiveEligibleLevelsInfo.h"
#include "ProgressiveProtocol/PromoGameRequest.h"
#include "ProgressiveProtocol/PromoGameAcceptanceStatus.h"
#include "ProgressiveProtocol/FirmwarePackage.h"
#include "ProgressiveProtocol/ConfigFilePackage.h"
#include "ProgressiveProtocol/CampaignPackage.h"
#include "ProgressiveProtocol/SkinPackage.h"
#include "SubgameSessionParameters.h"
#include "PollerProtocol/MobileConnectProtocol/PairingSettingsEvent.h"
#include "PollerProtocol/MobileConnectProtocol/PairingRequest.h"
#include "PollerProtocol/MobileConnectProtocol/PairingEvent.h"
#include "PollerProtocol/MobileConnectProtocol/PairingStatusEvent.h"
#include "PollerProtocol/MobileConnectProtocol/CardChangeRequest.h"
#include "PollerProtocol/MobileConnectProtocol/CardChangeEvent.h"
#include "PollerProtocol/MobileConnectProtocol/PlayerChangeEvent.h"
#include "PollerProtocol/MobileConnectProtocol/PlayerSessionChangeEvent.h"
#include "PollerProtocol/MobileConnectProtocol/CreditTotalsRequest.h"
#include "PollerProtocol/MobileConnectProtocol/CreditTotalsEvent.h"
#include "PollerProtocol/MobileConnectProtocol/FundTransferRequest.h"
#include "PollerProtocol/MobileConnectProtocol/FundsTransferEvent.h"
#include "PollerProtocol/PollerSubgameSelectionResponseMessage.h"
#include "PollerProtocol/MobileConnectProtocol/ProximitySettingsEvent.h"
#include "PollerProtocol/MobileConnectProtocol/ProximityStatusEvent.h"
#include "ProgressiveProtocol/CampaignConfig.h"
#include "ProgressiveProtocol/SkinConfig.h"
#include <Payout.h>

// How to add a new content class to CInternalMessage
// 1. Create a new content type by adding a new enum to InternalMsgContentType defined in InternalMsgContent.h
// 2a. if the class is not already part of CInternalMessage -> add to the inheritance list of the new content class, 'public CInternalMsgType<new content type>'
//     See CMeters as an example
// 2b. if the class is already part of CInternalMessage or cannot be touched -> use the CInternalMsgContentWrapper template to define a new content class
//     See CTransacitonMetersContent as an example
//
// You can set and access content using
//
//   CInternalMessage::CInternalMessage(InternalMsgType type, IInternalMsgContent* content, InternalMsgContentType contentType);
//   CInternalMessage::CInternalMessage(InternalMsgType type, IInternalMsgContent& content);
//   void CInternalMessage::SetData( IInternalMsgContent* content, InternalMsgContentType contentType )
//   void CInternalMessage::SetData( IInternalMsgContent& content )
//   IInternalMsgContent* CInternalMessage::GetContent(InternalMsgContentType type)
//
// You can also get the contentType with
//   the virtual method   -> content->GetContentType()
//   or the static method -> NewContentClass::MsgContentType()

enum InternalMsgType
{
	INT_MSG_NONE,
	INT_MSG_ACCEPT_VIDEO_MODE_CONFIG,
	INT_MSG_ALLOW_TICKET_PRINTED,
	INT_MSG_ATTENDANT_PAID_PAYOUT,
	INT_MSG_ATTRACTOR_SEQUENCE_COMPLETE,
	INT_MSG_AUTHORIZE_PIN,
	INT_MSG_AUTO_TRANSFER_CANCELLED,
	INT_MSG_BAD_CARD_READ,
	INT_MSG_BILL_METERS_COMPLETE,
	INT_MSG_BILL_VALIDATOR_DISABLE,
	INT_MSG_BILL_VALIDATOR_ENABLE,
	INT_MSG_BUTTON_ACTIVITY,
	INT_MSG_CARD_REMOVED,
	INT_MSG_CASINO_CHALLENGE_AWARD,
	INT_MSG_CASINO_CHALLENGE_END,
	INT_MSG_CASINO_CHALLENGE_COUNTER,
	INT_MSG_CASINO_CHALLENGE_FIVE_MINUTE_END,
	INT_MSG_CASINO_CHALLENGE_INCREMENT,
	INT_MSG_CASINO_CHALLENGE_STOP_PROGRESS_FROM_UI,
	INT_MSG_CASINO_CHALLENGE_TRIGGER,
	INT_MSG_CDM_ANNOUNCEMENT,
	INT_MSG_CHANGE_EGM_TYPE,
	INT_MSG_CHANGE_LIGHT_ON,
	INT_MSG_CHANGE_LIGHT_OFF,
	INT_MSG_CHANGE_SLOTMASTID,
	INT_MSG_CHECK_EMPLOYEE_PIN,
	INT_MSG_CHECK_OTHER_PBT_AMOUNT,
	INT_MSG_CARD_INFO,
	INT_MSG_CARDED_SESSION_IN_PROGRESS,
	INT_MSG_TRANSFER_PENDING,
	INT_MSG_CLEAR_PBT,
	INT_MSG_CLEAR_PBT_PROBE,
	INT_MSG_CLEAR_PERSISTENT_MEMORY,
	INT_MSG_CLOSE_PTE_LEADERBOARD,
	INT_MSG_DEFAULT_DATA_DIRECTORY_UPDATED,
	INT_MSG_DISPLAY_ACCESSING_ACCOUNT,
	INT_MSG_DISPLAY_ACCOUNT_BALANCE,
	INT_MSG_DISPLAY_ATTRACTOR,
	INT_MSG_SET_CARD_BEZEL,
	INT_MSG_DISPLAY_BANNED_PLAYER,
	INT_MSG_DISPLAY_DOWNLOAD_PENDING,
	INT_MSG_DISPLAY_INFORMATION,
	INT_MSG_DISPLAY_LOSS_LIMIT_BALANCE,
	INT_MSG_DISPLAY_PTE_ANTICIPATION,
	INT_MSG_DISPLAY_PTE_AWARD,
	INT_MSG_DISPLAY_PTE_ENDCOUNTDOWN,
	INT_MSG_DISPLAY_PTE_LEADERBOARD,
	INT_MSG_DISPLAY_PTE_LEADERBOARD_PLAYER_INITIATED,
	INT_MSG_DISPLAY_PTE_LEVELUP,
	INT_MSG_DISPLAY_PTE_RESULTS,
	INT_MSG_DISPLAY_UJP,
	INT_MSG_DISPLAY_UJP_INVALID_PIN,
	INT_MSG_DISPLAY_AUTOCARDOUT,
	INT_MSG_AUTO_CARDOUT_TIMED_OUT,
	INT_MSG_AUTO_CARDOUT_END_SESSION,
	INT_MSG_AUTO_CARDOUT_CONTINUE_SESSION,
	INT_MSG_DISPLAY_EXTENDED_BUCKETS,
	INT_MSG_DOLLAR_BUTTON_SUCCESS,
	INT_MSG_DOLLAR_BUTTON_ERROR,
	INT_MSG_DOOR_CLOSED,
	INT_MSG_DOOR_OPEN,
	INT_MSG_DOWNLOAD_CANCELED,
	INT_MSG_EFT_SEED,
	INT_MSG_EGM_AWARD, // Paltronics/CEGMAward.
	INT_MSG_EGM_AWARD_STATUS,//PROMO GAME AWARD status.
	INT_MSG_CUMULATIVE_PROGRESSIVE_WIN,
	INT_MSG_EGM_PROTOCOL_VERSION,
	INT_MSG_EGM_STATE,
	INT_MSG_EGM_SUPPORTS_LP_7B,
	INT_MSG_EMPLOYEE_CARD_IN,
	INT_MSG_EMPLOYEE_GOODBYE,
	INT_MSG_EMPLOYEE_RESPONSE,
	INT_MSG_END_CAPTURING_METERS,
	INT_MSG_ENABLE_GAME,
	INT_MSG_FINISHED_GAMESLIST_FIRSTPASS,
	INT_MSG_FINISHED_OFFLINE_PASS,
	INT_MSG_FIRST_PLAY,
	INT_MSG_FORCE_CARD_OUT,
	INT_MSG_GAME_COMM_ERROR,
	INT_MSG_GAME_COMM_STATUS,
	INT_MSG_GAME_DATA,
	INT_MSG_GCF_STARTED,
	INT_MSG_GET_ACCOUNT_BALANCE,
	INT_MSG_PERFORM_BATTERY_TEST,
	INT_MSG_GET_BILL_METERS,
	INT_MSG_GET_BONUS_METERS,
	INT_MSG_GET_CONFIG_FROM_CACHE,
	INT_MSG_GET_DENOM,
	INT_MSG_GET_EGM_CAPS,
	INT_MSG_GET_EGM_DISPLAY,
	INT_MSG_GET_EGM_SETTINGS,
	INT_MSG_GET_GAME_INFO,
	INT_MSG_GET_GAME_LIST,
	INT_MSG_GET_PROGRESSIVE_GAMES_INFO,
	INT_MSG_GET_IP,
	INT_MSG_GET_JACKPOT_INFO,
	INT_MSG_GET_LOSS_LIMIT_BALANCE_UPDATE,
	INT_MSG_GET_METERS,
	INT_MSG_GET_NETWORK_INFO,
	INT_MSG_GET_PB_BALANCES,
	INT_MSG_GET_PROGRESSIVE_HOPPER_PAID_INFO,
	INT_MSG_GET_PROGRESSIVE_WIN_INFO,
	INT_MSG_GET_STATUS,
	INT_MSG_GET_SUBGAME_METERS,
	INT_MSG_GET_TICKET_STATUS,
	INT_MSG_GET_VIDEO_MODE_CONFIG,
	INT_MSG_HANDPAY_RESET,
	INT_MSG_HEARTBEAT,
	INT_MSG_HIDE_SERVICE_WINDOW,
	INT_MSG_IN_EVENT_TRIGGER_REJECTED,
	INT_MSG_IN_TRIGGERED_EVENT_TRIGGER_REJECTED,
	INT_MSG_INC_BILL_METER,
	INT_MSG_EMPLOYEE_PIN_RESULT,
	INT_MSG_JACKPOT_ANNOUNCEMENT,
	INT_MSG_JACKPOT_HIT,
	INT_MSG_JACKPOT_INFO,
	INT_MSG_JACKPOT_STORED,
	INT_MSG_LCD_DETECT,
	INT_MSG_LOAD_EVENT_XML,
	INT_MSG_LOAD_EVENT_PACKAGE,
	INT_MSG_LOSS_LIMIT_BALANCE_UPDATE,
	INT_MSG_OS_UPGRADE,
	INT_MSG_MAC_ADDR,
	INT_MSG_MACHINE_MOVE_BEGIN,
	INT_MSG_MACHINE_MOVE_COMPLETE,
	INT_MSG_MACHINE_OFFLINE,
	INT_MSG_MACHINE_ONLINE,
	INT_MSG_METER_CHANGE_ALLOW,
	INT_MSG_METER_CHANGE_ALLOW_SENT,
	INT_MSG_METER_CHANGE_CANCEL,
	INT_MSG_METER_CHANGE_COMPLETE,
	INT_MSG_METER_CHANGE_REQUEST,
	INT_MSG_NOTIFY_GAME_END,
	INT_MSG_NOTIFY_GAME_START,
	INT_MSG_NOTIFY_RESEND_EMPTY,
	INT_MSG_NVRAM_VALID,
	INT_MSG_OFFLINE_COIN_IN,
	INT_MSG_PB_ACKED,
	INT_MSG_PB_CANCEL_SESSION,
	INT_MSG_PB_COMPLETE,
	INT_MSG_PB_PIN_RESET_RESULT,
	INT_MSG_PB_CHECK_DENOM,
	INT_MSG_PB_CHECK_PIN,
	INT_MSG_PB_INFO,
	INT_MSG_PB_NCEPOUT_FAIL,
	INT_MSG_PB_REQUEST_TRANSFER,
	INT_MSG_PB_RESPONSE,
	INT_MSG_PB_STATUS,
	INT_MSG_PB_STORED,
	INT_MSG_PB_TRANSFER,
	INT_MSG_PB_RESTORE_SESSION,
	INT_MSG_PB_SET_NEW_PIN,
	INT_MSG_PLAY_CONGRATS,
	INT_MSG_PLAYER_BONUS_EVENTS,
	INT_MSG_PLAYER_DATA,
	INT_MSG_PLAYER_DATA_PROCESSED,
	INT_MSG_PLAYER_GLOBAL_BALANCE,
	INT_MSG_PLAYER_GOODBYE,
	INT_MSG_PLAYER_TRACKING_DURATION,
	INT_MSG_PLAYER_UPDATE,
	INT_MSG_PLAYER_UPDATE_TIME,
	INT_MSG_CONNECTIVITY_STATUS,
	INT_MSG_POLLERID,
	INT_MSG_POLLER_LIST_FULL,
	INT_MSG_POLLER_LIST_OK,
	INT_MSG_POLLER_LIST_WARNING,
	INT_MSG_POLLER_PACKET_SENT_VERIFY_ERRORS,
	INT_MSG_PREVIEW_TASKBAR_SIZE,
	INT_MSG_PREVIEW_SW_LOCATION,
	INT_MSG_PREVIEW_SW_SIZE,
	INT_MSG_PRIZE_LOCK_CLEAR,
	INT_MSG_PROBE_PBT,
	INT_MSG_PROBE_PBT_RESULT,
	INT_MSG_PROCESS_TICKET_IN,
	INT_MSG_PROGRESSIVE_BONUSING_AVAILABLE,
	INT_MSG_PROGRESSIVE_DASHBOARD_STATUS,
	INT_MSG_PROGRESSIVE_JACKPOT_ANNOUNCEMENT,
	INT_MSG_PROGRESSIVE_TOTALIZER_ANNOUNCEMENT,
	INT_MSG_PROGRESSIVE_CLAIMABLE_AWARD,
	INT_MSG_PROGRESSIVE_EXTERNAL_COMMUNICATIONS_STATUS,
	INT_MSG_PROGRESSIVE_FORWARD,
	INT_MSG_PROGRESSIVE_HOPPER_PAID_HIT,
	INT_MSG_PROGRESSIVE_HOPPER_PAID_INFO,
	INT_MSG_PROGRESSIVE_WIN_HIT,
	INT_MSG_PROGRESSIVE_WIN_INFO,
	INT_MSG_PROGRESSIVE_WIN,
	INT_MSG_PROGRESSIVE_CONFIGURATION,
	INT_MSG_PROGRESSIVE_LEVEL_BROADCAST,
	INT_MSG_PROGRESSIVE_LEVEL_FORWARD,
	INT_MSG_PROGRESSIVE_SCHEDULE_END,
	INT_MSG_PROGRESSIVE_WINNER_PICK,
	INT_MSG_PROGRESSIVE_WINNER_PICKED,
	INT_MSG_PROGRESSIVE_ACCOUNTING_ONLY,
	INT_MSG_PROGRESSIVE_PROMO_GAME,
	INT_MSG_PROGRESSIVE_PRIZE,
	INT_MSG_PROCESS_BUTTON_PRESS,
	INT_MSG_PROCESS_BUTTON_PRESS_FOR_SCREEN,
	INT_MSG_PROMOTION_DURATION,
	INT_MSG_QUERY_POKER_HAND,
	INT_MSG_RECONNECT,
	INT_MSG_RESET_JACKPOT,
	INT_MSG_RESTART_SERVICEWINDOW,
	INT_MSG_RICOCHET_TRIGGER,
	INT_MSG_RICOCHET_AWARD,
	INT_MSG_SCF_PROTOCOL_VERSION,
	INT_MSG_SCF_CARDOUT,
	INT_MSG_SEND_CARD_IN,
	INT_MSG_SEND_CARD_OUT,
	INT_MSG_SEND_EXTERNAL_BONUS,
	INT_MSG_SEND_HOPPER_FILL,
	INT_MSG_SEND_MAINTENANCE,
	INT_MSG_SEND_PRINTER_MAINTENANCE,
	INT_MSG_SEND_PROGRESSIVE_LEVEL_FORWARD,
	INT_MSG_SEND_REPAIR_CODE,
	INT_MSG_SEND_STACKER_IN,
	INT_MSG_SEND_STACKER_OUT,
	INT_MSG_SEND_TICKET,
	INT_MSG_SEND_TILT,
	INT_MSG_SEND_WITNESS,
	INT_MSG_SERVICE_WINDOW_SHOW,
	INT_MSG_SERVICE_WINDOW_STATE,
	INT_MSG_SET_ALLOW_FLOOR_LOCKOUT,
	INT_MSG_SET_ALTERNATE_DNS,
	INT_MSG_SET_BACKLIGHT,
	INT_MSG_SET_TASKBAR,
	INT_MSG_SET_CDM_WINDOW,
	INT_MSG_SET_COMM_STATUS,
	INT_MSG_SET_CONNECTION_DATA,
	INT_MSG_SET_POLLER_SETTINGS,
	INT_MSG_SET_IP_ADDRESS,
	INT_MSG_SET_CURRENT_GAME_DENOM,
	INT_MSG_SET_DENOM,
	INT_MSG_SET_DEFAULT_GATEWAY,
	INT_MSG_SET_DESKTOP_ORIENTATION_TYPE,
	INT_MSG_SET_DHCP_SETTING,
	INT_MSG_SET_DISPLAY_MESSAGE,
	INT_MSG_SET_EGM_SETTINGS,
	INT_MSG_SET_FIRMWARE_UPGRADE_AVAILABLE,
	INT_MSG_SET_GAME_LIST,
	INT_MSG_SET_ENABLED_GAMES,
	INT_MSG_SET_GLOBAL_PASSWORD,
	INT_MSG_SET_GLOBAL_BALANCE,
	INT_MSG_SET_IMAGE_CHECKSUM,
	INT_MSG_SET_IP,
	INT_MSG_SET_JACKPOT_RESET,
	INT_MSG_SET_METER,
	INT_MSG_SET_METERS,
	INT_MSG_SET_MOBILE_CONNECT_COMMUNICATION_SUPPORT,
	INT_MSG_SET_MULTICAST_DATA,
	INT_MSG_SET_ONE_LINK_PROGRESSIVES_SEN_OPTION_SUPPORT,
	INT_MSG_SET_ONE_LINK_PROGRESSIVES_SUPPORT,
	INT_MSG_SET_SLOT_NUMBER,
	INT_MSG_SET_PAR,
	INT_MSG_SET_PB_BALANCES,
	INT_MSG_SET_PLAYER_INFO,
	INT_MSG_SET_PREFERRED_DNS,
	INT_MSG_SET_PROGRESSIVE_CONFIG,
	INT_MSG_SET_PROGRESSIVE_CONNECTION_INFO,
	INT_MSG_SET_PROGRESSIVE_DOWN,
	INT_MSG_SET_PROGRESSIVE_GAMES_INFO,
	INT_MSG_SET_PROGRESSIVE_HOST,
	INT_MSG_SET_PROGRESSIVE_HIT_SENT,
	INT_MSG_SET_PROGRESSIVE_PAID_SENT,
	INT_MSG_SET_SERVICE_WINDOW,
	INT_MSG_SET_SERVICE_WINDOW_AFTER_SHOW_OR_HIDE,
	INT_MSG_SET_SLOTMAST_REVISION,
	INT_MSG_SET_SLOTMASTID,
	INT_MSG_SET_STATUS,
	INT_MSG_SET_SYSTEM_CONFIG,
	INT_MSG_SET_SUBNET_MASK,
	INT_MSG_SET_TICKET_EXP_DAYS,
	INT_MSG_SET_TICKETING_INFO,
	INT_MSG_SET_TIME,
	INT_MSG_SET_TOUCH,
	INT_MSG_SET_VERSION,
	INT_MSG_SET_VIDEO_MODE_CONFIG,
	INT_MSG_SET_VIDEO_OFFSETS,
	INT_MSG_SET_VOLUME,
	INT_MSG_SHOW_ANNIVERSARY,
	INT_MSG_SHOW_ATTRACTOR,
	INT_MSG_SHOW_BIRTHDAY,
	INT_MSG_SHOW_BIRTHDAY_AND_ANNIVERSARY,
	INT_MSG_SHOW_CREDIT_TRANSFER_ENTRY_SCREEN,
	INT_MSG_SHOW_NON_CREDIT_TRANSFER_ENTRY_SCREEN,
	INT_MSG_SHOW_OTHER_BUTTON_ERROR,
	INT_MSG_SHOW_SERVICE_WINDOW,
	INT_MSG_SHUTDOWN,
	INT_MSG_SNAPSHOT_PROGRESS,
	INT_MSG_SPLASHDOWN_START,
	INT_MSG_SPLASHDOWN_START_TIMER,
	INT_MSG_SPLASHDOWN_AWARD,
	INT_MSG_SPLASHDOWN_END,
	INT_MSG_SPLASHDOWN_WINNER_END,
	INT_MSG_SPLASHDOWN_END_TIMER,
	INT_MSG_START_CAPTURING_METERS,
	INT_MSG_START_DEBUG_LOG,
	INT_MSG_START_UI,
	INT_MSG_START_VIDEO,
	INT_MSG_STATUS_REBOOTING,
	INT_MSG_STOP_DEBUG_LOG,
	INT_MSG_STOP_UI,
	INT_MSG_STOP_VIDEO,
	INT_MSG_SUBGAME_SELECTION,
	INT_MSG_SYSTEM_ONLINE,
	INT_MSG_SUBSCRIBE_REQUEST,
	INT_MSG_UNSUBSCRIBE_REQUEST,
	INT_MSG_SUBSCRIPTION_UPDATES,
	INT_MSG_TERMINATE_BROWSER,
	INT_MSG_TICKET_INSERTED,
	INT_MSG_TICKET_OUT_PENDING_COUNT,
	INT_MSG_TICKET_RESPONSE,
	INT_MSG_TICKET_RESPONSE_RECEIVED,
	INT_MSG_TICKET_STATUS,
	INT_MSG_TICKET_STORED,
	INT_MSG_TRANSACTION_AUTO_UPLOAD_CANCELED,
	INT_MSG_TRANSACTION_BILL_ACCEPTED,
	INT_MSG_TRANSACTION_BONUS_EVENTS,
	INT_MSG_TRANSACTION_CASH_TRANSFER,
	INT_MSG_TRANSACTION_CHANGE_LIGHT_ON,
	INT_MSG_TRANSACTION_CHANGE_LIGHT_OFF,
	INT_MSG_TRANSACTION_CONFIG_CHANGE,
	INT_MSG_TRANSACTION_CONTROLLED_REBOOT_WARNING,
	INT_MSG_TRANSACTION_CONTROLLED_REBOOT,
	INT_MSG_TRANSACTION_EMPLOYEE_CARD_IN,
	INT_MSG_TRANSACTION_EMPLOYEE_CARD_OUT,
	INT_MSG_TRANSACTION_FAILURE_CODE,
	INT_MSG_TRANSACTION_GAME_INFO,
	INT_MSG_TRANSACTION_ILLEGAL_DOOR_OPEN,
	INT_MSG_TRANSACTION_JACKPOT,
	INT_MSG_TRANSACTION_JACKPOT_FILL,
	INT_MSG_TRANSACTION_MACHINE_ONLINE,
	INT_MSG_TRANSACTION_MACHINE_OFFLINE,
	INT_MSG_TRANSACTION_MACHINE_MOVE_BEGIN,
	INT_MSG_TRANSACTION_MACHINE_MOVE_COMPLETE,
	INT_MSG_TRANSACTION_MAINTENANCE,
	INT_MSG_TRANSACTION_METER_RUNAWAY,
	INT_MSG_TRANSACTION_PB,
	INT_MSG_TRANSACTION_PD_CHASSIS_OPEN,
	INT_MSG_TRANSACTION_VIRTUAL_SESSION_STARTED,
	INT_MSG_TRANSACTION_PLAYER_CARD_IN,
	INT_MSG_TRANSACTION_PLAYER_CARD_OUT,
	INT_MSG_TRANSACTION_PLAYER_UPDATE,
	INT_MSG_TRANSACTION_SENTINEL_OFF,
	INT_MSG_TRANSACTION_SENTINEL_ON,
	INT_MSG_TRANSACTION_SERIAL_COMM_DOWN,
	INT_MSG_TRANSACTION_SERIAL_COMM_UP,
	INT_MSG_TRANSACTION_SLOT_POWER_OFF,
	INT_MSG_TRANSACTION_SLOT_POWER_ON,
	INT_MSG_TRANSACTION_SNAPSHOT,
	INT_MSG_TRANSACTION_TICKET,
	INT_MSG_TRANSACTION_UJP_RESULT,
	INT_MSG_TRANSACTION_ONELINK,
	INT_MSG_TRANSACTION_GENERIC,
	INT_MSG_UI_ACTIVITY_OCCURRED,
	INT_MSG_START_PB_STARVATION, 
	INT_MSG_CLEAR_DISPLAY_STARVATION,
	INT_MSG_UJP_CALLHOST,
	INT_MSG_UJP_CHECK_PIN,
	INT_MSG_UJP_TIMED_OUT,
	INT_MSG_UNATTENDED_JACKPOT,
	INT_MSG_UNLOAD_EVENT_PACKAGE,
	INT_MSG_UPDATE_AUTO_CLOSE_SETTING,
	INT_MSG_UPDATE_BONUS_TYPES,
	INT_MSG_UPDATE_EGM_DISPLAY_SETTING,
	INT_MSG_UPDATE_MEDIAWINDOW_CONFIG,
	INT_MSG_UPDATE_PLAYER_STATE_CLOSE_SETTING,
	INT_MSG_UPDATE_PLAYER_STATE_OPEN_SETTING,
	INT_MSG_UPDATE_SAS_INFO,
	INT_MSG_UPDATE_UI_CONFIG,
	INT_MSG_UPGRADE_AVAILABLE,
	INT_MSG_UPGRADE_MIXER,
	INT_MSG_UPGRADE_CARRIER,
	INT_MSG_USER_SELECTED_SCREEN,
	INT_MSG_CALIBRATE_SENTINEL_SCREEN,
	INT_MSG_MWCONFIG_FILE_AVAILABLE,
	INT_MSG_TRANSACTION_PB_AUTHORIZE,
	INT_MSG_AUTHORIZATION_GRANTED,
	INT_MSG_AUTHORIZATION_DENIED,
	INT_SHOW_POINTS_VS_ACCOUNT_BALANCE,
	INT_MSG_POINTS_OR_CASH_LABEL,
	INT_MSG_ACTIVE_SESSION,
	INT_MSG_GENERAL_POLL_EXCEPTION,
	// Messages which derive CInternalMessage
	INT_DMSG_PROGRESSIVE_QUEUE_TO_ONELINK,
	INT_MSG_ADMIN_MESSAGE,
	INT_MSG_DISPLAY_EMPLOYEE_SCREEN,
	INT_MSG_TEST_URL,
	INT_MSG_UPDATE_ATTRACTOR,
	INT_MSG_ARCHIVE_LOGS,
	INT_MSG_PLAY_UNMOUNT_SOUND,
	INT_MSG_FEATURE_FLAG_UPDATE,
	INT_MSG_FLUSH_PLAYER_UPDATE,
	INT_MSG_TRANSACTION_GAME_SESSION_CHANGE,
	INT_MSG_INCREMENT_GAME_SESSION_ID_IF_REQUIRED,
	INT_MSG_SEND_ALL_GAMES_LIST,
	INT_MSG_SEND_ENABLED_DENOMGAMES_LIST,
	INT_MSG_TRYSENDGAMELIST_GLOBAL_MGMD,
	INT_MSG_TRYSENDGAMELIST_POLLER_ONLINE,
	INT_MSG_REMOTE_REBOOT,
	INT_MSG_SEND_STATUS_AFTER_PARSING_CONFIG,
	INT_MSG_PAIRING_SETTINGS_EVENT,
	INT_MSG_PAIRING_REQUEST,
	INT_MSG_PAIRING_EVENT,
	INT_MSG_PAIRING_STATUS_EVENT,
	INT_MSG_CARD_CHANGE_REQUEST,
	INT_MSG_CARD_CHANGE_EVENT,
	INT_MSG_PLAYER_SESSION_INFO,
	INT_MSG_CREDIT_TOTALS_REQUEST,
	INT_MSG_FUNDS_TRANSFER_REQUEST,
	INT_MSG_FUNDS_TRANSFER_EVENT,
	INT_MSG_GAME_LIST_CHUNKS_SENT,
	INT_MSG_NEW_PIN_REQUIRED,
	INT_MSG_PROMOGAME_REQUEST,
	INT_MSG_PROMOGAME_COMPLETE,
	INT_MSG_LIST_OF_CERTS,
	INT_MSG_TRANSACTION_CARDLESS_SESSION_START,
	INT_MSG_TRANSACTION_CARDLESS_SESSION_END,
	INT_MSG_EGM_LOCK_UNLOCK_STATUS,
	INT_MSG_ENABLE_CASHOUT_TO_HOST,
	INT_MSG_CASHOUT_TO_HOST,
	INT_MSG_CASHOUT_TO_HOST_STATUS,
	INT_MSG_SET_POLLER_IP,
	INT_MSG_PERFORM_LOAD_BALANCING,
	INT_MSG_SET_NFD_POLLER,
	INT_MSG_EXTENDED_BUCKETS_EXIT,
	INT_MSG_SET_NUMBER_OF_SUBGAMES_MISMATCH_CHECK,
	INT_MSG_PROCESS_TAX_DEDUCTION_COMPLETE,
	INT_MSG_CANCEL_PENDING_AUTO_UPLOAD,
	INT_MSG_BAD_PACKET_RECEIVED_FROM_POLLER,
	INT_MSG_DISPLAY_CASHOUT_PBT_ERROR,
	INT_MSG_DISPLAY_AUTOUPLOAD_ERROR,
	INT_MSG_EXTENSIBLE_SETTINGS,
	INT_MSG_UPDATE_SAS_READ_TIMEOUT,
	INT_MSG_REQUEST_CONFIG,
	INT_MSG_LISTEN_TO_PBT_PROGRESS_FROM_GAME,
	INT_MSG_LISTEN_TO_PBT_PROGRESS_TO_GAME,
	INT_MSG_CHECK_UPLOADABLE_CREDITS,
	INT_MSG_MONITOR_MOBILE_PB_SESSION,  // for mobile specific
	INT_MSG_DISPLAY_PB_TRANSFER_ERROR,  // for mobile specific
	INT_MSG_PAYOUT, //JPA for OneLink
	INT_MSG_CAMPAIGN_DOWNLOAD_REQUEST,
	INT_MSG_SKIN_DOWNLOAD_REQUEST,
	INT_MSG_FIRMWARE_PACKAGE_AVAILABLE,
	INT_MSG_CONFIG_FILE_AVAILABLE,
	INT_MSG_CAMPAIGN_AVAILABLE,
	INT_MSG_SKIN_AVAILABLE,
	INT_MSG_REFRESH_UI,
	INT_MSG_MOBILE_CONNECT_PRESS,
	INT_MSG_TRANSACTION_MOBILE_CONNECT_PROXIMITY, // This is used for Mobile Connect as well as Proximity messages
	INT_MSG_CALL_MC_PAIRING_SETTINGS_REQUEST,
	INT_MSG_TRANSACTION_PROXIMITY_REQUEST_BEACON_SCAN_LIST,
	INT_MSG_PROXIMITY_BEACON_SCAN_LIST_RESPONSE,
	INT_MSG_CALL_MCP_PROXIMITY_SETTINGS_REQUEST, // proximity-related, not Mobile Connect
	INT_MSG_PROXIMITY_SETTINGS_EVENT, // proximity-related, not Mobile Connect
	NUMBER_OF_INT_MSG
};

extern const char* InternalMsgTypeStr[];

class CBonusEvents;
class CConfigFields;
class CConnection;
class CEGMAward;
class CEGMConfig;
class CEmployee;
class CFileList;
class CFlashStatus;
class CGameEndData;
class CGames;
class IInternalMsgContent;
class CJackpot;
class CLogs;
class CMediaWindowConfig;
class CMessagePersist;
class CMeters;
class CPersonalBankerInfo;
class CPersonalBankerTransfer;
class PokerHand;
class CPlayer;
class CPlayerGlobalBalance;
class CPlayerInfo;
class CProgressiveJackpotAnnouncement;
class CProgressiveTotalizerAnnouncement;
class CProgressiveBonusingAvailable;
class CProgressiveClaimableAward;
class CProgressiveConfig;
class CProgressiveConnectionInfo;
class CProgressiveHost;
class CProgressiveAnnouncementsInfo;
class CProgressiveLevelBroadcast;
class CProgressiveLevelForward;
class CProgressiveWinnerPick;
class CProgressiveWinnerPicked;
class CStringstringData;
class CUIConfig;
class CThirdPartyMessage;
class CTicket;
class CTicketingInfo;
class CTimeContent;
class CTransactionMetersContent;
class CTriggeredEvent;
class CVideoModeConfig;
class ProgressiveLevelsInfo;

class CInternalMessage : public CQueueItem
{
public:
	CInternalMessage(InternalMsgType type = INT_MSG_NONE);
	CInternalMessage(InternalMsgType type, WORD wordData, WORD additionalWordData = 0);
	CInternalMessage(InternalMsgType type, DWORD dwordData, DWORD additionalDWordData = 0);
	CInternalMessage(InternalMsgType type, int64_t int64Data, int64_t additionalInt64Data = 0);
	CInternalMessage(InternalMsgType type, int32_t intData, int32_t additionalIntData = 0);
	CInternalMessage(InternalMsgType type, const std::string& stringData, const std::string& stringDataAdditional = "");
	CInternalMessage(InternalMsgType type, std::vector<CSubscription>* subscriptions);
	CInternalMessage(InternalMsgType type, time_t *timeData);
	CInternalMessage(InternalMsgType type, const BYTE* ptrBytes, size_t size);
	CInternalMessage(InternalMsgType type, IInternalMsgContent* content, InternalMsgContentType contentType);
	CInternalMessage(InternalMsgType type, IInternalMsgContent& content);
	CInternalMessage(InternalMsgType type, CMeters *metersData, WORD gameNumber, bool bMeterSnapshot = false);
	CInternalMessage(InternalMsgType type, CTicket *ticketData);
	CInternalMessage(InternalMsgType type, CGames *gamesData);
	CInternalMessage(InternalMsgType type, CConfigFields *configData);
	CInternalMessage(InternalMsgType type, CConnection * connection);
	CInternalMessage(InternalMsgType type, CPlayerInfo *playerData);
	CInternalMessage(InternalMsgType type, CPlayerGlobalBalance *globalBalance);
	CInternalMessage(InternalMsgType type, CPlayer *player);
	CInternalMessage(InternalMsgType type, CEmployee *employee);
	CInternalMessage(InternalMsgType type, CBonusEvents *bonusEventsData);
	CInternalMessage(InternalMsgType type, CTicketingInfo *ticketingInfoData);
	CInternalMessage(InternalMsgType type, CLogs *log);
	CInternalMessage(InternalMsgType type, CPersonalBankerInfo *pbInfoData);
	CInternalMessage(InternalMsgType type, CSubscription* subscription);
	CInternalMessage(InternalMsgType type, SubgameSessionParameters *pParams);
	CInternalMessage(InternalMsgType type, SubgameSelectionResponse *pParams);

	virtual ~CInternalMessage();

	/// <summary>
	/// Initializes a new instance of the <see cref="CInternalMessage"/> class.
	/// </summary>
	/// <param name="type">IN - The type.</param>
	/// <param name="egmAward">The EGM award.</param>
	CInternalMessage(InternalMsgType type, CEGMAward *egmAward);

	/// <summary>
	/// Initializes a new instance of the <see cref="CInternalMessage"/> class.
	/// </summary>
	/// <param name="type">IN - The type.</param>
	/// <param name="progressiveConfig">The progressive configuration.</param>
	CInternalMessage(InternalMsgType type, CProgressiveConfig *progressiveConfig);

	/// <summary>
	/// Initializes a new instance of the <see cref="CInternalMessage"/> class.
	/// </summary>
	/// <param name="type">IN - The type.</param>
	/// <param name="progressiveConnectionInfo">The progressive connection information.</param>
	CInternalMessage(InternalMsgType type, CProgressiveConnectionInfo *progressiveConnectionInfo);

	/// <summary>
	/// Initializes a new instance of the <see cref="CInternalMessage"/> class.
	/// </summary>
	/// <param name="type">The type.</param>
	/// <param name="progressiveData">The progressive data.</param>
	CInternalMessage(InternalMsgType type, CProgressiveLevelForward *progressiveForward);

	/// <summary>
	/// Initializes a new instance of the <see cref="CInternalMessage"/> class.
	/// </summary>
	/// <param name="type">IN - The type.</param>
	/// <param name="gameEndData">The game end data.</param>
	CInternalMessage(InternalMsgType type, CGameEndData *gameEndData);

	/// <summary>
	/// Initializes a new instance of the <see cref="CInternalMessage"/> class.
	/// </summary>
	/// <param name="type">IN - The type.</param>
	/// <param name="progressiveConnectionInfo">The progressive host information.</param>
	CInternalMessage(InternalMsgType type, CProgressiveHost* progressiveHost);

	/// <summary>
	/// Initializes a new instance of the <see cref="CInternalMessage"/> class.
	/// </summary>
	/// <param name="type">IN - The type.</param>
	/// <param name="payoutData">The payout data.</param>
	CInternalMessage(InternalMsgType type, CPayout* payoutData);

	/// <summary>
	/// Constructor for a PersonalBanker transfer message.
	/// </summary>
	/// <param name="type">IN - Internal message type..</param>
	/// <param name="pbTransferData">REF - pointer to a CPersonalBankerTransfer instance. Caller MUST NOT access instance after call.</param>
	/// <param name="state">IN - the PersonalBanker state.</param>
	/// <param name="showTimedOut">IN - true for show timed out, false otherwise.</param>
	CInternalMessage(InternalMsgType type, CPersonalBankerTransfer* pbTransferData, PersonalBankerState state, bool showTimedOut = false);
	CInternalMessage(InternalMsgType type, CEGMConfig * EGMConfigData);
	CInternalMessage(InternalMsgType type, CJackpot *jackpot);
	CInternalMessage(InternalMsgType type, CFlashStatus *flashStatus);
	CInternalMessage(InternalMsgType type, CTriggeredEvent *triggeredEvent);
	CInternalMessage(InternalMsgType type, CFileList *fileList);
	CInternalMessage(InternalMsgType type, CThirdPartyMessage *thirdPartyMessage);
	CInternalMessage(InternalMsgType type, CMessagePersist *messagePersist);
	CInternalMessage(InternalMsgType type, CVideoModeConfig *videoModeConfig);

	// Constructor
	//     IN type - identifier of type of message being sent
	//     IN pAnnouncement - pointer to objects containing the announcement data
	//   For use when sending progressive jackpot announcements.
	CInternalMessage(InternalMsgType type, CProgressiveJackpotAnnouncement* pAnnouncement);

	// Constructor
	//     IN type - identifier of type of message being sent
	//     IN pAnnouncement - pointer to objects containing the announcement data
	//   For use when sending progressive totalizer announcements.
	CInternalMessage(InternalMsgType type, CProgressiveTotalizerAnnouncement* pAnnouncement);

	// Constructor
	//     IN type - identifier of type of message being sent
	//     IN pVerification - pointer to objects containing the campaign verification data
	//   For use when sending ICardConfiguration5 with campaign info for offline.
	CInternalMessage(InternalMsgType type, CCampaignConfig* pVerification);

	// Constructor
	//     IN type - identifier of type of message being sent
	//     IN pVerification - pointer to objects containing the skin verification data
	//   For use when sending ICardConfiguration5 with skin info for offline.
	CInternalMessage(InternalMsgType type, CSkinConfig* pVerification);

	// Constructor
	//     IN type - identifier of type of message being sent
	//     IN pClaimableAward - pointer to objects containing the claimable award data
	//   For use when sending messages that with the claimable award data.
	CInternalMessage(InternalMsgType type, CProgressiveClaimableAward* pClaimableAward);

	/// <summary>
	/// Sets the CProgressiveJackpotAnnouncement data.
	/// </summary>
	/// <param name="pAnnouncement">IN/OUT - The progressive announcement data.</param>
	void SetData(CProgressiveJackpotAnnouncement* pAnnouncement);

	/// <summary>
	/// Gets the CProgressiveJackpotAnnouncement.
	/// </summary>
	/// <returns>A pointer to a read/write CProgressiveJackpotAnnouncement instance.</returns>
	CProgressiveJackpotAnnouncement* GetProgressiveJackpotAnnouncement() const;

	/// <summary>
	/// Sets the CProgressiveTotalizerAnnouncement data.
	/// </summary>
	/// <param name="pAnnouncement">IN/OUT - The progressive totalizer announcement data.</param>
	void SetData(CProgressiveTotalizerAnnouncement* pAnnouncement);

	/// <summary>
	/// Gets the CProgressiveTotalizerAnnouncement.
	/// </summary>
	/// <returns>A pointer to a read/write CProgressiveTotalizerAnnouncement instance.</returns>
	CProgressiveTotalizerAnnouncement* GetProgressiveTotalizerAnnouncement() const;

	/// <summary>
	/// Sets the CCampaignConfig data.
	/// </summary>
	/// <param name="pConfig">IN/OUT - The progressive campaign verification data.</param>
	void SetData(CCampaignConfig* pConfig);

	/// <summary>
	/// Gets the CCampaignConfig.
	/// </summary>
	/// <returns>A pointer to a read/write CCampaignConfig instance.</returns>
	CCampaignConfig* GetCampaignConfig() const;

	/// <summary>
	/// Sets the CSkinConfig data.
	/// </summary>
	/// <param name="pConfig">IN/OUT - The progressive skin verification data.</param>
	void SetData(CSkinConfig* pConfig);

	/// <summary>
	/// Gets the CSkinConfig.
	/// </summary>
	/// <returns>A pointer to a read/write CSkinConfig instance.</returns>
	CSkinConfig* GetSkinConfig() const;

	/// <summary>
	/// Sets the CProgressiveClaimableAward data.
	/// </summary>
	/// <param name="pClaimableAward">IN/OUT - The progressive claimable award data.</param>
	void SetData(CProgressiveClaimableAward* pClaimableAward);

	/// <summary>
	/// Gets the CProgressiveClaimableAward.
	/// </summary>
	/// <returns>A pointer to a read/write CProgressiveClaimableAward instance.</returns>
	CProgressiveClaimableAward* GetProgressiveClaimableAward() const;

	// Constructor
	//     IN type - identifier of type of message being sent
	//     IN pLevelBroadcast - pointer to objects containing the progressive level broadcast data
	//   For use when sending messages that with the progressive level broadcast data.
	CInternalMessage(InternalMsgType type, CProgressiveLevelBroadcast* pLevelBroadcast);

	/// <summary>
	/// Sets the CProgressiveLevelBroadcast data.
	/// </summary>
	/// <param name="pLevelBroadcast">IN/OUT - The progressive level broadcast data.</param>
	void SetData(CProgressiveLevelBroadcast* pLevelBroadcast);

	/// <summary>
	/// Gets the progressive level broadcast.
	/// </summary>
	/// <returns>A pointer to a read/write GetProgressiveLevelBroadcast instance.</returns>
	CProgressiveLevelBroadcast* GetProgressiveLevelBroadcast() const;

	// Constructor
	//     IN type - identifier of type of message being sent
	//     IN pProgressiveLevelConfiguration - pointer to objects containing the progressive level information data
	//     IN pAnnouncmentsConfiguration - pointer to objects containing the progressive announcements information data
	//     IN pFirmwarePackage - pointer to the objects containing the firmware configuration and files
	//     IN pCampaignConfig - pointer to the objects containing the campaign configuration and files
	//     IN pSkinConfig - pointer to the objects containing the skin configuration and files
	//   For use when sending messages that with the progressive level information data.
	CInternalMessage(InternalMsgType type, ProgressiveLevelsInfo* pProgressiveLevelsInfo, CProgressiveAnnouncementsInfo* pAnnouncmentsConfiguration, CFirmwarePackage* pFirmwarePackage, CCampaignConfig* pCampaignConfig, CSkinConfig* pSkinConfig);

	/// <summary>
	/// Gets the progressive level information.
	/// </summary>
	/// <returns>A pointer to a read/write GetProgressiveLevelConfiguration instance.</returns>
	ProgressiveLevelsInfo* GetProgressiveLevelsInfo() const;

	/// <summary>
	/// Gets the progressive announcements information.
	/// </summary>
	/// <returns>A pointer to a read/write GetProgressiveAnnouncementsInfo instance.</returns>
	CProgressiveAnnouncementsInfo* GetProgressiveAnnouncementsInfo() const;

	/// <summary>
	/// CInternalMessage constructor
	/// </summary>
	/// <param name="type">Identifier of type of message being sent.</param>
	/// <param name="pWinnerPick">Pointer to CProgressiveWinnerPick object.</param>
	CInternalMessage(InternalMsgType type, CProgressiveWinnerPick* pWinnerPick);

	/// <summary>
	/// Sets the CProgressiveWinnerPick data.
	/// </summary>
	/// <param name="pWinnerPick">IN/OUT - The winner pick data.</param>
	void SetData(CProgressiveWinnerPick* pWinnerPick);

	/// <summary>
	/// Gets the winner pick broadcast.
	/// </summary>
	/// <returns>A pointer to a read/write CProgressiveWinnerPick instance.</returns>
	CProgressiveWinnerPick* GetProgressiveWinnerPick() const;

	/// <summary>
	/// CInternalMessage constructor
	/// </summary>
	/// <param name="type">Identifier of type of message being sent.</param>
	/// <param name="pWinnerPick">Pointer to CProgressiveWinnerPicked object.</param>
	CInternalMessage(InternalMsgType type, CProgressiveWinnerPicked* pWinnerPicked);

	/// <summary>
	/// Sets the CProgressiveWinnerPicked data.
	/// </summary>
	/// <param name="pWinnerPicked">IN/OUT - The winner picked data.</param>
	void SetData(CProgressiveWinnerPicked* pWinnerPicked);

	/// <summary>
	/// Gets the winner picked broadcast.
	/// </summary>
	/// <returns>A pointer to a read/write CProgressiveWinnerPicked instance.</returns>
	CProgressiveWinnerPicked* GetProgressiveWinnerPicked() const;

	// Constructor
	//     IN type - identifier of type of message being sent
	//     IN pPokerHand - pointer to poker hand object
	//   For use when sending messages that with the progressive level information data.
	CInternalMessage(InternalMsgType type, PokerHand *pPokerHand);

	/// <summary>
	/// Sets the CPokerHand data.
	/// </summary>
	/// <param name="pPokerHand">IN/OUT - The poker hand.</param>
	void SetData(PokerHand* pPokerHand);

	/// <summary>
	/// Gets the poker hand.
	/// </summary>
	/// <returns>A pointer to the poker hand.</returns>
	PokerHand* GetPokerHand() const;

	/// <summary>
	/// Bonusing Available constructor. Initializes a new instance of the <see cref="CInternalMessage"/> class.
	/// </summary>
	/// <param name="type">IN - The type.</param>
	/// <param name="progresiveBonusingAvailable">IN/OUT - The progressive bonusing available information.</param>
	CInternalMessage(InternalMsgType type, CProgressiveBonusingAvailable *progresiveBonusingAvailable);

	/// <summary>
	/// Sets the CProgressiveBonusingAvailable data.
	/// </summary>
	/// <param name="progresiveBonusingAvailable">IN/OUT - The CProgressiveBonusingAvailable data.</param>
	void SetData(CProgressiveBonusingAvailable *progresiveBonusingAvailable);

	/// <summary>
	/// Gets a pointer to the read/write CProgressiveBonusingAvailable instance. MAY be NULL.
	/// </summary>
	/// <returns>A pointer to the read/write CProgressiveBonusingAvailable instance. MAY be NULL.</returns>
	CProgressiveBonusingAvailable *GetProgressiveBonusingAvailable() const;

	/// <summary>
	/// Sets the ProgressivePatronSessionInfo data.
	/// </summary>
	/// <param name="progresivePatronSessionInfo">IN/OUT - The ProgressivePatronSessionInfo data.</param>
	void SetData(ProgressivePatronSessionInfo *progressivePatronSessionInfo);

	/// <summary>
	/// Gets a pointer to the read/write ProgressivePatronSessionInfo instance. MAY be NULL.
	/// </summary>
	/// <returns>A pointer to the read/write ProgressivePatronSessionInfo instance. MAY be NULL.</returns>
	ProgressivePatronSessionInfo *GetProgressivePatronSessionInfo() const;

	/// <summary>
	/// Sets the CProgressiveEligibleLevelsInfo data.
	/// </summary>
	/// <param name="CProgressiveEligibleLevelsInfo">IN/OUT - The CProgressiveEligibleLevelsInfo data.</param>
	void SetData(CProgressiveEligibleLevelsInfo *progressiveEligibleLevelsInfo);

	/// <summary>
	/// Sets the data.
	/// </summary>
	/// <param name="params">The subgame session parameters</param> 
	void SetData(SubgameSessionParameters *params);

	/// <summary>
	/// Sets the data.
	/// </summary>
	/// <param name="SubgameSelectionResponse">The subgame selection response</param> 
	void SetData(SubgameSelectionResponse *params);

	/// <summary>
	/// Gets a pointer to the read/write CProgressiveEligibleLevelsInfo instance. MAY be NULL.
	/// </summary>
	/// <returns>A pointer to the read/write CProgressiveEligibleLevelsInfo instance. MAY be NULL.</returns>
	CProgressiveEligibleLevelsInfo *GetProgressiveEligibleLevelsInfo() const;

	// Constructor
	//     IN type - identifier of type of message being sent
	//     IN vectorOfStrings - vector of strings
	CInternalMessage(InternalMsgType type, std::vector<string>* vectorOfStrings);

	// Constructor
	//     IN type - identifier of type of message being sent
	//     IN vectorOfStrings - vector of WORDs
	CInternalMessage(InternalMsgType type, std::vector<DWORD>* vectorOfWords);

	// Constructor
	//     IN type - identifier of type of message being sent
	//     IN uiConfig - pointer to objects containing the Task Bar Configuration settings
	//   For use when sending messages that need the all Task Bar Configuration settings
	CInternalMessage(InternalMsgType type, CUIConfig *uiConfig);

	/// <summary>
	/// Gets the message type.
	/// </summary>
	/// <returns>The message type.</returns>
	InternalMsgType GetMessageType() const;

	/// <summary>
	/// Gets the WORD data.
	/// </summary>
	/// <param name="bAdditional">IN - true to get the additional WORD data, false for the normal WORD data.</param>
	/// <returns>The WORD data.</returns>
	WORD GetWordData(bool bAdditional = false) const;
	DWORD GetDWordData(bool bAdditional = false) const;
	long  GetIntData(bool bAdditional = false) const;
	int64_t GetInt64Data(bool bAdditional = false) const;
	double GetDoubleData(bool bAdditional = false) const;
	const BYTE *GetBytesData(size_t &size) const;
	std::string GetStringData(bool bAdditional = false) const;

	void SetData(WORD wordData, WORD additionalWordData = 0);
	void SetData(DWORD dWordData, DWORD additionalDWordData = 0);
	void SetDoubleData(double doubleData, double additionalDoubleData = 0);
	void SetData(int64_t int64Data, int64_t additionalInt64Data = 0);
	void SetData(int32_t intData, int32_t additionalIntData = 0);

	void SetData(IInternalMsgContent* content, InternalMsgContentType contentType);
	void SetData(CMeters *meters, WORD gameNumber, bool bMeterSnapshot = false);

	// We need to get the InternalMsgContentType for the content object, so a NULL pointer cannot be passed.
	// To make this impossible we've made the function parameter a reference and not a pointer.
	void SetData(IInternalMsgContent& content);

	/// <summary>
	/// Sets the data.
	/// </summary>
	/// <param name="egmAward">The EGM award.</param>
	void SetData(CEGMAward* egmAward);

	/// <summary>
	/// Gets the EGM award.
	/// </summary>
	/// <returns>The pointer to the EGM award object.</returns>
	CEGMAward* GetEGMAward() const;

	/// <summary>
	/// Sets the data.
	/// </summary>
	/// <param name="progressiveConfig">The progressive configuration.</param>
	void SetData(CProgressiveConfig* progressiveConfig);

	/// <summary>
	/// Gets the progressive configuration.
	/// </summary>
	/// <returns>The pointer to the progressive configuration object.</returns>
	CProgressiveConfig* GetProgressiveConfig() const;

	/// <summary>
	/// Sets the data.
	/// </summary>
	/// <param name="progressiveConnectionInfo">The progressive connection information.</param>
	void SetData(CProgressiveConnectionInfo* progressiveConnectionInfo);

	/// <summary>
	/// Sets the progressive level forward.
	/// </summary>
	/// <param name="pProgressiveLevelForward">The progressive level forward.</param>
	void SetData(CProgressiveLevelForward* pProgressiveLevelForward);

	/// <summary>
	/// Gets the progressive level forward.
	/// </summary>
	/// <returns>Pointer to the progressive level forward.</returns>
	CProgressiveLevelForward* GetProgressiveLevelForward() const;

	/// <summary>
	/// Gets the progressive connection information.
	/// </summary>
	/// <returns>The pointer to the progressive connection information object.</returns>
	CProgressiveConnectionInfo* GetProgressiveConnectionInfo() const;

	/// <summary>
	/// Sets the game end data.
	/// </summary>
	/// <param name="progressiveData">The game end data.</param>
	void SetData(CGameEndData *gameEndData);

	/// <summary>
	/// Gets the game end data.
	/// </summary>
	/// <returns>Pointer to the game end data.</returns>
	CGameEndData *GetGameEndData() const;

	/// <summary>
	/// Sets the payout meter delta.
	/// </summary>
	/// <param name="payoutMeterDelta">The payout meter delta.</param>
	void SetData(CPayout* payout);

	/// <summary>
	/// Gets the payout meter delta.
	/// </summary>
	/// <returns>Pointer to the payout meter delta.</returns>
	CPayout* GetPayout() const;

	/// <summary>
	/// Sets the data.
	/// </summary>
	/// <param name="progressiveHost">The progressive host information.</param>
	void SetData(CProgressiveHost* progressiveHost);

	/// <summary>
	/// Gets the progressive host information.
	/// </summary>
	/// <returns>The pointer to the progressive host information object.</returns>
	CProgressiveHost* GetProgressiveHost() const;

	CInternalMessage(InternalMsgType type, CPairingSettingsEvent *pPairingSettingsEvent);
	void SetData(CPairingSettingsEvent *pairingSettingsEventData);
	CPairingSettingsEvent* GetPairingSettingsEventData() const;

	CInternalMessage(InternalMsgType type, CPairingRequest *pairingRequestData);
	void SetData(CPairingRequest *pairingRequestData);
	CPairingRequest* GetPairingRequestData() const;

	CInternalMessage(InternalMsgType type, CPairingEvent *pPairingEvent);
	void SetData(CPairingEvent *pairingEventData);
	CPairingEvent* GetPairingEventData() const;

	CInternalMessage(InternalMsgType type, CPairingStatusEvent *pPairingStatusEvent);
	void SetData(CPairingStatusEvent *pairingStatusEventData);
	CPairingStatusEvent* GetPairingStatusEventData() const;

	CInternalMessage(InternalMsgType type, CProximitySettingsEvent *pProximitySettingsEvent);
	void SetData(CProximitySettingsEvent *proximitySettingsEventData);
	CProximitySettingsEvent* GetProximitySettingsEventData() const;

	CInternalMessage(InternalMsgType type, CProximityStatusEvent *pProximityStatusEvent);
	void SetData(CProximityStatusEvent *proximityStatusEventData);
	CProximityStatusEvent* GetProximityStatusEventData() const;

	CInternalMessage(InternalMsgType type, CCardChangeRequest *pCardChangeRequest);
	void SetData(CCardChangeRequest *pCardChangeRequest);
	CCardChangeRequest* GetCardChangeRequestData() const;

	CInternalMessage(InternalMsgType type, CCardChangeEvent *pCardChangeEvent);
	void SetData(CCardChangeEvent *pCardChangeEventData);
	CCardChangeEvent* GetCardChangeEventData() const;

	CInternalMessage(InternalMsgType type, CPlayerChangeEvent *pPlayerChangeEvent);
	void SetData(CPlayerChangeEvent *pPlayerChangeEventData);
	CPlayerChangeEvent* GetPlayerChangeEventData() const;

	CInternalMessage(InternalMsgType type, CPlayerSessionChangeEvent *pPlayerSessionChangeEvent);
	void SetData(CPlayerSessionChangeEvent *pPlayerSessionChangeEventData);
	CPlayerSessionChangeEvent* GetPlayerSessionChangeEventData() const;

	CInternalMessage(InternalMsgType type, CFundsTransferBalanceRequest*pCreditTotalsRequest);
	void SetData(CFundsTransferBalanceRequest *pCreditTotalsRequestData);
	CFundsTransferBalanceRequest* GetFundsTransferBalanceRequestData() const;
		
	CInternalMessage(InternalMsgType type, CFundsTransferBalanceEvent*pFundsTransferBalanceEvent);
	void SetData(CFundsTransferBalanceEvent *pFundsTransferBalanceEventData);
	CFundsTransferBalanceEvent* GetFundsTransferBalanceEventData() const;

	CInternalMessage(InternalMsgType type, CFundsTransferRequest*pFundsTransferRequest);
	void SetData(CFundsTransferRequest *pFundsTransferRequestData);
	CFundsTransferRequest* GetFundsTransferRequestData() const;

	CInternalMessage(InternalMsgType type, CFundsTransferEvent* pFundsTransferEvent);
	void SetData(CFundsTransferEvent *pFundsTransferEventData);
	CFundsTransferEvent* GetFundsTransferEventData() const;

	CInternalMessage(InternalMsgType type, CPromoGameRequest* promoGameRequest);
	void SetData(CPromoGameRequest* promoGameRequest);
	CPromoGameRequest* GetPromoGameRequest() const;

	CInternalMessage(InternalMsgType type, CFirmwarePackage* firmwarePackage);
	void SetData(CFirmwarePackage* firmwarePackage);
	CFirmwarePackage* GetFirmwarePackage() const;

	CInternalMessage(InternalMsgType type, CConfigFilePackage* configFilePackage);
	void SetData(CConfigFilePackage* configFilePackage);
	CConfigFilePackage* GetConfigFilePackage() const;

	CInternalMessage(InternalMsgType type, CCampaignPackage* campaignPackage);
	void SetData(CCampaignPackage* campaignPackage);
	CCampaignPackage* GetCampaignPackage() const;

	CInternalMessage(InternalMsgType type, CSkinPackage* skinPackage);
	void SetData(CSkinPackage* skinPackage);
	CSkinPackage* GetSkinPackage() const;

	CInternalMessage(InternalMsgType type, CPromoGameAcceptanceStatus* promoGameAcceptanceStatus);
	void SetData(CPromoGameAcceptanceStatus* promoGameAcceptanceStatus);
	CPromoGameAcceptanceStatus* GetPromoGameAcceptanceStatus() const;

	void SetData(CBonusEvents *bonusEvents);
	void SetData(CConnection *connection);
	void SetData(CEGMConfig *EGMConfigData);
	void SetData(CEmployee *employee);
	void SetData(CFileList *fileList);
	void SetData(CFlashStatus *flashStatus);
	void SetData(CGames *gamesData);
	void SetData(CJackpot *jackpot);
	void SetData(CLogs *log);
	void SetData(const BYTE* ptrBytes, size_t size);
	void SetData(CPersonalBankerInfo *pbInfo);
	void SetData(CSubscription* subscription);
	void SetData(CPersonalBankerTransfer *pbTransferData);
	void SetData(CPlayer *player);
	void SetData(CPlayerGlobalBalance *globalBalance);
	void SetData(CPlayerInfo *playerInfo);
	void SetData(const std::string& stringData, const std::string& stringDataAdditional = "");
	void SetData(CTicketingInfo *ticketingInfo);
	void SetData(time_t *timeData);
	void SetData(CTriggeredEvent *triggeredEvent);
	void SetData(CUIConfig *uiConfig);;
	void SetData(CTicket *ticket);
	void SetMediaWindowConfig(CMediaWindowConfig *mwConfig);
	void SetMessagePersist(CMessagePersist * messagePersist);
	void SetMessageType(InternalMsgType type);
	void SetThirdPartyMessage(CThirdPartyMessage *thirdPartyMessage);
	void SetTransactionMetersData(CMeters *transactionMeters);
	void SetVideoModeConfig(CVideoModeConfig *videoModeConfig);

	CBonusEvents *GetBonusEventsData() const;
	CConfigFields *GetConfigData() const;
	CConnection *GetConnectionData() const;
	CEGMConfig *GetEGMConfigData() const;
	CEmployee *GetEmployeeData() const;
	CFileList *GetFileList() const;
	CFlashStatus *GetFlashStatus() const;
	CGames *GetGamesData() const;
	CJackpot *GetJackpotData() const;
	CLogs *GetLogData() const;
	CMediaWindowConfig *GetMediaWindowConfig() const;
	CMessagePersist* GetMessagePersist() const;
	CMeters *GetMetersData(WORD &gameNumber) const;
	CMeters *GetTransactionMetersData() const;
	CPersonalBankerInfo *GetPBInfoData() const;
	CPersonalBankerTransfer *GetPBTransferData() const;
	CPlayer *GetPlayerData() const;
	CPlayerGlobalBalance *GetPlayerGlobalBalanceData() const;
	CPlayerInfo *GetPlayerInfoData() const;
	CSubscription *GetSubscriptionData() const;
	CThirdPartyMessage *GetThirdPartyMessage() const;
	CTicket *GetTicketData() const;
	CTicketingInfo *GetTicketingInfoData() const;
	time_t *GetTimeData() const;
	CVideoModeConfig *GetVideoModeConfig() const;
	std::vector<CSubscription>* GetSubscriptions() const;
	std::vector<std::string>* GetVectorOfStrings() const;
	void SetVectorOfWords(std::vector<DWORD>* vectorOfWords);
	std::vector<DWORD>* GetVectorOfWords() const;
	SubgameSessionParameters *GetSubgameSessionParameters() const;
	SubgameSelectionResponse *GetSubgameSelectionResponse() const;
	IInternalMsgContent* GetContent(InternalMsgContentType type) const;

	/// <summary>
	/// Gets the task bar configuration.
	/// </summary>
	/// <returns>A pointer to a read/write CUIConfig instance.</returns>
	CUIConfig *GetUIConfig() const;

	/// <summary>
	/// Sets the Time-To-Live tickcount for the message and sets the start tick count to current tick count.
	/// </summary>
	/// <param name="ttl">The TTL.</param>
	void SetMessageTTL(DWORD ttl);

	/// <summary>
	/// Determines whether the message's ttl has expired.
	/// </summary>
	/// <remarks>
	/// If ttl is 0 then this will return false.
	/// </remarks>
	/// <returns>Is message too old.</returns>
	bool IsTTLExpired() const;

	const string GetMessageTypeStr() const;

private:
	InternalMsgType m_msgType;
	std::vector<CSubscription>* m_subcriptions;
	std::vector<std::string>* m_vectorOfStrings;
	std::vector<DWORD>* m_vectorOfWords;
	std::string m_strData;
	std::string m_strDataAdditional;
	const BYTE *m_bytesData;
	size_t m_size;
	WORD m_wordData;
	WORD m_wordDataAdditional;
	DWORD m_dwordData;
	DWORD m_dwordDataAdditional;
	double m_doubleData;
	double m_doubleDataAdditional;
	long m_intData;
	long m_intDataAdditional;
	int64_t m_int64Data;
	int64_t m_int64DataAdditional;
	DWORD m_createTickCount;
	DWORD m_ttlTickCount;
	CInternalMsgContentStore m_contentStore;

	void InitMembers(InternalMsgType msgType);
};
