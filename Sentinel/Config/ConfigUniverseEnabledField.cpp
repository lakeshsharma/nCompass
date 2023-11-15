#include "stdafx.h"
#include "ConfigUniverseEnabledField.h"

/// <summary>
/// Constructor for a CUniverseEnabledField instance.
/// </summary>
CUniverseEnabledField::CUniverseEnabledField(void) :
	CConfigField(UNIVERSE_ENABLED),
	m_universeEnabled((byte)false),
	m_mixedModeEnabled((byte)false)
{
}

/// <summary>
/// Destructor for a CUniverseEnabledField instance.
/// </summary>
CUniverseEnabledField::~CUniverseEnabledField(void)
{
}

/// <summary>
/// Set whether support for Universe Penn (player) cards (and property codes) is enabled.
/// Set whether support for Universe Penn legacy (player) cards is enabled.
/// </summary>
/// <param name="buffer">IN - address of source buffer. If buffer is NULL then a length of zero is used.</param>
/// <param name="length">IN - size of source buffer. If length is negative then a length of zero is used.</param>
void CUniverseEnabledField::SetUniverseFields(const byte * buffer, int length)
{
	const void *sourceBuffer = buffer;
	CUtilities::MemCopyZeroIncSourcePtr(&m_universeEnabled, sizeof(m_universeEnabled), sourceBuffer, length); 
	CUtilities::MemCopyZeroIncSourcePtr(&m_mixedModeEnabled, sizeof(m_mixedModeEnabled), sourceBuffer, length); 
}

//// <summary>
/// Returns true if support for Universe Penn (player) cards (and property codes) is enabled.  Returns false otherwise.
/// </summary>
/// <returns>true if support for Universe Penn (player) cards (and property codes) is enabled.  Returns false otherwise.</returns>
bool CUniverseEnabledField::GetUniverseEnabled() const
{
	return m_universeEnabled == (byte)true;
}

/// <summary>
/// Returns true if support for Universe Penn legacy (player) cards is enabled.  Returns false otherwise.
/// </summary>
/// <returns>true if support for Universe Penn legacy (player) cards is enabled.  Returns false otherwise.</returns>
bool CUniverseEnabledField::GetMixedModeEnabled() const
{
	return m_mixedModeEnabled == (byte)true;
}


