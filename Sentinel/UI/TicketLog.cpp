#include "stdafx.h"
#include "TicketLog.h"
#include "../Hardware/MemoryStorage.h"
#include "../Utilities.h"

using namespace std;

CTicketLog::CTicketLog( const CTicket &ticket )
{
	m_amount = ticket.Amount();

	byte validationLength = 0;
	byte *validationNumber;

	const byte * temp = ticket.ValidationNumber(validationLength);

	validationNumber = new byte[VALIDATION_NUMBER_LENGTH];

	validationLength = min(validationLength, VALIDATION_NUMBER_LENGTH); // Store as much as we can.

	// Leave m_validationNum as empty string, unless temp != NULL.
	if (temp != NULL)
	{
		memcpy(validationNumber, temp, validationLength);
		m_validationNum = CUtilities::MakeStringFromBCD(validationNumber, validationLength);

	}

	m_sequenceNum = ticket.Number();

	m_dateTime = ticket.GameDateTime();
		
	m_isTicketIn = ticket.IsTicketIn();
		
	m_ticketAction = ticket.ActionCode();

	delete [] validationNumber;
}

CTicketLog::CTicketLog( const byte *nvramBuffer)
{
	const byte *bufferPos = nvramBuffer;

	m_dateTime = CUtilities::MakeTimeFromNvramBuffer(bufferPos, NVRAM_TIME_SIZE);
	bufferPos += NVRAM_TIME_SIZE;

	memcpy( &m_amount, bufferPos, sizeof( m_amount ) );
	bufferPos += sizeof( m_amount );

	memcpy( &m_isTicketIn, bufferPos, sizeof( m_isTicketIn ) );
	bufferPos += sizeof( m_isTicketIn );

	memcpy( &m_ticketAction, bufferPos, sizeof( m_ticketAction ) );
	bufferPos += sizeof( m_ticketAction );

	char validationNumber[VALIDATION_NUMBER_LENGTH];

	memcpy( &validationNumber, bufferPos, VALIDATION_NUMBER_LENGTH );

	m_validationNum = validationNumber;

	bufferPos += VALIDATION_NUMBER_LENGTH;

	memcpy( &m_sequenceNum, bufferPos, sizeof( m_sequenceNum ) );
}

CTicketLog::CTicketLog( const CTicketLog &tRHS )
{
	*this = tRHS;
}

CTicketLog & CTicketLog::operator=( const CTicketLog &tRHS )
{	
	m_dateTime = tRHS.GetDateTime();
	m_amount = tRHS.GetAmount();
	m_isTicketIn = tRHS.GetTicketIn();
	m_validationNum = tRHS.GetValidationNum();
	m_sequenceNum = tRHS.GetSequenceNum();
	m_ticketAction = tRHS.GetTicketActionCode();	

	return *this;
}

CTicketLog::~CTicketLog(void)
{
}

int CTicketLog::GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength ) const
{
	byte *bufferPos = nvramBuffer;

	if (nvramBuffer == NULL)
	{
		nvramBufferLength = 0;
	}

	// Fix signed/unsigned comparison issue with NVRAM_TICKET_LOG_DATA_SIZE.
	if (nvramBufferLength < 0)
	{
		nvramBufferLength = 0;
	}

	if ( nvramBufferLength >= (int)NVRAM_TICKET_LOG_DATA_SIZE )
	{
		CUtilities::GetNvramBufferFromTime( m_dateTime, bufferPos, NVRAM_TIME_SIZE );
		bufferPos += NVRAM_TIME_SIZE;

		memcpy( bufferPos, &m_amount, sizeof( m_amount ) );
		bufferPos += sizeof( m_amount );

		memcpy( bufferPos, &m_isTicketIn, sizeof( m_isTicketIn ) );
		bufferPos += sizeof( m_isTicketIn );

		memcpy( bufferPos, &m_ticketAction, sizeof( m_ticketAction ) );
		bufferPos += sizeof( m_ticketAction );

		char validationNumber[VALIDATION_NUMBER_LENGTH];
		CUtilities::ConvertCharsToChars(m_validationNum, validationNumber, VALIDATION_NUMBER_LENGTH);
		memcpy( bufferPos, &validationNumber, VALIDATION_NUMBER_LENGTH );
		bufferPos += VALIDATION_NUMBER_LENGTH;

		memcpy( bufferPos, &m_sequenceNum, sizeof( m_sequenceNum ) );
		bufferPos += sizeof( m_sequenceNum );
	}

	return (bufferPos - nvramBuffer);
}

string CTicketLog::GetTicketInActionText() const
{
	string retStr;

	if( m_ticketAction != VendComplete )
	{
		retStr = "Rejected";
		
	}
	else
	{
		retStr = "Redeemed";
	}

	return retStr;
}

__int64 CTicketLog::GetAmount( void ) const
{
	return (m_amount);
}

bool CTicketLog::GetTicketIn( void ) const
{
	return (m_isTicketIn);
}

string CTicketLog::GetValidationNum() const
{
	return (m_validationNum);
}

string CTicketLog::GetMaskedValidationNum( void ) const
{
	// Mask the Valication Number
	// Get the last 5 digits off the original string	
	int length(m_validationNum.length());
	string number = m_validationNum;
	if (length > NUMBER_OF_DIGITS_TO_MASK)
	{
		number = m_validationNum.substr(length - NUMBER_OF_DIGITS_TO_MASK, NUMBER_OF_DIGITS_TO_MASK);

		// Replace all but the last 5 digits off the original string with the mask character.
		for (int pos = 0; pos < (length - NUMBER_OF_DIGITS_TO_MASK); pos++)
		{
			number.insert(pos, 1, '*');
		}
	}	

	return number;
}

int CTicketLog::GetSequenceNum( void ) const	   
{
	return m_sequenceNum;
}

TicketActionCode CTicketLog::GetTicketActionCode( void ) const
{
	return m_ticketAction;
}
