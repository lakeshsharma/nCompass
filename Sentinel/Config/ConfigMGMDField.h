#pragma once
#include "ConfigField.h"

class CConfigMGMDField : public CConfigField
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CConfigMGMDField"/> class.
	/// </summary>
	CConfigMGMDField(void);

	/// <summary>
	/// Finalizes an instance of the <see cref="CConfigMGMDField"/> class.
	/// </summary>
	~CConfigMGMDField(void);

	/// <summary>
	/// Sets the Global MGMD.
	/// </summary>
	/// <param name="MGMDEnabled">if set to <c>true</c> [Global MGMD].</param>
	void SetGlobalMGMD(bool MGMDEnabled);

	/// <summary>
	/// Checks if Global MGMD enabled.
	/// </summary>
	/// <returns>true if Global MGMD is enabled, false otherwise.</returns>
	bool IsGlobalMGMD() const;

private:
	bool m_globalMGMD;
};


