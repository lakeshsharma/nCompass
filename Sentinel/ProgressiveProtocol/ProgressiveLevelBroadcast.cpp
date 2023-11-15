#include "stdafx.h"
#include "ProgressiveLevelBroadcast.h"
#include "RapidJsonHelper.h"
#include "ProgressiveKeys.h"
#include "Util/Currency.h"

using namespace std;
using namespace rapidjson;

int64_t _currentTime((int64_t)std::time(0));

CProgressiveLevelBroadcast::Level::Level(int64_t levelId, int64_t levelValue, int64_t countdownStartTime, int64_t countdownEndTime, LPCTSTR pName)
	: id(levelId)
	, value(levelValue)
	, m_countdownStartTime(countdownStartTime)
	, m_countdownEndTime(countdownEndTime)
	, name(pName)
{
}


CProgressiveLevelBroadcast::Level::Level(int64_t levelId, int64_t levelValue, int64_t countdownStartTime, int64_t countdownEndTime, LPCWSTR pName)
	: id(levelId)
	, value(levelValue)
	, m_countdownStartTime(countdownStartTime)
	, m_countdownEndTime(countdownEndTime)
	, name(Convert::WStrToUtf8(pName))
{
}

CProgressiveLevelBroadcast::Level::Level(Value::ConstValueIterator level)
	: id(JsonGetInt64Ex(*level, pKeyLevelId))
	, value(Currency::ToInt64(JsonGetDoubleEx(*level, pKeyValue)))
	, m_countdownStartTime(JsonMemberExists(*level, pKeyCountdownStartTime) ? JsonGetInt64Ex(*level, pKeyCountdownStartTime) / MILLISECONDS_PER_SECOND : INVALID_VALUE)
	, m_countdownEndTime(JsonMemberExists(*level, pKeyCountdownEndTime) ? JsonGetInt64Ex(*level, pKeyCountdownEndTime) / MILLISECONDS_PER_SECOND : INVALID_VALUE)
    , name(JsonGetCStringEx(*level, pKeyName))
{
}

CProgressiveLevelBroadcast::Level::Level(const Level& other)
	: id(other.id)
	, value(other.value)
    , m_countdownStartTime(other.m_countdownStartTime)
	, m_countdownEndTime(other.m_countdownEndTime)
    , name(other.name)
{
}

// used only for testing
CProgressiveLevelBroadcast::CProgressiveLevelBroadcast(DWORD receivedTickCount)
	: m_receivedTickCount(receivedTickCount)
{
}

CProgressiveLevelBroadcast::CProgressiveLevelBroadcast(DWORD receivedTickCount, const Document& jsonDoc)
	: m_receivedTickCount(receivedTickCount)
{
	const Value& levels = JsonGetArrayEx(jsonDoc, pKeyLevelValues);
	for (auto it = levels.Begin(); it != levels.End(); ++it)
	{
		m_levels.push_back(Level(it));
	}
}

void CProgressiveLevelBroadcast::AddLevel(int64_t levelId, int64_t levelValue, int64_t countdownStartTime, int64_t countdownEndTime, LPCTSTR pName)
{
	m_levels.push_back(Level(levelId, levelValue, countdownStartTime, countdownEndTime, pName ? pName : _T("")));
}

