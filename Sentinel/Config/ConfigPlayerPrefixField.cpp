#include "stdafx.h"
#include "ConfigPlayerPrefixField.h"
#include "Utilities.h"

CConfigPlayerPrefixField::CConfigPlayerPrefixField(void) :
	CConfigField(PLAYER_PREFIXES)
{
}

CConfigPlayerPrefixField::~CConfigPlayerPrefixField(void)
{
}

void CConfigPlayerPrefixField::SetPlayerPrefixes(const byte* prefixes, int size)
{
	if (prefixes == NULL)
	{
		size = 0;
	}

    for (int prefixIndex = 0; prefixIndex < size; prefixIndex++) 
    {
        m_playerPrefixes.push_back(*prefixes);
        prefixes++;

    }
}


/// <summary>
/// Returns the pointer to the Universe Penn (player) card OCR prefixes stored.
/// </summary>
/// <returns>The pointer to the Universe Penn (player) card OCR prefixes stored.</returns>
const std::vector<BYTE> CConfigPlayerPrefixField::GetPlayerPrefixes() const
{
	return m_playerPrefixes;
}


