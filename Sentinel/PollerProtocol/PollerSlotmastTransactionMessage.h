#pragma once
#include "PollerMessage.h"

const byte SLOTMAST_CHANGE_DATACOUNT = 1;

class CPollerSlotmastTransactionMessage :
	public CPollerMessage
{
public:
	CPollerSlotmastTransactionMessage(CMeters *meters, DWORD newSlotmastId, DWORD uniqueId, 
                                      const byte * cardID);
	byte * GetReadWriteBuffer( DWORD &bufferSize );
	void SetStatus(DWORD status);
	DWORD GetStatus();

public:
	~CPollerSlotmastTransactionMessage(void);

private:
	CMeters *m_meters;
	DWORD m_newSlotmastId;
	void SetReadWriteBuffer();
	byte m_transCode;
	DWORD m_uniqueId;
	int m_meterCount;
	DWORD m_status;
	const byte * m_cardID;
};
