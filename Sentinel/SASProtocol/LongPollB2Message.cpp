#include "stdafx.h"
#include "LongPollB2Message.h"

extern const int NUM_DENOM_CODE_MAPPINGS;

CLongPollB2Message::CLongPollB2Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[2],2);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	SetResponseBuffer((byte *)new byte[25],25); // ??
	memset(GetSendBuffer(),0,GetSendBufferSize());

	m_enabledDenomList = NULL;

}

CLongPollB2Message::~CLongPollB2Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPollB2Message::GetPollValue()
{
	return 0xB2;
}

bool CLongPollB2Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPollB2Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();
		response += 3;
		//m_numberDenoms = *(response);
		memcpy(&m_numberDenoms,response,1);
		response += 1;
        m_numberDenoms = min(m_numberDenoms, m_enabledDenomList_LenMax);

		// 1 byte returned for each enabled denom.
		if (m_enabledDenomList)
			memcpy(m_enabledDenomList,response,m_numberDenoms);
		
		AdjustForDuplicates();
	}		
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPollB2Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0xB2;

	m_isVariableResponse = true;
	m_variableResponseLenghLocation = XB2_LENGTHBYTE; // length does NOT include CRC bytes.
	m_variableResponseLengthSize = XB2_LENGTHSIZE;
	m_variableResponseSizeConstant = XB2_RESPONSE_EXTRADATASIZE; // game address, game command, length byte, ..... , 2 byte CRC

	return;
}

void CLongPollB2Message::SetEnabledDenomList(byte *enabledDenomList, byte enabledDenomList_LenMax)
{
	m_enabledDenomList = enabledDenomList;
	m_enabledDenomList_LenMax = enabledDenomList_LenMax;
	return;
}

void CLongPollB2Message::AdjustForDuplicates()
{
	if (m_enabledDenomList)
	{
		for (int i = 0; i < m_numberDenoms - 1; i++)
		{
			for (int x = i + 1; x < m_numberDenoms; x++)
			{
				if (m_enabledDenomList[i] == m_enabledDenomList[x])
				{
					// found a duplicate, remove duplicate and shift remaining down 1, decrease m_numberDenoms, repeat this x value
					for (int y = x; y < m_numberDenoms; y++)
					{
						if (y + 1 < m_numberDenoms && y < NUM_DENOM_CODE_MAPPINGS) {
							m_enabledDenomList[y] = m_enabledDenomList[y + 1];
						}
					}

					m_numberDenoms--;
					x--;
				}
			}
		}
	}
}