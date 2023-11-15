#pragma once
#include "SASPollMessage.h"

class CLongPoll94Message :
	public CSASPollMessage
{
public:
	CLongPoll94Message(byte gameAddress);
public:
	virtual ~CLongPoll94Message(void);

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

	byte GetAckCode();

	byte m_ACKCode;

	typedef enum
	{
		RemoteResetSuccess = 0,
		UnableToRemoteReset = 1,
		NotInHandpay = 2,
	} ACKCodeType;
};
