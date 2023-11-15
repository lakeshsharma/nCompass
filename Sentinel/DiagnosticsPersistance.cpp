#include "stdafx.h"
#include "DiagnosticsPersistance.h"

#include "Logging/LogString.h"

CDiagnosticsPersistance::CDiagnosticsPersistance() :
	m_nvram(),
	m_eeprom(),
	m_rebootTiltCode(tltUnknown)
{
	BuildYourself();
}

CDiagnosticsPersistance::~CDiagnosticsPersistance()
{
}

TiltCode CDiagnosticsPersistance::GetRebootTiltCode() const
{
	return m_rebootTiltCode;
}

void CDiagnosticsPersistance::SetRebootTiltCode(TiltCode code, DiagnosticsPersistFlag flag)
{
	if (m_rebootTiltCode != code)
	{
		LogString(NVRAM_LOG,"%s %d, rebootTiltCode: %u, PersistFlag: %u", __FUNCTION__, __LINE__, code, flag);
		if (DIAGNOSTICS_PERSIST_EEPROM_ONLY != flag && !IsRebootTiltCodeValid(m_nvram.GetRebootTiltCode()))
		{
			m_nvram.SetRebootTiltCode(code);
			m_rebootTiltCode = code;
		}

		if (DIAGNOSTICS_PERSIST_NVRAM_ONLY != flag && !IsRebootTiltCodeValid(m_eeprom.GetRebootTiltCode()))
		{
			m_eeprom.SetRebootTiltCode(code);
			m_rebootTiltCode = code;
		}
	}
}

void CDiagnosticsPersistance::ClearReboot()
{
	m_nvram.SetRebootTiltCode(tltUnknown);
	m_eeprom.SetRebootTiltCode(tltUnknown);
	m_rebootTiltCode = tltUnknown;
}

void CDiagnosticsPersistance::BuildYourself()
{
	// Unless not set, value in NVRAM takes precedence over one stored in EEPROM.
	TiltCode code = m_nvram.GetRebootTiltCode();
	if (!IsRebootTiltCodeValid(code))
	{
		code = m_eeprom.GetRebootTiltCode();
		if (!IsRebootTiltCodeValid(code))
		{
			m_rebootTiltCode = tltRebootUnknown;
		}
	}
	m_rebootTiltCode = code;
}

bool CDiagnosticsPersistance::IsRebootTiltCodeValid(TiltCode code)
{
	return (code > tltRebootUnknown && code < tltRebootLastCode);
}
