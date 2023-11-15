
#include "CardChangeEvent.h"
#include "MobileConnectProtocolKeys.h"

CCardChangeEvent::CCardChangeEvent(std::string status, bool isSuccess, std::string correlationId):
        m_sStatusText(status),
        m_bSuccess(isSuccess),
        m_correlationId(correlationId)
{
	m_sMessageType = ToPlayerDeviceService::pEventCardChange;
}

const std::string &CCardChangeEvent::GetMessageType() const
{
    return m_sMessageType;
}

const std::string &CCardChangeEvent::GetStatusText() const
{
    return m_sStatusText;
}

bool CCardChangeEvent::GetIsSuccess() const
{
    return m_bSuccess;
}

std::string CCardChangeEvent::GetCorrelationId() const
{
	return m_correlationId;
}

