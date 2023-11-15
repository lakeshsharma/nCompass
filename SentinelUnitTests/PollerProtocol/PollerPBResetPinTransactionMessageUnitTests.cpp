#include "stdafx.h"
#include "PollerProtocol/PollerPBResetPinTransactionMessage.h"
#include "Card.h"

// Unit testing includes.
#include "gtest/gtest.h"

namespace SentinelNativeUnitTests
{
	static const byte OLD_PIN_OFFSET(54);
	static const byte NEW_PIN_OFFSET(60);
	static const DWORD MSG_BUF_SIZE(66);

	class PollerPBResetPinTransactionMessage : public ::testing::Test
	{
	public:
		static const time_t DEFAULT_CTIME; // Arbitrary value when any valid value is okay.
			
		void VerifyDWordData(CPollerPBResetPinTransactionMessage& pollerMsg, byte valOffset, DWORD actualVal, byte actualValType)
		{
			DWORD transLength(0);
			const byte *transBuffer(pollerMsg.GetReadWriteBuffer(transLength));
		
			DWORD msglen = valOffset + sizeof(byte) + sizeof(byte) + sizeof(byte); //offset + type	+ length + value
			ASSERT_TRUE(transLength >= msglen);
			
		
			DWORD rcvdVal = 0;
			byte rcvdValSz = 0;
			byte rcvdValType = 0;

			byte actualValSz = sizeof(actualVal);
			
			// Test the data
			memcpy(&rcvdValType, transBuffer + valOffset, sizeof(byte));
			ASSERT_EQ (rcvdValType, actualValType);
			
			uint8_t sizeOffset = valOffset + sizeof(byte);
			memcpy (&rcvdValSz, transBuffer + sizeOffset, sizeof(byte));
			ASSERT_EQ(rcvdValSz, actualValSz);
			
			uint8_t dataOffset = sizeOffset + sizeof(byte); 
			memcpy (&rcvdVal, transBuffer + dataOffset, rcvdValSz);
			ASSERT_EQ (rcvdVal, actualVal);
		
		}				
	};
		
	/// <summary>
	/// Functional test for:
	///     CPollerPBResetPinTransactionMessage::CPollerPBResetPinTransactionMessage()
	/// Test Buffer size to hold the transaction message data.
	TEST_F(PollerPBResetPinTransactionMessage, GetReadWriteBuffer_MessageBufferSize)
	{
		int oldPin(1234);
		int newPin(4567);

		// Setup CCard.
		CCard *card(new CCard(DEFAULT_CTIME, DEFAULT_CTIME));
		card->SetCardID(_T("1234567890"));
		int cardIDLen = 0;

		// Setup for CPollerPBResetPinTransactionMessage.
		CMeters *meters(new CMeters(MEMORY_NONE));

		// Perform operation(s) and test(s).
		CPollerPBResetPinTransactionMessage pollerMsg(meters, PBT_RESET_PIN, 0, card->GetCardID(cardIDLen), oldPin, newPin);
		pollerMsg.SetReadWriteBuffer();

		DWORD bufferSizeActual = 0;
		
		//Just need buffersize from this api
		pollerMsg.GetReadWriteBuffer(bufferSizeActual);
		ASSERT_EQ(MSG_BUF_SIZE, bufferSizeActual);

	}

	/// <summary>
	/// Functional test for:
	/// 	CPollerPBResetPinTransactionMessage::GetReadWriteBuffer()
	/// Test Old Pin and New Pin in the message
	/// </summary>
	TEST_F(PollerPBResetPinTransactionMessage, GetReadWriteBuffer_VerifyOldPin_NewPin)
	{
		int oldPin(1234);
		int newPin(4567);
		
		// Setup CCard.
		CCard *card(new CCard(DEFAULT_CTIME, DEFAULT_CTIME));
		card->SetCardID(_T("1234567890"));
		int cardIDLen = 0;
				
		// Setup for CPollerPBResetPinTransactionMessage.
		CMeters *meters(new CMeters(MEMORY_NONE));
			
		// Perform operation(s) and test(s).
		CPollerPBResetPinTransactionMessage pollerMsg(meters, PBT_RESET_PIN, 0, card->GetCardID(cardIDLen), oldPin, newPin);
		pollerMsg.SetReadWriteBuffer();

		VerifyDWordData(pollerMsg, OLD_PIN_OFFSET, CUtilities::EncryptPBTPin(oldPin), DATA_TYPE_PBT_PIN);
		VerifyDWordData(pollerMsg, NEW_PIN_OFFSET, CUtilities::EncryptPBTPin(newPin), DATA_TYPE_PBT_PIN);
	}


	const time_t PollerPBResetPinTransactionMessage::DEFAULT_CTIME(0);
}

