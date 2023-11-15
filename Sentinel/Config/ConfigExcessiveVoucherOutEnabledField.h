#pragma once
#include "ConfigField.h"

class CConfigExcessiveVoucherOutEnabledField : public CConfigField
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CConfigExcessiveVoucherOutEnabledField"/> class.
	/// </summary>
	CConfigExcessiveVoucherOutEnabledField(void);

	/// <summary>
	/// Finalizes an instance of the <see cref="CConfigExcessiveVoucherOutEnabledField"/> class.
	/// </summary>
	~CConfigExcessiveVoucherOutEnabledField(void);

	/// <summary>
	/// Sets the bill enabled flag.
	/// </summary>
	/// <param name="enabled">if set to <c>true</c> [enabled].</param>
	void SetEnabledFlag(bool enabled);

	/// <summary>
	/// Checks if excessive voucher out is enabled.
	/// </summary>
	/// <returns>true if excessive voucher out is enabled, false otherwise.</returns>
	bool IsEnabled() const;

private:
	bool m_enabled;
};


