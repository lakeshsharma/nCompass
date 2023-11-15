#pragma once

#include "ProgressiveKeys.h"
#include "RapidJsonHelper.h"
#include "ProgressiveEligibleMinorInfo.h"

class ProgressiveLevelManager;

namespace SentinelNativeUnitTests
{ 
	class ProgressiveEligibleLevelInfoTests;
	class EGMAwardManager;
}

class ProgressiveEligibleLevelInfo
{
private:
	int64_t m_levelId;
	PteStatus::Type m_pteStatus;
	bool	m_showInfo;
	bool	m_isEligible;
	bool	m_isPlayerQualified;
	bool	m_showOdometer;
	bool	m_pte;
	std::string m_name;
	std::string m_info;
	std::string m_triggerType;
	std::string m_leaderboardUrl;
	std::string m_imageUrl;
	std::string m_valueStr;
	int64_t m_value;
	int64_t m_startTimeUtc;
	int64_t m_endTimeUtc;
	int64_t m_countdownStartTime;
	int64_t m_countdownEndTime;
	int m_remainingDuration;
	double  m_percentEligible;
	AwardType::Type m_awardType;
	std::string m_prizeDescription;
	std::vector<ProgressiveEligibleMinorInfo> m_eligibleMinors;
	std::string m_jsonString;

public:
	///	<summary>
	///	ProgressiveEligibleLevelInfo constructor.
	///	</summary>
	///	<param name="levelId">The level ID.</param>
	ProgressiveEligibleLevelInfo(int64_t levelId);

	/// <summary>
	/// Default copy constructor. Initializes a new instance of the <see cref="ProgressiveEligibleLevelInfo"/> class.
	/// </summary>
	/// <param name="other">IN - The source instance.</param>
	ProgressiveEligibleLevelInfo(const ProgressiveEligibleLevelInfo &other) = default;

	/// <summary>
	/// Default assignment operator.
	/// </summary>
	/// <param name="other">The source instance.</param>
	///	<returns>A reference to this instance.</returns>
	ProgressiveEligibleLevelInfo &operator=(const ProgressiveEligibleLevelInfo &other) = default;

	///	<summary>
	///	ProgressiveEligibleLevelInfo constructor.
	///	</summary>
	///	<param name="levelId">The level ID.</param>
	///	<param name="level">The rapidjson level iterator.</param>
	ProgressiveEligibleLevelInfo(int64_t levelId, const rapidjson::Value& level);

	/// <summary>
	/// Gets a log string for the instance.
	/// </summary>
	/// <param name="description">IN - A description for the log string. MAY be nullptr.</param>
	/// <returns>A log string for the instance.</returns>
	std::string GetLogString(LPCTSTR description = nullptr);

	///	<summary>
	///	Updates this eligible levels info object with the provided object.
	///	</summary>
	///	<returns>true if this object was updated</returns>
	bool Update(const ProgressiveEligibleLevelInfo *progressiveEligibleLevelsInfo);

	///	<summary>
	///	Returns the level ID.
	///	</summary>
	///	<returns>The level ID.</returns>
	int64_t GetLevelId() const;

	///	<summary>
	///	Returns the PTE status.
	///	</summary>
	///	<returns>The PTE status.</returns>
	/// <remarks>This is valid only if IsPte() returns <b>true</b>.</remarks>
	PteStatus::Type GetPteStatus() const;

	///	<summary>
	///	Indicates whether to show the info.
	///	</summary>
	///	<returns><b>true</b> if info should be shown, else <b>false</b>.</returns>
	/// <remarks>This is valid only if IsPte() returns <b>true</b>.</remarks>
	bool ShowInfo() const;

	///	<summary>
	///	Indicates whether level is eligible.
	///	</summary>
	///	<returns><b>true</b> if level is eligible, else <b>false</b>.</returns>
	bool IsEligible() const;

	/// <summary>
	/// Determines whether current pplayer is qualified.
	/// </summary>
	/// <returns><b>true</b> if player is qualifie for this level, else <b>false</b>.</returns>
	bool IsPlayerQualified() const;

	///	<summary>
	///	Indicates whether to show the odometer.
	///	</summary>
	///	<returns><b>true</b> if odometer should be shown, else <b>false</b>.</returns>
	bool ShowOdometer() const;

	/// <summary>
	/// Set to not show odometer.
	/// </summary>
	void DoNotShowOdometer();

	///	<summary>
	///	Indicates whether PTE is present for this level.
	///	</summary>
	///	<returns><b>true</b> if PTE is present for this level, else <b>false</b>.</returns>
	bool IsPte() const;

	///	<summary>
	///	Indicates whether Scatter is present for this level.
	///	</summary>
	///	<returns><b>true</b> if Scatter is present for this level, else <b>false</b>.</returns>
	bool IsScatter() const;
	
	///	<summary>
	///	Indicates whether PlayXGetY is present for this level.
	///	</summary>
	///	<returns><b>true</b> if PlayXGetY is present for this level, else <b>false</b>.</returns>
	bool IsPlayXGetY() const;

	///	<summary>
	///	Determines whether level is scatter level needs to be displayed
	///	</summary>
	///	<returns><b>true</b> if scatter level needs to be displayed, else <b>false</b>.</returns>
	bool DisplayScatterLevel() const;

	///	<summary>
	///	Determines whether level is playxgety level needs to be displayed
	///	</summary>
	///	<returns><b>true</b> if playxgety trigger type level needs to be displayed, else <b>false</b>.</returns>
	bool DisplayPlayXGetYLevel() const;

	///	<summary>
	///	Returns the level name.
	///	</summary>
	///	<returns>The level name.</returns>
	LPCTSTR GetName() const;

	///	<summary>
	///	Returns the level info.
	///	</summary>
	///	<returns>The level info.</returns>
	/// <remarks>This is valid only if IsPte() returns <b>true</b>.</remarks>
	LPCTSTR GetInfo() const;

	///	<summary>
	///	Returns the trigger type.
	///	</summary>
	///	<returns>The trigger type.</returns>
	LPCTSTR GetTriggerType() const;

	///	<summary>
	///	Returns the level info.
	///	</summary>
	///	<returns>The level info.</returns>
	/// <remarks>This is valid only if IsPte() returns <b>true</b>.</remarks>
	LPCTSTR GetLeaderboardUrl() const;

	///	<summary>
	///	Returns the level info.
	///	</summary>
	///	<returns>The level info.</returns>
	/// <remarks>This is valid only if IsPte() returns <b>true</b>.</remarks>
	LPCTSTR GetImageUrl() const;

	///	<summary>
	///	Returns the level info.
	///	</summary>
	///	<returns>The level info.</returns>
	/// <remarks>This is valid only if IsPte() returns <b>true</b>.</remarks>
	int64_t GetValue() const;

	std::string GetValueStr() const;

	///	<summary>
	///	Returns the UTC start time.
	///	</summary>
	///	<returns>The UTC start time.</returns>
	int64_t GetStartTimeUtc() const;
	
	///	<summary>
	///	Returns the UTC end time.
	///	</summary>
	///	<returns>The UTC end time.</returns>
	int64_t GetEndTimeUtc() const;

	///	<summary>
	///	Returns the percent eligible.
	///	</summary>
	///	<returns>The percent eligible.</returns>
	double  GetPercentEligible() const;

	///	<summary>
	///	Returns the award type.
	///	</summary>
	///	<returns>The award type.</returns>
	AwardType::Type GetAwardType() const;

	///	<summary>
	///	Returns the prize description.
	///	</summary>
	///	<returns>The prize description.</returns>
	std::string GetPrizeDescription() const;

	///	<summary>
	///	Returns the JSON string for the eligible level info.
	///	</summary>
	///	<returns>JSON string</returns>
	std::string GetJSonString() const;

	///	<summary>
	///	Returns the number of ProgressiveEligibleMinors.
	///	</summary>
	///	<returns>The number of ProgressiveEligibleMinors.</returns>
	/// <remarks>Will be non-zero only if IsPte() returns <b>true</b>.</remarks>
	size_t GetNumMinors() const;

	///	<summary>
	///	Returns a temporary pointer of the ProgressiveEligibleMinors at index.
	///	</summary>
	///	<returns>The temporary pointer of the ProgressiveEligibleMinors at index. or
	///		nullptr if index is invalid. Do NOT delete this pointer.</returns>
	/// <remarks>Minors will only exist if IsPte() returns <b>true</b>.</remarks>
	const ProgressiveEligibleMinorInfo* operator[](size_t index) const;

	///	<summary>
	///	Iterates through the ProgressiveEligibleMinors, calling callback for each one.
	///	</summary>
	///	<param name="callback">The function which will called for each
	///		ProgressiveEligibleMinorInfo. Return <b>true</b> to continue iterating,
	///		else false to cancel iteration.</param>
	/// <remarks>Minors will only exist if IsPte() returns <b>true</b>.</remarks>
	void ForEachMinor(std::function<bool(const ProgressiveEligibleMinorInfo&)> callback) const;

	///	<summary>
	///	Sets the scatter start time
	///	</summary>
	///	<param name="time">The scatter start time</param>
	void SetScatterStartTime(int64_t time);

	///	<summary>
	///	Sets the scatter end time
	///	</summary>
	///	<param name="time">The scatter end time</param>
	void SetScatterEndTime(int64_t time);

	///	<summary>
	///	Returns the Scatter start time.
	///	</summary>
	///	<returns>The Scatter start time.</returns>
	int64_t GetScatterStartTime() const;

	///	<summary>
	///	Returns the Scatter end time.
	///	</summary>
	///	<returns>The Scatter end time.</returns>
	int64_t GetScatterEndTime() const;

	///	<summary>
	///	Reset the scatter duration.
	///	</summary>
	void ResetScatterDuration();

	///	<summary>
	///	Returns the odometer subscription.
	///	</summary>
	/// <param name="dynamicContentPath">The dynamic content path.</param>
	///	<returns>odometer subscription string.</returns>
	std::string GetSubscriptionOdometerData(const std::string &dynamicContentPath);

private:
	ProgressiveEligibleLevelInfo(int64_t levelId, bool isEligible, bool showOdometer, bool playerQualfied);

	friend SentinelNativeUnitTests::ProgressiveEligibleLevelInfoTests;
	friend SentinelNativeUnitTests::EGMAwardManager;
};
