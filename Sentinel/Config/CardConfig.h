#pragma once
#include "Config.h"
#include "GlobalDefs.h"

class CCardConfig
{
public:
	static const BYTE INVALID_CARD_PREFIX = 0x00;

	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CCardConfig"/> class.
	/// </summary>
	CCardConfig();

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CCardConfig"/> class.
	/// </summary>
	~CCardConfig();

	/// <summary>
	/// Setups the card configuration based on main CConfig.
	/// </summary>
	/// <param name="config">IN - The CConfig object</param>
	void Setup(const CConfig &config);

	/// <summary>
	/// Sets the CommID.
	/// </summary>
	/// <param name="commID">IN - The CommID.</param>
	/// <returns>The previous CommID.</returns>
	byte SetCommID(byte commID);

	/// <summary>
	/// Gets the CommID.
	/// </summary>
	/// <returns>The CommID.</returns>
	byte GetCommID() const;

	/// <summary>
	/// Sets the RCCL cards enabled status.
	/// </summary>
	/// <param name="rcclCardsEnabled">IN - true to enable RCCL cards, false to disable.</param>
	/// <returns>The previous value for the the RCCL cards enabled status.</returns>
	bool SetRcclCardsEnabled(bool rcclCardsEnabled);

	/// <summary>
	/// Gets the RCCL cards enabled status.
	/// </summary>
	/// <returns>true if RCCL cards are enabled, false if disabled.</returns>
	bool RcclCardsEnabled() const;

	/// <summary>
	/// Sets the NCL cards enabled status.
	/// </summary>
	/// <param name="rcclCardsEnabled">IN - true to enable NCL cards, false to disable.</param>
	/// <returns>The previous value for the the NCL cards enabled status.</returns>
	bool SetNclCardsEnabled(bool nclCardsEnabled);

	/// <summary>
	/// Gets the NCL cards enabled status.
	/// </summary>
	/// <returns>true if NCL cards are enabled, false if disabled.</returns>
	bool NclCardsEnabled() const;

	/// <summary>
	/// Sets the player card prefixes.
	/// </summary>
	/// <param name="playerCardsPrefix">IN - The player card prefixes.</param>
	void SetPlayerCardPrefixes(const std::vector<BYTE> &playerCardsPrefix);

	/// <summary>
	/// Gets the player card prefixes.
	/// </summary>
	/// <returns>A pointer to the valid card player prefixes.</returns>
	const std::vector<BYTE> GetPlayerCardPrefixes() const;

	/// <summary>
	/// Sets the global card prefix.
	/// </summary>
	/// <param name="globalCardPrefix">IN - The global card prefix.</param>
	/// <returns>The previous value for the global card prefix.</returns>
	BYTE SetGlobalCardPrefix(BYTE globalCardPrefix);

	/// <summary>
	/// Gets the global card prefix.
	/// </summary>
	/// <returns>The value for the global card prefix.</returns>
	BYTE GetGlobalCardPrefix() const;

	/// <summary>
	/// Sets the mechanic card prefix.
	/// </summary>
	/// <param name="mechanicCardPrefix">IN - The mechanic card prefix.</param>
	/// <returns>The previous value for the mechanic card prefix.</returns>
	BYTE SetMechanicCardPrefix(BYTE mechanicCardPrefix);

	/// <summary>
	/// Gets the mechanic card prefix.
	/// </summary>
	/// <returns>The value for the mechanic card prefix.</returns>
	BYTE GetMechanicCardPrefix() const;

	/// <summary>
	/// Sets the floor card prefix.
	/// </summary>
	/// <param name="floorCardPrefix">IN - The floor card prefix.</param>
	/// <returns>The previous value for the floor card prefix.</returns>
	BYTE SetFloorCardPrefix(BYTE floorCardPrefix);

	/// <summary>
	/// Gets the floor card prefix.
	/// </summary>
	/// <returns>The value for the floor card prefix.</returns>
	BYTE GetFloorCardPrefix() const;

	/// <summary>
	/// Sets the temporary card prefix.
	/// </summary>
	/// <param name="tempCardPrefix">IN - The temporary card prefix.</param>
	/// <returns>The previous value for the temporary card prefix.</returns>
	BYTE SetTempCardPrefix(BYTE tempCardPrefix);

	/// <summary>
	/// Gets the temporary card prefix.
	/// </summary>
	/// <returns>The value for the temporary card prefix.</returns>
	BYTE GetTempCardPrefix() const;

	/// <summary>
	/// Sets whether Universe cards are enabled.
	/// </summary>
	/// <param name="universeEnabled">IN - true to enable Universe cards, false to disable.</param>
	/// <returns>The previous value for whether Universe cards were enabled</returns>
	bool SetUniverseEnabled(bool universeEnabled);

	/// <summary>
	/// Gets whether Universe cards are enabled.
	/// </summary>
	/// <returns>true if Universe cards are enabled, false if disabled.</returns>
	bool GetUniverseEnabled(void) const;

	/// <summary>
	/// Sets whether mixed mode is enabled.
	/// </summary>
	/// <param name="mixedModeEnabled">IN - The new value for mixed mode enabled -- true to enable mixed mode, false to disable.</param>
	/// <returns>The previous value for whether mixed mode was enabled.</returns>
	bool SetMixedModeEnabled(bool mixedModeEnabled);

	/// <summary>
	/// Returns true if mixed mode is enabled, false if disabled.
	/// </summary>
	/// <returns>true if mixed mode is enabled, false if disabled.</returns>
	bool GetMixedModeEnabled(void) const;

	/// <summary>
	/// Gets whether "prefix" is a Universe player card prefix.
	/// </summary>const BYTE * play
	/// <param name="prefix">IN - The card prefix to test.</param>
	/// <returns>true if "prefix" is a Universe player card prefix, false otherwise.</returns>
	bool IsUniversePlayerCardPrefix(byte prefix) const;

	/// <summary>
	/// Sets the Universe player card prefixes.
	/// </summary>
	/// <param name="playerCardsPrefix">IN - The Universe player card prefixes.</param>
	void SetUniversePrefixes(const std::vector<BYTE> &universePrefixes);

	/// <summary>
	/// Gets the Universe player card prefixes.
	/// </summary>
	/// <returns>A pointer to the valid Universe player card prefixes.</returns>
	/// <remarks>
	/// The prefixCount is the count last set by SetUniversePrefixes() or SetUniversePropCodes().
	/// </remarks>
	const std::vector<BYTE>GetUniverseCardPrefix() const;

	/// <summary>
	/// Sets the Universe property codes.
	/// </summary>
	/// <param name="universePropCodes">IN - The Universe property codes.</param>
	void SetUniversePropCodes(const std::vector<WORD> &universePropCodes);

    /// <summary>
    /// Gets the Universe property codes.
    /// </summary>
    /// <returns>A pointer to the Universe property codes stored. </returns>
    /// <remarks>
    /// The propCodeCount is the count last set by SetUniversePrefixes() or SetUniversePropCodes().
    /// </remarks>
    const std::vector<WORD> GetUniversePropCodes() const;

	/// <summary>
	/// Set the configured card reader track number.
	/// </summary>
	/// <param name="cardReaderTrackNumber">IN - The card reader track number.</param>
	/// <returns>The previous value of the card reader track.</returns>
	/// <remarks>
	/// If cardReaderTrackNumber is not valid then set the card reader track to the default value.
	/// </remarks>
	BYTE SetCardReaderTrackNumber(BYTE cardReaderTrackNumber);

	/// <summary>
	/// Gets the configured card reader track number.
	/// </summary>
	/// <returns>The configured card reader track number.</returns>
	BYTE CardReaderTrackNumber() const;

	/// <summary>
	/// Gets the RCCL card player prefix.
	/// </summary>
	/// <returns>The RCCL card player prefix.</returns>
	BYTE GetRcclCardPlayerPrefix() const;

	/// <summary>
	/// Sets whether to use SDS card format or not
	/// </summary>
	/// <param name="value">Use SDS card format</param>
	/// <returns>the previous value of use SDS card format.</returns>
	bool SetUseSDSCardFormat(bool value);

	/// <summary>
	/// Returns whether to use SDS card format or not.
	/// </summary>
	/// <returns>Use SDS card format</returns>
	bool GetUseSDSCardFormat() const;

private:
	// Disable default copy constructor and default assignment operator.
	CCardConfig(const CCardConfig &rhs);
	CCardConfig& operator=(const CCardConfig &rhs);

    void ClearPlayerCardPrefixes();
    void ClearUniverseCardPrefixes();
    void ClearUniversePropCodes();

	BYTE m_commID;
	bool m_rcclCardsEnabled;
	bool m_nclCardsEnabled;
	std::vector<BYTE> m_playerCardPrefixes;
	BYTE m_globalCardPrefix;
	BYTE m_mechanicCardPrefix;
	BYTE m_floorCardPrefix;
	BYTE m_tempCardPrefix;
	bool m_universeEnabled;
	bool m_mixedModeEnabled;
	std::vector<BYTE> m_universePlayerCardPrefixes;
	std::vector<WORD> m_universePropCodes;
	BYTE m_cardReaderTrackNumber;
	bool m_useSDSCardFormat;

};

