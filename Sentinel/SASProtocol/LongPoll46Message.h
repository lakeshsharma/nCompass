#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int X46_MAX_METERS = 1;
const int LP46 = 0x46;

class CLongPoll46Message :
	public CMetersLongPoll
{
public:
	CLongPoll46Message(byte gameAddress, CMeters *meters);
public:
	~CLongPoll46Message(void);

	__int64 m_billsAcceptedCredits;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
};

