#include "stdafx.h"
#include "ConfigUniverseCardPrefixField.h"

#include "Utilities.h"

/// <summary>
/// Constructor for a CConfigUniverseCardPrefixField instance.
/// </summary>
CConfigUniverseCardPrefixField::CConfigUniverseCardPrefixField(void) :
	CConfigField(UNIVERSE_PLAYER_PREFIXES)
{
}

/// <summary>
/// Destructor for a CUniverseEnabledField instance.
/// </summary>
CConfigUniverseCardPrefixField::~CConfigUniverseCardPrefixField(void)
{
}

/// <summary>
/// Set the Universe Penn (player) card OCR prefixes.
/// <para/>
/// If prefixes is NULL then a size of zero is used.
/// <para/>
/// If size is negative then a size of zero is used.
/// If size is greater than SIZE_OF_VALID_CARDS_PREFIX then a size of SIZE_OF_VALID_CARDS_PREFIX is used.
/// </summary>
/// <param name="prefixes">IN - address of the (player) card OCR prefixes -- one byte per prefix.</param>
/// <param name="size">IN - number of prefixes in prefixes.</param>
void CConfigUniverseCardPrefixField::SetUniverseCardPrefixes(const byte* prefixes, int size)
{
	if (prefixes == NULL)
	{
		size = 0;
	}

    for (int prefixIndex = 0; prefixIndex < size; prefixIndex++) 
    {
        m_universeCardPrefixes.push_back(*prefixes);
        prefixes++;

    }
}

/// <summary>
/// Returns the pointer to the Universe Penn (player) card OCR prefixes stored.
/// </summary>
/// <returns>The pointer to the Universe Penn (player) card OCR prefixes stored.</returns>
const std::vector<BYTE> CConfigUniverseCardPrefixField::GetUniverseCardPrefixes() const
{
	return m_universeCardPrefixes;
}


