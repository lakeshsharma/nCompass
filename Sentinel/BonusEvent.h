#pragma once

typedef enum
{
	ONE_TIME_BONUS,
	MULTIPLIER_BONUS,
} BonusEventType;

class CBonusEvent
{
public:
	CBonusEvent(int eventID);
	CBonusEvent( byte *nvramStream);
public:
	virtual ~CBonusEvent(void);

	virtual int GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength );
	bool IsCurrentlyInEventTime( void );

	virtual bool AwardPoints( DWORD newPoints ) = 0;
	virtual bool AwardPartialPoints( DWORD countdownDecrement, DWORD divisor, DWORD multiplier) = 0;

	virtual void ResetCurrentAwardedPoints(void) = 0;
	virtual WORD GetAwardedPointsCurrent(void) = 0;

	void SetEventID( int eventID )
	{
		m_eventID = eventID;
	}

	/// <summary>
	/// Returns the event ID.
	/// </summary>
	/// <returns>The event ID.</returns>
	int GetEventID( void ) const;

	void SetBonusEventType( BonusEventType type )
	{
		m_bonusType = type;
	}

	/// <summary>
	/// Returns the type of the bonus event.
	/// </summary>
	/// <returns>The type of the bonus event.</returns>
	BonusEventType GetBonusEventType( void ) const;

	void AddBonusPoints( DWORD pointsToAdd )
	{
		m_pointsAwarded += pointsToAdd;
		m_pointsAwardedCurrent += pointsToAdd;
	}

	void SetBonusPoints( DWORD bonusPoints )
	{
		m_pointsAwarded = bonusPoints;
		m_pointsAwardedCurrent = bonusPoints;
	}

	DWORD GetAwardedPoints( void )
	{
		return m_pointsAwarded;
	}

	bool WerePointsAwared( void )
	{
		return m_pointsAwarded > 0;
	}

	bool WasTransactionsSent( void )
	{
		return m_bEventTransactionSent;
	}

	void SetTransactionAsSent( bool bSent = true )
	{
		m_bEventTransactionSent = bSent;
	}

	/// <summary>
	/// Returns the start time for the event.
	/// </summary>
	/// <returns>The start time for the event.</returns>
	time_t GetStartTime( void ) const
	{
		return m_startTime;
	}

	/// <summary>
	/// Returns the stop time for the event.
	/// </summary>
	/// <returns>The stop time for the event.</returns>
	time_t GetStopTime( void ) const
	{
		return m_stopTime;
	}

protected:
	// This is the about of buffer space used to read the buffer.
	int m_nvramUsedLength;	
	time_t m_startTime;
	time_t m_stopTime;
	bool m_bCurrentlyInEvent;
	DWORD m_pointsAwardedCurrent;

private:
	BonusEventType m_bonusType;
	int m_eventID;

	bool m_bEventTransactionSent;
	DWORD m_pointsAwarded;

};


