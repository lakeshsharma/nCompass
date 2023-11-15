#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int LP33 = 0x33;

class CLongPoll33Message :
	public CMetersLongPoll
{
public:
	CLongPoll33Message(byte gameAddress, CMeters *meters);
public:
	~CLongPoll33Message(void);

	__int64 m_five;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
};

