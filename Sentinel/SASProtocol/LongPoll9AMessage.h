#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int X9A_MAX_METERS = 1;
const int LP9A = 0x9a;

class CLongPoll9AMessage :
	public CMetersLongPoll
{
public:
	CLongPoll9AMessage(byte gameAddress, CMeters *meters);
public:
	~CLongPoll9AMessage(void);
public:
	__int64 m_deductibleBonus;
	__int64 m_nonDeductibleBonus;
	__int64 m_wagerMatchBonus;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	void SetGameNumber(int gameNumber);

private:
	int m_gameNumber;

	static const BYTE PollValue = 0x9A;
};

