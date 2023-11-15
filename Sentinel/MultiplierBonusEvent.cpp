#include "stdafx.h"

#include "MultiplierBonusEvent.h"
#include "Hardware/NonVolatileMemoryMaps.h"

CMultiplierBonusEvent::CMultiplierBonusEvent(int eventID, const time_t &startTime, const time_t &stopTime, WORD pointsMultiplier)
: CBonusEvent( eventID )
{
	SetBonusEventType( MULTIPLIER_BONUS );

	m_startTime = startTime;
	m_stopTime = stopTime;
	m_pointsMultiplier  = pointsMultiplier;
	m_partialBonusingRemainder = 0;
}

CMultiplierBonusEvent::CMultiplierBonusEvent( byte *nvramBuffer)
: CBonusEvent( nvramBuffer)
{
	// Adjust for the size used in the parent CBonusEvent class.
	nvramBuffer += m_nvramUsedLength;

	memcpy( &m_pointsMultiplier, nvramBuffer, sizeof( m_pointsMultiplier ) );
	nvramBuffer += sizeof( m_pointsMultiplier );

	m_partialBonusingRemainder = 0;
}

CMultiplierBonusEvent::CMultiplierBonusEvent(CMultiplierBonusEvent &eventRHS)
: CBonusEvent( eventRHS.GetEventID() )
{
	SetBonusEventType( MULTIPLIER_BONUS );
	*this = eventRHS;
}

CMultiplierBonusEvent::~CMultiplierBonusEvent(void)
{
}

CMultiplierBonusEvent & CMultiplierBonusEvent::operator=( CMultiplierBonusEvent &eventRHS )
{
	SetEventID( eventRHS.GetEventID() );
	if ( eventRHS.WasTransactionsSent() )
		SetTransactionAsSent();
	SetBonusPoints( eventRHS.GetAwardedPoints() );

	m_startTime = eventRHS.GetStartTime();
	m_stopTime = eventRHS.GetStopTime();
	m_pointsMultiplier = eventRHS.GetMultiplier();
	m_bCurrentlyInEvent = eventRHS.IsCurrentlyInEventTime();

	m_partialBonusingRemainder = 0;

	return *this;
}

int CMultiplierBonusEvent::GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength )
{
	byte *bufferPos = nvramBuffer;

	if ( nvramBufferLength >= (int)NVRAM_PLAYER_BONUS_EVENT_SIZE )
	{
		int commonLen = CBonusEvent::GetNvramBuffer( nvramBuffer, nvramBufferLength );
		bufferPos += commonLen;
		
		memcpy( bufferPos, &m_pointsMultiplier, sizeof( m_pointsMultiplier ) );
		bufferPos += sizeof( m_pointsMultiplier );

	}

	return (bufferPos - nvramBuffer);
}

// Will return 1 if not currently in bonus event.
WORD CMultiplierBonusEvent::GetCurrentMultiplier( void )
{
	// NOTE: TBD. The multilier we get from the poller is one less then we think it should be.
	//		 Make this change until this issue gets worked out.
//	WORD multiplier = 1;
	WORD multiplier = 0;

	m_bCurrentlyInEvent = IsCurrentlyInEventTime();

	if ( m_bCurrentlyInEvent )
	{
		multiplier = m_pointsMultiplier;
	}

	return multiplier;
}

bool CMultiplierBonusEvent::AwardPoints( DWORD newPoints )
{
	bool bPointsAwarded = false;

	// NOTE: TBD. The multilier we get from teh poller is one less then we think it should be.
	//		 Make this change until this issue gets worked out.
//	DWORD bonusPoints = (newPoints * GetCurrentMultiplier()) - newPoints;
	DWORD bonusPoints = newPoints * GetCurrentMultiplier();
	if ( bonusPoints > 0 )
	{
		AddBonusPoints( bonusPoints );
		bPointsAwarded = true;
	}

	return bPointsAwarded;
}


bool CMultiplierBonusEvent::AwardPartialPoints( DWORD countdownDecrement, DWORD divisor, DWORD multiplier)
{
	bool bPointsAwarded = false;

	if ( divisor < 1 )
		divisor = 1;

	DWORD product = (countdownDecrement * GetCurrentMultiplier()) + m_partialBonusingRemainder;

	// Get the partial bonus points awarded
	DWORD bonusPoints = product / divisor;
	if ( bonusPoints > 0 )
	{
		AddBonusPoints( bonusPoints * multiplier );
		bPointsAwarded = true;
	}
	
	// Now get the remainder.
	m_partialBonusingRemainder = (WORD)(product % divisor);

	return bPointsAwarded;
}

void CMultiplierBonusEvent::SessionReset( void )
{
	SetTransactionAsSent( false );
	SetBonusPoints( 0 );
}

void CMultiplierBonusEvent::ResetCurrentAwardedPoints(void)
{
	m_pointsAwardedCurrent = 0;
}

WORD CMultiplierBonusEvent::GetAwardedPointsCurrent(void)
{
	return m_pointsAwardedCurrent;
}
