#include "stdafx.h"
#include "ProgressiveHostManager.h"

using namespace std;

CProgressiveHostManager::CProgressiveHostManager(std::vector<std::string> &hostNameList, bool namesAreDNS)
{
	// If each name in the host name list is a host name in the DNS.
	// Therefore we need a progressive host object for each name in the list.
	if (namesAreDNS)
	{
		SetupWithDNSLookup(hostNameList);
	}

	// The list of host names are the addresses. 
	// In this case there will be single progressive host object
	else
	{
		SetupWithAddressList(hostNameList);
	}

	m_currentHostIndex = 0;
}


CProgressiveHostManager::~CProgressiveHostManager()
{
	for (int hostIndex = 0; hostIndex < (int)m_hostArray.size(); hostIndex++)
		delete m_hostArray[hostIndex];
}

void CProgressiveHostManager::SetupWithDNSLookup(std::vector<std::string> &hostNameList)
{
	for (int hostIndex = 0; hostIndex < (int)hostNameList.size(); hostIndex++)
	{
		std::string hostName;
		DWORD port = ExtractPortFromHostname(hostNameList[hostIndex], hostName);
		CProgressiveHost *progressiveHost = new CProgressiveHost(hostName.c_str(), port);
		m_hostArray.push_back(progressiveHost);
	}
}

void CProgressiveHostManager::SetupWithAddressList(std::vector<std::string> &addressList)
{
	CIPAddressArray addressArray;
	for (int hostIndex = 0; hostIndex < (int)addressList.size(); hostIndex++)
	{
		std::string hostName;
		DWORD port = ExtractPortFromHostname(addressList[hostIndex], hostName);
		addressArray.Add(hostName.c_str(), port);
	}

	CProgressiveHost *progressiveHost = new CProgressiveHost(addressArray);
	m_hostArray.push_back(progressiveHost);
}

string CProgressiveHostManager::GetUrl()
{
	string url;
	lock_guard<recursive_mutex> lock(m_criticalSection);
	CProgressiveHost *currentHost = GetCurrentHost();

	while (url.empty() && currentHost != NULL)
	{
		// Get the url from the current host
		if (currentHost->HasAddress())
		{
			url = currentHost->GetURL();
		}

		// We went through our list of addresses on this host, move on to the next one
		else
		{
			m_currentHostIndex++;
			currentHost = GetCurrentHost();
		}
	}
	return url;
}

void CProgressiveHostManager::ConnectionFailure()
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	CProgressiveHost *currentHost = GetCurrentHost();
	if (currentHost != NULL)
	{
		currentHost->SetToNextAddress();
	}
}

bool CProgressiveHostManager::AddressCheck()
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	// Get the address we are currently using before the update of the hosts
	CIPAddress prevAddress(GetCurrentAddress());

	m_currentHostIndex = -1;

	// Do an update on each host to get us back to the preferred address.
	for (int hostIndex = 0; hostIndex < (int)m_hostArray.size(); hostIndex++)
	{
		CProgressiveHost *host = m_hostArray[hostIndex];
		if (host != NULL)
		{
			if (host->UpdateAddress() && m_currentHostIndex < 0)
				m_currentHostIndex = hostIndex;
		}
	}

	if (m_currentHostIndex < 0)
		m_currentHostIndex = 0;

	// Returns true if the address has changed
	return (prevAddress != GetCurrentAddress());
}

DWORD CProgressiveHostManager::ExtractPortFromHostname(const std::string & hostNameWithPort, std::string &hostName)
{
	DWORD port = 0;
	hostName = hostNameWithPort;

	int portIndex = hostName.find_last_of(':');
	if (portIndex >= 0)
	{
		string portStr = hostName.substr(portIndex + 1);
		if (!portStr.empty())
		{
			port = stoi(portStr.c_str());
		}

		hostName = hostName.substr(0, portIndex);
		hostName.erase(std::remove(hostName.begin(), hostName.end(), '\t'), hostName.end());
	}

	return port;
}

CProgressiveHost *CProgressiveHostManager::GetCurrentHost()
{
	CProgressiveHost *currentHost = NULL;
	if (m_currentHostIndex < (int)m_hostArray.size())
	{
		currentHost = m_hostArray[m_currentHostIndex];
	}
	return currentHost;
}

CIPAddress CProgressiveHostManager::GetCurrentAddress()
{
	CIPAddress currentIPAddress;
	CProgressiveHost *currentHost = GetCurrentHost();
	if (currentHost != NULL)
		currentIPAddress = currentHost->GetIPAddress();

	return currentIPAddress;
}
