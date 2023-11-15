
#pragma once
#include "Employee.h"
#include "Hardware/MemoryStorageEx.h"
#include "Game.h"
#include "Player.h"
#include "ProcessDump/OperatingSystem.h"
#include "ProgressiveProtocol/ProgressiveBonusingAvailable.h"
#include "Card.h"
#include "UI/ScreenDefs.h"
#include "TiltsDef.h"
#include "UI/SentinelDlgState.h"

// Declare friend class(es) for unit test access.
namespace SentinelUnitTests { class PlayerSessionUnitTests; }
namespace SentinelNativeUnitTests { class PollerProtocolUnitTests; }

typedef enum
{
	Inserted,
	Removed,
	RemovedInserted,
	Ignored,
	StayCarded,
} InsertTranslation;

typedef enum
{
	PLAYERTYPEIDLE,
	PLAYERTYPECARDED,
	PLAYERTYPEUNCARDED,
	PLAYERTYPEMOBILE
} PlayerTypeState;

class CPlayerSession :
	public CMemoryStorageEx, public CDataSubscriptionProvider
{
private:
	static const DWORD m_waitForSessionIdMatchTimeoutMilliseconds = 10000; // 10000 milliseconds = 10000 tick counts = 10 seconds.

	IOperatingSystem &m_operatingSystem;
	DWORD m_creditsInPennies;
	time_t m_lastCreditMeterZeroTime;
	DWORD m_virtualBadCardReadTicks;
	bool m_virtualBadCardRead;
	DWORD m_negligibleCreditsThreshold;
	bool m_employeeOverPlayerIdleCardOut;
	WORD m_employeeIdleCardOutSeconds;
	bool m_playerRecardEmployeeCardOut;
	bool m_virtualSessionEnabled;
	bool m_miniVirtualSessionEnabled;

	SYSTEMTIME m_lastGameStart;
	SYSTEMTIME m_lastGameEnd;

	/// <summary>
	/// Set true if a game end event is received.
	/// Set false if two game starts are received without a receiving a game end between them.
	/// </summary>
	bool m_egmSendsGameEnd;

	int m_sessionId;
	DWORD m_gameSessionId;  // counter to hold the current game session id
	bool m_gameSessionIdIncremented;  //whether game sequence number incremented or not
	DWORD m_playTransactionSequence;  // counter to hold the play tx sequence
	
	time_t m_lastSessionIdIncrementTime;
	DWORD m_sessionActiveNegligibleCreditsTimeoutMilliseconds;
	DWORD m_sessionActiveNonNegligibleCreditsTimeoutMilliseconds;
	bool m_cardlessGameActivityOccurred; // Within timeout period. From GameEnded() with uncancelled m_gameInProgress OR CreditMeterIncreased().
	DWORD m_cardlessGameActivityOccurredTickCount;
	bool m_cardedPlayer; // Includes virtual sessions, but not banned players.
	bool m_allowedPlayer; // Is player allowed. This is false if the max player reached or banned player.
	bool m_bannedPlayer;
	bool m_cardInSentToProgressiveProtocol;
	bool m_mobilePlayer;

	bool m_sessionActive; // Calculated directly from the following parameters.
	bool m_gameInProgress;
	bool m_gameStartedDuringUncardedPlay;
	bool m_namedPlayer; // False if !m_cardedPlayer.  True if m_cardedPlayer and !player.IsGuest().
	string m_playerName;

	// If m_sessionIdMatchStatus is false then the tick count does not matter.
	// If m_sessionIdMatchStatus is true and m_hasSessionIdMatch is true then the tickCount does not matter.
	// If m_sessionIdMatchStatus is true and m_hasSessionIdMatch is false then the tickCount is active.
	DWORD m_lastSessionIdMatchTickCount; // So rapid card in/out does not make or retain eligibility determined.
	bool m_hasSessionIdMatch; // Actually received a matching eligibility session identifier.
	bool m_sessionIdMatchStatus; // Also true for session identifier mismatch prior to timeout.

	long m_lastCoinsPlayed;
	int m_lastGameNumber;
	int m_lastDenom;
	int m_lastGameSessionGameNumber;
	int m_lastGameSessionGameDenom;
	int m_prevGameSessionGameNumber;
	int m_prevGameSessionGameDenom;
	int m_lastPlayedGameSessionGameNumber;
	int m_lastPlayedGameSessionGameDenom;

	void IncrementSessionId();
	DWORD CalcCardlessGameActivityTimeoutMilliseconds();
	void UpdateSessionActiveTimers();
	bool CalcSessionActive(bool incrementSessionId = false);
	bool CardlessGameActivityOccurred();
	DWORD CalcSessionIdMatchTimeoutMilliseconds();
	void LogSessionInfo(LPCTSTR description);

// #pragma region OfflineCoinIn_Variables
	bool m_sasStartupQueueEmpty;
	bool m_offlineCoinIn;
// #pragma endregion OfflineCoinIn_Variables

	bool m_egmSendsGameStart;
	PlayerTypeState m_prevPlayerState; 

	CSentinelDlgState *m_uiState;
	bool m_jackpotPending;
	bool m_processHandPayResetAfterGameStart;

	bool m_isVirtualSession;
	bool m_physicalCardSession;
	bool m_displayAutoCardOut;
	DWORD m_lastPlayerActivityOccurredTickCount;

public:
	static const DWORD NVRAM_DATA_OFFSET = NVRAM_PLAYER_SESSION2_DATA_OFFSET;
	static const DWORD NVRAM_DATA_SIZE = NVRAM_PLAYER_SESSION2_DATA_SIZE;
	static const DWORD NVRAM_ACTIVE_OFFSET = NVRAM_PLAYER_SESSION2_CHECKBYTE_OFFSET;
	static const BYTE NVRAM_ACTIVE_VALUE = NVRAM_PLAYER_SESSION2_CHECKBYTE_VALUE;

	static const DWORD PLAY_TX_SEQ_ID_DEFAULT;
	static const bool EGM_SENDS_GAME_END_DEFAULT = false;
	static const bool EGM_SENDS_GAME_START_DEFAULT = false;
	static const bool DEFAULT_CARD_IN_SENT_TO_PROGRESSIVE_PROTOCOL = false;

// #pragma region OfflineCoinIn_Constants
	static const bool DEFAULT_SAS_STARTUP_QUEUE_EMPTY = false;
	static const bool DEFAULT_OFFLINE_COIN_IN;
// #pragma endregion OfflineCoinIn_Constants
	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CPlayerSession"/> class.
	/// </summary>
	/// <param name="operatingSystem">IN/OUT - The operating environment.</param>
	/// <param name="memoryType">IN - Type of the memory.</param>
	CPlayerSession(MEMORY_TYPE memoryType = MEMORY_NONE,
		IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CPlayerSession"/> class.
	/// </summary>
	~CPlayerSession();

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CPlayerSession"/> class.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	CPlayerSession(const CPlayerSession &rhs);

	/// <summary>
	/// Assignment operator.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	/// <returns>A reference to the target instance.</returns>
	CPlayerSession& operator=(const CPlayerSession &rhs);

	/// <summary>
	/// Updates the instance from an NVRAM buffer.
	/// </summary>
	/// <param name="buffer">IN - The buffer.</param>
	/// <param name="bufferSize">IN - The size of the buffer.</param>
	void SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize);

	/// <summary>
	/// Gets the NVRAM buffer for the instance.
	/// </summary>
	/// <param name="buffer">OUT - The buffer.</param>
	/// <param name="bufferSize">IN - The size of the buffer.</param>
	/// <param name="active">OUT - true to indicate the the record is active, false to indicate inactive.</param>
	/// <returns>The size of the buffer actually used.</returns>
	UINT GetNvramBuffer(BYTE *buffer, UINT bufferSize, bool &active) const;


	/// <summary>
	/// Sets the credits in pennies.
	/// </summary>
	/// <param name="credits">IN - The credits in pennies.</param>
	/// <param name="sessionActiveInfoChanged">OUT - true if session active info and player data should be sent to the progressive protocol, false otherwise.</returns>
	/// <param name="creditsIncreased">OUT - true if credits increased, false otherwise.</returns>
	/// <returns>Bool value indicating if HasSiginificantCredits state changed.</returns>
	bool SetCreditsInPennies(DWORD credits, bool &sessionActiveInfoChanged, bool& creditsIncreased);

	/// <summary>
	/// Determines whether [has significant credits].
	/// </summary>
	/// <returns></returns>
	bool HasSignificantCredits();

	/// <summary>
	/// Returns credits in pennies.
	/// </summary>
	/// <returns>DWORD credits in pennies</returns>
	DWORD GetCreditsInPennies();

	/// <summary>
	/// Transforms the inserted card.
	/// </summary>
	/// <param name="isCardInserted">Indicates if a physical card is inserted</param>
	/// <param name="employee">Employee or NULL if no employee</param>
	/// <param name="player">IN/OUT - Player or NULL if no player</param>
	/// <param name="card">Inserted card</param>
	/// <returns></returns>
	InsertTranslation TransformInsertedCard(bool isCardInserted, CEmployee *employee, CPlayer *player, CCard *card);

	/// <summary>
	/// Transforms the inserted player card.
	/// </summary>
	/// <param name="isCardInserted">Indicates if a physical card is inserted</param>
	/// <param name="employee">Employee or NULL if no employee</param>
	/// <param name="player">IN/OUT - Player or NULL if no player</param>
	/// <param name="card">Inserted card</param>
	/// <returns></returns>
	InsertTranslation TransformInsertedPlayerCard(bool isCardInserted, CEmployee *employee, CPlayer *player, CCard *card);

	/// <summary>
	/// Transforms the inserted employee card.
	/// </summary>
	/// <param name="isCardInserted">Indicates if a physical card is inserted</param>
	/// <param name="employee">Employee or NULL if no employee</param>
	/// <param name="player">IN/OUT - Player or NULL if no player</param>
	/// <param name="card">Inserted card</param>
	/// <returns>The insert translation.</returns>
	InsertTranslation TransformInsertedEmployeeCard(bool isCardInserted, CEmployee *employee, CPlayer *player, CCard *card);

	/// <summary>
	/// Sets the virtual bad card read flag.
	/// </summary>
	/// <param name="isCardInserted">IN - Is card physically inserted.</param>
	/// <remarks>
	/// If isCardInserted==false then the VirtualBadCardRead timer is started and marked active.
	/// If isCardInserted==true then the VirtualBadCardRead timer marked inactive (and not expired).
	/// ---
	/// I.e., use isCardInserted==true after a normal card insertion.
	/// I.e., use isCardInserted==true after a good RFID read.
	/// I.e., use isCardInserted==false after a bad RFID read.
	/// </remarks>
	void SetVirtualBadCardRead(bool isCardInserted);

	/// <summary>
	/// Gets the virtual bad card read flag.
	/// </summary>
	/// <returns>The virtual bad card read flag.</returns>
	bool IsVirtualBadCardRead();

	/// <summary>
	/// Clears the virtual bad card read flag.
	/// </summary>
	void ClearVirtualBadCardRead();

	/// <summary>
	/// Determines whether RFID bad card read timer has expired. Resets bad card read flag if timer has expired
	/// </summary>
	/// <returns>Indicates if timer has expired.</returns>
	/// <remarks>
	/// If true is returned then the timer is marked inactive (and not expired).
	/// </remarks>
	bool HasVirtualBadCardReadExpired();

	/// <summary>
	/// Gets whether to recard a virtual player session when employee cards out.
	/// </summary>
	/// <returns>true to recard a virtual player session when employee cards out, false otherwise.</returns>
	/// <remarks>
	/// Regardless of the return value, do not recard a virtual player session unless the card in for the employee session caused the virtual player session.
	/// </remarks>
	bool GetPlayerRecardEmployeeCardOut();

	/// <summary>
	/// Gets whether carded employee sessions timeout
	/// when there is a player session in progress that was made virtual by the employee card in.
	/// </summary>
	/// <returns>true if carded employee sessions timeout ..., false otherwise.</returns>
	bool GetEmployeeOverPlayerIdleCardOut();

	/// <summary>
	/// Gets the employee idle timeout duration in seconds for automatically carding out a employee session
	/// when there is a player session in progress that was made virtual by the employee card in.
	/// </summary>
	/// <returns>The employee idle timeout duration in seconds for automatically carding out a employee session ...</returns>
	WORD GetEmployeeIdleCardOutSeconds();

	/// <summary>
	/// Updates configuration items.
	/// </summary>
	/// <param name="config">IN - The configuration.</param>
	void UpdateConfigItems(const CConfig &config);

	/// <summary>
	/// Gets whether the EGM sends game end events to nCompass.
	/// </summary>
	/// <returns>true if the EGM provides game end events, false otherwise.</returns>
	bool GetEgmSendsGameEnd() const;

	/// <summary>
	/// Gets whether the EGM sends game start events to nCompass.
	/// </summary>
	/// <returns>true if the EGM provides game start events, false otherwise.</returns>
	bool GetEgmSendsGameStart() const;

// #pragma region SessionActive
	/// <summary>
	/// Gets session active information.
	/// </summary>
	/// <param name="sessionActive">OUT - The session active state.</param>
	/// <param name="cardedPlayer">OUT - true if the progressive protocol should treat the player as carded, false otherwise.</param>
	/// <param name="sessionId">OUT - The session identifier.</param>
	void GetSessionActiveInfo(bool &sessionActive, bool &cardedPlayer, int &sessionId) const;

	/// <summary>
	/// Gets the session identifier.
	/// </summary>
	/// <returns>The session identifier.</returns>
	int GetSessionId() const;

	/// <summary>
	/// Ihcrement the game session if if subgame is changes or denom is changed.
	/// </summary>
	/// <param name="games">IN - Game object holding info about current game number and denom.</param>
	/// <returns>None.</returns>
	void IncrementGameSessionIdIfRequired(CGames &games);

	/// <summary>
	/// Gets the current game session identifier.
	/// </summary>
	/// <returns>The game session identifier.</returns>
	DWORD GetGameSessionId() const;

	/// <summary>
	/// Check whether game start is received
	/// </summary>
	/// <returns>Return true if game start is received else false.</returns>
	bool IsGameSessionIdIncremented();

	/// <summary>
	/// Get the play tx sequence number
	/// </summary>
	/// <returns>Returns the play tx sequence number.</returns>
	DWORD GetPlayTransactionSequenceNumber();

	/// <summary>
	/// Increment the play tx sequence number by one
	/// </summary>
	/// <returns>None.</returns>
	void IncrementPlayTransactionSequenceNumber();
		
	/// <summary>
	/// Gets the last session identifier increment time.
	/// </summary>
	/// <returns>The last session identifier increment time.</returns>
	time_t GetLastSessionIdIncrementTime() const;

	/// <summary>
	/// Indicates that credits were added to the credit meter.  Counts as game activity.</param>
	/// </summary>
	/// <returns>true if session active info and player data should be sent to the progressive protocol, false otherwise.</returns>
	bool CreditMeterIncreased();

	/// <summary>
	/// Gets the last game started time.
	/// </summary>
	/// <returns></returns>
	SYSTEMTIME GetLastGameStarted() const;

	/// <summary>
	/// Gets the last game end time.
	/// </summary>
	/// <returns></returns>
	SYSTEMTIME GetLastGameEnd() const;

	time_t GetLastCreditMeterZeroTime() const;

	/// <summary>
	/// Gets the game in progress status.
	/// </summary>
	/// <returns>true if instance behaves as if there is an EGM game in progress, false otherwise.</returns>
	bool GetGameInProgress() const;

	/// <summary>
	/// Gets the last game number.
	/// </summary>
	/// <returns>The last game number.</returns>
	int GetLastGameNumber() const;

	/// <summary>
	/// Sets the last game number.
	/// </summary>
	/// <param name="lastGameNumber">The last game number.</param>
	void SetLastGameNumber(int lastGameNumber);

	/// <summary>
	/// Gets the game number from last game session
	/// </summary>
	/// <returns>The game number from last game session.</returns>
	int GetLastGameSessionGameNumber() const;

	/// <summary>
	/// Sets game number for last game session.
	/// </summary>
	/// <param name="lastGameNumber">The game number for last game session.</param>
	void SetLastGameSessionGameNumber(int lastGameSessionGameNumber);

	/// <summary>
	/// Gets game denom from last game session.
	/// </summary>
	/// <returns>The game denom from last game session.</returns>
	int GetLastGameSessionGameDenom() const;

	/// <summary>
	/// Sets game denom for last game session.
	/// </summary>
	/// <param name="lastGameNumber">The game denom for last game session.</param>
	void SetLastGameSessionGameDenom(int lastGameSessionGameNumber);
	
	/// <summary>
	/// Gets the game number from last Played game session
	/// </summary>
	/// <returns>The game number from last Played game session.</returns>
	int GetLastPlayedGameSessionGameNumber() const;

	/// <summary>
	/// Gets game denom from last Played game session.
	/// </summary>
	/// <returns>The game denom from last Played game session.</returns>
	int GetLastPlayedGameSessionGameDenom() const;
	
	/// <summary>
	/// Sets game number and denom for last played game session.
	/// </summary>
	/// <param name="games">Reference to games object.</param>
	void SetLastPlayedGameSessionGameNumberAndDenom(CGames &games);
		
	/// <summary>
	/// Gets the game number from previous game session
	/// </summary>
	/// <returns>The game number from previous game 
	/// session.</returns> 
	int GetPrevGameSessionGameNumber() const;

	/// <summary>
	/// Sets game number for previous game session.
	/// </summary>
	/// <param name="lastGameNumber">The game number for previous 
	/// game session.</param> 
	void SetPrevGameSessionGameNumber(int prevGameSessionGameNumber);

	/// <summary>
	/// Gets game denom from previous game session.
	/// </summary>
	/// <returns>The game denom from previous game 
	/// session.</returns> 
	int GetPrevGameSessionGameDenom() const;

	/// <summary>
	/// Sets game denom for previous game session.
	/// </summary>
	/// <param name="prevGameNumber">The game denom for previous 
	/// game session.</param> 
	void SetPrevGameSessionGameDenom(int prevGameSessionGameNumber);

	/// <summary>
	/// Gets the last coins played.
	/// </summary>
	/// <returns>The last coins played.</returns>
	long GetLastCoinsPlayed() const;

	/// <summary>
	/// Sets the last coins played.
	/// </summary>
	/// <param name="lastGameNumber">The last coins played.</param>
	void SetLastCoinsPlayed(long lastCoinsPlayed);

	/// <summary>
	/// Gets the last denomination.
	/// </summary>
	/// <returns>The last denomination.</returns>
	int GetLastDenom() const;

	/// <summary>
	/// Sets the last denomination.
	/// </summary>
	/// <param name="lastGameNumber">The last denomination.</param>
	void SetLastDenom(int lastDenom);

	/// <summary>
	/// Indicates that there is an EGM game in progress.
	/// </summary>
	/// <returns>true if session active info and player data should be sent to the progressive protocol, false otherwise.</returns>
	/// <remarks>
	/// This method can update offline coin in information.
	/// </remarks>
	bool GameStarted();

	/// <summary>
	/// Indicates that there is not an EGM game in progress.
	/// </summary>
	/// <returns>true if session active info and player data should be sent to the progressive protocol, false otherwise.</returns>
	bool GameEnded();

	/// <summary>
	/// Sets player state information.</param>
	/// </summary>
	/// <param name="player">IN - The player data.</param>
	/// <param name="isCardInserted">IN - True if physical card is inserted in the card reader.</param>
	/// <returns>true if session active info and player data should be sent to the progressive protocol, false otherwise.</returns>
	/// <remarks>
	/// The caller can send session active info and player data to the progressive protocol, regardless of the return value,
	/// in case fields changed that are not monitored by this class.
	/// </remarks>
	bool SetPlayerState(const CPlayer *player, bool isCardInserted = false);

	/// <summary>
	/// Processes progressive bonusing available information. Returns true if the session identifier match status changed.
	/// </summary>
	/// <param name="progressiveBonusingAvailable">IN - The progressive bonusing available information.</param>
	/// <returns>true if the session identifier match status changed, false otherwise.</returns>
	/// <remarks>
	/// Use GetSessionIdMatchStatus() to get the current session identifier match status.
	/// </remarks>
	bool ProcessProgressiveBonusingAvailable(const CProgressiveBonusingAvailable &progressiveBonusingAvailable);

	/// <summary>
	/// Gets the session identifier match status.
	/// </summary>
	/// <returns>true if the session identifier matches the CEGMAwardManager::m_progressiveLevels->m_eligibilitySessionId
	/// or has not timed out since it matched.</returns>
	bool GetSessionIdMatchStatus() const;

	/// <summary>
	/// Updates the SessionActive timers and changes the SessionActive state as needed.
	/// </summary>
	/// <returns>true if session active info and player data should be sent to the progressive protocol, false otherwise.</returns>
	bool PerformSessionActiveTimedEvents();

	/// <summary>
	/// Checks the session identifier match timer(s) and changes the session identifier match status as needed.
	/// </summary>
	/// <returns>true if the session identifier match status changed, false otherwise.</returns>
	/// <remarks>
	/// Use GetSessionIdMatchStatus() to get the current session identifier match status.
	/// </remarks>
	bool PerformSessionIdMatchTimedEvents();

	/// <summary>
	/// Sets the card in sent to progressive protocol status.
	/// </summary>
	/// <param name="cardInSentToProgressiveProtocol">IN - true if card in information has been sent
	/// to the progressive protocol, false otherwise.</param>
	void SetCardInSentToProgressiveProtocol(bool cardInSentToProgressiveProtocol);

	/// <summary>
	/// Gets the card in sent to progressive protocol status.
	/// </summary>
	/// <returns>true if card in information has been sent to the progressive protocol, false otherwise.</returns>
	bool GetCardInSentToProgressiveProtocol();

	/// <summary>
	/// Checks if player is a non-guest.
	/// </summary>
	/// <returns>true if player is non-guests, false otherwise.</returns>
	bool IsNamedPlayer() const;

// #pragma endregion SessionActive

// #pragma region OfflineCoinIn_Methods
	/// <summary>
	/// Updates the SAS startup queue empty status.
	/// </summary>
	/// <param name="sasStartupQueueEmpty">IN - true if the SAS startup queue is empty, false otherwise.</param>
	/// <param name="initializeLastCoinsPlayedAndWon">IN - true if the last coins played and won need to be initalized,
	/// false otherwise.</param>
	/// <param name="sendOfflineCoinIn">IN - true if an offline coin in message should be sent to One Link.</param>
	/// <remarks>
	/// If initializeLastCoinsPlayedAndWon and sendOfflineCoinIn are both true
	/// then the last coins played and won needs to be initalized before sending an offline coin in message to One Link.
	/// </remarks>
	void UpdateSasStartupQueueEmpty(bool sasStartupQueueEmpty,
		bool &initializeLastCoinsPlayedAndWon,
		bool &sendOfflineCoinIn);

	/// <summary>
	/// Gets the SAS startup queue empty status.
	/// </summary>
	/// <returns>true if the SAS startup queue is empty, false otherwise.</returns>
	bool IsSasStartupQueueEmpty();
	// #pragma endregion OfflineCoinIn_Methods

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

	/// <summary> 
	/// Sets previous denom and game number to current values. 
	void SetPrevDenomAndGameNumber();

	/// <summary> 
	/// Returns the type of player as string : Uncarded|Mobile|Carded|Idle 
	string GetPlayerType() const;

	/// <summary> 
	/// Returns the type of player as enum value : Uncarded|Mobile|Carded|Idle 
	PlayerTypeState GetPlayerTypeEnum() const;

	/// <summary> 
	/// Sets previous player type : Uncarded|Mobile|Carded|Idle 
	void SetPrevPlayerType(std::string pType);
	
	/// <summary> 
	/// Returns previous player type as string : Uncarded|Mobile|Carded|Idle 
	std::string GetPrevPlayerType();

	/// <summary>
	/// Set last activity occured tick count.
	/// </summary>
	void PlayerActivityOccurred();

	/// <summary>
	/// Checks if player has inactive for X amount of tick count
	/// </summary>
	/// <param name="cardedInactivityTimeoutTickCount">Carded inactivity timeout tick count</param>
	/// <param name="autoCardOutCountdownSeconds">auto cardout countdown seconds</param>
	/// <param name="cardedPlayer">carded player flag</param>
	/// <param name="playerIsVirtual">virtual player indicator</param>
	/// <param name="gameInProgress">game in progress indicator</param>
	/// <param name="jackpotPending">jackpot pending indicator</param>
	/// <returns>returns true if player is inactive, false otherwise.</returns>
	bool IsInactivePlayer(DWORD cardedInactivityTimeoutTickCount,
		DWORD autoCardOutCountdownSeconds,
		bool cardedPlayer, bool playerIsVirtual, bool gameInProgress, bool jackpotPending);

	/// <summary>
	/// Performs carded inactivity timed events.
	/// </summary>
	/// <returns>Tilt code related to the carded inactivity player.</returns>
	TiltCode PerformCardedInactivityTimedEvents(const CConfig& config, bool playerIsVirtual);

	/// <summary>
	/// Sets the UI state object.
	/// </summary>
	void SetUiStateObject(CSentinelDlgState* uiState);

	/// <summary>
	/// Sets the jackpot pending flag.
	/// </summary>
	void SetJackpotPending(bool value);

	/// <summary>
	/// Checks if hand pay reset processing is needed after a game start.
	/// </summary>
	/// <returns>true if hand pay reset processing is needed, false otherwise.</returns>
	bool CheckIfHandPayResetProcessingIsNeededAfterGameStart();

	/// <summary>
	/// Sets the virtual session flag
	/// </summary>
	void SetIsVirtualSession(bool value);

	// Grant "friend" access for testing.
	friend class SentinelUnitTests::PlayerSessionUnitTests;
	friend class SentinelNativeUnitTests::PollerProtocolUnitTests;
};


