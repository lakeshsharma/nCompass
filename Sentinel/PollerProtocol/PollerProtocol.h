#pragma once
#include "../SystemProtocol.h"
#include "Connection.h"
#include "PollerMessage.h"
#include "../GlobalDefs.h"
#include "Hardware/WatchDog.h"
#include "PollerCommunicator.h"
#include "PollerChangeSlotMastIDMessage.h"
#include "PollerChangeSlotMastIDRespMessage.h"
#include "PollerHeartbeatMessage.h"
#include "PollerHeartbeatRespMessage.h"
#include "PollerConfigRequestMessage.h"
#include "PollerConfigRespMessage.h"
#include "PollerStatusMessage.h"
#include "PollerMetersMessage.h"
#include "PollerSubGameMetersMessage.h"
#include "PollerGlobalPasswordMessage.h"
#include "PollerDisplayMessage.h"
#include "PollerTimeMessage.h"
#include "PollerPlayerTransactionMessage.h"
#include "TransactionResendList.h"
#include "PollerTransactionAckMessage.h"
#include "PollerPlayerResponseMessage.h"
#include "PollerSlotmastTransactionMessage.h"
#include "PollerBasicTransactionMessage.h"
#include "PollerTiltTransactionMessage.h"
#include "PollerPlayerBonusEventsMessage.h"
#include "PollerPlayerUpdateEventsMessage.h"
#include "PollerBonusTransactionMessage.h"
#include "PollerCTTTransactionMessage.h"
#include "PollerTicketTransactionMessage.h"
#include "PollerTicketResponseMessage.h"
#include "PollerEmployeeResponseMessage.h"
#include "PollerPersonalBankerResponseMessage.h"
#include "../PersonalBankerTransfer.h"
#include "PollerPBTransactionMessage.h"
#include "PollerEGMSnapshotTransactionMessage.h"
#include "PollerSubGameSnapshotTransactionMessage.h"
#include "PollerMachineMoveTransactionMessage.h"
#include "PollerPinTransactionMessage.h"
#include "PollerJackpotFillTransactionMessage.h"
#include "../Jackpot.h"
#include "PollerFailureCodeMessage.h"
#include "PollerJackpotTransactionMessage.h"
#include "PollerExternalBonusTransaction.h"
#include "PollerMechMaintTransactionMessage.h"
#include "PollerSerialCommDownTransactionMessage.h"
#include "PollerConfigChangeTransactionMessage.h"
#include "PollerBillAcceptedTransactionMessage.h"
#include "PollerLossLimitBalanceUpdate.h"
#include "PollerRequestLossLimitBalanceMessage.h"
#include "PollerPlayerOmniCardBalanceResponseMessage.h"
#include "PollerAuthorizationMessage.h"
#include "PollerProcessUJPMessage.h"
#include "PollerAuthorizePINResponseMessage.h"
#include "PollerAuthorizePINTransactionMessage.h"
#include "PollerUJPResultTransactionMessage.h"
#include "../FlashStatus.h"
#include "PollerGameSessionChangeTransactionMessage.h"
#include "SubgameSessionParameters.h"
#include "PollerSubgameSelectionMessage.h"
#include "PollerSubgameSelectionResponseMessage.h"
#include "PollerGamesListMessage.h"
#include "PollerGameListInfo.h"
#include "IPollerProtocolInterface.h"
#include "PollerRebootRequestMessage.h"
#include "PollerPBPinResetResponseMessage.h"
#include "PollerMobileConnectTransactionMessage.h"
#include "PollerMobileConnectResponseMessage.h"
#include "PollerProximityBeaconScanListResponseMessage.h"
#include "MobileConnectProtocol/MobileConnectMessageFactory.h"
#include "MobileConnectProtocol/PairingInformation.h"
#include "MobileConnectProtocol/PairingStatusEvent.h"
#include "MobileConnectProtocol/PairingSettingsEvent.h"
#include "MobileConnectProtocol/ProximitySettingsEvent.h"
#include "MobileConnectProtocol/ProximityStatusEvent.h"

union u_ConnectivityStatus
{
	// first representation (member of union) 
	struct tFlags
	{
		WORD isTcpConnected		: 1;//Bit1
		WORD isPollerOnline		: 1;//Bit2
		WORD isPMTUp			: 1;//Bit3
		WORD isMMTUp			: 1;//Bit4
		WORD isVoucherServiceUp : 1;//Bit5
		WORD m_spare			: 11;//Bits 6-16
	} flags;

	// second representation (member of union) 
	WORD uValue;

	u_ConnectivityStatus()
	{
		uValue = 0;
	}

	bool IsPollerReadyForSubgameList() const
	{
		LogStatus("IsPollerReadyForSubgameList");
		return flags.isPollerOnline && flags.isTcpConnected && flags.isPMTUp && flags.isMMTUp;
	}

	void LogStatus(const std::string& functionName) const
	{
		LogString(POLLER_LOG, "%s TCP=%u Poller=%u PMT=%u MMT=%u VS=%u",
			functionName.c_str(),
			flags.isTcpConnected,
			flags.isPollerOnline,
			flags.isPMTUp, 
			flags.isMMTUp, 
			flags.isVoucherServiceUp);
	}
};

enum class EnumConfigState
{
	SendRequest,
	RequestSent,
	Received,
};

const DWORD RECEIVE_TIMEOUT = 30000;

// Forward declare class to declare friend for unit test access.
namespace SentinelNativeUnitTests { class InternalsFrameworkUnitTests; }
namespace SentinelNativeUnitTests { class PollerProtocolUnitTests; }
namespace SentinelUnitTests { class PollerGamesListMessageUnitTests; }

class CPollerGameListInfo;

class CPollerProtocol :
	public CSystemProtocol, public CPollerProtocolInterface
{

private:
	//friend class declarations for unit test access.
	friend class SentinelNativeUnitTests::InternalsFrameworkUnitTests;
	friend class SentinelNativeUnitTests::PollerProtocolUnitTests;
	friend class SentinelUnitTests::PollerGamesListMessageUnitTests;

public:
	CPollerProtocol(const CPairingInformation& mobilePairingInfo, CQueueList &scfToInternalsQueue);
	virtual ~CPollerProtocol(void);

	virtual void PerformTimedEvents();

	void ProcessMessageFromInternals(CInternalMessage *msg);
	void ProcessMCCommunicationSupportFromInternals(CInternalMessage* msg);
	virtual void LowerTimeoutsForShutdown();
   
        //Making Virtual for UT
	virtual void AddToPollerQueue(CPollerMessage *pollerMsg, int priority);

	bool SendToPoller(CPollerMessage *pollerMsg);
	bool SendToPoller(byte *pollerMsg, WORD transLength, DWORD uniqueId, bool alreadySuspicious);
	bool IsPmtTransactionsUp();

	DWORD GetUniqueId();

	/// <summary>
	/// Read and process messages from the Poller.
	/// </summary>
	void ReadAndProcessFromPoller( void );

	CQueueList * GetPollerQueue(void)
	{
		return m_pollerMsgQueue;
	}

	void CheckForConnect( void );

	void SendHeartbeatIfNecessary();

	bool IsResendItemListEmpty();
	byte *GetFirstResendItem(WORD & transLength, DWORD &outUniqueId, DWORD &outLastSent, bool &alreadySuspicious );
	byte *GetNextResendItem(WORD & transLength, DWORD &currentUniqueId, DWORD &currentLastSent, bool &alreadySuspicious );
	bool SetLastSent(DWORD UniqueId);

	bool m_nvramValid;
	bool m_nvramMsgReceived;
	bool IsConnected();
	void LockResendList();
	void UnlockResendList();

	/// <summary>
	/// Gets the  the voucher separation flag.
	/// </summary>
	bool VoucherSeparationEnabled(void);

	/// <summary>
	/// Sets the voucher separation flag.
	/// </summary>
	void SetVoucherSeparationEnabled( bool voucherSeparationEnabled );

	/// <summary>
	/// Gets the  the MGMD flag.
	/// </summary>
	bool MGMDEnabled(void);

	/// <summary>
	/// Sets the MGMD flag.
	/// </summary>
	void SetMGMDEnabled(bool MGMDEnabled);

	/// <summary>
	/// Sets the ChunkedGamesList enabled flag.
	/// </summary>
	void SetChunkedGamesListEnabled(bool isChunkedGamesListEnabled);
	
	/// <summary>
	/// Checks if the voucher separation service enabled on pollar client , so ticket transaction is needed to send to the pollar
	///if PMT is down
	/// </summary>
	bool IsTranstoSendToPollarIfPMTDown(byte * resendItem);

	/// <summary>
	/// Checks if the voucher separation service enabled on pollar client , so ticket transaction is needed to send to the pollar
	///if PMT is down
	/// </summary>
	bool IsTranstoSendToPollarIfPMTDown(CPollerMessage *pollerMsg);

	/// <summary>
	///Get the Feature flag from Internal and process it to voucher feature flag from nvram while initilaising poller protocol
	/// </summary>
	bool  ProcessFeatureFlag(CInternalMessage *intMsg);

	/// <summary>
	///Send Game List to Poller.
	/// </summary>
	virtual void SendGameListToPoller();

	/// <summary>
	/// Validates bad transaction packet for maximum sending retires.
	/// </summary>
	/// <param name="uniqueId">IN - Unique ID of the transaction item.</param>
	/// <returns>false : if it is bad transaction and its max resend attempt is over. Also removes the bad transaction from resend list.</returns>
	/// <returns>true : otherwise, means transaction can be resend to poller.</returns>
	bool ValidateBadTransactionResendCount(DWORD uniqueId);

	/// <summary>
	/// It removes a resend item from the list with the unique ID passed in if necessary.
	/// </summary>
	/// <param name="uniqueId">IN - Unique ID of the transaction item.</param>
	/// <returns>true if a resend item is removed, false otherwise.</returns>
	bool RemoveResendListItemIfNecessary(DWORD uniqueId);

	/// <summary>
	/// to kick Watchdog.
	/// </summary>
	void WatchdogCheckIn()
	{
		CWatchDog::Instance().Checkin();
	}

protected:
	// Making it virtual for UT.
	virtual void ConnectToPoller(void);

private:
	static const DWORD MESSAGE_ID_OFFSET = 2; // First WORD is the packet version. Second WORD is the message ID.
	static const DWORD TRANSACTION_CODE_OFFSET = SIZE_OF_MESSAGE_HEADER; // Location is first byte after the message header.
	static const DWORD UNIQUE_ID_OFFSET = TRANSACTION_CODE_OFFSET + sizeof(BYTE) + sizeof(SYSTEMTIME); // BYTE is trans. code.

	PollerCommunicator *pc;

	CConnection *m_connection;

	// The tick count of the last heartbeat message sent to the poller.
	DWORD m_lastHBMessageSentTime;
	DWORD m_lastMessageReceivedTime;
	bool m_notifyResendEmpty;

	std::recursive_mutex m_criticalSection;

	std::string m_dataDrive;

	int m_ticketsBufferedCount;
	long m_slotMastID;
	long m_pendingSlotmastChange;
	BYTE  m_hostAddressOrName[SIZE_OF_NAME_IP];
	BYTE  m_macAddress[SIZE_OF_MAC_ADDRESS];
	DWORD m_sequenceNumber;   //TBD This should be stored in NVRAM!!
	WORD  m_slotMastRevision;
	const WORD m_packetVersion;
	WORD  m_pollerPacketVersion;
	u_ConnectivityStatus m_connectivityStatus;
	DWORD m_configRequestTime;
	bool m_bSendFullDataAfterConfig;
	byte m_versionNumberMajor;
	byte m_versionNumberMinor;
	byte m_versionBuild;
	byte m_versionRevision;
	DWORD m_imageChecksum;
	BYTE *tempsendpacket;
	BYTE *temprecvpacket;
	char m_readBuffer[SIZE_OF_READ_BUFFER];
	DWORD m_uniqueId;
	ResendState m_resendState;
	int m_receiveTimeoutCount;
	int m_receivedZeroCount;
	EnumConfigState m_configState;
	bool m_statusSendNeeded;
	bool m_pmtTransactionsUp;
	bool m_oneLinkProgressivesSenOptionSupported;
	std::string m_NewPollerIP;
	bool m_IsPollerIPChanged;
	bool m_performLoadBalancing;
	bool m_defaultIsPollerNFD;

	std::unique_ptr<std::thread> m_sendThread;
	std::unique_ptr<std::thread> m_readThread;
	// The external queue for messages going to the Poller
	CQueueList *m_pollerMsgQueue;

	// Keep this until we sent it successful
	CTransactionResendList *m_transactionResendList;

	// Map to maintain resend count of bad transaction item.
	std::map<DWORD, ushort> m_BadTxResendCountMap; // <uniqueId, resendCount>

	std::set<DWORD> m_listOfUniqueIdsNotToResend;
	u_ExtensibleFlags m_extensibleFlags;

	bool m_voucherSeparationEnabled;
	bool m_MGMDfeatureFlag;
	bool m_mobileConnectConfigEnabled;

	CPollerGameListInfo* m_pollergameListInfo;

	// Mobile pairing related
	CMobileConnectMessageFactory m_mobileConnectMessageFactory;
	DWORD m_lastPairingSettingsSendTicks;
	DWORD m_lastProximitySettingsRequestSendTicks;
	bool m_proximitySettingsResponseReceived;
	bool m_proximitySettingsRequestIssued;
	const CPairingInformation& m_mobilePairingInfo;
	const CPairingInformation& GetMobilePairingInformation() const;
	void SendMCPTransactionMessageToPoller( CInternalMessage *msg );

	// Messages from Internals
	void ProcessVersionFromInternals( CInternalMessage *msg );
	void ProcessSetMetersFromInternals( CInternalMessage *msg );
	void ProcessSetSlotMastIDFromInternals( CInternalMessage *msg );
	void ProcessSetConnectionFromInternals( CInternalMessage *msg );
	void ProcessSetMacAddressFromInternals( CInternalMessage *msg );
	void ProcessChangeSlotMastIDFromInternals( CInternalMessage *msg );
	void SetNvramStatus( CInternalMessage *msg );
	void ProcessSlotMastRevisionFromInternals( CInternalMessage *msg);
	void RemoveTicketFromResendList( CInternalMessage * msg );
	void ProcessPBTransfer( CInternalMessage * msg );
	void ProcessPBAuthorize( CInternalMessage * msg );
	void SendEmptyResendToInternals(CInternalMessage *msg);
	void ProcessReconnectFromInternals();
	void SendTicketOutCountToInternals();
    void ProcessDefaultDataDirectoryUpdated(CInternalMessage * msg);
	void SendStatusAfterParsingConfigFromInternals();
	void ProcessPerformLoadBalancingFromInternals();

	// Messages to Poller
	void SendHeartbeatToPoller(void);
	void SendRequestConfigToPoller(void);
	void SendStatusToPoller(void);
	void SendMetersToPoller( CGames *games, bool isEGM, DWORD playSessionId, DWORD playTxSeqNum );
	void SendCardInToPoller( CInternalMessage *msg );
	void SendCardOutToPoller( CInternalMessage *msg );
	void SendPlayerUpdateToPoller( CInternalMessage *msg );
	void SendGameSessionChange(CInternalMessage *msg);
	void SendTiltToPoller( CInternalMessage * msg );
	void SendBonusEventsToPoller( CInternalMessage * msg );
	void SendCashTransferToPoller( CInternalMessage * msg );
	void SendTicketTransactionToPoller( CInternalMessage * msg );
	void SendEmployeeCardToPoller( CInternalMessage * msg, bool cardIn );
	void SendPBToPoller( CInternalMessage * msg, PersonalBankerType ammendedPBtype );
	void SendSnapshotToPoller( CInternalMessage* msg );
	void SendMachineMoveToPoller( CInternalMessage *msg );
	void SendOneLinkTransactionToPoller(CInternalMessage *msg);
	void SendJackpotFillToPoller( CInternalMessage * msg  );
	void SendFailureCodeToPoller( CInternalMessage * msg );
	void SendMaintenanceToPoller( CInternalMessage * msg );
	void SendJackpotToPoller( CInternalMessage * msg );
	void SendExternalBonusToPoller(CInternalMessage *msg);
	void SendCommStatusToPoller(  CInternalMessage * msg );
	void SendBasicTransactionToPoller( CInternalMessage * msg );
	void SendConfigChangeNoticeToPoller( CInternalMessage *msg );
	void SendBillAcceptedToPoller( CInternalMessage *msg );
	void SendRequestLossLimitBalanceToPoller( CInternalMessage *msg );
	void SetIP( CInternalMessage *msg );
	void SendAuthorizePinToPoller(CInternalMessage *msg);
	void SendUJPResult(CInternalMessage *msg);
	void SendSubGameSelection(CInternalMessage *msg);
	void SendPBResetPinTransactionMessageToPoller(CInternalMessage * msg);
	void SendCardlessTransactionToPoller(CInternalMessage *msg);

	bool RemoveResendTransaction(DWORD uniqueId, bool isStoredInNVRAM);
	bool QueueToPoller(CPollerMessage*pollerMsg, byte msgType, DWORD uniqueId, byte *transPtr, DWORD transLength, bool isTicketOut = false);

	// Messages from Poller.
	void ProcessHeartbeatRespFromPoller( CPollerHeartbeatRespMessage *heartbeatRespMsg );
	void ProcessSentinelConfigMsgFromPoller(bool hdrDataLengthValid, PollerMessageHeader &hdr, const byte *dataPtr);
	void ProcessConfigFromPoller( CPollerConfigRespMessage *configMsg );
	void ProcessSentinelTimerFromPoller( CPollerTimeMessage *timeMsg );
	void ProcessDisplayMessageFromPoller( CPollerDisplayMessage *displayMsg );
	void ProcessGlobalPasswordFromPoller( CPollerGlobalPasswordMessage *globalPasswordMsg );
	void ProcessChangeSlotMastRespFromPoller( CPollerChangeSlotMastIDRespMessage *changeSlotMastRespMsg );
	void ProcessPlayerResponseFromPoller( CPollerPlayerResponseMessage *playerRespMsg);
	void ProcessPlayerGlobalBalanceResponseFromPoller( CPollerPlayerOmniCardBalanceResponseMessage *playerRespMsg);
	void ProcessPlayerBonusEventsFromPoller( CPollerPlayerBonusEventsMessage *pollerMsg);
	void ProcessPlayerUpdateEventsFromPoller( CPollerPlayerUpdateEventsMessage *pollerMsg);
	void ProcessTicketResponseFromPoller( CPollerTicketResponseMessage *pollerMsg );
	void ProcessEmployeeResponseFromPoller(CPollerEmployeeResponseMessage *pollerMsg);
	void ProcessPBTResponseFromPoller(CPollerPersonalBankerResponseMessage *pollerMsg);
	void ProcessLossLimitBalanceUpdateFromPoller(CPollerLossLimitBalanceUpdate *pollerMsg);
	void ProcessAuthorizationFromPoller(CPollerAuthorizationMessage *pollerMsg);
	void ProcessUJPFromPoller(CPollerProcessUJPMessage *pollerMsg);
	void ProcessAuthorizePINResponseFromPoller(CPollerAuthorizePINResponseMessage *pollerMsg);
	void ProcessSubgameSelectionResponseFromPoller(CPollerSubgameSelectionResponseMessage *pollerMsg);
	void ProcessRebootRequestFromPoller(CPollerRebootRequestMessage *pollerMsg);
	void ProcessPBPinResetResponseFromPoller(CPollerPBPinResetResponseMessage *pollerMsg);

	// Mobile Connect related messages
	void ProcessMCResponseFromPoller(CPollerMobileConnectResponseMessage *mobileConnectMsg);
	void ProcessMCEventPairingSettings(const rapidjson::Document& jsonDoc);
	void ProcessMCEventPairing(const rapidjson::Document &jsonDoc);
	void ProcessMCRequestCardChange(const rapidjson::Document &jsonDoc);
	void ProcessMCRequestBalance(const rapidjson::Document &jsonDoc);
	void ProcessMCRequestFundsTransfer(const rapidjson::Document &jsonDoc);
	void ProcessMCEndPBTSession();

	// Proximity related messages
	void ProcessProximityBeaconScanListResponseFromPoller(CPollerProximityBeaconScanListResponseMessage *pollerMsg);
	void ProcessEventProximitySettings(const rapidjson::Document &jsonDoc);

	// Utility
	void BeginOnlineProcessing( void );
	void SendOnlineToInternals( void );
	void SendPollerPacketSentVerifyErrorsToInternals();
	void SetMessageHeader(CPollerMessage * msg);
	void GetHeader(PollerMessageHeader * hdr, BYTE* temprecvpacket);
	void CheckResendUse( void );
	void SetOneLinkProgressivesSenOptionSupport(WORD oldPacketVersion, WORD packetVersion);
	void SendOneLinkProgressivesSenOptionSupportToInternals();
	void SendPacketTiltToInternals(const BYTE *packet, unsigned long packetLength, TiltCode tiltCode);
	void SendGenericTransactionToPoller(CInternalMessage *msg);
	void PrepareAndSendGameListChunksMessage(CParseGameList* parseGameList, time_t& currentTime, WORD chunkIndex, WORD gameListChunkoffset, WORD numberOfChunks, WORD gamesInChunk, WORD totalSubgames );
	void ProcessPollerIPFileIfExist();
	bool IsNFDPoller();

    //test
    void CreateAndSendFakeMetersMessage();
};
