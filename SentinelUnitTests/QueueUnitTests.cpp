#include "stdafx.h"
#include "QueueItem.h"
#include "QueueList.h"
#include "InternalMessageQueueList.h"
#include "InternalMessage.h"
#include "MessagePrioritizer.h"


// Sentinel includes.
#include "GlobalDefs.h"
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class CTestQueueItem : public CQueueItem
	{
	public:
		CTestQueueItem( int itemNumber ) : CQueueItem()
		{ 
			m_itemNumber = itemNumber; 
		};
	public:
		int m_itemNumber;
	};

	TEST(QueueItemTests, QueueItemConstructorTest)
	{
		//Setup
		CQueueItem queueItem;

		//Tests
		ASSERT_TRUE(queueItem.GetPriority() == 0);
	}

	TEST(QueueItemTests, QueueItemPriorityTest)
	{
		//Setup
		CQueueItem queueItem;
		queueItem.SetPriority(QUEUE_PRIORITY_HIGH);

		//Tests
		ASSERT_TRUE(queueItem.GetPriority() == QUEUE_PRIORITY_HIGH);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CQueueItem::SetPriority()
	///     CQueueItem::GetPriority()
	/// </summary>
	TEST(QueueItemTests, CQueueItem_SetPriorityTest)
	{
		// Setup.
		CQueueItem queueItem;

		// Perform operation(s) and test(s).
		queueItem.SetPriority(-1);
		ASSERT_TRUE(queueItem.GetPriority() == -1);

		queueItem.SetPriority(0);
		ASSERT_TRUE(queueItem.GetPriority() == 0);

		queueItem.SetPriority(1);
		ASSERT_TRUE(queueItem.GetPriority() == 1);

		queueItem.SetPriority(INT_MAX - 1);
		ASSERT_TRUE(queueItem.GetPriority() == INT_MAX - 1);

		queueItem.SetPriority(INT_MAX);
		ASSERT_TRUE(queueItem.GetPriority() == INT_MAX);

		queueItem.SetPriority(INT_MIN);
		ASSERT_TRUE(queueItem.GetPriority() == INT_MIN);

		queueItem.SetPriority(INT_MIN + 1);
		ASSERT_TRUE(queueItem.GetPriority() == INT_MIN + 1);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CQueueItem::CQueueItem()
	///     CQueueItem::OkToFlushLowerOnAdd()
	/// Code coverage:
	///     CCardConfig::~CCardConfig()
	// Test default value(s).
	/// </summary>
	TEST(QueueItemTests, CQueueItem_OkToFlushLowerOnAddTest)
	{
		// Perform operation(s) to be tested.
		CQueueItem queueItem; // Getter needs a read/write instance.

		// Test(s).
		ASSERT_TRUE(queueItem.OkToFlushLowerOnAdd() == true);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CQueueItem::SetOkToFlushLowerOnAdd()
	///     CQueueItem::OkToFlushLowerOnAdd()
	/// </summary>
	TEST(QueueItemTests, CQueueItem_SetOkToFlushLowerOnAddTest)
	{
		// Setup.
		CQueueItem queueItem;

		// Perform operation(s) and test(s).
		queueItem.SetOkToFlushLowerOnAdd(false);
		ASSERT_TRUE(queueItem.OkToFlushLowerOnAdd() == false);

		queueItem.SetOkToFlushLowerOnAdd(true);
		ASSERT_TRUE(queueItem.OkToFlushLowerOnAdd() == true);
	}

	TEST(QueueItemTests, QueueListPriorityTest)
	{
		//Setup
		CQueueList *queueList = new CQueueList();
		queueList->AddPriority( QUEUE_PRIORITY_HIGHEST );
		queueList->AddPriority( QUEUE_PRIORITY_HIGH );
		queueList->AddPriority( QUEUE_PRIORITY_ABOVE_NORMAL );
		queueList->AddPriority( QUEUE_PRIORITY_NORMAL );
		queueList->AddPriority( QUEUE_PRIORITY_BELOW_NORMAL );
		queueList->AddPriority( QUEUE_PRIORITY_LOW );
		queueList->AddPriority( QUEUE_PRIORITY_LOWEST );

		CTestQueueItem *qArray[14];
		for ( int qIndex = 0; qIndex < 14; qIndex++ )
		{
			qArray[qIndex] = new CTestQueueItem( qIndex );
		}
		queueList->Add( qArray[12], QUEUE_PRIORITY_LOWEST );
		queueList->Add( qArray[8], QUEUE_PRIORITY_BELOW_NORMAL );
		queueList->Add( qArray[4], QUEUE_PRIORITY_ABOVE_NORMAL );
		queueList->Add( qArray[2], QUEUE_PRIORITY_HIGH );
		queueList->Add( qArray[0], QUEUE_PRIORITY_HIGHEST );
		queueList->Add( qArray[6], QUEUE_PRIORITY_NORMAL );
		queueList->Add( qArray[5], QUEUE_PRIORITY_ABOVE_NORMAL );
		queueList->Add( qArray[10], QUEUE_PRIORITY_LOW );
		queueList->Add( qArray[1], QUEUE_PRIORITY_HIGHEST );
		queueList->Add( qArray[7], QUEUE_PRIORITY_NORMAL );
		queueList->Add( qArray[9], QUEUE_PRIORITY_BELOW_NORMAL );
		queueList->Add( qArray[3], QUEUE_PRIORITY_HIGH );
		queueList->Add( qArray[11], QUEUE_PRIORITY_LOW );
		queueList->Add( qArray[13], QUEUE_PRIORITY_LOWEST );

		// Test
		int expectedItemNumber = 0;
		CTestQueueItem *qi = (CTestQueueItem *)queueList->Remove();
		while ( qi != NULL )
		{
			ASSERT_TRUE(qi->m_itemNumber == expectedItemNumber++);
			delete qi;
			qi = (CTestQueueItem *)queueList->Remove();
		}
		
		delete queueList;

	}

	/// <summary>
	/// Functional test for:
	///     CQueueItem::Add()
	///     CQueueItem::AddPriority()
	///     CQueueItem::Remove()
	/// Test that items are removed in the correct order, even though items and subqueues are added out of order.
	/// </summary>
	TEST(QueueItemTests, QueueListPriorityUnorderedSubqueueTest)
	{
		//Setup
		CQueueList *queueList = new CQueueList();

		CTestQueueItem *qArray[14];
		for (int qIndex = 0; qIndex < 14; qIndex++)
		{
			qArray[qIndex] = new CTestQueueItem(qIndex);
		}

		queueList->Add(qArray[12], QUEUE_PRIORITY_LOWEST);
		queueList->Add(qArray[8], QUEUE_PRIORITY_BELOW_NORMAL);
		queueList->Add(qArray[4], QUEUE_PRIORITY_ABOVE_NORMAL);
		queueList->Add(qArray[2], QUEUE_PRIORITY_HIGH);
		queueList->Add(qArray[0], QUEUE_PRIORITY_HIGHEST);
		queueList->Add(qArray[6], QUEUE_PRIORITY_NORMAL);
		queueList->Add(qArray[5], QUEUE_PRIORITY_ABOVE_NORMAL);
		queueList->Add(qArray[10], QUEUE_PRIORITY_LOW);
		queueList->Add(qArray[1], QUEUE_PRIORITY_HIGHEST);
		queueList->Add(qArray[7], QUEUE_PRIORITY_NORMAL);
		queueList->Add(qArray[9], QUEUE_PRIORITY_BELOW_NORMAL);
		queueList->Add(qArray[3], QUEUE_PRIORITY_HIGH);
		queueList->Add(qArray[11], QUEUE_PRIORITY_LOW);
		queueList->Add(qArray[13], QUEUE_PRIORITY_LOWEST);

		// Test
		int expectedItemNumber = 0;
		CTestQueueItem *qi = (CTestQueueItem *)queueList->Remove();
		while (qi != NULL)
		{
			ASSERT_TRUE(qi->m_itemNumber == expectedItemNumber++);
			delete qi;
			qi = (CTestQueueItem *)queueList->Remove();
		}

		delete queueList;
	}

	TEST(QueueItemTests, QueueStarvationTest)
	{
		//Setup
		CQueueList *queueList = new CQueueList();
		queueList->AddPriority( QUEUE_PRIORITY_HIGHEST );
		queueList->AddPriority( QUEUE_PRIORITY_HIGH );
		queueList->AddPriority( QUEUE_PRIORITY_ABOVE_NORMAL );
		queueList->AddPriority( QUEUE_PRIORITY_NORMAL );
		queueList->AddPriority( QUEUE_PRIORITY_BELOW_NORMAL );
		queueList->AddPriority( QUEUE_PRIORITY_LOW );
		queueList->AddPriority( QUEUE_PRIORITY_LOWEST );

		CTestQueueItem *qArray[14];
		for ( int qIndex = 0; qIndex < 14; qIndex++ )
		{
			qArray[qIndex] = new CTestQueueItem( qIndex );
		}
		queueList->Add( qArray[12], QUEUE_PRIORITY_LOWEST );
		queueList->Add( qArray[8], QUEUE_PRIORITY_BELOW_NORMAL );
		queueList->Add( qArray[4], QUEUE_PRIORITY_ABOVE_NORMAL );
		queueList->Add( qArray[2], QUEUE_PRIORITY_HIGH );
		queueList->Add( qArray[0], QUEUE_PRIORITY_HIGHEST );
		queueList->Add( qArray[6], QUEUE_PRIORITY_NORMAL );
		queueList->Add( qArray[5], QUEUE_PRIORITY_ABOVE_NORMAL );
		queueList->Add( qArray[10], QUEUE_PRIORITY_LOW );
		queueList->Add( qArray[1], QUEUE_PRIORITY_HIGHEST );
		queueList->Add( qArray[7], QUEUE_PRIORITY_NORMAL );
		queueList->Add( qArray[9], QUEUE_PRIORITY_BELOW_NORMAL );
		queueList->Add( qArray[3], QUEUE_PRIORITY_HIGH );
		queueList->Add( qArray[11], QUEUE_PRIORITY_LOW );
		queueList->Add( qArray[13], QUEUE_PRIORITY_LOWEST );

		queueList->SetQueueStarvation(QUEUE_PRIORITY_ABOVE_NORMAL);

		int removalCount = 0;
		CTestQueueItem *qi = (CTestQueueItem *)queueList->Remove();
		while ( qi != NULL )
		{
			removalCount++;
			delete qi;
			qi = (CTestQueueItem *)queueList->Remove();
		}
		ASSERT_TRUE(removalCount == 6);
		
		queueList->ClearQueueStarvation();
		qi = (CTestQueueItem *)queueList->Remove();
		while ( qi != NULL )
		{
			removalCount++;
			delete qi;
			qi = (CTestQueueItem *)queueList->Remove();
		}
		ASSERT_TRUE(removalCount == 14);

		delete queueList;
	}

	TEST(QueueItemTests, InternalMessageQueueTest)
	{
		//Setup
		CInternalMessageQueueList *queueList = new CInternalMessageQueueList();
		queueList->AddPriority( QUEUE_PRIORITY_HIGHEST );
		queueList->AddPriority( QUEUE_PRIORITY_HIGH );
		queueList->AddPriority( QUEUE_PRIORITY_ABOVE_NORMAL );
		queueList->AddPriority( QUEUE_PRIORITY_NORMAL );
		queueList->AddPriority( QUEUE_PRIORITY_BELOW_NORMAL );
		queueList->AddPriority( QUEUE_PRIORITY_LOW );
		queueList->AddPriority( QUEUE_PRIORITY_LOWEST );

		CInternalMessage *mArray[14];
		for ( WORD qIndex = 0; qIndex < 14; qIndex++ )
		{
			mArray[qIndex] = new CInternalMessage( INT_MSG_NONE, qIndex );
		}
		queueList->Add( mArray[12], QUEUE_PRIORITY_LOWEST );
		queueList->Add( mArray[8], QUEUE_PRIORITY_BELOW_NORMAL );
		queueList->Add( mArray[4], QUEUE_PRIORITY_ABOVE_NORMAL );
		queueList->Add( mArray[2], QUEUE_PRIORITY_HIGH );
		queueList->Add( mArray[0], QUEUE_PRIORITY_HIGHEST );
		queueList->Add( mArray[6], QUEUE_PRIORITY_NORMAL );
		queueList->Add( mArray[5], QUEUE_PRIORITY_ABOVE_NORMAL );
		queueList->Add( mArray[10], QUEUE_PRIORITY_LOW );
		queueList->Add( mArray[1], QUEUE_PRIORITY_HIGHEST );
		queueList->Add( mArray[7], QUEUE_PRIORITY_NORMAL );
		queueList->Add( mArray[9], QUEUE_PRIORITY_BELOW_NORMAL );
		queueList->Add( mArray[3], QUEUE_PRIORITY_HIGH );
		queueList->Add( mArray[11], QUEUE_PRIORITY_LOW );
		queueList->Add( mArray[13], QUEUE_PRIORITY_LOWEST );

		// Test
		WORD expectedItemNumber = 0;
		CInternalMessage *qi = (CInternalMessage *)queueList->Remove();
		while ( qi != NULL )
		{
			ASSERT_TRUE(qi->GetWordData() == expectedItemNumber++);
			delete qi;
			qi = (CInternalMessage *)queueList->Remove();
		}
		
		delete queueList;
	}

	TEST(QueueItemTests, InternalMessageQueueTestPeekMsgType)
	{
		//Setup
		CMessagePrioritizer p;
		CInternalMessageQueueList *queueList = new CInternalMessageQueueList();
		queueList->AddPriority( QUEUE_PRIORITY_HIGHEST );
		queueList->AddPriority( QUEUE_PRIORITY_HIGH );
		queueList->AddPriority( QUEUE_PRIORITY_ABOVE_NORMAL );
		queueList->AddPriority( QUEUE_PRIORITY_NORMAL );
		queueList->AddPriority( QUEUE_PRIORITY_BELOW_NORMAL );
		queueList->AddPriority( QUEUE_PRIORITY_LOW );
		queueList->AddPriority( QUEUE_PRIORITY_LOWEST );

		CInternalMessage *msg1 = new CInternalMessage(INT_MSG_RICOCHET_TRIGGER);

		// Attach PERSIST_CONTENT_WINDOW to msg
		int priority = 0;
		msg1->SetMessagePersist(p.PrioritizeMessage(msg1, priority));

		CInternalMessage *msg2 = new CInternalMessage( INT_MSG_SET_METERS );

		queueList->Add( msg2, QUEUE_PRIORITY_HIGH );
		queueList->Add( msg1, QUEUE_PRIORITY_HIGH );

		InternalMsgType msgType;
        DWORD screenId;
        DWORD triggerLessType;
		queueList->PeekMsgType(msgType, screenId, triggerLessType, PERSIST_CONTENT_WINDOW);
		ASSERT_TRUE(msgType == INT_MSG_RICOCHET_TRIGGER);
		queueList->PeekMsgType(msgType, screenId, triggerLessType, PERSIST_EMPLOYEE_WINDOW);
		ASSERT_TRUE(msgType == INT_MSG_NONE);
		
		delete queueList;
	}

    TEST(QueueItemTests, InternalMessageQueueTestFlushMsgType)
	{
		//Setup
		CMessagePrioritizer p;
		CInternalMessageQueueList *queueList = new CInternalMessageQueueList();
		queueList->AddPriority( QUEUE_PRIORITY_HIGHEST );
		queueList->AddPriority( QUEUE_PRIORITY_HIGH );
		queueList->AddPriority( QUEUE_PRIORITY_ABOVE_NORMAL );
		queueList->AddPriority( QUEUE_PRIORITY_NORMAL );
		queueList->AddPriority( QUEUE_PRIORITY_BELOW_NORMAL );
		queueList->AddPriority( QUEUE_PRIORITY_LOW );
		queueList->AddPriority( QUEUE_PRIORITY_LOWEST );

		CInternalMessage *msg1 = new CInternalMessage(INT_MSG_RICOCHET_TRIGGER);

		// Attach PERSIST_CONTENT_WINDOW to msg
		int priority = 0;
		msg1->SetMessagePersist(p.PrioritizeMessage(msg1, priority));

		CInternalMessage *msg2 = new CInternalMessage( INT_MSG_SET_METERS );

		queueList->Add( msg2, QUEUE_PRIORITY_HIGH );
		queueList->Add( msg1, QUEUE_PRIORITY_HIGH );

		InternalMsgType msgType;
		DWORD screenId;
		DWORD triggerLessType;
		queueList->PeekMsgType(msgType, screenId, triggerLessType, PERSIST_CONTENT_WINDOW);
		ASSERT_TRUE(msgType == INT_MSG_RICOCHET_TRIGGER);
		queueList->PeekMsgType(msgType, screenId, triggerLessType, PERSIST_EMPLOYEE_WINDOW);
		ASSERT_TRUE(msgType == INT_MSG_NONE);

		delete queueList;
	}

	TEST(QueueItemTests, MesssagePersistTest)
	{
		CMessagePersist *mp = new CMessagePersist(false, 1000, QUEUE_PRIORITY_HIGH, PERSIST_CONTENT_WINDOW);

		ASSERT_TRUE(mp->GetPriority() == QUEUE_PRIORITY_HIGH);
		ASSERT_TRUE(mp->GetPersistType() == PERSIST_CONTENT_WINDOW);

		DWORD expireTick1 = CUtilities::GetTickCount() + 1000;
		mp->StartClock();
		DWORD expireTick2 = CUtilities::GetTickCount() + 1000;

		ASSERT_TRUE(mp->IsQueueStarved());

		delete mp;
	}


	TEST(QueueItemTests, MessagePersistFlushTest)
	{
		int flushType;
		string flushID100;

		CMessagePersist *mp = new CMessagePersist(false, 1000, QUEUE_PRIORITY_HIGH, PERSIST_CONTENT_WINDOW);
		ASSERT_FALSE(mp->IsFlushType(0));
		ASSERT_FALSE(mp->GetFlushType(flushType));
		ASSERT_FALSE(mp->GetFlushID(flushID100));
		delete mp;
		
		mp = new CMessagePersist(PERSIST_CONTENT_WINDOW);
		ASSERT_FALSE(mp->IsFlushType(0));
		ASSERT_FALSE(mp->GetFlushType(flushType));
		ASSERT_FALSE(mp->GetFlushID(flushID100));

		mp->SetFlushType(0);
		ASSERT_TRUE(mp->IsFlushType(0));
		ASSERT_TRUE(mp->GetFlushType(flushType));
		ASSERT_TRUE(flushType == 0);
		ASSERT_FALSE(mp->IsFlushType(1));
		ASSERT_FALSE(mp->IsFlushTypeAndID(0, ""));
		ASSERT_FALSE(mp->GetFlushID(flushID100));

		flushID100 = _T("100");
		mp->SetFlushTypeAndId(10, flushID100);
		ASSERT_TRUE(mp->IsFlushType(10));
		ASSERT_TRUE(mp->GetFlushType(flushType));
		ASSERT_TRUE(flushType == 10);
		ASSERT_TRUE(mp->IsFlushTypeAndID(10, flushID100));
		ASSERT_FALSE(mp->IsFlushTypeAndID(10, ""));
		ASSERT_FALSE(mp->IsFlushTypeAndID(0, flushID100));
		ASSERT_TRUE(mp->GetFlushID(flushID100));
		ASSERT_TRUE(flushID100 == _T("100"));

		ASSERT_FALSE(mp->IsFlusher());
		mp->SetAsFlusher();
		ASSERT_TRUE(mp->IsFlusher());

		delete mp;
	}



	TEST(QueueItemTests, SetInteractiveStarvationTest)
	{
		CMessagePersist *mp = new CMessagePersist(false, 1000, QUEUE_PRIORITY_HIGH, PERSIST_CONTENT_WINDOW);
		const int PRIORITY = 99;
		const int TICKS = 100;
		mp->SetInteractiveStarvation(PRIORITY, TICKS);

		ASSERT_TRUE(PRIORITY == mp->GetPriority());
		
		delete mp;
	}
}
