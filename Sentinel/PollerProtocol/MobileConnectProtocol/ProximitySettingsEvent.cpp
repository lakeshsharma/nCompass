
#include "MobileConnectProtocolKeys.h"
#include "Logging/LogString.h"
#include "ProximitySettingsEvent.h"

CProximitySettingsEvent::CProximitySettingsEvent(const rapidjson::Document &jsonDoc)
{
	std::string dummyStr; // to avoid crash, if key not found

	try
	{
		const rapidjson::Value& payload = jsonDoc;
		
		m_sDeviceType        = JsonGetString(payload, pFKeyDeviceType, dummyStr.c_str());
	    m_sDeviceId          = JsonGetString(payload, pFKeyDeviceId, dummyStr.c_str());
        m_sUUID              = JsonGetString(payload, pFKeyUUID, dummyStr.c_str());
        m_uBeaconId          = JsonGetUint(payload, pFKeyBeaconId);
        m_uPointOfInterest   = JsonGetUint(payload, pFKeyPointOfInterest);
        m_uTransmitPower     = JsonGetUint(payload, pFKeyTransmitPower);
        m_uTransmitFrequency = JsonGetUint(payload, pFKeyTransmitFrequency);
		m_bActive			 = JsonGetBool(payload, pFKeyActive);
		
		LogString(POLLER_LOG, "%s %d: Data received from queue : DeviceType: %s, DeviceId: %s, UUID: %s, BeaconId: %u, PointOfInterest: %u, TransmitPower: %u, TransmitFrequency: %u, Active: %u",
				  __FUNCTION__, __LINE__, m_sDeviceType.c_str(), m_sDeviceId.c_str(), m_sUUID.c_str(), m_uBeaconId, m_uPointOfInterest, m_uTransmitPower, m_uTransmitFrequency, m_bActive);
	}
	catch(...)
	{
		 LogString(ERROR_LOG,"Exception while reading response in CProximitySettingsEvent");
	}
}

const std::string &CProximitySettingsEvent::GetDeviceType() const
{
    return m_sDeviceType;
}

const std::string &CProximitySettingsEvent::GetDeviceId() const
{
    return m_sDeviceId;
}

const std::string &CProximitySettingsEvent::GetUUID() const
{
    return m_sUUID;
}

uint16_t CProximitySettingsEvent::GetBeaconId() const
{
	return m_uBeaconId;
}

uint16_t CProximitySettingsEvent::GetPointOfInterest() const
{
	return m_uPointOfInterest;
}

uint8_t CProximitySettingsEvent::GetTransmitPower() const
{
	return m_uTransmitPower;
}

uint8_t CProximitySettingsEvent::GetTransmitFrequency() const
{
	return m_uTransmitFrequency;
}

bool CProximitySettingsEvent::GetActive() const
{
	return m_bActive;
}