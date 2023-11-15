#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int LP38 = 0x38;

class CLongPoll38Message :
	public CMetersLongPoll
{
public:
	CLongPoll38Message(byte gameAddress, CMeters *meters);
public:
	~CLongPoll38Message(void);

	__int64 m_fivehundred;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
};

