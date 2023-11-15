#include "stdafx.h"
#include <ProgressiveProtocol/ProgressivePteState.h>


#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	// Note that copy constructor and assignment operator don't need to be tested since they are provided by compiler.
		TEST(ProgressivePteStateTest, ProgressivePteState_ctor)
		{
			ProgressivePteState pteState(5);

			ASSERT_EQ((uint64_t) 5, (uint64_t) pteState.GetLevelId()) << "GetLevelId()";
			ASSERT_EQ(0, pteState.curSequence) << "curSequence";
			ASSERT_FALSE(pteState.wasStartingDisplayed) << "wasStartingDisplayed";
			ASSERT_FALSE(pteState.isLeaderboardDisplayed) << "isLeaderboardDisplayed";
			ASSERT_FALSE(pteState.wasEndingDisplayed) << "wasEndingDisplayed";
			ASSERT_FALSE(pteState.wereResultsDisplayed) << "wereResultsDisplayed";
		}
}
