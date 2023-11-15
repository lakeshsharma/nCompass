#include "stdafx.h"

#include "resource.h"
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelUnitTests
{
	static const int MEMCMP_EQUAL = 0; // Return value for memcmp() indicating equality.
	const byte MEMSET_DEFAULT_VALUE = 0xcd;

        TEST(UtilitiesTests, UrlEncodeTest)
		{
			// Setup.
            //char instringArray[12] =  {'J','o', 's', (char)138, (char)96, 's', ' ', 'L', 'e', 'v', 'e', 'l'};
			string inString("Jos?'s Level");
			string expectedUrlEncodedString("Jos?'s Level");

			// Test and Report.
			ASSERT_TRUE(expectedUrlEncodedString == CUtilities::UrlEncode(inString)) <<  L"Incorrect result returned by CUtilities::UrlEncode()";

			// Cleanup.
			const DWORD END_OF_TEST_SLEEP_IN_MILLISECONDS = 1000; // 1000 milliseconds == 1 second. 
			// Sleep so execution engine doesn't crash as often (with errors like the following) after this test method completes.
			//     2013-02-05  A Sleep of 1 to 3 seconds seem to work well on the S300-VW7 Window 7 virtual machine.
			// Errors seem to occur more often when running in the IDE via "Debug Selected Tests" for this individual test, with no breakpoints set.
			//     Unhandled exception at 0x10034d66 in vstest.executionengine.x86.exe: 0xC0000005: Access violation.
			//     Source Not Available
			//     Frame not in module
			//     The current stack frame was not found in a loaded module. Source cannot be shown for this location.
			//     You can view disassembly in the Disassembly window. To always view disassembly for missing source files,
			//     change the setting in the Options dialog.
			// Sleep(END_OF_TEST_SLEEP_IN_MILLISECONDS);
		}

        TEST(UtilitiesTests, ConvertCharsToCharsTest)                                                                                           
        {                                                                                                                                           
            // Setup.                                                                                                                               
            const int length = 5;                                                                                                                   
            LPCTSTR inString = "abc";                                                                                                               
            char outChars[length];                                                                                                                  
            const char outCharsRequired[length] = { 'a', 'b', 'c', 0, 0};                                                                           
                                                                                                                                                    
            // Test.                                                                                                                                
            CUtilities::ConvertCharsToChars(inString, outChars, length);                                                                        
                                                                                                                                                    
            // Report.                                                                                                                              
            ASSERT_TRUE(0 == memcmp(outChars, outCharsRequired, length)) <<  L"Incorrect result returned by CUtilities::ConvertWideCharsToChars()"; 
                                                                                                                                                    
            // Cleanup.                                                                                                                             
            const DWORD END_OF_TEST_SLEEP_IN_MILLISECONDS = 1000; // 1000 milliseconds == 1 second.                                                 
            // Sleep so execution engine doesn't crash as often (with errors like the following) after this test method completes.                  
            //     2013-02-05  A Sleep of 1 to 3 seconds seem to work well on the S300-VW7 Window 7 virtual machine.                                
            // Errors seem to occur more often when running in the IDE via "Debug Selected Tests" for this individual test, with no breakpoints set.
            //     Unhandled exception at 0x10034d66 in vstest.executionengine.x86.exe: 0xC0000005: Access violation.                               
            //     Source Not Available                                                                                                             
            //     Frame not in module                                                                                                              
            //     The current stack frame was not found in a loaded module. Source cannot be shown for this location.                              
            //     You can view disassembly in the Disassembly window. To always view disassembly for missing source files,                         
            //     change the setting in the Options dialog.                                                                                        
            // Sleep(END_OF_TEST_SLEEP_IN_MILLISECONDS);                                                                                            
        }                                                                                                                                           


		TEST(UtilitiesTests, ConvertCharsToCharsTest2)
		{
			// Data-driven tests.
			typedef struct
			{
				const LPCTSTR inString;
				const int length;
				const char *outCharsExpected;
				const size_t outCharsExpectedLength;
			} TestDataType;

			const char outCharsExpected0[1] = { (char)0xcd };
			const char outCharsExpected1[2] = { 0, (char)0xcd };
			const char outCharsExpected2[3] = { 0, 0, (char)0xcd };

			const char outCharsExpected3[1] = { (char)0xcd };
			const char outCharsExpected4[2] = { 0, (char)0xcd };
			const char outCharsExpected5[3] = { 'a', 0, (char)0xcd };
			const char outCharsExpected6[4] = { 'a', 0, 0, (char)0xcd };

			const char outCharsExpected7[1] = { (char)0xcd };
			const char outCharsExpected8[2] = { 0, (char)0xcd };
			const char outCharsExpected9[3] = { 'a', 0, (char)0xcd };
			const char outCharsExpected10[4] = { 'a', 'b', 0, (char)0xcd };
			const char outCharsExpected11[5] = { 'a', 'b', 0, 0, (char)0xcd };

			const TestDataType TEST_DATA[] =
			{
				{ "", _countof(outCharsExpected0) - 1, outCharsExpected0, _countof(outCharsExpected0) },
				{ "", _countof(outCharsExpected1) - 1, outCharsExpected1, _countof(outCharsExpected1) },
				{ "", _countof(outCharsExpected2) - 1, outCharsExpected2, _countof(outCharsExpected2) },

				{ "a", _countof(outCharsExpected3) - 1, outCharsExpected3, _countof(outCharsExpected3) },
				{ "a", _countof(outCharsExpected4) - 1, outCharsExpected4, _countof(outCharsExpected4) },
				{ "a", _countof(outCharsExpected5) - 1, outCharsExpected5, _countof(outCharsExpected5) },
				{ "a", _countof(outCharsExpected6) - 1, outCharsExpected6, _countof(outCharsExpected6) },

				{ "ab", _countof(outCharsExpected7) - 1, outCharsExpected7, _countof(outCharsExpected7) },
				{ "ab", _countof(outCharsExpected8) - 1, outCharsExpected8, _countof(outCharsExpected8) },
				{ "ab", _countof(outCharsExpected9) - 1, outCharsExpected9, _countof(outCharsExpected9) },
				{ "ab", _countof(outCharsExpected10) - 1, outCharsExpected10, _countof(outCharsExpected10) },
				{ "ab", _countof(outCharsExpected11) - 1, outCharsExpected11, _countof(outCharsExpected11) },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				char *outChars = new char[TEST_DATA[testIndex].outCharsExpectedLength];
				memset(outChars, MEMSET_DEFAULT_VALUE, TEST_DATA[testIndex].outCharsExpectedLength);

				// Test.
				CUtilities::ConvertCharsToChars(TEST_DATA[testIndex].inString,
					outChars,
					TEST_DATA[testIndex].length);
				bool areEqual = 0 == memcmp(outChars, TEST_DATA[testIndex].outCharsExpected, TEST_DATA[testIndex].outCharsExpectedLength);

				// Report.
				string outCharsStr;
                string formatStr;
				for (size_t outCharsIndex = 0; outCharsIndex < TEST_DATA[testIndex].outCharsExpectedLength; ++outCharsIndex)
				{
					if (!outCharsStr.empty())
					{
                        char appendchar = ',';
						outCharsStr.append(&appendchar);
					}
                    int length = sprintf(const_cast<char*>(formatStr.c_str()), "0x%2.2x", (unsigned int)outChars[outCharsIndex]);
					outCharsStr.append(formatStr.c_str(), length);
				}

				string outCharsExpectedStr;
				for (size_t outCharsIndex = 0; outCharsIndex < TEST_DATA[testIndex].outCharsExpectedLength; ++outCharsIndex)
				{
					if (!outCharsExpectedStr.empty())
					{
                        char appendchar = ',';
						outCharsExpectedStr.append(&appendchar);
					}
                    int length = sprintf(const_cast<char*>(formatStr.c_str()), "0x%2.2x", (unsigned int)TEST_DATA[testIndex].outCharsExpected[outCharsIndex]);
					outCharsExpectedStr.append(formatStr.c_str(), length);
				}

				string failedMsg;
				failedMsg = FormatString("FAILED CUtilities::ConvertWideCharsToChars(\"%s\", \"%s\") outCharsExpected=\"%s\" testIndex=%d)",
					(TEST_DATA[testIndex].inString),
					(outCharsStr).c_str(),
					(outCharsExpectedStr).c_str(),
					testIndex);
				ASSERT_TRUE(areEqual) <<  failedMsg;

				// Test iteration cleanup.
				delete [] outChars;
			}

			// Test cleanup.
			const DWORD END_OF_TEST_SLEEP_IN_MILLISECONDS = 1000; // 1000 milliseconds == 1 second. 
			// Sleep so execution engine doesn't crash as often (with errors like the following) after this test method completes.
			//     2013-02-05  A Sleep of 1 to 3 seconds seem to work well on the S300-VW7 Window 7 virtual machine.
			// Errors seem to occur more often when running in the IDE via "Debug Selected Tests" for this individual test, with no breakpoints set.
			//     Unhandled exception at 0x10034d66 in vstest.executionengine.x86.exe: 0xC0000005: Access violation.
			//     Source Not Available
			//     Frame not in module
			//     The current stack frame was not found in a loaded module. Source cannot be shown for this location.
			//     You can view disassembly in the Disassembly window. To always view disassembly for missing source files,
			//     change the setting in the Options dialog.
			usleep(END_OF_TEST_SLEEP_IN_MILLISECONDS * 1000);
		}

		TEST(UtilitiesTests, HasFileExtensionTest)
		{
			typedef struct
			{
				const string filename;
				const string extension;
				const bool returnValueExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ "razor.ConFig", ".CoNfIg", true },
				{ "razor.ConFig", "fIg", true },
				{ "razor.ConFig", "", true },
				{ "razor.ConFig", ".CoNfIg", true },
				{ "razor.ConFig", "Or.CONfig", true },
				{ "r.ConFig", "Or.CONfig", false },
				{ "razor.ConFig", ".CON", false },
				{ "razor.", ".", true },
				{ "razor.", "", true },
				{ "razor", ".", true },
				{ "razor", "", true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Test.
				bool returnValue = CUtilities::HasFileExtension(TEST_DATA[testIndex].filename, TEST_DATA[testIndex].extension);

				// Report.
				string failedMsg;
				failedMsg = FormatString("FAILED CUtilities::HasFileExtension(\"%s\", \"%s\") returnValue=%d returnValueExpected=%d testIndex=%d)",
					(TEST_DATA[testIndex].filename).c_str(),
					(TEST_DATA[testIndex].extension).c_str(),
					returnValue,
					TEST_DATA[testIndex].returnValueExpected,
					testIndex);
				ASSERT_EQ(returnValue, TEST_DATA[testIndex].returnValueExpected)<< failedMsg;
			}
		}

		TEST(UtilitiesTests, IsEvenTest)
		{
			typedef struct
			{
				const __int64 num;
				const bool returnValueExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (__int64)0x8000000000000000, true },
				{ (__int64)0x8000000000000001, false },
				{ -8345678901234567890, true },
				{ -1234567890123456789, false },
				{ -123456789012345678, true },
				{ -12345678901234567, false },
				{ -1234567890123456, true },
				{ -123456789012345, false },
				{ -12345678901234, true },
				{ -1234567890123, false },
				{ -123456789012, true },
				{ -12345678901, false },
				{ 0x80000000, true },
				{ 0x80000001, false },
				{ -1234567890, true },
				{ -123456789, false },
				{ -12345678, true },
				{ -1234567, false },
				{ -123456, true },
				{ -12345, false },
				{ -1234, true },
				{ -123, false },
				{ -12, true },
				{ -9, false },
				{ -8, true },
				{ -7, false },
				{ -6, true },
				{ -5, false },
				{ -4, true },
				{ -3, false },
				{ -2, true },
				{ -1, false },
				{ 0, true },
				{ 1, false },
				{ 2, true },
				{ 3, false },
				{ 4, true },
				{ 5, false },
				{ 6, true },
				{ 7, false },
				{ 8, true },
				{ 9, false },
				{ 12, true },
				{ 123, false },
				{ 1234, true },
				{ 12345, false },
				{ 123456, true },
				{ 1234567, false },
				{ 12345678, true },
				{ 123456789, false },
				{ 1234567890, true },
				{ 0x7ffffffe, true },
				{ 0x7fffffff, false },
				{ 12345678901, false },
				{ 123456789012, true },
				{ 1234567890123, false },
				{ 12345678901234, true },
				{ 123456789012345, false },
				{ 1234567890123456, true },
				{ 12345678901234567, false },
				{ 123456789012345678, true },
				{ 1234567890123456789, false },
				{ 8345678901234567890, true },
				{ (__int64)0x7ffffffffffffffe, true },
				{ (__int64)0x7fffffffffffffff, false },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Test.
				bool returnValue = CUtilities::IsEven(TEST_DATA[testIndex].num);

				// Report.
				string failedMsg;
				failedMsg = FormatString("FAILED CUtilities::IsEven(%lld) returnValue=%d  returnValueExpected=%d testIndex=%d)",
					(long long)TEST_DATA[testIndex].num,
					returnValue,
					TEST_DATA[testIndex].returnValueExpected,
					testIndex);
				ASSERT_EQ(returnValue, TEST_DATA[testIndex].returnValueExpected)<< failedMsg;
			}
		}

		TEST(UtilitiesTests, MakeCardIDFromstringTest)
		{
			// Allocate extra memory before and after target buffer to check for unwanted overwrites.
			const unsigned int HeaderValue = 0xcdcdcdcd; // Each byte the same as MEMSET_DEFAULT_VALUE; 
			const size_t HeaderSize = sizeof(HeaderValue);
			const unsigned int TrailerValue = HeaderValue;
			const size_t TrailerSize = sizeof(TrailerValue);

			typedef struct
			{
				// Inputs.
				LPCTSTR m_text;
				int m_bufferLength; // Same length or less than m_bufferExpectedLength.

				// Expected results.
				const byte * const m_bufferExpected;
				size_t m_bufferExpectedSize;
			} TestDataType;

			const byte bufferA[] = { 0x01, 0x23, 0x45, 0x67, 0x89 };
			const byte bufferB[] = { 0x67, 0x89, 0xAB, 0xCD, 0xEF };
			const byte bufferC[] = { 0xFE, 0xDC, 0xBA, 0x98, 0x76 };
			const byte bufferD[] = { 0x01, 0x00, 0x00, 0x00, 0x00 };
			const byte bufferE[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

			const byte buffer0[] = { 0x00 };
			const byte buffer1[] = { 0x11 };
			const byte buffer2[] = { 0xcd };
			const byte buffer3[] = { 0x01 };
			const byte buffer4[] = { 0x12, 0x12 };
			const byte buffer5[] = { 0x12, 0x12, 0x00 };
			const byte buffer6[] = { 0x00, 0x34, 0x00 };
			const byte buffer7[] = { 0x23, 0x34 };
			const byte buffer8[] = { 0x34, 0x34 };
			const byte buffer9[] = { 0x11, 0x22, 0x33, 0x44, 0x55 };
			const byte buffer10[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };

			const TestDataType TEST_DATA[] =
			{
				{ "0123456789", sizeof(bufferA), bufferA, sizeof(bufferA) },
				{ "0123456789A", sizeof(bufferA), bufferA, sizeof(bufferA) },
				{ "0123456789AB", sizeof(bufferA), bufferA, sizeof(bufferA) },
				{ "0123456789ABC", sizeof(bufferA), bufferA, sizeof(bufferA) },
				{ "0123456789ABCZ", sizeof(bufferA), bufferA, sizeof(bufferA) },

				{ "6789ABCDEF", sizeof(bufferB), bufferB, sizeof(bufferB) },
				{ "6789ABCDEFA", sizeof(bufferB), bufferB, sizeof(bufferB) },
				{ "6789ABCDEFAB", sizeof(bufferB), bufferB, sizeof(bufferB) },
				{ "6789ABCDEFABC", sizeof(bufferB), bufferB, sizeof(bufferB) },
				{ "6789ABCDEFABCZ", sizeof(bufferB), bufferB, sizeof(bufferB) },
				{ "6789aBCDEF", sizeof(bufferB), bufferB, sizeof(bufferB) },
				{ "6789AbCDEFA", sizeof(bufferB), bufferB, sizeof(bufferB) },
				{ "6789ABcDEFAB", sizeof(bufferB), bufferB, sizeof(bufferB) },
				{ "6789ABCdEFABC", sizeof(bufferB), bufferB, sizeof(bufferB) },
				{ "6789ABCDeFABCZ", sizeof(bufferB), bufferB, sizeof(bufferB) },

				{ "FEDCBA9876", sizeof(bufferC), bufferC, sizeof(bufferC) },
				{ "FEDCBA9876A", sizeof(bufferC), bufferC, sizeof(bufferC) },
				{ "FEDCBA9876AB", sizeof(bufferC), bufferC, sizeof(bufferC) },
				{ "FEDCBA9876ABC", sizeof(bufferC), bufferC, sizeof(bufferC) },
				{ "FEDCBA9876ABCZ", sizeof(bufferC), bufferC, sizeof(bufferC) },
				{ "feDCBA9876", sizeof(bufferC), bufferC, sizeof(bufferC) },
				{ "FEdcBA9876A", sizeof(bufferC), bufferC, sizeof(bufferC) },
				{ "FEDCba9876AB", sizeof(bufferC), bufferC, sizeof(bufferC) },
				{ "fedCBA9876ABC", sizeof(bufferC), bufferC, sizeof(bufferC) },
				{ "FEDcba9876ABCZ", sizeof(bufferC), bufferC, sizeof(bufferC) },

				{ "01000000@", sizeof(bufferD), bufferD, sizeof(bufferD) },
				{ "010000000", sizeof(bufferD), bufferD, sizeof(bufferD) },
				{ "0100000000", sizeof(bufferD), bufferD, sizeof(bufferD) },
				{ "0100000000A", sizeof(bufferD), bufferD, sizeof(bufferD) },
				{ "0100000000AB", sizeof(bufferD), bufferD, sizeof(bufferD) },
				{ "0100000000ABC", sizeof(bufferD), bufferD, sizeof(bufferD) },
				{ "0100000000ABCZ", sizeof(bufferD), bufferD, sizeof(bufferD) },

				{ "FFFFFFFF@", sizeof(bufferE), bufferE, sizeof(bufferE) },
				{ "FFFFFFFFFF", sizeof(bufferE), bufferE, sizeof(bufferE) },
				{ "FFFFFFFFFFA", sizeof(bufferE), bufferE, sizeof(bufferE) },
				{ "FFFFFFFFFFAB", sizeof(bufferE), bufferE, sizeof(bufferE) },
				{ "FFFFFFFFFFABC", sizeof(bufferE), bufferE, sizeof(bufferE) },
				{ "FFFFFFFFFFABCZ", sizeof(bufferE), bufferE, sizeof(bufferE) },

				{ "", -33, NULL, 0},
				{ "", -1, NULL, 0},
				{ "", 0, NULL, 0},

				{ "g", sizeof(buffer0), buffer0, sizeof(buffer0) },
				{ " ", sizeof(buffer0), buffer0, sizeof(buffer0) },
				{ "@", sizeof(buffer0), buffer0, sizeof(buffer0) },
				{ "~", sizeof(buffer0), buffer0, sizeof(buffer0) },
				{ "\a", sizeof(buffer0), buffer0, sizeof(buffer0) },
				{ "\x01", sizeof(buffer0), buffer0, sizeof(buffer0) },
				{ "11", sizeof(buffer1), buffer1, sizeof(buffer1) },

				{ "11", -33, buffer2, sizeof(buffer2) },
				{ "11", -1, buffer2, sizeof(buffer2) },
				{ "11", 0, buffer2, sizeof(buffer2) },

				{ "1", sizeof(buffer3), buffer3, sizeof(buffer3) },

				{ "12g", sizeof(buffer4), buffer4, sizeof(buffer4) },
				{ "12 ", sizeof(buffer4), buffer4, sizeof(buffer4) },
				{ "12@", sizeof(buffer4), buffer4, sizeof(buffer4) },
				{ "12~", sizeof(buffer4), buffer4, sizeof(buffer4) },
				{ "12\a", sizeof(buffer4), buffer4, sizeof(buffer4) },
				{ "12\a", sizeof(buffer4), buffer4, sizeof(buffer4) },

				{ "12g", sizeof(buffer5), buffer5, sizeof(buffer5) },
				{ "12 ", sizeof(buffer5), buffer5, sizeof(buffer5) },
				{ "12@", sizeof(buffer5), buffer5, sizeof(buffer5) },
				{ "12~", sizeof(buffer5), buffer5, sizeof(buffer5) },
				{ "12\a", sizeof(buffer5), buffer5, sizeof(buffer5) },
				{ "12\x01", sizeof(buffer5), buffer5, sizeof(buffer5) },

				{ "g234", sizeof(buffer6), buffer6, sizeof(buffer6) },
				{ " .34", sizeof(buffer6), buffer6, sizeof(buffer6) },
				{ "@234", sizeof(buffer6), buffer6, sizeof(buffer6) },
				{ "~234", sizeof(buffer6), buffer6, sizeof(buffer6) },
				{ "\a234", sizeof(buffer6), buffer6, sizeof(buffer6) },
				//{ "\x01256" "234", sizeof(buffer6), buffer6, sizeof(buffer6) },

				{ " 234", sizeof(buffer7), buffer7, sizeof(buffer7) },
				{ "\t234", sizeof(buffer7), buffer7, sizeof(buffer7) },

				{ "  34", sizeof(buffer8), buffer8, sizeof(buffer8) },
				{ "   34", sizeof(buffer8), buffer8, sizeof(buffer8) },
				{ "    34", sizeof(buffer8), buffer8, sizeof(buffer8) },
				{ "     34", sizeof(buffer8), buffer8, sizeof(buffer8) },
				{ " \t34", sizeof(buffer8), buffer8, sizeof(buffer8) },
				{ " \t 34", sizeof(buffer8), buffer8, sizeof(buffer8) },
				{ " \t  34", sizeof(buffer8), buffer8, sizeof(buffer8) },
				{ " \t   34", sizeof(buffer8), buffer8, sizeof(buffer8) },

				{ "1122334455", sizeof(buffer9), buffer9, sizeof(buffer9) },

				{ "00112233445566778899AABBCCDDEEFF", sizeof(buffer10), buffer10, sizeof(buffer10) },
				{ "00112233445566778899aabbccddeeff", sizeof(buffer10), buffer10, sizeof(buffer10) },
				{ ".0112233445566778899aabbccddeeff", sizeof(buffer10), buffer10, sizeof(buffer10) },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				size_t bufferRawSize = HeaderSize + TEST_DATA[testIndex].m_bufferExpectedSize + TrailerSize;
				byte *bufferRaw = new byte[bufferRawSize];
				memset(bufferRaw, MEMSET_DEFAULT_VALUE, bufferRawSize);

				byte *buffer = bufferRaw + HeaderSize; // Target buffer.MakeCardIDFromCString

				// Perform operation(s) to be tested.
				CUtilities::MakeCardIDFromCString(TEST_DATA[testIndex].m_text, buffer, TEST_DATA[testIndex].m_bufferLength);

				// Test #1.
				{
					 // Verify that the header was not overwritten.
					const int headerMemcmpResult = memcmp(bufferRaw, &HeaderValue, HeaderSize);

					bool areEqual(headerMemcmpResult == MEMCMP_EQUAL);
					string failedMsg;
					const string headerStr = CUtilities::ToHexString(bufferRaw, HeaderSize);
					const string expectedStr = CUtilities::ToHexString((const byte *)&HeaderValue, HeaderSize);
					failedMsg = FormatString("FAILED headerMemcmpResult=%d expected=%d header=[%s] expected=[%s] testIndex=%d in %s",
						headerMemcmpResult,
						MEMCMP_EQUAL,
						(headerStr).c_str(),
						(expectedStr).c_str(),
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #2.
				{
					 // Verify that the trailer was not overwritten.
					const int trailerMemcmpResult = memcmp(bufferRaw + bufferRawSize - TrailerSize, &HeaderValue, HeaderSize);

					bool areEqual(trailerMemcmpResult == MEMCMP_EQUAL);
					string failedMsg;
					const string trailerStr = CUtilities::ToHexString(bufferRaw + bufferRawSize - TrailerSize, HeaderSize);
					const string expectedStr = CUtilities::ToHexString((const byte *)&HeaderValue, HeaderSize);
					failedMsg = FormatString("FAILED trailerMemcmpResult=%d expected=%d trailer=[%s] expected=[%s] testIndex=%d in %s",
						trailerMemcmpResult,
						MEMCMP_EQUAL,
						(trailerStr).c_str(),
						(expectedStr).c_str(),
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #3.
				{
					 // Verify that the target buffer is as expected.
					const int bufferMemcmpResult = memcmp(buffer, TEST_DATA[testIndex].m_bufferExpected, TEST_DATA[testIndex].m_bufferExpectedSize);

					bool areEqual(bufferMemcmpResult == MEMCMP_EQUAL);
					string failedMsg;
					const string bufferStr = CUtilities::ToHexString(buffer, TEST_DATA[testIndex].m_bufferExpectedSize);
					const string expectedStr = CUtilities::ToHexString(TEST_DATA[testIndex].m_bufferExpected, TEST_DATA[testIndex].m_bufferExpectedSize);
					failedMsg = FormatString("FAILED bufferMemcmpResult=%d expected=%d text=\"%s\" buffer=[%s] expected=[%s] testIndex=%d in %s",
						bufferMemcmpResult,
						MEMCMP_EQUAL,
						(TEST_DATA[testIndex].m_text),
						(bufferStr).c_str(),
						(expectedStr).c_str(),
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
				delete bufferRaw;
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUtilities::MakestringFromCardID()
		/// Test with a nullptr for the cardId buffer.
		/// </summary>
		TEST(UtilitiesTests, MakestringFromCardID_NullTest)
		{
			const byte * const cardId(nullptr);
			ASSERT_EQ(string(""), CUtilities::MakeStringFromCardID(cardId));
			ASSERT_EQ(string(""), CUtilities::MakeStringFromCardID(cardId, INT_MIN));
			ASSERT_EQ(string(""), CUtilities::MakeStringFromCardID(cardId, -1));
			ASSERT_EQ(string(""), CUtilities::MakeStringFromCardID(cardId, 0));
			ASSERT_EQ(string(""), CUtilities::MakeStringFromCardID(cardId, 1));
			ASSERT_EQ(string(""), CUtilities::MakeStringFromCardID(cardId, INT_MAX));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUtilities::MakestringFromCardID()
		/// Test with various cardIDLen values.
		/// </summary>
		TEST(UtilitiesTests, MakestringFromCardID_LengthTest)
		{
			const byte cardId[1] = { 0x5A };
			ASSERT_EQ(string(""), CUtilities::MakeStringFromCardID(cardId, INT_MIN));
			ASSERT_EQ(string(""), CUtilities::MakeStringFromCardID(cardId, -1));
			ASSERT_EQ(string(""), CUtilities::MakeStringFromCardID(cardId, 0));
			ASSERT_EQ(string("5A"), CUtilities::MakeStringFromCardID(cardId, _countof(cardId)));
		}

		TEST(UtilitiesTests, MakestringFromCardIDTest)
		{
			const int CARD_ID_LENGTH = 5;
			typedef struct
			{
				const byte cardID[CARD_ID_LENGTH];
				const string returnValueExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ { 0x00, 0x00, 0x00, 0x00, 0x00 }, "" },
				{ { 0x00, 0x23, 0x45, 0x67, 0x89 }, "" },
				{ { 0x01, 0x23, 0x45, 0x67, 0x89 }, "0123456789" },
				{ { 0x67, 0x89, 0xab, 0xcd, 0xef }, "6789ABCDEF" },
				{ { 0xfe, 0xdc, 0xba, 0x98, 0x76 }, "FEDCBA9876" },
				{ { 0x01, 0x00, 0x00, 0x00, 0x00 }, "0100000000" },
				{ { 0xff, 0xff, 0xff, 0xff, 0xff }, "FFFFFFFFFF" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Copy test data to non-constant variable(s).
				byte varCardID[CARD_ID_LENGTH];
				memcpy(varCardID, TEST_DATA[testIndex].cardID, sizeof varCardID);

				// Test.
				string returnValue = CUtilities::MakeStringFromCardID(varCardID);
				bool areEqual = returnValue == TEST_DATA[testIndex].returnValueExpected;

				// Report.
				string failedMsg;
				failedMsg = FormatString("FAILED CUtilities::MakestringFromCardID(&{0x%2.2x,0x%2.2x,0x%2.2x,0x%2.2x,0x%2.2x}) returnValue=\"%s\" returnValueExpected=\"%s\" testIndex=%d)",
					TEST_DATA[testIndex].cardID[0],
					TEST_DATA[testIndex].cardID[1],
					TEST_DATA[testIndex].cardID[2],
					TEST_DATA[testIndex].cardID[3],
					TEST_DATA[testIndex].cardID[4],
					(returnValue).c_str(),
					(TEST_DATA[testIndex].returnValueExpected).c_str(),
					testIndex);
				ASSERT_TRUE(areEqual) <<  failedMsg;
			}
		}

		// 	TEST(UtilitiesTests, )
		// Test both (overloaded) versions of CUtilities::MakestringFromBCD(), because they have similar limitations.
		TEST(UtilitiesTests, MakestringFromBCDTest)
		{
			const int MAXIMUM_BCD_ARRAY_LENGTH = 10;
			typedef struct
			{
				const byte bcdArray[MAXIMUM_BCD_ARRAY_LENGTH];
				const byte bcdLength;
				const string returnValueExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 10, "00000000000000000000" },
				{ { 0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34, 0x56, 0x78, 0x90 }, 10, "12345678901234567890" },
				{ { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0, "" },
				{ { 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 1, "12" },
				{ { 0x23, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 2, "2345" },
				{ { 0x34, 0x56, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 3, "345678" },
				{ { 0x45, 0x67, 0x89, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 4, "45678901" },
				{ { 0x56, 0x78, 0x90, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00 }, 5, "5678901234" },
				{ { 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x00, 0x00, 0x00 }, 6, "678901234567" },
				{ { 0x78, 0x90, 0x12, 0x34, 0x56, 0x78, 0x90, 0x00, 0x00, 0x00 }, 7, "78901234567890" },
				{ { 0x89, 0x01, 0x23, 0x45, 0x67, 0x89, 0x01, 0x23, 0x00, 0x00 }, 8, "8901234567890123" },
				{ { 0x90, 0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34, 0x56, 0x00 }, 9, "901234567890123456" },
				{ { 0x01, 0x23, 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x89 }, 10, "01234567890123456789" },
				// Include non-BCD test data to ensure no crashing.  Any return value is okay.
				{ { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa }, 10, "AAAAAAAAAAAAAAAAAAAA" },
				{ { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 10, "FFFFFFFFFFFFFFFFFFFF" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Copy test data to non-constant variable(s).
				byte *varBcdArray = new byte[TEST_DATA[testIndex].bcdLength];
				memcpy(varBcdArray, TEST_DATA[testIndex].bcdArray, TEST_DATA[testIndex].bcdLength);

				// Test.
				string returnValue1 = CUtilities::MakeCStringFromBCD(varBcdArray, (byte)TEST_DATA[testIndex].bcdLength);
				bool areEqual1 = returnValue1 == TEST_DATA[testIndex].returnValueExpected;
				string returnValue2 = CUtilities::MakeCStringFromBCD(varBcdArray, TEST_DATA[testIndex].bcdLength);
				bool areEqual2 = returnValue2 == TEST_DATA[testIndex].returnValueExpected;

				// Report.
				string bcdArrayStr;
                string formatStr;
				for (int bcdIndex = 0; bcdIndex < TEST_DATA[testIndex].bcdLength; ++bcdIndex)
				{
					if (!bcdArrayStr.empty())
					{
                        char appendchar =  ',';
						bcdArrayStr.append(&appendchar);
					}
                    int length = sprintf(const_cast<char*>(formatStr.c_str()), "0x%2.2x", TEST_DATA[testIndex].bcdArray[bcdIndex]);
					bcdArrayStr.append(formatStr.c_str(), length);
				}

				string failedMsg1;
				failedMsg1 = FormatString("FAILED CUtilities::MakestringFromBCD(&{%s}) returnValue=\"%s\" returnValueExpected=\"%s\" testIndex=%d-1)",
					(bcdArrayStr).c_str(),
					(returnValue1).c_str(),
					(TEST_DATA[testIndex].returnValueExpected).c_str(),
					testIndex);
				ASSERT_TRUE(areEqual1) <<  failedMsg1;

				string failedMsg2;
				failedMsg2 = FormatString("FAILED CUtilities::MakestringFromBCD(%s) returnValue=\"%s\" returnValueExpected=\"%s\" testIndex=%d-2)",
					(bcdArrayStr).c_str(),
					(returnValue2).c_str(),
					(TEST_DATA[testIndex].returnValueExpected).c_str(),
					testIndex);
				ASSERT_TRUE(areEqual2) <<  failedMsg2;

				delete [] varBcdArray;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CUtilities::ToHexString(const BYTE *data, DWORD, DWORD)
		/// Code coverage and functional test with data == NULL.
		/// </summary>
		TEST(UtilitiesTests, ToHexStringDwordDword_NullTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_data;
				const DWORD m_dataLength;

				// Expected results.
				const string m_returnValueExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ NULL, (DWORD)LONG_MIN, "" },
				{ NULL, (DWORD)INT_MIN, "" },
				{ NULL, (DWORD)-33, "" },
				{ NULL, (DWORD)-1, "" },
				{ NULL, 0, "" },
				{ NULL, 1, "" },
				{ NULL, 33, "" },
				{ NULL, (DWORD)INT_MAX, "" },
				{ NULL, (DWORD)LONG_MAX, "" },
				{ NULL, (DWORD)DWORD_MAX, "" },
			};

			// Test data - more inputs, same regardless of testIndex.
			const DWORD maximumDataLength(4096);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operations to be tested.
				const string returnValue = CUtilities::ToHexString(TEST_DATA[testIndex].m_data, TEST_DATA[testIndex].m_dataLength, maximumDataLength);

				// Test #1.
				{
					const bool areEqual = returnValue == TEST_DATA[testIndex].m_returnValueExpected;

					string failedMsg;
					failedMsg = FormatString("ToHexString(NULL,%u,%u) returnValue=\"%s\" returnValueExpected=\"%s\" testIndex=%d in %s)",
						(unsigned)TEST_DATA[testIndex].m_dataLength,
						(unsigned)maximumDataLength,
						(returnValue).c_str(),
						(TEST_DATA[testIndex].m_returnValueExpected).c_str(),
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CUtilities::ToHexString(const BYTE *data, DWORD, DWORD)
		/// Code coverage and functional test with data != NULL.
		/// </summary>
		TEST(UtilitiesTests, ToHexStringDwordDwordTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_data;
				const DWORD m_dataLength;
				const DWORD m_maximumDataLength;

				// Expected results.
				const string m_returnValueExpected;
			} TestDataType;

			const byte data0[1] = { 0xff };
			const byte data1[1] = { 0xab };
			const byte data2[8] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
			byte data3[16] =
			{
				0xb0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
			};
			byte data4[16*16] =
			{
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
			};
			byte data5[256+16] =
			{
				0xa0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			};

			const TestDataType TEST_DATA[] =
			{
				{ data0, 0, (DWORD)-1, "" },
				{ data0, 0, 0, "" },
				{ data0, 0, 1, "" },
				{ data0, 0, DWORD_MAX, "" },

				{ data1, _countof(data1), 0, "" },
				{ data1, _countof(data1), 1, "ab" },
				{ data1, _countof(data1), _countof(data1) - 1, "" },
				{ data1, _countof(data1), _countof(data1), "ab" },
				{ data1, _countof(data1), _countof(data1) + 1, "ab" },
				{ data1, _countof(data1), DWORD_MAX, "ab" },

				{ data2, _countof(data2), 0, "" },
				{ data2, _countof(data2), 1, "01" },
				{ data2, _countof(data2), 2, "01 23" },
				{ data2, _countof(data2), _countof(data2) - 1, "01 23 45 67 89 ab cd" },
				{ data2, _countof(data2), _countof(data2), "01 23 45 67 89 ab cd ef" },
				{ data2, _countof(data2), _countof(data2) + 1, "01 23 45 67 89 ab cd ef" },
				{ data2, _countof(data2), DWORD_MAX, "01 23 45 67 89 ab cd ef" },

				{ data3, _countof(data3),  0, "" 	},
				{ data3, _countof(data3),  1, "b0" },
				{ data3, _countof(data3),  2, "b0 01" },
				{ data3, _countof(data3),  _countof(data3) - 1, "b0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e" },
				{ data3, _countof(data3),  _countof(data3), "b0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f" },
				{ data3, _countof(data3),  _countof(data3) + 1, "b0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f" },
				{ data3, _countof(data3),  DWORD_MAX, "b0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f" },

				{ data4, _countof(data4),  0, "" },
				{ data4, _countof(data4),  1, "00" },
				{ data4, _countof(data4),  2, "00 01" },
				{ data4, _countof(data4), _countof(data4) - 1,
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe"
				},
				{ data4, _countof(data4), _countof(data4),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},
				{ data4, _countof(data4), _countof(data4) + 1,
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},
				{ data4, _countof(data4), DWORD_MAX,
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},

				{ data5, _countof(data5),  0, "" },
				{ data5, _countof(data5),  1, "a0" },
				{ data5, _countof(data5),  2, "a0 01" },
				{ data5, _countof(data5), 256,
					"a0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},
				{ data5, _countof(data5), _countof(data5) - 1,
					"a0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
					" 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e"
				},
				{ data5, _countof(data5), _countof(data5) - 1,
					"a0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
					" 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e"
				},
				{ data5, _countof(data5), _countof(data5),
					"a0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
					" 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
				},
				{ data5, _countof(data5), _countof(data5) + 1,
					"a0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
					" 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
				},
				{ data5, _countof(data5), DWORD_MAX,
					"a0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
					" 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
				},

			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operations to be tested.
				const string returnValue = CUtilities::ToHexString(TEST_DATA[testIndex].m_data,
					TEST_DATA[testIndex].m_dataLength,
					TEST_DATA[testIndex].m_maximumDataLength);

				// Test #1.
				{
					const bool areEqual = returnValue == TEST_DATA[testIndex].m_returnValueExpected;

					string dataStr;
                    string formatStr;
					for (DWORD dataIndex = 0; dataIndex < TEST_DATA[testIndex].m_dataLength; ++dataIndex)
					{
						if (!dataStr.empty())
						{
                            char appendchar = ',';
							dataStr.append(&appendchar);
						}
                        int length = sprintf(const_cast<char*>(formatStr.c_str()), "0x%2.2x", TEST_DATA[testIndex].m_data[dataIndex]);
                        dataStr.append(formatStr.c_str(), length);
					}

					// Emit testIndex first, as message could be truncated.
					string failedMsg;
					failedMsg = FormatString("testIndex=%d ToHexString(&{%s},%u,%u) returnValue=\"%s\" returnValueExpected=\"%s\" in %s(%d)",
						testIndex,
						(dataStr).c_str(),
						(unsigned)TEST_DATA[testIndex].m_dataLength,
						(unsigned)TEST_DATA[testIndex].m_maximumDataLength,
						(returnValue).c_str(),
						(TEST_DATA[testIndex].m_returnValueExpected).c_str(),
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CUtilities::ToHexString(const BYTE *data, DWORD)
		/// Code coverage and functional test with data == NULL.
		/// </summary>
		TEST(UtilitiesTests, ToHexStringDword_NullTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_data;
				const DWORD m_dataLength;

				// Expected results.
				const string m_returnValueExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ NULL, (DWORD)LONG_MIN, "" },
				{ NULL, (DWORD)INT_MIN, "" },
				{ NULL, (DWORD)-33, "" },
				{ NULL, (DWORD)-1, "" },
				{ NULL, 0, "" },
				{ NULL, 1, "" },
				{ NULL, 33, "" },
				{ NULL, (DWORD)INT_MAX, "" },
				{ NULL, (DWORD)LONG_MAX, "" },
				{ NULL, (DWORD)DWORD_MAX, "" },
				{ NULL, (DWORD)UINT_MAX, "" },
				{ NULL, (DWORD)ULONG_MAX, "" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operations to be tested.
				const string returnValue = CUtilities::ToHexString(TEST_DATA[testIndex].m_data, TEST_DATA[testIndex].m_dataLength);

				// Test #1.
				{
					const bool areEqual = returnValue == TEST_DATA[testIndex].m_returnValueExpected;

					string failedMsg;
					failedMsg = FormatString("ToHexString(NULL,%u) returnValue=\"%s\" returnValueExpected=\"%s\" testIndex=%d in %s)",
						(unsigned)TEST_DATA[testIndex].m_dataLength,
						(returnValue).c_str(),
						(TEST_DATA[testIndex].m_returnValueExpected).c_str(),
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CUtilities::ToHexString(const BYTE *data, DWORD)
		/// Code coverage and functional test with data != NULL.
		/// </summary>
		TEST(UtilitiesTests, ToHexStringDwordTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_data;
				const DWORD m_dataLength;

				// Expected results.
				const string m_returnValueExpected;
			} TestDataType;

			const byte data0[1] = { 0xff };
			const byte data1[1] = { 0xab };
			const byte data2[8] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
			byte data3[16] =
			{
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
			};
			byte data4[16*16] =
			{
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
			};
			byte data5[256+16] =
			{
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			};

			const TestDataType TEST_DATA[] =
			{
				{ data0, 0, "" },
				{ data1, _countof(data1), "ab" },
				{ data2, _countof(data2), "01 23 45 67 89 ab cd ef" },
				{ data3, _countof(data3),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
				},

				{ data4, _countof(data4) - 1,
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe"
				},
				{ data4, _countof(data4),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},

				{ data5, min(256 + 1, (int)_countof(data5)),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},
				{ data5, _countof(data5),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},

			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operations to be tested.
				const string returnValue = CUtilities::ToHexString(TEST_DATA[testIndex].m_data, TEST_DATA[testIndex].m_dataLength);

				// Test #1.
				{
					const bool areEqual = returnValue == TEST_DATA[testIndex].m_returnValueExpected;

					string dataStr;
                    string formatStr;
					for (DWORD dataIndex = 0; dataIndex < TEST_DATA[testIndex].m_dataLength; ++dataIndex)
					{
						if (!dataStr.empty())
						{
                            char appendchar = ',';
							dataStr.append(&appendchar);
						}
                        int length = sprintf(const_cast<char*>(formatStr.c_str()), "0x%2.2x", TEST_DATA[testIndex].m_data[dataIndex]);
                        dataStr.append(formatStr.c_str(), length);
					}

					// Emit testIndex first, as message could be truncated.
					string failedMsg;
					failedMsg = FormatString("testIndex=%d ToHexString(&{%s},%u) returnValue=\"%s\" returnValueExpected=\"%s\" in %s(%d)",
						testIndex,
						(dataStr).c_str(),
						(unsigned)TEST_DATA[testIndex].m_dataLength,
						(returnValue).c_str(),
						(TEST_DATA[testIndex].m_returnValueExpected).c_str(),
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CUtilities::ToHexString(const BYTE *data, size_t)
		/// Code coverage and functional test with data == NULL.
		/// </summary>
		TEST(UtilitiesTests, ToHexStringSizet_NullTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_data;
				const size_t m_dataLength;

				// Expected results.
				const string m_returnValueExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ NULL, (size_t)LONG_MIN, "" },
				{ NULL, (size_t)INT_MIN, "" },
				{ NULL, (size_t)-33, "" },
				{ NULL, (size_t)-1, "" },
				{ NULL, 0, "" },
				{ NULL, 1, "" },
				{ NULL, 33, "" },
				{ NULL, (size_t)INT_MAX, "" },
				{ NULL, (size_t)LONG_MAX, "" },
				{ NULL, (size_t)UINT_MAX, "" },
				{ NULL, (size_t)ULONG_MAX, "" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operations to be tested.
				const string returnValue = CUtilities::ToHexString(TEST_DATA[testIndex].m_data, TEST_DATA[testIndex].m_dataLength);

				// Test #1.
				{
					const bool areEqual = returnValue == TEST_DATA[testIndex].m_returnValueExpected;

					string failedMsg;
					failedMsg = FormatString("ToHexString(NULL,%lu) returnValue=\"%s\" returnValueExpected=\"%s\" testIndex=%d in %s)",
						(unsigned long)TEST_DATA[testIndex].m_dataLength,
						(returnValue).c_str(),
						(TEST_DATA[testIndex].m_returnValueExpected).c_str(),
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CUtilities::ToHexString(const BYTE *data, size_t)
		/// Code coverage and functional test with data != NULL.
		/// </summary>
		TEST(UtilitiesTests, ToHexStringSizetTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_data;
				const size_t m_dataLength;

				// Expected results.
				const string m_returnValueExpected;
			} TestDataType;

			const byte data0[1] = { 0xff };
			const byte data1[1] = { 0xab };
			const byte data2[8] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
			byte data3[16] =
			{
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
			};
			byte data4[16*16] =
			{
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
			};
			byte data5[256+16] =
			{
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			};

			const TestDataType TEST_DATA[] =
			{
				{ data0, 0, "" },
				{ data1, _countof(data1), "ab" },
				{ data2, _countof(data2), "01 23 45 67 89 ab cd ef" },
				{ data3, _countof(data3),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
				},

				{ data4, _countof(data4) - 1,
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe"
				},
				{ data4, _countof(data4),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},

				{ data5, min(256 + 1, (int)_countof(data5)),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},
				{ data5, _countof(data5),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},

			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operations to be tested.
				const string returnValue = CUtilities::ToHexString(TEST_DATA[testIndex].m_data, TEST_DATA[testIndex].m_dataLength);

				// Test #1.
				{
					const bool areEqual = returnValue == TEST_DATA[testIndex].m_returnValueExpected;

					string dataStr;
                    string formatStr;
					for (size_t dataIndex = 0; dataIndex < TEST_DATA[testIndex].m_dataLength; ++dataIndex)
					{
						if (!dataStr.empty())
						{
                            char appendchar = ',';
							dataStr.append(&appendchar);
						}
                        int length = sprintf(const_cast<char*>(formatStr.c_str()), "0x%2.2x", TEST_DATA[testIndex].m_data[dataIndex]);
                        dataStr.append(formatStr.c_str(), length);
					}

					// Emit testIndex first, as message could be truncated.
					string failedMsg;
					failedMsg = FormatString("testIndex=%d ToHexString(&{%s},%lu) returnValue=\"%s\" returnValueExpected=\"%s\" in %s(%d)",
						testIndex,
						(dataStr).c_str(),
						(unsigned long)TEST_DATA[testIndex].m_dataLength,
						(returnValue).c_str(),
						(TEST_DATA[testIndex].m_returnValueExpected).c_str(),
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CUtilities::ToHexString(const BYTE *data, long, DWORD)
		/// Code coverage and functional test with data == NULL.
		/// </summary>
		TEST(UtilitiesTests, ToHexStringLongDword_NullTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_data;
				const long m_dataLength;

				// Expected results.
				const string m_returnValueExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ NULL, LONG_MIN, "" },
				{ NULL, INT_MIN, "" },
				{ NULL, -33, "" },
				{ NULL, -1, "" },
				{ NULL, 0, "" },
				{ NULL, 1, "" },
				{ NULL, 33, "" },
				{ NULL, INT_MAX, "" },
				{ NULL, LONG_MAX, "" },
				{ NULL, (long)UINT_MAX, "" },
				{ NULL, (long)ULONG_MAX, "" },
			};

			// Test data - more inputs, same regardless of testIndex.
			const DWORD maximumDataLength(4096);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operations to be tested #1.
				const string returnValue = CUtilities::ToHexString(TEST_DATA[testIndex].m_data, (DWORD)TEST_DATA[testIndex].m_dataLength, maximumDataLength);

				// Test #1.
				{
					const bool areEqual = returnValue == TEST_DATA[testIndex].m_returnValueExpected;

					string failedMsg;
					failedMsg = FormatString("ToHexString(NULL,%u,%u) returnValue=\"%s\" returnValueExpected=\"%s\" testIndex=%d in %s)",
						(unsigned)TEST_DATA[testIndex].m_dataLength,
						(unsigned)maximumDataLength,
						(returnValue).c_str(),
						(TEST_DATA[testIndex].m_returnValueExpected).c_str(),
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CUtilities::ToHexString(const BYTE *data, long, DWORD)
		/// Code coverage and functional test with data != NULL.
		/// </summary>
		TEST(UtilitiesTests, ToHexStringLongDwordTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_data;
				const long m_dataLength;
				const DWORD m_maximumDataLength;

				// Expected results.
				const string m_returnValueExpected;
			} TestDataType;

			const byte data0[1] = { 0xff };
			const byte data1[1] = { 0xab };
			const byte data2[8] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
			byte data3[16] =
			{
				0xb0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
			};
			byte data4[16*16] =
			{
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
			};
			byte data5[256+16] =
			{
				0xa0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			};

			const TestDataType TEST_DATA[] =
			{
				{ data0, 0, (DWORD)LONG_MIN, "" },
				{ data0, 0, (DWORD)-1, "" },
				{ data0, 0, 0, "" },
				{ data0, 0, 1, "" },
				{ data0, 0, (DWORD)LONG_MAX, "" },

				{ data1, 0, (DWORD)LONG_MIN, "" },
				{ data1, 0, (DWORD)-1, "" },
				{ data1, _countof(data1), 0, "" },
				{ data1, _countof(data1), 1, "ab" },
				{ data1, _countof(data1), _countof(data1) - 1, "" },
				{ data1, _countof(data1), _countof(data1), "ab" },
				{ data1, _countof(data1), _countof(data1) + 1, "ab" },
				{ data1, _countof(data1), (DWORD)LONG_MAX, "ab" },

				{ data2, 0, (DWORD)LONG_MIN, "" },
				{ data2, 0, (DWORD)-1, "" },
				{ data2, _countof(data2), 0, "" },
				{ data2, _countof(data2), 1, "01" },
				{ data2, _countof(data2), 2, "01 23" },
				{ data2, _countof(data2), _countof(data2) - 1, "01 23 45 67 89 ab cd" },
				{ data2, _countof(data2), _countof(data2), "01 23 45 67 89 ab cd ef" },
				{ data2, _countof(data2), _countof(data2) + 1, "01 23 45 67 89 ab cd ef" },
				{ data2, _countof(data2), (DWORD)LONG_MAX, "01 23 45 67 89 ab cd ef" },

				{ data3, 0, (DWORD)LONG_MIN, "" },
				{ data3, 0, (DWORD)-1, "" },
				{ data3, _countof(data3),  0, "" 	},
				{ data3, _countof(data3),  1, "b0" },
				{ data3, _countof(data3),  2, "b0 01" },
				{ data3, _countof(data3),  _countof(data3) - 1, "b0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e" },
				{ data3, _countof(data3),  _countof(data3), "b0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f" },
				{ data3, _countof(data3),  _countof(data3) + 1, "b0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f" },
				{ data3, _countof(data3),  (DWORD)LONG_MAX, "b0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f" },

				{ data4, 0, (DWORD)LONG_MIN, "" },
				{ data4, 0, (DWORD)-1, "" },
				{ data4, _countof(data4),  0, "" },
				{ data4, _countof(data4),  1, "00" },
				{ data4, _countof(data4),  2, "00 01" },
				{ data4, _countof(data4), _countof(data4) - 1,
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe"
				},
				{ data4, _countof(data4), _countof(data4),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},
				{ data4, _countof(data4), _countof(data4) + 1,
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},
				{ data4, _countof(data4), DWORD_MAX,
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},

				{ data5, 0, (DWORD)LONG_MIN, "" },
				{ data5, 0, (DWORD)-1, "" },
				{ data5, _countof(data5),  0, "" },
				{ data5, _countof(data5),  1, "a0" },
				{ data5, _countof(data5),  2, "a0 01" },
				{ data5, _countof(data5), 256,
					"a0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},
				{ data5, _countof(data5), _countof(data5) - 1,
					"a0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
					" 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e"
				},
				{ data5, _countof(data5), _countof(data5) - 1,
					"a0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
					" 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e"
				},
				{ data5, _countof(data5), _countof(data5),
					"a0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
					" 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
				},
				{ data5, _countof(data5), _countof(data5) + 1,
					"a0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
					" 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
				},
				{ data5, _countof(data5), (DWORD)LONG_MAX,
					"a0 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
					" 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
				},

			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operations to be tested.
				const string returnValue = CUtilities::ToHexString(TEST_DATA[testIndex].m_data,
					(DWORD)TEST_DATA[testIndex].m_dataLength,
					TEST_DATA[testIndex].m_maximumDataLength);

				// Test #1.
				{
					const bool areEqual = returnValue == TEST_DATA[testIndex].m_returnValueExpected;

					// Output string for failures, if needed.
					string dataStr;
                    string formatStr;
					for (long dataIndex = 0; dataIndex < TEST_DATA[testIndex].m_dataLength; ++dataIndex)
					{
						if (!dataStr.empty())
						{
                            char appendchar = ',';
							dataStr.append(&appendchar);
						}
                        int length = sprintf(const_cast<char*>(formatStr.c_str()), "0x%2.2x", TEST_DATA[testIndex].m_data[dataIndex]);
                        dataStr.append(formatStr.c_str(), length);
					}

					// Emit testIndex first, as message could be truncated.
					string failedMsg;
					failedMsg = FormatString("testIndex=%d ToHexString(&{%s},%ld,%u) returnValue=\"%s\" returnValueExpected=\"%s\" in %s(%d)",
						testIndex,
						(dataStr).c_str(),
						(long)TEST_DATA[testIndex].m_dataLength,
						(unsigned)TEST_DATA[testIndex].m_maximumDataLength,
						(returnValue).c_str(),
						(TEST_DATA[testIndex].m_returnValueExpected).c_str(),
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CUtilities::ToHexString(const BYTE *data, long)
		/// Code coverage and functional test with data == NULL.
		/// </summary>
		TEST(UtilitiesTests, ToHexStringLong_NullTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_data;
				const long m_dataLength;

				// Expected results.
				const string m_returnValueExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ NULL, (long)LONG_MIN, "" },
				{ NULL, -33, "" },
				{ NULL, -1, "" },
				{ NULL, 0, "" },
				{ NULL, 1, "" },
				{ NULL, 33, "" },
				{ NULL, (long)LONG_MAX, "" },
				{ NULL, (long)ULONG_MAX, "" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operations to be tested.
				const string returnValue = CUtilities::ToHexString(TEST_DATA[testIndex].m_data, TEST_DATA[testIndex].m_dataLength);

				// Test #1.
				{
					const bool areEqual = returnValue == TEST_DATA[testIndex].m_returnValueExpected;

					string failedMsg;
					failedMsg = FormatString("ToHexString(NULL,%ld) returnValue=\"%s\" returnValueExpected=\"%s\" testIndex=%d in %s)",
						(long)TEST_DATA[testIndex].m_dataLength,
						(returnValue).c_str(),
						(TEST_DATA[testIndex].m_returnValueExpected).c_str(),
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CUtilities::ToHexString(const BYTE *data, long)
		/// Code coverage and functional test with data != NULL.
		/// </summary>
		TEST(UtilitiesTests, ToHexStringLongTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_data;
				const long m_dataLength;

				// Expected results.
				const string m_returnValueExpected;
			} TestDataType;

			const byte data0[1] = { 0xff };
			const byte data1[1] = { 0xab };
			const byte data2[8] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
			byte data3[16] =
			{
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
			};
			byte data4[16*16] =
			{
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
			};
			byte data5[256+16] =
			{
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			};

			const TestDataType TEST_DATA[] =
			{
				{ data0, 0, "" },
				{ data1, _countof(data1), "ab" },
				{ data2, _countof(data2), "01 23 45 67 89 ab cd ef" },
				{ data3, _countof(data3),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
				},

				{ data4, _countof(data4) - 1,
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe"
				},
				{ data4, _countof(data4),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},

				{ data5, min(256 + 1, (int)_countof(data5)),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},
				{ data5, _countof(data5),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},

			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operations to be tested.
				const string returnValue = CUtilities::ToHexString(TEST_DATA[testIndex].m_data, TEST_DATA[testIndex].m_dataLength);

				// Test #1.
				{
					const bool areEqual = returnValue == TEST_DATA[testIndex].m_returnValueExpected;

					string dataStr;
                    string formatStr;
					for (long dataIndex = 0; dataIndex < TEST_DATA[testIndex].m_dataLength; ++dataIndex)
					{
						if (!dataStr.empty())
						{
                            char appendchar = ',';
							dataStr.append(&appendchar);
						}
                        int length = sprintf(const_cast<char*>(formatStr.c_str()), "0x%2.2x", TEST_DATA[testIndex].m_data[dataIndex]);
                        dataStr.append(formatStr.c_str(), length);
					}

					// Emit testIndex first, as message could be truncated.
					string failedMsg;
					failedMsg = FormatString("testIndex=%d ToHexString(&{%s},%ld) returnValue=\"%s\" returnValueExpected=\"%s\" in %s(%d)",
						testIndex,
						(dataStr).c_str(),
						(long)TEST_DATA[testIndex].m_dataLength,
						(returnValue).c_str(),
						(TEST_DATA[testIndex].m_returnValueExpected).c_str(),
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CUtilities::ToHexString(const BYTE *data, int)
		/// Code coverage and functional test with data == NULL.
		/// </summary>
		TEST(UtilitiesTests, ToHexStringInt_NullTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_data;
				const int m_dataLength;

				// Expected results.
				const string m_returnValueExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ NULL, INT_MIN, "" },
				{ NULL, -33, "" },
				{ NULL, -1, "" },
				{ NULL, 0, "" },
				{ NULL, 1, "" },
				{ NULL, 33, "" },
				{ NULL, INT_MAX, "" },
				{ NULL, (int)UINT_MAX, "" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operations to be tested.
				const string returnValue = CUtilities::ToHexString(TEST_DATA[testIndex].m_data, TEST_DATA[testIndex].m_dataLength);

				// Test #1.
				{
					const bool areEqual = returnValue == TEST_DATA[testIndex].m_returnValueExpected;

					string failedMsg;
					failedMsg = FormatString("ToHexString(NULL,%d) returnValue=\"%s\" returnValueExpected=\"%s\" testIndex=%d in %s)",
						TEST_DATA[testIndex].m_dataLength,
						(returnValue).c_str(),
						(TEST_DATA[testIndex].m_returnValueExpected).c_str(),
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CUtilities::ToHexString(const BYTE *data, int)
		/// Code coverage and functional test with data != NULL.
		/// </summary>
		TEST(UtilitiesTests, ToHexStringIntTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_data;
				const int m_dataLength;

				// Expected results.
				const string m_returnValueExpected;
			} TestDataType;

			const byte data0[1] = { 0xff };
			const byte data1[1] = { 0xab };
			const byte data2[8] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
			byte data3[16] =
			{
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
			};
			byte data4[16*16] =
			{
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
			};
			byte data5[256+16] =
			{
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			};

			const TestDataType TEST_DATA[] =
			{
				{ data0, 0, "" },
				{ data1, _countof(data1), "ab" },
				{ data2, _countof(data2), "01 23 45 67 89 ab cd ef" },
				{ data3, _countof(data3),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
				},

				{ data4, _countof(data4) - 1,
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe"
				},
				{ data4, _countof(data4),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},

				{ data5, min(256 + 1, (int)_countof(data5)),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},
				{ data5, _countof(data5),
					"00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"
					" 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f"
					" 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f"
					" 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f"
					" 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f"
					" 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f"
					" 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f"
					" 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f"
					" 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f"
					" 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f"
					" a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af"
					" b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf"
					" c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf"
					" d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df"
					" e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef"
					" f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff"
				},

			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operations to be tested.
				const string returnValue = CUtilities::ToHexString(TEST_DATA[testIndex].m_data, TEST_DATA[testIndex].m_dataLength);

				// Test #1.
				{
					const bool areEqual = returnValue == TEST_DATA[testIndex].m_returnValueExpected;

					string dataStr;
                    string formatStr;
					for (long dataIndex = 0; dataIndex < TEST_DATA[testIndex].m_dataLength; ++dataIndex)
					{
						if (!dataStr.empty())
						{
                            char appendchar = ',';
							dataStr.append(&appendchar);
						}
                        int length = sprintf(const_cast<char*>(formatStr.c_str()), "0x%2.2x", TEST_DATA[testIndex].m_data[dataIndex]);
                        dataStr.append(formatStr.c_str(), length);
					}

					// Emit testIndex first, as message could be truncated.
					string failedMsg;
					failedMsg = FormatString("testIndex=%d ToHexString(&{%s},%d) returnValue=\"%s\" returnValueExpected=\"%s\" in %s(%d)",
						testIndex,
						(dataStr).c_str(),
						TEST_DATA[testIndex].m_dataLength,
						(returnValue).c_str(),
						(TEST_DATA[testIndex].m_returnValueExpected).c_str(),
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Functional coverage test for:
		///     CPlayer::GetFormattedNumberString(DWORD)
		/// Test edge conditions.
		/// </summary>
		TEST(UtilitiesTests, GetFormattedNumberStringTest)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_val;

				// Expected results.
				const string m_formattedNumberStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, "0" },
				{ 1, "1" },
				{ 9, "9" },
				{ 10, "10" },
				{ 99, "99" },
				{ 100, "100" },
				{ 999, "999" },
				{ 1000, "1,000" },
				{ 9999, "9,999" },
				{ 10000, "10,000" },
				{ 99999, "99,999" },
				{ 100000, "100,000" },
				{ 999999, "999,999" },
				{ 1000000, "1,000,000" },
				{ 9999999, "9,999,999" },
				{ 10000000, "10,000,000" },
				{ 99999999, "99,999,999" },
				{ 100000000, "100,000,000" },
				{ 999999999, "999,999,999" },
				{ 1000000000, "1,000,000,000" },
				{ (DWORD)INT_MAX - 1, "2,147,483,646" },
				{ (DWORD)INT_MAX, "2,147,483,647" },
				{ (DWORD)INT_MIN, "-2,147,483,648" },
				{ (DWORD)INT_MIN + 1, "-2,147,483,647" },
				{ DWORD_MAX - 1, "-2" },
				{ DWORD_MAX, "-1" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				string formattedNumberStringResult(CUtilities::GetFormattedNumberString(TEST_DATA[testIndex].m_val));

				// Test #1.
				{
					bool areEqual(formattedNumberStringResult == TEST_DATA[testIndex].m_formattedNumberStringExpected);

					string failedMsg;
					failedMsg = FormatString("FAILED formattedNumberStringResult=\"%s\" expected=\"%s\" testIndex=%d in %s)",
					(formattedNumberStringResult).c_str(),
					(TEST_DATA[testIndex].m_formattedNumberStringExpected).c_str(),
					testIndex,
					(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Functional coverage test for:
		///     CPlayer::GetFormattedNumberString(DWORD, LPCTSTR=NULL)
		/// Test edge conditions.
		/// </summary>
		TEST(UtilitiesTests, GetFormattedNumberString_NullTest)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_val;

				// Expected results.
				const string m_formattedNumberStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, "0" },
				{ 1, "1" },
				{ 9, "9" },
				{ 10, "10" },
				{ 99, "99" },
				{ 100, "100" },
				{ 999, "999" },
				{ 1000, "1,000" },
				{ 9999, "9,999" },
				{ 10000, "10,000" },
				{ 99999, "99,999" },
				{ 100000, "100,000" },
				{ 999999, "999,999" },
				{ 1000000, "1,000,000" },
				{ 9999999, "9,999,999" },
				{ 10000000, "10,000,000" },
				{ 99999999, "99,999,999" },
				{ 100000000, "100,000,000" },
				{ 999999999, "999,999,999" },
				{ 1000000000, "1,000,000,000" },
				{ (DWORD)INT_MAX - 1, "2,147,483,646" },
				{ (DWORD)INT_MAX, "2,147,483,647" },
				{ (DWORD)INT_MIN, "-2,147,483,648" },
				{ (DWORD)INT_MIN + 1, "-2,147,483,647" },
				{ DWORD_MAX - 1, "-2" },
				{ DWORD_MAX, "-1" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				string formattedNumberStringResult(CUtilities::GetFormattedNumberString(TEST_DATA[testIndex].m_val));

				// Test #1.
				{
					bool areEqual(formattedNumberStringResult == TEST_DATA[testIndex].m_formattedNumberStringExpected);

					string failedMsg;
					failedMsg = FormatString("FAILED formattedNumberStringResult=\"%s\" expected=\"%s\" testIndex=%d in %s)",
					(formattedNumberStringResult).c_str(),
					(TEST_DATA[testIndex].m_formattedNumberStringExpected).c_str(),
					testIndex,
					(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Functional coverage test for:
		///     CPlayer::GetFormattedNumberString(DWORD, LPCTSTR)
		/// Test edge conditions for the DWORD val.
		/// </summary>
		TEST(UtilitiesTests, GetFormattedNumberString_TextTest)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_val;
				const LPCTSTR m_text;

				// Expected results.
				const string m_formattedNumberStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, "", "0" },
				{ 1, "a", "1 a" },
				{ 9, "ab", "9 ab" },
				{ 10, "abc", "10 abc" },
				{ 99, " $~", "99  $~" },
				{ 100, "abcdefghijklmnopqrstuvwxyz", "100 abcdefghijklmnopqrstuvwxyz" },
				{ 999, "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ", "999 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ" },
				{ 1000, " `1234567890-=[]\\;',./ ~!@#$%^&*()_+{}|:\"<>? ", "1,000  `1234567890-=[]\\;',./ ~!@#$%^&*()_+{}|:\"<>? " },
				{ 9999, "abc", "9,999 abc" },
				{ 10000, "abc", "10,000 abc" },
				{ 99999, "abc", "99,999 abc" },
				{ 100000, "abc", "100,000 abc" },
				{ 999999, "abc", "999,999 abc" },
				{ 1000000, "abc", "1,000,000 abc" },
				{ 9999999, "abc", "9,999,999 abc" },
				{ 10000000, "abc", "10,000,000 abc" },
				{ 99999999, "abc", "99,999,999 abc" },
				{ 100000000, "abc", "100,000,000 abc" },
				{ 999999999, "abc", "999,999,999 abc" },
				{ 1000000000, "abc", "1,000,000,000 abc" },
				{ (DWORD)INT_MAX - 1, "abc", "2,147,483,646 abc" },
				{ (DWORD)INT_MAX, "abc", "2,147,483,647 abc" },
				{ (DWORD)INT_MIN + 1, "abc", "-2,147,483,647 abc" },
				{ DWORD_MAX - 1, "abc", "-2 abc" },
				{ DWORD_MAX, "abc", "-1 abc" },

				{ (DWORD)INT_MIN,
					"abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 `1234567890-=[]\\;',./ ~!@#$%^&*()_+{}|:\"<>? abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 `1234567890-=[]\\;',./ ~!@#$%^&*()_+{}|:\"<>? ",
					"-2,147,483,648 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 `1234567890-=[]\\;',./ ~!@#$%^&*()_+{}|:\"<>? abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 `1234567890-=[]\\;',./ ~!@#$%^&*()_+{}|:\"<>? " },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				string formattedNumberStringResult(CUtilities::GetFormattedNumberString(TEST_DATA[testIndex].m_val, TEST_DATA[testIndex].m_text));

				// Test #1.
				{
					bool areEqual(formattedNumberStringResult == TEST_DATA[testIndex].m_formattedNumberStringExpected);

					string failedMsg;
					failedMsg = FormatString("FAILED formattedNumberStringResult=\"%s\" expected=\"%s\" testIndex=%d in %s)",
					(formattedNumberStringResult).c_str(),
					(TEST_DATA[testIndex].m_formattedNumberStringExpected).c_str(),
					testIndex,
					(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Functional coverage test for:
		///     CPlayer::GetFormattedNumberString(DWORD, UINT)
		/// Test edge conditions.
		/// </summary>
		TEST(UtilitiesTests, GetFormattedNumberString_IdsTest)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_val;
				const LPCSTR m_textIds;

				// Expected results.
				const string m_formattedNumberStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, IDS_POINTS, "0 points" },
				{ 1, IDS_POINTS_TODAY, "1 points today" },
				{ 9, IDS_AVAILABLE, "9 available" },
				{ 10, IDS_COMP, "10 comp" },
				{ 99, IDS_POINTS_TO_NEXT_TIER, "99 points to next tier" },
				{ 100, IDS_POINTS, "100 points" },
				{ 999, IDS_POINTS, "999 points" },
				{ 1000, IDS_POINTS_TODAY, "1,000 points today" },
				{ 9999, IDS_AVAILABLE, "9,999 available" },
				{ 10000, IDS_COMP, "10,000 comp" },
				{ 99999, IDS_POINTS_TO_NEXT_TIER, "99,999 points to next tier" },
				{ 100000, IDS_POINTS, "100,000 points" },
				{ 999999, IDS_POINTS, "999,999 points" },
				{ 1000000, IDS_POINTS_TODAY, "1,000,000 points today" },
				{ 9999999, IDS_AVAILABLE, "9,999,999 available" },
				{ 10000000, IDS_COMP, "10,000,000 comp" },
				{ 99999999, IDS_POINTS_TO_NEXT_TIER, "99,999,999 points to next tier" },
				{ 100000000, IDS_POINTS, "100,000,000 points" },
				{ 999999999, IDS_POINTS, "999,999,999 points" },
				{ 1000000000, IDS_POINTS_TODAY, "1,000,000,000 points today" },
				{ (DWORD)INT_MAX - 1, IDS_AVAILABLE, "2,147,483,646 available" },
				{ (DWORD)INT_MAX, IDS_COMP, "2,147,483,647 comp" },
				{ (DWORD)INT_MIN, IDS_POINTS_TO_NEXT_TIER, "-2,147,483,648 points to next tier" },
				{ (DWORD)INT_MIN + 1, IDS_POINTS, "-2,147,483,647 points" },
				{ DWORD_MAX - 1, IDS_POINTS, "-2 points" },
				{ DWORD_MAX, IDS_POINTS_TODAY, "-1 points today" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				string formattedNumberStringResult(CUtilities::GetFormattedNumberString(TEST_DATA[testIndex].m_val, TEST_DATA[testIndex].m_textIds));

				// Test #1.
				{
					bool areEqual(formattedNumberStringResult == TEST_DATA[testIndex].m_formattedNumberStringExpected);

					string failedMsg;
					failedMsg = FormatString("FAILED formattedNumberStringResult=\"%s\" expected=\"%s\" testIndex=%d in %s)",
					(formattedNumberStringResult).c_str(),
					(TEST_DATA[testIndex].m_formattedNumberStringExpected).c_str(),
					testIndex,
					(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Functional coverage test for:
		///     CPlayer::GetFormattedDollarsPenniesString()
		/// Test edge conditions using a single cashValue to derive the dollars and pennies values.
		/// </summary>
		TEST(UtilitiesTests, GetFormattedDollarsPenniesString1Test)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_cashValue;

				// Expected results.
				const string m_cashStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, "$0.00" },
				{ 1, "$0.01" },
				{ 9, "$0.09" },
				{ 10, "$0.10" },
				{ 99, "$0.99" },
				{ 100, "$1.00" },
				{ 999, "$9.99" },
				{ 1000, "$10.00" },
				{ 9999, "$99.99" },
				{ 10000, "$100.00" },
				{ 99999, "$999.99" },
				{ 100000, "$1,000.00" },
				{ 999999, "$9,999.99" },
				{ 1000000, "$10,000.00" },
				{ 9999999, "$99,999.99" },
				{ 10000000, "$100,000.00" },
				{ 99999999, "$999,999.99" },
				{ 100000000, "$1,000,000.00" },
				{ 999999999, "$9,999,999.99" },
				{ 1000000000, "$10,000,000.00" },
				{ (DWORD)INT_MAX - 1, "$21,474,836.46" },
				{ (DWORD)INT_MAX, "$21,474,836.47" },
				{ (DWORD)INT_MIN, "$21,474,836.48" },
				{ (DWORD)INT_MIN + 1, "$21,474,836.49" },
				{ DWORD_MAX - 1, "$42,949,672.94" },
				{ DWORD_MAX, "$42,949,672.95" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				string cashStringResult = CUtilities::GetFormattedDollarsPenniesString(TEST_DATA[testIndex].m_cashValue / 100,
					TEST_DATA[testIndex].m_cashValue % 100);

				// Test #1.
				{
					bool areEqual = cashStringResult == TEST_DATA[testIndex].m_cashStringExpected;

					string failedMsg;
					failedMsg = FormatString("FAILED cashStringResult=\"%s\" expected=\"%s\" testIndex=%d in %s)",
					(cashStringResult).c_str(),
					(TEST_DATA[testIndex].m_cashStringExpected).c_str(),
					testIndex,
					(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Functional coverage test for:
		///     CPlayer::GetFormattedDollarsPenniesString()
		/// Test edge conditions using separate DWORD values for dollars and pennies.
		/// </summary>
		TEST(UtilitiesTests, GetFormattedDollarsPenniesString2Test)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_dollars;
				const DWORD m_pennies;

				// Expected results.
				const string m_cashStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, 0, "$0.00" },
				{ 0, 1, "$0.01" },
				{ 0, 9, "$0.09" },
				{ 0, 10, "$0.10" },
				{ 0, 99, "$0.99" },
				{ 0, 100, "$0.100" },
				{ 0, (DWORD)INT_MAX, "$0.2147483647" },
				{ 0, (DWORD)INT_MIN, "$0.-2147483648" },
				{ 0, DWORD_MAX, "$0.-1" },

				{ 1, 99, "$1.99" },
				{ 9, 98, "$9.98" },
				{ 10, 97, "$10.97" },
				{ 99, 96, "$99.96" },
				{ 100, 95, "$100.95" },
				{ 999, 94, "$999.94" },
				{ 1000, 93, "$1,000.93" },
				{ 9999, 92, "$9,999.92" },
				{ 10000, 91, "$10,000.91" },
				{ 99999, 90, "$99,999.90" },
				{ 100000, 89, "$100,000.89" },
				{ 999999, 88, "$999,999.88" },
				{ 1000000, 11, "$1,000,000.11" },
				{ 9999999, 10, "$9,999,999.10" },
				{ 10000000, 9, "$10,000,000.09" },
				{ 99999999, 8, "$99,999,999.08" },
				{ 100000000, 7, "$100,000,000.07" },
				{ 999999999, 6, "$999,999,999.06" },
				{ 1000000000, 5, "$1,000,000,000.05" },
				{ (DWORD)INT_MAX - 1, 4, "$2,147,483,646.04" },
				{ (DWORD)INT_MAX, 3, "$2,147,483,647.03" },
				{ (DWORD)INT_MIN, 2, "$-2,147,483,648.02" },
				{ (DWORD)INT_MIN + 1, 1, "$-2,147,483,647.01" },
				{ DWORD_MAX - 1, 0, "$-2.00" },

				{ DWORD_MAX, 0, "$-1.00" },
				{ DWORD_MAX, 1, "$-1.01" },
				{ DWORD_MAX, 9, "$-1.09" },
				{ DWORD_MAX, 10, "$-1.10" },
				{ DWORD_MAX, 99, "$-1.99" },
				{ DWORD_MAX, 100, "$-1.100" },
				{ DWORD_MAX, (DWORD)INT_MAX, "$-1.2147483647" },
				{ DWORD_MAX, (DWORD)INT_MIN, "$-1.-2147483648" },
				{ DWORD_MAX, DWORD_MAX, "$-1.-1" },

				// Test longer result strings.
				{ (DWORD)INT_MIN, 0, "$-2,147,483,648.00" },
				{ (DWORD)INT_MIN, 1, "$-2,147,483,648.01" },
				{ (DWORD)INT_MIN, 9, "$-2,147,483,648.09" },
				{ (DWORD)INT_MIN, 10, "$-2,147,483,648.10" },
				{ (DWORD)INT_MIN, 99, "$-2,147,483,648.99" },
				{ (DWORD)INT_MIN, 100, "$-2,147,483,648.100" },
				{ (DWORD)INT_MIN, (DWORD)INT_MAX, "$-2,147,483,648.2147483647" },
				{ (DWORD)INT_MIN, (DWORD)INT_MIN, "$-2,147,483,648.-2147483648" },
				{ (DWORD)INT_MIN, DWORD_MAX, "$-2,147,483,648.-1" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				string cashStringResult = CUtilities::GetFormattedDollarsPenniesString(TEST_DATA[testIndex].m_dollars,
					TEST_DATA[testIndex].m_pennies);

				// Test #1.
				{
					bool areEqual = cashStringResult == TEST_DATA[testIndex].m_cashStringExpected;

					string failedMsg;
					failedMsg = FormatString("FAILED cashStringResult=\"%s\" expected=\"%s\" testIndex=%d in %s)",
					(cashStringResult).c_str(),
					(TEST_DATA[testIndex].m_cashStringExpected).c_str(),
					testIndex,
					(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Functional coverage test for:
		///     CPlayer::GetFormattedDollarsPenniesString()
		/// Test edge conditions using the default value for the pennies parameter.
		/// </summary>
		TEST(UtilitiesTests, GetFormattedDollarsPenniesString3Test)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_dollars;

				// Expected results.
				const string m_cashStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, "$0.00" },
				{ 1, "$1.00" },
				{ 9, "$9.00" },
				{ 10, "$10.00" },
				{ 99, "$99.00" },
				{ 100, "$100.00" },
				{ 999, "$999.00" },
				{ 1000, "$1,000.00" },
				{ 9999, "$9,999.00" },
				{ 10000, "$10,000.00" },
				{ 99999, "$99,999.00" },
				{ 100000, "$100,000.00" },
				{ 999999, "$999,999.00" },
				{ 1000000, "$1,000,000.00" },
				{ 9999999, "$9,999,999.00" },
				{ 10000000, "$10,000,000.00" },
				{ 99999999, "$99,999,999.00" },
				{ 100000000, "$100,000,000.00" },
				{ 999999999, "$999,999,999.00" },
				{ 1000000000, "$1,000,000,000.00" },
				{ (DWORD)INT_MAX - 1, "$2,147,483,646.00" },
				{ (DWORD)INT_MAX, "$2,147,483,647.00" },
				{ (DWORD)INT_MIN, "$-2,147,483,648.00" },
				{ (DWORD)INT_MIN + 1, "$-2,147,483,647.00" },
				{ DWORD_MAX - 1, "$-2.00" },
				{ DWORD_MAX, "$-1.00" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				string cashStringResult = CUtilities::GetFormattedDollarsPenniesString(TEST_DATA[testIndex].m_dollars);

				// Test #1.
				{
					bool areEqual = cashStringResult == TEST_DATA[testIndex].m_cashStringExpected;

					string failedMsg;
					failedMsg = FormatString("FAILED cashStringResult=\"%s\" expected=\"%s\" testIndex=%d in %s)",
					(cashStringResult).c_str(),
					(TEST_DATA[testIndex].m_cashStringExpected).c_str(),
					testIndex,
					(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Functional coverage test for:
		///     CPlayer::GetFormattedCashString(DWORD)
		/// Test edge conditions.
		/// </summary>
		TEST(UtilitiesTests, GetFormattedCashString_DwordTest)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_cashValue;

				// Expected results.
				const string m_cashStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, "$0.00" },
				{ 1, "$0.01" },
				{ 9, "$0.09" },
				{ 10, "$0.10" },
				{ 99, "$0.99" },
				{ 100, "$1.00" },
				{ 999, "$9.99" },
				{ 1000, "$10.00" },
				{ 9999, "$99.99" },
				{ 10000, "$100.00" },
				{ 99999, "$999.99" },
				{ 100000, "$1,000.00" },
				{ 999999, "$9,999.99" },
				{ 1000000, "$10,000.00" },
				{ 9999999, "$99,999.99" },
				{ 10000000, "$100,000.00" },
				{ 99999999, "$999,999.99" },
				{ 100000000, "$1,000,000.00" },
				{ 999999999, "$9,999,999.99" },
				{ 1000000000, "$10,000,000.00" },
				{ (DWORD)INT_MAX - 1, "$21,474,836.46" },
				{ (DWORD)INT_MAX, "$21,474,836.47" },
				{ (DWORD)INT_MIN, "$21,474,836.48" },
				{ (DWORD)INT_MIN + 1, "$21,474,836.49" },
				{ DWORD_MAX - 1, "$42,949,672.94" },
				{ DWORD_MAX, "$42,949,672.95" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				string cashStringResult = CUtilities::GetFormattedCashString(TEST_DATA[testIndex].m_cashValue);

				// Test #1.
				{
					bool areEqual = cashStringResult == TEST_DATA[testIndex].m_cashStringExpected;

					string failedMsg;
					failedMsg = FormatString("FAILED cashStringResult=\"%s\" expected=\"%s\" testIndex=%d in %s)",
					(cashStringResult).c_str(),
					(TEST_DATA[testIndex].m_cashStringExpected).c_str(),
					testIndex,
					(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Functional coverage test for:
		///     CPlayer::GetFormattedCashString(long)
		/// Test edge conditions.
		/// </summary>
		TEST(UtilitiesTests, GetFormattedCashString_LongTest)
		{
			typedef struct
			{
				// Inputs.
				const long m_cashValue;

				// Expected results.
				const string m_cashStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{

                { 0, "$0.00" },                  
                { 1, "$0.01" },                  
                { 9, "$0.09" },                  
                { 10, "$0.10" },                 
                { 99, "$0.99" },                 
                { 100, "$1.00" },                
                { 999, "$9.99" },                
                { 1000, "$10.00" },              
                { 9999, "$99.99" },              
                { 10000, "$100.00" },            
                { 99999, "$999.99" },            
                { 100000, "$1,000.00" },         
                { 999999, "$9,999.99" },         
                { 1000000, "$10,000.00" },       
                { 9999999, "$99,999.99" },       
                { 10000000, "$100,000.00" },     
                { 99999999, "$999,999.99" },     
                { 100000000, "$1,000,000.00" },  
                { 999999999, "$9,999,999.99" },  
                { 1000000000, "$10,000,000.00" },
#  if __WORDSIZE == 64
				{ LONG_MAX - 1, "$92,233,720,368,547,758.06" },
                { LONG_MAX, "$92,233,720,368,547,758.07" },        
                { LONG_MIN, "$-92,233,720,368,547,758.08" },       
                { LONG_MIN + 1, "$-92,233,720,368,547,758.07" },
#  else
				{ LONG_MAX - 1, "$21,474,836.46" },
                { LONG_MAX, "$21,474,836.47" },        
                { LONG_MIN, "$-21,474,836.48" },       
                { LONG_MIN + 1, "$-21,474,836.47" },
#  endif                                        
                { -0, "$0.00" },                       
                { -1, "$-0.01" },                      
                { -9, "$-0.09" },                      
                { -10, "$-0.10" },                     
                { -99, "$-0.99" },                     
                { -100, "$-1.00" },                    
                { -999, "$-9.99" },                    
                { -1000, "$-10.00" },                  
                { -9999, "$-99.99" },                  
                { -10000, "$-100.00" },                
                { -99999, "$-999.99" },                
                { -100000, "$-1,000.00" },             
                { -999999, "$-9,999.99" },             
                { -1000000, "$-10,000.00" },           
                { -9999999, "$-99,999.99" },           
                { -10000000, "$-100,000.00" },         
                { -99999999, "$-999,999.99" },         
                { -100000000, "$-1,000,000.00" },      
                { -999999999, "$-9,999,999.99" },      
                { -1000000000, "$-10,000,000.00" },  
#  if __WORDSIZE == 64
				{ -(LONG_MAX - 1), "$-92,233,720,368,547,758.06" },
                { -LONG_MAX, "$-92,233,720,368,547,758.07" },        
                { -LONG_MIN, "$-92,233,720,368,547,758.08" },       
                { -(LONG_MIN + 1), "$92,233,720,368,547,758.07" },
#  else
				{ -(LONG_MAX - 1), "$-21,474,836.46" },
                { -LONG_MAX, "$-21,474,836.47" },        
                { -LONG_MIN, "$-21,474,836.48" },       
                { -(LONG_MIN + 1), "$21,474,836.47" },
#  endif         
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				string cashStringResult = CUtilities::GetFormattedCashString(TEST_DATA[testIndex].m_cashValue);

				// Test #1.
				{
					bool areEqual = cashStringResult == TEST_DATA[testIndex].m_cashStringExpected;

					string failedMsg;
					failedMsg = FormatString("FAILED cashStringResult=\"%s\" expected=\"%s\" testIndex=%d in %s)",
						(cashStringResult).c_str(),
						(TEST_DATA[testIndex].m_cashStringExpected).c_str(),
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Functional coverage test for:
		///     CPlayer::GetFormattedCashString(DWORD, LPCTSTR=NULL)
		/// Test edge conditions.
		/// </summary>
		TEST(UtilitiesTests, GetFormattedCashString_NullTest)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_cashValue;

				// Expected results.
				const string m_cashStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, "$0.00" },
				{ 1, "$0.01" },
				{ 9, "$0.09" },
				{ 10, "$0.10" },
				{ 99, "$0.99" },
				{ 100, "$1.00" },
				{ 999, "$9.99" },
				{ 1000, "$10.00" },
				{ 9999, "$99.99" },
				{ 10000, "$100.00" },
				{ 99999, "$999.99" },
				{ 100000, "$1,000.00" },
				{ 999999, "$9,999.99" },
				{ 1000000, "$10,000.00" },
				{ 9999999, "$99,999.99" },
				{ 10000000, "$100,000.00" },
				{ 99999999, "$999,999.99" },
				{ 100000000, "$1,000,000.00" },
				{ 999999999, "$9,999,999.99" },
				{ 1000000000, "$10,000,000.00" },
				{ (DWORD)INT_MAX - 1, "$21,474,836.46" },
				{ (DWORD)INT_MAX, "$21,474,836.47" },
				{ (DWORD)INT_MIN, "$21,474,836.48" },
				{ (DWORD)INT_MIN + 1, "$21,474,836.49" },
				{ DWORD_MAX - 1, "$42,949,672.94" },
				{ DWORD_MAX, "$42,949,672.95" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				string cashStringResult = CUtilities::GetFormattedCashString(TEST_DATA[testIndex].m_cashValue, (LPCTSTR)NULL);

				// Test #1.
				{
					bool areEqual = cashStringResult == TEST_DATA[testIndex].m_cashStringExpected;

					string failedMsg;
					failedMsg = FormatString("FAILED cashStringResult=\"%s\" expected=\"%s\" testIndex=%d in %s)",
					(cashStringResult).c_str(),
					(TEST_DATA[testIndex].m_cashStringExpected).c_str(),
					testIndex,
					(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Functional coverage test for:
		///     CPlayer::GetFormattedCashString(DWORD, LPCTSTR)
		/// Test edge conditions for the DWORD cashValue.
		/// </summary>
		TEST(UtilitiesTests, GetFormattedCashString_TextTest)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_cashValue;
				const LPCTSTR m_text;

				// Expected results.
				const string m_cashStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, "", "$0.00 " },
				{ 1, "a", "$0.01 a" },
				{ 9, "ab", "$0.09 ab" },
				{ 10, "abc", "$0.10 abc" },
				{ 99, " $~", "$0.99  $~" },
				{ 100, "abcdefghijklmnopqrstuvwxyz", "$1.00 abcdefghijklmnopqrstuvwxyz" },
				{ 999, "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ", "$9.99 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ" },
				{ 1000, " `1234567890-=[]\\;',./ ~!@#$%^&*()_+{}|:\"<>? ", "$10.00  `1234567890-=[]\\;',./ ~!@#$%^&*()_+{}|:\"<>? " },
				{ 9999, "abc", "$99.99 abc" },
				{ 10000, "abc", "$100.00 abc" },
				{ 99999, "abc", "$999.99 abc" },
				{ 100000, "abc", "$1,000.00 abc" },
				{ 999999, "abc", "$9,999.99 abc" },
				{ 1000000, "abc", "$10,000.00 abc" },
				{ 9999999, "abc", "$99,999.99 abc" },
				{ 10000000, "abc", "$100,000.00 abc" },
				{ 99999999, "abc", "$999,999.99 abc" },
				{ 100000000, "abc", "$1,000,000.00 abc" },
				{ 999999999, "abc", "$9,999,999.99 abc" },
				{ 1000000000, "abc", "$10,000,000.00 abc" },
				{ (DWORD)INT_MAX - 1, "abc", "$21,474,836.46 abc" },
				{ (DWORD)INT_MAX, "abc", "$21,474,836.47 abc" },
				{ (DWORD)INT_MIN, "abc", "$21,474,836.48 abc" },
				{ (DWORD)INT_MIN + 1, "abc", "$21,474,836.49 abc" },
				{ DWORD_MAX - 1, "abc", "$42,949,672.94 abc" },
				{ DWORD_MAX, "abc", "$42,949,672.95 abc" },

				{ (DWORD)INT_MIN,
					"abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 `1234567890-=[]\\;',./ ~!@#$%^&*()_+{}|:\"<>? abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 `1234567890-=[]\\;',./ ~!@#$%^&*()_+{}|:\"<>? ",
					"$21,474,836.48 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 `1234567890-=[]\\;',./ ~!@#$%^&*()_+{}|:\"<>? abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 `1234567890-=[]\\;',./ ~!@#$%^&*()_+{}|:\"<>? " },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				string cashStringResult = CUtilities::GetFormattedCashString(TEST_DATA[testIndex].m_cashValue, TEST_DATA[testIndex].m_text);

				// Test #1.
				{
					bool areEqual = cashStringResult == TEST_DATA[testIndex].m_cashStringExpected;

					string failedMsg;
					failedMsg = FormatString("FAILED cashStringResult=\"%s\" expected=\"%s\" testIndex=%d in %s)",
					(cashStringResult).c_str(),
					(TEST_DATA[testIndex].m_cashStringExpected).c_str(),
					testIndex,
					(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Functional coverage test for:
		///     CPlayer::GetFormattedCashString(DWORD, UINT)
		/// Test edge conditions.
		/// </summary>
		TEST(UtilitiesTests, GetFormattedCashString_IdsTest)
		{
			typedef struct
			{
				// Inputs.
				const DWORD m_val;
				const LPCSTR m_textIds;

				// Expected results.
				const string m_cashStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, IDS_POINTS, "$0.00 points" },
				{ 1, IDS_POINTS_TODAY, "$0.01 points today" },
				{ 9, IDS_AVAILABLE, "$0.09 available" },
				{ 10, IDS_COMP, "$0.10 comp" },
				{ 99, IDS_POINTS_TO_NEXT_TIER, "$0.99 points to next tier" },
				{ 100, IDS_COMP, "$1.00 comp" },
				{ 999, IDS_POINTS, "$9.99 points" },
				{ 1000, IDS_POINTS_TODAY, "$10.00 points today" },
				{ 9999, IDS_AVAILABLE, "$99.99 available" },
				{ 10000, IDS_COMP, "$100.00 comp" },
				{ 99999, IDS_POINTS_TO_NEXT_TIER, "$999.99 points to next tier" },
				{ 100000, IDS_POINTS, "$1,000.00 points" },
				{ 999999, IDS_POINTS, "$9,999.99 points" },
				{ 1000000, IDS_POINTS_TODAY, "$10,000.00 points today" },
				{ 9999999, IDS_AVAILABLE, "$99,999.99 available" },
				{ 10000000, IDS_COMP, "$100,000.00 comp" },
				{ 99999999, IDS_POINTS_TO_NEXT_TIER, "$999,999.99 points to next tier" },
				{ 100000000, IDS_POINTS, "$1,000,000.00 points" },
				{ 999999999, IDS_POINTS, "$9,999,999.99 points" },
				{ 1000000000, IDS_POINTS_TODAY, "$10,000,000.00 points today" },
				{ (DWORD)INT_MAX - 1, IDS_AVAILABLE, "$21,474,836.46 available" },
				{ (DWORD)INT_MAX, IDS_COMP, "$21,474,836.47 comp" },
				{ (DWORD)INT_MIN, IDS_POINTS_TO_NEXT_TIER, "$21,474,836.48 points to next tier" },
				{ (DWORD)INT_MIN + 1, IDS_COMP, "$21,474,836.49 comp" },
				{ DWORD_MAX - 1, IDS_POINTS, "$42,949,672.94 points" },
				{ DWORD_MAX, IDS_POINTS_TODAY, "$42,949,672.95 points today" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				string cashStringResult(CUtilities::GetFormattedCashString(TEST_DATA[testIndex].m_val, TEST_DATA[testIndex].m_textIds));

				// Test #1.
				{
					bool areEqual(cashStringResult == TEST_DATA[testIndex].m_cashStringExpected);

					string failedMsg;
					failedMsg = FormatString("FAILED cashStringResult=\"%s\" expected=\"%s\" testIndex=%d in %s)",
					(cashStringResult).c_str(),
					(TEST_DATA[testIndex].m_cashStringExpected).c_str(),
					testIndex,
					(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUtilities::FileTimeToComponents()
		/// </summary>
		TEST(UtilitiesTests, FileTimeToComponentsTest)
		{
			typedef struct
			{
				// Inputs.
				const ULONGLONG m_fileTime;

				// Expected results.
				const ULONGLONG m_hoursExpected;
				const DWORD m_minutesExpected;
				const DWORD m_secondsExpected;
				const DWORD m_millisecondsExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0xffffffffffffffff, 0xffffffffffffffff / 60 / 60 / 1000 / 10000, 0xffffffffffffffff / 60 / 1000 / 10000 % 60, 0xffffffffffffffff / 1000 / 10000 % 60, 0xffffffffffffffff / 10000 % 1000},
				{ 0, 0, 0, 0, 0 }, // 0:00:00.000   0000.
				{ 1, 0, 0, 0, 0 }, // 0:00:00.000   0001.
				{ 9999, 0, 0, 0, 0 }, // 0:00:00.000   9999.
				{ 10000, 0, 0, 0, 1 }, // 0:00:00.001   0000.
				{ 19999, 0, 0, 0, 1 }, // 0:00:00.001   9999.
				{ 999 * 10000, 0, 0, 0, 999 }, // 0:00:00.999   0000.
				{ 999 * 10000 + 9999, 0, 0, 0, 999 }, // 0:00:00.999   9999.
				{ 999 * 10000 + 9999 + 1, 0, 0, 1, 0 }, // 0:00:01.000   0000.

				{ 1 * 1000 * 10000, 0, 0, 1, 0 }, // 0:00:01.000   0000.
				{ 1 * 1000 * 10000 + 9999, 0, 0, 1, 0 }, // 0:00:01.000   9999.
				{ (1 * 1000 + 999) * 10000, 0, 0, 1, 999 }, // 0:00:01.999   0000.
				{ (1 * 1000 + 999) * 10000 + 9999, 0, 0, 1, 999 }, // 0:00:01.999   9999.
				{ 59 * 1000 * 10000, 0, 0, 59, 0 }, // 0:00:59.000   0000.
				{ 59 * 1000 * 10000 + 9999, 0, 0, 59, 0 }, // 0:00:59.000   9999.
				{ (59 * 1000 + 999) * 10000, 0, 0, 59, 999 }, // 0:00:59.999   0000.
				{ (59 * 1000 + 999) * 10000 + 9999, 0, 0, 59, 999 }, // 0:00:59.999   9999.
				{ (59 * 1000 + 999) * 10000 + 9999 + 1, 0, 1, 0, 0 }, // 0:01:00.000   0000.

				{ 1 * 60 * 1000 * 10000, 0, 1, 0, 0 }, // 0:01:00.000   0000.
				{ 1 * 60 * 1000 * 10000 + 9999, 0, 1, 0, 0 }, // 0:01:00.000   9999.
				{ (1 * 60 * 1000 + 999) * 10000, 0, 1, 0, 999 }, // 0:01:00.999   0000.
				{ (1 * 60 * 1000 + 999) * 10000 + 9999, 0, 1, 0, 999 }, // 0:01:00.999   9999.
				{ (1 * 60 + 59) * 1000 * 10000, 0, 1, 59, 0 }, // 0:01:59.000   0000.
				{ (1 * 60 + 59) * 1000 * 10000 + 9999, 0, 1, 59, 0 }, // 0:01:59.000   9999.
				{ ((1 * 60 + 59) * 1000 + 999) * 10000, 0, 1, 59, 999 }, // 0:01:59.999   0000.
				{ ((1 * 60 + 59) * 1000 + 999) * 10000 + 9999, 0, 1, 59, 999 }, // 0:01:59.999   9999.
				{ 59LL * 60 * 1000 * 10000, 0, 59, 0, 0 }, // 0:59:00.000   0000.
				{ 59LL * 60 * 1000 * 10000 + 9999, 0, 59, 0, 0 }, // 0:59:00.000   9999.
				{ (59LL * 60 * 1000 + 999) * 10000, 0, 59, 0, 999 }, // 0:59:00.999   0000.
				{ (59LL * 60 * 1000 + 999) * 10000 + 9999, 0, 59, 0, 999 }, // 0:59:00.999   9999.
				{ (59LL * 60 + 59) * 1000 * 10000, 0, 59, 59, 0 }, // 0:59:59.000   0000.
				{ (59LL * 60 + 59) * 1000 * 10000 + 9999, 0, 59, 59, 0 }, // 0:59:59.000   9999.
				{ ((59LL * 60 + 59) * 1000 + 999) * 10000, 0, 59, 59, 999 }, // 0:59:59.999   0000.
				{ ((59LL * 60 + 59) * 1000 + 999) * 10000 + 9999, 0, 59, 59, 999 }, // 0:59:59.999   9999.
				{ ((59LL * 60 + 59) * 1000 + 999) * 10000 + 9999 + 1, 1, 0, 0, 0 }, // 1:00:00.000   0000.

				{ 1LL * 60 * 60 * 1000 * 10000, 1, 0, 0, 0 }, // 1:00:00.000   0000.
				{ 1LL * 60 * 60 * 1000 * 10000 + 9999, 1, 0, 0, 0 }, // 1:00:00.000   9999.
				{ (1LL * 60 * 60 * 1000 + 999) * 10000, 1, 0, 0, 999 }, // 1:00:00.999   0000.
				{ (1LL * 60 * 60 * 1000 + 999) * 10000 + 9999, 1, 0, 0, 999 }, // 1:00:00.999   9999.
				{ (1LL * 60 * 60 + 59) * 1000 * 10000, 1, 0, 59, 0 }, // 1:00:59.000   0000.
				{ (1LL * 60 * 60 + 59) * 1000 * 10000 + 9999, 1, 0, 59, 0 }, // 1:00:59.000   9999.
				{ ((1LL * 60 * 60 + 59) * 1000 + 999) * 10000, 1, 0, 59, 999 }, // 1:00:59.999   0000.
				{ ((1LL * 60 * 60 + 59) * 1000 + 999) * 10000 + 9999, 1, 0, 59, 999 }, // 1:00:59.999   9999.
				{ (1LL * 60 + 59) * 60 * 1000 * 10000, 1, 59, 0, 0 }, // 1:59:00.000   0000.
				{ (1LL * 60 + 59) * 60 * 1000 * 10000 + 9999, 1, 59, 0, 0 }, // 1:59:00.000   9999.
				{ ((1LL * 60 + 59) * 60 * 1000 + 999) * 10000, 1, 59, 0, 999 }, // 1:59:00.999   0000.
				{ ((1LL * 60 + 59) * 60 * 1000 + 999) * 10000 + 9999, 1, 59, 0, 999 }, // 1:59:00.999   9999.
				{ ((1LL * 60 + 59) * 60 + 59) * 1000 * 10000, 1, 59, 59, 0 }, // 1:59:59.000   0000.
				{ ((1LL * 60 + 59) * 60 + 59) * 1000 * 10000 + 9999, 1, 59, 59, 0 }, // 1:59:59.000   9999.
				{ (((1LL * 60 + 59) * 60 + 59) * 1000 + 999) * 10000, 1, 59, 59, 999 }, // 1:59:59.999   0000.
				{ (((1LL * 60 + 59) * 60 + 59) * 1000 + 999) * 10000 + 9999, 1, 59, 59, 999 }, // 1:59:59.999   9999.

				{ (((123456789LL * 60 + 1) * 60 + 23) * 1000 + 456) * 10000 + 7890, 123456789, 1, 23, 456 }, // 123456789:01:23.456   7890.

				{ (ULONGLONG)INT64_MIN, 0x8000000000000000 / 60 / 60 / 1000 / 10000, 0x8000000000000000 / 60 / 1000 / 10000 % 60, 0x8000000000000000 / 1000 / 10000 % 60, 0x8000000000000000 / 10000 % 1000},
				{ (ULONGLONG)(INT64_MIN + 1), 0x8000000000000001 / 60 / 60 / 1000 / 10000, 0x8000000000000001 / 60 / 1000 / 10000 % 60, 0x8000000000000001 / 1000 / 10000 % 60, 0x8000000000000001 / 10000 % 1000},
				{ (ULONGLONG)(LONGLONG)INT_MIN, 0xffffffff80000000 / 60 / 60 / 1000 / 10000, 0xffffffff80000000 / 60 / 1000 / 10000 % 60, 0xffffffff80000000 / 1000 / 10000 % 60, 0xffffffff80000000 / 10000 % 1000},
				{ (ULONGLONG)(LONGLONG)(INT_MIN + 1), 0xffffffff80000001 / 60 / 60 / 1000 / 10000, 0xffffffff80000001 / 60 / 1000 / 10000 % 60, 0xffffffff80000001 / 1000 / 10000 % 60, 0xffffffff80000001 / 10000 % 1000},
				{ (ULONGLONG)(DWORD)INT_MIN, 0x80000000 / 60 / 60 / 1000 / 10000, 0x80000000 / 60 / 1000 / 10000 % 60, 0x80000000 / 1000 / 10000 % 60, 0x80000000 / 10000 % 1000},
				{ (ULONGLONG)(DWORD)(INT_MIN + 1), 0x80000001 / 60 / 60 / 1000 / 10000, 0x80000001 / 60 / 1000 / 10000 % 60, 0x80000001 / 1000 / 10000 % 60, 0x80000001 / 10000 % 1000},
				{ (ULONGLONG)(DWORD)(INT_MAX - 1), 0x7ffffffe / 60 / 60 / 1000 / 10000, 0x7ffffffe / 60 / 1000 / 10000 % 60, 0x7ffffffe / 1000 / 10000 % 60, 0x7ffffffe / 10000 % 1000},
				{ (ULONGLONG)(DWORD)INT_MAX, 0x7fffffff / 60 / 60 / 1000 / 10000, 0x7fffffff / 60 / 1000 / 10000 % 60, 0x7fffffff / 1000 / 10000 % 60, 0x7fffffff / 10000 % 1000},
				{ (ULONGLONG)(UINT_MAX - 1), 0xfffffffe / 60 / 60 / 1000 / 10000, 0xfffffffe / 60 / 1000 / 10000 % 60, 0xfffffffe / 1000 / 10000 % 60, 0xfffffffe / 10000 % 1000},
				{ (ULONGLONG)UINT_MAX, 0xffffffff / 60 / 60 / 1000 / 10000, 0xffffffff / 60 / 1000 / 10000 % 60, 0xffffffff / 1000 / 10000 % 60, 0xffffffff / 10000 % 1000},
				{ 0xffffffffffffffff, 0xffffffffffffffff / 60 / 60 / 1000 / 10000, 0xffffffffffffffff / 60 / 1000 / 10000 % 60, 0xffffffffffffffff / 1000 / 10000 % 60, 0xffffffffffffffff / 10000 % 1000},
				{ (ULONGLONG)-1LL, 0xffffffffffffffff / 60 / 60 / 1000 / 10000, 0xffffffffffffffff / 60 / 1000 / 10000 % 60, 0xffffffffffffffff / 1000 / 10000 % 60, 0xffffffffffffffff / 10000 % 1000},
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				DWORD hoursResult(0xcdefcdef);
				DWORD minutesResult(0xcdefcdef);
				DWORD secondsResult(0xcdefcdef);
				DWORD millisecondsResult(0xcdefcdef);
				CUtilities::FileTimeToComponents(TEST_DATA[testIndex].m_fileTime, hoursResult, minutesResult, secondsResult, millisecondsResult);

				// Test #1.
				{
					const bool success(hoursResult == TEST_DATA[testIndex].m_hoursExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("hoursResult=%llu expected=%llu testIndex=%d in %s(%d)",
							(unsigned long long)hoursResult,
							(unsigned long long)TEST_DATA[testIndex].m_hoursExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #2.
				{
					const bool success(minutesResult == TEST_DATA[testIndex].m_minutesExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("minutesResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)minutesResult,
							(unsigned)TEST_DATA[testIndex].m_minutesExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #3.
				{
					const bool success(secondsResult == TEST_DATA[testIndex].m_secondsExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("secondsResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)secondsResult,
							(unsigned)TEST_DATA[testIndex].m_secondsExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #4.
				{
					const bool success(millisecondsResult == TEST_DATA[testIndex].m_millisecondsExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("millisecondsResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)millisecondsResult,
							(unsigned)TEST_DATA[testIndex].m_millisecondsExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

        TEST(UtilitiesTests, MakeMMDDYYYYFromCTimeTest)
		{
			const int BCD_VAL_LENGTH = 4;
			typedef struct
			{
				const time_t  date;
				const byte bcdValExpected[BCD_VAL_LENGTH];
				const bool returnValueExpected;
			} TestDataType;

            time_t time[13];
            struct tm timeinfo;
            //The time structure should be in UTC time

            timeinfo.tm_year = 71;
            timeinfo.tm_mon = 1;
            timeinfo.tm_mday = 1;
            timeinfo.tm_hour = 0;
            timeinfo.tm_min = 0;
            timeinfo.tm_sec = 1;
            timeinfo.tm_isdst = 0;
            time[0] = timegm(&timeinfo);

            timeinfo.tm_year = 71;
            timeinfo.tm_mon = 2;
            timeinfo.tm_mday = 3;
            time[1] = timegm(&timeinfo);

            timeinfo.tm_year = 71;
            timeinfo.tm_mon = 11;
            timeinfo.tm_mday = 31;
            time[2] = timegm(&timeinfo);

            timeinfo.tm_year = 100;
            timeinfo.tm_mon = 2;
            timeinfo.tm_mday = 28;
            time[3] = timegm(&timeinfo);

            timeinfo.tm_year = 100;
            timeinfo.tm_mon = 2;
            timeinfo.tm_mday = 29;
            time[4] = timegm(&timeinfo);

            timeinfo.tm_year = 100;
            timeinfo.tm_mon = 3;
            timeinfo.tm_mday = 1;
            time[5] = timegm(&timeinfo);

            timeinfo.tm_year = 101;
            timeinfo.tm_mon = 2;
            timeinfo.tm_mday = 3;
            time[6] = timegm(&timeinfo);

            timeinfo.tm_year = 116;
            timeinfo.tm_mon = 2;
            timeinfo.tm_mday = 28;
            time[7] = timegm(&timeinfo);

            timeinfo.tm_year = 116;
            timeinfo.tm_mon = 2;
            timeinfo.tm_mday = 29;
            time[8] = timegm(&timeinfo);

            timeinfo.tm_year = 116;
            timeinfo.tm_mon = 3;
            timeinfo.tm_mday = 1;
            time[9] = timegm(&timeinfo);

            timeinfo.tm_year = 127;
            timeinfo.tm_mon = 1;
            timeinfo.tm_mday = 1;
            time[10] = timegm(&timeinfo);

            timeinfo.tm_year = 127;
            timeinfo.tm_mon = 2;
            timeinfo.tm_mday = 3;
            time[11] = timegm(&timeinfo);

            timeinfo.tm_year = 127;
            timeinfo.tm_mon = 11;
            timeinfo.tm_mday = 31;
            time[12] = timegm(&timeinfo);

			const TestDataType TEST_DATA[] =
			{
                //The expected values are changed wrt UTC timezone as test function returns in UTC.
                { time[0], { 0x02, 0x01, 0x19, 0x71 }, true },
                { time[1], { 0x03, 0x03, 0x19, 0x71 }, true }, 
                { time[2], { 0x12, 0x31, 0x19, 0x71 }, true }, 
                { time[3], { 0x03, 0x28, 0x20, 0x00 }, true }, 
                { time[4], { 0x03, 0x29, 0x20, 0x00 }, true }, 
                { time[5], { 0x04, 0x01, 0x20, 0x00 }, true }, 
                { time[6], { 0x03, 0x03, 0x20, 0x01 }, true }, 
                { time[7], { 0x03, 0x28, 0x20, 0x16 }, true }, 
                { time[8], { 0x03, 0x29, 0x20, 0x16 }, true }, 
                { time[9], { 0x04, 0x01, 0x20, 0x16 }, true }, 
                { time[10], { 0x02, 0x01, 0x20, 0x27 }, true },
                { time[11], { 0x03, 0x03, 0x20, 0x27 }, true },
                { time[12], { 0x12, 0x31, 0x20, 0x27 }, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				byte bcdVal[BCD_VAL_LENGTH];
				memset(bcdVal, MEMSET_DEFAULT_VALUE, sizeof bcdVal);

				// Test.
				bool returnValue = CUtilities::MakeMMDDYYYYFromTime(TEST_DATA[testIndex].date, bcdVal);
				bool areEqual1 = 0 == memcmp(bcdVal, TEST_DATA[testIndex].bcdValExpected, BCD_VAL_LENGTH);
				bool areEqual2 = returnValue == TEST_DATA[testIndex].returnValueExpected;

				// Report.
				string failedMsg;
                struct tm* timeinfo = localtime (&TEST_DATA[testIndex].date);
				failedMsg = FormatString("FAILED CUtilities::MakeMMDDYYYYFromCTime(&(CTime(%d,%d,%d), &{0x%2.2x,0x%2.2x,0x%2.2x,0x%2.2x}) returnValue=%d returnValueExpected=%d testIndex=%d)",
					timeinfo->tm_year,
					timeinfo->tm_mon,
					timeinfo->tm_mday,
					TEST_DATA[testIndex].bcdValExpected[0],
					TEST_DATA[testIndex].bcdValExpected[1],
					TEST_DATA[testIndex].bcdValExpected[2],
					TEST_DATA[testIndex].bcdValExpected[3],
					returnValue,
					TEST_DATA[testIndex].returnValueExpected,
					testIndex);
				ASSERT_TRUE(areEqual1 && areEqual2) <<  failedMsg;
			}
		}
		TEST(UtilitiesTests, Dword2BcdTest)
		{
			// Use a longer destination buffer than necessary and check for buffer overflow.
			const int BCD_VAL_TEST_LENGTH = 7;
			typedef struct
			{
				const DWORD dwVal;
				const int BcdSize;
				const byte bcdValExpected[BCD_VAL_TEST_LENGTH];
				const bool returnValueExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, 0, { 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd }, true },
				{ 1, 0, { 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd }, true },

				{ 0, 1, { 0x00, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd }, true },
				{ 1, 1, { 0x01, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd }, true },
				{ 99, 1, { 0x99, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd }, true },
				{ 100, 1, { 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd }, false },

				{ 0, 2, { 0x00, 0x00, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd }, true },
				{ 1, 2, { 0x00, 0x01, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd }, true },
				{ 1234, 2, { 0x12, 0x34, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd }, true },
				{ 9999, 2, { 0x99, 0x99, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd }, true },
				{ 10000, 2, { 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd }, false },

				{ 0, 3, { 0x00, 0x00, 0x00, 0xcd, 0xcd, 0xcd, 0xcd }, true },
				{ 1, 3, { 0x00, 0x00, 0x01, 0xcd, 0xcd, 0xcd, 0xcd }, true },
				{ 123456, 3, { 0x12, 0x34, 0x56, 0xcd, 0xcd, 0xcd, 0xcd }, true },
				{ 999999, 3, { 0x99, 0x99, 0x99, 0xcd, 0xcd, 0xcd, 0xcd }, true },
				{ 1000000, 3, { 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd }, false },

				{ 0, 4, { 0x00, 0x00, 0x00, 0x00, 0xcd, 0xcd, 0xcd }, true },
				{ 1, 4, { 0x00, 0x00, 0x00, 0x01, 0xcd, 0xcd, 0xcd }, true },
				{ 12345678, 4, { 0x12, 0x34, 0x56, 0x78, 0xcd, 0xcd, 0xcd }, true },
				{ 99999999, 4, { 0x99, 0x99, 0x99, 0x99, 0xcd, 0xcd, 0xcd }, true },
				{ 100000000, 4, { 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd }, false },

				{ 0, 5, { 0x00, 0x00, 0x00, 0x00, 0x00, 0xcd, 0xcd }, true },
				{ 1, 5, { 0x00, 0x00, 0x00, 0x00, 0x01, 0xcd, 0xcd }, true },
				{ 1234567890, 5, { 0x12, 0x34, 0x56, 0x78, 0x90, 0xcd, 0xcd }, true },
				{ 1999999999, 5, { 0x19, 0x99, 0x99, 0x99, 0x99, 0xcd, 0xcd }, true },
				{ 0xffffffff, 5, { 0x42, 0x94, 0x96, 0x72, 0x95, 0xcd, 0xcd }, true },

				{ 0, 6, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcd }, true },
				{ 1, 6, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xcd }, true },
				{ 1234567890, 6, { 0x00, 0x12, 0x34, 0x56, 0x78, 0x90, 0xcd }, true },
				{ 1999999999, 6, { 0x00, 0x19, 0x99, 0x99, 0x99, 0x99, 0xcd }, true },
				{ 0xffffffff, 6, { 0x00, 0x42, 0x94, 0x96, 0x72, 0x95, 0xcd }, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				byte bcdVal[BCD_VAL_TEST_LENGTH];
				memset(bcdVal, MEMSET_DEFAULT_VALUE, sizeof bcdVal);

				// Test.
				bool returnValue = CUtilities::Dword2Bcd(TEST_DATA[testIndex].dwVal, bcdVal, TEST_DATA[testIndex].BcdSize);
				bool areEqual1 = 0 == memcmp(bcdVal, TEST_DATA[testIndex].bcdValExpected, sizeof bcdVal);
				bool areEqual2 = returnValue == TEST_DATA[testIndex].returnValueExpected;

				// Report.
				string bcdValStr;
                string formatStr;
				for (int bcdIndex = 0; bcdIndex < _countof(bcdVal); ++bcdIndex)
				{
					if (!bcdValStr.empty())
					{
                        char appendchar = ',';
						bcdValStr.append(&appendchar);
					}
                    int length = sprintf(const_cast<char*>(formatStr.c_str()), "0x%2.2x", bcdVal[bcdIndex]);
                    bcdValStr.append(formatStr.c_str(), length);
				}

				string bcdValExpectedStr;
				for (int bcdIndex = 0; bcdIndex < _countof(TEST_DATA[testIndex].bcdValExpected); ++bcdIndex)
				{
					if (!bcdValExpectedStr.empty())
					{
                        char appendchar = ',';
						bcdValExpectedStr.append(&appendchar);
					}
                    int length = sprintf(const_cast<char*>(formatStr.c_str()), "0x%2.2x", TEST_DATA[testIndex].bcdValExpected[bcdIndex]);
                    bcdValExpectedStr.append(formatStr.c_str(), length);
				}

				string failedMsg;
				failedMsg = FormatString("FAILED CUtilities::Dword2Bcd(%u, &{%s}) bcdValExpected={%s} returnValue=%d returnValueExpected=%d testIndex=%d)",
					TEST_DATA[testIndex].dwVal,
					(bcdValStr).c_str(),
					(bcdValExpectedStr).c_str(),
					returnValue,
					TEST_DATA[testIndex].returnValueExpected,
					testIndex);
				ASSERT_TRUE(areEqual1 && areEqual2) <<  failedMsg;
			}
		}

        TEST(UtilitiesTests, SetTzOffsetTest)                                                                                                                                          
        {                                                                                                                                                                              
            typedef struct                                                                                                                                                             
            {                                                                                                                                                                          
                const int offset; // In minutes.                                                                                                                                       
                const time_t returnValueExpected;                                                                                                                                      
            } TestDataType;                                                                                                                                                            
                                                                                                                                                                                       
            const TestDataType TEST_DATA[] =                                                                                                                                           
            {                                                                                                                                                                          
                { -15 * 60 - 45, ((-15 * 60 - 45) * SECONDS_PER_MINUTE) },                                                                                                             
                { -15 * 60 - 30, ((-15 * 60 - 30) * SECONDS_PER_MINUTE) },                                                                                                             
                { -15 * 60 - 15, ((-15 * 60 - 15) * SECONDS_PER_MINUTE) },                                                                                                             
                                                                                                                                                                                       
                { -15 * 60, (-15 * 60 * SECONDS_PER_MINUTE) },                                                                                                                         
                { -14 * 60, (-14 * 60 * SECONDS_PER_MINUTE) },                                                                                                                         
                { -13 * 60, (-13 * 60 * SECONDS_PER_MINUTE) },                                                                                                                         
                { -12 * 60, (-12 * 60 * SECONDS_PER_MINUTE) },                                                                                                                         
                { -11 * 60, (-11 * 60 * SECONDS_PER_MINUTE) },                                                                                                                         
                { -10 * 60, (-10 * 60 * SECONDS_PER_MINUTE) },                                                                                                                         
                { -9 * 60, (-9 * 60 * SECONDS_PER_MINUTE) },                                                                                                                           
                { -8 * 60, (-8 * 60 * SECONDS_PER_MINUTE) },                                                                                                                           
                { -7 * 60, (-7 * 60 * SECONDS_PER_MINUTE) },                                                                                                                           
                { -6 * 60, (-6 * 60 * SECONDS_PER_MINUTE) },                                                                                                                           
                { -5 * 60, (-5 * 60 * SECONDS_PER_MINUTE) },                                                                                                                           
                { -4 * 60, (-4 * 60 * SECONDS_PER_MINUTE) },                                                                                                                           
                { -3 * 60, (-3 * 60 * SECONDS_PER_MINUTE) },                                                                                                                           
                { -2 * 60, (-2 * 60 * SECONDS_PER_MINUTE) },                                                                                                                           
                { -1 * 60, (-1 * 60 * SECONDS_PER_MINUTE) },                                                                                                                           
                                                                                                                                                                                       
                { -45, (-45 * SECONDS_PER_MINUTE) },                                                                                                                                   
                { -30, (-30 * SECONDS_PER_MINUTE) },                                                                                                                                   
                { -15, (-15 * SECONDS_PER_MINUTE) },                                                                                                                                   
                { -14, (-14 * SECONDS_PER_MINUTE) },                                                                                                                                   
                { -13, (-13 * SECONDS_PER_MINUTE) },                                                                                                                                   
                { -12, (-12 * SECONDS_PER_MINUTE) },                                                                                                                                   
                { -11, (-11 * SECONDS_PER_MINUTE) },                                                                                                                                   
                { -10, (-10 * SECONDS_PER_MINUTE) },                                                                                                                                   
                { -9, (-9 * SECONDS_PER_MINUTE) },                                                                                                                                     
                { -8, (-8 * SECONDS_PER_MINUTE) },                                                                                                                                     
                { -7, (-7 * SECONDS_PER_MINUTE) },                                                                                                                                     
                { -6, (-6 * SECONDS_PER_MINUTE) },                                                                                                                                     
                { -5, (-5 * SECONDS_PER_MINUTE) },                                                                                                                                     
                { -4, (-4 * SECONDS_PER_MINUTE) },                                                                                                                                     
                { -3, (-3 * SECONDS_PER_MINUTE) },                                                                                                                                     
                { -2, (-2 * SECONDS_PER_MINUTE) },                                                                                                                                     
                { -1, (-1 * SECONDS_PER_MINUTE) },                                                                                                                                     
                                                                                                                                                                                       
                { 0, (0 * SECONDS_PER_MINUTE) },                                                                                                                                       
                                                                                                                                                                                       
                { 1, (1 * SECONDS_PER_MINUTE) },                                                                                                                                       
                { 2, (2 * SECONDS_PER_MINUTE) },                                                                                                                                       
                { 3, (3 * SECONDS_PER_MINUTE) },                                                                                                                                       
                { 4, (4 * SECONDS_PER_MINUTE) },                                                                                                                                       
                { 5, (5 * SECONDS_PER_MINUTE) },                                                                                                                                       
                { 6, (6 * SECONDS_PER_MINUTE) },                                                                                                                                       
                { 7, (7 * SECONDS_PER_MINUTE) },                                                                                                                                       
                { 8, (8 * SECONDS_PER_MINUTE) },                                                                                                                                       
                { 9, (9 * SECONDS_PER_MINUTE) },                                                                                                                                       
                { 10, (10 * SECONDS_PER_MINUTE) },                                                                                                                                     
                { 11, (11 * SECONDS_PER_MINUTE) },                                                                                                                                     
                { 12, (12 * SECONDS_PER_MINUTE) },                                                                                                                                     
                { 13, (13 * SECONDS_PER_MINUTE) },                                                                                                                                     
                { 14, (14 * SECONDS_PER_MINUTE) },                                                                                                                                     
                { 15, (15 * SECONDS_PER_MINUTE) },                                                                                                                                     
                                                                                                                                                                                       
                { 1 * 60, (1 * 60 * SECONDS_PER_MINUTE) },                                                                                                                             
                { 2 * 60, (2 * 60 * SECONDS_PER_MINUTE) },                                                                                                                             
                { 3 * 60, (3 * 60 * SECONDS_PER_MINUTE) },                                                                                                                             
                { 4 * 60, (4 * 60 * SECONDS_PER_MINUTE) },                                                                                                                             
                { 5 * 60, (5 * 60 * SECONDS_PER_MINUTE) },                                                                                                                             
                { 6 * 60, (6 * 60 * SECONDS_PER_MINUTE) },                                                                                                                             
                { 7 * 60, (7 * 60 * SECONDS_PER_MINUTE) },                                                                                                                             
                { 8 * 60, (8 * 60 * SECONDS_PER_MINUTE) },                                                                                                                             
                { 9 * 60, (9 * 60 * SECONDS_PER_MINUTE) },                                                                                                                             
                { 10 * 60, (10 * 60 * SECONDS_PER_MINUTE) },                                                                                                                           
                { 11 * 60, (11 * 60 * SECONDS_PER_MINUTE) },                                                                                                                           
                { 12 * 60, (12 * 60 * SECONDS_PER_MINUTE) },                                                                                                                           
                { 13 * 60, (13 * 60 * SECONDS_PER_MINUTE) },                                                                                                                           
                { 14 * 60, (14 * 60 * SECONDS_PER_MINUTE) },                                                                                                                           
                { 15 * 60, (15 * 60 * SECONDS_PER_MINUTE) },                                                                                                                           
                                                                                                                                                                                       
                { 15 * 60 + 15, ((15 * 60 + 15) * SECONDS_PER_MINUTE) },                                                                                                               
                { 15 * 60 + 30, ((15 * 60 + 30) * SECONDS_PER_MINUTE) },                                                                                                               
                { 15 * 60 + 45, ((15 * 60 + 45) * SECONDS_PER_MINUTE) },                                                                                                               
            };                                                                                                                                                                         
                                                                                                                                                                                       
            for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)                                                                                                      
            {                                                                                                                                                                          
                // Setup.                                                                                                                                                              
                                                                                                                                                                                       
                // Test.                                                                                                                                                               
                CUtilities::SetTzOffset(TEST_DATA[testIndex].offset);                                                                                                                  
                time_t returnValue = CUtilities::GetTzOffset();                                                                                                                        
                bool areEqual1 = TEST_DATA[testIndex].returnValueExpected == returnValue;                                                                                              
                bool areEqual2 = TEST_DATA[testIndex].offset == returnValue/SECONDS_PER_MINUTE;                                                                                        
                                                                                                                                                                                       
                // Report.                                                                                                                                                             
                string failedMsg;                                                                                                                                                      
                failedMsg = FormatString("FAILED CUtilities::SetTzOffset(%d) GetTzOffset()returnValue.GetTotalMinutes()=%lld returnValueExpected.GetTotalMinutes()=%lld testIndex=%d)",
                    TEST_DATA[testIndex].offset,                                                                                                                                       
                    (long long)returnValue,                                                                                                                                            
                    (long long)TEST_DATA[testIndex].returnValueExpected,                                                                                                               
                    testIndex);                                                                                                                                                        
                ASSERT_TRUE(areEqual1 && areEqual2) <<  failedMsg;                                                                                                                     
            }                                                                                                                                                                          
        }                                                                                                                                                                              

        TEST(UtilitiesTests, GetUTCFromLocalTimeTest)                                                
        {                                                                                            
            typedef struct                                                                           
            {                                                                                        
                const int offset; // In minutes.                                                     
                const time_t  localTime;                                                             
                const time_t  returnValueExpected;                                                   
            } TestDataType;                                                                          
                                                                                                     
            time_t time[45];                                                                         
            struct tm timeinfo;                                                                      
                                                                                                     
            timeinfo.tm_year = 100;                                                                 
            timeinfo.tm_mon = 1;                                                                     
            timeinfo.tm_mday = 1;                                                                    
            timeinfo.tm_hour = 0;                                                                    
            timeinfo.tm_min = 0;                                                                     
            timeinfo.tm_sec = 0;                                                                     
            timeinfo.tm_isdst = 0;                                                                   
            time[0] = mktime(&timeinfo);                                                             
                                                                                                     
            timeinfo.tm_year = 99;                                                                 
            timeinfo.tm_mon = 12;                                                                    
            timeinfo.tm_mday = 31;                                                                   
            timeinfo.tm_hour = 8;                                                                    
            timeinfo.tm_min = 15;                                                                    
            timeinfo.tm_sec = 0;                                                                     
            timeinfo.tm_isdst = 0;                                                                   
            time[1] = mktime(&timeinfo);                                                             
                                                                                                     
            timeinfo.tm_min = 30;                                                                    
            time[2] = mktime(&timeinfo);                                                             
                                                                                                     
            timeinfo.tm_min = 45;                                                                    
            time[3] = mktime(&timeinfo);                                                             
                                                                                                     
            timeinfo.tm_hour = 9;                                                                    
            timeinfo.tm_min = 0;                                                                     
            timeinfo.tm_sec = 0;                                                                     
            time[4] = mktime(&timeinfo);                                                             
                                                                                                     
            timeinfo.tm_hour = 10;                                                                   
            time[5] = mktime(&timeinfo);                                                             
                                                                                                     
            timeinfo.tm_hour = 11;                                                                   
            time[6] = mktime(&timeinfo);                                                             
                                                                                                     
            timeinfo.tm_hour = 12;                                                                   
            time[7] = mktime(&timeinfo);                                                             
                                                                                                     
            timeinfo.tm_hour = 13;                                                                   
            time[8] = mktime(&timeinfo);                                                             
                                                                                                     
            timeinfo.tm_hour = 14;                                                                   
            time[9] = mktime(&timeinfo);                                                             
                                                                                                     
            timeinfo.tm_hour = 15;                                                                   
            time[10] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 16;                                                                   
            time[11] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 17;                                                                   
            time[12] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 18;                                                                   
            time[13] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 19;                                                                   
            time[14] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 20;                                                                   
            time[15] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 21;                                                                   
            time[16] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 22;                                                                   
            time[17] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 23;                                                                   
            time[18] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_min = 15;                                                                    
            time[19] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_min = 30;                                                                    
            time[20] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_min = 45;                                                                    
            time[21] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_min = 59;                                                                    
            time[22] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_year = 100;                                                                 
            timeinfo.tm_mon = 1;                                                                     
            timeinfo.tm_mday = 1;                                                                    
            timeinfo.tm_hour = 0;                                                                    
            timeinfo.tm_min = 1;                                                                     
            timeinfo.tm_sec = 0;                                                                     
            timeinfo.tm_isdst = 0;                                                                   
            time[23] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_min = 15;                                                                    
            time[24] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_min = 30;                                                                    
            time[25] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_min = 45;                                                                    
            time[26] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_year = 100;                                                                 
            timeinfo.tm_mon = 1;                                                                     
            timeinfo.tm_mday = 1;                                                                    
            timeinfo.tm_hour = 1;                                                                    
            timeinfo.tm_min = 0;                                                                     
            timeinfo.tm_sec = 0;                                                                     
            timeinfo.tm_isdst = 0;                                                                   
            time[27] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 2;                                                                    
            time[28] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 3;                                                                    
            time[29] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 4;                                                                    
            time[30] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 5;                                                                    
            time[31] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 6;                                                                    
            time[32] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 7;                                                                    
            time[33] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 8;                                                                    
            time[34] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 9;                                                                    
            time[35] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 10;                                                                   
            time[36] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 11;                                                                   
            time[37] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 12;                                                                   
            time[38] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 13;                                                                   
            time[39] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 14;                                                                   
            time[40] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_hour = 15;                                                                   
            time[41] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_year = 100;                                                                 
            timeinfo.tm_mon = 1;                                                                     
            timeinfo.tm_mday = 1;                                                                    
            timeinfo.tm_hour = 15;                                                                   
            timeinfo.tm_min = 15;                                                                    
            timeinfo.tm_sec = 0;                                                                     
            timeinfo.tm_isdst = 0;                                                                   
            time[42] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_min = 30;                                                                    
            time[43] = mktime(&timeinfo);                                                            
                                                                                                     
            timeinfo.tm_min = 45;                                                                    
            time[44] = mktime(&timeinfo);                                                            
                                                                                                     
            const TestDataType TEST_DATA[] =                                                         
            {                                                                                        
                { 15 * 60 + 45, time[0], time[1] },                                                  
                { 15 * 60 + 30, time[0], time[2] },                                                  
                { 15 * 60 + 15, time[0], time[3] },                                                  
                { 15 * 60, time[0], time[4] },                                                       
                { 14 * 60, time[0], time[5] },                                                       
                { 13 * 60, time[0], time[6] },                                                       
                { 12 * 60, time[0], time[7] },                                                       
                { 11 * 60, time[0], time[8] },                                                       
                { 10 * 60, time[0], time[9] },                                                       
                { 9 * 60, time[0], time[10] },                                                       
                { 8 * 60, time[0], time[11] },                                                       
                { 7 * 60, time[0], time[12] },                                                       
                { 6 * 60, time[0], time[13] },                                                       
                { 5 * 60, time[0], time[14] },                                                       
                { 4 * 60, time[0], time[15] },                                                       
                { 3 * 60, time[0], time[16] },                                                       
                { 2 * 60, time[0], time[17] },                                                       
                { 1 * 60, time[0], time[18] },                                                       
                { 45, time[0], time[19] },                                                           
                { 30, time[0], time[20] },                                                           
                { 15, time[0], time[21] },                                                           
                { 1, time[0], time[22] },                                                            
                { 0, time[0], time[0] },                                                             
                { -1, time[0], time[23] },                                                           
                { -15, time[0], time[24] },                                                          
                { -30, time[0], time[25] },                                                          
                { -45, time[0], time[26] },                                                          
                { -1 * 60, time[0], time[27] },                                                      
                { -2 * 60, time[0], time[28] },                                                      
                { -3 * 60, time[0], time[29] },                                                      
                { -4 * 60, time[0], time[30] },                                                      
                { -5 * 60, time[0], time[31] },                                                      
                { -6 * 60, time[0], time[32] },                                                      
                { -7 * 60, time[0], time[33] },                                                      
                { -8 * 60, time[0], time[34] },                                                      
                { -9 * 60, time[0], time[35] },                                                      
                { -10 * 60, time[0], time[36] },                                                     
                { -11 * 60, time[0], time[37] },                                                     
                { -12 * 60, time[0], time[38] },                                                     
                { -13 * 60, time[0], time[39] },                                                     
                { -14 * 60, time[0], time[40] },                                                     
                { -15 * 60, time[0], time[41] },                                                     
                { -15 * 60 - 15, time[0], time[42] },                                                
                { -15 * 60 - 30, time[0], time[43] },                                                
                { -15 * 60 - 45, time[0], time[44] },                                                
            };                                                                                       
                                                                                                     
            for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)                    
            {                                                                                        
                // Setup.                                                                            
                struct tm* timeinfo = localtime (&TEST_DATA[testIndex].localTime);                   
                struct tm* timeinfoexpected = localtime (&TEST_DATA[testIndex].returnValueExpected); 
                // Test.                                                                             
                CUtilities::SetTzOffset(TEST_DATA[testIndex].offset);                                
                time_t returnValue = CUtilities::GetUTCFromLocalTime(TEST_DATA[testIndex].localTime);
                struct tm* timeinforeturn = localtime (&returnValue);                                
                bool areEqual = TEST_DATA[testIndex].returnValueExpected == returnValue;             
                                                                                                     
                // Report.                                                                           
                string failedMsg;                                                                    
                failedMsg = FormatString(                                                            
                    "FAILED CUtilities::SetTzOffset(%d)" // No comma.                                
                    " GetUTCFromLocalTimeTest(\"%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d\")" // No comma. 
                    " returnValue=\"%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d\"" // No comma.              
                    " returnValueExpected=\"%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d\" testIndex=%d)",    
                    TEST_DATA[testIndex].offset,                                                     
                                                                                                     
                    timeinfo->tm_year,                                                               
                    timeinfo->tm_mon,                                                                
                    timeinfo->tm_mday,                                                               
                    timeinfo->tm_hour,                                                               
                    timeinfo->tm_min,                                                                
                    timeinfo->tm_sec,                                                                
                                                                                                     
                    timeinforeturn->tm_year,                                                         
                    timeinforeturn->tm_mon,                                                          
                    timeinforeturn->tm_mday,                                                         
                    timeinforeturn->tm_hour,                                                         
                    timeinforeturn->tm_min,                                                          
                    timeinforeturn->tm_sec,                                                          
                                                                                                     
                    timeinfoexpected->tm_year,                                                       
                    timeinfoexpected->tm_mon,                                                        
                    timeinfoexpected->tm_mday,                                                       
                    timeinfoexpected->tm_hour,                                                       
                    timeinfoexpected->tm_min,                                                        
                    timeinfoexpected->tm_sec,                                                        
                    testIndex);                                                                      
                ASSERT_TRUE(areEqual) <<  failedMsg;                                                 
            }                                                                                        
        }                                                                                            

		// CUtiltiesAngleOrientationTest
		// Test CUtilities::AngleOrientation().
		TEST(UtilitiesTests, CUtilitiesAngleOrientationTest)
		{
			typedef struct
			{
				const short angle;

				DesktopOrientationType orientationExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ -90, DesktopOrientationNoChange },
				{ 0, DesktopOrientationDefault },
				{ 90, DesktopOrientation90 },
				{ 180, DesktopOrientation180 },
				{ 270, DesktopOrientation270 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				// Test.
				short orientation = CUtilities::AngleOrientation(TEST_DATA[testIndex].angle);

				{
					// Report.
					bool areEqual = orientation == TEST_DATA[testIndex].orientationExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED CUtilities::AngleOrientation(start=%d) returnValue=%d returnValueExpected=%d testIndex=%d) in %s",
						TEST_DATA[testIndex].angle,
						(unsigned)orientation,
						(unsigned)TEST_DATA[testIndex].orientationExpected,
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		// CUtiltiesRotateAngleTest
		// Test CUtilities::RotateAngle().
		TEST(UtilitiesTests, CUtilitiesRotateAngleTest)
		{
			typedef struct
			{
				const short start;
				const short rotate;

				unsigned short resultExpected;
			} TestDataType;

			// 2013-03-20 Do not test out-of-range inputs.
			const TestDataType TEST_DATA[] =
			{
				{ 0, -270, 90 },
				{ 0, -180, 180 },
				{ 0, -90, 270 },
				{ 0, 0, 0 },
				{ 0, 90, 90 },
				{ 0, 180, 180 },
				{ 0, 270, 270 },

				{ 90, -270, 180 },
				{ 90, -180, 270 },
				{ 90, -90, 0 },
				{ 90, 0, 90 },
				{ 90, 90, 180 },
				{ 90, 180, 270 },
				{ 90, 270, 0 },

				{ 180, -270, 270 },
				{ 180, -180, 0 },
				{ 180, -90, 90 },
				{ 180, 0, 180 },
				{ 180, 90, 270 },
				{ 180, 180, 0 },
				{ 180, 270, 90 },

				{ 270, -270, 0 },
				{ 270, -180, 90 },
				{ 270, -90, 180 },
				{ 270, 0, 270 },
				{ 270, 90, 0 },
				{ 270, 180, 90 },
				{ 270, 270, 180 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				// Test.
				short result = CUtilities::RotateAngle(TEST_DATA[testIndex].start, TEST_DATA[testIndex].rotate);

				{
					// Report.
					bool areEqual = result == TEST_DATA[testIndex].resultExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED CUtilities::RotateAngle(start=%d, rotate=%d) returnValue=%d returnValueExpected=%d testIndex=%d) in %s",
						TEST_DATA[testIndex].start,
						(unsigned)TEST_DATA[testIndex].rotate,
						(unsigned)result,
						(unsigned)TEST_DATA[testIndex].resultExpected,
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		// CUtiltiesCRCTest
		// Test CUtilities::CRC().
		TEST(UtilitiesTests, CUtilitiesCRCTest)
		{
			typedef struct
			{
				const unsigned char *s;
				const int len;
				const size_t size;
				const unsigned short crcval;

				unsigned short crcResultExpected;
			} TestDataType;

			const unsigned char s1[1] = { 'a' };
			const unsigned char s2[2] = { 'a', 'b' };
			const unsigned char s3[3] = { 'a', 'b', 'c' };

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, 0, 0x1234, 0x1234 },
				{ s1, _countof(s1), sizeof(s1), 0x0000, 0x728f },
				{ s2, _countof(s2), sizeof(s2), 0xffff, 0xcc21 },
				{ s3, _countof(s3), sizeof(s3), 0xffff, 0x61da },
				{ NULL, -1, 0, 0x5678, 0x5678 }, // Test "len" with negative value, since CUtilities::CRC() is designed to handle it.
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// 2013-03-20 Cannot pass constants to CUtilities::CRC().
				unsigned char *var_s = new unsigned char[TEST_DATA[testIndex].size];
				memcpy(var_s, TEST_DATA[testIndex].s, TEST_DATA[testIndex].size);

				// Perform operation(s) to be tested.
				// Test.
				unsigned short crcResult = CUtilities::CRC(var_s, TEST_DATA[testIndex].len, TEST_DATA[testIndex].crcval);

				{
					// Report.
					bool areEqual = crcResult == TEST_DATA[testIndex].crcResultExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED CUtilities::CRC(s, len=%d, crcval=0x%4.4x) returnValue=0x%4.4x returnValueExpected=0x%4.4x testIndex=%d) in %s",
						TEST_DATA[testIndex].len,
						(unsigned)TEST_DATA[testIndex].crcval,
						(unsigned)crcResult,
						(unsigned)TEST_DATA[testIndex].crcResultExpected,
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
				delete[] var_s;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CUtilities::MemCopyZero_FromOffset().
		/// </summary>
		/// <remarks>
		/// Test with an extra check byte before and after destination buffer.
		/// </remarks>
		TEST(UtilitiesTests, MemCopyZero_FromOffsetTest)
		{
			typedef struct
			{
				// Inputs.
				const bool m_ok;
				const void *m_pTestBuffer;
				const size_t m_testBufferSize;
				const size_t m_destBufferOffset; // Offset into test buffer.
				const size_t m_destBufferSize;
				const byte *m_pSrcBuffer;
				const size_t m_srcBufferSize;
				const size_t m_srcBufferOffset;

				// Expected results.
				const bool m_okExpected;
				const void *m_pTestBufferExpected;
				const size_t m_srcBufferOffsetExpected;
			} TestDataType;

			// Copy/Fill zero bytes.
			const byte testBuffer0[] = { 0xcd, 0xcd };
			const byte testBufferExpected0[] = { 0xcd, 0xcd };

			const byte testBuffer1[] = { 0xcd, 0xcd };
			const byte testBufferExpected1[] = { 0xcd, 0xcd };

			const byte testBuffer2[] = { 0xcd, 0xcd };
			const byte srcBuffer2[] = { 0x01 };
			const byte testBufferExpected2[] = { 0xcd, 0xcd };

			const byte testBuffer3[] = { 0xcd, 0xcd };
			const byte srcBuffer3[] = { 0x01 };
			const byte testBufferExpected3[] = { 0xcd, 0xcd };

			const byte testBuffer4[] = { 0xcd, 0xcd };
			const byte srcBuffer4[] = { 0x01 };
			const byte testBufferExpected4[] = { 0xcd, 0xcd };

			// Fill 1 byte.
			const byte testBuffer5[] = { 0xcd, 0xcd, 0xcd };
			const byte testBufferExpected5[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer6[] = { 0xcd, 0xcd, 0xcd };
			const byte testBufferExpected6[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer7[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer7[] = { 0x01 };
			const byte testBufferExpected7[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer8[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer8[] = { 0x01 };
			const byte testBufferExpected8[] = { 0xcd, 0x00, 0xcd };

			// Copy 1 byte.
			const byte testBuffer9[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer9[] = { 0x00 };
			const byte testBufferExpected9[] = { 0xcd, 0x00, 0xcd };
			const byte testBufferExpected9f[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer10[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer10[] = { 0x01 };
			const byte testBufferExpected10[] = { 0xcd, 0x01, 0xcd };
			const byte testBufferExpected10f[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer11[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer11[] = { 0xfe };
			const byte testBufferExpected11[] = { 0xcd, 0xfe, 0xcd };
			const byte testBufferExpected11f[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer12[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer12[] = { 0xff };
			const byte testBufferExpected12[] = { 0xcd, 0xff, 0xcd };
			const byte testBufferExpected12f[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer13[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer13[] = { 0x01, 0x02 };
			const byte testBufferExpected13[] = { 0xcd, 0x01, 0xcd };
			const byte testBufferExpected13f[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer14[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer14[] = { 0xff, 0xfe };
			const byte testBufferExpected14[] = { 0xcd, 0xfe, 0xcd };
			const byte testBufferExpected14f[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer15[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer15[] = { 0x01, 0x02, 0x03 };
			const byte testBufferExpected15[] = { 0xcd, 0x02, 0xcd };
			const byte testBufferExpected15f[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer16[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer16[] = { 0xff, 0xfe, 0xfd };
			const byte testBufferExpected16[] = { 0xcd, 0xfd, 0xcd };
			const byte testBufferExpected16f[] = { 0xcd, 0x00, 0xcd };

			// Fill 2 bytes.
			const byte testBuffer17[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte testBufferExpected17[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer18[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte testBufferExpected18[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer19[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer19[] = { 0x03 };
			const byte testBufferExpected19[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer20[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer20[] = { 0x01 };
			const byte testBufferExpected20[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer21[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer21[] = { 0x02 };
			const byte testBufferExpected21[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer22[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer22[] = { 0xff, 0xfe };
			const byte testBufferExpected22[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer23[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer23[] = { 0xfe, 0xfd };
			const byte testBufferExpected23[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer24[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer24[] = { 0x01, 0x02, 0x03 };
			const byte testBufferExpected24[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer25[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer25[] = { 0xfe, 0xfd, 0xfc };
			const byte testBufferExpected25[] = { 0xcd, 0x00, 0x00, 0xcd };

			// Copy 2 bytes.
			const byte testBuffer26[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer26[] = { 0x00, 0x00 };
			const byte testBufferExpected26[] = { 0xcd, 0x00, 0x00, 0xcd };
			const byte testBufferExpected26f[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer27[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer27[] = { 0x01, 0x02 };
			const byte testBufferExpected27[] = { 0xcd, 0x01, 0x02, 0xcd };
			const byte testBufferExpected27f[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer28[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer28[] = { 0xfe, 0xfd };
			const byte testBufferExpected28[] = { 0xcd, 0xfe, 0xfd, 0xcd };
			const byte testBufferExpected28f[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer29[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer29[] = { 0xff, 0xff };
			const byte testBufferExpected29[] = { 0xcd, 0xff, 0xff, 0xcd };
			const byte testBufferExpected29f[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer30[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer30[] = { 0x01, 0x02, 0x03 };
			const byte testBufferExpected30[] = { 0xcd, 0x01, 0x02, 0xcd };
			const byte testBufferExpected30f[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer31[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer31[] = { 0xff, 0xfe, 0xfd };
			const byte testBufferExpected31[] = { 0xcd, 0xfe, 0xfd, 0xcd };
			const byte testBufferExpected31f[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer32[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer32[] = { 0x01, 0x02, 0x03, 0x04 };
			const byte testBufferExpected32[] = { 0xcd, 0x02, 0x03, 0xcd };
			const byte testBufferExpected32f[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer33[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer33[] = { 0xff, 0xfe, 0xfd, 0xfc };
			const byte testBufferExpected33[] = { 0xcd, 0xfd, 0xfc, 0xcd };
			const byte testBufferExpected33f[] = { 0xcd, 0x00, 0x00, 0xcd };

			const TestDataType TEST_DATA[] =
			{
				// Copy/Fill zero bytes.
				{ true, testBuffer0, sizeof(testBuffer0), 1, sizeof(testBuffer0) - 2, NULL, 0, 0,
					true, testBufferExpected0, 0 },
				{ true, testBuffer1, sizeof(testBuffer1), 1, sizeof(testBuffer1) - 2, NULL, 100, 0,
					true, testBufferExpected1, 0 },
				{ true, testBuffer2, sizeof(testBuffer2), 1, sizeof(testBuffer2) - 2, srcBuffer2, 0, 0,
					true, testBufferExpected2, 0 },
				{ true, testBuffer3, sizeof(testBuffer3), 1, sizeof(testBuffer3) - 2, srcBuffer3, sizeof(srcBuffer3), 0,
					true, testBufferExpected3, 0 },
				{ true, testBuffer4, sizeof(testBuffer4), 1, sizeof(testBuffer4) - 2, srcBuffer4, sizeof(srcBuffer4), 1,
					true, testBufferExpected4, 1 },

				// Fill 1 byte.
				{ true, testBuffer5, sizeof(testBuffer5), 1, sizeof(testBuffer5) - 2, NULL, 0, 0,
					false, testBufferExpected5, 0 },
				{ true, testBuffer6, sizeof(testBuffer6), 1, sizeof(testBuffer6) - 2, NULL, 100, 0,
					false, testBufferExpected6, 0 },
				{ true, testBuffer7, sizeof(testBuffer7), 1, sizeof(testBuffer7) - 2, srcBuffer7, 0, 0,
					false, testBufferExpected7, 0 },
				{ true, testBuffer8, sizeof(testBuffer8), 1, sizeof(testBuffer8) - 2, srcBuffer8, sizeof(srcBuffer8), 1,
					false, testBufferExpected8, 1 },

				// Copy 1 byte.
				{ true, testBuffer9, sizeof(testBuffer9), 1, sizeof(testBuffer9) - 2, srcBuffer9, sizeof(srcBuffer9), 0,
					true, testBufferExpected9, 1 },
				{ true, testBuffer10, sizeof(testBuffer10), 1, sizeof(testBuffer10) - 2, srcBuffer10, sizeof(srcBuffer10), 0,
					true, testBufferExpected10, 1 },
				{ true, testBuffer11, sizeof(testBuffer11), 1, sizeof(testBuffer11) - 2, srcBuffer11, sizeof(srcBuffer11), 0,
					true, testBufferExpected11, 1 },
				{ true, testBuffer12, sizeof(testBuffer12), 1, sizeof(testBuffer12) - 2, srcBuffer12, sizeof(srcBuffer12), 0,
					true, testBufferExpected12, 1 },
				{ true, testBuffer13, sizeof(testBuffer13), 1, sizeof(testBuffer13) - 2, srcBuffer13, sizeof(srcBuffer13), 0,
					true, testBufferExpected13, 1 },
				{ true, testBuffer14, sizeof(testBuffer14), 1, sizeof(testBuffer14) - 2, srcBuffer14, sizeof(srcBuffer14), 1,
					true, testBufferExpected14, 2 },
				{ true, testBuffer15, sizeof(testBuffer15), 1, sizeof(testBuffer15) - 2, srcBuffer15, sizeof(srcBuffer15), 1,
					true, testBufferExpected15, 2 },
				{ true, testBuffer16, sizeof(testBuffer16), 1, sizeof(testBuffer16) - 2, srcBuffer16, sizeof(srcBuffer16), 2,
					true, testBufferExpected16, 3 },

				// Fill 2 bytes.
				{ true, testBuffer17, sizeof(testBuffer17), 1, sizeof(testBuffer17) - 2, NULL, 0, 0,
					false, testBufferExpected17, 0 },
				{ true, testBuffer18, sizeof(testBuffer18), 1, sizeof(testBuffer18) - 2, NULL, 100, 0,
					false, testBufferExpected18, 0 },
				{ true, testBuffer19, sizeof(testBuffer19), 1, sizeof(testBuffer19) - 2, srcBuffer19, 0, 0,
					false, testBufferExpected19, 0 },
				{ true, testBuffer20, sizeof(testBuffer20), 1, sizeof(testBuffer20) - 2, srcBuffer20, sizeof(srcBuffer20), 0,
					false, testBufferExpected20, 0 },
				{ true, testBuffer21, sizeof(testBuffer21), 1, sizeof(testBuffer21) - 2, srcBuffer21, sizeof(srcBuffer21), 1,
					false, testBufferExpected21, 1 },
				{ true, testBuffer22, sizeof(testBuffer22), 1, sizeof(testBuffer22) - 2, srcBuffer22, sizeof(srcBuffer22), 1,
					false, testBufferExpected22, 1 },
				{ true, testBuffer23, sizeof(testBuffer23), 1, sizeof(testBuffer23) - 2, srcBuffer23, sizeof(srcBuffer23), 2,
					false, testBufferExpected23, 2 },
				{ true, testBuffer24, sizeof(testBuffer24), 1, sizeof(testBuffer24) - 2, srcBuffer24, sizeof(srcBuffer24), 2,
					false, testBufferExpected24, 2 },
				{ true, testBuffer25, sizeof(testBuffer25), 1, sizeof(testBuffer25) - 2, srcBuffer25, sizeof(srcBuffer25), 3,
					false, testBufferExpected25, 3 },

				// Copy 2 bytes.
				{ true, testBuffer26, sizeof(testBuffer26), 1, sizeof(testBuffer26) - 2, srcBuffer26, sizeof(srcBuffer26), 0,
					true, testBufferExpected26, 2 },
				{ true, testBuffer27, sizeof(testBuffer27), 1, sizeof(testBuffer27) - 2, srcBuffer27, sizeof(srcBuffer27), 0,
					true, testBufferExpected27, 2 },
				{ true, testBuffer28, sizeof(testBuffer28), 1, sizeof(testBuffer28) - 2, srcBuffer28, sizeof(srcBuffer28), 0,
					true, testBufferExpected28, 2 },
				{ true, testBuffer29, sizeof(testBuffer29), 1, sizeof(testBuffer29) - 2, srcBuffer29, sizeof(srcBuffer29), 0,
					true, testBufferExpected29, 2 },
				{ true, testBuffer30, sizeof(testBuffer30), 1, sizeof(testBuffer30) - 2, srcBuffer30, sizeof(srcBuffer30), 0,
					true, testBufferExpected30, 2 },
				{ true, testBuffer31, sizeof(testBuffer31), 1, sizeof(testBuffer31) - 2, srcBuffer31, sizeof(srcBuffer31), 1,
					true, testBufferExpected31, 3 },
				{ true, testBuffer32, sizeof(testBuffer32), 1, sizeof(testBuffer32) - 2, srcBuffer32, sizeof(srcBuffer32), 1,
					true, testBufferExpected32, 3 },
				{ true, testBuffer33, sizeof(testBuffer33), 1, sizeof(testBuffer33) - 2, srcBuffer33, sizeof(srcBuffer33), 2,
					true, testBufferExpected33, 4 },

				// Same tests with ok=false.

				// Copy/Fill zero bytes with ok=false
				{ false, testBuffer0, sizeof(testBuffer0), 1, sizeof(testBuffer0) - 2, NULL, 0, 0,
					false, testBufferExpected0, 0 },
				{ false, testBuffer1, sizeof(testBuffer1), 1, sizeof(testBuffer1) - 2, NULL, 100, 0,
					false, testBufferExpected1, 0 },
				{ false, testBuffer2, sizeof(testBuffer2), 1, sizeof(testBuffer2) - 2, srcBuffer2, 0, 0,
					false, testBufferExpected2, 0 },
				{ false, testBuffer3, sizeof(testBuffer3), 1, sizeof(testBuffer3) - 2, srcBuffer3, sizeof(srcBuffer3), 0,
					false, testBufferExpected3, 0 },

				// Fill 1 byte with ok=false.
				{ false, testBuffer5, sizeof(testBuffer5), 1, sizeof(testBuffer5) - 2, NULL, 0, 0,
					false, testBufferExpected5, 0 },
				{ false, testBuffer6, sizeof(testBuffer6), 1, sizeof(testBuffer6) - 2, NULL, 100, 0,
					false, testBufferExpected6, 0 },
				{ false, testBuffer7, sizeof(testBuffer7), 1, sizeof(testBuffer7) - 2, srcBuffer7, 0, 0,
					false, testBufferExpected7, 0 },
				{ false, testBuffer8, sizeof(testBuffer8), 1, sizeof(testBuffer8) - 2, srcBuffer8, sizeof(srcBuffer8), sizeof(srcBuffer8),
					false, testBufferExpected8, sizeof(srcBuffer8) },

				// Not copy 1 byte with ok=false.
				{ false, testBuffer9, sizeof(testBuffer9), 1, sizeof(testBuffer9) - 2, srcBuffer9, sizeof(srcBuffer9), 0,
					false, testBufferExpected9f, 0 },
				{ false, testBuffer10, sizeof(testBuffer10), 1, sizeof(testBuffer10) - 2, srcBuffer10, sizeof(srcBuffer10), 0,
					false, testBufferExpected10f, 0 },
				{ false, testBuffer11, sizeof(testBuffer11), 1, sizeof(testBuffer11) - 2, srcBuffer11, sizeof(srcBuffer11), 0,
					false, testBufferExpected11f, 0 },
				{ false, testBuffer12, sizeof(testBuffer12), 1, sizeof(testBuffer12) - 2, srcBuffer12, sizeof(srcBuffer12), 0,
					false, testBufferExpected12f, 0 },
				{ false, testBuffer13, sizeof(testBuffer13), 1, sizeof(testBuffer13) - 2, srcBuffer13, sizeof(srcBuffer13), 0,
					false, testBufferExpected13f, 0 },
				{ false, testBuffer14, sizeof(testBuffer14), 1, sizeof(testBuffer14) - 2, srcBuffer14, sizeof(srcBuffer14), 1,
					false, testBufferExpected14f, 1 },
				{ false, testBuffer15, sizeof(testBuffer15), 1, sizeof(testBuffer15) - 2, srcBuffer15, sizeof(srcBuffer15), 1,
					false, testBufferExpected15f, 1 },
				{ false, testBuffer16, sizeof(testBuffer16), 1, sizeof(testBuffer16) - 2, srcBuffer16, sizeof(srcBuffer16), 2,
					false, testBufferExpected16f, 2 },

					// Fill 2 bytes with ok=false.
				{ false, testBuffer17, sizeof(testBuffer17), 1, sizeof(testBuffer17) - 2, NULL, 0, 0,
					false, testBufferExpected17, 0 },
				{ false, testBuffer18, sizeof(testBuffer18), 1, sizeof(testBuffer18) - 2, NULL, 100, 0,
					false, testBufferExpected18, 0 },
				{ false, testBuffer19, sizeof(testBuffer19), 1, sizeof(testBuffer19) - 2, srcBuffer19, 0, 0,
					false, testBufferExpected19, 0 },
				{ false, testBuffer20, sizeof(testBuffer20), 1, sizeof(testBuffer20) - 2, srcBuffer20, sizeof(srcBuffer20), 0,
					false, testBufferExpected20, 0 },
				{ false, testBuffer21, sizeof(testBuffer21), 1, sizeof(testBuffer21) - 2, srcBuffer21, sizeof(srcBuffer21), 1,
					false, testBufferExpected21, 1 },
				{ false, testBuffer22, sizeof(testBuffer22), 1, sizeof(testBuffer22) - 2, srcBuffer22, sizeof(srcBuffer22), 1,
					false, testBufferExpected22, 1 },
				{ false, testBuffer23, sizeof(testBuffer23), 1, sizeof(testBuffer23) - 2, srcBuffer23, sizeof(srcBuffer23), 2,
					false, testBufferExpected23, 2 },
				{ false, testBuffer24, sizeof(testBuffer24), 1, sizeof(testBuffer24) - 2, srcBuffer24, sizeof(srcBuffer24), 2,
					false, testBufferExpected24, 2 },
				{ false, testBuffer25, sizeof(testBuffer25), 1, sizeof(testBuffer25) - 2, srcBuffer25, sizeof(srcBuffer25), 3,
					false, testBufferExpected25, 3 },

				// Not copy 2 bytes with ok=false.
				{ false, testBuffer26, sizeof(testBuffer26), 1, sizeof(testBuffer26) - 2, srcBuffer26, sizeof(srcBuffer26), 0,
					false, testBufferExpected26f, 0 },
				{ false, testBuffer27, sizeof(testBuffer27), 1, sizeof(testBuffer27) - 2, srcBuffer27, sizeof(srcBuffer27), 0,
					false, testBufferExpected27f, 0 },
				{ false, testBuffer28, sizeof(testBuffer28), 1, sizeof(testBuffer28) - 2, srcBuffer28, sizeof(srcBuffer28), 0,
					false, testBufferExpected28f, 0 },
				{ false, testBuffer29, sizeof(testBuffer29), 1, sizeof(testBuffer29) - 2, srcBuffer29, sizeof(srcBuffer29), 0,
					false, testBufferExpected29f, 0 },
				{ false, testBuffer30, sizeof(testBuffer30), 1, sizeof(testBuffer30) - 2, srcBuffer30, sizeof(srcBuffer30), 0,
					false, testBufferExpected30f, 0 },
				{ false, testBuffer31, sizeof(testBuffer31), 1, sizeof(testBuffer31) - 2, srcBuffer31, sizeof(srcBuffer31), 1,
					false, testBufferExpected31f, 1 },
				{ false, testBuffer32, sizeof(testBuffer32), 1, sizeof(testBuffer32) - 2, srcBuffer32, sizeof(srcBuffer32), 1,
					false, testBufferExpected32f, 1 },
				{ false, testBuffer33, sizeof(testBuffer33), 1, sizeof(testBuffer33) - 2, srcBuffer33, sizeof(srcBuffer33), 2,
					false, testBufferExpected33f, 2 },
			};

			const int TEST_DATA_MEMCMP_EXPECTED = MEMCMP_EQUAL;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				byte *testBuffer = new byte[TEST_DATA[testIndex].m_testBufferSize];
				memcpy_s(testBuffer,
					TEST_DATA[testIndex].m_testBufferSize,
					TEST_DATA[testIndex].m_pTestBuffer,
					TEST_DATA[testIndex].m_testBufferSize);

				// Perform operation(s) to be tested.
				bool ok = TEST_DATA[testIndex].m_ok;
				size_t srcBufferOffset = TEST_DATA[testIndex].m_srcBufferOffset;
				CUtilities::MemCopyZero_FromOffset(&ok,
					testBuffer + TEST_DATA[testIndex].m_destBufferOffset,
					TEST_DATA[testIndex].m_destBufferSize,
					TEST_DATA[testIndex].m_pSrcBuffer,
					TEST_DATA[testIndex].m_srcBufferSize,
					&srcBufferOffset);

				// Test.
				const int memcmpResult =
					memcmp(testBuffer, TEST_DATA[testIndex].m_pTestBufferExpected, TEST_DATA[testIndex].m_testBufferSize);

				{
					// Report #1.
					bool areEqual = ok == TEST_DATA[testIndex].m_okExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED MemCopyZero_FromOffset() ok=%d expected=%d testIndex=%d in %s",
						ok,
						TEST_DATA[testIndex].m_okExpected,
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				{
					// Report #2.
					bool areEqual = memcmpResult == TEST_DATA_MEMCMP_EXPECTED;
					string failedMsg;
					failedMsg = FormatString("FAILED MemCopyZero_FromOffset() memcmp testIndex=%d in %s",
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				{
					// Report #3.
					bool areEqual = srcBufferOffset == TEST_DATA[testIndex].m_srcBufferOffsetExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED MemCopyZero_FromOffset() srcBufferOffset=%u expected=%u testIndex=%d in %s",
						(unsigned)srcBufferOffset,
						(unsigned)TEST_DATA[testIndex].m_srcBufferOffsetExpected,
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
				delete testBuffer;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CUtilities::MemCopyZeroIncSourcePtr(..., size_t &srcBufferSize).
		/// </summary>
		/// <remarks>
		/// Test with an extra check byte before and after destination buffer.
		/// </remarks>
		TEST(UtilitiesTests, MemCopyZeroIncSourcePtrSizeTest)
		{
			typedef struct
			{
				// Inputs.
				const void *m_pTestBuffer;
				const size_t m_testBufferSize;
				const size_t m_destBufferOffset; // Offset into test buffer.
				const size_t m_destBufferSize;
				const void *m_pSrcBuffer;
				const size_t m_srcBufferSize;

				// Expected results.
				const void *m_pTestBufferExpected;
				const void *m_pSrcBufferExpected;
				const size_t m_srcBufferSizeExpected;
			} TestDataType;

			// Copy/Fill zero bytes.
			const byte testBuffer0[] = { 0xcd, 0xcd };
			const byte *srcBuffer0 = NULL;
			const size_t srcBufferSize0 = 0;
			const byte testBufferExpected0[] = { 0xcd, 0xcd };
			const size_t copySizeExpected0 = srcBuffer0 ? min(sizeof(testBuffer0) - 2, srcBufferSize0) : 0;
			const byte *srcBufferExpected0 = srcBuffer0 + copySizeExpected0;
			const size_t srcBufferSizeExpected0 = srcBuffer0 ? srcBufferSize0 - copySizeExpected0 : 0;

			const byte testBuffer1[] = { 0xcd, 0xcd };
			const byte *srcBuffer1 = NULL;
			const size_t srcBufferSize1 = 100;
			const byte testBufferExpected1[] = { 0xcd, 0xcd };
			const size_t copySizeExpected1 = srcBuffer1 ? min(sizeof(testBuffer1) - 2, srcBufferSize1) : 0;
			const byte *srcBufferExpected1 = srcBuffer1 + copySizeExpected1;
			const size_t srcBufferSizeExpected1 = srcBuffer1 ? srcBufferSize1 - copySizeExpected1 : 0;

			const byte testBuffer2[] = { 0xcd, 0xcd };
			const byte srcBuffer2[] = { 0x01 };
			const size_t srcBufferSize2 = 0;
			const byte testBufferExpected2[] = { 0xcd, 0xcd };
			const size_t copySizeExpected2 = min(sizeof(testBuffer2) - 2, srcBufferSize2);
			const byte *srcBufferExpected2 = srcBuffer2 + copySizeExpected2;
			const size_t srcBufferSizeExpected2 = srcBufferSize2 - copySizeExpected2;

			const byte testBuffer3[] = { 0xcd, 0xcd };
			const byte srcBuffer3[] = { 0x01 };
			const size_t srcBufferSize3 = sizeof(srcBuffer3);
			const byte testBufferExpected3[] = { 0xcd, 0xcd };
			const size_t copySizeExpected3 = min(sizeof(testBuffer3) - 2, srcBufferSize3);
			const byte *srcBufferExpected3 = srcBuffer3 + copySizeExpected3;
			const size_t srcBufferSizeExpected3 = srcBufferSize3 - copySizeExpected3;

			// Copy/Fill 1 byte.
			const byte testBuffer4[] = { 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer4 = NULL;
			const size_t srcBufferSize4 = 0;
			const byte testBufferExpected4[] = { 0xcd, 0x00, 0xcd };
			const size_t copySizeExpected4 = srcBuffer4 ? min(sizeof(testBuffer4) - 2, srcBufferSize4) : 0;
			const byte *srcBufferExpected4 = srcBuffer4 + copySizeExpected4;
			const size_t srcBufferSizeExpected4 = srcBuffer4 ? srcBufferSize4 - copySizeExpected4 : 0;

			const byte testBuffer5[] = { 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer5 = NULL;
			const size_t srcBufferSize5 = 100;
			const byte testBufferExpected5[] = { 0xcd, 0x00, 0xcd };
			const size_t copySizeExpected5 = srcBuffer5 ? min(sizeof(testBuffer5) - 2, srcBufferSize5) : 0;
			const byte *srcBufferExpected5 = srcBuffer5 + copySizeExpected5;
			const size_t srcBufferSizeExpected5 = srcBuffer5 ? srcBufferSize5 - copySizeExpected5 : 0;

			const byte testBuffer6[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer6[] = { 0x01 };
			const size_t srcBufferSize6 = 0;
			const byte testBufferExpected6[] = { 0xcd, 0x00, 0xcd };
			const size_t copySizeExpected6 = min(sizeof(testBuffer6) - 2, srcBufferSize6);
			const byte *srcBufferExpected6 = srcBuffer6 + copySizeExpected6;
			const size_t srcBufferSizeExpected6 = srcBufferSize6 - copySizeExpected6;

			const byte testBuffer7[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer7[] = { 0x01 };
			const size_t srcBufferSize7 = sizeof(srcBuffer7);
			const byte testBufferExpected7[] = { 0xcd, 0x01, 0xcd };
			const size_t copySizeExpected7 = min(sizeof(testBuffer7) - 2, srcBufferSize7);
			const byte *srcBufferExpected7 = srcBuffer7 + copySizeExpected7;
			const size_t srcBufferSizeExpected7 = srcBufferSize7 - copySizeExpected7;

			const byte testBuffer8[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer8[] = { 0xff };
			const size_t srcBufferSize8 = sizeof(srcBuffer8);
			const byte testBufferExpected8[] = { 0xcd, 0xff, 0xcd };
			const size_t copySizeExpected8 = min(sizeof(testBuffer8) - 2, srcBufferSize8);
			const byte *srcBufferExpected8 = srcBuffer8 + copySizeExpected8;
			const size_t srcBufferSizeExpected8 = srcBufferSize8 - copySizeExpected8;

			// Copy/Fill 2 bytes.
			const byte testBuffer9[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer9 = NULL;
			const size_t srcBufferSize9 = 0;
			const byte testBufferExpected9[] = { 0xcd, 0x00, 0x00, 0xcd };
			const size_t copySizeExpected9 = srcBuffer9 ? min(sizeof(testBuffer9) - 2, srcBufferSize9) : 0;
			const byte *srcBufferExpected9 = srcBuffer9 + copySizeExpected9;
			const size_t srcBufferSizeExpected9 = srcBuffer9 ? srcBufferSize9 - copySizeExpected9 : 0;

			const byte testBuffer10[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer10 = NULL;
			const size_t srcBufferSize10 = 100;
			const byte testBufferExpected10[] = { 0xcd, 0x00, 0x00, 0xcd };
			const size_t copySizeExpected10 = srcBuffer10 ? min(sizeof(testBuffer10) - 2, srcBufferSize10) : 0;
			const byte *srcBufferExpected10 = srcBuffer10 + copySizeExpected10;
			const size_t srcBufferSizeExpected10 = srcBuffer10 ? srcBufferSize10 - copySizeExpected10 : 0;

			const byte testBuffer11[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer11[] = { 0x01 };
			const size_t srcBufferSize11 = 0;
			const byte testBufferExpected11[] = { 0xcd, 0x00, 0x00, 0xcd };
			const size_t copySizeExpected11 = min(sizeof(testBuffer11) - 2, srcBufferSize11);
			const byte *srcBufferExpected11 = srcBuffer11 + copySizeExpected11;
			const size_t srcBufferSizeExpected11 = srcBufferSize11 - copySizeExpected11;

			const byte testBuffer12[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer12[] = { 0x01 };
			const size_t srcBufferSize12 = sizeof(srcBuffer12);
			const byte testBufferExpected12[] = { 0xcd, 0x01, 0x00, 0xcd };
			const size_t copySizeExpected12 = min(sizeof(testBuffer12) - 2, srcBufferSize12);
			const byte *srcBufferExpected12 = srcBuffer12 + copySizeExpected12;
			const size_t srcBufferSizeExpected12 = srcBufferSize12 - copySizeExpected12;

			const byte testBuffer13[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer13[] = { 0xff };
			const size_t srcBufferSize13 = sizeof(srcBuffer13);
			const byte testBufferExpected13[] = { 0xcd, 0xff, 0x00, 0xcd };
			const size_t copySizeExpected13 = min(sizeof(testBuffer13) - 2, srcBufferSize13);
			const byte *srcBufferExpected13 = srcBuffer13 + copySizeExpected13;
			const size_t srcBufferSizeExpected13 = srcBufferSize13 - copySizeExpected13;

			const byte testBuffer14[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer14[] = { 0x01, 0x02 };
			const size_t srcBufferSize14 = sizeof(srcBuffer14);
			const byte testBufferExpected14[] = { 0xcd, 0x01, 0x02, 0xcd };
			const size_t copySizeExpected14 = min(sizeof(testBuffer14) - 2, srcBufferSize14);
			const byte *srcBufferExpected14 = srcBuffer14 + copySizeExpected14;
			const size_t srcBufferSizeExpected14 = srcBufferSize14 - copySizeExpected14;

			const byte testBuffer15[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer15[] = { 0xff, 0xfe };
			const size_t srcBufferSize15 = sizeof(srcBuffer15);
			const byte testBufferExpected15[] = { 0xcd, 0xff, 0xfe, 0xcd };
			const size_t copySizeExpected15 = min(sizeof(testBuffer15) - 2, srcBufferSize15);
			const byte *srcBufferExpected15 = srcBuffer15 + copySizeExpected15;
			const size_t srcBufferSizeExpected15 = srcBufferSize15 - copySizeExpected15;

			const byte testBuffer16[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer16[] = { 0x01, 0x02, 0x03 };
			const size_t srcBufferSize16 = sizeof(srcBuffer16);
			const byte testBufferExpected16[] = { 0xcd, 0x01, 0x02, 0xcd };
			const size_t copySizeExpected16 = min(sizeof(testBuffer16) - 2, srcBufferSize16);
			const byte *srcBufferExpected16 = srcBuffer16 + copySizeExpected16;
			const size_t srcBufferSizeExpected16 = srcBufferSize16 - copySizeExpected16;

			const byte testBuffer17[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer17[] = { 0xff, 0xfe, 0xfd };
			const size_t srcBufferSize17 = sizeof(srcBuffer17);
			const byte testBufferExpected17[] = { 0xcd, 0xff, 0xfe, 0xcd };
			const size_t copySizeExpected17 = min(sizeof(testBuffer17) - 2, srcBufferSize17);
			const byte *srcBufferExpected17 = srcBuffer17 + copySizeExpected17;
			const size_t srcBufferSizeExpected17 = srcBufferSize17 - copySizeExpected17;

			const TestDataType TEST_DATA[] =
			{
				{ testBuffer0, sizeof(testBuffer0), 1, sizeof(testBuffer0) - 2, srcBuffer0, srcBufferSize0, testBufferExpected0,
					srcBufferExpected0, srcBufferSizeExpected0 },
				{ testBuffer1, sizeof(testBuffer1), 1, sizeof(testBuffer1) - 2, srcBuffer1, srcBufferSize1, testBufferExpected1,
					srcBufferExpected1, srcBufferSizeExpected1 },
				{ testBuffer2, sizeof(testBuffer2), 1, sizeof(testBuffer2) - 2, srcBuffer2, srcBufferSize2, testBufferExpected2,
					srcBufferExpected2, srcBufferSizeExpected2 },
				{ testBuffer3, sizeof(testBuffer3), 1, sizeof(testBuffer3) - 2, srcBuffer3, srcBufferSize3, testBufferExpected3,
					srcBufferExpected3, srcBufferSizeExpected3 },
				{ testBuffer4, sizeof(testBuffer4), 1, sizeof(testBuffer4) - 2, srcBuffer4, srcBufferSize4, testBufferExpected4,
					srcBufferExpected4, srcBufferSizeExpected4 },
				{ testBuffer5, sizeof(testBuffer5), 1, sizeof(testBuffer5) - 2, srcBuffer5, srcBufferSize5, testBufferExpected5,
					srcBufferExpected5, srcBufferSizeExpected5 },
				{ testBuffer6, sizeof(testBuffer6), 1, sizeof(testBuffer6) - 2, srcBuffer6, srcBufferSize6, testBufferExpected6,
					srcBufferExpected6, srcBufferSizeExpected6 },
				{ testBuffer7, sizeof(testBuffer7), 1, sizeof(testBuffer7) - 2, srcBuffer7, srcBufferSize7, testBufferExpected7,
					srcBufferExpected7, srcBufferSizeExpected7 },
				{ testBuffer8, sizeof(testBuffer8), 1, sizeof(testBuffer8) - 2, srcBuffer8, srcBufferSize8, testBufferExpected8,
					srcBufferExpected8, srcBufferSizeExpected8 },
				{ testBuffer9, sizeof(testBuffer9), 1, sizeof(testBuffer9) - 2, srcBuffer9, srcBufferSize9, testBufferExpected9,
					srcBufferExpected9, srcBufferSizeExpected9 },
				{ testBuffer10, sizeof(testBuffer10), 1, sizeof(testBuffer10) - 2, srcBuffer10, srcBufferSize10, testBufferExpected10,
					srcBufferExpected10, srcBufferSizeExpected10 },
				{ testBuffer11, sizeof(testBuffer11), 1, sizeof(testBuffer11) - 2, srcBuffer11, srcBufferSize11, testBufferExpected11,
					srcBufferExpected11, srcBufferSizeExpected11 },
				{ testBuffer12, sizeof(testBuffer12), 1, sizeof(testBuffer12) - 2, srcBuffer12, srcBufferSize12, testBufferExpected12,
					srcBufferExpected12, srcBufferSizeExpected12 },
				{ testBuffer13, sizeof(testBuffer13), 1, sizeof(testBuffer13) - 2, srcBuffer13, srcBufferSize13, testBufferExpected13,
					srcBufferExpected13, srcBufferSizeExpected13 },
				{ testBuffer14, sizeof(testBuffer14), 1, sizeof(testBuffer14) - 2, srcBuffer14, srcBufferSize14, testBufferExpected14,
					srcBufferExpected14, srcBufferSizeExpected14 },
				{ testBuffer15, sizeof(testBuffer15), 1, sizeof(testBuffer15) - 2, srcBuffer15, srcBufferSize15, testBufferExpected15,
					srcBufferExpected15, srcBufferSizeExpected15 },
				{ testBuffer16, sizeof(testBuffer16), 1, sizeof(testBuffer16) - 2, srcBuffer16, srcBufferSize16, testBufferExpected16,
					srcBufferExpected16, srcBufferSizeExpected16 },
				{ testBuffer17, sizeof(testBuffer17), 1, sizeof(testBuffer17) - 2, srcBuffer17, srcBufferSize17, testBufferExpected17,
					srcBufferExpected17, srcBufferSizeExpected17 },
			};

			const int TEST_DATA_MEMCMP_EXPECTED = MEMCMP_EQUAL;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				byte *testBuffer = new byte[TEST_DATA[testIndex].m_testBufferSize];
				memcpy_s(testBuffer,
					TEST_DATA[testIndex].m_testBufferSize,
					TEST_DATA[testIndex].m_pTestBuffer,
					TEST_DATA[testIndex].m_testBufferSize);

				// Perform operation(s) to be tested.
				void const *pSrcBuffer = TEST_DATA[testIndex].m_pSrcBuffer;
				size_t srcBufferSize = TEST_DATA[testIndex].m_srcBufferSize;
				CUtilities::MemCopyZeroIncSourcePtr(testBuffer + TEST_DATA[testIndex].m_destBufferOffset,
					TEST_DATA[testIndex].m_destBufferSize,
					pSrcBuffer,
					srcBufferSize);

				// Test #1.
				const int memcmpResult =
					memcmp(testBuffer, TEST_DATA[testIndex].m_pTestBufferExpected, TEST_DATA[testIndex].m_testBufferSize);

				{
					// Report #1.
					bool areEqual = memcmpResult == TEST_DATA_MEMCMP_EXPECTED;
					string failedMsg;
					failedMsg = FormatString("FAILED MemCopyZeroIncSourcePtr() memcmp testIndex=%d in %s",
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				{
					// Report #2.
					bool areEqual = pSrcBuffer == TEST_DATA[testIndex].m_pSrcBufferExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED MemCopyZeroIncSourcePtr() pSrcBuffer=%p expected=%p testIndex=%d in %s",
						(void *)pSrcBuffer,
						(void *)TEST_DATA[testIndex].m_pSrcBufferExpected,
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				{
					// Report #3.
					bool areEqual = srcBufferSize == TEST_DATA[testIndex].m_srcBufferSizeExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED MemCopyZeroIncSourcePtr() srcBufferSize=%u expected=%u testIndex=%d in %s",
						(unsigned)srcBufferSize,
						(unsigned)TEST_DATA[testIndex].m_srcBufferSizeExpected,
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
				delete testBuffer;
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CUtilities::MemCopyZeroIncSourcePtr(..., int &srcBufferLength).
		/// </summary>
		/// <remarks>
		/// Test with an extra check byte before and after destination buffer.
		/// </remarks>
		TEST(UtilitiesTests, MemCopyZeroIncSourcePtrIntTest)
		{
			typedef struct
			{
				// Inputs.
				const void *m_pTestBuffer;
				const int m_testBufferSize;
				const int m_destBufferOffset; // Offset into test buffer.
				const int m_destBufferSize;
				const void *m_pSrcBuffer;
				const int m_srcBufferLength;

				// Expected results.
				const void *m_pTestBufferExpected;
				const void *m_pSrcBufferExpected;
				const int m_srcBufferLengthExpected;
			} TestDataType;

			// Copy/Fill zero bytes.
			const byte testBuffer0[] = { 0xcd, 0xcd };
			const byte *srcBuffer0 = NULL;
			const int srcBufferLength0 = 0;
			const byte testBufferExpected0[] = { 0xcd, 0xcd };
			const int copyLengthExpected0 = srcBuffer0 ? min((int)sizeof(testBuffer0) - 2, max(0, srcBufferLength0)) : 0;
			const byte *srcBufferExpected0 = srcBuffer0 + copyLengthExpected0;
			const int srcBufferLengthExpected0 = srcBuffer0 && srcBufferLength0 > 0 ? srcBufferLength0 - copyLengthExpected0 : 0;

			const byte testBuffer1[] = { 0xcd, 0xcd };
			const byte *srcBuffer1 = NULL;
			const int srcBufferLength1 = 100;
			const byte testBufferExpected1[] = { 0xcd, 0xcd };
			const int copyLengthExpected1 = srcBuffer1 ? min((int)sizeof(testBuffer1) - 2, max(0, srcBufferLength1)) : 0;
			const byte *srcBufferExpected1 = srcBuffer1 + copyLengthExpected1;
			const int srcBufferLengthExpected1 = srcBuffer1 && srcBufferLength1 > 0 ? srcBufferLength1 - copyLengthExpected1 : 0;

			const byte testBuffer2[] = { 0xcd, 0xcd };
			const byte srcBuffer2[] = { 0x01 };
			const int srcBufferLength2 = 0;
			const byte testBufferExpected2[] = { 0xcd, 0xcd };
			const int copyLengthExpected2 = min((int)sizeof(testBuffer2) - 2, max(0, srcBufferLength2));
			const byte *srcBufferExpected2 = srcBuffer2 + copyLengthExpected2;
			const int srcBufferLengthExpected2 = srcBufferLength2 - copyLengthExpected2;

			const byte testBuffer3[] = { 0xcd, 0xcd };
			const byte srcBuffer3[] = { 0x01 };
			const int srcBufferLength3 = sizeof(srcBuffer3);
			const byte testBufferExpected3[] = { 0xcd, 0xcd };
			const int copyLengthExpected3 = min((int)sizeof(testBuffer3) - 2, max(0, srcBufferLength3));
			const byte *srcBufferExpected3 = srcBuffer3 + copyLengthExpected3;
			const int srcBufferLengthExpected3 = srcBufferLength3 - copyLengthExpected3;

			// Copy/Fill 1 byte.
			const byte testBuffer4[] = { 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer4 = NULL;
			const int srcBufferLength4 = 0;
			const byte testBufferExpected4[] = { 0xcd, 0x00, 0xcd };
			const int copyLengthExpected4 = srcBuffer4 ? min((int)sizeof(testBuffer4) - 2, max(0, srcBufferLength4)) : 0;
			const byte *srcBufferExpected4 = srcBuffer4 + copyLengthExpected4;
			const int srcBufferLengthExpected4 = srcBuffer4 && srcBufferLength4 > 0 ? srcBufferLength4 - copyLengthExpected4 : 0;

			const byte testBuffer5[] = { 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer5 = NULL;
			const int srcBufferLength5 = 100;
			const byte testBufferExpected5[] = { 0xcd, 0x00, 0xcd };
			const int copyLengthExpected5 = srcBuffer5 ? min((int)sizeof(testBuffer5) - 2, max(0, srcBufferLength5)) : 0;
			const byte *srcBufferExpected5 = srcBuffer5 + copyLengthExpected5;
			const int srcBufferLengthExpected5 = srcBuffer5 && srcBufferLength5 > 0 ? srcBufferLength5 - copyLengthExpected5 : 0;

			const byte testBuffer6[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer6[] = { 0x01 };
			const int srcBufferLength6 = 0;
			const byte testBufferExpected6[] = { 0xcd, 0x00, 0xcd };
			const int copyLengthExpected6 = min((int)sizeof(testBuffer6) - 2, max(0, srcBufferLength6));
			const byte *srcBufferExpected6 = srcBuffer6 + copyLengthExpected6;
			const int srcBufferLengthExpected6 = srcBufferLength6 - copyLengthExpected6;

			const byte testBuffer7[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer7[] = { 0x01 };
			const int srcBufferLength7 = sizeof(srcBuffer7);
			const byte testBufferExpected7[] = { 0xcd, 0x01, 0xcd };
			const int copyLengthExpected7 = min((int)sizeof(testBuffer7) - 2, max(0, srcBufferLength7));
			const byte *srcBufferExpected7 = srcBuffer7 + copyLengthExpected7;
			const int srcBufferLengthExpected7 = srcBufferLength7 - copyLengthExpected7;

			const byte testBuffer8[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer8[] = { 0xff };
			const int srcBufferLength8 = sizeof(srcBuffer8);
			const byte testBufferExpected8[] = { 0xcd, 0xff, 0xcd };
			const int copyLengthExpected8 = min((int)sizeof(testBuffer8) - 2, max(0, srcBufferLength8));
			const byte *srcBufferExpected8 = srcBuffer8 + copyLengthExpected8;
			const int srcBufferLengthExpected8 = srcBufferLength8 - copyLengthExpected8;

			// Copy/Fill 2 bytes.
			const byte testBuffer9[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer9 = NULL;
			const int srcBufferLength9 = 0;
			const byte testBufferExpected9[] = { 0xcd, 0x00, 0x00, 0xcd };
			const int copyLengthExpected9 = srcBuffer9 ? min((int)sizeof(testBuffer9) - 2, max(0, srcBufferLength9)) : 0;
			const byte *srcBufferExpected9 = srcBuffer9 + copyLengthExpected9;
			const int srcBufferLengthExpected9 = srcBuffer9 && srcBufferLength9 > 0 ? srcBufferLength9 - copyLengthExpected9 : 0;

			const byte testBuffer10[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer10 = NULL;
			const int srcBufferLength10 = 100;
			const byte testBufferExpected10[] = { 0xcd, 0x00, 0x00, 0xcd };
			const int copyLengthExpected10 = srcBuffer10 ? min((int)sizeof(testBuffer10) - 2, max(0, srcBufferLength10)) : 0;
			const byte *srcBufferExpected10 = srcBuffer10 + copyLengthExpected10;
			const int srcBufferLengthExpected10 = srcBuffer10 && srcBufferLength10 > 0 ? srcBufferLength10 - copyLengthExpected10 : 0;

			const byte testBuffer11[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer11[] = { 0x01 };
			const int srcBufferLength11 = 0;
			const byte testBufferExpected11[] = { 0xcd, 0x00, 0x00, 0xcd };
			const int copyLengthExpected11 = min((int)sizeof(testBuffer11) - 2, max(0, srcBufferLength11));
			const byte *srcBufferExpected11 = srcBuffer11 + copyLengthExpected11;
			const int srcBufferLengthExpected11 = srcBufferLength11 - copyLengthExpected11;

			const byte testBuffer12[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer12[] = { 0x01 };
			const int srcBufferLength12 = sizeof(srcBuffer12);
			const byte testBufferExpected12[] = { 0xcd, 0x01, 0x00, 0xcd };
			const int copyLengthExpected12 = min((int)sizeof(testBuffer12) - 2, max(0, srcBufferLength12));
			const byte *srcBufferExpected12 = srcBuffer12 + copyLengthExpected12;
			const int srcBufferLengthExpected12 = srcBufferLength12 - copyLengthExpected12;

			const byte testBuffer13[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer13[] = { 0xff };
			const int srcBufferLength13 = sizeof(srcBuffer13);
			const byte testBufferExpected13[] = { 0xcd, 0xff, 0x00, 0xcd };
			const int copyLengthExpected13 = min((int)sizeof(testBuffer13) - 2, max(0, srcBufferLength13));
			const byte *srcBufferExpected13 = srcBuffer13 + copyLengthExpected13;
			const int srcBufferLengthExpected13 = srcBufferLength13 - copyLengthExpected13;

			const byte testBuffer14[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer14[] = { 0x01, 0x02 };
			const int srcBufferLength14 = sizeof(srcBuffer14);
			const byte testBufferExpected14[] = { 0xcd, 0x01, 0x02, 0xcd };
			const int copyLengthExpected14 = min((int)sizeof(testBuffer14) - 2, max(0, srcBufferLength14));
			const byte *srcBufferExpected14 = srcBuffer14 + copyLengthExpected14;
			const int srcBufferLengthExpected14 = srcBufferLength14 - copyLengthExpected14;

			const byte testBuffer15[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer15[] = { 0xff, 0xfe };
			const int srcBufferLength15 = sizeof(srcBuffer15);
			const byte testBufferExpected15[] = { 0xcd, 0xff, 0xfe, 0xcd };
			const int copyLengthExpected15 = min((int)sizeof(testBuffer15) - 2, max(0, srcBufferLength15));
			const byte *srcBufferExpected15 = srcBuffer15 + copyLengthExpected15;
			const int srcBufferLengthExpected15 = srcBufferLength15 - copyLengthExpected15;

			const byte testBuffer16[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer16[] = { 0x01, 0x02, 0x03 };
			const int srcBufferLength16 = sizeof(srcBuffer16);
			const byte testBufferExpected16[] = { 0xcd, 0x01, 0x02, 0xcd };
			const int copyLengthExpected16 = min((int)sizeof(testBuffer16) - 2, max(0, srcBufferLength16));
			const byte *srcBufferExpected16 = srcBuffer16 + copyLengthExpected16;
			const int srcBufferLengthExpected16 = srcBufferLength16 - copyLengthExpected16;

			const byte testBuffer17[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer17[] = { 0xff, 0xfe, 0xfd };
			const int srcBufferLength17 = sizeof(srcBuffer17);
			const byte testBufferExpected17[] = { 0xcd, 0xff, 0xfe, 0xcd };
			const int copyLengthExpected17 = min((int)sizeof(testBuffer17) - 2, max(0, srcBufferLength17));
			const byte *srcBufferExpected17 = srcBuffer17 + copyLengthExpected17;
			const int srcBufferLengthExpected17 = srcBufferLength17 - copyLengthExpected17;

			// Copy/Fill zero bytes with negative source length.
			const byte testBuffer18[] = { 0xcd, 0xcd };
			const byte *srcBuffer18 = NULL;
			const int srcBufferLength18 = -33;
			const byte testBufferExpected18[] = { 0xcd, 0xcd };
			const int copyLengthExpected18 = srcBuffer18 ? min((int)sizeof(testBuffer18) - 2, max(0, srcBufferLength18)) : 0;
			const byte *srcBufferExpected18 = srcBuffer18 + copyLengthExpected18;
			const int srcBufferLengthExpected18 = srcBuffer18 && srcBufferLength18 > 0 ? srcBufferLength18 - copyLengthExpected18 : 0;

			const byte testBuffer19[] = { 0xcd, 0xcd };
			const byte srcBuffer19[] =  { 0x01 };
			const int srcBufferLength19 = -33;
			const byte testBufferExpected19[] = { 0xcd, 0xcd };
            //const int temp = sizeof(testBuffer19);
			const int copyLengthExpected19 = min((int)sizeof(testBuffer19) - 2, max(0, srcBufferLength19));
			const byte *srcBufferExpected19 = srcBuffer19 + copyLengthExpected19;
			const int srcBufferLengthExpected19 = srcBufferLength19 > 0 ? srcBufferLength19 - copyLengthExpected19 : 0;

			// Copy/Fill 1 byte with negative source length.
			const byte testBuffer20[] = { 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer20 = NULL;
			const int srcBufferLength20 = -33;
			const byte testBufferExpected20[] = { 0xcd, 0x00, 0xcd };
			const int copyLengthExpected20 = srcBuffer20 ? min((int)sizeof(testBuffer20) - 2, max(0, srcBufferLength20)) : 0;
			const byte *srcBufferExpected20 = srcBuffer20 + copyLengthExpected20;
			const int srcBufferLengthExpected20 = srcBuffer20 && srcBufferLength20 > 0 ? srcBufferLength20 - copyLengthExpected20 : 0;

			const byte testBuffer21[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer21[] =  { 0xff };
			const int srcBufferLength21 = -33;
			const byte testBufferExpected21[] = { 0xcd, 0x00, 0xcd };
			const int copyLengthExpected21 = min((int)sizeof(testBuffer21) - 2, max(0, srcBufferLength21));
			const byte *srcBufferExpected21 = srcBuffer21 + copyLengthExpected21;
			const int srcBufferLengthExpected21 = srcBufferLength21 > 0 ? srcBufferLength21 - copyLengthExpected21 : 0;

			const TestDataType TEST_DATA[] =
			{
				{ testBuffer0, sizeof(testBuffer0), 1, sizeof(testBuffer0) - 2, srcBuffer0, srcBufferLength0, testBufferExpected0,
					srcBufferExpected0, srcBufferLengthExpected0 },
				{ testBuffer1, sizeof(testBuffer1), 1, sizeof(testBuffer1) - 2, srcBuffer1, srcBufferLength1, testBufferExpected1,
					srcBufferExpected1, srcBufferLengthExpected1 },
				{ testBuffer2, sizeof(testBuffer2), 1, sizeof(testBuffer2) - 2, srcBuffer2, srcBufferLength2, testBufferExpected2,
					srcBufferExpected2, srcBufferLengthExpected2 },
				{ testBuffer3, sizeof(testBuffer3), 1, sizeof(testBuffer3) - 2, srcBuffer3, srcBufferLength3, testBufferExpected3,
					srcBufferExpected3, srcBufferLengthExpected3 },
				{ testBuffer4, sizeof(testBuffer4), 1, sizeof(testBuffer4) - 2, srcBuffer4, srcBufferLength4, testBufferExpected4,
					srcBufferExpected4, srcBufferLengthExpected4 },
				{ testBuffer5, sizeof(testBuffer5), 1, sizeof(testBuffer5) - 2, srcBuffer5, srcBufferLength5, testBufferExpected5,
					srcBufferExpected5, srcBufferLengthExpected5 },
				{ testBuffer6, sizeof(testBuffer6), 1, sizeof(testBuffer6) - 2, srcBuffer6, srcBufferLength6, testBufferExpected6,
					srcBufferExpected6, srcBufferLengthExpected6 },
				{ testBuffer7, sizeof(testBuffer7), 1, sizeof(testBuffer7) - 2, srcBuffer7, srcBufferLength7, testBufferExpected7,
					srcBufferExpected7, srcBufferLengthExpected7 },
				{ testBuffer8, sizeof(testBuffer8), 1, sizeof(testBuffer8) - 2, srcBuffer8, srcBufferLength8, testBufferExpected8,
					srcBufferExpected8, srcBufferLengthExpected8 },
				{ testBuffer9, sizeof(testBuffer9), 1, sizeof(testBuffer9) - 2, srcBuffer9, srcBufferLength9, testBufferExpected9,
					srcBufferExpected9, srcBufferLengthExpected9 },
				{ testBuffer10, sizeof(testBuffer10), 1, sizeof(testBuffer10) - 2, srcBuffer10, srcBufferLength10, testBufferExpected10,
					srcBufferExpected10, srcBufferLengthExpected10 },
				{ testBuffer11, sizeof(testBuffer11), 1, sizeof(testBuffer11) - 2, srcBuffer11, srcBufferLength11, testBufferExpected11,
					srcBufferExpected11, srcBufferLengthExpected11 },
				{ testBuffer12, sizeof(testBuffer12), 1, sizeof(testBuffer12) - 2, srcBuffer12, srcBufferLength12, testBufferExpected12,
					srcBufferExpected12, srcBufferLengthExpected12 },
				{ testBuffer13, sizeof(testBuffer13), 1, sizeof(testBuffer13) - 2, srcBuffer13, srcBufferLength13, testBufferExpected13,
					srcBufferExpected13, srcBufferLengthExpected13 },
				{ testBuffer14, sizeof(testBuffer14), 1, sizeof(testBuffer14) - 2, srcBuffer14, srcBufferLength14, testBufferExpected14,
					srcBufferExpected14, srcBufferLengthExpected14 },
				{ testBuffer15, sizeof(testBuffer15), 1, sizeof(testBuffer15) - 2, srcBuffer15, srcBufferLength15, testBufferExpected15,
					srcBufferExpected15, srcBufferLengthExpected15 },
				{ testBuffer16, sizeof(testBuffer16), 1, sizeof(testBuffer16) - 2, srcBuffer16, srcBufferLength16, testBufferExpected16,
					srcBufferExpected16, srcBufferLengthExpected16 },
				{ testBuffer17, sizeof(testBuffer17), 1, sizeof(testBuffer17) - 2, srcBuffer17, srcBufferLength17, testBufferExpected17,
					srcBufferExpected17, srcBufferLengthExpected17 },
				{ testBuffer18, sizeof(testBuffer18), 1, sizeof(testBuffer18) - 2, srcBuffer18, srcBufferLength18, testBufferExpected18,
					srcBufferExpected18, srcBufferLengthExpected18 },
				{ testBuffer19, sizeof(testBuffer19), 1, sizeof(testBuffer19) - 2, srcBuffer19, srcBufferLength19, testBufferExpected19,
					srcBufferExpected19, srcBufferLengthExpected19 },

                { testBuffer20, sizeof(testBuffer20), 1, sizeof(testBuffer20) - 2, srcBuffer20, srcBufferLength20, testBufferExpected20,
                    srcBufferExpected20, srcBufferLengthExpected20 },                                                                   
                { testBuffer21, sizeof(testBuffer21), 1, sizeof(testBuffer21) - 2, srcBuffer21, srcBufferLength21, testBufferExpected21,
                    srcBufferExpected21, srcBufferLengthExpected21 },                                                                   
			};

			const int TEST_DATA_MEMCMP_EXPECTED = MEMCMP_EQUAL;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				byte *testBuffer = new byte[TEST_DATA[testIndex].m_testBufferSize];
				memcpy_s(testBuffer,
					TEST_DATA[testIndex].m_testBufferSize,
					TEST_DATA[testIndex].m_pTestBuffer,
					TEST_DATA[testIndex].m_testBufferSize);

				// Perform operation(s) to be tested.
				void const *pSrcBuffer = TEST_DATA[testIndex].m_pSrcBuffer;
				int srcBufferLength = TEST_DATA[testIndex].m_srcBufferLength;
				CUtilities::MemCopyZeroIncSourcePtr(testBuffer + TEST_DATA[testIndex].m_destBufferOffset,
					TEST_DATA[testIndex].m_destBufferSize,
					pSrcBuffer,
					srcBufferLength);

				// Test #1.
				const int memcmpResult =
					memcmp(testBuffer, TEST_DATA[testIndex].m_pTestBufferExpected, TEST_DATA[testIndex].m_testBufferSize);

				{
					// Report #1.
					bool areEqual = memcmpResult == TEST_DATA_MEMCMP_EXPECTED;
					string failedMsg;
					failedMsg = FormatString("FAILED MemCopyZeroIncSourcePtr() memcmp testIndex=%d in %s",
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				{
					// Report #2.
					bool areEqual = pSrcBuffer == TEST_DATA[testIndex].m_pSrcBufferExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED MemCopyZeroIncSourcePtr() pSrcBuffer=%p expected=%p testIndex=%d in %s",
						(void *)pSrcBuffer,
						(void *)TEST_DATA[testIndex].m_pSrcBufferExpected,
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				{
					// Report #3.
					bool areEqual = srcBufferLength == TEST_DATA[testIndex].m_srcBufferLengthExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED MemCopyZeroIncSourcePtr() srcBufferLength=%d expected=%d testIndex=%d in %s",
						(unsigned)srcBufferLength,
						(unsigned)TEST_DATA[testIndex].m_srcBufferLengthExpected,
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
				delete testBuffer;
			}
		}

		// MemCopyZeroSizeTest
		// Test the implementation of:
		//     CUtilities::MemCopyZero(..., size_t srcBufferSize).
		// Test with an extra check byte before and after destination buffer.

		TEST(UtilitiesTests, MemCopyZeroSizeTest)
		{
			typedef struct
			{
				// Inputs.
				const void *m_pTestBuffer;
				const size_t m_testBufferSize;
				const size_t m_destBufferOffset; // Offset into test buffer.
				const size_t m_destBufferSize;
				const void *m_pSrcBuffer;
				const size_t m_srcBufferSize;

				// Expected results.
				const void *m_pTestBufferExpected;
			} TestDataType;

			// Copy/Fill zero bytes.
			const byte testBuffer0[] = { 0xcd, 0xcd };
			const byte *srcBuffer0 = NULL;
			const size_t srcBufferSize0 = 0;
			const byte testBufferExpected0[] = { 0xcd, 0xcd };

			const byte testBuffer1[] = { 0xcd, 0xcd };
			const byte *srcBuffer1 = NULL;
			const size_t srcBufferSize1 = 100;
			const byte testBufferExpected1[] = { 0xcd, 0xcd };

			const byte testBuffer2[] = { 0xcd, 0xcd };
			const byte srcBuffer2[] = { 0x01 };
			const size_t srcBufferSize2 = 0;
			const byte testBufferExpected2[] = { 0xcd, 0xcd };

			const byte testBuffer3[] = { 0xcd, 0xcd };
			const byte srcBuffer3[] = { 0x01 };
			const size_t srcBufferSize3 = sizeof(srcBuffer3);
			const byte testBufferExpected3[] = { 0xcd, 0xcd };

			// Copy/Fill 1 byte.
			const byte testBuffer4[] = { 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer4 = NULL;
			const size_t srcBufferSize4 = 0;
			const byte testBufferExpected4[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer5[] = { 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer5 = NULL;
			const size_t srcBufferSize5 = 100;
			const byte testBufferExpected5[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer6[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer6[] = { 0x01 };
			const size_t srcBufferSize6 = 0;
			const byte testBufferExpected6[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer7[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer7[] = { 0x01 };
			const size_t srcBufferSize7 = sizeof(srcBuffer7);
			const byte testBufferExpected7[] = { 0xcd, 0x01, 0xcd };

			const byte testBuffer8[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer8[] = { 0xff };
			const size_t srcBufferSize8 = sizeof(srcBuffer8);
			const byte testBufferExpected8[] = { 0xcd, 0xff, 0xcd };

			// Copy/Fill 2 bytes.
			const byte testBuffer9[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer9 = NULL;
			const size_t srcBufferSize9 = 0;
			const byte testBufferExpected9[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer10[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer10 = NULL;
			const size_t srcBufferSize10 = 100;
			const byte testBufferExpected10[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer11[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer11[] = { 0x01 };
			const size_t srcBufferSize11 = 0;
			const byte testBufferExpected11[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer12[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer12[] = { 0x01 };
			const size_t srcBufferSize12 = sizeof(srcBuffer12);
			const byte testBufferExpected12[] = { 0xcd, 0x01, 0x00, 0xcd };

			const byte testBuffer13[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer13[] = { 0xff };
			const size_t srcBufferSize13 = sizeof(srcBuffer13);
			const byte testBufferExpected13[] = { 0xcd, 0xff, 0x00, 0xcd };

			const byte testBuffer14[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer14[] = { 0x01, 0x02 };
			const size_t srcBufferSize14 = sizeof(srcBuffer14);
			const byte testBufferExpected14[] = { 0xcd, 0x01, 0x02, 0xcd };

			const byte testBuffer15[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer15[] = { 0xff, 0xfe };
			const size_t srcBufferSize15 = sizeof(srcBuffer15);
			const byte testBufferExpected15[] = { 0xcd, 0xff, 0xfe, 0xcd };

			const byte testBuffer16[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer16[] = { 0x01, 0x02, 0x03 };
			const size_t srcBufferSize16 = sizeof(srcBuffer16);
			const byte testBufferExpected16[] = { 0xcd, 0x01, 0x02, 0xcd };

			const byte testBuffer17[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer17[] = { 0xff, 0xfe, 0xfd };
			const size_t srcBufferSize17 = sizeof(srcBuffer17);
			const byte testBufferExpected17[] = { 0xcd, 0xff, 0xfe, 0xcd };

			const TestDataType TEST_DATA[] =
			{
				{ testBuffer0, sizeof(testBuffer0), 1, sizeof(testBuffer0) - 2, srcBuffer0, srcBufferSize0, testBufferExpected0 },
				{ testBuffer1, sizeof(testBuffer1), 1, sizeof(testBuffer1) - 2, srcBuffer1, srcBufferSize1, testBufferExpected1 },
				{ testBuffer2, sizeof(testBuffer2), 1, sizeof(testBuffer2) - 2, srcBuffer2, srcBufferSize2, testBufferExpected2 },
				{ testBuffer3, sizeof(testBuffer3), 1, sizeof(testBuffer3) - 2, srcBuffer3, srcBufferSize3, testBufferExpected3 },
				{ testBuffer4, sizeof(testBuffer4), 1, sizeof(testBuffer4) - 2, srcBuffer4, srcBufferSize4, testBufferExpected4 },
				{ testBuffer5, sizeof(testBuffer5), 1, sizeof(testBuffer5) - 2, srcBuffer5, srcBufferSize5, testBufferExpected5 },
				{ testBuffer6, sizeof(testBuffer6), 1, sizeof(testBuffer6) - 2, srcBuffer6, srcBufferSize6, testBufferExpected6 },
				{ testBuffer7, sizeof(testBuffer7), 1, sizeof(testBuffer7) - 2, srcBuffer7, srcBufferSize7, testBufferExpected7 },
				{ testBuffer8, sizeof(testBuffer8), 1, sizeof(testBuffer8) - 2, srcBuffer8, srcBufferSize8, testBufferExpected8 },
				{ testBuffer9, sizeof(testBuffer9), 1, sizeof(testBuffer9) - 2, srcBuffer9, srcBufferSize9, testBufferExpected9 },
				{ testBuffer10, sizeof(testBuffer10), 1, sizeof(testBuffer10) - 2, srcBuffer10, srcBufferSize10, testBufferExpected10 },
				{ testBuffer11, sizeof(testBuffer11), 1, sizeof(testBuffer11) - 2, srcBuffer11, srcBufferSize11, testBufferExpected11 },
				{ testBuffer12, sizeof(testBuffer12), 1, sizeof(testBuffer12) - 2, srcBuffer12, srcBufferSize12, testBufferExpected12 },
				{ testBuffer13, sizeof(testBuffer13), 1, sizeof(testBuffer13) - 2, srcBuffer13, srcBufferSize13, testBufferExpected13 },
				{ testBuffer14, sizeof(testBuffer14), 1, sizeof(testBuffer14) - 2, srcBuffer14, srcBufferSize14, testBufferExpected14 },
				{ testBuffer15, sizeof(testBuffer15), 1, sizeof(testBuffer15) - 2, srcBuffer15, srcBufferSize15, testBufferExpected15 },
				{ testBuffer16, sizeof(testBuffer16), 1, sizeof(testBuffer16) - 2, srcBuffer16, srcBufferSize16, testBufferExpected16 },
				{ testBuffer17, sizeof(testBuffer17), 1, sizeof(testBuffer17) - 2, srcBuffer17, srcBufferSize17, testBufferExpected17 },
			};

			const int TEST_DATA_MEMCMP_EXPECTED = MEMCMP_EQUAL;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				byte *testBuffer = new byte[TEST_DATA[testIndex].m_testBufferSize];
				memcpy_s(testBuffer,
					TEST_DATA[testIndex].m_testBufferSize,
					TEST_DATA[testIndex].m_pTestBuffer,
					TEST_DATA[testIndex].m_testBufferSize);

				// Perform operation(s) to be tested.
				CUtilities::MemCopyZero(testBuffer + TEST_DATA[testIndex].m_destBufferOffset,
					TEST_DATA[testIndex].m_destBufferSize,
					TEST_DATA[testIndex].m_pSrcBuffer,
					TEST_DATA[testIndex].m_srcBufferSize);

				// Test #1.
				const int memcmpResult =
					memcmp(testBuffer, TEST_DATA[testIndex].m_pTestBufferExpected, TEST_DATA[testIndex].m_testBufferSize);

				{
					// Report #1.
					bool areEqual = memcmpResult == TEST_DATA_MEMCMP_EXPECTED;
					string failedMsg;
					failedMsg = FormatString("FAILED MemCopyZero() test failed testIndex=%d in %s",
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
				delete testBuffer;
			}
		}

		// MemCopyZeroIntTest
		// Test the implementation of:
		//     CUtilities::MemCopyZero(..., int srcBufferLength).
		// Test with an extra check byte before and after destination buffer.
		// Each test with a srcBufferLength of zero is repeated with a negative length.
		TEST(UtilitiesTests, MemCopyZeroIntTest)
		{
			typedef struct
			{
				// Inputs.
				const void *m_pTestBuffer;
				const size_t m_testBufferSize;
				const size_t m_destBufferOffset; // Offset into test buffer.
				const size_t m_destBufferSize;
				const void *m_pSrcBuffer;
				const int m_srcBufferLength;

				// Expected results.
				const void *m_pTestBufferExpected;
			} TestDataType;

			// Copy/Fill zero bytes.
			const byte testBuffer0[] = { 0xcd, 0xcd };
			const byte *srcBuffer0 = NULL;
			const int srcBufferLength0 = 0;
			const byte testBufferExpected0[] = { 0xcd, 0xcd };

			const byte testBuffer1[] = { 0xcd, 0xcd };
			const byte *srcBuffer1 = NULL;
			const int srcBufferLength1 = -100;
			const byte testBufferExpected1[] = { 0xcd, 0xcd };

			const byte testBuffer2[] = { 0xcd, 0xcd };
			const byte *srcBuffer2 = NULL;
			const int srcBufferLength2 = 100;
			const byte testBufferExpected2[] = { 0xcd, 0xcd };

			const byte testBuffer3[] = { 0xcd, 0xcd };
			const byte srcBuffer3[] = { 0x01 };
			const int srcBufferLength3 = 0;
			const byte testBufferExpected3[] = { 0xcd, 0xcd };

			const byte testBuffer4[] = { 0xcd, 0xcd };
			const byte srcBuffer4[] = { 0x01 };
			const int srcBufferLength4 = -100;
			const byte testBufferExpected4[] = { 0xcd, 0xcd };

			const byte testBuffer5[] = { 0xcd, 0xcd };
			const byte srcBuffer5[] = { 0x01 };
			const int srcBufferLength5 = sizeof(srcBuffer5);
			const byte testBufferExpected5[] = { 0xcd, 0xcd };

			// Copy/Fill 1 byte.
			const byte testBuffer6[] = { 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer6 = NULL;
			const int srcBufferLength6 = 0;
			const byte testBufferExpected6[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer7[] = { 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer7 = NULL;
			const int srcBufferLength7 = -100;
			const byte testBufferExpected7[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer8[] = { 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer8 = NULL;
			const int srcBufferLength8 = 100;
			const byte testBufferExpected8[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer9[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer9[] = { 0x01 };
			const int srcBufferLength9 = 0;
			const byte testBufferExpected9[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer10[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer10[] = { 0x01 };
			const int srcBufferLength10 = -100;
			const byte testBufferExpected10[] = { 0xcd, 0x00, 0xcd };

			const byte testBuffer11[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer11[] = { 0x01 };
			const int srcBufferLength11 = sizeof(srcBuffer11);
			const byte testBufferExpected11[] = { 0xcd, 0x01, 0xcd };

			const byte testBuffer12[] = { 0xcd, 0xcd, 0xcd };
			const byte srcBuffer12[] = { 0xff };
			const int srcBufferLength12 = sizeof(srcBuffer12);
			const byte testBufferExpected12[] = { 0xcd, 0xff, 0xcd };

			// Copy/Fill 2 bytes.
			const byte testBuffer13[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer13 = NULL;
			const int srcBufferLength13 = 0;
			const byte testBufferExpected13[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer14[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer14 = NULL;
			const int srcBufferLength14 = -100;
			const byte testBufferExpected14[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer15[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte *srcBuffer15 = NULL;
			const int srcBufferLength15 = 100;
			const byte testBufferExpected15[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer16[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer16[] = { 0x01 };
			const int srcBufferLength16 = 0;
			const byte testBufferExpected16[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer17[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer17[] = { 0x01 };
			const int srcBufferLength17 = -100;
			const byte testBufferExpected17[] = { 0xcd, 0x00, 0x00, 0xcd };

			const byte testBuffer18[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer18[] = { 0x01 };
			const int srcBufferLength18 = sizeof(srcBuffer18);
			const byte testBufferExpected18[] = { 0xcd, 0x01, 0x00, 0xcd };

			const byte testBuffer19[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer19[] = { 0xff };
			const int srcBufferLength19 = sizeof(srcBuffer19);
			const byte testBufferExpected19[] = { 0xcd, 0xff, 0x00, 0xcd };

			const byte testBuffer20[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer20[] = { 0x01, 0x02 };
			const int srcBufferLength20 = sizeof(srcBuffer20);
			const byte testBufferExpected20[] = { 0xcd, 0x01, 0x02, 0xcd };

			const byte testBuffer21[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer21[] = { 0xff, 0xfe };
			const int srcBufferLength21 = sizeof(srcBuffer21);
			const byte testBufferExpected21[] = { 0xcd, 0xff, 0xfe, 0xcd };

			const byte testBuffer22[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer22[] = { 0x01, 0x02, 0x03 };
			const int srcBufferLength22 = sizeof(srcBuffer22);
			const byte testBufferExpected22[] = { 0xcd, 0x01, 0x02, 0xcd };

			const byte testBuffer23[] = { 0xcd, 0xcd, 0xcd, 0xcd };
			const byte srcBuffer23[] = { 0xff, 0xfe, 0xfd };
			const int srcBufferLength23 = sizeof(srcBuffer23);
			const byte testBufferExpected23[] = { 0xcd, 0xff, 0xfe, 0xcd };

			const TestDataType TEST_DATA[] =
			{
				{ testBuffer0, sizeof(testBuffer0), 1, sizeof(testBuffer0) - 2, srcBuffer0, srcBufferLength0, testBufferExpected0 },
				{ testBuffer1, sizeof(testBuffer1), 1, sizeof(testBuffer1) - 2, srcBuffer1, srcBufferLength1, testBufferExpected1 },
				{ testBuffer2, sizeof(testBuffer2), 1, sizeof(testBuffer2) - 2, srcBuffer2, srcBufferLength2, testBufferExpected2 },
				{ testBuffer3, sizeof(testBuffer3), 1, sizeof(testBuffer3) - 2, srcBuffer3, srcBufferLength3, testBufferExpected3 },
				{ testBuffer4, sizeof(testBuffer4), 1, sizeof(testBuffer4) - 2, srcBuffer4, srcBufferLength4, testBufferExpected4 },
				{ testBuffer5, sizeof(testBuffer5), 1, sizeof(testBuffer5) - 2, srcBuffer5, srcBufferLength5, testBufferExpected5 },
				{ testBuffer6, sizeof(testBuffer6), 1, sizeof(testBuffer6) - 2, srcBuffer6, srcBufferLength6, testBufferExpected6 },
				{ testBuffer7, sizeof(testBuffer7), 1, sizeof(testBuffer7) - 2, srcBuffer7, srcBufferLength7, testBufferExpected7 },
				{ testBuffer8, sizeof(testBuffer8), 1, sizeof(testBuffer8) - 2, srcBuffer8, srcBufferLength8, testBufferExpected8 },
				{ testBuffer9, sizeof(testBuffer9), 1, sizeof(testBuffer9) - 2, srcBuffer9, srcBufferLength9, testBufferExpected9 },
				{ testBuffer10, sizeof(testBuffer10), 1, sizeof(testBuffer10) - 2, srcBuffer10, srcBufferLength10, testBufferExpected10 },
				{ testBuffer11, sizeof(testBuffer11), 1, sizeof(testBuffer11) - 2, srcBuffer11, srcBufferLength11, testBufferExpected11 },
				{ testBuffer12, sizeof(testBuffer12), 1, sizeof(testBuffer12) - 2, srcBuffer12, srcBufferLength12, testBufferExpected12 },
				{ testBuffer13, sizeof(testBuffer13), 1, sizeof(testBuffer13) - 2, srcBuffer13, srcBufferLength13, testBufferExpected13 },
				{ testBuffer14, sizeof(testBuffer14), 1, sizeof(testBuffer14) - 2, srcBuffer14, srcBufferLength14, testBufferExpected14 },
				{ testBuffer15, sizeof(testBuffer15), 1, sizeof(testBuffer15) - 2, srcBuffer15, srcBufferLength15, testBufferExpected15 },
				{ testBuffer16, sizeof(testBuffer16), 1, sizeof(testBuffer16) - 2, srcBuffer16, srcBufferLength16, testBufferExpected16 },
				{ testBuffer17, sizeof(testBuffer17), 1, sizeof(testBuffer17) - 2, srcBuffer17, srcBufferLength17, testBufferExpected17 },
				{ testBuffer18, sizeof(testBuffer18), 1, sizeof(testBuffer18) - 2, srcBuffer18, srcBufferLength18, testBufferExpected18 },
				{ testBuffer19, sizeof(testBuffer19), 1, sizeof(testBuffer19) - 2, srcBuffer19, srcBufferLength19, testBufferExpected19 },
				{ testBuffer20, sizeof(testBuffer20), 1, sizeof(testBuffer20) - 2, srcBuffer20, srcBufferLength20, testBufferExpected20 },
				{ testBuffer21, sizeof(testBuffer21), 1, sizeof(testBuffer21) - 2, srcBuffer21, srcBufferLength21, testBufferExpected21 },
			};

			const int TEST_DATA_MEMCMP_EXPECTED = MEMCMP_EQUAL;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				byte *testBuffer = new byte[TEST_DATA[testIndex].m_testBufferSize];
				memcpy_s(testBuffer,
					TEST_DATA[testIndex].m_testBufferSize,
					TEST_DATA[testIndex].m_pTestBuffer,
					TEST_DATA[testIndex].m_testBufferSize);

				// Perform operation(s) to be tested.
				CUtilities::MemCopyZero(testBuffer + TEST_DATA[testIndex].m_destBufferOffset,
					TEST_DATA[testIndex].m_destBufferSize,
					TEST_DATA[testIndex].m_pSrcBuffer,
					TEST_DATA[testIndex].m_srcBufferLength);

				// Test #1.
				const int memcmpResult =
					memcmp(testBuffer, TEST_DATA[testIndex].m_pTestBufferExpected, TEST_DATA[testIndex].m_testBufferSize);

				{
					// Report #1.
					bool areEqual = memcmpResult == TEST_DATA_MEMCMP_EXPECTED;
					string failedMsg;
					failedMsg = FormatString("FAILED MemCopyZero() test failed testIndex=%d in %s",
						testIndex,
						(__TFUNCTION__));
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
				delete testBuffer;
			}
		}

		TEST(UtilitiesTests, ValidTimezoneOffset)
		{
			ASSERT_TRUE(CUtilities::IsTimezoneValid(0)) <<  L"0 minutes timezone offset is valid";
			ASSERT_TRUE(CUtilities::IsTimezoneValid(12 * 60)) <<  L"+12 hour timezone offset is valid";
			ASSERT_TRUE(CUtilities::IsTimezoneValid(-12 * 60)) <<  L"-12 hour timezone offset is valid";
			ASSERT_TRUE(CUtilities::IsTimezoneValid((+23 * 60) + 30)) <<  L"+23:30 timezone offset is valid";
			ASSERT_TRUE(CUtilities::IsTimezoneValid((-23 * 60) - 30)) <<  L"-23:30 timezone offset is valid";
			ASSERT_FALSE(CUtilities::IsTimezoneValid(24 * 60)) << "+24 hour timezone offset is NOT valid";
			ASSERT_FALSE(CUtilities::IsTimezoneValid(-24 * 60)) << "-24 hour timezone offset is NOT valid";
			ASSERT_FALSE(CUtilities::IsTimezoneValid(24 * 60)) << "+24 hour timezone offset is NOT valid";
			ASSERT_FALSE(CUtilities::IsTimezoneValid(-24 * 60)) << "-24 hour timezone offset is NOT valid";
			ASSERT_FALSE(CUtilities::IsTimezoneValid(30 * 60)) << "+30 hour timezone offset is NOT valid";
			ASSERT_FALSE(CUtilities::IsTimezoneValid(-32 * 60)) << "-32 hour timezone offset is NOT valid";
		}
}
