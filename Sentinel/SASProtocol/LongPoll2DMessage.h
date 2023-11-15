#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int X2D_MAX_METERS = 1;
const int LP2D = 0x2d;

class CLongPoll2DMessage :
	public CMetersLongPoll
{
public:
	CLongPoll2DMessage(byte gameAddress, CMeters *meters);
public:
	~CLongPoll2DMessage(void);

	__int64 m_cancelledCredits;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	void SetGameNumber(int gameNumber);

private:
	int m_gameNumber;

};

