#include "stdafx.h"
#include "SASMeter.h"

CSASMeter::CSASMeter(byte meterNumber)
: CMeter(meterNumber)
{
	m_meterCode = 0;
	m_longPoll = 0;
}

CSASMeter::~CSASMeter(void)
{
}

