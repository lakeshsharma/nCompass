#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int X2B_MAX_METERS = 1;
const int LP2B = 0x2b;

class CLongPoll2BMessage :
	public CMetersLongPoll
{
public:
	CLongPoll2BMessage(byte gameAddress, CMeters *meters);
public:
	~CLongPoll2BMessage(void);
public:
	__int64 m_trueCoinOut;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
};

