#include "stdafx.h"
#include "ConfigSlotNumberField.h"
#include "Utilities.h"

/// <summary>
/// Constructor for a CConfigSlotNumberField instance.
/// </summary>
CConfigSlotNumberField::CConfigSlotNumberField(void) :
	CConfigField(SLOT_NUMBER),
	m_slotNumber(0)
{
}

/// <summary>
/// Destructor for a CConfigSlotNumberField instance.
/// </summary>
CConfigSlotNumberField::~CConfigSlotNumberField(void)
{
}

/// <summary>
/// Set the Slot Number.
/// </summary>
/// <param name="slotNumber">IN - address of source buffer. If buffer is NULL then a length of zero is used.</param>
/// <param name="length">IN - size of source buffer. If length is negative then a length of zero is used.</param>
/// <remarks>
/// The Slot Number may also be known as the Machine Number.
/// </remarks>
void CConfigSlotNumberField::SetSlotNumber(const byte * slotNumber, int length)
{
	CUtilities::MemCopyZero(&m_slotNumber, sizeof(m_slotNumber), slotNumber, length);
}

/// <summary>
/// Returns the Slot Number.
/// </summary>
/// <returns>The Slot Number.</returns>
/// <remarks>
/// The Slot Number may also be known as the Machine Number.
/// </remarks>
DWORD CConfigSlotNumberField::GetSlotNumber() const
{
	return m_slotNumber;
}


