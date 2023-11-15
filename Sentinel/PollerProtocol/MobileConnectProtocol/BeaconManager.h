#pragma once

#include "stdafx.h"
#include "Beacon.h"
#include "RadBeacon.h"

class CBeaconManager
{

public:
	
    CBeaconManager();

    /// <summary>
	/// Called-in time event to assertain beacon attached.
	/// </summary>
	/// <returns>bool</returns>
    bool CheckBeaconConnected();

    /// <summary> Sets beacon UUID and checksum parameters for Eddystone Frame.
	/// </summary>
	/// <param name="uuid">10 byte uuid</param> 
    /// <param name="checksum">6 byte checksum string</param> 
	/// <param name="measuredPower">Measured power</param> 
	virtual bool SetEddystoneFrameData(const std::string& uuid, const std::string& checksum, uint8_t measuredPower);

	/// <summary> Sets beacon UUID, major and minor parameters for IBeacon Frame. 
	/// </summary>
	/// <param name="uuid">16 byte UUID</param> 
	/// <param name="major">2 byte major</param> 
	/// <param name="minor">2 byte minor</param> 
	/// <param name="power">Transmitter power level 255 through 
	/// 128</param> 
	/// </summary> 
	virtual bool SetIBeaconFrameData(const std::string& uuid, uint16_t major, uint16_t minor, uint8_t measuredPower);

    virtual ~CBeaconManager()
    {
    	delete m_beacon;
    }

    /// <summary>
	/// return the available beacon, can be nullptr.
	/// </summary>
	/// <returns>IBeacon*</returns>
    IBeacon* GetBeacon();

	/// <summary> Sets beacon power
	/// </summary>
	/// <param name="power">Beacon power level</param>
	void SetBeaconPower(uint8_t power);

	/// <summary> Gets the beacon power
	/// </summary>
	uint8_t GetBeaconPower() const;

	/// <summary>
	/// Set individual frames to emit.
	/// </summary>
	/// <returns>bool</returns>
	bool InitializeBeaconFrames(uint8_t frames);

private:
    void InitialiseAvailableBeacon();

	
private:
	
	IBeacon *m_beacon;
	uint8_t m_beaconPower;
	
	std::recursive_mutex m_criticalSection;
	
};

