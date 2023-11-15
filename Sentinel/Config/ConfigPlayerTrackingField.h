#pragma once
#include "ConfigField.h"

class CConfigPlayerTrackingField :
	public CConfigField
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	CConfigPlayerTrackingField(void);

	/// <summary>
	/// Destructor.
	/// </summary>
	~CConfigPlayerTrackingField(void);

	/// <summary>
	/// Sets the multiplier and divisor for player tracking points.
	/// </summary>
	/// <param name="buffer">IN - address of source buffer. If buffer is NULL then a length of zero is used.</param>
	/// <param name="length">IN - size of source buffer. If length is negative then a length of zero is used.</param>
	void SetPlayerTrackingOptions(const byte * buffer, int length);

	/// <summary>
	/// Returns the player tracking points multiplier.
	/// </summary>
	/// <returns>the player tracking points multiplier.</returns>
	WORD GetPlayerTrackingMultiplier() const;

	/// <summary>
	/// Returns the player tracking points divisor.
	/// </summary>
	/// <returns>the player tracking points divisor.</returns>
	WORD GetPlayerTrackingDivisor() const;

private:
	WORD m_playerTrackingMultiplier;
	WORD m_playerTrackingDivisor;
};


