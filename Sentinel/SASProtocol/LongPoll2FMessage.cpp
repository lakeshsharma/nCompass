#include "stdafx.h"
#include "LongPoll2FMessage.h"
#include "Utilities.h"
#include "../Games.h"
#include "Logging/LogString.h"

CLongPoll2FMessage::CLongPoll2FMessage(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	m_gameNumber = 0;

	m_meters = meters;
	m_isIgtS2000 = false;

	/* do not allocate buffers here, do it in fill after meterlist is setup */
}

CLongPoll2FMessage::~CLongPoll2FMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPoll2FMessage::GetPollValue()
{
	return 0x2F;
}

bool CLongPoll2FMessage::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPoll2FMessage::SetDenomMultiplier(WORD denom)
{
	m_denomMult = denom;
}

void CLongPoll2FMessage::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		const byte *response = GetResponseBuffer();
		const byte *responseEnd = response + GetResponseBufferSize() - CRCSIZE;
		bool ok(true); // Was the entire response formatted correctly?

		// Is the game address correct?
		BYTE gameAddress;
		CUtilities::MemCopySafe_IncSourcePtr(ok, &gameAddress, sizeof(gameAddress), response, responseEnd);
		if (ok)
		{
			ok = gameAddress == m_gameAddress;
		}

		if (ok)
		{
			// Is the poll value correct?
			BYTE pollValue;
			CUtilities::MemCopySafe_IncSourcePtr(ok, &pollValue, sizeof(pollValue), response, responseEnd);
			if (ok)
			{
				ok = pollValue == POLL_VALUE;
			}
		}

		if (ok)
		{
			// Is the length field value correct?
			byte length;
			CUtilities::MemCopySafe_IncSourcePtr(ok, &length, sizeof(length), response, responseEnd);
			if (ok)
			{
				ok = response + length == responseEnd;
			}
		}

		if (ok)
		{
			// Is the game number correct?
			ok = response + X2F_GAMENUMBERSIZE <= responseEnd;
			if (ok)
			{
				DWORD gameNum = Bcd2Dword(response,X2F_GAMENUMBERSIZE);
				response += X2F_GAMENUMBERSIZE;

				ok = (int)gameNum == m_gameNumber;
			}
		}

		if (ok)
		{
			while (response < responseEnd && ok)
			{
				MeterReceived(ok, response, responseEnd);
			}
		}

		if (!ok)
		{
			m_nReturnValue = SASPOLL_NACK; // Not a valid response.
			LogString(ERROR_LOG, "2F m_gameAddress=%02x m_gameNumber=%04u format/parsing error: %s",
				(unsigned)m_gameAddress,
				(unsigned)m_gameNumber,
				CUtilities::ToHexString(GetResponseBuffer(), (DWORD)GetResponseBufferSize()).c_str());

			m_meters->SetAllMetersInvalid(); // Mark all meters as invalid, because the entire response was NOT formatted correctly.
		}
	}
	CSASPollMessage::ResponseReceived();

	return;
}

void CLongPoll2FMessage::MeterReceived(bool &ok, const byte *&response, const byte *responseEnd)
{
	byte meterCode;
	CUtilities::MemCopySafe_IncSourcePtr(ok, &meterCode, sizeof(meterCode), response, responseEnd);
	if (ok)
	{

		int meterValueSize = DEFAULT_METER_VALUE_SIZE;
		if ( meterCode >= smcTotalSasCashableTicketIn_IncludingNonrestricted && meterCode <= smcTotalSasRestrictedTicketOut)
			meterValueSize = LONG_METER_VALUE_SIZE;
		ok = response + meterValueSize <= responseEnd;
		if (ok)
		{
			__int64 meterValue = Bcd2Int64(response,meterValueSize);
			response += meterValueSize;

			// Find the meter number that is associated with this code.
			int meterNumber = GetMeterNumberForMeterCode( meterCode, m_gameNumber );

			// Find the meter in our meter list. We don't want to add any new meters at this point.
			const CMeter *meter = m_meters->GetMeterByNumber( meterNumber );
			if ( meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
				const_cast<CMeter *>(meter)->SetValue( ConvertToCents( meterCode, meterValue ) );
			// handle the few metercodes that translate to multiple oasis meters
			bool updateSecondMeter = false;
			switch(meterCode)
			{
			case smcTotalCreditsFromCoinsToDrop:
				updateSecondMeter = true;
				meterNumber = mActualCD;
				break;
			case smcTotalCreditsFromCoinAcceptor:
				updateSecondMeter = true;
				meterNumber = mTValueCI;
				break;
			case smcTotalCreditsPaidFromHopper:
				updateSecondMeter = true;
				meterNumber = mTValueCO;
				break;
			}

			if (updateSecondMeter)
			{
				meter = m_meters->GetMeterByNumber( meterNumber );
				if ( meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
					const_cast<CMeter *>(meter)->SetValue(ConvertToCents( (byte)meterCode, meterValue) );
			}
		}
	}
}

void CLongPoll2FMessage::SetGameNumber(int gameNumber)
{
	m_gameNumber = gameNumber;
	return;
}


/*
void CLongPoll2FMessage::SetMeterList(byte *meterList, byte meterCount)
{
	m_meterList = meterList;
	m_meterCount = meterCount;

	return;
}
*/

void CLongPoll2FMessage::Fill(bool isIgtS2000)
{
	m_isIgtS2000 = isIgtS2000;
	Fill();
}
void CLongPoll2FMessage::Fill()
{
	// When IGT S2000, do not use the primary meterCode for retreiving meters.  Only use secondary code.
	// Need to enforce 10 meter limit for 2f poll
	byte meterCount = m_meters->GetMeterCount();
	if (meterCount > 10)
		meterCount = 10;

	byte primaryMeterCode = INVALID_SAS_METER_CODE;
	byte secondaryMeterCode = INVALID_SAS_METER_CODE;

	// allocate enough for max meters, need to adjust before exiting fill
	int bufferSize = X2F_MAX_METERS + X2F_EXTRADATASIZE;

	byte *sendData = (BYTE *)new byte[ bufferSize ];
	SetSendBuffer(sendData,bufferSize);


	// fill long poll command data
	m_sendBuffer[0] = m_gameAddress;
	m_sendBuffer[1] = 0x2F;
	// fill in gamenumber bcd
	Dword2Bcd(m_gameNumber,&m_sendBuffer[X2F_GAMENUMBERLOCATION],2);

	// fill in meter codes
	int bufferLoc = 0;
	int i;
	for (i = 0, bufferLoc = 0; i < meterCount; i++)
	{
		const CMeter *meter = m_meters->GetMeterByIndex( i );
		GetMeterCodeForMeterNumber( meter->GetMeterNumber(), m_gameNumber, primaryMeterCode, secondaryMeterCode );
		// Tracker# 018314:
		// Do not use 2f/0x28 poll
		if (primaryMeterCode != INVALID_SAS_METER_CODE && primaryMeterCode != 0x28 &&  (!m_isIgtS2000 || ( primaryMeterCode != 0x2C && primaryMeterCode != 0x2D && primaryMeterCode != 0x28 && primaryMeterCode != 0x2A && primaryMeterCode != 0x2B && primaryMeterCode != 0x29)))
		{
			// get this one as well
			memcpy(&m_sendBuffer[bufferLoc+X2F_METERLOCATION], &primaryMeterCode, 1 );
			bufferLoc += 1;
		}
		if (secondaryMeterCode != INVALID_SAS_METER_CODE)
		{
			// get this one as well
			memcpy(&m_sendBuffer[bufferLoc+X2F_METERLOCATION], &secondaryMeterCode, 1 );
			bufferLoc += 1;
		}
	}

	meterCount = bufferLoc;
	m_sendBuffer[2] = meterCount + 2; // byte count not including CRC
	SetSendBuffer(sendData,X2F_EXTRADATASIZE + meterCount);  // reset buffer size to actual size used
	ushort commandCRC = CRC(m_sendBuffer, GetSendBufferSize() - CRCSIZE,0);
	byte *crcLocation = (byte *) (GetSendBuffer() + GetSendBufferSize() - CRCSIZE);
	memcpy(crcLocation /*GetSendBuffer() + GetSendBufferSize() - CRCSIZE*/,&commandCRC,CRCSIZE);

	// Configure response buffer
	m_isVariableResponse = true;
	m_variableResponseLenghLocation = X2F_LENGTHBYTE; // length does NOT include CRC bytes.
	m_variableResponseLengthSize = X2F_LENGTHSIZE;
	m_variableResponseSizeConstant = X2F_RESPONSE_EXTRADATASIZE; // game address, game command, length byte, ..... , 2 byte CRC

	int responseSize = meterCount*X2F_DEFMETERSIZE + X2F_RESPONSE_EXTRADATASIZE + CRCSIZE;

	byte *respBuffer = new byte[responseSize];
	SetResponseBuffer(respBuffer,responseSize);
	memset(GetResponseBuffer(),0,responseSize);


	return;
}

__int64 CLongPoll2FMessage::ConvertToCents(byte meterCode, __int64 meterValue)
{
	__int64 returnValue; 

	// Check meter code used to get meter value to see if it came in as credits and needs to be converted to cents
	if (
		//meterCode == 0x24 ||
		meterCode == 0x28 ||
		meterCode == 0x29 ||
		meterCode == 0x2A ||
		meterCode == 0x2B ||
		meterCode == 0x2C ||
		meterCode == 0x2D ||
		false
		)
		returnValue = meterValue * m_denomMult;
	else
		returnValue = meterValue;

	return returnValue;
}

