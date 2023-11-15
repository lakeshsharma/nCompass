#include "stdafx.h"
#include "Config/ConfigPointsOrCashLabelField.h"

// Sentinel includes.
#include "Config.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigPointsOrCashLabelFieldUnitTests : public ::testing::Test
	{
	public:
	};

		TEST(ConfigPointsOrCashLabelFieldUnitTests, ConfigPointsOrCashLabelFieldConstructorTest)
		{
			CConfigPointsOrCashLabelField field;
			ASSERT_TRUE(POINTS_OR_CASH_LABEL == field.GetType()) << L"Field object has the wrong type.";
			ASSERT_TRUE("" == field.GetPointsOrCashLabel()) << L"Field has wrong default value for points/cash label.";
		}

		TEST(ConfigPointsOrCashLabelFieldUnitTests, ConfigPointsOrCashFieldGetSetTest)
		{
			string testMsg = "Nothin' but a stinkin test.";
			while (SIZE_OF_LABEL - 1 > testMsg.length())
			{
				testMsg += ".";
			}

			byte testBuf[SIZE_OF_LABEL - 1];
			for (int n = 0; n < _countof(testBuf); n++)
			{
				testBuf[n] = (byte)testMsg.at(n);
			}

			CConfigPointsOrCashLabelField field;
			field.SetPointsOrCashLabel(testBuf, _countof(testBuf));
			ASSERT_TRUE(testMsg == field.GetPointsOrCashLabel()) << L"Point/Cash Label not returned what was set.";
		}

}
