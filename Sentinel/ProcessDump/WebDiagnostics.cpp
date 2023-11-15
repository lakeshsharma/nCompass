#include "stdafx.h"
#include "WebDiagnostics.h"
#include "Config/ConfigExtFile.h"
#include "Diagnostics.h"
#include "DirectoryManager.h"
#include "Util/Path.h"
#include "resource.h"
#include "State/SentinelState.h"
#include "Platform.h"

using namespace std;

const int DUMP_MEMORY_CHECK_IN_SECONDS(600);//10 minutes

LPCTSTR CWebDiagnostics::CardReaderInfoFilename = _T("CardReader.html");
LPCTSTR CWebDiagnostics::CertificatesInfoFilename = _T("CertificatesInfo.html");
LPCTSTR CWebDiagnostics::OptionsAndSettingsFilename = _T("OptionsAndSettings.html");
LPCTSTR CWebDiagnostics::ProgressiveInfoFilename = _T("ProgressiveInfo.html");
LPCTSTR CWebDiagnostics::OneLinkContentFilename = _T("OneLinkContent.html");
LPCTSTR CWebDiagnostics::PlayerInfoFilename = _T("PlayerInfo.html");
LPCTSTR CWebDiagnostics::JackpotInfoFilename = _T("JackpotInfo.html");

static const LPCTSTR STATUS_DUMP_FILE_NAME = TEXT("StatusDump.html");
static const LPCTSTR MEMORY_DUMP_FILE_NAME = TEXT("MemoryDump.html");
static const LPCTSTR ERROR_DUMP_FILE_NAME = TEXT("ErrorDump.html");
static const LPCTSTR CONFIG_EXTENSIONS_FILE_NAME = TEXT("ConfigExtensions.html");
static const LPCTSTR QUEUING_CONFIG_FILE_NAME = TEXT("QueuingConfig.html");

LPCTSTR TOKEN = _T("token");
const string CUSTOM_TOKEN = _T("token=");

const DWORD BLOCK_SIZE = 1024;
const DWORD TAG_BYTES = 4;
const DWORD MINIMUM_TOKEN_SIZE = 10;


LPCTSTR const CWebDiagnostics::FMT_HTML_HEADER(_T("<html><META Http-equiv=\"refresh\" Content=\"%d\">"));
LPCTSTR const CWebDiagnostics::HTML_TRAILER(_T("</html>"));
LPCTSTR const CWebDiagnostics::BR_TAG(_T("<br>"));

LPCTSTR const CWebDiagnostics::LPCTSTR_FALSE(_T("false"));
LPCTSTR const CWebDiagnostics::LPCTSTR_TRUE(_T("true"));

LPCTSTR const CWebDiagnostics::LPCTSTR_DISABLED(_T("Disabled"));
LPCTSTR const CWebDiagnostics::LPCTSTR_ENABLED(_T("Enabled"));

LPCTSTR const CWebDiagnostics::LPCTSTR_NA(_T("N/A"));

LPCSTR const CWebDiagnostics::LPCSTR_FALSE("false");
LPCSTR const CWebDiagnostics::LPCSTR_TRUE("true");

LPCSTR const CWebDiagnostics::LPCSTR_DISABLED("Disabled");
LPCSTR const CWebDiagnostics::LPCSTR_ENABLED("Enabled");

LPCSTR const CWebDiagnostics::LPCSTR_NA("N/A");

time_t CWebDiagnostics::m_lastDumpMemorySystemCall(CUtilities::GetCurrentTime() - DUMP_MEMORY_CHECK_IN_SECONDS);
std::string CWebDiagnostics::m_sizeOS;
std::string CWebDiagnostics::m_usedOS;
std::string CWebDiagnostics::m_availableOS;
std::string CWebDiagnostics::m_sizeSentinelData;
std::string CWebDiagnostics::m_usedSentinelData;
std::string CWebDiagnostics::m_availableSentinelData;
std::string CWebDiagnostics::m_sizeContent;
std::string CWebDiagnostics::m_usedContent;
std::string CWebDiagnostics::m_availableContent;
std::string CWebDiagnostics::m_sizeUSB;
std::string CWebDiagnostics::m_usedUSB;
std::string CWebDiagnostics::m_availableUSB;

CWebDiagnostics::CWebDiagnostics(IOperatingSystem &operatingSystem,
	UINT bufferSize) :
	m_buffer(new BYTE[bufferSize]),
	m_bufferSize(bufferSize),
	m_bytesRead(0), // Indicates number of valid bytes in m_buffer.
	m_errorFileSize(0),
	m_errorFileSizeUsed(0),
	m_errorsFound(false),
	m_operatingSystem(operatingSystem)
{
}

void CWebDiagnostics::Start()
{
	m_processHandler.Start();

	// Get the path of the error file to use as a source for diagnostics.
	string errorFileName(CDirectoryManager::CallstackFilePath());

	shared_ptr<istream> errorFile(m_operatingSystem.CreateFile(errorFileName, ios_base::in));
	m_bytesRead = LoadErrorFile(errorFile, m_errorFileSizeUsed, m_errorFileSize, m_buffer, m_bufferSize);

	m_errorsFound = m_bytesRead > 0;
}

CWebDiagnostics::~CWebDiagnostics(void)
{
	delete m_buffer;
}

UINT CWebDiagnostics::LoadErrorFile(const std::shared_ptr<std::istream> &file,
	UINT &errorFileSizeUsed,
	ULONGLONG &errorFileSize,
	BYTE *buffer,
	UINT bufferSize)
{
	DWORD fileUpdateCount = CDiagnostics::GetFileUpdateCount(); // Get count identifier before reading.

	UINT bytesRead(0);
	memset(buffer, 0, bufferSize);
	BOOL ok = CDiagnostics::ReadEndOfFile(file, (char*)buffer, bufferSize, bytesRead, errorFileSize);

	if (ok)
	{
		m_fileUpdateCount = fileUpdateCount; // Save count identifier, so we can tell if a diagnostic file change occurred.
		string subject((char*)buffer, bytesRead);
		errorFileSizeUsed = subject.length();
		CUtilities::FindAndReplace(subject, "<", "&lt;");
		CUtilities::FindAndReplace(subject, "\n", "\n<BR>");
		memcpy(buffer, subject.c_str(), subject.length());
		bytesRead = subject.length();
	}

	return bytesRead;
}

const BYTE* CWebDiagnostics::GetErrorBuffer(int &bytesRead) const
{
	bytesRead = m_bytesRead;
	return m_buffer;
}

void CWebDiagnostics::Update(const CWebDiagnosticsCommonInfo &commonInfo,
	const CWebDiagnosticsStatusInfo &statusInfo,
	const CPlayerSession &playerSession,
	const CEGMConfig &egmConfig,
	const CExcessiveVoucherOut& excessiveVoucherOut)
{
	// If a diagnostic file change has occurred then reload the error file.
	if (m_fileUpdateCount != CDiagnostics::GetFileUpdateCount())
	{
		string errorFileName(CDirectoryManager::CallstackFilePath());

		shared_ptr<istream> errorFile(m_operatingSystem.CreateFile(errorFileName, ios_base::in));
		m_bytesRead = LoadErrorFile(errorFile, m_errorFileSizeUsed, m_errorFileSize, m_buffer, m_bufferSize);

		m_errorsFound = m_bytesRead > 0;
	}

	// Get the path of the status dump file
	string statusDumpFileName;
	statusDumpFileName = Path::AddSlash(CDirectoryManager::Instance().HtmlFilePath().c_str()) + STATUS_DUMP_FILE_NAME;

	// Get the path of the memory dump file
	string memoryDumpFileName;
	memoryDumpFileName = Path::AddSlash(CDirectoryManager::Instance().HtmlFilePath().c_str()) + MEMORY_DUMP_FILE_NAME;

	// Get the path of the error dump file
	string errorDumpFileName;
	errorDumpFileName = Path::AddSlash(CDirectoryManager::Instance().HtmlFilePath().c_str()) + ERROR_DUMP_FILE_NAME;

	m_processHandler.Update();

	shared_ptr<ostream> file = m_operatingSystem.CreateFile(statusDumpFileName, ios_base::out | ios_base::trunc);
	SaveStatusData(file, commonInfo, statusInfo, playerSession, egmConfig, excessiveVoucherOut, m_errorsFound);
	file->flush();

	file = m_operatingSystem.CreateFile(memoryDumpFileName, ios_base::out | ios_base::trunc);
	SaveMemoryData(file, commonInfo, m_processHandler, m_errorsFound);
	file->flush();

	file = m_operatingSystem.CreateFile(errorDumpFileName, ios_base::out | ios_base::trunc);
	SaveErrorData(file, commonInfo, m_errorFileSizeUsed, m_errorFileSize, m_buffer, m_bytesRead);
	file->flush();
}

void CWebDiagnostics::UpdateCertificatesInfo(const CWebDiagnosticsCommonInfo &commonInfo, const vector<string>& listOfCerts)
{
	// Get the path of the dump file.
	string dumpFileName(Path::AddSlash(CDirectoryManager::Instance().HtmlFilePath().c_str()) + CertificatesInfoFilename);
	shared_ptr<ostream> file = m_operatingSystem.CreateFile(dumpFileName, ios_base::out | ios_base::trunc);
	try
	{
		string pageHtml;
		pageHtml = FormatString("<html><META Http-equiv=\"refresh\" Content=\"%d\">", (int)CERTS_INFO_REFRESH_SECONDS);
		Write(file, pageHtml);
		Write(file, GetCommonHeaderHtml(commonInfo));

		if (listOfCerts.empty())
		{
			Write(file, _T("No certificates installed in the chrome certificate database store."));
		}
		else
		{
			Write(file, _T("List of Certs:<br>"));
			for (auto cert : listOfCerts)
			{
				Write(file, cert + "<br>");
			}
		}
		Write(file, _T("</html>"));
	}
	catch (...)
	{
		// We do not want to throw to calling the thread.
	}

	file->flush();
}

void CWebDiagnostics::UpdateOneLinkContentInfo(const CWebDiagnosticsCommonInfo &commonInfo, const string& contentInfo)
{
	// Get the path of the dump file.
	string dumpFileName(Path::AddSlash(CDirectoryManager::Instance().HtmlFilePath().c_str()) + OneLinkContentFilename);
	shared_ptr<ostream> file = m_operatingSystem.CreateFile(dumpFileName, ios_base::out | ios_base::trunc);
	try
	{
		string pageHtml;
		pageHtml = FormatString("<html><META Http-equiv=\"refresh\" Content=\"%d\">", (int)ONELINK_INFO_REFRESH_SECONDS);
		Write(file, pageHtml);
		Write(file, GetCommonHeaderHtml(commonInfo));
		Write(file, contentInfo);
		Write(file, _T("</html>"));
	}
	catch (...)
	{
		// We do not want to throw to calling the thread.
	}

	file->flush();
}

void CWebDiagnostics::UpdateQueuingConfig(const CWebDiagnosticsCommonInfo &commonInfo,
	const CQueuingConfig &queueConfig)
{
	// Get the path of the dump file.
	string dumpFileName;
	dumpFileName = Path::AddSlash(CDirectoryManager::Instance().HtmlFilePath().c_str()) + QUEUING_CONFIG_FILE_NAME;

	shared_ptr<ostream> file = m_operatingSystem.CreateFile(dumpFileName, ios_base::out | ios_base::trunc);
	SaveQueuingConfigData(file, commonInfo, queueConfig);
	file->flush();
}

void CWebDiagnostics::UpdateOptionsAndSettings(const CWebDiagnosticsCommonInfo& commonInfo, const std::string& optionsAndSettingsString, const string& extensibleSystemSettings)
{
	try
	{
		string combinedOptionsAndSettings(optionsAndSettingsString + extensibleSystemSettings);

		if (m_lastOptionsAndSettingsString != combinedOptionsAndSettings)
		{
			m_lastOptionsAndSettingsString = combinedOptionsAndSettings;
			
			string localLastConfigAndOptionsString(optionsAndSettingsString);
			localLastConfigAndOptionsString.append("<br><br>");
			localLastConfigAndOptionsString.append(extensibleSystemSettings);
			localLastConfigAndOptionsString.erase(remove(localLastConfigAndOptionsString.begin(), localLastConfigAndOptionsString.end(), '\"'), localLastConfigAndOptionsString.end());

			vector<string> strListToRemove;
			strListToRemove.push_back("{Label:");
			strListToRemove.push_back( ",Value");
			CUtilities::EraseSubStrings(localLastConfigAndOptionsString, strListToRemove);
			CUtilities::FindAndReplace(localLastConfigAndOptionsString, ",", "<br>");
			CUtilities::FindAndReplace(localLastConfigAndOptionsString, "SentinelOptions:", "<br><b>SentinelOptions:</b><br>");
			CUtilities::FindAndReplace(localLastConfigAndOptionsString, "GlobalOptions:", "<br><b>GlobalOptions:</b><br>");
			CUtilities::FindAndReplace(localLastConfigAndOptionsString, "SlotMastID:", "SlotMast ID:");
			localLastConfigAndOptionsString.erase(remove(localLastConfigAndOptionsString.begin(), localLastConfigAndOptionsString.end(), ']'), localLastConfigAndOptionsString.end());
			localLastConfigAndOptionsString.erase(remove(localLastConfigAndOptionsString.begin(), localLastConfigAndOptionsString.end(), '}'), localLastConfigAndOptionsString.end());
			localLastConfigAndOptionsString.erase(remove(localLastConfigAndOptionsString.begin(), localLastConfigAndOptionsString.end(), '{'), localLastConfigAndOptionsString.end());
			localLastConfigAndOptionsString.erase(remove(localLastConfigAndOptionsString.begin(), localLastConfigAndOptionsString.end(), '['), localLastConfigAndOptionsString.end());

			// Get the path of the dump file.
			string dumpFileName;
			dumpFileName = Path::AddSlash(CDirectoryManager::Instance().HtmlFilePath().c_str()) + CWebDiagnostics::OptionsAndSettingsFilename;
			shared_ptr<ostream> file = m_operatingSystem.CreateFile(dumpFileName, ios_base::out | ios_base::trunc);
			Write(file, "<html>");
			Write(file, FormatString(_T("Startup Time: %s<br>"), static_cast<LPCTSTR>(CUtilities::FormatDateTimeString(commonInfo.m_startupTime, "%m/%d/%Y %I:%M:%S %p").c_str())));
			Write(file, FormatString(_T("Last Update Time: %s<br><br>"), static_cast<LPCTSTR>(CUtilities::FormatDateTimeString(commonInfo.m_currentTime, "%m/%d/%Y %I:%M:%S %p").c_str())));
			Write(file, localLastConfigAndOptionsString);
			Write(file, _T("</html>"));
			file->flush();
		}
	}
	catch (...)
	{
		// We do not want to throw to calling the thread.
	}
}

void CWebDiagnostics::UpdateConfigExt(const CWebDiagnosticsCommonInfo &commonInfo,
	const CConfigExtFile &configExt)
{
	// Get the path of the dump file.
	string dumpFileName;
	dumpFileName = Path::AddSlash(CDirectoryManager::Instance().HtmlFilePath().c_str()) + CONFIG_EXTENSIONS_FILE_NAME;

	shared_ptr<ostream> file = m_operatingSystem.CreateFile(dumpFileName, ios_base::out | ios_base::trunc);
	SaveConfigExtData(file, commonInfo, configExt);
	file->flush();
}

std::string CWebDiagnostics::GetDiagString(const CWebDiagnosticsCommonInfo &info)
{
	string retString;
	retString = FormatString(_T("Available Physical Memory(KB): %u<BR>Poller IP: %s<BR>Poller Packet Sent Verify Errors: %u<BR>Poller Packet Version: %u<BR>Online Status: %s"),
		(unsigned)(info.m_availablePhysicalMemory / BYTES_PER_KB),
		info.m_pollerIP.c_str(),
		(unsigned)info.m_pollerPacketSentVerifyErrors,
		(unsigned)info.m_pollerPacketVersion,
		info.m_onlineStateStr.c_str());

	return retString;
}

std::string CWebDiagnostics::GetCommonHeaderHtml(const CWebDiagnosticsCommonInfo &info)
{
	std::string activeSkinName, activeSkinVersion, activeSkinRevision;
	std::string pendingSkinName, pendingSkinVersion, pendingSkinRevision, pendingSkinMediaCount;

	activeSkinName = info.m_activeSkinInfo.GetName();
	if (!activeSkinName.empty())
	{
		activeSkinVersion = info.m_activeSkinInfo.GetVersion();
		activeSkinRevision = std::to_string(info.m_activeSkinInfo.GetRevision());
	}

	pendingSkinName = info.m_pendingSkinInfo.GetName();
	if (!pendingSkinName.empty())
	{
		pendingSkinVersion = info.m_pendingSkinInfo.GetVersion();
		pendingSkinRevision = std::to_string(info.m_pendingSkinInfo.GetRevision());
		pendingSkinMediaCount = std::to_string(info.m_pendingSkinInfo.GetPendingMediaCount());
	}

	std::string activeCampaignName, activeCampaignId, activeCampaignRevision;
	std::string pendingCampaignName, pendingCampaignId, pendingCampaignRevision, pendingCampaignMediaCount;

	activeCampaignName = info.m_activeCampaignInfo.GetName();
	if (!activeCampaignName.empty())
	{
		activeCampaignId = std::to_string(info.m_activeCampaignInfo.GetId());
		activeCampaignRevision = std::to_string(info.m_activeCampaignInfo.GetRevision());
	}

	pendingCampaignName = info.m_pendingCampaignInfo.GetName();
	if (!pendingCampaignName.empty())
	{
		pendingCampaignId = std::to_string(info.m_pendingCampaignInfo.GetId());
		pendingCampaignRevision = std::to_string(info.m_pendingCampaignInfo.GetRevision());
		pendingCampaignMediaCount = std::to_string(info.m_pendingCampaignInfo.GetPendingMediaCount());
	}

	string html;
	html += FormatString(_T("Startup Time: %s<br>"), static_cast<LPCTSTR>(CUtilities::FormatDateTimeString(info.m_startupTime, "%m/%d/%Y %I:%M:%S %p").c_str()));
	html += FormatString(_T("Current Time: %s<br>"), static_cast<LPCTSTR>(CUtilities::FormatDateTimeString(info.m_currentTime, "%m/%d/%Y %I:%M:%S %p").c_str()));
	html += FormatString(_T("SlotMast ID: %ld<br>"), (long)info.m_slotMastID);
	html += FormatString(_T("Machine Number:%ld<br>"), (long)info.m_machineNumber); // No space after ':' for backwards compatibility.
	html += FormatString(_T("Sentinel Version: %s<br>"), static_cast<LPCTSTR>(info.m_sentinelVersion.c_str()));
	html += FormatString(_T("OS Image Version: %s<br>"), static_cast<LPCTSTR>(info.m_osImageVersion.c_str()));
	html += FormatString(_T("Active Skin Name: %s<br>"), static_cast<LPCTSTR>(activeSkinName.c_str()));
	html += FormatString(_T("Active Skin Version: %s<br>"), static_cast<LPCTSTR>(activeSkinVersion.c_str()));
	html += FormatString(_T("Active Skin Revision: %s<br>"), static_cast<LPCTSTR>(activeSkinRevision.c_str()));

	if (!pendingSkinName.empty())
	{
		html += FormatString(_T("Pending Skin Name: %s<br>"), static_cast<LPCTSTR>(pendingSkinName.c_str()));
		html += FormatString(_T("Pending Skin Version: %s<br>"), static_cast<LPCTSTR>(pendingSkinVersion.c_str()));
		html += FormatString(_T("Pending Skin Revision: %s<br>"), static_cast<LPCTSTR>(pendingSkinRevision.c_str()));
		html += FormatString(_T("Pending Skin Media Count Still To Download: %s<br>"), static_cast<LPCTSTR>(pendingSkinMediaCount.c_str()));
	}

	html += FormatString(_T("Active Campaign Name: %s<br>"), static_cast<LPCTSTR>(activeCampaignName.c_str()));
	html += FormatString(_T("Active Campaign Id: %s<br>"), static_cast<LPCTSTR>(activeCampaignId.c_str()));
	html += FormatString(_T("Active Campaign Revision: %s<br>"), static_cast<LPCTSTR>(activeCampaignRevision.c_str()));

	if (!pendingCampaignName.empty())
	{
		html += FormatString(_T("Pending Campaign Name: %s<br>"), static_cast<LPCTSTR>(pendingCampaignName.c_str()));
		html += FormatString(_T("Pending Campaign Id: %s<br>"), static_cast<LPCTSTR>(pendingCampaignId.c_str()));
		html += FormatString(_T("Pending Campaign Revision: %s<br>"), static_cast<LPCTSTR>(pendingCampaignRevision.c_str()));
		html += FormatString(_T("Pending Campaign Media Count Still To Download: %s<br>"), static_cast<LPCTSTR>(pendingCampaignMediaCount.c_str()));
	}

	html += FormatString(_T("Carrier Version: %s<br>"), static_cast<LPCTSTR>(info.m_carrierVersion.c_str()));
	html += FormatString(_T("VM Version: %s<br>"), static_cast<LPCTSTR>(info.m_vmVersion.c_str()));
	html += FormatString(_T("Battery Status: %s<br>"), static_cast<LPCTSTR>(info.m_batteryStatus.c_str()));
	html += FormatString(_T("CPU Temperature (Celsius): %.2f<br>"), ((double)(info.m_lCPUtemp))/CPU_TEMP_DIVISOR);
	html += FormatString(_T("BIOS Version: %s<br>"), CPlatform::Instance().GetBiosVersion().c_str());
	html += FormatString(_T("Processor Type: %s<br>"), CPlatform::Instance().GetProcessorType().c_str());
	string ethernetType, mcuType, touchscreenType;
	CPlatform::Instance().GetHardwareComponents(ethernetType, mcuType, touchscreenType);
	html += FormatString(_T("Ethernet Type: %s<br>"), ethernetType.c_str());
	html += FormatString(_T("MCU Type: %s<br>"), mcuType.c_str());
	html += FormatString(_T("Touchscreen Type: %s<br>"), touchscreenType.c_str());

	time_t currentTime(CUtilities::GetCurrentTime());
	bool makeMemorySystemCall(false);
	if (currentTime - m_lastDumpMemorySystemCall > DUMP_MEMORY_CHECK_IN_SECONDS)
	{
		makeMemorySystemCall = true;
		m_lastDumpMemorySystemCall = currentTime;
	}

	if (makeMemorySystemCall)
	{
		CUtilities::GetPathMemoryUsageInformation(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_OS), m_sizeOS, m_usedOS, m_availableOS);
		CUtilities::GetPathMemoryUsageInformation(string(CDirectoryManager::SENTINEL_DATA_SD_PATH), m_sizeSentinelData, m_usedSentinelData, m_availableSentinelData);
		CUtilities::GetPathMemoryUsageInformation(string(CDirectoryManager::CONTENT_SD_PATH), m_sizeContent, m_usedContent, m_availableContent);
	}
	html += FormatString(_T("Disk Space Info SD Card1 (OS) Size:%s<br>"), m_sizeOS.c_str());
	html += FormatString(_T("Disk Space Info SD Card1 (OS) Available:%s<br>"), m_availableOS.c_str());
	html += FormatString(_T("Disk Space Info SD Card1 (OS) Used:%s<br>"), m_usedOS.c_str());
	html += FormatString(_T("Disk Space Info SD Card1 (SentinelData) Size:%s<br>"), m_sizeSentinelData.c_str());
	html += FormatString(_T("Disk Space Info SD Card1 (SentinelData) Available:%s<br>"), m_availableSentinelData.c_str());
	html += FormatString(_T("Disk Space Info SD Card1 (SentinelData) Used:%s<br>"), m_usedSentinelData.c_str());
	html += FormatString(_T("Disk Space Info SD Card2 (Content) Size:%s<br>"), m_sizeContent.c_str());
	html += FormatString(_T("Disk Space Info SD Card2 (Content) Available:%s<br>"), m_availableContent.c_str());
	html += FormatString(_T("Disk Space Info SD Card2 (Content) Used:%s<br>"), m_usedContent.c_str());

	string usbPath(CSentinelState::GetUsbFilePath());
	if (usbPath.empty())
	{
		m_sizeUSB.clear();
		m_usedUSB.clear();
		m_availableUSB.clear();
	}
	else if(makeMemorySystemCall)
	{
		CUtilities::GetPathMemoryUsageInformation(usbPath, m_sizeUSB, m_usedUSB, m_availableUSB);
	}	
	if (!usbPath.empty())
	{
		html += FormatString(_T("Disk Space Info USB Size:%s<br>"), m_sizeUSB.c_str());
		html += FormatString(_T("Disk Space Info USB Available:%s<br>"), m_availableUSB.c_str());
		html += FormatString(_T("Disk Space Info USB Used:%s<br>"), m_usedUSB.c_str());
	}
	html+= _T("<br>");

	return html;
}

std::string CWebDiagnostics::GetDownloadStatusHtml(const CFlashStatus &downloadStatus)
{
	std::string filename(downloadStatus.GetFilename());
	std::string html;

	html += FormatString(_T("Download Information<BR>"));
	if (filename.empty())
	{
		html += FormatString(_T("N/A<BR>"));
	}
	else
	{
		html += FormatString(_T("Filename: %s<br>"), filename.c_str());
		html += FormatString(_T("Url: %s<br>"), downloadStatus.GetUrl().c_str());
		html += FormatString(_T("Checksum: %s<br>"), downloadStatus.GetChecksum().c_str());
		html += FormatString(_T("Timestamp: %s<br>"), downloadStatus.GetTimestamp().c_str());

		std::string status = downloadStatus.GetStatus();
		if (status == DOWNLOAD_STATUS_INCOMPATIBLE_FILE_TYPE)
		{
			status = "Failure - Incompatible File Type";
		}
		else if (status == DOWNLOAD_STATUS_MEDIA_CARD_ERROR)
		{
			status = "Failure - Media card missing or full";
		}
		else if (status == DOWNLOAD_STATUS_DOWNLOAD_ERROR)
		{
			status = "Failure - Download Error";
		}
		else if (status == DOWNLOAD_STATUS_CHECKSUM_VALIDATION)
		{
			status = "Validating Checksum...";
		}
		else if (status == DOWNLOAD_STATUS_CHECKSUM_ERROR)
		{
			status = "Failure - Checksum Mismatch";
		}
		else if (status == DOWNLOAD_STATUS_EXTRACTING_CONTENTS)
		{
			status = "Extracting Contents...";
		}
		else if (status == DOWNLOAD_STATUS_EXTRACTION_ERROR)
		{
			status = "Failure - Extraction Error";
		}
		else if (status == DOWNLOAD_STATUS_SUCCESS)
		{
			status = "Success";
		}
		else if (status.empty())
		{
			status = "In Progress";
		}
		else
		{
			status = "Unknown";
		}

		html += FormatString(_T("Status: %s<br>"), status.c_str());
	}

	html += _T("<br>");
	return html;
}

std::string CWebDiagnostics::GetErrorsFoundHtml(bool errorsFound)
{
	string html;
	if (errorsFound)
	{
		html += FormatString(_T("ERRORS FOUND - See %s<BR>"), ERROR_DUMP_FILE_NAME);
	}

	return html;
}

std::string CWebDiagnostics::GetCommonTrailerHtml(const CWebDiagnosticsCommonInfo &info, bool errorsFound)
{
	string html(GetDiagString(info));
	html += _T("<BR><BR>");
	html += GetErrorsFoundHtml(errorsFound);

	return html;
}

std::string CWebDiagnostics::GetErrorHeaderHtml(const CWebDiagnosticsCommonInfo &info, UINT errorFileSizeUsed, ULONGLONG errorFileSize)
{
	string html;
	html += GetCommonHeaderHtml(info);
	html += FormatString(_T("%s<br><br>"), GetDiagString(info).c_str());
	html += _T("Debug data");
	if (errorFileSizeUsed != errorFileSize)
	{
		html += _T(" (partial)");
	}
	html += _T(":<br>");

	return html;
}

std::string CWebDiagnostics::GetStatusHtml(const CWebDiagnosticsStatusInfo &info)
{
	string timeSinceFloorLogixUpdate(_T("N/A"));
	if (info.m_tickCountOfLastFloorLogixTimeUpdateAvailable)
	{
		DWORD elapsedTickCount = info.m_currentTickCount - info.m_tickCountOfLastFloorLogixTimeUpdate;
		const time_t timeSpan(elapsedTickCount / TICK_COUNTS_PER_SECOND);
		timeSinceFloorLogixUpdate = CUtilities::FormatTimeSpanString(timeSpan);
	}

	string timeSinceFloorLogixConfigString(_T("N/A"));
	if (info.m_timeOfLastFloorLogixConfigAvailable)
	{
		const time_t timeSpan(info.m_currentTime - info.m_timeOfLastFloorLogixConfig);
		timeSinceFloorLogixConfigString = CUtilities::FormatTimeSpanString(timeSpan);
	}

	string lastUIRestartTimeString(_T("N/A"));
	if (info.m_uiRestartCount > 0)
	{
		lastUIRestartTimeString = CUtilities::FormatDateTimeString(info.m_lastUIRestartTime, "%m/%d/%Y %I:%M:%S %p");
	}

	string lastUIRefreshTimeString(_T("N/A"));
	if (info.m_uiRefreshCount > 0)
	{
		lastUIRefreshTimeString = CUtilities::FormatDateTimeString(info.m_lastUIRefreshTime, "%m/%d/%Y %I:%M:%S %p");
	}

	string html;
	html += FormatString("Time Since FloorLogix Time Update: %s<br>", static_cast<LPCTSTR>(timeSinceFloorLogixUpdate.c_str()));
	html += FormatString("Time Since FloorLogix Config: %s<br>", static_cast<LPCTSTR>(timeSinceFloorLogixConfigString.c_str()));
	html += FormatString("Maintenance Mode: %s<br>", info.m_maintenanceMode ? "Yes" : "No");
	html += FormatString("Time zone offset: %d:%2.2u<br>",
		(unsigned)(info.m_timeZoneOffsetInMinutes / MINUTES_PER_HOUR),
		(unsigned)(info.m_timeZoneOffsetInMinutes % MINUTES_PER_HOUR));
	html += FormatString("Code For Last Shutdown: %d<br>", (int)info.m_tiltCodeForLastShutdown);
	html += FormatString("Reason For Last Shutdown: %s<br>", static_cast<LPCTSTR>(info.TiltCodeToString(info.m_tiltCodeForLastShutdown).c_str()));
	html += FormatString("Last UI Restart: %s<br>", static_cast<LPCTSTR>(lastUIRestartTimeString.c_str()));
	html += FormatString("UI Restart Count: %u<br>", (unsigned)info.m_uiRestartCount);
	html += FormatString("Last UI Refresh: %s<br>", static_cast<LPCTSTR>(lastUIRefreshTimeString.c_str()));
	html += FormatString("UI Refresh Count: %u<br>", (unsigned)info.m_uiRefreshCount);
	html += FormatString("MAC Address1: %s<br>", static_cast<LPCTSTR>(info.m_macAddress.c_str()));
	html += FormatString("MAC Address2: %s<br>", static_cast<LPCTSTR>(info.m_macAddress2.c_str()));
	html += FormatString("Maximum PBT Amount: %s<br>", static_cast<LPCTSTR>(CUtilities::GetFormattedCashString(info.m_maxPBTAmount).c_str()));
	html += FormatString("Maximum Ticket Amount: %s<br>", static_cast<LPCTSTR>(CUtilities::GetFormattedCashString(info.m_maxTicketAmount).c_str()));
	html += FormatString("LCD Model: %s<br>", static_cast<LPCTSTR>(info.m_lcdModel.c_str()));
	html += FormatString("Pending Action: %s<br>", static_cast<LPCTSTR>(GetPendingActionText(info.m_pendingAction).c_str()));

	// Display memory thresholds last, as the memory available could be displayed next.
	html += FormatString("Available Physical Memory Threshold for Reboot After Drop(KB): %u<br>", (unsigned)info.m_rebootAfterDropThreshold * BYTES_PER_KB);
	html += FormatString("Available Physical Memory Threshold for Reboot ASAP(KB): %u<br>", (unsigned)info.m_rebootASAPThreshold * BYTES_PER_KB);
	html += FormatString("Available Physical Memory Threshold for UI Restart After Drop(KB): %u<br>", (unsigned)info.m_uiRestartAfterDropThreshold * BYTES_PER_KB);
	html += FormatString("Available Physical Memory Threshold for UI Restart ASAP(KB): %u<br>", (unsigned)info.m_uiRestartAsapThreshold * BYTES_PER_KB);

	html+= _T("<br>");
	html += FormatString("Total Number of Games: %u<br>", (unsigned)info.m_numberSubgames);
	html += FormatString("Total Number of Subgames Enabled: %u<br>", (unsigned)info.m_numberSubgamesEnabled);
	
	if (info.m_isAllGamesFetched)
	{
		string lastGamelistTimestampString = CUtilities::FormatDateTimeString(info.m_lastGamelistTimestamp, "%m/%d/%Y %I:%M:%S %p");
		html += FormatString("Last Games List Gathered Timestamp : %s<br>", lastGamelistTimestampString.c_str());
	}
	else
	{
		html += FormatString("Last Games List Gathered Timestamp : In-Progress<br>");
	}
	
	html += FormatString("Game List Chunks Sent : %u out of %u<br>", info.m_currentGameChunkIndex, info.m_totalGameListChunks);
	html+= _T("<br>");

	return html;
}

std::string CWebDiagnostics::GetPendingActionText(const std::string pendingAction)
{
	std::string localizedPendingAction;

	if (pendingAction == "NO_PENDING_ACTION")
	{
		localizedPendingAction = "None";
	}
	else if (pendingAction == "RESTART_UI")
	{
		localizedPendingAction = "Restart UI";
	}
	else if (pendingAction == "REBOOT_AFTER_DROP")
	{
		localizedPendingAction = "Reboot after drop  - met memory threshold criteria";
	}
	else if (pendingAction == "REBOOT_ASAP")
	{
		localizedPendingAction = "Reboot ASAP - met memory threshold criteria";
	}
	else if (pendingAction == "OS_UPGRADE")
	{
		localizedPendingAction = "OS Upgrade";
	}
	else if (pendingAction == "REMOTE_REBOOT")
	{
		localizedPendingAction = "Remote Reboot";
	}
	else if (pendingAction == "REFRESH_UI")
	{
		localizedPendingAction = "Refresh UI";
	}

	return localizedPendingAction;
}

void CWebDiagnostics::Write(const std::shared_ptr<std::ostream> &file, LPCTSTR string)
{
	*file << string;
}

void CWebDiagnostics::Write(const std::shared_ptr<std::ostream> &file, const std::string &string)
{
	*file << string;
}

void CWebDiagnostics::SaveStatusData(const std::shared_ptr<std::ostream> &file,
	const CWebDiagnosticsCommonInfo &commonInfo,
	const CWebDiagnosticsStatusInfo &statusInfo,
	const CPlayerSession &playerSession,
	const CEGMConfig &egmConfig,
	const CExcessiveVoucherOut& excessiveVoucherOut,
	bool errorsFound)
{
	try
	{
		{
			string pageHtml;
			pageHtml = FormatString("<html><META Http-equiv=\"refresh\" Content=\"%d\">", (int)STATUS_DUMP_REFRESH_SECONDS);
			Write(file, pageHtml);
			Write(file, GetCommonHeaderHtml(commonInfo));
			Write(file, GetStatusHtml(statusInfo));
			Write(file, GetDiagString(commonInfo));
			Write(file, _T("<BR><BR>"));
			Write(file, GetLastGameInfo(playerSession));
			Write(file, GetCurrentMultiplierAndDivider(statusInfo));
			Write(file, GetEgmConfigHtml(statusInfo, egmConfig));
			Write(file, excessiveVoucherOut.GetHtmlString());
			Write(file, GetDownloadStatusHtml(statusInfo.m_downloadStatus));
			Write(file, GetErrorsFoundHtml(errorsFound));
			Write(file, _T("</html>"));
		}
	}
	catch (...)
	{
		//we do not want to throw to calling thread
	}
}
std::string CWebDiagnostics::GetProcessHtml(const CProcess &process)
{
	string output;
	const CProcess *proc(&process);

	// Convert and report CPU time using the full FILETIME range,
	// as the low DWORD of a FILETIME only has a range of about 429 seconds.
	DWORD cpuHours;
	DWORD cpuMinutes;
	DWORD cpuSeconds;
	DWORD cpuMilliseconds;
	CUtilities::FileTimeToComponents(proc->m_totalProcTime, cpuHours, cpuMinutes, cpuSeconds, cpuMilliseconds);

	output = FormatString("<tr>" // No comma.
		"<td>Process:</td><td> %-20s</td>" // No comma.
		"<td>CPU Usage(%%):</td><td align=\"right\"> %d</td>" // No comma.
		"<td>Module Size(KB):</td><td align=\"right\"> %-10u</td>" // No comma.
		"<td>VM Size(KB):</td><td align=\"right\"> %-10u</td>" // No comma.
		"<td>CPU Time:</td><td align=\"right\"> %u:%02u:%02u.%03u</td>" // No comma.
		"<td>Handles:</td><td align=\"right\"> %-10u</td>"  // No comma.
		"</tr>",
		// Data arguments.
		proc->m_processName.c_str(),
		proc->m_cpuUsage,
		proc->m_moduleSize,
		proc->m_vmSize,
		(unsigned)cpuHours, (unsigned)cpuMinutes, (unsigned)cpuSeconds, (unsigned)cpuMilliseconds,
		(unsigned)proc->m_handleCount);

	return output;
}

void CWebDiagnostics::SaveMemoryData(const std::shared_ptr<std::ostream> &file,
	const CWebDiagnosticsCommonInfo &commonInfo,
	const CProcessHandler &processHandler,
	bool errorsFound)
{
	try
	{
		{
			string pageHtml;
			pageHtml = FormatString("<html><META Http-equiv=\"refresh\" Content=\"%d\">", (int)MEMORY_DUMP_REFRESH_SECONDS);
			Write(file, pageHtml);
			Write(file, GetCommonHeaderHtml(commonInfo));

			Write(file, _T("<table cellspacing=\"10\">"));
			CProcessHandler::IteratorType procPos;
			const CProcess *proc = processHandler.GetFirstProcess(procPos);
			while (proc != NULL)
			{
				if (proc->m_memUsage != 0 || proc->m_vmSize != 0)
				{
					Write(file, GetProcessHtml(*proc));
				}

				proc = processHandler.GetNextProcess(procPos);
				CUtilities::Sleep(0);
			}
			Write(file, _T("</table>"));

			Write(file, GetCommonTrailerHtml(commonInfo, errorsFound));
			Write(file, _T("</html>"));
		}
	}
	catch(...)
	{
		//we do not want to throw to calling thread
	}
}

LPCTSTR CWebDiagnostics::GetBoolString(bool value)
{
	return value ? LPCTSTR_TRUE : LPCTSTR_FALSE;
}

LPCTSTR CWebDiagnostics::GetEnabledDisabledString(bool value)
{
	return value ? LPCTSTR_ENABLED : LPCTSTR_DISABLED;
}

LPCSTR CWebDiagnostics::GetBoolStringA(bool value)
{
	return value ? LPCSTR_TRUE : LPCSTR_FALSE;
}

LPCSTR CWebDiagnostics::GetEnabledDisabledStringA(bool value)
{
	return value ? LPCSTR_ENABLED : LPCSTR_DISABLED;
}

std::string CWebDiagnostics::GetConfigExtHtml(const CConfigExtFile &configExt)
{
	string html;
	html += GetConfigExt1Html(configExt);
	html += GetConfigExt2Html(configExt);
	return html;
}

std::string CWebDiagnostics::GetConfigExt1Html(const CConfigExtFile &configExt)
{
	string html;
	
	WORD gameListChunkSize;
	configExt.GetGameListChunkSize(gameListChunkSize);
	html += FormatString("Game List Chunk Size: %u <br>", (unsigned) gameListChunkSize);

	html+= _T("<br>");

	{
		WORD negligibleCreditsThreshold;
		configExt.GetNegligibleCreditsThreshold(negligibleCreditsThreshold);
		html += FormatString("Negligible Credits Threshold: %s<br>",
			static_cast<LPCTSTR>(CUtilities::GetFormattedCashString((DWORD)negligibleCreditsThreshold).c_str()));
	}

	{
		bool employeeOverPlayerIdleCardOut;
		configExt.GetEmployeeOverPlayerIdleCardOut(employeeOverPlayerIdleCardOut);
		html += FormatString("Employee Over Player Idle Card Out: %s<br>",
			static_cast<LPCTSTR>(GetBoolString(employeeOverPlayerIdleCardOut)));
	}

	{
		WORD employeeIdleCardOutSeconds;
		configExt.GetEmployeeIdleCardOutSeconds(employeeIdleCardOutSeconds);
		html += FormatString("Employee Idle Card Out Seconds: %u<br>", (unsigned)employeeIdleCardOutSeconds);
	}

	{
		bool playerRecardEmployeeCardOut;
		configExt.GetPlayerRecardEmployeeCardOut(playerRecardEmployeeCardOut);
		html += FormatString("Player Recard Employee Card Out: %s<br>",
			static_cast<LPCTSTR>(GetBoolString(playerRecardEmployeeCardOut)));
	}

	html+= _T("<br>");

	{
		WORD sessionActiveNegligibleCreditsTimeoutSeconds;
		configExt.GetSessionActiveNegligibleCreditsTimeoutSeconds(sessionActiveNegligibleCreditsTimeoutSeconds);
		html += FormatString("Session Active Negligible Credits Timeout: %u seconds<br>", (unsigned) sessionActiveNegligibleCreditsTimeoutSeconds);
	}

	{
		WORD sessionActiveNonNegligibleCreditsTimeoutSeconds;
		configExt.GetSessionActiveNonNegligibleCreditsTimeoutSeconds(sessionActiveNonNegligibleCreditsTimeoutSeconds);
		html += FormatString("Session Active Non-negligible Credits Timeout: %u seconds<br>", (unsigned) sessionActiveNonNegligibleCreditsTimeoutSeconds);
	}

	html+= _T("<br>");
	return html;
}

std::string CWebDiagnostics::GetConfigExt2Html(const CConfigExtFile &configExt)
{
	string html;

	{
		WORD notificationForShowPromotionTime;
		configExt.GetNotificationForShowPromotionTime(notificationForShowPromotionTime);
		html += FormatString("Promotion Notification Seconds: %u<br>", (unsigned)notificationForShowPromotionTime);
	}

	{
		WORD notificationForPlayerTrackingTime;
		configExt.GetNotificationForPlayerTrackingTime(notificationForPlayerTrackingTime);
		html += FormatString("Player Tracking Notification Seconds: %u<br>", (unsigned)notificationForPlayerTrackingTime);
	}

	html+= _T("<br>");

	{
		bool customButton1Enabled;
		configExt.GetCustomButton1Enabled(customButton1Enabled);
		html += FormatString("Custom Button 1: %s<br>",
			static_cast<LPCTSTR>(GetEnabledDisabledString(customButton1Enabled)));
	}

	{
		WORD customMenuButton1Count;
		configExt.GetCustomMenuButton1Count(customMenuButton1Count);
		html += FormatString("Custom 1 Menu Items: %u<br>", (unsigned)customMenuButton1Count);
	}

	{
		bool customButton2Enabled;
		configExt.GetCustomButton2Enabled(customButton2Enabled);
		html += FormatString("Custom Button 2: %s<br>",
			static_cast<LPCTSTR>(GetEnabledDisabledString(customButton2Enabled)));
	}

	{
		WORD customMenuButton2Count;
		configExt.GetCustomMenuButton2Count(customMenuButton2Count);
		html += FormatString("Custom 2 Menu Items: %u<br>", (unsigned)customMenuButton2Count);
	}

	{
		WORD resortMenuButtonCount;
		configExt.GetResortMenuButtonCount(resortMenuButtonCount);
		html += FormatString("Resort Menu Items: %u<br>", (unsigned)resortMenuButtonCount);
	}

	html+= _T("<br>");

	{
		bool rebootUiRestartOverride;
		configExt.GetRebootUiRestartOverride(rebootUiRestartOverride);
		html += FormatString("Reboot UI Restart Override: %s<br>",
			static_cast<LPCTSTR>(GetEnabledDisabledString(rebootUiRestartOverride)));
	}

	{
		html += FormatString("<br>Bonus Counter Priorities:<br>");

		WORD value(0);

		value = 0;
		configExt.GetBonusCounterNotificationPriority(value);
		html += FormatString("Notification: %u<br>", (unsigned)value);

		value = 0;
		configExt.GetBonusCounternRichLevelPriority(value);
		html += FormatString("nRich Level: %u<br>", (unsigned)value);

		value = 0;
		configExt.GetBonusCounterPlayXGetYPriority(value);
		html += FormatString("PlayXGetY Odometer: %u<br>", (unsigned)value);

		value = 0;
		configExt.GetBonusCounterPrizePriority(value);
		html += FormatString("Prize Odometer: %u<br>", (unsigned)value);

		value = 0;
		configExt.GetBonusCounterProgressiveOdometerPriority(value);
		html += FormatString("Progressive Odometer: %u<br>", (unsigned)value);

		value = 0;
		configExt.GetBonusCounterPtePriority(value);
		html += FormatString("PTE: %u<br>", (unsigned)value);

		value = 0;
		configExt.GetBonusCounterScatterCountdownTimerPriority(value);
		html += FormatString("Scatter Countdown Timer: %u<br>", (unsigned)value);

		value = 0;
		configExt.GetBonusCounterSplashdownPriority(value);
		html += FormatString("Splashdown Countdown: %u<br>", (unsigned)value);
	}

	html += _T("<br>");

	return html;
}

std::string CWebDiagnostics::GetQueuingConfigHtml(const CQueuingConfig &queuingConfig)
{
	string html;

	{
		html += FormatString(IDS_EQUALPRIORITYINTERRUPT, GetEnabledDisabledString(static_cast<CQueuingConfig>(queuingConfig).GetEqualPriorityInterrupt()));
		html += FormatString(IDS_INITIALINTERACTIVESTARVETIME, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetInitialInteractiveStarveTime()));
		html += FormatString(IDS_INTERACTIVECONTENTSTARVETIME, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetInteractiveContentStarveTime()));
		html += FormatString(IDS_USERSCREENTIME, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetUserScreenTime()));
		html += FormatString(IDS_UJPTIMEOUT, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetUjpTimeout()));
		html += FormatString(IDS_PBTSCREENS, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetPbtScreens()));
		html += FormatString(IDS_PLAYERINITIATEDPRIORITY, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetPlayerInitiated()));
		html += FormatString(IDS_PLAYERINITIATEDTTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetPlayerInitiatedTTL()));
		html += FormatString(IDS_INTERACTIVECONTENTPRIORITY, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetInteractiveContent()));
		html += FormatString(IDS_INTERACTIVECONTENTTTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetInteractiveContentTTL()));
		html += FormatString(IDS_WINNERMESSAGEPRIORITY, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetWinnerMessage()));
		html += FormatString(IDS_WINNERMESSAGETTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetWinnerMessageTTL()));
		html += FormatString(IDS_MASSMARKETING_PRIORITY, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetMassMarketing()));
		html += FormatString(IDS_MASSMARKETING_TTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetMassMarketingTTL()));
		html += FormatString(IDS_CONGRATSPRIORITY, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetCongrats()));
		html += FormatString(IDS_CONGRATSTTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetCongratsTTL()));
		html += FormatString(IDS_SCHEDULEENDPRIORITY, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetScheduleEnd()));
		html += FormatString(IDS_SCHEDULEENDTTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetScheduleEndTTL()));
		html += FormatString(IDS_WINNERANTICIPATIONPRIORITY, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetWinnerAnticipation()));
		html += FormatString(IDS_WINNERANTICIPATIONTTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetWinnerAnticipationTTL()));
		html += FormatString(IDS_ANNOUNCEMENTSPRIORITY, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetAnnouncements()));
		html += FormatString(IDS_ANNOUNCEMENTSTTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetAnnouncementsTTL()));
		html += FormatString(IDS_BIRTHDAYPRIORITY, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetBirthday()));
		html += FormatString(IDS_BIRTHDAYTTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetBirthdayTTL()));
		html += FormatString(IDS_ANNIVERSARYPRIORITY, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetAnniversary()));
		html += FormatString(IDS_ANNIVERSARYTTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetAnniversaryTTL()));
		html += FormatString(IDS_CDM_PRIORITY, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetCDM()));
		html += FormatString(IDS_CDM_TTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetCDMTTL()));
		html += FormatString(IDS_UNATTENDED_JACKPOT_PRIORITY, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetUnattendedJackpot()));
		html += FormatString(IDS_UNATTENDED_JACKPOT_TTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetUnattendedJackpotTTL()));
		html += FormatString(IDS_UNATTENDED_JACKPOT_MEDIA_PRIORITY, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetUnattendedJackpotMedia()));
		html += FormatString(IDS_UNATTENDED_JACKPOT_MEDIA_TTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetUnattendedJackpotMediaTTL()));
		html += FormatString(IDS_INVITED_GROUPS_ANTICIPATION_PRIORITY, static_cast<CQueuingConfig>(queuingConfig).GetInvitedGroupsAnticipation());
		html += FormatString(IDS_INVITED_GROUPS_ANTICIPATION_TTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetInvitedGroupsAnticipationTTL()));
		html += FormatString(IDS_INVITED_GROUPS_LEVEL_UNLOCKED_PRIORITY, static_cast<CQueuingConfig>(queuingConfig).GetInvitedGroupsLevelUnlocked());
		html += FormatString(IDS_INVITED_GROUPS_LEVEL_UNLOCKED_TTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetInvitedGroupsLevelUnlockedTTL()));
		html += FormatString(IDS_INVITED_GROUPS_LEADERBOARD_PRIORITY, static_cast<CQueuingConfig>(queuingConfig).GetInvitedGroupsLeaderBoard());
		html += FormatString(IDS_INVITED_GROUPS_LEADERBOARD_TTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetInvitedGroupsLeaderBoardTTL()));
		html += FormatString(IDS_INVITED_GROUPS_FINAL_RESULTS_PRIORITY, static_cast<CQueuingConfig>(queuingConfig).GetInvitedGroupsFinalResults());
		html += FormatString(IDS_INVITED_GROUPS_FINAL_RESULTS_TTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetInvitedGroupsFinalResultsTTL()));
		html += FormatString(IDS_INVITED_GROUPS_ENDING_SOON_PRIORITY, static_cast<CQueuingConfig>(queuingConfig).GetInvitedGroupsEndingSoon());
		html += FormatString(IDS_INVITED_GROUPS_ENDING_SOON_TTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetInvitedGroupsEndingSoonTTL()));
		html += FormatString(IDS_PROGRESSIVE_PRIZE_PRIORITY, static_cast<CQueuingConfig>(queuingConfig).GetProgressivePrize());
		html += FormatString(IDS_PROGRESSIVE_PRIZE_TTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetProgressivePrizeTTL()));
		html += FormatString(IDS_PROMO_GAME_PRIORITY, static_cast<CQueuingConfig>(queuingConfig).GetPromoGame());
		html += FormatString(IDS_PROMO_GAME_TTL, (unsigned)(static_cast<CQueuingConfig>(queuingConfig).GetPromoGameTTL()));
	}

	html+= _T("<br>");

	return html;
}

void CWebDiagnostics::SaveConfigExtData(const std::shared_ptr<std::ostream> &file,
	const CWebDiagnosticsCommonInfo &commonInfo,
	const CConfigExtFile &configExt)
{
	try
	{
		{
			string pageHtml;
			pageHtml = FormatString("<html><META Http-equiv=\"refresh\" Content=\"%d\">", (int)CONFIG_EXTENSIONS_REFRESH_SECONDS);
			Write(file, pageHtml);
			Write(file, GetCommonHeaderHtml(commonInfo));

			Write(file, GetConfigExtHtml(configExt));
			Write(file, _T("</html>"));
		}
	}
	catch (...)
	{
		// We do not want to throw to calling the thread.
	}
}

void CWebDiagnostics::SaveQueuingConfigData(const std::shared_ptr<std::ostream> &file,
	const CWebDiagnosticsCommonInfo &commonInfo,
	const CQueuingConfig &queuingConfig)
{
	try
	{
		{
			string pageHtml;
			pageHtml = FormatString("<html><META Http-equiv=\"refresh\" Content=\"%d\">", (int)QUEUINGCONFIG_REFRESH_SECONDS);
			Write(file, pageHtml);
			Write(file, GetCommonHeaderHtml(commonInfo));

			Write(file, GetQueuingConfigHtml(queuingConfig));
			Write(file, _T("</html>"));
		}
	}
	catch (...)
	{
		// We do not want to throw to calling the thread.
	}
}

void CWebDiagnostics::SaveErrorData(const std::shared_ptr<std::ostream> &file,
	const CWebDiagnosticsCommonInfo &commonInfo,
	UINT errorFileSizeUsed,
	ULONGLONG errorFileSize,
	const byte *errorBuffer,
	int errorBufferLength)
{
	{
		string html;

		//Write out page html
		html = FormatString("<html>");
		Write(file, html);
		Write(file, GetErrorHeaderHtml(commonInfo, errorFileSizeUsed, errorFileSize));

		if (errorBufferLength > 0)
		{
			file->write(reinterpret_cast<const char *>(errorBuffer), errorBufferLength);
		}

		Write(file, _T("</html>"));
	}
}

LPCSTR CWebDiagnostics::TrueFalseStr(bool val)
{
	return val ? LPCSTR_TRUE : LPCSTR_FALSE;
}

LPCSTR CWebDiagnostics::EnabledDisabledStr(bool val)
{
	return val ? LPCSTR_ENABLED : LPCSTR_DISABLED;
}
