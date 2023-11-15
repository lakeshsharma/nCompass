#pragma once
#include "PollerMessage.h"

#define FLASH_STATUS_MESSAGE_LENGTH 44
#define SESSIONID_LOCATION 0
#define BLOCKARRAY_LOCATION 4
#define BLOCKARRAY_SIZE 10

class CPollerFlashStatusMessage :
	public CPollerMessage
{
public:
	CPollerFlashStatusMessage(byte *response);
public:
	virtual ~CPollerFlashStatusMessage(void);
	DWORD GetSessionID();
	DWORD GetBlockElement(DWORD element);
};

