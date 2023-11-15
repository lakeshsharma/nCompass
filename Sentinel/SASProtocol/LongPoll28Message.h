#pragma once

#include "SASPollMessage.h"
#include "../Meters.h"

#define X28_MAX_METERS 1

class CLongPoll28Message : public CSASPollMessage
{
public:
	CLongPoll28Message(byte gameAddress);
public:
	~CLongPoll28Message(void);

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	byte GetEFTSeed()
	{
		return m_EFTSeed;
	}
	byte m_lastCmd;
	byte m_lastAck;
	byte m_lastStatus;
	DWORD m_lastAmount;

private:
	byte m_EFTSeed;
};
