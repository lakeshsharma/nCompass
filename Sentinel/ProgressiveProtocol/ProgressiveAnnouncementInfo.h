#pragma once
#include "stdafx.h"
#include "Logging/LogString.h"
#include "RapidJsonHelper.h"
#include "ProgressiveKeys.h"
#include "SlotsContent.h"

class CProgressiveAnnouncementInfo
{
protected:
	int64_t m_id;
	std::string m_name;
	ICardAnnouncementType::Type m_type;
	std::unique_ptr<CSlotsContentPackage> m_contentPackage;

	friend class CMediaPackageManager;

private:

public:
	CProgressiveAnnouncementInfo(int64_t id, ICardAnnouncementType::Type type, const rapidjson::Value& announcement);
	virtual ~CProgressiveAnnouncementInfo();
};