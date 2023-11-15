#pragma once
#include "Hardware/MemoryStorage.h"

#include "Card.h"
#include "PlayerInfo.h"
#include "PlayerGlobalBalance.h"
#include "Config/Config.h"
#include "Config/EGMConfig.h"
#include "BonusEvents.h"
#include "InternalMsgContent.h"
#include "Meters.h"
#include "Subscription.h"
#include "UI/NotificationTextConfig.h"
#include "DataSubscriptionProvider.h"
#include "SessionPoints.h"
#include "Game.h"
#include "Games.h"
#include "SubgameSelectionResponse.h"

const DWORD NVRAM_PLAYER_SESSION_SIZE = 48;			// Room to add stuff.
const DWORD NVRAM_PLAYER_VIRTUAL_SIZE = 183;		// Room to add stuff.

const DWORD NVRAM_PLAYER_CARD_OFFSET =		NVRAM_PLAYER_DATA_OFFSET;
const DWORD NVRAM_OLD_PLAYER_INFO_OFFSET =		NVRAM_PLAYER_CARD_OFFSET + NVRAM_CARD_SIZE;
const DWORD NVRAM_PLAYER_PLAY_TIME_OFFSET = NVRAM_OLD_PLAYER_INFO_OFFSET + CPlayerInfo::NVRAM_OLD_PLAYER_INFO_SIZE;
const DWORD NVRAM_PLAYER_SESSION_OFFSET =	NVRAM_PLAYER_PLAY_TIME_OFFSET + NVRAM_TIME_SIZE;
const DWORD NVRAM_PLAYER_VIRTUAL_OFFSET =	NVRAM_PLAYER_SESSION_OFFSET + NVRAM_PLAYER_SESSION_SIZE;

namespace SentinelNativeUnitTests { class ProgressiveMessageFactoryUnitTests; }
namespace SentinelNativeUnitTests { class Player7; }

// In Play Display Types
typedef enum
{
	IPDT_TOTAL = 0,                 // Display total point only
	IPDT_TOTALCASH,                 // Display total points as cash only
	IPDT_TOTAL_SESSION,             // Display total points and session points
	IPDT_TOTALCASH_SESSIONCASH,     // Display total points as cash and session points as cash
	IPDT_COUNTDOWN_SESSION,         // Display countdown and session points
	IPDT_GOODLUCK_SESSION,          // Display good luck and session points as cash
	IPDT_GOODLUCK_SESSIONPTS,       // Display goodluck and session points.
	IPDT_GOOD_CARD,                 // Display the good card message
} InPlayDisplayType;

// Card In Display Types
typedef enum
{
	CIDT_TOTAL = 0,				// Display total points
	CIDT_TOTALCASH,				// Display total cash
	CIDT_NONE,					// Do not diplay an in-card message
} CardInPointDisplayType;

typedef enum
{
	PTT_CARD_IN = 0,
	PTT_CARD_OUT,
	PTT_VIRTUAL_SESSION_START,
	PTT_VIRTUAL_SESSION_STOP,
	PTT_UPDATE,
} PlayerTransactionType;

// Status for Centralized Wallet leftover fund transfer from other casino during PBT start.
typedef enum
{
	LEFTOVER_CASH_TRANSFER_NA,
	LEFTOVER_CASH_TRANSFER_SUCCEED,
	LEFTOVER_CASH_TRANSFER_FAILED
}LeftoverCashTransferStatus;

class CPlayer : public CDataSubscriptionProvider,
			    public CMemoryStorage, 
				public CInternalMsgContent<INT_MSG_CONTENT_PLAYER>
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CPlayer();

	/// <summary>
	/// Constructor using CConfig.
	/// Player constructor that builds itself from persistant memory (NVRAM)
	/// </summary>
	/// <param name="config">IN - The nCompass configuration.</param>
	/// <param name="egmConfig">IN - The EGM configuration.</param>
	CPlayer(const CConfig &config, const CEGMConfig& egmConfig);

	/// <summary>
	/// Constructor using CCard, CPlayerInfo, and CConfig.
	/// Player constructor that does not build itself from NVRAM.
	/// </summary>
	/// <param name="card">IN - The card for the player session. Caller MUST NOT access after call.</param>
	/// <param name="playerInfo">IN - The player info. Caller MUST NOT access after call.</param>
	/// <param name="config">IN - The nCompass configuration.</param>
	/// <param name="egmConfig">IN - The EGM configuration.</param>
	/// <param name="utcTime">IN - The UTC time.</param>
	/// <param name="goodbyeEnabled">IN - true if a goodbye button should be enabled for this player session, false otherwise.</param>
	CPlayer(CCard *card, CPlayerInfo *playerInfo, const CConfig &config, const CEGMConfig& egmConfig,
		time_t utcTime, bool goodbyeEnabled, CGames* games);

	/// <summary>
	/// Copy constructor.
	/// </summary>
	/// <param name="playerRHS">IN - The source for the copy construction.</param>
	/// <remarks>
	/// NOTE: This will NOT always make a non-persistant player object.
	/// NOTE: m_lastUpdateSentTime is set to a default value.
	/// </remarks>
	CPlayer( const CPlayer &playerRHS );

	/// <summary>
	/// Destructor.
	/// </summary>
	/// <remarks>
	/// This WILL remove session meters from the Persistent Store, if m_sessionStartMeters != NULL.
	/// </remarks>
	~CPlayer(void);

	/// <summary>
	/// Assignment operator.
	/// </summary>
	/// <param name="playerRHS">IN - The source for the assignment.</param>
	/// <returns>A reference to the target of the assigment.</returns>
	/// <remarks>
	/// This will NOT remove session meters from the Persistent Store.
	/// NOTE: this->m_lastUpdateSentTime is not changed.
	/// </remarks>
	CPlayer &operator=( const CPlayer &playerRHS );  

	/// <summary>
	/// Returns true if there is player data in NVRAM, false otherwise.
	/// </summary>
	/// <returns>true if there is player data in NVRAM, false otherwise.</returns>
	/// <remarks>
	/// NOTE: Keep this function static.
	/// </remarks>
	static bool DoesPlayerExistInNVRAM( void );

	/// <summary>
	/// Returns Marks Session meters in NVRAm as Inactive location.
	/// </summary>
	/// <remarks>
	/// NOTE: Keep this function static.
	/// </remarks>
	static void MarkSessionMetersInactiveInNVRAM(void);

	/// <summary>
	/// Notify the player object that the player card has been removed. 
	/// This will set the card out time and possibly make the player virtual.
	/// </summary>
	/// <param name="removalTime">IN - The local removal time.</param>
	/// <param name="utcRemovalTime">IN - The UTC removal time.</param>
	/// <param name="bMakeVirtual">IN - true makes the player session virtual, false leaves virtual status unchanged.</param>
	void CardRemoved( time_t const &removalTime, time_t const &utcRemovalTime, bool bMakeVirtual );

	/// <summary>
	/// End a virtual session on this player object.
	/// </summary>
	/// <param name="virtualEndTime">IN - the card out time for the virtual card instance.</param>
	void VirtualSessionEnd( time_t const &virtualEndTime );
	
	/// <summary>
	/// Update the last play time for the player in UTC time.
	/// </summary>
	/// <param name="playTime">IN - The play time.</param>
	void SetLastPlayTime( const time_t &playTime );

	// Set the player info object for the player
	/// <summary>
	/// Sets the player info.
	/// </summary>
	/// <param name="playerInfo">IN - The player info. Caller MUST NOT access after call.</param>
	void SetPlayerInfo( CPlayerInfo *playerInfo );

	/// <summary>
	/// Sets the player global balance.
	/// </summary>
	/// <param name="playerGlobalBal">IN - The player global bal. Caller MUST NOT access after call.</param>
	/// <remarks>
	/// It is okay if overflow occurs.
	/// </remarks>
	void SetPlayerGlobalBalance( const CPlayerGlobalBalance *globalBalance );

	/// <summary>
	/// Checks the multiplier and divider.  Saves the new multipler and/or divider if changed.
	/// </summary>
	/// <param name="fallbackMultiplier">IN - Multiplier will be set to this value if current Multiplier is 0.</param>
	/// <param name="fallbackDivider">IN - Divider will be set to this value if current Divider is 0.</param>
	void CheckMultiplierDivider( DWORD fallbackMultiplier, DWORD fallbackDivider );

	/// <summary>
	/// Set the bonus events object for this player.
	/// </summary>
	/// <param name="bonusEvents">IN - The bonus events.</param>
	void SetBonusEvents( const CBonusEvents *bonusEvents );

	/// <summary>
	/// Set all the settings in this player object from the provided config object.
	/// </summary>
	/// <param name="config">IN - The nCompass configuration.</param>
	/// <param name="egmConfig">IN - The EGM configuration.</param>
	void ConfigSettings(const CConfig &config, const CEGMConfig& egmConfig);

	/// <summary>
	/// Set the current countdown for the player.
	/// </summary>
	/// <param name="currentCountdown">IN - The current countdown.</param>
	void SetCurrentCountdown( DWORD currentCountdown );


	/// <summary>
	/// Decrement the players countdown by the specified amount, award points and bonus points.
	/// </summary>
	/// <param name="meters">IN - current meters.</param> 
	/// <param name="origMeters">IN - original meters.</param>
	/// <param name="isMGMDEnabled">IN: Is MGMD enabled</param> 
	/// <returns>true if bonus points were awarded, false otherwise.</returns>
	bool DecrementCountdown(CMeters *meters, CMeters *origMeters, bool isMGMDEnabled);

	/// <summary>
	/// Return a pointer to a single outstanding bonus event from the player's bonus events, so caller can modify the bonus event.
	/// Once this bonus event is "gotten" it will no longer be outstanding.
	/// </summary>
	/// <param name="bIsCardOutTime">IN - true to include multiplier events, false otherwise.</param>
	/// <returns>A pointer to an outstanding bonus event, NULL if there are no more outstanding events.</returns>
	CBonusEvent * GetOutstandingBonusEvent( bool bIsCardOutTime );

	/// <summary>
	/// Reset all the session data.
	/// We need to do this when a virtual session begins.
	/// </summary>
	/// <param name="utcTime">IN - The UTC time.</param>
	void ResetSession( time_t utcTime );

	/// <summary>
	/// Return the total of all session points, bonus points earned this session, and all prior session points.
	/// </summary>
	/// <returns>The total of all session points, bonus points earned this session, and all prior session points.</returns>
	/// <remarks>
	/// It is okay if overflow occurs.
	/// </remarks>
	DWORD GetTotalPoints( void ) const;

	/// <summary>
	/// Return the total of all session points, bonus points earned this session, and all prior session points as a string in $x.xx format.
	/// </summary>
	/// <returns>The total of all session points, bonus points earned this session, and all prior session points as a string in $x.xx format.</returns>
	std::string GetTotalPointsAsCashString( void ) const;	

	/// <summary>
	/// Return the total of all session points and bonus points earned this session as a string in $x.xx format.
	/// </summary>
	/// <returns>The total of all session points and bonus points earned this session as a string in $x.xx format.</returns>
	std::string GetSessionPointsAsCashString( void ) const;

	/// <summary>
	/// Return the play comps amount as a string in $x.xx format.
	/// </summary>
	/// <returns>The play comps amount as a string in $x.xx format.</returns>
	std::string GetCompsAsCashString( void ) const;

	/// <summary>
	/// To get the display string for a player in play.
	/// </summary>
	/// <param name="text">OUT - points display string.</param>
	/// <param name="args">OUT - arguments of the display string.</param>
	void GetInPlayText(std::string &text, std::vector<std::string> &args) const;

	/// <summary>
	/// To get the comp display string for a card in.
	/// </summary>
	/// <param name="text">OUT - points display string.</param>
	/// <param name="args">OUT - arguments of the display string.</param>
	void GetCardInCompSingleLineString(std::string &text, std::vector<std::string> &args) const;

	/// <summary>
	/// To get the points display string for a card in.
	/// </summary>
	/// <param name="text">OUT - points display string.</param>
	/// <param name="args">OUT - arguments of the display string.</param>
	void GetCardInPointsSingleLineString(std::string &text, std::vector<std::string> &args) const;

	/// <summary>
	/// Gets the tiered points as string.
	/// </summary>
	/// <returns>tiered points string</returns>
	std::string GetTieredPointsAsString(void) const;

	/// <summary>
	/// Returns true if displaying points on the balance screen is enabled, false otherwise.
	/// </summary>
	/// <returns>true if displaying points on the balance screen is enabled, false otherwise.</returns>
	/// <remarks>
	/// This is used by the UI to determine if it should display points on the account balance screen.
	/// TR# 26396
	/// </remarks>
	bool DisplayPointsOnBalanceScreen() const;

	/// <summary>
	/// Returns true if displaying the account balance button is enabled for this player session, false otherwise.
	/// </summary>
	/// <returns></returns>
	/// <remarks>
	/// This is used by the UI to determine if it should display the account balance button.
	/// TR# 26396
	/// </remarks>
	bool DisplayAccountBalanceButton() const;

	/// <summary>
	/// Returns true if this player currently has guest status, false otherwise.
	/// </summary>
	/// <returns>true if this player currently has guest status, false otherwise.</returns>
	bool IsGuest( void ) const;

	/// <summary>
	/// Returns the virtual flag value.
	/// </summary>
	/// <returns>The virtual flag value.</returns>
	/// <remarks>
	/// This flag is used by FloorLogix to determine
	///     whether to increment the card in count when it receives a card in transaction and
	///     whether to decrement the card in count when it receives a card out transaction
	/// for use in determining whether the maximum concurrent card ins for the player has been exceeded.	
	///
	/// The requirements for this method are:
	///     Return false after a physical card in.
	///     Return false after a physical card out if no virtual session is being created.
	///     Return true after a physical card out if virtual session is being created.
	///     Return true after a virtual card in.
	///     Return false after a virtual card out.
	/// </remarks>
	bool IsVirtual( void ) const;

	/// <summary>
	/// Gets the UTC session start time corresponding to the last session (physical or virtual) that ended.
	/// </summary>
	/// <returns>The last UTC session start time.</returns>
	/// <remarks>
	//  This value is intended for use by the PollerProtocol when creating card out transactions
	///     and is only valid after the (physical or virtual) session has ended.
	/// This value is not persisted.
	/// </remarks>
	time_t GetLastSessionStartTime(void) const;

	/// <summary>
	/// Sets whether the player was carded out when the employee carded in.
	/// </summary>
	/// <param name="cardedOutOnEmployeeCardIn">IN - true if the player was carded out when the employee carded in, false otherwise.</param>
	void SetCardedOutOnEmployeeCardIn(bool cardedOutOnEmployeeCardIn);

	/// <summary>
	/// Was the player carded out when the employee carded in.
	/// </summary>
	/// <returns>true if the player was carded out when the employee carded in, false otherwise.</returns>
	bool WasCardedOutOnEmployeeCardIn() const;

	/// Returns true if "card" is considered a match for the player session, false otherwise.
	/// </summary>
	/// <param name="card">IN - The card  to compare the player session against.</param>
	/// <returns></returns>
	bool CompareCard( const CCard *card ) const;

	/// <summary>
	/// Returns a pointer to the CCard instance for the player session.
	/// </summary>
	/// <returns>A pointer to the CCard instance for the player session.</returns>
	const CCard *GetCard( void ) const;

	/// <summary>
	/// Returns a pointer to the current player name for this player session.
	/// </summary>
	/// <returns>A pointer to the current player name for this player session.</returns>
	std::string GetName( void ) const;

	/// <summary>
	/// Returns a pointer to const for the CPlayerInfo instance for this player session.
	/// The pointer may be NULL.
	/// </summary>
	/// <returns>A pointer to const for the CPlayerInfo instance for this player session. The pointer may be NULL.</returns>
	const CPlayerInfo *GetPlayerInfo(void) const;

	/// <summary>
	/// Returns a pointer to the CPlayerInfo instance for this player session, so the caller can modify player information directly.
	/// The pointer may be NULL.
	/// </summary>
	/// <returns>A pointer to the CPlayerInfo instance for this player session. The pointer may be NULL.</returns>
	CPlayerInfo *GetPlayerInfo( void );

	/// <summary>
	/// Gets the bonus points awarded so far for this player session.
	/// </summary>
	/// <returns></returns>
	DWORD GetBonusPoints( void ) const;

	/// <summary>
	/// Returns a pointer to the virtual CCard instance for the player session.
	/// </summary>
	/// <returns>A pointer to the virtual CCard instance for the player session.</returns>
	/// <remarks>
	/// The virtual CCard instance can be set non-NULL when a session is made virtual.
	/// </remarks>
	const CCard *GetVirtualCard( void ) const;

	/// <summary>
	/// Returns the time set for the last game play for this player session.
	/// </summary>
	/// <returns>The time set for the last game play for this player session.</returns>
	/// <remarks>
	/// Represents the time that the game was last played by this player.
	/// This should be set in UTC, and so returns UTC.
	/// </remarks>
	time_t GetLastPlayTime( void ) const;

	/// <summary>
	/// Returns the virtual start time.
	/// </summary>
	/// <returns>The virtual start time.</returns>
	/// <remarks>
	/// The virtual start time is set when a session is made virtual.
	/// </remarks>
	time_t GetVirtualStartTime( void ) const;

	/// <summary>
	/// Returns the current countdown for this player session.
	/// </summary>
	/// <returns>The current countdown for this player session.</returns>
	DWORD GetCurrentCountdown( void ) const;

	/// <summary>
	/// Returns the current session points for this player session.
	/// </summary>
	/// <returns>The current session points for this player session.</returns>
	DWORD GetSessionPoints( void ) const;

	/// <summary>
	/// Returns true if points have been added for this player session, false otherwise.
	/// </summary>
	/// <returns>true if points have been added for this player session, false otherwise.</returns>
	bool HasPointsBeenAdded( void ) const;

	/// <summary>
	/// Determines whether [is PB enabled].
	/// Returns true if both pb is enabled globally and for the player, false otherwise.
	/// </summary>
	/// <returns>true if both pb is enabled globally and for the player, false otherwise.</returns>
	bool IsPBEnabled( void ) const;

	/// <summary>
	/// Returns true if we go to the PB screen on card in, false otherwise.
	/// </summary>
	/// <returns>true if we go to the PB screen on card in, false otherwise.</returns>
	bool DoPBOnCardIn( void ) const;

	/// <summary>
	/// Returns true if player is banned from playing.
	/// Returns false otherwise.
	/// </summary>
	/// <returns>true if the player is banned from play, false otherwise.</returns>
	bool IsBannedFromPlay( void ) const;
	
	/// <summary>
	/// Returns the in-play display type.
	/// </summary>
	/// <returns>The in-play display type.</returns>
	InPlayDisplayType GetInPlayDisplayType( void ) const;

	/// <summary>
	/// Set whether loss limit support is enabled.
	/// </summary>
	/// <param name="enabled">IN - true to enable loss limit support, false to disable.</param>
	void SetLossLimitEnabled(bool enabled);

	/// <summary>
	/// Returns whether it is anniversary today or not.
	/// </summary>
	/// <returns>True if it is anniversary today, otherwise returns false.</returns>
	bool IsAnniversaryToday() const;

	/// <summary>
	/// Returns whether it is birthday today or not.
	/// </summary>
	/// <returns>True if it is birthday today, otherwise returns false.</returns>
	bool IsBirthdayToday() const;

	/// <summary>
	/// Get the formatted welcome string.
	/// </summary>
	/// <param name="text">OUT - the formatted text.</param>
	/// <param name="argList">OUT - list of arguments for formatted text.</param>
	/// <remarks>
	/// If there is no welcome text, an empty text is returned.
	/// </remarks>
	void GetFormattedWelcome(std::string &text, std::vector<std::string> &argList) const;

	/// <summary>
	/// Returns a pointer to the good card inserted text.
	/// </summary>
	/// <param name="inPlayText">IN - true to get in-play text, false otherwise.</param>
	/// <returns>A pointer to the good card inserted text.</returns>
	std::string GetGoodCardText(bool inPlayText) const;

	/// <summary>
	/// Returns text to display if a Balance Retrieval formatted for use
	/// and providing the default string if not set in CConfig.
	/// </summary>
	/// <returns>Balance Retrieval text string.</returns>
	std::string GetFormattedBalanceRetrievalText( void ) const;
	
	/// <summary>
	/// Gets the formatted tier points label.
	/// </summary>
	/// <returns>tier points label string</returns>
	std::string GetFormattedTierPointsLabel(void) const;

	/// <summary>
	/// Get the formatted daily points text.
	/// </summary>
	/// <param name="text">OUT - the formatted text.</param>
	/// <param name="argList">OUT - list of arguments for formatted text.</param>
	/// <remarks>
	/// Added for Tracker 26984.
	/// </remarks>
	void GetFormattedDailyPointsText(std::string &text, std::vector<std::string> &argList) const;

	/// <summary>
	/// Gets the formatted text for points needed to reach the next tier.
	/// </summary>
	/// <param name="text">OUT - the formatted text.</param>
	/// <param name="argList">OUT - list of arguments for formatted text.</param>
	/// <remarks>
	/// Added for Tracker 26928.
	/// </remarks>
	void GetFormattedPointsToNextTierText(std::string &text, std::vector<std::string> &argList) const;

	/// <summary>
	/// Set the snapshot of the meters for the start of the player session.
	/// </summary>
	/// <param name="meters">IN - The meter snapshot. Caller MUST NOT access after call.</param>
	/// <remarks>
	/// If meters is NULL then nothing is done.
	/// </remarks>
	void SetSessionStartMeters( CMeters *meters );

	/// <summary>
	/// Set the snapshot of the meters for the start and flush of the player session.
	/// </summary>
	/// <param name="meters">IN - The meter snapshot. Caller MUST NOT access after call.</param>
	/// <remarks>
	/// If meters is NULL then nothing is done.
	/// </remarks>
	void SetSessionStartMetersCurrent(CMeters *meters);

	/// <summary>
	/// Reset the snapshot of the meters for the current player session.
	/// </summary>
	/// <param>IN - void.</param>
	/// <remarks>	
	/// </remarks>
	void ResetCurrentSessionData();

	/// <summary>
	/// Returns a pointer to a copy of the session start meters.
	/// </summary>
	/// <returns>A pointer to a copy of the session start meters.</returns>
	/// <remarks>
	/// The caller is responsible for deleting the returned copy.
	/// </remarks>
	CMeters *GetSessionStartMetersCopy( void ) const;

	/// <summary>
	/// Resets the last update sent time using the value provided.
	/// </summary>
	/// <param name="utcTime">IN - The new UTC time.</param>
	void ResetLastUpdateSentTime( time_t utcTime );

	/// <summary>
	/// Returns the last update sent time.
	/// </summary>
	/// <returns>The last update sent time.</returns>
	time_t GetLastUpdateSentTime( void ) const;

	/// <summary>
	/// Gets whether a goodbye button should be enabled for this player session.
	/// </summary>
	/// <returns>true if a goodbye button should be enabled, false otherwise.</returns>
	bool IsGoodbyeEnabled() const;

	/// <summary>
	/// Returns the current loss limit session balance.
	/// </summary>
	/// <returns>The current loss limit session balance.</returns>
	WORD GetLossLimitSessionBalance() const;

	/// <summary>
	/// Sets the loss limit session balance using the value provided.
	/// </summary>
	/// <param name="newBalance">IN - The new balance.</param>
	void SetLossLimitSessionBalance(WORD newBalance);

	/// <summary>
	/// Returns true if the has played status has been set true, false otherwise.
	/// </summary>
	/// <returns>true if has played has been set true, false otherwise.</returns>
	bool GetHasPlayed() const;

	/// <summary>
	/// Sets the has played status to true.
	/// </summary>
	void SetHasPlayed();

	/// <summary>
	/// Returns true if the max card limit for the player was reached, otherwise returns false.
	/// </summary>
	/// <returns>true if the max card limit for the player was reached, otherwise returns false.</returns>
	bool IsMaxCardLimitReached() const;

	/// <summary>
	/// Sets the virtual flag to the value provided.
	/// </summary>
	/// <param name="virtualFlag">IN - The new virtual flag value.</param>
	/// <remarks>
	/// See IsVirtual() for details.
	/// </remarks>
	void SetVirtualFlag(bool virtualFlag);

	/// <summary>
	/// Determines whether the rank is valid.
	/// </summary>
	/// <returns>true if the rank is valid</returns>
	bool IsRankValid() const;

	/// <summary>
	/// Returns the text for maximum card limit reached.
	/// </summary>
	/// <returns></returns>
	/// <remarks>
	/// Added for Tracker 026984
	/// This returns the text used by the UI when the max card limit has been reached.
	/// </remarks>
	std::string GetMaxCardLimitText() const;

	/// <summary>
	/// Returns the player points due to the tier multiplier.
	/// </summary>
	/// <returns>The player points due to the tier multiplier.</returns>
	/// <remarks>
	/// It is okay if overflow occurs.
	/// </remarks>
	DWORD GetTierMultiplierPoints() const;

	/// <summary>
	/// Gets the player rank identifier.
	/// </summary>
	/// <returns>The rank identifier.</returns>
	DWORD GetRankId() const;

	/// <summary>
	/// Determines whether is universe enabled.
	/// </summary>
	/// <returns>true if universe is enabled, false otherwise.</returns>
	bool IsUniverseEnabled() const;

	/// <summary>
	/// Determines whether the profile profile button is enabled.
	/// </summary>
	/// <returns>true to displayer profile button, false otherwise.</returns>
	bool DisplayProfileButton() const;

	/// <summary>
	/// Sets the subgame session parameters when game/denom is changed.
	/// </summary>
	/// <param name="params">The subgame session parameters to be set with.</param>
	/// <returns>True if old SubgameSession was closed and new was created  succesfully otherwise false .</returns>
	bool SetGameSession (SubgameSessionParameters &params);

	/// <summary>
	/// Apply the subgame selection response received from poller 
	//  after changing game/denom.
	/// </summary>
	/// <param name="response">The subgame selection response to be set with.</param>
	/// <returns>true if subgame params could be applied successfully otherwise false.</returns>
	bool ApplySubgameSelectionResponse(SubgameSelectionResponse *response);

	/// <summary>
	/// Game start event.
	/// </summary>
	/// <returns>True if new game session started else false.</returns>
	bool GameStart();

	// Set the player info object for the player and the starting Multiplier, Divider and  countdown on card in.
	/// <summary>
	/// Sets the player info.
	/// </summary>
	/// <param name="playerInfo">IN - The player info. Caller MUST NOT access after call.</param>
	/// <param name="fallbackMultiplier">IN - Multiplier will be set to this value if current Multiplier is 0.</param>
	/// <param name="fallbackDivider">IN - Divider will be set to this value if current Divider is 0.</param>
	/// <param name="isMGMDEnabled">IN - Is MGMD enabled
	/// after call.</param> 
	/// <param name="denom">IN - denom</param> 
	/// <param name="gameNumber">IN - Game Number</param> 
	/// <param name="fallbackAccountingType">IN - Fallback 
	/// accounting type to use when MGMD not available</param> 
	/// <param name="fallbackNPFPe">IN - Fallback 
	/// no points for promo value to use when MGMD not 
	/// available.</param>
	void SetPlayerInfoAndMultiplierDividerCountdown( CPlayerInfo *playerInfo, DWORD fallbackMultiplier, DWORD fallbackDivider, bool isMGMDEnabled, int denom, DWORD gameNumber, AccountingType fallbackAccountingType, bool fallbackNPFP);

	/// <summary>
	/// Set the starting Multiplier, Divider and  countdown on card in
	/// </summary>
	/// <returns>None.</returns>
	void SetStartingMultiplierDividerCountdown();
	
	/// <summary>
	/// Get active multiplier and divider.
	/// </summary>
	/// <param name="activeMultiplier"> OUT: Multiplier from active subgame session else starting multiplier</param>
	/// <param name="activeDivider"> OUT: Divider from active subgame session else starting divider</param>
	/// <returns>None.</returns>	
	void GetActiveMultiplierDivider (DWORD &activeMultiplier, DWORD &activeDivider);

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

	/// <summary>
	/// Get the total of all session points and bonus points earned this session.
	/// </summary>
	/// <returns>The total of all session points and bonus points earned this session.</returns>
	/// <remarks>
	/// It is okay if overflow occurs.
	/// </remarks>
	DWORD GetSessionAndBonusPoints( void ) const;
	
	/// <summary>
	/// Set flag if player is in mobile session.
	/// </summary>
	void SetMobilePlayer();
	
	/// <summary>
	/// Get if player is in mobile session.
	/// </summary>
	/// <returns>true:if player is in mobile session .</returns>
	bool IsMobilePlayer() const;

	/// <summary>
	/// Gets the extended buckets available flag.
	/// </summary>
	/// <returns>True if extended buckets available. Otherwise false.</returns>
	bool GetExtendedBucketsAvailable() const;

	/// <summary>
	/// Updates the current countdown index for extended buckets.
	/// </summary>
	void MoveToNextBucket();

	/// <summary>
	/// Return the number of extended buckets.
	/// </summary>
	/// <returns>The number of extended buckets available.</returns>
	WORD GetBucketCount() const;

	/// <summary>
	/// Get current bucket details from the bucket-level tracking list.
	/// </summary>
	/// <param name="name"> OUT: current bucket name</param>
	/// <param name="currency"> OUT: current bucket currency</param>
	/// <param name="countdown"> OUT: current bucket countdown</param>
	/// <param name="sessionPoints"> OUT: current bucket session points</param>
	/// <returns>True on success. Otherwise returns false.</returns>	
	bool GetCurrentBucketDetails(std::string &name, std::string &currency, DWORD &countdown, DWORD &sessionPoints) const;

	/// <summary>
	/// This function resolves the display name of legacy bucket. 
	/// It gets the display name for given bucket id, if it is a legacy bucket.
	/// </summary>
	/// <param name="id"> IN: bucket id</param>
	/// <param name="name"> OUT: bucket name</param>
	void GetBucketDisplayName(DWORD id, std::string &name) const;

	/// <summary>
	/// Gets the currently valid buckets display information from CSessionPoints instance.
	/// </summary>
	/// <param name="data"> OUT: bucket display info</param>
	/// <param name="gameNumber"> OUT: game number of display information</param>
	/// <param name="denom"> OUT: denom of diaplay information</param>
	void GetBucketsDisplayData(std::vector<std::unique_ptr<BucketDisplayData>> &data, DWORD &gameNumber, int &denom) const;

	/// <summary>
	/// Sets the leftover cash transfer status during PBT for wallet 2.0.
	/// </summary>
	void SetLeftoverCashTransferStatus(LeftoverCashTransferStatus status);

	/// <summary>
	/// Gets the leftover cash transfer status during PBT for wallet 2.0.
	/// </summary>
	/// <returns>LeftoverCashTransferStatus.</returns>
	LeftoverCashTransferStatus GetLeftoverCashTransferStatus() const;

	/// <summary>
	/// Returns whether to display player name or not as per configuration.
	/// </summary>
	/// <returns>True if player name needs to be displayed. Otherwise return false.</returns>	
	bool IsDisplayPlayerName() const;

	/// <summary>
	/// Converts player transaction type to internal message yype.
	/// </summary>
	/// <param name="type">player transaction type to convert</param>
	/// <returns>internal message type.</returns>
	DWORD ConvertPlayerTxTypeToInternalMsgType(PlayerTransactionType type) const;

	/// <summary>
	/// Returns the base points earned.
	/// </summary>
	/// <returns>The base points earned.</returns>
	DWORD GetBasepointsEarned(void);

private:
	static const DWORD DEFAULT_POINTS_PER_DOLLAR = 1;
	static const size_t PARTIAL_BONUSING_REMAINDER_SIZE = sizeof(DWORD);

	void Init();
	void CopyObj(const CPlayer &playerRHS);
	void CleanUp();

	virtual void BuildYourself();

	void SetPlayerExistanceToNVRAM( bool bPlayerExists );

	void MakeVirtual( time_t const &virtualStartTime, time_t const &utcVirtualStartTime );

	DWORD GetPointsAsCash(DWORD points) const;
	DWORD GetTotalPointsAsCash( void ) const;	
	DWORD GetSessionPointsAsCash( void ) const;
	std::string GetFormattedNameText(const std::string &nameSuffixText, const std::string &noNameText) const;
	std::string GetFormattedString(const std::string &noNameStr, const std::string &appendStr) const;

	// Player card
	CCard *m_card;				
	void StoreCardDataToNVRAM( void );

	// Static player info (from poller player data)
	CPlayerInfo *m_playerInfo;
	void StorePlayerInfoDataToNVRAM( void );
	
	// Time the poller was last played
	time_t m_lastPlayTime;		
	void StoreLastPlayTimeToNVRAM( void );

	// Session data
	CSessionPoints m_sessionInfo;
	bool m_bPointsAdded;
	WORD m_lossLimitSessionBalance;
	bool m_hasPlayed;
	bool m_mobilePlayer;
	void StoreSessionDataToNVRAM( void );

	// Virtual session data
	bool m_bIsVirtual;			
	time_t m_virtualStartTime;
	CCard *m_virtualCard;
	void StoreVirtualSessionDataToNVRAM( void );
	bool m_cardedOutOnEmployeeCardIn; // Not persisted.

	void SetBonusExistanceToNVRAM( void );


	// The last time the player update was sent.
	// NOTE: This does not need to be stored in NVRAM;
	time_t m_lastUpdateSentTime;

	// Goodbye enabled should not be stored in NVRAM.
	bool m_isGoodbyeEnabled;

	time_t m_lastSessionStartTime; // Does not need to be stored in NVRAM.

	// These are config items. 
	//  NOTE: They do not need to be stored to NVRAM because config already is.
	bool m_bPBEnabled;
	bool m_bPBOnCardIn;			
	bool m_bPartialBonusing;
	DWORD m_pointsPerDollar;
	bool m_bDisplayName;
	InPlayDisplayType m_inPlayDisplayTypeConfig; // From CConfig only. Use GetInPlayDisplayType() to get full value.
	CardInPointDisplayType m_cardInPointsDisplayType;
	bool m_showCardInCompDisplay;
	CNotificationTextConfig m_notificationTextConfig;
	std::string m_tierPointsLabel;
	bool m_displayCongrats;
	bool m_lossLimitEnabled;
	bool m_showPointsVsAccountBalance;
	bool m_universeEnabled;
	bool m_displayProfileButton;
	bool m_pointsCountdownControlByUniverse;
	CGames* m_games;

	string ResponseForPlayerSessionFullSession();
	string ResponseForPlayerSessionUnFlushedSession();
	string ResponseForPlayerSessionExtFullSession();
	string ResponseForPlayerSessionExtUnFlushedSession();
	string ResponseForPlayerInfo();

	__int64 GetMeterDeltaUnflushed(byte meterNumber);
	__int64 GetMeterDeltafull(byte meterNumber);
	__int64 GetCurrentMeterValue(byte meterNumber);
	bool m_isMGMDEnabled;

	DWORD m_lastSessionPoints;

	LeftoverCashTransferStatus m_LeftoverCashTransferStatus;

	friend SentinelNativeUnitTests::ProgressiveMessageFactoryUnitTests;
	friend SentinelNativeUnitTests::Player7;
};
