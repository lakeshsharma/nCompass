#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int LP35 = 0x35;

class CLongPoll35Message :
	public CMetersLongPoll
{
public:
	CLongPoll35Message(byte gameAddress, CMeters *meters);
public:
	~CLongPoll35Message(void);

	__int64 m_twenty;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
};

