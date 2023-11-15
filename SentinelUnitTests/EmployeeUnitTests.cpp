#include "stdafx.h"
#include "../Sentinel/Employee.h"

// Unit testing includes.
#include "UnitTestingUtils.h"

#include "gtest/gtest.h"

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, (const int *)NULL, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

using namespace std;

namespace SentinelNativeUnitTests
{
    class EmployeeUnitTests : public ::testing::Test
    {
        protected:
            virtual void SetUp() 
            {
            }

            virtual void TearDown() 
            {
            }
    public:
        bool GetEmployeeidleTimeout(const CEmployee& obj)
        {
            return obj.m_idleTimeout;
        }
        DWORD GetEmployeeidleTimeoutDuration(const CEmployee& obj)
        {
            return obj.m_idleTimeoutDuration;
        }
        DWORD GetEmployeeActivityTimestamp(const CEmployee& obj)
        {
            return obj.m_activityTimestamp;
        }
    };

    TEST_F(EmployeeUnitTests, EmployeeConstructionTest)
    {

        CCard *card = new CCard( CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() + 60 * 10 );
        card->SetCardID("A500001122");
        CConfig config(false, "");

        CEmployee *employee = new CEmployee(card, config, false, false, 0, 0);
        const CCard *cpyCard = employee->GetCard();

        ASSERT_TRUE(card->Compare( cpyCard ));

        delete employee;
        
    }


    TEST_F(EmployeeUnitTests, EmployeeCopyConstructorTest)                                                                           
    {                                                                                                                  
        CCard *card = new CCard( CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() + 60 * 10 );
        card->SetCardID("A500001122");                                                                                
        CConfig config(false, "");                                                                                                
                                                                                                                       
        CEmployee *employee = new CEmployee(card, config, false, false, 0, 0);                                         
        CEmployee *cpyEmployee = new CEmployee( *employee );                                                           
                                                                                                                       
        const CCard *cpyCard = cpyEmployee->GetCard();                                                                 
                                
        ASSERT_TRUE(card->Compare( cpyCard ));                                                                                                                                                             
                                                                                                                       
        delete employee;                                                                                               
        delete cpyEmployee;                                                                                            
    }                                                                                                                  
                                                                                                                       
   TEST_F(EmployeeUnitTests, EmployeeCompareCardTest)                                                                               
    {                                                                                                                  
        time_t start = CUtilities::GetCurrentTime();                                                                    
        time_t stop = CUtilities::GetCurrentTime() + 60 * 10;                                            
        string cardID = "A500001122";                                                                                
        string differentCard = "A500001111";                                                                         
                                                                                                                       
        CCard *card = new CCard( start, stop );                                                                        
        card->SetCardID( cardID );                                                                                     
        CConfig config(false, "");                                                                                                
                                                                                                                       
        CEmployee *employee = new CEmployee(card, config, false, false, 0, 0);                                         
                                                                                                                       
        CCard *newCard = new CCard( start, stop );                                                                     
        newCard->SetCardID( cardID ); 
        ASSERT_TRUE( employee->CompareCard( newCard ) );                                                                                  
                                                                                                                       
        newCard->SetCardID( differentCard );                                                                           
        ASSERT_FALSE( employee->CompareCard( newCard ) );                                          
                                                                                                                       
        delete employee;                                                                                               
        delete newCard;                                                                                                
    }                                                                                                                  
                                                                                                                       
    TEST_F(EmployeeUnitTests, EmployeeComparePINTest)                                                                               
    {                                                                                                                  
        time_t start = CUtilities::GetCurrentTime();                                                                    
        time_t stop = CUtilities::GetCurrentTime() + 60 * 10;                                            
        string cardID = "A500001122";                                                                                
        LPCTSTR pin = _T("1234");                                                                                         
        LPCTSTR rightPin = _T("1234");                                                                                    
        LPCTSTR wrongPin = _T("2234");                                                                                    
                                                                                                                       
        CCard *card = new CCard( start, stop );                                                                        
        card->SetCardID( cardID );                                                                                     
        CConfig config(false, "");                                                                                                
                                                                                                                       
        CEmployee *employee = new CEmployee(card, config, false, false, 0, 0);                                         
        employee->SetPin( pin );                                                                                       
                     
        ASSERT_TRUE( employee->ComparePin( rightPin ) );                                                                                                                                                              
        ASSERT_FALSE( employee->ComparePin( wrongPin ) );                                                           
                                                                                                                       
        delete employee;                                                                                               
                                                                                                                       
    }                                                                                                                  
                                                                                                                       
    TEST_F(EmployeeUnitTests, EmployeeCompareGlobalPINTest)                                                                         
    {                                                                                                                  
        const string GLOBAL_GLOBAL_PASSWORD = "31459";                                                               
                                                                                                                       
        time_t start = CUtilities::GetCurrentTime();                                                                    
        time_t stop = CUtilities::GetCurrentTime() + 60 * 10;                                            
        string cardID = "6909FF0001";                                                                                
        string wrongPin = "2234";                                                                                    
                                                                                                                       
        CCard *card = new CCard( start, stop );                                                                        
        card->SetCardID( cardID );                                                                                     
        CConfig config(false, "");                                                                                                
                                                                                                                       
        CEmployee *employee = new CEmployee(card, config, false, false, 0, 0);                                         
        //employee->SetPin( pin );                                                                                     
                                
        ASSERT_TRUE( employee->ComparePin( GLOBAL_GLOBAL_PASSWORD ) );                                                                                       
        ASSERT_FALSE( employee->ComparePin( wrongPin ) );
                                                                                                                       
        delete employee;                                                                                               
                                                                                                                       
    }                                                                                                                  
                                                                                                                       
    TEST_F(EmployeeUnitTests, EmployeeIsWaitingForPINTest )                                                                         
    {                                                                                                                  
        const string GLOBAL_GLOBAL_PASSWORD = "31459";                                                               
                                                                                                                       
        time_t start = CUtilities::GetCurrentTime();                                                                    
        time_t stop = CUtilities::GetCurrentTime() + 60 * 10;                                            
        string cardID = "6909FF0001";                                                                                
        string wrongPin = "2234";                                                                                    
                                                                                                                       
        CCard *card = new CCard( start, stop );                                                                        
        card->SetCardID( cardID );                                                                                     
        CConfig config(false, "");                                                                                                
                                                                                                                       
        CEmployee *employee = new CEmployee(card, config, false, false, 0, 0);                                         
                                                                                                                       
        employee->SetWaitingForPin( true );
        ASSERT_TRUE( employee->IsWaitingForPin() );                                                                            
        ASSERT_FALSE( employee->IsDoneWaitingForPin() );
                                                                                                                       
        delete employee;                                                                                               
                                                                                                                       
    }                                                                                                                  
                                                                                                                       
    TEST_F( EmployeeUnitTests, EmployeeGetEmployeeCardTypeTest )                                                                     
    {                                                                                                                  
                                                                                                                       
        typedef struct                                                                                                 
        {                                                                                                              
            const byte cardID[5];                                                                                      
            const CardType cardType;                                                                                   
        } CardIDType;                                                                                                  
                                                                                                                       
        const int cardcount = 5;                                                                                       
        const  CardIDType cards[cardcount] =                                                                           
        {                                                                                                              
            { { 00, 01, 02, 03, 04 }, CARD_CURRENCY },                                                                 
            { { 00, 01, 02, 03, 04 }, CARD_MECHANIC },                                                                 
            { { 00, 01, 02, 03, 04 }, CARD_GLOBAL },                                                                   
            { { 00, 01, 02, 03, 04 }, CARD_FLOOR },                                                                    
            { { 00, 01, 02, 03, 04 }, CARD_METER },                                                                    
        };                                                                                                             
                                                                                                                       
        time_t start = CUtilities::GetCurrentTime();                                                                    
        time_t stop = CUtilities::GetCurrentTime() + 60 * 10;                                            
        CConfig config(false, "");                                                                                                
                                                                                                                       
        for (int i = 0; i < cardcount; i++)                                                                            
        {                                                                                                              
            CCard *card = new CCard( start, stop );                                                                    
            card->SetCardInfo( cards[i].cardType, cards[i].cardID, 5 );                                                
            CEmployee *employee = new CEmployee(card, config, false, false, 0, 0);                                     
            ASSERT_TRUE( employee->GetEmployeeCardType() == cards[i].cardType );
            delete employee;                                                                                           
        }                                                                                                              
                                                                                                                       
                                                                                                                       
    }                                                                                                                  
                                                                                                                       
    TEST_F(EmployeeUnitTests, EmployeeIsVirtualTest)                                                                                 
    {                                                                                                                  
                                                                                                                       
        CCard *card = new CCard( CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() + 60 * 10 );
        card->SetCardID("A500001122");                                                                                
        CConfig config(false, "");                                                                                                
                                                                                                                       
        CEmployee *employee = new CEmployee(card, config, false, false, 0, 0);                                         
        employee->CardRemoved(CUtilities::GetCurrentTime(), true );                                                    
        ASSERT_TRUE( employee->IsVirtual() );
        delete employee;                                                                                               
                                                                                                                       
        card = new CCard( CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() + 60 * 10 );       
        card->SetCardID("A500001122");                                                                                
        employee = new CEmployee(card, config, false, false, 0, 0);                                                    
        employee->CardRemoved(CUtilities::GetCurrentTime(), false );                                                   
        ASSERT_FALSE( employee->IsVirtual() );
        delete employee;                                                                                               
                                                                                                                       
    }                                                                                                                  
                                                                                                                       
    TEST_F(EmployeeUnitTests, EmployeeHasPINBeenReceivedTest)                                                                        
    {                                                                                                                  
                                                                                                                       
        CCard *card = new CCard( CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() + 60 * 10 );
        card->SetCardID("A500001122");                                                                                
        CConfig config(false, "");                                                                                                
                                                                                                                       
        CEmployee *employee = new CEmployee(card, config, false, false, 0, 0);                                         
                                
        ASSERT_FALSE( employee->HasPinBeenReceived() );                                                                                       
        employee->SetPin("1234");
        ASSERT_TRUE( employee->HasPinBeenReceived() );                                                                                     
                                                                                                                       
        delete employee;                                                                                               
                                                                                                                       
    }                                                                                                                  
                                                                                                                       
    TEST_F(EmployeeUnitTests, EmployeeRequiresPINTest)                                                                               
    {                                                                                                                  
                                                                                                                       
        CCard *card = new CCard( CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() + 60 * 10 );
        card->SetCardID("6909FF0001");                                                                                
        CConfig config(false, "");                                                                                                
                                                                                                                       
        config.SetEmployeePinRequired(true);                                                                           
        config.SetGlobalCardPassword(1234);                                                                            
        CEmployee *employee = new CEmployee(card, config, false, false, 0, 0);                                         
        ASSERT_TRUE( employee->EmployeeRequiresPin() );
        delete employee;                                                                                               
                                                                                                                       
        card = new CCard( CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() +60 * 10 );       
        card->SetCardID("A500001122");                                                                                
        employee = new CEmployee(card, config, false, false, 0, 0);                                                    
        employee->CardRemoved(CUtilities::GetCurrentTime(), false );                                                   
        ASSERT_TRUE( employee->EmployeeRequiresPin() );
        delete employee;                                                                                               
                                                                                                                       
        config.SetEmployeePinRequired(false);                                                                          
        config.SetGlobalCardPassword(0);                                                                               
        card = new CCard( CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() + 60 * 10 );       
        card->SetCardID("A500001122");                                                                                
        employee = new CEmployee(card, config, false, false, 0, 0);                                                    
        employee->CardRemoved(CUtilities::GetCurrentTime(), false );                                                   
        ASSERT_FALSE( employee->EmployeeRequiresPin() );
        delete employee;                                                                                               
                                                                                                                       
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *, ...)                                                                         
    ///     CEmployee::IsGoodbyeEnabled()                                                                              
    /// Test for IsGoodbyeEnabled() minimum value.                                                                     
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CEmployee_GoodbyeEnabledMinTest)                                                                       
    {                                                                                                                  
        // Test data.                                                                                                  
        const bool isGoodbyeEnabled(false);                                                                            
                                                                                                                       
        // Setup.                                                                                                      
        const bool idleTimeout(false);                                                                                 
        const DWORD idleTimeoutDuration(0);                                                                            
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.IsGoodbyeEnabled(), isGoodbyeEnabled);                                                       
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *, ...)                                                                         
    ///     CEmployee::IsGoodbyeEnabled()                                                                              
    /// Test for IsGoodbyeEnabled() maximum value.                                                                     
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CEmployee_GoodbyeEnabledMaxTest)                                                                       
    {                                                                                                                  
        // Test data.                                                                                                  
        const bool isGoodbyeEnabled(true);                                                                             
                                                                                                                       
        // Setup.                                                                                                      
        const bool idleTimeout(false);                                                                                 
        const DWORD idleTimeoutDuration(0);                                                                            
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.IsGoodbyeEnabled(), isGoodbyeEnabled);                                                       
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *, ...)                                                                         
    /// Test for m_idleTimeout minimum value.                                                                          
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CEmployee_IdleTimeoutMinTest)                                                                          
    {                                                                                                                  
        // Test data.                                                                                                  
        const bool idleTimeout(false);                                                                                 
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const DWORD idleTimeoutDuration(0);                                                                            
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                
        bool EmployeeIdleTimeout =  GetEmployeeidleTimeout(obj);                                                                                                     
        // Test(s).                                                                                                    
        AssertResult2(EmployeeIdleTimeout, idleTimeout);                                                                 
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *, ...)                                                                         
    /// Test for m_idleTimeout maximum value.                                                                          
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CEmployee_IdleTimeoutMaxTest)                                                                          
    {                                                                                                                  
        // Test data.                                                                                                  
        const bool idleTimeout(true);                                                                                  
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const DWORD idleTimeoutDuration(0);                                                                            
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                
        bool EmployeeIdleTimeout =  GetEmployeeidleTimeout(obj);                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(EmployeeIdleTimeout, idleTimeout);                                                                 
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *, ...)                                                                         
    /// Test for m_idleTimeoutDuration minimum value.                                                                  
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CEmployee_IdleTimeoutDurationMinTest)                                                                  
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(0);                                                                            
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(false);                                                                                 
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
           
        DWORD EmployeeTimeoutDuration = GetEmployeeidleTimeoutDuration(obj);                                                                                                            
        // Test(s).                                                                                                    
        AssertResult2(EmployeeTimeoutDuration, idleTimeoutDuration);                                                 
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *, ...)                                                                         
    /// Test for m_idleTimeoutDuration maximum value.                                                                  
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CEmployee_IdleTimeoutDurationMaxTest)                                                                  
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(DWORD_MAX);                                                                    
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(false);                                                                                 
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                            
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                            
        DWORD EmployeeTimeoutDuration = GetEmployeeidleTimeoutDuration(obj);                                                                                            
        // Test(s).                                                                                                    
        AssertResult2(EmployeeTimeoutDuration, idleTimeoutDuration);                                                 
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *, ...)                                                                         
    /// Test for m_activityTimestamp minimum value.                                                                    
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CEmployee_ActivityTimestampMinTest)                                                                    
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD activityTimestamp(0);                                                                              
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(false);                                                                                 
        const DWORD idleTimeoutDuration(0);                                                                            
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                  
        DWORD EmployeeActivityTimestamp = GetEmployeeActivityTimestamp(obj);                                                                                     
        // Test(s).                                                                                                    
        AssertResult2(EmployeeActivityTimestamp, activityTimestamp);                                                     
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *, ...)                                                                         
    /// Test for m_activityTimestamp maximum value.                                                                    
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CEmployee_ActivityTimestampMaxTest)                                                                    
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD activityTimestamp(DWORD_MAX);                                                                      
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(false);                                                                                 
        const DWORD idleTimeoutDuration(0);                                                                            
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
    
         DWORD EmployeeActivityTimestamp = GetEmployeeActivityTimestamp(obj);                                                                                                                  
        // Test(s).                                                                                                    
        AssertResult2(EmployeeActivityTimestamp, activityTimestamp);                                                     
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(const CEmployee &)                                                                    
    ///     CEmployee::IsGoodbyeEnabled()                                                                              
    /// Test for IsGoodbyeEnabled() minimum value.                                                                     
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CopyConstructor_GoodbyeEnabledMinTest)                                                                 
    {                                                                                                                  
        // Test data.                                                                                                  
        const bool isGoodbyeEnabled(false);                                                                            
                                                                                                                       
        // Setup.                                                                                                      
        const bool idleTimeout(false);                                                                                 
        const DWORD idleTimeoutDuration(0);                                                                            
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(objA);                                                                                     
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.IsGoodbyeEnabled(), isGoodbyeEnabled);                                                       
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(const CEmployee &)                                                                    
    ///     CEmployee::IsGoodbyeEnabled()                                                                              
    /// Test for IsGoodbyeEnabled() maximum value.                                                                     
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CopyConstructor_GoodbyeEnabledMaxTest)                                                                 
    {                                                                                                                  
        // Test data.                                                                                                  
        const bool isGoodbyeEnabled(true);                                                                             
                                                                                                                       
        // Setup.                                                                                                      
        const bool idleTimeout(false);                                                                                 
        const DWORD idleTimeoutDuration(0);                                                                            
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(objA);                                                                                     
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.IsGoodbyeEnabled(), isGoodbyeEnabled);                                                       
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(const CEmployee &)                                                                    
    /// Test for m_idleTimeout minimum value.                                                                          
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CopyConstructor_IdleTimeoutMinTest)                                                                    
    {                                                                                                                  
        // Test data.                                                                                                  
        const bool idleTimeout(false);                                                                                 
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const DWORD idleTimeoutDuration(0);                                                                            
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(objA);                                                                                     
               
        bool EmployeeIdleTimeout = GetEmployeeidleTimeout(obj);                                                                                                        
        // Test(s).                                                                                                    
        AssertResult2(EmployeeIdleTimeout, idleTimeout);                                                                 
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(const CEmployee &)                                                                    
    /// Test for m_idleTimeout maximum value.                                                                          
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CopyConstructor_IdleTimeoutMaxTest)                                                                    
    {                                                                                                                  
        // Test data.                                                                                                  
        const bool idleTimeout(true);                                                                                  
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const DWORD idleTimeoutDuration(0);                                                                            
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(objA);                                                                                     
                 
        bool EmployeeIdleTimeout = GetEmployeeidleTimeout(obj);                                                                                                      
        // Test(s).                                                                                                    
        AssertResult2(EmployeeIdleTimeout, idleTimeout);                                                                 
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(const CEmployee &)                                                                    
    /// Test for m_idleTimeoutDuration minimum value.                                                                  
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CopyConstructor_IdleTimeoutDurationMinTest)                                                            
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(0);                                                                            
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(false);                                                                                 
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(objA);                                                                                     
                    
        DWORD EmployeeTimeoutDuration = GetEmployeeidleTimeoutDuration(obj);                                                                                                   
        // Test(s).                                                                                                    
        AssertResult2(EmployeeTimeoutDuration, idleTimeoutDuration);                                                 
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(const CEmployee &)                                                                    
    /// Test for m_idleTimeoutDuration maximum value.                                                                  
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CopyConstructor_IdleTimeoutDurationMaxTest)                                                            
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(DWORD_MAX);                                                                    
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(false);                                                                                 
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(objA);                                                                                     
                 
        DWORD EmployeeTimeoutDuration = GetEmployeeidleTimeoutDuration(obj);                                                                                                      
        // Test(s).                                                                                                    
        AssertResult2(EmployeeTimeoutDuration, idleTimeoutDuration);                                                 
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(const CEmployee &)                                                                    
    /// Test for m_activityTimestamp minimum value.                                                                    
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CopyConstructor_ActivityTimestampMinTest)                                                              
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD activityTimestamp(0);                                                                              
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(false);                                                                                 
        const DWORD idleTimeoutDuration(0);                                                                            
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(objA);                                                                                     
                                    
        DWORD EmployeeactivityTimestamp = GetEmployeeActivityTimestamp(obj);                                                                                   
        // Test(s).                                                                                                    
        AssertResult2(EmployeeactivityTimestamp, activityTimestamp);                                                     
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(const CEmployee &)                                                                    
    /// Test for m_activityTimestamp maximum value.                                                                    
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, CopyConstructor_ActivityTimestampMaxTest)                                                              
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD activityTimestamp(DWORD_MAX);                                                                      
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(false);                                                                                 
        const DWORD idleTimeoutDuration(0);                                                                            
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(objA);                                                                                     
                       
        DWORD EmployeeactivityTimestamp = GetEmployeeActivityTimestamp(obj);                                                                                                
        // Test(s).                                                                                                    
        AssertResult2(EmployeeactivityTimestamp, activityTimestamp);                                                     
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::operator=(const CEmployee &)                                                                    
    ///     CEmployee::IsGoodbyeEnabled()                                                                              
    /// Test for IsGoodbyeEnabled() minimum value.                                                                     
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, AssignmentOperator_GoodbyeEnabledMinTest)                                                              
    {                                                                                                                  
        // Test data.                                                                                                  
        const bool isGoodbyeEnabled(false);                                                                            
                                                                                                                       
        // Setup.                                                                                                      
        const bool idleTimeout(false);                                                                                 
        const DWORD idleTimeoutDuration(0);                                                                            
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
        CEmployee varObj(card, config, !isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);        
                                                                                                                       
        // Verify setup.                                                                                               
        AssertResult2(varObj.IsGoodbyeEnabled(), !isGoodbyeEnabled);                                                   
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        varObj = objA;                                                                                                 
        const CEmployee &obj(varObj);                                                                                  
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.IsGoodbyeEnabled(), isGoodbyeEnabled);                                                       
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::operator=(const CEmployee &)                                                                    
    ///     CEmployee::IsGoodbyeEnabled()                                                                              
    /// Test for IsGoodbyeEnabled() maximum value.                                                                     
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, AssignmentOperator_GoodbyeEnabledMaxTest)                                                              
    {                                                                                                                  
        // Test data.                                                                                                  
        const bool isGoodbyeEnabled(true);                                                                             
                                                                                                                       
        // Setup.                                                                                                      
        const bool idleTimeout(false);                                                                                 
        const DWORD idleTimeoutDuration(0);                                                                            
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
        CEmployee varObj(card, config, !isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);        
                                                                                                                       
        // Verify setup.                                                                                               
        AssertResult2(varObj.IsGoodbyeEnabled(), !isGoodbyeEnabled);                                                   
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        varObj = objA;                                                                                                 
        const CEmployee &obj(varObj);                                                                                  
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.IsGoodbyeEnabled(), isGoodbyeEnabled);                                                       
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::operator=(const CEmployee &)                                                                    
    /// Test for m_idleTimeout minimum value.                                                                          
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, AssignmentOperator_IdleTimeoutMinTest)                                                                 
    {                                                                                                                  
        // Test data.                                                                                                  
        const bool idleTimeout(false);                                                                                 
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const DWORD idleTimeoutDuration(0);                                                                            
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
        CEmployee varObj(card, config, isGoodbyeEnabled, !idleTimeout, idleTimeoutDuration, activityTimestamp);        
                   
        bool EmployeeIdleTimeout_var = GetEmployeeidleTimeout(varObj);                                                                                                    
        // Verify setup.                                                                                               
        AssertResult2(EmployeeIdleTimeout_var, !idleTimeout);                                                             
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        varObj = objA;                                                                                                 
        const CEmployee &obj(varObj);                                                                                  
                
        bool EmployeeIdleTimeout = GetEmployeeidleTimeout(obj);                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(EmployeeIdleTimeout, idleTimeout);                                                                 
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::operator=(const CEmployee &)                                                                    
    /// Test for m_idleTimeout maximum value.                                                                          
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, AssignmentOperator_IdleTimeoutMaxTest)                                                                 
    {                                                                                                                  
        // Test data.                                                                                                  
        const bool idleTimeout(true);                                                                                  
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const DWORD idleTimeoutDuration(0);                                                                            
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
        CEmployee varObj(card, config, isGoodbyeEnabled, !idleTimeout, idleTimeoutDuration, activityTimestamp);        
                
        bool EmployeeIdleTimeout_var = GetEmployeeidleTimeout(varObj);                                                                                                       
        // Verify setup.                                                                                               
        AssertResult2(EmployeeIdleTimeout_var, !idleTimeout);                                                             
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        varObj = objA;                                                                                                 
        const CEmployee &obj(varObj);                                                                                  
                   
        bool EmployeeIdleTimeout = GetEmployeeidleTimeout(obj);                                                                                                    
        // Test(s).                                                                                                    
        AssertResult2(EmployeeIdleTimeout, idleTimeout);                                                                 
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::operator=(const CEmployee &)                                                                    
    /// Test for m_idleTimeoutDuration minimum value.                                                                  
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, AssignmentOperator_IdleTimeoutDurationMinTest)                                                         
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(0);                                                                            
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(false);                                                                                 
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
        CEmployee varObj(card, config, isGoodbyeEnabled, idleTimeout, ~idleTimeoutDuration, activityTimestamp);        
                                                    
        DWORD EmployeeIdleTimeoutDuration_var = GetEmployeeidleTimeoutDuration(varObj);                                                                   
        // Verify setup.                                                                                               
        AssertResult2(EmployeeIdleTimeoutDuration_var, ~idleTimeoutDuration);                                             
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        varObj = objA;                                                                                                 
        const CEmployee &obj(varObj);                                                                                  
                            
        DWORD EmployeeIdleTimeoutDuration = GetEmployeeidleTimeoutDuration(obj);                                                                                           
        // Test(s).                                                                                                    
        AssertResult2(EmployeeIdleTimeoutDuration, idleTimeoutDuration);                                                 
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::operator=(const CEmployee &)                                                                    
    /// Test for m_idleTimeoutDuration maximum value.                                                                  
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, AssignmentOperator_IdleTimeoutDurationMaxTest)                                                         
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(DWORD_MAX);                                                                    
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(false);                                                                                 
        const DWORD activityTimestamp(0);                                                                              
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
        CEmployee varObj(card, config, isGoodbyeEnabled, idleTimeout, ~idleTimeoutDuration, activityTimestamp);        
                                
        DWORD EmployeeIdleTimeoutDuration_var = GetEmployeeidleTimeoutDuration(varObj);                                                                                       
        // Verify setup.                                                                                               
        AssertResult2(EmployeeIdleTimeoutDuration_var, ~idleTimeoutDuration);                                             
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        varObj = objA;                                                                                                 
        const CEmployee &obj(varObj);                                                                                  
                   
        DWORD EmployeeIdleTimeoutDuration = GetEmployeeidleTimeoutDuration(obj);                                                                                                    
        // Test(s).                                                                                                    
        AssertResult2(EmployeeIdleTimeoutDuration, idleTimeoutDuration);                                                 
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::operator=(const CEmployee &)                                                                    
    /// Test for m_activityTimestamp minimum value.                                                                    
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, AssignmentOperator_ActivityTimestampMinTest)                                                           
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD activityTimestamp(0);                                                                              
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(false);                                                                                 
        const DWORD idleTimeoutDuration(0);                                                                            
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
        CEmployee varObj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, ~activityTimestamp);        
                                                          
        DWORD EmployeeactivityTimestamp_var = GetEmployeeActivityTimestamp(varObj);
        // Verify setup.                                                                                               
        AssertResult2(EmployeeactivityTimestamp_var, ~activityTimestamp);                                                 
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        varObj = objA;                                                                                                 
        const CEmployee &obj(varObj);                                                                                  
                                                                                   
        DWORD EmployeeactivityTimestamp = GetEmployeeActivityTimestamp(obj);                                    
        // Test(s).                                                                                                    
        AssertResult2(EmployeeactivityTimestamp, activityTimestamp);                                                     
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::operator=(const CEmployee &)                                                                    
    /// Test for m_activityTimestamp maximum value.                                                                    
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, AssignmentOperator_ActivityTimestampMaxTest)                                                           
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD activityTimestamp(DWORD_MAX);                                                                      
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(false);                                                                                 
        const DWORD idleTimeoutDuration(0);                                                                            
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        const CEmployee objA(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);     
        CEmployee varObj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, ~activityTimestamp);        
                      
        DWORD EmployeeactivityTimestamp_var = GetEmployeeActivityTimestamp(varObj);                                                                                                 
        // Verify setup.                                                                                               
        AssertResult2(EmployeeactivityTimestamp_var, ~activityTimestamp);                                                 
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        varObj = objA;                                                                                                 
        const CEmployee &obj(varObj);                                                                                  
                  
        DWORD EmployeeactivityTimestamp = GetEmployeeActivityTimestamp(obj);                                                                                                     
        // Test(s).                                                                                                    
        AssertResult2(EmployeeactivityTimestamp, activityTimestamp);                                                     
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for m_idleTimeout maximum value.                                                                          
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Constructor_IdleTimeoutMinTest)                                                 
    {                                                                                                                  
        // Test data.                                                                                                  
        const bool idleTimeout(false);                                                                                 
        const DWORD idleTimeoutDuration(0);                                                                            
        const DWORD activityTimestamp(0);                                                                              
        const DWORD tickCount(0);                                                                                      
        const bool hasIdleTimeoutOccurred(false);                                                                      
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for m_idleTimeout maximum value.                                                                          
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Constructor_IdleTimeoutMaxTest)                                                 
    {                                                                                                                  
        // Test data.                                                                                                  
        const bool idleTimeout(true);                                                                                  
        const DWORD idleTimeoutDuration(0);                                                                            
        const DWORD activityTimestamp(0);                                                                              
        const DWORD tickCount(0);                                                                                      
        const bool hasIdleTimeoutOccurred(true);                                                                       
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for m_idleTimeoutDuration minimum value -- always timed out.                                              
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Constructor_IdleTimeoutDurationMinTest)                                         
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(0);                                                                            
        const DWORD activityTimestamp(0);                                                                              
        const DWORD tickCount(0);                                                                                      
        const bool hasIdleTimeoutOccurred(true);                                                                       
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(true);                                                                                  
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for m_idleTimeoutDuration maximum value -- timeout not expired.                                           
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Constructor_IdleTimeoutDurationMax0Test)                                        
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(DWORD_MAX);                                                                    
        const DWORD activityTimestamp(0);                                                                              
        const DWORD tickCount(DWORD_MAX - 1);                                                                          
        const bool hasIdleTimeoutOccurred(false);                                                                      
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(true);                                                                                  
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for m_idleTimeoutDuration maximum value -- timeout expired.                                               
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Constructor_IdleTimeoutDurationMax1Test)                                        
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(DWORD_MAX);                                                                    
        const DWORD activityTimestamp(0);                                                                              
        const DWORD tickCount(DWORD_MAX);                                                                              
        const bool hasIdleTimeoutOccurred(true);                                                                       
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(true);                                                                                  
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for m_activityTimestamp minimum value -- for maximum expired threshold.                                   
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Constructor_ActivityTimestampMin0Test)                                          
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(5000);                                                                         
        const DWORD activityTimestamp(0);                                                                              
        const DWORD tickCount((DWORD)-1);                                                                              
        const bool hasIdleTimeoutOccurred(true);                                                                       
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(true);                                                                                  
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for m_activityTimestamp minimum value -- for minimum non-expired threshold.                               
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Constructor_ActivityTimestampMin1Test)                                          
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(5000);                                                                         
        const DWORD activityTimestamp(0);                                                                              
        const DWORD tickCount(0);                                                                                      
        const bool hasIdleTimeoutOccurred(false);                                                                      
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(true);                                                                                  
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for m_activityTimestamp minimum value -- for maximum non-expired threshold.                               
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Constructor_ActivityTimestampMin2Test)                                          
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(5000);                                                                         
        const DWORD activityTimestamp(0);                                                                              
        const DWORD tickCount(4999);                                                                                   
        const bool hasIdleTimeoutOccurred(false);                                                                      
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(true);                                                                                  
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for m_activityTimestamp minimum value -- for minimum expired threshold.                                   
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Constructor_ActivityTimestampMin3Test)                                          
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(5000);                                                                         
        const DWORD activityTimestamp(0);                                                                              
        const DWORD tickCount(5000);                                                                                   
        const bool hasIdleTimeoutOccurred(true);                                                                       
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(true);                                                                                  
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for m_activityTimestamp maximum value -- for maximum expired threshold.                                   
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Constructor_ActivityTimestampMax0Test)                                          
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(5000);                                                                         
        const DWORD activityTimestamp(DWORD_MAX);                                                                      
        const DWORD tickCount(DWORD_MAX - 1);                                                                          
        const bool hasIdleTimeoutOccurred(true);                                                                       
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(true);                                                                                  
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for m_activityTimestamp maximum value -- for minimum non-expired threshold.                               
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Constructor_ActivityTimestampMax1Test)                                          
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(5000);                                                                         
        const DWORD activityTimestamp(DWORD_MAX);                                                                      
        const DWORD tickCount(DWORD_MAX);                                                                              
        const bool hasIdleTimeoutOccurred(false);                                                                      
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(true);                                                                                  
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for m_activityTimestamp maximum value -- for maximum non-expired threshold.                               
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Constructor_ActivityTimestampMax2Test)                                          
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(5000);                                                                         
        const DWORD activityTimestamp(DWORD_MAX);                                                                      
        const DWORD tickCount((DWORD)(DWORD_MAX + (ULONGLONG)4999));                                                   
        const bool hasIdleTimeoutOccurred(false);                                                                      
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(true);                                                                                  
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for m_activityTimestamp maximum value -- for minimum expired threshold.                                   
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Constructor_ActivityTimestampMax3Test)                                          
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(5000);                                                                         
        const DWORD activityTimestamp(DWORD_MAX);                                                                      
        const DWORD tickCount((DWORD)(DWORD_MAX + (ULONGLONG)5000));                                                   
        const bool hasIdleTimeoutOccurred(true);                                                                       
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(true);                                                                                  
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        const CEmployee obj(card, config, isGoodbyeEnabled, idleTimeout, idleTimeoutDuration, activityTimestamp);      
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for updated m_activityTimestamp value -- for maximum expired threshold.                                   
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Update_ActivityTimestamp0Test)                                                  
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(5000);                                                                         
        const DWORD activityTimestamp(123456789);                                                                      
        const DWORD tickCount(123456789 - 1);                                                                          
        const bool hasIdleTimeoutOccurred(true);                                                                       
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(true);                                                                                  
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        CEmployee varObj(card,                                                                                         
            config,                                                                                                    
            isGoodbyeEnabled,                                                                                          
            idleTimeout,                                                                                               
            idleTimeoutDuration,                                                                                       
            activityTimestamp - idleTimeoutDuration);                                                                  
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        varObj.UpdateActivityTimestamp(activityTimestamp);                                                             
        const CEmployee &obj(varObj);                                                                                  
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for updated m_activityTimestamp value -- for minimum non-expired threshold.                               
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Update_ActivityTimestamp1Test)                                                  
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(5000);                                                                         
        const DWORD activityTimestamp(123456789);                                                                      
        const DWORD tickCount(123456789);                                                                              
        const bool hasIdleTimeoutOccurred(false);                                                                      
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(true);                                                                                  
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        CEmployee varObj(card,                                                                                         
            config,                                                                                                    
            isGoodbyeEnabled,                                                                                          
            idleTimeout,                                                                                               
            idleTimeoutDuration,                                                                                       
            activityTimestamp - idleTimeoutDuration);                                                                  
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        varObj.UpdateActivityTimestamp(activityTimestamp);                                                             
        const CEmployee &obj(varObj);                                                                                  
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for updated m_activityTimestamp value -- for maximum non-expired threshold.                               
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Update_ActivityTimestamp2Test)                                                  
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(5000);                                                                         
        const DWORD activityTimestamp(123456789);                                                                      
        const DWORD tickCount(123456789 + 4999);                                                                       
        const bool hasIdleTimeoutOccurred(false);                                                                      
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(true);                                                                                  
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        CEmployee varObj(card,                                                                                         
            config,                                                                                                    
            isGoodbyeEnabled,                                                                                          
            idleTimeout,                                                                                               
            idleTimeoutDuration,                                                                                       
            activityTimestamp - idleTimeoutDuration);                                                                  
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        varObj.UpdateActivityTimestamp(activityTimestamp);                                                             
        const CEmployee &obj(varObj);                                                                                  
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }                                                                                                                  
                                                                                                                       
    /// <summary>                                                                                                      
    /// Code coverage and functional test for:                                                                         
    ///     CEmployee::CEmployee(CCard *)                                                                              
    ///     CEmployee::HasIdleTimeoutOccurred()                                                                        
    /// Test for updated m_activityTimestamp value -- for minimum expired threshold.                                   
    /// </summary>                                                                                                     
    TEST_F(EmployeeUnitTests, HasIdleTimeoutOccurred_Update_ActivityTimestamp3Test)                                                  
    {                                                                                                                  
        // Test data.                                                                                                  
        const DWORD idleTimeoutDuration(5000);                                                                         
        const DWORD activityTimestamp(123456789);                                                                      
        const DWORD tickCount(123456789 + 5000);                                                                       
        const bool hasIdleTimeoutOccurred(true);                                                                       
                                                                                                                       
        // Setup.                                                                                                      
        const bool isGoodbyeEnabled(false);                                                                            
        const bool idleTimeout(true);                                                                                  
        CCard * const card(NULL);                                                                                      
        const CConfig config(false, "");                                                                               
        CEmployee varObj(card,                                                                                         
            config,                                                                                                    
            isGoodbyeEnabled,                                                                                          
            idleTimeout,                                                                                               
            idleTimeoutDuration,                                                                                       
            activityTimestamp - idleTimeoutDuration);                                                                  
                                                                                                                       
        // Perform operation(s) to be tested.                                                                          
        varObj.UpdateActivityTimestamp(activityTimestamp);                                                             
        const CEmployee &obj(varObj);                                                                                  
                                                                                                                       
        // Test(s).                                                                                                    
        AssertResult2(obj.HasIdleTimeoutOccurred(tickCount), hasIdleTimeoutOccurred);                                  
    }       
}                                                                                                        
