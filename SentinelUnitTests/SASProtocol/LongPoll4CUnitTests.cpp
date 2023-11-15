#include "stdafx.h"
#include "SASProtocol/LongPoll4CMessage.h"

#include "GlobalDefs.h"
#include "Utilities.h"


#include "gtest/gtest.h"

#define _I8_MIN     (-127 - 1)    // minimum signed 8 bit value
#define _I8_MAX       127         // maximum signed 8 bit value
#define _UI8_MAX      0xffu       // maximum unsigned 8 bit value

#define _I32_MIN    (-2147483647 - 1) // minimum signed 32 bit value
#define _I32_MAX      2147483647 // maximum signed 32 bit value

using namespace std;

/// Code coverage and functional test for:
///     CLongPoll4CMessage::CLongPoll4CMessage()
/// Code coverage:
///     CLongPoll4CMessage::~CLongPoll4CMessage()
/// Test for correct default values.
/// </summary>
TEST(LongPoll4CMessage, CLongPoll4CMessageTest)
{
    typedef struct
    {
        // Inputs.
        const byte m_gameAddress;

        // Expected results.
    } TestDataType;

    const TestDataType TEST_DATA[] =
    {
        { (byte)-1 },
        { 0 },
        { 1 },
        { 2 },
        { (byte)(_I8_MAX - 1) },
        { (byte)_I8_MAX },
        { (byte)_I8_MIN },
        { (byte)(_I8_MIN + 1) },
        { (byte)(_UI8_MAX - 1) },
        { (byte)_UI8_MAX },
    };

    // Expected results -- same for all loop iterations.
    const BYTE pollValueExpected(0x4C);
    const bool swapPollValueExpected(false);
    const UINT responseBufferSizeExpected(10);
    const BYTE responseBufferExpected[responseBufferSizeExpected] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    const UINT sendBufferSizeExpected(10);
    const BYTE sendBufferExpected[responseBufferSizeExpected] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    const DWORD machineValidationIDExpected(0);
    const DWORD validationSequenceNumberExpected(0);

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup.

        // Perform operation(s) to be tested.
        CLongPoll4CMessage varMsg(TEST_DATA[testIndex].m_gameAddress);
        const CLongPoll4CMessage &msg(varMsg);

        // Test #1.
        {
            const BYTE pollValueResult(varMsg.GetPollValue());
            const bool success(pollValueResult == pollValueExpected);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("pollValueResult=%u expected=%u testIndex=%d in %s(%d)",
                    (unsigned)pollValueResult,
                    (unsigned)pollValueExpected,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Test #2.
        {
            const bool swapPollValueResult(varMsg.SwapPollValue());
            const bool success(swapPollValueResult == swapPollValueExpected);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("swapPollValueResult=%d expected=%d testIndex=%d in %s(%d)",
                    swapPollValueResult,
                    swapPollValueExpected,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Test #3.
        const BYTE * const responseBufferResult(varMsg.GetResponseBuffer());

        {
            const bool success(responseBufferResult != NULL);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("responseBufferResult=%p expected=NOT NULL testIndex=%d in %s(%d)",
                    (void *)responseBufferResult,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Test #4.
        const UINT responseBufferSizeResult(varMsg.GetResponseBufferSize());

        {
            const bool success(responseBufferSizeResult == responseBufferSizeExpected);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("responseBufferSizeResult=%u expected=%u testIndex=%d in %s(%d)",
                    (unsigned)responseBufferSizeResult,
                    (unsigned)responseBufferSizeExpected,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Test #5.
        {
            const int memcmpResult = memcmp(responseBufferResult, responseBufferExpected, responseBufferSizeExpected);
            const bool success(memcmpResult == MEMCMP_EQUAL);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("responseBufferResult={%s} expected={%s} testIndex=%d in %s(%d)",
                    (CUtilities::ToHexString(responseBufferResult, responseBufferSizeResult)).c_str(),
                    (CUtilities::ToHexString(responseBufferExpected, responseBufferSizeExpected)).c_str(),
                    (int)testIndex,
                    static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Test #6.
        const BYTE * const sendBufferResult(varMsg.GetSendBuffer());

        {
            const bool success(sendBufferResult != NULL);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("sendBufferResult=%p expected=NOT NULL testIndex=%d in %s(%d)",
                    (void *)sendBufferResult,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Test #7.
        const UINT sendBufferSizeResult(varMsg.GetSendBufferSize());

        {
            const bool success(sendBufferSizeResult == sendBufferSizeExpected);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("sendBufferSizeResult=%u expected=%u testIndex=%d in %s(%d)",
                    (unsigned)sendBufferSizeResult,
                    (unsigned)sendBufferSizeExpected,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Test #8.
        {
            const int memcmpResult = memcmp(sendBufferResult, sendBufferExpected, sendBufferSizeExpected);
            const bool success(memcmpResult == MEMCMP_EQUAL);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("sendBufferResult={%s} expected={%s} testIndex=%d in %s(%d)",
                    (CUtilities::ToHexString(sendBufferResult, sendBufferSizeResult)).c_str(),
                    (CUtilities::ToHexString(sendBufferExpected, sendBufferSizeExpected)).c_str(),
                    testIndex,
                    (__TFUNCTION__), (int)__LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Test #9.
        {
            const DWORD machineValidationIDResult = msg.GetMachineValidationID();
            const bool success(machineValidationIDResult == machineValidationIDExpected);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("machineValidationIDResult=%u expected=%u testIndex=%d in %s(%d)",
                    (unsigned)machineValidationIDResult,
                    (unsigned)machineValidationIDExpected,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Test #10.
        {
            const DWORD validationSequenceNumberResult = msg.GetValidationSequenceNumber();
            const bool success(validationSequenceNumberResult == validationSequenceNumberExpected);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("validationSequenceNumberResult=%u expected=%u testIndex=%d in %s(%d)",
                    (unsigned)validationSequenceNumberResult,
                    (unsigned)validationSequenceNumberExpected,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Cleanup.
    }
}

/// <summary>
/// Code coverage and functional test for:
///     CLongPoll4CMessage::CLongPoll4CMessage()
///     CLongPoll4CMessage::Fill()
/// Test for correct default values.
/// </summary>
TEST(LongPoll4CMessage, Fill_DefaultTest)
{
    // Expected results -- same for all loop iterations.
    const UINT sendBufferSizeExpected(10);

    // Expected results -- per iteration.
    const BYTE sendBuffer0[sendBufferSizeExpected] = { 0xff, 0x4c,  0, 0, 0, 0, 0, 0, 0xb5, 0x84 };
    const BYTE sendBuffer1[sendBufferSizeExpected] = { 0x00, 0x4c, 0, 0, 0, 0, 0, 0, 0x9c, 0x76 };
    const BYTE sendBuffer2[sendBufferSizeExpected] = { 0x01, 0x4c, 0, 0, 0, 0, 0, 0, 0x23, 0xf7 };
    const BYTE sendBuffer3[sendBufferSizeExpected] = { 0x02, 0x4c, 0, 0, 0, 0, 0, 0, 0xf3, 0x7d };
    const BYTE sendBuffer4[sendBufferSizeExpected] = { 0x7e, 0x4c, 0, 0, 0, 0, 0, 0, 0xe8, 0xce };
    const BYTE sendBuffer5[sendBufferSizeExpected] = { 0x7f, 0x4c, 0, 0, 0, 0, 0, 0, 0x57, 0x4f };
    const BYTE sendBuffer6[sendBufferSizeExpected] = { 0x80, 0x4c, 0, 0, 0, 0, 0, 0, 0x7e, 0xbd };
    const BYTE sendBuffer7[sendBufferSizeExpected] = { 0x81, 0x4c, 0, 0, 0, 0, 0, 0, 0xc1, 0x3c };
    const BYTE sendBuffer8[sendBufferSizeExpected] = { 0xfe, 0x4c, 0, 0, 0, 0, 0, 0, 0x0a, 0x05 };
    const BYTE sendBuffer9[sendBufferSizeExpected] = { 0xff, 0x4c, 0, 0, 0, 0, 0, 0, 0xb5, 0x84 };

    typedef struct
    {
        // Inputs.
        const byte m_gameAddress;

        // Expected results.
        const BYTE * const m_sendBufferExpected;
    } TestDataType;

    const TestDataType TEST_DATA[] =
    {
        { (byte)-1, sendBuffer0 },
        { 0, sendBuffer1 },
        { 1, sendBuffer2 },
        { 2, sendBuffer3 },
        { (byte)(_I8_MAX - 1), sendBuffer4 },
        { (byte)_I8_MAX, sendBuffer5 },
        { (byte)_I8_MIN, sendBuffer6 },
        { (byte)(_I8_MIN + 1), sendBuffer7 },
        { (byte)(_UI8_MAX - 1), sendBuffer8 },
        { (byte)_UI8_MAX, sendBuffer9 },
    };

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup.

        // Perform operation(s) to be tested.
        CLongPoll4CMessage varMsg(TEST_DATA[testIndex].m_gameAddress);
        varMsg.Fill();
        const CLongPoll4CMessage &msg(varMsg);

        // Test #1.
        const BYTE * const sendBufferResult(varMsg.GetSendBuffer());

        {
            const bool success(sendBufferResult != NULL);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("sendBufferResult=%p expected=NOT NULL testIndex=%d in %s(%d)",
                    (void *)sendBufferResult,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Test #2.
        const UINT sendBufferSizeResult(varMsg.GetSendBufferSize());

        {
            const bool success(sendBufferSizeResult == sendBufferSizeExpected);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("sendBufferSizeResult=%u expected=%u testIndex=%d in %s(%d)",
                    (unsigned)sendBufferSizeResult,
                    (unsigned)sendBufferSizeExpected,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Test #3.
        {
            const int memcmpResult = memcmp(sendBufferResult, TEST_DATA[testIndex].m_sendBufferExpected, sendBufferSizeExpected);
            const bool success(memcmpResult == MEMCMP_EQUAL);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("sendBufferResult={%s} expected={%s} testIndex=%d in %s(%d)",
                    (CUtilities::ToHexString(sendBufferResult, sendBufferSizeResult)).c_str(),
                    (CUtilities::ToHexString(TEST_DATA[testIndex].m_sendBufferExpected, sendBufferSizeExpected)).c_str(),
                    testIndex,
                    (__TFUNCTION__), (int)__LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Cleanup.
    }
}

/// <summary>
/// Code coverage and functional test for:
///     CLongPoll4CMessage::SetMachineValidationID()
///     CLongPoll4CMessage::GetMachineValidationID()
/// </summary>
TEST(LongPoll4CMessage, SetMachineValidationIDTest)
{
    // Inputs -- same for all loop iterations.
    const byte gameAddress(0);

    typedef struct
    {
        // Inputs.
        const DWORD m_machineValidationID;

        // Expected results.
        const DWORD m_machineValidationIDExpected;
    } TestDataType;

    const TestDataType TEST_DATA[] =
    {
        { (DWORD)-1, 0xffffff },
        { 0, 0 },
        { 1, 1 },
        { 2, 2 },
        { 0xfffffe, 0xfffffe },
        { 0xffffff, 0xffffff },
        { (DWORD)(_I32_MAX - 1), 0xfffffe },
        { (DWORD)_I32_MAX, 0xffffff },
        { (DWORD)_I32_MIN, 0 },
        { (DWORD)(_I32_MIN + 1), 1 },
        { (DWORD_MAX - 1), 0xfffffe },
        { DWORD_MAX, 0xffffff },
    };

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup.
        CLongPoll4CMessage varMsg(gameAddress);

        // Perform operation(s) to be tested.
        varMsg.SetMachineValidationID(TEST_DATA[testIndex].m_machineValidationID);
        const CLongPoll4CMessage &msg(varMsg);

        // Test #1.
        {
            const DWORD machineValidationIDResult = msg.GetMachineValidationID();
            const bool success(machineValidationIDResult == TEST_DATA[testIndex].m_machineValidationIDExpected);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("machineValidationIDResult=%u expected=%u testIndex=%d in %s(%d)",
                    (unsigned)machineValidationIDResult,
                    (unsigned)TEST_DATA[testIndex].m_machineValidationIDExpected,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Cleanup.
    }
}

/// <summary>
/// Code coverage and functional test for:
///     CLongPoll4CMessage::SetValidationSequenceNumber()
///     CLongPoll4CMessage::GetValidationSequenceNumber()
/// </summary>
TEST(LongPoll4CMessage, SetValidationSequenceNumberTest)
{
    // Inputs -- same for all loop iterations.
    const byte gameAddress(0);

    typedef struct
    {
        // Inputs.
        const DWORD m_validationSequenceNumber;

        // Expected results.
        const DWORD m_validationSequenceNumberExpected;
    } TestDataType;

    const TestDataType TEST_DATA[] =
    {
        { (DWORD)-1, 0xffffff },
        { 0, 0 },
        { 1, 1 },
        { 2, 2 },
        { 0xfffffe, 0xfffffe },
        { 0xffffff, 0xffffff },
        { (DWORD)(_I32_MAX - 1), 0xfffffe },
        { (DWORD)_I32_MAX, 0xffffff },
        { (DWORD)_I32_MIN, 0 },
        { (DWORD)(_I32_MIN + 1), 1 },
        { (DWORD_MAX - 1), 0xfffffe },
        { DWORD_MAX, 0xffffff },
    };

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup.
        CLongPoll4CMessage varMsg(gameAddress);

        // Perform operation(s) to be tested.
        varMsg.SetValidationSequenceNumber(TEST_DATA[testIndex].m_validationSequenceNumber);
        const CLongPoll4CMessage &msg(varMsg);

        // Test #1.
        {
            const DWORD validationSequenceNumberResult = msg.GetValidationSequenceNumber();
            const bool success(validationSequenceNumberResult == TEST_DATA[testIndex].m_validationSequenceNumberExpected);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("validationSequenceNumberResult=%u expected=%u testIndex=%d in %s(%d)",
                    (unsigned)validationSequenceNumberResult,
                    (unsigned)TEST_DATA[testIndex].m_validationSequenceNumberExpected,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Cleanup.
    }
}

/// <summary>
/// Code coverage and functional test for:
///     CLongPoll4CMessage::SetMachineValidationID()
///     CLongPoll4CMessage::SetValidationSequenceNumber()
///     CLongPoll4CMessage::Fill()
/// </summary>
TEST(LongPoll4CMessage, FillTest)
{
    // Expected results -- same for all loop iterations.
    const UINT sendBufferSizeExpected(10);

    // Expected results -- per iteration.
    const BYTE sendBuffer0[sendBufferSizeExpected] = { 0xff, 0x4c,  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x69, 0xde };
    const BYTE sendBuffer1[sendBufferSizeExpected] = { 0x00, 0x4c, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0f, 0xc7 };
    const BYTE sendBuffer2[sendBufferSizeExpected] = { 0x01, 0x4c, 0x02, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xe2, 0x93 };
    const BYTE sendBuffer3[sendBufferSizeExpected] = { 0x02, 0x4c, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x31, 0x82 };
    const BYTE sendBuffer4[sendBufferSizeExpected] = { 0x03, 0x4c, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0x79, 0x5d };
    const BYTE sendBuffer5[sendBufferSizeExpected] = { 0x04, 0x4c, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x9f };
    const BYTE sendBuffer6[sendBufferSizeExpected] = { 0x7e, 0x4c, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x4a, 0x03 };
    const BYTE sendBuffer7[sendBufferSizeExpected] = { 0x7f, 0x4c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x8b, 0x15 };
    const BYTE sendBuffer8[sendBufferSizeExpected] = { 0x80, 0x4c, 0x01, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xc2, 0xd5 };
    const BYTE sendBuffer9[sendBufferSizeExpected] = { 0x81, 0x4c, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xc3 };
    const BYTE sendBuffer10[sendBufferSizeExpected] = { 0xfe, 0x4c, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xfe };
    const BYTE sendBuffer11[sendBufferSizeExpected] = { 0xff, 0x4c, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x17, 0x49 };

    typedef struct
    {
        // Inputs.
        const byte m_gameAddress;
        const DWORD m_machineValidationID;
        const DWORD m_validationSequenceNumber;

        // Expected results.
        const BYTE * const m_sendBufferExpected;
    } TestDataType;

    const TestDataType TEST_DATA[] =
    {
        { (byte)-1, 0, 1, sendBuffer0 },
        { 0, 1, 2, sendBuffer1 },
        { 1, 2, 0xfffffe, sendBuffer2 },
        { 2, 0xfffffe, 0xffffff, sendBuffer3 },
        { 3, 0xffffff, (DWORD)(_I32_MAX - 1), sendBuffer4 },
        { 4, (DWORD)(_I32_MAX - 1), (DWORD)_I32_MAX, sendBuffer5 },
        { (byte)(_I8_MAX - 1), (DWORD)_I32_MAX, (DWORD)_I32_MIN, sendBuffer6 },
        { (byte)_I8_MAX, (DWORD)_I32_MIN, (DWORD)(_I32_MIN + 1), sendBuffer7 },
        { (byte)_I8_MIN, (DWORD)(_I32_MIN + 1), (DWORD_MAX - 1), sendBuffer8 },
        { (byte)(_I8_MIN + 1), (DWORD_MAX - 1), DWORD_MAX, sendBuffer9 },
        { (byte)(_UI8_MAX - 1), DWORD_MAX, (DWORD)-1, sendBuffer10 },
        { (byte)_UI8_MAX, (DWORD)-1, 0, sendBuffer11 },
    };

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup.
        CLongPoll4CMessage varMsg(TEST_DATA[testIndex].m_gameAddress);

        // Perform operation(s) to be tested.
        varMsg.SetMachineValidationID(TEST_DATA[testIndex].m_machineValidationID);
        varMsg.SetValidationSequenceNumber(TEST_DATA[testIndex].m_validationSequenceNumber);
        varMsg.Fill();
        const CLongPoll4CMessage &msg(varMsg);

        // Test #1.
        const BYTE * const sendBufferResult(varMsg.GetSendBuffer());

        {
            const bool success(sendBufferResult != NULL);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("sendBufferResult=%p expected=NOT NULL testIndex=%d in %s(%d)",
                    (void *)sendBufferResult,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Test #2.
        const UINT sendBufferSizeResult(varMsg.GetSendBufferSize());

        {
            const bool success(sendBufferSizeResult == sendBufferSizeExpected);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("sendBufferSizeResult=%u expected=%u testIndex=%d in %s(%d)",
                    (unsigned)sendBufferSizeResult,
                    (unsigned)sendBufferSizeExpected,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Test #3.
        {
            const int memcmpResult = memcmp(sendBufferResult, TEST_DATA[testIndex].m_sendBufferExpected, sendBufferSizeExpected);
            const bool success(memcmpResult == MEMCMP_EQUAL);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("sendBufferResult={%s} expected={%s} testIndex=%d in %s(%d)",
                    (CUtilities::ToHexString(sendBufferResult, sendBufferSizeResult)).c_str(),
                    (CUtilities::ToHexString(TEST_DATA[testIndex].m_sendBufferExpected, sendBufferSizeExpected)).c_str(),
                    testIndex,
                    (__TFUNCTION__), (int)__LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Cleanup.
    }
}


/// <summary>
/// Code coverage and functional test for:
/// CSASPollMessage::GetResponseEventHandle()
/// CLongPoll4CMessage::ResponseReceived()
/// CLongPoll4CMessage::GetMachineValidationID()
/// CLongPoll4CMessage::GetValidationSequenceNumber()
/// </summary>
TEST(LongPoll4CMessage, ResponseReceivedTest)
{
    // Inputs -- same for all loop iterations.
    const byte gameAddress(0);

    // Expected results -- same for all loop iterations.
    const UINT responseBufferSizeExpected(10);

    // Expected results -- per iteration.
    const BYTE responseBuffer0[responseBufferSizeExpected] = { 0xff, 0x4c,  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x69, 0xde };
    const BYTE responseBuffer1[responseBufferSizeExpected] = { 0x00, 0x4c, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0f, 0xc7 };
    const BYTE responseBuffer2[responseBufferSizeExpected] = { 0x01, 0x4c, 0x02, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xe2, 0x93 };
    const BYTE responseBuffer3[responseBufferSizeExpected] = { 0x02, 0x4c, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x31, 0x82 };
    const BYTE responseBuffer4[responseBufferSizeExpected] = { 0x03, 0x4c, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0x79, 0x5d };
    const BYTE responseBuffer5[responseBufferSizeExpected] = { 0x04, 0x4c, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x9f };
    const BYTE responseBuffer6[responseBufferSizeExpected] = { 0x7e, 0x4c, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x4a, 0x03 };
    const BYTE responseBuffer7[responseBufferSizeExpected] = { 0x7f, 0x4c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x8b, 0x15 };
    const BYTE responseBuffer8[responseBufferSizeExpected] = { 0x80, 0x4c, 0x01, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xc2, 0xd5 };
    const BYTE responseBuffer9[responseBufferSizeExpected] = { 0x81, 0x4c, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xc3 };
    const BYTE responseBuffer10[responseBufferSizeExpected] = { 0xfe, 0x4c, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xfe };
    const BYTE responseBuffer11[responseBufferSizeExpected] = { 0xff, 0x4c, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x17, 0x49 };

    typedef struct
    {
        // Inputs.
        const BYTE * const m_responseBuffer;

        // Expected results.
        const DWORD m_machineValidationIDExpected;
        const DWORD m_validationSequenceNumberExpected;
    } TestDataType;

    const TestDataType TEST_DATA[] =
    {
        { responseBuffer0, 0, 1 },
        { responseBuffer1, 1, 2 },
        { responseBuffer2, 2, 0xfffffe },
        { responseBuffer3, 0xfffffe, 0xffffff },
        { responseBuffer4, 0xffffff, 0xfffffe },
        { responseBuffer5, 0xfffffe, 0xffffff },
        { responseBuffer6, 0xffffff, 0x000000 },
        { responseBuffer7, 0x000000, 0x000001 },
        { responseBuffer8, 0x000001, 0xfffffe },
        { responseBuffer9, 0xfffffe, 0xffffff },
        { responseBuffer10, 0xffffff, 0xffffff },
        { responseBuffer11, 0xffffff, 0 },
    };

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup.
        CLongPoll4CMessage varMsg(gameAddress);
        const CLongPoll4CMessage &msg(varMsg);
        BYTE * const responseBuffer(varMsg.GetResponseBuffer());
        const UINT responseBufferSize(varMsg.GetResponseBufferSize());

        // Verify setup.
        {
            ASSERT_TRUE(responseBuffer != nullptr);
            ASSERT_TRUE(responseBufferSize == responseBufferSizeExpected);
        }

        // Perform operation(s) to be tested.
        memcpy(responseBuffer, TEST_DATA[testIndex].m_responseBuffer, responseBufferSize);
        varMsg.ResponseReceived();

        // Test #1.
        {
            const DWORD machineValidationIDResult = msg.GetMachineValidationID();
            const bool success(machineValidationIDResult == TEST_DATA[testIndex].m_machineValidationIDExpected);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("machineValidationIDResult=%u expected=%u testIndex=%d in %s(%d)",
                    (unsigned)machineValidationIDResult,
                    (unsigned)TEST_DATA[testIndex].m_machineValidationIDExpected,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Test #2.
        {
            const DWORD validationSequenceNumberResult = msg.GetValidationSequenceNumber();
            const bool success(validationSequenceNumberResult == TEST_DATA[testIndex].m_validationSequenceNumberExpected);
            if (!success)
            {
                string failedMsg;
                failedMsg = FormatString("validationSequenceNumberResult=%u expected=%u testIndex=%d in %s(%d)",
                    (unsigned)validationSequenceNumberResult,
                    (unsigned)TEST_DATA[testIndex].m_validationSequenceNumberExpected,
                    testIndex,
                    (__TFUNCTION__), __LINE__);
                ASSERT_TRUE(success) <<  failedMsg;
            }
        }

        // Cleanup.
    }
}
