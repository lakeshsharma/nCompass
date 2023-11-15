#include "stdafx.h"
#include "Config/ConfigBalanceRetrievalTextField.h"

// Sentinel includes.
#include "Config.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigBalanceRetrievalTextUnitTests : public ::testing::Test
	{
	public:
	};

		TEST(ConfigBalanceRetrievalTextUnitTests, ConfigBalanceRetrievalTextFieldConstructorTest)
		{
			CConfigBalanceRetrievalTextField field;
			ASSERT_TRUE(BALANCE_RETRIEVAL_TEXT == field.GetType()) << L"Field object has the wrong type.";
			ASSERT_TRUE("" == field.GetBalanceRetrievalText()) << L"Field has wrong default value for player text.";
		}

		TEST(ConfigBalanceRetrievalTextUnitTests, ConfigBalanceRetrievalTextFieldGetSetTest)
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

			CConfigBalanceRetrievalTextField field;
			field.SetBalanceRetrievalText(testBuf, _countof(testBuf));
			ASSERT_TRUE(testMsg == field.GetBalanceRetrievalText()) << L"Balance Retrieval text not returned what was set.";
		}

}
