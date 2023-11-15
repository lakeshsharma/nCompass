#include "stdafx.h"
#include "PromoGameAcceptanceStatus.h"
#include "RapidJsonHelper.h"
#include "EnumMap.h"
#include "Util/Currency.h"

using namespace std;
using namespace rapidjson;

CPromoGameAcceptanceStatus::CPromoGameAcceptanceStatus(int64_t jackpotId) :
	m_promoGameStatus(PromoGameStatus::Accepted), m_jackpotId(jackpotId)
{
}

CPromoGameAcceptanceStatus::~CPromoGameAcceptanceStatus()
{
}

std::string CPromoGameAcceptanceStatus::GetPromoGameStatus() const
{
	return PromoGameStatus::emap.At(m_promoGameStatus);
}

int64_t CPromoGameAcceptanceStatus::GetJackpotId() const
{
	return m_jackpotId;
}
