#include "stdafx.h"
#include "Config/ConfigShowPointsVsAccountBalanceField.h"

// Sentinel includes.
#include "Config.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigShowPointsVsAccountBalanceUnitTests : public ::testing::Test
	{
	public:
	};

		TEST(ConfigShowPointsVsAccountBalanceUnitTests, ConfigPointsOrCashLabelFieldConstructorTest)
		{
			CConfigShowPointsVsAccountBalanceField field;
			ASSERT_TRUE(SHOW_POINTS_VS_ACCOUNT_BALANCE == field.GetType()) << L"Field object has the wrong type.";
			ASSERT_FALSE(field.GetShowPointsVsAccountBalance()) << L"Field has wrong default value for Show Points vs Account Balance.";
		}

		TEST(ConfigShowPointsVsAccountBalanceUnitTests, ConfigPointsOrCashLabelFieldGetSetTest)
		{
			CConfigShowPointsVsAccountBalanceField field;
			field.SetShowPointsVsAccountBalance(true);
			ASSERT_TRUE(field.GetShowPointsVsAccountBalance()) << L"Show Points vs Account Balance not returned what was set.";
		}
}
