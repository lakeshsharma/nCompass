
#include "MobileConnectProtocolKeys.h"
#include "PairingStatusEvent.h"

CPairingStatusEvent::CPairingStatusEvent(bool isSuccess, std::string status) :
        m_sMsgType(ToPlayerDeviceService::pEventPairingStatus),
        m_bSuccess(isSuccess),
        m_sStatus(status)
{
}

const std::string &CPairingStatusEvent::GetMsgType() const {
    return m_sMsgType;
}


bool CPairingStatusEvent::IsSuccess() const {
    return m_bSuccess;
}

const std::string &CPairingStatusEvent::GetStatus() const {
    return m_sStatus;
}
