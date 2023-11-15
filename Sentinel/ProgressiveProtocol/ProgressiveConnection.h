#pragma once
#include "stdafx.h"

#include <cms/Connection.h> //TODO-PORT Move third-party includes to stdafx.h
#include <cms/MessageListener.h>
#include <cms/MessageProducer.h>
#include <cms/MessageConsumer.h>
#include <cms/BytesMessage.h>

#include "ProcessDump/OperatingSystem.h"
#include "ProgressiveBonusingAvailable.h"
#include "ProgressiveDataConsumer.h"
#include "ProgressiveHostManager.h"
#include "ProgressiveMessageFactory.h"
#include "ProgressiveConfig.h"
#include "ProgressiveProtocol/ProgressiveMulticastHost.h"
#include "ProgressiveProtocol/ProgressiveQueueToOneLink.h"
#include "ProgressiveTransactionResendList.h"
#include "ProgressiveProtocol/ProgressiveMulticastEncryptionConfiguration.h"
#include "ProgressiveProtocol/AES128EncryptionUtil.h"
#include "ProgressiveProtocol/ProgressiveAnnouncementsInfo.h"
#include <Payout.h>
#include "PendingAction.h"
#include "CurrentActivity.h"

namespace SentinelNativeUnitTests { class ProgressiveConnectionUnitTests; }
namespace SentinelNativeUnitTests { class ProgressiveConnectionUnitTests2; }
namespace SentinelNativeUnitTests { class ProgressiveLevelTests; }
namespace SentinelNativeUnitTests { class WebDiagnostics_Progressive; }
namespace SentinelNativeUnitTests { class ProgressiveBonusingAvailableTests; }

/// <summary>
/// Handles the connection to the progressive system
/// This encapsulates the STOMP protocol interface
/// </summary> 

class CProgressiveConnection : public cms::MessageListener, public ProgressiveMulticastHost
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CProgressiveConnection" /> class.
	/// </summary>
	/// <param name="dataConsumer">The data consumer.</param>
	/// <param name="progConfig">The CProgressiveConfig.</param>
	/// <param name="paltronicsCertificatePath">The Paltronics certificate path.</param>
	/// <param name="firmwareVersion">The firmware version.</param>
	/// <param name="operatingSystem">An interface to operating system functionality.</param>
	CProgressiveConnection(CProgressiveDataConsumer &dataConsumer, CProgressiveConfig *progConfig,
		const std::string &paltronicsCertificatePathname,
		const std::string &firmwareVersion = "",
		IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

	/// <summary>
	/// Finalizes an instance of the <see cref="CProgressiveConnection"/> class.
	/// </summary>
	virtual ~CProgressiveConnection();

	/// <summary>
	/// Loads the progressive configuration.
	/// </summary>
	/// <param name="m_progConfig">The m_prog configuration.</param>
	void LoadProgressiveConfig(CProgressiveConfig *progConfig);

	/// <summary>
	/// Loads the device connection data.
	/// </summary>
	/// <param name="connection">The connection.</param>
	void LoadDeviceConnectionData(CConnection *connection);

	/// <summary>
	/// Sets the games data.
	/// </summary>
	/// <param name="games">The games.</param>
	/// <param name="gameData">IN - The game data.</param>
	void SetGames(CGames *games, CGameEndData *gameData);

	/// <summary>
	/// Performed the connection timed events.
	/// </summary>
	void TimedEvents();

	/// <summary>
	/// Sends the Card Entry data
	/// </summary>
	/// <param name="cardEntry">ICardEvent</param>
	void SendICardEvent(GeneralPollException generalPollException);

	/// <summary>
	/// Sends the offline coin in event.
	/// </summary>
	/// <param name="coinIn">IN - the current coin in meter value.</param>
	void SendOfflineCoinInEvent(__int64 coinIn);

	/// <summary>
	/// Sends the progressive level hit.
	/// </summary>
	/// <param name="award">IN - The EGM award for the progressive hit.</param>
	void SendProgressiveLevelHit(const CEGMAward &award);

	/// <summary>
	/// Sends a jackpot paid.
	/// </summary>
	/// <param name="jackpotPaid">IN - The jackpot paid information.</param>
	/// <remarks>
	/// A message will not be sent unless a valid message can be created for the CEGMAward instance.
	/// </remarks>
	void SendJackpotPaidRequest(const CEGMAward& jackpotPaidInfo);

	/// <summary>
	/// Sets the dependent communication status.
	/// </summary>
	/// <param name="commOK">Indicates if the dependant communications are ok.</param>
	/// <param name="multicastActive">IN - true when nCompass is receiving multicast messages adequately, false otherwise.</param>
	void SetDependentCommunicationStatus(bool commOK, bool multicastActive);

	// #pragma region Mockable Functions
	/// <summary>
	/// Sends the CardStatus message.
	/// </summary>
	virtual void SendCardStatus();
	// #pragma endregion Mockable Functions

	/// <summary>
	/// Sets the player data.
	/// </summary>
	/// <param name="player">IN - The player. NULL indicates no card present.</param>
	/// <param name="sessionActive">IN - true if a (carded or uncarded player) session is active, false otherwise.</param>
	/// <param name="sessionId">IN - The session identifier.</param>
	void SetPlayer(const CPlayer* player, bool sessionActive, int sessionId);

	/// <summary>
	/// Update the game data member variables.
	/// </summary>
	/// <param name="gameData">IN - The game data.</param>
	/// <returns>true if card status changes occurred, false otherwise.</returns>
	bool UpdateGameData(const CGameEndData *gameData);

	/// <summary>
	/// Send coin in message
	/// </summary>
	/// <param name="gameData">The game data.</param>
	void SendCoinIn(CGameEndData *gameData);

	/// <summary>
	/// Sends the game end message.
	/// </summary>
	/// <param name="gameData">The game data.</param>
	void SendGameEnd(CGameEndData *gameData);

	/// <summary>
	/// Sends the payout data
	/// </summary>
	/// <param name="payout">The payout data</param>
	void SendPayout(CPayout* payout);

	/// <summary>
	/// Sends the campaign download request data
	/// </summary>
	/// <param name="campaignId">The campaignId to request</param>
	void SendCampaignDownloadRequest(DWORD campaignId);

	/// <summary>
	/// Sends the Skin download request data
	/// </summary>
	/// <param name="skinId">The skinId to request</param>
	void SendSkinDownloadRequest(DWORD campaignId);
	
	/// <summary>
	/// Sends the Dashboard status to onelink
	/// </summary>
	/// <param name="campaignId">campaign id</param>
	/// <param name="campaignRevision">campaign revision</param>
	/// <param name="skinId">skin id</param>
	/// <param name="skinRevision">skin revision</param>
	/// <param name="currentActivity">current activity</param>
	/// <param name="pendingAction">pending action</param>
	void SetProgressiveDashboardStatus(DWORD campaignId, DWORD campaignRevision,
		DWORD skinId, DWORD skinRevision, WORD currentActivity, WORD pendingAction);

	/// <summary>
	/// Creates a status string based on activity and actions
	/// </summary>
	/// <param name="currentActivity">The current activity.</param>
	///  <param name="pendingAction">The pending actions.</param>
	std::string CreateStatusString(CurrentActivity currentActivity, PendingAction pendingAction);

	/// <summary>
	/// Sends the promo game acceptance status message.
	/// </summary>
	/// <param name="promoGameAcceptanceStatus">The promo game acceptance status object.</param>
	void SendPromoGameAcceptanceStatus(CPromoGameAcceptanceStatus* promoGameAcceptanceStatus);

	/// <summary>
	/// Sends the config file download acknowledgement message.
	/// </summary>
	/// <param name="messageRefId">The message ref id.</param>
	/// <param name="filename">The filename.</param>
	void SendConfigFileDownloadAck(const std::string &messageRefId, const std::string &filename);

	/// <summary>
	/// Processes a game data update.
	/// </summary>
	/// <param name="gameData">IN - The game data.</param>
	void ProcessGameData(const CGameEndData *gameData);

	/// <summary>
	/// Called from the m_consumer since this class is a registered MessageListener.
	/// </summary>
	/// <param name="message">The message.</param>
	virtual void onMessage(const cms::Message* message);

	/// <summary>
	/// Sets the current denom.
	/// </summary>
	/// <param name="currentDenom">The current denom.</param>
	void SetCurrentDenom(int currentDenom);

	/// <summary>
	/// Sends a queued message to OneLink.
	/// </summary>
	/// <param name="pMsg">The message to send. If nullptr, nothing will happen and no
	///		error will be logged.</param>
	void SendMessageToOneLink(const ProgressiveQueueToOneLink* pMsg);

	/// <summary>
	/// Sends an EgmStateResponse message to OneLink.
	/// </summary>
	/// <param name="enabled">Is machine enabled.</param>
	/// <remarks> Currently only "Enabled" supported</remarks>
	void SendEgmStateResponse(bool enabled);

	static bool m_multicastSocketFailed;

protected:
	/// <summary>
	/// Receives a JSON STOMP message from ProgressiveMulticastChannel.
	/// </summary>
	/// <param name="message">The STOMP frame with a JSON body.</param>
	virtual void OnMulticastMessage(ProgressiveMulticastFrame& frame);

	void UpdateConnectionState(ProgressiveConnectionState connectionState, ProgressiveConnectionError connectionError = ProgressiveConnectionErrorNone);
	cms::BytesMessage* CreateBytesMessage(const char* pMessageType, const std::string& messageBody);

	void CheckResendList();

protected:
	//The following variables are made protected so that the mock object of of this class
	//will have access to them.
	CProgressiveConnectionInfo m_connectionInfo;
	std::string m_paltronicsCertificatePathname;
	CProgressiveHostManager *m_hostManager;
	CProgressiveMessageFactory *m_messageFactory;
	CGames *m_games;
	CPlayer *m_player;
	bool m_dependentCommunicationsOK;
	DWORD m_uniqueId;
	CAES128EncryptionUtil* m_aes128EncryptionUtil;
	long m_campaignId;
	long m_campaignRevision;
	long m_skinId;
	long m_skinRevision;
	PendingAction m_pendingAction;
	CurrentActivity m_currentActivity;

private:
	CProgressiveConnection(const CProgressiveConnection&);
	CProgressiveConnection& operator=(const CProgressiveConnection&);

	// #pragma region Mockable Functions
protected:
	//The following variables are made protected so that the mock object of of this class
	//will have access to them.
	virtual void WatchdogCheckIn();
	virtual	void ConnectToConcentrator(std::string url);
	virtual bool IsTimeForConnectionAttempt();
	virtual void AddressCheck();
	virtual bool IsTimeForAddressCheck();
	virtual bool IsTimeForRediscovery();
	virtual cms::BytesMessage* CreateBytesMessage();

	virtual bool InternalSendMessageToOneLink(ToOneLink::Type messageType, const char* pMessageType, const std::string& messageBody);
	virtual bool SendMessageToOneLink(ToOneLink::Type messageType, const std::string& messageBody);
	// #pragma endregion Mockable Functions

private:
	void CheckDiscoveryStart();
	void StartDiscovery();
	void Connect();
	template <typename T> void DeleteActiveMqObject(T*& activeMqObject);
	template <typename T> void CloseActiveMqObject(T*& activeMqObject);
	template <typename T> void StopActiveMqObject(T*& activeMqObject);
	void Disconnect();
	bool IsFactoryPopulatedFromDiscovery();
	void ClearPlayer();
	bool IsTimeForPing();
	void SetLastHeartbeatTickCount(ToOneLink::Type messageType);
	void SendHostUpdate();

	//Incoming messages
	void ProcessBonusingAvailable(const rapidjson::Document& jsonDoc);
	void ProcessDiscoveryEvent(const rapidjson::Document& jsonDoc);
	void ProcessICardConfiguration5(const rapidjson::Document& jsonDoc);
	void ProcessDeviceRestartRequest(const rapidjson::Document& jsonDoc);
	void ProcessEgmStateRequest(const rapidjson::Document& jsonDoc);
	void ProcessJackpotHit(const rapidjson::Document& jsonDoc);
	void ProcesOfferRequest(const rapidjson::Document& jsonDoc);
	void ProcessMulticastEncryptionConfig(const rapidjson::Document& jsonDoc);
	void ProcessPromoGameRequest(const rapidjson::Document& jsonDoc);
	void ProcessFirmwareDownload(const rapidjson::Document& jsonDoc);
	void ProcessConfigFileDownload(const rapidjson::Document& jsonDoc);
	void ProcessCampaignDownload(const rapidjson::Document& jsonDoc);
	void ProcessSkinDownload(const rapidjson::Document& jsonDoc);
	void ProcessScheduleEnd(const rapidjson::Document& jsonDoc);
	void VerifyDeviceFields(CProgressiveTransactionItem& resendItem);

	//Outgoing messages
	void SendCardConfigurationRequest();

private:
	CProgressiveDataConsumer &m_progressiveDataConsumer;
	IOperatingSystem &m_operatingSystem;
	std::string m_firmwareVersion;

	//ActiveMQ variables
	cms::Connection* m_connection;
	cms::Session* m_session;
	bool m_sessionActive;
	int m_sessionId;
	bool m_multicastActive; // true when nCompass is receiving multicast messages adequately, false otherwise.
	bool m_haveGameData; // true when m_currentGameNumber and m_currentDenomination are valid, false otherwise.
	int  m_currentGameNumber;
	__int64 m_currentDenomination;
	cms::Destination* m_consumerDestination;
	cms::Destination* m_producerDestination;
	cms::MessageConsumer* m_consumer;
    cms::MessageProducer *m_producer; 

	std::recursive_mutex m_criticalSection;
	CProgressiveTransactionResendList* m_transactionResendList;

	bool m_performDisconnect;
	bool m_disconnecting;

	friend SentinelNativeUnitTests::ProgressiveConnectionUnitTests;
	friend SentinelNativeUnitTests::ProgressiveConnectionUnitTests2;
	friend SentinelNativeUnitTests::ProgressiveLevelTests;
	friend SentinelNativeUnitTests::WebDiagnostics_Progressive;
	friend SentinelNativeUnitTests::ProgressiveBonusingAvailableTests;
};

