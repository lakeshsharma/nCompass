#pragma once
#include "ConfigField.h"
#include "Utilities.h"

class CConfigDenomField :
	public CConfigField
{
public:
	CConfigDenomField(void);
public:
	~CConfigDenomField(void);

	void SetDenom(byte* Denom, int length)
	{
		CUtilities::MemCopyZero(&m_denom, sizeof(m_denom), Denom, length);
	}
	
	WORD GetDenom()
	{
		return m_denom;
	}

private:

	WORD m_denom;
};


