#include "stdafx.h"
#include "DiagnosticsEEPROM.h"
#include "DirectoryManager.h"
#include "TiltsDef.h"

CDiagnosticsEEPROM::CDiagnosticsEEPROM() : CEEPROMConfig(), m_rebootTiltCode(tltUnknown)
{
	m_type = MEMORY_EEPROM;
	m_dwOffset = EEPROM_REBOOT_DIAGNOSTIC_TILTCODE_OFFSET;
	m_dwAllowedSize = EEPROM_REBOOT_DIAGNOSTIC_TILTCODE_SIZE;
	m_bOffsetIsSet = true;

	if (GetMemoryResetStatus())
	{
		BuildYourself();
	}
	if (m_eeprom.GetEEPROMResetStatus())
	{
		m_rebootTiltCode = tltRebootMemoryClear;
	}
}

CDiagnosticsEEPROM::~CDiagnosticsEEPROM()
{
}

TiltCode CDiagnosticsEEPROM::GetRebootTiltCode() const
{
	return m_rebootTiltCode;
}

void CDiagnosticsEEPROM::SetRebootTiltCode(TiltCode code)
{
	if (m_rebootTiltCode != code)
	{
		m_rebootTiltCode = code;

		Write(EEPROM_REBOOT_DIAGNOSTIC_TILTCODE_OFFSET, (BYTE *)&code, EEPROM_REBOOT_DIAGNOSTIC_TILTCODE_SIZE);
		CUtilities::FlushVolume();
	}
}

void CDiagnosticsEEPROM::BuildYourself()
{
	DWORD code = 0;
	Read(EEPROM_REBOOT_DIAGNOSTIC_TILTCODE_OFFSET, (BYTE *)&code, EEPROM_REBOOT_DIAGNOSTIC_TILTCODE_SIZE);
	m_rebootTiltCode = TiltCode(code);
}
