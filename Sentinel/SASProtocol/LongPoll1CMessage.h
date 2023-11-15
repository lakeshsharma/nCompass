#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int X1C_MAX_METERS = 8;
const int LP1C = 0x1c;

class CLongPoll1CMessage :
	public CMetersLongPoll
{
public:
	CLongPoll1CMessage(byte gameAddress, CMeters *meters);
public:
	~CLongPoll1CMessage(void);
public:
	__int64 m_coinIn;
	__int64 m_coinOut;
	__int64 m_totalDrop;
	__int64 m_jackpot;
	__int64 m_gamesPlayed;
	__int64 m_gamesWon;
	__int64 m_slotDoorOpen;
	__int64 m_powerReset;


	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
};

