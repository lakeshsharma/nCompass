#pragma once
#include "PollerMessage.h"

class CPollerEGMSnapshotTransactionMessage :
	public CPollerMessage
{
public:
	CPollerEGMSnapshotTransactionMessage(CMeters *meters,
		byte transType,
		SYSTEMTIME transTime,
		DWORD uniqueId,
		SnapshotType type,
		bool gameCommDown,
		const byte * cardID);
public:
	~CPollerEGMSnapshotTransactionMessage(void);

	byte * GetReadWriteBuffer( DWORD &bufferSize );
	void SetStatus(DWORD status);
	DWORD GetStatus();
	void SetReadWriteBuffer();

private:
	byte m_DataCount;

	CMeters *m_meters;
	const byte * m_cardID;
	byte m_meterCount;
	byte m_transCode;
	DWORD m_uniqueId;
	DWORD m_status;
	SnapshotType m_type;
	bool m_bGameCommDown;

	bool m_rwBufferIsSet;
	SYSTEMTIME m_transTime;
};
