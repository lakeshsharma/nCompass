#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveCriticalText.h"

using namespace std;
#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{
	class ProgressiveCriticalTextTests : public ::testing::Test
	{
	public:
		void f_ProgressiveCriticalTextConstructorTest(void)
		{
			CProgressiveCriticalText criticalText;

			ASSERT_FALSE(criticalText.IsDown()) << "The critical text message should not be down after construction";

			WORD alert = criticalText.GetAlert();
			ASSERT_TRUE(alert == 0) << "Alert should not be there.";
		}

		void f_ProgressiveCriticalTextStateChangeTest(void)
		{
			CProgressiveCriticalText criticalText;

			CProgressiveConfig config;

			config.SetActive(false);

			ASSERT_FALSE(criticalText.ProcessProgressiveConfig(&config)) << "An inactive config should NOT have changed the state after construction";
			ASSERT_FALSE(criticalText.IsDown()) << "An inactive config should never make the critical text as down";

			config.SetActive(true);
			ASSERT_TRUE(criticalText.ProcessProgressiveConfig(&config)) << "Changing config to active should have changed the state";
			ASSERT_TRUE(criticalText.IsDown()) << "Should be considered down when the config is active";

			CProgressiveConnectionInfo info;
			info.SetState(InDiscovery);
			ASSERT_FALSE(criticalText.ProcessConnectionInfo(&info)) << "A not connected progressive should not have changed the state";
			ASSERT_TRUE(criticalText.IsDown()) << "Should be considered down when the progressive is not connected";

			info.SetState(Connected);
			ASSERT_FALSE(criticalText.ProcessConnectionInfo(&info)) << "The system and egm are not on-line so no state change";
			ASSERT_TRUE(criticalText.IsDown()) << "Should be considered down when the egm and sytem are not connected";

			ASSERT_FALSE(criticalText.ProcessSasStartupQueueEmpty(false)) << "The SAS startup queue empty status is false so no state change";
			ASSERT_TRUE(criticalText.IsDown()) << "Should be considered down when the SAS startup queue empty status is false ";

			ASSERT_FALSE(criticalText.ProcessEligibilityDetermined(false)) << "The eligibilty is not determined so no state change";
			ASSERT_TRUE(criticalText.IsDown()) << "Should be considered not down when the eligibilty is not determined";

			ASSERT_FALSE(criticalText.ProcessSessionIdMatchStatus(false)) << "The session ID match status is false so no state change";
			ASSERT_TRUE(criticalText.IsDown()) << "Should be considered down when the session ID match status is false ";

			// Tests ProcessSasStartupQueueEmpty().
			criticalText.ProcessSasStartupQueueEmpty(false);
			criticalText.ProcessEligibilityDetermined(true);
			criticalText.ProcessSessionIdMatchStatus(true);
			ASSERT_FALSE(criticalText.ProcessSasStartupQueueEmpty(false)) << "The SAS startup queue empty status is false so no state change";
			ASSERT_TRUE(criticalText.IsDown()) << "Should be considered down when the SAS startup queue empty status is false ";

			ASSERT_TRUE(criticalText.ProcessSasStartupQueueEmpty(true)) << "The SAS startup queue empty status is true so the state changes";
			ASSERT_FALSE(criticalText.IsDown()) << "Should be considered Up when the SAS startup queue empty status is true";

			// Tests ProcessEligibilityDetermined().
			criticalText.ProcessEligibilityDetermined(false);
			criticalText.ProcessSasStartupQueueEmpty(true);
			criticalText.ProcessSessionIdMatchStatus(true);
			ASSERT_FALSE(criticalText.ProcessEligibilityDetermined(false)) << "The eligibilty is not determined so no state change";
			ASSERT_FALSE(criticalText.IsDown()) << "Should be considered down when the eligibilty is not determined";

			ASSERT_TRUE(criticalText.ProcessEligibilityDetermined(true)) << "The eligibilty is determined so the state changes";
			ASSERT_FALSE(criticalText.IsDown()) << "Should be considered Up when the eligibilty is determined";

			// Tests ProcessSessionIdMatchStatus().
			criticalText.ProcessSessionIdMatchStatus(false);
			criticalText.ProcessSasStartupQueueEmpty(true);
			criticalText.ProcessEligibilityDetermined(true);
			ASSERT_FALSE(criticalText.ProcessSessionIdMatchStatus(false)) << "The session ID match status was false so no state change";
			ASSERT_TRUE(criticalText.IsDown()) << "Should be considered down when the session ID match status was false ";

			ASSERT_TRUE(criticalText.ProcessSessionIdMatchStatus(true)) << "The session ID match status is true so the state changes";
			ASSERT_FALSE(criticalText.IsDown()) << "Should be considered Up when the session ID match status is true";
		}
	};
	TEST_F(ProgressiveCriticalTextTests, ProgressiveCriticalTextConstructorTest)
	{
		f_ProgressiveCriticalTextConstructorTest();
	}

	TEST_F(ProgressiveCriticalTextTests, ProgressiveCriticalTextStateChangeTest)
	{
		f_ProgressiveCriticalTextStateChangeTest();
	}
}
