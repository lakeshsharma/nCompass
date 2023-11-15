#include "stdafx.h"
#include "QueueItem.h"
#include "QueueList.h"
#include "InternalMessageQueueList.h"
#include "InternalMessage.h"
#include "MessagePrioritizer.h"
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	TEST(InternalMessageQueueListUnitTests, CInternalMessageQueueListSessionChangeFlushAllFlushable)
	{
		// Setup.
		CInternalMessageQueueList queueList(false);
		CMessagePrioritizer p;

		CInternalMessage *qArray[4];
		for (WORD qIndex = 0; qIndex < 4; qIndex++)
		{
			CInternalMessage *msg = new CInternalMessage(INT_MSG_SPLASHDOWN_WINNER_END, qIndex);
			msg->SetMessagePersist(p.PrioritizeMessage(msg));
			qArray[qIndex] = msg;
		}

		// Perform operation(s) to be tested.
		ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST));

		// Flush all messages
		queueList.FlushPersisted();

		// Tests.
		CInternalMessage *qi = queueList.Remove();
		ASSERT_TRUE(qi == NULL);
	}

	TEST(InternalMessageQueueListUnitTests, CInternalMessageQueueListFlushAllButTypeSomeFlushable)
	{
		// Setup.
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList queueList(false);
		CMessagePrioritizer p;

		CInternalMessage *qArray[4];

		// Add a message without persistence
		qArray[0] = new CInternalMessage(INT_MSG_CASINO_CHALLENGE_INCREMENT, (WORD)4);
		qArray[0]->SetMessagePersist(p.PrioritizeMessage(qArray[0]));
		qArray[1] = new CInternalMessage(INT_MSG_PB_COMPLETE, (WORD)0);
		qArray[1]->SetMessagePersist(p.PrioritizeMessage(qArray[1]));
		qArray[2] = new CInternalMessage(INT_MSG_PB_NCEPOUT_FAIL, (WORD)1);
		qArray[2]->SetMessagePersist(p.PrioritizeMessage(qArray[2]));
		qArray[3] = new CInternalMessage(INT_MSG_SHOW_ANNIVERSARY, (WORD)4);
		qArray[3]->SetMessagePersist(p.PrioritizeMessage(qArray[3]));

		// Perform operation(s) to be tested.
		ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST));

		// Flush items 0 & 3 messages
		queueList.FlushAllPersistedButType(PBFlushType);

		// Tests.
		WORD expectedItemNumber = 0;
		CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
		while (qi != NULL)
		{
			ASSERT_TRUE(qi->GetWordData() == expectedItemNumber++);
			delete qi;
			queueList.ClearQueueStarvation(PERSIST_CONTENT_WINDOW);
			qi = queueList.Remove();
		}

		ASSERT_TRUE(expectedItemNumber == 2);
	}

	TEST(InternalMessageQueueListUnitTests, CInternalMessageQueueListFlushAllButTypeAllFlushableSameSubQueue)
	{
		// Setup.
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList queueList(false);
		CMessagePrioritizer p;

		CInternalMessage *qArray[4];
		for (WORD qIndex = 0; qIndex < 4; qIndex++)
		{
			CInternalMessage *msg = new CInternalMessage(INT_MSG_SPLASHDOWN_WINNER_END, qIndex);
			msg->SetMessagePersist(p.PrioritizeMessage(msg));
			qArray[qIndex] = msg;
		}

		// Perform operation(s) to be tested.
		ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_NORMAL));

		// Flush all messages
		queueList.FlushAllPersistedButType(PBFlushType);

		// Tests.
		CInternalMessage *qi = queueList.Remove();
		ASSERT_TRUE(qi == NULL);
	}

	TEST(InternalMessageQueueListUnitTests, CInternalMessageQueueListFlushAllButTypeSomeFlushableSameSubQueue)
	{
		// Setup.
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList queueList(false);
		CMessagePrioritizer p;

		CInternalMessage *qArray[4];
		for (WORD qIndex = 0; qIndex < 3; qIndex++)
		{
			CInternalMessage *msg = new CInternalMessage(INT_MSG_SPLASHDOWN_WINNER_END, qIndex);
			msg->SetMessagePersist(p.PrioritizeMessage(msg));
			qArray[qIndex] = msg;
		}

		// Add a message without persistence
		qArray[0] = new CInternalMessage(INT_MSG_CASINO_CHALLENGE_INCREMENT, (WORD)4);
		qArray[0]->SetMessagePersist(p.PrioritizeMessage(qArray[0]));
		qArray[1] = new CInternalMessage(INT_MSG_PB_COMPLETE, (WORD)0);
		qArray[1]->SetMessagePersist(p.PrioritizeMessage(qArray[1]));
		qArray[2] = new CInternalMessage(INT_MSG_PB_NCEPOUT_FAIL, (WORD)1);
		qArray[2]->SetMessagePersist(p.PrioritizeMessage(qArray[2]));
		qArray[3] = new CInternalMessage(INT_MSG_SHOW_ANNIVERSARY, (WORD)4);
		qArray[3]->SetMessagePersist(p.PrioritizeMessage(qArray[3]));

		// Perform operation(s) to be tested.
		ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_NORMAL));

		// Flush items 0 & 3 messages
		queueList.FlushAllPersistedButType(PBFlushType);

		// Tests.
		WORD expectedItemNumber = 0;
		CInternalMessage *qi = queueList.Remove();
		while (qi != NULL)
		{
			ASSERT_TRUE(qi->GetWordData() == expectedItemNumber++);
			delete qi;
			queueList.ClearQueueStarvation(PERSIST_CONTENT_WINDOW);
			qi = queueList.Remove();
		}

		ASSERT_TRUE(expectedItemNumber == 2);
	}

	TEST(InternalMessageQueueListUnitTests, CInternalMessageQueueListFlushByType)
	{
		// Setup.
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList queueList(false);
		CMessagePrioritizer p;

		CInternalMessage *qArray[4];

		// Add a message without persistence
		qArray[0] = new CInternalMessage(INT_MSG_RICOCHET_AWARD, (WORD)1);
		qArray[0]->SetMessagePersist(p.PrioritizeMessage(qArray[0]));
		qArray[1] = new CInternalMessage(INT_MSG_PB_COMPLETE, (WORD)2);
		qArray[1]->SetMessagePersist(p.PrioritizeMessage(qArray[1]));
		qArray[2] = new CInternalMessage(INT_MSG_JACKPOT_ANNOUNCEMENT, (WORD)4);
		qArray[2]->SetMessagePersist(p.PrioritizeMessage(qArray[2]));
		qArray[3] = new CInternalMessage(INT_MSG_SHOW_ANNIVERSARY, (WORD)8);
		qArray[3]->SetMessagePersist(p.PrioritizeMessage(qArray[3]));

		// Perform operation(s) to be tested.
		ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_ABOVE_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_BELOW_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_HIGHEST));

		// Flush items 0 & 3 messages
		queueList.FlushPersistedByType(PromotionFlushType);

		queueList.ClearQueueStarvation(PERSIST_CONTENT_WINDOW);

		// Tests.
		WORD wordAccum = 0;
		CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
		while (qi != NULL)
		{
			wordAccum += qi->GetWordData();
			delete qi;
			queueList.ClearQueueStarvation(PERSIST_CONTENT_WINDOW);
			qi = queueList.Remove();
		}

		ASSERT_TRUE(wordAccum == (2 + 8));
	}

	TEST(InternalMessageQueueListUnitTests, CInternalMessageQueueListFlushByTypeAndID)
	{
		// Setup.
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList queueList(false);
		CMessagePrioritizer p;
		CString flushID(_T("200"));
		CInternalMessage *qArray[4];

		// Add a message without persistence
		int priority;
		qArray[0] = new CInternalMessage(INT_MSG_RICOCHET_AWARD, (WORD)1);
		qArray[0]->SetMessagePersist(p.PrioritizeMessage(qArray[0], priority, 100));
		qArray[1] = new CInternalMessage(INT_MSG_PB_COMPLETE, (WORD)2);
		qArray[1]->SetMessagePersist(p.PrioritizeMessage(qArray[1]));
		qArray[2] = new CInternalMessage(INT_MSG_JACKPOT_ANNOUNCEMENT, (WORD)4);
		qArray[2]->SetMessagePersist(p.PrioritizeMessage(qArray[2], priority, flushID));
		qArray[3] = new CInternalMessage(INT_MSG_SHOW_ANNIVERSARY, (WORD)8);
		qArray[3]->SetMessagePersist(p.PrioritizeMessage(qArray[3]));

		// Perform operation(s) to be tested.
		ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_ABOVE_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_BELOW_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_HIGHEST));

		// Flush items 0 & 3 messages
		queueList.FlushPersistedByTypeAndId(PromotionFlushType, flushID);

		queueList.ClearQueueStarvation(PERSIST_CONTENT_WINDOW);

		// Tests.
		WORD wordAccum = 0;
		CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
		while (qi != NULL)
		{
			wordAccum += qi->GetWordData();
			delete qi;
			queueList.ClearQueueStarvation(PERSIST_CONTENT_WINDOW);
			qi = queueList.Remove();
		}

		ASSERT_TRUE(wordAccum == (1 + 2 + 8));
	}

	TEST(InternalMessageQueueListUnitTests, CInternalMessageQueueListFlushWinnerPick)
	{
		// Setup.
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList queueList(false);
		CMessagePrioritizer p;

		CInternalMessage *qArray[4];

		// Add a message without persistence
		int priority;
		qArray[0] = new CInternalMessage(INT_MSG_RICOCHET_AWARD, (WORD)1);
		qArray[0]->SetMessagePersist(p.PrioritizeMessage(qArray[0], priority, 100));
		qArray[1] = new CInternalMessage(INT_MSG_PROGRESSIVE_WINNER_PICK, (WORD)2);
		qArray[1]->SetMessagePersist(p.PrioritizeMessage(qArray[1], priority, 100));
		qArray[2] = new CInternalMessage(INT_MSG_JACKPOT_ANNOUNCEMENT, (WORD)4);
		qArray[2]->SetMessagePersist(p.PrioritizeMessage(qArray[2], priority, 100));

		// Adding the winner picked message should flush winner pick message
		qArray[3] = new CInternalMessage(INT_MSG_PROGRESSIVE_WINNER_PICKED, (WORD)8);
		qArray[3]->SetMessagePersist(p.PrioritizeMessage(qArray[3], priority, 100));

		// Perform operation(s) to be tested.
		ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_ABOVE_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_BELOW_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_HIGHEST));

		queueList.ClearQueueStarvation(PERSIST_CONTENT_WINDOW);
		// Tests.
		WORD wordAccum = 0;
		CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
		while (qi != NULL)
		{
			wordAccum += qi->GetWordData();
			delete qi;
			queueList.ClearQueueStarvation(PERSIST_CONTENT_WINDOW);
			qi = queueList.Remove();
		}

		ASSERT_TRUE(wordAccum == (1 + 4 + 8));
	}

	TEST(InternalMessageQueueListUnitTests, CInternalMessageQueueListFlushPlayAndWin)
	{
		// Setup.
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList queueList(false);
		CMessagePrioritizer p;

		CInternalMessage *qArray[4];

		// Add a message without persistence
		int priority;
		qArray[0] = new CInternalMessage(INT_MSG_PROGRESSIVE_WINNER_PICK, (WORD)1);
		qArray[0]->SetMessagePersist(p.PrioritizeMessage(qArray[0], priority, 100));
		qArray[1] = new CInternalMessage(INT_MSG_RICOCHET_AWARD, (WORD)2);
		qArray[1]->SetMessagePersist(p.PrioritizeMessage(qArray[1], priority, 100));
		qArray[2] = new CInternalMessage(INT_MSG_RICOCHET_AWARD, (WORD)4);
		qArray[2]->SetMessagePersist(p.PrioritizeMessage(qArray[2], priority, 200));

		// Adding the PnWEnd message should flush the ONE award message of the same flush ID
		CString screenType(PNW_END_SCREEN);
		qArray[3] = new CInternalMessage(INT_MSG_JACKPOT_ANNOUNCEMENT, (WORD)8);
		qArray[3]->SetMessagePersist(p.PrioritizeAnnouncementMessage(qArray[3], screenType, priority, 100));

		// Perform operation(s) to be tested.
		ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_ABOVE_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_BELOW_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_HIGHEST));

		queueList.ClearQueueStarvation(PERSIST_CONTENT_WINDOW);
		// Tests.
		WORD wordAccum = 0;
		CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
		while (qi != NULL)
		{
			wordAccum += qi->GetWordData();
			delete qi;
			queueList.ClearQueueStarvation(PERSIST_CONTENT_WINDOW);
			qi = queueList.Remove();
		}

		ASSERT_TRUE(wordAccum == (1 + 4 + 8));
	}

	TEST(InternalMessageQueueListUnitTests, InternalMessageQueueListStarvationEqualPriority)
	{
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList *queue = new CInternalMessageQueueList(false);

		CMessagePrioritizer mp;
		CInternalMessage *msg = new CInternalMessage(INT_MSG_DISPLAY_SYSTEM_BUSY);
		int priority = 0;
		CMessagePersist *persist = mp.PrioritizeMessage(msg);
		priority = persist->GetPriority();

		msg->SetMessagePersist(persist); // Will starve indefinitely
		queue->Add(msg, priority);

		msg = queue->Remove();
		ASSERT_TRUE(msg != NULL);
		ASSERT_TRUE(msg->GetMessageType() == INT_MSG_DISPLAY_SYSTEM_BUSY);
		delete msg;
		msg = new CInternalMessage(INT_MSG_DISPLAY_ATTRACTOR);
		msg->SetMessagePersist(mp.PrioritizeMessage(msg));
		queue->Add(msg, priority);

		msg = queue->Remove();
		ASSERT_TRUE(msg == NULL);

		queue->ClearQueueStarvation(PERSIST_CONTENT_WINDOW);
		msg = queue->Remove();
		ASSERT_TRUE(msg != NULL);
		ASSERT_TRUE(msg->GetMessageType() == INT_MSG_DISPLAY_ATTRACTOR);

		delete msg;
		delete queue;

	}

	TEST(InternalMessageQueueListUnitTests, InternalMessageQueueListStarvationHigherInterrupt)
	{
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList *queue = new CInternalMessageQueueList(false);

		CMessagePrioritizer mp;
		CInternalMessage *msg = new CInternalMessage(INT_MSG_CASINO_CHALLENGE_INCREMENT);
		int priority = 0;
		CMessagePersist *persist = mp.PrioritizeMessage(msg);
		priority = persist->GetPriority();

		msg->SetMessagePersist(persist); // Will starve indefinitely
		queue->Add(msg, priority);

		msg = queue->Remove();
		ASSERT_TRUE(msg != NULL);
		ASSERT_TRUE(msg->GetMessageType() == INT_MSG_CASINO_CHALLENGE_INCREMENT);
		delete msg;
		msg = new CInternalMessage(INT_MSG_CASINO_CHALLENGE_END);
		msg->SetMessagePersist(mp.PrioritizeMessage(msg));
		queue->Add(msg, priority - 1);

		msg = queue->Remove();
		ASSERT_TRUE(msg != NULL);
		ASSERT_TRUE(msg->GetMessageType() == INT_MSG_CASINO_CHALLENGE_END);

		delete msg;
		delete queue;

	}

	TEST(InternalMessageQueueListUnitTests, InternalMessageQueueListNoPersistWhileStarved)
	{
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList *queue = new CInternalMessageQueueList(false);

		CMessagePrioritizer mp;
		CInternalMessage *msg = new CInternalMessage(INT_MSG_DISPLAY_SYSTEM_BUSY);
		int priority = 0;
		CMessagePersist *persist = mp.PrioritizeMessage(msg);
		priority = persist->GetPriority();

		msg->SetMessagePersist(persist); // Will starve indefinitely
		queue->Add(msg, priority);

		msg = queue->Remove();
		ASSERT_TRUE(msg != NULL);
		ASSERT_TRUE(msg->GetMessageType() == INT_MSG_DISPLAY_SYSTEM_BUSY);
		delete msg;
		msg = new CInternalMessage(INT_MSG_DISPLAY_ATTRACTOR);
		queue->Add(msg, priority + 1);

		msg = queue->Remove();
		ASSERT_TRUE(msg != NULL);
		ASSERT_TRUE(msg->GetMessageType() == INT_MSG_DISPLAY_ATTRACTOR);

		delete msg;
		delete queue;

	}

	TEST(InternalMessageQueueListUnitTests, InternalMessageQueueListTTLExpire)
	{
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList *queue = new CInternalMessageQueueList(false);

		CMessagePrioritizer mp;
		CInternalMessage *msg = new CInternalMessage(INT_MSG_DISPLAY_SYSTEM_BUSY);
		int priority = 0;
		CMessagePersist *persist = mp.PrioritizeMessage(msg);
		priority = persist->GetPriority();

		msg->SetMessagePersist(persist); // Will starve indefinitely
		queue->Add(msg, priority);

		msg = queue->Remove();
		ASSERT_TRUE(msg != NULL);
		ASSERT_TRUE(msg->GetMessageType() == INT_MSG_DISPLAY_SYSTEM_BUSY);
		delete msg;
		msg = new CInternalMessage(INT_MSG_DISPLAY_ATTRACTOR);
		msg->SetMessagePersist(mp.PrioritizeMessage(msg));
		msg->SetMessageTTL(1);
		queue->Add(msg, priority);

		// Wait a spell and clear starvation
		Sleep(20);
		queue->ClearQueueStarvation(PERSIST_CONTENT_WINDOW);

		msg = queue->Remove();
		ASSERT_TRUE(msg == NULL);

		delete queue;

	}

	TEST(InternalMessageQueueListUnitTests, InternalMessageQueueListIsQueueStarvedNoMP)
	{
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList *queue = new CInternalMessageQueueList(false);

		CMessagePrioritizer mp;
		CInternalMessage *msg = new CInternalMessage(INT_MSG_DISPLAY_SYSTEM_BUSY);
		int priority = 1;
		queue->Add(msg, priority);

		msg = queue->Remove();
		ASSERT_TRUE(msg != NULL);
		ASSERT_TRUE(msg->GetMessageType() == INT_MSG_DISPLAY_SYSTEM_BUSY);
		Assert::IsFalse(queue->IsQueueStarved(PERSIST_CONTENT_WINDOW));

		delete msg;
		delete queue;
	}

	TEST(InternalMessageQueueListUnitTests, InternalMessageQueueListIsQueueStarvedWithMP)
	{
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList *queue = new CInternalMessageQueueList(false);

		CMessagePrioritizer mp;
		CInternalMessage *msg = new CInternalMessage(INT_MSG_DISPLAY_SYSTEM_BUSY);
		int priority = 0;
		CMessagePersist *persist = mp.PrioritizeMessage(msg);
		priority = persist->GetPriority();

		msg->SetMessagePersist(persist); // Will starve indefinitely
		queue->Add(msg, priority);

		msg = queue->Remove();
		ASSERT_TRUE(msg != NULL);
		ASSERT_TRUE(msg->GetMessageType() == INT_MSG_DISPLAY_SYSTEM_BUSY);
		ASSERT_TRUE(queue->IsQueueStarved(PERSIST_CONTENT_WINDOW));

		delete msg;
		delete queue;
	}

	TEST(InternalMessageQueueListUnitTests, InternalMessageQueueListIsPersistTypePresentNoMP)
	{
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList *queue = new CInternalMessageQueueList(false);

		CMessagePrioritizer mp;
		CInternalMessage *msg = new CInternalMessage(INT_MSG_DISPLAY_SYSTEM_BUSY);
		int priority = 1;
		queue->Add(msg, priority);

		Assert::IsFalse(queue->IsPersistTypePresent(PERSIST_CONTENT_WINDOW));

		delete queue;
	}
	TEST(InternalMessageQueueListUnitTests, InternalMessageQueueListIsPersistTypePresentWithMP)
	{
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList *queue = new CInternalMessageQueueList(false);

		CMessagePrioritizer mp;
		CInternalMessage *msg = new CInternalMessage(INT_MSG_DISPLAY_SYSTEM_BUSY);
		int priority = 0;
		CMessagePersist *persist = mp.PrioritizeMessage(msg);
		priority = persist->GetPriority();

		msg->SetMessagePersist(persist); // Will starve indefinitely
		queue->Add(msg, priority);

		ASSERT_TRUE(queue->IsPersistTypePresent(PERSIST_CONTENT_WINDOW));

		delete queue;
	}

	TEST(InternalMessageQueueListUnitTests, InternalMessageQueueListLastMessageStarvedQueue)
	{
		CDirectoryManager::MockInstance();
		CInternalMessageQueueList *queue = new CInternalMessageQueueList(false);

		CMessagePrioritizer mp;
		CInternalMessage *msg = new CInternalMessage(INT_MSG_SPLASHDOWN_AWARD);
		int priority = 0;
		CMessagePersist *persist = mp.PrioritizeMessage(msg);
		priority = persist->GetPriority();
		msg->SetMessagePersist(persist); // Will starve indefinitely
		queue->Add(msg, priority);

		ASSERT_TRUE(INT_MSG_NONE == queue->GetLastMessageStarvedQueue());

		msg = queue->Remove();

		ASSERT_TRUE(INT_MSG_SPLASHDOWN_AWARD == queue->GetLastMessageStarvedQueue());

		delete queue;
	}
}
