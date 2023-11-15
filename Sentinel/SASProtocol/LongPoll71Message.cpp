#include "stdafx.h"

#include "LongPoll71Message.h"
#include "../Utilities.h"
#include "../Sentinel.h"

const byte X71_RESPONSE_SIZE = 44;
const byte X71_SEND_SIZE = 50;
const byte X71_SEND_STATIC_DATA_SIZE = 13;
const byte X71_RESPONSE_EXTRADATASIZE = 5;
const byte X71_SEND_EXTRADATASIZE = 5;
const byte X71_LENGTHBYTE = 2;
const byte X71_LENGTHSIZE = 1;
const byte VALID_CASHABLE_TICKET = 0;
const byte VALID_RESTRICTED_PROMO = 1;
const byte VALID_NONRESTRICTED_PROMO = 2;

const DWORD CLongPoll71Message::DaysInYear = 365; // assume 365 days per year

CLongPoll71Message::CLongPoll71Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[X71_SEND_SIZE],X71_SEND_SIZE);
	SetResponseBuffer((BYTE *)new byte[X71_RESPONSE_SIZE],X71_RESPONSE_SIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	m_sendTicket = m_responseTicket = NULL;
	m_ticketStatus = TICKET_IN_STATUS_UNKNOWN;
	m_bValidationExtensions = true;
}

CLongPoll71Message::~CLongPoll71Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll71Message::GetPollValue()
{
	return 0x71;
}

bool CLongPoll71Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll71Message::SetTicketExpirationDuration(WORD ticketExpirationDuration)
{
	m_ticketExpirationDuration = ticketExpirationDuration;
}

void CLongPoll71Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		m_responseTicket = new CTicket();

		m_responseTicket->SetTicketNumber(0);
		byte *response = GetResponseBuffer();
		response += 3; // first byte of actual data

		byte ticketStatus;		
		memcpy(&ticketStatus, response, sizeof(ticketStatus));
		response += sizeof(ticketStatus);

		// Set ticket action code

		m_responseTicket->SetType(VoucherTicket); 

		switch (ticketStatus)
		{
		case 0: /* cashable ticket redeemed */
			m_responseTicket->SetActionCode(VendComplete);
			m_ticketStatus = TICKET_IN_STATUS_STACKED;
			m_responseTicket->SetType(VoucherTicket); 
			break;
		case 1: /* restricted ticket redeemed */
			m_responseTicket->SetActionCode(VendComplete);
			m_ticketStatus = TICKET_IN_STATUS_STACKED;
			m_responseTicket->SetType(CouponPromo); 
			break;
		case 2: /* nonrestricted ticket redeemed */
			m_responseTicket->SetActionCode(VendComplete);
			m_ticketStatus = TICKET_IN_STATUS_STACKED;
			m_responseTicket->SetType(VoucherPromo); 
			break;
		case 0x40:
			m_responseTicket->SetActionCode(TicketActionCodeNoValue);
			m_ticketStatus = TICKET_IN_STATUS_PENDING;
			break;
		case 0x80:
		case 0x81:
		case 0x82:
		case 0x83:
		case 0x84:
		case 0x85:
		case 0x86:
		case 0x87:
		case 0x88:
		case 0x89:
		case 0x8a:
		case 0x8b:
		case 0xc0:
			m_responseTicket->SetActionCode(VoucherRejectedByGame);
			m_ticketStatus = TICKET_IN_STATUS_ERROR;
			break;
		default:
			m_responseTicket->SetActionCode(TicketActionCodeNoValue);
			m_ticketStatus = TICKET_IN_STATUS_PENDING;
			break;
		}

		m_responseTicket->SetAmount(Bcd2Dword(response,5));
		response += 5;

		byte parsingCode;
		memcpy(&parsingCode, response, sizeof(parsingCode));
		response += 1;
		
		// should be zero
		if (parsingCode == 0)
		{
			m_responseTicket->SetValidationNumber(response,9);
			response += 9;

		}

		// default to now
		m_responseTicket->SetGameDateTime(CUtilities::GetCurrentTime());

			
	}
	else 
		m_responseTicket = NULL;

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll71Message::Fill()
{
	// fill long poll command data

	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x71;
	if (m_statusQuery)
	{
		m_sendBuffer[2] = 1;
		m_sendBuffer[3] = 0xff;
		ushort messageCRC = CRC(GetSendBuffer(),4,0);
		memcpy(&m_sendBuffer[4],&messageCRC,CRCSIZE);
		SetSendBuffer(GetSendBuffer(),6);
	}
	else
	{
		byte dataLength = X71_SEND_STATIC_DATA_SIZE + m_sendTicket->ValidationLength()  - (m_bValidationExtensions ? 0 : 1);
		m_sendBuffer[2] = dataLength;



		// transfer code
		byte transferCode = 0x80;
		switch (m_sendTicket->ResultCode())
		{
		case AuthorizedToVendVoucher:
			// Set ticket type
			switch(m_sendTicket->Type())
			{
			case VoucherTicket:
				transferCode = VALID_CASHABLE_TICKET;
				break;
			case CouponPromoGameGenerated:
				transferCode = VALID_RESTRICTED_PROMO;
				break;
			case VoucherPromo:
				transferCode = VALID_NONRESTRICTED_PROMO;
				break;
			case CouponPromo:
				transferCode = VALID_RESTRICTED_PROMO;
				break;
			default:
				break;
			}
			break;
		case RejectVoucher:
			transferCode = 0x80;
			break;
		case RejectVoucherAlreadyRedeemed:
			transferCode = 0x84;
			break;
		case RejectVoucherNoMatch:
			transferCode = 0x82;
			break;
		case RejectVoucherInvalidNumber:
			transferCode = 0x81;
			break;
		case RejectVoucherVoided:
			transferCode = 0x80;
			break;
		case RejectVoucherExpired:
			transferCode = 0x85;
			break;
		case RejectVoucherMultipleMatches:
			transferCode = 0x80;
			break;
		case RejectVoucherStaleRequest:
			transferCode = 0x80;
			break;
		case RejectVoucherReqFromFuture:
			transferCode = 0x80 ;
			break;
		case RejectVoucherNotRedeemable:
			transferCode = 0x80;
			break;
		case RejectVoucherRedeemInProgress:
			transferCode = 0x83;
			break;
		// rest of transfer codes...
		default:
			break;
		}
		LogString(SASGENERAL, "Long Poll 0x71 Fill response code %d ---> %d", m_sendTicket->ResultCode(),transferCode);

		memcpy(&m_sendBuffer[3],&transferCode,sizeof(transferCode));
		Dword2Bcd((DWORD)m_sendTicket->Amount(),&m_sendBuffer[4],5);

		memset(&m_sendBuffer[9],0,1); // parsing code is always 0
		byte valLength;
		const byte *validationNumber = m_sendTicket->ValidationNumber(valLength);
		memcpy(&m_sendBuffer[10],validationNumber,valLength);
		byte *sendPtr = &m_sendBuffer[10];
		sendPtr += valLength;

        // The ticket timestamp	and current time
	   time_t validationDate = m_sendTicket->GameDateTime();
	   time_t currentDate = CUtilities::GetCurrentTime();
        
        // Determine the days to expire
        DWORD expireDuration = m_ticketExpirationDuration;
		switch (m_sendTicket->ExpirationType())
		{
		case EXPIRES_DAYS:
			break;
		case EXPIRES_YEARS:
			expireDuration *= DaysInYear; 
			break;
		default:
			break;
		}

        // Get a BCD formated expiration date
		const int ExpireBCDSize = 4; 
        byte expireDateBCD[ExpireBCDSize];
        FormatTicketExpire(validationDate, currentDate, expireDuration, expireDateBCD, ExpireBCDSize);

		memcpy(sendPtr, expireDateBCD,sizeof(expireDateBCD));
		sendPtr += sizeof(expireDateBCD);

		if (m_bValidationExtensions)
		{
			// Set default pool id
			// TBD: restricted coupons would get something other than 0000
			memset(sendPtr, 0, 2);
			sendPtr += 2;
		}

		ushort messageCRC = CRC(GetSendBuffer(),sendPtr - GetSendBuffer(),0);
		memcpy(sendPtr,&messageCRC,CRCSIZE);
		SetSendBuffer(GetSendBuffer(),sendPtr - GetSendBuffer() + CRCSIZE);

	}

	// Configure response buffer
	m_isVariableResponse = true;
	m_variableResponseLenghLocation = X71_LENGTHBYTE; // length does NOT include CRC bytes.
	m_variableResponseLengthSize = X71_LENGTHSIZE;
	m_variableResponseSizeConstant = X71_RESPONSE_EXTRADATASIZE; // game address, game command, length byte, ..... , 2 byte CRC

	return;
}


void CLongPoll71Message::FormatTicketExpire(time_t validationDate, time_t currentDate, DWORD expireDurationDays, byte *expireBCD, int expireBCDLen)
{
    // Determine how many days until we expire
    DWORD daysToExpire = 0;
    DWORD currentDays =  currentDate / (SECONDS_PER_DAY);
    DWORD validationDays = validationDate / (SECONDS_PER_DAY);
    if (validationDays + expireDurationDays > currentDays) 
    {
        daysToExpire = validationDays + expireDurationDays - currentDays;
    }

    // Get the BCD expiration date
    if (daysToExpire > DaysInYear) 
    {
        // If we expire in more than 365 days, set the BCD days to expire in 0000NNNN days format
        Dword2Bcd(min(daysToExpire, (DWORD)MAX_DAYS_TO_EXPIRE_TICKETS), expireBCD, expireBCDLen);
    }
    else
    {
        // calculate expiration date in MMDDYYYY format
        time_t daysSpan = expireDurationDays * SECONDS_PER_DAY;
        time_t expireDate = validationDate + daysSpan;
        CUtilities::MakeMMDDYYYYFromTime(expireDate, expireBCD);
    }
}

