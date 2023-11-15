#include "stdafx.h"
#include "SASScheduler.h"

CSASScheduler::CSASScheduler(void)
{

	// Load all of the priority level lists.
	for ( int stn = 0; stn < NUM_SAS_SCHEDULED_TASKS; stn++ )
	{
		if ( !AddItem( (SAS_SCHEDULED_TASK)stn, sasTaskPriority[stn] ) )
		{
			AddPriority( sasTaskPriority[stn], sasPriorityRatios[sasTaskPriority[stn]] );
			AddItem( (SAS_SCHEDULED_TASK)stn, sasTaskPriority[stn] );
		}
	}
}

CSASScheduler::~CSASScheduler(void)
{
	for ( int sli = 0; sli < (int)m_scheduleListArray.size(); sli++ )
		delete m_scheduleListArray[sli];
}

SAS_SCHEDULED_TASK CSASScheduler::GetNextScheduledTask( void )
{
	int taskID = SST_LAST;

	bool taskFound = false;
	for ( int sli = 0; sli < (int)m_scheduleListArray.size() && !taskFound; sli++ )
	{
		taskFound = m_scheduleListArray[sli]->GetNextItem( taskID );
	}
	
	return (SAS_SCHEDULED_TASK)taskID;
}

DWORD CSASScheduler::GetLastIntervalForPriority( SAS_TASK_PRIORITY_LEVEL priorityLevel )
{
	DWORD interval = 0;

	for ( int li = 0; li < (int)m_scheduleListArray.size() && interval == 0; li++ )
	{
		if ( m_scheduleListArray[li]->ComparePriority( priorityLevel ) == 0 )
		{
			interval = m_scheduleListArray[li]->LastRetrievalInterval();
		}
	}

	return interval;
}

void CSASScheduler::AddPriority(SAS_TASK_PRIORITY_LEVEL priorityLevel, int priorityRatio)
{
	// Make a new sub queue item for with this priority.
	CSchedulerList *priorityList = new CSchedulerList(priorityLevel, priorityRatio);

	// Insert the new item to the list array.
	// This array is sorted by priority, so make sure it goes to the proper 
	//  location in the array.
	bool listAdded = false;
	for ( int li = 0; li < (int)m_scheduleListArray.size() && !listAdded; li++ )
	{
		if ( m_scheduleListArray[li]->ComparePriority( priorityLevel ) < 0 )
		{
			auto it = m_scheduleListArray.begin();
			m_scheduleListArray.insert( it + li, priorityList );
			listAdded = true;
		}
	}

	// If it was not inserted in the array, put it at the end.
	if ( !listAdded )
		m_scheduleListArray.push_back( priorityList );
}


bool CSASScheduler::AddItem(SAS_SCHEDULED_TASK item, SAS_TASK_PRIORITY_LEVEL priorityLevel)
{
	bool itemAdded = false;
	for ( int li = 0; li < (int)m_scheduleListArray.size() && !itemAdded; li++ )
	{
		if ( m_scheduleListArray[li]->IsPriorityLevel( priorityLevel ) )
		{
			m_scheduleListArray[li]->AddItem( item );
			itemAdded = true;
		}
	}

	return itemAdded;
}
