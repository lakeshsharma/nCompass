#pragma once

#include "Hardware/NonVolatileMemoryMaps.h"
#include "InternalMsgContent.h"
#include "PollerProtocol/PollerPlayerResponseMessage.h"

const LPCTSTR DEFAULT_PLAYER_NAME = "Guest";
const LPCTSTR DEFAULT_PLAYER_NAME_UPPERCASE = "GUEST";

static const DWORD MAX_PLAYER_NAME_LENGTH = 32;
static const DWORD MAX_PLAYER_RESPONSE_TEXT_LENGTH = 128;
static const DWORD MAX_PLAYER_CARD_NUMBER_LENGTH = 20;

namespace SentinelNativeUnitTests { class PlayerInfoUnitTests; }

class CPlayerInfo : public CInternalMsgContent<INT_MSG_CONTENT_PLAYERINFO>
{
public:
	//friend class declarations for unit test access.
	friend class SentinelNativeUnitTests::PlayerInfoUnitTests;

	static const DWORD COUNTDOWN_PRECISION_MULTIPLIER = 10000;

	static const DWORD INVALID_DAILY_POINTS;
	static const DWORD INVALID_PLAYER_ID;
	static const DWORD INVALID_POINTS_TILL_NEXT_TIER;
	static const DWORD INVALID_RANK_ID;
	static const DWORD DEFAULT_RANK_ID;

	static const DWORD NVRAM_OLD_PLAYER_INFO_SIZE_USED;
	static const DWORD NVRAM_OLD_PLAYER_INFO_SIZE;

	CPlayerInfo(void);
	~CPlayerInfo(void);
	CPlayerInfo(const CPlayerInfo &rhs);
	CPlayerInfo& operator=(const CPlayerInfo &rhs);

	/// <summary>
	/// PlayerResponse constructor. Initializes a new instance of the <see cref="CPlayerInfo"/> class.
	/// </summary>
	/// <param name="playerResponse">IN - The player response.</param>
	CPlayerInfo(const PlayerResponse& playerResponse);

	CPlayerInfo(const byte *nvramStream);
	int GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength );
private:
	void CleanUp();
	void CopyObj(const CPlayerInfo& rhs);
public:
	void SetName(const std::string &name);
	const std::string &GetName(void) const;

	void SetResponseText(const std::string &text);
	const std::string &GetResponseText(void) const;

	void SetCardNumber(const byte *cardNumber, int cardNumberLen);
	const byte *GetCardNumber(int &cardNumberLen) const;

	void SetMultiplierDivider(DWORD multiplier, DWORD divider);
	bool CheckMultiplierDivider( DWORD fallbackMultiplier, DWORD fallbackDivider );
	DWORD GetMultiplier( void ) const;
	DWORD GetDivider( void ) const;

    /// <summary> Sets the starting points and countdown for the 
    /// player's session. 
	/// </summary>
    /// <param name="points">IN - The starting points for the 
    /// player</param> 
    /// <param name="countdown">IN - The starting countdown value 
    /// for the player</param> 
    /// <param name="highPrecisionCountdown">IN - The starting high 
    /// precision countdown value for the player. Default 
    /// value if not available</param> 
	void SetPointsAndCountdown(DWORD points, DWORD countdown, DWORD countdownHighPrecision = CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

	DWORD GetPoints(void) const;
	DWORD GetCountdown(void) const;

	void SetFlags( bool bBirthdayToday, bool bAnniversaryToday, bool bBannedFromPlay );
	bool IsBirthdayToday( void ) const;
	bool IsAnniversaryToday( void ) const;
	bool IsBannedFromPlay() const;

	void SetCompValue(DWORD compVal);
	DWORD GetCompValue(void) const;

	void AllowPBT(bool bAllowPBT = true);
	bool IsPBTAllowed( void ) const;
	
	void SetDailyPoints(DWORD dailyPoints);
	DWORD GetDailyPoints( void ) const;

	void MaxCardLimitReached(bool bMaxCardLimitReached = false);
	bool IsMaxCardLimitReached(void) const;

	void SetPointsTillNextTier(DWORD pointsTillNextTier);
	DWORD GetPointsTillNextTier(void) const;

	void SetTierMultiplier(DWORD tierMultiplier);
	DWORD GetTierMultiplier( void ) const;

	/// <summary>
	/// Sets the rank identifier.
	/// </summary>
	/// <param name="rankId">IN - The rank identifier.</param>
	void SetRankId(DWORD rankId);

	/// <summary>
	/// Gets the player rank identifier.
	/// </summary>
	/// <returns>The rank identifier.</returns>
	DWORD GetRankId() const;

	/// <summary>
	/// Sets the name of the rank.
	/// </summary>
	/// <param name="rankName">IN - The name of the rank.</param>
	void SetRankName(const std::string &rankName);

	/// <summary>
	/// Gets the name of the rank.
	/// </summary>
	/// <returns>A pointer with read access to the name of the rank.</returns>
	/// <remarks>
	/// The CPlayerInfo instance manages the memory pointed to.
	/// </remarks>
	const std::string &GetRankName(void) const;

	/// <summary>
	/// Determines whether the rank is valid.
	/// </summary>
	/// <returns>true if teh rank is valid</returns>
	bool IsRankValid() const;

	/// <summary>
	/// Logs the rank information.
	/// </summary>
	void LogRankInfo() const;

	/// <summary>
	/// Sets the player identifier.
	/// </summary>
	/// <param name="playerId">The player identifier.</param>
	void SetPlayerId(DWORD playerId);

	/// <summary>
	/// Gets the player identifier.
	/// </summary>
	/// <returns>player identifier</returns>
	DWORD GetPlayerId(void) const;

	/// <summary>
	/// Sets the tier points.
	/// </summary>
	/// <param name="tierPoints">The tier points.</param>
	void SetTierPoints(DWORD tierPoints);

	/// <summary>
	/// Gets the tier points.
	/// </summary>
	/// <returns>tier points</returns>
	DWORD GetTierPoints(void) const;

	/// <summary>
	/// Gets the countdown high precision.
	/// </summary>
	/// <returns>countdown high precision</returns>
	DWORD GetCountdownHighPrecision(void) const;

	/// <summary>
	/// Sets the accounting type
	/// </summary>
	void SetAccountingType(AccountingType accountingType);

	/// <summary>
	/// Gets the accounting type
	/// </summary>
	/// <returns>Accounting type</returns>
	AccountingType GetAccountingType() const;

	/// <summary>
	/// Gets the extended buckets available flag.
	/// </summary>
	/// <returns>True if extended buckets available. Otherwise false.</returns>
	bool IsExtendedBucketsAvailable() const;

	/// <summary>
	/// Gets the extended buckets JSON data.
	/// </summary>
	/// <returns>Extended buckets JSON data</returns>
	std::string GetExtendedBucketsJSONData() const;

	/// <summary>
	/// Gets the ID of extended bucket mapped to legacy points bucket.
	/// </summary>
	/// <returns>Points bucket ID</returns>
	DWORD GetEBPointsId() const;

	/// <summary>
	/// Gets the ID of extended bucket mapped to legacy comps bucket.
	/// </summary>
	/// <returns>Comps bucket ID</returns>
	DWORD GetEBCompsId() const;

	/// <summary>
	/// Gets the ID of extended bucket mapped to legacy promos bucket.
	/// </summary>
	/// <returns>Promos bucket ID</returns>
	DWORD GetEBPromosId() const;

	/// <summary>
	/// Gets the ID of extended bucket mapped to legacy tier points bucket.
	/// </summary>
	/// <returns>Tier points bucket ID</returns>
	DWORD GetEBTierPointsId() const;

	/// <summary>
	/// Gets the extended buckets countdown data.
	/// </summary>
	/// <returns>The extended buckets countdown data.</returns>
	const std::vector<ExtendedBucketsCountdownData*> GetExtendedBucketsCountdownData() const;

	/// <summary>
	/// Gets whether player is enrolled in wallet 2.0 or not.
	/// </summary>
	/// <returns>True if player is enrolled in wallet 2.0. Otherwise false.</returns>
	bool IsEnrolledInWallet_2_0() const;

private:
	static const DWORD MAX_RANK_NAME_LENGTH = 51; // Includes trailing NUL character.

	std::string m_name;
	std::string  m_responseText;

	byte *m_cardNumber;
	int m_cardNumberLen;

	DWORD m_multiplier;		// This is how much to decrement the countdown on each coin-in (or coin-out for coin-out accounting)
	DWORD m_divider;		// This is the countdown reset
	DWORD m_lastPoints;		// This is the last total points we got from poller for this player
	DWORD m_lastCountdown;	// This was the last countdown value we got from teh poller. 
	DWORD m_compValue;		// in cents.

	bool m_bBirthdayToday;
	bool m_bAnniversaryToday;
	bool m_bBannedFromPlay;

	DWORD m_dailyPoints;
	bool m_bMaxCardLimitIndicator;
	DWORD m_pointsTillNextTier;

	DWORD m_tierMultiplier;

	bool m_bAllowPBT;

	DWORD m_rankId;
	std::string m_rankName;

	DWORD m_playerId;
	DWORD m_tierPoints;
	DWORD m_countdownHighPrecision;
	AccountingType m_accountingType;

	bool m_bEBAvailable;
	std::string m_ebJSONData;

	DWORD m_ebPointsId;
	DWORD m_ebCompsId;
	DWORD m_ebPromosId;
	DWORD m_ebTierPointsId;
	std::vector<ExtendedBucketsCountdownData*> m_ExtendedBucketsCountdowns;

	bool m_bEnrolledInWallet_2_0;
};
