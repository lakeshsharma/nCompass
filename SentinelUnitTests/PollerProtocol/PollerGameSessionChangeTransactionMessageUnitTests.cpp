#include "stdafx.h"
#include "PollerProtocol/PollerGameSessionChangeTransactionMessage.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;


namespace SentinelNativeUnitTests
{
	static const DWORD PSID (12);
	static const DWORD GSN (31);
	static const WORD DENOM (11);
	static const WORD GAME_NUM (13);
	static const bool MGMD_AVAIL (true);
	static const byte MAX_CARD_LIMIT_REACHED (1);


    class PollerGameSessionChangeTransactionMessage : public::testing::Test
	{
	private:
		CPollerGameSessionChangeTransactionMessage* m_pollerMsg;
        CPlayer *m_player;
        CCard *m_card;
        CMeters *m_meters;
        CGames *m_games;
        CPlayerInfo *m_playerInfo;
		
		static const time_t DEFAULT_TIME; // Arbitrary value when any valid value is okay.

    public:
		virtual void SetUp()
		{	
			// Data.
			time_t transTime;

			// Setup CPlayer specific data
			m_card  = new CCard(DEFAULT_TIME, DEFAULT_TIME);
			m_card->SetCardID("1234567890");
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			const time_t utcRemovalTime(transTime - time_t(5));
			const time_t removalTime(utcRemovalTime + -7 * 3600);
			const bool makeVirtual(false);
			CMeters *virtualStartMeters = nullptr;

            m_games = new CGames();
            m_playerInfo = new CPlayerInfo();
			m_playerInfo->MaxCardLimitReached(true);

			//Create and setup Player object
			m_player = new CPlayer(m_card, m_playerInfo, config, egmConfig, DEFAULT_TIME, false, m_games);
			m_player->CardRemoved(removalTime, utcRemovalTime, makeVirtual);

			// Setup for CPollerGameSessionChangeTransactionMessage
			m_meters = new CMeters(MEMORY_NONE);
			int cardIDLen = 0;
			const byte * cardID = m_card->GetCardID(cardIDLen);
					
			m_pollerMsg = new CPollerGameSessionChangeTransactionMessage (m_meters, m_player, cardID, &transTime, 0, PSID, GSN, DENOM, GAME_NUM, MGMD_AVAIL);
		}

        virtual void TearDown()
        {
        }

        		
		void VerifyByteData(byte valOffset, byte actualVal, byte actualValType)
		{
			ASSERT_NE(m_pollerMsg, nullptr);

			DWORD transLength(0);
			const byte *transBuffer(m_pollerMsg->GetReadWriteBuffer(transLength));

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
		
		void VerifyWordData(byte valOffset, WORD actualVal, byte actualValType)
		{

			ASSERT_NE(m_pollerMsg, nullptr);

			DWORD transLength(0);
			const byte *transBuffer(m_pollerMsg->GetReadWriteBuffer(transLength));

			DWORD msglen = valOffset + sizeof(byte) + sizeof(byte) + sizeof(WORD); //offset + type  + length + value
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
		
		void VerifyDWordData(byte valOffset, DWORD actualVal, byte actualValType)
		{
			ASSERT_NE(m_pollerMsg, nullptr);
		
			DWORD transLength(0);
			const byte *transBuffer(m_pollerMsg->GetReadWriteBuffer(transLength));
		
			DWORD msglen = valOffset + sizeof(byte) + sizeof(byte) + sizeof(DWORD); //offset + type	+ length + value
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
	};

	/// <summary>
	/// Test Game Game number data from the message
	/// </summary>
	TEST_F(PollerGameSessionChangeTransactionMessage, GetReadWriteBuffer_CheckGameNumberData)
	{
		byte gameNumOffset (82);

		VerifyWordData(gameNumOffset, GAME_NUM, DATA_TYPE_GAME_NUMBER);
    }

	/// <summary>
	/// Test Game denom data from the message
	/// </summary>
	TEST_F(PollerGameSessionChangeTransactionMessage, GetReadWriteBuffer_CheckGameDenomData)
	{
		byte denomOffset (86);

		VerifyWordData(denomOffset, DENOM, DATA_TYPE_DENOM);

    }
		
	/// <summary>
	/// Test Play Session ID in the message
	/// </summary>
	TEST_F(PollerGameSessionChangeTransactionMessage, GetReadWriteBuffer_CheckPlaySessionIdData)
	{
		byte psIdOffset (100);
	
		VerifyDWordData(psIdOffset, PSID, DATA_TYPE_PLAY_SESSION_ID);
	}
	
	/// <summary>
	/// Test Game Session ID in the message
	/// </summary>
	TEST_F(PollerGameSessionChangeTransactionMessage, GetReadWriteBuffer_CheckGameSessionIdData)
	{
		byte gsnIdOffset (106);
	
		VerifyDWordData(gsnIdOffset, GSN, DATA_TYPE_PLAY_TRANSACTION_SEQUENCE);
	}
	
	/// <summary>
	/// Test MGMD availability data in the message
	/// </summary>
	TEST_F(PollerGameSessionChangeTransactionMessage, GetReadWriteBuffer_CheckMGMDAvailability)
	{
		byte mgmdOffset (112);
			
		VerifyByteData(mgmdOffset, MGMD_AVAIL, DATA_TYPE_MGMD_AVAILABLE);
	}

	/// <summary>
	/// Test MGMD availability data in the message
	/// </summary>
	TEST_F(PollerGameSessionChangeTransactionMessage, GetReadWriteBuffer_CheckMaxCardLimitReached)
	{
		byte maxCardLimitReachedOffset (115);
			
		VerifyByteData(maxCardLimitReachedOffset, MAX_CARD_LIMIT_REACHED, DATA_TYPE_MAX_CARD_INDICATOR);
	}
	
	const time_t PollerGameSessionChangeTransactionMessage::DEFAULT_TIME(0);
}
