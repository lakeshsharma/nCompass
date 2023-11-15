#pragma once
#include "SASPollMessage.h"


class CLongPoll85Message :
	public CSASPollMessage
{
public:
	CLongPoll85Message(byte gameAddress);
public:
	~CLongPoll85Message(void);
public:
	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

	byte m_progressiveGroup;
	byte m_progressiveLevel;
	__int64 m_amount;


};
