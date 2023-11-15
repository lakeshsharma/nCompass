#include "stdafx.h"

#include "../Sentinel/Config/ConfigFeatureFlagField.h"
#include "../Sentinel/Config/ConfigAllowFloorLockoutField.h"
#include "../Sentinel/Config/ConfigCardTimeoutField.h"
#include "../Sentinel/Config/ConfigCashTransferTrackingField.h"
#include "../Sentinel/Config/ConfigCasinoNameField.h"
#include "../Sentinel/Config/ConfigCasinoNumField.h"
#include "../Sentinel/Config/ConfigCommIDField.h"
#include "../Sentinel/Config/ConfigDenomField.h"
#include "../Sentinel/Config/ConfigFloorLockField.h"
#include "../Sentinel/Config/ConfigLossLimitField.h"
#include "../Sentinel/Config/ConfigPlayerProfileButtonField.h"
#include "UnitTestingUtils.h"
#include "TestUtil.h"

#include "GlobalDefs.h"
#include "gtest/gtest.h"

using namespace std;

class ConfigFieldUnitTests : public ::testing::Test
{
public:
    protected:
        virtual void SetUp() 
        {
        }

        virtual void TearDown() 
        {
        }      
};


/// <summary>
/// CConfigFeatureFlagField constructor test.
/// </summary>
TEST_F(ConfigFieldUnitTests, ConfigFeatureFlagFieldConstructorTest)
{
    //Setup
    const DWORD DEFAULT_FEATURE_FLAG = 0;
    const bool DEFAULT_ATOMIC_REDEMPTIONS = false;

    //Constructor Test
    CConfigFeatureFlagField configField;

    //Test
    ASSERT_TRUE(configField.GetFeatureFlags() == DEFAULT_FEATURE_FLAG && 
        configField.GetType() == FEATURE_FLAGS &&
        configField.AtomicRedemptions() == DEFAULT_ATOMIC_REDEMPTIONS)<< L"CConfigFeatureFlagField constructor failed.";                
}

/// <summary>
/// ConfigFeatureFlagField Functions Test
/// </summary>

TEST_F(ConfigFieldUnitTests, ConfigFeatureFlagFieldFunctionsTest)                                                                                                         
{                                                                                                                                                        
    //Setup                                                                                                                                              
    const int featureFlagLength = 2;                                                                                                                     
    const int atomicIndex = 0;                                                                                                                           
    byte featureFlag[featureFlagLength] = {0};                                                                                                           
                                                                                                                                                         
    typedef struct                                                                                                                                       
    {                                                                                                                                                    
        // Inputs.                                                                                                                                       
        byte featureFlag;                                                                                                                                
                                                                                                                                                         
        // Expected results.                                                                                                                             
        const bool ExpectedAtomicRedemptions;                                                                                                            
    } TestDataType;                                                                                                                                      
                                                                                                                                                         
    const TestDataType TEST_DATA[] =                                                                                                                     
    {                                                                                                                                                    
        { 1, true},                                                                                                                                      
        { 0, false},                                                                                                                                     
    };                                                                                                                                                   
    CConfigFeatureFlagField configField;                                                                                                                 
    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)                                                                                
    {                                                                                                                                                    
        featureFlag[atomicIndex] = TEST_DATA[testIndex].featureFlag;                                                                                     
        configField.SetFeatureFlags(featureFlag, 4);
                                                                                                                                                         
        //Test                                                                                                                                           
            bool areEqual = configField.AtomicRedemptions() == TEST_DATA[testIndex].ExpectedAtomicRedemptions;                                           
            string failedMsg;                                                                                                                           
            failedMsg = FormatString(_T("AtomicRedemptions was not setup properly value=%d expected=%d testIndex=%d)"),                                          
                configField.AtomicRedemptions(),                                                                                                         
                TEST_DATA[testIndex].ExpectedAtomicRedemptions,                                                                                          
                (int)testIndex); 
            ASSERT_TRUE(areEqual)<< failedMsg;                                                                                                                        
    }                                                                                                                                                    
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigAllowFloorLockoutField constructor test.                                                                                                      
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigAllowFloorLockoutFieldConstructorTest)                                                                                                 
{                                                                                                                                                        
    //Constructor Test                                                                                                                                   
    CConfigAllowFloorLockoutField configField;                                                                                                           
                                                                                                                                                         
    //Test   
    ASSERT_TRUE(configField.GetType() == ALLOW_FLOOR_LOCKOUT)<< L"CConfigAllowFloorLockoutField constructor failed.";                                                                                                                                            
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigAllowFloorLockoutField functions test.                                                                                                        
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigAllowFloorLockoutFieldFunctionsTest)                                                                                                   
{                                                                                                                                                        
    //Constructor Test                                                                                                                                   
    CConfigAllowFloorLockoutField configField;                                                                                                           
                                                                                                                                                         
    typedef struct                                                                                                                                       
    {                                                                                                                                                    
        const bool allowFloorLockOut;                                                                                                                    
                                                                                                                                                         
        // Expected results.                                                                                                                             
        const bool expectedFloorLockOut;                                                                                                                 
    } TestDataType;                                                                                                                                      
                                                                                                                                                         
    const TestDataType TEST_DATA[] =                                                                                                                     
    {                                                                                                                                                    
        { true, true},                                                                                                                                   
        { false, false},                                                                                                                                 
    };                                                                                                                                                   
                                                                                                                                                         
    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)                                                                                
    {                                                                                                                                                    
        configField.SetAllowFloorLockout(TEST_DATA[testIndex].allowFloorLockOut);                                                                        
                                                                                                                                                         
        //Test                                                                                                                                           
        bool areEqual = configField.GetAllowFloorLockout() == TEST_DATA[testIndex].expectedFloorLockOut;                                                 
            string failedMsg;                                                                                                                           
            failedMsg = FormatString(_T("AllowFloorLockout was not setup properly value=%d expected=%d testIndex=%d)"),                                          
                configField.GetAllowFloorLockout(),                                                                                                      
                TEST_DATA[testIndex].expectedFloorLockOut,                                                                                               
                (int)testIndex);
            ASSERT_TRUE(areEqual)<< failedMsg;                                                                                                                         
    }                                                                                                                                                    
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigCardTimeoutField constructor test.                                                                                                            
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigCardTimeoutFieldConstructorTest)                                                                                                       
{                                                                                                                                                        
    //Constructor Test                                                                                                                                   
    CConfigCardTimeoutField configField;                                                                                                                 
                                                                                                                                                         
    //Test 
    ASSERT_TRUE(configField.GetType() == CARD_TIMEOUT)<< L"CConfigCardTimeoutField constructor failed.";                                                                                                                                              
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigCardTimeoutField functions test.                                                                                                              
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigCardTimeoutFieldFunctionsTest)                                                                                                         
{                                                                                                                                                        
    //Setup                                                                                                                                              
    CConfigCardTimeoutField configField;                                                                                                                 
                                                                                                                                                         
    typedef struct                                                                                                                                       
    {                                                                                                                                                    
        const DWORD cardTimeout;                                                                                                                         
                                                                                                                                                         
        // Expected results.                                                                                                                             
        const DWORD expectedCardTimeout;                                                                                                                 
    } TestDataType;                                                                                                                                      
                                                                                                                                                         
    const TestDataType TEST_DATA[] =                                                                                                                     
    {                                                                                                                                                    
        { 1234, 1234},                                                                                                                                   
        { 4321, 4321},                                                                                                                                   
    };                                                                                                                                                   
                                                                                                                                                         
    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)                                                                                
    {                                                                                                                                                    
        configField.SetCardTimeout((byte*)&TEST_DATA[testIndex].cardTimeout, sizeof(TEST_DATA[testIndex].cardTimeout));                                  
                                                                                                                                                         
        //Test                                                                                                                                           
        bool areEqual = configField.GetCardTimeout() == TEST_DATA[testIndex].expectedCardTimeout;                                                        
            string failedMsg;                                                                                                                           
            failedMsg = FormatString(_T("AllowFloorLockout was not setup properly value=%d expected=%d testIndex=%d)"),                                          
                configField.GetCardTimeout(),                                                                                                            
                TEST_DATA[testIndex].expectedCardTimeout,                                                                                                
                (int)testIndex);                                                                                                                         
            ASSERT_TRUE(areEqual)<< failedMsg;
    }                                                                                                                                                    
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigCashTransferTrackingField constructor test.                                                                                                   
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigCashTransferTrackingFieldConstructorTest)                                                                                              
{                                                                                                                                                        
    //Constructor Test                                                                                                                                   
    CConfigCashTransferTrackingField configField;                                                                                                        
                                                                                                                                                         
    //Test 
    ASSERT_TRUE(configField.GetType() == CASH_TRANSFER_TRACKING)<< L"CConfigCashTransferTrackingField constructor failed.";                                                                                                                                              
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigCashTransferTrackingField functions test.                                                                                                     
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigCashTransferTrackingFieldFunctionsTest)                                                                                                
{                                                                                                                                                        
    //Setup                                                                                                                                              
    CConfigCashTransferTrackingField configField;                                                                                                        
                                                                                                                                                         
    typedef struct                                                                                                                                       
    {                                                                                                                                                    
        const bool cashTransferTracking;                                                                                                                 
                                                                                                                                                         
        // Expected results.                                                                                                                             
        const bool expectedCashTransferTracking;                                                                                                         
    } TestDataType;                                                                                                                                      
                                                                                                                                                         
    const TestDataType TEST_DATA[] =                                                                                                                     
    {                                                                                                                                                    
        { true, true},                                                                                                                                   
        { false, false},                                                                                                                                 
    };                                                                                                                                                   
                                                                                                                                                         
    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)                                                                                
    {                                                                                                                                                    
        configField.SetCashTransferTracking(TEST_DATA[testIndex].cashTransferTracking);                                                                  
                                                                                                                                                         
        //Test                                                                                                                                           
        bool areEqual = configField.CashTransferTracking() == TEST_DATA[testIndex].expectedCashTransferTracking;                                         
            string failedMsg;                                                                                                                           
            failedMsg = FormatString(_T("Cash Transfer Tracking was not setup properly value=%d expected=%d testIndex=%d)"),                                     
                configField.CashTransferTracking(),                                                                                                      
                TEST_DATA[testIndex].expectedCashTransferTracking,                                                                                       
                (int)testIndex);
            ASSERT_TRUE(areEqual)<< failedMsg;                                                                                                                         
    }                                                                                                                                                    
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigCasinoNameField constructor test.                                                                                                             
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigCasinoNameFieldConstructorTest)                                                                                                        
{                                                                                                                                                        
    //Constructor Test                                                                                                                                   
    CConfigCasinoNameField configField;                                                                                                                  
                                                                                                                                                         
    //Test
    ASSERT_TRUE(configField.GetType() == CASINO_NAME)<< L"CConfigCasinoNameField constructor failed.";                                                                                                                                               
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigCasinoNameField function test.                                                                                                                
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigCasinoNameFieldFunctionsTest)                                                                                                          
{                                                                                                                                                        
    //Setup                                                                                                                                              
    CConfigCasinoNameField configField;                                                                                                                  
                                                                                                                                                         
    typedef struct                                                                                                                                       
    {                                                                                                                                                    
        string casinoName;                                                                                                                              
                                                                                                                                                         
        // Expected results.                                                                                                                             
        const LPCTSTR expectedCasinoName;                                                                                                                
    } TestDataType;                                                                                                                                      
                                                                                                                                                         
    const TestDataType TEST_DATA[] =                                                                                                                     
    {                                                                                                                                                    
        { "CasinoA", "CasinoA" },                                                                                                                      
        { "Casino11", "Casino11" }                                                                                                                     
    };                                                                                                                                                   
                                                                                                                                                         
    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)                                                                                
    {                                                                                                                                                    
        byte* casinoNameBuffer(new byte[TEST_DATA[testIndex].casinoName.length()]);
        for(int nameIndex = 0; nameIndex < TEST_DATA[testIndex].casinoName.length(); ++nameIndex)                                                     
        {                                                                                                                                                
            casinoNameBuffer[nameIndex] = (byte)TEST_DATA[testIndex].casinoName.at(nameIndex);                                                        
        }                                                                                                                                                
        configField.SetCasinoName(casinoNameBuffer, TEST_DATA[testIndex].casinoName.length());    

        string failedMsg;
        failedMsg = FormatString(_T("testIndex=%d"), testIndex);
        ASSERT_EQ(TEST_DATA[testIndex].expectedCasinoName, configField.GetCasinoName())<< failedMsg;                                                                                                                                                 
                                                                                                                                                         
        delete[] casinoNameBuffer;                                                                                                                       
    }                                                                                                                                                    
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigCasinoNumField constructor test.                                                                                                              
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigCasinoNumFieldConstructorTest)                                                                                                         
{                                                                                                                                                        
    //Constructor Test                                                                                                                                   
    CConfigCasinoNumField configField;                                                                                                                   
                                                                                                                                                         
    //Test 
    ASSERT_TRUE(configField.GetType() == CASINO_NUMBER)<< L"CConfigCasinoNumField constructor failed.";                                                                                                                                              
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigCasinoNumField functions test.                                                                                                                
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigCasinoNumFieldFunctionsTest)                                                                                                           
{                                                                                                                                                        
    //Setup                                                                                                                                              
    const byte casinoNumber = 123;                                                                                                                       
    CConfigCasinoNumField configField;                                                                                                                   
    configField.SetCasinoNumber(casinoNumber);                                                                                                           
                                                                                                                                                         
    //Test 
    ASSERT_TRUE(configField.GetCasinoNumber() == casinoNumber)<< L"CConfigCasinoNumField function failed.";                                                                                                                                              
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigCommIDField constructor test.                                                                                                                 
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigCommIDFieldConstructorTest)                                                                                                            
{                                                                                                                                                        
    //Constructor Test                                                                                                                                   
    CConfigCommIDField configField;                                                                                                                      
                                                                                                                                                         
    //Test 
    ASSERT_TRUE(configField.GetType() == COMM_ID)<< L"CConfigCommIDField constructor failed.";                                                                                                                                              
}                                                                                                                                                        
                                                                                                                                                         
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigCommIDField functions test.                                                                                                                   
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigCommIDFieldFunctionsTest)                                                                                                              
{                                                                                                                                                        
    //Setup                                                                                                                                              
    const byte commID = 111;                                                                                                                             
    CConfigCommIDField configField;                                                                                                                      
                                                                                                                                                         
    configField.SetCommID(commID);                                                                                                                       
                                                                                                                                                         
    //Test                                                                                                                                               
    ASSERT_TRUE(configField.GetCommID() == commID)<< L"CConfigCommIDField functions failed.";
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigDenomField constructor test.                                                                                                                  
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigDenomFieldConstructorTest)                                                                                                             
{                                                                                                                                                        
    //Constructor Test                                                                                                                                   
    CConfigDenomField configField;                                                                                                                       
                                                                                                                                                         
    //Test                                                                                                                                               
    ASSERT_TRUE(configField.GetType() == DENOMINATION)<< L"CConfigDenomField constructor failed.";
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigDenomField function test.                                                                                                                     
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigDenomFieldFunctionsTest)                                                                                                               
{                                                                                                                                                        
    //Setup                                                                                                                                              
    WORD denom = 25;                                                                                                                                     
    CConfigDenomField configField;                                                                                                                       
                                                                                                                                                         
    configField.SetDenom((byte*)&denom, sizeof(denom));                                                                                                  
                                                                                                                                                         
    //Test 
    ASSERT_TRUE(configField.GetDenom() == denom)<< L"CConfigDenomField functions failed.";                                                                                                                                              
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigFloorLockField constructor test.                                                                                                              
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigFloorLockFieldConstructorTest)                                                                                                         
{                                                                                                                                                        
    //Constructor Test                                                                                                                                   
    CConfigFloorLockField configField;                                                                                                                   
                                                                                                                                                         
    //Test                                                                                                                                               
    ASSERT_TRUE(configField.GetType() == FLOOR_LOCK)<< L"CConfigFloorLockField constructor failed.";            
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigFloorLockField functions test.                                                                                                                
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigFloorLockFieldFunctionsTest)                                                                                                           
{                                                                                                                                                        
    //Setup                                                                                                                                              
    CConfigFloorLockField configField;                                                                                                                   
    typedef struct                                                                                                                                       
    {                                                                                                                                                    
        const bool allowFloorLock;                                                                                                                       
                                                                                                                                                         
        // Expected results.                                                                                                                             
        const bool expectedFloorLock;                                                                                                                    
    } TestDataType;                                                                                                                                      
                                                                                                                                                         
    const TestDataType TEST_DATA[] =                                                                                                                     
    {                                                                                                                                                    
        { true, true},                                                                                                                                   
        { false, false},                                                                                                                                 
    };                                                                                                                                                   
                                                                                                                                                         
    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)                                                                                
    {                                                                                                                                                    
        configField.SetFloorLock(TEST_DATA[testIndex].allowFloorLock);                                                                                   
                                                                                                                                                         
        //Test                                                                                                                                           
        bool areEqual = configField.GetFloorLock() == TEST_DATA[testIndex].expectedFloorLock;                                                            
            string failedMsg;                                                                                                                           
            failedMsg = FormatString(_T("AllowFloorLock was not setup properly value=%d expected=%d testIndex=%d)"),                                             
                configField.GetFloorLock(),                                                                                                              
                TEST_DATA[testIndex].expectedFloorLock,                                                                                                  
                (int)testIndex);    
            ASSERT_TRUE(areEqual)<< failedMsg;                                                                                                                     
    }                                                                                                                                                    
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigLossLimitField constructor test.                                                                                                              
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigLossLimitFieldConstructorTest)                                                                                                         
{                                                                                                                                                        
    //Constructor Test                                                                                                                                   
    CConfigLossLimitField configField;                                                                                                                   
                                                                                                                                                         
    //Test
    ASSERT_TRUE(configField.GetType() == LOSS_LIMIT)<< L"CConfigLossLimitField constructor failed.";                                                                                                                                               
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigLossLimitField functions test.                                                                                                                
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigLossLimitFieldFunctionsTest)                                                                                                           
{                                                                                                                                                        
    //Setup                                                                                                                                              
    CConfigLossLimitField configField;                                                                                                                   
                                                                                                                                                         
    typedef struct                                                                                                                                       
    {                                                                                                                                                    
        const bool lossLimitActive;                                                                                                                      
        const DWORD lossLimitSessionNumber;                                                                                                              
        const WORD lossLimitSessionAmount;                                                                                                               
        const WORD lossLimitBillValidatorOffAmount;                                                                                                      
    } TestDataType;                                                                                                                                      
                                                                                                                                                         
    const TestDataType TEST_DATA[] =                                                                                                                     
    {                                                                                                                                                    
        { 1, 2, 3, 4},                                                                                                                                   
        { 0, 4, 3, 2},                                                                                                                                   
    };                                                                                                                                                   
                                                                                                                                                         
    const int sizeOfBufferInBytes = sizeof(TestDataType);                                                                                                
    byte* buffer = new byte[sizeOfBufferInBytes];                                                                                                        
    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)                                                                                
    {                                                                                                                                                    
        byte* tempBuffer(buffer);                                                                                                                        
        memset(buffer, 0, sizeOfBufferInBytes);                                                                                                          
                                                                                                                                                         
        memcpy(tempBuffer, &TEST_DATA[testIndex].lossLimitActive,                                                                                        
            sizeof(TEST_DATA[testIndex].lossLimitActive));                                                                                               
        tempBuffer = tempBuffer + sizeof(TEST_DATA[testIndex].lossLimitActive);                                                                          
                                                                                                                                                         
        memcpy(tempBuffer, &TEST_DATA[testIndex].lossLimitSessionNumber,                                                                                 
            sizeof(TEST_DATA[testIndex].lossLimitSessionNumber));                                                                                        
        tempBuffer = tempBuffer + sizeof(TEST_DATA[testIndex].lossLimitSessionNumber);                                                                   
                                                                                                                                                         
        memcpy(tempBuffer, &TEST_DATA[testIndex].lossLimitSessionAmount,                                                                                 
            sizeof(TEST_DATA[testIndex].lossLimitSessionAmount));                                                                                        
        tempBuffer = tempBuffer + sizeof(TEST_DATA[testIndex].lossLimitSessionAmount);                                                                   
                                                                                                                                                         
        memcpy(tempBuffer, &TEST_DATA[testIndex].lossLimitBillValidatorOffAmount,                                                                        
            sizeof(TEST_DATA[testIndex].lossLimitBillValidatorOffAmount));                                                                               
        tempBuffer = tempBuffer + sizeof(TEST_DATA[testIndex].lossLimitBillValidatorOffAmount);                                                          
                                                                                                                                                         
        configField.SetLossLimitOptions(buffer, sizeOfBufferInBytes);                                                                                    
                                                                                                                                                         
        //Test
        ASSERT_TRUE(configField.GetLossLimitActive() == TEST_DATA[testIndex].lossLimitActive)<< L"CConfigLossLimitField Setting lossLimitActive failed.";                                                                                                                                           
        ASSERT_TRUE(configField.GetLossLimitSessionNumber() == TEST_DATA[testIndex].lossLimitSessionNumber)<< L"CConfigLossLimitField Setting lossLimitSessionNumber failed.";
        ASSERT_TRUE(configField.GetLossLimitSessionAmount() == TEST_DATA[testIndex].lossLimitSessionAmount)<< L"CConfigLossLimitField Setting lossLimitSessionAmount failed.";
        ASSERT_TRUE(configField.GetLossLimitBillValidatorOffAmount() == TEST_DATA[testIndex].lossLimitBillValidatorOffAmount)<< L"CConfigLossLimitField Setting lossLimitBillValidatorOffAmount failed.";
    }                                                                                                                                                    
                                                                                                                                                         
    //Cleanup                                                                                                                                            
    delete []buffer;                                                                                                                                     
}                                                                                                                                                        
                                                                                                                                                         
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigPlayerProfileButtonField constructor test.                                                                                                    
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigPlayerProfileButtonFieldConstructorTest)                                                                                               
{                                                                                                                                                        
    //Constructor Test                                                                                                                                   
    CConfigPlayerProfileButtonField configField;                                                                                                         
                                                                                                                                                         
    //Test
    ASSERT_TRUE(configField.GetType() == PLAYER_PROFILE_BUTTON)<< L"CConfigPlayerProfileButtonField constructor failed.";                                                                                                                                               
}                                                                                                                                                        
                                                                                                                                                         
/// <summary>                                                                                                                                            
/// CConfigPlayerProfileButtonField functions test.                                                                                                      
/// </summary>                                                                                                                                           
TEST_F(ConfigFieldUnitTests, ConfigPlayerProfileButtonFieldFunctionsTest)                                                                                                 
{                                                                                                                                                        
    //Setup                                                                                                                                              
    CConfigPlayerProfileButtonField configField;                                                                                                         
                                                                                                                                                         
    typedef struct                                                                                                                                       
    {                                                                                                                                                    
        const bool displayPlayerProfileButton;                                                                                                           
                                                                                                                                                         
        // Expected results.                                                                                                                             
        const bool expecteddisplayPlayerProfileButton;                                                                                                   
    } TestDataType;                                                                                                                                      
                                                                                                                                                         
    const TestDataType TEST_DATA[] =                                                                                                                     
    {                                                                                                                                                    
        { true, true },                                                                                                                                  
        { false, false },                                                                                                                                
    };                                                                                                                                                   
                                                                                                                                                         
    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)                                                                                
    {                                                                                                                                                    
        configField.SetDisplayPlayerProfileButton(TEST_DATA[testIndex].displayPlayerProfileButton);                                                      
                                                                                                                                                         
        //Test                                                                                                                                           
        bool areEqual = configField.DisplayPlayerProfileButton() == TEST_DATA[testIndex].expecteddisplayPlayerProfileButton;                             
        string failedMsg;                                                                                                                               
        failedMsg = FormatString(_T("Display Player Profile Button flag was not setup properly value=%d expected=%d testIndex=%d)"),                             
            configField.DisplayPlayerProfileButton(),                                                                                                    
            TEST_DATA[testIndex].expecteddisplayPlayerProfileButton,                                                                                     
            (int)testIndex);    
        ASSERT_TRUE(areEqual)<< failedMsg;                                                                                                                         
    }                                                                                                                                                    
}                                                                                                                                                        