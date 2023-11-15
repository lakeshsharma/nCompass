#include "stdafx.h"
#include "PollerCTTTransactionMessage.h"

CPollerCTTTransactionMessage::CPollerCTTTransactionMessage(CMeters *meters,
		CPlayer *player,
		DWORD uniqueId,
		DWORD transferCode,
		DWORD transferCount,
		LONGLONG activeSeconds)
{
    m_status = 0;
	m_meters = meters;
	m_player = player;

	m_meterCount = m_meters->GetMeterCount();
	if( m_meterCount > NUM_METERS )
		m_meterCount = NUM_METERS;

	m_uniqueId = uniqueId;
	m_transactionCode = CASH_TRANSFER_TRACKING_CODE;
	m_transferCode = transferCode;
	m_transferCount = transferCount;
	m_DataCount = 3;

	m_bufferSize = SIZE_OF_MESSAGE_HEADER + 
		NONVARIABLE_TRANSACTION_SIZE +
		sizeof(byte) +
		(m_meterCount * (sizeof(PollerMeter().Number) + sizeof(PollerMeter().Value))) +
		sizeof(m_DataCount) +
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) + //Cash Transfer Code
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) + //Cash Transfer Count
		sizeof(byte) + sizeof(byte) + sizeof(__int64);  //active seconds

	m_lpBuffer = new byte[m_bufferSize];
	m_bufferIsSet = false;
	m_activeSeconds = activeSeconds;
}

CPollerCTTTransactionMessage::~CPollerCTTTransactionMessage(void)
{
}

void CPollerCTTTransactionMessage::SetStatus(DWORD status)
{
	m_status = status;
}

DWORD CPollerCTTTransactionMessage::GetStatus()
{
	return m_status;
}

byte * CPollerCTTTransactionMessage::GetReadWriteBuffer(DWORD &bufferSize)
{
	bufferSize = m_bufferSize;
	return m_lpBuffer;
}

void CPollerCTTTransactionMessage::SetReadWriteBuffer()
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
	int cardIdLen;
	const byte *cardId = m_player->GetCard()->GetCardID(cardIdLen);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, cardId, cardIdLen > CARD_ID_LEN ? CARD_ID_LEN : cardIdLen );
	offset += cardIdLen;

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

	// Cash Transfer Code
	transDataType = DATA_TYPE_CASH_TRANSFER_CODE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	byte dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_transferCode, sizeof(m_transferCode));
	offset += sizeof(m_transferCode);

	// Cash Transfer Count
	transDataType = DATA_TYPE_CASH_TRANSFER_COUNT;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_transferCount, sizeof(m_transferCount));
	offset += sizeof(m_transferCount);

	// Player's Active Seconds
	transDataType = DATA_TYPE_ACTIVE_SECONDS;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(__int64);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_activeSeconds, sizeof(m_activeSeconds));
}
