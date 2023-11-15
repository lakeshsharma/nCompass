#include "stdafx.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "RunawayStatus.h"
#include "Utilities.h"

CRunawayStatus::CRunawayStatus(void)
{
	//The only function that should be called is CRunawayStatus::GetSendAction
	//Every meter that is checked for runaway, gets added to the meter map.
	//if the meter gets checked more than x times in n minutes, then getsendaction
	//will first tell the caller to send a runaway transaction. Subsequent checks
	//will tell the caller not to send a transaction. After y minutes of not sending transactions
	//the meter runaway state resets, and the call will again allow a transaction send.
}

CRunawayStatus::~CRunawayStatus(void)
{

}

RunawayAction CRunawayStatus::GetSendAction(WORD meter)
{
	CRunawayItem runawayItem;

	//get the item
	auto runawayItemIt = m_runawayItemList.find(meter);
	if (runawayItemIt != m_runawayItemList.end())
	{
		runawayItem = runawayItemIt->second;
	}
	else
	{
		//if it wasn't found, initialise it
		runawayItem.ResetTime();	
		runawayItem.ResetCount();	
	}

	//check if the timer or count needs reseting
	runawayItem.CheckTimer();

	runawayItem.IncrementCount();

	RunawayAction retVal;

	if (runawayItem.GetCount() == MAXIMUM_RUNAWAY)
	{
		retVal = RUNAWAY_ACTION_SEND_RUNAWAY;	//send a meter runaway transaction
	}
	else if (runawayItem.GetCount() < MAXIMUM_RUNAWAY)
	{
		retVal = RUNAWAY_ACTION_SEND;			//it's ok to send the transaction
	}
	else // runawayItem.getcount > MAXIMUM_RUNAWAY
	{
		retVal = RUNAWAY_ACTION_DONT_SEND;		//send nothing
	}

	m_runawayItemList[meter] = runawayItem;

	return retVal;
}


//////////////////////
///RunawayItem Class
//////////////////////

CRunawayItem::CRunawayItem(void)
{
	ResetCount();
}

CRunawayItem::~CRunawayItem(void)
{
}

int CRunawayItem::GetCount()
{
	return m_count;
}

void CRunawayItem::ResetCount()
{
	m_count = 0;
}

void CRunawayItem::IncrementCount()
{
	m_count++;
}

DWORD CRunawayItem::GetMinutesSpan()
{
	return ((CUtilities::GetTickCount() - m_initialTick)/MILLISECONDS_PER_MINUTE);
}

void CRunawayItem::ResetTime()
{
	m_initialTick = CUtilities::GetTickCount();
}

void CRunawayItem::CheckTimer()
{
	if ((GetMinutesSpan() >= RUNAWAY_METER_TIME) && (GetCount() < MAXIMUM_RUNAWAY))
	{
		//it's been over a minute and we haven't exceeded the runaway,
		//so we can reset the time and the count
		ResetTime();
		ResetCount();
	}
	if (GetMinutesSpan() >= RUNAWAY_DOWN_TIME)
	{
		//The "do not send" timeout has expired, allow sends again
		ResetTime();
		ResetCount();
	}

}
