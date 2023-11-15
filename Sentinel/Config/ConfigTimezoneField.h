#pragma once
#include "ConfigField.h"

class CConfigTimezoneField :
	public CConfigField
{
public:
	CConfigTimezoneField(void);
	~CConfigTimezoneField(void);

	void SetTimezoneOptions(const byte * buffer, int length);
	short GetTimezoneOffset( void ) const;
	bool UseDST( void ) const;

private:
	short m_timezoneOffset;
	bool m_bUseDST;
};


