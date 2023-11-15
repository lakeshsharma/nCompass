#pragma once
#include "UI/ScreenDefs.h"
#include "FileList.h"
#include "InternalMessage.h"

using namespace pugi;
using namespace std;

extern const std::string ACTIVE_CAMPAIGN_PATH;
extern const std::string PENDING_CAMPAIGN_PATH;
extern const std::string PENDING_SKIN_PATH;

class CSlotsContentFile;

class CMediaPackageManager
{
public:
	static CMediaPackageManager & Instance();
    ~CMediaPackageManager();
    
	/// <summary>
	/// Gets the SlotsContentType based on the  InternalMsgType
	/// </summary>
	/// <param name="msgType">The message type</param>
	/// <returns>the slots content type</returns>
	SlotsContentType::Type GetPteSlotsContentType(InternalMsgType msgType) const;

	/// <summary>
	/// Gets the dynamic content path
	/// </summary>
	/// <param name="id">The event id</param>
	/// <param name="minorAwardId">The minor award id</param>
	/// <param name="type">The slots content type</param>
	/// <param name="orientation">The package orientation</param>
	/// <returns>screen info object</returns>
	std::string GetDynamicContentPath(int64_t id, const std::string& minorAwardId, SlotsContentType::Type type, const  std::string& orientation);

	/// <summary>
	/// Sets the OneLink content files URL and files.
	/// </summary>
	/// <param name="slotsContentUrl">The slots content URL.</param>
	/// <param name="slotsContentFiles">The slots content files.</param>
	void SetOneLinkContentFiles(const ProgressiveLevelsInfo* pLevelsInfo, const CProgressiveAnnouncementsInfo* pProgressiveAnnouncementsInfo);

	/// <summary>
	/// Sets the OneLink content files URL and files.
	/// </summary>
	/// <param name="dynamicContentKey">dynamic content key.</param>
	void SetOneLinkContent(const std::string& dynamicContentKey);

	/// <summary>
	/// Gets the OneLink content information.
	/// </summary>
	/// <param name="ipAddress">IN - the IP address.</param>
	/// <returns>OneLink content information</returns>
	std::string GetOneLinkContentInformation(const std::string &ipAddress);

	/// <summary>
	/// Loads CampaignInfo.json file.
	/// </summary>
	/// <returns>true if file loaded, otherwise returns false</returns>
	bool LoadActiveCampaignInfo();

	/// <summary>
	/// Gets active CampaignInfo data.
	/// </summary>
	/// <returns>the active CampaignInfo</returns>
	CPackageInfo GetActiveCampaignInfo();

	/// <summary>
	/// Gets pending CampaignInfo data.
	/// </summary>
	/// <returns>the pending CampaignInfo</returns>
	CPackageInfo GetPendingCampaignInfo();

	/// <summary>
	/// Checks if all pending campaign media is downloaded, then makes pending campaign as active campaign.
	/// </summary>
	/// <returns>true if pending campaign is active, else false.</returns>
	bool PendingCampaignMediaDownloaded();

	/// <summary>
	/// Sets pending campaign info.
	/// </summary>
	/// <param name="id">The campaign id.</param> 
	/// <param name="name">The campaign name.</param>
	/// <param name="revision">The campaign revision.</param>
	/// <param name="mediaCount">Media count to download for this campaign.</param>
	void SetPendingCampaignInfo(DWORD id, const std::string &name, DWORD revision, DWORD mediaCount);

	/// <summary>
	/// Sets pending skin info.
	/// </summary>
	/// <param name="id">The skin id.</param>
	/// <param name="name">The skin name.</param>
	/// <param name="version">The skin version.</param>
	/// <param name="revision">The skin revision.</param>
	/// <param name="mediaCount">Media count to download for this skin.</param>
	void SetPendingSkinInfo(DWORD id, const std::string &name, const std::string &version, DWORD revision, DWORD mediaCount);

	/// <summary>
	/// Checks if all pending skin media is downloaded.
	/// </summary>
	/// <returns>true if all pending skin media downloaded, else false.</returns>
	bool PendingSkinMediaDownloaded();

	/// <summary>
	/// Loads skin info from Skin.json file. It first tries to load custom skin info. If it fails, then it loads default skin info.
	/// </summary>
	void LoadActiveSkinInfo();

	/// <summary>
	/// Gets active SkinInfo data.
	/// </summary>
	/// <returns>the active SkinInfo</returns>
	CPackageInfo GetActiveSkinInfo();

	/// <summary>
	/// Gets pending SkinInfo data.
	/// </summary>
	/// <returns>the pending SkinInfo</returns>
	CPackageInfo GetPendingSkinInfo();

	/// <summary>
	/// Makes pending skin as active skin.
	/// </summary>
	/// <returns>the active SkinInfo</returns>
	CPackageInfo MakePendingSkinAsActiveSkin();

	/// <summary>
	/// Performed the timed events for media package manager.
	/// </summary>
	void TimedEvents();

	/// <summary>
	/// Extracts the NCD files from custom skin.
	/// </summary>
	void ExtractNCDFiles();

private:
	CMediaPackageManager();
	static CMediaPackageManager *m_instance;
	static std::recursive_mutex m_instanceCS;

	void DeleteOldContentFiles();
	void UpdateOneLinkContentInformation();
	std::string GetCampaignAndSkinInformation(const std::string &ipAddress) const;

	std::recursive_mutex  m_criticalSection;
	std::string m_onelinkPromotionalContentInformation;
	std::string m_slotsContentUrl;
	std::set<std::string> m_contentFiles;//stores CSlotsContentFile::GetKey
	std::set<std::string> m_contentFilesToDelete;
	vector<std::unique_ptr<CSlotsContentFile>> m_listOfContentFiles;

	/// <summary>
	/// Saves active campaign info into CampaignInfo.json file.
	/// </summary>
	/// <returns>true if saved, otherwise returns false</returns>
	bool SaveActiveCampaignInfo() const;

	// Campaign related stuff
	std::recursive_mutex m_campaignInfoMutex;
	std::unique_ptr<CPackageInfo> m_pActiveCampaignInfo;
	std::unique_ptr<CPackageInfo> m_pPendingCampaignInfo;

	/// <summary>
	/// Loads skin info from passed Skin.json file.
	/// </summary>
	/// <param name="filename">The filename.</param>
	/// <returns>true if file loaded, otherwise returns false</returns>
	bool LoadSkinInfo(const std::string &filename);

	// Skin related stuff
	std::recursive_mutex m_skinInfoMutex;
	std::unique_ptr<CPackageInfo> m_pActiveSkinInfo;
	std::unique_ptr<CPackageInfo> m_pPendingSkinInfo;

	time_t m_cleanupOldContentFileTime;
};
