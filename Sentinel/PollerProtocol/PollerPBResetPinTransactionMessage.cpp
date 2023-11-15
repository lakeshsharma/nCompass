#include "stdafx.h"
#include "PollerPBResetPinTransactionMessage.h"
#include "Logging/LogString.h"

const int DATACOUNT = 2;

CPollerPBResetPinTransactionMessage::CPollerPBResetPinTransactionMessage(CMeters *meters, byte transType, DWORD uniqueID, const byte * cardID,	int oldPin,	int newPin)
{
    m_status = 0;
	m_meters = meters;
	m_meterCount = m_meters->GetMeterCount();
	if (m_meterCount > NUM_METERS)
		m_meterCount = NUM_METERS;

	m_uniqueId = uniqueID;
	m_oldPin = oldPin;
	m_newPin = newPin;
	m_cardID = cardID;
	m_dataCount = DATACOUNT;
	m_bBufferIsSet = false;

	m_transCode = transType;

	m_bufferSize = SIZE_OF_MESSAGE_HEADER +
		NONVARIABLE_TRANSACTION_SIZE +	//
		sizeof(byte) +					//meter count
		(m_meterCount * (sizeof(PollerMeter().Number) + sizeof(PollerMeter().Value))) +
		sizeof(byte) + //data count
		sizeof(byte) + sizeof(byte) + sizeof(int) + //old Pin
		sizeof(byte) + sizeof(byte) + sizeof(int); //new Pin


	m_lpBuffer = new byte[m_bufferSize];
}


CPollerPBResetPinTransactionMessage::~CPollerPBResetPinTransactionMessage()
{
}

void CPollerPBResetPinTransactionMessage::SetStatus(DWORD status)
{
	m_status = status;
}

DWORD CPollerPBResetPinTransactionMessage::GetStatus()
{
	return m_status;
}

byte * CPollerPBResetPinTransactionMessage::GetReadWriteBuffer(DWORD &bufferSize)
{
	bufferSize = m_bufferSize;
	return m_lpBuffer;
}

void CPollerPBResetPinTransactionMessage::SetReadWriteBuffer()
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

	// Transaction Data
	byte transDataType;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_dataCount, sizeof(byte));
	offset += sizeof(byte);

	//Old Pin
	DWORD encryptedPin = CUtilities::EncryptPBTPin(m_oldPin);

	transDataType = DATA_TYPE_PBT_PIN;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	byte dataSize = sizeof(m_oldPin);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &encryptedPin, sizeof(encryptedPin));
	offset += sizeof(encryptedPin);

	//New Pin
	encryptedPin = CUtilities::EncryptPBTPin(m_newPin);

	transDataType = DATA_TYPE_PBT_PIN;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	dataSize = sizeof(m_newPin);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &encryptedPin, sizeof(encryptedPin));
}

