#include "stdafx.h"
#include "LongPoll4DMessage.h"
#include "../Utilities.h"

CLongPoll4DMessage::CLongPoll4DMessage(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte *) new byte[5],5);
	SetResponseBuffer((byte*)new byte[35],35);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_bPeek = true;
	m_responseTicket = NULL;
}

CLongPoll4DMessage::~CLongPoll4DMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll4DMessage::GetPollValue()
{
	return 0x4D;
}

bool CLongPoll4DMessage::SwapPollValue()
{
	return false;
}

void CLongPoll4DMessage::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		m_responseTicket = new CTicket();

		// move to response data
		response += 2;

		byte validationTypeCode;
		memcpy(&validationTypeCode,response,sizeof(validationTypeCode));
		response += sizeof(validationTypeCode);

		switch(validationTypeCode)
		{
			// TBD: figure out what Type and ActionCode should be for each case
		case 0: // Cashable ticket
			m_responseTicket->SetType(VoucherTicket,false);
			m_responseTicket->SetActionCode(PrintCompleteGameGeneratedValNum);
			break;
		case 1: // Restricted promotional ticket from cashout
			m_responseTicket->SetType(CouponPromo,false);
			m_responseTicket->SetActionCode(PrintCompleteGameGeneratedValNum);
			break;
		case 2: // Cashable ticket from AFT transfer
			m_responseTicket->SetType(VoucherTicket,false);
			m_responseTicket->SetActionCode(PrintCompleteGameGeneratedValNum);
			break;
		case 3: // Restricted ticket from AFT transfer
			m_responseTicket->SetType(CouponPromo,false);
			m_responseTicket->SetActionCode(InvalidPrintMessage);
			break;
		case 4: // Debit ticket from AFT transfer
			m_responseTicket->SetType(TicketTypeNoValue,false);
			m_responseTicket->SetActionCode(InvalidPrintMessage);
			break;
		case 0x10: // Handpay from cashout(receipt printed)
			m_responseTicket->SetType(AttPaidCCReceipt,false);
			m_responseTicket->SetActionCode(PrintCompleteGameGeneratedValNum);
			break;
		case 0x20: // Handpay from single win ( receipt printed) 
			m_responseTicket->SetType(AttPaidJPReceipt,false);
			m_responseTicket->SetActionCode(PrintCompleteGameGeneratedValNum);
			break;
		case 0x40: // handpay from cashout (no receipt)
			m_responseTicket->SetType(CashoutNoReceipt,false);
			m_responseTicket->SetActionCode(TicketActionCodeNoValue);
			break;
		case 0x60: // Handpay from single win ( no receipt)
			m_responseTicket->SetType(SingleWinNoReceipt,false);
			m_responseTicket->SetActionCode(TicketActionCodeNoValue);
			break;
		case 0x80: // Cashable ticket redeemed
			m_responseTicket->SetType(VoucherTicket,false);
			m_responseTicket->SetActionCode(TicketActionCodeNoValue);
			break;
		case 0x81: // Restricted promotional ticket redeemed
			m_responseTicket->SetType(CouponPromo,false);
			m_responseTicket->SetActionCode(TicketActionCodeNoValue);
			break;
		case 0x82: // Nonrestricted promotional ticket redeemed
			m_responseTicket->SetType(VoucherPromo,false);
			m_responseTicket->SetActionCode(TicketActionCodeNoValue);
			break;
		}

		// buffer index number
		byte index;
		memcpy(&index, response, sizeof(index));
		response += sizeof(byte);
		if (index != 0)
		{
			

			byte dateArray[4];
			// date is 4 BCD: MMDDYYYY
			memcpy(dateArray, response, sizeof(dateArray));
			response += sizeof(dateArray);

			// time is 3 BCD: HHMMSS
			byte timeArray[3];
			memcpy(timeArray, response, sizeof(timeArray));
			response += sizeof(timeArray);
			time_t validationDate = CUtilities::MakeTimeFromMMDDYYYY_HHMMSS(dateArray, timeArray);
			m_responseTicket->SetGameDateTime(validationDate);

			// validation number: 8 byte BCD number.
			byte validationNumber[9];
			memcpy(&validationNumber[1],response,8);
			response += 8;

			// amount -- 5 byte BCD number
			m_responseTicket->SetAmount(Bcd2Int64(response,5));
			response += 5;

			// 2 byte ticket number
			WORD ticketNumber;
			memcpy(&ticketNumber, response, sizeof(ticketNumber));
			response += sizeof(ticketNumber);
			m_responseTicket->SetTicketNumber(ticketNumber);

			// Validation SystemID, 1 byte BCD
			memcpy(&validationNumber, response, sizeof(byte));
			response += sizeof(byte);
			m_responseTicket->SetValidationNumber(validationNumber, 9);

			// expiration field is either MMDDYYYY or 00009999 in days.
			if (response[0] == 0)
			{
				// expiration in days
				WORD days = (WORD) Bcd2Dword(response,4);
				m_responseTicket->SetExpirationType(EXPIRES_DAYS);
				if (days > 254)
					days = 254;
				m_responseTicket->SetExpirationDuration((byte)days);
			}
			else
			{
				time_t expireDate = CUtilities::MakeTimeFromMMDDYYYY_HHMMSS(response);
				time_t expireDuration = expireDate - validationDate;
				m_responseTicket->SetExpirationType(EXPIRES_DAYS);
				m_responseTicket->SetExpirationDuration((byte)(expireDuration / SECONDS_PER_DAY));
				if ((expireDuration / SECONDS_PER_DAY) <= 0)
				{
					// Something wrong, use what the system should be configured to
					m_responseTicket->SetExpirationDuration((byte)m_defaultExpirationDuration);
				}
				else
					m_responseTicket->SetExpirationDuration((byte)(expireDuration / SECONDS_PER_DAY));
			}
			response += 4;

			// Pool ID, use???
			WORD poolId;
			memcpy(&poolId, response, sizeof(poolId));
			response += 2;
		}
		else
		{
			// poll succesful, but no ticket data returned
			m_nReturnValue = SASPOLL_INCOMPLETE_DATA;
		}
	}
	CSASPollMessage::ResponseReceived();
}

void CLongPoll4DMessage::Fill()
{
	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x4D;
	if ( m_bPeek )
		m_sendBuffer[2] = 0xFF;
	else
		m_sendBuffer[2] = 0x00;


	// TBD. This will need to get changed.

	ushort messageCRC = CRC(GetSendBuffer(),GetSendBufferSize() - CRCSIZE,0);
	byte *crc = (byte *)&m_sendBuffer[3];
	memcpy(crc,&messageCRC,CRCSIZE);
}

