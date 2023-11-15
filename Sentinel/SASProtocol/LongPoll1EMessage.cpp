#include "stdafx.h"
#include "LongPoll1EMessage.h"

CLongPoll1EMessage::CLongPoll1EMessage(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[2],2);
	SetResponseBuffer((BYTE *)new byte[28],28);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll1EMessage::~CLongPoll1EMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll1EMessage::GetPollValue()
{
	return 0x1E;
}

bool CLongPoll1EMessage::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll1EMessage::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();
		m_one = Bcd2Int64(response+2,4);
		if (m_meters)
			m_meters->SetMeter( mD1, m_one);
		m_five = Bcd2Int64(response+6,4);
		if (m_meters)
			m_meters->SetMeter( mD5, m_five);
		m_ten = Bcd2Int64(response+10,4);
		if (m_meters)
			m_meters->SetMeter( mD10, m_ten);
		m_twenty = Bcd2Int64(response+14,4);
		if (m_meters)
			m_meters->SetMeter( mD20, m_twenty);
		m_fifty = Bcd2Int64(response+18,4);
		if (m_meters)
			m_meters->SetMeter( mD50, m_fifty);
		m_hundred = Bcd2Int64(response+22,4);
		if (m_meters)
			m_meters->SetMeter( mD100, m_hundred);
	}

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll1EMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x1E;

	return;
}

