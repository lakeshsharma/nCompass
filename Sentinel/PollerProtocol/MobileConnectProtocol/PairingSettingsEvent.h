#ifndef _PAIRING_SETTINGS_EVENT_H
#define _PAIRING_SETTINGS_EVENT_H

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"

class CPairingSettingsEvent : public CInternalMsgContent<INT_MSG_CONTENT_PAIRING_SETTINGS_EVENT>
{
public:
    CPairingSettingsEvent(const rapidjson::Document& jsonDoc);
	
	/// <summary>
	/// Get Device Type.
	/// </summary>
	/// <returns>string</returns>
    const std::string &GetDeviceType() const;
	
	/// <summary>
	/// Get DeviceId.
	/// </summary>
	/// <returns>string</returns>
    const std::string &GetDeviceId() const;
	
	/// <summary>
	/// Get Pairing Type.
	/// </summary>
	/// <returns>string</returns>
    const std::string &GetPairingType() const;
	
	/// <summary>
	/// Is Pairing Enabled.
	/// </summary>
	/// <returns>bool</returns>
    bool IsPairingEnabled() const;
	
	/// <summary>
	/// Get Pairing TimeOut in Ms.
	/// </summary>
	/// <returns>uint32_t</returns>
    uint32_t GetPairingTimeOutMs() const;
	
	/// <summary>
	/// Get Idle PairingId.
	/// </summary>
	/// <returns>string</returns>
    const std::string &GetIdlePairingId() const;

	/// <summary>
	/// Get Idle Pairing checksum.
	/// </summary>
	/// <returns>string</returns>
    const std::string &GetIdlePairingChecksum() const;
	
	/// <summary>
	/// Get transmit power.
	/// </summary>
	/// <returns>uint8_t</returns>
    uint8_t GetTransmitPower() const;

	/// <summary>
	/// Get measured power.
	/// </summary>
	/// <returns>uint8_t</returns>
    uint8_t GetMeasuredPower() const;

private:
    std::string m_sDeviceType;
    std::string m_sDeviceId;
    std::string m_sPairingType;
    bool m_bPairingEnabled;
    uint32_t m_uPairingTimeOutMs;
    std::string m_sIdlePairingId;
    std::string m_sIdlePairingChecksum;
	uint8_t m_transmitPower;
	uint8_t m_measuredPower;

};

#endif //_PAIRING_SETTINGS_EVENT_H
