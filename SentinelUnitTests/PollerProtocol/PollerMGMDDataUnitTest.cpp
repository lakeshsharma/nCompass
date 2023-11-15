#include "stdafx.h"
#include "PollerProtocol/PollerMGMDData.h"
#include "GlobalDefs.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class PollerMGMDDataUnitTests : public ::testing::Test
	{
	};

		TEST(PollerMGMDDataUnitTests, PollerMGMDDataTest)
		{

            CPollerMGMDData::Instance().SetMGMDGameStart(true);
            ASSERT_EQ(CPollerMGMDData::Instance().GetMGMDGameStart(), 1);

            CPollerMGMDData::Instance().SetMGMDGameEnd(true);
            ASSERT_EQ(CPollerMGMDData::Instance().GetMGMDGameEnd(), 1);

            CPollerMGMDData::Instance().SetMGMDPollingSpeed(1);
            ASSERT_EQ(CPollerMGMDData::Instance().GetMGMDPollingSpeed(), 1);

            CPollerMGMDData::Instance().SetMGMDSenOption(true);
            ASSERT_EQ(CPollerMGMDData::Instance().GetMGMDSenOption(), 1);

            ASSERT_EQ(CPollerMGMDData::Instance().GetMGMDAvailable(), 0);

            CPollerMGMDData::Instance().SetMGMDPollingSpeed(0);
            ASSERT_EQ(CPollerMGMDData::Instance().GetMGMDPollingSpeed(), 0);

            ASSERT_EQ(CPollerMGMDData::Instance().GetMGMDAvailable(), 1);
		}
}
