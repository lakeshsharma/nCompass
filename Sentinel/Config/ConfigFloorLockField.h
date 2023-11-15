#pragma once
#include "ConfigField.h"

class CConfigFloorLockField :
	public CConfigField
{
public:
	CConfigFloorLockField(void);
public:
	~CConfigFloorLockField(void);

	void SetFloorLock(bool floorLock)
	{
		m_floorLock = floorLock;
	}

	bool GetFloorLock()
	{
		return m_floorLock;
	}
private:

	bool m_floorLock;
};


