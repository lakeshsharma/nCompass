#include "stdafx.h"
#include "ConfigTicketingField.h"

#include "Utilities.h"

/// <summary>
/// Constructor for a CConfigTicketingField instance.
/// </summary>
CConfigTicketingField::CConfigTicketingField(void) :
	CConfigField(TICKETING),
	m_ticketExpirationType(UNUSED),
	m_ticketExpirationDuration(0),
	m_ticketingEnabled(false),
	m_systemGeneratedTicketValidationNumbers(false),
	m_ticketBufferingEnabled(false)
{
}

/// <summary>
/// Destructor for a CConfigTicketingField instance.
/// </summary>
CConfigTicketingField::~CConfigTicketingField(void)
{
}

/// <summary>
/// Set ticketing options: Ticket Expiration Type, Ticket Expiration Duration, Ticketing Enabled,
/// System Generated Ticket Validation Numbers, and Ticket Buffering Enabled.
/// </summary>
/// <param name="buffer">IN - address of source buffer. If buffer is NULL then a length of zero is used.</param>
/// <param name="length">IN - size of source buffer. If length is negative then a length of zero is used.</param>
void CConfigTicketingField::SetTicketingOptions(const byte * buffer, int length)
{
	byte byteValue;
	WORD wordValue;
	size_t bufferSize = 0 <= length ? (size_t)length : 0;
	size_t bufferOffset = 0;
	bool ok = true;

	CUtilities::MemCopyZero_FromOffset(&ok, &byteValue, sizeof byteValue,
		buffer, bufferSize, &bufferOffset);
	m_ticketExpirationType = (TicketExpirationType)byteValue;

	//if the size is 5 we know we are talking to an older Floorlogix which does not support the extended expiration duration.
	if (length == OLDER_FLOORLOGIX_BUFFER_LENGTH)
	{
		CUtilities::MemCopyZero_FromOffset(&ok, &byteValue, sizeof byteValue,
			buffer, bufferSize, &bufferOffset);
		m_ticketExpirationDuration = byteValue;
	}
	else
	{
		CUtilities::MemCopyZero_FromOffset(&ok, &wordValue, sizeof wordValue,
			buffer, bufferSize, &bufferOffset);
		m_ticketExpirationDuration = wordValue;
	}

	CUtilities::MemCopyZero_FromOffset(&ok, &byteValue, sizeof byteValue,
		buffer, bufferSize, &bufferOffset);
	m_ticketingEnabled = (bool)byteValue;

	CUtilities::MemCopyZero_FromOffset(&ok, &byteValue, sizeof byteValue,
		buffer, bufferSize, &bufferOffset);
	m_systemGeneratedTicketValidationNumbers = (bool)byteValue;

	CUtilities::MemCopyZero_FromOffset(&ok, &byteValue, sizeof byteValue,
		buffer, bufferSize, &bufferOffset);
	m_ticketBufferingEnabled = (bool)byteValue;
}

//// <summary>
/// Returns the Ticket Expiration Type.
/// </summary>
/// <returns>The Ticket Expiration Type</returns>
TicketExpirationType CConfigTicketingField::GetTicketExpirationType() const
{
	return m_ticketExpirationType;
}

//// <summary>
/// Returns the Ticket Expiration Duration.
/// </summary>
/// <returns>The Ticket Expiration Duration.</returns>
WORD CConfigTicketingField::GetTicketExpirationDuration() const
{
	return m_ticketExpirationDuration;
}

//// <summary>
/// Returns true if Ticketing Enabled.  Returns false otherwise.
/// </summary>
/// <returns>true if Ticketing Enabled.  Returns false otherwise.</returns>
bool CConfigTicketingField::TicketingEnabled() const
{
	return m_ticketingEnabled;
}

//// <summary>
/// Returns true if System Generated Ticket Validation Numbers are used.  Returns false otherwise.
/// </summary>
/// <returns>true if System Generated Ticket Validation Numbers are used.  Returns false otherwise.</returns>
bool CConfigTicketingField::SystemGeneratedTicketValidationNumbers() const
{
	return m_systemGeneratedTicketValidationNumbers;
}

//// <summary>
/// Returns true if Ticket Buffering Enabled.  Returns false otherwise.
/// </summary>
/// <returns>true if Ticket Buffering Enabled.  Returns false otherwise.</returns>
bool CConfigTicketingField::TicketBufferingEnabled() const
{
	return m_ticketBufferingEnabled;
}
