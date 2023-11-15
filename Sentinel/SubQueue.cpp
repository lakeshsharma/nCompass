#include "stdafx.h"
#include "SubQueue.h"
#include "InternalMessage.h"
#include <algorithm>

CSubQueue::CSubQueue(int priorityLevel, int priorityRatio, int maxSize)
: m_usageCount(0)
, m_priorityRatio(priorityRatio)
, m_priorityLevel(priorityLevel)
, m_maxSize(maxSize)
{
}

CSubQueue::~CSubQueue(void)
{
	ClearAll();
}

// Add queue item to the tail of this subqueue
bool CSubQueue::Add(CQueueItem * item)
{
	bool retVal = false;

	item->SetPriority(m_priorityLevel);

	if (m_maxSize < 1 || m_queueList.size() < m_maxSize)
	{
		m_queueList.push_back(item);
		retVal = true;
	}

	return retVal;
}

bool CSubQueue::IsPriorityLevel(int priorityLevel)
{
	return priorityLevel == m_priorityLevel;
}

CQueueItem * CSubQueue::Remove(bool &bItemsInQueue)
{
	CQueueItem *item(nullptr);

	// Remove the next item at the head of the list if the list is not empty
	//  and we have not exceeded our usage count
	if (!m_queueList.empty() && m_usageCount < m_priorityRatio)
	{
		auto itemIter(m_queueList.begin());
		item = *itemIter;
		m_queueList.erase(itemIter);
		m_usageCount++;
	}
	else
	{
		m_usageCount = 0;
	}

	bItemsInQueue = !m_queueList.empty();

	return item;
}

CQueueItem *CSubQueue::Remove(bool &bItemsInQueue, int &index)
{
	CQueueItem *item(nullptr);
	unsigned int queueListSize(m_queueList.size());

	if (!m_queueList.empty() && m_usageCount < m_priorityRatio)
	{
		if (index < (int)queueListSize)
		{
			item = m_queueList.at(index);
			m_queueList.erase(m_queueList.begin() + index);
			queueListSize--;
			m_usageCount++;
		}
	}
	else
	{
		m_usageCount = 0;
	}

	bItemsInQueue = index < (int)queueListSize ? true : false;

	return item;
}

CQueueItem *CSubQueue::Peek(bool &bItemsInQueue, int &index)
{
	CQueueItem *item(nullptr);

	// Get the item at the head of the list if the list is not empty
	//  and we have not exceeded our usage count
	if (!m_queueList.empty() && m_usageCount < m_priorityRatio)
	{
		if (index < (int)m_queueList.size())
		{
			item = m_queueList.at(index);
		}
	}
	else
	{
		m_usageCount = 0;
	}

	bItemsInQueue = index < (int)m_queueList.size() ? true : false;

	return item;
}

void CSubQueue::ClearAll(void)
{
	std::for_each(m_queueList.begin(), m_queueList.end(), std::default_delete<CQueueItem>());
	m_queueList.clear();
}

void CSubQueue::ClearType(MessagePersistType mpType)
{
	// Clear all messages from subqueue of mpType
	int sqIndex = 0;

	CQueueItem *item(nullptr);
	while (sqIndex >= 0 && sqIndex < (int)m_queueList.size())
	{
		item = m_queueList.at(sqIndex);
		CMessagePersist *mp = (CMessagePersist *)((CInternalMessage *)item)->GetContent(CMessagePersist::MsgContentType());
		if (mp && mp->GetPersistType() == mpType)
		{
			m_queueList.erase(m_queueList.begin() + sqIndex);
			delete item;
			sqIndex--; // removed item at this index, so try it again.
		}
	}
}

// Returns the difference of the specified priority to the subqueue priority.
// A negative return means the the specified priority is less than the subqueue priority
// A positive return means the the specified priority is greater than the subqueue priority
// A 0 return means they are equal.
int CSubQueue::ComparePriority(int priorityLevel)
{
	return (priorityLevel - m_priorityLevel);
}

// Gets the priority level of this subqueue
int CSubQueue::GetPriorityLevel(void)
{
	return m_priorityLevel;
}

bool CSubQueue::FindAndRemoveItem(CQueueItem *item)
{
   bool itemRemoved = false;
   auto itemIterator = find(m_queueList.begin(), m_queueList.end(), item);
   if (itemIterator != m_queueList.end()) 
   {
      m_queueList.erase(itemIterator);
	  itemRemoved = true;
   }
   return itemRemoved;
}
