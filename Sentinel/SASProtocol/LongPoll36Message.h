#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int LP36 = 0x36;

class CLongPoll36Message :
	public CMetersLongPoll
{
public:
	CLongPoll36Message(byte gameAddress, CMeters *meters);
public:
	~CLongPoll36Message(void);

	__int64 m_fifty;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
};

