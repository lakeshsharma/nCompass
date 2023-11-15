#include "stdafx.h"

#include "PersonalBankerLog.h"
#include "../Hardware/MemoryStorage.h"
#include "../Utilities.h"
#include "Logging/LogString.h"

CPersonalBankerLog::CPersonalBankerLog( CPersonalBankerTransfer *pbTransfer, PersonalBankerTransferType transferType, 
	__int64 amount, const time_t dateTime, bool isPartial )
{
	m_dateTime = dateTime;
	m_amount = amount;
	m_source = transferType;
	m_bPartial = isPartial;
	m_bCashoutToHost = pbTransfer->IsCashoutToHostTransfer();

	LogString("GENERAL", "CPersonalBankerLog Amount: %lld PBTransfer Source Type: %d IsPartial: %d", 
		amount, m_source, isPartial);

	if( pbTransfer->IsTransferRequestCanceled() )
		m_bSuccessful = false;
	else
		m_bSuccessful = true;

	const byte *cardID = pbTransfer->GetCardNumber(m_cardIDLen);

	m_cardID = new byte[m_cardIDLen];
	memcpy( m_cardID, cardID, m_cardIDLen );
	
	m_transIDLength = pbTransfer->GetPBTIDLength();
	const char *transID = pbTransfer->GetID( );

	// Add terminating NUL character.
	m_transID = new char[m_transIDLength + 1];
	memcpy( m_transID, transID, m_transIDLength );
	m_transID[m_transIDLength] = 0;
	
	m_seqNum = pbTransfer->GetSequenceNumber();

	m_cancelCode = pbTransfer->GetCancelledType();

	m_bIsPending = true;
}

CPersonalBankerLog::CPersonalBankerLog( byte *nvramBuffer)
{

	byte *bufferPos = nvramBuffer;
	
	m_dateTime = CUtilities::MakeTimeFromNvramBuffer( bufferPos, NVRAM_TIME_SIZE );
	bufferPos += NVRAM_TIME_SIZE;

	memcpy( &m_amount, bufferPos, sizeof( m_amount ) );
	bufferPos += sizeof( m_amount );

	memcpy( &m_bSuccessful, bufferPos, sizeof( m_bSuccessful ) );
	bufferPos += sizeof( m_bSuccessful );

	memcpy( &m_source, bufferPos, sizeof( m_source ) );
	bufferPos += sizeof( m_source );

	memcpy( &m_seqNum, bufferPos, sizeof( m_seqNum ) );
	bufferPos += sizeof( m_seqNum );

	memcpy( &m_cardIDLen, bufferPos, sizeof( m_cardIDLen ) );
	bufferPos += sizeof( m_cardIDLen );

	if ( m_cardIDLen > 0 )
	{
		m_cardID = new byte [m_cardIDLen];
		memcpy( m_cardID, bufferPos, m_cardIDLen );
	}
	else
	{
		m_cardID = NULL;
	}
	bufferPos += m_cardIDLen;

	memcpy( &m_transIDLength, bufferPos, sizeof( m_transIDLength ) );
	bufferPos += sizeof( m_transIDLength );

	if ( m_transIDLength > 0 )
	{
		// Add terminating NUL character.
		m_transID = new char [m_transIDLength + 1];
		memcpy( m_transID, bufferPos, m_transIDLength );
		m_transID[m_transIDLength] = 0;
	}
	else
	{
		m_transID = NULL;
	}
	bufferPos += m_transIDLength;	
	
	memcpy( &m_bIsPending, bufferPos, sizeof( m_bIsPending ) );
	bufferPos += sizeof( m_bIsPending );

	memcpy( &m_cancelCode, bufferPos, sizeof( m_cancelCode ) );
	bufferPos += sizeof( m_cancelCode );

	byte checkbyte2 = 0;
	memcpy(&checkbyte2, bufferPos, sizeof(checkbyte2));
	bufferPos += sizeof(checkbyte2);

	if (NVRAM_PBT_LOG_LOGS_CHECKBYTE2_VALUE == checkbyte2)
	{
		memcpy(&m_bPartial, bufferPos, sizeof(m_bPartial));
		bufferPos += sizeof(m_bPartial);
		memcpy(&m_bCashoutToHost, bufferPos, sizeof(m_bCashoutToHost));
		bufferPos += sizeof(m_bCashoutToHost);
	}
	else
	{
		// The entry belongs to older PBLog and hence partial and
		// cashout-to-host fields should not be read from NVRAM
		m_bPartial = false;
		m_bCashoutToHost = false;
	}
}

CPersonalBankerLog::CPersonalBankerLog( CPersonalBankerLog &pRHS )
{
	*this = pRHS;
}

CPersonalBankerLog & CPersonalBankerLog::operator=( CPersonalBankerLog &pRHS )
{	
	m_dateTime = pRHS.GetDateTime( );
	m_amount = pRHS.GetAmount( );
	m_bSuccessful = pRHS.IsSuccessful( );
	m_source = pRHS.GetSource( );
	m_bIsPending = pRHS.IsPending();
	m_bPartial = pRHS.IsPartial();
	m_bCashoutToHost = pRHS.IsCashoutToHost();

	byte *cardID = pRHS.GetCardID( m_cardIDLen );

	if ( m_cardIDLen > 0 && cardID != NULL )
	{
		m_cardID = new byte [m_cardIDLen];
		memcpy( m_cardID, cardID, m_cardIDLen );
	}
	else
	{
		m_cardID = NULL;
		m_cardIDLen = 0;
	}

	char *transID = pRHS.GetTransID( m_transIDLength );

	if ( m_transIDLength > 0 && transID != NULL )
	{
		// Add terminating NUL character.
		m_transID = new char [m_transIDLength + 1];
		memcpy( m_transID, transID, m_transIDLength );
		m_transID[m_transIDLength] = 0;
	}
	else
	{
		m_transID = NULL;
		m_transIDLength = 0;
	}
	

	m_seqNum = pRHS.GetSequenceNumber( );	

	m_cancelCode = pRHS.GetCancellationType( );

	return *this;
}

CPersonalBankerLog::~CPersonalBankerLog(void)
{
	delete [] m_cardID;
	delete [] m_transID;
}

int CPersonalBankerLog::GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength )
{
	byte *bufferPos = nvramBuffer;

	if ( nvramBufferLength >= (int)NVRAM_PBT_LOG_LOG_DATA_SIZE )
	{
		CUtilities::GetNvramBufferFromTime( m_dateTime, bufferPos, NVRAM_TIME_SIZE );
		bufferPos += NVRAM_TIME_SIZE;

		memcpy( bufferPos, &m_amount, sizeof( m_amount ) );
		bufferPos += sizeof( m_amount );

		memcpy( bufferPos, &m_bSuccessful, sizeof( m_bSuccessful ) );
		bufferPos += sizeof( m_bSuccessful );

		memcpy( bufferPos, &m_source, sizeof( m_source ) );
		bufferPos += sizeof( m_source );

		memcpy( bufferPos, &m_seqNum, sizeof( m_seqNum ) );
		bufferPos += sizeof( m_seqNum );

		memcpy( bufferPos, &m_cardIDLen, sizeof( m_cardIDLen ) );
		bufferPos += sizeof( m_cardIDLen );

		memcpy( bufferPos, m_cardID, m_cardIDLen );
		bufferPos += m_cardIDLen;

		memcpy( bufferPos, &m_transIDLength, sizeof( m_transIDLength ) );
		bufferPos += sizeof( m_transIDLength );

		memcpy( bufferPos, m_transID, m_transIDLength );
		bufferPos += m_transIDLength;
		
		memcpy( bufferPos, &m_bIsPending, sizeof( m_bIsPending ) );
		bufferPos += sizeof( m_bIsPending );

		memcpy( bufferPos, &m_cancelCode, sizeof( m_cancelCode ) );
		bufferPos += sizeof( m_cancelCode );

		const byte checkByte2(NVRAM_PBT_LOG_LOGS_CHECKBYTE2_VALUE);
		memcpy(bufferPos, &checkByte2, sizeof(checkByte2));
		bufferPos += sizeof(checkByte2);

		memcpy(bufferPos, &m_bPartial, sizeof(m_bPartial));
		bufferPos += sizeof(m_bPartial);

		memcpy(bufferPos, &m_bCashoutToHost, sizeof(m_bCashoutToHost));
		bufferPos += sizeof(m_bCashoutToHost);
	}

	else
	{
		LogString(ERROR_LOG, "Buffer length is less than the size required for NVRAM PBT LOG");
	}

	return (bufferPos - nvramBuffer);
}
