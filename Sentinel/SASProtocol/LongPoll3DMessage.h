#pragma once

#include "SASPollMessage.h"
#include "../Ticket.h"

class CLongPoll3DMessage :
	public CSASPollMessage
{
public:
	CLongPoll3DMessage(byte gameAddress);
	CTicket *GetResponseTicket( void )
	{
		return m_responseTicket;
	}

public:
	~CLongPoll3DMessage(void);

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

private:
	CTicket *m_responseTicket;
};
