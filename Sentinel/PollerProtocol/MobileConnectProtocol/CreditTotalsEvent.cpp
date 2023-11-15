
#include "MobileConnectProtocolKeys.h"
#include "CreditTotalsEvent.h"
#include "Logging/LogString.h"

CFundsTransferBalanceEvent::CFundsTransferBalanceEvent(DWORD cashable, DWORD cashablePromo, DWORD nonCashablePromo,
	CPersonalBankerInfo* pbInfo, std::string error, std::string correlationId, bool isCashFunctionalityUnavailable) :
	m_sMsgType(ToPlayerDeviceService::pEventCreditTotals),
	m_cashable(cashable),
	m_cashablePromo(cashablePromo),
	m_nonCashablePromo(nonCashablePromo),
	m_pbInfo(pbInfo),
	m_error(error),
	m_correlationId(correlationId),
	m_isCashFunctionalityUnavailable(isCashFunctionalityUnavailable)
{
}

CFundsTransferBalanceEvent::~CFundsTransferBalanceEvent()
{
	delete m_pbInfo;
}

const std::string& CFundsTransferBalanceEvent::GetMessageType() const
{
	return m_sMsgType;
}

DWORD CFundsTransferBalanceEvent::GetCashable() const
{
	return (m_pbInfo->DisplayTransferCashFromGame() ? m_cashable + m_cashablePromo : AmountZero);
}

DWORD CFundsTransferBalanceEvent::GetNonCashablePromo() const
{
	return (m_pbInfo->DisplayTransferPromoFromGame() ? m_nonCashablePromo : AmountZero);
}

DWORD CFundsTransferBalanceEvent::GetCashablePromo() const
{
	return AmountZero; // we dont upload Points !
}

std::string CFundsTransferBalanceEvent::GetCorrelationId() const
{
	return m_correlationId;
}

bool CFundsTransferBalanceEvent::IsSuccess() const
{
	return (m_error.empty() ? true : false);
}

std::string CFundsTransferBalanceEvent::GetStatus() const
{
	return m_error;
}

bool CFundsTransferBalanceEvent::AreSomePropertiesOffline() const
{
	return m_pbInfo->ArePropertiesOffline();
}

bool CFundsTransferBalanceEvent::IsBannedFromRedemption() const
{
	return m_pbInfo->IsBannedFromRedemptions();
}

bool CFundsTransferBalanceEvent::IsPartialTransferCashFromGameEnabled() const
{
	return m_pbInfo->IsPartialTransferCashFromGameEnabled();
}

DWORD CFundsTransferBalanceEvent::GetCashBalance() const
{
#ifdef _DEBUG
	LogString(POLLER_LOG, "CFundsTransferBalanceEvent::DisplayTransferCashToGame = %d", m_pbInfo->DisplayTransferCashToGame());
	LogString(POLLER_LOG, "CFundsTransferBalanceEvent::GetCashBalance = %d", m_pbInfo->GetCashBalance());
#endif
	return (m_pbInfo->DisplayTransferCashToGame() ? m_pbInfo->GetCashBalance() : AmountZero);
}

DWORD CFundsTransferBalanceEvent::GetPromoBalance() const
{
	return (m_pbInfo->DisplayTransferPromoToGame() ? m_pbInfo->GetPromo2Balance() : AmountZero);
}

DWORD CFundsTransferBalanceEvent::GetPointsAvailable() const
{
	return (m_pbInfo->DisplayTransferPointsToGame() ? m_pbInfo->GetPointsAvailable() : AmountZero);
}

DWORD CFundsTransferBalanceEvent::GetPointsPerDollar() const
{
	return m_pbInfo->GetPointsPerDollar();
}

DWORD CFundsTransferBalanceEvent::GetMinimumPoints() const
{
	return m_pbInfo->GetMinimumPoints();
}

DWORD CFundsTransferBalanceEvent::GetMaximumPoints() const
{
	return m_pbInfo->GetMaximumPoints();
}

DWORD CFundsTransferBalanceEvent::GetMinimumCashDown() const
{
	return m_pbInfo->GetMinimumCashDown();
}

DWORD CFundsTransferBalanceEvent::GetMaximumCashDown() const
{
	return m_pbInfo->GetMaximumCashDown();
}

DWORD CFundsTransferBalanceEvent::GetMinimumPromoDown() const
{
	return m_pbInfo->GetMinimumPromoDown();
}

DWORD CFundsTransferBalanceEvent::GetMaximumPromoDown() const
{
	return m_pbInfo->GetMaximumPromoDown();
}

bool CFundsTransferBalanceEvent::IsCashInactive() const
{
	return m_pbInfo->IsCashInactive();
}

bool CFundsTransferBalanceEvent::IsCashFunctionalityUnavailable() const
{
	return m_isCashFunctionalityUnavailable;
}