#pragma once
#include "SASPollMessage.h"


class CLongPoll84Message :
	public CSASPollMessage
{
public:
	CLongPoll84Message(byte gameAddress);
public:
	~CLongPoll84Message(void);
public:
	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

	byte m_progressiveGroup;
	byte m_progressiveLevel;
	__int64 m_amount;


};
