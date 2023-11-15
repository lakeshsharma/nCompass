#pragma once
#include "ConfigField.h"

class CConfigCasinoNameField :
	public CConfigField
{
public:
	CConfigCasinoNameField(void);
public:
	~CConfigCasinoNameField(void);

	void SetCasinoName(byte* lpName, int length)
	{
		if (lpName != nullptr && length > 0)
		{
			m_casinoName.assign(reinterpret_cast<char const*>(lpName), (size_t)length);
		}
	}

    const std::string &GetCasinoName()
	{
		return m_casinoName;
	}

private:

	std::string m_casinoName;
};


