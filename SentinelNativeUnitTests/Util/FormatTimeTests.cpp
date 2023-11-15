#if 000//TODO-PORT
#include "stdafx.h"
#include <Util/FormatTime.h>
#include "AssertMsg.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(Util_FormatTimeTests)
	{
	public:
		TEST_METHOD(Util_FormatTime_Sortable)
		{
			SYSTEMTIME sysTime = { 2015, 1, 1, 2, 3, 4, 5, 6 };

			struct
			{
				int line;
				FormatTime::Style style;
				const char* pExpected;
			}
			data[] =
			{
				{ __LINE__, (FormatTime::Style)(FormatTime::Style::FourDigitYear | FormatTime::Style::Milliseconds)  , "2015/01/02 03:04:05.006" },
				{ __LINE__, (FormatTime::Style)(FormatTime::Style::FourDigitYear | FormatTime::Style::NoMilliseconds), "2015/01/02 03:04:05"     },
				{ __LINE__, (FormatTime::Style)(FormatTime::Style::TwoDigitYear  | FormatTime::Style::Milliseconds)  ,   "15/01/02 03:04:05.006" },
				{ __LINE__, (FormatTime::Style)(FormatTime::Style::TwoDigitYear  | FormatTime::Style::NoMilliseconds),   "15/01/02 03:04:05"     },
				{ __LINE__, (FormatTime::Style)(FormatTime::Style::NoYear        | FormatTime::Style::Milliseconds)  ,      "01/02 03:04:05.006" },
				{ __LINE__, (FormatTime::Style)(FormatTime::Style::NoYear        | FormatTime::Style::NoMilliseconds),      "01/02 03:04:05"     }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				string s = FormatTime::Sortable(sysTime, data[i].style);
				Assert::AreEqual(data[i].pExpected, s.c_str(), AssertMsg::Format(data[i].line, nullptr));
			}
		}

		TEST_METHOD(Util_FormatTime_local_noyear)
		{
			const char* pRegExNoYear = "^[0-9][0-9]/[0-9][0-9] [0-9][0-9]:[0-9][0-9]:[0-9][0-9].[0-9][0-9][0-9]$";

			string s = FormatTime::Sortable(false, FormatTime::NoYear);
			Assert::IsTrue(regex_match(s, regex(pRegExNoYear)));
		}

		TEST_METHOD(Util_FormatTime_local_year)
		{
			const char* pRegExYear = "^[0-9][0-9][0-9][0-9]/[0-9][0-9]/[0-9][0-9] [0-9][0-9]:[0-9][0-9]:[0-9][0-9].[0-9][0-9][0-9]$";

			string s = FormatTime::Sortable(false, FormatTime::FourDigitYear);
			Assert::IsTrue(regex_match(s, regex(pRegExYear)));
		}

		TEST_METHOD(Util_FormatTime_system_noyear)
		{
			const char* pRegExNoYear = "^[0-9][0-9]/[0-9][0-9] [0-9][0-9]:[0-9][0-9]:[0-9][0-9].[0-9][0-9][0-9]$";

			string s = FormatTime::Sortable(true, FormatTime::NoYear);
			Assert::IsTrue(regex_match(s, regex(pRegExNoYear)));
		}

		TEST_METHOD(Util_FormatTime_system_year)
		{
			const char* pRegExYear = "^[0-9][0-9][0-9][0-9]/[0-9][0-9]/[0-9][0-9] [0-9][0-9]:[0-9][0-9]:[0-9][0-9].[0-9][0-9][0-9]$";

			string s = FormatTime::Sortable(true, FormatTime::FourDigitYear);
			Assert::IsTrue(regex_match(s, regex(pRegExYear)));
		}
	};
}
#endif //TODO-PORT