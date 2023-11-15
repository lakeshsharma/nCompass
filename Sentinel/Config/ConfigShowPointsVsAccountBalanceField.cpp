#include "stdafx.h"
#include "Config/ConfigShowPointsVsAccountBalanceField.h"

CConfigShowPointsVsAccountBalanceField::CConfigShowPointsVsAccountBalanceField(void) : CConfigField(SHOW_POINTS_VS_ACCOUNT_BALANCE), m_showPointsVsAccountBalance(false)
{
}

CConfigShowPointsVsAccountBalanceField::~CConfigShowPointsVsAccountBalanceField(void)
{
}

bool CConfigShowPointsVsAccountBalanceField::GetShowPointsVsAccountBalance() const
{
	return m_showPointsVsAccountBalance;
}

void CConfigShowPointsVsAccountBalanceField::SetShowPointsVsAccountBalance(bool flag)
{
	m_showPointsVsAccountBalance = flag;
}


