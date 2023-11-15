#include "stdafx.h"
#include "PollerOneLinkTransactionMessage.h"

const int DATACOUNT(3);

CPollerOneLinkTransactionMessage::CPollerOneLinkTransactionMessage(CMeters *meters,
	byte transType,
	DWORD uniqueID,
	const byte * cardID,
	int64_t sessionPoints,
	int64_t bonusPoints,
	int64_t residualValue)
{
    m_status = 0;
	m_meters = meters;
	m_meterCount = m_meters->GetMeterCount();
	if (m_meterCount > NUM_METERS)
		m_meterCount = NUM_METERS;

	m_uniqueId = uniqueID;
	m_sessionPoints = sessionPoints;
	m_bonusPoints = bonusPoints;
	m_residualValue = residualValue;
	m_cardID = cardID;
	m_dataCount = DATACOUNT;
	m_bBufferIsSet = false;

	m_transCode = transType;

	m_bufferSize = SIZE_OF_MESSAGE_HEADER +
		NONVARIABLE_TRANSACTION_SIZE +	//
		sizeof(byte) +					//meter count
		(m_meterCount * (sizeof(PollerMeter().Number) + sizeof(PollerMeter().Value))) +
		sizeof(byte) + //data count
		sizeof(byte) + sizeof(byte) + sizeof(m_sessionPoints) + //(Session Points)
		sizeof(byte) + sizeof(byte) + sizeof(m_bonusPoints) + //(Bonus Points)
		sizeof(byte) + sizeof(byte) + sizeof(m_residualValue);  //(Residual Value)

	m_lpBuffer = new byte[m_bufferSize];
}

void CPollerOneLinkTransactionMessage::SetStatus(DWORD status)
{
	m_status = status;
}

DWORD CPollerOneLinkTransactionMessage::GetStatus()
{
	return m_status;
}

byte * CPollerOneLinkTransactionMessage::GetReadWriteBuffer(DWORD &bufferSize)
{
	bufferSize = m_bufferSize;
	return m_lpBuffer;
}

void CPollerOneLinkTransactionMessage::SetReadWriteBuffer()
{
	m_bBufferIsSet = true;
	MessageHeader.MessageID = POLLER_MSG_TRANSACTION;
	MessageHeader.DataLength = m_bufferSize - SIZE_OF_MESSAGE_HEADER;

	int offset = 0;
	//Header (piece by piece to avoid missaligned data error)
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.PacketVersion), sizeof(MessageHeader.PacketVersion));
	offset += sizeof(MessageHeader.PacketVersion);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.MessageID), sizeof(MessageHeader.MessageID));
	offset += sizeof(MessageHeader.MessageID);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.DataLength), sizeof(MessageHeader.DataLength));
	offset += sizeof(MessageHeader.DataLength);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.MacAddress), sizeof(MessageHeader.MacAddress));
	offset += sizeof(MessageHeader.MacAddress);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SequenceNumber), sizeof(MessageHeader.SequenceNumber));
	offset += sizeof(MessageHeader.SequenceNumber);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SlotMastID), sizeof(MessageHeader.SlotMastID));
	offset += sizeof(MessageHeader.SlotMastID);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SlotmastRevision), sizeof(MessageHeader.SlotmastRevision));
	offset += sizeof(MessageHeader.SlotmastRevision);

	// Transaction body
	// Transaction code
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_transCode, sizeof(m_transCode));
	offset += sizeof(m_transCode);

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
	offset += sizeof(byte);

	PollerMeter pollerMeter;
	CMeter *lpMeter;

	//Meters
	for (int n = 0; n < m_meterCount && n < NUM_METERS; n++)
	{
		lpMeter = m_meters->GetMeterByIndex(n);
		pollerMeter.Number = lpMeter->GetMeterNumber();
		pollerMeter.Value = lpMeter->GetValue();

		memcpy(m_lpBuffer + offset, &(pollerMeter.Number), sizeof(pollerMeter.Number));
		offset += sizeof(pollerMeter.Number);

		memcpy(m_lpBuffer + offset, &(pollerMeter.Value), sizeof(pollerMeter.Value));
		offset += sizeof(pollerMeter.Value);
	}

	byte transDataType;
    byte dataSize(sizeof(int64_t));

	// Transaction Data Count
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_dataCount, sizeof(byte));
	offset += sizeof(byte);

	//(Session Points)
	transDataType = DATA_TYPE_HOPPER_METER_VALUE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

    memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_sessionPoints, sizeof(m_sessionPoints));
	offset += sizeof(m_sessionPoints);

	//m_bonusPoints
	transDataType = DATA_TYPE_METER_VALUE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

    memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_bonusPoints, sizeof(m_bonusPoints));
	offset += sizeof(m_bonusPoints);

	//m_residualValue
	transDataType = DATA_TYPE_METER_NUMBER;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

    memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_residualValue, sizeof(m_residualValue));
	offset += sizeof(m_residualValue);
}


