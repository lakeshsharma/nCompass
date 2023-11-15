#pragma once
#include "GlobalDefs.h"
#include "Log.h"

// UI_REWRITE_CLEANUP
class CMediaWindowLog :
	public CLog
{
public:
	CMediaWindowLog( DWORD code, byte *cardID );
	CMediaWindowLog( byte *nvramBuffer );
	CMediaWindowLog( CMediaWindowLog &pRHS );
	~CMediaWindowLog(void);

	CMediaWindowLog &operator=( CMediaWindowLog &rRHS );
	int GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength );

	byte *GetCardID()
	{
		return m_cardID;
	}

	DWORD GetMediaWindowCode( void )
	{
		return m_code;
	}

private:
	// Disable default constructor.
	CMediaWindowLog();

	byte m_cardID[CARD_ID_LEN];
	DWORD m_code; // MediaWindowCodeType, extendable with bit flags, etc.
};

