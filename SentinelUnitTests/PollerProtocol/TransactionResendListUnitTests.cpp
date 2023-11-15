#include "stdafx.h"
#include "PollerProtocol/TransactionResendList.h"
#include "PollerProtocol/PollerPlayerTransactionMessage.h"
#include "PollerProtocol/PollerTicketTransactionMessage.h"
#include "PollerProtocolMock.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;
    static time_t DEFAULT_TIME = 0;

	namespace SentinelNativeUnitTests
	{
	class TransactionResendList : public ::testing::Test
	{
	public:
	};

		/// <summary>
		/// Functional test for:
		///     CTransactionItem::CTransactionItem()
		///     CTransactionItem::IsSuspicious()
		/// Test for default value with standard constructor.
		/// </summary>
		TEST(TransactionResendList, Item_IsSuspicious_Default)
		{
			// Perform operation(s) to be tested.
			const CTransactionItem item;

			// Test(s).
			ASSERT_FALSE(item.IsSuspicious()) << L"IsSuspicious";
		}

		/// <summary>
		/// Functional test for:
		///     CTransactionItem::CTransactionItem(byte msgType)
		///     CTransactionItem::IsSuspicious()
		/// Test for default value with msgType constructor.
		/// </summary>
		TEST(TransactionResendList, Item_IsSuspicious_MsgTypeDefault)
		{
			// Perform operation(s) to be tested.
			const CTransactionItem item(0, 0, nullptr, 0);

			// Test(s).
			ASSERT_FALSE(item.IsSuspicious()) << L"IsSuspicious";
		}

		/// <summary>
		/// Functional test for:
		///     CTransactionItem::CTransactionItem()
		///     CTransactionItem::SetSuspicious()
		///     CTransactionItem::IsSuspicious()
		/// Test for default value with standard constructor.
		/// </summary>
		TEST(TransactionResendList, Item_IsSuspicious_Set)
		{
			// Setup.
			CTransactionItem item;

			// Perform operation(s) to be tested.
			item.SetSuspicious();

			// Test(s).
			ASSERT_EQ(true, item.IsSuspicious()) << L"IsSuspicious";
		}

		/// <summary>
		/// Functional test for:
		///     CTransactionItem::AddTransaction()
		///     CTransactionItem::GetFirstItem()
		/// Test getting the first item when the first item has already suspicious set to default.
		/// </summary>
		TEST(TransactionResendList, List_GetFirstItem_AlreadySuspiciousFalse)
		{
			// Data.
			BYTE transaction[555];
			ZeroMemory(transaction, sizeof(transaction));

			// Setup.
			CTransactionResendList list(false, MEMORY_NONE);
			ASSERT_EQ(true, list.AddTransaction(NONE_SPECIFIED, 111, transaction, sizeof(transaction), false, false));


			// Perform operation(s) to be tested.
			WORD transLength;
			DWORD outUniqueId;
			DWORD outLastSent;
			bool alreadySuspicious;
			BYTE *trans(list.GetFirstItem(transLength, outUniqueId, outLastSent, alreadySuspicious));
			
			ASSERT_NE((void *)nullptr, (void *)trans) << L"trans";
			ASSERT_EQ((WORD)sizeof(transaction), transLength) << L"transLength";
			ASSERT_EQ((DWORD)111, outUniqueId) << L"outUniqueId";
			ASSERT_FALSE(alreadySuspicious) << L"alreadySuspicious";
		}

		/// <summary>
		/// Functional test for:
		///     CTransactionItem::AddTransaction()
		///     CTransactionItem::SetSuspicious()
		///     CTransactionItem::GetFirstItem()
		/// Test getting the first item when the first item has already suspicious set to true.
		/// </summary>
		TEST(TransactionResendList, List_GetFirstItem_AlreadySuspiciousTrue)
		{
			// Data.
			BYTE transaction[555];
			ZeroMemory(transaction, sizeof(transaction));

			// Setup.
			CTransactionResendList list(false, MEMORY_NONE);
			ASSERT_EQ(true, list.AddTransaction(NONE_SPECIFIED, 111, transaction, sizeof(transaction), false, false));
			list.SetSuspicious(111);

			// Perform operation(s) to be tested.
			WORD transLength;
			DWORD outUniqueId;
			DWORD outLastSent;
			bool alreadySuspicious;
			BYTE *trans(list.GetFirstItem(transLength, outUniqueId, outLastSent, alreadySuspicious));

			ASSERT_NE((void *)nullptr, (void *)trans) << L"trans";
			ASSERT_EQ((WORD)sizeof(transaction), transLength) << L"transLength";
			ASSERT_EQ((DWORD)111, outUniqueId) << L"outUniqueId";
			ASSERT_EQ(true, alreadySuspicious) << L"alreadySuspicious";
		}

		/// <summary>
		/// Functional test for:
		///     CTransactionItem::AddTransaction()
		///     CTransactionItem::SetSuspicious()
		///     CTransactionItem::GetFirstItem()
		///     CTransactionItem::GetNextItem()
		/// Test getting items with different values for already suspicious.
		/// </summary>
		TEST(TransactionResendList, List_GetNextItem_AlreadySuspicious)
		{
			// Data.
			BYTE transaction[555];
			ZeroMemory(transaction, sizeof(transaction));

			// Setup.
			CTransactionResendList list(false, MEMORY_NONE);
			ASSERT_EQ(true, list.AddTransaction(NONE_SPECIFIED, 1, transaction, sizeof(transaction), false, false));
			ASSERT_EQ(true, list.AddTransaction(NONE_SPECIFIED, 2, transaction, sizeof(transaction), false, false));
			ASSERT_EQ(true, list.AddTransaction(NONE_SPECIFIED, 3, transaction, sizeof(transaction), false, false));
			ASSERT_EQ(true, list.AddTransaction(NONE_SPECIFIED, 4, transaction, sizeof(transaction), false, false));
			list.SetSuspicious(1);
			list.SetSuspicious(3);
			bool gotItem[5] = { false, false, false, false, false };

			// Perform operation(s) to be tested.
			WORD transLength;
			DWORD outUniqueId;
			DWORD outLastSent;
			bool alreadySuspicious;
			BYTE *trans(list.GetFirstItem(transLength, outUniqueId, outLastSent, alreadySuspicious));

			ASSERT_NE((void *)nullptr, (void *)trans) << L"trans";
			ASSERT_EQ((WORD)sizeof(transaction), transLength) << L"transLength";
			ASSERT_TRUE(1 <= outUniqueId && outUniqueId <= 4) << L"outUniqueId";
			gotItem[outUniqueId] = true;
			if (1 == outUniqueId || 3 == outUniqueId)
			{
				ASSERT_EQ(true, alreadySuspicious) << L"alreadySuspicious odd";
			}
			else
			{
				ASSERT_FALSE(alreadySuspicious) << L"alreadySuspicious even";
			}

			for (int num(0); num < 3; ++num)
			{
				BYTE *trans(list.GetNextItem(transLength, outUniqueId, outLastSent, alreadySuspicious));

				ASSERT_NE((void *)nullptr, (void *)trans) << L"trans";
				ASSERT_EQ((WORD)sizeof(transaction), transLength) << L"transLength";
				ASSERT_TRUE(1 <= outUniqueId && outUniqueId <= 4) << L"outUniqueId";
				gotItem[outUniqueId] = true;
				if (1 == outUniqueId || 3 == outUniqueId)
				{
					ASSERT_EQ(true, alreadySuspicious) << L"alreadySuspicious odd";
				}
				else
				{
					ASSERT_FALSE(alreadySuspicious) << L"alreadySuspicious even";
				}
			}

			// Make sure we got each item once.
			ASSERT_EQ(true, gotItem[1]) << L"gotItem[1]";
			ASSERT_EQ(true, gotItem[2]) << L"gotItem[2]";
			ASSERT_EQ(true, gotItem[3]) << L"gotItem[3]";
			ASSERT_EQ(true, gotItem[4]) << L"gotItem[4]";
		}

        /// <summary>
        /// Checks if non-ticket transaction is PMT dependent
    	/// </summary>
        TEST(TransactionResendList, PollerMessage_isNotPMTDependent)
		{
            const int activeSecondsValueOffset(84); // Offset into transBuffer.
            const __int64 activeSecondsExpected(60);
            const time_t lastSessionStartTime(CUtilities::GetTimeFromFields(2015, 10, 20, 12, 23, 34));
            time_t transTime(lastSessionStartTime + activeSecondsExpected); // Need read/write value.

            // Setup CPlayer for physical card out with no virtual session.
            CCard *card(new CCard(lastSessionStartTime, lastSessionStartTime));
            card->SetCardID(_T("1234567890"));
            const CConfig config(false, "");
            const CEGMConfig egmConfig(MEMORY_NONE);

            const time_t utcRemovalTime(transTime - time_t(5));
            const bool makeVirtual(false);
            CMeters *virtualStartMeters(nullptr);

            CGames *games = new CGames();
            CPlayer *player(new CPlayer(card, new CPlayerInfo, config, egmConfig, lastSessionStartTime, false, games));
            player->CardRemoved(utcRemovalTime, utcRemovalTime, makeVirtual);
            ASSERT_EQ(lastSessionStartTime, player->GetLastSessionStartTime()); // Verify setup.

            CMeters *meters(new CMeters(MEMORY_NONE));

            // Perform operation(s) and test(s).
            CPollerPlayerTransactionMessage pollerMsg(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime, 0);
			pollerMsg.MessageHeader.PacketVersion = POLLER_PROTOCOL_VERSION;
            DWORD transLength(0);

            byte *transBuffer(pollerMsg.GetReadWriteBuffer(transLength));
	
            ASSERT_EQ(CTransactionItem::IsMessagePMTDependent(transBuffer), true) << "Player transaction is not PMT dependent";
		}

        /// <summary>
        /// Checks if ticket transaction is PMT independent
    	/// </summary>
        TEST(TransactionResendList, PollerMessage_isPMTDependent)
		{
            CMeters *meters = (new CMeters(MEMORY_NONE));
            CTicket  *ticket = (new CTicket());
            byte transType = TICKET_CODE;
            const byte * cardID = reinterpret_cast<const byte*>("1234567890");
        
            CPollerTicketTransactionMessage pollerMsg(meters, ticket, transType, 0, cardID);
            pollerMsg.MessageHeader.PacketVersion = POLLER_PROTOCOL_VERSION;
            pollerMsg.SetReadWriteBuffer();

            DWORD transLength(0);
            byte *transBuffer(pollerMsg.GetReadWriteBuffer(transLength));
	
            ASSERT_EQ(CTransactionItem::IsMessagePMTDependent(transBuffer), false) <<  "Ticket transaction is PMT dependent";
		}

		/// <summary>
		/// IsEligibleToResend
		/// </summary>
		TEST(TransactionResendList, IsEligibleToResendTest)
		{
			CMeters *meters(new CMeters(MEMORY_NONE));
			byte transType = PERSONAL_BANKER_CODE;
			byte* cardID = new byte[CARD_ID_LEN];
			CPersonalBankerTransfer* transfer(new CPersonalBankerTransfer(0));
			transfer->SetPersonalBankerType(SessionRequest);
			transfer->SetCardNumber(cardID, CARD_ID_LEN);
			CPollerPBTransactionMessage pollerMsg(meters, transfer, transType, time_t(0), 1, cardID, SessionRequest);
			pollerMsg.MessageHeader.PacketVersion = POLLER_PROTOCOL_VERSION;
			ASSERT_FALSE(pollerMsg.IsEligibleToResend());

			delete transfer;
			delete meters;
		}
	};
