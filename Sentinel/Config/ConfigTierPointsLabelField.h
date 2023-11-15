#pragma once
#include "ConfigField.h"

class CConfigTierPointsLabelField : public CConfigField
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CConfigTierPointsLabelField(void);

	/// <summary>
	/// Destructor.
	/// </summary>
	virtual ~CConfigTierPointsLabelField(void);

	/// <summary>
	/// Gets the tier points label.
	/// </summary>
	/// <returns>tier points label</returns>
	const std::string &GetTierPointsLabel() const;

	/// <summary>
	/// Sets the tier points label.
	/// </summary>
	/// <param name="lpText">The lp text.</param>
	/// <param name="length">The length.</param>
	void SetTierPointsLabel(byte* lpText, int length);

private:
	std::string m_tierPointsLabel;
};

