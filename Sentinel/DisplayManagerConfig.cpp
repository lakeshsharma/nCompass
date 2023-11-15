#include "stdafx.h"

#include "DisplayManagerConfig.h"
#include "Hardware/NonVolatileMemoryMaps.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDisplayManagerConfig::CDisplayManagerConfig(void): 
	CMemoryStorage ()
{
	m_dwOffset = 0;
	m_type = MEMORY_NVRAM;
	m_dwAllowedSize = NVRAM_MONITOR_RESOLUTION_SIZE + NVRAM_MONITOR_RESOLUTION_CHECK_SIZE;
	m_bOffsetIsSet = true;

	BuildYourself();
}

CDisplayManagerConfig::~CDisplayManagerConfig(void)
{
}

// Returns true if resolution was set.
// Returns false if resolution was not set.
bool CDisplayManagerConfig::GetMonitorResolution(SIZE& resolution) const
{
	if (m_monitorResolutionValid)
	{
		resolution = m_monitorResolution;
	}

	return m_monitorResolutionValid;
}

// See NVRAM_MONITOR_RESOLUTION... in Sentinel\Hardware\NonVolatileMemoryMaps.h
// for information about the data format used in persistent memory.
void CDisplayManagerConfig::SetMonitorResolution(SIZE const &resolution)
{
	if (m_monitorResolution.cx != resolution.cx || m_monitorResolution.cy != resolution.cy || !m_monitorResolutionValid)
	{
		m_monitorResolution = resolution;
		m_monitorResolutionValid = true;

		// DWORD used [...] CSize. The low-order word is the cx member and the high-order word is the cy member.
		DWORD dwSize = ((DWORD)resolution.cx & 0xffff) | ((DWORD)resolution.cy << 16);
		Write(NVRAM_MONITOR_RESOLUTION_OFFSET, (BYTE *)&dwSize, NVRAM_MONITOR_RESOLUTION_SIZE);

		DWORD dwCheck = ~dwSize;
		Write(NVRAM_MONITOR_RESOLUTION_CHECK_OFFSET, (BYTE *)&dwCheck, NVRAM_MONITOR_RESOLUTION_CHECK_SIZE);
	}
} 

// See NVRM_MONITOR_RESOLUTION... Sentinel\Hardware\NonVolatileMemoryMaps.h
// for information about the data format used in persistent memory.
void CDisplayManagerConfig::BuildYourself()
{
	DWORD dwSize;
	DWORD dwCheck;

	Read(NVRAM_MONITOR_RESOLUTION_OFFSET, (BYTE *)&dwSize, NVRAM_MONITOR_RESOLUTION_SIZE);
	Read(NVRAM_MONITOR_RESOLUTION_CHECK_OFFSET, (BYTE *)&dwCheck, NVRAM_MONITOR_RESOLUTION_CHECK_SIZE);
	
	// DWORD used to initialize CSize. The low-order word is the cx member and the high-order word is the cy member.
	m_monitorResolution.cx = (long)(dwSize & 0xff); 
	m_monitorResolution.cy = (long)((dwSize >> 16) & 0xff);
	m_monitorResolutionValid = dwCheck == ~dwSize;
}

