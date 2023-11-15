#pragma once
#include "Log.h"


class CRepairLog:
	public CLog
{
public:
	CRepairLog( DWORD code );
	CRepairLog( byte *nvramBuffer );
	CRepairLog( CRepairLog &pRHS );
	virtual ~CRepairLog(void);

	CRepairLog &operator=( CRepairLog &rRHS );
	int GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength );

	DWORD GetRepairCode( void )
	{
		return m_code;
	}

private:
	DWORD m_code;
};
