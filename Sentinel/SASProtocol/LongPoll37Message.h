#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int LP37 = 0x37;

class CLongPoll37Message :
	public CMetersLongPoll
{
public:
	CLongPoll37Message(byte gameAddress, CMeters *meters);
public:
	~CLongPoll37Message(void);

	__int64 m_hundred;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
};

