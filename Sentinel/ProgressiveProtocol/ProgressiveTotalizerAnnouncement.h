#pragma once

#include "InternalMsgContent.h"
using namespace rapidjson;

/// \class CProgressiveTotalizerAnnouncement
/// \brief A progressive totalizer announcement.
class CProgressiveTotalizerAnnouncement : public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_TOTALIZER_ANNOUNCEMENT>
{
public:

	/// \fn CProgressiveTotalizerAnnouncement::CProgressiveTotalizerAnnouncement(const Document& doc);
	/// \brief Initializes a new instance of the <see cref="CProgressiveTotalizerAnnouncement"/> class
	/// \param  doc The document.
	CProgressiveTotalizerAnnouncement(const Document& doc);
	int64_t m_Id;
	std::string m_Name;
	std::string m_MetricName;
	std::string m_MetricValue;
};

