#include "stdafx.h"
#include "SASProtocol/LongPoll2FMessage.h"

// Unit testing includes.
#include "TestUtil.h"
#include "Utility/AssertMsg.h"

#include "gtest/gtest.h"
using namespace std;

static const byte m_gameAddress = 0x01;
static const byte m_gameNumber = 0;

static const byte POLL_VALUE = CLongPoll2FMessage::POLL_VALUE;
static const int DEFAULT_METER_VALUE_SIZE = CLongPoll2FMessage::DEFAULT_METER_VALUE_SIZE;
static const int LONG_METER_VALUE_SIZE = CLongPoll2FMessage::LONG_METER_VALUE_SIZE;

class LongPoll2FMessage : public ::testing::Test
{
public: 
    void CallMeterReceived(CLongPoll2FMessage& obj, bool &ok, const byte *&response, const byte *responseEnd)
    {
        obj.MeterReceived(ok, response, responseEnd);
    }

protected:
    virtual void SetUp() 
    {
    }

    virtual void TearDown() 
    {
    }
        
};
/// <summary>
/// Test that SAS long poll 2F constants are correct.
/// </summary>
TEST_F(LongPoll2FMessage, LongPoll2FMessage_Constants)
{
    // Perform test(s).
    ASSERT_EQ(7, X2F_EXTRADATASIZE) << "X2F_EXTRADATASIZE";
    ASSERT_EQ(5, X2F_RESPONSE_EXTRADATASIZE) << "X2F_RESPONSE_EXTRADATASIZE";
    ASSERT_EQ(2, X2F_LENGTHBYTE) << "X2F_LENGTHBYTE";
    ASSERT_EQ(1, X2F_LENGTHSIZE) << "X2F_LENGTHSIZE";
    ASSERT_EQ(6, X2F_DEFMETERSIZE) << "X2F_DEFMETERSIZE";
    ASSERT_EQ(3, X2F_GAMENUMBERLOCATION) << "X2F_GAMENUMBERLOCATION";
    ASSERT_EQ(2, X2F_GAMENUMBERSIZE) << "X2F_GAMENUMBERSIZE";
    ASSERT_EQ(5, X2F_METERLOCATION) << "X2F_METERLOCATION";
    ASSERT_EQ(10, X2F_MAX_METERS) << "X2F_MAX_METERS";

    ASSERT_EQ((byte)0x2f, POLL_VALUE) << "POLL_VALUE";
    ASSERT_EQ((int)4, DEFAULT_METER_VALUE_SIZE) << "DEFAULT_METER_VALUE_SIZE";
    ASSERT_EQ((int)5, LONG_METER_VALUE_SIZE) << "LONG_METER_VALUE_SIZE";
}

CMeters *GetDefaultMeters()
{
    CMeters *meters(new CMeters(MEMORY_NONE, false));
    meters->SetMeter(gCI);
    meters->SetMeter(gCO);
    meters->SetMeter(mCoupV);
    meters->SetMeter(mCD);
    meters->SetMeter(mActualCD);
    meters->SetMeter(mTCi);
    meters->SetMeter(mTValueCI);
    meters->SetMeter(mTCo);
    meters->SetMeter(mTValueCO);

    return meters;
}

void AssertMetersAreEqual(const CMeters &a, const CMeters &b)
{
    ASSERT_EQ(a.GetMeterCount(), b.GetMeterCount()) << "GetMeterCount";
    for (int index(0); index < a.GetMeterCount(); ++index)
    {
        const CMeter *meterA = a.GetMeterByIndex(index);
        const CMeter *meterB = b.GetMeterByIndex(index);
        ASSERT_TRUE(meterA != nullptr)<< AssertMsg::Format(__LINE__, L"a.GetMeterByIndex", L"index=%d", (int)index);
        ASSERT_TRUE(meterB != nullptr)<< AssertMsg::Format(__LINE__, L"b.GetMeterByIndex", L"index=%d", (int)index);
        ASSERT_EQ(meterA->GetMeterNumber(), meterB->GetMeterNumber())<< AssertMsg::Format(__LINE__, L"GetMeterNumber", L"index=%d", (int)index);
        ASSERT_EQ(meterA->GetValue(), meterB->GetValue())<< AssertMsg::Format(__LINE__, L"GetValue", L"index=%d", (int)index);
    }
}

// #pragma region MeterReceived
/// <summary>
/// Code coverage and functional test for:
///     CLongPoll2FMessage::MeterReceived()
/// Test with ok false.
/// </summary>
TEST_F(LongPoll2FMessage, MeterReceived_NotOkay)
{
    // Data.
    const byte responseBuffer[] = {
        0x0d, // SAS meter code for Oasis mCoupV.
        0x12, 0x34, 0x56, 0x78, 0x90, // Meter value.
    };

    bool ok(false);
    const byte *response(responseBuffer);
    const byte *responseEnd(responseBuffer + _countof(responseBuffer));

    // Setup.
    unique_ptr<CMeters> meters(GetDefaultMeters());
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());

    // Perform operation(s) and test(s).
    CallMeterReceived(lp2f, ok, response, responseEnd);
    //lp2f.MeterReceived(ok, response, responseEnd);
    ASSERT_FALSE(ok) << "ok";
    ASSERT_EQ((const byte *)responseBuffer, response) << "response";
    AssertMetersAreEqual(*metersExpected, *meters);
}

/// <summary>
/// Code coverage and functional test for:
///     CLongPoll2FMessage::MeterReceived()
/// Test with meter code missing.
/// </summary>

TEST_F(LongPoll2FMessage, MeterReceived_NoMeterCode)                                                                                 
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        0x0d, // SAS meter code for Oasis mCoupV.                                                                                  
        0x00, 0x00, 0x00, 0x12, 0x34, // Meter value.
    };
                                                                                                                                   
    bool ok(true);                                                                                                                 
    const byte *response(responseBuffer);                                                                                          
    const byte *responseEnd(responseBuffer + 0); // Meter code field missing.                                                      
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).      
    CallMeterReceived(lp2f, ok, response, responseEnd);                                                                                     
    //lp2f.MeterReceived(ok, response, responseEnd);                                                                                 
    ASSERT_FALSE(ok) << "ok";                                                                                                  
    ASSERT_EQ((const byte *)responseBuffer, response) << "response";                                                               
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a meter length of 5 and the response buffer too small to contain the meter length.                                   
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, MeterReceived_ResponseTooSmall)                                                                            
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        0x0d, // SAS meter code for Oasis mCoupV.                                                                                  
        0x00, 0x00, 0x00, 0x12, 0x34, // Meter value.                                                                              
    };
                                                                                                                                           
    bool ok(true);                                                                                                                 
    const byte *response(responseBuffer);                                                                                          
    const byte *responseEnd(responseBuffer + _countof(responseBuffer) - 1); // Response buffer to small to contain the meter value.
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).   
    CallMeterReceived(lp2f, ok, response, responseEnd);                                                                                        
    //lp2f.MeterReceived(ok, response, responseEnd);                                                                                 
    ASSERT_FALSE(ok) << "ok";                                                                                                  
    ASSERT_EQ(responseBuffer + 1, response) << "response";                                                                         
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response that contains an unexpected meter code.                                                                   
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, MeterReceived_WrongMeterCode)                                                                              
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        0x1f, // SAS meter code for Oasis gJP.                                                                                     
        0x00, 0x00, 0x12, 0x34, // Meter value.                                                                                    
    };
                                                                                                                                     
    bool ok(true);                                                                                                                 
    const byte *response(responseBuffer);                                                                                          
    const byte *responseEnd(responseBuffer + _countof(responseBuffer));                                                            
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).  
    CallMeterReceived(lp2f, ok, response, responseEnd);                                                                                         
    //lp2f.MeterReceived(ok, response, responseEnd);                                                                                 
    ASSERT_TRUE(ok) << "ok";                                                                                                   
    ASSERT_EQ(responseEnd, response) << "response";                                                                                
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a meter length of 4 and a relatively small value.                                                                    
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, MeterReceived_Good4a)                                                                                      
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        0x01, // SAS meter code for Oasis gCO.                                                                                     
        0x00, 0x00, 0x12, 0x34, // Meter value.                                                                                    
    };
                                                                                                                                        
    bool ok(true);                                                                                                                 
    const byte *response(responseBuffer);                                                                                          
    const byte *responseEnd(responseBuffer + _countof(responseBuffer));                                                            
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
    metersExpected->SetMeter(gCO, 1234);                                                                                           
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).  
    CallMeterReceived(lp2f, ok, response, responseEnd);                                                                                         
    //lp2f.MeterReceived(ok, response, responseEnd);                                                                                 
    ASSERT_TRUE(ok) << "ok";                                                                                                   
    ASSERT_EQ(responseEnd, response) << "response";                                                                                
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a meter length of 4 and a relatively large value.                                                                    
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, MeterReceived_Good4b)                                                                                      
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        0x01, // SAS meter code for Oasis gCO.                                                                                     
        0x12, 0x34, 0x56, 0x78, // Meter value.                                                                                    
    };
                                                                                                                                      
    bool ok(true);                                                                                                                 
    const byte *response(responseBuffer);                                                                                          
    const byte *responseEnd(responseBuffer + _countof(responseBuffer));                                                            
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
    metersExpected->SetMeter(gCO, 12345678);                                                                                       
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).  
    CallMeterReceived(lp2f, ok, response, responseEnd);                                                                                         
    //lp2f.MeterReceived(ok, response, responseEnd);                                                                                 
    ASSERT_TRUE(ok) << "ok";                                                                                                   
    ASSERT_EQ(responseEnd, response) << "response";                                                                                
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a meter length of 5 and a relatively small value.                                                                    
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, MeterReceived_Good5a)                                                                                      
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        0x0d, // SAS meter code for Oasis mCoupV.                                                                                  
        0x00, 0x00, 0x00, 0x12, 0x34, // Meter value.                                                                              
    };
                                                                                                                                     
    bool ok(true);                                                                                                                 
    const byte *response(responseBuffer);                                                                                          
    const byte *responseEnd(responseBuffer + _countof(responseBuffer));                                                            
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
    metersExpected->SetMeter(mCoupV, 1234);                                                                                        
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    CallMeterReceived(lp2f, ok, response, responseEnd);
    //lp2f.MeterReceived(ok, response, responseEnd);                                                                                 
    ASSERT_TRUE(ok) << "ok";                                                                                                   
    ASSERT_EQ(responseEnd, response) << "response";                                                                                
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a meter length of 5 and a relatively large value.                                                                    
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, MeterReceived_Good5b)                                                                                      
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        0x0d, // SAS meter code for Oasis mCoupV.                                                                                  
        0x12, 0x34, 0x56, 0x78, 0x90, // Meter value.                                                                              
    };
                                                                                                                                     
    bool ok(true);                                                                                                                 
    const byte *response(responseBuffer);                                                                                          
    const byte *responseEnd(responseBuffer + _countof(responseBuffer));                                                            
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
    metersExpected->SetMeter(mCoupV, 1234567890);                                                                                  
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    CallMeterReceived(lp2f, ok, response, responseEnd);
    //lp2f.MeterReceived(ok, response, responseEnd);                                                                                 
    ASSERT_TRUE(ok) << "ok";                                                                                                   
    ASSERT_EQ(responseEnd, response) << "response";                                                                                
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with the meter code for coin drop.  Verify that both coin drop meters are set.                                            
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, MeterReceived_CoinDrop)                                                                                    
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        0x0a, // SAS meter code for Oasis gCD and mActualCD.                                                                       
        0x12, 0x34, 0x56, 0x78, // Meter value.                                                                                    
    };
                                                                                                                                     
    bool ok(true);                                                                                                                 
    const byte *response(responseBuffer);                                                                                          
    const byte *responseEnd(responseBuffer + _countof(responseBuffer));                                                            
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
    metersExpected->SetMeter(mCD, 12345678);                                                                                       
    metersExpected->SetMeter(mActualCD, 12345678);                                                                                 
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    CallMeterReceived(lp2f, ok, response, responseEnd);
    //lp2f.MeterReceived(ok, response, responseEnd);                                                                                 
    ASSERT_TRUE(ok) << "ok";                                                                                                   
    ASSERT_EQ(responseEnd, response) << "response";                                                                                
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with the meter code for true coin in.  Verify that both true coin in meters are set.                                      
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, MeterReceived_TrueCoinIn)                                                                                  
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        0x08, // SAS meter code for Oasis mTCi and mTValueCI.                                                                      
        0x12, 0x34, 0x56, 0x78, // Meter value.                                                                                    
    };
                                                                                                                                     
    bool ok(true);                                                                                                                 
    const byte *response(responseBuffer);                                                                                          
    const byte *responseEnd(responseBuffer + _countof(responseBuffer));                                                            
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
    metersExpected->SetMeter(mTCi, 12345678);                                                                                      
    metersExpected->SetMeter(mTValueCI, 12345678);                                                                                 
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    CallMeterReceived(lp2f, ok, response, responseEnd);
    //lp2f.MeterReceived(ok, response, responseEnd);                                                                                 
    ASSERT_TRUE(ok) << "ok";                                                                                                   
    ASSERT_EQ(responseEnd, response) << "response";                                                                                
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with the meter code for true coin in.  Verify that both true coin in meters are set.                                      
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, MeterReceived_TrueCoinOut)                                                                                 
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        0x09, // SAS meter code for Oasis mTCo and mTValueCO.                                                                      
        0x12, 0x34, 0x56, 0x78, // Meter value.                                                                                    
    };
                                                                                                                                     
    bool ok(true);                                                                                                                 
    const byte *response(responseBuffer);                                                                                          
    const byte *responseEnd(responseBuffer + _countof(responseBuffer));                                                            
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
    metersExpected->SetMeter(mTCo, 12345678);                                                                                      
    metersExpected->SetMeter(mTValueCO, 12345678);                                                                                 
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    CallMeterReceived(lp2f, ok, response, responseEnd);
    //lp2f.MeterReceived(ok, response, responseEnd);                                                                                 
    ASSERT_TRUE(ok) << "ok";                                                                                                   
    ASSERT_EQ(responseEnd, response) << "response";                                                                                
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that contains 2 meters.                                                                            
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, MeterReceived_2Meters)                                                                                     
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        0x00, // SAS meter code for Oasis gCI.                                                                                     
        0x12, 0x34, 0x56, 0x78, // Meter value.                                                                                    
        0x0d, // SAS meter code for Oasis mCoupV.                                                                                  
        0x12, 0x34, 0x56, 0x78, 0x90, // Meter value.                                                                              
    };
                                                                                                                                     
    bool ok(true);                                                                                                                 
    const byte *response(responseBuffer);                                                                                          
    const byte *responseEnd(responseBuffer + _countof(responseBuffer));                                                            
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
    metersExpected->SetMeter(gCI, 12345678);                                                                                       
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    CallMeterReceived(lp2f, ok, response, responseEnd);
    //lp2f.MeterReceived(ok, response, responseEnd); // Process first meter.                                                         
    ASSERT_TRUE(ok) << "ok";                                                                                                   
    ASSERT_EQ(responseBuffer + 5, response) << "response";                                                                         
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
                                                                                                                                   
    CallMeterReceived(lp2f, ok, response, responseEnd);
    //lp2f.MeterReceived(ok, response, responseEnd);                                                                                 
    ASSERT_TRUE(ok) << "ok"; // Process second meter.                                                                          
    ASSERT_EQ(responseEnd, response) << "response";                                                                                
    metersExpected->SetMeter(mCoupV, 1234567890);                                                                                  
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
// #pragma endregion MeterReceived                                                                                                         
                                                                                                                                   
// #pragma region ResponseReceived                                                                                                         
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that is missing the CRC field -- by reducing the response length.                                  
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_NoCrc)                                                                                    
{
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        m_gameAddress, // Game address.                                                                                            
        0x2f, // Poll value.                                                                                                       
        0, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                 
        0x00, 0x00, // Game number.                                                                                                
        0xcc, 0xcc, // CRC field.                                                                                                  
    };
    size_t responseBufferSize(0);                                                                                                  
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= responseBufferSize) << "GetResponseBufferSize";                                    
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, responseBufferSize);                                                          
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), responseBufferSize);                                                          
    lp2f.SetReturnValue(SASPOLL_SUCCESS);
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived();  
    std::cout << "TestForBuild\n";                                                                                                       
    ASSERT_EQ(SASPOLL_NACK, lp2f.GetReturnValue()) << "GetReturnValue";                                                            
    AssertMetersAreEqual(*metersExpected, *meters);
}                                                                                                                                  
                                                                                                                                   
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that is partially missing the CRC field -- by reducing the response length.                        
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_PartialCrc)                                                                               
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        m_gameAddress, // Game address.                                                                                            
        0x2f, // Poll value.                                                                                                       
        0, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                 
        0x00, 0x00, // Game number.                                                                                                
        0xcc, 0xcc, // CRC field.                                                                                                  
    };
    size_t responseBufferSize(1);                                                                                                  
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= responseBufferSize) << "GetResponseBufferSize";                                    
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, responseBufferSize);                                                          
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), responseBufferSize);                                                          
    lp2f.SetReturnValue(SASPOLL_SUCCESS);                                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived();
    std::cout << "TestForBuild\n";                                                                                                       
    ASSERT_EQ(SASPOLL_NACK, lp2f.GetReturnValue()) << "GetReturnValue";                                                            
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that is missing the game address field -- by reducing the response length.                         
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_NoGameAddress)                                                                            
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        m_gameAddress, // Game address.                                                                                            
        0x2f, // Poll value.                                                                                                       
        0, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                 
        0x00, 0x00, // Game number.                                                                                                
        0xcc, 0xcc, // CRC field.                                                                                                  
    };
    size_t responseBufferSize(sizeof(responseBuffer) - 5);                                                                         
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= responseBufferSize) << "GetResponseBufferSize";                                    
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, responseBufferSize);                                                          
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), responseBufferSize);                                                          
    lp2f.SetReturnValue(SASPOLL_SUCCESS);                                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived(); 
    std::cout << "TestForBuild\n";                                                                                                       
    ASSERT_EQ(SASPOLL_NACK, lp2f.GetReturnValue()) << "GetReturnValue";                                                            
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that is missing the poll value field -- by reducing the response length.                           
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_NoPollValue)                                                                              
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        m_gameAddress, // Game address.                                                                                            
        0x2f, // Poll value.                                                                                                       
        0, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                 
        0x00, 0x00, // Game number.                                                                                                
        0xcc, 0xcc, // CRC field.                                                                                                  
    };
    size_t responseBufferSize(sizeof(responseBuffer) - 4);                                                                         
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= responseBufferSize) << "GetResponseBufferSize";                                    
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, responseBufferSize);                                                          
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), responseBufferSize);                                                          
    lp2f.SetReturnValue(SASPOLL_SUCCESS);                                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived();
    std::cout << "TestForBuild\n";                                                                                                       
    ASSERT_EQ(SASPOLL_NACK, lp2f.GetReturnValue()) << "GetReturnValue";                                                            
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that is missing the length field -- by reducing the response length.                               
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_NoLength)                                                                                 
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        m_gameAddress, // Game address.                                                                                            
        0x2f, // Poll value.                                                                                                       
        0, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                 
        0x00, 0x00, // Game number.                                                                                                
        0xcc, 0xcc, // CRC field.                                                                                                  
    };
    size_t responseBufferSize(sizeof(responseBuffer) - 3);                                                                         
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= responseBufferSize) << "GetResponseBufferSize";                                    
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, responseBufferSize);                                                          
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), responseBufferSize);                                                          
    lp2f.SetReturnValue(SASPOLL_SUCCESS);                                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived();                                                                                                       
    ASSERT_EQ(SASPOLL_NACK, lp2f.GetReturnValue()) << "GetReturnValue";                                                            
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that is missing the game number field -- by reducing the response length.                          
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_NoGameNumber)                                                                             
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        m_gameAddress, // Game address.                                                                                            
        0x2f, // Poll value.                                                                                                       
        0, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                 
        0x00, 0x00, // Game number.                                                                                                
        0xcc, 0xcc, // CRC field.                                                                                                  
    };
    size_t responseBufferSize(sizeof(responseBuffer) - 2);                                                                         
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= responseBufferSize) << "GetResponseBufferSize";                                    
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, responseBufferSize);                                                          
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), responseBufferSize);                                                          
    lp2f.SetReturnValue(SASPOLL_SUCCESS);                                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived();                                                                                                       
    ASSERT_EQ(SASPOLL_NACK, lp2f.GetReturnValue()) << "GetReturnValue";                                                            
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that is missing the last byte of the game number field -- by reducing the response length.         
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_PartialGameNumber)                                                                        
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        m_gameAddress, // Game address.                                                                                            
        0x2f, // Poll value.                                                                                                       
        1, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                 
        0x00, 0x00, // Game number.                                                                                                
        0xcc, 0xcc, // CRC field.                                                                                                  
    };
    size_t responseBufferSize(sizeof(responseBuffer) - 1);                                                                         
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= responseBufferSize) << "GetResponseBufferSize";                                    
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, responseBufferSize);                                                          
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), responseBufferSize);                                                          
    lp2f.SetReturnValue(SASPOLL_SUCCESS);                                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived();                                                                                                       
    ASSERT_EQ(SASPOLL_NACK, lp2f.GetReturnValue()) << "GetReturnValue";                                                            
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that contains the wrong game address.                                                              
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_WrongGameAddress)                                                                         
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        (byte)(m_gameAddress + 1), // Game address.                                                                                
        0x2f, // Poll value.                                                                                                       
        13, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                
        0x00, 0x00, // Game number.                                                                                                
        0x01, // SAS meter code for Oasis gCO.                                                                                     
        0x12, 0x34, 0x56, 0x78, // Meter value.                                                                                    
        0x0d, // SAS meter code for Oasis mCoupV.                                                                                  
        0x12, 0x34, 0x56, 0x78, 0x90, // Meter value.                                                                              
        0xcc, 0xcc, // CRC field.                                                                                                  
    };
                                                                                                                                       
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= sizeof(responseBuffer)) << "GetResponseBufferSize";                                
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, sizeof(responseBuffer));                                                      
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), sizeof(responseBuffer));                                                      
    lp2f.SetReturnValue(SASPOLL_SUCCESS);                                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived();                                                                                                       
    ASSERT_EQ(SASPOLL_NACK, lp2f.GetReturnValue()) << "GetReturnValue";                                                            
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that contains the wrong poll value.                                                                
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_WrongPollValue)                                                                           
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        m_gameAddress, // Game address.                                                                                            
        0xaf, // Poll value.                                                                                                       
        13, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                
        0x00, 0x00, // Game number.                                                                                                
        0x01, // SAS meter code for Oasis gCO.                                                                                     
        0x12, 0x34, 0x56, 0x78, // Meter value.                                                                                    
        0x0d, // SAS meter code for Oasis mCoupV.                                                                                  
        0x12, 0x34, 0x56, 0x78, 0x90, // Meter value.                                                                              
        0xcc, 0xcc, // CRC field.                                                                                                  
    };
                                                                                                                                     
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= sizeof(responseBuffer)) << "GetResponseBufferSize";                                
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, sizeof(responseBuffer));                                                      
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), sizeof(responseBuffer));                                                      
    lp2f.SetReturnValue(SASPOLL_SUCCESS);                                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived();                                                                                                       
    ASSERT_EQ(SASPOLL_NACK, lp2f.GetReturnValue()) << "GetReturnValue";                                                            
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that is missing the last byte.                                                                     
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_ShortLength)                                                                              
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        m_gameAddress, // Game address.                                                                                            
        0x2f, // Poll value.                                                                                                       
        13, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                
        0x00, 0x00, // Game number.                                                                                                
        0x01, // SAS meter code for Oasis gCO.                                                                                     
        0x12, 0x34, 0x56, 0x78, // Meter value.                                                                                    
        0x0d, // SAS meter code for Oasis mCoupV.                                                                                  
        0x12, 0x34, 0x56, 0x78, 0x90, // Meter value.                                                                              
        0xcc, 0xcc, // CRC field.                                                                                                  
    };
    size_t responseBufferSize(sizeof(responseBuffer) - 1);                                                                         
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= responseBufferSize) << "GetResponseBufferSize";                                    
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, responseBufferSize);                                                          
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), responseBufferSize);                                                          
    lp2f.SetReturnValue(SASPOLL_SUCCESS);                                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived();                                                                                                       
    ASSERT_EQ(SASPOLL_NACK, lp2f.GetReturnValue()) << "GetReturnValue";                                                            
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that contains extra data after the CRC.                                                            
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_ExtraLength)                                                                              
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        m_gameAddress, // Game address.                                                                                            
        0x2f, // Poll value.                                                                                                       
        13, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                
        0x00, 0x00, // Game number.                                                                                                
        0x01, // SAS meter code for Oasis gCO.                                                                                     
        0x12, 0x34, 0x56, 0x78, // Meter value.                                                                                    
        0x0d, // SAS meter code for Oasis mCoupV.                                                                                  
        0x12, 0x34, 0x56, 0x78, 0x90, // Meter value.                                                                              
        0xcc, 0xcc, // CRC field.                                                                                                  
        0xdd,   
    };
                                                                                                                                   
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= sizeof(responseBuffer)) << "GetResponseBufferSize";                                
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, sizeof(responseBuffer));                                                      
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), sizeof(responseBuffer));                                                      
    lp2f.SetReturnValue(SASPOLL_SUCCESS);                                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived();                                                                                                       
    ASSERT_EQ(SASPOLL_NACK, lp2f.GetReturnValue()) << "GetReturnValue";                                                            
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that contains the wrong game number.                                                               
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_WrongGameNumber)                                                                          
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        m_gameAddress, // Game address.                                                                                            
        0x2f, // Poll value.                                                                                                       
        13, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                
        0x00, 0x01, // Game number.                                                                                                
        0x01, // SAS meter code for Oasis gCO.                                                                                     
        0x12, 0x34, 0x56, 0x78, // Meter value.                                                                                    
        0x0d, // SAS meter code for Oasis mCoupV.                                                                                  
        0x12, 0x34, 0x56, 0x78, 0x90, // Meter value.                                                                              
        0xcc, 0xcc, // CRC field.                                                                                                  
    };
                                                                                                                                     
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= sizeof(responseBuffer)) << "GetResponseBufferSize";                                
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, sizeof(responseBuffer));                                                      
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), sizeof(responseBuffer));                                                      
    lp2f.SetReturnValue(SASPOLL_SUCCESS);                                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived();                                                                                                       
    ASSERT_EQ(SASPOLL_NACK, lp2f.GetReturnValue()) << "GetReturnValue";                                                            
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that contains 0 meter.                                                                             
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_0Meters)                                                                                  
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        m_gameAddress, // Game address.                                                                                            
        0x2f, // Poll value.                                                                                                       
        2, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                 
        0x00, 0x00, // Game number.                                                                                                
        0xcc, 0xcc, // CRC field.                                                                                                  
    };
                                                                                                                                     
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= sizeof(responseBuffer)) << "GetResponseBufferSize";                                
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, sizeof(responseBuffer));                                                      
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), sizeof(responseBuffer));                                                      
    lp2f.SetReturnValue(SASPOLL_SUCCESS);                                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived();                                                                                                       
    ASSERT_EQ(SASPOLL_SUCCESS, lp2f.GetReturnValue()) << "GetReturnValue";                                                         
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that contains 1 meter.                                                                             
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_1Meter)                                                                                   
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        m_gameAddress, // Game address.                                                                                            
        0x2f, // Poll value.                                                                                                       
        8, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                 
        0x00, 0x00, // Game number.                                                                                                
        0x0d, // SAS meter code for Oasis mCoupV.                                                                                  
        0x12, 0x34, 0x56, 0x78, 0x90, // Meter value.                                                                              
        0xcc, 0xcc, // CRC field.                                                                                                  
    };
                                                                                                                                     
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
    metersExpected->SetMeter(mCoupV, 1234567890);                                                                                  
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= sizeof(responseBuffer)) << "GetResponseBufferSize";                                
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, sizeof(responseBuffer));                                                      
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), sizeof(responseBuffer));                                                      
    lp2f.SetReturnValue(SASPOLL_SUCCESS);                                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived();                                                                                                       
    ASSERT_EQ(SASPOLL_SUCCESS, lp2f.GetReturnValue()) << "GetReturnValue";                                                         
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that contains 2 meters.                                                                            
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_2Meters)                                                                                  
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        m_gameAddress, // Game address.                                                                                            
        0x2f, // Poll value.                                                                                                       
        13, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                
        0x00, 0x00, // Game number.                                                                                                
        0x01, // SAS meter code for Oasis gCO.                                                                                     
        0x12, 0x34, 0x56, 0x78, // Meter value.                                                                                    
        0x0d, // SAS meter code for Oasis mCoupV.                                                                                  
        0x12, 0x34, 0x56, 0x78, 0x90, // Meter value.                                                                              
        0xcc, 0xcc, // CRC field.                                                                                                  
    };
                                                                                                                                     
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
    metersExpected->SetMeter(gCO, 12345678);                                                                                       
    metersExpected->SetMeter(mCoupV, 1234567890);                                                                                  
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= sizeof(responseBuffer)) << "GetResponseBufferSize";                                
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, sizeof(responseBuffer));                                                      
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), sizeof(responseBuffer));                                                      
    lp2f.SetReturnValue(SASPOLL_SUCCESS);                                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived();                                                                                                       
    ASSERT_EQ(SASPOLL_SUCCESS, lp2f.GetReturnValue()) << "GetReturnValue";                                                         
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
                                                                                                                                   
/// <summary>                                                                                                                      
/// Code coverage and functional test for:                                                                                         
///     CLongPoll2FMessage::MeterReceived()                                                                                        
/// Test with a response buffer that contains 2 meters, except that the 2nd meter is only partially contained.                     
/// </summary>                                                                                                                     
TEST_F(LongPoll2FMessage, ResponseReceived_2MetersPartial)                                                                           
{                                                                                                                                  
    // Data.                                                                                                                       
    const byte responseBuffer[] = {                                                                                                
        m_gameAddress, // Game address.                                                                                            
        0x2f, // Poll value.                                                                                                       
        13, // Response data length == response length - 5 -- less 5 for gameAddress, poll value, data length, CRC.                
        0x00, 0x00, // Game number.                                                                                                
        0x01, // SAS meter code for Oasis gCO.                                                                                     
        0x12, 0x34, 0x56, 0x78, // Meter value.                                                                                    
        0x0d, // SAS meter code for Oasis mCoupV.                                                                                  
        0x12, 0x34, 0x56, 0x78, // Meter value with 5th byte missing.                                                              
        0xcc, 0xcc, // CRC field.                                                                                                  
    };
                                                                                                                                     
    // Setup.                                                                                                                      
    unique_ptr<CMeters> meters(GetDefaultMeters());                                                                                
    unique_ptr<CMeters> metersExpected(GetDefaultMeters());                                                                        
                                                                                                                                   
    CLongPoll2FMessage lp2f(m_gameAddress, meters.get());                                                                          
    lp2f.SetGameNumber(m_gameNumber);                                                                                              
    lp2f.Fill();                                                                                                                   
    ASSERT_TRUE(lp2f.GetResponseBuffer() != nullptr) << "GetResponseBuffer";                                                          
    ASSERT_TRUE(lp2f.GetResponseBufferSize() >= sizeof(responseBuffer)) << "GetResponseBufferSize";                                
    memcpy(lp2f.GetResponseBuffer(), responseBuffer, sizeof(responseBuffer));                                                      
    lp2f.SetResponseBuffer(lp2f.GetResponseBuffer(), sizeof(responseBuffer));                                                      
    lp2f.SetReturnValue(SASPOLL_SUCCESS);                                                                                          
                                                                                                                                   
    // Perform operation(s) and test(s).                                                                                           
    lp2f.ResponseReceived();                                                                                                       
    ASSERT_EQ(SASPOLL_NACK, lp2f.GetReturnValue()) << "GetReturnValue";                                                            
    AssertMetersAreEqual(*metersExpected, *meters);                                                                                
}                                                                                                                                  
