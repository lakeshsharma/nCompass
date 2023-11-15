#pragma once

#include "InternalMsgContent.h"
using namespace rapidjson;

/// \class CProgressiveJackpotAnnouncement
/// \brief A progressive jackpot announcement.
class CProgressiveJackpotAnnouncement : public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_JACKPOT_ANNOUNCEMENT>
{
private:
	const std::string PatronInfo = "PatronInformation";
public:

	/// \fn CProgressiveJackpotAnnouncement::CProgressiveJackpotAnnouncement(const Document& doc);
	/// \brief Initializes a new instance of the <see cref="CProgressiveJackpotAnnouncement"/> class
	/// \param  doc The document.
	CProgressiveJackpotAnnouncement(const Document& doc);
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

