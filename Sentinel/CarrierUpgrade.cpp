#include "stdafx.h"
#include "CarrierUpgrade.h"

// Sentinel includes.
#include "DirectoryManager.h"
#include "Logging/LogString.h"
#include "Util/FileSys.h"
#include "Utilities.h"

using namespace std;

const std::string FPGADIRCHUNK = "/FPGA/";
const std::string FPGAXML = "FPGA.xml";
const size_t VERSIONSIZE = 15;
bool icompare_pred(unsigned char a, unsigned char b);
bool icompare(std::string const& a, std::string const& b);

CCarrierUpgrade::CCarrierUpgrade( )
{
	m_locationType = LocationNone;
	m_type = TypeNone;
	m_drive = "";
	Init("", LocationNone);
}

CCarrierUpgrade::CCarrierUpgrade(const string& path, FPGAUpgradeLocationType locationType)
{
	Init(path, locationType);
}

void CCarrierUpgrade::Init(const string &path, FPGAUpgradeLocationType locationType)
{
	m_locationType = locationType;
	m_type = CarrierType;

	switch (locationType)
	{
	case DownloadLocation:
		m_drive = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_MEDIADISK) + FPGADIRCHUNK;
		break;
	case ExternalLocation:
		{
			// Remove FPGA.xml from end of path
			int pos; 
			pos = path.find(FPGAXML);
			if (pos != -1)
			{
				m_drive = path.substr(pos);
			}
			else
			{
				m_drive = path + FPGADIRCHUNK;
			}
		}
		break;
	default:
		m_drive = "";
		break;
	}
	char version[VERSIONSIZE];
	size_t versionSize = VERSIONSIZE;
    S7LITE_Firmware_Version(version, &versionSize);
	m_currentVersionNumber = string(version);

	string filePath = m_drive + string("FPGA.xml");

	pugi::xml_document xmlDoc;
	if (xmlDoc.load_file(filePath.c_str()))
	{
		pugi::xml_node vmNode = xmlDoc.child("FPGA").child("CarrierFiles").child("CarrierFile");

		if (vmNode != nullptr)
		{
			m_fpgaFilePath = m_drive + vmNode.attribute("filename").as_string();
			m_fpgaFileCRC = vmNode.attribute("CRC").as_int();

			// Check for Braemac names
			if (vmNode.attribute("FPGAVersion") != nullptr)
			{
				m_upgradeVersionNumber = vmNode.attribute("FPGAVersion").as_string();
			}
		}
	}
}

CCarrierUpgrade::~CCarrierUpgrade(void)
{
}

std::string CCarrierUpgrade::GetCurrentVersionString()
{
	return m_currentVersionNumber;
}

std::string CCarrierUpgrade::GetUpgradeVersionString()
{
	return m_upgradeVersionNumber;
}

bool CCarrierUpgrade::IsCurrent()
{
	bool bCurrent = true;
	if (!m_upgradeVersionNumber.empty() && !CUtilities::StringCompareIgnoreCase(m_upgradeVersionNumber, m_currentVersionNumber))
		bCurrent = false;

	return bCurrent;
}

bool CCarrierUpgrade::Upgrade()
{
	bool upgradePerformed = false;
	if (!m_fpgaFilePath.empty())
	{
		// Verify 
		LogString("FLASHING", "Carrier upgrade file = %s", m_fpgaFilePath.c_str());
		FILE *file = fopen(m_fpgaFilePath.c_str(), "r");
		if (file != nullptr)
		{
			LogString("FLASHING", "Carrier upgrade file opened");
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
				char filename[256];
				memcpy(filename, m_fpgaFilePath.c_str(), 256);
				S7_Result result = S7LITE_FirmwareUpgradeByFile(filename);
				upgradePerformed = result != S7DLL_STATUS_ERROR ? true : false;
				LogString("FLASHING", "Carrier upgrade successful = %d", result);
			}
			else
			{
				LogString("FLASHING", "Carrier upgrade incorrect CRC, calculated %d, expected %d", calcedCRC, m_fpgaFileCRC);
			}

            fclose(file);
		}
		else
		{
			LogString("FLASHING", "Carrier upgrade file not opened");
		}
	}

	return upgradePerformed;
}
