#include "stdafx.h"
#include "ProcessDump/WebDiagnosticsCommonInfo.h"

// Sentinel includes.
#include "Util/Convert.h"
#include "Utilities.h"
#include "Logging/LogString.h"
#include "Platform.h"

using namespace std;

CWebDiagnosticsCommonInfoBase::CWebDiagnosticsCommonInfoBase(
	time_t startupTime,
	time_t currentTime,
	long slotMastID,
	long machineNumber,
	const std::string &sentinelVersion,
	const std::string &osImageVersion,
	const CPackageInfo activeSkinInfo,
	const CPackageInfo pendingSkinInfo,
	const CPackageInfo activeCampaignInfo,
	const CPackageInfo pendingCampaignInfo,
	const std::string &carrierVersion,
	const std::string &vmVersion,
    const std::string &batteryStatus,
	long lCPUtemp)
	: m_startupTime(startupTime)
	, m_currentTime(currentTime)
	, m_slotMastID(slotMastID)
	, m_machineNumber(machineNumber)
	, m_sentinelVersion(sentinelVersion)
	, m_osImageVersion(osImageVersion)
	, m_activeSkinInfo(activeSkinInfo)
	, m_pendingSkinInfo(pendingSkinInfo)
	, m_activeCampaignInfo(activeCampaignInfo)
	, m_pendingCampaignInfo(pendingCampaignInfo)
	, m_carrierVersion(carrierVersion)
	, m_vmVersion(vmVersion)
    , m_batteryStatus(batteryStatus)
	, m_lCPUtemp(lCPUtemp)
{
}

void CWebDiagnosticsCommonInfoBase::WriteHtml(std::ostream& os) const
{
	std::string activeSkinName, activeSkinVersion, activeSkinRevision;
	std::string pendingSkinName, pendingSkinVersion, pendingSkinRevision, pendingSkinMediaCount;

	activeSkinName = m_activeSkinInfo.GetName();
	if (!activeSkinName.empty())
	{
		activeSkinVersion = m_activeSkinInfo.GetVersion();
		activeSkinRevision = std::to_string(m_activeSkinInfo.GetRevision());
	}

	pendingSkinName = m_pendingSkinInfo.GetName();
	if (!pendingSkinName.empty())
	{
		pendingSkinVersion = m_pendingSkinInfo.GetVersion();
		pendingSkinRevision = std::to_string(m_pendingSkinInfo.GetRevision());
		pendingSkinMediaCount = std::to_string(m_pendingSkinInfo.GetPendingMediaCount());
	}

	std::string activeCampaignName, activeCampaignId, activeCampaignRevision;
	std::string pendingCampaignName, pendingCampaignId, pendingCampaignRevision, pendingCampaignMediaCount;

	activeCampaignName = m_activeCampaignInfo.GetName();
	if (!activeCampaignName.empty())
	{
		activeCampaignId = std::to_string(m_activeCampaignInfo.GetId());
		activeCampaignRevision = std::to_string(m_activeCampaignInfo.GetRevision());
	}

	pendingCampaignName = m_pendingCampaignInfo.GetName();
	if (!pendingCampaignName.empty())
	{
		pendingCampaignId = std::to_string(m_pendingCampaignInfo.GetId());
		pendingCampaignRevision = std::to_string(m_pendingCampaignInfo.GetRevision());
		pendingCampaignMediaCount = std::to_string(m_pendingCampaignInfo.GetPendingMediaCount());
	}

	string ethernetType, mcuType, touchscreenType;
	CPlatform::Instance().GetHardwareComponents(ethernetType, mcuType, touchscreenType);
	os << "Startup Time: " << CUtilities::FormatDateTimeString(m_startupTime) << "<br>" <<
		"Current Time: " << CUtilities::FormatDateTimeString(m_currentTime) << "<br>" <<
		"SlotMast ID: " << m_slotMastID << "<br>" <<
		"Machine Number: " << m_machineNumber << "<br>" <<
		"Sentinel Version: " << m_sentinelVersion << "<br>" <<
		"OS Image Version: " << m_osImageVersion << "<br>" <<
		"Active Skin Name: " << activeSkinName << "<br>" <<
		"Active Skin Version: " << activeSkinVersion << "<br>" <<
		"Active Skin Revision: " << activeSkinRevision << "<br>";

	if (!pendingSkinName.empty())
	{
		os << "Pending Skin Name: " << pendingSkinName << "<br>" <<
			"Pending Skin Version: " << pendingSkinVersion << "<br>" <<
			"Pending Skin Revision: " << pendingSkinRevision << "<br>" <<
			"Pending Skin Media Count Still To Download: " << pendingSkinMediaCount << "<br>";
	}

	os << "Active Campaign Name: " << activeCampaignName << "<br>" <<
		"Active Campaign Id: " << activeCampaignId << "<br>" <<
		"Active Campaign Revision: " << activeCampaignRevision << "<br>";

	if (!pendingCampaignName.empty())
	{
		os << "Pending Campaign Name: " << pendingCampaignName << "<br>" <<
			"Pending Campaign Id: " << pendingCampaignId << "<br>" <<
			"Pending Campaign Revision: " << pendingCampaignRevision << "<br>" <<
			"Pending Campaign Media Count Still To Download: " << pendingCampaignMediaCount << "<br>";
	}

	os << "Carrier Version: " << m_carrierVersion << "<br>" <<
		"VM Version: " << m_vmVersion << "<br>" <<
		"Battery Status: " << m_batteryStatus << "<br>" <<
		"CPU Temperature (Celsius): " << ((double)(m_lCPUtemp))/CPU_TEMP_DIVISOR << "<br>" <<
		"BIOS Version: " << CPlatform::Instance().GetBiosVersion() << "<br>" <<
		"Processor Type: " << CPlatform::Instance().GetProcessorType() << "<br>" <<
		"Ethernet Type: " << ethernetType << "<br>" <<
		"MCU Type: " << mcuType << "<br>" <<
		"Touchscreen Type: " << touchscreenType << "<br>" <<
		"<br>";
}

CWebDiagnosticsCommonInfo::CWebDiagnosticsCommonInfo(time_t startupTime,
	time_t currentTime,
	long slotMastID,
	long machineNumber,
	const std::string &sentinelVersion,
	const std::string &osImageVersion,
	const CPackageInfo activeSkinInfo,
	const CPackageInfo pendingSkinInfo,
	const CPackageInfo activeCampaignInfo,
	const CPackageInfo pendingCampaignInfo,
	DWORD availablePhysicalMemory,
	const std::string pollerIP,
	DWORD pollerPacketSentVerifyErrors,
	WORD pollerPacketVersion,
	const std::string & onlineStateStr,
	const std::string &carrierVersion,
	const std::string &vmVersion,
	const std::string &batteryStatus,
	const long lCPUtemp) :
	CWebDiagnosticsCommonInfoBase(startupTime, currentTime, slotMastID, machineNumber, sentinelVersion, osImageVersion,
		activeSkinInfo, pendingSkinInfo, activeCampaignInfo, pendingCampaignInfo, carrierVersion, vmVersion, batteryStatus, lCPUtemp),
	m_availablePhysicalMemory(availablePhysicalMemory),
	m_pollerIP(pollerIP),
	m_pollerPacketSentVerifyErrors(pollerPacketSentVerifyErrors),
	m_pollerPacketVersion(pollerPacketVersion),
	m_onlineStateStr(onlineStateStr)
{
}

void CWebDiagnosticsCommonInfo::WriteHtml(std::ostream& os) const
{
	CWebDiagnosticsCommonInfoBase::WriteHtml(os);

}
