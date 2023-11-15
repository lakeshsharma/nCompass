#include "stdafx.h"
#include "SpeedMedia/SpeedMediaResendList.h"
#include "SpeedMedia/SentinelInformationResendItem.h"
#include "SpeedMedia/SentinelStatusResendItem.h"
#include "SpeedMedia/GamePlayedResendItem.h"
#include "PollerProtocol/PollerGenericTransactionMessage.h"
#include "Config/ConfigExcessiveVoucherOutEnabledField.h"
#include "Config/ConfigExcessiveVoucherOutThresholdField.h"
#include "gtest/gtest.h"

#include "UnitTestingOperatingSystem.h"

using namespace std;

class SpeedMediaResendListUnitTests : public ::testing::Test{};
		
TEST_F(SpeedMediaResendListUnitTests, SpeedMediaResendListConstructorTest)
{
	CSpeedMediaResendList speedMediaResendList(false, MEMORY_NONE);
	ASSERT_TRUE(100 == speedMediaResendList.m_freeList.size());
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 0);
}

TEST_F(SpeedMediaResendListUnitTests, SpeedMediaResendListOffsetValuesTest)
{
	CSpeedMediaResendList speedMediaResendList(false, MEMORY_NONE);
	DWORD startingOffset(688181);
	for (DWORD offset : speedMediaResendList.m_freeList)
	{
		ASSERT_TRUE(offset == startingOffset);
		startingOffset += 200;
	}
}

TEST_F(SpeedMediaResendListUnitTests, SpeedMediaResendRemoveAnItemWithTypeUnitTest)
{
	CSpeedMediaResendList speedMediaResendList(false, MEMORY_NONE);
	CSentinelInformationResendItem* sentinelInformationResendItem1(new CSentinelInformationResendItem(1, string("1.0.0"), string("1.0.0.0"), string("TestSkin1"), nCompassLCD));
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 0);
	speedMediaResendList.AddItemToBottom(sentinelInformationResendItem1);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 1);
	CSentinelInformationResendItem* sentinelInformationResendItem2(new CSentinelInformationResendItem(1, string("1.0.0"), string("1.0.0.0"), string("TestSkin2"), nCompassLCD));
	speedMediaResendList.AddItemToBottom(sentinelInformationResendItem2);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 1);
	CSentinelStatusResendItem* sentinelStatusResendItem1(new CSentinelStatusResendItem(1, "status1"));
	speedMediaResendList.AddItemToBottom(sentinelStatusResendItem1);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 2);
	CSentinelStatusResendItem* sentinelStatusResendItem2(new CSentinelStatusResendItem(1, "status2"));
	speedMediaResendList.AddItemToBottom(sentinelStatusResendItem2);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 2);
}

TEST_F(SpeedMediaResendListUnitTests, UpdateResendItemListRelatedToCardInIfNecessaryUpdateHasPlayed)
{
	CSpeedMediaResendList speedMediaResendList(false, MEMORY_NONE);
	SYSTEMTIME transactionTime;
	BYTE tempOCR1[]{ 0x01, 0x01, 0x01, 0x01, 0x01 };
	BYTE tempOCR2[]{ 0x02, 0x02, 0x02, 0x02, 0x02 };
	CCardInResendItem *cardInResendItemHasNotPlayed(new CCardInResendItem(1, tempOCR1, false, transactionTime));
	CCardInResendItem *cardInResendItemHasPlayed(new CCardInResendItem(1, tempOCR1, true, transactionTime));
	CCardInResendItem *anotherCardInResendItem(new CCardInResendItem(1, tempOCR2, false, transactionTime));
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 0);
	speedMediaResendList.AddItemToBottom(cardInResendItemHasNotPlayed);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 1);
	ASSERT_FALSE(((CCardInResendItem*)(speedMediaResendList.m_resendList.at(0)))->GetHasPlayed());
	speedMediaResendList.AddItemToBottom(cardInResendItemHasPlayed);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 1);
	ASSERT_TRUE(((CCardInResendItem*)(speedMediaResendList.m_resendList.at(0)))->GetHasPlayed());
	speedMediaResendList.AddItemToBottom(anotherCardInResendItem);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 2);
}


TEST_F(SpeedMediaResendListUnitTests, UpdateResendItemListRelatedToCardInIfNecessary2OCRs)
{
	CSpeedMediaResendList speedMediaResendList(false, MEMORY_NONE);
	SYSTEMTIME transactionTime;
	BYTE tempOCR1[]{ 0x01, 0x01, 0x01, 0x01, 0x01 };
	BYTE tempOCR2[]{ 0x02, 0x02, 0x02, 0x02, 0x02 };
	CCardInResendItem *cardInResendItem1(new CCardInResendItem(1, tempOCR1, false, transactionTime));
	CCardInResendItem *cardInResendItem2(new CCardInResendItem(1, tempOCR2, false, transactionTime));
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 0);
	speedMediaResendList.AddItemToBottom(cardInResendItem1);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 1);
	speedMediaResendList.AddItemToBottom(cardInResendItem2);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 2);
}

TEST_F(SpeedMediaResendListUnitTests, RemoveResendItemsListRelatedToCardOutIfNecessaryUnitTest1)
{
	CSpeedMediaResendList speedMediaResendList(false, MEMORY_NONE);
	SYSTEMTIME transactionTime;
	BYTE tempOCR1[]{ 0x01, 0x01, 0x01, 0x01, 0x01 };
	CCardInResendItem* cardInResendItem(new CCardInResendItem(1, tempOCR1, false, transactionTime));
	CGamePlayedResendItem* gamePlayedResendItem(new CGamePlayedResendItem(1, tempOCR1));	
	CCardOutResendItem* cardOutResendItem(new CCardOutResendItem(1, tempOCR1, transactionTime));
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 0);
	speedMediaResendList.AddItemToBottom(cardInResendItem);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 1);
	speedMediaResendList.AddItemToBottom(gamePlayedResendItem);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 2);
	speedMediaResendList.AddItemToBottom(cardOutResendItem);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 0);
}

TEST_F(SpeedMediaResendListUnitTests, RemoveResendItemsListRelatedToCardOutIfNecessaryUnitTest2)
{
	CSpeedMediaResendList speedMediaResendList(false, MEMORY_NONE);
	SYSTEMTIME transactionTime;
	BYTE tempOCR1[]{ 0x01, 0x01, 0x01, 0x01, 0x01 };
	CCardInResendItem* cardInResendItem(new CCardInResendItem(1, tempOCR1, false, transactionTime));
	CGamePlayedResendItem* gamePlayedResendItem(new CGamePlayedResendItem(1, tempOCR1));
	CCardOutResendItem* cardOutResendItem(new CCardOutResendItem(1, tempOCR1, transactionTime));
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 0);
	speedMediaResendList.AddItemToBottom(gamePlayedResendItem);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 1);
	speedMediaResendList.AddItemToBottom(cardOutResendItem);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 2);
}

TEST_F(SpeedMediaResendListUnitTests, RemoveResendItemsListRelatedToCardOutIfNecessaryUnitTest3)
{
	CSpeedMediaResendList speedMediaResendList(false, MEMORY_NONE);
	SYSTEMTIME transactionTime;
	BYTE tempOCR1[]{ 0x01, 0x01, 0x01, 0x01, 0x01 };
	CCardInResendItem* cardInResendItem(new CCardInResendItem(1, tempOCR1, false, transactionTime));
	cardInResendItem->SetItemSentFlag(true);
	CGamePlayedResendItem* gamePlayedResendItem(new CGamePlayedResendItem(1, tempOCR1));
	CCardOutResendItem* cardOutResendItem(new CCardOutResendItem(1, tempOCR1, transactionTime));
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 0);
	speedMediaResendList.AddItemToBottom(cardInResendItem);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 1);
	speedMediaResendList.AddItemToBottom(gamePlayedResendItem);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 2);
	speedMediaResendList.AddItemToBottom(cardOutResendItem);
	ASSERT_TRUE(speedMediaResendList.GetResendListItemCount() == 3);
}
