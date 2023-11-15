#pragma once

const int MAXIMUM_RUNAWAY = 20;    //the number of changes per timeout
const int RUNAWAY_METER_TIME = 1;  //minute
const int RUNAWAY_DOWN_TIME = 10;  //minutes before reset.

class CRunawayItem
{
public:
	CRunawayItem(void);
	~CRunawayItem(void);
public:
	int GetCount();
	void ResetCount();
	void IncrementCount();
	void ResetTime();
	DWORD GetMinutesSpan();
	void CheckTimer();
private:
	DWORD m_initialTick;
	int m_count;
};


typedef enum 
{
	RUNAWAY_ACTION_SEND,
	RUNAWAY_ACTION_DONT_SEND,
	RUNAWAY_ACTION_SEND_RUNAWAY,
} RunawayAction;

//typedef CMap<WORD, WORD, CRunawayItem, CRunawayItem&> RunawayMeterMap;
typedef std::map<WORD,CRunawayItem> RunawayMeterMap;


class CRunawayStatus
{
public:
	CRunawayStatus(void);
public:
	~CRunawayStatus(void);

public:
	RunawayAction GetSendAction(WORD meter);

private:

	RunawayMeterMap m_runawayItemList;

};

