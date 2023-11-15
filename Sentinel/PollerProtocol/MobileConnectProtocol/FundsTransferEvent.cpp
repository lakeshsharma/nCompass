
#include "MobileConnectProtocolKeys.h"
#include "FundsTransferEvent.h"

CFundsTransferEvent::CFundsTransferEvent(std::string pin, std::string fundType, std::string transferDirection, bool success, std::string status,
		DWORD cashable, DWORD cashablePromo, DWORD nonCashablePromo, std::string correlationId) :
        m_messageType(ToPlayerDeviceService::pEventFundsTransfer),
		m_pin(pin),
		m_fundType(fundType),
		m_transferDirection(transferDirection),
		m_success(success),
		m_status(status),
        m_cashable(cashable),
        m_cashablePromo(cashablePromo),
        m_nonCashablePromo(nonCashablePromo),
        m_correlationId(correlationId)
{
}

const std::string& CFundsTransferEvent::GetMessageType() const 
{
    return m_messageType;
}

const std::string& CFundsTransferEvent::GetPin() const 
{
    return m_pin;
}

const std::string& CFundsTransferEvent::GetFundType() const 
{
    return m_fundType;
}

const std::string& CFundsTransferEvent::GetTransferDirection() const 
{
    return m_transferDirection;
}


bool CFundsTransferEvent::IsSuccess() const
{
	return m_success;
}

const std::string& CFundsTransferEvent::GetStatus() const
{
	 return m_status;
}

DWORD CFundsTransferEvent::GetCashable() const
{
	return m_cashable;
}

DWORD CFundsTransferEvent::GetNonCashablePromo() const
{
	return m_nonCashablePromo;
}

DWORD CFundsTransferEvent::GetCashablePromo() const
{
	return m_cashablePromo;
}

std::string CFundsTransferEvent::GetCorrelationId() const
{
	return m_correlationId;
}

