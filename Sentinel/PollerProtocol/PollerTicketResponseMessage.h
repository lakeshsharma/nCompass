#pragma once
#include "PollerMessage.h"
#include "../Ticket.h"

typedef struct _ticketResp
{
	int		number;
	byte	type;
	WORD	sequenceNumber;
	__int64 ID;
	__int64	amount;
	byte	actionCode;
	WORD	resultCode;
	byte	validationLength;
	byte*	validationNumber;
	int		machineNumber;
	byte	expirationType;
	byte	expirationDuration;
	SYSTEMTIME 	gameDateTime;
}TicketResponse;

const int VALIDATION_LENGTH_OFFSET = sizeof(int) + sizeof(byte) + sizeof(WORD) + sizeof(__int64) +
									 sizeof(__int64) + sizeof(byte) + sizeof(WORD);

class CPollerTicketResponseMessage :
	public CPollerMessage
{
public:
	CPollerTicketResponseMessage(byte *response);
	CTicket * GetTicketResponse();

public:
	~CPollerTicketResponseMessage(void);

private:
	TicketResponse m_buffer;
};
