#pragma once
#include "PollerMessage.h"

class CPollerMobileConnectTransactionMessage :
	public CPollerMessage
{
public:
	CPollerMobileConnectTransactionMessage(CMeters *meters,
		byte transType,
		DWORD uniqueID,
		const byte * cardID,
		string& msgJson);

public:
	~CPollerMobileConnectTransactionMessage(void);
public:
	byte * GetReadWriteBuffer( DWORD &bufferSize );
	void SetStatus(DWORD status);
	DWORD GetStatus();
	void SetReadWriteBuffer();

private:
	CMeters *m_meters;
	byte m_meterCount;
	DWORD m_status;
	byte m_transCode;
	DWORD m_uniqueId;
	const byte * m_cardID;

	string m_msgJson;

	byte m_dataCount;
	bool m_bBufferIsSet;
};
