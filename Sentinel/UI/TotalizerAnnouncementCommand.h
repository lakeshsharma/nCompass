#pragma once

#include <string>
#include "GlobalDefs.h"
#include "FullScreenAttractorCommand.h"
#include "../ProgressiveProtocol/ProgressiveTotalizerAnnouncement.h"

/// \class CTotalizerAnnouncementCommand
/// \brief A totalizer announcement command.
class CTotalizerAnnouncementCommand : public CFullScreenAttractorCommand
{
public:

	/// \fn CTotalizerAnnouncementCommand::CTotalizerAnnouncementCommand(const std::string& assetTag, DWORD assetDuration, const CProgressiveTotalizerAnnouncement& announcment);
	/// \brief Initializes a new instance of the <see cref="CTotalizerAnnouncementCommand"/> class
	/// \param  assetDuration Duration of the asset.
	/// \param  announcment The announcment.
	CTotalizerAnnouncementCommand(DWORD assetDuration,
		const CProgressiveTotalizerAnnouncement& announcment, const std::string& dynamicContentPath);

	/// \fn virtual CTotalizerAnnouncementCommand::~CTotalizerAnnouncementCommand();
	/// \brief Finalizes an instance of the <see cref="CTotalizerAnnouncementCommand"/> class
	virtual ~CTotalizerAnnouncementCommand();

	/// \fn virtual void CTotalizerAnnouncementCommand::AddDynamicTags(JsonWriter& writer) const;
	/// \brief Adds a dynamic tags
	/// \param [in,out] writer The writer.
	virtual void AddDynamicTags(JsonWriter& writer) const;

private:
	int64_t m_Id;
	std::string m_Name;
	std::string m_MetricName;
	std::string m_MetricValue;
};