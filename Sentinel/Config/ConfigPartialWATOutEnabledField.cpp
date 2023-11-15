#include "stdafx.h"
#include "ConfigPartialWATOutEnabledField.h"

CConfigPartialWATOutEnabledField::CConfigPartialWATOutEnabledField(void) :
CConfigField(PARTIAL_WAT_OUT_ENABLED),
m_enabled(false)
{	
}

CConfigPartialWATOutEnabledField::~CConfigPartialWATOutEnabledField(void)
{
}

void CConfigPartialWATOutEnabledField::SetEnabledFlag(bool enabled)
{
	m_enabled = enabled;
}

bool CConfigPartialWATOutEnabledField::IsEnabled() const
{
	return m_enabled;
}


