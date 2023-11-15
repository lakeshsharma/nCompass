#include "stdafx.h"

#include "BonusEvent.h"
#include "Hardware/MemoryStorage.h"
#include "Utilities.h"
#include "Logging/LogString.h"

CBonusEvent::CBonusEvent(int eventID)
	: m_nvramUsedLength(0)
	, m_bCurrentlyInEvent(false)
	, m_pointsAwardedCurrent(0)
	, m_bonusType(ONE_TIME_BONUS)
	, m_eventID(eventID)
	, m_bEventTransactionSent(0)
	, m_pointsAwarded(0)

{
}

CBonusEvent::CBonusEvent( byte *nvramBuffer)
{
	byte *bufferPos = nvramBuffer;

	memcpy( &m_bonusType, bufferPos, sizeof( m_bonusType ) );
	bufferPos += sizeof( m_bonusType );

	memcpy( &m_eventID, bufferPos, sizeof( m_eventID ) );
	bufferPos += sizeof( m_eventID );

	memcpy( &m_bEventTransactionSent, bufferPos, sizeof( m_bEventTransactionSent ) );
	bufferPos += sizeof( m_bEventTransactionSent );

	memcpy( &m_pointsAwarded, bufferPos, sizeof( m_pointsAwarded ) );
	bufferPos += sizeof( m_pointsAwarded );

	// Moved from CMultiplierBonusEvent constructor
	m_startTime = CUtilities::MakeTimeFromNvramBuffer( bufferPos, NVRAM_TIME_SIZE );
	bufferPos += NVRAM_TIME_SIZE;

	m_stopTime = CUtilities::MakeTimeFromNvramBuffer( bufferPos, NVRAM_TIME_SIZE );
	bufferPos += NVRAM_TIME_SIZE;

	memcpy( &m_bCurrentlyInEvent, bufferPos, sizeof( m_bCurrentlyInEvent ) );
	bufferPos += sizeof( m_bCurrentlyInEvent );

	memcpy(&m_pointsAwardedCurrent, bufferPos, sizeof(m_pointsAwardedCurrent));
	bufferPos += sizeof(m_pointsAwardedCurrent);

	m_nvramUsedLength = bufferPos - nvramBuffer;
}

CBonusEvent::~CBonusEvent(void)
{
}

int CBonusEvent::GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength )
{
	byte *bufferPos = nvramBuffer;

	if ( nvramBufferLength >= (int)NVRAM_PLAYER_BONUS_EVENT_SIZE )
	{
		memcpy( bufferPos, &m_bonusType, sizeof( m_bonusType ) );
		bufferPos += sizeof( m_bonusType );

		memcpy( bufferPos, &m_eventID, sizeof( m_eventID ) );
		bufferPos += sizeof( m_eventID );

		memcpy( bufferPos, &m_bEventTransactionSent, sizeof( m_bEventTransactionSent ) );
		bufferPos += sizeof( m_bEventTransactionSent );

		memcpy( bufferPos, &m_pointsAwarded, sizeof( m_pointsAwarded ) );
		bufferPos += sizeof( m_pointsAwarded );

		// Moved from CMultiplierBonusEvent 
		CUtilities::GetNvramBufferFromTime( m_startTime, bufferPos, NVRAM_TIME_SIZE );
		bufferPos += NVRAM_TIME_SIZE;

		CUtilities::GetNvramBufferFromTime( m_stopTime, bufferPos, NVRAM_TIME_SIZE );
		bufferPos += NVRAM_TIME_SIZE;

		memcpy( bufferPos, &m_bCurrentlyInEvent, sizeof( m_bCurrentlyInEvent ) );
		bufferPos += sizeof( m_bCurrentlyInEvent );

		memcpy(bufferPos, &m_pointsAwardedCurrent, sizeof(m_pointsAwardedCurrent));
		bufferPos += sizeof(m_pointsAwardedCurrent);
	}

	return (bufferPos - nvramBuffer);
}

bool CBonusEvent::IsCurrentlyInEventTime( void )
{
	bool bInEventTime = false;

	time_t currentTime = CUtilities::GetCurrentTime();
	if ( currentTime >= m_startTime && currentTime <= m_stopTime )
		bInEventTime = true;

	return bInEventTime;
}

int CBonusEvent::GetEventID( void ) const
{
	return m_eventID;
}

BonusEventType CBonusEvent::GetBonusEventType( void ) const
{
	return m_bonusType;
}
