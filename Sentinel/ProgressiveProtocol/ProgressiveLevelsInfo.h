#pragma once

#include "InternalMsgContent.h"
#include "ProgressiveLevelInfo.h"

namespace SentinelNativeUnitTests 
{ 
	class ProgressiveLevelsInfoTests;
	class ProgressiveLevelManagerTests;
	namespace ProgressiveLevelTestHelpers { class _ICardConfig5; }
}

class CMediaPackageManager;

class ProgressiveLevelManager;

class ProgressiveLevelsInfo : public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_CONFIG>
{
protected:
	std::map<int64_t, std::unique_ptr<ProgressiveLevelInfo>> m_levels;
	std::set<DWORD> m_queryPokerCardsSet;
	SYSTEMTIME m_receivedTime;
	DWORD m_receivedTickCount;
	int  m_progressiveGroupId = 0;
	bool m_useAftLockOnTransfers = false;
	bool m_queryPokerCards = false;
	std::string m_slotsContentUrl;

	friend class CMediaPackageManager;

private:
	// These methods are only to be used by the Constructor
	void Ctor_AddLevels(const rapidjson::Document& cardConfig);
	void Ctor_AssignSasLevels(const rapidjson::Document& cardConfig);
	void Ctor_Cleanup();

	ProgressiveLevelsInfo() = default;
	ProgressiveLevelsInfo(const ProgressiveLevelsInfo& other) = delete;
	ProgressiveLevelsInfo& operator=(const ProgressiveLevelsInfo& other) = delete;

public:
	/// <summary>
	/// ProgressiveLevelsInfo constructor.
	/// </summary>
	/// <param name="cardConfig">The ICardConfiguration5 message.</param>
	ProgressiveLevelsInfo(const rapidjson::Document& cardConfig);

	///	<summary>
	/// Indicates whether any levels had query cards set.
	///	</summary>
	///	<returns><b>true</b> if any levels had query cards set, else <b>false</b>.</returns>
	///	<remarks>
	///	The underlying value become valid only after calling GetQueryPokerCardsSet().
	///	</remarks>
	bool QueryPokerCards() const;

	///	<summary>
	/// Returns the set of game numbers to query for poker cards.
	///	</summary>
	///	<returns>The set of game numbers to query for poker cards.</returns>
	std::set<DWORD> GetQueryPokerCardsSet();

	///	<summary>
	/// Returns a list of urls to receive multicasts.
	///	</summary>
	///	<returns>A list of urls to receive multicasts</returns>
	std::vector<std::string> GetUrls() const;

	///	<summary>
	/// Returns a slots content URL.
	///	</summary>
	std::string GetSlotsContentUrl() const;

private:
	friend ProgressiveLevelManager;
	friend SentinelNativeUnitTests::ProgressiveLevelsInfoTests;
	friend SentinelNativeUnitTests::ProgressiveLevelManagerTests;
	friend SentinelNativeUnitTests::ProgressiveLevelTestHelpers::_ICardConfig5;
};
