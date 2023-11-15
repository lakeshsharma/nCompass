#pragma once
#include "SASPollMessage.h"

#define X0E_POLLSIZE 5
#define X0E_RESPONSESIZE 1

class CLongPoll0EMessage :
	public CSASPollMessage
{
public:
	CLongPoll0EMessage(byte gameAddress);
public:
	~CLongPoll0EMessage(void);

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	void EnableRealTimeReporting( bool bEnable )
	{
		m_bEnableRealTimerReporting = bEnable;
	}

private:
	bool m_bEnableRealTimerReporting;
};
