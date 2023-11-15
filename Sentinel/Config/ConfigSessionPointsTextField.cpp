#include "stdafx.h"
#include "ConfigSessionPointsTextField.h"

CConfigSessionPointsTextField::CConfigSessionPointsTextField(void) : CConfigField(SESSION_POINTS_TEXT)
{
}

CConfigSessionPointsTextField::~CConfigSessionPointsTextField(void)
{
}

const std::string &CConfigSessionPointsTextField::GetSessionPointsText() const
{
	return m_sessionPointsText;
}

void CConfigSessionPointsTextField::SetSessionPointsText(byte* lpText, int length)
{
	if (lpText != nullptr && length > 0)
	{
		m_sessionPointsText.assign(reinterpret_cast<char const*>(lpText), (size_t)length);
	}
}
