#pragma once
#include "SASPollMessage.h"
#include "../Ticket.h"

class CLongPoll70Message :
	public CSASPollMessage
{
public:
	CLongPoll70Message(byte gameAddress);

	CTicket *m_responseTicket;

public:
	~CLongPoll70Message(void);
public:
	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	CTicket *GetResponseTicket( void )
	{
		return m_responseTicket;
	}


};
