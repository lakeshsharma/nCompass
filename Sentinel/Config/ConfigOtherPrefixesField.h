#pragma once
#include "ConfigField.h"

class CConfigOtherPrefixesField :
	public CConfigField
{
public:
	CConfigOtherPrefixesField(void);
public:
	~CConfigOtherPrefixesField(void);

	void SetPrefixes(byte* lpPrefixes,int length)
	{
		if (length >= 4)
		{
			m_globalCardPrefix = lpPrefixes[0];
			m_mechCardPrefix = lpPrefixes[1];
			m_floorCardPrefix = lpPrefixes[2];
			m_tempCardPrefix = lpPrefixes[3];
		}
		else
		{
			//TBD log the error
		}
	}

	byte GetGlobalCardPrefix()
	{
		return m_globalCardPrefix;
	}

	byte GetMechCardPrefix()
	{
		return m_mechCardPrefix;
	}

	byte GetFloorCardPrefix()
	{
		return m_floorCardPrefix;
	}

	byte GetTempCardPrefix()
	{
		return m_tempCardPrefix;
	}

private:

	byte m_globalCardPrefix;
	byte m_mechCardPrefix;
	byte m_floorCardPrefix;
	byte m_tempCardPrefix;
};


