#include "stdafx.h"
#include "Config/ConfigDailyPointsDisplaySuffixField.h"				 

// Sentinel includes.
#include "Config.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigDailyPointsDisplaySuffixFieldUnitTests : public ::testing::Test
	{
	public:
	};

		TEST(ConfigDailyPointsDisplaySuffixFieldUnitTests, ConfigDailyPointsDisplaySuffixFieldConstructorTest)
		{
			CConfigDailyPointsDisplaySuffixField field;
			ASSERT_TRUE(DAILY_POINTS_DISPLAY_SUFFIX == field.GetType()) << L"Field object has the wrong type.";
			ASSERT_TRUE("" == field.GetDailyPointsDisplaySuffixText()) << L"Field has wrong default value for Daily Points Display Suffix.";
		}

		TEST(ConfigDailyPointsDisplaySuffixFieldUnitTests, ConfigTierPointsLabelGetSetTest)
		{
			string testMsg = "Nothin' but a stinkin test.";
			while (SIZE_OF_MESSAGES - 1 > testMsg.length())
			{
				testMsg += ".";
			}

			byte testBuf[SIZE_OF_MESSAGES - 1];
			for (int n = 0; n < _countof(testBuf); n++)
			{
				testBuf[n] = (byte)testMsg.at(n);
			}

			CConfigDailyPointsDisplaySuffixField field;
			field.SetDailyPointsDisplaySuffixText(testBuf, _countof(testBuf));
			ASSERT_TRUE(testMsg == field.GetDailyPointsDisplaySuffixText()) << L"Daily Points Display Suffix not returned what was set.";
		}

}
