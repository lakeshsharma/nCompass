#include "stdafx.h"
#include "SASQueueList.h"
#include "SASPollMessage.h"

CSASQueueList::CSASQueueList(const std::string& logIdentifier) : CQueueList(logIdentifier)
{
}

/// <summary>
/// Adds a queue item to the list.
/// </summary>
/// <param name="item">IN - The queue item.</param>
/// <param name="priorityLevel">IN - The priority level for the queue item.</param>
/// <returns>true if the queue item was added to the list, false otherwise.</returns>
bool CSASQueueList::Add(CSASPollMessage *item, int priorityLevel)
{
   item->SetQueue(this, priorityLevel);
   return CQueueList::Add(item,priorityLevel);
}

bool CSASQueueList::ForceRemoveItem(CSASPollMessage *item, int priorityLevel)
{
   bool itemRemoved = false;
   // Look up item in queueList. If found, remove it.
   lock_guard<recursive_mutex> lock(m_criticalSection);

   // Go through all of the sub queues
	for (int sqi = 0; sqi < (int)m_subQueueArray.size(); sqi++)
	{
		CSubQueue *subQueue(m_subQueueArray[sqi]);

		int priorityDiff = subQueue->ComparePriority(priorityLevel);

		// If the sub queue priority matches, look for item here
		if (priorityDiff == 0)
		{
			itemRemoved = subQueue->FindAndRemoveItem((CQueueItem *)item);
            break;
		}
   }
   return itemRemoved;
}
