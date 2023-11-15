#include "stdafx.h"
#include "ConfigTimezoneField.h"
#include "Utilities.h"

/// <summary>
/// Constructor for a CConfigTimezoneField instance.
/// </summary>
CConfigTimezoneField::CConfigTimezoneField(void) :
	CConfigField(TIMEZONE),
	m_timezoneOffset(0),
	m_bUseDST(false)
{
}

/// <summary>
/// Destructor for a CConfigTimezoneField instance.
/// </summary>
CConfigTimezoneField::~CConfigTimezoneField(void)
{
}

/// <summary>
/// Set time zone offset and Daylight Saving Time flag.
/// </summary>
/// <param name="buffer">IN - address of source buffer. If buffer is NULL then a length of zero is used.</param>
/// <param name="length">IN - size of source buffer. If length is negative then a length of zero is used.</param>
void CConfigTimezoneField::SetTimezoneOptions(const byte * buffer, int length)
{
	size_t bufferSize = 0 <= length ? (size_t)length : 0;
	size_t bufferOffset = 0;
	bool ok = true;

	CUtilities::MemCopyZero_FromOffset(&ok, &m_timezoneOffset, sizeof(m_timezoneOffset),
		buffer, bufferSize, &bufferOffset);

	byte useDSTFlag;
	CUtilities::MemCopyZero_FromOffset(&ok, &useDSTFlag, sizeof(useDSTFlag),
		buffer, bufferSize, &bufferOffset);
	m_bUseDST = useDSTFlag == 0 ? false : true;
}

//// <summary>
/// Returns the time zone offset.
/// </summary>
/// <returns>The time zone offset.</returns>
short CConfigTimezoneField::GetTimezoneOffset( void ) const
{
	return m_timezoneOffset;
}

//// <summary>
/// Returns true if Daylight Saving Time is in effect. Returns false otherwise.
/// </summary>
/// <returns>true if Daylight Saving Time is in effect. Returns false otherwise.</returns>
bool CConfigTimezoneField::UseDST( void ) const
{
	return m_bUseDST;
}


