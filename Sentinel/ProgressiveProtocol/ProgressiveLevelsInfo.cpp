#include "stdafx.h"
#include "ProgressiveLevelsInfo.h"
#include "Logging/LogString.h"
#include "RapidJsonHelper.h"
#include "ProgressiveKeys.h"


using namespace std;
using namespace rapidjson;

ProgressiveLevelsInfo::ProgressiveLevelsInfo(const Document& jsonDoc)
	: m_receivedTickCount(CUtilities::GetTickCount())
{
	CUtilities::GetCurrentTime();

	m_progressiveGroupId = JsonGetIntEx(jsonDoc, pKeyProgressiveGroupId);
	m_useAftLockOnTransfers = JsonGetOptionalBoolEx(jsonDoc, pKeyUseAFTLockOnTransfers);
	m_slotsContentUrl = JsonGetOptionalStringEx(jsonDoc, pKeySlotsContentUrl);
	
	Ctor_AddLevels(jsonDoc);
	Ctor_AssignSasLevels(jsonDoc);
	Ctor_Cleanup();
}

void ProgressiveLevelsInfo::Ctor_AddLevels(const Document& jsonDoc)
{
	const rapidjson::Value& levels = jsonDoc[pKeyLevels];

	if (levels.IsArray())
	{
		for (auto it = levels.Begin(); it != levels.End(); ++it)
		{
			int64_t id = JsonGetInt64Ex(*it, pKeyId);
			if (id > 0)
			{
				const char* pType = JsonGetStringEx(*it, pKeyType);
				ProgressiveLevelType::Type type = ProgressiveLevelType::emap.At(pType);
				if (type != ProgressiveLevelType::Invalid)
				{
					unique_ptr<ProgressiveLevelInfo>& level = m_levels[id];
					if (level)
					{
						ostringstream sStream;
						sStream << "Level Id " << id << " is a duplicate.";
						throw SentinelException(sStream.str().c_str());
					}

					level.reset(new ProgressiveLevelInfo(id, type, *it));
				}
			}
		}
	}
}

void ProgressiveLevelsInfo::Ctor_AssignSasLevels(const Document& jsonDoc)
{
	// At this point, ProgressiveLevelsInfo::m_levels contains the levels
	// we may be interested in. Now assign SAS levels to them.

	int64_t sasLevels[MaxSasLevels];
	ZeroMemory(sasLevels, sizeof(sasLevels));

	const rapidjson::Value& games = JsonGetArrayEx(jsonDoc, pKeyGames);
	for (auto it = games.Begin(); it != games.End(); ++it)
	{
		if (JsonGetBool(*it, pKeyQueryCards))
		{
			m_queryPokerCards = true;
			m_queryPokerCardsSet.insert(JsonGetUint(*it, pKeyGameNumber));
		}

		const rapidjson::Value& levels = (*it)[pKeyLevels];
		if (levels.IsArray())
		{
			for (auto gamesLevelIt = levels.Begin(); gamesLevelIt != levels.End(); ++gamesLevelIt)
			{
				if (gamesLevelIt->IsObject())
				{
					int64_t levelId = JsonGetInt64Ex(*gamesLevelIt, pKeyLevelId);

					auto levelIt = m_levels.find(levelId);
					if (levelIt != m_levels.cend())
					{
						int sasLevel = JsonGetIntEx(*gamesLevelIt, pKeyLevelSequence);
						if (levelIt->second->m_type == ProgressiveLevelType::Bonus)
						{
							if (sasLevel != 0)
							{

								LogString(PROGRESSIVE, "Level Id %lld: Bonus level contains a non-zero SAS level (%d).", levelId, sasLevel);
								LogString(ERROR_LOG, "Level Id %lld: Bonus level contains a non-zero SAS level (%d).", levelId, sasLevel);
								m_levels.erase(levelIt);
							}
						}
						else
						{
							// Checking for out-of-bounds.
							if (sasLevel < MinSasLevel || sasLevel >= MaxSasLevels)
							{
								stringstream sStream;
								sStream << "Level Id " << levelId << ": SAS level " << sasLevel << " is out-of-bounds.";
								throw SentinelException(sStream.str().c_str());
							}

							if (sasLevels[sasLevel] != levelId)
							{
								// A non-Bonus SAS level can be used only once.
								if (sasLevels[sasLevel] > InvalidProgressiveLevelId)
								{
									stringstream sStream;
									sStream << "ERROR: Level ID " << levelId << ": SAS level " << sasLevel << " is already being used by level ID " << sasLevels[sasLevel] << ".";
									throw SentinelException(sStream.str().c_str());
								}

								levelIt->second->AddSasLevel((BYTE) sasLevel);
								sasLevels[sasLevel] = levelId;
							}
						}
					}
				}
			}
		}
	}
}

void ProgressiveLevelsInfo::Ctor_Cleanup()
{
	for (auto it = m_levels.begin(); it != m_levels.end();)
	{
		if (it->second->m_sasLevels.size() == 0 && it->second->m_type != ProgressiveLevelType::Bonus)
		{
			it = m_levels.erase(it);
		}
		else
		{
			it->second->SortSASLevels();
			++it;
		}
	}
}

bool ProgressiveLevelsInfo::QueryPokerCards() const
{
	return m_queryPokerCards;
}

std::set<DWORD>  ProgressiveLevelsInfo::GetQueryPokerCardsSet()
{
	return m_queryPokerCardsSet;
}

vector<string> ProgressiveLevelsInfo::GetUrls() const
{
	set<string, str_less_than_nocase> urlSet;
	for (auto& level : m_levels)
	{
		urlSet.insert(level.second->m_url);
	}

	vector<string> urls;
	copy(urlSet.begin(), urlSet.end(), std::back_inserter(urls));
	return urls;
}

std::string ProgressiveLevelsInfo::GetSlotsContentUrl() const
{
	return m_slotsContentUrl;
}