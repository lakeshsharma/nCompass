#include "stdafx.h"

#include "SASPollMessage.h"
//#include "../games.h"
#include "Test9BitnCompass.h"
#include "math.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define EGM_GAME_NUMBER		0		// EGM game number is always 0

CSASPollMessage::CSASPollMessage(byte gameAddress)
{
	m_hResponseEventHandle = CreateEvent( NULL, FALSE, FALSE, NULL );
	m_bWaitForSASResponse = true;
	m_isVariableResponse = false;

	m_gameAddress = gameAddress;
	m_sendBuffer = NULL;
	m_responseBuffer = NULL;

	m_retryCount = 0;
	m_nReturnValue = 0;

}

CSASPollMessage::~CSASPollMessage(void)
{
	CloseHandle( m_hResponseEventHandle );
	if (m_sendBuffer)
		delete [] m_sendBuffer;
	if (m_responseBuffer)
		delete [] m_responseBuffer;
}

void CSASPollMessage::ResponseReceived()
{
	SetEvent( m_hResponseEventHandle );
}

void CSASPollMessage::WaitForResponse()
{
	while ( WaitForSingleObject( m_hResponseEventHandle, EVENT_WAIT_TIMEOUT ) == WAIT_TIMEOUT )
	{
		//((CTest9BitnCompassApp *)AfxGetApp())->WatchDog->Checkin(GetCurrentThreadId());	//RRN
	}
}

void CSASPollMessage::IncRetryCount()
{
	m_retryCount++;
	return;
}

int CSASPollMessage::GetRetryCount()
{
	return m_retryCount;
}

BYTE *CSASPollMessage::GetSendBuffer()
{
	return m_sendBuffer;
}

BYTE *CSASPollMessage::GetResponseBuffer()
{
	return m_responseBuffer;
}

UINT CSASPollMessage::GetResponseBufferSize()
{
	return m_responseBufferSize;
}

UINT CSASPollMessage::GetSendBufferSize()
{
	return m_sendBufferSize;
}

void CSASPollMessage::SetSendBuffer(BYTE *buffer, UINT size)
{
	m_sendBuffer = buffer;
	m_sendBufferSize = size;
}

void CSASPollMessage::SetResponseBuffer(BYTE *buffer, UINT size)
{
	m_responseBuffer = buffer;
	m_responseBufferSize = size;
}


__int64 CSASPollMessage::Bcd2Int64(BYTE *buffer, UINT bufferSize)
{

	int hNibble, lNibble;
	__int64 result = 0;

	for (UINT i = 0; i < bufferSize; i++)
	{
		result *= 100;

		hNibble = buffer[i] >> 4;
		lNibble = buffer[i] & 0x0F;

		result += hNibble*10 + lNibble;
	}
	return result;
}

DWORD CSASPollMessage::Bcd2Dword(BYTE *buffer, UINT bufferSize)
{

	int hNibble, lNibble;
	DWORD result = 0;

	for (UINT i = 0; i < bufferSize; i++)
	{
		result *= 100;

		hNibble = buffer[i] >> 4;
		lNibble = buffer[i] & 0x0F;

		result += hNibble*10 + lNibble;
	}
	return result;
}

bool CSASPollMessage::Dword2Bcd(DWORD dwVal, byte *bcdVal, int BcdSize)
{

	DWORD value;
	byte *sVal = new byte[BcdSize*2+4]; // 2 digits per BCD byte + 4 extra chars.
	bool retVal = true;
	byte tmpByte;
	double power;

	value = dwVal;
	for (int x = 0, i = BcdSize*2 - 1; i >= 0 && retVal == true; i -= 2, x++)
	{
		power = pow((double)10,i);
		tmpByte = (byte) ( power == 0 ? value : value / power); // get digit
		if (tmpByte > 9)
			retVal = false;
		sVal[x] = tmpByte << 4;
		value -= (DWORD)(tmpByte * power);

		power /= 10;
		tmpByte = (byte)(power == 0 ? value : value / power); // get digit
		if (tmpByte > 9)
			retVal = false;
		sVal[x] |= tmpByte;
		value -= (DWORD)(tmpByte * power);

	}

	if (retVal)
	{
		// all good here, copy Sval to bcdVal
		memcpy(bcdVal,sVal,BcdSize);
	}
	delete [] sVal;

	return retVal;
}

bool CSASPollMessage::VerifyResponseChecksum()
{
	bool bReturnValue;

	if ((m_responseBuffer != NULL) && (m_responseBufferSize > 2))
	{
		WORD crcCalc = CRC((unsigned char *)m_responseBuffer, m_responseBufferSize - 2);
		WORD *crcResponse = (WORD *)&m_responseBuffer[m_responseBufferSize - 2];
		bReturnValue = memcmp(crcResponse, &crcCalc, CRCSIZE) == 0;
	}
	else
	{
		bReturnValue = true;
	}

	return bReturnValue;
}

unsigned short CSASPollMessage::CRC(unsigned char *s, int len, unsigned short crcval)
{
	unsigned short c,q;

	for (; len > 0; len--)
	{
		c = *s++;
		q = (crcval ^ c) & 017;
		crcval = (crcval >> 4) ^ (q * 010201);
		q = (crcval ^ (c >> 4)) & 017;
		crcval = (crcval >> 4) ^ (q * 010201);
	}
	return (crcval);
}
