#include "stdafx.h"
#include "SASPollMessage.h"
#include "SASGeneralPoll.h"
#include "../Games.h"
#include "Hardware/WatchDog.h"
#include "../Logging/LogString.h"

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
	m_responseReceived = false;
	m_ownerQueue = nullptr;
	m_hResponseSem.lock();
}

CSASPollMessage::~CSASPollMessage(void)
{
   if (m_sendBuffer)
		delete [] m_sendBuffer;
	if (m_responseBuffer)
		delete [] m_responseBuffer;
}

void CSASPollMessage::WaitForSASProcessingDone()
{
#ifndef _UNIT_TEST
	if (nullptr != m_ownerQueue)
	{
		// ResponseReceived is actually acting as SAS processing done flag and we set this 
		// even if we do not wait for response from SAS but conclude our processing
		while (!m_responseReceived)
		{
			CUtilities::Sleep(SLEEP_YIELD_MILLISECONDS);
		}
	}
#endif
}

void CSASPollMessage::ResponseReceived()
{
	m_responseReceived = true;
	m_hResponseSem.unlock();
}

void CSASPollMessage::ResetResponse()
{
	CSASPollMessage::WaitForSASProcessingDone();

	// resetting to default values.
	m_retryCount = 0;
	m_nReturnValue = 0;
	m_responseReceived = false;
	m_hResponseSem.lock();
}

void CSASPollMessage::WaitForResponse()
{
#ifndef _UNIT_TEST
   uint startTick = (uint)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
#endif

   if (m_responseReceived)
   {
	   LogString(ERROR_LOG, "[WARNING] SAS poll instance is being reused, LP = %x", GetPollValue());
   }

   // Original code repeated wait() call until it didn't timeout
   if (!m_hResponseSem.try_wait_for(SASPOLLTIMEOUT, false))
   {
#ifndef _UNIT_TEST
      // Timeout
      LogString(ERROR_LOG, "SAS poll timeout waiting for long poll %x", GetPollValue());
      uint duration = ((uint)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()) - startTick;

      if (duration < SASPOLLTIMEOUT)
      {
         // If we are here there must have been a timechange forward.
         // Do this sleep so SAS message processing does not crash
         LogString(ERROR_LOG, "SAS poll timeout was premature. sleeping %d", SASPOLLTIMEOUT - duration);
         CUtilities::Sleep(SASPOLLTIMEOUT - duration);
      }

      // call timed out, try to remove from queue
      bool itemRemoved = m_ownerQueue->ForceRemoveItem(this, m_priority);
	  if (itemRemoved)
	  {
		  LogString(ERROR_LOG, "SAS message successfully removed from the processing queue after it timed-out to be processed.");

		  // Item is removed before its processing is started by SAS processing thread, set responseReceived 
		  // so that message is marked processed and safe to be deleted
		  m_responseReceived = true;
	  }
	  else
	  {
		  LogString(ERROR_LOG, "SAS message processing timed-out but message is still being processed (because not found in the queue). "
			  "We will wait for it to be done before deleting the message");
	  }
#endif
   }

	// clear binsem
   m_hResponseSem.unlock();


   CWatchDog::Instance().Checkin();
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
		WORD crcCalc = CRC((unsigned char *)m_sendBuffer, m_sendBufferSize - CRCSIZE);
		memcpy(m_sendBuffer + m_sendBufferSize - CRCSIZE, &crcCalc, CRCSIZE);
	}
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

byte CSASPollMessage::GetMeterCodeForMeterNumber(int meterNumber, int gameNumber, byte &primaryMeterCode, byte &secondaryMeterCode)
{
	byte retVal;
	retVal = primaryMeterCode = secondaryMeterCode = INVALID_SAS_METER_CODE;

	if ( gameNumber == EGM_GAME_NUMBER )
	{
		for ( int mapIndex = 0; mapIndex < NUM_MAIN_GAME_METER_CODE_MAPPINGS && retVal == 0xff; mapIndex ++ )
		{
			if ( mainGameMeterCodeMap[mapIndex].oasisMeterNumber == meterNumber )
			{
				retVal = primaryMeterCode = mainGameMeterCodeMap[mapIndex].sasMeterCode;
				secondaryMeterCode = mainGameMeterCodeMap[mapIndex].sasMeterCodeSecondary;
			}
		}
	}

	else
	{
		for ( int mapIndex = 0; mapIndex < NUM_SUB_GAME_METER_CODE_MAPPINGS && retVal == 0xff; mapIndex ++ )
		{
			if ( subGameMeterCodeMap[mapIndex].oasisMeterNumber == meterNumber )
			{
				retVal = primaryMeterCode = subGameMeterCodeMap[mapIndex].sasMeterCode;
				secondaryMeterCode = subGameMeterCodeMap[mapIndex].sasMeterCodeSecondary;
			}
		}
	}

	return retVal;
}

int CSASPollMessage::GetMeterNumberForMeterCode(byte meterCode, int gameNumber)
{
	int retVal = INVALID_METER_NUMBER;

	if ( gameNumber == EGM_GAME_NUMBER )
	{
		// look for primary meter code match first.
		for ( int mapIndex = 0; mapIndex < NUM_MAIN_GAME_METER_CODE_MAPPINGS && retVal == INVALID_METER_NUMBER; mapIndex ++ )
		{
			if ( mainGameMeterCodeMap[mapIndex].sasMeterCode == meterCode)
				retVal = mainGameMeterCodeMap[mapIndex].oasisMeterNumber;
		}
		// look for secondary meter code match if primary meter code didn't find anything.
		for ( int mapIndex = 0; mapIndex < NUM_MAIN_GAME_METER_CODE_MAPPINGS && retVal == INVALID_METER_NUMBER; mapIndex ++ )
		{
			if ( mainGameMeterCodeMap[mapIndex].sasMeterCodeSecondary == meterCode)
				retVal = mainGameMeterCodeMap[mapIndex].oasisMeterNumber;
		}
	}

	else 
	{
		// look for primary meter code match first.
		for ( int mapIndex = 0; mapIndex < NUM_SUB_GAME_METER_CODE_MAPPINGS && retVal == INVALID_METER_NUMBER; mapIndex ++ )
		{
			if ( subGameMeterCodeMap[mapIndex].sasMeterCode == meterCode)
				retVal = subGameMeterCodeMap[mapIndex].oasisMeterNumber;
		}
		// look for secondary meter code match if primary meter code didn't find anything.
		for ( int mapIndex = 0; mapIndex < NUM_SUB_GAME_METER_CODE_MAPPINGS && retVal == INVALID_METER_NUMBER; mapIndex ++ )
		{
			if ( subGameMeterCodeMap[mapIndex].sasMeterCodeSecondary == meterCode)
				retVal = subGameMeterCodeMap[mapIndex].oasisMeterNumber;
		}
	}

	return retVal;
}

void CSASPollMessage::SetGameNumber(int gameNumber)
{
    UNREFERENCED_PARAMETER(&gameNumber); 
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

void CSASPollMessage::SetQueue(CSASQueueList *queueList, int priority)
{
   m_ownerQueue = queueList;
   m_priority = priority;
}
