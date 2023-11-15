#include "stdafx.h"
#include "ConfigExcessiveVoucherOutThresholdField.h"
#include "Utilities.h"

CConfigExcessiveVoucherOutThresholdField::CConfigExcessiveVoucherOutThresholdField(FieldTypes type) : CConfigField(type), 
m_threshold(0)
{	
}

CConfigExcessiveVoucherOutThresholdField::~CConfigExcessiveVoucherOutThresholdField()
{
}

void CConfigExcessiveVoucherOutThresholdField::SetThreshold(const byte* threshold, int length)
{
	CUtilities::MemCopyZero(&m_threshold, sizeof(m_threshold), threshold, length);
}

DWORD CConfigExcessiveVoucherOutThresholdField::GetThreshold() const
{
	return m_threshold;
}


