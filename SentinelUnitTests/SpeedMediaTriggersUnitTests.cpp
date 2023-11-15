#include "stdafx.h"
#include "SpeedMedia/SpeedMediaTriggers.h"
#include "SpeedMedia/CoinInTrigger.h"
#include "SpeedMedia/CoinOutTrigger.h"
#include "SpeedMedia/CoinLostTrigger.h"
#include "DirectoryManager.h"

#include "Config/Config.h"
#include "Config/EGMConfig.h"

#include "gtest/gtest.h"
#include <sstream>
#include <string.h>

namespace SentinelNativeUnitTests
{
		TEST(SpeedMediaTriggersUnitTests,SpeedMediaTriggersConstructorUnitTest)
		{
			const __int64 initialCoinIn = -1;
			const __int64 initialCoinOut = -1;
			const __int64 initialExternalBonusingAttendantPayout = -1;
			const __int64 initialExternalBonusingMachinePayout = -1;
			const __int64 initialJackpotHandpay = -1;
			const __int64 initialMachinePayProgressivePayout = -1;
			const __int64 initialAttendantPayProgressivePayout = -1;
			const __int64 initialPoints = 0;

			const __int64 currentCoinIn = -1;
			const __int64 currentCoinOut = -1;
			const __int64 currentExternalBonusingAttendantPayout = -1;
			const __int64 currentExternalBonusingMachinePayout = -1;
			const __int64 currentJackpotHandpay = -1;
			const __int64 currentMachinePayProgressivePayout = -1;
			const __int64 currentAttendantPayProgressivePayout = -1;
			const __int64 currentPoints = 0;
			const __int64 m_jackpotAmount = 0;

			const JackpotType jackpotType = JT_NONE;
			const JackpotTransactionType jackpotTransType = JTRANS_NONE;
			const bool triggered = false;
			const bool bHaveSetStartTimestamp = false;
			const bool isCumulative = false;
			const int triggerIndex = 0;

			CSpeedMediaTriggers *triggers = new CSpeedMediaTriggers();

            std::stringstream failedMsg;
            failedMsg << "FAILED in " << static_cast<LPCTSTR>(__TFUNCTION__);

			ASSERT_TRUE(triggers->GetInitialCoinIn() == initialCoinIn ||
							triggers->GetInitialCoinOut() == initialCoinOut ||
							triggers->GetInitialExternalBonusingAttendantPayout() == initialExternalBonusingAttendantPayout ||
							triggers->GetInitialExternalBonusingMachinePayout() == initialExternalBonusingMachinePayout ||
							triggers->GetInitialJackpotHandpay() == initialJackpotHandpay ||
							triggers->GetInitialMachinePayProgressivePayout() == initialMachinePayProgressivePayout ||
							triggers->GetInitialAttendantPayProgressivePayout() == initialAttendantPayProgressivePayout ||
							triggers->GetInitialPoints() == initialPoints ||
							triggers->GetCurrentCoinIn() == currentCoinIn ||
							triggers->GetCurrentCoinOut() == currentCoinOut ||
							triggers->GetCurrentExternalBonusingAttendantPayout() == currentExternalBonusingAttendantPayout ||
							triggers->GetCurrentExternalBonusingMachinePayout() == currentExternalBonusingMachinePayout ||
							triggers->GetCurrentJackpotHandpay() == currentJackpotHandpay ||
							triggers->GetCurrentMachinePayProgressivePayout() == currentMachinePayProgressivePayout ||
							triggers->GetCurrentAttendantPayProgressivePayout() == currentAttendantPayProgressivePayout ||
							triggers->GetCurrentPoints() == currentPoints ||
							triggers->IsCumulative() == isCumulative) << failedMsg.str();

			//Cleanup
			delete triggers;
		}

		TEST(SpeedMediaTriggersUnitTests,SpeedMediaTriggersCopyConstructorUnitTest)
		{
			const __int64 initialCoinIn = -1;
			const __int64 initialCoinOut = -1;
			const __int64 initialExternalBonusingAttendantPayout = -1;
			const __int64 initialExternalBonusingMachinePayout = -1;
			const __int64 initialJackpotHandpay = -1;
			const __int64 initialMachinePayProgressivePayout = -1;
			const __int64 initialAttendantPayProgressivePayout = -1;
			const __int64 initialPoints = 0;

			const __int64 currentCoinIn = 9;
			const __int64 currentCoinOut = 10;
			const __int64 currentExternalBonusingAttendantPayout = 11;
			const __int64 currentExternalBonusingMachinePayout = 12;
			const __int64 currentJackpotHandpay = 13;
			const __int64 currentMachinePayProgressivePayout = 14;
			const __int64 currentAttendantPayProgressivePayout = 15;
			const __int64 currentPoints = 16;
			const __int64 jackpotAmount = 17;

			const JackpotType jackpotType = JT_NONE;
			const JackpotTransactionType jackpotTransType = JTRANS_NONE;
			const bool triggered = true;
			const bool bHaveSetStartTimestamp = true;
			const bool isCumulative = true;
			const int triggerIndex = 1;

			CSpeedMediaTriggers *triggers = new CSpeedMediaTriggers();
			CCoinInTrigger* coinInTrigger = new CCoinInTrigger(1, 2, CoinInTrigger);

			triggers->UpdateCoinIn(currentCoinIn);
			triggers->UpdateCoinOut(currentCoinOut);
			triggers->UpdateExternalBonusingAttendantPayout(currentExternalBonusingAttendantPayout);
			triggers->UpdateExternalBonusingMachinePayout(currentExternalBonusingMachinePayout);
			triggers->UpdateJackpotHandpay(currentJackpotHandpay);
			triggers->UpdateMachinePayProgressivePayout(currentMachinePayProgressivePayout);
			triggers->UpdateAttendantPayProgressivePayout(currentAttendantPayProgressivePayout);
			triggers->UpdatePointsEarned(currentPoints);
			triggers->SetJackpotAmount(jackpotAmount);
			triggers->SetJackpotTypes(jackpotType, jackpotTransType);
			triggers->SetAsTriggered();
			triggers->UpdateStartTime();
			triggers->SetIsCumulative(isCumulative);			
			triggers->AddTrigger(coinInTrigger);

			CSpeedMediaTriggers *copiedTriggers = new CSpeedMediaTriggers(*triggers);			
			CCoinInTrigger* copiedCoinInTrigger((CCoinInTrigger*)copiedTriggers->GetFirstTrigger());

            std::stringstream failedMsg;
            failedMsg << "FAILED in " << static_cast<LPCTSTR>(__TFUNCTION__);

			ASSERT_TRUE(*coinInTrigger == *copiedCoinInTrigger ||
				triggers->GetInitialCoinIn() == copiedTriggers->GetInitialCoinIn() ||
				triggers->GetInitialCoinOut() == copiedTriggers->GetInitialCoinOut() ||
				triggers->GetInitialExternalBonusingAttendantPayout() == copiedTriggers->GetInitialExternalBonusingAttendantPayout() ||
				triggers->GetInitialExternalBonusingMachinePayout() == copiedTriggers->GetInitialExternalBonusingMachinePayout() ||
				triggers->GetInitialJackpotHandpay() == copiedTriggers->GetInitialJackpotHandpay() ||
				triggers->GetInitialMachinePayProgressivePayout() == copiedTriggers->GetInitialMachinePayProgressivePayout() ||
				triggers->GetInitialAttendantPayProgressivePayout() == copiedTriggers->GetInitialAttendantPayProgressivePayout() ||
				triggers->GetInitialPoints() == copiedTriggers->GetInitialPoints() ||
				triggers->GetCurrentCoinIn() == copiedTriggers->GetCurrentCoinIn() ||
				triggers->GetCurrentCoinOut() == copiedTriggers->GetCurrentCoinOut() ||
				triggers->GetCurrentExternalBonusingAttendantPayout() == copiedTriggers->GetCurrentExternalBonusingAttendantPayout() ||
				triggers->GetCurrentExternalBonusingMachinePayout() == copiedTriggers->GetCurrentExternalBonusingMachinePayout() ||
				triggers->GetCurrentJackpotHandpay() == copiedTriggers->GetCurrentJackpotHandpay() ||
				triggers->GetCurrentMachinePayProgressivePayout() == copiedTriggers->GetCurrentMachinePayProgressivePayout() ||
				triggers->GetCurrentAttendantPayProgressivePayout() == copiedTriggers->GetCurrentAttendantPayProgressivePayout() ||
				triggers->GetCurrentPoints() == copiedTriggers->GetCurrentPoints() ||
				triggers->IsCumulative() == copiedTriggers->IsCumulative()) << failedMsg.str();

			delete triggers;//will delete coinInTrigger
			delete copiedTriggers;
		}

		TEST(SpeedMediaTriggersUnitTests,SpeedMediaTriggersAssignmentOperatorUnitTest)
		{
			const __int64 initialCoinIn = -1;
			const __int64 initialCoinOut = -1;
			const __int64 initialExternalBonusingAttendantPayout = -1;
			const __int64 initialExternalBonusingMachinePayout = -1;
			const __int64 initialJackpotHandpay = -1;
			const __int64 initialMachinePayProgressivePayout = -1;
			const __int64 initialAttendantPayProgressivePayout = -1;
			const __int64 initialPoints = 0;

			const __int64 currentCoinIn = 9;
			const __int64 currentCoinOut = 10;
			const __int64 currentExternalBonusingAttendantPayout = 11;
			const __int64 currentExternalBonusingMachinePayout = 12;
			const __int64 currentJackpotHandpay = 13;
			const __int64 currentMachinePayProgressivePayout = 14;
			const __int64 currentAttendantPayProgressivePayout = 15;
			const __int64 currentPoints = 16;
			const __int64 jackpotAmount = 17;

			const JackpotType jackpotType = JT_NONE;
			const JackpotTransactionType jackpotTransType = JTRANS_NONE;
			const bool triggered = true;
			const bool bHaveSetStartTimestamp = true;
			const bool isCumulative = true;
			const int triggerIndex = 1;

			CSpeedMediaTriggers *triggers = new CSpeedMediaTriggers();
			CCoinInTrigger* coinInTrigger = new CCoinInTrigger(1, 2, CoinInTrigger);

			triggers->UpdateCoinIn(currentCoinIn);
			triggers->UpdateCoinOut(currentCoinOut);
			triggers->UpdateExternalBonusingAttendantPayout(currentExternalBonusingAttendantPayout);
			triggers->UpdateExternalBonusingMachinePayout(currentExternalBonusingMachinePayout);
			triggers->UpdateJackpotHandpay(currentJackpotHandpay);
			triggers->UpdateMachinePayProgressivePayout(currentMachinePayProgressivePayout);
			triggers->UpdateAttendantPayProgressivePayout(currentAttendantPayProgressivePayout);
			triggers->UpdatePointsEarned(currentPoints);
			triggers->SetJackpotAmount(jackpotAmount);
			triggers->SetJackpotTypes(jackpotType, jackpotTransType);
			triggers->SetAsTriggered();
			triggers->UpdateStartTime();
			triggers->SetIsCumulative(isCumulative);			
			triggers->AddTrigger(coinInTrigger);

			CSpeedMediaTriggers *copiedTriggers = new CSpeedMediaTriggers();
			*copiedTriggers = *triggers;

			CCoinInTrigger* copiedCoinInTrigger((CCoinInTrigger*)copiedTriggers->GetFirstTrigger());

            std::stringstream failedMsg;
            failedMsg << "FAILED in " << static_cast<LPCTSTR>(__TFUNCTION__);

			ASSERT_TRUE(*coinInTrigger == *copiedCoinInTrigger ||
				triggers->GetInitialCoinIn() == copiedTriggers->GetInitialCoinIn() ||
				triggers->GetInitialCoinOut() == copiedTriggers->GetInitialCoinOut() ||
				triggers->GetInitialExternalBonusingAttendantPayout() == copiedTriggers->GetInitialExternalBonusingAttendantPayout() ||
				triggers->GetInitialExternalBonusingMachinePayout() == copiedTriggers->GetInitialExternalBonusingMachinePayout() ||
				triggers->GetInitialJackpotHandpay() == copiedTriggers->GetInitialJackpotHandpay() ||
				triggers->GetInitialMachinePayProgressivePayout() == copiedTriggers->GetInitialMachinePayProgressivePayout() ||
				triggers->GetInitialAttendantPayProgressivePayout() == copiedTriggers->GetInitialAttendantPayProgressivePayout() ||
				triggers->GetInitialPoints() == copiedTriggers->GetInitialPoints() ||
				triggers->GetCurrentCoinIn() == copiedTriggers->GetCurrentCoinIn() ||
				triggers->GetCurrentCoinOut() == copiedTriggers->GetCurrentCoinOut() ||
				triggers->GetCurrentExternalBonusingAttendantPayout() == copiedTriggers->GetCurrentExternalBonusingAttendantPayout() ||
				triggers->GetCurrentExternalBonusingMachinePayout() == copiedTriggers->GetCurrentExternalBonusingMachinePayout() ||
				triggers->GetCurrentJackpotHandpay() == copiedTriggers->GetCurrentJackpotHandpay() ||
				triggers->GetCurrentMachinePayProgressivePayout() == copiedTriggers->GetCurrentMachinePayProgressivePayout() ||
				triggers->GetCurrentAttendantPayProgressivePayout() == copiedTriggers->GetCurrentAttendantPayProgressivePayout() ||
				triggers->GetCurrentPoints() == copiedTriggers->GetCurrentPoints() ||
				triggers->IsCumulative() == copiedTriggers->IsCumulative()) << failedMsg.str();

			delete triggers;//will delete coinInTrigger
			delete copiedTriggers;
		}
		
		TEST(SpeedMediaTriggersUnitTests,SpeedMediaTriggerResetUnitTest)
		{
			const __int64 currentCoinIn = 9;
			const __int64 currentCoinOut = 10;
			const __int64 currentExternalBonusingAttendantPayout = 11;
			const __int64 currentExternalBonusingMachinePayout = 12;
			const __int64 currentJackpotHandpay = 13;
			const __int64 currentMachinePayProgressivePayout = 14;
			const __int64 currentAttendantPayProgressivePayout = 15;
			const __int64 currentPoints = 16;
			const __int64 jackpotAmount = 17;

			const JackpotType jackpotType = JT_NONE;
			const JackpotTransactionType jackpotTransType = JTRANS_NONE;
			const bool triggered = true;
			const bool bHaveSetStartTimestamp = true;
			const bool isCumulative = true;
			const int triggerIndex = 1;

			CSpeedMediaTriggers *triggers = new CSpeedMediaTriggers();
			triggers->Reset(currentCoinIn,
				currentCoinOut,
				currentExternalBonusingAttendantPayout,
				currentExternalBonusingMachinePayout,
				currentJackpotHandpay,
				currentMachinePayProgressivePayout,
				currentAttendantPayProgressivePayout,
				currentPoints);

            std::stringstream failedMsg;
            failedMsg << "FAILED in " << static_cast<LPCTSTR>(__TFUNCTION__);
			ASSERT_TRUE(triggers->GetCurrentCoinIn() == currentCoinIn ||
							triggers->GetCurrentCoinOut() == currentCoinOut ||
							triggers->GetCurrentExternalBonusingAttendantPayout() == currentExternalBonusingAttendantPayout ||
							triggers->GetCurrentExternalBonusingMachinePayout() == currentExternalBonusingMachinePayout ||
							triggers->GetCurrentJackpotHandpay() == currentJackpotHandpay ||
							triggers->GetCurrentMachinePayProgressivePayout() == currentMachinePayProgressivePayout ||
							triggers->GetCurrentAttendantPayProgressivePayout() == currentAttendantPayProgressivePayout ||
							triggers->GetCurrentPoints() == currentPoints ||
							triggers->IsCumulative() == isCumulative) << failedMsg.str();

			delete triggers;
		}

		TEST(SpeedMediaTriggersUnitTests,SpeedMediaTriggerEventStart1Test)
		{
			CDirectoryManager::MockInstance();

			CGame *egm = new CGame(MEMORY_NONE, true);
			egm->SetMeter(gCI, 10);
			egm->SetMeter(gCO, 10);
			egm->SetMeter(mBonusAtt, 10);
			egm->SetMeter(mBonusMach, 10);
			egm->SetMeter(gJP, 10);
			egm->SetMeter(mPrgPayMac, 10);
			egm->SetMeter(mPrgPayAtt, 10);

			CConfig config (false, "") ;
			const CEGMConfig egmConfig(MEMORY_NONE);

			//// player card
			time_t currentTime = CUtilities::GetCurrentTime();
			time_t utcTime = CUtilities::GetUTCTime();
			CCard *card(new CCard(currentTime, utcTime));
			card->SetCardID("245B22968F");
			card->SetCardType(CARD_PLAYER);
            CGames *games = new CGames();
			CPlayerInfo *playerInfo(new CPlayerInfo);
			CPlayer *player = new CPlayer(card, playerInfo, config, egmConfig, utcTime, false, games);

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(gCI, 100);
			meters->SetMeter(gCO, 100);
			meters->SetMeter(mBonusAtt, 100);
			meters->SetMeter(mBonusMach, 100);
			meters->SetMeter(gJP, 100);
			meters->SetMeter(mPrgPayMac, 100);
			meters->SetMeter(mPrgPayAtt, 100);
			player->SetSessionStartMeters(meters);

			CSpeedMediaTriggers *triggers = new CSpeedMediaTriggers();
			triggers->SetAtEventStart(player, egm);

			ASSERT_TRUE(triggers->GetInitialCoinIn() == 100 &&
				triggers->GetInitialCoinOut() == 100 &&
				triggers->GetInitialExternalBonusingAttendantPayout() == 100 &&
				triggers->GetInitialExternalBonusingMachinePayout() == 100 &&
				triggers->GetInitialJackpotHandpay() == 100 &&
				triggers->GetInitialMachinePayProgressivePayout() == 100 &&
				triggers->GetInitialAttendantPayProgressivePayout() == 100) << "The player meters should be used as the initial meters";

			delete triggers;
			delete player;
			delete egm;
		}

		TEST(SpeedMediaTriggersUnitTests,SpeedMediaTriggerEventStart2Test)
		{
			CDirectoryManager::MockInstance();

			CGame *egm = new CGame(MEMORY_NONE, true);
			egm->SetMeter(gCI, 10);
			egm->SetMeter(gCO, 10);
			egm->SetMeter(mBonusAtt, 10);
			egm->SetMeter(mBonusMach, 10);
			egm->SetMeter(gJP, 10);
			egm->SetMeter(mPrgPayMac, 10);
			egm->SetMeter(mPrgPayAtt, 10);

			CConfig config (false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			//// player card
			time_t currentTime = CUtilities::GetCurrentTime() - 21;
			time_t utcTime = CUtilities::GetUTCTime() - 21;
			CCard *card(new CCard(currentTime, utcTime));
			card->SetCardID("245B22968F");
			card->SetCardType(CARD_PLAYER);
            CGames *games = new CGames();
			CPlayerInfo *playerInfo(new CPlayerInfo);
			CPlayer *player = new CPlayer(card, playerInfo, config, egmConfig, utcTime, false, games);

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(gCI, 100);
			meters->SetMeter(gCO, 100);
			meters->SetMeter(mBonusAtt, 100);
			meters->SetMeter(mBonusMach, 100);
			meters->SetMeter(gJP, 100);
			meters->SetMeter(mPrgPayMac, 100);
			meters->SetMeter(mPrgPayAtt, 100);
			player->SetSessionStartMeters(meters);

			CSpeedMediaTriggers *triggers = new CSpeedMediaTriggers();
			triggers->SetAtEventStart(player, egm);

			ASSERT_TRUE(triggers->GetInitialCoinIn() == 10 &&
				triggers->GetInitialCoinOut() == 10 &&
				triggers->GetInitialExternalBonusingAttendantPayout() == 10 &&
				triggers->GetInitialExternalBonusingMachinePayout() == 10 &&
				triggers->GetInitialJackpotHandpay() == 10 &&
				triggers->GetInitialMachinePayProgressivePayout() == 10 &&
				triggers->GetInitialAttendantPayProgressivePayout() == 10) << "The player meters should be used as the initial meters";

			delete triggers;
			delete player;
			delete egm;
		}

        TEST(SpeedMediaTriggersUnitTests, SpeedMediaTriggeredTimestampUnitTest) {
            CSpeedMediaTriggers *triggers = new CSpeedMediaTriggers();
            time_t time1, time2, rawtime;
            tm tm1;
            memset(&tm1, 0, sizeof(tm));
			tm1.tm_year = 137;
            tm1.tm_mon = 1;
            tm1.tm_mday = 10;
            tm1.tm_hour = 11;
            tm1.tm_min = 22;
            tm1.tm_sec = 33;
            //time1.wMilliseconds = 44;
            time1 = mktime(&tm1);
			
            triggers->SetTriggeredTimestamp(time1);
            time2 = triggers->GetTriggerTimestamp();
			tm *tm2 = localtime(&time2);

            ASSERT_TRUE(tm1.tm_year == tm2->tm_year) << "Year wrong";
            ASSERT_TRUE(tm1.tm_mon	== tm2->tm_mon) << "Month wrong";
            ASSERT_TRUE(tm1.tm_mday == tm2->tm_mday) << "Days wrong";
            ASSERT_TRUE(tm1.tm_hour == tm2->tm_hour) << "Hours wrong";
            ASSERT_TRUE(tm1.tm_min == tm2->tm_min) << "Minutes wrong";
            ASSERT_TRUE(tm1.tm_sec == tm2->tm_sec) << "Seconds wrong";

            //Cleanup
            delete triggers;
        }
        
    TEST(SpeedMediaTriggersUnitTests, SpeedMediaTriggerJackpotAmountUnitTest) {
            //Setup
            const __int64 origJackpotAmount = 17;
            CSpeedMediaTriggers *triggers = new CSpeedMediaTriggers();
            triggers->SetJackpotAmount(origJackpotAmount);

            //Tests
            std::stringstream failedMsg;
            failedMsg << "FAILED in " << static_cast<LPCTSTR>(__TFUNCTION__);
            ASSERT_TRUE(triggers->GetJackpotAmount() == origJackpotAmount) << failedMsg.str();

            //Cleanup
            delete triggers;
        }

        TEST(SpeedMediaTriggersUnitTests, SpeedMediaTriggerIsCumulativeUnitTest) {
            //Setup
            const bool isCumulative = true;
            const bool isNotCumulative = false;

            CSpeedMediaTriggers *triggers = new CSpeedMediaTriggers();

            std::stringstream failedMsg;
            failedMsg << "FAILED in " << static_cast<LPCTSTR>(__TFUNCTION__);

            //Tests
            triggers->SetIsCumulative(isCumulative);
            ASSERT_TRUE(triggers->IsCumulative() == isCumulative) << failedMsg.str();

            triggers->SetIsCumulative(isNotCumulative);
            ASSERT_TRUE(triggers->IsCumulative() == isNotCumulative) << failedMsg.str();

            //Cleanup
            delete triggers;
        }

        TEST(SpeedMediaTriggersUnitTests, SpeedMediaTriggerCoinInUnitTest) {
            //Setup
            const __int64 currentCoinIn = 945;

            CSpeedMediaTriggers *triggers = new CSpeedMediaTriggers();

            std::stringstream failedMsg;
            failedMsg << "FAILED in " << static_cast<LPCTSTR>(__TFUNCTION__);

            //Tests
            triggers->UpdateCoinIn(currentCoinIn);
            ASSERT_TRUE(triggers->GetCurrentCoinIn() == currentCoinIn) << failedMsg.str();

            //Cleanup
            delete triggers;
        }

        TEST(SpeedMediaTriggersUnitTests, SpeedMediaTriggerCoinOutUnitTest) {
            //Setup
            const __int64 currentCoinOut = 549;

            CSpeedMediaTriggers *triggers = new CSpeedMediaTriggers();

            std::stringstream failedMsg;
            failedMsg << "FAILED in " << static_cast<LPCTSTR>(__TFUNCTION__);

            //Tests
            triggers->UpdateCoinOut(currentCoinOut);
            ASSERT_TRUE(triggers->GetCurrentCoinOut() == currentCoinOut) << failedMsg.str();

            //Cleanup
            delete triggers;
        }

        TEST(SpeedMediaTriggersUnitTests, SpeedMediaTriggerExternalBonusingAttendantPayoutUnitTest) {
            //Setup
            const __int64 currentExternalBonusingAttendantPayout = 677;

            CSpeedMediaTriggers *triggers = new CSpeedMediaTriggers();

            std::stringstream failedMsg;
            failedMsg << "FAILED in " << static_cast<LPCTSTR>(__TFUNCTION__);

            //Tests
            triggers->UpdateExternalBonusingAttendantPayout(currentExternalBonusingAttendantPayout);
            ASSERT_TRUE(triggers->GetCurrentExternalBonusingAttendantPayout() == currentExternalBonusingAttendantPayout) << failedMsg.str();

            //Cleanup
            delete triggers;
        }

}
