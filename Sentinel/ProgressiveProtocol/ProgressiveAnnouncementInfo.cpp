#include "ProgressiveAnnouncementInfo.h"

CProgressiveAnnouncementInfo::CProgressiveAnnouncementInfo(int64_t id, ICardAnnouncementType::Type type, const rapidjson::Value& announcement)
{
	m_id = id;
	m_name = JsonGetString(announcement, pKeyName, "");
	m_type = type;

	const rapidjson::Value& contentPackage = announcement[pKeyAnnouncementContent];
	if (contentPackage.IsObject())
	{
		m_contentPackage.reset(new CSlotsContentPackage(contentPackage, m_id, 
			(type == ICardAnnouncementType::Type::JackpotAnnouncement) ? SlotsContentType::Type::JackpotAnnouncement : SlotsContentType::Type::TotalizerAnnouncement));
	}
	else
	{
		m_contentPackage.release();
	}
}

CProgressiveAnnouncementInfo::~CProgressiveAnnouncementInfo()
{
}