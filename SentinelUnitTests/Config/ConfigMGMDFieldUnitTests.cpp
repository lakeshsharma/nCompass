#include "stdafx.h"
#include "ConfigMGMDField.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class ConfigMGMDFieldUnitTests : public ::testing::Test
	{

	public:
	};

		TEST(ConfigMGMDFieldUnitTests, ConfigGlobalMGMDConstructorUnitTest)
		{
			CConfigMGMDField configMGMD;

			ASSERT_FALSE(configMGMD.IsGlobalMGMD());
		}

        TEST(ConfigMGMDFieldUnitTests, ConfigGlobalMGMDUnitTest)
		{
			CConfigMGMDField configMGMD;

            configMGMD.SetGlobalMGMD(false);
			ASSERT_FALSE(configMGMD.IsGlobalMGMD());

            configMGMD.SetGlobalMGMD(true);
			ASSERT_TRUE(configMGMD.IsGlobalMGMD());
		}
}
