#include "stdafx.h"
#include "LongPoll7BMessage.h"

#include "Utilities.h"

// LSB for m_controlMask and m_status.
const int PRINTER_CASHOUT_BYTE = 0;
const int PRINTER_CASHOUT_BIT = 0;

const int PRINTER_HANDPAY_RECEIPT_BYTE = 0;
const int PRINTER_HANDPAY_RECEIPT_BIT = 1;

const int VALIDATE_HANDPAY_BYTE = 0;
const int VALIDATE_HANDPAY_BIT = 2;

const int PRINT_RESTRICTED_TICKETS_BYTE = 0;
const int PRINT_RESTRICTED_TICKETS_BIT = 3;

const int FOREIGN_RESTRICTED_AMOUNTS_BYTE = 0;
const int FOREIGN_RESTRICTED_AMOUNTS_BIT = 4;

const int TICKET_REDEMPTION_BYTE = 0;
const int TICKET_REDEMPTION_BIT = 5;

// MSB for m_controlMask and m_status.
const int SECURE_ENHANCED_VALIDATION_BYTE = 0;  // SAS documentation indicates BYTE 1.
const int SECURE_ENHANCED_VALIDATION_BIT = 7;

CLongPoll7BMessage::CLongPoll7BMessage(byte gameAddress) :
	CSASPollMessage(gameAddress),
	m_cashableExpiration(0),
	m_restrictedExpiration(0),
	m_assetNumber(0)
{
	// allocate buffers
	SetSendBuffer((byte *)new byte[X7B_COMMANDSIZE],X7B_COMMANDSIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	SetResponseBuffer((byte*)new byte[X7B_RESPONSE_SIZE],X7B_RESPONSE_SIZE);
	memset(GetResponseBuffer(),0,GetResponseBufferSize());

	ZeroMemory(m_controlMask, sizeof(m_controlMask));
	ZeroMemory(m_status, sizeof(m_status));
}

CLongPoll7BMessage::~CLongPoll7BMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll7BMessage::GetPollValue()
{
	return POLL_VALUE;
}

void CLongPoll7BMessage::ResponseReceived()
{

	if (m_nReturnValue == SASPOLL_SUCCESS)
	{

		byte *response = GetResponseBuffer();
		response += OPTIONAL_DATA_OFFSET;
		memcpy(&m_assetNumber, response, sizeof(m_assetNumber));
		response += sizeof(m_assetNumber);

		memcpy(m_status, response, sizeof(WORD));
		response += sizeof(WORD);

		memcpy(&m_cashableExpiration, response, sizeof(m_cashableExpiration));
		response += sizeof(m_cashableExpiration);

		memcpy(&m_restrictedExpiration, response, sizeof(m_restrictedExpiration));
	}

	CSASPollMessage::ResponseReceived();
}

void CLongPoll7BMessage::Fill()
{
	// fill long poll command data
	
	m_sendBuffer[GAME_ADDRESS_OFFSET] = m_gameAddress;
	m_sendBuffer[POLL_VALUE_OFFSET] = POLL_VALUE;
	m_sendBuffer[OPTIONAL_LENGTH_OFFSET] = X7B_DEFSIZE;
	CUtilities::MemCopyZero(m_sendBuffer + CONTROL_MASK_OFFSET, CONTROL_MASK_SIZE, m_controlMask, sizeof(m_controlMask));
	CUtilities::MemCopyZero(m_sendBuffer + STATUS_OFFSET, STATUS_SIZE, m_status, sizeof(m_status));
	Dword2Bcd(m_cashableExpiration, &m_sendBuffer[CASHABLE_EXPIRATION_OFFSET], CASHABLE_EXPIRATION_SIZE);
	Dword2Bcd(m_restrictedExpiration, &m_sendBuffer[RESTRICTED_EXPIRATION_OFFSET], CASHABLE_EXPIRATION_SIZE);
	byte *crcLocation;
	ushort messageCRC = CRC(GetSendBuffer(),GetSendBufferSize() - CRCSIZE);
	crcLocation = (byte *)&m_sendBuffer[X7B_COMMANDSIZE - CRCSIZE];
	memcpy(crcLocation,&messageCRC,CRCSIZE);

	// Response is structured like a variable response, but appears to be static.
	//m_isVariableResponse = true;
	//m_variableResponseLenghLocation = X7B_LENGTHBYTE; // length does NOT include CRC bytes.
	//m_variableResponseLengthSize = X7B_LENGTHSIZE;
	//m_variableResponseSizeConstant = X7B_RESPONSE_EXTRADATASIZE; // game address, game command, length byte, ..... , 2 byte CRC
}

void CLongPoll7BMessage::SetPrinterCashout(bool value)
{
	m_controlMask[PRINTER_CASHOUT_BYTE] |= 1 << PRINTER_CASHOUT_BIT;
	m_status[PRINTER_CASHOUT_BYTE] |= (value ? 1 : 0) << PRINTER_CASHOUT_BIT;
}

bool CLongPoll7BMessage::GetPrinterCashout() const
{
	return (m_status[PRINTER_CASHOUT_BYTE] & (1 << PRINTER_CASHOUT_BIT)) ? true : false;
}

void CLongPoll7BMessage::SetPrinterHandpayReceipt(bool value)
{
	m_controlMask[PRINTER_HANDPAY_RECEIPT_BYTE] |= 1 << PRINTER_HANDPAY_RECEIPT_BIT;
	m_status[PRINTER_HANDPAY_RECEIPT_BYTE] |= (value ? 1 : 0) << PRINTER_HANDPAY_RECEIPT_BIT;
}

bool CLongPoll7BMessage::GetPrinterHandpayReceipt() const
{
	return (m_status[PRINTER_HANDPAY_RECEIPT_BYTE] & (1 << PRINTER_HANDPAY_RECEIPT_BIT)) ? true : false;
}

void CLongPoll7BMessage::SetValidateHandpays(bool value)
{
	m_controlMask[VALIDATE_HANDPAY_BYTE] |= 1 << VALIDATE_HANDPAY_BIT;
	m_status[VALIDATE_HANDPAY_BYTE] |= (value ? 1 : 0) << VALIDATE_HANDPAY_BIT;
}

bool CLongPoll7BMessage::GetValidateHandpays() const
{
	return (m_status[VALIDATE_HANDPAY_BYTE] & (1 << VALIDATE_HANDPAY_BIT)) ? true : false;
}

void CLongPoll7BMessage::SetPrintRestrictedTickets(bool value)
{
	m_controlMask[PRINT_RESTRICTED_TICKETS_BYTE] |= 1 << PRINT_RESTRICTED_TICKETS_BIT;
	m_status[PRINT_RESTRICTED_TICKETS_BYTE] |= (value ? 1 : 0) << PRINT_RESTRICTED_TICKETS_BIT;
}

bool CLongPoll7BMessage::GetPrintRestrictedTickets() const
{
	return (m_status[PRINT_RESTRICTED_TICKETS_BYTE] & (1 << PRINT_RESTRICTED_TICKETS_BIT)) ? true : false;
}

void CLongPoll7BMessage::SetForeignRestrictedAmounts(bool value)
{
	m_controlMask[FOREIGN_RESTRICTED_AMOUNTS_BYTE] |= 1 << FOREIGN_RESTRICTED_AMOUNTS_BIT;
	m_status[FOREIGN_RESTRICTED_AMOUNTS_BYTE] |= (value ? 1 : 0) << FOREIGN_RESTRICTED_AMOUNTS_BIT;
}

bool CLongPoll7BMessage::GetForeignRestrictedAmounts() const
{
	return (m_status[FOREIGN_RESTRICTED_AMOUNTS_BYTE] & (1 << FOREIGN_RESTRICTED_AMOUNTS_BIT)) ? true : false;
}

void CLongPoll7BMessage::SetTicketRedemption(bool value)
{
	m_controlMask[TICKET_REDEMPTION_BYTE] |= 1 << TICKET_REDEMPTION_BIT;
	m_status[TICKET_REDEMPTION_BYTE] |= (value ? 1 : 0) << TICKET_REDEMPTION_BIT;
}

bool CLongPoll7BMessage::GetTicketRedemption() const
{
	return (m_status[TICKET_REDEMPTION_BYTE] & (1 << TICKET_REDEMPTION_BIT)) ? true : false;
}

void CLongPoll7BMessage::SetSecureEnhancedConfig(bool value)
{
	// Note:  According to SAS documentation:
	// The "SECURE_ENHANCED_VALIDATION_BIT" on byte 0 (of m_controlMask and m_status) is "Reserved".
	// The SECURE_ENHANCED_VALIDATION_BIT is actually on byte 1 (of m_controlMask and m_status).
	// If compatibility with SAS documentation is needed, change the operations from byte 0 to byte 1 (of m_controlMask and m_status).
	m_controlMask[SECURE_ENHANCED_VALIDATION_BYTE] |= 1 << SECURE_ENHANCED_VALIDATION_BIT;
	m_status[SECURE_ENHANCED_VALIDATION_BYTE] |= (value ? 1 : 0) << SECURE_ENHANCED_VALIDATION_BIT;
}

bool CLongPoll7BMessage::GetSecureEnhancedConfig() const
{
	// Note: According to SAS documentation:
	// The "SECURE_ENHANCED_VALIDATION_BIT" on byte 0 (of m_status) is "Reserved".
	// The SECURE_ENHANCED_VALIDATION_BIT is actually on byte 1 (of m_status).
	// If compatibility with SAS documentation is needed, change the operation from byte 0 to byte 1 (of m_status).
	return (m_status[SECURE_ENHANCED_VALIDATION_BYTE] & (1 << SECURE_ENHANCED_VALIDATION_BIT)) ? true : false;
}

void CLongPoll7BMessage::SetCashableExpiration(WORD value)
{
	m_cashableExpiration = value;
}

WORD CLongPoll7BMessage::GetCashableExpiration() const
{
	return m_cashableExpiration;
}

void CLongPoll7BMessage::SetRestrictedExpiration(WORD value)
{
	m_restrictedExpiration = value;
}

WORD CLongPoll7BMessage::GetRestrictedExpiration() const
{
	return m_restrictedExpiration;
}

DWORD CLongPoll7BMessage::GetAssetNumber() const
{
	return m_assetNumber;
}
