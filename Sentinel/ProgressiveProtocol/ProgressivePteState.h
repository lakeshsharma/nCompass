#pragma once
#include <stdafx.h>

namespace SentinelNativeUnitTests { class ProgressivePteStateTest; }

// Done as struct since it's POD and only the level ID is protected after construction.
struct ProgressivePteState
{
private:
	int64_t m_levelId = 0;

public:
	int  curSequence = 0;
	bool wasStartingDisplayed = false;
	bool isLeaderboardDisplayed = false;
	bool wasLeaderboardManuallyInitiated = false;
	bool wasEndingDisplayed = false;
	bool wereResultsDisplayed = false;
	bool initialized = false;
	bool wasEligible = false;
	bool was5secondDisplayed = false;

public:
	///	<summary>
	/// Constructor.
	///	</summary>
	ProgressivePteState() = default;

	///	<summary>
	/// Constructor.
	///	</summary>
	///	<param name="levelId">The level ID.</param>
	explicit ProgressivePteState(int64_t levelId) : m_levelId(levelId) {}

	///	<summary>
	/// Default copy constructor.
	///	</summary>
	ProgressivePteState(const ProgressivePteState&) = default;

	///	<summary>
	/// Default assignment.
	///	</summary>
	ProgressivePteState& operator=(const ProgressivePteState&) = default;

	///	<summary>
	/// Returns the level ID;
	///	</summary>
	///	<returns>The level ID.</returns>
	int64_t GetLevelId() const
	{
		return m_levelId;
	}

private:
	friend SentinelNativeUnitTests::ProgressivePteStateTest;
};
