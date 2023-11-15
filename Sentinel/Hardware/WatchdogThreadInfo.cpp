#include "stdafx.h"
#include "WatchdogThreadInfo.h"

// Sentinel includes.
#include "GlobalDefs.h"

#ifdef _DEBUG
//TODO #define new DEBUG_NEW
#endif

using namespace std;

CWatchdogThreadInfo::CWatchdogThreadInfo()
	:
	m_checkInTime(0),
	m_timeOutMiliSeconds(0),
	m_suspended(false)
{
}

CWatchdogThreadInfo::CWatchdogThreadInfo(const std::string &name, DWORD timeOutMiliSeconds, DWORD tickCount, pthread_t nativeHandle)
	:
	m_checkInTime(0),
	m_timeOutMiliSeconds(timeOutMiliSeconds),
	m_name(name),
	m_suspended(false),
    m_nativeHandle(nativeHandle)
{
	CheckIn(tickCount);
}

DWORD CWatchdogThreadInfo::GetCheckInTime() const
{
	return m_checkInTime;
}

std::string CWatchdogThreadInfo::GetName() const
{
	return m_name;
}

DWORD CWatchdogThreadInfo::GetTimeOutMilliseconds() const
{
	return m_timeOutMiliSeconds;
}

void CWatchdogThreadInfo::CheckIn(DWORD tickCount)
{
	m_checkInTime = tickCount;
}

bool CWatchdogThreadInfo::IsSuspended() const
{
	return m_suspended;
}

void CWatchdogThreadInfo::Resume(DWORD tickCount)
{
	m_suspended = false;
	CheckIn(tickCount);
}

void CWatchdogThreadInfo::Suspend()
{
	m_suspended = true;
}

bool CWatchdogThreadInfo::HasTimedOut(DWORD tickCount) const
{
	bool hasTimedOut = true;

	if (IsSuspended() || tickCount - m_checkInTime < m_timeOutMiliSeconds)
	{
		hasTimedOut = false;
	}

	return hasTimedOut;
}

void CWatchdogThreadInfo::SetExceptionInformation(const std::string& exceptionType, const std::string& exceptionStackTrace)
{
    m_exception_type = exceptionType;
    m_exception_stack_trace = exceptionStackTrace;
}

std::string CWatchdogThreadInfo::GetExceptionType()
{
    return m_exception_type;
}
std::string CWatchdogThreadInfo::GetExceptionStackTrace()
{
    return m_exception_stack_trace;
}

std::string CWatchdogThreadInfo::GetThreadName()
{
    return m_name;
}

pthread_t CWatchdogThreadInfo::GetNativeHandle()
{
    return m_nativeHandle;
}

