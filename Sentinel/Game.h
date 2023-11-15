#pragma once
//#include "Hardware/MemoryStorage.h"
#include "Meters.h"

#define EGM_GAME_NUMBER		0

const int ONLY_ONE_GAME_COUNT = 1;
const int SINGLE_GAME_NUMBER = 1;
const int HAS_ONE_SUBGAME_COUNT = 2;
const int PAY_TABLE_ID_LEN = 21; //this includes zero terminator
const int MFG_ID_LEN = 5;		//this includes zero terminator
const int MFG_ID_SAS_LEN = 2;	//2 is the ascii length defined in SAS
const int ADDITIONAL_ID_LEN = 4;	//this includes zero terminator
const int GAME_NAME_LENGTH = 21;	//this includes zero terminator
const int MIN_GAME_NAME_LENGTH = 5;	//this includes zero terminator
const int ADDITIONAL_INFO_LENGTH = 3;//this includes zero terminator


typedef enum
{
	SlotDoor,
	DropDoor,
	LogicDoor,
	CashDoor,
	AuxFillDoor,
	ExtraDoor,
	BellyDoor,
} DoorType;


class CGame :
	public CMemoryStorage
{
public:
	static const byte NON_SAS_PROGRESSIVE_GROUP = 0; // Progressive Group (ID) 0 is reserved for non-sas progressives.
	static const byte DEFAULT_PROGRESSIVE_GROUP = NON_SAS_PROGRESSIVE_GROUP;
	static const DWORD DEFAULT_GAME_NUMBER = (DWORD)-1;
	static const int DEFAULT_DENOMINATION_MULTIPLIER = 1;

	CGame(MEMORY_TYPE memType, bool bIsEGM = false /*SubGame By Default*/);

	/// <summary>
	/// Copy constructor. Creates an instance with the value of the source instance.
	/// </summary>
	/// <param name="gameRHS">IN - The source instance.</param>
	/// <returns>A reference to the created instance.</returns>
	CGame(const CGame &gameRHS);

	~CGame(void);

	/// <summary>
	/// Sets this instance to the value of the source instance.
	/// </summary>
	/// <param name="gameRHS">IN - The source instance.</param>
	/// <returns>A reference to the target of the assigment.</returns>
	CGame &operator=(const CGame &gameRHS);

	//S.Fairbairn 3/12/07 Added to allow the Games object to set memory type
	void SetMemoryType(MEMORY_TYPE memType);
	MEMORY_TYPE GetMemoryType();
	//S.Fairbairn 3/12/07 End

	/// <summary>
	/// This is set by the parent when it's added (to the parent)
	/// </summary>
	/// <param name="dwOffset">IN - offset.</param>
	/// <param name="preMGMD">IN - pre MGMD flag.</param>
	void SetOffset(DWORD dwOffset, bool preMGMD = false);

	/// <summary>
	/// Sets the game data for this game. Does not affect meters.
	/// </summary>
	/// <param name="sourceGame">IN - The source game.</param>
	void SetGameData(const CGame &sourceGame);

	/// <summary>
	/// Sets the non progressive game data for this game. Does not affect meters.
	/// </summary>
	/// <param name="sourceGame">IN - The source game.</param>
	void SetNonProgressiveGameData(const CGame& sourceGame);

	/// <summary>
	/// Sets the meters data for this game.
	/// </summary>
	/// <param name="sourceMeters">IN - The source meters.</param>
	void SetGameMeters(const CMeters &sourceMeters);

	/// <summary>
	/// Sets the game number.
	/// </summary>
	/// <param name="dwGameNumber">IN - The game number.</param>
	void SetGameNumber(DWORD dwGameNumber);

	/// <summary>
	/// Sets the denomination multiplier.
	/// </summary>
	/// <param name="nDenomMultiplier">IN - The denom mulptiplier.</param>
	void SetDenom(int nDenomMultiplier, DenomCurrencyType DenomType = US_DOLLARS);

	/// <summary>
	/// Sets the denomination code and multiplier
	/// </summary>
	/// <param name="denomCode">IN - The denom code.</param>
	/// <param name="nDenomMultiplier">IN - The denom mulptiplier.</param>
	void SetDenomInfo(int code, int nDenomMultiplier, DenomCurrencyType DenomType = US_DOLLARS);

	/// <summary>
	/// Sets the paytable identifier.
	/// </summary>
	/// <param name="lpPaytableID">IN - The paytable identifier.</param>
	void SetPaytableID(const char * lpcPaytableID);

	void SetPaytableID(char * lpcPaytableID);

	/// <summary>
	/// Sets the paytable identifier length.
	/// </summary>
	/// <param name="paytableIDLength">IN - The paytable identifier.</param>
	void SetPaytableIDLength(byte paytableIDLength);

	/// <summary>
	/// Sets the paytable identifier length.
	/// </summary>
	/// <returns>The paytable identifier length.</returns>
	byte GetPaytableIDLength() const;

	/// <summary>
	/// Sets the MFG identifier.
	/// </summary>
	/// <param name="lpcMfgID">IN - The MFG identifier.</param>
	void SetMfgID(const char *lpcMfgID);

	void SetMfgID(char *  lpcMfgID);

	/// <summary>
	/// Sets the additional identifier.
	/// </summary>
	/// <param name="lpcMfgID">IN - The additional identifier.</param>
	void SetAdditionalID(const char *lpcAdditionalID);

	void SetAdditionalID(char *  lpcAdditionalID);

	/// <summary>
	/// Sets the theoretical payback.
	/// </summary>
	/// <param name="wTheoreticalPayback">IN - The theoretical payback.</param>
	void SetTheoreticalPayback(WORD wTheoreticalPayback);

	void SetTheoreticalPayback(char *lpcTheoreticalPayback);
	void Enable();
	void Disable();
	void ChangeCurrentGameStatus(bool bCurrentGame = true);
	void SetEGMIndicator( bool bIsEGM );
	void SetMeter(byte meterNumber, __int64 meterValue);
	void AddToMeter(byte meterNumber, __int64 additionalMeterValue);

	/// <summary>
	/// Gets the meter value for meterNumber.
	/// </summary>
	/// <param name="meterNumber">IN - The meter number.</param>
	/// <returns>The meter value for meterNumber.</returns>
	__int64 GetMeter(byte meterNumber) const;

	__int64 CompareMeter( CMeter *meter );


	/// <summary>
	/// Gets the game number.
	/// </summary>
	/// <returns>The game number.</returns>
	DWORD GetGameNumber() const;

	/// <summary>
	/// Determines whether this game is EGM total for all games.
	/// </summary>
	/// <returns>true if this game is EGM total for all games, false otherwise.</returns>
	bool IsEGM() const;

	/// <summary>
	/// Gets whether this instance is enabled.
	/// </summary>
	/// <returns>true if this instance is enabled, false otherwise.</returns>
	bool IsEnabled() const;

	/// <summary>
	/// Gets whether this is the current game.
	/// </summary>
	/// <returns>true if this is the current game, false otherwise.</returns>
	bool IsCurrentGame() const;

	/// <summary>
	/// Gets the type of the denom.
	/// </summary>
	/// <returns></returns>
	DenomCurrencyType GetDenomType() const;

	/// <summary>
	/// Gets the denom multiplier.
	/// </summary>
	/// <returns>The denom multiplier.</returns>
	int GetDenomMultiplier() const;

	/// <summary>
	/// Gets the denom code.
	/// </summary>
	/// <returns>The denom code.</returns>
	int GetDenomCode() const;

	/// <summary>
	/// Gets the paytable identifier.
	/// </summary>
	/// <returns>A read-only pointer to the paytable identifier.</returns>
	const char *GetPaytableID() const;

	/// <summary>
	/// Gets the MFG identifier.
	/// </summary>
	/// <returns>A read-only pointer to the MFG identifier.</returns>
	const char *GetMfgID() const;

	/// <summary>
	/// Gets the additional identifier.
	/// </summary>
	/// <returns>A read-only pointer to the additional identifier.</returns>
	const char *GetAdditionalID() const;

	/// <summary>
	/// Gets the theoretical payback with an implied decimal point.
	/// </summary>
	/// <returns>The theoretical payback with an implied decimal point.</returns>
	WORD GetTheoreticalPayback() const;

	void InitialWriteToPermanentStore();
	void RemoveFromPermanentStore();
	DWORD GetOffSet(void)
	{
		return m_dwOffset;
	}

	CMeters *GetMeters(void)
	{
		return &m_Meters;
	}

	virtual void BuildYourself();

	/// <summary>
	/// Gets the credits won. Does not include attendant-paid bonus win.
	/// </summary>
	/// <returns>The credits won. Does not include attendant-paid bonus win.</returns>
	__int64 GetCreditsWon() const;

	/// <summary>
	/// Gets the maximum bet.
	/// </summary>
	/// <returns>Max Bet (in credits).</returns>
	__int64 GetMaxBet() const;

	/// <summary>
	/// Sets the maximum bet.
	/// </summary>
	/// <param name="maxBet">The maximum bet (in credits).</param>
	void SetMaxBet(__int64 maxBet);

	/// <summary>
	/// Gets the progressive group.
	/// </summary>
	/// <returns>The progressive group.</returns>
	byte GetProgressiveGroup() const;

	/// <summary>
	/// Sets the progressive group.
	/// </summary>
	/// <param name="progressiveGroup">IN - The progressive group.</param>
	void SetProgressiveGroup(byte progressiveGroup);

	/// <summary>
	/// Gets whether this the provided door type is open.
	/// </summary>
	/// <returns>true if the door is open, false otherwise.</returns>
	bool IsDoorOpen(DoorType door) const;

	/// <summary>
	/// Gets the Game Name Length.
	/// </summary>
	/// <returns>The Game Name Length.</returns>
	byte GetGamenameLength() const;

	/// <summary>
	/// Sets the Game Name Length.
	/// </summary>
	/// <param name="gamenameLength">IN - The Game Name Length.</param>
	void SetGamenameLength(byte gamenameLength);

	/// <summary>
	/// Gets the Game Name.
	/// </summary>
	/// <returns>The Game Name as byte pointer.</returns>
	const char* GetGameName() const;
	
	/// <summary>
	/// Gets the game name will be the MfgID follow by a “#” and game number. 
	/// </summary>
	/// <returns>The Game Name as byte pointer.</returns>
	string GetDefaultGameName() const;

	/// <summary>
	/// Sets the Game Name.
	/// </summary>
	/// <param name=name="gameName">IN - The char pointer containig game name.</param>
	void SetGameName(char* gameName);

	/// <summary>
	/// Sets the Game Name.
	/// </summary>
	/// <param name=name="gameName">IN - The wchar pointer containig game name.</param>
	void SetGameName(const char *gameName);
	/// <summary>
	/// Gets the Additional Info.
	/// </summary>
	/// <returns>The Additional Info as byte pointer.</returns>

	const char* GetAdditionalInfo() const;

	/// <summary>
	/// Sets the Additional Info.
	/// </summary>
	/// <param name=name="gameName">IN - The char pointer containig Additional Info.</param>
	void SetAdditionalInfo(char* additionalInfo);

	/// <summary>
	/// Sets the Additional Info.
	/// </summary>
	/// <param name=name="gameName">IN - The wchar pointer containig Additional Info.</param>
	void SetAdditionalInfo(const char *lpcAdditionalInfo);


	/// <summary>
	/// Sets the information fetch flag.
	/// </summary>
	/// <param name=name="value">IN - true if game info has been fetch via SAS, false otherwise.</param>
	void SetInfoFetchedFlag(bool value);

	/// <summary>
	/// Gets the information fetch flag.
	/// </summary>
	/// <returns>true if game info has been fetch via SAS, false otherwise.</returns>
	bool IsInfoFetched() const;

	/// <summary>
	/// Gets the progressive level IDs as a comma delimited string
	/// </summary>
	/// <returns>Progressive level IDs</returns>
	std::string GetLevelIdsString() const;

	/// <summary>
	/// Gets the progressive level IDs
	/// </summary>
	/// <returns>Progressive level IDs</returns>
	DWORD GetLevelIds() const;

	/// <summary>
	/// Sets the progressive level IDs
	/// </summary>
	/// <param name="levelIds">Progressive level IDs</param>
	void SetLevelIds(DWORD levelIds);

private:
	DWORD		m_dwGameNumber;
	DenomCurrencyType	m_DenomType;
	int			m_nDenomMultiplier;
	int			m_denomCode;
	char		m_lpPaytableID[PAY_TABLE_ID_LEN];	// This seems very SASSY
	char		m_lpMfgID[MFG_ID_LEN];				// This seems very SASSY
	bool		m_bIsEnabled;
	bool		m_bIsCurrentGame;
	bool		m_bIsEGM;
	bool		m_infoIsFetched;//Not persisted
	CMeters	 	m_Meters;
	char		m_lpAdditionalID[ADDITIONAL_ID_LEN];
	WORD		m_wTheoreticalPayback;
	__int64		m_maxBet; // In credits.
	byte m_progressiveGroup;
	byte m_gamenameLength;
	byte m_paytableIDLength;
	char m_gameName[GAME_NAME_LENGTH];
	char m_gameAdditionalInfo[ADDITIONAL_INFO_LENGTH];

	DWORD m_dwGameNumberOffset;
	DWORD m_dwDenomTypeOffset;
	DWORD m_dwDenomMultiplierOffset;
	DWORD m_dwPaytableIDOffset;
	DWORD m_dwMfgIDOffset;
	DWORD m_dwIsEnabledOffset;
	DWORD m_dwIsCurrentGameOffset;
	DWORD m_dwAdditionalIDOffset;
	DWORD m_dwTheoreticalPaybackOffset;
	DWORD m_dwMetersOffset;
	DWORD m_levelIds;

	void SetMemberOffsets(bool preMGMD = false);
};

