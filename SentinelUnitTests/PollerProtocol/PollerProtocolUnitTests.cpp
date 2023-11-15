#include "PollerProtocol/PollerProtocol.h"
#include "PlayerSession.h"
#include "Sentinel.h"

#include "SentinelMock.h"
#include "InternalsFrameworkMock.h"
#include "PollerProtocolMock.h"
#include "PollerProtocol/TransactionResendList.h"
#include "PollerProtocol/PollerPlayerTransactionMessage.h"
#include "PollerProtocol/PollerTicketTransactionMessage.h"
#include "PollerProtocol/PollerGameSessionChangeTransactionMessage.h"
#include "PollerProtocol/PollerGamesListMessage.h"


#include "gtest/gtest.h"

using ::testing::Mock;
using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::NiceMock;


time_t DEFAULT_TIME = 0;
namespace SentinelNativeUnitTests
{
	static const int GAME_LAST_DENOM = 12;
	static const int GAME_NUMBER = 14;

	class PollerProtocolUnitTests :public ::testing::Test
	{
	private:
		CInternalsFrameworkMock* m_internalsMock;
		CPollerProtocolMock* m_pollerProtocolMock;
		CPairingInformation m_pairingInformationMock;
		NiceMock<CSentinelMock> m_sentinelMock;

	public:
		virtual void SetUp()
		{
			string firmwareVersion;
			time_t startupTime = CUtilities::GetCurrentTime();
			m_internalsMock = new CInternalsFrameworkMock(static_cast<IProtocolManager*>(&m_sentinelMock), startupTime, false, firmwareVersion);

			m_internalsMock->Initialize();
			ON_CALL(m_sentinelMock, SendMessageToProtocol(_, _, _)).WillByDefault(Invoke(&m_sentinelMock, &CSentinelMock::SendMessageToProtocolMocked));

			m_pollerProtocolMock = new CPollerProtocolMock(m_pairingInformationMock, *m_internalsMock->GetSCFQueue());
		}

		virtual void TearDown()
		{
			if (m_pollerProtocolMock)
			{
				delete m_pollerProtocolMock;
				m_pollerProtocolMock = nullptr;
			}
		}

		DWORD GetPollerProtocolVersion()
		{
			if (m_pollerProtocolMock)
			{
				return m_pollerProtocolMock->m_packetVersion;
			}
			return 0;
		}

		CInternalMessage* SendPlayerCardTransaction(PlayerTransactionType transactionType)
		{
			if (!m_internalsMock)
				return nullptr;

			//first initiates a transfer through internals framework
			CMeters* meters(new CMeters(MEMORY_NONE));
			m_internalsMock->m_playerSession.SetLastDenom(GAME_LAST_DENOM);
			m_internalsMock->m_playerSession.SetLastGameNumber(GAME_NUMBER);

			m_internalsMock->SendPlayerTransactionToSCF(transactionType, meters);
			
			//Now capture that message from internals
			CInternalMessage* msg = m_sentinelMock.GetInternalMsg();
			ON_CALL(*m_pollerProtocolMock, AddToPollerQueue(_, _)).WillByDefault(Invoke(m_pollerProtocolMock, &CPollerProtocolMock::AddToPollerQueueMocked));

			//Pass that captured message to Poller protocol
			switch (transactionType) {
			case PTT_CARD_IN:
				m_pollerProtocolMock->SendCardInToPoller(msg);
				break;
			case PTT_CARD_OUT:
				m_pollerProtocolMock->SendCardOutToPoller(msg);
				break;
			case PTT_UPDATE:
				m_pollerProtocolMock->SendPlayerUpdateToPoller(msg);
				break;
			default:
				return nullptr;
			}
			return msg;
		}

		bool SendPlayerTicketTransaction()
		{
			if (!m_internalsMock)
				return false;

			//first initiates a transfer through internals framework
			CTicket* ticket = new CTicket();
			m_internalsMock->SendTicketInFinalToSCF(ticket);

			//Now capture that message from internals
			CInternalMessage* msg = m_sentinelMock.GetInternalMsg();
			ON_CALL(*m_pollerProtocolMock, AddToPollerQueue(_, _)).WillByDefault(Invoke(m_pollerProtocolMock, &CPollerProtocolMock::AddToPollerQueueMocked));

			//Pass that captured message to Poller protocol
			m_pollerProtocolMock->SendTicketTransactionToPoller(msg);

			return true;
		}

		CInternalMessage* SendGameSessionChangeTransaction()
		{
			if (!m_internalsMock)
				return nullptr;

			//Set factors to enable MGMD				
			m_internalsMock->m_config.SetMGMDEnabled(true);
			m_internalsMock->m_EGMConfig.SetEGMPollingSpeed(MS40);
			m_internalsMock->m_playerSession.GameStarted();
			m_internalsMock->m_playerSession.GameEnded();

			m_internalsMock->m_playerSession.IncrementPlayTransactionSequenceNumber();
			m_internalsMock->m_playerSession.SetLastDenom(GAME_LAST_DENOM);
			m_internalsMock->m_playerSession.SetLastGameNumber(GAME_NUMBER);

			//first initiates a transfer through internals framework	
			CInternalMessage* gsmsg = m_internalsMock->PrepareGameSessionChangeTransactionMsg();

			if (gsmsg != nullptr)
			{
				std::cout << "Sending Game Session Change message (127) to Poller Protocol" << std::endl;
				m_internalsMock->SendTransactionMessageToSCF(gsmsg, QUEUE_PRIORITY_NORMAL);
			}

			//Now capture that message from internals
			CInternalMessage* msg = m_sentinelMock.GetInternalMsg();
			ON_CALL(*m_pollerProtocolMock, AddToPollerQueue(_, _)).WillByDefault(Invoke(m_pollerProtocolMock, &CPollerProtocolMock::AddToPollerQueueMocked));

			//Pass that captured message to Poller protocol
			m_pollerProtocolMock->SendGameSessionChange(msg);

			return gsmsg;
		}

		DWORD GetPlayTransactionSequenceNumber()
		{

			if (!m_internalsMock)
				return (DWORD)-1;

			return m_internalsMock->m_playerSession.GetPlayTransactionSequenceNumber();
		}

		CPollerMessage* GetPollerResponse()
		{
			return m_pollerProtocolMock->GetPollerMsg();
		}

		bool GetVoucherSeparation(void)
		{
			return m_pollerProtocolMock->VoucherSeparationEnabled();
		}

		void SetVoucherSeparationEnabled(bool val)
		{
			return m_pollerProtocolMock->SetVoucherSeparationEnabled(val);
		}

		bool IsTranstoSendToPollerIfPMTDown(byte* item)
		{
			return m_pollerProtocolMock->IsTranstoSendToPollarIfPMTDown(item);
		}

		bool IsTranstoSendToPollerIfPMTDown(CPollerMessage* msg)
		{
			return m_pollerProtocolMock->IsTranstoSendToPollarIfPMTDown(msg);
		}

		void SetReadWriteBuffer(CPollerPlayerTransactionMessage* msg)
		{
			return msg->SetReadWriteBuffer();
		}

		void SetMessageHead(CPollerTicketTransactionMessage* msg)
		{
			msg->MessageHeader.PacketVersion = POLLER_PROTOCOL_VERSION;
			msg->MessageHeader.SequenceNumber = 1;

			msg->MessageHeader.SlotMastID = 23;
			msg->MessageHeader.SlotmastRevision = 23;
		}

		CPollerGamesListMessage* GetGameListMessage(CParseGameList* gameList, uint16_t gamesCount)
		{
			CPollerGamesListMessage* pollerGamesListMessage = nullptr;

			if (nullptr != gameList)
			{
				CGames* games = new CGames(MEMORY_NONE);
				CGame* game;

				int game_index;
				for (game_index = 1; game_index <= gamesCount; game_index++)
				{
					game = new CGame(MEMORY_NONE);
					games->AddGame(game);
					game->SetGameNumber(game_index);
					game->SetDenom((WORD)1, US_DOLLARS);
					game->SetMfgID("1234");
					game->SetPaytableID("xyzw");
					game->Enable();
					game->SetTheoreticalPayback(9492);
					game->SetGamenameLength((byte)5);
					game->SetGameName("xxxx");
					game->SetAdditionalInfo("ab");
				}
				gameList->ParseDenomGames(games);
				gameList->ParseGamesRange(games, 0, gamesCount);
				time_t currentTime = CUtilities::GetCurrentTime();
				pollerGamesListMessage = new CPollerGamesListMessage(gameList, currentTime, 0, gamesCount, 0, 0, 1, 10);
			}
			return pollerGamesListMessage;
		}

		void SendGameListMessageToPoller()
		{
			//set up
			CParseGameList* gameList(new CParseGameList());
			uint16_t gamesCount(10);
			CPollerGamesListMessage* pollerGamesListMessage = GetGameListMessage(gameList, gamesCount);

			m_pollerProtocolMock->SetMessageHeader(pollerGamesListMessage);

			DWORD transLength = 0;
			pollerGamesListMessage->GetReadWriteBuffer(transLength);
			ASSERT_TRUE(transLength == 431);
			
			delete gameList;
			delete pollerGamesListMessage;
		}

		void ValidateBadTransactionResendCount_TestForBadPacket()
		{
			// uniqueId packet.
			DWORD uniqueId = 101;

			// Add this packet into m_BadTxResendCountMap map to mark it as a bad packet.
			// We assume that sometime ago this bad packet was generated.
			m_pollerProtocolMock->m_BadTxResendCountMap[uniqueId] = 1;

			// Also add the same packet into m_transactionResendList which contains all the packets.
			m_pollerProtocolMock->m_transactionResendList = new CTransactionResendList(false);
			m_pollerProtocolMock->m_transactionResendList->AddTransaction(PLAYER_CARD_IN_CODE, uniqueId, NULL, 0, false, false);

			// perform test : ValidateBadTransactionResendCount() call
			ASSERT_TRUE(m_pollerProtocolMock->ValidateBadTransactionResendCount(uniqueId)) << "resend_count = 1";

			// increase the resend count considering that we tried sending this packet to poller.
			m_pollerProtocolMock->m_BadTxResendCountMap[uniqueId]++;

			// perform test : ValidateBadTransactionResendCount() call
			// Here the resend count is 2, so the function will return false, and will remove the packet from the map and the transaction list.
			ASSERT_FALSE(m_pollerProtocolMock->ValidateBadTransactionResendCount(uniqueId)) << "resend_count = 2";

			// make sure the packet is not present in the map.
			ASSERT_EQ(m_pollerProtocolMock->m_BadTxResendCountMap.find(uniqueId), m_pollerProtocolMock->m_BadTxResendCountMap.end()) << "Packet not removed.";

			// clean up
			delete m_pollerProtocolMock->m_transactionResendList;
			m_pollerProtocolMock->m_transactionResendList = NULL;
		}

		void ValidateBadTransactionResendCount_TestForValidPacket()
		{
			// uniqueId packet.
			DWORD uniqueId = 102;

			// Since the packet is not added to m_BadTxResendCountMap, so it is not a bad packet.
			// That means every ValidateBadTransactionResendCount() call for this uniqueId will return true.

			// perform test : ValidateBadTransactionResendCount() call
			ASSERT_TRUE(m_pollerProtocolMock->ValidateBadTransactionResendCount(uniqueId)) << "first call";
			ASSERT_TRUE(m_pollerProtocolMock->ValidateBadTransactionResendCount(uniqueId)) << "second call";
			ASSERT_TRUE(m_pollerProtocolMock->ValidateBadTransactionResendCount(uniqueId)) << "third call";
		}

		void LoadBalancing_CheckForReconnection()
		{
			// setup : make PollerCommunicator instance connected & idle.
			m_pollerProtocolMock->pc->_connected = true;
			m_pollerProtocolMock->pc->_reading = false;
			m_pollerProtocolMock->pc->_sending = false;

			// setup : turn on the load balancing flag.
			m_pollerProtocolMock->m_performLoadBalancing = true;

			// mocking ConnectToPoller()
			ON_CALL(*m_pollerProtocolMock, ConnectToPoller()).WillByDefault(Invoke(m_pollerProtocolMock, &CPollerProtocolMock::ConnectToPollerMocked));

			// perform test
			m_pollerProtocolMock->CheckForConnect();

			// verification
			ASSERT_TRUE(m_pollerProtocolMock->IsConnectToPollerCalled()) << "ConnectToPoller() was not called.";
		}

		void RemoteConfigUpdate_CheckForReconnection()
		{
			// setup : make PollerCommunicator instance connected & idle.
			m_pollerProtocolMock->pc->_connected = true;
			m_pollerProtocolMock->pc->_reading = false;
			m_pollerProtocolMock->pc->_sending = false;

			// setup : create CConnection instance.
			m_pollerProtocolMock->m_connection = new CConnection();

			// setup : make a new poller IP available.
			m_pollerProtocolMock->m_NewPollerIP = "192.168.1.1";

			// mocking ConnectToPoller()
			ON_CALL(*m_pollerProtocolMock, ConnectToPoller()).WillByDefault(Invoke(m_pollerProtocolMock, &CPollerProtocolMock::ConnectToPollerMocked));

			// perform test
			m_pollerProtocolMock->CheckForConnect();

			// verification
			ASSERT_TRUE(m_pollerProtocolMock->IsConnectToPollerCalled()) << "ConnectToPoller() was not called.";
			ASSERT_TRUE(m_pollerProtocolMock->m_NewPollerIP.size() == 0) << "m_NewPollerIP string is not empty.";
		}
	};

#if 000//TODO-SEGMENTATION FAULT yocto in jenkins
	/// <summary> 
	/// Verify session id from the card in message passed to poller.
	/// </summary>
	TEST_F(PollerProtocolUnitTests, SendGameListToPoller)
	{
		SendGameListMessageToPoller();
	}


	/// <summary> 
	/// Verify session id from the card in message passed to poller.
	/// </summary>
	TEST_F(PollerProtocolUnitTests, SendCardInToPoller)
	{
		CInternalMessage* msg = SendPlayerCardTransaction(PTT_CARD_IN);
		ASSERT_NE(msg, nullptr);

		CPollerMessage* pollerMsg = GetPollerResponse();
		CPollerPlayerTransactionMessage* pollerTxMsg = dynamic_cast<CPollerPlayerTransactionMessage*>(GetPollerResponse());
		DWORD psid = pollerTxMsg->GetPlaySessionID();

		ASSERT_EQ(psid, SESSION_ID_DEFAULT);
	}

	/// <summary>
	/// Verify session id from the card out message passed to 
	/// poller. 
	/// </summary>
	TEST_F(PollerProtocolUnitTests, SendCardOutToPoller)
	{
		CInternalMessage* msg = SendPlayerCardTransaction(PTT_CARD_OUT);
		ASSERT_NE(msg, nullptr);

		CPollerMessage* pollerMsg = GetPollerResponse();

		CPollerPlayerTransactionMessage* pollerTxMsg = dynamic_cast<CPollerPlayerTransactionMessage*>(GetPollerResponse());
		ASSERT_NE(pollerTxMsg, nullptr);

		ASSERT_EQ(pollerTxMsg->GetPlaySessionID(), SESSION_ID_DEFAULT);
	}

	/// <summary>
	/// Verify session id from the player update message passed to 
	/// poller. 
	/// </summary>
	TEST_F(PollerProtocolUnitTests, SendPlayerUpdateToPoller)
	{
		CInternalMessage* msg = SendPlayerCardTransaction(PTT_UPDATE);
		ASSERT_NE(msg, nullptr);

		CPollerMessage* pollerMsg = GetPollerResponse();

		CPollerPlayerTransactionMessage* pollerTxMsg = dynamic_cast<CPollerPlayerTransactionMessage*>(GetPollerResponse());
		ASSERT_NE(pollerTxMsg, nullptr);

		ASSERT_EQ(pollerTxMsg->GetPlaySessionID(), SESSION_ID_DEFAULT);
	}

	/// <summary>
	/// Verify session id from the ticket transaction message passed
	/// to poller. 
	/// </summary>
	TEST_F(PollerProtocolUnitTests, SendTicketTransactionToPoller)
	{
		ASSERT_TRUE(SendPlayerTicketTransaction());

		CPollerMessage* pollerMsg = GetPollerResponse();

		CPollerTicketTransactionMessage* pollerTxMsg = dynamic_cast<CPollerTicketTransactionMessage*>(GetPollerResponse());
		ASSERT_NE(pollerTxMsg, nullptr);

		ASSERT_EQ(pollerTxMsg->GetPlaySessionID(), SESSION_ID_DEFAULT);
	}

	/// <summary>
	/// Verify play session tx number updated when player update message passed to poller. 
	/// </summary>
	TEST_F(PollerProtocolUnitTests, VerifyPlaySessionTxNumber)
	{
		DWORD old_pstn = GetPlayTransactionSequenceNumber();
		ASSERT_NE(old_pstn, (DWORD)-1);

		CInternalMessage* msg = SendPlayerCardTransaction(PTT_UPDATE);
		ASSERT_NE(msg, nullptr);

		DWORD new_pstn = GetPlayTransactionSequenceNumber();
		ASSERT_EQ(new_pstn, old_pstn + 1);
	}

	/*TODO-PORT
	/// <summary>
	/// Verify ptsn, gameNumber, denom, psid field from the
	/// PlayerCardTransaction message to be sent to poller.
	/// </summary>
	TEST_F(PollerProtocolUnitTests, TestSendPlayerCardTransactionFields)
	{
		CInternalMessage* msg = SendPlayerCardTransaction(PTT_UPDATE);
		ASSERT_NE(msg, nullptr);

		int gameNumber	= msg->GetIntData();
		int denom		= msg->GetIntData(true);
		DWORD ptsn		= msg->GetDWordData();
		DWORD psid		= msg->GetDWordData(true);

		ASSERT_EQ(ptsn, CPlayerSession::PLAY_TX_SEQ_ID_DEFAULT);
		ASSERT_EQ(psid, SESSION_ID_DEFAULT); //session id is already incremented
		ASSERT_EQ(gameNumber, GAME_NUMBER);
		ASSERT_EQ(denom, GAME_LAST_DENOM);
	}

	/// <summary>
	/// Verify ptsn, gameNumber, denom, psid, mgmdAvailable field
	/// from the GameSessionChangeTransaction message to be sent to
	/// poller.
	/// </summary>
	TEST_F(PollerProtocolUnitTests, SendGameSessionChangeToPoller)
	{
		CInternalMessage* msg = SendGameSessionChangeTransaction();
		ASSERT_NE(msg, nullptr);

		DWORD ptsn		= msg->GetDWordData();
		int gameNumber	= msg->GetIntData();
		int denom		= msg->GetIntData(true);
		DWORD psid		= msg->GetDWordData(true);
		byte mgmdAvailable = (byte) msg->GetWordData();

		ASSERT_EQ(ptsn, CPlayerSession::PLAY_TX_SEQ_ID_DEFAULT);
		ASSERT_EQ(psid, SESSION_ID_DEFAULT + 1); //session id is already incremented
		ASSERT_EQ(gameNumber, GAME_NUMBER);
		ASSERT_EQ(denom, GAME_LAST_DENOM);
		ASSERT_TRUE(mgmdAvailable);
	}
	*/

	/// <summary>
	/// Verify Voucher separation flag value
	/// </summary>
	TEST_F(PollerProtocolUnitTests, GetVoucherSeparartionFlag)
	{
		//Test the value of default value of flag
		ASSERT_FALSE(GetVoucherSeparation()) << "Voucher separation default value true";

		//Test the getter function for voucher separation flag
		SetVoucherSeparationEnabled(true);
		ASSERT_TRUE(GetVoucherSeparation()) << "Voucher separation value false";
	}

	/// <summary>
	/// Checks if the voucher separation service enabled on poller client , so ticket transaction is needed to send to the poller
	///if PMT is down
	/// </summary>
	TEST_F(PollerProtocolUnitTests, IsTicketTransSentToPollerIfPMTDown)
	{
		//Assume that pollar sends feature flag with voucher separation flag enabled
		SetVoucherSeparationEnabled(true);

		CMeters* meters = (new CMeters(MEMORY_NONE));
		CTicket* ticket = (new CTicket());
		byte transType = TICKET_CODE;
		const byte* cardID = reinterpret_cast<const byte*>("1234567890");

		CPollerTicketTransactionMessage pollerMsg(meters, ticket, transType, 0, cardID);
		SetMessageHead(&pollerMsg);
		pollerMsg.SetReadWriteBuffer();
		DWORD transLength(0);
		//Get the lp_buffer, which is used as resend item
		byte* transBuffer(pollerMsg.GetReadWriteBuffer(transLength));

		ASSERT_TRUE(IsTranstoSendToPollerIfPMTDown(transBuffer)) << "Ticket transaction not sent to pollar when pmt down";
	}

	/// <summary>
	/// Checks if ticket transaction sent to the poller
	///if PMT is down
	/// </summary>
	TEST_F(PollerProtocolUnitTests, IsTicketTransSentToPollerIfPMTDownPollerMssg)
	{
		SetVoucherSeparationEnabled(true);
		CMeters* meters = (new CMeters(MEMORY_NONE));
		CTicket* ticket = (new CTicket());
		byte transType = TICKET_CODE;
		const byte* cardID = reinterpret_cast<const byte*>("1234567890");

		CPollerTicketTransactionMessage pollerMsg(meters, ticket, transType, 0, cardID);
		SetMessageHead(&pollerMsg);
		pollerMsg.SetReadWriteBuffer();

		DWORD transLength(0);
		byte* transBuffer(pollerMsg.GetReadWriteBuffer(transLength));

		ASSERT_TRUE(IsTranstoSendToPollerIfPMTDown(&pollerMsg)) << "Ticket transaction not sent to pollar when pmt down";
	}

	/// <summary>
   /// Checks if non-ticket transaction sent to the poller
   ///if PMT is down
   /// </summary>
	TEST_F(PollerProtocolUnitTests, IsPlayerTransNotSentToPollerIfPMTDown)
	{
		SetVoucherSeparationEnabled(true);
		const int activeSecondsValueOffset(84); // Offset into transBuffer.
		const __int64 activeSecondsExpected(60);
		const time_t lastSessionStartTime(CUtilities::GetTimeFromFields(2015, 10, 20, 12, 23, 34));
		time_t transTime(lastSessionStartTime + activeSecondsExpected); // Need read/write value.

		// Setup CPlayer for physical card out with no virtual session.
		CCard* card(new CCard(lastSessionStartTime, lastSessionStartTime));
		card->SetCardID(_T("1234567890"));
		const CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);

		const time_t utcRemovalTime(transTime - time_t(5));
		const bool makeVirtual(false);
		CMeters* virtualStartMeters(nullptr);

		CGames* games = new CGames();
		CPlayer* player(new CPlayer(card, new CPlayerInfo, config, egmConfig, lastSessionStartTime, false, games));
		player->CardRemoved(utcRemovalTime, utcRemovalTime, makeVirtual);
		ASSERT_EQ(lastSessionStartTime, player->GetLastSessionStartTime()); // Verify setup.

		CMeters* meters(new CMeters(MEMORY_NONE));

		// Perform operation(s) and test(s).
		CPollerPlayerTransactionMessage pollerMsg(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime, 0);
		SetReadWriteBuffer(&pollerMsg);
		DWORD transLength(0);
		byte* transBuffer(pollerMsg.GetReadWriteBuffer(transLength));

		ASSERT_FALSE(IsTranstoSendToPollerIfPMTDown(transBuffer)) << "Non Ticket Transaction sent when PMT down through resend queue";
	}

	/// <summary>
	/// Checks if non-ticket transaction is not to send to the 
	/// poller if PMT is down 
	 /// </summary>
	TEST_F(PollerProtocolUnitTests, IsPlayerTransNotSentToPollerIfPMTDownPollerMssg)
	{
		SetVoucherSeparationEnabled(true);
		const int activeSecondsValueOffset(84); // Offset into transBuffer.
		const __int64 activeSecondsExpected(60);
		const time_t lastSessionStartTime(CUtilities::GetTimeFromFields(2015, 10, 20, 12, 23, 34));
		time_t transTime(lastSessionStartTime + activeSecondsExpected); // Need read/write value.

		// Setup CPlayer for physical card out with no virtual session.
		CCard* card(new CCard(lastSessionStartTime, lastSessionStartTime));
		card->SetCardID(_T("1234567890"));
		const CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);

		const time_t utcRemovalTime(transTime - time_t(5));
		const bool makeVirtual(false);
		CMeters* virtualStartMeters(nullptr);

		CGames* games = new CGames();
		CPlayer* player(new CPlayer(card, new CPlayerInfo, config, egmConfig, lastSessionStartTime, false, games));
		player->CardRemoved(utcRemovalTime, utcRemovalTime, makeVirtual);
		ASSERT_EQ(lastSessionStartTime, player->GetLastSessionStartTime()); // Verify setup.

		CMeters* meters(new CMeters(MEMORY_NONE));

		// Perform operation(s) and test(s).
		CPollerPlayerTransactionMessage pollerMsg(player, meters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime, 0);
		SetReadWriteBuffer(&pollerMsg);
		DWORD transLength(0);
		byte* transBuffer(pollerMsg.GetReadWriteBuffer(transLength));

		ASSERT_FALSE(IsTranstoSendToPollerIfPMTDown(&pollerMsg)) << "Non Ticket Transaction sent when PMT down though poller message";
	}

	/// <summary>
	/// Checks the correct functioning of PollerProtocolUnitTests::ValidateBadTransactionResendCount() method for a bad packet.
	/// </summary>
	TEST_F(PollerProtocolUnitTests, ValidateBadTransactionResendCount_TestForBadPacket)
	{
		ValidateBadTransactionResendCount_TestForBadPacket();
	}

	/// <summary>
	/// Checks the correct functioning of PollerProtocolUnitTests::ValidateBadTransactionResendCount() method for a valid packet.
	/// </summary>
	TEST_F(PollerProtocolUnitTests, ValidateBadTransactionResendCount_TestForValidPacket)
	{
		ValidateBadTransactionResendCount_TestForValidPacket();
	}

	/// <summary> 
	/// Verify that when load balancing flag is on, then ConnectToPoller() was called.
	/// </summary>
	TEST_F(PollerProtocolUnitTests, LoadBalancing_CheckForReconnection)
	{
		LoadBalancing_CheckForReconnection();
	}

	/// <summary> 
	/// Verify that when new poller IP is available, then ConnectToPoller() was called.
	/// </summary>
	TEST_F(PollerProtocolUnitTests, RemoteConfigUpdate_CheckForReconnection)
	{
		//TODO-PORT RemoteConfigUpdate_CheckForReconnection();
	}

#endif
};


