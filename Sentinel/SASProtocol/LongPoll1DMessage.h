#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int X1D_MAX_METERS = 4;
const int LP1D = 0x1d;

class CLongPoll1DMessage :
	public CMetersLongPoll
{
public:
	CLongPoll1DMessage(byte gameAddress, CMeters *meters);
public:
	~CLongPoll1DMessage(void);
public:
	__int64 m_promoCreditIn;
	__int64 m_nonCashCreditIn;
	__int64 m_transferredCredits;
	__int64 m_cashableCredits;


	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	void SetDenomMultiplier(WORD denomMult);
	

private:
	WORD m_denomMult;
};

