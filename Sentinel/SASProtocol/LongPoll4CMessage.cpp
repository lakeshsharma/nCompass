#include "stdafx.h"
#include "LongPoll4CMessage.h"

CLongPoll4CMessage::CLongPoll4CMessage(byte gameAddress) :
	CSASPollMessage(gameAddress),
	m_machineValidationID(0),
	m_validationSequenceNumber(0)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[SEND_BUFFER_SIZE],SEND_BUFFER_SIZE);
	SetResponseBuffer((BYTE *)new byte[RESPONSE_BUFFER_SIZE],RESPONSE_BUFFER_SIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
}

CLongPoll4CMessage::~CLongPoll4CMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll4CMessage::GetPollValue()
{
	return POLL_VALUE;
}

void CLongPoll4CMessage::ResponseReceived()
{
	if ( GetReturnValue() == SASPOLL_SUCCESS )
	{

		m_machineValidationID = 0;
		memcpy(&m_machineValidationID,&m_responseBuffer[MACHINE_VALIDATION_ID_OFFSET],MACHINE_VALIDATION_ID_SIZE);

		m_validationSequenceNumber = 0;
		memcpy(&m_validationSequenceNumber,&m_responseBuffer[VALIDATION_SEQUENCE_NUMBER_OFFSET],VALIDATION_SEQUENCE_NUMBER_SIZE);
	}

	CSASPollMessage::ResponseReceived();
}

void CLongPoll4CMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[GAME_ADDRESS_OFFSET] = m_gameAddress;
	m_sendBuffer[POLL_VALUE_OFFSET] = POLL_VALUE;
	memcpy(&m_sendBuffer[MACHINE_VALIDATION_ID_OFFSET],&m_machineValidationID,MACHINE_VALIDATION_ID_SIZE);
	memcpy(&m_sendBuffer[VALIDATION_SEQUENCE_NUMBER_OFFSET],&m_validationSequenceNumber,VALIDATION_SEQUENCE_NUMBER_SIZE);

	byte *crcLocation;
	ushort messageCRC = CRC(GetSendBuffer(),GetSendBufferSize() - CRCSIZE,0);
	crcLocation = (byte *)&m_sendBuffer[CRC_OFFSET];
	memcpy(crcLocation,&messageCRC,CRCSIZE);
}

void CLongPoll4CMessage::SetMachineValidationID( DWORD validationID )
{
	m_machineValidationID = 0;
	memcpy(&m_machineValidationID,&validationID,MACHINE_VALIDATION_ID_SIZE);
}

void CLongPoll4CMessage::SetValidationSequenceNumber( DWORD sequenceNumber )
{
	m_validationSequenceNumber = 0;
	memcpy(&m_validationSequenceNumber, &sequenceNumber, VALIDATION_SEQUENCE_NUMBER_SIZE);
}

DWORD CLongPoll4CMessage::GetMachineValidationID( void ) const
{
	return m_machineValidationID;
}

DWORD CLongPoll4CMessage::GetValidationSequenceNumber( void ) const
{
	return m_validationSequenceNumber;
}
