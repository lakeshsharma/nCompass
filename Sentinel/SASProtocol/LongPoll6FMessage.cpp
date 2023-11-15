#include "stdafx.h"

#include "LongPoll6FMessage.h"
#include "Utilities.h"
#include "../Sentinel.h"
#include "Logging/LogString.h"

CLongPoll6FMessage::CLongPoll6FMessage(byte gameAddress, CMeters *meters)
	: CMetersLongPoll(gameAddress)
{
	m_pollValue = 0x6f;
	m_meters = meters;
	m_denomMult = 0;
	m_gameNumber = EGM_GAME_NUMBER;
	m_isIgtS2000 = false;
}

CLongPoll6FMessage::~CLongPoll6FMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}


BYTE CLongPoll6FMessage::GetPollValue()
{
	return m_pollValue;
}

bool CLongPoll6FMessage::SwapPollValue()
{
	if ( m_pollValue == 0x6f )
		m_pollValue = 0xaf;
	else
		m_pollValue = 0x6f;

	// Need to update m_sendBuffer with new long poll value and CRC
	m_sendBuffer[1] = m_pollValue;
	ushort commandCRC = CRC(GetSendBuffer(), GetSendBufferSize() - CRCSIZE,0);
	byte *crcLocation = (byte *) (GetSendBuffer() + GetSendBufferSize() - CRCSIZE);
	memcpy(crcLocation,&commandCRC,CRCSIZE);

	return true;
}

void CLongPoll6FMessage::SetDenomMultiplier(WORD denom)
{
	m_denomMult = denom;
}

void CLongPoll6FMessage::Fill(bool isIgtS2000)
{
	m_isIgtS2000 = isIgtS2000;
	Fill();
}

void CLongPoll6FMessage::Fill()
{
	// When IGT S2000, do not use the primary meterCode for retreiving meters.  Only use secondary code.
	//((CSentinel3App *)AfxGetApp())->LogString(L"SASGENERAL", L"Begin 6f poll");
	try
	{
		// Need to enforce 12 meter limit for 6f poll
		byte meterCount = m_meters->GetMeterCount();
		if (meterCount > 12)
			meterCount = 12;
		
		byte primaryMeterCode = INVALID_SAS_METER_CODE;
		byte secondaryMeterCode = INVALID_SAS_METER_CODE;
		WORD wMeterCode;

		// allocate enough for max meters, need to adjust before exiting fill
		int bufferSize = X6F_MAX_METERS*2 + X6F_EXTRADATASIZE;

		byte *sendData = (BYTE *)new byte[ bufferSize ];
		SetSendBuffer(sendData,bufferSize);


		// fill long poll command data
		m_sendBuffer[0] = m_gameAddress;
		m_sendBuffer[1] = m_pollValue;
		// fill in gamenumber bcd
		Dword2Bcd(m_gameNumber,&m_sendBuffer[X6F_GAMENUMBERLOCATION],CRCSIZE);

		// fill in meter codes
		int bufferLoc, i;
		for (i = 0, bufferLoc = 0; i < meterCount; i++)
		{
			const CMeter *meter = m_meters->GetMeterByIndex( i );
			GetMeterCodeForMeterNumber( meter->GetMeterNumber(), m_gameNumber, primaryMeterCode, secondaryMeterCode );
			if (primaryMeterCode != INVALID_SAS_METER_CODE && (!m_isIgtS2000 || ( primaryMeterCode != 0x2C && primaryMeterCode != 0x2D && primaryMeterCode != 0x28 && primaryMeterCode != 0x2A && primaryMeterCode != 0x2B && primaryMeterCode != 0x29)))
			{
				// get this one as well
				wMeterCode = primaryMeterCode;
				memcpy(&m_sendBuffer[bufferLoc+X6F_METERLOCATION], &wMeterCode, 2 );
				bufferLoc += 2;
			}
			if (secondaryMeterCode != INVALID_SAS_METER_CODE)
			{
				// get this one as well
				wMeterCode = secondaryMeterCode;
				memcpy(&m_sendBuffer[bufferLoc+X6F_METERLOCATION], &wMeterCode, 2 );
				bufferLoc += 2;
			}
		}
		
		meterCount = bufferLoc/2;
		m_sendBuffer[2] = meterCount*2 + 2; // byte count not including CRC
		SetSendBuffer(sendData,X6F_EXTRADATASIZE + meterCount*2);  // reset buffer size to actual size used
		ushort commandCRC = CRC(GetSendBuffer(), GetSendBufferSize() - CRCSIZE,0);
		byte *crcLocation = (byte *) (GetSendBuffer() + GetSendBufferSize() - CRCSIZE);
		memcpy(crcLocation,&commandCRC,CRCSIZE);

		// Configure response buffer
		m_isVariableResponse = true;
		m_variableResponseLenghLocation = X6F_LENGTHBYTE; // length does NOT include CRC bytes.
		m_variableResponseLengthSize = X6F_LENGTHSIZE;
		m_variableResponseSizeConstant = X6F_RESPONSE_EXTRADATASIZE; // game address, game command, length byte, ..... , 2 byte CRC

		meterCount = bufferLoc/2;
		int responseSize = meterCount*X6F_DEFMETERSIZE + X6F_RESPONSE_EXTRADATASIZE + CRCSIZE;
		SetResponseBuffer((BYTE *)new byte[responseSize],responseSize);
		memset(GetResponseBuffer(),0,responseSize);

	}
	catch(...)
	{
	}
	return;
}

void CLongPoll6FMessage::SetGameNumber(int gameNumber)
{
	m_gameNumber = gameNumber;
	return;
}

void CLongPoll6FMessage::ResponseReceived()
{
	const byte *response = GetResponseBuffer();
	const byte *responseEnd = response + GetResponseBufferSize() - CRCSIZE;
	//CString logStr;

	//((CSentinelApp *)AfxGetApp())->LogString(L"SASGENERAL", L"6F response: Received %d bytes, return result %d",GetResponseBufferSize(), m_nReturnValue);
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
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
				ok = pollValue == m_pollValue;
			}
		}

		if (ok)
		{
			// Is the length field value correct?
			byte length; //  = *(response);
			CUtilities::MemCopySafe_IncSourcePtr(ok, &length, sizeof(length), response, responseEnd);
			if (ok)
			{
				ok = response + length == responseEnd;
			}
		}

		if (ok)
		{
			// Is game number correct?
			ok = response + X6F_GAMENUMBERSIZE <= responseEnd;
			if (ok)
			{
				DWORD gameNum = Bcd2Dword(response, X6F_GAMENUMBERSIZE);
				//((CSentinelApp *)AfxGetApp())->LogString(L"SASGENERAL", L"6f response gamenum %d == m_gameNumber %d",gameNum, m_gameNumber);
				response += X6F_GAMENUMBERSIZE;

				ok = (int)gameNum == m_gameNumber;
			}
		}

		if ( ok )
		{
			//logStr.Format(L"6F response: beginning meter loop response %X, responseEnd %X",response,responseEnd);
			//((CSentinelApp *)AfxGetApp())->LogString(L"SASGENERAL",logStr);
			while ( response < responseEnd && ok )
			{
				MeterReceived(ok, response, responseEnd);
			}
		}

		if (!ok)
		{
			m_nReturnValue = SASPOLL_NACK; // Not a valid response.
			LogString(ERROR_LOG, "6F m_gameAddress=%02x m_pollValue=%02x m_gameNumber=%04u format/parsing error: %s",
				(unsigned)m_gameAddress,
				(unsigned)m_pollValue,
				(unsigned)m_gameNumber,
				CUtilities::ToHexString(GetResponseBuffer(), (DWORD)GetResponseBufferSize()).c_str());

			m_meters->SetAllMetersInvalid(); // Mark all meters as invalid, because the entire response was NOT formatted correctly.
		}
	}

	CSASPollMessage::ResponseReceived();
	return;
}

void CLongPoll6FMessage::MeterReceived(bool &ok, const BYTE *&response, const BYTE *responseEnd)
{
	WORD meterCode; // = *(response);
	CUtilities::MemCopySafe_IncSourcePtr(ok, &meterCode, sizeof(meterCode), response, responseEnd);

	byte meterValueSize; // = *(response); // x BCD digits
	CUtilities::MemCopySafe_IncSourcePtr(ok, &meterValueSize, sizeof(meterValueSize), response, responseEnd);
	if (ok)
	{
		ok = response + meterValueSize <= responseEnd;
		if (ok)
		{
			// sometimes, you see a meter code followed by meterValueSize = 0, ignores these.
			if (meterValueSize > 0)
			{
				__int64 meterValue = Bcd2Int64(response,meterValueSize);
				response += meterValueSize;

				// Find the meter number that is associated with this code.
				int meterNumber = GetMeterNumberForMeterCode( (byte)meterCode, m_gameNumber );

				// Find the meter in our meter list. We don't want to add any new meters at this point.
				const CMeter *meter = m_meters->GetMeterByNumber( meterNumber );
				if ( meter != NULL && meter->GetValue() == INVALID_METER_VALUE)
					const_cast<CMeter *>(meter)->SetValue(ConvertToCents( (byte)meterCode, meterValue) );

				//logStr.Format(L"6F response: looping response %X, responseEnd %X",response,responseEnd);
				//((CSentinelApp *)AfxGetApp())->LogString(L"SASGENERAL", logStr);

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
}

__int64 CLongPoll6FMessage::ConvertToCents(byte meterCode, __int64 meterValue)
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

