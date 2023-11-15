#pragma once
#include "SASPollMessage.h"

#define X56_RESPONSE_EXTRADATASIZE 5
#define X56_LENGTHBYTE 2
#define X56_LENGTHSIZE 1
#define X56_DEFSIZE 2
#define X56_GAMENUMBERLOCATION 4

class CLongPoll56Message :
	public CSASPollMessage
{
public:
	CLongPoll56Message(byte gameAddress);
public:
	~CLongPoll56Message(void);

	DWORD m_numberSubGamesImplemented;
	DWORD m_enabledGames;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	void SetImplementedGames(int implementedGames);
	void SetEnabledGamesList(WORD *enabledGamesList);

private:
	int m_implementedGames;
	WORD *m_enabledGamesList;
};
