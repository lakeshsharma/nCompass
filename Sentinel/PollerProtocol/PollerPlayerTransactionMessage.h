#pragma once
#include "PollerMessage.h"
#include "../Player.h"
#include "../Meters.h"
#include "../InternalMessage.h"

class CPollerPlayerTransactionMessage :
	public CPollerMessage
{
public:
	static const DWORD DEFAULT_PLAYER_TX_SEQ_NUM = (DWORD)-1;
	static const int DEFAULT_DENOM = -1;
	static const int DEFAULT_GAME_NUMBER = -1;
	
	CPollerPlayerTransactionMessage(CPlayer *player, CMeters *meters, InternalMsgType msgType, time_t *transTime, 
		DWORD uniqueId, DWORD playTxSeqNum = DEFAULT_PLAYER_TX_SEQ_NUM, int denom = DEFAULT_DENOM, int gameNumber = DEFAULT_GAME_NUMBER, bool mgmdGlobal = 0, bool mgmdAvailable = 0);

	byte * GetReadWriteBuffer( DWORD &bufferSize );
	void SetStatus(DWORD status);
	DWORD GetStatus();

	void SetPlaySessionID(DWORD psid);
	DWORD GetPlaySessionID();
	
public:
	~CPollerPlayerTransactionMessage(void);

private:
	void SetReadWriteBuffer();
	void SetReadWriteBufferHeader(int &offset);
	void SetReadWriteBufferMeters(int &offset);
	void SetReadWriteBufferTransactionData(int &offset);
	void SetReadWriteBufferSessionInfo(int &offset);
	void SetReadWriteBufferVirtualIndicator(int &offset);
	void SetReadWriteBufferGameSessionData (int &offset);
	
	DWORD m_psid;
	CMeters *m_meters;
	byte m_dataCount;
	CPlayer *m_player;
	byte m_meterCount;
	byte m_transCode;
	DWORD m_uniqueId;
	time_t m_transTime;
	DWORD m_status;
	
	DWORD m_playTxSeqNum;
	WORD m_denom, m_gameNumber;
	bool m_mgmdGlobal, m_mgmdAvailable;

    friend SentinelNativeUnitTests::PollerMessageTests;
    friend SentinelNativeUnitTests::TransactionResendList;
    friend SentinelNativeUnitTests::PollerProtocolUnitTests;
};
