#pragma once
#include "Config/EEPROMConfig.h"
#include "TiltsDef.h"

class CDiagnosticsEEPROM :
	public CEEPROMConfig
{
public:
	/// <summary>
	/// Constructor
	/// </summary>
	CDiagnosticsEEPROM();

	/// <summary>
	/// Deconstructor
	/// </summary>
	virtual ~CDiagnosticsEEPROM();

	/// <summary>
	/// Access to the Reboot Tilt Code
	/// </summary>
	/// <returns>Returns the Reboot Tilt Code stored.</returns>
	TiltCode GetRebootTiltCode() const;

	/// <summary>
	/// Sets the Reboot Tilt Code to be stored.
	/// </summary>
	/// <param name="code">Reboot Tilt Code to be stored.</param>
	void SetRebootTiltCode(TiltCode code);

private:
	void BuildYourself();

	TiltCode m_rebootTiltCode;
};

