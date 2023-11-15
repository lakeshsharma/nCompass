#pragma once
#include "SASPollMessage.h"

#define X8A_POLLSIZE 9
#define X8A_RESPONSESIZE 1

class CLongPoll8AMessage :
	public CSASPollMessage
{
public:
	static const DWORD MAXIMUM_LEGACY_BONUS_AMOUNT = 99999999; // 8-digit maximum, in credits.

	CLongPoll8AMessage(byte gameAddress);

	~CLongPoll8AMessage(void);

	DWORD m_legacyBonusAmount;
	byte m_legacyBonusTaxStatus;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

private:
};

