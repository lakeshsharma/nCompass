#include "stdafx.h"
#include "LongPoll52Message.h"

#define EGM_GAME_NUMBER		0		// EGM game number is always 0

CLongPoll52Message::CLongPoll52Message(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[X52_POLLSIZE],X52_POLLSIZE);
	SetResponseBuffer((BYTE *)new byte[X52_RESPONSESIZE],X52_RESPONSESIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_meters = meters;
}

CLongPoll52Message::~CLongPoll52Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll52Message::GetPollValue()
{
	return 0x52;
}

bool CLongPoll52Message::SwapPollValue()
{
	// what do we do here?
	return false;
}

void CLongPoll52Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		BYTE *response = GetResponseBuffer();
		if (m_gameNumber == EGM_GAME_NUMBER)
		{
			m_coinIn = Bcd2Int64(response+4,4);
			if (m_meters)
				m_meters->SetMeter( gCI, m_coinIn);
			m_coinOut = Bcd2Int64(response+8,4);
			if (m_meters)
				m_meters->SetMeter( gCO, m_coinOut);
			m_jackpot = Bcd2Int64(response+12,4);
			if (m_meters)
				m_meters->SetMeter( gJP, m_jackpot);
			m_gamesPlayed = Bcd2Int64(response+16,4);
			if (m_meters)
				m_meters->SetMeter( gGS, m_gamesPlayed);
		}
		else
		{
			m_coinIn = Bcd2Int64(response+4,4);
			if (m_meters)
				m_meters->SetMeter( SUBGAME_METER_COIN_IN, m_coinIn);
			m_coinOut = Bcd2Int64(response+8,4);
			if (m_meters)
				m_meters->SetMeter( SUBGAME_METER_COIN_OUT, m_coinOut);
			m_jackpot = Bcd2Int64(response+12,4);
			if (m_meters)
				m_meters->SetMeter( SUBGAME_METER_ATT_PAID_JACKPOT, m_jackpot);
			m_gamesPlayed = Bcd2Int64(response+16,4);
			if (m_meters)
				m_meters->SetMeter( SUBGAME_METER_GAMES_PLAYED, m_gamesPlayed);
		}
	}
	CSASPollMessage::ResponseReceived();

	return;


}

void CLongPoll52Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x52;
	Dword2Bcd(m_gameNumber,&m_sendBuffer[2],2);

	byte *crcLocation;
	ushort messageCRC = CRC(GetSendBuffer(),GetSendBufferSize() - CRCSIZE,0);
	crcLocation = (byte *)&m_sendBuffer[4];
	memcpy(crcLocation,&messageCRC,CRCSIZE);

	return;
}

void CLongPoll52Message::SetGameNumber(int gameNumber)
{
	m_gameNumber = gameNumber;

	return;
}

