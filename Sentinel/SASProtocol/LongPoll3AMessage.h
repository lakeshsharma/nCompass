#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int LP3A = 0x3a;

class CLongPoll3AMessage :
	public CMetersLongPoll
{
public:
	CLongPoll3AMessage(byte gameAddress, CMeters *meters);
public:
	~CLongPoll3AMessage(void);

	__int64 m_twohundred;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
};

