#include "stdafx.h"
#include "SASPollMessage.h"
#include "SASGeneralPoll.h"
#include "Test9BitnCompass.h"
#include "math.h"

const int SASPOLLTIMEOUT = 10000;

CSASPollMessage::CSASPollMessage(byte gameAddress) :
	m_sendBufferSize(0),
	m_responseBufferSize(0)
{
	m_bWaitForSASResponse = true;
	m_isVariableResponse = false;

	m_gameAddress = gameAddress;
	m_sendBuffer = NULL;
	m_responseBuffer = NULL;

	m_retryCount = 0;
	m_nReturnValue = 0;
	m_incRetryOnNoResponse = true;
	//m_hResponseSem.lock();

}

CSASPollMessage::~CSASPollMessage(void)
{
	if (m_sendBuffer)
		delete [] m_sendBuffer;
	if (m_responseBuffer)
		delete [] m_responseBuffer;
}

void CSASPollMessage::ResponseReceived()
{
	//m_hResponseSem.unlock();
}

void CSASPollMessage::WaitForResponse()
{
	// Original code repeated wait() call until it didn't timeout
	//m_hResponseSem.wait();
	//if (!m_hResponseSem.try_wait_for(SASPOLLTIMEOUT))
	//{
		// Timeout
		//prtf((char *)"ERROR: SAS poll timeout waiting for long poll %x", (char *)"f", GetPollValue());
	//}
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

bool CSASPollMessage::SwapPollValue()
{
	// Default to swap poll not supported.
	return false;
}

bool CSASPollMessage::FillWithTickCountBeforeSending()
{
	return false;
}

void CSASPollMessage::Fill(DWORD)
{
	// Do nothing.
}

DWORD CSASPollMessage::Bcd2Dword(const BYTE *buffer, UINT bufferSize)
{

	int hNibble, lNibble;
	DWORD result = 0;

	if (buffer != nullptr)
	{
		for (UINT i = 0; i < bufferSize; i++)
		{
			result *= 100;

			hNibble = buffer[i] >> 4;
			lNibble = buffer[i] & 0x0F;

			result += hNibble*10 + lNibble;
		}
	}
	return result;
}

__int64 CSASPollMessage::Bcd2Int64(const BYTE *buffer, UINT bufferSize)
{

	int hNibble, lNibble;
	__int64 result = 0;

	if (buffer != nullptr)
	{
		for (UINT i = 0; i < bufferSize; i++)
		{
			result *= 100;

			hNibble = buffer[i] >> 4;
			lNibble = buffer[i] & 0x0F;

			result += hNibble*10 + lNibble;
		}
	}
	return result;
}
unsigned short CSASPollMessage::CRC(unsigned char *s, int len, unsigned short crcval)
{
	unsigned c,q;

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

void CSASPollMessage::SetSizeLengthAndCRC(UINT size)
{
	if (m_sendBuffer != NULL && size <= m_sendBufferSize && size >= NON_DATA_LENGTH)
	{
		// Set the size,  length and CRC fields.
		SetSendBuffer(m_sendBuffer, size);
		m_sendBuffer[OPTIONAL_LENGTH_OFFSET] = m_sendBufferSize - NON_DATA_LENGTH; // Length of data (that follows the length byte).
		GenerateSendChecksum();
	}
}

void CSASPollMessage::GenerateSendChecksum()
{
	//WARNING this places the checksum in the last two bytes of the databuffer
	if ((m_sendBuffer != NULL) && (m_sendBufferSize > CRCSIZE))
	{
		WORD crcCalc = CRC((unsigned char *)m_sendBuffer+1, m_sendBufferSize - CRCSIZE);
		memcpy(m_sendBuffer + m_sendBufferSize - CRCSIZE, &crcCalc, CRCSIZE);
	}
}

bool CSASPollMessage::VerifyResponseChecksum()
{
	bool bReturnValue;

	if ((m_responseBuffer != NULL) && (m_responseBufferSize > 2))
	{
		unsigned short crcCalc = CRC((unsigned char *)&m_responseBuffer, (m_responseBufferSize - CRCSIZE));
		unsigned short *crcResponse = (unsigned short *)&m_responseBuffer[m_responseBufferSize - CRCSIZE-1];
		printf("NOTE: CRC = %04x, calculated = %04x\n", *crcResponse, crcCalc);
		bReturnValue = memcmp(crcResponse, &crcCalc, CRCSIZE) == 0;
	}
	else
	{
		bReturnValue = true;
	}

	return bReturnValue;
}

bool CSASPollMessage::VerifyResponsePollValue()
{
	bool bReturnValue = true;

	if ((m_responseBuffer != NULL) && (m_responseBufferSize > SASPOLL_POLL_VALUE_OFFSET))
	{
		BYTE messagePollValue = GetPollValue();
		BYTE responsePollValue = m_responseBuffer[SASPOLL_POLL_VALUE_OFFSET];
		bReturnValue = messagePollValue == responsePollValue;
	}

	return bReturnValue;
}

bool CSASPollMessage::Int64ToBcd(__int64 value, byte *bcdVal, int BcdSize)
{
	bool retVal = true;

	if (value >= 0)
	{
		for (int index = BcdSize - 1; index >= 0; --index)
		{
			__int64 lowNibble = value % 10;
			value /= 10;
			__int64 highNibble = value % 10;
			value /= 10;

			bcdVal[index] = (byte)((highNibble << 4) + lowNibble);
		}
	}

	if (value != 0)
	{
		memset(bcdVal, 0, BcdSize);
		retVal = false;
	}
	return retVal;
}

bool CSASPollMessage::Dword2Bcd(DWORD dwVal, byte *bcdVal, int BcdSize)
{

	DWORD value;
	byte *sVal = new byte[BcdSize*2+4]; // 2 digits per BCD byte + 4 extra chars.
	int charcount = 0;
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



void CSASPollMessage::SetGameNumber(int gameNumber)
{
	// does nothing
	//overridden in 2F and 6F long polls
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

void CSASPollMessage::SetReturnValue(int returnValue)
{
	m_nReturnValue = returnValue;
	return;
}

int CSASPollMessage::GetReturnValue()
{
	return m_nReturnValue;
}

bool CSASPollMessage::IncRetryOnNoResponse()
{
	return m_incRetryOnNoResponse;
}

void CSASPollMessage::SetIncRetryOnNoResponse(bool incRetryOnNoResponse)
{
	m_incRetryOnNoResponse = incRetryOnNoResponse;
}

bool CSASPollMessage::IsGeneralPoll()
{
	return false;
}

bool CSASPollMessage::IsNACK(DWORD numberBytesRead)
{
	bool isNack(false);

	// General polls cannot be NACKed.
	if (!IsGeneralPoll() && numberBytesRead > 0)
	{
		const BYTE nackValue((BYTE)(m_gameAddress | GAME_ADDRESS_NACK_OR_VALUE));

		if (nackValue == *m_responseBuffer)
		{
			isNack = true;
		}
	}

	return isNack;
}
