#pragma once
#include "ConfigField.h"
#include "../GlobalDefs.h"

class CConfigUniversePropCodesField :
	public CConfigField
{
public:
	CConfigUniversePropCodesField(void);
	~CConfigUniversePropCodesField(void);

	void SetUniversePropCodes(const byte* propCodes, int size);
	const std::vector<WORD> GetUniversePropCodes() const;
	int GetUniversePropCodeCount() const;

private:
	std::vector<WORD> m_universePropCodes;
};


