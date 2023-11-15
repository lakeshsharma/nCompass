#pragma once
#include "InternalMsgContent.h"
#include "MessagePersistType.h"
#include <stdafx.h>

// 10 Seconds
const DWORD INTERACTIVESTARVETIME = 10000;

typedef enum
{
	PERSIST_TICKS,
	PERSIST_TIME,
} MessagePersistDelayType;


// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class QueueUnitTests; }

class CMessagePersist : public CInternalMsgContent<INT_MSG_CONTENT_PERSIST>
{
	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::QueueUnitTests;

public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CMessagePersist" /> class.
	/// </summary>
	/// <param name="activate">Most times the timer shouldn't start until object is pulled off the queue</param>
	/// <param name="mainQueueStarveTicks">The main queue starve ticks.</param>
	/// <param name="mainPriority">The main priority.</param>
	/// <param name="persistType">Type of the persist.</param>
	/// <param name="interactiveQueueStarveTicks">The interactive queue starve ticks.</param>
	/// <param name="interactivePriority">The interactive priority.</param>
	CMessagePersist(bool activate, DWORD mainQueueStarveTicks, int mainPriority, MessagePersistType persistType, DWORD interactiveQueueStarveTicks = 0, int interactivePriority = -1);

	/// <summary>
	/// Initialize new instance of CMessagePersist that doesn't starve a queue.
	/// </summary>
	/// <param name="persistType">Type of the persist.</param>
	CMessagePersist(MessagePersistType persistType);
	
	/// <summary>
	/// Finalizes an instance of the <see cref="CMessagePersist"/> class.
	/// </summary>
	virtual ~CMessagePersist(void);

	int GetPriority();
	MessagePersistType GetPersistType();
	
	/// <summary>
	/// Starts or resets the clock.
	/// </summary>
	void StartClock();

	/// <summary>
	/// Reset clock so primary starvation remains active.
	/// </summary>
	void Activity();

	/// <summary>
	/// Checks the queue starvation. Handles transition to secondary starvation when primary expires
	/// </summary>
	/// <param name="prioirty">The queue priority to use to determine starvation. If -1 don't consider priority in determination</param>
	/// <returns>
	/// Returns true if queue is starved.
	/// </returns>
	bool IsQueueStarved(int prioirty = -1, bool equalInterrupt = false);

	/// <summary>
	/// Should a new starvation be activated when processed.
	/// </summary>
	/// <returns>Should a new starvation be activated.</returns>
	bool isNewStarvation();

	/// <summary>
	/// Sets the main ticks to starve.
	/// </summary>
	/// <param name="ticks">The ticks.</param>
	void SetMainStarveTicks(DWORD ticks);

	/// <summary>
	/// Sets the main starvation.
	/// </summary>
	/// <param name="priority">The priority to starve the queue.</param>
	/// <param name="ticks">The ticks to keep starvation active.</param>
	void SetMainStarvation(int priority, DWORD ticks = 0);

	/// <summary>
	/// Sets the interactive starvation.
	/// </summary>
	/// <param name="priority">The priority to starve the queue.</param>
	/// <param name="ticks">The ticks to keep starvation active.</param>
	void SetInteractiveStarvation(int priority, DWORD ticks);

	/// <summary>
	/// Resets the interactive starvation ticks.
	/// </summary>
	void ResetInteractive();

	/// <summary>
	/// Sets the item's flush type.
	/// </summary>
	/// <param name="flushType">The flush type of this persist item.</param>
	void SetFlushType(int flushType);

	/// <summary>
	/// Sets the item's flush type and flush ID.
	/// </summary>
	/// <param name="flushType">The flush type of this persist item.</param>
	/// <param name="flushID">The flush ID of this persist item.</param>
	void SetFlushTypeAndId(int flushType, const std::string& flushID);

	/// <summary>
	/// Gets the flush type of this item
	/// </summary>
	/// <param name="flushType">The reference to the flush type to be returned</param>
	/// <returns>
	/// Returns true if this item has a flush type set, otherwise the flush type returned is N/A
	/// </returns>
	bool GetFlushType(int &flushType);

	/// <summary>
	/// Gets the flush ID of this item
	/// </summary>
	/// <param name="flushID">The reference to the flush ID to be returned</param>
	/// <returns>
	/// Returns true if this item has a flush ID set, otherwise the flush ID returned is N/A
	/// </returns>
	bool GetFlushID(std::string& flushID);

	/// <summary>
	/// Determines if this item is of the specified flush type
	/// </summary>
	/// <param name="flushType">The flush type to be checked for.</param>
	/// <returns>
	/// Returns true if this item is of the specified flush type.
	/// </returns>
	bool IsFlushType(int flushType);

	/// <summary>
	/// Determines if this item is of the specified flush type AND the flush ID matches
	/// </summary>
	/// <param name="flushType">The flush type to be checked for.</param>
	/// <param name="flushID">The flush ID to be checked for.</param>
	/// <returns>
	/// Returns true if this item is of the specified flush type and the flush ID matches
	/// </returns>
	bool IsFlushTypeAndID(int flushType, const std::string& flushID);

	/// <summary>
	/// Sets this persist item as a flusher item to flush other items
	/// </summary>
	void SetAsFlusher();

	/// <summary>
	/// Determines if this item is a to flush other items
	/// </summary>
	/// <returns>
	/// Returns true if this item is a flusher item.
	/// </returns>
	bool IsFlusher();

	/// <summary>
	/// Gets the string for logging the priorities
	/// </summary>
	/// <returns>
	/// The string describing the priorities for this message persist object
	/// </returns>
	std::string GetPriorityLogString() const;

	/// <summary>
	/// Gets the main starvation.
	/// </summary>
	/// <returns>main starvation</returns>
	DWORD GetMainQueueStarveTicks() const;

private:
	/// <summary>
	/// Tick count that queue starvation started
	/// </summary>
	DWORD m_queueStarveStart;

	/// <summary>
	/// Tick count of most recent activity
	/// </summary>
	DWORD m_recentActivityTicks;

	/// <summary>
	/// Interactive queue priority to starve
	/// </summary>
	int	m_interactivePriority;

	/// <summary>
	/// Main queue priority to starve
	/// </summary>
	int m_mainPriority;

	/// <summary>
	/// Number of ticks to starve at interactive queue priority
	/// </summary>
	DWORD m_interactiveQueueStarveTicks;

	/// <summary>
	/// Number of ticks to starve at main queue priority
	/// </summary>
	DWORD m_mainQueueStarveTicks;

	bool CheckElapsedTime(DWORD startTicks, DWORD waitTicks);

	MessagePersistType m_persistType;
	bool m_activated;
	bool m_newStarvation;

	// Flushable attributes
	bool m_hasFlushType;
	int m_flushTypeID;
	bool m_hasFlushID;
	std::string m_flushID;
	bool m_isFlusher;
};
