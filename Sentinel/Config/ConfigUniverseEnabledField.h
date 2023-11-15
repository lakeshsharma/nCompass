#pragma once
#include "ConfigField.h"
#include "Utilities.h"

class CUniverseEnabledField :
	public CConfigField
{
public:
	CUniverseEnabledField(void);
	~CUniverseEnabledField(void);

	void SetUniverseFields(const byte * buffer, int length);
	bool GetUniverseEnabled() const;
	bool GetMixedModeEnabled() const;

private:
	byte m_universeEnabled;
	byte m_mixedModeEnabled;
};

