
#include "stdafx.h"
#include "../Sentinel/QueueItem.h"
#include "../Sentinel/QueueList.h"
#include "../Sentinel/InternalMessageQueueList.h"
#include "../Sentinel/InternalMessage.h"

#include "gtest/gtest.h"

class InternalMessageQueueListAddItemUnitTests : public ::testing::Test
{
    protected:
        virtual void SetUp() 
        {
        }

        virtual void TearDown() 
        {
        }
};
/// <summary>
/// Code coverage and functional test for:
///     CInternalMessageQueueList::Add()
///     CInternalMessageQueueList::AddItem()
///     CInternalMessageQueueList::Remove()
/// Test with messages that do not have a CMessagePersist instance.
/// </summary>
TEST_F(InternalMessageQueueListAddItemUnitTests, CInternalMessageQueueList_AddItem_DefaultFlushLowerTest)
{
    // Setup.
    CInternalMessageQueueList queueList;

    CInternalMessage *qArray[4];
    for (WORD qIndex = 0; qIndex < _countof(qArray); qIndex++)
    {
        qArray[qIndex] = new CInternalMessage(INT_MSG_NONE, qIndex);
    }

    // Perform operation(s) to be tested.
    ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST))<< L"queueList.Add() failed";

    // Tests.
    WORD expectedItemNumber = 0;
    CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
    while (qi != NULL)
    {
        ASSERT_TRUE(qi->GetWordData() == expectedItemNumber++)<< L"Items removed from priority list in incorrect order";
        delete qi;
        qi = (CInternalMessage *)queueList.Remove();
    }

    ASSERT_TRUE(expectedItemNumber == _countof(qArray))<< L"Wrong number of items removed";
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessageQueueList::FlushLowerPriorityItemsUponAdd()
///     CInternalMessageQueueList::Add()
///     CInternalMessageQueueList::AddItem()
///     CInternalMessageQueueList::Remove()
/// Test with messages that do not have a CMessagePersist instance.
/// </summary>
TEST_F(InternalMessageQueueListAddItemUnitTests, CInternalMessageQueueList_AddItem_SetFlushLowerFalseTest)
{
    // Setup.
    CInternalMessageQueueList queueList;
    queueList.FlushLowerPriorityItemsUponAdd(false);

    CInternalMessage *qArray[4];
    for (WORD qIndex = 0; qIndex < _countof(qArray); qIndex++)
    {
        qArray[qIndex] = new CInternalMessage(INT_MSG_NONE, qIndex);
    }

    // Perform operation(s) to be tested.
    ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST))<< L"queueList.Add() failed";

    // Tests.
    WORD expectedItemNumber = 0;
    CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
    while (qi != NULL)
    {
        ASSERT_TRUE(qi->GetWordData() == expectedItemNumber++)<< L"Items removed from priority list in incorrect order";
        delete qi;
        qi = (CInternalMessage *)queueList.Remove();
    }

    ASSERT_TRUE(expectedItemNumber == _countof(qArray))<< L"Wrong number of items removed";
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessageQueueList::FlushLowerPriorityItemsUponAdd()
///     CInternalMessageQueueList::Add()
///     CInternalMessageQueueList::AddItem()
///     CInternalMessageQueueList::Remove()
/// Test with messages that do not have a CMessagePersist instance.
/// </summary>
TEST_F(InternalMessageQueueListAddItemUnitTests, CInternalMessageQueueList_AddItem_SetFlushLowerDefaultTest)
{
    // Setup.
    CInternalMessageQueueList queueList;
    queueList.FlushLowerPriorityItemsUponAdd();
 
    CInternalMessage *qArray[4];
    for (WORD qIndex = 0; qIndex < _countof(qArray); qIndex++)
    {
        qArray[qIndex] = new CInternalMessage(INT_MSG_NONE, qIndex);
        qArray[qIndex]->SetMessagePersist(new CMessagePersist(PERSIST_NONE));
    }

    // Perform operation(s) to be tested.
    ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST))<< L"queueList.Add() failed";

    // Tests.
    WORD expectedItemNumber = 0;
    CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
    while (qi != NULL)
    {
        ASSERT_TRUE(qi->GetWordData() == expectedItemNumber++)<< L"Items removed from priority list in incorrect order";
        delete qi;
        qi = (CInternalMessage *)queueList.Remove();
    }

    ASSERT_TRUE(expectedItemNumber == _countof(qArray)-1)<< L"Wrong number of items removed";
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessageQueueList::FlushLowerPriorityItemsUponAdd()
///     CInternalMessageQueueList::Add()
///     CInternalMessageQueueList::AddItem()
///     CInternalMessageQueueList::Remove()
/// Test with messages that do not have a CMessagePersist instance.
/// </summary>
TEST_F(InternalMessageQueueListAddItemUnitTests, CInternalMessageQueueList_AddItem_SetFlushLowerTrueTest)
{
    // Setup.
    CInternalMessageQueueList queueList;
    queueList.FlushLowerPriorityItemsUponAdd(true);

    CInternalMessage *qArray[4];
    for (WORD qIndex = 0; qIndex < _countof(qArray); qIndex++)
    {
        qArray[qIndex] = new CInternalMessage(INT_MSG_NONE, qIndex);
        qArray[qIndex]->SetMessagePersist(new CMessagePersist(PERSIST_NONE));
    }

    // Perform operation(s) to be tested.
    ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL))<< "queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST))<< L"queueList.Add() failed";

    // Tests.
    WORD expectedItemNumber = 0;
    CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
    while (qi != NULL)
    {
        ASSERT_TRUE(qi->GetWordData() == expectedItemNumber++)<< L"Items removed from priority list in incorrect order";
        delete qi;
        qi = (CInternalMessage *)queueList.Remove();
    }

    ASSERT_TRUE(expectedItemNumber == _countof(qArray)-1)<< L"Wrong number of items removed";
}

/// <summary>
/// Code coverage and functional test for:
///     CQueueItem::SetOkToFlushLowerOnAdd()
///     CInternalMessageQueueList::FlushLowerPriorityItemsUponAdd()
///     CInternalMessageQueueList::Add()
///     CInternalMessageQueueList::AddItem()
///     CInternalMessageQueueList::Remove()
/// Test with messages that do not have a CMessagePersist instance.
/// </summary>
TEST_F(InternalMessageQueueListAddItemUnitTests, CInternalMessageQueueList_AddItem_FlushNotOkTest)
{
    // Setup.
    CInternalMessageQueueList queueList;
    queueList.FlushLowerPriorityItemsUponAdd(true);

    CInternalMessage *qArray[4];
    for (WORD qIndex = 0; qIndex < _countof(qArray); qIndex++)
    {
        qArray[qIndex] = new CInternalMessage(INT_MSG_NONE, qIndex);
        qArray[qIndex]->SetMessagePersist(new CMessagePersist(PERSIST_NONE));
    }

    // Perform operation(s) to be tested.
    queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL);
    queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL);
    qArray[2]->SetOkToFlushLowerOnAdd(false);
    queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL);
    queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST);

    // Tests.
    WORD expectedItemNumber = 0;
    CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
    while (qi != NULL)
    {
        ASSERT_EQ((unsigned) expectedItemNumber, (unsigned) qi->GetWordData())<< L"Items removed from priority list in incorrect order";
        expectedItemNumber++;

        delete qi;
        qi = (CInternalMessage *)queueList.Remove();
    }

    ASSERT_EQ((unsigned) expectedItemNumber, (unsigned) _countof(qArray))<< L"Items removed";
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessageQueueList::Add()
///     CInternalMessageQueueList::AddItem()
///     CInternalMessageQueueList::Remove()
/// Test with messages that are explicitly PERSIST_NONE.
/// </summary>
TEST_F(InternalMessageQueueListAddItemUnitTests, CInternalMessageQueueList_AddItemPersistNone_DefaultFlushLowerTest)
{
    // Setup.
    CInternalMessageQueueList queueList;

    CInternalMessage *qArray[4];
    for (WORD qIndex = 0; qIndex < _countof(qArray); qIndex++)
    {
        qArray[qIndex] = new CInternalMessage(INT_MSG_NONE, qIndex);
        qArray[qIndex]->SetMessagePersist(new CMessagePersist(PERSIST_NONE));
    }

    // Perform operation(s) to be tested.
    ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST))<< L"queueList.Add() failed";

    // Tests.
    WORD expectedItemNumber = 0;
    CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
    while (qi != NULL)
    {
        ASSERT_TRUE(qi->GetWordData() == expectedItemNumber++)<< L"Items removed from priority list in incorrect order";
        delete qi;
        qi = (CInternalMessage *)queueList.Remove();
    }

    ASSERT_TRUE(expectedItemNumber == _countof(qArray))<< L"Wrong number of items removed";
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessageQueueList::FlushLowerPriorityItemsUponAdd()
///     CInternalMessageQueueList::Add()
///     CInternalMessageQueueList::AddItem()
///     CInternalMessageQueueList::Remove()
/// Test with messages that are explicitly PERSIST_NONE.
/// </summary>
TEST_F(InternalMessageQueueListAddItemUnitTests, CInternalMessageQueueList_AddItemPersistNone_SetFlushLowerFalseTest)
{
    // Setup.
    CInternalMessageQueueList queueList;
    queueList.FlushLowerPriorityItemsUponAdd(false);

    CInternalMessage *qArray[4];
    for (WORD qIndex = 0; qIndex < _countof(qArray); qIndex++)
    {
        qArray[qIndex] = new CInternalMessage(INT_MSG_NONE, qIndex);
        qArray[qIndex]->SetMessagePersist(new CMessagePersist(PERSIST_NONE));
    }

    // Perform operation(s) to be tested.
    ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST))<< L"queueList.Add() failed";

    // Tests.
    WORD expectedItemNumber = 0;
    CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
    while (qi != NULL)
    {
        ASSERT_TRUE(qi->GetWordData() == expectedItemNumber++)<< L"Items removed from priority list in incorrect order";
        delete qi;
        qi = (CInternalMessage *)queueList.Remove();
    }

    ASSERT_TRUE(expectedItemNumber == _countof(qArray))<< L"Wrong number of items removed";
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessageQueueList::FlushLowerPriorityItemsUponAdd()
///     CInternalMessageQueueList::Add()
///     CInternalMessageQueueList::AddItem()
///     CInternalMessageQueueList::Remove()
/// Test with messages that are explicitly PERSIST_NONE.
/// </summary>
TEST_F(InternalMessageQueueListAddItemUnitTests, CInternalMessageQueueList_AddItemPersistNone_SetFlushLowerDefaultTest)
{
    // Setup.
    CInternalMessageQueueList queueList;
    queueList.FlushLowerPriorityItemsUponAdd();

    CInternalMessage *qArray[4];
    for (WORD qIndex = 0; qIndex < _countof(qArray); qIndex++)
    {
        qArray[qIndex] = new CInternalMessage(INT_MSG_NONE, qIndex);
        qArray[qIndex]->SetMessagePersist(new CMessagePersist(PERSIST_NONE));
    }

    // Perform operation(s) to be tested.
    ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST))<< L"queueList.Add() failed";

    // Tests.
    WORD expectedItemNumber = 0;
    CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
    while (qi != NULL)
    {
        ASSERT_TRUE(qi->GetWordData() == expectedItemNumber++)<< L"Items removed from priority list in incorrect order";
        delete qi;
        qi = (CInternalMessage *)queueList.Remove();
    }

    ASSERT_TRUE(expectedItemNumber == _countof(qArray) - 1)<< L"Wrong number of items removed";
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessageQueueList::FlushLowerPriorityItemsUponAdd()
///     CInternalMessageQueueList::Add()
///     CInternalMessageQueueList::AddItem()
///     CInternalMessageQueueList::Remove()
/// Test with messages that are explicitly PERSIST_NONE.
/// </summary>
TEST_F(InternalMessageQueueListAddItemUnitTests, CInternalMessageQueueList_AddItemPersistNone_SetFlushLowerTrueTest)
{
    // Setup.
    CInternalMessageQueueList queueList;
    queueList.FlushLowerPriorityItemsUponAdd(true);

    CInternalMessage *qArray[4];
    for (WORD qIndex = 0; qIndex < _countof(qArray); qIndex++)
    {
        qArray[qIndex] = new CInternalMessage(INT_MSG_NONE, qIndex);
        qArray[qIndex]->SetMessagePersist(new CMessagePersist(PERSIST_NONE));
    }

    // Perform operation(s) to be tested.
    ASSERT_TRUE(queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL))<< L"queueList.Add() failed";
    ASSERT_TRUE(queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST))<< L"queueList.Add() failed";

    // Tests.
    WORD expectedItemNumber = 0;
    CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
    while (qi != NULL)
    {
        ASSERT_TRUE(qi->GetWordData() == expectedItemNumber++)<< L"Items removed from priority list in incorrect order";
        delete qi;
        qi = (CInternalMessage *)queueList.Remove();
    }

    ASSERT_TRUE(expectedItemNumber == _countof(qArray) - 1)<< L"Wrong number of items removed";
}

/// <summary>
/// Code coverage and functional test for:
///     CQueueItem::SetOkToFlushLowerOnAdd()
///     CInternalMessageQueueList::FlushLowerPriorityItemsUponAdd()
///     CInternalMessageQueueList::Add()
///     CInternalMessageQueueList::AddItem()
///     CInternalMessageQueueList::Remove()
/// Test with messages that are explicitly PERSIST_NONE.
/// </summary>
TEST_F(InternalMessageQueueListAddItemUnitTests, CInternalMessageQueueList_AddItemPersistNone_FlushNotOkTest)
{
    // Setup.
    CInternalMessageQueueList queueList;
    queueList.FlushLowerPriorityItemsUponAdd(true);

    CInternalMessage *qArray[4];
    for (WORD qIndex = 0; qIndex < _countof(qArray); qIndex++)
    {
        qArray[qIndex] = new CInternalMessage(INT_MSG_NONE, qIndex);
        qArray[qIndex]->SetMessagePersist(new CMessagePersist(PERSIST_NONE));
    }

    // Perform operation(s) to be tested.
    queueList.Add(qArray[1], QUEUE_PRIORITY_ABOVE_NORMAL);
    queueList.Add(qArray[3], QUEUE_PRIORITY_BELOW_NORMAL);
    qArray[2]->SetOkToFlushLowerOnAdd(false);
    queueList.Add(qArray[2], QUEUE_PRIORITY_NORMAL);
    queueList.Add(qArray[0], QUEUE_PRIORITY_HIGHEST);

    // Tests.
    WORD expectedItemNumber = 0;
    CInternalMessage *qi = (CInternalMessage *)queueList.Remove();
    while (qi != NULL)
    {
        ASSERT_TRUE(qi->GetWordData() == expectedItemNumber++)<< L"Items removed from priority list in incorrect order";
        delete qi;
        qi = (CInternalMessage *)queueList.Remove();
    }

    ASSERT_TRUE(expectedItemNumber == _countof(qArray))<< L"Wrong number of items removed";
}
	