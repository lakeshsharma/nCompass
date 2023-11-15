
#include "stdafx.h"
#include "CardReaderTestHelpers.h"
#include "TestUtil.h"
#include "gtest/gtest.h"
#include "AssertMsg.h"
#include "../Sentinel/Util/FileSys.h"

using namespace pugi;
using namespace std;
using namespace TestUtil;

    class CardReaderConfigTests : public ::testing::Test
	{    
		public:        
			virtual void SetUp()  
			{
				DeleteSentinelFiles(DIRECTORY_SENTINELDATA);
				DeleteSentinelFiles(DIRECTORY_APPLICATIONS);
                FileSys::CreateDirectory(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_APPLICATIONS));
			} 
			virtual void TearDown()
			{
				DeleteSentinelDirectories();
			}  
			
			void TestGetCardTypeColorExt(int line, CardReaderConfig& config, CardReaderCardType::Type type, const CardReaderColor* pExpected)
			{
				const CardReaderColor* pColor = config.GetCardTypeColor(type);
				ASSERT_NE(pColor, nullptr) << AssertMsg::Format(line, nullptr, L"GetCardTypeColor(CardReaderCardType::%d)", (int) type);
                ASSERT_TRUE((*pExpected == *pColor)) << AssertMsg::Format(line, nullptr, L"GetCardTypeColor(CardReaderCardType::%d)", (int) type);
			}
			void TestGetCardTypeColor(int line, CardReaderConfig& config, CardReaderCardType::Type type, const CardReaderColor* pExpected)
			{
				const CardReaderColor* pColor = config.GetCardTypeColor(type);
				ASSERT_NE(pColor, nullptr) << AssertMsg::Format(line, nullptr, L"GetCardTypeColor(CardReaderCardType::%d)", (int) type);
				ASSERT_TRUE(pExpected->CompareHidData(*pColor)) << AssertMsg::Format(line, nullptr, L"GetCardTypeColor(CardReaderCardType::%d)", (int) type);
			}
			void TestGetRankColor(int line, CardReaderConfig& config, int rank, const CardReaderColor* pExpected)
			{
				const CardReaderColor* pColor = config.GetRankColor(rank);
				ASSERT_NE(pColor, nullptr) << AssertMsg::Format(line, nullptr, L"GetRankColor(%d)", rank);
				ASSERT_TRUE(pExpected->CompareHidData(*pColor)) << AssertMsg::Format(line, nullptr, L"GetRankColor(%d)", rank);
			}
	};
	

    TEST_F(CardReaderConfigTests, CardReaderCardType_emap_pBad)                                                                                                 
    {                                                                                                                                                           
        CardReaderCardType::Type type = CardReaderCardType::emap[CardReaderCardType::pBadRead];                                                                 
        ASSERT_EQ((int) CardReaderCardType::BadRead, (int) type);                                                                                        
    }                                                                                                                                                           
                                                                                                                                                               
    TEST_F(CardReaderConfigTests, CardReaderCardType_emap_Bad)                                                                                                  
    {              
		ASSERT_TRUE("BadRead" == string(CardReaderCardType::emap[CardReaderCardType::BadRead]));                                                           
    }                                                                                                                                                                
                                                                                                                                                                
    TEST_F(CardReaderConfigTests, CardReaderConfig_ctor_nullptr)                                                                                                
    {                                                                                                                                                           
        try                                                                                                                                                     
        {                                                                                                                                                
            CardReaderConfig config("");                                                                                                                   
                                                                                                                                                               
            TestGetCardTypeColorExt(__LINE__, config, CardReaderCardType::None,     &CardReaderTestHelpers::colorNone);                                         
            TestGetCardTypeColorExt(__LINE__, config, CardReaderCardType::Player,   &CardReaderTestHelpers::colorPlayer);                                       
            TestGetCardTypeColorExt(__LINE__, config, CardReaderCardType::Employee, &CardReaderTestHelpers::colorEmployeeDefault);                              
            TestGetCardTypeColorExt(__LINE__, config, CardReaderCardType::BadRead,  &CardReaderTestHelpers::colorBadRead);                                      
        }                                                                                                                                                       
        catch (...)                                                                                                                                             
        {                                                                                                                                                       
            FAIL() << L"Unexpected exception";                                                                                                              
        }                                                                                                                                                       
    }                                                                                                                                               
                                                                                                                                                                 

    TEST_F(CardReaderConfigTests, CardReaderConfig_missingContents)                                                               
    {                                                                                                                             
        try                                                                                                                       
        {                                                                                                                    
            DeleteOnCloseFile configFile(DIRECTORY_APPLICATIONS, pCardReaderConfigFilename.c_str(), "");                                                                                
            CardReaderConfig config((configFile.GetPathname()));                                          
            FAIL() << L"Expected exception";                                                                                      
        }                                                                                                                         
        catch (...)                                                                                                               
        {                                                                                                                         
        }                                                                                                                         
    }                                                                                                                             
                                                                                                                                                              
    TEST_F(CardReaderConfigTests, CardReaderConfig_missingEmployee)                                                                                             
    {                                                                                                                                                           
        try                                                                                                                                                     
        {                                                                                                                                                    
            DeleteOnCloseFile cardReaderConfigFile(DIRECTORY_APPLICATIONS, pCardReaderConfigFilename.c_str(), CardReaderTestHelpers::pConfigXmlNoRanksMissingEmployee); 
            CardReaderConfig config(cardReaderConfigFile.GetPathname());                                                                                                      
            FAIL() << L"Expected exception";                                                                                                                
        }                                                                                                                                                       
        catch (...)                                                                                                                                             
        {                                                                                                                                                       
        }                                                                                                                                                       
    }                                                                                                                                                           
                                                                                                                                                               
    TEST_F(CardReaderConfigTests, CardReaderConfig_missingRanks)                                                                                                
    {                                                                                                                                                           
        try                                                                                                                                                     
        {                                                                                                                                                       
            DeleteOnCloseFile configFile(DIRECTORY_APPLICATIONS, pCardReaderConfigFilename.c_str(), CardReaderTestHelpers::pConfigXmlNoRanks);                          
            CardReaderConfig config(configFile.GetPathname());                                                                                                                
                                                                                                                                                                
            const CardReaderColor* pColor = config.GetRankColor(0);                                                                                             
            ASSERT_EQ(pColor, nullptr) << L"GetRankColor(0)";                                                                                                         
                                                                                                                                                               
            pColor = config.GetRankColor(5678);                                                                                                                 
            ASSERT_EQ(pColor, nullptr) << L"GetRankColor(5678)";                                                                                                      
        }                                                                                                                                                       
        catch (...)                                                                                                                                             
        {                                                                                                                                                       
            FAIL() << L"Unexpected exception";                                                                                                              
        }                                                                                                                                                       
    }                                                                                                                                                           
                                                                                                                                                                 
    TEST_F(CardReaderConfigTests, CardReaderConfig_extraCardTypeColorsElement)                                                                                  
    {                                                                                                                                                           
        try                                                                                                                                                     
        {                                                                                                                                                      
            DeleteOnCloseFile configFile(DIRECTORY_APPLICATIONS, pCardReaderConfigFilename.c_str(), CardReaderTestHelpers::pConfigXmlNoRanksExtraCardTypeColorsElement);
            CardReaderConfig config(configFile.GetPathname());                                                                                                                
        }                                                                                                                                                       
        catch (...)                                                                                                                                             
        {                                                                                                                                                       
            FAIL() << L"Unexpected exception";                                                                                                              
        }                                                                                                                                                       
    }                                                                                                                                                           
                                                                                                                                                                
    TEST_F(CardReaderConfigTests, CardReaderConfig_GetCardTypeColor_invalid)                                                                                    
    {                                                                                                                                                           
        try                                                                                                                                                     
        {                                                                                                                                                       
            DeleteOnCloseFile configFile(DIRECTORY_APPLICATIONS, pCardReaderConfigFilename.c_str(), CardReaderTestHelpers::pConfigXml);                                 
            CardReaderConfig config(configFile.GetPathname());                                                                                                                
                                                                                                                                                                
            const CardReaderColor* pColor = config.GetCardTypeColor((CardReaderCardType::Type) -1);                                                             
            ASSERT_EQ(pColor, nullptr) << L"GetCardTypeColor((CardReaderCardType::Type) -1)";                                                                         
                                                                                                                                                                
            pColor = config.GetCardTypeColor((CardReaderCardType::Type) INT_MAX);                                                                               
            ASSERT_EQ(pColor, nullptr) << L"GetCardTypeColor((CardReaderCardType::Type) INT_MAX)";                                                                    
        }                                                                                                                                                       
        catch (...)                                                                                                                                             
        {                                                                                                                                                       
            FAIL() << L"Unexpected exception";                                                                                                              
        }                                                                                                                                                       
    }                                                                                                                                                           
                                                                                                                                                                
    TEST_F(CardReaderConfigTests, CardReaderConfig_GetCardTypeColor)                                                                                            
    {                                                                                                                                                           
        try                                                                                                                                                     
        {                                                                                                                                                       
            DeleteOnCloseFile configFile(DIRECTORY_APPLICATIONS, pCardReaderConfigFilename.c_str(), CardReaderTestHelpers::pConfigXml);                                 
            CardReaderConfig config(configFile.GetPathname());                                                                                                                
                                                                                                                                                                
            TestGetCardTypeColor(__LINE__, config, CardReaderCardType::None,     &CardReaderTestHelpers::colorNone);                                            
            TestGetCardTypeColor(__LINE__, config, CardReaderCardType::Player,   &CardReaderTestHelpers::colorPlayer);                                          
            TestGetCardTypeColor(__LINE__, config, CardReaderCardType::Employee, &CardReaderTestHelpers::colorEmployee);                                        
            TestGetCardTypeColor(__LINE__, config, CardReaderCardType::BadRead,  &CardReaderTestHelpers::colorBadRead);                                         
        }                                                                                                                                                       
        catch (...)                                                                                                                                             
        {                                                                                                                                                       
            FAIL() << L"Unexpected exception";                                                                                                             
        }                                                                                                                                                       
    }                                                                                                                                                           
                                                                                                                                                            
    TEST_F(CardReaderConfigTests, CardReaderConfig_GetRankColor_empty)                                                                                          
    {                                                                                                                                                           
        try                                                                                                                                                     
        {                                                                                                                                                       
            CardReaderConfig config("");                                                                                                                   
                                                                                                                                                                
            const CardReaderColor* pColor = config.GetRankColor(0);                                                                                             
            ASSERT_EQ(pColor, nullptr) << L"GetRankColor(0)";                                                                                                         
                                                                                                                                                                
            pColor = config.GetRankColor(5678);                                                                                                                 
            ASSERT_EQ(pColor, nullptr) << L"GetRankColor(5678)";                                                                                                      
        }                                                                                                                                                       
        catch (...)                                                                                                                                             
        {                                                                                                                                                       
            FAIL() << L"Unexpected exception";                                                                                                             
        }                                                                                                                                                       
    }                                                                                                                                                           
                                                                                                                                                                
    TEST_F(CardReaderConfigTests, CardReaderConfig_GetRankColor_invalid)                                                                                        
    {                                                                                                                                                           
        try                                                                                                                                                     
        {                                                                                                                                                       
            CardReaderConfig config("");                                                                                                                   
                                                                                                                                                                
            const CardReaderColor* pColor = config.GetRankColor(-1);                                                                                            
            ASSERT_EQ(pColor, nullptr) << L"GetRankColor(-1)";                                                                                                        
                                                                                                                                                                
            pColor = config.GetRankColor(INT_MAX);                                                                                                              
            ASSERT_EQ(pColor, nullptr) << L"GetRankColor(INT_MAX)";                                                                                                   
        }                                                                                                                                                       
        catch (...)                                                                                                                                             
        {                                                                                                                                                       
            FAIL() << L"Unexpected exception";                                                                                                             
        }                                                                                                                                                       
    }                                                                                                                                                           
                                                                                                                                                                
    TEST_F(CardReaderConfigTests, CardReaderConfig_GetRankColor)                                                                                                
    {                                                                                                                                                           
        try                                                                                                                                                     
        {                                                                                                                                                       
            DeleteOnCloseFile cardReadersFile(DIRECTORY_APPLICATIONS, pCardReaderConfigFilename.c_str(), CardReaderTestHelpers::pConfigXml);                            
            CardReaderConfig config(cardReadersFile.GetPathname());                                                                                                           
                                                                                                                                                                
            TestGetRankColor(__LINE__, config, 0, &CardReaderTestHelpers::colorRank0);                                                                          
            TestGetRankColor(__LINE__, config, 1, &CardReaderTestHelpers::colorRank1);                                                                          
            TestGetRankColor(__LINE__, config, 2, &CardReaderTestHelpers::colorRank2);                                                                          
        }                                                                                                                                                       
        catch (...)                                                                                                                                             
        {                                                                                                                                                       
            FAIL() << L"Unexpected exception";                                                                                                             
        }                                                                                                                                                       
    }                                                                                                                                                           
                                                                                                                                                         
