#pragma once
#include "stdafx.h"
#include "InternalMsgContent.h"
#include "ProgressiveKeys.h"
#include "PackageInfo.h"

typedef enum
{
	CAMPAIGN_ACTIVE_SUNDAY		= 0x01,
	CAMPAIGN_ACTIVE_MONDAY		= 0x02,
	CAMPAIGN_ACTIVE_TUESDAY		= 0x04,
	CAMPAIGN_ACTIVE_WEDNESDAY	= 0x08,
	CAMPAIGN_ACTIVE_THURSDAY	= 0x10,
	CAMPAIGN_ACTIVE_FRIDAY		= 0x20,
	CAMPAIGN_ACTIVE_SATURDAY	= 0x40,
}CampaignActiveDays;

typedef struct
{
	bool isEnabled;
	bool isImage;
	bool isMute;
	std::string filename;
	std::string url;
	std::string checksum;
	std::string beginDate;
	std::string beginTime;
	std::string endDate;
	std::string endTime;
	DWORD duration;
	DWORD sequenceId;
	DWORD activeDays;
}CampaignPlaylistItem;

typedef std::vector<std::unique_ptr<CampaignPlaylistItem>> CampaignPlaylistItems;

typedef struct
{
	std::string name;
	CampaignPlaylistItems items;
}CampaignPlaylist;

typedef struct
{
	std::string name;
	CampaignPlaylist playlist;
}CampaignEvent;

typedef std::vector<std::unique_ptr<CampaignEvent>> CampaignEvents;

typedef struct
{
	std::string name;
	DWORD width;
	DWORD height;
	CampaignEvents events;
}CampaignMediaPackage;

typedef std::vector<std::unique_ptr<CampaignMediaPackage>> CampaignMediaPackages;

class CCampaignPackage : public CInternalMsgContent<INT_MSG_CONTENT_CAMPAIGN_PACKAGE>
{
public:
	/// <summary>
	/// CCampaignPackage json deserialize constructor.
	/// </summary>
	/// <param name="jsonDoc">The json document to use for deserialization.</param>
	CCampaignPackage(const rapidjson::Document& jsonDoc);

	/// <summary>
	/// Destructor.
	/// </summary>
	~CCampaignPackage();

	/// <summary>
	/// Gets the campaign name.
	/// </summary>
	/// <returns>The campaign name</returns>
	std::string GetName() const;

	/// <summary>
	/// Gets the campaign id.
	/// </summary>
	/// <returns>The campaign id</returns>
	DWORD GetId() const;

	/// <summary>
	/// Gets the campaign revision.
	/// </summary>
	/// <returns>The campaign version</returns>
	DWORD GetRevision() const;

	/// <summary>
	/// Gets the base url.
	/// </summary>
	/// <returns>The base url</returns>
	std::string GetBaseUrl() const;

	/// <summary>
	/// Gets the campaign media packages.
	/// </summary>
	/// <returns>Campaign media packages</returns>
	const CampaignMediaPackages& GetCampaignMediaPackages() const;

	/// <summary>
	/// Gets the campaign info.
	/// </summary>
	/// <returns>The campaign info</returns>
	CPackageInfo GetCampaignInfo() const;

private:
	std::string m_name;
	DWORD m_id;
	DWORD m_revision;
	std::string m_baseUrl;
	CampaignMediaPackages m_campaignMediaPackages;
};
