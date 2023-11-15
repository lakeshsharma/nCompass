#pragma once
#include "SASPollMessage.h"

#define X53_POLLSIZE 6
#define X53_RESPONSESIZE 26

class CLongPoll53Message :
	public CSASPollMessage
{
public:
	CLongPoll53Message(byte gameAddress);
public:
	~CLongPoll53Message(void);

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

	void SetGameNumber(int gameNumber);

	char m_gameID[3];
	char m_additionalID[4];
	byte m_denomination;
	byte m_maxBet;
	byte m_progressiveGroup;
	byte m_gameOptions[2];
	char m_payTable[7];
	char m_basePercent[5];

private:
	int m_gameNumber;
};
