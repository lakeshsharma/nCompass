#include "stdafx.h"
#include "PollerStatusMessage.h"

static const byte debugFieldCount = 11;
static const byte debugFieldCountWithMgmd = 15;

static const int MAX_NUMBER_OF_VALUE_CHARS(42);

CPollerStatusMessage::CPollerStatusMessage(const Status&  status, WORD pollerPacketVersion) :
	m_gameStart("GameStart"), m_gameEnd("GameEnd"), m_pollingSpeed("PollingSpeed"), m_yes("Yes"), m_no("No"), m_speeed200("200"), m_speed40("40"),
	m_mgmdsenoption("MGMDSenOption"), m_resolution("Resolution"), m_orientation("Orientation"), m_touchScreen("TouchScreen"), m_processorType("ProcessorType"), 
	m_numberOfCores("NumCores"), m_biosVersion("BIOSVersion"), m_totalMemoryInKB("TotalMemoryInKB"), m_beaconCapability("BeaconCapability"), 
	m_beaconFirmware("BeaconFirmware"), m_carrierFirmwareVersion("CarrierFirmwareVersion"), m_videoMixerFirmwareVersion("VideoMixerFirmwareVersion")
{
	m_bufferSize = SIZE_OF_MESSAGE_HEADER + sizeof(m_Status)
		+ sizeof(byte) // count of debug fields
		+ sizeof(byte) + m_gameStart.size()		+ sizeof(byte) + MAX_NUMBER_OF_VALUE_CHARS /* KeySize(byte) + key + ValueSize(byte) + value(take biggest it can take)   */
		+ sizeof(byte) + m_gameEnd.size()		+ sizeof(byte) + MAX_NUMBER_OF_VALUE_CHARS
		+ sizeof(byte) + m_pollingSpeed.size()	+ sizeof(byte) + MAX_NUMBER_OF_VALUE_CHARS
		+ sizeof(byte) + m_mgmdsenoption.size()	+ sizeof(byte) + MAX_NUMBER_OF_VALUE_CHARS
		+ sizeof(byte) + m_resolution.size()	+ sizeof(byte) + MAX_NUMBER_OF_VALUE_CHARS
		+ sizeof(byte) + m_orientation.size()	+ sizeof(byte) + MAX_NUMBER_OF_VALUE_CHARS
		+ sizeof(byte) + m_touchScreen.size()	+ sizeof(byte) + MAX_NUMBER_OF_VALUE_CHARS
		+ sizeof(byte) + m_processorType.size() + sizeof(byte) + MAX_NUMBER_OF_VALUE_CHARS
		+ sizeof(byte) + m_numberOfCores.size() + sizeof(byte) + MAX_NUMBER_OF_VALUE_CHARS 
		+ sizeof(byte) + m_biosVersion.size() + sizeof(byte) + MAX_NUMBER_OF_VALUE_CHARS 
		+ sizeof(byte) + m_totalMemoryInKB.size() + sizeof(byte) + MAX_NUMBER_OF_VALUE_CHARS 
		+ sizeof(byte) + m_beaconCapability.size() + sizeof(byte) + MAX_NUMBER_OF_VALUE_CHARS
		+ sizeof(byte) + m_beaconFirmware.size() + sizeof(byte) + MAX_NUMBER_OF_VALUE_CHARS
		+ sizeof(byte) + m_carrierFirmwareVersion.size() + sizeof(byte) + MAX_NUMBER_OF_VALUE_CHARS
		+ sizeof(byte) + m_videoMixerFirmwareVersion.size() + sizeof(byte) + MAX_NUMBER_OF_VALUE_CHARS;

	m_lpBuffer = new byte[m_bufferSize];

	m_Status.Build =  status.Build;
	m_Status.Checksum = status.Checksum;
	m_Status.VerMajor = status.VerMajor;
	m_Status.VerMinor = status.VerMinor;
	m_Status.Revision = status.Revision;

	//TR 26590 Added IP and OS Image version version to the poller status message so we need to send this to SCF.
	m_Status.IPOctet1 = status.IPOctet1;
	m_Status.IPOctet2 = status.IPOctet2;
	m_Status.IPOctet3 = status.IPOctet3;
	m_Status.IPOctet4 = status.IPOctet4;
	m_Status.OSBuild = status.OSBuild;
	m_Status.OSVerMajor = status.OSVerMajor;
	m_Status.OSVerMinor = status.OSVerMinor;
	m_Status.FeatureFlag = status.FeatureFlag;
	m_Status.GameStart = status.GameStart;
	m_Status.GameEnd = status.GameEnd;
	m_Status.PollingSpeed = status.PollingSpeed;
	m_Status.MGMDAvailable = status.MGMDAvailable;
	m_Status.MGMDGlobalOption = status.MGMDGlobalOption;
	m_Status.MGMDSentinelOption = status.MGMDSentinelOption;
	m_Status.resolution = status.resolution;
	m_Status.landscape = status.landscape;
	m_Status.touchScreen = status.touchScreen;
	m_Status.processorType				= status.processorType;
	m_Status.numberOfCores				= status.numberOfCores;
	m_Status.biosVersion				= status.biosVersion;
	m_Status.totalMemoryInKB			= status.totalMemoryInKB;
	m_Status.beaconCapability			= status.beaconCapability;
	m_Status.beaconFirmware				= status.beaconFirmware;
	m_Status.carrierFirmwareVersion		= status.carrierFirmwareVersion;
	m_Status.videoMixerFirmwareVersion	= status.videoMixerFirmwareVersion;
	m_pollerPacketVersion = pollerPacketVersion;
}

CPollerStatusMessage::~CPollerStatusMessage(void)
{
}

byte * CPollerStatusMessage::GetReadWriteBuffer( long &bufferSize )
{
	MessageHeader.MessageID = POLLER_MSG_SENTINEL_STATUS;
	DWORD zeroDword = 0;
	byte zeroByte = 0;
	
	if (m_pollerPacketVersion < 3)
	{
		MessageHeader.DataLength = sizeof(m_Status.Build) + sizeof(m_Status.Checksum) + sizeof(m_Status.VerMajor) + sizeof(m_Status.VerMinor);
	}
	else
	{
		/* Since Message Packet can have variable length we will put the Packet length in the end. 
		     FL needs this as actual length for parsing */		
	}

	int offset = 0;
	//Header (piece by piece to avoid missaligned data error)
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.PacketVersion), sizeof(MessageHeader.PacketVersion));
	offset+= sizeof(MessageHeader.PacketVersion);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.MessageID), sizeof(MessageHeader.MessageID));
	offset+= sizeof(MessageHeader.MessageID);

	int dataLengthOffset = offset;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.DataLength), sizeof(MessageHeader.DataLength));
	offset+= sizeof(MessageHeader.DataLength);
	
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.MacAddress), sizeof(MessageHeader.MacAddress));
	offset+= sizeof(MessageHeader.MacAddress);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SequenceNumber), sizeof(MessageHeader.SequenceNumber));
	offset+= sizeof(MessageHeader.SequenceNumber);
	
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SlotMastID), sizeof(MessageHeader.SlotMastID));
	offset+= sizeof(MessageHeader.SlotMastID);
	
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(MessageHeader.SlotmastRevision), sizeof(MessageHeader.SlotmastRevision));
	offset+= sizeof(MessageHeader.SlotmastRevision);

	//Copy the Data
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(m_Status.VerMajor), sizeof(m_Status.VerMajor));
	offset+= sizeof(m_Status.VerMajor);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(m_Status.VerMinor), sizeof(m_Status.VerMinor));
	offset+= sizeof(m_Status.VerMinor);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(m_Status.Build), sizeof(m_Status.Build));
	offset+= sizeof(m_Status.Build);

	if (m_pollerPacketVersion >= 3)
	{
		memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(m_Status.Revision), sizeof(m_Status.Revision));
		offset+= sizeof(m_Status.Revision);
	}

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(m_Status.Checksum), sizeof(m_Status.Checksum));
	offset+= sizeof(m_Status.Checksum);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(m_Status.IPOctet1), sizeof(m_Status.IPOctet1));
	offset+= sizeof(m_Status.IPOctet1);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(m_Status.IPOctet2), sizeof(m_Status.IPOctet2));
	offset+= sizeof(m_Status.IPOctet2);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(m_Status.IPOctet3), sizeof(m_Status.IPOctet3));
	offset+= sizeof(m_Status.IPOctet3);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(m_Status.IPOctet4), sizeof(m_Status.IPOctet4));
	offset+= sizeof(m_Status.IPOctet4);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(m_Status.OSVerMajor), sizeof(m_Status.OSVerMajor));
	offset+= sizeof(m_Status.OSVerMajor);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(m_Status.OSVerMinor), sizeof(m_Status.OSVerMinor));
	offset+= sizeof(m_Status.OSVerMinor);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(m_Status.OSBuild), sizeof(m_Status.OSBuild));
	offset += sizeof(m_Status.OSBuild);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, &(m_Status.FeatureFlag), sizeof(m_Status.FeatureFlag));
	offset += sizeof(m_Status.FeatureFlag);

	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, m_Status.MGMDGlobalOption ? &(m_Status.MGMDAvailable) : &zeroByte, sizeof(m_Status.MGMDAvailable));
	offset += sizeof(m_Status.MGMDAvailable);

	byte size = 0;
	memcpy_s(m_lpBuffer + offset, m_bufferSize - offset, (m_Status.MGMDGlobalOption ? &debugFieldCountWithMgmd : &debugFieldCount), sizeof(byte)); // count
	offset += sizeof(byte);
	if (m_Status.MGMDGlobalOption)
	{
		WriteDebugData(offset, m_gameStart, m_Status.GameStart ? m_yes : m_no);
		WriteDebugData(offset, m_gameEnd, m_Status.GameEnd ? m_yes : m_no);
		WriteDebugData(offset, m_pollingSpeed, m_Status.PollingSpeed ? m_speeed200 : m_speed40);
		WriteDebugData(offset, m_mgmdsenoption, m_Status.MGMDSentinelOption ? m_yes : m_no);
	}

	WriteDebugData(offset, m_resolution, m_Status.resolution);
	WriteDebugData(offset, m_orientation, m_Status.landscape ? ORIENTATION_LANDSCAPE : ORIENTATION_PORTRAIT);
	WriteDebugData(offset, m_touchScreen, m_Status.touchScreen ? m_yes : m_no);
	WriteDebugData(offset, m_processorType, m_Status.processorType);
	WriteDebugData(offset, m_numberOfCores, m_Status.numberOfCores);
	WriteDebugData(offset, m_biosVersion, m_Status.biosVersion);
	WriteDebugData(offset, m_totalMemoryInKB, m_Status.totalMemoryInKB);
	WriteDebugData(offset, m_beaconCapability, m_Status.beaconCapability);
	WriteDebugData(offset, m_beaconFirmware, m_Status.beaconFirmware);
	WriteDebugData(offset, m_carrierFirmwareVersion, m_Status.carrierFirmwareVersion);
	WriteDebugData(offset, m_videoMixerFirmwareVersion, m_Status.videoMixerFirmwareVersion);

	/* Update the Packet size after the data has been filled */
	if (m_pollerPacketVersion >= 3)
	{
		MessageHeader.DataLength = (WORD)offset - SIZE_OF_MESSAGE_HEADER;
		memcpy(m_lpBuffer + dataLengthOffset, &(MessageHeader.DataLength), sizeof(MessageHeader.DataLength));
	}
	
	bufferSize = offset;
	return m_lpBuffer;
}

void CPollerStatusMessage::WriteDebugData(int& currentOffset, const std::string& keyString, const std::string& valueString)
{
	string tempValueString(valueString);
	if (tempValueString.size() > MAX_NUMBER_OF_VALUE_CHARS)
	{
		tempValueString = tempValueString.substr(0, MAX_NUMBER_OF_VALUE_CHARS);// Truncate to the first MAX_NUMBER_OF_VALUE_CHARS characters
	}
	byte size = keyString.size();
	memcpy_s(m_lpBuffer + currentOffset, m_bufferSize - currentOffset, &size, sizeof(byte)); // key size
	currentOffset += sizeof(byte);
	memcpy_s(m_lpBuffer + currentOffset, m_bufferSize - currentOffset, keyString.c_str(), size); // key
	currentOffset += size;
	size = tempValueString.size();
	memcpy_s(m_lpBuffer + currentOffset, m_bufferSize - currentOffset, &size, sizeof(byte)); // value size
	currentOffset += sizeof(byte);
	memcpy_s(m_lpBuffer + currentOffset, m_bufferSize - currentOffset, tempValueString.c_str(), size); // value
	currentOffset += size;
}