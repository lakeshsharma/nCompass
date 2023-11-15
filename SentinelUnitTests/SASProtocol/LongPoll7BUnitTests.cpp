#include "stdafx.h"
#include "SASProtocol/LongPoll7BMessage.h"

#include "GlobalDefs.h"
#include "Utilities.h"


#include "gtest/gtest.h"
using namespace std;

#define _I8_MIN     (-127 - 1)    // minimum signed 8 bit value
#define _I8_MAX       127         // maximum signed 8 bit value
#define _UI8_MAX      0xffu       // maximum unsigned 8 bit value

#define _I16_MIN    (-32767 - 1) // minimum signed 16 bit value
#define _I16_MAX      32767      // maximum signed 16 bit value

namespace SentinelUnitTests
{

		/// Code coverage and functional test for:
		///     CLongPoll7BMessage::CLongPoll7BMessage()
		/// Code coverage:
		///     CLongPoll7BMessage::~CLongPoll7BMessage()
		/// Test for correct default values.
		/// </summary>
		TEST(LongPoll7BMessage, CLongPoll7BMessageTest)
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
			const BYTE pollValueExpected(0x7B);
			const bool swapPollValueExpected(false);
			const UINT responseBufferSizeExpected(15);
			const BYTE responseBufferExpected[responseBufferSizeExpected] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			const UINT sendBufferSizeExpected(13);
			const BYTE sendBufferExpected[sendBufferSizeExpected] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			const bool printerCashoutExpected(false);
			const bool printerHandpayReceiptExpected(false);
			const bool validateHandpaysExpected(false);
			const bool printRestrictedTicketsExpected(false);
			const bool foreignRestrictedAmountsExpected(false);
			const bool ticketRedemptionExpected(false);
			const bool secureEnhancedConfigExpected(false);
			const WORD cashableExpirationExpected(0);
			const WORD restrictedExpirationExpected(0);
			const DWORD assetNumberExpected(0);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CLongPoll7BMessage varMsg(TEST_DATA[testIndex].m_gameAddress);
				const CLongPoll7BMessage &msg(varMsg);

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
							testIndex,
							(__TFUNCTION__), __LINE__);
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
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #9.
				{
					const bool printerCashoutResult = msg.GetPrinterCashout();
					const bool success(printerCashoutResult == printerCashoutExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("printerCashoutResult=%d expected=%d testIndex=%d in %s(%d)",
							printerCashoutResult,
							printerCashoutExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #10.
				{
					const bool printerHandpayReceiptResult = msg.GetPrinterHandpayReceipt();
					const bool success(printerHandpayReceiptResult == printerHandpayReceiptExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("printerHandpayReceiptResult=%d expected=%d testIndex=%d in %s(%d)",
							printerHandpayReceiptResult,
							printerHandpayReceiptExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #11.
				{
					const bool validateHandpaysResult = msg.GetValidateHandpays();
					const bool success(validateHandpaysResult == validateHandpaysExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("validateHandpaysResult=%d expected=%d testIndex=%d in %s(%d)",
							validateHandpaysResult,
							validateHandpaysExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #12.
				{
					const bool printRestrictedTicketsResult = msg.GetPrintRestrictedTickets();
					const bool success(printRestrictedTicketsResult == printRestrictedTicketsExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("printRestrictedTicketsResult=%d expected=%d testIndex=%d in %s(%d)",
							printRestrictedTicketsResult,
							printRestrictedTicketsExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #13.
				{
					const bool foreignRestrictedAmountsResult = msg.GetForeignRestrictedAmounts();
					const bool success(foreignRestrictedAmountsResult == foreignRestrictedAmountsExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("foreignRestrictedAmountsResult=%d expected=%d testIndex=%d in %s(%d)",
							foreignRestrictedAmountsResult,
							foreignRestrictedAmountsExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #14.
				{
					const bool ticketRedemptionResult = msg.GetTicketRedemption();
					const bool success(ticketRedemptionResult == ticketRedemptionExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("ticketRedemptionResult=%d expected=%d testIndex=%d in %s(%d)",
							ticketRedemptionResult,
							ticketRedemptionExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #15.
				{
					const bool secureEnhancedConfigResult = msg.GetSecureEnhancedConfig();
					const bool success(secureEnhancedConfigResult == secureEnhancedConfigExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("secureEnhancedConfigResult=%d expected=%d testIndex=%d in %s(%d)",
							secureEnhancedConfigResult,
							secureEnhancedConfigExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #16.
				{
					const WORD cashableExpirationResult = msg.GetCashableExpiration();
					const bool success(cashableExpirationResult == cashableExpirationExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("cashableExpirationResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)cashableExpirationResult,
							(unsigned)cashableExpirationExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}
				
				// Test #17.
				{
					const WORD restrictedExpirationResult = msg.GetRestrictedExpiration();
					const bool success(restrictedExpirationResult == restrictedExpirationExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("restrictedExpirationResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)restrictedExpirationResult,
							(unsigned)restrictedExpirationExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #18.
				{
					const DWORD assetNumberResult = msg.GetAssetNumber();
					const bool success(assetNumberResult == assetNumberExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("assetNumberResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)assetNumberResult,
							(unsigned)assetNumberExpected,
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
		///     CLongPoll7BMessage::CLongPoll7BMessage()
		///     CLongPoll7BMessage::Fill()
		/// Test for correct default values.
		/// </summary>
		TEST(LongPoll7BMessage, Fill_DefaultTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(13);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0xff, 0x7b,  8, 0, 0, 0, 0, 0, 0, 0, 0, 0x8b, 0x9d };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 0x00, 0x7b, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0x16, 0x4d };
			const BYTE sendBuffer2[sendBufferSizeExpected] = { 0x01, 0x7b, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0x87, 0x18 };
			const BYTE sendBuffer3[sendBufferSizeExpected] = { 0x02, 0x7b, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0x34, 0xe6 };
			const BYTE sendBuffer4[sendBufferSizeExpected] = { 0x7e, 0x7b, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0x01, 0x5a };
			const BYTE sendBuffer5[sendBufferSizeExpected] = { 0x7f, 0x7b, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0x90, 0x0f };
			const BYTE sendBuffer6[sendBufferSizeExpected] = { 0x80, 0x7b, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0x0d, 0xdf };
			const BYTE sendBuffer7[sendBufferSizeExpected] = { 0x81, 0x7b, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0x9c, 0x8a };
			const BYTE sendBuffer8[sendBufferSizeExpected] = { 0xfe, 0x7b, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0x1a, 0xc8 };
			const BYTE sendBuffer9[sendBufferSizeExpected] = { 0xff, 0x7b, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0x8b, 0x9d };

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
				CLongPoll7BMessage varMsg(TEST_DATA[testIndex].m_gameAddress);
				varMsg.Fill();
				const CLongPoll7BMessage &msg(varMsg);

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
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7BMessage::SetPrinterCashout()
		///     CLongPoll7BMessage::GetPrinterCashout()
		/// </summary>
		TEST(LongPoll7BMessage, SetPrinterCashoutTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const bool m_printerCashout;

				// Expected results.
				const bool m_printerCashoutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetPrinterCashout(TEST_DATA[testIndex].m_printerCashout);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const bool printerCashoutResult = msg.GetPrinterCashout();
					const bool success(printerCashoutResult == TEST_DATA[testIndex].m_printerCashoutExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("printerCashoutResult=%d expected=%d testIndex=%d in %s(%d)",
							printerCashoutResult,
							TEST_DATA[testIndex].m_printerCashoutExpected,
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
		///     CLongPoll7BMessage::SetPrinterHandpayReceipt()
		///     CLongPoll7BMessage::GetPrinterHandpayReceipt()
		/// </summary>
		TEST(LongPoll7BMessage, SetPrinterHandpayReceiptTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const bool m_printerHandpayReceipt;

				// Expected results.
				const bool m_printerHandpayReceiptExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetPrinterHandpayReceipt(TEST_DATA[testIndex].m_printerHandpayReceipt);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const bool printerHandpayReceiptResult = msg.GetPrinterHandpayReceipt();
					const bool success(printerHandpayReceiptResult == TEST_DATA[testIndex].m_printerHandpayReceiptExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("printerHandpayReceiptResult=%d expected=%d testIndex=%d in %s(%d)",
							printerHandpayReceiptResult,
							TEST_DATA[testIndex].m_printerHandpayReceiptExpected,
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
		///     CLongPoll7BMessage::SetValidateHandpays()
		///     CLongPoll7BMessage::GetValidateHandpays()
		/// </summary>
		TEST(LongPoll7BMessage, SetValidateHandpaysTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const bool m_validateHandpays;

				// Expected results.
				const bool m_validateHandpaysExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetValidateHandpays(TEST_DATA[testIndex].m_validateHandpays);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const bool validateHandpaysResult = msg.GetValidateHandpays();
					const bool success(validateHandpaysResult == TEST_DATA[testIndex].m_validateHandpaysExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("validateHandpaysResult=%d expected=%d testIndex=%d in %s(%d)",
							validateHandpaysResult,
							TEST_DATA[testIndex].m_validateHandpaysExpected,
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
		///     CLongPoll7BMessage::SetPrintRestrictedTickets()
		///     CLongPoll7BMessage::GetPrintRestrictedTickets()
		/// </summary>
		TEST(LongPoll7BMessage, SetPrintRestrictedTicketsTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const bool m_printRestrictedTickets;

				// Expected results.
				const bool m_printRestrictedTicketsExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetPrintRestrictedTickets(TEST_DATA[testIndex].m_printRestrictedTickets);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const bool printRestrictedResult = msg.GetPrintRestrictedTickets();
					const bool success(printRestrictedResult == TEST_DATA[testIndex].m_printRestrictedTicketsExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("printRestrictedResult=%d expected=%d testIndex=%d in %s(%d)",
							printRestrictedResult,
							TEST_DATA[testIndex].m_printRestrictedTicketsExpected,
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
		///     CLongPoll7BMessage::SetForeignRestrictedAmounts()
		///     CLongPoll7BMessage::GetForeignRestrictedAmounts()
		/// </summary>
		TEST(LongPoll7BMessage, SetForeignRestrictedAmountsTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const bool m_foreignRestrictedAmounts;

				// Expected results.
				const bool m_foreignRestrictedAmountsExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetForeignRestrictedAmounts(TEST_DATA[testIndex].m_foreignRestrictedAmounts);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const bool foreignRestrictedAmountsResult = msg.GetForeignRestrictedAmounts();
					const bool success(foreignRestrictedAmountsResult == TEST_DATA[testIndex].m_foreignRestrictedAmountsExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("foreignRestrictedAmountsResult=%d expected=%d testIndex=%d in %s(%d)",
							foreignRestrictedAmountsResult,
							TEST_DATA[testIndex].m_foreignRestrictedAmountsExpected,
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
		///     CLongPoll7BMessage::SetTicketRedemption()
		///     CLongPoll7BMessage::GetTicketRedemption()
		/// </summary>
		TEST(LongPoll7BMessage, SetTicketRedemptionTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const bool m_ticketRedemption;

				// Expected results.
				const bool m_ticketRedemptionExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetTicketRedemption(TEST_DATA[testIndex].m_ticketRedemption);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const bool ticketRedemptionResult = msg.GetTicketRedemption();
					const bool success(ticketRedemptionResult == TEST_DATA[testIndex].m_ticketRedemptionExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("ticketRedemptionResult=%d expected=%d testIndex=%d in %s(%d)",
							ticketRedemptionResult,
							TEST_DATA[testIndex].m_ticketRedemptionExpected,
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
		///     CLongPoll7BMessage::SetSecureEnhancedConfig()
		///     CLongPoll7BMessage::GetSecureEnhancedConfig()
		/// </summary>
		TEST(LongPoll7BMessage, SetSecureEnhancedConfigTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const bool m_secureEnhancedConfig;

				// Expected results.
				const bool m_secureEnhancedConfigExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetSecureEnhancedConfig(TEST_DATA[testIndex].m_secureEnhancedConfig);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const bool secureEnhancedConfigResult = msg.GetSecureEnhancedConfig();
					const bool success(secureEnhancedConfigResult == TEST_DATA[testIndex].m_secureEnhancedConfigExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("secureEnhancedConfigResult=%d expected=%d testIndex=%d in %s(%d)",
							secureEnhancedConfigResult,
							TEST_DATA[testIndex].m_secureEnhancedConfigExpected,
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
		///     CLongPoll7BMessage::SetPrinterCashout()
		///     CLongPoll7BMessage::GetPrinterCashout()
		/// </summary>
		TEST(LongPoll7BMessage, SetPrinterCashoutThenFalseTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const bool m_printerCashout;

				// Expected results.
				const bool m_printerCashoutExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetPrinterCashout(TEST_DATA[testIndex].m_printerCashout);
				varMsg.SetPrinterCashout(false);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const bool printerCashoutResult = msg.GetPrinterCashout();
					const bool success(printerCashoutResult == TEST_DATA[testIndex].m_printerCashoutExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("printerCashoutResult=%d expected=%d testIndex=%d in %s(%d)",
							printerCashoutResult,
							TEST_DATA[testIndex].m_printerCashoutExpected,
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
		///     CLongPoll7BMessage::SetPrinterHandpayReceipt()
		///     CLongPoll7BMessage::GetPrinterHandpayReceipt()
		/// </summary>
		TEST(LongPoll7BMessage, SetPrinterHandpayReceiptThenFalseTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const bool m_printerHandpayReceipt;

				// Expected results.
				const bool m_printerHandpayReceiptExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetPrinterHandpayReceipt(TEST_DATA[testIndex].m_printerHandpayReceipt);
				varMsg.SetPrinterHandpayReceipt(false);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const bool printerHandpayReceiptResult = msg.GetPrinterHandpayReceipt();
					const bool success(printerHandpayReceiptResult == TEST_DATA[testIndex].m_printerHandpayReceiptExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("printerHandpayReceiptResult=%d expected=%d testIndex=%d in %s(%d)",
							printerHandpayReceiptResult,
							TEST_DATA[testIndex].m_printerHandpayReceiptExpected,
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
		///     CLongPoll7BMessage::SetValidateHandpays()
		///     CLongPoll7BMessage::GetValidateHandpays()
		/// </summary>
		TEST(LongPoll7BMessage, SetValidateHandpaysThenFalseTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const bool m_validateHandpays;

				// Expected results.
				const bool m_validateHandpaysExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetValidateHandpays(TEST_DATA[testIndex].m_validateHandpays);
				varMsg.SetValidateHandpays(false);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const bool validateHandpaysResult = msg.GetValidateHandpays();
					const bool success(validateHandpaysResult == TEST_DATA[testIndex].m_validateHandpaysExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("validateHandpaysResult=%d expected=%d testIndex=%d in %s(%d)",
							validateHandpaysResult,
							TEST_DATA[testIndex].m_validateHandpaysExpected,
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
		///     CLongPoll7BMessage::SetPrintRestrictedTickets()
		///     CLongPoll7BMessage::GetPrintRestrictedTickets()
		/// </summary>
		TEST(LongPoll7BMessage, SetPrintRestrictedTicketsThenFalseTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const bool m_printRestrictedTickets;

				// Expected results.
				const bool m_printRestrictedTicketsExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetPrintRestrictedTickets(TEST_DATA[testIndex].m_printRestrictedTickets);
				varMsg.SetPrintRestrictedTickets(false);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const bool printRestrictedResult = msg.GetPrintRestrictedTickets();
					const bool success(printRestrictedResult == TEST_DATA[testIndex].m_printRestrictedTicketsExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("printRestrictedResult=%d expected=%d testIndex=%d in %s(%d)",
							printRestrictedResult,
							TEST_DATA[testIndex].m_printRestrictedTicketsExpected,
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
		///     CLongPoll7BMessage::SetForeignRestrictedAmounts()
		///     CLongPoll7BMessage::GetForeignRestrictedAmounts()
		/// </summary>
		TEST(LongPoll7BMessage, SetForeignRestrictedAmountsThenFalseTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const bool m_foreignRestrictedAmounts;

				// Expected results.
				const bool m_foreignRestrictedAmountsExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetForeignRestrictedAmounts(TEST_DATA[testIndex].m_foreignRestrictedAmounts);
				varMsg.SetForeignRestrictedAmounts(false);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const bool foreignRestrictedAmountsResult = msg.GetForeignRestrictedAmounts();
					const bool success(foreignRestrictedAmountsResult == TEST_DATA[testIndex].m_foreignRestrictedAmountsExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("foreignRestrictedAmountsResult=%d expected=%d testIndex=%d in %s(%d)",
							foreignRestrictedAmountsResult,
							TEST_DATA[testIndex].m_foreignRestrictedAmountsExpected,
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
		///     CLongPoll7BMessage::SetTicketRedemption()
		///     CLongPoll7BMessage::GetTicketRedemption()
		/// </summary>
		TEST(LongPoll7BMessage, SetTicketRedemptionThenFalseTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const bool m_ticketRedemption;

				// Expected results.
				const bool m_ticketRedemptionExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetTicketRedemption(TEST_DATA[testIndex].m_ticketRedemption);
				varMsg.SetTicketRedemption(false);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const bool ticketRedemptionResult = msg.GetTicketRedemption();
					const bool success(ticketRedemptionResult == TEST_DATA[testIndex].m_ticketRedemptionExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("ticketRedemptionResult=%d expected=%d testIndex=%d in %s(%d)",
							ticketRedemptionResult,
							TEST_DATA[testIndex].m_ticketRedemptionExpected,
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
		///     CLongPoll7BMessage::SetSecureEnhancedConfig()
		///     CLongPoll7BMessage::GetSecureEnhancedConfig()
		/// </summary>
		TEST(LongPoll7BMessage, SetSecureEnhancedConfigThenFalseTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const bool m_secureEnhancedConfig;

				// Expected results.
				const bool m_secureEnhancedConfigExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ false, false },
				{ true, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetSecureEnhancedConfig(TEST_DATA[testIndex].m_secureEnhancedConfig);
				varMsg.SetSecureEnhancedConfig(false);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const bool secureEnhancedConfigResult = msg.GetSecureEnhancedConfig();
					const bool success(secureEnhancedConfigResult == TEST_DATA[testIndex].m_secureEnhancedConfigExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("secureEnhancedConfigResult=%d expected=%d testIndex=%d in %s(%d)",
							secureEnhancedConfigResult,
							TEST_DATA[testIndex].m_secureEnhancedConfigExpected,
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
		///     CLongPoll7BMessage::SetCashableExpiration()
		///     CLongPoll7BMessage::GetCashableExpiration()
		/// </summary>
		TEST(LongPoll7BMessage, SetCashableExpirationTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const WORD m_cashableExpiration;

				// Expected results.
				const WORD m_cashableExpirationExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (WORD)-1, (WORD)-1 },
				{ 0, 0 },
				{ 1, 1 },
				{ 2, 2 },
				{ 1234, 1234 },
				{ 9999, 9999 },
				{ 10000, 10000 },
				{ 59999, 59999 },
				{ (WORD)(_I16_MAX - 1), (WORD)(_I16_MAX - 1) },
				{ (WORD)_I16_MAX, (WORD)_I16_MAX },
				{ (WORD)_I16_MIN, (WORD)_I16_MIN },
				{ (WORD)(_I16_MIN + 1), (WORD)(_I16_MIN + 1) },
				{ (WORD)(WORD_MAX - 1), (WORD)(WORD_MAX - 1) },
				{ (WORD)WORD_MAX, (WORD)WORD_MAX },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetCashableExpiration(TEST_DATA[testIndex].m_cashableExpiration);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const WORD cashableExpirationResult = msg.GetCashableExpiration();
					const bool success(cashableExpirationResult == TEST_DATA[testIndex].m_cashableExpirationExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("cashableExpirationResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)cashableExpirationResult,
							(unsigned)TEST_DATA[testIndex].m_cashableExpirationExpected,
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
		///     CLongPoll7BMessage::SetRestrictedExpiration()
		///     CLongPoll7BMessage::GetRestrictedExpiration()
		/// </summary>
		TEST(LongPoll7BMessage, SetRestrictedExpirationTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			typedef struct
			{
				// Inputs.
				const WORD m_restrictedExpiration;

				// Expected results.
				const WORD m_restrictedExpirationExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (WORD)-1, (WORD)-1 },
				{ 0, 0 },
				{ 1, 1 },
				{ 2, 2 },
				{ 1234, 1234 },
				{ 9999, 9999 },
				{ 10000, 10000 },
				{ 59999, 59999 },
				{ (WORD)(_I16_MAX - 1), (WORD)(_I16_MAX - 1) },
				{ (WORD)_I16_MAX, (WORD)_I16_MAX },
				{ (WORD)_I16_MIN, (WORD)_I16_MIN },
				{ (WORD)(_I16_MIN + 1), (WORD)(_I16_MIN + 1) },
				{ (WORD)(WORD_MAX - 1), (WORD)(WORD_MAX - 1) },
				{ (WORD)WORD_MAX, (WORD)WORD_MAX },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetRestrictedExpiration(TEST_DATA[testIndex].m_restrictedExpiration);
				const CLongPoll7BMessage &msg(varMsg);

				// Test #1.
				{
					const WORD restrictedExpirationResult = msg.GetRestrictedExpiration();
					const bool success(restrictedExpirationResult == TEST_DATA[testIndex].m_restrictedExpirationExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("restrictedExpirationResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)restrictedExpirationResult,
							(unsigned)TEST_DATA[testIndex].m_restrictedExpirationExpected,
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
		///     CLongPoll7BMessage::SetPrinterCashout()
		///     CLongPoll7BMessage::Fill()
		/// </summary>
		TEST(LongPoll7BMessage, SetPrinterCashoutFillTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(13);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0, 0x7b,  8, 0x01, 0x00, 0x00, 0x00, 0, 0, 0, 0, 0xa9, 0xcc };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 1, 0x7b,  8, 0x01, 0x00, 0x01, 0x00, 0, 0, 0, 0, 0x13, 0x9d };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const bool m_printerCashout;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, false, sendBuffer0 },
				{ 1, true, sendBuffer1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(TEST_DATA[testIndex].m_gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetPrinterCashout(TEST_DATA[testIndex].m_printerCashout);
				varMsg.Fill();
				const CLongPoll7BMessage &msg(varMsg);

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
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7BMessage::SetPrinterHandpayReceipt()
		///     CLongPoll7BMessage::Fill()
		/// </summary>
		TEST(LongPoll7BMessage, SetPrinterHandpayReceiptFillTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(13);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0, 0x7b,  8, 0x02, 0x00, 0x00, 0x00, 0, 0, 0, 0, 0x79, 0x46 };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 1, 0x7b,  8, 0x02, 0x00, 0x02, 0x00, 0, 0, 0, 0, 0xbe, 0x1b };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const bool m_printerHandpayReceipt;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, false, sendBuffer0 },
				{ 1, true, sendBuffer1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(TEST_DATA[testIndex].m_gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetPrinterHandpayReceipt(TEST_DATA[testIndex].m_printerHandpayReceipt);
				varMsg.Fill();
				const CLongPoll7BMessage &msg(varMsg);

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
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7BMessage::SetValidateHandpays()
		///     CLongPoll7BMessage::Fill()
		/// </summary>
		TEST(LongPoll7BMessage, SetValidateHandpaysFillTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(13);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0, 0x7b,  8, 0x04, 0x00, 0x00, 0x00, 0, 0, 0, 0, 0xc8, 0x5b };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 1, 0x7b,  8, 0x04, 0x00, 0x04, 0x00, 0, 0, 0, 0, 0xf5, 0x1e };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const bool m_validateHandpays;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, false, sendBuffer0 },
				{ 1, true, sendBuffer1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(TEST_DATA[testIndex].m_gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetValidateHandpays(TEST_DATA[testIndex].m_validateHandpays);
				varMsg.Fill();
				const CLongPoll7BMessage &msg(varMsg);

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
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7BMessage::SetPrintRestrictedTickets()
		///     CLongPoll7BMessage::Fill()
		/// </summary>
		TEST(LongPoll7BMessage, SetPrintRestrictedTicketsFillTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(13);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0, 0x7b,  8, 0x08, 0x00, 0x00, 0x00, 0, 0, 0, 0, 0xaa, 0x60 };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 1, 0x7b,  8, 0x08, 0x00, 0x08, 0x00, 0, 0, 0, 0, 0x63, 0x14 };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const bool m_printRestrictedTickets;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, false, sendBuffer0 },
				{ 1, true, sendBuffer1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(TEST_DATA[testIndex].m_gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetPrintRestrictedTickets(TEST_DATA[testIndex].m_printRestrictedTickets);
				varMsg.Fill();
				const CLongPoll7BMessage &msg(varMsg);

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
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7BMessage::SetForeignRestrictedAmounts()
		///     CLongPoll7BMessage::Fill()
		/// </summary>
		TEST(LongPoll7BMessage, SetForeignRestrictedAmountsFillTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(13);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0, 0x7b,  8, 0x10, 0x00, 0x00, 0x00, 0, 0, 0, 0, 0x6e, 0x16 };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 1, 0x7b,  8, 0x10, 0x00, 0x10, 0x00, 0, 0, 0, 0, 0x4f, 0x01 };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const bool m_foreignRestrictedAmounts;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, false, sendBuffer0 },
				{ 1, true, sendBuffer1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(TEST_DATA[testIndex].m_gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetForeignRestrictedAmounts(TEST_DATA[testIndex].m_foreignRestrictedAmounts);
				varMsg.Fill();
				const CLongPoll7BMessage &msg(varMsg);

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
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7BMessage::SetTicketRedemption()
		///     CLongPoll7BMessage::Fill()
		/// </summary>
		TEST(LongPoll7BMessage, SetTicketRedemptionFillTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(13);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0, 0x7b,  8, 0x20, 0x00, 0x00, 0x00, 0, 0, 0, 0, 0xe6, 0xfb };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 1, 0x7b,  8, 0x20, 0x00, 0x20, 0x00, 0, 0, 0, 0, 0x17, 0x2b };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const bool m_ticketRedemption;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, false, sendBuffer0 },
				{ 1, true, sendBuffer1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(TEST_DATA[testIndex].m_gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetTicketRedemption(TEST_DATA[testIndex].m_ticketRedemption);
				varMsg.Fill();
				const CLongPoll7BMessage &msg(varMsg);

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
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7BMessage::SetSecureEnhancedConfig()
		///     CLongPoll7BMessage::Fill()
		/// </summary>
		TEST(LongPoll7BMessage, SetSecureEnhancedConfigFillTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(13);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0, 0x7b,  8, 0x80, 0x00, 0x00, 0x00, 0, 0, 0, 0, 0xf4, 0x86 };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 1, 0x7b,  8, 0x80, 0x00, 0x80, 0x00, 0, 0, 0, 0, 0xc7, 0xd6 };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const bool m_secureEnhancedConfig;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, false, sendBuffer0 },
				{ 1, true, sendBuffer1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(TEST_DATA[testIndex].m_gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetSecureEnhancedConfig(TEST_DATA[testIndex].m_secureEnhancedConfig);
				varMsg.Fill();
				const CLongPoll7BMessage &msg(varMsg);

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
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7BMessage::SetCashableExpiration()
		///     CLongPoll7BMessage::Fill()
		/// </summary>
		TEST(LongPoll7BMessage, SetCashableExpirationFillTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(13);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0, 0x7b,  8, 0, 0, 0, 0, 0x00, 0x00, 0, 0, 0x16, 0x4d };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 1, 0x7b,  8, 0, 0, 0, 0, 0x00, 0x00, 0, 0, 0x87, 0x18 };
			const BYTE sendBuffer2[sendBufferSizeExpected] = { 2, 0x7b,  8, 0, 0, 0, 0, 0x00, 0x01, 0, 0, 0xe8, 0xbc };
			const BYTE sendBuffer3[sendBufferSizeExpected] = { 3, 0x7b,  8, 0, 0, 0, 0, 0x00, 0x02, 0, 0, 0x1d, 0x06 };
			const BYTE sendBuffer4[sendBufferSizeExpected] = { 4, 0x7b,  8, 0, 0, 0, 0, 0x12, 0x34, 0, 0, 0x5b, 0x0c };
			const BYTE sendBuffer5[sendBufferSizeExpected] = { 5, 0x7b,  8, 0, 0, 0, 0, 0x99, 0x99, 0, 0, 0x19, 0x44 };
			const BYTE sendBuffer6[sendBufferSizeExpected] = { 6, 0x7b,  8, 0, 0, 0, 0, 0x00, 0x00, 0, 0, 0x61, 0xb8 };
			const BYTE sendBuffer7[sendBufferSizeExpected] = { 7, 0x7b,  8, 0, 0, 0, 0, 0x00, 0x00, 0, 0, 0xf0, 0xed };
			const BYTE sendBuffer8[sendBufferSizeExpected] = { 8, 0x7b,  8, 0, 0, 0, 0, 0x00, 0x00, 0, 0, 0xbc, 0xf1 };
			const BYTE sendBuffer9[sendBufferSizeExpected] = { 9, 0x7b,  8, 0, 0, 0, 0, 0x00, 0x00, 0, 0, 0x2d, 0xa4 };
			const BYTE sendBuffer10[sendBufferSizeExpected] = { 10, 0x7b,  8, 0, 0, 0, 0, 0x00, 0x00, 0, 0, 0x9e, 0x5a };
			const BYTE sendBuffer11[sendBufferSizeExpected] = { 11, 0x7b,  8, 0, 0, 0, 0, 0x00, 0x00, 0, 0, 0x0f, 0x0f };
			const BYTE sendBuffer12[sendBufferSizeExpected] = { 12, 0x7b,  8, 0, 0, 0, 0, 0x00, 0x00, 0, 0, 0xe9, 0xaf };
			const BYTE sendBuffer13[sendBufferSizeExpected] = { 13, 0x7b,  8, 0, 0, 0, 0, 0x00, 0x00, 0, 0, 0x78, 0xfa };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const WORD m_cashableExpiration;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, (WORD)-1, sendBuffer0 },
				{ 1, 0, sendBuffer1 },
				{ 2, 1, sendBuffer2 },
				{ 3, 2, sendBuffer3 },
				{ 4, 1234, sendBuffer4 },
				{ 5, 9999, sendBuffer5 },
				{ 6, 10000, sendBuffer6 },
				{ 7, 59999, sendBuffer7 },
				{ 8, (WORD)(_I16_MAX - 1), sendBuffer8 },
				{ 9, (WORD)_I16_MAX, sendBuffer9 },
				{ 10, (WORD)_I16_MIN, sendBuffer10 },
				{ 11, (WORD)(_I16_MIN + 1), sendBuffer11 },
				{ 12, (WORD)(WORD_MAX - 1), sendBuffer12 },
				{ 13, (WORD)WORD_MAX, sendBuffer13 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(TEST_DATA[testIndex].m_gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetCashableExpiration(TEST_DATA[testIndex].m_cashableExpiration);
				varMsg.Fill();
				const CLongPoll7BMessage &msg(varMsg);

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
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7BMessage::SetRestrictedExpiration()
		///     CLongPoll7BMessage::Fill()
		/// </summary>
		TEST(LongPoll7BMessage, SetRestrictedExpirationFillTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(13);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0, 0x7b,  8, 0, 0, 0, 0, 0, 0, 0x00, 0x00, 0x16, 0x4d };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 1, 0x7b,  8, 0, 0, 0, 0, 0, 0, 0x00, 0x00, 0x87, 0x18 };
			const BYTE sendBuffer2[sendBufferSizeExpected] = { 2, 0x7b,  8, 0, 0, 0, 0, 0, 0, 0x00, 0x01, 0xbd, 0xf7 };
			const BYTE sendBuffer3[sendBufferSizeExpected] = { 3, 0x7b,  8, 0, 0, 0, 0, 0, 0, 0x00, 0x02, 0xb7, 0x90 };
			const BYTE sendBuffer4[sendBufferSizeExpected] = { 4, 0x7b,  8, 0, 0, 0, 0, 0, 0, 0x12, 0x34, 0xc5, 0xc2 };
			const BYTE sendBuffer5[sendBufferSizeExpected] = { 5, 0x7b,  8, 0, 0, 0, 0, 0, 0, 0x99, 0x99, 0xdf, 0x81 };
			const BYTE sendBuffer6[sendBufferSizeExpected] = { 6, 0x7b,  8, 0, 0, 0, 0, 0, 0, 0x00, 0x00, 0x61, 0xb8 };
			const BYTE sendBuffer7[sendBufferSizeExpected] = { 7, 0x7b,  8, 0, 0, 0, 0, 0, 0, 0x00, 0x00, 0xf0, 0xed };
			const BYTE sendBuffer8[sendBufferSizeExpected] = { 8, 0x7b,  8, 0, 0, 0, 0, 0, 0, 0x00, 0x00, 0xbc, 0xf1 };
			const BYTE sendBuffer9[sendBufferSizeExpected] = { 9, 0x7b,  8, 0, 0, 0, 0, 0, 0, 0x00, 0x00, 0x2d, 0xa4 };
			const BYTE sendBuffer10[sendBufferSizeExpected] = { 10, 0x7b,  8, 0, 0, 0, 0, 0, 0, 0x00, 0x00, 0x9e, 0x5a };
			const BYTE sendBuffer11[sendBufferSizeExpected] = { 11, 0x7b,  8, 0, 0, 0, 0, 0, 0, 0x00, 0x00, 0x0f, 0x0f };
			const BYTE sendBuffer12[sendBufferSizeExpected] = { 12, 0x7b,  8, 0, 0, 0, 0, 0, 0, 0x00, 0x00, 0xe9, 0xaf };
			const BYTE sendBuffer13[sendBufferSizeExpected] = { 13, 0x7b,  8, 0, 0, 0, 0, 0, 0, 0x00, 0x00, 0x78, 0xfa };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const WORD m_restrictedExpiration;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, (WORD)-1, sendBuffer0 },
				{ 1, 0, sendBuffer1 },
				{ 2, 1, sendBuffer2 },
				{ 3, 2, sendBuffer3 },
				{ 4, 1234, sendBuffer4 },
				{ 5, 9999, sendBuffer5 },
				{ 6, 10000, sendBuffer6 },
				{ 7, 59999, sendBuffer7 },
				{ 8, (WORD)(_I16_MAX - 1), sendBuffer8 },
				{ 9, (WORD)_I16_MAX, sendBuffer9 },
				{ 10, (WORD)_I16_MIN, sendBuffer10 },
				{ 11, (WORD)(_I16_MIN + 1), sendBuffer11 },
				{ 12, (WORD)(WORD_MAX - 1), sendBuffer12 },
				{ 13, (WORD)WORD_MAX, sendBuffer13 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(TEST_DATA[testIndex].m_gameAddress);

				// Perform operation(s) to be tested.
				varMsg.SetRestrictedExpiration(TEST_DATA[testIndex].m_restrictedExpiration);
				varMsg.Fill();
				const CLongPoll7BMessage &msg(varMsg);

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
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CSASPollMessage::GetResponseEventHandle()
		/// CLongPoll7BMessage::ResponseReceived()
		/// CLongPoll7BMessage::GetPrinterCashout()
		/// CLongPoll7BMessage::GetPrinterHandpayReceipt()
		/// CLongPoll7BMessage::GetValidateHandpays()
		/// CLongPoll7BMessage::GetPrintRestrictedTickets()
		/// CLongPoll7BMessage::GetForeignRestrictedAmounts()
		/// CLongPoll7BMessage::GetTicketRedemption()
		/// CLongPoll7BMessage::GetSecureEnhancedConfig()
		/// CLongPoll7BMessage::GetCashableExpiration()
		/// CLongPoll7BMessage::GetRestrictedExpiration()
		/// CLongPoll7BMessage::GetAssetNumber()
		/// </summary>
		TEST(LongPoll7BMessage, ResponseReceivedTest)
		{
			// Inputs -- same for all loop iterations.
			const byte gameAddress(0);

			// Expected results -- same for all loop iterations.
			const UINT responseBufferSizeExpected(15);

			// Expected results -- per iteration.
			const BYTE responseBuffer0[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
			const BYTE responseBuffer1[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
			const BYTE responseBuffer2[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
			const BYTE responseBuffer3[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
			const BYTE responseBuffer4[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
			const BYTE responseBuffer5[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
			const BYTE responseBuffer6[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
			const BYTE responseBuffer7[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
			const BYTE responseBuffer8[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
			const BYTE responseBuffer9[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

			const BYTE responseBuffer10[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
			const BYTE responseBuffer11[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x00, 0x00, 0x00, 0x00 };
			const BYTE responseBuffer12[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 };
			const BYTE responseBuffer13[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 };
			const BYTE responseBuffer14[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x00, 0x00 };
			const BYTE responseBuffer15[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00 };
			const BYTE responseBuffer16[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
			const BYTE responseBuffer17[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
			const BYTE responseBuffer18[responseBufferSizeExpected] = { 0x00, 0x00,  0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

			const BYTE responseBuffer19[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
			const BYTE responseBuffer20[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
			const BYTE responseBuffer21[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
			const BYTE responseBuffer22[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
			const BYTE responseBuffer23[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
			const BYTE responseBuffer24[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
			const BYTE responseBuffer25[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
			const BYTE responseBuffer26[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
			const BYTE responseBuffer27[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

			const BYTE responseBuffer28[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff };
			const BYTE responseBuffer29[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0xff, 0xff, 0xff, 0xff };
			const BYTE responseBuffer30[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff };
			const BYTE responseBuffer31[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff };
			const BYTE responseBuffer32[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0xff, 0xff };
			const BYTE responseBuffer33[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff };
			const BYTE responseBuffer34[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
			const BYTE responseBuffer35[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0x01, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
			const BYTE responseBuffer36[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

			const BYTE responseBuffer99[responseBufferSizeExpected] = { 0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
			typedef struct
			{
				// Inputs.
				const BYTE * const m_responseBuffer;

				// Expected results.
				const bool m_printerCashoutExpected;
				const bool m_printerHandpayReceiptExpected;
				const bool m_validateHandpaysExpected;
				const bool m_printRestrictedTicketsExpected;
				const bool m_foreignRestrictedAmountsExpected;
				const bool m_ticketRedemptionExpected;
				const bool m_secureEnhancedConfigExpected;
				const WORD m_cashableExpirationExpected;
				const WORD m_restrictedExpirationExpected;
				const DWORD m_assetNumberExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// All values zero, all flags disabled.
				{ responseBuffer0, false, false, false, false, false, false, false, 0, 0, 0 },

				// All values zero, all flags disabled, or one enabled at a time.
				{ responseBuffer1, true, false, false, false, false, false, false, 0, 0, 0 },
				{ responseBuffer2, false, true, false, false, false, false, false, 0, 0, 0 },
				{ responseBuffer3, false, false, true, false, false, false, false, 0, 0, 0 },
				{ responseBuffer4, false, false, false, true, false, false, false, 0, 0, 0 },
				{ responseBuffer5, false, false, false, false, true, false, false, 0, 0, 0 },
				{ responseBuffer6, false, false, false, false, false, true, false, 0, 0, 0 },
				{ responseBuffer7, false, false, false, false, false, false, false, 0, 0, 0 },
				{ responseBuffer8, false, false, false, false, false, false, true, 0, 0, 0 },
				{ responseBuffer9, false, false, false, false, false, false, false, 0, 0, 0 },

				// All flags disabled, values zero, except test range of one value at a time.
				{ responseBuffer10, false, false, false, false, false, false, false, 1, 0, 0 },
				{ responseBuffer11, false, false, false, false, false, false, false, WORD_MAX - 1, 0, 0 },
				{ responseBuffer12, false, false, false, false, false, false, false, WORD_MAX, 0, 0 },
				{ responseBuffer13, false, false, false, false, false, false, false, 0, 1, 0 },
				{ responseBuffer14, false, false, false, false, false, false, false, 0, WORD_MAX - 1, 0 },
				{ responseBuffer15, false, false, false, false, false, false, false, 0, WORD_MAX, 0 },
				{ responseBuffer16, false, false, false, false, false, false, false, 0, 0, 1 },
				{ responseBuffer17, false, false, false, false, false, false, false, 0, 0, DWORD_MAX - 1 },
				{ responseBuffer18, false, false, false, false, false, false, false, 0, 0, DWORD_MAX },

				// All values maximum, all flags enabled, or one disabled at a time.
				{ responseBuffer19, false, true, true, true, true, true, true, 0xffff, 0xffff, 0xffffffff },
				{ responseBuffer20, true, false, true, true, true, true, true, 0xffff, 0xffff, 0xffffffff },
				{ responseBuffer21, true, true, false, true, true, true, true, 0xffff, 0xffff, 0xffffffff },
				{ responseBuffer22, true, true, true, false, true, true, true, 0xffff, 0xffff, 0xffffffff },
				{ responseBuffer23, true, true, true, true, false, true, true, 0xffff, 0xffff, 0xffffffff },
				{ responseBuffer24, true, true, true, true, true, false, true, 0xffff, 0xffff, 0xffffffff },
				{ responseBuffer25, true, true, true, true, true, true, true, 0xffff, 0xffff, 0xffffffff },
				{ responseBuffer26, true, true, true, true, true, true, false, 0xffff, 0xffff, 0xffffffff },
				{ responseBuffer27, true, true, true, true, true, true, true, 0xffff, 0xffff, 0xffffffff },

				// All flags enabled, values maximum, except test range of one value at a time.
				{ responseBuffer28, true, true, true, true, true, true, true, 0, 0xffff, 0xffffffff },
				{ responseBuffer29, true, true, true, true, true, true, true, 1, 0xffff, 0xffffffff },
				{ responseBuffer30, true, true, true, true, true, true, true, WORD_MAX - 1, 0xffff, 0xffffffff },
				{ responseBuffer31, true, true, true, true, true, true, true, 0xffff, 0, 0xffffffff },
				{ responseBuffer32, true, true, true, true, true, true, true, 0xffff, 1, 0xffffffff },
				{ responseBuffer33, true, true, true, true, true, true, true, 0xffff, WORD_MAX - 1, 0xffffffff },
				{ responseBuffer34, true, true, true, true, true, true, true, 0xffff, 0xffff, 0 },
				{ responseBuffer35, true, true, true, true, true, true, true, 0xffff, 0xffff, 1 },
				{ responseBuffer36, true, true, true, true, true, true, true, 0xffff, 0xffff, DWORD_MAX - 1 },

				// All flags enabled, values maximum.
				{ responseBuffer99, true, true, true, true, true, true, true, 0xffff, 0xffff, 0xffffffff },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7BMessage varMsg(gameAddress);
				const CLongPoll7BMessage &msg(varMsg);
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
					const bool printerCashoutResult = msg.GetPrinterCashout();
					const bool success(printerCashoutResult == TEST_DATA[testIndex].m_printerCashoutExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("printerCashoutResult=%d expected=%d testIndex=%d in %s(%d)",
							printerCashoutResult,
							TEST_DATA[testIndex].m_printerCashoutExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #2.
				{
					const bool printerHandpayReceiptResult = msg.GetPrinterHandpayReceipt();
					const bool success(printerHandpayReceiptResult == TEST_DATA[testIndex].m_printerHandpayReceiptExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("printerHandpayReceiptResult=%d expected=%d testIndex=%d in %s(%d)",
							printerHandpayReceiptResult,
							TEST_DATA[testIndex].m_printerHandpayReceiptExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #3.
				{
					const bool validateHandpaysResult = msg.GetValidateHandpays();
					const bool success(validateHandpaysResult == TEST_DATA[testIndex].m_validateHandpaysExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("validateHandpaysResult=%d expected=%d testIndex=%d in %s(%d)",
							validateHandpaysResult,
							TEST_DATA[testIndex].m_validateHandpaysExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #4.
				{
					const bool printRestrictedTicketsResult = msg.GetPrintRestrictedTickets();
					const bool success(printRestrictedTicketsResult == TEST_DATA[testIndex].m_printRestrictedTicketsExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("printRestrictedTicketsResult=%d expected=%d testIndex=%d in %s(%d)",
							printRestrictedTicketsResult,
							TEST_DATA[testIndex].m_printRestrictedTicketsExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #5.
				{
					const bool foreignRestrictedAmountsResult = msg.GetForeignRestrictedAmounts();
					const bool success(foreignRestrictedAmountsResult == TEST_DATA[testIndex].m_foreignRestrictedAmountsExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("foreignRestrictedAmountsResult=%d expected=%d testIndex=%d in %s(%d)",
							foreignRestrictedAmountsResult,
							TEST_DATA[testIndex].m_foreignRestrictedAmountsExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #6.
				{
					const bool ticketRedemptionResult = msg.GetTicketRedemption();
					const bool success(ticketRedemptionResult == TEST_DATA[testIndex].m_ticketRedemptionExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("ticketRedemptionResult=%d expected=%d testIndex=%d in %s(%d)",
							ticketRedemptionResult,
							TEST_DATA[testIndex].m_ticketRedemptionExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #7.
				{
					const bool secureEnhancedConfigResult = msg.GetSecureEnhancedConfig();
					const bool success(secureEnhancedConfigResult == TEST_DATA[testIndex].m_secureEnhancedConfigExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("secureEnhancedConfigResult=%d expected=%d testIndex=%d in %s(%d)",
							secureEnhancedConfigResult,
							TEST_DATA[testIndex].m_secureEnhancedConfigExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #8.
				{
					const WORD cashableExpirationResult = msg.GetCashableExpiration();
					const bool success(cashableExpirationResult == TEST_DATA[testIndex].m_cashableExpirationExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("cashableExpirationResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)cashableExpirationResult,
							(unsigned)TEST_DATA[testIndex].m_cashableExpirationExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}
				
				// Test #9.
				{
					const WORD restrictedExpirationResult = msg.GetRestrictedExpiration();
					const bool success(restrictedExpirationResult == TEST_DATA[testIndex].m_restrictedExpirationExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("restrictedExpirationResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)restrictedExpirationResult,
							(unsigned)TEST_DATA[testIndex].m_restrictedExpirationExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #10.
				{
					const DWORD assetNumberResult = msg.GetAssetNumber();
					const bool success(assetNumberResult == TEST_DATA[testIndex].m_assetNumberExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("assetNumberResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)assetNumberResult,
							(unsigned)TEST_DATA[testIndex].m_assetNumberExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}
}
