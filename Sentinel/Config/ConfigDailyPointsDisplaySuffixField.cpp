#include "stdafx.h"
#include "ConfigDailyPointsDisplaySuffixField.h"


CConfigDailyPointsDisplaySuffixField::CConfigDailyPointsDisplaySuffixField(void) : CConfigField(DAILY_POINTS_DISPLAY_SUFFIX)
{
}


CConfigDailyPointsDisplaySuffixField::~CConfigDailyPointsDisplaySuffixField(void)
{
}

const std::string &CConfigDailyPointsDisplaySuffixField::GetDailyPointsDisplaySuffixText() const
{
	return m_DailyPointsDisplaySuffixText;
}

void CConfigDailyPointsDisplaySuffixField::SetDailyPointsDisplaySuffixText(byte* lpText, int length)
{
	if (lpText != nullptr && length > 0)
	{
		m_DailyPointsDisplaySuffixText.assign(reinterpret_cast<char const*>(lpText), (size_t)length);
	}
}
