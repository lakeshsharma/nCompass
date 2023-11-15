#include "stdafx.h"
#include "FirmwarePackage.h"
#include "RapidJsonHelper.h"
#include "Logging/LogString.h"

using namespace rapidjson;

CFirmwarePackage::CFirmwarePackage(const Document& jsonDoc, bool isICardConfiguration)
{
	//The firmware can come from different places. If it comes from ICardConfiguration5 then
	// the key is "AssignedFirmware". If it comes from other it is "FirwareFiles".
	char const* pKeyToUse = pKeyFirmwareFiles;
	if (isICardConfiguration)
	{
		pKeyToUse = pKeyAssignedFirmware;
	}
	
	const rapidjson::Value& files = jsonDoc[pKeyToUse];
	if (files.IsArray())
	{
		for (auto it = files.Begin(); it != files.End(); ++it)
		{
			std::unique_ptr<FirmwareFile> file(new FirmwareFile());
			file->m_filename = JsonGetStringEx(*it, pKeyFirmwareFileName);
			file->m_downloadURL = JsonGetStringEx(*it, pKeyFirmwareFileDownloadURL);
			file->m_platform = JsonGetStringEx(*it, pKeyPlatform);
			file->m_version = JsonGetStringEx(*it, pKeyFirmwareFileVersion);
			file->m_checksum = JsonGetStringEx(*it, pKeyFirmwareFileChecksum);

			m_firmwareFiles.push_back(std::move(file));
		}
	}
}

CFirmwarePackage::~CFirmwarePackage()
{
}

const FirmwareFiles& CFirmwarePackage::GetFirmwareFiles() const
{
	return m_firmwareFiles;
}