#include "stdafx.h"
#include "TestUtil.h"
#include "AssertMsg.h"
#include "Util/Currency.h"


#include "gtest/gtest.h"

using namespace std;

namespace SentinelUnitTests
{
		TEST(Util_CurrencyTests, Util_Currency_ToInt64a)
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
				{ __LINE__, " -3.14",  true,  -314   },

				{ __LINE__, "",        false, 0      },
				{ __LINE__, "1",       true,  100    },
				{ __LINE__, "2x",      false, 200    },
				{ __LINE__, "3.",      true,  300    },
				{ __LINE__, "3.1",     true,  310    },
				{ __LINE__, "3.14",    true,  314    },
				{ __LINE__, " 3.14",   true,  314    },
				{ __LINE__, " -3.14",  true,  -314   },
				{ __LINE__, "1234.56", true,  123456 },
				{ __LINE__, "1234.567",false, 123456 },
				{ __LINE__, "1234.56x",false, 123456 },
				{ __LINE__, nullptr,   false, 0      }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				int64_t result;
				bool rval = Currency::ToInt64(data[i].pStr, result);
				ASSERT_EQ(data[i].expectedReturn, rval) << AssertMsg::Format(data[i].line, L"return");
				ASSERT_TRUE(data[i].expectedResult == result) << AssertMsg::Format(data[i].line, L"result", L"%lld != %lld", data[i].expectedResult, result);
			}
		}

		TEST(Util_CurrencyTests, Util_Currency_ToInt64b)
		{
			struct
			{
				int line;
				double val;
				int64_t expectedResult;
			}
			static data[] =
			{
				{ __LINE__, 0.0		 ,       0 },
				{ __LINE__, 1.1		 ,     110 },
				{ __LINE__, 1234.567 ,  123457 },
				{ __LINE__, -1234.567, -123457 }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				int64_t result = Currency::ToInt64(data[i].val);
				ASSERT_EQ((int) data[i].expectedResult, (int) result) << AssertMsg::Format(data[i].line, L"result", L"%lld != %lld", data[i].expectedResult, result);
			}
		}

		TEST(Util_CurrencyTests, Util_Currency_ToString_double)
		{
			struct
			{
				int line;
				double val;
				const char* pExpectedResult;
			}
			static data[] =
			{
				{ __LINE__,  0.0,      "0.00"    },
				{ __LINE__,  1.1,      "1.10"    },
				{ __LINE__,  1234.567, "1234.57" },
				{ __LINE__, -1234.567, "-1234.57"}
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string result = Currency::ToString(data[i].val);
				ASSERT_EQ(strcmp(data[i].pExpectedResult, result.c_str()), 0) << AssertMsg::Format(data[i].line, L"result");
			}
		}

		TEST(Util_CurrencyTests, Util_Currency_Tostring_double)
		{
			struct
			{
				int line;
				double val;
				const char* pExpectedResult;
			}
			static data[] =
			{
				{ __LINE__, 0.0,       "0.00"     },
				{ __LINE__, 1.1,       "1.10"     },
				{ __LINE__, 1234.567,  "1234.57"  },
				{ __LINE__, -1234.567, "-1234.57" }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string result = Currency::ToString(data[i].val);
				ASSERT_EQ(strcmp(data[i].pExpectedResult, result.c_str()), 0) << AssertMsg::Format(data[i].line, L"result");
			}
		}

		TEST(Util_CurrencyTests, Util_Currency_ToString_int64_t)
		{
			struct
			{
				int line;
				int64_t val;
				const char* pExpectedResult;
			}
			static data[] =
			{
				{ __LINE__, 110,     "1.10"    },
				{ __LINE__, 123456,  "1234.56" },
				{ __LINE__, -123456, "-1234.56"}
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string result = Currency::ToString(data[i].val);
				ASSERT_EQ(strcmp(data[i].pExpectedResult, result.c_str()), 0) << AssertMsg::Format(data[i].line, L"result");
			}
		}

		TEST(Util_CurrencyTests, Util_Currency_Tostring_int64_t)
		{
			struct
			{
				int line;
				int64_t val;
				const char* pExpectedResult;
			}
			static data[] =
			{
				{ __LINE__, -123456, "-1234.56"},
				{ __LINE__, 110,     "1.10"    },
				{ __LINE__, 123456,  "1234.56" },
				{ __LINE__, -123456, "-1234.56"},
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string result = Currency::ToString(data[i].val);
				ASSERT_EQ(strcmp(data[i].pExpectedResult, result.c_str()), 0) << AssertMsg::Format(data[i].line, L"result");
			}
		}

		TEST(Util_CurrencyTests, Util_Convert_Int64ToCurrency_int64_t)
		{
			struct
			{
				int line;
				int64_t val;
				double expectedResult;
			}
			static data[] =
			{
				{ __LINE__, 0,       0.00    },
				{ __LINE__, 110,     1.10    },
				{ __LINE__, 123456,  1234.56 },
				{ __LINE__, -123456, -1234.56}
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				double result = Currency::Int64ToCurrency(data[i].val);
				ASSERT_EQ(data[i].expectedResult, result) << AssertMsg::Format(data[i].line, L"result");
			}
		}
}
