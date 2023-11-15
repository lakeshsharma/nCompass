#pragma once
#include <vector>

class CSchedulerList
{
public:
	CSchedulerList(int priorityLevel, int priorityRatio);
public:
	~CSchedulerList(void);

	// This returns the next item until the list has reached its ratio count.
	// If the ratio count has been reached, false is returned
	bool GetNextItem( int &item );

	void AddItem( int item );

	// Does this list have the specified priority level.
	bool IsPriorityLevel(int priorityLevel)
	{
		return priorityLevel == m_priorityLevel;
	}

	// Returns the difference of the specified priority to the list priority.
	// A negative return means the the specified priority is less than the subqueue priority
	// A positive return means the the specified priority is greater than the subqueue priority
	// A 0 return means they are equal.
	int ComparePriority( int priorityLevel )
	{
		return ( priorityLevel - m_priorityLevel);
	}

	// This returns the elapsed time for an item at this priority level.
	DWORD LastRetrievalInterval( void )
	{
		return m_lastRetrievalInterval;
	}


private:
	// A lower number is a higher priority queue
	int m_priorityLevel;

	int m_priorityRatio;

	std::vector<DWORD> m_items;

	// This is the count of times in a row we have accessed an item from this list.
	int m_usageCount;

	int m_currentIndex;

	DWORD m_lastRetrievalTime;
	DWORD m_lastRetrievalInterval;

};

