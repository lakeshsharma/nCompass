#pragma once
#include "ConfigField.h"
#include "../GlobalDefs.h"

class CConfigUniverseCardPrefixField :
	public CConfigField
{
	public:
		CConfigUniverseCardPrefixField(void);
		~CConfigUniverseCardPrefixField(void);

		void SetUniverseCardPrefixes(const byte* prefixes, int size);
		const std::vector<BYTE> GetUniverseCardPrefixes() const;
		int GetUniverseCardPrefixCount()  const;

	private:
    std::vector<BYTE> m_universeCardPrefixes;
};


