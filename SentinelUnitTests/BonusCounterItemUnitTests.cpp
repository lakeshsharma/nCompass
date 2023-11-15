
#include "stdafx.h"
#include "UI/BonusCounterItem.h"
#include "UI/ProgressiveBonusCounterItem.h"
#include "UI/PrizeBonusCounterItem.h"
#include "UI/ScatterBonusCounterItem.h"
#include "UI/PlayXGetYBonusCounterItem.h"

#include "gtest/gtest.h"

class BonusCounterItemUnitTests : public ::testing::Test
{
};

TEST_F(BonusCounterItemUnitTests, BonusCounterItemPreferredCtorTest)
{
	BonusCounterType expectedCounterType = BonusCounterType::Logo;
	int expectedID = 12345;
	int expectedPriorityLevel = CBonusCounterItem::TOP_PRIORITY_LEVEL_ID;
	CBonusCounterItem actualItem(nullptr, BonusCounterType::Logo, expectedID, expectedPriorityLevel);

	ASSERT_TRUE(actualItem.GetID() == expectedID) << "Preferred Ctor failure in BonusCounterItem - ID";
	ASSERT_TRUE(actualItem.GetPriorityLevel() == expectedPriorityLevel) << "Preferred Ctor failure in BonusCounterItem - PriorityLevel";
	ASSERT_TRUE(actualItem.GetBonusCounterType() == expectedCounterType) << "Preferred Ctor failure in BonusCounterItem - Type";
}


TEST_F(BonusCounterItemUnitTests, BonusCounterItemAssigmentOperatorTest)
{
	int expectedID = 12345;
	int expectedPriorityLevel = CBonusCounterItem::TOP_PRIORITY_LEVEL_ID;
	BonusCounterType expectedCounterType = BonusCounterType::Logo;

	CBonusCounterItem originalItem(&COperatingSystem::m_operatingSystem, BonusCounterType::Logo, expectedID, expectedPriorityLevel);
	CBonusCounterItem assignedItem = originalItem;

	ASSERT_TRUE(assignedItem.GetID() == expectedID) << "Assigment operator failure in BonusCounterItem - ID";
	ASSERT_TRUE(assignedItem.GetPriorityLevel() == expectedPriorityLevel) << "Assigment operator failure in BonusCounterItem - PriorityLevel";
	ASSERT_TRUE(assignedItem.GetBonusCounterType() == expectedCounterType) << "Assigment operator failure in BonusCounterItem - Type";

}

TEST_F(BonusCounterItemUnitTests, BonusCounterItemComparisonOperatorTest)
{
	int expectedID = 12345;
	int expectedPriorityLevel = CBonusCounterItem::TOP_PRIORITY_LEVEL_ID;

	CBonusCounterItem originalItem(&COperatingSystem::m_operatingSystem, BonusCounterType::Logo, expectedID, expectedPriorityLevel);

	CBonusCounterItem assignedItem = originalItem;

	ASSERT_TRUE(originalItem == assignedItem) << "Comparison operator failure in BonusCounterItem";
}

TEST_F(BonusCounterItemUnitTests, BonusCounterItemLessThanOperatorTest)
{
	int originalID = 12345;
	int originalPriorityLevel = 5;
	CBonusCounterItem originalItem(nullptr, BonusCounterType::Logo, originalID, originalPriorityLevel);

	int lessThanID = 334456;
	int lessThanPriorityLevel = CBonusCounterItem::TOP_PRIORITY_LEVEL_ID;
	CBonusCounterItem lessThanItem(nullptr, BonusCounterType::Logo, lessThanID, lessThanPriorityLevel);

	ASSERT_TRUE(lessThanItem < originalItem) << "LessThan operator failure in BonusCounterItem";
}

TEST_F(BonusCounterItemUnitTests, BonusCounterPriorityAccessorTest)
{
	CBonusCounterItem objItem(nullptr, BonusCounterType::InvalidCounterType, 0, 0);

	int expected = 5;

	objItem.SetPriorityLevel(expected);

	int actual = objItem.GetPriorityLevel();

	ASSERT_TRUE(expected == actual) << "Priority Accessor failure in BonusCounterItem";
}

TEST_F(BonusCounterItemUnitTests, ProgressiveBonusCounterItemTest)
{
	CProgressiveBonusCounterItem bonusCounterItem(&COperatingSystem::m_operatingSystem, 100, _T("Level Name"), 5, 200);

	const std::string EXPECTED_PARAMETERS(_T("timeString=\"Level Name\" percentage=\"5\""));

	bool areEqual = EXPECTED_PARAMETERS == bonusCounterItem.GetParameters();

	ASSERT_TRUE(areEqual) << "Parameters are not set properly";
}