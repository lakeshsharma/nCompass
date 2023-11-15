#pragma once

#include "stdafx.h"
#include "InternalMsgContent.h"
#include "ProgressiveKeys.h"
#include "ProgressiveAnnouncementInfo.h"
#include "Logging/LogString.h"
#include "RapidJsonHelper.h"

class CProgressiveAnnouncementsInfo : public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_ANNOUNCEMENT_CONFIGURATION>
{
protected:
	std::map<int64_t, std::unique_ptr<CProgressiveAnnouncementInfo>> m_announcements;

	friend class CMediaPackageManager;

private:
	// These methods are only to be used by the Constructor
	void Ctor_AddAnnouncements(const rapidjson::Document& jsonDoc);

public:
	CProgressiveAnnouncementsInfo(const rapidjson::Document& jsonDoc);
};