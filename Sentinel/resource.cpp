#if 1//TODO-PORT - Temporary support for resource strings.
#include "stdafx.h"
#include "resource.h"

using namespace std;

const LPCTSTR IDS_EQUALPRIORITYINTERRUPT(_T("Equal Priority Interrupt: %s<br>"));

const LPCTSTR IDS_INITIALINTERACTIVESTARVETIME(_T("Initial Interactive Starve Time: %u<br>"));
const LPCTSTR IDS_INTERACTIVECONTENTSTARVETIME(_T("Interactive Content Starve Time: %u<br>"));
const LPCTSTR IDS_USERSCREENTIME(_T("User Screen Time: %u<br>"));
const LPCTSTR IDS_UJPTIMEOUT(_T("UJP Timeout: %u<br>"));
const LPCTSTR IDS_PBTSCREENS(_T("PBT Screens Priority: %u<br>"));
const LPCTSTR IDS_PLAYERINITIATEDPRIORITY(_T("Player Initiated Priority: %u<br>"));
const LPCTSTR IDS_INTERACTIVECONTENTPRIORITY(_T("Interactive Content Priority: %u<br>"));
const LPCTSTR IDS_WINNERMESSAGEPRIORITY(_T("Winner Message Priority: %u<br>"));
const LPCTSTR IDS_WINNERMESSAGETTL(_T("Winner Message TTL: %u<br>"));
const LPCTSTR IDS_AVAILABLE(_T("available"));
const LPCTSTR IDS_CASH_SYMBOL(_T("$"));
const LPCTSTR IDS_CONGRATSPRIORITY(_T("Congrats Priority: %u<br>"));
const LPCTSTR IDS_CREDIT_COMMUNICATION_ERROR_1(_T("Communications Error"));
const LPCTSTR IDS_CREDIT_COMMUNICATION_ERROR_2(_T("Check Account Balance"));
const LPCTSTR IDS_CREDIT_COMMUNICATION_ERROR_3(_T("for Transaction Results"));
const LPCTSTR IDS_CREDIT_TRANSFER(_T("Credit transfer"));

const LPCTSTR IDS_SUCCESSFUL(_T("successful"));
const LPCTSTR IDS_GLOBAL_REBOOT_YESNO_1(_T("Proceed with Reboot?"));
const LPCTSTR IDS_GLOBAL_REBOOT_YESNO_2(_T("Press Yes to continue."));
const LPCTSTR IDS_GLOBAL_REBOOT_YESNO_3(_T("Press No to cancel."));
const LPCTSTR IDS_GLOBAL_REBOOT_SUCCESS(_T("Reboot in progress..."));
const LPCTSTR IDS_PBT_INVALID_ID_FLOORLOGIX(_T("Invalid PBT ID from FloorLogix"));
const LPCTSTR IDS_UNEXPTECTED_PBT_AMOUNT_FROM_GAME(_T("Unexpected PBT amount transfered to game."));
const LPCTSTR IDS_TKT_PRINT_AMOUNT_OVER_LIMIT_AFTER(_T("Printed ticket amount larger than maximum allowed amount."));
const LPCTSTR IDS_PBT_AMOUNT_OVER_LIMIT(_T("PBT amount transfered over maximum limit."));
const LPCTSTR IDS_PBT_AMOUNT_OVER_LIMIT_BEFORE(_T("PBT amount over transfer limit before transfer, transaction cancelled."));
const LPCTSTR IDS_GLOBAL_FULLEGM_YESNO_1(_T("Press Yes for full"));
const LPCTSTR IDS_GLOBAL_FULLEGM_YESNO_2(_T("display. Remove card"));
const LPCTSTR IDS_GLOBAL_FULLEGM_YESNO_3(_T("when complete."));
const LPCTSTR IDS_GLOBAL_FULLEGMWAIT_YESNO_1(_T("Press Yes (or wait)"));
const LPCTSTR IDS_GLOBAL_FULLEGMWAIT_YESNO_2(_T("for full display. Remove"));
const LPCTSTR IDS_GLOBAL_FULLEGMWAIT_YESNO_3(_T("card when complete."));

const LPCTSTR IDS_PBT_ID_INVALIDE_BEFORE(_T("Invalid PBT ID before transfer, transaction cancelled."));
const LPCTSTR IDS_PBT_ID_INVALID_AFTER(_T("Invalid PBT ID after transfer complete."));
const LPCTSTR IDS_TKT_REDEEM_AMOUNT_OVER_LIMIT_AFTER(_T("Redeemed ticket amount larger than maximum allowed amount."));
const LPCTSTR IDS_TKT_REDEEM_AMOUNT_MISMATCH_AFTER(_T("Redeemed ticket amount different than expected amount."));
const LPCTSTR IDS_TKT_REDEEM_AMOUNT_OVER_LIMIT_BEFORE(_T("Ticket amount from FloorLogix over allowed limit, rejecting ticket."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_CARD_IN_PROMOTION(_T("You've won a prize."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_CARDIN_ELIGIBILITY_SCREEN(_T("You are eligible for the promotion."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_PROXIMITY_SCREEN(_T("Keep playing! You are about to trigger the promotion."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_SPLASHDOWN_START_SCREEN(_T("The promotion has started."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_RICOCHET_START_SCREEN(_T("The promotion has started."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_CASINO_CHALLENGE_START_SCREEN(_T("The promotion has started."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_EVENT_TRIGGERED_SCREEN(_T("The promotion has started."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_SPLASHDOWN_AWARD_SCREEN(_T("You've won an award."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_RICOCHET_CELEBRATION_SCREEN(_T("You've won an award."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_CASINO_CHALLENGE_AWARD1_SCREEN(_T("You've won an award."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_CASINO_CHALLENGE_AWARD2_SCREEN(_T("You've won an award."));

const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_CASINO_CHALLENGE_AWARD3_SCREEN(_T("You've won an award."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_NONWINNER_NOTIFICATION_SCREEN(_T("Thank you for playing."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_CASINO_CHALLENGE_FIVE_MINUTE_WARNING_END_SCREEN(_T("Keep Playing! Five minutes remaining in the promotion."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_SPLASHDOWN_END_SCREEN(_T("Thank you for playing."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_SPLASHDOWN_WINNER_END_SCREEN(_T("Thank you for playing."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_CASINO_CHALLENGE_END_SCREEN(_T("The promotion has ended. Play again for more chances to win."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_PNW_END_SCREEN(_T("The promotion has ended. Play again for more chances to win."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_JPACARDED_SCREEN(_T("Someone just hit a jackpot!"));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_JPANONCARDED_SCREEN(_T("Someone just hit a jackpot!"));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_TOTALIZER_SCREEN(_T("Players have won a large amount of promotional awards."));
const LPCTSTR IDS_DEFAULT_CUSTOM_TEXT_PLAYER_ANNOUNCMENT(_T("Please see your host."));
const LPCTSTR IDS_PROGRESSIVE_DOWN_MESSAGE(_T("This machine is NOT participating in one or more Progressive promotion(s)."));
const LPCTSTR IDS_CONNECTION_DEFAULT(_T("N/A"));
const LPCTSTR IDS_CONNECTION_NOT_STARTED(_T("Not Started"));
const LPCTSTR IDS_CONNECTION_DISABLED(_T("Disabled"));
const LPCTSTR IDS_CONNECTION_WAITING_FOR_INFO(_T("Waiting For Info"));

const LPCTSTR IDS_CONNECTION_IN_DISCOVERY(_T("In Discovery"));
const LPCTSTR IDS_CONNECTION_INITIATE_DISCOVERY(_T("Quarantined"));
const LPCTSTR IDS_CONNECTION_CONNECTED(_T("Connected"));
const LPCTSTR IDS_CONNECTION_DISCONNECTED(_T("Disconnected"));
const LPCTSTR IDS_CONNECTION_ERROR_DEFAULT(_T("Error"));
const LPCTSTR IDS_CONNECTION_ERROR_COULD_NOT_FIND_SERVER(_T("Error: Could Not Find Server"));
const LPCTSTR IDS_CONNECTION_ERROR_COULD_NOT_FIND_SERVER_SHORT(_T("Error: Server N/A"));
const LPCTSTR IDS_CONNECTION_ERROR_DISCOVERY_CONNECTION(_T("Error: Discovery Connection"));
const LPCTSTR IDS_CONNECTION_ERROR_DISCOVERY_CONNECTION_SHORT(_T("Error: Discovery Conn."));
const LPCTSTR IDS_CONNECTION_ERROR_MESSAGE_TRANSMISSION(_T("Error: Message Transmission"));
const LPCTSTR IDS_CONNECTION_ERROR_MESSAGE_TRANSMISSION_SHORT(_T("Error: Message Trans."));
const LPCTSTR IDS_CONNECTION_ERROR_CONNECT_FAILED(_T("Error: Connect Failed"));
const LPCTSTR IDS_CONNECTION_ERROR_CONNECT_FAILED_SHORT(_T("Error: Connect Failed"));
const LPCTSTR IDS_CONNECTION_ERROR_DISCONNECT_FAILED(_T("Error: Disconnect Failed"));
const LPCTSTR IDS_CONNECTION_ERROR_DISCONNECT_FAILED_SHORT(_T("Error: Disconnect Fail"));
const LPCTSTR IDS_GLOBAL_CARD_PROGRESSIVE_CONFIGURATION(_T("Global Card: Progressive Configuration"));
const LPCTSTR IDS_MECHANIC_CARD_ONE_LINK_CONNECTION_INFO(_T("Mechanic Card: ONE LINK Connection Info"));

const LPCTSTR IDS_MECHANIC_CARD_PROGRESSIVE_HOST_LIST(_T("Mechanic Card: Progressive Host List"));
const LPCTSTR IDS_MECHANIC_CARD_PROGRESSIVE_HOST_IP_ADDRESSES(_T("Mechanic Card: Progressive Host IP Addresses"));
const LPCTSTR IDS_PROGRESSIVE_CONFIGURATION_COLON(_T("Progressive Configuration:"));
const LPCTSTR IDS_CONNECTION_STATE_COLON(_T("Connection state:"));
const LPCTSTR IDS_LAST_MESSAGE_IP_COLON(_T("Last message IP:"));
const LPCTSTR IDS_LAST_MESSAGE_TIME_COLON(_T("Last message time:"));
const LPCTSTR IDS_HOST_NAME_COLON(_T("Host name:"));
const LPCTSTR IDS_CURRENT_HOST_IP_COLON(_T("Current host IP:"));
const LPCTSTR IDS_HOST_COLON(_T("Host:"));
const LPCTSTR IDS_HOST_IP_COLON(_T("Host IP:"));
const LPCTSTR IDS_ENABLE(_T("Enable"));
const LPCTSTR IDS_DISABLE(_T("Disable"));
const LPCTSTR IDS_LAST_MESSAGE_IP_DEFAULT(_T("N/A"));
const LPCTSTR IDS_HOST_NAME_DEFAULT(_T("N/A"));
const LPCTSTR IDS_CURRENT_HOST_IP_DEFAULT(_T("N/A"));
const LPCTSTR IDS_LAST_TIMESTAMP_DEFAULT(_T("N/A"));

const LPCTSTR IDS_FLOOR_CARD_LOGIN(_T("Floor Card: Login"));
const LPCTSTR IDS_GLOBAL_CARD_LOGIN(_T("Global Card: Login"));
const LPCTSTR IDS_MECHANIC_CARD_LOGIN(_T("Mechanic Card: Login"));
const LPCTSTR IDS_MECHANIC_CARD_CHECKSUMS(_T("Mechanic Card: Checksums"));
const LPCTSTR IDS_TIER_POINTS(_T("Tier Points:"));
const LPCTSTR IDS_MECHANIC_MWCODE_TASK_BAR(_T("Task Bar"));
const LPCTSTR IDS_GLOBAL_CARD_CONTENT_WINDOW(_T("Global Card: Content Window"));
const LPCTSTR IDS_GLOBAL_CARD_TASK_BAR(_T("Global Card: Task Bar"));
const LPCTSTR IDS_GLOBAL_CARD_MEDIA_WINDOW_PLAYER_STATE(_T("Global Card: Media Window Player State"));
const LPCTSTR IDS_GLOBAL_CARD_PLAYER_STATE_CLOSE_WINDOW(_T("Global Card: Player State Close Window"));
const LPCTSTR IDS_BONUS_ANNOUNCEMENT_EVENT(_T("Bonus/Announcement Event"));
const LPCTSTR IDS_NA(_T("N/A"));
const LPCTSTR IDS_WEB_SESSION_ID(_T("Session ID"));
const LPCTSTR IDS_CONGRATSTTL(_T("Congrats TTL: %u<br>"));

const LPCTSTR IDS_WELCOMEPRIORITY(_T("Welcome Priority: %u<br>"));
const LPCTSTR IDS_SCHEDULEENDPRIORITY(_T("Schedule End Priority: %u<br>"));
const LPCTSTR IDS_SCHEDULEENDTTL(_T("Schedule End TTL: %u<br>"));
const LPCTSTR IDS_WINNERANTICIPATIONPRIORITY(_T("Winner Anticipation Priority: %u<br>"));
const LPCTSTR IDS_WINNERANTICIPATIONTTL(_T("Winner Anticipation TTL: %u<br>"));
const LPCTSTR IDS_ANNOUNCEMENTSPRIORITY(_T("Announcements Priority: %u<br>"));
const LPCTSTR IDS_ANNOUNCEMENTSTTL(_T("Announcements TTL: %u<br>"));
const LPCTSTR IDS_BIRTHDAYPRIORITY(_T("Birthday Priority: %u<br>"));
const LPCTSTR IDS_ANNIVERSARYPRIORITY(_T("Anniversary Priority: %u<br>"));
const LPCTSTR IDS_ANNIVERSARYTTL(_T("Anniversary TTL: %u<br>"));
const LPCTSTR IDS_PLAYERINITIATEDTTL(_T("Player Initiated TTL: %u<br>"));
const LPCTSTR IDS_INTERACTIVECONTENTTTL(_T("Interactive Content TTL: %u<br>"));
const LPCTSTR IDS_WELCOMETTL(_T("Welcome TTL: %u<br>"));
const LPCTSTR IDS_BIRTHDAYTTL(_T("Birthday TTL: %u<br>"));
const LPCTSTR IDS_SENTINELSTATUS_IDLE(_T("Idle"));
const LPCTSTR IDS_SENTINELSTATUS_CARDED(_T("Carded"));
const LPCTSTR IDS_SENTINELSTATUS_UNCARDED(_T("Uncarded"));
const LPCTSTR IDS_SENTINELSTATUS_UPGRADING(_T("Upgrading"));
const LPCTSTR IDS_SENTINELSTATUS_FAILEDRECOVERY(_T("Failed Recovery"));
const LPCTSTR IDS_SENTINELSTATUS_DOWNLOADING(_T("Downloading"));
const LPCTSTR IDS_SENTINELSTATUS_FILE_RETRIEVAL(_T("File Retrieval"));
const LPCTSTR IDS_SENTINELSTATUS_REBOOTING(_T("Rebooting"));
const LPCTSTR IDS_SENTINELSTATUS_UPGRADE_PENDING(_T("Upgrade Pending"));
const LPCTSTR IDS_SENTINELSTATUS_UI_RESTARTING(_T("UI Restarting"));
const LPCTSTR IDS_SENTINELSTATUS_UI_RESTART_PENDING(_T("UI Restart Pending"));
const LPCTSTR IDS_SENTINELSTATUS_UNKNOWN(_T("Unknown"));
const LPCTSTR IDS_MASSMARKETING_PRIORITY(_T("Mass Marketing Priority: %u<br>"));
const LPCTSTR IDS_MASSMARKETING_TTL(_T("Mass Marketing TTL: %u<br>"));
const LPCTSTR IDS_CDM_PRIORITY(_T("CDM Priority: %u<br>"));
const LPCTSTR IDS_CDM_TTL(_T("CDM TTL: %u<br>"));
const LPCTSTR IDS_UNATTENDED_JACKPOT_PRIORITY(_T("Unattended Jackpot Priority: %u<br>"));
const LPCTSTR IDS_UNATTENDED_JACKPOT_TTL(_T("Unattended Jackpot TTL: %u<br>"));
const LPCTSTR IDS_UNATTENDED_JACKPOT_MEDIA_PRIORITY(_T("Unattended Jackpot Media Priority: %u<br>"));
const LPCTSTR IDS_UNATTENDED_JACKPOT_MEDIA_TTL(_T("Unattended Jackpot Media TTL: %u<br>"));
const LPCTSTR IDS_FMT_BAD_POLLER_PACKET_SEND(_T("\r\n@@%s - Bad Poller Protocol packet created/sent. tiltCode=%u reason=""%s""\r\n"));
const LPCTSTR IDS_FMT_MSG_LEN_HEADER(_T("Message Length %u/%u is too small for Message Header."));
const LPCTSTR IDS_FMT_MSG_LEN_TOO_BIG(_T("Message Length %u/%u is too big for message %u."));
const LPCTSTR IDS_FMT_INVALID_PACKET_VERSION(_T("Invalid Packet Version %u."));
const LPCTSTR IDS_FMT_INVALID_MESSAGE_ID(_T("Invalid Message ID %u."));

const LPCTSTR IDS_FMT_INCORRECT_DATA_LEN(_T("Incorrect Data Length %u <> message length %u - header length %u."));
const LPCTSTR IDS_INVALID_MAC_ADDRESS(_T("Invalid MAC Address."));
const LPCTSTR IDS_WRONG_MAC_ADDRESS(_T("Wrong MAC Address."));
const LPCTSTR IDS_FMT_INVALID_SLOTMAST_ID(_T("Invalid SlotMastID %u."));
const LPCTSTR IDS_FMT_MSG_LEN_FIXED_DATA(_T("Message length %u/%u is too small for non-variable transaction data."));
const LPCTSTR IDS_FMT_INVALID_TRANS_CODE(_T("Invalid transaction code %u."));
const LPCTSTR IDS_FMT_INVALID_TRANS_TIME(_T("Invalid transaction time: %04u-%02u-%02u %02u:%02u:%02u.%03u."));
const LPCTSTR IDS_FMT_INVALID_UNIQUE_ID(_T("Invalid transaction unique ID %u."));
const LPCTSTR IDS_FMT_METER_NUMBER_MISSING(_T("Meter Number %u (%s) is missing from the transaction."));
const LPCTSTR IDS_FMT_MSG_LEN_METER_COUNT(_T("Message length %u/%u is too small for meter count."));
const LPCTSTR IDS_FMT_INVALID_METER_COUNT(_T("Invalid Meter Count %u."));
const LPCTSTR IDS_FMT_MSG_LEN_METERS(_T("Message length %u/%u is too small for meter count %u."));
const LPCTSTR IDS_FMT_INVALID_METER(_T("Invalid Meter: Number=%u (%s) Value=%lld."));
const LPCTSTR IDS_FMT_INVALID_METER_PLAYER(_T("Invalid Meter: Number=%u (%s) Value=%lld for player transaction."));
const LPCTSTR IDS_FMT_MSG_LEN_DATA_COUNT(_T("Message length %u/%u is too small for transaction data count."));

const LPCTSTR IDS_FMT_INVALID_DATA_COUNT(_T("Invalid Transaction Data Count %u."));
const LPCTSTR IDS_FMT_MSG_LEN_DATA_TYPE(_T("Message length %u/%u is too small for transaction data type with data count %u/%u."));
const LPCTSTR IDS_FMT_INVALID_DATA_TYPE(_T("Invalid Transaction Data Type %u with data count %u/%u."));
const LPCTSTR IDS_FMT_MSG_LEN_DATA_SIZE(_T("Message length %u/%u is too small for transaction data size %u with data count %u/%u."));
const LPCTSTR IDS_FMT_MSG_LEN_DATA_LEN(_T("Message length %u/%u is too small for transaction data length %u with data count %u/%u."));
const LPCTSTR IDS_FMT_MSG_LEN_TRANS_MSG(_T("Transaction Message < Message length %u/%u with data count %u/%u."));
const LPCTSTR IDS_FMT_MSG_LEN_INCONSISTENT(_T("Processed inconsistent message length %u/%u with data count %u/%u."));
const LPCTSTR IDS_LAST_PROGRESSIVE_AWARD(_T("Last Progressive Award"));
const LPCTSTR IDS_LAST_BONUS_AWARD   (_T("Last Bonus Award"));
const LPCTSTR IDS_PREVIOUS_STANDARD_SAS_PROGRESSIVE_WIN(_T("Previous Standard SAS Progressive Win"));
const LPCTSTR IDS_CURRENT_STANDARD_SAS_PROGRESSIVE_WINS(_T("Current Standard SAS Progressive Win(s)"));
const LPCTSTR IDS_FMT_RECEIVED_FROM_EGM_TIME(_T("Received from EGM Time: %s"));
const LPCTSTR IDS_FMT_SAS_LEVEL_ID   (_T("SAS Level ID: %d"));
const LPCTSTR IDS_FMT_SYSTEM_LEVEL_ID(_T("System Level ID: %d"));
const LPCTSTR IDS_FMT_JACKPOT_ID     (_T("Jackpot ID: %lld"));
const LPCTSTR IDS_FMT_AWARD_AMOUNT_HIT(_T("Award Amount Hit: %s"));
const LPCTSTR IDS_FMT_AWARD_STATE    (_T("Award State: %s"));

const LPCTSTR IDS_FMT_HIT_RECEIVED_FROM_SYSTEM(_T("Hit Received from System: %s"));
const LPCTSTR IDS_FMT_HAND_PAY_PENDING_SIGNALED(_T("Hand Pay Pending Signaled: %s"));
const LPCTSTR IDS_FMT_EGM_PAID_SIGNALED(_T("EGM Paid Signaled: %s"));
const LPCTSTR IDS_FMT_EGM_TYPE(_T("EGM Type: %s"));
const LPCTSTR IDS_STANDARD(_T("STANDARD"));
const LPCTSTR IDS_BALLY(_T("BALLY"));
const LPCTSTR IDS_WMS(_T("WMS"));
const LPCTSTR IDS_IGTS2000(_T("IGT S2000"));
const LPCTSTR IDS_CUSTOM(_T("CUSTOM"));
const LPCTSTR IDS_NONE(_T("NONE"));
const LPCTSTR IDS_FMT_EGM_PROTOCOL(_T("EGM Protocol: %s"));
const LPCTSTR IDS_SAS_3XX(_T("SAS 3.xx / 4.xx"));
const LPCTSTR IDS_SAS_5XX(_T("SAS 5.xx"));
const LPCTSTR IDS_SAS_6XX(_T("SAS 6.xx"));
const LPCTSTR IDS_FMT_REPORTED_EGM_PROTOCOL_VERSION(_T("Reported EGM Protocol Version: %s"));
const LPCTSTR IDS_FMT_EGM_SUPPORTS_TICKET_EXPIRATION_GT_255_DAYS(_T("EGM Supports SAS Long Poll 7B Ticket Expiration &gt 255 Days: %s"));
const LPCTSTR IDS_INVITED_GROUPS_ANTICIPATION_PRIORITY(_T("Invited Groups Anticipation Priority: %d<br>"));
const LPCTSTR IDS_INVITED_GROUPS_ANTICIPATION_TTL(_T("Invited Groups Anticipation TTL: %u<br>"));
const LPCTSTR IDS_INVITED_GROUPS_LEVEL_UNLOCKED_PRIORITY(_T("Invited Groups Level Unlocked Priority: %d<br>"));
const LPCTSTR IDS_INVITED_GROUPS_LEVEL_UNLOCKED_TTL(_T("Invited Groups Level Unlocked TTL: %u<br>"));
const LPCTSTR IDS_INVITED_GROUPS_LEADERBOARD_PRIORITY(_T("Invited Groups Leaderboard Priority: %d<br>"));
const LPCTSTR IDS_INVITED_GROUPS_LEADERBOARD_TTL(_T("Invited Groups Leaderboard TTL: %u<br>"));
const LPCTSTR IDS_INVITED_GROUPS_FINAL_RESULTS_PRIORITY(_T("Invited Groups Final Results Priority: %d<br>"));
const LPCTSTR IDS_INVITED_GROUPS_FINAL_RESULTS_TTL(_T("Invited Groups Final Results TTL: %u<br>"));
const LPCTSTR IDS_INVITED_GROUPS_ENDING_SOON_PRIORITY(_T("Invited Groups Ending Soon Priority: %d<br>"));
const LPCTSTR IDS_INVITED_GROUPS_ENDING_SOON_TTL(_T("Invited Groups Ending Soon TTL: %u<br>"));
const LPCTSTR IDS_PROGRESSIVE_PRIZE_PRIORITY(_T("Progressive Prize Priority: %d<br>"));
const LPCTSTR IDS_PROGRESSIVE_PRIZE_TTL(_T("Progressive Prize TTL: %u<br>"));
const LPCTSTR IDS_PROMO_GAME_PRIORITY(_T("Promo Game Priority: %d<br>"));
const LPCTSTR IDS_PROMO_GAME_TTL(_T("Promo Game TTL: %u<br>"));


// Resources for unit tests.
const LPCTSTR IDS_POINTS(_T("points"));
const LPCTSTR IDS_COMMA_YOU_HAVE(_T(", you have"));
const LPCTSTR IDS_YOU_HAVE(_T("You have"));
const LPCTSTR IDS_POINTS_TO_NEXT_TIER(_T("points to next tier"));
const LPCTSTR IDS_COMMA_YOU_NEED(_T(", you need"));
const LPCTSTR IDS_YOU_NEED(_T("You need"));
const LPCTSTR IDS_POINTS_TODAY(_T("points today"));
const LPCTSTR IDS_COMMA_YOU_HAVE_EARNED(_T(", you have earned"));
const LPCTSTR IDS_YOU_HAVE_EARNED(_T("You have earned"));
const LPCTSTR IDS_COMP(_T("comp"));
#endif//TODO-PORT
