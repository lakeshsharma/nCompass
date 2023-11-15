#pragma once
#include "PollerMessage.h"
#include "../Games.h"


class CPollerSubGameMetersMessage :
	public CPollerMessage
{
public:
	CPollerSubGameMetersMessage(CGames *games, bool nfdSupported = false);
public:
	~CPollerSubGameMetersMessage(void);

	byte * GetReadWriteBuffer( long &bufferSize );

private:
	void SetReadWriteBuffer();

	BYTE m_meterCount;
	DWORD m_gameNumber;
	CGames *m_games;
	BYTE m_gameStatus;
	char *m_lpPaytableID;	
	char *m_lpMfgID;	
	char *m_lpAdditionalID;
	WORD m_wTheoreticalPayback;	
	bool m_nfdSupported;
};
