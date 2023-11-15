#include "stdafx.h"
#include "ProgressiveOfferRequest.h"
#include "RapidJsonHelper.h"
#include "EnumMap.h"
#include "Util/Currency.h"

using namespace std;
using namespace rapidjson;

CProgressiveOfferRequest::CProgressiveOfferRequest(const Document& jsonDoc)
	: m_jackpotId(JsonGetInt64Ex(jsonDoc, pKeyJackpotId)),
	  m_playerId(JsonGetInt64Ex(jsonDoc, pKeyPlayerId)),
	  m_offerId(JsonGetStringEx(jsonDoc, pKeyOfferId)),
	  m_transactionId(JsonGetStringEx(jsonDoc, pKeyTransactionId))
{
}

CProgressiveOfferRequest::~CProgressiveOfferRequest()
{
}

int64_t CProgressiveOfferRequest::GetJackpotId() const
{
	return m_jackpotId;
}

int64_t CProgressiveOfferRequest::GetPlayerId() const
{
	return m_playerId;
}

string CProgressiveOfferRequest::GetOfferId() const
{
	return m_offerId;
}

string CProgressiveOfferRequest::GetTransactionId() const
{
	return m_transactionId;
}