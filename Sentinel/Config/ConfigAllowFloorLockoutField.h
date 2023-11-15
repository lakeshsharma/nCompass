#pragma once
#include "ConfigField.h"

class CConfigAllowFloorLockoutField :
	public CConfigField
{
public:
	CConfigAllowFloorLockoutField(void);
	~CConfigAllowFloorLockoutField(void);

	void SetAllowFloorLockout(bool allowFloorLockout)
	{
		m_allowFloorLockout = allowFloorLockout;
	}

	bool GetAllowFloorLockout()
	{
		return m_allowFloorLockout;
	}

private:
	bool m_allowFloorLockout;
};


