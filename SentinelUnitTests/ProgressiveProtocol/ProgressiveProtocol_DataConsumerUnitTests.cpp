#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveProtocol.h"

// Sentinel includes.
#include "InternalMessage.h"
#include "InternalMessageQueueList.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
//#include "UnitTestingToString.h"


#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{
    class ProgressiveProtocol_DataConsumer : public ::testing::Test {
		protected:
            CQueueList scfToInternalsQueue;
            CProgressiveProtocol obj;

            ProgressiveProtocol_DataConsumer():scfToInternalsQueue(), obj(scfToInternalsQueue){}

			virtual void SetUp() {
				CDirectoryManager::MockInstance(NULL);
			}

			virtual void TearDown() 
			{
				CDirectoryManager::MockInstanceCleanup();
			}
     // Remove all messages and assert that they are the correct default message(s).
		void AssertRemoveDefaultMessages(CQueueList &scfToInternalsQueue)
		{
			// Check first message.
			CQueueItem *item1 = scfToInternalsQueue.Remove();
			ASSERT_TRUE(item1 != NULL) << "item1";
			CInternalMessage *msg1 = dynamic_cast<CInternalMessage *>(item1);
			ASSERT_TRUE(msg1 != NULL) << "msg1";
			ASSERT_EQ(INT_MSG_SET_PROGRESSIVE_CONFIG, msg1->GetMessageType()) << "msg1->GetMessageType()";

			// Verify that there is no second message.
			ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "item2";
		}

		void SendMsgToInternalsDefault()
		{
			// Data.
			const InternalMsgType msgType(INT_MSG_PROGRESSIVE_BONUSING_AVAILABLE);
			const DWORD dwordValue(1234567);
			const int msgPriority(QUEUE_PRIORITY_NORMAL);

			// Setup.
			obj.Init();
			AssertRemoveDefaultMessages(scfToInternalsQueue); // Clear out default messages.

			CInternalMessage *msg = new CInternalMessage(msgType, dwordValue);

			// Perform operation(s) and test(s).
			obj.SendMsgToInternals(msg);
			{
				CInternalMessage *msg(static_cast<CInternalMessage *>(scfToInternalsQueue.Remove()));
				ASSERT_TRUE(msg != NULL) << "msg";
				ASSERT_EQ(msgPriority, msg->GetPriority()) << "GetPriority";
				ASSERT_EQ(msgType, msg->GetMessageType()) << "GetMessageType";
				ASSERT_EQ(dwordValue, msg->GetDWordData()) << "GetDWordData";
			}
			ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "Remove final"; // Verify that the queue is now empty.
		}

        void SendMsgToInternalsPriority(){
			// Data.
			const InternalMsgType msgType(INT_MSG_PROGRESSIVE_BONUSING_AVAILABLE);
			const DWORD dwordValue(1234567);
			const int msgPriority(QUEUE_PRIORITY_HIGH);

			// Setup.
			obj.Init();
			AssertRemoveDefaultMessages(scfToInternalsQueue); // Clear out default messages.

			CInternalMessage *msg = new CInternalMessage(msgType, dwordValue);

			// Perform operation(s) and test(s).
			obj.SendMsgToInternals(msg, msgPriority);
			{
				CInternalMessage *msg(static_cast<CInternalMessage *>(scfToInternalsQueue.Remove()));
				ASSERT_TRUE(msg != NULL) << "msg";
				ASSERT_EQ(msgPriority, msg->GetPriority()) << "GetPriority";
				ASSERT_EQ(msgType, msg->GetMessageType()) << "GetMessageType";
				ASSERT_EQ(dwordValue, msg->GetDWordData()) << "GetDWordData";
			}
			ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "Remove final"; // Verify that the queue is now empty.
        }

        void ConnectionUpdate()
		{
			// Data.
			const InternalMsgType msgType(INT_MSG_SET_PROGRESSIVE_CONNECTION_INFO);
			const int msgPriority(QUEUE_PRIORITY_NORMAL);
			const ProgressiveConnectionState connectionState(InDiscovery);

			// Setup.
			obj.Init();
			AssertRemoveDefaultMessages(scfToInternalsQueue); // Clear out default messages.

			CProgressiveConnectionInfo connectionInfo;
			connectionInfo.SetState(connectionState);

			// Perform operation(s) and test(s).
			obj.ConnectionUpdate(connectionInfo);
			{
				CInternalMessage *msg(static_cast<CInternalMessage *>(scfToInternalsQueue.Remove()));
				ASSERT_TRUE(msg != NULL) << "msg";
				ASSERT_EQ(msgPriority, msg->GetPriority()) << "GetPriority";
				ASSERT_EQ(msgType, msg->GetMessageType()) << "GetMessageType";
				CProgressiveConnectionInfo *connectionInfo(msg->GetProgressiveConnectionInfo());
				ASSERT_TRUE(connectionInfo != NULL) << "connectionInfo";
				ASSERT_EQ(connectionState, connectionInfo->GetState()) << "GetState";
			}
			ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "Remove final"; // Verify that the queue is now empty.
		}

        void ProgressiveHitSentStatusUpdateSentFalse()
		{
			// Data.
			const bool wasSent(false);

			// Setup.
			obj.Init();
			AssertRemoveDefaultMessages(scfToInternalsQueue); // Clear out default messages.

			const CEGMAward egmAward;

			// Perform operation(s) and test(s).
			obj.ProgressiveHitSentStatusUpdate(egmAward, wasSent);
			{
				CInternalMessage *msg(static_cast<CInternalMessage *>(scfToInternalsQueue.Remove()));
				ASSERT_TRUE(msg == NULL) << "msg";
			}
			ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "Remove final"; // Verify that the queue is now empty.
		}

        void ProgressiveHitSentStatusUpdateSentTrue()
		{
			// Data.
			const InternalMsgType msgType(INT_MSG_SET_PROGRESSIVE_HIT_SENT);
			const __int64 awardAmountPaid(1234567);
			const int msgPriority(QUEUE_PRIORITY_NORMAL);
			const bool wasSent(true);

			// Setup.
			obj.Init();
			AssertRemoveDefaultMessages(scfToInternalsQueue); // Clear out default messages.

			CEGMAward egmAward;
			egmAward.SetAwardAmountPaid(awardAmountPaid);

			// Perform operation(s) and test(s).
			obj.ProgressiveHitSentStatusUpdate(egmAward, wasSent);
			{
				CInternalMessage *msg(static_cast<CInternalMessage *>(scfToInternalsQueue.Remove()));
				ASSERT_TRUE(msg != NULL) << "msg";
				ASSERT_EQ(msgPriority, msg->GetPriority()) << "GetPriority";
				ASSERT_EQ(msgType, msg->GetMessageType()) << "GetMessageType";
				CEGMAward *egmAward(msg->GetEGMAward());
				ASSERT_TRUE(egmAward != NULL) << "GetEGMAward";
				ASSERT_EQ(awardAmountPaid, egmAward->GetAwardAmountPaid()) << "GetAwardAmountPaid";
			}
			ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "Remove final"; // Verify that the queue is now empty.
		}

        void SendJackpotPaidRequestStatusUpdateSentFalse()
		{
			// Data.
			const bool wasSent(false);

			// Setup.
			obj.Init();
			AssertRemoveDefaultMessages(scfToInternalsQueue); // Clear out default messages.

			const CEGMAward egmAward;

			// Perform operation(s) and test(s).
			obj.SendJackpotPaidRequestStatusUpdate(egmAward, wasSent);
			{
				CInternalMessage *msg(static_cast<CInternalMessage *>(scfToInternalsQueue.Remove()));
				ASSERT_TRUE(msg == NULL) << "msg";
			}
			ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "Remove final"; // Verify that the queue is now empty.
		}

        void SendJackpotPaidRequestStatusUpdateSentTrue()
		{
			// Data.
			const InternalMsgType msgType(INT_MSG_SET_PROGRESSIVE_PAID_SENT);
			const __int64 awardAmountPaid(1234567);
			const int msgPriority(QUEUE_PRIORITY_NORMAL);
			const bool wasSent(true);

			// Setup.
			obj.Init();
			AssertRemoveDefaultMessages(scfToInternalsQueue); // Clear out default messages.

			CEGMAward egmAward;
			egmAward.SetAwardAmountPaid(awardAmountPaid);

			// Perform operation(s) and test(s).
			obj.SendJackpotPaidRequestStatusUpdate(egmAward, wasSent);
			{
				CInternalMessage *msg(static_cast<CInternalMessage *>(scfToInternalsQueue.Remove()));
				ASSERT_TRUE(msg != NULL) << "msg";
				ASSERT_EQ(msgPriority, msg->GetPriority()) << "GetPriority";
				ASSERT_EQ(msgType, msg->GetMessageType()) << "GetMessageType";
				CEGMAward *egmAward(msg->GetEGMAward());
				ASSERT_TRUE(egmAward != NULL) << "GetEGMAward";
				ASSERT_EQ(awardAmountPaid, egmAward->GetAwardAmountPaid()) << "GetAwardAmountPaid";
			}
			ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "Remove final"; // Verify that the queue is now empty.
		}

        void HostUpdate_Nullptr()
        {
            // Data.
            const InternalMsgType msgType(INT_MSG_SET_PROGRESSIVE_HOST);
            const int msgPriority(QUEUE_PRIORITY_NORMAL);
            const std::string hostName("");
            const DWORD hostPort(0);

            // Setup.
            obj.Init();
            AssertRemoveDefaultMessages(scfToInternalsQueue); // Clear out default messages.

            const CProgressiveHost *host(nullptr);

            // Perform operation(s) and test(s).
            obj.HostUpdate(host);
            {
                CInternalMessage *msg(static_cast<CInternalMessage *>(scfToInternalsQueue.Remove()));
                ASSERT_TRUE(msg != NULL) << "msg";
                ASSERT_EQ(msgPriority, msg->GetPriority()) << "GetPriority";
                ASSERT_EQ(msgType, msg->GetMessageType()) << "GetMessageType";
                CProgressiveHost *host(msg->GetProgressiveHost());
                ASSERT_TRUE(host != NULL) << "GetProgressiveHost";
                ASSERT_EQ(hostName, host->GetName()) << "GetName";
                ASSERT_EQ(hostPort, host->GetHostPort()) << "GetHostPort";
            }
            ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "Remove final"; // Verify that the queue is now empty.
        }

        void HostUpdate()
        {
            // Data.
            const InternalMsgType msgType(INT_MSG_SET_PROGRESSIVE_HOST);
            const int msgPriority(QUEUE_PRIORITY_NORMAL);
            const std::string hostName("");
            const string hostNameInit(hostName.c_str());
            const DWORD hostPort(0);

            // Setup.
             obj.Init();
            AssertRemoveDefaultMessages(scfToInternalsQueue); // Clear out default messages.

            const CProgressiveHost host(hostNameInit.c_str(), hostPort);

            // Perform operation(s) and test(s).
            obj.HostUpdate(&host);
            {
                CInternalMessage *msg(static_cast<CInternalMessage *>(scfToInternalsQueue.Remove()));
                ASSERT_TRUE(msg != NULL) << "msg";
                ASSERT_EQ(msgPriority, msg->GetPriority()) << "GetPriority";
                ASSERT_EQ(msgType, msg->GetMessageType()) << "GetMessageType";
                CProgressiveHost *host(msg->GetProgressiveHost());
                ASSERT_TRUE(host != NULL) << "GetProgressiveHost";
                ASSERT_EQ(hostName, host->GetName()) << "GetName";
                ASSERT_EQ(hostPort, host->GetHostPort()) << "GetHostPort";
            }
            ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "Remove final"; // Verify that the queue is now empty.
        }

        void GetProgressiveGameInfo()
        {
            // Data.
            const InternalMsgType msgType(INT_MSG_GET_PROGRESSIVE_GAMES_INFO);
            const int msgPriority(QUEUE_PRIORITY_NORMAL);

            // Setup.
            obj.Init();
            AssertRemoveDefaultMessages(scfToInternalsQueue); // Clear out default messages.

            // Perform operation(s) and test(s).
            obj.GetProgressiveGameInfo();
            {
                CInternalMessage *msg(static_cast<CInternalMessage *>(scfToInternalsQueue.Remove()));
                ASSERT_TRUE(msg != NULL) << "msg";
                ASSERT_EQ(msgPriority, msg->GetPriority()) << "GetPriority";
                ASSERT_EQ(msgType, msg->GetMessageType()) << "GetMessageType";
            }
            ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "Remove final"; // Verify that the queue is now empty.
        }

        void JackpotHit_StandardProgressive()
        {
            // Data.
            const bool isStandardProgressive(true);
            const InternalMsgType msgType(INT_MSG_EGM_AWARD);
            const int msgPriority(QUEUE_PRIORITY_NORMAL);
            const __int64 awardAmountPaid(1234567);

            // Setup.
            obj.Init();
            AssertRemoveDefaultMessages(scfToInternalsQueue); // Clear out default messages.

            CEGMAward *egmAward(new CEGMAward());
            egmAward->SetAwardAmountPaid(awardAmountPaid);
            egmAward->SetStandardProgressive(isStandardProgressive);

            // Perform operation(s) and test(s).
            obj.JackpotHit(egmAward);
            {
				bool starvationIsCleared(false);
				ASSERT_FALSE(scfToInternalsQueue.IsQueueStarved(PERSIST_JACKPOTHIT, starvationIsCleared)) << "IsQueueStarved";
                ASSERT_FALSE(scfToInternalsQueue.IsQueueStarvedEx(PERSIST_JACKPOTHIT)) << "IsQueueStarvedEx";
                ASSERT_FALSE(scfToInternalsQueue.IsPersistTypePresent(PERSIST_JACKPOTHIT)) << "IsPersistTypePresent";
                ASSERT_FALSE(scfToInternalsQueue.IsPersistTypePresentEx(PERSIST_JACKPOTHIT)) << "IsPersistTypePresent";

                CInternalMessage *msg(static_cast<CInternalMessage *>(scfToInternalsQueue.Remove()));
                ASSERT_TRUE(msg != NULL) << "msg";
                ASSERT_EQ(msgPriority, msg->GetPriority()) << "GetPriority";
                ASSERT_EQ(msgType, msg->GetMessageType()) << "GetMessageType";
                CEGMAward *egmAward(msg->GetEGMAward());
                ASSERT_TRUE(egmAward != NULL) << "GetEGMAward";
                ASSERT_EQ(awardAmountPaid, egmAward->GetAwardAmountPaid()) << "GetAwardAmountPaid";
                ASSERT_EQ(isStandardProgressive, egmAward->IsStandardProgressive()) << "IsStandardProgressive";

                // Test CMessagePersist member.
                CMessagePersist *mp = (CMessagePersist *)(msg->GetContent(CMessagePersist::MsgContentType()));
                ASSERT_TRUE(mp == NULL) << "GetContent(CMessagePersist::MsgContentType()";
            }
            ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "Remove final"; // Verify that the queue is now empty.
        }

        void JackpotHit_NotStandardProgressive()
        {
            // Data.
            const bool isStandardProgressive(false);
            const InternalMsgType msgType(INT_MSG_EGM_AWARD);
            const int msgPriority(QUEUE_PRIORITY_NORMAL);
            const __int64 awardAmountPaid(1234567);

            // Setup.
            obj.Init();
            AssertRemoveDefaultMessages(scfToInternalsQueue); // Clear out default messages.

            CEGMAward *egmAward(new CEGMAward());
            egmAward->SetAwardAmountPaid(awardAmountPaid);
            egmAward->SetStandardProgressive(isStandardProgressive);

            // Perform operation(s) and test(s).
            obj.JackpotHit(egmAward);
            {
                bool starvationIsCleared(false);
				ASSERT_FALSE(scfToInternalsQueue.IsQueueStarved(PERSIST_JACKPOTHIT, starvationIsCleared)) << "IsQueueStarved";
                ASSERT_FALSE(scfToInternalsQueue.IsQueueStarvedEx(PERSIST_JACKPOTHIT)) << "IsQueueStarvedEx";
                ASSERT_FALSE(scfToInternalsQueue.IsPersistTypePresent(PERSIST_JACKPOTHIT)) << "IsPersistTypePresent";
                ASSERT_TRUE(scfToInternalsQueue.IsPersistTypePresentEx(PERSIST_JACKPOTHIT)) << "IsPersistTypePresent";

                CInternalMessage *msg(static_cast<CInternalMessage *>(scfToInternalsQueue.Remove()));
                ASSERT_TRUE(msg != NULL) << "msg";
                ASSERT_EQ(msgPriority, msg->GetPriority()) << "GetPriority";
                ASSERT_EQ(msgType, msg->GetMessageType()) << "GetMessageType";
                CEGMAward *egmAward(msg->GetEGMAward());
                ASSERT_TRUE(egmAward != NULL) << "GetEGMAward";
                ASSERT_EQ(awardAmountPaid, egmAward->GetAwardAmountPaid()) << "GetAwardAmountPaid";
                ASSERT_EQ(isStandardProgressive, egmAward->IsStandardProgressive()) << "IsStandardProgressive";

                // Test CMessagePersist member.
                CMessagePersist *mp = (CMessagePersist *)(msg->GetContent(CMessagePersist::MsgContentType()));
                ASSERT_TRUE(mp != NULL) << "GetContent(CMessagePersist::MsgContentType()";
                ASSERT_EQ(PERSIST_JACKPOTHIT, mp->GetPersistType()) << "GetPersistType";
                ASSERT_EQ(QUEUE_PRIORITY_NORMAL, mp->GetPriority()) << "GetPersistType";
                ASSERT_EQ(true, mp->isNewStarvation()) << "isNewStarvation";
                int flushType(123);
                ASSERT_FALSE(mp->GetFlushType(flushType)) << "GetFlushType return";
                ASSERT_EQ((int)0, flushType) << "GetFlushType value";
                string flushId("xx");
                ASSERT_FALSE(mp->GetFlushID(flushId)) << "GetFlushID return";
                ASSERT_EQ(string(""), flushId) << "GetFlushID value";
                ASSERT_FALSE(mp->IsQueueStarved()) << "mp->IsQueueStarved default";
                ASSERT_FALSE(mp->IsFlusher()) << "IsFlusher";
            }
            ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "Remove final"; // Verify that the queue is now empty.
        }

        /// <summary>
        /// Functional test for:
        /// CProgressiveProtocol::JackpotHit()
        /// Test sending an outgoing message using the data consumer method and a CInternalMessageQueueList.
        /// </summary>
        void JackpotHit_CInternalMessageQueueList()
        {
            // Data.
            const bool isStandardProgressive(false);
            const InternalMsgType msgType(INT_MSG_EGM_AWARD);
            const int msgPriority(QUEUE_PRIORITY_NORMAL);
            const __int64 awardAmountPaid(1234567);

            // Setup.
            CInternalMessageQueueList scfToInternalsQueue("");
            CProgressiveProtocol obj(scfToInternalsQueue, "1.1.2");
            obj.Init();
            AssertRemoveDefaultMessages(scfToInternalsQueue); // Clear out default messages.

            CEGMAward *egmAward(new CEGMAward());
            egmAward->SetAwardAmountPaid(awardAmountPaid);
            egmAward->SetStandardProgressive(isStandardProgressive);

            // Perform operation(s) and test(s).
            obj.JackpotHit(egmAward);
            {
                bool starvationIsCleared(false);
                ASSERT_FALSE(scfToInternalsQueue.IsQueueStarved(PERSIST_JACKPOTHIT, starvationIsCleared)) << "IsQueueStarved";
                ASSERT_FALSE(scfToInternalsQueue.IsQueueStarvedEx(PERSIST_JACKPOTHIT)) << "IsQueueStarvedEx";
                ASSERT_TRUE(scfToInternalsQueue.IsPersistTypePresent(PERSIST_JACKPOTHIT)) << "IsPersistTypePresent";
                ASSERT_TRUE(scfToInternalsQueue.IsPersistTypePresentEx(PERSIST_JACKPOTHIT)) << "IsPersistTypePresent";

                CInternalMessage *msg(static_cast<CInternalMessage *>(scfToInternalsQueue.Remove()));
                ASSERT_TRUE(msg != NULL) << "msg";
                ASSERT_EQ(msgPriority, msg->GetPriority()) << "GetPriority";
                ASSERT_EQ(msgType, msg->GetMessageType()) << "GetMessageType";
                CEGMAward *egmAward(msg->GetEGMAward());
                ASSERT_TRUE(egmAward != NULL) << "GetEGMAward";
                ASSERT_EQ(awardAmountPaid, egmAward->GetAwardAmountPaid()) << "GetAwardAmountPaid";
                ASSERT_EQ(isStandardProgressive, egmAward->IsStandardProgressive()) << "IsStandardProgressive";

                // Test CMessagePersist member.
                CMessagePersist *mp = (CMessagePersist *)(msg->GetContent(CMessagePersist::MsgContentType()));
                ASSERT_TRUE(mp == NULL) << "GetContent(CMessagePersist::MsgContentType()";
            }
            ASSERT_TRUE(scfToInternalsQueue.Remove() == NULL) << "Remove final"; // Verify that the queue is now empty.
        }

    };

    // #pragma region SendMsgToInternals
    /// <summary>
    /// Functional test for:
    ///     CProgressiveProtocol::SendMsgToInternals()
    /// Test sending an outgoing message using the data consumer method with default priority.
    /// </summary>
    TEST_F(ProgressiveProtocol_DataConsumer, SendMsgToInternals_Default)
    {
        SendMsgToInternalsDefault();
    }

    /// <summary>
    /// Functional test for:
    ///     CProgressiveProtocol::SendMsgToInternals()
    /// Test sending an outgoing message using the data consumer method with non-default priority.
    /// </summary>
    TEST_F(ProgressiveProtocol_DataConsumer, SendMsgToInternals_Priority)
    {
        SendMsgToInternalsPriority();
    }
// #pragma endregion SendMsgToInternals

 
// #pragma region ConnectionUpdate
		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::ConnectionUpdate()
		/// Test sending an outgoing message using the data consumer method.
		/// </summary>
		TEST_F(ProgressiveProtocol_DataConsumer, ConnectionUpdate)
        {
            ConnectionUpdate();
        }
		
// #pragma endregion ConnectionUpdate


// #pragma region ProgressiveHitSentStatusUpdate
		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::ProgressiveHitSentStatusUpdate()
		/// Test sending an outgoing message using the data consumer method.
		/// </summary>
		TEST_F(ProgressiveProtocol_DataConsumer, ProgressiveHitSentStatusUpdate_SentFalse)
        {
            ProgressiveHitSentStatusUpdateSentFalse();
        }

    	/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::ProgressiveHitSentStatusUpdate()
		/// Test sending an outgoing message using the data consumer method.
		/// </summary>
		TEST_F(ProgressiveProtocol_DataConsumer, ProgressiveHitSentStatusUpdate_SentTrue)
		{
            ProgressiveHitSentStatusUpdateSentTrue();
		}
// #pragma endregion ProgressiveHitSentStatusUpdate
	
// #pragma region SendJackpotPaidRequestStatusUpdate
		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::SendJackpotPaidRequestStatusUpdate()
		/// Test sending an outgoing message using the data consumer method.
		/// </summary>
		TEST_F(ProgressiveProtocol_DataConsumer, SendJackpotPaidRequestStatusUpdate_SentFalse)
		{
            SendJackpotPaidRequestStatusUpdateSentFalse();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::SendJackpotPaidRequestStatusUpdate()
		/// Test sending an outgoing message using the data consumer method.
		/// </summary>
		TEST_F(ProgressiveProtocol_DataConsumer, SendJackpotPaidRequestStatusUpdate_SentTrue)
		{
            SendJackpotPaidRequestStatusUpdateSentTrue();
		}

// #pragma endregion ProgressiveHitSentStatusUpdate
// 
// #pragma region HostUpdate
		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::HostUpdate()
		/// Test sending an outgoing message using the data consumer method.
		/// </summary>
		TEST_F(ProgressiveProtocol_DataConsumer, HostUpdate_Nullptr)
		{
            HostUpdate_Nullptr();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::HostUpdate()
		/// Test sending an outgoing message using the data consumer method.
		/// </summary>
		TEST_F(ProgressiveProtocol_DataConsumer, HostUpdate)
		{
              HostUpdate();
		}
// #pragma endregion HostUpdate

// #pragma region GetProgressiveGameInfo
		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::GetProgressiveGameInfo()
		/// Test sending an outgoing message using the data consumer method.
		/// </summary>
		TEST_F(ProgressiveProtocol_DataConsumer, GetProgressiveGameInfo)
		{
            GetProgressiveGameInfo();
		}
// #pragma endregion GetProgressiveGameInfo

// #pragma region JackpotHit
		/// <summary>
		/// Functional test for:
		///     CProgressiveProtocol::JackpotHit()
		/// Test sending an outgoing message using the data consumer method.
		/// </summary>
		TEST_F(ProgressiveProtocol_DataConsumer, JackpotHit_StandardProgressive)
        {
            JackpotHit_StandardProgressive();
        }

		/// <summary>
		/// Functional test for:
		/// CProgressiveProtocol::JackpotHit()
		/// Test sending an outgoing message using the data consumer method.
		/// </summary>
		TEST_F(ProgressiveProtocol_DataConsumer, JackpotHit_NotStandardProgressive)
        {
            JackpotHit_NotStandardProgressive();
        }

		/// <summary>
		/// Functional test for:
		/// CProgressiveProtocol::JackpotHit()
		/// Test sending an outgoing message using the data consumer method and a CInternalMessageQueueList.
		/// </summary>
		TEST_F(ProgressiveProtocol_DataConsumer, JackpotHit_CInternalMessageQueueList)
        {
            JackpotHit_CInternalMessageQueueList();
        }

 // #pragma endregion JackpotHit
}
