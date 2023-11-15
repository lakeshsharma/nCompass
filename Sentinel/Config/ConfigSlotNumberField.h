#pragma once
#include "ConfigField.h"

class CConfigSlotNumberField :
	public CConfigField
{
public:
	CConfigSlotNumberField(void);
	~CConfigSlotNumberField(void);

	void SetSlotNumber(const byte * SlotNumber, int length);
	DWORD GetSlotNumber() const;

private:
	DWORD m_slotNumber;
};


