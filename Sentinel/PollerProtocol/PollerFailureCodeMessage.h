#pragma once
#include "PollerMessage.h"

class CPollerFailureCodeMessage :
	public CPollerMessage
{
public:
	CPollerFailureCodeMessage(CMeters *meters,
		byte transCode,
		DWORD uniqueId,
		const byte * cardID,
		DWORD machineNumber,
		__int64 failureCode);

public:
	~CPollerFailureCodeMessage(void);
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
	DWORD m_machineNumber;
	__int64 m_failureCode;
};
