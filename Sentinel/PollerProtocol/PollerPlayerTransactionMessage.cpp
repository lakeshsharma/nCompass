#include "stdafx.h"

#include "PollerPlayerTransactionMessage.h"
#include "PollerProtocol.h"
#include "../Utilities.h"

static const byte FIELD_COUNT = 9;

CPollerPlayerTransactionMessage::CPollerPlayerTransactionMessage(CPlayer *player, CMeters *meters, InternalMsgType msgType, time_t *transTime, 
	DWORD uniqueId, DWORD playTxSeqNum, int denom, int gameNumber, bool mgmdGlobal, bool mgmdAvailable)
{
    m_status = 0;
	m_meters = meters;
	m_player = player;
	m_meterCount = m_meters->GetMeterCount();
	if( m_meterCount > NUM_METERS )
		m_meterCount = NUM_METERS;

	m_uniqueId = uniqueId;
	m_transTime = *transTime;
	m_dataCount = FIELD_COUNT;
	m_playTxSeqNum = playTxSeqNum;
	m_denom = denom;
	m_gameNumber = gameNumber;
	m_mgmdGlobal = mgmdGlobal;
	m_mgmdAvailable = mgmdAvailable;
	
	switch (msgType)
	{
	case INT_MSG_TRANSACTION_VIRTUAL_SESSION_STARTED: // Virtual Player Card In
		m_transCode = VIRTUAL_SESSION_STARTED;
		break;
	case INT_MSG_TRANSACTION_PLAYER_CARD_IN : // Player Card In
		m_transCode = PLAYER_CARD_IN_CODE;
		break;
	case INT_MSG_TRANSACTION_PLAYER_CARD_OUT : // Player Card Out
		m_transCode = PLAYER_CARD_OUT_CODE;
		break;
	case INT_MSG_TRANSACTION_PLAYER_UPDATE:	// Player Update
		m_transCode = PLAYER_UPDATE_CODE;
		break;
	default:
		break;
	}


	m_bufferSize = SIZE_OF_MESSAGE_HEADER + 
		NONVARIABLE_TRANSACTION_SIZE +
		sizeof(byte) +
		(m_meterCount * (sizeof(PollerMeter().Number) + sizeof(PollerMeter().Value))) +
		sizeof(m_dataCount) +
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) + //Session Points
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) + //Bonus Points
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) + //Residual Value
		sizeof(byte) + sizeof(byte) + sizeof(__int64) + //Total Coin Out
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) +  //Play Session ID
		sizeof(byte) + sizeof(byte) + sizeof(WORD) +    //Game Number
		sizeof(byte) + sizeof(byte) + sizeof(WORD) +    //Denomination
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) +  //Play Transaction Sequence
		sizeof(byte) + sizeof(byte) + sizeof(byte);     //MGMD Available
		

	if (m_transCode != PLAYER_CARD_IN_CODE)
	{
		m_bufferSize+= sizeof(byte) + sizeof(byte) + sizeof(__int64); //Active Seconds
		m_dataCount++;

		//Added for highlander changes
		m_bufferSize+= sizeof(byte) + sizeof(byte) + sizeof(byte); //max card indicator
		m_dataCount++;
	}

	if (m_transCode != PLAYER_UPDATE_CODE)
	{
		//Added for highlander changes
		m_bufferSize+= sizeof(byte) + sizeof(byte) + sizeof(byte); //Virtual Session indicator
		m_dataCount++;
	}

	m_lpBuffer = new byte[m_bufferSize];
}

CPollerPlayerTransactionMessage::~CPollerPlayerTransactionMessage(void)
{
	if (m_meters != NULL)
		delete m_meters;

	if (m_player != NULL)
		delete m_player;

}

void CPollerPlayerTransactionMessage::SetStatus(DWORD status)
{
	m_status = status;
}

DWORD CPollerPlayerTransactionMessage::GetStatus()
{
	return m_status;
}

void CPollerPlayerTransactionMessage::SetPlaySessionID(DWORD psid)
{
	m_psid = psid;
}

DWORD CPollerPlayerTransactionMessage::GetPlaySessionID()
{
	return m_psid;
}

byte * CPollerPlayerTransactionMessage::GetReadWriteBuffer(DWORD &bufferSize)
{
	bufferSize = m_bufferSize;
	SetReadWriteBuffer();
	return m_lpBuffer;
}

void CPollerPlayerTransactionMessage::SetReadWriteBuffer()
{

	MessageHeader.MessageID = POLLER_MSG_TRANSACTION;
	MessageHeader.DataLength = m_bufferSize - SIZE_OF_MESSAGE_HEADER;

	int offset = 0;
	SetReadWriteBufferHeader(offset);
	SetReadWriteBufferMeters(offset);
	SetReadWriteBufferTransactionData(offset);
	SetReadWriteBufferSessionInfo(offset);
	SetReadWriteBufferVirtualIndicator(offset);
	SetReadWriteBufferGameSessionData (offset);
}

void CPollerPlayerTransactionMessage::SetReadWriteBufferHeader(int &offset)
{
	//Header (piece by piece to avoid missaligned data error)
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.PacketVersion), sizeof(MessageHeader.PacketVersion));
	offset+= sizeof(MessageHeader.PacketVersion);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.MessageID), sizeof(MessageHeader.MessageID));
	offset+= sizeof(MessageHeader.MessageID);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.DataLength), sizeof(MessageHeader.DataLength));
	offset+= sizeof(MessageHeader.DataLength);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.MacAddress), sizeof(MessageHeader.MacAddress));
	offset+= sizeof(MessageHeader.MacAddress);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SequenceNumber), sizeof(MessageHeader.SequenceNumber));
	offset+= sizeof(MessageHeader.SequenceNumber);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SlotMastID), sizeof(MessageHeader.SlotMastID));
	offset+= sizeof(MessageHeader.SlotMastID);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SlotmastRevision), sizeof(MessageHeader.SlotmastRevision));
	offset+= sizeof(MessageHeader.SlotmastRevision);

	// Transaction body
	// Transaction code
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_transCode, sizeof(m_transCode));
	offset+= sizeof (m_transCode);

	// Transaction time
	SYSTEMTIME transTime = CUtilities::GetUTCSystemTime(m_transTime);
	offset = AppendUTCTimeStamp(offset, transTime);

	// unique ID.
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_uniqueId, sizeof(m_uniqueId));
	offset += sizeof(m_uniqueId);

	// status.
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_status, sizeof(m_status));
	offset += sizeof(m_status);

	// card #
	int cardIdLen;
	const byte *cardId = m_player->GetCard()->GetCardID(cardIdLen);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, cardId, cardIdLen > CARD_ID_LEN ? CARD_ID_LEN : cardIdLen);
	offset += cardIdLen;
}

void CPollerPlayerTransactionMessage::SetReadWriteBufferMeters(int &offset)
{

	//Meter Count
	memcpy(m_lpBuffer + offset, &m_meterCount, sizeof(byte));
	offset+= sizeof(byte);

	PollerMeter pollerMeter;
	CMeter *lpMeter;

	//Meters
	for(int n = 0; n < m_meterCount && n < NUM_METERS; n++)
	{
		lpMeter = m_meters->GetMeterByIndex(n);
		pollerMeter.Number = lpMeter->GetMeterNumber();
		pollerMeter.Value = lpMeter->GetValue();

		memcpy(m_lpBuffer + offset, &(pollerMeter.Number), sizeof(pollerMeter.Number));
		offset+=sizeof(pollerMeter.Number);

		memcpy(m_lpBuffer + offset, &(pollerMeter.Value), sizeof(pollerMeter.Value));
		offset+=sizeof(pollerMeter.Value);
	}
}

void CPollerPlayerTransactionMessage::SetReadWriteBufferTransactionData(int &offset)
{
	// Transaction Data
	byte transDataType;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_dataCount, sizeof(byte));
	offset += sizeof(byte);

	// Session Points
	transDataType = DATA_TYPE_SESSION_POINTS;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	byte dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	DWORD Pts = m_player->GetSessionPoints() + m_player->GetTierMultiplierPoints();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &Pts, sizeof(Pts));
	offset += sizeof(Pts);

	// Bonus Points
	transDataType = DATA_TYPE_BONUS_POINTS;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	Pts = m_player->GetBonusPoints();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &Pts, sizeof(Pts));
	offset += sizeof(Pts);

	// Residual Value/Countdown
	transDataType = DATA_TYPE_RESIDUAL_VALUE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	Pts = m_player->GetCurrentCountdown();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &Pts, sizeof(Pts));
	offset += sizeof(Pts);

	// Total Coin Out
	transDataType = DATA_TYPE_TOTAL_COIN_OUT;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(__int64);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	__int64 mgCO = 0, mmBonusMach = 0, mmPrgPayMac = 0;
	CMeter *meter;
	meter = m_meters->GetMeterByNumber(gCO);
	if (meter)
		mgCO = meter->GetValue();

	meter = m_meters->GetMeterByNumber(mBonusMach);
	if (meter)
		mmBonusMach = meter->GetValue();

	meter = m_meters->GetMeterByNumber(mPrgPayMac);
	if (meter)
		mmPrgPayMac = meter->GetValue();

	__int64 totalCoinOut = mgCO + mmBonusMach + mmPrgPayMac;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &totalCoinOut, sizeof(totalCoinOut));
	offset += sizeof(totalCoinOut);
}

void CPollerPlayerTransactionMessage::SetReadWriteBufferSessionInfo(int &offset)
{
	byte transDataType;
	byte dataSize;

	if (m_transCode != PLAYER_CARD_IN_CODE)
	{
		// Player's Active Seconds
		transDataType = DATA_TYPE_ACTIVE_SECONDS;
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
		offset += sizeof(transDataType);

		dataSize = sizeof(__int64);
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
		offset += sizeof(dataSize);

		//If we are sending a transaction before card removed or end of virtual session, then LastSessionStartTime will be 0,
		//hence get delta from card in time.
		time_t activeTime; // timespan in seconds

		if (0 == m_player->GetLastSessionStartTime())
		{
			activeTime = m_transTime - m_player->GetCard()->GetCardInTime();
		}
		else
		{
			activeTime = m_transTime - m_player->GetLastSessionStartTime();
		}
		LONGLONG activeSeconds = activeTime;
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &activeSeconds, sizeof(activeSeconds));
		offset += sizeof(activeSeconds);

		// Player's MAX card indicator
		transDataType = DATA_TYPE_MAX_CARD_INDICATOR;
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
		offset += sizeof(transDataType);

		dataSize = sizeof(byte);
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
		offset += sizeof(dataSize);

		byte maxCardLimit = m_player->GetPlayerInfo()->IsMaxCardLimitReached() ? 1 : 0;
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &maxCardLimit, sizeof(maxCardLimit));
		offset += sizeof(maxCardLimit);
	}
}

void CPollerPlayerTransactionMessage::SetReadWriteBufferVirtualIndicator(int &offset)
{
	byte transDataType;
	byte dataSize;

	if (m_transCode != PLAYER_UPDATE_CODE)
	{
		// Player's Virtual session indicator
		transDataType = DATA_TYPE_VIRTUAL_SESSION_INDICATOR;
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
		offset += sizeof(transDataType);

		dataSize = sizeof(byte);
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
		offset += sizeof(dataSize);

		byte virtualFlag; 
	    if(!m_player->IsVirtual())
			virtualFlag = 0;
		else
			virtualFlag = 1;

		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &virtualFlag, sizeof(virtualFlag));
		offset += sizeof(virtualFlag);
	}

}

void CPollerPlayerTransactionMessage:: SetReadWriteBufferGameSessionData (int &offset)
{
	byte transDataType;
	byte dataSize;
    WORD zeroWord = 0;
	bool zeroBool = 0;
	// Play Session ID
	transDataType = DATA_TYPE_PLAY_SESSION_ID;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	
	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);
	
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_psid, sizeof(m_psid));
	offset += sizeof(m_psid);
	
	
	// Game Number
	transDataType = DATA_TYPE_GAME_NUMBER;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(WORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, (m_mgmdGlobal) ? &m_gameNumber : &zeroWord, sizeof(m_gameNumber));
	offset += dataSize;

	// Denomination
	transDataType = DATA_TYPE_DENOM;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(WORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset,  (m_mgmdGlobal) ? &m_denom : &zeroWord, sizeof(m_denom));
	offset += dataSize;

	// Play Transaction Sequence
	transDataType = DATA_TYPE_PLAY_TRANSACTION_SEQUENCE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset,  &m_playTxSeqNum, sizeof(m_playTxSeqNum));
	offset += dataSize;

	// MGMD Available
	transDataType = DATA_TYPE_MGMD_AVAILABLE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(byte);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset,  (m_mgmdGlobal) ? &m_mgmdAvailable : &zeroBool, sizeof(m_mgmdAvailable));
	offset += dataSize; 

}

