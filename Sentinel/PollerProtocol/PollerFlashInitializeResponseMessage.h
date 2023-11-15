#pragma once
#include "PollerMessage.h"

#define FLASH_INITIALIZE_RESPONSE_MESSAGE_LENGTH 27

class CPollerFlashInitializeResponseMessage :
	public CPollerMessage
{
public:
	CPollerFlashInitializeResponseMessage();
public:
	virtual ~CPollerFlashInitializeResponseMessage(void);
	void SetSessionID(DWORD sessoinId);
	byte * GetReadWriteBuffer( long &bufferSize );
	
	void SetStatus(byte status)
	{
		m_status = status;
	}

private:
	DWORD m_sessionID;
	byte m_status;
	void SetReadWriteBuffer();

};

