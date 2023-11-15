#pragma once
#include "ConfigField.h"

class CConfigExcessiveVoucherOutThresholdField : public CConfigField
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CConfigExcessiveVoucherOutThresholdField"/> class.
	/// </summary>
	/// <param name="type">field type</param>
	CConfigExcessiveVoucherOutThresholdField(FieldTypes type);

	/// <summary>
	/// Finalizes an instance of the <see cref="CConfigExcessiveVoucherOutThresholdField"/> class.
	/// </summary>
	~CConfigExcessiveVoucherOutThresholdField();

	/// <summary>
	/// Sets the threshold value.
	/// </summary>
	/// <param name="threshold">threshold value</param>
	void SetThreshold(const byte* threshold, int length);

	/// <summary>
	/// Returns the threshold value.
	/// </summary>
	DWORD GetThreshold() const;

private:
	DWORD m_threshold;
};