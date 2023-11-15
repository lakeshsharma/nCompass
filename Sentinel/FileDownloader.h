#pragma once
#include "InternalMessage.h"
#include "ProgressiveProtocol/FirmwarePackage.h"
#include "FlashStatus.h"
#include "MediaPackageManager.h"

namespace SentinelUnitTests
{
	class FileDownloaderTests;
}

class CInternalsFramework;

class CFileDownloader
{
public:

	static CFileDownloader & Instance();
    ~CFileDownloader();
    
	typedef enum
	{
		FILE_TYPE_OS,
		FILE_TYPE_EXE,
		FILE_TYPE_FPGA,
		FILE_TYPE_SKIN,
		FILE_TYPE_CUSTOM_CONTENT,
		FILE_TYPE_CAMPAIGN_MEDIA,
		FILE_TYPE_CONFIG
	}Filetype;

	void StartThread(CInternalsFramework* internals, CMediaPackageManager* mediaPackageManager);
	void AddFileToDownload(const std::string &filename, const std::string &url, Filetype type,
		const std::string &checksum, const std::string &additionalInfo = "");
	void PurgeFilesToDownload(Filetype filetypeToPurge);
	std::string GetChecksum(Filetype type, const std::string &filename) const;

protected:
	typedef struct
	{
		std::string filename;
		std::string url;
		Filetype type;
		std::string checksum;
		std::string timestamp;
		std::string additionalInfo;
	}FileInfo;

private:
	CFileDownloader();
	static CFileDownloader *m_instance;
	static std::recursive_mutex m_instanceMutex;
	static UINT _thread(CFileDownloader *pClass);

	void StopThread();
	void ThreadMain();
	void ClearUpgradePath();
	
	void UpdateUIStatus(const std::string &status);
	bool Download(FileInfo* pFileInfo, std::string &destFilename, std::string &error) const;
	bool ConsumeActiveDownload();
	bool ConsumeActiveDownload_OS_EXE();
	bool ConsumeActiveDownload_FPGA();
	bool ConsumeActiveDownload_Config();
	bool ConsumeActiveDownload_CustomContent();
	bool ConsumeActiveDownload_CampaignMedia();
	bool ConsumeActiveDownload_SkinMedia();
	bool Unzip(const std::string& sourceDirectoryPath, const std::string &filename, const std::string &path, bool createPath) const;
	bool MoveFiles(const std::string &src, const std::string &destDir, bool movingMultipleFiles = true) const;
	bool PrepareLiteUnitUpgradeFolder(const char *pUpgradePath);
	void PerformOsExeUpgradeIfNeeded();
	void ProcessesCertFiles(const std::string &extractionPath);

	static int DownloadProgressCallback(void *clientp, int64_t dltotal, int64_t dlnow, int64_t ultotal, int64_t ulnow);
	static DWORD m_progressCaptureTickCount;
	static CFlashStatus *m_pFlashStatus;
	static CInternalsFramework *m_pInternals;
	
	std::unique_ptr<std::thread> m_pThread;
	CMediaPackageManager* m_mediaPackageManager;
	bool m_stopThread;
	std::recursive_mutex m_downloadQueueMutex;
	std::queue<std::unique_ptr<FileInfo>> m_downloadQueue;
	std::unique_ptr<FileInfo> m_pActiveDownloadInfo;
	bool m_isPurgeCurrentDownload;

	// Parameters to handle OS and EXE downloads
	bool m_isOSAvailable;
	bool m_isOSDownloaded;
	bool m_isEXEAvailable;
	bool m_isEXEDownloaded;

	bool m_isOsExeUpgradeReady;
private:
	friend SentinelUnitTests::FileDownloaderTests;

};
