#pragma once
#include "ConfigField.h"
#include "Utilities.h"

class CConfigPointsPerDollarField :
	public CConfigField
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CConfigPointsPerDollarField(void);

	/// <summary>
	/// Destructor.
	/// </summary>
	~CConfigPointsPerDollarField(void);

	/// <summary>
	/// Sets the points per dollar.
	/// </summary>
	/// <param name="pointsPerDollar">IN - address of source buffer. If pointsPerDollar is NULL then a length of zero is used.</param>
	/// <param name="length">IN - size of source buffer. If length is negative then a length of zero is used.</param>
	void SetPointsPerDollar(const byte * pointsPerDollar, int length);

	/// <summary>
	/// Returns the points per dollar.
	/// </summary>
	/// <returns>The points per dollar.</returns>
	WORD GetPointsPerDollar() const;

private:
	WORD m_pointsPerDollar;
};


