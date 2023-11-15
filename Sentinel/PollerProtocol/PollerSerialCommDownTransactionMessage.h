#pragma once
#include "PollerMessage.h"

class CPollerSerialCommDownTransactionMessage :
	public CPollerMessage
{
public:
	CPollerSerialCommDownTransactionMessage(CMeters *meters,
		byte transCode,
		DWORD uniqueId,
		const byte * cardID,
		byte EGMProtocol);

public:
	~CPollerSerialCommDownTransactionMessage(void);
public:

	byte * GetReadWriteBuffer( DWORD &bufferSize );
	void SetStatus(DWORD status);
	DWORD GetStatus();
	void SetReadWriteBuffer();

private:
	byte m_DataCount;

	bool m_bufferIsSet;
	CMeters *m_meters;
	const byte * m_cardID;
	byte m_meterCount;
	byte m_transactionCode;
	DWORD m_uniqueId;
	DWORD m_status;
	byte m_EGMProtocol;
};
