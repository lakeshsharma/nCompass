#include "stdafx.h"

#include "Meters.h"

#ifdef LOGGING
#include "Logging/LogString.h"
#endif


using namespace std;

//MGMD specific; These meters should not go to FL during game in progress.
const vector<oasisMeters> CMeters::ignoreDuringGameProgress = { mCrd, gCI, gCO, mBonusAtt, mBonusMach, gJP, mPrgPayMac, mPrgPayAtt };

CMeters::CMeters(MEMORY_TYPE memoryType, bool bMakeInvalidZero)
{
	m_type = memoryType;

	m_bMakeInvalidZero = bMakeInvalidZero; 
}

CMeters::CMeters(CMeters &metersRHS)
{
	*this = metersRHS;
}

CMeters::~CMeters(void)
{
	ClearAllMeters();
}

CMeters & CMeters::operator=(const CMeters &metersRHS )
{
	m_type = MEMORY_NONE;
	m_bMakeInvalidZero = metersRHS.m_bMakeInvalidZero;

	ClearAllMeters();

	for ( int mi = 0; mi < metersRHS.GetMeterCount(); mi++ )
	{
		const CMeter *meter = metersRHS.GetMeterByIndex( mi );
		SetMeter( meter->GetMeterNumber(), meter->GetValue() );
	}

	return *this;
}

void CMeters::SetMemoryType(MEMORY_TYPE memType)
{
	//N.B This can only be done before anything is added.
	//otherwise, InitialWriteToPermanentStore must be called
	m_type = memType;
}

void CMeters::ClearAllMeters( void )
{
    
	for (std::list<CMeter*>::iterator it = m_metersArray.begin(); it != m_metersArray.end(); ++it)
	{
		delete *it;
	}
	
    m_metersArray.clear();
}

void CMeters::SetAllMetersInvalid()
{
	for (std::list<CMeter*>::iterator it = m_metersArray.begin(); it != m_metersArray.end(); ++it)
	{
        (*it)->SetValue(INVALID_METER_VALUE);
	}
}

void CMeters::SetMeter(byte meterNumber, __int64 meterValue, bool bIsEGMMeter, bool bWriteToPermStore )
{
	// If the meter value passed in is INVALID_METER_VALUE, see if we need to convert the value to zero.
	if ( m_bMakeInvalidZero && meterValue == INVALID_METER_VALUE )
	{
		meterValue = 0;
	}

    CMeter* meter = GetMeterByNumber(meterNumber);
	
    if (meter != nullptr)
	{
		 if (meterValue != INVALID_METER_VALUE)
		 {
			 // Only update value if new value isn't invalid
			 meter->SetValue(meterValue);
		 }
	}
	else
	{
        m_metersArray.push_back(new CMeter( meterNumber, meterValue ));
		m_metersArray.sort(CompareMeters);
	}

	//Set in Permanent Storage
	if (bWriteToPermStore && (m_type == MEMORY_NVRAM))
	{
		//beginning of meters
		//base address + active byte + count of meters
		DWORD dwMeterOffset = m_dwOffset + NVRAM_LOCATION_IS_ACTIVE	+ sizeof(DWORD);
		
		//multiply up to the meter location
		if (bIsEGMMeter)
		{
			dwMeterOffset+= (meterNumber - 1) * NVRAM_METER_SIZE;	
		}
		else
		{
			dwMeterOffset+= (meterNumber) * NVRAM_METER_SIZE;
		}

		BYTE byActive = ACTIVE_LOCATION;
        DWORD dwCount = m_metersArray.size();

		Write(m_dwOffset + NVRAM_LOCATION_IS_ACTIVE, (BYTE*)&dwCount, sizeof(dwCount),
			dwMeterOffset, (BYTE*)&byActive, sizeof(byActive),
			dwMeterOffset + sizeof(byActive), (BYTE*)&meterNumber, sizeof(meterNumber),
			dwMeterOffset + sizeof(byActive) + sizeof(meterNumber), (BYTE*)&meterValue, sizeof(meterValue));


#ifdef LOGGING
		LogString(NVRAM_LOG, "Writing meter %d:%lld to NVRAM at offset [%u]", meterNumber, meterValue, dwMeterOffset);
#endif
	} 
}


void CMeters::PersistMeters(bool bIsEGMMeters)
{
	if (m_type == MEMORY_NVRAM)
    {
        const int NumMetersPerWrite = 4;
        const UINT bufferSize = sizeof(byte) + sizeof(byte) + sizeof(__int64);

        DWORD meterCount = m_metersArray.size();

        // The offset in NVRAM where these meters are written
        DWORD dwMetersStartOffset = m_dwOffset + NVRAM_LOCATION_IS_ACTIVE	+ sizeof(DWORD);

#ifdef LOGGING
		LogString(NVRAM_LOG, "[CMeters::PersistMeters] [bIsEGMMeters: %d] Writing %d meters to NVRAM block "
			"at [%u] offset in 4 meter blocks", bIsEGMMeters, meterCount, dwMetersStartOffset);
#endif

		// Set up the offset and data buffer to write the meters in blocks of 4.
        DWORD meterWriteOffset[NumMetersPerWrite];
        byte meterBuffer[NumMetersPerWrite][bufferSize];

        // This will be the current index of the meter in the 4-position buffer
        int meterBufferIndex = 0;

        BYTE byActive = ACTIVE_LOCATION;

        // Initialize the 4 offset value in case they are not all set
        meterWriteOffset[0] = meterWriteOffset[1] = meterWriteOffset[2] = meterWriteOffset[3] = NVRAM_NULL_VALUE;
                
        // Iterate thru the meters
        for (std::list<CMeter *>::const_iterator meterIter = m_metersArray.begin(); meterIter != m_metersArray.end(); ++meterIter)
        {
            byte meterNumber = (*meterIter)->GetMeterNumber();
            __int64 meterValue = (*meterIter)->GetValue();

            // Set the NVRAM offset for this meter
			if (bIsEGMMeters)
			{
				meterWriteOffset[meterBufferIndex] = dwMetersStartOffset + (meterNumber - 1) * NVRAM_METER_SIZE;
			}
			else
			{
				meterWriteOffset[meterBufferIndex] = dwMetersStartOffset + (meterNumber) * NVRAM_METER_SIZE;
			}
			LogString(NVRAM_LOG, "PersistMeters:: Offset: %u, meterNum: %d, meterValue: %lld", meterWriteOffset[meterBufferIndex], meterNumber, meterValue);

            // Set the NVRAM buffer for this meter
            byte *bufferPtr = meterBuffer[meterBufferIndex++];
            *bufferPtr = byActive;
            bufferPtr += sizeof(byActive);
            *bufferPtr = meterNumber;
            bufferPtr += sizeof(meterNumber);
            *(reinterpret_cast<__int64 *>(bufferPtr)) = meterValue;

			// If we have 4 meters in our buffer, write them
            if (meterBufferIndex == NumMetersPerWrite) 
            {
				Write(meterWriteOffset[0],  meterBuffer[0], bufferSize, meterWriteOffset[1],  meterBuffer[1], bufferSize, meterWriteOffset[2],  meterBuffer[2], bufferSize, meterWriteOffset[3],  meterBuffer[3], bufferSize);
                meterBufferIndex = 0;
                meterWriteOffset[0] = meterWriteOffset[1] = meterWriteOffset[2] = meterWriteOffset[3] = NVRAM_NULL_VALUE;
            }
        }

		// Write the remaining meters
		Write(meterWriteOffset[0], meterBuffer[0], bufferSize, meterWriteOffset[1], meterBuffer[1], bufferSize, meterWriteOffset[2], meterBuffer[2], bufferSize, meterWriteOffset[3], meterBuffer[3], bufferSize);

		// Write the count of meters
		Write(m_dwOffset + NVRAM_LOCATION_IS_ACTIVE, (BYTE *)&meterCount, sizeof(meterCount));
	}
}

void CMeters::AddToMeter( byte meterNumber, __int64 additionalMeterValue, bool bIsEGMMeter,  bool bWriteToPermStore )
{
	__int64 newMeterValue = additionalMeterValue;

	const CMeter *meter = GetMeterByNumber( meterNumber );

	if ( meter != NULL && meter->GetValue() != INVALID_METER_VALUE )
	{
		newMeterValue += const_cast<CMeter *>(meter)->GetValue();
	}

	SetMeter( meterNumber, newMeterValue, bIsEGMMeter, bWriteToPermStore );
}


const CMeter * CMeters::GetMeterByIndex( int meterIndex ) const
{
    CMeter *retVal = nullptr;

    if ( meterIndex < (int)m_metersArray.size() )
    {
       std::list<CMeter*>::const_iterator it = m_metersArray.begin();
       std::advance(it, meterIndex);

       retVal = *it;
    }

	return retVal;
}

CMeter * CMeters::GetMeterByIndex( int meterIndex )
{
    CMeter *retVal = nullptr;

    if ( meterIndex < (int)m_metersArray.size() )
    {
       std::list<CMeter*>::iterator it = m_metersArray.begin();
       std::advance(it, meterIndex);

       retVal = *it;
    }

	return retVal;
}

CMeter * CMeters::GetMeterByNumber( byte meterNumber ) const
{
	CMeter *retVal = nullptr;

	for (std::list<CMeter *>::const_iterator it = m_metersArray.begin(); it != m_metersArray.end(); ++it)
	{
		if ((*it)->GetMeterNumber() == meterNumber)
		{
			retVal = *it;
			break;
		}
	}

	return retVal;
}

//const CMeter * CMeters::GetMeterByNumber( byte meterNumber ) const
//{
//	// Use the const method to do the work then cast the result.
//	return const_cast<CMeter *>(static_cast<const CMeters &>(*this).GetMeterByNumber(meterNumber));
//}

__int64 CMeters::CompareMeter( CMeter *meter )
{
	__int64 meterValue = INVALID_METER_VALUE;
	__int64 meterDiff = 0;

	const CMeter *ourMeter = GetMeterByNumber( meter->GetMeterNumber() );

	if ( ourMeter != NULL )
		meterValue = ourMeter->GetValue();

	// Tracker #21501 - add code to deal with meter rollover and INVALID_METER_VALUEs.
	if (meter->GetValue() != INVALID_METER_VALUE)
	{
		if (meterValue != INVALID_METER_VALUE)
		{
			meterDiff = meter->GetValue() - meterValue;
			if (meterDiff < 0)
			{
				// meter rolled over, recalc delta
				LogString(GENERAL, _T("Meter rollover detected meter %d."), meter->GetMeterNumber());
				LogString(GENERAL, _T("%s:%u original meter=%lld"),__FUNCTION__, __LINE__, ourMeter->GetValue());
				LogString(GENERAL, _T("%s:%u new meter=%lld"),__FUNCTION__, __LINE__, meter->GetValue());
				LogString(GENERAL, _T("%s:%u meterDiff=%lld"),__FUNCTION__, __LINE__, meterDiff);
				double fNumDigits = log10((double)meterValue);
				double numDigits = ceil(fNumDigits);

				double rollOverValue = pow(10, numDigits);
				__int64 iRollOverValue = (__int64)rollOverValue;
				meterDiff = (iRollOverValue - meterValue) + meter->GetValue();

				// tracker #23807 - if difference is now 0, we know meter changed so it was something like 100 -> 0.
				// recalculate original difference.
				if (meterDiff == 0)
				{
					meterDiff = meter->GetValue() - meterValue;
				}
			}
		}
		else
		{
			// original value is INVALID_METER_VALUE, new meter has a real value
			meterDiff = meter->GetValue() == 0 ? -1 : meter->GetValue();
		}
	}

	return meterDiff;
}

void CMeters::SetOffset(DWORD dwOffset)
{
	m_dwOffset = dwOffset;
	m_bOffsetIsSet = true;
}

void CMeters::BuildYourself()
{
	if (m_type != MEMORY_NONE)
	{
		DWORD dwCurrentOffset = m_dwOffset + NVRAM_LOCATION_IS_ACTIVE;
		DWORD dwMeterCount = 0;

		//Get the number of meters
		Read(dwCurrentOffset, (BYTE*)&dwMeterCount, sizeof(dwMeterCount));
		dwCurrentOffset+= sizeof(dwMeterCount);

		LogString(NVRAM_LOG, "CMeters::BuildYourself - Building meters object from meters starting at offset %u", dwCurrentOffset);

		//If there are any meters:
		if (dwMeterCount)
		{
            LogString( NVRAM_LOG, "CMeters::BuildYourself - There are %d meters", dwMeterCount);
			BYTE meterNumber;
			__int64 meterValue;

			BYTE lastMeterNumber = INVALID_METER_NUMBER;
			for (DWORD i = 0; i < dwMeterCount; i++)
			{
				while (!IsActiveLocation_wrapper(dwCurrentOffset) && (dwCurrentOffset < (m_dwAllowedSize + m_dwOffset)) )
				{
					dwCurrentOffset+= NVRAM_METER_SIZE;
				}

				if (dwCurrentOffset > (m_dwAllowedSize + m_dwOffset))
				{
					LogString( ERROR_LOG, "CMeters::BuildYourself - Attempting to read meters passed the allocation NVRAM bmeters buffer");
					return;
				}

				DWORD offset = dwCurrentOffset;
				dwCurrentOffset+= NVRAM_LOCATION_IS_ACTIVE; //the isActive byte
				Read(dwCurrentOffset, &meterNumber, sizeof(meterNumber));
				dwCurrentOffset+= sizeof(meterNumber);
				
				Read(dwCurrentOffset, (BYTE*)&meterValue, sizeof(meterValue));
				dwCurrentOffset+= sizeof(meterValue);

				if (meterNumber == lastMeterNumber)
				{
					// Duplicate meter entries found at consecutive locations, invalidating 
					// the 2nd one, ignoring this entry as valid meter read, and going to
					// read next meter slot
					LogString(NVRAM_LOG, "CMeters::BuildYourself - Duplicate meter entries found "
						"at consecutive locations, marking location %u as inactive.", offset);
					SetInactiveLocation_wrapper(offset);
					i--;
					continue;
				}
				LogString(NVRAM_LOG, "CMeters::BuildYourself - MeterNumber=%d. MeterValue:%lld, offset=%u", meterNumber, meterValue, offset);

				SetMeter(meterNumber, meterValue, false, false); //don't rewrite it to nvram
				lastMeterNumber = meterNumber;
			}
		}
		else
		{
            LogString( NVRAM_LOG, "CMeters::BuildYourself - There are NO meters in NVRAM");
		}
	}
}

void CMeters::InitialWriteToPermanentStore(bool bIsEGMMeter)
{

	//There should be no meters in the array at this time,
	//But if there are, we'll write them to be safe.
	DWORD dwMeterCount = m_metersArray.size();

	//If there are any meters:
	if (dwMeterCount > 0)
	{
		for (CMeter* meter : m_metersArray)
		{
			// Here bIsEGMMeter flag doesnot matter in SetMeter() call because last argument is false (bWriteToPermStore).
			SetMeter(meter->GetMeterNumber(), meter->GetValue(), bIsEGMMeter, false);
		}
	}

	//set the NVRAM memory active for the entire block of meters and the count of meters in memory
	BYTE byActive = ACTIVE_LOCATION;
	Write(m_dwOffset, (BYTE *)&byActive, sizeof(byActive),
			m_dwOffset + NVRAM_LOCATION_IS_ACTIVE, (BYTE *)&dwMeterCount, sizeof(dwMeterCount));

	// This is the place where bIsEGMMeter flag matters because PersistMeters() will calculate offset based on bIsEGMMeter flag.
	PersistMeters(bIsEGMMeter);
}


void CMeters::SetAllowedSize(DWORD dwAllowedSize)
{
	m_dwAllowedSize = dwAllowedSize;
}

void CMeters::LogMeters()
{
#ifdef LOGGING
	const CMeter *lpMeter;
	byte number;
	__int64 value;

	LogString("GENERAL", "Meters contents");
	for(int n = 0; n < this->GetMeterCount(); n++)
	{
		lpMeter = this->GetMeterByIndex(n);
		number = lpMeter->GetMeterNumber();
		value = lpMeter->GetValue();
		LogString("GENERAL", "Meter %d, %lld",(int)number, (long long)value);
	}
#endif
}

CMeters *CMeters::DuplicateChangedMeters(bool gameInProgress)
{
	// Tracker #20839
	// only copy meters from this meters object that have m_meterChanged set
	CMeters *changedMeters = new CMeters(MEMORY_NONE);
    
    for (std::list<CMeter*>::iterator it = m_metersArray.begin(); it != m_metersArray.end(); ++it)
	{
		//ignore meters which are part of ignoreDuringGameProgress vector
        if ((*it)->IsMeterChanged() && !(gameInProgress && std::find(ignoreDuringGameProgress.begin(), ignoreDuringGameProgress.end(), (*it)->GetMeterNumber()) != ignoreDuringGameProgress.end()))
        {
            changedMeters->SetMeter((*it)->GetMeterNumber(), (*it)->GetValue(), true, false);
        }
	}

	return changedMeters;
}

void CMeters::SetBillMeters(int oasisBillMeterNumber)
{
	switch (oasisBillMeterNumber)
	{
	case mNone:
		SetMeter(mD1);	//{Ones}
		SetMeter(mD2);	//{Twos}
		SetMeter(mD5);	//{Fives}
		SetMeter(mD10);	//{Tens}
		SetMeter(mD20);	//{Twenties}
		SetMeter(mD50);	//{Fifties}
		SetMeter(mD100);	//{Hundreds}
		SetMeter(mD200); //{Two Hundreds}
		SetMeter(mD500); //{Five Hundreds}
		SetMeter(mD1000); //{One Thousands}
		break;
	case(mD1):	//{Ones}
	case(mD2):	//{Twos}
	case(mD5):	//{Fives}
	case(mD10):	//{Tens}
	case(mD20):	//{Twenties}
	case(mD50):	//{Fifties}
	case(mD100):	//{Hundreds}
	case(mD200): //{Two Hundreds}
	case(mD500): //{Five Hundreds}
	case(mD1000): //{One Thousands}
		SetMeter(oasisBillMeterNumber);
		break;
	default:
		// Not a bill meter. Do nothing.
		break;
	}
	SetMeter(mBD);//{Bill In}
}


const CMeter *CMeters::RemoveMeterByNumber(byte meterNumber)
{
    const CMeter *retVal = GetMeterByNumber(meterNumber);
    
    if (retVal != nullptr) 
	{
        m_metersArray.remove(const_cast<CMeter *>(retVal));
	}

	return retVal;
}

bool CMeters::CompareMeters(CMeter *meter1, CMeter *meter2)
{
    return (meter1->GetMeterNumber() < meter2->GetMeterNumber());
}

void CMeters::TrimToNonZeroValidMeters()
{
	size_t sizeBefore = m_metersArray.size();

	// Delete the unwanted CMeter objects and set their pointer location
	// in the list to nullptr so that those can be removed in next step
	for (auto& meter : m_metersArray)
	{
		__int64 meterValue = meter->GetValue();

		if (meterValue == 0 || meterValue == INVALID_METER_VALUE)
		{
			delete meter;
			meter = nullptr;
		}
	}

	// Remove the nullptr entries from the meters array
	m_metersArray.erase(std::remove(m_metersArray.begin(), m_metersArray.end(), nullptr), 
		m_metersArray.end());

	LogString(GAMES, "[%s]: TrimToNonZeroValidMeters [NVRAM: %d sizeBefore: %u sizeAfter: %u]",
		__FUNCTION__, (m_type == MEMORY_NVRAM), sizeBefore, m_metersArray.size());
}
