#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int LP32 = 0x32;

class CLongPoll32Message :
	public CMetersLongPoll
{
public:
	CLongPoll32Message(byte gameAddress, CMeters *meters);
public:
	~CLongPoll32Message(void);

	__int64 m_two;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
};

