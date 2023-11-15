#pragma once

#include "Hardware/MemoryStorage.h"
#include "Meter.h"
#include "InternalMsgContent.h"

class CMeters :public CMemoryStorage, public CInternalMsgContent<INT_MSG_CONTENT_METERS>
{
public:
	/// <summary>
	/// Standard constructor.
	/// </summary>
	/// <param name="memoryType">IN - Type of memory used for persistent storage.</param>
	/// <param name="bMakeInvalidZero">IN - true to use zero instead of INVALID_METER_VALUE for SetMeter() calls,
	/// false to use the meterValue provided.</param>
	CMeters(MEMORY_TYPE memoryType, bool bMakeInvalidZero = false );

	/// <summary>
	/// "Copy" constructor.
	/// </summary>
	/// <param name="metersRHS">IN - The source for the copy construction.</param>
	/// <remarks>
	/// Sets memory type (m_type) to MEMORY_NONE.
	/// Sets m_bMakeInvalidZero from the source for the copy.
	/// </remarks>
	CMeters( CMeters &metersRHS );

	/// <summary>
	/// Destructor.
	/// </summary>
	~CMeters(void);

	/// <summary>
	/// Assignment operator.
	/// </summary>
	/// <param name="metersRHS">IN - The source for the assignment.</param>
	/// <returns>A reference to the target of the assigment.</returns>
	/// <remarks>
	/// Sets memory type (m_type) to MEMORY_NONE.
	/// Sets m_bMakeInvalidZero from the source for the assignment.
	/// </remarks>
	CMeters &operator=(const CMeters &metersRHS);  

public:
	/// <summary>
	/// Sets the meter value for the specified meter number.
	/// </summary>
	/// <param name="meterNumber">IN - The meter number to set.</param>
	/// <param name="meterValue">IN - The meter value to set.</param>
	/// <param name="bIsEGMMeter">IN - true if the meter is an EGM base game meter, false for a sub-game meter.</param>
	/// <param name="bWriteToPermStore">IN - true to write to NVRAM if m_type==MEMORY_NVRAM, otherwise skip write to NVRAM.</param>
	/// <remarks>
	/// If meterValue==INVALID_METER_VALUE and m_bMakeInvalidZero==true then zero is used for the meter value.
	/// </remarks>
	void SetMeter( byte meterNumber, __int64 meterValue = INVALID_METER_VALUE, bool bIsEGMMeter = true,  bool bWriteToPermStore = true );

    /// <summary> 
    /// Writes all the meter values to NVRAM if this object is set 
    /// to do so. 
	/// </summary>
	/// <param name="bIsEGMMeters">IN - Boolean flag to indicate if these are EGM meters (used to calculate meter offsets).</param>
	/// <remarks> This function will write meters 4 at a time. 
	/// </remarks>
    void PersistMeters(bool bIsEGMMeters);

	void AddToMeter( byte meterNumber, __int64 additionalMeterValue, bool bIsEGMMeter = true,  bool bWriteToPermStore = true ); 
	void ClearAllMeters( void );
	void SetMemoryType(MEMORY_TYPE memType);

	/// <summary>
	/// Sets all meter values to INVALID_METER_VALUE.
	/// </summary>
	void SetAllMetersInvalid();

	/// <summary>
	/// Removes meterNumber meter from collection without deleting it.
	/// </summary>
	/// <param name="meterNumber">The meter number.</param>
	/// <returns>meterNumber meter or NULL if not found.</returns>
	const CMeter *RemoveMeterByNumber(byte meterNumber);

	/// <summary>
	/// Returns the number of meters.
	/// </summary>
	/// <returns>The number of meters.</returns>
	int GetMeterCount( void ) const
	{
        return m_metersArray.size();
	}

	void RemoveFromPermanentStore()
	{
		SetInactiveLocation(m_dwOffset);
	}

	__int64 CompareMeter( CMeter *meter );

	/// <summary>
	/// Returns a pointer to the CMeter instance corresponding to meterIndex, so the caller can modify the meter.
	/// </summary>
	/// <param name="meterIndex">IN - Index of the meter.</param>
	/// <returns>A pointer to the CMeter instance corresponding to meterIndex.</returns>
	const CMeter * GetMeterByIndex( int meterIndex ) const;
	CMeter * GetMeterByIndex( int meterIndex );

	/// <summary>
	/// Returns a pointer to the meter instance for meterNumber.
	/// The pointer may be NULL.
	/// </summary>
	/// <param name="meterNumber">The meter number.</param>
	/// <returns>A pointer to the meter instance for meterNumber. The pointer may be NULL.</returns>
	CMeter * GetMeterByNumber( byte meterNumber ) const;
	//CMeter * GetMeterByNumber( byte meterNumber );

	virtual void BuildYourself();
	void SetOffset(DWORD dwOffset);
	void InitialWriteToPermanentStore(bool bIsEGMMeter);
	void SetAllowedSize(DWORD dwAllowedSize);

	void LogMeters();
	// Tracker #20839
	CMeters *DuplicateChangedMeters(bool gameInProgress = false);

	void TrimToNonZeroValidMeters();

	/// <summary>
	/// Help function to set up this meter object with oasis bill meters
	/// </summary>
	/// <param name="oasisMeterNumber">The oasis meter number to be set. mNone indicates all bill meters are to be set</param>
	void SetBillMeters(int oasisMeterNumber);
	static const vector<oasisMeters> ignoreDuringGameProgress;

private:
	// This indicates if we are adding a meter with an invalid, set it to zero
	bool m_bMakeInvalidZero;

    std::list<CMeter *> m_metersArray;	
    
	static bool CompareMeters(CMeter *meter1, CMeter *meter2);
};


