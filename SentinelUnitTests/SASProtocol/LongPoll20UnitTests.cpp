#include "stdafx.h"
#include "SASProtocol/LongPoll20Message.h"

// Unit testing includes.
#include "UnitTestingUtils.h"

#include "gtest/gtest.h"
using namespace std;

namespace SentinelUnitTests
{
		static const int m_maxMetersExpected = 1;
		static const BYTE m_pollValueExpected = 0x20;
		static const __int64 m_totalBillsExpected = -1;

		static const UINT m_sendBufferSizeExpected = 2;
		static const UINT m_responseBufferSizeExpected = 8;
		static const bool m_swapPollValueExpected = false;
		static const bool m_isGeneralPollExpected = false;
		static const int m_returnValueDefaultExpected = SASPOLL_SUCCESS;
		static const size_t m_meterValueOffsetExpected = 2;
		static const size_t m_meterValueSizeExpected = 4;

		static const BYTE m_nackOrValue = 0x80;
        class LongPoll20Message : public ::testing::Test
        {
        public: 
            BYTE GetPOLL_VALUE(CLongPoll20Message& obj)
            {
                return obj.POLL_VALUE;
            }
            UINT GetPOLL_SIZE(CLongPoll20Message& obj)
            {
                return obj.POLL_SIZE;
            }
            UINT GetRESPONSE_SIZE(CLongPoll20Message& obj)
            {
                return obj.RESPONSE_SIZE;
            }
            UINT GetMETER_VALUE_OFFSET(CLongPoll20Message& obj)
            {
                return obj.METER_VALUE_OFFSET;
            }
            UINT GetMETER_VALUE_SIZE(CLongPoll20Message& obj)
            {
                return obj.METER_VALUE_SIZE;
            }
            __int64 GetTotalBills(CLongPoll20Message& obj)
            {
                return obj.m_totalBills;
            }
            CMeters* GetMeters(CLongPoll20Message& obj)
            {
                return obj.m_meters;
            }
            WORD GetdenomMult(CLongPoll20Message& obj)
            {
                return obj.m_denomMult;
            }

        protected:
            virtual void SetUp() 
            {
            }

            virtual void TearDown() 
            {
            }
                
        };
// #pragma region CLongPoll20Message
		/// <summary>
		/// Functional test for static class and/or class-related constants.
		/// </summary>
		TEST_F(LongPoll20Message, CLongPoll20Message_StaticTest)
		{
            const byte gameAddress = 1;
            CLongPoll20Message obj(gameAddress);
			ASSERT_EQ(m_maxMetersExpected, (int)X20_MAX_METERS) << "X20_MAX_METERS";
			ASSERT_EQ(m_pollValueExpected, GetPOLL_VALUE(obj)) << "POLL_VALUE";
			ASSERT_EQ(m_sendBufferSizeExpected, GetPOLL_SIZE(obj)) << "POLL_SIZE";
			ASSERT_EQ(m_responseBufferSizeExpected, GetRESPONSE_SIZE(obj)) << "RESPONSE_SIZE";
			ASSERT_EQ(m_meterValueOffsetExpected, GetMETER_VALUE_OFFSET(obj)) << "METER_VALUE_OFFSET";
			ASSERT_EQ(m_meterValueSizeExpected, GetMETER_VALUE_SIZE(obj)) << "METER_VALUE_SIZE";
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll20Message::CLongPoll20Message()
		/// Code coverage for:
		///     CLongPoll20Message::~CLongPoll20Message()
		/// Test for correct default and/or initialized values.
		/// </summary>
		TEST_F(LongPoll20Message, CLongPoll20Message_Test)
		{
			// Data.
			const byte gameAddress = 1;
			const WORD denomMult = 25;

			// Setup.
			CMeters meters(MEMORY_NONE);
            //meters.
			meters.SetMeter(mBD);

			// Perform operation(s) and test(s).
			CLongPoll20Message msg(gameAddress, &meters, denomMult);

			ASSERT_EQ(m_sendBufferSizeExpected, msg.GetSendBufferSize()) << "GetSendBufferSize";
			ASSERT_TRUE(msg.GetSendBuffer() != NULL) << "GetSendBuffer";
			ASSERT_EQ(m_responseBufferSizeExpected, msg.GetResponseBufferSize()) << "GetResponseBufferSize";
			ASSERT_TRUE(msg.GetResponseBuffer() != NULL) << "GetResponseBuffer";
			ASSERT_EQ(m_pollValueExpected, msg.GetPollValue()) << "GetPollValue";
			ASSERT_EQ(m_swapPollValueExpected, msg.SwapPollValue()) << "SwapPollValue";
			ASSERT_EQ(m_returnValueDefaultExpected, msg.GetReturnValue()) << "GetReturnValue";
			ASSERT_EQ(m_totalBillsExpected, GetTotalBills(msg)) << "m_totalBills";
			ASSERT_EQ(m_totalBillsExpected, msg.GetTotalBills()) << "m_totalBills";
			ASSERT_EQ((void *)&meters, GetMeters(msg)) << "m_meters";
			ASSERT_EQ(denomMult, GetdenomMult(msg)) << "m_denomMult";
		}
// #pragma endregion CLongPoll20Message

// #pragma region Fill
		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll20Message::Fill()
		///     CLongPoll20Message::GetSendBuffer()
		///     CLongPoll20Message::GetSendBufferSize()
		///     CLongPoll20Message::GetResponseBufferSize()
		/// Test that the send buffer is set correctly for sending the poll to the EGM and verify the expected response size.
		/// </summary>
		TEST_F(LongPoll20Message, CLongPoll20Message_FillTest)
		{
			// Data.
			const byte gameAddress = 1;
			const WORD denomMult = 25;

			const BYTE sendBufferExpected[] = { 0x01, 0x20 };
			ASSERT_EQ(m_sendBufferSizeExpected, sizeof(sendBufferExpected)) << "sizeof(sendBufferExpected)";

			// Setup.
			CMeters meters(MEMORY_NONE);
			meters.SetMeter(mBD);

			// Perform operation(s) and test(s).
			CLongPoll20Message msg(gameAddress, &meters, denomMult);
			msg.Fill();

			ASSERT_EQ(m_sendBufferSizeExpected, msg.GetSendBufferSize()) << "GetSendBufferSize";
			ASSERT_TRUE(msg.GetSendBuffer() != NULL) << "GetSendBuffer";
			ASSERT_EQ(0, memcmp(msg.GetSendBuffer(), sendBufferExpected, sizeof(sendBufferExpected))) << "memcmp msg.GetSendBuffer()";

			ASSERT_EQ(m_responseBufferSizeExpected, msg.GetResponseBufferSize()) << "GetResponseBufferSize";
			ASSERT_TRUE(msg.GetResponseBuffer() != NULL) << "GetResponseBuffer";
		}
// #pragma endregion Fill

// #pragma region ResponseReceived
		/// <summary>
		/// Functional test for:
		///     CLongPoll20Message::CLongPoll20Message()
		///     CLongPoll20Message::ResponseReceived()
		///     CLongPoll20Message::GetReturnValue()
		///     CLongPoll20Message::GetTotalBills()
		/// Test a response with SASPOLL_INCOMPLETE_DATA for the return value.
		/// </summary>
		TEST_F(LongPoll20Message, CLongPoll20Message_ResponseReceived_IncompleteDataTest)
		{
			// Data.
			const byte gameAddress = 1;
			const WORD denomMult = 10;
			const byte meterNumber(mBD);
			const int returnValueExpected(SASPOLL_INCOMPLETE_DATA);
			const __int64 totalBillsExpected(-1);
			const __int64 meterValueExpected(-1);

			BYTE responseBuffer[] = { gameAddress, m_pollValueExpected, 0x12, 0x34, 0x56, 0x78, 0xcc, 0xcc };
			CUnitTestingUtils::SetSasCrc(responseBuffer, sizeof(responseBuffer));
			ASSERT_EQ(m_responseBufferSizeExpected, sizeof(responseBuffer)) << "sizeof(responseBuffer)";

			// Setup.
			CMeters meters(MEMORY_NONE);
			meters.SetMeter(mBD);
			ASSERT_TRUE(meters.GetMeterByNumber(meterNumber) != NULL) << "GetMeterByNumber";

			// Perform operation(s) and test(s).
			CLongPoll20Message msg(gameAddress, &meters, denomMult);
			msg.Fill();
			memcpy(msg.GetResponseBuffer(), responseBuffer, msg.GetResponseBufferSize());
			msg.SetReturnValue(SASPOLL_INCOMPLETE_DATA);
			msg.ResponseReceived();

			ASSERT_EQ(returnValueExpected, msg.GetReturnValue()) << "GetReturnValue";
			ASSERT_EQ(totalBillsExpected, msg.GetTotalBills()) << "GetTotalBills";
			ASSERT_EQ(meterValueExpected, meters.GetMeterByNumber(meterNumber)->GetValue()) << "GetValue";
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll20Message::CLongPoll20Message()
		///     CLongPoll20Message::ResponseReceived()
		///     CLongPoll20Message::GetReturnValue()
		///     CLongPoll20Message::GetTotalBills()
		/// Test a response with SASPOLL_NACK for the return value.
		/// </summary>
		TEST_F(LongPoll20Message, CLongPoll20Message_ResponseReceived_NackTest)
		{
			// Data.
			const byte gameAddress = 1;
			const WORD denomMult = 10;
			const byte meterNumber(mBD);
			const int returnValueExpected(SASPOLL_NACK);
			const __int64 totalBillsExpected(-1);
			const __int64 meterValueExpected(-1);

			BYTE responseBuffer[] = { gameAddress, m_pollValueExpected, 0x12, 0x34, 0x56, 0x78, 0xcc, 0xcc };
			CUnitTestingUtils::SetSasCrc(responseBuffer, sizeof(responseBuffer));
			ASSERT_EQ(m_responseBufferSizeExpected, sizeof(responseBuffer)) << "sizeof(responseBuffer)";

			// Setup.
			CMeters meters(MEMORY_NONE);
			meters.SetMeter(mBD);
			ASSERT_TRUE(meters.GetMeterByNumber(meterNumber) != NULL) << "GetMeterByNumber";

			// Perform operation(s) and test(s).
			CLongPoll20Message msg(gameAddress, &meters, denomMult);
			msg.Fill();
			memcpy(msg.GetResponseBuffer(), responseBuffer, msg.GetResponseBufferSize());
			msg.SetReturnValue(SASPOLL_NACK);
			msg.ResponseReceived();

			ASSERT_EQ(returnValueExpected, msg.GetReturnValue()) << "GetReturnValue";
			ASSERT_EQ(totalBillsExpected, msg.GetTotalBills()) << "GetTotalBills";
			ASSERT_EQ(meterValueExpected, meters.GetMeterByNumber(meterNumber)->GetValue()) << "GetValue";
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll20Message::CLongPoll20Message()
		///     CLongPoll20Message::ResponseReceived()
		///     CLongPoll20Message::GetReturnValue()
		///     CLongPoll20Message::GetTotalBills()
		/// Test a response with a zero length response.
		/// </summary>
		TEST_F(LongPoll20Message, CLongPoll20Message_ResponseReceived_ZeroResponseLengthTest)
		{
			// Data.
			const byte gameAddress = 1;
			const WORD denomMult = 10;
			const byte meterNumber(mBD);
			const int returnValueExpected(SASPOLL_NACK);
			const __int64 totalBillsExpected(-1);
			const __int64 meterValueExpected(-1);

			// Omit last CRC byte from response buffer.
			BYTE responseBuffer[] = { (BYTE)(gameAddress + 1), m_pollValueExpected, 0x12, 0x34, 0x56, 0x78, 0xcc };

			// Setup.
			CMeters meters(MEMORY_NONE);
			meters.SetMeter(mBD);
			ASSERT_TRUE(meters.GetMeterByNumber(meterNumber) != NULL) << "GetMeterByNumber";

			// Perform operation(s) and test(s).
			CLongPoll20Message msg(gameAddress, &meters, denomMult);
			msg.Fill();
			memcpy(msg.GetResponseBuffer(), responseBuffer, sizeof(responseBuffer));
			msg.SetResponseBuffer(msg.GetResponseBuffer(), 0);
			msg.SetReturnValue(SASPOLL_SUCCESS);
			msg.ResponseReceived();

			ASSERT_EQ(returnValueExpected, msg.GetReturnValue()) << "GetReturnValue";
			ASSERT_EQ(totalBillsExpected, msg.GetTotalBills()) << "GetTotalBills";
			ASSERT_EQ(meterValueExpected, meters.GetMeterByNumber(meterNumber)->GetValue()) << "GetValue";
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll20Message::CLongPoll20Message()
		///     CLongPoll20Message::ResponseReceived()
		///     CLongPoll20Message::GetReturnValue()
		///     CLongPoll20Message::GetTotalBills()
		/// Test a response with response length too short.
		/// </summary>
		TEST_F(LongPoll20Message, CLongPoll20Message_ResponseReceived_ShortResponseLengthTest)
		{
			// Data.
			const byte gameAddress = 1;
			const WORD denomMult = 10;
			const byte meterNumber(mBD);
			const int returnValueExpected(SASPOLL_NACK);
			const __int64 totalBillsExpected(-1);
			const __int64 meterValueExpected(-1);

			// Omit last CRC byte from response buffer.
			BYTE responseBuffer[] = { (BYTE)(gameAddress + 1), m_pollValueExpected, 0x12, 0x34, 0x56, 0x78, 0xcc };

			// Setup.
			CMeters meters(MEMORY_NONE);
			meters.SetMeter(mBD);
			ASSERT_TRUE(meters.GetMeterByNumber(meterNumber) != NULL) << "GetMeterByNumber";

			// Perform operation(s) and test(s).
			CLongPoll20Message msg(gameAddress, &meters, denomMult);
			msg.Fill();
			memcpy(msg.GetResponseBuffer(), responseBuffer, sizeof(responseBuffer));
			msg.SetResponseBuffer(msg.GetResponseBuffer(), sizeof(responseBuffer));
			msg.SetReturnValue(SASPOLL_SUCCESS);
			msg.ResponseReceived();

			ASSERT_EQ(returnValueExpected, msg.GetReturnValue()) << "GetReturnValue";
			ASSERT_EQ(totalBillsExpected, msg.GetTotalBills()) << "GetTotalBills";
			ASSERT_EQ(meterValueExpected, meters.GetMeterByNumber(meterNumber)->GetValue()) << "GetValue";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll20Message::CLongPoll20Message()
		///     CLongPoll20Message::ResponseReceived()
		///     CLongPoll20Message::GetReturnValue()
		///     CLongPoll20Message::GetTotalBills()
		/// Test successful response with NACK data.
		/// </summary>
		TEST_F(LongPoll20Message, CLongPoll20Message_ResponseReceived_NackDataTest)
		{
			// Data.
			const byte gameAddress = 1;
			const WORD denomMult = 10;
			const byte meterNumber(mBD);
			const int returnValueExpected(SASPOLL_NACK);
			const __int64 totalBillsExpected(-1);
			const __int64 meterValueExpected(-1);

			// Add extra CRC byte to response buffer.
			BYTE responseBuffer[] = { (BYTE)(gameAddress + 1), m_pollValueExpected, 0x12, 0x34, 0x56, 0x78, 0xcc, 0xcc, 0xcc };
			CUnitTestingUtils::SetSasCrc(responseBuffer, sizeof(responseBuffer));

			// Setup.
			CMeters meters(MEMORY_NONE);
			meters.SetMeter(mBD);
			ASSERT_TRUE(meters.GetMeterByNumber(meterNumber) != NULL) << "GetMeterByNumber";

			// Perform operation(s) and test(s).
			CLongPoll20Message msg(gameAddress, &meters, denomMult);
			msg.Fill();
			*msg.GetResponseBuffer() = (BYTE)(gameAddress | m_nackOrValue);
			msg.SetResponseBuffer(msg.GetResponseBuffer(), sizeof(BYTE));
			msg.SetReturnValue(SASPOLL_SUCCESS);
			msg.ResponseReceived();

			ASSERT_EQ(returnValueExpected, msg.GetReturnValue()) << "GetReturnValue";
			ASSERT_EQ(totalBillsExpected, msg.GetTotalBills()) << "GetTotalBills";
			ASSERT_EQ(meterValueExpected, meters.GetMeterByNumber(meterNumber)->GetValue()) << "GetValue";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll20Message::CLongPoll20Message()
		///     CLongPoll20Message::ResponseReceived()
		///     CLongPoll20Message::GetReturnValue()
		///     CLongPoll20Message::GetTotalBills()
		/// Test successful response with ACK data.
		/// </summary>
		TEST_F(LongPoll20Message, CLongPoll20Message_ResponseReceived_AckTest)
		{
			// Data.
			const byte gameAddress = 1;
			const WORD denomMult = 10;
			const byte meterNumber(mBD);
			const int returnValueExpected(SASPOLL_NACK);
			const __int64 totalBillsExpected(-1);
			const __int64 meterValueExpected(-1);

			// Add extra CRC byte to response buffer.
			BYTE responseBuffer[] = { (BYTE)(gameAddress + 1), m_pollValueExpected, 0x12, 0x34, 0x56, 0x78, 0xcc, 0xcc, 0xcc };
			CUnitTestingUtils::SetSasCrc(responseBuffer, sizeof(responseBuffer));

			// Setup.
			CMeters meters(MEMORY_NONE);
			meters.SetMeter(mBD);
			ASSERT_TRUE(meters.GetMeterByNumber(meterNumber) != NULL) << "GetMeterByNumber";

			// Perform operation(s) and test(s).
			CLongPoll20Message msg(gameAddress, &meters, denomMult);
			msg.Fill();
			*msg.GetResponseBuffer() = (BYTE)(gameAddress);
			memcpy(msg.GetResponseBuffer(), responseBuffer, sizeof(BYTE));
			msg.SetResponseBuffer(msg.GetResponseBuffer(), sizeof(BYTE));
			msg.SetReturnValue(SASPOLL_SUCCESS);
			msg.ResponseReceived();

			ASSERT_EQ(returnValueExpected, msg.GetReturnValue()) << "GetReturnValue";
			ASSERT_EQ(totalBillsExpected, msg.GetTotalBills()) << "GetTotalBills";
			ASSERT_EQ(meterValueExpected, meters.GetMeterByNumber(meterNumber)->GetValue()) << "GetValue";
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll20Message::CLongPoll20Message()
		///     CLongPoll20Message::ResponseReceived()
		///     CLongPoll20Message::GetReturnValue()
		///     CLongPoll20Message::GetTotalBills()
		/// Test a response with an incorrect game address.
		/// </summary>
		TEST_F(LongPoll20Message, CLongPoll20Message_ResponseReceived_BadGameAddressTest)
		{
			// Data.
			const byte gameAddress = 1;
			const WORD denomMult = 10;
			const byte meterNumber(mBD);
			const int returnValueExpected(SASPOLL_NACK);
			const __int64 totalBillsExpected(-1);
			const __int64 meterValueExpected(-1);

			BYTE responseBuffer[] = { (BYTE)(gameAddress + 1), m_pollValueExpected, 0x12, 0x34, 0x56, 0x78, 0xcc, 0xcc };
			CUnitTestingUtils::SetSasCrc(responseBuffer, sizeof(responseBuffer));
			ASSERT_EQ(m_responseBufferSizeExpected, sizeof(responseBuffer)) << "sizeof(responseBuffer)";

			// Setup.
			CMeters meters(MEMORY_NONE);
			meters.SetMeter(mBD);
			ASSERT_TRUE(meters.GetMeterByNumber(meterNumber) != NULL) << "GetMeterByNumber";

			// Perform operation(s) and test(s).
			CLongPoll20Message msg(gameAddress, &meters, denomMult);
			msg.Fill();
			memcpy(msg.GetResponseBuffer(), responseBuffer, msg.GetResponseBufferSize());
			msg.SetReturnValue(SASPOLL_SUCCESS);
			msg.ResponseReceived();

			ASSERT_EQ(returnValueExpected, msg.GetReturnValue()) << "GetReturnValue";
			ASSERT_EQ(totalBillsExpected, msg.GetTotalBills()) << "GetTotalBills";
			ASSERT_EQ(meterValueExpected, meters.GetMeterByNumber(meterNumber)->GetValue()) << "GetValue";
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll20Message::CLongPoll20Message()
		///     CLongPoll20Message::ResponseReceived()
		///     CLongPoll20Message::GetReturnValue()
		///     CLongPoll20Message::GetTotalBills()
		/// Test a response with an incorrect poll value.
		/// </summary>
		TEST_F(LongPoll20Message, CLongPoll20Message_ResponseReceived_BadPollValueTest)
		{
			// Data.
			const byte gameAddress = 1;
			const WORD denomMult = 10;
			const byte meterNumber(mBD);
			const int returnValueExpected(SASPOLL_NACK);
			const __int64 totalBillsExpected(-1);
			const __int64 meterValueExpected(-1);

			BYTE responseBuffer[] = { gameAddress, (BYTE)(m_pollValueExpected + 1), 0x12, 0x34, 0x56, 0x78, 0xcc, 0xcc };
			CUnitTestingUtils::SetSasCrc(responseBuffer, sizeof(responseBuffer));
			ASSERT_EQ(m_responseBufferSizeExpected, sizeof(responseBuffer)) << "sizeof(responseBuffer)";

			// Setup.
			CMeters meters(MEMORY_NONE);
			meters.SetMeter(mBD);
			ASSERT_TRUE(meters.GetMeterByNumber(meterNumber) != NULL) << "GetMeterByNumber";

			// Perform operation(s) and test(s).
			CLongPoll20Message msg(gameAddress, &meters, denomMult);
			msg.Fill();
			memcpy(msg.GetResponseBuffer(), responseBuffer, msg.GetResponseBufferSize());
			msg.SetReturnValue(SASPOLL_SUCCESS);
			msg.ResponseReceived();

			ASSERT_EQ(returnValueExpected, msg.GetReturnValue()) << "GetReturnValue";
			ASSERT_EQ(totalBillsExpected, msg.GetTotalBills()) << "GetTotalBills";
			ASSERT_EQ(meterValueExpected, meters.GetMeterByNumber(meterNumber)->GetValue()) << "GetValue";
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll20Message::CLongPoll20Message()
		///     CLongPoll20Message::ResponseReceived()
		///     CLongPoll20Message::GetReturnValue()
		///     CLongPoll20Message::GetTotalBills()
		/// Test a valid response with zero for the denomination.
		/// </summary>
		TEST_F(LongPoll20Message, CLongPoll20Message_ResponseReceived_InvalidDenomTest)
		{
			// Data.
			const byte gameAddress = 1;
			const WORD denomMult = 0;
			const byte meterNumber(mBD);
			const int returnValueExpected(SASPOLL_SUCCESS);
			const __int64 totalBillsExpected(12345678);
			const __int64 meterValueExpected(0);

			BYTE responseBuffer[] = { gameAddress, m_pollValueExpected, 0x12, 0x34, 0x56, 0x78, 0xcc, 0xcc };
			CUnitTestingUtils::SetSasCrc(responseBuffer, sizeof(responseBuffer));
			ASSERT_EQ(m_responseBufferSizeExpected, sizeof(responseBuffer)) << "sizeof(responseBuffer)";

			// Setup.
			CMeters meters(MEMORY_NONE);
			meters.SetMeter(mBD);
			ASSERT_TRUE(meters.GetMeterByNumber(meterNumber) != NULL) << "GetMeterByNumber";

			// Perform operation(s) and test(s).
			CLongPoll20Message msg(gameAddress, &meters, denomMult);
			msg.Fill();
			memcpy(msg.GetResponseBuffer(), responseBuffer, msg.GetResponseBufferSize());
			msg.SetReturnValue(SASPOLL_SUCCESS);
			msg.ResponseReceived();

			ASSERT_EQ(returnValueExpected, msg.GetReturnValue()) << "GetReturnValue";
			ASSERT_EQ(totalBillsExpected, msg.GetTotalBills()) << "GetTotalBills";
			ASSERT_EQ(meterValueExpected, meters.GetMeterByNumber(meterNumber)->GetValue()) << "GetValue";
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll20Message::CLongPoll20Message()
		///     CLongPoll20Message::ResponseReceived()
		///     CLongPoll20Message::GetReturnValue()
		///     CLongPoll20Message::GetTotalBills()
		/// Test a valid response with a non-zero denomination.
		/// </summary>
		TEST_F(LongPoll20Message, CLongPoll20Message_ResponseReceived_ValidTest)
		{
			// Data.
			const byte gameAddress = 1;
			const WORD denomMult = 10;
			const byte meterNumber(mBD);
			const int returnValueExpected(SASPOLL_SUCCESS);
			const __int64 totalBillsExpected(12345678);
			const __int64 meterValueExpected(totalBillsExpected * (__int64)100 / denomMult);

			BYTE responseBuffer[] = { gameAddress, m_pollValueExpected, 0x12, 0x34, 0x56, 0x78, 0xcc, 0xcc };
			CUnitTestingUtils::SetSasCrc(responseBuffer, sizeof(responseBuffer));
			ASSERT_EQ(m_responseBufferSizeExpected, sizeof(responseBuffer)) << "sizeof(responseBuffer)";

			// Setup.
			CMeters meters(MEMORY_NONE);
			meters.SetMeter(mBD);
			ASSERT_TRUE(meters.GetMeterByNumber(meterNumber) != NULL) << "GetMeterByNumber";

			// Perform operation(s) and test(s).
			CLongPoll20Message msg(gameAddress, &meters, denomMult);
			msg.Fill();
			memcpy(msg.GetResponseBuffer(), responseBuffer, msg.GetResponseBufferSize());
			msg.SetReturnValue(SASPOLL_SUCCESS);
			msg.ResponseReceived();

			ASSERT_EQ(returnValueExpected, msg.GetReturnValue()) << "GetReturnValue";
			ASSERT_EQ(totalBillsExpected, msg.GetTotalBills()) << "GetTotalBills";
			ASSERT_EQ(meterValueExpected, meters.GetMeterByNumber(meterNumber)->GetValue()) << "GetValue";
		}
// #pragma endregion ResponseReceived
}
