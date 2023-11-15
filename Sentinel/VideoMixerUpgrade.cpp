#include "stdafx.h"
#include "VideoMixerUpgrade.h"

// Sentinel includes.
#include "Hardware/WatchDog.h"
#include "Sentinel.h"
#include "Utilities.h"
#include "axhybrid.h"

// Third-party includes.
//#include "s5dll_api.h"

const std::string FPGADIRCHUNK = "/FPGA/";
const std::string FPGAXML = "FPGA.xml";

// time to allow VM to reset after rollback
const DWORD VMRESETTIME = 15000;
const DWORD VMMAXWAITTIME = 60000;
const UCHAR VEB_VERSION_MASK = 0x7F;
bool icompare_pred(unsigned char a, unsigned char b);
bool icompare(std::string const& a, std::string const& b);

CVideoMixerUpgrade::CVideoMixerUpgrade( )
{
	m_locationType = LocationNone;
	m_type = TypeNone;
	m_drive = "";
	Init("", LocationNone);
}

CVideoMixerUpgrade::CVideoMixerUpgrade(const string& path, FPGAUpgradeLocationType locationType)
{
	Init(path, locationType);
}

void CVideoMixerUpgrade::Init(const string& path, FPGAUpgradeLocationType locationType)
{
	lock_guard<recursive_mutex> lock(m_cs);
	
	m_locationType = locationType;
	m_type = VideoMixerType;
	switch (locationType)
	{
	case DownloadLocation:
		m_drive = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_MEDIADISK);
		break;
	case ExternalLocation:
		{
			m_drive = path;
		}
		break;
	default:
		m_drive = "";
		break;
	}
	
	struct axvm_version vmVersion;
	int vmResult = AXVM_GetFirmwareVersion(&vmVersion);
	if (vmResult != VM_Success)
	{
		m_currentVersionNumber.SetVersion("0", "0", "0");
	}
	else
	{
		m_currentVersionNumber.SetVersion(vmVersion.core_major, vmVersion.core_minor, vmVersion.core_build);
	}


	// Parse xml to get upgrade version info
	string major;
	string minor;
	string build;
	string filePath = m_drive + FPGADIRCHUNK + FPGAXML;
	
	pugi::xml_document xmlDoc;
	if (xmlDoc.load_file(filePath.c_str()))
	{
		pugi::xml_node vmNode = xmlDoc.child("FPGA").child("VideoMixerFiles").child("VideoMixerFile");

		if (vmNode != nullptr)
		{
			m_fpgaFilePath = m_drive + FPGADIRCHUNK + vmNode.attribute("filename").as_string();
			m_fpgaFileCRC = vmNode.attribute("CRC").as_int();

			// Check for Braemac names
			if (vmNode.attribute("EP3SLFPGAVersion") != nullptr)
			{
				major = vmNode.attribute("EP3SLFPGAVersion").as_string();
				minor = vmNode.attribute("EP4CFPGAVersion").as_string();
				build = vmNode.attribute("EP4CNIOSVersion").as_string();
			}
			else
			{
				// Use major/minor/build
				major = vmNode.attribute("major").as_string();
				minor = vmNode.attribute("minor").as_string();
				build = vmNode.attribute("build").as_string();
			}
		}
		m_upgradeVersionNumber.SetVersion(major, minor, build);
	}
}

CVideoMixerUpgrade::~CVideoMixerUpgrade(void)
{
}

string CVideoMixerUpgrade::GetCurrentVersionString()
{
	return m_currentVersionNumber.GetVersionString();
}

string CVideoMixerUpgrade::GetUpgradeVersionString()
{
	return m_upgradeVersionNumber.GetUpgradeVersionString();
}

bool CVideoMixerUpgrade::IsCurrent()
{
	bool bCurrent = true;
	
	if (!m_upgradeVersionNumber.GetUpgradeVersionString().empty() && m_currentVersionNumber.GetVersionString() != "0.0.0" && !CUtilities::StringCompareIgnoreCase(m_upgradeVersionNumber.GetUpgradeVersionString(), m_currentVersionNumber.GetVersionString()))
		bCurrent = false;

	return bCurrent;
}

bool CVideoMixerUpgrade::Upgrade()
{
	lock_guard<recursive_mutex> lock(m_cs);

	LogString(FPGA_LOG, "Video mixer firmware upgrade started.");

	// upgrade video mixer
	bool upgradePerformed = false;

	if (!m_fpgaFilePath.empty())
	{
		// Verify 
		
		FILE *file = fopen(m_fpgaFilePath.c_str(), "r");
		if (file != nullptr)
		{
			std::ifstream::pos_type filesize;
			std::ifstream in(m_fpgaFilePath.c_str(), std::ifstream::ate | std::ifstream::binary);
			filesize = in.tellg();
			
			byte *buffer = new byte[filesize];
			in.seekg(0, ios_base::beg);
			
			in.read((char *)buffer, filesize);
			in.close();
			
			unsigned short calcedCRC = CUtilities::CRC(buffer, filesize, 0xffff);
			if (calcedCRC == m_fpgaFileCRC)
			{
				int32_t result = AXVM_UpdateFirmware(m_fpgaFilePath.c_str(), nullptr);
				if (result == VM_Success)
				{
					upgradePerformed = true;
					LogString(FPGA_LOG, "Video mixer firmware upgraded successfully.");
				}
				else
				{
					LogString(FPGA_LOG, "Error upgrading video mixer firmware, error code = %d.", result);
				}
			
			}

            fclose(file);
		}
	}

	return upgradePerformed;
}

