#pragma once

#include "stdafx.h"
//#include "hardware/memorystorage.h"
#include "Meter.h"
//#include "InternalMsgContent.h"

//#define COUNT_METERS_OBJECTS

typedef	enum
{
	MEMORY_NONE,
	MEMORY_EEPROM,
	MEMORY_NVRAM
} MEMORY_TYPE;

class CMeters
{
public:
	CMeters(MEMORY_TYPE memoryType, bool bMakeInvalidZero = false );
	CMeters( CMeters &metersRHS );
public:
	~CMeters(void);

	CMeters &operator=( CMeters &metersRHS );  

public:
	void SetMeter( byte meterNumber, __int64 meterValue = INVALID_METER_VALUE, bool bIsEGMMeter = true,  bool bWriteToPermStore = true );
	void ClearAllMeters( void );

	int GetMeterCount( void )
	{
		return m_metersArray.size();
	}

	CMeter * GetMeterByIndex( int meterIndex );
	CMeter * GetMeterByNumber( byte meterNumber );


private:
	// This indicates if we are adding a meter with an invalid, set it to zero
	bool m_bMakeInvalidZero;
	MEMORY_TYPE	m_type;

	std::vector<void *>  m_metersArray;

#ifdef COUNT_METERS_OBJECTS
	static int m_metersObjectCount;
#endif


};
