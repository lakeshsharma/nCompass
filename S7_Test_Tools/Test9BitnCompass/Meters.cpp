#include "stdafx.h"

#include "math.h"
#include "Meters.h"
#ifdef LOGGING
#include "Test9BitnCompass.h"
#endif

#ifdef COUNT_METERS_OBJECTS
int CMeters::m_metersObjectCount = 0;
#endif

CMeters::CMeters(MEMORY_TYPE memoryType, bool bMakeInvalidZero)
{
	m_type = memoryType;

	m_bMakeInvalidZero = bMakeInvalidZero; 

	assert(m_metersArray.size() == 0);

#ifdef COUNT_METERS_OBJECTS
	m_metersObjectCount++;
#ifdef LOGGING
	LogString( L"GENERAL", L"Number of meters object instantiated: %d", m_metersObjectCount );
#endif
#endif
}

CMeters::CMeters(CMeters &metersRHS)
{
	*this = metersRHS;
}

CMeters::~CMeters(void)
{
	ClearAllMeters();

#ifdef COUNT_METERS_OBJECTS
	m_metersObjectCount--;
#endif
}

CMeters & CMeters::operator=( CMeters &metersRHS )
{
	m_type = MEMORY_NONE;

	ClearAllMeters();

	for ( int mi = 0; mi < metersRHS.GetMeterCount(); mi++ )
	{
		CMeter *meter = metersRHS.GetMeterByIndex( mi );
		SetMeter( meter->GetMeterNumber(), meter->GetValue() );
	}

	return *this;
}


void CMeters::ClearAllMeters( void )
{
	for ( int mi = 0; mi < m_metersArray.size(); mi++ )
	{
		delete ((CMeter *)m_metersArray[mi]);
	}
	m_metersArray.clear();
}

void CMeters::SetMeter(byte meterNumber, __int64 meterValue, bool bIsEGMMeter, bool bWriteToPermStore )
{
	UNREFERENCED_PARAMETER(bIsEGMMeter);
	UNREFERENCED_PARAMETER(bWriteToPermStore);
	assert ( meterNumber < mMeterLast );

	// If the meter value passed in is INVALID_METER_VALUE, see if we need to convert the value to zero.
	if ( m_bMakeInvalidZero && meterValue == INVALID_METER_VALUE )
	{
		meterValue = 0;
	}

	bool bMeterFoundOrInserted = false;
	for ( int mi = 0; mi < m_metersArray.size() && !bMeterFoundOrInserted; mi++ )
	{
		if ( meterNumber < ((CMeter *)m_metersArray[mi])->GetMeterNumber() )
		{
			CMeter *meter = new CMeter( meterNumber, meterValue );
			m_metersArray.assign(mi, meter);
			bMeterFoundOrInserted = true;
		}
		else if ( meterNumber == ((CMeter *)m_metersArray[mi])->GetMeterNumber() )
		{
			// Don't override a good value with an invalid one.
			if ( meterValue != INVALID_METER_VALUE )
				((CMeter *)m_metersArray[mi])->SetValue( meterValue );
			bMeterFoundOrInserted = true;
		}
	}

	// Add this meter only if we did not find it or we inserted it.
	if ( !bMeterFoundOrInserted )
	{
		CMeter *meter = new CMeter( meterNumber, meterValue );
#ifdef _DEBUG
		DWORD tmpMeter = (DWORD)meter;
		assert(tmpMeter % 4 == 0);
#endif // _DEBUG.
		m_metersArray.push_back(meter);
	}
}


CMeter * CMeters::GetMeterByIndex( int meterIndex )
{
	CMeter *retVal = NULL;

	if ( meterIndex < m_metersArray.size() )
		retVal = (CMeter *)m_metersArray[meterIndex];

#ifdef _DEBUG
	DWORD tmpMeter = (DWORD)retVal;
	assert(tmpMeter % 4 == 0);
#endif // _DEBUG.

	return retVal;
}

CMeter * CMeters::GetMeterByNumber( byte meterNumber )
{
	CMeter *retVal = NULL;

	bool bMeterFound = false;
	for ( int mi = 0; mi < m_metersArray.size() && !bMeterFound; mi++ )
	{
		if ( ((CMeter *)m_metersArray[mi])->GetMeterNumber() == meterNumber )
		{
			retVal = (CMeter *)m_metersArray[mi];
			bMeterFound = true;
		}
	}

#ifdef _DEBUG
	DWORD tmpMeter = (DWORD)retVal;
	assert(tmpMeter % 4 == 0);
#endif // _DEBUG.

	return retVal;
}





