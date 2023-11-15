#include "TotalizerAnnouncementCommand.h"
#include "../ProgressiveProtocol/ProgressiveKeys.h"

using namespace std;
using namespace AwardType;
using namespace MethodOfPayment;

using namespace std;

/// \fn CTotalizerAnnouncementCommand::CTotalizerAnnouncementCommand(const string& assetTag, DWORD assetDuration, const CProgressiveTotalizerAnnouncement& announcment)
/// \brief Initializes a new instance of the <see cref="CTotalizerAnnouncementCommand"/> class
/// \param  assetTag The asset tag.
/// \param  assetDuration Duration of the asset.
/// \param  announcment The announcment.
CTotalizerAnnouncementCommand::CTotalizerAnnouncementCommand(DWORD assetDuration, const CProgressiveTotalizerAnnouncement& announcment, const string& dynamicContentPath) :
	CFullScreenAttractorCommand(assetDuration, "TotalizerAnnouncement", dynamicContentPath)
{
	//setup all variables...
	m_Id = announcment.m_Id;
	m_Name = announcment.m_Name;
	m_MetricName = announcment.m_MetricName;
	m_MetricValue = announcment.m_MetricValue;
}

/// \fn CTotalizerAnnouncementCommand::~CTotalizerAnnouncementCommand()
/// \brief Finalizes an instance of the <see cref="CTotalizerAnnouncementCommand"/> class
CTotalizerAnnouncementCommand::~CTotalizerAnnouncementCommand()
{
}

/// \fn void CTotalizerAnnouncementCommand::AddDynamicTags(JsonWriter& writer) const
/// \brief Adds a dynamic tags
/// \param [in,out] writer The writer.
void CTotalizerAnnouncementCommand::AddDynamicTags(JsonWriter& writer) const
{
	writer.Key("dynamicTags");
	writer.StartArray();

	writer.StartObject();
	writer.Key("name");		writer.String("AnnouncementId");
	writer.Key("value");	writer.Int64(m_Id);
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("AnnouncementName");
	writer.Key("value");	writer.String(m_Name);
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("MetricName");
	writer.Key("value");	writer.String(m_MetricName);
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("AwardDescription");
	writer.Key("value");	writer.String(CUtilities::GetFormattedNumberString(m_MetricValue));
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("TotalizerLine1");
	writer.Key("value");	writer.String("TotalizerLine1");
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("TotalizerLine2");
	writer.Key("value");	writer.String("TotalizerLine2");
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("AwardCurrency");
	writer.Key("value");	writer.String("AwardCurrency");
	writer.EndObject();

	writer.EndArray();
}