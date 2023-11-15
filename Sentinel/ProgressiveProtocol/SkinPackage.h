#pragma once
#include "stdafx.h"
#include "InternalMsgContent.h"
#include "ProgressiveKeys.h"
#include "PackageInfo.h"

typedef struct
{
	std::string name;
	DWORD width;
	DWORD height;
}SkinResolution;

typedef struct
{
	DWORD id;
	std::string name;
	std::string code;
	std::string checksum;
	std::string url;
}SkinLanguangePack;

typedef std::vector<std::unique_ptr<SkinLanguangePack>> SkinLanguangePacks;

typedef struct
{
	std::string filename;
	std::string checksum;
	std::string url;
}SkinFile;

typedef std::vector<std::unique_ptr<SkinFile>> SkinFiles;

class CSkinPackage : public CInternalMsgContent<INT_MSG_CONTENT_SKIN_PACKAGE>
{
public:
	/// <summary>
	/// CSkinPackage json deserialize constructor.
	/// </summary>
	/// <param name="jsonDoc">The json document to use for deserialization.</param>
	CSkinPackage(const rapidjson::Document& jsonDoc);

	/// <summary>
	/// Destructor.
	/// </summary>
	~CSkinPackage();

	/// <summary>
	/// Returns whether this skin package instance is valid or not.
	/// </summary>
	/// <returns>true if valid, else false</returns>
	bool IsValid() const;

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

	/// <summary>
	/// Gets the skin name.
	/// </summary>
	/// <returns>The skin name</returns>
	std::string GetName() const;

	/// <summary>
	/// Gets the skin version.
	/// </summary>
	/// <returns>The skin version</returns>
	std::string GetVersion() const;

	/// <summary>
	/// Gets the skin url.
	/// </summary>
	/// <returns>The skin url</returns>
	std::string GetSkinUrl() const;

	/// <summary>
	/// Gets the skin checksum.
	/// </summary>
	/// <returns>The skin checksum</returns>
	std::string GetSkinChecksum() const;

	/// <summary>
	/// Gets the base url.
	/// </summary>
	/// <returns>The base url</returns>
	std::string GetBaseUrl() const;

	/// <summary>
	/// Gets the skin resolution.
	/// </summary>
	/// <returns>The skin resolution</returns>
	SkinResolution GetResolution() const;

	/// <summary>
	/// Gets the skin language packs.
	/// </summary>
	/// <returns>The skin language packs</returns>
	const SkinLanguangePacks& GetLanguagePacks() const;

	/// <summary>
	/// Gets the skin files.
	/// </summary>
	/// <returns>The skin files</returns>
	const SkinFiles& GetFiles() const;

	/// <summary>
	/// Gets the skin info.
	/// </summary>
	/// <returns>The skin info</returns>
	CPackageInfo GetSkinInfo() const;

private:
	bool m_isValid;
	DWORD m_id;
	DWORD m_skinRevision;
	std::string m_name;
	std::string m_version;
	std::string m_skinUrl;
	std::string m_skinChecksum;
	std::string m_baseUrl;
	SkinResolution m_resolution;
	SkinLanguangePacks m_languagePacks;
	SkinFiles m_files;
};
