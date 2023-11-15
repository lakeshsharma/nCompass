#pragma once
#include "ConfigField.h"
class CConfigShowPointsVsAccountBalanceField :
	public CConfigField
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CConfigShowPointsVsAccountBalanceField(void);

	/// <summary>
	/// Destructor.
	/// </summary>
	virtual ~CConfigShowPointsVsAccountBalanceField(void);

	/// <summary>
	/// Returns whether to show Points or Account Balance.
	/// </summary>
	/// <returns>'true' if to show Points. Otherwise, show Account Balance.</returns>
	bool GetShowPointsVsAccountBalance() const;

	/// <summary>
	/// Sets whether to show Points or Account Balance.
	/// </summary>
	/// <param name="flag">IN - 'true' if to show Points. Otherwise, show Account Balance.</param>
	void SetShowPointsVsAccountBalance(bool flag);

private:
	bool m_showPointsVsAccountBalance;
};



