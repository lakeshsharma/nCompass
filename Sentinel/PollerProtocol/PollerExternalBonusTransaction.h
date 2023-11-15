#pragma once
#include "PollerMessage.h"
#include "../Meters.h"
#include "../InternalMessage.h"

class CPollerExternalBonusTransaction :
	public CPollerMessage
{
public:
	CPollerExternalBonusTransaction(CMeters *meters,
		byte transCode,
		DWORD uniqueId,
		const byte * cardID,
		__int64 amount
		);
	~CPollerExternalBonusTransaction();

	void SetReadWriteBuffer();
	byte * GetReadWriteBuffer(DWORD &bufferSize);
	void SetStatus(DWORD status);
	DWORD GetStatus();

private:
	__int64 m_amount;
	CMeters *m_meters;
	byte m_dataCount;
	byte m_meterCount;
	const byte * m_cardID;
	byte m_transCode;
	DWORD m_uniqueId;
	DWORD m_status;
	bool m_bufferIsSet;
};

