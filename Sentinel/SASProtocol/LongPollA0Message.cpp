#include "stdafx.h"
#include "LongPollA0Message.h"

CLongPollA0Message::CLongPollA0Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[6],6);
	SetResponseBuffer((BYTE *)new byte[12],12);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
}

CLongPollA0Message::~CLongPollA0Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPollA0Message::GetPollValue()
{
	return 0xA0;
}

bool CLongPollA0Message::SwapPollValue()
{
	
	return false;
}

void CLongPollA0Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();
		m_bJackpotMultiplier = response[4] & 0x01 ? true : false;
		m_bAFTBonusAwards = response[4] & 0x02 ? true : false;
		m_bLegacyBonusAwards = response[4] & 0x04 ? true : false;
		m_bTournament = response[4] & 0x08 ? true : false;
		m_bValidationExtensions = response[4] & 0x10 ? true : false;
		m_cValidationStyle = (response[4] & 0x60) >> 5;
		m_bTicketRedemption = response[4] & 0x80 ? true : false;

		m_cMeterModelFlag = response[5] & 0x03;
		m_bTicketsToTotalDrop = response[5] & 0x04 ? true : false;
		m_bExtendedMeters = response[5] & 0x08 ? true : false;
		m_bComponentAuthentication = response[5] & 0x10 ? true : false;
		m_bAFT = response[5] & 0x40 ? true : false;
		m_bMultiDenomExtensions = response[5] & 0x80 ? true : false;

		m_bMaximumPollingRate = response[6] & 0x01 ? true : false;
		m_bMultipleSASProgressiveWinReporting = response[6] & 0x02 ? true : false;
	}
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPollA0Message::Fill()
{
	byte *crc;

	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0xA0;
	// fill in gamenumber bcd
	Dword2Bcd(m_gameNumber,&m_sendBuffer[2],2);
	ushort commandCRC = CRC(m_sendBuffer, 4, 0);
	crc = (byte *)&m_sendBuffer[4];
	memcpy(crc,&commandCRC,CRCSIZE);

	return;
}

void CLongPollA0Message::SetGameNumber(int gameNumber)
{
	m_gameNumber = gameNumber;
	return;
}
