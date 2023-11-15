#pragma once
#include "PollerMessage.h"
#include "../Player.h"
#include "../Meters.h"

class CPollerCTTTransactionMessage :
	public CPollerMessage
{
public:
	CPollerCTTTransactionMessage(CMeters *meters,
		CPlayer *player,
		DWORD uniqueId,
		DWORD transferCode,
		DWORD transferCount,
		LONGLONG activeSeconds);

public:
	~CPollerCTTTransactionMessage(void);
public:

	byte * GetReadWriteBuffer( DWORD &bufferSize );
	void SetStatus(DWORD status);
	DWORD GetStatus();
	void SetReadWriteBuffer();

private:
	byte m_DataCount;

	bool m_bufferIsSet;
	CMeters *m_meters;
	CPlayer * m_player;
	byte m_meterCount;
	byte m_transactionCode;
	DWORD m_uniqueId;
	DWORD m_status;
	DWORD m_transferCode;
	DWORD m_transferCount;
	LONGLONG m_activeSeconds;
};
