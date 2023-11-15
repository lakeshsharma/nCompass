#include "stdafx.h"
#include "ProgressiveWinnerPicked.h"
#include "RapidJsonHelper.h"
#include "ProgressiveKeys.h"
#include "EnumMap.h"
#include "Util/Currency.h"

using namespace std;
using namespace rapidjson;

CProgressiveWinnerPicked::CProgressiveWinnerPicked(DWORD receivedTickCount, const Document& doc)
	: m_receivedTickCount(receivedTickCount)
	, m_levelId(JsonGetIntEx(doc, pKeyLevelId))
	, m_levelName(JsonGetCStringEx(doc, pKeyLevelName))
	, m_winAmount(Currency::ToString(JsonGetDoubleEx(doc, pKeyWinAmount)))
	, m_winnerVenue(JsonGetCString(doc, pKeyWinnerVenue))
	, m_winnerLocation(JsonGetCString(doc, pKeyWinnerLocation))
	, m_winnerAssetNumber(JsonGetCStringEx(doc, pKeyWinnerAssetNumber))
	, m_winningHand(JsonGetCString(doc, pKeyWinningHand))
	, m_winnerDeviceId(JsonGetInt64Ex(doc, pKeyWinnerDeviceId))
	, m_jackpotId(JsonGetInt64Ex(doc, pKeyJackpotId))
	, m_mediaMinSeconds(JsonGetIntEx(doc, pKeyMediaMinSeconds))
	, m_mediaMaxSeconds(JsonGetIntEx(doc, pKeyMediaMaxSeconds))
	, m_sessionId(JsonGetOptionalIntEx(doc, pKeySessionId))
	, m_jackpotTriggerType(JackpotTrigger::emap[JsonGetStringEx(doc, pKeyJackpotTriggerType)])
	, m_pte(JsonGetBool(doc, pKeyPte, false))
	, m_methodOfPayment(MethodOfPayment::emap.At(JsonGetString(doc, pKeyMethodOfPayment), DEFAULT_METHOD_OF_PAYMENT))
	, m_awardType(AwardType::emap.At(JsonGetString(doc, pKeyAwardType), DEFAULT_AWARD_TYPE))
	, m_minorAwardId(JsonMemberExists(doc, pKeyMinorAwardId) ? to_string(JsonGetInt64Ex(doc, pKeyMinorAwardId)) : "")
	, m_externalAwardName(JsonMemberExists(doc, pKeyExternalAwardName) ? JsonGetCStringEx(doc, pKeyExternalAwardName) : "")
	, m_externalAwardValue(JsonMemberExists(doc, pKeyExternalAwardValue) ? JsonGetCStringEx(doc, pKeyExternalAwardValue) : "0")
	, m_externalAwardCurrency(JsonMemberExists(doc, pKeyExternalAwardCurrency) ? JsonGetCStringEx(doc, pKeyExternalAwardCurrency) : "")
	, m_externalAwardId(JsonMemberExists(doc, pKeyExternalAwardId) ? JsonGetCStringEx(doc, pKeyExternalAwardId) : "")
{
}

CProgressiveWinnerPicked::CProgressiveWinnerPicked(const CProgressiveWinnerPicked& other)
{
	*this = other;
}

void CProgressiveWinnerPicked::operator=(const CProgressiveWinnerPicked& other)
{
	m_receivedTickCount = other.m_receivedTickCount;
	m_levelId = other.m_levelId;
	m_levelName = other.m_levelName;
	m_winAmount = other.m_winAmount;
	m_winnerVenue = other.m_winnerVenue;
	m_winnerLocation = other.m_winnerLocation;
	m_winnerAssetNumber = other.m_winnerAssetNumber;
	m_winningHand = other.m_winningHand;
	m_winnerDeviceId = other.m_winnerDeviceId;
	m_mediaMinSeconds = other.m_mediaMinSeconds;
	m_mediaMaxSeconds = other.m_mediaMaxSeconds;
	m_sessionId = other.m_sessionId;
	m_jackpotTriggerType = other.m_jackpotTriggerType;
    m_jackpotId = other.m_jackpotId;
	m_pte = other.m_pte;
	m_methodOfPayment = other.m_methodOfPayment;
	m_awardType = other.m_awardType;
	m_externalAwardName = other.m_externalAwardName;
	m_externalAwardCurrency = other.m_externalAwardCurrency;
	m_externalAwardValue = other.m_externalAwardValue;
	m_externalAwardId = other.m_externalAwardId;
}

std::string CProgressiveWinnerPicked::MethodOfPaymentName()
{
	// return method of payment

	string methodName = "";
	switch (m_methodOfPayment)
	{
	case MethodOfPayment::Type::PayToGame :
		methodName = "PayToGame";
		break;
	case MethodOfPayment::Type::PayToGameWithHandPay:
		methodName = "PayToGameWithHandPay";
		break;
	case MethodOfPayment::Type::PayToPlayerAccount :
		methodName = "PayToPlayerAccount";
		break;
	case MethodOfPayment::Type::TriggerOnly:
		methodName = "TriggerOnly";
		break;
	case MethodOfPayment::Type::PayToPromoGameDevice:
		methodName = "PayToPromoGameDevice";
		break;
	case MethodOfPayment::Type::Claimable:
		methodName = "Claimable";
		break;
	default:
		methodName = "";
		break;
	}

	return methodName;
}

std::string CProgressiveWinnerPicked::AwardTypeName()
{
	string awardType = "";
	switch (m_awardType)
	{
	case AwardType::Type::CashableCredits :
		awardType = "CashableCredits";
		break;
	case AwardType::Type::RestrictedCredits:
		awardType = "RestrictedCredits";
		break;
	case AwardType::Type::NonRestrictedCredits:
		awardType = "NonRestrictedCredits";
		break;
	case AwardType::Type::Points:
		awardType = "Points";
		break;
	case AwardType::Type::TriggerOnly:
		awardType = "TriggerOnly";
		break;
	case AwardType::Type::Freeplay:
		awardType = "Freeplay";
		break;
	case AwardType::Type::Comps:
		awardType = "Comps";
		break;
	case AwardType::Type::Prize:
		awardType = "Prize";
		break;
	case AwardType::Type::Offer:
		awardType = "Offer";
		break;
	case AwardType::Type::ExtendedBucket:
		awardType = "ExtendedBucket";
		break;
	default:
		awardType = "";
		break;
	}

	return awardType;
}

DWORD CProgressiveWinnerPicked::GetReceivedTickCount() const
{
	return m_receivedTickCount;
}

int CProgressiveWinnerPicked::GetLevelId() const
{
	return m_levelId;
}

std::string CProgressiveWinnerPicked::GetLevelName() const
{
	return m_levelName;
}

std::string	CProgressiveWinnerPicked::GetWinAmount() const
{
	return m_winAmount;
}

int64_t CProgressiveWinnerPicked::GetWinAmountInt64() const
{
	std::string::size_type sz;
	double winAmount = stod(m_winAmount, &sz);
	return (int64_t)(winAmount * ONE_DOLLAR_IN_CENTS);
}

std::string CProgressiveWinnerPicked::GetWinnerVenue() const
{
	return m_winnerVenue;
}

std::string CProgressiveWinnerPicked::GetWinnerLocation() const
{
	return m_winnerLocation;
}

std::string CProgressiveWinnerPicked::GetWinnerAssetNumber() const
{
	return m_winnerAssetNumber;
}

std::string CProgressiveWinnerPicked::GetWinningHand() const
{
	return m_winningHand;
}

int64_t CProgressiveWinnerPicked::GetWinnerDeviceId() const
{
	return m_winnerDeviceId;
}

int64_t CProgressiveWinnerPicked::GetJackpotId() const
{
	return m_jackpotId;
}

int CProgressiveWinnerPicked::GetMediaMinSeconds() const
{
	return m_mediaMinSeconds;
}

void CProgressiveWinnerPicked::SetMediaMinSeconds(int mediaMinSeconds)
{
	m_mediaMinSeconds = mediaMinSeconds;
}

int CProgressiveWinnerPicked::GetMediaMaxSeconds() const
{
	return m_mediaMaxSeconds;
}

void CProgressiveWinnerPicked::SetMediaMaxSeconds(int mediaMaxSeconds)
{
	m_mediaMaxSeconds = mediaMaxSeconds;
}

JackpotTrigger::Type CProgressiveWinnerPicked::GetJackpotTriggerType() const
{
	return m_jackpotTriggerType;
}

bool CProgressiveWinnerPicked::IsPte() const
{
	return m_pte;
}

MethodOfPayment::Type CProgressiveWinnerPicked::GetMethodOfPayment() const
{
	return m_methodOfPayment;
}

AwardType::Type CProgressiveWinnerPicked::GetAwardType() const
{
	return m_awardType;
}

string CProgressiveWinnerPicked::GetMinorAwardId() const
{
	return m_minorAwardId;
}

string CProgressiveWinnerPicked::GetExternalAwardName() const
{
	return m_externalAwardName;
}

string CProgressiveWinnerPicked::GetExternalAwardValue() const
{
	return m_externalAwardValue;
}

string CProgressiveWinnerPicked::GetExternalAwardCurrency() const
{
	return m_externalAwardCurrency;
}

string CProgressiveWinnerPicked::GetExternalAwardId() const
{
	return m_externalAwardId;
}

bool CProgressiveWinnerPicked::ForwardToUI() const
{
	return GetWinAmountInt64() > 0 || AwardType::Prize == m_awardType || AwardType::Offer == m_awardType;
}