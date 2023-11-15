#include "stdafx.h"

#include "PollerTicketTransactionMessage.h"
#include "Logging/LogString.h"

CPollerTicketTransactionMessage::CPollerTicketTransactionMessage(CMeters *meters,
		CTicket * ticket,
		byte transType,
		DWORD uniqueId,
		const byte * cardID)
{
    m_status = 0;
	m_meters = meters;
	m_meterCount = m_meters->GetMeterCount();
	if( m_meterCount > NUM_METERS )
		m_meterCount = NUM_METERS;

	m_uniqueId = uniqueId;
	m_cardID = cardID;
	m_ticket = ticket;
	m_dataCount = 11;

	m_transCode = transType;
	
	m_bufferSize = SIZE_OF_MESSAGE_HEADER + 
		NONVARIABLE_TRANSACTION_SIZE +	
		sizeof(byte) +						//meter count
		(m_meterCount * (sizeof(PollerMeter().Number) + sizeof(PollerMeter().Value))) +
		sizeof(m_dataCount) +
		sizeof(byte) + sizeof(byte) + sizeof(int) +			// Ticket Number
		sizeof(byte) + sizeof(byte) + (m_ticket ? m_ticket->ValidationLength() : 0) + //validation Number
		sizeof(byte) + sizeof(byte) + sizeof(int) +			//machine number
		sizeof(byte) + sizeof(byte) + sizeof(byte) +		//expiration type
		sizeof(byte) + sizeof(byte) + sizeof(byte) +		//expiration duration
		sizeof(byte) + sizeof(byte) + sizeof(WORD) +		//ticket sequence number
		sizeof(byte) + sizeof(byte) + sizeof(byte) +		//action code
		sizeof(byte) + sizeof(byte) + sizeof(byte) +		//ticket type
		sizeof(byte) + sizeof(byte) + sizeof(__int64) +		//amount in pennies
		sizeof(byte) + sizeof(byte) + SIZE_OF_UTC_TIME +	//machine time
		sizeof(byte) + sizeof(byte) + sizeof(DWORD) ;		//Play Session ID


	m_lpBuffer = new byte[m_bufferSize];
	m_bufferIsSet = false;

}

CPollerTicketTransactionMessage::~CPollerTicketTransactionMessage(void)
{
}

void CPollerTicketTransactionMessage::SetStatus(DWORD status)
{
	m_status = status;
}

DWORD CPollerTicketTransactionMessage::GetStatus()
{
	return m_status;
}

void CPollerTicketTransactionMessage::SetPlaySessionID(DWORD psid)
{
	m_psid = psid;
}

DWORD CPollerTicketTransactionMessage::GetPlaySessionID()
{
	return m_psid;
}

byte * CPollerTicketTransactionMessage::GetReadWriteBuffer(DWORD &bufferSize)
{
	bufferSize = m_bufferSize;
	return m_lpBuffer;
}

void CPollerTicketTransactionMessage::SetReadWriteBuffer()
{
	m_bufferIsSet = true;
	byte typeSize;
		
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
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_dataCount, sizeof(byte));
	offset += sizeof(byte);


	// Ticket Number Data Type
	byte transDataType = DATA_TYPE_TICKET_NUMBER;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	//Ticket Number Size
	typeSize = sizeof(int);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &typeSize, sizeof(typeSize));
	offset += sizeof(typeSize);
	// Ticket Number Data 
	int number = m_ticket->Number();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &number, typeSize);
	offset += typeSize;


	// Validation Number data type
	transDataType = DATA_TYPE_TICKET_VALIDATION_NUMBER;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	//Validation Number Size
	byte validationLength = 0;
	const byte *validationNumber;
	validationNumber = m_ticket->ValidationNumber(validationLength);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &validationLength, sizeof(validationLength));
	offset += sizeof(validationLength);
	// Validation Number Data 
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, validationNumber, validationLength);
	offset += validationLength;


	// Machine Number data type
	transDataType = DATA_TYPE_TICKET_MACHINE_NUMBER;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	// Machine Number Size
	typeSize = sizeof(int);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &typeSize, sizeof(typeSize));
	offset += sizeof(typeSize);
	// Machine Number Data 
	number = m_ticket->MachineNumber();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &number, typeSize);
	offset += typeSize;

	LogString(POLLER_LOG, "Sending Ticket - Machine Number %d", number);


	//Expiration type data type
	transDataType = DATA_TYPE_TICKET_EXPIRATION_TYPE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	// Expiration type Number Size
	typeSize = sizeof(byte);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &typeSize, sizeof(typeSize));
	offset += sizeof(typeSize);
	// Expiration type Data 
	byte value = m_ticket->ExpirationType();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &value, typeSize);
	offset += typeSize;


	//Expiration Duration data type
	transDataType = DATA_TYPE_TICKET_EXPIRATION_DURATION;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	// Expiration Duration Number Size
	typeSize = sizeof(byte);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &typeSize, sizeof(typeSize));
	offset += sizeof(typeSize);
	// Expiration Duration Data 
	value = m_ticket->ExpirationDuration();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &value, typeSize);
	offset += typeSize;


	// Sequence Number data type
	transDataType = DATA_TYPE_TICKET_SEQUENCE_NUMBER;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	// Sequence Number Number Size
	typeSize = sizeof(WORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &typeSize, sizeof(typeSize));
	offset += sizeof(typeSize);
	// Sequence Number Data 
	WORD wValue = m_ticket->SequenceNumber();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &wValue, typeSize);
	offset += typeSize;

	
	// Action Code data type
	transDataType = DATA_TYPE_TICKET_ACTION_CODE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	// Action Code Size
	typeSize = sizeof(byte);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &typeSize, sizeof(typeSize));
	offset += sizeof(typeSize);
	// Action Code Data 
	value = m_ticket->ActionCode();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &value, typeSize);
	offset += typeSize;

	LogString(POLLER_LOG, "Sending Ticket - Machine Number %d", value);


	// Ticket Type data type
	transDataType = DATA_TYPE_TICKET_TYPE;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	// Ticket Type Size
	typeSize = sizeof(byte);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &typeSize, sizeof(typeSize));
	offset += sizeof(typeSize);
	// Ticket Type Data 
	value = m_ticket->Type();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &value, typeSize);
	offset += typeSize;


	// Amount data type
	transDataType = DATA_TYPE_AMOUNT_IN_PENNIES;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	// Amount Size
	typeSize = sizeof(__int64);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &typeSize, sizeof(typeSize));
	offset += sizeof(typeSize);
	// Amount Data 
	__int64 nValue = m_ticket->Amount();
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &nValue, typeSize);
	offset += typeSize;


	// Game Time data type
	transDataType = DATA_TYPE_MACHINE_TIME;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);
	// Game Time Size
	typeSize = sizeof(SYSTEMTIME);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &typeSize, sizeof(typeSize));
	offset += sizeof(typeSize);
	// Transaction time
    offset = AppendUTCTimeStamp(offset);

	// Play Session ID
	transDataType = DATA_TYPE_PLAY_SESSION_ID;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &transDataType, sizeof(transDataType));
	offset += sizeof(transDataType);

	typeSize = sizeof(DWORD);
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &typeSize, sizeof(typeSize));
	offset += sizeof(typeSize);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &m_psid, sizeof(m_psid));
	offset += sizeof(m_psid);

}
