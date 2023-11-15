#pragma once

#include "DataSubscriptionProvider.h"
#include "Hardware/MemoryStorage.h"
#include "Game.h"
#include "InternalMsgContent.h"
#include "binsem.h"
#include "ProcessDump/OperatingSystem.h"

typedef struct _denomCodeMapping
{
	int denomCode;
	int multiplier;
} denomCodeMapping;

const int NUM_DENOM_CODE_MAPPINGS = 27;

const denomCodeMapping denomCodeMap[NUM_DENOM_CODE_MAPPINGS] =
{
	{0,0},
	{1,1},
	{2,5},
	{3,10},
	{4,25},
	{5,50},
	{6,100},
	{7,500},
	{8,1000},
	{9,2000},
	{0x0a,10000},
	{0x0b,20},
	{0x0c,200},
	{0x0d,250},
	{0x0e,2500},
	{0x0f,5000},
	{0x10,20000},
	{0x11,25000},
	{0x12,50000},
	{0x13,100000},
	{0x14,200000},
	{0x15,250000},
	{0x16,500000},
	{0x17,2},
	{0x18,3},
	{0x19,15},
	{0x1a,40}
};


class CGames : public CDataSubscriptionProvider,
			   public CInternalMsgContent<INT_MSG_CONTENT_GAMES>, 
			   public CMemoryStorage 
{
public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CGames"/> class.
	/// </summary>
	/// <param name="memType">IN - Type of the memory.</param>
	CGames(MEMORY_TYPE memType = MEMORY_NONE, IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CGames"/> class.
	/// </summary>
	/// <param name="gRHS">IN - The source instance.</param>
	/// <remarks>
	/// The memory type of the new instance is set to MEMORY_NONE.
	/// </remarks>
	CGames(const CGames &gRHS);

	/// <summary>
	/// Special constructor. Initializes a new instance of the <see cref="CGames"/> class from NVRAM using small paytableID field size for all CGame objects.
	/// </summary>
	/// <param name="bool">IN - Not used</param>
	/// <remarks>
	/// Specialized constructor to be used for Upgrading NVRAM from versions prior to v21.
	/// </remarks>
	CGames(const bool, IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

	~CGames(void);

	/// <summary>
	/// Assignment operator.
	/// </summary>
	/// <param name="gRHS">IN - The source instance.</param>
	/// <returns>A reference to the target of the assignment.</returns>
	/// <remarks>
	/// The memory type of the target instance is set to MEMORY_NONE.
	/// </remarks>
	CGames &operator=(const CGames &gRHS);

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

	void	AddGame(CGame* lpGame);
	void    RemoveGame(DWORD dwGameNumber);

	/// <summary>
	/// If the source game number already exists, update it, otherwise add the source game.
	/// </summary>
	/// <param name="sourceGame">IN - The source game instance to add or update with.</param>
	/// <returns>true if the source game was added, false otherwise.</returns>
	bool SetGame(const CGame *sourceGame);

	/// <summary>
	/// Sets the game with newCurrentGameNumber, if present, to the current game.
	/// </summary>
	/// <param name="newCurrentGameNumber">IN - The new current game number.</param>
	void SetCurrentGame(WORD newCurrentGameNumber);

	/// <summary>
	/// Sets the current denom.
	/// </summary>
	/// <param name="currentDenom">The current denom.</param>
	void SetCurrentDenom(int currentDenomMultiplier);

	/// <summary>
	/// Gets the game count.
	/// </summary>
	/// <returns>The game count.</returns>
	int	GetGameCount() const;

	/// <summary>
	/// Gets a pointer to the const instance for gameNumber.
	/// </summary>
	/// <param name="gameNumber">IN - The game number to get an instance for.</param>
	/// <returns>A pointer to the const instance for gameNumber if a game with gameNumber is present,
	/// otherwise returns nullptr.</returns>
	const CGame *GetGame(DWORD gameNumber) const;

	/// <summary>
	/// Gets a pointer to the instance for gameNumber.
	/// </summary>
	/// <param name="gameNumber">IN - The game number to get an instance for.</param>
	/// <returns>A pointer to the instance for gameNumber if a game with gameNumber is present,
	/// otherwise returns nullptr.</returns>
	CGame *GetGame( DWORD gameNumber );

	CGame *GetEGM();

	/// <summary>
	/// Gets the meter value for the meter number for the base EGM game.
	/// </summary>
	/// <param name="meterNumber">IN - The meter number.</param>
	/// <returns>The value for the meter number for the base EGM game.</returns>
	__int64 GetEgmMeterValue(byte meterNumber);

	/// <summary>
	/// Gets the const game for the specified index.
	/// </summary>
	/// <param name="gameIndex">IN - Index of the game.</param>
	/// <returns>A pointer to the const game for the specified index if the index is value, otherwise returns NULL.</returns>
	const CGame *GetGameByIndex(int gameIndex) const;

	/// <summary>
	/// Gets the game for the specified index.
	/// </summary>
	/// <param name="gameIndex">IN - Index of the game.</param>
	/// <returns>A pointer to the game for the specified index if the index is value, otherwise returns NULL.</returns>
	CGame *GetGameByIndex(int gameIndex);

	bool MemoryValid()
	{
		return !m_memoryReset;
	}

	/// <summary>
	/// Gets a pointer to const for the current game.
	/// </summary>
	/// <returns>A pointer to const for the current game if there is a current game, otherwise returns nullptr.</returns>
	const CGame *GetCurrentGame() const;

	/// <summary>
	/// Gets the current denom.
	/// </summary>
	/// <returns>Current denom mulitiplier.</returns>
	int GetCurrentDenom() const;

	/// <summary>
	/// Gets a pointer to the current game.
	/// </summary>
	/// <returns>A pointer to the current game if there is a current game, otherwise returns nullptr.</returns>
	CGame *GetCurrentGame( void );

	/// <summary>
	/// Gets a pointer to the current session's game.
	/// </summary>
	/// <returns>A pointer to the current game if there is a current game, otherwise returns nullptr.</returns>
	CGame *GetCurrentSessionGame( void );

	/// <summary>
	/// Gets a copy of a game, adjusting the current game number as needed, trying to get an enabled game
	/// with a non-zero game number that represents the game currently selected by the player.
	/// </summary>
	/// <returns>A pointer to the newly created game. Returns nullptr if there are no games.</returns>
	/// <remarks>
	/// The caller is responsible for deleting the returned instance.
	/// </remarks>
	CGame *GetAdjustedCurrentGameCopy() const;

	__int64 ConvertCreditsForSelectedDenom( __int64 creditValue );
	__int64 ConvertCredits( __int64 creditValue );

	void ResetGames(); //Clears out all games and readds the EGM.

	/// <summary>
	/// Sets the sas version.
	/// </summary>
	/// <param name="sasVersion">The sas version.</param>
	void SetSASVersion(const std::string& sasVersion);

	/// <summary>
	/// Gets the SAS version.
	/// </summary>
	/// <returns>The SAS version.</returns>
	std::string GetSASVersion() const;

	/// <summary>
	/// Sets the game identifier.
	/// </summary>
	/// <param name="gameId">The game identifier.</param>
	void SetGameId(const std::string& gameId);

	/// <summary>
	/// Gets the game identifier.
	/// </summary>
	/// <returns>The game identifier.</returns>
	std::string GetGameId() const;

	/// <summary>
	/// Sets the progressive group.
	/// </summary>
	/// <param name="progressiveGroup">The progressive group.</param>
	void SetProgressiveGroup(byte progressiveGroup);

	/// <summary>
	/// Gets the progressive group.
	/// </summary>
	/// <returns>The progressive group.</returns>
	byte GetProgressiveGroup() const;

	/// <summary>
	/// Calculates the progressive group.
	/// </summary>
	/// <remarks>
	/// If m_progressiveGroup is the non-SAS progressive group then try to find a sub-game with
	/// a SAS progressive group and use that.
	/// </remarks>
	void CalcProgressiveGroup();

	/// <summary>
	/// Sets the address.
	/// </summary>
	/// <param name="address">The address.</param>
	void SetAddress(byte address);

	/// <summary>
	/// Gets the address.
	/// </summary>
	/// <returns>The address.</returns>
	byte GetAddress() const;


	/// <summary>
	/// Sets the serial number.
	/// </summary>
	/// <param name="serialNumber">The serial number.</param>
	void SetSerialNumber(const std::string& serialNumber);

	/// <summary>
	/// Gets the serial number.
	/// </summary>
	/// <returns>The serial number.</returns>
	std::string GetSerialNumber() const;

	/// <summary>
	/// Sets the asset number.
	/// </summary>
	/// <param name="assetNumber">The asset number.</param>
	void SetAssetNumber(const std::string& assetNumber);

	/// <summary>
	/// Gets the asset number.
	/// </summary>
	/// <returns>The asset number.</returns>
	std::string GetAssetNumber() const;

	/// <summary>
	/// Sets the accounting denom.
	/// </summary>
	/// <param name="accountingDenom">The accounting denom.</param>
	void SetAccountingDenom(int accountingDenom);

	/// <summary>
	/// Gets the accounting denom.
	/// </summary>
	/// <returns>The accounting denom.</returns>
	int GetAccountingDenom() const;

	/// <summary>
	/// Sets whether the EGM supports multi denom extensions.
	/// </summary>
	/// <param name="multi">IN - true if the EGM supports multi denom extensions, false otherwise.</param>
	void SetMultiDenomEnabled(bool multi);

	/// <summary>
	/// Gets whether the EGM supports multi denom extensions.
	/// </summary>
	/// <returns>true if the EGM supports multi denom extensions, false otherwise.</returns>
	bool GetMultiDenomEnabled() const;

	/// <summary>
	/// Sets the extended meters enabled.
	/// </summary>
	/// <param name="val">The value.</param>
	void SetExtendedMetersEnabled(bool val);

	/// <summary>
	/// Gets the extended meters enabled.
	/// </summary>
	/// <returns>The extended meters enabled.</returns>
	bool GetExtendedMetersEnabled() const;

	/// <summary>
	/// Sets the AFT enabled.
	/// </summary>
	/// <param name="val">The value.</param>
	void SetAftEnabled(bool val);

	/// <summary>
	/// Gets the AFT enabled.
	/// </summary>
	/// <returns>The AFT enabled.</returns>
	bool GetAftEnabled() const;

	/// <summary>
	/// Sets the AFT bonus enabled.
	/// </summary>
	/// <param name="val">The value.</param>
	void SetAftBonusEnabled(bool val);

	/// <summary>
	/// Gets the AFT bonus enabled.
	/// </summary>
	/// <returns>The AFT bonus enabled.</returns>
	bool GetAftBonusEnabled() const;

	/// <summary>
	/// Sets the AFT in house enabled.
	/// </summary>
	/// <param name="val">The value.</param>
	void SetAftInHouseEnabled(bool val);

	/// <summary>
	/// Gets the AFT in house enabled.
	/// </summary>
	/// <returns>The AFT in house enabled.</returns>
	bool GetAftInHouseEnabled() const;

	/// <summary>
	/// Sets the legacy bonus enabled.
	/// </summary>
	/// <param name="val">The value.</param>
	void SetLegacyBonusEnabled(bool val);

	/// <summary>
	/// Gets the legacy bonus enabled.
	/// </summary>
	/// <returns>The legacy bonus enabled.</returns>
	bool GetLegacyBonusEnabled() const;

	/// <summary>
	/// Sets the electronic transfer mode of EGM.
	/// </summary>
	/// <param name="val">The value.</param>
	void SetEGMTransferMode(EGMTransferMode mode);

	/// <summary>
	/// Gets the electronic transfer mode of EGM.
	/// </summary>
	/// <returns>The LP6A succeed status.</returns>
	EGMTransferMode GetEGMTransferMode() const;

	/// <summary>
	/// Sets the EFT transfer to enabled.
	/// </summary>
	/// <param name="val">The value.</param>
	void SetEftTransferToEnabled(bool val);

	/// <summary>
	/// Gets the EFT transfer to enabled.
	/// </summary>
	/// <returns>The EFT transfer to enabled.</returns>
	bool GetEftTransferToEnabled() const;

	/// <summary>
	/// Sets the EFT transfer from enabled.
	/// </summary>
	/// <param name="val">The value.</param>
	void SetEftTransferFromEnabled(bool val);

	/// <summary>
	/// Gets the EFT transfer from enabled.
	/// </summary>
	/// <returns>The EFT transfer from enabled.</returns>
	bool GetEftTransferFromEnabled() const;

	/// <summary>
	/// Sets the validation extensions enabled.
	/// </summary>
	/// <param name="val">The value.</param>
	void SetValidationExtensionsEnabled(bool val);

	/// <summary>
	/// Gets the validation extensions enabled.
	/// </summary>
	/// <returns>The validation extensions enabled.</returns>
	bool GetValidationExtensionsEnabled() const;

	/// <summary>
	/// Sets the last total coin out meter value set
	/// </summary>
	/// <param name="val">IN - The value.</param>
	void SetLastTotalCoinOutMeterValue(__int64 val);

	/// <summary>
	/// Gets the last total coin out meter value set.
	/// </summary>
	/// <returns>The last total coin out meter value set.</returns>
	__int64 GetLastTotalCoinOutMeterValue() const;

	/// <summary>
	/// Gets CGames object consisting of enabled games.
	/// </summary>
	/// <returns>A pointer to a newly created CGames object.</returns>
	/// <remarks>
	/// The caller is responsible for deleting the returned instance.
	/// </remarks>
	CGames *GetEnabledGames() const;

	/// <summary>
	/// Does a shallow copy from games to "this".
	/// </summary>
	/// <param name="games">The games.</param>
	void ShallowCopy(const CGames &games);

	/// <summary>
	/// Updates subgames PaytableID, maxbet and progressivegroup using supplied CGames.
	/// </summary>
	/// <param name="games">The games.</param>
	void UpdateSubGameData(CGames *games);

    /// <summary> 
    /// Get game array 
	/// </summary>
    /// <returns>std::vector<CGame *></returns> 
    std::vector<CGame *>& GetGames();

    /// <summary> 
    /// Update timestamp when EGM or subgame meters are last  
	/// sent to poller via periodic trigger
	/// </summary>
	/// <param name="egmMeters">true if last time of sending EGM meters to be updated, false if for subgame meters</param>
	void UpdateLastMetersSentTime(bool egmMeters);

    /// <summary> 
    /// Is is time to send full meters for games
	/// </summary>
    /// <returns>true if time to send full games meters</returns> 
	/// <param name="egmMeters">true if last time of sending EGM meters to be updated, false if for subgame meters</param>
	bool IsTimeToSendMeters(bool egmMeters);
	
	/// <summary> 
	/// Set to send EGM meters immediately
	/// </summary>
	void ForceEgmMetersUpdate();

	/// <summary> 
	/// Refresh the frequencies on which to send EGM or subgame meters to poller
	/// </summary>
	/// <param name="isNfdPoller">Is poller NFD.</param>
	void RefreshMetersSendFrequency(bool isNfdPoller);

    /// <summary> 
    /// Get timestamp when last game list was received from EGM
	/// </summary>
    /// <returns>timestamp when last game list was received from EGM</returns> 
	time_t GetLastGamelistTimestamp();

    /// <summary> 
    /// Set timestamp when last game list was received from EGM and also update the game counts fetched
	/// </summary>
    /// <params>lastGamelistTimestamp: IN - timestamp when last game was received from EGM</params> 
	/// <returns>Number of games which has its info fetched via SAS</returns> 
	WORD UpdateLastGamelistTimestamp(time_t lastGamelistTimestamp);

    /// <summary> 
    /// Is complete game list fetched
	/// </summary>
    /// <returns>true if complete game list fetched else false</returns> 
    bool IsCompleteGamelistFetched();

    /// <summary> 
    /// Reset game counts fetched status to false. This happens when EGM comes online/ GPE Options change
	/// </summary>
    /// <returns>Set complete game list fetched status to false</returns> 
	void ResetIsCompleteGamelistFetched();

	/// <summary> 
	/// Gets the number of games which has its info fetched via SAS.
	/// </summary>
	/// <returns>Number of games which has its info fetched via SAS</returns> 
	WORD GetGameCountsFetched();

	/// <summary> 
	/// Sets the number of implemented games.
	/// </summary>
	/// <params>value: IN - number of implemented games</params> 
	void SetNumberOfImplementedGames(WORD value);
	
	/// <summary> 
	/// Gets the number of implemented games.
	/// </summary>
	/// <returns>Number of games implemented</returns> 
	WORD GetNumberOfImplementedGames() const;

	/// <summary> 
	/// Sets the number of enabled games.
	/// </summary>
	/// <params>value: IN - number of enabled games</params>
	void SetNumberOfEnabledGames(DWORD value);

	/// <summary> 
	/// Gets the number of enabled games.
	/// </summary>
	/// <returns>Number of games enabled</returns> 
	DWORD GetNumberOfEnabledGames() const;

	/// <summary> 
	/// Checks to see if all the enabled games have been fetched via SAS
	/// </summary>
	/// <returns>true if all the enabled games have been fetched via SAS</returns> 
	bool HaveAllTheEnabledGamesBeenFetched() const;

	/// <summary> 
	/// Syncs the enabled games in NVRAM
	/// </summary>
	/// <returns>false if new enabled games does not match NVRAM, true otherwise.</returns> 
	bool SyncTheEnabledGames(vector<DWORD>* vEnabledGameNumbers);

	/// <summary> 
	/// Gets bonus payment method
	/// </summary>
	/// <returns>bonus payment method</returns> 
	std::string GetBonusPaymentMethod() const;

	/// <summary> 
	/// Checks if a denom passed in is a valid denom.
	/// </summary>
	/// <params>denom: IN - denom to check</params>
	/// <returns>true if denom is a valid denom, false otherwise.</returns> 
	static bool IsDenomValid(WORD denom);

private:
	std::vector<CGame *> m_gamesArray;
	DWORD	GetNewGameOffset();

	virtual void BuildYourself();
	void BuildYourselfPreMGMD();

	bool GameCountsMatch();
	void DecrementEnabledNVRAMGameCount();
	void UpdateGameCount();

	bool m_memoryReset;

	DWORD m_NVRAMGameCount;

	IOperatingSystem &m_operatingSystem;

	// Extra data used by CProgressiveProtocol, not persisted
	std::string m_SASVersion;
	std::string m_gameId;
	byte m_progressiveGroup;
	byte m_address;
	std::string m_serialNumber;
	std::string m_assetNumber;
	int m_accountingDenom;
	bool m_multiDenomEnabled;
	bool m_extendedMetersEnabled;
	bool m_aftEnabled;
	bool m_aftBonusEnabled;
	bool m_aftInHouseEnabled;
	bool m_legacyBonusEnabled;
	EGMTransferMode m_egmTransferMode;
	bool m_eftTransferToEnabled;
	bool m_eftTransferFromEnabled;
	bool m_validationExtensionsEnabled;
	__int64 m_lastTotalCoinOutMeterValue;
	mutable std::recursive_mutex m_criticalSection;
	int m_currentDenomMultiplier;

    DWORD m_lastEgmMetersSendTick;
	DWORD m_egmMetersSendFrequency;

	DWORD m_lastSubgameMetersSendTick;
	DWORD m_subgameMetersSendFrequency;
	bool m_forceEgmMetersUpdate;

	time_t m_lastGamelistTimestamp;
	bool m_completeGamelistFetched;
	WORD m_implementedGames;
	DWORD m_numberOfEnabledGames;
};

