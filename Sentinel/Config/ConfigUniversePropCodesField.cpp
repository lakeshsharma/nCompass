#include "stdafx.h"
#include "ConfigUniversePropCodesField.h"

#include "Utilities.h"

using namespace std;

// CConfigUniversePropCodesField::CConfigUniversePropCodesField.
// Constructor for a CConfigUniversePropCodesField instance.
CConfigUniversePropCodesField::CConfigUniversePropCodesField(void) :
	CConfigField(UNIVERSE_PROP_CODES)
{
}

// CConfigUniversePropCodesField::~CConfigUniversePropCodesField.
// Destructor for a CConfigUniversePropCodesField instance.
CConfigUniversePropCodesField::~CConfigUniversePropCodesField(void)
{
}

// CConfigUniversePropCodesField::SetUniversePropCodes.
//     IN buffer - pointer to data to be used for Universe Penn property codes.
//         If buffer is NULL then a length of zero is used.
//     IN size - size in bytes of data buffer to be used for setting Universe Penn property codes.
//         If size is negative then a size of zero is used.
// Set the Universe Penn property codes, using length bytes of buffer.
void CConfigUniversePropCodesField::SetUniversePropCodes(const byte* propCodes, int size)
{
    int numPropCodes = 0;
	if (propCodes != NULL)
	{
        numPropCodes = size / SIZE_OF_VALID_PROP_CODE;
    }

    for (int propCodeIndex = 0; propCodeIndex < numPropCodes; propCodeIndex++) 
    {
        BYTE propCodeHighByte = *propCodes++;
        BYTE propCodeLowByte = *propCodes++;
        WORD universePropCode = (WORD)(propCodeHighByte * 100) + (WORD)propCodeLowByte;
        m_universePropCodes.push_back(universePropCode);
    }
}

// CConfigUniversePropCodesField::GetUniversePropCodes.
//     RETURN - the pointer to the Universe Penn property codes stored.
//         - Only the first GetUniversePropCodeCount() elements are valid.
const std::vector<WORD> CConfigUniversePropCodesField::GetUniversePropCodes() const
{
	return m_universePropCodes;
}


