#include "stdafx.h"
#include "LongPoll70Message.h"
#include "../Utilities.h"

const byte X70_RESPONSE_SIZE = 5;
const byte X70_STATIC_DATA_LENGTH = 11;
const byte X70_NONDATA_LENGTH = 5;
const byte X70_MAX_RESPONSE_SIZE = 44;
const byte X70_RESPONSE_EXTRADATASIZE = 5;
const byte X70_LENGTHBYTE = 2;
const byte X70_LENGTHSIZE = 1;

CLongPoll70Message::CLongPoll70Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[2],2);
	SetResponseBuffer((BYTE *)new byte[X70_MAX_RESPONSE_SIZE],X70_MAX_RESPONSE_SIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

}

CLongPoll70Message::~CLongPoll70Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll70Message::GetPollValue()
{
	return 0x70;
}

bool CLongPoll70Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll70Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();
		response += 3; // first byte of actual data

		byte ticketStatus;		
		memcpy(&ticketStatus, response, sizeof(ticketStatus));
		response += sizeof(ticketStatus);
		if (ticketStatus == 0)
		{
			m_responseTicket = new CTicket();

			m_responseTicket->SetTicketNumber(0);
			m_responseTicket->SetAmount(Bcd2Dword(response,5));
			response += 5;

			byte parsingCode;
			memcpy(&parsingCode, response, sizeof(parsingCode));
			response += 1;
			
			// should be zero
			if (parsingCode == 0)
			{
				m_responseTicket->SetValidationNumber(response,9);
			}

			//// restricted expiration date or 0000NNNN days
			//if (*request == 0)
			//{
			//	// expiration in days
			//	response += 2; // beginning 4 zero's
			//	WORD nDays = Bcd2Dword(response, 2);
			//	response += 2;
			//}
			//else
			//{
			//	// expiration date
			//	CTime expDate = CUtilities::MakeCTimeFromMMDDYYYY_HHMMSS(response,4);
			//	response += 4;
			//}

			m_responseTicket->SetActionCode(RequestVendAuthorization);
			m_responseTicket->SetType(VoucherTicket); 
			
			// default to now
			m_responseTicket->SetGameDateTime(CUtilities::GetCurrentTime());
		}
		else
		{
			// ticket no longer in machine, signal error
			m_nReturnValue = SASPOLL_INCOMPLETE_DATA;
		}
			
	}

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll70Message::Fill()
{
	// fill long poll command data

	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x70;

	// Configure response buffer
	m_isVariableResponse = true;
	m_variableResponseLenghLocation = X70_LENGTHBYTE; // length does NOT include CRC bytes.
	m_variableResponseLengthSize = X70_LENGTHSIZE;
	m_variableResponseSizeConstant = X70_RESPONSE_EXTRADATASIZE; // game address, game command, length byte, ..... , 2 byte CRC

	return;
}

