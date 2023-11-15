#include "stdafx.h"
#include "Config/ConfigTierPointsLabelField.h"				 

// Sentinel includes.
#include "Config.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigTierPointsLabelFieldUnitTests : public ::testing::Test
	{
	public:
	};

		TEST(ConfigTierPointsLabelFieldUnitTests, ConfigTierPointsLabelFieldConstructorTest)
		{
			CConfigTierPointsLabelField field;
			ASSERT_TRUE(TIER_POINTS_LABEL == field.GetType()) << L"Field object has the wrong type.";
			ASSERT_TRUE("" == field.GetTierPointsLabel()) << L"Field has wrong default value for Tier Points Label.";
		}

		TEST(ConfigTierPointsLabelFieldUnitTests, ConfigTierPointsLabelGetSetTest)
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

			CConfigTierPointsLabelField field;
			field.SetTierPointsLabel(testBuf, _countof(testBuf));
			ASSERT_TRUE(testMsg == field.GetTierPointsLabel()) << L"Tier Points Label not returned what was set.";
		}

}
