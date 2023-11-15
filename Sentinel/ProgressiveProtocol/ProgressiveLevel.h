#pragma once

#include "ProgressiveLevelBroadcast.h"
#include "ProgressiveLevelForward.h"
#include "ProgressiveLevelInfo.h"
#include "ProgressiveEligibleLevelInfo.h"
#include "ProgressiveBonusingAvailable.h"
#include "ProgressivePteState.h"
#include "CountHistory.h"
#include "InternalMessage.h"
#include "SlotsContent.h"
#include <stdafx.h>

class CountHistory;
class ProgressiveLevelManager;
namespace SentinelNativeUnitTests 
{
	class StandardProgressiveWinsTests;
	class WebDiagnostics_Progressive;
	class ProgressiveLevelTests;
	class ProgressiveLevelForwardTests;
	class ProgressiveLevelBroadcastTests;
	class ProgressiveLevelManagerTests;
	class ProgressiveLevelManagerTests2;
	namespace ProgressiveLevelTestHelpers
	{ 
		class _ICardConfig5; 
		struct LevelData;
		struct TestData_ProcessForward;
	}
}

class CProgressiveLevel
{
protected:
	int64_t m_id = 0;
	int64_t m_value = 0;
	ProgressiveLevelType::Type m_type = ProgressiveLevelType::Standard;
	DWORD m_updatedTickCount = 0;
	std::string m_name;
	std::string m_url;
	std::vector<BYTE> m_sasLevels;

	enum State
	{
		Configured,	// A broadcast has never been received
		Updated,	// Level has received a broadcast
		Forwarded,	// Level has been forwarded
		Stale		// Level hasn't received a broadcast within MaxInterval
	};
	State m_state = Configured;
	CountHistory m_staleCount;
    ProgressivePteState m_pteState;
    std::unique_ptr<ProgressiveEligibleLevelInfo> m_eligibleInfo;
	static const size_t StaleCountDays = 2;

public:
	enum StaleState
	{
		IsStale,
		GoneStale,
		NoLongerStale,
	};

public:
	///	<summary>
	/// CProgressiveLevel constructor.
	///	</summary>
	///	<param name="levelInfo">The level info.</param>
	///	<param name="updatedTickCount">The updated tick count.</param>
	CProgressiveLevel(ProgressiveLevelInfo& levelInfo, DWORD updatedTickCount);

	///	<summary>
	/// CProgressiveLevel copy constructor.
	///	</summary>
	///	<param name="other">The CProgressiveLevel instance to copy.</param>
	CProgressiveLevel(const CProgressiveLevel& other);

	///	<summary>
	/// Update progressive level.
	///	</summary>
	///	<param name="levelInfo">The level info.</param>
	///	<param name="updatedTickCount">The updated tick count.</param>
	///	<param name="slotsContentFiles">Slots Content Files.</param>
	///	<param name="listOfContentFiles">List of content files.</param>
	void Update(ProgressiveLevelInfo& levelInfo, DWORD updatedTickCount);

	///	<summary>
	/// Returns the level type.
	///	</summary>
	///	<returns>The level type.</returns>
	ProgressiveLevelType::Type GetType() const;

	/// <summary>
	/// Gets the Level ID
	/// </summary>
	/// <returns>The Level ID</returns>
	__int64 GetID() const;

	/// <summary>
	/// Returns true if this level is eligible.
	/// </summary>
	/// <returns>true if level is eligible</returns>
	bool IsEligible() const;

	/// <summary>
	/// Returns true if this odometer is to be shown for this level.
	/// </summary>
	/// <returns>true if odometer is to be shown for this level</returns>
	bool ShowOdometer() const;

	/// <summary>
	/// Set to not show odometer.
	/// </summary>
	void DoNotShowOdometer();

	///	<summary>
	/// Processes a level broadcast.
	///	</summary>
	///	<param name="broadcastLevel">The broadcast level.</param>
	///	<param name="receivedTickCount">The tick count when the broadcast was received.</param>
	///	<returns>true if a priority forward to the EGM is needed, false otherwise.</returns>
	bool ProcessLevelBroadcast(const CProgressiveLevelBroadcast::Level& broadcastLevel, DWORD receivedTickCount);

	///	<summary>
	/// Processes a eligible level Information.
	///	</summary>
	///	<param name="eligibleLevelInfo">The eligible level.</param>
	///	<returns>true if eligible level was changed, false otherwise.</returns>
	bool ProcessEligibleLevelInfo(const ProgressiveEligibleLevelInfo* eligibleLevelInfo);

	///	<summary>
	/// Clears the eligible level information from this level.
	///	</summary>
	void ClearEligibleLevelInfo();

	///	<summary>
	/// Returns the JSON string containing the level's eligible info.
	///	</summary>
	///	<returns>The JASON string with eligible level info. If no eligible level info an empty string is returned.</returns>
	std::string GetEligibleLevelJSONString();

	///	<summary>
	/// Returns a string representing the contents of this level.
	///	</summary>
	///	<param name="pLogEvent">Pointer to the string to start the log entry with.</param>
	///	<param name="pNewName">Optional pointer to the new name, which will cause
	///		both old and new names to be logged.</param>
	///	<param name="pNewValue">Optional pointer to the new value, which will cause
	///		both old and new values to be logged.</param>
	///	<param name="isStale">If <b>true</b> and stale, message will indicate
	///		that level will be deleted.</param>
	/// <returns>A string representing the contents of this level.</returns>
	std::string GetLogString(LPCTSTR pLogEvent, LPCTSTR pNewName = nullptr, const __int64* pNewValue = nullptr, StaleState staleState = IsStale) const;

	///	<summary>
	///	Returns the web diagnostic html string.
	///	</summary>
	///	<param name="tickCount">The tick count.</param>
	///	<param name="localTime">The LOCAL time.</param>
	///	<returns>The web diagnostic html string.</returns>
	std::string GetDiagnosticsHtml(DWORD tickCount, const SYSTEMTIME& localTime) const;

	///	<summary>
	/// Determines if a level is stale and optionally adds non-stale levels to pProgressiveLevelForward.
	///	</summary>
	///	<param name="tickCount">The tick count.</param>
	///	<param name="pProgressiveLevelForward">An optional pointer to a CProgressiveLevelForward. If
	///		not nullptr, all valid standard or bonusing levels will be added to it.</param>
	///	<param name="pLocalTime">Optional pointer to the LOCAL time.</param>
	///	<returns>true if the level is not stale, false otherwise.</returns>
	bool ProcessForward(DWORD tickCount, CProgressiveLevelForward* pProgressiveLevelForward, const SYSTEMTIME* pLocalTime = nullptr);

	/// <summary>
	/// Updates the PTEState for this object
	/// </summary>
	/// <param name="pteState">PTEState</param>
	void UpdatePteState(const ProgressivePteState& pteState);

	/// <summary>
	/// Gets the state of the pte.
	/// </summary>
	/// <returns>PTE State</returns>
	ProgressivePteState GetPteState();

	/// <summary>
	/// Checks if it is time to display anticipation.
	/// </summary>
	/// <param name="intMsg">The CInternalMessage to send to UI.</param>
	/// <returns>Should display anticipation</returns>
	bool CheckDisplayAnticipation(CInternalMessage *&intMsg);

	/// <summary>
	/// Checks the display end countdown.
	/// </summary>
	/// <param name="intMsg">The int MSG.</param>
	/// <returns>Should display ending countdown</returns>
	bool CheckDisplayEndCountdown(CInternalMessage *&intMsg);

	/// <summary>
	/// Checks if PTE result needs to be displayed.
	/// </summary>
	/// <param name="intMsg">The int MSG.</param>
	/// <returns>Should display PTE result</returns>
	bool CheckDisplayPteResult(CInternalMessage *&intMsg);

	/// <summary>
	/// Checks the display PTE level up needs to be displayed.
	/// </summary>
	/// <param name="intMsg">Should display PTE level up</param>
	/// <returns>Should display PTE level up screen</returns>
	bool CheckDisplayPteLevelUp(CInternalMessage *&intMsg);

	/// <summary>
	/// Checks the display pte leaderboard.
	/// </summary>
	/// <param name="intMsg">The int MSG.</param>
	/// <returns>Should display PTE leaderboard screen</returns>
	bool CheckDisplayPteLeaderboard(CInternalMessage *&intMsg);

	/// <summary>
	/// Determines whether this instance is PTE.
	/// </summary>
	/// <returns>Event is PTE event</returns>
	bool IsPte() const;

	/// <summary>
	/// Determines whether this instance is Scatter.
	/// </summary>
	/// <returns>true if event is Scatter, false otherwise.</returns>
	bool IsScatter() const;

	/// <summary>
	/// Determines whether this instance is PlayXGetYTrigger.
	/// </summary>
	/// <returns>true if event is PlayXGetYTrigger, false otherwise.</returns>
	bool IsPlayXGetY() const;

	///	<summary>
	///	Indicates that the leaderboard was manually initiated by the player.
	///	</summary>
	void LeaderboardManuallyInitiated();

	/// <summary>
	/// Returns prize description for the level.
	/// </summary>
	std::string GetPrizeDesription();

	///	<summary>
	///	Reset the scatter duration.
	///	</summary>
	void ResetScatterDuration();

	///	<summary>
	///	Returns the odometer subscription.
	///	</summary>
	/// <param name="dynamicContentPath">The dynamic content path.</param>
	///	<returns>odometer subscription string.</returns>
	std::string GetSubscriptionOdometerData(const std::string &dynamicContentPath) const;

protected:
	CProgressiveLevel() = default;
	CProgressiveLevel& operator=(const CProgressiveLevel& other) = delete;

	std::string GetLevelTypeString() const;
	std::string GetShowOdometerString() const;

	friend ProgressiveLevelManager;
	friend SentinelNativeUnitTests::StandardProgressiveWinsTests;
	friend SentinelNativeUnitTests::ProgressiveLevelTests;
	friend SentinelNativeUnitTests::WebDiagnostics_Progressive;
	friend SentinelNativeUnitTests::ProgressiveLevelsTests;
	friend SentinelNativeUnitTests::ProgressiveLevelForwardTests;
	friend SentinelNativeUnitTests::ProgressiveLevelBroadcastTests;
	friend SentinelNativeUnitTests::ProgressiveLevelManagerTests;
	friend SentinelNativeUnitTests::ProgressiveLevelManagerTests2;
	friend SentinelNativeUnitTests::ProgressiveLevelTestHelpers::_ICardConfig5;
	friend SentinelNativeUnitTests::ProgressiveLevelTestHelpers::LevelData;
	friend SentinelNativeUnitTests::ProgressiveLevelTestHelpers::TestData_ProcessForward;
};
