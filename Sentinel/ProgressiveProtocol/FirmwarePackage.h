#pragma once
#include "stdafx.h"
#include "InternalMsgContent.h"
#include "ProgressiveKeys.h"

struct FirmwareFile
{
	std::string m_filename;
	std::string m_downloadURL;
	std::string m_platform;
	std::string m_version;
	std::string m_checksum;
};

typedef std::vector<std::unique_ptr<FirmwareFile>> FirmwareFiles;

class CFirmwarePackage : public CInternalMsgContent<INT_MSG_CONTENT_FIRMWARE_PACKAGE>
{
public:
	/// <summary>
	/// CFirmwarePackage json deserialize constructor.
	/// </summary>
	/// <param name="jsonDoc">The json document to use for deserialization.</param>
	CFirmwarePackage(const rapidjson::Document& jsonDoc, bool isICardConfiguration = false);

	/// <summary>
	/// Destructor.
	/// </summary>
	~CFirmwarePackage();

	/// <summary>
	/// Gets the firmware files vector.
	/// </summary>
	/// <returns>Firmware files vector</returns>
	const FirmwareFiles& GetFirmwareFiles() const;

private:
	FirmwareFiles m_firmwareFiles;
};
