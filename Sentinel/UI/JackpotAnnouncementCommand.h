#pragma once

#include <string>
#include "GlobalDefs.h"
#include "FullScreenAttractorCommand.h"
#include "ProgressiveProtocol/ProgressiveJackpotAnnouncement.h"

/// \class CJackpotAnnouncementCommand
/// \brief A jackpot announcement command.
class CJackpotAnnouncementCommand : public CFullScreenAttractorCommand
{
public:

	/// \fn CJackpotAnnouncementCommand::CJackpotAnnouncementCommand(const std::string& assetTag, DWORD assetDuration, const CProgressiveJackpotAnnouncement& announcment);
	/// \brief Initializes a new instance of the <see cref="CJackpotAnnouncementCommand"/> class
	/// \param  assetDuration Duration of the asset.
	/// \param  announcment The announcment.
	CJackpotAnnouncementCommand(DWORD assetDuration, const CProgressiveJackpotAnnouncement& announcment, const std::string& dynamicContentPath);

	/// \fn virtual CJackpotAnnouncementCommand::~CJackpotAnnouncementCommand();
	/// \brief Finalizes an instance of the <see cref="CJackpotAnnouncementCommand"/> class
	virtual ~CJackpotAnnouncementCommand();

	/// \fn virtual void CJackpotAnnouncementCommand::AddDynamicTags(JsonWriter &writer) const;
	/// \brief Adds a dynamic tags
	/// \param [in,out] writer The writer.
	virtual void AddDynamicTags(JsonWriter &writer) const;

private:
	int64_t m_Id;
	std::string m_Name;
	std::string m_Amount;
	std::string m_CardNumber;
	int64_t m_PlayerId;
	std::string m_Tier;
	std::string m_Venue;
	std::string m_Bank;
	std::string m_AssetNumber;
	std::string m_WinnerName;
	std::string m_WinnerFirstName;
	std::string m_WinnerLastName;
};