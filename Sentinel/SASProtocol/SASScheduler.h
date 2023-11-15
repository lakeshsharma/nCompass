#pragma once
#include "SchedulerList.h"

// This is the priority 
typedef enum _SAS_TASK_PRIORITY_LEVEL
{
	SAS_PRIORITY_HIGHEST = 0,
	SAS_PRIORITY_HIGH,
	SAS_PRIORITY_ABOVE_NORMAL,
	SAS_PRIORITY_NORMAL,
	SAS_PRIORITY_BELOW_NORMAL,
	SAS_PRIORITY_LOW,
	SAS_PRIORITY_LAST		// NOTE: Keep this last if levels are added.
} SAS_TASK_PRIORITY_LEVEL;

const int NUM_SAS_PRIORITY_LEVELS = SAS_PRIORITY_LAST;

// This is the ratios of all of the priorites above.
const int sasPriorityRatios[NUM_SAS_PRIORITY_LEVELS] =
{
	1,		// Ratio for SAS_PRIORITY_HIGHER -- one for each TASK at this priority.
	2,		// Ratio for SAS_PRIORITY_HIGH -- one for each TASK at this priority.
	2,		// Ratio for SAS_PRIORITY_ABOVE_NORMAL
	60,		// Ratio for SAS_PRIORITY_NORMAL
	20,		// Ratio for SAS_PRIORITY_BELOW_NORMAL
	1		// Ratio for SAS_PRIORITY_LOW
};

// 2015-05-30 Leave unused "no-op" tasks out of the schedulde.
// - Unused tasks reduce the scheduling frequency for tasks of the same or lesser priority.
// - "No-Op" tasks can require the insertion of dup-spacer 1A long polls between higher-priority polls.
typedef enum _SAS_SCHEDULED_TASK
{
	SST_DO_GENERAL_POLL,
	SST_GET_HIGH_PRIORITY_METERS,
	SST_SEND_SUBGAME_METERS,	// AKA SST_GET_MULTIGAME_INFORMATION
	SST_GET_PBT_METERS,
	SST_GET_BILL_METERS,
	SST_GET_TICKET_METERS,
	SST_GET_MEDIUM_PRIORITY_GAME_METERS,
	SST_GET_CURRENT_GAME_DENOM,
	SST_ENABLE_DISABLE_EGM_AND_FEATURES,
	SST_SET_GAME_DELAY_UPDATE_CURRENT_GAME_DENOM,
	SST_SEND_DISABLE_REAL_TIME_EVENTS,
	SST_DETERMINE_CAPS_OF_EGM,
	SST_SET_TIME,
	SST_SET_TICKET_DATA,
	SST_LAST		// NOTE: Keep this last if tasks are added.
} SAS_SCHEDULED_TASK;

const int NUM_SAS_SCHEDULED_TASKS = SST_LAST;

// NOTE: This must match the order of tasks above.
const SAS_TASK_PRIORITY_LEVEL sasTaskPriority[NUM_SAS_SCHEDULED_TASKS] =
{
	SAS_PRIORITY_HIGHEST,			// priority level for SST_DO_GENERAL_POLL
	SAS_PRIORITY_HIGH,			// priority level for SST_GET_HIGH_PRIORITY_METERS
	SAS_PRIORITY_HIGH,			// priority level for SST_GET_MULTIGAME_INFORMATION
	SAS_PRIORITY_ABOVE_NORMAL,	// priority level for SST_GET_PBT_METERS
	SAS_PRIORITY_ABOVE_NORMAL,	// priority level for SST_GET_BILL_METERS
	SAS_PRIORITY_ABOVE_NORMAL,	// priority level for SST_GET_TICKET_METERS
	SAS_PRIORITY_ABOVE_NORMAL,	// priority level for SST_GET_MEDIUM_PRIORITY_GAME_METERS
	SAS_PRIORITY_ABOVE_NORMAL,	// priority level for SST_GET_CURRENT_GAME_DENOM
	SAS_PRIORITY_NORMAL,		// priority level for SST_ENABLE_DISABLE_EGM
	SAS_PRIORITY_NORMAL,		// priority level for SST_SET_GAME_DELAY_UPDATE_CURRENT_GAME_DENOM
	SAS_PRIORITY_NORMAL,		// priority level for SST_SEND_DISABLE_REAL_TIME_EVENTS
	SAS_PRIORITY_BELOW_NORMAL,	// priority level for SST_DETERMINE_CAPS_OF_EGM
	SAS_PRIORITY_BELOW_NORMAL,	// priority level for SST_SET_TIME
	SAS_PRIORITY_BELOW_NORMAL,	// priority level for SST_SET_TICKET_DATA
};

class CSASScheduler
{
public:
	CSASScheduler(void);
public:
	~CSASScheduler(void);

	SAS_SCHEDULED_TASK GetNextScheduledTask( void );

	int GetPriorityCount( void );

	DWORD GetLastIntervalForPriority( SAS_TASK_PRIORITY_LEVEL priorityLevel );

private:

		// Add a priority sub queue to the queue list.
	void AddPriority(SAS_TASK_PRIORITY_LEVEL priorityLevel, int priorityRatio);
	bool AddItem(SAS_SCHEDULED_TASK item, SAS_TASK_PRIORITY_LEVEL priorityLevel);

	std::vector<CSchedulerList *> m_scheduleListArray;

};
