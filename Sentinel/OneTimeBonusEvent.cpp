#include "stdafx.h"

#include "OneTimeBonusEvent.h"
#include "Hardware/NonVolatileMemoryMaps.h"

COneTimeBonusEvent::COneTimeBonusEvent(int eventID, const time_t &startTime, const time_t &stopTime, WORD oneTimeBonusPoints)
: CBonusEvent( eventID )
{
	SetBonusEventType ( ONE_TIME_BONUS );
	m_startTime = startTime;
	m_stopTime = stopTime;

	m_oneTimeBonusPoints = oneTimeBonusPoints;
	m_oneTimeBonusPointsCurrent = oneTimeBonusPoints;
}

COneTimeBonusEvent::COneTimeBonusEvent(COneTimeBonusEvent &eventRHS)
: CBonusEvent( eventRHS.GetEventID() )
{
	SetBonusEventType ( ONE_TIME_BONUS );

	*this = eventRHS;
}

COneTimeBonusEvent::COneTimeBonusEvent( byte *nvramBuffer )
: CBonusEvent( nvramBuffer )
{
	// Adjust for the size used in the parent CBonusEvent class.
	nvramBuffer += m_nvramUsedLength;

	memcpy( &m_oneTimeBonusPoints, nvramBuffer, sizeof( m_oneTimeBonusPoints ) );
	nvramBuffer += sizeof( m_oneTimeBonusPoints );

	memcpy(&m_oneTimeBonusPointsCurrent, nvramBuffer, sizeof(m_oneTimeBonusPointsCurrent));
	nvramBuffer += sizeof(m_oneTimeBonusPointsCurrent);
}

COneTimeBonusEvent::~COneTimeBonusEvent(void)
{
}

COneTimeBonusEvent & COneTimeBonusEvent::operator=( COneTimeBonusEvent &eventRHS )
{
	SetEventID( eventRHS.GetEventID() );
	if ( eventRHS.WasTransactionsSent() )
		SetTransactionAsSent();
	SetBonusPoints( eventRHS.GetAwardedPoints() );

	m_startTime = eventRHS.GetStartTime();
	m_stopTime = eventRHS.GetStopTime();
	m_oneTimeBonusPoints = eventRHS.GetOneTimeBonusPointsToAward();
	m_oneTimeBonusPointsCurrent = eventRHS.GetAwardedPointsCurrent();
	m_bCurrentlyInEvent = eventRHS.IsCurrentlyInEventTime();

	return *this;
}

int COneTimeBonusEvent::GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength )
{
	byte *bufferPos = nvramBuffer;

	if ( nvramBufferLength >= (int)NVRAM_PLAYER_BONUS_EVENT_SIZE )
	{
		int commonLen = CBonusEvent::GetNvramBuffer( nvramBuffer, nvramBufferLength );
		bufferPos += commonLen;
		
		memcpy( bufferPos, &m_oneTimeBonusPoints, sizeof( m_oneTimeBonusPoints ) );
		bufferPos += sizeof( m_oneTimeBonusPoints );

		memcpy(bufferPos, &m_oneTimeBonusPointsCurrent, sizeof(m_oneTimeBonusPointsCurrent));
		bufferPos += sizeof(m_oneTimeBonusPointsCurrent);
	}

	return (bufferPos - nvramBuffer);
}

bool COneTimeBonusEvent::AwardPoints( DWORD newPoints )
{
	bool bPointsAwarded = false;

	if ( !WerePointsAwared() && newPoints > 0 )
	{
		m_bCurrentlyInEvent = IsCurrentlyInEventTime();

		if ( m_bCurrentlyInEvent )
		{
			AddBonusPoints( m_oneTimeBonusPoints );
			bPointsAwarded = true;
		}
	}

	return bPointsAwarded;
}

bool COneTimeBonusEvent::AwardPartialPoints( DWORD countdownDecrement, DWORD divisor, DWORD multiplier)
{
	(void)countdownDecrement; // Unused formal parameter.
	(void)divisor; // Unused formal parameter.
	(void)multiplier; // Unused formal parameter.

	// Partial points are never awarded with one-time bonus events.
	return false;
}

void COneTimeBonusEvent::ResetCurrentAwardedPoints(void)
{
	m_oneTimeBonusPointsCurrent = 0;
}

WORD COneTimeBonusEvent::GetAwardedPointsCurrent(void)
{
	return m_oneTimeBonusPointsCurrent;
}
