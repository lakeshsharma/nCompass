#include "stdafx.h"
#include "PollerEmployeeResponseMessage.h"


CPollerEmployeeResponseMessage::CPollerEmployeeResponseMessage(const byte *response)
{
	if (response != nullptr)
	{
		byte offset = 0;

		memcpy(&m_buffer.card_id, response + offset, sizeof(m_buffer.card_id));
		offset += sizeof(m_buffer.card_id);

		memcpy(&m_buffer.EncryptedPIN, response + offset, sizeof(m_buffer.EncryptedPIN));
	}
	else
	{
		m_buffer.EncryptedPIN = 0;
		memset(m_buffer.card_id, 0, CARD_ID_LEN);
	}
}

CPollerEmployeeResponseMessage::~CPollerEmployeeResponseMessage(void)
{
}

DWORD CPollerEmployeeResponseMessage::GetPIN()
{
	//To Encode
	//3rd pin 1000
	//2nd pin 100
	//4th pin 10
	//1st pin 1 + 7521

	//but we're decoding:
	DWORD receivedPin = m_buffer.EncryptedPIN;
	receivedPin-= MAGIC_ENCRYPTION_NUMBER;
	DWORD dwPin = 0;
	dwPin += (receivedPin % 10) * 1000;			//1st pin
	dwPin += ((receivedPin / 100)  % 10) * 100;	//2nd pin
	dwPin += (receivedPin / 1000 % 10) * 10;	//3rd pin
	dwPin += (receivedPin / 10) % 10;			//4th pin

	return dwPin;
}

byte* CPollerEmployeeResponseMessage::GetCardID()
{
	return m_buffer.card_id;
}
