#include "stdafx.h"
#include "InternalMessageQueueList.h"
#include "Logging/LogString.h"

using namespace std;

CInternalMessageQueueList::CInternalMessageQueueList( const string& logIdentifier )
	: CQueueList(logIdentifier)
{
	Init();
}

CInternalMessageQueueList::~CInternalMessageQueueList(void)
{
}

void CInternalMessageQueueList::Init()
{
	m_lastMessageStarvedQueue = INT_MSG_NONE;
	m_bFlushLowerOnAdd = false;
	m_equalPriorityInterrupt = false;

	// initialize array to null pointers
	for (int i = 0; i < PERSIST_MAX; i++)
	{
		m_messagePersist[i] = nullptr;
	}
}

// Removed the next appropriate item from the queue
CInternalMessage *CInternalMessageQueueList::Remove(void)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	CInternalMessage *item(nullptr);
	int priorityLevel(-1);

	try
	{

		// Iterate thru the sub queues until we have successfully removed an item.
		// The sub queue array is sorted in order of priority
		unsigned int sqi = 0;
		bool bCycleAgain = false;
		while (item == nullptr && sqi < m_subQueueArray.size())
		{
			bool removeFromSubQueue = true;

			// Try to remove an item for this sub-queue
			if (removeFromSubQueue)
			{
				// Remove an item from this sub-queue. 
				// If no item can be removed, the item will be null.
				//
				bool bItemInQueue = false;
				int sqItemIndex = 0;
				do {
					item = (CInternalMessage *)m_subQueueArray[sqi]->Peek(bItemInQueue, sqItemIndex);
					if (item)
					{
						if (!item->IsTTLExpired())
						{
							// got an item.  Determine if this item is related to a "frozen" area
							CMessagePersist *mp(item->GetMessagePersist());
							if (mp)
							{
								// MessagePersist object provided
								CMessagePersist *existingMP = m_messagePersist[mp->GetPersistType()];
								if (existingMP)
								{
									// Area currently frozen, is it time to unfreeze?
									// Also only use equalpriorityinterrupt if persist type is PERSIST_CONTENT_WINDOW
									if (existingMP->IsQueueStarved(m_subQueueArray[sqi]->GetPriorityLevel(), m_equalPriorityInterrupt && mp->GetPersistType() == PERSIST_CONTENT_WINDOW))
									{
										// this message should be frozen still
										item = nullptr;
									}
									else
									{
										// No longer starved
										delete existingMP;
										m_messagePersist[mp->GetPersistType()] = nullptr;
										if (LoggingEnabled())
										{
											LogString(m_logIdentifier, "Queue no longer starved for persist type %d", 
												mp->GetPersistType());
										}
									}
								}

								// Start a new starvation now?
								if (item && mp->isNewStarvation())
								{
									m_lastMessageStarvedQueue = item->GetMessageType();
									// Disassociate CMessagePersist mp from item
									item->SetMessagePersist(nullptr);

									// Start starvation
									mp->StartClock();

									m_messagePersist[mp->GetPersistType()] = mp;
									if (LoggingEnabled())
									{
										LogString(m_logIdentifier, "Queue starved for persist type %d. %s", mp->GetPersistType(), mp->GetPriorityLogString().c_str());
									}
								}

								if (nullptr != item && LoggingEnabled())
								{					
									LogString(m_logIdentifier, "Message %s of persist type %d taken off queue.", InternalMsgTypeStr[item->GetMessageType()], mp->GetPersistType());
								}
							}
							else
							{
								// if no MP item, decide if subqueue is being starved in general
								// when all code is converted, this shouldn't be necessary anymore
								if (m_starveThresholdPriority != NO_QUEUE_STARVATION &&
									m_subQueueArray[sqi]->GetPriorityLevel() > m_starveThresholdPriority)
								{
									// subqueue is starved
									item = nullptr;
								}
							}
						}
						else
						{
							// Message has expired, remove/delete it.
							if (LoggingEnabled())
							{
								LogString(m_logIdentifier, "Removing expired message from queue, type = %s", InternalMsgTypeStr[item->GetMessageType()]);
							}
							m_subQueueArray[sqi]->Remove(bItemInQueue, sqItemIndex);
							delete item;
							item = nullptr;

							sqItemIndex--;
						}

					}
					// if item still not null, then remove and use the item
					if (item)
					{
						m_subQueueArray[sqi]->Remove(bItemInQueue, sqItemIndex);
						priorityLevel = m_subQueueArray[sqi]->GetPriorityLevel();
					}
					else
					{
						sqItemIndex++;
					}

				} while (!item && bItemInQueue);

				// If the item is nullptr and there is stil item(s) in this sub-queue, that means we are surrendering to 
				// lower priority sub-queues. Set a flag to go back into this sub-queue if no items are ever removed
				// from lower priority queues.
				if (item == nullptr && bItemInQueue)
				{
					bCycleAgain = true;
				}
			}

			// If we have itterated thru all queues and we are set to cycle thru again, reset the sub-queu index.
			if (++sqi == m_subQueueArray.size() && bCycleAgain)
			{
				sqi = 0;
				bCycleAgain = false;
			}
		}
	}

	catch (...)
	{
		throw;
	}

	// Only one thread is expected to remove items. Logging outside of the critical section should log removals in order per queue.
	LogItem(item, priorityLevel, LPCTSTR_POP);

	return item;
}

void CInternalMessageQueueList::FlushMsgType(MessagePersistType persistType)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	CInternalMessage *item(nullptr);

	try
	{

		// Iterate thru the sub queues until we have successfully removed an item.
		// The sub queue array is sorted in order of priority
		unsigned int sqi = 0;
		while (item == nullptr && sqi < m_subQueueArray.size())
		{
			// Remove an item from this sub-queue. 
			// If no item can be removed, the item will be null.
			//
			bool bItemInQueue = false;
			int sqItemIndex = 0;
			do {
				item = (CInternalMessage *)m_subQueueArray[sqi]->Peek(bItemInQueue, sqItemIndex);
				if (item)
				{
					CMessagePersist *mp(item->GetMessagePersist());
					if (mp != nullptr && persistType == mp->GetPersistType())
					{
						m_subQueueArray[sqi]->Remove(bItemInQueue, sqItemIndex);
						delete item;
					}
					else
					{
						item = nullptr;
					}
				}
				else
				{
					sqItemIndex++;
				}

			} while (!item && bItemInQueue);

			sqi++;
		}
	}

	catch (...)
	{
		throw;
	}
}

void CInternalMessageQueueList::PeekMsgType(InternalMsgType& msgType, DWORD& screenId, DWORD& triggerLessType, MessagePersistType persistType)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	CInternalMessage *item(nullptr);
	msgType = INT_MSG_NONE;

	try
	{

		// Iterate thru the sub queues until we have successfully "peeked" an item.
		// The sub queue array is sorted in order of priority
		unsigned int sqi = 0;
		while (item == nullptr && sqi < m_subQueueArray.size())
		{
			// Peek an item from this sub-queue. 
			// If no item can be peeked, the item will be null.
			//
			bool bItemInQueue = false;
			int sqItemIndex = 0;
			do {
				item = (CInternalMessage *)m_subQueueArray[sqi]->Peek(bItemInQueue, sqItemIndex);
				if (item)
				{
					if (!item->IsTTLExpired())
					{
						// got an item.  Determine if this item is related to a "frozen" area
						CMessagePersist *mp(item->GetMessagePersist());

						// is this item the correct persistType to peek?
						if (mp == nullptr || persistType != mp->GetPersistType())
						{
							// Not the right persistType
							item = nullptr;
						}
					}
					else
					{
						item = nullptr;
					}
				}

				if (item)
				{
					msgType = item->GetMessageType();
					screenId = item->GetDWordData();
					triggerLessType = item->GetWordData();
				}
				else
				{
					sqItemIndex++;
				}

			} while (!item && bItemInQueue);

			sqi++;
		}
	}

	catch (...)
	{
		throw;
	}
}

void CInternalMessageQueueList::SetQueueStarvation(int starveThresholdPriority, CMessagePersist *msgPersist)
{
	if (msgPersist)
	{
		if (m_messagePersist[msgPersist->GetPersistType()])
		{
			delete m_messagePersist[msgPersist->GetPersistType()];
		}
		m_messagePersist[msgPersist->GetPersistType()] = msgPersist;

		if (LoggingEnabled())
		{
			LogString(m_logIdentifier, "SetQueueStarvation for persist type %d. %s", msgPersist->GetPersistType(), msgPersist->GetPriorityLogString().c_str());
		}
	}
	else
	{
		m_starveThresholdPriority = starveThresholdPriority;
	}
}

void CInternalMessageQueueList::ClearQueueStarvation(MessagePersistType mpType)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	CMessagePersist *mp = m_messagePersist[mpType];

	if (mp)
	{
		delete mp;
		m_messagePersist[mpType] = nullptr;

		if (LoggingEnabled())
		{
			LogString(m_logIdentifier, "ClearQueueStarvation for persist type %d", mpType);
		}
	}

	if (mpType == PERSIST_NONE)
	{
		CQueueList::ClearQueueStarvation(mpType);
	}
}
bool CInternalMessageQueueList::IsQueueStarved(MessagePersistType mpType, bool& queueIsCleared)
{
	CMessagePersist* mp(nullptr);
	
	lock_guard<recursive_mutex> lock(m_criticalSection);

	if (mpType != PERSIST_NONE)
	{
		mp = m_messagePersist[mpType];
		if (mp != nullptr)
		{
			// Check and expire starvation
			if (!mp->IsQueueStarved())
			{
				// clear starvation
				mp = nullptr;
				ClearQueueStarvation(mpType);
				queueIsCleared = true;
			}
		}
	}

	return mp == nullptr ? false : true;
}

bool CInternalMessageQueueList::IsQueueStarvedEx(MessagePersistType mpType)
{
	bool queueIsCleared(false);
	bool retval = IsQueueStarved(mpType, queueIsCleared);
	return retval;
}

bool CInternalMessageQueueList::IsPersistTypePresent(MessagePersistType mpType)
{
	bool retval = IsPersistTypePresentEx(mpType);

	return retval;
}

// Adds the queue item to the list
bool CInternalMessageQueueList::Add(CQueueItem *item, int priorityLevel)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	bool itemAdded = false;

	try
	{
		itemAdded = AddItem(item, priorityLevel);

		// If we could not add this item, create a new sub queue and try again
		if (!itemAdded)
		{
			AddPriority(priorityLevel);
			itemAdded = AddItem(item, priorityLevel);
		}

		// 2015-12-31 "itemAdded" is always true, because sub-queues are created with unlimited maximum size.
		LogItem(item, priorityLevel, itemAdded ? LPCTSTR_ADD : LPCTSTR_NOTADDED);

		m_binarySemaphore.signal(false);
	}

	catch (...)
	{
		throw;
	}

	return itemAdded;
}

bool CInternalMessageQueueList::AddItem(CQueueItem * item, int priorityLevel)
{
	bool itemAdded = false;
	bool bFlushLowerOnAdd = (m_bFlushLowerOnAdd && item->OkToFlushLowerOnAdd());

	MessagePersistType persistTypeToClear = PERSIST_NONE;
	CMessagePersist *mp = (CMessagePersist *)((CInternalMessage *)item)->GetContent(CMessagePersist::MsgContentType());
	if (mp != nullptr)
	{
		persistTypeToClear = mp->GetPersistType();

		// if this a flusher item, flush persisted items before this item is added
		if (mp->IsFlusher())
		{
			// Get the flush type we are to flush
			int flushType = 0;
			if (mp->GetFlushType(flushType))
			{
				// Get the flush ID we are going to flush
				string flushID;
				if (mp->GetFlushID(flushID))
				{
					// Flush by type and ID
					FlushPersistedByTypeAndId(flushType, flushID);
				}
				else
				{
					// NO flush ID. Flush all of this type
					FlushPersistedByType(flushType);
				}
			}
			else
			{
				// No flush type, Flush all persisted messages
				FlushPersisted();
			}
		}
	}

	// Go through all of the sub queues
	for (unsigned int sqi = 0; sqi < m_subQueueArray.size() && (!itemAdded || bFlushLowerOnAdd); sqi++)
	{
		int priorityDiff = m_subQueueArray[sqi]->ComparePriority(priorityLevel);

		// If the sub queue priority matches, add it to thsi sub queue.
		if (priorityDiff == 0)
		{
			itemAdded = m_subQueueArray[sqi]->Add(item);
		}

		// If we are flushing messages of a lower priority and this sub queue has a lower priority,
		//  clear all the messages in this sub queue
		// NOTE: HIGHEST priority messages will not flush the queue,
		else if (bFlushLowerOnAdd && priorityDiff < 0 && priorityLevel != QUEUE_PRIORITY_HIGHEST)
		{
			m_subQueueArray[sqi]->ClearType(persistTypeToClear);
		}
	}

	if (LoggingEnabled() && nullptr != mp && itemAdded)
	{
		CInternalMessage *msg = (CInternalMessage *)item;
		LogString(m_logIdentifier, "Message %s added to queue at %d priority for persist type %d", InternalMsgTypeStr[msg->GetMessageType()], priorityLevel, mp->GetPersistType());
	}

	return itemAdded;
}

void CInternalMessageQueueList::ResetQueueStarvation(MessagePersistType mpType)
{
	CMessagePersist *mp = nullptr;
	if (mpType != PERSIST_NONE)
	{
		mp = m_messagePersist[mpType];
		if (mp != nullptr)
		{
			mp->StartClock();
		}
	}
}

void CInternalMessageQueueList::RegisterActivity()
{
	CMessagePersist *mp = m_messagePersist[PERSIST_CONTENT_WINDOW];
	if (mp != nullptr)
	{
		mp->Activity();
	}
}

void CInternalMessageQueueList::FlushPersisted()
{
	FlushPersisted(FlushAllPersisted);
}

void CInternalMessageQueueList::FlushAllPersistedButType(int flushTypeIDToNotFlush)
{
	FlushPersisted(FlushAllButType, flushTypeIDToNotFlush);
}

void CInternalMessageQueueList::FlushPersistedByType(int flushTypeIDToFlush)
{
	FlushPersisted(FlushOfType, flushTypeIDToFlush);
}

void CInternalMessageQueueList::FlushPersistedByTypeAndId(int flushTypeID, const string& flushID)
{
	FlushPersisted(FlushOfTypeAndID, flushTypeID, flushID);
}

void CInternalMessageQueueList::FlushPersisted(QueueFlushMethod flushMethod, int flushTypeID, const string& flushID)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	CInternalMessage* item(nullptr);

	try
	{
		if (LoggingEnabled())
		{
			switch (flushMethod)
			{
			case FlushAllPersisted:
				LogString(m_logIdentifier, "Flushing all persisted messages from the queue.");
				break;
			case FlushAllButType:
				LogString(m_logIdentifier, "Flushing all persisted messages EXCEPT flush type %d from queue.", flushTypeID);
				break;
			case FlushOfType:
				LogString(m_logIdentifier, "Flushing persisted messages of flush type %d from queue.", flushTypeID);
				break;
			case FlushOfTypeAndID:
				LogString(m_logIdentifier, "Flushing persisted messages of flush type %d and flush ID %s from queue.", flushTypeID, flushID.c_str());
				break;
			default:
				LogString(m_logIdentifier, "Unknown flush method %d", flushMethod);
				break;
			}
		}

		// Iterate thru the sub queues.
		unsigned int sqi = 0;
		while (sqi < m_subQueueArray.size())
		{
			bool bItemInQueue = false;
			int sqItemIndex = 0;
			do {
				item = (CInternalMessage *)m_subQueueArray[sqi]->Peek(bItemInQueue, sqItemIndex);
				if (item)
				{
					// got an item.  make sure item isn't related to a "frozen" area and a pbt message
					CMessagePersist *mp = (CMessagePersist *)((CInternalMessage *)item)->GetContent(CMessagePersist::MsgContentType());
					if (mp != nullptr)
					{
						bool flushItem = false;
						switch (flushMethod)
						{
						case FlushAllPersisted:
							flushItem = true;
							break;

						case FlushAllButType:
							flushItem = !mp->IsFlushType(flushTypeID);
							break;

						case FlushOfType:
							flushItem = mp->IsFlushType(flushTypeID);
							break;

						case FlushOfTypeAndID:
							flushItem = mp->IsFlushTypeAndID(flushTypeID, flushID);
							break;
						}

						if (flushItem)
						{
							if (LoggingEnabled())
							{
								LogString(m_logIdentifier, "Flushing message, type = %s", InternalMsgTypeStr[item->GetMessageType()]);
							}

							m_subQueueArray[sqi]->Remove(bItemInQueue, sqItemIndex);
							delete item;
							item = nullptr;
							sqItemIndex--;
						}
					}
					sqItemIndex++;
				}

			} while (bItemInQueue);

			sqi++;
		}
	}

	catch (...)
	{
		throw;
	}
}

void CInternalMessageQueueList::SetInterruptible(bool interrupt)
{
	m_equalPriorityInterrupt = interrupt;
}

InternalMsgType CInternalMessageQueueList::GetLastMessageStarvedQueue() const
{
	return m_lastMessageStarvedQueue;
}