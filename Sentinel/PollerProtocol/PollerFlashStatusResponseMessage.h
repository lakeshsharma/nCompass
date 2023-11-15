#pragma once
#include "PollerMessage.h"

#define FLASH_STATUS_RESPONSE_MESSAGE_LENGTH 72
#define SESSIONID_LOCATION 0
#define BLOCKARRAY_LOCATION 4
#define ARRAYSIZE_LOCATION 48

#define FLASH_STATUS_BAD_SESSIONID 1

#define FLASH_BLOCKARRAY_SIZE 10

class CPollerFlashStatusResponseMessage :
	public CPollerMessage
{
public:
	CPollerFlashStatusResponseMessage();
public:
	virtual ~CPollerFlashStatusResponseMessage(void);
	void SetSessionID(DWORD sessoinId);
	void SetStatus(byte status);
	void AddBlockElement(DWORD blockNumber);
	byte GetBlockCount();
	byte * GetReadWriteBuffer( long &bufferSize );
	void SetNextBlock(DWORD nextBlock);
	DWORD GetNextBlock();

private:
	DWORD m_sessionID;
	byte m_elementCount;
	byte m_status;  // 0 success; 2 bad sessionid
	DWORD m_nextBlock;
	DWORD m_blockArray[FLASH_BLOCKARRAY_SIZE];
	void SetReadWriteBuffer();

};

