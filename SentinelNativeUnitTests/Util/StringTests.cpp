#if 000//TODO-PORT
#include "stdafx.h"
#include "TestUtil\AssertMsg.h"
#include <Util\String.h>
#include <Util\Convert.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SentinelNativeUnitTests
{
	struct
	{
		int line;
		const char* pStr;
		const char* pEnd;
		bool ignoreCase;
		bool expected;
	}

	struct
	{
		int line;
		uint64_t val;
		bool useUpperCase;
		int padLen;
		const char* pExpected;
	}
	static hexData[] =
	{
		{ __LINE__, 0x1a, false, 27, "000000000000001a" },

		{ __LINE__, 0, false,  0, "0" },
		{ __LINE__, 0, false,  1, "0" },
		{ __LINE__, 0, false,  2, "00" },
		{ __LINE__, 0, false, 16, "0000000000000000" },
		{ __LINE__, 0, false, 27, "0000000000000000" },
		{ __LINE__, 0,  true,  0, "0" },
		{ __LINE__, 0,  true, 16, "0000000000000000" },
		{ __LINE__, 0,  true, 27, "0000000000000000" },
		{ __LINE__, 0x1a, false,  0, "1a" },
		{ __LINE__, 0x1a, false,  2, "1a" },
		{ __LINE__, 0x1a, false,  3, "01a" },
		{ __LINE__, 0x1a, false, 16, "000000000000001a" },
		{ __LINE__, 0x1a, false, 27, "000000000000001a" },
		{ __LINE__, 0x1a,  true,  0, "1A" },
		{ __LINE__, 0x1a,  true,  1, "1A" },
		{ __LINE__, 0x1a,  true,  3, "01A" },
		{ __LINE__, 0x1a,  true, 16, "000000000000001A" },
		{ __LINE__, 0x1a,  true, 27, "000000000000001A" },
		{ __LINE__, 0x123456789abcdef0, false,  0, "123456789abcdef0" },
		{ __LINE__, 0x123456789abcdef0, false,  2, "123456789abcdef0" },
		{ __LINE__, 0x123456789abcdef0, false,  3, "123456789abcdef0" },
		{ __LINE__, 0x123456789abcdef0, false, 16, "123456789abcdef0" },
		{ __LINE__, 0x123456789abcdef0, false, 27, "123456789abcdef0" },
		{ __LINE__, 0x123456789abcdef0,  true,  0, "123456789ABCDEF0" },
		{ __LINE__, 0x123456789abcdef0,  true,  1, "123456789ABCDEF0" },
		{ __LINE__, 0x123456789abcdef0,  true,  3, "123456789ABCDEF0" },
		{ __LINE__, 0x123456789abcdef0,  true, 16, "123456789ABCDEF0" },
		{ __LINE__, 0x123456789abcdef0,  true, 27, "123456789ABCDEF0" }
	};

	TEST_CLASS(Util_StringTests)
	{
	public:
		TEST_METHOD_CLEANUP(Util_StringTests_Cleanup)
		{
		}

		TEST_METHOD(Util_String_RemoveAll)
		{
			struct
			{
				int line;
				const char* pStr;
				const char* pFind;
				const char* pExpected;
			}
			data[] =
			{
				{ __LINE__, "123abc456ab789abcabc", "abc"  , "123456ab789" },
				{ __LINE__, ""					  , "abc"  , ""            },
				{ __LINE__, "12ab12"			  , ""     , "12ab12"      },
				{ __LINE__, "12ab12"			  , nullptr, "12ab12"      }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string str(data[i].pStr);
				string& result = String::RemoveAll(str, data[i].pFind);
				Assert::AreEqual(data[i].pExpected, result.c_str(), AssertMsg::Format(data[i].line, L"result"));
				Assert::AreSame(str, result, AssertMsg::Format(data[i].line, L"result"));
			}
		}

		TEST_METHOD(Util_String_ReplaceAll)
		{
			struct
			{
				int line;
				const char* pStr;
				const char* pFind;
				const char* pReplace;
				const char* pExpected;
			}
			data[] =
			{
				{ __LINE__, "123abc456ab789abcabc", "abc"  , "xyz"  , "123xyz456ab789xyzxyz" },
				{ __LINE__, ""					  , "abc"  , "xyz"  , ""              },
				{ __LINE__, "12ab12"			  , ""     , "xyz"  , "12ab12"        },
				{ __LINE__, "12ab12"			  , nullptr, "xyz"  , "12ab12"        },
				{ __LINE__, "123abc456ab789abcabc", "abc"  , nullptr, "123456ab789"   },
				{ __LINE__, ""					  , "abc"  , nullptr, ""              },
				{ __LINE__, "12ab12"			  , ""     , nullptr, "12ab12"        },
				{ __LINE__, "12ab12"			  , nullptr, nullptr, "12ab12"        },
				{ __LINE__, "123abc456ab789abcabc", "abc"  , ""     , "123456ab789"   },
				{ __LINE__, ""					  , "abc"  , ""     , ""              },
				{ __LINE__, "12ab12"			  , ""     , ""     , "12ab12"        },
				{ __LINE__, "12ab12"			  , nullptr, ""     , "12ab12"        }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string str(data[i].pStr);
				string& result = String::ReplaceAll(str, data[i].pFind, data[i].pReplace);
				Assert::AreEqual(data[i].pExpected, result.c_str(), AssertMsg::Format(data[i].line, L"result"));
				Assert::AreSame(str, result, AssertMsg::Format(data[i].line, L"result"));
			}
		}

		TEST_METHOD(Util_String_AppendHex)
		{
			for (size_t i = 0; i < _countof(hexData); i++)
			{
				string str = "0x";
				string& retStr = String::AppendHex(hexData[i].val, str, hexData[i].useUpperCase, hexData[i].padLen);
				
				string expectedStr("0x");
				expectedStr += hexData[i].pExpected;

				Assert::AreEqual(expectedStr.c_str(), str.c_str(), AssertMsg::Format(hexData[i].line, nullptr));
				Assert::AreEqual(expectedStr.c_str(), retStr.c_str(), AssertMsg::Format(hexData[i].line, nullptr));
			}
		}

		TEST_METHOD(Util_String_Hex)
		{
			struct
			{
				int line;
				uint64_t val;
				bool useUpperCase;
				int padLen;
				const char* pResult;
			}
			data[] =
			{
				{ __LINE__, 0,					 true,       0,                "0" },
				{ __LINE__, 0,					false,      -1,                "0" },
				{ __LINE__, 0,					false,       3,              "000" },
				{ __LINE__, 0,					false, INT_MAX, "0000000000000000" },
				{ __LINE__, 0xfedcba9876543210, false, INT_MAX, "fedcba9876543210" },
				{ __LINE__, 0xfedcba9876543210,  true, INT_MAX, "FEDCBA9876543210" },
				{ __LINE__, 1234,		  	    false,       5,            "004d2" },
				{ __LINE__, 1234,		  	    false,       4,             "04d2" },
				{ __LINE__, 1234,		  	    false,       3,              "4d2" },
				{ __LINE__, 1234,		  	    false,       2,              "4d2" },
				{ __LINE__, 1234,		  	    false,       1,              "4d2" },
				{ __LINE__, 1234,		  	    false,       0,              "4d2" }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string s = String::ToHex(data[i].val, data[i].useUpperCase, data[i].padLen);
				Assert::AreEqual(data[i].pResult, s.c_str(), AssertMsg::Format(data[i].line, nullptr));
			}
		}

		TEST_METHOD(Util_String_BinToHex_true_0)
		{
			uint8_t data[4] = { 0x1F, 0x2E, 0x0C, 0x9A };

			string str = String::BinToHex(data, sizeof(data), true, 0);
			Assert::AreEqual("1F2E0C9A", str.c_str());
		}

		TEST_METHOD(Util_String_BinToHex_false_0)
		{
			uint8_t data[4] = { 0x1F, 0x2E, 0x0C, 0x9A };

			string str = String::BinToHex(data, sizeof(data), false, 0);
			Assert::AreEqual("1f2e0c9a", str.c_str());
		}

		TEST_METHOD(Util_String_BinToHex_false_space)
		{
			uint8_t data[4] = { 0x1F, 0x2E, 0x0C, 0x9A };

			string str = String::BinToHex(data, sizeof(data), false, ' ');
			Assert::AreEqual("1f 2e 0c 9a", str.c_str());
		}

		TEST_METHOD(Util_String_BinToHex_tobuffer)
		{
			struct
			{
				int line;
				const uint8_t data[3];
				size_t dataLen;
				size_t bufferLen;
				bool useUpperCase;
				char separator;
				const char* pExpected;
			}
			data[] =
			{
				{ __LINE__, { 0x00, 0x00, 0x00 }, 0, 12, false,   0, ""         },
				{ __LINE__, { 0x9a, 0xf1, 0x6d }, 3, 12, false,   0, "9af16d"   },
				{ __LINE__, { 0x9a, 0xf1, 0x6d }, 3, 12,  true,   0, "9AF16D"   },
				{ __LINE__, { 0x9a, 0xf1, 0x6d }, 3, 12, false, '.', "9a.f1.6d" },
				{ __LINE__, { 0x9a, 0xf1, 0x6d }, 3, 12,  true, '.', "9A.F1.6D" },
				{ __LINE__, { 0x9a, 0xf1, 0x6d }, 3,  6, false,   0, "9af16"    },
				{ __LINE__, { 0x9a, 0xf1, 0x6d }, 3,  6,  true,   0, "9AF16"    },
				{ __LINE__, { 0x9a, 0xf1, 0x6d }, 3,  7, false, '.', "9a.f1."   },
				{ __LINE__, { 0x9a, 0xf1, 0x6d }, 3,  7,  true, '.', "9A.F1."   }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				char buffer[12];
				memset(buffer, 'x', sizeof(buffer));
				char* pBuffer = String::BinToHex(data[i].data, data[i].dataLen, buffer, data[i].bufferLen, data[i].useUpperCase, data[i].separator);
				Assert::AreEqual(data[i].pExpected, buffer, AssertMsg::Format(data[i].line, L"buffer"));
				Assert::AreEqual(data[i].pExpected, pBuffer, AssertMsg::Format(data[i].line, L"pBuffer"));
			}
		}

		TEST_METHOD(Util_String_SafeCopy)
		{
			char buffer[5];
			memset(buffer, 'z', _countof(buffer));
			size_t len = String::SafeCopy(buffer, "12", 4, false);
			Assert::AreEqual((size_t) 2, len, L"return");
			Assert::AreEqual("12", buffer, L"buffer");
			Assert::IsTrue(memcmp("12\000zz", buffer, sizeof(buffer)) == 0, L"buffer contents");
		}

		TEST_METHOD(Util_String_SafeCopy_srcnullptr)
		{
			char buffer[5];
			memset(buffer, 'z', _countof(buffer));
			size_t len = String::SafeCopy(buffer, nullptr, 4, false);
			Assert::AreEqual((size_t) 0, len, L"return");
			Assert::AreEqual("", buffer, L"buffer");
			Assert::IsTrue(memcmp("\000zzzz", buffer, sizeof(buffer)) == 0, L"buffer contents");
		}

		TEST_METHOD(Util_String_SafeCopy_dstnullptr)
		{
			char buffer[5];
			memset(buffer, 'z', _countof(buffer));
			size_t len = String::SafeCopy(nullptr, "12", 4, false);
			Assert::AreEqual((size_t) 0, len, L"return");
			Assert::IsTrue(memcmp("zzzzz", buffer, sizeof(buffer)) == 0, L"buffer contents");
		}

		TEST_METHOD(Util_String_SafeCopy_srcdstnullptr)
		{
			char buffer[5];
			memset(buffer, 'z', _countof(buffer));
			size_t len = String::SafeCopy((char*) nullptr, nullptr, 4, false);
			Assert::AreEqual((size_t) 0, len, L"return");
			Assert::IsTrue(memcmp("zzzzz", buffer, sizeof(buffer)) == 0, L"buffer contents");
		}

		TEST_METHOD(Util_String_SafeCopy_fill)
		{
			char buffer[5];
			memset(buffer, 'z', _countof(buffer));
			size_t len = String::SafeCopy(buffer, "12", 4, true);
			Assert::AreEqual((size_t) 2, len, L"return");
			Assert::AreEqual("12", buffer, L"buffer");
			Assert::IsTrue(memcmp("12\000\000z", buffer, sizeof(buffer)) == 0, L"buffer contents");
		}

		TEST_METHOD(Util_String_SafeCopy_toolong)
		{
			char buffer[5];
			memset(buffer, 'z', _countof(buffer));
			size_t len = String::SafeCopy(buffer, "123456", 4, false);
			Assert::AreEqual((size_t) 3, len, L"return");
			Assert::AreEqual("123", buffer, L"buffer");
			Assert::IsTrue(memcmp("123\000z", buffer, sizeof(buffer)) == 0, L"buffer contents");
		}

		TEST_METHOD(Util_String_SafeCopy_toolong_err)
		{
			char buffer[5];
			memset(buffer, 'z', _countof(buffer));

			bool err = false;
			size_t len = String::SafeCopy(buffer, "123456", 4, false, &err);
			Assert::AreEqual((size_t) 3, len, L"return");
			Assert::AreEqual("123", buffer, L"buffer");
			Assert::IsTrue(memcmp("123\000z", buffer, sizeof(buffer)) == 0, L"buffer contents");
			Assert::IsTrue(err, L"err");
		}

		TEST_METHOD(Util_String_SafeCopy_wchar)
		{
			wchar_t buffer[5] = { L'z',  L'z',  L'z',  L'z',  L'z' };
			size_t len = String::SafeCopy(buffer, L"12", 4, false);
			Assert::AreEqual((size_t) 2, len, L"return");
			Assert::AreEqual(L"12", buffer, L"buffer");
			Assert::IsTrue(memcmp(L"12\000zz", buffer, sizeof(buffer)) == 0, L"buffer contents");
		}

		TEST_METHOD(Util_String_SafeCopy_wchar_srcnullptr)
		{
			wchar_t buffer[5] = { L'z',  L'z',  L'z',  L'z',  L'z' };
			size_t len = String::SafeCopy(buffer, nullptr, 4, false);
			Assert::AreEqual((size_t) 0, len, L"return");
			Assert::AreEqual(L"", buffer, L"buffer");
			Assert::IsTrue(memcmp(L"\000zzzz", buffer, sizeof(buffer)) == 0, L"buffer contents");
		}

		TEST_METHOD(Util_String_SafeCopy_wchar_dstnullptr)
		{
			wchar_t buffer[5] = { L'z',  L'z',  L'z',  L'z',  L'z' };
			size_t len = String::SafeCopy(nullptr, L"12", 4, false);
			Assert::AreEqual((size_t) 0, len, L"return");
			Assert::IsTrue(memcmp(L"zzzzz", buffer, sizeof(buffer)) == 0, L"buffer contents");
		}

		TEST_METHOD(Util_String_SafeCopy_wchar_srcdstnullptr)
		{
			wchar_t buffer[5] = { L'z',  L'z',  L'z',  L'z',  L'z' };
			size_t len = String::SafeCopy((wchar_t*) nullptr, nullptr, 4, false);
			Assert::AreEqual((size_t) 0, len, L"return");
			Assert::IsTrue(memcmp(L"zzzzz", buffer, sizeof(buffer)) == 0, L"buffer contents");
		}

		TEST_METHOD(Util_String_SafeCopy_wchar_fill)
		{
			wchar_t buffer[5] = { L'z',  L'z',  L'z',  L'z',  L'z' };
			size_t len = String::SafeCopy(buffer, L"12", 4, true);
			Assert::AreEqual((size_t) 2, len, L"return");
			Assert::AreEqual(L"12", buffer, L"buffer");
			Assert::IsTrue(memcmp(L"12\000\000z", buffer, sizeof(buffer)) == 0, L"buffer contents");
		}

		TEST_METHOD(Util_String_SafeCopy_wchar_toolong)
		{
			wchar_t buffer[5] = { L'z',  L'z',  L'z',  L'z',  L'z' };
			size_t len = String::SafeCopy(buffer, L"123456", 4, false);
			Assert::AreEqual((size_t) 3, len, L"return");
			Assert::AreEqual(L"123", buffer, L"buffer");
			Assert::IsTrue(memcmp(L"123\000z", buffer, sizeof(buffer)) == 0, L"buffer contents");
		}

		TEST_METHOD(Util_String_SafeCopy_wchar_toolong_err)
		{
			wchar_t buffer[5] = { L'z',  L'z',  L'z',  L'z',  L'z' };

			bool err = false;
			size_t len = String::SafeCopy(buffer, L"123456", 4, false, &err);
			Assert::AreEqual((size_t) 3, len, L"return");
			Assert::AreEqual(L"123", buffer, L"buffer");
			Assert::IsTrue(memcmp(L"123\000z", buffer, sizeof(buffer)) == 0, L"buffer contents");
			Assert::IsTrue(err, L"err");
		}

		TEST_METHOD(Util_String_Concat_char_char)
		{
			struct
			{
				int line;
				const char* pStr1;
				const char* pStr2;
				const char* pResult;
			}
			data[] =
			{
				{ __LINE__, nullptr, nullptr, ""       },
				{ __LINE__, nullptr, "",	  ""       },
				{ __LINE__, "",		 nullptr, ""       },
				{ __LINE__, "",		 "",	  ""       },
				{ __LINE__, nullptr, "123",	  "123"    },
				{ __LINE__, "abc",	 nullptr, "abc"    },
				{ __LINE__, "abc",	 "123",   "abc123" }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string result = String::Concat(data[i].pStr1, data[i].pStr2);
				Assert::AreEqual(data[i].pResult, result.c_str(), AssertMsg::Format(data[i].line));
			}
		}
		
		TEST_METHOD(Util_String_Concat_char_wchar)
		{
			struct
			{
				int line;
				const char* pStr1;
				const wchar_t* pStr2;
				const char* pResult;
			}
			data[] =
			{
				{ __LINE__, nullptr, nullptr, "" },
				{ __LINE__, nullptr, L"",	  "" },
				{ __LINE__, "",		 nullptr, "" },
				{ __LINE__, "",		 L"",	  "" },
				{ __LINE__, nullptr, L"123",  "123" },
				{ __LINE__, "abc",	 nullptr, "abc" },
				{ __LINE__, "abc",	 L"123",  "abc123" }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string result = String::Concat(data[i].pStr1, data[i].pStr2);
				Assert::AreEqual(data[i].pResult, result.c_str(), AssertMsg::Format(data[i].line));
			}
		}
		
		TEST_METHOD(Util_String_Concat_wchar_char)
		{
			struct
			{
				int line;
				const wchar_t* pStr1;
				const char* pStr2;
				const char* pResult;
			}
			data[] =
			{
				{ __LINE__, nullptr, nullptr, "" },
				{ __LINE__, nullptr, "",	  "" },
				{ __LINE__, L"",	 nullptr, "" },
				{ __LINE__, L"",	 "",	  "" },
				{ __LINE__, nullptr, "123",   "123" },
				{ __LINE__, L"abc",	 nullptr, "abc" },
				{ __LINE__, L"abc",	 "123",   "abc123" }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string result = String::Concat(data[i].pStr1, data[i].pStr2);
				Assert::AreEqual(data[i].pResult, result.c_str(), AssertMsg::Format(data[i].line));
			}
		}

		TEST_METHOD(Util_String_Concat_wchar_wchar)
		{
			struct
			{
				int line;
				const wchar_t* pStr1;
				const wchar_t* pStr2;
				const char* pResult;
			}
			data[] =
			{
				{ __LINE__, nullptr, nullptr, "" },
				{ __LINE__, nullptr, L"",	  "" },
				{ __LINE__, L"",	 nullptr, "" },
				{ __LINE__, L"",	 L"",	  "" },
				{ __LINE__, nullptr, L"123",  "123" },
				{ __LINE__, L"abc",	 nullptr, "abc" },
				{ __LINE__, L"abc",	 L"123",  "abc123" }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string result = String::Concat(data[i].pStr1, data[i].pStr2);
				Assert::AreEqual(data[i].pResult, result.c_str(), AssertMsg::Format(data[i].line));
			}
		}
	};
}
#endif //TODO-PORT