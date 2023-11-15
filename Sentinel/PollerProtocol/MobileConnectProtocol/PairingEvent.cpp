#include "PairingEvent.h"
#include "MobileConnectProtocolKeys.h"
#include "Logging/LogString.h"

CPairingEvent::CPairingEvent(const rapidjson::Document &jsonDoc)
{
	std::string dummyStr; // to avoid crash, if key not found

	try
	{
		const rapidjson::Value& payload = jsonDoc;
	
	    m_sMsgType       = JsonGetString(payload, pFKeyMessageType, dummyStr.c_str());
	    m_sDeviceType    = JsonGetString(payload, pFKeyDeviceType, dummyStr.c_str());
	    m_sDeviceId      = JsonGetString(payload, pFKeyDeviceId, dummyStr.c_str());
	    m_sPairingType   = JsonGetString(payload, pFKeyPairingType, dummyStr.c_str());
	    m_sPairingId     = JsonGetString(payload, pFKeyPairingId, dummyStr.c_str());
        if (JsonMemberExists(payload, pFKeyPairingChecksum)) 
        {
            m_sPairingChecksum = JsonGetStringEx(payload, pFKeyPairingChecksum);
        }
	    m_bSuccess       = JsonGetBool(payload, pFKeySuccess);
	    m_sStatus        = JsonGetString(payload, pFKeyStatus, dummyStr.c_str());

	    LogString(POLLER_LOG, "%s %d: Data received from queue : DeviceType: %s, DeviceId: %s, PairingType: %s, PairingId: %s, Success: %d, Status: %s",
	              __FUNCTION__, __LINE__, m_sDeviceType.c_str(), m_sDeviceId.c_str(), m_sPairingType.c_str(), m_sPairingId.c_str(), m_bSuccess, m_sStatus.c_str());
	}
	catch(...)
	{
		 LogString(ERROR_LOG,"Exception while reading response in CPairingEvent");
	}

}

CPairingEvent::CPairingEvent(std::string msgType, std::string deviceType, std::string deviceId, std::string pairingType, std::string pairingId, std::string pairingChecksum, bool success, std::string status)
: m_sMsgType(msgType),
  m_sDeviceType(deviceType),
  m_sDeviceId(deviceId),
  m_sPairingType(pairingType),
  m_sPairingId(pairingId),
  m_sPairingChecksum(pairingChecksum),
  m_bSuccess(success),
  m_sStatus(status)
{
}

const std::string& CPairingEvent::GetDeviceType() const
{
	return m_sDeviceType;
}

const std::string& CPairingEvent::GeDeviceId() const
{
	return m_sDeviceId;
}

const std::string& CPairingEvent::GetPairingType() const
{
	return m_sPairingType;
}

const std::string& CPairingEvent::GetPairingId() const
{
	return m_sPairingId;
}

const std::string& CPairingEvent::GetPairingChecksum() const
{
	return m_sPairingChecksum;
}

const std::string& CPairingEvent::GetStatus() const
{
	return m_sStatus;
}

bool CPairingEvent::GetIsSuccess() const
{
	return m_bSuccess;
}


