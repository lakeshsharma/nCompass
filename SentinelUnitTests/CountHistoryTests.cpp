#include "stdafx.h"
#include "CountHistory.h"
#include "TestUtil.h"
#include "Utility/AssertMsg.h"

#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	typedef struct _lastTimes{
		SYSTEMTIME lastSetTime;
		SYSTEMTIME lastResetTime;
		SYSTEMTIME lastRolledTime;
		SYSTEMTIME lastIntervalTime;
	} LastTimes;

	class CountHistoryTests : public ::testing::Test
	{    
		public:    
			void AssertCountHistory(CountHistory& history, CountHistory& historyOrg)
			{		
				ASSERT_TRUE(TestUtil::AreBitEqual(historyOrg.m_lastSetTime, history.GetLastSetTime())) << "GetLastSetTime()";
				ASSERT_TRUE(TestUtil::AreBitEqual(historyOrg.m_lastResetTime, history.GetLastResetTime())) << "GetLastResetTime()";
				ASSERT_TRUE(TestUtil::AreBitEqual(historyOrg.m_lastRolledTime, history.m_lastRolledTime)) << "m_lastRolledTime";
				ASSERT_TRUE(TestUtil::AreBitEqual(historyOrg.m_lastIntervalTime, history.m_lastIntervalTime)) << "m_lastIntervalTime";
			}

			void AssertCountHistoryAgainstTimes(CountHistory& history, LastTimes& lastTimes)
			{
				
				ASSERT_TRUE(TestUtil::AreBitEqual(history.m_lastSetTime, lastTimes.lastSetTime)) << "m_lastSetTime";
				ASSERT_TRUE(TestUtil::AreBitEqual(history.m_lastResetTime, lastTimes.lastResetTime)) << "m_lastResetTime";
				ASSERT_TRUE(TestUtil::AreBitEqual(history.m_lastRolledTime, lastTimes.lastRolledTime)) << "m_lastRolledTime";
				ASSERT_TRUE(TestUtil::AreBitEqual(history.m_lastIntervalTime, lastTimes.lastIntervalTime)) << "m_lastIntervalTime";

			}

			void AssertCountHistoryIndex(unsigned int exp, CountHistory& history, const int index)
			{				
				ASSERT_EQ(exp, history[index]) << "history[index]";
			}
			
			void setCountHistoryLastTime(CountHistory& history, SYSTEMTIME lastTime)
			{
				history.m_lastSetTime = lastTime;
			}

			void setCountHistoryLastResetTime(CountHistory& history, SYSTEMTIME lastResetTime)
			{
				history.m_lastResetTime = lastResetTime;
			}

			void setCountHistoryLastRolledTime(CountHistory& history, SYSTEMTIME lastRolledTime)
			{
				history.m_lastRolledTime	= lastRolledTime;
			}

			void setCountHistoryLastIntervalTime(CountHistory& history, SYSTEMTIME lastIntervalTime)
			{
				history.m_lastIntervalTime	= lastIntervalTime;
			}


			SYSTEMTIME getCountHistoryLastTime(CountHistory& history)
			{
				return history.m_lastSetTime;
			}

			SYSTEMTIME getCountHistoryLastResetTime(CountHistory& history)
			{
				return history.m_lastResetTime;
			}

			SYSTEMTIME getCountHistoryLastRolledTime(CountHistory& history)
			{
				return history.m_lastRolledTime;
			}

			SYSTEMTIME getCountHistoryLastIntervalTime(CountHistory& history)
			{
				return history.m_lastIntervalTime;
			}

			void setCountHistoryTimes(CountHistory& history, LastTimes &lastTimes)
			{
				setCountHistoryLastTime(history, lastTimes.lastSetTime);
				setCountHistoryLastResetTime(history, lastTimes.lastResetTime);
				setCountHistoryLastRolledTime(history, lastTimes.lastRolledTime);
				setCountHistoryLastIntervalTime(history, lastTimes.lastIntervalTime);
			}

			
			void setCountHistoryTodayData(CountHistory& historyOrg, const bool isSet, const unsigned count, const unsigned ms)
			{
				historyOrg.m_today.m_isSet = isSet;
				historyOrg.m_today.m_count =  count;
				historyOrg.m_today.m_milliseconds = ms;
			}
			
			void setCountHistoryYdayData(CountHistory& historyOrg, const bool isSet, const unsigned count, const unsigned ms)
			{
				historyOrg.m_yesterday.m_isSet = isSet;
				historyOrg.m_yesterday.m_count = count;
				historyOrg.m_yesterday.m_milliseconds = ms;
			}

			CountHistory::Data& getCountHistoryTodayData(CountHistory& historyOrg)
			{
				return historyOrg.m_today;
			}
			CountHistory::Data& getCountHistoryYdayData(CountHistory& historyOrg){
				return historyOrg.m_yesterday;
			}

			void setCountHistory(CountHistory& history, int day, unsigned count, unsigned ms, bool isSet){
				history.Set(day, count, ms, isSet);
			}
	};

	// These tests were converted from another test framework
	TEST_F(CountHistoryTests, CountHistory_Data_ctor_default)
	{
		CountHistory::Data data;

		ASSERT_EQ(0U, data.GetCount()) << "GetCount()";
		ASSERT_EQ(0U, data.GetMilliseconds()) << "GetMilliseconds()";
		ASSERT_FALSE(data.IsSet()) << "IsSet()";
	}

	TEST_F(CountHistoryTests, CountHistory_Data_ctor)
	{
		CountHistory::Data data(0x12345678, 0x87654321, true);

		ASSERT_EQ(0x12345678U, data.GetCount()) << "GetCount()";
		ASSERT_EQ(0x87654321U, data.GetMilliseconds()) << "GetMilliseconds()";
		ASSERT_TRUE(data.IsSet()) << "IsSet()";
	}

	TEST_F(CountHistoryTests, CountHistory_Data_isequaloperator_true)
	{
		CountHistory::Data data1(0x12345678, 0x87654321, true);
		CountHistory::Data data2(0x12345678, 0x87654321, true);

		ASSERT_TRUE(data1 == data2);
	}

	TEST_F(CountHistoryTests, CountHistory_Data_isequaloperator_false)
	{
		CountHistory::Data data1(0x12345678, 0x87654321, true);
		CountHistory::Data data2(0x12345678, 0x87654321, false);

		ASSERT_FALSE(data1 == data2);
	}

	TEST_F(CountHistoryTests, CountHistory_ctor)
	{
		CountHistory history;

		ASSERT_TRUE(TestUtil::IsZeroes(getCountHistoryLastTime(history))) << "m_lastSetTime";
		ASSERT_TRUE(TestUtil::IsZeroes(getCountHistoryLastResetTime(history))) << "m_lastResetTime";
		ASSERT_TRUE(TestUtil::IsZeroes(getCountHistoryLastRolledTime(history))) << "m_lastRolledTime";
		ASSERT_TRUE(TestUtil::IsZeroes(getCountHistoryLastIntervalTime(history))) << "m_lastIntervalTime";
	}

	TEST_F(CountHistoryTests, CountHistory_cctor)
	{
		const SYSTEMTIME lastSetTime		= { 2001, 2, 0, 3, 12, 12, 56, 123 };
		const SYSTEMTIME lastResetTime		= { 2001, 2, 0, 3, 12, 23, 45, 456 };
		const SYSTEMTIME lastRolledTime		= { 2001, 2, 0, 3, 12, 34, 34, 789 };
		const SYSTEMTIME lastIntervalTime	= { 2001, 2, 0, 3, 12, 56, 23, 147 };

		LastTimes lastTimes = {lastSetTime, lastResetTime, lastRolledTime, lastIntervalTime};
		
		CountHistory historyOrg;
		
		setCountHistoryTodayData(historyOrg, true, 0x87654321, 0x12345678);
		setCountHistoryYdayData(historyOrg, false, 0x10101010, 0xaa55aa55);
		
		setCountHistoryTimes(historyOrg, lastTimes);
		
		CountHistory history(historyOrg);

		ASSERT_TRUE(getCountHistoryTodayData(history) == getCountHistoryTodayData(historyOrg)) << "m_today";
        ASSERT_TRUE(getCountHistoryYdayData(history) == getCountHistoryYdayData(historyOrg)) << "m_yesterday";

		AssertCountHistory(history, historyOrg);

	}

	TEST_F(CountHistoryTests, CountHistory_assignment)
	{
		const SYSTEMTIME lastSetTime		= { 2001, 2, 0, 3, 12, 12, 56, 123 };
		const SYSTEMTIME lastResetTime		= { 2001, 2, 0, 3, 12, 23, 45, 456 };
		const SYSTEMTIME lastRolledTime		= { 2001, 2, 0, 3, 12, 34, 34, 789 };
		const SYSTEMTIME lastIntervalTime	= { 2001, 2, 0, 3, 12, 56, 23, 147 };

		LastTimes lastTimes = {lastSetTime, lastResetTime, lastRolledTime, lastIntervalTime};
		
		CountHistory historyOrg;
		
		setCountHistoryTodayData(historyOrg, true, 0x87654321, 0x12345678);
		setCountHistoryYdayData(historyOrg, false, 0x10101010, 0xaa55aa55);
		
		setCountHistoryTimes(historyOrg, lastTimes);
		
		CountHistory history;
		history = historyOrg;

		ASSERT_TRUE(getCountHistoryTodayData(history) == getCountHistoryTodayData(historyOrg)) << "m_today";
        ASSERT_TRUE(getCountHistoryYdayData(history) == getCountHistoryYdayData(historyOrg)) << "m_yesterday";

		AssertCountHistory(history, historyOrg);
	}

	TEST_F(CountHistoryTests, CountHistory_IsLastSetTimeValid_false)
	{
		CountHistory history;
		ASSERT_FALSE(history.IsLastSetTimeValid());
	}

	TEST_F(CountHistoryTests, CountHistory_IsLastResetTimeValid_false)
	{
		CountHistory history;
		ASSERT_FALSE(history.IsLastResetTimeValid());
	}

	TEST_F(CountHistoryTests, CountHistory_IsLastSetTimeValid_true)
	{
		const SYSTEMTIME lastTime = { 2001, 2, 0, 3, 12, 12, 56, 123 };

		CountHistory history;
		setCountHistoryLastTime(history, lastTime);
			
		ASSERT_TRUE(history.IsLastSetTimeValid());
	}

	TEST_F(CountHistoryTests, CountHistory_IsLastResetTimeValid_true)
	{
		const SYSTEMTIME lastTime = { 2001, 2, 0, 3, 12, 12, 56, 123 };

		CountHistory history;
		setCountHistoryLastResetTime(history, lastTime);
		
		ASSERT_FALSE(history.IsLastSetTimeValid());
	}

	TEST_F(CountHistoryTests, CountHistory_GetData_nullptr_nullptr)
	{
		SYSTEMTIME lastSetTime		= { 2014, 12, 3, 31, 23, 59, 58, 999 };
		SYSTEMTIME lastResetTime	= { 2014, 12, 3, 31,  1,  2,  3,   0 };
		SYSTEMTIME lastRolledTime	= { 2014, 12, 3, 31,  0,  1,  0,   0 };
		SYSTEMTIME lastIntervalTime	= {    0,  0, 0,  0,  0,  0,  0,   0 };
		SYSTEMTIME curTime			= { 2014, 12, 3, 31, 23, 59, 59,   0 };

		LastTimes lastTimes = {lastSetTime, lastResetTime, lastRolledTime, lastIntervalTime};
		
		CountHistory history;
		
		setCountHistoryTimes(history, lastTimes);
		
    	history.GetData(nullptr, nullptr, &curTime);
		
		AssertCountHistoryAgainstTimes(history, lastTimes);
	}

	TEST_F(CountHistoryTests, CountHistory_GetData_today_nullptr)
	{
		SYSTEMTIME lastSetTime		= { 2014, 12, 3, 31, 23, 59, 58, 999 };
		SYSTEMTIME lastResetTime	= { 2014, 12, 3, 31,  1,  2,  3,   0 };
		SYSTEMTIME lastRolledTime	= { 2014, 12, 3, 31,  0,  1,  0,   0 };
		SYSTEMTIME lastIntervalTime = {    0,  0, 0,  0,  0,  0,  0,   0 };
		SYSTEMTIME curTime			= { 2014, 12, 3, 31, 23, 59, 59,   0 };
		
		LastTimes lastTimes = {lastSetTime, lastResetTime, lastRolledTime, lastIntervalTime};

		CountHistory history;

		setCountHistoryTimes(history, lastTimes);

		CountHistory::Data today;
		history.GetData(&today, nullptr, &curTime);

		AssertCountHistoryAgainstTimes(history, lastTimes);

		ASSERT_EQ(0U, today.GetCount()) << "today.GetCount()";
		ASSERT_EQ(0U, today.GetMilliseconds()) << "today.GetMilliseconds()";
		ASSERT_FALSE(today.IsSet()) << "today.IsSet()";
	}

	TEST_F(CountHistoryTests, CountHistory_GetData_nullptr_yesterday)
	{
		SYSTEMTIME lastSetTime		= { 2014, 12, 3, 31, 23, 59, 58, 999 };
		SYSTEMTIME lastResetTime	= { 2014, 12, 3, 31,  1,  2,  3,   0 };
		SYSTEMTIME lastRolledTime	= { 2014, 12, 3, 31,  0,  1,  0,   0 };
		SYSTEMTIME lastIntervalTime	= {    0,  0, 0,  0,  0,  0,  0,   0 };
		SYSTEMTIME curTime			= { 2014, 12, 3, 31, 23, 59, 59,   0 };

		LastTimes lastTimes = {lastSetTime, lastResetTime, lastRolledTime, lastIntervalTime};

		CountHistory history;

		setCountHistoryTimes(history, lastTimes);

		CountHistory::Data yesterday;
		history.GetData(nullptr, &yesterday, &curTime);

		AssertCountHistoryAgainstTimes(history, lastTimes);

		ASSERT_EQ(0U, yesterday.GetCount()) << "yesterday.GetCount()";
		ASSERT_EQ(0U, yesterday.GetMilliseconds()) << "yesterday.GetMilliseconds()";
		ASSERT_FALSE(yesterday.IsSet()) << "yesterday.IsSet()";
	}

	TEST_F(CountHistoryTests, CountHistory_GetData)
	{
		const SYSTEMTIME lastSetTime		= { 2014, 12, 3, 31, 23, 59, 58, 999 };
		const SYSTEMTIME lastResetTime		= { 2014, 12, 3, 31,  1,  2,  3,   0 };
		const SYSTEMTIME lastRolledTime		= { 2014, 12, 3, 31,  0,  1,  0,   0 };
		const SYSTEMTIME lastIntervalTime	= {    0,  0, 0,  0,  0,  0,  0,   0 };
		const SYSTEMTIME curTime			= { 2014, 12, 3, 31, 23, 59, 59,   0 };

		LastTimes lastTimes = {lastSetTime, lastResetTime, lastRolledTime, lastIntervalTime};

		CountHistory history;

		setCountHistoryTimes(history, lastTimes);

		CountHistory::Data today;
		CountHistory::Data yesterday;

		history.GetData(&today, &yesterday, &curTime);

		AssertCountHistoryAgainstTimes(history, lastTimes);

		ASSERT_EQ(0U, today.GetCount()) << "today.GetCount()";
		ASSERT_EQ(0U, today.GetMilliseconds()) << "today.GetMilliseconds()";
		ASSERT_FALSE(today.IsSet()) << "today.IsSet()";

		ASSERT_EQ(0U, yesterday.GetCount()) << "yesterday.GetCount()";
		ASSERT_EQ(0U, yesterday.GetMilliseconds()) << "yesterday.GetMilliseconds()";
		ASSERT_FALSE(yesterday.IsSet()) << "yesterday.IsSet()";
	}

	/*This test case has been rewritten in different way. I re-enabled it just as it looks easier 
	to check different kind of data at once. So simple to add any new data and shoot for test*/
	TEST_F(CountHistoryTests, CountHistory_Set_series2)
	{
	
	   struct ExpectedData
	   {
		   bool isSet;
		   uint32_t count;
		   uint32_t milliseconds;
	   };
	   
	   struct
	   {
		   int line;
		   bool set;
		   SYSTEMTIME setTime;
		   SYSTEMTIME getTime;
		   SYSTEMTIME expectedSetTime;
		   SYSTEMTIME expectedResetTime;
		   ExpectedData expectedToday;
		   ExpectedData expectedYesterday;

		   void Test(CountHistory& history)
		   {
			   history.Set(set, &setTime);

			   CountHistory::Data today;
			   CountHistory::Data yesterday;

			   history.GetData(&today, &yesterday, &getTime);

			   ASSERT_EQ(expectedToday.isSet, today.IsSet()) << AssertMsg::Format(line) << "today.IsSet()";
			   ASSERT_EQ(expectedToday.count, today.GetCount()) << AssertMsg::Format(line) << "today.GetCount()";
			   ASSERT_EQ(expectedToday.milliseconds, today.GetMilliseconds()) << AssertMsg::Format(line) << "today.GetMilliseconds()";

			   ASSERT_EQ(expectedYesterday.isSet, yesterday.IsSet()) << AssertMsg::Format(line) << "yesterday.IsSet()";
			   ASSERT_EQ(expectedYesterday.count, yesterday.GetCount()) << AssertMsg::Format(line) << "yesterday.GetCount()";
			   ASSERT_EQ(expectedYesterday.milliseconds, yesterday.GetMilliseconds()) << AssertMsg::Format(line) << "yesterday.GetMilliseconds()";

			   ASSERT_TRUE(TestUtil::AreBitEqual(expectedSetTime, history.GetLastSetTime())) << AssertMsg::Format(line) << "GetLastSetTime()";
			   ASSERT_TRUE(TestUtil::AreBitEqual(expectedResetTime, history.GetLastResetTime())) << AssertMsg::Format(line) << "GetLastResetTime()";
		   }
	   }data[] =
	   {
		   { __LINE__,  true,
		   { 2014, 12, 3, 31, 23, 59, 58, 999 },
		   { 2014, 12, 3, 31, 23, 59, 59,   0 },
		   { 2014, 12, 3, 31, 23, 59, 58, 999 }, 
		   {    0,  0, 0,  0,  0,  0,  0,   0 },
		   {  true, 1, 1 },
		   { false, 0, 0 }
		   },
		   
		   { __LINE__,  true,
		   { 2014, 12, 3, 31, 23, 59, 59,   1 },
		   { 2014, 12, 3, 31, 23, 59, 59,   2 },
		   { 2014, 12, 3, 31, 23, 59, 58, 999 }, 
		   {    0,  0, 0,  0,  0,  0,  0,   0 },
		   {  true, 1, 3 },
		   { false, 0, 0 }
		   },
		   
		   { __LINE__,  false,
		   { 2014, 12, 3, 31, 23, 59, 59, 499 },
		   { 2014, 12, 3, 31, 23, 59, 59, 750 },
		   { 2014, 12, 3, 31, 23, 59, 58, 999 }, 
		   { 2014, 12, 3, 31, 23, 59, 59, 499 },
		   { false, 1, 500 },
		   { false, 0,   0 }
		   },
	   };

	   	CountHistory history;
	   	data[0].Test(history);
		data[1].Test(history);
		data[2].Test(history);
	}
	
	///	<summary>
	/// Verify day data.
	///	</summary>
	void VerifyData(int line, CountHistory::Data& data, bool expectedIsSet, uint32_t expectedCount, uint32_t expectedTime)
	{
		ASSERT_EQ(expectedIsSet, data.IsSet()) << AssertMsg::Format(line, L"IsSet()");
		ASSERT_EQ(expectedCount, data.GetCount()) << AssertMsg::Format(line, L"GetCount()");
		ASSERT_EQ(expectedTime,  data.GetMilliseconds()) << AssertMsg::Format(line, L"GetMilliseconds()");
	}

	///	<summary>
	/// Verify times.
	///	</summary>
	void VerifyTime(int line, CountHistory& history, const SYSTEMTIME& expectedSetTime, const SYSTEMTIME& expectedResetTime)
	{
		ASSERT_TRUE(TestUtil::AreBitEqual(expectedSetTime, history.GetLastSetTime())) << AssertMsg::Format(line, L"GetLastSetTime()");
		ASSERT_TRUE(TestUtil::AreBitEqual(expectedResetTime, history.GetLastResetTime())) << AssertMsg::Format(line, L"GetLastResetTime()");
	}

	TEST_F(CountHistoryTests, CountHistory_Set_series)
	{
		CountHistory history;
		CountHistory::Data today;
		CountHistory::Data yesterday;

		const SYSTEMTIME setTime1   = { 2014, 12, 3, 31, 23, 59, 58, 999 };
		const SYSTEMTIME getTime1   = { 2014, 12, 3, 31, 23, 59, 59,   0 };
		const SYSTEMTIME resetTime1 = {    0,  0, 0,  0,  0,  0,  0,   0 };

		history.Set(&setTime1);
		history.GetData(&today, &yesterday, &getTime1);
		VerifyData(__LINE__, today, true, 1, 1);
		VerifyData(__LINE__, yesterday, false, 0, 0);
		VerifyTime(__LINE__, history, setTime1, resetTime1);

		const SYSTEMTIME setTime2 = { 2014, 12, 3, 31, 23, 59, 58,   1 };
		const SYSTEMTIME getTime2 = { 2014, 12, 3, 31, 23, 59, 59,   2 };

		history.Set(&setTime2);
		history.GetData(&today, &yesterday, &getTime2);
		VerifyData(__LINE__, today, true, 1, 3);
		VerifyData(__LINE__, yesterday, false, 0, 0);
		VerifyTime(__LINE__, history, setTime1, resetTime1);

		const SYSTEMTIME resetTime3 = { 2014, 12, 3, 31, 23, 59, 59, 499 };
		const SYSTEMTIME getTime3   = { 2014, 12, 3, 31, 23, 59, 59, 750 };

		history.Reset(&resetTime3);
		history.GetData(&today, &yesterday, &getTime3);
		VerifyData(__LINE__, today, false, 1, 500);
		VerifyData(__LINE__, yesterday, false, 0, 0);
		VerifyTime(__LINE__, history, setTime1, resetTime3);

		const SYSTEMTIME setTime4 = { 2015,  1, 4,  1,  0,  0,  0, 500 };
		const SYSTEMTIME getTime4 = { 2015,  1, 4,  1,  0,  0, 10,   0 };

		history.Set(true, &setTime4);
		history.GetData(&today, &yesterday, &getTime4);
		VerifyData(__LINE__, today, true, 1, 9500);
		VerifyData(__LINE__, yesterday, false, 1, 500);
		VerifyTime(__LINE__, history, setTime4, resetTime3);

		const SYSTEMTIME getTime5 = { 2015,  1, 4,  2,  0,  0,  0,   0 };

		history.GetData(&today, &yesterday, &getTime5);
		VerifyData(__LINE__, today, true, 0, 0);
		VerifyData(__LINE__, yesterday, true, 1, 24 * 60 * 60 * 1000 - 500);
		VerifyTime(__LINE__, history, setTime4, resetTime3);

		const SYSTEMTIME resetTime6 = { 2015,  1, 4,  2,  0,  0,  0,   1 };
		const SYSTEMTIME getTime6   = { 2015,  1, 5,  3,  0,  0,  0,   0 };

		history.Set(false, &resetTime6);
		history.GetData(&today, &yesterday, &getTime6);

		SYSTEMTIME time = { 2015,  1, 6,  4,  12,  0,  0,  0 };

		for (int i = 0; i < 10; i++)
		{
			history.Set(&time);
			time.wMilliseconds += 10;
			history.Reset(&time);
			time.wMilliseconds += 10;
		}

		history.GetData(&today, nullptr, &time);
		VerifyData(__LINE__, today, false, 10, 100);

		time.wMilliseconds -= 10;
		SYSTEMTIME resetTime7 = time;
		time.wMilliseconds -= 10;
		VerifyTime(__LINE__, history, time, resetTime7);
	}

	TEST_F(CountHistoryTests, CountHistory_CodeCoverage)
	{
		CountHistory history;
		history.Set(true);
		history.Set(false);

		CountHistory::Data today;
		CountHistory::Data yesterday;
		history.GetData(&today, &yesterday);
		history.GetDataNoRoll(&today, &yesterday);
	}

	TEST_F(CountHistoryTests, CountHistory_Set_4params_0)
	{
		CountHistory history;
		setCountHistory(history, 0, 1, 2, true);

		ASSERT_EQ(1U, getCountHistoryTodayData(history).GetCount()) << "m_today.GetCount()";
		ASSERT_EQ(0U, getCountHistoryYdayData(history).GetCount()) << "m_yesterday.GetCount()";

		ASSERT_EQ(2U, getCountHistoryTodayData(history).GetMilliseconds()) << "m_today.GetMilliseconds()";
		ASSERT_EQ(0U, getCountHistoryYdayData(history).GetMilliseconds()) << "m_yesterday.GetMilliseconds()";

		ASSERT_TRUE(getCountHistoryTodayData(history).IsSet()) << "m_today.IsSet()";
		ASSERT_FALSE(getCountHistoryYdayData(history).IsSet()) << "m_yesterday.IsSet()";
	}

	TEST_F(CountHistoryTests, CountHistory_Set_4params_1)
	{
		CountHistory history;
		setCountHistory(history, 1, 1, 2, true);

		ASSERT_EQ(0U, getCountHistoryTodayData(history).GetCount()) << "m_today.GetCount()";
		ASSERT_EQ(1U, getCountHistoryYdayData(history).GetCount()) << "m_yesterday.GetCount()";

		ASSERT_EQ(0U, getCountHistoryTodayData(history).GetMilliseconds()) << "m_today.GetMilliseconds()";
		ASSERT_EQ(2U, getCountHistoryYdayData(history).GetMilliseconds()) << "m_yesterday.GetMilliseconds()";

		ASSERT_FALSE(getCountHistoryTodayData(history).IsSet()) << "m_today.IsSet()";
		ASSERT_TRUE(getCountHistoryYdayData(history).IsSet()) << "m_yesterday.IsSet()";
	}

	TEST_F(CountHistoryTests, CountHistory_Set_4params_2)
	{
		CountHistory history;
		setCountHistory(history, 2, 1, 2, true);

		ASSERT_EQ(0U, getCountHistoryTodayData(history).GetCount()) << "m_today.GetCount()";
		ASSERT_EQ(0U, getCountHistoryYdayData(history).GetCount()) << "m_yesterday.GetCount()";

		ASSERT_EQ(0U, getCountHistoryTodayData(history).GetMilliseconds()) << "m_today.GetMilliseconds()";
		ASSERT_EQ(0U, getCountHistoryYdayData(history).GetMilliseconds()) << "m_yesterday.GetMilliseconds()";

		ASSERT_FALSE(getCountHistoryTodayData(history).IsSet()) << "m_today.IsSet()";
		ASSERT_FALSE(getCountHistoryYdayData(history).IsSet()) << "m_yesterday.IsSet()";
	}

	TEST_F(CountHistoryTests, CountHistory_indexoperator_0)
	{
		CountHistory history;
		setCountHistory(history, 0, 1, 2, true);
		AssertCountHistoryIndex(1U, history, 0);
	}

	TEST_F(CountHistoryTests, CountHistory_indexoperator_1)
	{
		CountHistory history;
		setCountHistory(history, 1, 1, 2, true);
		AssertCountHistoryIndex(1U, history, 1);
	}

	TEST_F(CountHistoryTests, CountHistory_indexoperator_2)
	{
		CountHistory history;
		AssertCountHistoryIndex(0U, history, 2);
	}

}


