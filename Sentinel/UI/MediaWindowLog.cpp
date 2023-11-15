#include "stdafx.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "MediaWindowLog.h"
#include "../Hardware/MemoryStorage.h"
#include "../Utilities.h"

CMediaWindowLog::CMediaWindowLog( DWORD code, byte *cardID )
{
	m_dateTime = CUtilities::GetCurrentTime();
	memcpy(m_cardID, cardID, sizeof m_cardID);
	m_code = code;
}

CMediaWindowLog::CMediaWindowLog( byte *nvramBuffer )
{
	byte *bufferPos = nvramBuffer;

	m_dateTime = CUtilities::MakeTimeFromNvramBuffer( bufferPos, NVRAM_TIME_SIZE );
	bufferPos += NVRAM_TIME_SIZE;

	memcpy( m_cardID, bufferPos, sizeof( m_cardID ) );
	bufferPos += sizeof( m_cardID );

	memcpy( &m_code, bufferPos, sizeof( m_code ) );
}

CMediaWindowLog::CMediaWindowLog( CMediaWindowLog &rRHS )
{
	*this = rRHS;
}

CMediaWindowLog::~CMediaWindowLog(void)
{
}

CMediaWindowLog & CMediaWindowLog::operator=( CMediaWindowLog &rRHS )
{	
	m_dateTime = rRHS.m_dateTime;
	memcpy(m_cardID, rRHS.m_cardID, sizeof m_cardID);
	m_code = rRHS.m_code;

	return *this;
}

int CMediaWindowLog::GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength )
{
	byte *bufferPos = nvramBuffer;

	if ( nvramBufferLength >= (int)NVRAM_REPAIR_CODES_LOG_DATA_SIZE )
	{
		CUtilities::GetNvramBufferFromTime( m_dateTime, bufferPos, NVRAM_TIME_SIZE );
		bufferPos += NVRAM_TIME_SIZE;

		memcpy( bufferPos, &m_cardID, sizeof( m_cardID ) );
		bufferPos += sizeof( m_cardID );

		memcpy( bufferPos, &m_code, sizeof( m_code ) );
		bufferPos += sizeof( m_code );
	}

	return (bufferPos - nvramBuffer);
}


