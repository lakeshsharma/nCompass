#include "stdafx.h"
#include "ConfigField.h"

// CConfigField::CConfigField
// Default constructor for a CConfigField instance.
CConfigField::CConfigField(void)
{
}

// CConfigField::CConfigField
//     IN type - indicates the field type for the instance.
// Constructor for a CConfigField instance.
CConfigField::CConfigField(FieldTypes type) :
	m_type(type)
{
}

// CConfigField::~CConfigField
// Destructor for CConfigField instances.
CConfigField::~CConfigField(void)
{
}
