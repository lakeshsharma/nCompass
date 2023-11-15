#pragma once
#include "SASPollMessage.h"

#define LP01SENDSIZE 4
#define LP01RECVSIZE 1
#define CRCLOC	2

class CLongPoll01Message :
	public CSASPollMessage
{
public:
	CLongPoll01Message(byte gameAddress);
public:
	~CLongPoll01Message(void);
public:

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	
};

