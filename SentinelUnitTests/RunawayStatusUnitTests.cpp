#include "stdafx.h"
#include "RunawayStatus.h"
#include "Meters.h"
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;

namespace SentinelUnitTests
{
	class RunawayStatusUnitTests : public ::testing::Test
	{
	public:
        virtual void SetUp() 
        {
        }

        virtual void TearDown() 
        {
        }
    };
	TEST_F(RunawayStatusUnitTests, RunawayStatusCheckStatusWithRunawayUnitTest)
	{
		CRunawayStatus *run = new CRunawayStatus();

		bool action = false;
			
		// Get gCI ready to Runaway
		for (int i = 0; i < MAXIMUM_RUNAWAY - 1; i++)
		{
			action = run->GetSendAction(gCI);
			ASSERT_FALSE(action) << "Status checked returned incorrect value";
		}

		// Next time should return true
		action = run->GetSendAction(gCI);
		ASSERT_TRUE(action) << "Runaway status returned false";

		delete run;

	}

    TEST_F(RunawayStatusUnitTests, RunawayStatusCheckStatusWithMultipleMetersUnitTest)
	{
		CRunawayStatus *run = new CRunawayStatus();

		bool action = false;
		run->GetSendAction(mCrd);
		run->GetSendAction(gCI);
		run->GetSendAction(gCO);
		run->GetSendAction(mTHP);
			
		// Get gCI ready to Runaway
		for (int i = 0; i < MAXIMUM_RUNAWAY - 2; i++)
		{
			action = run->GetSendAction(gCI);
			ASSERT_FALSE(action) << "Status checked returned incorrect value";
		}

		// Next time should return true
		action = run->GetSendAction(gCI);
		ASSERT_TRUE(action) << "Runaway status returned false";

		// These should return False
		action = run->GetSendAction(mCrd);
		ASSERT_FALSE(action) << "Runaway status returned false";
		action = run->GetSendAction(gCO);
		ASSERT_FALSE(action) << "Runaway status returned false";
		action = run->GetSendAction(mTHP);
		ASSERT_FALSE(action) << "Runaway status returned false";
		delete run;
	}
}
