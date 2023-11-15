#include "stdafx.h"

#include "PollerGameSessionChangeTransactionMessage.h"
#include "PollerProtocol.h"
#include "../Utilities.h"

CPollerGameSessionChangeTransactionMessage::CPollerGameSessionChangeTransactionMessage
    (CMeters *meters, CPlayer *player, const byte *cardID, time_t *transTime, DWORD uniqueId, 
        DWORD psid, DWORD playTxSeqNum, int denom, int gameNumber, bool mgmdAvailable)
{
	m_meters = meters;
	m_player = player;
    m_cardID = cardID;
    m_transTime = *transTime;

    m_uniqueId = uniqueId;
    m_psid = psid;
    m_playTxSeqNum = playTxSeqNum;
    m_denom = denom;
    m_gameNumber = gameNumber;
    m_mgmdAvailable = mgmdAvailable;

	m_dataCount = FIELD_COUNT;
	
    m_meterCount = m_meters->GetMeterCount();
	if( m_meterCount > NUM_METERS )
		m_meterCount = NUM_METERS;


    m_status = 0;
    m_transCode = GAME_SESSION_CHANGE;


	m_bufferSize = SIZE_OF_MESSAGE_HEADER + 
		NONVARIABLE_TRANSACTION_SIZE +
		sizeof(byte) +                                  //meter count
		(m_meterCount * (sizeof(PollerMeter().Number) + sizeof(PollerMeter().Value))) +
		sizeof(m_dataCount) +
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) +   //Session Points
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) +   //Bonus Points
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) +   //Residual Value
		sizeof(byte) + sizeof(byte) + sizeof(__int64) + //Total Coin Out
		sizeof(byte) + sizeof(byte) + sizeof(WORD) +    //Game Number
		sizeof(byte) + sizeof(byte) + sizeof(WORD) +    //Denomination
		sizeof(byte) + sizeof(byte) + sizeof(__int64) + //Active Seconds
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) +   //Play Session ID
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) +   //Game Session Sequence
		sizeof(byte) + sizeof(byte) + sizeof(byte) +    //MGMD Available
		sizeof(byte) + sizeof(byte) + sizeof(byte);     //Max Card Limit Reached

	m_lpBuffer = new byte[m_bufferSize];
}

CPollerGameSessionChangeTransactionMessage::~CPollerGameSessionChangeTransactionMessage(void)
{
	if (m_player != nullptr)
		delete m_player;

	if (m_meters != nullptr)
		delete m_meters;
}

void CPollerGameSessionChangeTransactionMessage::SetStatus(DWORD status)
{
	m_status = status;
}

DWORD CPollerGameSessionChangeTransactionMessage::GetStatus()
{
	return m_status;
}

byte * CPollerGameSessionChangeTransactionMessage::GetReadWriteBuffer(DWORD &bufferSize)
{
	bufferSize = m_bufferSize;
	SetReadWriteBuffer();
	return m_lpBuffer;
}

void CPollerGameSessionChangeTransactionMessage::SetReadWriteBuffer()
{

	MessageHeader.MessageID = POLLER_MSG_TRANSACTION;
	MessageHeader.DataLength = m_bufferSize - SIZE_OF_MESSAGE_HEADER;

	int offset = 0;

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
    offset = AppendUTCTimeStamp(offset);

	// unique ID.
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_uniqueId, sizeof(m_uniqueId));
	offset += sizeof(m_uniqueId);

	// status.
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_status, sizeof(m_status));
	offset += sizeof(m_status);

	// card #
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, m_cardID, CARD_ID_LEN);
	offset += CARD_ID_LEN;

    //Meter Count
	memcpy(m_lpBuffer + offset, &m_meterCount, sizeof(byte));
	offset+= sizeof(byte);

	PollerMeter pollerMeter;
	CMeter *lpMeter;

	//Meters
	for(byte n = 0; n < m_meterCount && n < NUM_METERS; n++)
	{
		lpMeter = m_meters->GetMeterByIndex(n);
		pollerMeter.Number = lpMeter->GetMeterNumber();
		pollerMeter.Value = lpMeter->GetValue();

		memcpy(m_lpBuffer + offset, &(pollerMeter.Number), sizeof(pollerMeter.Number));
		offset+=sizeof(pollerMeter.Number);

		memcpy(m_lpBuffer + offset, &(pollerMeter.Value), sizeof(pollerMeter.Value));
		offset+=sizeof(pollerMeter.Value);
	}

	byte transDataType;
	byte dataSize;

	// Transaction Data Count
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_dataCount, sizeof(byte));
	offset += sizeof(byte);
	
	// Session Points
	transDataType = DATA_TYPE_SESSION_POINTS;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	DWORD Pts = 0;
	if (m_player)
		Pts = m_player->GetSessionPoints() + m_player->GetTierMultiplierPoints();
	
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &Pts, sizeof(Pts));
	offset += sizeof(Pts);

	// Bonus Points
	transDataType = DATA_TYPE_BONUS_POINTS;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	Pts = 0;
	if (m_player)
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

	Pts = 0;
	if (m_player)
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
    
    // Game Number
	transDataType = DATA_TYPE_GAME_NUMBER;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(WORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_gameNumber, sizeof(m_gameNumber));
	offset += sizeof(m_gameNumber);

	// Denomination
	transDataType = DATA_TYPE_DENOM;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(WORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_denom, sizeof(m_denom));
	offset += dataSize;

    // Player's Active Seconds
    transDataType = DATA_TYPE_ACTIVE_SECONDS;
    memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
    offset += sizeof(transDataType);

    dataSize = sizeof(__int64);
    memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
    offset += sizeof(dataSize);

    time_t activeTime = 0;

    //If we are sending a transaction before card removed or end of virtual session, then LastSessionStartTime will be 0,
    //hence get delta from card in time.    
    if (m_player)
	{
	    if ( (m_player->GetCard()) && (0 == m_player->GetLastSessionStartTime()) )
	    {
	        activeTime = m_transTime - m_player->GetCard()->GetCardInTime();
	    }
	    else
	    {
	        activeTime = m_transTime - m_player->GetLastSessionStartTime();
	    }
    }
	
    LONGLONG activeSeconds = activeTime;
    memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &activeSeconds, sizeof(activeSeconds));
    offset += sizeof(activeSeconds);

    // Play Session ID
	transDataType = DATA_TYPE_PLAY_SESSION_ID;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_psid, sizeof(m_psid));
	offset += sizeof(m_psid);                             

    // Game Session Sequence
	transDataType = DATA_TYPE_PLAY_TRANSACTION_SEQUENCE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_playTxSeqNum, sizeof(m_playTxSeqNum));
	offset += sizeof(m_playTxSeqNum);                             
    
    // MGMD Available
	transDataType = DATA_TYPE_MGMD_AVAILABLE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(byte);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_mgmdAvailable, sizeof(m_mgmdAvailable));
	offset += sizeof(m_mgmdAvailable);                             

	// Max Card Limit Reached (Applicable for Carded play)
	transDataType = DATA_TYPE_MAX_CARD_INDICATOR;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(byte);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	byte maxCardLimitReached = 0;
	if (m_player && m_player->GetPlayerInfo() && m_player->GetPlayerInfo()->IsMaxCardLimitReached())
	{
		maxCardLimitReached = 1;
	}
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &maxCardLimitReached, sizeof(maxCardLimitReached));
	offset += sizeof(maxCardLimitReached);
}

