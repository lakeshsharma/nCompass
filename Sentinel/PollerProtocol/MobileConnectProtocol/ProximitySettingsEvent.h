#ifndef _PROXIMITY_SETTINGS_EVENT_H
#define _PROXIMITY_SETTINGS_EVENT_H

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"

class CProximitySettingsEvent : public CInternalMsgContent<INT_MSG_CONTENT_PROXIMITY_SETTINGS_EVENT>
{
public:
    CProximitySettingsEvent(const rapidjson::Document& jsonDoc);
	
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
	/// Get UUId.
	/// </summary>
	/// <returns>string</returns>
    const std::string &GetUUID() const;

    /// <summary>
	/// Get BeaconId (to be set as Major).
	/// </summary>
	/// <returns>uint8_t</returns>
    uint16_t GetBeaconId() const;

    /// <summary>
	/// Get Point of Interest (to be set as Minor).
	/// </summary>
	/// <returns>uint8_t</returns>
    uint16_t GetPointOfInterest() const;

	/// <summary>
	/// Get transmit power.
	/// </summary>
	/// <returns>uint8_t</returns>
    uint8_t GetTransmitPower() const;

	/// <summary>
	/// Get transmit frequency.
	/// </summary>
	/// <returns>uint8_t</returns>
    uint8_t GetTransmitFrequency() const;

	/// <summary>
	/// Get state of beacon
	/// </summary>
	/// <returns>bool</returns>
    bool GetActive() const;

private:
    std::string m_sDeviceType;
    std::string m_sDeviceId;
	std::string m_sUUID;
	uint16_t m_uBeaconId;
    uint16_t m_uPointOfInterest;
	uint8_t m_uTransmitPower;
	uint8_t m_uTransmitFrequency;
	bool m_bActive;

};

#endif //_PROXIMITY_SETTINGS_EVENT_H
