#include "stdafx.h"
#include "TiltLog.h"
#include "../Hardware/MemoryStorage.h"
#include "../Utilities.h"

using namespace std;

CTiltLog::CTiltLog( WORD code, time_t time )
{
	m_dateTime = time;
	m_code = code;
}

CTiltLog::CTiltLog( CTiltLog &tRHS )
{
	*this = tRHS;
}

CTiltLog & CTiltLog::operator=( CTiltLog &tRHS )
{	
	m_dateTime = tRHS.GetDateTime( );
	m_code = tRHS.GetTiltCode();

	return *this;
}

CTiltLog::CTiltLog( byte *nvramBuffer)
{
	byte *bufferPos = nvramBuffer;

	m_dateTime = CUtilities::MakeTimeFromNvramBuffer( bufferPos, NVRAM_TIME_SIZE );
	bufferPos += NVRAM_TIME_SIZE;

	memcpy( &m_code, bufferPos, sizeof( m_code ) );
}
	

CTiltLog::~CTiltLog(void)
{
}

int CTiltLog::GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength )
{
	byte *bufferPos = nvramBuffer;

	if ( nvramBufferLength >= (int)NVRAM_COMPOSITE_TILTS_LOG_DATA_SIZE )
	{
		CUtilities::GetNvramBufferFromTime( m_dateTime, bufferPos, NVRAM_TIME_SIZE );
		bufferPos += NVRAM_TIME_SIZE;

		memcpy( bufferPos, &m_code, sizeof( m_code ) );
		bufferPos += sizeof( m_code );
	}

	return (bufferPos - nvramBuffer);
}
