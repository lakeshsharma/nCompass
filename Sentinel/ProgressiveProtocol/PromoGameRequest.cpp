#include "stdafx.h"
#include "PromoGameRequest.h"
#include "RapidJsonHelper.h"
#include "EnumMap.h"
#include "Util/Currency.h"

using namespace std;
using namespace rapidjson;

const int NO_PAYTABLE_ITEM_TAG(-1);

CPromoGameRequest::CPromoGameRequest(const Document& jsonDoc)
	: m_levelId(JsonGetInt64Ex(jsonDoc, pKeyLevelId)),
	  m_minorAwardId(JsonMemberExists(jsonDoc, pKeyMinorAwardId) ? to_string(JsonGetInt64Ex(jsonDoc, pKeyMinorAwardId)) : ""),
	  m_jackpotId(JsonGetInt64Ex(jsonDoc, pKeyJackpotId)),
	  m_amount(Currency::ToInt64(JsonGetDoubleEx(jsonDoc, pKeyAmount))),
	  m_paytableItemTag(JsonMemberExists(jsonDoc, pKeyPaytableItemTag) ? JsonGetIntEx(jsonDoc, pKeyPaytableItemTag) : NO_PAYTABLE_ITEM_TAG),
	  m_paytableItemType(JsonGetStringEx(jsonDoc, pKeyPaytableItemType)),
	  m_paytableItemValue(JsonGetInt64Ex(jsonDoc, pKeyPaytableItemValue)),	 
	  m_jackpotTriggerType(JackpotTrigger::emap[JsonGetStringEx(jsonDoc, pKeyJackpotTriggerType)])
{
}

CPromoGameRequest::~CPromoGameRequest()
{
}

int64_t CPromoGameRequest::GetLevelId() const
{
	return m_levelId;
}

string CPromoGameRequest::GetMinorAwardId() const
{
	return m_minorAwardId;
}

int64_t CPromoGameRequest::GetJackpotId() const
{
	return m_jackpotId;
}

int64_t CPromoGameRequest::GetAmount() const
{
	return m_amount;
}

std::string CPromoGameRequest::GetPaytableItemType() const
{
	return m_paytableItemType;
}

int64_t CPromoGameRequest::GetPaytableItemValue() const
{
	return m_paytableItemValue;
}

JackpotTrigger::Type CPromoGameRequest::GetJackpotTriggerType() const
{
	return m_jackpotTriggerType;
}

bool CPromoGameRequest::PaytableItemTagExists() const
{
	return NO_PAYTABLE_ITEM_TAG != m_paytableItemTag;
}