#include "stdafx.h"
#include "QueueList.h"
#include "Logging/LogString.h"

const LPCTSTR CQueueList::LPCTSTR_ADD(_T("add"));
const LPCTSTR CQueueList::LPCTSTR_NOTADDED(_T("NotAdded"));
const LPCTSTR CQueueList::LPCTSTR_POP(_T("pop"));

using namespace std;

recursive_mutex CQueueList::m_criticalSection;

CQueueList::CQueueList(const string& logIdentifier)
	: m_logIdentifier(logIdentifier)
{
	m_binarySemaphore.wait();

	m_starveThresholdPriority = NO_QUEUE_STARVATION;
	m_messagePersistType = PERSIST_NONE;

	m_bFlushLowerOnAdd = false;

#ifdef DEBUG_MESSAGE_QUEUE
	if (LoggingEnabled())
	{
		LogString(MESSAGES_LOG, _T("%s created"), static_cast<LPCTSTR>(m_logIdentifier.c_str()));
	}
#endif
}

CQueueList::~CQueueList(void)
{
	std::for_each(m_subQueueArray.begin(), m_subQueueArray.end(), std::default_delete<CSubQueue>());
	m_subQueueArray.clear();

#ifdef DEBUG_MESSAGE_QUEUE
	if (LoggingEnabled())
	{
		LogString(MESSAGES_LOG, _T("%s destroyed"), static_cast<LPCTSTR>(m_logIdentifier.c_str()));
	}
#endif
}

// Add a priority sub queue to the queue list.
void CQueueList::AddPriority(int priorityLevel, int priorityRatio)
{
	if (priorityRatio == USE_DEFAULT_QUEUE_RATIO)
	{
		switch (priorityLevel)
		{
		case QUEUE_PRIORITY_HIGHEST:
			priorityRatio = DEFAULT_QP_RATIO_HIGHEST;
			break;
		case QUEUE_PRIORITY_HIGH:
			priorityRatio = DEFAULT_QP_RATIO_HIGH;
			break;
		case QUEUE_PRIORITY_ABOVE_NORMAL:
			priorityRatio = DEFAULT_QP_RATIO_ABOVE_NORMAL;
			break;
		case QUEUE_PRIORITY_NORMAL:
			priorityRatio = DEFAULT_QP_RATIO_NORMAL;
			break;
		case QUEUE_PRIORITY_BELOW_NORMAL:
			priorityRatio = DEFAULT_QP_RATIO_BELOW_NORMAL;
			break;
		case QUEUE_PRIORITY_LOW:
			priorityRatio = DEFAULT_QP_RATIO_LOW;
			break;
		case QUEUE_PRIORITY_LOWEST:
			priorityRatio = DEFAULT_QP_RATIO_LOWEST;
			break;
		default:
			priorityRatio = DEFAULT_QP_RATIO_OTHER;
			break;
		}
	}

	// Make a new sub queue item for with this priority.
	CSubQueue *subQueue = new CSubQueue(priorityLevel, priorityRatio);

	lock_guard<recursive_mutex> lock(m_criticalSection);

	// Insert the new item to the sub queue array.
	// This array is sorted by priority, so make sure it goes to the proper 
	//  location in the array.
	bool subQueueAdded = false;
	for (auto iterQueue = m_subQueueArray.begin(); iterQueue != m_subQueueArray.end(); ++iterQueue)
	{
		if ((*iterQueue)->ComparePriority(priorityLevel) < 0)
		{
			m_subQueueArray.insert(iterQueue, subQueue);
			subQueueAdded = true;
			break;
		}
	}

	// If it was not inserted in the array, put it at the end.
	if (!subQueueAdded)
	{
		m_subQueueArray.push_back(subQueue);
	}
}

// Adds the queue item to the list
bool CQueueList::Add(CQueueItem *item, int priorityLevel)
{
	bool itemAdded = false;

	if (item != nullptr)
	{
		lock_guard<recursive_mutex> lock(m_criticalSection);

		itemAdded = AddItem(item, priorityLevel);

		// If we could not add this item, create a new sub queue and try again
		if (!itemAdded)
		{
			AddPriority(priorityLevel);
			itemAdded = AddItem(item, priorityLevel);
		}

		// 2015-12-31 "itemAdded" is always true, because sub-queues are created with unlimited maximum size.
		LogItem(item, priorityLevel, itemAdded ? LPCTSTR_ADD : LPCTSTR_NOTADDED);
	}

	return itemAdded;
}

// Removed the next appropriate item from the queue
CQueueItem* CQueueList::Remove()
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	CQueueItem* item(nullptr);
	int priorityLevel(-1);

	// Iterate thru the sub queues until we have successfully removed an item.
	// The sub queue array is sorted in order of priority
	unsigned int sqi = 0;
	bool bCycleAgain = false;
	while (item == nullptr && sqi < m_subQueueArray.size())
	{
		bool removeFromSubQueue = true;

		// See if we are starving this  sub-queue.
		if (m_starveThresholdPriority != NO_QUEUE_STARVATION && 
			m_subQueueArray[sqi]->GetPriorityLevel() > m_starveThresholdPriority)
		{
			removeFromSubQueue = false;
		}

		// Try to remove an item for this sub-queue
		if (removeFromSubQueue)
		{
			// Remove an item from this sub-queue. 
			// If no item can be removed, the item will be null.
			bool bItemInQueue = false;
			item = m_subQueueArray[sqi]->Remove(bItemInQueue);
			priorityLevel = m_subQueueArray[sqi]->GetPriorityLevel();

			// If the item is nullptr and there is still item(s) in this sub-queue, that means we are surrendering to 
			// lower priority sub-queues. Set a flag to go back into this sub-queue if no items are ever removed
			// from lower priority queues.
			if (item == nullptr && bItemInQueue)
			{
				bCycleAgain = true;
			}
		}

		// If we have iterated thru all queues and we are set to cycle thru again, reset the sub-queue index.
		if (++sqi == m_subQueueArray.size() && bCycleAgain)
		{
			sqi = 0;
			bCycleAgain = false;
		}

		CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
	}

	LogItem(item, priorityLevel, _T("pop"));
	return item;
}

void CQueueList::ClearQueueStarvation(MessagePersistType type)
{
	int oldStarveThresholdPriority(m_starveThresholdPriority);
	int oldMessagePersistType(m_messagePersistType);

	m_messagePersistType = PERSIST_NONE;
	m_starveThresholdPriority = NO_QUEUE_STARVATION;

#ifdef DEBUG_MESSAGE_QUEUE
	if (LoggingEnabled())
	{
		LogString(MESSAGES_LOG, _T("%s clear starvation(type=%d) priority=%d type=%d oldPriority=%d oldType=%d"),
			m_logIdentifier.c_str(),
			(int)type,
			(int)m_starveThresholdPriority,
			(int)m_messagePersistType,
			(int)oldStarveThresholdPriority,
			(int)oldMessagePersistType);
	}
#endif

	// Set the event just in case there are items stuck in the queue
	m_binarySemaphore.signal(false);
}

void CQueueList::SetQueueStarvation(int starveThresholdPriority, CMessagePersist *msgPersist)
{
	int oldStarveThresholdPriority(m_starveThresholdPriority);
	int oldMessagePersistType(m_messagePersistType);

	m_starveThresholdPriority = starveThresholdPriority;
	if (msgPersist != nullptr)
	{
		m_messagePersistType = msgPersist->GetPersistType();
		delete msgPersist;
	}
	else
	{
		m_messagePersistType = PERSIST_NONE;
	}

#ifdef DEBUG_MESSAGE_QUEUE
	if (LoggingEnabled())
	{
		LogString(MESSAGES_LOG, _T("%s set starvation priority=%d type=%d oldPriority=%d oldType=%d"),
			m_logIdentifier.c_str(),
			(int)m_starveThresholdPriority,
			(int)m_messagePersistType,
			(int)oldStarveThresholdPriority,
			(int)oldMessagePersistType);
	}
#endif
}

bool CQueueList::IsQueueStarved(MessagePersistType mpType, bool& queueIsCleared)
{
    UNREFERENCED_PARAMETER(&mpType); 
	queueIsCleared = false;

	// This function is only here to give a function to override in inherited class
	return false;
}

bool CQueueList::IsQueueStarvedEx(MessagePersistType mpType)
{
	bool retval = false;

	if (mpType != PERSIST_NONE)
	{
		if (m_messagePersistType == mpType)
		{
			retval = true;
		}
	}

	return retval;
}

binsem& CQueueList::GetAddedItemEventHandle()
{
	return m_binarySemaphore;
}

void CQueueList::FlushLowerPriorityItemsUponAdd(bool bFlush)
{
	m_bFlushLowerOnAdd = bFlush;
}

bool CQueueList::AddItem(CQueueItem* item, int priorityLevel)
{
	bool itemAdded = false;

	lock_guard<recursive_mutex> lock(m_criticalSection);

	bool bFlushLowerOnAdd(m_bFlushLowerOnAdd && item->OkToFlushLowerOnAdd());

	// Go through all of the sub queues
	for (int sqi = 0; sqi < (int)m_subQueueArray.size() && (!itemAdded || bFlushLowerOnAdd); sqi++)
	{
		CSubQueue *subQueue(m_subQueueArray[sqi]);

		int priorityDiff = subQueue->ComparePriority(priorityLevel);

		// If the sub queue priority matches, add it to this sub queue.
		if (priorityDiff == 0)
		{
			itemAdded = subQueue->Add(item);
		}

		// If we are flushing messages of a lower priority and this sub queue has a lower priority,
		//  clear all the messages in this sub queue
		// NOTE: HIGHEST priority messages will not flush the queue,
		else if (bFlushLowerOnAdd && priorityDiff < 0 && priorityLevel != QUEUE_PRIORITY_HIGHEST)
		{
#ifdef DEBUG_MESSAGE_QUEUE
			if (LoggingEnabled())
			{
				LogString(MESSAGES_LOG, _T("%s pri=%d subQueue->ClearAll()"), m_logIdentifier.c_str(), (int)priorityLevel);
			}
#endif
			subQueue->ClearAll();
		}
	}

	if (itemAdded)
	{
		m_binarySemaphore.signal(false);
	}

	return itemAdded;
}

void CQueueList::LogItem(CQueueItem *item, int priorityLevel, const string& desc)
{
#ifdef DEBUG_MESSAGE_QUEUE
	UNREFERENCED_PARAMETER(priorityLevel);
	if (item != nullptr && LoggingEnabled())
	{
		CInternalMessage *msg(dynamic_cast<CInternalMessage *>(item));
		if (msg != nullptr)
		{

            // The follow log below was shortened. 
			// The previous log was heavily formatted and was adding 1% to everall Sentinel CPU usage.            
			LogString(MESSAGES_LOG, "%s %s %3d",m_logIdentifier.c_str(), desc.c_str(), (int)msg->GetMessageType());
		}
	}
#else
	UNREFERENCED_PARAMETER(item);
	UNREFERENCED_PARAMETER(priorityLevel);
	UNREFERENCED_PARAMETER(desc);
#endif
}

bool CQueueList::LoggingEnabled()
{
	return !m_logIdentifier.empty();
}

bool CQueueList::IsPersistTypePresent(MessagePersistType)
{
	// No persist types in CQueueList, just return false;
	return false;
}

// CQueueList::IsPersistTypePresent
// Caller ensures that queued items are CInternalMessage instances.
bool CQueueList::IsPersistTypePresentEx(MessagePersistType mpType)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	// Search through sub queues looking for this persist type.
	bool bFound = false;
	CInternalMessage *item = nullptr;
	int sqi = 0;

	while (!bFound && sqi < (int)m_subQueueArray.size())
	{
		// Peek at item from this sub-queue. 
		// If no item found, the item will be null.
		// 
		bool bItemInQueue = false;
		int sqItemIndex = 0;
		do {
			// peek at messages to find persist type, stop when the first one is found
			item = (CInternalMessage *)m_subQueueArray[sqi]->Peek(bItemInQueue, sqItemIndex);
			if (item)
			{
				// got an item.  Determine if this item is related to a persistence area
				CMessagePersist *mp = (CMessagePersist *)((CInternalMessage *)item)->GetContent(CMessagePersist::MsgContentType());
				if (mp)
				{
					// MessagePersist object provided
					if (mp->GetPersistType() == mpType)
					{
						bFound = true;
					}
				}
			}
			sqItemIndex++;
		} while (!bFound && !item && bItemInQueue);
		sqi++;
	}
	return bFound;
}

void CQueueList::ResetQueueStarvation(MessagePersistType)
{
	// Does nothing in CQueueList, just return;
}
