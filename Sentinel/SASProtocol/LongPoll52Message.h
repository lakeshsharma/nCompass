#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

#define X52_POLLSIZE 6
#define X52_RESPONSESIZE 22
const int X52_MAX_METERS = 4;
const int LP52 = 0x52;

class CLongPoll52Message :
	public CMetersLongPoll
{
public:
	CLongPoll52Message(byte gameAddress, CMeters *meters);
public:
	~CLongPoll52Message(void);

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

	void SetGameNumber(int gameNumber);
	__int64 m_coinIn;
	__int64 m_coinOut;
	__int64 m_jackpot;
	__int64 m_gamesPlayed;

	
private:
	int m_gameNumber;


};

