#pragma once
#include "ConfigField.h"

class CConfigPointsForPromoField :
	public CConfigField
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CConfigPointsForPromoField(void);

	/// <summary>
	/// Destructor.
	/// </summary>
	~CConfigPointsForPromoField(void);

	/// <summary>
	/// Sets whether points and comp accruals are enabled for NCEP credits played.
	/// </summary>
	/// <param name="buffer">IN - address of source buffer. If buffer is NULL then a length of zero is used.</param>
	/// <param name="length">IN - size of source buffer. If length is negative then a length of zero is used.</param>
	void SetAccruePointsForPromo(const byte * buffer, int length);

	/// <summary>
	/// Returns true if Accrue Points For NCEP is enabled, false otherwise.
	/// </summary>
	/// <returns>true if Accrue Points For NCEP is enabled, false otherwise.</returns>
	BOOL GetAccruePointsForNCEP() const;

	/// <summary>
	/// Returns true if Accrue Comp For NCEP is enabled, false otherwise.
	/// </summary>
	/// <returns>true if Accrue Comp For NCEP is enabled, false otherwise.</returns>
	BOOL GetAccrueCompForNCEP() const;

private:
	byte m_accruePointsForNCEP;
	byte m_accrueCompForNCEP;
};


