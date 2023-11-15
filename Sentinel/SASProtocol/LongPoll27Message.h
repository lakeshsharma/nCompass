#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int X27_MAX_METERS = 1;
const int LP27 = 0x27;

class CLongPoll27Message :
	public CMetersLongPoll
{
public:
	CLongPoll27Message(byte gameAddress, CMeters *meters);
public:
	~CLongPoll27Message(void);
public:
	__int64 m_promoCredits;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
};

