#include "stdafx.h"
#include "ProgressivePatronSessionInfo.h"
#include "ProgressiveKeys.h"

using namespace std;

ProgressivePatronSessionInfo::ProgressivePatronSessionInfo(int sessionId, const rapidjson::Document& jsonDoc)
	: m_sessionId(sessionId)
{
	const rapidjson::Value& patronInfo = jsonDoc[pKeyPatronInformation];
	if (patronInfo.IsObject())
	{
		m_playerId = JsonGetOptionalInt64Ex(patronInfo, pKeyPlayerId);
		m_sessionPoints = JsonGetOptionalIntEx(patronInfo, pKeySessionPts);
		m_name = JsonGetCString(patronInfo, pKeyName);
		m_cardNumber = JsonGetCString(patronInfo, pKeyCardNumber);
		m_tier = JsonGetCString(patronInfo, pKeyTier);
	}
}

int ProgressivePatronSessionInfo::GetSessionId() const
{
	return m_sessionId;
}

int64_t ProgressivePatronSessionInfo::GetPlayerId(bool& isValid) const
{
	isValid = m_playerId.IsValid();
	return m_playerId.val;
}

int ProgressivePatronSessionInfo::GetSessionPoints(bool& isValid) const
{
	isValid = m_sessionPoints.IsValid();
	return m_sessionPoints.val;
}

LPCTSTR	ProgressivePatronSessionInfo::GetName() const
{
	return m_name.c_str();
}

LPCTSTR ProgressivePatronSessionInfo::GetCardNumber() const
{
	return m_cardNumber.c_str();
}

LPCTSTR	ProgressivePatronSessionInfo::GetTier() const
{
	return m_tier.c_str();
}

