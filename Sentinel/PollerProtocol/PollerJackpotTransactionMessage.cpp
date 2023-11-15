#include "stdafx.h"
#include "PollerJackpotTransactionMessage.h"
#include "../Utilities.h"

CPollerJackpotTransactionMessage::CPollerJackpotTransactionMessage(CMeters *meters,
		byte transCode,
		DWORD uniqueId,
		const byte * cardID,
		__int64 amount,
		DWORD coinsPlayed,
		DWORD jackpotType,
		DWORD progressiveLevel,
		DWORD machineNumber,
		bool keyToCreditAvailable)
{
    m_status = 0;
	m_meters = meters;
	m_cardID = cardID;
	m_transactionCode = transCode;

	m_meterCount = m_meters->GetMeterCount();
	if( m_meterCount > NUM_METERS )
		m_meterCount = NUM_METERS;

	m_uniqueId = uniqueId;
	m_amount = amount;
	m_coinsPlayed = coinsPlayed;
	m_jackpotType = jackpotType;
	m_progressiveLevel = progressiveLevel;
	m_machineNumber = machineNumber;
	m_keyToCreditAvailable = keyToCreditAvailable;

	m_DataCount = 6;

	m_bufferSize = SIZE_OF_MESSAGE_HEADER + 
		NONVARIABLE_TRANSACTION_SIZE +
		sizeof(byte) +
		(m_meterCount * (sizeof(PollerMeter().Number) + sizeof(PollerMeter().Value))) +
		sizeof(m_DataCount) +
		sizeof(byte) + sizeof(byte) + sizeof(__int64) + //Jackpot Amount
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) +   //Coins Played
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) +   //Jackpot Type
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) +    //Progressive Level
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) +	//Machine Number
		sizeof(byte) + sizeof(byte) + sizeof(bool);		//Key To Credit Available

	m_lpBuffer = new byte[m_bufferSize];
	m_bufferIsSet = false;
}

CPollerJackpotTransactionMessage::~CPollerJackpotTransactionMessage(void)
{
}

void CPollerJackpotTransactionMessage::SetStatus(DWORD status)
{
	m_status = status;
}

DWORD CPollerJackpotTransactionMessage::GetStatus()
{
	return m_status;
}

byte * CPollerJackpotTransactionMessage::GetReadWriteBuffer(DWORD &bufferSize)
{
	bufferSize = m_bufferSize;
	return m_lpBuffer;
}

void CPollerJackpotTransactionMessage::SetReadWriteBuffer()
{
	m_bufferIsSet = true;
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
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_transactionCode, sizeof(m_transactionCode));
	offset+= sizeof (m_transactionCode);

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

	// Transaction Data
	byte transDataType;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_DataCount, sizeof(byte));
	offset += sizeof(byte);

	// Jackpot Amount
	transDataType = DATA_TYPE_JACKPOT_AMOUNT;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	byte dataSize = sizeof(__int64);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_amount, sizeof(m_amount));
	offset += sizeof(m_amount);

	// coins played
	transDataType = DATA_TYPE_COINS_PLAYED;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_coinsPlayed, sizeof(m_coinsPlayed));
	offset += sizeof(m_coinsPlayed);

	//Jackpot Type
	transDataType = DATA_TYPE_JACKPOT_TYPE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_jackpotType, sizeof(m_jackpotType));
	offset += sizeof(m_jackpotType);

	//Progressive level
	transDataType = DATA_TYPE_PROGRESSIVE_LEVEL;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_progressiveLevel, sizeof(m_progressiveLevel));
	offset += sizeof(m_progressiveLevel);


	//machine number
	transDataType = DATA_TYPE_MACHINE_NUMBER;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_machineNumber, sizeof(m_machineNumber));
	offset += sizeof(m_machineNumber);

	//key to credit available
	transDataType = DATA_TYPE_KEY_TO_CREDIT_AVAILABLE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(m_keyToCreditAvailable);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_keyToCreditAvailable, sizeof(m_keyToCreditAvailable));
}
