#include "stdafx.h"
#include "PollerMobileConnectTransactionMessage.h"
#include "../Utilities.h"
#include "Logging/LogString.h"

CPollerMobileConnectTransactionMessage::CPollerMobileConnectTransactionMessage(CMeters *meters,
	byte transType,
	DWORD uniqueID,
	const byte * cardID,
	string& msgJson)
{
    m_status = 0;
	m_meters = meters;
	m_meterCount = m_meters->GetMeterCount();
	if( m_meterCount > NUM_METERS )
		m_meterCount = NUM_METERS;

	m_cardID = cardID;
	m_transCode = transType;
	m_uniqueId = uniqueID;
	m_dataCount = 0;
	m_bBufferIsSet = false;

	if (msgJson.size() > MAX_MOBILE_CONNECT_MESSAGE_SIZE) 
	{
		LogString (POLLER_LOG, "CPollerMobileConnectTransactionMessage: JSON message length exceeds maximum size (%d>%d), truncated!",
			msgJson.size(), MAX_MOBILE_CONNECT_MESSAGE_SIZE);
		m_msgJson = std::string(msgJson, MAX_MOBILE_CONNECT_MESSAGE_SIZE);
	}
	else
	{
		m_msgJson = msgJson;
	}

	m_bufferSize = SIZE_OF_MESSAGE_HEADER +
		NONVARIABLE_TRANSACTION_SIZE +	//
		sizeof(byte) +					//meter count
		(m_meterCount * (sizeof(PollerMeter().Number) + sizeof(PollerMeter().Value))) +
		sizeof(m_dataCount) + //data count
		sizeof(WORD) +   // Binary Blob Length
		msgJson.size();   // Binary Blob Size

	// To remove this log later
	LogString (POLLER_LOG, "CPollerMobileConnectTransactionMessage: JSON '%s', size=%d", CUtilities::MaskPin(m_msgJson).c_str(), msgJson.size());
	m_lpBuffer = new byte[m_bufferSize];
	memset(m_lpBuffer, 0, m_bufferSize);
}

CPollerMobileConnectTransactionMessage::~CPollerMobileConnectTransactionMessage(void)
{
}

void CPollerMobileConnectTransactionMessage::SetStatus(DWORD status)
{
	m_status = status;
}

DWORD CPollerMobileConnectTransactionMessage::GetStatus()
{
	return m_status;
}

byte * CPollerMobileConnectTransactionMessage::GetReadWriteBuffer(DWORD &bufferSize)
{
	bufferSize = m_bufferSize;
	return m_lpBuffer;
}

void CPollerMobileConnectTransactionMessage::SetReadWriteBuffer()
{
	if (m_bBufferIsSet)
	{
		return;
	}

	m_bBufferIsSet = true;
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

	// Transaction Data Count
	byte transDataType;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_dataCount, sizeof(byte));
	offset += sizeof(byte);

	// Transaction Data Type, Length and Values are empty when Count is 0!

	// Binary Blob Length - Json Payload Length
	WORD dataSize = m_msgJson.size();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	// Binary Blob Value - Json Payload Data
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, m_msgJson.c_str(), dataSize);

#ifdef _DEBUG
	LogString (POLLER_LOG, "PollerMobileConnectTransactionMessage::SetReadWriteBuffer populated buffer: '%s'",((char*)(m_lpBuffer + 1 * offset)));
#endif
}
