#pragma once
#include "SASPollMessage.h"

#define LP02SENDSIZE 4
#define LP02RECVSIZE 1
#define CRCLOC	2

class CLongPoll02Message :
	public CSASPollMessage
{
public:
	CLongPoll02Message(byte gameAddress);
public:
	~CLongPoll02Message(void);
public:

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	
};

