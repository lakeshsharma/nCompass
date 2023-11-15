
#pragma once
#include "SASPollMessage.h"
#include "Meters.h"

#define X19_MAX_METERS 5

class CLongPoll19Message :
	public CSASPollMessage
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

private:
	CMeters *m_meters;
	
};
