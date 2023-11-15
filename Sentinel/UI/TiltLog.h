#pragma once
#include "Log.h"

class CTiltLog : public CLog
{
public:
	CTiltLog( WORD code, time_t time );
	CTiltLog( byte *nvramBuffer);
	CTiltLog( CTiltLog &tRHS );
	~CTiltLog(void);
	CTiltLog &operator=( CTiltLog &tRHS );

	int GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength );

	WORD GetTiltCode( void )
	{
		return m_code;
	}

private:
	WORD m_code;
};
