#include "stdafx.h"

#include "ConfigNclEnabledCardTrackField.h"

#include "GlobalDefs.h"
#include "Logging/LogString.h"

// CNclEnabledCardTrackField::CNclEnabledCardTrackField()
// Constructor.
CNclEnabledCardTrackField::CNclEnabledCardTrackField(void)
{
	m_type = NCL_ENABLED_CARD_TRACK;
	m_nclEnabled = (byte)NCL_CARDS_ENABLED_DEFAULT;
	m_cardTrack = CARD_READER_TRACK_NUMBER_DEFAULT;
}

// CNclEnabledCardTrackField::~CNclEnabledCardTrackField()
// Destructor.
CNclEnabledCardTrackField::~CNclEnabledCardTrackField(void)
{
}

// CNclEnabledCardTrackField::GetNclEnabled()
//     IN buffer - reference MUST point to a readable location at least "length" bytes long.
//     IN length - indicates the length of the buffer that is used to specify the subsidary field values.
// Set the subsidiary fields that make up the whole configurable "field".
void CNclEnabledCardTrackField::SetField(const byte buffer[], int length)
{
	int srcLength;
	int offset = 0;

	length = max(0, length);

	srcLength = min((int)sizeof(m_nclEnabled), length);
	CUtilities::MemCopyZero(&m_nclEnabled, sizeof(m_nclEnabled), buffer + offset, srcLength);
	offset += srcLength;
	length -= srcLength;

	srcLength = min((int)sizeof(m_cardTrack), length);
	CUtilities::MemCopyZero(&m_cardTrack, sizeof(m_cardTrack), buffer + offset, srcLength);
}

// CNclEnabledCardTrackField::GetNclEnabled()
//     RETURN - the NCL Enabled configuration value, after converting it to a bool.
bool CNclEnabledCardTrackField::GetNclEnabled()
{
	return m_nclEnabled == (byte)true;
}

// CNclEnabledCardTrackField::GetCardTrack()
//     RETURN - the Card Track configuration value.
byte CNclEnabledCardTrackField::GetCardTrack()
{
	return m_cardTrack;
}

