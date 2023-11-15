#pragma once
#include "PollerMessage.h"

class CPollerBillAcceptedTransactionMessage :
	public CPollerMessage
{
public:
	CPollerBillAcceptedTransactionMessage(CMeters *meters,
		byte transType,
		DWORD uniqueId,
		const byte * cardID,
		WORD billType,
		__int64 meterValue,
		__int64 billCount);

public:
	~CPollerBillAcceptedTransactionMessage(void);

public:
	byte * GetReadWriteBuffer( DWORD &bufferSize );
	void SetStatus(DWORD status);
	DWORD GetStatus();
	void SetReadWriteBuffer();

private:
	CMeters *m_meters;
	byte m_meterCount;
	byte m_transCode;
	DWORD m_uniqueId;
	DWORD m_status;
	const byte * m_cardID;

	WORD m_billType;
	__int64 m_meterValue;
	__int64 m_billCount;

	int m_dataCount;
	bool m_bBufferIsSet;
};
