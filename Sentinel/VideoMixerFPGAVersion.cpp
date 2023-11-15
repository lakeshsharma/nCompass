#include "stdafx.h"
#include "VideoMixerFPGAVersion.h"

// Sentinel includes.
#include "Utilities.h"
#include "axhybrid.h"

using namespace std;

CVideoMixerFPGAVersion::CVideoMixerFPGAVersion(void)
	:   m_major("")
	,   m_minor("")
	,   m_build("")
{
}

CVideoMixerFPGAVersion::~CVideoMixerFPGAVersion(void)
{
}

std::string CVideoMixerFPGAVersion::GetVersionString()
{
	string returnStr;
	struct axvm_version vmVersion;
	int32_t vmResult = AXVM_GetFirmwareVersion((struct axvm_version *)&vmVersion);

	if (vmResult == 0)
	{
		stringstream ss;
		ss << vmVersion.core_major << "." << vmVersion.core_minor << "." << vmVersion.core_build;
		returnStr = ss.str();
	}
	else
	{
		returnStr = "";
	}

	return returnStr;
}

std::string CVideoMixerFPGAVersion::GetUpgradeVersionString()
{
	string returnStr = "";
	if (m_major != "")
	{
		stringstream ss;
		ss << m_major << "." << m_minor << "." << m_build;
		returnStr = ss.str();
	}

	return returnStr;
}

void CVideoMixerFPGAVersion::SetVersion(int16_t major, int16_t minor, int16_t build)
{
	// convert parameters to string
	const int PARAMSIZE = 8;
	char val[PARAMSIZE];
	
	sprintf(val, "%02d", major);
	string majorstr(val);	
	sprintf(val, "%02d", minor);
	string minorstr(val);	
	sprintf(val, "%02d", build);
	string buildstr(val);
	SetVersion(majorstr, minorstr, buildstr);
}

void CVideoMixerFPGAVersion::SetVersion(const std::string& major, const std::string& minor, const std::string& build)
{
	m_major = major;
	m_minor = minor;
	m_build = build;
}

