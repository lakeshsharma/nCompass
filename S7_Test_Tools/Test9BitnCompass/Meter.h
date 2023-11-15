#pragma once

#include "metersdef.h"

class CMeter
{
public:
	CMeter(byte meterNumber);
	CMeter(byte meterNumber, __int64 meterValue);
public:
	virtual ~CMeter(void);

	void SetValue( __int64 value )
	{
		m_meterValue = value;
	}

	__int64 GetValue( void )
	{
		return m_meterValue;
	}

	void SetMeterNumber( byte meterNumber )
	{
		m_meterNumber = meterNumber;
	}

	byte GetMeterNumber( void )
	{
		return m_meterNumber;
	}

	void SetMeterChanged()
	{
		m_meterChanged = true;
	}

	bool IsMeterChanged()
	{
		return m_meterChanged;
	}

private:
	byte m_meterNumber;
	__int64 m_meterValue;

	// Tracker #20839
	bool m_meterChanged; // used to determine which meters to send to SCF when processesing meters.

};
