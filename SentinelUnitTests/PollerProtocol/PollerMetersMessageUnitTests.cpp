#include "stdafx.h"
#include "PollerProtocol/PollerMetersMessage.h"
#include "PollerProtocol/PollerSubGameMetersMessage.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
    TEST(PollerMetersMessage, PollerEGMMetersMessage_NonNFDPoller)
    {
		CGames *games = new CGames();
		
		CGame *egmGame = new CGame(MEMORY_NONE, true /* isEgm */);
		egmGame->GetMeters()->SetMeter(mCrd, 2500);
		games->AddGame(egmGame);
		
		CGame *subGame = new CGame(MEMORY_NONE);
		subGame->GetMeters()->SetMeter(SUBGAME_METER_GAMES_PLAYED, 200);
		subGame->ChangeCurrentGameStatus(true);		
		games->AddGame(subGame);
		
        CPollerMetersMessage pollerMsg(games, 
			false,  /*nfdSupported*/
			60,     /*session ID*/ 
			 1      /*transaction sequence*/
			 );

		long bufferLenExpected = SIZE_OF_MESSAGE_HEADER + 
			sizeof(byte) +                    /* meters count */
			sizeof(byte) + sizeof(__int64);   /* meter number and value for 1 meter */

		long bufferLen = 0;
		byte *buffer = pollerMsg.GetReadWriteBuffer(bufferLen);

		ASSERT_TRUE(buffer != nullptr) << "Buffer returned by CPollerMetersMessage is NULL.";
        ASSERT_EQ(bufferLen, bufferLenExpected) << "Buffer Length does not match as expected.";
    }

    TEST(PollerMetersMessage, PollerEGMMetersMessage_NFDPoller)
    {
		CGames *games = new CGames();
		
		CGame *egmGame = new CGame(MEMORY_NONE, true /* isEgm */);
		egmGame->GetMeters()->SetMeter(mCrd, 25L);
		egmGame->GetMeters()->SetMeter(gCI, 2500L);
		games->AddGame(egmGame);
		
		CGame *subGame = new CGame(MEMORY_NONE);
		subGame->GetMeters()->SetMeter(SUBGAME_METER_GAMES_PLAYED, 200);
		subGame->ChangeCurrentGameStatus(true);		
		games->AddGame(subGame);
		
        CPollerMetersMessage pollerMsg(games, 
			true,  /*nfdSupported*/
			60,     /*session ID*/ 
			 1      /*transaction sequence*/
			 );

		long bufferLenExpected = SIZE_OF_MESSAGE_HEADER + 
			sizeof(byte) +   /* meters count */
			2 * (sizeof(byte) + sizeof(__int64)) + /* meterNum and meterValue for 2 meters */
		    sizeof(SYSTEMTIME) + 2 * sizeof(DWORD) + /* DateTime, session ID, and TxSeq */
			sizeof(DWORD) +   // Current subgame number, and its related fields follow
			POLLER_MFG_ID_LEN + POLLER_PAY_TABLE_ID_LEN + POLLER_ADDITIONAL_ID_LEN + 
			sizeof(WORD) + sizeof(BYTE) +  // Theoretical payback, game status
			(NVRAM_NUMBER_OF_SUBGAME_METERS * sizeof(__int64)); // current subgame meters

		long bufferLen = 0;
		byte *buffer = pollerMsg.GetReadWriteBuffer(bufferLen);

		ASSERT_TRUE(buffer != nullptr) << "Buffer returned by CPollerMetersMessage is NULL";
        ASSERT_EQ(bufferLen, bufferLenExpected) << "Buffer Length does not match as expected.";
    }

    TEST(PollerMetersMessage, PollerSubgameMetersMessage_NonNFDPoller)
    {
		CGames *games = new CGames();
		CGame *subGame = new CGame(MEMORY_NONE);
		subGame->GetMeters()->SetMeter(SUBGAME_METER_GAMES_PLAYED, 200);
		games->AddGame(subGame);
		
        CPollerSubGameMetersMessage pollerMsg(games, 
			false  /*nfdSupported*/
			 );

		long bufferLenExpected = SIZE_OF_MESSAGE_HEADER + 
			sizeof(WORD) + 									    // Game number 
			POLLER_PAY_TABLE_ID_LEN + POLLER_MFG_ID_LEN +	    // PaytableID and MfgID
			POLLER_ADDITIONAL_ID_LEN +                          // AdditionalID
			sizeof(WORD) + sizeof(byte) +                       // Th.Payback and GameStatus
			(NVRAM_NUMBER_OF_SUBGAME_METERS * sizeof(__int64)); // 10 subgame meters
			
		long bufferLen = 0;
		byte *buffer = pollerMsg.GetReadWriteBuffer(bufferLen);

		ASSERT_TRUE(buffer != nullptr) << "Buffer returned by CPollerSubGameMetersMessage is NULL";
        ASSERT_EQ(bufferLen, bufferLenExpected) << "Buffer Length does not match as expected.";
    }

    TEST(PollerMetersMessage, PollerSubgameMetersMessage_NFDPoller)
    {
		CGames *games = new CGames();
		CGame *subGame = new CGame(MEMORY_NONE);
		subGame->GetMeters()->SetMeter(SUBGAME_METER_GAMES_PLAYED, 200);
		games->AddGame(subGame);
		
        CPollerSubGameMetersMessage pollerMsg(games, 
			true  /*nfdSupported*/
			 );

		long bufferLenExpected = SIZE_OF_MESSAGE_HEADER + 
			sizeof(DWORD) + 									// Game number 
			sizeof(SYSTEMTIME) +                                // Current DateTime
			POLLER_PAY_TABLE_ID_LEN + POLLER_MFG_ID_LEN +	    // PaytableID and MfgID
			POLLER_ADDITIONAL_ID_LEN +                          // AdditionalID
			sizeof(WORD) + sizeof(byte) +                       // Th.Payback and GameStatus
			(NVRAM_NUMBER_OF_SUBGAME_METERS * sizeof(__int64)); // 10 subgame meters
			
		long bufferLen = 0;
		byte *buffer = pollerMsg.GetReadWriteBuffer(bufferLen);

		ASSERT_TRUE(buffer != nullptr) << "Buffer returned by CPollerSubGameMetersMessage is NULL";
        ASSERT_EQ(bufferLen, bufferLenExpected) << "Buffer Length does not match as expected.";
    }
}