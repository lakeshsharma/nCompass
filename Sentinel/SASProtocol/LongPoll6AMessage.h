#pragma once
#include "SASPollMessage.h"
#include "../Meters.h"

#define X6A_MAX_METERS 1

class CLongPoll6AMessage :
	public CSASPollMessage
{
public:
	CLongPoll6AMessage(byte gameAddress);
public:
	~CLongPoll6AMessage(void);

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	bool CanEFTDownload();
	bool CanEFTUpload();
private:
	byte m_flag;
};

