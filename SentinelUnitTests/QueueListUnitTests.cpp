#define protected public

#include "stdafx.h"
#include "QueueList.h"

// Sentinel includes.
#include "GlobalDefs.h"
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	/// <summary>
	/// Functional test for:
	///     CQueueList::CQueueList()
	/// Test default value for m_starveThresholdPriority.
	/// </summary>
	TEST(QueueListUnitTests, CQueueList_StarveThresholdPriorityTest)
	{
		// Perform operation(s) to be tested.
		CQueueList queueList;
		ASSERT_EQ(NO_QUEUE_STARVATION, queueList.m_starveThresholdPriority);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::CQueueList()
	/// Test default value for m_messagePersistType.
	/// </summary>
	TEST(QueueListUnitTests, CQueueList_MessagePersistTypeTest)
	{
		// Perform operation(s) to be tested.
		CQueueList queueList;
		ASSERT_EQ(PERSIST_NONE, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::CQueueList()
	/// Test default value for m_bFlushLowerOnAdd.
	/// </summary>
	TEST(QueueListUnitTests, CQueueList_FlushLowerOnAddTest)
	{
		// Perform operation(s) to be tested.
		CQueueList queueList;
		ASSERT_FALSE(queueList.m_bFlushLowerOnAdd);
	}

// #pragma region CQueueList_LogIdentifier
	/// <summary>
	/// Functional test for:
	///     CQueueList::CQueueList()
	///     CQueueList::LoggingEnabled()
	/// Code coverage test for:
	///     CQueueList::~CQueueList()
	/// Test with default value for logIdentifier.
	/// </summary>
	TEST(QueueListUnitTests, CQueueList_LogIdentifierDefaultTest)
	{
		// Data.
		const string logIdentifierExpected;
		const bool loggingEnabled(!logIdentifierExpected.empty());

		// Perform operation(s) to be tested.
		CQueueList queueList;
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled());
		ASSERT_EQ(logIdentifierExpected, queueList.m_logIdentifier);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::CQueueList()
	///     CQueueList::LoggingEnabled()
	/// Code coverage test for:
	///     CQueueList::~CQueueList()
	/// Test with nullptr value for logIdentifier.
	/// </summary>
	TEST(QueueListUnitTests, CQueueList_LogIdentifierNullTest)
	{
		// Data.
		const string logIdentifier;
		const string logIdentifierExpected;
		const bool loggingEnabled(!logIdentifierExpected.empty());

		// Perform operation(s) to be tested.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled());
		ASSERT_EQ(logIdentifierExpected, queueList.m_logIdentifier);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::CQueueList()
	///     CQueueList::LoggingEnabled()
	/// Code coverage test for:
	///     CQueueList::~CQueueList()
	/// Test with empty string value for logIdentifier.
	/// </summary>
	TEST(QueueListUnitTests, CQueueList_LogIdentifierEmptyTest)
	{
		// Data.
		const string logIdentifier;
		const string logIdentifierExpected = logIdentifier;
		const bool loggingEnabled(!logIdentifierExpected.empty());

		// Perform operation(s) to be tested.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled());
		ASSERT_EQ(logIdentifierExpected, queueList.m_logIdentifier);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::CQueueList()
	///     CQueueList::LoggingEnabled()
	/// Code coverage test for:
	///     CQueueList::~CQueueList()
	/// Test with non-empty string value for logIdentifier.
	/// </summary>
	TEST(QueueListUnitTests, CQueueList_LogIdentifierNonEmptyTest)
	{
		// Data.
		const string logIdentifier = "abcde";
		const string logIdentifierExpected = logIdentifier;
		const bool loggingEnabled(!logIdentifierExpected.empty());

		// Perform operation(s) to be tested.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled());
		ASSERT_EQ(logIdentifierExpected, queueList.m_logIdentifier);
	}
// #pragma endregion CQueueList_LogIdentifier

// #pragma region AddPriority
	void AddPriorityTest(int priorityLevel, int priorityRatio, int priorityRatioExpected)
	{
		// Setup.
		CQueueList queueList("logIdentifier");

		// Perform operation(s) and TEST(QueueListUnitTests, s).
		queueList.AddPriority(priorityLevel, priorityRatio);
		ASSERT_EQ(1, queueList.m_subQueueArray.size());

		CSubQueue *subQueue((CSubQueue *)queueList.m_subQueueArray[0]);
		ASSERT_TRUE(subQueue != nullptr);
		ASSERT_EQ(priorityLevel, subQueue->GetPriorityLevel());
	}

	void AddPriorityImpliedRatioTest(int priorityLevel, int priorityRatioExpected)
	{
		// Setup.
		CQueueList queueList("logIdentifier");

		// Perform operation(s) and TEST(QueueListUnitTests, s).
		queueList.AddPriority(priorityLevel);
		ASSERT_EQ(1, queueList.m_subQueueArray.size());

		CSubQueue *subQueue((CSubQueue *)queueList.m_subQueueArray[0]);
		ASSERT_TRUE(subQueue != nullptr);
		ASSERT_EQ(priorityLevel, subQueue->GetPriorityLevel());
	}

	/// <summary>
	/// Functional test for:
	///     CQueueItem::AddPriority()
	/// Test with a variety of priority levels.
	/// </summary>
	TEST(QueueListUnitTests, AddPriority_PriorityLevelRangeTest)
	{
		AddPriorityTest(INT_MIN, 0, 0);
		AddPriorityTest(INT_MIN + 1, 0, 0);
		AddPriorityTest(-1, 0, 0);
		AddPriorityTest(0, 1, 1);
		AddPriorityTest(1, 0, 0);
		AddPriorityTest(INT_MAX - 1, 0, 0);
		AddPriorityTest(INT_MAX, 0, 0);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueItem::AddPriority()
	/// Test with a variety of priority ratios.
	/// </summary>
	TEST(QueueListUnitTests, AddPriority_PriorityRatioRangeTest)
	{
		// Don't use magic values for priority ratios.
		ASSERT_EQ((int)-1, USE_DEFAULT_QUEUE_RATIO);

		AddPriorityTest(QUEUE_PRIORITY_HIGHEST, INT_MIN, INT_MIN);
		AddPriorityTest(QUEUE_PRIORITY_HIGHEST, INT_MIN + 1, INT_MIN + 1);
		AddPriorityTest(QUEUE_PRIORITY_HIGHEST, -2, -2);
		AddPriorityTest(QUEUE_PRIORITY_HIGHEST, 0, 0);
		AddPriorityTest(QUEUE_PRIORITY_HIGHEST, 1, 1);
		AddPriorityTest(QUEUE_PRIORITY_HIGHEST, INT_MAX - 1, INT_MAX - 1);
		AddPriorityTest(QUEUE_PRIORITY_HIGHEST, INT_MAX, INT_MAX);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueItem::Add()
	///     CQueueItem::AddPriority()
	/// Test with explicit default parameter for priority ratios.
	/// </summary>
	TEST(QueueListUnitTests, AddPriority_PriorityRatiosUseDefaultTest)
	{
		AddPriorityTest(-1, USE_DEFAULT_QUEUE_RATIO, DEFAULT_QP_RATIO_OTHER);
		AddPriorityTest(0, USE_DEFAULT_QUEUE_RATIO, DEFAULT_QP_RATIO_OTHER);
		AddPriorityTest(QUEUE_PRIORITY_HIGHEST, USE_DEFAULT_QUEUE_RATIO, DEFAULT_QP_RATIO_HIGHEST);
		AddPriorityTest(QUEUE_PRIORITY_HIGH, USE_DEFAULT_QUEUE_RATIO, DEFAULT_QP_RATIO_HIGH);
		AddPriorityTest(QUEUE_PRIORITY_ABOVE_NORMAL, USE_DEFAULT_QUEUE_RATIO, DEFAULT_QP_RATIO_ABOVE_NORMAL);
		AddPriorityTest(QUEUE_PRIORITY_NORMAL, USE_DEFAULT_QUEUE_RATIO, DEFAULT_QP_RATIO_NORMAL);
		AddPriorityTest(QUEUE_PRIORITY_BELOW_NORMAL, USE_DEFAULT_QUEUE_RATIO, DEFAULT_QP_RATIO_BELOW_NORMAL);
		AddPriorityTest(QUEUE_PRIORITY_LOW, USE_DEFAULT_QUEUE_RATIO, DEFAULT_QP_RATIO_LOW);
		AddPriorityTest(QUEUE_PRIORITY_LOWEST, USE_DEFAULT_QUEUE_RATIO, DEFAULT_QP_RATIO_LOWEST);
		AddPriorityTest(999, USE_DEFAULT_QUEUE_RATIO, DEFAULT_QP_RATIO_OTHER);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueItem::Add()
	///     CQueueItem::AddPriority()
	/// Test with implied default parameter for priority ratios.
	/// </summary>
	TEST(QueueListUnitTests, AddPriority_PriorityRatiosImpliedTest)
	{
		AddPriorityImpliedRatioTest(-1, DEFAULT_QP_RATIO_OTHER);
		AddPriorityImpliedRatioTest(0, DEFAULT_QP_RATIO_OTHER);
		AddPriorityImpliedRatioTest(QUEUE_PRIORITY_HIGHEST, DEFAULT_QP_RATIO_HIGHEST);
		AddPriorityImpliedRatioTest(QUEUE_PRIORITY_HIGH, DEFAULT_QP_RATIO_HIGH);
		AddPriorityImpliedRatioTest(QUEUE_PRIORITY_ABOVE_NORMAL, DEFAULT_QP_RATIO_ABOVE_NORMAL);
		AddPriorityImpliedRatioTest(QUEUE_PRIORITY_NORMAL, DEFAULT_QP_RATIO_NORMAL);
		AddPriorityImpliedRatioTest(QUEUE_PRIORITY_BELOW_NORMAL, DEFAULT_QP_RATIO_BELOW_NORMAL);
		AddPriorityImpliedRatioTest(QUEUE_PRIORITY_LOW, DEFAULT_QP_RATIO_LOW);
		AddPriorityImpliedRatioTest(QUEUE_PRIORITY_LOWEST, DEFAULT_QP_RATIO_LOWEST);
		AddPriorityImpliedRatioTest(999, DEFAULT_QP_RATIO_OTHER);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueItem::AddPriority()
	/// Add the same priority level twice.
	/// </summary>
	TEST(QueueListUnitTests, AddPriority_SameTest)
	{
		// Data.
		int priorityLevel0(QUEUE_PRIORITY_HIGHEST);
		int priorityLevel1(QUEUE_PRIORITY_HIGHEST);
		int priorityRatio0(11);
		int priorityRatio1(2);

		// Setup.
		CQueueList queueList(_T("logIdentifier"));

		// Perform operation(s) and test(s).
		queueList.AddPriority(priorityLevel0, priorityRatio0);
		queueList.AddPriority(priorityLevel1, priorityRatio1);
		ASSERT_EQ(2, queueList.m_subQueueArray.size());

		CSubQueue *subQueue0((CSubQueue *)queueList.m_subQueueArray[0]);
		ASSERT_TRUE(subQueue0 != nullptr);
		ASSERT_EQ(priorityLevel0, subQueue0->GetPriorityLevel());

		CSubQueue *subQueue1((CSubQueue *)queueList.m_subQueueArray[1]);
		ASSERT_TRUE(subQueue1 != nullptr);
		ASSERT_EQ(priorityLevel1, subQueue1->GetPriorityLevel());
	}
// #pragma endregion AddPriority

// #pragma region SetQueueStarvation
	/// <summary>
	/// Functional test for:
	///     CQueueList::SetQueueStarvation()
	/// Test with default value for msgPersist and logging disabled.
	/// </summary>
	TEST(QueueListUnitTests, SetQueueStarvation_MsgPersistDefaultTest)
	{
		// Data.
		const string logIdentifier;
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority(QUEUE_PRIORITY_HIGH);
		const MessagePersistType messagePersistType(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.

		// Perform operation(s) to be tested.
		queueList.SetQueueStarvation(starveThresholdPriority);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::SetQueueStarvation()
	/// Test with nullptr value for msgPersist and logging disabled.
	/// </summary>
	TEST(QueueListUnitTests, SetQueueStarvation_MsgPersistNullTest)
	{
		// Data.
		const string logIdentifier;
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority(QUEUE_PRIORITY_HIGH);
		const MessagePersistType messagePersistType(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.
		CMessagePersist *msgPersist(nullptr);

		// Perform operation(s) to be tested.
		queueList.SetQueueStarvation(starveThresholdPriority, msgPersist);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::SetQueueStarvation()
	/// Test with non-default value for msgPersist and logging disabled.
	/// </summary>
	TEST(QueueListUnitTests, SetQueueStarvation_MsgPersistTest)
	{
		// Data.
		const string logIdentifier;
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority(QUEUE_PRIORITY_HIGH);
		const MessagePersistType messagePersistType(PERSIST_JACKPOTHIT);

		// Setup.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.
		CMessagePersist *msgPersist(new CMessagePersist(messagePersistType));

		// Perform operation(s) to be tested.
		queueList.SetQueueStarvation(starveThresholdPriority, msgPersist);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::SetQueueStarvation()
	/// Test with default value for msgPersist and logging enabled.
	/// </summary>
	TEST(QueueListUnitTests, SetQueueStarvation_MsgPersistDefaultLogTest)
	{
		// Data.
		const string logIdentifier = "abcde";
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority(QUEUE_PRIORITY_HIGH);
		const MessagePersistType messagePersistType(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.

		// Perform operation(s) to be tested.
		queueList.SetQueueStarvation(starveThresholdPriority);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::SetQueueStarvation()
	/// Test with nullptr value for msgPersist and logging enabled.
	/// </summary>
	TEST(QueueListUnitTests, SetQueueStarvation_MsgPersistNullLogTest)
	{
		// Data.
		const string logIdentifier = "abcde";
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority(QUEUE_PRIORITY_HIGH);
		const MessagePersistType messagePersistType(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.
		CMessagePersist *msgPersist(nullptr);

		// Perform operation(s) to be tested.
		queueList.SetQueueStarvation(starveThresholdPriority, msgPersist);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::SetQueueStarvation()
	/// Test with non-default value for msgPersist and logging enabled.
	/// </summary>
	TEST(QueueListUnitTests, SetQueueStarvation_MsgPersistLogTest)
	{
		// Data.
		const string logIdentifier = "abcde";
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority(QUEUE_PRIORITY_HIGH);
		const MessagePersistType messagePersistType(PERSIST_JACKPOTHIT);

		// Setup.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.
		CMessagePersist *msgPersist(new CMessagePersist(messagePersistType));

		// Perform operation(s) to be tested.
		queueList.SetQueueStarvation(starveThresholdPriority, msgPersist);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::SetQueueStarvation()
	/// Test with non-default and nullptr values for msgPersist and logging enabled.
	/// Set the queue starvation twice without clearing.
	/// </summary>
	TEST(QueueListUnitTests, SetQueueStarvation_MsgPersist2NullLogTest)
	{
		// Data.
		const string logIdentifier = "abcde";
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(QUEUE_PRIORITY_HIGH);
		const MessagePersistType messagePersistType1(PERSIST_JACKPOTHIT);
		const int starveThresholdPriority2(QUEUE_PRIORITY_LOW);
		const MessagePersistType messagePersistType2(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.
		CMessagePersist *msgPersist1(new CMessagePersist(messagePersistType1));
		CMessagePersist *msgPersist2(nullptr);

		// Perform operation(s) to be tested.
		queueList.SetQueueStarvation(starveThresholdPriority1, msgPersist1);
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		queueList.SetQueueStarvation(starveThresholdPriority2, msgPersist2);
		ASSERT_EQ(starveThresholdPriority2, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType2, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::SetQueueStarvation()
	/// Test with non-default values for msgPersist and logging enabled.
	/// Set the queue starvation twice without clearing.
	/// </summary>
	TEST(QueueListUnitTests, SetQueueStarvation_MsgPersist2LogTest)
	{
		// Data.
		const string logIdentifier = "abcde";
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(QUEUE_PRIORITY_HIGH);
		const MessagePersistType messagePersistType1(PERSIST_JACKPOTHIT);
		const int starveThresholdPriority2(QUEUE_PRIORITY_LOW);
		const MessagePersistType messagePersistType2(PERSIST_EMPLOYEE_WINDOW);

		// Setup.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.
		CMessagePersist *msgPersist1(new CMessagePersist(messagePersistType1));
		CMessagePersist *msgPersist2(new CMessagePersist(messagePersistType2));

		// Perform operation(s) to be tested.
		queueList.SetQueueStarvation(starveThresholdPriority1, msgPersist1);
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		queueList.SetQueueStarvation(starveThresholdPriority2, msgPersist2);
		ASSERT_EQ(starveThresholdPriority2, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType2, queueList.m_messagePersistType);
	}
// #pragma endregion SetQueueStarvation

// #pragma region ClearQueueStarvation
	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with queue starvation already cleared, matching type, and logging disabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_ClearMatchTest)
	{
		// Data.
		const string logIdentifier;
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(NO_QUEUE_STARVATION);
		const MessagePersistType messagePersistType1(PERSIST_NONE);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(messagePersistType1);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with queue starvation already cleared, non-matching type, and logging disabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_ClearNonMatchTest)
	{
		// Data.
		const string logIdentifier;
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(NO_QUEUE_STARVATION);
		const MessagePersistType messagePersistType1(PERSIST_NONE);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(PERSIST_EMPLOYEE_WINDOW);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with non-default starvation priority, starvation type already cleared, matching type, and logging disabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_PriorityMatchTest)
	{
		// Data.
		const string logIdentifier;
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(QUEUE_PRIORITY_LOW);
		const MessagePersistType messagePersistType1(PERSIST_NONE);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(messagePersistType1);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		queueList.SetQueueStarvation(starveThresholdPriority1);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled());
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with non-default starvation priority, starvation type already cleared, non-matching type, and logging disabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_PriorityNonMatchTest)
	{
		// Data.
		const string logIdentifier;
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(QUEUE_PRIORITY_LOW);
		const MessagePersistType messagePersistType1(PERSIST_NONE);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(PERSIST_EMPLOYEE_WINDOW);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		queueList.SetQueueStarvation(starveThresholdPriority1);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled());
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with starvation priority already cleared and non-default starvation type, matching type, and logging disabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_TypeMatchTest)
	{
		// Data.
		const string logIdentifier;
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(NO_QUEUE_STARVATION);
		const MessagePersistType messagePersistType1(PERSIST_JACKPOTHIT);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(messagePersistType1);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		CMessagePersist *msgPersist1(new CMessagePersist(messagePersistType1));
		queueList.SetQueueStarvation(starveThresholdPriority1, msgPersist1);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled());
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with starvation priority already cleared and non-default starvation type, non-matching type, and logging disabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_TypeNonMatchTest)
	{
		// Data.
		const string logIdentifier;
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(NO_QUEUE_STARVATION);
		const MessagePersistType messagePersistType1(PERSIST_JACKPOTHIT);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(PERSIST_EMPLOYEE_WINDOW);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		CMessagePersist *msgPersist1(new CMessagePersist(messagePersistType1));
		queueList.SetQueueStarvation(starveThresholdPriority1, msgPersist1);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with non-default starvation priority and non-default starvation type, matching type, and logging disabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_PriorityTypeMatchTest)
	{
		// Data.
		const string logIdentifier;
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(QUEUE_PRIORITY_LOW);
		const MessagePersistType messagePersistType1(PERSIST_JACKPOTHIT);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(messagePersistType1);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		CMessagePersist *msgPersist1(new CMessagePersist(messagePersistType1));
		queueList.SetQueueStarvation(starveThresholdPriority1, msgPersist1);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled());
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with non-default starvation priority and non-default starvation type, non-matching type, and logging disabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_PriorityTypeNonMatchTest)
	{
		// Data.
		const string logIdentifier;
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(QUEUE_PRIORITY_LOW);
		const MessagePersistType messagePersistType1(PERSIST_JACKPOTHIT);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(PERSIST_EMPLOYEE_WINDOW);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		CMessagePersist *msgPersist1(new CMessagePersist(messagePersistType1));
		queueList.SetQueueStarvation(starveThresholdPriority1, msgPersist1);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with queue starvation already cleared, matching type, and logging enabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_ClearMatchLogTest)
	{
		// Data.
		const string logIdentifier = "abcde";
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(NO_QUEUE_STARVATION);
		const MessagePersistType messagePersistType1(PERSIST_NONE);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(messagePersistType1);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with queue starvation already cleared, non-matching type, and logging enabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_ClearNonMatchLogTest)
	{
		// Data.
		const string logIdentifier = "abcde";
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(NO_QUEUE_STARVATION);
		const MessagePersistType messagePersistType1(PERSIST_NONE);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(PERSIST_EMPLOYEE_WINDOW);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with non-default starvation priority, starvation type already cleared, matching type, and logging enabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_PriorityMatchLogTest)
	{
		// Data.
		const string logIdentifier = "abcde";
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(QUEUE_PRIORITY_LOW);
		const MessagePersistType messagePersistType1(PERSIST_NONE);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(messagePersistType1);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		queueList.SetQueueStarvation(starveThresholdPriority1);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled());
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with non-default starvation priority, starvation type already cleared, non-matching type, and logging enabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_PriorityNonMatchLogTest)
	{
		// Data.
		const string logIdentifier = "abcde";
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(QUEUE_PRIORITY_LOW);
		const MessagePersistType messagePersistType1(PERSIST_NONE);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(PERSIST_EMPLOYEE_WINDOW);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		queueList.SetQueueStarvation(starveThresholdPriority1);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled());
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with starvation priority already cleared and non-default starvation type, matching type, and logging enabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_TypeMatchLogTest)
	{
		// Data.
		const string logIdentifier = "abcde";
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(NO_QUEUE_STARVATION);
		const MessagePersistType messagePersistType1(PERSIST_JACKPOTHIT);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(messagePersistType1);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		CMessagePersist *msgPersist1(new CMessagePersist(messagePersistType1));
		queueList.SetQueueStarvation(starveThresholdPriority1, msgPersist1);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled());
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with starvation priority already cleared and non-default starvation type, non-matching type, and logging enabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_TypeNonMatchLogTest)
	{
		// Data.
		const string logIdentifier = "abcde";
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(NO_QUEUE_STARVATION);
		const MessagePersistType messagePersistType1(PERSIST_JACKPOTHIT);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(PERSIST_EMPLOYEE_WINDOW);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		CMessagePersist *msgPersist1(new CMessagePersist(messagePersistType1));
		queueList.SetQueueStarvation(starveThresholdPriority1, msgPersist1);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with non-default starvation priority and non-default starvation type, matching type, and logging enabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_PriorityTypeMatchLogTest)
	{
		// Data.
		const string logIdentifier = "abcde";
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(QUEUE_PRIORITY_LOW);
		const MessagePersistType messagePersistType1(PERSIST_JACKPOTHIT);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(messagePersistType1);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		CMessagePersist *msgPersist1(new CMessagePersist(messagePersistType1));
		queueList.SetQueueStarvation(starveThresholdPriority1, msgPersist1);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled());
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with non-default starvation priority and non-default starvation type, non-matching type, and logging enabled.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_PriorityTypeNonMatchLogTest)
	{
		// Data.
		const string logIdentifier = "abcde";
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(QUEUE_PRIORITY_LOW);
		const MessagePersistType messagePersistType1(PERSIST_JACKPOTHIT);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(PERSIST_EMPLOYEE_WINDOW);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		CMessagePersist *msgPersist1(new CMessagePersist(messagePersistType1));
		queueList.SetQueueStarvation(starveThresholdPriority1, msgPersist1);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
	}
// #pragma endregion ClearQueueStarvation

// #pragma region ClearQueueStarvation_EventHandle
	/// <summary>
	/// Functional test for:
	///     CQueueList::ClearQueueStarvation()
	/// Test with queue starvation with an event handle.
	/// </summary>
	TEST(QueueListUnitTests, ClearQueueStarvation_EventHandleTest)
	{
		// Data.
		const string logIdentifier;
		const bool loggingEnabled(!logIdentifier.empty());
		const int starveThresholdPriority1(NO_QUEUE_STARVATION);
		const MessagePersistType messagePersistType1(PERSIST_NONE);
		const int starveThresholdPriority(NO_QUEUE_STARVATION);
		const MessagePersistType type(messagePersistType1);
		const MessagePersistType typeExpected(PERSIST_NONE);

		// Setup.
		CQueueList queueList(logIdentifier);
		ASSERT_EQ(loggingEnabled, queueList.LoggingEnabled()); // Verify setup.
		ASSERT_EQ(starveThresholdPriority1, queueList.m_starveThresholdPriority);
		ASSERT_EQ(messagePersistType1, queueList.m_messagePersistType);
		//TODO: ASSERT_EQ((DWORD)WAIT_TIMEOUT, WaitForSingleObject(queueList.GetAddedItemEventHandle(), 0), L"WaitForSingleObject()");

		// Perform operation(s) to be tested.
		queueList.ClearQueueStarvation(type);
		ASSERT_EQ(starveThresholdPriority, queueList.m_starveThresholdPriority);
		ASSERT_EQ(typeExpected, queueList.m_messagePersistType);
		//TODO: ASSERT_EQ((DWORD)WAIT_OBJECT_0, WaitForSingleObject(queueList.GetAddedItemEventHandle(), 0), L"WaitForSingleObject()");
	}
// #pragma endregion ClearQueueStarvation_EventHandle

// #pragma region IsQueueStarved
	/// <summary>
	/// Functional test for:
	///     CQueueList::CQueueList()
	///     CQueueList::IsQueueStarved()
	/// Test with default value(s).
	/// </summary>
	TEST(QueueListUnitTests, IsQueueStarved_DefaultTest)
	{
		// Setup.
		CQueueList queueList;
		bool queueStarvationIsCleared(false);

		// Perform operation(s) to be tested.
		ASSERT_FALSE(queueList.IsQueueStarved(PERSIST_NONE, queueStarvationIsCleared));
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::IsQueueStarved()
	/// Test with matching non-default value for msgPersist.
	/// </summary>
	TEST(QueueListUnitTests, IsQueueStarved_MatchTest)
	{
		// Data.
		const int starveThresholdPriority(QUEUE_PRIORITY_HIGH);
		const MessagePersistType messagePersistType(PERSIST_JACKPOTHIT);

		// Setup.
		CQueueList queueList;
		CMessagePersist *msgPersist(new CMessagePersist(messagePersistType));
		queueList.SetQueueStarvation(starveThresholdPriority, msgPersist);

		// Perform operation(s) to be tested.
        bool queueStarvationIsCleared(false);
		ASSERT_FALSE(queueList.IsQueueStarved(messagePersistType, queueStarvationIsCleared));
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::IsQueueStarved()
	/// Test with non-default value for msgPersist, using IsQueueStarved() default parameter.
	/// </summary>
	TEST(QueueListUnitTests, IsQueueStarved_MatchDefaultTest)
	{
		// Data.
		const int starveThresholdPriority(QUEUE_PRIORITY_HIGH);
		const MessagePersistType messagePersistType(PERSIST_JACKPOTHIT);

		// Setup.
		CQueueList queueList;
		CMessagePersist *msgPersist(new CMessagePersist(messagePersistType));
		queueList.SetQueueStarvation(starveThresholdPriority, msgPersist);

		// Perform operation(s) to be tested.
        bool queueStarvationIsCleared(false);
		ASSERT_FALSE(queueList.IsQueueStarved(PERSIST_NONE, queueStarvationIsCleared));
	}
// #pragma endregion IsQueueStarved

// #pragma region IsQueueStarvedEx
	/// <summary>
	/// Functional test for:
	///     CQueueList::CQueueList()
	///     CQueueList::IsQueueStarvedEx()
	/// Test with default value(s).
	/// </summary>
	TEST(QueueListUnitTests, IsQueueStarvedEx_DefaultTest)
	{
		// Setup.
		CQueueList queueList;

		// Perform operation(s) to be tested.
		ASSERT_FALSE(queueList.IsQueueStarvedEx());
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::CQueueList()
	///     CQueueList::IsQueueStarvedEx()
	/// Test with PERSIST_NONE value(s).
	/// </summary>
	TEST(QueueListUnitTests, IsQueueStarvedEx_NoneTest)
	{
		// Data.
		const int starveThresholdPriority(QUEUE_PRIORITY_HIGH);
		const MessagePersistType messagePersistType(PERSIST_NONE);

		// Setup.
		CQueueList queueList;
		CMessagePersist *msgPersist(new CMessagePersist(messagePersistType));
		queueList.SetQueueStarvation(starveThresholdPriority, msgPersist);

		// Perform operation(s) to be tested.
		ASSERT_FALSE(queueList.IsQueueStarvedEx(messagePersistType));
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::IsQueueStarvedEx()
	/// Test with matching non-default value for msgPersist.
	/// </summary>
	TEST(QueueListUnitTests, IsQueueStarvedEx_MatchTest)
	{
		// Data.
		const int starveThresholdPriority(QUEUE_PRIORITY_HIGH);
		const MessagePersistType messagePersistType(PERSIST_JACKPOTHIT);

		// Setup.
		CQueueList queueList;
		CMessagePersist *msgPersist(new CMessagePersist(messagePersistType));
		queueList.SetQueueStarvation(starveThresholdPriority, msgPersist);

		// Perform operation(s) to be tested.
		ASSERT_EQ(true, queueList.IsQueueStarvedEx(messagePersistType));
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::IsQueueStarvedEx()
	/// Test with non-matching non-default value for msgPersist.
	/// </summary>
	TEST(QueueListUnitTests, IsQueueStarvedEx_NonMatchTest)
	{
		// Data.
		const int starveThresholdPriority(QUEUE_PRIORITY_HIGH);
		const MessagePersistType messagePersistType(PERSIST_JACKPOTHIT);

		// Setup.
		CQueueList queueList;
		CMessagePersist *msgPersist(new CMessagePersist(messagePersistType));
		queueList.SetQueueStarvation(starveThresholdPriority, msgPersist);

		// Perform operation(s) to be tested.
		ASSERT_FALSE(queueList.IsQueueStarvedEx(PERSIST_EMPLOYEE_WINDOW));
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::IsQueueStarvedEx()
	/// Test with non-matching non-default value for msgPersist, using IsQueueStarvedEx() default parameter.
	/// </summary>
	TEST(QueueListUnitTests, IsQueueStarvedEx_NonMatchDefaultTest)
	{
		// Data.
		const int starveThresholdPriority(QUEUE_PRIORITY_HIGH);
		const MessagePersistType messagePersistType(PERSIST_JACKPOTHIT);

		// Setup.
		CQueueList queueList;
		CMessagePersist *msgPersist(new CMessagePersist(messagePersistType));
		queueList.SetQueueStarvation(starveThresholdPriority, msgPersist);

		// Perform operation(s) to be tested.
		ASSERT_FALSE(queueList.IsQueueStarvedEx());
	}
// #pragma endregion IsQueueStarvedEx

// #pragma region IsPersistTypePresent
	/// <summary>
	/// Functional test for:
	///     CQueueList::IsPersistTypePresent()
	/// Test with default value(s).
	/// </summary>
	TEST(QueueListUnitTests, IsPersistTypePresent_DefaultTest)
	{
		// Setup.
		CQueueList queueList;

		// Perform operation(s) to be tested.
		ASSERT_FALSE(queueList.IsPersistTypePresent(PERSIST_NONE));
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::IsPersistTypePresentEx()
	/// Test with a matching persist type.
	/// </summary>
	TEST(QueueListUnitTests, IsPersistTypePresent_MatchTest)
	{
		// Data.
		const MessagePersistType messagePersistType2(PERSIST_JACKPOTHIT);

		// Setup.
		CQueueList queueList;

		CMessagePersist *msgPersist2(new CMessagePersist(messagePersistType2));
		CInternalMessage *msg2(new CInternalMessage(INT_MSG_JACKPOT_HIT, msgPersist2));
		queueList.Add(msg2, QUEUE_PRIORITY_ABOVE_NORMAL - 1);

		// Perform operation(s) to be tested.
		ASSERT_FALSE(queueList.IsPersistTypePresent(messagePersistType2));
	}
// #pragma endregion IsPersistTypePresent

// #pragma region IsPersistTypePresentEx
	/// <summary>
	/// Functional test for:
	///     CQueueList::IsPersistTypePresentEx()
	/// Test with default value(s).
	/// </summary>
	TEST(QueueListUnitTests, IsPersistTypePresentEx_DefaultTest)
	{
		// Setup.
		CQueueList queueList;

		// Perform operation(s) to be tested.
		ASSERT_FALSE(queueList.IsPersistTypePresentEx(PERSIST_NONE));
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::IsPersistTypePresentEx()
	/// Test with a matching persist type.
	/// </summary>
	TEST(QueueListUnitTests, IsPersistTypePresentEx_MatchTest)
	{
		// Data.
		const MessagePersistType messagePersistType2(PERSIST_JACKPOTHIT);

		// Setup.
		CQueueList queueList;

		CMessagePersist *msgPersist2(new CMessagePersist(messagePersistType2));
		CInternalMessage *msg2(new CInternalMessage(INT_MSG_JACKPOT_HIT, msgPersist2));
		queueList.Add(msg2, QUEUE_PRIORITY_ABOVE_NORMAL);

		// Perform operation(s) to be tested.
		ASSERT_EQ(true, queueList.IsPersistTypePresentEx(messagePersistType2));
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::IsPersistTypePresentEx()
	/// Try to test the code path where CSubQueue::Peek() returns a nullptr CQueueItem and bItemsInQueue=true.
	/// Also see Defect 128186:nCompass: UI displays can be cleared before they should be when persisted message types are not correctly detected.
	/// </summary>
	TEST(QueueListUnitTests, IsPersistTypePresentEx_MatchUsageBeforeTest)
	{
		// Data.
		const bool isPresent(false); // Should be true after fixing Defect 128186.
		const MessagePersistType messagePersistType2(PERSIST_JACKPOTHIT);

		// Setup.
		CQueueList queueList;
		queueList.AddPriority(QUEUE_PRIORITY_ABOVE_NORMAL, 1); // Force CSubQueue::m_priorityRatio to a known value (of 1).

		queueList.Add(new CInternalMessage(INT_MSG_SET_DENOM), QUEUE_PRIORITY_ABOVE_NORMAL);

		// Force value of CSubQueue::m_priorityRatio to be equal to the value of CSubQueue::m_priorityRatio.
		queueList.Remove(); // This can be done before or after adding the persisted type.

		queueList.Add(new CInternalMessage(INT_MSG_JACKPOT_HIT, new CMessagePersist(messagePersistType2)),
			QUEUE_PRIORITY_ABOVE_NORMAL);

		// Perform operation(s) to be tested.
		ASSERT_EQ(isPresent, queueList.IsPersistTypePresentEx(messagePersistType2));
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::IsPersistTypePresentEx()
	/// Try to test the code path where CSubQueue::Peek() returns a nullptr CQueueItem and bItemsInQueue=true.
	/// Also see Defect 128186:nCompass: UI displays can be cleared before they should be when persisted message types are not correctly detected.
	/// </summary>
	TEST(QueueListUnitTests, IsPersistTypePresentEx_MatchUsageAfterTest)
	{
		// Data.
		const bool isPresent(false); // Should be true after fixing Defect 128186.
		const MessagePersistType messagePersistType2(PERSIST_JACKPOTHIT);

		// Setup.
		CQueueList queueList;
		queueList.AddPriority(QUEUE_PRIORITY_ABOVE_NORMAL, 1); // Force CSubQueue::m_priorityRatio to a known value (of 1).

		queueList.Add(new CInternalMessage(INT_MSG_SET_DENOM), QUEUE_PRIORITY_ABOVE_NORMAL);

		queueList.Add(new CInternalMessage(INT_MSG_JACKPOT_HIT, new CMessagePersist(messagePersistType2)),
			QUEUE_PRIORITY_ABOVE_NORMAL);

		// Force value of CSubQueue::m_priorityRatio to be equal to the value of CSubQueue::m_priorityRatio.
		queueList.Remove(); // This can be done before or after adding the persisted type.

		// Perform operation(s) to be tested.
		ASSERT_EQ(isPresent, queueList.IsPersistTypePresentEx(messagePersistType2));
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::IsPersistTypePresentEx()
	/// Test with a matching persist type when non-matching persist types are present.
	/// </summary>
	TEST(QueueListUnitTests, IsPersistTypePresentEx_MatchNonMatchTest)
	{
		// Data.
		const MessagePersistType messagePersistType1(PERSIST_CONTENT_WINDOW);
		const MessagePersistType messagePersistType2(PERSIST_JACKPOTHIT);
		const MessagePersistType messagePersistType3(PERSIST_EMPLOYEE_WINDOW);

		// Setup.
		CQueueList queueList;
		queueList.Add(new CInternalMessage(INT_MSG_SET_DENOM, new CMessagePersist(messagePersistType1)),
			QUEUE_PRIORITY_HIGH);
		queueList.Add(new CInternalMessage(INT_MSG_JACKPOT_HIT, new CMessagePersist(messagePersistType2)),
			QUEUE_PRIORITY_ABOVE_NORMAL);
		queueList.Add(new CInternalMessage(INT_MSG_EMPLOYEE_PIN_RESULT, new CMessagePersist(messagePersistType3)),
			QUEUE_PRIORITY_NORMAL);

		// Perform operation(s) to be tested.
		ASSERT_EQ(true, queueList.IsPersistTypePresentEx(messagePersistType2));
	}

	/// <summary>
	/// Functional test for:
	///     CQueueList::IsPersistTypePresentEx()
	/// Test when only non-matching persist types are present.
	/// </summary>
	TEST(QueueListUnitTests, IsPersistTypePresentEx_NonMatchTest)
	{
		// Data.
		const MessagePersistType messagePersistType1(PERSIST_CONTENT_WINDOW);
		const MessagePersistType messagePersistType2(PERSIST_JACKPOTHIT);
		const MessagePersistType messagePersistType3(PERSIST_EMPLOYEE_WINDOW);

		// Setup.
		CQueueList queueList;
		queueList.Add(new CInternalMessage(INT_MSG_SET_DENOM, new CMessagePersist(messagePersistType1)),
			QUEUE_PRIORITY_HIGH);
		queueList.Add(new CInternalMessage(INT_MSG_EMPLOYEE_PIN_RESULT, new CMessagePersist(messagePersistType3)),
			QUEUE_PRIORITY_NORMAL);

		// Perform operation(s) to be tested.
		ASSERT_FALSE(queueList.IsPersistTypePresentEx(messagePersistType2));
	}
// #pragma endregion IsPersistTypePresentEx

// #pragma region ResetQueueStarvation
	/// <summary>
	/// Code coverage test for:
	///     CQueueList::ResetQueueStarvation()
	/// Test with default value(s).
	/// </summary>
	TEST(QueueListUnitTests, ResetQueueStarvation_DefaultTest)
	{
		// Setup.
		CQueueList queueList;

		// Perform operation(s) to be tested.
		queueList.ResetQueueStarvation(PERSIST_NONE);
		//TODO: Assert::IsNull(queueList.GetAddedItemEventHandle(), L"GetAddedItemEventHandle()");
	}
// #pragma endregion ResetQueueStarvation
}