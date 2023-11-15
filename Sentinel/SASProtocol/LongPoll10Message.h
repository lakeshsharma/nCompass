#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int X10_MAX_METERS = 1;
const int LP10 = 0x10;


class CLongPoll10Message :
	public CMetersLongPoll
{
public:
	CLongPoll10Message(byte gameAddress, CMeters *meters);
public:
	~CLongPoll10Message(void);

	__int64 m_cancelledCredits;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

};
