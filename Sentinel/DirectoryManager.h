#pragma once

#define DIRECTORY_MANAGER_H

#include "Utilities.h"
#include "ChkDskHandler.h"

typedef enum
{
	DIRECTORY_OS = 0,					// The OS
	DIRECTORY_BASE,						// The Sentinel base path
	DIRECTORY_APPLICATIONS,				// The sentinel applications
	DIRECTORY_MWMEDIA,					// The media window (service window) graphics
	DIRECTORY_MWMEDIATEMP,				// The media window (service window) graphics temp directory (skinning downloads)
	DIRECTORY_UPGRADE,					// Upgrade
	DIRECTORY_SENTINELDATA,				// Config files, misc downloaded files, EEPROM file, and reg filter files
	DIRECTORY_COMPACTMEDIA,				// The compact media graphics
	DIRECTORY_LCDMEDIA,					// The lcd media graphics
	DIRECTORY_LCDMEDIATEMP,				// The lcd media graphics temp directory (skinning downloads)
	DIRECTORY_ERROR_FILE,				// The Error file directory
	DIRECTORY_PLCONTENT,				// The playlist media
	DIRECTORY_PLCONTENTTEMP,			// The playlist temp download directory
	DIRECTORY_MEDIADISK,				// The location to media disk
	DIRECTORY_URL_COMPACT,				// The Compact url base path
	DIRECTORY_URL_LCD,					// The LCD url base path
	DIRECTORY_URL_MW,					// The MW url base path
	DIRECTORY_CUSTOM,					// /mediadisk/custom"
	DIRECTORY_URL_ADLOOP,				// The url for adloop path
	DIRECTORY_ONELINK_CONTENT,			// OneLink content
	DIRECTORY_ONELINK_CONTENTTEMP,		// OneLink content temp (used for downloads)
	DIRECTORY_DYNAMIC_CONTENT,			// Path where zip content get extracted to
	DIRECTORY_ASSETS,					// Path of assets
	DIRECTORY_CUSTOM_ASSETS,			// Path of custom assets
	DIRECTORY_CUSTOM_DYNAMIC_CONTENT,	// Path where campaigns will be downloaded
	DIRECTORY_URL_ONELINK_CONTENT,		// OneLink content URL
	DIRECTORY_DEFAULT_CERTIFICATE_STORE,// Default Certificate Store Path
	DIRECTORY_ADDITIONAL_CERTIFICATE_STORE,// Additional Certificate Store Path
	DIRECTORY_ADDITIONAL_CERTIFICATE_STORETEMP,// Additional Certificate Store Path
	NUM_OF_DIRECTORY_TYPES				// Keep last
} SentinelDirectoryType;

typedef enum
{
	DRIVETYPE_DEFAULT = 0,
	DRIVETYPE_CUSTOM,
} SentinelDriveType;

// Declare friend class(es) for unit test access.
namespace SentinelUnitTests { class DirectoryManagerUnitTests; }

// Singleton directory manager.
class CDirectoryManager
{
public:
	///	<summary>
	/// Creates a global instance of CDirectoryManager.
	///	</summary>
	///	<param name="full">If <b>true</b>, calls the FullSetup method.</param>
	///	<returns>A reference to the created CDirectoryManager instance.</returns>
	static CDirectoryManager & Instance();

	///	<summary>
	/// Creates a global instance of CDirectoryManager.
	///	</summary>
	///	<param name="sentinelDrives">An optional pointer to a file containing drive
	///		mappings.</param>
	///	<param name="reset">If <b>true</b>, the CDirectoryManager will always be
	///		deleted and	recreated. If <b>false</b>, the CDirectoryManager will be
	///		deleted and	recreated only if sentinelDrives is not NULL.</param>
	///	<returns>A reference to the created CDirectoryManager instance.</returns>
	static CDirectoryManager & MockInstance(std::ifstream *sentinelDrives = NULL, bool reset = false);

	///	<summary>
	/// Creates a global instance of CDirectoryManager.
	///	</summary>
	///	<param name="sentinelDrives">An option pointer to a file containing the drive
	///		mappings.</param>
	///	<returns>A reference to the created CDirectoryManager instance.</returns>
	/// <remarks>
	///	This calls MockInstance with the reset parameter set to <b>true</b>. It makes
	/// the intent in test modules much clearer.
	/// </remarks>
	static CDirectoryManager & MockInstanceReset(std::ifstream *sentinelDrives = nullptr);

	///	<summary>
	/// Deletes the global instance of CDirectoryManager.
	///	</summary>
	/// <remarks>
	///	This is used in testing where the paths in CDirectoryManager have been modified.
	/// Without this, other tests which call MockInstance with the defaults will use
	/// the modified instance.
	/// </remarks>
	static void MockInstanceCleanup();

	/// <summary>
	/// The Path of where callstacks to crashed or frozen threads can be written.
	/// </summary>
	/// <returns>'e:\\error.txt'.</returns>
	/// <remarks>
	/// The method exists so we can get the path string without allocating or modifying memory
	/// while the Sentinel.exe is in a compromised state.
	/// </remarks>
	static std::string CallstackFilePath();

	/// <summary>
	/// The Path of where Reboot Diagnostics can be written.
	/// </summary>
	/// <returns>'e:\\error.txt'.</returns>
	/// <remarks>
	/// Currently returns the same as CallstackFilePath, so exists if needs to be changed in the future.
	/// </remarks>
	static std::string RebootDiagnosticsFilePath();

	/// <summary>
	/// The Path of where PBT/Ticket Diagnostics can be written.
	/// </summary>
	/// <returns>Location of Error.txt file.</returns>
	/// <remarks>
	/// Currently returns the same as CallstackFilePath, so exists if needs to be changed in the future.
	/// </remarks>
	static std::string PBTTicketDiagnosticsFilePath();

	/// <summary>
	/// The path where Poller bad message error text diagnostics can be written.
	/// </summary>
	/// <returns>The location of the Error.txt file.</returns>
	/// <remarks>
	/// Currently returns the same as CallstackFilePath, so exists if needs to be changed in the future.
	/// </remarks>
	static std::string PollerBadMessageDiagnosticsTextFilePath();

	/// <summary>
	/// The path where Poller bad message binary diagnostics can be written.
	/// </summary>
	/// <returns>The location of the BADPOLLERDATA.sdf file.</returns>
	/// <remarks>
	/// Currently returns the same directory as CallstackFilePath, so exists if needs to be changed in the future.
	/// </remarks>
	static std::string PollerBadMessageDiagnosticsLogFilePath();

	/// <summary>
	/// The Path of where the bad sprinkle values can be written.
	/// </summary>
	/// <returns>The location of the Error.txt file.</returns>
	/// <remarks>
	/// The method exists so we can get the path string without allocating or modifying memory
	/// while the Sentinel.exe is in a compromised state.
	/// </remarks>
	static std::string NVRAMBadSprinklesFilePath();

	/// <summary>
	/// The path for the eeprom.bin file.
	/// </summary>
	/// <returns>The path for the eeprom.bin file.</returns>
	std::string EepromBinFilePath();

	/// <summary>
	/// The path for the internal video mixer diagnostics file.
	/// </summary>
    /// <returns>The path for the internal video mixer diagnostics 
    /// file.</returns> 
	std::string VideoMixerDiagnosticsInternalFilePath();

	/// <summary>
    /// The path for the video mixer diagnostics file for the
    /// specified usb index.
	/// </summary>
	///	<param name="usbIndex">The index of the usb drive</param>
    /// <returns>The path for the video mixer diagnostics 
    /// file for the specified usb index. Returns an empty string if
    /// no path for given index</returns>
	std::string VideoMixerDiagnosticsUSBFilePath(int usbIndex);

	/// <summary>
	/// The path where the httpd (HTTP Server) serves files from.
	/// </summary>
	/// <returns>The path where the httpd (HTTP Server) serves files from.</returns>
	static std::string HtmlFilePath();

	/// <summary>
	/// Gets the full path and filename for an HTTP daemon (Hypertext Transfer Protocol Server).
	/// </summary>
	/// <returns>The full path and filename for an HTTP daemon (Hypertext Transfer Protocol Server).</returns>
	static std::string HttpdPathFilename();
	
	/// <summary>
	/// Gets the path for the USB drive for the given index. There are several 
	/// options possible drives. 
	/// </summary> 
	///	<param name="usbIndex">The index of the usb drive</param>
    /// <returns>The path for the USB drive for the given index.
    /// Returns an empty string if no path for given index</returns>
	std::string USBPath(int usbIndex);

	// The working drive and path locations
	std::string GetWorkingPath( SentinelDirectoryType directoryType );

	/// <summary>
	/// When new skins are downloaded, they are put into the temp directories.
	/// When we come up, we need to rename these temp directories in order to use the newly downloaded skins.
	/// </summary>
	void RenameTempDirectories();

	/// <summary>
	/// Return path used for undo purposes during upgrade
	/// </summary>
	std::string GetUpgradeUndoPath();
	
	void MockSetup(std::ifstream *sentinelDrives);
	static void MockSetPath(SentinelDirectoryType dirType, const std::string& pDrive, const std::string& pPath);
	static void MockResetPath(SentinelDirectoryType dirType);

	void DetermineWorkingMediaPaths();
	std::string GetCustomPath(SentinelDirectoryType dirtype);

	/// <summary>
	/// Gets the Shell XML path
	/// </summary> 
	///	<param name="directoryType">Directory Type</param>
	/// <returns>shell xml path</returns>
	std::string GetShellXmlPath(SentinelDirectoryType directoryType);

	/// <summary>
	/// Return path for RADBeacon
	/// </summary>
	std::string GetRADBeaconPath();

	/// <summary>
	/// Return path for RBCOM binary.
	/// </summary>
	std::string GetRBCOMPath();

	/// <summary> 
	/// Gets the content URL path.
	/// </summary>
	///	<param name="platform">Display Platform</param>
	/// <returns>Content URL path</returns> 
	std::string GetContentUrlPath(DisplayPlatform platform);

public:	
	static LPCTSTR BASEURL;
	static LPCSTR USB_PATH_PREFIX;
	static LPCSTR SENTINEL_DATA_SD_PATH;
	static LPCSTR CONTENT_SD_PATH;
	static LPCSTR DIRECTORY_KERNEL_NET_ETH;
	static LPCTSTR CUSTOMMEDIA;
	static LPCTSTR HTML_FILE_DRIVE_AND_PATH;
	static LPCTSTR FIRMWAREPATH_LITE;
	static LPCTSTR CLEAR_MEMORY;
	static LPCTSTR AUTOMATION_MODE;
	static const std::string REMOTE_DEBUG_FILE;
	static const std::string REMOTE_UI_FILE;
private:
	static LPCTSTR OSPath;
	static LPCTSTR SentinelBasePath;
	static LPCTSTR ApplicationPath;
	static LPCTSTR CompactMediaDefaultPath;
	static LPCTSTR LCDMediaDefaultPath;
	static LPCTSTR OneLinkContentPathUrl;
	static LPCTSTR MWMediaDefaultPath;
	static LPCTSTR CompactMediaCustomPath;
	static LPCTSTR LCDMediaCustomPath;
	static LPCTSTR MWMediaCustomPath;
	static LPCTSTR CompactMediaTempPath;
	static LPCTSTR LCDMediaTempPath;
	static LPCTSTR MWMediaTempPath;
	static LPCTSTR PLContentTempPath;
	static LPCTSTR PLContentPath;
	static LPCTSTR DataDrivePath;
	static LPCTSTR SentinelDataPath;
	static LPCTSTR ErrorPath;
	static LPCTSTR ErrorFilePath;
	static LPCTSTR BadPollerFilePath;
	static LPCTSTR EepromBinFilename;
	static LPCTSTR VideoMixerDiagnosticsFile;
	static LPCTSTR ContentDrivePath;
	static LPCTSTR OneLinkContentPath;
	static LPCTSTR OneLinkContentTempPath;
	static LPCTSTR DynamicContentPath;
	static LPCTSTR AssetsPath;
	static LPCTSTR CustomAssetsPath;
	static LPCTSTR CustomDynamicContentPath;
	static LPCTSTR USBDrive1;
	static LPCTSTR USBDrive2;
	static LPCTSTR USBDrive3;
	static LPCTSTR USBDrive4;
	static LPCTSTR USBDrive5;
	static LPCTSTR USBDrive6;
	static LPCTSTR USBDrive7;
	static LPCTSTR USBDrive8;
	static LPCTSTR HTTPD_PATH_FILENAME;
	static LPCTSTR UPDATEPATH;
	static LPCTSTR UPGRADEUNDOPATH;
    static LPCTSTR UPDATEPATH_LITE;
    static LPCTSTR UPGRADEUNDOPATH_LITE;
	static LPCTSTR DEFAULTMEDIA;
	static LPCTSTR CompactMedia;
	static LPCTSTR LCDMedia;
	static LPCTSTR MWMedia;
	static LPCTSTR AdLoop;
	static LPCTSTR COMPACT_SHELL_XML;
	static LPCTSTR LCD_SHELL_XML;
	static LPCTSTR MEDIA_WINDOW_SHELL_XML;
	static LPCTSTR RADBEACON_DEVICE_PATTERN;
	static LPCTSTR RBCOMpath;
	static LPCTSTR DefaultCertsPath;
	static LPCTSTR AdditonalCertsPath;
	static LPCTSTR AdditonalCertsTempPath;
	
	static const std::string OrientationCompact;
	static const std::string OrientationLcd;
	static const std::string OrientationMediaWindowBottom;
	static const std::string OrientationMediaWindowSide;

	static const DWORD WAIT_FOR_DRIVE_TIMEOUT = 0;	// milliseconds
	static const DWORD WAIT_FOR_DRIVE_SLEEP = 0;	// milliseconds
	static const DWORD WAITFORDRIVEDELAY = 0;	// milliseconds
	static const int NumDirectoriesToCreate = 13;
	static const SentinelDirectoryType DirectoryToCreate[NumDirectoriesToCreate];

	CDirectoryManager();
	~CDirectoryManager();

	void FullSetup();

	void CreateDirectories();
	bool CheckDisk();
	void DeleteUnwantedDirectories();
	bool OpenAndReadXmlFile(const std::string &drive, const std::string &fpgaXml) const;
	SentinelDirectoryType GetWorkingDirectoryType(DisplayPlatform platform);

	// The singleton instance
	static CDirectoryManager *m_instance;
	static std::recursive_mutex m_instanceCS;
	
	std::string m_workingCompactMediaPath;
	std::string m_workingLCDMediaPath;
	std::string m_workingMWMediaPath;

	std::string m_tempCompactMediaPath;
	std::string m_tempLCDMediaPath;
	std::string m_tempMWMediaPath;
	
	std::vector<std::string> m_usbPathArray;
	bool m_fullSetupDone;

	void _MockSetPath(SentinelDirectoryType dirType, const std::string& pDrive, const std::string& pPath);
	void _MockResetPath(SentinelDirectoryType dirType);

	std::string m_compactMediaPathChunk;
	std::string m_lcdMediaPathChunk;
	std::string m_mwMediaPathChunk;

#ifdef _UNIT_TEST
	bool m_openAndReadXmlFile;
	std::string m_fpgaXmlDrive;

	bool m_isMockPath;
	std::string m_mockContentPath;
	std::string m_mockContentTmpPath;
	
#endif

	// Grant "friend" access for testing.
	friend class SentinelUnitTests::DirectoryManagerUnitTests;
	FRIEND_TEST(DirectoryManagerUnitTests, DirectoryManager_FullSetupSleep);
	FRIEND_TEST(DirectoryManagerUnitTests, DirectoryManager_FpgaUpdateAvailableTest);
	FRIEND_TEST(DirectoryManagerUnitTests, DirectoryManager_FpgaUpdateLocationIsOs);
};
