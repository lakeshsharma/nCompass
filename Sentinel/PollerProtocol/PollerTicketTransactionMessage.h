#pragma once
#include "PollerMessage.h"
#include "../Ticket.h"
#include "../Utilities.h"

class CPollerTicketTransactionMessage :
	public CPollerMessage
{
public:
	CPollerTicketTransactionMessage(CMeters *meters,
		CTicket * ticket,
		byte transType,
		DWORD uniqueId,
		const byte * cardID);

public:
	~CPollerTicketTransactionMessage(void);
	void SetReadWriteBuffer();
	byte * GetReadWriteBuffer( DWORD &bufferSize );
	void SetStatus(DWORD status);
	DWORD GetStatus();

	
	void SetPlaySessionID(DWORD psid);
	DWORD GetPlaySessionID();

private:
    DWORD m_psid;
	bool m_bufferIsSet;
	byte m_dataCount;
	CMeters *m_meters;
	byte m_meterCount;
	byte m_transCode;
	DWORD m_uniqueId;
	DWORD m_status;
	const byte * m_cardID;
	CTicket *m_ticket;
};
