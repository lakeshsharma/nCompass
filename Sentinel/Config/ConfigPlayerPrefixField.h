#pragma once
#include "ConfigField.h"
#include "../GlobalDefs.h"

class CConfigPlayerPrefixField :
	public CConfigField
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CConfigPlayerPrefixField(void);

	/// <summary>
	/// Destructor.
	/// </summary>
	~CConfigPlayerPrefixField(void);

	/// <summary>
	/// Set the player card OCR prefixes.
	/// <para/>
	/// If prefixes is NULL then a size of zero is used.
	/// <para/>
	/// If size is negative then a size of zero is used.
	/// If size is greater than SIZE_OF_VALID_CARDS_PREFIX then a size of SIZE_OF_VALID_CARDS_PREFIX is used.
	/// </summary>
	/// <param name="prefixes">IN - address of the player card OCR prefixes -- one byte per prefix.</param>
	/// <param name="size">IN - number of prefixes in prefixes.</param>
	void SetPlayerPrefixes(const byte* prefixes, int size);

	/// <summary>
	/// Returns the pointer to the Universe Penn (player) card OCR prefixes stored.
	/// </summary>
	/// <returns>The pointer to the Universe Penn (player) card OCR prefixes stored.</returns>
	const std::vector<BYTE> GetPlayerPrefixes() const;

private:
	std::vector<BYTE> m_playerPrefixes;
};


