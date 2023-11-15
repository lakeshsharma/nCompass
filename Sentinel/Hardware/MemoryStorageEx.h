#pragma once
#include "Hardware/MemoryStorage.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class EGMAward; }
namespace SentinelNativeUnitTests { class EGMAwardManager; }
namespace SentinelNativeUnitTests { class MemoryStorageEx; }
namespace SentinelNativeUnitTests { class MemoryStorageEx2; }

class CMemoryStorageEx :
	public CMemoryStorage
{
public:
	static const BYTE NVRAM_DEFAULT_ACTIVE_VALUE = 0; // Should only be used with MEMORY_NONE.

	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CMemoryStorageEx"/> class.
	/// </summary>
	CMemoryStorageEx();

	/// <summary>
	/// Memory type constructor. Initializes a new instance of the <see cref="CMemoryStorageEx"/> class.
	/// </summary>
	/// <param name="memoryType">IN - Type of the memory.</param>
	/// <param name="dataOffset">IN - The NVRAM data offset.</param>
	/// <param name="dataSize">IN - The size of the NVRAM data record.</param>
	/// <param name="activeOffset">IN - The active (location) offset.</param>
	/// <param name="activeValue">IN - The active (location) value. ~activeValue is the inactive (location) value.</param>
	CMemoryStorageEx(MEMORY_TYPE memoryType, DWORD dataOffset, DWORD dataSize, DWORD activeOffset, BYTE activeValue);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CMemoryStorageEx"/> class.
	/// </summary>
	virtual ~CMemoryStorageEx();

	/// <summary>
	/// Sets the memory storage parameters.
	/// </summary>
	/// <param name="memoryType">IN - Type of the memory.</param>
	/// <param name="dataOffset">IN - The NVRAM data offset.</param>
	/// <param name="dataSize">IN - The size of the NVRAM data record.</param>
	/// <param name="activeOffset">IN - The active (location) offset.</param>
	/// <param name="activeValue">IN - The active (location) value. ~activeValue is the inactive (location) value.</param>
	void SetMemoryStorageParameters(MEMORY_TYPE memoryType,
		DWORD dataOffset,
		DWORD dataSize,
		DWORD activeOffset,
		BYTE activeValue);

	/// <summary>
	/// Clears the memory storage parameters.
	/// </summary>
	void ClearMemoryStorageParameters();

	/// <summary>
	/// Gets the NVRAM active offset memory storage parameter.
	/// </summary>
	DWORD GetNvramActiveOffset();

	/// <summary>
	/// Builds the instance from NVRAM.
	/// </summary>
	/// <param name="buildInactive">IN - true to build from both active and inactive records, false to build from only active records.</param>
	/// <remarks>
	/// This method does not attempt to build the instance from NVRAM unless the following are all true:
	///     m_type == MEMORY_NVRAM,
	///     m_bOffsetIsSet == true,
	///     IntegrityCheck() == true, and
	///     NVRAM[m_activeOffset] == m_activeValue
	/// If the active location is active and the integrity check fails then the active location is set inactive.
	/// </remarks>
	virtual void BuildYourself();

	/// <summary>
	/// Builds the instance from NVRAM.
	/// </summary>
	/// <param name="buildInactive">IN - true to build from both active and inactive records, false to build from only active records.</param>
	/// <remarks>
	/// This method does not attempt to build the instance from NVRAM unless the following are all true:
	///     m_type == MEMORY_NVRAM,
	///     m_bOffsetIsSet == true,
	///     IntegrityCheck() == true, and
	///     (NVRAM[m_activeOffset] == m_activeValue || buildInactive)
	/// If the active location is active and the integrity check fails then the active location is set inactive.
	/// </remarks>
	virtual void BuildYourself(bool buildInactive);

	/// <summary>
	/// Stores the instance to NVRAM.
	/// </summary>
	virtual void StoreYourself();

	/// <summary>
	/// Updates the instance from an active NVRAM buffer.
	/// </summary>
	/// <param name="buffer">IN - The buffer.</param>
	/// <param name="bufferSize">IN - The size of the buffer.</param>
	/// <remarks>
	/// Derived (child) classes are expected to implement one of the SetFromNvramBuffer() methods.
	/// </remarks>
	virtual void SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize);

	/// <summary>
	/// Updates the instance from an NVRAM buffer.
	/// </summary>
	/// <param name="buffer">IN - The buffer.</param>
	/// <param name="bufferSize">IN - The size of the buffer.</param>
	/// <param name="active">IN - true if the buffer location is marked as active, false otherwise.</param>
	/// <remarks>
	/// Derived (child) classes are expected to implement one of the SetFromNvramBuffer() methods.
	/// </remarks>
	virtual void SetFromNvramBuffer(const BYTE *buffer, UINT bufferSize, bool active);

	/// <summary>
	/// Gets the NVRAM buffer for the instance.
	/// </summary>
	/// <param name="buffer">OUT - The buffer.</param>
	/// <param name="bufferSize">IN - The size of the buffer.</param>
	/// <param name="active">OUT - true to indicate the the record is active, false to indicate inactive.</param>
	/// <returns>The size of the buffer actually used.</returns>
	virtual UINT GetNvramBuffer(BYTE *buffer, UINT bufferSize, bool& active) const = 0;

	/// <summary>
	/// Sets the check byte location to indicate whether there is an active record.
	/// </summary>
	/// <param name="active">IN - true to indicate that the record is active, false to indicate that the record is inactive.</param>
	virtual void SetActiveLocation(bool active);

protected:
	DWORD m_activeOffset;
	BYTE m_activeValue;
	bool m_activeOffsetIsSet; // m_activeValue is also set. 

	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::EGMAward;
	friend class SentinelNativeUnitTests::EGMAwardManager;
	friend class SentinelNativeUnitTests::MemoryStorageEx;
	friend class SentinelNativeUnitTests::MemoryStorageEx2;

	FRIEND_TEST(MemoryStorageEx, SetMemoryStorageParameters_Default);
	FRIEND_TEST(MemoryStorageEx, SetMemoryStorageParameters_CMemoryStorageEx);
	FRIEND_TEST(MemoryStorageEx, SetMemoryStorageParameters);
	FRIEND_TEST(MemoryStorageEx, ClearMemoryStorageParameters);
	FRIEND_TEST(MemoryStorageEx, GetNvramActiveOffset_ClearMemoryStorageParameters);

	FRIEND_TEST(MemoryStorageEx2, SetFromNvramBuffer3_BuildYourself_NvramInvalidTest);
	FRIEND_TEST(MemoryStorageEx2, SetFromNvramBuffer3_BuildYourselfFalse_NvramInvalidTest);
	FRIEND_TEST(MemoryStorageEx2, SetFromNvramBuffer3_BuildYourselfTrue_NvramInvalidTest);
};
