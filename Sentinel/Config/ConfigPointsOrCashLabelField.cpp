#include "stdafx.h"
#include "ConfigPointsOrCashLabelField.h"

CConfigPointsOrCashLabelField::CConfigPointsOrCashLabelField(void) : CConfigField(POINTS_OR_CASH_LABEL)
{
}

CConfigPointsOrCashLabelField::~CConfigPointsOrCashLabelField(void)
{
}

const std::string &CConfigPointsOrCashLabelField::GetPointsOrCashLabel() const
{
	return m_pointsOrCashLabel;
}

void CConfigPointsOrCashLabelField::SetPointsOrCashLabel(byte* lpLabel, int length)
{
	if (lpLabel != nullptr && length > 0)
	{
		m_pointsOrCashLabel.assign(reinterpret_cast<char const*>(lpLabel), (size_t)length);
	}
}
