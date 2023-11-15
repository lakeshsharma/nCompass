#pragma once
#include "PollerMessage.h"

typedef struct _Status
{
	byte VerMajor;
	byte VerMinor;
	byte Build;
	byte Revision;
	DWORD Checksum;
	byte IPOctet1;
	byte IPOctet2;
	byte IPOctet3;
	byte IPOctet4;
	byte OSVerMajor;
	byte OSVerMinor;
	byte OSBuild;
	DWORD FeatureFlag;
	byte MGMDAvailable;	
	byte GameStart;	
	byte GameEnd;	
	byte PollingSpeed;
	byte MGMDGlobalOption;
	byte MGMDSentinelOption;
	std::string resolution;
	byte landscape;
	byte touchScreen;
	std::string biosVersion;
	std::string processorType;
	std::string totalMemoryInKB;
	std::string numberOfCores;
	std::string beaconCapability;
	std::string beaconFirmware; 
	std::string carrierFirmwareVersion;
	std::string videoMixerFirmwareVersion;
}Status;

class CPollerStatusMessage :
	public CPollerMessage
{
public:
	CPollerStatusMessage(const Status& status, WORD packetVersion);
	~CPollerStatusMessage(void);

	byte * GetReadWriteBuffer( long &bufferSize );

private:
	void WriteDebugData(int& currentOffset, const std::string& keyString, const std::string& valueString);

	Status m_Status;
	WORD m_pollerPacketVersion;
	// Debug fields
	std::string m_gameStart, m_gameEnd, m_pollingSpeed, m_yes, m_no, m_speeed200, m_speed40, m_mgmdsenoption,
		m_resolution, m_orientation, m_touchScreen, m_processorType, m_numberOfCores, m_biosVersion, m_totalMemoryInKB,
		m_beaconCapability, m_beaconFirmware, m_carrierFirmwareVersion, m_videoMixerFirmwareVersion;
};
