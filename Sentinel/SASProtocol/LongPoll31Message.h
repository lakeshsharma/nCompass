#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int LP31 = 0x31;

class CLongPoll31Message :
	public CMetersLongPoll
{
public:
	CLongPoll31Message(byte gameAddress, CMeters *meters);
public:
	~CLongPoll31Message(void);

	__int64 m_one;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
};

