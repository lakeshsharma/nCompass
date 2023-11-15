#include "stdafx.h"
#include "DirectoryManager.h"

// Sentinel includes.
#include "CarrierUpgrade.h"
#include "GlobalDefs.h"
#include "Hardware/WatchDog.h"
#include "DisplayManager.h"
#include "Logging/LogString.h"
#include "UI/ScreenDefs.h"
#include "Util/FileSys.h"
#include "Util/Path.h"
#include "VideoMixerUpgrade.h"

using namespace std;

CDirectoryManager *CDirectoryManager::m_instance = NULL;
std::recursive_mutex CDirectoryManager::m_instanceCS;

LPCTSTR CDirectoryManager::BASEURL("https://localhost:8080");
LPCTSTR CDirectoryManager::USB_PATH_PREFIX("/run/media/sd");
LPCSTR CDirectoryManager::SENTINEL_DATA_SD_PATH("/opt/ncompass/sys/");
LPCSTR CDirectoryManager::CONTENT_SD_PATH("/opt/ncompass/mediadisk/");
LPCSTR CDirectoryManager::DIRECTORY_KERNEL_NET_ETH("/sys/class/net/eth0");

LPCTSTR CDirectoryManager::OSPath("/sdboot");// the OS path
#ifndef _UNIT_TEST 
LPCTSTR CDirectoryManager::SentinelBasePath("/opt/ncompass");// the base home path
#else
LPCTSTR CDirectoryManager::SentinelBasePath("/tmp/ncompass");// the base home path for gtest, as it doesn't have previleges to access /opt/*
#endif
LPCTSTR CDirectoryManager::ApplicationPath("/bin");// the ncompass application path
LPCTSTR CDirectoryManager::CompactMediaDefaultPath("/media/default/compactmedia");// the default Compact media folder
LPCTSTR CDirectoryManager::LCDMediaDefaultPath("/media/default/lcdmedia");// the default LCD media folder
LPCTSTR CDirectoryManager::OneLinkContentPathUrl("/assets/dynamicContent/");// the OneLink media folder
LPCTSTR CDirectoryManager::MWMediaDefaultPath("/media/default/mwmedia");// the default MediaWindow media folder
LPCTSTR CDirectoryManager::CompactMediaCustomPath("/media/custom/compactmedia");// the custom Compact media folder
LPCTSTR CDirectoryManager::LCDMediaCustomPath("/media/custom/lcdmedia");// the custom LCD media folder
LPCTSTR CDirectoryManager::MWMediaCustomPath("/media/custom/mwmedia");// the default MediaWindow media folder
LPCTSTR CDirectoryManager::CompactMediaTempPath("/media/custom/tempcompactmedia");// the temporary compact media folder (when skin is being downloaded)
LPCTSTR CDirectoryManager::LCDMediaTempPath("/media/custom/templcdmedia");// the temporary LCD media folder (when skin is being downloaded)
LPCTSTR CDirectoryManager::MWMediaTempPath("/media/custom/tempmwmedia");// the temporary MediaWindow media folder (when skin is being downloaded)
LPCTSTR CDirectoryManager::PLContentTempPath("/media/custom/tempplcontent");// the temporary PL content folder (when adloop is being downloaded)
LPCTSTR CDirectoryManager::PLContentPath("/media/custom/plcontent");// the PL content folder
LPCTSTR CDirectoryManager::DataDrivePath("/sys");// The data drive (equivalent to the "D" drive in the WES image)
LPCTSTR CDirectoryManager::SentinelDataPath("/sys/SentinelData");			// The sentinel data path. Where downloaded misc files go.
LPCTSTR CDirectoryManager::ErrorPath("/sys/ErrorFile");// There error path where the error.txt goes
LPCTSTR CDirectoryManager::ErrorFilePath("/opt/ncompass/sys/ErrorFile/Error.txt");// The full file path for the error file
LPCTSTR CDirectoryManager::BadPollerFilePath("/opt/ncompass/sys/ErrorFile/BADPOLLERDATA.sdf");	// The full file path for the errorbad poller data file
LPCTSTR CDirectoryManager::EepromBinFilename("/eeprom.bin");
LPCTSTR CDirectoryManager::VideoMixerDiagnosticsFile("/VideoMixerDiagnostics.txt");
LPCTSTR CDirectoryManager::ContentDrivePath("/opt/ncompass/mediadisk");// The content drive (equivalent to teh "E" drive in the WES image.
LPCTSTR CDirectoryManager::OneLinkContentPath("/media/custom/onelinkcontent");// The Onelink content path
LPCTSTR CDirectoryManager::OneLinkContentTempPath("/mediadisk/custom/temponelinkcontent");// the temporary OneLink content folder (when OneLink content is being downloaded)
LPCTSTR CDirectoryManager::DynamicContentPath("/media/assets/dynamicContent");// Path where zip content get extracted to
LPCTSTR CDirectoryManager::AssetsPath("/media/assets/");
LPCTSTR CDirectoryManager::CustomAssetsPath("/media/custom/assets/");
LPCTSTR CDirectoryManager::CustomDynamicContentPath("/media/custom/assets/dynamicContent");
LPCTSTR CDirectoryManager::DefaultCertsPath("/etc/certs");
LPCTSTR CDirectoryManager::AdditonalCertsPath("/media/custom/certs");
LPCTSTR CDirectoryManager::AdditonalCertsTempPath("/media/custom/certs_temp");

LPCTSTR CDirectoryManager::USBDrive1("/run/media/sda1");
LPCTSTR CDirectoryManager::USBDrive2("/run/media/sdb1");
LPCTSTR CDirectoryManager::USBDrive3("/run/media/sdc1");
LPCTSTR CDirectoryManager::USBDrive4("/run/media/sdd1");
LPCTSTR CDirectoryManager::USBDrive5("/run/media/sde1");
LPCTSTR CDirectoryManager::USBDrive6("/run/media/sdf1");
LPCTSTR CDirectoryManager::USBDrive7("/run/media/sdg1");
LPCTSTR CDirectoryManager::USBDrive8("/run/media/sdh1");
LPCTSTR CDirectoryManager::HTML_FILE_DRIVE_AND_PATH("/opt/ncompass/www");
LPCTSTR CDirectoryManager::HTTPD_PATH_FILENAME("/usr/sbin/httpd");
LPCTSTR CDirectoryManager::UPDATEPATH("/sdboot/update");
LPCTSTR CDirectoryManager::UPGRADEUNDOPATH("/sdboot/undo");
LPCTSTR CDirectoryManager::FIRMWAREPATH_LITE("/sdboot/firmware");
LPCTSTR CDirectoryManager::UPDATEPATH_LITE("/sdboot/firmware.new");
LPCTSTR CDirectoryManager::CLEAR_MEMORY("/sdboot/CLEAR_MEMORY");
LPCTSTR CDirectoryManager::AUTOMATION_MODE("/sdboot/AUTOMATION_MODE");
LPCTSTR CDirectoryManager::UPGRADEUNDOPATH_LITE("/sdboot/firmware.old");
LPCTSTR CDirectoryManager::CUSTOMMEDIA("/custom");
LPCTSTR CDirectoryManager::DEFAULTMEDIA("/default");
LPCTSTR CDirectoryManager::CompactMedia("/compactmedia");
LPCTSTR CDirectoryManager::LCDMedia("/lcdmedia");
LPCTSTR CDirectoryManager::MWMedia("/mwmedia");
LPCTSTR CDirectoryManager::AdLoop("/custom/plcontent");
LPCTSTR CDirectoryManager::COMPACT_SHELL_XML("/CompactShell.xml");
LPCTSTR CDirectoryManager::LCD_SHELL_XML("/LCDShell.xml");
LPCTSTR CDirectoryManager::MEDIA_WINDOW_SHELL_XML("/MediaWindowShell.xml");
LPCTSTR CDirectoryManager::RADBEACON_DEVICE_PATTERN("/dev/radbeacon");
LPCTSTR CDirectoryManager::RBCOMpath("/opt/ncompass/bin/rbcom list");

const string CDirectoryManager::OrientationCompact("compact");
const string CDirectoryManager::OrientationLcd("lcd");
const string CDirectoryManager::OrientationMediaWindowBottom("mwbottom");
const string CDirectoryManager::OrientationMediaWindowSide("mwside");
const string CDirectoryManager::REMOTE_DEBUG_FILE("/sdboot/REMOTE_DEBUG");
const string CDirectoryManager::REMOTE_UI_FILE("/sdboot/REMOTE_UI");

#ifdef _UNIT_TEST 
static string UTErrorFilePath("/tmp/Error.txt");
#endif

const SentinelDirectoryType CDirectoryManager::DirectoryToCreate[CDirectoryManager::NumDirectoriesToCreate] =
{
	DIRECTORY_CUSTOM,
	DIRECTORY_SENTINELDATA,
	DIRECTORY_MWMEDIATEMP,
	DIRECTORY_LCDMEDIATEMP,
	DIRECTORY_ERROR_FILE,
	DIRECTORY_PLCONTENT,
	DIRECTORY_PLCONTENTTEMP,
	DIRECTORY_ONELINK_CONTENT,
	DIRECTORY_ONELINK_CONTENTTEMP,
	DIRECTORY_ADDITIONAL_CERTIFICATE_STORE,
	DIRECTORY_ADDITIONAL_CERTIFICATE_STORETEMP
};

CDirectoryManager & CDirectoryManager::Instance()
{
	lock_guard<recursive_mutex> lock(m_instanceCS);

	if ( m_instance == NULL )
	{
		m_instance = new CDirectoryManager();
	}

	if (!m_instance->m_fullSetupDone)
	{
		m_instance->FullSetup();
	}

	return *m_instance;
}

CDirectoryManager & CDirectoryManager::MockInstance(std::ifstream *sentinelDrives, bool reset)
{
	// Instance without running full setup
	// Modify this to parse sentinelDrives for sentineldrives.txt data
	lock_guard<recursive_mutex> lock(m_instanceCS);

	if (m_instance == NULL || reset)
	{
		delete m_instance;
		m_instance = new CDirectoryManager();
	}

	if (!m_instance->m_fullSetupDone)
	{
		m_instance->MockSetup(sentinelDrives);
	}

	return *m_instance;
}

CDirectoryManager & CDirectoryManager::MockInstanceReset(std::ifstream *sentinelDrives)
{
	return MockInstance(sentinelDrives, true);
}

void CDirectoryManager::MockInstanceCleanup()
{
	lock_guard<recursive_mutex> lock(m_instanceCS);

	delete m_instance;
	m_instance = NULL;
}

void CDirectoryManager::MockSetup(std::ifstream *sentinelDrives)
{
    UNREFERENCED_PARAMETER(&sentinelDrives); 
#ifdef _UNIT_TEST
	m_isMockPath = true;
#endif
}

CDirectoryManager::CDirectoryManager()
{
	m_fullSetupDone = false;
}

CDirectoryManager::~CDirectoryManager()
{
	// To support test instances, do not delete the m_instance singleton.
	// Do not call this destructor recursively.
}


void CDirectoryManager::DetermineWorkingMediaPaths()
{
	string defaultPath, customPath;

	//Media Window
	defaultPath = string(SentinelBasePath) + string(MWMediaDefaultPath);
	customPath = string(SentinelBasePath) + string(MWMediaCustomPath);	
	m_mwMediaPathChunk = DEFAULTMEDIA;
	m_workingMWMediaPath = defaultPath;
	if (CUtilities::FileExists(customPath + MEDIA_WINDOW_SHELL_XML))
	{
		m_mwMediaPathChunk = CUSTOMMEDIA;
		m_workingMWMediaPath = customPath;
	}
	
	//LCD
	defaultPath = string(SentinelBasePath) + string(LCDMediaDefaultPath);
	customPath = string(SentinelBasePath) + string(LCDMediaCustomPath);
	m_lcdMediaPathChunk = DEFAULTMEDIA;
	m_workingLCDMediaPath = defaultPath;	
	if (CUtilities::FileExists(customPath + LCD_SHELL_XML))
	{
		m_lcdMediaPathChunk = CUSTOMMEDIA;
		m_workingLCDMediaPath = customPath;
	}

	//Compact
	defaultPath = string(SentinelBasePath) + string(CompactMediaDefaultPath);
	customPath = string(SentinelBasePath) + string(CompactMediaCustomPath);
	m_compactMediaPathChunk = DEFAULTMEDIA;
	m_workingCompactMediaPath = defaultPath;
	if (CUtilities::FileExists(customPath + COMPACT_SHELL_XML))
	{
		m_compactMediaPathChunk = CUSTOMMEDIA;
		m_workingCompactMediaPath = customPath;
	}
}

void CDirectoryManager::FullSetup()
{
	// When new skins are downloaded, they are put into the temp directories.
	// When we come up, we need to rename these temp directories in order to use the newly downloaded skins.
	RenameTempDirectories();
	
	DetermineWorkingMediaPaths();

	DeleteUnwantedDirectories();
	
	// Check for existence of temp directories, unwanted smd files, FOUND.XXX files
	CheckDisk();

	// This will create directories that are expected to be there.
	CreateDirectories();
	
	m_usbPathArray.push_back(USBDrive1);
	m_usbPathArray.push_back(USBDrive2);
	m_usbPathArray.push_back(USBDrive3);
    m_usbPathArray.push_back(USBDrive4);
	m_usbPathArray.push_back(USBDrive5);
	m_usbPathArray.push_back(USBDrive6);
	m_usbPathArray.push_back(USBDrive7);
	m_usbPathArray.push_back(USBDrive8);

	m_fullSetupDone = true;
}

void CDirectoryManager::RenameTempDirectories()
{
	// Only rename the temp mwmedia directory if it is there and it contains a service window XML
	if ( CUtilities::FileExists( GetWorkingPath(DIRECTORY_MWMEDIATEMP) + MEDIA_WINDOW_SHELL_XML ) )
	{
		string customMWMediaPath  = string(SentinelBasePath) + string(MWMediaCustomPath);
		// Delete the current mwmedia directory if there is one.
		CUtilities::DeleteDirectory(customMWMediaPath, true);
		// Rename the temp media window to be the current one.
		CUtilities::RenameFile(GetWorkingPath(DIRECTORY_MWMEDIATEMP), customMWMediaPath, NULL, true, true);
		CUtilities::FlushVolume();
	}

	// Only rename the temp lcdmedia directory if it is there and it contains a LCD window XML
	if ( CUtilities::FileExists( GetWorkingPath(DIRECTORY_LCDMEDIATEMP) + LCD_SHELL_XML ) )
	{
		string customLCDMediaPath = string(SentinelBasePath) + string(LCDMediaCustomPath);
		
		// Delete the current lcdmedia directory if there is one.
		CUtilities::DeleteDirectory( customLCDMediaPath, true );

		// Rename the temp lcdmedia to be the current one.
		CUtilities::RenameFile( GetWorkingPath(DIRECTORY_LCDMEDIATEMP), customLCDMediaPath, NULL, true, true );
		CUtilities::FlushVolume();
	}

	
	const string EMPTY_CERTIFICATE_STORE("0");
	string numberOfCertsInTemp(CUtilities::GetSystemResultString(FormatString("ls %s | wc -l", GetWorkingPath(DIRECTORY_ADDITIONAL_CERTIFICATE_STORETEMP).c_str())));
	if (EMPTY_CERTIFICATE_STORE != numberOfCertsInTemp)
	{
		//Delete the current certificate store directory if there is one.
		CUtilities::DeleteDirectory(GetWorkingPath(DIRECTORY_ADDITIONAL_CERTIFICATE_STORE), true);

		// Rename the temp certs store to be the current one.
		CUtilities::RenameFile(GetWorkingPath(DIRECTORY_ADDITIONAL_CERTIFICATE_STORETEMP), GetWorkingPath(DIRECTORY_ADDITIONAL_CERTIFICATE_STORE), NULL, true, true);
		CUtilities::FlushVolume();
	}
}

bool CDirectoryManager::CheckDisk()
{
	//TODO-CODE CLEANUP - Revisit this function later.
	
	// Look for temp directories, if there see if files or directories exist inside temp folders.
	const int DIRCOUNT = 5;
	string directories[DIRCOUNT];
	directories[0] = GetWorkingPath(DIRECTORY_MWMEDIATEMP);
	directories[1] = GetWorkingPath(DIRECTORY_LCDMEDIATEMP);
	directories[2] = GetWorkingPath(DIRECTORY_PLCONTENTTEMP);
	directories[3] = GetWorkingPath(DIRECTORY_ONELINK_CONTENTTEMP);
	directories[4] = GetWorkingPath(DIRECTORY_ADDITIONAL_CERTIFICATE_STORETEMP);

	ChkDskHandler chkdsk(directories, DIRCOUNT);

	CUtilities::DeleteDirectory( GetWorkingPath(DIRECTORY_LCDMEDIATEMP), true );
	CUtilities::FlushVolume();

	CUtilities::DeleteDirectory( GetWorkingPath(DIRECTORY_MWMEDIATEMP), true );
	CUtilities::FlushVolume();

	CUtilities::DeleteDirectory( GetWorkingPath(DIRECTORY_PLCONTENTTEMP ), true);
	CUtilities::FlushVolume();

	CUtilities::DeleteDirectory(GetWorkingPath(DIRECTORY_ONELINK_CONTENTTEMP), true);
	CUtilities::FlushVolume();

	CUtilities::DeleteDirectory(GetWorkingPath(DIRECTORY_ADDITIONAL_CERTIFICATE_STORETEMP), true);
	CUtilities::FlushVolume();

	return true;
}

void CDirectoryManager::DeleteUnwantedDirectories()
{
	// Delete DIRECTORY_MWMEDIATEMP if it exists
	if (CUtilities::FileExists(GetWorkingPath(DIRECTORY_MWMEDIATEMP)))
	{
		// Delete the DIRECTORY_MWMEDIATEMP directory if there is one.
		CUtilities::DeleteDirectory(GetWorkingPath(DIRECTORY_MWMEDIATEMP), true);
	}

	// Delete DIRECTORY_LCDMEDIATEMP if it exists
	if (CUtilities::FileExists(GetWorkingPath(DIRECTORY_LCDMEDIATEMP)))
	{
		// Delete the DIRECTORY_LCDMEDIATEMP directory if there is one.
		CUtilities::DeleteDirectory(GetWorkingPath(DIRECTORY_LCDMEDIATEMP), true);
	}

	// Delete DIRECTORY_PLCONTENTTEMP if it exists
	if (CUtilities::FileExists(GetWorkingPath(DIRECTORY_PLCONTENTTEMP)))
	{
		// Delete the DIRECTORY_PLCONTENTTEMP directory if there is one.
		CUtilities::DeleteDirectory(GetWorkingPath(DIRECTORY_PLCONTENTTEMP), true);
	}

	// Delete DIRECTORY_ONELINK_CONTENTTEMP if it exists
	if (CUtilities::FileExists(GetWorkingPath(DIRECTORY_ONELINK_CONTENTTEMP)))
	{
		// Delete the DIRECTORY_ONELINK_CONTENTTEMP directory if there is one.
		CUtilities::DeleteDirectory(GetWorkingPath(DIRECTORY_ONELINK_CONTENTTEMP), true);
	}

	// Delete DIRECTORY_ADDITIONAL_CERTIFICATE_STORETEMP if it exists
	if (CUtilities::FileExists(GetWorkingPath(DIRECTORY_ADDITIONAL_CERTIFICATE_STORETEMP)))
	{
		// Delete the DIRECTORY_ADDITIONAL_CERTIFICATE_STORETEMP directory if there is one.
		CUtilities::DeleteDirectory(GetWorkingPath(DIRECTORY_ADDITIONAL_CERTIFICATE_STORETEMP), true);
	}
}

void CDirectoryManager::CreateDirectories()
{
	for (int createIndex = 0; createIndex < NumDirectoriesToCreate; createIndex++)
	{
		string directoryPathToCreate = GetWorkingPath(DirectoryToCreate[createIndex]);
		if (!directoryPathToCreate.empty())
		{
			bool dirCreateRetVal = FileSys::CreateDirectory(directoryPathToCreate);
			if (dirCreateRetVal)
			{
				//LogString("DISK", "Created directory: %s", directoryPathToCreate.c_str());
			}
			else
			{
				LogString(ERROR_LOG, "Could not create directory %s", directoryPathToCreate.c_str());
			}
		}
	}
}

std::string CDirectoryManager::GetWorkingPath( SentinelDirectoryType directoryType )
{
	string path;
	
	switch (directoryType)
	{
	case DIRECTORY_OS:
		path = string(OSPath);
		break;
	case DIRECTORY_BASE:
		path = string(SentinelBasePath);
		break;
	case DIRECTORY_APPLICATIONS:
		path = string(SentinelBasePath) + string(ApplicationPath);
		break;
	case DIRECTORY_COMPACTMEDIA:
		path = m_workingCompactMediaPath;
		break;
	case DIRECTORY_LCDMEDIA:
		path = m_workingLCDMediaPath;
		break;
	case DIRECTORY_LCDMEDIATEMP:
		path = string(SentinelBasePath) + string(LCDMediaTempPath);
		break;
	case DIRECTORY_MWMEDIA:
		path = m_workingMWMediaPath;
		break;
	case DIRECTORY_MWMEDIATEMP:
		path = string(SentinelBasePath) + string(MWMediaTempPath);
		break;
    case DIRECTORY_UPGRADE:
        path = CUtilities::DoesDirExist(FIRMWAREPATH_LITE) ? string(UPDATEPATH_LITE) : string(UPDATEPATH);
		break;
	case DIRECTORY_ERROR_FILE:
		path = string(SentinelBasePath) + string(ErrorPath);
		break;
	case DIRECTORY_PLCONTENT:
		#ifdef _UNIT_TEST
				path = m_mockContentPath;
		#else
				path = string(SentinelBasePath) + string(PLContentPath);
		#endif
				break;
			case DIRECTORY_PLCONTENTTEMP:
		#ifdef _UNIT_TEST
				path = m_mockContentTmpPath;
		#else
				path = string(SentinelBasePath) + string(PLContentTempPath);
		#endif
		break;
	case DIRECTORY_MEDIADISK:
		path = string(ContentDrivePath);
		break; 
	case DIRECTORY_URL_COMPACT:
		path = string(BASEURL) + m_compactMediaPathChunk;
		break;
	case DIRECTORY_URL_LCD:
		path = string(BASEURL) + m_lcdMediaPathChunk;
		break;
	case DIRECTORY_URL_MW:
		path = string(BASEURL) + m_mwMediaPathChunk;
		break;
	case DIRECTORY_CUSTOM:
		path = string(ContentDrivePath) + CUSTOMMEDIA;
		break;
	case DIRECTORY_URL_ADLOOP:
		path = string(BASEURL) + AdLoop;
		break;
	case DIRECTORY_ONELINK_CONTENT:
		path = string(SentinelBasePath) + string(OneLinkContentPath);
		break;		
	case DIRECTORY_ONELINK_CONTENTTEMP:
#ifdef _UNIT_TEST
		path = m_mockContentTmpPath;
#else
		path = string(SentinelBasePath) + string(OneLinkContentTempPath);
#endif
		break;
	case DIRECTORY_DYNAMIC_CONTENT:
		path = string(SentinelBasePath) + string(DynamicContentPath);
		break;
	case DIRECTORY_ASSETS:
		path = string(SentinelBasePath) + string(AssetsPath);
		break;
	case DIRECTORY_CUSTOM_ASSETS:
		path = string(SentinelBasePath) + string(CustomAssetsPath);
		break;
	case DIRECTORY_CUSTOM_DYNAMIC_CONTENT:
		path = string(SentinelBasePath) + string(CustomDynamicContentPath);
		break;
	case DIRECTORY_URL_ONELINK_CONTENT:
		path = string(BASEURL) + string(OneLinkContentPathUrl);
		break;
	case DIRECTORY_DEFAULT_CERTIFICATE_STORE:
		path = string(SentinelBasePath) + string(DefaultCertsPath);
		break;
	case DIRECTORY_ADDITIONAL_CERTIFICATE_STORE:		
		path = string(SentinelBasePath) + string(AdditonalCertsPath);
		break;
	case DIRECTORY_ADDITIONAL_CERTIFICATE_STORETEMP:
		path = string(SentinelBasePath) + string(AdditonalCertsTempPath);
		break;
	case DIRECTORY_SENTINELDATA:
	default:
		path = string(SentinelBasePath) + string(SentinelDataPath);
		break;
	}
	
	return path;
}

std::string CDirectoryManager::GetCustomPath(SentinelDirectoryType dirtype)
{
	string path = SentinelBasePath;

	switch (dirtype)
	{
	case DIRECTORY_COMPACTMEDIA:
		path.append(CompactMediaCustomPath);
		break;
	case DIRECTORY_LCDMEDIA:
		path.append(LCDMediaCustomPath);
		break;
	case DIRECTORY_LCDMEDIATEMP:
		path.append(LCDMediaTempPath);
		break;
	case DIRECTORY_MWMEDIA:
		path.append(MWMediaCustomPath);
		break;
	case DIRECTORY_MWMEDIATEMP:
		path.append(MWMediaTempPath);
		break;
	case DIRECTORY_PLCONTENTTEMP:
		path.append(PLContentTempPath);
		break;
	case DIRECTORY_ADDITIONAL_CERTIFICATE_STORETEMP:
		path.append(AdditonalCertsTempPath);
		break;
	default:
		path = string("");
		break;
	}

	return path;
}

string CDirectoryManager::GetShellXmlPath(SentinelDirectoryType directoryType)
{
	string shellXmlFilePathPath;
	
	if (DIRECTORY_URL_COMPACT == directoryType ||
		DIRECTORY_COMPACTMEDIA == directoryType ||
		DIRECTORY_URL_LCD == directoryType ||
		DIRECTORY_LCDMEDIA == directoryType ||
		DIRECTORY_URL_MW == directoryType ||
		DIRECTORY_MWMEDIA == directoryType)
	{
		shellXmlFilePathPath = GetWorkingPath(directoryType);
		if (!shellXmlFilePathPath.empty())
		{
			if (DIRECTORY_URL_COMPACT == directoryType || DIRECTORY_COMPACTMEDIA == directoryType)
			{
				if (DIRECTORY_URL_COMPACT == directoryType)
				{
					shellXmlFilePathPath += CompactMedia;
				}				
				shellXmlFilePathPath += COMPACT_SHELL_XML;
			}
			else if (DIRECTORY_URL_LCD == directoryType || DIRECTORY_LCDMEDIA == directoryType)
			{
				if (DIRECTORY_URL_LCD == directoryType)
				{
					shellXmlFilePathPath += LCDMedia;
				}
				shellXmlFilePathPath += LCD_SHELL_XML;
			}
			else if (DIRECTORY_URL_MW == directoryType || DIRECTORY_MWMEDIA == directoryType)
			{
				if (DIRECTORY_URL_MW == directoryType)
				{
					shellXmlFilePathPath += MWMedia;
				}
				shellXmlFilePathPath += MEDIA_WINDOW_SHELL_XML;
			}
		}			
	}

	return shellXmlFilePathPath;
}

bool CDirectoryManager::OpenAndReadXmlFile(const std::string &drive, const std::string &fpgaXml) const
{
#ifdef _UNIT_TEST
	return m_openAndReadXmlFile;
#else
	bool bReadXmlFile(false);

	// found fpga upgrade xml file
	string xmlFileName;
	xmlFileName = FormatString("%s/%s", drive.c_str(), fpgaXml.c_str());
	ifstream xmlFile(xmlFileName.c_str());
	if (xmlFile)
	{
		bReadXmlFile = true;
		xmlFile.close();
	}

	return bReadXmlFile;
#endif
}

std::string CDirectoryManager::CallstackFilePath()
{
#ifndef _UNIT_TEST
	return string(ErrorFilePath);
#else
	return UTErrorFilePath; 
#endif
}

std::string CDirectoryManager::RebootDiagnosticsFilePath()
{
	return string(ErrorFilePath);
}

std::string CDirectoryManager::PBTTicketDiagnosticsFilePath()
{
	return string(ErrorFilePath);
}

std::string CDirectoryManager::PollerBadMessageDiagnosticsTextFilePath()
{
	return string(ErrorFilePath);
}

std::string CDirectoryManager::PollerBadMessageDiagnosticsLogFilePath()
{
	return string(BadPollerFilePath);
}

std::string CDirectoryManager::NVRAMBadSprinklesFilePath()
{
	return string(ErrorFilePath);
}

std::string CDirectoryManager::EepromBinFilePath()
{
	string eepromPathString = GetWorkingPath(DIRECTORY_SENTINELDATA) + string(EepromBinFilename);
	
	return eepromPathString;
}

std::string CDirectoryManager::VideoMixerDiagnosticsInternalFilePath()
{
    string vmDiagPathString = GetWorkingPath(DIRECTORY_SENTINELDATA) + string(VideoMixerDiagnosticsFile);

    return vmDiagPathString;
}

std::string CDirectoryManager::VideoMixerDiagnosticsUSBFilePath(int usbIndex)
{
	string vmDiagPathString;
	
	if (usbIndex < (int)m_usbPathArray.size())
	{
		vmDiagPathString = m_usbPathArray[usbIndex] + string(VideoMixerDiagnosticsFile);
	}
	
	return vmDiagPathString;
}

std::string CDirectoryManager::HtmlFilePath()
{
	return HTML_FILE_DRIVE_AND_PATH;
}

std::string CDirectoryManager::HttpdPathFilename()
{
	return HTTPD_PATH_FILENAME;
}

string CDirectoryManager::USBPath(int usbIndex)
{
	string usbPath;
	
	if (usbIndex < (int)m_usbPathArray.size())
	{
		usbPath = m_usbPathArray[usbIndex];
	}
	
	return usbPath;
}

void CDirectoryManager::MockSetPath(SentinelDirectoryType dirType, const std::string& pDrive, const std::string& pPath)
{
	if (m_instance)
		m_instance->_MockSetPath(dirType, pDrive, pPath);
}

void CDirectoryManager::_MockSetPath(SentinelDirectoryType dirType, const std::string& pDrive, const std::string& pPath)
{
#ifndef _UNIT_TEST
	UNREFERENCED_PARAMETER(&pDrive);
	UNREFERENCED_PARAMETER(&pPath);
#endif

	switch (dirType) {
	case DIRECTORY_PLCONTENT:
#ifdef _UNIT_TEST
		m_mockContentPath = pPath;
#endif
		break;
	case DIRECTORY_PLCONTENTTEMP:
#ifdef _UNIT_TEST
		m_mockContentTmpPath = pPath;
#endif
		break;
	default:
		break;
	}
}

void CDirectoryManager::MockResetPath(SentinelDirectoryType dirType)
{
	if (m_instance)
		m_instance->_MockResetPath(dirType);
}

void CDirectoryManager::_MockResetPath(SentinelDirectoryType dirType)
{
    UNREFERENCED_PARAMETER(&dirType); 
}

std::string CDirectoryManager::GetUpgradeUndoPath()
{
    return CUtilities::DoesDirExist(FIRMWAREPATH_LITE) ? string(UPGRADEUNDOPATH_LITE) : string(UPGRADEUNDOPATH);
}

std::string CDirectoryManager::GetRADBeaconPath()
{
	return RADBEACON_DEVICE_PATTERN;
}

std::string CDirectoryManager::GetRBCOMPath()
{
	return RBCOMpath;
}

std::string CDirectoryManager::GetContentUrlPath(DisplayPlatform platform)
{
	string contentPath(GetWorkingPath(GetWorkingDirectoryType(platform)));

	switch (platform)
	{
	case CompactPlatform:
		contentPath.append(CompactMedia);
		break;
	case LcdPlatform:
		contentPath.append(LCDMedia);
		break;
	case VideoMixerPlatform:
		contentPath.append(MWMedia);
		break;
	default:
		break;
	}
	contentPath.append("/");

	return contentPath;
}

SentinelDirectoryType CDirectoryManager::GetWorkingDirectoryType(DisplayPlatform platform)
{
	SentinelDirectoryType directoryType(DIRECTORY_URL_LCD);

	switch (platform)
	{
	case CompactPlatform:
		directoryType = DIRECTORY_URL_COMPACT;
		break;
	case LcdPlatform:
		directoryType = DIRECTORY_URL_LCD;
		break;
	case VideoMixerPlatform:
		directoryType = DIRECTORY_URL_MW;
		break;
	default:
		break;
	}

	return directoryType;
}