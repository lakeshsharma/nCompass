#pragma once

#include "QueueItem.h"
#include "MessagePersist.h"

class CSubQueue
{
public:
	CSubQueue(int priorityLevel, int priorityRatio, int maxSize = -1);
public:
	~CSubQueue(void);


public:
	// Add queue item to the tail of this subqueue
	bool Add(CQueueItem * item);
public:

	// Is this sub queue the specified priority level.
	bool IsPriorityLevel(int priorityLevel)
	{
		return priorityLevel == m_priorityLevel;
	}

public:
	CQueueItem *Remove(bool &bItemsInQueue);
	CQueueItem *Remove(bool &bItemsInQueue, int &index);
	CQueueItem *Peek(bool &bItemsInQueue, int &index);

	void ClearAll( void );
	void ClearType(MessagePersistType mpType);

	// This is the count of times in a row we have removed an item from this subqueue.
	int m_usageCount;
public:

	// Returns the difference of the specified priority to the subqueue priority.
	// A negative return means the the specified priority is less than the subqueue priority
	// A positive return means the the specified priority is greater than the subqueue priority
	// A 0 return means they are equal.
	int ComparePriority( int priorityLevel )
	{
		return ( priorityLevel - m_priorityLevel);
	}
public:
	// Gets the priority level of this subqueue
	int GetPriorityLevel(void)
	{
		return m_priorityLevel;
	}

private:
	int m_priorityRatio;

	// A lower number is a higher priority queue
	int m_priorityLevel;

	int m_maxSize;
	CPtrList m_queueList;

};
