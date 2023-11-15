#include "stdafx.h"
#include "CampaignPackage.h"
#include "RapidJsonHelper.h"

using namespace rapidjson;

CCampaignPackage::CCampaignPackage(const Document& jsonDoc)
{
	const rapidjson::Value& campaign = jsonDoc[pKeyCampaign];
	m_name = JsonGetStringEx(campaign, pKeyName);
	m_id = JsonGetInt64Ex(campaign, pKeyId);
	m_revision = JsonGetInt64Ex(campaign, pKeyCampaignVersion);
	m_baseUrl = JsonGetStringEx(jsonDoc, pKeyCampaignBaseURL);

	const rapidjson::Value& campaignMediaPackages = campaign[pKeyCampaignMediaPackages];

	if (campaignMediaPackages.IsArray())
	{
		for (auto itr = campaignMediaPackages.Begin(); itr != campaignMediaPackages.End(); ++itr)
		{
			std::unique_ptr<CampaignMediaPackage> campaignMediaPackage(new CampaignMediaPackage());
			campaignMediaPackage->name = JsonGetStringEx(*itr, pKeyName);

			const rapidjson::Value& resolution = JsonGetObjectEx(*itr, pKeyResolution);
			campaignMediaPackage->width = JsonGetInt64Ex(resolution, pKeyResolutionWidth);
			campaignMediaPackage->height = JsonGetInt64Ex(resolution, pKeyResolutionHeight);

#ifndef _UNIT_TEST
			// verify current device resolution
			DWORD currentWidth = 0;
			DWORD currentHeight = 0;
			CUtilities::GetResolution(currentWidth, currentHeight);
			if (currentWidth == campaignMediaPackage->width && currentHeight == campaignMediaPackage->height)
			{
#endif // !_UNIT_TEST

				const rapidjson::Value& events = JsonGetArrayEx(*itr, pKeyCampaignEventAssignments);

				if (events.IsArray())
				{
					for (auto eventItr = events.Begin(); eventItr != events.End(); ++eventItr)
					{
						std::unique_ptr<CampaignEvent> campaignEvent(new CampaignEvent());
						campaignEvent->name = JsonGetStringEx(*eventItr, pKeyCampaignEvent);

						const rapidjson::Value& playlist = JsonGetObjectEx(*eventItr, pKeyCampaignPlaylist);

						campaignEvent->playlist.name = JsonGetStringEx(playlist, pKeyName);

						const rapidjson::Value& items = JsonGetArrayEx(playlist, pKeyCampaignPlaylistItems);

						if (items.IsArray())
						{
							for (auto itemItr = items.Begin(); itemItr != items.End(); ++itemItr)
							{
								std::unique_ptr<CampaignPlaylistItem> playlistItem(new CampaignPlaylistItem());
								playlistItem->activeDays = 0;
								if (JsonGetBoolEx(*itemItr, pKeyCampaignPlaylistItem_ActiveSunday))
								{
									playlistItem->activeDays = CampaignActiveDays::CAMPAIGN_ACTIVE_SUNDAY;
								}
								if (JsonGetBoolEx(*itemItr, pKeyCampaignPlaylistItem_ActiveMonday))
								{
									playlistItem->activeDays |= CampaignActiveDays::CAMPAIGN_ACTIVE_MONDAY;
								}
								if (JsonGetBoolEx(*itemItr, pKeyCampaignPlaylistItem_ActiveTuesday))
								{
									playlistItem->activeDays |= CampaignActiveDays::CAMPAIGN_ACTIVE_TUESDAY;
								}
								if (JsonGetBoolEx(*itemItr, pKeyCampaignPlaylistItem_ActiveWednesday))
								{
									playlistItem->activeDays |= CampaignActiveDays::CAMPAIGN_ACTIVE_WEDNESDAY;
								}
								if (JsonGetBoolEx(*itemItr, pKeyCampaignPlaylistItem_ActiveThursday))
								{
									playlistItem->activeDays |= CampaignActiveDays::CAMPAIGN_ACTIVE_THURSDAY;
								}
								if (JsonGetBoolEx(*itemItr, pKeyCampaignPlaylistItem_ActiveFriday))
								{
									playlistItem->activeDays |= CampaignActiveDays::CAMPAIGN_ACTIVE_FRIDAY;
								}
								if (JsonGetBoolEx(*itemItr, pKeyCampaignPlaylistItem_ActiveSaturday))
								{
									playlistItem->activeDays |= CampaignActiveDays::CAMPAIGN_ACTIVE_SATURDAY;
								}
								playlistItem->isEnabled = JsonGetBoolEx(*itemItr, pKeyCampaignPlaylistItem_Enabled);
								playlistItem->isImage = JsonGetBoolEx(*itemItr, pKeyCampaignPlaylistItem_Image);
								playlistItem->isMute = JsonGetBoolEx(*itemItr, pKeyCampaignPlaylistItem_Mute);
								playlistItem->beginDate = JsonGetStringEx(*itemItr, pKeyCampaignPlaylistItem_BeginDate);
								playlistItem->beginTime = JsonGetStringEx(*itemItr, pKeyCampaignPlaylistItem_BeginTime);
								if (JsonMemberExists(*itemItr, pKeyCampaignPlaylistItem_EndDate))
								{
									playlistItem->endDate = JsonGetStringEx(*itemItr, pKeyCampaignPlaylistItem_EndDate);
								}
								playlistItem->endTime = JsonGetStringEx(*itemItr, pKeyCampaignPlaylistItem_EndTime);
								playlistItem->duration = JsonGetInt64Ex(*itemItr, pKeyCampaignPlaylistItem_DurationMilliSec);
								playlistItem->sequenceId = JsonGetInt64Ex(*itemItr, pKeyCampaignPlaylistItem_SequenceId);

								const rapidjson::Value& content = JsonGetObjectEx(*itemItr, pKeyCampaignPlaylistItem_Content);
								playlistItem->filename = JsonGetStringEx(content, pKeyCampaignPlaylistItem_Filename);
								playlistItem->url = m_baseUrl + "/" + CUtilities::UrlEncode(playlistItem->filename);
								playlistItem->checksum = JsonGetStringEx(content, pKeyCampaignPlaylistItem_Checksum);

								campaignEvent->playlist.items.push_back(std::move(playlistItem));
							}
						}

						campaignMediaPackage->events.push_back(std::move(campaignEvent));
					}
				}
#ifndef _UNIT_TEST
			}
#endif // !_UNIT_TEST


			m_campaignMediaPackages.push_back(std::move(campaignMediaPackage));
		}
	}
}

CCampaignPackage::~CCampaignPackage()
{
}

std::string CCampaignPackage::GetName() const
{
	return m_name;
}

DWORD CCampaignPackage::GetId() const
{
	return m_id;
}

DWORD CCampaignPackage::GetRevision() const
{
	return m_revision;
}

std::string CCampaignPackage::GetBaseUrl() const
{
	return m_baseUrl;
}

const CampaignMediaPackages& CCampaignPackage::GetCampaignMediaPackages() const
{
	return m_campaignMediaPackages;
}

CPackageInfo CCampaignPackage::GetCampaignInfo() const
{
	return CPackageInfo(m_id, m_name, "", m_revision);
}