#include "stdafx.h"

#include "Platform.h"
#include "ProcessDump/ProcessHandler.h"
#include "Logging/LogString.h"
#include "CarrierUpgrade.h"
#include "VideoMixerUpgrade.h"

using namespace std;

CPlatform *CPlatform::m_instance = NULL;
std::recursive_mutex CPlatform::m_instanceCS;
const string SILICON_LAB("Silicon LAB");
const LPCTSTR UPGRADEFILE_Label = "UpgradeFile";
const LPCTSTR PLATFORMID_LABEL = "PlatformID";
const LPCTSTR DEFAULT_OSIMAGEVERSION = "";
const LPCTSTR NEED_PLATFORMNAME = "need-to-get";
const LPCTSTR NEED_CPUTEMP = "need-to-get";
const LPCTSTR UNKNOWN_PLATFORMNAME = "unknown";
const LPCTSTR UNKNOWN_CPUTEMP = "unknown";
const LPCTSTR DEFAULT_UPGRADESMDFILENAME = "S5Upgrade.smd";
const LPCTSTR UNKNOWN_TOUCH_SCREEN = "unknown";
const UINT DEFAULT_PLATFORMID = 2;
const UINT CPlatform::AXIOMTEK_PLATFORMID = 3;
const UINT CPlatform::BRAEMAC_BLUE_PLATFORM_ID = 4;

CPlatform & CPlatform::Instance()
{
	lock_guard<recursive_mutex> lock(m_instanceCS);
	if (m_instance == NULL)
	{
		m_instance = new CPlatform();
	}

	return *m_instance;
}

void CPlatform::CleanupInstance()
{
	lock_guard<recursive_mutex> lock(m_instanceCS);
	if (NULL != m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}


CPlatform & CPlatform::MockInstance(LPCTSTR osVersion, UINT platformID, LPCTSTR platformName, LPCTSTR upgradeSMDFilename)
{
	lock_guard<recursive_mutex> lock(m_instanceCS);
	if (m_instance == NULL)
	{
		m_instance = new CPlatform(osVersion, platformID, platformName, upgradeSMDFilename);
	}

	return *m_instance;
}


CPlatform::CPlatform() : m_directoryManager(CDirectoryManager::Instance())
{
    m_osImageVersionNumber = DEFAULT_OSIMAGEVERSION;
    m_platformID = DEFAULT_PLATFORMID;
    m_platformName = NEED_PLATFORMNAME;
    m_CPUTemp = NEED_CPUTEMP;

    //This will set up Platform and OS Image version values 
	GetOSImageInformation();

	// Initialize upgrade SMD name based on the current OS variant.
	// Version 2.x belongs to LITE devices running i386 OS and 
	// 3.x will belong to PT316 running amd64 OS
	if (0 == m_osImageVersionNumber.find("2."))
	{
		m_upgradeSMDfilename = "S5Upgrade-i386.smd";
	}
	else if (0 == m_osImageVersionNumber.find("3."))
	{
		m_upgradeSMDfilename = "S5Upgrade-amd64.smd";
	}
	else
	{
		m_upgradeSMDfilename = DEFAULT_UPGRADESMDFILENAME;
	}

    //This sets up the array of services needed by the sentinel.exe for this OS
	WaitForNeededServices();
}

CPlatform::CPlatform(LPCTSTR osVersion, UINT platformID, LPCTSTR platformName, LPCTSTR upgradeSMDFilename)
	: m_directoryManager(CDirectoryManager::MockInstance())
{
    m_osImageVersionNumber = osVersion;
    m_platformID = platformID;
    m_platformName = platformName;
    m_upgradeSMDfilename = upgradeSMDFilename;
}


CPlatform::~CPlatform()
{
}

const std::string & CPlatform::GetPlatformName()
{
	if (m_platformName == NEED_PLATFORMNAME)
	{
		const char *platform = std::getenv("PLATFORM");
		m_platformName = platform ? platform : UNKNOWN_PLATFORMNAME;
		cout << "PLATFORM: " << m_platformName << "\n";
	}
	return m_platformName;
}

bool CPlatform::IsLiteDevice() const
{
	return PLATFORM_LITE == m_platformName;
}

bool CPlatform::IsNexusDevice() const
{
	return PLATFORM_NEXUS == m_platformName;
}

bool CPlatform::PlatformUsesOSUpgradeFlag() const
{
	return (GetPlatformID() == AXIOMTEK_PLATFORMID ||
		GetPlatformID() == BRAEMAC_BLUE_PLATFORM_ID);
}

void CPlatform::GetOSImageInformation()
{
	FILE* file = nullptr;
	const string SSH_CONNECTION("SSH_CONNECTION=");

	const string AXPT("AXPT-");
	const string axpt_rootsys("axpt_rootsys=");
	const string axpt_rootsys2("axpt_rootsys: ");
	file = popen("axpt_versions", "r");
	if (file)
	{
		std::ostringstream stm;

		constexpr std::size_t MAX_LINE_SZ = 1024;
		char buffer[MAX_LINE_SZ];

		while (fgets(buffer, sizeof(buffer), file) != nullptr)
		{
			string line(buffer);

			size_t osVersionPos(line.find(axpt_rootsys));

			if (std::string::npos != osVersionPos)
			{
				line.erase(0, axpt_rootsys.length());
				m_osImageVersionNumber = line;
				m_osImageVersionNumber.erase(m_osImageVersionNumber.length() - 1, 1);
				m_osImageVersionNumber.erase(0, AXPT.size());
			}
			else {
				osVersionPos = line.find(axpt_rootsys2);
				if (std::string::npos != osVersionPos)
				{
					line.erase(0, axpt_rootsys2.length());
					m_osImageVersionNumber = line;
				}
			}

			m_osImageVersionNumber.erase(std::find_if(m_osImageVersionNumber.rbegin(), m_osImageVersionNumber.rend(), [](unsigned char ch) {
					return (!std::isspace(ch) && !std::iscntrl(ch));
					}).base(), m_osImageVersionNumber.end());
			
		}
		pclose(file);
	}

	cout << axpt_rootsys << m_osImageVersionNumber << endl;

	if (CUtilities::FileExists(CDirectoryManager::REMOTE_DEBUG_FILE))
	{
		string sshConnectionInfo;
		file = popen("env", "r");
		if (file)
		{
			std::ostringstream stm;

			constexpr std::size_t MAX_LINE_SZ = 1024;
			char buffer[MAX_LINE_SZ];

			while (fgets(buffer, sizeof(buffer), file) != nullptr)
			{
				string line(buffer);
				size_t sshPos(line.find(SSH_CONNECTION));
				if (std::string::npos != sshPos)
				{
					line.erase(0, SSH_CONNECTION.length());
					sshConnectionInfo = line;
					sshConnectionInfo.erase(sshConnectionInfo.length() - 1, 1);
					break;
				}
			}
			pclose(file);
		}
		cout << SSH_CONNECTION << sshConnectionInfo << endl;
	}
}

void CPlatform::GetHardwareComponents(string& ethernetType, string& mcuType, string& touchscreenType)
{
	if (m_ethernetType.empty() || m_mcuType.empty() || m_touchscreenType.empty())
	{
		if (IsLiteDevice())
		{
			m_ethernetType = "LITE";
			m_mcuType = "LITE";
			m_touchscreenType = "LITE";
		}
		else if (IsNexusDevice()) 
		{
			m_ethernetType = "NEXUS";
			m_mcuType = "NEXUS";
			m_touchscreenType = "NEXUS";
		}
		else
		{
			m_ethernetType = CUtilities::GetSystemResultString("/usr/sbin/i2cget -y -f 0x02 0x51 0x0a");
			if ("0x01" == m_ethernetType)
			{
				m_ethernetType = "RTL8211e";
			}
			else if ("0xff" == m_ethernetType)
			{
				m_ethernetType = "AR8031";
			}
			if (m_ethernetType.empty())
			{
				m_ethernetType = "unknown";
			}

			m_mcuType = CUtilities::GetSystemResultString("/usr/sbin/i2cget -y -f 0x02 0x51 0x0b");
			if ("0x01" == m_mcuType)
			{
				m_mcuType = "F446";
			}
			else if ("0xff" == m_mcuType)
			{
				m_mcuType = "F103";
			}
			if (m_mcuType.empty())
			{
				m_mcuType = "unknown";
			}

			m_touchscreenType = CUtilities::GetSystemResultString("/usr/sbin/i2cget -y -f 0x02 0x51 0x09");
			if ("0x01" == m_touchscreenType)
			{
				m_touchscreenType = SILICON_LAB;
			}
			else if ("0xff" == m_touchscreenType)
			{
				m_touchscreenType = "Microchip AR1011";
			}
			if (m_touchscreenType.empty())
			{
				m_touchscreenType = UNKNOWN_TOUCH_SCREEN;
			}
		}
	}

	ethernetType = m_ethernetType;
	mcuType = m_mcuType;
	touchscreenType = m_touchscreenType;
}

bool CPlatform::CheckIfTouchScreenWorksWithTheCurrentOS()
{
	bool touchscreenWithCurrentOS(true);

	if (m_touchscreenType == SILICON_LAB)
	{
		string osVersion(m_osImageVersionNumber);
		osVersion.erase(std::remove(osVersion.begin(), osVersion.end(), '.'), osVersion.end());
		cout << "CheckIfTouchScreenWorksWithTheCurrentOS=" << osVersion << endl;
		try
		{
			const DWORD MINIMUM_OS_VERSION(11112);
			DWORD osVersionNumber(stoul(osVersion, nullptr, 10));
			cout << "CheckIfTouchScreenWorksWithTheCurrentOS=" << osVersionNumber << endl;
			if (osVersionNumber < MINIMUM_OS_VERSION)
			{				
				LogString(GENERAL, "%s", FormatString("CheckIfTouchScreenWorksWithTheCurrentOS touchscreen=%s is incompatible with osVersion=%s", SILICON_LAB.c_str(), osVersion.c_str()).c_str());
				touchscreenWithCurrentOS = false;
			}
		}
		catch (const std::invalid_argument& e)
		{
			LogString(ERROR_LOG, "%s", FormatString("CheckIfTouchScreenWorksWithTheCurrentOS invalid_argument osVersion=%s", osVersion.c_str()).c_str());
		}
		catch (const std::out_of_range& e)
		{
			LogString(ERROR_LOG, "%s", FormatString("CheckIfTouchScreenWorksWithTheCurrentOS out_of_range osVersion=%s", osVersion.c_str()).c_str());
		}
		catch (const std::exception& e)
		{
			LogString(ERROR_LOG, "%s", FormatString("CheckIfTouchScreenWorksWithTheCurrentOS exception=%s osVersion=%s", e.what(), osVersion.c_str()).c_str());
		}
	}
	return touchscreenWithCurrentOS;
}

void CPlatform::WaitForNeededServices()
{
}

long CPlatform::GetCPUTemperature()
{
	const string CmdLineFile = "cat /sys/devices/virtual/thermal/thermal_zone0/temp 2>/dev/null";

	m_CPUTemp = UNKNOWN_CPUTEMP;
	long lCPUTemp = 0.0;

    m_CPUTemp = CUtilities::GetSystemResultString(CmdLineFile);

    if (m_CPUTemp.empty())
    {
        m_CPUTemp = UNKNOWN_CPUTEMP;
    }
	else
	{
		try
		{
			lCPUTemp = stol(m_CPUTemp);
		}
		catch (...)
		{
			LogString(ERROR_LOG, "CPlatform - GetCPUTemperature CPU Temperature (Celsius): %s", UNKNOWN_CPUTEMP);
		}
	}
	return lCPUTemp;
}

std::string CPlatform::GetResolution()
{
	if (m_resolution.empty())
	{
		if ((PLATFORM_PT == m_platformName) || (PLATFORM_PT_306 == m_platformName))
		{
			m_resolution = CUtilities::GetSystemResultString("/usr/sbin/fbset | awk '/geometry/ {print $2 \"x\" $3}'");
		}
		else if (PLATFORM_LITE == m_platformName)
		{
			m_resolution = CUtilities::GetSystemResultString("/usr/bin/xrandr 2>/dev/null | awk '/current/ {gsub(/,/, ""); print $8 \"x\" $10}'");
		}
		else if (PLATFORM_NEXUS == m_platformName)
		{
			m_resolution = CUtilities::GetSystemResultString("/usr/bin/xrandr | awk '/current/ {gsub(/,/, ""); print $8 \"x\" $10}'");
		}
	}

	return m_resolution;
}

bool CPlatform::GetLandscape()
{
	bool landscape(false);//TODO when will this be false

	if ((PLATFORM_PT == m_platformName) || (PLATFORM_PT_306 == m_platformName))
	{
		landscape = true;
	}
	else if (PLATFORM_LITE == m_platformName)
	{
		landscape = true;
	}
	else if (PLATFORM_NEXUS == m_platformName)
	{
		landscape = true;
	}

	return landscape;
}

bool CPlatform::GetTouchScreen()
{
	if (m_touchscreenType.empty())
	{
		string ethernetType, mcuType, touchscreenType;
		CPlatform::Instance().GetHardwareComponents(ethernetType, mcuType, touchscreenType);
	}

	return m_touchscreenType != UNKNOWN_TOUCH_SCREEN;
}

string CPlatform::GetProcessorType()
{
	if (m_processorType.empty())
	{
		m_processorType = CUtilities::GetSystemResultString("awk -F '[ \\t]+:[ \\t]+' '$1 == \"model name\" { print $2 }' /proc/cpuinfo | sort -u");
		if (m_processorType.empty()) 
		{
			m_processorType = "unknown";
		}
	}

	return m_processorType;
}

std::string CPlatform::GetNumberOfCores()
{
	if (m_numberOfCores.empty())
	{
		m_numberOfCores = CUtilities::GetSystemResultString("grep -c ^processor /proc/cpuinfo");
		if (m_numberOfCores.empty())
		{
			m_numberOfCores = "unknown";
		}
	}

	return m_numberOfCores;
}

string CPlatform::GetBiosVersion()
{
	if (m_biosVersion.empty())
	{
		m_biosVersion = CUtilities::GetSystemResultString("cat /sys/class/dmi/id/bios_version");
		if (m_biosVersion.empty())
		{
			m_biosVersion = "unknown";
		}
	}

	return m_biosVersion;
}

std::string CPlatform::GetTotalMemoryInKB()
{
	if (m_totalMemoryInKB.empty())
	{
		m_totalMemoryInKB = CUtilities::GetSystemResultString("awk '/MemTotal/ {print $2}' /proc/meminfo");
		if (m_totalMemoryInKB.empty())
		{
			m_totalMemoryInKB = "unknown";
		}
	}

	return m_totalMemoryInKB;
}

std::string CPlatform::GetBeaconCapability()
{
	if (m_beaconCapability.empty())
	{
		m_beaconCapability = "Transmitter";//Transmitter/Receiver/Transreceiver
		if (m_beaconCapability.empty())
		{
			m_beaconCapability = "unknown";
		}
	}

	return m_beaconCapability;
}

std::string CPlatform::GetBeaconFirmware()
{
	if (m_beaconFirmware.empty())
	{
		m_beaconFirmware = "0.0.0";//TODO
		if (m_beaconFirmware.empty())
		{
			m_beaconFirmware = "unknown";
		}
	}

	return m_beaconFirmware;
}

std::string CPlatform::GetCarrierFirmwareVersion()
{
	if (m_carrierFirmwareVersion.empty())
	{
		CCarrierUpgrade carrier;
		m_carrierFirmwareVersion = carrier.GetCurrentVersionString();
		if (m_carrierFirmwareVersion.empty())
		{
			m_carrierFirmwareVersion = "unknown";
		}
	}

	return m_carrierFirmwareVersion;		
}

std::string CPlatform::GetVideoMixerFirmwareVersion()
{
	if (m_videoMixerFirmwareVersion.empty())
	{
		CVideoMixerUpgrade vm;
		m_videoMixerFirmwareVersion = vm.GetCurrentVersionString();
		if (m_videoMixerFirmwareVersion.empty())
		{
			m_videoMixerFirmwareVersion = "unknown";
		}
	}

	return m_videoMixerFirmwareVersion;
}