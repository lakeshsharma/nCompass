#pragma once
#include "ConfigField.h"

class CConfigPartialWATOutEnabledField : public CConfigField
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CConfigPartialWATOutEnabledField"/> class.
	/// </summary>
	CConfigPartialWATOutEnabledField(void);

	/// <summary>
	/// Finalizes an instance of the <see cref="CConfigPartialWATOutEnabledField"/> class.
	/// </summary>
	~CConfigPartialWATOutEnabledField(void);

	/// <summary>
	/// Sets the partial WAT out enabled flag.
	/// </summary>
	/// <param name="enabled">if set to <c>true</c> [enabled].</param>
	void SetEnabledFlag(bool enabled);

	/// <summary>
	/// Checks if partial WAT out is enabled.
	/// </summary>
	/// <returns>true if partial WAT out is enabled, false otherwise.</returns>
	bool IsEnabled() const;

private:
	bool m_enabled;
};


