#pragma once

#include "SASPollMessage.h"
#include "../Meters.h"

#define X50_SEND_SIZE 5
#define X50_RESPONSE_SIZE 14
#define X50_MAX_METERS 20


class CLongPoll50Message :
	public CSASPollMessage
{
public:
	CLongPoll50Message(byte gameAddress, CMeters *meters);
public:
	~CLongPoll50Message(void);

	__int64 m_currentCredits;

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();

private:
	CMeters *m_meters;
	byte m_validationType;
	DWORD m_totalValidations;
	DWORD m_cumeAmount;

};
