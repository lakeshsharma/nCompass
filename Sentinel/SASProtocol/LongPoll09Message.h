#pragma once
#include "SASPollMessage.h"

class CLongPoll09Message : public CSASPollMessage
{
public:
	CLongPoll09Message(byte gameAddress);
public:
	~CLongPoll09Message(void);

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill(byte enable);
	void Fill();
	void SetGameNumber(int gameNumber);

private:
	int m_gameNumber;

};
