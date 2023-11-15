#pragma once

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"
#include "ProgressiveEligibleLevelInfo.h"
#include "ProgressivePatronSessionInfo.h"

namespace SentinelNativeUnitTests
{
	class ProgressiveBonusingAvailableTests;
	class ProgressiveLevelManagerTests;
}

class CProgressiveEligibleLevelsInfo : public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_ELIGIBLE_LEVELS_INFO>
{
public:
	/// <summary>
	/// CProgressiveEligibleLevelsInfo Constructor.
	/// </summary>
	///	<param name="jsonDoc">The JSON document containing the EligibleLevelsInfo message.</param>
	CProgressiveEligibleLevelsInfo(const rapidjson::Document& jsonDoc);

	/// <summary>
	/// Gets the json string for the eligible levels.
	/// </summary>
	/// <returns>the json string for the eligible levels</returns>
	std::string GetEligibleLevelsJSONString() const;

	/// <summary>
	/// Finds the eligible level for the givel level ID
	/// </summary>
	/// <param name="levelID">The level ID</param>
	/// <returns>The elibible level if found for the ID. If not found returns nullptr</returns>
	ProgressiveEligibleLevelInfo *FindEligibleLevel(int64_t levelID) const;

private:
	CProgressiveEligibleLevelsInfo(const CProgressiveEligibleLevelsInfo& other) = delete;
	void operator=(const CProgressiveEligibleLevelsInfo&) = delete;
	std::map<int64_t, std::unique_ptr<ProgressiveEligibleLevelInfo>> m_levels;
	std::string m_jsonString;
};
