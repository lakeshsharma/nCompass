#pragma once
#include "GlobalDefs.h"
#include "ServiceWindowDefs.h"
#include "Config/Config.h"
#include "InternalMsgContent.h"
#include "UI/ScreenDefs.h"

typedef enum
{
	AccessDenied,
	InvalidPin,
	AccountInUse,
	AccountLocked,
	NewPinRequired,
	AccessError,
	AcctHasNoPin,
	CreditTransferOtherIssue = 23,
	CreditTransferCommserror = 42,
	CreditTransferInsufficientCredits = 43,
	BannedPinEntered = 0xfd,//This error is set by the nCompass and not by the Poller.
	InvalidID = 0xfe,//This error is set by the nCompass and not by the Poller.
	NoError = 0xff,
} PBError;

// Update error string map, when PBError is updated
extern std::map<PBError,std::string> PBErrorString;

union u_PBFlags
{
	// first representation (member of union) 
	struct tPBFlags
	{
		DWORD PropertiesOffline				: 1;//Bit1
		DWORD IsBannedFromRedemptions		: 1;//Bit2
		DWORD CashInactive					: 1;//Bit3
		DWORD PromoInactive					: 1;//Bit4
		DWORD UnableToMoveLeftoverCash		: 1;//Bit5
		DWORD m_spare						: 27;//Bits 6-32
	} flags;

	// second representation (member of union) 
	DWORD uValue;

	u_PBFlags()
	{
		uValue = 0;
	}
};

// RCCL: external credit transfer type
typedef enum
{
	TransferToAccount,
	DownloadCashToGame,
	DownloadPromoToGame,
	Invalid_Transfer
}
ExtCreditTransferType;

// Declare friend class for unit test access.
class PersonalBankerInfoUnitTests;

class CPersonalBankerInfo: public CMemoryStorage, public CInternalMsgContent<INT_MSG_CONTENT_PERSONALBANKERINFO>
{
	// Grant "friend" access for testing.
	friend class PersonalBankerInfoUnitTests;

public:
// #pragma region constructors
	/// <summary>
	/// Standard memory type constructor.
	/// </summary>
	/// <param name="memoryType">IN - Specifies the persisted memory type for the instance.</param>
	/// <param name="bBuildFromNVRAM">IN - If memoryType == MEMORY_NVRAM then true indicates build from NVRAM and false indicates store to NVRAM.
	/// No effect if memoryType != MEMORY_NVRAM.</param>
	/// <remarks>
	/// This constructor does not check if the NVRAM location is active.
	/// </remarks>
	CPersonalBankerInfo(MEMORY_TYPE memoryType = MEMORY_NONE, bool bBuildFromNVRAM = false);

	/// <summary>
	/// Copy constructor.
	/// </summary>
	/// <param name="memoryType">IN/OUT - instance to copy from.</param>
	CPersonalBankerInfo(const CPersonalBankerInfo &rhs);

// #pragma region destructor
	/// <summary>
	/// Destructor.
	/// </summary>
	~CPersonalBankerInfo(void);
// #pragma endregion destructor

// #pragma region operators
	/// <summary>
	/// Assignment operator.
	/// </summary>
	/// <param name="rhs">IN/OUT - instance to assign from.</param>
	/// <returns>A reference to the left hand side (lhs).</returns>
	/// <remarks>
	/// CMemoryStorage values are not altered by this operator.
	/// </remarks>
	CPersonalBankerInfo &operator=( const CPersonalBankerInfo &rhs );
// #pragma endregion operators

	static bool DoesPersonalBankerInfoExistInNVRAM( void );

	/// <summary>
	/// Builds the instance if m_type == MEMORY_NVRAM. Otherwise, nothing is done.
	/// </summary>
	/// <remarks>
	/// This method does not check if the NVRAM location is active.
	/// </remarks>
	virtual void BuildYourself();

	/// <summary>
	/// Builds the instance if m_type == MEMORY_NVRAM. Otherwise, nothing is done.
	/// </summary>
	/// <param name="config">IN - nCompass system configuration.</param>
	/// <param name="bEGMIsInitialized">IN - true indicates the EGM is initialized, false indicates otherwise.</param>
	/// <param name="bIsEFT">IN - true indicates SAS EFT is supported instead of SAS AFT, false indicates otherwise.</param>
	/// <remarks>
	/// added isEFT for TR #19313
	/// </remarks>
	void SetConfigData( const CConfig &config, bool bEGMIsInitialized, bool bIsEFT );

	/// <summary>
	/// Set the card number.
	/// </summary>
	/// <param name="cardNumber">IN - pointer to card number data. Card number data MUST be at least CARD_ID_LEN bytes long.</param>
	void SetCardNumber(const byte* cardNumber)
	{
		memcpy(m_cardNumber, cardNumber, CARD_ID_LEN);
	}

	const byte * GetCardNumber() const
	{
		return m_cardNumber;
	}

	int CompareCardNumber( const byte *cardNumber ) const
	{
		return memcmp( cardNumber, m_cardNumber, CARD_ID_LEN );
	}

	void SetErrorCode(PBError code)
	{
		m_errorCode = code;
	} 

	PBError GetErrorCode() const
	{
		return m_errorCode;
	}

	bool HasError( void ) const
	{
		return (m_errorCode != NoError);
	}

	void SetCashBalance(DWORD cash)
	{
		m_cashBalance = cash;
	}

	DWORD GetCashBalance() const
	{
		return m_cashBalance;
	}

	/// <summary>
	/// Sets the PersonalBanker flags.
	/// </summary>
	void SetPBFlags(DWORD pbFlags);

	/// <summary>
	/// Returns the PersonalBanker flags.
	/// </summary>
	/// <returns>The PersonalBanker flags.</returns>
	DWORD GetPBFlags() const;

	/// <summary>
	/// Returns true if the properties are offline, false otherwise.
	/// </summary>
	/// <returns>true if the properties are offline, false otherwise.</returns>
	bool ArePropertiesOffline() const;

	/// <summary>
	/// Returns true if player is banned from redemptions, false otherwise.
	/// </summary>
	/// <returns>true if player is banned from redemptions, false otherwise.</returns>
	bool IsBannedFromRedemptions() const;

	/// <summary>
	/// Returns true if player's cash is inactive, false otherwise.
	/// </summary>
	/// <returns>true if player's cash is inactive, false otherwise.</returns>
	bool IsCashInactive() const;

	/// <summary>
	/// Returns true if leftover cash movement from other casino failed, false otherwise.
	/// </summary>
	/// <returns>true if leftover cash movement from other casino failed, false otherwise.</returns>
	bool IsUnableToMoveLeftoverCash() const;

	/// <summary>
	/// Sets the flag that determines whether credit tranfer is available for the player
	/// </summary>
	/// <param name="enabled">external credit provider enabled flag</param>
	void SetExternalCreditProviderEnabled(bool enabled);

	/// <summary>
	/// Returns the flag that determines whether credit tranfer is available for the player.
	/// </summary>
	/// <returns>true if external credit provider is enabled, false otherwise</returns>
	bool ExternalCreditProviderEnabled() const;

	/// <summary>
	/// Sets the credit transfer fee value.
	/// This can be either a fixed amount in pennies or
	/// a percentage divided by 100 so that 25 percent is 2500.
	/// The m_creditFeeFixed member determines if this value a fixed amount of a percentage.
	/// </summary>
	/// <param name="value">Credit fee value</param>
	void SetCreditFee(DWORD value);

	/// <summary>
	/// Returns the credit transfer fee value.
	/// This can be either a fixed amount in pennies or
	/// a percentage divided by 100 so that 25 percent is 2500.
	/// The m_creditFeeFixed member determines if this value a fixed amount of a percentage.
	/// </summary>
	/// <returns>Credit fee value.</returns>
	DWORD CreditFee() const;

	/// <summary>
	/// Sets the flag that determines whether the m_creditFee member value is interpreted
	/// as a fixed amount or a percentage.
	/// </summary>
	/// <param name="fixed">Credit fee fixed flag.</param>
	void SetCreditFeeFixed(bool fixed);

	/// <summary>
	/// Gets the flag that determines whether the m_creditFee member value is interpreted
	/// as a fixed amount or a percentage.
	/// </summary>
	/// <returns>true if the credit fee is fixed, false otherwise</returns>
	bool CreditFeeFixed() const;

	void SetPromo2Balance(DWORD promo2)
	{
		m_promo2Balance = promo2;
	}

	DWORD GetPromo2Balance() const
	{
		return m_promo2Balance;
	}

	void SetPointsAvailable(DWORD ptsAvailable)
	{
		// Floorlogix treats ptsAvailable as a signed int with a negative balance possible
		// When balance is negative adjust it to 0.
		if ((int)ptsAvailable < 0)
		{
			ptsAvailable = 0;
		}

		m_pointsAvailable = ptsAvailable;
	}

	DWORD GetPointsAvailable() const
	{
		return m_pointsAvailable;
	}
	
	DWORD GetPromoBalanceOnGame() const
	{
		return m_promoBalanceOnGame;
	}

	void SetPromoBalanceOnGame(DWORD promoOnGame)
	{
		m_promoBalanceOnGame = promoOnGame;
	}

	void SetCashBalanceOnGame(DWORD cashOnGame)
	{
		m_cashBalanceOnGame = cashOnGame;
	}

	DWORD GetCashBalanceOnGame() const
	{
		return m_cashBalanceOnGame;
	}

	void SetPointsPerDollar(DWORD PPD)
	{
		m_pointsPerDollar = PPD > 0 ? PPD : 1;
	}

	DWORD GetPointsPerDollar() const
	{
		return m_pointsPerDollar;
	}

	void SetSequenceNumber(DWORD sequence)
	{
		m_sequenceNumber = sequence;
	}

	DWORD GetSequenceNumber() const
	{
		return m_sequenceNumber;
	}

	void SetMinimumPoints(DWORD pts)
	{
		m_minimumPoints = pts;
	}

	DWORD GetMinimumPoints() const
	{
		return m_minimumPoints;
	}

	void SetMaximumPoints(DWORD maxPts)
	{
		m_maximumPoints = maxPts;
	}

	DWORD GetMaximumPoints() const
	{
		return ( m_maximumPoints > m_pointsAvailable ) ? m_pointsAvailable : m_maximumPoints;
	}

	void SetMinimumCashDown(DWORD minCash)
	{
		m_minimumCashDown = minCash;
	}

	DWORD GetMinimumCashDown() const
	{
		return m_minimumCashDown;
	}

	void SetMaximumCashDown(DWORD maxCash)
	{
		m_maximumCashDown = maxCash;
	}

	DWORD GetMaximumCashDown() const
	{
		return (m_maximumCashDown > (m_cashBalance / ONE_DOLLAR_IN_CENTS)) ? (m_cashBalance / ONE_DOLLAR_IN_CENTS) : m_maximumCashDown;
	}


	void SetMinimumCashUp(DWORD minCash)
	{
		m_minimumCashUp = minCash;
	}

	DWORD GetMinimumCashUp() const
	{
		return m_minimumCashUp;
	}

	void SetMaximumCashUp(DWORD maxCash)
	{
		m_maximumCashUp = maxCash;
	}

	DWORD GetMaximumCashUp() const
	{
		return m_maximumCashUp;
	}

	void SetMinimumPromoDown(DWORD minPromo)
	{
		m_minimumPromoDown = minPromo;
	}

	DWORD GetMinimumPromoDown() const
	{
		return m_minimumPromoDown;
	}

	void SetMaximumPromoDown(DWORD maxPromo)
	{
		m_maximumPromoDown = maxPromo;
	}

	DWORD GetMaximumPromoDown() const
	{
		return (m_maximumPromoDown > (m_promo2Balance / ONE_DOLLAR_IN_CENTS)) ? (m_promo2Balance / ONE_DOLLAR_IN_CENTS) : m_maximumPromoDown;
	}

	void SetMinimumPromoUp(DWORD minPromo)
	{
		m_minimumPromoUp = minPromo;
	}

	DWORD GetMinimumPromoUp() const
	{
		return m_minimumPromoUp;
	}

	void SetMaximumPromoUp(DWORD maxPromo)
	{
		m_maximumPromoUp = maxPromo;
	}

	bool IsSessionForceExit() const
	{
		return m_isSessionForceExit;
	}

	void SetSessionForceExit()
	{
		m_isSessionForceExit = true;
	}

	/// <summary>
	/// Gets the maximum allowed promo up transfer amount in dollars.
	/// </summary>
	/// <returns>The maximum allowed promo up transfer amount in dollars.</returns>
	DWORD GetMaximumPromoUp() const;

	/// <summary>
	/// Sets the PersonalBanker transfer ID.
	/// </summary>
	void SetPersonalBankerID(const char* pbID, int length)
	{
		memcpy_s(m_PBTID, MAX_PBID_LENGTH, pbID, length);
		m_PBIDLength = length;
	}

	/// <summary>
	/// Returns a pointer to the PersonalBanker transfer ID.
	/// </summary>
	/// <returns>A pointer to the PersonalBanker transfer ID.</returns>
	const char* GetPersonalBankerID() const
	{
		return m_PBTID;
	}

	int GetPBIDLength() const
	{
		return m_PBIDLength;
	}

	bool IsTransferToGameEnabled( void ) const
	{
		return ( (IsTransferCashToGameEnabled() || IsTransferPointsToGameEnabled() || IsTransferPromoDollarsToGameEnabled()) &&  m_bEGMInitialized );
	}
	bool IsTransFromGameEnabled( void )
	{
		return (IsTransferCashFromGameEnabled() || IsTransferPromoFromGameEnabled()) && m_bEGMInitialized;
	}
	bool IsTransferCashToGameEnabled( void ) const
	{
		return m_bTransferCashToGameEnabled;
	}
	bool IsTransferPromoFromGameEnabled( void ) const
	{
		return m_bTransferPromoFromGameEnabled;
	}
	bool IsTransferPointsToGameEnabled( void ) const
	{
		return m_bTransferPointsToGameEnabled;
	}
	bool IsTransferPromoDollarsToGameEnabled( void ) const
	{
		return m_bTransferPromoDollarsToGameEnabled;
	}
	bool IsTransferCashFromGameEnabled( void ) const
	{
		return m_bTransferCashFromGameEnabled;
	}
	bool IsCEPFromGameEnabled(void) const
	{
		return IsTransferCashFromGameEnabled() && m_cepFromGame;
	}	
	bool IsPartialTransferCashFromGameEnabled(void) const
	{
		return m_bPartialTransferCashFromGameEnabled;
	}

	bool DoesGameDenomMatch( void ) const
	{
		return m_bGameDenomMatches;
	}

	/// <summary>
	/// Sets the show from game and to game button flags.
	/// </summary>
	void SetShowFromGameAndToGameButtonFlags();

	/// <summary>
	/// Gets the show to game button flag.
	/// </summary>
	/// <returns>True if transfer to game button flag is on. Otherwise false.</returns>
	bool GetShowToGameButton() const;

	/// <summary>
	/// Gets the show from game button flag.
	/// </summary>
	/// <returns>True if transfer from game button flag is on. Otherwise false.</returns>
	bool GetShowFromGameButton() const;

	/// <summary>
	/// Sets the show to game button.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetShowToGameButton(bool value);

	/// <summary>
	/// Gets the personal banker main screen.
	/// </summary>
	/// <returns>personal banker main screen</returns>
	screenID GetPersonalBankerMainScreen() const;

	/// <summary>
	/// Gets the credit transfer screen.
	/// </summary>
	/// <returns>the credit transfer screen</returns>
	screenID GetCreditTransferScreen() const;

	/// <summary>
	/// Gets the transfer to game screen.
	/// </summary>
	/// <returns>transfer to game screen</returns>
	screenID GetTransferToGameScreen();

	/// <summary>
	/// Gets the transfer from game screen.
	/// </summary>
	/// <returns></returns>
	screenID GetTransferFromGameScreen();

	/// <summary>
	/// Gets the maximum points dollar value download.
	/// </summary>
	/// <returns>maximum points dollar value to download</returns>
	DWORD GetMaximumPointsDollarValueDownload() const;

	/// <summary>
	/// Gets the minimum points dollar value download.
	/// </summary>
	/// <returns>minimum points dollar value to download</returns>
	/// <remarks>
	/// The return value is rounded up.
	/// </remarks>
	DWORD GetMinimumPointsDollarValueDownload() const;

	/// <summary>
	/// Sets the external credit transfer type for RRCL
	/// </summary>
	/// <param>The type of external transfer </param>
	/// <returns>The external credit transfer type for RRCL</returns>
	void SetExternalCreditTransferType(ExtCreditTransferType val);

	/// <summary>
	/// Gets the external credit transfer type for RCCL
	/// </summary>
	/// <returns>the external credit transfer type for RCCL</returns>
	/// <remarks>
	/// </remarks>
	ExtCreditTransferType ExternalCreditTransferType()const;

	/// <summary>
	/// Determine if we can Display Transfer Cash To Game.
	/// </summary>
	/// <returns>true/false based on conditions met.</returns>
	bool DisplayTransferCashToGame() const;

	/// <summary>
	/// Determine if we can Display Transfer Cash From Game.
	/// </summary>
	/// <returns>true/false based on conditions met.</returns>
	bool DisplayTransferCashFromGame() const;

	/// <summary>
	/// Determine if we can Display Transfer Points To Game.
	/// </summary>
	/// <returns>true/false based on conditions met.</returns>
	bool DisplayTransferPointsToGame() const;

	/// <summary>
	/// Determine if we can Display Transfer Promo To Game.
	/// </summary>
	/// <returns>true/false based on conditions met.</returns>
	bool DisplayTransferPromoToGame() const;

	/// <summary>
	/// Determine if we can Display Transfer Promo From Game.
	/// </summary>
	/// <returns>true/false based on conditions met.</returns>
	bool DisplayTransferPromoFromGame() const;

	/// <summary>
	/// Returns whether to show Points or not.
	/// </summary>
	/// <returns>'true' if to show Points. Otherwise, false.</returns>
	bool GetShowPointsVsAccountBalance() const;

private:
	byte m_cardNumber[CARD_ID_LEN];
	PBError m_errorCode;
	DWORD m_cashBalance; // In cents.
	u_PBFlags m_pbFlags;
	DWORD m_promo2Balance; // In cents.
	DWORD m_pointsAvailable;
	DWORD m_pointsPerDollar;
	DWORD m_sequenceNumber;
	DWORD m_minimumPoints;
	DWORD m_maximumPoints;
	DWORD m_minimumCashDown; // In dollars.
	DWORD m_maximumCashDown; // In dolars.
	DWORD m_minimumCashUp;
	DWORD m_maximumCashUp;
	DWORD m_minimumPromoDown; // In dollars.
	DWORD m_maximumPromoDown; // In dollars.
	DWORD m_minimumPromoUp;
	DWORD m_maximumPromoUp;
	DWORD m_promoBalanceOnGame;
	DWORD m_cashBalanceOnGame;

	bool m_externalCreditProviderEnabled;
	DWORD m_creditFee;
	bool m_creditFeeFixed;
	DWORD m_currentCreditTransferAmount;

	int m_PBIDLength;
	char m_PBTID[MAX_PBID_LENGTH];
	void StoreToNVRAM( void );

	// Config stuff.
	// Do not need to store to NVRAM.
	bool m_bTransferCashToGameEnabled;
	bool m_bTransferPointsToGameEnabled;
	bool m_bTransferPromoDollarsToGameEnabled;
	bool m_bTransferCashFromGameEnabled;
	bool m_cepFromGame;
	bool m_showToGameButton;
	bool m_showFromGameButton;
	bool m_bGameDenomMatches;
	bool m_bEGMInitialized;
	bool m_bTransferPromoFromGameEnabled;
	bool m_bPartialTransferCashFromGameEnabled;
	ExtCreditTransferType m_extCreditTransferType;
	bool m_externalCreditVerificationEnabled;
	bool m_showPointsVsAccountBalance;

	bool m_isSessionForceExit;
};
