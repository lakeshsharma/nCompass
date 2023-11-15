#pragma once
#include "PollerMessage.h"
class CPollerPBResetPinTransactionMessage :
	public CPollerMessage
{
public:
	CPollerPBResetPinTransactionMessage(CMeters *meters,
		byte transType,
		DWORD uniqueID,
		const byte * cardID,
		int oldPin,	
		int newPin);
	~CPollerPBResetPinTransactionMessage();

	byte * GetReadWriteBuffer(DWORD &bufferSize);
	void SetStatus(DWORD status);
	DWORD GetStatus();
	void SetReadWriteBuffer();


private:
	CMeters *m_meters;
	byte m_meterCount;
	byte m_transCode;
	int m_oldPin;
	int m_newPin;
	DWORD m_uniqueId;
	DWORD m_status;
	const byte * m_cardID;

	int m_dataCount;
	bool m_bBufferIsSet;
};


