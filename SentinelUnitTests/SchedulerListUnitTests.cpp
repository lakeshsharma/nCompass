#include "stdafx.h"
#include "SASProtocol/SchedulerList.h"
#include "Meters.h"
#include "gtest/gtest.h"

namespace SentinelNativeUnitTests
{
		TEST(SchedulerListUnitTests, SchedulerListGetNextTest)
		{
			int item = 0;
			CSchedulerList *list = new CSchedulerList(1, 8);
			for (int i = 1; i <= 10; i++)
			{
				list->AddItem(i);
			}

			// Should get 8 items before returning false, then eight more starting with the 9th.
			for (int i = 1; list->GetNextItem(item); i++)
			{
				ASSERT_TRUE(item == i) << "CSchedulerList->GetNextItem() returned item out of order";
				ASSERT_TRUE(i <= 8) << "CSchedulerList->GetNextItem() returned too many items";
			}

			// get next 2, should be 9, 10.
			for (int i = 9; list->GetNextItem(item) && i <= 10; i++)
			{
				ASSERT_TRUE(item == i) << "CSchedulerList->GetNextItem() returned item out of order";
			}

			// next 1 should be back to 1
			ASSERT_TRUE(item == 1) << "SchedulerList->GetNextItem() didn't wrap to first item in list";
			delete list;
		}

		TEST(SchedulerListUnitTests, SchedulerListComparePriorityTest)
		{
			CSchedulerList *list = new CSchedulerList(1, 8);
			for (int i = 1; i <= 10; i++)
			{
				list->AddItem(i);
			}

			ASSERT_TRUE(list->ComparePriority(0) < 0) << "Priority Compare returned incorrect value";
			ASSERT_TRUE(list->ComparePriority(2) > 0) << "Priority Compare returned incorrect value";
			ASSERT_TRUE(list->ComparePriority(1) == 0) << "Priority Compare returned incorrect value";

			ASSERT_TRUE(list->IsPriorityLevel(1)) << "SchedulerList->IsPriorityLevel() returned false, expected true";
			ASSERT_FALSE(list->IsPriorityLevel(0)) << "SchedulerList->IsPriorityLevel() returned true, expected false";
			delete list;
		}

		TEST(SchedulerListUnitTests, SchedulerListIntervalTest)
		{
			int item = 0;
			CSchedulerList *list = new CSchedulerList(1, 8);
			for (int i = 1; i <= 10; i++)
			{
				list->AddItem(i);
			}

			// Should get 8 items before returning false, then eight more starting with the 9th.
			for (int i = 1; list->GetNextItem(item); i++)
			{
				usleep(10*1000);
			}

			// get next 2, should be 9, 10.
			for (int i = 9; list->GetNextItem(item) && i <= 10; i++)
			{
				usleep(10*1000);
			}

			ASSERT_TRUE(list->LastRetrievalInterval() > 0) << "Priority interval not calculated";

		}
}
