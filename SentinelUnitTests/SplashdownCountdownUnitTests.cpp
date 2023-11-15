#include "stdafx.h"

#include "GlobalDefs.h"
#include "SpeedMedia/SplashdownCountdown.h"

#include "gtest/gtest.h"

namespace SentinelNativeUnitTests
{

    TEST(SplashdownCountdownUnitTests, SplashdownCountdownDefaultConstructor)
    {
        //Setup
        CSplashdownCountdown *splashdown = new CSplashdownCountdown();

        //Tests
        ASSERT_TRUE( splashdown->GetEventID() == -1) << "EventID is incorrect" ;
        ASSERT_TRUE( splashdown->GetInEventTrigger() == NULL) << "InEventTrigger should be null" ;
        int cardIDLen;
        ASSERT_TRUE( splashdown->GetCardID( cardIDLen ) == NULL) << "Card ID is incorrect" ;
        ASSERT_TRUE( cardIDLen == 0) << "Card ID Len is incorrect" ;
        ASSERT_TRUE( splashdown->GetCardIDLen() == 0) << "Card ID Len is incorrect" ;
        ASSERT_TRUE( splashdown->GetTriggerCustomScreenCtrlInfo() == NULL) << "TriggerCustomScreenCtrlInfo should be null" ;
        ASSERT_TRUE( splashdown->GetFinaleCustomScreenCtrlInfo() == NULL) << "FinaleCustomScreenCtrlInfo should be null" ;
        ASSERT_TRUE( splashdown->GetWinnerFinaleCustomScreenCtrlInfo() == NULL) << "WinnerFinaleCustomScreenCtrlInfo should be null" ;
        ASSERT_TRUE( splashdown->GetParams().empty()) << "params should be empty" ;
        ASSERT_TRUE( splashdown->GetTotalAwardAmount() == 0) << "Total award amount should be 0" ;
        ASSERT_FALSE( splashdown->IsTotalAwardInPoints()) << "Is total award in points flag should be false" ;

        //Cleanup
        delete splashdown;
    }

    TEST(SplashdownCountdownUnitTests, SplashdownCountdownSetGet)
    {
        //Setup
        CSplashdownCountdown *splashdown = new CSplashdownCountdown();
        const int EventID = 1234;
        splashdown->SetEventID( EventID );
        CSpeedMediaTriggers *inEventTrigger = new CSpeedMediaTriggers();
        splashdown->SetInEventTrigger( inEventTrigger );
        const int CardIDLen = 5;
        byte cardID[CardIDLen] = {1,2,3,4,5};
        splashdown->SetCardID( cardID, CardIDLen );
        CCtrlInfoList *TriggerCustomList = new CCtrlInfoList();
        splashdown->SetTriggerCustomScreenCtrlInfo(TriggerCustomList);
        CCtrlInfoList *FinaleCustomList = new CCtrlInfoList();
        splashdown->SetFinaleCustomScreenCtrlInfo(FinaleCustomList);
        CCtrlInfoList *WinnerCustomList = new CCtrlInfoList();
        splashdown->SetWinnerFinaleCustomScreenCtrlInfo(WinnerCustomList);
        const LPCTSTR Params = "These are the Params";
        splashdown->SetParams( Params );
        const int AwardAmount = 500;
        const int NumberOfAwards = 6;
        for ( int ai = 0; ai < NumberOfAwards; ai++ )
        {
            splashdown->AddToTotalAwardAmount(AwardAmount);
        }
        splashdown->SetTotalAwardInPoints( true );

        //Tests
        ASSERT_TRUE( splashdown->GetEventID() == EventID) << "EventID is incorrect" ;
        ASSERT_TRUE( splashdown->GetInEventTrigger() == inEventTrigger) << "InEventTrigger is incorrect" ;
        int cardIDLen;
        ASSERT_TRUE( memcmp( splashdown->GetCardID( cardIDLen ), cardID, CardIDLen) == 0) << "Card ID is incorrect" ;
        ASSERT_TRUE( cardIDLen == CardIDLen) << "Card ID Len is incorrect" ;
        ASSERT_TRUE( splashdown->GetCardIDLen() == CardIDLen) << "Card ID Len is incorrect" ;
        ASSERT_TRUE( splashdown->GetTriggerCustomScreenCtrlInfo() == TriggerCustomList) << "TriggerCustomScreenCtrlInfo is incorrect" ;
        ASSERT_TRUE( splashdown->GetFinaleCustomScreenCtrlInfo() == FinaleCustomList) << "FinaleCustomScreenCtrlInfo is incorrect" ;
        ASSERT_TRUE( splashdown->GetWinnerFinaleCustomScreenCtrlInfo() == WinnerCustomList) << "WinnerFinaleCustomScreenCtrlInfo is incorrect" ;
        ASSERT_TRUE( splashdown->GetParams() == Params) << "params is incorrect" ;
        ASSERT_TRUE( splashdown->GetTotalAwardAmount() == AwardAmount * NumberOfAwards) << "Total award amount is incorrect" ;
        ASSERT_TRUE( splashdown->IsTotalAwardInPoints()) << "Is total award in points flag should be true" ;

        //Cleanup
        delete splashdown;
    }

    TEST(SplashdownCountdownUnitTests, SplashdownCountdownCopyConstructor)
    {
        //Setup
        CSplashdownCountdown *originalSplashdown = new CSplashdownCountdown();
        const int EventID = 1234;
        originalSplashdown->SetEventID( EventID );
        CSpeedMediaTriggers *inEventTrigger = new CSpeedMediaTriggers();
        inEventTrigger->Reset(1,2,3,4,5,6,7,8);
        originalSplashdown->SetInEventTrigger( inEventTrigger );
        const int CardIDLen = 5;
        byte cardID[CardIDLen] = {1,2,3,4,5};
        originalSplashdown->SetCardID( cardID, CardIDLen );
        CCtrlInfoList *TriggerCustomList = new CCtrlInfoList();
        CCtrlInfo *triggerCustomMsg = new CCtrlInfo();
        const LPCTSTR TriggerCustomText = "TriggerCustomText";
        triggerCustomMsg->SetDefaultText(TriggerCustomText);
        TriggerCustomList->AddCtrl(triggerCustomMsg);
        originalSplashdown->SetTriggerCustomScreenCtrlInfo(TriggerCustomList);
        CCtrlInfoList *FinaleCustomList = new CCtrlInfoList();
        CCtrlInfo *finaleCustomMsg = new CCtrlInfo();
        const LPCTSTR FinaleCustomText = "FinaleCustomText";
        finaleCustomMsg->SetDefaultText(FinaleCustomText);
        FinaleCustomList->AddCtrl(finaleCustomMsg);
        originalSplashdown->SetFinaleCustomScreenCtrlInfo(FinaleCustomList);
        CCtrlInfoList *WinnerCustomList = new CCtrlInfoList();
        CCtrlInfo *winnerCustomMsg = new CCtrlInfo();
        const LPCTSTR WinnerCustomText = "WinnerCustomText";
        winnerCustomMsg->SetDefaultText(WinnerCustomText);
        WinnerCustomList->AddCtrl(winnerCustomMsg);
        originalSplashdown->SetWinnerFinaleCustomScreenCtrlInfo(WinnerCustomList);
        const LPCTSTR Params = "These are the Params";
        originalSplashdown->SetParams( Params );
        const int AwardAmount = 500;
        const int NumberOfAwards = 6;
        for ( int ai = 0; ai < NumberOfAwards; ai++ )
        {
            originalSplashdown->AddToTotalAwardAmount(AwardAmount);
        }
        originalSplashdown->SetTotalAwardInPoints( true );

        CSplashdownCountdown *splashdown = new CSplashdownCountdown(*originalSplashdown);

        //Tests
        ASSERT_TRUE( splashdown->GetEventID() == EventID) << "EventID is incorrect" ;
        ASSERT_TRUE( splashdown->GetInEventTrigger() != NULL) << "InEventTrigger should not be null" ;
        ASSERT_TRUE( splashdown->GetInEventTrigger()->GetInitialCoinIn() == 1) << "InEventTrigger is incorrect after copy" ;
        int cardIDLen;
        ASSERT_TRUE( memcmp( splashdown->GetCardID( cardIDLen ), cardID, CardIDLen) == 0) << "Card ID is incorrect" ;
        ASSERT_TRUE( cardIDLen == CardIDLen) << "Card ID Len is incorrect" ;
        ASSERT_TRUE( splashdown->GetCardIDLen() == CardIDLen) << "Card ID Len is incorrect" ;

        ASSERT_TRUE( splashdown->GetTriggerCustomScreenCtrlInfo() != NULL) << "TriggerCustomScreenCtrlInfo should not be NULL" ; 
        ASSERT_TRUE( splashdown->GetTriggerCustomScreenCtrlInfo()->GetCount() == 1) << "TriggerCustomScreenCtrlInfo count should be 1" ; 
        ASSERT_TRUE( splashdown->GetTriggerCustomScreenCtrlInfo()->GetCtrlAt(0) != NULL) << "TriggerCustomScreenCtrlInfo should not be NULL" ; 
        ASSERT_TRUE( splashdown->GetTriggerCustomScreenCtrlInfo()->GetCtrlAt(0)->GetDefaultText() == TriggerCustomText) << "TriggerCustomScreenCtrlInfo text is incorrect" ;

        ASSERT_TRUE( splashdown->GetFinaleCustomScreenCtrlInfo() != NULL) << "FinaleCustomScreenCtrlInfo should not be NULL" ; 
        ASSERT_TRUE( splashdown->GetFinaleCustomScreenCtrlInfo()->GetCount() == 1) << "FinaleCustomScreenCtrlInfo count should be 1" ; 
        ASSERT_TRUE( splashdown->GetFinaleCustomScreenCtrlInfo()->GetCtrlAt(0) != NULL) << "FinaleCustomScreenCtrlInfo should not be NULL" ; 
        ASSERT_TRUE( splashdown->GetFinaleCustomScreenCtrlInfo()->GetCtrlAt(0)->GetDefaultText() == FinaleCustomText) << "FinaleCustomScreenCtrlInfo text is incorrect" ; 

        ASSERT_TRUE( splashdown->GetWinnerFinaleCustomScreenCtrlInfo() != NULL) << "WinnerFinaleCustomScreenCtrlInfo should not be NULL" ; 
        ASSERT_TRUE( splashdown->GetWinnerFinaleCustomScreenCtrlInfo()->GetCount() == 1) << "WinnerFinaleCustomScreenCtrlInfo count should be 1" ; 
        ASSERT_TRUE( splashdown->GetWinnerFinaleCustomScreenCtrlInfo()->GetCtrlAt(0) != NULL) << "WinnerFinaleCustomScreenCtrlInfo should not be NULL" ; 
        ASSERT_TRUE( splashdown->GetWinnerFinaleCustomScreenCtrlInfo()->GetCtrlAt(0)->GetDefaultText() == WinnerCustomText) << "WinnerFinaleCustomScreenCtrlInfo text is incorrect" ; 

        ASSERT_TRUE( splashdown->GetParams() == Params) << "params is incorrect" ;
        ASSERT_TRUE( splashdown->GetTotalAwardAmount() == AwardAmount * NumberOfAwards) << "Total award amount is incorrect" ;
        ASSERT_TRUE( splashdown->IsTotalAwardInPoints()) << "Is total award in points flag should be true" ;

        //Cleanup
        delete splashdown;
        delete originalSplashdown;
    }
}
