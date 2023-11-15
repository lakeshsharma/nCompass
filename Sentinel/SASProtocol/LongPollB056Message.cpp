#include "stdafx.h"
#include "LongPollB056Message.h"

CLongPollB056Message::CLongPollB056Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[7],7);
	memset(GetSendBuffer(),0,GetSendBufferSize());

	m_enabledGamesList = NULL;
}

CLongPollB056Message::~CLongPollB056Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPollB056Message::GetPollValue()
{
	return 0xB0;
}

bool CLongPollB056Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPollB056Message::ResponseReceived()
{
	try
	{
		if (m_nReturnValue == SASPOLL_SUCCESS)
		{
			byte *response = GetResponseBuffer();
			if (response[4] == 0x56)



			{
				m_enabledGames = response[6];
				response += 7;
				
				if (m_enabledGamesList)
				{
					// only parse list if m_enabledGamesList isn't null
					for (byte i=0; i < m_enabledGames; i++)
					{
						m_enabledGamesList[i] = (WORD)Bcd2Dword(response,2);
						response += 2;
					}
				}
			}
			else
			{
				// error detected
				m_nReturnValue = SASPOLL_INCOMPLETE_DATA;
			}
		}
		CSASPollMessage::ResponseReceived();
	}
	catch (...)
	{
	}

	return;

}

void CLongPollB056Message::Fill()
{
	try
	{
		// fill long poll command data
		m_sendBuffer[0] = m_gameAddress;
		m_sendBuffer[1] = 0xB0; // b0 command
		m_sendBuffer[2] = 2;	// number bytes remaining not including crc
		m_sendBuffer[3] = m_denom; // denomination
		m_sendBuffer[4] = 0x56; // base command
		
		m_isVariableResponse = true;
		m_variableResponseLenghLocation = XB056_LENGTHBYTE; // length does NOT include CRC bytes.
		m_variableResponseLengthSize = XB056_LENGTHSIZE;
		m_variableResponseSizeConstant = XB056_RESPONSE_EXTRADATASIZE; // game address, game command, length byte, ..... , 2 byte CRC

		// assume all implemented games are enabled
		uint responseInitSize = m_implementedGames * XB056_DEFSIZE + XB056_RESPONSE_EXTRADATASIZE + CRCSIZE + 2; // 2 is extra bytes because of b0 preamble
		SetResponseBuffer((byte*)new byte[responseInitSize],responseInitSize);

		byte *crcLocation;
		ushort messageCRC = CRC(GetSendBuffer(),GetSendBufferSize() - CRCSIZE,0);
		crcLocation = (byte *)&m_sendBuffer[5];
		memcpy(crcLocation,&messageCRC,CRCSIZE);
	}
	catch(...)
	{
	}
	return;
}

void CLongPollB056Message::SetImplementedGames(int implementedGames)
{
	m_implementedGames = implementedGames;
	return;
}

void CLongPollB056Message::SetEnabledGamesList(WORD *enabledGamesList)
{
	m_enabledGamesList = enabledGamesList;
	return;
}

void CLongPollB056Message::SetDenom(byte denom)
{
	m_denom = denom;
	return;
}
