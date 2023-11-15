#include "stdafx.h"
#include "ConfigExcessiveVoucherOutEnabledField.h"

CConfigExcessiveVoucherOutEnabledField::CConfigExcessiveVoucherOutEnabledField(void) :
CConfigField(EXCESSIVE_VOUCHER_OUT_ENABLED),
m_enabled(false)
{	
}

CConfigExcessiveVoucherOutEnabledField::~CConfigExcessiveVoucherOutEnabledField(void)
{
}

void CConfigExcessiveVoucherOutEnabledField::SetEnabledFlag(bool enabled)
{
	m_enabled = enabled;
}

bool CConfigExcessiveVoucherOutEnabledField::IsEnabled() const
{
	return m_enabled;
}


