#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int LP39 = 0x39;

class CLongPoll39Message :
	public CMetersLongPoll
{
public:
	CLongPoll39Message(byte gameAddress, CMeters *meters);
public:
	~CLongPoll39Message(void);

	__int64 m_thousand;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

};

