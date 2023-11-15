#pragma once
#include "PollerMessage.h"

#define FLASH_DATA_BLOCK_MESSAGE_LENGTH 1408
#define FLASH_DATA_BLOCK_SESSIONID_LOCATION 0
#define FLASH_BLOCK_NUMBER_LOCATION 4
#define FLASH_DATA_LOCATION 8

class CPollerFlashDataBlockMessage :
	public CPollerMessage
{
public:
	CPollerFlashDataBlockMessage(byte *response);
public:
	~CPollerFlashDataBlockMessage(void);
	DWORD GetSessionID();
	DWORD GetBlockNumber();
	byte *GetDataBlock();
};
