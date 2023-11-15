#include "stdafx.h"
#include "MessagePersist.h"
#include "QueueList.h"
#include "Utilities.h"

using namespace std;

CMessagePersist::CMessagePersist(bool activate, DWORD mainQueueStarveTicks, int mainPriority, MessagePersistType persistType, DWORD interactiveQueueStarveTicks, int interactivePriority)
{
	if (activate)
	{
		m_queueStarveStart = CUtilities::GetTickCount();
		m_recentActivityTicks = m_queueStarveStart;
		m_activated = true;
	}
	else
	{
		m_queueStarveStart = 0;
		m_recentActivityTicks = CUtilities::GetTickCount();
		m_activated = false;
	}
	m_interactiveQueueStarveTicks = interactiveQueueStarveTicks;
	m_mainQueueStarveTicks = mainQueueStarveTicks;

	m_interactivePriority = interactivePriority;
	m_mainPriority = mainPriority;
	m_persistType = persistType;
	m_newStarvation = true;

	m_hasFlushType = false;
	m_flushTypeID = 0;
	m_hasFlushID = false;
	m_isFlusher = false;
}

CMessagePersist::CMessagePersist(MessagePersistType persistType)
{
	m_queueStarveStart = 0;
	m_persistType = persistType;
	m_interactivePriority = QUEUE_PRIORITY_LOWEST;
	m_mainPriority = QUEUE_PRIORITY_LOWEST;
	m_activated = false;
	m_newStarvation = false;

	m_hasFlushType = false;
	m_flushTypeID = 0;
	m_hasFlushID = false;
	m_isFlusher = false;
}

CMessagePersist::~CMessagePersist(void)
{
}

bool CMessagePersist::isNewStarvation()
{
	return m_newStarvation;
}

bool CMessagePersist::IsQueueStarved(int priority, bool equalInterrupt)
{
	bool result = false;
	
	if (m_activated)
	{
		if (priority == -1)
		{
			// check if starvation should be timed out.
			// check interactive
			if (m_interactiveQueueStarveTicks > 0)
			{
				// Starve if ticks since last activy have not elapsed
				result = !CheckElapsedTime(m_recentActivityTicks, m_interactiveQueueStarveTicks);
			}

			// check main queue starvation
			if (!result && m_mainQueueStarveTicks > 0)
			{
				// Starve if main ticks have not elapsed
				result = !CheckElapsedTime(m_queueStarveStart, m_mainQueueStarveTicks);
			}

			// reset m_activated to current starvation status
			m_activated = result;
		}
		else
		{
			// check interactive
			if (m_interactiveQueueStarveTicks > 0)
			{
				// Starve if lower/equal priority AND ticks since last activy have not elapsed
				result = priority >= m_interactivePriority && !CheckElapsedTime(m_recentActivityTicks, m_interactiveQueueStarveTicks);
			}

			// check main queue starvation
			if (!result && m_mainQueueStarveTicks > 0)
			{
				// Starve if lower/equal priority AND main ticks have not elapsed
				// Allow equal priority interrupt if configured this way
				result = ((priority >= m_mainPriority && !equalInterrupt) || (priority > m_mainPriority && equalInterrupt)) 
					&& !CheckElapsedTime(m_queueStarveStart, m_mainQueueStarveTicks);

			}

			// reset m_activated to current starvation status
			m_activated = result;
		}
	}


	return result;
}

bool CMessagePersist::CheckElapsedTime(DWORD startTicks, DWORD waitTicks)
{
	return CUtilities::GetTickCount() - startTicks >= waitTicks;
}

int CMessagePersist::GetPriority()
{
	return CheckElapsedTime(m_recentActivityTicks, m_interactiveQueueStarveTicks) ? m_mainPriority : m_interactivePriority;
}

MessagePersistType CMessagePersist::GetPersistType()
{
	return m_persistType;
}

void CMessagePersist::StartClock()
{
	m_queueStarveStart = CUtilities::GetTickCount();
	m_recentActivityTicks = m_queueStarveStart;
	m_activated = true;
}

void CMessagePersist::Activity()
{
	m_recentActivityTicks = CUtilities::GetTickCount();

}

void CMessagePersist::SetMainStarveTicks(DWORD ticks)
{
	m_mainQueueStarveTicks = ticks;
}

void CMessagePersist::ResetInteractive()
{
	m_interactiveQueueStarveTicks = INTERACTIVESTARVETIME;
}

void CMessagePersist::SetMainStarvation(int priority, DWORD ticks)
{
	m_mainPriority = priority;
	if (ticks > 0)
		m_mainQueueStarveTicks = ticks;
}

void CMessagePersist::SetFlushType(int flushType)
{
	m_hasFlushType = true;
	m_flushTypeID = flushType;
}

void CMessagePersist::SetFlushTypeAndId(int flushType, const string& flushID)
{
	SetFlushType(flushType);

	m_hasFlushID = true;
	m_flushID = flushID;
}

bool CMessagePersist::GetFlushType(int &flushType)
{
	flushType = m_flushTypeID;

	return m_hasFlushType;
}

bool CMessagePersist::GetFlushID(string& flushID)
{
	flushID = m_flushID;

	return m_hasFlushID;
}

bool CMessagePersist::IsFlushType(int flushType)
{
	return (m_hasFlushType && flushType == m_flushTypeID);
}

bool CMessagePersist::IsFlushTypeAndID(int flushType, const string& flushID)
{
	return (IsFlushType(flushType) && m_hasFlushID && flushID == m_flushID);
}

void CMessagePersist::SetAsFlusher()
{
	m_isFlusher = true;
}

bool CMessagePersist::IsFlusher()
{
	return m_isFlusher;
}

void CMessagePersist::SetInteractiveStarvation(int priority, DWORD ticks)
{
	m_interactivePriority = priority;
	m_interactiveQueueStarveTicks = ticks;
}

DWORD CMessagePersist::GetMainQueueStarveTicks() const
{
	return m_mainQueueStarveTicks;
}

string CMessagePersist::GetPriorityLogString() const
{
	stringstream stream;
	stream << "Interactive Priority " << m_interactivePriority << " for " << m_interactiveQueueStarveTicks << " msg. "
		<< "Main Priority " << m_mainPriority << " for " << m_mainQueueStarveTicks << " ms.";
	return stream.str();
}
