#include "MobileConnectProtocolKeys.h"
#include "ProximityStatusEvent.h"

const std::string PROXIMITY_ID_SET("ProximityIdSet");
const std::string PROXIMITY_ID_SET_INACTIVE("ProximityIdSetInactive");
const std::string BEACON_ERROR("BeaconError");

CProximityStatusEvent::CProximityStatusEvent(bool isSuccess, std::string status) :
        m_sMsgType(ToPlayerDeviceService::pEventProximityStatus),
        m_bSuccess(isSuccess),
        m_sStatus(status)
{
}

const std::string &CProximityStatusEvent::GetMsgType() const {
    return m_sMsgType;
}

bool CProximityStatusEvent::IsSuccess() const {
    return m_bSuccess;
}

const std::string &CProximityStatusEvent::GetStatus() const {
    return m_sStatus;
}
