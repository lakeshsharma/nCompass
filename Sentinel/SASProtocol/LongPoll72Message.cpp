#include "stdafx.h"
#include "LongPoll72Message.h"
#include "../Utilities.h"
#include "../Sentinel.h"
#include "../Logging/LogString.h"

const byte X72_RESPONSE_SIZE = 93;
const byte X72_SEND_SIZE = 96;
const byte X72_SEND_STATUS_SIZE = 5;
const byte X72_SEND_STATIC_DATA_SIZE = 56;
const byte X72_RESPONSE_EXTRADATASIZE = 5;
const byte X72_SEND_EXTRADATASIZE = 5;
const byte X72_LENGTHBYTE = 2;
const byte X72_LENGTHSIZE = 1;
const byte VALID_CASHABLE_TICKET = 0;
const byte VALID_RESTRICTED_PROMO = 1;
const byte VALID_NONRESTRICTED_PROMO = 2;
const double SECSPERDAY = 60 * 60 * 24;

CLongPoll72Message::CLongPoll72Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[X72_SEND_SIZE],X72_SEND_SIZE);
	SetResponseBuffer((BYTE *)new byte[X72_RESPONSE_SIZE],X72_RESPONSE_SIZE);
	memset(GetSendBuffer(),0,GetSendBufferSize());
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
	m_cashableAmount = m_restrictedAmount = m_nonrestrictedAmount = 0;
	m_transferCode = 0; // always use full transfer only
	m_transferFlags = 0;
	SetTransferFlag(TRANSFER_FLAG_ACCEPT_TRANSFER_ONLY_IF_LOCKED,true);
	m_poolID = 0;
	m_expiration = 1; // 1 day
	m_receiptDataLength = 0;
	m_transferStatus = 0xff;
	m_transactionIDLength = 0;
}

CLongPoll72Message::~CLongPoll72Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll72Message::GetPollValue()
{
	return 0x72;
}

bool CLongPoll72Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll72Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		byte *response = GetResponseBuffer();
		response += 3; // first byte of actual data

		// transaction buffer position
		response += sizeof(byte);

		// Tracker #30848, it is incorrect to use default value for transaction status, commenting out tmpTransferStatus code
		
		// transfer status
		//byte tmpTransferStatus = m_transferStatus; // keep backup copy
		memcpy(&m_transferStatus, response, sizeof(m_transferStatus));
		response += sizeof(m_transferStatus);

		// Tracker #23891
		if (m_transferStatus != 0xff) // PBT Result received
		{

			// receipt status
			memcpy(&m_receiptStatus, response, sizeof(m_receiptStatus));
			response += sizeof(m_receiptStatus);

			// transfer type
			memcpy(&m_transferType, response, sizeof(m_transferType));
			response += sizeof(m_transferType);

			// cashable amount
			m_cashableAmount = Bcd2Dword(response, 5);
			response += 5;

			// restricted amount
			m_restrictedAmount = Bcd2Dword(response, 5);
			response += 5;

			// nonrestricted amount
			m_nonrestrictedAmount = Bcd2Dword(response, 5);
			response += 5;

			// transfer flags
			memcpy(&m_transferFlags, response, sizeof(m_transferFlags));
			response += sizeof(m_transferFlags);

			// asset number
			memcpy(&m_assetNumber, response, sizeof(m_assetNumber));
			response += sizeof(m_assetNumber);

			// transaction id length
			memcpy(&m_transactionIDLength, response, sizeof(m_transactionIDLength));
			response += sizeof(m_transactionIDLength);

			if (m_transactionIDLength > 0)
			{
				// transaction id
				memcpy(m_transactionID, response, m_transactionIDLength);
				response += m_transactionIDLength;
			}
			else
			{
				// no transaction id, clear to 0's
				memset(m_transactionID, 0, sizeof(m_transactionID));
			}

			// if status is pending, then date will be 0's
			if (m_transferStatus == 0x00)
			{
				byte dateArray[4];
				// date is 4 BCD: MMDDYYYY
				memcpy(dateArray, response, sizeof(dateArray));
				response += sizeof(dateArray);

				// time is 3 BCD: HHMMSS
				byte timeArray[3];
				memcpy(timeArray, response, sizeof(timeArray));
				response += sizeof(timeArray);
				m_transactionDate = CUtilities::MakeTimeFromMMDDYYYY_HHMMSS(dateArray, timeArray);

				// expiration field is either MMDDYYYY or 00009999 in days.
				if (response[0] == 0)
				{
					// expiration in days
					m_expirationDays = (WORD) Bcd2Dword(response,4);
				}
				else
				{
					time_t expireDate = CUtilities::MakeTimeFromMMDDYYYY_HHMMSS(response);
					double expireDuration = expireDate - CUtilities::GetCurrentTime();
					// m_expirationDays is NOT critical if value > 0xffff.  2009-10-01 Alan Sheldon
					m_expirationDays = (WORD)expireDuration/SECSPERDAY;
				}
			}
			else
				m_expirationDays = 0;
			response += 4;

			// Pool ID, use???
			memcpy(&m_poolID, response, sizeof(m_poolID));
			response += 2;

			byte meterSize;

			// Cumulative cashable amount meter size
			memcpy(&meterSize, response, sizeof(meterSize));
			response += sizeof(meterSize);

			if (meterSize > 0)
			{
				m_cumulativeCashableAmountMeter = Bcd2Dword(response, meterSize);
				response += meterSize;
			}

			// Cumulative restricted amount meter size
			memcpy(&meterSize, response, sizeof(meterSize));
			response += sizeof(meterSize);

			if (meterSize > 0)
			{
				m_cumulativeRestrictedAmountMeter = Bcd2Dword(response, meterSize);
				response += meterSize;
			}

			// Cumulative nonrestricted amount meter size
			memcpy(&meterSize, response, sizeof(meterSize));
			response += sizeof(meterSize);

			if (meterSize > 0)
			{
				m_cumulativeNonrestrictedAmountMeter = Bcd2Dword(response, meterSize);
            }
		}

	}

	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPoll72Message::Fill()
{
	// fill long poll command data

	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x72;
	if (m_statusQuery)
	{
		LogString(SASGENERAL, "PBT: Long Poll 72 -- Query Status = Yes");
		m_sendBuffer[2] = 2;
		m_sendBuffer[3] = 0xff;
		m_sendBuffer[4] = 0;
		ushort messageCRC = CRC(GetSendBuffer(),X72_SEND_STATUS_SIZE,0);
		memcpy(&m_sendBuffer[5],&messageCRC,CRCSIZE);
		SetSendBuffer(GetSendBuffer(),X72_SEND_STATUS_SIZE + CRCSIZE);
	}
	else
	{
		LogString(SASGENERAL, "PBT: Long Poll 72 -- Query Status = No");

		byte dataLength = X72_SEND_STATIC_DATA_SIZE - X72_SEND_EXTRADATASIZE + m_transactionIDLength + m_receiptDataLength ;
		dataLength += 2; // Tracker 018406 -- add 2 bytes for receipt type code/receipt length
		m_sendBuffer[2] = dataLength;

		// transfer code
		memcpy(&m_sendBuffer[3],&m_transferCode,sizeof(m_transferCode));

		// transaction index
		m_sendBuffer[4] = 0; // always zero

		// transfer type
		m_sendBuffer[5] = m_transferType;

		// cashable amount
		Dword2Bcd(m_cashableAmount,&m_sendBuffer[6],5);

		// restricted amount
		Dword2Bcd(m_restrictedAmount,&m_sendBuffer[11],5);

		// nonrestructed amount
		Dword2Bcd(m_nonrestrictedAmount,&m_sendBuffer[16],5);

		// transfer flags
		m_sendBuffer[21] = m_transferFlags;

		// Asset number
		memcpy(&m_sendBuffer[22],&m_assetNumber,sizeof(m_assetNumber));

		// Registration Key
		memset(&m_sendBuffer[26], 0, 20); // set 20 byte registration key to 0's

		// transaction id length
		memcpy(&m_sendBuffer[46], &m_transactionIDLength, sizeof(m_transactionIDLength));

		// transaction id
		memcpy(&m_sendBuffer[47], m_transactionID, m_transactionIDLength);

		byte *sendIndex = &m_sendBuffer[47] + m_transactionIDLength;

		//SS TR# 25892
		if (m_restrictedAmount > 0)
		{
			//restricted transfers require expiration days = 0
			m_expiration = 0;
		}

		// Expiration 0000NNNN
		Dword2Bcd(m_expiration, sendIndex, 4);
		sendIndex += 4;

		// pool id
		memcpy(sendIndex, &m_poolID, sizeof(m_poolID));
		sendIndex += sizeof(m_poolID);

		// Receipt data length
		// Tracker 018406
		byte receiptLength = m_receiptDataLength + 2;
		memcpy(sendIndex, &receiptLength, sizeof(receiptLength));
		sendIndex += sizeof(receiptLength);
		byte receiptCode = 0x11;
		memcpy(sendIndex,&receiptCode,sizeof(receiptCode));
		sendIndex += sizeof(receiptCode);
		memcpy(sendIndex, &m_receiptDataLength, sizeof(m_receiptDataLength));
		sendIndex += sizeof(m_receiptDataLength);

		if (m_receiptDataLength > 0)
		{
			// receipt data
			memcpy(sendIndex, &m_receiptData, m_receiptDataLength);
			sendIndex += m_receiptDataLength;
		}


		ushort messageCRC = CRC(GetSendBuffer(),sendIndex - GetSendBuffer(),0);
		memcpy(sendIndex,&messageCRC,CRCSIZE);
		SetSendBuffer(GetSendBuffer(),sendIndex - GetSendBuffer() + CRCSIZE);

	}

	// Configure response buffer
	m_isVariableResponse = true;
	m_variableResponseLenghLocation = X72_LENGTHBYTE; // length does NOT include CRC bytes.
	m_variableResponseLengthSize = X72_LENGTHSIZE;
	m_variableResponseSizeConstant = X72_RESPONSE_EXTRADATASIZE; // game address, game command, length byte, ..... , 2 byte CRC

	return;
}

void CLongPoll72Message::SetTransferCode(byte transferCode)
{
	m_transferCode = transferCode;
}

byte CLongPoll72Message::GetTransferCode()
{
	return m_transferCode;
}

void CLongPoll72Message::SetTransferType(byte transferType)
{
	m_transferType = transferType;
}

byte CLongPoll72Message::GetTransferType()
{
	return m_transferType;
}

void CLongPoll72Message::SetCashableAmount(DWORD cashableAmount)
{
	m_cashableAmount = cashableAmount;
}

DWORD CLongPoll72Message::GetCashableAmount()
{
	return m_cashableAmount;
}

void CLongPoll72Message::SetRestrictedAmount(DWORD restrictedAmount)
{
	m_restrictedAmount = restrictedAmount;
}

DWORD CLongPoll72Message::GetRestrictedAmount()
{
	return m_restrictedAmount;
}

void CLongPoll72Message::SetNonrestrictedAmount(DWORD nonrestrictedAmount)
{
	m_nonrestrictedAmount = nonrestrictedAmount;
}

DWORD CLongPoll72Message::GetNonrestrictedAmount()
{
	return m_nonrestrictedAmount;
}

void CLongPoll72Message::SetTransferFlag(byte bit, bool val)
{
	// set bit of transfer flag to val
	if (val)
		m_transferFlags |= (1 << bit);
	else
		m_transferFlags &= ~(1 << bit);

}

bool CLongPoll72Message::GetTransferFlag(byte bit)
{
	byte bitVal;
	bitVal = m_transferFlags & (1 << bit);
	return bitVal > 0 ? true : false;
}


void CLongPoll72Message::SetAssetNumber(DWORD assetNumber)
{
	m_assetNumber = assetNumber;
}

DWORD CLongPoll72Message::GetAssetNumber()
{
	return m_assetNumber;
}

void CLongPoll72Message::SetTransactionID(byte *transactionID, byte length)
{
	memcpy(m_transactionID, transactionID, length);
	m_transactionIDLength = length;
}

void CLongPoll72Message::SetExpiration(WORD days)
{
	m_expiration = days;
}

WORD CLongPoll72Message::GetExpiration()
{
	return m_expiration;
}

void CLongPoll72Message::SetPoolID(WORD poolID)
{
	m_poolID = poolID;
}

WORD CLongPoll72Message::GetPoolID()
{
	return m_poolID;
}

void CLongPoll72Message::SetReceiptData(byte *receiptData, byte length)
{
	memcpy(m_receiptData, receiptData, length);
	m_receiptDataLength = length;
}
