#pragma once
#include "PollerMessage.h"
#include "../TiltsDef.h"
#include "../Player.h"
#include "../BonusEvent.h"

class CPollerBonusTransactionMessage :
	public CPollerMessage
{
public:
	CPollerBonusTransactionMessage(CMeters *meters, CPlayer *player, CBonusEvent *bonusEvent, byte transType, 
                                   DWORD uniqueId);
public:
	~CPollerBonusTransactionMessage(void);

public:

	byte * GetReadWriteBuffer( DWORD &bufferSize );
	void SetStatus(DWORD status);
	DWORD GetStatus();
	void SetReadWriteBuffer();

private:
	byte m_DataCount;

	bool m_bufferIsSet;
	CMeters *m_meters;
	CPlayer *m_player;
	CBonusEvent *m_bonusEvent;
	byte m_meterCount;
	byte m_transCode;
	DWORD m_uniqueId;
	DWORD m_status;
};
