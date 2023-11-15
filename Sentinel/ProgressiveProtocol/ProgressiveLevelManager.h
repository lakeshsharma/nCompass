#pragma once

#include "ProgressiveBonusingAvailable.h"
#include "ProgressiveLevelForward.h"
#include "ProgressiveLevelBroadcast.h"
#include "ProgressiveLevel.h"
#include "ProgressiveLevelsInfo.h"
#include "DataSubscriptionProvider.h"
#include "ProgressiveEligibleLevelsInfo.h"
#include "InternalMessage.h"
#include "ProgressiveWinnerPicked.h"
#include "UI/BonusCounterState.h"


namespace SentinelNativeUnitTests 
{ 
	class WebDiagnostics_Progressive;
	class EGMAwardManager;
	class ProgressiveLevelTests;
	class ProgressiveLevelForwardTests;
	class ProgressiveLevelBroadcastTests;
	class ProgressiveLevelManagerTests;
	class ProgressiveLevelManagerTests2;
	namespace ProgressiveLevelTestHelpers
	{ 
		class _ICardConfig5;
		struct LevelData;
	}
}

class ProgressiveLevelManager : public CDataSubscriptionProvider
{
protected:
	std::map<int64_t, std::shared_ptr<CProgressiveLevel>> m_levels;
	std::map<uint8_t, std::shared_ptr<CProgressiveLevel>> m_levelsBySAS;
	std::map<int64_t, std::shared_ptr<CProgressiveLevel>>::iterator m_levelsItr;

	std::string m_eligibleLevelsJSONString;

	SYSTEMTIME m_receivedTime = DEFAULT_SYSTEMTIME;	// This is used only for logging and diagnostics
	DWORD m_updatedTickCount = 0;
	DWORD m_forwardedTickCount = 0;
	int   m_progressiveGroupId = 0;
	bool  m_useAftLockOnTransfers = false;
	bool  m_queryPokerCards = false;
	bool  m_isUpdated = false;
	bool  m_needPriorityForward = false; // Progressive level amount went backwards (etc.).  Need to forward levels to EGM quickly.

	SYSTEMTIME m_eligibilityReceivedTime = DEFAULT_SYSTEMTIME;
	int  m_eligibilitySessionId = 0;
	bool m_eligibilityDetermined = false;
	volatile bool m_sessionActive = false; // true if there is an (carded or uncarded) player session active, false otherwise.
	volatile int m_sessionId = 0; // The (carded or uncarded) player session identifier.
	volatile bool m_multicastActive = DefaultMulticastActive; // true when nCompass is receiving multicast messages adequately, false otherwise.

	// Limit priority forwards during window. After timeout, it is okay to do more priority forwards.
	static const DWORD  PriorityForwardWindowTimeout = 2000; // Tick Count timeout for elements in the priority forward window.
	static const size_t PriorityForwardWindowMaxUpdates = 5;    // Maximum number forwards tracked during a priority forward window.
	std::vector<DWORD>  m_lastForwardedTickCounts;			    // Oldest value at the back of the array.

	CountHistory m_downCount;

	int64_t m_lastLevelIdSubscriptionProvided = 0;

	mutable ::std::recursive_mutex m_criticalSection;

	BonusCounterState::CBonusCounterState m_bonusCounterState;

private:
	void SetForwardedTickCount(DWORD tickCount);

	///	<summary>
	/// Checks if a priority update is needed.
	///	</summary>
	///	<param name="tickCount">IN - The current tick count.</param>
	///	<returns>true if a priority update is needed, false otherwise.</returns>
	bool OkToSendPriorityForward(DWORD tickCount) const;

	void GetLevelForward(DWORD tickCount, CProgressiveLevelForward* pProgressiveLevelForward, const SYSTEMTIME* pLocalTime = nullptr);
	CProgressiveLevelForward* GetLevelForward(DWORD tickCount, const SYSTEMTIME* pLocalTime = nullptr);

	void AddLevel(std::shared_ptr<CProgressiveLevel>& level);

	CInternalMessage *CheckAnticipation(const shared_ptr<CProgressiveLevel> level);
	CInternalMessage *CheckEndCountdown(const shared_ptr<CProgressiveLevel> level);
	CInternalMessage *CheckPteResult(const shared_ptr<CProgressiveLevel> level);
	CInternalMessage *CheckPteLevelUp(const shared_ptr<CProgressiveLevel> level);
	CInternalMessage *CheckPteLeaderboard(const shared_ptr<CProgressiveLevel> level);

	void CopyObj(const ProgressiveLevelManager &rhs);

public:
	///	<summary>
	/// ProgressiveLevelManager constructor.
	///	</summary>
	ProgressiveLevelManager();

	///	<summary>
	/// ProgressiveLevelManager copy constructor.
	///	</summary>
	///	<param name="other">A reference to the ProgressiveLevelManager instance to copy.</param>
	ProgressiveLevelManager(const ProgressiveLevelManager& other);

	///	<summary>
	/// Assignment operator.
	///	</summary>
	///	<param name="other">A reference to the ProgressiveLevelManager instance to copy.</param>
	///	<returns>A reference to this instance.</returns>
	/// <remarks>
	/// Only the source instance "other" is locked, to avoid potential deadlocks
	/// when source and target "this" instances are locked in different orders by different threads.
	/// </remarks>
	ProgressiveLevelManager& operator=(const ProgressiveLevelManager& other);

	///	<summary>
	/// Processes a progressive level broadcast.
	///	</summary>
	///	<param name="pLevelBroadcast">A pointer to a CProgressiveLevelBroadcast instance.</param>
	///	<returns><b>true</b> if a priority forward is needed because of this broadcast, else <b>false</b>.</returns>
	bool ProcessLevelBroadcast(const CProgressiveLevelBroadcast* pLevelBroadcast);

	///	<summary>
	/// Processes progressive bonusing available information.
	///	</summary>
	///	<param name="pProgressiveBonusingAvailable">The progressive bonusing available information.</param>
	///	<param name="progressiveLevelsInfo">The eligible levels information.</param>
	/// ///	<returns>levels that expired</returns>
	void ProcessBonusingAvailable(const CProgressiveBonusingAvailable* pProgressiveBonusingAvailable, CProgressiveEligibleLevelsInfo *progressiveLevelsInfo);

	/// <summary>
	/// Returns the level ID associated with the SAS level.
	/// </summary>
	/// <param name="sasLevelId">The SAS level.</param>
	///	<returns>The level ID associated with the SAS level, else 
	///		InvalidProgressiveLevelId sasLevelId is invalid or there is no level
	///		associated with this SAS level.</returns>
	int64_t GetLevelIdFromSASLevel(int sasLevel) const;

	///	<summary>
	/// Updates the levels.
	///	</summary>
	///	<param name="pLevelsInfo">The ProgressiveLevelsInfo.</param>
	void UpdateLevels(const ProgressiveLevelsInfo* pLevelsInfo);

	///	<summary>
	/// Returns the progressive group ID.
	///	</summary>
	///	<returns>The progressive group ID.</returns>
	int GetProgressiveGroupId() const;

	///	<summary>
	/// Indicates whether to use AFT lock on transfers.
	///	</summary>
	///	<returns><b>true</b> if AFT lock are to be used on transfers, else <b>false</b>.</returns>
	bool UseAftLockOnTransfers() const;

	///	<summary>
	/// Logs the data in this instance.
	///	</summary>
	///	<param name="isNew">If <b>true</b> prepends "New " to log string, else "Current ".</param>
	void Log(bool isNew) const;

	///	<summary>
	/// Returns the strings reflecting the contents of this instance.
	///	</summary>
	///	<param name="isNew">Indicates whether this is a new configuration or
	///		an existing one is being logged.</param>
	///	<param name="logStrings">The array to receive the strings.</param>
	void GetLogStrings(bool isNew, std::vector<std::string>& logStrings) const;

	///	<summary>
	///	Formats the web diagnostic html string.
	///	</summary>
	///	<param name="tickCount">The current tick count.</param>
	///	<param name="localTime">The LOCAL time.</param>
	///	<returns>The web diagnostic html string.</returns>
	std::string GetDiagnosticsHtml(DWORD tickCount, const SYSTEMTIME& localTime) const;

	///	<summary>
	///	Formats the down info web diagnostic html string.
	///	</summary>
	///	<param name="localTime">The LOCAL time.</param>
	///	<returns>The web diagnostic html string.</returns>
	std::string GetDownInfoDiagnosticsHtml(const SYSTEMTIME& localTime) const;

	///	<summary>
	///	Returns the applicable progressive level updates.
	///	</summary>
	///	<param name="tickCount">The current tick count.</param>
	///	<param name="progressiveProcessingIsDown"><b>true</b> if progressive processing is down, else <b>false</b>.</param>
	///	<param name="multicastActiveChanged"><b>true</b> if multicast active state changed, else <b>false</b>.</param>
	///	<param name="pLocalTime">Optional pointer to the LOCAL time.</param>
	///	<returns>An instance of CProgressiveLevelForward with the applicable updates.</returns>
	/// <remarks>
	/// For multicastActive, specifically:
	/// - Set to true when nCompass is receiving all configured all Standard and Bonus levels adequately.
	/// - Set to false when one or more configured Standard or Bonus levels are Stale.
	///     - I.e., false indicates a problem that needs to be addressed in order to achieve adequate multicast performance.
	/// - For compatibility, multicastActive is set true when no Standard levels and no Bonus levels are configured.
	/// - The name "multicastActive" is used to match the ICardStatus field with the same meaning.
	/// ---
	/// Returns a nullptr if there are no updates.
	/// Returns an empty list to indicate an update with no active levels.
	/// Currently, at least CProgressiveLevelForward::MinInterval elapses between returning an empty list and the next update.
	/// </remarks>
	CProgressiveLevelForward* GetLevelForward(DWORD tickCount, bool progressiveProcessingIsDown, bool& multicastActiveChanged, const SYSTEMTIME* pLocalTime = nullptr);

	///	<summary>
	///	Returns the eligibility session identifier.
	///	</summary>
	/// <returns>The eligibility session identifier.</returns>
	int GetEligibilitySessionId() const;

	///	<summary>
	/// Indicates whether any levels are of a particular type.
	///	</summary>
	///	<param name="type">The ProgressiveLevelType type.</param>
	///	<returns><b>true</b> if any levels are the passed type, else <b>false</b>.</returns>
	bool HasLevelType(ProgressiveLevelType::Type type) const;

	/// <summary>
	/// Checks if poker hand should be queried.
	/// </summary>
	/// <returns><b>true</b> if poker hand should be polled, else <b>false</b>.</returns>
	bool QueryPokerCards() const;

	///	<summary>
	///	Indicates whether the level ID from a winner pick message is valid.
	///	</summary>
	///	<param name="levelId">The level ID to check.</param>
	/// <returns><b>true</b> if the level ID from a winner pick message is valid,
	///		else <b>false</b>.</returns>
	bool IsWinnerPickValid(int64_t levelId) const;

	///	<summary>
	/// Set progressive down count.
	///	</summary>
	///	<param name="progressiveDown"><b>true</b> if progressive is down, else <b>false<b>.</param>
	void SetProgressiveDown(bool progressiveDown);

	///	<summary>
	///	Set session active info.
	///	</summary>
	///	<param name="sessionActive"><b>true</b> if session is acctive, else <b>false<b>.</param>
	///	<param name="sessionId">The session id.</param>
	void SetSessionActiveInfo(bool sessionActive, int sessionId);

	///	<summary>
	///	Indicates whether multicast is active.
	///	</summary>
	/// <returns><b>true</b> if multicast is active, else <b>false</b>.</returns>
	bool GetMulticastActive() const;

	///	<summary>
	///	Indicates if eligibility has been determined.
	///	</summary>
	/// <returns><b>true</b> if eligibility has been determined, else <b>false</b>.</returns>
	bool GetEligibilityDetermined() const;
	
	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

	/// <summary>
	/// Gets the next progressive level display item.
	/// </summary>
	/// <returns>Message to send to UI</returns>
	CInternalMessage *GetNextProgressiveLevelDisplayItem();

	/// <summary>
	/// Returns a display message if winner pick is a PTE award.
	/// </summary>
	/// <param name="winner">The winner.</param>
	/// <returns>CInternalMessage object to send to internals.</returns>
	CInternalMessage *GetPteAwardDisplayItem(CProgressiveWinnerPicked *winner);

	///	<summary>
	///	Indicates that the leaderboard was manually initiated by the player.
	///	</summary>
	///	<param name="levelId">The level ID of the leaderboard.</param>
	void LeaderboardManuallyInitiated(int64_t levelId);

	///	<summary>
	///	Returns the prize description for the level.
	///	</summary>
	///	<param name="levelId">The level ID to retrieve prize description for.</param>
	std::string GetPrizeDescription(int64_t levelId);

	///	<summary>
	/// Progressive connection went down remove progressive levels
	///	</summary>
	void ProgressiveDownClearLevels();

	/// <summary>
	/// Timed events check.
	/// </summary>
	/// <param name="timer">The timer.</param>
	void CheckTimedEvents(const time_t& timer);

	/// <summary>
	/// Updates the level counters based upon ght elevel forward list.
	/// </summary>
	/// <param name="msg">The MSG that contains the level forward list.</param>
	void UpdateLevelCounters(CProgressiveLevelForward* pLevels);
	
	/// <summary>
	/// Deletes the carded bonus counter items.
	/// </summary>
	void DeleteCardedBonusCounterItems();

	/// <summary>
	/// Sets the UI configuration object.
	/// </summary>
	void SetUIConfig(const CUIConfig& uiConfig);

	/// <summary>
	/// Removes a level.
	/// </summary>
	/// <param name="levelId">level id to remove.</param>
	void RemoveLevel(int64_t levelId);

private:
	friend SentinelNativeUnitTests::WebDiagnostics_Progressive;
	friend SentinelNativeUnitTests::EGMAwardManager;	
	friend SentinelNativeUnitTests::ProgressiveLevelTests;
	friend SentinelNativeUnitTests::ProgressiveLevelForwardTests;
	friend SentinelNativeUnitTests::ProgressiveLevelBroadcastTests;
	friend SentinelNativeUnitTests::ProgressiveLevelManagerTests;
	friend SentinelNativeUnitTests::ProgressiveLevelManagerTests2;
	friend SentinelNativeUnitTests::ProgressiveLevelTestHelpers::_ICardConfig5;
	friend SentinelNativeUnitTests::ProgressiveLevelTestHelpers::LevelData;
};
