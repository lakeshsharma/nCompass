#pragma once

#include "../QueueItem.h"
#include "../Meters.h"
#include "../Ticket.h"

namespace SentinelNativeUnitTests
{ class PollerMessageTests; }
namespace SentinelNativeUnitTests
{ class TransactionResendList; }
namespace SentinelNativeUnitTests
{ class PollerProtocolUnitTests; }

const WORD PACKET_VERSION_LOSS_LIMIT = 0x02;
const WORD PACKET_VERSION_NO_POINTS_FOR_PROMO = 0x03;
const WORD PACKET_VERSION_HIGHLANDER = 0x04;
const WORD PACKET_VERSION_CREDIT_TRANSFER = 0x05;
const WORD PACKET_VERSION_RANK = 0x06;
const WORD PACKET_TIER_POINTS = 0x07;
const WORD PACKET_VERSION_ONE_LINK_PROGRESIVES_SEN_OPTION = 0x08;
const WORD PACKET_VERSION_VOUCHER_SEPARATION = 0x09;
const WORD PACKET_VERSION_UNATTENDED_JACKPOT_AMOUNT = 0x0A;
const WORD PACKET_VERSION_MULTI_GAME_MULTI_DENOM = 0x0B;
const WORD PACKET_VERSION_CHUNKED_GAME_LIST = 0x0C;
const WORD PACKET_VERSION_EXTENSIBLE_SYSTEM_SETTINGS = 0x0D;
const WORD PACKET_VERSION_NEW_FLOOR_DESIGN = 0x0E;
const WORD PACKET_VERSION_EXTENDED_BUCKETS = 0x0F;

const WORD POLLER_PROTOCOL_VERSION = PACKET_VERSION_EXTENDED_BUCKETS;

const WORD PACKET_VERSION_MINIMUM = 3; // At most, the smallest that that nCompass would use.
const WORD PACKET_VERSION_MAXIMUM = 255; // At least as big as anything that nCompass would use.

const byte DATA_TYPE_MINIMUM = 1;
const byte DATA_TYPE_SESSION_POINTS = 1;
const byte DATA_TYPE_BONUS_POINTS   = 2;
const byte DATA_TYPE_RESIDUAL_VALUE = 3;
const byte DATA_TYPE_TOTAL_COIN_OUT = 4;
const byte DATA_TYPE_TILT_SUB_CODE = 5;
const byte DATA_TYPE_JACKPOT_AMOUNT = 6;
const byte DATA_TYPE_COINS_PLAYED = 7;
const byte DATA_TYPE_JACKPOT_TYPE = 8;
const byte DATA_TYPE_PROGRESSIVE_LEVEL = 9;
const byte DATA_TYPE_CHANGE_SLOTMAST = 10;
const byte DATA_TYPE_BONUS_EVENT_ID = 11;
const byte DATA_TYPE_CASH_TRANSFER_CODE = 12;
const byte DATA_TYPE_CASH_TRANSFER_COUNT = 13;
const byte DATA_TYPE_TICKET_NUMBER = 14;
const byte DATA_TYPE_TICKET_VALIDATION_NUMBER = 16;
const byte DATA_TYPE_TICKET_MACHINE_NUMBER = 17;
const byte DATA_TYPE_TICKET_EXPIRATION_TYPE = 18;
const byte DATA_TYPE_TICKET_EXPIRATION_DURATION = 19;
const byte DATA_TYPE_TICKET_SEQUENCE_NUMBER = 20;
const byte DATA_TYPE_TICKET_ACTION_CODE = 21;
const byte DATA_TYPE_TICKET_TYPE = 22;
const byte DATA_TYPE_AMOUNT_IN_PENNIES = 23;
const byte DATA_TYPE_MACHINE_TIME = 24;
const byte DATA_TYPE_PBT_ID = 25;
const byte DATA_TYPE_PBT_CARD_IN_DURATION = 26;
const byte DATA_TYPE_PBT_ORIGINAL_CREDIT_METER = 27;
const byte DATA_TYPE_PBT_CANCELLED_TYPE = 28;
const byte DATA_TYPE_PBT_TYPE = 29;
const byte DATA_TYPE_PBT_PIN = 30;
const byte DATA_TYPE_PBT_AMOUNT_IN_PENNIES = 31;
const byte DATA_TYPE_PBT_SEQUENCE_NUMBER = 32;
const byte DATA_TYPE_SNAPSHOT_TYPE = 33;
const byte DATA_TYPE_GAME_COMM_DOWN = 34;
const byte DATA_TYPE_GAME_NUMBER = 35;
const byte DATA_TYPE_DENOM = 36;
const byte DATA_TYPE_MACHINE_TYPE = 37;
const byte DATA_TYPE_MACHINE_NUMBER = 38;
const byte DATA_TYPE_METER_NUMBER = 39;
const byte DATA_TYPE_METER_VALUE = 40;
const byte DATA_TYPE_HOPPER_METER_VALUE = 41;
const byte DATA_TYPE_SUBGAME_METERS = 42;
const byte DATA_TYPE_KEYPAD_ENTRY_TYPE = 43;
const byte DATA_TYPE_FAILURE_CODE = 44;
const byte DATA_TYPE_MAINTENANCE_CODE = 45;
const byte DATA_TYPE_EGM_PROTOCOL_CODE = 46;
const byte DATA_TYPE_CONFIG_CHANGE_TYPE_CODE = 47;
const byte DATA_TYPE_CONFIG_CHANGE_BEFORE_CODE = 48;
const byte DATA_TYPE_CONFIG_CHANGE_AFTER_CODE = 49;
const byte DATA_TYPE_PIN_NUMBER_CODE = 50;
const byte DATA_TYPE_BILL_COUNT = 51;
const byte DATA_TYPE_BILL_TYPE = 52;
const byte DATA_TYPE_MANUFACTURER_ID = 53;
const byte DATA_TYPE_PAYTABLE_ID = 54;
const byte DATA_TYPE_ADDITIONAL_ID = 55;
const byte DATA_TYPE_ACTIVE_SECONDS = 56;
const byte DATA_TYPE_MAX_CARD_INDICATOR = 60; //apparently 57 - 59 are in use on the Poller client.
const byte DATA_TYPE_VIRTUAL_SESSION_INDICATOR = 61;
const byte DATA_TYPE_PBT_AUTHORIZATION = 62;
const byte DATA_TYPE_KEY_TO_CREDIT_AVAILABLE = 63;
const byte DATA_TYPE_JACKPOT_ID = 64;
const byte DATA_TYPE_UNATTENDED_JACKPOT_STATUS = 65;
const byte DATA_TYPE_PLAY_SESSION_ID = 66;
const byte DATA_TYPE_PLAY_TRANSACTION_SEQUENCE = 67;
const byte DATA_TYPE_MGMD_AVAILABLE = 68;
const byte DATA_TYPE_FEDERAL_TAX_WITHHELD = 69;
const byte DATA_TYPE_FEDERAL_TAX_OVERRIDDEN = 70;
const byte DATA_TYPE_STATE_TAX_OVERRIDDEN = 71;
// Add new types here (or above) and update DATA_TYPE_MAXIMUM below.

const byte DATA_TYPE_MAXIMUM = DATA_TYPE_STATE_TAX_OVERRIDDEN; // Update this item when adding data types.

const int POLLER_PAY_TABLE_ID_LEN = 6;
const int POLLER_MFG_ID_LEN = 2;
const int POLLER_ADDITIONAL_ID_LEN = 3;

typedef enum
{
	POLLER_MSG_NONE = 0,
	POLLER_MSG_MINIMUM = 300, // Set this to the lowest valid enumeration, or less.
	POLLER_MSG_SENTINEL_HEARTBEAT = 300,
	POLLER_MSG_REQUEST_CONFIG = 301,
	POLLER_MSG_SLOTMASTID_CHANGE = 302,
	POLLER_MSG_SENTINEL_STATUS = 303,
	POLLER_MSG_METERS = 304,
	POLLER_MSG_SUBGAME_METERS = 305,
	POLLER_MSG_SENTINEL_TIME = 306,
	POLLER_MSG_GLOBAL_PASSWORD = 307,
	POLLER_MSG_DISPLAY_MESSAGE = 308,
	POLLER_MSG_PLAYER_UPDATE_EVENTS = 309,
	POLLER_MSG_TRANSACTION = 310,
	POLLER_MSG_ALL_GAME_LIST = 322,
	POLLER_MSG_SUBGAME_SELECTION = 323,
	POLLER_MSG_SENTINEL_HEARTBEAT_RESP = 400,
	POLLER_MSG_SENTINEL_CONFIG = 401,
	POLLER_MSG_TRANSACTION_ACK = 410,
	POLLER_MSG_PLAYER_RESPONSE = 411,
	POLLER_MSG_PLAYER_BONUS_EVENTS = 412,
	POLLER_MSG_SLOTMASTID_CHANGE_RESP = 413,
	POLLER_MSG_TICKET = 414,
	POLLER_MSG_PBT_RESPONSE = 415,
	POLLER_MSG_EMPLOYEE_RESPONSE = 416,
	POLLER_MSG_FLASH_INITIALIZE = 311,
	POLLER_MSG_FLASH_DATA_BLOCK = 313,
	POLLER_MSG_FLASH_STATUS = 312,
	POLLER_MSG_FLASH_BURN = 314,
	POLLER_MSG_FLASH_STATUS_RESPONSE = 316,
	POLLER_MSG_FLASH_INITIALIZE_RESPONSE = 315,
	POLLER_MSG_REQUEST_LOSS_LIMIT_BALANCE_UPDATE = 317,
	POLLER_MSG_LOSS_LIMIT_BALANCE_UPDATE = 417,
	POLLER_MSG_GLOBAL_BALANCE = 418,
	POLLER_MSG_AUTHORIZATION_RESPONSE = 419,
	POLLER_MSG_PROCESS_UNATTENDED_JACKPOT = 420,
	POLLER_MSG_AUTHORIZE_PIN_RESPONSE = 421,
	POLLER_MSG_SUBGAME_SELECTION_RESPONSE = 423,
	POLLER_MSG_REBOOT_REQUEST = 424,
	POLLER_MSG_PIN_RESET_RESPONSE = 425,
	POLLER_MSG_PROXIMITY_BEACON_SCAN_LIST_RESPONSE = 426,
	POLLER_MSG_MOBILE_CONNECT_RESPONSE = 427,
	// Add new types here (or above) and update POLLER_MSG_MAXIMUM below.

	POLLER_MSG_MAXIMUM = POLLER_MSG_MOBILE_CONNECT_RESPONSE, // Set this to the highest valid enumeration, or more.
} PollerMsgType;

typedef enum
{
	NONE_SPECIFIED = 0,
	SENTINEL_ON_MEM_BAD_CODE = 1,
	PLAYER_CARD_IN_CODE = 2,
	EXTERNAL_BONUS_CODE = 3,
	DOOR_OPEN_CODE = 4,
	DOOR_CLOSED_CODE = 5,
	DROP_DOOR_OPEN_CODE = 16,
	DROP_DOOR_CLOSED_CODE = 17,
	LOGIC_DOOR_OPEN_CODE = 18,
	LOGIC_DOOR_CLOSED_CODE = 19,
	CHANGE_LIGHT_ON_CODE = 20,
	CHANGE_LIGHT_OFF_CODE = 21,
	ILLEGAL_DOOR_OPEN_CODE = 22,
	PBT_RESET_PIN = 23,
	SLOT_DOOR_OPEN_CODE = 31,
	SLOT_DOOR_CLOSED_CODE = 32,
	BELLY_DOOR_OPEN_CODE = 34,
	BELLY_DOOR_CLOSED_CODE = 35,
	SLOT_POWER_OFF_CODE = 40,
	SLOT_POWER_ON_CODE = 41,
	SENTINEL_ON_MEM_OK_CODE = 42,
	CONTROLLED_REBOOT_WARNING_CODE = 44,
	CONTROLLED_REBOOT_CODE = 45,
	AUTO_UPLOAD_CANCELED_BY_USER = 46,
	CHASSIS_DOOR_OPEN_CODE = 49,
	CHASSIS_DOOR_CLOSED_CODE = 50,
	BILL_ACCEPTED_CODE = 53,
	MAINTENANCE_CODE = 59,
	PLAYER_CARD_OUT_CODE = 60,
	GAME_INFO_CODE = 61,
	JACKPOT_FILL_CODE = 62,
	PLAYER_UPDATE_CODE = 63,
	FAILURE_CODE = 64,
	MECHANIC_CARD_IN_CODE = 66,
	MECHANIC_CARD_OUT_CODE = 67,
	FLOOR_CARD_IN_CODE = 68,
	FLOOR_CARD_OUT_CODE = 69,
	POSSIBLE_BAD_CARD_READER_CODE = 70,
	METER_RUNAWAY_CODE = 71,
	CASH_DOOR_OPEN_CODE = 78,
	CASH_DOOR_CLOSED_CODE = 79,
	STACKER_REMOVED_CODE = 80,
	STACKER_INSTALLED_CODE = 81,
	MACHINE_ONLINE_CODE = 83,
	MACHINE_MOVE_COMPLETE_CODE = 85,
	BEGIN_MACHINE_MOVE_CODE = 87,
	MACHINE_OFFLINE_CODE = 88,
	PROGRESSIVE_JP_HOPPER_CODE = 89,
	PERSONAL_BANKER_CODE = 90,
	GAME_COMM_DOWN_CODE = 91,
	COMPOSITE_JACKPOT_CODE = 93,
	COMPOSITE_TILT = 94,
	ONE_TIME_BONUS_EVENT_CODE = 99,
	ONE_LINK_AWARD_HIT = 100,
	ONE_LINK_AWARD_STATUS = 101,
	GAME_COMM_UP_CODE = 102,
	FILL_DOOR_OPEN_CODE = 105,
	FILL_DOOR_CLOSED_CODE = 106,
	TICKET_CODE = 107,
	WITNESS_CODE = 108,
	CASH_TRANSFER_TRACKING_CODE = 109,
	GLOBAL_CARD_IN_CODE = 110,
	GLOBAL_CARD_OUT_CODE = 111,
	CURRENCY_CARD_IN_CODE = 112,
	CURRENCY_CARD_OUT_CODE = 113,
	METER_CARD_IN_CODE = 114,
	METER_CARD_OUT_CODE = 115,
	PROGRESSIVE_JP_SCANNER_CODE = 116,
	CONFIG_CHANGE_CODE = 120,
	EGM_SNAPSHOT_CODE = 121,
	SUBGAME_SNAPSHOT_CODE = 122,
	MULTIPLE_BONUS_EVENT_CODE = 123,
	CHANGE_SLOTMAST_ID_CODE = 124,
	AUTHORIZE_PIN = 125,
	UNATTENDED_JACKPOT_RESULT = 126,
	GAME_SESSION_CHANGE = 127,
	CHASSIS_DOOR_OPEN_POWER_DOWN_CODE = 128,
	EXCESSIVE_TICKET_OUT = 137,
	CARDLESS_SESSION_START_CODE = 138,
	CARDLESS_SESSION_END_CODE = 139,
	EGM_LOCK_ACK = 145,
	EGM_UNLOCK_ACK = 146,
	EGM_LOCK_NACK = 147,
	EGM_UNLOCK_NACK = 148,
	VIRTUAL_SESSION_STARTED = 149,
	PROXIMITY_REQUEST_BEACON_SCAN_LIST = 150,
	MOBILE_CONNECT_PAIRING_SETTINGS_REQUEST = 155,
	MOBILE_CONNECT_PAIRING_REQUEST = 156,
	MOBILE_CONNECT_PAIRING_STATUS = 157,
	MOBILE_CONNECT_PLAYER_CHANGE = 158,
	MOBILE_CONNECT_CARD_CHANGE = 159,
	MOBILE_CONNECT_PLAYER_SESSION_CHANGE = 160,
	MOBILE_CONNECT_PBT_BALANCE = 161,
	MOBILE_CONNECT_PBT_FUNDS_TRANSFER = 162,
	MOBILE_CONNECT_END_PBT_SESSION = 163,
	PROXIMITY_SETTINGS_REQUEST = 164,
	PROXIMITY_STATUS = 165,
	// Add new types here (or above) and update TRANSACTION_CODE_MAXIMUM below.

	TRANSACTION_CODE_MAXIMUM = PROXIMITY_STATUS // Set this to the highest valid enumeration, or more.
} TransactionType;

typedef struct _PollerMeter
{
	byte Number;
	LONGLONG Value;
}PollerMeter;

static const int SIZE_OF_MESSAGE_HEADER = 22;
static const int SIZE_OF_UTC_TIME = 16;
static const int PACKET_VERSION_OFFSET = 0;

//#pragma pack (1)
typedef struct _MessageHeader
{
	WORD PacketVersion;
	WORD MessageID;
	WORD DataLength;
	BYTE MacAddress[SIZE_OF_MAC_ADDRESS];
	DWORD SequenceNumber;
	DWORD SlotMastID;
	WORD SlotmastRevision;
}PollerMessageHeader;

#define NONVARIABLE_TRANSACTION_SIZE  30
#define MAX_MOBILE_CONNECT_MESSAGE_SIZE		  (5 * 1024)

class CPollerMessage :
	public CQueueItem
{
public:
	CPollerMessage();
	virtual ~CPollerMessage();

	virtual byte* GetReadWriteBuffer(long& bufferSize);

	static std::string TransactionTypeToString(TransactionType aType);
	static std::string PollerMsgTypeToString(PollerMsgType aMsgType);

	PollerMessageHeader MessageHeader;
	bool IsPMTDependent(void);
protected:
	byte *m_lpBuffer;
	int m_bufferSize;

	/// <summary>
	/// Appends timestamp to buffer at given offset
	/// </summary>
	/// <param name="offset">Buffer offset to append 
	/// timestamp to</param> 
	/// <returns>Next buffer location to append to.</returns>
	int AppendUTCTimeStamp(int offset);

	/// <summary>
	/// Appends timestamp to buffer at given offset
	/// </summary>
	/// <param name="offset">Buffer offset to append 
	/// timestamp to</param> 
	/// <param name="timestamp">Timestamp to append.</param> 
	/// <returns>Next buffer location to append to.</returns>
	int AppendUTCTimeStamp(int offset, SYSTEMTIME timestamp);

private:
	friend SentinelNativeUnitTests::PollerMessageTests;

	FRIEND_TEST(PollerMessageTests, PollerMessage_ctor);
	FRIEND_TEST(PollerMessageTests, PollerMessage_Derived_GetReadWriteBuffer);
};
