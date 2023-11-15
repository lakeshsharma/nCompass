#define protected public

#include "stdafx.h"
#include "SASProtocol/SASQueueList.h"
#include "SASProtocol/LongPoll20Message.h"
#include "SASProtocol/SASPollMessage.h"

// Sentinel includes.
#include "GlobalDefs.h"
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;

namespace SentinelNativeUnitTests
{
   TEST(SASQueueListUnitTests, CSASQueueList_ForceRemoveItem)
   {
      CSASQueueList *queueList = new CSASQueueList();

      CLongPoll20Message *lp20 = new CLongPoll20Message(1);
      CLongPoll20Message *msg = new CLongPoll20Message(1);

      queueList->Add(lp20, QUEUE_PRIORITY_NORMAL);
      queueList->Add(msg, QUEUE_PRIORITY_NORMAL);

      queueList->ForceRemoveItem(lp20, QUEUE_PRIORITY_NORMAL);

      CSASPollMessage *qMsg = (CSASPollMessage *)queueList->Remove();
      ASSERT_EQ(qMsg, msg);

      qMsg = (CSASPollMessage *)queueList->Remove();
      ASSERT_EQ(qMsg, nullptr);

      delete lp20;
      delete msg;
      delete queueList;
   }

   TEST(SASQueueListUnitTests, CSASQueueList_ForceRemoveItem_NotFound)
   {
      CSASQueueList *queueList = new CSASQueueList();

      CLongPoll20Message *lp20 = new CLongPoll20Message(1);
      CLongPoll20Message *msg = new CLongPoll20Message(1);

      queueList->Add(msg, QUEUE_PRIORITY_NORMAL);

      queueList->ForceRemoveItem(lp20, QUEUE_PRIORITY_NORMAL);

      CSASPollMessage *qMsg = (CSASPollMessage *)queueList->Remove();
      ASSERT_EQ(qMsg, msg);

      qMsg = (CSASPollMessage *)queueList->Remove();
      ASSERT_EQ(qMsg, nullptr);

      delete lp20;
      delete msg;
      delete queueList;
   }
}

