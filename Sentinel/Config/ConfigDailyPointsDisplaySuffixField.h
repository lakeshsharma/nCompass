#pragma once
#include "ConfigField.h"

class CConfigDailyPointsDisplaySuffixField : public CConfigField
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CConfigDailyPointsDisplaySuffixField(void);

	/// <summary>
	/// Destructor.
	/// </summary>
	virtual ~CConfigDailyPointsDisplaySuffixField(void);

	/// <summary>
	/// Gets the welcome point suffix text.
	/// </summary>
	/// <returns>welcome point suffix text</returns>
    const std::string &GetDailyPointsDisplaySuffixText() const;

	/// <summary>
	/// Sets the welcome point suffix text.
	/// </summary>
	/// <param name="lpText">The lp text.</param>
	/// <param name="length">The length.</param>
	void SetDailyPointsDisplaySuffixText(byte* lpText, int length);

private:
    std::string m_DailyPointsDisplaySuffixText;
};

