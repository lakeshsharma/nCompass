#include "stdafx.h"
#include "BonusEvents.h"

// Sentinel includes.
#include "Card.h"
#include "Logging/LogString.h"
#include "OneTimeBonusEvent.h"
#include "MultiplierBonusEvent.h"

CBonusEvents::CBonusEvents(MEMORY_TYPE memoryType, DWORD memoryOffset)
{
	m_type = memoryType;

	if ( memoryType == MEMORY_NVRAM )
	{
		m_dwAllowedSize = NVRAM_PLAYER_BONUS_EVENTS_SIZE;
		m_dwOffset = memoryOffset;
		m_bOffsetIsSet = true;
	}

	m_cardNumber = NULL;
	m_cardNumberLen = 0;

	m_oneTimeBonusToSendCount = 0;

	StoreOverheadDataToNVRAM();
}

CBonusEvents::CBonusEvents(const CBonusEvents &eventsRHS, MEMORY_TYPE memoryType, DWORD memoryOffset)
{
	m_type = memoryType;

	if ( memoryType == MEMORY_NVRAM )
	{
		m_dwAllowedSize = NVRAM_PLAYER_BONUS_EVENTS_SIZE;
		m_dwOffset = memoryOffset;
		m_bOffsetIsSet = true;
	}

	m_cardNumber = NULL;
	m_cardNumberLen = 0;

	*this = eventsRHS;
}

CBonusEvents::~CBonusEvents(void)
{
	ClearEvents();
}


CBonusEvents & CBonusEvents::operator=( const CBonusEvents &eventsRHS )
{
	int cardNumLen;
	const byte *cardNum = eventsRHS.GetCardNumber( cardNumLen );
	SetCardNumber( cardNum, cardNumLen );
	m_oneTimeBonusToSendCount = 0;

	for ( int ei = 0; ei < eventsRHS.GetBonusEventsCount(); ei++ )
	{
		const CBonusEvent *bonusEvent = eventsRHS.GetBonusEvent( ei );

		switch ( bonusEvent->GetBonusEventType() )
		{
		case ONE_TIME_BONUS:
			{
				COneTimeBonusEvent *oneTimeBonus = new COneTimeBonusEvent( *(COneTimeBonusEvent *)bonusEvent );
				AddEvent( oneTimeBonus );
				if ( !oneTimeBonus->WasTransactionsSent() )
					m_oneTimeBonusToSendCount++;
			}
			break;
		case MULTIPLIER_BONUS:
			{
				CMultiplierBonusEvent *multiplierBonus = new CMultiplierBonusEvent( *(CMultiplierBonusEvent *)bonusEvent );
				AddEvent( multiplierBonus );
			}
			break;
		}
	}

	StoreOverheadDataToNVRAM();

	return *this;
}

void CBonusEvents::MergeEvents( const CBonusEvents *events )
{
	if (events != NULL)
	{
		for ( int ei = 0; ei < events->GetBonusEventsCount(); ei++ )
		{
			bool bAddEvent = true;
			const CBonusEvent *checkBonusEvent = events->GetBonusEvent( ei );

			// Go through all the existing events and see if there is a match
			for ( int ti = 0; ti < GetBonusEventsCount() && bAddEvent; ti++ )
			{
				const CBonusEvent *thisBonusEvent = events->GetBonusEvent( ti );

				if ( thisBonusEvent->GetEventID() == checkBonusEvent->GetEventID() &&
					thisBonusEvent->GetStartTime() == checkBonusEvent->GetStartTime() &&
					thisBonusEvent->GetStopTime() == checkBonusEvent->GetStopTime() )
				{
					bAddEvent = false;
				}
			}

			if ( bAddEvent )
			{
				switch ( checkBonusEvent->GetBonusEventType() )
				{
				case ONE_TIME_BONUS:
					{
						COneTimeBonusEvent *oneTimeBonus = new COneTimeBonusEvent( *(COneTimeBonusEvent *)checkBonusEvent );
						AddEvent( oneTimeBonus );
						if ( !oneTimeBonus->WasTransactionsSent() )
							m_oneTimeBonusToSendCount++;
					}
					break;
				case MULTIPLIER_BONUS:
					{
						CMultiplierBonusEvent *multiplierBonus = new CMultiplierBonusEvent( *(CMultiplierBonusEvent *)checkBonusEvent );
						AddEvent( multiplierBonus );
					}
					break;
				}
			}
		}

		StoreOverheadDataToNVRAM();
	}
}

// NOTE: Keep this function static.
bool CBonusEvents::DoBonusEventsExistInNVRAM( void )
{
	bool bExists = CMemoryStorage::IntegrityCheck() && IsActiveLocation( NVRAM_PLAYER_BONUS_EVENTS_CHECKBYTE_OFFSET, NVRAM_PLAYER_BONUS_EVENTS_CHECKBYTE_VALUE );

	if ( !bExists )
		SetInactiveLocation( NVRAM_PLAYER_BONUS_EVENTS_CHECKBYTE_OFFSET,
			(BYTE)~NVRAM_PLAYER_BONUS_EVENTS_CHECKBYTE_VALUE );

	return bExists;
}

void CBonusEvents::BuildYourself()
{
	if ( m_type == MEMORY_NVRAM )
	{
		byte nvramBuffer[NVRAM_MAX_READ];

		if ( !Read( NVRAM_PLAYER_BONUS_EVENTS_DATA_OFFSET, nvramBuffer, NVRAM_PLAYER_BONUS_EVENTS_DATA_SIZE ) )
			LogString(GENERAL, "Could not read bonus events data from NVRAM" );

		byte *bufferPos = nvramBuffer;

		int eventCount;
		memcpy( &eventCount, bufferPos, sizeof( eventCount ) );
		bufferPos += sizeof( eventCount );
		if ( eventCount > (int)NVRAM_MAX_BONUS_EVENTS )
			eventCount = NVRAM_MAX_BONUS_EVENTS;

		memcpy( &m_cardNumberLen, bufferPos, sizeof( m_cardNumberLen ) );
		bufferPos += sizeof( m_cardNumberLen );
		memcpy( m_cardNumber, bufferPos, m_cardNumberLen );
		bufferPos += MAX_CARD_NUMBER_LENGTH;
		memcpy( &m_oneTimeBonusToSendCount, bufferPos, sizeof( m_oneTimeBonusToSendCount ) );
		bufferPos += sizeof( m_oneTimeBonusToSendCount );

		ClearEvents();
		for ( int ei = 0; ei < eventCount; ei++ )
		{
			if ( !Read( NVRAM_PLAYER_BONUS_EVENTS_DATA_OFFSET, nvramBuffer, NVRAM_PLAYER_BONUS_EVENTS_DATA_SIZE ) )
				LogString(GENERAL, "Could not read bonus event %d from NVRAM", ei );

			bufferPos = nvramBuffer;

			BonusEventType bonusType;
			memcpy( &bonusType, bufferPos, sizeof( bonusType ) );
			// NOTE; Don't increment pointer here.

			switch ( bonusType )
			{
				case ONE_TIME_BONUS:
					{
						COneTimeBonusEvent *oneTimeBonus = new COneTimeBonusEvent( bufferPos );
						AddEvent( oneTimeBonus );
						if ( !oneTimeBonus->WasTransactionsSent() )
							m_oneTimeBonusToSendCount++;
					}
					break;
				case MULTIPLIER_BONUS:
					{
						CMultiplierBonusEvent *multiplierBonus = new CMultiplierBonusEvent( bufferPos );
						AddEvent( multiplierBonus );
					}
					break;
			}

			bufferPos += NVRAM_PLAYER_BONUS_EVENT_SIZE;
		}
	}
}

void CBonusEvents::AddEvent( CBonusEvent *bonusEvent )
{
	if ( m_bonusEvents.size() < NVRAM_MAX_BONUS_EVENTS )
	{
		m_bonusEvents.push_back( bonusEvent );

		if ( bonusEvent->GetBonusEventType() == ONE_TIME_BONUS )
		{
			m_oneTimeBonusToSendCount++;	// This provides a quick way to tell if we have one time bonus events to send
		}

		StoreOverheadDataToNVRAM();
		StoreEventToNVRAM(m_bonusEvents.size() - 1);
	}
	else
	{
		LogString(ERROR_LOG, "Error - Maximum bonus events reached" );
	}
}

void CBonusEvents::ClearEvents( void )
{
	for ( int bi = 0; bi < (int)m_bonusEvents.size(); bi++ )
	{
		CBonusEvent *bonusEvent = (CBonusEvent *)m_bonusEvents[ bi ];
		if ( bonusEvent != NULL )
		{
			delete bonusEvent;
		}
	}

	if ( m_cardNumber != NULL )
	{
		delete [] m_cardNumber;
		m_cardNumber = NULL;
	}
	m_cardNumberLen = 0;
	m_oneTimeBonusToSendCount = 0;

	m_bonusEvents.clear();

	StoreOverheadDataToNVRAM();
}

// Returns true if any bonus points have been awarded.
bool CBonusEvents::AwardBonusPoints( DWORD newPoints )
{
	bool bPointsAwarded = false;

	for ( int bi = 0; bi < (int)m_bonusEvents.size(); bi++ )
	{
		CBonusEvent *bonusEvent = (CBonusEvent *)m_bonusEvents[ bi ];
		if ( bonusEvent->AwardPoints( newPoints ) )
		{
			bPointsAwarded = true;
			StoreEventToNVRAM(bi);
		}
	}

	return bPointsAwarded;
}

// Returns true if any bonus points have been awarded.
bool CBonusEvents::AwardPartialBonusPoints( DWORD countdownDecrement, DWORD divisor, DWORD multiplier )
{
	bool bPointsAwarded = false;

	for ( int bi = 0; bi < (int)m_bonusEvents.size(); bi++ )
	{
		CBonusEvent *bonusEvent = (CBonusEvent *)m_bonusEvents[ bi ];
		if ( bonusEvent->AwardPartialPoints( countdownDecrement, divisor, multiplier ) )
		{
			bPointsAwarded = true;
			StoreEventToNVRAM(bi);
		}
	}

	return bPointsAwarded;
}

DWORD CBonusEvents::GetBonusPoints( void )
{
	DWORD totalBonusPoints = 0;

	for ( int bi = 0; bi < (int)m_bonusEvents.size(); bi++ )
	{
		CBonusEvent *bonusEvent = (CBonusEvent *)m_bonusEvents[ bi ];
		totalBonusPoints += bonusEvent->GetAwardedPoints();
	}

	return totalBonusPoints;
}

void CBonusEvents::ClearBonusPoints( void )
{
	for ( int bi = 0; bi < (int)m_bonusEvents.size(); bi++ )
	{
		CBonusEvent *bonusEvent = (CBonusEvent *)m_bonusEvents[ bi ];
		bonusEvent->SetBonusPoints( 0 );
		StoreEventToNVRAM(bi);
	}
}

CBonusEvent * CBonusEvents::GetBonusEvent( int eventIndex )
{
	// Use the const method to get a pointer.
	const CBonusEvents *constThis = const_cast<const CBonusEvents *>(this);
	return const_cast<CBonusEvent *>(constThis->GetBonusEvent(eventIndex));
}

const CBonusEvent * CBonusEvents::GetBonusEvent( int eventIndex ) const
{
	CBonusEvent *bonusEvent = NULL;

	if ( eventIndex < (int)m_bonusEvents.size() )
	{
		bonusEvent = (CBonusEvent *)m_bonusEvents[eventIndex];
	}

	return bonusEvent;
}

CBonusEvent * CBonusEvents::GetOutstandingBonusEvent( bool bIsCardOutTime )
{
	CBonusEvent *outstandingBonusEvent = NULL;

	if ( m_oneTimeBonusToSendCount > 0 || bIsCardOutTime )
	{
		// Find the first event that transactions need to be sent for
		for ( int bi = 0; bi < (int)m_bonusEvents.size() && outstandingBonusEvent == NULL; bi++ )
		{
			CBonusEvent *bonusEvent = (CBonusEvent *)m_bonusEvents[ bi ];

			// Find out if we need to send transactions for this event.
			// Transactions are to be sent if they were not already sent and pointes were awarded.
			// For multiplier bonus, only send at card out time.
			if ( (bIsCardOutTime || bonusEvent->GetBonusEventType() == ONE_TIME_BONUS ) &&
				  bonusEvent->WerePointsAwared() && 
				  !bonusEvent->WasTransactionsSent() )
			{
				outstandingBonusEvent = bonusEvent;
				bonusEvent->SetTransactionAsSent();		// Mark this event as sent

				if ( bonusEvent->GetBonusEventType() == ONE_TIME_BONUS )
				{
					m_oneTimeBonusToSendCount--;
					StoreOverheadDataToNVRAM();
				}

				StoreEventToNVRAM(bi);
			}
		}
	}

	return outstandingBonusEvent;
}

// this will reset the bonus events for when a session is reset
void CBonusEvents::SessionReset( void )
{
	for ( int bi = 0; bi < (int)m_bonusEvents.size(); bi++ )
	{
		CBonusEvent *bonusEvent = (CBonusEvent *)m_bonusEvents[ bi ];

		// Find out if we need to send transactions for this event.
		// Transactions are to be sent if they were not already sent and pointes were awarded.
		// For multiplier bonus, only send at card out time.
		if ( bonusEvent->GetBonusEventType() == MULTIPLIER_BONUS )
		{
			((CMultiplierBonusEvent *)bonusEvent)->SessionReset();

			StoreEventToNVRAM(bi);
		}
	}

}

void CBonusEvents::SetCardNumber( const byte *cardNumber, int cardNumberLen )
{
	if ( m_cardNumber != NULL )
		delete [] m_cardNumber;

	if ( cardNumberLen > 0 && cardNumber != NULL )
	{
		m_cardNumber = new byte[cardNumberLen];
		memcpy( m_cardNumber, cardNumber, cardNumberLen );
		m_cardNumberLen = cardNumberLen;
	}
	else
	{
		m_cardNumber = NULL;
		m_cardNumberLen = 0;
	}

	StoreOverheadDataToNVRAM();
}

const byte * CBonusEvents::GetCardNumber( int &cardNumberLen ) const
{
	cardNumberLen = m_cardNumberLen;

	return m_cardNumber;
}

void CBonusEvents::StoreOverheadDataToNVRAM( void )
{
	if ( m_type == MEMORY_NVRAM )
	{
		byte nvramBuffer[NVRAM_PLAYER_BONUS_EVENTS_DATA_SIZE];
		byte *bufferPos = nvramBuffer;

		int eventCount = m_bonusEvents.size();
		memcpy( bufferPos, &eventCount, sizeof( eventCount ) );
		bufferPos += sizeof( eventCount );

		memcpy( bufferPos, &m_cardNumberLen, sizeof( m_cardNumberLen ) );
		bufferPos += sizeof( m_cardNumberLen );

		memcpy( bufferPos, m_cardNumber, m_cardNumberLen );
		bufferPos += MAX_CARD_NUMBER_LENGTH;

		memcpy( bufferPos, &m_oneTimeBonusToSendCount, sizeof( m_oneTimeBonusToSendCount ) );
		bufferPos += sizeof( m_oneTimeBonusToSendCount );

		int bytesInBuffer = bufferPos - nvramBuffer;
		if ( !Write( NVRAM_PLAYER_BONUS_EVENTS_DATA_OFFSET, nvramBuffer, bytesInBuffer ) )
			LogString(ERROR_LOG, "Could not write bonus events data to NVRAM" );
	}
}


void CBonusEvents::StoreEventToNVRAM( int eventIndex )
{
	if (  m_type == MEMORY_NVRAM && eventIndex < (int)m_bonusEvents.size() )
	{
		CBonusEvent *bonusEvent = (CBonusEvent *)m_bonusEvents[ eventIndex ];

		byte nvramBuffer[NVRAM_PLAYER_BONUS_EVENT_SIZE];
		int bytesInBuffer = bonusEvent->GetNvramBuffer( nvramBuffer, NVRAM_PLAYER_BONUS_EVENT_SIZE );

		if ( !Write( NVRAM_PLAYER_BONUS_EVENT_OFFSET + (NVRAM_PLAYER_BONUS_EVENT_SIZE * eventIndex), nvramBuffer, bytesInBuffer ) )
			LogString(ERROR_LOG, "Could not write bonus event to NVRAM" );

	}
}

DWORD CBonusEvents::GetBonusPointsCurrent(void) const
{
	DWORD totalBonusPoints = 0;
	for (int bi = 0; bi < (int)m_bonusEvents.size(); bi++)
	{
		CBonusEvent *bonusEvent = (CBonusEvent *)m_bonusEvents[bi];
		totalBonusPoints += bonusEvent->GetAwardedPointsCurrent();
	}
	return totalBonusPoints;
}

void CBonusEvents::ResetCurrentAwardedPoints(void)
{
	for (int bi = 0; bi < (int)m_bonusEvents.size(); bi++)
	{
		CBonusEvent *bonusEvent = (CBonusEvent *)m_bonusEvents[bi];
		bonusEvent->ResetCurrentAwardedPoints();
		StoreEventToNVRAM(bi);
	}
}

