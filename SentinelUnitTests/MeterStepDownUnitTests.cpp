
#include "stdafx.h"
#include "SASProtocol/MeterStepdown.h"


#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{

		TEST(MeterStepdownUnitTests, MeterStepdownAddMeterItem)
		{
			byte oasisMeter = 26;
			byte primaryLongPoll = 0x6f;
			byte secondaryLongPoll = 0x2f;
			byte fallback = 0x2a;
			byte secondaryFallback = 0x1c;

			CMeterStepdown *stepdown = new CMeterStepdown();

			ASSERT_TRUE(stepdown->GetMeterCount() == 0);
			stepdown->AddMeterItem(oasisMeter, primaryLongPoll, secondaryLongPoll, fallback, secondaryFallback);
			ASSERT_TRUE(stepdown->GetMeterCount() == 1);

			delete stepdown;
		}

		TEST(MeterStepdownUnitTests, MeterstepdownDecrementCount)
		{
			byte oasisMeter = 26;
			byte primaryLongPoll = 0x6f;
			byte secondaryLongPoll = 0x2f;
			byte fallback = 0x2a;
			byte secondaryFallback = 0x1c;

			CMeterStepdown *stepdown = new CMeterStepdown();

			stepdown->AddMeterItem(oasisMeter, primaryLongPoll, secondaryLongPoll, fallback, secondaryFallback);

			ASSERT_TRUE(stepdown->GetMeterLongPoll(oasisMeter) == primaryLongPoll);
			
			// Simulate 3 failures
			for (int i = 0; i < 3; i++)
			{
				stepdown->MeterLongPollFail(oasisMeter);
			}
			ASSERT_TRUE(stepdown->GetMeterLongPoll(oasisMeter) == secondaryLongPoll);

			delete stepdown;
		}

		TEST(MeterStepdownUnitTests, MeterstepdownResetAllPollsResetOneSuccess)
		{
			byte oasisMeter = 12;
			byte primaryLongPoll = 0x6f;
			byte secondaryLongPoll = 0x2f;
			byte fallback = 0x1e;
			byte secondaryFallback = 0x31;

			CMeterStepdown *stepdown = new CMeterStepdown();
			stepdown->AddMeterItem(oasisMeter, primaryLongPoll, secondaryLongPoll, fallback, secondaryFallback);
			stepdown->DeactivateForSAS3x();

			// Set up last fallback as the successful poll
			byte lp = stepdown->GetMeterLongPoll(oasisMeter);
			while (lp != secondaryFallback && lp != 0)
			{
				stepdown->MeterLongPollFail(oasisMeter);
				lp = stepdown->GetMeterLongPoll(oasisMeter);
			}
			stepdown->MeterLongPollSuccessful(oasisMeter);

			// Reset polls
			stepdown->ResetAllStepdowns();

			ASSERT_TRUE(stepdown->GetMeterLongPoll(oasisMeter) == primaryLongPoll);

			delete stepdown;
		}

		TEST(MeterStepdownUnitTests, MeterstepdownDeactivateForSAS3x)
		{
			byte oasisMeter = 26;
			byte primaryLongPoll = 0x2a;
			byte secondaryLongPoll = 0;
			byte fallback = 0;
			byte secondaryFallback = 0;

			CMeterStepdown* stepdown = new CMeterStepdown();
			stepdown->AddMeterItem(oasisMeter, primaryLongPoll, secondaryLongPoll, fallback, secondaryFallback);

			int lp = stepdown->GetMeterLongPoll(oasisMeter);

			// Before deactivating SAS 3 polls
			ASSERT_EQ(lp, primaryLongPoll);

			stepdown->DeactivateForSAS3x();

			// After deactivating SAS 3 polls
			lp = stepdown->GetMeterLongPoll(oasisMeter);
			ASSERT_EQ(lp, 0);

			// Reset polls
			stepdown->ResetAllStepdowns();
			lp = stepdown->GetMeterLongPoll(oasisMeter);
			ASSERT_EQ(lp, primaryLongPoll);

			delete stepdown;
		}

		TEST(MeterStepdownUnitTests, MeterstepdownDeactivateForSAS4x)
		{
			byte oasisMeter = 26;
			byte primaryLongPoll = 0x2a;
			byte secondaryLongPoll = 0;
			byte fallback = 0;
			byte secondaryFallback = 0;

			CMeterStepdown* stepdown = new CMeterStepdown();
			stepdown->AddMeterItem(oasisMeter, primaryLongPoll, secondaryLongPoll, fallback, secondaryFallback);

			byte lp = stepdown->GetMeterLongPoll(oasisMeter);

			// Before deactivating SAS 3 polls
			ASSERT_EQ(lp, primaryLongPoll);

			stepdown->DeactivateForSAS4x();

			// After deactivating SAS 3 polls
			lp = stepdown->GetMeterLongPoll(oasisMeter);
			ASSERT_EQ(lp, primaryLongPoll);

			// Reset polls
			stepdown->ResetAllStepdowns();
			lp = stepdown->GetMeterLongPoll(oasisMeter);
			ASSERT_EQ(lp, primaryLongPoll);

			delete stepdown;
		}
}

