#include "stdafx.h"
#include "ProgressiveProtocol/EGMAward.h"
#include "UnitTestingUtils.h"
#include "TestUtil.h"
#include "RapidJsonHelper.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "UnitTestingUtils.h"

namespace SentinelUnitTests
{
    class EGMAward : public ::testing::Test
    {
    public:
        // Default expected result(s).
        static const __int64 m_awardAmountPaidExpected = 0;
        static const EGMAwardState m_awardStateExpected = (EGMAwardState)0; // Use the value required for backwards compatibility with NVRAM.
        static const int m_levelIdExpected = 0;
        static const LONG64 m_jackpotIdExpected = -1;
        static const LONG64 m_remoteJackpotIdExpected = -1;
        static const __int64 m_awardAmountHitExpected = 0;
        static const __int64 m_originalMachineBonusMeterValueExpected = -1;
        static const __int64 m_originalAttendantBonusingMeterValueExpected = -1;
        static const int m_sentToGameCountExpected = 0;
        static const SYSTEMTIME m_sentToGameTimeExpected;
        static const SYSTEMTIME m_receivedFromSystemTimeExpected;
        static const DWORD m_duplicateHitCountExpected = 0;
        static const DWORD m_otherHitCountExpected = 0;
        static const EGMAwardTransferType m_awardTransferTypeExpected = EGMAwardTransferType_None;
        static const MethodOfPayment::Type m_methodOfPaymentExpected = MethodOfPayment::Invalid;
        static const __int64 m_originalLegacyBonusMeterValueExpected = -1;

        // Default expected result(s) for v1.50.0 extension(s).
        static const EGMAwardPaidType m_awardPaidTypeExpected = EGMAwardPaidType_Unknown;

        static const bool m_isStandardProgressiveExpected = false;
        static const bool m_sessionIdIsValidExpected = false;
        static const int m_sessionIdExpected = 0;

        static const int m_sasLevelIdExpected = 0xff;
        static const bool m_isHitReceived = false;
        static const bool m_isHandpayPendingSignaled = false;
        static const bool m_isEgmPaidSignaled = false;
        static const SYSTEMTIME m_receivedFromEGMTimeExpected;

        static const bool m_inProgressExpected = false;
        static const string m_statusInfoExpected;
        static const string m_statusInfoBuildYourself;
        static const string m_logInfoStringExpected;

        // Default expected CMemoryStorage member values.
        static const MEMORY_TYPE m_memoryTypeExpected = MEMORY_NONE;
        static const DWORD m_dwAllowedSizeExpected = 710226; // Use the value required for backwards compatibility with NVRAM.
        static const DWORD m_dwOffsetExpected = 256; // Use the value required for backwards compatibility with NVRAM.
        static const bool m_bOffsetIsSetExpected = true;

        // Class member constant(s).
        static const SYSTEMTIME SYSTEMTIME_INITIAL_VALUE; // Arbitrary value when any valid value is okay.
        static const LPCSTR m_validJson; // Use to create a valid CEGMAward instance.

        // Unique non-default values.
        static const SYSTEMTIME m_sentToGameTime;
        static const SYSTEMTIME m_receivedFromSystemTime;
        static const SYSTEMTIME m_receivedFromEGMTime;

        // Class member variable(s).
        int m_testIndex;

        /// <summary>
        /// Initialize class member(s) before each test method is run.
        /// </summary>
        EGMAward()
        {
            // Test method initialization code.
            m_testIndex = 0;
        }

        void AssertEqual(const CEGMAward &obj,
            __int64 awardAmountPaidExpected,
            EGMAwardState awardStateExpected,
            int levelIdExpected,
            LONG64 jackpotIdExpected,
            LONG64 remoteJackpotIdExpected,
            __int64 awardAmountHitExpected,
            __int64 originalMachineBonusMeterValueExpected,
            __int64 originalAttendantBonusingMeterValueExpected,
            int sentToGameCountExpected,
            const SYSTEMTIME& sentToGameTimeExpected,
            const SYSTEMTIME& receivedFromSystemTimeExpected,
            DWORD duplicateHitCountExpected,
            DWORD otherHitCountExpected,
            bool inProgressExpected,
            const string& statusInfoExpected,
            const string& logInfoStringExpected,
            MEMORY_TYPE memoryTypeExpected,
            int testIndex,
            LPCTSTR functionName,
            int lineNumber,
            const int* extraIndex = NULL)
        {
            ASSERT_TRUE(obj.GetAwardAmountPaid() == awardAmountPaidExpected);
            ASSERT_TRUE(obj.GetAwardState() == awardStateExpected);
            ASSERT_TRUE(obj.GetLevelId() == levelIdExpected);
            ASSERT_TRUE(obj.GetJackpotId() == jackpotIdExpected);
            ASSERT_TRUE(obj.GetRemoteJackpotId() == remoteJackpotIdExpected);
            ASSERT_TRUE(obj.GetAwardAmountHit() == awardAmountHitExpected);
            ASSERT_TRUE(obj.GetOriginalMachineBonusMeterValue() == originalMachineBonusMeterValueExpected);
            ASSERT_TRUE(obj.GetOriginalAttendantBonusingMeterValue() == originalAttendantBonusingMeterValueExpected);
            ASSERT_TRUE(obj.GetSentToGameCount() == sentToGameCountExpected);
            ASSERT_TRUE(CUtilities::CompareSysTimes(obj.GetSentToGameTime(), sentToGameTimeExpected) == 0);
            ASSERT_TRUE(CUtilities::CompareSysTimes(obj.GetReceivedFromSystemTime(), receivedFromSystemTimeExpected) == 0);
            ASSERT_TRUE(obj.GetDuplicateHitCount() == duplicateHitCountExpected);
            ASSERT_TRUE(obj.GetOtherHitCount() == otherHitCountExpected);
            ASSERT_TRUE(obj.InProgress() == inProgressExpected);
            ASSERT_TRUE(obj.GetStatusInfo() == statusInfoExpected);
            ASSERT_TRUE(obj.GetLogInfoString(_T("")) == logInfoStringExpected);
            ASSERT_TRUE(obj.GetMemoryType() == memoryTypeExpected);
        }

        void GetJsonDocFromString(rapidjson::Document &jsonDoc, LPCSTR json)
        {
            // Setup.
            jsonDoc.Clear();
            if (jsonDoc.Parse(json).HasParseError())
                throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");
        }
    };

    /// <summary>
    /// Code coverage and functional test for:
    ///     CEGMAward::CEGMAward()
    ///     CEGMAward::GetAwardAmountPaid()
    ///     CEGMAward::GetAwardState()
    ///     CEGMAward::GetLevelId()
    ///     CEGMAward::GetJackpotId()
    ///     CEGMAward::GetRemoteJackpotId()
    ///     CEGMAward::GetAwardAmountHit()
    ///     CEGMAward::GetOriginalMachineBonusMeterValue()
    ///     CEGMAward::GetOriginalAttendantBonusingMeterValue()
    ///     CEGMAward::GetSentToGameCount()
    ///     CEGMAward::GetSentToGameTime()
    ///     CEGMAward::GetDuplicateHitCount()
    ///     CEGMAward::GetOtherHitCount()
    ///     CEGMAward::GetLogInfoString()
    ///     CEGMAward::GetMemoryType()
    /// Code coverage:
    ///     CEGMAward::~CEGMAward()
    /// </summary>
    TEST_F(EGMAward, CEGMAwardTest)
    {
        // Setup.
        const int testIndex(0);

        // Perform operation(s) to be tested.
        const CEGMAward obj;

        // Test(s).
        AssertEqual(obj,
            m_awardAmountPaidExpected,
            m_awardStateExpected,
            m_levelIdExpected,
            m_jackpotIdExpected,
            m_remoteJackpotIdExpected,
            m_awardAmountHitExpected,
            m_originalMachineBonusMeterValueExpected,
            m_originalAttendantBonusingMeterValueExpected,
            m_sentToGameCountExpected,
            m_sentToGameTimeExpected,
            m_receivedFromSystemTimeExpected,
            m_duplicateHitCountExpected,
            m_otherHitCountExpected,
            m_inProgressExpected,
            m_statusInfoExpected,
            m_logInfoStringExpected,
            m_memoryTypeExpected,
            testIndex, __TFUNCTION__, __LINE__);
    }

    TEST_F(EGMAward, EGMAwardRoundingTest)
    {
        CEGMAward award;

        // award amount in cents
        award.SetAwardAmountHit(515);

        // test rounding with different denoms
        award.RoundAndSetAwardAmountPaid(1);
        ASSERT_TRUE(award.GetAwardAmountPaid() == 515);

        award.RoundAndSetAwardAmountPaid(5);
        ASSERT_TRUE(award.GetAwardAmountPaid() == 515);

        award.RoundAndSetAwardAmountPaid(25);
        ASSERT_TRUE(award.GetAwardAmountPaid() == 525);

        award.RoundAndSetAwardAmountPaid(100);
        ASSERT_TRUE(award.GetAwardAmountPaid() == 600);

        // Test to make sure 0 denom doesn't result in divide by zero exception
        award.RoundAndSetAwardAmountPaid(0);
        ASSERT_TRUE(award.GetAwardAmountPaid() == 515);

    }

    TEST_F(EGMAward, EGMAwardIsJackpotMatchTest)
    {
        CEGMAward award;

        award.SetAwardAmountHit(10001);
        award.RoundAndSetAwardAmountPaid(1);

        award.SetAwardState(NoAward);
        ASSERT_FALSE(award.IsJackpotMatch(100));
        ASSERT_FALSE(award.IsJackpotMatch(10001));

        award.SetAwardState(ReceivedFromSystem);
        ASSERT_FALSE(award.IsJackpotMatch(100));
        ASSERT_FALSE(award.IsJackpotMatch(10001));

        award.SetAwardState(WaitForMetersInitial);
        ASSERT_FALSE(award.IsJackpotMatch(100));
        ASSERT_FALSE(award.IsJackpotMatch(10001));

        award.SetAwardState(SentToGame);
        ASSERT_FALSE(award.IsJackpotMatch(100));
        ASSERT_TRUE(award.IsJackpotMatch(10001));

        award.SetAwardState(GameBusy);
        ASSERT_FALSE(award.IsJackpotMatch(100));
        ASSERT_FALSE(award.IsJackpotMatch(10001));

        award.SetAwardState(Signaled);
        ASSERT_FALSE(award.IsJackpotMatch(100));
        ASSERT_TRUE(award.IsJackpotMatch(10001));

        award.SetAwardState(AwardToGameSuccess);
        ASSERT_FALSE(award.IsJackpotMatch(100));
        ASSERT_FALSE(award.IsJackpotMatch(10001));

        award.SetAwardState(AwardToGameError);
        ASSERT_FALSE(award.IsJackpotMatch(100));
        ASSERT_FALSE(award.IsJackpotMatch(10001));

        award.SetAwardState(AwardToGameIneligible);
        ASSERT_FALSE(award.IsJackpotMatch(100));
        ASSERT_FALSE(award.IsJackpotMatch(10001));

        award.SetAwardState(AftLockPending);
        ASSERT_FALSE(award.IsJackpotMatch(100));
        ASSERT_TRUE(award.IsJackpotMatch(10001));

        award.SetAwardState(AwardSessionMismatch);
        ASSERT_FALSE(award.IsJackpotMatch(100));
        ASSERT_FALSE(award.IsJackpotMatch(10001));

    }

    /// <summary>
    /// Test that persisted enumeration values have not changed across versions.
    /// </summary>
    TEST_F(EGMAward, EGMAwardState_Persist)
    {
        // Perform test(s).
        ASSERT_TRUE((EGMAwardState)0 == NoAward);
        ASSERT_TRUE((EGMAwardState)1 == ReceivedFromSystem);
        ASSERT_TRUE((EGMAwardState)2 == WaitForMetersInitial);
        ASSERT_TRUE((EGMAwardState)3 == SentToGame);
        ASSERT_TRUE((EGMAwardState)4 == GameBusy);
        ASSERT_TRUE((EGMAwardState)5 == Signaled);
        ASSERT_TRUE((EGMAwardState)6 == HandpayPending);
        ASSERT_TRUE((EGMAwardState)7 == AwardToGameSuccess);
        ASSERT_TRUE((EGMAwardState)8 == AwardToGameError);
        ASSERT_TRUE((EGMAwardState)9 == AwardToGameIneligible);
        ASSERT_TRUE((EGMAwardState)10 == AftLockPending);
        ASSERT_TRUE((EGMAwardState)11 == AwardSessionMismatch);
        ASSERT_TRUE((EGMAwardState)12 == ReceivedFromGame);
        ASSERT_TRUE((EGMAwardState)13 == SentToSystem);
    }

    // #pragma region IsValidJackpotHitInitalization
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward()
    ///     CEGMAward::IsValidJackpotHitInitalization()
    /// Test for valid values from the json constructor.
    /// </summary>
    TEST_F(EGMAward, IsValidJackpotHitInitalization_JsonValid)
    {
        // Data.
        const SYSTEMTIME receivedTime = { 2013, 1, 0, 11, 12, 34, 56, 789 };

        // Setup.
        rapidjson::Document jsonDoc;
        GetJsonDocFromString(jsonDoc, m_validJson);
        CEGMAward obj(receivedTime, jsonDoc);

        // Perform operation(s) and test(s).
        ASSERT_TRUE(obj.IsValidJackpotHitInitalization());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward()
    ///     CEGMAward::IsValidJackpotHitInitalization()
    /// Test for invalid JackpotId value from the json constructor.
    /// </summary>
    TEST_F(EGMAward, IsValidJackpotHitInitalization_JsonInvalidJackpotId)
    {
        // Data.
        const SYSTEMTIME receivedTime = { 2013, 1, 0, 11, 12, 34, 56, 789 };
        static const LPCSTR json(
            "{"
            "\"JackpotId\":-1,"
            "\"RemoteJackpotId\":0,"
            "\"WinningDeviceId\":6467,"
            "\"LevelId\":23886,"
            "\"LevelName\":\"JacksonTest\","
            "\"OverageAdjustment\":0,"
            "\"Amount\":3,"
            "\"OriginalAmount\":0,"
            "\"JackpotType\":\"Bonus\","
            "\"JackpotTriggerType\":\"CoinInTrigger\","
            "\"PriorityAward\":false,"
            "\"WinningAssetNumber\":\"113087\","
            "\"Remote\":false,"
            "\"MethodOfPayment\":\"PayToGame\","
            "\"AwardType\":\"CashableCredits\","
            "\"SessionId\":123123,"
            "\"SourceDeviceId\":1,"
            "\"SourceDeviceConfigVersion\":0,"
            "\"SourceAsset\":\"\","
            "\"Timestamp\":1434039783129,"
            "\"MessageId\":\"909-1\""
            "}");
        string statusInfoExpected(_T("Internals received invalid jackpot hit: jackpotId=-1 awardAmountHit=300 methodOfPayment=0"));

        // Setup.
        rapidjson::Document jsonDoc;
        GetJsonDocFromString(jsonDoc, json);
        CEGMAward obj(receivedTime, jsonDoc);

        // Perform operation(s) and test(s).
        ASSERT_FALSE(obj.IsValidJackpotHitInitalization());
        ASSERT_TRUE(statusInfoExpected == obj.GetStatusInfo());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward()
    ///     CEGMAward::IsValidJackpotHitInitalization()
    /// Test for invalid Amout value from the json constructor.
    /// </summary>
    TEST_F(EGMAward, IsValidJackpotHitInitalization_JsonInvalidAmount)
    {
        // Data.
        const SYSTEMTIME receivedTime = { 2013, 1, 0, 11, 12, 34, 56, 789 };
        static const LPCSTR json(
            "{"
            "\"JackpotId\":909,"
            "\"RemoteJackpotId\":0,"
            "\"WinningDeviceId\":6467,"
            "\"LevelId\":23886,"
            "\"LevelName\":\"JacksonTest\","
            "\"OverageAdjustment\":0,"
            "\"Amount\":0,"
            "\"OriginalAmount\":0,"
            "\"JackpotType\":\"Bonus\","
            "\"JackpotTriggerType\":\"CoinInTrigger\","
            "\"PriorityAward\":false,"
            "\"WinningAssetNumber\":\"113087\","
            "\"Remote\":false,"
            "\"MethodOfPayment\":\"PayToGame\","
            "\"AwardType\":\"CashableCredits\","
            "\"SessionId\":123123,"
            "\"SourceDeviceId\":1,"
            "\"SourceDeviceConfigVersion\":0,"
            "\"SourceAsset\":\"\","
            "\"Timestamp\":1434039783129,"
            "\"MessageId\":\"909-1\""
            "}");
        string statusInfoExpected(_T("Internals received invalid jackpot hit: jackpotId=909 awardAmountHit=0 methodOfPayment=0"));

        // Setup.
        rapidjson::Document jsonDoc;
        GetJsonDocFromString(jsonDoc, json);
        CEGMAward obj(receivedTime, jsonDoc);

        // Perform operation(s) and test(s).
        ASSERT_FALSE(obj.IsValidJackpotHitInitalization());
        ASSERT_TRUE(statusInfoExpected == obj.GetStatusInfo());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward()
    ///     CEGMAward::IsValidJackpotHitInitalization()
    /// Test for invalid Amout value from the json constructor.
    /// </summary>
    TEST_F(EGMAward, IsValidJackpotHitInitalization_JsonInvalidMethodOfPayment)
    {
        // Data.
        const SYSTEMTIME receivedTime = { 2013, 1, 0, 11, 12, 34, 56, 789 };
        static const LPCSTR json(
            "{"
            "\"JackpotId\":909,"
            "\"RemoteJackpotId\":0,"
            "\"WinningDeviceId\":6467,"
            "\"LevelId\":23886,"
            "\"LevelName\":\"JacksonTest\","
            "\"OverageAdjustment\":0,"
            "\"Amount\":3,"
            "\"OriginalAmount\":0,"
            "\"JackpotType\":\"Bonus\","
            "\"JackpotTriggerType\":\"CoinInTrigger\","
            "\"PriorityAward\":false,"
            "\"WinningAssetNumber\":\"113087\","
            "\"Remote\":false,"
            "\"MethodOfPayment\":\"PayToPlayerAccount\","
            "\"AwardType\":\"CashableCredits\","
            "\"SessionId\":123123,"
            "\"SourceDeviceId\":1,"
            "\"SourceDeviceConfigVersion\":0,"
            "\"SourceAsset\":\"\","
            "\"Timestamp\":1434039783129,"
            "\"MessageId\":\"909-1\""
            "}");
        string statusInfoExpected(_T("Internals received invalid jackpot hit: jackpotId=909 awardAmountHit=300 methodOfPayment=3"));

        // Setup.
        rapidjson::Document jsonDoc;
        GetJsonDocFromString(jsonDoc, json);
        CEGMAward obj(receivedTime, jsonDoc);

        // Perform operation(s) and test(s).
        ASSERT_FALSE(obj.IsValidJackpotHitInitalization());
        ASSERT_TRUE(statusInfoExpected == obj.GetStatusInfo());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::IsValidJackpotHitInitalization()
    /// Test for valid values from the json constructor.
    /// </summary>
    TEST_F(EGMAward, IsValidJackpotHitInitalization_InvalidAwardState)
    {
        // Data.
        const SYSTEMTIME receivedTime = { 2013, 1, 0, 11, 12, 34, 56, 789 };
        string statusInfoExpected(_T("Internals initialized invalid award: awardState=0 origMachBonusMeter=-1 origAttnBonusMeter=-1 awardTransferType=0"));

        // Setup.
        rapidjson::Document jsonDoc;
        GetJsonDocFromString(jsonDoc, m_validJson);
        CEGMAward obj(receivedTime, jsonDoc);
        obj.SetAwardState(NoAward);

        // Perform operation(s) and test(s).
        ASSERT_FALSE(obj.IsValidJackpotHitInitalization());
        ASSERT_TRUE(statusInfoExpected == obj.GetStatusInfo());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::IsValidJackpotHitInitalization()
    /// Test for valid values from the json constructor.
    /// </summary>
    TEST_F(EGMAward, IsValidJackpotHitInitalization_InvalidMachBonusMeter)
    {
        // Data.
        const SYSTEMTIME receivedTime = { 2013, 1, 0, 11, 12, 34, 56, 789 };
        string statusInfoExpected(_T("Internals initialized invalid award: awardState=1 origMachBonusMeter=0 origAttnBonusMeter=-1 awardTransferType=0"));

        // Setup.
        rapidjson::Document jsonDoc;
        GetJsonDocFromString(jsonDoc, m_validJson);
        CEGMAward obj(receivedTime, jsonDoc);
        obj.SetOriginalMachineBonusMeterValue(0);

        // Perform operation(s) and test(s).
        ASSERT_FALSE(obj.IsValidJackpotHitInitalization());
        ASSERT_TRUE(statusInfoExpected == obj.GetStatusInfo());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::IsValidJackpotHitInitalization()
    /// Test for valid values from the json constructor.
    /// </summary>
    TEST_F(EGMAward, IsValidJackpotHitInitalization_InvalidAttnBonusMeter)
    {
        // Data.
        const SYSTEMTIME receivedTime = { 2013, 1, 0, 11, 12, 34, 56, 789 };
        string statusInfoExpected(_T("Internals initialized invalid award: awardState=1 origMachBonusMeter=-1 origAttnBonusMeter=0 awardTransferType=0"));

        // Setup.
        rapidjson::Document jsonDoc;
        GetJsonDocFromString(jsonDoc, m_validJson);
        CEGMAward obj(receivedTime, jsonDoc);
        obj.SetOriginalAttendantBonusingMeterValue(0);

        // Perform operation(s) and test(s).
        ASSERT_FALSE(obj.IsValidJackpotHitInitalization());
        ASSERT_TRUE(statusInfoExpected == obj.GetStatusInfo());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::IsValidJackpotHitInitalization()
    /// Test for valid values from the json constructor.
    /// </summary>
    TEST_F(EGMAward, IsValidJackpotHitInitalization_InvalidAwardTransferType)
    {
        // Data.
        const SYSTEMTIME receivedTime = { 2013, 1, 0, 11, 12, 34, 56, 789 };
        string statusInfoExpected(_T("Internals initialized invalid award:")
            _T(" awardState=1 origMachBonusMeter=-1 origAttnBonusMeter=-1 awardTransferType=1"));

        // Setup.
        rapidjson::Document jsonDoc;
        GetJsonDocFromString(jsonDoc, m_validJson);
        CEGMAward obj(receivedTime, jsonDoc);
        obj.SetAwardTransferType(EGMAwardTransferType_SASLegacyBonusing);

        // Perform operation(s) and test(s).
        ASSERT_FALSE(obj.IsValidJackpotHitInitalization());
        ASSERT_TRUE(statusInfoExpected == obj.GetStatusInfo());
    }
    // #pragma endregion IsValidJackpotHitInitalization

    // #pragma region CEGMAward::GetAwardState
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward()
    ///     CEGMAward::GetAwardState()
    /// Test default value.
    /// </summary>
    TEST_F(EGMAward, GetAwardState_Default)
    {
        const CEGMAward obj;
        ASSERT_TRUE(obj.GetAwardState() == NoAward);
    }

    // #pragma region CEGMAward::GetAwardStateString
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward()
    ///     CEGMAward::GetAwardStateString()
    /// Test default value.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_Default)
    {
        const CEGMAward obj;
        ASSERT_TRUE(obj.GetAwardStateString() == string(_T("No Award")));
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting to the minimum enumeration value, less one.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_MinimumLessOne)
    {
        CEGMAward varObj;
        varObj.SetAwardState((EGMAwardState)(NoAward - 1));
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(obj.GetAwardStateString() == string(_T("N/A")));
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting a specific enumeration value.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_ReceivedFromSystem)
    {
        CEGMAward varObj;
        varObj.SetAwardState(ReceivedFromSystem);
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(string(_T("Received From System")) == obj.GetAwardStateString());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting a specific enumeration value.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_WaitForMetersInitial)
    {
        CEGMAward varObj;
        varObj.SetAwardState(WaitForMetersInitial);
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(string(_T("Wait For Meters Initial")) == obj.GetAwardStateString());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting a specific enumeration value.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_NoAward)
    {
        CEGMAward varObj;
        varObj.SetAwardState(NoAward);
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(string(_T("No Award")) == obj.GetAwardStateString());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting a specific enumeration value.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_SentToGame)
    {
        CEGMAward varObj;
        varObj.SetAwardState(SentToGame);
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(string(_T("Sent To Game")) == obj.GetAwardStateString());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting a specific enumeration value.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_GameBusy)
    {
        CEGMAward varObj;
        varObj.SetAwardState(GameBusy);
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(string(_T("Game Busy")) == obj.GetAwardStateString());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting a specific enumeration value.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_Signaled)
    {
        CEGMAward varObj;
        varObj.SetAwardState(Signaled);
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(string(_T("Signaled on Game")) == obj.GetAwardStateString());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting a specific enumeration value.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_HandpayPending)
    {
        CEGMAward varObj;
        varObj.SetAwardState(HandpayPending);
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(string(_T("Handpay is pending")) == obj.GetAwardStateString());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting a specific enumeration value.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_AwardToGameSuccess)
    {
        CEGMAward varObj;
        varObj.SetAwardState(AwardToGameSuccess);
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(string(_T("Award To Game Success")) == obj.GetAwardStateString());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting a specific enumeration value.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_AwardToGameError)
    {
        CEGMAward varObj;
        varObj.SetAwardState(AwardToGameError);
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(string(_T("Award To Game Error")) == obj.GetAwardStateString());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting a specific enumeration value.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_AwardToGameIneligible)
    {
        CEGMAward varObj;
        varObj.SetAwardState(AwardToGameIneligible);
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(string(_T("Award To Game Ineligible")) == obj.GetAwardStateString());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting a specific enumeration value.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_AftLockPending)
    {
        CEGMAward varObj;
        varObj.SetAwardState(AftLockPending);
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(string(_T("Game Lock Pending")) == obj.GetAwardStateString());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting a specific enumeration value.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_AwardSessionMismatch)
    {
        CEGMAward varObj;
        varObj.SetAwardState(AwardSessionMismatch);
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(string(_T("Award Session Mismatch")) == obj.GetAwardStateString());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting a specific enumeration value.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_ReceivedFromGame)
    {
        CEGMAward varObj;
        varObj.SetAwardState(ReceivedFromGame);
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(string(_T("Received From Game")) == obj.GetAwardStateString());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting a specific enumeration value.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_SentToSystem)
    {
        CEGMAward varObj;
        varObj.SetAwardState(SentToSystem);
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(string(_T("Sent To System")) == obj.GetAwardStateString());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetAwardStateString()
    ///     CEGMAward::GetAwardStateString()
    /// Test setting to the maximum enumeration value, plus one.
    /// </summary>
    TEST_F(EGMAward, GetAwardStateString_MaximumPlusOne)
    {
        CEGMAward varObj;
        varObj.SetAwardState((EGMAwardState)(SentToSystem + 1));
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(obj.GetAwardStateString() == string(_T("N/A")));
    }

    /// <summary>
    /// Test that persisted enumeration values have not changed across versions.
    /// </summary>
    TEST_F(EGMAward, EGMAwardTransferType_Persist)
    {
        // Perform test(s).
        ASSERT_TRUE((EGMAwardTransferType)0 == EGMAwardTransferType_None);
        ASSERT_TRUE((EGMAwardTransferType)1 == EGMAwardTransferType_SASLegacyBonusing);
        ASSERT_TRUE((EGMAwardTransferType)2 == EGMAwardTransferType_SASAFTBonusingCash);
    }
    
    /// <summary>
    /// Test that persisted enumeration values have not changed across versions.
    /// </summary>
    TEST_F(EGMAward, EGMAwardPaidType_Persist)
    {
        // Perform test(s).
        ASSERT_TRUE((EGMAwardPaidType)0 == EGMAwardPaidType_Unknown);
        ASSERT_TRUE((EGMAwardPaidType)1 == EGMAwardPaidType_Attendant);
        ASSERT_TRUE((EGMAwardPaidType)2 == EGMAwardPaidtype_Machine);
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward()
    ///     CEGMAward::GetSessionId()
    /// Test with value omitted.
    /// </summary>
    TEST_F(EGMAward, GetSessionId_Omitted)
    {
        // Data.
        const SYSTEMTIME receivedTime(SYSTEMTIME_INITIAL_VALUE);

        static const LPCSTR json(
            "{"
            "\"JackpotId\":909,"
            "\"RemoteJackpotId\":0,"
            "\"WinningDeviceId\":6467,"
            "\"LevelId\":23886,"
            "\"LevelName\":\"JacksonTest\","
            "\"OverageAdjustment\":0,"
            "\"Amount\":3,"
            "\"OriginalAmount\":0,"
            "\"JackpotType\":\"Bonus\","
            "\"JackpotTriggerType\":\"CoinInTrigger\","
            "\"PriorityAward\":false,"
            "\"WinningAssetNumber\":\"113087\","
            "\"Remote\":false,"
            "\"MethodOfPayment\":\"PayToGame\","
            "\"AwardType\":\"CashableCredits\","
            // SessionId is omitted for this test.
            "\"SourceDeviceId\":1,"
            "\"SourceDeviceConfigVersion\":0,"
            "\"SourceAsset\":\"\","
            "\"Timestamp\":1434039783129,"
            "\"MessageId\":\"909-1\""
            "}");

        // Setup.
        rapidjson::Document jsonDoc;
        if (!jsonDoc.Parse(json).HasParseError())
        {
            // Perform operation(s) and test(s).
            CEGMAward award(receivedTime, jsonDoc);
            int sessionId;
            ASSERT_TRUE(award.GetSessionId(sessionId) == false);
        }
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward()
    ///     CEGMAward::GetStatusInfo()
    /// Test default value.
    /// </summary>
    TEST_F(EGMAward, GetStatusInfo_Default)
    {
        // Perform operation(s) and test(s).
        const CEGMAward obj;
        ASSERT_TRUE(obj.GetStatusInfo() == m_statusInfoExpected);
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward()
    ///     CEGMAward::GetStatusInfo()
    /// Test BuildYourself value.
    /// </summary>
    TEST_F(EGMAward, GetStatusInfo_BuildYourself_NvramInvalid)
    {
        // Setup.
        CUnitTestingUtils::SetNvramInvalid();

        // Perform operation(s) and test(s).
        const CEGMAward obj(MEMORY_NVRAM, true, NVRAM_EGM_AWARD_CHECKBYTE_OFFSET);
        ASSERT_TRUE(obj.GetStatusInfo() == m_statusInfoExpected);
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward()
    ///     CEGMAward::GetStatusInfo()
    /// Test BuildYourself value.
    /// </summary>
    TEST_F(EGMAward, GetStatusInfo_BuildYourself_NvramValid)
    {
        // Data.
        const string statusInfoOther("Other value");
    
        // Setup.
        CUnitTestingUtils::SetNvramValidAndUpgraded();
        {
            CEGMAward varObj(MEMORY_NVRAM, true, NVRAM_EGM_AWARD_CHECKBYTE_OFFSET);
            varObj.SetAwardState(AwardToGameError);
            varObj.SetStatusInfo(statusInfoOther);
            varObj.StoreYourself();
        }
    
        // Perform operation(s) and test(s).
        const CEGMAward obj(MEMORY_NVRAM, true, NVRAM_EGM_AWARD_CHECKBYTE_OFFSET);
        ASSERT_TRUE(obj.GetStatusInfo() == m_statusInfoBuildYourself);
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetStatusInfo()
    ///     CEGMAward::GetStatusInfo()
    /// Test non-default value.
    /// </summary>
    TEST_F(EGMAward, GetStatusInfo_NonNull)
    {
        // Data.
        const string statusInfoExpected("Test value");
    
        // Setup.
        CEGMAward varObj;
    
        // Perform operation(s) and test(s).
        varObj.SetStatusInfo(statusInfoExpected);
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(obj.GetStatusInfo() == statusInfoExpected);
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(&CEGMAward)
    ///     CEGMAward::GetStatusInfo()
    /// Test copy constructor with a non-default value.
    /// </summary>
    TEST_F(EGMAward, GetStatusInfo_Copy)
    {
        // Data.
        const string statusInfoExpected("Test value");
    
        // Setup.
        CEGMAward varObjA;
        varObjA.SetStatusInfo(statusInfoExpected);
        const CEGMAward &objA(varObjA);
        ASSERT_TRUE(objA.GetStatusInfo() == statusInfoExpected); // Verify setup.
    
        // Perform operation(s) and test(s).
        const CEGMAward &objB(objA);
        ASSERT_TRUE(objB.GetStatusInfo() == statusInfoExpected);
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::operator=()
    ///     CEGMAward::GetStatusInfo()
    /// Test assigment operator with non-default values.
    /// </summary>
    TEST_F(EGMAward, GetStatusInfo_Assignment)
    {
        // Data.
        const string statusInfoExpected("Test value");
        const string statusInfoOther("Other value");
    
        // Setup.
        CEGMAward varObjA;
        varObjA.SetStatusInfo(statusInfoExpected);
        const CEGMAward &objA(varObjA);
        ASSERT_TRUE(objA.GetStatusInfo() == statusInfoExpected); // Verify setup.
        CEGMAward varObjB;
        varObjB.SetStatusInfo(statusInfoOther);
        ASSERT_TRUE(varObjB.GetStatusInfo() == statusInfoOther); // Verify setup.
    
        // Perform operation(s) and test(s).
        varObjB = objA;
        const CEGMAward &objB(varObjB);
        ASSERT_TRUE(objB.GetStatusInfo() == statusInfoExpected);
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetStatusInfo()
    ///     CEGMAward::GetStatusInfo()
    /// Test non-default value then nullptr.
    /// </summary>
    TEST_F(EGMAward, GetStatusInfo_NonNull_Null)
    {
        // Data.
        const string statusInfoExpected("Test value");
    
        // Setup.
        CEGMAward varObj;
        varObj.SetStatusInfo(statusInfoExpected);
        ASSERT_TRUE(varObj.GetStatusInfo() == statusInfoExpected); // Verify setup.
    
        // Perform operation(s) and test(s).
        varObj.SetStatusInfo("");
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(obj.GetStatusInfo() == statusInfoExpected);
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetStatusInfo()
    ///     CEGMAward::GetStatusInfo()
    /// Test non-default value then zero length string.
    /// </summary>
    TEST_F(EGMAward, GetStatusInfo_NonNull_ZeroLength)
    {
        // Data.
        const string statusInfoExpected("Test value");
    
        // Setup.
        CEGMAward varObj;
        varObj.SetStatusInfo(statusInfoExpected);
        ASSERT_TRUE(varObj.GetStatusInfo() == statusInfoExpected); // Verify setup.
    
        // Perform operation(s) and test(s).
        varObj.SetStatusInfo(_T(""));
        const CEGMAward &obj(varObj);
        ASSERT_TRUE(obj.GetStatusInfo() == statusInfoExpected);
    }
    
    /// <summary>
    /// Tests CEGMAward::SetStatusInfo() and CEGMAward::GetStatusInfo.
    /// Tests that the value set is the value returned, except that setting an empty string returns the previous value.
    /// </summary>
    /// <param name="obj">IN/OUT - The object to test.</param>
    /// <param name="testValue">IN - The test value.</param>
    void TestSetStatusInfo(CEGMAward &obj, const string &testValue)
    {
        if (testValue.empty())
        {
            string previousValue(obj.GetStatusInfo());
            obj.SetStatusInfo(testValue);
            ASSERT_TRUE(previousValue == obj.GetStatusInfo());
        }
        else
        {
            obj.SetStatusInfo(testValue);
            ASSERT_TRUE(testValue == obj.GetStatusInfo());
        }
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetStatusInfo()
    ///     CEGMAward::GetStatusInfo()
    /// Test with various values.
    /// </summary>
    TEST_F(EGMAward, GetStatusInfo_Data)
    {
        // Setup.
        CEGMAward obj;
    
        // Perform operation(s) and test(s).
        // Leading and/or trailing whitespace or lack thereof.
        TestSetStatusInfo(obj, _T(""));
        TestSetStatusInfo(obj, _T(" "));
        TestSetStatusInfo(obj, _T("  "));
        TestSetStatusInfo(obj, _T(" ab "));
        TestSetStatusInfo(obj, _T("abc "));
        TestSetStatusInfo(obj, _T("abc\t "));
        TestSetStatusInfo(obj, _T("abc \t"));
        TestSetStatusInfo(obj, _T("abc\n "));
        TestSetStatusInfo(obj, _T("abc def"));
        TestSetStatusInfo(obj, _T("abc\t def"));
        TestSetStatusInfo(obj, _T("abc \tdef"));
        TestSetStatusInfo(obj, _T("abc\n def"));
        TestSetStatusInfo(obj, _T("abc \ndef"));
        TestSetStatusInfo(obj, _T(" def"));
        TestSetStatusInfo(obj, _T("\t def"));
        TestSetStatusInfo(obj, _T("\n def"));
        TestSetStatusInfo(obj, _T(" \ndef"));
    
        // All displayable ASCII characters, with imbedded quote and apostrophe.
        TestSetStatusInfo(obj, _T("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"));
    
        // All displayable ASCII characters, with trailing double quote.
        TestSetStatusInfo(obj, _T("!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\""));
    
        // All displayable ASCII characters, with trailing apostrophe.
        TestSetStatusInfo(obj, _T("!\"#$%&()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~'"));
    
        // 300 characters.
        TestSetStatusInfo(obj,
            _T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789.")
            _T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789.")
            _T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789."));
    
        // Test that an empty string does not overwrite the previous value.
        TestSetStatusInfo(obj, _T(""));
    }
    // #pragma endregion CEGMAward::GetStatusInfo
    
    // #pragma region GetSASLevelId
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward()
    ///     CEGMAward::GetSASLevelId()
    /// Test for default value(s) using the default constructor.
    /// </summary>
    TEST_F(EGMAward, GetSASLevelId_Default)
    {
        // Data.
        const int sasLevelIdExpected(255);
    
        // Perform operation(s) and test(s).
        CEGMAward obj;
        ASSERT_TRUE(sasLevelIdExpected == obj.GetSASLevelId());
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(const SYSTEMTIME&)
    ///     CEGMAward::GetSASLevelId()
    /// Test for default value(s) using the JSON constructor.
    /// </summary>
    TEST_F(EGMAward, GetSASLevelId_DefaultJson)
    {
        // Data.
        const int sasLevelIdExpected(255);
    
        const SYSTEMTIME receivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
        static const char *json =
            "{"
            "\"LevelId\":1,"
            "\"JackpotId\":2,"
            "\"RemoteJackpotId\":3,"
            "\"Amount\":4.44,"
            "\"JackpotType\":\"StandardProgressive\","
            "\"MethodOfPayment\":\"PayToGame\","
            "\"SessionId\":5"
            "}";
        rapidjson::Document jsonDoc;
        ASSERT_FALSE(jsonDoc.Parse(json).HasParseError());
    
        // Perform operation(s) and test(s).
        CEGMAward obj(receivedTime, jsonDoc);
        ASSERT_TRUE(sasLevelIdExpected == obj.GetSASLevelId());
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(MEMORY_TYPE, ...)
    ///     CEGMAward::GetSASLevelId()
    /// Test for default value(s) using the MEMORY_TYPE constructor.
    /// </summary>
    TEST_F(EGMAward, GetSASLevelId_DefaultMemory)
    {
        // Data.
        const int sasLevelIdExpected(255);
    
        // Perform operation(s) and test(s).
        CEGMAward obj(MEMORY_NONE, false, 0);
        ASSERT_TRUE(sasLevelIdExpected == obj.GetSASLevelId());
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetSASLevelId()
    ///     CEGMAward::GetSASLevelId()
    /// Test SetSASLevelId() with various value(s).
    /// </summary>
    TEST_F(EGMAward, GetSASLevelId_Set)
    {
        // Data.
        const int sasLevelId1(0);
        const int sasLevelId2(31);
        const int sasLevelId3(32);
        const int sasLevelId4(255);
    
        // Setup.
        CEGMAward obj(MEMORY_NONE, false, 0);
    
        // Perform operation(s) and test(s).
        obj.SetSASLevelId(sasLevelId1);
        ASSERT_TRUE(sasLevelId1 == obj.GetSASLevelId());
    
        obj.SetSASLevelId(sasLevelId2);
        ASSERT_TRUE(sasLevelId2 == obj.GetSASLevelId());
    
        obj.SetSASLevelId(sasLevelId3);
        ASSERT_TRUE(sasLevelId3 == obj.GetSASLevelId());
    
        obj.SetSASLevelId(sasLevelId4);
        ASSERT_TRUE(sasLevelId4 == obj.GetSASLevelId());
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(const CEGMAward &)
    ///     CEGMAward::GetSASLevelId()
    /// Test copy constructor.
    /// </summary>
    TEST_F(EGMAward, GetSASLevelId_Copy)
    {
        // Data.
        const int sasLevelIdExpected(11);
    
        // Setup.
        CEGMAward objA(MEMORY_NONE, false, 0);
        objA.SetSASLevelId(sasLevelIdExpected);
    
        // Perform operation(s) and test(s).
        CEGMAward obj(objA);
        ASSERT_TRUE(sasLevelIdExpected == obj.GetSASLevelId());
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::operator=()
    ///     CEGMAward::GetSASLevelId()
    /// Test assignment operator.
    /// </summary>
    TEST_F(EGMAward, GetSASLevelId_Assignment)
    {
        // Data.
        const int sasLevelId(11);
        const int sasLevelIdExpected(22);
    
        // Setup.
        CEGMAward objA(MEMORY_NONE, false, 0);
        objA.SetSASLevelId(sasLevelIdExpected);
    
        CEGMAward obj(MEMORY_NONE, false, 0);
        obj.SetSASLevelId(sasLevelId);
    
        // Perform operation(s) and test(s).
        obj = objA;
        ASSERT_TRUE(sasLevelIdExpected == obj.GetSASLevelId());
    }
    // #pragma endregion GetSASLevelId
    
    // #pragma region isHitReceived
    /// <summary>
    /// Functional test for:
    /// CEGMAward::CEGMAward()
    /// CEGMAward::isHitReceived()
    /// Test for default value(s) using the default constructor.
    /// </summary>
    TEST_F(EGMAward, isHitReceived_Default)
    {
        // Data.
        const bool isHitReceivedExpected(false);
    
        // Perform operation(s) and test(s).
        CEGMAward obj;
        ASSERT_TRUE(isHitReceivedExpected == obj.isHitReceived());
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(const SYSTEMTIME&)
    ///     CEGMAward::isHitReceived()
    /// Test for default value(s) using the JSON constructor.
    /// </summary>
    TEST_F(EGMAward, isHitReceived_DefaultJson)
    {
        // Data.
        const bool isHitReceivedExpected(false);
    
        const SYSTEMTIME receivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
        static const char *json =
            "{"
            "\"LevelId\":1,"
            "\"JackpotId\":2,"
            "\"RemoteJackpotId\":3,"
            "\"Amount\":4.44,"
            "\"JackpotType\":\"StandardProgressive\","
            "\"MethodOfPayment\":\"PayToGame\","
            "\"SessionId\":5"
            "}";
        rapidjson::Document jsonDoc;
        ASSERT_FALSE(jsonDoc.Parse(json).HasParseError());
    
        // Perform operation(s) and test(s).
        CEGMAward obj(receivedTime, jsonDoc);
        ASSERT_TRUE(isHitReceivedExpected == obj.isHitReceived());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(MEMORY_TYPE, ...)
    ///     CEGMAward::isHitReceived()
    /// Test for default value(s) using the MEMORY_TYPE constructor.
    /// </summary>
    TEST_F(EGMAward, isHitReceived_DefaultMemory)
    {
        // Data.
        const bool isHitReceivedExpected(false);

        // Perform operation(s) and test(s).
        CEGMAward obj(MEMORY_NONE, false, 0);
        ASSERT_TRUE(isHitReceivedExpected == obj.isHitReceived());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetHitReceived()
    ///     CEGMAward::isHitReceived()
    /// Test SetHitReceived() with various value(s).
    /// </summary>
    TEST_F(EGMAward, isHitReceived_Set)
    {
        // Data.
        const bool isHitReceived1(true);
        const bool isHitReceived2(false);
        const bool isHitReceived3(true);

        // Setup.
        CEGMAward obj(MEMORY_NONE, false, 0);

        // Perform operation(s) and test(s).
        obj.SetHitReceived(isHitReceived1);
        ASSERT_TRUE(isHitReceived1 == obj.isHitReceived());

        obj.SetHitReceived(isHitReceived2);
        ASSERT_TRUE(isHitReceived2 == obj.isHitReceived());

        obj.SetHitReceived(isHitReceived3);
        ASSERT_TRUE(isHitReceived3 == obj.isHitReceived());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(const CEGMAward &)
    ///     CEGMAward::isHitReceived()
    /// Test copy constructor.
    /// </summary>
    TEST_F(EGMAward, isHitReceived_CopyTrue)
    {
        // Data.
        const bool isHitReceivedExpected(true);

        // Setup.
        CEGMAward objA(MEMORY_NONE, false, 0);
        objA.SetHitReceived(isHitReceivedExpected);

        // Perform operation(s) and test(s).
        CEGMAward obj(objA);
        ASSERT_TRUE(isHitReceivedExpected == obj.isHitReceived());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(const CEGMAward &)
    ///     CEGMAward::isHitReceived()
    /// Test copy constructor.
    /// </summary>
    TEST_F(EGMAward, isHitReceived_CopyFalse)
    {
        // Data.
        const bool isHitReceivedExpected(false);

        // Setup.
        CEGMAward objA(MEMORY_NONE, false, 0);
        objA.SetHitReceived(isHitReceivedExpected);

        // Perform operation(s) and test(s).
        CEGMAward obj(objA);
        ASSERT_TRUE(isHitReceivedExpected == obj.isHitReceived());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::operator=()
    ///     CEGMAward::isHitReceived()
    /// Test assignment operator.
    /// </summary>
    TEST_F(EGMAward, isHitReceived_AssignmentTrue)
    {
        // Data.
        const bool isHitReceivedExpected(true);

        // Setup.
        CEGMAward objA(MEMORY_NONE, false, 0);
        objA.SetHitReceived(isHitReceivedExpected);

        CEGMAward obj(MEMORY_NONE, false, 0);
        obj.SetHitReceived(!isHitReceivedExpected);

        // Perform operation(s) and test(s).
        obj = objA;
        ASSERT_TRUE(isHitReceivedExpected == obj.isHitReceived());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::operator=()
    ///     CEGMAward::isHitReceived()
    /// Test assignment operator.
    /// </summary>
    TEST_F(EGMAward, isHitReceived_AssignmentFalse)
    {
        // Data.
        const bool isHitReceivedExpected(false);

        // Setup.
        CEGMAward objA(MEMORY_NONE, false, 0);
        objA.SetHitReceived(isHitReceivedExpected);

        CEGMAward obj(MEMORY_NONE, false, 0);
        obj.SetHitReceived(!isHitReceivedExpected);

        // Perform operation(s) and test(s).
        obj = objA;
        ASSERT_TRUE(isHitReceivedExpected == obj.isHitReceived());
    }
    // #pragma endregion isHitReceived

    // #pragma region isHandpayPendingSignaled
    /// <summary>
    /// Functional test for:
    /// CEGMAward::CEGMAward()
    /// CEGMAward::isHandpayPendingSignaled()
    /// Test for default value(s) using the default constructor.
    /// </summary>
    TEST_F(EGMAward, isHandpayPendingSignaled_Default)
    {
        // Data.
        const bool isHandpayPendingSignaledExpected(false);

        // Perform operation(s) and test(s).
        CEGMAward obj;
        ASSERT_TRUE(isHandpayPendingSignaledExpected == obj.isHandpayPendingSignaled());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(const SYSTEMTIME&)
    ///     CEGMAward::isHandpayPendingSignaled()
    /// Test for default value(s) using the JSON constructor.
    /// </summary>
    TEST_F(EGMAward, isHandpayPendingSignaled_DefaultJson)
    {
        // Data.
        const bool isHandpayPendingSignaledExpected(false);

        const SYSTEMTIME receivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
        static const char *json =
            "{"
            "\"LevelId\":1,"
            "\"JackpotId\":2,"
            "\"RemoteJackpotId\":3,"
            "\"Amount\":4.44,"
            "\"JackpotType\":\"StandardProgressive\","
            "\"MethodOfPayment\":\"PayToGame\","
            "\"SessionId\":5"
            "}";
        rapidjson::Document jsonDoc;
        bool result = jsonDoc.Parse(json).HasParseError();
        ASSERT_FALSE(result);

        // Perform operation(s) and test(s).
        CEGMAward obj(receivedTime, jsonDoc);
        ASSERT_TRUE(isHandpayPendingSignaledExpected == obj.isHandpayPendingSignaled());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(MEMORY_TYPE, ...)
    ///     CEGMAward::isHandpayPendingSignaled()
    /// Test for default value(s) using the MEMORY_TYPE constructor.
    /// </summary>
    TEST_F(EGMAward, isHandpayPendingSignaled_DefaultMemory)
    {
        // Data.
        const bool isHandpayPendingSignaledExpected(false);

        // Perform operation(s) and test(s).
        CEGMAward obj(MEMORY_NONE, false, 0);
        ASSERT_TRUE(isHandpayPendingSignaledExpected == obj.isHandpayPendingSignaled());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::setHandpayPendingSignaled()
    ///     CEGMAward::isHandpayPendingSignaled()
    /// Test setHandpayPendingSignaled() with various value(s).
    /// </summary>
    TEST_F(EGMAward, isHandpayPendingSignaled_Set)
    {
        // Data.
        const bool isHandpayPendingSignaled1(true);
        const bool isHandpayPendingSignaled2(false);
        const bool isHandpayPendingSignaled3(true);

        // Setup.
        CEGMAward obj(MEMORY_NONE, false, 0);

        // Perform operation(s) and test(s).
        obj.setHandpayPendingSignaled(isHandpayPendingSignaled1);
        ASSERT_TRUE(isHandpayPendingSignaled1 == obj.isHandpayPendingSignaled());

        obj.setHandpayPendingSignaled(isHandpayPendingSignaled2);
        ASSERT_TRUE(isHandpayPendingSignaled2 == obj.isHandpayPendingSignaled());

        obj.setHandpayPendingSignaled(isHandpayPendingSignaled3);
        ASSERT_TRUE(isHandpayPendingSignaled3 == obj.isHandpayPendingSignaled());
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(const CEGMAward &)
    ///     CEGMAward::isHandpayPendingSignaled()
    /// Test copy constructor.
    /// </summary>
    TEST_F(EGMAward, isHandpayPendingSignaled_CopyTrue)
    {
        // Data.
        const bool isHandpayPendingSignaledExpected(true);

        // Setup.
        CEGMAward objA(MEMORY_NONE, false, 0);
        objA.setHandpayPendingSignaled(isHandpayPendingSignaledExpected);

        // Perform operation(s) and test(s).
        CEGMAward obj(objA);
        ASSERT_TRUE(isHandpayPendingSignaledExpected == obj.isHandpayPendingSignaled());
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(const CEGMAward &)
    ///     CEGMAward::isHandpayPendingSignaled()
    /// Test copy constructor.
    /// </summary>
    TEST_F(EGMAward, isHandpayPendingSignaled_CopyFalse)
    {
        // Data.
        const bool isHandpayPendingSignaledExpected(false);
    
        // Setup.
        CEGMAward objA(MEMORY_NONE, false, 0);
        objA.setHandpayPendingSignaled(isHandpayPendingSignaledExpected);
    
        // Perform operation(s) and test(s).
        CEGMAward obj(objA);
        ASSERT_TRUE(isHandpayPendingSignaledExpected == obj.isHandpayPendingSignaled());
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::operator=()
    ///     CEGMAward::isHandpayPendingSignaled()
    /// Test assignment operator.
    /// </summary>
    TEST_F(EGMAward, isHandpayPendingSignaled_AssignmentTrue)
    {
        // Data.
        const bool isHandpayPendingSignaledExpected(true);
    
        // Setup.
        CEGMAward objA(MEMORY_NONE, false, 0);
        objA.setHandpayPendingSignaled(isHandpayPendingSignaledExpected);
    
        CEGMAward obj(MEMORY_NONE, false, 0);
        obj.setHandpayPendingSignaled(!isHandpayPendingSignaledExpected);
    
        // Perform operation(s) and test(s).
        obj = objA;
        ASSERT_TRUE(isHandpayPendingSignaledExpected == obj.isHandpayPendingSignaled());
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::operator=()
    ///     CEGMAward::isHandpayPendingSignaled()
    /// Test assignment operator.
    /// </summary>
    TEST_F(EGMAward, isHandpayPendingSignaled_AssignmentFalse)
    {
        // Data.
        const bool isHandpayPendingSignaledExpected(false);
    
        // Setup.
        CEGMAward objA(MEMORY_NONE, false, 0);
        objA.setHandpayPendingSignaled(isHandpayPendingSignaledExpected);
    
        CEGMAward obj(MEMORY_NONE, false, 0);
        obj.setHandpayPendingSignaled(!isHandpayPendingSignaledExpected);
    
        // Perform operation(s) and test(s).
        obj = objA;
        ASSERT_TRUE(isHandpayPendingSignaledExpected == obj.isHandpayPendingSignaled());
    }
    
    /// <summary>
    /// Functional test for:
    /// CEGMAward::CEGMAward()
    /// CEGMAward::isEgmPaidSignaled()
    /// Test for default value(s) using the default constructor.
    /// </summary>
    TEST_F(EGMAward, isEgmPaidSignaled_Default)
    {
        // Data.
        const bool isEgmPaidSignaledExpected(false);
    
        // Perform operation(s) and test(s).
        CEGMAward obj;
        ASSERT_TRUE(isEgmPaidSignaledExpected == obj.isEgmPaidSignaled());
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(const SYSTEMTIME&)
    ///     CEGMAward::isEgmPaidSignaled()
    /// Test for default value(s) using the JSON constructor.
    /// </summary>
    TEST_F(EGMAward, isEgmPaidSignaled_DefaultJson)
    {
        // Data.
        const bool isEgmPaidSignaledExpected(false);
    
        const SYSTEMTIME receivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
        static const char *json =
            "{"
            "\"LevelId\":1,"
            "\"JackpotId\":2,"
            "\"RemoteJackpotId\":3,"
            "\"Amount\":4.44,"
            "\"JackpotType\":\"StandardProgressive\","
            "\"MethodOfPayment\":\"PayToGame\","
            "\"SessionId\":5"
            "}";
        rapidjson::Document jsonDoc;
        ASSERT_FALSE(jsonDoc.Parse(json).HasParseError());
    
        // Perform operation(s) and test(s).
        CEGMAward obj(receivedTime, jsonDoc);
        ASSERT_TRUE(isEgmPaidSignaledExpected == obj.isEgmPaidSignaled());
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(MEMORY_TYPE, ...)
    ///     CEGMAward::isEgmPaidSignaled()
    /// Test for default value(s) using the MEMORY_TYPE constructor.
    /// </summary>
    TEST_F(EGMAward, isEgmPaidSignaled_DefaultMemory)
    {
        // Data.
        const bool isEgmPaidSignaledExpected(false);
    
        // Perform operation(s) and test(s).
        CEGMAward obj(MEMORY_NONE, false, 0);
        ASSERT_TRUE(isEgmPaidSignaledExpected == obj.isEgmPaidSignaled());
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::setEgmPaidSignaled()
    ///     CEGMAward::isEgmPaidSignaled()
    /// Test setEgmPaidSignaled() with various value(s).
    /// </summary>
    TEST_F(EGMAward, isEgmPaidSignaled_Set)
    {
        // Data.
        const bool isEgmPaidSignaled1(true);
        const bool isEgmPaidSignaled2(false);
        const bool isEgmPaidSignaled3(true);
    
        // Setup.
        CEGMAward obj(MEMORY_NONE, false, 0);
    
        // Perform operation(s) and test(s).
        obj.setEgmPaidSignaled(isEgmPaidSignaled1);
        ASSERT_TRUE(isEgmPaidSignaled1 == obj.isEgmPaidSignaled());
    
        obj.setEgmPaidSignaled(isEgmPaidSignaled2);
        ASSERT_TRUE(isEgmPaidSignaled2 == obj.isEgmPaidSignaled());
    
        obj.setEgmPaidSignaled(isEgmPaidSignaled3);
        ASSERT_TRUE(isEgmPaidSignaled3 == obj.isEgmPaidSignaled());
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(const CEGMAward &)
    ///     CEGMAward::isEgmPaidSignaled()
    /// Test copy constructor.
    /// </summary>
    TEST_F(EGMAward, isEgmPaidSignaled_CopyTrue)
    {
        // Data.
        const bool isEgmPaidSignaledExpected(true);
    
        // Setup.
        CEGMAward objA(MEMORY_NONE, false, 0);
        objA.setEgmPaidSignaled(isEgmPaidSignaledExpected);
    
        // Perform operation(s) and test(s).
        CEGMAward obj(objA);
        ASSERT_TRUE(isEgmPaidSignaledExpected == obj.isEgmPaidSignaled());
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(const CEGMAward &)
    ///     CEGMAward::isEgmPaidSignaled()
    /// Test copy constructor.
    /// </summary>
    TEST_F(EGMAward, isEgmPaidSignaled_CopyFalse)
    {
        // Data.
        const bool isEgmPaidSignaledExpected(false);
    
        // Setup.
        CEGMAward objA(MEMORY_NONE, false, 0);
        objA.setEgmPaidSignaled(isEgmPaidSignaledExpected);
    
        // Perform operation(s) and test(s).
        CEGMAward obj(objA);
        ASSERT_TRUE(isEgmPaidSignaledExpected == obj.isEgmPaidSignaled());
    }

	/// <summary>
	/// Functional test for:
	///     CEGMAward::operator=()
	///     CEGMAward::isEgmPaidSignaled()
	/// Test assignment operator.
	/// </summary>
	TEST_F(EGMAward, isEgmPaidSignaled_AssignmentTrue)
	{
		// Data.
		const bool isEgmPaidSignaledExpected(true);

		// Setup.
		CEGMAward objA(MEMORY_NONE, false, 0);
		objA.setEgmPaidSignaled(isEgmPaidSignaledExpected);

		CEGMAward obj(MEMORY_NONE, false, 0);
		obj.setEgmPaidSignaled(!isEgmPaidSignaledExpected);

		// Perform operation(s) and test(s).
		obj = objA;
		ASSERT_TRUE(isEgmPaidSignaledExpected == obj.isEgmPaidSignaled());
	}

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::operator=()
    ///     CEGMAward::isEgmPaidSignaled()
    /// Test assignment operator.
    /// </summary>
    TEST_F(EGMAward, isEgmPaidSignaled_AssignmentFalse)
    {
        // Data.
        const bool isEgmPaidSignaledExpected(false);
    
        // Setup.
        CEGMAward objA(MEMORY_NONE, false, 0);
        objA.setEgmPaidSignaled(isEgmPaidSignaledExpected);
    
        CEGMAward obj(MEMORY_NONE, false, 0);
        obj.setEgmPaidSignaled(!isEgmPaidSignaledExpected);
    
        // Perform operation(s) and test(s).
        obj = objA;
        ASSERT_TRUE(isEgmPaidSignaledExpected == obj.isEgmPaidSignaled());
    }

    /// <summary>
    /// Functional test for:
    /// CEGMAward::CEGMAward()
    /// CEGMAward::GetReceivedFromEGMTime()
    /// Test for default value(s) using the default constructor.
    /// </summary>
    TEST_F(EGMAward, GetReceivedFromEGMTime_Default)
    {
        // Data.
        const bool isEgmPaidSignaledExpected(false);
    
        // Perform operation(s) and test(s).
        CEGMAward obj;
        ASSERT_TRUE(0 == memcmp(&m_receivedFromEGMTimeExpected, &obj.GetReceivedFromEGMTime(), sizeof(m_receivedFromEGMTimeExpected)));
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(const SYSTEMTIME&)
    ///     CEGMAward::GetReceivedFromEGMTime()
    /// Test for default value(s) using the JSON constructor.
    /// </summary>
    TEST_F(EGMAward, GetReceivedFromEGMTime_DefaultJson)
    {
        // Data.
        const bool isEgmPaidSignaledExpected(false);
    
        const SYSTEMTIME receivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
        static const char *json =
            "{"
            "\"LevelId\":1,"
            "\"JackpotId\":2,"
            "\"RemoteJackpotId\":3,"
            "\"Amount\":4.44,"
            "\"JackpotType\":\"StandardProgressive\","
            "\"MethodOfPayment\":\"PayToGame\","
            "\"SessionId\":5"
            "}";
        rapidjson::Document jsonDoc;
        ASSERT_FALSE(jsonDoc.Parse(json).HasParseError());
    
        // Perform operation(s) and test(s).
        CEGMAward obj(receivedTime, jsonDoc);
        ASSERT_TRUE(0 == memcmp(&m_receivedFromEGMTimeExpected, &obj.GetReceivedFromEGMTime(), sizeof(m_receivedFromEGMTimeExpected)));
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(MEMORY_TYPE, ...)
    ///     CEGMAward::GetReceivedFromEGMTime()
    /// Test for default value(s) using the MEMORY_TYPE constructor.
    /// </summary>
    TEST_F(EGMAward, GetReceivedFromEGMTime_DefaultMemory)
    {
        // Data.
        const bool isEgmPaidSignaledExpected(false);
    
        // Perform operation(s) and test(s).
        CEGMAward obj(MEMORY_NONE, false, 0);
        ASSERT_TRUE(0 == memcmp(&m_receivedFromEGMTimeExpected, &obj.GetReceivedFromEGMTime(), sizeof(m_receivedFromEGMTimeExpected)));
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::SetReceivedFromEGMTime()
    ///     CEGMAward::GetReceivedFromEGMTime()
    /// Test setEgmPaidSignaled() with various value(s).
    /// </summary>
    TEST_F(EGMAward, GetReceivedFromEGMTime_Set)
    {
        // Data.
        const SYSTEMTIME receivedFromEGMTime1 = { 2011, 1, 0, 11, 12, 34, 56, 789 };
        const SYSTEMTIME receivedFromEGMTime2 = { 2012, 2, 0, 12, 23, 45, 67, 890 };
        const SYSTEMTIME receivedFromEGMTime3 = { 2013, 2, 0, 23, 34, 56, 78, 901 };
    
    
        // Setup.
        CEGMAward obj(MEMORY_NONE, false, 0);
    
        // Perform operation(s) and test(s).
        obj.SetReceivedFromEGMTime(receivedFromEGMTime1);
        ASSERT_TRUE(0 == memcmp(&receivedFromEGMTime1, &obj.GetReceivedFromEGMTime(), sizeof(receivedFromEGMTime1)));
    
        obj.SetReceivedFromEGMTime(receivedFromEGMTime2);
        ASSERT_TRUE(0 == memcmp(&receivedFromEGMTime2, &obj.GetReceivedFromEGMTime(), sizeof(receivedFromEGMTime2)));
    
        obj.SetReceivedFromEGMTime(receivedFromEGMTime3);
        ASSERT_TRUE(0 == memcmp(&receivedFromEGMTime3, &obj.GetReceivedFromEGMTime(), sizeof(receivedFromEGMTime3)));
    }

      
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward(const CEGMAward &)
    ///     CEGMAward::GetReceivedFromEGMTime()
    /// Test copy constructor.
    /// </summary>
    TEST_F(EGMAward, GetReceivedFromEGMTime_Copy)
    {
        // Data.
        const SYSTEMTIME receivedFromEGMTime = { 2013, 1, 0, 11, 12, 34, 56, 789 };
    
        // Setup.
        CEGMAward objA(MEMORY_NONE, false, 0);
        objA.SetReceivedFromEGMTime(receivedFromEGMTime);
    
        // Perform operation(s) and test(s).
        CEGMAward obj(objA);
        ASSERT_TRUE(0 == memcmp(&receivedFromEGMTime, &obj.GetReceivedFromEGMTime(), sizeof(receivedFromEGMTime)));
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward::operator=()
    ///     CEGMAward::GetReceivedFromEGMTime()
    /// Test assignment operator.
    /// </summary>
    TEST_F(EGMAward, GetReceivedFromEGMTime_Assignment)
    {
        // Data.
        const SYSTEMTIME receivedFromEGMTime = { 2013, 1, 0, 11, 12, 34, 56, 789 };
    
        // Setup.
        CEGMAward objA(MEMORY_NONE, false, 0);
        objA.SetReceivedFromEGMTime(receivedFromEGMTime);
    
        CEGMAward obj(MEMORY_NONE, false, 0);
        ASSERT_TRUE(0 == memcmp(&m_receivedFromEGMTimeExpected, &obj.GetReceivedFromEGMTime(), sizeof(m_receivedFromEGMTimeExpected)));
    
        // Perform operation(s) and test(s).
        obj = objA;
        ASSERT_TRUE(0 == memcmp(&receivedFromEGMTime, &obj.GetReceivedFromEGMTime(), sizeof(receivedFromEGMTime)));
    }

    /// <summary>
    /// Functional test for:
    ///     CEGMAward
    /// Test persisted static const values for forwards and backwards compatibility.
    /// </summary>
    TEST_F(EGMAward, GetMethodOfPayment_StaticConst)
    {
        // Perform test(s) for forwards and backwards compatibility.
        ASSERT_TRUE((int)-1 == (int)MethodOfPayment::Invalid);
        ASSERT_TRUE((int)0 == (int)MethodOfPayment::PayToGame);
        // PayToGameWithGameLock is obsolete with One Link version 5.5 and greater.
        ASSERT_TRUE((int)2 == (int)MethodOfPayment::PayToGameWithHandPay);
        ASSERT_TRUE((int)3 == (int)MethodOfPayment::PayToPlayerAccount);
        ASSERT_TRUE((int)4 == (int)MethodOfPayment::TriggerOnly);
        ASSERT_TRUE((int)5 == (int)MethodOfPayment::PayToPromoGameDevice);
        ASSERT_TRUE((int)6 == (int)MethodOfPayment::Claimable);
    
        ASSERT_TRUE(MethodOfPayment::Invalid == CEGMAward::DEFAULT_METHOD_OF_PAYMENT);
        ASSERT_TRUE(MethodOfPayment::PayToGame == CEGMAward::UPGRADE_METHOD_OF_PAYMENT);
    }
    
    /// <summary>
    /// Functional test for:
    ///     CEGMAward::CEGMAward()
    ///     CEGMAward::GetMethodOfPaymentForBonus()
    /// Test for default value(s) using the default constructor.
    /// </summary>
    TEST_F(EGMAward, GetMethodOfPaymentForBonus_Default)
    {
        // Data.
        MethodOfPayment::Type methodOfPaymentExpected(CEGMAward::DEFAULT_METHOD_OF_PAYMENT);
    
        // Perform operation(s) and test(s).
        CEGMAward obj;
        ASSERT_TRUE(methodOfPaymentExpected == obj.GetMethodOfPayment());
    }

#if 000//TODO-PORT    
            void GetJsonDocForJackpotHit_VaryMethodOfPayment(rapidjson::Document &jsonDoc, LPCSTR pMethodOfPayment)
            {
                // Setup.
                CStringA json;
                json.Format(
                    "{"
                    "\"JackpotId\":909,"
                    "\"RemoteJackpotId\":0,"
                    "\"WinningDeviceId\":6467,"
                    "\"LevelId\":23886,"
                    "\"LevelName\":\"JacksonTest\","
                    "\"OverageAdjustment\":0,"
                    "\"Amount\":3,"
                    "\"OriginalAmount\":0,"
                    "\"JackpotType\":\"Bonus\","
                    "\"JackpotTriggerType\":\"CoinInTrigger\","
                    "\"PriorityAward\":false,"
                    "\"WinningAssetNumber\":\"113087\","
                    "\"Remote\":false,"
                    "\"MethodOfPayment\":\"%s\"," // Set MethodOfPayment to pMethodOfPayment.
                    "\"AwardType\":\"CashableCredits\","
                    "\"SessionId\":123123,"
                    "\"SourceDeviceId\":1,"
                    "\"SourceDeviceConfigVersion\":0,"
                    "\"SourceAsset\":\"\","
                    "\"Timestamp\":1434039783129,"
                    "\"MessageId\":\"909-1\""
                    "}",
                    static_cast<LPCSTR>(pMethodOfPayment));
    
                GetJsonDocFromString(jsonDoc, json);
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::CEGMAward()
            ///     CEGMAward::GetMethodOfPayment()
            /// Test for non-default value(s) using the json constructor.
            /// </summary>
            TEST_F(EGMAward, GetMethodOfPayment_JsonData)
            {
                // Data.
                const SYSTEMTIME receivedTime = { 2013, 1, 0, 11, 12, 34, 56, 789 };
    
                // Setup.
                rapidjson::Document jsonDoc;
    
                // Perform operation(s) and test(s).
                {
                    GetJsonDocForJackpotHit_VaryMethodOfPayment(jsonDoc, MethodOfPayment::pPayToGame);
                    CEGMAward obj(receivedTime, jsonDoc);
                    ASSERT_TRUE(MethodOfPayment::PayToGame, obj.GetMethodOfPayment(), L"PayToGame");
                }
    
    {
        GetJsonDocForJackpotHit_VaryMethodOfPayment(jsonDoc, MethodOfPayment::pPayToGameWithHandPay);
        CEGMAward obj(receivedTime, jsonDoc);
        ASSERT_TRUE(MethodOfPayment::PayToGameWithHandPay, obj.GetMethodOfPayment(), L"PayToGameWithHandPay");
    }
    
    {
        GetJsonDocForJackpotHit_VaryMethodOfPayment(jsonDoc, MethodOfPayment::pPayToPlayerAccount);
        CEGMAward obj(receivedTime, jsonDoc);
        ASSERT_TRUE(MethodOfPayment::PayToPlayerAccount, obj.GetMethodOfPayment(), L"PayToPlayerAccount");
    }
    
    {
        GetJsonDocForJackpotHit_VaryMethodOfPayment(jsonDoc, MethodOfPayment::pTriggerOnly);
        CEGMAward obj(receivedTime, jsonDoc);
        ASSERT_TRUE(MethodOfPayment::TriggerOnly, obj.GetMethodOfPayment(), L"TriggerOnly");
    }
    
    {
        GetJsonDocForJackpotHit_VaryMethodOfPayment(jsonDoc, MethodOfPayment::pPayToPromoGameDevice);
        CEGMAward obj(receivedTime, jsonDoc);
        ASSERT_TRUE(MethodOfPayment::PayToPromoGameDevice, obj.GetMethodOfPayment(), L"PayToPromoGameDevice");
    }
    
    {
        GetJsonDocForJackpotHit_VaryMethodOfPayment(jsonDoc, MethodOfPayment::pClaimable);
        CEGMAward obj(receivedTime, jsonDoc);
        ASSERT_TRUE(MethodOfPayment::Claimable, obj.GetMethodOfPayment(), L"Claimable");
    }
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::CEGMAward()
            ///     CEGMAward::GetMethodOfPayment()
            /// Test for invalid value(s) using the json constructor.
            /// </summary>
            TEST_F(EGMAward, GetMethodOfPayment_JsonInvalid)
            {
                // Data.
                const SYSTEMTIME receivedTime = { 2013, 1, 0, 11, 12, 34, 56, 789 };
    
                // Setup.
                rapidjson::Document jsonDoc;
    
                // Perform operation(s) and test(s).
                {
                    GetJsonDocForJackpotHit_VaryMethodOfPayment(jsonDoc, "xxx"); // Invalid MethodOfPayment value.
                    CEGMAward obj(receivedTime, jsonDoc);
                    ASSERT_TRUE(MethodOfPayment::Invalid, obj.GetMethodOfPayment(), L"Claimable");
                }
    
    {
        GetJsonDocForJackpotHit_VaryMethodOfPayment(jsonDoc, ""); // Empty MethodOfPayment value.
        CEGMAward obj(receivedTime, jsonDoc);
        ASSERT_TRUE(MethodOfPayment::Invalid, obj.GetMethodOfPayment(), L"Claimable");
    }
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::CEGMAward()
            ///     CEGMAward::GetMethodOfPayment()
            /// Test for omitted parameter(s) using the json constructor.
            /// </summary>
            TEST_F(EGMAward, GetMethodOfPayment_JsonOmitted)
            {
                // Data.
                const SYSTEMTIME receivedTime = { 2013, 1, 0, 11, 12, 34, 56, 789 };
                LPCSTR json(
                    "{"
                    "\"JackpotId\":909,"
                    "\"RemoteJackpotId\":0,"
                    "\"WinningDeviceId\":6467,"
                    "\"LevelId\":23886,"
                    "\"LevelName\":\"JacksonTest\","
                    "\"OverageAdjustment\":0,"
                    "\"Amount\":3,"
                    "\"OriginalAmount\":0,"
                    "\"JackpotType\":\"Bonus\","
                    "\"JackpotTriggerType\":\"CoinInTrigger\","
                    "\"PriorityAward\":false,"
                    "\"WinningAssetNumber\":\"113087\","
                    "\"Remote\":false,"
                    // MethodOfPayment omitted.
                    "\"AwardType\":\"CashableCredits\","
                    "\"SessionId\":123123,"
                    "\"SourceDeviceId\":1,"
                    "\"SourceDeviceConfigVersion\":0,"
                    "\"SourceAsset\":\"\","
                    "\"Timestamp\":1434039783129,"
                    "\"MessageId\":\"909-1\""
                    "}");
    
                // Setup.
                rapidjson::Document jsonDoc;
                GetJsonDocFromString(jsonDoc, json);
    
                // Perform operation(s) and test(s).
                CEGMAward obj(receivedTime, jsonDoc);
                ASSERT_TRUE(MethodOfPayment::Invalid, obj.GetMethodOfPayment(), L"Claimable");
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::CEGMAward(MEMORY_TYPE, ...)
            ///     CEGMAward::GetMethodOfPayment()
            /// Test for default value(s) using the MEMORY_TYPE constructor.
            /// </summary>
            TEST_F(EGMAward, GetMethodOfPayment_DefaultMemory)
            {
                // Data.
                MethodOfPayment::Type methodOfPaymentExpected(CEGMAward::DEFAULT_METHOD_OF_PAYMENT);
    
                // Perform operation(s) and test(s).
                CEGMAward obj(MEMORY_NONE, false, 0);
                ASSERT_TRUE(methodOfPaymentExpected, obj.GetMethodOfPayment(), L"GetMethodOfPayment");
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::CEGMAward()
            ///     CEGMAward::GetMethodOfPayment()
            /// Test BuildYourself value.
            /// </summary>
            TEST_F(EGMAward, GetMethodOfPayment_BuildYourself_NvramInvalid)
            {
                // Data.
                MethodOfPayment::Type methodOfPaymentExpected(CEGMAward::DEFAULT_METHOD_OF_PAYMENT);
    
                // Setup.
                CUnitTestingUtils::SetNvramInvalid();
    
                // Perform operation(s) and test(s).
                const CEGMAward obj(MEMORY_NVRAM, true, NVRAM_EGM_AWARD_CHECKBYTE_OFFSET);
                ASSERT_TRUE(methodOfPaymentExpected, obj.GetMethodOfPayment(), L"GetMethodOfPayment");
            }
    
            // Use GetNvramBuffer_Set, GetNvramBuffer_Active, SetFromNvramBuffer0, SetFromNvramBuffer1, etc. for testing persisted values.
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::CEGMAward()
            ///     CEGMAward::GetMethodOfPayment()
            /// Test BuildYourself value.
            /// </summary>
            TEST_F(EGMAward, GetMethodOfPayment_BuildYourself_NvramValid)
            {
                // Data.
                const CString statusInfoOther("Other value");
    
                // Setup.
                CUnitTestingUtils::SetNvramValidAndUpgraded();
                {
                    CEGMAward varObj(MEMORY_NVRAM, true, NVRAM_EGM_AWARD_CHECKBYTE_OFFSET);
                    varObj.SetAwardState(AwardToGameError);
                    varObj.SetStatusInfo(statusInfoOther);
                    varObj.StoreYourself();
                }
    
                // Perform operation(s) and test(s).
                const CEGMAward obj(MEMORY_NVRAM, true, NVRAM_EGM_AWARD_CHECKBYTE_OFFSET);
                AssertResult2m(obj.GetStatusInfo(), m_statusInfoBuildYourself);
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetMethodOfPayment()
            ///     CEGMAward::GetMethodOfPayment()
            /// Test copy constructor.
            /// </summary>
            TEST_F(EGMAward, GetMethodOfPayment_Set)
            {
                // Setup.
                CEGMAward obj;
    
                // Perform operation(s) and test(s).
                obj.SetMethodOfPayment(MethodOfPayment::PayToGame);
                ASSERT_TRUE(MethodOfPayment::PayToGame, obj.GetMethodOfPayment(), L"PayToGame");
    
                obj.SetMethodOfPayment(MethodOfPayment::PayToGameWithHandPay);
                ASSERT_TRUE(MethodOfPayment::PayToGameWithHandPay, obj.GetMethodOfPayment(), L"PayToGameWithHandPay");
    
                obj.SetMethodOfPayment(MethodOfPayment::PayToPlayerAccount);
                ASSERT_TRUE(MethodOfPayment::PayToPlayerAccount, obj.GetMethodOfPayment(), L"PayToPlayerAccount");
    
                obj.SetMethodOfPayment(MethodOfPayment::TriggerOnly);
                ASSERT_TRUE(MethodOfPayment::TriggerOnly, obj.GetMethodOfPayment(), L"TriggerOnly");
    
                obj.SetMethodOfPayment(MethodOfPayment::PayToPromoGameDevice);
                ASSERT_TRUE(MethodOfPayment::PayToPromoGameDevice, obj.GetMethodOfPayment(), L"PayToPromoGameDevice");
    
                obj.SetMethodOfPayment(MethodOfPayment::Claimable);
                ASSERT_TRUE(MethodOfPayment::Claimable, obj.GetMethodOfPayment(), L"Claimable");
    
                obj.SetMethodOfPayment(MethodOfPayment::Invalid);
                ASSERT_TRUE(MethodOfPayment::Invalid, obj.GetMethodOfPayment(), L"Invalid");
            }
    
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::CEGMAward(const CEGMAward &)
            ///     CEGMAward::GetMethodOfPayment()
            /// Test copy constructor.
            /// </summary>
            TEST_F(EGMAward, GetMethodOfPayment_Copy)
            {
                // Data.
                const SYSTEMTIME receivedTime = { 2013, 1, 0, 11, 12, 34, 56, 789 };
    
                // Setup.
                rapidjson::Document jsonDoc;
                GetJsonDocForJackpotHit_VaryMethodOfPayment(jsonDoc, MethodOfPayment::pPayToGameWithHandPay);
                CEGMAward objA(receivedTime, jsonDoc);
    
                // Perform operation(s) and test(s).
                CEGMAward obj(objA);
                ASSERT_TRUE(MethodOfPayment::PayToGameWithHandPay, obj.GetMethodOfPayment(), L"PayToGameWithHandPay");
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::operator=()
            ///     CEGMAward::GetMethodOfPayment()
            /// Test assignment operator.
            /// </summary>
            TEST_F(EGMAward, GetMethodOfPayment_Assignment)
            {
                // Data.
                const SYSTEMTIME receivedTime = { 2013, 1, 0, 11, 12, 34, 56, 789 };
                MethodOfPayment::Type methodOfPaymentExpected(CEGMAward::DEFAULT_METHOD_OF_PAYMENT);
    
                // Setup.
                rapidjson::Document jsonDoc;
                GetJsonDocForJackpotHit_VaryMethodOfPayment(jsonDoc, MethodOfPayment::pPayToGameWithHandPay);
                CEGMAward objA(receivedTime, jsonDoc);
    
                CEGMAward obj(MEMORY_NONE, false, 0);
                ASSERT_TRUE(methodOfPaymentExpected, obj.GetMethodOfPayment(), L"Invalid"); // Verify setup.
    
                // Perform operation(s) and test(s).
                obj = objA;
                ASSERT_TRUE(MethodOfPayment::PayToGameWithHandPay, obj.GetMethodOfPayment(), L"PayToGameWithHandPay");
            }
            // #pragma endregion GetMethodOfPayment
    
            // #pragma region IsActive
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::CEGMAward()
            ///     CEGMAward::IsActive()
            /// Test for default value(s) using the default constructor.
            /// </summary>
            TEST_F(EGMAward, IsActive_Default)
            {
                // Data.
                const bool isActiveExpected(true);
    
                // Perform operation(s) and test(s).
                CEGMAward obj;
                ASSERT_TRUE(isActiveExpected, obj.IsActive(), L"IsActive");
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::ClearActive()
            ///     CEGMAward::IsActive()
            /// Test for default value(s) using the default constructor.
            /// </summary>
            TEST_F(EGMAward, IsActive_ClearActive)
            {
                // Data.
                const bool isActiveExpected(false);
    
                // Setup.
                CEGMAward obj;
                ASSERT_TRUE(!isActiveExpected, obj.IsActive(), L"IsActive Setup");
    
                // Perform operation(s) and test(s).
                obj.ClearActive();
                ASSERT_TRUE(isActiveExpected, obj.IsActive(), L"IsActive");
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::CEGMAward(&CEGMAward)
            ///     CEGMAward::IsActive()
            /// Test copy constructor with true value.
            /// </summary>
            TEST_F(EGMAward, IsActive_CopyTrue)
            {
                // Data.
                const bool active(true);
    
                // Setup.
                CEGMAward objA;
                ASSERT_TRUE(active, objA.IsActive(), L"objA.IsActive");
    
                // Perform operation(s) and test(s).
                const CEGMAward &objB(objA);
                ASSERT_TRUE(active, objB.IsActive(), L"objB.IsActive");
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::CEGMAward(&CEGMAward)
            ///     CEGMAward::IsActive()
            /// Test copy constructor with false value.
            /// </summary>
            TEST_F(EGMAward, IsActive_CopyFalse)
            {
                // Data.
                const bool active(false);
    
                // Setup.
                CEGMAward objA;
                objA.ClearActive();
                ASSERT_TRUE(active, objA.IsActive(), L"objA.IsActive");
    
                // Perform operation(s) and test(s).
                const CEGMAward &objB(objA);
                ASSERT_TRUE(active, objB.IsActive(), L"objB.IsActive");
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::operator=()
            ///     CEGMAward::IsActive()
            /// Test assigment operator with true value.
            /// </summary>
            TEST_F(EGMAward, IsActive_AssignmentTrue)
            {
                // Data.
                const bool active(true);
    
                // Setup.
                CEGMAward objA;
                CEGMAward objB;
                objB.ClearActive();
                ASSERT_TRUE(!active, objB.IsActive(), L"IsActive Setup");
                Assert::AreNotEqual(objA.IsActive(), objB.IsActive(), L"IsActive AreNotEqual");
    
                // Perform operation(s) and test(s).
                objB = objA;
                ASSERT_TRUE(active, objB.IsActive(), L"IsActive");
                ASSERT_TRUE(objA.IsActive(), objB.IsActive(), L"IsActive AreEqual");
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::operator=()
            ///     CEGMAward::IsActive()
            /// Test assigment operator with non-default value.
            /// </summary>
            TEST_F(EGMAward, IsActive_AssignmentFalse)
            {
                // Data.
                const bool active(false);
    
                // Setup.
                CEGMAward objA;
                objA.ClearActive();
                CEGMAward objB;
                ASSERT_TRUE(!active, objB.IsActive(), L"IsActive Setup");
                Assert::AreNotEqual(objA.IsActive(), objB.IsActive(), L"IsActive AreNotEqual");
    
                // Perform operation(s) and test(s).
                objB = objA;
                ASSERT_TRUE(active, objB.IsActive(), L"IsActive");
                ASSERT_TRUE(objA.IsActive(), objB.IsActive(), L"IsActive AreEqual");
            }
            // #pragma endregion IsActive
    
            // #pragma region SetMemoryStorage
            /// <summary>
            /// Test that the NVRAM memory map has not changed across versions.
            /// If the NVRAM memory map does change across versions then memory map upgrades should be tested.
            /// </summary>
            TEST_F(EGMAward, SetMemoryStorage_NvramMap)
            {
                // Perform test(s) for forwards and backwards compatibility.
                ASSERT_TRUE((DWORD)713739, NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET, L"NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET");
                ASSERT_TRUE((DWORD)32, NVRAM_EGM_AWARD_STANDARD_DATA_COUNT, L"NVRAM_EGM_AWARD_STANDARD_DATA_COUNT");
                ASSERT_TRUE((DWORD)257, NVRAM_EGM_AWARD_STANDARD_ITEM_SIZE, L"NVRAM_EGM_AWARD_STANDARD_ITEM_SIZE");
                ASSERT_TRUE((DWORD)0x2020, NVRAM_EGM_AWARD_STANDARD_SIZE, L"NVRAM_EGM_AWARD_STANDARD_SIZE");
    
                ASSERT_TRUE((DWORD)1, NVRAM_EGM_AWARD_CHECKBYTE_SIZE, L"NVRAM_EGM_AWARD_CHECKBYTE_SIZE");
                ASSERT_TRUE((BYTE)0x1c, NVRAM_EGM_AWARD_CHECKBYTE_VALUE, L"NVRAM_EGM_AWARD_CHECKBYTE_VALUE");
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::CEGMAward()
            /// Test for default value(s).
            /// </summary>
            TEST_F(EGMAward, SetMemoryStorage_Default)
            {
                // Data.
                const MEMORY_TYPE memoryTypeExpected(MEMORY_NONE);
                const DWORD nvramDataOffsetExpected(CMemoryStorage::NVRAM_DEFAULT_OFFSET);
                const DWORD nvramDataSizeExpected(CMemoryStorage::NVRAM_DEFAULT_SIZE);
                const DWORD nvramActiveOffsetExpected(CMemoryStorage::NVRAM_DEFAULT_OFFSET);
                const BYTE nvramActiveValueExpected(CMemoryStorageEx::NVRAM_DEFAULT_ACTIVE_VALUE);
    
                // Perform operation(s) and test(s).
                CEGMAward obj;
                ASSERT_TRUE(memoryTypeExpected, obj.m_type, L"obj.m_type");
                ASSERT_TRUE(nvramDataOffsetExpected, obj.m_dwOffset, L"obj.m_dwOffset");
                ASSERT_TRUE(nvramDataSizeExpected, obj.m_dwAllowedSize, L"obj.m_dwAllowedSize");
                ASSERT_TRUE(nvramActiveOffsetExpected, obj.m_activeOffset, L"obj.m_activeOffset");
                ASSERT_TRUE(nvramActiveValueExpected, obj.m_activeValue, L"obj.m_activeValue");
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::CEGMAward(MEMORY_TYPE ...)
            /// Test for non-default value(s) set using a constructor.
            /// </summary>
            TEST_F(EGMAward, SetMemoryStorage_CEGMAward)
            {
                // Data.
                const MEMORY_TYPE memoryType(MEMORY_NVRAM);
                const DWORD nvramActiveOffset(5);
    
                const MEMORY_TYPE memoryTypeExpected(memoryType);
                const DWORD nvramDataOffsetExpected(nvramActiveOffset + NVRAM_EGM_AWARD_CHECKBYTE_SIZE);
                const DWORD nvramDataSizeExpected(CEGMAward::NVRAM_DATA_SIZE);
                const DWORD nvramActiveOffsetExpected(nvramActiveOffset);
                const BYTE nvramActiveValueExpected(CEGMAward::NVRAM_ACTIVE_VALUE);
    
                // Perform operation(s) and test(s).
                CEGMAward obj(memoryType, false, nvramActiveOffset);
                ASSERT_TRUE(memoryTypeExpected, obj.m_type, L"obj.m_type");
                ASSERT_TRUE(nvramDataOffsetExpected, obj.m_dwOffset, L"obj.m_dwOffset");
                ASSERT_TRUE(nvramDataSizeExpected, obj.m_dwAllowedSize, L"obj.m_dwAllowedSize");
                ASSERT_TRUE(nvramActiveOffsetExpected, obj.m_activeOffset, L"obj.m_activeOffset");
                ASSERT_TRUE(nvramActiveValueExpected, obj.m_activeValue, L"obj.m_activeValue");
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetMemoryStorage()
            /// Test for non-default value(s).
            /// Does not test persist behavior.
            /// </summary>
            TEST_F(EGMAward, SetMemoryStorage_Set)
            {
                // Data.
                const MEMORY_TYPE memoryType(MEMORY_NVRAM);
                const DWORD nvramActiveOffset(5);
    
                const MEMORY_TYPE memoryTypeExpected(memoryType);
                const DWORD nvramDataOffsetExpected(nvramActiveOffset + NVRAM_EGM_AWARD_CHECKBYTE_SIZE);
                const DWORD nvramDataSizeExpected(CEGMAward::NVRAM_DATA_SIZE);
                const DWORD nvramActiveOffsetExpected(nvramActiveOffset);
                const BYTE nvramActiveValueExpected(CEGMAward::NVRAM_ACTIVE_VALUE);
    
                // Setup.
                CEGMAward obj;
    
                // Perform operation(s) and test(s).
                obj.SetMemoryStorage(nvramActiveOffset);
                ASSERT_TRUE(memoryTypeExpected, obj.m_type, L"obj.m_type");
                ASSERT_TRUE(nvramDataOffsetExpected, obj.m_dwOffset, L"obj.m_dwOffset");
                ASSERT_TRUE(nvramDataSizeExpected, obj.m_dwAllowedSize, L"obj.m_dwAllowedSize");
                ASSERT_TRUE(nvramActiveOffsetExpected, obj.m_activeOffset, L"obj.m_activeOffset");
                ASSERT_TRUE(nvramActiveValueExpected, obj.m_activeValue, L"obj.m_activeValue");
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetMemoryStorage()
            /// Test for default value(s) after calling SetMemoryStorage() with the default offset.
            /// </summary>
            TEST_F(EGMAward, SetMemoryStorage_Clear)
            {
                // Data.
                const MEMORY_TYPE memoryType(MEMORY_NVRAM);
                const DWORD nvramActiveOffset(5);
                const DWORD nvramDataOffset(nvramActiveOffset + NVRAM_EGM_AWARD_CHECKBYTE_SIZE);
                const DWORD nvramDataSize(CEGMAward::NVRAM_DATA_SIZE);
                const BYTE nvramActiveValue(CEGMAward::NVRAM_ACTIVE_VALUE);
    
                const MEMORY_TYPE memoryTypeExpected(MEMORY_NONE);
                const DWORD nvramDataOffsetExpected(CMemoryStorage::NVRAM_DEFAULT_OFFSET);
                const DWORD nvramDataSizeExpected(CMemoryStorage::NVRAM_DEFAULT_SIZE);
                const DWORD nvramActiveOffsetExpected(CMemoryStorage::NVRAM_DEFAULT_OFFSET);
                const BYTE nvramActiveValueExpected(CMemoryStorageEx::NVRAM_DEFAULT_ACTIVE_VALUE);
    
                // Setup.
                CEGMAward obj;
                obj.SetMemoryStorageParameters(memoryType, nvramDataOffset, nvramDataSize, nvramActiveOffset, nvramActiveValue);
    
                // Verify setup.
                ASSERT_TRUE(memoryType, obj.m_type, L"obj.m_type");
                ASSERT_TRUE(nvramDataOffset, obj.m_dwOffset, L"obj.m_dwOffset");
                ASSERT_TRUE(nvramDataSize, obj.m_dwAllowedSize, L"obj.m_dwAllowedSize");
                ASSERT_TRUE(nvramActiveOffset, obj.m_activeOffset, L"obj.m_activeOffset");
                ASSERT_TRUE(nvramActiveValue, obj.m_activeValue, L"obj.m_activeValue");
    
                // Perform operation(s) and test(s).
                obj.SetMemoryStorage(CMemoryStorage::NVRAM_DEFAULT_OFFSET);
                ASSERT_TRUE(memoryTypeExpected, obj.m_type, L"obj.m_type");
                ASSERT_TRUE(nvramDataOffsetExpected, obj.m_dwOffset, L"obj.m_dwOffset");
                ASSERT_TRUE(nvramDataSizeExpected, obj.m_dwAllowedSize, L"obj.m_dwAllowedSize");
                ASSERT_TRUE(nvramActiveOffsetExpected, obj.m_activeOffset, L"obj.m_activeOffset");
                ASSERT_TRUE(nvramActiveValueExpected, obj.m_activeValue, L"obj.m_activeValue");
            }
            // #pragma endregion SetMemoryStorage
    
            void SetEgmAwardNonDefault(CEGMAward &obj)
            {
                // Set the persistable values to unique non-default values (where practical).
                obj.SetAwardAmountPaid(0x0101010101010101);
                obj.SetAwardState((EGMAwardState)0x00000002);
                obj.SetSentToGameCount(0x03030303);
                obj.SetSentToGameTime(m_sentToGameTime);
                obj.SetOriginalMachineBonusMeterValue(0x0404040404040404);
                obj.SetOriginalAttendantBonusingMeterValue(0x0505050505050505);
    
                obj.SetLevelId(0x06060606);
                obj.SetJackpotId(0x0707070707070707);
                obj.SetRemoteJackpotId(0x0808080808080808);
                obj.SetAwardAmountHit(0x0909090909090909);
    
                obj.SetReceivedFromSystemTime(m_receivedFromSystemTime);
                obj.SetDuplicateHitCount(0x0a0a0a0a);
                obj.SetOtherHitCount(0x0b0b0b0b);
                obj.SetAwardTransferType((EGMAwardTransferType)0x00000001);
    
                // extraCheckByte.
                obj.SetAwardPaidType((EGMAwardPaidType)0x00000002);
    
                obj.SetStandardProgressive(true);
                // m_sessionId.isValid = false; // Not easy to set.                                                                                                                                                                                                                                                                                                                                                                                                               
                // m_sessionId.val = CPlayerSession::SESSION_ID_DEFAULT;
    
                obj.SetSASLevelId(0x0d0d0d0d);
                obj.SetHitReceived(true);
                obj.setHandpayPendingSignaled(false);
                obj.setEgmPaidSignaled(true);
                obj.SetReceivedFromEGMTime(m_receivedFromEGMTime);
                obj.SetMethodOfPayment(MethodOfPayment::TriggerOnly);
    
                // CheckByte3 for v1.51.0 extension(s).
                obj.SetOriginalLegacyBonusMeterValue(0x0f0f0f0f0f0f0f0f);
    
            }
    
            void AssertEgmAward(const CEGMAward &obj,
                bool recoveredBase,
                bool recovered150,
                bool upgrade150, // Only used when recovered150 = false.
                bool recovered151,
                bool active,
                bool sessionIdIsValidExpected, // Only used when recovered150 = true.
                int sessionIdExpected) // Only used when recovered150 = true.
            {
                if (recoveredBase)
                {
                    ASSERT_TRUE((__int64)0x0101010101010101, obj.GetAwardAmountPaid(), L"GetAwardAmountPaid");
                    ASSERT_TRUE((EGMAwardState)0x00000002, obj.GetAwardState(), L"GetAwardState");
                    ASSERT_TRUE((int)0x03030303, obj.GetSentToGameCount(), L"GetSentToGameCount");
                    ASSERT_TRUE(0, memcmp(&m_sentToGameTime, &obj.GetSentToGameTime(), sizeof(m_sentToGameTime)), L"GetSentToGameTime");
                    ASSERT_TRUE((__int64)0x0404040404040404,
                        obj.GetOriginalMachineBonusMeterValue(),
                        L"GetOriginalMachineBonusMeterValue");
                    ASSERT_TRUE((__int64)0x0505050505050505,
                        obj.GetOriginalAttendantBonusingMeterValue(),
                        L"GetOriginalAttendantBonusingMeterValue");
    
                    ASSERT_TRUE((int)0x06060606, obj.GetLevelId(), L"GetLevelId");
                    ASSERT_TRUE((__int64)0x0707070707070707, obj.GetJackpotId(), L"GetJackpotId");
                    ASSERT_TRUE((__int64)0x0808080808080808, obj.GetRemoteJackpotId(), L"GetRemoteJackpotId");
                    ASSERT_TRUE((__int64)0x0909090909090909, obj.GetAwardAmountHit(), L"GetAwardAmountHit");
    
                    ASSERT_TRUE(0,
                        memcmp(&m_receivedFromSystemTime, &obj.GetReceivedFromSystemTime(), sizeof(m_receivedFromSystemTime)),
                        L"GetReceivedFromSystemTime");
                    ASSERT_TRUE((DWORD)0x0a0a0a0a, obj.GetDuplicateHitCount(), L"GetDuplicateHitCount");
                    ASSERT_TRUE((DWORD)0x0b0b0b0b, obj.GetOtherHitCount(), L"GetOtherHitCount");
                    ASSERT_TRUE((EGMAwardTransferType)0x00000001, obj.GetAwardTransferType(), L"GetAwardTransferType");
                }
                else
                {
                    ASSERT_TRUE(m_awardAmountPaidExpected, obj.GetAwardAmountPaid(), L"GetAwardAmountPaid");
                    ASSERT_TRUE(m_awardStateExpected, obj.GetAwardState(), L"GetAwardState");
                    ASSERT_TRUE(m_sentToGameCountExpected, obj.GetSentToGameCount(), L"GetSentToGameCount");
                    ASSERT_TRUE(0,
                        memcmp(&m_sentToGameTimeExpected, &obj.GetSentToGameTime(), sizeof(m_sentToGameTimeExpected)),
                        L"GetSentToGameTime");
                    ASSERT_TRUE(m_originalMachineBonusMeterValueExpected,
                        obj.GetOriginalMachineBonusMeterValue(),
                        L"GetOriginalMachineBonusMeterValue");
                    ASSERT_TRUE(m_originalAttendantBonusingMeterValueExpected,
                        obj.GetOriginalAttendantBonusingMeterValue(),
                        L"GetOriginalAttendantBonusingMeterValue");
    
                    ASSERT_TRUE(m_levelIdExpected, obj.GetLevelId(), L"GetLevelId");
                    ASSERT_TRUE(m_jackpotIdExpected, obj.GetJackpotId(), L"GetJackpotId");
                    ASSERT_TRUE(m_remoteJackpotIdExpected, obj.GetRemoteJackpotId(), L"GetRemoteJackpotId");
                    ASSERT_TRUE(m_awardAmountHitExpected, obj.GetAwardAmountHit(), L"GetAwardAmountHit");
    
                    ASSERT_TRUE(0,
                        memcmp(&m_receivedFromSystemTimeExpected, &obj.GetReceivedFromSystemTime(), sizeof(m_receivedFromSystemTimeExpected)),
                        L"GetReceivedFromSystemTime");
                    ASSERT_TRUE(m_duplicateHitCountExpected, obj.GetDuplicateHitCount(), L"GetDuplicateHitCount");
                    ASSERT_TRUE(m_otherHitCountExpected, obj.GetOtherHitCount(), L"GetOtherHitCount");
                    ASSERT_TRUE(m_awardTransferTypeExpected, obj.GetAwardTransferType(), L"GetAwardTransferType");
                }
    
                // extraCheckByte for v1.50.0 extension(s).
                if (recovered150)
                {
                    ASSERT_TRUE((EGMAwardPaidType)0x00000002, obj.GetAwardPaidType(), L"GetAwardPaidType");
    
                    ASSERT_TRUE(true, obj.IsStandardProgressive(), L"IsStandardProgressive");
                    int sessionId(-1);
                    ASSERT_TRUE(sessionIdIsValidExpected, obj.GetSessionId(sessionId), L"GetSessionId");
                    ASSERT_TRUE(sessionIdExpected, sessionId, L"sessionId");
    
                    ASSERT_TRUE(0x0d0d0d0d, obj.GetSASLevelId(), L"GetSASLevelId");
                    ASSERT_TRUE(true, obj.isHitReceived(), L"isHitReceived");
                    ASSERT_TRUE(false, obj.isHandpayPendingSignaled(), L"isHandpayPendingSignaled");
                    ASSERT_TRUE(true, obj.isEgmPaidSignaled(), L"isEgmPaidSignaled");
                    ASSERT_TRUE(0,
                        memcmp(&m_receivedFromEGMTime, &obj.GetReceivedFromEGMTime(), sizeof(m_receivedFromEGMTime)),
                        L"GetReceivedFromEGMTime");
                    ASSERT_TRUE(MethodOfPayment::TriggerOnly, obj.GetMethodOfPayment(), L"GetMethodOfPayment");
                }
                else
                {
                    ASSERT_TRUE(m_awardPaidTypeExpected, obj.GetAwardPaidType(), L"GetAwardPaidType");
    
                    ASSERT_TRUE(m_isStandardProgressiveExpected, obj.IsStandardProgressive(), L"IsStandardProgressive");
                    int sessionId(-1);
                    ASSERT_TRUE(m_sessionIdIsValidExpected, obj.GetSessionId(sessionId), L"GetSessionId");
                    ASSERT_TRUE(m_sessionIdExpected, sessionId, L"sessionId");
    
                    ASSERT_TRUE(m_sasLevelIdExpected, obj.GetSASLevelId(), L"GetSASLevelId");
                    ASSERT_TRUE(m_isHitReceived, obj.isHitReceived(), L"isHitReceived");
                    ASSERT_TRUE(m_isHandpayPendingSignaled, obj.isHandpayPendingSignaled(), L"isHandpayPendingSignaled");
                    ASSERT_TRUE(m_isEgmPaidSignaled, obj.isEgmPaidSignaled(), L"isEgmPaidSignaled");
                    ASSERT_TRUE(0,
                        memcmp(&m_receivedFromEGMTimeExpected, &obj.GetReceivedFromEGMTime(), sizeof(m_receivedFromEGMTimeExpected)),
                        L"GetReceivedFromEGMTime");
    
                    if (upgrade150)
                    {
                        ASSERT_TRUE(CEGMAward::UPGRADE_METHOD_OF_PAYMENT,
                            obj.GetMethodOfPayment(), L"GetMethodOfPayment");
                    }
                    else
                    {
                        ASSERT_TRUE(m_methodOfPaymentExpected, obj.GetMethodOfPayment(), L"GetMethodOfPayment");
                    }
                }
    
                // CheckByte3 for v1.51.0 extension(s).
                if (recovered151)
                {
                    ASSERT_TRUE((__int64)0x0f0f0f0f0f0f0f0f,
                        obj.GetOriginalLegacyBonusMeterValue(),
                        L"GetOriginalLegacyBonusMeterValue");
                }
                else
                {
                    ASSERT_TRUE(m_originalLegacyBonusMeterValueExpected,
                        obj.GetOriginalLegacyBonusMeterValue(),
                        L"GetOriginalLegacyBonusMeterValue");
                }
    
                ASSERT_TRUE(active, obj.IsActive(), L"IsActive");
            }
    
            void AssertEgmAward(const CEGMAward &obj,
                bool recoveredBase,
                bool recovered150,
                bool upgrade150, // Only used when recovered150 = false.
                bool recovered151,
                bool active)
            {
                AssertEgmAward(obj, recoveredBase, recovered150, upgrade150, recovered151, active,
                    m_sessionIdIsValidExpected, m_sessionIdExpected);
            }
    
            // #pragma region SetMemoryStorage_Persist
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetMemoryStorage()
            ///     CEGMAward::CEGMAward(MEMORY_TYPE, ...)
            ///     CEGMAward::BuildYourself()
            /// Test that using SetMemoryStorage() to save to NVRAM and restoring from NVRAM using the MEMORY_TYPE constructor
            /// produces an equivalent instance.
            /// ---------
            /// This tests persistance technology typically used with Standard (SAS progressive) awards.
            /// I.e., using a first instance, set all persistable member variables to unique non-default values
            /// and then save the instance to NVRAM -- using SetMemoryStorage() --,
            /// and then, using a second instance (with member variables set to default values), read from NVRAM,
            /// and verify that all the persistable values in the second instance match the values set with the first instance.
            /// </summary>
            TEST_F(EGMAward, SetMemoryStorage_Persist)
            {
                {
                    // Setup.
                    CUnitTestingUtils::SetNvramValidAndUpgraded();
                    CEGMAward obj(MEMORY_NVRAM, false, CEGMAward::NVRAM_DATA_OFFSET);
                    SetEgmAwardNonDefault(obj);
    
                    // Perform operation(s) and test(s).
                    obj.SetMemoryStorage(NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET);
                }
    
    {
        // Perform operation(s) and test(s).
        CEGMAward obj(MEMORY_NVRAM, true, NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET);
        AssertEgmAward(obj, true, true, false, true, true);
    }
            }
            // #pragma endregion SetMemoryStorage_Persist
    
            // #pragma region BuildYourself
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::StoreYourself()
            ///     CEGMAward::CEGMAward(MEMORY_TYPE, ...)
            ///     CEGMAward::BuildYourself()
            /// Test that saving to NVRAM and restoring from NVRAM using the MEMORY_TYPE constructor produces an equivalent
            /// instance.
            /// ---------
            /// This tests persistance technology typically used with both Bonus and Standard (SAS progressive) awards.
            /// I.e., using a first instance, set all persistable member variables to unique non-default values
            /// and then save the instance to NVRAM -- !!! using StoreYourself() !!!,
            /// and then, using a second instance (with member variables set to default values), read from NVRAM,
            /// and verify that all the persistable values in the second instance match the values set with the first instance.
            /// </summary>
            TEST_F(EGMAward, BuildYourself_StoreYourself)
            {
                {
                    // Setup.
                    CUnitTestingUtils::SetNvramValidAndUpgraded();
                    CEGMAward obj(MEMORY_NVRAM, false, CEGMAward::NVRAM_DATA_OFFSET);
                    SetEgmAwardNonDefault(obj);
    
                    // Perform operation(s) and test(s).
                    obj.StoreYourself();
                }
    
    {
        // Perform operation(s) and test(s).
        CEGMAward obj(MEMORY_NVRAM, true, CEGMAward::NVRAM_DATA_OFFSET);
        AssertEgmAward(obj, true, true, false, true, true);
    }
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetInactiveLocation()
            ///     CEGMAward::CEGMAward(MEMORY_TYPE, ...)
            ///     CEGMAward::BuildYourself()
            /// Test that calling SetActiveLocation(false) and restoring from NVRAM using the MEMORY_TYPE constructor produces a default
            /// instance.
            /// </summary>
            TEST_F(EGMAward, BuildYourself_SetActiveLocationFalse)
            {
                {
                    // Setup.
                    CUnitTestingUtils::SetNvramValidAndUpgraded();
                    CEGMAward obj(MEMORY_NVRAM, false, NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET);
                    SetEgmAwardNonDefault(obj);
    
                    // Perform operation(s) and test(s).
                    obj.StoreYourself();
                    obj.SetActiveLocation(false);
                    ASSERT_TRUE(true, obj.IsActive(), L"IsActive Set");
                }
    
    {
        // Perform operation(s) and test(s).
        CEGMAward obj(MEMORY_NVRAM, true, NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET);
        AssertEgmAward(obj, false, false, false, false, true);
    }
            }
            // #pragma endregion BuildYourself
    
            // #pragma region BuildYourself_Bool
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetInactiveLocation()
            ///     CEGMAward::CEGMAward(MEMORY_TYPE, ...)
            ///     CEGMAward::BuildYourself()
            /// Test calling SetActiveLocation(activeLocation) and restoring from NVRAM using the BuildYourself(buildInactive).
            /// </summary>
            void BuildYourself_SetActiveLocation(bool buildInactive, bool activeLocation)
            {
                {
                    // Setup.
                    CUnitTestingUtils::SetNvramValidAndUpgraded();
                    CEGMAward obj(MEMORY_NVRAM, false, NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET);
                    SetEgmAwardNonDefault(obj);
    
                    // Perform operation(s) and test(s).
                    obj.StoreYourself();
                    obj.SetActiveLocation(activeLocation);
                    ASSERT_TRUE(true, obj.IsActive(), L"IsActive Set");
                }
    
    {
        // Perform operation(s) and test(s).
        CEGMAward obj;
        obj.SetMemoryStorageParameters(MEMORY_NVRAM,
            NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET + NVRAM_EGM_AWARD_CHECKBYTE_SIZE,
            CEGMAward::NVRAM_DATA_SIZE,
            NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET,
            CEGMAward::NVRAM_ACTIVE_VALUE);
        obj.BuildYourself(buildInactive);
    
        if (activeLocation || buildInactive)
        {
            AssertEgmAward(obj, true, true, false, true, activeLocation);
        }
        else
        {
            AssertEgmAward(obj, false, false, false, false, true);
        }
    }
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetInactiveLocation()
            ///     CEGMAward::CEGMAward(MEMORY_TYPE, ...)
            ///     CEGMAward::BuildYourself()
            /// Test that calling SetActiveLocation(false) and restoring from NVRAM using the BuildYourself(false) produces a default
            /// instance.
            /// </summary>
            TEST_F(EGMAward, BuildYourselfFalse_SetActiveLocationFalse)
            {
                BuildYourself_SetActiveLocation(false, false);
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetInactiveLocation()
            ///     CEGMAward::CEGMAward(MEMORY_TYPE, ...)
            ///     CEGMAward::BuildYourself()
            /// Test that calling SetActiveLocation(true) and restoring from NVRAM using the BuildYourself(false) produces a default
            /// instance.
            /// </summary>
            TEST_F(EGMAward, BuildYourselfFalse_SetActiveLocationTrue)
            {
                BuildYourself_SetActiveLocation(false, true);
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetInactiveLocation()
            ///     CEGMAward::CEGMAward(MEMORY_TYPE, ...)
            ///     CEGMAward::BuildYourself()
            /// Test that calling SetActiveLocation(false) and restoring from NVRAM using the BuildYourself(true) produces a default
            /// instance.
            /// </summary>
            TEST_F(EGMAward, BuildYourselfTrue_SetActiveLocationFalse)
            {
                BuildYourself_SetActiveLocation(true, false);
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetInactiveLocation()
            ///     CEGMAward::CEGMAward(MEMORY_TYPE, ...)
            ///     CEGMAward::BuildYourself()
            /// Test that calling SetActiveLocation(true) and restoring from NVRAM using the BuildYourself(true) produces a default
            /// instance.
            /// </summary>
            TEST_F(EGMAward, BuildYourselfTrue_SetActiveLocationTrue)
            {
                BuildYourself_SetActiveLocation(true, true);
            }
            // #pragma endregion BuildYourself_Bool
    
            // #pragma region GetNvramBuffer
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::GetNvramBuffer()
            ///     CEGMAward::SetNvramBuffer()
            /// Test that a generated memory buffer is processed correctly by the same nCompass version.
            /// </summary>
            TEST_F(EGMAward, GetNvramBuffer_Set)
            {
                // Setup.
                bool active(false);
                BYTE buffer[CEGMAward::NVRAM_DATA_SIZE];
                ASSERT_TRUE((DWORD)256, CEGMAward::NVRAM_DATA_SIZE, L"CEGMAward::NVRAM_DATA_SIZE");
                memset(buffer, 0xae, sizeof(buffer));
    
                {
                    // Setup.
                    CEGMAward obj;
                    SetEgmAwardNonDefault(obj);
    
                    // Perform operation(s) and test(s).
                    // Set an NVRAM buffer with the persistable values from the first instance.
                    obj.GetNvramBuffer(buffer, sizeof(buffer), active);
                }
    
    {
        // Perform operation(s) and test(s).
        CEGMAward obj;
        obj.SetFromNvramBuffer(buffer, sizeof(buffer), active);
        AssertEgmAward(obj, true, true, false, true, true);
    }
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::GetNvramBuffer()
            ///     CEGMAward::SetNvramBuffer()
            /// Test that a generated memory buffer is processed correctly by the same nCompass version when the active flag is cleared.
            /// </summary>
            TEST_F(EGMAward, GetNvramBuffer_Set_ClearActive)
            {
                // Setup.
                bool active(false);
                BYTE buffer[CEGMAward::NVRAM_DATA_SIZE];
                ASSERT_TRUE((DWORD)256, CEGMAward::NVRAM_DATA_SIZE, L"CEGMAward::NVRAM_DATA_SIZE");
                memset(buffer, 0xae, sizeof(buffer));
    
                {
                    // Setup.
                    CEGMAward obj;
                    SetEgmAwardNonDefault(obj);
                    obj.ClearActive();
    
                    // Perform operation(s) and test(s).
                    obj.GetNvramBuffer(buffer, sizeof(buffer), active);
                    ASSERT_TRUE(false, obj.IsActive(), L"IsActive Get");
                    ASSERT_TRUE(false, active, L"active");
                }
    
    {
        // Perform operation(s) and test(s).
        CEGMAward obj;
        obj.SetFromNvramBuffer(buffer, sizeof(buffer), active);
        AssertEgmAward(obj, true, true, false, true, false);
    }
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::GetNvramBuffer()
            /// Test that memory buffer is generated correctly for backwards/forwards compatibility.
            /// </summary>
            TEST_F(EGMAward, GetNvramBuffer_NotActive)
            {
                // Data.
                const UINT resultExpected(0);
    
                // Setup.
                CEGMAward obj;
                obj.SetAwardState(NoAward);
    
                BYTE buffer[CEGMAward::NVRAM_DATA_SIZE];
                ASSERT_TRUE((DWORD)256, CEGMAward::NVRAM_DATA_SIZE, L"CEGMAward::NVRAM_DATA_SIZE");
    
                // Perform operation(s) and test(s).
                bool active(true);
                UINT result = obj.GetNvramBuffer(buffer, sizeof(buffer), active);
                ASSERT_TRUE(false, active, L"active");
                ASSERT_TRUE(resultExpected, result, L"result");
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::GetNvramBuffer()
            /// Test that memory buffer is generated correctly for backwards/forwards compatibility.
            /// </summary>
            TEST_F(EGMAward, GetNvramBuffer_Active)
            {
                // Data.
                const UINT resultExpected(151);
                BYTE bufferExpected[resultExpected] = {
                    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // m_awardAmountPaid.
                    0x02, 0x00, 0x00, 0x00, // m_awardState.
                    0x03, 0x03, 0x03, 0x03, // m_sentToGameCount.
                    0xd1, 0x07, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0c, 0x00, 0x22, 0x00, 0x38, 0x00, 0x15, 0x03, // sentToGameTime.
                    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, // m_originalMachineBonusMeterValue.
                    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, // m_originalAttendantBonusingMeterValue.
    
                    0x06, 0x06, 0x06, 0x06, // m_levelId.
                    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, // m_jackpotId.
                    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, // m_remoteJackpotId.
                    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, // m_awardAmountHit.
    
                    0xdb, 0x07, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0c, 0x00, 0x22, 0x00, 0x38, 0x00, 0x15, 0x03, // m_receivedFromSystemTime.
                    0x0a, 0x0a, 0x0a, 0x0a, // m_duplicateHitCount.
                    0x0b, 0x0b, 0x0b, 0x0b, // m_otherHitCount.
                    0x01, 0x00, 0x00, 0x00, // m_awardTransferType.
    
                    0x1e, // extraCheckByte for v1.50.0 extension(s).
                    0x02, 0x00, 0x00, 0x00, // m_paidType.
    
                    0x01, // m_isStandardProgressive.
                    0x00, // m_sessionId.isValid.
                    0x00, 0x00, 0x00, 0x00, // m_sessionId.val.
    
                    0x0d, 0x0d, 0x0d, 0x0d, // m_sasLevelId.
                    0x01, // m_isHitReceived.
                    0x00, // m_isHandpayPendingSignaled.
                    0x01, // m_isEgmPaidSignaled.
                    0xdd, 0x07, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0c, 0x00, 0x22, 0x00, 0x38, 0x00, 0x15, 0x03, // m_receivedFromEGMTime.
                    0x04, 0x00, 0x00, 0x00, // m_methodOfPayment = MethodOfPayment::TriggerOnly.
    
                    0x1f, // CheckByte3 for v1.51.0 extension(s) -- set valid.
                    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, // m_originalAttendantBonusingMeterValue.
                };
    
                // Setup.
                CEGMAward obj;
                SetEgmAwardNonDefault(obj);
    
                BYTE buffer[CEGMAward::NVRAM_DATA_SIZE];
                ASSERT_TRUE((DWORD)256, CEGMAward::NVRAM_DATA_SIZE, L"CEGMAward::NVRAM_DATA_SIZE");
    
                // Perform operation(s) and test(s).
                bool active(false);
                UINT result = obj.GetNvramBuffer(buffer, sizeof(buffer), active);
                ASSERT_TRUE(true, active, L"active");
                ASSERT_TRUE(resultExpected, result, L"result");
                ASSERT_TRUE(0, memcmp(bufferExpected, buffer, resultExpected), L"memcmp");
                ASSERT_TRUE(true, obj.IsActive(), L"IsActive");
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetNvramBuffer()
            /// Test that a memory buffer with no valid upgrade check bytes is processed correctly for backwards/forwards
            /// compatibility when the NVRAM location is either active or inactive.
            /// </summary>
            void SetNvramBuffer0(bool activeLocation)
            {
                // Data.
                ASSERT_TRUE((DWORD)256, CEGMAward::NVRAM_DATA_SIZE, L"CEGMAward::NVRAM_DATA_SIZE");
                BYTE buffer[CEGMAward::NVRAM_DATA_SIZE] = {
                    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // m_awardAmountPaid.
                    0x02, 0x00, 0x00, 0x00, // m_awardState.
                    0x03, 0x03, 0x03, 0x03, // m_sentToGameCount.
                    0xd1, 0x07, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0c, 0x00, 0x22, 0x00, 0x38, 0x00, 0x15, 0x03, // sentToGameTime.
                    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, // m_originalMachineBonusMeterValue.
                    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, // m_originalAttendantBonusingMeterValue.
    
                    0x06, 0x06, 0x06, 0x06, // m_levelId.
                    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, // m_jackpotId.
                    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, // m_remoteJackpotId.
                    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, // m_awardAmountHit.
    
                    0xdb, 0x07, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0c, 0x00, 0x22, 0x00, 0x38, 0x00, 0x15, 0x03, // m_receivedFromSystemTime.
                    0x0a, 0x0a, 0x0a, 0x0a, // m_duplicateHitCount.
                    0x0b, 0x0b, 0x0b, 0x0b, // m_otherHitCount.
                    0x01, 0x00, 0x00, 0x00, // m_awardTransferType.
    
                    0xee, // extraCheckByte invalid.
                    0x02, 0x00, 0x00, 0x00, // m_paidType.
    
                    0x01, // m_isStandardProgressive.
                    0x01, // m_sessionId.isValid.
                    0x0c, 0x0c, 0x0c, 0x0c, // m_sessionId.val.
    
                    0x0d, 0x0d, 0x0d, 0x0d, // m_sasLevelId.
                    0x01, // m_isHitReceived.
                    0x00, // m_isHandpayPendingSignaled.
                    0x01, // m_isEgmPaidSignaled.
                    0xdd, 0x07, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0c, 0x00, 0x22, 0x00, 0x38, 0x00, 0x15, 0x03, // m_receivedFromEGMTime.
                    0x02, 0x00, 0x00, 0x00, // m_methodOfPayment = MethodOfPayment::PayToGameWithHandPay.
    
                    0xee, // CheckByte3 for v1.51.0 extension(s) -- invalid.
                    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, // m_originalAttendantBonusingMeterValue.
    
                    // Pad with generally unexpected values.
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                };
                Assert::AreNotEqual(CEGMAward::UPGRADE_METHOD_OF_PAYMENT,
                    (MethodOfPayment::Type)0x02, L"(MethodOfPayment::Type)0x01"); // Verify data.
    
                // Setup.
                CEGMAward obj;
    
                // Perform operation(s) and test(s).
                obj.SetFromNvramBuffer(buffer, sizeof(buffer), activeLocation);
    
                if (activeLocation)
                {
                    AssertEgmAward(obj, true, false, true, false, true);
                }
                else
                {
                    AssertEgmAward(obj, false, false, false, false, true);
                }
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetNvramBuffer()
            /// Test that a memory buffer with no valid upgrade check bytes is processed correctly for backwards/forwards
            /// compatibility when the NVRAM location is inactive.
            /// </summary>
            TEST_F(EGMAward, SetNvramBuffer0_LocationInactive)
            {
                SetNvramBuffer0(false);
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetNvramBuffer()
            /// Test that a memory buffer with no valid upgrade check bytes is processed correctly for backwards/forwards
            /// compatibility when the NVRAM location is active.
            /// </summary>
            TEST_F(EGMAward, SetNvramBuffer0_LocationActive)
            {
                SetNvramBuffer0(true);
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetNvramBuffer()
            /// Test that a memory buffer processed correctly for backwards/forwards compatibility
            /// when the extra check byte for 1.5.0 extensions is invalid and the CheckByte3 for 1.51.0 extensions is valid
            /// when the NVRAM location is either active or inactive.
            /// </summary>
            void SetNvramBuffer0b(bool activeLocation)
            {
                // Data.
                ASSERT_TRUE((DWORD)256, CEGMAward::NVRAM_DATA_SIZE, L"CEGMAward::NVRAM_DATA_SIZE");
                BYTE buffer[CEGMAward::NVRAM_DATA_SIZE] = {
                    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // m_awardAmountPaid.
                    0x02, 0x00, 0x00, 0x00, // m_awardState.
                    0x03, 0x03, 0x03, 0x03, // m_sentToGameCount.
                    0xd1, 0x07, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0c, 0x00, 0x22, 0x00, 0x38, 0x00, 0x15, 0x03, // sentToGameTime.
                    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, // m_originalMachineBonusMeterValue.
                    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, // m_originalAttendantBonusingMeterValue.
    
                    0x06, 0x06, 0x06, 0x06, // m_levelId.
                    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, // m_jackpotId.
                    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, // m_remoteJackpotId.
                    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, // m_awardAmountHit.
    
                    0xdb, 0x07, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0c, 0x00, 0x22, 0x00, 0x38, 0x00, 0x15, 0x03, // m_receivedFromSystemTime.
                    0x0a, 0x0a, 0x0a, 0x0a, // m_duplicateHitCount.
                    0x0b, 0x0b, 0x0b, 0x0b, // m_otherHitCount.
                    0x01, 0x00, 0x00, 0x00, // m_awardTransferType.
    
                    0xee, // extraCheckByte invalid.
                    0x02, 0x00, 0x00, 0x00, // m_paidType.
    
                    0x01, // m_isStandardProgressive.
                    0x01, // m_sessionId.isValid.
                    0x0c, 0x0c, 0x0c, 0x0c, // m_sessionId.val.
    
                    0x0d, 0x0d, 0x0d, 0x0d, // m_sasLevelId.
                    0x01, // m_isHitReceived.
                    0x00, // m_isHandpayPendingSignaled.
                    0x01, // m_isEgmPaidSignaled.
                    0xdd, 0x07, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0c, 0x00, 0x22, 0x00, 0x38, 0x00, 0x15, 0x03, // m_receivedFromEGMTime.
                    0x02, 0x00, 0x00, 0x00, // m_methodOfPayment = MethodOfPayment::PayToGameWithHandPay.
    
                    0x1f, // CheckByte3 for v1.51.0 extension(s) -- valid.
                    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, // m_originalAttendantBonusingMeterValue.
    
                    // Pad with generally unexpected values.
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                };
                Assert::AreNotEqual(CEGMAward::UPGRADE_METHOD_OF_PAYMENT,
                    (MethodOfPayment::Type)0x02, L"(MethodOfPayment::Type)0x01"); // Verify data.
    
                // Setup.
                CEGMAward obj;
    
                // Perform operation(s) and test(s).
                obj.SetFromNvramBuffer(buffer, sizeof(buffer), activeLocation);
    
                if (activeLocation)
                {
                    AssertEgmAward(obj, true, false, true, false, true);
                }
                else
                {
                    AssertEgmAward(obj, false, false, false, false, true);
                }
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetNvramBuffer()
            /// Test that a memory buffer processed correctly for backwards/forwards compatibility
            /// when the extra check byte for 1.5.0 extensions is invalid and the CheckByte3 for 1.51.0 extensions is valid
            /// when the NVRAM location is inactive.
            /// </summary>
            TEST_F(EGMAward, SetNvramBuffer0b_LocationInactive)
            {
                SetNvramBuffer0b(false);
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetNvramBuffer()
            /// Test that a memory buffer processed correctly for backwards/forwards compatibility
            /// when the extra check byte for 1.5.0 extensions is invalid and the CheckByte3 for 1.51.0 extensions is valid
            /// when the NVRAM location is active.
            /// </summary>
            TEST_F(EGMAward, SetNvramBuffer0b_LocationActive)
            {
                SetNvramBuffer0b(true);
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetNvramBuffer()
            /// Test that a memory buffer processed correctly for backwards/forwards compatibility
            /// when the extra check byte for 1.5.0 extensions is valid and the CheckByte3 for 1.51.0 extensions is invalid
            /// when the NVRAM location is either active or inactive.
            /// </summary>
            void SetNvramBuffer1(bool activeLocation)
            {
                // Data.
                ASSERT_TRUE((DWORD)256, CEGMAward::NVRAM_DATA_SIZE, L"CEGMAward::NVRAM_DATA_SIZE");
                BYTE buffer[CEGMAward::NVRAM_DATA_SIZE] = {
                    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // m_awardAmountPaid.
                    0x02, 0x00, 0x00, 0x00, // m_awardState.
                    0x03, 0x03, 0x03, 0x03, // m_sentToGameCount.
                    0xd1, 0x07, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0c, 0x00, 0x22, 0x00, 0x38, 0x00, 0x15, 0x03, // sentToGameTime.
                    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, // m_originalMachineBonusMeterValue.
                    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, // m_originalAttendantBonusingMeterValue.
    
                    0x06, 0x06, 0x06, 0x06, // m_levelId.
                    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, // m_jackpotId.
                    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, // m_remoteJackpotId.
                    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, // m_awardAmountHit.
    
                    0xdb, 0x07, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0c, 0x00, 0x22, 0x00, 0x38, 0x00, 0x15, 0x03, // m_receivedFromSystemTime.
                    0x0a, 0x0a, 0x0a, 0x0a, // m_duplicateHitCount.
                    0x0b, 0x0b, 0x0b, 0x0b, // m_otherHitCount.
                    0x01, 0x00, 0x00, 0x00, // m_awardTransferType.
    
                    0x1e, // extraCheckByte for v1.50.0 extension(s).
                    0x02, 0x00, 0x00, 0x00, // m_paidType.
    
                    0x01, // m_isStandardProgressive.
                    0x01, // m_sessionId.isValid.
                    0x0c, 0x0c, 0x0c, 0x0c, // m_sessionId.val.
    
                    0x0d, 0x0d, 0x0d, 0x0d, // m_sasLevelId.
                    0x01, // m_isHitReceived.
                    0x00, // m_isHandpayPendingSignaled.
                    0x01, // m_isEgmPaidSignaled.
                    0xdd, 0x07, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0c, 0x00, 0x22, 0x00, 0x38, 0x00, 0x15, 0x03, // m_receivedFromEGMTime.
                    0x04, 0x00, 0x00, 0x00, // m_methodOfPayment = MethodOfPayment::TriggerOnly.
    
                    0xee, // CheckByte3 for v1.51.0 extension(s) -- invalid.
                    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, // m_originalAttendantBonusingMeterValue.
    
                    // Pad with generally unexpected values.
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                };
    
                // Setup.
                CEGMAward obj;
    
                // Perform operation(s) and test(s).
                obj.SetFromNvramBuffer(buffer, sizeof(buffer), activeLocation);
                AssertEgmAward(obj, true, true, false, false, activeLocation, true, 0x0c0c0c0c);
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetNvramBuffer()
            /// Test that a memory buffer processed correctly for backwards/forwards compatibility
            /// when the extra check byte for 1.5.0 extensions is valid and the CheckByte3 for 1.51.0 extensions is invalid
            /// when the NVRAM location is inactive.
            /// </summary>
            TEST_F(EGMAward, SetNvramBuffer1_LocationInactive)
            {
                SetNvramBuffer1(false);
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetNvramBuffer()
            /// Test that a memory buffer processed correctly for backwards/forwards compatibility
            /// when the extra check byte for 1.5.0 extensions is valid and the CheckByte3 for 1.51.0 extensions is invalid
            /// when the NVRAM location is active.
            /// </summary>
            TEST_F(EGMAward, SetNvramBuffer1_LocationActive)
            {
                SetNvramBuffer1(true);
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetNvramBuffer()
            /// Test that a memory buffer processed correctly for backwards/forwards compatibility
            /// when the extra check byte for 1.5.0 extensions is valid and the CheckByte3 for 1.51.0 extensions is valid
            /// when the NVRAM location is either active or inactive.
            /// </summary>
            void SetNvramBuffer1b(bool activeLocation)
            {
                // Data.
                ASSERT_TRUE((DWORD)256, CEGMAward::NVRAM_DATA_SIZE, L"CEGMAward::NVRAM_DATA_SIZE");
                BYTE buffer[CEGMAward::NVRAM_DATA_SIZE] = {
                    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // m_awardAmountPaid.
                    0x02, 0x00, 0x00, 0x00, // m_awardState.
                    0x03, 0x03, 0x03, 0x03, // m_sentToGameCount.
                    0xd1, 0x07, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0c, 0x00, 0x22, 0x00, 0x38, 0x00, 0x15, 0x03, // sentToGameTime.
                    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, // m_originalMachineBonusMeterValue.
                    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, // m_originalAttendantBonusingMeterValue.
    
                    0x06, 0x06, 0x06, 0x06, // m_levelId.
                    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, // m_jackpotId.
                    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, // m_remoteJackpotId.
                    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, // m_awardAmountHit.
    
                    0xdb, 0x07, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0c, 0x00, 0x22, 0x00, 0x38, 0x00, 0x15, 0x03, // m_receivedFromSystemTime.
                    0x0a, 0x0a, 0x0a, 0x0a, // m_duplicateHitCount.
                    0x0b, 0x0b, 0x0b, 0x0b, // m_otherHitCount.
                    0x01, 0x00, 0x00, 0x00, // m_awardTransferType.
    
                    0x1e, // extraCheckByte for v1.50.0 extension(s).
                    0x02, 0x00, 0x00, 0x00, // m_paidType.
    
                    0x01, // m_isStandardProgressive.
                    0x01, // m_sessionId.isValid.
                    0x0c, 0x0c, 0x0c, 0x0c, // m_sessionId.val.
    
                    0x0d, 0x0d, 0x0d, 0x0d, // m_sasLevelId.
                    0x01, // m_isHitReceived.
                    0x00, // m_isHandpayPendingSignaled.
                    0x01, // m_isEgmPaidSignaled.
                    0xdd, 0x07, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0c, 0x00, 0x22, 0x00, 0x38, 0x00, 0x15, 0x03, // m_receivedFromEGMTime.
                    0x04, 0x00, 0x00, 0x00, // m_methodOfPayment = MethodOfPayment::TriggerOnly.
    
                    0x1f, // CheckByte3 for v1.51.0 extension(s) -- invalid.
                    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, // m_originalAttendantBonusingMeterValue.
    
                    // Pad with generally unexpected values.
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                };
    
                // Setup.
                CEGMAward obj;
    
                // Perform operation(s) and test(s).
                obj.SetFromNvramBuffer(buffer, sizeof(buffer), activeLocation);
                AssertEgmAward(obj, true, true, false, true, activeLocation, true, 0x0c0c0c0c);
                ASSERT_TRUE(activeLocation, obj.IsActive(), L"IsActive");
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetNvramBuffer()
            /// Test that a memory buffer processed correctly for backwards/forwards compatibility
            /// when the extra check byte for 1.5.0 extensions is valid and the CheckByte3 for 1.51.0 extensions is valid
            /// when the NVRAM location is inactive.
            /// </summary>
            TEST_F(EGMAward, SetNvramBuffer1b_LocationInactive)
            {
                SetNvramBuffer1b(false);
            }
    
            /// <summary>
            /// Functional test for:
            ///     CEGMAward::SetNvramBuffer()
            /// Test that a memory buffer processed correctly for backwards/forwards compatibility
            /// when the extra check byte for 1.5.0 extensions is valid and the CheckByte3 for 1.51.0 extensions is valid
            /// when the NVRAM location is active.
            /// </summary>
            TEST_F(EGMAward, SetNvramBuffer1b_LocationActive)
            {
                SetNvramBuffer1b(true);
            }
            // #pragma endregion GetNvramBuffer
#endif

    const SYSTEMTIME EGMAward::m_sentToGameTimeExpected = { 0, 0, 0, 0, 0, 0, 0, 0 };
    const SYSTEMTIME EGMAward::m_receivedFromSystemTimeExpected = { 0, 0, 0, 0, 0, 0, 0, 0 };
    const SYSTEMTIME EGMAward::m_receivedFromEGMTimeExpected = { 0, 0, 0, 0, 0, 0, 0, 0 };
    const string EGMAward::m_statusInfoExpected;
    const string EGMAward::m_statusInfoBuildYourself(_T("Restart occurred"));
    const string EGMAward::m_logInfoStringExpected(
        _T("AWARD: rcvd=N/A state=\"No Award\" lvl=0 sasLvl=255 std=0 sesId= id=-1 hit=0 paid=0 sent=0 sentTime=N/A")
        _T(" machBnsMtr=-1 attnBnsMtr=-1 legcyBnsMtr=-1 dups=0 other=0")
        _T(" xferType=\"None\" hitRcvd=0 hpPend=0 paidSig=0 fromEgm=N/A")
        _T(" methOfPay=-1 awardType= fromPromoGame=0")
        _T(" info=\"\" -- "));

    const SYSTEMTIME EGMAward::SYSTEMTIME_INITIAL_VALUE = { 1971, 2, 3, 3, 12, 34, 56, 789 };

    const LPCSTR EGMAward::m_validJson(
        "{"
        "\"JackpotId\":909,"
        "\"RemoteJackpotId\":0,"
        "\"WinningDeviceId\":6467,"
        "\"LevelId\":23886,"
        "\"LevelName\":\"JacksonTest\","
        "\"OverageAdjustment\":0,"
        "\"Amount\":3,"
        "\"OriginalAmount\":0,"
        "\"JackpotType\":\"Bonus\","
        "\"JackpotTriggerType\":\"CoinInTrigger\","
        "\"PriorityAward\":false,"
        "\"WinningAssetNumber\":\"113087\","
        "\"Remote\":false,"
        "\"MethodOfPayment\":\"PayToGame\","
        "\"AwardType\":\"CashableCredits\","
        "\"SessionId\":123123,"
        "\"SourceDeviceId\":1,"
        "\"SourceDeviceConfigVersion\":0,"
        "\"SourceAsset\":\"\","
        "\"Timestamp\":1434039783129,"
        "\"MessageId\":\"909-1\""
        "}");

    // Unique non-default values.
    const SYSTEMTIME EGMAward::m_sentToGameTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
    const SYSTEMTIME EGMAward::m_receivedFromSystemTime = { 2011, 2, 0, 3, 12, 34, 56, 789 };
    const SYSTEMTIME EGMAward::m_receivedFromEGMTime = { 2013, 2, 0, 3, 12, 34, 56, 789 };
}
