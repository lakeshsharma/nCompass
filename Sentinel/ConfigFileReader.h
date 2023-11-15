#pragma once

#include "IPAddress.h"

class CConfigFileReader
{
public:

	CConfigFileReader();
	~CConfigFileReader(void);
	bool ConfigFileExists();

	CIPAddress GetNudgeMulticastIP();
	int GetNudgeMulticastPort();

	CIPAddress GetCacheIPMulticastIP();
	int GetCacheIPMulticastPort();

private:
	bool SetAddresses(byte * buffer);
	char * GetNextItem(char* buffer, int& itemlength);

	CIPAddress m_nudgeMulticastIP;
	int m_nudgeMulticastPort;

	CIPAddress m_cacheIPMulticastIP;
	int m_cacheIPMulticastPort;

	bool m_fileExists;
};

