#include <activemq/library/ActiveMQCPP.h> //TODO-PORT Move third-party includes to stdafx.h
#include <decaf/lang/System.h> //TODO-PORT Move third-party includes to stdafx.h
#include <activemq/core/ActiveMQConnectionFactory.h> //TODO-PORT Move third-party includes to stdafx.h
#include <activemq/core/ActiveMQConnection.h> //TODO-PORT Move third-party includes to stdafx.h
#include "stdafx.h"
#include "ProgressiveConnection.h"

#include "Diagnostics.h"
#include "Hardware/WatchDog.h"
#include "Sentinel.h"
#include "Utilities.h"
#include "RapidJsonHelper.h"
#include "ProgressiveKeys.h"
#include "Logging/LogString.h"
#include "Stomp/StompHelper.h"
#include "Util/Convert.h"
#include "ProgressiveOfferRequest.h"
#include "ProgressiveJackpotAnnouncement.h"
#include "ProgressiveTotalizerAnnouncement.h"
#include "DashboardIdentifiers.h"

using namespace activemq::core;
using namespace std;

const long RECONNECT_MILLISECONDS = 10000;	// 10 seconds
const long REDISCOVERY_MILLISECONDS = 180000;	// 3 minutes
// The extra 500 milliseconds ensures that the ping interval and
// AddressCheck interval never line up.
const long ADDRESS_CHECK_MILLISECONDS = 600500;	// 10 minutes + 500 milliseconds
const long INDISCOVERRETRY_MILLISECONDS = 30000;	// 30 seconds
const DWORD RESEND_TRANSACTION_INTERVAL = 15000; // Allow 15 seconds before resend kicks in

const long MAX_CONNECT_ATTEMPTS = 5;		// The maximum number of connect attempts
const long CONNECT_TIMEOUT = 60000;	// 60 seconds
const long SEND_TIMEOUT = 30000;	// 30 seconds
const long CLOSE_TIMEOUT = 10000;	// 10 seconds


bool CProgressiveConnection::m_multicastSocketFailed = false;

CProgressiveConnection::CProgressiveConnection(
	CProgressiveDataConsumer& dataConsumer,
	CProgressiveConfig* progConfig,
	const std::string& paltronicsCertificatePathname,
	const std::string& firmwareVersion,
	IOperatingSystem& operatingSystem) :
	m_paltronicsCertificatePathname("/etc/ssl/certs/ca-certificates.crt"),
	m_hostManager(NULL),
	m_messageFactory(NULL),
	m_games(NULL),
	m_player(NULL),
	m_dependentCommunicationsOK(false),
	m_campaignId(-1),
	m_campaignRevision(-1),
	m_skinId(-1),
	m_skinRevision(-1),
	m_pendingAction(PendingAction::NonePending),
	m_currentActivity(CurrentActivity::None),
	m_progressiveDataConsumer(dataConsumer),
	m_operatingSystem(operatingSystem),
	m_firmwareVersion(firmwareVersion),
	m_connection(NULL),
	m_session(NULL),
	m_sessionActive(false),
	m_sessionId(0),
	m_multicastActive(DefaultMulticastActive),
	m_haveGameData(false),
	m_currentGameNumber((int)CGame::DEFAULT_GAME_NUMBER),
	m_currentDenomination(CGame::DEFAULT_DENOMINATION_MULTIPLIER),
	m_consumerDestination(NULL),
	m_producerDestination(NULL),
	m_consumer(NULL),
	m_producer(NULL),
	m_performDisconnect(false),
	m_disconnecting(false)

{
  #ifdef LEAK_CHECK
  // The ActiveMQ library allocates several initial one-time structures from the heap that it does not release.
  // Decrementing track_heap_allocations hides these allocations from the leak report.
  //track_heap_allocations--; // disable heap tracking temporarily
  LockHeapAndDisableTracking();

  //track_heap_allocations++; // re-enable heap tracking
  UnlockHeapAndReenableTracking();
  #endif

	m_aes128EncryptionUtil = new CAES128EncryptionUtil();
	m_transactionResendList = new CProgressiveTransactionResendList(true);
	m_uniqueId = m_transactionResendList->GetHighestUniqueId();
	LoadProgressiveConfig(progConfig);

	if (progConfig != NULL && progConfig->GetActive())
	{
		m_connectionInfo.SetStartupTimestamp();
		UpdateConnectionState(WaitingForInfo);
	}
	else
	{
		UpdateConnectionState(Disabled);
	}

	SendHostUpdate();
}

CProgressiveConnection::~CProgressiveConnection()
{
	ProgressiveMulticastHost::CloseChannels();
	m_progressiveDataConsumer.HostUpdate(NULL);
	ClearPlayer();
	Disconnect();

	// See if we were able to disconnect.
	if (m_connectionInfo.GetState() == Connected || m_connectionInfo.GetState() == InDiscovery)
	{
		LogString(ERROR_LOG, "Could not disconnect before shutdown of progressive connection");
	}
	else
	{
		UpdateConnectionState(Disabled);
	}

	delete m_hostManager;
	m_hostManager = nullptr;

	delete m_messageFactory;
	m_messageFactory = nullptr;

	delete m_transactionResendList;
	m_transactionResendList = nullptr;

	delete m_aes128EncryptionUtil;
	m_aes128EncryptionUtil = nullptr;
}

void CProgressiveConnection::LoadProgressiveConfig(CProgressiveConfig *progConfig)
{
	if (progConfig != NULL)
	{
		LogString(PROGRESSIVE, "Loading progressive config");

		// Set-up the host manager.
		std::vector<std::string> endPoints;
		for (int eIndex = 0; eIndex < progConfig->GetEndpointCount(); eIndex++)
		{
			endPoints.push_back(progConfig->GetEndpoint(eIndex));
		}

        std::unique_lock<std::recursive_mutex> lock(m_criticalSection);
		delete m_hostManager;
		m_hostManager = new CProgressiveHostManager(endPoints, progConfig->GetDNSLookup());
		lock.unlock();

		DWORD pingInterval = progConfig->GetPingInterval();
		// Round ping interval to nearest second. This reduces the change of an
		// ICardStatus and CheckAddress from occurring within a half second of each other.
		m_connectionInfo.SetPingInterval((pingInterval / 1000) * 1000);

		SendHostUpdate();
	}
}

void CProgressiveConnection::LoadDeviceConnectionData(CConnection *connection)
{
	// When we get the connection data, this is when we load the message factory.
	if (connection != nullptr)
	{
    	 std::unique_lock<std::recursive_mutex> lock(m_criticalSection);
		if (m_messageFactory == nullptr)
		{
			LogString(PROGRESSIVE, "Received connection data. Instantiating message factory.");
			m_messageFactory = new CProgressiveMessageFactory(*connection, m_firmwareVersion);
		}
		else
		{
			string newIP = connection->GetActualIP().GetIP();
			if (m_messageFactory->SetActualIP(newIP))
			{
				SendCardConfigurationRequest();
			}
		}
        lock.unlock();
	}
}

void CProgressiveConnection::SetGames(CGames *games, CGameEndData *gameData)
{
	 std::unique_lock<std::recursive_mutex> lock(m_criticalSection);
	 LogString(GAMES, "%s %d", __FUNCTION__, __LINE__);
	if (NULL != games)
	{
		LogString(GAMES, "%s %d", __FUNCTION__, __LINE__);
		delete m_games;
		m_games = new CGames(*games);
	}

	UpdateGameData(gameData);
    lock.unlock();
}

void CProgressiveConnection::TimedEvents()
{
	std::unique_lock<std::recursive_mutex> lock(m_criticalSection);
	
	if ((m_messageFactory == NULL || m_games == NULL) && m_connectionInfo.GetState() != WaitingForInfo)
	{
		UpdateConnectionState(WaitingForInfo);
	}

	if (m_connectionInfo.GetState() == WaitingForInfo)
	{
		if (m_messageFactory != NULL && m_games != NULL)
		{
			LogString(PROGRESSIVE, "No longer waiting for games and connection info. Set state to disconnected");
			UpdateConnectionState(Disconnected);
		}
	}

	lock.unlock();

	WatchdogCheckIn();

	lock.lock();

	// See if it is time to make a connection
	if (IsTimeForConnectionAttempt())
	{
		LogString(PROGRESSIVE, "Time to make a connection attempt");
		Connect();
	}

	// Restart connection process if in discover for more than 30 seconds
	if (m_connectionInfo.GetState() == InDiscovery && CUtilities::GetMillisecondsSinceTimestamp(m_connectionInfo.GetLastConnectAttemptTimestamp()) > INDISCOVERRETRY_MILLISECONDS)
	{
		UpdateConnectionState(WaitingForInfo);
	}

	lock.unlock();

	WatchdogCheckIn();

	// See if we need to start the discovery process
	CheckDiscoveryStart();

	lock.lock();
	// See if it is time to send a ping
	bool isTimeForPing = IsTimeForPing();
	lock.unlock();

	if (isTimeForPing)
	{
		LogString(PROGRESSIVE, "Time to ping");
		SendCardStatus();
	}

	WatchdogCheckIn();

	// Check if time to attempt send of resend list
	CheckResendList();

	// See if we are using our preferred address
	AddressCheck();

	WatchdogCheckIn();

	if (m_performDisconnect || m_multicastSocketFailed)
	{
		Disconnect();
	}

	WatchdogCheckIn();
}

void CProgressiveConnection::WatchdogCheckIn()
{
	CWatchDog::Instance().Checkin();
}

void CProgressiveConnection::CheckDiscoveryStart()
{
	 std::unique_lock<std::recursive_mutex> lock(m_criticalSection);
	// Discovery needs to be started if currently connected and 
	//  need to populate the factory or it has been too long since a successful connection.
	if (m_connectionInfo.GetState() == InitiateDiscovery && (!IsFactoryPopulatedFromDiscovery() || IsTimeForRediscovery()))
	{
		StartDiscovery();

	}
}

void CProgressiveConnection::StartDiscovery()
{
	if (m_messageFactory != NULL && m_games != NULL)
	{
		LogString(PROGRESSIVE, "Discovery started");

		m_messageFactory->ClearDiscoveryData();
		UpdateConnectionState(InDiscovery);
		SendCardConfigurationRequest();
	}
}

bool CProgressiveConnection::IsFactoryPopulatedFromDiscovery()
{
	return (m_messageFactory != NULL && m_messageFactory->IsPopulatedFromDiscovery());
}

bool CProgressiveConnection::IsTimeForConnectionAttempt()
{
	return ((m_connectionInfo.GetState() == Error || m_connectionInfo.GetState() == Disconnected) &&
		CUtilities::GetMillisecondsSinceTimestamp(m_connectionInfo.GetLastConnectAttemptTimestamp()) >= RECONNECT_MILLISECONDS);
}

void CProgressiveConnection::AddressCheck()
{
	std::unique_lock<std::recursive_mutex> lock(m_criticalSection);
	// Check to see if it is time to check if this is the IP address we should be using.
	if (IsTimeForAddressCheck() && m_hostManager != NULL)
	{
		LogString(PROGRESSIVE, "Performing a host address check");

		// Set the timestamp
		m_connectionInfo.SetLastAddressUpdateTimestamp();

		// Check the address
		// NOTE: This might perform a DNS lookup.
		if (m_hostManager->AddressCheck())
		{
			LogString(PROGRESSIVE, "Host address has changed. Disconnect to re-connection can occur.");

			// Our address has changed, so disconnect
			// The next time this function is called, we will reconnect.
			Disconnect();
		}

		WatchdogCheckIn();
		SendHostUpdate();
		m_progressiveDataConsumer.ConnectionUpdate(m_connectionInfo);
	}
}

bool CProgressiveConnection::IsTimeForAddressCheck()
{
	return (CUtilities::GetMillisecondsSinceTimestamp(m_connectionInfo.GetLastAddressUpdateTimestamp()) >= ADDRESS_CHECK_MILLISECONDS ||
		(m_connectionInfo.GetState() == Error && m_connectionInfo.GetLastError() == CouldNotFindServer));
}

bool CProgressiveConnection::IsTimeForRediscovery()
{
	return (CUtilities::GetMillisecondsSinceTimestamp(m_connectionInfo.GetLastMessageTimestamp()) >= REDISCOVERY_MILLISECONDS);
}

bool CProgressiveConnection::IsTimeForPing()
{
	return m_connectionInfo.IsTimeForPing(m_operatingSystem.GetTickCount32());
}

void CProgressiveConnection::SetLastHeartbeatTickCount(ToOneLink::Type messageType)
{
	switch (messageType)
	{
	case ToOneLink::ICardStatus:
		// Only this message type counts as a heartbeat message, as it is the only one that 
		// One Link will reliably generate BonusingAvailable messages in response to.
		m_connectionInfo.SetLastHeartbeatMessageTickCount(m_operatingSystem.GetTickCount32());
		break;
	default:
		break;
	}
}

void CProgressiveConnection::UpdateConnectionState(ProgressiveConnectionState connectionState, ProgressiveConnectionError connectionError)
{
	bool requestProgressiveGameInfo(false);

	switch (connectionState)
	{
	case Disabled:
		LogString(PROGRESSIVE, "Connection - Disabled.");
		break;
	case WaitingForInfo:
		if (m_connectionInfo.GetState() != WaitingForInfo)
		{
			requestProgressiveGameInfo = true;
			LogString(PROGRESSIVE, "Connection - WaitingForInfo.");
		}
		break;
	case InDiscovery:
		LogString(PROGRESSIVE, "Connection - InDiscovery.");
		break;
	case Connected:
		LogString(PROGRESSIVE, "Connection - Connected.");
		break;
	case Disconnected:
		LogString(PROGRESSIVE, "Connection - Disconnected.");
		break;
	case InitiateDiscovery:
		LogString(PROGRESSIVE, "Connection - InitiateDiscovery");
		break;
	case Error:
	{
		//Update the Disconnet Timestamp and disconnection count
		m_connectionInfo.SetDisconnectTimeStampSinceBoot();
		m_connectionInfo.IncrementDisconnectCount();
		switch (connectionError)
		{
		case CouldNotFindServer:
			LogString(PROGRESSIVE, "Connection Error - CouldNotFindServer.");
			break;
		case DiscoveryConnectionError:
			LogString(PROGRESSIVE, "Connection Error - DiscoveryConnectionError.");
			break;
		case MessageTransmissionError:
			LogString(PROGRESSIVE, "Connection - MessageTransmissionError.");
			break;
		case ConnectFailed:
			LogString(PROGRESSIVE, "Connection Error - ConnectFailed.");
			break;
		case DisconnectFailed:
			LogString(PROGRESSIVE, "Connection Error - ActiveMQ failed to disconnect from the server properly.");
			break;
		default:
			break;
		}
		break;
    }
	default:
		break;
	}

	std::unique_lock<std::recursive_mutex> lock(m_criticalSection);
	m_connectionInfo.SetState(connectionState);
	m_connectionInfo.SetLastError(connectionError);
	m_progressiveDataConsumer.ConnectionUpdate(m_connectionInfo);
	lock.unlock();//Unique lock can be unlocked when needed, but 
	//lock_guard is unlocked upon going off scope

	if (requestProgressiveGameInfo)
	{
		// Request game info after we are actually in the WaitingForInfo state.
		m_progressiveDataConsumer.GetProgressiveGameInfo();
	}
}

void CProgressiveConnection::SendHostUpdate()
{
	CProgressiveHost *host(NULL);
	if (m_hostManager != NULL)
	{
		host = m_hostManager->GetCurrentHost();
	}
	m_progressiveDataConsumer.HostUpdate(host);
}

void CProgressiveConnection::SendMessageToOneLink(const ProgressiveQueueToOneLink* pMsg)
{
	SendMessageToOneLink(pMsg->GetMessageType(), pMsg->GetMessageBody());
}

bool CProgressiveConnection::InternalSendMessageToOneLink(ToOneLink::Type messageType, const char* pMessageType, const string& messageBody)
{
	bool messageSent = false;
	unique_ptr<cms::BytesMessage> bytesMessage(CreateBytesMessage(pMessageType, messageBody));
	if (bytesMessage && m_producer != nullptr)
	{
		//Only ICardConfigurationRequest5 gets sent to producerDiscoveryQueueName, 
		//all other messages get sent to producerQueueName
		if (messageType == ToOneLink::ICardConfigurationRequest5)
			bytesMessage->setStringProperty(OneLinkHeader::destination, OneLinkHeader::producerDiscoveryQueueName);

		m_producer->send(bytesMessage.get());
		messageSent = true;
	}
	return messageSent;
}

bool CProgressiveConnection::SendMessageToOneLink(ToOneLink::Type messageType, const string& messageBody)
{
	bool messageSent = false;

	const char* pMessageType = ToOneLink::emap.At(messageType, nullptr);
	if (pMessageType == nullptr)
	{
		LogString(ERROR_LOG, "SendMessageToOneLink Error: MessageType=%d is invalid. body=%s", messageType, messageBody.c_str());
	}
	else if (messageBody.empty())
	{
		LogString(ERROR_LOG, "SendMessageToOneLink Error: Message body is empty. type=%s", pMessageType);
	}
	else
	{
		std::lock_guard<std::recursive_mutex> lock(m_criticalSection);
		if (m_connectionInfo.GetState() == InDiscovery || m_connectionInfo.GetState() == Connected)
		{
			try
			{
				if (InternalSendMessageToOneLink(messageType, pMessageType, messageBody))
				{
					messageSent = true;
					m_connectionInfo.SetLastMessageTimestamp();
					SetLastHeartbeatTickCount(messageType);

					if (m_hostManager != nullptr)
						m_connectionInfo.SetLastMessageIP(m_hostManager->GetCurrentAddress());

					LogString(PROGRESSIVE, "SendMessageToOneLink Type: %s", pMessageType);
					LogString(PROGRESSIVEDATA, "SendMessageToOneLink Type: %s\nBody:%s",
						pMessageType, messageBody.c_str());

					m_progressiveDataConsumer.ConnectionUpdate(m_connectionInfo);
				}
			}
			catch (cms::CMSException& e)
			{
				LogString(ERROR_LOG, "SendMessageToOneLink: exception: %s", e.getStackTraceString().c_str());
				m_performDisconnect = true;
			}
			catch (...)
			{
				LogString(ERROR_LOG, "SendMessageToOneLink: unknown exception");
				m_performDisconnect = true;
			}
		}
		else
		{
			LogString(ERROR_LOG, "SendMessageToOneLink: Connection to the concentrator was lost.");
		}
	}

	return messageSent;
}

cms::BytesMessage* CProgressiveConnection::CreateBytesMessage()
{
	return m_session != nullptr ? m_session->createBytesMessage() : nullptr;
}

cms::BytesMessage* CProgressiveConnection::CreateBytesMessage(const char* pMessageType, const string& messageBody)
{
	cms::BytesMessage* pBytesMessage = CreateBytesMessage();
	if (pBytesMessage != nullptr)
	{
		pBytesMessage->setStringProperty(OneLinkHeader::type, pMessageType);
		pBytesMessage->setStringProperty(OneLinkHeader::destination, OneLinkHeader::producerQueueName);
		pBytesMessage->setStringProperty(OneLinkHeader::persistentType, OneLinkHeader::nonPersistent);
		pBytesMessage->setStringProperty(Stomp::Header::pContentType, ContentType::pJson);
		pBytesMessage->setStringProperty(Stomp::Header::pContentLength, to_string((unsigned long long) messageBody.size()));

		pBytesMessage->setBodyBytes((const unsigned char*)messageBody.c_str(), (int)messageBody.size());
	}
	return pBytesMessage;
}

void CProgressiveConnection::CheckResendList() 
{
	WORD itemLength = 0;
	DWORD uniqueId = 0;
	DWORD lastSent;
	bool alreadySuspicious(false);
	ToOneLink::Type msgType = ToOneLink::NumberOfToOneLinkMessages;//not a valid message
	CProgressiveTransactionItem resendItem;

	if (Connected == m_connectionInfo.GetState())
	{
		// Resend until list is empty or fails
		bool result = m_transactionResendList->GetFirstItem(resendItem, uniqueId, lastSent, alreadySuspicious);
		if (abs(int(CUtilities::GetTickCount() - lastSent)) > (int)(RESEND_TRANSACTION_INTERVAL))
		{
			while (result)
			{
				VerifyDeviceFields(resendItem);

				if (ToOneLink::NumberOfToOneLinkMessages == resendItem.GetMsgType() || SendMessageToOneLink(resendItem.GetMsgType(), resendItem.GetJson()))
				{
					const char* pMessageType = ToOneLink::emap.At(resendItem.GetMsgType(), nullptr);
					if (nullptr == pMessageType)
					{
						LogString(ERROR_LOG, "CheckResendList Error: Message type is invalid. body=%s", resendItem.GetJson().c_str());
					}
					else
					{
						LogString(PROGRESSIVE, "Type:%s was sent from resend list.", pMessageType);
					}

					// either invalid message or it was sent successful, remove it
					m_transactionResendList->RemoveTransaction(resendItem.GetUniqueId());

					// Get next item
					result = m_transactionResendList->GetFirstItem(resendItem, uniqueId, lastSent, alreadySuspicious);
				}
				else
				{
					// Call not successful so stop resend list processing
					result = false;

					// reset last sent
					m_transactionResendList->SetLastSent(uniqueId);
				}
				
			}
		}
	}
}

void CProgressiveConnection::SendCardConfigurationRequest()
{
	if (m_messageFactory && m_games)
	{
		SendMessageToOneLink(ToOneLink::ICardConfigurationRequest5, m_messageFactory->CreateICardConfigurationRequest5(m_games));
	}
}

void CProgressiveConnection::SendICardEvent(GeneralPollException generalPollException)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

	if (NULL != m_messageFactory && Connected == m_connectionInfo.GetState())
	{
		SendMessageToOneLink(ToOneLink::ICardEventNotification, m_messageFactory->CreateICardEvent(generalPollException));
	}
}

void CProgressiveConnection::SendOfflineCoinInEvent(__int64 coinIn)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

	if (NULL != m_messageFactory)
	{
		SendMessageToOneLink(ToOneLink::EventNotification, m_messageFactory->CreateOfflineCoinInEvent(coinIn));
	}
}

void CProgressiveConnection::SendProgressiveLevelHit(const CEGMAward& egmAward)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

	if (m_messageFactory != nullptr)
	{
		string message = m_messageFactory->CreateProgressiveLevelHit(egmAward, Connected != m_connectionInfo.GetState());
		if (Connected != m_connectionInfo.GetState() || !m_transactionResendList->IsEmpty() || !SendMessageToOneLink(ToOneLink::ProgressiveLevelHit, message))
		{
			// Send failed or current comms problem, add message to resend list
			LogString(PROGRESSIVE, "CProgressiveConnection::SendProgressiveLevelHit trying to Send Message to Concentrator returned false.");
			m_transactionResendList->AddTransaction(ToOneLink::ProgressiveLevelHit, m_transactionResendList->GetHighestUniqueId() + 1, message);
			m_progressiveDataConsumer.ProgressiveHitSentStatusUpdate(egmAward, false);
		}
		else
		{
			LogString(PROGRESSIVE, "CProgressiveConnection::SendProgressiveLevelHit trying to Send Message to Concentrator returned true.");
			m_progressiveDataConsumer.ProgressiveHitSentStatusUpdate(egmAward, true);
		}
	}
}

void CProgressiveConnection::SendJackpotPaidRequest(const CEGMAward &jackpotPaidInfo)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

	jackpotPaidInfo.LogInfo(_T("PP Jackpot Paid Message about to be created"));

	if (NULL != m_messageFactory)
	{
		// The return string is empty if the CEGMAward state has no equivalent jackpot paid message.
		string jackpotPaidMessage = m_messageFactory->CreateJackpotPaid(jackpotPaidInfo);
		if (!jackpotPaidMessage.empty())
		{
			jackpotPaidInfo.LogInfo("PP Jackpot Paid Sent to System");

			bool didSend = SendMessageToOneLink(ToOneLink::JackpotPaid, jackpotPaidMessage);

			if (AwardToGameSuccess == jackpotPaidInfo.GetAwardState())
			{
				LogString(PROGRESSIVE, "CProgressiveConnection::SendJackpotPaidRequest trying to Send Message to Concentrator returned %s.", didSend ? "true" : "false");
				m_progressiveDataConsumer.SendJackpotPaidRequestStatusUpdate(jackpotPaidInfo, didSend);
			}
		}
	}
}

void CProgressiveConnection::SetDependentCommunicationStatus(bool commOK, bool multicastActive)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

	bool commStatusChanged = m_dependentCommunicationsOK != commOK;

	m_dependentCommunicationsOK = commOK;

	if (m_multicastActive != multicastActive)
	{
		m_multicastActive = multicastActive;
		commStatusChanged = true;
	}

	if (commStatusChanged)
	{
		SendCardStatus();
	}
}

void CProgressiveConnection::SendCardStatus()
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);
	LogString(PROGRESSIVE, "Send Card Status to progressive");

	if (NULL != m_messageFactory)
	{
		string cardStatusString;
		cardStatusString = m_messageFactory->CreateICardStatus(
			m_player,
			m_sessionId,
			m_dependentCommunicationsOK,
			m_sessionActive,
			m_multicastActive,
			m_haveGameData ? &m_currentGameNumber : nullptr,
			m_haveGameData ? &m_currentDenomination : nullptr,
			m_campaignId,
			m_campaignRevision,
			m_skinId,
			m_skinRevision,
			CreateStatusString(m_currentActivity, m_pendingAction));

		SendMessageToOneLink(ToOneLink::ICardStatus, cardStatusString);
	}
}

void CProgressiveConnection::SetPlayer(const CPlayer* player, bool sessionActive, int sessionId)
{
	LogString(PROGRESSIVE, "Set player info to be sent on timer interval");
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

	ClearPlayer();
	m_sessionActive = sessionActive;
	m_sessionId = sessionId;

	if (NULL != player)
	{
		m_player = new CPlayer(*player);
	}

	/// Let OneLink know the player changed
	SendCardStatus();
}

bool CProgressiveConnection::UpdateGameData(const CGameEndData *gameData)
{
	bool changed(false);
	if (gameData != nullptr)
	{
		const CGame *game(gameData->GetCurrentGame());
		if (game != nullptr)
		{
			std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

			// Make a copy of the old game data.
			int oldGameNumber(m_currentGameNumber);
			bool oldHaveGameData(m_haveGameData);

			// Update the game data.
			m_currentGameNumber = (int)game->GetGameNumber();
			m_haveGameData = true;

			// Any changes?
			if (oldGameNumber != m_currentGameNumber ||
				oldHaveGameData != m_haveGameData)
			{
				changed = true;
			}
		}
	}

	return changed;
}

void CProgressiveConnection::SendCoinIn(CGameEndData* pGameEndData)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);
	UpdateGameData(pGameEndData);

	if (m_messageFactory != nullptr && pGameEndData != nullptr)
	{
		string message = m_messageFactory->CreateCoinIn5(*pGameEndData, m_player, m_sessionId, pGameEndData->GetOffline(), Connected != m_connectionInfo.GetState());
		LogString(PROGRESSIVE, "Send CoinIn5 to OneLink");
		if (Connected != m_connectionInfo.GetState() || !m_transactionResendList->IsEmpty() || !SendMessageToOneLink(ToOneLink::CoinIn5, message))
		{
			// Send failed or current comms problem, add message to resend list
			m_transactionResendList->AddTransaction(ToOneLink::CoinIn5, m_transactionResendList->GetHighestUniqueId() + 1, message);
		}
	}
}

void CProgressiveConnection::SendGameEnd(CGameEndData* pGameEndData)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);
	UpdateGameData(pGameEndData);
	if (m_messageFactory != nullptr && pGameEndData != nullptr)
	{ 
		string message = m_messageFactory->CreateGameEnd5(*pGameEndData, m_player, m_sessionId, Connected != m_connectionInfo.GetState());

		LogString(PROGRESSIVE, "Send GameEnd5 to OneLink");
		if (Connected != m_connectionInfo.GetState() || !m_transactionResendList->IsEmpty() || !SendMessageToOneLink(ToOneLink::GameEnd5, message))
		{
			m_transactionResendList->AddTransaction(ToOneLink::GameEnd5, m_transactionResendList->GetHighestUniqueId() + 1, message);
		}
	}
}

void CProgressiveConnection::SendPayout(CPayout* payout)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

	if (m_messageFactory != nullptr && payout != nullptr)
	{
		string message = m_messageFactory->CreatePayout(payout, m_player, m_sessionId);

		LogString(PROGRESSIVE, "Send PayoutMeterDelta to OneLink");
		if (Connected != m_connectionInfo.GetState() || !m_transactionResendList->IsEmpty() || !SendMessageToOneLink(ToOneLink::Payout, message))
		{
			m_transactionResendList->AddTransaction(ToOneLink::Payout, m_transactionResendList->GetHighestUniqueId() + 1, message);
		}
	}
}

void CProgressiveConnection::SendCampaignDownloadRequest(DWORD campaignId)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

	if (m_messageFactory != nullptr)
	{
		string message = m_messageFactory->CreateCampaignDownloadRequest(campaignId);

		LogString(PROGRESSIVE, "Send CampaignDownloadRequest to OneLink");
		if (Connected != m_connectionInfo.GetState() || !m_transactionResendList->IsEmpty() || !SendMessageToOneLink(ToOneLink::CampaignDownloadRequest, message))
		{
			m_transactionResendList->AddTransaction(ToOneLink::CampaignDownloadRequest, m_transactionResendList->GetHighestUniqueId() + 1, message);
		}
	}
}

void CProgressiveConnection::SendSkinDownloadRequest(DWORD skinId)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

	if (m_messageFactory != nullptr)
	{
		string message = m_messageFactory->CreateSkinDownloadRequest(skinId);

		LogString(PROGRESSIVE, "Send SkinDownloadRequest to OneLink");
		if (Connected != m_connectionInfo.GetState() || !m_transactionResendList->IsEmpty() || !SendMessageToOneLink(ToOneLink::SkinDownloadRequest, message))
		{
			m_transactionResendList->AddTransaction(ToOneLink::SkinDownloadRequest, m_transactionResendList->GetHighestUniqueId() + 1, message);
		}
	}
}

void CProgressiveConnection::SetProgressiveDashboardStatus(DWORD campaignId, DWORD campaignRevision,
	DWORD skinId, DWORD skinRevision, WORD currentActivity, WORD pendingAction)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

	if (m_campaignId != campaignId)
	{ 
		LogString(PROGRESSIVE, "DashboardStatus changed: CampaignId -> old: %ld new: %u", m_campaignId, campaignId);
		m_campaignId = campaignId;
	}

	if (m_campaignRevision != campaignRevision)
	{
		LogString(PROGRESSIVE, "DashboardStatus changed: CampaignRevision -> old: %ld new: %u", m_campaignRevision, campaignRevision);
		m_campaignRevision = campaignRevision;
	}

	if (m_skinId != skinId)
	{
		LogString(PROGRESSIVE, "DashboardStatus changed: SkinId -> old: %ld new: %u", m_skinId, skinId);
		m_skinId = skinId;
	}

	if (m_skinRevision != skinRevision)
	{
		LogString(PROGRESSIVE, "DashboardStatus changed: SkinRevision -> old: %ld new: %u", m_skinRevision, skinRevision);
		m_skinRevision = skinRevision;
	}

	auto action = static_cast<PendingAction>(pendingAction);
	if (m_pendingAction != action)
	{
		LogString(PROGRESSIVE, "DashboardStatus changed: PendingAction -> old: %d new: %d", m_pendingAction, action);
		m_pendingAction = action;
	}

	auto activity = static_cast<CurrentActivity>(currentActivity);
	if (m_currentActivity != activity)
	{
		LogString(PROGRESSIVE, "DashboardStatus changed: CurrentActivity -> old: %d new: %d", (int)m_currentActivity, (int)activity);
		m_currentActivity = activity;
	}
}

std::string CProgressiveConnection::CreateStatusString(CurrentActivity currentActivity, PendingAction pendingAction)
{
	if (currentActivity == CurrentActivity::None && pendingAction == PendingAction::NonePending)
	{
		if (m_sessionActive == true)
		{
			if (m_player && !m_player->IsBannedFromPlay() && !m_player->IsMaxCardLimitReached())
				return "Carded";
			else
				return "UnCarded";
		}
		return "Idle";
	}

	if (currentActivity != CurrentActivity::None)
	{
		switch (currentActivity)
		{
		case CurrentActivity::Upgrading:
			return "Upgrading";
			break;
		case CurrentActivity::Rebooting:
			return "Rebooting";
			break;
		case CurrentActivity::RestartingUi:
			return "Restarting UI";
			break;
		case CurrentActivity::Downloading:
			return "Downloading";
			break;
		case CurrentActivity::None:
			return "";
			break;
		}
	}

	if (pendingAction != PendingAction::NonePending)
	{
		switch (pendingAction)
		{
		case PendingAction::RestartUI:
			return "UI Restart Pending";
			break;
		case PendingAction::RebootAsap:
		case PendingAction::RemoteReboot:
		case PendingAction::RebootAfterDrop:
			return "Reboot Pending";
			break;
		case PendingAction::OsUpgrade:
			return "Upgrade Pending";
			break;
		case PendingAction::RefreshUI:
			return "UI Refresh Pending";
			break;
		case PendingAction::NonePending:
			return "";
			break;
		}
	}

	return "Idle";
}

void CProgressiveConnection::SendPromoGameAcceptanceStatus(CPromoGameAcceptanceStatus* promoGameAcceptanceStatus)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);
	if (m_messageFactory != nullptr && Connected == m_connectionInfo.GetState() && promoGameAcceptanceStatus != nullptr)
	{
		LogString(PROGRESSIVE, "Send PromoGameAcceptanceStatus to OneLink");
		SendMessageToOneLink(ToOneLink::PromoGameAcceptanceStatus, m_messageFactory->CreatePromoGameAcceptanceStatus(*promoGameAcceptanceStatus));
	}
}

void CProgressiveConnection::SendConfigFileDownloadAck(const std::string &messageRefId, const std::string &filename)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);
	if (m_messageFactory != nullptr && Connected == m_connectionInfo.GetState())
	{
		LogString(PROGRESSIVE, "Send ConfigFileDownloadAcknowledgement to OneLink");
		SendMessageToOneLink(ToOneLink::ConfigFileDownloadAck, m_messageFactory->CreateConfigFileDownloadAck(messageRefId, filename));
	}
}

void  CProgressiveConnection::ProcessGameData(const CGameEndData *gameData)
{
	std::lock_guard < std::recursive_mutex > cs(m_criticalSection);
	if (UpdateGameData(gameData))
	{
		SendCardStatus();
	}
}

void CProgressiveConnection::Connect()
{
	// If we tried to connect to the current address our max number of times, 
	///  tell the host manager that this address has failed so it can go on to the next.
	if (m_connectionInfo.GetConnectAttemptCount() >= MAX_CONNECT_ATTEMPTS)
	{
		m_connectionInfo.SetConnectAttemptCount(0);
		m_hostManager->ConnectionFailure();
		m_progressiveDataConsumer.ConnectionUpdate(m_connectionInfo);
	}

	if (m_hostManager != NULL)
	{
		// Get the url we are to connect to
		std::string url = m_hostManager->GetUrl();

		if (url.empty())
		{
			UpdateConnectionState(Error, CouldNotFindServer);
		}
		else
		{
			ConnectToConcentrator(url);
			m_progressiveDataConsumer.ConnectionUpdate(m_connectionInfo);
		}

		// Tell the connection info that we are trying to make a connection
		m_connectionInfo.ConnectAttempt();

		SendHostUpdate();
	}
}

void CProgressiveConnection::ConnectToConcentrator(string url)
{
	unique_ptr<ActiveMQConnectionFactory> connectionFactory; // Define outside try/catch.
	std::lock_guard < std::recursive_mutex > cs(m_criticalSection);
	try
	{
		if (NULL != m_messageFactory)
		{
			if (CUtilities::DoesFileExist(m_paltronicsCertificatePathname.c_str()))
			{
				Disconnect();
				LogString(PROGRESSIVE, "Loading the certificate %s", m_paltronicsCertificatePathname.c_str());
				decaf::lang::System::setProperty(OneLinkHeader::trustStore, m_paltronicsCertificatePathname);

				// Create a ConnectionFactory
				connectionFactory.reset(new ActiveMQConnectionFactory(url));

				// Create a Connection(s)
				LogString(PROGRESSIVE, "Connecting to host %s",url.c_str());
				m_connection = connectionFactory.get()->createConnection(OneLinkHeader::userName, OneLinkHeader::password);
				LogString(PROGRESSIVE, "Connection created");

				ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>(m_connection);
				amqConnection->setConnectResponseTimeout(CONNECT_TIMEOUT);
				amqConnection->setSendTimeout(SEND_TIMEOUT);
				amqConnection->setCloseTimeout(CLOSE_TIMEOUT);
				amqConnection->setWatchTopicAdvisories(false);

				// Create a Session
				m_session = m_connection->createSession(cms::Session::AUTO_ACKNOWLEDGE);
				LogString(PROGRESSIVE, "Session created");

				//Create MessageConsumer
				m_consumerDestination = m_session->createQueue(OneLinkHeader::consumerPartialQueueName + m_messageFactory->GetMacAddressWithoutColon());
				m_consumer = m_session->createConsumer(m_consumerDestination);
				m_consumer->setMessageListener(this);

				//Create MessageProducer
				m_producerDestination = m_session->createQueue(OneLinkHeader::producerQueueName);
				m_producer = m_session->createProducer(m_producerDestination);
				m_connection->start();
				ReconnectChannels();
				m_connectionInfo.SetConnected();
				UpdateConnectionState(InitiateDiscovery);
				LogString(PROGRESSIVE, "Connection started");				
			}
			else
			{
				LogString(PROGRESSIVE, "ConnectToConcentrator Error - cannot find the Paltronics certificate %s.", m_paltronicsCertificatePathname.c_str());
				UpdateConnectionState(Error, ConnectFailed);
			}
		}
		else
		{
			LogString(ERROR_LOG, "ConnectToConcentrator Error - m_messageFactory is NULL.");
			UpdateConnectionState(Error, ConnectFailed);
		}
	}
	catch (cms::CMSException& e)
	{
		LogString(ERROR_LOG, "ConnectToConcentrator exception: %s", e.getStackTraceString().c_str());
		UpdateConnectionState(Error, ConnectFailed);
	}
	catch (...)
	{
		LogString(ERROR_LOG, "ConnectToConcentrator - unhandled exception");
		UpdateConnectionState(Error, ConnectFailed);
	}
}

void CProgressiveConnection::onMessage(const  cms::Message* message)
{
	LogString(PROGRESSIVE, "OnMessage called");

	if (m_disconnecting)
	{
		LogString(PROGRESSIVE, "Bypassing OnMessage since disconnecting");
	}
	else
	{
		CUtilities::SetThreadName(CUtilities::GetCurrentThreadId(), THREAD_PROGRESSIVE_ON_MESSAGE);
		CWatchDog::Instance().RegisterThread(_T(THREAD_PROGRESSIVE_ON_MESSAGE), CUtilities::GetCurrentThreadId(), ON_MESSAGE_TIMEOUT, pthread_self());
		WatchdogCheckIn();

		try
		{
			const cms::BytesMessage* bytesMessage = dynamic_cast<const cms::BytesMessage*>(message);
			if (bytesMessage != nullptr)
			{
				string messageType(bytesMessage->getStringProperty(OneLinkHeader::type));
				FromOneLink::Type type = FromOneLink::emap[messageType.c_str()];
				LogString(PROGRESSIVE, "Received Message Type: %s", messageType.c_str());

				if (bytesMessage->getBodyLength() > 0)
				{
					unique_ptr<BYTE> body(bytesMessage->getBodyBytes());
					vector<char> vMessageBody;
					vMessageBody.resize(bytesMessage->getBodyLength() + 1);
					memcpy(&vMessageBody[0], body.get(), bytesMessage->getBodyLength());
					vMessageBody[bytesMessage->getBodyLength()] = 0;
					std::string messageBody(vMessageBody.begin(), vMessageBody.end());
					LogString(PROGRESSIVEDATA, "OnMessage: Type: %s\nBody:%s", messageType.c_str(), messageBody.c_str());

					rapidjson::Document jsonDoc;
					if (jsonDoc.ParseInsitu(&vMessageBody[0]).HasParseError())
					{
						LogString(PROGRESSIVE, "ERROR: onMessage parse error: Type: %s: Error: %s", messageType.c_str(),
							JsonException::GetParseErrorString(jsonDoc.GetParseError()));
					}
					else
					{
						if (m_messageFactory != nullptr)
						{
							switch (type)
							{
							case FromOneLink::BonusingAvailable:
								ProcessBonusingAvailable(jsonDoc);
								break;
							case FromOneLink::ClaimableAward:
								m_progressiveDataConsumer.ClaimableAward(new CProgressiveClaimableAward(CUtilities::GetTickCount(), jsonDoc));
								break;
							case FromOneLink::DeviceRestartRequest:
								ProcessDeviceRestartRequest(jsonDoc);
								break;
							case FromOneLink::DiscoveryEvent:
								ProcessDiscoveryEvent(jsonDoc);
								break;
							case FromOneLink::ICardConfiguration5:
								ProcessICardConfiguration5(jsonDoc);
								break;
							case FromOneLink::JackpotHit:
								ProcessJackpotHit(jsonDoc);
								break;
							case FromOneLink::PromoGameRequest:
								ProcessPromoGameRequest(jsonDoc);
								break;
							case FromOneLink::OfferRequest:
								ProcesOfferRequest(jsonDoc);
								break;
							case FromOneLink::EgmStateRequest:
								ProcessEgmStateRequest(jsonDoc);
								break;
							case FromOneLink::MulticastEncryptionConfiguration:
								ProcessMulticastEncryptionConfig(jsonDoc);
								break;
							case FromOneLink::FirmwareDownload:
								ProcessFirmwareDownload(jsonDoc);
								break;
							case FromOneLink::ICardJackpotAnnouncement:
								m_progressiveDataConsumer.ProgressiveJackpotAnnouncement(new CProgressiveJackpotAnnouncement(jsonDoc));
								break;
							case FromOneLink::ICardTotalizerAnnouncement:
								m_progressiveDataConsumer.ProgressiveTotalizerAnnouncement(new CProgressiveTotalizerAnnouncement(jsonDoc));
								break;
                            case FromOneLink::ConfigFileDownload:
								ProcessConfigFileDownload(jsonDoc);
								break;
							case FromOneLink::CampaignDownload:
								ProcessCampaignDownload(jsonDoc);
								break;
							case FromOneLink::SkinDownload:
								ProcessSkinDownload(jsonDoc);
								break;
							case FromOneLink::ScheduleEnd:
								ProcessScheduleEnd(jsonDoc);
								break;								
							}
						}
						else
						{
							LogString(PROGRESSIVE, "ERROR: onMessage MessageFactory missing for Message Type: %s", messageType.c_str());
						}
					}
				}
			}
		}
		catch (cms::CMSException& e)
		{
			LogString(ERROR_LOG, "ERROR: onMessage CMSException: %s", e.getStackTraceString().c_str());
			LogString(PROGRESSIVE, "ERROR: onMessage CMSException: %s", e.getStackTraceString().c_str());
		}
		catch (JsonException& e)
		{
			LogString(ERROR_LOG, "ERROR: onMessage JsonException: %s: '%s'", e.codestring(), e.what());
			LogString(PROGRESSIVE, "ERROR: onMessage JsonException: %s: '%s'", e.codestring(), e.what());
		}
		catch (EnumMap<FromOneLink::Type>::Exception& e)
		{
			LogString(ERROR_LOG, "ERROR: onMessage EnumMap<FromOneLink::Type>::Exception (unhandled message type): '%s'", e.what());
			LogString(PROGRESSIVE, "ERROR: onMessage EnumMap<FromOneLink::Type>::Exception (unhandled message type): '%s'", e.what());
			const cms::BytesMessage* bytesMessage = dynamic_cast<const cms::BytesMessage*>(message);
			unique_ptr<BYTE> body(bytesMessage->getBodyBytes());
			vector<char> vMessageBody;
			vMessageBody.resize(bytesMessage->getBodyLength() + 1);
			memcpy(&vMessageBody[0], body.get(), bytesMessage->getBodyLength());
			vMessageBody[bytesMessage->getBodyLength()] = 0;
			std::string messageBody(vMessageBody.begin(), vMessageBody.end());
			LogString(ERROR_LOG, "OnMessage: \nBody:%s", messageBody.c_str());
		}
		catch (...)
		{
			LogString(ERROR_LOG, "ERROR: onMessage unhandled exception");
			LogString(PROGRESSIVE, "ERROR: onMessage unhandled exception");
		}
		CWatchDog::Instance().DeregisterThread();
	}
}

void CProgressiveConnection::OnMulticastMessage(ProgressiveMulticastFrame& frame)
{
    try
    {
		std::shared_ptr<unsigned char> decryptedBody;
		unsigned char* newBody;
		if (frame.HasEncryptedBody())
		{
			//Get the body of the frame and decrypt it.
			decryptedBody = 
				this->m_aes128EncryptionUtil->Decrypt(reinterpret_cast<unsigned char*>(frame.GetBody()));
			newBody = decryptedBody.get();
		}
		else
		{
			newBody = reinterpret_cast<unsigned char*>(frame.GetBody());
		}

		auto frameType = frame.GetType();
    	if (frameType == FromOneLinkUdp::WinnerPick ||
			frameType == FromOneLinkUdp::WinnerPicked)
    	{
    		LogString(PROGRESSIVEDATA, "OnMulticastMessage: Type: %s\nBody: %s", frame.GetTypeStr(), newBody);
    	}
		else if(frameType == FromOneLinkUdp::LevelBroadcast)
		{
			LogString(LEVELBROADCAST_LOG, "Level Broadcast\n%s", newBody);
		}
		
    	rapidjson::Document jsonDoc;
    	if (jsonDoc.ParseInsitu(reinterpret_cast<char *>(newBody)).HasParseError())
    	{
    		LogString(ERROR_LOG, "ERROR: OnMulticastMessage JSON Parse Error: %s: %s", frame.GetTypeStr(),
    			JsonException::GetParseErrorString(jsonDoc.GetParseError()));
    	}
    	else
    	{
			switch (frameType)
			{
            case FromOneLinkUdp::LevelBroadcast:
                {
    				LogString(PROGRESSIVE, "Received LevelBroadcast");
    				m_progressiveDataConsumer.ProgressiveLevelBroadcast(new CProgressiveLevelBroadcast(CUtilities::GetTickCount(), jsonDoc));
                }
				break;

			case FromOneLinkUdp::WinnerPick:
				LogString(PROGRESSIVE, "Received WinnerPick");
				m_progressiveDataConsumer.ProgressiveWinnerPick(new CProgressiveWinnerPick(CUtilities::GetTickCount(), jsonDoc));
				break;

			case FromOneLinkUdp::WinnerPicked:
			{
				LogString(PROGRESSIVE, "Received WinnerPicked");
				OptionalVal<int> sessionId(JsonGetOptionalIntEx(jsonDoc, pKeySessionId));
				if (sessionId.IsInvalidOrEqual(m_sessionId))
				{
					const rapidjson::Value& value = jsonDoc[pKeyWinnerDeviceId];
					if (value.IsInt64())
					{
						if (value.GetInt64() == m_messageFactory->GetSourceDeviceID())
						{
							LogString(PROGRESSIVE, "Forwarding WinnerPicked");
							m_progressiveDataConsumer.ProgressiveWinnerPicked(new CProgressiveWinnerPicked(CUtilities::GetTickCount(), jsonDoc));
						}
						else
						{
							LogString(PROGRESSIVE, "WinnerPicked deviceId (%lld) did not match current deviceId (%lld): Message ignored.", value.GetInt64(), m_messageFactory->GetSourceDeviceID());
						}
					}
					else
					{
						LogString(PROGRESSIVE, "WinnerPicked deviceId is missing or malformed: Message ignored.");
					}
				}
				else
				{
					LogString(PROGRESSIVE, "WinnerPicked sessionId (%d) did not match current sessionId (%d): Message ignored.", sessionId.val, m_sessionId);
				}
				break;
			}

			default:
				LogString(PROGRESSIVE, "ERROR: OnMulticastMessage Invalid Frame type: %d", (int)frame.GetType());
				break;
			}
		}
	}

    catch (JsonException& e)
    {
        LogString(ERROR_LOG, "ERROR: OnMulticastMessage JsonException: %s: %s: '%s'", frame.GetTypeStr(), e.codestring(), e.what());
    }
	catch (...)
	{
		LogString(ERROR_LOG, "Unknown ERROR: %s", frame.GetBodyAsString().c_str());
	}
}

template <typename T>
void CProgressiveConnection::DeleteActiveMqObject(T*& activeMqObject)
{
	try
	{
		delete activeMqObject;
	}
	catch (cms::CMSException& e)
	{
		LogString(ERROR_LOG, "DeleteActiveMqObject exception: %s", e.getStackTraceString().c_str());
		LogString(PROGRESSIVE, "DeleteActiveMqObject exception: %s", e.getStackTraceString().c_str());
	}
	catch (...)
	{
		LogString(ERROR_LOG, "DeleteActiveMqObject - unhandled exception.");
		LogString(PROGRESSIVE, "DeleteActiveMqObject - unhandled exception.");
	}
	activeMqObject = NULL;
}

template <typename T>
void CProgressiveConnection::CloseActiveMqObject(T*& activeMqObject)
{
	// Close open resources.
	try
	{
		if (NULL != activeMqObject)
		{
			activeMqObject->close();
		}
	}

	catch (cms::CMSException& e)
	{
		LogString(ERROR_LOG, "CloseActiveMqObject exception: %s", e.getStackTraceString().c_str());
		LogString(PROGRESSIVE, "CloseActiveMqObject exception: %s", e.getStackTraceString().c_str());
	}
	catch (...)
	{
		LogString(ERROR_LOG, "CloseActiveMqObject - unhandled exception.");
		LogString(PROGRESSIVE, "CloseActiveMqObject - unhandled exception.");
	}
}

template <typename T>
void CProgressiveConnection::StopActiveMqObject(T*& activeMqObject)
{
	// stop open resources.
	try
	{
		if (NULL != activeMqObject)
		{
			activeMqObject->stop();
		}
	}
	catch (cms::CMSException& e)
	{
		LogString(ERROR_LOG, "StopActiveMqObject exception: %s", e.getStackTraceString().c_str());
		LogString(PROGRESSIVE, "StopActiveMqObject exception: %s", e.getStackTraceString().c_str());
	}
	catch (...)
	{
		LogString(ERROR_LOG, "StopActiveMqObject - unhandled exception.");
		LogString(PROGRESSIVE, "StopActiveMqObject - unhandled exception.");
	}
}

/// <summary>
/// Disconnects this instance.
/// </summary>
void CProgressiveConnection::Disconnect()
{
	if (!m_performDisconnect && m_multicastSocketFailed)
	{
		m_connectionInfo.SetLastMulticastSocketFailedTimeStamp();
	}		
	m_multicastSocketFailed = false;
	m_performDisconnect = false;
	m_disconnecting = true;

	LogString(PROGRESSIVE, "Progressive connection disconnect called");

    std::unique_lock<std::recursive_mutex> cs(m_criticalSection);
	WatchdogCheckIn();

	// stop and close resources
	LogString(PROGRESSIVE, "Stopping and closing progressive connection");
	StopActiveMqObject(m_consumer);
	CloseActiveMqObject(m_producer);
	StopActiveMqObject(m_connection);
	StopActiveMqObject(m_session);
	CloseActiveMqObject(m_session);
	CloseActiveMqObject(m_connection);

	// Destroy resources.
	LogString(PROGRESSIVE, "Deleting progressive connection");
	DeleteActiveMqObject(m_session);
	DeleteActiveMqObject(m_connection);
	DeleteActiveMqObject(m_consumerDestination);
	DeleteActiveMqObject(m_consumer);
	DeleteActiveMqObject(m_producerDestination);
	DeleteActiveMqObject(m_producer);

	WatchdogCheckIn();
	CloseChannels();

	UpdateConnectionState(Disconnected);

	m_disconnecting = false;

	LogString(PROGRESSIVE, "Done with Progressive connection disconnect.");
}

void CProgressiveConnection::ProcessDiscoveryEvent(const rapidjson::Document& jsonDoc)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);
	LogString(PROGRESSIVE, "Processing discovery event.");
	const char* status = JsonGetString(jsonDoc, pKeyStatus, nullptr);
	LogString(PROGRESSIVE, "DiscoveryEvent status = %s.",status);
	if (strcmp(status, pKeyQuarantined) == 0)
	{
		// Device is quarantined
		LogString(PROGRESSIVE, "Setting connection state to InDiscovery");
		UpdateConnectionState(InDiscovery);
		m_progressiveDataConsumer.SendMsgToInternals(new CInternalMessage(INT_MSG_SET_PROGRESSIVE_DOWN, new CProgressiveConnectionInfo(m_connectionInfo)), QUEUE_PRIORITY_HIGH);
	}

}

void CProgressiveConnection::ProcessICardConfiguration5(const rapidjson::Document& jsonDoc)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);
	const char* pMessageId = nullptr;

	try
	{
		pMessageId = JsonGetString(jsonDoc, pKeyMessageId, nullptr);
		m_messageFactory->UpdateCardConfiguration(jsonDoc);

		unique_ptr<ProgressiveLevelsInfo> levels(new ProgressiveLevelsInfo(jsonDoc));
		const char* jsonString = JsonGetStringEx(jsonDoc, OneLinkHeader::pOneLinkSystemId);
		std::vector<std::string> urlsList = levels->GetUrls();
		UpdateMulticastConfiguration(jsonString, urlsList);

		std::set<DWORD> l_PokerCardsSet = levels->GetQueryPokerCardsSet();
		m_messageFactory->SetQueryPokerCardsSet(l_PokerCardsSet);

		CCampaignConfig* campaignConfig = nullptr;
		try
		{
			auto id = JsonGetOptionalInt64Ex(jsonDoc, pKeyCampaignId);
			if (id.IsValid())
			{
				campaignConfig = new CCampaignConfig(jsonDoc);
			}
		}
		catch (JsonException& e)
		{
			LogString(ERROR_LOG, "Campaign Verification deserialize error: '%s': Type: %s",
				e.codestring(), e.what());
		}

		CSkinConfig* skinConfig = nullptr;
		try
		{
			auto id = JsonGetOptionalInt64Ex(jsonDoc, pKeySkinId);
			if (id.IsValid())
			{
				skinConfig = new CSkinConfig(jsonDoc);
			}
		}
		catch (JsonException& e)
		{
			LogString(ERROR_LOG, "Skin Verification deserialize error: '%s': Type: %s",
				e.codestring(), e.what());
		}

		m_progressiveDataConsumer.SendProgressiveConfiguration(
			levels.release(),
			new CProgressiveAnnouncementsInfo(jsonDoc),
			new CFirmwarePackage(jsonDoc, true),
			campaignConfig,
			skinConfig);

		m_progressiveDataConsumer.QueueToOneLink(new ProgressiveQueueToOneLink(ToOneLink::DeviceAcknowledgement,
			m_messageFactory->CreateDeviceAcknowledgement(pMessageId, true)));

		// Set m_connectionInfo to the connected state, but don't update any of the related connection status information.
		// We are coming off Discovery state and we do not need to update Last Time Connected value.
		UpdateConnectionState(Connected);
	}
	catch (SentinelException& e)
	{
		std::stringstream errStr;
		errStr << "ERROR: ProcessICardConfiguration failed with exception:  " << e.what();

		m_progressiveDataConsumer.QueueToOneLink(new ProgressiveQueueToOneLink(ToOneLink::DeviceAcknowledgement,
			m_messageFactory->CreateDeviceAcknowledgement(pMessageId, true, errStr.str().c_str())));

		LogStringNoFormat(ERROR_LOG, errStr.str());
	}
}

void CProgressiveConnection::ProcessBonusingAvailable(const rapidjson::Document& jsonDoc)
{
	OptionalVal<int> sessionId = JsonGetOptionalIntEx(jsonDoc, pKeySessionId);
	if (!sessionId)
	{
		// Check the PROGRESSIVEDATA log for message details.
		LogStringNoFormat(PROGRESSIVE, "BonusingAvailable m_sessionId=NULL");
	}
	else
	{
		CProgressiveBonusingAvailable *progressiveBonusingAvailable = new CProgressiveBonusingAvailable(sessionId.val, jsonDoc);
		CProgressiveEligibleLevelsInfo *progressiveEligibleLevelsInfo = new CProgressiveEligibleLevelsInfo(jsonDoc);
		ProgressivePatronSessionInfo *progressivePatronSessionInfo = new ProgressivePatronSessionInfo(sessionId.val, jsonDoc);
		m_progressiveDataConsumer.ProgressiveBonusingAvailable(progressiveBonusingAvailable, progressiveEligibleLevelsInfo, progressivePatronSessionInfo);
	}
}

void CProgressiveConnection::ProcessDeviceRestartRequest(const rapidjson::Document&)
{
	LogString(PROGRESSIVE, "Sending Remote Reboot request to Internals.");
	m_progressiveDataConsumer.SendRemoteReboot();
}

void CProgressiveConnection::ProcessJackpotHit(const rapidjson::Document& jsonDoc)
{
	SYSTEMTIME systemTime;
	CUtilities::GetUTCSystemTime(&systemTime);
	CEGMAward *egmAward(new CEGMAward(systemTime, jsonDoc));

	if (!egmAward->IsStandardProgressive() && !egmAward->DoesAwardMatchSession(m_sessionId))
	{
		egmAward->SetAwardState(AwardSessionMismatch);
		egmAward->SetAwardAmountPaid(0);
		std::string descriptionString;
		std::sprintf((char*)descriptionString.c_str(), CEGMAward::FORMAT_INT__EGM_AWARD_DOES_NOT_MATCH_CURRENT_SESSION_ID, (int)m_sessionId);
		egmAward->SetStatusInfo(descriptionString);
		egmAward->LogInfo("[Progressive from One Link]");
		SendJackpotPaidRequest(*egmAward);
	}
	else
	{
		LogString(PROGRESSIVE, "Sending JackpotHit to Internals.");
		m_progressiveDataConsumer.JackpotHit(egmAward);
	}
}

void CProgressiveConnection::ProcesOfferRequest(const rapidjson::Document& jsonDoc)
{
	LogString(PROGRESSIVE, "Process Offer Request");
	try
	{
		CProgressiveOfferRequest progressiveOfferRequest(jsonDoc);
		string progressiveOfferAcceptanceResponse = m_messageFactory->CreateOfferAcceptanceResponse(progressiveOfferRequest);
		if (!progressiveOfferAcceptanceResponse.empty())
		{
			SendMessageToOneLink(ToOneLink::OfferAcceptanceResponse, progressiveOfferAcceptanceResponse);
		}
	}
	catch (JsonException& e)
	{
		LogString(ERROR_LOG, "ProcesOfferRequest deserialize error: '%s': Type: %s",
			e.codestring(), e.what());
	}
}

//When we recieve a multicast configuration message from onlink we need to update the keys.
void CProgressiveConnection::ProcessMulticastEncryptionConfig(const rapidjson::Document& jsonDoc)
{
	LogString(PROGRESSIVE, "Process Multicast Encryption Configuration");
	try
	{
		CProgressiveMulticastEncryptionConfiguration encryptionConfiguration(jsonDoc);
		m_aes128EncryptionUtil->UpdateKeys({ encryptionConfiguration.GetCurrentEncryptionSeed(), encryptionConfiguration.GetPreviousEncryptionSeed() });
	}
	catch (JsonException& e)
	{
		LogString(PROGRESSIVE, "ProcessMulticastEncryptionConfig deserialize error: '%s': Type: %s",
			e.codestring(), e.what());
		LogString(ERROR_LOG, "ProcessMulticastEncryptionConfig deserialize error: '%s': Type: %s",
			e.codestring(), e.what());
	}
}

void CProgressiveConnection::ProcessPromoGameRequest(const rapidjson::Document& jsonDoc)
{
	LogString(PROGRESSIVE, "Sending PromoGameRequest to Internals.");
	m_progressiveDataConsumer.SendPromoGameRequest(new CPromoGameRequest(jsonDoc));
}

void CProgressiveConnection::ProcessFirmwareDownload(const rapidjson::Document& jsonDoc)
{
	LogString(PROGRESSIVE, "Sending FirmwareDownload to Internals.");
	m_progressiveDataConsumer.SendFirmwareDownload(new CFirmwarePackage(jsonDoc));
}

void CProgressiveConnection::ProcessConfigFileDownload(const rapidjson::Document& jsonDoc)
{
	LogString(PROGRESSIVE, "Sending ConfigFileDownload to Internals.");
	m_progressiveDataConsumer.SendConfigFileDownload(new CConfigFilePackage(jsonDoc));
}

void CProgressiveConnection::ProcessCampaignDownload(const rapidjson::Document& jsonDoc)
{
	LogString(PROGRESSIVE, "Sending CampaignDownload to Internals.");
	m_progressiveDataConsumer.SendCampaignDownload(new CCampaignPackage(jsonDoc));
}

void CProgressiveConnection::ProcessSkinDownload(const rapidjson::Document& jsonDoc)
{
	CSkinPackage *skinPackage = new CSkinPackage(jsonDoc);
	if (skinPackage->IsValid())
	{
		LogString(PROGRESSIVE, "Sending SkinDownload to Internals.");
		m_progressiveDataConsumer.SendSkinDownload(skinPackage);
	}
	else
	{
		LogString(PROGRESSIVE, "Ignoring SkinDownload request due to resolution mismatch.");
		delete skinPackage;
		skinPackage = nullptr;
	}
}

void CProgressiveConnection::ProcessScheduleEnd(const rapidjson::Document& jsonDoc)
{
	int64_t levelId(JsonGetInt64Ex(jsonDoc, pKeyLevelId));
	string levelName(JsonGetCStringEx(jsonDoc, pKeyLevelName));
	LogString(PROGRESSIVE, "ProcessScheduleEnd for level id=%lld name=%s to Internals.", levelId, levelName.c_str());
	m_progressiveDataConsumer.SendScheduleEnd(levelId, levelName);
}

void CProgressiveConnection::VerifyDeviceFields(CProgressiveTransactionItem& resendItem) 
{
	if (m_messageFactory == NULL)
		return;

	// Verify that the device specific fields have been filled in with valid data.
	// This is necessary when game play is detected before device specific info from ICardConfiguration5 is available
	// verify following fields:
	// "SourceDeviceConfigVersion":145,
	//"SourceDeviceId":63,
	//"SourceDevice":"@device.ICARD.0060E06C8764",
	//"SourceAsset":"88884444",
	//"Timestamp":1604081013069,
	//"MessageId":"1604080987000-31"
	string json = resendItem.GetJson();

	// location of "SourceDeviceConfigVersion"
	int pos = json.find(string(pKeySourceDeviceConfigVersion)) - 1;

	// location of "MessageId"
	int endpos = json.find(string(pKeyMessageId), pos) - 1;
	const int NPOS = -2;

	if (pos != NPOS && endpos != NPOS)
	{
		// replace pos through endpos with new string
		string newString = m_messageFactory->GetDeviceAttributesString();
		json.replace(pos, endpos - pos, newString);
		resendItem.setJson(json);
	}
}

void CProgressiveConnection::ProcessEgmStateRequest(const rapidjson::Document& jsonDoc)
{
	bool egmStateRequest = JsonGetBool(jsonDoc, pKeyEnable, FALSE);
	int64_t jackpotId(0);
	if (JsonMemberExists(jsonDoc, pKeyJackpotId))
	{
		jackpotId = JsonGetInt64Ex(jsonDoc, pKeyJackpotId);
	}
	m_progressiveDataConsumer.EgmStateRequest(egmStateRequest, jackpotId);
}

void CProgressiveConnection::ClearPlayer()
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);

	delete m_player;
	m_player = nullptr;

	m_connectionInfo.SetLastPatronInfoTimestamp(CUtilities::Get1970Time());
}

void CProgressiveConnection::SetCurrentDenom(int currentDenom)
{
	__int64 oldDenom = m_currentDenomination;
	m_currentDenomination = currentDenom;
	std::stringstream logStr;
	logStr << "INFO: Setting current denomination: " << m_currentDenomination;

	LogStringNoFormat(PROGRESSIVE, logStr.str());

	if (oldDenom != m_currentDenomination)
	{
		SendCardStatus();
	}
}

void CProgressiveConnection::SendEgmStateResponse(bool enabled)
{
	std::lock_guard<std::recursive_mutex> cs(m_criticalSection);
	if (enabled)
	{
		if (NULL != m_messageFactory)
		{
			SendMessageToOneLink(ToOneLink::EgmStateResponse, m_messageFactory->CreateEgmStateResponse(enabled));
		}
	}
}

