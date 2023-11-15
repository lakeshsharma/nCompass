#pragma once
#include "ConfigField.h"
class CConfigBalanceRetrievalTextField :
	public CConfigField
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CConfigBalanceRetrievalTextField(void);

	/// <summary>
	/// Destructor.
	/// </summary>
	virtual ~CConfigBalanceRetrievalTextField(void);

	/// <summary>
	/// Returns text to display if a Guest Player.
	/// </summary>
	/// <returns>Guest Player text string.</returns>
    const std::string &GetBalanceRetrievalText() const;

	/// <summary>
	/// Sets text to display if a Guest Player.
	/// </summary>
	/// <param name="lpText">IN - buffer with text to display.</param>
	/// <param name="length">IN - length of buffer.</param>
	void SetBalanceRetrievalText(byte* lpText, int length);

private:
    std::string m_balanceRetrievalText;
};

