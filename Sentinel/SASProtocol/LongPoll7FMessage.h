#pragma once
#include "SASPollMessage.h"

#define X7F_POLLSIZE 11
#define X7F_RESPONSESIZE 1

class CLongPoll7FMessage :
	public CSASPollMessage
{
public:
	CLongPoll7FMessage(byte gameAddress);
public:
	~CLongPoll7FMessage(void);

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

private:
};
