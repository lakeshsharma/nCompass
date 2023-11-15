#pragma once

#include "SASPollMessage.h"

class CLongPoll55Message : public CSASPollMessage
{
public:
	CLongPoll55Message(byte gameAddress);
public:
	~CLongPoll55Message(void);

	DWORD m_selectedGameNumber;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

};
