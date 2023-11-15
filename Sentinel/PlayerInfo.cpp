#include "stdafx.h"
#include "PlayerInfo.h"
#include "SessionPoints.h"

// Sentinel includes.
#include "Logging/LogString.h"
#include "Utilities.h"

using namespace std;

const DWORD CPlayerInfo::INVALID_DAILY_POINTS = (DWORD)-1;
const DWORD CPlayerInfo::INVALID_PLAYER_ID = (DWORD)-1; // AKA 0xFFFFFFFF.
const DWORD CPlayerInfo::INVALID_POINTS_TILL_NEXT_TIER = (DWORD)-1;
const DWORD CPlayerInfo::INVALID_RANK_ID = CPollerPlayerResponseMessage::INVALID_RANK_ID;
const DWORD CPlayerInfo::DEFAULT_RANK_ID = CPollerPlayerResponseMessage::DEFAULT_RANK_ID;

const DWORD CPlayerInfo::NVRAM_OLD_PLAYER_INFO_SIZE_USED = 221;
const DWORD CPlayerInfo::NVRAM_OLD_PLAYER_INFO_SIZE = 256;

/// <summary>
/// Constructor
/// </summary>
CPlayerInfo::CPlayerInfo(void) :
m_name(DEFAULT_PLAYER_NAME),
m_responseText(_T("")),
m_cardNumber(NULL),
m_cardNumberLen(0),
m_multiplier(DEFAULT_MULTIPLIER),
m_divider(DEFAULT_COUNTDOWN),
m_lastPoints(0),
m_lastCountdown(DEFAULT_COUNTDOWN),
m_compValue(0),
m_bBirthdayToday(false),
m_bAnniversaryToday(false),
m_bBannedFromPlay(false),
m_dailyPoints(0),
m_bMaxCardLimitIndicator(false),
m_pointsTillNextTier(0),
m_tierMultiplier(0),
m_bAllowPBT(true),
m_rankId(DEFAULT_RANK_ID),
m_playerId(CPollerPlayerResponseMessage::DEFAULT_PLAYER_ID),
m_tierPoints(CPollerPlayerResponseMessage::DEFAULT_TIER_POINTS),
m_countdownHighPrecision(CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION),
m_accountingType(AccountingType::ACCOUNTING_COINS_IN),
m_bEBAvailable(false),
m_ebPointsId(0),
m_ebCompsId(0),
m_ebPromosId(0),
m_ebTierPointsId(0),
m_bEnrolledInWallet_2_0(false)
{
}

/// <summary>
/// Destructor
/// </summary>
CPlayerInfo::~CPlayerInfo(void)
{
	CleanUp();
}

/// <summary>
/// Copy Constructor
/// </summary>
CPlayerInfo::CPlayerInfo(const CPlayerInfo &rhs)
{
	m_cardNumber = NULL;
	CopyObj(rhs);
}

CPlayerInfo::CPlayerInfo(const PlayerResponse& playerResponse) :
	m_cardNumber(NULL),
	m_bAllowPBT(true)
{
	SetCardNumber(playerResponse.card_id, sizeof(playerResponse.card_id));
	SetMultiplierDivider(playerResponse.multiplier, playerResponse.divider);
	SetFlags(playerResponse.birthdayIndicator, playerResponse.anniversaryIndicator, playerResponse.bannedFromPlayIndicator);
	SetPointsAndCountdown(playerResponse.totalPoints, playerResponse.countdown, playerResponse.countdownHighPrecision);
	SetCompValue(playerResponse.comps);
	string tempStr(playerResponse.name != nullptr ? (char *)playerResponse.name : "");
	SetName(tempStr);
	string tempStr2(playerResponse.response != nullptr ? (char *)playerResponse.response : "");
	SetResponseText(tempStr2);
	SetDailyPoints(playerResponse.dailyPoints); //added for highlander changes
	MaxCardLimitReached(playerResponse.maxCardLimitIndicator != 0 ? true : false); //added for highlander changes
	SetPointsTillNextTier(playerResponse.pointsTillNextTier); //added for highlander changes

	//TR #27073
	SetTierMultiplier(playerResponse.tierMultiplier);
	LogString(GENERAL, "Player Response Tier Multiplier: %u", playerResponse.tierMultiplier);

	SetRankId(playerResponse.m_rankId);
	string tempStr3(playerResponse.m_rankName != nullptr ? (char *)playerResponse.m_rankName : "");
	SetRankName(tempStr3);

	SetPlayerId(playerResponse.m_playerId);
	SetTierPoints(playerResponse.m_tierPoints);

	m_accountingType = playerResponse.accountingType;
	m_bEBAvailable = playerResponse.ebIndicator;
	m_ebJSONData = playerResponse.ebJSONData;
	m_ebPointsId = playerResponse.ebPointsId;
	m_ebCompsId = playerResponse.ebCompsId;
	m_ebPromosId = playerResponse.ebPromosId;
	m_ebTierPointsId = playerResponse.ebTierPointsId;

	for (auto itr : playerResponse.ebCountdowns)
	{
		ExtendedBucketsCountdownData *countdownData = new ExtendedBucketsCountdownData(*itr);
		m_ExtendedBucketsCountdowns.push_back(countdownData);
	}

	m_bEnrolledInWallet_2_0 = playerResponse.enrolledInWallet_2_0;
}

/// <summary>
/// Create a player info from NVRAM.
/// </summary>
CPlayerInfo::CPlayerInfo( const byte *nvramBuffer )
{
	m_dailyPoints = 0;
	m_bMaxCardLimitIndicator = false;
	m_pointsTillNextTier = 0;
	m_bEnrolledInWallet_2_0 = false;

	m_name = (char *)nvramBuffer;
	nvramBuffer += MAX_PLAYER_NAME_LENGTH;

	m_responseText = (char *)nvramBuffer;
	nvramBuffer += MAX_PLAYER_RESPONSE_TEXT_LENGTH;

	memcpy( &m_cardNumberLen, nvramBuffer, sizeof( m_cardNumberLen ) );
	nvramBuffer += sizeof( m_cardNumberLen );

	if ( m_cardNumberLen > 0 )
	{
		m_cardNumber = new byte[m_cardNumberLen];
		memcpy( m_cardNumber, nvramBuffer, m_cardNumberLen );
	}
	else
	{
		m_cardNumberLen = 0;
		m_cardNumber = NULL;
	}
	nvramBuffer += MAX_PLAYER_CARD_NUMBER_LENGTH;

	memcpy( &m_multiplier, nvramBuffer, sizeof( m_multiplier ) );
	nvramBuffer += sizeof( m_multiplier );

	memcpy( &m_divider, nvramBuffer, sizeof( m_divider ) );
	nvramBuffer += sizeof( m_divider );

	memcpy( &m_lastPoints, nvramBuffer, sizeof( m_lastPoints ) );
	nvramBuffer += sizeof( m_lastPoints );

	memcpy( &m_lastCountdown, nvramBuffer, sizeof( m_lastCountdown ) );
	nvramBuffer += sizeof( m_lastCountdown );

	memcpy( &m_compValue, nvramBuffer, sizeof( m_compValue ) );
	nvramBuffer += sizeof( m_compValue );

	memcpy( &m_bBirthdayToday, nvramBuffer, sizeof( m_bBirthdayToday ) );
	nvramBuffer += sizeof( m_bBirthdayToday );

	memcpy( &m_bAnniversaryToday, nvramBuffer, sizeof( m_bAnniversaryToday ) );
	nvramBuffer += sizeof( m_bAnniversaryToday );

	memcpy( &m_bAllowPBT, nvramBuffer, sizeof( m_bAllowPBT ) );
	nvramBuffer += sizeof( m_bAllowPBT );

	memcpy( &m_dailyPoints, nvramBuffer, sizeof( m_dailyPoints ) );
	nvramBuffer += sizeof( m_dailyPoints );

	memcpy( &m_bMaxCardLimitIndicator, nvramBuffer, sizeof( m_bMaxCardLimitIndicator ) );
	nvramBuffer += sizeof( m_bMaxCardLimitIndicator );

	memcpy( &m_pointsTillNextTier, nvramBuffer, sizeof( m_pointsTillNextTier ) );
	nvramBuffer += sizeof( m_pointsTillNextTier );

	memcpy( &m_tierMultiplier, nvramBuffer, sizeof( m_tierMultiplier ) );
	nvramBuffer += sizeof( m_tierMultiplier );

	memcpy( &m_bBannedFromPlay, nvramBuffer, sizeof( m_bBannedFromPlay ) );
	nvramBuffer += sizeof( m_bBannedFromPlay );

	memcpy(&m_rankId, nvramBuffer, sizeof(m_rankId));
	nvramBuffer += sizeof(m_rankId);

	m_rankName = (char *)nvramBuffer;
	nvramBuffer += MAX_RANK_NAME_LENGTH;

	memcpy(&m_playerId, nvramBuffer, sizeof(m_playerId));
	nvramBuffer += sizeof(m_playerId);

	memcpy(&m_tierPoints, nvramBuffer, sizeof(m_tierPoints));
	nvramBuffer += sizeof(m_tierPoints);

	memcpy(&m_countdownHighPrecision, nvramBuffer, sizeof(m_countdownHighPrecision));
	nvramBuffer += sizeof(m_countdownHighPrecision);

	memcpy(&m_accountingType, nvramBuffer, sizeof(m_accountingType));
	nvramBuffer += sizeof(m_accountingType);
}

/// <summary>
/// Assignment operator
/// </summary>
/// <param name="rhs">Constant reference to CPlayerInfo object that is to be copied.</param>
CPlayerInfo& CPlayerInfo::operator=(const CPlayerInfo &rhs)
{
	if( this != &rhs)
	{
		CleanUp();
		CopyObj(rhs);
	}
	return *this;
}

/// <summary>
/// Deletes all the allocated memory.
/// </summary>
void CPlayerInfo::CleanUp()
{
	if(m_cardNumber != NULL)
	{
		delete [] m_cardNumber;
		m_cardNumber = NULL;
	}

	for (auto itr : m_ExtendedBucketsCountdowns)
	{
		delete itr;
	}
	m_ExtendedBucketsCountdowns.clear();
}

/// <summary>
/// Copies an object of type CPlayerInfo.
/// </summary>
/// <param name="rhs">Constant reference to CPlayerInfo object that is to be copied.</param>
void CPlayerInfo::CopyObj(const CPlayerInfo& rhs)
{
	int cardNumLen;
	const byte *cardNum = rhs.GetCardNumber( cardNumLen );
	SetCardNumber(cardNum, cardNumLen);

	SetName(rhs.GetName());
	SetResponseText(rhs.GetResponseText());
	SetMultiplierDivider( rhs.GetMultiplier(), rhs.GetDivider() );
    SetPointsAndCountdown(rhs.GetPoints(), rhs.GetCountdown(), rhs.GetCountdownHighPrecision());
	SetCompValue( rhs.GetCompValue() );
	SetFlags( rhs.IsBirthdayToday(), rhs.IsAnniversaryToday(), rhs.IsBannedFromPlay() );
	AllowPBT( rhs.IsPBTAllowed() );
	SetDailyPoints( rhs.GetDailyPoints() );
	MaxCardLimitReached( rhs.IsMaxCardLimitReached());
	SetPointsTillNextTier( rhs.GetPointsTillNextTier());
	SetTierMultiplier( rhs.GetTierMultiplier() );
	SetRankId(rhs.m_rankId);
	SetRankName(rhs.m_rankName);
	SetPlayerId(rhs.m_playerId);
	SetTierPoints(rhs.m_tierPoints);
	m_accountingType = rhs.m_accountingType;
	m_bEBAvailable = rhs.m_bEBAvailable;
	m_ebJSONData = rhs.m_ebJSONData;
	m_ebPointsId = rhs.m_ebPointsId;
	m_ebCompsId = rhs.m_ebCompsId;
	m_ebPromosId = rhs.m_ebPromosId;
	m_ebTierPointsId = rhs.m_ebTierPointsId;

	for (auto itr : rhs.m_ExtendedBucketsCountdowns)
	{
		ExtendedBucketsCountdownData *countdownData = new ExtendedBucketsCountdownData(*itr);
		m_ExtendedBucketsCountdowns.push_back(countdownData);
	}

	m_bEnrolledInWallet_2_0 = rhs.m_bEnrolledInWallet_2_0;
}

/// <summary>Copies the contents of "this" object into a byte buffer.</summary>
/// <param name="nvramBuffer">Address of the buffer to copy "this" object's contents to.</param>
/// <param name="nvramBufferLength">Maximum buffer length to copy from "this" object's contents.</param>
/// <returns>Returns the number of bytes copied.</returns>
int CPlayerInfo::GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength )
{
	byte *bufferPos = nvramBuffer;

	ZeroMemory(nvramBuffer, nvramBufferLength);

	if ( nvramBufferLength >= (int)NVRAM_PLAYER_INFO_SIZE )
	{
		CUtilities::ConvertCharsToChars( m_name, (char *)bufferPos, MAX_PLAYER_NAME_LENGTH );
		bufferPos += MAX_PLAYER_NAME_LENGTH;

		CUtilities::ConvertCharsToChars(m_responseText, (char *)bufferPos, MAX_PLAYER_RESPONSE_TEXT_LENGTH);
		bufferPos += MAX_PLAYER_RESPONSE_TEXT_LENGTH;

		memcpy( bufferPos, &m_cardNumberLen, sizeof( m_cardNumberLen ) );
		bufferPos += sizeof( m_cardNumberLen );

		memcpy( bufferPos, m_cardNumber, m_cardNumberLen );
		bufferPos += MAX_PLAYER_CARD_NUMBER_LENGTH;

		memcpy( bufferPos, &m_multiplier, sizeof( m_multiplier ) );
		bufferPos += sizeof( m_multiplier );

		memcpy( bufferPos, &m_divider, sizeof( m_divider ) );
		bufferPos += sizeof( m_divider );

		memcpy( bufferPos, &m_lastPoints, sizeof( m_lastPoints ) );
		bufferPos += sizeof( m_lastPoints );

		memcpy( bufferPos, &m_lastCountdown, sizeof( m_lastCountdown ) );
		bufferPos += sizeof( m_lastCountdown );

		memcpy( bufferPos, &m_compValue, sizeof( m_compValue ) );
		bufferPos += sizeof( m_compValue );

		memcpy( bufferPos, &m_bBirthdayToday, sizeof( m_bBirthdayToday ) );
		bufferPos += sizeof( m_bBirthdayToday );

		memcpy( bufferPos, &m_bAnniversaryToday, sizeof( m_bAnniversaryToday ) );
		bufferPos += sizeof( m_bAnniversaryToday );

		memcpy( bufferPos, &m_bAllowPBT, sizeof( m_bAllowPBT ) );
		bufferPos += sizeof( m_bAllowPBT );

		memcpy( bufferPos, &m_dailyPoints, sizeof( m_dailyPoints ) );
		bufferPos += sizeof( m_dailyPoints );

		memcpy( bufferPos, &m_bMaxCardLimitIndicator, sizeof( m_bMaxCardLimitIndicator ) );
		bufferPos += sizeof( m_bMaxCardLimitIndicator );

		memcpy( bufferPos, &m_pointsTillNextTier, sizeof( m_pointsTillNextTier ) );
		bufferPos += sizeof( m_pointsTillNextTier );

		memcpy( bufferPos, &m_tierMultiplier, sizeof( m_tierMultiplier ) );
		bufferPos += sizeof( m_tierMultiplier );

		memcpy( bufferPos, &m_bBannedFromPlay, sizeof( m_bBannedFromPlay ) );
		bufferPos += sizeof( m_bBannedFromPlay );

		memcpy(bufferPos, &m_rankId, sizeof(m_rankId));
		bufferPos += sizeof(m_rankId);

		CUtilities::ConvertCharsToChars(m_rankName, (char *)bufferPos, MAX_RANK_NAME_LENGTH);
		bufferPos += MAX_RANK_NAME_LENGTH;

		memcpy(bufferPos, &m_playerId, sizeof(m_playerId));
		bufferPos += sizeof(m_playerId);

		memcpy(bufferPos, &m_tierPoints, sizeof(m_tierPoints));
		bufferPos += sizeof(m_tierPoints);

		memcpy(bufferPos, &m_countdownHighPrecision, sizeof(m_countdownHighPrecision));
		bufferPos += sizeof(m_countdownHighPrecision);

		memcpy(bufferPos, &m_accountingType, sizeof(m_accountingType));
		bufferPos += sizeof(m_accountingType);
	}

	return (bufferPos - nvramBuffer);
}

/// <summary>
/// Sets the name of the player.
/// </summary>
/// <param name="name">Name of the player</param>
void CPlayerInfo::SetName( const std::string &name )
{
	m_name = name;
	m_name = m_name.substr( 0, MAX_PLAYER_NAME_LENGTH - 1 );
}

/// <summary>
/// Returns the name of the player.
/// </summary>
const std::string &CPlayerInfo::GetName( void ) const
{
	return m_name;
}

/// <summary>
/// Sets the response text
/// </summary>
void CPlayerInfo::SetResponseText( const std::string &text )
{
	m_responseText = text;
	m_responseText = m_responseText.substr( 0, MAX_PLAYER_RESPONSE_TEXT_LENGTH - 1 );
}

/// <summary>
/// Returns the response text
/// </summary>
const std::string &CPlayerInfo::GetResponseText( void ) const
{
	return m_responseText;
}

/// <summary>Sets the cardNumber and cardNumberLen value.</summary>
/// <param name="cardNumber">pointer to the card number</param>
/// <param name="cardNumberLen">the length of the card number</param>
void CPlayerInfo::SetCardNumber( const byte *cardNumber, int cardNumberLen )
{
	if ( m_cardNumber != NULL )
		delete [] m_cardNumber;

	if ( cardNumberLen > 0 && cardNumber != NULL )
	{
		m_cardNumber = new byte[cardNumberLen];
		memcpy( m_cardNumber, cardNumber, cardNumberLen );
		m_cardNumberLen = cardNumberLen;
	}
	else
	{
		m_cardNumber = NULL;
		m_cardNumberLen = 0;
	}

}

/// <summary>
/// Returns a read-only pointer to the card number.
/// </summary>
/// <param name="cardNumberLen">The length of the card number.</param>
const byte * CPlayerInfo::GetCardNumber( int &cardNumberLen ) const
{
	cardNumberLen = m_cardNumberLen;

	return m_cardNumber;
}

/// <summary>Sets the multiplier and divider value.</summary>
/// <param name="multiplier">multiplier value</param>
/// <param name="divider">divider value</param>
void CPlayerInfo::SetMultiplierDivider(DWORD multiplier, DWORD divider)
{
	m_multiplier = multiplier;
	m_divider = divider;
}

/// <summary>
/// Checks the multiplier and divider.
/// </summary>
/// <param name="fallbackMultiplier">Multiplier will be set to this value if current Multiplier is 0.</param>
/// <param name="fallbackDivider">Divider will be set to this value if current Divider is 0.</param>
/// <returns>Returns true if multiplier or divider is changed.</returns>
bool CPlayerInfo::CheckMultiplierDivider( DWORD fallbackMultiplier, DWORD fallbackDivider )
{
	bool bChanged = false;

	// If the multiplier is not valid, set to the fallback multiplier
	if (m_multiplier == 0)
	{
		m_multiplier = fallbackMultiplier;
		bChanged = true;
	}

	// If the divider is not valid, set to the fallback divider
	if ( m_divider == 0 )
	{
		m_divider = fallbackDivider;
		bChanged = true;
	}

	// If the divider is still zero, set the multipler to zero so points will not be awared.
	if ( m_divider == 0 )
	{
		m_multiplier = 0;
		m_divider = 1;
		bChanged = true;
	}

	return bChanged;
}

/// <summary>Returns the multiplier value.</summary>
DWORD CPlayerInfo::GetMultiplier( void ) const
{
	return m_multiplier;
}

/// <summary>Returns the divider value.</summary>
DWORD CPlayerInfo::GetDivider( void ) const
{
	return m_divider;
}

/// <summary>Sets the points and countdown value.</summary>
/// <param name="points">points value</param>
/// <param name="countdown">countdown value</param>
void CPlayerInfo::SetPointsAndCountdown( DWORD points, DWORD countdown, DWORD countdownHighPrecision )
{
	m_lastPoints = points;
	m_lastCountdown = countdown;
   if (countdownHighPrecision == CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION)
   {
      m_countdownHighPrecision = countdown * CPlayerInfo::COUNTDOWN_PRECISION_MULTIPLIER;
   }
   else
   {
       m_countdownHighPrecision = countdownHighPrecision;
   }
}

/// <summary>Returns the last points value.</summary>
DWORD CPlayerInfo::GetPoints( void ) const
{
	return m_lastPoints;
}

/// <summary>Returns the last countdown value.</summary>
DWORD CPlayerInfo::GetCountdown( void ) const
{
	return m_countdownHighPrecision == CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION ?
	m_lastCountdown : (DWORD)ceil(double(m_countdownHighPrecision) / CPlayerInfo::COUNTDOWN_PRECISION_MULTIPLIER);
}

DWORD CPlayerInfo::GetCountdownHighPrecision(void) const
{
	return m_countdownHighPrecision == CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION ? 
		m_lastCountdown * CPlayerInfo::COUNTDOWN_PRECISION_MULTIPLIER : m_countdownHighPrecision;
}

/// <summary>
/// Sets player information indicators.
/// </summary>
/// <param name="bBirthdayToday">Indicates if player's birhtday is today</param>
/// <param name="bAnniversaryToday">Indicates if player's anniverssary is today</param>
/// <param name="bBannedFromPlay">Indicates if player is banned from playing</param>
void CPlayerInfo::SetFlags(bool bBirthdayToday, bool bAnniversaryToday, bool bBannedFromPlay)
{
	m_bBirthdayToday = bBirthdayToday;
	m_bAnniversaryToday = bAnniversaryToday;
	m_bBannedFromPlay = bBannedFromPlay;
}

/// <summary>Returns true if player's birthday is today.</summary>
bool CPlayerInfo::IsBirthdayToday( void ) const
{
	return m_bBirthdayToday;
}

/// <summary>Returns true if player's anniversary is today.</summary>
bool CPlayerInfo::IsAnniversaryToday( void ) const
{
	return m_bAnniversaryToday;
}

/// <summary>
/// Returns true if player is banned from playing.
/// Returns false otherwise.
/// </summary>
bool CPlayerInfo::IsBannedFromPlay() const
{
	return m_bBannedFromPlay;
}

/// <summary>Sets the comp value.</summary>
/// <param name="compVal">comp value</param>
void CPlayerInfo::SetCompValue(DWORD compVal)
{
	m_compValue = compVal;
}

/// <summary>
/// Returns the comp value.
/// </summary>
DWORD CPlayerInfo::GetCompValue(void) const
{
	return m_compValue;
}

/// <summary>Sets to allow or disallow PBT</summary>
/// <param name="bAllowPBT">Flag that sets to allow/disallow PBT</param>
void CPlayerInfo::AllowPBT(bool bAllowPBT)
{
	m_bAllowPBT = bAllowPBT;
}

/// <summary>Returns true if PBT is allowed, otherwise returns false</summary>
bool CPlayerInfo::IsPBTAllowed(void) const
{
	return m_bAllowPBT;
}

/// <summary>
/// Sets the daily points.
/// </summary>
/// <param name="dailyPoints">daily points</param>
void CPlayerInfo::SetDailyPoints(DWORD dailyPoints)
{
	m_dailyPoints = dailyPoints;
}

/// <summary>
/// Returns daily points value
/// </summary>
DWORD CPlayerInfo::GetDailyPoints(void) const
{
	return m_dailyPoints;
}

/// <summary>
/// Sets the MaxCardLimitIndicator flag.
/// </summary>
void CPlayerInfo::MaxCardLimitReached(bool bMaxCardLimitReached)
{
	m_bMaxCardLimitIndicator = bMaxCardLimitReached;
}

/// <summary>
/// Returns true if the max card limit for the player was reached, otherwise returns false.
/// </summary>
/// <returns>true if the max card limit for the player was reached, otherwise returns false.</returns>
bool CPlayerInfo::IsMaxCardLimitReached( void ) const
{
	return m_bMaxCardLimitIndicator;
}

/// <summary>
/// Sets the points till next tier.
/// </summary>
/// <param name="pointsTillNextTier">tier multiplier</param>
void CPlayerInfo::SetPointsTillNextTier(DWORD pointsTillNextTier)
{
	m_pointsTillNextTier = pointsTillNextTier;
}

/// <summary>
/// Returs points till next tier.
/// </summary>
DWORD CPlayerInfo::GetPointsTillNextTier(void) const
{
	return m_pointsTillNextTier;
}

/// <summary>
/// Sets the tier multiplier.
/// </summary>
/// <param name="tierMultiplier">tier multiplier</param>
void CPlayerInfo::SetTierMultiplier(DWORD tierMultiplier)
{
	m_tierMultiplier = tierMultiplier;
}

/// <summary>
/// Returns tier multiplier.
/// </summary>
DWORD CPlayerInfo::GetTierMultiplier(void) const
{
	return m_tierMultiplier;
}

void CPlayerInfo::SetRankId(DWORD rankId)
{
	m_rankId = rankId;
}

DWORD CPlayerInfo::GetRankId() const
{
	return m_rankId;
}

void CPlayerInfo::SetRankName(const std::string &rankName)
{
	m_rankName = rankName;
	m_rankName = m_rankName.substr(0, MAX_RANK_NAME_LENGTH - 1);
}

const std::string &CPlayerInfo::GetRankName(void) const
{
	return m_rankName;
}

bool CPlayerInfo::IsRankValid() const
{
	return (m_rankId != CPlayerInfo::INVALID_RANK_ID && !m_rankName.empty());
}

void CPlayerInfo::LogRankInfo() const
{
	LogString(GENERAL, _T("PlayerInfo.m_rankId=%u"), (unsigned)m_rankId);
	LogString(GENERAL, _T("PlayerInfo.m_rankname=%s"), static_cast<LPCTSTR>(m_rankName.c_str()));
}

void CPlayerInfo::SetPlayerId(DWORD playerId)
{
	m_playerId = playerId;
}

DWORD CPlayerInfo::GetPlayerId(void) const
{
	return m_playerId;
}

void CPlayerInfo::SetTierPoints(DWORD tierPoints)
{
	m_tierPoints = tierPoints;
}

DWORD CPlayerInfo::GetTierPoints(void) const
{
	return m_tierPoints;
}

void CPlayerInfo::SetAccountingType(AccountingType accountingType)
{
	m_accountingType = accountingType;
}

AccountingType CPlayerInfo::GetAccountingType() const
{
	return m_accountingType;
}

bool CPlayerInfo::IsExtendedBucketsAvailable() const
{
	return m_bEBAvailable;
}

std::string CPlayerInfo::GetExtendedBucketsJSONData() const
{
	return m_ebJSONData;
}

DWORD CPlayerInfo::GetEBPointsId() const
{
	return m_ebPointsId;
}

DWORD CPlayerInfo::GetEBCompsId() const
{
	return m_ebCompsId;
}

DWORD CPlayerInfo::GetEBPromosId() const
{
	return m_ebPromosId;
}

DWORD CPlayerInfo::GetEBTierPointsId() const
{
	return m_ebTierPointsId;
}

const std::vector<ExtendedBucketsCountdownData*> CPlayerInfo::GetExtendedBucketsCountdownData() const
{
	return m_ExtendedBucketsCountdowns;
}

bool CPlayerInfo::IsEnrolledInWallet_2_0() const
{
	return m_bEnrolledInWallet_2_0;
}
