#include "stdafx.h"
#include "LongPoll2DMessage.h"

CLongPoll2DMessage::CLongPoll2DMessage(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[6],6);
	SetResponseBuffer((byte*)new byte[10],10);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll2DMessage::~CLongPoll2DMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll2DMessage::GetPollValue()
{
	return 0x2D;
}

bool CLongPoll2DMessage::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll2DMessage::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		m_cancelledCredits = Bcd2Int64(response + 4,4);
		if (m_meters)
			m_meters->SetMeter( mHCC, m_cancelledCredits);
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll2DMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x2D;
	memcpy(&m_sendBuffer[2],&m_gameNumber,2);

	byte *crcLocation;
	ushort messageCRC = CRC(GetSendBuffer(),GetSendBufferSize() - CRCSIZE,0);
	crcLocation = (byte *)&m_sendBuffer[4];
	memcpy(crcLocation,&messageCRC,CRCSIZE);

	return;
}

void CLongPoll2DMessage::SetGameNumber(int gameNumber)
{
	m_gameNumber = gameNumber;
	return;
}

