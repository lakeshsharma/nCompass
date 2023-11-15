
#include "gtest/gtest.h"
#include "windef.h"
#include "../Sentinel/Meter.h"
#include "../Sentinel/metersdef.h"

class MeterTest : public ::testing::Test
{
    protected:
        virtual void SetUp() 
        {
        }

        virtual void TearDown() 
        {
        }

};

TEST_F(MeterTest,MeterConstructionTest)
{
    // Default Value
    CMeter *meter = new CMeter(mCrd);
    EXPECT_EQ(mCrd,meter->GetMeterNumber());
    EXPECT_EQ(INVALID_METER_VALUE,meter->GetValue());
    EXPECT_FALSE(meter->IsMeterChanged());
    delete meter;
    
    // Constructed with initialized value
    meter = new CMeter(mCrd,1000);
    EXPECT_EQ(1000,meter->GetValue());
    
    // Construct from existing meter
    CMeter *copyMeter = new CMeter(*meter);
    EXPECT_EQ(mCrd,copyMeter->GetMeterNumber());
    EXPECT_EQ(1000,copyMeter->GetValue());
    
    delete meter;
    delete copyMeter;

 }

TEST_F(MeterTest,MeterNumberTest)
{
    // Default Value
    CMeter *meter = new CMeter(mCrd);
    meter->SetMeterNumber(gCI);
    EXPECT_EQ(gCI,meter->GetMeterNumber());
 }

TEST_F(MeterTest,MeterValueTest)
{
    // Default Value
    CMeter *meter = new CMeter(mCrd);
    meter->SetValue(INTMAX_MAX);
    EXPECT_EQ(INTMAX_MAX,meter->GetValue());
 }

