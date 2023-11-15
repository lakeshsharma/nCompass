#include "stdafx.h"
#include "LongPoll7DMessage.h"

const byte X7D_RESPONSE_SIZE = 5;
const byte X7D_STATIC_DATA_LENGTH = 11;
const byte X7D_NONDATA_LENGTH = 5;

CLongPoll7DMessage::CLongPoll7DMessage(byte gameAddress) :
	CSASPollMessage(gameAddress),
	m_hostID(0),
	m_expiration(0),
	m_locationLength(0),
	m_address1Length(0),
	m_address2Length(0)
{
	// allocate buffers
	// Allocate send buffer when we know how big it is.
	// Allocate response buffer now.
	SetResponseBuffer((byte*)new byte[X7D_RESPONSE_SIZE],X7D_RESPONSE_SIZE);
	memset(GetResponseBuffer(),0,GetResponseBufferSize());
	
	m_address1 = NULL;
	m_address2 = NULL;
	m_location = NULL;
}

CLongPoll7DMessage::~CLongPoll7DMessage(void)
{
	CSASPollMessage::WaitForSASProcessingDone();

	if (m_location != NULL)
	{
		delete [] m_location;
	}
	if (m_address1 != NULL)
	{
		delete [] m_address1;
	}
	if (m_address2 != NULL)
	{
		delete [] m_address2;
	}
}

BYTE CLongPoll7DMessage::GetPollValue()
{
	return POLL_VALUE;
}

void CLongPoll7DMessage::Fill()
{
	// calculate message length
	byte length = m_locationLength + m_address1Length + m_address2Length + X7D_STATIC_DATA_LENGTH;

	SetSendBuffer(new byte[length],length);
	byte *buffer = m_sendBuffer;
	m_sendBuffer[GAME_ADDRESS_OFFSET] = m_gameAddress;
	buffer += sizeof(m_gameAddress);

	m_sendBuffer[POLL_VALUE_OFFSET] = POLL_VALUE;
	buffer += sizeof(byte);

	m_sendBuffer[OPTIONAL_LENGTH_OFFSET] = length - X7D_NONDATA_LENGTH;
	buffer += sizeof(byte);

	memcpy(buffer, &m_hostID, sizeof(m_hostID));
	buffer += sizeof(m_hostID);

	memcpy(buffer, &m_expiration, sizeof(m_expiration));
	buffer += sizeof(m_expiration);
	
	memcpy(buffer, &m_locationLength, sizeof(m_locationLength));
	buffer += sizeof(m_locationLength);

	if (m_locationLength > 0)
	{
		memcpy(buffer, m_location, m_locationLength);
		buffer += m_locationLength;
	}

	memcpy(buffer, &m_address1Length, sizeof(m_address1Length));
	buffer += sizeof(m_address1Length);
	if (m_address1Length > 0)
	{
		memcpy(buffer, m_address1, m_address1Length);
		buffer += m_address1Length;
	}

	memcpy(buffer, &m_address2Length, sizeof(m_address2Length));
	buffer += sizeof(m_address2Length);
	if (m_address2Length > 0)
	{
		memcpy(buffer, m_address2, m_address2Length);
		buffer += m_address2Length;
	}

	byte *crcLocation;
	ushort messageCRC = CRC(GetSendBuffer(),GetSendBufferSize() - CRCSIZE,0);
	crcLocation = buffer;
	memcpy(crcLocation,&messageCRC,CRCSIZE);
}

const static byte MAXIMUM_LOCATION_LENGTH = 40; // SAS limit.
const static byte MAXIMUM_ADDRESS1_LENGTH = 40; // SAS limit.
const static byte MAXIMUM_ADDRESS2_LENGTH = 40; // SAS limit.

void CLongPoll7DMessage::SetAddressData(const byte *location,
	byte locationLength,
	const byte *address1,
	byte address1Length,
	const byte *address2,
	byte address2Length)
{
	// clean up previous values
	if (m_location != NULL)
	{
		delete [] m_location;
		m_location = NULL;
		m_locationLength = 0;
	}
	if (m_address1 != NULL)
	{
		delete [] m_address1;
		m_address1 = NULL;
		m_address1Length = 0;
	}
	if (m_address2 != NULL)
	{
		delete [] m_address2;
		m_address2 = NULL;
		m_address2Length = 0;
	}

	// set new values
	if (location != NULL)
	{
		locationLength = min(locationLength, MAXIMUM_LOCATION_LENGTH);
		m_location = new byte[locationLength];
		memcpy(m_location,location,locationLength);
		m_locationLength = locationLength;
	}

	if (address1 != NULL)
	{
		address1Length = min(address1Length, MAXIMUM_ADDRESS1_LENGTH);
		m_address1 = new byte[address1Length];
		memcpy(m_address1,address1,address1Length);
		m_address1Length = address1Length;
	}

	if (address2 != NULL)
	{
		address2Length = min(address2Length, MAXIMUM_ADDRESS2_LENGTH);
		m_address2 = new byte[address2Length];
		memcpy(m_address2,address2,address2Length);
		m_address2Length = address2Length;
	}
}

void CLongPoll7DMessage::SetHostID(WORD hostId)
{
	m_hostID = hostId;
}

void CLongPoll7DMessage::SetExpiration(byte expiration)
{
	m_expiration = expiration;
}
