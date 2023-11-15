#include "stdafx.h"
#include "DiagnosticsNvram.h"
#include "Diagnostics.h"
#include "DirectoryManager.h"
#include "Logging/LogString.h"

CDiagnosticsNVRAM::CDiagnosticsNVRAM() : m_rebootTiltCode(tltUnknown)
{
	m_dwOffset = 0;
	m_type = MEMORY_NVRAM;
	m_dwAllowedSize = NVRAM_REBOOT_DIAGNOSTIC_TILTCODE_SIZE;
	m_bOffsetIsSet = true;

    std::vector<BYTE> sprinkles;

	bool sprinklesGood(true);
    if (IntegrityCheck(&sprinkles))
	{
		BuildYourself();
	}
    else
    {
		sprinklesGood = false;
		//There is no need to print the bad sprinkles to the ERROR dump page if know we have to clear NVRAM.
		if (!CUtilities::CheckIfNvramNeedsToBeCleared())
		{
			// write the bad sprinkle values to a file
			CDiagnostics::ReportNVRAMSprinklesToFile(CDirectoryManager::NVRAMBadSprinklesFilePath(), sprinkles);
		}
    }

	string logStr = "Sprinkles NVRAM at start-up:";
	for (vector<BYTE>::iterator sprinkleIter = sprinkles.begin(); sprinkleIter != sprinkles.end(); ++sprinkleIter)
	{
		logStr += FormatString("\t%d", *sprinkleIter);
	}
	LogString(sprinklesGood ? NVRAM_LOG : ERROR_LOG, "%s", logStr.c_str());

	if (IsNVRAMCleared())
	{
		LogString(NVRAM_LOG,"%s %d", __FUNCTION__, __LINE__);
		m_rebootTiltCode = tltRebootMemoryClear;
	}
	LogString(NVRAM_LOG,"%s %d m_rebootTiltCode: %u", __FUNCTION__, __LINE__, m_rebootTiltCode);
}

CDiagnosticsNVRAM::~CDiagnosticsNVRAM()
{
}

TiltCode CDiagnosticsNVRAM::GetRebootTiltCode() const
{
	return m_rebootTiltCode;
}

void CDiagnosticsNVRAM::SetRebootTiltCode(TiltCode code)
{
	if (m_rebootTiltCode != code)
	{
		m_rebootTiltCode = code;

		Write(NVRAM_REBOOT_DIAGNOSTIC_TILTCODE_OFFSET, (BYTE *)&code, NVRAM_REBOOT_DIAGNOSTIC_TILTCODE_SIZE);
	}
}

void CDiagnosticsNVRAM::BuildYourself()
{
	DWORD code = 0;
	Read(NVRAM_REBOOT_DIAGNOSTIC_TILTCODE_OFFSET, (BYTE *)&code, NVRAM_REBOOT_DIAGNOSTIC_TILTCODE_SIZE);
	m_rebootTiltCode = TiltCode(code);	
}
