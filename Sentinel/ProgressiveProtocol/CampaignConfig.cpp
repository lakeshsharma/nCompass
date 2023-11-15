#include "stdafx.h"
#include "CampaignConfig.h"
#include "RapidJsonHelper.h"
#include <ProgressiveProtocol/ProgressiveKeys.h>

CCampaignConfig::CCampaignConfig(const rapidjson::Document& jsonDoc)
{
	m_id = JsonGetInt64Ex(jsonDoc, pKeyCampaignId);
	m_revision = JsonGetInt64Ex(jsonDoc, pKeyCampaignRevision);
}

DWORD CCampaignConfig::GetId() const
{
	return m_id;
}

DWORD CCampaignConfig::GetRevision() const
{
	return m_revision;
}