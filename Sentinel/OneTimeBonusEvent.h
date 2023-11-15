#pragma once
#include "BonusEvent.h"

class COneTimeBonusEvent :
	public CBonusEvent
{
public:
	COneTimeBonusEvent(int eventID, const time_t &startTime, const time_t &stopTime, WORD oneTimeBonusPoints);
	COneTimeBonusEvent( COneTimeBonusEvent &eventRHS );
	COneTimeBonusEvent( byte *nvramStream );
public:
	virtual ~COneTimeBonusEvent(void);

	COneTimeBonusEvent &operator=( COneTimeBonusEvent &eventRHS );

	int GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength );

	virtual bool AwardPoints( DWORD newPoints );
	virtual bool AwardPartialPoints( DWORD countdownDecrement, DWORD divisor, DWORD multiplier);

	WORD GetOneTimeBonusPointsToAward( void )
	{
		return m_oneTimeBonusPoints;
	}

	virtual void ResetCurrentAwardedPoints(void);
	virtual WORD GetAwardedPointsCurrent(void);

private:
	WORD m_oneTimeBonusPoints;
	WORD m_oneTimeBonusPointsCurrent;
};
