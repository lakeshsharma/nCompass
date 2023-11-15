#pragma once
#include "SASPollMessage.h"


class CLongPoll1BMessage :
	public CSASPollMessage
{
public:
	CLongPoll1BMessage(byte gameAddress);
public:
	~CLongPoll1BMessage(void);
public:
	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	bool isEmpty();
	bool KeyToCreditAvailable();

	byte m_progressiveGroup;
	byte m_progressiveLevel;
	__int64 m_amount;
	WORD m_partialAmountPaid;
	byte m_resetID;

	typedef enum
	{
		StandardResetAvailable = 0,
		CreditMeterResetAvailable = 1,
	}ResetIDType;

};
