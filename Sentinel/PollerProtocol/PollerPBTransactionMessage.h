#pragma once
#include "PollerMessage.h"
#include "../PersonalBankerTransfer.h"

class CPollerPBTransactionMessage :
	public CPollerMessage
{
public:
	CPollerPBTransactionMessage(CMeters *meters,
		CPersonalBankerTransfer * PBTrans,
		byte transType,
		time_t transTime,
		DWORD uniqueId,
		const byte * cardID,
		PersonalBankerType ammendedPBtype);

public:
	~CPollerPBTransactionMessage(void);
public:
	byte * GetReadWriteBuffer( DWORD &bufferSize );
	void SetStatus(DWORD status);
	DWORD GetStatus();
	void SetReadWriteBuffer();
	bool IsEligibleToResend();

private:
	CMeters *m_meters;
	CPersonalBankerTransfer *m_PBTransfer;
	byte m_meterCount;
	byte m_transCode;
	DWORD m_uniqueId;
	time_t m_transTime;
	DWORD m_status;
	byte * m_cardID;
	bool m_RWBufferIsSet;
	int m_DataCount;
	PersonalBankerType m_ammendedPBType;

};
