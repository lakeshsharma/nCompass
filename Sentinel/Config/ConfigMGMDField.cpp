#include "stdafx.h"
#include "ConfigMGMDField.h"

CConfigMGMDField::CConfigMGMDField(void) :
CConfigField(GLOBAL_MGMD_ENABLED),
m_globalMGMD(false)
{	
}

CConfigMGMDField::~CConfigMGMDField(void)
{
}

void CConfigMGMDField::SetGlobalMGMD(bool MGMDEnabled)
{
	m_globalMGMD = MGMDEnabled;
}

bool CConfigMGMDField::IsGlobalMGMD() const
{
	return m_globalMGMD;
}


