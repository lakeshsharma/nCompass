#pragma once
#include "PollerMessage.h"


typedef struct 
{
	DWORD slotMastID;
	byte Response;
}ChangeSlotMastResp;

class CPollerChangeSlotMastIDRespMessage :
	public CPollerMessage
{
public:
	CPollerChangeSlotMastIDRespMessage(void);
public:
	~CPollerChangeSlotMastIDRespMessage(void);

	SlotmastIDResponse SlotMastChangeResponse( void );
	DWORD GetSlotMastID( void );

};
