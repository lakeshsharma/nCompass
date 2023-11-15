
using namespace std;

#include "gtest/gtest.h"
#include "stdafx.h"
#include "../Sentinel/Utilities.h"
#include "../Sentinel/Meters.h"
#include "../Sentinel/metersdef.h"

// Unit testing includes.
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::Mock;
using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::NiceMock;

class CMetersMock : public CMeters
{
public:
	CMetersMock(MEMORY_TYPE memoryType, DWORD numMaxMeters) : CMeters(memoryType)
	{
		m_NumMaxMeters = numMaxMeters;
		m_MetersBlock = (NVRAM_METERS_BLOCK*)new BYTE[sizeof(NVRAM_METERS_BLOCK) + m_NumMaxMeters*sizeof(NVRAM_METER_BLOCK)]();

		m_dwOffset = 0;  // to simplify offset calculation 
		SetAllowedSize(m_NumMaxMeters * NVRAM_METER_SIZE);
	}

	~CMetersMock()
	{
		if (m_MetersBlock)
		{
			delete m_MetersBlock;
			m_MetersBlock = NULL;
		}
	}

	MOCK_METHOD3(Read, bool(DWORD offset, BYTE * data, DWORD len));
	MOCK_METHOD2(IsActiveLocation_wrapper, bool(DWORD offset, BYTE value));
	MOCK_METHOD2(SetInactiveLocation_wrapper, bool(DWORD offset, BYTE value));

	#pragma pack(1)
	struct NVRAM_METER_BLOCK
	{
		BYTE isLocationActive;
		BYTE meterNumber;
		__int64 meterValue;
	};

	#pragma pack(1)
	struct NVRAM_METERS_BLOCK
	{
		BYTE isLocationActive;
		DWORD metersCount;
		NVRAM_METER_BLOCK meters[];
	};

	DWORD m_NumMaxMeters;
	NVRAM_METERS_BLOCK *m_MetersBlock;  // buffer to hold meters.

	void AddMeter(int index, BYTE isValid, BYTE meterNumber, __int64 meterValue)
	{
		if (index < m_NumMaxMeters)
		{
			m_MetersBlock->meters[index].isLocationActive = isValid;
			m_MetersBlock->meters[index].meterNumber = meterNumber;
			m_MetersBlock->meters[index].meterValue = meterValue;
		}
	}

	bool ReadMocked(DWORD offset, BYTE * data, DWORD len)
	{
		memcpy(data, (BYTE*)m_MetersBlock + offset, len);
		return true;
	}

	bool IsActiveLocation_wrapper_Mocked(DWORD offset, BYTE value)
	{
		return (*((BYTE*)m_MetersBlock + offset) == value);
	}

	bool SetInactiveLocation_wrapper_Mocked(DWORD offset, BYTE value)
	{
		*((BYTE*)m_MetersBlock + offset) = value;
		return true;
	}

	// NOTE : Write() method has 12 arguments and gmock does not support mocking of method with more than 10 arguments.
	//        That's why we are overriding Write() method here instead of mocking it.
	virtual bool Write(DWORD offset1, BYTE * data1, UINT len1,
		DWORD offset2 = NVRAM_NULL_VALUE, BYTE * data2 = NULL, UINT len2 = 0,
		DWORD offset3 = NVRAM_NULL_VALUE, BYTE * data3 = NULL, UINT len3 = 0,
		DWORD offset4 = NVRAM_NULL_VALUE, BYTE * data4 = NULL, UINT len4 = 0) const
	{
		if (offset1 != NVRAM_NULL_VALUE)
		{
			memcpy((BYTE*)m_MetersBlock + offset1, data1, len1);

			if (offset2 != NVRAM_NULL_VALUE)
			{
				memcpy((BYTE*)m_MetersBlock + offset2, data2, len2);

				if (offset3 != NVRAM_NULL_VALUE)
				{
					memcpy((BYTE*)m_MetersBlock + offset3, data3, len3);

					if (offset4 != NVRAM_NULL_VALUE)
					{
						memcpy((BYTE*)m_MetersBlock + offset4, data4, len4);
					}
				}
			}
		}

		return true;
	}
};

class MetersTest : public ::testing::Test
{
    protected:
        virtual void SetUp() 
        {
        }

        virtual void TearDown() 
        {
        }

	public:
		void BuildYourself_DuplicateMeters()
		{
			// setup
			NiceMock<CMetersMock> metersMock(MEMORY_NVRAM, 7);

			// adding 7 meters out of which 4 will be valid.
			metersMock.m_MetersBlock->isLocationActive = 0xFF;  // means NVRAM is active
			metersMock.m_MetersBlock->metersCount = 4;  // valid meters
			metersMock.AddMeter(0, 0xFF, 101, 1000);
			metersMock.AddMeter(1, 0xFF, 101, 1100); // duplicate meter
			metersMock.AddMeter(2, 0xFF, 102, 2000);
			metersMock.AddMeter(3, 0x00, 102, 2200); // invalid location
			metersMock.AddMeter(4, 0xFF, 103, 3000);
			metersMock.AddMeter(5, 0xFF, 103, 3300); // duplicate meter
			metersMock.AddMeter(6, 0xFF, 104, 4000);

			// mocking functions
			ON_CALL(metersMock, Read(_, _, _)).WillByDefault(Invoke(&metersMock, &CMetersMock::ReadMocked));
			ON_CALL(metersMock, IsActiveLocation_wrapper(_, _)).WillByDefault(Invoke(&metersMock, &CMetersMock::IsActiveLocation_wrapper_Mocked));
			ON_CALL(metersMock, SetInactiveLocation_wrapper(_, _)).WillByDefault(Invoke(&metersMock, &CMetersMock::SetInactiveLocation_wrapper_Mocked));

			// perform operation.
			metersMock.BuildYourself();

			// verify meter count and isLocationActive of duplicate meter.
			int meterCount = metersMock.GetMeterCount();
			ASSERT_TRUE(meterCount == 4) << "Meter Count is wrong.";
			ASSERT_TRUE(metersMock.m_MetersBlock->meters[1].isLocationActive == 0x00) << "First Duplicate Meter : isLocationActive is not 0.";
			ASSERT_TRUE(metersMock.m_MetersBlock->meters[5].isLocationActive == 0x00) << "Second Duplicate Meter : isLocationActive is not 0.";

			// verify meter 101 value
			CMeter *meter = metersMock.GetMeterByNumber(101);
			ASSERT_TRUE(meter != NULL) << "Meter 101 is NULL.";
			__int64 value = meter->GetValue();
			ASSERT_EQ(value, 1000) << "Meter 101 : value is incorrect.";

			// verify meter 102 value
			meter = metersMock.GetMeterByNumber(102);
			ASSERT_TRUE(meter != NULL) << "Meter 102 is NULL.";
			value = meter->GetValue();
			ASSERT_EQ(value, 2000) << "Meter 102 : value is incorrect.";

			// verify meter 103 value
			meter = metersMock.GetMeterByNumber(103);
			ASSERT_TRUE(meter != NULL) << "Meter 103 is NULL.";
			value = meter->GetValue();
			ASSERT_EQ(value, 3000) << "Meter 103 : value is incorrect.";

			// verify meter 104 value
			meter = metersMock.GetMeterByNumber(104);
			ASSERT_TRUE(meter != NULL) << "Meter 104 is NULL.";
			value = meter->GetValue();
			ASSERT_EQ(value, 4000) << "Meter 104 : value is incorrect.";
		}

		void PersistMeters()
		{
			// setup
			__int64 meterValue = 0x03ff1234;
			NiceMock<CMetersMock> metersMock(MEMORY_NVRAM, 10);
			metersMock.SetMeter(4, meterValue, false, false);
			metersMock.SetMeter(5, meterValue, false, false);
			metersMock.SetMeter(6, meterValue, false, false);
			metersMock.SetMeter(7, meterValue, false, false);

			// mocking functions
			ON_CALL(metersMock, Read(_, _, _)).WillByDefault(Invoke(&metersMock, &CMetersMock::ReadMocked));
			ON_CALL(metersMock, IsActiveLocation_wrapper(_, _)).WillByDefault(Invoke(&metersMock, &CMetersMock::IsActiveLocation_wrapper_Mocked));
			ON_CALL(metersMock, SetInactiveLocation_wrapper(_, _)).WillByDefault(Invoke(&metersMock, &CMetersMock::SetInactiveLocation_wrapper_Mocked));

			// perform operation
			metersMock.PersistMeters(true);  // write meters into NVRAM buffer
			metersMock.ClearAllMeters();     // clear all in memory meters
			metersMock.BuildYourself();      // read meters from NVRAM buffer

			// verify meter count.
			int meterCount = metersMock.GetMeterCount();
			ASSERT_EQ(meterCount, 4) << "Meter count is wrong.";

			// verify meter 4
			CMeter *meter = metersMock.GetMeterByNumber(4);
			ASSERT_TRUE(meter != NULL) << "Meter 4 is NULL.";
			__int64 value = meter->GetValue();
			ASSERT_EQ(value, meterValue) << "Meter 4 : value is incorrect.";

			// verify meter 5
			meter = metersMock.GetMeterByNumber(5);
			ASSERT_TRUE(meter != NULL) << "Meter 5 is NULL.";
			value = meter->GetValue();
			ASSERT_EQ(value, meterValue) << "Meter 5 : value is incorrect.";

			// verify meter 6
			meter = metersMock.GetMeterByNumber(6);
			ASSERT_TRUE(meter != NULL) << "Meter 6 is NULL.";
			value = meter->GetValue();
			ASSERT_EQ(value, meterValue) << "Meter 6 : value is incorrect.";

			// verify meter 7
			meter = metersMock.GetMeterByNumber(7);
			ASSERT_TRUE(meter != NULL) << "Meter 7 is NULL.";
			value = meter->GetValue();
			ASSERT_EQ(value, meterValue) << "Meter 7 : value is incorrect.";
		}
};

TEST_F(MetersTest,MetersConstructionTest)
{
    long long value = 1000;
    
    // Construct empty meters
    CMeters *meters = new CMeters(MEMORY_NONE);
    EXPECT_EQ(0,meters->GetMeterCount());
    
    // Construct from meters copy
    meters->SetMeter(mCrd, value, true, false);
    CMeters *copy = new CMeters(*meters);
    EXPECT_EQ(1,meters->GetMeterCount() == 1);
    EXPECT_EQ(value,meters->GetMeterByNumber(mCrd)->GetValue());
    
    delete meters;
    delete copy;
}

TEST_F(MetersTest, MetersConstructionCopyInvalidValue)
{
    CMeters *meters = new CMeters(MEMORY_NONE);
    meters->SetMeter(mCrd, INVALID_METER_VALUE);

    CMeters *copy = new CMeters(*meters);
    EXPECT_EQ(INVALID_METER_VALUE, copy->GetMeterByNumber(mCrd)->GetValue());

    delete meters;
    delete copy;

    // The second parameter specifies that invalid mater values are converted to 0
    meters = new CMeters(MEMORY_NONE, true);
    meters->SetMeter(mCrd, INVALID_METER_VALUE);

    copy = new CMeters(*meters);
    EXPECT_EQ(0, meters->GetMeterByNumber(mCrd)->GetValue());

    delete meters;
    delete copy;
}

TEST_F(MetersTest, MetersAssignmentTest)
{
    CMeters *meters = new CMeters(MEMORY_NONE, false);
    CMeters *zMeters = new CMeters(MEMORY_NONE, true);

    long long value = 1000;

    // Test add meter
    meters->SetMeter(mCrd, value, true, false);
    EXPECT_EQ(meters->GetMeterCount(), 1);
    EXPECT_EQ(meters->GetMeterByNumber(mCrd)->GetValue(),value);
    meters->SetMeter(gCI, INVALID_METER_VALUE, true, false);
    EXPECT_EQ(meters->GetMeterByNumber(gCI)->GetValue(), INVALID_METER_VALUE);
    zMeters->SetMeter(gCI, INVALID_METER_VALUE, true, false);
    EXPECT_NE(zMeters->GetMeterByNumber(gCI)->GetValue(), INVALID_METER_VALUE);


    // assign new value
    meters->SetMeter(mCrd, value * 2, true, false);
    EXPECT_EQ(meters->GetMeterByNumber(mCrd)->GetValue(), value * 2);

    delete meters;
    delete zMeters;
}

TEST_F(MetersTest, MetersClearAllMetersTest)
{
    long long value = 1000;
    CMeters *meters = new CMeters(MEMORY_NONE, false);
    CMeters *zMeters = new CMeters(MEMORY_NONE, true);

    meters->SetMeter(mCrd, value, true, false);
    meters->SetMeter(gCI, value, true, false);
    meters->SetMeter(gCO, value, true, false);
    EXPECT_EQ(meters->GetMeterCount(), 3);
    meters->ClearAllMeters();
    EXPECT_EQ(meters->GetMeterCount(), 0);

    delete meters;
    delete zMeters;
}

/// <summary>
/// Functional test for:
///     CMeters::SetAllMetersInvalid()
/// Test with 0 meters present and bMakeInvalidZero false.
/// </summary>
TEST_F(MetersTest, SetAllMetersInvalid0)
{
    // Data.
    const bool bMakeInvalidZero(false);
    const __int64 value = 1000;

    // Setup.
    CMeters meters(MEMORY_NONE, false);
    EXPECT_EQ(0, meters.GetMeterCount());

    // Perform operation(s) and test(s).
    meters.SetAllMetersInvalid();
    EXPECT_EQ(0, meters.GetMeterCount());
}

TEST_F(MetersTest, SetMeterInvalid)
{
	CMeters meters(MEMORY_NONE, false);
	const __int64 value = INVALID_METER_VALUE;
	const byte meternumber = 90;
	EXPECT_EQ(0, meters.GetMeterCount());

	meters.SetMeter(meternumber, value);
	EXPECT_EQ(1, meters.GetMeterCount());

	// This 2nd call should not add another meter entry
	meters.SetMeter(meternumber, value);
	EXPECT_EQ(1, meters.GetMeterCount());

}

/// <summary>
/// Functional test for:
///     CMeters::SetAllMetersInvalid()
/// Test with 3 meters present and bMakeInvalidZero false.
/// </summary>
TEST_F(MetersTest, SetAllMetersInvalid3_Invalid)
{
    // Data.
    const bool bMakeInvalidZero= false;
    const __int64 value = 1000;
    const __int64 valueExpected = bMakeInvalidZero ? 0 : INVALID_METER_VALUE;

    // Setup.
    CMeters meters(MEMORY_NONE, false);
    meters.SetMeter(mCrd, value, true, false);
    meters.SetMeter(gCI, value, true, false);
    meters.SetMeter(gCO, value, true, false);
    EXPECT_EQ(3, meters.GetMeterCount());

    // Perform operation(s) and test(s).
    meters.SetAllMetersInvalid();
    EXPECT_EQ(3, meters.GetMeterCount());
    for (int meterIndex(0); meterIndex < meters.GetMeterCount(); ++meterIndex)
    {
        const CMeter *meter(meters.GetMeterByIndex(meterIndex));
        EXPECT_NE(meter, nullptr);
        EXPECT_EQ(valueExpected, meter->GetValue());
    }
}


/// <summary>
/// Functional test for:
///     CMeters::SetAllMetersInvalid()
/// Test with 3 meters present and bMakeInvalidZero true.
/// </summary>
TEST_F(MetersTest, SetAllMetersInvalid3_Zero)
{
    // Data.
    const bool bMakeInvalidZero(true);
    const __int64 value = 1000;
    const __int64 valueExpected = !bMakeInvalidZero ? 0 : INVALID_METER_VALUE;

    // Setup.
    CMeters meters(MEMORY_NONE, false);
    meters.SetMeter(mCrd, value, true, false);
    meters.SetMeter(gCI, value, true, false);
    meters.SetMeter(gCO, value, true, false);
    EXPECT_EQ(3, meters.GetMeterCount());

    // Perform operation(s) and test(s).
    meters.SetAllMetersInvalid();
    EXPECT_EQ(3, meters.GetMeterCount());
    for (int meterIndex(0); meterIndex < meters.GetMeterCount(); ++meterIndex)
    {
        const CMeter *meter(meters.GetMeterByIndex(meterIndex));
        EXPECT_NE(meter, nullptr);
        EXPECT_EQ(valueExpected, meter->GetValue());
    }
}

TEST_F(MetersTest, MetersAddToMeterTest)
{
    CMeters *meters = new CMeters(MEMORY_NONE, false);
    CMeters *zMeters = new CMeters(MEMORY_NONE, true);

    long long value = 1000;

    // Test add meter
    meters->SetMeter(mCrd, value, true, false);
    meters->SetMeter(gCI, INVALID_METER_VALUE, true, false);
    zMeters->SetMeter(gCI, INVALID_METER_VALUE, true, false);

    // Add to meter
    meters->AddToMeter(mCrd, value, true, false);
    EXPECT_EQ(meters->GetMeterByNumber(mCrd)->GetValue(), value * 2);

    // Add to meters initialized with INVALID_METER_VALUE
    meters->AddToMeter(gCI, value, true, false);
    zMeters->AddToMeter(gCI, value, true, false);
    EXPECT_EQ(meters->GetMeterByNumber(gCI)->GetValue(), value);
    EXPECT_EQ(zMeters->GetMeterByNumber(gCI)->GetValue(), value);

    delete meters;
    delete zMeters;
}

TEST_F(MetersTest, MetersCompareMeterTest)
{
    long long value = 1000;

    CMeters *meters = new CMeters(MEMORY_NONE, false);
    CMeter *crdMeter = new CMeter(mCrd);
    CMeter *invMeter = new CMeter(gCO, INVALID_METER_VALUE);
    CMeter *nonMeter = new CMeter(mSelG, value);
    CMeter *gccMeter = new CMeter(gCC, value);

    // compare equal values
    crdMeter->SetValue(value);
    meters->SetMeter(mCrd, value, true, false);
    EXPECT_EQ(meters->CompareMeter(crdMeter), 0);

    // compare non-equal values
    crdMeter->SetValue(value + 1);
    EXPECT_EQ(meters->CompareMeter(crdMeter), 1);

    // compare INVALID_METER_VALUE values
    meters->SetMeter(gCC, INVALID_METER_VALUE, true, false);
    EXPECT_EQ(meters->CompareMeter(gccMeter), value);
    meters->SetMeter(gCO, value, true, false);
    EXPECT_EQ(meters->CompareMeter(invMeter), 0);

    // compare meter not present
    EXPECT_EQ(meters->CompareMeter(nonMeter), value);

    delete meters;
    delete crdMeter;
    delete invMeter;
    delete nonMeter;
    delete gccMeter;
}

TEST_F(MetersTest, MetersDuplicateChangedMetersTest)
{
    CMeters *meters = new CMeters(MEMORY_NONE, false);
    CMeters *changedMeters;
    int value = 1000;

    // Add meters, mark 2 of 5 changed
    meters->SetMeter(mCrd, value, true, false);               
    const CMeter *meter = meters->GetMeterByNumber(mCrd);     
    const_cast<CMeter *>(meter)->SetMeterChanged();           
    meters->SetMeter(gCI, value, true, false);                
    meters->SetMeter(gCO, value, true, false);                
    meters->SetMeter(gGS, value, true, false);                
    meter = meters->GetMeterByNumber(gGS);                    
    const_cast<CMeter *>(meter)->SetMeterChanged();           
    meters->SetMeter(gGW, value, true, false);                
                                                              
    changedMeters = meters->DuplicateChangedMeters();         
    EXPECT_EQ(changedMeters->GetMeterCount(), 2);             
    EXPECT_NE(changedMeters->GetMeterByNumber(mCrd), nullptr);
    EXPECT_NE(changedMeters->GetMeterByNumber(gGS), nullptr); 
    EXPECT_EQ(changedMeters->GetMeterByIndex(gGW), nullptr);  
                                                              
    delete meters;
    delete changedMeters;
}

TEST_F(MetersTest, MetersDuplicateChangedMetersGameInProgressTest)
{
    CMeters *meters = new CMeters(MEMORY_NONE, false);
    CMeters *changedMeters;
    int value = 1000;
    bool gameInProgress = true;

    // Add meters, mark mCrd, gCI, gGS and gCO changed
    meters->SetMeter(mCrd, value, true, false);               
    const CMeter *meter = meters->GetMeterByNumber(mCrd);     
    const_cast<CMeter *>(meter)->SetMeterChanged();           
    meters->SetMeter(gCI, value, true, false);  
    meter = meters->GetMeterByNumber(gCI);                    
    const_cast<CMeter *>(meter)->SetMeterChanged();               
    meters->SetMeter(gCO, value, true, false);
    meter = meters->GetMeterByNumber(gCO);                    
    const_cast<CMeter *>(meter)->SetMeterChanged();
    meters->SetMeter(gGS, value, true, false);                
    meter = meters->GetMeterByNumber(gGS);                    
    const_cast<CMeter *>(meter)->SetMeterChanged();           
    meters->SetMeter(gGW, value, true, false);                
                                                              
    changedMeters = meters->DuplicateChangedMeters(gameInProgress);         
    EXPECT_EQ(changedMeters->GetMeterCount(), 1);             
    EXPECT_EQ(changedMeters->GetMeterByNumber(mCrd), nullptr);
    EXPECT_EQ(changedMeters->GetMeterByNumber(gCI), nullptr);
    EXPECT_EQ(changedMeters->GetMeterByNumber(gCO), nullptr);
    EXPECT_NE(changedMeters->GetMeterByNumber(gGS), nullptr); 
    EXPECT_EQ(changedMeters->GetMeterByIndex(gGW), nullptr);  
                                                              
    delete meters;
    delete changedMeters;
}

TEST_F(MetersTest, MetersTestRolloverDetectionTest)
{
    // meter value 9990 compared to 100 will assume to have rolled over at 9999
    long long startValue = 9990;
    long long newValue = 100;

    CMeters *meters = new CMeters(MEMORY_NONE);
    CMeter *newMeter = new CMeter(mCrd, newValue);

    meters->SetMeter(mCrd, startValue, true, false);
    long long diff = meters->CompareMeter(newMeter);
    EXPECT_EQ(diff, 110);

    startValue = 999990;
    meters->SetMeter(mCrd, startValue, true, false);
    diff = meters->CompareMeter(newMeter);
    EXPECT_EQ(diff, 110);
}

TEST_F(MetersTest, MetersRemoveMeterByNumber)
{
    CMeters *meters = new CMeters(MEMORY_NONE);

    meters->SetMeter(gCI, 100);
    meters->SetMeter(gCO, 5);
    EXPECT_EQ(meters->GetMeterCount(), 2);

    // Remove existing meter
    const CMeter *removedMeter = meters->RemoveMeterByNumber(gCO);
    EXPECT_EQ(meters->GetMeterCount(), 1);
    EXPECT_EQ(removedMeter->GetMeterNumber(), gCO);
    EXPECT_EQ(meters->GetMeterByNumber(gCO), nullptr);

    // Remove non-existent meter
    removedMeter = meters->RemoveMeterByNumber(gCC);
    EXPECT_EQ(meters->GetMeterCount(), 1);
    EXPECT_EQ(removedMeter, nullptr);

    // Remove last meter
    removedMeter = meters->RemoveMeterByNumber(gCI);
    EXPECT_EQ(meters->GetMeterCount(), 0);
    EXPECT_EQ(removedMeter->GetMeterNumber(), gCI);
    EXPECT_EQ(meters->GetMeterByNumber(gCI), nullptr);

    // Remove meter from empty meters
    removedMeter = meters->RemoveMeterByNumber(mD200);
    EXPECT_EQ(meters->GetMeterCount(), 0);
    EXPECT_EQ(removedMeter, nullptr);

    delete meters;
}

TEST_F(MetersTest, SetBillMeters1)
{
    CMeters *meters = new CMeters(MEMORY_NONE);

    meters->SetBillMeters(mD1);	//{Ones}
    meters->SetBillMeters(mD2);	//{Twos}
    meters->SetBillMeters(mD5);	//{Fives}
    meters->SetBillMeters(mD10);	//{Tens}
    meters->SetBillMeters(mD20);	//{Twenties}
    meters->SetBillMeters(mD50);	//{Fifties}
    meters->SetBillMeters(mD100);	//{Hundreds}
    meters->SetBillMeters(mD200); //{Two Hundreds}
    meters->SetBillMeters(mD500); //{Five Hundreds}
    meters->SetBillMeters(mD1000); //{One Thousands}

    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD1));	//{Ones}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD2));	//{Twos}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD5));	//{Fives}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD10));	//{Tens}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD20));	//{Twenties}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD50));	//{Fifties}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD100));	//{Hundreds}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD200)); //{Two Hundreds}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD500)); //{Five Hundreds}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD1000)); //{One Thousands}

    delete meters;
}

TEST_F(MetersTest, SetBillMeters2)
{
    CMeters *meters = new CMeters(MEMORY_NONE);

    meters->SetBillMeters(mNone);

    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD1));	//{Ones}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD2));	//{Twos}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD5));	//{Fives}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD10));	//{Tens}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD20));	//{Twenties}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD50));	//{Fifties}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD100));	//{Hundreds}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD200)); //{Two Hundreds}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD500)); //{Five Hundreds}
    EXPECT_NE(nullptr, meters->GetMeterByNumber(mD1000)); //{One Thousands}

    delete meters;
}

TEST_F(MetersTest, SetBillMeters3)
{
    CMeters *meters = new CMeters(MEMORY_NONE);

    meters->SetBillMeters(gCI);
    meters->SetBillMeters(gCO);
    meters->SetBillMeters(gGS);
    meters->SetBillMeters(mDr);

    EXPECT_EQ(1, meters->GetMeterCount());

    delete meters;
}

TEST_F(MetersTest, TrimToNonZeroValidMetersTest)
{
	CMeters *meters = new CMeters(MEMORY_NONE);

	// Add few meters with valid values, invalid values, and 0 values 
	meters->SetMeter(mCrd, 100);
	meters->SetMeter(mD1, 0);
	meters->SetMeter(mD2, 2);
	meters->SetMeter(mD5, INVALID_METER_VALUE);
	meters->SetMeter(gCI, 0);
	meters->SetMeter(gCO, INVALID_METER_VALUE);

	ASSERT_EQ(6, meters->GetMeterCount());

	meters->TrimToNonZeroValidMeters();

	// Validate that meters with 0 and invalid value are removed
	// and other valid meters have values intact
	ASSERT_EQ(meters->GetMeterCount(), 2);
	ASSERT_EQ(meters->GetMeterByNumber(mCrd)->GetValue(), 100);
	ASSERT_EQ(meters->GetMeterByNumber(mD1), nullptr);
	ASSERT_EQ(meters->GetMeterByNumber(mD2)->GetValue(), 2);
	ASSERT_EQ(meters->GetMeterByNumber(mD5), nullptr);

	delete meters;
}

/// <summary>
/// Functional test for:
///     CMeters::BuildYourself()
/// Test BuildYourself() method with duplicate meters in the memory buffer.
/// </summary>
TEST_F(MetersTest, BuildYourself_DuplicateMeters)
{
	BuildYourself_DuplicateMeters();
}

/// <summary>
/// Functional test for:
///     CMeters::PersistMeters()
/// Test PersistMeters() method.
/// </summary>
TEST_F(MetersTest, PersistMeters)
{
	PersistMeters();
}