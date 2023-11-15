#pragma once
#include "SASPollMessage.h"

class CLongPollB1Message : public CSASPollMessage
{
public:
	CLongPollB1Message(byte gameAddress);
public:
	~CLongPollB1Message(void);	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	byte m_currentDenom;


};
