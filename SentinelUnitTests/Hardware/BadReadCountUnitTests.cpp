#include "stdafx.h"
#include "BadReadCount.h"
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"


class BadReadCountUnitTests : public ::testing::Test
{

protected:
	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

};
TEST_F(BadReadCountUnitTests, BadReadCountTest)
{
	CBadReadCount brc;

	brc.ResetBadReadCount();
	ASSERT_TRUE(brc.GetBadReadCount() == 0);

	brc.IncBadReadCount();
	ASSERT_TRUE(brc.GetBadReadCount() == 1);
	brc.IncBadReadCount();
	ASSERT_TRUE(brc.GetBadReadCount() == 2);
	brc.IncBadReadCount();
	brc.IncBadReadCount();
	ASSERT_FALSE(brc.IsTooManyBadCardReads());
	brc.IncBadReadCount();
	ASSERT_TRUE(brc.IsTooManyBadCardReads());

	brc.ResetBadReadCount();
	ASSERT_TRUE(brc.GetBadReadCount() == 0);

}

TEST_F(BadReadCountUnitTests, BadCardReadRemindersTest)
{
	CBadReadCount brc;
	brc.SetBadCardReadReminders(0);
	ASSERT_TRUE(brc.GetBadCardReadReminders() == 0);

	brc.SetBadCardReadReminders(10);
	ASSERT_TRUE(brc.GetBadCardReadReminders() == 10);

	brc.DecBadCardReadReminders();
	ASSERT_TRUE(brc.GetBadCardReadReminders() == 9);

}

TEST_F(BadReadCountUnitTests, BadCardReadReminderTickTest)
{

	CBadReadCount brc;
	// 1 remaining, in future
	brc.SetBadCardReadReminders(1);
	brc.SetBadCardReadReminderTick(CUtilities::GetTickCount() + 100000);
	ASSERT_FALSE(brc.IsBadCardReminderRequired());

	// None remaining, in future
	brc.DecBadCardReadReminders();
	ASSERT_FALSE(brc.IsBadCardReminderRequired());

	// None remaining, in past
	brc.SetBadCardReadReminderTick(CUtilities::GetTickCount() - 1000);
	ASSERT_FALSE(brc.IsBadCardReminderRequired());

	// 1 remaining, in past
	brc.SetBadCardReadReminders(1);
	ASSERT_TRUE(brc.IsBadCardReminderRequired());
}

