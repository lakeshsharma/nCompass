#pragma once
#include "ConfigField.h"

class CConfigVirtualSessionField :
	public CConfigField
{
public:
	CConfigVirtualSessionField(void);
	~CConfigVirtualSessionField(void);

	void SetVirtualSessionOptions(const byte * buffer, int length);
	DWORD GetVirtualSessionNoPlayTimeout() const;
	DWORD GetVirtualSessionMaxDuration() const;

private:
	DWORD m_virtualSessionNoPlayTimeout;
	DWORD m_virtualSessionMaxDuration;
};


