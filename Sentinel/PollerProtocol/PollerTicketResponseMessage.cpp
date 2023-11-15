#include "stdafx.h"

#include "PollerTicketResponseMessage.h"
#include "Logging/LogString.h"

CPollerTicketResponseMessage::CPollerTicketResponseMessage(byte *response)
{
	byte offset = 0;
	memcpy(&m_buffer.number, response + offset, sizeof(m_buffer.number));
	offset+= sizeof(m_buffer.number);

	memcpy(&m_buffer.type, response + offset, sizeof(m_buffer.type));
	offset+= sizeof(m_buffer.type);
	LogString(POLLER_LOG, "Receiving Ticket Type code = %d", m_buffer.type);

	memcpy(&m_buffer.sequenceNumber, response + offset, sizeof(m_buffer.sequenceNumber));
	offset+= sizeof(m_buffer.sequenceNumber);

	memcpy(&m_buffer.ID, response + offset, sizeof(m_buffer.ID));
	offset+= sizeof(m_buffer.ID);
	LogString(POLLER_LOG, "Receiving Ticket ID = %lld", (long long)m_buffer.ID);


	memcpy(&m_buffer.amount, response + offset, sizeof(m_buffer.amount));
	offset+= sizeof(m_buffer.amount);

	memcpy(&m_buffer.actionCode, response + offset, sizeof(m_buffer.actionCode));
	offset+= sizeof(m_buffer.actionCode);
	LogString(POLLER_LOG, "Receiving Ticket Action Code = %d", m_buffer.actionCode);

	memcpy(&m_buffer.resultCode, response + offset, sizeof(m_buffer.resultCode));
	offset+= sizeof(m_buffer.resultCode);
	LogString(POLLER_LOG, "Receiving Ticket Result Code = %d", m_buffer.resultCode);

	byte numberOfDigits;
	memcpy(&numberOfDigits, response + offset, sizeof(numberOfDigits));
	offset+= sizeof(numberOfDigits);

	//The number of digits is twice the number of bytes we'll receive
	//Not my doing!!!
	m_buffer.validationLength = (numberOfDigits / 2);
	m_buffer.validationLength = m_buffer.validationLength + (numberOfDigits % 2);
	m_buffer.validationNumber = NULL;

	m_buffer.validationNumber = new byte[m_buffer.validationLength];
	memcpy(m_buffer.validationNumber, response + offset, m_buffer.validationLength);
	offset+= m_buffer.validationLength;

	memcpy(&m_buffer.machineNumber, response + offset, sizeof(m_buffer.machineNumber));
	offset+= sizeof(m_buffer.machineNumber);
	LogString(POLLER_LOG, "Receiving Ticket Machine Number = %d", m_buffer.machineNumber);


	memcpy(&m_buffer.expirationType, response + offset, sizeof(m_buffer.expirationType));
	offset+= sizeof(m_buffer.expirationType);

	memcpy(&m_buffer.expirationDuration, response + offset, sizeof(m_buffer.expirationDuration));
	offset+= sizeof(m_buffer.expirationDuration);

	memcpy(&m_buffer.gameDateTime, response + offset, sizeof(m_buffer.gameDateTime));
}

CPollerTicketResponseMessage::~CPollerTicketResponseMessage(void)
{
	if (m_buffer.validationNumber != NULL)
	{
		delete [] m_buffer.validationNumber;
	}
}

CTicket * CPollerTicketResponseMessage::GetTicketResponse()
{
	CTicket * ticket = new CTicket();


	ticket->SetTicketNumber(m_buffer.number);
	ticket->SetType((TicketType)m_buffer.type);
	ticket->SetSequenceNumber(m_buffer.sequenceNumber);
	ticket->SetID(m_buffer.ID);
	ticket->SetAmount(m_buffer.amount);
	ticket->SetActionCode((TicketActionCode)m_buffer.actionCode);
	ticket->SetResultCode((TicketResultCode)m_buffer.resultCode);
	ticket->SetValidationNumber(m_buffer.validationNumber, m_buffer.validationLength);
	ticket->SetMachineNumber(m_buffer.machineNumber);
	ticket->SetExpirationType((TicketExpirationType)m_buffer.expirationType);
	ticket->SetExpirationDuration(m_buffer.expirationDuration);

	ticket->SetGameDateTime(CUtilities::GetTimeFromUTCSystemTime(m_buffer.gameDateTime));

	return ticket;
}
