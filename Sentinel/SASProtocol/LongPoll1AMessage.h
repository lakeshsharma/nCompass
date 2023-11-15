#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int LP1A = 0x1a;

class CLongPoll1AMessage :
	public CMetersLongPoll
{
public:
	CLongPoll1AMessage(byte gameAddress, CMeters *meters);
public:
	~CLongPoll1AMessage(void);

	__int64 m_currentCredits;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

};

