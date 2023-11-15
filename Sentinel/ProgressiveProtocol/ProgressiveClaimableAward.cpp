#include "stdafx.h"
#include "ProgressiveClaimableAward.h"
#include "RapidJsonHelper.h"
#include "ProgressiveKeys.h"
#include "Util/Currency.h"


using namespace std;
using namespace rapidjson;


CProgressiveClaimableAward::CProgressiveClaimableAward(DWORD receivedTickCount, const Document& doc)
	: m_levelId(JsonMemberExists(doc, pKeyLevelId) ? JsonGetIntEx(doc, pKeyLevelId) : -1)
	, m_minorAwardId(JsonMemberExists(doc, pKeyMinorAwardId) ? to_string(JsonGetIntEx(doc, pKeyMinorAwardId)) : "")
	, m_receivedTickCount(receivedTickCount)
	, m_amount(Currency::ToString(JsonGetDoubleEx(doc, pKeyAmount)))
	, m_sessionId(JsonGetOptionalIntEx(doc, pKeySessionId))
	, m_acceptanceUrl(JsonGetCString(doc, pKeyClaimableAcceptanceUrl))
	, m_expirationMilliseconds(JsonGetInt64(doc, pKeyClaimableExpirationMillis, 0))
	, m_isSamePlayer(JsonGetBool(doc, pKeyClaimableSamePlayer))
{
}

