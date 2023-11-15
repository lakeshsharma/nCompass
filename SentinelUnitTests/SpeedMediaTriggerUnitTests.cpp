#include "stdafx.h"

#include "SpeedMedia/CoinInTrigger.h"
#include "SpeedMedia/CoinLostTrigger.h"
#include "SpeedMedia/CoinOutTrigger.h"
#include "SpeedMedia/JackpotTrigger.h"
#include "SpeedMedia/TheoreticalWinTrigger.h"
#include "SpeedMedia/TimePlayedTrigger.h"
#include "GlobalDefs.h"

#include <iostream>
#include <sstream>
#include "gtest/gtest.h"

const __int64 minThreshold = 500;
const __int64 maxThreshold = 1000;
const int DENOM = 100;

typedef struct
{
    // Inputs.
    int denom;
    __int64 initialCoinIn;
    __int64 initialCoinOut;
    __int64 initialExternalBonusingAttendantPayout;
    __int64 initialExternalBonusingMachinePayout;
    __int64 initialJackpotHandpay;
    __int64 initialMachinePayProgressivePayout;
    __int64 initialAttendantPayProgressivePayout;
    __int64 currentCoinIn;
    __int64 currentCoinOut;
    __int64 currentExternalBonusingAttendantPayout;
    __int64 currentExternalBonusingMachinePayout;
    __int64 currentJackpotHandpay;
    __int64 currentMachinePayProgressivePayout;
    __int64 currentAttendantPayProgressivePayout;
    double par;

    // Expected results.
    bool expectedTriggered;
} TestTriggerDataType;

typedef struct
{
    //Inputs
    JackpotType inputJackpotType;
    JackpotTransactionType inputJackpotTransactionType;
    __int64 inputJackpotAmount;

    // Expected results.
    bool expectedTriggered;
} TestJackpotTriggerDataType;


namespace SentinelNativeUnitTests
{
    //Test the CCoinInTrigger Constructor, Copy constructor and Assignment operator
    TEST(SpeedMediaTriggerUnitTests, CoinInTriggerConstructorAndOperatorTests)
    {
        const TriggerClassType triggerType = CoinInTrigger;

        //Constructor Test
        CCoinInTrigger Trigger1(minThreshold, maxThreshold, triggerType);
        ASSERT_TRUE(Trigger1.GetMinThreshold() == minThreshold && 
                Trigger1.GetMaxThreshold() == maxThreshold &&
                Trigger1.GetType()		  == triggerType)
            << "CCoinInTrigger constructor failed.";

        //Copy Constructor Test
        CCoinInTrigger Trigger2 = Trigger1;//calls copy constructor
        ASSERT_TRUE(Trigger2.GetMinThreshold() == minThreshold && 
                Trigger2.GetMaxThreshold() == maxThreshold &&
                Trigger2.GetType()		  == triggerType)
            << "CCoinInTrigger copy constructor failed.";

        //Assignment Operator Test
        CCoinInTrigger Trigger3(minThreshold, maxThreshold, NoTrigger);//Make trigger type different
        Trigger3 = Trigger2;//calls assignment operator
        ASSERT_TRUE(Trigger3.GetMinThreshold() == minThreshold && 
                Trigger3.GetMaxThreshold() == maxThreshold &&
                Trigger3.GetType()		  == triggerType)
            << "CCoinInTrigger assignment operator failed." ;
    }

    //Test the CCoinInTrigger equality operator
    TEST(SpeedMediaTriggerUnitTests, CoinInTriggerEqualityTest) {
        const TriggerClassType triggerType = CoinInTrigger;

        //Setup
        CCoinInTrigger Trigger1(minThreshold, maxThreshold, triggerType);
        CCoinInTrigger Trigger2(minThreshold, maxThreshold, triggerType); //equals Trigger1
        CCoinInTrigger Trigger3(minThreshold + 1, maxThreshold, triggerType); //Different m_minThreshold
        CCoinInTrigger Trigger4(minThreshold, maxThreshold + 1, triggerType); //Different m_maxThreshold
        CCoinInTrigger Trigger5(minThreshold, maxThreshold, NoTrigger); //Different m_triggerType

        //Tests, compare all triggers to Trigger1
        ASSERT_TRUE(Trigger1  == Trigger2) << "CCoinInTrigger overloaded == operator failed.";
        ASSERT_FALSE(Trigger1 == Trigger3) << "CCoinInTrigger overloaded == operator failed.";
        ASSERT_FALSE(Trigger1 == Trigger4) << "CCoinInTrigger overloaded == operator failed.";
        ASSERT_FALSE(Trigger1 == Trigger5) << "CCoinInTrigger overloaded == operator failed.";
    }


    //Test the CCoinInTrigger Triggered Function Test
    TEST(SpeedMediaTriggerUnitTests, CoinInTriggerTriggeredFunctionTests) {
        const TestTriggerDataType TEST_DATA[] =
        {
            //Test 1
            { DENOM,  //denom
                1000,   //initialCoinIn;
                0,      //initialCoinOut;
                0,      //initialExternalBonusingAttendantPayout;
                0,      //initialExternalBonusingMachinePayout;
                0,      //initialJackpotHandpay;
                0,      //initialMachinePayProgressivePayout;
                0,      //initialAttendantPayProgressivePayout;
                1001,   //currentCoinIn;
                0,      //currentCoinOut;
                0,      //currentExternalBonusingAttendantPayout;
                0,      //currentExternalBonusingMachinePayout;
                0,      //currentJackpotHandpay;
                0,      //currentMachinePayProgressivePayout;
                0,      //currentAttendantPayProgressivePayout;
                0,      //par
                false }, //expectedTriggered

            //Test 2
            { DENOM,  //denom
                1000,   //initialCoinIn;
                0,      //initialCoinOut;
                0,      //initialExternalBonusingAttendantPayout;
                0,      //initialExternalBonusingMachinePayout;
                0,      //initialJackpotHandpay;
                0,      //initialMachinePayProgressivePayout;
                0,      //initialAttendantPayProgressivePayout;
                1004,   //currentCoinIn;
                0,      //currentCoinOut;
                0,      //currentExternalBonusingAttendantPayout;
                0,      //currentExternalBonusingMachinePayout;
                0,      //currentJackpotHandpay;
                0,      //currentMachinePayProgressivePayout;
                0,      //currentAttendantPayProgressivePayout;
                0,      //par
                false }, //expectedTriggered

            //Test 3
            { DENOM,  //denom
                1000,   //initialCoinIn;
                0,      //initialCoinOut;
                0,      //initialExternalBonusingAttendantPayout;
                0,      //initialExternalBonusingMachinePayout;
                0,      //initialJackpotHandpay;
                0,      //initialMachinePayProgressivePayout;
                0,      //initialAttendantPayProgressivePayout;
                1011,   //currentCoinIn;
                0,      //currentCoinOut;
                0,      //currentExternalBonusingAttendantPayout;
                0,      //currentExternalBonusingMachinePayout;
                0,      //currentJackpotHandpay;
                0,      //currentMachinePayProgressivePayout;
                0,      //currentAttendantPayProgressivePayout;
                0,      //par
                false }, //expectedTriggered

            //Test 4
            { DENOM,  //denom
                1000,   //initialCoinIn;
                0,      //initialCoinOut;
                0,      //initialExternalBonusingAttendantPayout;
                0,      //initialExternalBonusingMachinePayout;
                0,      //initialJackpotHandpay;
                0,      //initialMachinePayProgressivePayout;
                0,      //initialAttendantPayProgressivePayout;
                1005,   //currentCoinIn;
                0,      //currentCoinOut;
                0,      //currentExternalBonusingAttendantPayout;
                0,      //currentExternalBonusingMachinePayout;
                0,      //currentJackpotHandpay;
                0,      //currentMachinePayProgressivePayout;
                0,      //currentAttendantPayProgressivePayout;
                0,      //par
                true }, //expectedTriggered

            //Test 5
            { DENOM,  //denom
                1000,   //initialCoinIn;
                0,      //initialCoinOut;
                0,      //initialExternalBonusingAttendantPayout;
                0,      //initialExternalBonusingMachinePayout;
                0,      //initialJackpotHandpay;
                0,      //initialMachinePayProgressivePayout;
                0,      //initialAttendantPayProgressivePayout;
                1007,   //currentCoinIn;
                0,      //currentCoinOut;
                0,      //currentExternalBonusingAttendantPayout;
                0,      //currentExternalBonusingMachinePayout;
                0,      //currentJackpotHandpay;
                0,      //currentMachinePayProgressivePayout;
                0,      //currentAttendantPayProgressivePayout;
                0,      //par
                true },  //expectedTriggered
        };

        for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex) {
            //Setup
            CCoinInTrigger trigger(minThreshold, maxThreshold, CoinInTrigger);

            //Test
            bool triggered = trigger.Triggered(TEST_DATA[testIndex].denom, //denom
                    TEST_DATA[testIndex].initialCoinIn,
                    TEST_DATA[testIndex].initialCoinOut,
                    TEST_DATA[testIndex].initialExternalBonusingAttendantPayout,
                    TEST_DATA[testIndex].initialExternalBonusingMachinePayout,
                    TEST_DATA[testIndex].initialJackpotHandpay,
                    TEST_DATA[testIndex].initialMachinePayProgressivePayout,
                    TEST_DATA[testIndex].initialAttendantPayProgressivePayout,
                    TEST_DATA[testIndex].currentCoinIn, //currentCoinIn,
                    TEST_DATA[testIndex].currentCoinOut,
                    TEST_DATA[testIndex].currentExternalBonusingAttendantPayout,
                    TEST_DATA[testIndex].currentExternalBonusingMachinePayout,
                    TEST_DATA[testIndex].currentJackpotHandpay,
                    TEST_DATA[testIndex].currentMachinePayProgressivePayout,
                    TEST_DATA[testIndex].currentAttendantPayProgressivePayout,
                    TEST_DATA[testIndex].par);

            bool areEqual = triggered == TEST_DATA[testIndex].expectedTriggered;
            std::stringstream failedMsg;
            failedMsg << "FAILED Triggered=" << (int)triggered 
                << " expected=" << (int)TEST_DATA[testIndex].expectedTriggered 
                << " in " << static_cast<LPCTSTR>(__TFUNCTION__);

            ASSERT_TRUE(areEqual) << failedMsg.str();
        }
    }

    //Test the CCoinLostTrigger Constructor, Copy constructor and Assignment operator
    TEST(SpeedMediaTriggerUnitTests, CoinLostTriggerConstructorAndOperatorTests) {
        const TriggerClassType triggerType = CoinLostTrigger;

        //Constructor Test
        CCoinLostTrigger Trigger1(minThreshold, maxThreshold, triggerType);
        ASSERT_TRUE(Trigger1.GetMinThreshold() == minThreshold &&
                Trigger1.GetMaxThreshold() == maxThreshold &&
                Trigger1.GetType() == triggerType)
            << "CCoinLostTrigger constructor failed.";

        //Copy Constructor Test
        CCoinLostTrigger Trigger2 = Trigger1; //calls copy constructor
        ASSERT_TRUE(Trigger2.GetMinThreshold() == minThreshold &&
                Trigger2.GetMaxThreshold() == maxThreshold &&
                Trigger2.GetType() == triggerType)
            << "CCoinLostTrigger copy constructor failed.";

        //Assignment Operator Test
        CCoinLostTrigger Trigger3(minThreshold, maxThreshold, NoTrigger); //Make trigger type different
        Trigger3 = Trigger2; //calls assignment operator
        ASSERT_TRUE(Trigger3.GetMinThreshold() == minThreshold &&
                Trigger3.GetMaxThreshold() == maxThreshold &&
                Trigger3.GetType() == triggerType)
            << "CCoinLostTrigger assignment operator failed.";
    }

    //Test the CCoinOutTrigger Constructor, Copy constructor and Assignment operator
    TEST(SpeedMediaTriggerUnitTests, CoinOutTriggerConstructorAndOperatorTests) {
        const TriggerClassType triggerType = CoinOut;

        //Constructor Test
        CCoinOutTrigger Trigger1(minThreshold, maxThreshold, triggerType);
        ASSERT_TRUE(Trigger1.GetMinThreshold()== minThreshold &&
                Trigger1.GetMaxThreshold()== maxThreshold &&
                Trigger1.GetType()== triggerType)
            << "CCoinOutTrigger constructor failed.";

        //Copy Constructor Test
        CCoinOutTrigger Trigger2 = Trigger1; //calls copy constructor
        ASSERT_TRUE(Trigger2.GetMinThreshold()== minThreshold &&
                Trigger2.GetMaxThreshold()== maxThreshold &&
                Trigger2.GetType()== triggerType)
            << "CCoinOutTrigger copy constructor failed.";

        //Assignment Operator Test
        CCoinOutTrigger Trigger3(minThreshold, maxThreshold, NoTrigger); //Make trigger type different
        Trigger3 = Trigger2; //calls assignment operator
        ASSERT_TRUE(Trigger3.GetMinThreshold() == minThreshold &&
                Trigger3.GetMaxThreshold() == maxThreshold &&
                Trigger3.GetType() == triggerType)
            << "CCoinOutTrigger assignment operator failed.";
    }

    //Test the CCoinOutTrigger equality operator
    TEST(SpeedMediaTriggerUnitTests, CoinOutTriggerEqualityTest) {
        const TriggerClassType triggerType = CoinOut;

        //Setup
        CCoinOutTrigger Trigger1(minThreshold, maxThreshold, triggerType);
        CCoinOutTrigger Trigger2(minThreshold, maxThreshold, triggerType); //equals Trigger1
        CCoinOutTrigger Trigger3(minThreshold + 1, maxThreshold, triggerType); //Different m_minThreshold
        CCoinOutTrigger Trigger4(minThreshold, maxThreshold + 1, triggerType); //Different m_maxThreshold
        CCoinOutTrigger Trigger5(minThreshold, maxThreshold, NoTrigger); //Different m_triggerType

        //Tests, compare all triggers to Trigger1
        ASSERT_TRUE(Trigger1  == Trigger2) << "CCoinOutTrigger overloaded == operator failed.";
        ASSERT_FALSE(Trigger1 == Trigger3) << "CCoinOutTrigger overloaded == operator failed.";
        ASSERT_FALSE(Trigger1 == Trigger4) << "CCoinOutTrigger overloaded == operator failed.";
        ASSERT_FALSE(Trigger1 == Trigger5) << "CCoinOutTrigger overloaded == operator failed.";
    }

    //Test the CCoinOutTrigger Triggered Function Test
    TEST(SpeedMediaTriggerUnitTests, CoinOutTriggerTriggeredFunctionTest) {
        const TestTriggerDataType TEST_DATA[] =
        {
            //Test 1
            { DENOM,  //denom
                0,  //initialCoinIn;
                1000,   //initialCoinOut;
                0,      //initialExternalBonusingAttendantPayout;
                0,      //initialExternalBonusingMachinePayout;
                0,      //initialJackpotHandpay;
                0,      //initialMachinePayProgressivePayout;
                0,      //initialAttendantPayProgressivePayout;
                0,      //currentCoinIn;
                1001,   //currentCoinOut;
                0,      //currentExternalBonusingAttendantPayout;
                0,      //currentExternalBonusingMachinePayout;
                0,      //currentJackpotHandpay;
                0,      //currentMachinePayProgressivePayout;
                0,      //currentAttendantPayProgressivePayout;
                0,      //par
                false }, //expectedTriggered

            //Test 2
            { DENOM,  //denom
                0,  //initialCoinIn;
                1000,   //initialCoinOut;
                0,      //initialExternalBonusingAttendantPayout;
                0,      //initialExternalBonusingMachinePayout;
                0,      //initialJackpotHandpay;
                0,      //initialMachinePayProgressivePayout;
                0,      //initialAttendantPayProgressivePayout;
                0,      //currentCoinIn;
                1004,   //currentCoinOut;
                0,      //currentExternalBonusingAttendantPayout;
                0,      //currentExternalBonusingMachinePayout;
                0,      //currentJackpotHandpay;
                0,      //currentMachinePayProgressivePayout;
                0,      //currentAttendantPayProgressivePayout;
                0,      //par
                false }, //expectedTriggered

            //Test 3
            { DENOM,  //denom
                0,  //initialCoinIn;
                1000,   //initialCoinOut;
                0,      //initialExternalBonusingAttendantPayout;
                0,      //initialExternalBonusingMachinePayout;
                0,      //initialJackpotHandpay;
                0,      //initialMachinePayProgressivePayout;
                0,      //initialAttendantPayProgressivePayout;
                0,      //currentCoinIn;
                1011,   //currentCoinOut;
                0,      //currentExternalBonusingAttendantPayout;
                0,      //currentExternalBonusingMachinePayout;
                0,      //currentJackpotHandpay;
                0,      //currentMachinePayProgressivePayout;
                0,      //currentAttendantPayProgressivePayout;
                0,      //par
                false }, //expectedTriggered

            //Test 4
            { DENOM,  //denom
                0,  //initialCoinIn;
                1000,   //initialCoinOut;
                0,      //initialExternalBonusingAttendantPayout;
                0,      //initialExternalBonusingMachinePayout;
                0,      //initialJackpotHandpay;
                0,      //initialMachinePayProgressivePayout;
                0,      //initialAttendantPayProgressivePayout;
                0,      //currentCoinIn;
                1005,   //currentCoinOut;
                0,      //currentExternalBonusingAttendantPayout;
                0,      //currentExternalBonusingMachinePayout;
                0,      //currentJackpotHandpay;
                0,      //currentMachinePayProgressivePayout;
                0,      //currentAttendantPayProgressivePayout;
                0,      //par
                true }, //expectedTriggered

            //Test 5
            { DENOM,  //denom
                0,  //initialCoinIn;
                1000,   //initialCoinOut;
                0,      //initialExternalBonusingAttendantPayout;
                0,      //initialExternalBonusingMachinePayout;
                0,      //initialJackpotHandpay;
                0,      //initialMachinePayProgressivePayout;
                0,      //initialAttendantPayProgressivePayout;
                0,      //currentCoinIn;
                1007,   //currentCoinOut;
                0,      //currentExternalBonusingAttendantPayout;
                0,      //currentExternalBonusingMachinePayout;
                0,      //currentJackpotHandpay;
                0,      //currentMachinePayProgressivePayout;
                0,      //currentAttendantPayProgressivePayout;
                0,      //par
                true },  //expectedTriggered
        };

        for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex) {
            //Setup
            CCoinOutTrigger trigger(minThreshold, maxThreshold, CoinOut);

            //Test
            bool triggered = trigger.Triggered(TEST_DATA[testIndex].denom, //denom
                    TEST_DATA[testIndex].initialCoinIn,
                    TEST_DATA[testIndex].initialCoinOut,
                    TEST_DATA[testIndex].initialExternalBonusingAttendantPayout,
                    TEST_DATA[testIndex].initialExternalBonusingMachinePayout,
                    TEST_DATA[testIndex].initialJackpotHandpay,
                    TEST_DATA[testIndex].initialMachinePayProgressivePayout,
                    TEST_DATA[testIndex].initialAttendantPayProgressivePayout,
                    TEST_DATA[testIndex].currentCoinIn,
                    TEST_DATA[testIndex].currentCoinOut,
                    TEST_DATA[testIndex].currentExternalBonusingAttendantPayout,
                    TEST_DATA[testIndex].currentExternalBonusingMachinePayout,
                    TEST_DATA[testIndex].currentJackpotHandpay,
                    TEST_DATA[testIndex].currentMachinePayProgressivePayout,
                    TEST_DATA[testIndex].currentAttendantPayProgressivePayout,
                    TEST_DATA[testIndex].par);

            bool areEqual = triggered == TEST_DATA[testIndex].expectedTriggered;
            std::stringstream failedMsg;
            failedMsg << "FAILED Triggered=" << (int)triggered 
                << " expected=" << (int)TEST_DATA[testIndex].expectedTriggered
                << " in " << static_cast<LPCTSTR>(__TFUNCTION__);

            ASSERT_TRUE(areEqual) <<  failedMsg.str();
        }
    }

    //Test the CJackpotTrigger Constructor, Copy constructor and Assignment operator
    TEST(SpeedMediaTriggerUnitTests, JackpotTriggerConstructorAndOperatorTests) {
        const TriggerClassType triggerType = AllJackpots;

        //Constructor Test
        CJackpotTrigger Trigger1(minThreshold, maxThreshold, triggerType);
        ASSERT_TRUE(Trigger1.GetMinThreshold()== minThreshold &&
                Trigger1.GetMaxThreshold()== maxThreshold &&
                Trigger1.GetType()== triggerType)
            << "CJackpotTrigger constructor failed.";

        //Copy Constructor Test
        CJackpotTrigger Trigger2 = Trigger1; //calls copy constructor
        ASSERT_TRUE(Trigger2.GetMinThreshold()== minThreshold &&
                Trigger2.GetMaxThreshold()== maxThreshold &&
                Trigger2.GetType()== triggerType)
            << "CJackpotTrigger copy constructor failed.";

        //Assignment Operator Test
        CJackpotTrigger Trigger3(minThreshold, maxThreshold, NoTrigger); //Make trigger type different
        Trigger3 = Trigger2; //calls assignment operator
        ASSERT_TRUE(Trigger3.GetMinThreshold() == minThreshold &&
                Trigger3.GetMaxThreshold() == maxThreshold &&
                Trigger3.GetType() == triggerType)
            << "CJackpotTrigger assignment operator failed.";
    }

    //Test the JackpotTrigger equality operator
    TEST(SpeedMediaTriggerUnitTests, JackpotTriggerEqualityTest) {
        const TriggerClassType triggerType = AllJackpots;

        //Setup
        CJackpotTrigger Trigger1(minThreshold, maxThreshold, triggerType);
        CJackpotTrigger Trigger2(minThreshold, maxThreshold, triggerType); //equals Trigger1
        CJackpotTrigger Trigger3(minThreshold + 1, maxThreshold, triggerType); //Different m_minThreshold
        CJackpotTrigger Trigger4(minThreshold, maxThreshold + 1, triggerType); //Different m_maxThreshold
        CJackpotTrigger Trigger5(minThreshold, maxThreshold, NoTrigger); //Different m_triggerType

        //Tests, compare all triggers to Trigger1
        ASSERT_TRUE(Trigger1  == Trigger2) << "CJackpotTrigger overloaded == operator failed.";
        ASSERT_FALSE(Trigger1 == Trigger3) << "CJackpotTrigger overloaded == operator failed.";
        ASSERT_FALSE(Trigger1 == Trigger4) << "CJackpotTrigger overloaded == operator failed.";
        ASSERT_FALSE(Trigger1 == Trigger5) << "CJackpotTrigger overloaded == operator failed.";
    }

    //Test the CJackpotTrigger AllJackpots type Triggered function
    TEST(SpeedMediaTriggerUnitTests, AllJackpotsTriggeredFunctionTests) {
        const TestJackpotTriggerDataType TEST_DATA[] =
        {
            //JT_NONE type tests
            { JT_NONE, JTRANS_NONE, minThreshold - 1, false },
            { JT_NONE, JTRANS_NONE, maxThreshold + 1, false },
            { JT_NONE, JTRANS_NONE, minThreshold + 1, true },
            { JT_NONE, JTRANS_NONE, maxThreshold - 1, true },
            { JT_NONE, JTRANS_NONE, minThreshold, true },
            { JT_NONE, JTRANS_NONE, maxThreshold, true },

            //JT_CANCEL_CREDIT type tests -> These will never trigger
            { JT_CANCEL_CREDIT, JTRANS_NONE, minThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, maxThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, minThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, maxThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, minThreshold, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, maxThreshold, false },

            //JT_HANDPAY type tests
            { JT_HANDPAY, JTRANS_NONE, minThreshold - 1, false },
            { JT_HANDPAY, JTRANS_NONE, maxThreshold + 1, false },
            { JT_HANDPAY, JTRANS_NONE, minThreshold + 1, true },
            { JT_HANDPAY, JTRANS_NONE, maxThreshold - 1, true },
            { JT_HANDPAY, JTRANS_NONE, minThreshold, true },
            { JT_HANDPAY, JTRANS_NONE, maxThreshold, true },

            //JT_TOPAWARD type tests
            { JT_TOPAWARD, JTRANS_NONE, minThreshold - 1, false },
            { JT_TOPAWARD, JTRANS_NONE, maxThreshold + 1, false },
            { JT_TOPAWARD, JTRANS_NONE, minThreshold + 1, true },
            { JT_TOPAWARD, JTRANS_NONE, maxThreshold - 1, true },
            { JT_TOPAWARD, JTRANS_NONE, minThreshold, true },
            { JT_TOPAWARD, JTRANS_NONE, maxThreshold, true },
        };

        for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex) {
            //Setup
            CJackpotTrigger trigger(minThreshold, maxThreshold, AllJackpots);
            CJackpot *jackpot(new CJackpot());
            jackpot->SetJackpotType(TEST_DATA[testIndex].inputJackpotType);
            jackpot->SetJackpotTransactionType(TEST_DATA[testIndex].inputJackpotTransactionType);
            jackpot->SetJackpotAmount(TEST_DATA[testIndex].inputJackpotAmount);

            //Test
            bool triggered = trigger.Triggered(jackpot);
            bool areEqual = triggered == TEST_DATA[testIndex].expectedTriggered;
            std::stringstream failedMsg;
            failedMsg << "FAILED Triggered=" << (int)triggered 
                << " expected=" << (int)TEST_DATA[testIndex].expectedTriggered
                << " in " << static_cast<LPCTSTR>(__TFUNCTION__);

            ASSERT_TRUE(areEqual) <<  failedMsg.str();

            //Cleanup
            delete jackpot;
        }
    }

    //Test the CJackpotTrigger AttendantPaid type Triggered function
    TEST(SpeedMediaTriggerUnitTests, AttendantPaidTriggeredFunctionTests) {
        const TestJackpotTriggerDataType TEST_DATA[] =
        {
            //JT_NONE type tests
            { JT_NONE, JTRANS_COMPOSITE, minThreshold - 1, false },
            { JT_NONE, JTRANS_COMPOSITE, maxThreshold + 1, false },
            { JT_NONE, JTRANS_COMPOSITE, minThreshold + 1, false },
            { JT_NONE, JTRANS_COMPOSITE, maxThreshold - 1, false },
            { JT_NONE, JTRANS_COMPOSITE, minThreshold, false },
            { JT_NONE, JTRANS_COMPOSITE, maxThreshold, false },
            { JT_NONE, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold - 1, false },
            { JT_NONE, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold + 1, false },
            { JT_NONE, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold + 1, false },
            { JT_NONE, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold - 1, false },
            { JT_NONE, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold, false },
            { JT_NONE, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold, false },
            { JT_NONE, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold - 1, false },
            { JT_NONE, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold + 1, false },
            { JT_NONE, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold + 1, false },
            { JT_NONE, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold - 1, false },
            { JT_NONE, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold, false },
            { JT_NONE, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold, false },
            { JT_NONE, JTRANS_NONE, minThreshold - 1, false },
            { JT_NONE, JTRANS_NONE, maxThreshold + 1, false },
            { JT_NONE, JTRANS_NONE, minThreshold + 1, false },
            { JT_NONE, JTRANS_NONE, maxThreshold - 1, false },
            { JT_NONE, JTRANS_NONE, minThreshold, false },
            { JT_NONE, JTRANS_NONE, maxThreshold, false },

            //JT_CANCEL_CREDIT type tests -> These will never trigger
            { JT_CANCEL_CREDIT, JTRANS_COMPOSITE, minThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_COMPOSITE, maxThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_COMPOSITE, minThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_COMPOSITE, maxThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_COMPOSITE, minThreshold, false },
            { JT_CANCEL_CREDIT, JTRANS_COMPOSITE, maxThreshold, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, minThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, maxThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, minThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, maxThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, minThreshold, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, maxThreshold, false },

            //JT_HANDPAY type tests ******************************************
            { JT_HANDPAY, JTRANS_COMPOSITE, minThreshold - 1, false },
            { JT_HANDPAY, JTRANS_COMPOSITE, maxThreshold + 1, false },
            { JT_HANDPAY, JTRANS_COMPOSITE, minThreshold + 1, true },
            { JT_HANDPAY, JTRANS_COMPOSITE, maxThreshold - 1, true },
            { JT_HANDPAY, JTRANS_COMPOSITE, minThreshold, true },
            { JT_HANDPAY, JTRANS_COMPOSITE, maxThreshold, true },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold - 1, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold + 1, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold + 1, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold - 1, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold - 1, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold + 1, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold + 1, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold - 1, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold, false },
            { JT_HANDPAY, JTRANS_NONE, minThreshold - 1, false },
            { JT_HANDPAY, JTRANS_NONE, maxThreshold + 1, false },
            { JT_HANDPAY, JTRANS_NONE, minThreshold + 1, false },
            { JT_HANDPAY, JTRANS_NONE, maxThreshold - 1, false },
            { JT_HANDPAY, JTRANS_NONE, minThreshold, false },
            { JT_HANDPAY, JTRANS_NONE, maxThreshold, false },

            //JT_TOPAWARD type tests ******************************************
            { JT_TOPAWARD, JTRANS_COMPOSITE, minThreshold - 1, false },
            { JT_TOPAWARD, JTRANS_COMPOSITE, maxThreshold + 1, false },
            { JT_TOPAWARD, JTRANS_COMPOSITE, minThreshold + 1, true },
            { JT_TOPAWARD, JTRANS_COMPOSITE, maxThreshold - 1, true },
            { JT_TOPAWARD, JTRANS_COMPOSITE, minThreshold, true },
            { JT_TOPAWARD, JTRANS_COMPOSITE, maxThreshold, true },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold - 1, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold + 1, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold + 1, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold - 1, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold - 1, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold + 1, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold + 1, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold - 1, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold, false },
            { JT_TOPAWARD, JTRANS_NONE, minThreshold - 1, false },
            { JT_TOPAWARD, JTRANS_NONE, maxThreshold + 1, false },
            { JT_TOPAWARD, JTRANS_NONE, minThreshold + 1, false },
            { JT_TOPAWARD, JTRANS_NONE, maxThreshold - 1, false },
            { JT_TOPAWARD, JTRANS_NONE, minThreshold, false },
            { JT_TOPAWARD, JTRANS_NONE, maxThreshold, false },
        };

        for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex) {
            //Setup
            CJackpotTrigger trigger(minThreshold, maxThreshold, AttendantPaidJackpot);
            CJackpot *jackpot(new CJackpot());
            jackpot->SetJackpotType(TEST_DATA[testIndex].inputJackpotType);
            jackpot->SetJackpotTransactionType(TEST_DATA[testIndex].inputJackpotTransactionType);
            jackpot->SetJackpotAmount(TEST_DATA[testIndex].inputJackpotAmount);

            //Test
            bool triggered = trigger.Triggered(jackpot);
            bool areEqual = triggered == TEST_DATA[testIndex].expectedTriggered;
            std::stringstream failedMsg;
            failedMsg << "FAILED Triggered=" << (int)triggered 
                << " expected=" << (int)TEST_DATA[testIndex].expectedTriggered
                << " in " << static_cast<LPCTSTR>(__TFUNCTION__);

            ASSERT_TRUE(areEqual) <<  failedMsg.str();

            //Cleanup
            delete jackpot;
        }
    }


    //Test the CJackpotTrigger AttendantPaidProgressiveJackpot type Triggered function
    TEST(SpeedMediaTriggerUnitTests, AttendantPaidProgressiveJackpotTriggeredFunctionTests) {
        const TestJackpotTriggerDataType TEST_DATA[] =
        {
            //JT_NONE type tests
            { JT_NONE, JTRANS_COMPOSITE, minThreshold - 1, false },
            { JT_NONE, JTRANS_COMPOSITE, maxThreshold + 1, false },
            { JT_NONE, JTRANS_COMPOSITE, minThreshold + 1, true },
            { JT_NONE, JTRANS_COMPOSITE, maxThreshold - 1, true },
            { JT_NONE, JTRANS_COMPOSITE, minThreshold, true },
            { JT_NONE, JTRANS_COMPOSITE, maxThreshold, true },
            { JT_NONE, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold - 1, false },
            { JT_NONE, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold + 1, false },
            { JT_NONE, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold + 1, true },
            { JT_NONE, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold - 1, true },
            { JT_NONE, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold, true },
            { JT_NONE, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold, true },
            { JT_NONE, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold - 1, false },
            { JT_NONE, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold + 1, false },
            { JT_NONE, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold + 1, true },
            { JT_NONE, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold - 1, true },
            { JT_NONE, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold, true },
            { JT_NONE, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold, true },
            { JT_NONE, JTRANS_NONE, minThreshold - 1, false },
            { JT_NONE, JTRANS_NONE, maxThreshold + 1, false },
            { JT_NONE, JTRANS_NONE, minThreshold + 1, false },
            { JT_NONE, JTRANS_NONE, maxThreshold - 1, false },
            { JT_NONE, JTRANS_NONE, minThreshold, false },
            { JT_NONE, JTRANS_NONE, maxThreshold, false },


            //JT_CANCEL_CREDIT type tests
            { JT_CANCEL_CREDIT, JTRANS_COMPOSITE, minThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_COMPOSITE, maxThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_COMPOSITE, minThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_COMPOSITE, maxThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_COMPOSITE, minThreshold, false },
            { JT_CANCEL_CREDIT, JTRANS_COMPOSITE, maxThreshold, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold + 1, true },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold - 1, true },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold, true },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold, true },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold + 1, true },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold - 1, true },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold, true },
            { JT_CANCEL_CREDIT, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold, true },
            { JT_CANCEL_CREDIT, JTRANS_NONE, minThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, maxThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, minThreshold + 1, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, maxThreshold - 1, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, minThreshold, false },
            { JT_CANCEL_CREDIT, JTRANS_NONE, maxThreshold, false },

            //JT_HANDPAY type tests ******************************************
            { JT_HANDPAY, JTRANS_COMPOSITE, minThreshold - 1, false },
            { JT_HANDPAY, JTRANS_COMPOSITE, maxThreshold + 1, false },
            { JT_HANDPAY, JTRANS_COMPOSITE, minThreshold + 1, false },
            { JT_HANDPAY, JTRANS_COMPOSITE, maxThreshold - 1, false },
            { JT_HANDPAY, JTRANS_COMPOSITE, minThreshold, false },
            { JT_HANDPAY, JTRANS_COMPOSITE, maxThreshold, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold - 1, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold + 1, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold + 1, true },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold - 1, true },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold, true },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold, true },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold - 1, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold + 1, false },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold + 1, true },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold - 1, true },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold, true },
            { JT_HANDPAY, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold, true },
            { JT_HANDPAY, JTRANS_NONE, minThreshold - 1, false },
            { JT_HANDPAY, JTRANS_NONE, maxThreshold + 1, false },
            { JT_HANDPAY, JTRANS_NONE, minThreshold + 1, false },
            { JT_HANDPAY, JTRANS_NONE, maxThreshold - 1, false },
            { JT_HANDPAY, JTRANS_NONE, minThreshold, false },
            { JT_HANDPAY, JTRANS_NONE, maxThreshold, false },

            //JT_TOPAWARD type tests ******************************************
            { JT_TOPAWARD, JTRANS_COMPOSITE, minThreshold - 1, false },
            { JT_TOPAWARD, JTRANS_COMPOSITE, maxThreshold + 1, false },
            { JT_TOPAWARD, JTRANS_COMPOSITE, minThreshold + 1, false },
            { JT_TOPAWARD, JTRANS_COMPOSITE, maxThreshold - 1, false },
            { JT_TOPAWARD, JTRANS_COMPOSITE, minThreshold, false },
            { JT_TOPAWARD, JTRANS_COMPOSITE, maxThreshold, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold - 1, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold + 1, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold + 1, true },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold - 1, true },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_HOPPER_PAID, minThreshold, true },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_HOPPER_PAID, maxThreshold, true },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold - 1, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold + 1, false },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold + 1, true },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold - 1, true },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_SCANNER_ONLY, minThreshold, true },
            { JT_TOPAWARD, JTRANS_PROGRESSIVE_SCANNER_ONLY, maxThreshold, true },
            { JT_TOPAWARD, JTRANS_NONE, minThreshold - 1, false },
            { JT_TOPAWARD, JTRANS_NONE, maxThreshold + 1, false },
            { JT_TOPAWARD, JTRANS_NONE, minThreshold + 1, false },
            { JT_TOPAWARD, JTRANS_NONE, maxThreshold - 1, false },
            { JT_TOPAWARD, JTRANS_NONE, minThreshold, false },
            { JT_TOPAWARD, JTRANS_NONE, maxThreshold, false },
        };

        for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex) {
            //Setup
            CJackpotTrigger trigger(minThreshold, maxThreshold, AttendantPaidProgressiveJackpot);
            CJackpot *jackpot(new CJackpot());
            jackpot->SetJackpotType(TEST_DATA[testIndex].inputJackpotType);
            jackpot->SetJackpotTransactionType(TEST_DATA[testIndex].inputJackpotTransactionType);
            jackpot->SetJackpotAmount(TEST_DATA[testIndex].inputJackpotAmount);

            //Test
            bool triggered = trigger.Triggered(jackpot);
            bool areEqual = triggered == TEST_DATA[testIndex].expectedTriggered;
            std::stringstream failedMsg;
            failedMsg << "FAILED Triggered=" << (int)triggered 
                << " expected=" << (int)TEST_DATA[testIndex].expectedTriggered
                << " in " << static_cast<LPCTSTR>(__TFUNCTION__);

            ASSERT_TRUE(areEqual) <<  failedMsg.str();

            //Cleanup
            delete jackpot;
        }
    }

    //Test the CTheoreticalWinTrigger Constructor, Copy constructor and Assignment operator
    TEST(SpeedMediaTriggerUnitTests, TheoreticalWinTriggerConstructorAndOperatorTests) {
        const TriggerClassType triggerType = TheoreticalWin;

        //Constructor Test
        CTheoreticalWinTrigger Trigger1(minThreshold, maxThreshold, triggerType);
        ASSERT_TRUE(Trigger1.GetMinThreshold()== minThreshold &&
                Trigger1.GetMaxThreshold()== maxThreshold &&
                Trigger1.GetType()== triggerType)
            << "CTheoreticalWinTrigger constructor failed.";

        //Copy Constructor Test
        CTheoreticalWinTrigger Trigger2 = Trigger1; //calls copy constructor
        ASSERT_TRUE(Trigger2.GetMinThreshold()== minThreshold &&
                Trigger2.GetMaxThreshold()== maxThreshold &&
                Trigger2.GetType()== triggerType)
            << "CTheoreticalWinTrigger copy constructor failed.";

        //Assignment Operator Test
        CTheoreticalWinTrigger Trigger3(minThreshold, maxThreshold, NoTrigger); //Make trigger type different
        Trigger3 = Trigger2; //calls assignment operator
        ASSERT_TRUE(Trigger3.GetMinThreshold() == minThreshold &&
                Trigger3.GetMaxThreshold() == maxThreshold &&
                Trigger3.GetType() == triggerType)
            << "CTheoreticalWinTrigger assignment operator failed.";
    }

    //Test the CTheoreticalWinTrigger equality operator
    TEST(SpeedMediaTriggerUnitTests, TheoreticalWinTriggerEqualityTest) {
        const TriggerClassType triggerType = TheoreticalWin;

        //Setup
        CTheoreticalWinTrigger Trigger1(minThreshold, maxThreshold, triggerType);
        CTheoreticalWinTrigger Trigger2(minThreshold, maxThreshold, triggerType); //equals Trigger1
        CTheoreticalWinTrigger Trigger3(minThreshold + 1, maxThreshold, triggerType); //Different m_minThreshold
        CTheoreticalWinTrigger Trigger4(minThreshold, maxThreshold + 1, triggerType); //Different m_maxThreshold
        CTheoreticalWinTrigger Trigger5(minThreshold, maxThreshold, NoTrigger); //Different m_triggerType

        //Tests, compare all triggers to Trigger1
        ASSERT_TRUE(Trigger1  == Trigger2) << "CTheoreticalWinTrigger overloaded == operator failed.";
        ASSERT_FALSE(Trigger1 == Trigger3) << "CTheoreticalWinTrigger overloaded == operator failed.";
        ASSERT_FALSE(Trigger1 == Trigger4) << "CTheoreticalWinTrigger overloaded == operator failed.";
        ASSERT_FALSE(Trigger1 == Trigger5) << "CTheoreticalWinTrigger overloaded == operator failed.";
    }

    //Test the CTheoreticalWinTrigger Trigger Function
    TEST(SpeedMediaTriggerUnitTests, TheoreticalWinTriggerFunctionTest) {
        const TestTriggerDataType TEST_DATA[] =
        {
            //Test 1
            { DENOM,  //denom
                1,  //initialCoinIn;
                0,      //initialCoinOut;
                0,      //initialExternalBonusingAttendantPayout;
                0,      //initialExternalBonusingMachinePayout;
                0,      //initialJackpotHandpay;
                0,      //initialMachinePayProgressivePayout;
                0,      //initialAttendantPayProgressivePayout;
                50,     //currentCoinIn;
                0,      //currentCoinOut;
                0,      //currentExternalBonusingAttendantPayout;
                0,      //currentExternalBonusingMachinePayout;
                0,      //currentJackpotHandpay;
                0,      //currentMachinePayProgressivePayout;
                0,      //currentAttendantPayProgressivePayout;
                10,         //par
                false }, //expectedTriggered

            //Test 2
            { DENOM,  //denom
                1,  //initialCoinIn;
                0,      //initialCoinOut;
                0,      //initialExternalBonusingAttendantPayout;
                0,      //initialExternalBonusingMachinePayout;
                0,      //initialJackpotHandpay;
                0,      //initialMachinePayProgressivePayout;
                0,      //initialAttendantPayProgressivePayout;
                51,     //currentCoinIn;
                0,      //currentCoinOut;
                0,      //currentExternalBonusingAttendantPayout;
                0,      //currentExternalBonusingMachinePayout;
                0,      //currentJackpotHandpay;
                0,      //currentMachinePayProgressivePayout;
                0,      //currentAttendantPayProgressivePayout;
                10,         //par
                true }, //expectedTriggered

            //Test 3
            { DENOM,  //denom
                1,  //initialCoinIn;
                0,      //initialCoinOut;
                0,      //initialExternalBonusingAttendantPayout;
                0,      //initialExternalBonusingMachinePayout;
                0,      //initialJackpotHandpay;
                0,      //initialMachinePayProgressivePayout;
                0,      //initialAttendantPayProgressivePayout;
                100,    //currentCoinIn;
                0,      //currentCoinOut;
                0,      //currentExternalBonusingAttendantPayout;
                0,      //currentExternalBonusingMachinePayout;
                0,      //currentJackpotHandpay;
                0,      //currentMachinePayProgressivePayout;
                0,      //currentAttendantPayProgressivePayout;
                10,         //par
                true },  //expectedTriggered

            //Test 4
            { DENOM,  //denom
                1,  //initialCoinIn;
                0,      //initialCoinOut;
                0,      //initialExternalBonusingAttendantPayout;
                0,      //initialExternalBonusingMachinePayout;
                0,      //initialJackpotHandpay;
                0,      //initialMachinePayProgressivePayout;
                0,      //initialAttendantPayProgressivePayout;
                102,    //currentCoinIn;
                0,      //currentCoinOut;
                0,      //currentExternalBonusingAttendantPayout;
                0,      //currentExternalBonusingMachinePayout;
                0,      //currentJackpotHandpay;
                0,      //currentMachinePayProgressivePayout;
                0,      //currentAttendantPayProgressivePayout;
                10,         //par
                false },  //expectedTriggered
        };

        for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex) {
            //Setup
            CTheoreticalWinTrigger trigger(minThreshold, maxThreshold, TheoreticalWin);

            //Test
            bool triggered = trigger.Triggered(TEST_DATA[testIndex].denom, //denom
                    TEST_DATA[testIndex].initialCoinIn,
                    TEST_DATA[testIndex].initialCoinOut,
                    TEST_DATA[testIndex].initialExternalBonusingAttendantPayout,
                    TEST_DATA[testIndex].initialExternalBonusingMachinePayout,
                    TEST_DATA[testIndex].initialJackpotHandpay,
                    TEST_DATA[testIndex].initialMachinePayProgressivePayout,
                    TEST_DATA[testIndex].initialAttendantPayProgressivePayout,
                    TEST_DATA[testIndex].currentCoinIn,
                    TEST_DATA[testIndex].currentCoinOut,
                    TEST_DATA[testIndex].currentExternalBonusingAttendantPayout,
                    TEST_DATA[testIndex].currentExternalBonusingMachinePayout,
                    TEST_DATA[testIndex].currentJackpotHandpay,
                    TEST_DATA[testIndex].currentMachinePayProgressivePayout,
                    TEST_DATA[testIndex].currentAttendantPayProgressivePayout,
                    TEST_DATA[testIndex].par);

            bool areEqual = triggered == TEST_DATA[testIndex].expectedTriggered;
            std::stringstream failedMsg;
            failedMsg << "FAILED Triggered=" << (int)triggered 
                << " expected=" << (int)TEST_DATA[testIndex].expectedTriggered
                << " in " << static_cast<LPCTSTR>(__TFUNCTION__);

            ASSERT_TRUE(areEqual) <<  failedMsg.str();
        }
    }

    //Test the CTimePlayedTrigger Constructor, Copy constructor and Assignment operator
    TEST(SpeedMediaTriggerUnitTests, TimePlayedTriggerConstructorAndOperatorTests) {
        const TriggerClassType triggerType = TimePlayed;

        //Constructor Test
        CTimePlayedTrigger Trigger1(minThreshold, maxThreshold, triggerType);
        ASSERT_TRUE(Trigger1.GetMinThreshold()== minThreshold &&
                Trigger1.GetMaxThreshold()== maxThreshold &&
                Trigger1.GetType()== triggerType)
            << "CTimePlayedTrigger constructor failed.";

        //Copy Constructor Test
        CTimePlayedTrigger Trigger2 = Trigger1; //calls copy constructor
        ASSERT_TRUE(Trigger2.GetMinThreshold()== minThreshold &&
                Trigger2.GetMaxThreshold()== maxThreshold &&
                Trigger2.GetType()== triggerType)
            << "CTimePlayedTrigger copy constructor failed.";

        //Assignment Operator Test
        CTimePlayedTrigger Trigger3(minThreshold, maxThreshold, NoTrigger); //Make trigger type different
        Trigger3 = Trigger2; //calls assignment operator
        ASSERT_TRUE(Trigger3.GetMinThreshold() == minThreshold &&
                Trigger3.GetMaxThreshold() == maxThreshold &&
                Trigger3.GetType() == triggerType)
            << "CTimePlayedTrigger assignment operator failed.";
    }

    //Test the CTimePlayedTrigger equality operator
    TEST(SpeedMediaTriggerUnitTests, TimePlayedTriggerEqualityTest) {
        const TriggerClassType triggerType = TimePlayed;

        //Setup
        CTimePlayedTrigger Trigger1(minThreshold, maxThreshold, triggerType);
        CTimePlayedTrigger Trigger2(minThreshold, maxThreshold, triggerType); //equals Trigger1
        CTimePlayedTrigger Trigger3(minThreshold + 1, maxThreshold, triggerType); //Different m_minThreshold
        CTimePlayedTrigger Trigger4(minThreshold, maxThreshold + 1, triggerType); //Different m_maxThreshold
        CTimePlayedTrigger Trigger5(minThreshold, maxThreshold, NoTrigger); //Different m_triggerType

        //Tests, compare all triggers to Trigger1
        ASSERT_TRUE(Trigger1  == Trigger2) << "CTimePlayedTrigger overloaded == operator failed.";
        ASSERT_FALSE(Trigger1 == Trigger3) << "CTimePlayedTrigger overloaded == operator failed.";
        ASSERT_FALSE(Trigger1 == Trigger4) << "CTimePlayedTrigger overloaded == operator failed.";
        ASSERT_FALSE(Trigger1 == Trigger5) << "CTimePlayedTrigger overloaded == operator failed.";
    }


}
