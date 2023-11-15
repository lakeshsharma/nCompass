
#pragma once
#include "stdafx.h"

class ICumulativeProgressiveManager
{
public:
	/// <summary>
	/// Determines whether has cumulative meter already.
	/// </summary>
	/// <returns>true if we have set the meter value at least once.</returns>
	virtual bool HasCumulativeMeterAlready() = 0;
	
	/// <summary>
	/// Clears the has cumulative meter status. This is used when an account
	/// only progressive was configured, but then reconfigured to not be accounting only.
	/// </summary>
	virtual void ClearHasCumulativeMeterStatus() = 0;

	/// <summary>
	/// Gets the cumulative delta.
	/// </summary>
	/// <param name="">The .</param>
	/// <returns>The delta between the passed in meter and the stored meter and then stores the passed in meter</returns>
	virtual __int64 GetCumulativeDelta(__int64) = 0;
};

