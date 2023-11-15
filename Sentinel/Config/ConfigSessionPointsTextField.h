#pragma once
#include "ConfigField.h"
class CConfigSessionPointsTextField :
	public CConfigField
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CConfigSessionPointsTextField(void);

	/// <summary>
	/// Destructor.
	/// </summary>
	virtual ~CConfigSessionPointsTextField(void);

	/// <summary>
	/// Returns text to display when configured to only show session points.
	/// </summary>
	/// <returns>Session Points text string.</returns>
    const std::string &GetSessionPointsText() const;

	/// <summary>
	/// Sets text to display when configured to only show session points.
	/// </summary>
	/// <param name="lpText">IN - buffer with text to display.</param>
	/// <param name="length">IN - length of buffer.</param>
	void SetSessionPointsText(byte* lpText, int length);

private:
    std::string m_sessionPointsText;
};

