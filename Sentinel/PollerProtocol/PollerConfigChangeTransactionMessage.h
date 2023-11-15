#pragma once
#include "PollerMessage.h"

class CPollerConfigChangeTransactionMessage :
	public CPollerMessage
{
public:
	CPollerConfigChangeTransactionMessage(CMeters *meters,
		byte transCode,
		DWORD uniqueId,
		const byte * cardID,
		DWORD configChangeType,
		__int64 valueBefore,
		__int64 valueAfter,
		byte pinNumber);

public:
	~CPollerConfigChangeTransactionMessage(void);
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

	DWORD m_configChangeType;
	__int64 m_valueBefore;
	__int64 m_valueAfter;
	byte m_pinNumber;

};
