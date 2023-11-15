#include "stdafx.h"
#include "ConfigSlotMastRevisionField.h"
#include "Utilities.h"

CConfigSlotMastRevisionField::CConfigSlotMastRevisionField(void) :
	CConfigField(SLOTMAST_REVISION),
	m_SlotmastRevision(0)
{
}

CConfigSlotMastRevisionField::~CConfigSlotMastRevisionField(void)
{
}

void CConfigSlotMastRevisionField::SetSlotMastRevision(const byte * buffer, int length)
{
	CUtilities::MemCopyZero(&m_SlotmastRevision, sizeof(m_SlotmastRevision), buffer, length);
}

WORD CConfigSlotMastRevisionField::GetSlotMastRevision() const
{
	return m_SlotmastRevision;
}


