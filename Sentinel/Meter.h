#pragma once


#include "metersdef.h"

class CMeter
{
public:
	/// <summary>
	/// Meter number constructor.
	/// </summary>
	CMeter(byte meterNumber);

	/// <summary>
	/// Meter value constructor.
	/// </summary>
	CMeter(byte meterNumber, __int64 meterValue);

	/// <summary>
	/// Destructor.
	/// </summary>
	virtual ~CMeter(void);

	/// <summary>
	/// Sets the meter value.
	/// </summary>
	/// <param name="value">IN - the new meter value.</param>
	void SetValue( __int64 value );

	/// <summary>
	/// Returns the meter value.
	/// </summary>
	/// <returns>The meter value.</returns>
	__int64 GetValue( void ) const;

	/// <summary>
	/// Sets the meter number.
	/// </summary>
	/// <param name="meterNumber">IN - the new meter number value.</param>
	/// <remarks>
	/// The normal range for meterNumber is mNone to mMeterLast - 1.
	/// </remarks>
	void SetMeterNumber( byte meterNumber );

	/// <summary>
	/// Returns the meter number.
	/// </summary>
	/// <returns>The meter number.</returns>
	byte GetMeterNumber( void ) const;

	/// <summary>
	/// Sets the meter changed status to true.
	/// </summary>
	void SetMeterChanged();

	/// <summary>
	/// Returns true if the meter changed status is true, false otherwise.
	/// </summary>
	/// <returns>true if the meter has changed, false otherwise.</returns>
	bool IsMeterChanged() const;

private:
	byte m_meterNumber;
	__int64 m_meterValue;

	// Tracker #20839
	bool m_meterChanged; // used to determine which meters to send to SCF when processesing meters.

};


