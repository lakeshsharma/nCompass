#include "stdafx.h"
#include "QueueList.h"

// Sentinel includes.
#include "GlobalDefs.h"
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;


// Unit testing includes.
//TODO #include "UnitTestingToString.h"


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

	/// <summary>
	/// Code coverage and functional test for:
	///     CQueueList::Add()
	///     CQueueList::AddItem()
	///     CQueueList::Remove()
	/// </summary>
	TEST(QueueListUnitTests, CQueueList_AddItem_DefaultFlushLowerTest)
	{
		// Setup.
		CQueueList queueList;

		CTestQueueItem *qArray[4];
		for (int qIndex = 0; qIndex < _countof(qArray); qIndex++)
		{
			qArray[qIndex] = new CTestQueueItem(qIndex);
		}

		// Perform operation(s) to be tested.
		ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST));

		// Tests.
		int expectedItemNumber = 0;
		CTestQueueItem *qi = (CTestQueueItem *)queueList.Remove();
		while (qi != NULL)
		{
			ASSERT_TRUE(qi->m_itemNumber == expectedItemNumber++);
			delete qi;
			qi = (CTestQueueItem *)queueList.Remove();
		}

		ASSERT_TRUE(expectedItemNumber == _countof(qArray));
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CQueueList::Add()
	///     CQueueList::AddItem()
	///     CQueueList::Remove()
	/// Test default flush behavior with logging enabled and CInternalMessage items.
	/// </summary>
	TEST(QueueListUnitTests, CQueueList_AddItem_DefaultFlushLowerLogTest)
	{
		// Setup.
		CQueueList queueList(_T("logIdentifier"));

		CInternalMessage *qArray[4];
		for (int qIndex = 0; qIndex < _countof(qArray); qIndex++)
		{
			qArray[qIndex] = new CInternalMessage((InternalMsgType)qIndex);
		}

		// Perform operation(s) to be tested.
		ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST));

		// Tests.
		int expectedItemNumber = 0;
		CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
		while (qi != NULL)
		{
			ASSERT_EQ(qi->GetMessageType(), (InternalMsgType)expectedItemNumber++);
			delete qi;
			qi = (CInternalMessage *)queueList.Remove();
		}

		ASSERT_TRUE(expectedItemNumber == _countof(qArray));
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CQueueList::FlushLowerPriorityItemsUponAdd()
	///     CQueueList::Add()
	///     CQueueList::AddItem()
	///     CQueueList::Remove()
	/// </summary>
	TEST(QueueListUnitTests, CQueueList_AddItem_SetFlushLowerFalseTest)
	{
		// Setup.
		CQueueList queueList;
		queueList.FlushLowerPriorityItemsUponAdd(false);

		CTestQueueItem *qArray[4];
		for (int qIndex = 0; qIndex < _countof(qArray); qIndex++)
		{
			qArray[qIndex] = new CTestQueueItem(qIndex);
		}

		// Perform operation(s) to be tested.
		ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST));

		// Tests.
		int expectedItemNumber = 0;
		CTestQueueItem *qi = (CTestQueueItem *)queueList.Remove();
		while (qi != NULL)
		{
			ASSERT_TRUE(qi->m_itemNumber == expectedItemNumber++);
			delete qi;
			qi = (CTestQueueItem *)queueList.Remove();
		}

		ASSERT_TRUE(expectedItemNumber == _countof(qArray));
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CQueueList::FlushLowerPriorityItemsUponAdd()
	///     CQueueList::Add()
	///     CQueueList::AddItem()
	///     CQueueList::Remove()
	/// </summary>
	TEST(QueueListUnitTests, CQueueList_AddItem_SetFlushLowerDefaultTest)
	{
		// Setup.
		CQueueList queueList;
		queueList.FlushLowerPriorityItemsUponAdd();

		CTestQueueItem *qArray[4];
		for (int qIndex = 0; qIndex < _countof(qArray); qIndex++)
		{
			qArray[qIndex] = new CTestQueueItem(qIndex);
		}

		// Perform operation(s) to be tested.
		ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST));

		// Tests.
		int expectedItemNumber = 0;
		CTestQueueItem *qi = (CTestQueueItem *)queueList.Remove();
		while (qi != NULL)
		{
			ASSERT_TRUE(qi->m_itemNumber == expectedItemNumber++);
			delete qi;
			qi = (CTestQueueItem *)queueList.Remove();
		}

		ASSERT_TRUE(expectedItemNumber == _countof(qArray) - 1);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CQueueList::FlushLowerPriorityItemsUponAdd()
	///     CQueueList::Add()
	///     CQueueList::AddItem()
	///     CQueueList::Remove()
	/// </summary>
	TEST(QueueListUnitTests, CQueueList_AddItem_SetFlushLowerTrueTest)
	{
		// Setup.
		CQueueList queueList;
		queueList.FlushLowerPriorityItemsUponAdd(true);

		CTestQueueItem *qArray[4];
		for (int qIndex = 0; qIndex < _countof(qArray); qIndex++)
		{
			qArray[qIndex] = new CTestQueueItem(qIndex);
		}

		// Perform operation(s) to be tested.
		ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST));

		// Tests.
		int expectedItemNumber = 0;
		CTestQueueItem *qi = (CTestQueueItem *)queueList.Remove();
		while (qi != NULL)
		{
			ASSERT_TRUE(qi->m_itemNumber == expectedItemNumber++);
			delete qi;
			qi = (CTestQueueItem *)queueList.Remove();
		}

		ASSERT_TRUE(expectedItemNumber == _countof(qArray) - 1);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CQueueList::FlushLowerPriorityItemsUponAdd()
	///     CQueueList::Add()
	///     CQueueList::AddItem()
	///     CQueueList::Remove()
	/// Test flushing lower priority items when logging is enabled.
	/// </summary>
	TEST(QueueListUnitTests, CQueueList_AddItem_SetFlushLowerTrueLogTest)
	{
		// Setup.
		CQueueList queueList(_T("logIdentifier"));
		queueList.FlushLowerPriorityItemsUponAdd(true);

		CTestQueueItem *qArray[4];
		for (int qIndex = 0; qIndex < _countof(qArray); qIndex++)
		{
			qArray[qIndex] = new CTestQueueItem(qIndex);
		}

		// Perform operation(s) to be tested.
		ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL));
		ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST));

		// Tests.
		int expectedItemNumber = 0;
		CTestQueueItem *qi = (CTestQueueItem *)queueList.Remove();
		while (qi != NULL)
		{
			ASSERT_TRUE(qi->m_itemNumber == expectedItemNumber++);
			delete qi;
			qi = (CTestQueueItem *)queueList.Remove();
		}

		ASSERT_TRUE(expectedItemNumber == _countof(qArray) - 1);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CQueueItem::SetOkToFlushLowerOnAdd()
	///     CQueueList::FlushLowerPriorityItemsUponAdd()
	///     CQueueList::Add()
	///     CQueueList::AddItem()
	///     CQueueList::Remove()
	/// </summary>
	TEST(QueueListUnitTests, CQueueList_AddItem_FlushNotOkTest)
	{
		// Setup.
		CQueueList queueList;
		queueList.FlushLowerPriorityItemsUponAdd(true);

		CTestQueueItem *qArray[4];
		for (int qIndex = 0; qIndex < _countof(qArray); qIndex++)
		{
			qArray[qIndex] = new CTestQueueItem(qIndex);
		}

		// Perform operation(s) to be tested.
		queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL);
		queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL);
		qArray[2]->SetOkToFlushLowerOnAdd(false);
		queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL);
		queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST);

		// Tests.
		int expectedItemNumber = 0;
		CTestQueueItem *qi = (CTestQueueItem *)queueList.Remove();
		while (qi != NULL)
		{
			ASSERT_TRUE(qi->m_itemNumber == expectedItemNumber++);
			delete qi;
			qi = (CTestQueueItem *)queueList.Remove();
		}

		ASSERT_TRUE(expectedItemNumber == _countof(qArray));
	}
}