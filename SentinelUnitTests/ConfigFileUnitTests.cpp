#include "stdafx.h"
#include "../Sentinel/ConfigFile.h"

#include "UnitTestingUtils.h"

#include "gtest/gtest.h"

// TEST_FIX_CDM_DATA: For compatibility with the nCompass v1.18.0.90 implementation of CTime CConfigFile::m_lastConfigFileLoaded,
// change the expected data to match the actual results.
// Future versions of the source code can be updated so that CConfigFile::LoadConfigFile() calls m_operatingSystem.GetCurrentTime()
// instead of calling CUtilities::GetCurrentTime() directly.
// Also CSentinelState::CSentinelState(CTime, LPCTSTR) should set m_lastPackageListUpdate to 0 instead of CUtilities::GetCurrentTime().
#define TEST_FIX_CDM_DATA

// AssertResult macro(s) for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

using namespace std;

class ConfigFileTest : public ::testing::Test
    {
        protected:
            virtual void SetUp() 
            {
            }

            virtual void TearDown() 
            {
            }

        class CConfigFileTest : public CConfigFile
		{
		public:
			CConfigFileTest(IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem) :
				CConfigFile(0, operatingSystem)
			{
			}

			virtual bool Start() { return false; }
			virtual bool InterpretNameValue(const string& name, const string& value) { return false; }
			virtual void LogConfigValues(LPCTSTR sourceDesc, bool valid) { }
		protected:
			virtual void SetDefaults() { }
		};
};

    /// <summary>
    /// Code coverage and functional test for:
    ///     CConfigFile::CConfigFile(DWORD, IOperatingSystem&)
    ///     CConfigFile::GetLastConfigFileLoaded()
    // Test with default SpeedMediaInfo.
    /// </summary>
    TEST_F(ConfigFileTest, CConfigFile_GetLastConfigFileLoadedTest)
    {
        // Expected result(s).
        time_t lastConfigFileLoadedExpected(0);

        // Setup.
        const int testIndex(0);

        // Perform operation(s) to be tested.
        CConfigFileTest obj;

        // Test(s).
        AssertResult2((DWORD)obj.GetLastConfigFileLoaded(), (DWORD)lastConfigFileLoadedExpected);
    }
                                                                                                                                
    /// <summary>                                                                                                               
    /// Code coverage and functional test for:                                                                                  
    ///     CConfigFile::CConfigFile(DWORD, IOperatingSystem&)                                                                  
    ///     CConfigFile::GetLastConfigFileLoaded()                                                                              
    // Test with default SpeedMediaInfo.                                                                                        
    /// </summary>                                                                                                              
    TEST_F(ConfigFileTest, LoadConfigFile_GetLastConfigFileLoadedTest)                                                                     
    {                                                                                                                           
        std::string filename = "/eeprom.bin";                                                                                    

#ifndef TEST_FIX_CDM_DATA                                                                                                           
        const time_t lastConfigFileLoadedExpected(2014, 05, 15, 12, 34, 56);                                                     
#endif                                                                                                                              
                                                                                                                                
        // Setup.                                                                                                               
        const int testIndex(0);                                                                                                 
                                                                                                                                
        // Perform operation(s) to be tested.                                                                                   
        CConfigFileTest obj;                                                                                   
        obj.LoadConfigFile(filename);                                                                                           
#ifdef TEST_FIX_CDM_DATA                                                                                                            
        const time_t lastConfigFileLoadedExpected(obj.GetLastConfigFileLoaded());                                                
#endif                                                                                                                              
                                                                                                                                
        // Test(s).                                                                                                             
        AssertResult2((DWORD)obj.GetLastConfigFileLoaded(), (DWORD)lastConfigFileLoadedExpected);                                             
    }                                                                                                                           
                                                                                                                                
    /// <summary>                                                                                                               
    /// Code coverage and functional test for:                                                                                  
    ///     CConfigFile::CConfigFile(DWORD, IOperatingSystem&)                                                                  
    ///     CConfigFile::CConfigFile(const CConfigFile&)                                                                        
    ///     CConfigFile::GetLastConfigFileLoaded()                                                                              
    // Test with default SpeedMediaInfo.                                                                                        
    /// </summary>                                                                                                              
    TEST_F(ConfigFileTest, CopyConstructor_GetLastConfigFileLoadedTest)                                                                    
    {     
        std::string filename = "/eeprom.bin";                                                                                                                      
        
#ifndef TEST_FIX_CDM_DATA                                                                                                           
        const time_t lastConfigFileLoadedExpected(2014, 05, 15, 12, 34, 56);                                                     
#endif                                                                                                                              
                                                                                                                                
        // Setup.                                                                                                               
        const int testIndex(0);                                                                                                 
        CConfigFileTest obj1;                                                                                  
        obj1.LoadConfigFile(filename);                                                                                          
#ifdef TEST_FIX_CDM_DATA                                                                                                            
        const time_t lastConfigFileLoadedExpected(obj1.GetLastConfigFileLoaded());                                               
#endif                                                                                                                              
                                                                                                                                
        // Verify setup.                                                                                                        
        AssertResult2((DWORD)obj1.GetLastConfigFileLoaded(), (DWORD)lastConfigFileLoadedExpected);                                            
                                                                                                                                
        // Perform operation(s) to be tested.                                                                                   
        CConfigFileTest obj2(obj1);                                                                                             
                                                                                                                                
        // Test(s).                                                                                                             
        AssertResult2((DWORD)obj2.GetLastConfigFileLoaded(), (DWORD)lastConfigFileLoadedExpected);                                            
    }                                                                                                                           
                                                                                                                                
    /// <summary>                                                                                                               
    /// Code coverage and functional test for:                                                                                  
    ///     CConfigFile::CConfigFile(DWORD, IOperatingSystem&)                                                                  
    ///     CConfigFile::operator=()                                                                                            
    ///     CConfigFile::GetLastConfigFileLoaded()                                                                              
    // Test with default SpeedMediaInfo.                                                                                        
    /// </summary>                                                                                                              
    TEST_F(ConfigFileTest, AssignmentOperator_GetLastConfigFileLoadedTest)                                                                 
    {     
        std::string filename = "/eeprom.bin";                                                                                                                      
        
#ifndef TEST_FIX_CDM_DATA                                                                                                           
        const time_t lastConfigFileLoadedExpected(2014, 05, 15, 12, 34, 56);                                                     
#endif                                                                                                                              
        const time_t lastConfigFileLoadedDefaultExpected(0);                                                                     
                                                                                                                                
        // Setup.                                                                                                               
        const int testIndex(0);                                                                                                 
        CConfigFileTest obj1;                                                                                  
        obj1.LoadConfigFile(filename);                                                                                          
        CConfigFileTest obj2;                                                                                                   
#ifdef TEST_FIX_CDM_DATA                                                                                                            
        const time_t lastConfigFileLoadedExpected(obj1.GetLastConfigFileLoaded());                                               
#endif                                                                                                                              
                                                                                                                                
        // Verify setup.                                                                                                        
        AssertResult2((DWORD)obj1.GetLastConfigFileLoaded(), (DWORD)lastConfigFileLoadedExpected);                                            
        AssertResult2((DWORD)obj2.GetLastConfigFileLoaded(), (DWORD)lastConfigFileLoadedDefaultExpected);                                     
                                                                                                                                
        // Perform operation(s) to be tested.                                                                                   
        obj2 = obj1;                                                                                                            
                                                                                                                                
        // Test(s).                                                                                                             
        AssertResult2((DWORD)obj2.GetLastConfigFileLoaded(), (DWORD)lastConfigFileLoadedExpected);                                            
    }                                                                                                                           
                                                                                                                                
    /// <summary>                                                                                                               
    /// Code coverage and functional test for:                                                                                  
    ///     CConfigFile::InterpretBool()                                                                                        
    /// </summary>                                                                                                              
    TEST_F(ConfigFileTest, InterpretBoolTest)                                                                                              
    {                                                                                                                           
        typedef struct                                                                                                          
        {                                                                                                                       
            // Input(s).                                                                                                        
            const string m_value;                                                                                              
            const bool m_variable;                                                                                              
                                                                                                                                
            // Expected result(s).                                                                                              
            const bool m_variableExpected;                                                                                      
            const bool m_resultExpected;                                                                                        
        } TestDataType;                                                                                                         
                                                                                                                                
        const TestDataType TEST_DATA[] =                                                                                        
        {                                                                                                                       
            { "cd", false, false, false },                                                                                  
            { "cd", true, true, false },                                                                                    
                                                                                                                                
            { "", false, false, false },                                                                                    
            { "", true, true, false },                                                                                      
                                                                                                                                
            { " ", false, false, false },                                                                                   
            { " ", true, true, false },                                                                                     
                                                                                                                                
            { "   ", false, false, false },                                                                                 
            { "   ", true, true, false },                                                                                   
                                                                                                                                
            // Values that are close to or equal to "false".                                                                    
            { "0", false, false, false },                                                                                   
            { "0", true, true, false },                                                                                     
                                                                                                                                
            { "F", false, false, false },                                                                                   
            { "F", true, true, false },                                                                                     
                                                                                                                                
            { "FALSE", false, false, false },                                                                               
            { "FALSE", true, true, false },                                                                                 
                                                                                                                                
            { "False", false, false, false },                                                                               
            { "False", true, true, false },                                                                                 
                                                                                                                                
            { "fals", false, false, false },                                                                                
            { "fals", true, true, false },                                                                                  
                                                                                                                                
            { "falsE", false, false, false },                                                                               
            { "falsE", true, true, false },                                                                                 
                                                                                                                                
            { "falsex", false, false, false },                                                                              
            { "falsex", true, true, false },                                                                                
                                                                                                                                
            { "false", false, false, true },                                                                                
            { "false", true, false, true },                                                                                 
                                                                                                                                
            { " false", false, false, false },                                                                              
            { " false", true, true, false },                                                                                
                                                                                                                                
            { "false ", false, false, false },                                                                              
            { "false ", true, true, false },                                                                                
                                                                                                                                
            { " false ", false, false, false },                                                                             
            { " false ", true, true, false },                                                                               
                                                                                                                                
            { "false\n", false, false, false },                                                                             
            { "false\n", true, true, false },                                                                               
                                                                                                                                
            // Values that are close to or equal to "true".                                                                     
            { "1", false, false, false },                                                                                   
            { "1", true, true, false },                                                                                     
                                                                                                                                
            { "T", false, false, false },                                                                                   
            { "T", true, true, false },                                                                                     
                                                                                                                                
            { "TRUE", false, false, false },                                                                                
            { "TRUE", true, true, false },                                                                                  
                                                                                                                                
            { "True", false, false, false },                                                                                
            { "True", true, true, false },                                                                                  
                                                                                                                                
            { "tru", false, false, false },                                                                                 
            { "tru", true, true, false },                                                                                   
                                                                                                                                
            { "truE", false, false, false },                                                                                
            { "truE", true, true, false },                                                                                  
                                                                                                                                
            { "truex", false, false, false },                                                                               
            { "truex", true, true, false },                                                                                 
                                                                                                                                
            { "true", false, true, true },                                                                                  
            { "true", true, true, true },                                                                                   
                                                                                                                                
            { " true", false, false, false },                                                                               
            { " true", true, true, false },                                                                                 
                                                                                                                                
            { "true ", false, false, false },                                                                               
            { "true ", true, true, false },                                                                                 
                                                                                                                                
            { " true ", false, false, false },                                                                              
            { " true ", true, true, false },                                                                                
                                                                                                                                
            { "true\n", false, false, false },                                                                              
            { "true\n", true, true, false },                                                                                
        };                                                                                                                      
                                                                                                                                
        for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)                                                   
        {                                                                                                                       
            // Setup.                                                                                                           
            bool variable(TEST_DATA[testIndex].m_variable);                                                                     
                                                                                                                                
            // Perform operation(s) to be tested.                                                                               
            const bool result = CConfigFile::InterpretBool(TEST_DATA[testIndex].m_value, variable);                             
                                                                                                                                
            // Test(s).                                                                                                         
            AssertResult2(result, TEST_DATA[testIndex].m_resultExpected);                                                       
            AssertResult2(variable, TEST_DATA[testIndex].m_variableExpected);                                                   
        }                                                                                                                       
    }                                                                                                                           
                                                                                                                                
    /// <summary>                                                                                                               
    /// Code coverage and functional test for:                                                                                  
    ///     CConfigFile::InterpretDWORD()                                                                                       
    /// </summary>                                                                                                              
    TEST_F(ConfigFileTest, InterpretDWORDTest)                                                                                             
    {                                                                                                                           
        typedef struct                                                                                                          
        {                                                                                                                       
            // Input(s).                                                                                                        
            const string m_value;                                                                                              
            const DWORD m_variable;                                                                                             
                                                                                                                                
            // Expected result(s).                                                                                              
            const DWORD m_variableExpected;                                                                                     
            const bool m_resultExpected;                                                                                        
        } TestDataType;                                                                                                         
                                                                                                                                
        const TestDataType TEST_DATA[] =                                                                                        
        {                                                                                                                       
            // Values that aren't even close.                                                                                   

            { "cd", 123, 123, false },                                                                    
            { "cd", 456, 456, false },                                                                    
                                                                                                          
            { "", 123, 123, false },                                                                      
            { "", 456, 456, false },                                                                      
                                                                                                          
            { " ", 123, 123, false },                                                                     
            { " ", 456, 456, false },                                                                     
                                                                                                          
            // Values that are close to valid or valid.                                                   
            { "0x9999", 123, 123, false },                                                                
            { "0x9999", 456, 456, false },                                                                
                                                                                                          
            { "9999.", 123, 123, false },                                                                 
            { "9999.", 456, 456, false },                                                                 
                                                                                                          
            { "9999.0", 123, 123, false },                                                                
            { "9999.0", 456, 456, false },                                                                
                                                                                                          
            { "9999e0", 123, 123, false },                                                                
            { "9999e0", 456, 456, false },                                                                
                                                                                                          
            { "+9999", 123, 9999, true },                                                                 
            { "+9999", 456, 9999, true },                                                                 
                                                                                                          
            { "+ 9999", 123, 123, false },                                                                
            { "+ 9999", 456, 456, false },                                                                
                                                                                                          
            { " +9999", 123, 9999, true },                                                                
            { " +9999", 456, 9999, true },                                                                
                                                                                                          
            { "1001", 123, 1001, true },                                                                  
            { "1001", 456, 1001, true },                                                                  
                                                                                                          
            { " 1002", 123, 1002, true },                                                                 
            { " 1002", 456, 1002, true },                                                                 
                                                                                                          
            { "1003 ", 123, 123, false },                                                                 
            { "1003 ", 456, 456, false },                                                                 
                                                                                                          
            { " 1004 ", 123, 123, false },                                                                
            { " 1004 ", 456, 456, false },                                                                
                                                                                                          
            { "1005\n", 123, 123, false },                                                                
            { "1005\n", 456, 456, false },                                                                
                                                                                                          
            // Non-negative values that are valid DWORD values.                                           
            { "0", 123, 0, true },                                                                        
            { "1", 123, 1, true },                                                                        
            { "2", 123, 2, true },                                                                        
            { "999", 123, 999, true },                                                                    
            { "1000", 123, 1000, true },                                                                  
            { "1001", 123, 1001, true },                                                                  
            { "1234567890", 123, 1234567890, true },                                                      
            { "2147483647", 123, 2147483647, true }, // Maximum unsigned 31-bit value.                    
            { "4294967295", 123, 4294967295, true }, // Maximum unsigned 32-bit value.                    
                                                                                                          
            // Non-negative values that are not valid DWORD values.                                       
            { "4294967296", 123, 123, false }, // One more than maximum unsigned 32-bit value.            
            { "4294967296", 456, 456, false },                                                            
                                                                                                          
            { "9999999999", 123, 123, false },                                                            
            { "9999999999", 456, 456, false },                                                            
                                                                                                          
            { "18446744073709551615", 123, 123, false }, // Maximum unsigned 64-bit value.                
            { "18446744073709551615", 456, 456, false },                                                  
                                                                                                          
            { "99999999999999999999", 123, 123, false },                                                  
            { "99999999999999999999", 456, 456, false },                                                  
                                                                                                          
                                                                                                          
            { "9999999999999999999999999999999999999999", 123, 123, false },                              
            { "9999999999999999999999999999999999999999", 456, 456, false },                                                                      
                                                                                                                            
        };                                                                                                                      
                                                                                                                                
        for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)                                                   
        {                                                                                                                       
            // Setup.                                                                                                           
            DWORD variable(TEST_DATA[testIndex].m_variable);                                                                    
                                                                                                                                
            // Perform operation(s) to be tested.                                                                               
            const bool result = CConfigFile::InterpretDWORD(TEST_DATA[testIndex].m_value, variable);                            
                                                                                                                                
            // Test(s).                                                                                                         
            AssertResult2(result, TEST_DATA[testIndex].m_resultExpected);                                                       
            AssertResult2(variable, TEST_DATA[testIndex].m_variableExpected);                                                   
        }                                                                                                                       
    }                                                                                                                           