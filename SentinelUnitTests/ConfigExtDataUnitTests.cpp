#include "stdafx.h"
#include "../Sentinel/Config/ConfigExtData.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingUtils.h"

#include "gtest/gtest.h"

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a class initialization method.
#define AssertResult3i(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, 0, __TFUNCTION__, __LINE__);
#define AssertResult2i(result, expected) AssertResult3i(result, expected, _T(#result))

// Expected result(s).
static const WORD m_sessionActiveNegligibleCreditsTimeoutSecondsDefault = 10;
static const WORD m_sessionActiveNonNegligibleCreditsTimeoutSecondsDefault = 300;                                                                   
                                                                                                                                                    
static const LPCTSTR m_sessionActiveNegligibleCreditsTimeoutSecondsName = "SessionActiveNegligibleCreditsTimeoutSeconds";                           
static const LPCTSTR m_sessionActiveNonNegligibleCreditsTimeoutSecondsName = "SessionActiveNonNegligibleCreditsTimeoutSeconds";                     


class ConfigExtDataUnitTests : public ::testing::Test
{
public:
    // Class member used by test (data) methods.
	int m_testIndex;
    protected:
        virtual void SetUp() 
        {
            m_testIndex = 0;
            AssertResult2i(m_sessionActiveNegligibleCreditsTimeoutSecondsDefault, CConfigExtData::SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
			AssertResult2i(m_sessionActiveNonNegligibleCreditsTimeoutSecondsDefault, CConfigExtData::SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS);
        }

        virtual void TearDown() 
        {
        }      
};

TEST_F(ConfigExtDataUnitTests, ConfigExtData_AddTest)
{
    CConfigExtData configData;

    // Add LPCTSTR Test
    LPCTSTR test1bKey = _T("Test1");
    bool test1bData = true;

    configData.AddConfig(test1bKey, test1bData);

    bool expectedb;
    configData.GetConfigValue(test1bKey, expectedb);
    ASSERT_TRUE(test1bData && expectedb)<< L"Added config data (bool) but could not find it again";

    LPCTSTR test1wKey = _T("Test1w");
    WORD test1wData = 12345;
    configData.AddConfig(test1wKey, test1wData);

    bool expectedw;
    configData.GetConfigValue(test1wKey, expectedw);
    ASSERT_TRUE(test1wData && expectedw)<< L"Added config data (WORD) but could not find it again";
}


TEST_F(ConfigExtDataUnitTests, ConfigExtData_AddTest2)                                                                                                          
{                                                                                                                                            
    CConfigExtData configData;                                                                                                               
                                                                                                                                             
    // Add LPCTSTR Test                                                                                                                      
    LPCTSTR test1bKey = _T("Test1");                                                                                                         
    bool test1bData = true;                                                                                                                  
                                                                                                                                             
    configData.AddConfig(test1bKey, test1bData);                                                                                             
                                                                                                                                             
    // add with the same key again.  Should not throw exception and not add another item                                                     
    configData.AddConfig(test1bKey, test1bData);                                                                                             
                                                                                                                                             
    bool expectedb;                                                                                                                          
    configData.GetConfigValue(test1bKey, expectedb);
    ASSERT_TRUE(test1bData && expectedb)<< L"Added config data (bool) but could not find it again";                                                                                         
}                                                                                                                                            
                                                                                                                                            
TEST_F(ConfigExtDataUnitTests, ConfigExtData_FindTest) // Insure retrieval is by Key Value not by reference                                                     
{                                                                                                                                            
    CConfigExtData configData;                                                                                                               
                                                                                                                                             
    LPCTSTR test1bKey = _T("Test1b");                                                                                                        
    LPCTSTR testCopy1bKey = _T("Test1b");                                                                                                    
                                                                                                                                             
    bool test1bData = true;                                                                                                                  
    configData.AddConfig(test1bKey, test1bData);                                                                                             
                                                                                                                                             
    bool expectedb;                                                                                                                          
    configData.GetConfigValue(testCopy1bKey, expectedb);   
    ASSERT_TRUE(test1bData && expectedb)<< L"Added config data (bool) but could not find it again by key value";                                                                                 
                                                                                                                                             
    LPCTSTR test1wKey = _T("Test1w");                                                                                                        
    LPCTSTR testCopy1wKey = _T("Test1w");                                                                                                    
                                                                                                                                             
    WORD test1wData = 12345;                                                                                                                 
    configData.AddConfig(test1wKey, test1wData);                                                                                             
                                                                                                                                             
    bool expectedw;                                                                                                                          
    configData.GetConfigValue(testCopy1wKey, expectedw); 
    ASSERT_TRUE(test1wData && expectedw)<< L"Added config data (WORD) but could not find it again by key value";                                                                                    
                                                                                                                                             
    LPCTSTR test1pKey = _T("Test1p");                                                                                                        
    LPCTSTR testCopy1pKey = _T("Test1p");                                                                                                    
                                                                                                                                             
    WORD test1pData = 12345;                                                                                                                 
    configData.AddConfig(test1pKey, test1pData);                                                                                             
                                                                                                                                             
    bool expectedp;                                                                                                                          
    configData.GetConfigValue(testCopy1pKey, expectedp);  
    ASSERT_TRUE(test1pData && expectedp)<< L"Added config data (WORD) but could not find it again by key value";                                                                                   
}                                                                                                                                            
                                                                                                                                             
TEST_F(ConfigExtDataUnitTests, ConfigExtData_SetFromFileFalseTest)                                                                                              
{                                                                                                                                            
    CConfigExtData configData;                                                                                                               
                                                                                                                                             
    LPCTSTR test1bKey = _T("Test1b");                                                                                                        
    LPCTSTR testCopy1bKey = _T("Test1b");                                                                                                    
                                                                                                                                             
    bool test1bData = true;                                                                                                                  
    configData.AddConfig(test1bKey, test1bData);                                                                                             
                                                                                                                                             
    bool expectedb = false;                                                                                                                  
    bool expectedFromFile(false);                                                                                                            
    configData.SetConfigValue(testCopy1bKey, expectedb, expectedFromFile);                                                                   
                                                                                                                                             
    bool actualb;                                                                                                                            
    bool acutalFromFile = configData.GetConfigValue(testCopy1bKey, actualb);    
    ASSERT_TRUE(expectedb == actualb)<< L"Setting config data (bool) did not set correctly";                                                             
    ASSERT_TRUE(expectedFromFile == acutalFromFile)<< L"Setting expectedFromFile did not set correctly";                                           
                                                                                                                                             
}                                                                                                                                            
                                                                                                                                             
TEST_F(ConfigExtDataUnitTests, ConfigExtData_SetFromFileTrueTest)                                                                                               
{                                                                                                                                            
    CConfigExtData configData;                                                                                                               
                                                                                                                                             
    LPCTSTR test1bKey = _T("Test1b");                                                                                                        
    LPCTSTR testCopy1bKey = _T("Test1b");                                                                                                    
                                                                                                                                             
    bool test1bData = true;                                                                                                                  
    configData.AddConfig(test1bKey, test1bData);                                                                                             
                                                                                                                                             
    bool expectedb = false;                                                                                                                  
    bool expectedFromFile(true);                                                                                                             
    configData.SetConfigValue(testCopy1bKey, expectedb, expectedFromFile);                                                                   
                                                                                                                                             
    bool actualb;                                                                                                                            
    bool acutalFromFile = configData.GetConfigValue(testCopy1bKey, actualb);  
    ASSERT_TRUE(expectedb == actualb)<< L"Setting config data (bool) did not set correctly";                                                               
    ASSERT_TRUE(expectedFromFile == acutalFromFile)<< L"Setting expectedFromFile did not set correctly";                                            
                                                                                                                                             
}                                                                                                                                            
                                                                                                                                             
TEST_F(ConfigExtDataUnitTests, ConfigExtData_SetFromFileFalseTest2)                                                                                             
{                                                                                                                                            
    CConfigExtData configData;                                                                                                               
    LPCTSTR testNotAddedbKey = _T("Not added yet");                                                                                          
                                                                                                                                             
    bool expectedb = false;                                                                                                                  
    bool expectedFromFile(false);                                                                                                            
    // Should not throw exception                                                                                                            
    configData.SetConfigValue(testNotAddedbKey, expectedb, expectedFromFile);                                                                
                                                                                                                                             
    // Verify get value as expected.                                                                                                         
    bool actualb(!expectedb);                                                                                                                
    bool acutalFromFile = configData.GetConfigValue(testNotAddedbKey, actualb); 
    ASSERT_TRUE(!expectedb == actualb)<< L"Getting config data (bool) did not get correctly";                                                             
    ASSERT_FALSE(acutalFromFile)<< L"Getting expectedFromFile did not get correctly";
}                                                                                                                                            
                                                                                                                                             
TEST_F(ConfigExtDataUnitTests, ConfigExtData_SetFromFileTrueTest2)                                                                                              
{                                                                                                                                            
    CConfigExtData configData;                                                                                                               
                                                                                                                                             
    LPCTSTR testNotAddedbKey = _T("Not added yet");                                                                                          
                                                                                                                                             
    bool expectedb = false;                                                                                                                  
    bool expectedFromFile(true);                                                                                                             
    // Should not throw exception                                                                                                            
    configData.SetConfigValue(testNotAddedbKey, expectedb, expectedFromFile);                                                                
                                                                                                                                             
    // Verify get value as expected.                                                                                                         
    bool actualb(!expectedb);                                                                                                                
    bool acutalFromFile = configData.GetConfigValue(testNotAddedbKey, actualb);
    ASSERT_TRUE(!expectedb == actualb)<< L"Getting config data (bool) did not get correctly";                                                              
    ASSERT_FALSE(acutalFromFile)<< L"Getting expectedFromFile did not get correctly";
}                                                                                                                                            
                                                                                                                                             
TEST_F(ConfigExtDataUnitTests, ConfigExtData_HasTest)                                                                                                           
{                                                                                                                                            
    CConfigExtData configData;                                                                                                               
    LPCTSTR testKey = _T("Test1");                                                                                                           
                                                                                                                                             
    bool expectedFalseBool = configData.HasConfigNameBool(testKey);
    ASSERT_FALSE(expectedFalseBool)<< L"Says data key (bool) exists when it does not";                                                                          
                                                                                                                                             
    bool expectedFalseWord = configData.HasConfigNameWORD(testKey);
    ASSERT_FALSE(expectedFalseBool)<< L"Says data key (WORD) exists when it does not";                                                                          
}                                                                                                                                            
                                                                                                                                             
TEST_F(ConfigExtDataUnitTests, ConfigExtData_DefaultsTest)                                                                                                      
{                                                                                                                                            
    CConfigExtData configData;                                                                                                               
	bool boolValue;
	WORD wordValue;                                                                                                                               

    configData.GetConfigValue(CConfigExtData::EMPLOYEE_OVER_PLAYER_IDLE_CARD_OUT, boolValue);
    ASSERT_TRUE(CConfigExtData::DEFAULT_EMPLOYEE_OVER_PLAYER_IDLE_CARD_OUT == boolValue)<< L"Player Idle default not retrieved";
                                                                                                                                             
    configData.GetConfigValue(CConfigExtData::PLAYER_RECARD_EMPLOYEE_CARD_OUT, boolValue);
    ASSERT_TRUE(CConfigExtData::DEFAULT_PLAYER_RECARD_EMPLOYEE_CARD_OUT == boolValue)<< L"Player Recard out default not retrieved";
                                                                                                                                             
    configData.GetConfigValue(CConfigExtData::CUSTOM_BUTTON_1_ENABLE, boolValue);
    ASSERT_TRUE(CConfigExtData::DEFAULT_CUSTOM_BUTTON_1_ENABLE == boolValue)<< L"Custom Button 1 Enable default not retrieved";
                                                                                                                                             
    configData.GetConfigValue(CConfigExtData::CUSTOM_BUTTON_2_ENABLE, boolValue);
    ASSERT_TRUE(CConfigExtData::DEFAULT_CUSTOM_BUTTON_2_ENABLE == boolValue)<< L"Custom Button 2 Enable default not retrieved";
                                                                                                                                             
    configData.GetConfigValue(CConfigExtData::NEGLIGIBLE_CREDITS_THRESHOLD, wordValue);
    ASSERT_TRUE(CConfigExtData::DEFAULT_NEGLIGIBLE_CREDITS_THRESHOLD == wordValue)<< L"Negligible Credit Threshold default not retrieved";
                                                                                                                                             
    configData.GetConfigValue(CConfigExtData::EMPLOYEE_IDLE_CARD_OUT_SECONDS, wordValue);
    ASSERT_TRUE(CConfigExtData::DEFAULT_EMPLOYEE_IDLE_CARD_OUT_SECONDS == wordValue)<< L"Employee Idle Card Out Seconds default not retrieved";
                                                                                                                                             
    configData.GetConfigValue(CConfigExtData::NOTIFICATION_SHOW_PROMOTION_TIME, wordValue);
    ASSERT_TRUE(CConfigExtData::DEFAULT_NOTIFICATION_SHOW_PROMOTION == wordValue)<< L"Show Promotion Time default not retrieved";
                                                                                                                                             
    configData.GetConfigValue(CConfigExtData::NOTIFICATION_PLAYER_TRACKING_TIME, wordValue);
    ASSERT_TRUE(CConfigExtData::DEFAULT_NOTIFICATION_PLAYER_TRACKING == wordValue)<< L"Player Tracking Time default not retrieved";
                                                                                                                                             
    configData.GetConfigValue(CConfigExtData::CUSTOM_MENU_BUTTON_1_COUNT, wordValue);
    ASSERT_TRUE(CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_1_COUNT == wordValue) << L"Custom Menu Button 1 default not retrieved";
                                                                                                                             
    configData.GetConfigValue(CConfigExtData::CUSTOM_MENU_BUTTON_2_COUNT, wordValue);
    ASSERT_TRUE(CConfigExtData::DEFAULT_CUSTOM_MENU_BUTTON_2_COUNT == wordValue) << L"Custom Menu Button 2 default not retrieved";

	configData.GetConfigValue(CConfigExtData::REBOOT_UI_RESTART_OVERRIDE_ENABLE, boolValue);
	ASSERT_TRUE(CConfigExtData::DEFAULT_REBOOT_UI_RESTART_OVERRIDE_ENABLE == boolValue);

	configData.GetConfigValue(CConfigExtData::GAME_LIST_CHUNK_SIZE, wordValue);
	ASSERT_TRUE(DEFAULT_GAME_LIST_CHUNK_SIZE == wordValue);

	configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_NRICH_LEVEL_PRIORITY, wordValue);
	ASSERT_TRUE(CConfigExtData::DEFAULT_NRICH_PRIORITY == wordValue);

	configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_SPLASHDOWN_PRIORITY, wordValue);
	ASSERT_TRUE(CConfigExtData::DEFAULT_SPLASHDOWN_PRIORITY == wordValue);

	configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_PROGRESSIVE_ODOMETER_PRIORITY, wordValue);
	ASSERT_TRUE(CConfigExtData::DEFAULT_PROGRESSIVE_ODOMETER_PRIORITY == wordValue);

	configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_NOTIFICATION_BOX_PRIORITY, wordValue);
	ASSERT_TRUE(CConfigExtData::DEFAULT_NOTIFICATION_BOX_PRIORITY == wordValue);

	configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_PLAYXGETY_PRIORITY, wordValue);
	ASSERT_TRUE(CConfigExtData::DEFAULT_PTE_PRIORITY == wordValue);

	configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_PTE_PRIORITY, wordValue);
	ASSERT_TRUE(CConfigExtData::DEFAULT_PTE_PRIORITY == wordValue);

	configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_SCATTER_COUNTDOWN_TIMER_PRIORITY, wordValue);
	ASSERT_TRUE(CConfigExtData::DEFAULT_SCATTER_COUNTDOWN_TIMER_PRIORITY == wordValue);

	configData.GetConfigValue(CConfigExtData::BONUS_COUNTER_PRIZE_PRIORITY, wordValue);
	ASSERT_TRUE(CConfigExtData::DEFAULT_PRIZE_ODOMETER_PRIORITY == wordValue);
}                                                                                                                                            
                                                                                                                                             
TEST_F(ConfigExtDataUnitTests, ConfigExtData_AccessorTest)                                                                                                      
{                                                                                                                                            
    CConfigExtData configData; 
    ASSERT_TRUE(configData.GetBoolConfigs().size() == 5)<< L"Wrong number of bool configs on instantiation";                                                                                                              
    cout << "=ConfigExtData_AccessorTest=" << configData.GetWordConfigs().size() << endl;
    ASSERT_TRUE(configData.GetWordConfigs().size() == 18)<< L"Wrong number of word configs on instantiation";
                                                                                                                                             
    LPCTSTR test1bKey = _T("Test1b");                                                                                                        
    LPCTSTR test1wKey = _T("Test1w");                                                                                                        
    WORD testData = 12345;                                                                                                                   
                                                                                                                                             
    configData.AddConfig(test1bKey, true);                                                                                                   
    configData.AddConfig(test1wKey, testData, testData, testData);                                                                           

    ASSERT_TRUE(configData.GetBoolConfigs().size() == 6)<< L"Wrong number of bool configs after adding";                                                                                                                                         
    ASSERT_TRUE(configData.GetWordConfigs().size() == 19)<< L"Wrong number of word configs after adding";

}                                                                                                                                            

// SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS test(s).                                                                                
                                                                                                                                             
TEST_F(ConfigExtDataUnitTests, SESSION_ACTIVE_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS_DefaultTest)                                                                   
{                                                                                                                                            
    CConfigExtData configData;                                                                                                               
    WORD value(55555);                                                                                                                       
         
    AssertResult2(configData.GetConfigValue(m_sessionActiveNegligibleCreditsTimeoutSecondsName, value), false);
	AssertResult2(value, m_sessionActiveNegligibleCreditsTimeoutSecondsDefault);  
}                                                                                                                                            
                                                                                                                                             
// SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS test(s).                                                                            
                                                                                                                                             
TEST_F(ConfigExtDataUnitTests, SESSION_ACTIVE_NON_NEGLIGIBLE_CREDITS_TIMEOUT_SECONDS_DefaultTest)                                                               
{                                                                                                                                            
    CConfigExtData configData;                                                                                                               
    WORD value(55555);                                                                                                                       
          
    AssertResult2(configData.GetConfigValue(m_sessionActiveNonNegligibleCreditsTimeoutSecondsName, value), false);
    AssertResult2(value, m_sessionActiveNonNegligibleCreditsTimeoutSecondsDefault);
}                                                                                                                                            