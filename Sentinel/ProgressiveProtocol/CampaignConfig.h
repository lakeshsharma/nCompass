#pragma once
#include "stdafx.h"
#include "InternalMsgContent.h"

class CCampaignConfig : public CInternalMsgContent<INT_MSG_CONTENT_CAMPAIGN_VERIFICATION>
{
public:
	/// <summary>
	/// CCampaignConfig json deserialize constructor.
	/// </summary>
	/// <param name="jsonDoc">The json document to use for deserialization.</param>
	CCampaignConfig(const rapidjson::Document& jsonDoc);

	/// <summary>
	/// Gets the campaign id.
	/// </summary>
	/// <returns>The campaign id</returns>
	DWORD GetId() const;

	/// <summary>
	/// Gets the campaign revision.
	/// </summary>
	/// <returns>The campaign revision</returns>
	DWORD GetRevision() const;

private:
	DWORD m_id;
	DWORD m_revision;
};

