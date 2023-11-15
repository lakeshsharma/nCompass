
#include "MobileConnectProtocolKeys.h"
#include "Logging/LogString.h"
#include "PairingSettingsEvent.h"

CPairingSettingsEvent::CPairingSettingsEvent(const rapidjson::Document &jsonDoc)
{
	std::string dummyStr; // to avoid crash, if key not found

	try
	{
		const rapidjson::Value& payload = jsonDoc;
		
		m_sDeviceType       = JsonGetString(payload, pFKeyDeviceType, dummyStr.c_str());
	    m_sDeviceId         = JsonGetString(payload, pFKeyDeviceId, dummyStr.c_str());
	    m_sPairingType      = JsonGetString(payload, pFKeyPairingType, dummyStr.c_str());
	    m_bPairingEnabled   = JsonGetBool(payload, pFKeyPairingEnabled);
	    m_uPairingTimeOutMs = JsonGetUint(payload, pFKeyPairingTimeOutMs);
	    m_sIdlePairingId    = JsonGetString(payload, pFKeyIdlePairingId, dummyStr.c_str());
        if (JsonMemberExists(payload, pFKeyIdlePairingChecksum))
        {
            m_sIdlePairingChecksum = JsonGetStringEx(payload, pFKeyIdlePairingChecksum);
        }

		if (JsonMemberExists(payload, pFKeyMeasuredPower))
		{
			m_measuredPower = JsonGetUint(payload, pFKeyMeasuredPower);
			m_transmitPower = JsonGetUint(payload, pFKeyTransmitPower);
		}
		else
		{
			// This message came from older Player Device Service where measured power
			// was being sent to sentinel with the name 'transmitPower' and transmit
			// power is configured externally at sentinel side through config file
			m_measuredPower = JsonGetUint(payload, pFKeyTransmitPower);
			m_transmitPower = 0xff; // marker used for field not specified
		}

		LogString(POLLER_LOG, "%s %d: Data received from queue : DeviceType: %s, DeviceId: %s, PairingType: %s, PairingEnabled: %d, PairingTimeOutMs: %u, IdlePairingId: %s, TransmitPower: %u MeasuredPower: %u",
				  __FUNCTION__, __LINE__, m_sDeviceType.c_str(), m_sDeviceId.c_str(), m_sPairingType.c_str(), m_bPairingEnabled, m_uPairingTimeOutMs, m_sIdlePairingId.c_str(), m_transmitPower, m_measuredPower);
	}
	catch(...)
	{
		 LogString(ERROR_LOG,"Exception while reading response in CPairingSettingsEvent");
	}
}

const std::string &CPairingSettingsEvent::GetDeviceType() const
{
    return m_sDeviceType;
}

const std::string &CPairingSettingsEvent::GetDeviceId() const
{
    return m_sDeviceId;
}

const std::string &CPairingSettingsEvent::GetPairingType() const
{
    return m_sPairingType;
}

bool CPairingSettingsEvent::IsPairingEnabled() const
{
    return m_bPairingEnabled;
}

uint32_t CPairingSettingsEvent::GetPairingTimeOutMs() const
{
    return m_uPairingTimeOutMs;
}

const std::string &CPairingSettingsEvent::GetIdlePairingId() const
{
    return m_sIdlePairingId;
}

const std::string &CPairingSettingsEvent::GetIdlePairingChecksum() const
{
    return m_sIdlePairingChecksum;
}

uint8_t CPairingSettingsEvent::GetTransmitPower() const
{
	return m_transmitPower;
}

uint8_t CPairingSettingsEvent::GetMeasuredPower() const
{
	return m_measuredPower;
}
