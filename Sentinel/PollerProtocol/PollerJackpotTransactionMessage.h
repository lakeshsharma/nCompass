#pragma once
#include "PollerMessage.h"

class CPollerJackpotTransactionMessage :
	public CPollerMessage
{
public:
	CPollerJackpotTransactionMessage(CMeters *meters,
		byte transCode,
		DWORD uniqueId,
		const byte * cardID,
		__int64 amount,
		DWORD coinsPlayed,
		DWORD jackpotType,
		DWORD progressiveLevel,
		DWORD machineNumber,
		bool keyToCreditAvailable);

public:
	~CPollerJackpotTransactionMessage(void);
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
	__int64 m_amount;
	DWORD m_coinsPlayed;
	DWORD m_jackpotType;
	DWORD m_progressiveLevel;
	DWORD m_machineNumber;
	bool m_keyToCreditAvailable;
};
