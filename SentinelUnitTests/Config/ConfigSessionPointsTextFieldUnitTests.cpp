#include "stdafx.h"
#include "Config/ConfigSessionPointsTextField.h"

// Sentinel includes.
#include "Config.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigSessionPointsTextFieldUnitTests : public ::testing::Test
	{
	public:
	};

		TEST(ConfigSessionPointsTextFieldUnitTests, ConfigSessionPointsTextFieldConstructorTest)
		{
			CConfigSessionPointsTextField field;
			ASSERT_TRUE(SESSION_POINTS_TEXT == field.GetType()) << L"Field object has the wrong type.";
			ASSERT_TRUE("" == field.GetSessionPointsText()) << L"Field has wrong default value for guest text.";
		}

		TEST(ConfigSessionPointsTextFieldUnitTests, ConfigSessionPointsTextFieldGetSetTest)
		{
			string testMsg = "This is a test. Only a test. Nothin' but a stinkin test.";
			while (SIZE_OF_MESSAGES - 1 > testMsg.length())
			{
				testMsg += ".";
			}

			byte testBuf[SIZE_OF_MESSAGES - 1];
			for (int n = 0; n < _countof(testBuf); n++)
			{
				testBuf[n] = (byte)testMsg.at(n);
			}

			CConfigSessionPointsTextField field;
			field.SetSessionPointsText(testBuf, _countof(testBuf));
			ASSERT_TRUE(testMsg == field.GetSessionPointsText()) << L"Session Points text not returned what was set.";
		}

}
