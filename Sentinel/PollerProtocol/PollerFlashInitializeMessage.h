#pragma once
#include "PollerMessage.h"

#define IPADDRESS_SIZE 16
#define FLASHINIT_KEYLENGTH_REQ 24 // MUST be exactly 24 bytes (192 bits) for Triple DES.

typedef struct _flashInit
{
	DWORD sessionID;
	DWORD blockCount;
	DWORD checksum;
	byte type;
	byte autoBurn;
	WORD port;
	DWORD fileSize;
	DWORD keyLength; // 2009-10-08  Always FLASHINIT_KEYLENGTH_REQ, once initialized.
	byte* sessionKey; // 2009-10-08  Array is all zeros if invalid.
	DWORD ipAddress;
} FlashInitializer;

class CPollerFlashInitializeMessage :
	public CPollerMessage
{
public:
	CPollerFlashInitializeMessage(byte *response, WORD packetVersion, WORD dataLength );
public:
	virtual ~CPollerFlashInitializeMessage(void);
	DWORD GetNumberOfBlocks();
	DWORD GetFileSize();
	byte GetType();
	byte GetAutoBurn();
	DWORD GetSessionID();
	DWORD GetIP();
	WORD GetPort();
	DWORD GetChecksum();
	byte *GetSessionKey();
	DWORD GetSessionKeyLength();

private:

	FlashInitializer m_initializer;

};

