#include "stdafx.h"
#include "ConfigSenOption2Field.h"
#include "../GlobalDefs.h"
#include "Utilities.h"

CConfigSenOption2Field::CConfigSenOption2Field(void) :
	CConfigField(SEN_OPTIONS2),
	m_SenOption(0)
{
}

CConfigSenOption2Field::~CConfigSenOption2Field(void)
{
}

void CConfigSenOption2Field::SetSenOptions(const byte * senOption, int length)
{
	CUtilities::MemCopyZero(&m_SenOption, sizeof(m_SenOption),senOption, length);

	m_PBTPinNotRequired	=			(m_SenOption & SO2_PBT_PIN_NOT_REQUIRED)			== SO2_PBT_PIN_NOT_REQUIRED;
	m_couponPromoOutEnabled =		(m_SenOption & SO2_COUPON_PROMO_OUT_ENABLED)		== SO2_COUPON_PROMO_OUT_ENABLED;
	m_voucherOutEnabled	=			(m_SenOption & SO2_VOUCHER_OUT_ENABLED)				== SO2_VOUCHER_OUT_ENABLED;
	m_displayGoodLuckAndSessionPts=	(m_SenOption & SO2_DISPLAY_GOOD_LUCK_AND_SESSION_POINTS) == SO2_DISPLAY_GOOD_LUCK_AND_SESSION_POINTS;
	m_displayTotalAndSessionPtsNum= (m_SenOption & SO2_DISPLAY_TOTAL_AND_SESSION_POINTS_NUM) == SO2_DISPLAY_TOTAL_AND_SESSION_POINTS_NUM;	
	m_displayPtsTotalAsCashSession=	(m_SenOption & SO2_DISPLAY_POINTS_TOTAL_AS_CASH_SESSION) == SO2_DISPLAY_POINTS_TOTAL_AS_CASH_SESSION;
	m_miniVirtualSessionEnabled =	(m_SenOption & SO2_MINI_VIRTUAL_SESSION_ENABLED)	== SO2_MINI_VIRTUAL_SESSION_ENABLED;
	m_accruePointsForNCEP =			(m_SenOption & SO2_ACCRUE_POINTS_FOR_NCEP)			== SO2_ACCRUE_POINTS_FOR_NCEP;
	m_accrueCompForNCEP =			(m_SenOption & SO2_ACCRUE_COMP_FOR_NCEP)			== SO2_ACCRUE_COMP_FOR_NCEP;
	m_oneLinkProgressivesEnabled =	(m_SenOption & SO2_ONE_LINK_PROGRESSIVES_ENABLED) == SO2_ONE_LINK_PROGRESSIVES_ENABLED;
	m_rcclCardsEnabled =			(m_SenOption & SO2_RCCL_CARDS_ENABLED)				== SO2_RCCL_CARDS_ENABLED;
	m_ujpEnabled =					(m_SenOption & SO2_UJP_ENABLED)						== SO2_UJP_ENABLED;
	m_probePBTLock = 				(m_SenOption & SO2_PROBE_PBT_LOCK)					== SO2_PROBE_PBT_LOCK;
	m_mgmdEnabled =					(m_SenOption & SO2_MGMD_ENABLED)					== SO2_MGMD_ENABLED;
	m_mobileConnectCommEnabled =    (m_SenOption & SO2_MOBILE_CONNECT_COMMUNICATION_ENABLED)         == SO2_MOBILE_CONNECT_COMMUNICATION_ENABLED;
	m_remoteDebuggingEnabled =		(m_SenOption & SO2_REMOTE_DEBUGGING_ENABLED)	    == SO2_REMOTE_DEBUGGING_ENABLED;
	m_partialWATOutEnabled   =      (m_SenOption & SO2_PB_PARTIAL_WAT_OUT_ENABLED)		== SO2_PB_PARTIAL_WAT_OUT_ENABLED;
	m_cashoutToHostEnabled	 =		(m_SenOption & SO2_CASHOUT_TO_HOST_ENABLED)			== SO2_CASHOUT_TO_HOST_ENABLED;
	m_externalCreditTransferVerification=(m_SenOption & SO2_EXTERNAL_CREDIT_TRANSFER_VERIFICATION)	== SO2_EXTERNAL_CREDIT_TRANSFER_VERIFICATION;
}

DWORD CConfigSenOption2Field::GetSenOptions() const
{
	return m_SenOption;
}

bool CConfigSenOption2Field::PBTPinNotRequired() const
{
	return m_PBTPinNotRequired;
}

bool CConfigSenOption2Field::CouponPromoOutEnabled() const
{
	return m_couponPromoOutEnabled;
}

bool CConfigSenOption2Field::VoucherOutEnabled() const
{
	return m_voucherOutEnabled;
}

bool CConfigSenOption2Field::DisplayGoodLuckAndSessionPts() const
{
	return m_displayGoodLuckAndSessionPts;
}

bool CConfigSenOption2Field::DisplayTotalAndSessionPtsNum() const
{
	return m_displayTotalAndSessionPtsNum;
}

bool CConfigSenOption2Field::DisplayPtsTotalAsCashSession() const
{
	return m_displayPtsTotalAsCashSession;
}

bool CConfigSenOption2Field::RcclCardsEnabled() const
{
	return m_rcclCardsEnabled;
}

bool CConfigSenOption2Field::MiniVirtualSessionEnabled() const
{
	return m_miniVirtualSessionEnabled;
}

bool CConfigSenOption2Field::AccruePointsForNCEP() const
{
	return m_accruePointsForNCEP;
}

bool CConfigSenOption2Field::AccrueCompForNCEP() const
{
	return m_accrueCompForNCEP;
}

bool CConfigSenOption2Field::OneLinkProgressivesEnabled() const
{
	return m_oneLinkProgressivesEnabled;
}

bool CConfigSenOption2Field::MobileConnectCommunicationEnabled() const
{
	return m_mobileConnectCommEnabled;
}

bool CConfigSenOption2Field::RemoteDebuggingEnabled() const
{
	return m_remoteDebuggingEnabled;
}

bool CConfigSenOption2Field::PartialWATOutEnabled() const
{
	return m_partialWATOutEnabled;
}

bool CConfigSenOption2Field::CashoutToHostEnabled() const
{
	return m_cashoutToHostEnabled;
}

bool CConfigSenOption2Field::UJPEnabled() const
{
	return m_ujpEnabled;
}

bool CConfigSenOption2Field::ProbePBTLockEnabled() const
{
	return m_probePBTLock;
}

bool CConfigSenOption2Field::MGMDEnabled() const
{
	return m_mgmdEnabled;
}

bool CConfigSenOption2Field::ExternalCreditTransferVerificationEnabled() const
{
	return m_externalCreditTransferVerification;
}

