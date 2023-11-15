#pragma once
#include "PollerMessage.h"
#include "../BonusEvents.h"
#include "../OneTimeBonusEvent.h"
#include "../MultiplierBonusEvent.h"

class CPollerPlayerBonusEventsMessage :
	public CPollerMessage
{
public:
	CPollerPlayerBonusEventsMessage(byte *response);

	CBonusEvents GetBonusEvents();

public:
	~CPollerPlayerBonusEventsMessage(void);

private:
	CBonusEvents m_events;
};
