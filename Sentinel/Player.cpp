#include "stdafx.h"
#include "Player.h"

// Sentinel includes.
#include "Diagnostics.h"
#include "DirectoryManager.h"
#include "Logging/LogString.h"
#include "UI/ScreenDefs.h"
#include "resource.h"
#include "SessionPoints.h"
#include "RapidJsonHelper.h"
#include "PollerProtocol/PollerMGMDData.h"
#include "InternalMessage.h"

using namespace std;

const string PLAYER_INFO_TOPIC("PlayerInfo");
const string PLAYER_FULL_SESSION_TOPIC("PlayerFullSession");
const string PLAYER_UNFLUSHED_SESSION_TOPIC("PlayerUnflushedSession");
const string PLAYER_FULL_SESSION_EXT_TOPIC("PlayerFullSessionExt");
const string PLAYER_UNFLUSHED_SESSION_EXT_TOPIC("PlayerUnflushedSessionExt");
const string EXTENDED_BUCKETS_TOPIC("ExtendedBuckets");

const string LEGACY_POINTS("Points");
const string LEGACY_PROMOS("Promos");
const string LEGACY_COMPS("Comps");
const string LEGACY_TIER_POINTS("Tier Points");

CPlayer::CPlayer() : CDataSubscriptionProvider(PLAYER_INFO_TOPIC)
{
	Init();
}

/// <summary>
/// Initializes to default values, without any cleanup.
/// </summary>
/// <param name="utcTime">The UTC time.</param>
/// <returns></returns>
void CPlayer::Init()
{
	m_type = MEMORY_NONE;
	m_card = nullptr;
	m_playerInfo = nullptr;
	m_lastPlayTime = 0;
	m_games = nullptr;

	// Session data
	m_bPointsAdded = false;
	m_lossLimitSessionBalance = 0;
	m_hasPlayed = false;
	m_mobilePlayer = false;

	// Virtual session data
	m_bIsVirtual = false;
	m_virtualStartTime = 0;
	m_virtualCard = nullptr;
	m_cardedOutOnEmployeeCardIn = false;

	m_lastUpdateSentTime = 0;
	m_isGoodbyeEnabled = false;
	m_lastSessionStartTime = 0;

	m_bPBEnabled = false;
	m_bPBOnCardIn = false;
	m_bPartialBonusing = false;
	m_pointsPerDollar = DEFAULT_POINTS_PER_DOLLAR;
	m_bDisplayName = false;
	m_inPlayDisplayTypeConfig = IPDT_TOTAL;
	m_cardInPointsDisplayType = CIDT_NONE;
	m_showCardInCompDisplay = false;
	m_lossLimitEnabled = false;
	m_showPointsVsAccountBalance = false;
	m_universeEnabled = false;
	m_displayProfileButton = false;
	m_pointsCountdownControlByUniverse = false;
	m_isMGMDEnabled = false;
	m_lastSessionPoints = 0;
	m_LeftoverCashTransferStatus = LeftoverCashTransferStatus::LEFTOVER_CASH_TRANSFER_NA;

	AddTopic(PLAYER_FULL_SESSION_TOPIC);
	AddTopic(PLAYER_UNFLUSHED_SESSION_TOPIC);
	AddTopic(PLAYER_FULL_SESSION_EXT_TOPIC);
	AddTopic(PLAYER_UNFLUSHED_SESSION_EXT_TOPIC);
	AddTopic(EXTENDED_BUCKETS_TOPIC);
}

CPlayer::CPlayer(const CConfig &config, const CEGMConfig& egmConfig) : 
CDataSubscriptionProvider(PLAYER_INFO_TOPIC)
{
	Init();

	m_type = MEMORY_NVRAM;
	m_dwAllowedSize = NVRAM_PLAYER_SIZE;
	m_dwOffset = NVRAM_PLAYER_OFFSET;
	m_bOffsetIsSet = true;

	ConfigSettings( config, egmConfig );
	BuildYourself();
}

CPlayer::CPlayer(CCard *card, CPlayerInfo *playerInfo, const CConfig &config, const CEGMConfig& egmConfig, 
	time_t utcTime, bool goodbyeEnabled, CGames* games) :
CDataSubscriptionProvider(PLAYER_INFO_TOPIC)
{
	Init();

	m_type = MEMORY_NVRAM;
	m_dwAllowedSize = NVRAM_PLAYER_SIZE;
	m_dwOffset = NVRAM_PLAYER_OFFSET;
	m_bOffsetIsSet = true;

	m_card = card;
	m_playerInfo = playerInfo;

	m_lastPlayTime = utcTime;
	m_lastUpdateSentTime = m_lastPlayTime;
	m_isGoodbyeEnabled = goodbyeEnabled;
	m_games = games;

	if (m_playerInfo != nullptr)
	{
		m_sessionInfo.SetSessionCountdown(m_playerInfo->GetCountdown() );
	}

	ConfigSettings( config, egmConfig );

	StoreCardDataToNVRAM();
	StorePlayerInfoDataToNVRAM();
	StoreLastPlayTimeToNVRAM();
	StoreSessionDataToNVRAM();
	StoreVirtualSessionDataToNVRAM();
	SetBonusExistanceToNVRAM();
	SetPlayerExistanceToNVRAM( true );
}

CPlayer::CPlayer(const CPlayer &playerRHS) : 
CDataSubscriptionProvider(PLAYER_INFO_TOPIC)
{
	Init();
	CopyObj(playerRHS);
}

CPlayer::~CPlayer(void)
{
	if ( m_sessionInfo.GetSessionStartMeters() != nullptr)
	{
		// Remove session meters from the Persistent Store, because CleanUp() does not.
		if (m_type == MEMORY_NVRAM)
		{
			(m_sessionInfo.GetSessionStartMeters())->RemoveFromPermanentStore();
		}
	}
	if (m_sessionInfo.GetSessionStartMetersCurrent() != nullptr)
	{
		// Remove current session meters from the Persistent Store, because CleanUp() does not.
		if (m_type == MEMORY_NVRAM)
		{
			(m_sessionInfo.GetSessionStartMetersCurrent())->RemoveFromPermanentStore();
		}
	}

	CleanUp();

	SetBonusExistanceToNVRAM();	// This will remove from NVRAM.

	SetPlayerExistanceToNVRAM( false );

	m_games = nullptr;
}

CPlayer & CPlayer::operator=( const CPlayer &playerRHS )
{
	if(this != &playerRHS)
	{
		// Do not remove session meters from the Persistent Store.
		CleanUp();
		CopyObj(playerRHS);
	}
	return *this;
}

/// <summary>
/// In general, copies source instance values to this instance.
/// </summary>
/// <param name="playerRHS">IN - The source instance.</param>
/// <remarks>
/// "this" MUST NOT equal "&playerRHS".
/// All pointer members of "this" SHOULD be set to NULL.
/// </remarks>
void CPlayer::CopyObj(const CPlayer &playerRHS)
{
	// Make the recipient object non-persistent memory.
	m_type = MEMORY_NONE;

	// Copy the card object
	if ( playerRHS.m_card != nullptr )
	{
		m_card = new CCard( *playerRHS.m_card );
	}

	// Copy the player info object
	if ( playerRHS.m_playerInfo != nullptr )
	{
		m_playerInfo = new CPlayerInfo( *playerRHS.m_playerInfo );
	}

	m_lastPlayTime = playerRHS.m_lastPlayTime;
	m_isGoodbyeEnabled = playerRHS.m_isGoodbyeEnabled;
	m_lastSessionStartTime = playerRHS.m_lastSessionStartTime;

	// Copy the session data
	m_sessionInfo = playerRHS.m_sessionInfo;
	m_bPointsAdded = playerRHS.m_bPointsAdded;
	m_lossLimitSessionBalance = playerRHS.m_lossLimitSessionBalance;
	m_hasPlayed = playerRHS.m_hasPlayed;
	m_mobilePlayer = playerRHS. m_mobilePlayer;

	// Set the virtual player data
	m_bIsVirtual = playerRHS.m_bIsVirtual;
	m_virtualStartTime = playerRHS.m_virtualStartTime;
	if ( m_bIsVirtual )
	{
		// For virtual sessions, don't copy m_virtualCard, use m_card instead.
		if ( playerRHS.m_card != nullptr )
		{
			m_virtualCard = new CCard( *playerRHS.m_card );
		}
	}
	m_cardedOutOnEmployeeCardIn = playerRHS.m_cardedOutOnEmployeeCardIn;

	// Copy the config data
	m_bPBEnabled = playerRHS.m_bPBEnabled;
	m_bPBOnCardIn = playerRHS.m_bPBOnCardIn;
	m_pointsPerDollar = playerRHS.m_pointsPerDollar;
	m_bPartialBonusing = playerRHS.m_bPartialBonusing;
	m_bDisplayName = playerRHS.m_bDisplayName;
	m_inPlayDisplayTypeConfig = playerRHS.m_inPlayDisplayTypeConfig;
	m_cardInPointsDisplayType = playerRHS.m_cardInPointsDisplayType;
	m_showCardInCompDisplay = playerRHS.m_showCardInCompDisplay;
	m_notificationTextConfig = playerRHS.m_notificationTextConfig;
	m_lossLimitEnabled = playerRHS.m_lossLimitEnabled;
	m_showPointsVsAccountBalance = playerRHS.m_showPointsVsAccountBalance;
	m_universeEnabled = playerRHS.m_universeEnabled;
	m_displayProfileButton = playerRHS.m_displayProfileButton;
	m_pointsCountdownControlByUniverse = playerRHS.m_pointsCountdownControlByUniverse;
	m_games = playerRHS.m_games;
	m_LeftoverCashTransferStatus = playerRHS.m_LeftoverCashTransferStatus;
}

/// <summary>
/// Frees member variable instances.
/// This can be called from the destructor and other methods.
/// For backwards compatibility, does NOT remove the session meters from the Permanent Store.
/// </summary>
void CPlayer::CleanUp()
{
	if(m_card != nullptr)
	{
		delete m_card;
		m_card = nullptr;
	}
	
	if(m_virtualCard != nullptr)
	{
		delete m_virtualCard;
		m_virtualCard = nullptr;
	}
	
	if(m_playerInfo != nullptr)
	{
		delete m_playerInfo;
		m_playerInfo = nullptr;
	}
}

bool CPlayer::DoesPlayerExistInNVRAM( void )
{
	bool bPlayerExists = CMemoryStorage::IntegrityCheck() && IsActiveLocation( NVRAM_PLAYER_CHECKBYTE_OFFSET, NVRAM_PLAYER_CHECKBYTE_VALUE );

	if ( !bPlayerExists )
		SetInactiveLocation( NVRAM_PLAYER_CHECKBYTE_OFFSET, (BYTE)~NVRAM_PLAYER_CHECKBYTE_VALUE );

	return bPlayerExists;
}


void CPlayer::MarkSessionMetersInactiveInNVRAM()
{
	//initialize the meters in permanent store
	int nNumberOfMetersInNVRAM = NVRAM_NUMBER_OF_EGM_METERS;
	BYTE byInactive = INACTIVE_LOCATION;
	DWORD dwMetersOffset = NVRAM_PLAYER_START_METERS_OFFSET + NVRAM_LOCATION_IS_ACTIVE + sizeof(DWORD); //jump over the meters isactive and the count
	for (int meterIndex = 0; meterIndex < nNumberOfMetersInNVRAM; meterIndex++)
	{
		SetInactiveLocation(dwMetersOffset, byInactive);
		dwMetersOffset += NVRAM_METER_SIZE;
	}
}
/// <summary>
/// Build this player object from NVRAM.
/// </summary>
void CPlayer::BuildYourself()
{
	// Make sure we are a persistant memory player object
	if ( m_type == MEMORY_NVRAM )
	{
		byte nvramBuffer[NVRAM_MAX_READ];

		// Build the card object
		if ( !Read( NVRAM_PLAYER_CARD_OFFSET, nvramBuffer, NVRAM_CARD_SIZE ) )
			LogString(ERROR_LOG, "Could not read player card data from NVRAM" );
		m_card = new CCard( nvramBuffer );
		
		// Build the player info object
		{
			byte buffer[NVRAM_PLAYER_INFO_SIZE];
			if ( !Read( NVRAM_PLAYER_INFO_OFFSET, buffer, NVRAM_PLAYER_INFO_SIZE ) )
				LogString(ERROR_LOG, "Could not read player info data from NVRAM" );
			m_playerInfo = new CPlayerInfo(buffer);
			m_playerInfo->LogRankInfo();
		}
		
		// last Play time
		if ( !Read( NVRAM_PLAYER_PLAY_TIME_OFFSET, nvramBuffer, NVRAM_TIME_SIZE ) )
			LogString(ERROR_LOG, "Could not read last play data from NVRAM" );
		m_lastPlayTime = CUtilities::MakeTimeFromNvramBuffer( nvramBuffer, NVRAM_TIME_SIZE );

		// Session data
		if ( !Read( NVRAM_PLAYER_SESSION_OFFSET, nvramBuffer, NVRAM_PLAYER_SESSION_SIZE ) )
			LogString(ERROR_LOG, "Could not read player session data from NVRAM" );
		byte *bufferPos = nvramBuffer;

		// skipping session points related params as we are not maintaining session after reboot.
		bufferPos += sizeof(DWORD) * 3;  // currentCountdown, sessionPoints, sessionPointsFlushed

		bufferPos += PARTIAL_BONUSING_REMAINDER_SIZE;
		memcpy( &m_bPointsAdded, bufferPos, sizeof( m_bPointsAdded ) );
		bufferPos += sizeof( m_bPointsAdded );
		memcpy( &m_lossLimitSessionBalance, bufferPos, sizeof( m_lossLimitSessionBalance ) );
		bufferPos += sizeof( m_lossLimitSessionBalance );
		memcpy( &m_hasPlayed, bufferPos, sizeof(m_hasPlayed) );
		bufferPos += sizeof(m_hasPlayed);
		memcpy( &m_mobilePlayer, bufferPos, sizeof(m_mobilePlayer) );
		bufferPos += sizeof(m_mobilePlayer);

		// Virtual player data
		if ( !Read( NVRAM_PLAYER_VIRTUAL_OFFSET, nvramBuffer, NVRAM_PLAYER_VIRTUAL_SIZE ) )
			LogString(ERROR_LOG, "Could not read player virtual session data from NVRAM" );
		bufferPos = nvramBuffer;
		memcpy( &m_bIsVirtual, bufferPos, sizeof( m_bIsVirtual ) );
		bufferPos += sizeof( m_bIsVirtual );
		if ( m_bIsVirtual )
		{
			m_virtualStartTime = CUtilities::MakeTimeFromNvramBuffer( bufferPos, NVRAM_TIME_SIZE );
			bufferPos += NVRAM_TIME_SIZE;

			m_virtualCard = new CCard( bufferPos );
		}
		else
		{
			m_virtualCard = nullptr;
		}
		
		//Build Session Meters
		m_sessionInfo.BuildSessionStartMeters();

		// Initialize starting divider/multiplier
		SetStartingMultiplierDividerCountdown();

		m_lastUpdateSentTime = m_card->GetCardInTime();
	}
}

// Returns true if the player is a guest.
bool CPlayer::IsGuest( void ) const
{
	bool bIsGuest = true;

	// Assume that if the card number in player info is null, we have a guest player.
	if ( m_playerInfo != nullptr )
	{
		int cardLen;
		string name(m_playerInfo->GetName());
		std::transform(name.begin(), name.end(), name.begin(), ::toupper);

		bIsGuest = (m_playerInfo->GetCardNumber( cardLen ) == NULL) || (name == DEFAULT_PLAYER_NAME_UPPERCASE);
	}

	return bIsGuest;
}

bool CPlayer::IsVirtual( void ) const
{
	return m_bIsVirtual;
}

time_t CPlayer::GetLastSessionStartTime(void) const
{
	return m_lastSessionStartTime;
}

void CPlayer::SetCardedOutOnEmployeeCardIn(bool cardedOutOnEmployeeCardIn)
{
	m_cardedOutOnEmployeeCardIn = cardedOutOnEmployeeCardIn;
}

bool CPlayer::WasCardedOutOnEmployeeCardIn() const
{
	return m_cardedOutOnEmployeeCardIn;
}

bool CPlayer::CompareCard( const CCard *card ) const
{
	bool isSame(false);

	if (m_card != nullptr)
	{
		isSame = m_card->Compare( card );
	}

	return isSame;
}

const CCard *CPlayer::GetVirtualCard( void ) const
{
	return m_virtualCard;
}

const CCard *CPlayer::GetCard( void ) const
{
	return m_card;
}

std::string CPlayer::GetName( void ) const
{
	string retVal;

	if ( m_playerInfo != nullptr )
	{
		retVal = m_playerInfo->GetName();
	}

	return retVal;
}

bool CPlayer::IsBannedFromPlay( void ) const
{
	bool bBannedFromPlay(false);// Assume that if the player info is null, we have a guest player.

	if ( m_playerInfo != nullptr )
	{
		bBannedFromPlay = m_playerInfo->IsBannedFromPlay();
	}

	return bBannedFromPlay;
}

void CPlayer::CardRemoved( time_t const &removalTime, time_t const &utcRemovalTime, bool bMakeVirtual )
{
	if (m_card != nullptr)
	{
		m_card->SetCardOutTime( removalTime );
		StoreCardDataToNVRAM();
		m_lastSessionStartTime = m_card->GetCardInTime();
	}

	// Make the player virtual if told to do so.
	if ( bMakeVirtual )
	{
		MakeVirtual( removalTime, utcRemovalTime );
	}
}

time_t CPlayer::GetVirtualStartTime( void ) const
{
	return m_virtualStartTime;
}

/// <summary>
/// Make this player object in a virtual session.
/// </summary>
/// <param name="virtualStartTime">The local time for the session start.</param>
/// <param name="utcVirtualStartTime">The UTC time for the session start.</param>
void CPlayer::MakeVirtual( time_t const &virtualStartTime, time_t const &utcVirtualStartTime )
{
	m_bIsVirtual = true;

	m_virtualStartTime = virtualStartTime;

	if (m_virtualCard != nullptr)
	{
		delete m_virtualCard;
		m_virtualCard = nullptr;
	}
	
	if ( m_card != nullptr )
	{
		m_virtualCard = new CCard( *m_card );

		m_virtualCard->SetCardInTime( virtualStartTime, utcVirtualStartTime );
	}
	
	StoreVirtualSessionDataToNVRAM();
}

void CPlayer::VirtualSessionEnd( time_t const &virtualEndTime )
{
	m_bIsVirtual = false;

	//set the flag now for the virtual session to end.
	if ( m_virtualCard != nullptr )
	{
		m_virtualCard->SetCardOutTime( virtualEndTime );
	}
	StoreVirtualSessionDataToNVRAM();

	m_lastSessionStartTime = m_virtualStartTime;
}

time_t CPlayer::GetLastPlayTime( void ) const
{
	return m_lastPlayTime;
}

void CPlayer::SetLastPlayTime( const time_t &playTime )
{
	m_lastPlayTime = playTime;

	StoreLastPlayTimeToNVRAM();
}

void CPlayer::ConfigSettings(const CConfig &config, const CEGMConfig& egmConfig)
{

	// Set the PB enabled flag
	m_bPBEnabled = (config.PBTCashEnabled() ||
		config.PBTPointsEnabled() ||
		config.PBTPointsAsPromo() ||
		config.PBTPromoEnabled() ||
		config.PBFromGameEnabled()) && egmConfig.GetEGMProtocol() != IGT_SAS_3xx;

	// Set card in on PB.
	m_bPBOnCardIn = config.CardInPBT();

	m_bPartialBonusing = config.PartialBonusAward();

	// Set the points per doller value
	m_pointsPerDollar = config.GetPointsPerDollar();
	if ( m_pointsPerDollar == 0 )
		m_pointsPerDollar = DEFAULT_POINTS_PER_DOLLAR;

	m_bDisplayName = config.DisplayPlayerName();

	// Set the In Play Display Type
	if ( config.DisplayPlayerPtsTotal() )
	{
		m_inPlayDisplayTypeConfig = IPDT_TOTAL;
	}
	else if ( config.DisplayCountdownAndSession() )
	{
		m_inPlayDisplayTypeConfig = IPDT_COUNTDOWN_SESSION;
	}
	else if ( config.DisplayGoodLuckAndSession() )
	{
		m_inPlayDisplayTypeConfig = IPDT_GOODLUCK_SESSION;
	}
	else if ( config.DisplayTotalAndSessionPtsNum() )
	{
		m_inPlayDisplayTypeConfig = IPDT_TOTAL_SESSION;
	}
	else if ( config.DisplayTotalPtsSessionPtsAsCash() )
	{
		m_inPlayDisplayTypeConfig = IPDT_TOTALCASH_SESSIONCASH;
	}
	else if ( config.DisplayGoodLuckAndSessionPts() )
	{
		m_inPlayDisplayTypeConfig = IPDT_GOODLUCK_SESSIONPTS;
	}
	else if ( config.DisplayPtsTotalAsCashSession() )
	{
		m_inPlayDisplayTypeConfig = IPDT_TOTALCASH;
	}
	else
	{
		m_inPlayDisplayTypeConfig = IPDT_GOOD_CARD;
	}

	// Set the Card In Display Type for points
	if ( config.DisplayPtsTotalAsCash() )
		m_cardInPointsDisplayType = CIDT_TOTALCASH;
	else if ( config.DisplayNumericPointTotal() )
		m_cardInPointsDisplayType = CIDT_TOTAL;
	else
		m_cardInPointsDisplayType = CIDT_NONE;

	// Set the Card In Display Type for comps
	if ( config.DisplayComps() )
		m_showCardInCompDisplay = true;
	else
		m_showCardInCompDisplay = false;

	m_lossLimitEnabled = config.GetLossLimitActive();

	m_showPointsVsAccountBalance = config.GetShowPointsVsAccountBalance();

	m_universeEnabled = config.GetUniverseEnabled();

	m_displayProfileButton = config.DisplayPlayerProfileButton();

	m_isMGMDEnabled = config.GetGlobalMGMDEnabled()	&& CPollerMGMDData::Instance().GetMGMDAvailable();

	m_pointsCountdownControlByUniverse = config.GetPointsCountdownControlByUniverse();
}

const CPlayerInfo *CPlayer::GetPlayerInfo(void) const
{
	return m_playerInfo;
}

CPlayerInfo *CPlayer::GetPlayerInfo(void)
{
	return m_playerInfo;
}

void CPlayer::SetPlayerInfo(CPlayerInfo *playerInfo)
{
	// Set the new player
	m_playerInfo = playerInfo;

	if (m_playerInfo != nullptr)
	{
		// Set the countdown for this player so it reflects where the player left off
		DWORD playerCountdown = m_playerInfo->GetCountdown();

		// Tracker #22667 - The sentinel should NEVER display countdown of 0, even if it was sent 0.
		// fall back to divider.
		if (playerCountdown == 0)
		{
			playerCountdown = m_playerInfo->GetDivider();
		}
		
		// Now set the countdown for this player
		m_sessionInfo.SetSessionCountdown(playerCountdown);
	}

	StorePlayerInfoDataToNVRAM();
	StoreSessionDataToNVRAM();
}

void CPlayer::SetPlayerGlobalBalance( const CPlayerGlobalBalance *playerGlobalBal )
{
	if (m_playerInfo != nullptr)
	{
		// If we already have the player info then adjust the points in it	
		if (playerGlobalBal != nullptr)
		{
			DWORD points =  (DWORD)playerGlobalBal->GetTotalPoints() + m_sessionInfo.GetSessionPoints() + GetBonusPoints();
            m_playerInfo->SetPointsAndCountdown(points, m_playerInfo->GetCountdown(), m_playerInfo->GetCountdownHighPrecision());
		}
	}

	delete playerGlobalBal;
}

void CPlayer::CheckMultiplierDivider( DWORD fallbackMultiplier, DWORD fallbackDivider )
{
	if ( m_playerInfo != nullptr && m_playerInfo->CheckMultiplierDivider( fallbackMultiplier, fallbackDivider ) )
	{
		StorePlayerInfoDataToNVRAM();
	}
}

void CPlayer::SetBonusEvents( const CBonusEvents *bonusEvents )
{
	// If there are no bonus events for this player, add it.
	if (m_sessionInfo.GetBonusEvents() == nullptr)
	{
		if (bonusEvents != nullptr)
		{
			CBonusEvents* tempBonusEvents = new CBonusEvents(*bonusEvents, MEMORY_NVRAM, NVRAM_PLAYER_BONUS_EVENTS_OFFSET);
			m_sessionInfo.SetBonusEvents(tempBonusEvents);
			SetBonusExistanceToNVRAM();
		}
	}

	// If we already have bonus events, merge the new table.
	else
	{
		(m_sessionInfo.GetBonusEvents())->MergeEvents(bonusEvents);
	}
}

DWORD CPlayer::GetCurrentCountdown( void ) const
{
	DWORD countdown = 0;

	if (m_playerInfo && m_playerInfo->IsExtendedBucketsAvailable() && m_pointsCountdownControlByUniverse)
	{
		if (m_playerInfo != nullptr)
		{
			countdown = m_sessionInfo.GetEBSessionCountdown(m_playerInfo->GetEBPointsId());
		}
	}
	else
	{
		countdown = m_sessionInfo.GetSessionCountdown();
	}

	return countdown;
}

void CPlayer::SetCurrentCountdown( DWORD currentCountdown )
{
	m_sessionInfo.SetSessionCountdown(currentCountdown);
	StoreSessionDataToNVRAM();
}

bool CPlayer::DecrementCountdown(CMeters *meters, CMeters *origMeters, bool isMGMDEnabled)
{
	bool bBonusPointsAwarded = false;

	if (m_playerInfo != nullptr)
	{
		bool isMeterDeltaApplied (false);
		
		m_bPointsAdded = false;

		//Pass 'decrement' value to SessionPoint class as meter delta
		isMeterDeltaApplied = m_sessionInfo.ApplyMeterChange(meters, origMeters, m_bPartialBonusing, isMGMDEnabled, bBonusPointsAwarded, m_bPointsAdded);

		//Update with new countdown value
		if(isMeterDeltaApplied)
			StoreSessionDataToNVRAM();
	}

	return bBonusPointsAwarded;
}

DWORD CPlayer::GetSessionPoints( void ) const
{
	DWORD sessionPoints = 0;

	if (m_playerInfo && m_playerInfo->IsExtendedBucketsAvailable() && m_pointsCountdownControlByUniverse)
	{
		if (m_playerInfo != nullptr)
		{
			sessionPoints = m_sessionInfo.GetEBSessionPoints(m_playerInfo->GetEBPointsId());
		}
	}
	else
	{
		sessionPoints = m_sessionInfo.GetSessionPoints();
	}

	return sessionPoints;
}

bool CPlayer::HasPointsBeenAdded( void ) const
{
	return m_bPointsAdded;
}

CBonusEvent * CPlayer::GetOutstandingBonusEvent( bool bIsCardOutTime )
{
	CBonusEvent *bonusEvent = nullptr;

	if ( m_sessionInfo.GetBonusEvents() != nullptr )
	{
		bonusEvent = (m_sessionInfo.GetBonusEvents())->GetOutstandingBonusEvent( bIsCardOutTime );
	}

	return bonusEvent;
}

void CPlayer::ResetSession( time_t utcTime )
{
	if ( m_playerInfo != nullptr )
	{
		DWORD points = m_playerInfo->GetPoints() + m_sessionInfo.GetSessionPoints() + GetBonusPoints();
		DWORD countdown = m_sessionInfo.GetSessionCountdown();
		
		m_playerInfo->SetPointsAndCountdown( points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION );
		m_sessionInfo.SetSessionPoints(0);
		m_sessionInfo.SetSessionCountdown(0);
		m_sessionInfo.SetSessionPointsFlushed(0);
		m_lastSessionPoints = 0;

		ResetLastUpdateSentTime(utcTime);

		StorePlayerInfoDataToNVRAM();
		StoreSessionDataToNVRAM();
	}

	if (m_sessionInfo.GetBonusEvents() != nullptr )
	{
		(m_sessionInfo.GetBonusEvents())->SessionReset();
	}
}

DWORD CPlayer::GetBonusPoints( void ) const
{
	DWORD bonusPoints = 0;

	if (m_sessionInfo.GetBonusEvents() != nullptr )
	{
		bonusPoints = (m_sessionInfo.GetBonusEvents())->GetBonusPoints();
	}

	return bonusPoints;
}

bool CPlayer::DisplayAccountBalanceButton() const
{
	bool retVal = true;

	if (m_bIsVirtual || (m_cardInPointsDisplayType == CIDT_NONE && !m_showCardInCompDisplay))
	{
		retVal = false;
	}

	return retVal;
}

std::string CPlayer::GetTieredPointsAsString(void) const
{
	string retStr;

	if (IsUniverseEnabled() &&
		m_playerInfo != nullptr &&
		m_playerInfo->GetTierPoints() != CPollerPlayerResponseMessage::DEFAULT_TIER_POINTS)
	{
		retStr = CUtilities::GetFormattedNumberString(m_playerInfo->GetTierPoints());
	}
	
	return retStr;
}

bool CPlayer::DisplayPointsOnBalanceScreen() const
{
	bool retVal = true;

	if ( m_cardInPointsDisplayType == CIDT_NONE )
	{
		retVal = false;
	}

	return retVal;
}

DWORD CPlayer::GetTierMultiplierPoints() const
{
	DWORD tierMultiplierPoints = 0;

	if (m_playerInfo != nullptr)
	{
		//TR #27073
		//We need to add the tier multiplier only if it is greater than zero
		DWORD tierMultiplier = m_playerInfo->GetTierMultiplier();
		if(tierMultiplier > 0)
		{
			//We need to do this because we can have tier multipliers with decimals now
			tierMultiplierPoints = (DWORD)(m_sessionInfo.GetSessionPoints())  *  tierMultiplier;
				tierMultiplierPoints = (tierMultiplierPoints / ONE_DOLLAR_IN_CENTS ) +
					((tierMultiplierPoints % ONE_DOLLAR_IN_CENTS) / (ONE_DOLLAR_IN_CENTS / 2));
		}
	}

	return tierMultiplierPoints;
}

DWORD CPlayer::GetRankId() const
{
	DWORD rankId(CPlayerInfo::INVALID_RANK_ID);

	if (m_playerInfo != nullptr)
	{
		rankId = m_playerInfo->GetRankId();
	}

	return rankId;
}

/// <summary>
/// Returns the cash value in pennies for the point value provided.
/// </summary>
/// <param name="points">IN - The point value to convert.</param>
/// <returns>The cash value in pennies for the point value provided.</returns>
/// <remarks>
/// It is okay if overflow occurs.
/// </remarks>
DWORD CPlayer::GetPointsAsCash(DWORD points) const
{
	DWORD pennies = points * ONE_DOLLAR_IN_CENTS / m_pointsPerDollar;
	return pennies;
}

/// <summary>
/// Get the total of all session points and bonus points earned this session.
/// </summary>
/// <returns>The total of all session points and bonus points earned this session.</returns>
/// <remarks>
/// It is okay if overflow occurs.
/// </remarks>
DWORD CPlayer::GetSessionAndBonusPoints( void ) const
{
	DWORD points = GetSessionPoints() + GetBonusPoints() + GetTierMultiplierPoints();
	return points;
}

DWORD CPlayer::GetTotalPoints(void) const
{
	DWORD totalPoints = 0;

	if (m_playerInfo != nullptr)
	{
		totalPoints = m_playerInfo->GetPoints();

		if (m_playerInfo->GetTierPoints() == CPollerPlayerResponseMessage::DEFAULT_TIER_POINTS)
		{
			totalPoints += GetSessionAndBonusPoints();
		}
	}

	return totalPoints;
}

// Get the total of all session points, bonus points earned this session, and all prior session points in pennies.
// It is okay if overflow occurs.
DWORD CPlayer::GetTotalPointsAsCash( void ) const
{
	DWORD totalPoints = GetTotalPoints();
	DWORD pennies = GetPointsAsCash(totalPoints);
	return pennies;
}

// Get the total of all session points and bonus points earned this session in pennies.
// It is okay if overflow occurs.
DWORD CPlayer::GetSessionPointsAsCash( void ) const
{
	DWORD totalPoints = GetSessionAndBonusPoints();
	DWORD pennies = GetPointsAsCash(totalPoints);
	return pennies;
}

std::string CPlayer::GetTotalPointsAsCashString( void ) const
{
	DWORD cashPoints = GetTotalPointsAsCash();
	string retStr(CUtilities::GetFormattedCashString(cashPoints));
	return retStr;
}

std::string CPlayer::GetSessionPointsAsCashString( void ) const
{
	DWORD cashPoints = GetSessionPointsAsCash();
	string retStr(CUtilities::GetFormattedCashString(cashPoints));
	return retStr;
}

std::string CPlayer::GetCompsAsCashString( void ) const
{
	string retStr;

	if ( m_playerInfo != nullptr && m_showCardInCompDisplay)//Tr. 020551
	{
		DWORD compCents = m_playerInfo->GetCompValue();
		retStr = CUtilities::GetFormattedCashString(compCents);
	}

	return retStr;
}

std::string CPlayer::GetFormattedString(const std::string &noNameStr, const std::string &appendStr) const
{
	std::string retStr(noNameStr);
	retStr += _T(" ");
	retStr += appendStr;
	return retStr;
}

void CPlayer::GetInPlayText(std::string &text, std::vector<std::string> &args) const
{
	if ( m_inPlayDisplayTypeConfig == IPDT_TOTAL )
	{
		string points = CUtilities::GetFormattedNumberString(this->GetTotalPoints());

		if (m_bDisplayName)
		{
			text = m_notificationTextConfig.GetText(CNotificationTextConfig::PointsWithName);
			args.push_back(GetName());
			args.push_back(points);
		}
		else
		{
			text = m_notificationTextConfig.GetText(CNotificationTextConfig::Points);
			args.push_back(points);
		}
	}
	else if ( m_inPlayDisplayTypeConfig == IPDT_TOTALCASH )
	{
		string pointsAsCash = GetTotalPointsAsCashString();

		if (m_bDisplayName)
		{
			text = m_notificationTextConfig.GetText(CNotificationTextConfig::PointsAsCashWithName);
			args.push_back(GetName());
			args.push_back(pointsAsCash);
		}
		else
		{
			text = m_notificationTextConfig.GetText(CNotificationTextConfig::PointsAsCash);
			args.push_back(pointsAsCash);
		}
	}
	else if ( m_inPlayDisplayTypeConfig == IPDT_GOOD_CARD )
	{
		text = GetGoodCardText(true);
	}
}

void CPlayer::GetCardInPointsSingleLineString(std::string &text, std::vector<std::string> &args) const
{
	if ( m_cardInPointsDisplayType != CIDT_NONE )
	{
		// We are set to display the total points as cash at card in.
		if (m_cardInPointsDisplayType == CIDT_TOTALCASH || 
			(m_universeEnabled && m_showPointsVsAccountBalance))
		{
			text = m_notificationTextConfig.GetText(CNotificationTextConfig::PointsAsCash);
			args.push_back(GetTotalPointsAsCashString());
		}
		else
		{
			// Otherwise we display the points
			text = m_notificationTextConfig.GetText(CNotificationTextConfig::Points);
			args.push_back(CUtilities::GetFormattedNumberString(GetTotalPoints()));
		}
	}
}

void CPlayer::GetCardInCompSingleLineString(std::string &text, std::vector<std::string> &args) const
{
	// We are set to display he comps at card in time.
	if ( m_showCardInCompDisplay )
	{
		DWORD compValue(0);
		if (m_playerInfo != nullptr)
		{
			compValue = m_playerInfo->GetCompValue();
		}

		text = m_notificationTextConfig.GetText(CNotificationTextConfig::Comp);
		args.push_back(CUtilities::GetFormattedCashString(compValue));
	}
}

bool CPlayer::IsAnniversaryToday() const
{
	return (m_playerInfo && m_playerInfo->IsAnniversaryToday());
}

bool CPlayer::IsBirthdayToday() const
{
	return (m_playerInfo && m_playerInfo->IsBirthdayToday());
}

void CPlayer::GetFormattedWelcome(std::string &text, std::vector<std::string> &argList) const
{
	if (m_bDisplayName)
	{
		text = m_notificationTextConfig.GetText(CNotificationTextConfig::WelcomeWithName);
		argList.push_back(GetName());
	}
	else
	{
		text = m_notificationTextConfig.GetText(CNotificationTextConfig::Welcome);
	}
}

std::string CPlayer::GetFormattedBalanceRetrievalText( void ) const
{
	return m_notificationTextConfig.GetText(CNotificationTextConfig::BalanceRetrieval);
}

std::string CPlayer::GetGoodCardText(bool inPlayText) const
{
	string retStr;
	if (m_mobilePlayer)
	{
		retStr = m_notificationTextConfig.GetText(CNotificationTextConfig::MobileSessionAccepted);
	}
	else
	{
		// Configured to only display a good card message
		if (inPlayText)
		{
			retStr = m_notificationTextConfig.GetText(CNotificationTextConfig::GoodCardRead_InPlay);
		}
		else
		{
			retStr = m_notificationTextConfig.GetText(CNotificationTextConfig::GoodCardRead);
		}
	}
	return retStr;
}

std::string CPlayer::GetMaxCardLimitText() const
{
	string retStr;

	if (m_playerInfo != nullptr)
	{
		retStr = m_playerInfo->GetResponseText();
	}

	return retStr;
}

std::string CPlayer::GetFormattedTierPointsLabel(void) const
{
	string retStr;

	if (!m_tierPointsLabel.empty())
	{
		retStr = m_tierPointsLabel;
	}
	else
	{
		retStr = CUtilities::LoadResourceString(IDS_TIER_POINTS);
		if (retStr.empty())
		{
			retStr = TXT_PLAYER_TIER_POINTS;
		}
	}

	return retStr;
}

void CPlayer::GetFormattedDailyPointsText(std::string &text, std::vector<std::string> &argList) const
{
	if (m_playerInfo != nullptr)
	{
		DWORD dailyPointValue = m_playerInfo->GetDailyPoints();
		if (dailyPointValue != CPlayerInfo::INVALID_DAILY_POINTS)
		{
			text = m_notificationTextConfig.GetText(CNotificationTextConfig::DailyEarnedPoints);
			argList.push_back(CUtilities::GetFormattedNumberString(dailyPointValue));
		}
	}
}

void CPlayer::GetFormattedPointsToNextTierText(std::string &text, std::vector<std::string> &argList) const
{
	if (m_playerInfo != nullptr)
	{
		DWORD pointsToNextTierValue = m_playerInfo->GetPointsTillNextTier();
		if (pointsToNextTierValue != CPlayerInfo::INVALID_POINTS_TILL_NEXT_TIER)
		{
			text = m_notificationTextConfig.GetText(CNotificationTextConfig::PointsNeededForNextTier);
			argList.push_back(CUtilities::GetFormattedNumberString(pointsToNextTierValue));
		}
	}
}

bool CPlayer::IsPBEnabled( void ) const
{
	bool bPBEnabled = false;

	if ( m_bPBEnabled && m_playerInfo != nullptr && m_playerInfo->IsPBTAllowed() && !IsVirtual() )
	{
		bPBEnabled = true;
	}

	return bPBEnabled;
}

bool CPlayer::DoPBOnCardIn( void ) const
{
	bool bDoPBOnCardIn = false;

	if ( m_bPBOnCardIn && IsPBEnabled() )
	{
		bDoPBOnCardIn = true;
	}

	return bDoPBOnCardIn;
}

InPlayDisplayType CPlayer::GetInPlayDisplayType( void ) const
{
	return m_inPlayDisplayTypeConfig;
}

void CPlayer::SetLossLimitEnabled(bool enabled)
{
	m_lossLimitEnabled = enabled;
}

CMeters *CPlayer::GetSessionStartMetersCopy( void ) const
{
	CMeters *meters = nullptr;
	meters = m_sessionInfo.GetSessionStartMetersCopy();

	return meters;
}

void CPlayer::SetSessionStartMeters( CMeters *meters )
{
	if (meters != nullptr)
	{
		m_sessionInfo.SetSessionStartMeters(meters);
	}
}

void CPlayer::SetSessionStartMetersCurrent(CMeters *meters)
{
	if (meters != nullptr)
	{
		m_sessionInfo.SetSessionStartMetersCurrent(meters);
	}
}

void CPlayer::ResetCurrentSessionData()
{
	// Reset current session points to session points(at flush)
	m_sessionInfo.SetSessionPointsFlushed(GetSessionPoints() + GetTierMultiplierPoints());
	m_sessionInfo.ResetCurrentBonusData();
}

void CPlayer::ResetLastUpdateSentTime( time_t utcTime )
{
	m_lastUpdateSentTime = utcTime;
}

time_t CPlayer::GetLastUpdateSentTime( void ) const
{
	return m_lastUpdateSentTime;
}

bool CPlayer::IsGoodbyeEnabled() const
{
	return m_isGoodbyeEnabled;
}

WORD CPlayer::GetLossLimitSessionBalance() const
{
	return m_lossLimitSessionBalance;
}

void CPlayer::SetLossLimitSessionBalance(WORD newBalance)
{
	m_lossLimitSessionBalance = newBalance;
	StoreSessionDataToNVRAM();
}

bool CPlayer::GetHasPlayed() const
{
	return m_hasPlayed;
}

void CPlayer::SetHasPlayed()
{
	m_hasPlayed = true;
	StoreSessionDataToNVRAM();
}

bool CPlayer::IsMaxCardLimitReached() const
{
	bool limitReached(false);

	if (m_playerInfo != nullptr)
	{
		limitReached = m_playerInfo->IsMaxCardLimitReached();
	}

	return limitReached;
}

void CPlayer::SetVirtualFlag(bool virtualFlag)
{
	m_bIsVirtual = virtualFlag;
}

bool CPlayer::IsRankValid() const
{
	return (m_playerInfo != nullptr && m_playerInfo->IsRankValid());
}

// Set the flag in NVRAM indicating that a player exists.
// NOTE: Keep this function as static
void CPlayer::SetPlayerExistanceToNVRAM( bool bPlayerExists )
{
	if ( m_type == MEMORY_NVRAM )
	{
		byte playerCheckByteValue = bPlayerExists ? NVRAM_PLAYER_CHECKBYTE_VALUE : ~NVRAM_PLAYER_CHECKBYTE_VALUE;

		if ( !Write( NVRAM_PLAYER_CHECKBYTE_OFFSET, &playerCheckByteValue, sizeof (playerCheckByteValue) ) )
			LogString(ERROR_LOG, "Could not write player check byte to NVRAM" );
	}
}

// Store all the card object for the player data to NVRAM.
void CPlayer::StoreCardDataToNVRAM( void )
{
	if ( m_type == MEMORY_NVRAM && m_card != nullptr )
	{

		byte nvramBuffer[NVRAM_CARD_SIZE];

		int bytesInBuffer = m_card->GetNvramBuffer( nvramBuffer, NVRAM_CARD_SIZE );

		if ( bytesInBuffer > 0 )
		{
			if ( !Write( NVRAM_PLAYER_CARD_OFFSET, nvramBuffer, bytesInBuffer ) )
				LogString(ERROR_LOG, "Could not write player card data to NVRAM" );
		}
		else
		{
			LogString(ERROR_LOG, "Could not get player card data to write to NVRAM" );
		}
	}
}

// Store the player info object for the play in NVRAM
void CPlayer::StorePlayerInfoDataToNVRAM( void )
{
	if ( m_type == MEMORY_NVRAM && m_playerInfo != nullptr )
	{
		byte nvramBuffer[NVRAM_PLAYER_INFO_SIZE];

		int bytesInBuffer = m_playerInfo->GetNvramBuffer( nvramBuffer, NVRAM_PLAYER_INFO_SIZE );

		if ( bytesInBuffer > 0 )
		{
			if ( !Write( NVRAM_PLAYER_INFO_OFFSET, nvramBuffer, bytesInBuffer ) )
				LogString(ERROR_LOG, "Could not write player info data to NVRAM" );
		}
		else
		{
			LogString(ERROR_LOG, "Could not get player info data to write to NVRAM" );
		}
	}
}

// Set the last play time to NVRAM.
void CPlayer::StoreLastPlayTimeToNVRAM( void )
{
	if ( m_type == MEMORY_NVRAM )
	{
		byte nvramBuffer[NVRAM_TIME_SIZE];

		int bytesInBuffer = CUtilities::GetNvramBufferFromTime( m_lastPlayTime, nvramBuffer, NVRAM_TIME_SIZE );

		if ( bytesInBuffer > 0 )
		{
			if ( !Write( NVRAM_PLAYER_PLAY_TIME_OFFSET, nvramBuffer, bytesInBuffer ) )
				LogString(ERROR_LOG, "Could not write last play data to NVRAM" );
		}
		else
		{
			LogString(ERROR_LOG, "Could not get last play data to write to NVRAM" );
		}
	}
}

// Set all the player's session related data to NVRAM.
void CPlayer::StoreSessionDataToNVRAM( void )
{
	if ( m_type == MEMORY_NVRAM )
	{
		byte nvramBuffer[NVRAM_PLAYER_SESSION_SIZE];

		byte *bufferPos = nvramBuffer;

		// skipping session points related params.
		bufferPos += sizeof(DWORD) * 3;  // currentCountdown, sessionPoints, sessionPointsFlushed

		ZeroMemory(bufferPos, PARTIAL_BONUSING_REMAINDER_SIZE);
		bufferPos += PARTIAL_BONUSING_REMAINDER_SIZE;

		memcpy( bufferPos, &m_bPointsAdded, sizeof( m_bPointsAdded ) );
		bufferPos += sizeof( m_bPointsAdded );

		memcpy( bufferPos, &m_lossLimitSessionBalance, sizeof ( m_lossLimitSessionBalance ));
		bufferPos += sizeof (m_lossLimitSessionBalance);

		memcpy( bufferPos, &m_hasPlayed, sizeof(m_hasPlayed));
		bufferPos += sizeof(m_hasPlayed);

		memcpy( bufferPos, &m_mobilePlayer, sizeof( m_mobilePlayer ) );
		bufferPos += sizeof( m_mobilePlayer );

		int bytesInBuffer = bufferPos - nvramBuffer;
		if ( bytesInBuffer > 0 )
		{
			if ( !Write( NVRAM_PLAYER_SESSION_OFFSET, nvramBuffer, bytesInBuffer ) )
				LogString(ERROR_LOG, "Could not write player session data to NVRAM" );
		}
		else
		{
			LogString(ERROR_LOG, "Could not get player session data to write to NVRAM" );
		}
	}
}

// Store all the virtual session data to NVRAM.
void CPlayer::StoreVirtualSessionDataToNVRAM( void )
{
	if ( m_type == MEMORY_NVRAM )
	{
		byte nvramBuffer[NVRAM_PLAYER_VIRTUAL_SIZE];
		byte *bufferPos = nvramBuffer;

		memcpy( bufferPos, &m_bIsVirtual, sizeof( m_bIsVirtual ) );
		bufferPos += sizeof( m_bIsVirtual );

		if ( m_bIsVirtual )
		{
			CUtilities::GetNvramBufferFromTime( m_virtualStartTime, bufferPos, NVRAM_TIME_SIZE );
			bufferPos += NVRAM_TIME_SIZE;

			if ( m_virtualCard != nullptr )
			{
				m_virtualCard->GetNvramBuffer( bufferPos, NVRAM_CARD_SIZE );				
				bufferPos += NVRAM_CARD_SIZE;
			}
		}

		int bytesInBuffer = bufferPos - nvramBuffer;
		if ( bytesInBuffer > 0 )
		{
			if ( !Write( NVRAM_PLAYER_VIRTUAL_OFFSET, nvramBuffer, bytesInBuffer ) )
				LogString(ERROR_LOG, "Could not write player virtual session data to NVRAM" );
		}
		else
		{
			LogString(ERROR_LOG, "Could not get player virtual session data to write to NVRAM" );
		}
	}
}

// Set the flag in NVRAM indicating there is player 
void CPlayer::SetBonusExistanceToNVRAM( void )
{
	if ( m_type == MEMORY_NVRAM )
	{
		byte bonusCheckByteValue = m_sessionInfo.GetBonusEvents() != nullptr ? NVRAM_PLAYER_BONUS_EVENTS_CHECKBYTE_VALUE : ~NVRAM_PLAYER_BONUS_EVENTS_CHECKBYTE_VALUE;

		if ( !Write( NVRAM_PLAYER_BONUS_EVENTS_CHECKBYTE_OFFSET, &bonusCheckByteValue, sizeof (bonusCheckByteValue) ) )
			LogString(ERROR_LOG, "Could not write bonus check byte to NVRAM" );
	}
}

bool CPlayer::IsUniverseEnabled() const
{
	return m_universeEnabled;
}

bool CPlayer::DisplayProfileButton() const
{
	return m_displayProfileButton;
}

bool CPlayer::SetGameSession (SubgameSessionParameters &subgameSessionParams)
{
	return m_sessionInfo.SetGameSession(subgameSessionParams);
}

bool CPlayer::ApplySubgameSelectionResponse(SubgameSelectionResponse *response)
{
	if (m_playerInfo && m_playerInfo->IsExtendedBucketsAvailable() && m_pointsCountdownControlByUniverse)
	{
		response->m_subgameSessionParams.m_multiplier = INVALID_MULTIPLIER;
		response->m_subgameSessionParams.m_divider = INVALID_DIVIDER;
	}

	m_bPointsAdded = false;

	// Iterate countdown data received in 423 response and 
	// 1. validate each bucket's data. Skip if errors found
	// 2. start countdown tracking for the bucket if not started yet
	// 3. add its countdown calculation info to subgame session's parameters
	for (auto &ebCountdownData : response->m_ebCountdowns)
	{
		if (ebCountdownData->id == 0 ||
			ebCountdownData->multiplier == 0 ||
			ebCountdownData->divider == 0 ||
			ebCountdownData->countdown > ebCountdownData->divider)
		{
			LogString(SESSION_LOG, "[%s:%d] Bad countdown data encountered for [BucketId: %u], "
				"skipping starting countdown for this.", __FUNCTION__, __LINE__, ebCountdownData->id);
			continue;
		}

		m_sessionInfo.InitializeExtendedBucketCountdownTracking(ebCountdownData.get());

		response->m_subgameSessionParams.m_ebSessionParams[ebCountdownData->id] = ExtendedBucketSessionParameters(
			ebCountdownData->multiplier, ebCountdownData->divider);
	}

	return m_sessionInfo.ApplySubgameSessionParams(response->m_subgameSessionParams, m_bPartialBonusing, m_bPointsAdded);
}

bool CPlayer::GameStart()
{
	bool newSubgameSessionStart = m_sessionInfo.GameStart();
	return newSubgameSessionStart;
}

void CPlayer::SetPlayerInfoAndMultiplierDividerCountdown( CPlayerInfo *playerInfo, DWORD fallbackMultiplier, DWORD fallbackDivider, bool isMGMDEnabled, int denom, DWORD gameNumber, AccountingType fallbackAccountingType, bool fallbackNPFP)
{
	// Set the player info for this player
	SetPlayerInfo( playerInfo );

	// Make sure the multiplier and divisor are okay. Fallback to the config values if valid.
	CheckMultiplierDivider( fallbackMultiplier, fallbackDivider );

	SubgameSessionParameters params;
	if (m_playerInfo)
	{
		if (m_playerInfo->IsExtendedBucketsAvailable() && m_pointsCountdownControlByUniverse)
		{
			params.m_multiplier = INVALID_MULTIPLIER;
			params.m_divider = INVALID_DIVIDER;
		}
		else
		{
			params.m_multiplier = m_playerInfo->GetMultiplier();
			params.m_divider = m_playerInfo->GetDivider();
		}
	}
	params.m_accountingType = fallbackAccountingType;
	if (fallbackAccountingType == AccountingType::ACCOUNTING_COINS_IN && fallbackNPFP)
	{
		params.m_accountingType = AccountingType::ACCOUNTING_CASHABLE_PLAYED;
	}

	params.m_denom = denom;
	params.m_gameNumber = gameNumber;
	params.m_gameSessionSequenceNumber = 0;
	params.m_playSessionID = 0;
	
	if (isMGMDEnabled)
	{
		LogString(SESSION_LOG, "%s:%d MGMD Enabled. Countdown %u, Divider %u", __FUNCTION__, __LINE__, m_playerInfo->GetCountdown(), m_playerInfo->GetDivider());
		if (m_playerInfo->GetCountdown() > m_playerInfo->GetDivider())
		{
			// Countdown larger than divider signifies error from FloorLogix, deactivate MGMD and use fallback values
		     params.m_multiplier = DEFAULT_MULTIPLIER;
		     params.m_divider = NVRAM_PLAYER_TRACKING_DIVISOR_DEFAULT;
		     m_playerInfo->SetMultiplierDivider(DEFAULT_MULTIPLIER, NVRAM_PLAYER_TRACKING_DIVISOR_DEFAULT);
		     m_playerInfo->SetPointsAndCountdown(m_playerInfo->GetPoints(), DEFAULT_COUNTDOWN);
		     SetCurrentCountdown(DEFAULT_COUNTDOWN);
		     m_sessionInfo.SetStartingCountdown(DEFAULT_COUNTDOWN, DEFAULT_COUNTDOWN, 1);
		     m_sessionInfo.DeactivateMGMD();
		     StorePlayerInfoDataToNVRAM();
		     StoreSessionDataToNVRAM();
	    }
		
		params.m_accountingType = m_playerInfo->GetAccountingType();
	}
	
	LogString(SESSION_LOG, "%s:%d startingMultiplier: %u, startingDivider: %u", __FUNCTION__, __LINE__, m_playerInfo->GetMultiplier(), m_playerInfo->GetDivider());

	//Set the starting multiplier, divider and countdown
	SetStartingMultiplierDividerCountdown();

	const std::vector<ExtendedBucketsCountdownData*> ebsCountdownData = playerInfo->GetExtendedBucketsCountdownData();

	// Iterate countdown data received in 411 response and 
	// 1. validate each bucket's data. Skip if errors found
	// 2. start countdown tracking for the bucket if not started yet
	// 3. add its countdown calculation info to current subgame session's parameters
	for (auto ebCountdownData : ebsCountdownData)
	{
		if (ebCountdownData->id == 0 ||
			ebCountdownData->multiplier == 0 ||
			ebCountdownData->divider == 0 ||
			ebCountdownData->countdown > ebCountdownData->divider)
		{
			LogString(SESSION_LOG, "[%s:%d] Bad countdown data encountered for [BucketId: %u], "
				"skipping starting countdown for this.", __FUNCTION__, __LINE__, ebCountdownData->id);
			continue;
		}

		m_sessionInfo.InitializeExtendedBucketCountdownTracking(ebCountdownData);

		params.m_ebSessionParams[ebCountdownData->id] = ExtendedBucketSessionParameters(
			ebCountdownData->multiplier, ebCountdownData->divider);
	}

	SetGameSession(params);
}

void CPlayer::SetStartingMultiplierDividerCountdown()
{
	if (m_playerInfo != nullptr)
	{
		m_sessionInfo.SetStartingMultiplierDividerCountdown(m_playerInfo->GetMultiplier(),
			m_playerInfo->GetDivider(),
			m_playerInfo->GetCountdown(), m_playerInfo->GetCountdownHighPrecision(),
			CPlayerInfo::COUNTDOWN_PRECISION_MULTIPLIER);
	}
}

void CPlayer::GetActiveMultiplierDivider (DWORD &activeMultiplier, DWORD &activeDivider)
{
	m_sessionInfo.GetActiveMultiplierDivider(activeMultiplier, activeDivider);
}

string CPlayer::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId)
{
	string subscriptionDataResponse;

	if (PLAYER_FULL_SESSION_TOPIC == topic)
	{
		subscriptionDataResponse = ResponseForPlayerSessionFullSession();
	}
	else if (PLAYER_UNFLUSHED_SESSION_TOPIC == whereId)
	{
		subscriptionDataResponse = ResponseForPlayerSessionUnFlushedSession();
	}
	else if (PLAYER_FULL_SESSION_EXT_TOPIC == topic)
	{
		subscriptionDataResponse = ResponseForPlayerSessionExtFullSession();
	}
	else if (PLAYER_UNFLUSHED_SESSION_EXT_TOPIC == whereId)
	{
		subscriptionDataResponse = ResponseForPlayerSessionExtUnFlushedSession();
	}
	else if (EXTENDED_BUCKETS_TOPIC == topic)
	{
		if (m_playerInfo)
		{
			subscriptionDataResponse = m_playerInfo->GetExtendedBucketsJSONData();
		}
	}
	else if (PLAYER_INFO_TOPIC == topic)
	{
		if (m_playerInfo)
		{
			subscriptionDataResponse = ResponseForPlayerInfo();
		}
	}
	return subscriptionDataResponse;
}

string CPlayer::ResponseForPlayerSessionUnFlushedSession()
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key("PlayerID");							writer.Int((m_playerInfo != nullptr) ? m_playerInfo->GetPlayerId() : CPlayerInfo::INVALID_PLAYER_ID);
	writer.Key("CardNumber");						writer.String(CCard::MakeCStringCardNumber(GetCard()));
	writer.Key("DisplayName");						writer.String(GetName());
	writer.Key("SessionPoints");					writer.Uint((GetSessionPoints() + GetTierMultiplierPoints()) - m_sessionInfo.GetSessionPointsFlushed());
	writer.Key("BonusPoints");						writer.Uint(m_sessionInfo.GetBonusPointsCurrent());
	writer.Key("Countdown");						writer.Uint(GetCurrentCountdown());

	writer.Key("SessionMeters");
	{
		writer.StartObject();
		writer.Key("CoinInDelta");					writer.Int64(GetMeterDeltaUnflushed(gCI));
		writer.Key("CoinOutDelta");					writer.Int64(GetMeterDeltaUnflushed(gCO));
		writer.Key("GameStarts");					writer.Int64(GetMeterDeltaUnflushed(gGS));
		writer.Key("GamesWon");						writer.Int64(GetMeterDeltaUnflushed(gGW));
		writer.Key("CreditMeter");					writer.Int64(GetCurrentMeterValue(mCrd));
		writer.Key("ExternalBonusMachinePay");		writer.Int64(GetMeterDeltaUnflushed(mBonusMach));
		writer.Key("ExternalBonusAttendantPay");	writer.Int64(GetMeterDeltaUnflushed(mBonusAtt));
		writer.Key("ProgressiveAttendantPay");		writer.Int64(GetMeterDeltaUnflushed(mPrgPayAtt));
		writer.Key("ProgressiveMachinePay");		writer.Int64(GetMeterDeltaUnflushed(mPrgPayMac));
		writer.Key("LegacyBonus");					writer.Int64(GetMeterDeltaUnflushed(gBW));
		writer.Key("Jackpot Handpay");				writer.Int64(GetMeterDeltaUnflushed(gJP));
		writer.EndObject();
	}
	writer.EndObject(); 
	return writeStream.m_str;
}

string CPlayer::ResponseForPlayerSessionFullSession()
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key("PlayerID");							writer.Int((m_playerInfo != nullptr) ? m_playerInfo->GetPlayerId() : CPlayerInfo::INVALID_PLAYER_ID);
	writer.Key("CardNumber");						writer.String(CCard::MakeCStringCardNumber(GetCard()));
	writer.Key("DisplayName");						writer.String(GetName());
	writer.Key("SessionPoints");					writer.Uint(GetSessionPoints() + GetTierMultiplierPoints());
	writer.Key("BonusPoints");						writer.Uint(m_sessionInfo.GetBonusPoints());
	writer.Key("Countdown");						writer.Uint(GetCurrentCountdown());

	writer.Key("SessionMeters");
	{
		writer.StartObject();
		writer.Key("CoinInDelta");					writer.Int64(GetMeterDeltafull(gCI));
		writer.Key("CoinOutDelta");					writer.Int64(GetMeterDeltafull(gCO));
		writer.Key("GameStarts");					writer.Int64(GetMeterDeltafull(gGS));
		writer.Key("GamesWon");						writer.Int64(GetMeterDeltafull(gGW));
		writer.Key("CreditMeter");					writer.Int64(GetCurrentMeterValue(mCrd));
		writer.Key("ExternalBonusMachinePay");		writer.Int64(GetMeterDeltafull(mBonusMach));
		writer.Key("ExternalBonusAttendantPay");	writer.Int64(GetMeterDeltafull(mBonusAtt));
		writer.Key("ProgressiveAttendantPay");		writer.Int64(GetMeterDeltafull(mPrgPayAtt));
		writer.Key("ProgressiveMachinePay");		writer.Int64(GetMeterDeltafull(mPrgPayMac));
		writer.Key("LegacyBonus");					writer.Int64(GetMeterDeltafull(gBW));
		writer.Key("Jackpot Handpay");				writer.Int64(GetMeterDeltafull(gJP));
		writer.EndObject();
	}
	writer.EndObject();
	return writeStream.m_str;
}

string CPlayer::ResponseForPlayerSessionExtUnFlushedSession()
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	writer.StartObject();

	writer.Key("SessionMeters");
	{
		writer.StartObject();
		writer.Key("CoinDropDelta");						writer.Int64(GetMeterDeltaUnflushed(mCD));
		writer.Key("OneBillDelta");							writer.Int64(GetMeterDeltaUnflushed(mD1));
		writer.Key("TwoBillDelta");							writer.Int64(GetMeterDeltaUnflushed(mD2));
		writer.Key("FiveBillDelta");						writer.Int64(GetMeterDeltaUnflushed(mD5));
		writer.Key("TenBillDelta");							writer.Int64(GetMeterDeltaUnflushed(mD10));
		writer.Key("TwentyBillDelta");						writer.Int64(GetMeterDeltaUnflushed(mD20));
		writer.Key("HundredBillDelta");						writer.Int64(GetMeterDeltaUnflushed(mD100));
		writer.Key("TicketInDelta");						writer.Int64(GetMeterDeltaUnflushed(mCoupV));
		writer.Key("TicketOutDelta");						writer.Int64(GetMeterDeltaUnflushed(mTVal));
		writer.Key("TrueCoinInDelta");						writer.Int64(GetMeterDeltaUnflushed(mTCi));
		writer.Key("BillDropDelta");						writer.Int64(GetMeterDeltaUnflushed(mBD));
		writer.Key("TotalHandPayDelta");					writer.Int64(GetMeterDeltaUnflushed(mTHP));
		writer.Key("TicketPromoInDelta");					writer.Int64(GetMeterDeltaUnflushed(mCTktPV));
		writer.Key("HandPayJackpotReceiptDelta");			writer.Int64(GetMeterDeltaUnflushed(mHPWinRcptVal));
		writer.Key("ActualCoinDropDelta");					writer.Int64(GetMeterDeltaUnflushed(mActualCD));
		writer.Key("TicketPromoOutDelta");					writer.Int64(GetMeterDeltaUnflushed(mCPromoOutVal));
		writer.Key("CancelCreditDelta");					writer.Int64(GetMeterDeltaUnflushed(gCC));
		writer.Key("HandPayCancelCreditDelta");				writer.Int64(GetMeterDeltaUnflushed(mHCC));
		writer.Key("SlotDoorCountDelta");					writer.Int64(GetMeterDeltaUnflushed(mActualSlotDr));
		writer.Key("AFTCashableToGameDelta");				writer.Int64(GetMeterDeltaUnflushed(mPbAC2G));
		writer.Key("AFTRestrictedToGameDelta");				writer.Int64(GetMeterDeltaUnflushed(mPbAR2G));
		writer.Key("AFTNonRestrictedToGameDelta");			writer.Int64(GetMeterDeltaUnflushed(mPbAN2G));
		writer.Key("AFTCashableToHostDelta");				writer.Int64(GetMeterDeltaUnflushed(mPbAC2H));
		writer.Key("AFTRestrictedToHostDelta");				writer.Int64(GetMeterDeltaUnflushed(mPbAR2H));
		writer.Key("AFTNonRestrictedToHostDelta");			writer.Int64(GetMeterDeltaUnflushed(mPbAN2H));
		writer.Key("AFTDebitToGameDelta");					writer.Int64(GetMeterDeltaUnflushed(mPbAD2G));
		writer.Key("PhysicalCoinInDelta");					writer.Int64(GetMeterDeltaUnflushed(mTValueCI));
		writer.Key("PhysicalCoinOutDelta");					writer.Int64(GetMeterDeltaUnflushed(mTValueCO));
		writer.Key("RestrictedAmountPlayedDelta");			writer.Int64(GetMeterDeltaUnflushed(mRstrPlayd));
		writer.Key("WeightedAverageTheoPayPercent");		writer.Int64(GetCurrentMeterValue(mGameWTPP));
		writer.Key("TotalDropDelta");						writer.Int64(GetMeterDeltaUnflushed(mTotalDrop));
		writer.Key("CashableTicketInCreditsDelta");			writer.Int64(GetMeterDeltaUnflushed(mVchrTktDrop));
		writer.EndObject();
	}
	writer.EndObject();
	return writeStream.m_str;
}

string CPlayer::ResponseForPlayerSessionExtFullSession()
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	writer.StartObject();

	writer.Key("SessionMeters");
	{
		writer.StartObject();
		writer.Key("CoinDropDelta");						writer.Int64(GetMeterDeltafull(mCD));
		writer.Key("OneBillDelta");							writer.Int64(GetMeterDeltafull(mD1));
		writer.Key("TwoBillDelta");							writer.Int64(GetMeterDeltafull(mD2));
		writer.Key("FiveBillDelta");						writer.Int64(GetMeterDeltafull(mD5));
		writer.Key("TenBillDelta");							writer.Int64(GetMeterDeltafull(mD10));
		writer.Key("TwentyBillDelta");						writer.Int64(GetMeterDeltafull(mD20));
		writer.Key("HundredBillDelta");						writer.Int64(GetMeterDeltafull(mD100));
		writer.Key("TicketInDelta");						writer.Int64(GetMeterDeltafull(mCoupV));
		writer.Key("TicketOutDelta");						writer.Int64(GetMeterDeltafull(mTVal));
		writer.Key("TrueCoinInDelta");						writer.Int64(GetMeterDeltafull(mTCi));
		writer.Key("BillDropDelta");						writer.Int64(GetMeterDeltafull(mBD));
		writer.Key("TotalHandPayDelta");					writer.Int64(GetMeterDeltafull(mTHP));
		writer.Key("TicketPromoInDelta");					writer.Int64(GetMeterDeltafull(mCTktPV));
		writer.Key("HandPayJackpotReceiptDelta");			writer.Int64(GetMeterDeltafull(mHPWinRcptVal));
		writer.Key("ActualCoinDropDelta");					writer.Int64(GetMeterDeltafull(mActualCD));
		writer.Key("TicketPromoOutDelta");					writer.Int64(GetMeterDeltafull(mCPromoOutVal));
		writer.Key("CancelCreditDelta");					writer.Int64(GetMeterDeltafull(gCC));
		writer.Key("HandPayCancelCreditDelta");				writer.Int64(GetMeterDeltafull(mHCC));
		writer.Key("SlotDoorCountDelta");					writer.Int64(GetMeterDeltafull(mActualSlotDr));
		writer.Key("AFTCashableToGameDelta");				writer.Int64(GetMeterDeltafull(mPbAC2G));
		writer.Key("AFTRestrictedToGameDelta");				writer.Int64(GetMeterDeltafull(mPbAR2G));
		writer.Key("AFTNonRestrictedToGameDelta");			writer.Int64(GetMeterDeltafull(mPbAN2G));
		writer.Key("AFTCashableToHostDelta");				writer.Int64(GetMeterDeltafull(mPbAC2H));
		writer.Key("AFTRestrictedToHostDelta");				writer.Int64(GetMeterDeltafull(mPbAR2H));
		writer.Key("AFTNonRestrictedToHostDelta");			writer.Int64(GetMeterDeltafull(mPbAN2H));
		writer.Key("AFTDebitToGameDelta");					writer.Int64(GetMeterDeltafull(mPbAD2G));
		writer.Key("PhysicalCoinInDelta");					writer.Int64(GetMeterDeltafull(mTValueCI));
		writer.Key("PhysicalCoinOutDelta");					writer.Int64(GetMeterDeltafull(mTValueCO));
		writer.Key("RestrictedAmountPlayedDelta");			writer.Int64(GetMeterDeltafull(mRstrPlayd));
		writer.Key("WeightedAverageTheoPayPercent");		writer.Int64(GetCurrentMeterValue(mGameWTPP));
		writer.Key("TotalDropDelta");						writer.Int64(GetMeterDeltafull(mTotalDrop));
		writer.Key("CashableTicketInCreditsDelta");			writer.Int64(GetMeterDeltafull(mVchrTktDrop));
		writer.EndObject();
	}
	writer.EndObject();
	return writeStream.m_str;
}

string CPlayer::ResponseForPlayerInfo()
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key("Name");								writer.String(m_playerInfo->GetName());
	writer.Key("IsGuest");							writer.Bool(IsGuest());
	writer.Key("PlayerId");							writer.Int(static_cast<int>(m_playerInfo->GetPlayerId()));
	writer.Key("PBEnabled");						writer.Bool(IsPBEnabled());
	writer.Key("TierPoints");						writer.String(GetTieredPointsAsString());

	if (m_showPointsVsAccountBalance)
	{
		writer.Key("CashValueOfPoints");			writer.String(GetTotalPointsAsCashString());
	}
	else
	{
		writer.Key("PointBalance");					writer.String(CUtilities::GetFormattedNumberString(GetTotalPoints()));
	}

	writer.Key("CompDollars");						writer.String(GetCompsAsCashString());

	writer.Key("ExtendedBucketsAvailable");			writer.Bool(m_playerInfo->IsExtendedBucketsAvailable());
	writer.EndObject();

	return writeStream.m_str;
}

__int64 CPlayer::GetMeterDeltaUnflushed(byte meterNumber)
{
	__int64 currentMeterValue = INVALID_METER_VALUE;
	__int64 flushedMeterValue = INVALID_METER_VALUE;
	if (nullptr != m_games)
	{
		if (nullptr != m_games->GetEGM())
		{
			if (nullptr != (m_games->GetEGM())->GetMeters()->GetMeterByNumber(meterNumber))
			{
				currentMeterValue = m_games->GetEGM()->GetMeters()->GetMeterByNumber(meterNumber)->GetValue();
				if (nullptr != m_sessionInfo.GetSessionStartMetersCurrent()->GetMeterByNumber(meterNumber))
				{
					flushedMeterValue = m_sessionInfo.GetSessionStartMetersCurrent()->GetMeterByNumber(meterNumber)->GetValue();
				}
			}
		}
	}
	return (std::abs(currentMeterValue - flushedMeterValue));
}

__int64 CPlayer::GetMeterDeltafull(byte meterNumber)
{	
	__int64 currentMeterValue = INVALID_METER_VALUE;
	__int64 fullMeterValue = INVALID_METER_VALUE;
	if (nullptr != m_games)
	{
		if (nullptr != m_games->GetEGM())
		{
			if (nullptr != m_games->GetEGM()->GetMeters()->GetMeterByNumber(meterNumber))
			{
				currentMeterValue = m_games->GetEGM()->GetMeters()->GetMeterByNumber(meterNumber)->GetValue();
				if (nullptr != m_sessionInfo.GetSessionStartMeters()->GetMeterByNumber(meterNumber))
				{
					fullMeterValue = m_sessionInfo.GetSessionStartMeters()->GetMeterByNumber(meterNumber)->GetValue();
				}
			}
		}
	}
	return (std::abs(currentMeterValue - fullMeterValue));
}

__int64 CPlayer::GetCurrentMeterValue(byte meterNumber)
{
	__int64 currentMeterValue = INVALID_METER_VALUE;
	if (nullptr != m_games)
	{
		if (nullptr != m_games->GetEGM())
		{
			if (nullptr != m_games->GetEGM()->GetMeters()->GetMeterByNumber(meterNumber))
			{
				currentMeterValue = m_games->GetEGM()->GetMeters()->GetMeterByNumber(meterNumber)->GetValue();
			}
		}
	}
	return currentMeterValue;
}

void CPlayer::SetMobilePlayer()
{
	m_mobilePlayer = true;
	StoreSessionDataToNVRAM();
}

bool CPlayer::IsMobilePlayer() const
{
	return m_mobilePlayer;
}

bool CPlayer::GetExtendedBucketsAvailable() const
{
	return (m_playerInfo ? m_playerInfo->IsExtendedBucketsAvailable() : false);
}

void CPlayer::MoveToNextBucket()
{
	m_sessionInfo.MoveToNextBucket();
}

WORD CPlayer::GetBucketCount() const
{
	return m_sessionInfo.GetBucketCount();
}

bool CPlayer::GetCurrentBucketDetails(std::string &name, std::string &currency, DWORD &countdown, DWORD &sessionPoints) const
{
	DWORD id = 0;
	bool returnVal = m_sessionInfo.GetCurrentBucketDetails(id, name, currency, countdown, sessionPoints);

	if (returnVal)
	{
		if (id == 0)
		{
			// It is parent countdown tracking object for OASIS points
			// We want to include bonus and tier mult points also here
			sessionPoints = GetSessionAndBonusPoints();
		}

		// Resolve the display name if it is legacy bucket
		GetBucketDisplayName(id, name);
	}

	return returnVal;
}

void CPlayer::GetBucketDisplayName(DWORD id, std::string &name) const
{
	if (id == 0)
	{
		// ID 0 must always be mapped to Points bucket (OASIS bucket)
		name = LEGACY_POINTS;
	}
	else if (id == m_playerInfo->GetEBPointsId())
	{
		name = LEGACY_POINTS;
	}
	else if (id == m_playerInfo->GetEBPromosId())
	{
		name = LEGACY_PROMOS;
	}
	else if (id == m_playerInfo->GetEBCompsId())
	{
		name = LEGACY_COMPS;
	}
	else if (id == m_playerInfo->GetEBTierPointsId())
	{
		name = LEGACY_TIER_POINTS;
	}
}

void CPlayer::GetBucketsDisplayData(std::vector<std::unique_ptr<BucketDisplayData>> &data, DWORD &gameNumber, int &denom) const
{
	m_sessionInfo.GetBucketsDisplayData(data, gameNumber, denom);

	for (auto &displayData : data)
	{
		GetBucketDisplayName(displayData->id, displayData->displayName);
	}
}

void CPlayer::SetLeftoverCashTransferStatus(LeftoverCashTransferStatus status)
{
	m_LeftoverCashTransferStatus = status;
}

LeftoverCashTransferStatus CPlayer::GetLeftoverCashTransferStatus() const
{
	return m_LeftoverCashTransferStatus;
}

bool CPlayer::IsDisplayPlayerName() const
{
	return m_bDisplayName;
}

DWORD CPlayer::ConvertPlayerTxTypeToInternalMsgType(PlayerTransactionType type) const
{
	InternalMsgType msgType = INT_MSG_TRANSACTION_PLAYER_UPDATE;

	if (type == PTT_CARD_IN)
	{
		msgType = INT_MSG_TRANSACTION_PLAYER_CARD_IN;
	}
	else if (type == PTT_VIRTUAL_SESSION_START)
	{
		msgType = INT_MSG_TRANSACTION_VIRTUAL_SESSION_STARTED;
	}
	else if (type == PTT_CARD_OUT || type == PTT_VIRTUAL_SESSION_STOP)
	{
		msgType = INT_MSG_TRANSACTION_PLAYER_CARD_OUT;
	}

	return msgType;
}

DWORD CPlayer::GetBasepointsEarned(void)
{
	DWORD basepointsEarned(0);
	DWORD sessionPoints(GetSessionPoints());

	LogString(SESSION_LOG, "GetBasepointsEarned sessionPoints=%u m_lastSessionPoints=%u", sessionPoints, m_lastSessionPoints);

	if (sessionPoints > m_lastSessionPoints)
	{
		basepointsEarned = sessionPoints - m_lastSessionPoints;
		m_lastSessionPoints = sessionPoints;
	}

	return basepointsEarned;
}