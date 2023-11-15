#pragma once
#include "Beacon.h"

#define EDDYSTONE_FRAME		4
#define IBEACON_FRAME		1

class CRadBeacon : public IBeacon
{
public:
	CRadBeacon();
	~CRadBeacon();
	
	/// <summary>
	/// Turn-on beacon.
	/// </summary>
	/// <returns>bool</returns>
	virtual bool TurnOn();

	/// <summary>
	/// Turn-off beacon.
	/// </summary>
	/// <returns>bool</returns>
	virtual bool TurnOff();

	/// <summary> Sets beacon UUID and checksum parameters for Eddystone Frame. 
	/// </summary>
	/// <param name="uuid">10 byte UUID</param> 
	/// <param name="checksum">6 byte checksum</param> 
	/// <param name="power">Transmitter power level 255 through 
	/// 128</param> 
	/// </summary> 
	virtual bool SetEddystoneFrameData(const std::string& uuid, const std::string& checksum, uint8_t power);

	/// <summary> Sets beacon UUID, major and minor parameters for IBeacon Frame.
	/// </summary>
	/// <param name="uuid">16 byte UUID</param> 
	/// <param name="major">uint16_t major</param> 
	/// <param name="minor">uint16_t minor</param> 
	/// <param name="power">Transmitter power level 255 through 
	/// 128</param> 
	/// </summary> 
	virtual bool SetIBeaconFrameData(const std::string& uuid, uint16_t major, uint16_t minor, uint8_t power);
	
	/// <summary>
	/// Get UUID value.
	/// </summary>
	/// <param name="bool">OUT - UUID value.</param>
	/// <returns>bool</returns>
	virtual bool GetUUID(std::string uuid);
	
	/// <summary>
	/// Set major number.
	/// </summary>
	/// <param name="major">IN - Major number.</param>
	/// <returns>bool</returns>
	virtual bool SetMajor(WORD major);
	
	/// <summary>
	/// Gets Major number.
	/// </summary>
	/// <param name="major">OUT - Major number.</param>
	/// <returns>bool</returns>
	virtual bool GetMajor(WORD &major);
	
	/// <summary>
	/// Set minor number.
	/// </summary>
	/// <param name="minor">IN - Minor number.</param>
	/// <returns>bool</returns>
	virtual bool SetMinor(WORD minor);
		
	/// <summary>
	/// Get minor number.
	/// </summary>
	/// <param name="minor">OUT - Minor number.</param>
	/// <returns>bool</returns>
	virtual bool GetMinor(WORD &minor);
	
	/// <summary>
	/// Set transmission power.
	/// </summary>
	/// <param name="power">IN - transmission power.</param>
	/// <returns>bool</returns>
	virtual bool SetTransmissionPower(int power);
	
	/// <summary>
	/// Get transmission power.
	/// </summary>
	/// <param name="power">OUT - transmission power.</param>
	/// <returns>bool</returns>
	virtual bool GetTransmissionPower(int &power);
		
	/// <summary>
	/// Set measured power.
	/// </summary>
	/// <param name="power">IN - measured power.</param>
	/// <returns>bool</returns>
	virtual bool SetMeasuredPower(int power);
		
	/// <summary>
	/// Get measured power.
	/// </summary>
	/// <param name="power">OUT - measured power.</param>
	/// <returns>bool</returns>
	virtual bool GetMeasuredPower(int &power);
		
	/// <summary>
	/// Set advertising interval.
	/// </summary>
	/// <param name="interval">IN - advertising interval.</param>
	/// <returns>bool</returns>
	virtual bool SetAdvertisingInterval(int interval);
		
	/// <summary>
	/// Get advertising interval.
	/// </summary>
	/// <param name="interval">OUT - advertising interva.</param>
	/// <returns>bool</returns>
	virtual bool GetAdvertisingInterval(int &interval);

	/// <summary> Set beacon name
	/// </summary>
	/// <param name="name">Name</param> 
	/// <param name="length">Length</param> 
	/// </summary> 
	virtual bool SetName(byte *name, size_t length);
	
	/// <summary>
	/// Check if the beacon is connected.
	/// </summary>
	/// <returns>bool</returns>
	virtual bool IsConnected() const;
	
	/// <summary>
	/// Reboot beacon.
	/// </summary>
	/// <returns>bool</returns>
	virtual bool Reboot();

	/// <summary>
	/// Set individual frames to emit.
	/// </summary>
	/// <returns>bool</returns>
	virtual bool InitializeBeaconFrames(uint8_t frames);

private:
	bool InitializeSerial(const std::string &deviceString);
	bool Echo();
	void coutData(std::string label, uint8_t *data, ssize_t len);
	int m_usb;
	std::string m_devicePath;
};


