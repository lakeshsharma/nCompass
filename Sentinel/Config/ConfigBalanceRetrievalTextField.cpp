#include "stdafx.h"
#include "ConfigBalanceRetrievalTextField.h"


CConfigBalanceRetrievalTextField::CConfigBalanceRetrievalTextField(void) : CConfigField(BALANCE_RETRIEVAL_TEXT)
{
}


CConfigBalanceRetrievalTextField::~CConfigBalanceRetrievalTextField(void)
{
}

 const std::string &CConfigBalanceRetrievalTextField::GetBalanceRetrievalText() const
{
	return m_balanceRetrievalText;
}

void CConfigBalanceRetrievalTextField::SetBalanceRetrievalText(byte* lpText, int length)
{
	if (lpText != nullptr && length > 0)
	{
		m_balanceRetrievalText.assign(reinterpret_cast<char const*>(lpText), (size_t)length);
	}
}
