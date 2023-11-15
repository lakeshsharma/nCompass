#pragma once
#include "PollerMessage.h"

class ExcessiveVoucherOutTests;

class CPollerGenericTransactionMessage : public CPollerMessage
{
public:
	CPollerGenericTransactionMessage(CMeters *meters,
		byte transCode,
		DWORD uniqueID,
		const byte * cardID,
		int64_t sessionPoints,
		int64_t residualValue,
		int64_t bonusPoints);

	byte * GetReadWriteBuffer(DWORD &bufferSize);
	void SetStatus(DWORD status);
	DWORD GetStatus();
	void SetReadWriteBuffer();
	
private:
	CMeters *m_meters;
	byte m_meterCount;
	byte m_transCode;
	int64_t m_sessionPoints;
	int64_t m_residualValue;
	int64_t m_bonusPoints;

	DWORD m_uniqueId;
	DWORD m_status;
	const byte * m_cardID;

	int m_dataCount;
	bool m_bBufferIsSet;

	FRIEND_TEST(ExcessiveVoucherOutTests, PollerGenericTransactionMessageTests);
};

