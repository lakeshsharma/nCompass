#include "stdafx.h"
#include "Connection.h"

CConnection::CConnection(void)
{
	m_slotmastID = INVALID_SLOT_MAST_ID;
	m_slotNumber = INVALID_SLOT_NUMBER;
	CIPAddress address;
	address.SetIP("0.0.0.0");
	m_subnetMask = address;
	m_defaultGateway = address;
	m_preferredDNS = address;
	m_alternateDNS = address;
	m_actualIP = address;
	m_autoDHCP = true;
	m_lPort = HOST_PORT_DEFAULT;
}

CConnection::~CConnection(void)
{
}

void CConnection::SetSlotNumber(long slotNumber)
{
	m_slotNumber = slotNumber;
}

long CConnection::GetSlotNumber() const
{
	return m_slotNumber;
}

void CConnection::SetSlotmastID(long slotmastID)
{
	m_slotmastID = slotmastID;
}

long CConnection::GetSlotmastID()
{
	return m_slotmastID;
}

void CConnection::SetHost(const std::string& host)
{
	m_strHost = host;
}

void CConnection::SetHost(char * lpHost)
{
    m_strHost = (lpHost == nullptr ? std::string() : lpHost);
}

void CConnection::SetPort(const std::string& port)
{
	m_lPort = _tcstol(port.c_str(), NULL, RADIX_DECIMAL);
}

void CConnection::SetPort(long lPort)
{
	m_lPort = lPort;
}

std::string CConnection::GetHost()
{
	return m_strHost;
}

long CConnection::GetPort()
{
	return m_lPort;
}

void CConnection::SetMyIP(CIPAddress myIP)
{
	m_myIP = myIP;
}
CIPAddress CConnection::GetMyIP(void)
{
	return m_myIP;
}

void CConnection::SetSubnetMask(CIPAddress subnetMask)
{
	m_subnetMask = subnetMask;
}
CIPAddress CConnection::GetSubnetMask(void)
{
	return m_subnetMask;
}

void CConnection::SetDefaultGateway(CIPAddress defaultGateway)
{
	m_defaultGateway = defaultGateway;
}
CIPAddress CConnection::GetDefaultGateway(void)
{
	return m_defaultGateway;
}

void CConnection::SetPreferredDNS(CIPAddress preferredDNS)
{
	m_preferredDNS = preferredDNS;
}
CIPAddress CConnection::GetPreferredDNS(void)
{
	return m_preferredDNS;
}

void CConnection::SetAlternateDNS(CIPAddress alternateDNS)
{
	m_alternateDNS = alternateDNS;
}
CIPAddress CConnection::GetAlternateDNS(void)
{
	return m_alternateDNS;
}

void CConnection::SetActualIP(CIPAddress actualIP)
{
	m_actualIP = actualIP;
}

CIPAddress CConnection::GetActualIP()
{
	return m_actualIP;
}

void CConnection::SetMacAddress(const std::string &macAddress)
{
	m_macAddress = macAddress;
}
std::string CConnection::GetMacAddress()
{
	return m_macAddress;
}

void CConnection::SetDHCP(bool autoDHCP)
{
	m_autoDHCP = autoDHCP;
}
bool CConnection::AutoDHCP(void)
{
	return m_autoDHCP;
}
