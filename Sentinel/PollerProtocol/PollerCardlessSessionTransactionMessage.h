
#pragma once
#include "PollerMessage.h"

class CPollerCardlessSessionTransactionMessage :
	public CPollerMessage
{
public:
	CPollerCardlessSessionTransactionMessage(CMeters *meters,
		byte transCode,
		DWORD uniqueId,
		DWORD playSessionId,
		DWORD playTxSeqNum,
		int gameNumber,
		int gameDenom,
		bool mgmdGlobal,
		bool mgmdAvailable);

public:
	~CPollerCardlessSessionTransactionMessage(void);

public:
	byte * GetReadWriteBuffer( DWORD &bufferSize );
	void SetStatus(DWORD status);
	DWORD GetStatus();

private:
	byte m_DataCount;

	void SetReadWriteBuffer();
	CMeters *m_meters;
	byte m_meterCount;
	byte m_transCode;
	DWORD m_uniqueId;
	DWORD m_status;
	DWORD m_playSessionId;
	DWORD m_playTxSeqNum;
	WORD m_gameNumber;
	WORD m_gameDenom;
	bool m_mgmdGlobal;
	bool m_mgmdAvailable;
};
