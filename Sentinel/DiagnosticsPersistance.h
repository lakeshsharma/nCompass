#pragma once
#include "DiagnosticsNvram.h"
#include "DiagnosticsEEPROM.h"

enum DiagnosticsPersistFlag
{
	DIAGNOSTICS_PERSIST_BOTH = 0,
	DIAGNOSTICS_PERSIST_NVRAM_ONLY = 1,
	DIAGNOSTICS_PERSIST_EEPROM_ONLY = 2
};

/// <summary>
/// For persisting data associated with diagnostics, such as Reboot Tilt Code.
/// </summary>
/// <remarks>
/// Encapsulate access and storing to both to NVRAM and EEPROM.
/// </remarks>
class CDiagnosticsPersistance
{
public:
	/// <summary>
	/// Constructor
	/// </summary>
	CDiagnosticsPersistance();

	/// <summary>
	/// Deconstructor
	/// </summary>
	virtual ~CDiagnosticsPersistance();

	/// <summary>
	/// Access to the Reboot Tilt Code.
	/// </summary>
	/// <returns>Returns the Reboot Tilt Code stored.</returns>
	/// <remarks>
	/// Unless not set, returns value from NVRAM. Otherwise, returns from EEPROM.
	/// </remarks>
	TiltCode GetRebootTiltCode() const;

	/// <summary>
	/// Sets the Reboot Tilt Code to be stored.
	/// </summary>
	/// <param name="code">Reboot Tilt Code to be stored.</param>
	/// <param name="flag">Where to persist code.</param>
	/// <remarks>
	/// Set both NVRAM and EEPROM, depending on "flag".
	/// </remarks>
	void SetRebootTiltCode(TiltCode code, DiagnosticsPersistFlag flag);

	/// <summary>
	/// Clear the Reboot Tilt Code.
	/// </summary>
	void ClearReboot();

private:
	static bool IsRebootTiltCodeValid(TiltCode code);

	void BuildYourself();

	CDiagnosticsNVRAM m_nvram;
	CDiagnosticsEEPROM m_eeprom;
	TiltCode m_rebootTiltCode;
};

