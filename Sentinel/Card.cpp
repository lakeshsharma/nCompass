#include "stdafx.h"
#include "Card.h"

#include "Utilities.h"
#include "GlobalDefs.h"
#include "Logging/LogString.h"

using namespace std;

const byte TRACK_1_CARD_START_VALUE = (byte)'%';
const byte TRACK_2_CARD_START_VALUE = (byte)';';
const int CARD_SITE_LEN = 3;
const int CARD_TYPE_LEN = 3;
const int CARD_X_LEN = 2;
const int CARD_Y_LEN = 2;
const int CARD_NUM_LEN = 8;
const int UNIVERSE_CARD_NUM_LENGTH = 17;
const int UNIVERSE_CARD_PROP_LEN = 4;
const int UNIVERSE_ACCOUNT_NUM_LEN = 9;
const int UNIVERSE_CHECK_DIGIT_LEN = 1;
const byte CARD_END_VALUE = (byte)'?';

const byte NCL_CARD_OCR_LENGTH = 10;
const byte NCL_CARD_END_VALUE = CARD_END_VALUE;

const int RCCL_CARD_FIRST_READ_POS = 11;
const int RCCL_CARD_FIRST_READ_POS_TYPE2 = 9;
const int RCCL_CARD_CHARS_PER_DIGIT = 2;
const int RCCL_CARD_NUM_OF_DIGITS = 4;
const byte RCCL_CARD_END_VALUE = 0x3d;
const int RCCL_CARD_LENGTH_TYPE2 = RCCL_CARD_FIRST_READ_POS_TYPE2 +
	(RCCL_CARD_CHARS_PER_DIGIT * RCCL_CARD_NUM_OF_DIGITS) +
	sizeof(RCCL_CARD_END_VALUE) +
	sizeof(CARD_END_VALUE);

const int CARD_ID_XY_POS = 1;
const int CARD_ID_NUM_POS = 2;

const int GLOBAL_GLOBAL_CARD_LEN = 5;
const byte GLOBAL_GLOBAL_CARD_NUM[GLOBAL_GLOBAL_CARD_LEN] = { 0x69, 0x09, 0xff, 0x00, 0x01 };
const int RFID_DATA_SIZE = 176;
const int RFID_PLAYER_LOC = 64;
const int RFID_PLAYER_SIZE = 8;
const int RFID_EMPLOYEE_LOC = 76;
const byte NULLS[RFID_PLAYER_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0 };

const int SDS_CARD_DATA_LENGTH = 16;
const int SDS_CARD_PROPERTY_CODE_LENGTH = 3;
const int SDS_CARD_TYPE_LENGTH = 1;
const int SDS_CARD_ACC_NUM_LENGTH = 9;

// CCard::CCard()
//     IN cardInTime - the card in time for the constructed instance.
//     IN utcCardInTime - the UTC card in time for the constructed instance.
//     RETURN - reference to the constructed instance.
// Construct a CCard instance using time_t parameters.
CCard::CCard(time_t const cardInTime, time_t const utcCardInTime )
{
	Init(); // Always call first in constructor.

	m_cardInTime = cardInTime;
	m_utcCardInTime = utcCardInTime;
	m_cardOutTime = cardInTime;
}

// CCard::CCard()
//     IN cardRHS - CCard instance to be copied.
//     RETURN - reference to a copy of cardRHS.
// Copy constructor.
CCard::CCard( const CCard &cardRHS )
{
	//initialize before assigning
	Init(); // Always call first in constructor.

	*this = cardRHS;
}

// CCard::SetCardID()
//     IN cardIDStr - card ID string used to update m_cardID of the instance.
//         MUST be at least 10 (CARD_ID_LEN * 2) characters long.
//         The first 10 (CARD_ID_LEN * 2) characters MUST be hexadecimal characters.
//         Only the first 10 (CARD_ID_LEN * 2) characters are used to update the instance.
//         Where "* 2" is for the number of hexadecimal digits (nibbles) stored per byte.
// This is used to automate card in/out for testing (in MCRAutoTest.cpp)
void CCard::SetCardID( std::string cardIDStr, const CCardConfig *config )
{
	if (m_cardID != NULL)
	{
		delete [] m_cardID;
		m_cardID = NULL;
	}

	m_cardIDLen = CARD_ID_LEN;
	m_cardID = new byte[m_cardIDLen];
	CUtilities::MakeCardIDFromCString(cardIDStr.c_str(), m_cardID, m_cardIDLen);
	m_cardTypeNumber = m_cardID[CARD_ID_TYPE_POS];
	m_siteNumber = DEFAULT_SITE_NUMBER;
	m_validRead = true;

	if (config != NULL)
	{
		m_siteNumber = config->GetCommID();
		m_isUniversalCard = config->IsUniversePlayerCardPrefix(m_cardTypeNumber);
	}
}

// CCard::CCard()
//     IN nvramBuffer - memory buffer used to specify member values.
//     RETURN - reference to the constructed instance.
// Construct a CCard instance using a memory buffer to specify member values.
CCard::CCard( const byte *nvramBuffer )
{
	Init(); // Always call first in constructor.

	memcpy( &m_cardType, nvramBuffer, sizeof( m_cardType ) );
	nvramBuffer += sizeof( m_cardType );

	memcpy( &m_cardTypeNumber, nvramBuffer, sizeof( m_cardTypeNumber ) );
	nvramBuffer += sizeof( m_cardTypeNumber );

	memcpy( &m_siteNumber, nvramBuffer, sizeof( m_siteNumber ) );
	nvramBuffer += sizeof( m_siteNumber );

	memcpy( &m_cardIDLen, nvramBuffer, sizeof( m_cardIDLen ) );
	nvramBuffer += sizeof( m_cardIDLen );

	if (0 < m_cardIDLen && m_cardIDLen <= (int)MAX_CARD_NUMBER_LENGTH)
	{
		m_cardID = new byte [m_cardIDLen];
		memcpy( m_cardID, nvramBuffer, m_cardIDLen );
	}
	else
	{
		m_cardIDLen = 0;
		m_cardID = NULL;
	}
	nvramBuffer += MAX_CARD_NUMBER_LENGTH;

	m_cardInTime = CUtilities::MakeTimeFromNvramBuffer( nvramBuffer, NVRAM_TIME_SIZE );
	nvramBuffer += NVRAM_TIME_SIZE;

	m_cardOutTime = CUtilities::MakeTimeFromNvramBuffer( nvramBuffer, NVRAM_TIME_SIZE );
	nvramBuffer += NVRAM_TIME_SIZE;

	memcpy( &m_validRead, nvramBuffer, sizeof( m_validRead ) );
	nvramBuffer += sizeof( m_validRead );

	m_utcCardInTime = CUtilities::MakeTimeFromNvramBuffer( nvramBuffer, NVRAM_TIME_SIZE );
	nvramBuffer += NVRAM_TIME_SIZE;

	//If this is the first time that we've run, then the utc time should be way off.
	//Set it to an appropriate value.
	time_t timeDiff = m_cardInTime - m_utcCardInTime;
	if (timeDiff / SECONDS_PER_HOUR < -12 || timeDiff / SECONDS_PER_HOUR >12)
	{
		m_utcCardInTime = m_cardInTime - CUtilities::GetTzOffset();
	}

	memcpy( &m_isUniversalCard, nvramBuffer, sizeof( m_isUniversalCard ) );
	nvramBuffer += sizeof( m_isUniversalCard );
}

// CCard::Init()
// Set all member variables to initial values during initialization.
// Do not use this method except for initialization, as it does not free resources.
// If this method allocates resources then ensure that the constructor(s) do not leak them. 
void CCard::Init()
{
	// Set all pointers to NULL then call Clear().
	m_cardID = NULL;
	Clear();
}

// CCard::Clear()
// Specifically set m_validRead=false.
// Set all other member variables to initial/default values.
// This method frees all resources, so pointers MUST already be valid.
// If this method allocates resources then ensure that the constructor(s) do not leak them. 
void CCard::Clear()
{
	if ( m_cardID != NULL )
	{
		delete [] m_cardID;
		m_cardID = NULL;
	}

	// Initialize in same order as declared in the class declaration.
	m_cardType = CARD_UNKNOWN;
	m_cardTypeNumber = CCardConfig::INVALID_CARD_PREFIX;
	m_siteNumber = 0;
	m_cardIDLen = 0;
	m_cardID = NULL;
	m_cardInTime = 0;
	m_utcCardInTime = 0;
	m_cardOutTime = 0;
	m_validRead = false;
	m_rawDataLength = 0;
	m_isUniversalCard = false;
    m_universalPropCode = 0;
}

CCard::~CCard(void)
{
	if ( m_cardID != NULL )
	{
		delete [] m_cardID;
	}
}

CCard & CCard::operator=( const CCard &cardRHS )
{
	if (this != &cardRHS)
	{
		m_cardType = cardRHS.GetCardType();
		m_cardInTime = cardRHS.GetCardInTime();
		m_utcCardInTime = cardRHS.GetUTCCardInTime();
		m_isUniversalCard = cardRHS.GetIsUniversalCard();
		m_cardOutTime = cardRHS.GetCardOutTime();
		m_validRead = cardRHS.m_validRead;
		m_rawDataLength = cardRHS.m_rawDataLength;

		byte *pCardIdOld = m_cardID; 
		const byte *cardID = cardRHS.GetCardID( m_cardIDLen );

		if ( m_cardIDLen > 0 && cardID != NULL )
		{
			m_cardID = new byte [m_cardIDLen];
			memcpy( m_cardID, cardID, m_cardIDLen );
		}
		else
		{
			m_cardID = NULL;
			m_cardIDLen = 0;
		}

		if (pCardIdOld)
		{
			delete [] pCardIdOld;
		}
	}

	return *this;
}

int CCard::GetNvramBuffer( byte *nvramBuffer, int nvramBufferLength ) const
{
	byte *bufferPos = nvramBuffer;

	if ( nvramBufferLength >= (int)NVRAM_CARD_SIZE )
	{
		memcpy( bufferPos, &m_cardType, sizeof( m_cardType ) );
		bufferPos += sizeof( m_cardType );

		memcpy( bufferPos, &m_cardTypeNumber, sizeof( m_cardTypeNumber ) );
		bufferPos += sizeof( m_cardTypeNumber );

		memcpy( bufferPos, &m_siteNumber, sizeof( m_siteNumber ) );
		bufferPos += sizeof( m_siteNumber );

		memcpy( bufferPos, &m_cardIDLen, sizeof( m_cardIDLen ) );
		bufferPos += sizeof( m_cardIDLen );

		memcpy( bufferPos, m_cardID, m_cardIDLen );
		bufferPos += MAX_CARD_NUMBER_LENGTH;

		CUtilities::GetNvramBufferFromTime( m_cardInTime, bufferPos, NVRAM_TIME_SIZE );
		bufferPos += NVRAM_TIME_SIZE;

		CUtilities::GetNvramBufferFromTime( m_cardOutTime, bufferPos, NVRAM_TIME_SIZE );
		bufferPos += NVRAM_TIME_SIZE;

		memcpy( bufferPos, &m_validRead, sizeof( m_validRead ) );
		bufferPos += sizeof( m_validRead );

		CUtilities::GetNvramBufferFromTime( m_utcCardInTime, bufferPos, NVRAM_TIME_SIZE );
		bufferPos += NVRAM_TIME_SIZE;

		memcpy( bufferPos, &m_isUniversalCard, sizeof( m_isUniversalCard ) );
		bufferPos += sizeof( m_isUniversalCard );
	}

	return (bufferPos - nvramBuffer);
}

void CCard::SetAsInvalidRead( void )
{
	Clear();
}

bool CCard::IsValidRead( void ) const
{
	return m_validRead;
}

bool CCard::IsGlobalGlobal() const
{
	bool isglobalglobal = Compare(GLOBAL_GLOBAL_CARD_NUM, GLOBAL_GLOBAL_CARD_LEN);
	return isglobalglobal;
}

byte CCard::GetCardTypeNumber( void ) const
{
	return m_cardTypeNumber;
}

bool CCard::IsEmployee() const
{
	bool isEmployee(false);

	if (m_validRead)
	{
		switch (m_cardType)
		{
		case CARD_MECHANIC:
		case CARD_GLOBAL:
		case CARD_FLOOR:
		case CARD_CURRENCY:
		case CARD_METER:
			isEmployee = true;
			break;
		default:
			break;
		}
	}

	return isEmployee;
}

CardType CCard::GetCardType( void ) const
{
	return m_cardType;
}

void CCard::SetCardType( CardType type )
{
	m_cardType = type;
}

const byte* CCard::GetCardID(int &cardIDLen) const
{
	cardIDLen = m_cardIDLen;
	return m_cardID;
}

void CCard::SetCardInTime( time_t const cardInTime, time_t const utcCardInTime )
{
	m_cardInTime = cardInTime;
	m_utcCardInTime = utcCardInTime;
	m_cardOutTime = cardInTime;
}

void CCard::SetCardOutTime( time_t const cardOutTime )
{
	m_cardOutTime = cardOutTime;
}

time_t CCard::GetCardInTime( void ) const
{
	return m_cardInTime;
}

time_t CCard::GetUTCCardInTime( void ) const
{
	return m_utcCardInTime;
}

time_t CCard::GetCardOutTime( void ) const
{
	return m_cardOutTime;
}

bool CCard::GetIsUniversalCard() const
{
	return m_isUniversalCard;
}

void CCard::SetCardInfoFromRawData(const byte *rawData, int rawDataLen, const CCardConfig &config)
{
	m_siteNumber = config.GetCommID();

	if (config.NclCardsEnabled())
	{
		// Try to set the number in the card object from the mcr raw data using the NCL card format.
		SetCardInfoFromRawNclData(rawData, rawDataLen, m_siteNumber);
	}

	if (!IsValidRead() && config.RcclCardsEnabled())
	{
		// Try to set the number in the card object from the mcr raw data using the RCCL card format.
		SetCardInfoFromRawRcclData(rawData, rawDataLen, config.GetRcclCardPlayerPrefix(), m_siteNumber, false);
	}

	if (!IsValidRead() && config.GetUniverseEnabled())
	{
		if (config.GetUseSDSCardFormat() && rawDataLen == SDS_CARD_DATA_LENGTH)
		{
			SetCardInfoFromRawSdsData(rawData, rawDataLen);
		}
		else if (rawDataLen == UNIVERSE_CARD_NUM_LENGTH)
		{
			// Try to set the number in the card object from the mcr raw data using the Penn Universe card format.
			const std::vector<WORD>propCodes = config.GetUniversePropCodes();
			const std::vector<BYTE>prefixes = config.GetUniverseCardPrefix();
			SetCardInfoFromRawUniverseData(rawData, rawDataLen, propCodes, prefixes);
		}
	}

	if (!IsValidRead() && rawDataLen == RFID_DATA_SIZE)
	{
		// Allows RCCL RFID global card to be read before sentinel onlined
		SetCardInfoFromRawRcclData(rawData, rawDataLen, config.GetRcclCardPlayerPrefix(), m_siteNumber, true);
	}

	if (!IsValidRead() && rawDataLen > UNIVERSE_CARD_NUM_LENGTH)
	{
		// Try to set the number in the card object from the mcr raw data using the Oasis card format.
		SetCardInfoFromRawAtiData(rawData, rawDataLen);
	}
}

// This method converts the character data from the card reader parsed as SDS card format into a 5 byte OCR.
// The input data has the format:
//  "%SSSTUNNNNNNNNN?" for track 1
//  ";SSSTUNNNNNNNNN?" for track 2
// % or ; = Begining Character
//		S = Property Code
//		T = Card Type (0: player, 1: employee)
//		U = Unused (0 value)
//		N = Customer Account Number (9 digit)
//		? = Last character
void CCard::SetCardInfoFromRawSdsData(const byte* rawData, int rawDataLen)
{
	bool bStatus = true;

	// Verify the first character
	if (bStatus && rawDataLen >= (int)sizeof(TRACK_1_CARD_START_VALUE) &&
		(*rawData == TRACK_1_CARD_START_VALUE || *rawData == TRACK_2_CARD_START_VALUE))
	{
		rawData++;
		rawDataLen--;
	}
	else
	{
		bStatus = false;
	}

	// Get the property code.
	uint16_t propertyCode = 0;
	if (bStatus && rawDataLen > SDS_CARD_PROPERTY_CODE_LENGTH)
	{
		rawData = GetNextField(rawData, rawDataLen, SDS_CARD_PROPERTY_CODE_LENGTH, propertyCode);
	}
	else
	{
		bStatus = false;
	}

	// Get the card type (0 for player card)
	if (bStatus && rawDataLen > SDS_CARD_TYPE_LENGTH)
	{
		byte cardType = 0;
		rawData = GetNextField(rawData, rawDataLen, SDS_CARD_TYPE_LENGTH, cardType);

		// We will only accept player cards (cardType = 0) for SDS format.
		// CardType = 1 in SDS format means employee cards, and we will only accept
		// employee cards in ATI format.
		if (0 != cardType)
		{
			bStatus = false;
		}
	}
	else
	{
		bStatus = false;
	}

	// Ignore the unused position.
	rawData++;
	rawDataLen--;

	// Gets the account number value
	DWORD accountNumber = 0;
	if (bStatus && rawDataLen > SDS_CARD_ACC_NUM_LENGTH)
	{
		rawData = GetNextField(rawData, rawDataLen, SDS_CARD_ACC_NUM_LENGTH, accountNumber);
	}
	else
	{
		bStatus = false;
	}

	// Verify the last character.
	if (bStatus && (rawDataLen < (int)sizeof(CARD_END_VALUE) || *rawData != CARD_END_VALUE))
	{
		bStatus = false;
	}

	m_validRead = bStatus;

	// Make the card ID from the info gathered above
	if (bStatus)
	{
		if (nullptr != m_cardID)
			delete[] m_cardID;

		m_cardIDLen = CARD_ID_LEN;

		m_cardID = new byte[CARD_ID_LEN];

		m_cardID[0] = propertyCode & 0xff;
		m_cardID[1] = ((propertyCode >> 2) & 0xC0) | ((accountNumber & 0xff000000) >> 24);
		m_cardID[2] = ((accountNumber & 0xff0000) >> 16);
		m_cardID[3] = ((accountNumber & 0xff00) >> 8);
		m_cardID[4] = (accountNumber & 0xff);
	}
}

// This method converts the character data from the card reader into 
// a 5 byte array.
// The input data has the format:
//  ";SSSTTTXXYYNNNNNNNN?"
//		; = begining character
//		S = Site number
//		T = card type
//		X = X value
//		Y = Y value
//		N = Number
//		? = End chatacter
// The above numbers are decimal numbers in character form
// They are converted into bytes in the following format to form the ID
//  TZNNN
//		T = Card type (1 byte)
//		Z = X value high nibble/Y value low nibble (1 byte)
//		N = Number (3 bytes)
void CCard::SetCardInfoFromRawAtiData( const byte *rawData, int rawDataLen )
{
	bool bStatus = true;

	// Need rawDataLen is >= 0 so C/C++ signed/unsigned comparisons work as intended.
	// For safety, explicitly check for NULL pointers and rawDataLen > 0
	if ( bStatus && rawData != NULL && rawDataLen > 0)
	{
		// Okay so far.
	}
	else
	{
		bStatus = false;
	}


	// Make sure it starts with the start character
	if (bStatus && rawDataLen >= (int)sizeof(TRACK_2_CARD_START_VALUE) &&
		(*rawData == TRACK_2_CARD_START_VALUE || *rawData == TRACK_1_CARD_START_VALUE))
	{
		rawData++;
		rawDataLen--;
	}
	else
	{
		bStatus = false;
	}


	// Get the site number.
	// NOTE: This does not get used in the ID. Just store it as a member variable
	if ( bStatus && rawDataLen > CARD_SITE_LEN )
	{
		rawData = GetNextField( rawData, rawDataLen, CARD_SITE_LEN, m_siteNumber );
	}
	else
	{
		bStatus = false;
	}

	// Get the card type number
	// This is used for the ID as well as stored as a member variable
	if ( bStatus && rawDataLen > CARD_TYPE_LEN )
	{
		rawData = GetNextField( rawData, rawDataLen, CARD_TYPE_LEN, m_cardTypeNumber );
	}
	else
	{
		bStatus = false;
	}

	// Get the X and Y values. This gets combined to be used in the ID
	byte xValue = 0;
	if ( bStatus && rawDataLen > CARD_X_LEN )
	{
		rawData = GetNextField( rawData, rawDataLen, CARD_X_LEN, xValue );
	}
	else
	{
		bStatus = false;
	}
	byte yValue = 0;
	if ( bStatus && rawDataLen > CARD_Y_LEN )
	{
		rawData = GetNextField( rawData, rawDataLen, CARD_Y_LEN, yValue );
	}
	else
	{
		bStatus = false;
	}

	// Gets the number value
	DWORD number = 0;
	if ( bStatus && rawDataLen > CARD_NUM_LEN )
	{
		rawData = GetNextField( rawData, rawDataLen, CARD_NUM_LEN, number );
	}
	else
	{
		bStatus = false;
	}

	// Make sure the ending character is there.
	if ( bStatus && (rawDataLen < (int)sizeof(CARD_END_VALUE) || *rawData != CARD_END_VALUE) )
	{
		bStatus = false;
	}

	// Make the card ID from the info gathered above
	if ( bStatus )
	{
		if ( m_cardID != NULL )
			delete [] m_cardID;

		m_cardIDLen = CARD_ID_LEN;

		m_cardID = new byte[m_cardIDLen];

		m_cardID[CARD_ID_TYPE_POS] = m_cardTypeNumber;
		m_cardID[CARD_ID_XY_POS] = (xValue & 0xf) << 4 | (yValue & 0xf);
		m_cardID[CARD_ID_NUM_POS] = (byte)((number & 0xff0000) >> 16);
		m_cardID[CARD_ID_NUM_POS+1] = (byte)((number & 0xff00) >> 8);
		m_cardID[CARD_ID_NUM_POS+2] = (byte)(number & 0xff);
	}

	// Tracker #21830
	// Tracker #22372
	if ( bStatus && (number != 0xFF0001 || m_cardTypeNumber != 0x69 || yValue != 0x09))
	{
		// Check high bit count
		int hbc = 0;
		int seed = number;
		for (hbc = 0; seed; hbc++)
		{
			seed &= (seed - 1);
		}
		
		if ((hbc & 0x0f) == yValue)
		{
			// OCR meets high bit count check, now try xor check
			int _xor = 0;
			_xor = (m_cardID[CARD_ID_NUM_POS] & 0xf) ^ ((m_cardID[CARD_ID_NUM_POS] & 0xf0) >> 4);
			_xor ^= m_cardID[CARD_ID_NUM_POS + 1] & 0xf;
			_xor ^= (m_cardID[CARD_ID_NUM_POS + 1] & 0xf0) >> 4;
			_xor ^= m_cardID[CARD_ID_NUM_POS + 2] & 0xf;
			_xor ^= (m_cardID[CARD_ID_NUM_POS + 2] & 0xf0) >> 4;
			_xor ^= m_siteNumber & 0xf;
			_xor ^= (m_siteNumber & 0xf0) >> 4;

			if (_xor != xValue)
			{
				bStatus = false;
			}

		}
		else
			bStatus = false;
	}

	if (bStatus)
	{
		m_validRead = bStatus;
	}
	else
	{
		SetAsInvalidRead();
	}
}


// CCard::SetCardInfoFromRawNclData()
//     IN rawData - data from the mcr raw data read -- MUST NOT be NULL.
//     IN rawDataLen - number of bytes read and stored in readbuffer.
//     RETURN - true if card info was set to a valid read, else the return value is false. 
// This method converts the character data from the card reader into 
// a 5 byte array.
// The input data has the format:
//  ";HHHHHHHHHH?"
//		(1) ; = begining character
//		(12) H = hexadecimal OCR character
//		(1) ? = End character
//      14 bytes total length
bool CCard::SetCardInfoFromRawNclData(const byte *rawData, int rawDataLen, byte siteNumber /* will be in cardconfig class */)
{
	bool bStatus = true; // Card data is valid.

	// Make sure the card data starts with a start character.
	if (bStatus && rawDataLen >= (int)sizeof(TRACK_2_CARD_START_VALUE) &&
		(*rawData == TRACK_2_CARD_START_VALUE || *rawData == TRACK_1_CARD_START_VALUE))
	{
		rawData++;
		rawDataLen--;
	}
	else
	{
		bStatus = false;
	}

	// Get OCR field from the card data and store it into a string.
	string ocrStr;
	if (bStatus)
	{
		// Make sure the card data has sufficient length.
		if (rawDataLen >= NCL_CARD_OCR_LENGTH)
		{
			for (int charCount = 0; charCount < NCL_CARD_OCR_LENGTH; ++charCount)
			{
				// Make sure each character of the the card data is valid.
				TCHAR ch = (TCHAR)*rawData;
				if (isxdigit(ch))
				{
					ocrStr += (TCHAR)*rawData;
					rawData++;
					rawDataLen--;
				}
				else
				{
					bStatus = false;
					break;
				}
			}
		}
		else
		{
			bStatus = false;
		}
	}

	// Make sure the card data has the end character.
	if (bStatus && rawDataLen >= (int)sizeof(NCL_CARD_END_VALUE) && *rawData == NCL_CARD_END_VALUE)
	{
		rawDataLen--;
	}
	else
	{
		bStatus = false;
	}

	// Make sure the card data has the correct length.
	if (rawDataLen != 0)
	{
		bStatus = false;
	}

	// Set the card ID from the string, if the card data is valid.
	if (bStatus)
	{
		SetCardID(ocrStr);
		m_siteNumber = siteNumber;
	}

	return bStatus;
}

bool CCard::SetCardInfoFromRawRcclData( const byte *rawData, int rawDataLen, byte rcclCardType, byte siteNumber, bool bEmployeeOnly)
{
	bool bStatus = true;
	bool bIsRfid = false;
	byte rfidPlayerData[RFID_PLAYER_SIZE];

	if (rawData == NULL)
	{
		rawDataLen = 0;
	}

	// Make sure the rccl card type charater is specified.
	if ( rcclCardType == 0 )
	{
		bStatus = false;
	}

	bool bIsPlayer = true;

	// Make sure it starts with the start character
	if (bStatus && rawDataLen == RFID_DATA_SIZE)
	{
		bIsRfid = true;
		
		// Added bEmployeeOnly check so player cards cannot be read until RCCL is enabled
		if (memcmp(rawData + RFID_PLAYER_LOC, NULLS, RFID_PLAYER_SIZE) && !bEmployeeOnly)
		{
			// player card
			rawData += RFID_PLAYER_LOC;

			// Copy the last 8 digits of the byte-reversed binary primaryFolioNumber
			// into the ASCII character rfidPlayerData byte buffer.
			UINT64 primaryFolioNumber = *(PUINT64)rawData;
			for (int index = _countof(rfidPlayerData) - 1; index >= 0; --index)
			{
				rfidPlayerData[index] = (byte)('0' + primaryFolioNumber % 10);
				primaryFolioNumber /= 10;
			}

			// Point rawData to the rfidPlayerData buffer.
			rawData = rfidPlayerData;
			rawDataLen = RFID_PLAYER_SIZE;
			bIsPlayer = true;
		}
		else if (memcmp(rawData + RFID_EMPLOYEE_LOC, NULLS, RFID_PLAYER_SIZE))
		{
			// employee card
			rawData += RFID_EMPLOYEE_LOC;
			rawDataLen -= RFID_EMPLOYEE_LOC;
			bIsPlayer = false;
		}
		else
		{
			bStatus = false;
		}
	}
	else if (bStatus && rawDataLen > (int)sizeof(TRACK_2_CARD_START_VALUE) && *rawData == TRACK_2_CARD_START_VALUE)
	{
		if (rawDataLen == RCCL_CARD_LENGTH_TYPE2)
		{
			rawData += RCCL_CARD_FIRST_READ_POS_TYPE2;
			rawDataLen -= RCCL_CARD_FIRST_READ_POS_TYPE2;
		}
		else
		{
			// The next character we read is 11 characters in
			rawData+=RCCL_CARD_FIRST_READ_POS;
			rawDataLen-=RCCL_CARD_FIRST_READ_POS;
		}
	}
	else
	{
		bStatus = false;
	}
	
	// Get the fields from the the raw data.
	byte xyValue = 0;
	byte numValue1 = 0;
	byte numValue2 = 0;
	byte numValue3 = 0;
	if ( bStatus && rawDataLen >= (RCCL_CARD_CHARS_PER_DIGIT * RCCL_CARD_NUM_OF_DIGITS) )
	{
		if (bIsPlayer)
		{
			rawData = GetNextField(rawData, rawDataLen, RCCL_CARD_CHARS_PER_DIGIT, xyValue);
			rawData = GetNextField(rawData, rawDataLen, RCCL_CARD_CHARS_PER_DIGIT, numValue1);
			rawData = GetNextField(rawData, rawDataLen, RCCL_CARD_CHARS_PER_DIGIT, numValue2);
			rawData = GetNextField(rawData, rawDataLen, RCCL_CARD_CHARS_PER_DIGIT, numValue3);

			// Make sure the rccl ending character is there.
			if (!bIsRfid)
			{
				if (rawDataLen > (int)sizeof(RCCL_CARD_END_VALUE) && *rawData == RCCL_CARD_END_VALUE)
				{
					rawData++;
					rawDataLen--;
				}
				else
				{
					bStatus = false;
				}
			}
		}
		else
		{
			if (m_cardID != NULL)
				delete[] m_cardID;

			m_cardIDLen = CARD_ID_LEN;
			m_cardID = new byte[m_cardIDLen];

			// if bEmployeeOnly is true then only allow global global cards
			if (!bEmployeeOnly || memcmp(rawData, GLOBAL_GLOBAL_CARD_NUM, GLOBAL_GLOBAL_CARD_LEN) == 0)
			{

				m_siteNumber = siteNumber;

				// copy employee ocr
				memcpy_s(m_cardID, m_cardIDLen, rawData, m_cardIDLen);

				m_cardTypeNumber = m_cardID[CARD_ID_TYPE_POS];
			}
			else
			{
				bStatus = false;
			}
		}
	}
	else
	{
		bStatus = false;
	}

	// Make sure the ending character is there.
	if (bStatus &&  !bIsRfid && (rawDataLen < (int)sizeof(CARD_END_VALUE) || *rawData != CARD_END_VALUE))
	{
		bStatus = false;
	}


	// Make the card ID from the fields gathered above
	if ( bStatus && bIsPlayer )
	{
		if ( m_cardID != NULL )
			delete [] m_cardID;

		m_cardIDLen = CARD_ID_LEN;

		m_cardID = new byte[m_cardIDLen];
		m_siteNumber = siteNumber;

		m_cardTypeNumber = rcclCardType;
		m_cardID[CARD_ID_TYPE_POS] = rcclCardType;
		m_cardID[CARD_ID_XY_POS] = ((xyValue/10) * 16) + (xyValue%10);
		m_cardID[CARD_ID_NUM_POS] = ((numValue1/10) * 16) + (numValue1%10);
		m_cardID[CARD_ID_NUM_POS+1] = ((numValue2/10) * 16) + (numValue2%10);
		m_cardID[CARD_ID_NUM_POS+2] = ((numValue3/10) * 16) + (numValue3%10);
	}

	m_validRead = bStatus;

	return bStatus;
}

// This method converts the character data from the card reader into 
// a 5 byte array.
// The input data has the format:
//  ";0PPPPAAAAAAAAAC?"
//		(1) ; = begining character
//		(1) 0 = leading zero
//		(4) P = Property Code used to determine card prefix from table
//		(9) A = Account number 9 digit player acount number attached to card prefix to create 10 digit OCR
//		(1) C = Check Digit
//		(1) ? = End character
//      17 bytes total length
void CCard::SetCardInfoFromRawUniverseData(const byte *rawData, int rawDataLen, const std::vector<WORD>&propCodes, const std::vector<BYTE>&universePrefixes)
{
	m_isUniversalCard = true;

	LogString(GENERAL, "Attempting to interpret inserted card as a penn/universal player card.");

	bool bStatus = true;

	//get rid of first two items
	rawData+=2;
	rawDataLen-=2;	

	// Get the property code.
	// NOTE: This does not get used in the ID. Just store it as a member variable
	if ( bStatus && rawDataLen > UNIVERSE_CARD_PROP_LEN )
	{
		//property code is encoded on the mag stripe as a two-digit decimal ASCII number 
		//  for each of the the two ASCII characters of the code. 
		//   Example: "6548" on the mag stripe means property code "A0"
		//            - "65" is the ASCII code for 'A'
		//            - "48" is the ASCII code for '0'
        m_universalPropCode = (WORD)(CharToDWord(reinterpret_cast<const char *>(rawData), UNIVERSE_CARD_PROP_LEN));
		rawDataLen -= UNIVERSE_CARD_PROP_LEN;
		rawData += UNIVERSE_CARD_PROP_LEN;

		bool bFound = false;
		//Now that we have the prop code translate it to the prefix if the prefix does not exist lets stop here.

        for (int propCodeIndex = 0; propCodeIndex < propCodes.size() && !bFound ; propCodeIndex++)
		{
			if(propCodes[propCodeIndex] == m_universalPropCode)
			{
				bFound = true;
				m_cardTypeNumber = universePrefixes[propCodeIndex];
                m_cardType = CARD_PLAYER;
                LogString(GENERAL, _T("Universal prop code %c%c found. Player prefix %x determined. Setting this card as a valid player card."), (char)(m_universalPropCode/100), (char)(m_universalPropCode%100), m_cardTypeNumber);
			}
		}
		bStatus = bFound;

		if(!bFound)
		{
			LogString(ERROR_LOG, _T("Universal prefix not found for prop code: %c%c"), (char)(m_universalPropCode/100), (char)(m_universalPropCode%100));
			
		}
	}
	else
	{
		//if we did not find the prefix set bStatus to false
		bStatus = false;		
		LogString(ERROR_LOG, "Universal card encoding was too short");
		
	}

	byte accountNumber[UNIVERSE_ACCOUNT_NUM_LEN];

	//Get the Account Number (represented as a 9-digit decimal ASCII number in the mag stripe)
	if( bStatus && rawDataLen > UNIVERSE_ACCOUNT_NUM_LEN )
	{
		memcpy_s(&accountNumber, UNIVERSE_ACCOUNT_NUM_LEN, rawData, UNIVERSE_ACCOUNT_NUM_LEN);
		rawDataLen -= UNIVERSE_ACCOUNT_NUM_LEN;
		rawData += UNIVERSE_ACCOUNT_NUM_LEN;
	}
	else
	{
		bStatus = false;
	}

	byte checkDigit;
	//Get the Check Digit
	if( bStatus && rawDataLen > UNIVERSE_CHECK_DIGIT_LEN )
	{
		memcpy_s(&checkDigit, UNIVERSE_CHECK_DIGIT_LEN, rawData, UNIVERSE_CHECK_DIGIT_LEN);
		rawDataLen -= UNIVERSE_CHECK_DIGIT_LEN;
		rawData += UNIVERSE_CHECK_DIGIT_LEN;
	}
	else
	{
		bStatus = false;
	}

	// Make the card ID from the fields gathered above
	if ( bStatus )
	{
		// convert the ASCII account number to a binary int32, and
		// create the checksum intermediate value in one loop over the account number characters
		int accno = 0;
		int checkInterm = 0;
		for (int i = 0; i < 9; i++)
		{
			byte digitValue = accountNumber[i] - ((byte)'0');

			accno = accno * 10 + digitValue;

			if ((i % 2) == 0)
			{
				checkInterm += digitValue * 2 + ((digitValue >= 5) ? 1 : 0);
			}
			else
				checkInterm += digitValue;
		}

		LogString(GENERAL, "Universal card account#: %d", accno);
		LogString(GENERAL, "Universal card intermediate sum: %d", checkInterm);
		LogString(GENERAL, "Universal card property prefix: 0x%X", m_cardTypeNumber);
		

		checkInterm %= 10;

		if (checkInterm != 0)
			checkInterm = 10 - checkInterm;

		// if the check digit matches, create the OCR
		if (checkInterm == (checkDigit - ((byte)'0')))
		{
			if ( m_cardID != NULL )
				delete [] m_cardID;

			m_cardIDLen = CARD_ID_LEN;

			m_cardID = new byte[m_cardIDLen];

			//m_siteNumber = siteNumber;
			//m_cardTypeNumber = rcclCardType;

			// encode the OCR as our prefix translated from the property id,
			// followed by the 4 bytes of the binary account number, most significant byte first
			m_cardID[0] = m_cardTypeNumber;
			m_cardID[1] = (accno >> 24) & 0xFF;
			m_cardID[2] = (accno >> 16) & 0xFF;
			m_cardID[3] = (accno >> 8) & 0xFF;
			m_cardID[4] = (accno >> 0) & 0xFF;
		}
		else 
		{
			// check digit did not match, mark it as a failed read
			bStatus = false;

			LogString(ERROR_LOG, "Universal check digit did not match rejecting card. Calculated: %d. Found on card: %d", checkInterm, (checkDigit - ((byte)'0')));
			
		}
	}

	m_validRead = bStatus;
}

// A card with a floor card prefix may be a currency or meter card.
// This is determined by the ID.
CardType CCard::DetermineFloorCardType( void )
{
	CardType cardType = CARD_FLOOR;

	if ( m_cardID != NULL )
	{
		if ( m_cardID[CARD_ID_NUM_POS] == 0x10 && m_cardID[CARD_ID_NUM_POS+1] == 0x00 && m_cardID[CARD_ID_NUM_POS+2] == 0x00)
			cardType = CARD_CURRENCY;
		else if ( m_cardID[CARD_ID_NUM_POS] == 0x10 && m_cardID[CARD_ID_NUM_POS+1] == 0x00 && m_cardID[CARD_ID_NUM_POS+2] == 0x01)
			cardType = CARD_METER;
		else if ( m_cardID[CARD_ID_NUM_POS] == 0x10 && m_cardID[CARD_ID_NUM_POS+1] == 0x00 && m_cardID[CARD_ID_NUM_POS+2] == 0x11)
			cardType = CARD_METER;
	}

	return cardType;
}

const byte * CCard::GetNextField( const byte *rawDataPtr, int &rawDataLen, int fieldLen, DWORD &value )
{
	value = CharToDWord( (char *)rawDataPtr, fieldLen);

	rawDataLen -= fieldLen;
	return rawDataPtr + fieldLen;
}

const byte * CCard::GetNextField( const byte *rawDataPtr, int &rawDataLen, int fieldLen, byte &value )
{
	value = (byte)CharToDWord( (char *)rawDataPtr, fieldLen);

	rawDataLen -= fieldLen;
	return rawDataPtr + fieldLen;
}

const byte* CCard::GetNextField(const byte* rawDataPtr, int& rawDataLen, int fieldLen, uint16_t& value)
{
	value = (uint16_t)CharToDWord((char*)rawDataPtr, fieldLen);

	rawDataLen -= fieldLen;
	return rawDataPtr + fieldLen;
}

DWORD CCard::CharToDWord( const char *charData, int charLen )
{
	DWORD retVal = 0;
	long factor = 1;
	for ( int ci = charLen - 1; ci >= 0; ci--, factor *= 10 )
	{
		retVal += (charData[ci] - '0') * factor;
	}

	return retVal;
}

bool CCard::Compare(LPCTSTR cardToCompare) const
{
	BYTE tempOCR[CARD_ID_LEN];
	CUtilities::MakeCardIDFromCString(cardToCompare, tempOCR, CARD_ID_LEN);
	return Compare( tempOCR, CARD_ID_LEN );
}

bool CCard::Compare( const CCard *cardToCompare ) const
{
	bool isSame(false);

	if (cardToCompare != NULL)
	{
		int cardIDLen;
		const byte *cardID = cardToCompare->GetCardID(cardIDLen);

		isSame = Compare( cardID, cardIDLen );
	}

	return isSame;
}

bool CCard::Compare( const byte *cardID, int cardIDLen ) const
{
	bool bIsSame = false;

	if ( cardID != NULL && cardIDLen == m_cardIDLen )
	{
		bIsSame = true;
		for ( int ci = 0; ci < cardIDLen && bIsSame; ci++ )
		{
			if ( cardID[ci] != m_cardID[ci] )
				bIsSame = false;
		}
	}

	return bIsSame;

}

bool CCard::CheckCommID( byte commId ) const
{
	// Tracker #21732
	// compare site id with commId passed in.
	// if they are identical or card is a global card, return true

	bool retVal = false;
	if (commId == m_siteNumber || Compare( (byte *)GLOBAL_GLOBAL_CARD_NUM, GLOBAL_GLOBAL_CARD_LEN) || m_isUniversalCard)
	{
		retVal = true;
	}
	
	return retVal;
}

void CCard::ClearLastCardReadIfNecessary(CConfig& config)
{
	CCardConfig cardConfig;
	cardConfig.Setup(config);
	string ocrCardNumber(CUtilities::MakeStringFromCardID(config.GetLastCardRead(), CARD_ID_LEN));
	SetCardID(ocrCardNumber, &cardConfig);
	if ((m_cardTypeNumber == cardConfig.GetGlobalCardPrefix() ||
		m_cardTypeNumber == cardConfig.GetMechanicCardPrefix() ||
		m_cardTypeNumber == cardConfig.GetFloorCardPrefix()))
	{
		LogString(POLLER_LOG, "OCR=%s is cleared at startup because it is an employee card.", ocrCardNumber.c_str());
		config.ClearLastCardRead();
	}
}

std::string CCard::MakeCStringCardNumber(const CCard *card)
{
	string cardNumber;

	if (card != nullptr)
	{
		int cardIDLen;
		const byte *cardID(card->GetCardID(cardIDLen));
		cardNumber = CUtilities::MakeStringFromCardID(cardID, cardIDLen);
	}

	return cardNumber;
}

// CCard::SetCardInfo()
//     IN cardType - indicates the CardType used to update m_cardType of the instance.
//     IN cardID - byte array used to update m_cardID of the instance.
//         MUST be at least cardIDLen bytes long.
//     IN cardIDLen - length of byte array used to update m_cardID of the instance.
//         MUST be >= 0.
//         Typically, has the value CARD_ID_LEN.
// This is used to set card information for the instance.
void CCard::SetCardInfo( CardType cardType, const byte *cardID, int cardIDLen )
{
	m_cardType = cardType;
	m_cardIDLen = cardIDLen;

	m_cardID = new byte[cardIDLen];
	ZeroMemory(m_cardID, cardIDLen);
	
	if(cardID != NULL && m_cardIDLen > CARD_ID_TYPE_POS)
	{
		memcpy( m_cardID, cardID, m_cardIDLen );
		m_cardTypeNumber = m_cardID[CARD_ID_TYPE_POS];
		m_siteNumber = DEFAULT_SITE_NUMBER;
		m_validRead = true; 
	}
}

void CCard::SetAsValidPlayer(void)
{
	if (m_cardID != NULL && m_cardIDLen > CARD_ID_TYPE_POS)
	{
		m_cardType = CARD_PLAYER;
		m_cardTypeNumber = m_cardID[CARD_ID_TYPE_POS];
		m_validRead = true;
	}
}
