#pragma once
#include "stdafx.h"
#include "InternalMsgContent.h"

class CSkinConfig : public CInternalMsgContent<INT_MSG_CONTENT_SKIN_VERIFICATION>
{
public:
	/// <summary>
	/// CSkinConfig json deserialize constructor.
	/// </summary>
	/// <param name="jsonDoc">The json document to use for deserialization.</param>
	CSkinConfig(const rapidjson::Document& jsonDoc);

	/// <summary>
	/// Gets the skin id.
	/// </summary>
	/// <returns>The skin id</returns>
	DWORD GetId() const;

	/// <summary>
	/// Gets the skin revision.
	/// </summary>
	/// <returns>The skin revision</returns>
	DWORD GetRevision() const;

private:
	DWORD m_id;
	DWORD m_revision;
};

