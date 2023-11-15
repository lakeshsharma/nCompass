#include "stdafx.h"

#include "ConfigFileReader.h"
#include "Utilities.h"
#include "Sentinel.h"
#include "DirectoryManager.h"

const LPCTSTR CONFIG_FILE_NAME = "config.txt";

CConfigFileReader::CConfigFileReader()
{
	m_nudgeMulticastPort = 0;
	m_cacheIPMulticastPort = 0;

	//first look for the file on the systemCF card then look on the other card.  This is because if the file is flashed through
	//speedMedia it is saved on one and if it is flashed through floorlogix it is saved on another one.
	string configFilePath;
	configFilePath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA);
    configFilePath += "/";
    configFilePath += CONFIG_FILE_NAME;
	m_fileExists = true;

	std::ifstream file;
	file.open(configFilePath);
	if (file.good())
	{
		char buffer[1024];
		file.read(buffer, 1024);

		LogString("GENERAL", "CConfigFileReader: Found config file, read %u bytes.", sizeof(buffer));

		m_fileExists = SetAddresses((byte *)buffer);
		file.close();
	}
	else
	{
		m_fileExists = false;
	}

	if(!m_fileExists)
	{
        configFilePath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA);
        configFilePath += "/";
        configFilePath += CONFIG_FILE_NAME;

		file.open(configFilePath);
		if (file.good())
		{
    		char buffer[1024];
			file.read(buffer, 1024);

    		LogString("GENERAL", "CConfigFileReader: Found config file, read %u bytes.", sizeof(buffer));

    		m_fileExists = SetAddresses((byte *)buffer);
    		file.close();
    	}
		else
		{
			m_fileExists = false;
		}
	}

	if(m_fileExists)
	{
		remove(configFilePath.c_str());
	}
}

CConfigFileReader::~CConfigFileReader(void)
{
}

bool CConfigFileReader::ConfigFileExists()
{
	return m_fileExists;
}

bool  CConfigFileReader::SetAddresses(byte * buffer)
{
	bool retVal = false;
	char * charBuf = (char*)buffer;
	char* temp;
	int itemLength = 0;
	char tempNum[6];


	//***nudge IP***
	if ((temp = GetNextItem(charBuf, itemLength)) != NULL)
	{
		if (itemLength <= 15)
		{
			m_nudgeMulticastIP.SetIP( string(temp) );
			LogString("GENERAL", "Nudge IP = %s", m_nudgeMulticastIP.GetIP().c_str());

			//***nudge Port***
			charBuf = temp + itemLength;
			if ((temp = GetNextItem(charBuf, itemLength)) != NULL)
			{
				if (itemLength <= 5)
				{
					memcpy_s(tempNum,6, temp, itemLength);
					tempNum[itemLength] = 0;
					if (sscanf(tempNum, "%d", &m_nudgeMulticastPort) > 0)
					{
						LogString("GENERAL", "Nudge Port = %d", m_nudgeMulticastPort);

						//***annoncement IP***
						charBuf = temp + itemLength;
						if ((temp = GetNextItem(charBuf, itemLength)) != NULL)
						{
							if (itemLength <= 15)
							{	
								m_cacheIPMulticastIP.SetIP( string(temp) );
								LogString("GENERAL", "Multicast IP = %s", m_cacheIPMulticastIP.GetIP().c_str());

								//***announcement port***
								charBuf = temp + itemLength;
								if ((temp = GetNextItem(charBuf, itemLength)) != NULL)
								{
									if (itemLength <= 5)
									{
										memcpy_s(tempNum,6, temp, itemLength);
										tempNum[itemLength] = 0;
										if (sscanf(tempNum, "%d", &m_cacheIPMulticastPort) > 0)
										{
											LogString("GENERAL", "Multicast Port = %d", m_cacheIPMulticastPort);
											retVal = true;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return retVal;
}

char * CConfigFileReader::GetNextItem(char* buffer, int& itemLength)
{
	char* retChar;
	itemLength = 0;

	retChar = strchr(buffer, '<');

	if (retChar!= NULL)
	{
		retChar++;
		buffer = strchr(retChar, '>');
		if (buffer != NULL)
		{
			itemLength = buffer - retChar;
		}
	}
	return retChar;
}

	CIPAddress CConfigFileReader::GetNudgeMulticastIP()
	{
		return m_nudgeMulticastIP;
	}

	int CConfigFileReader::GetNudgeMulticastPort()
	{
		return m_nudgeMulticastPort;
	}

	CIPAddress CConfigFileReader::GetCacheIPMulticastIP()
	{
		return m_cacheIPMulticastIP;
	}

	int CConfigFileReader::GetCacheIPMulticastPort()
	{
		return m_cacheIPMulticastPort;
	}

