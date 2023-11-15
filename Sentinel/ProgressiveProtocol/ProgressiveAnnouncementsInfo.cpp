#include "ProgressiveAnnouncementsInfo.h"

using namespace std;
using namespace rapidjson;


CProgressiveAnnouncementsInfo::CProgressiveAnnouncementsInfo(const Document& jsonDoc)
{
	Ctor_AddAnnouncements(jsonDoc);
}

void CProgressiveAnnouncementsInfo::Ctor_AddAnnouncements(const Document& jsonDoc)
{
	const rapidjson::Value& announcements = jsonDoc[pKeyAnnouncements];

	if (announcements.IsArray())
	{
		for (auto announcementIter = announcements.Begin(); announcementIter != announcements.End(); ++announcementIter)
		{
			int64_t id = JsonGetInt64Ex(*announcementIter, pKeyId);
			if (id > 0)
			{
				const char* pType = JsonGetStringEx(*announcementIter, pKeyType);
				ICardAnnouncementType::Type type = ICardAnnouncementType::emap.At(pType);
				if (type != ICardAnnouncementType::Invalid)
				{
					unique_ptr<CProgressiveAnnouncementInfo>& announcement = m_announcements[id];
					if (announcement)
					{
						ostringstream sStream;
						sStream << "Announcement Id " << id << " is a duplicate.";
						throw SentinelException(sStream.str().c_str());
					}
					announcement.reset(new CProgressiveAnnouncementInfo(id, type, *announcementIter));
				}
			}
		}
	}
}