#include "stdafx.h"
#include "PlayerUpdateManager.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "AssertMsg.h"
#include "TestUtil.h"

using namespace std;

	class PlayerUpdateManager : public ::testing::Test
	{
	};

		// Add to sleep time, to help ensure that time_t increments during a CUtilities::Sleep() call.
		// Note that time_t might not be calculated directly from CUtilities::GetTickCount(), especially if the server uses NTP.
		// A 17-tick count granularity (1/60 of a second) has been observed on some servers.
		static const DWORD SLEEP_ADD = 35; // Two 17-tick count intervals (1/60 of a second), rounded up.

		TEST(PlayerUpdateManager, PlayerUpdateTimeTest)
		{
			CPlayerUpdateManager updateManager;

			time_t lastPlayerUpdateTime = CUtilities::GetUTCTime();

			ASSERT_FALSE(updateManager.CheckIfPlayerUpdateNeedsToBeSent(lastPlayerUpdateTime)) << L"No times added to update manager";

			time_t *updateTime = new time_t(CUtilities::GetCurrentTime());
			updateManager.AddPlayerUpdateTime(updateTime);
			ASSERT_FALSE(updateManager.CheckIfPlayerUpdateNeedsToBeSent(lastPlayerUpdateTime)) << L"current time entry should not have been added";

			time_t twoHourSpan(2 * SECONDS_PER_HOUR);
			updateTime = new time_t(CUtilities::GetCurrentTime() + twoHourSpan);
			updateManager.AddPlayerUpdateTime(updateTime);	// add a time that's two hours in the future;
			ASSERT_FALSE(updateManager.CheckIfPlayerUpdateNeedsToBeSent(lastPlayerUpdateTime)) << L"Future time should not trigger update";

			DWORD startTicks(CUtilities::GetTickCount());
			time_t startTime(CUtilities::GetCurrentTime());

			time_t twoSecondTimeSpan(2);
			time_t fourSecondTimeSpan(4);
			updateTime = new time_t(startTime + fourSecondTimeSpan);
			updateManager.AddPlayerUpdateTime(updateTime);	  //add a time that is about 3.001 to 4.000 seconds into the future
			updateTime = new time_t(startTime + twoSecondTimeSpan);
			updateManager.AddPlayerUpdateTime(updateTime);	  //add a time that is about 1.001 to 2.000 seconds into the future
			ASSERT_FALSE(updateManager.CheckIfPlayerUpdateNeedsToBeSent(lastPlayerUpdateTime)) <<
				AssertMsg::Format(__LINE__, nullptr,
					L"Near future time should not trigger update" // No comma.
					L" startTime=%lld currentTime=%lld startTicks=%u currentTicks=%u playerUpdateTimesRemaining=%d",
					(__int64)startTime, (__int64)CUtilities::GetCurrentTime(), (unsigned)startTicks, (unsigned)CUtilities::GetTickCount(), (int)updateManager.m_playerUpdateTimes.size());

			CUtilities::Sleep(2000 + SLEEP_ADD);	// Sleep to first expiration time
			// If we sleep into the second expiration time, CheckIfPlayerUpdateNeedsToBeSent()
			// will delete both expiration times in one call.
			ASSERT_TRUE(updateManager.CheckIfPlayerUpdateNeedsToBeSent(lastPlayerUpdateTime)) <<
				AssertMsg::Format(__LINE__, nullptr,
					L"First Time should have expired and triggered an update" // No comma.
					L" startTime=%lld currentTime=%lld startTicks=%u currentTicks=%u playerUpdateTimesRemaining=%d",
					(__int64)startTime, (__int64)CUtilities::GetCurrentTime(), (unsigned)startTicks, (unsigned)CUtilities::GetTickCount(), (int)updateManager.m_playerUpdateTimes.size());
			ASSERT_FALSE(updateManager.CheckIfPlayerUpdateNeedsToBeSent(lastPlayerUpdateTime)) <<
				AssertMsg::Format(__LINE__, nullptr,
					L"Second time should not have expired" // No comma.
					L" startTime=%lld currentTime=%lld startTicks=%u currentTicks=%u playerUpdateTimesRemaining=%d",
					(__int64)startTime, (__int64)CUtilities::GetCurrentTime(), (unsigned)startTicks, (unsigned)CUtilities::GetTickCount(), (int)updateManager.m_playerUpdateTimes.size());

			CUtilities::CUtilities::Sleep(2000 + SLEEP_ADD);	// Sleep to next expiration time
			ASSERT_TRUE(updateManager.CheckIfPlayerUpdateNeedsToBeSent(lastPlayerUpdateTime)) <<
				AssertMsg::Format(__LINE__, nullptr,
				L"seconds time should have expired and triggered an update" // No comma.
				L" startTime=%lld currentTime=%lld startTicks=%u currentTicks=%u playerUpdateTimesRemaining=%d",
				(__int64)startTime, (__int64)CUtilities::GetCurrentTime(), (unsigned)startTicks, (unsigned)CUtilities::GetTickCount(), (int)updateManager.m_playerUpdateTimes.size());
			ASSERT_FALSE(updateManager.CheckIfPlayerUpdateNeedsToBeSent(lastPlayerUpdateTime)) <<
				AssertMsg::Format(__LINE__, nullptr,
					L"no more items to expire" // No comma.
					L" startTime=%lld currentTime=%lld startTicks=%u currentTicks=%u playerUpdateTimesRemaining=%d",
					(__int64)startTime, (__int64)CUtilities::GetCurrentTime(), (unsigned)startTicks, (unsigned)CUtilities::GetTickCount(), (int)updateManager.m_playerUpdateTimes.size());
		}

		TEST(PlayerUpdateManager, PlayerUpdateIntervalTest)
		{
			CPlayerUpdateManager updateManager;

			time_t twoHourSpan(2 * SECONDS_PER_HOUR);

			time_t lastPlayerUpdateTime = CUtilities::GetUTCTime() - twoHourSpan;
			ASSERT_TRUE(updateManager.CheckIfPlayerUpdateNeedsToBeSent(lastPlayerUpdateTime)) << L"A last update from 2 hours ago should trigger an update";

			time_t oneSecondTimeSpan(1);
			time_t *updateTime = new time_t(CUtilities::GetCurrentTime() + oneSecondTimeSpan);
			CUtilities::CUtilities::Sleep(1000 + SLEEP_ADD);	// Sleep to expiration time
			ASSERT_TRUE(updateManager.CheckIfPlayerUpdateNeedsToBeSent(lastPlayerUpdateTime)) << L"Two reasons to trigger update";
			lastPlayerUpdateTime = CUtilities::GetUTCTime();
			ASSERT_FALSE(updateManager.CheckIfPlayerUpdateNeedsToBeSent(lastPlayerUpdateTime)) << L"Expired update should have been flushed";


		}

		/// <summary>
		/// Functional test for:
		///     CPlayerUpdateManager::AddPlayerUpdateTime()
		/// Test that trying to add a time in the past does not get added to the internal array.
		/// </summary>
		TEST(PlayerUpdateManager, AddPlayerUpdateTime0a)
		{
			// Setup.
			CPlayerUpdateManager updateManager;
			ASSERT_EQ(0, updateManager.m_playerUpdateTimes.size());

			// Perform operation(s) and test(s).
			time_t *updateTime = new time_t(CUtilities::GetCurrentTime() - 1);
			updateManager.AddPlayerUpdateTime(updateTime);
			ASSERT_EQ(0, updateManager.m_playerUpdateTimes.size());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerUpdateManager::AddPlayerUpdateTime()
		/// Test that trying to add the current time does not get added to the internal array.
		/// </summary>
		TEST(PlayerUpdateManager, AddPlayerUpdateTime0b)
		{
			// Setup.
			CPlayerUpdateManager updateManager;
			ASSERT_EQ(0, updateManager.m_playerUpdateTimes.size());

			// Perform operation(s) and test(s).
			time_t *updateTime = new time_t(CUtilities::GetCurrentTime() - 1);
			updateManager.AddPlayerUpdateTime(updateTime);
			ASSERT_EQ(0, updateManager.m_playerUpdateTimes.size());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerUpdateManager::AddPlayerUpdateTime()
		/// Test that adding a time in the future does get added to the internal array.
		/// </summary>
		TEST(PlayerUpdateManager, AddPlayerUpdateTime1)
		{
			// Setup.
			CPlayerUpdateManager updateManager;
			ASSERT_EQ(0, updateManager.m_playerUpdateTimes.size());

			// Perform operation(s) and test(s).
			time_t time100(CUtilities::GetCurrentTime() + 100);

			updateManager.AddPlayerUpdateTime(new time_t(time100));

			ASSERT_EQ(1, updateManager.m_playerUpdateTimes.size());
			ASSERT_EQ(time100, *(time_t *)*updateManager.m_playerUpdateTimes.cbegin());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerUpdateManager::AddPlayerUpdateTime()
		/// Test that adding the same time in the future only gets added to the internal array once.
		/// </summary>
		TEST(PlayerUpdateManager, AddPlayerUpdateTime11)
		{
			// Setup.
			CPlayerUpdateManager updateManager;
			ASSERT_EQ(0, updateManager.m_playerUpdateTimes.size());

			// Perform operation(s) and test(s).
			time_t time100(CUtilities::GetCurrentTime() + 100);

			updateManager.AddPlayerUpdateTime(new time_t(time100));
			updateManager.AddPlayerUpdateTime(new time_t(time100));

			ASSERT_EQ(1, updateManager.m_playerUpdateTimes.size());
			ASSERT_EQ(time100, *(time_t *)*updateManager.m_playerUpdateTimes.cbegin());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerUpdateManager::AddPlayerUpdateTime()
		/// Test that adding two times in the future (in order) get added to the internal array in the correct order.
		/// </summary>
		TEST(PlayerUpdateManager, AddPlayerUpdateTime2a)
		{
			// Setup.
			CPlayerUpdateManager updateManager;
			ASSERT_EQ(0, updateManager.m_playerUpdateTimes.size());

			// Perform operation(s) and test(s).
			time_t time100(CUtilities::GetCurrentTime() + 100);
			time_t time101(time100 + 1);

			updateManager.AddPlayerUpdateTime(new time_t(time100));
			updateManager.AddPlayerUpdateTime(new time_t(time101));

			ASSERT_EQ(2, updateManager.m_playerUpdateTimes.size());
			auto iter(updateManager.m_playerUpdateTimes.cbegin());
			ASSERT_EQ(time100, *(time_t *)*iter++);
			ASSERT_EQ(time101, *(time_t *)*iter++);
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerUpdateManager::AddPlayerUpdateTime()
		/// Test that adding two times in the future (in reverse order) get added to the internal array in the correct order.
		/// </summary>
		TEST(PlayerUpdateManager, AddPlayerUpdateTime2b)
		{
			// Setup.
			CPlayerUpdateManager updateManager;
			ASSERT_EQ(0, updateManager.m_playerUpdateTimes.size());

			// Perform operation(s) and test(s).
			time_t time100(CUtilities::GetCurrentTime() + 100);
			time_t time101(time100 + 1);

			updateManager.AddPlayerUpdateTime(new time_t(time101));
			updateManager.AddPlayerUpdateTime(new time_t(time100));

			ASSERT_EQ(2, updateManager.m_playerUpdateTimes.size());
			auto iter(updateManager.m_playerUpdateTimes.cbegin());
			ASSERT_EQ(time100, *(time_t *)*iter++);
			ASSERT_EQ(time101, *(time_t *)*iter++);
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerUpdateManager::AddPlayerUpdateTime()
		/// Test that adding three times in the future (where the third time is earliest) get added to the internal array in the correct order.
		/// </summary>
		TEST(PlayerUpdateManager, AddPlayerUpdateTime3a)
		{
			// Setup.
			CPlayerUpdateManager updateManager;
			ASSERT_EQ(0, updateManager.m_playerUpdateTimes.size());

			// Perform operation(s) and test(s).
			time_t time100(CUtilities::GetCurrentTime() + 100);
			time_t time101(time100 + 1);
			time_t time102(time100 + 2);

			updateManager.AddPlayerUpdateTime(new time_t(time101));
			updateManager.AddPlayerUpdateTime(new time_t(time102));
			updateManager.AddPlayerUpdateTime(new time_t(time100));

			ASSERT_EQ(3, updateManager.m_playerUpdateTimes.size());
			auto iter(updateManager.m_playerUpdateTimes.cbegin());
			ASSERT_EQ(time100, *(time_t *)*iter++);
			ASSERT_EQ(time101, *(time_t *)*iter++);
			ASSERT_EQ(time102, *(time_t *)*iter++);
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerUpdateManager::AddPlayerUpdateTime()
		/// Test that adding three times in the future (where the third time is in between) get added to the internal array in the correct order.
		/// </summary>
		TEST(PlayerUpdateManager, AddPlayerUpdateTime3b)
		{
			// Setup.
			CPlayerUpdateManager updateManager;
			ASSERT_EQ(0, updateManager.m_playerUpdateTimes.size());

			// Perform operation(s) and test(s).
			time_t time100(CUtilities::GetCurrentTime() + 100);
			time_t time101(time100 + 1);
			time_t time102(time100 + 2);

			updateManager.AddPlayerUpdateTime(new time_t(time100));
			updateManager.AddPlayerUpdateTime(new time_t(time102));
			updateManager.AddPlayerUpdateTime(new time_t(time101));

			ASSERT_EQ(3, updateManager.m_playerUpdateTimes.size());
			auto iter(updateManager.m_playerUpdateTimes.cbegin());
			ASSERT_EQ(time100, *(time_t *)*iter++);
			ASSERT_EQ(time101, *(time_t *)*iter++);
			ASSERT_EQ(time102, *(time_t *)*iter++);
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerUpdateManager::AddPlayerUpdateTime()
		/// Test that adding three times in the future (where the third time is latest) get added to the internal array in the correct order.
		/// </summary>
		TEST(PlayerUpdateManager, AddPlayerUpdateTime3c)
		{
			// Setup.
			CPlayerUpdateManager updateManager;
			ASSERT_EQ(0, updateManager.m_playerUpdateTimes.size());

			// Perform operation(s) and test(s).
			time_t time100(CUtilities::GetCurrentTime() + 100);
			time_t time101(time100 + 1);
			time_t time102(time100 + 2);

			updateManager.AddPlayerUpdateTime(new time_t(time100));
			updateManager.AddPlayerUpdateTime(new time_t(time101));
			updateManager.AddPlayerUpdateTime(new time_t(time102));

			ASSERT_EQ(3, updateManager.m_playerUpdateTimes.size());
			auto iter(updateManager.m_playerUpdateTimes.cbegin());
			ASSERT_EQ(time100, *(time_t *)*iter++);
			ASSERT_EQ(time101, *(time_t *)*iter++);
			ASSERT_EQ(time102, *(time_t *)*iter++);
		}

        TEST(PlayerUpdateManager, PlayerFlushIntervalTest)
		{
			CPlayerUpdateManager updateManager;

			time_t fiveMinutesSpan(5 * SECONDS_PER_MINUTE);

			time_t lastPlayerUpdateTime = CUtilities::GetUTCTime() - fiveMinutesSpan;
			ASSERT_TRUE(updateManager.CheckIfPlayerBeFlushed(lastPlayerUpdateTime)) << L"A last update from 5 minutes ago should flush an update";

			lastPlayerUpdateTime = CUtilities::GetUTCTime();
			ASSERT_FALSE(updateManager.CheckIfPlayerUpdateNeedsToBeSent(lastPlayerUpdateTime)) << L"Expired update should have been flushed";
		}

        		/// <summary>
		/// Functional test for:
		///     CPlayerUpdateManager::ClearPlayerUpdateTimes()
		/// Test to make sure update times list gets cleared properly.
		/// </summary>
		TEST(PlayerUpdateManager, ClearPlayerUpdateTimesTest)
		{
			// Setup.
			CPlayerUpdateManager updateManager;
			ASSERT_EQ(0, updateManager.m_playerUpdateTimes.size());

			// Perform operation(s) and test(s).
			updateManager.AddPlayerUpdateTime(new time_t(CUtilities::GetCurrentTime() + 1));
			updateManager.AddPlayerUpdateTime(new time_t(CUtilities::GetCurrentTime() + 2));
			ASSERT_EQ(2, updateManager.m_playerUpdateTimes.size());
			updateManager.ClearPlayerUpdateTimes();
			ASSERT_EQ(0, updateManager.m_playerUpdateTimes.size());
		}
