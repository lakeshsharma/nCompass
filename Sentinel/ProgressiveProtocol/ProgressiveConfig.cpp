#include "stdafx.h"
#include "ProgressiveConfig.h"


const LPCTSTR CProgressiveConfig::DEFAULT_ENDPOINT1("onelinkdiscossl:61614");
const LPCTSTR CProgressiveConfig::DEFAULT_ENDPOINT2("onelinkdisco:61613");

CProgressiveConfig::CProgressiveConfig()
{
	SetDefaults();
}

CProgressiveConfig::~CProgressiveConfig()
{
}

CProgressiveConfig::CProgressiveConfig(const CProgressiveConfig &rhs)
{
	*this = rhs;
}

CProgressiveConfig& CProgressiveConfig::operator=(const CProgressiveConfig &rhs)
{
	if (this != &rhs)
	{
		SetDefaults();

		m_hasConfigFile = rhs.m_hasConfigFile;
		m_active = rhs.m_active;
		m_dnsLookup = rhs.m_dnsLookup;
		m_endpoints = rhs.m_endpoints;
		m_haveDefaultEndpoints = rhs.m_haveDefaultEndpoints;
		m_pingInterval = rhs.m_pingInterval;
		m_patronInterval = rhs.m_patronInterval;
	}

	return *this;
}

void CProgressiveConfig::SetDefaults()
{
	m_hasConfigFile = DEFAULT_HAS_CONFIG_FILE;
	m_active = DEFAULT_ACTIVE;
	m_dnsLookup = DEFAULT_DNS_LOOKUP;
	m_endpoints.clear();
	AddEndpoint(DEFAULT_ENDPOINT1);
	AddEndpoint(DEFAULT_ENDPOINT2);
	m_haveDefaultEndpoints = true;
	m_pingInterval = DEFAULT_PING_INTERVAL;

	SetPatronInterval();
}

void CProgressiveConfig::SetHasConfigFile(bool hasConfigFile)
{
	this->m_hasConfigFile = hasConfigFile;
}

bool CProgressiveConfig::HasConfigFile() const
{
	return this->m_hasConfigFile;
}

void CProgressiveConfig::SetActive(bool active)
{
	this->m_active = active;
}

bool CProgressiveConfig::GetActive() const
{
	return this->m_active;
}

void CProgressiveConfig::SetDNSLookup(bool dnsLookup)
{
	this->m_dnsLookup = dnsLookup;
}

bool CProgressiveConfig::GetDNSLookup() const
{
	return this->m_dnsLookup;
}

void CProgressiveConfig::AddEndpoint(const std::string &endpoint)
{
	if (m_haveDefaultEndpoints)
	{
		m_haveDefaultEndpoints = false;
		m_endpoints.clear();
	}
	this->m_endpoints.push_back(endpoint);
}

int CProgressiveConfig::GetEndpointCount() const
{
	return this->m_endpoints.size();
}

std::string CProgressiveConfig::GetEndpoint(int index) const
{
	std::string endpoint;
	if (0 <= index && index < (int)m_endpoints.size())
	{
		endpoint = m_endpoints[index];
	}

	return endpoint;
}

void CProgressiveConfig::SetPingInterval(DWORD pingInterval)
{
	if (MINIMUM_PING_INTERVAL <= pingInterval && pingInterval <= MAXIMUM_PING_INTERVAL)
	{
		this->m_pingInterval = pingInterval;
		SetPatronInterval();
	}
}

DWORD CProgressiveConfig::GetPingInterval() const
{
	return this->m_pingInterval;
}

DWORD CProgressiveConfig::GetPatronInterval() const
{
	return this->m_patronInterval;
}

void CProgressiveConfig::SetPatronInterval()
{
	// Set the patron interval based upon the ping interval
	if (m_pingInterval > MINIMUM_PING_INTERVAL + PATRON_PING_INTERVAL_DELTA)
	{
		m_patronInterval = m_pingInterval - PATRON_PING_INTERVAL_DELTA;
	}
	else
	{
		m_patronInterval = MINIMUM_PING_INTERVAL;
	}
}

