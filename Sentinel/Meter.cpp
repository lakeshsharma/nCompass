#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Meter.h"
#include "metersdef.h"

CMeter::CMeter(byte meterNumber)
{
	m_meterNumber = meterNumber;
	m_meterValue = (__int64)INVALID_METER_VALUE;
	// Tracker #20839
	m_meterChanged = false;
}

CMeter::CMeter(byte meterNumber, __int64 meterValue)
{

	m_meterNumber = meterNumber;
	m_meterValue = meterValue;
	// Tracker #20839
	m_meterChanged = false;
}

CMeter::~CMeter(void)
{
}

void CMeter::SetValue( __int64 value )
{
	m_meterValue = value;
}

__int64 CMeter::GetValue( void ) const
{
	return m_meterValue;
}

void CMeter::SetMeterNumber( byte meterNumber )
{
	m_meterNumber = meterNumber;
}

byte CMeter::GetMeterNumber( void ) const
{
	return m_meterNumber;
}

void CMeter::SetMeterChanged()
{
	m_meterChanged = true;
}

bool CMeter::IsMeterChanged() const
{
	return m_meterChanged;
}


