#pragma once
#include "PollerMessage.h"
#include "../Games.h"

class CPollerMetersMessage :
	public CPollerMessage
{
public:
	CPollerMetersMessage(CGames * games, bool nfdSupported = false,
		DWORD playSessionId = 0, DWORD playTxSeqNum = 0);
public:
	~CPollerMetersMessage( void );

	byte * GetReadWriteBuffer( long &bufferSize );
private:

	void SetReadWriteBuffer();

	CGames *m_games;
	byte m_meterCount;
	bool m_nfdSupported;
	DWORD m_playSessionId;
	DWORD m_playTxSeqNum;
};
