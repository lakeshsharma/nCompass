#pragma once
#include "ConfigField.h"

class CConfigCommIDField :
	public CConfigField
{
public:
	CConfigCommIDField(void);
public:
	~CConfigCommIDField(void);

	void SetCommID(byte commID)
	{
		m_commID = commID;
	}

	byte GetCommID()
	{
		return m_commID;
	}

private:
	byte m_commID;

};


