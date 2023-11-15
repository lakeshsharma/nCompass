#pragma once

#include "ProgressiveProtocol/CampaignPackage.h"
#include "ProgressiveProtocol/SkinPackage.h"

//Divisor for CPU Temperature returned from Platform
const int CPU_TEMP_DIVISOR = 1000;

class CWebDiagnosticsCommonInfoBase
{
public:
	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="startupTime">IN - Local time that the Service Window UI was last restarted.</param>
	/// <param name="currentTime">IN - The current local time.</param>
	/// <param name="slotMastID">IN - The Slot Mast identifier.</param>
	/// <param name="machineNumber">IN - The machine number.</param>
	/// <param name="sentinelVersion">IN - The Sentinel.exe version.</param>
	/// <param name="osImageVersion">IN - The nCompass-specific OS image version.</param>
	/// <param name="activeSkinInfo">IN - The active skin info</param>
	/// <param name="pendingSkinInfo">IN - The pending skin info</param>
	/// <param name="activeCampaignInfo">IN - The active campaign info</param>
	/// <param name="pendingCampaignInfo">IN - The pending campaign info</param>
	/// <param name="carrierVersion">IN - The carrier version.</param> 
	/// <param name="vmVersion">IN - The video mixer 
	/// <param name="batteryStatus">IN - The battery status
	/// <param name="lCPUtemp">IN - The CPU temperature
	/// version.</param> 
	CWebDiagnosticsCommonInfoBase(
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
		const long lCPUtemp);

	time_t m_startupTime;
	time_t m_currentTime;
	long m_slotMastID;
	long m_machineNumber;
	std::string m_sentinelVersion;
	std::string m_osImageVersion;
	CPackageInfo m_activeSkinInfo;
	CPackageInfo m_pendingSkinInfo;
	CPackageInfo m_activeCampaignInfo;
	CPackageInfo m_pendingCampaignInfo;
	std::string m_carrierVersion;
	std::string m_vmVersion;
	std::string m_batteryStatus;
	long m_lCPUtemp;

	///	<summary>
	/// Writes HTML.
	///	</summary>
	/// <param name="os">The ostream to write to.</param>
	void WriteHtml(std::ostream& os) const;

private:
	CWebDiagnosticsCommonInfoBase(const CWebDiagnosticsCommonInfoBase&);
	void operator=(const CWebDiagnosticsCommonInfoBase&);
};

/// <summary>
/// Common info for display on web diagnostics pages.
/// </summary>
class CWebDiagnosticsCommonInfo : public CWebDiagnosticsCommonInfoBase
{
public:
	/// <summary>
	/// Constructor. Initializes a new instance of the <see cref="CWebDiagnosticsCommonInfo"/> class.
	/// </summary>
	/// <param name="startupTime">IN - local time that the Service Window UI was last restarted.</param>
	/// <param name="currentTime">IN - The current local time.</param>
	/// <param name="slotMastID">IN - The Slot Mast identifier.</param>
	/// <param name="machineNumber">IN - The machine number.</param>
	/// <param name="sentinelVersion">IN - The Sentinel.exe version.</param>
	/// <param name="osImageVersion">IN - The nCompass-specific OS image version.</param>
	/// <param name="activeSkinInfo">IN - The active skin info</param>
	/// <param name="pendingSkinInfo">IN - The pending skin info</param>
	/// <param name="activeCampaignInfo">IN - The active campaign info</param>
	/// <param name="pendingCampaignInfo">IN - The pending campaign info</param>
	/// <param name="availablePhysicalMemory">IN - The available physical memory.</param>
	/// <param name="pollerPacketSentVerifyErrors">IN - The poller packet sent verify errors.</param>
	/// <param name="pollerPacketVersion">IN - The poller packet version.</param>
	/// <param name="onlineStateStr">IN - The online state string.</param>
	/// <param name="carrierVersion">IN - The carrier version.</param> 
	/// <param name="vmVersion">IN - The video mixer 
	/// <param name="batteryStatus">IN - The battery status
	/// <param name="lCPUtemp">IN - The CPU temperature
	/// version.</param>
	CWebDiagnosticsCommonInfo(time_t startupTime,
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
		const std::string& onlineStateStr,
		const std::string &carrierVersion,
		const std::string &vmVersion,
		const std::string &batteryStatus,
		const long lCPUtemp);

	DWORD m_availablePhysicalMemory;
	std::string m_pollerIP;
	DWORD m_pollerPacketSentVerifyErrors;
	WORD m_pollerPacketVersion;
	std::string m_onlineStateStr;

	///	<summary>
	/// Writes HTML.
	///	</summary>
	/// <param name="os">The ostream to write to.</param>
	///	<remarks>
	/// This is implemented in ProcessDump\WebDiagnosticsCardReader.cpp
	///	</remarks>
	void WriteHtml(std::ostream& os) const;

private:
	CWebDiagnosticsCommonInfo(const CWebDiagnosticsCommonInfo&);
	void operator=(const CWebDiagnosticsCommonInfo&);
};
