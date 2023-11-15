#pragma once
#include "SASPollMessage.h"

#define XB6_POLLSIZE 6
#define XB6_RESPONSESIZE 6

class CLongPollB6Message :
	public CSASPollMessage
{
public:
	CLongPollB6Message(byte gameAddress);
public:
	~CLongPollB6Message(void);

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	byte GetStatus();
	void SetAcknowledge();
	void SetReady();
	void SetUnable();

private:
	byte m_status;
};
