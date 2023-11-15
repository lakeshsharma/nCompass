#include "stdafx.h"
#include "Config.h"

#include "gtest/gtest.h"

class ConfigUniversalEnterpriseUnitTests : public ::testing::Test
{
    public:
		/// <summary>
		/// Sets the nvram invalid, so CMemoryStorage::IntegrityCheck() returns false.
		/// </summary>
		void SetNvramInvalid()
		{
			CMemoryStorage::SetInactiveLocation(SPRINKLE1_OFFSET, ~SPRINKLE_VALUE);
		}

		/// <summary>
		/// Write data to NVRAM..
		/// </summary>
		/// <param name="nvramOffset">IN - The NVRAM offset to write to.</param>
		/// <param name="data">IN - a pointer to the data to be written.</param>
		/// <param name="dataLength">IN - The length of data to be written.</param>
		void NvramWrite(DWORD nvramOffset, const void *data, UINT dataLength)
		{
			const byte *dataBuffer = reinterpret_cast <const byte *>(data);
			for (DWORD index = 0; index < dataLength; ++index)
			{
				CMemoryStorage::SetInactiveLocation(nvramOffset + index, *dataBuffer);
				++dataBuffer;
			}
		}

		/// <summary>
		/// Sets the NVRAM map version.
		/// </summary>
		/// <param name="nvramVersion">IN - The NVRAM version value.</param>
		void SetNvramVersion(DWORD nvramVersion)
		{
			NvramWrite(NVRAM_MAP_VERSION_OFFSET, &nvramVersion, sizeof(nvramVersion));
		}

		/// <summary>
		/// Sets NVRAM and CMemoryStorage so it is being upgraded from prior to toNvramVersion.
		/// </summary>
		/// <param name="toNvramVersion">IN - The NVRAM version that needs to be upgraded to.</param>
		void SetNvramNeedsUpgrade(DWORD toNvramVersion)
		{
			CMemoryStorage::SetSprinkles();
			SetNvramVersion(toNvramVersion - 1);
			CMemoryStorage::UpgradeNVRAM(); // Set CMemoryStorage::PreviousNVRAMVersion.
		}

		/// <summary>
		/// Sets NVRAM and CMemoryStorage so it is being upgraded from prior to NVRAM_UNIVERSAL_POINTS_VERSION.
		/// </summary>
		void SetNvramNeedsUpgradeToUniversalPoints()
		{
			SetNvramNeedsUpgrade(NVRAM_UNIVERSAL_POINTS_VERSION);
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
/// Test the Universal Enterprise configuration support for persisted memory.
/// </summary>
TEST_F(ConfigUniversalEnterpriseUnitTests, ConfigUniversalEnterprisePersistUnitTest)
{
    LPCTSTR sessionPointsmessage(_T("sessionPointsmessage"));
    LPCTSTR sessionPointsmessage2(_T("sessionPointsmessage2"));
    LPCTSTR balanceRetrievalMessage(_T("balanceRetrievalMessage"));
    LPCTSTR pointsOrCashLabel(_T("pointsOrCashLabel"));
    LPCTSTR dailyPointsDisplaySuffixText(_T("dailyPointsDisplaySuffix"));
    LPCTSTR tierPointsLabel(_T("tierPointsLabel"));
    bool showPointsVsAccountBalance(true);

    // Set persisted values.
    {
        CMemoryStorage::SetSprinkles();
        CMemoryStorage::UpgradeNVRAM();

        CConfig config(false, "");
        config.SetSessionPointsMessage(sessionPointsmessage);
        config.SetBalanceRetrievalMessage(balanceRetrievalMessage);
        config.SetPointsOrCashLabel(pointsOrCashLabel);
        config.SetShowPointsVsAccountBalance(showPointsVsAccountBalance);
        config.SetDailyPointsDisplaySuffixMessage(dailyPointsDisplaySuffixText);
        config.SetTierPointsLabel(tierPointsLabel);

        // Set memory storage to already upgraded.
        CMemoryStorage::UpgradeNVRAM();
    }

    CConfig config(false, "");
    config.BuildYourself();

    LPCTSTR resultMsg;
    bool resultState;

    resultMsg = config.GetSessionPointsMessage();
    ASSERT_TRUE(0 == _tcscmp(resultMsg,  sessionPointsmessage))<< L"Session Points Message not persisted.";

    resultMsg = config.GetBalanceRetrievalMessage();
    ASSERT_TRUE(0 == _tcscmp(resultMsg,  balanceRetrievalMessage))<< L"Balance Retrieval Message not persisted.";

    resultMsg = config.GetPointsOrCashLabel();
    ASSERT_TRUE(0 == _tcscmp(resultMsg,  pointsOrCashLabel))<< L"Points or Cash Label not persisted.";

    resultState = config.GetShowPointsVsAccountBalance();
    ASSERT_TRUE(resultState == showPointsVsAccountBalance)<< L"Show Points vs Account Balance not persisted.";

    resultMsg = config.GetDailyPointsDisplaySuffixMessage();
    ASSERT_TRUE(0 == _tcscmp(resultMsg, dailyPointsDisplaySuffixText))<< L"Daily Points Display Suffix not persisted.";

    resultMsg = config.GetTierPointsLabel();
    ASSERT_TRUE(0 == _tcscmp(resultMsg, tierPointsLabel))<< L"Tier Points Label not persisted.";

    // Configure NVRAM for subsequent tests.
    SetNvramInvalid();
}

/// <summary>
/// Test the Universal Enterprise configuration for default values, when NVRAM is invalid.
/// </summary>

TEST_F(ConfigUniversalEnterpriseUnitTests, ConfigUniversalEnterpriseDefault_NvramInvalidUnitTest)                                      
{                                                                                                       
    SetNvramInvalid();                                                                                  
    CConfig config(false, "");                                                                                     
    config.BuildYourself();                                                                             
                                                                                                        
    LPCTSTR resultMsg;                                                                                  
    bool resultState;                                                                                   
                                                                                                        
    resultMsg = config.GetSessionPointsMessage(); 
    ASSERT_TRUE(resultMsg[0] == _T('\0'))<< L"Session Points Message not defaulted to 'empty'.";                                                      
                                                                                                        
    resultMsg = config.GetBalanceRetrievalMessage();  
    ASSERT_TRUE(resultMsg[0] == _T('\0'))<< L"Balance Retrieval Message not defaulted to 'empty'.";                                                  
                                                                                                        
    resultMsg = config.GetPointsOrCashLabel();  
    ASSERT_TRUE(resultMsg[0] == _T('\0'))<< L"Points or Cash Label not defaulted to 'empty'.";                                                        
                                                                                                        
    resultState = config.GetShowPointsVsAccountBalance();                                               
    ASSERT_FALSE(resultState)<< L"Show Points vs Account Balance not defaulted to 'false'.";
                                                                                                        
    resultMsg = config.GetDailyPointsDisplaySuffixMessage();                                            
    ASSERT_TRUE(resultMsg[0] == _T('\0'))<< L"Daily Points Display Suffix not defaulted to 'false'.";
                                                                                                        
    resultMsg = config.GetTierPointsLabel();                                                            
    ASSERT_TRUE(resultMsg[0] == _T('\0'))<< L"Tier Points Label not defaulted to 'false'.";
}                                                                                                       
                                                                                                        
/// <summary>                                                                                           
/// Test the Universal Enterprise configuration for default values, when NVRAM needs to be upgraded.    
/// </summary>                                                                                          
TEST_F(ConfigUniversalEnterpriseUnitTests, ConfigUniversalEnterpriseDefault_NvramNeedsUpgradeUnitTest)                                 
{                                                                                                       
    SetNvramNeedsUpgradeToUniversalPoints(); // Cause original to have default value.                   
    CConfig config(false, "");                                                                                     
    config.BuildYourself();                                                                             
                                                                                                        
    LPCTSTR resultMsg;                                                                                  
    bool resultState;                                                                                   
                                                                                                        
    resultMsg = config.GetSessionPointsMessage(); 
    ASSERT_TRUE(resultMsg[0] == _T('\0'))<< L"Session Points Message not defaulted to 'empty'.";                                                      
                                                                                                        
    resultMsg = config.GetBalanceRetrievalMessage();                                                    
    ASSERT_TRUE(resultMsg[0] == _T('\0'))<< L"Balance Retrieval Message not defaulted to 'empty'.";
                                                                                                        
    resultMsg = config.GetPointsOrCashLabel();                                                          
    ASSERT_TRUE(resultMsg[0] == _T('\0'))<< L"Points or Cash Label not defaulted to 'empty'.";
                                                                                                        
    resultState = config.GetShowPointsVsAccountBalance();                                               
    ASSERT_FALSE(resultState)<< L"Show Points vs Account Balance not defaulted to 'false'.";
}                                                                                                       
                                                                                                        
/// <summary>                                                                                           
/// Test the Universal Enterprise configuration for default values, when NVRAM needs to be upgraded.    
/// </summary>                                                                                          
TEST_F(ConfigUniversalEnterpriseUnitTests, ConfigUniversalEnterpriseDefault2_NvramNeedsUpgradeUnitTest)                                
{                                                                                                       
    SetNvramNeedsUpgrade(NVRAM_CONFIG_VERSION_2);// Cause original to have default value.               
    CConfig config(false, "");                                                                                     
    config.BuildYourself();                                                                             
                                                                                                        
    LPCTSTR resultMsg;                                                                                  
                                                                                                        
    resultMsg = config.GetDailyPointsDisplaySuffixMessage();   
    ASSERT_TRUE(resultMsg[0] == _T('\0'))<< L"Daily Points Display Suffix not defaulted to 'empty'.";                                         
                                                                                                        
    resultMsg = config.GetTierPointsLabel();                                                            
    ASSERT_TRUE(resultMsg[0] == _T('\0'))<< L"Tier Points Label not defaulted to 'empty'.";
}                                                                                                       
                                                                                                        
TEST_F(ConfigUniversalEnterpriseUnitTests, ConfigUniversalEnterpriseSetGetSessionPointsMessageUnitTest)                                
{                                                                                                       
    CConfig config(false, "");                                                                                     
    config.BuildYourself();                                                                             
                                                                                                        
    std::string testMsg = "This is a test. Only a test. Nothin' but a stinkin test.";
    unsigned i = 0;                   
    while (SIZE_OF_MESSAGES - 1 > testMsg.length())                                                  
    {                                    
        char temp = char('0' + i++%10);  
        testMsg.append(1, temp);
    }                                                                                                   
                                                                                                        
    config.SetSessionPointsMessage(testMsg.c_str());                                                            
                                                                                                        
    LPCTSTR resultMsg = config.GetSessionPointsMessage();                                               
                                                                                                                                                                
    ASSERT_TRUE(resultMsg == testMsg)<< L"Session Points Message sent is not what was returned.";       

    // Make sure config does not keep test data                                                              
    config.SetSessionPointsMessage(_T(""));                                                                                                         
}                                                                                                       
                                                                                                        
TEST_F(ConfigUniversalEnterpriseUnitTests, ConfigUniversalEnterpriseSetGetBalanceRetrievalMessageUnitTest)                             
{                                                                                                       
    CConfig config(false, "");                                                                                     
    config.BuildYourself();                                                                             
                                                                                                        
    std::string testMsg = "This is a test. Only a test. Nothin' but a stinkin test.";                      
    unsigned i = 0;                                                                                     
    while (SIZE_OF_MESSAGES - 1 > testMsg.length())                                                  
    {                                             
        char temp = char('0' + i++%10);
        testMsg.append(1, temp);                                                      
    }                                                                                                   
                                                                                                        
    config.SetBalanceRetrievalMessage(testMsg.c_str());                                                         
                                                                                                        
    LPCTSTR resultMsg = config.GetBalanceRetrievalMessage();                                            
                                            
    ASSERT_TRUE(resultMsg == testMsg)<< L"Balance Retrieval Message sent is not what was returned.";       
    
    // Make sure config does not keep test data                                                         
    config.SetBalanceRetrievalMessage(_T(""));                                                                                                                  
}                                                                                                       
                                                                                                        
TEST_F(ConfigUniversalEnterpriseUnitTests, ConfigUniversalEnterpriseSetGetPointsOrCashLabelUnitTest)                                   
{                                                                                                       
    CConfig config(false, "");                                                                                     
    config.BuildYourself();                                                                             
                                                                                                        
    std::string testMsg = _T("Nothin' but a stinkin test.");                                                   
    unsigned i = 0;                                                                                     
    while (SIZE_OF_LABEL - 1 > testMsg.length())                                                     
    {                                          
        char temp = char('0' + i++%10);
        testMsg.append(1, temp);                                                         
    }                                                                                                   
                                                                                                        
    config.SetPointsOrCashLabel(testMsg.c_str());                                                               
                                                                                                        
    LPCTSTR resultMsg = config.GetPointsOrCashLabel();                                                  
                                                                                                        
    ASSERT_TRUE(resultMsg == testMsg)<< L"Points or Cash Label sent is not what was returned.";         
    
    // Make sure config does not keep test data                                                         
    config.SetPointsOrCashLabel(_T(""));                                                                                                                            
}                                                                                                       
                                                                                                        
TEST_F(ConfigUniversalEnterpriseUnitTests, ConfigUniversalEnterpriseSetGetDailyPointsDisplaySuffixMessageUnitTest)                     
{                                                                                                       
    CConfig config(false, "");                                                                                     
    config.BuildYourself();                                                                             
                                                                                                        
    std::string testMsg = _T("Nothin' but a stinkin test.");                                                   
    unsigned i = 0;                                                                                     
    while (SIZE_OF_MESSAGES - 1 > testMsg.length())                                                  
    {                                             
        char temp = char('0' + i++%10);
        testMsg.append(1, temp);                                                      
    }                                                                                                   
                                                                                                        
    config.SetDailyPointsDisplaySuffixMessage(testMsg.c_str());                                                 
                                                                                                        
    LPCTSTR resultMsg = config.GetDailyPointsDisplaySuffixMessage();                                    
     
    ASSERT_TRUE(resultMsg == testMsg)<< L"Daily Points Display Suffix sent is not what was returned.";  
    
    // Make sure config does not keep test data                                                         
    config.SetDailyPointsDisplaySuffixMessage(_T(""));                                                                                                                                                      
}                                                                                                       
                                                                                                        
TEST_F(ConfigUniversalEnterpriseUnitTests, ConfigUniversalEnterpriseSetGetTierPointsLabelUnitTest)                                     
{                                                                                                       
    CConfig config(false, "");                                                                                     
    config.BuildYourself();                                                                             
                                                                                                        
    std::string testMsg = _T("Nothin' but a stinkin test.");                                                   
    unsigned i = 0;                                                                                     
    while (SIZE_OF_LABEL - 1 > testMsg.length())                                                     
    {                                          
        char temp = char('0' + i++%10);
        testMsg.append(1, temp);                                                         
    }                                                                                                   
                                                                                                        
    config.SetTierPointsLabel(testMsg.c_str());                                                                 
                                                                                                        
    LPCTSTR resultMsg = config.GetTierPointsLabel();                                                    
           
    ASSERT_TRUE(resultMsg == testMsg)<< L"Tier Points Label sent is not what was returned.";            
    
    // Make sure config does not keep test data                                                         
    config.SetTierPointsLabel(_T(""));                                                                                                                                                      
}                                                                                                       
                                                                                                        
TEST_F(ConfigUniversalEnterpriseUnitTests, ConfigUniversalEnterpriseSetGetShowPointsVsAccountBalanceFalseUnitTest)                     
{                                                                                                       
    SetNvramNeedsUpgradeToUniversalPoints(); // Cause original to have default value.                   
    CConfig config(false, "");                                                                                     
    config.BuildYourself();                                                                             
                                                                                                        
    bool test = false;                                                                                  
                                                                                                        
    bool original = config.SetShowPointsVsAccountBalance(test);                                         
    ASSERT_FALSE(original)<< L"Show Points vs Account Balance did not have the correct default.";
                                                                                                        
    bool result = config.GetShowPointsVsAccountBalance();                                               
                                                                                                        
    // Make sure config does not keep test data                                                         
    config.SetShowPointsVsAccountBalance(original);                                                     
      
    ASSERT_TRUE(result == test)<< L"Show Points vs Account Balance sent is not what was returned.";                                                                                                  
}                                                                                                       
                                                                                                        
TEST_F(ConfigUniversalEnterpriseUnitTests, ConfigUniversalEnterpriseSetGetShowPointsVsAccountBalanceTrueUnitTest)                      
{                                                                                                       
    CConfig config(false, "");                                                                                     
    config.BuildYourself();                                                                             
                                                                                                        
    bool test = true;                                                                                   
                                                                                                        
    bool original = config.SetShowPointsVsAccountBalance(test);                                         
    ASSERT_FALSE(original)<< L"Show Points vs Account Balance did not have the correct default.";
                                                                                                        
    bool result = config.GetShowPointsVsAccountBalance();                                               
                                                                                                        
    // Make sure config does not keep test data                                                         
    config.SetShowPointsVsAccountBalance(original);                                                     
      
    ASSERT_TRUE(result == test)<< L"Show Points vs Account Balance sent is not what was returned.";                                                                                                  
}                                                                                                       