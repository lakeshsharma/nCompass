#include "stdafx.h"
#include "Config/ConfigExtData.h"
#include "PlayerSession.h"
#include "Utilities.h"
#include "Logging/LogString.h"

static const DWORD NEGLIGIBLECREDITSTHRESHOLD = 0;
static const bool EMPLOYEEOVERPLAYERIDLECARDOUT = false;
static const WORD EMPLOYEEIDLECARDOUTSECONDS = 120;
static const bool PLAYERRECARDEMPLOYEECARDOUT = false;
static const DWORD VIRTUALBADREADEXPIRETICKS = 3000;
static const string TOPIC_PLAYSESSION("PlaySession");
const LPCTSTR GUEST_PLAYER_NAME = "Guest";

static const std::string PLAYER_TYPE[] = { 
	"Idle",
	"Carded",
	"Uncarded",
	"Mobile"
};

static const int DEF_VAL = -1;

// https://stackoverflow.com/a/16957554
const bool CPlayerSession::DEFAULT_OFFLINE_COIN_IN(true);
const DWORD CPlayerSession::PLAY_TX_SEQ_ID_DEFAULT(0);

CPlayerSession::CPlayerSession(MEMORY_TYPE memoryType, IOperatingSystem &operatingSystem)
	:
	CMemoryStorageEx(memoryType, NVRAM_DATA_OFFSET, NVRAM_DATA_SIZE, NVRAM_ACTIVE_OFFSET, NVRAM_ACTIVE_VALUE),
	CDataSubscriptionProvider(TOPIC_PLAYSESSION),
	m_operatingSystem(operatingSystem),
	m_creditsInPennies(0),
	m_virtualBadCardRead(false),	
	m_virtualSessionEnabled(false),
	m_miniVirtualSessionEnabled(CConfigExtData::DEFAULT_MINI_VIRTUAL_SESSION_ENABLED),
	m_egmSendsGameEnd(EGM_SENDS_GAME_END_DEFAULT),
	m_sessionId(SESSION_ID_DEFAULT),
	m_gameSessionId (GAME_SESSION_ID_DEFAULT),	
	m_gameSessionIdIncremented(false),
	m_playTransactionSequence (PLAY_TX_SEQ_ID_DEFAULT),
	m_lastSessionIdIncrementTime(operatingSystem.GetCurrentTime()),
	m_sessionActiveNegligibleCreditsTimeoutMilliseconds(
		CConfigExtData::DEFAULT_SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS * MILLISECONDS_PER_SECOND),
	m_sessionActiveNonNegligibleCreditsTimeoutMilliseconds(
		CConfigExtData::DEFAULT_SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS * MILLISECONDS_PER_SECOND),
	m_cardlessGameActivityOccurred(false),
	m_cardlessGameActivityOccurredTickCount(0),
	m_cardedPlayer(false),
	m_allowedPlayer(true),
	m_bannedPlayer(false),
	m_cardInSentToProgressiveProtocol(DEFAULT_CARD_IN_SENT_TO_PROGRESSIVE_PROTOCOL),
	m_mobilePlayer(false),
	m_sessionActive(false),
	m_gameInProgress(false),
	m_gameStartedDuringUncardedPlay(false),
	m_namedPlayer(false),
	m_lastSessionIdMatchTickCount(0),
	m_hasSessionIdMatch(false),
	m_sessionIdMatchStatus(false),
	m_lastCoinsPlayed(-1),
	m_lastGameNumber(-1),
	m_lastDenom(-1),
	m_lastGameSessionGameNumber(DEF_VAL),
	m_lastGameSessionGameDenom(DEF_VAL),
	m_prevGameSessionGameNumber(DEF_VAL),
	m_prevGameSessionGameDenom(DEF_VAL),
	m_lastPlayedGameSessionGameNumber(DEF_VAL),
	m_lastPlayedGameSessionGameDenom(DEF_VAL),
	m_sasStartupQueueEmpty(DEFAULT_SAS_STARTUP_QUEUE_EMPTY),
	m_offlineCoinIn(DEFAULT_OFFLINE_COIN_IN),
	m_egmSendsGameStart(EGM_SENDS_GAME_START_DEFAULT),
	m_prevPlayerState(PLAYERTYPEIDLE),
	m_uiState(nullptr),
	m_jackpotPending(false),
	m_processHandPayResetAfterGameStart(false),
	m_isVirtualSession(false),
	m_physicalCardSession(false),
	m_displayAutoCardOut(false),
	m_lastPlayerActivityOccurredTickCount(0)
{
	m_lastCreditMeterZeroTime = CUtilities::GetUTCTime();
	m_negligibleCreditsThreshold = NEGLIGIBLECREDITSTHRESHOLD;
	m_employeeOverPlayerIdleCardOut = EMPLOYEEOVERPLAYERIDLECARDOUT;
	m_employeeIdleCardOutSeconds = EMPLOYEEIDLECARDOUTSECONDS;
	m_playerRecardEmployeeCardOut = PLAYERRECARDEMPLOYEECARDOUT;
	
	// Initialize game start/end dates to 1970
	m_lastGameStart = JAN1970_SYSTIME;
	m_lastGameEnd = JAN1970_SYSTIME;

	BuildYourself();
	LogSessionInfo(__TFUNCTION__);
}


CPlayerSession::~CPlayerSession()
{
}

CPlayerSession::CPlayerSession(const CPlayerSession &rhs) :
	CMemoryStorageEx(rhs.GetMemoryType(), NVRAM_DATA_OFFSET, NVRAM_DATA_SIZE, NVRAM_ACTIVE_OFFSET, NVRAM_ACTIVE_VALUE),
	CDataSubscriptionProvider(TOPIC_PLAYSESSION),
	m_operatingSystem(rhs.m_operatingSystem)
{
	*this = rhs;
}

CPlayerSession &CPlayerSession::operator=(const CPlayerSession &rhs)
{
	if (this != &rhs)
	{
		m_creditsInPennies = rhs.m_creditsInPennies;
		m_lastCreditMeterZeroTime = rhs.m_lastCreditMeterZeroTime;
		m_virtualBadCardReadTicks = rhs.m_virtualBadCardReadTicks;
		m_virtualBadCardRead = rhs.m_virtualBadCardRead;
		m_negligibleCreditsThreshold = rhs.m_negligibleCreditsThreshold;
		m_employeeOverPlayerIdleCardOut = rhs.m_employeeOverPlayerIdleCardOut;
		m_employeeIdleCardOutSeconds = rhs.m_employeeIdleCardOutSeconds;
		m_playerRecardEmployeeCardOut = rhs.m_playerRecardEmployeeCardOut;

		m_virtualSessionEnabled = rhs.m_virtualSessionEnabled;
		m_miniVirtualSessionEnabled = rhs.m_miniVirtualSessionEnabled;
		m_lastGameStart = rhs.m_lastGameStart;
		m_lastGameEnd = rhs.m_lastGameEnd;
		m_egmSendsGameEnd = rhs.m_egmSendsGameEnd;

		m_sessionId = rhs.m_sessionId;
		m_gameSessionId = rhs.m_gameSessionId;
		m_gameSessionIdIncremented = rhs.m_gameSessionIdIncremented;
		m_lastSessionIdIncrementTime = rhs.m_lastSessionIdIncrementTime;
		m_sessionActiveNegligibleCreditsTimeoutMilliseconds = rhs.m_sessionActiveNegligibleCreditsTimeoutMilliseconds;
		m_sessionActiveNonNegligibleCreditsTimeoutMilliseconds = rhs.m_sessionActiveNonNegligibleCreditsTimeoutMilliseconds;
		m_cardlessGameActivityOccurredTickCount = rhs.m_cardlessGameActivityOccurredTickCount;
		m_cardedPlayer = rhs.m_cardedPlayer;
        m_allowedPlayer = rhs.m_allowedPlayer;
		m_bannedPlayer = rhs.m_bannedPlayer;
		m_cardInSentToProgressiveProtocol = rhs.m_cardInSentToProgressiveProtocol;
		m_mobilePlayer = rhs.m_mobilePlayer;

		m_sessionActive = rhs.m_sessionActive;
		m_gameInProgress = rhs.m_gameInProgress;
		m_gameStartedDuringUncardedPlay = rhs.m_gameStartedDuringUncardedPlay;
		m_cardlessGameActivityOccurred = rhs.m_cardlessGameActivityOccurred;
		m_namedPlayer = rhs.m_namedPlayer;

		m_lastSessionIdMatchTickCount = rhs.m_lastSessionIdMatchTickCount;
		m_hasSessionIdMatch = rhs.m_hasSessionIdMatch;
		m_sessionIdMatchStatus = rhs.m_sessionIdMatchStatus;

		m_lastGameNumber = rhs.m_lastGameNumber;
		m_lastCoinsPlayed = rhs.m_lastCoinsPlayed;
		m_lastDenom = rhs.m_lastDenom;

		m_lastGameSessionGameNumber = rhs.m_lastGameSessionGameNumber;
		m_lastGameSessionGameDenom = rhs.m_lastGameSessionGameDenom;
		m_prevGameSessionGameNumber = rhs.m_prevGameSessionGameNumber;
		m_prevGameSessionGameDenom = rhs.m_prevGameSessionGameDenom;
		m_lastPlayedGameSessionGameNumber = rhs.m_lastPlayedGameSessionGameNumber;
		m_lastPlayedGameSessionGameDenom = rhs.m_lastPlayedGameSessionGameDenom;

		m_playTransactionSequence = rhs.m_playTransactionSequence;
		m_egmSendsGameStart = rhs.m_egmSendsGameStart;

		m_uiState = rhs.m_uiState;
		m_jackpotPending = rhs.m_jackpotPending;
		m_processHandPayResetAfterGameStart = rhs.m_processHandPayResetAfterGameStart;
		m_isVirtualSession = rhs.m_isVirtualSession;
		m_physicalCardSession = rhs.m_physicalCardSession;
		m_displayAutoCardOut = rhs.m_displayAutoCardOut;
		m_lastPlayerActivityOccurredTickCount = rhs.m_lastPlayerActivityOccurredTickCount;
	}
	
	m_prevPlayerState = rhs.m_prevPlayerState;

	return *this;
}

bool CPlayerSession::SetCreditsInPennies(DWORD creditsInPennies, bool &sessionActiveInfoChanged, bool& creditsIncreased)
{
	LogString(SESSION_LOG, "%s:%d :=  oldCreditsInPennies [%d] newCreditInPennies [%d] m_negligibleCreditsThreshold [%d]", 
		__FUNCTION__, __LINE__, m_creditsInPennies, creditsInPennies, m_negligibleCreditsThreshold);

	bool currState = HasSignificantCredits();
	bool retValue = false;

	sessionActiveInfoChanged = false;
	if (creditsInPennies > m_creditsInPennies)
	{
		creditsIncreased = true;
		sessionActiveInfoChanged = CreditMeterIncreased();
	}

	m_creditsInPennies = creditsInPennies;
	if (creditsInPennies == 0)
	{
		m_lastCreditMeterZeroTime = CUtilities::GetUTCTime();
	}

	bool newState(HasSignificantCredits());
	if (currState != newState)
	{
		retValue = true;
	}
	else
		retValue = false;

	return retValue;
}

bool CPlayerSession::HasSignificantCredits()
{
	return m_creditsInPennies > m_negligibleCreditsThreshold;
}

DWORD CPlayerSession::GetCreditsInPennies()
{
	return m_creditsInPennies;
}

InsertTranslation CPlayerSession::TransformInsertedCard(bool isCardInserted, CEmployee *employee, CPlayer *player, CCard *card)
{
	// Default action "Inserted" so unknown card types will get the bad card read treatment.
	InsertTranslation ret = Inserted;
	if (card != NULL)
	{
		switch (card->GetCardType())
		{
			case CARD_PLAYER :
			case CARD_TEMP :
				ret = TransformInsertedPlayerCard(isCardInserted, employee, player, card);
				break;
			case CARD_MECHANIC:
			case CARD_GLOBAL:
			case CARD_FLOOR:
			case CARD_CURRENCY:
			case CARD_METER:
				ret = TransformInsertedEmployeeCard(isCardInserted, employee, player, card);
				break;
			case CARD_UNKNOWN:
				if (player && !player->IsVirtual() && m_mobilePlayer)
					ret = Ignored;	
			default:
				break;
		}
		LogString(MCR_LOG, "Card translation: isInserted=%d, employee=%d, player=%d, result=%d", isCardInserted, employee!=NULL, player!=NULL, (int) ret);

	}

	return ret;
}

InsertTranslation CPlayerSession::TransformInsertedPlayerCard(bool isCardInserted, CEmployee *employee, CPlayer *player, CCard *card)
{
	// Default action "Inserted" so unknown card types will get the bad card read treatment.
	InsertTranslation ret = Inserted;

	if (player != NULL && !player->IsVirtual())
	{
		if (player->CompareCard(card))
		{
			// Cards match
			if (isCardInserted && !m_mobilePlayer)
			{
				ret = StayCarded;
			}
			else if (m_mobilePlayer)
			{
				ret = Ignored;
			}
			else
			{// RFID will remove
				ret = Removed;
			}
		}
		else
		{
			// Cards are different, ignore the new card if we think there are still significant credits on the EGM.
			// Also, ignore the new card if carding out the old card would create a virtual session for the old card.
			if (HasSignificantCredits() || (!m_virtualSessionEnabled && m_miniVirtualSessionEnabled && m_gameInProgress) || m_mobilePlayer)
			{
				ret = Ignored;
			}
			else
			{
				ret = RemovedInserted;
			}
		}
	}
	else
	{
		// No player already inserted
		if (employee != NULL)
		{
			ret = Ignored;
		}
		else if (player != NULL && player->IsVirtual() && !player->CompareCard(card))
		{
			// New card does not match existing virtual session.
			if (!isCardInserted || m_mobilePlayer)
			{
				// Player Session - RFID.
				// During a player virtual session, a swipe by another player will be ignored. The virtual session with the original player shall continue.
				ret = Ignored;
			}
			else
			{
				ret = Inserted;
			}
		}
		else
		{
			ret = Inserted;
		}
	}

	return ret;

}

InsertTranslation CPlayerSession::TransformInsertedEmployeeCard(bool isCardInserted, CEmployee *employee, CPlayer *player, CCard *card)
{
	// Default action "Inserted" so unknown card types will get the bad card read treatment.
	InsertTranslation ret = Inserted;

	if (employee != NULL)
	{
		// Employee already carded in
		if (employee->GetCard()->Compare(card))
		{
			if (isCardInserted)
			{
				ret = RemovedInserted;
			}
			else
			{
				ret = Removed;
			}
		}
		else
		{
			ret = RemovedInserted;
		}
	}
	else
	{
		// No employee card present
		if (player != NULL && !player->IsVirtual())
		{
			if (!m_mobilePlayer)
			{
				player->SetCardedOutOnEmployeeCardIn(true);
				ret = RemovedInserted;
			}
			else
			{
				ret = Inserted;
			}
		}
		else
		{
			ret = Inserted;
		}
	}

	return ret;
}

void CPlayerSession::SetVirtualBadCardRead(bool isCardInserted)
{
	if (!isCardInserted)
	{
		m_virtualBadCardReadTicks = CUtilities::GetTickCount();
		m_virtualBadCardRead = true;
	}
	else
	{
		ClearVirtualBadCardRead();
	}
}

void CPlayerSession::SetIsVirtualSession(bool value)
{
	m_isVirtualSession = value;
	if (m_isVirtualSession)
	{
		m_physicalCardSession = false;
	}
}

bool CPlayerSession::IsVirtualBadCardRead()
{
	// A good card read needs to cancel bad card read behavior
	return m_virtualBadCardRead;
}

void CPlayerSession::ClearVirtualBadCardRead()
{
	// A good card read needs to cancel bad card read behavior
	m_virtualBadCardRead = false;
}

bool CPlayerSession::HasVirtualBadCardReadExpired()
{
	bool retValue = CUtilities::GetTickCount() - m_virtualBadCardReadTicks > VIRTUALBADREADEXPIRETICKS;
	if (retValue && m_virtualBadCardRead)
	{
		m_virtualBadCardRead = false;
	}
	else
	{
		retValue = false;
	}
	return retValue;
}

bool CPlayerSession::GetEmployeeOverPlayerIdleCardOut()
{
	return m_employeeOverPlayerIdleCardOut;
}

WORD CPlayerSession::GetEmployeeIdleCardOutSeconds()
{
	return m_employeeIdleCardOutSeconds;
}

bool CPlayerSession::GetPlayerRecardEmployeeCardOut()
{
	return m_playerRecardEmployeeCardOut;
}

void CPlayerSession::UpdateConfigItems(const CConfig &config)
{
	m_negligibleCreditsThreshold = config.GetNegligibleCreditsThreshold();
	m_employeeOverPlayerIdleCardOut = config.GetEmployeeOverPlayerIdleCardOut();
	m_employeeIdleCardOutSeconds = config.GetEmployeeIdleCardOutSeconds();
	m_playerRecardEmployeeCardOut = config.GetPlayerRecardEmployeeCardOut();
	m_virtualSessionEnabled = config.VirtualSessionEnabled();
	m_miniVirtualSessionEnabled = config.MiniVirtualSessionEnabled();

	m_lastCoinsPlayed = config.GetLastCoinsPlayed();
	m_gameInProgress = config.GetGameInProgress();
	m_sessionActiveNegligibleCreditsTimeoutMilliseconds = config.GetSessionActiveNegligibleCreditsTimeoutSeconds() *
		MILLISECONDS_PER_SECOND;
	m_sessionActiveNonNegligibleCreditsTimeoutMilliseconds = config.GetSessionActiveNonNegligibleCreditsTimeoutSeconds() *
		MILLISECONDS_PER_SECOND;

	LogString(SESSION_LOG, "[CPlayerSession::UpdateConfigItems]: m_negligibleCreditsThreshold [%d] "
		"m_sessionActiveNegligibleCreditsTimeoutMilliseconds [%d] "
		"m_sessionActiveNonNegligibleCreditsTimeoutMilliseconds [%d]",
		m_negligibleCreditsThreshold,
		m_sessionActiveNegligibleCreditsTimeoutMilliseconds,
		m_sessionActiveNonNegligibleCreditsTimeoutMilliseconds);
}

void CPlayerSession::SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize)
{
	size_t offset(0);
	bool ok(true);

	CUtilities::MemCopyZero_FromOffset(&ok, &m_sessionId, sizeof(m_sessionId), buffer, bufferSize, &offset);

	//Write current game, denom and game session id to nvram to persist across reboot
	CUtilities::MemCopyZero_FromOffset(&ok, &m_lastGameNumber, sizeof(m_lastGameNumber), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_lastDenom, sizeof(m_lastDenom), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_gameSessionId, sizeof(m_gameSessionId), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_playTransactionSequence, sizeof(m_playTransactionSequence), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_creditsInPennies, sizeof(m_creditsInPennies), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_cardedPlayer, sizeof(m_cardedPlayer), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_sessionActive, sizeof(m_sessionActive), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_lastPlayedGameSessionGameNumber, sizeof(m_lastPlayedGameSessionGameNumber), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_lastPlayedGameSessionGameDenom, sizeof(m_lastPlayedGameSessionGameDenom), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_prevGameSessionGameNumber, sizeof(m_prevGameSessionGameNumber), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_prevGameSessionGameDenom, sizeof(m_prevGameSessionGameDenom), buffer, bufferSize, &offset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_prevPlayerState, sizeof(m_prevPlayerState), buffer, bufferSize, &offset);

	m_lastGameSessionGameNumber = m_lastGameNumber;
	m_lastGameSessionGameDenom = m_lastDenom;
}

UINT CPlayerSession::GetNvramBuffer(BYTE *buffer, UINT bufferSize, bool& active) const
{
	size_t offset(0);
	bool ok(true);

	active = true;
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_sessionId, sizeof(m_sessionId));

	//Read current game, denom and game session id from nvram
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_lastGameNumber, sizeof(m_lastGameNumber));
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_lastDenom, sizeof(m_lastDenom));
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_gameSessionId, sizeof(m_gameSessionId));
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_playTransactionSequence, sizeof(m_playTransactionSequence));
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_creditsInPennies, sizeof(m_creditsInPennies));
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_cardedPlayer, sizeof(m_cardedPlayer));
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_sessionActive, sizeof(m_sessionActive));
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_lastPlayedGameSessionGameNumber, sizeof(m_lastPlayedGameSessionGameNumber));
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_lastPlayedGameSessionGameDenom, sizeof(m_lastPlayedGameSessionGameDenom));
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_prevGameSessionGameNumber, sizeof(m_prevGameSessionGameNumber));
	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_prevGameSessionGameDenom, sizeof(m_prevGameSessionGameDenom));

	CUtilities::MemCopySafe_ToOffset(&ok, buffer, bufferSize, &offset, &m_prevPlayerState, sizeof(m_prevPlayerState));

	return (UINT)offset;
}

bool CPlayerSession::GetEgmSendsGameEnd() const
{
	return m_egmSendsGameEnd;
}

bool CPlayerSession::GetEgmSendsGameStart() const
{
	return m_egmSendsGameStart;
}

// 2015-03-29
//	Session Active when:
//		named player (carded non-guest) -- includes non-guest virtual session
//				Banned player counts as non-carded.
//				MaxCardLimitReached counts as non-carded.
//		OR game in progress triggerred while no card inserted
//		OR game activity occurred with last timeout seconds while no card inserted
//				Separate timeouts for Negligible and Non-Negligible credits present.
//				Credit meter increased counts as game activity
//				Game ended counts as game activity.
//				At startup, the game activity timer is set to expired.
//
//	Events:
//		Check Timed Events.
//			Eventually, expires m_cardlessGameActivityOccurredTickCount and sets m_cardlessGameActivityOccurred = false.
//		Credit Meter Increased.
//		Game Started.
//		Game Ended.
//		SetPlayerState called, which happens at:
//			Startup Initialization.
//			Card In. (player object != null)
//			Player Response Received From FloorLogix. (player object != null)
//				All carded and virtual sessions are treated as guests until this response is received.
//			Card Out Without starting virtual Session. (player object = null)
//			Virtual Session End. (player object = null)
//
// Increment m_sessionId when:
//		m_sessionActive changes.
//		m_cardedPlayer changes.

void CPlayerSession::GetSessionActiveInfo(bool &sessionActive, bool &allowedCardedPlayer, int &sessionId) const
{
	sessionActive = m_sessionActive;
	allowedCardedPlayer = (m_cardedPlayer || m_mobilePlayer) && m_allowedPlayer;
	sessionId = m_sessionId;
}

int CPlayerSession::GetSessionId() const
{
	return m_sessionId;
}

void CPlayerSession::IncrementGameSessionIdIfRequired(CGames &games)
{
	CGame *game = games.GetCurrentSessionGame();

	DWORD currentGameNumber 		= 0;
	int currentDenom 				= 0;
	
	if (game != nullptr)
	{
		currentGameNumber = game->GetGameNumber();
		currentDenom = games.GetCurrentDenom();
				
		if( (currentGameNumber != (DWORD)m_lastGameSessionGameNumber) || (currentDenom != m_lastGameSessionGameDenom) )
		{
			LogString(SESSION_LOG, "%s:%d := Current Game: %u, Denom: %d | Last Game: %u, Denom: %d", 
				__FUNCTION__, __LINE__,	currentGameNumber, currentDenom, m_lastGameSessionGameNumber, m_lastGameSessionGameDenom);
			
			++m_gameSessionId;
			StoreYourself();
			
			m_lastGameSessionGameNumber = currentGameNumber;
			m_lastGameSessionGameDenom = currentDenom;
			
			m_gameSessionIdIncremented = true;
			
			LogString(SESSION_LOG, "%s:%d := GameSessionId Incremented. New Last Game: %u, Denom: %d", __FUNCTION__, __LINE__, m_lastGameSessionGameNumber, m_lastGameSessionGameDenom);
			
		}
	}
	else
	{
		//If game doesn't exist then nothing should be changed
		LogString(SESSION_LOG, "%s:%d :=  Unchanged Last Game: %u, Denom: %d", __FUNCTION__, __LINE__, m_lastGameSessionGameNumber, m_lastGameSessionGameDenom);
	}
	
	return;
}

DWORD CPlayerSession::GetGameSessionId() const
{
	return m_gameSessionId;
}

bool CPlayerSession::IsGameSessionIdIncremented() 
{
	bool retVal = m_gameSessionIdIncremented;

	// Reset after first pass to take care of subsequent play
	m_gameSessionIdIncremented = false;

	return retVal;
}

void CPlayerSession::IncrementPlayTransactionSequenceNumber() 
{ 
	m_playTransactionSequence++; 
	StoreYourself();
}

DWORD CPlayerSession::GetPlayTransactionSequenceNumber()
{
	return m_playTransactionSequence;
}

time_t CPlayerSession::GetLastSessionIdIncrementTime() const
{
	return m_lastSessionIdIncrementTime;
}

void CPlayerSession::IncrementSessionId()
{

	++m_sessionId;
	LogString(SESSION_LOG, "%s: %d", __FUNCTION__, __LINE__);
	StoreYourself();

	m_playTransactionSequence = 0; // reset at start of new player session

    LogString(SESSION_LOG, "%s : Player session id is increased.The session id is: %d", __FUNCTION__, m_sessionId);
	m_lastSessionIdIncrementTime = m_operatingSystem.GetCurrentTime();
	if (m_hasSessionIdMatch)
	{
		// Track when the session identifer last matched, so rapid card in/out does does not make or retain a session identifier match.
		// Leave m_sessionIdMatchStatus as-is, until a timeout occurs and/or we receive progressive bonusing available information.
		m_lastSessionIdMatchTickCount = m_operatingSystem.GetTickCount32();
		m_hasSessionIdMatch = false;
	}
}

/// <summary>
/// Calculates the milliseconds remaining before cardless gaming activity times out.
/// </summary>
/// <param name="tickCount">IN The current tick count.</param>
/// <returns>Milliseconds remaining before timeout occurs, 0 if timed out.</returns>
DWORD CPlayerSession::CalcCardlessGameActivityTimeoutMilliseconds()
{
	DWORD timeout(0);

	if (m_cardlessGameActivityOccurred)
	{
		DWORD timeoutTickCount = HasSignificantCredits() ? m_sessionActiveNonNegligibleCreditsTimeoutMilliseconds : m_sessionActiveNegligibleCreditsTimeoutMilliseconds;
		DWORD elapsedTickCount = m_operatingSystem.GetTickCount32() - m_cardlessGameActivityOccurredTickCount;
		if (timeoutTickCount > elapsedTickCount)
		{
			timeout = timeoutTickCount - elapsedTickCount ;
		}
	}

	return timeout;
}

void CPlayerSession::UpdateSessionActiveTimers()
{
	if (m_cardlessGameActivityOccurred)
	{
		if (0 == CalcCardlessGameActivityTimeoutMilliseconds())
		{
			m_cardlessGameActivityOccurred = false;
		}
	}
}

bool CPlayerSession::CalcSessionActive(bool incrementSessionId)
{
	bool oldSessionActive = m_sessionActive;
	m_sessionActive = false; // Until determined otherwise.

	UpdateSessionActiveTimers();

	if ( m_cardedPlayer|| m_namedPlayer || m_mobilePlayer)
	{
		m_sessionActive = true;
	}
	else if (m_gameStartedDuringUncardedPlay && m_gameInProgress)
	{
		m_sessionActive = true;
	}
	else if (m_cardlessGameActivityOccurred)
	{
		m_sessionActive = true;
	}

	bool changed = incrementSessionId || ((oldSessionActive != m_sessionActive) && (m_allowedPlayer));
	if (changed)
	{
		LogString(SESSION_LOG, _T("%s %d"), __TFUNCTION__, __LINE__);
		IncrementSessionId();
		m_prevGameSessionGameDenom = m_lastGameSessionGameDenom;
		m_prevGameSessionGameNumber = m_lastGameSessionGameNumber;
		PlayerActivityOccurred();
	}
	return changed;
}

bool CPlayerSession::PerformSessionActiveTimedEvents()
{
	bool changed(CalcSessionActive());
	if (changed) LogSessionInfo(__TFUNCTION__);
	return changed;
}

bool CPlayerSession::CardlessGameActivityOccurred()
{
	m_cardlessGameActivityOccurredTickCount = m_operatingSystem.GetTickCount32();
	m_cardlessGameActivityOccurred = true;
	return CalcSessionActive();
}

bool CPlayerSession::CreditMeterIncreased()
{
	if (m_cardedPlayer || m_namedPlayer || m_mobilePlayer)
	{
		PlayerActivityOccurred();
		// We need to consider credit meter increase as game activity 
		// only while we are not in a carded session
		return false;
	}

	bool changed(CardlessGameActivityOccurred());
	if (changed) LogSessionInfo(__TFUNCTION__);
	return changed;
}

bool CPlayerSession::GetGameInProgress() const
{
	return m_gameInProgress;
}

int CPlayerSession::GetLastGameNumber() const
{
	return m_lastGameNumber;
}

void CPlayerSession::SetLastGameNumber(int lastGameNumber)
{
	m_lastGameNumber = lastGameNumber;
}

int CPlayerSession::GetLastGameSessionGameNumber() const
{
	return m_lastGameSessionGameNumber;
}

void CPlayerSession::SetLastGameSessionGameNumber(int lastGameSessionGameNumber)
{
	m_lastGameSessionGameNumber = lastGameSessionGameNumber;
}

int CPlayerSession::GetLastGameSessionGameDenom() const
{
	return m_lastGameSessionGameDenom;
}

int CPlayerSession::GetLastPlayedGameSessionGameNumber() const
{
	return m_lastPlayedGameSessionGameNumber;
}

int CPlayerSession::GetLastPlayedGameSessionGameDenom() const
{
	return m_lastPlayedGameSessionGameDenom;
}

void CPlayerSession::SetLastPlayedGameSessionGameNumberAndDenom(CGames &games)
{
	LogString(SESSION_LOG, "%s:%d :=  Last Played Game number: %u, Denom: %d", __FUNCTION__, __LINE__,	m_lastPlayedGameSessionGameNumber, m_lastPlayedGameSessionGameDenom);

    DWORD currentPlayedGameNumber 		= EGM_GAME_NUMBER;  
    int currentPlayedDenom 				= CGame::DEFAULT_DENOMINATION_MULTIPLIER;
	
	CGame *game = games.GetCurrentSessionGame();

    // If there is not a current subgame game found, use the EGM
    if (game == nullptr)
    {
        game = games.GetGame( EGM_GAME_NUMBER );
    }
	
	if (game != nullptr)
	{
		currentPlayedGameNumber = game->GetGameNumber();
		currentPlayedDenom = games.GetCurrentDenom();
	}

	LogString(SESSION_LOG, "%s:%d := current Played Game: %u, Denom: %d", __FUNCTION__, __LINE__,	currentPlayedGameNumber, currentPlayedDenom);
	
	if( (currentPlayedGameNumber != (DWORD)m_lastPlayedGameSessionGameNumber) || (currentPlayedDenom != m_lastPlayedGameSessionGameDenom) )
	{
		m_lastPlayedGameSessionGameNumber = currentPlayedGameNumber;
		m_lastPlayedGameSessionGameDenom = currentPlayedDenom;
	}

	LogString(SESSION_LOG, "%s:%d := New Last Played Game: %u, Denom: %d", __FUNCTION__, __LINE__,	m_lastPlayedGameSessionGameNumber, m_lastPlayedGameSessionGameDenom);
}

void CPlayerSession::SetLastGameSessionGameDenom(int lastGameSessionGameDenom)
{
	m_lastGameSessionGameDenom = lastGameSessionGameDenom;
}

int CPlayerSession::GetPrevGameSessionGameNumber() const
{
	return m_prevGameSessionGameNumber;
}

void CPlayerSession::SetPrevGameSessionGameNumber(int prevGameSessionGameNumber)
{
	m_prevGameSessionGameNumber = prevGameSessionGameNumber;
}

int CPlayerSession::GetPrevGameSessionGameDenom() const
{
	return m_prevGameSessionGameDenom;
}

void CPlayerSession::SetPrevGameSessionGameDenom(int prevGameSessionGameDenom)
{
	m_prevGameSessionGameDenom = prevGameSessionGameDenom;
}

long CPlayerSession::GetLastCoinsPlayed() const
{
	return m_lastCoinsPlayed;
}

void CPlayerSession::SetLastCoinsPlayed(long lastCoinsPlayed)
{
	m_lastCoinsPlayed = lastCoinsPlayed;
}

void CPlayerSession::SetLastDenom(int lastDenom)
{
	m_lastDenom = lastDenom;
}

int CPlayerSession::GetLastDenom() const
{
	return m_lastDenom;
}

bool CPlayerSession::GameStarted()
{
	PlayerActivityOccurred();
	m_jackpotPending = false;

	if (!m_sasStartupQueueEmpty)
	{
		m_offlineCoinIn = true;
	}

	if (m_gameInProgress)
	{
		m_egmSendsGameEnd = false;
	}

	m_gameInProgress = true;
	m_gameStartedDuringUncardedPlay = !(m_cardedPlayer || m_namedPlayer || m_mobilePlayer);
	m_egmSendsGameStart = true;
	CUtilities::GetUTCSystemTime(&m_lastGameStart);
	LogString(GENERAL, _T("PlayerSession: GameStarted - m_gameStartedDuringUncardedPlay [%d]"), m_gameStartedDuringUncardedPlay);

	m_lastGameNumber = -1;
	m_lastCoinsPlayed = -1;
	m_lastDenom = -1;

	bool changed(CalcSessionActive());
	if (changed) LogSessionInfo(__TFUNCTION__);
	return changed;
}

bool CPlayerSession::GameEnded()
{
	m_egmSendsGameEnd = true;

	m_gameInProgress = false;
	CUtilities::GetUTCSystemTime(&m_lastGameEnd);
	LogString(GENERAL, _T("PlayerSession: GameEnded"));

	bool changed = false;
	if (m_gameStartedDuringUncardedPlay)
	{
		// We need to consider game end as the last game
		// activity done, only when the game is started 
		// during uncarded session
		changed = CardlessGameActivityOccurred();
	}
	else
	{
		PlayerActivityOccurred();
		changed = CalcSessionActive();
	}

	if (changed) LogSessionInfo(__TFUNCTION__);

	m_gameStartedDuringUncardedPlay = false;
	return changed;
}

SYSTEMTIME CPlayerSession::GetLastGameStarted() const
{
	return m_lastGameStart;
}

SYSTEMTIME CPlayerSession::GetLastGameEnd() const
{
	return m_lastGameEnd;
}

time_t CPlayerSession::GetLastCreditMeterZeroTime() const
{
	return m_lastCreditMeterZeroTime;
}

bool CPlayerSession::SetPlayerState(const CPlayer *player, bool isCardInserted)
{
	bool oldCardedPlayer = m_cardedPlayer;
	bool oldMobilePlayer = m_mobilePlayer;
	bool oldNamedPlayer = m_namedPlayer;
	bool oldBannedPlayer = m_bannedPlayer;
	bool bannedPlayerWasForcedCardedOut(false);
	bool namedPlayerChanged = false;

	if (player)
	{
		if (player->IsBannedFromPlay())
		{
			m_cardedPlayer = false;
			m_namedPlayer = false;
			m_playerName = "";
			m_mobilePlayer = false;
			m_bannedPlayer = true;
			m_physicalCardSession = false;
		}
		else
		{
			m_bannedPlayer = false;
			if (player->IsMobilePlayer())
			{
				m_mobilePlayer = true;
			}
			else
			{
				m_cardedPlayer = true;
			}

			m_physicalCardSession = isCardInserted;
			m_namedPlayer = !player->IsGuest();
			if (m_namedPlayer)
			{
				m_playerName = player->GetName();
			}
			else
			{
				m_playerName = GUEST_PLAYER_NAME;
			}
			namedPlayerChanged = m_namedPlayer != oldNamedPlayer;

			// We are entering the carded session, ensure we reset m_cardlessGameActivityOccurred 
			// so that cardless session timeouts no longer occur and we do not identify ourself 
			// to be in the cardless session mistakenly when this carded session ends.
			// We also need to reset m_gameStartedDuringUncardedPlay because that is also solely 
			// used to track when to trigger Tx 139 (Uncarded Session End) but at card-in Tx 139 
			// has already been generated to end the older session.
			LogString(SESSION_LOG, "%s:%d := Resetting m_cardlessGameActivityOccurred and "
				"m_gameStartedDuringUncardedPlay.", __FUNCTION__, __LINE__);

			m_cardlessGameActivityOccurred = false;
			m_gameStartedDuringUncardedPlay = false;
		}
	}
	else
	{
		m_cardedPlayer = false;
		m_namedPlayer = false;
		m_playerName = "";
		m_mobilePlayer = false;
		m_bannedPlayer = false;
		m_physicalCardSession = false;
	}

	bannedPlayerWasForcedCardedOut = oldBannedPlayer && !m_bannedPlayer;

	if (player != NULL && (player->IsMaxCardLimitReached() || player->IsBannedFromPlay()))
	{
		LogString(POLLER_LOG, _T("%s: %d"), __TFUNCTION__, __LINE__);
		m_allowedPlayer = false;
	}
	else
	{
		m_allowedPlayer = true;
	}

	LogString(SESSION_LOG, "%s:%d := Player [0x%p] IsBannedFromPlay [%d] IsMaxCardLimitReached [%d] "
		"m_allowedPlayer [%d]. bannedPlayerWasForcedCardedOut [%d]", __FUNCTION__, __LINE__, player,
		(player != nullptr) ? player->IsBannedFromPlay() : 0,
		(player != nullptr) ? player->IsMaxCardLimitReached() : 0,
		m_allowedPlayer, bannedPlayerWasForcedCardedOut);

	bool incrementSessionId = ((oldCardedPlayer != m_cardedPlayer || oldMobilePlayer != m_mobilePlayer) && m_allowedPlayer) || bannedPlayerWasForcedCardedOut;
	LogString(POLLER_LOG, _T("%s %d incrementSessionId %u"), __TFUNCTION__, __LINE__, incrementSessionId);
	bool sessionActiveChanged = CalcSessionActive(incrementSessionId);
	LogString(SESSION_LOG, _T("%s m_sessionActive %d"), __TFUNCTION__, (int)m_sessionActive);
	bool changed = (namedPlayerChanged || sessionActiveChanged);
	if (changed)
	{
		LogSessionInfo(__TFUNCTION__);
	}

	return changed;
}

void CPlayerSession::SetPrevPlayerType(std::string pType)
{
	for(int state=PLAYERTYPEIDLE;state<=PLAYERTYPEMOBILE;state++)
		if (!PLAYER_TYPE[state].compare(pType))
			m_prevPlayerState = (PlayerTypeState)state;

	StoreYourself();
}

std::string CPlayerSession::GetPrevPlayerType()
{
	return PLAYER_TYPE[m_prevPlayerState];
}

bool CPlayerSession::ProcessProgressiveBonusingAvailable(const CProgressiveBonusingAvailable &progressiveBonusingAvailable)
{
	bool oldHasSessionIdMatch(m_hasSessionIdMatch);
	bool oldSessionIdMatchStatus(m_sessionIdMatchStatus);

	int sessionId(progressiveBonusingAvailable.GetSessionId());

	if (sessionId == m_sessionId)
	{
		m_hasSessionIdMatch = true;
		m_sessionIdMatchStatus = true;
	}

	bool changed(oldHasSessionIdMatch != m_hasSessionIdMatch || oldSessionIdMatchStatus != m_sessionIdMatchStatus);
	if (changed) LogSessionInfo(__TFUNCTION__);
	return changed;
}

bool CPlayerSession::GetSessionIdMatchStatus() const
{
	return m_sessionIdMatchStatus;
}

/// <summary>
/// Calculates the milliseconds remaining before a session ID mismatch status times out.
/// </summary>
/// <returns>Milliseconds remaining before timeout occurs, 0 if timed out or the session ID matches.</returns>
DWORD CPlayerSession::CalcSessionIdMatchTimeoutMilliseconds()
{
	DWORD timeout(0);

	if (m_sessionIdMatchStatus && !m_hasSessionIdMatch)
	{
		// Timer is active.
		DWORD elapsedTickCount = m_operatingSystem.GetTickCount32() - m_lastSessionIdMatchTickCount;
		if (m_waitForSessionIdMatchTimeoutMilliseconds > elapsedTickCount)
		{
			timeout = m_waitForSessionIdMatchTimeoutMilliseconds - elapsedTickCount;
		}
	}

	return timeout;
}

bool CPlayerSession::PerformSessionIdMatchTimedEvents()
{
	bool oldSessionIdMatchStatus(m_sessionIdMatchStatus);

	// If m_sessionIdMatchStatus is false then the timer does not matter.
	// If m_sessionIdMatchStatus is true and m_hasSessionIdMatch is true then the timer does not matter.
	// If m_sessionIdMatchStatus is true and m_hasSessionIdMatch is false then the timer is active.
	if (m_sessionIdMatchStatus && !m_hasSessionIdMatch)
	{
		// Timer is active.
		if (0 == CalcSessionIdMatchTimeoutMilliseconds())
		{
			m_sessionIdMatchStatus = false;
		}
	}

	bool changed(oldSessionIdMatchStatus != m_sessionIdMatchStatus);
	if (changed) LogSessionInfo(__TFUNCTION__);
	return changed;
}

/// <summary>
/// Logs the (player) session information.
/// </summary>
/// <param name="description">IN - The description of the caller conditions.</param>
/// <remarks>
/// Note that timer-related variables are updated asynchronously, so timer expirations may not match the associated variables.
/// </remarks>
void CPlayerSession::LogSessionInfo(LPCTSTR description)
{
	LogString(SESSION_LOG,
		"[%s] [CPlayerSession::LogSessionInfo]: PlayerType [%s] m_sessionId=%d m_sessionActive=%d HasSignificantCredits=%d m_gameInProgress=%d"
		" m_gameStartedDuringUncardedPlay=%d m_cardlessGameActivityOccurred=%d CalcCardlessGameActivityTimeoutMilliseconds=%u m_cardedPlayer=%d m_namedPlayer=%d m_mobilePlayer=%d"
		" m_sessionIdMatchStatus=%d m_hasSessionIdMatch=%d CalcSessionIdMatchTimeoutMilliseconds=%u m_virtualSessionEnabled=%d m_miniVirtualSessionEnabled=%d",
		description,
		GetPlayerType().c_str(),
		m_sessionId,
		m_sessionActive,
		HasSignificantCredits(),
		m_gameInProgress,
		m_gameStartedDuringUncardedPlay,
		m_cardlessGameActivityOccurred,
		CalcCardlessGameActivityTimeoutMilliseconds(),
		m_cardedPlayer,
		m_namedPlayer,
		m_mobilePlayer,
		m_sessionIdMatchStatus,
		m_hasSessionIdMatch,
		CalcSessionIdMatchTimeoutMilliseconds(),
		m_virtualSessionEnabled,
		m_miniVirtualSessionEnabled);
}

void CPlayerSession::SetCardInSentToProgressiveProtocol(bool cardInSentToProgressiveProtocol)
{
	m_cardInSentToProgressiveProtocol = cardInSentToProgressiveProtocol;
}

bool CPlayerSession::GetCardInSentToProgressiveProtocol()
{
	return m_cardInSentToProgressiveProtocol;
}

bool CPlayerSession::IsNamedPlayer() const
{
	return m_namedPlayer;
}

// #pragma region OfflineCoinIn_Methods
void CPlayerSession::UpdateSasStartupQueueEmpty(bool sasStartupQueueEmpty,
	bool &initializeLastCoinsPlayedAndWon,
	bool &sendOfflineCoinIn)
{
	initializeLastCoinsPlayedAndWon = false;
	sendOfflineCoinIn = false;

	if (!m_sasStartupQueueEmpty && sasStartupQueueEmpty)
	{
		initializeLastCoinsPlayedAndWon = true;
		if (m_offlineCoinIn)
		{
			m_offlineCoinIn = false;
			sendOfflineCoinIn = true;
		}
	}

	if (m_sasStartupQueueEmpty != sasStartupQueueEmpty)
	{
		m_sasStartupQueueEmpty = sasStartupQueueEmpty;
		LogString(GENERAL, "SasStartupQueueEmpty=%d initializeLastCoinsPlayedAndWon=%d sendOfflineCoinIn=%d",
			(int)m_sasStartupQueueEmpty,
			(int)initializeLastCoinsPlayedAndWon,
			(int)sendOfflineCoinIn);
	}
}

bool CPlayerSession::IsSasStartupQueueEmpty()
{
	return m_sasStartupQueueEmpty;
}
// #pragma endregion OfflineCoinIn_Methods

PlayerTypeState CPlayerSession::GetPlayerTypeEnum() const
{
	PlayerTypeState playerTypeEnum;

	if (m_cardedPlayer)
		playerTypeEnum = PLAYERTYPECARDED;
	else if (m_mobilePlayer)
		playerTypeEnum = PLAYERTYPEMOBILE;
	else if (m_sessionActive)
		playerTypeEnum = PLAYERTYPEUNCARDED;
	else
		playerTypeEnum = PLAYERTYPEIDLE;

	return playerTypeEnum;

}

string CPlayerSession::GetPlayerType() const
{
	return PLAYER_TYPE[GetPlayerTypeEnum()];
}

string CPlayerSession::GetSubscriptionDataResponse(const string& topic, const string& whereId)
{
	UNREFERENCED_PARAMETER(&whereId);
	JsonWriteStreamString writeStream;
	string resultString;
	if (TOPIC_PLAYSESSION == topic)
	{
		resultString = "{\"PlaySession\":";
		JsonWriter writer(writeStream);
		writer.StartObject();
		writer.Key("PlayerType");					writer.String(GetPlayerType());
		writer.Key("PlayerName");					writer.String(m_playerName);
		writer.Key("PlayerSessionID");				writer.Int(m_sessionId);
		writer.Key("IsVirtualSession");             writer.Bool(m_isVirtualSession);
		writer.Key("IsPhysicalCardSession");        writer.Bool(m_physicalCardSession);
		writer.Key("SessionInactive");				writer.Bool(m_displayAutoCardOut);
		writer.EndObject();
		resultString.append(writeStream.m_str+"}");
	}
	return resultString;
}

void CPlayerSession::SetPrevDenomAndGameNumber()
{
	m_prevGameSessionGameDenom = m_lastGameSessionGameDenom;
	m_prevGameSessionGameNumber = m_lastGameSessionGameNumber;
}

void CPlayerSession::PlayerActivityOccurred()
{	 
	m_displayAutoCardOut = false;
	m_lastPlayerActivityOccurredTickCount = CUtilities::GetTickCount();
}

bool CPlayerSession::IsInactivePlayer(DWORD cardedInactivityTimeoutTickCount,
	DWORD autoCardOutCountdownSeconds,
	bool cardedPlayer, bool playerIsVirtual, bool gameInProgress, bool jackpotPending)
{
	DWORD ticksSinceLastCardedActivityOccurred = CUtilities::GetTickCount() - m_lastPlayerActivityOccurredTickCount;
	return ((cardedInactivityTimeoutTickCount > 0 && autoCardOutCountdownSeconds > 0) &&
		(cardedPlayer && !playerIsVirtual) &&
		(ticksSinceLastCardedActivityOccurred >= cardedInactivityTimeoutTickCount) &&
		(!gameInProgress && !jackpotPending));
}

TiltCode CPlayerSession::PerformCardedInactivityTimedEvents(const CConfig& config, bool playerIsVirtual)
{
	TiltCode tilt(tltUnknown);

	DWORD cardedInactivityTimeoutTickCount((HasSignificantCredits() ?
		config.GetAutoCardOutSecToCountdownCredits() : config.GetAutoCardOutSecToCountdownNoCrdts()) * MILLISECONDS_PER_SECOND);

	if(!m_displayAutoCardOut &&
		IsInactivePlayer(cardedInactivityTimeoutTickCount,
			config.GetAutoCardOutCountdownSeconds(), (m_cardedPlayer || m_namedPlayer || m_mobilePlayer), 
			playerIsVirtual, m_gameInProgress, m_jackpotPending))
	{
		LogString(SESSION_LOG, "*****************DISPLAY AUTOCARDOUT*****************");
		PlayerActivityOccurred();
		tilt = HasSignificantCredits() ? tltAutoCardOutCountdownStartWithCredits : tltAutoCardOutCountdownStartWithNoCredits;
		m_displayAutoCardOut = true;
	}

	return tilt;
}

void CPlayerSession::SetUiStateObject(CSentinelDlgState* uiState)
{
	m_uiState = uiState;
}

void CPlayerSession::SetJackpotPending(bool value)
{
	PlayerActivityOccurred();
	m_jackpotPending = value;
	if (m_jackpotPending)
	{
		m_processHandPayResetAfterGameStart = true;
	}
}

bool CPlayerSession::CheckIfHandPayResetProcessingIsNeededAfterGameStart()
{
	bool oldProcessHandPayResetAfterGameStart(m_processHandPayResetAfterGameStart);
	m_processHandPayResetAfterGameStart = false;
	return oldProcessHandPayResetAfterGameStart;
}