#pragma once
#include "PollerMessage.h"

class CPollerPinTransactionMessage :
	public CPollerMessage
{
public:
	CPollerPinTransactionMessage(CMeters *meters,
		byte transType,
		DWORD uniqueId,
		const byte * cardID,
		byte meterNumber,
		__int64 meterValue,
		__int64 hopperMeterValue);

public:
	~CPollerPinTransactionMessage(void);

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
	byte m_meterNumber;
	__int64 m_meterValue;
	__int64 m_hopperMeterValue;

	int m_dataCount;
	bool m_bBufferIsSet;
};
