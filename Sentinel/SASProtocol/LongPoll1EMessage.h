#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int X1E_MAX_METERS = 6;
const int LP1E = 0x1e;

class CLongPoll1EMessage :
	public CMetersLongPoll
{
public:
	CLongPoll1EMessage(byte gameAddress, CMeters *meters);
public:
	~CLongPoll1EMessage(void);
public:
	__int64 m_one;
	__int64 m_five;
	__int64 m_ten;
	__int64 m_twenty;
	__int64 m_fifty;
	__int64 m_hundred;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	
};

