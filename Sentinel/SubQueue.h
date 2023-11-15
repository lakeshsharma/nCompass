#pragma once

#include "QueueItem.h"
#include "MessagePersist.h"

namespace SentinelNativeUnitTests { class QueueListAddItemUnitTests; }
namespace SentinelNativeUnitTests { class QueueListUnitTests; }

class CSubQueue
{
public:
	CSubQueue(int priorityLevel, int priorityRatio, int maxSize = 0);
	virtual ~CSubQueue(void);

	bool Add(CQueueItem * item);
	bool IsPriorityLevel(int priorityLevel);
	CQueueItem *Remove(bool &bItemsInQueue);
	CQueueItem *Remove(bool &bItemsInQueue, int &index);
	CQueueItem *Peek(bool &bItemsInQueue, int &index);
	void ClearAll(void);
	void ClearType(MessagePersistType mpType);
	int ComparePriority(int priorityLevel);
	int GetPriorityLevel();

	///<summary>
	/// Finds and removes item from subqueue
	///</summary>
	///<param name="item">Item to remove frrom subqueue</param>
	bool FindAndRemoveItem(CQueueItem *item);

private:
	// This is the count of times in a row we have removed an item from this subqueue.
	int m_usageCount;

	int m_priorityRatio;

	// A lower number is a higher priority queue
	int m_priorityLevel;

	unsigned int m_maxSize;
	std::vector<CQueueItem*> m_queueList;

	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::QueueListAddItemUnitTests;
	friend class SentinelNativeUnitTests::QueueListUnitTests;
};
