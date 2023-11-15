#pragma once
#include "PollerMessage.h"

class CPollerMachineMoveTransactionMessage :
	public CPollerMessage
{
public:
	CPollerMachineMoveTransactionMessage(CMeters *meters,
		byte transType,
		DWORD uniqueId,
		const byte * cardID,
		WORD denom, 
		WORD machineType,
		DWORD machineNumber);

public:
	~CPollerMachineMoveTransactionMessage(void);
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
	WORD m_denom;
	WORD m_machineType;
	DWORD m_machineNumber;

	int m_dataCount;
	bool m_bBufferIsSet;
};
