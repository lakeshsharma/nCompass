#pragma once
#include "SASPollMessage.h"
#include "Meters.h"
class CMetersLongPoll :
	public CSASPollMessage
{
public:
	CMetersLongPoll(byte gameAddress);
	virtual ~CMetersLongPoll();
	CMeters *GetMeters();

protected:
	CMeters *m_meters;
};

