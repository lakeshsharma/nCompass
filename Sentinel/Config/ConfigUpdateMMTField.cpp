#include "stdafx.h"

#include "ConfigUpdateMMTField.h"

// CConfigUpdateMMTField::CConfigUpdateMMTField.
// Constructor for a CConfigUpdateMMTField instance.
CConfigUpdateMMTField::CConfigUpdateMMTField(void) :
	CConfigField(SEND_FULL_METER_UPDATE),
	m_updateMMT(false)
{
}

// CConfigUpdateMMTField::~CConfigUpdateMMTField.
// Destructor for a CConfigUpdateMMTField instance.
CConfigUpdateMMTField::~CConfigUpdateMMTField(void)
{
}

// CConfigVirtualSessionField::SetUpdateMMT.
//     IN updateMMT - true enables sending updates to MMT, false disables.
// Set whether sending updates to MMT is enabled or not.
void CConfigUpdateMMTField::SetUpdateMMT(bool updateMMT)
{
	m_updateMMT = updateMMT;
}

// CConfigVirtualSessionField::GetUpdateMMT.
//     RETURN - true if sending updates to MMT is enabled.  Returns false otherwise.
bool CConfigUpdateMMTField::GetUpdateMMT() const
{
	return m_updateMMT;
}


