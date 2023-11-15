#pragma once

class CQueueItem
{
public:
	CQueueItem(void);
	virtual ~CQueueItem(void);

	void SetPriority( int priority )
	{
		m_priority = priority;
	}

	int GetPriority( void )
	{
		return m_priority;
	}

	/// <summary>
	/// Sets whether it is ok to flush lower priority items on add.
	/// </summary>
	/// <param name="okToFlushLowerOnAdd">IN - Whether it is ok to flush lower priority items on add.</param>
	void SetOkToFlushLowerOnAdd(bool okToFlushLowerOnAdd);

	/// <summary>
	/// Gets whether it is ok to flush lower priority items on add.
	/// </summary>
	/// <returns>True if it is ok to flush lower priority items on add, false otherwise</returns>
	bool OkToFlushLowerOnAdd();

private:
	int m_priority;
	bool m_okToFlushLowerOnAdd;
};