#pragma once

#include "ICumulativeProgressiveManager.h"
#include "Hardware/MemoryStorageEx.h"
#include "Logging/LogString.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class CumulativeProgressiveManagerTests; }
namespace SentinelNativeUnitTests { class EGMAwardManager; }

class CumulativeProgressiveManager : public ICumulativeProgressiveManager,
	public CMemoryStorageEx
{
public:
	static const DWORD NVRAM_ACTIVE_OFFSET;
	static const BYTE NVRAM_ACTIVE_VALUE;
	static const DWORD NVRAM_DATA_OFFSET;
	static const DWORD NVRAM_DATA_SIZE;

	/// <summary>
	/// Initializes a new instance of the <see cref="CumulativeProgressiveManager"/> class.
	/// </summary>
	/// <param name="memoryType">IN - Type of the memory.</param>
	CumulativeProgressiveManager(MEMORY_TYPE memoryType = MEMORY_NONE);

	/// <summary>
	/// Updates the instance from an NVRAM buffer.
	/// </summary>
	/// <param name="buffer">IN - The buffer.</param>
	/// <param name="bufferSize">IN - The size of the buffer.</param>
	void SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize);

	/// <summary>
	/// Gets the NVRAM buffer for the instance.
	/// </summary>
	/// <param name="buffer">OUT - The buffer.</param>
	/// <param name="bufferSize">IN - The size of the buffer.</param>
	/// <param name="active">OUT - true to indicate the the record is active, false to indicate inactive.</param>
	/// <returns>The size of the buffer actually used.</returns>
	UINT GetNvramBuffer(BYTE *buffer, UINT bufferSize, bool &active) const;

	/// <summary>
	/// Determines whether has cumulative meter already.
	/// </summary>
	/// <returns>true if we have set the meter value at least once.</returns>
	bool HasCumulativeMeterAlready();

	/// <summary>
	/// Clears the has cumulative meter status. This is used when an account
	/// only progressive was configured, but then reconfigured to not be accounting only.
	/// </summary>
	void ClearHasCumulativeMeterStatus();

	/// <summary>
	/// Gets the cumulative delta.
	/// </summary>
	/// <param name="">The .</param>
	/// <returns>The delta between the passed in meter and the stored meter</returns>
	__int64 GetCumulativeDelta(__int64);

private:
	__int64 m_currentCumulativeMeterValue;
	bool m_hasCumulativeMeterAlready;

	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::CumulativeProgressiveManagerTests;
	friend class SentinelNativeUnitTests::EGMAwardManager;
};

