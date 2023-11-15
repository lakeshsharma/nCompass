#include "stdafx.h"
#include "ProgressiveConfigFile.h"

// Include Sentinel headers.
#include "Sentinel.h"

const LPCTSTR CProgressiveConfigFile::CONFIG_FILENAME(_T("/ProgressiveProtocolConfig.txt"));
const LPCTSTR CProgressiveConfigFile::HAS_CONFIG_FILE(_T("HasConfigFile"));
const LPCTSTR CProgressiveConfigFile::ACTIVE(_T("Active"));
const LPCTSTR CProgressiveConfigFile::DNS_LOOKUP(_T("DNSLookup"));
const LPCTSTR CProgressiveConfigFile::ENDPOINT(_T("Endpoint"));
const LPCTSTR CProgressiveConfigFile::PING_INTERVAL(_T("PingInterval"));

CProgressiveConfigFile::CProgressiveConfigFile(IOperatingSystem &operatingSystem) :
	CConfigFile(0, operatingSystem)
{
	SetDefaults();
}

CProgressiveConfigFile::~CProgressiveConfigFile()
{
}

bool CProgressiveConfigFile::Start()
{
	bool success(LoadConfigFile(CONFIG_FILENAME));
	return success;
}

void CProgressiveConfigFile::RefreshConfig(bool refresh)
{
	UNREFERENCED_PARAMETER(refresh);
	Start();
}

/// <summary>
/// Set member(s) to default values.
/// </summary>
void CProgressiveConfigFile::SetDefaults()
{
	m_progressiveConfig.SetDefaults();
}

bool CProgressiveConfigFile::InterpretNameValue(const std::string& name, const std::string& value)
{
	bool retVal(false);

	if (ACTIVE == name)
	{
		bool boolValue;
		retVal = InterpretBool(value, boolValue);
		if (retVal)
		{
			m_progressiveConfig.SetActive(boolValue);
		}
	}
	else if (DNS_LOOKUP == name)
	{
		bool boolValue;
		retVal = InterpretBool(value, boolValue);
		if (retVal)
		{
			m_progressiveConfig.SetDNSLookup(boolValue);
		}
	}
	else if (ENDPOINT == name)
	{
		m_progressiveConfig.AddEndpoint(value);
		retVal = true;
	}
	else if (PING_INTERVAL == name)
	{
		DWORD dwordValue;
		retVal = InterpretDWORD(value, dwordValue);
		if (retVal)
		{
			m_progressiveConfig.SetPingInterval(dwordValue);
		}
	}

	return retVal;
}


void CProgressiveConfigFile::LogConfigValues(LPCTSTR sourceDesc, bool valid)
{
	m_progressiveConfig.SetHasConfigFile(valid);

	LogString(_T("GENERAL"), _T("%s: %s"),
	static_cast<LPCTSTR>(sourceDesc),
	static_cast<LPCTSTR>(valid ? _T("valid") : _T("invalid")));

	LogString(_T("GENERAL"), _T("%s: %s=\"%s\""),
	static_cast<LPCTSTR>(sourceDesc),
	static_cast<LPCTSTR>(HAS_CONFIG_FILE),
	static_cast<LPCTSTR>(m_progressiveConfig.HasConfigFile() ? TRUE_STRING : FALSE_STRING));

	LogString(_T("GENERAL"), _T("%s: %s=\"%s\""),
	static_cast<LPCTSTR>(sourceDesc),
	static_cast<LPCTSTR>(ACTIVE),
	static_cast<LPCTSTR>(m_progressiveConfig.GetActive() ? TRUE_STRING : FALSE_STRING));

	LogString(_T("GENERAL"), _T("%s: %s=\"%s\""),
	sourceDesc,
	static_cast<LPCTSTR>(DNS_LOOKUP),
	static_cast<LPCTSTR>(m_progressiveConfig.GetDNSLookup() ? TRUE_STRING : FALSE_STRING));

	int endpointCount(m_progressiveConfig.GetEndpointCount());
	for (int index = 0; index < endpointCount; ++index)
	{
		LogString(_T("GENERAL"), _T("%s: %s=\"%s\""),
		static_cast<LPCTSTR>(sourceDesc),
		static_cast<LPCTSTR>(ENDPOINT),
		m_progressiveConfig.GetEndpoint(index).c_str());
	}

	LogString(_T("GENERAL"), _T("%s: %s=%u"),
	static_cast<LPCTSTR>(sourceDesc),
	static_cast<LPCTSTR>(PING_INTERVAL),
	(unsigned)m_progressiveConfig.GetPingInterval());
}


/// <summary>
/// Writes a configuration file.
/// </summary>
/// <param name="file">IN - The configuration file, which has already been opened for writing.</param>
bool CProgressiveConfigFile::WriteConfigFile(const std::shared_ptr<std::ofstream>  &file)
{
	bool retval(true);

	WriteConfigItem(file, ACTIVE, m_progressiveConfig.GetActive());
	WriteConfigItem(file, DNS_LOOKUP, m_progressiveConfig.GetDNSLookup());

	int endpointCount(m_progressiveConfig.GetEndpointCount());
	for (int index = 0; index < endpointCount; ++index)
	{
		WriteConfigItem(file, ENDPOINT, m_progressiveConfig.GetEndpoint(index).c_str());
	}

	WriteConfigItem(file, PING_INTERVAL, m_progressiveConfig.GetPingInterval());

	return retval;
}

void CProgressiveConfigFile::SetProgressiveConfig(const CProgressiveConfig &progressiveConfig)
{
	m_progressiveConfig = progressiveConfig;
	SaveConfigFile(CONFIG_FILENAME);
}

CProgressiveConfig* CProgressiveConfigFile::GetProgressiveConfigCopy() const
{
	CProgressiveConfig* progressiveConfig(new CProgressiveConfig(m_progressiveConfig));
	return progressiveConfig;
}


