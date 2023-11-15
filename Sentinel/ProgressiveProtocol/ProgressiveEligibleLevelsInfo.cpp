#include "stdafx.h"

#include "ProgressiveEligibleLevelsInfo.h"
#include "RapidJsonHelper.h"
#include "ProgressiveKeys.h"
#include "Logging/LogString.h"

using namespace std;

CProgressiveEligibleLevelsInfo::CProgressiveEligibleLevelsInfo(const rapidjson::Document& jsonDoc)
{
	const rapidjson::Value *eligibleLevels = JsonGetOptionalArrayEx(jsonDoc, pKeyEligibleLevels);
	if (eligibleLevels != nullptr)
	{
		for (auto it = eligibleLevels->Begin(); it != eligibleLevels->End(); ++it)
		{
			int64_t levelId = JsonGetInt64(*it, pKeyLevelId);
			m_levels[levelId] = move(unique_ptr<ProgressiveEligibleLevelInfo>(new ProgressiveEligibleLevelInfo(levelId, *it)));
		}

		JsonWriteStreamString writeStream;
		JsonWriter writer(writeStream);
		eligibleLevels->Accept(writer);
		m_jsonString = move(writeStream.m_str);
	}
}

std::string CProgressiveEligibleLevelsInfo::GetEligibleLevelsJSONString() const
{
	return m_jsonString;
}

ProgressiveEligibleLevelInfo *CProgressiveEligibleLevelsInfo::FindEligibleLevel(int64_t levelID) const
{
	ProgressiveEligibleLevelInfo *eligibleLevel = nullptr;

	auto levelIt = m_levels.find(levelID);
	if (levelIt != m_levels.end())
	{
		eligibleLevel = levelIt->second.get();
	}

	return eligibleLevel;
}