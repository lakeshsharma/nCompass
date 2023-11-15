#include "stdafx.h"
#include "ConfigTierPointsLabelField.h"


CConfigTierPointsLabelField::CConfigTierPointsLabelField(void) : CConfigField(TIER_POINTS_LABEL)
{
}


CConfigTierPointsLabelField::~CConfigTierPointsLabelField(void)
{
}

const std::string &CConfigTierPointsLabelField::GetTierPointsLabel() const
{
	return m_tierPointsLabel;
}

void CConfigTierPointsLabelField::SetTierPointsLabel(byte* lpText, int length)
{
	if (lpText != nullptr && length > 0)
	{
		m_tierPointsLabel.assign(reinterpret_cast<char const*>(lpText), (size_t)length);
	}
}
