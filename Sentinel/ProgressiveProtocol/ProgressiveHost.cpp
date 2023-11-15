#include "stdafx.h"
#include "ProgressiveHost.h"
#include "GlobalDefs.h"

using namespace std;

const std::string URL_HEADER_MULTICAST = "multicast://"; // {IP}:{Port}?group=multimic

const int PORT_STR_MAX = 13; // For worst-case int: ":-2147483648", plus trailing NUL character.
const std::string URL_HEADER_TCP = "tcp://";
const std::string URL_HEADER_SSL = "ssl://";
const std::string URL_TAIL = "?wireFormat=stomp";
const long TCP_PORT = 61613;

CProgressiveHost::CProgressiveHost(const std::string& hostName, DWORD hostPort)
	:   m_name(hostName)
    ,   m_port(hostPort)
	,   m_currentAddressIndex(0)
{
	UpdateAddress();
}

CProgressiveHost::CProgressiveHost(CIPAddressArray &addressArray)
{
	m_name.clear();
	m_port = 0;
	m_currentAddressIndex = 0;

	RandomAddressListMove(addressArray);
}


CProgressiveHost::~CProgressiveHost()
{

}

CProgressiveHost::CProgressiveHost(const CProgressiveHost &rhs)
{
	*this = rhs;
}

CProgressiveHost& CProgressiveHost::operator=(const CProgressiveHost &rhs)
{
	if (this != &rhs)
	{
		m_name = rhs.m_name;
		m_port = rhs.m_port;
		m_addresses = rhs.m_addresses;
		m_currentAddressIndex = rhs.m_currentAddressIndex;
	}

	return *this;
}

string CProgressiveHost::GetName() const
{
	return m_name;
}

bool CProgressiveHost::HasAddress() const
{
	return (m_currentAddressIndex < m_addresses.GetCount());
}

CIPAddress CProgressiveHost::GetIPAddress() const
{
	return GetIPAddress(m_currentAddressIndex);
}

bool CProgressiveHost::SetToNextAddress()
{
	m_currentAddressIndex++;

	return HasAddress();
}

size_t CProgressiveHost::GetCurrentAddressIndex() const
{
	return m_currentAddressIndex;
}

size_t CProgressiveHost::GetIPAddressCount() const
{
	return m_addresses.GetCount();
}

CIPAddress CProgressiveHost::GetIPAddress(size_t index) const
{
	CIPAddress currentAddress;
	if (m_addresses.GetCount() > index)
	{
		currentAddress = m_addresses.GetAt(index);
	}

	return currentAddress;
}

DWORD CProgressiveHost::GetHostPort() const
{
	return m_port;
}

string CProgressiveHost::GetURL() const
{
	string url;
	
	if (HasAddress())
	{
		CIPAddress ipAddress = GetIPAddress();

		if (ipAddress.GetPort() == TCP_PORT)
		{
			url = URL_HEADER_TCP;
		}
		else
		{
			url = URL_HEADER_SSL;
		}

		char ipAddressStr[IPADDRESS_STR_LENGTH];
		ipAddress.GetIP(ipAddressStr, IPADDRESS_STR_LENGTH);

		url.append(ipAddressStr);

		char portStr[PORT_STR_MAX];
		snprintf(portStr, PORT_STR_MAX, ":%ld", (long)ipAddress.GetPort());
		url.append(portStr);
		url.append(URL_TAIL);
	}

	return url;
}

bool CProgressiveHost::UpdateAddress()
{
	CIPAddress currentAddress = GetIPAddress();

	// We do a DNS look-up only if we have a host name 
	if (!m_name.empty())
	{
		// Do a DNS look up for this host
		CIPAddressArray newAddressList;
		CUtilities::DNSLookup(m_name.c_str(), newAddressList);

		// Set the port for all the addresses in our list.
		newAddressList.SetPortAll(m_port);

		// See if the new address list is the same as our old list (NOTE: order does not matter).
		if ( newAddressList != m_addresses)
		{
			// If the list has changed, make this list our new list but in a random order.
			RandomAddressListMove(newAddressList);
		}
	}

	// The first item in this host's address list should always be the preferred.
	// We always want to get back to the preferred item.
	m_currentAddressIndex = 0;

	return !GetIPAddress().IsEmpty();
}

/// <summary>
/// Randomly moves items from the provided list to this class' list.
/// </summary>
/// <param name="addressArray">The address array.</param>
void CProgressiveHost::RandomAddressListMove(CIPAddressArray &newAddresses)
{
	// Clear our address list
	m_addresses.RemoveAll();

	// Seed the random number generator.
	// Use the current milliseconds in addition to the tick count so the unit tests are successful
	SYSTEMTIME systime;
	CUtilities::GetUTCSystemTime(&systime);
	srand(systime.wMilliseconds + CUtilities::GetTickCount());

	// Move items from the new list to our address list
	while (newAddresses.GetCount() > 0)
	{
		// Randomly pick an item from the new list
		size_t randomIndex = rand() % (size_t)(newAddresses.GetCount());
		CIPAddress listAddress = newAddresses.GetAt(randomIndex);

		// If the port is not specified, use this class' port
		if (listAddress.GetPort() == 0)
		{
			listAddress.SetPort(m_port);
		}

		// move this address from the new list into our list
		m_addresses.Add(listAddress);
		newAddresses.RemoveAt(randomIndex);
	}
}
