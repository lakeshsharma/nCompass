#pragma once
#include "ConfigField.h"

class CConfigPBTField :
	public CConfigField
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CConfigPBTField(void);

	/// <summary>
	/// Destructor.
	/// </summary>
	~CConfigPBTField(void);

	/// <summary>
	/// Set PBT options.
	/// </summary>
	/// <param name="buffer">IN - address of the PBT options.</param>
	/// <param name="length">IN - number of bytes in buffer.</param>
	void SetPBTOptions(const byte * buffer, int length);

	/// <summary>
	/// This will create the buffer for the given flags.
	/// </summary>
	/// <param name="buffer">OUT - address of the buffer for the given flags.</param>
	/// <param name="length">IN - number of bytes in buffer.</param>
	/// <param name="bPBTEnabled">IN - true if PBT transfers are enabled, false otherwise.</param>
	/// <param name="bPointsEnabled">IN - true if point transfers are enabled, false otherwise.</param>
	/// <param name="bCashEnabled">IN - true if cash transfers are enabled, false otherwise.</param>
	static void SetPBTOptionsBuffer( byte *buffer, int length, bool bPBTEnabled, bool bPointsEnabled, bool bCashEnabled );

	/// <summary>
	/// Returns true if Promo transfers are enabled, false otherwise.
	/// </summary>
	/// <returns>true if PromoEnabled transfers are enabled, false otherwise.</returns>
	bool PromoEnabled() const;

	/// <summary>
	/// Returns true if point transfers are enabled, false otherwise.
	/// </summary>
	/// <returns>true if point transfers are enabled, false otherwise.</returns>
	bool PointsEnabled() const;

	/// <summary>
	/// Returns true if cash transfers are enabled, false otherwise.
	/// </summary>
	/// <returns>true if cash transfers are enabled, false otherwise.</returns>
	bool CashEnabled() const;

private:
	bool m_PromoEnabled;
	bool m_PointsEnabled;
	bool m_CashEnabled;
};


