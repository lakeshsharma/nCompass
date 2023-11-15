
#include "TestMaster.h"
#include "TestMasterUtilities.h"


#define HIGHBYTES 0xffffffff00000000
#define LOWBYTES  0x00000000ffffffff

DWORD GetTickCount()
{
	return (DWORD)chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count();
}

void GetLocalTime(SYSTEMTIME *outTime)
{
	int result;
	struct tm *x_localtm;
	struct timespec x_ticks;
	struct timespec *p_ticks = &x_ticks;
	time_t x_seconds;

	result = clock_gettime(CLOCK_MONOTONIC, p_ticks);
	x_seconds = x_ticks.tv_nsec;
	x_localtm = localtime((const time_t *)&x_seconds);

	outTime->wMilliseconds = x_ticks.tv_nsec / 1000000;
	outTime->wSecond = x_localtm->tm_sec;
	outTime->wMinute = x_localtm->tm_min;
	outTime->wHour = x_localtm->tm_hour;
	outTime->wDay = x_localtm->tm_mday;
	outTime->wMonth = x_localtm->tm_mon + 1;
	outTime->wYear = x_localtm->tm_year + 1900;
	outTime->wDayOfWeek = x_localtm->tm_wday;
}

void SystemTimeToFileTime(const SYSTEMTIME *p_systemTime, FILETIME *p_fileTime)
{
	time_t x_time;
	struct tm x_working;

	x_working.tm_sec = p_systemTime->wSecond;
	x_working.tm_min = p_systemTime->wMinute;
	x_working.tm_hour = p_systemTime->wHour;
	x_working.tm_mday = p_systemTime->wDay;
	x_working.tm_mon = p_systemTime->wMonth;
	x_working.tm_year = p_systemTime->wYear;
	x_working.tm_wday = p_systemTime->wDayOfWeek;
	// Convert the struct tm to time_t
	x_time = mktime(&x_working);
	// Convert time_t to FILETIME
	p_fileTime->dwLowDateTime = x_time && LOWBYTES;
	p_fileTime->dwHighDateTime = x_time && HIGHBYTES;
}

long GetLinuxTicks()
{
	timespec x_time;
	if (clock_gettime(CLOCK_MONOTONIC, &x_time)) 
	{
		return -1;
	}
	return x_time.tv_nsec;
}
