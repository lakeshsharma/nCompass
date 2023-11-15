#include "stdafx.h"
#include "ConfigSenOption1Field.h"
#include "../GlobalDefs.h"
#include "Utilities.h"

CConfigSenOption1Field::CConfigSenOption1Field(void) :
	CConfigField(SEN_OPTIONS1),
	m_SenOption(0)
{
}

CConfigSenOption1Field::~CConfigSenOption1Field(void)
{
}

void CConfigSenOption1Field::SetSenOptions(const byte * senOption, int length)
{
	//TBD break these out
	CUtilities::MemCopyZero(&m_SenOption, sizeof(m_SenOption), senOption, length);

	
	m_fillAmount =				(m_SenOption & SO1_FILL_AMOUNT)							== SO1_FILL_AMOUNT;
	m_useHopper =				(m_SenOption & SO1_USE_HOPPER)							== SO1_USE_HOPPER;
	m_useTicketDispenser =		(m_SenOption & SO1_NOT_USED_1)							== SO1_NOT_USED_1;
	m_displayNumericPointTotal= (m_SenOption & SO1_DISPLAY_NUMERIC_POINT_TOTOAL)		== SO1_DISPLAY_NUMERIC_POINT_TOTOAL;
	m_displayPlayerName =		(m_SenOption & SO1_DISPLAY_PLAYER_NAME)					== SO1_DISPLAY_PLAYER_NAME;
	m_displayCountdownAndSession = (m_SenOption & SO1_DISPLAY_COUNTDOWN_AND_SESSION)	== SO1_DISPLAY_COUNTDOWN_AND_SESSION;
	m_displayTotalPtsSessionPtsAsCash = (m_SenOption & SO1_DISPLAY_TOTAL_POINTS_SESSION_POINTS_AS_CASH) == SO1_DISPLAY_TOTAL_POINTS_SESSION_POINTS_AS_CASH;
	m_personalJackpot =			(m_SenOption & SO1_NOT_USED_2)							== SO1_NOT_USED_2;
	m_PBTCashEnable =			(m_SenOption & SO1_PBT_CASH_ENABLE)						== SO1_PBT_CASH_ENABLE;
	m_PBTPointsEnable =			(m_SenOption & SO1_PBT_POINSTS_ENABLE)					== SO1_PBT_POINSTS_ENABLE;
	m_PBTPromoEnable =			(m_SenOption & SO1_PBT_PROMO_ENABLE)					== SO1_PBT_PROMO_ENABLE;
	m_displayComp =				(m_SenOption & SO1_DISPLAY_COMP)						== SO1_DISPLAY_COMP;
	m_PBTPointsAsPromo =		(m_SenOption & SO1_PBT_POINTS_AS_PROMO)					== SO1_PBT_POINTS_AS_PROMO;
	m_coinsWonAccounting =		(m_SenOption & SO1_COINS_WON_ACCOUNTING)				== SO1_COINS_WON_ACCOUNTING;
	m_coinsInAccounting =		(m_SenOption & SO1_COINS_IN_ACCOUNTING)					== SO1_COINS_IN_ACCOUNTING;
	m_displayCongrats =			(m_SenOption & SO1_DISPLAY_CONGRATS)					== SO1_DISPLAY_CONGRATS;
	m_theoreticalAccounting =	(m_SenOption & SO1_THEORETICAL_ACCOUNTING)				== SO1_THEORETICAL_ACCOUNTING;
	m_coinsOutAccounting =		(m_SenOption & SO1_COINS_OUT_ACCOUNTING)				== SO1_COINS_OUT_ACCOUNTING;
	m_cardInPBT =				(m_SenOption & SO1_PBT_ON_CARD_IN)						== SO1_PBT_ON_CARD_IN;
	m_employeePinRequired =		(m_SenOption & SO1_EMPLOYEE_PIN_REQUIRED)				== SO1_EMPLOYEE_PIN_REQUIRED;
	m_gameZeroProgressiveIn =	(m_SenOption & SO1_NOT_USED_3)							== SO1_NOT_USED_3;
	m_partialBonusAward =		(m_SenOption & SO1_PARTIAL_BONUS_AWARD)					== SO1_PARTIAL_BONUS_AWARD;
	m_displayPtsTotalAsCash =	(m_SenOption & SO1_DISPLAY_POINTS_TOTAL_AS_CASH)		== SO1_DISPLAY_POINTS_TOTAL_AS_CASH;
	m_displayGoodLuckAndSession=(m_SenOption & SO1_DISPLAY_GOOD_LUCK_AND_SESSION)		== SO1_DISPLAY_GOOD_LUCK_AND_SESSION;
	m_couponPromoInEnabled =	(m_SenOption & SO1_COUPON_PROMO_IN_ENABLED)				== SO1_COUPON_PROMO_IN_ENABLED;
	m_voucherPromoInEnabled =	(m_SenOption & SO1_VOUCHER_PROMO_IN_ENABLED)			== SO1_VOUCHER_PROMO_IN_ENABLED;
	m_virtualSessionEnabled =	(m_SenOption & SO1_VIRTUAL_SESSION_ENABLED)				== SO1_VIRTUAL_SESSION_ENABLED;
	m_ticketInEnabled =			(m_SenOption & SO1_TICKET_IN_ENABLED)					== SO1_TICKET_IN_ENABLED;
	m_useNewPBMenus =			(m_SenOption & SO1_NOT_USED_4)							== SO1_NOT_USED_4;
	m_PBFromGameEnabled =		(m_SenOption & SO1_PB_FROM_GAME_ENABLED)				== SO1_PB_FROM_GAME_ENABLED;
	m_displayPlayerPtsTotal =	(m_SenOption & SO1_DISPLAY_PLAYER_POINTS_TOTAL)			== SO1_DISPLAY_PLAYER_POINTS_TOTAL;
	m_Lockout =					(m_SenOption & SO1_NOT_USED_5)							== SO1_NOT_USED_5;	

}

DWORD CConfigSenOption1Field::GetSenOptions() const
{
	return m_SenOption;
}

bool CConfigSenOption1Field::FillAmount() const
{
	return m_fillAmount;
}

bool CConfigSenOption1Field::UseHopper() const
{
	return m_useHopper;
}
	
bool CConfigSenOption1Field::DisplayNumericPointTotal() const
{
	return m_displayNumericPointTotal;	
}
	
bool CConfigSenOption1Field::DisplayPlayerName() const
{
	return m_displayPlayerName;
}

bool CConfigSenOption1Field::DisplayCountdownAndSession() const
{
	return m_displayCountdownAndSession;
}

bool CConfigSenOption1Field::DisplayTotalPtsSessionPtsAsCash() const
{
	return m_displayTotalPtsSessionPtsAsCash;
}

bool CConfigSenOption1Field::PBTCashEnable() const
{
	return m_PBTCashEnable;
}

bool CConfigSenOption1Field::PBTPointsEnable() const
{
	return m_PBTPointsEnable;
}
	
bool CConfigSenOption1Field::PBTPromoEnable() const
{
	return m_PBTPromoEnable;
}
	
bool CConfigSenOption1Field::DisplayComp() const
{
	return m_displayComp;
}
	
bool CConfigSenOption1Field::PBTPointsAsPromo() const
{
	return m_PBTPointsAsPromo;
}
	
bool CConfigSenOption1Field::CoinsWonAccounting() const
{
	return m_coinsWonAccounting;
}
	
bool CConfigSenOption1Field::CoinsInAccounting() const
{
	return m_coinsInAccounting;
}
	
bool CConfigSenOption1Field::DisplayCongrats() const
{
	return m_displayCongrats;
}
	
	
bool CConfigSenOption1Field::TheoreticalAccounting() const
{
	return m_theoreticalAccounting;
}
	
bool CConfigSenOption1Field::CoinsOutAccounting() const
{
	return m_coinsOutAccounting;
}
	
bool CConfigSenOption1Field::CardInPBT() const
{
	return m_cardInPBT;
}
	
bool CConfigSenOption1Field::EmployeePinRequired() const
{
	return m_employeePinRequired;
}
	
bool CConfigSenOption1Field::PartialBonusAward() const
{
	return m_partialBonusAward;
}
	
bool CConfigSenOption1Field::DisplayPtsTotalAsCash() const
{
	return m_displayPtsTotalAsCash;
}
	
bool CConfigSenOption1Field::DisplayGoodLuckAndSession() const
{
	return m_displayGoodLuckAndSession;
}

bool CConfigSenOption1Field::CouponPromoInEnabled() const
{
	return m_couponPromoInEnabled;
}
	
bool CConfigSenOption1Field::VoucherPromoInEnabled() const
{
	return m_voucherPromoInEnabled;
}
	
bool CConfigSenOption1Field::VirtualSessionEnabled() const
{
	return m_virtualSessionEnabled;
}
	
bool CConfigSenOption1Field::TicketInEnabled() const
{
	return m_ticketInEnabled;
}
	
bool CConfigSenOption1Field::UseNewPBMenus() const
{
	return m_useNewPBMenus;
}
	
bool CConfigSenOption1Field::PBFromGameEnabled() const
{
	return m_PBFromGameEnabled;
}
	
bool CConfigSenOption1Field::DisplayPlayerPtsTotal() const
{
	return m_displayPlayerPtsTotal;
}
