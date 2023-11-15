#include "stdafx.h"

#include "AttendantPaidPayoutLog.h"
#include "../Hardware/MemoryStorage.h"
#include "../Utilities.h"

CAttendantPaidPayoutLog::CAttendantPaidPayoutLog( __int64 amount, JackpotTransactionType type )
{
	m_amount = amount;
	m_dateTime = CUtilities::GetCurrentTime();
	m_type = type;
}

CAttendantPaidPayoutLog::CAttendantPaidPayoutLog( byte *nvramBuffer)
{
	byte *bufferPos = nvramBuffer;

	m_dateTime = CUtilities::MakeTimeFromNvramBuffer(bufferPos, NVRAM_TIME_SIZE);
	bufferPos += NVRAM_TIME_SIZE;

	memcpy( &m_amount, bufferPos, sizeof( m_amount ) );
	bufferPos += sizeof( m_amount );

	memcpy( &m_type, bufferPos, sizeof( m_type ) );
}
	
CAttendantPaidPayoutLog::CAttendantPaidPayoutLog( CAttendantPaidPayoutLog &aRHS )
{
	*this = aRHS;
}

CAttendantPaidPayoutLog & CAttendantPaidPayoutLog::operator=( CAttendantPaidPayoutLog &aRHS )
{	
	m_dateTime = aRHS.GetDateTime( );
	m_amount = aRHS.GetAmount();
	m_type = aRHS.GetJackpotType();

	return *this;
}

CAttendantPaidPayoutLog::~CAttendantPaidPayoutLog(void)
{
}

int CAttendantPaidPayoutLog::GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength )
{
	byte *bufferPos = nvramBuffer;

	if ( nvramBufferLength >= (int)NVRAM_ATT_PAID_LOG_DATA_SIZE )
	{
		CUtilities::GetNvramBufferFromTime(m_dateTime, bufferPos, NVRAM_TIME_SIZE);
		bufferPos += NVRAM_TIME_SIZE;

		memcpy( bufferPos, &m_amount, sizeof( m_amount ) );
		bufferPos += sizeof( m_amount );
		
		memcpy( bufferPos, &m_type, sizeof( m_type ) );
		bufferPos += sizeof( m_type );
	}

	return (bufferPos - nvramBuffer);
}
