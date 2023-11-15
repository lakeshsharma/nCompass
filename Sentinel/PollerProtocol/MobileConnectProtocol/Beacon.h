#pragma once

class IBeacon
{
public:
	
	/// <summary>
	/// Turn-on beacon.
	/// </summary>
	/// <returns>bool</returns>
	virtual bool TurnOn() = 0;

	/// <summary>
	/// Turn-off beacon.
	/// </summary>
	/// <returns>bool</returns>
	virtual bool TurnOff() = 0;

	/// <summary>
	/// Set Eddystone Frame parameters.
	/// </summary>
	/// <param name="string">IN - eddystone_namespace value.</param>
	/// <param name="string">IN - instance.</param>
	/// <param name="uint8_t">IN - Tx power value.</param>
	/// <returns>bool</returns>
	virtual bool SetEddystoneFrameData(const std::string& eddystone_namespace, const std::string& instance, uint8_t txPower) = 0;

	/// <summary>
	/// Set IBeacon Frame parameters.
	/// </summary>
	/// <param name="string">IN - UUID value.</param>
	/// <param name="string">IN - major</param>
	/// <param name="string">IN - minor</param>
	/// <param name="uint8_t">IN - Tx power value.</param>
	/// <returns>bool</returns>
	virtual bool SetIBeaconFrameData(const std::string& uuid, uint16_t major, uint16_t minor, uint8_t txPower) = 0;

	/// <summary>
	/// Get UUID value.
	/// </summary>
	/// <param name="bool">OUT - UUID value.</param>
	/// <returns>bool</returns>
	virtual bool GetUUID(std::string uuid) = 0;

	/// <summary>
	/// Set major number.
	/// </summary>
	/// <param name="major">IN - Major number.</param>
	/// <returns>bool</returns>
	virtual bool SetMajor(WORD major) = 0;

	/// <summary>
	/// Gets Major number.
	/// </summary>
	/// <param name="major">OUT - Major number.</param>
	/// <returns>bool</returns>
	virtual bool GetMajor(WORD &major) = 0;

	/// <summary>
	/// Set minor number.
	/// </summary>
	/// <param name="minor">IN - Minor number.</param>
	/// <returns>bool</returns>
	virtual bool SetMinor(WORD minor) = 0;

	/// <summary>
	/// Get minor number.
	/// </summary>
	/// <param name="minor">OUT - Minor number.</param>
	/// <returns>bool</returns>
	virtual bool GetMinor(WORD &minor) = 0;

	/// <summary>
	/// Set transmission power.
	/// </summary>
	/// <param name="power">IN - transmission power.</param>
	/// <returns>bool</returns>
	virtual bool SetTransmissionPower(int power) = 0;

	/// <summary>
	/// Get transmission power.
	/// </summary>
	/// <param name="power">OUT - transmission power.</param>
	/// <returns>bool</returns>
	virtual bool GetTransmissionPower(int &power) = 0;

	/// <summary>
	/// Set measured power.
	/// </summary>
	/// <param name="power">IN - measured power.</param>
	/// <returns>bool</returns>
	virtual bool SetMeasuredPower(int power) = 0;

	/// <summary>
	/// Get measured power.
	/// </summary>
	/// <param name="power">OUT - measured power.</param>
	/// <returns>bool</returns>
	virtual bool GetMeasuredPower(int &power) = 0;

	/// <summary>
	/// Set advertising interval.
	/// </summary>
	/// <param name="interval">IN - advertising interval.</param>
	/// <returns>bool</returns>
	virtual bool SetAdvertisingInterval(int interval) = 0;

	/// <summary>
	/// Get advertising interval.
	/// </summary>
	/// <param name="interval">OUT - advertising interva.</param>
	/// <returns>bool</returns>
	virtual bool GetAdvertisingInterval(int &interval) = 0;

	/// <summary>
	/// Set name for beacon.
	/// </summary>
	/// <param name="name">IN - name.</param>
	/// <param name="length">IN - length.</param>
	/// <returns>bool</returns>
	virtual bool SetName(byte *name, size_t length) = 0;

	/// <summary>
	/// Check if the beacon is connected.
	/// </summary>
	/// <returns>bool</returns>
	virtual bool IsConnected()const = 0;

	/// <summary>
	/// Reboot beacon.
	/// </summary>
	/// <returns>bool</returns>
	virtual bool Reboot() = 0;

	/// <summary>
	/// Set individual frames to emit.
	/// </summary>
	/// <returns>bool</returns>
	virtual bool InitializeBeaconFrames(uint8_t frames) = 0;

	virtual ~IBeacon(){};
protected:
	bool m_beaconReady;

};
