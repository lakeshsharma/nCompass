#pragma once

#include "Connection.h"
#include "Player.h"
#include "Games.h"
#include "EGMAward.h"
#include "GameEndData.h"
#include "RapidJsonHelper.h"
#include "SASProtocol/SASGeneralException.h"
#include "ProgressiveOfferRequest.h"

namespace SentinelNativeUnitTests {	class ProgressiveMessageFactoryUnitTests; }

/// <summary>
/// This class will generate the messages that are to be sent to the progressive controller.
/// It will be populated with data during the discovery process that will be used in the
/// creating of the messages
/// </summary>
class CProgressiveMessageFactory
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CProgressiveMessageFactory"/> class.
	/// </summary>
	/// <param name="connection">The connection data.</param>
	/// <param name="firmwareVersion">The firmware version.</param>
	CProgressiveMessageFactory(CConnection &connection, const std::string &firmwareVersion = "");

	/// <summary>
	/// Finalizes an instance of the <see cref="CProgressiveMessageFactory"/> class.
	/// </summary>
	virtual ~CProgressiveMessageFactory();

	/// <summary>
	/// Determines whether factory is populated from discovery.
	/// </summary>
	/// <returns>true if it has been populated by discovery</returns>
	bool IsPopulatedFromDiscovery();

	/// <summary>
	/// Clears the discovery data.
	/// </summary>
	void ClearDiscoveryData();

	/// <summary>
	/// Creates the card configuration request.
	/// </summary>
	/// <param name="pGames">The games.</param>
	/// <returns>CardConfigurationRequest JSON string</returns>
	std::string CreateICardConfigurationRequest5(const CGames* pGames);

	/// <summary>
	/// Creates the device acknowledgement.
	/// </summary>
	/// <param name="pMessageRefId">The MessageId from ICardConfiguration5.</param>
	/// <param name="success">Whether to indicate success or failure.</param>
	/// <param name="pDescription">Optional description to send with the acknowledgement.</param>
	/// <returns>DeviceAcknowledgement json string</returns>
	/// <remarks>
	///	success and pDescription are independent of each other. however when success
	/// is false, a description should be included for logging purposes.
	/// </remarks>
	std::string CreateDeviceAcknowledgement(const char* pMessageId, bool success = true, const char* pDescription = nullptr);

	/// <summary>
	/// Creates the Card Event notification.
	/// </summary>
	/// <param name="generalPollException">The GeneralPollException.</param>
	/// <returns>JSON string</returns>
	std::string CreateICardEvent(GeneralPollException generalPollException);

	/// <summary>
	/// Gets the XML string for an offline coin in event.
	/// </summary>
	/// <param name="coinIn">IN - The current coin in meter value.</param>
	/// <returns>paltronicsEvent as xml string</returns>
	std::string CreateOfflineCoinInEvent(__int64 coinIn);

	/// <summary>
	/// Creates the jackpot paid.
	/// </summary>
	/// <param name="jackpotPaidInfo">IN - The jackpot paid information.</param>
	/// <returns>JackpotPaid xml string</returns>
	/// <remarks>
	/// Returns an empty string to indicate an invalid message.
	/// </remarks>
	std::string CreateJackpotPaid(const CEGMAward& jackpotPaidInfo);

	/// <summary>
	/// Creates the CardStatus xml string.
	/// </summary>
	/// <param name="player">IN - The current player. May be nullptr.</param>
	/// <param name="sessionId">IN - The (carded/uncarded, active/inactive player) session ID.</param>
	/// <param name="isPollingActive">IN - true if EGM polling is active, false otherwise.</param>
	/// <param name="isSessionActive">IN - true if there is an active (carded or uncarded) player session, false otherwise.</param>
	/// <param name="isMulticastActive">IN - true if we are receiving multicast data on each configured channel, false otherwise.</param>
	/// <param name="gameNumber">IN - The EGM game number currently selected by the player.  MAY be nullptr.</param>
	/// <param name="denomination">IN - The EGM denomination currently selected by the player.  MAY be nullptr.</param>
	/// <param name="campaignId">IN - The Campaign Id on the UI.</param>
	/// <param name="campaignRevision">IN - The Campaign revision on the UI.</param>
	/// <param name="skinId">IN - The Skin Id on the UI.</param>
	/// <param name="skinRevision">IN - The Skin revision on the UI.</param>
	/// <param name="status">IN - The Status sentinel.  MAY be nullptr.</param>
	/// <returns>CardStatus xml string</returns>
	/// <remarks>
	/// isMulticastActive: Set to true if the device is not configured to use receive multicast from One Link.
	/// Set to false to indicate that the device is not receiving multicast on one or more configured channels.
	/// - One Link generates events/alerts based on this, so that IT can know that multicast is not being received
	///   on a particular section of the network.
	/// Otherwise, set to true.
	/// </remarks>
	std::string CreateICardStatus(
		const CPlayer *player,
		int sessionId,
		bool isPollingActive,
		bool isSessionActive,
		bool isMulticastActive,
		int *gameNumber,
		__int64 *denomination,
		long campaignId,
		long campaignRevision,
		long skinId,
		long skinRevision,
		std::string status);

	/// <summary>
	/// Creates the coin in message.
	/// </summary>
	/// <param name="gameData">IN - The game end data.</param>
	/// <param name="player">IN - The current player. May be nullptr.</param>
	/// <param name="sessionId">IN - The (carded/uncarded, active/inactive player) session ID.</param>
	/// <param name="egmCommReconnect">IN - True if this message represents coin in while SAS comms down.</param>
	/// <param name="reconnect">IN - True if this message represents coin in while connection to concentrator is down.</param>
	/// <returns>the json representation of a CoinIn5 message.</returns>
	/// <param name="egmCommReconnect">IN - True if this message represents coin in while SAS comms down.</param>
	/// <param name="reconnect">IN - True if this message represents coin in while connection to concentrator is down.</param>
	std::string CreateCoinIn5(const CGameEndData &gameData, const CPlayer *player, int sessionId, bool egmCommReconnect = false, bool reconnect = false);

	/// <summary>
	/// Creates the game end message.
	/// </summary>
	/// <param name="gameData">IN - The game data.</param>
	/// <param name="player">IN - The current player. May be nullptr.</param>
	/// <param name="sessionId">IN - The (carded/uncarded, active/inactive player) session ID.</param>
	/// <param name="reconnect">IN - True if this message represents coin in while connection to concentrator is down.</param>
	/// <returns>the json representation of a GameEnd5 message.</returns>
	std::string CreateGameEnd5(const CGameEndData &gameData, const CPlayer *player, int sessionId, bool reconnect = false);

	/// <summary>
	/// Creates the payout meter delta message.
	/// </summary>
	/// <param name="payoutMeterDelta">IN - The payout meter delta data</param>
	/// <param name="pPlayer">IN - The current player. May be nullptr.</param>
	/// <param name="sessionId">IN - The (carded/uncarded, active/inactive player) session ID.</param>
	/// <returns>the json representation of a PayoutMeteDelta message.</returns>
	std::string CreatePayout(const CPayout* payout, const CPlayer* pPlayer, int sessionId);

	/// <summary>
	/// Creates the campaign download request message.
	/// </summary>
	/// <param name="campaignId">IN - The campaign id</param>
	/// <returns>the json representation of a Campaign Download Request message.</returns>
	std::string CreateCampaignDownloadRequest(const DWORD campaignId);

	/// <summary>
	/// Creates the skin download request message.
	/// </summary>
	/// <param name="skinId">IN - The skin id</param>
	/// <returns>the json representation of a Skin Download Request message.</returns>
	std::string CreateSkinDownloadRequest(const DWORD skinId);
	
	/// <summary>
	/// Creates the promo game acceptance status message.
	/// </summary>
	/// <param name="promoGameAcceptanceStatus">IN - The promo game acceptance status object.</param>
	/// <returns>the json representation of a GameEnd5 message.</returns>
	std::string CreatePromoGameAcceptanceStatus(const CPromoGameAcceptanceStatus& promoGameAcceptanceStatus);

	/// <summary>
	/// Creates the config file download ack message.
	/// </summary>
	/// <param name="messageRefId">The message ref id.</param>
	/// <param name="filename">The filename.</param>
	/// <returns>the json representation of ConfigFileDownloadAcknowledgement message.</returns>
	std::string CreateConfigFileDownloadAck(const std::string &messageRefId, const std::string &filename);

	/// <summary>
	/// Updates the factory with data from configuration.
	/// </summary>
	/// <param name="ICardConfiguration5">The ICardConfiguration5 json document.</param>
	void UpdateCardConfiguration(const rapidjson::Document& ICardConfiguration5);

	/// <summary>
	/// Creates the progressive level hit.
	/// </summary>
	/// <param name="award">IN - The EGM award for the progressive hit.</param>
	/// <param name="reconnect">IN - True if this message represents coin in while connection to concentrator is down.</param>
	/// <returns>the json representation of a Progressive Level Hit</returns>
	std::string CreateProgressiveLevelHit(const CEGMAward &egmAward, bool reconnect = false);

	/// <summary>
	/// Creates EgmStateResponse message to OneLink.
	/// </summary>
	/// <param name="enabled">Is machine enabled.</param>
	/// <remarks> Currently only "Enabled" supported</remarks>
	std::string CreateEgmStateResponse(bool enabled);

	/// <summary>
	/// Creates the offer acceptance response message.
	/// </summary>
	/// <param name="jackpotPaidInfo">IN - The jackpot paid information.</param>
	/// <returns>JackpotPaid xml string</returns>
	/// <remarks>
	/// Returns an empty string to indicate an invalid message.
	/// </remarks>
	std::string CreateOfferAcceptanceResponse(const CProgressiveOfferRequest& progressiveOfferRequest);

	/// <summary>
	/// Gets the MAC address with colon.
	/// </summary>
	/// <returns>MAC Address</returns>
	std::string GetMacAddressWithColon() const;

	/// <summary>
	/// Gets the MAC address without colon.
	/// </summary>
	/// <returns>MAC Address</returns>
	std::string GetMacAddressWithoutColon() const;

	///	<summary>
	/// Gets the source device ID.
	///	</summary>
	///	<returns>The source device ID.</returns>
	LONG64 GetSourceDeviceID() const;

	///	<summary>
	///	Returns a pointer to the query poker cards set.
	///	</summary>
	///	<returns>A pointer to the query poker cards set.</returns>
	void SetQueryPokerCardsSet(std::set<DWORD>& queryCards);

	///	<summary>
	/// Returns the actual IP.
	///	</summary>
	///	<returns>The actual IP.</returns>
	const std::string& GetActualIP() const;

	///	<summary>
	///	Sets the actual IP.
	///	</summary>
	///	<param name="actualIP">The actual IP.</param>
	///	<returns><b>true</b> if the IP changed, else <b>false</b>.</returns>
	bool SetActualIP(const std::string& actualIP);

	///	<summary>
	///	Gets device attribute string portion of a onelink message.
	///	</summary>
	///	<returns>Device attribute message string.</returns>
	std::string GetDeviceAttributesString();

private:
	static const char* pWebDiagnosticsProtocol;
	static const char* pWebDiagnosticsPage;

	/// <summary>
	/// This is the flag indicating that this object was populated from the discovery process
	/// </summary>
	bool m_isPopulatedFromDiscovery;
		
	/// <summary>
	/// The IP address needed for messages
	/// </summary>
	std::string m_actualIP;
		
	/// <summary>
	/// The MAC address
	/// </summary>
	std::string m_macAddress;

	/// <summary>
	/// This will be the source device ID sent in messages.
	/// This is set in UpdateCardConfiguration(), where we expect the progressive system to provide our Slotmast ID.
	/// It is also used to echo 64-bit device IDs for acknowledgements, when it should also contain our Slotmast ID.
	/// </summary>
	LONG64 m_sourceDeviceID;

	int64_t m_sourceDeviceConfigVersion;

	/// <summary>
	/// The sentinel version sent in messages
	/// </summary>
	std::string m_sentinelVersion;

	/// <summary>
	/// The message counter. This is okay if it wraps. 
	/// This is appended to the timestamp in order to make a messageID.
	/// </summary>
	WORD m_messageCounter;

	/// <summary>
	/// The source device returned in discovery
	/// </summary>
	std::string m_sourceDevice;

	/// <summary>
	/// The source asset returned in discovery. This may be blank
	/// </summary>
	std::string m_sourceAsset;

	/// <summary>
	/// The slot number
	/// </summary>
	long m_slotNumber;

	/// <summary>
	/// The firmware version
	/// </summary>
	std::string m_firmwareVersion;

	/// <summary>
	/// The OS version
	/// </summary>
	std::string m_osVersion;

	std::set<DWORD> m_queryPokerCardsSet;

	const CSASGeneralException m_sasGeneralException;

private:
	void WriteGameData(const CGame* pGame, JsonWriter& writer, unsigned gameNumber);
	void WriteCommonGameEndData(const CGameEndData &gameData, const CPlayer *pPlayer, int sessionId, JsonWriter& writer);
	void WriteMessageId(JsonWriter& writer);
	void PopulateDeviceAttributes(JsonWriter& writer);
	void WritePatronInformation(JsonWriter& writer, const CPlayer* pPlayer);
	void WriteResolution(JsonWriter& writer);

	friend SentinelNativeUnitTests::ProgressiveMessageFactoryUnitTests;
};

