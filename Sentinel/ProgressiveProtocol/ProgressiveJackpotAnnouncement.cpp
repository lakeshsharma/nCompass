#include "stdafx.h"
#include "ProgressiveJackpotAnnouncement.h"
#include "RapidJsonHelper.h"
#include "ProgressiveKeys.h"
#include "Util/Currency.h"

using namespace rapidjson;

/// \fn CProgressiveJackpotAnnouncement::CProgressiveJackpotAnnouncement(const Document& doc) : m_Id(JsonMemberExists(doc, pKeyId) ? JsonGetInt64Ex(doc, pKeyId) : -1), m_Name(JsonGetStringEx(doc, pKeyName)), m_Amount(JsonMemberExists(doc, pKeyAmount) ? JsonGetDoubleEx(doc, pKeyAmount)
/// \brief Initializes a new instance of the <see cref="CProgressiveJackpotAnnouncement"/> class
/// \param  doc The document.
CProgressiveJackpotAnnouncement::CProgressiveJackpotAnnouncement(const Document& doc) :
	m_Id(JsonGetInt64Ex(doc, pKeyId)),
	m_Name(JsonGetStringEx(doc, pKeyName)),
	m_Amount(Currency::ToString(JsonGetDoubleEx(doc, pKeyAmount))),
	m_PlayerId(-1),
	m_Venue(JsonGetOptionalStringEx(doc, pKeyVenue)),
	m_Bank(JsonGetOptionalStringEx(doc, pKeyBank)),
	m_AssetNumber(JsonGetOptionalStringEx(doc, pKeyAssetNumber))
{
	//The json struct has an object called PatronInformation. This parses data out of it if exists.
	auto patronMember = doc.FindMember(PatronInfo);
	if (patronMember != doc.MemberEnd() && patronMember->value.IsObject())
	{
		m_CardNumber = JsonGetOptionalStringEx(patronMember->value, pKeyCardNumber);
		m_PlayerId = JsonMemberExists(patronMember->value, pKeyPlayerId) ? JsonGetInt64Ex(patronMember->value, pKeyPlayerId) : -1;
		m_Tier = JsonGetOptionalStringEx(patronMember->value, pKeyTier);
		m_WinnerName = JsonGetOptionalStringEx(patronMember->value, pKeyName);
		m_WinnerFirstName = JsonGetOptionalStringEx(patronMember->value, pKeyFirstName);
		m_WinnerLastName = JsonGetOptionalStringEx(patronMember->value, pKeyLastName);
	}
}