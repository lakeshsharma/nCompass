#include "stdafx.h"
#include "BeaconManager.h"
#include "Logging/LogString.h"
const uint8_t DEFAULTPOWER = 2;

using namespace std;

CBeaconManager::CBeaconManager()
{
	LogString(BEACON_LOG, "%s %d : ", __FUNCTION__, __LINE__);
	m_beacon = nullptr;
	m_beaconPower = DEFAULTPOWER;

	InitialiseAvailableBeacon();
}

// return : true-> if state changed, false -> for no state change
bool CBeaconManager::CheckBeaconConnected()
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);
	// assume connected if beacon instance is available
	bool initialConnectedState = (m_beacon != nullptr) ? true : false;
	bool finalConnectedState = false, isChanged = false;

    if (nullptr == m_beacon || !m_beacon->IsConnected()) 
    {
		InitialiseAvailableBeacon();
		// No beacon or not connected, check if there is any beacon available, if yes, stateChanged
		if (nullptr != m_beacon)
			finalConnectedState = true;
	}
	else
	{
		finalConnectedState = true;
	}

	// state changed
	if (initialConnectedState != finalConnectedState)
		isChanged = true;

	return isChanged;
}

IBeacon* CBeaconManager::GetBeacon()
{
    return m_beacon;
}

bool CBeaconManager::SetEddystoneFrameData(const std::string& uuid, const std::string& checksum, uint8_t measuredPower)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

    bool retVal = false;

    if (nullptr != GetBeacon() ) 
    {
        retVal = GetBeacon()->SetEddystoneFrameData(uuid, checksum, measuredPower);
    }

    LogString(BEACON_LOG, "SetEddystoneFrameData: Setting beacon UUID [%s], Checksum [%s], MeasuredPower [%d]: Result [%s].", 
		uuid.c_str(), checksum.c_str(), measuredPower,
		retVal ? "successful" : "failure");

    return retVal;
}

bool CBeaconManager::SetIBeaconFrameData(const std::string& uuid, uint16_t major, uint16_t minor, uint8_t measuredPower)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

    bool retVal = false;

    if (nullptr != GetBeacon() ) 
    {
        retVal = GetBeacon()->SetIBeaconFrameData(uuid, major, minor, measuredPower);
    }

    LogString(BEACON_LOG, "SetIBeaconFrameData: Setting Beacon UUID [%s], Major [%d], Minor [%d], MeasuredPower [%d]: Result [%s].", 
		uuid.c_str(), major, minor, measuredPower,
		retVal ? "successful" : "failure");

    return retVal;
}

void CBeaconManager::InitialiseAvailableBeacon()
{
	/* TODO put back once OS work is ready.
	
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

	LogString(BEACON_LOG, "%s %d : ", __FUNCTION__, __LINE__);
	delete m_beacon;
	m_beacon = nullptr;

 	CRadBeacon *radBeacon = new CRadBeacon();
    // Create instance for 2nd beacon and check if it is connected, similar to radBeacon. m_beacon will hold instance of connected beacon.

	if (radBeacon->IsConnected())
	{
		m_beacon = radBeacon;
		m_beacon->SetTransmissionPower(m_beaconPower);
	}
	else
		delete radBeacon;

	*/
}

void CBeaconManager::SetBeaconPower(uint8_t power)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);
	LogString(BEACON_LOG, _T("Setting beacon transmit power to %d"), power);
	m_beaconPower = power;
	if (m_beacon != nullptr)
	{
		m_beacon->SetTransmissionPower(power);
	}
}

uint8_t CBeaconManager::GetBeaconPower() const
{
	return m_beaconPower;
}

bool CBeaconManager::InitializeBeaconFrames(uint8_t frames) 
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

	bool retVal = false;

	if (m_beacon != nullptr)
	{
		retVal = m_beacon->InitializeBeaconFrames(frames);
	}

	return retVal;
}