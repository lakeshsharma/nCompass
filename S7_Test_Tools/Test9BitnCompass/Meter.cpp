#include "stdafx.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Meter.h"
#include "metersdef.h"

CMeter::CMeter(byte meterNumber)
{
	assert ( meterNumber < mMeterLast );
	m_meterNumber = meterNumber;
	m_meterValue = (__int64)INVALID_METER_VALUE;
	// Tracker #20839
	m_meterChanged = false;
}

CMeter::CMeter(byte meterNumber, __int64 meterValue)
{
	assert ( meterNumber < mMeterLast );
	m_meterNumber = meterNumber;
	m_meterValue = meterValue;
	// Tracker #20839
	m_meterChanged = false;
}

CMeter::~CMeter(void)
{
}

