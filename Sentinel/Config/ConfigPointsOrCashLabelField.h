#pragma once
#include "ConfigField.h"
class CConfigPointsOrCashLabelField :
	public CConfigField
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CConfigPointsOrCashLabelField(void);

	/// <summary>
	/// Destructor.
	/// </summary>
	virtual ~CConfigPointsOrCashLabelField(void);

	/// <summary>
	/// Returns label to display over Points/Cash.
	/// </summary>
	/// <returns>Label string.</returns>
	const std::string &GetPointsOrCashLabel() const;

	/// <summary>
	/// Sets label to display over Points/Cash.
	/// </summary>
	/// <param name="lpLabel">IN - buffer with label to display.</param>
	/// <param name="length">IN - length of buffer.</param>
	void SetPointsOrCashLabel(byte* lpLabel, int length);

private:
	std::string m_pointsOrCashLabel;
};

