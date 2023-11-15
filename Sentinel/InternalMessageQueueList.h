#pragma once
#include "QueueList.h"
#include "InternalMessage.h"
#include "MessagePersist.h"

class CInternalMessageQueueList : public CQueueList
{
public:
	/// <summary>
	/// Constructor of the <see cref="CInternalMessageQueueList"/> class.
	/// Use this constructor when the item-added event handle to be created by this class
	/// </summary>
	/// <param name="createItemAddedEventHandle">True if item-added event is to be created. False if not</param>
	/// <param name="logIdentifier">The identifier used for logging. NULL indicated no logging</param>
	CInternalMessageQueueList(const std::string& logIdentifier = "");

	/// <summary>
	/// Destructor of the <see cref="CInternalMessageQueueList"/> class.
	/// </summary>
	virtual ~CInternalMessageQueueList(void);

public:
	// Removed the next appropriate item from the queue
	virtual CInternalMessage * Remove(void);
	virtual void ClearQueueStarvation( MessagePersistType mpType = PERSIST_NONE);
	virtual bool IsQueueStarved(MessagePersistType mpType, bool& queueIsCleared);
	virtual bool IsQueueStarvedEx( MessagePersistType mpType = PERSIST_NONE);
	virtual bool IsPersistTypePresent( MessagePersistType mpType = PERSIST_NONE);
	virtual bool Add(CQueueItem *item, int priorityLevel);

	/// <summary>
	/// Gets the next message with a MessagePersistType without removing it from the queue.
	/// </summary>
	/// <param name="msgType">OUT- message type</param>
	/// <param name="screenId">OUT- screen id</param>
	/// <param name="triggerLessType">OUT- triggerless type</param>
	/// <param name="persistType">IN- persist type</param>
	virtual void PeekMsgType(InternalMsgType& msgType, DWORD& screenId, DWORD& triggerLessType, MessagePersistType persistType);

	/// <summary>
	/// Flushes the next message with a MessagePersistType.
	/// </summary>
	/// <param name="persistType">IN- the persist type to remove</param>
	virtual void FlushMsgType(MessagePersistType persistType);	
	
	/// <summary>
	/// Registers screen activity with current CMessagePersist objects.
	/// </summary>
	virtual void RegisterActivity();

	/// <summary>
	/// Restarts the queue starvation using original ticks and current time. If MessagePersistDelayType is not PERSIST_TICKS then No Op.
	/// </summary>
	/// <param name="mpType">MessagePersistType to reset</param>
	virtual void ResetQueueStarvation(MessagePersistType mpType);

	/// <summary>
	/// Flushes all persisted messages from queue.
	/// </summary>
	virtual void FlushPersisted();

	/// <summary>
	/// Flushes all persisted messages from queue, but leaves specified flush type messages.
	/// </summary>
	/// <param name="flushTypeIDToNotFlush">The flush type to not flush</param>
	virtual void FlushAllPersistedButType(int flushTypeIDToNotFlush);

	/// <summary>
	/// Flushes all persisted messages from queue of the specified flush type.
	/// </summary>
	/// <param name="flushTypeIDToFlush">The flush type to flush</param>
	virtual void FlushPersistedByType(int flushTypeIDToFlush);

	/// <summary>
	/// Flushes all persisted messages from queue of the specified flush type and flush ID.
	/// </summary>
	virtual void FlushPersistedByTypeAndId(int flushTypeID, const std::string& flushID);

	/// <summary>
	/// Sets the equal priority interrupt flag.
	/// </summary>
	/// <param name="interrupt">if set to <c>true</c> [interrupt].</param>
	virtual void SetInterruptible(bool interrupt);

	/// <summary>
	/// Gets the last message which starved the queue.
	/// </summary>
	/// <returns>the last message type that starved the queue.</returns>
	InternalMsgType GetLastMessageStarvedQueue() const;
private:
	InternalMsgType m_lastMessageStarvedQueue;

	typedef enum
	{
		FlushAllPersisted,
		FlushAllButType,
		FlushOfType,
		FlushOfTypeAndID,
	} QueueFlushMethod;

	CMessagePersist *m_messagePersist[PERSIST_MAX]; // array used for storing current CMessagePersist classes
	bool m_equalPriorityInterrupt;
	void Init();
	virtual bool AddItem(CQueueItem * item, int priorityLevel);
	virtual void SetQueueStarvation(int starveThresholdPriority, CMessagePersist *msgPersist);

	virtual void FlushPersisted(QueueFlushMethod flushMethod, int flushTypeID = 0, const std::string& flushID = _T(""));


};
