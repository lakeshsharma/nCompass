#pragma once
#include "PollerMessage.h"
#include "Jackpot.h"

class CPollerUJPResultTransactionMessage :
	public CPollerMessage
{
public:
	CPollerUJPResultTransactionMessage(CMeters *meters,
		byte transType,
		DWORD uniqueID,
		const byte * cardID, 
		DWORD jackpotID,
		UnattendedJackpotStatus::Type jackpotStatus,
		__int64 amount);
	~CPollerUJPResultTransactionMessage();

	byte * GetReadWriteBuffer(DWORD &bufferSize);
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

	DWORD m_jackpotID;
	UnattendedJackpotStatus::Type m_jackpotStatus;

	int m_dataCount;
	bool m_bBufferIsSet;

	__int64 m_amount;

};

