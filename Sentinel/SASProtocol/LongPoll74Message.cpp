#include "stdafx.h"
#include "LongPoll74Message.h"
#include "../Utilities.h"
#include "../Sentinel.h"

const byte X74_RESPONSE_SIZE = 40;
const byte X74_SEND_SIZE = 8;
const byte X74_RESPONSE_EXTRADATASIZE = 5;
const byte X74_LENGTHBYTE = 2;
const byte X74_LENGTHSIZE = 1;
const double SECSPERDAY = 60 * 60 * 24;

CLongPoll74Message::CLongPoll74Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[X74_SEND_SIZE],X74_SEND_SIZE);
	SetResponseBuffer((BYTE *)new byte[X74_RESPONSE_SIZE],X74_RESPONSE_SIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
	m_transferCondition = 0;
}

CLongPoll74Message::~CLongPoll74Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll74Message::GetPollValue()
{
	return 0x74;
}

bool CLongPoll74Message::SwapPollValue()
{
	return 0;
}

void CLongPoll74Message::SetXferToGame(bool bEnabled)
{
	if (bEnabled)
		m_transferCondition |= XFER_TO_GAME;
	else
		m_transferCondition &= ~XFER_TO_GAME;
}

void CLongPoll74Message::SetXferFromGame(bool bEnabled)
{
	if (bEnabled)
		m_transferCondition |= XFER_FROM_GAME;
	else
		m_transferCondition &= ~XFER_FROM_GAME;
}

void CLongPoll74Message::SetXferToPrinter(bool bEnabled)
{
	if (bEnabled)
		m_transferCondition |= XFER_TO_PRINTER;
	else
		m_transferCondition &= ~XFER_TO_PRINTER;
}

void CLongPoll74Message::SetBonusToGame(bool bEnabled)
{
	if (bEnabled)
		m_transferCondition |= COMMAND_BONUS_TO_GAME;
	else
		m_transferCondition &= ~COMMAND_BONUS_TO_GAME;
}

void CLongPoll74Message::Fill()
{
	// fill long poll command data

	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x74;
	m_sendBuffer[2] = m_lockCode;
	m_sendBuffer[3] = m_transferCondition;
	Dword2Bcd(m_lockTimeout,&m_sendBuffer[4],2);
	ushort messageCRC = CRC(GetSendBuffer(),m_sendBufferSize - CRCSIZE,0);
	memcpy(&m_sendBuffer[6],&messageCRC,CRCSIZE);

	// Configure response buffer
	m_isVariableResponse = true;
	m_variableResponseLenghLocation = X74_LENGTHBYTE; // length does NOT include CRC bytes.
	m_variableResponseLengthSize = X74_LENGTHSIZE;
	m_variableResponseSizeConstant = X74_RESPONSE_EXTRADATASIZE; // game address, game command, length byte, ..... , 2 byte CRC

	return;
}

void CLongPoll74Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();

		// first meaningful data
		response += 3;

		memcpy(&m_assetNumber, response, sizeof(m_assetNumber));
		response += sizeof(m_assetNumber);

		memcpy(&m_gameLockStatus, response, sizeof(byte));
		response += sizeof(byte);

		memcpy(&m_availableTransfers, response, sizeof(byte));
		response += sizeof(byte);

		memcpy(&m_hostCashoutStatus, response, sizeof(byte));
		response += sizeof(byte);

		memcpy(&m_AFTStatus, response, sizeof(byte));
		response += sizeof(byte);

		memcpy(&m_maxBufferIndex, response, sizeof(byte));
		response += sizeof(byte);

		m_currentCashableAmount = Bcd2Dword(response, 5);
		response += 5;

		m_currentRestrictedAmount = Bcd2Dword(response, 5);
		response += 5;

		m_currentNonrestrictedAmount = Bcd2Dword(response, 5);
		response += 5;

		m_egmTransferLimit = Bcd2Dword(response, 5);
		response += 5;

		// expiration field is either MMDDYYYY or 00009999 in days.
		if (response[0] == 0)
		{
			// expiration in days

			m_restrictedExpiration = (WORD) Bcd2Dword(response,4);
		}
		else
		{
			time_t expireDate = CUtilities::MakeTimeFromMMDDYYYY_HHMMSS(response);
			double expireDuration = expireDate - CUtilities::GetCurrentTime();
			m_restrictedExpiration = (WORD)expireDuration/SECSPERDAY;
		}
		response += 4;

		memcpy(&m_poolID, response, sizeof(WORD));

	}

	CSASPollMessage::ResponseReceived();

	return;

}

bool CLongPoll74Message::ResponseXferToGame()
{
	byte val = m_availableTransfers & XFER_TO_GAME;
	return  (val > 0 ? true : false);
}

bool CLongPoll74Message::ResponseXferFromGame()
{
	byte val = m_availableTransfers & XFER_FROM_GAME;
	return val ? true : false;
}

bool CLongPoll74Message::ResponseXferToPrinter()
{
	byte val = m_availableTransfers & XFER_TO_PRINTER;
	return val ? true : false;
}

bool CLongPoll74Message::ResponseWinAmountPendingCashoutToHost()
{
	byte val = m_availableTransfers & WIN_AMOUNT_PENDING_CASHOUT_TO_HOST;
	return val ? true : false;
}

bool CLongPoll74Message::ResponseBonusAwardToGamingMachine()
{
	byte val = m_availableTransfers & RESPONSE_BONUS_TO_GAME;
	return val ? true : false;
}


// Response -- Host cashout status
bool CLongPoll74Message::ResponseCashoutToHostControlledByHost()
{
	byte val = m_hostCashoutStatus & CASHOUT_TO_HOST_HOST_CONTROLLED;
	return val ? true : false;
}

bool CLongPoll74Message::ResponseCashoutToHostEnabled()
{
	byte val = m_hostCashoutStatus & CASHOUT_TO_HOST_ENABLED;
	return val ? true : false;
}

bool CLongPoll74Message::ResponseCashoutToHostCurrentlyHard()
{
	byte val = m_hostCashoutStatus & HOST_CASHOUT_MODE;
	return val ? true : false;
}

// Response -- AFT Status
bool CLongPoll74Message::ResponseAFTStatusPrinterAvailable()
{
	byte val = m_AFTStatus & PRINTER_AVAILABLE_RECEIPTS;
	return val ? true : false;
}

bool CLongPoll74Message::ResponseAFTStatusTransferToHostPartialAmounts()
{
	byte val = m_AFTStatus & TRANSFER_TO_HOST_PARTIAL_AMOUNT;
	return val ? true : false;
}

bool CLongPoll74Message::ResponseAFTStatusCustomTicketDataSupported()
{
	byte val = m_AFTStatus & CUSTOM_TICKET_DATA_SUPPORTED;
	return val ? true : false;
}

bool CLongPoll74Message::ResponseAFTStatusAFTRegistered()
{
	byte val = m_AFTStatus & AFT_REGISTERED;
	return val ? true : false;
}

bool CLongPoll74Message::ResponseAFTStatusInHouseTransfersEnabled()
{
	byte val = m_AFTStatus & INHOUSE_TRANSFERS_ENABLED;
	return val ? true : false;
}

bool CLongPoll74Message::ResponseAFTStatusBonusTransfersEnabled()
{
	byte val = m_AFTStatus & BONUS_TRANSFERS_ENABLED;
	return val ? true : false;
}

bool CLongPoll74Message::ResponseAFTStatusDebitTransfersEnabled()
{
	byte val = m_AFTStatus & DEBIT_TRANSFERS_ENABLED;
	return val ? true : false;
}

bool CLongPoll74Message::ResponseAFTStatusAnyAFTEnabled()
{
	byte val = m_AFTStatus & ANY_AFT_ENABLED;
	return val ? true : false;
}
