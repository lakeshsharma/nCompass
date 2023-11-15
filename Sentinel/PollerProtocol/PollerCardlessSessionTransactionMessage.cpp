
#include "stdafx.h"
#include "PollerCardlessSessionTransactionMessage.h"

CPollerCardlessSessionTransactionMessage::CPollerCardlessSessionTransactionMessage(
	 CMeters *meters,
	 byte transCode,
	 DWORD uniqueId,
	 DWORD playSessionId,
	 DWORD playTxSeqNum,
	 int gameNumber,
	 int gameDenom,
	 bool mgmdGlobal, 
	 bool mgmdAvailable)
{
    m_status = 0;
	m_meters = meters;
	m_meterCount = m_meters->GetMeterCount();
	
	if (m_meterCount > NUM_METERS)
	{
		m_meterCount = NUM_METERS;
	}

	m_uniqueId = uniqueId;
	m_DataCount = 5; //number of data items being sent
	m_transCode = transCode;
	m_playSessionId = playSessionId;
	m_playTxSeqNum = playTxSeqNum;
	m_gameNumber = (WORD) gameNumber;
	m_gameDenom = (WORD) gameDenom;
	m_mgmdGlobal = mgmdGlobal;
	m_mgmdAvailable = mgmdAvailable;
	
	m_bufferSize = SIZE_OF_MESSAGE_HEADER + 
		NONVARIABLE_TRANSACTION_SIZE +	//
		sizeof(byte) +					//meter count
		(m_meterCount * (sizeof(PollerMeter().Number) + sizeof(PollerMeter().Value)) )+ //meters
		sizeof(byte) + //transaction data count
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) +   // Play Session ID
		sizeof(byte) + sizeof(byte) + sizeof(DWORD)+    // Play Tx Sequence Number
		sizeof(byte) + sizeof(byte) + sizeof(WORD) +    // Game Number
		sizeof(byte) + sizeof(byte) + sizeof(WORD) +    // Game Denomination
		sizeof(byte) + sizeof(byte) + sizeof(byte);     // MGMD Available
		
	m_lpBuffer = new byte[m_bufferSize];
}

CPollerCardlessSessionTransactionMessage::~CPollerCardlessSessionTransactionMessage(void)
{
}

void CPollerCardlessSessionTransactionMessage::SetStatus(DWORD status)
{
	m_status = status;
}

DWORD CPollerCardlessSessionTransactionMessage::GetStatus()
{
	return m_status;
}

byte * CPollerCardlessSessionTransactionMessage::GetReadWriteBuffer(DWORD &bufferSize)
{
	bufferSize = m_bufferSize;
	SetReadWriteBuffer();
	return m_lpBuffer;
}

void CPollerCardlessSessionTransactionMessage::SetReadWriteBuffer()
{
	WORD zeroWord = 0;
	bool zeroBool = 0;

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

	// card #, should be 0 for cardless transactions.
	memset(m_lpBuffer + offset, 0, CARD_ID_LEN);
	offset += CARD_ID_LEN;

	//Meter Count
	memcpy(m_lpBuffer + offset, &m_meterCount, sizeof(byte));
	offset+= sizeof(byte);

	PollerMeter pollerMeter;
	CMeter *lpMeter;

	//Meters
	for(int n = 0; n < m_meterCount; n++)
	{
		lpMeter = m_meters->GetMeterByIndex(n);
		pollerMeter.Number = lpMeter->GetMeterNumber();
		pollerMeter.Value = lpMeter->GetValue();

		memcpy(m_lpBuffer + offset, &(pollerMeter.Number), sizeof(pollerMeter.Number));
		offset+=sizeof(pollerMeter.Number);

		memcpy(m_lpBuffer + offset, &(pollerMeter.Value), sizeof(pollerMeter.Value));
		offset+=sizeof(pollerMeter.Value);
	}

	// Transaction Data Count
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_DataCount, sizeof(byte));
	offset += sizeof(byte);

	// Play Session ID
	byte transDataType = DATA_TYPE_PLAY_SESSION_ID;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	byte dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_playSessionId, sizeof(m_playSessionId));
	offset += sizeof(m_playSessionId);

	// Play transaction sequence number
	transDataType = DATA_TYPE_PLAY_TRANSACTION_SEQUENCE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_playTxSeqNum, sizeof(m_playTxSeqNum));
	offset += sizeof(m_playTxSeqNum);
	
	// Game Number
	transDataType = DATA_TYPE_GAME_NUMBER;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(WORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, m_mgmdGlobal ? &m_gameNumber : &zeroWord, sizeof(m_gameNumber));
	offset += dataSize;

	// Denomination
	transDataType = DATA_TYPE_DENOM;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(WORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, m_mgmdGlobal ? &m_gameDenom : &zeroWord, sizeof(m_gameDenom));
	offset += dataSize;	

	// MGMD Available
	transDataType = DATA_TYPE_MGMD_AVAILABLE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(byte);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, m_mgmdGlobal ? &m_mgmdAvailable : &zeroBool, sizeof(m_mgmdAvailable));
	offset += dataSize;
}
