#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int X2A_MAX_METERS = 1;
const int LP2A = 0x2a;

class CLongPoll2AMessage :
	public CMetersLongPoll
{
public:
	CLongPoll2AMessage(byte gameAddress, CMeters *meters);
public:
	~CLongPoll2AMessage(void);
public:
	__int64 m_trueCoinIn;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
};

