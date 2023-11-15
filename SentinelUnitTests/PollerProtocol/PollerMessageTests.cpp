#include "stdafx.h"
#include "PollerProtocol/PollerMessage.h"
#include "PollerProtocol/PollerPlayerTransactionMessage.h"
#include "PollerProtocol/PollerTicketTransactionMessage.h"
#include "PollerProtocol/PollerProximityBeaconScanListResponseMessage.h"
#include "PollerProtocol/TransactionResendList.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "AssertMsg.h"
#include "TestUtil.h"

const time_t DEFAULT_TIME=0; // Arbitrary value when any valid value is okay.
using namespace std;

	class PollerMessageTests : public ::testing::Test
	{
	public:
	};

		TEST(PollerMessageTests, PollerMessage_ctor)
		{
			CPollerMessage msg;

			ASSERT_EQ((void *)nullptr, (void *)msg.m_lpBuffer) <<  L"m_lpBuffer";
			ASSERT_EQ(0, msg.m_bufferSize) << L"m_bufferSize";

			char* p = (char*) &msg.MessageHeader;
			for (size_t i = 0; i < sizeof(msg.MessageHeader); i++, p++)
				ASSERT_EQ(0, (int) *p);
		}

		TEST(PollerMessageTests, PollerMessage_GetReadWriteBuffer)
		{
			CPollerMessage msg;

			long bufferSize;
			byte* pBuffer = msg.GetReadWriteBuffer(bufferSize);

			ASSERT_EQ((void *)nullptr, (void *)pBuffer) << L"pBuffer";
			ASSERT_EQ((long) 0, bufferSize) << L"bufferSize";
		}

		TEST(PollerMessageTests, PollerMessage_Derived_ctor)
		{
			class CTest : public CPollerMessage
			{
			public:
				CTest()
				{
					m_bufferSize = 4;
					m_lpBuffer = new byte[4];
					memcpy(m_lpBuffer, "abc", 4);
				}
			};

			CTest msg;

			long bufferSize;
			byte* pBuffer = msg.GetReadWriteBuffer(bufferSize);

			ASSERT_NE((void *)nullptr, (void *)pBuffer) << L"pBuffer";
			ASSERT_EQ(0, strcmp("abc", (char*) pBuffer)) << L"pBuffer contents";
			ASSERT_EQ((long) 4, bufferSize) << L"bufferSize";
		}

		TEST(PollerMessageTests, PollerMessage_Derived_GetReadWriteBuffer)
		{
			class CTest : public CPollerMessage
			{
			public:
				CTest()
				{
				}

				byte *GetReadWriteBuffer(long &bufferSize)
				{
					bufferSize = 4;
					return (byte*) "abc";
				}
			};

			CTest msg;

			ASSERT_EQ((void *)nullptr, (void *)msg.m_lpBuffer) << L"m_lpBuffer";
			ASSERT_EQ(0, msg.m_bufferSize) << L"m_bufferSize";

			long bufferSize;
			byte* pBuffer = msg.GetReadWriteBuffer(bufferSize);

			ASSERT_NE((void *)nullptr, (void *)pBuffer) << L"pBuffer";
			ASSERT_EQ("abc", (char*) pBuffer) << L"pBuffer contents";
			ASSERT_EQ((long) 4, bufferSize) << L"bufferSize";
		}

		TEST(PollerMessageTests, PollerMessage_TransactionTypeToString)
		{
			struct
			{
				int line;
				TransactionType type;
				const char* pStr;
			}
			static data[] =
			{
				{ __LINE__, NONE_SPECIFIED, "NONE_SPECIFIED" },
				{ __LINE__, SENTINEL_ON_MEM_BAD_CODE, "SENTINEL_ON_MEM_BAD_CODE" },
				{ __LINE__, PLAYER_CARD_IN_CODE, "PLAYER_CARD_IN_CODE" },
				{ __LINE__, EXTERNAL_BONUS_CODE, "EXTERNAL_BONUS_CODE" },
				{ __LINE__, DOOR_OPEN_CODE, "DOOR_OPEN_CODE" },
				{ __LINE__, DOOR_CLOSED_CODE, "DOOR_CLOSED_CODE" },
				{ __LINE__, DROP_DOOR_OPEN_CODE, "DROP_DOOR_OPEN_CODE" },
				{ __LINE__, DROP_DOOR_CLOSED_CODE, "DROP_DOOR_CLOSED_CODE" },
				{ __LINE__, LOGIC_DOOR_OPEN_CODE, "LOGIC_DOOR_OPEN_CODE" },
				{ __LINE__, LOGIC_DOOR_CLOSED_CODE, "LOGIC_DOOR_CLOSED_CODE" },
				{ __LINE__, CHANGE_LIGHT_ON_CODE, "CHANGE_LIGHT_ON_CODE" },
				{ __LINE__, CHANGE_LIGHT_OFF_CODE, "CHANGE_LIGHT_OFF_CODE" },
				{ __LINE__, ILLEGAL_DOOR_OPEN_CODE, "ILLEGAL_DOOR_OPEN_CODE" },
				{ __LINE__, SLOT_DOOR_OPEN_CODE, "SLOT_DOOR_OPEN_CODE" },
				{ __LINE__, SLOT_DOOR_CLOSED_CODE, "SLOT_DOOR_CLOSED_CODE" },
				{ __LINE__, BELLY_DOOR_OPEN_CODE, "BELLY_DOOR_OPEN_CODE" },
				{ __LINE__, BELLY_DOOR_CLOSED_CODE, "BELLY_DOOR_CLOSED_CODE" },
				{ __LINE__, SLOT_POWER_OFF_CODE, "SLOT_POWER_OFF_CODE" },
				{ __LINE__, SLOT_POWER_ON_CODE, "SLOT_POWER_ON_CODE" },
				{ __LINE__, SENTINEL_ON_MEM_OK_CODE, "SENTINEL_ON_MEM_OK_CODE" },
				{ __LINE__, CONTROLLED_REBOOT_WARNING_CODE, "CONTROLLED_REBOOT_WARNING_CODE" },
				{ __LINE__, CONTROLLED_REBOOT_CODE, "CONTROLLED_REBOOT_CODE" },
				{ __LINE__, AUTO_UPLOAD_CANCELED_BY_USER, "AUTO_UPLOAD_CANCELED_BY_USER" },
				{ __LINE__, CHASSIS_DOOR_OPEN_CODE, "CHASSIS_DOOR_OPEN_CODE" },
				{ __LINE__, CHASSIS_DOOR_CLOSED_CODE, "CHASSIS_DOOR_CLOSED_CODE" },
				{ __LINE__, BILL_ACCEPTED_CODE, "BILL_ACCEPTED_CODE" },
				{ __LINE__, MAINTENANCE_CODE, "MAINTENANCE_CODE" },
				{ __LINE__, PLAYER_CARD_OUT_CODE, "PLAYER_CARD_OUT_CODE" },
				{ __LINE__, GAME_INFO_CODE, "GAME_INFO_CODE" },
				{ __LINE__, JACKPOT_FILL_CODE, "JACKPOT_FILL_CODE" },
				{ __LINE__, PLAYER_UPDATE_CODE, "PLAYER_UPDATE_CODE" },
				{ __LINE__, FAILURE_CODE, "FAILURE_CODE" },
				{ __LINE__, MECHANIC_CARD_IN_CODE, "MECHANIC_CARD_IN_CODE" },
				{ __LINE__, MECHANIC_CARD_OUT_CODE, "MECHANIC_CARD_OUT_CODE" },
				{ __LINE__, FLOOR_CARD_IN_CODE, "FLOOR_CARD_IN_CODE" },
				{ __LINE__, FLOOR_CARD_OUT_CODE, "FLOOR_CARD_OUT_CODE" },
				{ __LINE__, POSSIBLE_BAD_CARD_READER_CODE, "POSSIBLE_BAD_CARD_READER_CODE" },
				{ __LINE__, METER_RUNAWAY_CODE, "METER_RUNAWAY_CODE" },
				{ __LINE__, CASH_DOOR_OPEN_CODE, "CASH_DOOR_OPEN_CODE" },
				{ __LINE__, CASH_DOOR_CLOSED_CODE, "CASH_DOOR_CLOSED_CODE" },
				{ __LINE__, STACKER_REMOVED_CODE, "STACKER_REMOVED_CODE" },
				{ __LINE__, STACKER_INSTALLED_CODE, "STACKER_INSTALLED_CODE" },
				{ __LINE__, MACHINE_ONLINE_CODE, "MACHINE_ONLINE_CODE" },
				{ __LINE__, MACHINE_MOVE_COMPLETE_CODE, "MACHINE_MOVE_COMPLETE_CODE" },
				{ __LINE__, BEGIN_MACHINE_MOVE_CODE, "BEGIN_MACHINE_MOVE_CODE" },
				{ __LINE__, MACHINE_OFFLINE_CODE, "MACHINE_OFFLINE_CODE" },
				{ __LINE__, PROGRESSIVE_JP_HOPPER_CODE, "PROGRESSIVE_JP_HOPPER_CODE" },
				{ __LINE__, PERSONAL_BANKER_CODE, "PERSONAL_BANKER_CODE" },
				{ __LINE__, GAME_COMM_DOWN_CODE, "GAME_COMM_DOWN_CODE" },
				{ __LINE__, COMPOSITE_JACKPOT_CODE, "COMPOSITE_JACKPOT_CODE" },
				{ __LINE__, COMPOSITE_TILT, "COMPOSITE_TILT" },
				{ __LINE__, ONE_TIME_BONUS_EVENT_CODE, "ONE_TIME_BONUS_EVENT_CODE" },
				{ __LINE__, ONE_LINK_AWARD_HIT, "ONE_LINK_AWARD_HIT" },
				{ __LINE__, ONE_LINK_AWARD_STATUS, "ONE_LINK_AWARD_STATUS" },
				{ __LINE__, GAME_COMM_UP_CODE, "GAME_COMM_UP_CODE" },
				{ __LINE__, FILL_DOOR_OPEN_CODE, "FILL_DOOR_OPEN_CODE" },
				{ __LINE__, FILL_DOOR_CLOSED_CODE, "FILL_DOOR_CLOSED_CODE" },
				{ __LINE__, TICKET_CODE, "TICKET_CODE" },
				{ __LINE__, WITNESS_CODE, "WITNESS_CODE" },
				{ __LINE__, CASH_TRANSFER_TRACKING_CODE, "CASH_TRANSFER_TRACKING_CODE" },
				{ __LINE__, GLOBAL_CARD_IN_CODE, "GLOBAL_CARD_IN_CODE" },
				{ __LINE__, GLOBAL_CARD_OUT_CODE, "GLOBAL_CARD_OUT_CODE" },
				{ __LINE__, CURRENCY_CARD_IN_CODE, "CURRENCY_CARD_IN_CODE" },
				{ __LINE__, CURRENCY_CARD_OUT_CODE, "CURRENCY_CARD_OUT_CODE" },
				{ __LINE__, METER_CARD_IN_CODE, "METER_CARD_IN_CODE" },
				{ __LINE__, METER_CARD_OUT_CODE, "METER_CARD_OUT_CODE" },
				{ __LINE__, PROGRESSIVE_JP_SCANNER_CODE, "PROGRESSIVE_JP_SCANNER_CODE" },
				{ __LINE__, CONFIG_CHANGE_CODE, "CONFIG_CHANGE_CODE" },
				{ __LINE__, EGM_SNAPSHOT_CODE, "EGM_SNAPSHOT_CODE" },
				{ __LINE__, SUBGAME_SNAPSHOT_CODE, "SUBGAME_SNAPSHOT_CODE" },
				{ __LINE__, MULTIPLE_BONUS_EVENT_CODE, "MULTIPLE_BONUS_EVENT_CODE" },
				{ __LINE__, CHANGE_SLOTMAST_ID_CODE, "CHANGE_SLOTMAST_ID_CODE" },
				{ __LINE__, CHASSIS_DOOR_OPEN_POWER_DOWN_CODE, "CHASSIS_DOOR_OPEN_POWER_DOWN_CODE" },
				{ __LINE__, EXCESSIVE_TICKET_OUT, "EXCESSIVE_TICKET_OUT" },
				{ __LINE__, CARDLESS_SESSION_START_CODE, "CARDLESS_SESSION_START_CODE" },
				{ __LINE__, CARDLESS_SESSION_END_CODE, "CARDLESS_SESSION_END_CODE" }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string s = CPollerMessage::TransactionTypeToString(data[i].type);
				ASSERT_EQ(data[i].pStr, s) << AssertMsg::Format(data[i].line, nullptr);
			}
		}

		TEST(PollerMessageTests, PollerMessage_TransactionTypeToString_Bad)
		{
			string s = CPollerMessage::TransactionTypeToString((TransactionType) INT_MAX);
			ASSERT_EQ("Unknown transaction type", s);
		}

		TEST(PollerMessageTests, PollerMessage_TransactionTypeToString_Bad2)
		{
			string s = CPollerMessage::TransactionTypeToString((TransactionType) -1);
			ASSERT_EQ("Unknown transaction type", s);
		}

		TEST(PollerMessageTests, PollerMessage_PollerMsgTypeToString)
		{
			struct
			{
				int line;
				PollerMsgType type;
				const char* pStr;
			}
			static data[] =
			{
				{ __LINE__, POLLER_MSG_NONE, "POLLER_MSG_NONE" },
				{ __LINE__, POLLER_MSG_SENTINEL_HEARTBEAT, "POLLER_MSG_SENTINEL_HEARTBEAT" },
				{ __LINE__, POLLER_MSG_REQUEST_CONFIG, "POLLER_MSG_REQUEST_CONFIG" },
				{ __LINE__, POLLER_MSG_SLOTMASTID_CHANGE, "POLLER_MSG_SLOTMASTID_CHANGE" },
				{ __LINE__, POLLER_MSG_SENTINEL_STATUS, "POLLER_MSG_SENTINEL_STATUS" },
				{ __LINE__, POLLER_MSG_METERS, "POLLER_MSG_METERS" },
				{ __LINE__, POLLER_MSG_SUBGAME_METERS, "POLLER_MSG_SUBGAME_METERS" },
				{ __LINE__, POLLER_MSG_SENTINEL_TIME, "POLLER_MSG_SENTINEL_TIME" },
				{ __LINE__, POLLER_MSG_GLOBAL_PASSWORD, "POLLER_MSG_GLOBAL_PASSWORD" },
				{ __LINE__, POLLER_MSG_DISPLAY_MESSAGE, "POLLER_MSG_DISPLAY_MESSAGE" },
				{ __LINE__, POLLER_MSG_PLAYER_UPDATE_EVENTS, "POLLER_MSG_PLAYER_UPDATE_EVENTS" },
				{ __LINE__, POLLER_MSG_TRANSACTION, "POLLER_MSG_TRANSACTION" },
				{ __LINE__, POLLER_MSG_SENTINEL_HEARTBEAT_RESP, "POLLER_MSG_SENTINEL_HEARTBEAT_RESP" },
				{ __LINE__, POLLER_MSG_SENTINEL_CONFIG, "POLLER_MSG_SENTINEL_CONFIG" },
				{ __LINE__, POLLER_MSG_TRANSACTION_ACK, "POLLER_MSG_TRANSACTION_ACK" },
				{ __LINE__, POLLER_MSG_PLAYER_RESPONSE, "POLLER_MSG_PLAYER_RESPONSE" },
				{ __LINE__, POLLER_MSG_PLAYER_BONUS_EVENTS, "POLLER_MSG_PLAYER_BONUS_EVENTS" },
				{ __LINE__, POLLER_MSG_SLOTMASTID_CHANGE_RESP, "POLLER_MSG_SLOTMASTID_CHANGE_RESP" },
				{ __LINE__, POLLER_MSG_TICKET, "POLLER_MSG_TICKET" },
				{ __LINE__, POLLER_MSG_PBT_RESPONSE, "POLLER_MSG_PBT_RESPONSE" },
				{ __LINE__, POLLER_MSG_EMPLOYEE_RESPONSE, "POLLER_MSG_EMPLOYEE_RESPONSE" },
				{ __LINE__, POLLER_MSG_FLASH_INITIALIZE, "POLLER_MSG_FLASH_INITIALIZE" },
				{ __LINE__, POLLER_MSG_FLASH_DATA_BLOCK, "POLLER_MSG_FLASH_DATA_BLOCK" },
				{ __LINE__, POLLER_MSG_FLASH_STATUS, "POLLER_MSG_FLASH_STATUS" },
				{ __LINE__, POLLER_MSG_FLASH_BURN, "POLLER_MSG_FLASH_BURN" },
				{ __LINE__, POLLER_MSG_FLASH_STATUS_RESPONSE, "POLLER_MSG_FLASH_STATUS_RESPONSE" },
				{ __LINE__, POLLER_MSG_FLASH_INITIALIZE_RESPONSE, "POLLER_MSG_FLASH_INITIALIZE_RESPONSE" },
				{ __LINE__, POLLER_MSG_REQUEST_LOSS_LIMIT_BALANCE_UPDATE, "POLLER_MSG_REQUEST_LOSS_LIMIT_BALANCE_UPDATE" },
				{ __LINE__, POLLER_MSG_LOSS_LIMIT_BALANCE_UPDATE, "POLLER_MSG_LOSS_LIMIT_BALANCE_UPDATE" },
				{ __LINE__, POLLER_MSG_GLOBAL_BALANCE, "POLLER_MSG_GLOBAL_BALANCE" },
				{ __LINE__, POLLER_MSG_AUTHORIZATION_RESPONSE, "POLLER_MSG_AUTHORIZATION_RESPONSE" },
				{ __LINE__, POLLER_MSG_PROXIMITY_BEACON_SCAN_LIST_RESPONSE, "POLLER_MSG_PROXIMITY_BEACON_SCAN_LIST_RESPONSE" }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string s = CPollerMessage::PollerMsgTypeToString(data[i].type);
				ASSERT_EQ(data[i].pStr, s) << AssertMsg::Format(data[i].line, nullptr);
			}
		}

		TEST(PollerMessageTests, PollerMessage_PollerMsgTypeToString_Bad)
		{
			string s = CPollerMessage::PollerMsgTypeToString((PollerMsgType) INT_MAX);
			ASSERT_EQ("Unknown transaction type", s);
		}

		TEST(PollerMessageTests, PollerMessage_PollerMsgTypeToString_Bad2)
		{
			string s = CPollerMessage::PollerMsgTypeToString((PollerMsgType) -1);
			ASSERT_EQ("Unknown transaction type", s);
        }

        /// <summary>
        /// Checks if ticket transaction is PMT independent
    	/// </summary>
       TEST(PollerMessageTests, PollerMsg_PMTDependent)
		{
            // Data.
            CMeters *meters = (new CMeters(MEMORY_NONE));
            CTicket  *ticket = (new CTicket());
            byte transType = TICKET_CODE;
            const byte * cardID = reinterpret_cast<const byte*>("1234567890");
            
            CPollerTicketTransactionMessage pollerMsg(meters, ticket, transType, 0, cardID);
            pollerMsg.MessageHeader.PacketVersion = POLLER_PROTOCOL_VERSION;
            pollerMsg.SetReadWriteBuffer();
            DWORD transLength(0);
            byte *transBuffer(pollerMsg.GetReadWriteBuffer(transLength));
	
            ASSERT_EQ(pollerMsg.IsPMTDependent(), false) <<  "Transaction is PMT dependent";
		}

        /// <summary>
        /// Checks if non-ticket transaction is PMT dependent
    	/// </summary>
        TEST(PollerMessageTests, PollerMsg_isNotPMTDependent)
		{
            const __int64 activeSecondsExpected(60);
            const time_t lastSessionStartTime(CUtilities::GetTimeFromFields(2015, 10, 20, 12, 23, 34));
            time_t transTime(lastSessionStartTime + activeSecondsExpected); // Need read/write value.

            // Setup CPlayer for physical card out with no virtual session.
            CCard *card = new CCard(lastSessionStartTime, lastSessionStartTime);
            card->SetCardID(_T("1234567890"));
            const CConfig config(false, "");
            const CEGMConfig egmConfig(MEMORY_NONE);

            const time_t utcRemovalTime(transTime - time_t(5));
            const time_t removalTime(utcRemovalTime + -7 * 3600);
            const bool makeVirtual(false);
            CMeters *virtualStartMeters(nullptr);

            CGames *games = new CGames();
            CPlayerInfo *playerInfo = new CPlayerInfo();

            CPlayer *player = new CPlayer(card, playerInfo, config, egmConfig, DEFAULT_TIME, false, games);
            player->CardRemoved(removalTime, utcRemovalTime, makeVirtual);
            ASSERT_EQ(lastSessionStartTime, player->GetLastSessionStartTime()); // Verify setup.

            CMeters *meters = new CMeters(MEMORY_NONE);

            // Perform operation(s) and test(s).
            CPollerPlayerTransactionMessage pollerMsg(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime, 0);
            pollerMsg.MessageHeader.PacketVersion = POLLER_PROTOCOL_VERSION;
            DWORD transLength(0);
            byte *transBuffer(pollerMsg.GetReadWriteBuffer(transLength));
	
            ASSERT_EQ(CTransactionItem::IsMessagePMTDependent(transBuffer), true) << "Transaction is  PMT dependent";
        }