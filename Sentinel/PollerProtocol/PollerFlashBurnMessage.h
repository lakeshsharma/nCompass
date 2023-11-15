#pragma once
#include "PollerMessage.h"

#define FLASH_BURN_MESSAGE_LENGTH 5
#define SESSIONID_LOCATION 0
#define FLASH_BURN_LOCATION 4

class CPollerFlashBurnMessage :
	public CPollerMessage
{
public:
	CPollerFlashBurnMessage(byte *response);
public:
	~CPollerFlashBurnMessage(void);

public:
	DWORD GetSessionID();
	byte GetBurnClear();
};
