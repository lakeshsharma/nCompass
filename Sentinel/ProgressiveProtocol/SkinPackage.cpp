#include "stdafx.h"
#include "SkinPackage.h"
#include "RapidJsonHelper.h"

using namespace rapidjson;

CSkinPackage::CSkinPackage(const Document& jsonDoc)
{
	const rapidjson::Value& skin = jsonDoc[pKeySkin];

	// read resolution
	const rapidjson::Value& resolution = skin[pKeyResolution];
	m_resolution.name = JsonGetStringEx(resolution, pKeyName);
	m_resolution.width = JsonGetInt64Ex(resolution, pKeyResolutionWidth);
	m_resolution.height = JsonGetInt64Ex(resolution, pKeyResolutionHeight);

#ifndef _UNIT_TEST
	// verify the resolution of current device.
	DWORD width = 0, height = 0;
	CUtilities::GetResolution(width, height);
	if (width == m_resolution.width && height == m_resolution.height)
	{
#endif // !_UNIT_TEST
		m_isValid = true;

		// read remaining data

		m_id = JsonGetInt64Ex(skin, pKeyId);
		m_skinRevision = JsonGetInt64Ex(skin, pKeySkinRevision);
		m_name = JsonGetStringEx(skin, pKeyName);
		m_version = JsonGetStringEx(skin, pKeySkinVersion);
		m_skinChecksum = JsonGetStringEx(skin, pKeySkinChecksum);
		m_baseUrl = JsonGetStringEx(skin, pKeySkinBaseURL);
		m_skinUrl = m_baseUrl + "/skins/" + std::to_string(m_id) + "/" + CUtilities::UrlEncode(m_name) + ".json";

		const rapidjson::Value& languagePacks = skin[pKeySkinLanguagePacks];
		if (languagePacks.IsArray())
		{
			for (auto itr = languagePacks.Begin(); itr != languagePacks.End(); ++itr)
			{
				std::unique_ptr<SkinLanguangePack> languagePack(new SkinLanguangePack());
				languagePack->id = JsonGetInt64Ex(*itr, pKeyId);
				languagePack->name = JsonGetStringEx(*itr, pKeyName);
				languagePack->code = JsonGetStringEx(*itr, pKeySkinLanguagePack_Code);
				languagePack->checksum = JsonGetStringEx(*itr, pKeySkinLanguagePack_Checksum);
				languagePack->url = m_baseUrl + "/skins/" + std::to_string(m_id) + "/" + languagePack->code + ".json";

				m_languagePacks.push_back(std::move(languagePack));
			}
		}

		const rapidjson::Value& files = skin[pKeySkinFiles];
		if (files.IsArray())
		{
			for (auto itr = files.Begin(); itr != files.End(); ++itr)
			{
				std::unique_ptr<SkinFile> file(new SkinFile());
				file->filename = JsonGetStringEx(*itr, pKeySkinFile_Filename);
				file->checksum = JsonGetStringEx(*itr, pKeySkinFile_Checksum);
				file->url = m_baseUrl + "/original/" + CUtilities::UrlEncode(file->filename);

				m_files.push_back(std::move(file));
			}
		}
#ifndef _UNIT_TEST
	}
	else
	{
		m_isValid = false;
	}
#endif // !_UNIT_TEST
}

CSkinPackage::~CSkinPackage()
{
}

bool CSkinPackage::IsValid() const
{
	return m_isValid;
}

DWORD CSkinPackage::GetId() const
{
	return m_id;
}

DWORD CSkinPackage::GetRevision() const
{
	return m_skinRevision;
}

std::string CSkinPackage::GetName() const
{
	return m_name;
}

std::string CSkinPackage::GetVersion() const
{
	return m_version;
}

std::string CSkinPackage::GetSkinUrl() const
{
	return m_skinUrl;
}

std::string CSkinPackage::GetSkinChecksum() const
{
	return m_skinChecksum;
}

std::string CSkinPackage::GetBaseUrl() const
{
	return m_baseUrl;
}

SkinResolution CSkinPackage::GetResolution() const
{
	return m_resolution;
}

const SkinLanguangePacks& CSkinPackage::GetLanguagePacks() const
{
	return m_languagePacks;
}

const SkinFiles& CSkinPackage::GetFiles() const
{
	return m_files;
}

CPackageInfo CSkinPackage::GetSkinInfo() const
{
	return CPackageInfo(m_id, m_name, m_version, m_skinRevision);
}