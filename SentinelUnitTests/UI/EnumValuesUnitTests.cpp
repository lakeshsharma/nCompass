#include "stdafx.h"
#include "ServiceWindowDefs.h"


#include "gtest/gtest.h"


namespace SentinelUnitTests
{
		//This tests ensures that All param prefix labels are grouped together
		//and Custom1aParamPrefix is first entry and last entry is PteParamPrefix
		TEST(EnumValuesUnitTets, LabelTypeParamValuesTest)
		{
			{
				ASSERT_TRUE(Custom1aParamPrefix + 1 == Custom1bParamPrefix);
				ASSERT_TRUE(Custom1bParamPrefix + 1 == Custom1cParamPrefix);
				ASSERT_TRUE(Custom1cParamPrefix + 1 == Custom1dParamPrefix);
				ASSERT_TRUE(Custom1dParamPrefix + 1 == Custom1eParamPrefix);
				ASSERT_TRUE(Custom1eParamPrefix + 1 == Custom1fParamPrefix);
				ASSERT_TRUE(Custom1fParamPrefix + 1 == Custom2aParamPrefix);
				ASSERT_TRUE(Custom2aParamPrefix + 1 == Custom2bParamPrefix);
				ASSERT_TRUE(Custom2bParamPrefix + 1 == Custom2cParamPrefix);
				ASSERT_TRUE(Custom2cParamPrefix + 1 == Custom2dParamPrefix);
				ASSERT_TRUE(Custom2dParamPrefix + 1 == Custom2eParamPrefix);
				ASSERT_TRUE(Custom2eParamPrefix + 1 == Custom2fParamPrefix);
				ASSERT_TRUE(Custom2fParamPrefix + 1 == ResortParamPrefix);
				ASSERT_TRUE(ResortParamPrefix  + 1 == ResortaParamPrefix);
				ASSERT_TRUE(ResortaParamPrefix + 1 == ResortbParamPrefix);
				ASSERT_TRUE(ResortbParamPrefix + 1 == ResortcParamPrefix);
				ASSERT_TRUE(ResortcParamPrefix + 1 == ResortdParamPrefix);
				ASSERT_TRUE(ResortdParamPrefix + 1 == ResorteParamPrefix);
				ASSERT_TRUE(ResorteParamPrefix + 1 == ResortfParamPrefix);
				ASSERT_TRUE(ResortfParamPrefix + 1 == PteParamPrefix);	
			}
		}
}
