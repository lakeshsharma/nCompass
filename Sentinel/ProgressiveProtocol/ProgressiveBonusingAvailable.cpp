#include "stdafx.h"
#include "ProgressiveBonusingAvailable.h"
#include "RapidJsonHelper.h"
#include "ProgressiveKeys.h"
#include "Logging/LogString.h"

using namespace std;

CProgressiveBonusingAvailable::CProgressiveBonusingAvailable(int sessionId, const rapidjson::Document& jsonDoc)
	: m_receivedTickCount(CUtilities::GetTickCount())
	, m_sessionId(sessionId)
	, m_eligibilityDetermined(JsonGetBoolEx(jsonDoc, pKeyEligibilityDetermined))
{
	CUtilities::GetUTCSystemTime(&m_receivedTime);

}

int CProgressiveBonusingAvailable::GetSessionId() const
{
	return m_sessionId;
}

bool CProgressiveBonusingAvailable::IsEligibilityDetermined() const
{
	return m_eligibilityDetermined;
}

SYSTEMTIME CProgressiveBonusingAvailable::GetReceivedTime() const
{
	return m_receivedTime;
}
