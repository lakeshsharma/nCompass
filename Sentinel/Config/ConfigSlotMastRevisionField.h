#pragma once
#include "ConfigField.h"

class CConfigSlotMastRevisionField :
	public CConfigField
{
public:
	/// <summary>
	/// Constructor for a CConfigSlotMastRevisionField instance.
	/// </summary>
	CConfigSlotMastRevisionField(void);

	/// <summary>
	/// Destructor for a CConfigSlotMastRevisionField instance.
	/// </summary>
	~CConfigSlotMastRevisionField(void);

	/// <summary>
	/// Sets the Slot Master Revision.
	/// </summary>
	/// <param name="buffer">IN - address of source buffer. If buffer is NULL then a length of zero is used.</param>
	/// <param name="length">IN - size of source buffer. If length is negative then a length of zero is used.</param>
	void SetSlotMastRevision(const byte * buffer, int length);

	/// <summary>
	/// Returns the Slot Master Revision.
	/// </summary>
	/// <returns>The Slot Master Revision.</returns>
	WORD GetSlotMastRevision() const;

private:
	WORD m_SlotmastRevision;
};


