#include "stdafx.h"
#include "MultiplierBonusEvent.h"
#include "OneTimeBonusEvent.h"
#include "Utilities.h"
#include "Hardware/NonVolatileMemoryMaps.h"
#include "gtest/gtest.h"


CBonusEvent * NewBonus(long id, time_t start, time_t stop, WORD value, BonusEventType type)
{
	CBonusEvent *event = NULL;
	switch (type)
	{
	case ONE_TIME_BONUS:
		event = new COneTimeBonusEvent(id, start, stop, value);
		break;
	case MULTIPLIER_BONUS:
		event = new CMultiplierBonusEvent(id, start, stop, value);
		break;
	}

	return event;
}

class BonusEventUnitTests : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

};

TEST_F(BonusEventUnitTests, BonusEventConstructorUnitTest)
{
	long id = 1234;
	time_t start = CUtilities::GetCurrentTime();
	time_t stop = CUtilities::GetCurrentTime() + 600;
	WORD multiplier = 10;

	CBonusEvent *mBonus = NewBonus(id, start, stop, multiplier, MULTIPLIER_BONUS);

	EXPECT_EQ(mBonus->GetEventID(), id);
	EXPECT_EQ(mBonus->GetStartTime(), start);
	EXPECT_EQ(mBonus->GetStopTime(), stop);
	EXPECT_EQ(mBonus->GetBonusEventType(), MULTIPLIER_BONUS);

	WORD points = 100;

	CBonusEvent *oBonus = NewBonus(id, start, stop, points, ONE_TIME_BONUS);

	EXPECT_EQ(oBonus->GetEventID(), id);
	EXPECT_EQ(oBonus->GetStartTime(), start);
	EXPECT_EQ(oBonus->GetStopTime(), stop);
	EXPECT_EQ(oBonus->GetBonusEventType(), ONE_TIME_BONUS);

	delete mBonus;
	delete oBonus;
}

TEST_F(BonusEventUnitTests, BonusEventCopyConstructorUnitTest)
{
	long id = 1234;
	time_t start = CUtilities::GetCurrentTime();
	time_t stop = CUtilities::GetCurrentTime() + 600;
	WORD multiplier = 10;

	CMultiplierBonusEvent *mBonus = (CMultiplierBonusEvent *)NewBonus(id, start, stop, multiplier, MULTIPLIER_BONUS);
	CBonusEvent *cmBonus = new CMultiplierBonusEvent(*mBonus);

	EXPECT_EQ(cmBonus->GetEventID(), id);
	EXPECT_EQ(cmBonus->GetStartTime(), start);
	EXPECT_EQ(cmBonus->GetStopTime(), stop);
	EXPECT_EQ(cmBonus->GetBonusEventType(), MULTIPLIER_BONUS);

	WORD points = 100;

	COneTimeBonusEvent *oBonus = (COneTimeBonusEvent *)NewBonus(id, start, stop, points, ONE_TIME_BONUS);
	CBonusEvent *coBonus = new COneTimeBonusEvent(*oBonus);

	EXPECT_EQ(coBonus->GetEventID(), id);
	EXPECT_EQ(coBonus->GetStartTime(), start);
	EXPECT_EQ(coBonus->GetStopTime(), stop);
	EXPECT_EQ(coBonus->GetBonusEventType(), ONE_TIME_BONUS);

	delete mBonus;
	delete oBonus;
	delete cmBonus;
	delete coBonus;
}

TEST_F(BonusEventUnitTests, MultiplierBonusEventConstructorFromNvramTest)
{
	//Test Data
	int eventId = 1234;
	time_t start(1382716381);
	time_t stop (1382716981);
	byte NVRAM_DATA[NVRAM_PLAYER_BONUS_EVENT_SIZE] =
	{
		0x01,0x00,0x00,0x00,0xd2,0x04,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0xdd,0x07,0x00,0x00,0x0a,0x00,0x00,
		0x00,0x19,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0x35,
		0x00,0x00,0x00,0x01,0x00,0x00,0x00,0xdd,0x07,0x00,
		0x00,0x0a,0x00,0x00,0x00,0x19,0x00,0x00,0x00,0x10,
		0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x01,0x00,0x00,
		0x00,0xcd,0x0a,0x00,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,
		0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc
	};

	//Setup
	CMultiplierBonusEvent* bonusEventFromNvram(new CMultiplierBonusEvent(NVRAM_DATA));

	//Tests
	EXPECT_EQ(bonusEventFromNvram->GetEventID(), eventId);
	EXPECT_EQ(bonusEventFromNvram->GetStartTime(), start);
	EXPECT_EQ(bonusEventFromNvram->GetStopTime(), stop);
	EXPECT_EQ(bonusEventFromNvram->GetBonusEventType(), MULTIPLIER_BONUS);

	//Cleanup
	delete bonusEventFromNvram;
}

TEST_F(BonusEventUnitTests, BonusEventTestEventTimes)
{
	long id = 1234;
	time_t zero = 0;
	WORD points = 100;

	// EVENT ACTIVE	
	time_t start = CUtilities::GetCurrentTime() - 60;
	time_t stop = CUtilities::GetCurrentTime() + 600;
	CBonusEvent *bonus = NewBonus(id, start, stop, points, ONE_TIME_BONUS);
	EXPECT_TRUE(bonus->IsCurrentlyInEventTime());
	delete bonus;

	// EVENT NOT ACTIVE
	start = CUtilities::GetCurrentTime() + 540;
	bonus = NewBonus(id, start, stop, points, ONE_TIME_BONUS);
	EXPECT_FALSE(bonus->IsCurrentlyInEventTime());
	delete bonus;

	start = CUtilities::GetCurrentTime() - 600;
	stop = CUtilities::GetCurrentTime() - 300;
	bonus = NewBonus(id, start, stop, points, ONE_TIME_BONUS);
	EXPECT_FALSE(bonus->IsCurrentlyInEventTime());
	delete bonus;
}

TEST_F(BonusEventUnitTests, OneTimeBonusEventAwardPoints)
{
	long id = 1234;
	WORD points = 100;

	// EVENT ACTIVE	
	time_t start = CUtilities::GetCurrentTime() - 60;
	time_t stop = CUtilities::GetCurrentTime() + 600;
	COneTimeBonusEvent *event = (COneTimeBonusEvent *)NewBonus(id, start, stop, points, ONE_TIME_BONUS);

	// Award points
	EXPECT_FALSE(event->WerePointsAwared());
	EXPECT_TRUE(event->AwardPoints(points));
	EXPECT_TRUE(event->WerePointsAwared());
	EXPECT_EQ(event->GetAwardedPoints(), points);

	// Try to award points again
	EXPECT_FALSE(event->AwardPoints(points));
	EXPECT_EQ(event->GetAwardedPoints(), points);

	delete event;
}

TEST_F(BonusEventUnitTests, OneTimeBonusEventAwardPointsEventNotActive)
{
	long id = 1234;
	WORD points = 100;

	// EVENT NOT ACTIVE
	time_t start = CUtilities::GetCurrentTime() + 540;
	time_t stop = CUtilities::GetCurrentTime() + 600;
	COneTimeBonusEvent *event = (COneTimeBonusEvent *)NewBonus(id, start, stop, points, ONE_TIME_BONUS);

	// Award points
	EXPECT_FALSE(event->WerePointsAwared());
	EXPECT_FALSE(event->AwardPoints(points));
	EXPECT_FALSE(event->WerePointsAwared());
	EXPECT_EQ(event->GetAwardedPoints(), 0);

	delete event;
}

TEST_F(BonusEventUnitTests, OneTimeBonusEventAwardPartialPoints)
{
	long id = 1234;
	WORD points = 100;

	// EVENT ACTIVE	
	time_t start = CUtilities::GetCurrentTime() - 60;
	time_t stop = CUtilities::GetCurrentTime() + 600;
	COneTimeBonusEvent *event = (COneTimeBonusEvent *)NewBonus(id, start, stop, points, ONE_TIME_BONUS);

	// Award partial points, shouldn't work
	EXPECT_FALSE(event->WerePointsAwared());
	EXPECT_FALSE(event->AwardPartialPoints(10, 2, 3));
	EXPECT_FALSE(event->WerePointsAwared());
	EXPECT_TRUE(event->GetAwardedPoints() == 0);

	delete event;
}

TEST_F(BonusEventUnitTests, MultiplierBonusEventAwardPoints)
{
	long id = 1234;
	WORD points = 10;
	WORD multiplier = 2;

	// EVENT ACTIVE	
	time_t start = CUtilities::GetCurrentTime() - 60;
	time_t stop = CUtilities::GetCurrentTime() + 600;
	CMultiplierBonusEvent *event = (CMultiplierBonusEvent *)NewBonus(id, start, stop, multiplier, MULTIPLIER_BONUS);

	// Award points
	EXPECT_FALSE(event->WerePointsAwared());
	EXPECT_TRUE(event->AwardPoints(points));
	EXPECT_TRUE(event->WerePointsAwared());
	EXPECT_EQ(event->GetAwardedPoints(), points * multiplier);

	// Try to award points again
	EXPECT_TRUE(event->AwardPoints(points));
	EXPECT_EQ(event->GetAwardedPoints(), 2 * points * multiplier);

	delete event;
}

TEST_F(BonusEventUnitTests, MultiplierEventAwardPointsEventNotActive)
{
	long id = 1234;
	WORD points = 10;
	WORD multiplier = 2;

	// EVENT NOT ACTIVE
	time_t start = CUtilities::GetCurrentTime() + 540;
	time_t stop = CUtilities::GetCurrentTime() + 600;
	CMultiplierBonusEvent *event = (CMultiplierBonusEvent *)NewBonus(id, start, stop, multiplier, MULTIPLIER_BONUS);

	// Award points
	EXPECT_FALSE(event->WerePointsAwared());
	EXPECT_FALSE(event->AwardPoints(points));
	EXPECT_FALSE(event->WerePointsAwared());
	EXPECT_EQ(event->GetAwardedPoints(), 0);

	delete event;
}

TEST_F(BonusEventUnitTests, MultiplierBonusEventAwardPartialPoints)
{
	long id = 1234;
	WORD points = 10;
	DWORD countdown = 5;
	DWORD divisor = 3;
	WORD multiplier = 2;

	// EVENT ACTIVE	
	time_t start = CUtilities::GetCurrentTime() - 60;
	time_t stop = CUtilities::GetCurrentTime() + 600;
	CMultiplierBonusEvent *event = (CMultiplierBonusEvent *)NewBonus(id, start, stop, multiplier, MULTIPLIER_BONUS);

	// Award partial points
	EXPECT_FALSE(event->WerePointsAwared());
	EXPECT_TRUE(event->AwardPartialPoints(countdown, divisor, multiplier));
	EXPECT_TRUE(event->WerePointsAwared());
	int awardedPoints = event->GetAwardedPoints();
	EXPECT_EQ(awardedPoints, countdown * multiplier * multiplier / divisor);


	// Award partial again to see if previous remainder (1) is accounted for
	multiplier = 1;
	countdown = 1;
	EXPECT_TRUE(event->AwardPartialPoints(countdown, divisor, multiplier));
	EXPECT_EQ(event->GetAwardedPoints(), awardedPoints + 1);

	delete event;
}

TEST_F(BonusEventUnitTests, MultiplierBonusEventAwardPartialPointsEventNotActive)
{
	long id = 1234;
	WORD points = 10;
	DWORD countdown = 5;
	DWORD divisor = 3;
	WORD multiplier = 2;

	// EVENT NOT ACTIVE	
	time_t start = CUtilities::GetCurrentTime() + 540;
	time_t stop = CUtilities::GetCurrentTime() + 600;
	CMultiplierBonusEvent *event = (CMultiplierBonusEvent *)NewBonus(id, start, stop, multiplier, MULTIPLIER_BONUS);

	// Award partial points, shouldn't work
	EXPECT_FALSE(event->WerePointsAwared());
	EXPECT_FALSE(event->AwardPartialPoints(countdown, divisor, multiplier));
	EXPECT_FALSE(event->WerePointsAwared());
	EXPECT_EQ(event->GetAwardedPoints(), 0);

	delete event;
}

TEST_F(BonusEventUnitTests, MultiplierBonusEventSessionReset)
{
	long id = 1234;
	WORD points = 10;
	WORD multiplier = 2;

	// EVENT ACTIVE	
	time_t start = CUtilities::GetCurrentTime() - 60;
	time_t stop = CUtilities::GetCurrentTime() + 600;
	CMultiplierBonusEvent *event = (CMultiplierBonusEvent *)NewBonus(id, start, stop, multiplier, MULTIPLIER_BONUS);

	// Award points
	event->AwardPoints(points);
	event->SessionReset();
	EXPECT_FALSE(event->WerePointsAwared());
	EXPECT_EQ(event->GetAwardedPoints(), 0);

	delete event;

}
