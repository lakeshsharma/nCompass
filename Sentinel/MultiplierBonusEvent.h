#pragma once
#include "BonusEvent.h"

class CMultiplierBonusEvent :
	public CBonusEvent
{
public:
	CMultiplierBonusEvent(int eventID, const time_t &startTime, const time_t &stopTime, WORD pointsMultiplier);
	CMultiplierBonusEvent( CMultiplierBonusEvent &eventRHS );
	CMultiplierBonusEvent( byte *nvramStream);
public:
	virtual ~CMultiplierBonusEvent(void);

	CMultiplierBonusEvent &operator=( CMultiplierBonusEvent &eventRHS );  

	int GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength );


	virtual bool AwardPoints( DWORD newPoints );
	virtual bool AwardPartialPoints( DWORD countdownDecrement, DWORD divisor, DWORD multiplier);

	// Will return 1 if not currently in bonus event.
	WORD GetCurrentMultiplier( void );

	void SessionReset( void );

	WORD GetMultiplier( void )
	{
		return m_pointsMultiplier;
	}

	virtual void ResetCurrentAwardedPoints(void);
	virtual WORD GetAwardedPointsCurrent(void);
private:
	WORD m_pointsMultiplier;

	WORD m_partialBonusingRemainder;

};
