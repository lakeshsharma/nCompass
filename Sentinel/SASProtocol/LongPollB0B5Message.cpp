#include "stdafx.h"
#include "LongPollB0B5Message.h"

CLongPollB0B5Message::CLongPollB0B5Message(byte gameAddress, int gameNumber)
	: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((byte*)new byte[9], 9);
	SetResponseBuffer((BYTE*)new byte[XB0B5_MAX_RESPONSE_SIZE], XB0B5_MAX_RESPONSE_SIZE);
	memset(GetSendBuffer(), 0, GetSendBufferSize());
	memset(GetResponseBuffer(), 0, GetResponseBufferSize());

	m_gameNumber = gameNumber;
	m_isVariableResponse = true;
	m_maxBet = 0;
	m_progressiveGroup = XB0B5_DEFAULT_PROGRESSIVE_GROUP;
	m_gameNameLength = 0;
	m_gameName = nullptr;
	m_paytableNameLength = 0;
	m_paytableName = nullptr;
	m_levelIds = 0;
	m_denom = 0;
}

CLongPollB0B5Message::~CLongPollB0B5Message(void)
{
	CSASPollMessage::WaitForSASProcessingDone();
}

BYTE CLongPollB0B5Message::GetPollValue()
{
	return 0xB0;
}

bool CLongPollB0B5Message::SwapPollValue()
{
	// what do we do here?
	return 0;
}

void CLongPollB0B5Message::ResponseReceived()
{
	try
	{
		const BYTE* response = GetResponseBuffer();
		if (m_nReturnValue == SASPOLL_SUCCESS && response[4] == 0xB5)
		{
			const BYTE* responseEnd = response + GetResponseBufferSize() - CRCSIZE;

			// Check if the game address and poll value in the response is correct.
			if (response[GAME_ADDRESS_LOCATION] == m_gameAddress &&
				response[POLL_VALUE_LOCATION] == POLL_VALUE)
			{
				/* Check if the response is long enough to contain the desired field.
				   Subsequent fields are accessed on the basis of available buffer.
				   otherwise proper return status will be set*/

				DWORD responseGameNumber = Bcd2Dword(response + XB0B5_RESPONSE_GAMENUMBERLOCATION, XB0B5_GAMENUMBERSIZE);

				if (m_responseBufferSize >= XB0B5_PROGRESSIVELEVELSIDLOCATION + sizeof(DWORD) + CRCSIZE
					&& (int)responseGameNumber == m_gameNumber)
				{
					// Do this even if response is eventually marked SASPOLL_INCOMPLETE_DATA
					memcpy(&m_levelIds, response + XB0B5_PROGRESSIVELEVELSIDLOCATION, sizeof(DWORD));
				}

				if (m_responseBufferSize >= XB0B5_RESPONSE_GAMENAME_LENGTH + XB0B5_RESPONSE_GAMENAME_LENGTH_SIZE + CRCSIZE)
				{
					// Check if the game number is what we wanted.
					if ((int)responseGameNumber == m_gameNumber)
					{
						m_maxBet = Bcd2Int64(response + XB0B5_MAXBETLOCATION, XB0B5_MAXBETSIZE);
						m_progressiveGroup = response[XB0B5_PROGRESSIVEGROUPLOCATION];

						const BYTE* responseBuffer = &response[XB0B5_RESPONSE_GAMENAME_LENGTH];

						FillGameName(&responseBuffer, responseEnd);
						FillPaytableName(&responseBuffer, responseEnd);
					}
					else
					{
						// The response is not useful to the originator.  Set an error code.
						m_nReturnValue = SASPOLL_INCOMPLETE_DATA;
					}

				}
				else
				{
					// The minimum length for a valid B5 response (14 bytes total) includes the max bet field.
					m_nReturnValue = SASPOLL_INCOMPLETE_DATA;
				}
			}
			else
			{
				m_nReturnValue = SASPOLL_NACK;
			}
		}
	}
	catch (...)
	{
	}

	CSASPollMessage::ResponseReceived();

	return;

}


void CLongPollB0B5Message::Fill()
{
	try
	{
		// fill long poll command data
		m_sendBuffer[0] = m_gameAddress;
		m_sendBuffer[1] = 0xB0; // b0 command
		m_sendBuffer[2] = 4;	// number bytes remaining not including crc
		m_sendBuffer[3] = m_denom; // denomination
		m_sendBuffer[4] = 0xB5; // base command
		Dword2Bcd(m_gameNumber, &m_sendBuffer[XB0B5_GAMENUMBERLOCATION], XB0B5_GAMENUMBERSIZE);

		m_isVariableResponse = true;
		m_variableResponseLenghLocation = XB0B5_LENGTHBYTE; // length does NOT include CRC bytes.
		m_variableResponseLengthSize = XB0B5_LENGTHSIZE;
		m_variableResponseSizeConstant = XB0B5_RESPONSE_EXTRADATASIZE; // game address, game command, length byte, ..... , 2 byte CRC

		byte* crcLocation;
		ushort messageCRC = CRC(GetSendBuffer(), GetSendBufferSize() - CRCSIZE, 0);
		crcLocation = (byte*)&m_sendBuffer[7];
		memcpy(crcLocation, &messageCRC, CRCSIZE);
	}
	catch (...)
	{
	}
	return;
}

void CLongPollB0B5Message::SetDenom(byte denom)
{
	m_denom = denom;
	return;
}

bool CLongPollB0B5Message::FillGameName(const BYTE** responseBuffer, const BYTE* responseEnd)
{
	bool fillStatus(true);
	m_gameNameLength = 0;

	CUtilities::MemCopySafe_IncSourcePtr(fillStatus, &m_gameNameLength, sizeof(m_gameNameLength), *responseBuffer, responseEnd);

	if (fillStatus)
	{
		m_gameName = new char[m_gameNameLength];
		CUtilities::MemCopySafe_IncSourcePtr(fillStatus, m_gameName, m_gameNameLength, *responseBuffer, responseEnd);
	}

	if (false == fillStatus)
	{
		DeleteArray(m_gameName);
		m_gameNameLength = 0;
		m_nReturnValue = SASPOLL_INCOMPLETE_DATA;

		LogString(ERROR_LOG, "%s:%d: Minimum length failed for a valid B5 response", __FUNCTION__, __LINE__);
	}
	else
	{
		LogString("SAS", "%s:%d: Game: (Length: %u) %s", __FUNCTION__, __LINE__,
			m_gameNameLength, m_gameName);
	}

	return fillStatus;
}

bool CLongPollB0B5Message::FillPaytableName(const BYTE** responseBuffer, const BYTE* responseEnd)
{
	bool fillStatus(true);
	m_paytableNameLength = 0;
	CUtilities::MemCopySafe_IncSourcePtr(fillStatus, &m_paytableNameLength, sizeof(m_paytableNameLength), *responseBuffer, responseEnd);

	if (fillStatus)
	{
		m_paytableName = new char[m_paytableNameLength];
		CUtilities::MemCopySafe_IncSourcePtr(fillStatus, m_paytableName, m_paytableNameLength, *responseBuffer, responseEnd);
	}

	if (false == fillStatus)
	{
		DeleteArray(m_paytableName);
		m_paytableNameLength = 0;
		m_nReturnValue = SASPOLL_INCOMPLETE_DATA;

		LogString(ERROR_LOG, "%s:%d: Minimum length failed for a valid B5 response", __FUNCTION__, __LINE__);
	}
	else
	{
		LogString("SAS", "%s:%d: Paytable: (Length: %u) %s", __FUNCTION__, __LINE__,
			m_paytableNameLength, m_paytableName);
	}

	return fillStatus;
}

__int64 CLongPollB0B5Message::GetMaxBet() const
{
	return m_maxBet;
}

byte CLongPollB0B5Message::GetProgressiveGroup() const
{
	return m_progressiveGroup;
}

WORD CLongPollB0B5Message::GetGameName(char** gameName)
{
	WORD gameLength = 0;
	if ((m_gameName != nullptr) && (m_gameNameLength > 0))
	{
		//Expected unallocated buffer
		if (*gameName != nullptr)
		{
			delete[] * gameName;
			*gameName = nullptr;
		}
		*gameName = new char[m_gameNameLength + 1];
		memset(*gameName, 0, m_gameNameLength + 1);
		memcpy_s(*gameName, m_gameNameLength, m_gameName, m_gameNameLength);
		gameLength = m_gameNameLength;
	}

	return gameLength;
}

WORD CLongPollB0B5Message::GetPaytableName(char** paytableName)
{
	WORD paytableNameLength = 0;
	if ((m_paytableName != nullptr) && (m_paytableNameLength > 0))
	{
		//Expected unallocated buffer
		if (*paytableName != nullptr)
		{
			delete[] * paytableName;
			*paytableName = nullptr;
		}
		*paytableName = new char[m_paytableNameLength + 1];
		memset(*paytableName, 0, m_paytableNameLength + 1);
		memcpy_s(*paytableName, m_paytableNameLength, m_paytableName, m_paytableNameLength);
		paytableNameLength = m_paytableNameLength;
	}

	return paytableNameLength;
}

DWORD CLongPollB0B5Message::GetLevelIds() const
{
	return m_levelIds;
}
