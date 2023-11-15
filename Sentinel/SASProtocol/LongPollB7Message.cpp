#include "stdafx.h"
#include "LongPollB7Message.h"


CLongPollB7Message::CLongPollB7Message(byte gameAddress): CSASPollMessage(gameAddress)
{
	m_length = 5;
	m_assetNumber = 0;
	m_floorLocationLength = 0;
	memset(m_floorLocation, 0, MAX_FLOORLOCATIONSIZE);
	m_controlFlag = 0;	
}

CLongPollB7Message::~CLongPollB7Message(void)
{
}

BYTE CLongPollB7Message::GetPollValue()
{
	return LP_B7_VALUE;
}

bool CLongPollB7Message::SwapPollValue()
{
	return false;
}

void CLongPollB7Message::ResponseReceived(void)
{
	if (GetReturnValue() == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();
		response += 2;
			
		memcpy(&m_length, response++, 1);
		memcpy(&m_controlFlag, response++, 1);

		memcpy(&m_assetNumber, response, 4);
		response += 4;

		memcpy(&m_floorLocationLength, response++, 1);
		memcpy(m_floorLocation, response, m_floorLocationLength);
	}

	CSASPollMessage::ResponseReceived();

	return;
}

void CLongPollB7Message::Fill()
{
	byte offset = 0;
	SetSendBuffer((BYTE *) new byte[XB7_POLLSIZE],XB7_POLLSIZE);
	SetResponseBuffer((BYTE *)new byte[XB7_RESPONSESIZE],XB7_RESPONSESIZE);
	memset(GetSendBuffer(), 0, GetSendBufferSize());
	memset(GetResponseBuffer(), 0, GetResponseBufferSize());

	// fill long poll command data
	m_sendBuffer[offset++] = m_gameAddress;
	m_sendBuffer[offset++] = LP_B7_VALUE;
	m_sendBuffer[offset++] = m_length;
	memcpy(&m_sendBuffer[offset], &m_assetNumber, sizeof(DWORD));
	offset +=sizeof(DWORD);
		
	m_sendBuffer[offset++] =  m_floorLocationLength;
	// Send floor location info, if required
	if (m_floorLocationLength != 0)
	{
		memcpy(&m_sendBuffer[offset], m_floorLocation, m_floorLocationLength);
		offset += m_floorLocationLength;
	}
	// adjust buffer size variable based on variable content, updated size needed in base class functions.
	m_sendBufferSize -= (MAX_FLOORLOCATIONSIZE - m_floorLocationLength);
		
	ushort commandCRC = CRC(m_sendBuffer, m_sendBufferSize - CRCSIZE,0);
	memcpy(&m_sendBuffer[offset],&commandCRC,CRCSIZE);
	
	// Configure response buffer
	m_isVariableResponse = true;
	m_variableResponseLenghLocation = 2; 
	m_variableResponseLengthSize = 1;
	m_variableResponseSizeConstant = 5;
		
	return;
}
	
void CLongPollB7Message::SetAssetNumber(DWORD assetNum)
{
	m_assetNumber = assetNum;
}

DWORD CLongPollB7Message::GetAssetNumber()
{
	return m_assetNumber;
}

void CLongPollB7Message::SetFloorLocation(byte *floorLoc, byte floorLocLength)
{
	if (floorLoc != NULL)
	{
		m_floorLocationLength = min(floorLocLength, MAX_FLOORLOCATIONSIZE);
		memcpy(m_floorLocation, floorLoc, m_floorLocationLength);
		m_length += m_floorLocationLength; 
	}
}

byte* CLongPollB7Message::GetFloorLocation(byte &length)
{
	length = m_floorLocationLength;
	return m_floorLocation;
}

bool CLongPollB7Message::ResponseHostControlAssetNumber()
{
	return m_controlFlag & EGM_SUPPORT_ASSET_NUMBER_UPDATE_BY_HOST;
}

bool CLongPollB7Message::ResponseHostControlFloorLocation()
{
	return m_controlFlag & EGM_SUPPORT_FLOOR_LOCATION_UPDATE_BY_HOST;
}



