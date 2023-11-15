#include "stdafx.h"
#include <dirent.h>

#include "FileDownloader.h"
#include "Logging/LogString.h"
#include "MediaPackageManager.h"
#include "ProgressiveProtocol/ProgressiveAnnouncementsInfo.h"
#include "ProgressiveProtocol/ProgressiveLevelsInfo.h"
#include "ProgressiveProtocol/SlotsContent.h"
#include "Utilities.h"
#include "Hardware/WatchDog.h"
#include "DirectoryManager.h"

using namespace std;
const string DEFAULT_ONELINK_CONTENT_INFO("No promotional content from OneLink.");
const string BASE_DYNAMIC_CONTENT_URL(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_URL_ONELINK_CONTENT));
const std::string ONELINK_CONTENT_PATH = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_ONELINK_CONTENT) + "/";
const std::string ONELINK_DYNAMIC_CONTENT_PATH = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_DYNAMIC_CONTENT) + "/";
const std::string ACTIVE_CAMPAIGN_PATH(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_CUSTOM_DYNAMIC_CONTENT) + "/Campaign");
const std::string PENDING_CAMPAIGN_PATH(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_CUSTOM_DYNAMIC_CONTENT) + "/tempCampaign");
const std::string CAMPAIGN_INFO_FILENAME("CampaignInfo.json");
const std::string PENDING_SKIN_PATH(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_ONELINK_CONTENTTEMP) + "/tempSkin/");

CMediaPackageManager *CMediaPackageManager::m_instance = nullptr;
recursive_mutex CMediaPackageManager::m_instanceCS;

const int DELETE_CONTENT_FILES_IN_SECONDS(120);//2 minutes

CMediaPackageManager::CMediaPackageManager() :
m_onelinkPromotionalContentInformation(DEFAULT_ONELINK_CONTENT_INFO),
m_pActiveCampaignInfo(nullptr),
m_pPendingCampaignInfo(nullptr),
m_pActiveSkinInfo(nullptr),
m_pPendingSkinInfo(nullptr),
m_cleanupOldContentFileTime(0)
{
}

CMediaPackageManager & CMediaPackageManager::Instance()
{
	lock_guard<recursive_mutex> lock(m_instanceCS);
	if (m_instance == nullptr)
	{
		m_instance = new CMediaPackageManager();
	}
	return *m_instance;
}

CMediaPackageManager::~CMediaPackageManager(void)
{
}

SlotsContentType::Type CMediaPackageManager::GetPteSlotsContentType(InternalMsgType msgType) const
{
	SlotsContentType::Type type(SlotsContentType::Invalid);

	switch (msgType)
	{
	case INT_MSG_DISPLAY_PTE_ANTICIPATION:
		type = SlotsContentType::Anticipation;
		break;
	case INT_MSG_DISPLAY_PTE_AWARD:
		type = SlotsContentType::Anticipation;
		break;
	case INT_MSG_DISPLAY_PTE_ENDCOUNTDOWN:
		type = SlotsContentType::PTE_EndingSoon;
		break;
	case INT_MSG_DISPLAY_PTE_LEADERBOARD:
		type = SlotsContentType::PTE_Leaderboard;
		break;
	case INT_MSG_DISPLAY_PTE_LEADERBOARD_PLAYER_INITIATED:
		type = SlotsContentType::PTE_Leaderboard;
		break;
	case INT_MSG_DISPLAY_PTE_LEVELUP:
		type = SlotsContentType::PTE_LevelUnlocked;
		break;
	case INT_MSG_DISPLAY_PTE_RESULTS:
		type = SlotsContentType::PTE_FinalResults;
		break;
	default:
		break;
	}

	return type;
}

string CMediaPackageManager::GetDynamicContentPath(int64_t id, const string& minorAwardId, SlotsContentType::Type type, const string& orientation)
{
	string dynamicContentPath;

	if (INVALID_VALUE != id)
	{
		string key(CSlotsContentFile::GetKey(to_string(id), minorAwardId, SlotsContentType::emap.At(type), orientation));

		if (type != SlotsContentType::Odometer)
		{//this prevents the streaming of logs
			LogString(ONELINKCONTENT_LOG, "GetDynamicContentPath key=%s", key.c_str());
		}		

		std::unique_lock<std::recursive_mutex> criticalSection(m_criticalSection);
		auto contentFile(m_contentFiles.find(key));
		if(contentFile != m_contentFiles.end())
		{
			dynamicContentPath.append(BASE_DYNAMIC_CONTENT_URL);
			dynamicContentPath.append(*contentFile);
			dynamicContentPath.append("/");
			if (type != SlotsContentType::Odometer)
			{//this prevents the streaming of logs
				LogString(ONELINKCONTENT_LOG, "dynamicContentPath=%s found from the current list of content files.", dynamicContentPath.c_str());
			}
		}
		else
		{
			auto contentFile(m_contentFilesToDelete.find(key));
			if (contentFile != m_contentFilesToDelete.end())
			{
				dynamicContentPath.append(BASE_DYNAMIC_CONTENT_URL);
				dynamicContentPath.append(*contentFile);
				dynamicContentPath.append("/");
				if (type != SlotsContentType::Odometer)
				{//this prevents the streaming of logs
					LogString(ONELINKCONTENT_LOG, "dynamicContentPath=%s found in the soon to be deleted list of content files.", dynamicContentPath.c_str());
				}
			}
		}
	}

	return dynamicContentPath;
}

void CMediaPackageManager::SetOneLinkContentFiles(const ProgressiveLevelsInfo* pLevelsInfo, const CProgressiveAnnouncementsInfo* pProgressiveAnnouncementsInfo)
{
	std::unique_lock<std::recursive_mutex> criticalSection(m_criticalSection);

	m_slotsContentUrl = pLevelsInfo->GetSlotsContentUrl();
	LogString(ONELINKCONTENT_LOG, "SetOneLinkContentFiles m_slotsContentUrl=%s", m_slotsContentUrl.c_str());
	m_contentFiles.clear();
	m_listOfContentFiles.clear();
	map<string, bool> filesToKeep;
	for (auto& level : pLevelsInfo->m_levels)
	{
		for (auto& slotsContentPackage : level.second->m_slotsContentPackages)
		{
			for (auto& contentFile : slotsContentPackage->m_contentFiles)
			{
				LogString(ONELINKCONTENT_LOG, "SetOneLinkContentFiles key=%s, m_filename=%s m_checksum=%s", contentFile->GetKey().c_str(), contentFile->m_filename.c_str(), contentFile->m_checksum.c_str());
				filesToKeep[contentFile->GetFileName()] = true;
				m_listOfContentFiles.push_back(std::move(contentFile));
			}
		}

		for (auto& minorAward : level.second.get()->m_minorAwards)
		{
			if (minorAward->m_promoGameContentPackage)
			{
				for (auto& contentFile : minorAward->m_promoGameContentPackage->m_contentFiles)
				{
					LogString(ONELINKCONTENT_LOG, "SetOneLinkContentFiles key=%s, m_filename=%s m_checksum=%s", contentFile->GetKey().c_str(), contentFile->m_filename.c_str(), contentFile->m_checksum.c_str());
					filesToKeep[contentFile->GetFileName()] = true;
					m_listOfContentFiles.push_back(std::move(contentFile));
				}
			}

			if (minorAward->m_celebrationContentPackage)
			{
				for (auto& contentFile : minorAward->m_celebrationContentPackage->m_contentFiles)
				{
					LogString(ONELINKCONTENT_LOG, "SetOneLinkContentFiles key=%s, m_filename=%s m_checksum=%s", contentFile->GetKey().c_str(), contentFile->m_filename.c_str(), contentFile->m_checksum.c_str());
					filesToKeep[contentFile->GetFileName()] = true;
					m_listOfContentFiles.push_back(std::move(contentFile));
				}
			}
		}
	}

	for (auto& announcement : pProgressiveAnnouncementsInfo->m_announcements)
	{
		if (announcement.second->m_contentPackage)
		{
			for (auto& contentFile : announcement.second->m_contentPackage->m_contentFiles)
			{
				LogString(ONELINKCONTENT_LOG, "SetOneLinkContentFiles key=%s, m_filename=%s m_checksum=%s", contentFile->GetKey().c_str(), contentFile->m_filename.c_str(), contentFile->m_checksum.c_str());
				filesToKeep[contentFile->GetFileName()] = true;
				m_listOfContentFiles.push_back(std::move(contentFile));
			}
		}
	}

	CUtilities::DeleteDirectory(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_ONELINK_CONTENT), true, &filesToKeep);
	for (auto fileToKeep : filesToKeep)
	{
		LogString(ONELINKCONTENT_LOG, "Keeping file=%s", fileToKeep.first.c_str());
	}
	DeleteOldContentFiles();

	if (m_listOfContentFiles.empty())
	{
		m_onelinkPromotionalContentInformation = DEFAULT_ONELINK_CONTENT_INFO;
	}

	for (auto& slotContent : m_listOfContentFiles)
	{
		string fileName(slotContent->GetFileName());
		string url(m_slotsContentUrl);
		url.append("/");
		url.append(CUtilities::UrlEncode(fileName));
		CFileDownloader::Instance().AddFileToDownload(fileName, url, CFileDownloader::FILE_TYPE_CUSTOM_CONTENT, slotContent->GetChecksum(), slotContent->GetKey());
	}
}

void CMediaPackageManager::SetOneLinkContent(const std::string& dynamicContentKey)
{
	std::unique_lock<std::recursive_mutex> criticalSection(m_criticalSection);
	LogString(ONELINKCONTENT_LOG, "SetOneLinkContent key=%s", dynamicContentKey.c_str());
	m_contentFiles.insert(dynamicContentKey);

	UpdateOneLinkContentInformation();
}

string CMediaPackageManager::GetOneLinkContentInformation(const std::string &ipAddress)
{
	string oneLinkContentInformation;
	{
		std::unique_lock<std::recursive_mutex> criticalSection(m_criticalSection);

		oneLinkContentInformation = GetCampaignAndSkinInformation(ipAddress);

		oneLinkContentInformation += m_onelinkPromotionalContentInformation;
	}	
	return oneLinkContentInformation;
}

struct closedir_deleter {
	void operator()(DIR* d) const {
		closedir(d);
	}
};

void CMediaPackageManager::DeleteOldContentFiles()
{
	m_contentFilesToDelete.clear();
	std::unique_ptr<DIR, closedir_deleter> dir(opendir(ONELINK_DYNAMIC_CONTENT_PATH.c_str()));
	if (dir == nullptr)
	{
		LogString(ERROR_LOG, "DeleteOldContentFiles Error opening directory=%s found", ONELINK_DYNAMIC_CONTENT_PATH.c_str());
		LogString(ONELINKCONTENT_LOG, "DeleteOldContentFiles Error opening directory=%s found", ONELINK_DYNAMIC_CONTENT_PATH.c_str());
	}
	else
	{
		dirent* entry;
		while ((entry = readdir(dir.get())))
		{
			if (entry->d_type == DT_DIR)
			{
				// Ignore . and .. directories
				if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0)
					continue;

				string contentDirectory(entry->d_name);
				//make sure the first letter is digit or later we will also have a list of
				//custom content for birthdays, bad card, etc. Then we can remove this isdigit check.
				bool deleteContentDirectoryFile(true);
				for (const auto& contentFile : m_listOfContentFiles)
				{
					if (contentDirectory == contentFile->GetKey())
					{
						deleteContentDirectoryFile = false;
						break;
					}
				}
				if (isdigit(contentDirectory.at(0)) && deleteContentDirectoryFile)
				{
					m_contentFilesToDelete.insert(contentDirectory);
					LogString(ONELINKCONTENT_LOG, "Schedule to delete old content directory=%s", contentDirectory.c_str());
				}
				else
				{
					LogString(ONELINKCONTENT_LOG, "Keeping content directory=%s", contentDirectory.c_str());

				}
			}
		}
		if (m_contentFilesToDelete.size() > 0)
		{
			m_cleanupOldContentFileTime = CUtilities::GetCurrentTime() + DELETE_CONTENT_FILES_IN_SECONDS;
			LogString(ONELINKCONTENT_LOG, "Old content files are going to be deleted on %s", 
				CUtilities::FormatDateTimeString(m_cleanupOldContentFileTime, DATE_TIME_MASK).c_str());;
		}		
	}
}

void CMediaPackageManager::UpdateOneLinkContentInformation()
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key("SlotsContentPackages");
	writer.StartArray();
	for (const auto& contentFile : m_listOfContentFiles)
	{
		writer.StartObject();
		writer.Key(pKeyId);			writer.String(contentFile->GetKey());
		writer.Key(pKeyFileName);	writer.String(contentFile->GetFileName());
		writer.Key(pKeyMD5);		writer.String(contentFile->GetChecksum());
		writer.EndObject();
	}
	writer.EndArray();
	writer.EndObject();

	m_onelinkPromotionalContentInformation = writeStream.m_str;
}

std::string CMediaPackageManager::GetCampaignAndSkinInformation(const std::string &ipAddress) const
{
	std::string strInfo;

	const std::string mediaDir = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_BASE) + "/media";
	const std::string anniversarySpecPath = ACTIVE_CAMPAIGN_PATH + "/Anniversary/Spec.json";
	const std::string attractorSpecPath = ACTIVE_CAMPAIGN_PATH + "/Attractor/Spec.json";
	const std::string birthdaySpecPath = ACTIVE_CAMPAIGN_PATH + "/Birthday/Spec.json";
	const std::string congratsSpecPath = ACTIVE_CAMPAIGN_PATH + "/Congrats/Spec.json";

	std::string trimmedCampaignPath = ACTIVE_CAMPAIGN_PATH;
	CUtilities::FindAndReplace(trimmedCampaignPath, mediaDir, "");

	if (CUtilities::DoesFileExist(anniversarySpecPath))
	{
		strInfo = "<a href=\"https://" + ipAddress + ":8080" + trimmedCampaignPath + "/Anniversary/Spec.json\">Campaign - Anniversary</a><br>";
	}

	if (CUtilities::DoesFileExist(attractorSpecPath))
	{
		strInfo += "<a href=\"https://" + ipAddress + ":8080" + trimmedCampaignPath + "/Attractor/Spec.json\">Campaign - Attractor</a><br>";
	}

	if (CUtilities::DoesFileExist(birthdaySpecPath))
	{
		strInfo += "<a href=\"https://" + ipAddress + ":8080" + trimmedCampaignPath + "/Birthday/Spec.json\">Campaign - Birthday</a><br>";
	}

	if (CUtilities::DoesFileExist(congratsSpecPath))
	{
		strInfo += "<a href=\"https://" + ipAddress + ":8080" + trimmedCampaignPath + "/Congrats/Spec.json\">Campaign - Congratulations</a><br>";
	}

	const std::string customAssetsPath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_CUSTOM_ASSETS);
	const std::string skinPath = customAssetsPath + "Skin.json";
	std::string trimmedCustomAssetsPath = customAssetsPath;
	CUtilities::FindAndReplace(trimmedCustomAssetsPath, mediaDir, "");

	if (CUtilities::DoesFileExist(skinPath))
	{
		strInfo += "<a href=\"https://" + ipAddress + ":8080" + trimmedCustomAssetsPath + "Skin.json\">Custom Skin</a><br>";
	}

	if (!strInfo.empty())
	{
		strInfo += "<br>";
	}

	return strInfo;
}

bool CMediaPackageManager::SaveActiveCampaignInfo() const
{
	bool success = false;

	if (m_pActiveCampaignInfo)
	{
		JsonWriteStreamString writeStream;
		JsonWriter writer(writeStream);

		writer.StartObject();
		writer.Key("name");		writer.String(m_pActiveCampaignInfo->GetName());
		writer.Key("id");		writer.Int64(m_pActiveCampaignInfo->GetId());
		writer.Key("revision");	writer.Int64(m_pActiveCampaignInfo->GetRevision());
		writer.EndObject();

		std::string jsonData = writeStream.m_str;

		const std::string filename = ACTIVE_CAMPAIGN_PATH + "/" + CAMPAIGN_INFO_FILENAME;
		ofstream outFile(filename);
		outFile << jsonData;
		outFile.close();

		success = true;
	}

	return success;
}

bool CMediaPackageManager::LoadActiveCampaignInfo()
{
	std::unique_lock<std::recursive_mutex> lock(m_campaignInfoMutex);
	bool success = false;

	const std::string filename = ACTIVE_CAMPAIGN_PATH + "/" + CAMPAIGN_INFO_FILENAME;
	if (CUtilities::DoesFileExist(filename.c_str()))
	{
		ifstream inFile(filename);
		std::stringstream strStream;
		strStream << inFile.rdbuf();
		std::string jsonData = strStream.str();
		inFile.close();

		rapidjson::Document jsonDoc;
		if (jsonDoc.Parse(jsonData.c_str()).HasParseError())
		{
			LogString(ERROR_LOG, "Error : Unable to parse active campaign info file, filename=%s", filename.c_str());
		}
		else
		{
			std::string name = JsonGetStringEx(jsonDoc, "name");
			DWORD id = JsonGetInt64Ex(jsonDoc, "id");
			DWORD revision = JsonGetInt64Ex(jsonDoc, "revision");

			m_pActiveCampaignInfo = std::unique_ptr<CPackageInfo>(new CPackageInfo(id, name, "", revision));

			success = true;
		}
	}

	return success;
}

CPackageInfo CMediaPackageManager::GetActiveCampaignInfo()
{
	std::unique_lock<std::recursive_mutex> lock(m_campaignInfoMutex);
	CPackageInfo info;

	if (m_pActiveCampaignInfo)
	{
		info = *m_pActiveCampaignInfo.get();
	}

	return info;
}

CPackageInfo CMediaPackageManager::GetPendingCampaignInfo()
{
	std::unique_lock<std::recursive_mutex> lock(m_campaignInfoMutex);
	CPackageInfo info;

	if (m_pPendingCampaignInfo)
	{
		info = *m_pPendingCampaignInfo.get();
	}

	return info;
}

bool CMediaPackageManager::PendingCampaignMediaDownloaded()
{
	std::unique_lock<std::recursive_mutex> lock(m_campaignInfoMutex);
	bool success = false;

	if (m_pPendingCampaignInfo && m_pPendingCampaignInfo->GetPendingMediaCount() > 0)
	{
		m_pPendingCampaignInfo->DecrementPendingMediaCount();
		LogString(FLASHING, "[%s] Remaining campaign media count to download = %u", __FUNCTION__, m_pPendingCampaignInfo->GetPendingMediaCount());

		if (0 == m_pPendingCampaignInfo->GetPendingMediaCount())
		{
			// all media downloaded for pending campaign, so make it active campaign.

			// delete active campaign folder.
			CUtilities::DeleteDirectory(ACTIVE_CAMPAIGN_PATH);

			// rename pending campaign folder
			LogString(FLASHING, "[%s] Renaming campaign folder...", __FUNCTION__);
			const std::string moveCmd(FormatString("mv '%s' '%s'", PENDING_CAMPAIGN_PATH.c_str(), ACTIVE_CAMPAIGN_PATH.c_str()));
			LogString(FLASHING, "[%s] %s", __FUNCTION__, moveCmd.c_str());
			success = (0 == system(moveCmd.c_str()));

			if (success)
			{
				LogString(FLASHING, "[%s] Folder renamed successfully.", __FUNCTION__);

				// make pending campaign active
				m_pActiveCampaignInfo = std::move(m_pPendingCampaignInfo);
				m_pPendingCampaignInfo = nullptr;

				// create CampaignInfo.json
				SaveActiveCampaignInfo();
			}
			else
			{
				LogString(FLASHING, "[%s] Error: Folder renaming failed.", __FUNCTION__);
				LogString(ERROR_LOG, "[%s] Error: Folder renaming failed.", __FUNCTION__);
			}
		}
	}
	else
	{
		LogString(FLASHING, "[%s] Error: Unwanted call", __FUNCTION__);
		LogString(ERROR_LOG, "[%s] Error: Unwanted call", __FUNCTION__);
	}

	return success;
}

void CMediaPackageManager::SetPendingCampaignInfo(DWORD id, const std::string &name, DWORD revision, DWORD mediaCount)
{
	std::unique_lock<std::recursive_mutex> lock(m_campaignInfoMutex);
	m_pPendingCampaignInfo = std::unique_ptr<CPackageInfo>(new CPackageInfo(id, name, "", revision, mediaCount));
}

void CMediaPackageManager::SetPendingSkinInfo(DWORD id, const std::string &name, const std::string &version, DWORD revision, DWORD mediaCount)
{
	std::unique_lock<std::recursive_mutex> lock(m_skinInfoMutex);
	m_pPendingSkinInfo = std::unique_ptr<CPackageInfo>(new CPackageInfo(id, name, version, revision, mediaCount));
}

bool CMediaPackageManager::PendingSkinMediaDownloaded()
{
	std::unique_lock<std::recursive_mutex> lock(m_skinInfoMutex);
	bool success = false;

	if (m_pPendingSkinInfo && m_pPendingSkinInfo->GetPendingMediaCount() > 0)
	{
		m_pPendingSkinInfo->DecrementPendingMediaCount();
		LogString(FLASHING, "[%s] Remaining skin media count to download = %u", __FUNCTION__, m_pPendingSkinInfo->GetPendingMediaCount());

		if (0 == m_pPendingSkinInfo->GetPendingMediaCount())
		{
			// all media downloaded for pending skin, so make it active skin.
			LogString(FLASHING, "[%s] All media downloaded for pending skin.", __FUNCTION__);
			success = true;
		}
	}
	else
	{
		LogString(FLASHING, "[%s] Error: Unwanted call", __FUNCTION__);
		LogString(ERROR_LOG, "[%s] Error: Unwanted call", __FUNCTION__);
	}

	return success;
}

void CMediaPackageManager::LoadActiveSkinInfo()
{
	std::unique_lock<std::recursive_mutex> lock(m_skinInfoMutex);

	const std::string customSkinFilename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_CUSTOM_ASSETS) + "Skin.json";
	const std::string defaultSkinFilename = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_ASSETS) + "Skin.json";

	if (CUtilities::DoesFileExist(customSkinFilename.c_str()))
	{
		if (!LoadSkinInfo(customSkinFilename))
		{
			LogString(ERROR_LOG, "Error : Custom skin file loading failed, so deleting custom skin.json file and loading the default skin.json file.");
			CUtilities::DeleteFile(customSkinFilename.c_str());

			LoadSkinInfo(defaultSkinFilename);
		}
	}
	else
	{
		LoadSkinInfo(defaultSkinFilename);
	}
}


bool CMediaPackageManager::LoadSkinInfo(const std::string &filename)
{
	bool success = false;

	ifstream inFile(filename);
	std::stringstream strStream;
	strStream << inFile.rdbuf();
	std::string jsonData = strStream.str();
	inFile.close();

	rapidjson::Document jsonDoc;
	if (jsonDoc.Parse(jsonData.c_str()).HasParseError())
	{
		LogString(ERROR_LOG, "Error : Unable to parse skin file, filename=%s", filename.c_str());
	}
	else
	{
		const std::string name = JsonGetStringEx(jsonDoc, "name");
		const std::string version = JsonGetStringEx(jsonDoc, "skinVersion");
		DWORD revision = JsonGetInt64Ex(jsonDoc, "skinRevision");
		int id = JsonGetIntEx(jsonDoc, "id");

		m_pActiveSkinInfo = std::unique_ptr<CPackageInfo>(new CPackageInfo(id, name, version, revision));
		success = true;
	}

	return success;
}

CPackageInfo CMediaPackageManager::GetActiveSkinInfo()
{
	std::unique_lock<std::recursive_mutex> lock(m_skinInfoMutex);
	CPackageInfo info;

	if (m_pActiveSkinInfo)
	{
		info = *m_pActiveSkinInfo.get();
	}

	return info;
}

CPackageInfo CMediaPackageManager::GetPendingSkinInfo()
{
	std::unique_lock<std::recursive_mutex> lock(m_skinInfoMutex);
	CPackageInfo info;

	if (m_pPendingSkinInfo)
	{
		info = *m_pPendingSkinInfo.get();
	}

	return info;
}

CPackageInfo CMediaPackageManager::MakePendingSkinAsActiveSkin()
{
	std::unique_lock<std::recursive_mutex> lock(m_skinInfoMutex);
	CPackageInfo activeSkinInfo = *m_pActiveSkinInfo.get();

	const std::string dynamicContentPath(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_CUSTOM_DYNAMIC_CONTENT));
	const std::string customAssetsPath(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_CUSTOM_ASSETS));
	std::string oldCustomAssetsPath(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_CUSTOM_ASSETS));
	if (oldCustomAssetsPath.at(oldCustomAssetsPath.size() - 1) == '/')
	{
		oldCustomAssetsPath = oldCustomAssetsPath.substr(0, oldCustomAssetsPath.size() - 1);
	}
	oldCustomAssetsPath += "_old";

	std::string moveCmd;
	if (CUtilities::DoesDirExist(dynamicContentPath.c_str()))
	{
		// copy dynamic content folder to pending skin path
		moveCmd = FormatString("cp -r %s %s && ", dynamicContentPath.c_str(), PENDING_SKIN_PATH.c_str());
	}
	if (CUtilities::DoesDirExist(customAssetsPath.c_str()))
	{
		// rename custom assets folder as assets_old
		moveCmd += FormatString("mv %s %s && ", customAssetsPath.c_str(), oldCustomAssetsPath.c_str());
	}
	// rename pending skin folder as custom assets folder
	moveCmd += FormatString("mv %s %s", PENDING_SKIN_PATH.c_str(), customAssetsPath.c_str());
	LogString(FLASHING, "[%s] %s", __FUNCTION__, moveCmd.c_str());

	bool success = (0 == system(moveCmd.c_str()));

	if (success)
	{
		LogString(FLASHING, "[%s] Skin contents moved successfully, pending skin will now be active skin.", __FUNCTION__);
		CUtilities::DeleteDirectory(oldCustomAssetsPath);

		m_pActiveSkinInfo = std::move(m_pPendingSkinInfo);
		m_pPendingSkinInfo = nullptr;
		activeSkinInfo = *m_pActiveSkinInfo.get();
	}
	else
	{
		LogString(FLASHING, "[%s] Error : unable to move skin contents.", __FUNCTION__);
		LogString(ERROR_LOG, "[%s] Error : unable to move skin contents.", __FUNCTION__);

		LogString(FLASHING, "[%s] Restoring data...", __FUNCTION__);
		CUtilities::DeleteDirectory(customAssetsPath);
		std::string renameCmd(FormatString("mv %s %s", oldCustomAssetsPath.c_str(), customAssetsPath.c_str()));
		LogString(FLASHING, "[%s] %s", __FUNCTION__, renameCmd.c_str());
		system(renameCmd.c_str());
	}

	return activeSkinInfo;
}

void CMediaPackageManager::TimedEvents()
{
	std::unique_lock<std::recursive_mutex> criticalSection(m_criticalSection);
	if (!m_contentFilesToDelete.empty())
	{
		if (CUtilities::GetCurrentTime() > m_cleanupOldContentFileTime)
		{
			LogString(ONELINKCONTENT_LOG, "Time to delete old content files:");
			for (string contentDirectory : m_contentFilesToDelete)
			{
				CUtilities::DeleteDirectory(ONELINK_DYNAMIC_CONTENT_PATH + contentDirectory);
				LogString(ONELINKCONTENT_LOG, "directory=%s", contentDirectory.c_str());
			}
			m_contentFilesToDelete.clear();
		}
	}
}

void CMediaPackageManager::ExtractNCDFiles()
{
	CWatchDog::Instance().Checkin();
	const std::string sourcePath(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_CUSTOM_ASSETS));
	if (CUtilities::DoesDirExist(sourcePath.c_str()))
	{
		const std::initializer_list<std::string> fileTypes = { "*.ncd" };
		int fileCount = CUtilities::FileCount(sourcePath, fileTypes);
		LogString(FLASHING, "[%s] NCD file count to extract = %d", __FUNCTION__, fileCount);

		if (fileCount > 0)
		{
			const std::string targetPath(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_DYNAMIC_CONTENT));
			std::string command = "cd " + sourcePath +
				"&& find . -name '*.ncd' -exec sh -c 'a=\"${1#*.}\";" +
				" rm -rf \"" + targetPath + "${a%.*}\";" +
				" mkdir \"" + targetPath + "${a%.*}\";" +
				" unzip -d \"" + targetPath + "${a%.*}\" \"$1\"' _ {} \\;";

			LogString(FLASHING, "[%s] %s", __FUNCTION__, command.c_str());

			bool success = (0 == system(command.c_str()));

			if (success)
			{
				LogString(FLASHING, "[%s] NCD files extracted successfully.", __FUNCTION__);
			}
			else
			{
				LogString(FLASHING, "[%s] Unable to extract NCD files.", __FUNCTION__);
			}
		}
	}
	CWatchDog::Instance().Checkin();
}
