#include "stdafx.h"
#include "BadReadCount.h"
#include "Utilities.h"

CBadReadCount::CBadReadCount(void)
{
	m_nextBadCardReaderReminderTick = 0;
	m_remainingBadCardReadReminders = BAD_CARD_READ_MAX_REMINDERS;
	m_badCardReadCount = 0;
}


CBadReadCount::~CBadReadCount(void)
{
}

void CBadReadCount::IncBadReadCount( )
{
	m_badCardReadCount++;
}

DWORD CBadReadCount::GetBadReadCount( )
{
	return m_badCardReadCount;
}

void CBadReadCount::SetBadCardReadReminderTick( DWORD ticks )
{
	m_nextBadCardReaderReminderTick = ticks;
}

void CBadReadCount::DecBadCardReadReminders( )
{
	m_remainingBadCardReadReminders--;
}

int CBadReadCount::GetBadCardReadReminders( )
{
	return m_remainingBadCardReadReminders;
}

void CBadReadCount::SetBadCardReadReminders( int count )
{
	m_remainingBadCardReadReminders = count;
}
	
void CBadReadCount::ResetBadReadCount( )
{
	m_badCardReadCount = 0;
}

bool CBadReadCount::IsBadCardReminderRequired( )
{
	return m_remainingBadCardReadReminders > 0 && CUtilities::GetTickCount() >= m_nextBadCardReaderReminderTick;
}

bool CBadReadCount::IsTooManyBadCardReads( )
{
	return m_badCardReadCount >= BAD_CARD_READ_THREASHOLD;
}
