#pragma once
#include "SASPollMessage.h"

class CLongPollA8Message :
	public CSASPollMessage
{
public:
	CLongPollA8Message(byte gameAddress);
public:
	virtual ~CLongPollA8Message(void);

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

	void SetStandardHandpay();
	void SetCreditReset();

	byte GetAckCode();

	byte m_resetMethod;
	byte m_ACKCode;

	typedef enum
	{
		CreditResetEnabled = 0,
		UnableToResetToCredit = 1,
		NotInHandpay = 2,
	} ACKCodeType;
};
