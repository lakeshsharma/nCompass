#include "stdafx.h"
#include "SchedulerList.h"
#include "Utilities.h"

CSchedulerList::CSchedulerList(int priorityLevel, int priorityRatio)
{
	m_priorityLevel = priorityLevel;
	m_priorityRatio = priorityRatio;
	m_currentIndex = 0;
	m_lastRetrievalTime = 0;
	m_lastRetrievalInterval = 0;
	m_usageCount = 0;
}

CSchedulerList::~CSchedulerList(void)
{
}

void CSchedulerList::AddItem(int item)
{
	m_items.push_back( item );
}

bool CSchedulerList::GetNextItem(int &item)
{
	bool retVal = true;

	if ( !m_items.empty() && m_usageCount < m_priorityRatio )
	{

		if (m_currentIndex > (int)m_items.size() - 1)
			m_currentIndex = 0;

		// For timing info
		if ( m_currentIndex == 0 )
		{
			DWORD newRetrievalTime = CUtilities::GetTickCount(); 
			if ( m_lastRetrievalTime > 0 && newRetrievalTime > m_lastRetrievalTime )
			{
				m_lastRetrievalInterval = newRetrievalTime - m_lastRetrievalTime;
			}
			m_lastRetrievalTime = CUtilities::GetTickCount();
		}

		item = m_items[m_currentIndex];
		m_usageCount++;
		m_currentIndex++;
	}

	else
	{
		m_usageCount = 0;
		retVal = false;
	}

	return retVal;
}

