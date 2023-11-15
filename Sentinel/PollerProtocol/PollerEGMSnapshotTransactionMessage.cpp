#include "stdafx.h"
#include "PollerEGMSnapshotTransactionMessage.h"
#include "../Utilities.h"

CPollerEGMSnapshotTransactionMessage::CPollerEGMSnapshotTransactionMessage(CMeters *meters,
																		byte transType,
																		SYSTEMTIME transTime,
																		DWORD uniqueId,
																		SnapshotType type,
																		bool gameCommDown,
																		const byte * cardID)
{
    m_status = 0;
	m_meters = meters;
	m_meterCount = m_meters->GetMeterCount();
	if( m_meterCount > NUM_METERS )
		m_meterCount = NUM_METERS;

	m_uniqueId = uniqueId;
	m_cardID = cardID;
	m_DataCount = 2; //number of data items being sent
	m_bGameCommDown = gameCommDown;
	m_transCode = transType;
	m_type = type;
	
	m_bufferSize = SIZE_OF_MESSAGE_HEADER + 
		NONVARIABLE_TRANSACTION_SIZE +	//
		sizeof(byte) +					//meter count
		(m_meterCount * ( sizeof(PollerMeter().Number) + sizeof(PollerMeter().Value) )) + //meters
		sizeof(byte) + //transaction data count
		sizeof(byte) + 	sizeof(byte) + sizeof(byte) + //Snapshot Type
		sizeof(byte) + sizeof(byte) + sizeof(byte); //game comm. down flag

	m_lpBuffer = new byte[m_bufferSize];
	m_rwBufferIsSet = false;
	m_transTime = transTime;
}

CPollerEGMSnapshotTransactionMessage::~CPollerEGMSnapshotTransactionMessage(void)
{
}

void CPollerEGMSnapshotTransactionMessage::SetStatus(DWORD status)
{
	m_status = status;
}

DWORD CPollerEGMSnapshotTransactionMessage::GetStatus()
{
	return m_status;
}

byte * CPollerEGMSnapshotTransactionMessage::GetReadWriteBuffer(DWORD &bufferSize)
{
	bufferSize = m_bufferSize;
	return m_lpBuffer;
}

void CPollerEGMSnapshotTransactionMessage::SetReadWriteBuffer()
{
	MessageHeader.MessageID = POLLER_MSG_TRANSACTION;
	MessageHeader.DataLength = m_bufferSize - SIZE_OF_MESSAGE_HEADER;

	m_rwBufferIsSet = true;

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
	offset = AppendUTCTimeStamp(offset, m_transTime);

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
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_DataCount, sizeof(byte));
	offset += sizeof(byte);

	// Transaction Data Type
	byte transDataType = DATA_TYPE_SNAPSHOT_TYPE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	// Data length
	byte dataSize = sizeof(byte);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	// Data - Snapshot type
	byte snapshot = (byte)m_type;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &snapshot, sizeof(snapshot));
	offset += sizeof(snapshot);

	// Transaction Data Type
	transDataType = DATA_TYPE_GAME_COMM_DOWN;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	// Data length
	dataSize = sizeof(byte);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	// Data - Game Comm Down
	byte commmDown = (byte) m_bGameCommDown;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &commmDown, sizeof(commmDown));
}
