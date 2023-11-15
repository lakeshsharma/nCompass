#include "stdafx.h"
#include "MemoryThresholds.h"

// Sentinel includes.
#include "Logging/LogString.h"
#include "Utilities.h"

using namespace std;

#ifdef _DEBUG	
const DWORD MinimumTickCountForRebootASAP = 600000;
#else
const DWORD MinimumTickCountForRebootASAP = 14400000; // 14400000 ms == 4 hours.
#endif

CMemoryThresholds::CMemoryThresholds(DWORD startupTickCount) :
	m_startupTickCount(startupTickCount),
	m_mininumRebootTimeExpired(false)
{
}

CMemoryThresholds::~CMemoryThresholds()
{
}

MemoryThresholdStatusType CMemoryThresholds::GetMemoryThresholdStatus(
	bool restartUiAfterDrop, 
	bool isAbleToRestartUi,
	DWORD rebootAsapThresholdInMb, 
	DWORD rebootWarningThresholdMb, 
	DWORD restartUIAsapThresholdMb, 
	DWORD restartUIAfterDropThresholdMb, 
	DWORD currentTickCount, 
	DWORD &availablePhysicalMemory)
{
	MemoryThresholdStatusType retval = MemoryThresholdStatusOkay;
	
	DWORD rebootASAPThresholdKb(rebootAsapThresholdInMb * KBS_PER_MB);
	DWORD rebootAfterDropThresholdKb(rebootWarningThresholdMb * KBS_PER_MB);
	DWORD uiRestartAsapThresholdKb(restartUIAsapThresholdMb * KBS_PER_MB);
	DWORD uiRestartAfterDropThresholdKb(restartUIAfterDropThresholdMb * KBS_PER_MB);

	// "sysconf(_SC_AVPHYS_PAGES)" gets us "free" memory, not "available" memory, so isn't an accurate measure of whether we're low (http://man7.org/linux/man-pages/man5/proc.5.html)
	// ideally, we'd just use the "sysinfo" system call (http://man7.org/linux/man-pages/man2/sysinfo.2.html)
	// ... but it doesn't provide "cached" memory, which we have to take into account since the OS will happily reclaim cached memory when allocating memory for Sentinel
	// it appears the only accurate source of this information is "/proc/meminfo":
	//   http://lists.busybox.net/pipermail/busybox/2014-December/082110.html
	//   https://github.com/mirror/busybox/blob/b097a84d62dcfcf201a695b54bcfafae70cac8a6/procps/free.c#L55-L77
	// (busybox itself uses "sysinfo" for everything but the "cached" number, which it parses from "/proc/meminfo")
	// since Linux 3.14, "/proc/meminfo" has a field labelled "MemAvailable:", which is exactly the number we want, so since we have to parse the file anyhow, let's just grab that field directly
	// (http://man7.org/linux/man-pages/man5/proc.5.html, "/proc/meminfo")
	string memAvailable = CUtilities::GetSystemResultString("awk '$1 ~ /^MemAvailable:?$/ { if ($3 ~ /[kK][bB]?/) { print $2 } else if ($3 ~ /[mM][bB]?/) { print $2 * 1024 } else if (!$3) { print $2 / 1024 } else { exit 1 } }' /proc/meminfo");
	if (!memAvailable.empty())
	{
		try
		{
			availablePhysicalMemory = stoll(memAvailable);
		}
		catch (const invalid_argument&) {}
		catch (const out_of_range&) {}
	}
	// TODO (tianon@infosiftr.com, 2019-03-08) what should "availablePhysicalMemory" be set to if any of the above fails? (fails to awk for some reason, fails to execute for some reason, fails to stoll, etc)

	// Once this timer expires, it stays expired even if the tick count wraps past the start value.
	if (!m_mininumRebootTimeExpired)
	{
		if (currentTickCount - m_startupTickCount >= MinimumTickCountForRebootASAP)
		{
			m_mininumRebootTimeExpired = true;
		}
	}

	// To simplify code, check for easiest condition first and override as needed.


	// 1. Check criteria for a UI Restart after drop.
	// If available physical memory is less than or equal to the ui restart after drop threshold
	// then indicate to restart UI after the drop. or
	// if restartUiAfterDrop is set to true.
	if (restartUiAfterDrop || (isAbleToRestartUi && availablePhysicalMemory <= uiRestartAfterDropThresholdKb))
	{
		retval = MemoryThresholdStatusUIRestartAfterDrop;
	}

	// 2. Check criteria for a UI Restart ASAP.
	// If available physical memory is less than or equal to the ui restart asap threshold
	// then indicate to restart UI after the drop.
	if (isAbleToRestartUi && availablePhysicalMemory <= uiRestartAsapThresholdKb)
	{
		retval = MemoryThresholdStatusUIRestartAsap;
	}

	// 3. Check criteria for a reboot after drop.
	// If available physical memory is less than or equal to the reboot after drop threshold
	// then indicate to wait for a drop before rebooting.
	if (availablePhysicalMemory <= rebootAfterDropThresholdKb)
	{
		retval = MemoryThresholdStatusRebootAfterDrop;
	}

	// 4. Check criteria for a reboot ASAP.
	// If available physical memory is less than or equal to the reboot ASAP threshold then indicate to reboot ASAP
	// after ensuring that a minimum period of time has elapsed.
	// Wraparound every 49.7 days is acceptable.
	// - A reboot "ASAP" after a multiple of 49.7 days might be delayed up to the minimum period of time.
	if (availablePhysicalMemory <= rebootASAPThresholdKb)
	{
		if (m_mininumRebootTimeExpired)
		{
			retval = MemoryThresholdStatusRebootASAP;
		}
	}
 
	availablePhysicalMemory *= BYTES_PER_KB;

	return retval;
}
