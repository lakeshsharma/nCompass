#include "stdafx.h"
#include "Diagnostics.h"
#include "Logger.h"
#include "LogString.h"
#include "DirectoryManager.h"
#include "DisplayManager.h"
#include "Platform.h"
#include "Hardware/WatchDog.h"
#include "Util/Convert.h"
#include "Util/Path.h"
#include "Util/String.h"
#include "RestartManager.h"
#include "ProcessDump/WebDiagnostics.h"

using namespace std;

const LPCTSTR INITIAL_NVRAM_BUFFER_VALUE = "Initialization of error log buffer. Uncontrolled shutdown possibly preceeded.\n";
const LPCTSTR INITIAL_ERROR_LOG = "Initial error log entry.";

LPCTSTR const CSmartDataLogger::LPCTSTR_FALSE(_T("false"));
LPCTSTR const CSmartDataLogger::LPCTSTR_TRUE(_T("true"));

const string CSmartDataLogger::DEBUG_FILE_NAME("debug.txt");
const string CSmartDataLogger::m_debugPath("/opt/ncompass/logs");
const string CSmartDataLogger::m_debugFilePath(m_debugPath + "/" + DEBUG_FILE_NAME);

const LPCTSTR CSmartDataLogger::TIME_FORMAT_FILE_NAME("%m-%d-%Y-%H-%M-%S");
const LPCSTR CSmartDataLogger::SYSLOG_IDENTIFIER_NAME("SYSLOG");
const LPCSTR CSmartDataLogger::SYSCMD_IDENTIFIER_NAME("SYSCMD");
const LPCTSTR CSmartDataLogger::PATH_SENT_DEBUG_FILE_ID(".sdf");
const DWORD CSmartDataLogger::SLEEP_TIME(1000); // 1000 milliseconds = 1 second.
const byte DATA_TIMESTAMP = 0x40;
const time_t CSmartDataLogger::FLUSH_LOGS_RATE(SECONDS_PER_MINUTE);//every minute
const time_t CSmartDataLogger::ZIP_LOGS_RATE(12 * SECONDS_PER_HOUR); //12 hours in seconds
const time_t CSmartDataLogger::MAX_TIME_OF_ZIPPED_LOGS_TO_KEEP(7 * SECONDS_PER_DAY);
const string DEBUG_KEY("8D33AE49-D558-4aca-B99B-B7C65C97DCBD");
CSmartDataLogger *CSmartDataLogger::m_instance(nullptr);
std::recursive_mutex CSmartDataLogger::m_instanceCS;
std::recursive_mutex CSmartDataLogger::_logFileCritSection;
const int ZIP_LOGS_COMPLETE_PERCENTAGE(60);
const int COMBINED_ZIP_COMPLETE_PERCENTAGE(80);
const int PROGRESS_BAR_STEP_SIZE(20);
const string STATUS_SUCESS("Success");
const string STATUS_FAIL("Fail");
const string PARTIAL_LOG_NAME_TO_REMOVE("nCompass_Logs_");
const string DEBUG_FILE_KEY("debugkey");
const string AUTO_ARCHIVE_DESCRIPTION("auto_archive");
const string COMBINED_ZIP_LOG("CombinedLogsForSlotMastID_");
const size_t MAXIMUM_FILE_NAME_LENGTH(251);//Need to accomodate for .zip
const DWORD DISK_SPACE_MINIMUM_REQUIRED_FOR_ARCHIVE_IN_MB(1000);
#define MINUTE_TO_MS_FACTOR 60000


static EnumMap<ZipLogActionType>::EnumPair ZipLogActionTypes[] =
{
	{ ZipLogActionType::AutoArchive, "AutoArchive" },
	{ ZipLogActionType::Archive, "Archive" },
	{ ZipLogActionType::ArchiveFromFLM, "ArchiveFromFLM" },	
	{ ZipLogActionType::None, "None" },
	{ ZipLogActionType::Shutdown, "Shutdown" },
};

EnumMap<ZipLogActionType> ZipLogActionTypeEnumMap(ZipLogActionTypes);

/// <summary>
/// Gets a reference to the global instance of CSmartDataLogger.
/// </summary>
/// <returns>
/// A reference to the global CSmartDataLogger instance.
/// </returns>
CSmartDataLogger &CSmartDataLogger::Instance()
{
	lock_guard<recursive_mutex> lock(m_instanceCS);
	if (m_instance == nullptr)
	{
		m_instance = new CSmartDataLogger();
	}

	return *m_instance;
}

CSmartDataLogger::CSmartDataLogger() : CDataSubscriptionProvider("")
, m_zipLogActionType(ZipLogActionType::None)
, m_remoteLoggingEnabled(false)
, m_combinedLogsEnabled(true)
, m_mediaDiskExists(false)
, m_diskSpaceAvailable(false)
, m_manualLoggingInProgress(false)
, m_requestForSubscriptionUpdate(false)
, m_sizeOfZipToBeCopiedToUsb(0)
, m_progressBar(0)
{
	AddTopic(ArchiveLogsProgress);
}

CSmartDataLogger::~CSmartDataLogger(void)
{
	if (!m_instance)
		return;
}

bool CSmartDataLogger::IsActive()
{
	return (m_instance != nullptr);
}

void CSmartDataLogger::CleanupInstance()
{
	lock_guard<recursive_mutex> lock(m_instanceCS);
	if (m_instance)
	{
		delete m_instance;
		m_instance = nullptr;
	}
}

void CSmartDataLogger::Initialize()
{
	CheckIfDiskSpaceIsAvailableForArchive();

	m_zipLogsTime = CUtilities::GetCurrentTime() + ZIP_LOGS_RATE;
	m_logsFlushedTime = CUtilities::GetCurrentTime() + FLUSH_LOGS_RATE;

	CreateLogFiles();

	CheckIfManualLoggingNeedsToBeEnabled();
}

string CSmartDataLogger::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId)
{
	string subscriptionDataResponse;

	if (!m_vProgressData.empty())
	{
		JsonWriteStreamString writeStream;
		JsonWriter writer(writeStream);
		writer.StartObject();
		tProgressData progressData(m_vProgressData.front());
		m_vProgressData.pop();
		writer.Key("InProgress");	writer.Bool(progressData.m_downloadInProgress);
		writer.Key("Status");		writer.String(progressData.m_downloadStatus);
		writer.Key("Percentage");	writer.Int(progressData.m_percentage);
		writer.Key("Message");		writer.String(progressData.m_downloadMessage);
		writer.EndObject();
		subscriptionDataResponse = writeStream.m_str;
		cout << subscriptionDataResponse << endl;
	}

	return subscriptionDataResponse;
}

void CSmartDataLogger::RefreshArchivedFolder()
{
	cout << "CSmartDataLogger::RefreshArchivedFolder" << endl;
	m_listsOfZipFiles.clear();
	if (m_mediaDiskExists)
	{
		m_archivedLogLocation = FormatString("%s/archivedlogs", CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_MEDIADISK).c_str());
		if (!CUtilities::FileExists(m_archivedLogLocation))
		{
			CUtilities::GetSystemResultString(FormatString("mkdir -p %s", m_archivedLogLocation.c_str()));
		}

		string listOfZipFilesTextFilePath(FormatString("%s/listOfZipFiles.txt", m_debugPath.c_str()));
		string getListOfZipFiles(FormatString("ls %s | grep .zip >  %s", m_archivedLogLocation.c_str(), listOfZipFilesTextFilePath.c_str()));
		CUtilities::GetSystemResultString(getListOfZipFiles);

		ifstream listOfZipFilesStream(listOfZipFilesTextFilePath);
		string zipFile;
		std::map<std::string, bool> filesToKeep;
		if (listOfZipFilesStream.is_open())
		{
			const time_t currentTime(CUtilities::GetCurrentTime());
			while (std::getline(listOfZipFilesStream, zipFile))
			{
				size_t pos(zipFile.find(PARTIAL_LOG_NAME_TO_REMOVE));
				string newZipFile(zipFile);
				if (std::string::npos != pos)
				{
					newZipFile.erase(0, PARTIAL_LOG_NAME_TO_REMOVE.length());
					string removePartialLogName(FormatString("mv %s/%s %s/%s", m_archivedLogLocation.c_str(), zipFile.c_str(), m_archivedLogLocation.c_str(), newZipFile.c_str()));
					CUtilities::GetSystemResultString(removePartialLogName);
				}

				struct stat fd;
				string zipFilePath(m_archivedLogLocation);
				zipFilePath.append("/");
				zipFilePath.append(newZipFile);
				if (0 == stat(zipFilePath.c_str(), &fd))
				{
					time_t ageOfZippedLogFile(currentTime - fd.st_mtime);
					if (ageOfZippedLogFile < MAX_TIME_OF_ZIPPED_LOGS_TO_KEEP  && string::npos == zipFile.find(COMBINED_ZIP_LOG))
					{
						filesToKeep[newZipFile] = true;
						m_listsOfZipFiles.push_back(newZipFile);
					}
				}
			}
		}

		CUtilities::DeleteDirectory(m_archivedLogLocation, true, &filesToKeep);
		FlushDebugVolume();
		sort(m_listsOfZipFiles.begin(), m_listsOfZipFiles.end(), greater<string>());
	}
}

void CSmartDataLogger::RefreshLogFiles()
{
	cout << "RefreshLogFiles\n";
	std::map<std::string, bool> filesToKeep;
	filesToKeep[DEBUG_FILE_NAME] = true;
	CUtilities::DeleteDirectory(m_debugPath, true, &filesToKeep);
	CreateLogFiles();

	if (m_combinedLogsEnabled && m_combinedZipLogPath.empty() && m_diskSpaceAvailable)
	{
		//The following lines of code will zip all the zipped logs into a single zipped file.
		m_combinedZipFileName = (FormatString("/%s%s.zip", COMBINED_ZIP_LOG.c_str(), to_string(m_slotMastId).c_str()));
		if (!m_downloadZipFilePath.empty())
		{
			m_downloadZipFilePath.append(m_combinedZipFileName);
		}
		m_combinedZipLogPath = (FormatString("%s%s", m_debugPath.c_str(), m_combinedZipFileName.c_str()));
		string zipCombinedLogs = FormatString("zip -0 -r -j %s %s >/dev/null &", m_combinedZipLogPath.c_str(), m_archivedLogLocation.c_str());
		system(zipCombinedLogs.c_str());
		cout << zipCombinedLogs << endl;
	}
	else
	{
		m_combinedZipFileName.clear();
		m_combinedZipLogPath.clear();
	}
}

void CSmartDataLogger::ZipAllLogs()
{
	CWatchDog::Instance().Checkin();

	string copyDiagnosticPages(FormatString("cp %s/* %s", CDirectoryManager::HTML_FILE_DRIVE_AND_PATH, m_debugPath.c_str()));
	CUtilities::GetSystemResultString(copyDiagnosticPages);

	string combinedZipLogs(FormatString("%s%s", m_debugPath.c_str(), m_combinedZipFileName.c_str()));
	if (CUtilities::FileExists(combinedZipLogs))
	{
		CUtilities::DeleteFile(combinedZipLogs.c_str());
	}

	string zipLogsInRamToDisk(FormatString("zip -r -j %s %s", m_archiveZipFilePath.c_str(), m_debugPath.c_str()));
	//If not shutting down, we should do the zipping as a nonblocking call.
	if (ZipLogActionType::Shutdown != m_zipLogActionType)
	{
		zipLogsInRamToDisk.append(" >/dev/null &");
	}
	system(zipLogsInRamToDisk.c_str());
	cout << zipLogsInRamToDisk << endl;
}

void CSmartDataLogger::InitializeProgressSubscriptionData()
{
	{
		lock_guard<recursive_mutex> lock(_logFileCritSection);
		m_progressBar = 0;
		m_sizeOfZipToBeCopiedToUsb = 0;
	}
	std::queue<tProgressData> empty;
	std::swap(m_vProgressData, empty);
}

bool CSmartDataLogger::NeedToRequestForSubscriptionUpdate()
{
	bool requestForSubscriptionUpdate(m_requestForSubscriptionUpdate);
	m_requestForSubscriptionUpdate = false;
	return requestForSubscriptionUpdate;
}

void CSmartDataLogger::UpdateProgressBar(const tProgressData& progressData)
{
	m_requestForSubscriptionUpdate = true;
	m_vProgressData.push(progressData);
}

bool CSmartDataLogger::FindDebugKey(std::string& debugKeyDirectory)
{
	bool debugKeyFound(false);

	// cycle thru the USB paths
	int usbIndex = 0;
	bool moreUSBPaths = true;
	while (!debugKeyFound && moreUSBPaths)
	{
		string directoryPath = CDirectoryManager::Instance().USBPath(usbIndex);
		if (!directoryPath.empty())
		{
			string debugFileKeyPath(directoryPath + +"/" + DEBUG_FILE_KEY);
			if (CUtilities::FileExists(debugFileKeyPath))
			{
				cout << "FileExists=" << debugFileKeyPath << endl;
				ifstream ifs(debugFileKeyPath, std::ifstream::in);
				if (ifs.is_open())
				{
					string key;
					ifs >> key;
					ifs.close();
					if (DEBUG_KEY == key)
					{
						debugKeyDirectory = directoryPath;
						debugKeyFound = true;
					}
				}
			}
		}
		else
		{
			moreUSBPaths = false;
		}
		usbIndex++;
	}

	return debugKeyFound;
}

bool CSmartDataLogger::FileExists(const std::string& filePath)
{
	return CUtilities::FileExists(filePath);
}

TiltCode CSmartDataLogger::ArchiveLogs(const std::string& description)
{
	TiltCode tiltCode(tltUnknown);

	if (m_instance)
	{
		CheckIfDiskSpaceIsAvailableForArchive();
		InitializeProgressSubscriptionData();

		if (ZipLogActionType::None != m_zipLogActionType)
		{
			tiltCode = tltArchivingLogsDeniedAlreadyInProgress;
			UpdateProgressBar(tProgressData(false, 0, "ZIPPING_LOGS_ALREADY_IN_PROGRESS", STATUS_FAIL));
		}
		else if (!m_remoteLoggingEnabled && !m_manualLoggingInProgress)
		{
			tiltCode = tltArchivingLogsDeniedRemoteDebuggingDisabled;
			UpdateProgressBar(tProgressData(false, 0, "REMOTE_DEBUGGING_NOT_ENABLED", STATUS_FAIL));
		}
		else if (!m_mediaDiskExists)
		{
			tiltCode = tltArchivingLogsFailedNoDisk;
			UpdateProgressBar(tProgressData(false, 0, "MEDIA_DISK_MISSING", STATUS_FAIL));

		}
		else if (!m_diskSpaceAvailable)
		{
			tiltCode = tltArchivingLogsFailedNoDiskSpace;
			UpdateProgressBar(tProgressData(false, 0, "DISK_SPACE_NOT_AVAILABLE", STATUS_FAIL));
		}
		else
		{
			UpdateProgressBar(tProgressData(true, m_progressBar, "ARCHIVING_LOGS", ""));
			string debugKeyDirectory;
			FindDebugKey(debugKeyDirectory);
			tiltCode = WriteLogsToMediaDisk(ZipLogActionType::Archive, debugKeyDirectory, description);
		}
	}

	return tiltCode;
}

TiltCode CSmartDataLogger::WriteLogsToMediaDisk(ZipLogActionType zipLogActionType, const string& usbPath, std::string description)
{
	TiltCode tiltCode(tltUnknown);
	
	if (m_instance)
	{
		cout << "\n\n\n\n******************WriteLogsToMediaDisk zipLogActionType=" << FormatString("%s.", ZipLogActionTypeEnumMap.At(zipLogActionType, ""))
			 << " current m_zipLogActionType= " << FormatString("%s.", ZipLogActionTypeEnumMap.At(m_zipLogActionType, ""))
			 << " description=" << description << " usbPath=" << usbPath << "******************\n";
		CheckIfDiskSpaceIsAvailableForArchive();
		if (ZipLogActionType::None != m_zipLogActionType)
		{
			tiltCode = tltArchivingLogsDeniedAlreadyInProgress;		
		}
		else if (!m_remoteLoggingEnabled && !m_manualLoggingInProgress)
		{
			tiltCode = tltArchivingLogsDeniedRemoteDebuggingDisabled;
		}
		else if (!m_mediaDiskExists)
		{
			tiltCode = tltArchivingLogsFailedNoDisk;
		}
		else if (!m_diskSpaceAvailable)
		{
			tiltCode = tltArchivingLogsFailedNoDiskSpace;
		}
		else
		{
			lock_guard<recursive_mutex> lock(_logFileCritSection);
			if (!m_combinedZipLogPath.empty())
			{
				CUtilities::DeleteFile(m_combinedZipLogPath.c_str());
				m_combinedZipLogPath.clear();
			}

			m_zipLogActionType = zipLogActionType;
			if (!description.empty())
			{
				std::transform(description.begin(), description.end(), description.begin(), ::tolower);
				description.erase(std::remove(description.begin(), description.end(), '/'), description.end());
				description.erase(std::remove(description.begin(), description.end(), ':'), description.end());
				description.erase(std::remove(description.begin(), description.end(), '@'), description.end());
				std::replace(description.begin(), description.end(), ' ', '_');
			}

			string dateAndTimeStr(CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), TIME_FORMAT_FILE_NAME).c_str());
			m_zipFileName = FormatString("SlotMastID_%s_%s_%s", to_string(m_slotMastId).c_str(), dateAndTimeStr.c_str(), description.c_str());
			if (m_zipFileName.size() >= MAXIMUM_FILE_NAME_LENGTH)
			{
				m_zipFileName.resize(MAXIMUM_FILE_NAME_LENGTH);
			}
			m_zipFileName.append(".zip");
			m_archiveZipFilePath = FormatString("%s/%s", m_archivedLogLocation.c_str(), m_zipFileName.c_str());
			m_downloadZipFilePath = usbPath;
			cout << "m_archiveZipFilePath=" << m_archiveZipFilePath << endl;
			cout << "m_downloadZipFilePath=" << m_downloadZipFilePath << endl;

			//Clean out the map
			std::map <string, DebugFileInfo*>::iterator mapIterator;	// need an iterator to point at an existing record in the map.
			for (mapIterator = mapSyslogs.begin(); mapIterator != mapSyslogs.end(); ++mapIterator)
			{
				delete mapIterator->second;
			}
			mapSyslogs.clear();
			m_consoleLogs.clear();
			m_listOfLiveLogs.clear();
			m_listsOfArchivedLogs.clear();

			ZipAllLogs();

			FlushDebugVolume();
		}

		//We need this here so we don't keep resending tilts when we're not able to log for whatever reason.
		m_zipLogsTime = CUtilities::GetCurrentTime() + ZIP_LOGS_RATE;
	}

	return tiltCode;
}

void CSmartDataLogger::SetSlotMastId(int32_t slotMastId)
{
	m_slotMastId = slotMastId;
}

void CSmartDataLogger::FistTimeSetFromPoller()
{
	LogString(GENERAL, "CSmartDataLogger::FistTimeSetFromPoller called");
	m_zipLogsTime = CUtilities::GetCurrentTime() + ZIP_LOGS_RATE;
	m_logsFlushedTime = CUtilities::GetCurrentTime() + FLUSH_LOGS_RATE;
	LogString(GENERAL, "CSmartDataLogger::FistTimeSetFromPoller: Setting zip logs time to %s", CUtilities::FormatDateTimeString(m_zipLogsTime).c_str());
}

TiltCode CSmartDataLogger::TimedEvents(bool activeSession)
{
	TiltCode tiltCode(tltUnknown);

	if (m_instance)
	{
		if (m_remoteLoggingEnabled)
		{
			if (!activeSession && ZipLogActionType::None == m_zipLogActionType)
			{
				const time_t currentTime(CUtilities::GetCurrentTime());
				if (currentTime >= m_zipLogsTime)
				{
					InitializeProgressSubscriptionData();
					cout << "Time to archive logs" << endl;
					tiltCode = WriteLogsToMediaDisk(ZipLogActionType::AutoArchive, "", AUTO_ARCHIVE_DESCRIPTION);
				}
			}
		}
		else if(!m_manualLoggingInProgress)
		{
			CheckIfManualLoggingNeedsToBeEnabled();
		}

		if (m_combinedZipLogPath.empty() && !m_archiveZipFilePath.empty() && ZipLogActionType::Shutdown != m_zipLogActionType)
		{
			lock_guard<recursive_mutex> lock(_logFileCritSection);
			struct stat media_archive_fd;
			if (0 == stat(m_archiveZipFilePath.c_str(), &media_archive_fd))
			{
				string zipFileName(m_archiveZipFilePath);
				if (m_progressBar < ZIP_LOGS_COMPLETE_PERCENTAGE && ZipLogActionType::Archive == m_zipLogActionType)
				{
					m_progressBar = ZIP_LOGS_COMPLETE_PERCENTAGE;
					UpdateProgressBar(tProgressData(true, m_progressBar, "ARCHIVING_LOGS", ""));
				}
				cout << "***********************************" << m_archiveZipFilePath << " done*******************************\n";
				RefreshLogFiles();				

				//If m_combinedLogsEnabled is false we need to check if zip logs
				//need to be copied to the USB stick
				if (!m_combinedLogsEnabled)
				{
					if (!m_downloadZipFilePath.empty() && !m_zipFileName.empty())
					{
						m_sizeOfZipToBeCopiedToUsb = media_archive_fd.st_size;
						m_downloadZipFilePath.append(FormatString("/%s", m_zipFileName.c_str()));
						string copyZippedLogsToUsb(FormatString("cp %s %s >/dev/null &", m_archiveZipFilePath.c_str(), m_downloadZipFilePath.c_str()));
						system(copyZippedLogsToUsb.c_str());
						cout << copyZippedLogsToUsb << endl;
					}
					else 
					{
						if (ZipLogActionType::Archive == m_zipLogActionType)
						{
							m_progressBar = ONE_HUNDRED_PERCENT;
							UpdateProgressBar(tProgressData(true, m_progressBar, "ARCHIVING_LOGS", ""));
							UpdateProgressBar(tProgressData(true, m_progressBar, m_zipFileName, STATUS_SUCESS));
						}
						m_zipLogActionType = ZipLogActionType::None;
					}
				}
				m_archiveZipFilePath.clear();
			}
			else
			{
				if (m_progressBar < ZIP_LOGS_COMPLETE_PERCENTAGE && ZipLogActionType::Archive == m_zipLogActionType)
				{
					m_progressBar += PROGRESS_BAR_STEP_SIZE;
					UpdateProgressBar(tProgressData(true, m_progressBar, "ARCHIVING_LOGS", ""));
				}
			}
		}

		if (!m_combinedZipLogPath.empty())
		{
			lock_guard<recursive_mutex> lock(_logFileCritSection);
			struct stat media_combined_fd;
			if (0 == stat(m_combinedZipLogPath.c_str(), &media_combined_fd))
			{
				if (m_downloadZipFilePath.empty())
				{
					if (ZipLogActionType::Archive == m_zipLogActionType)
					{
						m_progressBar = ONE_HUNDRED_PERCENT;
						UpdateProgressBar(tProgressData(true, m_progressBar, "ARCHIVING_LOGS", ""));
						UpdateProgressBar(tProgressData(true, m_progressBar, m_zipFileName, STATUS_SUCESS));
					}
					m_zipLogActionType = ZipLogActionType::None;
					m_sizeOfZipToBeCopiedToUsb = 0;
					cout << "***********************************" << m_combinedZipLogPath << " done*******************************\n";
				}
				else
				{
					//Copy combined zipped logs to USB
					if (ZipLogActionType::Archive == m_zipLogActionType)
					{
						m_progressBar = COMBINED_ZIP_COMPLETE_PERCENTAGE;
						UpdateProgressBar(tProgressData(true, m_progressBar, "ARCHIVING_LOGS", ""));
						cout << "Copy combined zipped logs to USB m_progressBar3=" << m_progressBar << endl;
					}
					m_sizeOfZipToBeCopiedToUsb = media_combined_fd.st_size;
					string copyZippedLogsToUsb(FormatString("cp %s %s >/dev/null &", m_combinedZipLogPath.c_str(), m_downloadZipFilePath.c_str()));
					system(copyZippedLogsToUsb.c_str());
					cout << copyZippedLogsToUsb << endl;
				}
				m_combinedZipLogPath.clear();
			}
			else
			{

				if (m_progressBar < COMBINED_ZIP_COMPLETE_PERCENTAGE && ZipLogActionType::Archive == m_zipLogActionType)
				{
					m_progressBar += PROGRESS_BAR_STEP_SIZE;
					UpdateProgressBar(tProgressData(true, m_progressBar, "ARCHIVING_LOGS", ""));
				}
			}
		}

		//Check if copying of logs to USB is finished.
		if (m_sizeOfZipToBeCopiedToUsb > 0)
		{
			lock_guard<recursive_mutex> lock(_logFileCritSection);
			struct stat usb_archive_fd;
			if (0 == stat(m_downloadZipFilePath.c_str(), &usb_archive_fd))
			{
				cout << m_downloadZipFilePath << " " << usb_archive_fd.st_size << " of " << m_sizeOfZipToBeCopiedToUsb << "KB has been copied." << endl;
				if (m_sizeOfZipToBeCopiedToUsb == usb_archive_fd.st_size)
				{					
					if (ZipLogActionType::Archive == m_zipLogActionType)
					{
						m_progressBar = ONE_HUNDRED_PERCENT;
						UpdateProgressBar(tProgressData(true, m_progressBar, "ARCHIVING_LOGS", ""));
						UpdateProgressBar(tProgressData(true, m_progressBar, m_zipFileName, STATUS_SUCESS));
					}					
					m_zipLogActionType = ZipLogActionType::None;
					m_sizeOfZipToBeCopiedToUsb = 0;
					cout << "***********************************" << m_downloadZipFilePath << " done*******************************\n";
					m_downloadZipFilePath.clear();
				}
				else if (m_progressBar < ONE_HUNDRED_PERCENT && ZipLogActionType::Archive== m_zipLogActionType)
				{
					m_progressBar += PROGRESS_BAR_STEP_SIZE;
					UpdateProgressBar(tProgressData(true, m_progressBar, "ARCHIVING_LOGS", ""));
				}
			}
			else
			{
				if (m_progressBar < ONE_HUNDRED_PERCENT && ZipLogActionType::Archive == m_zipLogActionType)
				{
					m_progressBar += PROGRESS_BAR_STEP_SIZE;
					UpdateProgressBar(tProgressData(true, m_progressBar, "ARCHIVING_LOGS", ""));
				}
			}
		}
	}
	return tiltCode;
}

void CSmartDataLogger::CheckIfManualLoggingNeedsToBeEnabled()
{
	if (!m_manualLoggingInProgress)
	{
		string debugKeyDirectory;
		if (FindDebugKey(debugKeyDirectory))
		{
			m_manualLoggingInProgress = true;
		}
	}
}

void CSmartDataLogger::FlushToNVRAM()
{
	if (!m_instance)
		return;

	mapSyslogs[ERROR_LOG]->BufferHandler->Flush(true);
}

LPCTSTR CSmartDataLogger::GetBoolString(bool value)
{
	return value ? LPCTSTR_TRUE : LPCTSTR_FALSE;
}

void CSmartDataLogger::LogString(const std::string &Identifier, LPCSTR strToLog, bool useTimeStamp)
{
	if (m_instance &&
		(m_combinedZipLogPath.empty() && m_archiveZipFilePath.empty() && m_downloadZipFilePath.empty() && ZipLogActionType::None == m_zipLogActionType) &&
		(m_remoteLoggingEnabled || m_manualLoggingInProgress))
	{
		try
		{
			if (!Identifier.empty() && strToLog != nullptr && *strToLog != 0)
			{
				char retChar = '\n';
				lock_guard<recursive_mutex> lock(_logFileCritSection);
				{
					if (mapSyslogs.size() > 0)
					{
						DebugFileInfo * dbgfi = NULL;
						std::map <string, DebugFileInfo*>::const_iterator mapIterator;	// need an iterator to point at an existing record in the map.

																						//Are we logging this time
						mapIterator = mapSyslogs.find(Identifier);
						if (mapIterator != mapSyslogs.end())
						{
							dbgfi = mapSyslogs[Identifier];
						}

						BOOL result = TRUE;
						char ticks[MAX_DWORD_CHARS];
						if (useTimeStamp)
							sprintf(ticks, "%lu ", (unsigned long)CUtilities::GetTickCount());

						if (dbgfi != NULL)
						{
							//Write out text stream
							if (useTimeStamp)
								result = (*dbgfi).BufferHandler->Write((BYTE *)ticks, strlen(ticks));
							result = result && (*dbgfi).BufferHandler->Write(strToLog, strlen(strToLog));
							result = result && (*dbgfi).BufferHandler->Write(&retChar, 1);

							//if the write failed, remove the entry
							//just leave a dead handle for the ERROR log
							if (!result && Identifier != ERROR_LOG)
							{
								delete(dbgfi);
								mapSyslogs.erase(Identifier);
							}
							else if (dbgfi->BufferHandler->GetBufferFileSize() > dbgfi->iSizeInBytes)
							{
								// Create a new file
								CreateDebugInfoFile(dbgfi->LoggingID, dbgfi->iSizeInBytes, dbgfi->numberOfLogsToKeep);
							}
							else
							{
								CheckIfLogsNeedsToBeFlushed();
							}
						}
					}
					if (m_consoleLogs.size() > 0)
					{
						vector<string>::iterator findLogIter = find(m_consoleLogs.begin(), m_consoleLogs.end(), Identifier);
						if (findLogIter != m_consoleLogs.end())
						{
							cout << strToLog << endl;
						}
					}

					auto liveLog(m_listOfLiveLogs.find(Identifier));
					if (liveLog != m_listOfLiveLogs.end())
					{
						if (!liveLog->second.m_active)
						{
							liveLog->second.m_active = true;
							WriteHtmlPages();
						}
					}
				}
			}
		}
		catch (...)
		{
			//we do not want to throw to calling thread
		}
	}
}

//LogData writes binary to the log file.
//The format is:
//1 byte  - Code (or of COMM_DATA_TYPEs)
//2 bytes - Length of Data
//n bytes - Data
void CSmartDataLogger::LogData(const std::string &Identifier, BYTE code, const void *data, WORD dataLength)
{
	if (m_instance &&
		(m_combinedZipLogPath.empty() && m_archiveZipFilePath.empty() && m_downloadZipFilePath.empty() && ZipLogActionType::None == m_zipLogActionType) &&
		(m_remoteLoggingEnabled || m_manualLoggingInProgress))
	{
		USES_CONVERSION;
		try
		{
			if (!Identifier.empty() && data != nullptr && dataLength > 0)
			{
				lock_guard<recursive_mutex> lock(_logFileCritSection);
				{
					if (mapSyslogs.size() > 0)
					{
						DebugFileInfo * dbgfi = NULL;

						std::map <string, DebugFileInfo*>::const_iterator mapIterator;	// need an iterator to point at an existing record in the map.
						mapIterator = mapSyslogs.find(Identifier);
						if (mapIterator != mapSyslogs.end())
						{
							dbgfi = mapSyslogs[Identifier];
						}

						//if we need to write somewhere, then do the work
						if (dbgfi != NULL)
						{
							//Add current ticks into data
							code |= DATA_TIMESTAMP;

							//Write out the data code
							BOOL result = dbgfi->BufferHandler->Write((BYTE *)&code, sizeof(BYTE));

							//Write tickcount
							DWORD tickCount = CUtilities::GetTickCount();
							result = result && dbgfi->BufferHandler->Write(&tickCount, sizeof(tickCount));

							//Write out the data length
							result = result && dbgfi->BufferHandler->Write((BYTE *)&dataLength, sizeof(WORD));

							//and write out the data
							result = result && dbgfi->BufferHandler->Write(reinterpret_cast<const BYTE *>(data), dataLength);

							if (!result)
							{
								delete(dbgfi);
								mapSyslogs.erase(Identifier);
							}
							else
							{
								// We need to check that the file hasn't grown too big
								if (dbgfi->BufferHandler->GetBufferFileSize() > dbgfi->iSizeInBytes)
								{
									// Create a new file
									CreateDebugInfoFile(dbgfi->LoggingID, dbgfi->iSizeInBytes, dbgfi->numberOfLogsToKeep);
								}
								else
								{
									CheckIfLogsNeedsToBeFlushed();
								}
							}
						}
					}

					auto liveLog(m_listOfLiveLogs.find(Identifier));
					if (liveLog != m_listOfLiveLogs.end())
					{
						if (!liveLog->second.m_active)
						{
							liveLog->second.m_active = true;
							WriteHtmlPages();
						}
					}
				}
			}
		}
		catch (...)
		{
			//we do not want to throw to calling thread
		}
	}
}

//Parses data from a line sent in from the debug file, and adds it to the map.
//a syslog passed in from the debug file should look like
//debugID@SizeInMB@numberOfLogsToKeep
void CSmartDataLogger::HandleSYSLOG(const char* syslogstring)
{
	lock_guard<recursive_mutex> lock(_logFileCritSection);
	if (!m_instance)
		return;

	istringstream cmdstringstream(syslogstring);	//create a new input stream from passed in line of debug file.

													//Setup Actual Filename to open on found drive
	char file[MAX_DEBUG_IDENTIFIER_SIZE];
	cmdstringstream.getline(file, MAX_DEBUG_IDENTIFIER_SIZE, SYS_IDENTIFIER_PARSE_CHAR);

	//setup size of file to grow to
	char sizeInMB[MAX_DEBUG_MB_HOLDER_SIZE];
	cmdstringstream.getline(sizeInMB, MAX_DEBUG_MB_HOLDER_SIZE, SYS_IDENTIFIER_PARSE_CHAR);

	//setup if the file will roll to a new file or not
	char numberOfLogsToKeep[MAX_DEBUG_NUMBER_OF_LOGS_TO_KEEP_HOLDER_SIZE];
	cmdstringstream.getline(numberOfLogsToKeep, MAX_DEBUG_NUMBER_OF_LOGS_TO_KEEP_HOLDER_SIZE, SYS_IDENTIFIER_LINE_PARSE_CHAR);

	CreateDebugInfoFile(file, atoi(sizeInMB) * BYTES_PER_MEG, atoi(numberOfLogsToKeep));
}	//CSmartDataLogger::HandleSYSLOG


void CSmartDataLogger::CreateDebugInfoFile(const std::string &loggingid, int sizeInBytes, int numberOfLogsToKeep)
{
	if (!m_instance)
		return;

	std::map <string, DebugFileInfo*>::const_iterator mapIterator;	// need an iterator to point at an existing record in the map.

	lock_guard<recursive_mutex> lock(_logFileCritSection);
	{
		mapIterator = mapSyslogs.find(loggingid);

		DebugFileInfo *NewDebugInfoFile;

		if (mapIterator != mapSyslogs.end())
		{
			NewDebugInfoFile = mapIterator->second;
		}
		else
		{
			NewDebugInfoFile = new DebugFileInfo();	//create record for map
		}

		string logToArchive, logFileToDelete;

		//Set all the new options
		CreateActualFileName(NewDebugInfoFile->ActualFileName, logToArchive, loggingid, numberOfLogsToKeep);	//create new filename

		NewDebugInfoFile->iSizeInBytes = sizeInBytes;
		NewDebugInfoFile->numberOfLogsToKeep = numberOfLogsToKeep;
		NewDebugInfoFile->LoggingID = loggingid;

		//if it's a new one, create a new BufferHandler
		if (mapIterator == mapSyslogs.end())
		{
			NewDebugInfoFile->BufferHandler = new CBufferHandler(NewDebugInfoFile->ActualFileName);
			mapSyslogs[loggingid] = NewDebugInfoFile;

			if (ERROR_LOG == loggingid)
			{
				//We need to set a default Log for ERRORs.
				//This does not get written to a file until set by a debug.txt
				USES_CONVERSION;

				//load in any previous fatal error from NVRAM
				NewDebugInfoFile->BufferHandler->GetFromNVRAM();

				// Initialize the NVRAM buffer
				NewDebugInfoFile->BufferHandler->InitializeNVRAM((BYTE *)T2A(INITIAL_NVRAM_BUFFER_VALUE), _tcslen(INITIAL_NVRAM_BUFFER_VALUE));

				// Put a start-up value in the log file
				LogString(ERROR_LOG, INITIAL_ERROR_LOG);
			}
		}
		else //otherwise just give it a file handle to change or set the file
		{
			NewDebugInfoFile->BufferHandler->SetFileHandle(NewDebugInfoFile->ActualFileName);

			// Flush volume associated with debug file
			FlushDebugVolume();
		}

		if (!logToArchive.empty())
		{
			auto listOfLogs(m_listsOfArchivedLogs.find(loggingid));
			if (listOfLogs == m_listsOfArchivedLogs.end())
			{
				list<string> list;
				list.push_front(logToArchive);
				m_listsOfArchivedLogs[loggingid] = list;
			}
			else
			{
				listOfLogs->second.push_front(logToArchive);
				if (listOfLogs->second.size() > numberOfLogsToKeep)
				{
					logFileToDelete = listOfLogs->second.back();
					listOfLogs->second.pop_back();
				}
			}

			if (!logFileToDelete.empty())
			{
				//Delete the oldest log filer
				logFileToDelete.insert(0, "/");
				logFileToDelete.insert(0, m_debugPath);
				string deleteOldLog(FormatString("rm %s", logFileToDelete.c_str()));
				CUtilities::GetSystemResultString(deleteOldLog);
			}
		}

		// Flush volume associated with debug file
		FlushDebugVolume();

		WriteHtmlPages();
	}
	AddDebugInfoFileHeader(loggingid);
}

// Add header information to logging files, depending on identifier.
void CSmartDataLogger::AddDebugInfoFileHeader(const std::string &loggingid)
{
	if (!m_instance)
		return;

	if (0 == loggingid.compare(GENERAL))
	{
#ifdef LOG_REGISTER_THREADS
		CWatchDog::Instance().LogRegisteredThreads(loggingid);
#endif
	}
}


//Parses data from a line sent in from the debug file, and adds it to a list of 
// system commands for use throughout this debugging code.
void CSmartDataLogger::HandleSYSCMD(const char* syslogstring)
{
	lock_guard<recursive_mutex> lock(_logFileCritSection);
	if (!m_instance)
		return;

	string consoleLog = syslogstring;

	vector<string>::iterator findLogIter = find(m_consoleLogs.begin(), m_consoleLogs.end(), consoleLog);
	if (findLogIter == m_consoleLogs.end())
	{
		m_consoleLogs.push_back(consoleLog);
	}
}


// Caller is responsible for maximum filename length.  2009-11-10 Alan Sheldon
void CSmartDataLogger::CreateActualFileName(std::string &currentLogFilePath, std::string& logToArchive, const std::string &filename, int numberOfLogsToKeep)
{
	if (!m_instance)
		return;

	currentLogFilePath = m_debugPath;
	currentLogFilePath.append("/");
	currentLogFilePath.append(filename);
	currentLogFilePath.append("-");
	currentLogFilePath.append(CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), TIME_FORMAT_FILE_NAME));
	currentLogFilePath.append((SASDATA == filename || POLLERDATA_LOG == filename) ? ".sdf" : ".txt");
	string nextLogToArchive(filename);
	if (numberOfLogsToKeep > 0)
	{
		nextLogToArchive.append("-");
		nextLogToArchive.append(CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), TIME_FORMAT_FILE_NAME));
		nextLogToArchive.append((SASDATA == filename || POLLERDATA_LOG == filename) ? ".sdf" : ".txt");
		auto liveLog(m_listOfLiveLogs.find(filename));
		if (liveLog != m_listOfLiveLogs.end())
		{
			logToArchive = liveLog->second.m_fileName;
		}
	}

	tLogData logData;
	logData.m_fileName = nextLogToArchive;
	m_listOfLiveLogs[filename] = logData;
}

void CSmartDataLogger::SetRemoteLoggingEnabledFlag(bool value)
{
	bool oldRemoteDebuggingEnabled(m_remoteLoggingEnabled);
	if (oldRemoteDebuggingEnabled != value)
	{
		lock_guard<recursive_mutex> lock(_logFileCritSection);
		m_remoteLoggingEnabled = value;
		if (m_remoteLoggingEnabled)
		{
			m_manualLoggingInProgress = false;
		}
		WriteHtmlPages();
	}
}

void CSmartDataLogger::SetCombineZipLogs(bool value)
{
	m_combinedLogsEnabled = value;
}

void CSmartDataLogger::FlushAllBuffers()
{
	std::map <string , DebugFileInfo*>::iterator mapIterator;	// need an iterator to point at an existing record in the map.
	std::map <string , DebugFileInfo*>::iterator tempIterator;	// need an iterator to point at an existing record in the map.
	bool result;

	lock_guard<recursive_mutex> lock(_logFileCritSection);
	mapIterator = mapSyslogs.begin();
	while (mapIterator != mapSyslogs.end() )
	{
		result = mapIterator->second->BufferHandler->Flush(FALSE);
		tempIterator = mapIterator;
		++mapIterator;
		if (!result && tempIterator->first != ERROR_LOG)
		{
			delete(tempIterator->second);
			mapSyslogs.erase(tempIterator->first);
		}
	}

	FlushDebugVolume();
}

void CSmartDataLogger::FlushDebugVolume()
{
	if (!m_instance)
		return;

	CUtilities::FlushVolume();
}

void CSmartDataLogger::CheckIfDiskSpaceIsAvailableForArchive()
{
	try
	{
		m_mediaDiskExists = FileExists(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_MEDIADISK));
		if (m_mediaDiskExists)
		{
			string sizeOfDiskSpace, usedDiskSpace, availableDiskSpace;
			CUtilities::GetPathMemoryUsageInformation(string(CDirectoryManager::CONTENT_SD_PATH), sizeOfDiskSpace, usedDiskSpace, availableDiskSpace);
			DWORD multiplier(1);
			if (string::npos != availableDiskSpace.find("G"))
			{
				multiplier = 1000;//1000MB in 1G
			}
			availableDiskSpace.pop_back();//remove M or G
			DWORD availableDiskSpaceMB(DWORD(std::stof(availableDiskSpace) * multiplier));
			if (availableDiskSpaceMB > DISK_SPACE_MINIMUM_REQUIRED_FOR_ARCHIVE_IN_MB)
			{
				m_diskSpaceAvailable = true;
			}
			else
			{
				m_diskSpaceAvailable = false;
			}
		}
		else
		{
			m_diskSpaceAvailable = false;
		}
	}
	catch (const std::invalid_argument& e)
	{
		m_mediaDiskExists = false;
		m_diskSpaceAvailable = false;
		cout << "invalid_argument\n";
	}
	catch (const std::out_of_range& e)
	{
		m_mediaDiskExists = false;
		m_diskSpaceAvailable = false;
		cout << "out_of_range\n";
	}
	catch (const std::exception& e)
	{
		m_mediaDiskExists = false;
		m_diskSpaceAvailable = false;
		cout << string(e.what()) << endl;
	}
}

void CSmartDataLogger::CheckIfLogsNeedsToBeFlushed()
{
	const time_t currentTime(CUtilities::GetCurrentTime());
	if (currentTime >= m_logsFlushedTime)
	{
		//Set the next time to flush again
		m_logsFlushedTime = currentTime + FLUSH_LOGS_RATE;
		FlushAllBuffers();
	}
}

void CSmartDataLogger::CreateLogFiles()	
{
	RefreshArchivedFolder();

	ifstream debugStream(m_debugPath + "/" + DEBUG_FILE_NAME, ifstream::in);
	if (debugStream.is_open())
	{
		while (true)
		{
			string temp;
			debugStream >> temp;
			if (debugStream.eof())
				break;
			if (temp.find(SYSLOG_IDENTIFIER_NAME) == 0)
			{
				temp = temp.substr(strlen(SYSLOG_IDENTIFIER_NAME) + 1);
				HandleSYSLOG(temp.c_str());
			}
			else if (temp.find(SYSCMD_IDENTIFIER_NAME) == 0)
			{
				temp = temp.substr(strlen(SYSCMD_IDENTIFIER_NAME) + 1);
				string::size_type pos = temp.find('@');
				if (pos != string::npos)
				{
					temp = temp.substr(0, pos);
				}
				HandleSYSCMD(temp.c_str());
			}
		}
		debugStream.close();
	}
}

void CSmartDataLogger::WriteHtmlPages()
{
	string liveLogsHtmlPath(m_debugPath + "/logs.html");
	ofstream liveLogsStream;
	liveLogsStream.open(liveLogsHtmlPath, ios_base::out | ios_base::binary | ios_base::trunc);
	if (liveLogsStream)
	{
		if (m_remoteLoggingEnabled)
		{
			string slotmastIdString(FormatString("SlotMastID(%s)", to_string(m_slotMastId).c_str()));
			CWebDiagnostics::WriteHtmlHeader(liveLogsStream, 1000, slotmastIdString.c_str(), CWebDiagnostics::UseTableStyle | CWebDiagnostics::UseRgbColorStyle);
			string htmlContent;
			htmlContent.append(FormatString("<h1>Remote Debugging %s</h1>", slotmastIdString.c_str()));
			htmlContent.append("<div><ul>");
			for (auto liveLog : m_listOfLiveLogs)
			{
				if (liveLog.second.m_active)
				{
					string logLi;
					logLi = FormatString("<li>%s - <a href=\"/%s\">[LIVE]</a>", liveLog.first.c_str(), liveLog.second.m_fileName.c_str());
					if (m_listsOfArchivedLogs.find(liveLog.first) != m_listsOfArchivedLogs.end())
					{
						logLi.append(FormatString("<a href=\"/archived%s.html\">[Archives]</a>", liveLog.first.c_str()));
					}
					logLi.append("</li>");
					htmlContent.append(logLi);
				}
			}
			htmlContent.append("</div>");
			if (!m_listsOfZipFiles.empty())
			{
				htmlContent.append("<p><a href=\"/olderarchived.html\">[Older Archived Logs]</a>");
			}
			htmlContent.append("</body>");
			htmlContent.append("</html>");
			liveLogsStream << htmlContent;

			for (auto listOfLogs : m_listsOfArchivedLogs)
			{
				string logName(listOfLogs.first);
				string archivedLogHtmlPath(FormatString("%s/archived%s.html", m_debugPath.c_str(), logName.c_str()));
				ofstream archivedLogsStream;
				archivedLogsStream.open(archivedLogHtmlPath, ios_base::out | ios_base::binary | ios_base::trunc);
				if (archivedLogsStream)
				{
					CWebDiagnostics::WriteHtmlHeader(archivedLogsStream, 1000, FormatString("%s", slotmastIdString.c_str()).c_str(), CWebDiagnostics::UseTableStyle | CWebDiagnostics::UseRgbColorStyle);
					string htmlContent;
					htmlContent.append(FormatString("<h1>Recent Archived %s Logs %s</h1>", logName.c_str(), slotmastIdString.c_str()));
					htmlContent.append("<div><ul>");
					if (listOfLogs.second.empty())
					{
						htmlContent.append("<p>There is nothing archived.");
					}
					else
					{
						for (auto logToPrint : listOfLogs.second)
						{
							string logLi(FormatString("<li><a href=\"/%s\">%s</a></li>", logToPrint.c_str(), logToPrint.c_str()));
							htmlContent.append(logLi);
						}
					}
					htmlContent.append("</div>");
					auto liveLog(m_listOfLiveLogs.find(logName));
					if (liveLog != m_listOfLiveLogs.end())
					{
						if (liveLog->second.m_active)
						{
							htmlContent.append(FormatString("<p><a href=\"/%s\">[%s LIVE]</a>", liveLog->second.m_fileName.c_str(), logName.c_str()));
						}
					}
					htmlContent.append("<p><a href=\"/logs.html\">[Live nCompass Logs]</a>");
					if (!m_listsOfZipFiles.empty())
					{
						htmlContent.append("<p><a href=\"/olderarchived.html\">[Older Archived Logs]</a>");
					}
					htmlContent.append("</body>");
					htmlContent.append("</html>");
					archivedLogsStream << htmlContent;
				}
				else
				{
					cout << "Opening " << archivedLogHtmlPath << " failed\n";
				}
			}

			string olderArchivedLogHtmlPath(FormatString("%s/olderarchived.html", m_debugPath.c_str()));
			ofstream olderLogsStream;
			olderLogsStream.open(olderArchivedLogHtmlPath, ios_base::out | ios_base::binary | ios_base::trunc);
			if (olderLogsStream)
			{
				CWebDiagnostics::WriteHtmlHeader(olderLogsStream, 1000, slotmastIdString.c_str(), CWebDiagnostics::UseTableStyle | CWebDiagnostics::UseRgbColorStyle);
				string htmlContent;
				htmlContent.append(FormatString("<h1>Older Archived Logs %s</h1>", slotmastIdString.c_str()));

				if (CUtilities::FileExists(FormatString("%s%s", m_debugPath.c_str(), m_combinedZipFileName.c_str())))
				{
					string combinedZipFileName(m_combinedZipFileName);
					combinedZipFileName.erase(0, 1);//removes /
					htmlContent.append(FormatString("<p><a href=\"/%s\">%s</a>", combinedZipFileName.c_str(), combinedZipFileName.c_str()));
				}

				htmlContent.append("<div><ul>");
				for (string zipFile : m_listsOfZipFiles)
				{
					string logLi(FormatString("<li><a href=\"/archivedlogs/%s\">%s</a></li>", zipFile.c_str(), zipFile.c_str()));
					htmlContent.append(logLi);
				}
				htmlContent.append("</div>");
				htmlContent.append("<p><a href=\"/logs.html\">[Live nCompass Logs]</a>");
				htmlContent.append("</body>");
				htmlContent.append("</html>");
				olderLogsStream << htmlContent;
			}
			else
			{
				cout << "Opening " << olderArchivedLogHtmlPath << " failed\n";
			}
		}
		else
		{
			liveLogsStream << "Remote logging is not enabled.";
		}
	}
	else
	{
		cout << "Opening " << liveLogsHtmlPath << " failed\n";
	}
}