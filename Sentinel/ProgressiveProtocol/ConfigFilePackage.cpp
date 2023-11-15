#include "stdafx.h"
#include "ConfigFilePackage.h"
#include "RapidJsonHelper.h"

using namespace rapidjson;


CConfigFilePackage::CConfigFilePackage(const Document& jsonDoc) : 
	m_messageId(JsonGetStringEx(jsonDoc, pKeyMessageId)),
	m_filename(JsonGetStringEx(jsonDoc, pKeyConfigFilename)),
	m_url(JsonGetStringEx(jsonDoc, pKeyConfigURL)),
	m_checksum(JsonGetStringEx(jsonDoc, pKeyConfigChecksum))
{
}

CConfigFilePackage::~CConfigFilePackage()
{
}

std::string CConfigFilePackage::GetMessageId() const
{
	return m_messageId;
}

std::string CConfigFilePackage::GetFilename() const
{
	return m_filename;
}

std::string CConfigFilePackage::GetUrl() const
{
	return m_url;
}

std::string CConfigFilePackage::GetChecksum() const
{
	return m_checksum;
}