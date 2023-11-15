#pragma once
#include "SASPollMessage.h"

#define X1F_POLLSIZE 2
#define X1F_RESPONSESIZE 24

class CLongPoll1FMessage : public CSASPollMessage
{
public:
	CLongPoll1FMessage(byte gameAddress);
public:
	~CLongPoll1FMessage(void);

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

	char m_gameID[3];
	char m_additionalID[4];
	byte m_denomination;
	byte m_maxBet;
	byte m_progressiveGroup;
	byte m_gameOptions[2];
	char m_payTable[7];
	char m_basePercent[5];

};
