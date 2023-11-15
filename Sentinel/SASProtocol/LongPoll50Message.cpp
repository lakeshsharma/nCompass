#include "stdafx.h"
#include "LongPoll50Message.h"

CLongPoll50Message::CLongPoll50Message(byte gameAddress, CMeters *meters)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[X50_SEND_SIZE],X50_SEND_SIZE);
	SetResponseBuffer((byte*)new byte[X50_RESPONSE_SIZE],X50_RESPONSE_SIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll50Message::~CLongPoll50Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll50Message::GetPollValue()
{
	return 0x50;
}

bool CLongPoll50Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll50Message::ResponseReceived()
{
	CMeter *meter = NULL;

	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		memcpy(&m_validationType, &response[2],sizeof(byte));
		m_totalValidations = Bcd2Dword(&response[3],4);
		m_cumeAmount = Bcd2Dword(&response[7],5);

		switch(m_validationType)
		{
		case 0x00:
			meter = m_meters->GetMeterByNumber(mTO);
			if (meter != NULL)
			{
				meter->SetValue(m_totalValidations);
			}
			meter = m_meters->GetMeterByNumber(mTVal);
			if (meter != NULL)
			{
				meter->SetValue(m_cumeAmount);
			}
			break;
		case 0x01:
			meter = m_meters->GetMeterByNumber(mCPromoOutCnt);
			if (meter != NULL)
			{
				meter->SetValue(m_totalValidations);
			}
			meter = m_meters->GetMeterByNumber(mCPromoOutVal);
			if (meter != NULL)
			{
				meter->SetValue(m_cumeAmount);
			}
			break;
		case 0x10:
			meter = m_meters->GetMeterByNumber(mHPCshRcptCnt);
			if (meter != NULL)
			{
				meter->SetValue(m_totalValidations);
			}
			meter = m_meters->GetMeterByNumber(mHPCshRcptVal);
			if (meter != NULL)
			{
				meter->SetValue(m_cumeAmount);
			}
			break;
		case 0x20:
			meter = m_meters->GetMeterByNumber(mHPWinRcptCnt);
			if (meter != NULL)
			{
				meter->SetValue(m_totalValidations);
			}
			meter = m_meters->GetMeterByNumber(mHPWinRcptVal);
			if (meter != NULL)
			{
				meter->SetValue(m_cumeAmount);
			}
			break;
		case 0x80:
			//{mCoupV       ,0x28, 0x0d},
			//{mVchrTktDrop ,0x29, INVALID_SAS_METER_CODE},
			//{mCoup        ,0x11, INVALID_SAS_METER_CODE},
			//{mCTktPV      ,0x2a, 0x0f},
			meter = m_meters->GetMeterByNumber(mCoup);
			if (meter != NULL)
			{
				meter->SetValue(m_totalValidations);
			}
			meter = m_meters->GetMeterByNumber(mCoup);
			if (meter != NULL)
			{
				meter->SetValue(m_totalValidations);
			}
			meter = m_meters->GetMeterByNumber(mCoupV);
			if (meter != NULL)
			{
				meter->SetValue(m_cumeAmount);
			}
			break;
		case 0x81:
			meter = m_meters->GetMeterByNumber(mCTktPN);
			if (meter != NULL)
			{
				meter->SetValue(m_totalValidations);
			}
			meter = m_meters->GetMeterByNumber(mCTktPV);
			if (meter != NULL)
			{
				meter->SetValue(m_cumeAmount);
			}
			break;
		case 0x82:
			meter = m_meters->GetMeterByNumber(mCTktCN);
			if (meter != NULL)
			{
				meter->SetValue(m_totalValidations);
			}
			meter = m_meters->GetMeterByNumber(mCTktCV);
			if (meter != NULL)
			{
				meter->SetValue(m_cumeAmount);
			}
			break;
		}
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll50Message::Fill()
{
	// Use first meter in meters to determine what validation type to use
	
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x50;

	switch(m_meters->GetMeterByIndex(0)->GetMeterNumber())
	{
	case mTVal:
	case mTO:
		m_validationType = 0x00;
		break;
	case mCPromoOutVal:
	case mCPromoOutCnt:
		m_validationType = 0x01;
		break;
	case mHPCshRcptCnt:
	case mHPCshRcptVal:
		m_validationType = 0x10;
		break;
	case mHPWinRcptCnt:
	case mHPWinRcptVal:
		m_validationType = 0x20;
		break;
	case mCoupV:
	case mCoup:
		m_validationType = 0x80;
		break;
	case mCTktPN:
	case mCTktPV:
		m_validationType = 0x81;
		break;
	case mCTktCN:
	case mCTktCV:
		m_validationType = 0x82;
		break;
	}
	memcpy(&m_sendBuffer[2], & m_validationType, sizeof(byte));

	ushort messageCRC = CRC(GetSendBuffer(),X50_SEND_SIZE - CRCSIZE,0);
	memcpy(&m_sendBuffer[3],&messageCRC,CRCSIZE);
	return;
}
