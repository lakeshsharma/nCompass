#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int X19_MAX_METERS = 5;
const int LP19 = 0x19;

class CLongPoll19Message :
	public CMetersLongPoll
{
public:
	CLongPoll19Message(byte gameAddress, CMeters *meters);
public:
	~CLongPoll19Message(void);
public:
	__int64 m_coinIn;
	__int64 m_coinOut;
	__int64 m_totalDrop;
	__int64 m_jackpot;
	__int64 m_gamesPlayed;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

};

