#include "stdafx.h"
#include "PollerProtocol/PollerPlayerTransactionMessage.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	static const DWORD GSN (31);
	static const WORD DENOM (11);
	static const WORD GAME_NUM (13);
	static const WORD ZeroWord(0);
	static const DWORD ZeroDword(0);

	class PollerPlayerTransactionMessage : public ::testing::Test
	{
	public:
		static const time_t DEFAULT_TIME; // Arbitrary value when any valid value is okay.
	
		void VerifyWordData(CPollerPlayerTransactionMessage& pollerMsg, byte valOffset, WORD actualVal, byte actualValType)
		{
			DWORD transLength(0);
			const byte *transBuffer(pollerMsg.GetReadWriteBuffer(transLength));
		
			DWORD msglen = valOffset + sizeof(byte) + sizeof(byte) + sizeof(byte); //offset + type	+ length + value
			ASSERT_GE(transLength, msglen);
		
			WORD rcvdVal = 0;
			byte rcvdValSz = 0;
			byte rcvdValType = 0;
			
			// Test the data
			memcpy(&rcvdValType, transBuffer + valOffset, sizeof(byte));
			ASSERT_EQ (rcvdValType, actualValType);
			
			uint8_t sizeOffset = valOffset + sizeof(byte);
			memcpy (&rcvdValSz, transBuffer + sizeOffset, sizeof(byte));
			ASSERT_EQ (rcvdValSz, sizeof(actualVal) );
			
			uint8_t dataOffset = sizeOffset + sizeof(byte); 
			memcpy (&rcvdVal, transBuffer + dataOffset, rcvdValSz);
			ASSERT_EQ (rcvdVal, actualVal);
		}
		
		void VerifyDWordData(CPollerPlayerTransactionMessage& pollerMsg, byte valOffset, DWORD actualVal, byte actualValType)
		{
			DWORD transLength(0);
			const byte *transBuffer(pollerMsg.GetReadWriteBuffer(transLength));
		
			DWORD msglen = valOffset + sizeof(byte) + sizeof(byte) + sizeof(byte); //offset + type	+ length + value
			ASSERT_GE(transLength, msglen);
		
			DWORD rcvdVal = 0;
			byte rcvdValSz = 0;
			byte rcvdValType = 0;
			
			// Test the data
			memcpy(&rcvdValType, transBuffer + valOffset, sizeof(byte));
			ASSERT_EQ (rcvdValType, actualValType);
			
			uint8_t sizeOffset = valOffset + sizeof(byte);
			memcpy (&rcvdValSz, transBuffer + sizeOffset, sizeof(byte));
			ASSERT_EQ (rcvdValSz, sizeof(actualVal) );
			
			uint8_t dataOffset = sizeOffset + sizeof(byte); 
			memcpy (&rcvdVal, transBuffer + dataOffset, rcvdValSz);
			ASSERT_EQ (rcvdVal, actualVal);
		
		}
		
		void VerifyByteData(CPollerPlayerTransactionMessage& pollerMsg, byte valOffset, byte actualVal, byte actualValType)
		{
			DWORD transLength(0);
			const byte *transBuffer(pollerMsg.GetReadWriteBuffer(transLength));

			DWORD msglen = valOffset + sizeof(byte) + sizeof(byte) + sizeof(byte); //offset + type  + length + value
			ASSERT_GE(transLength, msglen);

            		byte rcvdVal = 0;
			byte rcvdValSz = 0;
			byte rcvdValType = 0;
			
			// Test the data
			memcpy(&rcvdValType, transBuffer + valOffset, sizeof(byte));
			ASSERT_EQ (rcvdValType, actualValType);

			uint8_t sizeOffset = valOffset + sizeof(byte);
			memcpy (&rcvdValSz, transBuffer + sizeOffset, sizeof(byte));
			ASSERT_EQ (rcvdValSz, sizeof(actualVal) );

			uint8_t dataOffset = sizeOffset + sizeof(byte); 
			memcpy (&rcvdVal, transBuffer + dataOffset, rcvdValSz);
			ASSERT_EQ (rcvdVal, actualVal);
		}
		
	};

		/// <summary>
		/// Functional test for:
		///     CPollerPlayerTransactionMessage::CPollerPlayerTransactionMessage()
		///     CPollerPlayerTransactionMessage::GetReadWriteBuffer()
		/// Test physical card out with no virtual session for correct activeSeconds value(s).
		TEST_F(PollerPlayerTransactionMessage, GetReadWriteBuffer_ActiveSecondsValue_CardOutNoVirtual)
		{
			// Data.
			const int activeSecondsValueOffset(84); // Offset into transBuffer.
			const __int64 activeSecondsExpected(60);
			const time_t lastSessionStartTime(CUtilities::GetTimeFromFields(2015, 10, 20, 12, 23, 34));
			time_t transTime(lastSessionStartTime + activeSecondsExpected); // Need read/write value.

			// Setup CPlayer for physical card out with no virtual session.
			CCard *card(new CCard(lastSessionStartTime, lastSessionStartTime));
			card->SetCardID(_T("1234567890"));
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			const time_t utcRemovalTime(transTime - time_t(5));
			const time_t removalTime(utcRemovalTime + -7 * 3600);
			const bool makeVirtual(false);
			CMeters *virtualStartMeters(nullptr);

            CGames *games = new CGames();
			CPlayer *player(new CPlayer(card, new CPlayerInfo, config, egmConfig, DEFAULT_TIME, false, games));
			player->CardRemoved(removalTime, utcRemovalTime, makeVirtual);
			ASSERT_EQ(lastSessionStartTime, player->GetLastSessionStartTime()); // Verify setup.

			// Setup for CPollerPlayerTransactionMessage.
			CMeters *meters(new CMeters(MEMORY_NONE));

			// Perform operation(s) and test(s).
			CPollerPlayerTransactionMessage pollerMsg(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime, 0);
			DWORD transLength(0);
			const byte *transBuffer(pollerMsg.GetReadWriteBuffer(transLength));

			__int64 activeSeconds(0);
			ASSERT_TRUE(transLength >= activeSecondsValueOffset + sizeof(activeSeconds));
			memcpy(&activeSeconds, transBuffer + activeSecondsValueOffset, sizeof(activeSeconds));
			ASSERT_EQ(activeSecondsExpected, activeSeconds);
		}

		/// <summary>
		/// Functional test for:
		///     CPollerPlayerTransactionMessage::CPollerPlayerTransactionMessage()
		///     CPollerPlayerTransactionMessage::GetReadWriteBuffer()
		/// Test physical card out with make virtual session for correct activeSeconds value(s).
		TEST_F(PollerPlayerTransactionMessage, GetReadWriteBuffer_ActiveSecondsValue_CardOutMakeVirtual)
		{
			// Data.
			const int activeSecondsValueOffset(84); // Offset into transBuffer.
			const __int64 activeSecondsExpected(60);
			const time_t lastSessionStartTime(CUtilities::GetTimeFromFields(2015, 10, 20, 12, 23, 34));
			time_t transTime(lastSessionStartTime + activeSecondsExpected); // Need read/write value.

			// Setup CPlayer for physical card out with no virtual session.
			CCard *card(new CCard(lastSessionStartTime, lastSessionStartTime));
			card->SetCardID(_T("1234567890"));
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			const time_t utcRemovalTime(transTime - time_t(5));
			const time_t removalTime(utcRemovalTime + -7 * 3600);
			const bool makeVirtual(true);
			CMeters *virtualStartMeters(new CMeters(MEMORY_NONE));

            CGames *games = new CGames();
			CPlayer *player(new CPlayer(card, new CPlayerInfo, config, egmConfig, DEFAULT_TIME, false, games));
			player->CardRemoved(removalTime, utcRemovalTime, makeVirtual);
			ASSERT_EQ(lastSessionStartTime, player->GetLastSessionStartTime()); // Verify setup.

			// Setup for CPollerPlayerTransactionMessage.
			CMeters *meters(new CMeters(MEMORY_NONE));

			// Perform operation(s) and test(s).
			CPollerPlayerTransactionMessage pollerMsg(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime, 0);
			DWORD transLength(0);
			const byte *transBuffer(pollerMsg.GetReadWriteBuffer(transLength));

			__int64 activeSeconds(0);
			ASSERT_TRUE(transLength >= activeSecondsValueOffset + sizeof(activeSeconds));
			memcpy(&activeSeconds, transBuffer + activeSecondsValueOffset, sizeof(activeSeconds));
			ASSERT_EQ(activeSecondsExpected, activeSeconds);
		}

		/// <summary>
		/// Functional test for:
		///     CPollerPlayerTransactionMessage::CPollerPlayerTransactionMessage()
		///     CPollerPlayerTransactionMessage::GetReadWriteBuffer()
		/// Test virtual session end for correct activeSeconds value(s).
		TEST_F(PollerPlayerTransactionMessage, GetReadWriteBuffer_ActiveSecondsValue_VirtualSessionEnd)
		{
			// Data.
			const int activeSecondsValueOffset(84); // Offset into transBuffer.
			const __int64 activeSecondsExpected(60);
			const time_t lastSessionStartTime(CUtilities::GetTimeFromFields(2015, 10, 20, 12, 23, 34));
			time_t transTime(lastSessionStartTime + activeSecondsExpected); // Need read/write value.

			// Setup CPlayer for virtual card out.
			CCard *card(new CCard(DEFAULT_TIME, DEFAULT_TIME));
			card->SetCardID(_T("1234567890"));
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			const time_t utcRemovalTime(lastSessionStartTime);
			const bool makeVirtual(true);
			CMeters *virtualStartMeters(new CMeters(MEMORY_NONE));

			const time_t virtualEndTime(utcRemovalTime + time_t(activeSecondsExpected - 5));

            CGames *games = new CGames();
			CPlayer *player(new CPlayer(card, new CPlayerInfo, config, egmConfig, lastSessionStartTime, false, games));
			player->CardRemoved(utcRemovalTime, utcRemovalTime, makeVirtual);
			player->VirtualSessionEnd(virtualEndTime);
			ASSERT_EQ(lastSessionStartTime, player->GetLastSessionStartTime()); // Verify setup.

			// Setup for CPollerPlayerTransactionMessage.
			CMeters *meters(new CMeters(MEMORY_NONE));

			// Perform operation(s) and test(s).
			CPollerPlayerTransactionMessage pollerMsg(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime, 0);
			DWORD transLength(0);
			const byte *transBuffer(pollerMsg.GetReadWriteBuffer(transLength));

			__int64 activeSeconds(0);
			ASSERT_TRUE(transLength >= activeSecondsValueOffset + sizeof(activeSeconds));
			memcpy(&activeSeconds, transBuffer + activeSecondsValueOffset, sizeof(activeSeconds));
			ASSERT_EQ(activeSecondsExpected, activeSeconds);
		}

		/// <summary>
		/// Functional test for:
		///     CPollerPlayerTransactionMessage::CPollerPlayerTransactionMessage()
		///     CPollerPlayerTransactionMessage::GetPlaySessionID()
		/// Test Player session ID.
		TEST_F(PollerPlayerTransactionMessage, GetPlaySessionID_ValueUpdate)
		{
			//No need to set this time to do basic tests on class instances.
			time_t transTime; 

			// Setup for CPollerPlayerTransactionMessage.
			CMeters *meters(new CMeters(MEMORY_NONE));

			// Perform operation(s) and test(s).
			CPollerPlayerTransactionMessage pollerMsg(nullptr, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime , 0);

			DWORD psid = 12;
			pollerMsg.SetPlaySessionID(psid);
			ASSERT_EQ(pollerMsg.GetPlaySessionID(), psid);
		}

		/// <summary>
		/// Functional test for:
		///     CPollerPlayerTransactionMessage::CPollerPlayerTransactionMessage()
		///     CPollerPlayerTransactionMessage::GetReadWriteBuffer()
		/// Test Player session ID in the PollerPlayerTransactionMessage buffer.
		TEST_F(PollerPlayerTransactionMessage, GetReadWriteBuffer_PlaySessionID)
		{
			// Default time fine for unit tests, must be initialized for gcc 12 else segfault city
			time_t transTime = DEFAULT_TIME;

			// Setup CPlayer for virtual card out.
			CCard *card(new CCard(DEFAULT_TIME, DEFAULT_TIME));
			card->SetCardID(_T("1234567890"));
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

            CGames *games = new CGames();
			CPlayer *player(new CPlayer(card, new CPlayerInfo, config, egmConfig, DEFAULT_TIME, false, games));

			// Setup for CPollerPlayerTransactionMessage.
			CMeters *meters(new CMeters(MEMORY_NONE));

			// Perform operation(s) and test(s).
			CPollerPlayerTransactionMessage pollerMsg(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime , 0);

			DWORD psid = 12;
			pollerMsg.SetPlaySessionID(psid);

			DWORD transLength(0);
			const byte *transBuffer(pollerMsg.GetReadWriteBuffer(transLength));

			uint8_t typeOffset(98);
			DWORD recvdPsid(0);
			byte recvdType(0), recvdDataSize(0), actualDataSize(4);

			ASSERT_TRUE(transLength >= typeOffset + sizeof(byte));
			memcpy(&recvdType, transBuffer + typeOffset, sizeof(byte));
			ASSERT_EQ (recvdType, DATA_TYPE_PLAY_SESSION_ID);

			uint8_t sizeOffset = typeOffset + sizeof(byte);
			ASSERT_TRUE(transLength >= sizeOffset + sizeof(byte));
			memcpy (&recvdDataSize, transBuffer + sizeOffset, sizeof(byte));
			ASSERT_EQ (recvdDataSize, actualDataSize );

			uint8_t dataOffset = sizeOffset + sizeof(byte);	
			ASSERT_TRUE(transLength >= dataOffset + recvdDataSize );
			memcpy (&recvdPsid, transBuffer + dataOffset, recvdDataSize);
			ASSERT_EQ (recvdPsid, psid);
		}


	/// <summary>
	/// Functional test for:
	/// 	CPollerPlayerTransactionMessage::CPollerPlayerTransactionMessage()
	/// 	CPollerPlayerTransactionMessage::GetReadWriteBuffer()
	/// Test Game Game number data from the message
	/// </summary>
	TEST_F(PollerPlayerTransactionMessage, GetReadWriteBuffer_CheckGameNumberData)
	{
		//No need to set this time to do basic tests on class instances.
		time_t transTime; 
		
		// Setup CPlayer for virtual card out.
		CCard *card(new CCard(DEFAULT_TIME, DEFAULT_TIME));
		card->SetCardID(_T("1234567890"));
		const CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);
		
		CGames *games = new CGames();
		CPlayer *player(new CPlayer(card, new CPlayerInfo, config, egmConfig, DEFAULT_TIME, false, games));
		
		// Setup for CPollerPlayerTransactionMessage.
		CMeters *meters(new CMeters(MEMORY_NONE));
		
		// Perform operation(s) and test(s).
		CPollerPlayerTransactionMessage pollerMsg(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime , 0, GSN, DENOM, GAME_NUM, 1);

		byte gameNumOffset (104);

		VerifyWordData(pollerMsg, gameNumOffset, GAME_NUM, DATA_TYPE_GAME_NUMBER);
	 }
	/// <summary>
	/// Functional test for:
	/// 	CPollerPlayerTransactionMessage::CPollerPlayerTransactionMessage()
	/// 	CPollerPlayerTransactionMessage::GetReadWriteBuffer()
	/// Test Game Game number data from the message
	/// </summary>
	TEST_F(PollerPlayerTransactionMessage, GetReadWriteBuffer_CheckGameNumberDataMGMDGlobalOff)
	{
		//No need to set this time to do basic tests on class instances.
		time_t transTime; 
		
		// Setup CPlayer for virtual card out.
		CCard *card(new CCard(DEFAULT_TIME, DEFAULT_TIME));
		card->SetCardID(_T("1234567890"));
		const CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);
		
		CGames *games = new CGames();
		CPlayer *player(new CPlayer(card, new CPlayerInfo, config, egmConfig, DEFAULT_TIME, false, games));
		
		// Setup for CPollerPlayerTransactionMessage.
		CMeters *meters(new CMeters(MEMORY_NONE));
		
		// Perform operation(s) and test(s).
		CPollerPlayerTransactionMessage pollerMsgGlobalMGMDOff(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime , 0, GSN, DENOM, GAME_NUM, 0);
		byte gameNumOffset (104);

		//VerifyWordData(pollerMsg, gameNumOffset, GAME_NUM, DATA_TYPE_GAME_NUMBER);
		VerifyWordData(pollerMsgGlobalMGMDOff, gameNumOffset, ZeroWord, DATA_TYPE_GAME_NUMBER);
    }

	/// <summary>
	/// Functional test for:
	/// 	CPollerPlayerTransactionMessage::CPollerPlayerTransactionMessage()
	/// 	CPollerPlayerTransactionMessage::GetReadWriteBuffer()
	/// Test Game denom data from the message
	/// </summary>
	TEST_F(PollerPlayerTransactionMessage, GetReadWriteBuffer_CheckGameDenomData)
	{
		//No need to set this time to do basic tests on class instances.
		time_t transTime; 
		
		// Setup CPlayer for virtual card out.
		CCard *card(new CCard(DEFAULT_TIME, DEFAULT_TIME));
		card->SetCardID(_T("1234567890"));
		const CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);
		
		CGames *games = new CGames();
		CPlayer *player(new CPlayer(card, new CPlayerInfo, config, egmConfig, DEFAULT_TIME, false, games));
		
		// Setup for CPollerPlayerTransactionMessage.
		CMeters *meters(new CMeters(MEMORY_NONE));
		
		// Perform operation(s) and test(s).
		CPollerPlayerTransactionMessage pollerMsg(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime , 0, GSN, DENOM, GAME_NUM, 1);
		
		byte denomOffset (108);

		VerifyWordData(pollerMsg, denomOffset, DENOM, DATA_TYPE_DENOM);
   	 }
	/// <summary>
	/// Functional test for:
	/// 	CPollerPlayerTransactionMessage::CPollerPlayerTransactionMessage()
	/// 	CPollerPlayerTransactionMessage::GetReadWriteBuffer()
	/// Test Game denom data from the message
	/// </summary>
	TEST_F(PollerPlayerTransactionMessage, GetReadWriteBuffer_CheckGameDenomDataMGMDGlobalOff)
	{
		//No need to set this time to do basic tests on class instances.
		time_t transTime; 
		
		// Setup CPlayer for virtual card out.
		CCard *card(new CCard(DEFAULT_TIME, DEFAULT_TIME));
		card->SetCardID(_T("1234567890"));
		const CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);
		
		CGames *games = new CGames();
		CPlayer *player(new CPlayer(card, new CPlayerInfo, config, egmConfig, DEFAULT_TIME, false, games));
		
		// Setup for CPollerPlayerTransactionMessage.
		CMeters *meters(new CMeters(MEMORY_NONE));
		
		// Perform operation(s) and test(s).
		CPollerPlayerTransactionMessage pollerMsgGlobalMGMDOff(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime , 0, GSN, DENOM, GAME_NUM, 0);

		byte denomOffset (108);

		VerifyWordData(pollerMsgGlobalMGMDOff, denomOffset, ZeroWord, DATA_TYPE_DENOM);
    }		
	/// <summary>
	/// Functional test for:
	/// 	CPollerPlayerTransactionMessage::CPollerPlayerTransactionMessage()
	/// 	CPollerPlayerTransactionMessage::GetReadWriteBuffer()
	/// Test Game Session ID in the message
	/// </summary>
	TEST_F(PollerPlayerTransactionMessage, GetReadWriteBuffer_CheckGameSessionIdData)
	{
		//No need to set this time to do basic tests on class instances.
		time_t transTime; 
		
		// Setup CPlayer for virtual card out.
		CCard *card(new CCard(DEFAULT_TIME, DEFAULT_TIME));
		card->SetCardID(_T("1234567890"));
		const CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);
		
		CGames *games = new CGames();
		CPlayer *player(new CPlayer(card, new CPlayerInfo, config, egmConfig, DEFAULT_TIME, false, games));
		
		// Setup for CPollerPlayerTransactionMessage.
		CMeters *meters(new CMeters(MEMORY_NONE));
		
		// Perform operation(s) and test(s).
		CPollerPlayerTransactionMessage pollerMsg(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime , 0, GSN, DENOM, GAME_NUM, 1);
			byte gsnIdOffset (112);
	
		VerifyDWordData(pollerMsg, gsnIdOffset, GSN, DATA_TYPE_PLAY_TRANSACTION_SEQUENCE);
	}
	/// <summary>
		/// Functional test for:
		/// 	CPollerPlayerTransactionMessage::CPollerPlayerTransactionMessage()
		/// 	CPollerPlayerTransactionMessage::GetReadWriteBuffer()
		/// Test Game Session ID in the message
		/// </summary>
	TEST_F(PollerPlayerTransactionMessage, GetReadWriteBuffer_CheckGameSessionIdDataMGMDGlobalOff)
	{
		//No need to set this time to do basic tests on class instances.
		time_t transTime; 
			
		// Setup CPlayer for virtual card out.
		CCard *card(new CCard(DEFAULT_TIME, DEFAULT_TIME));
		card->SetCardID(_T("1234567890"));
		const CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);
			
		CGames *games = new CGames();
		CPlayer *player(new CPlayer(card, new CPlayerInfo, config, egmConfig, DEFAULT_TIME, false, games));
			
		// Setup for CPollerPlayerTransactionMessage.
		CMeters *meters(new CMeters(MEMORY_NONE));
			
		// Perform operation(s) and test(s).
		CPollerPlayerTransactionMessage pollerMsgGlobalMGMDOff(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime , 0, GSN, DENOM, GAME_NUM, 0);
		byte gsnIdOffset (112);
		
		VerifyDWordData(pollerMsgGlobalMGMDOff, gsnIdOffset, GSN, DATA_TYPE_PLAY_TRANSACTION_SEQUENCE);
	}

	/// <summary>
	/// Functional test for:
	/// 	CPollerPlayerTransactionMessage::CPollerPlayerTransactionMessage()
	/// 	CPollerPlayerTransactionMessage::GetReadWriteBuffer()
	/// Test MGMD Available in the message
	/// </summary>
	TEST_F(PollerPlayerTransactionMessage, GetReadWriteBuffer_CheckMGMDAvailabilityMGMDGlobalOn)
	{
		//No need to set this time to do basic tests on class instances.
		time_t transTime; 
		
		// Setup CPlayer for virtual card out.
		CCard *card(new CCard(DEFAULT_TIME, DEFAULT_TIME));
		card->SetCardID(_T("1234567890"));
		const CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);
		
		CGames *games = new CGames();
		CPlayer *player(new CPlayer(card, new CPlayerInfo, config, egmConfig, DEFAULT_TIME, false, games));
		
		// Setup for CPollerPlayerTransactionMessage.
		CMeters *meters(new CMeters(MEMORY_NONE));
		
		// Perform operation(s) and test(s).
		CPollerPlayerTransactionMessage pollerMsgMGMDAvailOn(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime , 0, GSN, DENOM, GAME_NUM, 1, 1);
		byte mgmdAvailOffset (118);
	
		VerifyByteData(pollerMsgMGMDAvailOn, mgmdAvailOffset, 1, DATA_TYPE_MGMD_AVAILABLE);
	}	
	/// <summary>
	/// Functional test for:
	/// 	CPollerPlayerTransactionMessage::CPollerPlayerTransactionMessage()
	/// 	CPollerPlayerTransactionMessage::GetReadWriteBuffer()
	/// Test MGMD Available in the message
	/// </summary>
	TEST_F(PollerPlayerTransactionMessage, GetReadWriteBuffer_CheckMGMDAvailabilityMGMDGlobalOff)
	{
		//No need to set this time to do basic tests on class instances.
		time_t transTime; 
			
		// Setup CPlayer for virtual card out.
		CCard *card(new CCard(DEFAULT_TIME, DEFAULT_TIME));
		card->SetCardID(_T("1234567890"));
		const CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);
			
		CGames *games = new CGames();
		CPlayer *player(new CPlayer(card, new CPlayerInfo, config, egmConfig, DEFAULT_TIME, false, games));
			
		// Setup for CPollerPlayerTransactionMessage.
		CMeters *meters(new CMeters(MEMORY_NONE));
			
		// Perform operation(s) and test(s).
		CPollerPlayerTransactionMessage pollerMsgMGMDAvailOff(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime , 0, GSN, DENOM, GAME_NUM, 1, 0);
		byte mgmdAvailOffset (118);
		
		VerifyByteData(pollerMsgMGMDAvailOff, mgmdAvailOffset, 0, DATA_TYPE_MGMD_AVAILABLE);
	}

	const time_t PollerPlayerTransactionMessage::DEFAULT_TIME(0);
}
