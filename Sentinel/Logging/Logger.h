#pragma once

#include "BufferHandler.h"
#include "LoggerTypes.h"
#include "../Utilities.h"
#include "DataSubscriptionProvider.h"
#include "EnumMap.h"
#include "TiltsDef.h"

#define MAX_DEBUG_IDENTIFIER_SIZE 100
#define MAX_DEBUG_MB_HOLDER_SIZE 20
#define MAX_DEBUG_NUMBER_OF_LOGS_TO_KEEP_HOLDER_SIZE 20
#define MAX_DEBUG_LINE_SIZE 70
#define MAX_DWORD_CHARS 13

const std::string ArchiveLogsProgress("ArchiveLogsProgress");

enum class ZipLogActionType
{
	AutoArchive,
	Archive,
	ArchiveFromFLM,
	None,
	Shutdown
};
extern EnumMap<ZipLogActionType> ZipLogActionTypeEnumMap;

class DebugFileInfo
{
public:
	CBufferHandler *BufferHandler;
	std::string ActualFileName;
	std::string LoggingID;
	int iSizeInBytes;
	int numberOfLogsToKeep;

	DebugFileInfo()
		: BufferHandler(nullptr)
		, iSizeInBytes(BYTES_PER_MEG)
		, numberOfLogsToKeep(1)
	{
	}

	~DebugFileInfo()
	{
		delete BufferHandler;
	}
};

struct tLogData
{
	std::string m_fileName;
	bool m_active;
	tLogData() : m_active(false) {}
};

struct tProgressData
{
	bool m_downloadInProgress;
	int m_percentage;
	string m_downloadMessage;
	string m_downloadStatus;
	tProgressData(bool downloadInProgress, int percentage, const std::string& downloadMessage, const std::string& downloadStatus) :
		m_downloadInProgress(downloadInProgress), 
		m_percentage(percentage), 
		m_downloadMessage(downloadMessage),
		m_downloadStatus(downloadStatus) {}
};

// Declare friend class(es) for unit test access.
class LoggerTests;

class CSmartDataLogger : public CDataSubscriptionProvider
{
public:
	///	<summary>
	/// Gets a reference to the global instance of CSmartDataLogger.
	///	</summary>
	///	<returns>A reference to the global CSmartDataLogger instance.</returns>	
	/// <remarks>
	/// The caller is responsible for successfully calling S5_DLL_Init().
	/// </remarks>
	static CSmartDataLogger &Instance();

	/// <summary>
	/// Cleans up instance of CSmartDataLogger object
	/// </summary>
	static void CleanupInstance();

	/// <summary>
	/// Checks to see if instance is created or not
	/// </summary>
	/// <returns>Returns true if instance of the  CSmartDataLogger is created, false otherwise.</returns>
	static bool IsActive();

	/// <summary>
	/// Initialize the singleton CSmartDataLogger object
	/// </summary>
	void Initialize();

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

	/// <summary>
	/// "Format" a bool value into a string intended for debug log files.
	/// </summary>
	/// <param name="value">IN - The bool value.</param>
	/// <returns>An LPCTSTR to the "formatted" string value.</returns>
	static LPCTSTR GetBoolString(bool value);

	void LogString(const std::string &Identifier, LPCSTR strToLog, bool useTimeStamp = true);

	/// <summary>
	/// Logs the data.
	/// </summary>
	/// <param name="Identifier">IN - The logging category identifier.</param>
	/// <param name="code">IN - The code that identifies the data source and/or data type.</param>
	/// <param name="data">IN - The data.</param>
	/// <param name="dataLength">IN - Length of the data.</param>
	void LogData(const std::string &Identifier, BYTE code, const void *data, WORD dataLength);

	void FlushToNVRAM();
	void FlushAllBuffers();

	/// <summary>
	/// Archive log files
	/// </summary>
	/// <param name="description">IN - Archive description.</param>
	/// <returns>Tilt Code, tltUnknown if successfull</returns>
	TiltCode ArchiveLogs(const std::string& description);

	/// <summary>
	/// Writes the logs to media disk
	/// </summary>
	/// <param name="zipLogActionType">IN - Zip log action type.</param>
	/// <param name="usbPath">IN - usb path.</param>
	/// <param name="description">IN - Archive description.</param>
	/// <returns>Tilt Code, tltUnknown if successfull</returns>
	TiltCode WriteLogsToMediaDisk(ZipLogActionType zipLogActionType, const std::string& usbPath = "", std::string description = "");

	/// <summary>
	/// Sets SlotMast Id
	/// </summary>
	/// <param name="slotMastId">IN - slotMast Id.</param>
	void SetSlotMastId(int32_t slotMastId);

	/// <summary>
	/// Used to set the file clean up time when we recieve our first time from the poller.
	/// </summary>
	void FistTimeSetFromPoller();

	/// <summary>
	/// Performs timed events.
	/// </summary>
	/// <param name="activeSession">IN - flag which indicates whether there is an active session.</param>
	/// <returns>Returns approriate tilt code, when necessary.</returns>
	TiltCode TimedEvents(bool activeSession);

	/// <summary>
	/// Checks to see if manual logging needs to be enabled
	/// </summary>
	void CheckIfManualLoggingNeedsToBeEnabled();

	/// <summary>
	/// Checks if logs needs to be archived
	/// </summary>
	/// <param name="value">IN - true to turn on remote logging, otherwise it is disabled..</param>
	void SetRemoteLoggingEnabledFlag(bool value);

	/// <summary>
	/// Checks if remote logging is enabled.
	/// </summary>
	/// <returns>Returns true if remote logging is enabled, false otherwise.</returns>
	inline bool RemoteLoggingEnabled() { return m_remoteLoggingEnabled; }

	/// <summary>
	/// Sets combine zip logs enabled flag
	/// </summary>
	/// <param name="value">IN - true if zip logs should be combined into a single file, false otherwise.</param>
	void SetCombineZipLogs(bool value);

public:
	 virtual ~CSmartDataLogger(void);
	static const std::string DEBUG_FILE_NAME;
	bool NeedToRequestForSubscriptionUpdate();
protected:
	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CSmartDataLogger"/> class.
	/// </summary>
	CSmartDataLogger();

	virtual void CheckIfDiskSpaceIsAvailableForArchive();
	virtual bool FileExists(const std::string& filePath);
	virtual bool FindDebugKey(std::string& debugKeyDirectory);
	virtual void UpdateProgressBar(const tProgressData& progressData);
private:	
	static const time_t FLUSH_LOGS_RATE;
	static const time_t ZIP_LOGS_RATE;
	static const time_t MAX_TIME_OF_ZIPPED_LOGS_TO_KEEP;

	static LPCTSTR const LPCTSTR_FALSE;
	static LPCTSTR const LPCTSTR_TRUE;

	static const LPCTSTR TIME_FORMAT_FILE_NAME;
	static const LPCTSTR PATH_SENT_DEBUG_FILE_ID;
	static const int  WAIT_TIME_BETWEEN_CHECKS = 10;
	static const LPCSTR SYSLOG_IDENTIFIER_NAME;
	static const LPCSTR SYSCMD_IDENTIFIER_NAME;
	static const char SYS_IDENTIFIER_PARSE_CHAR = '@';
	static const char SYS_IDENTIFIER_LINE_PARSE_CHAR = '\n';
	static const int NUMBER_OF_LOOPS_TO_WAIT = 6;
	static const DWORD SLEEP_TIME;
	static const std::string m_debugPath;
	static const std::string m_debugFilePath;

	// The singleton instance
	static CSmartDataLogger *m_instance;
	static std::recursive_mutex m_instanceCS;

	std::map <std::string , DebugFileInfo*> mapSyslogs;
	std::vector<std::string> m_consoleLogs;
	ZipLogActionType m_zipLogActionType;
    bool m_remoteLoggingEnabled;
	bool m_combinedLogsEnabled;
	bool m_mediaDiskExists;
	bool m_diskSpaceAvailable;
	bool m_manualLoggingInProgress;
	bool m_requestForSubscriptionUpdate;
	long m_sizeOfZipToBeCopiedToUsb;
	int m_progressBar;
	std::string m_zipFileName;
	std::string m_combinedZipFileName;
	std::string m_combinedZipLogPath;
	std::string m_archiveZipFilePath;
	std::string m_downloadZipFilePath;

	void CheckIfLogsNeedsToBeFlushed();
	void CreateLogFiles();
	void FlushDebugVolume();
	void HandleSYSLOG(const char* syslogstring);
	void HandleSYSCMD(const char* syslogstring);
	void CreateActualFileName(std::string &filenametoaddto, std::string& logToArchive, const std::string &filename, int numberOfLogsToKeep);
	void CreateDebugInfoFile(const std::string &loggingid, int sizeInBytes, int numberOfLogsToKeep);
	void AddDebugInfoFileHeader(const std::string &loggingid);
	void WriteHtmlPages();
	void RefreshArchivedFolder();
	void RefreshLogFiles();
	void ZipAllLogs();
	void InitializeProgressSubscriptionData();

	static std::recursive_mutex _logFileCritSection;
	time_t m_zipLogsTime;
	time_t m_logsFlushedTime;
	std::map <std::string, tLogData> m_listOfLiveLogs;
	std::map <std::string, std::list<std::string>> m_listsOfArchivedLogs;
	std::queue<tProgressData> m_vProgressData;
	std::vector <std::string> m_listsOfZipFiles;
	std::string m_archivedLogLocation;
	int32_t m_slotMastId;

	FRIEND_TEST(LoggerTests, DefaultRemoteDebuggingFlagTest);
	FRIEND_TEST(LoggerTests, SetRemoteLoggingEnabledFlagTest);
	FRIEND_TEST(LoggerTests, DefaultGetSubscriptionDataResponseTest);
	FRIEND_TEST(LoggerTests, ArchiveLogsTest);
	FRIEND_TEST(LoggerTests, WriteLogsToMediaDisk_SuccessTest);
	FRIEND_TEST(LoggerTests, WriteLogsToMediaDisk_ArchivingLogsDeniedAlreadyInProgress);
	FRIEND_TEST(LoggerTests, WriteLogsToMediaDisk_ArchivingLogsDeniedRemoteDebuggingDisabled);
	FRIEND_TEST(LoggerTests, WriteLogsToMediaDisk_ArchivingLogsFailedNoDisk);
	FRIEND_TEST(LoggerTests, WriteLogsToMediaDisk_ArchivingLogsFailedNoDiskSpace);
};
