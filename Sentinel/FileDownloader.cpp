#include "stdafx.h"

#include "FileDownloader.h"
#include "Logging/Logger.h"
#include "Logging/LogString.h"
#include "Hardware/WatchDog.h"
#include "HttpGet.h"
#include "Platform.h"
#include "Utilities.h"
#include <iostream>
#include <fstream>
#include "InternalsFramework.h"

const std::string ONELINK_CONTENT_PATH = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_ONELINK_CONTENT) + "/";
const std::string ONELINK_CONTENTTEMP_PATH = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_ONELINK_CONTENTTEMP) + "/";
const std::string ONELINK_DYNAMIC_CONTENT_PATH = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_DYNAMIC_CONTENT) + "/";
const std::string CREATE_MANIFEST_CMD = "find . -type f ! -path ./manifest.sha1 -exec sha1sum '{}' + | sort > manifest.sha1";
const int DOWNLOAD_CHECK_INTERVAL = 1000; // in milliseconds
const int DOWNLOAD_PROGRESS_INTERVAL = 200; // in milliseconds
const int YIELD_FOR_UI_PROGRESS_STATUS = 500; // in milliseconds

CFileDownloader *CFileDownloader::m_instance = nullptr;
recursive_mutex CFileDownloader::m_instanceMutex;
CInternalsFramework* CFileDownloader::m_pInternals = nullptr;
DWORD CFileDownloader::m_progressCaptureTickCount = 0;;
CFlashStatus* CFileDownloader::m_pFlashStatus = nullptr;

CFileDownloader::CFileDownloader() :
	m_pThread(nullptr),
	m_mediaPackageManager(nullptr),
	m_stopThread(false),
	m_pActiveDownloadInfo(nullptr),
	m_isPurgeCurrentDownload(false),
	m_isOSAvailable(false),
	m_isOSDownloaded(false),
	m_isEXEAvailable(false),
	m_isEXEDownloaded(false),
	m_isOsExeUpgradeReady(false)
{
}

CFileDownloader& CFileDownloader::Instance()
{
	if (nullptr == m_instance)
	{
		lock_guard<recursive_mutex> lock(m_instanceMutex);
		if (nullptr == m_instance)
		{
			m_instance = new CFileDownloader();
		}
	}
	return *m_instance;
}

CFileDownloader::~CFileDownloader(void)
{
	m_pInternals = nullptr;
	StopThread();
}

void CFileDownloader::StartThread(CInternalsFramework *internals, CMediaPackageManager* mediaPackageManager)
{
	// This check will make sure any duplicate call will not start a new thread.
	if (nullptr == m_pThread)
	{
		LogString(FLASHING, "[%s] Creating FileDownloader thread.", __FUNCTION__);
		m_pInternals = internals;
		m_mediaPackageManager = mediaPackageManager;
		m_pThread = std::unique_ptr<std::thread>(new std::thread(_thread, this));
	}
	else
	{
		LogString(FLASHING, "[%s] FileDownloader already created.", __FUNCTION__);
		LogString(ERROR_LOG, "[%s] FileDownloader already created.", __FUNCTION__);
	}
}

void CFileDownloader::AddFileToDownload(const std::string &filename, const std::string &url, Filetype type,
	const std::string &checksum, const std::string &additionalInfo)
{
	if (m_isOsExeUpgradeReady && (type == Filetype::FILE_TYPE_OS || type == Filetype::FILE_TYPE_EXE))
	{
		LogString(FLASHING, "[%s] Ignoring this file as system is already ready for upgrade.", __FUNCTION__);
		return;
	}

	std::unique_ptr<FileInfo> info(new FileInfo);
	info->filename = filename;
	info->url = url;
	info->type = type;
	info->checksum = checksum;
	info->timestamp = CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), DATE_TIME_MASK);
	info->additionalInfo = additionalInfo;

	{
		std::unique_lock<std::recursive_mutex> lock(m_downloadQueueMutex);
		m_downloadQueue.push(std::move(info));
	}

	switch (type)
	{
	case Filetype::FILE_TYPE_OS:
		m_isOSAvailable = true;
		if (!m_isEXEAvailable)
		{
			ClearUpgradePath();
		}
		break;
	case Filetype::FILE_TYPE_EXE:
		m_isEXEAvailable = true;
		if (!m_isOSAvailable)
		{
			ClearUpgradePath();
		}
		break;
	default:
		break;
	}
}

void CFileDownloader::PurgeFilesToDownload(Filetype filetypeToPurge)
{
	std::unique_lock<std::recursive_mutex> lock(m_downloadQueueMutex);
	std::queue<std::unique_ptr<FileInfo>> newDownloadQueue;

	// create new queue without filetypeToPurge entries
	while (!m_downloadQueue.empty())
	{
		std::unique_ptr<FileInfo> fileInfo = std::move(m_downloadQueue.front());
		m_downloadQueue.pop();
		if (fileInfo->type == filetypeToPurge)
		{
			LogString(FLASHING, "[%s] Purging file from download queue, filename=%s, checksum=%s",
				__FUNCTION__, fileInfo->filename.c_str(), fileInfo->checksum.c_str());
		}
		else
		{
			newDownloadQueue.push(std::move(fileInfo));
		}
	}
	m_downloadQueue = std::move(newDownloadQueue);

	// check if current download is of purge type
	if (m_pActiveDownloadInfo && m_pActiveDownloadInfo->type == filetypeToPurge)
	{
		m_isPurgeCurrentDownload = true;
	}
}

void CFileDownloader::ClearUpgradePath()
{
	const std::string upgradePath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE);
	LogString(FLASHING, "[%s] Deleting upgrade folder %s", __FUNCTION__, upgradePath.c_str());
	CUtilities::DeleteDirectory(upgradePath);

	m_isOSDownloaded = false;
	m_isEXEDownloaded = false;
}

void CFileDownloader::UpdateUIStatus(const std::string &status)
{
	m_pFlashStatus->SetStatus(status);
	m_pInternals->UpdateFirmareDownloadStatus(*m_pFlashStatus);
}

void CFileDownloader::StopThread()
{
	if (nullptr != m_pThread)
	{
		CWatchDog::Instance().DeregisterThread();
		m_stopThread = true;
		m_pThread->join();
		m_pThread.reset();
		m_pThread = nullptr;
	}
}

void CFileDownloader::ThreadMain()
{
	CUtilities::ShowThreadId(_T(THREAD_FILE_DOWNLOADER).c_str());
	CWatchDog::Instance().RegisterThread(THREAD_FILE_DOWNLOADER, CUtilities::GetCurrentThreadId(), FILE_DOWNLOADER_TIMEOUT, pthread_self());

	while (!m_stopThread)
	{
		CWatchDog::Instance().Checkin();
		if (m_downloadQueue.empty())
		{
			CUtilities::Sleep(DOWNLOAD_CHECK_INTERVAL);
			continue;
		}

		// get file info to download
		{
			std::unique_lock<std::recursive_mutex> lock(m_downloadQueueMutex);
			m_pActiveDownloadInfo = std::move(m_downloadQueue.front());
			m_downloadQueue.pop();
			m_isPurgeCurrentDownload = false;

			if (m_isOsExeUpgradeReady && 
				(m_pActiveDownloadInfo->type == Filetype::FILE_TYPE_OS || 
				m_pActiveDownloadInfo->type == Filetype::FILE_TYPE_EXE))
			{
				LogString(FLASHING, "[%s] System is already ready for upgrade, so ignoring the download, file: %s, url: %s",
					__FUNCTION__, m_pActiveDownloadInfo->filename.c_str(), m_pActiveDownloadInfo->url.c_str());

				m_pActiveDownloadInfo.release();
				continue;
			}
		}

		m_pInternals->SetCurrentActivity(CurrentActivity::Downloading);
		m_pInternals->SendDashboardStatusToProgressive();

		// we take the last 8 hexadecimal characters of checksum to display on UI
		std::string checksumForUI(m_pActiveDownloadInfo->checksum);
		const size_t LAST_8_CHECKSUM_CHARS(8);
		checksumForUI.erase(0, checksumForUI.size() - LAST_8_CHECKSUM_CHARS);

		// flash type
		FlashType flashType = FlashType::FLASH_NONE;
		switch (m_pActiveDownloadInfo->type)
		{
		case Filetype::FILE_TYPE_OS:
		case Filetype::FILE_TYPE_EXE:
		case Filetype::FILE_TYPE_FPGA:
			flashType = FlashType::FLASH_SYSTEM;
			break;
		case Filetype::FILE_TYPE_CONFIG:
			flashType = FlashType::FLASH_CONFIG;
			break;
		case Filetype::FILE_TYPE_CUSTOM_CONTENT:
			flashType = FlashType::FLASH_CUSTOM_CONTENT;
			break;
		case Filetype::FILE_TYPE_CAMPAIGN_MEDIA:
			flashType = FlashType::FLASH_CAMPAIGN_MEDIA;
			break;
		case Filetype::FILE_TYPE_SKIN:
			flashType = FlashType::FLASH_SKIN;
			break;
		default:
			break;
		}

		// create CFlashStatus instance for progress tracking at UI
		m_pFlashStatus = new CFlashStatus(flashType, 0,
			checksumForUI,
			m_pActiveDownloadInfo->timestamp,
			m_pActiveDownloadInfo->filename,
			m_pActiveDownloadInfo->url);
		m_pInternals->UpdateFirmareDownloadStatus(*m_pFlashStatus);
		CWatchDog::Instance().Checkin();

		// start downloading
		std::string destFilename, error;
		bool success = Download(m_pActiveDownloadInfo.get(), destFilename, error);
		if (success)
		{
			CWatchDog::Instance().Checkin();
			m_pFlashStatus->SetPercentComplete(100);
			m_pInternals->UpdateFirmareDownloadStatus(*m_pFlashStatus);

			// Sleep to make sure UI displays the 100% progress status
			CUtilities::Sleep(YIELD_FOR_UI_PROGRESS_STATUS);

			UpdateUIStatus(DOWNLOAD_STATUS_CHECKSUM_VALIDATION);

			// calculate checksum
			LogString(FLASHING, "[%s] Validating checksum...", __FUNCTION__);
			const std::string checksum = GetChecksum(m_pActiveDownloadInfo->type, destFilename);

			// verify checksum
			if (checksum == m_pActiveDownloadInfo->checksum)
			{
				LogString(FLASHING, "[%s] Checksum verified.", __FUNCTION__);
				success = ConsumeActiveDownload();
			}
			else
			{
				success = false;
				LogString(FLASHING, "[%s] Error : Checksum verification failed. calculatedChecksum=%s checksumFromOnelink=%s",
					__FUNCTION__, checksum.c_str(), m_pActiveDownloadInfo->checksum.c_str());
				LogString(ERROR_LOG, "[%s] Error : Checksum verification failed. calculatedChecksum=%s checksumFromOnelink=%s",
					__FUNCTION__, checksum.c_str(), m_pActiveDownloadInfo->checksum.c_str());

				UpdateUIStatus(DOWNLOAD_STATUS_CHECKSUM_ERROR);

				// delete the downloaded file
				CUtilities::DeleteFile(destFilename.c_str());
			}
		}
		else
		{
			UpdateUIStatus(error);
			CUtilities::DeleteFile(destFilename.c_str());
		}

		CWatchDog::Instance().Checkin();

		switch (m_pActiveDownloadInfo->type)
		{
			case Filetype::FILE_TYPE_EXE:
				// On bad download, reset EXE available flag so that this item can be discarded.
				// On success, again set the EXE available flag because any previous download 
				// failure might have reset this flag.
				success ? m_isEXEDownloaded = m_isEXEAvailable = true : m_isEXEAvailable = false;
				PerformOsExeUpgradeIfNeeded();
			break;
			case Filetype::FILE_TYPE_OS:
				// On bad download, reset OS available flag so that this item can be discarded.
				// On success, again set the OS available flag because any previous download
				// failure might have reset this flag.
				success ? m_isOSDownloaded = m_isOSAvailable = true : m_isOSAvailable = false;
				PerformOsExeUpgradeIfNeeded();
			break;
			default:
			break;
		}

		m_pInternals->SetCurrentActivity(CurrentActivity::None);
		m_pInternals->SendDashboardStatusToProgressive();

		// cleanup
		m_pActiveDownloadInfo.release();
		delete m_pFlashStatus;
		m_pFlashStatus = nullptr;

		if (!m_downloadQueue.empty())
		{
			// Sleep to make sure UI displays the previous status before picking up next item to download
			CUtilities::Sleep(YIELD_FOR_UI_PROGRESS_STATUS);
		}
	}
}

bool CFileDownloader::Download(FileInfo* pFileInfo, std::string &destFilename, std::string &error) const
{
	bool success = false;
	bool proceedWithDownload(true);

	destFilename = ONELINK_CONTENTTEMP_PATH + pFileInfo->filename;

	error.clear();

	if (pFileInfo->type == FILE_TYPE_CUSTOM_CONTENT)
	{
		destFilename = ONELINK_CONTENT_PATH + pFileInfo->filename;
		if (CUtilities::DoesFileExist(destFilename))
		{
			// calculate checksum
			const std::string checksum = GetChecksum(m_pActiveDownloadInfo->type, destFilename);

			// verify checksum
			if (checksum == m_pActiveDownloadInfo->checksum)
			{
				LogString(FLASHING, "[%s] %s already exist with the proper checksum, so skipping the download.", __FUNCTION__, pFileInfo->filename.c_str());
				proceedWithDownload = false;
				success = true;
			}
			else
			{
				LogString(FLASHING, "[%s] %s already exist, but has a different checksum.", __FUNCTION__, pFileInfo->filename.c_str());
			}
		}
		else
		{
			LogString(FLASHING, "[%s] %s doesn't exist on media card.", __FUNCTION__, pFileInfo->filename.c_str());
		}
	}
	
	if (proceedWithDownload)
	{
		LogString(FLASHING, "[%s] Downloading file: %s, url: %s destFilename: %s", __FUNCTION__, pFileInfo->filename.c_str(), pFileInfo->url.c_str(), destFilename.c_str());

		ofstream destFileStream(destFilename, ofstream::binary | ofstream::trunc);
		if (destFileStream.is_open())
		{
			destFileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			CWatchDog::Instance().Checkin();

			long statusCode(0);
			if (HttpGet(pFileInfo->url, &destFileStream, statusCode, false, HTTP_GET_TIMEOUT_IN_SECONDS, &CFileDownloader::DownloadProgressCallback)
				&& HTTP_STATUS_OK == statusCode)
			{
				LogString(FLASHING, "[%s] %s downloaded successfully", __FUNCTION__, pFileInfo->filename.c_str());
				CUtilities::FlushVolume();
				success = true;
			}
			else
			{
				LogString(FLASHING, "[%s] Error : %s download failed", __FUNCTION__, pFileInfo->filename.c_str());
				LogString(ERROR_LOG, "[%s] Error : %s download failed", __FUNCTION__, pFileInfo->filename.c_str());
				error = DOWNLOAD_STATUS_DOWNLOAD_ERROR;
			}
		}
		else
		{
			LogString(FLASHING, "[%s] Error : unable to create output file %s, media card missing or full", __FUNCTION__, destFilename.c_str());
			LogString(ERROR_LOG, "[%s] Error : unable to create output file %s, media card missing or full", __FUNCTION__, destFilename.c_str());
			error = DOWNLOAD_STATUS_MEDIA_CARD_ERROR;
		}
	}

	return success;
}

int CFileDownloader::DownloadProgressCallback(void *clientp, int64_t dltotal, int64_t dlnow, int64_t ultotal, int64_t ulnow)
{
	DWORD currentVal = dlnow / 1000000; // in MB
	DWORD totalVal = dltotal / 1000000; // in MB
	DWORD currentTickCount = CUtilities::GetTickCount();

	if (currentVal > 0 && totalVal > 0 
		&& (currentTickCount - m_progressCaptureTickCount > DOWNLOAD_PROGRESS_INTERVAL))
	{
		CWatchDog::Instance().Checkin();
		m_progressCaptureTickCount = currentTickCount;
		byte progress = floor((currentVal * 100.0) / totalVal);

		m_pFlashStatus->SetPercentComplete(progress);
		m_pInternals->UpdateFirmareDownloadStatus(*m_pFlashStatus);
	}

	return 0;
}

std::string CFileDownloader::GetChecksum(Filetype type, const std::string &filename) const
{
	std::string command;
	switch (type)
	{
		case Filetype::FILE_TYPE_OS:
		case Filetype::FILE_TYPE_EXE:
		case Filetype::FILE_TYPE_FPGA:
			command = "sha1sum";
		break;
		default:
			command = "md5sum";
		break;
	}

	CWatchDog::Instance().Checkin();
	const std::string checksumCommand(FormatString("%s '%s' | awk '{print $1}'", command.c_str(), filename.c_str()));
	LogString(FLASHING, "[%s] %s", __FUNCTION__, checksumCommand.c_str());
	const std::string checksum(CUtilities::GetSystemResultString(checksumCommand));
	CWatchDog::Instance().Checkin();

	return checksum;
}

bool CFileDownloader::ConsumeActiveDownload()
{
	bool success = false;
	switch (m_pActiveDownloadInfo->type)
	{
		case Filetype::FILE_TYPE_OS:
		case Filetype::FILE_TYPE_EXE:
			success = ConsumeActiveDownload_OS_EXE();
		break;
		case Filetype::FILE_TYPE_FPGA:
			success = ConsumeActiveDownload_FPGA();
		break;
		case Filetype::FILE_TYPE_SKIN:
			success = ConsumeActiveDownload_SkinMedia();
		break;
		case Filetype::FILE_TYPE_CAMPAIGN_MEDIA:
			success = ConsumeActiveDownload_CampaignMedia();
		break;
		case Filetype::FILE_TYPE_CONFIG:
			success = ConsumeActiveDownload_Config();
		break;
		case Filetype::FILE_TYPE_CUSTOM_CONTENT:
			success = ConsumeActiveDownload_CustomContent();
		break;
		default:
			break;
	}
	return success;
}

bool CFileDownloader::ConsumeActiveDownload_OS_EXE()
{
	const std::string upgradePath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE);
	const std::string extractionPath = ONELINK_CONTENTTEMP_PATH + "extract_" + m_pActiveDownloadInfo->filename;

	// make sure extraction path is clear.
	CUtilities::DeleteDirectory(extractionPath);
	CUtilities::CreateDirectory(extractionPath);

	LogString(FLASHING, "[%s] Extracting file %s at location %s ...",
		__FUNCTION__, m_pActiveDownloadInfo->filename.c_str(), extractionPath.c_str());
	UpdateUIStatus(DOWNLOAD_STATUS_EXTRACTING_CONTENTS);

	bool success = Unzip(ONELINK_CONTENTTEMP_PATH, m_pActiveDownloadInfo->filename, extractionPath, false);

	if (success)
	{
		LogString(FLASHING, "[%s] File extracted successfully.", __FUNCTION__);

		UpdateUIStatus(DOWNLOAD_STATUS_SUCCESS);

		LogString(FLASHING, "[%s] Validating checksums in manifest.sha1 file...", __FUNCTION__);

		std::string checksumValidationCmd(FormatString("cd %s && sha1sum -c manifest.sha1", extractionPath.c_str()));
		success = (0 == system(checksumValidationCmd.c_str()));
		CWatchDog::Instance().Checkin();

		if (success)
		{
			LogString(FLASHING, "[%s] manifest.sha1 checksums verified.", __FUNCTION__);
			LogString(FLASHING, "[%s] Moving extracted files to upgrade folder...", __FUNCTION__);

			success = MoveFiles(extractionPath, upgradePath);

			if (success)
			{
				LogString(FLASHING, "[%s] Files moved to upgrade path : %s", __FUNCTION__, upgradePath.c_str());
			}
			else
			{
				LogString(FLASHING, "[%s] Unable to move extracted files.", __FUNCTION__);
				LogString(ERROR_LOG, "[%s] Unable to move extracted files from %s to upgrade path %s",
					__FUNCTION__, extractionPath.c_str(), upgradePath.c_str());
			}
		}
		else
		{
			LogString(FLASHING, "[%s] manifest.sha1 checksum verification failed on extracted contents %s",
				__FUNCTION__, extractionPath.c_str());
			LogString(ERROR_LOG, "[%s] manifest.sha1 checksum verification failed on extracted contents %s",
				__FUNCTION__, extractionPath.c_str());
		}
	}
	else
	{
		// Sleep to make sure UI displays the extracting contents status
		CUtilities::Sleep(YIELD_FOR_UI_PROGRESS_STATUS);

		LogString(FLASHING, "[%s] File extraction failed.", __FUNCTION__);
		LogString(ERROR_LOG, "[%s] File extraction failed, filename=%s, path=%s",
			__FUNCTION__, m_pActiveDownloadInfo->filename.c_str(), extractionPath.c_str());
		UpdateUIStatus(DOWNLOAD_STATUS_EXTRACTION_ERROR);
	}

	CUtilities::DeleteDirectory(extractionPath);
	return success;
}

bool CFileDownloader::ConsumeActiveDownload_FPGA()
{
	const std::string extractionPath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_MEDIADISK) + "/FPGA/";

	LogString(FLASHING, "[%s] Extracting file %s at location %s ...",
		__FUNCTION__, m_pActiveDownloadInfo->filename.c_str(), extractionPath.c_str());
	UpdateUIStatus(DOWNLOAD_STATUS_EXTRACTING_CONTENTS);

	bool success = Unzip(ONELINK_CONTENTTEMP_PATH, m_pActiveDownloadInfo->filename, extractionPath, true);

	if (success)
	{
		LogString(FLASHING, "[%s] File extracted successfully.", __FUNCTION__);
		UpdateUIStatus(DOWNLOAD_STATUS_SUCCESS);
	}
	else
	{
		// Sleep to make sure UI displays the extracting contents status
		CUtilities::Sleep(YIELD_FOR_UI_PROGRESS_STATUS);

		LogString(FLASHING, "[%s] File extraction failed.", __FUNCTION__);
		LogString(ERROR_LOG, "[%s] File extraction failed, filename=%s, path=%s",
			__FUNCTION__, m_pActiveDownloadInfo->filename.c_str(), extractionPath.c_str());
		UpdateUIStatus(DOWNLOAD_STATUS_EXTRACTION_ERROR);
		CUtilities::DeleteDirectory(extractionPath);
	}

	return success;
}

bool CFileDownloader::ConsumeActiveDownload_Config()
{
	std::string filename = m_pActiveDownloadInfo->filename;
	filename.erase(std::remove(filename.begin(), filename.end(), ' '), filename.end()); // remove spaces
	const std::string extractionPath = ONELINK_CONTENTTEMP_PATH + "extract_" + filename;

	// make sure extraction path is clear.
	CUtilities::DeleteDirectory(extractionPath);
	CUtilities::CreateDirectory(extractionPath);

	LogString(FLASHING, "[%s] Extracting file %s at location %s ...",
		__FUNCTION__, m_pActiveDownloadInfo->filename.c_str(), extractionPath.c_str());
	UpdateUIStatus(DOWNLOAD_STATUS_EXTRACTING_CONTENTS);

	bool success = Unzip(ONELINK_CONTENTTEMP_PATH, m_pActiveDownloadInfo->filename, extractionPath, false);

	if (success)
	{
		LogString(FLASHING, "[%s] File extracted successfully.", __FUNCTION__);
		UpdateUIStatus(DOWNLOAD_STATUS_SUCCESS);

		//Process Certificate files if any exist
		ProcessesCertFiles(extractionPath);

		if (CUtilities::FileCount(extractionPath, {}))
		{
			// Move extracted files to sentinel data folder.
			const std::string destPath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA);
			success = MoveFiles(extractionPath, destPath);

			if (success)
			{
				LogString(FLASHING, "[%s] Files moved to destination path : %s", __FUNCTION__, destPath.c_str());

				LogString(FLASHING, "[%s] Calling ProcessDefaultDataDirectoryUpdated() of internals.", __FUNCTION__);
				const std::string messageId(m_pActiveDownloadInfo->additionalInfo);
				m_pInternals->ProcessDefaultDataDirectoryUpdated(messageId, m_pActiveDownloadInfo->filename);
			}
			else
			{
				LogString(FLASHING, "[%s] Unable to move extracted files.", __FUNCTION__);
				LogString(ERROR_LOG, "[%s] Unable to move extracted files from %s to destination path %s",
					__FUNCTION__, extractionPath.c_str(), destPath.c_str());
			}
		}
	}
	else
	{
		// Sleep to make sure UI displays the extracting contents status
		CUtilities::Sleep(YIELD_FOR_UI_PROGRESS_STATUS);

		LogString(FLASHING, "[%s] File extraction failed.", __FUNCTION__);
		LogString(ERROR_LOG, "[%s] File extraction failed, filename=%s, path=%s",
			__FUNCTION__, m_pActiveDownloadInfo->filename.c_str(), extractionPath.c_str());
		UpdateUIStatus(DOWNLOAD_STATUS_EXTRACTION_ERROR);
		CUtilities::DeleteDirectory(extractionPath);
	}

	return success;
}

/// \fn void CFileDownloader::ProcessesCertFiles(const std::string &extractionPath)
/// \brief Processes the certificate files into the cert directory then prep UI for restart.
/// \param  extractionPath Full pathname of the extraction file.
void CFileDownloader::ProcessesCertFiles(const std::string &extractionPath)
{
	const std::initializer_list<std::string> certTypes = 
	  { "*.pem" ,"*.PEM" ,"*.crt" ,"*.CRT" ,"*.pfx" ,"*.PFX" ,"*.p12" ,"*.P12" ,"*.cer" ,"*.CER" ,"*.der" ,"*.DER" ,"*.p7b" ,"*.P7B" ,"*.p7r" ,"*.P7R" ,"*.spc" ,"*.SPC"};

	auto certStore = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_ADDITIONAL_CERTIFICATE_STORE);
	if (!CUtilities::DoesDirExist(certStore.c_str()))
	{
		CUtilities::CreateDirectory(certStore.c_str());
	}

	if (CUtilities::FileCount(extractionPath, certTypes) > 0)
	{
		LogString(FLASHING, "[%s] Moving certificates...", __FUNCTION__);
		std::string fileTypes = "";

		for (auto certType : certTypes) {
			if (!certType.empty())
			{
				fileTypes.append(certType);
				fileTypes.append(" ");
			}
		}

		std::string moveCmd = "cd '" + extractionPath + "' && mv " + fileTypes + " " + certStore;
		LogString(FLASHING, "[%s] %s", __FUNCTION__, moveCmd.c_str());

		// NOTE: We are not checking the successful execution of move command because
		//       here move command has multiple file filters, so it will succeed 
		//       only when all filter types are present.
		system(moveCmd.c_str());
		LogString(FLASHING, "[%s] Certificates moved successfully.", __FUNCTION__);

		CWatchDog::Instance().Checkin();
		m_pInternals->ProcessRestartServiceWindow();
	}
}

bool CFileDownloader::ConsumeActiveDownload_CustomContent()
{
	bool success(false);
	const std::string dynamicContentKey(m_pActiveDownloadInfo->additionalInfo);

	if (!dynamicContentKey.empty())
	{
		const std::string extractionPath = ONELINK_DYNAMIC_CONTENT_PATH + dynamicContentKey;

		// make sure extraction path is clear.
		CUtilities::DeleteDirectory(extractionPath);
		CUtilities::CreateDirectory(extractionPath);

		LogString(FLASHING, "[%s] Extracting file %s at location %s ...",
			__FUNCTION__, m_pActiveDownloadInfo->filename.c_str(), extractionPath.c_str());
		UpdateUIStatus(DOWNLOAD_STATUS_EXTRACTING_CONTENTS);

		if (Unzip(ONELINK_CONTENT_PATH, m_pActiveDownloadInfo->filename, extractionPath, true))
		{
			LogString(FLASHING, "[%s] File extracted successfully.", __FUNCTION__);
			UpdateUIStatus(DOWNLOAD_STATUS_SUCCESS);

			success = true;
			m_mediaPackageManager->SetOneLinkContent(dynamicContentKey);
		}
		else
		{
			// Sleep to make sure UI displays the extracting contents status
			CUtilities::Sleep(YIELD_FOR_UI_PROGRESS_STATUS);

			LogString(FLASHING, "[%s] File extraction failed.", __FUNCTION__);
			LogString(ERROR_LOG, "[%s] File extraction failed, filename=%s, path=%s",
				__FUNCTION__, m_pActiveDownloadInfo->filename.c_str(), extractionPath.c_str());
			UpdateUIStatus(DOWNLOAD_STATUS_EXTRACTION_ERROR);
			CUtilities::DeleteDirectory(extractionPath);
		}
	}
	else
	{
		// Sleep to make sure UI displays the extracting contents status
		CUtilities::Sleep(YIELD_FOR_UI_PROGRESS_STATUS);

		LogString(FLASHING, "[%s] dynamicContentKey is empty.", __FUNCTION__);
		LogString(ERROR_LOG, "[%s] dynamicContentKey is empt, filename=%s", __FUNCTION__, m_pActiveDownloadInfo->filename.c_str());
		UpdateUIStatus(DOWNLOAD_STATUS_EXTRACTION_ERROR);
	}

	return success;
}

bool CFileDownloader::ConsumeActiveDownload_CampaignMedia()
{
	UpdateUIStatus(DOWNLOAD_STATUS_SUCCESS);

	const std::string filename = ONELINK_CONTENTTEMP_PATH + m_pActiveDownloadInfo->filename;
	const std::string targetPath = m_pActiveDownloadInfo->additionalInfo;

	// Move the file to its target path
	bool success = MoveFiles(filename, targetPath, false);

	if (success)
	{
		LogString(FLASHING, "[%s] File moved to destination path : %s", __FUNCTION__, targetPath.c_str());
		if (m_isPurgeCurrentDownload)
		{
			LogString(FLASHING, "[%s] Purging this campaign media.", __FUNCTION__);
		}
		else
		{
			m_pInternals->PendingCampaignMediaDownloaded();
		}
	}
	else
	{
		LogString(FLASHING, "[%s] Unable to move the file.", __FUNCTION__);
		LogString(ERROR_LOG, "[%s] Unable to move the file %s to destination path %s",
			__FUNCTION__, filename.c_str(), targetPath.c_str());
	}

	return success;
}

bool CFileDownloader::ConsumeActiveDownload_SkinMedia()
{
	UpdateUIStatus(DOWNLOAD_STATUS_SUCCESS);

	const std::string filename = ONELINK_CONTENTTEMP_PATH + m_pActiveDownloadInfo->filename;
	const std::string targetPath = m_pActiveDownloadInfo->additionalInfo;

	if (m_pActiveDownloadInfo->filename == "Skin.json")
	{
		ifstream inputFile(filename);
		if (inputFile.is_open())
		{
			// read skin data
			std::ostringstream dataStream;
			dataStream << inputFile.rdbuf();
			std::string data(dataStream.str());
			inputFile.close();

			// find and replace
			std::string findStr(":url(/assets/images/");
			std::string replaceStr(":url(/custom/assets/");
			CUtilities::FindAndReplace(data, findStr, replaceStr);

			// write file
			ofstream outputFile;
			outputFile.open(filename);
			outputFile << data;
			outputFile.close();

			LogString(FLASHING, "[%s] Skin.json data modified, findStr = %s, replaceStr = %s",
				__FUNCTION__, findStr.c_str(), replaceStr.c_str());
		}
		else
		{
			LogString(FLASHING, "[%s] Error : Unable to open Skin.json file for modification.", __FUNCTION__);
			LogString(ERROR_LOG, "[%s] Error : Unable to open Skin.json file for modification.", __FUNCTION__);
		}
	}

	// Move the file to its target path
	bool success = MoveFiles(filename, targetPath, false);

	if (success)
	{
		LogString(FLASHING, "[%s] File moved to destination path : %s", __FUNCTION__, targetPath.c_str());
		m_pInternals->PendingSkinMediaDownloaded();
	}
	else
	{
		LogString(FLASHING, "[%s] Unable to move the file.", __FUNCTION__);
		LogString(ERROR_LOG, "[%s] Unable to move the file %s to destination path %s",
			__FUNCTION__, filename.c_str(), targetPath.c_str());
	}

	return success;
}

bool CFileDownloader::PrepareLiteUnitUpgradeFolder(const char *pUpgradePath)
{
	// LITE device
	LogString(FLASHING, "[%s] LITE device detected, manifest.sha1 checksums already verified.", __FUNCTION__);
	bool success = true;

	// manifest.sha1 checksums already verified, so now we need to determine which case we have:
	// 1. full upgrade in SMD, including sentinel.img: should not touch/prep anything
	// 2. full OS upgrade, no sentinel.img: should copy/sha1 *just* sentinel.img from current firmware
	// 3. only sentinel.img upgrade: should copy current firmware and sha1 *just* the new sentinel.img
	// 4. special "INCREMENTAL" file for incremental upgrade, should copy full firmware plus new firmware, and fully regenerate manifest.sha1

	bool upgradeHasSentinel(m_isEXEDownloaded);
	bool upgradeOnlySentinel(upgradeHasSentinel && !m_isOSDownloaded);
	bool incrementalUpgrade(false);
	string incrementalUpgradeFile = FormatString("%s/%s", pUpgradePath, "INCREMENTAL");
	if (CUtilities::FileExists(incrementalUpgradeFile))
	{
		LogString(FLASHING, "[%s] Incremental upgrade file detected...", __FUNCTION__);
		incrementalUpgrade = true;
		CUtilities::DeleteFile(incrementalUpgradeFile.c_str());
	}

	if (!upgradeHasSentinel || upgradeOnlySentinel || incrementalUpgrade) // cases 2., 3., 4., respectively (from above)
	{
		bool copyEverything = (upgradeOnlySentinel || incrementalUpgrade); // cases 3. and 4. respectively (from above)
		string copyCommandWithoutOverride(FormatString("cp -vnpr %s/%s %s", CDirectoryManager::FIRMWAREPATH_LITE,
			copyEverything ? "*" : "sentinel.img", pUpgradePath));

		LogString(FLASHING, "[%s] Copying %s from original firmware to update directory [%s] ...",
			__FUNCTION__, copyEverything ? "rest of the firmware files" : "sentinel.img", copyCommandWithoutOverride.c_str());

		CWatchDog::Instance().Checkin();
		success = (0 == system(copyCommandWithoutOverride.c_str()));
		CWatchDog::Instance().Checkin();
		if (!success)
		{
			// Unexpected failure
			LogString(FLASHING, "[%s] Unknown Error", __FUNCTION__);
			LogString(ERROR_LOG, "[%s] Unknown Error", __FUNCTION__);
		}
	}

	LogString(FLASHING, "[%s] NCompass LITE firmware upgrade preparation: %s", __FUNCTION__, success ? "SUCCEEDED" : "FAILED");

	return success;
}

bool CFileDownloader::Unzip(const std::string& sourceDirectoryPath, const std::string &filename, const std::string &path, bool createPath) const
{
	bool success(false);

	CWatchDog::Instance().Checkin();

	bool folderSuccessfullyCreated(true);
	if (createPath)
	{
		folderSuccessfullyCreated = (CUtilities::DoesDirExist(path.c_str()) ? true : CUtilities::CreateDirectory(path));
		LogString(FLASHING, "[%s] Directory %s creation : %s", __FUNCTION__, path.c_str(), folderSuccessfullyCreated ? "passed" : "failed");
	}

	if (folderSuccessfullyCreated)
	{
		const std::string unzipCmd = "cd '" + sourceDirectoryPath + "' && unzip '" + filename + "' -d '" + path + "'";
		std::string result = CUtilities::GetSystemResultString(unzipCmd);
		CWatchDog::Instance().Checkin();

		LogString(FLASHING, "[%s] %s result=%s", __FUNCTION__, unzipCmd.c_str(), result.c_str());

		// on success, unzip command returns the extracted file names.
		// on failure, it returns none.
		success = !result.empty();

		LogString(FLASHING, "[%s] %s success=%d", __FUNCTION__, filename.c_str(), success);
	}
	else
	{
		LogString(FLASHING, "[%s] %s failed to be created.", __FUNCTION__, path.c_str());
		LogString(ERROR_LOG, "[%s] %s failed to be created.", __FUNCTION__, path.c_str());
	}

	return success;
}

bool CFileDownloader::MoveFiles(const std::string &src, const std::string &destDir, bool movingMultipleFiles) const
{
	if (!CUtilities::DoesDirExist(destDir.c_str()))
	{
		CUtilities::CreateDirectory(destDir);
	}

	std::string moveCmd;
	if (movingMultipleFiles)
	{
		moveCmd = FormatString("mv '%s/'* '%s'", src.c_str(), destDir.c_str());
	}
	else
	{
		moveCmd = FormatString("mv '%s' '%s'", src.c_str(), destDir.c_str());
	}

	LogString(FLASHING, "[%s] %s", __FUNCTION__, moveCmd.c_str());
	bool success = (0 == system(moveCmd.c_str()));
	CWatchDog::Instance().Checkin();
	return success;
}

void CFileDownloader::PerformOsExeUpgradeIfNeeded()
{
	const std::string upgradePath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE);

	// check if OS or EXE is available and available downloads are downloaded.
	if ((m_isOSAvailable || m_isEXEAvailable)
		&& (!m_isOSAvailable || m_isOSDownloaded)
		&& (!m_isEXEAvailable || m_isEXEDownloaded))
	{
		bool success = true;
		if (CPlatform::Instance().IsLiteDevice() || CPlatform::Instance().IsNexusDevice())
		{
			success = PrepareLiteUnitUpgradeFolder(upgradePath.c_str());
			CWatchDog::Instance().Checkin();
		}

		if (success)
		{
			// Create the manifest.sha1
			LogString(FLASHING, "[%s] Proceeding with firmware upgrade, Re-creating manifest.sha1 file at upgrade path...", __FUNCTION__);
			std::string createManifestCmd(FormatString(
				"cd %s && %s", upgradePath.c_str(), CREATE_MANIFEST_CMD.c_str()));
			success = (0 == system(createManifestCmd.c_str()));
			CWatchDog::Instance().Checkin();

			if (success)
			{
				LogString(FLASHING, "[%s] manifest.sha1 file created successfully.", __FUNCTION__);

				// All set for firmware upgrade to continue by boot script
				m_pInternals->ProcessOsUpgrade();

				m_isOsExeUpgradeReady = true;
				LogString(FLASHING, "[%s] System ready for upgrade.", __FUNCTION__);
			}
			else
			{
				LogString(FLASHING, "[%s] Unable to create manifest.sha1 file at upgrade path.", __FUNCTION__);
				LogString(ERROR_LOG, "[%s] Unable to create manifest.sha1 file at upgrade path.", __FUNCTION__);
			}
		}

		if (!success)
		{
			// Delete the upgrade folder
			LogString(FLASHING, "[%s] Deleting the upgrade folder...", __FUNCTION__);
			CUtilities::DeleteDirectory(upgradePath);
		}
	}
}

UINT AFX_CDECL CFileDownloader::_thread(CFileDownloader* pClass)
{
	try
	{
		pClass->ThreadMain();
	}
	catch (...)
	{
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_FILE_DOWNLOADER);
	}
	return 0;
}