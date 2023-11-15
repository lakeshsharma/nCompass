#include "stdafx.h"
#include "PollerSubGameSnapshotTransactionMessage.h"
#include "../Utilities.h"
#include "../Game.h"

CPollerSubGameSnapshotTransactionMessage::CPollerSubGameSnapshotTransactionMessage(CMeters *meters,
		byte transType,
		SYSTEMTIME transTime,
		DWORD uniqueId,
		bool gameCommDown,
		WORD gameNumber,
		CMeters * subgameMeters,
		SnapshotType type,
		const byte * cardID,
		const char * manufacturerID,
		const char * paytableID,
		const char * additionalID)
{	
    m_status = 0;
	m_meters = meters;
	m_subgameMeters = subgameMeters;
	m_meterCount = m_meters->GetMeterCount();
	m_subgameMetersCount = subgameMeters->GetMeterCount();
	m_uniqueId = uniqueId;
	m_cardID = cardID;
	m_DataCount = 7; //all of the subgame meters are a single data object of 80 bytes
	m_bGameCommDown = gameCommDown;
	m_transCode = transType;
	m_gameNumber = gameNumber;
	m_type = type;

	m_lpMfgID = new char[MFG_ID_LEN];
	m_lpPaytableID = new char[PAY_TABLE_ID_LEN];
	m_lpAdditionalID = new char[ADDITIONAL_ID_LEN];

	CUtilities::ConvertCharsToChars(manufacturerID, m_lpMfgID, MFG_ID_LEN);
	CUtilities::ConvertCharsToChars(paytableID, m_lpPaytableID, PAY_TABLE_ID_LEN);
	CUtilities::ConvertCharsToChars(additionalID, m_lpAdditionalID, ADDITIONAL_ID_LEN);

	m_bufferSize = SIZE_OF_MESSAGE_HEADER + 
		NONVARIABLE_TRANSACTION_SIZE +	//
		sizeof(byte) +					//meter count
		(m_meterCount * ( sizeof(PollerMeter().Number) + sizeof(PollerMeter().Value) )) + //meters
		sizeof(byte) + //transaction data count
		sizeof(byte) + sizeof(byte) + sizeof(byte) +  //snapshot type
		sizeof(byte) + sizeof(byte) + POLLER_MFG_ID_LEN +			//manufacturer ID
		sizeof(byte) + sizeof(byte) + POLLER_PAY_TABLE_ID_LEN +    //pay table ID
		sizeof(byte) + sizeof(byte) + POLLER_ADDITIONAL_ID_LEN +   //additional ID
		sizeof(byte) + sizeof(byte) + sizeof(byte) +  //game comm down flag
		sizeof(byte) + sizeof(byte) + sizeof(WORD) +  //game number
		sizeof(byte) + sizeof(byte) + (m_subgameMetersCount * sizeof(__int64));   //subgame meters

	m_lpBuffer = new byte[m_bufferSize];
	m_rwBufferIsSet = false;
	m_transTime = transTime;
}

CPollerSubGameSnapshotTransactionMessage::~CPollerSubGameSnapshotTransactionMessage(void)
{
	delete [] m_lpPaytableID;
	delete [] m_lpMfgID;
	delete [] m_lpAdditionalID;
}

void CPollerSubGameSnapshotTransactionMessage::SetStatus(DWORD status)
{
	m_status = status;
}

DWORD CPollerSubGameSnapshotTransactionMessage::GetStatus()
{
	return m_status;
}

byte * CPollerSubGameSnapshotTransactionMessage::GetReadWriteBuffer(DWORD &bufferSize)
{
	bufferSize = m_bufferSize;
	return m_lpBuffer;
}

void CPollerSubGameSnapshotTransactionMessage::SetReadWriteBuffer()
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
	transDataType = DATA_TYPE_MANUFACTURER_ID;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	// Data Length
	dataSize = POLLER_MFG_ID_LEN;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	// Data Manufacturer ID
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, m_lpMfgID, POLLER_MFG_ID_LEN);
	offset += POLLER_MFG_ID_LEN;

	// Transaction Data Type
	transDataType = DATA_TYPE_PAYTABLE_ID;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	// Data Length
	dataSize = POLLER_PAY_TABLE_ID_LEN;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	// Data Manufacturer ID
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, m_lpPaytableID, POLLER_PAY_TABLE_ID_LEN);
	offset += POLLER_PAY_TABLE_ID_LEN;

	// Transaction Data Type
	transDataType = DATA_TYPE_ADDITIONAL_ID;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	// Data Length
	dataSize = POLLER_ADDITIONAL_ID_LEN;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	// Data Manufacturer ID
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, m_lpAdditionalID, POLLER_ADDITIONAL_ID_LEN);
	offset += POLLER_ADDITIONAL_ID_LEN;

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
	offset += sizeof(commmDown);


	// Transaction Data Type
	transDataType = DATA_TYPE_GAME_NUMBER;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	// Data length
	dataSize = sizeof(WORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	// Data - Game Number
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_gameNumber, sizeof(m_gameNumber));
	offset += sizeof(m_gameNumber);


	// Transaction Data Type
	transDataType = DATA_TYPE_SUBGAME_METERS;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	// Data length
	dataSize = m_subgameMetersCount * sizeof(__int64);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &dataSize, sizeof(dataSize));
	offset += sizeof(dataSize);

	// Data - SubgameMeters
	__int64 meterVal;
	for (int i = 0; i < m_subgameMetersCount; i++)
	{
		meterVal = m_subgameMeters->GetMeterByIndex(i)->GetValue();
		if (meterVal == INVALID_METER_VALUE)
		{
			meterVal = 0;
		}
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &meterVal, sizeof(meterVal));
		offset += sizeof(meterVal);
	}
}
