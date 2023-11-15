#pragma once


//Number of consecutive bad card reads before a bad card transaction
const int BAD_CARD_READ_THREASHOLD = 5;

// Bad card read reminder
const int BAD_CARD_READ_REMINDER_INTERVAL = 3000;	// Milliseconds inbetween reminders
const int BAD_CARD_READ_MAX_REMINDERS = 5;			// MAximum number of reminders.


class CBadReadCount
{
public:
	CBadReadCount(void);
	~CBadReadCount(void);

	/// <summary>
	/// Increment bad card read counter
	/// </summary>
	/// <returns></returns>
	void IncBadReadCount( );

	/// <summary>
	/// Reset bad card read count to 0
	/// </summary>
	/// <returns></returns>
	void ResetBadReadCount( );

	/// <summary>
	/// Get bad card read counter
	/// </summary>
	/// <returns>Returns bad card read counter</returns>
	DWORD GetBadReadCount( );

	/// <summary>
	/// Set tick count of next bad card read reminder
	/// </summary>
	/// <param name="ticks">tick count of next bad card read reminder</param>
	/// <returns></returns>
	void SetBadCardReadReminderTick( DWORD ticks );

	/// <summary>
	/// Decrement bad card read reminders count
	/// </summary>
	/// <returns></returns>
	void DecBadCardReadReminders( );

	/// <summary>
	/// Set bad card read reminders
	/// </summary>
	/// <param name="count">Set count of bad card read reminders to be sent</param>
	/// <returns></returns>
	void SetBadCardReadReminders( int count );
	

	/// <summary>
	/// Get bad card read reminder count
	/// </summary>
	/// <returns></returns>
	int GetBadCardReadReminders( );

	/// <summary>
	/// Check if bad card read reminder is now required
	/// </summary>
	/// <returns>Returns true if bad card read reminder is required</returns>
	bool IsBadCardReminderRequired( );

	/// <summary>
	/// Check if too many bad card reads
	/// </summary>
	/// <returns>Returns true if too many bad card reads</returns>
	bool IsTooManyBadCardReads( );

private:
	DWORD m_nextBadCardReaderReminderTick;
	int m_remainingBadCardReadReminders;
	DWORD m_badCardReadCount;

};

