#include "JackpotAnnouncementCommand.h"
#include "../ProgressiveProtocol/ProgressiveKeys.h"

using namespace std;

/// \fn CJackpotAnnouncementCommand::CJackpotAnnouncementCommand(const string& assetTag, DWORD assetDuration, const CProgressiveJackpotAnnouncement& announcment)
/// \brief Initializes a new instance of the <see cref="CJackpotAnnouncementCommand"/> class
/// \param  assetTag The asset tag.
/// \param  assetDuration Duration of the asset.
/// \param  announcment The announcment.
CJackpotAnnouncementCommand::CJackpotAnnouncementCommand(DWORD assetDuration, const CProgressiveJackpotAnnouncement& announcment, const string& dynamicContentPath) :
	CFullScreenAttractorCommand(assetDuration, "JackpotAnnouncement", dynamicContentPath)
{
	//setup all variables...
	m_Id = announcment.m_Id;
	m_Name = announcment.m_Name;
	m_Amount = announcment.m_Amount;
	m_CardNumber = announcment.m_CardNumber;
	m_PlayerId = announcment.m_PlayerId;
	m_Tier = announcment.m_Tier;
	m_Venue = announcment.m_Venue;
	m_Bank = announcment.m_Bank;
	m_AssetNumber = announcment.m_AssetNumber;
	m_WinnerName = announcment.m_WinnerName;
	m_WinnerFirstName = announcment.m_WinnerFirstName;
	m_WinnerLastName = announcment.m_WinnerLastName;
}

/// \fn CJackpotAnnouncementCommand::~CJackpotAnnouncementCommand()
/// \brief Finalizes an instance of the <see cref="CJackpotAnnouncementCommand"/> class
CJackpotAnnouncementCommand::~CJackpotAnnouncementCommand()
{
}

/// \fn void CJackpotAnnouncementCommand::AddDynamicTags(JsonWriter &writer) const
/// \brief Adds a dynamic tags
/// \param [in,out] writer The writer.
void CJackpotAnnouncementCommand::AddDynamicTags(JsonWriter &writer) const
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
	writer.Key("name");		writer.String("AwardDescription");
	writer.Key("value");	writer.String(CUtilities::GetFormattedNumberString(m_Amount));
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("CardNumber");
	writer.Key("value");	writer.String(m_CardNumber);
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("WinnerPlayerId");
	writer.Key("value");	writer.Int64(m_PlayerId);
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("WinnerTier");
	writer.Key("value");	writer.String(m_Tier);
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("Venue");
	writer.Key("value");	writer.String(m_Venue);
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("Bank");
	writer.Key("value");	writer.String(m_Bank);
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("WinnerName");
	writer.Key("value");	writer.String(m_WinnerName);
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("WinnerFirstName");
	writer.Key("value");	writer.String(m_WinnerFirstName);
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("WinnerLastName");
	writer.Key("value");	writer.String(m_WinnerLastName);
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("AssetNumber");
	writer.Key("value");	writer.String(m_AssetNumber);
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("AwardCurrency");
	writer.Key("value");	writer.String("AwardCurrency");
	writer.EndObject();

	writer.EndArray();
}