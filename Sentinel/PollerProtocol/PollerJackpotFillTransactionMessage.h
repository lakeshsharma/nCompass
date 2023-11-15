#pragma once
#include "PollerMessage.h"
#include "../Meters.h"
#include "../Player.h"

class CPollerJackpotFillTransactionMessage :
	public CPollerMessage
{
public:
	CPollerJackpotFillTransactionMessage(CMeters *meters,
		byte transCode,
		DWORD uniqueId,
		const byte * cardID,
		byte keypadEntryType,
		__int64 jackpotFill,
		DWORD machineNumber);

public:
	~CPollerJackpotFillTransactionMessage(void);
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
	byte m_keypadEntryType;
	__int64 m_jackpotFill;
	DWORD m_machineNumber;
};
