#pragma once
#include "SASPollMessage.h"

class CLongPoll51Message : public CSASPollMessage
{
public:
	CLongPoll51Message(byte gameAddress);
public:
	~CLongPoll51Message(void);

	DWORD m_numberSubGamesImplemented;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

};
