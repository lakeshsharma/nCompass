#pragma once
#include "../Meter.h"

class CSASMeter :
	public CMeter
{
public:
	CSASMeter(byte meterNumber);
public:
	virtual ~CSASMeter(void);
	void SetLongPoll(byte longPoll)
	{
		m_longPoll = longPoll;
	}

	byte GetLongPoll()
	{
		return m_longPoll;
	}

	void SetMeterCode(byte meterCode)
	{
		m_meterCode = meterCode;
	}

	byte GetMeterCode()
	{
		return m_meterCode;
	}



private:
	byte m_longPoll;
	byte m_meterCode;
};

