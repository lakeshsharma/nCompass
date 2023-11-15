#include "stdafx.h"
#include "SASProtocol/SASProtocol.h"
#include "SASProtocol/SASPollMessage.h"
#include "gtest/gtest.h"

// Sentinel includes.
#include "SASProtocol/LongPoll01Message.h"
#include "SASProtocol/LongPoll02Message.h"
#include "SASProtocol/LongPoll09Message.h"
#include "SASProtocol/LongPoll0EMessage.h"
#include "SASProtocol/LongPoll10Message.h"
#include "SASProtocol/LongPoll19Message.h"
#include "SASProtocol/LongPoll1BMessage.h"
#include "SASProtocol/LongPoll1DMessage.h"
#include "SASProtocol/LongPoll1EMessage.h"
#include "SASProtocol/LongPoll1FMessage.h"
#include "SASProtocol/LongPoll27Message.h"
#include "SASProtocol/LongPoll28Message.h"
#include "SASProtocol/LongPoll2AMessage.h"
#include "SASProtocol/LongPoll2BMessage.h"
#include "SASProtocol/LongPoll2DMessage.h"
#include "SASProtocol/LongPoll2EMessage.h"
#include "SASProtocol/LongPoll32Message.h"
#include "SASProtocol/LongPoll39Message.h"
#include "SASProtocol/LongPoll3AMessage.h"
#include "SASProtocol/LongPoll3DMessage.h"
#include "SASProtocol/LongPoll46Message.h"
#include "SASProtocol/LongPoll4DMessage.h"
#include "SASProtocol/LongPoll50Message.h"
#include "SASProtocol/LongPoll51Message.h"
#include "SASProtocol/LongPoll52Message.h"
#include "SASProtocol/LongPoll53Message.h"
#include "SASProtocol/LongPoll55Message.h"
#include "SASProtocol/LongPoll56Message.h"
#include "SASProtocol/LongPoll6AMessage.h"
#include "SASProtocol/LongPoll70Message.h"
#include "SASProtocol/LongPoll72Message.h"
#include "SASProtocol/LongPoll74Message.h"
#include "SASProtocol/LongPoll7FMessage.h"
#include "SASProtocol/LongPoll83Message.h"
#include "SASProtocol/LongPoll84Message.h"
#include "SASProtocol/LongPoll85Message.h"
#include "SASProtocol/LongPoll94Message.h"
#include "SASProtocol/LongPollA0Message.h"
#include "SASProtocol/LongPollA8Message.h"
#include "SASProtocol/LongPollB056Message.h"
#include "SASProtocol/LongPollB0B5Message.h"
#include "SASProtocol/LongPollB1Message.h"
#include "SASProtocol/LongPollB2Message.h"
#include "SASProtocol/LongPollB6Message.h"
#include "SASProtocol/LongPollB7Message.h"
#include "SASProtocol/SASGeneralPoll.h"

// Unit testing includes.
//#include "UnitTestingToString.h"
#include "UnitTestingUtils.h"

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a class initialization method.
#define AssertResult3i(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, 0, __TFUNCTION__, __LINE__);
#define AssertResult2i(result, expected) AssertResult3i(result, expected, _T(#result))

namespace SentinelUnitTests
{
class SASPollMessageTests : public ::testing::Test
{
protected:

   // Default expected result(s).
   static const BYTE m_gameAddressNackOrValueExpected = 0x80;

   // Class member(s) used by test (data) methods.
   int m_testIndex;

   virtual void SetUp()
   {
      // Test static default value(s).
      AssertResult2i(CSASPollMessage::GAME_ADDRESS_NACK_OR_VALUE, m_gameAddressNackOrValueExpected);
   }

   virtual void TearDown()
   {
      // Class cleanup code.
   }
};

/// <summary>
/// Test that protocol-dependent SAS meter code enumeration values are correct.
/// </summary>
TEST_F(SASPollMessageTests, SASMeterCode_Values)
{
   // Perform test(s).
   ASSERT_EQ((SASMeterCode)0x08, smcTotalCreditsFromCoinAcceptor);
   ASSERT_EQ((SASMeterCode)0x09, smcTotalCreditsPaidFromHopper);
   ASSERT_EQ((SASMeterCode)0x0a, smcTotalCreditsFromCoinsToDrop);
   ASSERT_EQ((SASMeterCode)0x0d, smcTotalSasCashableTicketIn_IncludingNonrestricted);
   ASSERT_EQ((SASMeterCode)0x10, smcTotalSasRestrictedTicketOut);
}

// Following code tests friend functions, which gtest cannot do. Need to find a to test Bcd2Dword and Bcd2Int

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Dword()
/// Test with buffer nullptr.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Dword_BufferNullptr)
{
   // Data.
   const DWORD returnExpected(0);
   const BYTE *buffer(nullptr);
   const UINT bufferSize(4);

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Dword(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Dword()
/// Test with buffer length of 0.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Dword_Buffer0)
{
   // Data.
   const DWORD returnExpected(0);
   const BYTE buffer[1] = { 0x12 };
   const UINT bufferSize(0);

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Dword(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Dword()
/// Test with buffer length of 1.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Dword_Buffer1)
{
   // Data.
   const DWORD returnExpected(12);
   const BYTE buffer[1] = { 0x12 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Dword(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Dword()
/// Test with buffer length of 1 with the low nibble invalid.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Dword_Buffer1InvalidLowNibble)
{
   // Data.
   const DWORD returnExpected(10);
   const BYTE buffer[1] = { 0x0a };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Dword(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Dword()
/// Test with buffer length of 1 with the high nibble invalid.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Dword_Buffer1InvalidHighNibble)
{
   // Data.
   const DWORD returnExpected(100);
   const BYTE buffer[1] = { 0xa0 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Dword(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Dword()
/// Test with buffer length of 4 with a relatively small value.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Dword_Buffer4a)
{
   // Data.
   const DWORD returnExpected(1234);
   const BYTE buffer[4] = { 0x00, 0x00, 0x12, 0x34 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Dword(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Dword()
/// Test with buffer length of 4 with a relatively large value.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Dword_Buffer4b)
{
   // Data.
   const DWORD returnExpected(12345678);
   const BYTE buffer[4] = { 0x12, 0x34, 0x56, 0x78 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Dword(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Dword()
/// Test with buffer length of 5 and leading zero(s).
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Dword_Buffer5)
{
   // Data.
   const DWORD returnExpected(12345678);
   const BYTE buffer[5] = { 0x00, 0x12, 0x34, 0x56, 0x78 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Dword(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Dword()
/// Test with buffer length of 5 and maximum DWORD value with no overflow.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Dword_Buffer5Maximum)
{
   // Data.
   const DWORD returnExpected(DWORD_MAX);
   const BYTE buffer[5] = { 0x42, 0x94, 0x96, 0x72, 0x95 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Dword(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Dword()
/// Test with buffer length of 5 and minimum overflow.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Dword_Buffer5OverflowMin)
{
   // Data.
   const DWORD returnExpected(0);
   const BYTE buffer[5] = { 0x42, 0x94, 0x96, 0x72, 0x96 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Dword(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Dword()
/// Test with buffer length of 5 and maximum overflow.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Dword_Buffer5OverflowMax)
{
   // Data.
   const DWORD returnExpected((DWORD)9999999999);
   const BYTE buffer[5] = { 0x99, 0x99, 0x99, 0x99, 0x99 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Dword(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Int64()
/// Test with buffer nullptr.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Int64_BufferNullptr)
{
   // Data.
   const __int64 returnExpected(0);
   const BYTE *buffer(nullptr);
   const UINT bufferSize(4);

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Int64(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Int64()
/// Test with buffer length of 0.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Int64_Buffer0)
{
   // Data.
   const __int64 returnExpected(0);
   const BYTE buffer[1] = { 0x12 };
   const UINT bufferSize(0);

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Int64(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Int64()
/// Test with buffer length of 1.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Int64_Buffer1)
{
   // Data.
   const __int64 returnExpected(12);
   const BYTE buffer[1] = { 0x12 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Int64(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Int64()
/// Test with buffer length of 1 with the low nibble invalid.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Int64_Buffer1InvalidLowNibble)
{
   // Data.
   const __int64 returnExpected(10);
   const BYTE buffer[1] = { 0x0a };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Int64(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Int64()
/// Test with buffer length of 1 with the high nibble invalid.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Int64_Buffer1InvalidHighNibble)
{
   // Data.
   const __int64 returnExpected(100);
   const BYTE buffer[1] = { 0xa0 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Int64(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Int64()
/// Test with buffer length of 9 with a relatively small value.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Int64_Buffer9a)
{
   // Data.
   const __int64 returnExpected(1234);
   const BYTE buffer[9] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Int64(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Int64()
/// Test with buffer length of 9 with a relatively large value.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Int64_Buffer9b)
{
   // Data.
   const __int64 returnExpected(123456789012345678);
   const BYTE buffer[9] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34, 0x56, 0x78 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Int64(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Int64()
/// Test with buffer length of 10 and leading zero(s).
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Int64_Buffer10)
{
   // Data.
   const __int64 returnExpected(123456789012345678);
   const BYTE buffer[10] = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34, 0x56, 0x78 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Int64(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Int64()
/// Test with buffer length of 10 and maximum __int64 value with no overflow.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Int64_Buffer10Maximum)
{
   // Data.
   const __int64 returnExpected(INT64_MAX);
   const BYTE buffer[10] = { 0x09, 0x22, 0x33, 0x72, 0x03, 0x68, 0x54, 0x77, 0x58, 0x07 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Int64(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Int64()
/// Test with buffer length of 10 and minimum overflow.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Int64_Buffer10OverflowMin)
{
   // Data.
   const __int64 returnExpected(INT64_MIN);
   const BYTE buffer[10] = { 0x09, 0x22, 0x33, 0x72, 0x03, 0x68, 0x54, 0x77, 0x58, 0x08 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Int64(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::Bcd2Int64()
/// Test with buffer length of 10 and maximum overflow.
/// </summary>
TEST_F(SASPollMessageTests, Bcd2Int64_Buffer10OverflowMax)
{
   // Data.
   const __int64 returnExpected(7766279631452241919);
   const BYTE buffer[10] = { 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99 };
   const UINT bufferSize(sizeof(buffer));

   // Perform operation(s) and test(s).
   ASSERT_EQ(returnExpected, CSASGeneralPoll::Bcd2Int64(buffer, bufferSize));
}

/// <summary>
/// Code coverage and functional test for:
///     CSASGeneralPoll::IsGeneralPoll()
/// Test return value.
/// </summary>
TEST_F(SASPollMessageTests, IsGeneralPoll_GeneralPollTest)
{
   // Data.
   const BYTE gameAddress(1);
   const bool isGeneralPollExpected(true);

   // Setup.
   CSASGeneralPoll obj(gameAddress);

   // Perform operation(s) and test(s).
   AssertResult2(obj.IsGeneralPoll(), isGeneralPollExpected);
}

/// <summary>
/// Code coverage and functional test for:
///     CSASPollMessage::IsGeneralPoll()
/// Test return value.
/// </summary>
TEST_F(SASPollMessageTests, IsGeneralPoll_LongPoll01MessageTest)
{
   // Data.
   const BYTE gameAddress(1);
   const bool isGeneralPollExpected(false);

   // Setup.
   CLongPoll01Message obj(gameAddress);

   // Perform operation(s) and test(s).
   AssertResult2(obj.IsGeneralPoll(), isGeneralPollExpected);

   obj.ResponseReceived();

   obj.Fill();

   ASSERT_EQ(obj.GetPollValue(), 0x01);

   ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll02MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll02Message obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x02);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll09MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll09Message obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.SetGameNumber(1);

    obj.Fill();

    obj.Fill(true);

    ASSERT_EQ(obj.GetPollValue(), 0x09);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll0EMessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll0EMessage obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.EnableRealTimeReporting(true);

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x0E);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll10MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CMeters meters(MEMORY_TYPE::MEMORY_NONE, false);
    CLongPoll10Message obj(gameAddress, &meters);

    // Perform operation(s) and test(s).

    obj.m_cancelledCredits = 50;

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x10);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll19MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CMeters meters(MEMORY_TYPE::MEMORY_NONE, false);
    meters.SetMeter(gCI, INVALID_METER_VALUE);
    meters.SetMeter(gCO, INVALID_METER_VALUE);
    meters.SetMeter(mTotalDrop, INVALID_METER_VALUE);
    meters.SetMeter(mCD, INVALID_METER_VALUE);
    meters.SetMeter(gJP, INVALID_METER_VALUE);
    meters.SetMeter(gGS, INVALID_METER_VALUE);
    CLongPoll19Message obj(gameAddress, &meters);

    // Perform operation(s) and test(s).

    obj.m_coinIn = 100;
    obj.m_coinOut = 50;
    obj.m_totalDrop = 50;
    obj.m_jackpot = 0;
    obj.m_gamesPlayed = 1;

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x19);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll1BMessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll1BMessage obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.m_progressiveGroup = 0;
    obj.m_progressiveLevel = 1;
    obj.m_amount = 1000;
    obj.m_partialAmountPaid = 0;
    obj.m_resetID = 0;

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x1B);

    ASSERT_EQ(obj.SwapPollValue(), 0);

    ASSERT_EQ(obj.isEmpty(), true);

    ASSERT_EQ(obj.KeyToCreditAvailable(), false);
}

TEST_F(SASPollMessageTests, LongPoll1DMessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CMeters meters(MEMORY_TYPE::MEMORY_NONE, false);
    meters.SetMeter(mPbAR2G, INVALID_METER_VALUE);
    meters.SetMeter(mPbAN2G, INVALID_METER_VALUE);
    meters.SetMeter(mPbAC2H, INVALID_METER_VALUE);
    meters.SetMeter(mPbAC2G, INVALID_METER_VALUE);
    CLongPoll1DMessage obj(gameAddress, &meters);

    // Perform operation(s) and test(s).

    obj.m_promoCreditIn = 100;
    obj.m_nonCashCreditIn = 100;
    obj.m_transferredCredits = 0;
    obj.m_cashableCredits = 0;

    obj.SetDenomMultiplier(1);

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x1D);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll1EMessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CMeters meters(MEMORY_TYPE::MEMORY_NONE, false);
    meters.SetMeter(mPbAR2G, INVALID_METER_VALUE);
    meters.SetMeter(mPbAN2G, INVALID_METER_VALUE);
    meters.SetMeter(mPbAC2H, INVALID_METER_VALUE);
    meters.SetMeter(mPbAC2G, INVALID_METER_VALUE);
    CLongPoll1EMessage obj(gameAddress, &meters);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x1E);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll1FMessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll1FMessage obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x1F);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll27MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CMeters meters(MEMORY_TYPE::MEMORY_NONE, false);
    meters.SetMeter(gCI, INVALID_METER_VALUE);
    meters.SetMeter(gCO, INVALID_METER_VALUE);
    meters.SetMeter(mTotalDrop, INVALID_METER_VALUE);
    meters.SetMeter(mCD, INVALID_METER_VALUE);
    meters.SetMeter(gJP, INVALID_METER_VALUE);
    meters.SetMeter(gGS, INVALID_METER_VALUE);
    CLongPoll27Message obj(gameAddress, &meters);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x27);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll28MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll28Message obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetEFTSeed(), 0);

    ASSERT_EQ(obj.GetPollValue(), 0x28);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll2AMessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CMeters meters(MEMORY_TYPE::MEMORY_NONE, false);
    meters.SetMeter(gCI, INVALID_METER_VALUE);
    meters.SetMeter(gCO, INVALID_METER_VALUE);
    meters.SetMeter(mTotalDrop, INVALID_METER_VALUE);
    meters.SetMeter(mCD, INVALID_METER_VALUE);
    meters.SetMeter(gJP, INVALID_METER_VALUE);
    meters.SetMeter(gGS, INVALID_METER_VALUE);
    CLongPoll2AMessage obj(gameAddress, &meters);

    // Perform operation(s) and test(s).
    obj.m_trueCoinIn = 123;

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x2A);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll2BMessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CMeters meters(MEMORY_TYPE::MEMORY_NONE, false);
    meters.SetMeter(gCI, INVALID_METER_VALUE);
    meters.SetMeter(gCO, INVALID_METER_VALUE);
    meters.SetMeter(mTotalDrop, INVALID_METER_VALUE);
    meters.SetMeter(mCD, INVALID_METER_VALUE);
    meters.SetMeter(gJP, INVALID_METER_VALUE);
    meters.SetMeter(gGS, INVALID_METER_VALUE);
    CLongPoll2BMessage obj(gameAddress, &meters);

    // Perform operation(s) and test(s).
    obj.m_trueCoinOut = 123;

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x2B);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

/// <summary>
/// Code coverage and functional test for:
///     CSASPollMessage::IsGeneralPoll()
/// Test return value.
/// </summary>
TEST_F(SASPollMessageTests, IsGeneralPoll_LongPoll2DMessageTest)
{
   // Data.
   const BYTE gameAddress(1);
   const bool isGeneralPollExpected(false);

   // Setup.
   CLongPoll2DMessage obj(gameAddress, NULL);

   // Perform operation(s) and test(s).
   AssertResult2(obj.IsGeneralPoll(), isGeneralPollExpected);
}

TEST_F(SASPollMessageTests, LongPoll2EMessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll2EMessage obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.SetGameDelay(25);
    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x2E);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll32MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CMeters meters(MEMORY_TYPE::MEMORY_NONE, false);
    meters.SetMeter(gCI, INVALID_METER_VALUE);
    meters.SetMeter(gCO, INVALID_METER_VALUE);
    CLongPoll32Message obj(gameAddress, &meters);

    // Perform operation(s) and test(s).

    obj.m_two = 2;

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x32);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll39MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CMeters meters(MEMORY_TYPE::MEMORY_NONE, false);
    meters.SetMeter(gCI, INVALID_METER_VALUE);
    meters.SetMeter(gCO, INVALID_METER_VALUE);
    CLongPoll39Message obj(gameAddress, &meters);

    // Perform operation(s) and test(s).

    obj.m_thousand = 0;

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x39);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll3AMessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CMeters meters(MEMORY_TYPE::MEMORY_NONE, false);
    meters.SetMeter(gCI, INVALID_METER_VALUE);
    meters.SetMeter(gCO, INVALID_METER_VALUE);
    CLongPoll3AMessage obj(gameAddress, &meters);

    // Perform operation(s) and test(s).

    obj.m_twohundred = 1;

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x3A);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll3DMessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll3DMessage obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x3D);

    ASSERT_EQ(obj.SwapPollValue(), 0);

    ASSERT_EQ(obj.GetResponseTicket()->Number(), 0);
}

TEST_F(SASPollMessageTests, LongPoll46MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CMeters meters(MEMORY_TYPE::MEMORY_NONE, false);
    meters.SetMeter(gCI, INVALID_METER_VALUE);
    meters.SetMeter(gCO, INVALID_METER_VALUE);
    CLongPoll46Message obj(gameAddress, &meters);

    // Perform operation(s) and test(s).

    obj.m_billsAcceptedCredits = 0;

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x46);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll4DMessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll4DMessage obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.SetPeekOption(true);

    obj.SetDefaultExpiration(0);

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x4D);

    ASSERT_EQ(obj.SwapPollValue(), 0);

    ASSERT_EQ(obj.GetResponseTicket()->Number(), -1);
}

TEST_F(SASPollMessageTests, LongPoll50MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CMeters meters(MEMORY_TYPE::MEMORY_NONE, false);
    meters.SetMeter(gCI, INVALID_METER_VALUE);
    meters.SetMeter(gCO, INVALID_METER_VALUE);

    CLongPoll50Message obj(gameAddress, &meters);

    // Perform operation(s) and test(s).

    obj.m_currentCredits = 0;

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x50);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll51MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll51Message obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.m_numberSubGamesImplemented = true;

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x51);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll52MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CMeters meters(MEMORY_TYPE::MEMORY_NONE, false);
    meters.SetMeter(gCI, INVALID_METER_VALUE);
    meters.SetMeter(gCO, INVALID_METER_VALUE);

    CLongPoll52Message obj(gameAddress, &meters);

    // Perform operation(s) and test(s).

    obj.SetGameNumber(3);
    obj.m_coinIn = 0;
    obj.m_coinOut = 0;
    obj.m_jackpot = 0;
    obj.m_gamesPlayed = 0;

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x52);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll53MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll53Message obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.SetGameNumber(3);

    obj.m_gameID[0] = '1';
    obj.m_gameID[1] = '2';
    obj.m_denomination = 1;
    obj.m_maxBet = 25;
    obj.m_progressiveGroup = 0;
    obj.m_gameOptions[0] = 0;
    obj.m_gameOptions[1] = 0;

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x53);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll55MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll55Message obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.SetGameNumber(3);

    obj.m_selectedGameNumber = 0;

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x55);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll56MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll56Message obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.m_numberSubGamesImplemented = true;
    obj.m_enabledGames = 1;

    WORD enabledGamesList[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    obj.SetEnabledGamesList(&enabledGamesList[0]);

    obj.SetImplementedGames(1);

    obj.SetGameNumber(3);
    BYTE* responseBuffer = (BYTE*)malloc(256);
    memset(responseBuffer, 0, 256);
    obj.SetResponseBuffer(responseBuffer, 256);

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x56);

    ASSERT_EQ(obj.SwapPollValue(), 0);

    free(responseBuffer);
}

TEST_F(SASPollMessageTests, LongPoll6AMessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll6AMessage obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.CanEFTDownload(), false);
    ASSERT_EQ(obj.CanEFTUpload(), false);

    ASSERT_EQ(obj.GetPollValue(), 0x6A);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll70MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll70Message obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.m_responseTicket = new CTicket();

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_TRUE(obj.GetResponseTicket() != nullptr);

    ASSERT_EQ(obj.GetPollValue(), 0x70);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll72MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll72Message obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.ClearStatusQuery();

    obj.SetStatusQuery();

    obj.SetTransferCode(TRANSFER_REQUEST_CANCEL_REQUEST);

    ASSERT_EQ(obj.GetTransferCode(), TRANSFER_REQUEST_CANCEL_REQUEST);

    obj.SetTransferType(SAS_TRANSFER_TYPE_CASH_TO_HOST);

    ASSERT_EQ(obj.GetTransferType(), SAS_TRANSFER_TYPE_CASH_TO_HOST);

    obj.SetCashableAmount(123);

    ASSERT_EQ(obj.GetCashableAmount(), 123);

    obj.SetRestrictedAmount(456);

    ASSERT_EQ(obj.GetRestrictedAmount(), 456);

    obj.SetNonrestrictedAmount(789);

    ASSERT_EQ(obj.GetNonrestrictedAmount(), 789);

    obj.SetTransferFlag(TRANSFER_FLAG_ACCEPT_TRANSFER_ONLY_IF_LOCKED, true);

    obj.SetAssetNumber(246);

    ASSERT_EQ(obj.GetAssetNumber(), 246);

    BYTE transactionID[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    obj.SetTransactionID(&transactionID[0], 8);
    obj.SetReceiptData(&transactionID[0], 8);

    obj.SetExpiration(357);

    ASSERT_EQ(obj.GetExpiration(), 357);

    obj.SetPoolID(888);

    ASSERT_EQ(obj.GetPoolID(), 888);

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x72);

    ASSERT_FALSE(obj.GetTransferFlag(TRANSFER_FLAG_ACCEPT_TRANSFER_ONLY_IF_LOCKED));

    ASSERT_EQ(obj.SwapPollValue(), 0);

}

TEST_F(SASPollMessageTests, LongPoll74MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll74Message obj(gameAddress);

    obj.SetXferToGame(true);
    obj.SetXferFromGame(true);
    obj.SetXferToPrinter(true);
    obj.SetBonusToGame(true);
    obj.SetLockTimeout(100);
    obj.SetLockCode(LOCK_CODE_QUERY_STATUS);

    ASSERT_EQ(obj.GetLockCode(), LOCK_CODE_QUERY_STATUS);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_FALSE(obj.ResponseXferToGame());

    ASSERT_FALSE(obj.ResponseXferFromGame());

    ASSERT_FALSE(obj.ResponseXferToPrinter());

    ASSERT_FALSE(obj.ResponseWinAmountPendingCashoutToHost());

    ASSERT_FALSE(obj.ResponseBonusAwardToGamingMachine());

    ASSERT_FALSE(obj.ResponseCashoutToHostControlledByHost());

    ASSERT_FALSE(obj.ResponseCashoutToHostEnabled());

    ASSERT_FALSE(obj.ResponseCashoutToHostCurrentlyHard());

    ASSERT_FALSE(obj.ResponseAFTStatusPrinterAvailable());

    ASSERT_FALSE(obj.ResponseAFTStatusTransferToHostPartialAmounts());

    ASSERT_FALSE(obj.ResponseAFTStatusCustomTicketDataSupported());

    ASSERT_FALSE(obj.ResponseAFTStatusAFTRegistered());

    ASSERT_FALSE(obj.ResponseAFTStatusInHouseTransfersEnabled());

    ASSERT_FALSE(obj.ResponseAFTStatusBonusTransfersEnabled());

    ASSERT_FALSE(obj.ResponseAFTStatusDebitTransfersEnabled());

    ASSERT_FALSE(obj.ResponseAFTStatusAnyAFTEnabled());

    ASSERT_EQ(obj.GetPollValue(), 0x74);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll7FMessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll7FMessage obj(gameAddress);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x7F);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPoll83MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll83Message obj(gameAddress);

    obj.SetGameNumber(12);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x83);

    ASSERT_EQ(obj.SwapPollValue(), 0);

    ASSERT_EQ(obj.GetAmount(), 0);

}

TEST_F(SASPollMessageTests, LongPoll84MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll84Message obj(gameAddress);

    obj.SetGameNumber(12);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x84);

    ASSERT_EQ(obj.SwapPollValue(), 0);

}

TEST_F(SASPollMessageTests, LongPoll85MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll85Message obj(gameAddress);

    obj.SetGameNumber(12);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x85);

    ASSERT_EQ(obj.SwapPollValue(), 0);

}
// 
TEST_F(SASPollMessageTests, LongPoll94MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPoll94Message obj(gameAddress);

    obj.m_ACKCode = 0x01;

    ASSERT_EQ(obj.GetAckCode(), 1);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0x94);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPollA0MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPollA0Message obj(gameAddress);

    obj.SetGameNumber(12);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0xA0);

    ASSERT_EQ(obj.SwapPollValue(), 0);

}

TEST_F(SASPollMessageTests, LongPollA8MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPollA8Message obj(gameAddress);

    obj.SetGameNumber(12);
    obj.SetStandardHandpay();
    obj.SetCreditReset();
    obj.m_ACKCode = 0x01;

    // Perform operation(s) and test(s).

    ASSERT_EQ(obj.GetAckCode(), 1);

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0xA8);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPollB056MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPollB056Message obj(gameAddress);

    obj.SetGameNumber(12);
    obj.SetImplementedGames(1);
    obj.SetEnabledGamesList(nullptr);
    obj.SetDenom(1);

    // Perform operation(s) and test(s).

    BYTE* responseBuffer = (BYTE*)malloc(256);
    memset(responseBuffer, 0, 256);
    obj.SetResponseBuffer(responseBuffer, 256);

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0xB0);

    ASSERT_EQ(obj.SwapPollValue(), 0);

    free(responseBuffer);
}

TEST_F(SASPollMessageTests, LongPollB0B5MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPollB0B5Message obj(gameAddress, 0);

    obj.SetGameNumber(12);
    obj.SetDenom(1);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0xB0);

    ASSERT_EQ(obj.SwapPollValue(), 0);

    ASSERT_EQ(obj.GetMaxBet(), 0);

    ASSERT_EQ(obj.GetProgressiveGroup(), 0);

    ASSERT_EQ(obj.GetLevelIds(), 0);

    char* gameName = nullptr;
    WORD gameLength = obj.GetGameName(&gameName);
    ASSERT_EQ(gameLength, 0);

    char* paytableName = nullptr;
    WORD paytableLength = obj.GetPaytableName(&paytableName);
    ASSERT_EQ(paytableLength, 0);
}
//
TEST_F(SASPollMessageTests, LongPollB1MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPollB1Message obj(gameAddress);

    obj.m_currentDenom = 1;

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0xB1);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPollB2MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPollB2Message obj(gameAddress);

    BYTE* enabledDenomList = new byte[NUM_DENOM_CODE_MAPPINGS]; // enough space for 27 denoms?
    obj.SetEnabledDenomList(enabledDenomList, NUM_DENOM_CODE_MAPPINGS);

    // Perform operation(s) and test(s).

    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0xB2);

    ASSERT_EQ(obj.SwapPollValue(), 0);
}

TEST_F(SASPollMessageTests, LongPollB6MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.
    CLongPollB6Message obj(gameAddress);

    obj.SetAcknowledge();
    obj.SetUnable();
    obj.SetReady();

    // Perform operation(s) and test(s).

    BYTE* responseBuffer = (BYTE*)malloc(256);
    memset(responseBuffer, 0, 256);
    obj.SetResponseBuffer(responseBuffer, 256);
    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0xB6);

    ASSERT_EQ(obj.SwapPollValue(), 0);

    ASSERT_EQ(obj.GetStatus(), 0);
    free(responseBuffer);
}

TEST_F(SASPollMessageTests, LongPollB7MessageTest)
{
    // Data.
    const BYTE gameAddress(1);

    // Setup.

    CLongPollB7Message obj(gameAddress);

    obj.SetAssetNumber(123);

    ASSERT_EQ(obj.GetAssetNumber(), 123);

    // Perform operation(s) and test(s).

    BYTE* responseBuffer = (BYTE*)malloc(256);
    memset(responseBuffer, 0, 256);
    obj.SetResponseBuffer(responseBuffer, 256);
    obj.ResponseReceived();

    obj.Fill();

    ASSERT_EQ(obj.GetPollValue(), 0xB7);

    ASSERT_EQ(obj.SwapPollValue(), 0);

    ASSERT_EQ(obj.ResponseHostControlAssetNumber(), false);
    
    ASSERT_EQ(obj.ResponseHostControlFloorLocation(), false);
    
    free(responseBuffer);
}

/// <summary>
/// Code coverage and functional test for:
///     CSASPollMessage::IsNACK()
/// Test with a CSASGeneralPoll object, which has a maximum response length of 1.
/// </summary>
TEST_F(SASPollMessageTests, IsNack_GeneralPollTest)
{
   // Data
   const BYTE gameAddress(1);
   const BYTE nackValue(gameAddress | CSASPollMessage::GAME_ADDRESS_NACK_OR_VALUE);

   // Setup.
   CSASGeneralPoll obj(gameAddress);
   BYTE *bResponse(obj.GetResponseBuffer());
   ASSERT_TRUE(obj.GetResponseBufferSize() >= 1);


   // Perform operation(s) and test(s).
   *bResponse = nackValue;
   AssertResult2(obj.IsNACK(0), false);
   AssertResult2(obj.IsNACK(1), false);
}

/// <summary>
/// Code coverage and functional test for:
///     CSASPollMessage::IsNACK()
/// Test with a CLongPoll01Message object, which has a maximum response length of 1.
/// </summary>
TEST_F(SASPollMessageTests, IsNack_LongPoll01MessageTest)
{
   // Data
   const BYTE gameAddress(1);
   const BYTE nackValue(gameAddress | CSASPollMessage::GAME_ADDRESS_NACK_OR_VALUE);

   // Setup.
   CLongPoll01Message obj(gameAddress);
   BYTE *bResponse(obj.GetResponseBuffer());
   ASSERT_TRUE(obj.GetResponseBufferSize() >= 1);

   // Perform operation(s) and test(s).
   *bResponse = nackValue;
   AssertResult2(obj.IsNACK(0), false);
   AssertResult2(obj.IsNACK(1), true);

   *bResponse = (BYTE)(nackValue - 1);
   AssertResult2(obj.IsNACK(0), false);
   AssertResult2(obj.IsNACK(1), false);

   *bResponse = (BYTE)(nackValue + 1);
   AssertResult2(obj.IsNACK(0), false);
   AssertResult2(obj.IsNACK(1), false);

   *bResponse = gameAddress;
   AssertResult2(obj.IsNACK(0), false);
   AssertResult2(obj.IsNACK(1), false);
}

/// <summary>
/// Code coverage and functional test for:
///     CSASPollMessage::IsNACK()
/// Test with a CLongPoll2DMessage object, which has a maximum response length of 10.
/// </summary>
TEST_F(SASPollMessageTests, IsNack_LongPoll2DMessageTest)
{
   // Data
   const BYTE gameAddress(1);
   const BYTE nackValue(gameAddress | CSASPollMessage::GAME_ADDRESS_NACK_OR_VALUE);

   // Setup.
   CLongPoll2DMessage obj(gameAddress, NULL);
   BYTE *bResponse(obj.GetResponseBuffer());
   ASSERT_TRUE(obj.GetResponseBufferSize() >= 3);

   // Perform operation(s) and test(s).
   *bResponse = nackValue;
   *(bResponse + 1) = 0x2d;
   *(bResponse + 2) = 0x00;
   AssertResult2(obj.IsNACK(0), false);
   AssertResult2(obj.IsNACK(1), true);
   AssertResult2(obj.IsNACK(2), true);
   AssertResult2(obj.IsNACK(3), true);

   *bResponse = (BYTE)(nackValue - 1);
   AssertResult2(obj.IsNACK(0), false);
   AssertResult2(obj.IsNACK(1), false);
   AssertResult2(obj.IsNACK(2), false);
   AssertResult2(obj.IsNACK(3), false);

   *bResponse = (BYTE)(nackValue + 1);
   AssertResult2(obj.IsNACK(0), false);
   AssertResult2(obj.IsNACK(1), false);
   AssertResult2(obj.IsNACK(2), false);
   AssertResult2(obj.IsNACK(3), false);

   *bResponse = gameAddress;
   AssertResult2(obj.IsNACK(0), false);
   AssertResult2(obj.IsNACK(1), false);
   AssertResult2(obj.IsNACK(2), false);
   AssertResult2(obj.IsNACK(3), false);
}
}

