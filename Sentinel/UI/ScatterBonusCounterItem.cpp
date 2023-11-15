#include "stdafx.h"
#include "ScatterBonusCounterItem.h"
#include "../Utilities.h"

using namespace std;

CScatterBonusCounterItem::CScatterBonusCounterItem(IOperatingSystem* operatingSystem,
	long long itemId, const string& levelName, time_t countdownTimerEnd, WORD priorityLevel) :
	CBonusCounterItem(operatingSystem, BonusCounterType::ScatterTimer, itemId, priorityLevel, CBonusCounterItem::DEFAULT_DISPLAY_TIME),
	m_levelName(levelName),
	m_timer(CUtilities::GetCurrentTime()),//1 second timer for countdown timer
	m_countdownTimerEnd(countdownTimerEnd),
	m_remainingDuration(countdownTimerEnd - m_timer)
{
}

CScatterBonusCounterItem::~CScatterBonusCounterItem()
{
}

CScatterBonusCounterItem::CScatterBonusCounterItem(const CScatterBonusCounterItem &rhs) : CBonusCounterItem(rhs)
{
	m_levelName = rhs.m_levelName;
	m_timer = rhs.m_timer;
	m_countdownTimerEnd = rhs.m_countdownTimerEnd;
	m_remainingDuration = rhs.m_remainingDuration;
}

CScatterBonusCounterItem & CScatterBonusCounterItem::operator = (const CScatterBonusCounterItem &rhs)
{
	CBonusCounterItem::operator =(rhs);
	m_levelName = rhs.m_levelName;
	m_timer = rhs.m_timer;
	m_countdownTimerEnd = rhs.m_countdownTimerEnd;
	m_remainingDuration = rhs.m_remainingDuration;

	return *this;
}

string CScatterBonusCounterItem::GetParameters() const
{
	int hours((int)m_remainingDuration / (MINUTES_PER_HOUR * SECONDS_PER_MINUTE));
	int minutes(((int)m_remainingDuration % (MINUTES_PER_HOUR * SECONDS_PER_MINUTE)) / SECONDS_PER_MINUTE);
	int seconds((int)m_remainingDuration % SECONDS_PER_MINUTE);
	return FormatString("timeString=\"%02d:%02d:%02d\" percentage=\"%s\"", hours, minutes, seconds, m_levelName.c_str());
}

bool CScatterBonusCounterItem::DisplayOnAddOrUpdate(CBonusCounterItem *currentlyDisplayedItem)
{
	// Set to display if no item currently displayed OR this is the currently displayed item OR
	//   this item has a higher priority than the currently displayed item
	return (NULL == currentlyDisplayedItem || *this == *currentlyDisplayedItem || *this < *currentlyDisplayedItem);
}

bool CScatterBonusCounterItem::CheckTimedEvent(time_t timer)
{
	CheckForSplashdownCountdown(timer);

	return CBonusCounterItem::CheckTimedEvent(timer);
}

bool CScatterBonusCounterItem::CheckForSplashdownCountdown(time_t timer)
{
	bool needToUpdate(false);

	//this handles the countdown timer if we are in a splashdown event countdown
	if (timer >= m_timer || m_timer - timer >= 3)
	{
		needToUpdate = true;

		time_t span = m_countdownTimerEnd - CUtilities::GetCurrentTime();
		if (span >= 0) //This so we do not paint any negative numbers
		{
			m_remainingDuration = span;
		}
		else
		{
			m_remainingDuration = 0;
		}
		m_timer = timer + 1;    // 1 second later
	}

	return needToUpdate;
}