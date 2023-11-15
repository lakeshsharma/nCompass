#include "stdafx.h"
#include "PollerProtocol/PollerSubGameSnapshotTransactionMessage.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class PollerSubgameSnapshotTransactionMessage : public ::testing::Test
	{
	public:
	
		template <typename T>
		void VerifyTransData(byte * transBuffer, byte valOffset, byte expectedValType, T expectedVal)
		{
			byte rcvdValType = 0;
			memcpy(&rcvdValType, transBuffer + valOffset, sizeof(byte));
			EXPECT_EQ (rcvdValType, expectedValType);
			
			byte rcvdValSz = 0;
			uint8_t sizeOffset = valOffset + sizeof(byte);
			memcpy (&rcvdValSz, transBuffer + sizeOffset, sizeof(byte));
			EXPECT_EQ (rcvdValSz, sizeof(T));
			
			T rcvdVal = 0;
			uint8_t dataOffset = sizeOffset + sizeof(byte); 
			memcpy (&rcvdVal, transBuffer + dataOffset, rcvdValSz);
			EXPECT_EQ (rcvdVal, expectedVal);
		}
		
		void VerifyTransStringData(byte * transBuffer, byte dataOffset, byte expectedDataType, byte expectedDataSize, const char* expectedStr)
		{
			byte rcvdDataType = 0;
			memcpy(&rcvdDataType, transBuffer + dataOffset, sizeof(byte));
			EXPECT_EQ (rcvdDataType, expectedDataType);
			
			byte rcvdDataSize = 0;
			uint8_t sizeOffset = dataOffset + sizeof(byte);
			memcpy (&rcvdDataSize, transBuffer + sizeOffset, sizeof(byte));
			EXPECT_EQ (rcvdDataSize, expectedDataSize);
			
			char *rcvdStr = new char[rcvdDataSize + 1];
			uint8_t strDataOffset = sizeOffset + sizeof(byte);
			memcpy (rcvdStr, transBuffer + strDataOffset, rcvdDataSize);
			rcvdStr[rcvdDataSize] = '\0';
			EXPECT_STREQ (rcvdStr, expectedStr);
			delete [] rcvdStr;
		}		
	};

	TEST_F(PollerSubgameSnapshotTransactionMessage, ValidateTransData)
	{
		CMeters *meters(new CMeters(MEMORY_NONE));
		meters->SetMeter(mCrd, 2500);

		CMeters *subgameMeters(new CMeters(MEMORY_NONE));
		subgameMeters->SetMeter(SUBGAME_METER_GAMES_PLAYED, 200);

		SYSTEMTIME curTime = { 2021, 5, 2, 19, 23, 59, 59, 0 };
        byte cardID[CARD_ID_LEN] = {0};                                                                                

		CPollerSubGameSnapshotTransactionMessage pollerMsg(meters, SUBGAME_SNAPSHOT_CODE, curTime, 
			20,				/* unique ID */ 
			false,			/* game comm down */ 
			5,				/* game number*/
			subgameMeters,	/* subgame meters */
			InitialMeters,	/* snapshot type */
			cardID,			/* cardID */
			"G",			/* manufacturerID */
			"P1780",		/* paytableID */
			"01");			/* AdditionalID */
		
		pollerMsg.SetReadWriteBuffer();
		
		DWORD transLength(0);
		byte *transBuffer = pollerMsg.GetReadWriteBuffer(transLength);

		DWORD transLengthTillTransData = SIZE_OF_MESSAGE_HEADER +
			NONVARIABLE_TRANSACTION_SIZE +
			sizeof(byte) +									// meter count
			((sizeof(byte) + sizeof(__int64))) + 			// 1 meter
			sizeof(byte); 									// Transaction data count

		DWORD transLengthExpected = transLengthTillTransData + 
			sizeof(byte) + sizeof(byte) + sizeof(byte) + 				// Snapshot type
			sizeof(byte) + sizeof(byte) + POLLER_MFG_ID_LEN +  			// ManufactureId
			sizeof(byte) + sizeof(byte) + POLLER_PAY_TABLE_ID_LEN + 	// PaytableId
			sizeof(byte) + sizeof(byte) + POLLER_ADDITIONAL_ID_LEN +    // AdditionalId
			sizeof(byte) + sizeof(byte) + sizeof(byte) +				// Game Comm. Down
			sizeof(byte) + sizeof(byte) + sizeof(WORD) +			    // Game number
			sizeof(byte) + sizeof(byte) + (subgameMeters->GetMeterCount() * sizeof(__int64)); // Subgame Meters

		ASSERT_TRUE(transBuffer != nullptr) << "Buffer returned by CPollerSubGameSnapshotTransactionMessage is NULL.";
        ASSERT_EQ(transLength, transLengthExpected) << "Transaction buffer Length does not match as expected.";

		// Verify transaction data count is 5
		ASSERT_EQ(transBuffer[transLengthTillTransData - 1], 7) << "Transaction data count is not as expected.";

		// Offsets into transBuffer.
		int snapshotTypeOffset = transLengthTillTransData;
		int manufIdOffset = snapshotTypeOffset + (sizeof(byte) + sizeof(byte) + sizeof(byte));
		int paytableIdOffset  = manufIdOffset + (sizeof(byte) + sizeof(byte) + POLLER_MFG_ID_LEN);
		int additionalIdOffset = paytableIdOffset + (sizeof(byte) + sizeof(byte) + POLLER_PAY_TABLE_ID_LEN);
		int gameCommDownOffset = additionalIdOffset + (sizeof(byte) + sizeof(byte) + POLLER_ADDITIONAL_ID_LEN);
		int gameNumberOffset = gameCommDownOffset + (sizeof(byte) + sizeof(byte) + sizeof(byte));
		int subgameMetersOffset = gameNumberOffset + (sizeof(byte) + sizeof(byte) + sizeof(WORD));

		VerifyTransData(transBuffer, snapshotTypeOffset, DATA_TYPE_SNAPSHOT_TYPE, (byte) InitialMeters);
		VerifyTransStringData(transBuffer, manufIdOffset, DATA_TYPE_MANUFACTURER_ID, POLLER_MFG_ID_LEN, "G");
		VerifyTransStringData(transBuffer, paytableIdOffset, DATA_TYPE_PAYTABLE_ID, POLLER_PAY_TABLE_ID_LEN, "P1780");
		VerifyTransStringData(transBuffer, additionalIdOffset, DATA_TYPE_ADDITIONAL_ID, POLLER_ADDITIONAL_ID_LEN, "01");
		VerifyTransData(transBuffer, gameCommDownOffset, DATA_TYPE_GAME_COMM_DOWN, false);
		VerifyTransData(transBuffer, gameNumberOffset, DATA_TYPE_GAME_NUMBER, (WORD)5);
	}

	TEST_F(PollerSubgameSnapshotTransactionMessage, ValidateTransDataWithMaxStringLimits)
	{
		CMeters *meters(new CMeters(MEMORY_NONE));
		meters->SetMeter(mCrd, 2500);

		CMeters *subgameMeters(new CMeters(MEMORY_NONE));
		subgameMeters->SetMeter(SUBGAME_METER_GAMES_PLAYED, 200);

		SYSTEMTIME curTime = { 2021, 5, 2, 19, 23, 59, 59, 0 };
        byte cardID[CARD_ID_LEN] = {0};                                                                                

		CPollerSubGameSnapshotTransactionMessage pollerMsg(meters, SUBGAME_SNAPSHOT_CODE, curTime, 
			20,				/* unique ID */ 
			true,			/* game comm down */ 
			7,				/* game number*/
			subgameMeters,	/* subgame meters */
			InitialMeters,	/* snapshot type */
			cardID,			/* cardID */
			"GK",			/* manufacturerID */
			"P1780A",		/* paytableID */
			"001");			/* AdditionalID */
		
		pollerMsg.SetReadWriteBuffer();
		
		DWORD transLength(0);
		byte *transBuffer = pollerMsg.GetReadWriteBuffer(transLength);

		DWORD transLengthTillTransData = SIZE_OF_MESSAGE_HEADER +
			NONVARIABLE_TRANSACTION_SIZE +
			sizeof(byte) +									// meter count
			((sizeof(byte) + sizeof(__int64))) + 			// 1 meter
			sizeof(byte); 									// Transaction data count

		DWORD transLengthExpected = transLengthTillTransData + 
			sizeof(byte) + sizeof(byte) + sizeof(byte) + 				// Snapshot type
			sizeof(byte) + sizeof(byte) + POLLER_MFG_ID_LEN +  			// ManufactureId
			sizeof(byte) + sizeof(byte) + POLLER_PAY_TABLE_ID_LEN + 	// PaytableId
			sizeof(byte) + sizeof(byte) + POLLER_ADDITIONAL_ID_LEN +    // AdditionalId
			sizeof(byte) + sizeof(byte) + sizeof(byte) +				// Game Comm. Down
			sizeof(byte) + sizeof(byte) + sizeof(WORD) +			    // Game number
			sizeof(byte) + sizeof(byte) + (subgameMeters->GetMeterCount() * sizeof(__int64)); // Subgame Meters

		ASSERT_TRUE(transBuffer != nullptr) << "Buffer returned by CPollerSubGameSnapshotTransactionMessage is NULL.";
        ASSERT_EQ(transLength, transLengthExpected) << "Transaction buffer Length does not match as expected.";

		// Verify transaction data count is 5
		ASSERT_EQ(transBuffer[transLengthTillTransData - 1], 7) << "Transaction data count is not as expected.";

		// Offsets into transBuffer.
		int snapshotTypeOffset = transLengthTillTransData;
		int manufIdOffset = snapshotTypeOffset + (sizeof(byte) + sizeof(byte) + sizeof(byte));
		int paytableIdOffset  = manufIdOffset + (sizeof(byte) + sizeof(byte) + POLLER_MFG_ID_LEN);
		int additionalIdOffset = paytableIdOffset + (sizeof(byte) + sizeof(byte) + POLLER_PAY_TABLE_ID_LEN);
		int gameCommDownOffset = additionalIdOffset + (sizeof(byte) + sizeof(byte) + POLLER_ADDITIONAL_ID_LEN);
		int gameNumberOffset = gameCommDownOffset + (sizeof(byte) + sizeof(byte) + sizeof(byte));
		int subgameMetersOffset = gameNumberOffset + (sizeof(byte) + sizeof(byte) + sizeof(WORD));

		VerifyTransData(transBuffer, snapshotTypeOffset, DATA_TYPE_SNAPSHOT_TYPE, (byte) InitialMeters);
		VerifyTransStringData(transBuffer, manufIdOffset, DATA_TYPE_MANUFACTURER_ID, POLLER_MFG_ID_LEN, "GK");
		VerifyTransStringData(transBuffer, paytableIdOffset, DATA_TYPE_PAYTABLE_ID, POLLER_PAY_TABLE_ID_LEN, "P1780A");
		VerifyTransStringData(transBuffer, additionalIdOffset, DATA_TYPE_ADDITIONAL_ID, POLLER_ADDITIONAL_ID_LEN, "001");
		VerifyTransData(transBuffer, gameCommDownOffset, DATA_TYPE_GAME_COMM_DOWN, true);
		VerifyTransData(transBuffer, gameNumberOffset, DATA_TYPE_GAME_NUMBER, (WORD)7);
	}
}
