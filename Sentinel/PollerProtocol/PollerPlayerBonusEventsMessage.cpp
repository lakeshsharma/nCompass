#include "stdafx.h"

#include "PollerPlayerBonusEventsMessage.h"
#include "../GlobalDefs.h"
#include "Logging/LogString.h"
#include "MultiplierBonusEvent.h"

CPollerPlayerBonusEventsMessage::CPollerPlayerBonusEventsMessage(byte *response)
{
	//build CBonusEvents from poller response
	m_lpBuffer = NULL;
	byte bonusCount;
	int offset = 0;

	m_events.SetCardNumber(response,CARD_ID_LEN);
	offset += CARD_ID_LEN;

	memcpy_s(&bonusCount, sizeof(bonusCount),response + offset, sizeof(bonusCount));
	offset += sizeof(bonusCount);

	for (int i = 0; i < bonusCount; i++)
	{
		int eventId;
		memcpy_s(&eventId, sizeof(eventId), response + offset, sizeof(eventId));
		offset += sizeof(eventId);
		
		CBonusEvent *BEvent;
		WORD bonusData;
		SYSTEMTIME startTime, stopTime;

		memcpy_s(&startTime, sizeof(startTime), response + offset, sizeof(startTime));
		offset += sizeof(startTime);

		memcpy_s(&stopTime, sizeof(stopTime), response + offset, sizeof(stopTime));
		offset += sizeof(stopTime);

		byte isMultiplier;
		memcpy_s(&isMultiplier, sizeof(isMultiplier), response + offset, sizeof(isMultiplier));
		offset += sizeof(isMultiplier);

		memcpy_s(&bonusData, sizeof(bonusData), response + offset, sizeof(bonusData));
		offset += sizeof(bonusData);
		
		//When a one time bonus event arrives with no time, it is 
		//actually from now until the bonus is awarded.
		//The check is if the start or end times are less a year old.
		//If so, set the start time as now, and the end time as some time in the future.
		SYSTEMTIME currentTime;
		CUtilities::GetLocalSystemTime(&currentTime);
		if (startTime.wYear < currentTime.wYear)
		{
			CUtilities::GetLocalSystemTime(&startTime);
			stopTime = startTime;
			if (startTime.wYear > 1971) //we don't want to break CTime
			{
				startTime.wYear-=1;
			}
			stopTime.wYear+= 5;
		}


		// FILETIME startFileTime, stopFileTime;
		
		//the system time is UTC.. but CTime only takes utc from a filetime
		/*SystemTimeToFileTime(&startTime, &startFileTime);
		SystemTimeToFileTime(&stopTime, &stopFileTime);*/

		if (isMultiplier == 1)
		{
			LogString(GENERAL, "Multiplier Event Start %d/%d/%d  %d:%d.",startTime.wMonth, startTime.wDay, startTime.wYear, startTime.wHour, startTime.wMinute);
			LogString(GENERAL, "Multiplier Event End %d/%d/%d  %d:%d.",stopTime.wMonth, stopTime.wDay, stopTime.wYear, stopTime.wHour, stopTime.wMinute);
			BEvent = new CMultiplierBonusEvent(eventId, CUtilities::GetTimeFromUTCSystemTime(startTime), CUtilities::GetTimeFromUTCSystemTime(stopTime), bonusData);
		}
		else
		{
			LogString(GENERAL, "One Time Event Start %d/%d/%d  %d:%d.",startTime.wMonth, startTime.wDay, startTime.wYear, startTime.wHour, startTime.wMinute);
			LogString(GENERAL, "One Time Event End %d/%d/%d  %d:%d.",stopTime.wMonth, stopTime.wDay, stopTime.wYear, stopTime.wHour, stopTime.wMinute);
			BEvent = new COneTimeBonusEvent(eventId, CUtilities::GetTimeFromUTCSystemTime(startTime), CUtilities::GetTimeFromUTCSystemTime(stopTime), bonusData);
		}

		m_events.AddEvent(BEvent);
	}

}

CPollerPlayerBonusEventsMessage::~CPollerPlayerBonusEventsMessage(void)
{
}

CBonusEvents CPollerPlayerBonusEventsMessage::GetBonusEvents()
{
	return m_events;
}

