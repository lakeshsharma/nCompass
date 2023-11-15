#include "stdafx.h"
#include "LongPoll54Message.h"

using namespace std;

const byte X54_MAX_RESPONSE_SIZE = 48;
const byte X54_LENGTHBYTE = 2;
const byte X54_LENGTHSIZE = 1;
const byte X54_POLLSIZE = 2;
const byte X54_RESPONSE_EXTRADATASIZE = 5;

#define VERSIONLOCATION 3
#define GAMEIDLOCATION 6
#define VERSIONSIZE 3
const int MAXSERIALSIZE = 41; // Includes trailing NUL character.

CLongPoll54Message::CLongPoll54Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	m_isVariableResponse = true;
	m_variableResponseLenghLocation = X54_LENGTHBYTE; // length does NOT include CRC bytes.
	m_variableResponseLengthSize = X54_LENGTHSIZE;
	m_variableResponseSizeConstant = X54_RESPONSE_EXTRADATASIZE; // game address, game command, length byte, ..... , 2 byte CRC

	// allocate buffers
	SetSendBuffer((BYTE *) new byte[X54_POLLSIZE],X54_POLLSIZE);
	SetResponseBuffer((BYTE *)new byte[X54_MAX_RESPONSE_SIZE],X54_MAX_RESPONSE_SIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_serialNumber = "";
}

CLongPoll54Message::~CLongPoll54Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll54Message::GetPollValue()
{
	return 0x54;
}

bool CLongPoll54Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll54Message::ResponseReceived()
{
	// Check if m_responseBufferSize was set correctly.
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		// Check if the game address and poll value in the response is correct.
		if (m_responseBuffer[GAME_ADDRESS_LOCATION] == m_gameAddress &&
			m_responseBuffer[POLL_VALUE_LOCATION] == POLL_VALUE)
		{
			// Check if the response is long enough to contain the required version field.
			if (m_responseBufferSize >= VERSIONLOCATION + VERSIONSIZE + CRCSIZE)
			{
				memcpy(version, m_responseBuffer + VERSIONLOCATION, VERSIONSIZE);

				// Calculate length of serial number
				int lengthByte = m_responseBuffer[m_variableResponseLenghLocation] - VERSIONSIZE;
				// For a valid length of zero, skip processing and retain the default empty string.
				if (lengthByte > 0 && lengthByte < MAXSERIALSIZE)
				{
					byte *serialLocation = &m_responseBuffer[m_variableResponseLenghLocation + VERSIONSIZE + 1];
					char cSerialNumber[MAXSERIALSIZE];
					memcpy(cSerialNumber, serialLocation, lengthByte);
					cSerialNumber[lengthByte] = '\0';
					m_serialNumber = cSerialNumber;
				}
			}
			else
			{
				m_nReturnValue = SASPOLL_INCOMPLETE_DATA;
			}
		}
		else
		{
			m_nReturnValue = SASPOLL_NACK;
		}
	}
	
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll54Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x54;

	// Calculate

	return;
}

string CLongPoll54Message::GetSerialNumber() const
{
	return m_serialNumber;
}
