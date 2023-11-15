#pragma once
#include "SASPollMessage.h"

#define X2E_POLLSIZE 6
#define X2E_RESPONSESIZE 1

class CLongPoll2EMessage :
	public CSASPollMessage
{
public:
	CLongPoll2EMessage(byte gameAddress);
public:
	~CLongPoll2EMessage(void);

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	void SetGameDelay(short gameDelay);

private:
	short m_gameDelay;
};
