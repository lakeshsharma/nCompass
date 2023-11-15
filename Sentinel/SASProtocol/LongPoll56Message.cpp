#include "stdafx.h"
#include "LongPoll56Message.h"

CLongPoll56Message::CLongPoll56Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2],2);
	memset(GetSendBuffer(),0,GetSendBufferSize());

	m_enabledGamesList = NULL;
}

CLongPoll56Message::~CLongPoll56Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll56Message::GetPollValue()
{
	return 0x56;
}

bool CLongPoll56Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll56Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();
		response += 3;
		m_enabledGames = *(response);
		response += 1;

		if (m_enabledGamesList)
		{
			// only parse list if m_enabledGamesList isn't null
			for (byte i=0; i < m_enabledGames; i++)
			{
				m_enabledGamesList[i] = (WORD)Bcd2Int64(response,2);
				response += 2;
			}
		}
	}

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll56Message::Fill()
{
	// fill long poll command data
	
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x56;

	m_isVariableResponse = true;
	m_variableResponseLenghLocation = X56_LENGTHBYTE; // length does NOT include CRC bytes.
	m_variableResponseLengthSize = X56_LENGTHSIZE;
	m_variableResponseSizeConstant = X56_RESPONSE_EXTRADATASIZE; // game address, game command, length byte, ..... , 2 byte CRC

	// assume all implemented games are enabled
	uint responseInitSize = m_implementedGames * X56_DEFSIZE + X56_RESPONSE_EXTRADATASIZE + CRCSIZE;
	SetResponseBuffer((byte*)new byte[responseInitSize],responseInitSize);

	return;
}

void CLongPoll56Message::SetImplementedGames(int implementedGames)
{
	m_implementedGames = implementedGames;
	return;
}

void CLongPoll56Message::SetEnabledGamesList(WORD *enabledGamesList)
{
	m_enabledGamesList = enabledGamesList;
	return;
}
