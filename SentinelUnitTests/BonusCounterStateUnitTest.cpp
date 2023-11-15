#include "stdafx.h"
#include "UI/BonusCounterItem.h"
#include "UI/BonusCounterState.h"
#include "Utilities.h"
#include "UI/ProgressiveBonusCounterItem.h"
#include "UI/NotificationBoxCounterItem.h"



#include "gtest/gtest.h"


using namespace BonusCounterState;

/// <summary>
/// Test the default Ctor, validates that the default id is set in the Ctor
/// </summary>
TEST(BonusCounterStateUnitTests, BonusCounterStateCtorTest)
{
	CBonusCounterState counterState;
	counterState.CycleDisplay();
	ASSERT_TRUE(-1 == counterState.GetCurrentLevelId()) <<  "Default Ctor failed to Show the Default Item";
}


/// <summary>
/// Test the AddBonusCounterItem function, validates that the CycleDisplay returns the added item
/// </summary>
TEST(BonusCounterStateUnitTests, BonusCounterStateAddOneItemTest)
{
	CBonusCounterState counterState;

	CProgressiveBonusCounterItem counterItem(&COperatingSystem::m_operatingSystem, 1111, "LevelName", 0, 100);

	counterState.AddOrUpdateBonusCounterItem(counterItem);

	counterState.CycleDisplay();

	ASSERT_TRUE(1111 == counterState.GetCurrentLevelId()) <<  "AddItem failed to cycle the single item";
}

TEST(BonusCounterStateUnitTests, BonusCounterStateUpdateItemsTest)
{
	CBonusCounterState counterState;
	CProgressiveBonusCounterItem counterItem2(&COperatingSystem::m_operatingSystem, 1111, "LevelName", 0, 100);
	CProgressiveBonusCounterItem counterItem3(&COperatingSystem::m_operatingSystem, 4444, "LevelName", 0, 200);

	counterState.AddOrUpdateBonusCounterItem(counterItem2);
	counterState.AddOrUpdateBonusCounterItem(counterItem3);

	CProgressiveBonusCounterItem counterItem2b(&COperatingSystem::m_operatingSystem, 3333, "newName", 100, 200);
	counterState.AddOrUpdateBonusCounterItem(counterItem2b);
	ASSERT_TRUE(counterState.GetCurrentLevelId() == 1111) <<  L"Current item should still be ProgressiveOdometer after ProgressiveBonusCounterItem update";
}

/// <summary>
/// Test the AddNotificationBoxBonusCounterItem function, 
/// validates that the CycleDisplay returns the added item 
/// </summary> 
TEST(BonusCounterStateUnitTests, AddNotificationBoxBonusCounterItemTest)
{
	CBonusCounterState counterState;

    CNotificationBoxCounterItem counterItem(&COperatingSystem::m_operatingSystem, 200);

	counterState.AddNotificationBoxBonusCounterItem();

	counterState.CycleDisplay();

	ASSERT_TRUE(0 == counterState.GetCurrentLevelId()) <<  "AddItem failed to cycle the single item";
}
