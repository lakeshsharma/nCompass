#include "stdafx.h"
#include "LongPollB5Message.h"
#include "Utilities.h"
#include "Logging/LogString.h"

CLongPollB5Message::CLongPollB5Message(byte gameAddress)
: CSASPollMessage(gameAddress)
{
	// allocate buffers
	SetSendBuffer((BYTE *)new byte[XB5_POLLSIZE], XB5_POLLSIZE);
	SetResponseBuffer((BYTE *)new byte[XB5_MAX_RESPONSE_SIZE], XB5_MAX_RESPONSE_SIZE);
	memset(GetSendBuffer(), 0, GetSendBufferSize());
	memset(GetResponseBuffer(), 0, GetResponseBufferSize());

	m_isVariableResponse = true;
	m_maxBet = 0;
	m_progressiveGroup = XB5_DEFAULT_PROGRESSIVE_GROUP;
	m_gameNumber = 0;
	m_gameNameLength = 0;
	m_gameName = nullptr;
	m_paytableNameLength = 0;
	m_paytableName = nullptr;
	m_levelIds = 0;
}


CLongPollB5Message::~CLongPollB5Message()
{
	CSASPollMessage::WaitForSASProcessingDone();
	DeleteArray(m_gameName);
	DeleteArray(m_paytableName);
}

void CLongPollB5Message::SetGameNumber(int gameNumber)
{
	m_gameNumber = gameNumber;
	return;
}

BYTE CLongPollB5Message::GetPollValue()
{
	return POLL_VALUE;
}

bool CLongPollB5Message::SwapPollValue()
{
	// what do we do here?
	return false;
}

void CLongPollB5Message::ResponseReceived()
{
	if (m_nReturnValue == SASPOLL_SUCCESS)
	{
		const BYTE *response = GetResponseBuffer();
		const BYTE *responseEnd = response + GetResponseBufferSize() - CRCSIZE;

		// Check if the game address and poll value in the response is correct.
		if (response[GAME_ADDRESS_LOCATION] == m_gameAddress &&
			response[POLL_VALUE_LOCATION] == POLL_VALUE)
		{
			if (m_responseBufferSize >= XB5_PROGRESSIVELEVELSIDLOCATION + sizeof(DWORD) + CRCSIZE)
			{
				memcpy(&m_levelIds, response + XB5_PROGRESSIVELEVELSIDLOCATION, sizeof(DWORD));
			}

			/* Check if the response is long enough to contain the desired field.
			   Subsequent fields are accessed on the basis of available buffer. 
			   otherwise proper return status will be set*/
			if (m_responseBufferSize >= XB5_RESPONSE_GAMENAME_LENGTH + XB5_RESPONSE_GAMENAME_LENGTH_SIZE + CRCSIZE)
			{
				// Check if the game number is what we wanted.
				DWORD responseGameNumber = Bcd2Dword(response + XB5_RESPONSE_GAMENUMBERLOCATION, XB5_GAMENUMBERSIZE);
				if ((int)responseGameNumber == m_gameNumber)
				{
					m_maxBet = Bcd2Int64(response + XB5_MAXBETLOCATION, XB5_MAXBETSIZE);
					m_progressiveGroup = response[XB5_PROGRESSIVEGROUPLOCATION];
				
					const BYTE* responseBuffer = &response[XB5_RESPONSE_GAMENAME_LENGTH];

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
	CSASPollMessage::ResponseReceived();

	return;

}

void CLongPollB5Message::Fill()
{
	// fill long poll command data
	m_sendBuffer[GAME_ADDRESS_LOCATION] = m_gameAddress;
	m_sendBuffer[POLL_VALUE_LOCATION] = POLL_VALUE;
	Dword2Bcd(m_gameNumber, &m_sendBuffer[XB5_GAMENUMBERLOCATION], XB5_GAMENUMBERSIZE);

	ushort commandCRC = CRC(m_sendBuffer, XB5_POLLSIZE - CRCSIZE, 0);
	byte *crc = (byte *)&m_sendBuffer[XB5_POLLSIZE - CRCSIZE];
	memcpy(crc, &commandCRC, CRCSIZE);

	m_variableResponseLenghLocation = XB5_LENGTHBYTE; // length does NOT include CRC bytes.
	m_variableResponseLengthSize = XB5_LENGTHSIZE;
	m_variableResponseSizeConstant = XB5_RESPONSE_EXTRADATASIZE; // game address, game command, length byte, ..... , 2 byte CRC

	return;
}

__int64 CLongPollB5Message::GetMaxBet() const
{
	return m_maxBet;
}

byte CLongPollB5Message::GetProgressiveGroup() const
{
	return m_progressiveGroup;
}

bool CLongPollB5Message::FillGameName(const BYTE **responseBuffer, const BYTE *responseEnd)
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

		LogString (ERROR_LOG, "%s:%d: Minimum length failed for a valid B5 response", __FUNCTION__, __LINE__);
	}
	else
	{
		LogString ("SAS", "%s:%d: Game: (Length: %u) %s", __FUNCTION__, __LINE__, 
		m_gameNameLength, m_gameName);
	}

	return fillStatus;
}

bool CLongPollB5Message::FillPaytableName(const BYTE **responseBuffer, const BYTE *responseEnd)
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
	
		LogString (ERROR_LOG, "%s:%d: Minimum length failed for a valid B5 response", __FUNCTION__, __LINE__);
	}
	else
	{
		LogString ("SAS", "%s:%d: Paytable: (Length: %u) %s", __FUNCTION__, __LINE__, 
		m_paytableNameLength, m_paytableName);
	}

	return fillStatus;
}

WORD CLongPollB5Message::GetGameName (char** gameName)
{
	WORD gameLength = 0;
	if ((m_gameName != nullptr) && (m_gameNameLength > 0))
	{
		//Expected unallocated buffer
		if(*gameName != nullptr)
		{
			delete []*gameName;
			*gameName = nullptr;
		}
		*gameName = new char [m_gameNameLength + 1];
		memset (*gameName, 0, m_gameNameLength + 1);
		memcpy_s(*gameName, m_gameNameLength, m_gameName, m_gameNameLength);
		gameLength = m_gameNameLength;
	}

	return gameLength;
}

WORD CLongPollB5Message::GetPaytableName (char** paytableName)
{
	WORD paytableNameLength = 0;
	if ((m_paytableName != nullptr) && (m_paytableNameLength > 0))
	{
		//Expected unallocated buffer
		if(*paytableName != nullptr)
		{
			delete []*paytableName;
			*paytableName = nullptr;
		}
		*paytableName = new char [m_paytableNameLength + 1];
		memset (*paytableName, 0, m_paytableNameLength + 1);
		memcpy_s(*paytableName, m_paytableNameLength, m_paytableName, m_paytableNameLength);
		paytableNameLength = m_paytableNameLength;
	}

	return paytableNameLength;
}

DWORD CLongPollB5Message::GetLevelIds() const
{
	return m_levelIds;
}