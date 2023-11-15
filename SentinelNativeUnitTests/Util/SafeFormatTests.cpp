#if 000//TODO-PORT
#include "stdafx.h"
#include "Util\SafeFormat.h"
#include "Util\OutputDebugFormat.h"
#include "AssertMsg.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SentinelNativeUnitTests
{
	// These tests were converted from another test framework
	TEST_CLASS(Util_SafeFormatTests)
	{
	public:
		TEST_METHOD(Util_SafeFormatEx)
		{
			struct
			{
				int line;
				int bufferLen;
				bool useEllipses;
				const char* pFormat;

				int result;
				SafeFormatError error;
				const char* pStr;
			}
			static data[] =
			{
				{ __LINE__,  9, false,   "12%d45", 5,  SafeFormatError::Success,   "12345" },
				{ __LINE__,  8, false,   "12%d45", 5,  SafeFormatError::Success,   "12345" },
				{ __LINE__,  7, false,   "12%d45", 5,  SafeFormatError::Success,   "12345" },
				{ __LINE__,  6, false,   "12%d45", 5,  SafeFormatError::Success,   "12345" },
				{ __LINE__,  5, false,   "12%d45", 4,  SafeFormatError::Length,    "1234" },
				{ __LINE__,  4, false,   "12%d45", 3,  SafeFormatError::Length,    "123" },
				{ __LINE__,  3, false,   "12%d45", 2,  SafeFormatError::Length,    "12" },
				{ __LINE__,  2, false,   "12%d45", 1,  SafeFormatError::Length,    "1" },
				{ __LINE__,  1, false,   "12%d45", 0,  SafeFormatError::Length,    "" },
				{ __LINE__,  0, false,   "12%d45", 5,  SafeFormatError::Length,    "abcdefghijklmn" },
				{ __LINE__,  0, false,   "12%d45", 5,  SafeFormatError::Length,    "abcdefghijklmn" },
				{ __LINE__, 12,  true, "12%d4567", 7,  SafeFormatError::Success,   "1234567" },
				{ __LINE__, 11,  true, "12%d4567", 7,  SafeFormatError::Success,   "1234567" },
				{ __LINE__, 10,  true, "12%d4567", 7,  SafeFormatError::Success,   "1234567" },
				{ __LINE__,  9,  true, "12%d4567", 7,  SafeFormatError::Success,   "1234567" },
				{ __LINE__,  8,  true, "12%d4567", 7,  SafeFormatError::Success,   "1234567" },
				{ __LINE__,  7,  true, "12%d4567", 6,  SafeFormatError::Length,    "123..." },
				{ __LINE__,  6,  true, "12%d4567", 5,  SafeFormatError::Length,    "12..." },
				{ __LINE__,  5,  true, "12%d4567", 4,  SafeFormatError::Length,	   "1..." },
				{ __LINE__,  4,  true, "12%d4567", 3,  SafeFormatError::Length,	   "123" },
				{ __LINE__,  3,  true, "12%d4567", 2,  SafeFormatError::Length,	   "12" },
				{ __LINE__,  2,  true, "12%d4567", 1,  SafeFormatError::Length,	   "1" },
				{ __LINE__,  1,  true, "12%d4567", 0,  SafeFormatError::Length,	   "" },
				{ __LINE__,  0,  true, "12%d4567", 7,  SafeFormatError::Length,    "abcdefghijklmn" },
				{ __LINE__,  9, false,       "%s", 6,  SafeFormatError::Exception, "!EX:%s" },
				{ __LINE__,  8, false,       "%s", 6,  SafeFormatError::Exception, "!EX:%s" },
				{ __LINE__,  7, false,       "%s", 6,  SafeFormatError::Exception, "!EX:%s" },
				{ __LINE__,  6, false,       "%s", 5,  SafeFormatError::Exception, "!EX:%" },
				{ __LINE__,  5, false,       "%s", 4,  SafeFormatError::Exception, "!EX:" },
				{ __LINE__,  4, false,       "%s", 3,  SafeFormatError::Exception, "!EX" },
				{ __LINE__,  3, false,       "%s", 2,  SafeFormatError::Exception, "!E" },
				{ __LINE__,  2, false,       "%s", 1,  SafeFormatError::Exception, "!" },
				{ __LINE__,  1, false,       "%s", 0,  SafeFormatError::Exception, "" },
				{ __LINE__,  0, false,       "%s",-6,  SafeFormatError::Exception, "abcdefghijklmn" },
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				char buffer[16];
				strcpy_s(buffer, "abcdefghijklmn");

				SafeFormatError error;
				int result = SafeFormatEx(data[i].useEllipses, &error, buffer, data[i].bufferLen, data[i].pFormat, 3);
				Assert::AreEqual(data[i].result, result, AssertMsg::Format(data[i].line, L"result"));
				Assert::AreEqual((int) data[i].error, (int) error, AssertMsg::Format(data[i].line, L"error"));
				Assert::AreEqual(data[i].pStr, buffer, AssertMsg::Format(data[i].line, L"buffer"));
			}
		}

		TEST_METHOD(Util_SafeFormatEx_nullbuffer)
		{
			int result = SafeFormatEx(false, nullptr, nullptr, 5, "12%d45", 3);
			Assert::AreEqual(5, result);
		}

		TEST_METHOD(Util_SafeFormatEx_nullbuffer_zerolen)
		{
			int result = SafeFormatEx(false, nullptr, nullptr, 0, "12%d45", 3);
			Assert::AreEqual(5, result);
		}

		TEST_METHOD(Util_SafeFormatEx_formatnullptr)
		{
			char buffer[32];
			strcpy_s(buffer, "abcdefghijklmn");
			int result = SafeFormatEx(false, nullptr, buffer, _countof(buffer), nullptr, 5);
			Assert::AreEqual(0, result, L"result");
		}

		TEST_METHOD(Util_SafeFormatExW)
		{
			struct
			{
				int line;
				int bufferLen;
				bool useEllipses;
				const wchar_t* pFormat;

				int result;
				SafeFormatError error;
				const wchar_t* pStr;
			}
			static data[] =
			{
				{ __LINE__,  9, false,   L"12%d45", 5,  SafeFormatError::Success,   L"12345" },
				{ __LINE__,  8, false,   L"12%d45", 5,  SafeFormatError::Success,   L"12345" },
				{ __LINE__,  7, false,   L"12%d45", 5,  SafeFormatError::Success,   L"12345" },
				{ __LINE__,  6, false,   L"12%d45", 5,  SafeFormatError::Success,   L"12345" },
				{ __LINE__,  5, false,   L"12%d45", 4,  SafeFormatError::Length,    L"1234" },
				{ __LINE__,  4, false,   L"12%d45", 3,  SafeFormatError::Length,    L"123" },
				{ __LINE__,  3, false,   L"12%d45", 2,  SafeFormatError::Length,    L"12" },
				{ __LINE__,  2, false,   L"12%d45", 1,  SafeFormatError::Length,    L"1" },
				{ __LINE__,  1, false,   L"12%d45", 0,  SafeFormatError::Length,    L"" },
				{ __LINE__,  0, false,   L"12%d45", 5,  SafeFormatError::Length,    L"abcdefghijklmn" },
				{ __LINE__,  0, false,   L"12%d45", 5,  SafeFormatError::Length,    L"abcdefghijklmn" },
				{ __LINE__, 12,  true, L"12%d4567", 7,  SafeFormatError::Success,   L"1234567" },
				{ __LINE__, 11,  true, L"12%d4567", 7,  SafeFormatError::Success,   L"1234567" },
				{ __LINE__, 10,  true, L"12%d4567", 7,  SafeFormatError::Success,   L"1234567" },
				{ __LINE__,  9,  true, L"12%d4567", 7,  SafeFormatError::Success,   L"1234567" },
				{ __LINE__,  8,  true, L"12%d4567", 7,  SafeFormatError::Success,   L"1234567" },
				{ __LINE__,  7,  true, L"12%d4567", 6,  SafeFormatError::Length,    L"123..." },
				{ __LINE__,  6,  true, L"12%d4567", 5,  SafeFormatError::Length,    L"12..." },
				{ __LINE__,  5,  true, L"12%d4567", 4,  SafeFormatError::Length,    L"1..." },
				{ __LINE__,  4,  true, L"12%d4567", 3,  SafeFormatError::Length,    L"123" },
				{ __LINE__,  3,  true, L"12%d4567", 2,  SafeFormatError::Length,    L"12" },
				{ __LINE__,  2,  true, L"12%d4567", 1,  SafeFormatError::Length,    L"1" },
				{ __LINE__,  1,  true, L"12%d4567", 0,  SafeFormatError::Length,    L"" },
				{ __LINE__,  0,  true, L"12%d4567", 7,  SafeFormatError::Length,    L"abcdefghijklmn" },
				{ __LINE__,  9, false,       L"%s", 6,  SafeFormatError::Exception, L"!EX:%s" },
				{ __LINE__,  8, false,       L"%s", 6,  SafeFormatError::Exception, L"!EX:%s" },
				{ __LINE__,  7, false,       L"%s", 6,  SafeFormatError::Exception, L"!EX:%s" },
				{ __LINE__,  6, false,       L"%s", 5,  SafeFormatError::Exception, L"!EX:%" },
				{ __LINE__,  5, false,       L"%s", 4,  SafeFormatError::Exception, L"!EX:" },
				{ __LINE__,  4, false,       L"%s", 3,  SafeFormatError::Exception, L"!EX" },
				{ __LINE__,  3, false,       L"%s", 2,  SafeFormatError::Exception, L"!E" },
				{ __LINE__,  2, false,       L"%s", 1,  SafeFormatError::Exception, L"!" },
				{ __LINE__,  1, false,       L"%s", 0,  SafeFormatError::Exception, L"" },
				{ __LINE__,  0, false,       L"%s",-6,  SafeFormatError::Exception, L"abcdefghijklmn" },
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				wchar_t buffer[16];
				wcscpy_s(buffer, L"abcdefghijklmn");

				SafeFormatError error;
				int result = SafeFormatEx(data[i].useEllipses, &error, buffer, data[i].bufferLen, data[i].pFormat, 3);
				Assert::AreEqual(data[i].result, result, AssertMsg::Format(data[i].line, L"result"));
				Assert::AreEqual((int) data[i].error, (int) error, AssertMsg::Format(data[i].line, L"error"));
				Assert::AreEqual(data[i].pStr, buffer, AssertMsg::Format(data[i].line, L"buffer"));
			}
		}

		TEST_METHOD(Util_SafeFormatExW_nullbuffer)
		{
			int result = SafeFormatEx(false, nullptr, nullptr, 5, L"12%d45", 3);
			Assert::AreEqual(5, result);
		}

		TEST_METHOD(Util_SafeFormatExW_nullbuffer_zerolen)
		{
			int result = SafeFormatEx(false, nullptr, nullptr, 0, L"12%d45", 3);
			Assert::AreEqual(5, result);
		}

		TEST_METHOD(Util_SafeFormatExW_formatnullptr)
		{
			wchar_t buffer[32];
			wcscpy_s(buffer, L"abcdefghijklmn");
			int result = SafeFormatEx(false, nullptr, buffer, _countof(buffer), nullptr, 5);
			Assert::AreEqual(0, result, L"result");
		}

		TEST_METHOD(Util_SafeFormatStdEx)
		{
			struct
			{
				int line;
				const char* pFormat;
				SafeFormatError error;
				const char* pStr;
			}
			static data[] =
			{
				{ __LINE__,   "12%d45", SafeFormatError::Success,   "12345"   },
				{ __LINE__, "12%d4567", SafeFormatError::Success,   "1234567" },
				{ __LINE__,       "%s", SafeFormatError::Exception, "!EX:%s"  },
				{ __LINE__,       "%s", SafeFormatError::Exception, "!EX:%s"  }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				SafeFormatError error;
				string s = SafeFormatStdEx(0, &error, data[i].pFormat, 3);
				Assert::AreEqual((int) data[i].error, (int) error, AssertMsg::Format(data[i].line, L"error"));
				Assert::AreEqual(data[i].pStr, s.c_str(), AssertMsg::Format(data[i].line, L"buffer"));
			}
		}

		TEST_METHOD(Util_SafeFormatStdExW)
		{
			struct
			{
				int line;
				const wchar_t* pFormat;
				SafeFormatError error;
				const wchar_t* pStr;
			}
			static data[] =
			{
				{ __LINE__,   L"12%d45", SafeFormatError::Success,   L"12345"   },
				{ __LINE__, L"12%d4567", SafeFormatError::Success,   L"1234567" },
				{ __LINE__,       L"%s", SafeFormatError::Exception, L"!EX:%s"  },
				{ __LINE__,       L"%s", SafeFormatError::Exception, L"!EX:%s"  }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				SafeFormatError error;
				wstring s = SafeFormatStdEx(0, &error, data[i].pFormat, 3);
				Assert::AreEqual((int) data[i].error, (int) error, AssertMsg::Format(data[i].line, L"error"));
				Assert::AreEqual(data[i].pStr, s.c_str(), AssertMsg::Format(data[i].line, L"buffer"));
			}
		}

		TEST_METHOD(Util_SafeOutputDebugFormatA)
		{
			struct
			{
				int line;
				const char* pFormat;
			}
			static data[] =
			{
				{ __LINE__,   "12%d45" },
				{ __LINE__,       "%s" },
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				SafeOutputDebugFormat(data[i].pFormat, 3);
			}
		}

		TEST_METHOD(Util_SafeOutputDebugFormatW)
		{
			struct
			{
				int line;
				const wchar_t* pFormat;
			}
			static data[] =
			{
				{ __LINE__,   L"12%d45" },
				{ __LINE__,       L"%s" },
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				SafeOutputDebugFormat(data[i].pFormat, 3);
			}
		}
	};
}
#endif //TODO-PORT