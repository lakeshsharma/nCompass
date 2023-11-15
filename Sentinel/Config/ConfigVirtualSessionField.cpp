#include "stdafx.h"
#include "ConfigVirtualSessionField.h"
#include "GlobalDefs.h"

// CConfigVirtualSessionField::CConfigVirtualSessionField.
// Constructor for a CConfigVirtualSessionField instance.
CConfigVirtualSessionField::CConfigVirtualSessionField(void) :
	CConfigField(VIRTUAL_SESSIONS),
	m_virtualSessionNoPlayTimeout(0),
	m_virtualSessionMaxDuration(0)
{
}

// CConfigVirtualSessionField::~CConfigVirtualSessionField.
// Destructor for a CConfigVirtualSessionField instance.
CConfigVirtualSessionField::~CConfigVirtualSessionField(void)
{
}

// CConfigVirtualSessionField::SetVirtualSessionOptions.
//     IN buffer - pointer to data to be used for setting virtual session options.
//         If buffer is NULL then the instance is not changed.
//     IN length - length in bytes of data buffer to be used for setting virtual session options.
// Set virtual session options using length of buffer.
void CConfigVirtualSessionField::SetVirtualSessionOptions(const byte * buffer, int length)
{
	if (buffer != NULL)
	{
		int offset = 0;
		length = max(0, length); // Fix signed/unsigned comparision in next "if" expression.
		if (length >= (int)sizeof(m_virtualSessionNoPlayTimeout) + (int)sizeof(m_virtualSessionMaxDuration))
		{
			memcpy_s(&m_virtualSessionNoPlayTimeout, sizeof(m_virtualSessionNoPlayTimeout), buffer + offset, sizeof(m_virtualSessionNoPlayTimeout));
			offset+= sizeof(m_virtualSessionNoPlayTimeout);
			memcpy_s(&m_virtualSessionMaxDuration, sizeof(m_virtualSessionMaxDuration), buffer + offset, sizeof(m_virtualSessionMaxDuration));
		}
		else
		{
			//TBD log error
		}
	}
}

// CConfigVirtualSessionField::GetVirtualSessionNoPlayTimeout.
//     RETURN - the virtual session timeout period for no play.
DWORD CConfigVirtualSessionField::GetVirtualSessionNoPlayTimeout() const
{
	return m_virtualSessionNoPlayTimeout;
}

// CConfigVirtualSessionField::GetVirtualSessionMaxDuration.
//     RETURN - the maximum duration for a virtual session.
DWORD CConfigVirtualSessionField::GetVirtualSessionMaxDuration() const
{
	return m_virtualSessionMaxDuration;
}


