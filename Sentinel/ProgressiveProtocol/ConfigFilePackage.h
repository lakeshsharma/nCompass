#pragma once
#include "stdafx.h"
#include "InternalMsgContent.h"
#include "ProgressiveKeys.h"


class CConfigFilePackage : public CInternalMsgContent<INT_MSG_CONTENT_MISC_FILES_PACKAGE>
{
public:
	/// <summary>
	/// CConfigFilePackage json deserialize constructor.
	/// </summary>
	/// <param name="jsonDoc">The json document to use for deserialization.</param>
	CConfigFilePackage(const rapidjson::Document& jsonDoc);

	/// <summary>
	/// Destructor.
	/// </summary>
	~CConfigFilePackage();

	/// <summary>
	/// Gets the message id.
	/// </summary>
	/// <returns>The message id</returns>
	std::string GetMessageId() const;

	/// <summary>
	/// Gets the filename.
	/// </summary>
	/// <returns>The filename</returns>
	std::string GetFilename() const;

	/// <summary>
	/// Gets the url.
	/// </summary>
	/// <returns>The url</returns>
	std::string GetUrl() const;

	/// <summary>
	/// Gets the checksum.
	/// </summary>
	/// <returns>The checksum</returns>
	std::string GetChecksum() const;

private:
	std::string m_messageId;
	std::string m_filename;
	std::string m_url;
	std::string m_checksum;
};
