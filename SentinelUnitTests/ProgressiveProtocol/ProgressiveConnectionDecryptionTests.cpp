
#include "stdafx.h"
#include "ProgressiveConnectionTestHelpers.h"
#include "ProgressiveProtocol/ProgressiveAnnouncementsInfo.h"
#include "ProgressiveProtocol/ProgressiveLevelBroadcast.h"
#include "ProgressiveProtocol/ProgressiveJackpotAnnouncement.h"
#include "ProgressiveProtocol/ProgressiveTotalizerAnnouncement.h"
#include <activemq/commands/ActiveMQBytesMessage.h>
#include "Platform.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace cms;
using namespace activemq;
using namespace activemq::util;
using namespace activemq::commands;

namespace SentinelUnitTests
{
	/// \class ProgressiveConnectionDecryptionTests
	/// \brief A progressive connection decryption tests.
	class ProgressiveConnectionDecryptionTests : public ::testing::Test
	{
	protected:
		FakeProgressiveConnection* DecryptTestConnection;
		FakeProgressiveProtocol* DecryptProgressiveProtocol;
		virtual void SetUp() 
		{
			CPlatform::MockInstance("", 0, "", "");
			CProgressiveConfig pc;
			const std::string certLocation("/");
			const std::string firmwareVersion("1");

			//This creates a fake progressive protocol so we can reach in and grab
			// data manipulated from connection and fake data coming in.
			DecryptProgressiveProtocol = new FakeProgressiveProtocol();

			//This creats a fake connection which lets our tests call into
			// ProgressiveConnection to test the decryption logic.
			DecryptTestConnection = new FakeProgressiveConnection(
				*DecryptProgressiveProtocol, &pc, certLocation, firmwareVersion);
		}

		virtual void TearDown()
		{
			delete(DecryptTestConnection);
			delete(DecryptProgressiveProtocol);
		}

		//sends a message to Progressive connection so we can test without running
		void SendMessageToConnection(std::string MessageToSend, std::string HeaderType)
		{
			ActiveMQBytesMessage msg;
			msg.setStringProperty(OneLinkHeader::type, HeaderType);
			msg.setBodyBytes((const unsigned char*)MessageToSend.c_str(), (int)MessageToSend.size());
			msg.reset();
			DecryptTestConnection->onMessage(&msg);
		}

		//sends a multicast frame to Progressive connection so we can test without running
		void SendMulticastToConnection(std::string FrameData)
		{
			FakeProgressiveMulticastFrame frame(pValidSystemId);
			size_t len = FrameData.length() + 1;
			char* pData = frame.GetBuffer(len);
			strncpy(pData, FrameData.c_str(), len);
			frame.Parse();

			//Send the multicast message
			DecryptTestConnection->CallOnMultiCast(frame);
		}
	};

	//Tests decrypt with the good key as the current key and a blank previous.
	TEST_F(ProgressiveConnectionDecryptionTests, ProgressiveConnectionDecryption_Decrypt)
	{
		//Send the keys through normal prog protocol sent directly from onelink
		string testConfig = "{\"CurrMulticastEncryptionSeed\":\"eBfEcK$$cNk-Y*7xnGpccb\",\"PrevMulticastEncryptionSeed\":\"\"}";
		SendMessageToConnection(testConfig, "MulticastEncryptionConfiguration");

		std::string enryptedBodyFrameData =
			"MESSAGE\nOneLinkSystemId:a35ef42d-8995-4060-befe-f09e75210a56\ncontent-type:application/vnd.onelink.v1+enc-json\ntype:LevelBroadcast\n\n7oRGUcbrUc/sTjuh3LBw0WVRO/Z725LJSpiT+LMArkhJg6+yS/hs6OCthnIUlg76+X9oWVNg/loEGzPROtzDzJ/uv0Wn2DOROYqVzx8GHely5IuDnVFpz0E7IQvTiR3ej+Zv4v3VfVzzWXcgFGhzlzFr1A/FQ0v6NeJ2r784vwytEYiqbLPtPKoMbbyBsajBc4I21/gD+8Ixqf472HT/qCU7D1FrCHkqPvzxn2vojxAwQPlMt2xgfK3vznC4luG+fxBXpkBpgerj5Zpf9nJb9bVwSFVvw1ww5Q66Qte3nBWezVpVNi1nlr8QXHyyRP+/EgX455wymSJdZDI+3MXpAA==";		
		SendMulticastToConnection(enryptedBodyFrameData);
		
		CProgressiveLevelBroadcast* levels = DecryptProgressiveProtocol->GetProgLevels();
		ASSERT_EQ(levels->m_levels.size(), 3);
		ASSERT_EQ(levels->m_levels[0].name, "Prog100");
	}

	//Tests decrypt with the good key as the current key and a blank previous.
	TEST_F(ProgressiveConnectionDecryptionTests, ProgressiveConnectionDecryption_Decrypt_multiple_keys_good_current)
	{
		//Send the keys through normal prog protocol sent directly from onelink
		string testConfig = "{\"CurrMulticastEncryptionSeed\":\"eBfEcK$$cNk-Y*7xnGpccb\",\"PrevMulticastEncryptionSeed\":\"*WJRX$r*g33o-\"}";
		SendMessageToConnection(testConfig, "MulticastEncryptionConfiguration");

		//Encrypted body frame
		const char* enryptedBodyFrameData =
			"MESSAGE\nOneLinkSystemId:a35ef42d-8995-4060-befe-f09e75210a56\ncontent-type:application/vnd.onelink.v1+enc-json\ntype:LevelBroadcast\n\n7oRGUcbrUc/sTjuh3LBw0WVRO/Z725LJSpiT+LMArkhJg6+yS/hs6OCthnIUlg76+X9oWVNg/loEGzPROtzDzJ/uv0Wn2DOROYqVzx8GHely5IuDnVFpz0E7IQvTiR3ej+Zv4v3VfVzzWXcgFGhzlzFr1A/FQ0v6NeJ2r784vwytEYiqbLPtPKoMbbyBsajBc4I21/gD+8Ixqf472HT/qCU7D1FrCHkqPvzxn2vojxAwQPlMt2xgfK3vznC4luG+fxBXpkBpgerj5Zpf9nJb9bVwSFVvw1ww5Q66Qte3nBWezVpVNi1nlr8QXHyyRP+/EgX455wymSJdZDI+3MXpAA==";
		SendMulticastToConnection(enryptedBodyFrameData);

		CProgressiveLevelBroadcast* levels = DecryptProgressiveProtocol->GetProgLevels();
		ASSERT_EQ(levels->m_levels.size(), 3);
		ASSERT_EQ(levels->m_levels[0].name, "Prog100");
	}

	//Tests decrypt with the good key as the previous key
	TEST_F(ProgressiveConnectionDecryptionTests, ProgressiveConnectionDecryption_Decrypt_multiple_keys_good_previous)
	{
		//Send the keys through normal prog protocol sent directly from onelink
		string testConfig = "{\"CurrMulticastEncryptionSeed\":\"*WJRX$r*g33o-\",\"PrevMulticastEncryptionSeed\":\"eBfEcK$$cNk-Y*7xnGpccb\"}";
		SendMessageToConnection(testConfig, "MulticastEncryptionConfiguration");

		//Encrypted body frame
		const char* enryptedBodyFrameData =
			"MESSAGE\nOneLinkSystemId:a35ef42d-8995-4060-befe-f09e75210a56\ncontent-type:application/vnd.onelink.v1+enc-json\ntype:LevelBroadcast\n\n7oRGUcbrUc/sTjuh3LBw0WVRO/Z725LJSpiT+LMArkhJg6+yS/hs6OCthnIUlg76+X9oWVNg/loEGzPROtzDzJ/uv0Wn2DOROYqVzx8GHely5IuDnVFpz0E7IQvTiR3ej+Zv4v3VfVzzWXcgFGhzlzFr1A/FQ0v6NeJ2r784vwytEYiqbLPtPKoMbbyBsajBc4I21/gD+8Ixqf472HT/qCU7D1FrCHkqPvzxn2vojxAwQPlMt2xgfK3vznC4luG+fxBXpkBpgerj5Zpf9nJb9bVwSFVvw1ww5Q66Qte3nBWezVpVNi1nlr8QXHyyRP+/EgX455wymSJdZDI+3MXpAA==";
		SendMulticastToConnection(enryptedBodyFrameData);
		
		CProgressiveLevelBroadcast* levels = DecryptProgressiveProtocol->GetProgLevels();
		ASSERT_EQ(levels->m_levels.size(), 3);
		ASSERT_EQ(levels->m_levels[0].name, "Prog100");
	}

	//Tests decrypt with the good key as the previous key
	TEST_F(ProgressiveConnectionDecryptionTests, ProgressiveConnectionDecryption_Decrypt_no_keys_passthrough)
	{
		FakeProgressiveMulticastFrame frame(pValidSystemId);

		//non-Encrypted body frame
		const char* nonEnryptedBodyFrameData =
			"MESSAGE\nOneLinkSystemId:a35ef42d-8995-4060-befe-f09e75210a56\ncontent-type:application/vnd.onelink.v1+json\ntype:LevelBroadcast\n\n{\"LevelValues\":[{\"LevelId\":7,\"Value\":100.00,\"Name\":\"Prog100\"},{\"LevelId\":9,\"Value\":200.00,\"Name\":\"Prog200\"},{\"LevelId\":11,\"Value\":300.00,\"Name\":\"Prog300\"}],\"SourceDeviceId\":1,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1679998659613}";

		SendMulticastToConnection(nonEnryptedBodyFrameData);
		
		CProgressiveLevelBroadcast* levels = DecryptProgressiveProtocol->GetProgLevels();
		ASSERT_EQ(levels->m_levels.size(), 3);
		ASSERT_EQ(levels->m_levels[0].name, "Prog100");
	}

	TEST_F(ProgressiveConnectionDecryptionTests, ProgressiveConnection_Frame_encrypted)
	{
		std::string enryptedBodyFrameData =
			"MESSAGE\nOneLinkSystemId:a35ef42d-8995-4060-befe-f09e75210a56\ncontent-type:application/vnd.onelink.v1+enc-json\ntype:LevelBroadcast\n\n7oRGUcbrUc/sTjuh3LBw0WVRO/Z725LJSpiT+LMArkhJg6+yS/hs6OCthnIUlg76+X9oWVNg/loEGzPROtzDzJ/uv0Wn2DOROYqVzx8GHely5IuDnVFpz0E7IQvTiR3ej+Zv4v3VfVzzWXcgFGhzlzFr1A/FQ0v6NeJ2r784vwytEYiqbLPtPKoMbbyBsajBc4I21/gD+8Ixqf472HT/qCU7D1FrCHkqPvzxn2vojxAwQPlMt2xgfK3vznC4luG+fxBXpkBpgerj5Zpf9nJb9bVwSFVvw1ww5Q66Qte3nBWezVpVNi1nlr8QXHyyRP+/EgX455wymSJdZDI+3MXpAA==";
		
		FakeProgressiveMulticastFrame frame(pValidSystemId);
		size_t len = enryptedBodyFrameData.length() + 1;
		char* pData = frame.GetBuffer(len);
		strncpy(pData, enryptedBodyFrameData.c_str(), len);
		frame.Parse();

		ASSERT_TRUE(frame.HasEncryptedBody());
	}

	TEST_F(ProgressiveConnectionDecryptionTests, ProgressiveConnection_Frame_non_encrypted)
	{
		std::string nonEnryptedBodyFrameData =
			"MESSAGE\nOneLinkSystemId:a35ef42d-8995-4060-befe-f09e75210a56\ncontent-type:application/vnd.onelink.v1+json\ntype:LevelBroadcast\n\n{\"LevelValues\":[{\"LevelId\":7,\"Value\":100.00,\"Name\":\"Prog100\"},{\"LevelId\":9,\"Value\":200.00,\"Name\":\"Prog200\"},{\"LevelId\":11,\"Value\":300.00,\"Name\":\"Prog300\"}],\"SourceDeviceId\":1,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1679998659613}";
		FakeProgressiveMulticastFrame frame(pValidSystemId);
		size_t len = nonEnryptedBodyFrameData.length() + 1;
		char* pData = frame.GetBuffer(len);
		strncpy(pData, nonEnryptedBodyFrameData.c_str(), len);
		frame.Parse();

		ASSERT_FALSE(frame.HasEncryptedBody());
	}

	TEST_F(ProgressiveConnectionDecryptionTests, ProgressiveConnection_Frame_encrypted_then_reset_with_non_encrypted)
	{
		std::string enryptedBodyFrameData =
			"MESSAGE\nOneLinkSystemId:a35ef42d-8995-4060-befe-f09e75210a56\ncontent-type:application/vnd.onelink.v1+enc-json\ntype:LevelBroadcast\n\n7oRGUcbrUc/sTjuh3LBw0WVRO/Z725LJSpiT+LMArkhJg6+yS/hs6OCthnIUlg76+X9oWVNg/loEGzPROtzDzJ/uv0Wn2DOROYqVzx8GHely5IuDnVFpz0E7IQvTiR3ej+Zv4v3VfVzzWXcgFGhzlzFr1A/FQ0v6NeJ2r784vwytEYiqbLPtPKoMbbyBsajBc4I21/gD+8Ixqf472HT/qCU7D1FrCHkqPvzxn2vojxAwQPlMt2xgfK3vznC4luG+fxBXpkBpgerj5Zpf9nJb9bVwSFVvw1ww5Q66Qte3nBWezVpVNi1nlr8QXHyyRP+/EgX455wymSJdZDI+3MXpAA==";

		std::string nonEnryptedBodyFrameData =
			"MESSAGE\nOneLinkSystemId:a35ef42d-8995-4060-befe-f09e75210a56\ncontent-type:application/vnd.onelink.v1+json\ntype:LevelBroadcast\n\n{\"LevelValues\":[{\"LevelId\":7,\"Value\":100.00,\"Name\":\"Prog100\"},{\"LevelId\":9,\"Value\":200.00,\"Name\":\"Prog200\"},{\"LevelId\":11,\"Value\":300.00,\"Name\":\"Prog300\"}],\"SourceDeviceId\":1,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1679998659613}";
		
		FakeProgressiveMulticastFrame frame(pValidSystemId);

		size_t len = enryptedBodyFrameData.length() + 1;
		char* pData = frame.GetBuffer(len);
		strncpy(pData, enryptedBodyFrameData.c_str(), len);
		frame.Parse();

		ASSERT_TRUE(frame.HasEncryptedBody());

		frame.Reset();
		
		len = nonEnryptedBodyFrameData.length() + 1;
		pData = frame.GetBuffer(len);
		strncpy(pData, nonEnryptedBodyFrameData.c_str(), len);
		frame.Parse();

		ASSERT_FALSE(frame.HasEncryptedBody());
	}
}