#include "stdafx.h"
#include "Process.h"

CProcess::CProcess(void) :
	m_processId(0),
	m_memUsage(0),
	m_vmSize(0),
	m_moduleSize(0),
	m_totalProcTime(0),
	m_prevProcTime(0),
	m_currentActive(false),
	m_cpuUsage(0),
	m_peakCpuUsage(0),
	m_handleCount(0)
{
}

CProcess::CProcess(const CProcess &rhs)
{
	*this = rhs;
}

CProcess::~CProcess(void)
{
}

CProcess& CProcess::operator=(const CProcess &rhs)
{
	m_processName = rhs.m_processName;
	m_processId = rhs.m_processId;
	m_memUsage = rhs.m_memUsage;
	m_vmSize = rhs.m_vmSize;
	m_moduleSize = rhs.m_moduleSize;
	m_totalProcTime = rhs.m_totalProcTime;
	m_prevProcTime = rhs.m_prevProcTime;
	m_currentActive = rhs.m_currentActive;
	m_cpuUsage = rhs.m_cpuUsage;
	m_peakCpuUsage = rhs.m_peakCpuUsage;
	m_handleCount = rhs.m_handleCount;

	return *this;
}
