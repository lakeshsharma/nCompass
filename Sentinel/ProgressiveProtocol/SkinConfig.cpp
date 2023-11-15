#include "stdafx.h"
#include "SkinConfig.h"
#include "RapidJsonHelper.h"
#include <ProgressiveProtocol/ProgressiveKeys.h>

CSkinConfig::CSkinConfig(const rapidjson::Document& jsonDoc)
{
	m_id = JsonGetInt64Ex(jsonDoc, pKeySkinId);
	m_revision = JsonGetInt64Ex(jsonDoc, pKeySkinRevision);
}

DWORD CSkinConfig::GetId() const
{
	return m_id;
}

DWORD CSkinConfig::GetRevision() const
{
	return m_revision;
}