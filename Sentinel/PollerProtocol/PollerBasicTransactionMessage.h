#pragma once
#include "PollerMessage.h"

class CPollerBasicTransactionMessage :
	public CPollerMessage
{
public:
	CPollerBasicTransactionMessage(CMeters *meters, byte transType, 
                                   DWORD uniqueId, const byte * cardID, SYSTEMTIME transTime);
public:
	~CPollerBasicTransactionMessage(void);

public:
	byte * GetReadWriteBuffer( DWORD &bufferSize );
	void SetStatus(DWORD status);
	DWORD GetStatus();
	void SetReadWriteBuffer();

private:

	bool m_bufferIsSet;

	CMeters *m_meters;
	byte m_meterCount;
	byte m_transCode;
	DWORD m_uniqueId;
	DWORD m_status;
	const byte * m_cardID;
	SYSTEMTIME m_transTime;
};
