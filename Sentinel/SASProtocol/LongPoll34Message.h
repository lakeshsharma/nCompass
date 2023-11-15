#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int LP34 = 0x34;

class CLongPoll34Message :
	public CMetersLongPoll
{
public:
	CLongPoll34Message(byte gameAddress, CMeters *meters);
public:
	~CLongPoll34Message(void);

	__int64 m_ten;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
};

