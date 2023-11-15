#pragma once
#include "ConfigField.h"
#include "GlobalDefs.h"

class CConfigCardTimeoutField :
	public CConfigField
{
public:
	CConfigCardTimeoutField(void);
public:
	~CConfigCardTimeoutField(void);

public:

	void SetCardTimeout(byte * cardTimeout, int length)
	{
		if (length >= 0 && ((unsigned int)length >= sizeof(m_cardTimeout)))
		{
			memcpy_s(&m_cardTimeout, sizeof(m_cardTimeout), cardTimeout, sizeof(m_cardTimeout));
		}
		else
		{
			//TBD log error
		}
	}

	DWORD GetCardTimeout()
	{
		return m_cardTimeout;
	}

private:

	DWORD m_cardTimeout;
};


