#pragma once
#include "SASPollMessage.h"
#include "../Ticket.h"

class CLongPoll4DMessage :
	public CSASPollMessage
{
public:
	CLongPoll4DMessage(byte gameAddress);
public:
	~CLongPoll4DMessage(void);

	void SetPeekOption( bool bPeek )
	{
		m_bPeek = bPeek;
	}

	CTicket *GetResponseTicket( void )
	{
		return m_responseTicket;
	}

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

	bool m_bPeek;

	CTicket *m_responseTicket;
	void SetDefaultExpiration(int defaultDuration)
	{
		m_defaultExpirationDuration = defaultDuration;
	}

private:
	int m_defaultExpirationDuration;
};
