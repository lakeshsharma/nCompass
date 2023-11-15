#include "stdafx.h"
#include "TicketingInfo.h"

// Unit testing includes.
//#include "UnitTestingToString.h"

#include "Config.h"
#include "gtest/gtest.h"

namespace SentinelNativeUnitTests
{
		/// <summary>
		/// Helper method to test:
		///     CTicketingInfo::CTicketingInfo(const CConfig &);
		///     CTicketingInfo::GetExpirationInDays()
		/// <param name="expType">Value to set.</param>
		/// <param name="expDuration">Value to set.</param>
		/// <param name="expected">Value expected when getter is called.</param>
		/// </summary>
		void GetExpirationInDaysTest(TicketExpirationType expType, int expirationDuration, WORD expected)
		{
			// Setup.
			CConfig config(false, "");
			config.SetTicketExpirationType(expType);
			config.SetTicketExpirationDuration(expirationDuration);

			// Perform operation(s) and test(s).
			CTicketingInfo obj(config);
			ASSERT_EQ(expected, obj.GetExpirationInDays()) << "GetExpirationInDays()";
		}

		/// <summary>
		/// Functional test for:
		///     CSASSerialWrapper::SetTicketExpirationDuration()
		///     CSASSerialWrapper::GetTicketExpirationDuration()
		/// Test with a range of values with EXPIRES_SECONDS.
		/// </summary>
		TEST(TicketingInfo, GetExpirationInDays_SecondsTest)
		{
			GetExpirationInDaysTest(EXPIRES_SECONDS, 0, 1);
			GetExpirationInDaysTest(EXPIRES_SECONDS, SHRT_MAX, 1);
			GetExpirationInDaysTest(EXPIRES_SECONDS, WORD_MAX, 1);
			GetExpirationInDaysTest(EXPIRES_SECONDS, INT_MAX, 1);
			GetExpirationInDaysTest(EXPIRES_SECONDS, INT_MIN, 1);
		}

		/// <summary>
		/// Functional test for:
		///     CSASSerialWrapper::SetTicketExpirationDuration()
		///     CSASSerialWrapper::GetTicketExpirationDuration()
		/// Test with a range of values with EXPIRES_MINUTES.
		/// </summary>
		TEST(TicketingInfo, GetExpirationInDays_MinutesTest)
		{
			GetExpirationInDaysTest(EXPIRES_MINUTES, 0, 1);
			GetExpirationInDaysTest(EXPIRES_MINUTES, SHRT_MAX, 1);
			GetExpirationInDaysTest(EXPIRES_MINUTES, WORD_MAX, 1);
			GetExpirationInDaysTest(EXPIRES_MINUTES, INT_MAX, 1);
			GetExpirationInDaysTest(EXPIRES_MINUTES, INT_MIN, 1);
		}

		/// <summary>
		/// Functional test for:
		///     CSASSerialWrapper::SetTicketExpirationDuration()
		///     CSASSerialWrapper::GetTicketExpirationDuration()
		/// Test with a range of values with EXPIRES_HOURS.
		/// </summary>
		TEST(TicketingInfo, GetExpirationInDays_HoursTest)
		{
			GetExpirationInDaysTest(EXPIRES_HOURS, 0, 1);
			GetExpirationInDaysTest(EXPIRES_HOURS, 24, 1);
			GetExpirationInDaysTest(EXPIRES_HOURS, 48, 1);
			GetExpirationInDaysTest(EXPIRES_HOURS, 72, 1);
			GetExpirationInDaysTest(EXPIRES_HOURS, 96, 1);
			GetExpirationInDaysTest(EXPIRES_HOURS, SHRT_MAX, 1);
			GetExpirationInDaysTest(EXPIRES_HOURS, WORD_MAX, 1);
			GetExpirationInDaysTest(EXPIRES_HOURS, INT_MAX, 1);
			GetExpirationInDaysTest(EXPIRES_HOURS, INT_MIN, 1);
		}

		/// <summary>
		/// Functional test for:
		///     CSASSerialWrapper::SetTicketExpirationDuration()
		///     CSASSerialWrapper::GetTicketExpirationDuration()
		/// Test with a range of values with EXPIRES_DAYS.
		/// </summary>
		TEST(TicketingInfo, GetExpirationInDays_DaysTest)
		{
			GetExpirationInDaysTest(EXPIRES_DAYS, -3, 0xfffd);
			GetExpirationInDaysTest(EXPIRES_DAYS, -2, 0xfffe);
			GetExpirationInDaysTest(EXPIRES_DAYS, -1, 0xffff);
			GetExpirationInDaysTest(EXPIRES_DAYS, 0, 0);
			GetExpirationInDaysTest(EXPIRES_DAYS, 1, 1);
			GetExpirationInDaysTest(EXPIRES_DAYS, 2, 2);
			GetExpirationInDaysTest(EXPIRES_DAYS, 255, 255);
			GetExpirationInDaysTest(EXPIRES_DAYS, 256, 256);
			GetExpirationInDaysTest(EXPIRES_DAYS, 9998, 9998);
			GetExpirationInDaysTest(EXPIRES_DAYS, 9999, 9999);
			GetExpirationInDaysTest(EXPIRES_DAYS, 10000, 9999);
			GetExpirationInDaysTest(EXPIRES_DAYS, SHRT_MAX, 9999);
			GetExpirationInDaysTest(EXPIRES_DAYS, WORD_MAX, 9999);
			GetExpirationInDaysTest(EXPIRES_DAYS, INT_MAX, 9999);
			GetExpirationInDaysTest(EXPIRES_DAYS, INT_MIN, 0);
			GetExpirationInDaysTest(EXPIRES_DAYS, SHRT_MIN, 0x8000);
		}
		/// <summary>
		/// Functional test for:
		///     CSASSerialWrapper::SetTicketExpirationDuration()
		///     CSASSerialWrapper::GetTicketExpirationDuration()
		/// Test with a range of values with EXPIRES_YEARS.
		/// </summary>
		TEST(TicketingInfo, GetExpirationInDays_YearsTest)
		{
			GetExpirationInDaysTest(EXPIRES_YEARS, 0, 0);
			GetExpirationInDaysTest(EXPIRES_YEARS, 1, 0);
			GetExpirationInDaysTest(EXPIRES_YEARS, 2, 0);
			GetExpirationInDaysTest(EXPIRES_YEARS, 3, 0);
			GetExpirationInDaysTest(EXPIRES_YEARS, 4, 0);
			GetExpirationInDaysTest(EXPIRES_YEARS, SHRT_MAX, 0);
			GetExpirationInDaysTest(EXPIRES_YEARS, WORD_MAX, 0);
			GetExpirationInDaysTest(EXPIRES_YEARS, INT_MAX, 0);
			GetExpirationInDaysTest(EXPIRES_YEARS, INT_MIN, 0);
		}
// #pragma endregion GetExpirationInDays
}
