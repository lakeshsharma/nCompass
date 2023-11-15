#include "stdafx.h"
#include "ConfigFile.h"
#include "DirectoryManager.h"
#include "Logging/LogString.h"
#include <ios>

using namespace std;

const LPCTSTR CConfigFile::FALSE_STRING(_T("false"));
const LPCTSTR CConfigFile::TRUE_STRING(_T("true"));
const LPCTSTR CConfigFile::EQUALS_STRING(_T("="));
const LPCTSTR CConfigFile::NEW_LINE_STRING(_T("\n"));

std::recursive_mutex CConfigFile::m_criticalSection;

CConfigFile::CConfigFile(DWORD startupTickCount, IOperatingSystem &operatingSystem) :
m_lastConfigFileLoaded(0),
m_startupTickCount(startupTickCount),
m_operatingSystem(operatingSystem)
{
}

CConfigFile::~CConfigFile()
{
}

// The compiler requires an initializer for m_operatingSystem.
CConfigFile::CConfigFile(const CConfigFile &rhs) :
	m_operatingSystem(rhs.m_operatingSystem)
{
	*this = rhs;
}

CConfigFile& CConfigFile::operator=(const CConfigFile &rhs)
{
	std::unique_lock<std::recursive_mutex> lock(m_criticalSection);

	if (this != &rhs)
	{
		m_lastConfigFileLoaded = rhs.m_lastConfigFileLoaded;
		m_startupTickCount = rhs.m_startupTickCount;
		m_operatingSystem = rhs.m_operatingSystem;
	}

	return *this;
}

bool CConfigFile::LoadConfigFile(std::string configFileName, bool tryUSB)
{
	std::unique_lock<std::recursive_mutex> lock(m_criticalSection);

 	int success = 0;
    std::string filePath;

	SetDefaults();

    if (tryUSB) 
    {
		// cycle thru the USB paths
		int usbIndex = 0;
		bool moreUSBPaths = true;
		while (success == 0 && moreUSBPaths)
		{
			filePath = CDirectoryManager::Instance().USBPath(usbIndex);

			if (!filePath.empty())
			{
				success = ReadConfigValues(filePath + configFileName); 
            }
			else
			{
				moreUSBPaths = false;
			}
			usbIndex++;
		}
    }
    
	if (success == 0)
	{
    	// Try reading a file from the default download directory.
    	filePath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + configFileName;

    	success = ReadConfigValues(filePath); 
    }

    if (success == 0)
    {
        // Try reading a file from the OS directory.  It is ok to try again if tried previously.
        filePath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_OS) + configFileName;
        success = ReadConfigValues(filePath);
    }
	
	if (success > 0)
	{
		LogConfigValues(filePath.c_str(), success == 2);
	}

	// Set the Time when the file was read
	m_lastConfigFileLoaded = CUtilities::GetCurrentTime();
	return success;
}

time_t CConfigFile::GetLastConfigFileLoaded() const
{
	return m_lastConfigFileLoaded;
}

int CConfigFile::ReadConfigValues(std::string filePath)
{
	std::unique_lock<std::recursive_mutex> lock(m_criticalSection);

	int retval = 0;

    ifstream file;    
    file.open(filePath);

    if (file.is_open()) 
    {
		retval = 1;

        string lineRead;

        while (!file.eof()) 
        {
            getline(file, lineRead);

            // trim any unwanted white space
            lineRead.erase(0, lineRead.find_first_not_of(" \n\r\t"));                                                                                               
            lineRead.erase(lineRead.find_last_not_of(" \n\r\t")+1);

            int eqIndex;
            if ((eqIndex = lineRead.find('=')) > 0)
            {
                string desc = lineRead.substr(0, eqIndex);
                string value = lineRead.substr(eqIndex + 1);

                if (InterpretNameValue(desc, value))
                {
                    retval = 2;
                }
            }
        }
        file.close();
    }
    
	return retval;

}

bool CConfigFile::InterpretBool(const std::string &value, bool &variable)
{
	std::unique_lock<std::recursive_mutex> lock(m_criticalSection);

	bool retVal(false);

	if (value == TRUE_STRING)
	{
		variable = true;
		retVal = true;
	}
	else if (value == FALSE_STRING)
	{
		variable = false;
		retVal = true;
	}

	return retVal;
}

bool CConfigFile::InterpretWORD(const std::string &value, WORD &variable)
{
	std::unique_lock<std::recursive_mutex> lock(m_criticalSection);

	DWORD numericValue;
	bool ok(InterpretDWORD(value, numericValue));

	if (ok)
	{
		if (numericValue <= WORD_MAX)
		{
			variable = (WORD)numericValue;
		}
		else
		{
			ok = false;
		}
	}

	return ok;
}

bool CConfigFile::InterpretDWORD(const std::string &value, DWORD &variable)
{
	std::unique_lock<std::recursive_mutex> lock(m_criticalSection);

	bool retVal(false);

	if (value.size() > 0)
	{
		LPTSTR endPtr(NULL);
		errno = 0; // Clear errno so we can tell if the following function sets it.
		unsigned long numericValue = _tcstoul(value.c_str(), &endPtr, RADIX);
		if (errno != ERANGE)
		{
			if (endPtr == static_cast<LPCTSTR>(value.c_str())+value.size())
			{
				if (numericValue <= DWORD_MAX)
				{
					variable = (DWORD)numericValue;
					retVal = true;
				}
			}
		}
	}

	return retVal;
}

bool CConfigFile::InterpretInt(const std::string &value, int &variable)
{
	std::unique_lock<std::recursive_mutex> lock(m_criticalSection);

	bool retVal(false);

	if (value.size() > 0)
	{
		LPTSTR endPtr(NULL);
		errno = 0; // Clear errno so we can tell if the following function sets it.
		int numericValue = _tcstol(value.c_str(), &endPtr, RADIX);
		if (errno != ERANGE)
		{
			if (endPtr == static_cast<LPCTSTR>(value.c_str())+value.size())
			{
				if (numericValue <= INT_MAX)
				{
					variable = numericValue;
					retVal = true;
				}
			}
		}
	}

	return retVal;
}

void CConfigFile::SaveConfigFile(const std::string &configFileName)
{
	std::unique_lock<std::recursive_mutex> lock(m_criticalSection);
	std::string filePath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + configFileName;
	bool success = WriteConfigFile(filePath);
    filePath = (filePath + " [SAVE]"); 
    LogConfigValues(filePath.c_str(), success);
}

bool CConfigFile::WriteConfigFile(const std::string &filePath)
{
	std::unique_lock<std::recursive_mutex> lock(m_criticalSection);

	bool retval(false);
    std::shared_ptr<std::ofstream> file ;
    file = std::shared_ptr<std::ofstream>(new std::ofstream(filePath.c_str(), std::ios_base::app | std::ios_base::out));
    file.get()->exceptions (std::ofstream::failbit | std::ofstream::badbit);

    if ( file.get()!=NULL)//No error in opening filestd::shared_ptr<std::ofstream> file
	{
		try
		{
			retval = WriteConfigFile(file);
		}
		catch (std::ofstream::failure &pEx)
		{
			const int MSG_SIZE(400);
			std::string errorMsg;
            errorMsg.reserve(MSG_SIZE);
            errorMsg = pEx.what();
			LogString(ERROR_LOG, _T("%s(%s) failed. %s"),
                      static_cast<LPCTSTR>(__TFUNCTION__), filePath.c_str(), errorMsg.c_str());
		}

		file.get()->close();
	}

	return retval;
}

bool CConfigFile::WriteConfigFile(const std::shared_ptr<std::ofstream>  &file)
{
    UNREFERENCED_PARAMETER(&file); 

	return false;
}

void CConfigFile::WriteConfigItem(const std::shared_ptr<std::ofstream>  &file, LPCTSTR name, LPCTSTR value)
{
	(*file) << name;
	(*file) << EQUALS_STRING;
	(*file) << value;
	(*file) << NEW_LINE_STRING;
}

void CConfigFile::WriteConfigItem(const std::shared_ptr<std::ofstream>  &file, LPCTSTR name, bool value)
{
	WriteConfigItem(file, name, value ? TRUE_STRING : FALSE_STRING);
}

void CConfigFile::WriteConfigItem(const std::shared_ptr<std::ofstream>  &file, LPCTSTR name, DWORD value)
{
    std::stringstream sValueStream;
    sValueStream << (unsigned)value;
    WriteConfigItem(file, name, sValueStream.str().c_str());
}

void CConfigFile::WriteConfigItem(const std::shared_ptr<std::ofstream>  &file, LPCTSTR name, int value)
{
    std::stringstream sValueStream;
    sValueStream << value;
    WriteConfigItem(file, name, sValueStream.str().c_str());
}

