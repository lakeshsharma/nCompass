#include "stdafx.h"
#include "ProgressiveLevelInfo.h"
#include "Logging/LogString.h"
#include "RapidJsonHelper.h"
#include "MediaPackageManager.h"
#include "SlotsContent.h"

using namespace std;

CMinorAward::CMinorAward(const rapidjson::Value& minorAwardValue, int64_t levelId)
{
	if (minorAwardValue.IsObject())
	{
		m_minorId = JsonGetInt64Ex(minorAwardValue, pKeyId);
		m_sequence = JsonGetInt64Ex(minorAwardValue, pKeySequence);
		m_name = JsonGetStringEx(minorAwardValue, pKeyName);
		const rapidjson::Value& promoGameContentPackage = minorAwardValue[pKeyPromoGameContent];
		if (promoGameContentPackage.IsObject())
		{
			m_promoGameContentPackage.reset(new CSlotsContentPackage(promoGameContentPackage, levelId, SlotsContentType::Type::PromoGame, m_minorId));
		}
		const rapidjson::Value& celebrationContentPackage = minorAwardValue[pKeyCelebrationContent];
		if (celebrationContentPackage.IsObject())
		{
			m_celebrationContentPackage.reset(new CSlotsContentPackage(celebrationContentPackage, levelId, SlotsContentType::Type::Celebration, m_minorId));
		}
	}
}

ProgressiveLevelInfo::ProgressiveLevelInfo(int64_t levelId, ProgressiveLevelType::Type type, const rapidjson::Value& level)
{
	m_id = levelId;
	m_type = type;
	m_name = JsonGetString(level, pKeyName, "");
	m_url = JsonGetStringEx(JsonGetObjectEx(level, pKeyChannel), pKeyMulticastDestination);

	for (unsigned type = 0; type < (unsigned)SlotsContentType::Unknown; type++)
	{
		AddContentPackage(level, (SlotsContentType::Type)type);
	}

	const rapidjson::Value& minorAwards = level[pKeyMinorAwards];

	if (minorAwards.IsArray())
	{
		for (auto minorAward = minorAwards.Begin(); minorAward != minorAwards.End(); ++minorAward)
		{
			m_minorAwards.push_back(unique_ptr<CMinorAward>(new CMinorAward(*minorAward, m_id)));
		}
	}
}

ProgressiveLevelInfo::~ProgressiveLevelInfo()
{
}

void ProgressiveLevelInfo::AddQueryCardsGame(int gameNumber)
{
	m_queryCardsGames.push_back(gameNumber);
}

void ProgressiveLevelInfo::AppendQueryCardsSet(set<DWORD>& queryCardsSet) const
{
	for (int gameNumber : m_queryCardsGames)
	{
		queryCardsSet.insert(gameNumber);
	}
}

void ProgressiveLevelInfo::AddSasLevel(BYTE sasLevel)
{
	if (sasLevel < MaxSasLevels)
	{
		m_sasLevels.push_back(sasLevel);
	}
}

void ProgressiveLevelInfo::SortSASLevels()
{
	sort(m_sasLevels.begin(), m_sasLevels.end());
}

void ProgressiveLevelInfo::AddContentPackage(const rapidjson::Value& level, SlotsContentType::Type type)
{
	const rapidjson::Value& contentPackage = level[SlotsContentType::emap.At(type)];
	if (contentPackage.IsObject())
	{
		m_slotsContentPackages.push_back(unique_ptr<CSlotsContentPackage>(new CSlotsContentPackage(contentPackage, m_id, type)));
	}
}