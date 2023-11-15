#pragma once

#include "SubQueue.h"
#include "InternalMessage.h"
#include "MessagePersist.h"
#include "binsem.h"
#include <gtest/gtest_prod.h>

const int USE_DEFAULT_QUEUE_RATIO = -1;

const int QUEUE_PRIORITY_HIGHEST = 1;
const int QUEUE_PRIORITY_HIGH = 2;
const int QUEUE_PRIORITY_ABOVE_NORMAL = 3;
const int QUEUE_PRIORITY_NORMAL = 4;
const int QUEUE_PRIORITY_BELOW_NORMAL = 5;
const int QUEUE_PRIORITY_LOW = 6;
const int QUEUE_PRIORITY_LOWEST = 7;

const int DEFAULT_QP_RATIO_HIGHEST = 100;
const int DEFAULT_QP_RATIO_HIGH = 50;
const int DEFAULT_QP_RATIO_ABOVE_NORMAL = 10;
const int DEFAULT_QP_RATIO_NORMAL = 5;
const int DEFAULT_QP_RATIO_BELOW_NORMAL = 2;
const int DEFAULT_QP_RATIO_LOW = 2;
const int DEFAULT_QP_RATIO_LOWEST = 1;
const int DEFAULT_QP_RATIO_OTHER = 1000;

const int NO_QUEUE_STARVATION = -1;

namespace SentinelNativeUnitTests { class InternalMessageQueueListUnitTests; }
namespace SentinelNativeUnitTests { class QueueListAddItemUnitTests; }
namespace SentinelNativeUnitTests { class QueueListUnitTests; }

class CQueueList
{
public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CQueueList"/> class.
	/// Use this constructor when the item-added event handle is to be created by this class.
	/// </summary>
	/// <param name="createItemAddedEventHandle">IN - true if the item-added event is to be created, false otherwise.</param>
	/// <param name="logIdentifier">IN - The identifier used for logging, nullptr indicates no logging.</param>
	CQueueList(const std::string& logIdentifier = "");

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CQueueList"/> class.
	/// </summary>
	virtual ~CQueueList(void);

	/// <summary>
	/// Add a priority sub queue to the queue list.
	/// </summary>
	/// <param name="priorityLevel">IN - The priority level.</param>
	/// <param name="priorityRatio">IN - The priority ratio.</param>
	void AddPriority(int priorityLevel, int priorityRatio = USE_DEFAULT_QUEUE_RATIO);

	/// <summary>
	/// Adds a queue item to the list.
	/// </summary>
	/// <param name="item">IN - The queue item.</param>
	/// <param name="priorityLevel">IN - The priority level for the queue item.</param>
	/// <returns>true if the queue item was added to the list, false otherwise.</returns>
	virtual bool Add(CQueueItem *item, int priorityLevel);

	/// <summary>
	/// Removes the next appropriate item from the queue.
	/// </summary>
	/// <returns>The item removed if an item was removed, nullptr otherwise.</returns>
	virtual CQueueItem * Remove();

	/// <summary>
	/// Clears the queue starvation.
	/// </summary>
	/// <param name="type">IN - The starvation type (which is ignored).</param>
	virtual void ClearQueueStarvation(MessagePersistType type = PERSIST_NONE);

	/// <summary>
	/// Determines whether the queue is starved for the message persist starvation type.
	/// </summary>
	/// <param name="mpType">IN - the message persist starvation type.</param>
	/// <param name="queueIsCleared">OUT - flag to indicated whether the queue is cleared or not</param>
	/// <returns>false.</returns>
	virtual bool IsQueueStarved(MessagePersistType mpType, bool& queueIsCleared);

	/// <summary>
	/// Determines whether the queue is starved for the message persist starvation type.
	/// </summary>
	/// <param name="mpType">IN - the message persist starvation type.</param>
	/// <returns>true if the queue is starved for the message persist starvation type, false otherwise.</returns>
	virtual bool IsQueueStarvedEx(MessagePersistType mpType = PERSIST_NONE);

	/// <summary>
	/// Gets the added item event handle.
	/// </summary>
	/// <returns>The added item event handle if one exists, NULL otherise</returns>
	binsem& GetAddedItemEventHandle();

	/// <summary>
	/// Sets whether to flush lower priority items when adding queue items.
	/// </summary>
	/// <param name="bFlush">IN - true to flush, false to not flush.</param>
	void FlushLowerPriorityItemsUponAdd(bool bFlush = true);

	/// <summary>
	/// Determines whether a queued item at the head of a sub-queue contains the message persist type.
	/// </summary>
	/// <param name="mpType">IN - the message persist type.</param>
	/// <returns>false.</returns>
	virtual bool IsPersistTypePresent(MessagePersistType mpType = PERSIST_NONE);

	/// <summary>
	/// In general: determines whether a queued item at the head of a sub-queue contains the message persist type.
	/// </summary>
	/// <param name="mpType">IN - the message persist type.</param>
	/// <returns>In general: true is the persist type is present at the head of a sub-queue, false otherwise.</returns>
	/// <remarks>
	/// The caller is responsible for ensuring that queued items are CInternalMessage instances.
	/// Exception: false is returned when the persist type is present at the head of a sub-queue,
	///     if the sub-queue m_usageCount is (greater than or) equal to the sub-queue m_priorityRatio.
	///     See Defect 128186:nCompass: UI displays can be cleared before they should be when persisted message types are not correctly detected.
	/// </remarks>
	virtual bool IsPersistTypePresentEx(MessagePersistType mpType = PERSIST_NONE);

	/// <summary>
	/// NO OP for CQueueList.
	/// </summary>
	/// <param name="mpType">MessagePersistType to reset</param>
	virtual void ResetQueueStarvation(MessagePersistType mpType);

	/// <summary>
	/// Sets the queue starvation.
	/// </summary>
	/// <param name="starveThresholdPriority">IN - The queue starvation threshold priority.</param>
	/// <param name="msgPersist">Contains the message persist type, if nullptr then PERSIST_NONE is used.</param>
	/// <remarks>
	/// msgPersist is deleted by this message.
	/// </remarks>
	virtual void SetQueueStarvation(int starveThresholdPriority, CMessagePersist *msgPersist = NULL);

protected:
	static const LPCTSTR LPCTSTR_ADD;
	static const LPCTSTR LPCTSTR_NOTADDED;
	static const LPCTSTR LPCTSTR_POP;

	static std::recursive_mutex m_criticalSection;

	std::vector<CSubQueue*> m_subQueueArray;
	virtual bool AddItem(CQueueItem * item, int priorityLevel);
	virtual void LogItem(CQueueItem * item, int priorityLevel, const std::string& desc);
	bool LoggingEnabled();

	binsem m_binarySemaphore;

	// Any lower priority sub queues will be starved out.
	// If this is set to -1, no starvation occurs.
	int m_starveThresholdPriority;
	volatile MessagePersistType m_messagePersistType;

	// This is a flag to indicate that when an item is added, flush lower priority items
	bool m_bFlushLowerOnAdd;

	std::string m_logIdentifier;	// Empty string indicates no logging.

	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::InternalMessageQueueListUnitTests;
	friend class SentinelNativeUnitTests::QueueListAddItemUnitTests;
	friend class SentinelNativeUnitTests::QueueListUnitTests;
};
