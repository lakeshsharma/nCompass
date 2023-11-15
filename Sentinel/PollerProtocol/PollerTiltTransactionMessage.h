#pragma once
#include "PollerMessage.h"
#include "../TiltsDef.h"
#include "../Player.h"


class CPollerTiltTransactionMessage :
	public CPollerMessage
{
public:
	CPollerTiltTransactionMessage(CMeters *meters,
		byte transType,
		DWORD uniqueId,
		TiltCode tilt,
		const byte * cardID,
		DWORD machineNumber);
public:
	~CPollerTiltTransactionMessage(void);

public:
	byte * GetReadWriteBuffer( DWORD &bufferSize );
	void SetStatus(DWORD status);
	DWORD GetStatus();

private:
	byte m_DataCount;

	void SetReadWriteBuffer();
	CMeters *m_meters;
	const byte * m_cardID;
	byte m_meterCount;
	byte m_transCode;
	DWORD m_uniqueId;
	DWORD m_status;
	TiltCode m_tilt;
	DWORD m_machineNumber;
};
