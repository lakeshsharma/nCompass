#include "stdafx.h"
#include "CountHistory.h"
#include "Utilities.h"

using namespace std;


CountHistory::Data::Data(unsigned count, unsigned milliseconds, bool isSet)
	: m_count(count)
	, m_milliseconds(milliseconds)
	, m_isSet(isSet)
{
}

bool CountHistory::Data::IsSet() const
{
	return m_isSet;
}

unsigned CountHistory::Data::GetCount() const
{
	return m_count;
}

unsigned CountHistory::Data::GetMilliseconds() const
{
	return m_milliseconds;
}

bool CountHistory::Data::operator==(const Data& other) const
{
	return m_count == other.m_count && 
		m_milliseconds == other.m_milliseconds && 
		m_isSet == other.m_isSet;
}

CountHistory::CountHistory()
{
	ZeroMemory(&m_lastSetTime, sizeof(m_lastSetTime));
	ZeroMemory(&m_lastResetTime, sizeof(m_lastResetTime));
	ZeroMemory(&m_lastRolledTime, sizeof(m_lastRolledTime));
	ZeroMemory(&m_lastIntervalTime, sizeof(m_lastIntervalTime));
}

CountHistory::CountHistory(const CountHistory& other)
{
	*this = other;
}

CountHistory::~CountHistory()
{
}

CountHistory& CountHistory::operator=(const CountHistory& other)
{
	if (this != &other)
	{
		m_today = other.m_today;
		m_yesterday = other.m_yesterday;
		m_lastSetTime = other.m_lastSetTime;
		m_lastResetTime = other.m_lastResetTime;
		m_lastRolledTime = other.m_lastRolledTime;
		m_lastIntervalTime = other.m_lastIntervalTime;
	}
	return *this;
}

bool CountHistory::IsLastSetTimeValid() const
{
	bool l_LstTime = m_lastSetTime.wYear != 0;
	return  l_LstTime;
}

SYSTEMTIME CountHistory::GetLastSetTime() const
{
	return m_lastSetTime;
}

bool CountHistory::IsLastResetTimeValid() const
{
	return m_lastResetTime.wYear != 0;
}

SYSTEMTIME CountHistory::GetLastResetTime() const
{
	return m_lastResetTime;
}

void CountHistory::GetData(Data* pToday, Data* pYesterday, const SYSTEMTIME* pTime)
 {
	SYSTEMTIME time;
	if (pTime == nullptr)
	{
		CUtilities::GetLocalSystemTime(&time);
		pTime = &time;
	}

	CheckRoll(*pTime);

	GetDataNoRoll(pToday, pYesterday, pTime);
}

void CountHistory::GetDataNoRoll(Data* pToday, Data* pYesterday, const SYSTEMTIME* pTime) const
{
	SYSTEMTIME time;
	if (pTime == nullptr)
	{
		CUtilities::GetLocalSystemTime(&time);
		pTime = &time;
	}

	if (pToday)
	{
		*pToday = m_today;

		if (m_today.IsSet())
		{
			int64_t diffTime = CUtilities::GetSystemTimespanInMilliseconds(
					m_lastIntervalTime.wYear == 0 ? m_lastSetTime : m_lastIntervalTime, *pTime);

			if (diffTime > 0)
				pToday->m_milliseconds += diffTime;
		}
	}

	if (pYesterday)
		*pYesterday = m_yesterday;
}

void CountHistory::CheckRoll(const SYSTEMTIME& time)
{
	if (m_lastRolledTime.wYear == 0)
	{
		m_lastRolledTime = time;
	}
	else
	{
		if (time.wDay != m_lastRolledTime.wDay)
		{
			m_lastRolledTime = time;

			if (m_today.IsSet())
			{
				SYSTEMTIME intervalTime(time);
				intervalTime.wHour = 0;
				intervalTime.wMinute = 0;
				intervalTime.wSecond = 0;
				intervalTime.wMilliseconds = 0;

				int64_t setTime = CUtilities::GetSystemTimespanInMilliseconds(m_lastIntervalTime.wYear == 0 ?
					m_lastSetTime : m_lastIntervalTime, intervalTime);

				if (setTime > 0)
					m_today.m_milliseconds += (unsigned) setTime;

				m_lastIntervalTime = intervalTime;
			}

			m_yesterday = m_today;

			m_today.m_isSet = m_yesterday.IsSet();
			m_today.m_count = 0;
			m_today.m_milliseconds = 0;
		}
	}
}

void CountHistory::Set(const SYSTEMTIME* pTime)
{
	SYSTEMTIME time;
	if (pTime == nullptr)
	{
		CUtilities::GetLocalSystemTime(&time);
		pTime = &time;
	}

	CheckRoll(*pTime);

	if (!m_today.IsSet())
	{
		m_today.m_isSet = true;
		m_today.m_count++;
		m_lastSetTime = *pTime;
		ZeroMemory(&m_lastIntervalTime, sizeof(m_lastIntervalTime));
     
	}
}

void CountHistory::Set(bool set, const SYSTEMTIME* pTime)
{
	if (set)
	{
		Set(pTime);
	}
	else
	{
		Reset(pTime);
	}
}

void CountHistory::Reset(const SYSTEMTIME* pTime)
{
	SYSTEMTIME time;
	if (pTime == nullptr)
	{
      CUtilities::GetLocalSystemTime(&time);
		pTime = &time;
	}

	CheckRoll(*pTime);

	if (m_today.IsSet())
	{
		m_today.m_isSet = false;
		m_lastResetTime = *pTime;

		if (m_lastIntervalTime.wYear == 0)
		{
            int64_t diff = CUtilities::GetSystemTimespanInMilliseconds(m_lastSetTime, m_lastResetTime);
			if (diff > 0)
				m_today.m_milliseconds += diff;
		}
		else
		{
			int64_t diff = CUtilities::GetSystemTimespanInMilliseconds(m_lastResetTime, m_lastIntervalTime);
			if (diff > 0)
				m_today.m_milliseconds += diff;

			ZeroMemory(&m_lastIntervalTime, sizeof(m_lastIntervalTime));
		}
	}
}

void CountHistory::Set(int day, unsigned count, unsigned milliseconds, bool isSet)
{
	switch (day)
	{
		case 0:
			m_today.m_count = count;
			m_today.m_milliseconds = milliseconds;
			m_today.m_isSet = isSet;
			break;

		case 1:
			m_yesterday.m_count = count;
			m_yesterday.m_milliseconds = milliseconds;
			m_yesterday.m_isSet = isSet;
			break;
	}
}

unsigned CountHistory::operator[](int day) const
{
	unsigned count;

	switch (day)
	{
		case 0:
			count = m_today.GetCount();
			break;

		case 1:
			count = m_yesterday.GetCount();
			break;

		default:
			count = 0;
			break;
	}

	return count;
}
