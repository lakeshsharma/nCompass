#include "stdafx.h"
#include "PollerProtocol/PollerCardlessSessionTransactionMessage.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class PollerCardlessSessionTransactionMessage : public ::testing::Test
	{
	public:
	
		template <typename T>
		void VerifyTransData(byte * transBuffer, byte valOffset, byte actualValType, T actualVal)
		{
			T rcvdVal = 0;
			byte rcvdValSz = 0;
			byte rcvdValType = 0;
			
			// Test the data
			memcpy(&rcvdValType, transBuffer + valOffset, sizeof(byte));
			ASSERT_EQ (rcvdValType, actualValType);
			
			uint8_t sizeOffset = valOffset + sizeof(byte);
			memcpy (&rcvdValSz, transBuffer + sizeOffset, sizeof(byte));
			ASSERT_EQ (rcvdValSz, sizeof(T));
			
			uint8_t dataOffset = sizeOffset + sizeof(byte); 
			memcpy (&rcvdVal, transBuffer + dataOffset, rcvdValSz);
			ASSERT_EQ (rcvdVal, actualVal);
		}
	};

	TEST_F(PollerCardlessSessionTransactionMessage, CardlessSessionStart_NonZeroMeters_Test)
	{
		CMeters *meters(new CMeters(MEMORY_NONE));
		meters->SetMeter(mCrd, 2500);
		
		// Perform operation(s) and test(s).
		CPollerCardlessSessionTransactionMessage pollerMsg(meters, CARDLESS_SESSION_START_CODE, 
			20,		/* unique ID */ 
			30		/* play session ID */, 
			7		/* play transaction sequence*/,
			2,		/* game number */
			20,		/* game denom */
			true,	/* global MGMD enabled */
			true);	/* MGMD available */
		
		DWORD transLength(0);
		byte *transBuffer = pollerMsg.GetReadWriteBuffer(transLength);

		DWORD transLengthTillTransData = SIZE_OF_MESSAGE_HEADER +
			NONVARIABLE_TRANSACTION_SIZE +
			sizeof(byte) +									// meter count
			((sizeof(byte) + sizeof(__int64))) + 			// 1 meter
			sizeof(byte); 									// Transaction data count

		DWORD transLengthExpected = transLengthTillTransData + 
			sizeof(byte) + sizeof(byte) + sizeof(DWORD) + 	// Play Session ID
			sizeof(byte) + sizeof(byte) + sizeof(DWORD) +  	// Play Tx Sequence Number
			sizeof(byte) + sizeof(byte) + sizeof(WORD) +    // Game Number
			sizeof(byte) + sizeof(byte) + sizeof(WORD) +    // Game Denomination
			sizeof(byte) + sizeof(byte) + sizeof(byte);     // MGMD Available

		ASSERT_TRUE(transBuffer != nullptr) << "Buffer returned by CPollerCardlessTransactionMessage is NULL.";
        ASSERT_EQ(transLength, transLengthExpected) << "Transaction buffer Length does not match as expected.";

		// Verify transaction data count is 5
		ASSERT_EQ(transBuffer[transLengthTillTransData - 1], 5) << "Transaction data count is not as expected.";

		// Offsets into transBuffer.
		int playSessionIdOffset = transLengthTillTransData;
		int playTxSeqNumOffset = playSessionIdOffset + (sizeof(byte) + sizeof(byte) + sizeof(DWORD));
		int gameNumberOffset  = playTxSeqNumOffset + (sizeof(byte) + sizeof(byte) + sizeof(DWORD));
		int gameDenomOffset = gameNumberOffset + (sizeof(byte) + sizeof(byte) + sizeof(WORD));
		int mgmdAvailableOffset = gameDenomOffset + (sizeof(byte) + sizeof(byte) + sizeof(WORD));

		VerifyTransData(transBuffer, playSessionIdOffset, DATA_TYPE_PLAY_SESSION_ID, (DWORD) 30);
		VerifyTransData(transBuffer, playTxSeqNumOffset, DATA_TYPE_PLAY_TRANSACTION_SEQUENCE, (DWORD) 7);
		VerifyTransData(transBuffer, gameNumberOffset, DATA_TYPE_GAME_NUMBER, (WORD)2);
		VerifyTransData(transBuffer, gameDenomOffset, DATA_TYPE_DENOM, (WORD)20);
		VerifyTransData(transBuffer, mgmdAvailableOffset, DATA_TYPE_MGMD_AVAILABLE, true);
	}

	TEST_F(PollerCardlessSessionTransactionMessage, CardlessSessionEnd_ZeroMeters_Test)
	{
		CMeters *meters(new CMeters(MEMORY_NONE));

		// Perform operation(s) and test(s).
		CPollerCardlessSessionTransactionMessage pollerMsg(meters, CARDLESS_SESSION_END_CODE, 
			15,		/* unique ID */
			20		/* play session ID */,
			0		/* play transaction sequence*/,
			2,		/* game number */
			20,		/* game denom */
			false,	/* global MGMD enabled */
			true);	/* MGMD available */
		
		DWORD transLength(0);
		byte *transBuffer = pollerMsg.GetReadWriteBuffer(transLength);

		DWORD transLengthTillTransData = SIZE_OF_MESSAGE_HEADER +
			NONVARIABLE_TRANSACTION_SIZE +
			sizeof(byte) +									// meter count
			sizeof(byte); 									// Transaction data count

		DWORD transLengthExpected = transLengthTillTransData +
			sizeof(byte) + sizeof(byte) + sizeof(DWORD) + 	// Play Session ID
			sizeof(byte) + sizeof(byte) + sizeof(DWORD) +  	// Play Tx Sequence Number
			sizeof(byte) + sizeof(byte) + sizeof(WORD) +    // Game Number
			sizeof(byte) + sizeof(byte) + sizeof(WORD) +    // Game Denomination
			sizeof(byte) + sizeof(byte) + sizeof(byte);     // MGMD Available

		ASSERT_TRUE(transBuffer != nullptr) << "Buffer returned by CPollerCardlessTransactionMessage is NULL.";
        ASSERT_EQ(transLength, transLengthExpected) << "Transaction buffer Length does not match as expected.";

		// Verify transaction data count is 5
		ASSERT_EQ(transBuffer[transLengthTillTransData - 1], 5) << "Transaction data count is not as expected.";

		// Offsets into transBuffer.
		int playSessionIdOffset = transLengthTillTransData;
		int playTxSeqNumOffset = playSessionIdOffset + (sizeof(byte) + sizeof(byte) + sizeof(DWORD));
		int gameNumberOffset = playTxSeqNumOffset + (sizeof(byte) + sizeof(byte) + sizeof(DWORD));
		int gameDenomOffset = gameNumberOffset + (sizeof(byte) + sizeof(byte) + sizeof(WORD));
		int mgmdAvailableOffset = gameDenomOffset + (sizeof(byte) + sizeof(byte) + sizeof(WORD));

		VerifyTransData(transBuffer, playSessionIdOffset, DATA_TYPE_PLAY_SESSION_ID, (DWORD)20);
		VerifyTransData(transBuffer, playTxSeqNumOffset, DATA_TYPE_PLAY_TRANSACTION_SEQUENCE, (DWORD)0);
		VerifyTransData(transBuffer, gameNumberOffset, DATA_TYPE_GAME_NUMBER, (WORD)0);
		VerifyTransData(transBuffer, gameDenomOffset, DATA_TYPE_DENOM, (WORD)0);
		VerifyTransData(transBuffer, mgmdAvailableOffset, DATA_TYPE_MGMD_AVAILABLE, false);
	}
}
