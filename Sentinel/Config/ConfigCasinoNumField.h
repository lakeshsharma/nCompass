#pragma once
#include "ConfigField.h"

class CConfigCasinoNumField :
	public CConfigField
{
public:
	CConfigCasinoNumField(void);

public:
	~CConfigCasinoNumField(void);

	void SetCasinoNumber(byte number)
	{
		m_casinoNum = number;
	}

	byte GetCasinoNumber()
	{
		return m_casinoNum;
	}

private:

	byte m_casinoNum;
};


