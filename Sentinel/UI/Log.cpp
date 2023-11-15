#include "stdafx.h"
#include "Log.h"
#include "../Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLog::CLog(void) :
	m_dateTime(0)
{
	
}

CLog::~CLog(void)
{
}

std::string CLog::GetDateTimeAsString( void ) const
{
	return (CUtilities::FormatDateTimeString(m_dateTime, "%m/%d/%Y %I:%M%p"));
}

time_t CLog::GetDateTime( void ) const
{
	return (m_dateTime);
}

