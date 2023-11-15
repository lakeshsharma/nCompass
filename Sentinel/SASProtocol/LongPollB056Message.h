#pragma once
#include "SASPollMessage.h"

#define XB056_RESPONSE_EXTRADATASIZE 5
#define XB056_LENGTHBYTE 2
#define XB056_LENGTHSIZE 1
#define XB056_DEFSIZE 2
#define XB056_GAMENUMBERLOCATION 4

class CLongPollB056Message : public CSASPollMessage
{
public:
	CLongPollB056Message(byte gameAddress);
public:
	~CLongPollB056Message(void);

	DWORD m_numberSubGamesImplemented;
	DWORD m_enabledGames;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	void SetImplementedGames(int implementedGames);
	void SetEnabledGamesList(WORD *enabledGamesList);
	void SetDenom(byte denom);

private:
	int m_implementedGames;
	WORD *m_enabledGamesList;
	byte m_denom;
};
