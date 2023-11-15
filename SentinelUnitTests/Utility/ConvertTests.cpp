#include "stdafx.h"
#include "TestUtil.h"
#include "AssertMsg.h"
#include "Util/Convert.h"

// Note: this file is saved as UTF-8, do not change this.


#include "gtest/gtest.h"

using namespace std;

namespace SentinelUnitTests
{	
		TEST(Util_ConvertTests, Util_Convert_WStrToUtf8_FailExample)
		{
			const wchar_t pStr[] = L"ぬるを";
			
			wstring strw(pStr);
            string stra(strw.begin(), strw.end());
  
            // Copy string to wstring.
            std::copy(stra.begin(), stra.end(), strw.begin());

            ASSERT_TRUE(wcscmp(pStr, strw.c_str()));
		}

		TEST(Util_ConvertTests, Util_Convert_WStrToUtf8_pStr)
		{
			const wchar_t pStr[] = L"ぬるを";
			
			string utf8str = Convert::WStrToUtf8(pStr);
			wstring utf16str = Convert::Utf8ToWStr(utf8str);

            ASSERT_FALSE(wcscmp(pStr, utf16str.c_str()));
		}
		
		TEST(Util_ConvertTests, Util_Convert_WStrToUtf8_nullptr)
		{
			string utf8str = Convert::WStrToUtf8(nullptr);
            ASSERT_FALSE(utf8str.compare(""));
		}

		TEST(Util_ConvertTests, Util_Convert_WStrToUtf8_zerolen)
		{
			string utf8str = Convert::WStrToUtf8(L"123", 0);
			ASSERT_FALSE(utf8str.compare(""));
		}

		TEST(Util_ConvertTests, Util_Convert_WStrToUtf8_wstring)
		{
			const wchar_t pStr[] = L"ぬるを";
			wstring orgstr(pStr);
			
			string utf8str = Convert::WStrToUtf8(orgstr);
			wstring utf16str = Convert::Utf8ToWStr(utf8str);

            ASSERT_FALSE(wcscmp(pStr, utf16str.c_str()));
		}

		TEST(Util_ConvertTests, Util_Convert_WStrToUtf8_pStr_length)
		{
			const wchar_t pStr[] = L"ぬるを";
			
			string utf8str = Convert::WStrToUtf8(pStr, 3);
			wstring utf16str = Convert::Utf8ToWStr(utf8str);

            ASSERT_FALSE(wcscmp(pStr, utf16str.c_str()));
		}

		TEST(Util_ConvertTests, Util_Convert_Utf8ToWStr_nullptr)
		{
			wstring utf16str = Convert::Utf8ToWStr(nullptr);
            ASSERT_FALSE(wcscmp(L"", utf16str.c_str()));			
		}

		TEST(Util_ConvertTests, Util_Convert_Utf8ToWStr_nullptr2)
		{
			wstring utf16str = Convert::Utf8ToWStr(nullptr, 5);
            ASSERT_FALSE(wcscmp(L"", utf16str.c_str()));
		}
		
		TEST(Util_ConvertTests, Util_Convert_ToUint64a)
		{
			struct
			{
				int line;
				const char* pStr;
				bool expectedReturn;
				uint64_t expectedResult;
			}
			static data[] =
			{
				{ __LINE__, "1",   true, 1 },
				{ __LINE__, "2",   true, 2 },
				{ __LINE__, "3",   true, 3 },
				{ __LINE__, "1.", false, 1 },
				{ __LINE__, "2.", false, 2 },
				{ __LINE__, "3.", false, 3 },
				{ __LINE__, "1844674407370955161", true, (uint64_t)(1844674407370955161) },
				{ __LINE__, " 4", true, 4  },
				{ __LINE__, nullptr, false, 0 }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				uint64_t result;
				bool rval = Convert::ToUint64(data[i].pStr, result);
				ASSERT_EQ(data[i].expectedReturn, rval) << AssertMsg::Format(data[i].line, L"return");
				if (rval)
				{
					ASSERT_EQ(data[i].expectedResult, result) << AssertMsg::Format(data[i].line, L"result");
				}
			}
		}
		
		TEST(Util_ConvertTests, Util_Convert_ToUint64b)
		{
			struct
			{
				int line;
				const char* pStr;
				bool expectedReturn;
				uint64_t expectedResult;
				int expectedOffset;
			}
			static data[] =
			{
				{ __LINE__, "",    false,  0, 0 },
				{ __LINE__, "1",    true,  1, 1 },
				{ __LINE__, "2.",  false,  2, 1 },
				{ __LINE__, "31.", false, 31, 2 },
				{ __LINE__, " 4",   true,  4, 2 },
				{ __LINE__, "1844674407370955161", true, (uint64_t)(1844674407370955161), 19 },
				{ __LINE__, nullptr, false, 0, 0 }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				const char* pEnd = nullptr;
				uint64_t result;
				bool rval = Convert::ToUint64(data[i].pStr, result, &pEnd);
				ASSERT_EQ(data[i].expectedReturn, rval) << AssertMsg::Format(data[i].line, L"return");
				ASSERT_EQ((uint64_t)(data[i].expectedResult), result) << AssertMsg::Format(data[i].line, L"result");
				int offset = pEnd - data[i].pStr;
				ASSERT_EQ(data[i].expectedOffset, offset) << AssertMsg::Format(data[i].line, L"offset");
			}
		}

		TEST(Util_ConvertTests, Util_Convert_ToInt64)
		{
			struct
			{
				int line;
				const char* pStr;
				bool expectedReturn;
				int64_t expectedResult;
			}
			static data[] =
			{
				{ __LINE__, "9223372036854775807",   true,  (int64_t)(9223372036854775807) },
				{ __LINE__, " 9223372036854775807",  true,  (int64_t)(9223372036854775807) },
				{ __LINE__, "-922337203685477580",  true, (int64_t)(-922337203685477580) },
				{ __LINE__, " 0"	,  true, 0 },
				{ __LINE__, nullptr	, false, 0 }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				int64_t result;
				bool rval = Convert::ToInt64(data[i].pStr, result);
				ASSERT_EQ(data[i].expectedReturn, rval) << AssertMsg::Format(data[i].line, L"return");
				ASSERT_TRUE(data[i].expectedResult == result) << AssertMsg::Format(data[i].line, L"result", L"%lld != %lld", data[i].expectedResult, result);
			}
		}

		TEST(Util_ConvertTests, Util_Convert_ToUint32a)
		{
			struct
			{
				int line;
				const char* pStr;
				bool expectedReturn;
				unsigned expectedResult;
			}
			static data[] =
			{
				{ __LINE__, "1"   , true , 1 },
				{ __LINE__, "2"   , true , 2 },
				{ __LINE__, "3"   , true , 3 },
				{ __LINE__, " 3"  , true , 3 },
				{ __LINE__, "1."  , false, 1 },
				{ __LINE__, "2."  , false, 2 },
				{ __LINE__, "3."  , false, 3 },
				{ __LINE__, " 3." , false, 3 },
				{ __LINE__, "4294967295", true, 4294967295ul },
				{ __LINE__, nullptr, false, 0 }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				unsigned result;
				bool rval = Convert::ToUint32(data[i].pStr, result);
				ASSERT_EQ(data[i].expectedReturn, rval) << AssertMsg::Format(data[i].line) << "return";
				ASSERT_EQ(data[i].expectedResult, result) << AssertMsg::Format(data[i].line) << "result";
			}
		}

		TEST(Util_ConvertTests, Util_Convert_ToUint32b)
		{
			struct
			{
				int line;
				const char* pStr;
				bool expectedReturn;
				unsigned expectedResult;
			}
			static data[] =
			{
				{ __LINE__, "1",   true, 1 },
				{ __LINE__, "2",   true, 2 },
				{ __LINE__, "3",   true, 3 },
				{ __LINE__, "1.", false, 1 },
				{ __LINE__, "2.", false, 2 },
				{ __LINE__, "3.", false, 3 },
				{ __LINE__, "4294967295", true, 4294967295u }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				unsigned result;
				bool rval = Convert::ToUint32(data[i].pStr, result);
				ASSERT_EQ(data[i].expectedReturn, rval) << AssertMsg::Format(data[i].line) << "return";
				ASSERT_EQ(data[i].expectedResult, result) << AssertMsg::Format(data[i].line) << "result";
			}
		}
		
		TEST(Util_ConvertTests, Util_Convert_ToUint32c)
		{
			struct
			{
				int line;
				const char* pStr;
				bool expectedReturn;
				unsigned expectedResult;
				int expectedOffset;
			}
			static data[] =
			{
				{ __LINE__, "1",    true, 1,  1 },
				{ __LINE__, "2.",  false, 2,  1 },
				{ __LINE__, "",    false, 0,  0 },
				{ __LINE__, "31.", false, 31, 2 },
				{ __LINE__, "4294967295", true, 4294967295u, 10 }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				const char* pEnd = nullptr;
				unsigned result;
				bool rval = Convert::ToUint32(data[i].pStr, result, &pEnd);
				ASSERT_EQ(data[i].expectedReturn, rval) << AssertMsg::Format(data[i].line) << "return";
				ASSERT_EQ(data[i].expectedResult, result) << AssertMsg::Format(data[i].line) << "result";
				int offset = pEnd - data[i].pStr;
				ASSERT_EQ(data[i].expectedOffset, offset) << AssertMsg::Format(data[i].line) << "offset";
			}
		}

		TEST(Util_ConvertTests, Util_Convert_ToInt32)
		{
			struct
			{
				int line;
				const char* pStr;
				bool expectedReturn;
				int expectedResult;
			}
			static data[] =
			{
				{ __LINE__, "2147483647", true, 2147483647 },
				{ __LINE__, " 2147483647", true, 2147483647 },
				{ __LINE__, "-2147483248", true, -2147483248 },
				{ __LINE__, " -2147483248", true, -2147483248 },
				{ __LINE__, " 0", true, 0 },
				{ __LINE__, nullptr, false, 0 }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				int result;
				bool rval = Convert::ToInt32(data[i].pStr, result);
				ASSERT_EQ(data[i].expectedReturn, rval) << AssertMsg::Format(data[i].line) << "return";
				ASSERT_EQ(data[i].expectedResult, result) << AssertMsg::Format(data[i].line) << "result";
			}
		}
}
