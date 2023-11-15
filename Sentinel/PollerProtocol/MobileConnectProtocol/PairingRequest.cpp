
#include "PairingRequest.h"

CPairingRequest::CPairingRequest(std::string msgType):
        m_msgType(msgType)
{}

const std::string &CPairingRequest::GetMsgType() const {
    return m_msgType;
}

void CPairingRequest::SetMsgType(const std::string &msgType) {
    m_msgType = msgType;
}
