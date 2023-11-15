#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <dlfcn.h>
#include <execinfo.h>
#include <typeinfo>
#include <string>
#include <memory>
#include <cxxabi.h>
#include <cstdlib>

//#include <unwind.h>
#include <cxxabi.h>

#include "PollerProtocol.h"
#include "PollerMGMDData.h"

//Sentinel includes.
#include "Logging/LogString.h"
#include "PollerCheckMessage.h"
#include "Diagnostics.h"
#include "Hardware/WatchDog.h"
#include "../Utilities.h"
#include "../Sentinel.h"
#include "../Meter.h"
#include "../UdpSocket.h"
#include "../MulticastSocket.h"
#include "../Platform.h"
#include "../PlayerGlobalBalance.h"

#include "PollerPBResetPinTransactionMessage.h"
#include "PollerOneLinkTransactionMessage.h"
#include "PollerGenericTransactionMessage.h"
#include "PollerCardlessSessionTransactionMessage.h"
#include "PollerMobileConnectResponseMessage.h"
#include "MobileConnectProtocol/MobileConnectProtocolKeys.h"

//#define ThrowInSendToPoller //- for testing exception logging only

using namespace std;

#define CYPHERBLOCKSIZE 8
#define UDP_TIMEOUT_MS 200

// Time interval to check the resend list.
const DWORD RESEND_CHECK_INTERVAL = 100;
const DWORD CONNECT_RETRY_TIMEOUT = 3000;
const DWORD READ_LOOP_WAIT_MSECS = 500;

const DWORD DELAY_GAME_LIST_SEND_CHUNKS(250);//250 ms
const long RESEND_PAIRING_SETTINGS_INTERVAL_IN_SECS = 300; // 5 minutes
const long RESEND_PROXIMITY_SETTINGS_INTERVAL_IN_SECS = 300; // 5 minutes

// Time interval a transaction will sit in the resend list before it is retried
const DWORD RESEND_TRANSACTION_INTERVAL = 30000;
static const bool DEFAULT_VOUCHERSEPARATION = false;
static const bool DEFAULT_MULTIGAMEMULTIDENOM = false;

// It is the number of times a bad transaction will be resend to poller.
// After this many retries, the transaction will be removed from the list.
const DWORD MAX_BAD_TX_RESEND_COUNT = 2;

static UINT AFX_CDECL SendToPollerThread( PVOID threadParam );
static void SendResendItems(CPollerProtocol *pollerProtocol, bool forceSend = false);
static UINT AFX_CDECL ReadFromPollerThread( PVOID threadParam );

CPollerProtocol::CPollerProtocol(const CPairingInformation& mobilePairingInfo, CQueueList &scfToInternalsQueue)
	: CSystemProtocol( scfToInternalsQueue, THREAD_POLLER_INTERNALS_QUEUE, INT2SCF_MessageQueueLogIdentifier ),
	m_packetVersion(POLLER_PROTOCOL_VERSION),
    m_voucherSeparationEnabled(DEFAULT_VOUCHERSEPARATION),
	m_MGMDfeatureFlag(DEFAULT_MULTIGAMEMULTIDENOM),
	m_mobileConnectMessageFactory(INVALID_SLOT_MAST_ID),
	m_mobilePairingInfo(mobilePairingInfo)
{
	//_packetbuilder = new PacketBuilder();
	m_mobileConnectConfigEnabled = false;
	m_nvramValid = FALSE;
	m_nvramMsgReceived = FALSE;
	tempsendpacket = new BYTE[ SIZE_OF_READ_BUFFER ];
	temprecvpacket = new BYTE[ SIZE_OF_READ_BUFFER ];
	// Set up the queue for messages going to the poller
	m_pollerMsgQueue = new CQueueList();

	pc = new PollerCommunicator();	// Create instance of the socket to poller

	m_bSendFullDataAfterConfig = false;

	m_resendState = RESEND_FILLING;

	m_readInternalsTimeout = HEARTBEAT_TIME;

	m_slotMastID = INVALID_SLOT_MAST_ID;
	m_pendingSlotmastChange = INVALID_SLOT_MAST_ID;
	m_sequenceNumber = 0;
	m_notifyResendEmpty = false;
	m_receiveTimeoutCount = 0;
	m_receivedZeroCount = 0;
	m_configState = EnumConfigState::SendRequest;
	m_statusSendNeeded = false;
	m_NewPollerIP.clear();
	m_IsPollerIPChanged = false;
	m_performLoadBalancing = false;
	m_defaultIsPollerNFD = false;
	m_proximitySettingsResponseReceived = false;
	m_proximitySettingsRequestIssued = false;

	m_connection = NULL;
	m_transactionResendList = NULL;

	m_configRequestTime = CUtilities::GetTickCount();
	m_lastHBMessageSentTime = CUtilities::GetTickCount();
	m_lastMessageReceivedTime = CUtilities::GetTickCount();
	m_lastPairingSettingsSendTicks = m_operatingSystem.GetTickCount32();
	m_lastProximitySettingsRequestSendTicks = m_operatingSystem.GetTickCount32();

	for (int i = 0; i < SIZE_OF_MAC_ADDRESS; i++)
	{
		m_macAddress[i] = 0;
	}

	m_versionNumberMajor = 0;
	m_versionNumberMinor = 0;
	m_versionBuild = 0;
	m_versionRevision = 0;
	m_imageChecksum = 0;
	m_ticketsBufferedCount = 0;
	m_pmtTransactionsUp = false;
	//this is the packet version of the poller we are communicating with
	m_pollerPacketVersion = 0;
	m_oneLinkProgressivesSenOptionSupported = DEFAULT_ONE_LINK_PROGRESSIVES_SEN_OPTION_SUPPORTED;

	// set unique id
	m_uniqueId = 0; //try to make this the last one in NVRAM once built

	// Start the Send thread.
	m_sendThread.reset(new thread(SendToPollerThread, this));
    CWatchDog::Instance().RegisterThread(THREAD_SEND_TO_POLLER, m_sendThread->get_id(), SEND_TO_POLLER_THREAD_TIMEOUT, m_sendThread->native_handle());

	// Start the Read thread.
	m_readThread.reset(new thread(ReadFromPollerThread, this));
    CWatchDog::Instance().RegisterThread(THREAD_READ_FROM_POLLER, m_readThread->get_id(), READ_FROM_POLLER_THREAD_TIMEOUT, m_readThread->native_handle());

	SendOneLinkProgressivesSenOptionSupportToInternals();
	m_pollergameListInfo = new CPollerGameListInfo(this);
}

CPollerProtocol::~CPollerProtocol(void)
{
	m_timeToClose = true;

	// wake up send thread for closing
	GetPollerQueue()->GetAddedItemEventHandle().signal(false);

	//wait for the send thread to close
	if (m_sendThread.get() != nullptr)
	{
		m_sendThread->join();
		delete m_sendThread.release(); // Need release to prevent gtest unit tests from hanging or misbehaving if destructor is called during an assert failure.
	}

	//wait for the read thread to end
	if (m_readThread.get() != nullptr)
	{
		m_readThread->join();
		delete m_readThread.release(); // Need release to prevent gtest unit tests from hanging or misbehaving if destructor is called during an assert failure.
	}

	if (tempsendpacket)
		delete [] tempsendpacket;

	if (temprecvpacket)
		delete [] temprecvpacket;

	if (pc)
		delete pc;

	if (m_transactionResendList)
		delete m_transactionResendList;

	if(m_pollerMsgQueue)
		delete m_pollerMsgQueue;

	if (m_connection)
		delete m_connection;
}

void CPollerProtocol::PerformTimedEvents( void )
{
	std::unique_lock<std::recursive_mutex> lock(m_criticalSection);

	if (pc && pc->Connected())
	{
		if (m_mobileConnectConfigEnabled &&
			(m_operatingSystem.GetTickCount32() - m_lastPairingSettingsSendTicks) >
			RESEND_PAIRING_SETTINGS_INTERVAL_IN_SECS * MILLISECONDS_PER_SECOND)
		{
			// If no reply from PDS or not connected to protocol
			const CPairingInformation &mobilePairingInfo = GetMobilePairingInformation();

			// check if we should be sending request.pairingSettings again if
			// we are not able to get 'Connect' button yet - either because PDS didnt responded or responded
			// with pairingEnabled=false.
			// (First check below should be mostly sufficient and subsequent checks are just
			// for safeguarding.)
			if (!mobilePairingInfo.IsPairingEnabled() &&
				!mobilePairingInfo.IsWaitingForPairing() && !mobilePairingInfo.IsWaitingForRequestCardChange() &&
				!mobilePairingInfo.IsMobileCardedSession())
			{
				LogString(POLLER_LOG, "Waiting for mobile.event.pairingSettings timed-out, resending request.pairingSettings.");
				CInternalMessage *internalMsg =
					new CInternalMessage(INT_MSG_CALL_MC_PAIRING_SETTINGS_REQUEST, (WORD) true);
				SendToInternals(internalMsg, QUEUE_PRIORITY_NORMAL);

				// By the time internals gathers the meters and routes them to Poller again we might trigger this again
				m_lastPairingSettingsSendTicks = m_operatingSystem.GetTickCount32();
			}
		}

		// Global flag must be set, then we issue the message the immediately after connecting to poller
		// Otherwise if we have no response and 5 minutes or more passed, also issue the message again)
		if (!m_proximitySettingsResponseReceived
			&& !m_proximitySettingsRequestIssued
			&& m_extensibleFlags.EnableLocationServices()
			&& ((m_operatingSystem.GetTickCount32() - m_lastProximitySettingsRequestSendTicks) >
				RESEND_PROXIMITY_SETTINGS_INTERVAL_IN_SECS * MILLISECONDS_PER_SECOND))
		{
			LogString(POLLER_LOG, "Sending mobile.request.proximitySettings.");
			CInternalMessage *internalMsg =
				new CInternalMessage(INT_MSG_CALL_MCP_PROXIMITY_SETTINGS_REQUEST, (WORD) true);
			SendToInternals(internalMsg, QUEUE_PRIORITY_NORMAL);

			m_lastProximitySettingsRequestSendTicks = m_operatingSystem.GetTickCount32();
			m_proximitySettingsRequestIssued = true;
		}
	}
	else
	{
		m_proximitySettingsResponseReceived = false;
		m_proximitySettingsRequestIssued = false;
	}

	WatchdogCheckIn();
}

void CPollerProtocol::ProcessMCCommunicationSupportFromInternals(CInternalMessage* msg)
{
	if (msg != NULL)
	{
		m_mobileConnectConfigEnabled = (bool)msg->GetWordData();
	}
}

bool CPollerProtocol::IsResendItemListEmpty()
{
	return m_transactionResendList->IsEmpty();
}

byte * CPollerProtocol::GetFirstResendItem(WORD & transLength,
	DWORD &outUniqueId,
	DWORD &outLastSent,
	bool &alreadySuspicious )
{
	byte *retVal = NULL;
	if (m_transactionResendList != nullptr)
		retVal = m_transactionResendList->GetFirstItem(transLength, outUniqueId, outLastSent, alreadySuspicious);
	return retVal;
}

byte * CPollerProtocol::GetNextResendItem(WORD & transLength,
	DWORD &currentUniqueId,
	DWORD & currentLastSent,
	bool &alreadySuspicious )
{
	byte *retVal = NULL;
	if (m_transactionResendList != nullptr)
		retVal = m_transactionResendList->GetNextItem(transLength, currentUniqueId, currentLastSent, alreadySuspicious);
	return retVal;
}

bool CPollerProtocol::SetLastSent(DWORD UniqueId)
{
	if (m_transactionResendList)
	{
		return m_transactionResendList->SetLastSent(UniqueId);
	}
	return false;
}

void CPollerProtocol::LockResendList()
{
	m_transactionResendList->Lock();
}

void CPollerProtocol::UnlockResendList()
{
	m_transactionResendList->Unlock();
}

void CPollerProtocol::ProcessMessageFromInternals( CInternalMessage *msg )
{
	LogString(POLLER_LOG, "PollerProtocol: ProcessMessageFromInternals: %s", InternalMsgTypeStr[msg->GetMessageType()]);
	switch (msg->GetMessageType())
	{
	case INT_MSG_RECONNECT:
		ProcessReconnectFromInternals();
		break;
	case INT_MSG_SET_METERS:
		ProcessSetMetersFromInternals( msg );
		break;
	case INT_MSG_SET_STATUS:
	{
		string sentinelIp = msg->GetStringData();
		if (!sentinelIp.empty() && m_connection)
		{
			LogString(POLLER_LOG, "[INT_MSG_SET_STATUS] Setting sentinel IP address to %s.",
				sentinelIp.c_str());

			CIPAddress sentinelIpAddr;
			sentinelIpAddr.SetIP(sentinelIp);
			m_connection->SetMyIP(sentinelIpAddr);
			m_connection->SetActualIP(sentinelIpAddr);
		}
		SendStatusToPoller();
	}
	break;
	case INT_MSG_SET_SLOTMASTID:
		ProcessSetSlotMastIDFromInternals( msg );
		break;
	case INT_MSG_CHANGE_SLOTMASTID:
		ProcessChangeSlotMastIDFromInternals( msg );
		break;
	case INT_MSG_SET_CONNECTION_DATA:
		ProcessSetConnectionFromInternals( msg );
		break;
	case INT_MSG_SET_MOBILE_CONNECT_COMMUNICATION_SUPPORT:
		ProcessMCCommunicationSupportFromInternals(msg);
		break;
	case INT_MSG_MAC_ADDR:
		ProcessSetMacAddressFromInternals( msg );
		break;
	case INT_MSG_SET_VERSION:
		ProcessVersionFromInternals( msg );
		break;
	case INT_MSG_SET_IMAGE_CHECKSUM:
		m_imageChecksum = msg->GetDWordData();
		SendStatusToPoller();
		break;
	case INT_MSG_TRANSACTION_VIRTUAL_SESSION_STARTED:
	case INT_MSG_TRANSACTION_PLAYER_CARD_IN:
		SendCardInToPoller( msg );
		break;
	case INT_MSG_TRANSACTION_PLAYER_CARD_OUT:
		SendCardOutToPoller( msg );
		break;
	case INT_MSG_TRANSACTION_EMPLOYEE_CARD_IN:
		SendEmployeeCardToPoller( msg, true );
		break;
	case INT_MSG_TRANSACTION_EMPLOYEE_CARD_OUT:
		SendEmployeeCardToPoller( msg, false );
		break;
	case INT_MSG_TRANSACTION_SENTINEL_ON:
	case INT_MSG_TRANSACTION_ILLEGAL_DOOR_OPEN:
	case INT_MSG_TRANSACTION_CHANGE_LIGHT_ON:
	case INT_MSG_TRANSACTION_CHANGE_LIGHT_OFF:
	case INT_MSG_TRANSACTION_METER_RUNAWAY:
	case INT_MSG_DOOR_OPEN:
	case INT_MSG_DOOR_CLOSED:
	case INT_MSG_TRANSACTION_PD_CHASSIS_OPEN:
	case INT_MSG_SEND_STACKER_IN:
	case INT_MSG_SEND_STACKER_OUT:
	case INT_MSG_TRANSACTION_SLOT_POWER_ON:
	case INT_MSG_TRANSACTION_SLOT_POWER_OFF:
	case INT_MSG_SEND_WITNESS:
	case INT_MSG_BAD_CARD_READ:
	case INT_MSG_TRANSACTION_CONTROLLED_REBOOT_WARNING:
	case INT_MSG_TRANSACTION_CONTROLLED_REBOOT:
	case INT_MSG_TRANSACTION_AUTO_UPLOAD_CANCELED:
	case INT_MSG_TRANSACTION_PROXIMITY_REQUEST_BEACON_SCAN_LIST:
		SendBasicTransactionToPoller( msg );
		break;
	case INT_MSG_NVRAM_VALID:
		SetNvramStatus( msg );
		break;
	case INT_MSG_TRANSACTION_PLAYER_UPDATE:
		SendPlayerUpdateToPoller( msg );
		break;
	case INT_MSG_SEND_TILT:
		SendTiltToPoller( msg );
		break;
	case INT_MSG_TRANSACTION_BONUS_EVENTS:
		SendBonusEventsToPoller( msg );
		break;
	case INT_MSG_TRANSACTION_CASH_TRANSFER:
		SendCashTransferToPoller( msg );
		break;
	case INT_MSG_TRANSACTION_TICKET:
		SendTicketTransactionToPoller( msg );
		break;
	case INT_MSG_TICKET_RESPONSE_RECEIVED:
		RemoveTicketFromResendList( msg );
		break;
	case INT_MSG_TRANSACTION_PB_AUTHORIZE:
		ProcessPBAuthorize( msg );
		break;
	case INT_MSG_TRANSACTION_PB:
		LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
		ProcessPBTransfer( msg );
		break;
	case INT_MSG_TRANSACTION_SNAPSHOT:
		SendSnapshotToPoller( msg );
		break;
	case INT_MSG_DEFAULT_DATA_DIRECTORY_UPDATED:
		ProcessDefaultDataDirectoryUpdated( msg );
		break;
	case INT_MSG_TRANSACTION_GAME_INFO:
	case INT_MSG_TRANSACTION_MACHINE_ONLINE:
	case INT_MSG_TRANSACTION_MACHINE_OFFLINE:
	case INT_MSG_TRANSACTION_MACHINE_MOVE_BEGIN:
	case INT_MSG_TRANSACTION_MACHINE_MOVE_COMPLETE:
		SendMachineMoveToPoller( msg );
		break;
	case INT_MSG_TRANSACTION_ONELINK:
		SendOneLinkTransactionToPoller(msg);
		break;
	case INT_MSG_NOTIFY_RESEND_EMPTY:
		SendEmptyResendToInternals(msg);
		break;
	case INT_MSG_SET_SLOTMAST_REVISION:
		ProcessSlotMastRevisionFromInternals( msg );
		break;
	case INT_MSG_TRANSACTION_JACKPOT_FILL:
		SendJackpotFillToPoller( msg );
		break;
	case INT_MSG_TRANSACTION_FAILURE_CODE:
		SendFailureCodeToPoller( msg );
		break;
	case INT_MSG_TRANSACTION_JACKPOT:
		SendJackpotToPoller( msg );
		break;
	case INT_MSG_TRANSACTION_MAINTENANCE:
		SendMaintenanceToPoller( msg );
		break;
	case INT_MSG_TRANSACTION_SERIAL_COMM_DOWN:
	case INT_MSG_TRANSACTION_SERIAL_COMM_UP:
		SendCommStatusToPoller( msg );
		break;
	case INT_MSG_TRANSACTION_CONFIG_CHANGE:
		SendConfigChangeNoticeToPoller( msg );
		break;
	case INT_MSG_TRANSACTION_BILL_ACCEPTED:
		SendBillAcceptedToPoller( msg );
		break;
	case INT_MSG_TICKET_OUT_PENDING_COUNT:
		SendTicketOutCountToInternals();
		break;
	case INT_MSG_GET_LOSS_LIMIT_BALANCE_UPDATE:
		SendRequestLossLimitBalanceToPoller( msg );
		break;
	case INT_MSG_SET_IP:
		SetIP( msg );
		break;
	case INT_MSG_SEND_EXTERNAL_BONUS:
		SendExternalBonusToPoller(msg);
		break;
	case INT_MSG_UJP_CHECK_PIN:
		SendAuthorizePinToPoller(msg);
		break;
	case INT_MSG_UNATTENDED_JACKPOT:
		SendUJPResult(msg);
		break;
	case INT_MSG_UPGRADE_AVAILABLE:
		{
			// Forward to internals
			CInternalMessage *intMsg = new CInternalMessage(INT_MSG_UPGRADE_AVAILABLE);
			SendMessageToInternals(intMsg, QUEUE_PRIORITY_NORMAL);
		}
		break;

	case INT_MSG_FEATURE_FLAG_UPDATE:
		ProcessFeatureFlag(msg);
		break;
	case INT_MSG_TRANSACTION_GAME_SESSION_CHANGE:
		SendGameSessionChange(msg);
		break;
	case INT_MSG_SUBGAME_SELECTION:
		SendSubGameSelection(msg);
		break;
	case INT_MSG_SEND_ALL_GAMES_LIST:
		m_pollergameListInfo->ProcessAllGameListFromInternals(msg);
        msg->SetData((CGames*)nullptr);
		break;
	case INT_MSG_SEND_ENABLED_DENOMGAMES_LIST:
		m_pollergameListInfo->ProcessDenomGamesFromInternalsAndSendMessageToPoller(msg);
		break;
	case INT_MSG_SET_ENABLED_GAMES:
		m_pollergameListInfo->ProcessEnabledGamesFromInternals(msg);
		break;
	case INT_MSG_TRYSENDGAMELIST_GLOBAL_MGMD:
		m_pollergameListInfo->ProcessGlobalMGMDFlag(msg);
		break;
	case INT_MSG_TRYSENDGAMELIST_POLLER_ONLINE:
		m_pollergameListInfo->ProcessandPollerOnlineFlag(msg);
		break;
	case INT_MSG_SEND_STATUS_AFTER_PARSING_CONFIG:
	{
		string sentinelIp = msg->GetStringData();
		if (!sentinelIp.empty() && m_connection)
		{
			LogString(POLLER_LOG, "[INT_MSG_SEND_STATUS_AFTER_PARSING_CONFIG] Setting sentinel IP address to %s.",
				sentinelIp.c_str());

			CIPAddress sentinelIpAddr;
			sentinelIpAddr.SetIP(sentinelIp);
			m_connection->SetMyIP(sentinelIpAddr);
			m_connection->SetActualIP(sentinelIpAddr);
		}
		SendStatusAfterParsingConfigFromInternals();
	}
	break;
	case INT_MSG_PB_SET_NEW_PIN:
		SendPBResetPinTransactionMessageToPoller(msg);
		break;
	case INT_MSG_TRANSACTION_GENERIC:
		SendGenericTransactionToPoller(msg);
		break;
	case INT_MSG_PERFORM_LOAD_BALANCING:
		ProcessPerformLoadBalancingFromInternals();
		break;
	case INT_MSG_TRANSACTION_CARDLESS_SESSION_START:
	case INT_MSG_TRANSACTION_CARDLESS_SESSION_END:
		SendCardlessTransactionToPoller(msg);
		break;
	case INT_MSG_SET_NFD_POLLER:
		{
			m_defaultIsPollerNFD = (bool)msg->GetWordData();
			LogString(POLLER_LOG, "Received INT_MSG_SET_NFD_POLLER %d.", m_defaultIsPollerNFD);
		}
		break;
	case INT_MSG_TRANSACTION_MOBILE_CONNECT_PROXIMITY:
#ifdef _DEBUG
		LogString(POLLER_LOG, "SendMCPTransactionMessageToPoller() - %d", msg->GetWordData());
#endif
		SendMCPTransactionMessageToPoller(msg);
		break;
	case INT_MSG_EXTENSIBLE_SETTINGS:
		m_extensibleFlags.m_wordValue = msg->GetDWordData();
		break;
	case INT_MSG_REQUEST_CONFIG:
		SendRequestConfigToPoller();
		break;
	default: 
		break;
	}

	delete msg;
}

void CPollerProtocol::SendEmptyResendToInternals(CInternalMessage *msg)
{
	bool fromRemoteReboot = (bool)msg->GetWordData();
	m_notifyResendEmpty = true;

	if( m_transactionResendList->IsEmpty())
	{
		CInternalMessage *msg = new CInternalMessage(INT_MSG_NOTIFY_RESEND_EMPTY, (WORD)true);
		SendToInternals( msg, QUEUE_PRIORITY_NORMAL );
		m_notifyResendEmpty = false;
	}
	else
	{
		if (fromRemoteReboot)
		{
			CInternalMessage *msg = new CInternalMessage(INT_MSG_NOTIFY_RESEND_EMPTY, (WORD)false);
			SendToInternals( msg, QUEUE_PRIORITY_NORMAL );
		}
	}
	// otherwise it will be sent after an ack.
}

void CPollerProtocol::SendTicketOutCountToInternals()
{
	CInternalMessage * msgCount = new CInternalMessage(INT_MSG_TICKET_OUT_PENDING_COUNT, (WORD)m_ticketsBufferedCount);
	SendMessageToInternals(msgCount, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::ProcessVersionFromInternals( CInternalMessage *msg )
{
	DWORD versionInfo = msg->GetDWordData();

	m_versionBuild = (versionInfo >> 24) & 0xff;
	m_versionNumberMajor = (versionInfo >> 16) & 0xff;
	m_versionNumberMinor = (versionInfo >> 8) & 0xff;
	m_versionRevision =	versionInfo & 0xff;

}

void CPollerProtocol::ProcessSlotMastRevisionFromInternals( CInternalMessage *msg)
{
	m_slotMastRevision = msg->GetWordData();
}

void CPollerProtocol::ProcessSetMetersFromInternals( CInternalMessage *msg )
{
	CGames *games = msg->GetGamesData();
	DWORD playSessionId = (DWORD)msg->GetDWordData();
	DWORD playTxSeqNum = (DWORD)msg->GetDWordData(true);

	if ( games != NULL )
	{
		// Use this trick to allow us to reuse the games.
		msg->SetData( (CGames *)NULL );


		if( games->GetGameCount() > 0 )
		{		
			CGame *game = games->GetGameByIndex( 0 );

			if( game != NULL && game->GetMeters() != NULL )
			{
				SendMetersToPoller(games, game->IsEGM(), playSessionId, playTxSeqNum);
			}	
			else
			{
				delete games;
			}
		}
		else
		{
			delete games;
		}
	}
}

void CPollerProtocol::ProcessSetSlotMastIDFromInternals( CInternalMessage *msg )
{
	//This should only happen when starting up.
	//TBD. This also happens on the successful return Message of a change request
	m_slotMastID = msg->GetDWordData();

	// If it exists already, gets updated
	m_mobileConnectMessageFactory.UpdateDeviceId(m_slotMastID);
}

void CPollerProtocol::ProcessChangeSlotMastIDFromInternals( CInternalMessage *msg )
{

	LogString(POLLER_LOG, "PollerProtocol: Sending change slotmastID.");

	DWORD uniqueId;
	size_t size;

	m_pendingSlotmastChange =  (long)msg->GetDWordData();

	// SlotmastId change has been made a transaction
	CMeters *msgMeters = msg->GetTransactionMetersData();
	msg->SetTransactionMetersData((CMeters *)NULL);

	uniqueId = GetUniqueId();

	CPollerSlotmastTransactionMessage *transMsg = new CPollerSlotmastTransactionMessage( msgMeters, m_pendingSlotmastChange, uniqueId, msg->GetBytesData(size) );
	CPollerMessage *pollerMsg = transMsg;
	SetMessageHeader(pollerMsg);

	DWORD status = msg->GetDWordData(true);
	// adjust status, show transaction backlog
	// TBD: how to know backlog exists.
	transMsg->SetStatus(status & 0xFFF7FFFF); // clear bit, for now.

	DWORD bufferSize;
	byte *transBuffer;
	transBuffer = transMsg->GetReadWriteBuffer(bufferSize);	

	QueueToPoller(pollerMsg, CHANGE_SLOTMAST_ID_CODE, uniqueId, transBuffer,  (WORD) bufferSize);
}

void CPollerProtocol::ProcessSetMacAddressFromInternals( CInternalMessage *msg )
{
	size_t bytesSize;
	const BYTE * ptr = msg->GetBytesData(bytesSize);

	memcpy_s(m_macAddress, SIZE_OF_MAC_ADDRESS, ptr, bytesSize);

}


void CPollerProtocol::ProcessSetConnectionFromInternals( CInternalMessage *msg )
{
	if ( m_connection != NULL )
		delete m_connection;

	m_connection = msg->GetConnectionData();

	// We want to keep this connection message to ourself. 
	// Do the following so the connection data does not get deleted.
	msg->SetData( (CConnection *)NULL );

	//Close the conection and reconnect;
	pc->Close();

	//Finally this is a good place to tell internals the state of the resend list
	CheckResendUse();

	m_statusSendNeeded = true;
}

//Disconnect. The reconnect happens automatically.
void CPollerProtocol::ProcessReconnectFromInternals()
{
	pc->Close();
}

void CPollerProtocol::AddToPollerQueue(CPollerMessage*pollerMsg, int priority)
{
	m_pollerMsgQueue->Add( pollerMsg, priority );
}

void CPollerProtocol::CheckForConnect( void )
{

	DWORD currentTick = CUtilities::GetTickCount();

	//check for wrap around
	if (currentTick < m_lastMessageReceivedTime)
	{
		m_lastMessageReceivedTime = 0;
	}

	// load balancing
	if (m_performLoadBalancing)
	{
		if (!pc->Reading() && !pc->Sending())
		{
			LogString(POLLER_LOG, "[LB] Poller Load Balancing : Reconnecting to poller...");
			ConnectToPoller();
		}
	}

	if (!m_NewPollerIP.empty())  // means poller IP has been pushed via SpeedMedia
	{
		if (!pc->Reading() && !pc->Sending())
		{
			m_connection->SetHost(m_NewPollerIP.c_str());
			m_NewPollerIP.clear();
			m_IsPollerIPChanged = true;
			LogString(FLASHING, "New Poller IP consumed. Trying to connect to new poller...");
			ConnectToPoller();
		}
	}

	//if we're not connected, try to reconnect
	if ( false == pc->Connected() && !pc->Reading() && !pc->Sending())
	{
		ConnectToPoller();
	}

	// If the time we last sent a message to the poller exceeds our hearbeat time,
	//   send a heartbeat to the poller.
	if (true == pc->Connected())
	{
		SendHeartbeatIfNecessary();
	}
}

void CPollerProtocol::SendHeartbeatIfNecessary()
{
	DWORD currentTick = CUtilities::GetTickCount();
	if (currentTick >= m_lastHBMessageSentTime + HEARTBEAT_TIME || currentTick < m_lastHBMessageSentTime)
	{
		SendHeartbeatToPoller();
		m_lastHBMessageSentTime = CUtilities::GetTickCount();
	}
}

void CPollerProtocol::ConnectToPoller( void )
{
	m_lastHBMessageSentTime = 0;
	m_connectivityStatus.flags.isPollerOnline = false;
	bool isSendOnlineToInternals = !m_performLoadBalancing;
	bool isConnected = false;

	// reset pmt transaction status to false
	m_pmtTransactionsUp = false;

	if (isSendOnlineToInternals)
	{
		SendOnlineToInternals();
	}

	if ( m_connection != NULL && !(m_connection->GetHost().empty()) )
	{
		try
		{
			// When the ip address of the poller gets changed, the connection needs to be remade
			if( true == pc->Connected() )
			{
				// close existing connection
				pc->Close();
			}

			// connect to poller
			CWatchDog::Instance().Checkin();
			TiltCode tiltCode(tltUnknown);
			LogString(POLLER_LOG, "Calling PollerCommunicator::Connect hostName=\"%s\" port=%d slotMastID=%ld myIP=\"%s\" actualIP=\"%s\"",
				static_cast<LPCTSTR>(m_connection->GetHost().c_str()), (int)m_connection->GetPort(), (long)m_slotMastID,
				static_cast<LPCTSTR>(m_connection->GetMyIP().GetIP().c_str()), static_cast<LPCTSTR>(m_connection->GetActualIP().GetIP().c_str()));
			isConnected = pc->Connect(m_connection->GetHost(), m_connection->GetPort(), tiltCode);

			std::string strConnectStatus = (isConnected) ? "Poller connected." : "Poller not connected.";
			LogStringNoFormat(POLLER_LOG, strConnectStatus.c_str());

			if (isConnected)
			{
				if (m_IsPollerIPChanged)
				{
					LogStringNoFormat(FLASHING, strConnectStatus.c_str());
					m_IsPollerIPChanged = false;
				}
			}
			SendPacketTiltToInternals(nullptr, 0, tiltCode);
			CWatchDog::Instance().Checkin();
		}
		catch(...)
		{
			//TBD: not sure what we do for errors yet.
		}
	}

	if (m_performLoadBalancing && isConnected)
	{
		m_connectivityStatus.flags.isPollerOnline = true;  // required because config msg won't come from Poller this time.
		LogString(POLLER_LOG, "[LB] Load balancing done successfully. Ignoring config request from Poller.");
	}
	else if (isConnected)
	{
		m_configState = EnumConfigState::SendRequest;
	}

	m_performLoadBalancing = false;

	if (isSendOnlineToInternals || !isConnected)
	{
		SendOnlineToInternals();
	}

	// Send an update to internals in case the error info changed during the pc->Connect() call.
	SendPollerPacketSentVerifyErrorsToInternals();
	if (pc->Connected() && m_statusSendNeeded)
	{
		SendStatusToPoller();
		m_statusSendNeeded = false;
	}

    if (!pc->Connected()) 
    {
		// If not able to connect, make sure it does not get into a tight connection attempt loop.
        CUtilities::Sleep(CONNECT_RETRY_TIMEOUT);
    }
}

bool CPollerProtocol::SendToPoller(byte *pollerMsgBuffer, WORD transLength, DWORD uniqueId, bool alreadySuspicious)
{
	long bytesSent= 0;
	bool sendResult = false;
	long tempsendpacketretlength = 0;

	if( true == pc->Connected() )
	{
		byte transactionCode;
		memcpy(&transactionCode, pollerMsgBuffer + SIZE_OF_MESSAGE_HEADER, sizeof(byte));

		WORD messageID;
		DWORD sequence;
		memcpy(&messageID, pollerMsgBuffer + 2, sizeof(WORD));
		memcpy(&sequence, pollerMsgBuffer + 12, sizeof(DWORD));

		LogString(POLLER_LOG, "PollerProtocol: sending %s, transaction %s Sequence %d to Poller at %d.", 
			CPollerMessage::PollerMsgTypeToString( (PollerMsgType)messageID ).c_str(), CPollerMessage::TransactionTypeToString( (TransactionType)transactionCode ).c_str(), sequence, CUtilities::GetTickCount() );
#ifdef _DEBUG
		LogData(POLLERDATA_LOG, DATA_WRITE | DATA_ETHERNET, pollerMsgBuffer, transLength);
#endif
		//send the buffer from the poller message 
		TiltCode tiltCode(tltUnknown);
		bool createOk = pc->CreatePacketToSend(pollerMsgBuffer,
			tempsendpacket,
			transLength,
			&tempsendpacketretlength,
			alreadySuspicious,
			m_macAddress,
			tiltCode,
			messageID, 
			SIZE_OF_READ_BUFFER/*,
			IsNFDPoller()*/);
		SendPacketTiltToInternals(pollerMsgBuffer, transLength, tiltCode);

		if (tempsendpacketretlength > SIZE_OF_READ_BUFFER)
		{
			string reason;
			reason = FormatString("CPollerProtocol::SendToPoller(byte *) packet buffer overrun tempsendpacketretlength=%ld > SIZE_OF_READ_BUFFER=%ld",
				(long)tempsendpacketretlength,
				(long)SIZE_OF_READ_BUFFER);
			CDiagnostics::ReportBadPollerProtocolSendPacket(tiltCode, reason.c_str(), pollerMsgBuffer, transLength);
			SendPacketTiltToInternals(pollerMsgBuffer, transLength, tltPollerProtocolCreatePacket);
		}

		if (createOk)
		{
			sendResult = pc->Send((char *)tempsendpacket, tempsendpacketretlength, bytesSent);
			if (!pc->VerifyPacketSent(tempsendpacket, tempsendpacketretlength, pollerMsgBuffer, transLength))
			{
				SendPollerPacketSentVerifyErrorsToInternals();
				CDiagnostics::ReportBadPollerProtocolSendPacket(tiltCode, _T("Packet sent verification"), nullptr, 0);
				SendPacketTiltToInternals(pollerMsgBuffer, transLength, tltPollerProtocolPacketSentVerification);
			}
		}

		// check if it is a bad Tx packet.
		if (m_BadTxResendCountMap.find(uniqueId) != m_BadTxResendCountMap.end())
		{
			// increment the resend count.
			m_BadTxResendCountMap[uniqueId]++;
			LogString(POLLER_LOG, "[uniqueId = %d] Tried sending bad packet to poller... %s : createOk = %d, tiltCode = %d, alreadySuspicious = %d, resendCount = %d",
				uniqueId, sendResult ? "SENT" : "NOT SENT", createOk, tiltCode, alreadySuspicious, m_BadTxResendCountMap[uniqueId]);
		}
		else if (!createOk)
		{
			// increment the resend count.
			LogString(POLLER_LOG, "[bad packet] uniqueId = %d, coming directly from resend list [NVRAM]", uniqueId);
			m_BadTxResendCountMap[uniqueId] = 0;
			LogString(POLLER_LOG, "[uniqueId = %d] Failed to send bad packet to poller : createOk = %d, tiltCode = %d, alreadySuspicious = %d",
				uniqueId, createOk, tiltCode, alreadySuspicious);
		}
	}
	return sendResult;
}

bool CPollerProtocol::IsConnected()
{
	return pc->Connected();
}

bool CPollerProtocol::SendToPoller(CPollerMessage *pollerMsg)
{
	long bufferSize = 0;
	long bytesSent= 0;
	bool sendResult = false;
	long tempsendpacketretlength = 0;
	DWORD newMessageSize = 0 ; //For special cases like 322 message.
	bool newMessageAllocated = true;

	if (true == pc->Connected())
	{
#ifdef ThrowInSendToPoller
        SentinelException* e = new SentinelException("Bummer"); // TEST TEST TEST
        throw e;
#endif
		//send the buffer from the poller message 
		BYTE * buffer = pollerMsg->GetReadWriteBuffer(bufferSize); 

		//this is just for logging
		WORD messageID;
		byte transactionCode;
		memcpy(&messageID, buffer + 2, sizeof(WORD));
		memcpy(&transactionCode, buffer + SIZE_OF_MESSAGE_HEADER, sizeof(byte));
		LogString(POLLER_LOG, "PollerProtocol::SendToPoller messageID=%u message=%s.", messageID, CPollerMessage::PollerMsgTypeToString((PollerMsgType)messageID).c_str());

		//Pollar message will sent to pollar if PMT is up or in case PMT is down, then only ticket transactions are sent to pollar if 
		//voucher separation featur enabled on pollar client
		bool isTranstoSendToPollarIfPMTDown(IsTranstoSendToPollarIfPMTDown(pollerMsg));
		if (messageID != POLLER_MSG_TRANSACTION || IsPmtTransactionsUp() || isTranstoSendToPollarIfPMTDown)
		{
			if (messageID == POLLER_MSG_TRANSACTION)
			{
				LogString(POLLER_LOG, "PollerProtocol::SendToPoller POLLER_MSG_TRANSACTION with %s Transaction Code.",
					CPollerMessage::TransactionTypeToString((TransactionType)transactionCode).c_str());
			}
			else if (messageID != POLLER_MSG_SENTINEL_HEARTBEAT)
			{
				LogString(POLLER_LOG, "PollerProtocol::SendToPoller %s.", CPollerMessage::PollerMsgTypeToString((PollerMsgType)messageID).c_str());
			}
			LogData(POLLERDATA_LOG, DATA_WRITE | DATA_ETHERNET, buffer, bufferSize);

			// if it's 322 message which could be more than the buffer alloted then increase the buffer size.
			// Error reporting will remain as it is to report that buffer was larger than as it supposed to be
			// DLE encoding can almost double the length of a message. So check for half the size
			newMessageSize = bufferSize * 2;
			newMessageAllocated = pc->ResizePacketToSendIfRequired(&tempsendpacket, bufferSize, messageID, newMessageSize);

			if (newMessageAllocated)
			{
				//Also pass message id to make it easier to debug while bad packet is reported
				TiltCode tiltCode(tltUnknown);
				bool createOk = pc->CreatePacketToSend(buffer,
					tempsendpacket,
					bufferSize,
					&tempsendpacketretlength,
					false,
					m_macAddress,
					tiltCode, 
					messageID,
					newMessageSize/*,
					IsNFDPoller()*/);
				
				SendPacketTiltToInternals(buffer, bufferSize, tiltCode);

				if (tempsendpacketretlength > SIZE_OF_READ_BUFFER  && POLLER_MSG_ALL_GAME_LIST != messageID)
				{
					LogString(ERROR_LOG, "CPollerProtocol::SendToPoller(CPollerMessage) packet buffer overrun tempsendpacketretlength=%ld > SIZE_OF_READ_BUFFER=%ld",
						(long)tempsendpacketretlength,
						(long)SIZE_OF_READ_BUFFER);
					SendPacketTiltToInternals(buffer, bufferSize, tltPollerProtocolCreatePacket);
				}

				if (createOk)
				{
					LogString(POLLER_LOG, "PollerProtocol::SendToPoller - Sending Packet finally to Poller");
					sendResult = pc->Send((char *)tempsendpacket, tempsendpacketretlength, bytesSent);

					if (!pc->VerifyPacketSent(tempsendpacket, tempsendpacketretlength, buffer, bufferSize))
					{
						SendPollerPacketSentVerifyErrorsToInternals();
						SendPacketTiltToInternals(buffer, bufferSize, tltPollerProtocolPacketSentVerification);
					}

				}
				else
				{
					// Tilt code already sent to internals via SendPacketTiltToInternals() in this case.

					LogString(POLLER_LOG, "[new bad packet] createOk = %d, tiltCode = %d", createOk, tiltCode);

					// getting the uniqueID of packet
					if (bufferSize > UNIQUE_ID_OFFSET)
					{
						DWORD uniqueId(*(DWORD *)(buffer + UNIQUE_ID_OFFSET));
						LogString(POLLER_LOG, "[new bad packet] uniqueId = %d", uniqueId);
						m_BadTxResendCountMap[uniqueId] = 0;  // adding entry to Bad Tx Count Map
					}
				}
			}
		}
		else
		{
			LogString(POLLER_LOG, "PollerProtocol::SendToPoller - IsPmtTransactionsUp()=%d isTranstoSendToPollarIfPMTDown=%d", 
				(int)IsPmtTransactionsUp(), (int)isTranstoSendToPollarIfPMTDown);
		}

		//Reset the buffer to older size. No need to keep bigger ones for smaller sizes
		pc->ResizePacketToSendIfRequired(&tempsendpacket, bufferSize, messageID, SIZE_OF_READ_BUFFER);
	}
	
	delete pollerMsg;
	return sendResult;
}

void CPollerProtocol::SendHeartbeatToPoller(void)
{
	LogString(POLLER_LOG, "PollerProtocol: Sending Heartbeat.");
	CPollerMessage *pollerMsg = new CPollerHeartbeatMessage();
	SetMessageHeader(pollerMsg);
	
	AddToPollerQueue( pollerMsg, QUEUE_PRIORITY_HIGHEST );
}

void CPollerProtocol::SendMCPTransactionMessageToPoller( CInternalMessage *msg )
{
	// This function sends Mobile Connect and Proximity transaction messages to Poller
	DWORD uniqueId;
	size_t size = 0;
	TransactionType transCode;
	string jsonMsg;

	CMeters *msgMeters = msg->GetTransactionMetersData();
	uniqueId = GetUniqueId();

	// Get Mobile Connect message subtype
	WORD mcMsgType = msg->GetWordData(false);
	string mcMsgTypeStr = ToPlayerDeviceService::emap.At((ToPlayerDeviceService::Type)mcMsgType, nullptr);
	LogString(POLLER_LOG, "%s:%d, MOBILE_CONNECT message type '%s'", __FUNCTION__, __LINE__, mcMsgTypeStr.c_str());

	switch (mcMsgType)
	{
	case ToPlayerDeviceService::RequestPairingSettings:
	{
		CPairingInformation& mobilePairingInfo = const_cast<CPairingInformation&>(GetMobilePairingInformation());

		if (!mobilePairingInfo.IsWaitingForPairing() && !mobilePairingInfo.IsWaitingForRequestCardChange() &&
			!mobilePairingInfo.IsMobileCardedSession())
		{
			m_lastPairingSettingsSendTicks = m_operatingSystem.GetTickCount32();

			// get to the mobile pairing information
			// Reset PairingEnabled flag so in case we don't receive pairing settings 
			// event we can resend the pairing request again in 5 minutes
			mobilePairingInfo.SetPairingEnabled(false);

			// Creating a new pairing settings request
			jsonMsg = m_mobileConnectMessageFactory.CreatePairingSettingsRequest();
#ifdef _DEBUG
			LogString(POLLER_LOG, "MessageFactory created this new message: '%s'.", jsonMsg.substr(0, 900).c_str());
#endif
			transCode = MOBILE_CONNECT_PAIRING_SETTINGS_REQUEST;
		}
		else
		{
			LogString(POLLER_LOG, "Discard sending the for mobile.request.pairingSettings since we're in the middle of a mobile session.");
			return;
		}

		break;
	}
	case ToPlayerDeviceService::RequestPairing:
	{
		LogString(POLLER_LOG, "Sending PairingRequest to Poller.");
		CPairingRequest *pairingRequestData = msg->GetPairingRequestData();
		if (pairingRequestData != nullptr)
		{
			// Create a new pairing settings request
			jsonMsg = m_mobileConnectMessageFactory.CreatePairingRequest(pairingRequestData);
#ifdef _DEBUG
			LogString(POLLER_LOG, "MessageFactory created this new message: '%s'", jsonMsg.substr(0, 900).c_str());
#endif
			transCode = MOBILE_CONNECT_PAIRING_REQUEST;
		}
		else
		{
			LogString(ERROR_LOG, "Cannot send PairingSettingsRequest to Poller, pairingRequestData uninitialized!");
			return;
		}
		break;
	}
	case ToPlayerDeviceService::EventPairingStatus:
	{
		CPairingStatusEvent *pairingStatusEventData = msg->GetPairingStatusEventData();
		if (pairingStatusEventData != nullptr)
		{
			// Create a new pairing status event
			jsonMsg = m_mobileConnectMessageFactory.CreatePairingStatusEvent(pairingStatusEventData);
#ifdef _DEBUG
			LogString(POLLER_LOG, "MessageFactory created this new message: '%s'", jsonMsg.substr(0, 900).c_str());
#endif
			transCode = MOBILE_CONNECT_PAIRING_STATUS;
		}
		else
		{
			LogString(ERROR_LOG, "Cannot send PairingStatusEvent to Poller, pairingStatusEventData uninitialized!");
			return;
		}
		break;
	}
	case ToPlayerDeviceService::EventPlayerChange:
	{
		CPlayerChangeEvent *pPlayerChangeEventData = msg->GetPlayerChangeEventData();
		if (pPlayerChangeEventData != nullptr)
		{
			// Create a new player change event
			jsonMsg = m_mobileConnectMessageFactory.CreatePlayerChangeEvent(pPlayerChangeEventData);
#ifdef _DEBUG
			LogString(POLLER_LOG, "MessageFactory created this new message: '%s'", jsonMsg.substr(0, 900).c_str());
#endif
			transCode = MOBILE_CONNECT_PLAYER_CHANGE;
		}
		else
		{
			LogString(ERROR_LOG, "Cannot send PlayerChangeEvent to Poller, pPlayerChangeEventData uninitialized.");
		}
		break;
	}
	case ToPlayerDeviceService::EventCardChange:
	{
		CCardChangeEvent *pCardChangeEventData = msg->GetCardChangeEventData();
		if (pCardChangeEventData != nullptr)
		{
			// Create a new card change event
			jsonMsg = m_mobileConnectMessageFactory.CreateCardChangeEvent(pCardChangeEventData);
#ifdef _DEBUG
			LogString(POLLER_LOG, "MessageFactory created this new message: '%s'", jsonMsg.substr(0, 900).c_str());
#endif
			transCode = MOBILE_CONNECT_CARD_CHANGE;
		}
		else
		{
			LogString(ERROR_LOG, "Cannot send CardChangeEvent to Poller, pCardChangeEventData uninitialized.");
		}
		break;
	}
	case ToPlayerDeviceService::EventPlayerSessionChange:
	{
		CPlayerSessionChangeEvent *pPlayerSessionChangeEvent = msg->GetPlayerSessionChangeEventData();
		if (pPlayerSessionChangeEvent != nullptr)
		{
			// Create a new player session change event
			jsonMsg = m_mobileConnectMessageFactory.CreatePlayerSessionChangeEvent(pPlayerSessionChangeEvent);
#ifdef _DEBUG
			LogString(POLLER_LOG, "MessageFactory created this new message: '%s'", jsonMsg.substr(0, 900).c_str());
#endif
			transCode = MOBILE_CONNECT_PLAYER_SESSION_CHANGE;
		}
		else
		{
			LogString(ERROR_LOG, "Cannot send PlayerSessionChangeEvent to Poller, pPlayerSessionChangeEvent uninitialized.");
		}
		break;
	}
	case ToPlayerDeviceService::EventFundsTransferBalance:
	{
		CFundsTransferBalanceEvent *pFundsTransferBalanceEvent = msg->GetFundsTransferBalanceEventData();
		if (pFundsTransferBalanceEvent != nullptr)
		{
			// Create a new get funds balance event
			jsonMsg = m_mobileConnectMessageFactory.CreateFundsTransferBalanceEvent(pFundsTransferBalanceEvent);
#ifdef _DEBUG
			LogString(POLLER_LOG, "MessageFactory created this new message: '%s'", jsonMsg.substr(0, 900).c_str());
#endif
			transCode = MOBILE_CONNECT_PBT_BALANCE;
		}
		else
		{
			LogString(ERROR_LOG, "Cannot send EventFundsTransferBalance to Poller, pFundsTransferBalanceEvent uninitialized.");
		}
		break;
	}
	case ToPlayerDeviceService::EventFundsTransfer:
	{
		CFundsTransferEvent *pFundsTransferEvent = msg->GetFundsTransferEventData();
		if (pFundsTransferEvent != nullptr)
		{
			// Create a new funds transfer event
			jsonMsg = m_mobileConnectMessageFactory.CreateFundsTransferEvent(pFundsTransferEvent);
#ifdef _DEBUG
			LogString(POLLER_LOG, "MessageFactory created this new message: '%s'", jsonMsg.substr(0, 900).c_str());
#endif
			transCode = MOBILE_CONNECT_PBT_FUNDS_TRANSFER;
		}
		else
		{
			LogString(ERROR_LOG, "Cannot send EventFundsTransfer to Poller, pFundsTransferEvent uninitialized.");
		}
		break;
	}
	case ToPlayerDeviceService::EventEndPBTSession:
	{
		// Create a new end PBT session event
		jsonMsg = m_mobileConnectMessageFactory.CreateEndPBTSessionEvent();
#ifdef _DEBUG
		LogString(POLLER_LOG, "MessageFactory created this new message: '%s'", jsonMsg.substr(0, 900).c_str());
#endif
		transCode = MOBILE_CONNECT_END_PBT_SESSION;
		break;
	}
	case ToPlayerDeviceService::RequestProximitySettings:
	{
		// Creating a new proximity settings request
		jsonMsg = m_mobileConnectMessageFactory.CreateProximitySettingsRequest();
#ifdef _DEBUG
		LogString(POLLER_LOG, "MessageFactory created this new message: '%s'.", jsonMsg.substr(0, 900).c_str());
#endif
		m_proximitySettingsRequestIssued = true;
		transCode = PROXIMITY_SETTINGS_REQUEST;
		break;
	}
	case ToPlayerDeviceService::EventProximityStatus:
	{
		CProximityStatusEvent *proximityStatusEventData = msg->GetProximityStatusEventData();
		if (proximityStatusEventData != nullptr)
		{
			// Create a new proximity status event
			jsonMsg = m_mobileConnectMessageFactory.CreateProximityStatusEvent(proximityStatusEventData);
#ifdef _DEBUG
			LogString(POLLER_LOG, "MessageFactory created this new message: '%s'", jsonMsg.substr(0, 900).c_str());
#endif
			transCode = PROXIMITY_STATUS;
		}
		else
		{
			LogString(ERROR_LOG, "Cannot send ProximityStatusEvent to Poller, proximityStatusEventData uninitialized!");
			return;
		}
		break;
	}
	default:
		LogString(POLLER_LOG, "SendMCPTransactionMessageToPoller - message type '%s' not implemented!", mcMsgTypeStr.c_str());
		return;
		break;
	}

	LogString(POLLER_LOG, "%s %d - sending MCP transaction type: %s", __FUNCTION__,
		__LINE__, CPollerMessage::TransactionTypeToString((TransactionType) transCode).c_str());

	CPollerMobileConnectTransactionMessage *pollerMsg = new CPollerMobileConnectTransactionMessage(msgMeters,
		transCode,
		uniqueId,
		msg->GetBytesData(size),
		jsonMsg);

	SetMessageHeader(pollerMsg);

	pollerMsg->SetReadWriteBuffer();

	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transCode, uniqueId, transBuffer, (WORD)transLength);
}

void CPollerProtocol::SendRequestConfigToPoller(void)
{
	m_configRequestTime = CUtilities::GetTickCount();

	if (m_slotMastID > 0)
	{
		// Poller config main request
		LogString(POLLER_LOG, "PollerProtocol: Sending config request.");
		CPollerMessage *pollerMsg = new CPollerConfigRequestMessage();
		SetMessageHeader(pollerMsg);
		
		AddToPollerQueue( pollerMsg, QUEUE_PRIORITY_NORMAL );

		m_configState = EnumConfigState::RequestSent;
	}
	else
	{
		m_configState = EnumConfigState::SendRequest;
	}
}

void CPollerProtocol::SendRequestLossLimitBalanceToPoller( CInternalMessage * msg )
{
	LogString(POLLER_LOG, "PollerProtocol: Sending Loss Limit Balance Request.");
	size_t size;
	const BYTE *pCardId = msg->GetBytesData(size);
	CPollerMessage *pollerMsg = new CPollerRequestLossLimitBalanceMessage(pCardId, size);
	SetMessageHeader(pollerMsg);
	AddToPollerQueue( pollerMsg, QUEUE_PRIORITY_NORMAL );
}

void CPollerProtocol::SetIP( CInternalMessage *msg )
{
	if(msg != NULL && m_connection != NULL)
	{
		CConnection *connection = msg->GetConnectionData();

		if(connection != NULL)
		{
			m_connection->SetMyIP( connection->GetMyIP() );
			m_connection->SetActualIP( connection->GetActualIP() );
		}
	}
}

void CPollerProtocol::SendStatusToPoller()
{
	if (m_connection == nullptr || 0 == m_pollerPacketVersion)
	{
		return;
	}

	byte tempIP[4];
	int a, b, c;
	string ip;

	//check our current IP if it is wrong ask internals to send it so we can refresh our memory
	//that way the next time we send this message the ip will be correct
	//Tracker #29541
	ip = m_connection->GetActualIP().GetIP();

	if(ip == "0.0.0.0" || (ip.find("169.") == 0))
	{
		CInternalMessage *msg = new CInternalMessage( INT_MSG_GET_IP );
		SendToInternals( msg, QUEUE_PRIORITY_HIGH );
	}

	LogString(POLLER_LOG, "PollerProtocol: Sending Status. m_pollerPacketVersion=%u", m_pollerPacketVersion);

	//TR 26590 Added IP and OS Image version version to the poller status message so we need to send this to SCF.
	m_connection->GetActualIP().GetIP(tempIP, IP_ADDRESS_SIZE);

	Status status;
	status.VerMajor = m_versionNumberMajor;
	status.VerMinor = m_versionNumberMinor;
	status.Build = m_versionBuild;
	status.Revision = m_versionRevision;

	status.Checksum = m_imageChecksum;

	//TR 26590 Added IP and OS Image version version to the poller status message so we need to send this to SCF.
	status.IPOctet1 = tempIP[0];
	status.IPOctet2 = tempIP[1];
	status.IPOctet3 = tempIP[2];
	status.IPOctet4 = tempIP[3];

	sscanf(CPlatform::Instance().GetOSImageVersionNumber().c_str(), "%d.%d.%d", &a, &b, &c);
	status.OSVerMajor = a;
	status.OSVerMinor = b;
	status.OSBuild = c;	
	LogString(POLLER_LOG, "Sending status.OSVerMajor %d", status.OSVerMajor);
	LogString(POLLER_LOG, "Sending status.OSVerMinor %d", status.OSVerMinor);
	LogString(POLLER_LOG, "Sending status.OSBuild %d", status.OSBuild);
	LogString(POLLER_LOG, "Sending status.IP %d:%d:%d:%d", status.IPOctet1, status.IPOctet2, status.IPOctet3, status.IPOctet4 );

	status.FeatureFlag =  FF_ATOMIC_REDEMPTIONS | FF_JACKPOT_EXTERNAL_BONUS | FF_EXTENDED_TICKET_EXPIRATION | FF_VOUCHER_SEPARATION | FF_MULTI_GAME_MULTI_DENOM | FF_EXTERNAL_CREDIT_TO_GAME | FF_CHUNKED_GAMES_LIST | FF_WALLET_2_0_SUPPORTED;
	status.MGMDAvailable = CPollerMGMDData::Instance().GetMGMDAvailable();
	status.GameStart = CPollerMGMDData::Instance().GetMGMDGameStart();
	status.GameEnd = CPollerMGMDData::Instance().GetMGMDGameEnd();
	status.PollingSpeed = CPollerMGMDData::Instance().GetMGMDPollingSpeed();
	status.MGMDGlobalOption = CPollerMGMDData::Instance().GetMGMDGlobalOption();
	status.MGMDSentinelOption = CPollerMGMDData::Instance().GetMGMDSenOption();
	status.resolution = CPlatform::Instance().GetResolution();
	status.landscape = CPlatform::Instance().GetLandscape();
	status.touchScreen = CPlatform::Instance().GetTouchScreen();
	status.processorType = CPlatform::Instance().GetProcessorType();
	status.numberOfCores = CPlatform::Instance().GetNumberOfCores();
	status.biosVersion = CPlatform::Instance().GetBiosVersion();
	status.totalMemoryInKB = CPlatform::Instance().GetTotalMemoryInKB();
	status.beaconCapability = CPlatform::Instance().GetBeaconCapability();
	status.beaconFirmware = CPlatform::Instance().GetBeaconFirmware();
	status.carrierFirmwareVersion = CPlatform::Instance().GetCarrierFirmwareVersion();
	status.videoMixerFirmwareVersion = CPlatform::Instance().GetVideoMixerFirmwareVersion();

	LogString(POLLER_LOG, "Sending status.MGMDFeatureFlag %x", status.FeatureFlag);
	LogString(POLLER_LOG, "Sending status.MGMDAvailable %d", status.MGMDAvailable);
	LogString(POLLER_LOG, "Sending status.GameStart %d", status.GameStart);
	LogString(POLLER_LOG, "Sending status.GameEnd %d", status.GameEnd);
	LogString(POLLER_LOG, "Sending status.PollingSpeed %d", status.PollingSpeed);
	LogString(POLLER_LOG, "Sending status.MGMDGlobalOption %d", status.MGMDGlobalOption);
	LogString(POLLER_LOG, "Sending status.MGMDSentinelOption %d", status.MGMDSentinelOption);
	LogString(POLLER_LOG, "Sending status.resolution %s", status.resolution.c_str());
	LogString(POLLER_LOG, "Sending status.landscape %d", status.landscape);
	LogString(POLLER_LOG, "Sending status.touchScreen %d", status.touchScreen);
	LogString(POLLER_LOG, "Sending status.processorType %d", status.processorType.c_str());
	LogString(POLLER_LOG, "Sending status.numberOfCores %d", status.numberOfCores.c_str());
	LogString(POLLER_LOG, "Sending status.biosVersion %d", status.biosVersion.c_str());
	LogString(POLLER_LOG, "Sending status.totalMemoryInKB %d", status.totalMemoryInKB.c_str());
	LogString(POLLER_LOG, "Sending status.beaconCapability %d", status.beaconCapability.c_str());
	LogString(POLLER_LOG, "Sending status.beaconFirmware %d", status.beaconFirmware.c_str());
	LogString(POLLER_LOG, "Sending status.carrierFirmwareVersion %d", status.carrierFirmwareVersion.c_str());
	LogString(POLLER_LOG, "Sending status.videoMixerFirmwareVersion %d", status.videoMixerFirmwareVersion.c_str());
	
	CPollerMessage *pollerMsg = new CPollerStatusMessage(status, m_pollerPacketVersion);
	SetMessageHeader(pollerMsg);

	AddToPollerQueue( pollerMsg, QUEUE_PRIORITY_NORMAL );
}

void CPollerProtocol::SendMetersToPoller(CGames *games, bool isEGM, DWORD playSessionId, DWORD playTxSeqNum)
{
	CPollerMessage *pollerMsg;

	if (isEGM)
	{
		pollerMsg = new CPollerMetersMessage(games, 
			IsNFDPoller(),
			playSessionId, playTxSeqNum);
		LogString(POLLER_LOG, "Sending EGM Meters.");
	}
	else
	{
		pollerMsg = new CPollerSubGameMetersMessage(games, 
			IsNFDPoller());
		LogString(POLLER_LOG, "Sending Subgame Meters For Game: %d", games->GetGameByIndex(0)->GetGameNumber());
	}

	SetMessageHeader(pollerMsg);
	AddToPollerQueue( pollerMsg, QUEUE_PRIORITY_NORMAL );
}

// CPollerProtocol::ReadAndProcessFromPoller()
// Try to read and process one or more packets from the Poller, while maintaining a connection to the Poller.
void CPollerProtocol::ReadAndProcessFromPoller( void )
{
	CPollerMessage *pollerMsg = nullptr;
	CPollerTransactionAckMessage *ackMsg = nullptr;

	if( true == pc->Connected() && !m_performLoadBalancing)
	{
		PollerMessageHeader hdr;

		long bytesRead = 0;
		long outputlength = 0;
		int BytesToExtract = SIZE_OF_READ_BUFFER;
		long bytesToRead = 0;
		bytesRead = 0;
		byte * dataPtr;

		int copySize = 0;

		CWatchDog::Instance().Checkin();

		//10/25/2007 Spencer fairbairn.
		//Increased the read timeout to give the poller more time to respond.
		SocketReadStatus readStatus = pc->Read(m_readBuffer, SIZE_OF_READ_BUFFER, bytesRead, m_pollerSocketReadTimeout);
		CWatchDog::Instance().Checkin();

		if ( (readStatus == SOCKET_READ_SUCCESS) && (0 < bytesRead) )
		{
			WORD oldPollerPacketVersion(m_pollerPacketVersion);

			while (true == pc->AddPacket((BYTE*)m_readBuffer, bytesRead, temprecvpacket, BytesToExtract, &outputlength))
			{
				LogData(POLLERDATA_LOG, DATA_READ | DATA_ETHERNET, temprecvpacket, outputlength);
				//the second time around, AddPacket needs byteRead to be Zero.
				pollerMsg = NULL;
				bytesRead = 0;  
				GetHeader(&hdr, temprecvpacket);

				// Update poller packet version only from a recognized 
				// packet. Guards against corrupted packets.
				if ((hdr.PacketVersion <= PACKET_VERSION_MAXIMUM) &&
					(hdr.MessageID <= POLLER_MSG_MAXIMUM))
				{
					m_pollerPacketVersion = hdr.PacketVersion;
				}

				dataPtr = temprecvpacket + SIZE_OF_MESSAGE_HEADER;
				copySize = outputlength - SIZE_OF_MESSAGE_HEADER;
				bool hdrDataLengthValid = copySize == hdr.DataLength;
				BYTE * ptr = NULL;
				LogString(POLLER_LOG, "Received %s.", CPollerMessage::PollerMsgTypeToString( (PollerMsgType)hdr.MessageID ).c_str() );

				switch ( hdr.MessageID )
				{
				case POLLER_MSG_SENTINEL_HEARTBEAT_RESP:
					LogString(POLLER_LOG, "Received HeartBeat Response.");
					pollerMsg = new CPollerHeartbeatRespMessage(m_pollerPacketVersion);
					ptr = pollerMsg->GetReadWriteBuffer( bytesToRead );
					if ( copySize == bytesToRead )
					{
						memcpy_s(ptr, bytesToRead, dataPtr, copySize );
						ProcessHeartbeatRespFromPoller( (CPollerHeartbeatRespMessage *)pollerMsg );
					}
					else
					{
						LogString(ERROR_LOG, "HeartBeatResp Mismatched Buffer Size.");
					}
					break;
				case POLLER_MSG_SENTINEL_TIME:
					LogString(POLLER_LOG, "Received Time.");
					pollerMsg = new CPollerTimeMessage();
					ptr = pollerMsg->GetReadWriteBuffer( bytesToRead );
					if ( copySize == bytesToRead )
					{
						memcpy_s(ptr, bytesToRead, dataPtr, copySize );
						ProcessSentinelTimerFromPoller( (CPollerTimeMessage *)pollerMsg );
					}
					else
						LogString(ERROR_LOG, "SentinelTime Mismatched Buffer Size.");
					break;
				case POLLER_MSG_GLOBAL_PASSWORD:
					LogString(POLLER_LOG, "Received Global Password.");
					pollerMsg = new CPollerGlobalPasswordMessage();
					ptr = pollerMsg->GetReadWriteBuffer( bytesToRead );
					if ( copySize == bytesToRead )
					{
						memcpy_s(ptr, bytesToRead, dataPtr, copySize );
						ProcessGlobalPasswordFromPoller( (CPollerGlobalPasswordMessage *)pollerMsg );
					}
					else
						LogString(ERROR_LOG, "GlobalPass Mismatched Buffer Size.");
					break;
				case POLLER_MSG_DISPLAY_MESSAGE:
					LogString(POLLER_LOG, "Received Display Message.");
					pollerMsg = new CPollerDisplayMessage(hdr.DataLength);
					ptr = pollerMsg->GetReadWriteBuffer( bytesToRead );
					if ( copySize == bytesToRead )
					{
						memcpy_s(ptr, bytesToRead, dataPtr, copySize );
						ProcessDisplayMessageFromPoller( (CPollerDisplayMessage *)pollerMsg );
					}
					else
						LogString(ERROR_LOG, "DisplayMessage Mismatched Buffer Size.");
					break;
				case POLLER_MSG_SLOTMASTID_CHANGE_RESP:
					LogString(POLLER_LOG, "Received SlotMastId Change Response.");
					pollerMsg = new CPollerChangeSlotMastIDRespMessage();
					ptr = pollerMsg->GetReadWriteBuffer( bytesToRead );
					if ( copySize == bytesToRead )
					{
						memcpy_s(ptr, bytesToRead, dataPtr, copySize );
						ProcessChangeSlotMastRespFromPoller( (CPollerChangeSlotMastIDRespMessage *)pollerMsg );
					}
					else
						LogString(ERROR_LOG, "ChangeSlotMastID Mismatched Buffer Size.");
					break;
				case POLLER_MSG_SENTINEL_CONFIG:
					ProcessSentinelConfigMsgFromPoller(hdrDataLengthValid, hdr, dataPtr);
					break;
				case POLLER_MSG_TRANSACTION_ACK:
					ackMsg = new CPollerTransactionAckMessage(hdr.DataLength);
					ptr = ackMsg->GetReadWriteBuffer ( bytesToRead );
					if ( copySize == bytesToRead )
					{
						memcpy_s(ptr, bytesToRead, dataPtr, copySize);
						// remove transaction from resend list
						// if it isn't waiting for a response
						TransactionType transType = (TransactionType)ackMsg->GetTransType();

						LogString(POLLER_LOG, "%s Acknowledged.", CPollerMessage::TransactionTypeToString( transType ).c_str());
						//tickets get removed only after a ticket response					
						if( (transType != TICKET_CODE) && (transType != CHANGE_SLOTMAST_ID_CODE) )
						{
							if ( (transType == PERSONAL_BANKER_CODE) )
							{
								//internals needs to know when this is aknowledged
								//and needs to know the sequence number.
								CInternalMessage * msg = new CInternalMessage(INT_MSG_PB_ACKED);
								msg->SetData( m_transactionResendList->GetPBSequenceNum( ackMsg->GetUniqueId() ) );
								SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
							}
							RemoveResendTransaction(ackMsg->GetUniqueId(), true);
						}
					}
					delete ackMsg;
					break;
				case POLLER_MSG_PLAYER_RESPONSE:
					LogString(POLLER_LOG, "Received Player Response");
					pollerMsg = new CPollerPlayerResponseMessage( dataPtr, hdr.PacketVersion, hdr.DataLength );
					ProcessPlayerResponseFromPoller((CPollerPlayerResponseMessage *)pollerMsg);
					break;
				case POLLER_MSG_GLOBAL_BALANCE:
					LogString(POLLER_LOG, "Received Player Global Balance");
					pollerMsg = new CPollerPlayerOmniCardBalanceResponseMessage( dataPtr);
					ProcessPlayerGlobalBalanceResponseFromPoller((CPollerPlayerOmniCardBalanceResponseMessage *)pollerMsg);
					break;
				case POLLER_MSG_LOSS_LIMIT_BALANCE_UPDATE:
					LogString(POLLER_LOG, "Received Loss Limit Balance Update");
					pollerMsg = new CPollerLossLimitBalanceUpdate( dataPtr );
					ProcessLossLimitBalanceUpdateFromPoller((CPollerLossLimitBalanceUpdate *)pollerMsg);
					break;
				case POLLER_MSG_PLAYER_BONUS_EVENTS:
					LogString(POLLER_LOG, "Player Bonus Events received.");
					pollerMsg = new CPollerPlayerBonusEventsMessage( dataPtr);
					ProcessPlayerBonusEventsFromPoller((CPollerPlayerBonusEventsMessage *)pollerMsg);
					break;
				case POLLER_MSG_PLAYER_UPDATE_EVENTS:
					LogString(POLLER_LOG, "Player Update Events received.");
					pollerMsg = new CPollerPlayerUpdateEventsMessage( dataPtr );
					ProcessPlayerUpdateEventsFromPoller((CPollerPlayerUpdateEventsMessage *)pollerMsg);
					break;
				case POLLER_MSG_TICKET:
					LogString(POLLER_LOG, "Ticket Response received.");
					pollerMsg = new CPollerTicketResponseMessage( dataPtr );
					ProcessTicketResponseFromPoller((CPollerTicketResponseMessage *)pollerMsg);
					break;
				case POLLER_MSG_PBT_RESPONSE:
					LogString(POLLER_LOG, "PBT Response received.");
					LogString(PBT_LOG, "PBT Response received.");
					pollerMsg = new CPollerPersonalBankerResponseMessage(dataPtr, hdr.PacketVersion, hdr.DataLength);
					ProcessPBTResponseFromPoller((CPollerPersonalBankerResponseMessage *)pollerMsg);
					break;
				case POLLER_MSG_EMPLOYEE_RESPONSE:
					LogString(POLLER_LOG, "Employee Response received.");
					pollerMsg = new CPollerEmployeeResponseMessage( dataPtr );
					ProcessEmployeeResponseFromPoller((CPollerEmployeeResponseMessage *)pollerMsg);
					break;
				case POLLER_MSG_AUTHORIZATION_RESPONSE:
					pollerMsg = new CPollerAuthorizationMessage( dataPtr );
					ProcessAuthorizationFromPoller((CPollerAuthorizationMessage *)pollerMsg);
					break;
				case POLLER_MSG_PROCESS_UNATTENDED_JACKPOT:
					pollerMsg = new CPollerProcessUJPMessage(dataPtr);
					ProcessUJPFromPoller((CPollerProcessUJPMessage *)pollerMsg);
					break;
				case POLLER_MSG_AUTHORIZE_PIN_RESPONSE:
					pollerMsg = new CPollerAuthorizePINResponseMessage(dataPtr);
					ProcessAuthorizePINResponseFromPoller((CPollerAuthorizePINResponseMessage *)pollerMsg);
					break;
				case POLLER_MSG_SUBGAME_SELECTION_RESPONSE:
					pollerMsg = new CPollerSubgameSelectionResponseMessage(dataPtr, hdr.PacketVersion, hdr.DataLength);
					ProcessSubgameSelectionResponseFromPoller((CPollerSubgameSelectionResponseMessage *)pollerMsg);
					break;
				case POLLER_MSG_REBOOT_REQUEST:
					pollerMsg = new CPollerRebootRequestMessage(dataPtr);
					ProcessRebootRequestFromPoller((CPollerRebootRequestMessage *)pollerMsg);
					break;
				case POLLER_MSG_PIN_RESET_RESPONSE:
					pollerMsg = new CPollerPBPinResetResponseMessage(dataPtr);
					ProcessPBPinResetResponseFromPoller((CPollerPBPinResetResponseMessage *)pollerMsg);
					break;
				case POLLER_MSG_PROXIMITY_BEACON_SCAN_LIST_RESPONSE:
					LogString(PROXIMITY_LOG, "PollerProtocol:: Retrieved Proximity Beacon Scan List Response.");
					pollerMsg = new CPollerProximityBeaconScanListResponseMessage(dataPtr);
					ProcessProximityBeaconScanListResponseFromPoller((CPollerProximityBeaconScanListResponseMessage *)pollerMsg);
					break;
				case POLLER_MSG_MOBILE_CONNECT_RESPONSE:
					LogString(POLLER_LOG, "PollerProtocol:: Retrieved Mobile Connect Response.");
					pollerMsg = new CPollerMobileConnectResponseMessage(dataPtr);
					ProcessMCResponseFromPoller((CPollerMobileConnectResponseMessage *)pollerMsg);
					break;
				default:
					SendMessageToInternals(new CInternalMessage(INT_MSG_BAD_PACKET_RECEIVED_FROM_POLLER), QUEUE_PRIORITY_HIGHEST);
					LogString(ERROR_LOG, "PollerProtocol:: Unknown Message From Poller.");
					LogData(POLLERDATA_LOG, DATA_READ | DATA_ETHERNET, temprecvpacket, outputlength);
					break;
				}

				if ( pollerMsg != NULL )
					delete pollerMsg;
			}

			//if we have not yet received a config message, request a new one
			//but don't send the request too frequently.

			//so that we know if we're offline.
			m_receiveTimeoutCount = 0;
			m_receivedZeroCount = 0;
			m_lastMessageReceivedTime = CUtilities::GetTickCount();

			//watch for tick wrap around
			if (m_configRequestTime > m_lastMessageReceivedTime)
			{
				m_configRequestTime = 0;
			}

			DWORD configTimeout = m_lastMessageReceivedTime - m_configRequestTime;
			if ((true == pc->Connected()) &&
				(EnumConfigState::SendRequest == m_configState || EnumConfigState::RequestSent == m_configState) &&
				(configTimeout > (DWORD)CONFIG_REQUEST_TIMEOUT))
			{
				LogString(POLLER_LOG, "ReadAndProcessFromPoller-BeginOnlineProcessing");
				BeginOnlineProcessing();
			}

			if (m_pollerPacketVersion)
			{
				SetOneLinkProgressivesSenOptionSupport(oldPollerPacketVersion, m_pollerPacketVersion);
			}
		}
		else if (SOCKET_READ_TIMEOUT == readStatus)
		{
			if (m_receiveTimeoutCount++ == 3)
			{
				LogString(ERROR_LOG, "PollerCommunicator::Recv TimedOut 3x in a row, close the connection.\n");
				m_receiveTimeoutCount = 0;
				pc->Close();
			}
			else
			{
				SendHeartbeatIfNecessary();
			}
		}
		else if (SOCKET_READ_ZERO == readStatus)
		{
			if (m_receivedZeroCount++ == 3)
			{
				LogString(ERROR_LOG, "PollerCommunicator::Recv zero bytes 3x in a row, close the connection.\n");
				m_receivedZeroCount = 0;
				pc->Close();
			}
			else
			{
				SendHeartbeatIfNecessary();
			}
		}
	}
	else
	{
		//if we're not connected, then take a little nap so that we don't loop too tightly.
		CUtilities::Sleep(READ_LOOP_WAIT_MSECS);
	}

	CWatchDog::Instance().Checkin();
}

bool CPollerProtocol::ValidateBadTransactionResendCount(DWORD uniqueId)
{
	bool retVal = true;

	// check if it is a bad transaction, i.e. entry found in the map.
	if (m_BadTxResendCountMap.find(uniqueId) != m_BadTxResendCountMap.end())
	{
		// check for resend count limit
		if (m_BadTxResendCountMap[uniqueId] >= MAX_BAD_TX_RESEND_COUNT)
		{
			// remove transaction item entry.
			if (RemoveResendTransaction(uniqueId, true))
			{
				retVal = false;
			}
		}
	}

	return retVal;
}

bool CPollerProtocol::RemoveResendListItemIfNecessary(DWORD uniqueId)
{
	bool removed(false);

	auto uniqueIdIt = m_listOfUniqueIdsNotToResend.find(uniqueId);
	if (m_listOfUniqueIdsNotToResend.end() != uniqueIdIt)
	{
		removed = true;
		m_listOfUniqueIdsNotToResend.erase(uniqueIdIt);
		RemoveResendTransaction(uniqueId, true);
		SendToInternals(new CInternalMessage(INT_MSG_SEND_TILT, (WORD)tltTicketInRejectedDueToSystemTimeout), QUEUE_PRIORITY_NORMAL);
		LogString(TICKETING_LOG, "Remove uniqueId=%u from the resend list.", uniqueId);
	}

	return removed;
}

bool CPollerProtocol::RemoveResendTransaction(DWORD uniqueId, bool isStoredInNVRAM)
{
	bool removed = m_transactionResendList->RemoveTransaction(uniqueId, isStoredInNVRAM);

	if (removed)
	{
		// Remove the entry from Bad Tx Map if it exist.
		if (m_BadTxResendCountMap.find(uniqueId) != m_BadTxResendCountMap.end())
		{
			m_BadTxResendCountMap.erase(uniqueId);
			LogString(POLLER_LOG, "[uniqueId = %d] Bad packet removed from resend list.", uniqueId);
		}
	}

	if( m_notifyResendEmpty && m_transactionResendList->IsEmpty() )
	{
		CInternalMessage *msg = new CInternalMessage(INT_MSG_NOTIFY_RESEND_EMPTY, (WORD)true);
		SendToInternals( msg, QUEUE_PRIORITY_NORMAL );
		m_notifyResendEmpty = false;
	}

	CheckResendUse();

	return removed;
}

void CPollerProtocol::RemoveTicketFromResendList( CInternalMessage * msg )
{
	// the DWORD data is the unique id.
	bool removed = RemoveResendTransaction(msg->GetDWordData(), true);
	WORD isTicketIn = msg->GetWordData();

	if((isTicketIn == 0) && removed)
	{
		lock_guard<recursive_mutex> lock(m_criticalSection);
		{
			try
			{
				m_ticketsBufferedCount--;
				LogString(TICKETING_LOG, "RemoveTicketFromResendList m_ticketsBufferedCount=%d", m_ticketsBufferedCount);
				SendMessageToInternals(new CInternalMessage(INT_MSG_TICKET_OUT_PENDING_COUNT, (WORD)m_ticketsBufferedCount), QUEUE_PRIORITY_NORMAL);
			}
			catch( char * str )
			{
				LogString(ERROR_LOG, "Failed To Send New Tickets Pending Size - %s", str);
			}
		}
	}

	if( m_notifyResendEmpty && m_transactionResendList->IsEmpty() )
	{
		CInternalMessage *msgNotify = new CInternalMessage(INT_MSG_NOTIFY_RESEND_EMPTY, (WORD)true);
		SendToInternals( msgNotify, QUEUE_PRIORITY_NORMAL );
		m_notifyResendEmpty = false;
	}

	CheckResendUse();

}

void CPollerProtocol::ProcessPBTResponseFromPoller(CPollerPersonalBankerResponseMessage *pollerMsg)
{
	CPersonalBankerInfo* pbInfo = pollerMsg->GetPBInfo();

	LogString(POLLER_LOG, "%s %d: PBT Cash Balance received: %d. Error Code: %u", __FUNCTION__, __LINE__, pbInfo->GetCashBalance(), pbInfo->GetErrorCode());
	CInternalMessage *msg = new CInternalMessage(INT_MSG_PB_RESPONSE, pbInfo);
	SendToInternals( msg, QUEUE_PRIORITY_NORMAL );
}

void CPollerProtocol::ProcessEmployeeResponseFromPoller(CPollerEmployeeResponseMessage *pollerMsg)
{
	CInternalMessage *msg = new CInternalMessage(INT_MSG_EMPLOYEE_RESPONSE, pollerMsg->GetPIN());

	byte * cardID = new byte[CARD_ID_LEN];
	byte * temp = pollerMsg->GetCardID();
	memcpy(cardID, temp, CARD_ID_LEN);

	msg->SetData(cardID, CARD_ID_LEN);
	SendToInternals( msg, QUEUE_PRIORITY_NORMAL );
}


void CPollerProtocol::ProcessTicketResponseFromPoller(CPollerTicketResponseMessage *pollerMsg)
{
	//Send the ticket response to internals
	CTicket* ticket = pollerMsg->GetTicketResponse();
	bool isTicketOut, bFound = false;
	DWORD dwUniqueID;

	if (m_transactionResendList != NULL)
	{
		bFound = m_transactionResendList->GetTicketTransaction(ticket, isTicketOut, dwUniqueID	);
	}

	if (bFound)
	{
		//set the ticket in/out state
		ticket->SetType(ticket->Type(), !isTicketOut);
		CInternalMessage *msg = new CInternalMessage( INT_MSG_TICKET_RESPONSE, ticket);
		//tell internals the uniqueID
		msg->SetData(dwUniqueID);

		SendToInternals( msg, QUEUE_PRIORITY_NORMAL );
	}
	else 
	{
		//if this was not on our resend list, it can't be for us
		LogString(TICKETING_LOG, "ERROR: Unsolicited Ticket Received...Searching for null validation number in resend list to remove");

		//Fix for tracker 30706
		if(m_transactionResendList->GetNULLValidationNumberTicket(dwUniqueID, isTicketOut))
		{
			LogString(TICKETING_LOG, "ERROR: NULL validation number found in resend list removing resend item UniqueID=%u", dwUniqueID);

			CInternalMessage *msg = new CInternalMessage( INT_MSG_TICKET_RESPONSE_RECEIVED, dwUniqueID );
			msg->SetData((WORD)!isTicketOut);
			RemoveTicketFromResendList(msg);

			delete msg;
		}

		delete ticket;
	}
}


void CPollerProtocol::ProcessHeartbeatRespFromPoller(CPollerHeartbeatRespMessage *heartbeatRespMsg)
{
	if (m_pollerPacketVersion >= 3)
	{
		if (heartbeatRespMsg->IsPMTTransactionsUp())
			m_pmtTransactionsUp = true;
		else
		{
			//SPS Tracker 26592 added logging so we can monitor this in the field.
			if (heartbeatRespMsg->IsPMTUp())
				LogString(ERROR_LOG, "ERROR: Recieved stop transactions message from PMT." );

			m_pmtTransactionsUp = false;
		}
	}
	else
	{
		if (heartbeatRespMsg->IsPMTUp())
			m_pmtTransactionsUp = true;
		else
			m_pmtTransactionsUp = false;
	}

	SendToInternals(new CInternalMessage(INT_MSG_POLLERID, heartbeatRespMsg->GetPollerID()), QUEUE_PRIORITY_NORMAL);

	m_connectivityStatus.flags.isPMTUp = heartbeatRespMsg->IsPMTUp();
	m_connectivityStatus.flags.isMMTUp = heartbeatRespMsg->IsMMTUp();
	m_connectivityStatus.flags.isVoucherServiceUp = heartbeatRespMsg->IsVoucherServiceUp();
	m_connectivityStatus.LogStatus("ProcessHeartbeatRespFromPoller");
	SendToInternals(new CInternalMessage(INT_MSG_CONNECTIVITY_STATUS, m_connectivityStatus.uValue), QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::ProcessPlayerResponseFromPoller(CPollerPlayerResponseMessage *playerRespMsg)
{
	// create playerinfo object and send to internals.
	PlayerResponse playerResponse = playerRespMsg->GetPlayerResponse();
	CPlayerInfo *pi = new CPlayerInfo(playerResponse);

	CInternalMessage *piMsg = new CInternalMessage( INT_MSG_SET_PLAYER_INFO, pi);
	piMsg->SetData(playerResponse.lossLimitBalance, (WORD)playerResponse.lossLimitStatus);

	LogString(POLLER_LOG, "PollerProtocol: Player response: balance %d, status %d",playerResponse.lossLimitBalance,  playerResponse.lossLimitStatus);
	pi->LogRankInfo();

	SendToInternals( piMsg, QUEUE_PRIORITY_NORMAL );

}

//This is used to process the new player global balance response message in 12.x
void CPollerProtocol::ProcessPlayerGlobalBalanceResponseFromPoller( CPollerPlayerOmniCardBalanceResponseMessage *playerRespMsg )
{
	PlayerOmniCardBalanceResponse playerResponse = playerRespMsg->GetPlayerResponse();
	CPlayerGlobalBalance *pig    = new CPlayerGlobalBalance();
	CInternalMessage     *pigMsg = new CInternalMessage( INT_MSG_SET_GLOBAL_BALANCE, pig );

	pig->SetCardNumber( playerResponse.card_id, sizeof(playerResponse.card_id) );
	pig->PutTotalRedeemablePoints( playerResponse.totalRedeemablePoints );
	pig->PutTotalRedeemableComps( playerResponse.totalRedeemableComps );
	pig->PutTotalPoints( playerResponse.totalPoints );
	pig->PutTotalComps( playerResponse.totalComps );

	LogString(POLLER_LOG, "PollerProtocol: Player Global Balance comps: %lld, points: %lld", playerResponse.totalRedeemableComps,  playerResponse.totalRedeemablePoints );

	SendToInternals( pigMsg, QUEUE_PRIORITY_NORMAL );
}

void CPollerProtocol::ProcessLossLimitBalanceUpdateFromPoller(CPollerLossLimitBalanceUpdate *pollerMsg)
{
	CInternalMessage * llUpdateMessage = new CInternalMessage( INT_MSG_LOSS_LIMIT_BALANCE_UPDATE, 
		pollerMsg->GetWithdrawalAmount(), 
		pollerMsg->GetNewBalance());

	BYTE * cardId = new BYTE[CARD_ID_LEN];
	memcpy(cardId, pollerMsg->GetCardID(), CARD_ID_LEN);
	llUpdateMessage->SetData(cardId, CARD_ID_LEN);

	LogString(POLLER_LOG, "PollerProtocol: Loss Limit Balance Update: withdrawal: %d, balance: %d",pollerMsg->GetWithdrawalAmount(), pollerMsg->GetNewBalance());
	SendToInternals(llUpdateMessage, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::ProcessAuthorizationFromPoller(CPollerAuthorizationMessage *pollerMsg)
{
	if (pollerMsg->IsAuthorized())
	{
		CInternalMessage * msg = new CInternalMessage(INT_MSG_AUTHORIZATION_GRANTED);
		msg->SetData(pollerMsg->GetTransactionID());
		LogString(POLLER_LOG, "PollerProtocol: Authorization granted");
		SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
	}	
	else
	{
		CInternalMessage * msg = new CInternalMessage(INT_MSG_AUTHORIZATION_DENIED);
		msg->SetData(pollerMsg->GetTransactionID());
		LogString(POLLER_LOG, "PollerProtocol: Authorization denied");
		SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
	}

}

void CPollerProtocol::ProcessUJPFromPoller(CPollerProcessUJPMessage *pollerMsg)
{
	CJackpot *jackpot = new CJackpot(MEMORY_NONE);
	jackpot->SetJackpotID(pollerMsg->GetJackpotID());
	jackpot->SetJackpotAmount(pollerMsg->GetAmount());
	jackpot->SetCardID(pollerMsg->GetCardID(), CARD_ID_LEN);
	CInternalMessage *msg = new CInternalMessage(INT_MSG_UNATTENDED_JACKPOT, jackpot);
	msg->SetData((DWORD)pollerMsg->GetType());
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::ProcessAuthorizePINResponseFromPoller(CPollerAuthorizePINResponseMessage *pollerMsg)
{
	CJackpot *jackpot = new CJackpot(MEMORY_NONE);
	jackpot->SetCardID(pollerMsg->GetCardID(), CARD_ID_LEN);
	CInternalMessage *msg = new CInternalMessage(INT_MSG_AUTHORIZE_PIN, jackpot);
	msg->SetData((WORD)pollerMsg->GetAuthorizeResult());
	SendToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::BeginOnlineProcessing(void)
{
	m_bSendFullDataAfterConfig = true;
	SendRequestConfigToPoller();
}

void CPollerProtocol::SendOnlineToInternals( void )
{
	m_connectivityStatus.flags.isTcpConnected = pc->Connected();
	m_connectivityStatus.LogStatus("SendOnlineToInternals");
	SendToInternals(new CInternalMessage(INT_MSG_SYSTEM_ONLINE, m_connectivityStatus.uValue), QUEUE_PRIORITY_HIGH);
}

void CPollerProtocol::SendPollerPacketSentVerifyErrorsToInternals()
{
	CInternalMessage *msg = new CInternalMessage(INT_MSG_POLLER_PACKET_SENT_VERIFY_ERRORS, (DWORD)pc->GetPollerPacketSentVerifyErrors());
	SendToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::ProcessChangeSlotMastRespFromPoller( CPollerChangeSlotMastIDRespMessage *changeSlotMastRespMsg )
{
	m_pendingSlotmastChange = INVALID_SLOT_MAST_ID;

	long tempSlotmast = changeSlotMastRespMsg->GetSlotMastID();
	m_transactionResendList->RemoveChangeSlotMastIDTransaction(tempSlotmast);

	if ( changeSlotMastRespMsg->SlotMastChangeResponse() == CHANGE_SUCCESS )
	{
		m_slotMastID = tempSlotmast;
		m_mobileConnectMessageFactory.UpdateDeviceId(m_slotMastID);
		LogString(POLLER_LOG, "ProcessChangeSlotMastRespFromPoller-SendRequestConfigToPoller");
		SendRequestConfigToPoller();
	}

	CInternalMessage *msg = new CInternalMessage( INT_MSG_SET_SLOTMASTID, (DWORD)m_slotMastID);
	msg->SetData( (WORD)changeSlotMastRespMsg->SlotMastChangeResponse() );
	SendToInternals( msg, QUEUE_PRIORITY_HIGH );
}


// CPollerProtocol::ProcessSentinelConfigMsgFromPoller(bool hdrDataLengthValid, PollerMessageHeader &hdr, const byte *dataPtr)
//     IN hdrDataLengthValid - true if hdr.DataLength is valid and is the length of data that dataPtr points to, false otherwise.
//     IN hdr - MessageHeader for received message.
//     IN dataPtr - pointer to data for received message.
// Process a Sentinel
void CPollerProtocol::ProcessSentinelConfigMsgFromPoller(bool hdrDataLengthValid, PollerMessageHeader &hdr, const byte *dataPtr)
{
	LogString(POLLER_LOG, "Received Config Response.");
	if (hdrDataLengthValid)
	{
		CPollerConfigRespMessage *pollerMsg = new CPollerConfigRespMessage(hdr, dataPtr);
		ProcessConfigFromPoller(pollerMsg);
		delete pollerMsg;
	}
	else
	{
		LogString(ERROR_LOG, "SentinelConfig Mismatched Buffer Size.");
	}
}

void CPollerProtocol::ProcessConfigFromPoller(CPollerConfigRespMessage *configMsg)
{
	CConfigFields* objConfigFields(configMsg->GetFields());
	CConfigFeatureFlagField* configFeatureFlagField = dynamic_cast<CConfigFeatureFlagField*>(objConfigFields->GetFieldByType(FEATURE_FLAGS));
	if (configFeatureFlagField != nullptr)
	{
		//Set The voucher separation flag
		SetVoucherSeparationEnabled(configFeatureFlagField->VoucherSeparationEnabled());

		//Set The MGMD flag
		SetMGMDEnabled(configFeatureFlagField->MultiGameMultiDenom());
	
		//set game list chunk flag		
		SetChunkedGamesListEnabled(configFeatureFlagField->IsChunkedGamesListEnabled());
	}
	else
	{
		LogString(ERROR_LOG, "ConfigFeatureFlagField object null");
	}

	SendToInternals(new CInternalMessage(INT_MSG_SET_SYSTEM_CONFIG, objConfigFields), QUEUE_PRIORITY_NORMAL);

	if (EnumConfigState::RequestSent == m_configState)
	{
		m_configState = EnumConfigState::Received;
	}

	m_connectivityStatus.flags.isPollerOnline = true;
	SendOnlineToInternals();

	// Notify internals of protocol version currently being used.
	WORD protVer = (m_packetVersion < m_pollerPacketVersion) ? m_packetVersion : m_pollerPacketVersion;
	CInternalMessage *versionMsg = new CInternalMessage ( INT_MSG_SCF_PROTOCOL_VERSION, protVer );
	SendToInternals (versionMsg, QUEUE_PRIORITY_NORMAL );

}

void CPollerProtocol::SendStatusAfterParsingConfigFromInternals()
{
	//Send this any time we receive a config.
	//This will keep the data fresh TR #29541
	SendStatusToPoller();

	if ( m_bSendFullDataAfterConfig )
	{
		LogString(GAMES, "[%s]: Triggerring to send 304/305 messages after receiving "
			"401 Config response (at poller connect).", __FUNCTION__);

		/*CMeters *meters = new CMeters( MEMORY_NONE );
		for ( int mi = mNone + 1; mi < mMeterLast; mi++ )
		meters->SetMeter( mi );*/
		CInternalMessage *metersMsg = new CInternalMessage( INT_MSG_GET_METERS, (WORD) EGM_GAME_NUMBER );
		SendToInternals( metersMsg, QUEUE_PRIORITY_NORMAL );

		CInternalMessage *subMetersMsg = new CInternalMessage( INT_MSG_GET_SUBGAME_METERS );
		SendToInternals( subMetersMsg, QUEUE_PRIORITY_NORMAL );

		m_bSendFullDataAfterConfig = false;
	}

}

void CPollerProtocol::ProcessPerformLoadBalancingFromInternals()
{
	m_performLoadBalancing = true;
}

void CPollerProtocol::ProcessSentinelTimerFromPoller( CPollerTimeMessage *timeMsg )
{
	CInternalMessage *msg = new CInternalMessage( INT_MSG_SET_TIME, (BYTE*)timeMsg->GetTime(), sizeof(SYSTEMTIME));

	// Add tick count to determine how long it takes to get this message to internals.
	msg->SetData((DWORD)CUtilities::GetTickCount());
	SendToInternals( msg, QUEUE_PRIORITY_HIGH );
}

void CPollerProtocol::ProcessDisplayMessageFromPoller( CPollerDisplayMessage *displayMsg )
{
	DisplayMessageType dispMsgType;
	string dispMessage = displayMsg->GetDisplayString( dispMsgType );

	CInternalMessage *msg = new CInternalMessage( INT_MSG_SET_DISPLAY_MESSAGE, dispMessage );
	msg->SetData( (WORD)dispMsgType );

	SendToInternals( msg, QUEUE_PRIORITY_BELOW_NORMAL );
}

void CPollerProtocol::ProcessGlobalPasswordFromPoller( CPollerGlobalPasswordMessage *globalPasswordMsg )
{
	WORD password = globalPasswordMsg->GetPassword();

	CInternalMessage *msg = new CInternalMessage( INT_MSG_SET_GLOBAL_PASSWORD, password );

	SendToInternals( msg, QUEUE_PRIORITY_NORMAL );
}

void CPollerProtocol::ProcessDefaultDataDirectoryUpdated(CInternalMessage * msg)
{
	// Try to consume new poller IP.
	ProcessPollerIPFileIfExist();
}

void CPollerProtocol::SetMessageHeader(CPollerMessage * msg)
{
	memcpy_s(msg->MessageHeader.MacAddress, SIZE_OF_MAC_ADDRESS, m_macAddress, SIZE_OF_MAC_ADDRESS);
	msg->MessageHeader.PacketVersion = m_packetVersion;
	msg->MessageHeader.SequenceNumber = m_sequenceNumber++;

	msg->MessageHeader.SlotMastID = m_slotMastID;
	msg->MessageHeader.SlotmastRevision = m_slotMastRevision;
}

void CPollerProtocol::GetHeader(PollerMessageHeader * hdr, BYTE* temprecvpacket)
{
	int offset = 0;
	memcpy(&(hdr->PacketVersion), temprecvpacket + offset, sizeof( hdr->PacketVersion ));
	offset += sizeof(hdr->PacketVersion);

	memcpy(&(hdr->MessageID), temprecvpacket + offset, sizeof( hdr->MessageID ));
	offset +=  sizeof(hdr->MessageID);

	memcpy(&(hdr->DataLength), temprecvpacket + offset, sizeof( hdr->DataLength ));
	offset +=  sizeof(hdr->DataLength);

	memcpy(&(hdr->MacAddress), temprecvpacket + offset, sizeof( hdr->MacAddress ));
	offset +=  sizeof(hdr->MacAddress);

	memcpy(&(hdr->SequenceNumber), temprecvpacket + offset, sizeof( hdr->SequenceNumber ));
	offset +=  sizeof(hdr->SequenceNumber);

	memcpy(&(hdr->SlotMastID), temprecvpacket + offset, sizeof( hdr->SlotMastID ));
	offset +=  sizeof(hdr->SlotMastID);

	memcpy(&(hdr->SlotmastRevision), temprecvpacket + offset, sizeof( hdr->SlotmastRevision ));

}

DWORD CPollerProtocol::GetUniqueId()
{
	// retrieve next uniqueid to use for poller transactions
	if (m_uniqueId++ >= UINT_MAX)
		m_uniqueId = 1;

	return m_uniqueId;
}

void CPollerProtocol::SendCardInToPoller(CInternalMessage *msg)
{
	LogString(POLLER_LOG, "SendCardInToPoller: %s", InternalMsgTypeStr[msg->GetMessageType()]);

	DWORD uniqueId;

	// Need to prevent msg->m_metersData and m_player from getting deleted when msg is deleted
	CMeters *msgMeters = msg->GetTransactionMetersData();
	CPlayer *msgPlayer = msg->GetPlayerData();
	msg->SetTransactionMetersData((CMeters *)NULL);
	msg->SetData((CPlayer *)NULL);

	int gameNumber 	= msg->GetIntData();
	int denom 		= msg->GetIntData(true);
	DWORD ptsn 		= (DWORD)msg->GetInt64Data();
	DWORD psid 		= (DWORD)msg->GetInt64Data(true);
	bool mgmdGlobal     = (bool) msg->GetWordData();
	bool mgmdAvailable = (bool) msg->GetWordData(true);
	
	LogString( POLLER_LOG, "Received %s (%d) := PlayTx SN: %u, denom: %d, gamenum: %d", 
		__FUNCTION__, __LINE__, ptsn, denom, gameNumber);

	uniqueId = GetUniqueId();
	time_t *transTime = msg->GetTimeData();
	CPollerPlayerTransactionMessage *pollerMsg = new CPollerPlayerTransactionMessage(msgPlayer,msgMeters, msg->GetMessageType(),
		transTime, uniqueId, ptsn, denom, gameNumber,mgmdGlobal,mgmdAvailable);
	SetMessageHeader(pollerMsg);

	DWORD status = msg->GetDWordData(true);
	// adjust status, show transaction backlog
	// TBD: how to know backlog exists.
	pollerMsg->SetStatus(status & 0xFFF7FFFF); // clear bit, for now.

    //Pass Play Session ID
	LogString(POLLER_LOG, "%s: PollerProtocol: Play Session ID received from internal: %u", __FUNCTION__, psid);
	pollerMsg->SetPlaySessionID(psid);

	byte *transBuffer;
	DWORD transLength;
	TransactionType transactionType(INT_MSG_TRANSACTION_PLAYER_CARD_IN == msg->GetMessageType() ? PLAYER_CARD_IN_CODE : VIRTUAL_SESSION_STARTED);
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transactionType, uniqueId, transBuffer, transLength);
}

void CPollerProtocol::SendCardOutToPoller(CInternalMessage *msg)
{
	LogString(POLLER_LOG, "SendCardOutToPoller: %s", InternalMsgTypeStr[msg->GetMessageType()]);

	DWORD uniqueId;

	// Need to prevent msg->m_metersData and m_player from getting deleted when msg is deleted
	CMeters *msgMeters = msg->GetTransactionMetersData();
	CPlayer *msgPlayer = msg->GetPlayerData();
	msg->SetTransactionMetersData((CMeters *)NULL);
	msg->SetData((CPlayer *)NULL);

	int gameNumber 	= msg->GetIntData();
	int denom 		= msg->GetIntData(true);
	DWORD ptsn 		= (DWORD)msg->GetInt64Data();
	DWORD psid 		= (DWORD)msg->GetInt64Data(true);
	bool mgmdGlobal     = (bool) msg->GetWordData();
	bool mgmdAvailable = (bool) msg->GetWordData(true);
	
	LogString( POLLER_LOG, "Received %s (%d) := PlayTx SN: %u, denom: %d, gamenum: %d", 
		__FUNCTION__, __LINE__, ptsn, denom, gameNumber);

	uniqueId = GetUniqueId();
	time_t *transTime = msg->GetTimeData();
	CPollerPlayerTransactionMessage *pollerMsg = new CPollerPlayerTransactionMessage(msgPlayer,msgMeters, INT_MSG_TRANSACTION_PLAYER_CARD_OUT, 
		transTime, uniqueId, ptsn, denom, gameNumber,mgmdGlobal,mgmdAvailable);
	DWORD status = msg->GetDWordData(true);
	// adjust status, show transaction backlog
	// TBD: how to know backlog exists.
	pollerMsg->SetStatus(status & 0xFFF7FFFF); // clear bit, for now.

    //Pass Play Session ID
	LogString(POLLER_LOG, "%s: PollerProtocol: Play Session ID received from internal: %u", __FUNCTION__, psid);
	pollerMsg->SetPlaySessionID(psid);

	SetMessageHeader(pollerMsg);
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	if (msgPlayer->IsBannedFromPlay())
	{
		// We tell internals about the card out so that we can unlock the game if it was locked
		CInternalMessage *piMsg = new CInternalMessage( INT_MSG_SCF_CARDOUT);
		SendToInternals( piMsg, QUEUE_PRIORITY_NORMAL );
	}

	QueueToPoller(pollerMsg, PLAYER_CARD_OUT_CODE, uniqueId, transBuffer, (WORD)transLength);
}

void CPollerProtocol::SendGameSessionChange(CInternalMessage *msg)
{
	LogString(POLLER_LOG, "PollerProtocol: Sending GameSessionChange to Poller.");

	if(msg != nullptr)
	{
		size_t size;

		// Need to prevent msg->m_metersData and m_player from getting deleted when msg is deleted
		CMeters *meters = msg->GetTransactionMetersData();
		CPlayer *player = msg->GetPlayerData();

		msg->SetTransactionMetersData((CMeters *)NULL);
		msg->SetData((CPlayer *)NULL);
		
		DWORD ptsn 		= (DWORD)msg->GetInt64Data();
		int gameNumber 	= msg->GetIntData();
		int denom 		= msg->GetIntData(true);
		DWORD psid 		= (DWORD)msg->GetInt64Data(true);
		byte mgmdAvailable = (byte) msg->GetWordData();

		if (mgmdAvailable)
			LogString(POLLER_LOG, "In %s := isMGMD: Yes", __FUNCTION__);
		else
			LogString(POLLER_LOG, "In %s := isMGMD: No", __FUNCTION__);

		LogString( POLLER_LOG, "Received %s (%d) := PlayTx SN: %u, denom: %d, gamenum: %d, psid: %u", 
			__FUNCTION__, __LINE__, ptsn, denom, gameNumber, psid);

		DWORD uniqueId = GetUniqueId();
		time_t *transTime = msg->GetTimeData();
		CPollerGameSessionChangeTransactionMessage *transMsg = new CPollerGameSessionChangeTransactionMessage(
			meters, player, msg->GetBytesData(size), transTime, uniqueId, psid,  ptsn, denom, gameNumber, mgmdAvailable);

		DWORD status = msg->GetDWordData(true);
		// adjust status, show transaction backlog
		// TBD: how to know backlog exists.
		transMsg->SetStatus(status & 0xFFF7FFFF); // clear bit, for now.

		SetMessageHeader(transMsg);
		
		byte *transBuffer;
		DWORD transLength;
		transBuffer = transMsg->GetReadWriteBuffer(transLength);
		
		LogString(POLLER_LOG, "Sending Game Session Change message (127) to Poller");
		QueueToPoller(transMsg, GAME_SESSION_CHANGE, uniqueId, transBuffer, transLength);
	}
}

void CPollerProtocol::SendBasicTransactionToPoller( CInternalMessage *msg )
{
	byte transType = NONE_SPECIFIED;
	DWORD uniqueId;
	size_t size;
	SYSTEMTIME transTime = CUtilities::GetUTCSystemTime(*(msg->GetTimeData()));

	switch (msg->GetMessageType())
	{
	case INT_MSG_TRANSACTION_SENTINEL_ON:
		LogString(POLLER_LOG, "PollerProtocol: Sending Sentinel On.");
		if ( msg->GetWordData() == NVRAM_MEMORY_BAD )
		{
			transType = SENTINEL_ON_MEM_BAD_CODE;
		}
		else
		{
			transType = SENTINEL_ON_MEM_OK_CODE;	
		}
		break;

	case INT_MSG_TRANSACTION_ILLEGAL_DOOR_OPEN:
		LogString(POLLER_LOG, "PollerProtocol: Sending Illegal Door Open.");
		transType = ILLEGAL_DOOR_OPEN_CODE;
		break;

	case INT_MSG_TRANSACTION_METER_RUNAWAY:
		LogString(POLLER_LOG, "PollerProtocol: Sending Meter Runaway.");
		transType = METER_RUNAWAY_CODE;
		break;

	case INT_MSG_DOOR_OPEN:
		LogString(POLLER_LOG, "PollerProtocol: Sending Door Open.");
		switch (msg->GetWordData()) 
		{
		case mSDr:
			transType = SLOT_DOOR_OPEN_CODE;
			break;
		case mDDr:
			transType = DROP_DOOR_OPEN_CODE;
			break;
		case mLDr:
			transType = LOGIC_DOOR_OPEN_CODE;
			break;
		case mCDr:
			transType = CASH_DOOR_OPEN_CODE;
			break;
		case mFDr:
			transType = FILL_DOOR_OPEN_CODE;
			break;
		case mDr:
			transType = DOOR_OPEN_CODE;
			break;
		case mChassisDoor:
			transType = CHASSIS_DOOR_OPEN_CODE;
			break;
		case mBDr:
			transType = BELLY_DOOR_OPEN_CODE;
			break;
		}
		break;

	case INT_MSG_DOOR_CLOSED:
		LogString(POLLER_LOG, "PollerProtocol: Sending Door Closed.");
		switch (msg->GetWordData()) 
		{
		case mSDr:
			transType = SLOT_DOOR_CLOSED_CODE;
			break;
		case mDDr:
			transType = DROP_DOOR_CLOSED_CODE;
			break;
		case mLDr:
			transType = LOGIC_DOOR_CLOSED_CODE;
			break;
		case mCDr:
			transType = CASH_DOOR_CLOSED_CODE;
			break;
		case mFDr:
			transType = FILL_DOOR_CLOSED_CODE;
			break;
		case mDr:
			transType = DOOR_CLOSED_CODE;
			break;
		case mChassisDoor:
			transType = CHASSIS_DOOR_CLOSED_CODE;
			break;
		case mBDr:
			transType = BELLY_DOOR_CLOSED_CODE;
			break;
		}
		break;

	case INT_MSG_TRANSACTION_PD_CHASSIS_OPEN:
		LogString(POLLER_LOG, "PollerProtocol: Sending Chassis Door State.");
		transType = CHASSIS_DOOR_OPEN_POWER_DOWN_CODE;
		break;

	case INT_MSG_SEND_STACKER_OUT:
		LogString(POLLER_LOG, "PollerProtocol: Sending Stacker removed.");
		transType = STACKER_REMOVED_CODE;
		break;

	case INT_MSG_SEND_STACKER_IN:
		LogString(POLLER_LOG, "PollerProtocol: Sending Stacker installed.");
		transType = STACKER_INSTALLED_CODE;
		break;

	case INT_MSG_TRANSACTION_SLOT_POWER_ON:
		LogString(POLLER_LOG, "PollerProtocol: Sending Slot Power On.");
		transType = SLOT_POWER_ON_CODE;
		break;

	case INT_MSG_TRANSACTION_SLOT_POWER_OFF:
		LogString(POLLER_LOG, "PollerProtocol: Sending Slot Power Off.");
		transType = SLOT_POWER_OFF_CODE;
		break;

	case INT_MSG_SEND_WITNESS:
		LogString(POLLER_LOG, "PollerProtocol: Sending Witness To Poller.");
		transType = WITNESS_CODE;
		break;

	case INT_MSG_BAD_CARD_READ:
		LogString(POLLER_LOG, "PollerProtocol: Sending Possible Bad Card Reader.");
		transType = POSSIBLE_BAD_CARD_READER_CODE;
		break;
	case INT_MSG_TRANSACTION_CHANGE_LIGHT_ON:
		LogString(POLLER_LOG, "PollerProtocol: Sending Change Light On.");
		transType = CHANGE_LIGHT_ON_CODE;
		break;
	case INT_MSG_TRANSACTION_CHANGE_LIGHT_OFF:
		LogString(POLLER_LOG, "PollerProtocol: Sending Change Light Off.");
		transType = CHANGE_LIGHT_OFF_CODE;
		break;

	case INT_MSG_TRANSACTION_CONTROLLED_REBOOT_WARNING:
		LogString(POLLER_LOG, "PollerProtocol: Sending Controlled Reboot Warning.");
		transType = CONTROLLED_REBOOT_WARNING_CODE;
		break;

	case INT_MSG_TRANSACTION_CONTROLLED_REBOOT:
		LogString(POLLER_LOG, "PollerProtocol: Sending Controlled Reboot.");
		transType = CONTROLLED_REBOOT_CODE;
		break;
	case INT_MSG_TRANSACTION_AUTO_UPLOAD_CANCELED:
		LogString(POLLER_LOG, "PollerProtocol: Sending auto upload canceled by user.");
		transType = AUTO_UPLOAD_CANCELED_BY_USER;
		break;
	case INT_MSG_TRANSACTION_PROXIMITY_REQUEST_BEACON_SCAN_LIST:
		LogString(POLLER_LOG, "PollerProtocol: Sending Proximity Beacon Scan List Request.");
		transType = PROXIMITY_REQUEST_BEACON_SCAN_LIST;
		break;

	default:
		LogString(POLLER_LOG, "PollerProtocol: INVALID TRANSACTION.");
		transType = NONE_SPECIFIED;
		break;
	}

	CMeters *msgMeters = msg->GetTransactionMetersData();

	uniqueId = GetUniqueId();

	CPollerBasicTransactionMessage *pollerMsg = new CPollerBasicTransactionMessage(msgMeters, transType, uniqueId, msg->GetBytesData(size), transTime);
	DWORD status = msg->GetDWordData(true);
	pollerMsg->SetStatus(status);

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transType, uniqueId, transBuffer, (WORD)transLength);	
}

void CPollerProtocol::SendPlayerUpdateToPoller(CInternalMessage *msg)
{
	LogString(POLLER_LOG, "PollerProtocol: Sending Player Update.");

	DWORD uniqueId;

	// Need to prevent msg->m_metersData and m_player from getting deleted when msg is deleted
	CMeters *msgMeters = msg->GetTransactionMetersData();
	CPlayer *msgPlayer = msg->GetPlayerData();
	msg->SetTransactionMetersData((CMeters *)NULL);
	msg->SetData((CPlayer *)NULL);
	
	int gameNumber 	= msg->GetIntData();
	int denom 		= msg->GetIntData(true);
	DWORD ptsn 		= (DWORD)msg->GetInt64Data();
	DWORD psid 		= (DWORD)msg->GetInt64Data(true);
	bool mgmdGlobal     = (bool) msg->GetWordData();
	bool mgmdAvailable = (bool) msg->GetWordData(true);
	
	LogString( POLLER_LOG, "Received %s (%d) := PlayTx SN: %u, denom: %d, gamenum: %d", 
		__FUNCTION__, __LINE__, ptsn, denom, gameNumber);

	uniqueId = GetUniqueId();
	time_t *transTime = msg->GetTimeData();
	CPollerPlayerTransactionMessage *pollerMsg = new CPollerPlayerTransactionMessage(msgPlayer,msgMeters, INT_MSG_TRANSACTION_PLAYER_UPDATE, 
		transTime,uniqueId, ptsn, denom, gameNumber,mgmdGlobal,mgmdAvailable);
	DWORD status = msg->GetDWordData(true);
	// adjust status, show transaction backlog
	// TBD: how to know backlog exists.
	pollerMsg->SetStatus(status & 0xFFF7FFFF); // clear bit, for now.

    //Pass Play Session ID
	LogString(POLLER_LOG, "%s: PollerProtocol: Play Session ID received from internal: %u", __FUNCTION__, psid);
	pollerMsg->SetPlaySessionID(psid);

	SetMessageHeader(pollerMsg);
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, PLAYER_UPDATE_CODE, uniqueId, transBuffer, (WORD)transLength);
}



void CPollerProtocol::SendEmployeeCardToPoller(CInternalMessage * msg, bool cardIn )
{
	byte transType = NONE_SPECIFIED;
	DWORD uniqueId;
	size_t size;
	CEmployee *employee = msg->GetEmployeeData();

	CardType employeeType = employee->GetEmployeeCardType();
	SYSTEMTIME transTime = CUtilities::GetUTCSystemTime(*(msg->GetTimeData()));
	if (cardIn)
	{
		LogString(POLLER_LOG, "PollerProtocol: Sending Employee Card In.");
		switch ( employeeType )
		{
		case CARD_MECHANIC:
			transType = MECHANIC_CARD_IN_CODE;
			break;
		case CARD_GLOBAL:
			transType = GLOBAL_CARD_IN_CODE;
			break;
		case CARD_FLOOR:
			transType = FLOOR_CARD_IN_CODE;
			break;
		case CARD_CURRENCY:
			transType = CURRENCY_CARD_IN_CODE;
			break;
		case CARD_METER:
			transType = METER_CARD_IN_CODE;
			break;
		default:
			break;
		}
	}
	else
	{
		LogString(POLLER_LOG, "PollerProtocol: Sending Employee Card Out.");

		switch ( employeeType )
		{
		case CARD_MECHANIC:
			transType = MECHANIC_CARD_OUT_CODE;
			break;
		case CARD_GLOBAL:
			transType = GLOBAL_CARD_OUT_CODE;
			break;
		case CARD_FLOOR:
			transType = FLOOR_CARD_OUT_CODE;
			break;
		case CARD_CURRENCY:
			transType = CURRENCY_CARD_OUT_CODE;
			break;
		case CARD_METER:
			transType = METER_CARD_OUT_CODE;
			break;
		default:
			break;
		}
	}

	// prevent msg->m_metersData from getting deleted with the msg
	CMeters *msgMeters = msg->GetTransactionMetersData();

	uniqueId = GetUniqueId();

	CPollerBasicTransactionMessage *pollerMsg = new CPollerBasicTransactionMessage(msgMeters, transType, uniqueId, msg->GetBytesData(size), transTime);
	DWORD status = msg->GetDWordData(true);
	// adjust status, show transaction backlog
	// TBD: how to know backlog exists.
	pollerMsg->SetStatus(status & 0xFFF7FFFF); // clear bit, for now.

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transType, uniqueId, transBuffer, (WORD)transLength);	
}



void CPollerProtocol::SendTiltToPoller(CInternalMessage * msg )
{
	TiltCode tilt = (TiltCode)msg->GetWordData();
	DWORD machineNumber = msg->GetDWordData();

	LogString(POLLER_LOG, "PollerProtocol: Sending Tilt %d.", tilt);
	byte transType = COMPOSITE_TILT;
	DWORD uniqueId;
	size_t size;

	if (tilt == tltOpChangedOption)
	{
		m_pollergameListInfo->ProcessGameOptionChange();
	}

	// Need to prevent msg->m_metersData and m_player from getting deleted when msg is deleted
	CMeters *msgMeters = msg->GetTransactionMetersData();
	msg->SetTransactionMetersData((CMeters *)NULL);

	uniqueId = GetUniqueId();

	CPollerTiltTransactionMessage *pollerMsg = new CPollerTiltTransactionMessage(msgMeters,
		transType,
		uniqueId,
		tilt,
		msg->GetBytesData(size),
		machineNumber);

	DWORD status = msg->GetDWordData(true);
	// adjust status, show transaction backlog
	// TBD: how to know backlog exists.
	pollerMsg->SetStatus(status & 0xFFF7FFFF); // clear bit, for now.

	SetMessageHeader(pollerMsg);
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transType, uniqueId, transBuffer, (WORD)transLength);	
}

void CPollerProtocol::SendBonusEventsToPoller(CInternalMessage * msg )
{
	LogString(POLLER_LOG, "PollerProtocol: Sending Bonus Event.");
	byte transType;
	DWORD uniqueId;
	byte *transBuffer;
	DWORD transLength;
	DWORD status = msg->GetDWordData(true);
	CBonusEvents * msgBonusEvents = msg->GetBonusEventsData();
	CBonusEvent * bonusEvent = NULL;
	CPlayer * msgPlayer = msg->GetPlayerData();
	CMeters * msgMeters = msg->GetTransactionMetersData();
	//Don't set this meters object to null.

	CPollerBonusTransactionMessage * pollerMsg;

	for ( int i = 0; i < msgBonusEvents->GetBonusEventsCount(); i++)
	{
		bonusEvent = msgBonusEvents->GetBonusEvent(i);

		switch (bonusEvent->GetBonusEventType())
		{
		case ONE_TIME_BONUS:
			transType = ONE_TIME_BONUS_EVENT_CODE;
			break;
		case MULTIPLIER_BONUS: 
			transType = MULTIPLE_BONUS_EVENT_CODE;
			break;
		default:
			LogString(ERROR_LOG, "Invalid Bonus Event Type.");	
			continue;	
		}

		uniqueId = GetUniqueId();
		pollerMsg = new CPollerBonusTransactionMessage(msgMeters, msgPlayer, bonusEvent, transType, uniqueId);
		// TBD: how to know backlog exists.
		pollerMsg->SetStatus(status & 0xFFF7FFFF); // clear bit, for now.
		SetMessageHeader(pollerMsg);
		//When looping we need to set the buffer here, so the meters object can be deleted.
		pollerMsg->SetReadWriteBuffer();	
		transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

		QueueToPoller(pollerMsg, transType, uniqueId, transBuffer, (WORD)transLength);		
	}
}

void CPollerProtocol::SendCashTransferToPoller( CInternalMessage *  msg )
{
	LogString(POLLER_LOG, "PollerProtocol: Sending Cash Transfer.");
	DWORD uniqueId;
	byte *transBuffer;
	DWORD transLength;
	WORD gameNumber;
	DWORD status = msg->GetDWordData(true);
	time_t *transTime = msg->GetTimeData();
	CPlayer * msgPlayer = msg->GetPlayerData();
	CMeters * endMeters = msg->GetTransactionMetersData();
	CMeters * startMeters = msg->GetMetersData(gameNumber);

	time_t activeTime = *transTime - msgPlayer->GetCard()->GetCardInTime();
	LONGLONG activeSeconds = activeTime;

	//don't delete the meter / player objects here.

	CPollerCTTTransactionMessage * pollerMsg;

	//there are 14 meters that must be sent in a CTT Transaction if they changed
	//between the beginning and end meters.

	//so....

	byte metersToCheck [] = {mD1, mD2, mD5, mD10, mD20, mD50, mD100, mTCi, mCoupV, mPbAC2G, mPtR, mPbAN2G, mPbAR2G, gJP};
	CMeter * endMeter;
	CMeter * startMeter;
	DWORD dwDifference;

	for (DWORD i = 0; i < sizeof(metersToCheck); i++)
	{
		startMeter = startMeters->GetMeterByNumber(metersToCheck[i]);
		endMeter = endMeters->GetMeterByNumber(metersToCheck[i]);

		// Ensure that we are able to proceed well even if a meter was not  
		// in the start meters because its value was not yet available.
		__int64 startMeterValue = startMeter ? startMeter->GetValue() : 0;
		__int64 endMeterValue = endMeter ? endMeter->GetValue() : 0;

		dwDifference = (DWORD)(endMeterValue - startMeterValue);

		if ( dwDifference > 0 )
		{
			uniqueId = GetUniqueId();
			pollerMsg = new CPollerCTTTransactionMessage(endMeters, msgPlayer, uniqueId, i + 1, dwDifference, activeSeconds);

			pollerMsg->SetStatus(status & 0xFFF7FFFF); // clear bit, for now.
			SetMessageHeader(pollerMsg);
			pollerMsg->SetReadWriteBuffer();
			transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

			QueueToPoller(pollerMsg, CASH_TRANSFER_TRACKING_CODE, uniqueId, transBuffer, (WORD)transLength);
		}
	}
}

void CPollerProtocol::ProcessPBAuthorize( CInternalMessage * msg )
{
	LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
	ProcessPBTransfer( msg );
}

void CPollerProtocol::ProcessPBTransfer( CInternalMessage * msg )
{
	CPersonalBankerTransfer * msgPBT = msg->GetPBTransferData();
	PersonalBankerType pbtType(msgPBT->GetPersonalBankerType());
	PersonalBankerType promoPbtType(msgPBT->GetPromoPersonalBankerType());
	LogString(PBT_LOG, "ProcessPBTransfer PersonalBankerType=%s amount=%lld NonrestrictedAmount=%lld RestrictedAmount=%lld",
		msgPBT->GetPBTypeText(pbtType).c_str(),
		msgPBT->GetAmount(),
		msgPBT->GetNonrestrictedAmount(),
		msgPBT->GetRestrictedAmount());

	bool bSent = false;
	//if it's a cash upload, check for restricted and unrestricted too.
	if (pbtType == CashUpload ||
		pbtType == CashUploadAuto ||
		promoPbtType == PromoCreditsUpload ||
		promoPbtType == PromoCreditsUploadAuto)
	{
		if (msgPBT->GetAmount() > 0)
		{
			if (msgPBT->IsCashoutToHostTransfer())
			{
				pbtType = CashUploadCashout;
			}
			else if(msgPBT->IsPartialWATOutTransfer())
			{
				pbtType = PartialCashUpload;
			}
			else if (msgPBT->IsProcessTaxDeduction())
			{
				pbtType = TaxWithheld;
			}
			SendPBToPoller(msg, pbtType);
			bSent = true;
		}
		if (msgPBT->GetRestrictedAmount() > 0)
		{
			SendPBToPoller(msg, msgPBT->IsCashoutToHostTransfer() ? PromoCreditsUploadCashout :
				promoPbtType);
			bSent = true;
		}
		if (msgPBT->GetNonrestrictedAmount() > 0)
		{
			SendPBToPoller(msg, msgPBT->IsCashoutToHostTransfer() ? PointCashUploadCashout :
				(msgPBT->IsPartialCEPOutTransfer() ? PartialPointCashUpload :
				(pbtType == CashUploadAuto ? PointCashUploadAuto : PointCashUpload)));
			bSent = true;
		}
		if (!bSent)
		{
			SendPBToPoller(msg, Unspecified);
		}
	}
	else if (pbtType == CanceledCashUpload ||
		     promoPbtType == CanceledPromoCreditsUpload)
	{
		if (pbtType == CanceledCashUpload)
		{			
			if (msgPBT->IsCashoutToHostTransfer())
			{
				pbtType = CanceledCashUploadCashout;
			}
			else if (msgPBT->IsPartialWATOutTransfer())
			{
				pbtType = CanceledPartialCashUpload;
			}
			else if (msgPBT->IsProcessTaxDeduction())
			{
				pbtType = CanceledTaxWithheld;
			}
			SendPBToPoller(msg, pbtType);
			bSent = true;
		}
		if (promoPbtType == CanceledPromoCreditsUpload)
		{
			SendPBToPoller(msg, msgPBT->IsCashoutToHostTransfer() ? CanceledPromoCreditsUploadCashout : 
				CanceledPromoCreditsUpload);
			bSent = true;
		}
		if (msgPBT->GetNonrestrictedAmount() > 0)
		{
			SendPBToPoller(msg, msgPBT->IsCashoutToHostTransfer() ? CanceledPointCashUploadCashout : 
				(msgPBT->IsPartialCEPOutTransfer() ? CanceledPartialPointCashUpload : CanceledPointCashUpload));
			bSent = true;
		}
		if (!bSent)
		{
			SendPBToPoller(msg, Unspecified);
		}
	}
	else if (pbtType == CanceledCashUploadAuto ||
		     pbtType == CanceledPointCashUploadAuto ||
		     pbtType == CanceledPromoCreditsUploadAuto ||
		     promoPbtType == CanceledPromoCreditsUploadAuto)
	{
		if (msgPBT->GetAmount() > 0)
		{
			SendPBToPoller(msg, CanceledCashUploadAuto);
			bSent = true;
		}
		if (msgPBT->GetNonrestrictedAmount() > 0)
		{
			SendPBToPoller(msg, CanceledPointCashUploadAuto);
			bSent = true;
		}
		if (msgPBT->GetRestrictedAmount() > 0)
		{
			SendPBToPoller(msg, CanceledPromoCreditsUploadAuto);
			bSent = true;
		}
		if (!bSent)
		{
			SendPBToPoller(msg, Unspecified);
		}
	}
	else
	{
		if (AutomaticUploadCanceledByUser == msgPBT->GetCancelledType() ||
			AutomaticUploadCanceledBySystem == msgPBT->GetCancelledType())
		{
			if (msgPBT->GetAmount() > 0)
			{
				SendPBToPoller(msg, CanceledCashUploadAuto);
				bSent = true;
			}
			if (msgPBT->GetNonrestrictedAmount() > 0)
			{
				SendPBToPoller(msg, CanceledPointCashUploadAuto);
				bSent = true;
			}
			if (msgPBT->GetRestrictedAmount() > 0)
			{
				SendPBToPoller(msg, CanceledPromoCreditsUploadAuto);
				bSent = true;
			}
			
			if (!bSent)
			{
				SendPBToPoller(msg, pbtType);
			}			
		}
		else
		{
			SendPBToPoller(msg, Unspecified);
		}
	}
}

void CPollerProtocol::SendPBToPoller(CInternalMessage * msg, PersonalBankerType ammendedPBtype)
{
	LogString(POLLER_LOG, "PollerProtocol: Sending Personal Banker Transaction.");

	byte transType = PERSONAL_BANKER_CODE;
	DWORD uniqueId;

	CMeters *msgMeters = msg->GetTransactionMetersData();
	CPersonalBankerTransfer * msgPBT = msg->GetPBTransferData();
	uniqueId = GetUniqueId();
	time_t *transTime = msg->GetTimeData();
	const byte *cardNumber = nullptr;

	if (msgPBT)
	{
		int size = 0;
		cardNumber = msgPBT->GetCardNumber(size);
		LogString(PBT_LOG, "Sending PersonalBankerType=%s", msgPBT->GetPBTypeText(ammendedPBtype).c_str());
	}

	CPollerPBTransactionMessage *pollerMsg = new CPollerPBTransactionMessage(msgMeters, msgPBT, transType, *transTime, uniqueId, cardNumber, ammendedPBtype);
	DWORD status = msg->GetDWordData(true);
	// adjust status, show transaction backlog
	// TBD: how to know backlog exists.
	pollerMsg->SetStatus(status & 0xFFF7FFFF); // clear bit, for now.

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	bool stored = QueueToPoller(pollerMsg, transType, uniqueId, transBuffer, (WORD)transLength);	

	if (stored)
	{
		CInternalMessage *respMsg = new CInternalMessage(INT_MSG_PB_STORED);
		SendMessageToInternals(respMsg, QUEUE_PRIORITY_NORMAL);
	}
}

void CPollerProtocol::SendSnapshotToPoller( CInternalMessage* msg )
{
	LogString(POLLER_LOG, "PollerProtocol: Sending Snapshot Transaction.");

	DWORD uniqueId;
	size_t size;
	// gamenumber is word - if it's zero it's transaction121, any other is 122
	WORD gameNumber = msg->GetWordData();
	// snapshot type is in extended word
	SnapshotType SSType = (SnapshotType)msg->GetWordData(true);


	//egm meters are always in the transaction meters object
	CMeters *msgMeters = msg->GetTransactionMetersData();
	uniqueId = GetUniqueId();


	// commdown is in the status
	DWORD status = msg->GetDWordData(true);
	bool commDown = (status & 0x00000400) == 0x00000400;

	byte *transBuffer;
	DWORD transLength;
	SYSTEMTIME transTime = CUtilities::GetUTCSystemTime(*(msg->GetTimeData()));
	if (gameNumber == 0)
	{
		CPollerEGMSnapshotTransactionMessage *pollerMsg = new CPollerEGMSnapshotTransactionMessage(msgMeters,
			EGM_SNAPSHOT_CODE,
			transTime,
			uniqueId,
			SSType,
			commDown,
			msg->GetBytesData(size));
		pollerMsg->SetStatus(status & 0xFFF7FFFF); // clear bit, for now.

		SetMessageHeader(pollerMsg);
		pollerMsg->SetReadWriteBuffer();
		transBuffer = pollerMsg->GetReadWriteBuffer(transLength);


		QueueToPoller(pollerMsg, EGM_SNAPSHOT_CODE, uniqueId, transBuffer, (WORD)transLength);
	}
	else
	{
		CGames * games = msg->GetGamesData();
		CGame * game = games->GetGameByIndex(0);
		CPollerSubGameSnapshotTransactionMessage * pollerMsg = new CPollerSubGameSnapshotTransactionMessage(msgMeters,
			SUBGAME_SNAPSHOT_CODE,
			transTime,
			uniqueId,
			commDown,
			gameNumber,
			game->GetMeters(),
			SSType,
			msg->GetBytesData(size),
			game->GetMfgID(),
			game->GetPaytableID(),
			game->GetAdditionalID());
		pollerMsg->SetStatus(status & 0xFFF7FFFF); // clear bit, for now.

		SetMessageHeader(pollerMsg);
		pollerMsg->SetReadWriteBuffer();
		transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

		QueueToPoller(pollerMsg, SUBGAME_SNAPSHOT_CODE, uniqueId, transBuffer, (WORD)transLength);
	}

}

void CPollerProtocol::SendMachineMoveToPoller( CInternalMessage *msg )
{
	LogString(POLLER_LOG, "PollerProtocol: Sending Machine Move / Game Info Transaction.");


	DWORD uniqueId;
	size_t size;
	CMeters *msgMeters = msg->GetTransactionMetersData();
	uniqueId = GetUniqueId();

	WORD denom = msg->GetWordData();
	WORD machineType = msg->GetWordData(true);
	DWORD machineNumber = msg->GetDWordData();

	byte transType = NONE_SPECIFIED;

	switch(msg->GetMessageType())
	{
	case INT_MSG_TRANSACTION_MACHINE_ONLINE:
		transType = MACHINE_ONLINE_CODE;
		break;
	case INT_MSG_TRANSACTION_MACHINE_OFFLINE:
		transType = MACHINE_OFFLINE_CODE;
		break;
	case INT_MSG_TRANSACTION_MACHINE_MOVE_BEGIN:
		transType = BEGIN_MACHINE_MOVE_CODE;
		break;
	case INT_MSG_TRANSACTION_MACHINE_MOVE_COMPLETE:
		transType = MACHINE_MOVE_COMPLETE_CODE;
		break;
	case INT_MSG_TRANSACTION_GAME_INFO:
		transType = GAME_INFO_CODE;
		break;
	default:
		break;
	}

	// commdown is in the status
	DWORD status = msg->GetDWordData(true);

	byte *transBuffer;
	DWORD transLength;

	CPollerMachineMoveTransactionMessage *pollerMsg = new CPollerMachineMoveTransactionMessage(msgMeters,
		transType,
		uniqueId,
		msg->GetBytesData(size),
		denom,
		machineType,
		machineNumber);
	pollerMsg->SetStatus(status & 0xFFF7FFFF); // clear bit, for now.

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transType, uniqueId, transBuffer, (WORD)transLength);
}

void CPollerProtocol::SendOneLinkTransactionToPoller(CInternalMessage *msg)
{
	byte transCode(0);
	int64_t sessionPoints(0), bonusPoints(0), residualValue(0);
	CEGMAward::GetFieldsForOneLinkTransactionMessage(msg, transCode, sessionPoints, bonusPoints, residualValue);
	LogString(POLLER_LOG, _T("PollerProtocol: SendOneLinkTransactionToPoller transCode=%d. sessionPoints=%lld bonusPoints=%lld residualValue=%lld"),
			transCode, sessionPoints, bonusPoints, residualValue);
	DWORD uniqueId;
	size_t size;
	CMeters *msgMeters = msg->GetTransactionMetersData();
	uniqueId = GetUniqueId();
	CPollerOneLinkTransactionMessage* pollerMsg(new CPollerOneLinkTransactionMessage(msgMeters, transCode, uniqueId, msg->GetBytesData(size), sessionPoints, bonusPoints, residualValue));
	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);
	QueueToPoller(pollerMsg, transCode, uniqueId, transBuffer, (WORD)transLength);
}

void CPollerProtocol::SendJackpotFillToPoller( CInternalMessage * msg  )
{
	LogString(POLLER_LOG, "PollerProtocol: Sending Jackpot/Fill Transaction.");

	byte transType = JACKPOT_FILL_CODE;
	DWORD uniqueId;
	size_t size;

	CMeters *msgMeters = msg->GetTransactionMetersData();
	byte type = (byte)msg->GetWordData();
	__int64 amount = msg->GetInt64Data();
	DWORD machineNumber = msg->GetDWordData();

	uniqueId = GetUniqueId();

	CPollerJackpotFillTransactionMessage *pollerMsg = new CPollerJackpotFillTransactionMessage(msgMeters,
		transType,
		uniqueId,
		msg->GetBytesData(size),
		type,
		amount,
		machineNumber);

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transType, uniqueId, transBuffer, (WORD)transLength);
}

void CPollerProtocol::SendBillAcceptedToPoller( CInternalMessage *msg )
{
	LogString(POLLER_LOG, "PollerProtocol: Sending Bill Accepted Transaction.");

	byte transType = BILL_ACCEPTED_CODE;
	DWORD uniqueId;
	size_t size;

	CMeters *msgMeters = msg->GetTransactionMetersData();

	WORD billType = msg->GetWordData();
	__int64 billCount = msg->GetInt64Data();
	__int64 meterValue = msg->GetInt64Data(true);

	uniqueId = GetUniqueId();

	CPollerBillAcceptedTransactionMessage *pollerMsg = new CPollerBillAcceptedTransactionMessage(msgMeters,
		transType,
		uniqueId,
		msg->GetBytesData(size), 
		billType,
		meterValue,
		billCount);

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transType, uniqueId, transBuffer, (WORD)transLength);
}

void CPollerProtocol::SendConfigChangeNoticeToPoller( CInternalMessage *msg )
{
	byte transType = CONFIG_CHANGE_CODE;
	DWORD uniqueId;
	size_t size;

	CMeters *msgMeters = msg->GetTransactionMetersData();

	DWORD type = msg->GetWordData();
	byte pin = (byte) msg->GetWordData(true);
	__int64 valueBefore = msg->GetInt64Data();
	__int64 valueAfter = msg->GetInt64Data(true);

	//this gets sent after any of the sentinel configuration settings are changed.
	LogString(POLLER_LOG, "PollerProtocol: Sending Config Change Notice Transaction type=%u valueBefore=%lld valueAfter=%lld", type, valueBefore, valueAfter);

	uniqueId = GetUniqueId();

	CPollerConfigChangeTransactionMessage *pollerMsg = new CPollerConfigChangeTransactionMessage(msgMeters,
		transType,
		uniqueId,
		msg->GetBytesData(size), 
		type,
		valueBefore,
		valueAfter,
		pin);

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transType, uniqueId, transBuffer, (WORD)transLength);	
}


void CPollerProtocol::SendJackpotToPoller(  CInternalMessage *msg )
{
	LogString(POLLER_LOG, "PollerProtocol: Sending Jackpot Transaction.");

	byte transType = NONE_SPECIFIED; 
	DWORD uniqueId;
	size_t size;
	bool bSendAdditional116 = false;

	CMeters *msgMeters = msg->GetTransactionMetersData();
	CJackpot * msgJackpot = msg->GetJackpotData();
	uniqueId = GetUniqueId();

	JackpotTransactionType jptType = msgJackpot->GetJackpotTransactionType();
	switch (jptType)
	{
	case JTRANS_COMPOSITE:
		transType = COMPOSITE_JACKPOT_CODE;
		if(msgJackpot->GetJackpotType() == JT_NONE)
		{
			bSendAdditional116 = true;
			LogString(POLLER_LOG, "PollerProtocol: COMPOSITE_JACKPOT_CODE With jackpot type NONE Sending 116.");
		}
		break;
	case JTRANS_PROGRESSIVE_HOPPER_PAID:
		transType = PROGRESSIVE_JP_HOPPER_CODE;
		msgJackpot->SetJackpotType(JT_NONE);
		bSendAdditional116 = true;		
		LogString(POLLER_LOG, "PollerProtocol: JTRANS_PROGRESSIVE_HOPPER_PAID Sending 116.");
		break;
	case JTRANS_PROGRESSIVE_SCANNER_ONLY:
		transType = PROGRESSIVE_JP_SCANNER_CODE;
		msgJackpot->SetJackpotType(JT_NONE);
		LogString(POLLER_LOG, "PollerProtocol: JTRANS_PROGRESSIVE_SCANNER_ONLY Sending 116.");
		break;
	default:
		break;
	}

	CPollerJackpotTransactionMessage *pollerMsg = new CPollerJackpotTransactionMessage(msgMeters,
		transType,
		uniqueId,
		msg->GetBytesData(size),
		msgJackpot->GetJackpotAmount(),
		msgJackpot->GetCoinsPlayed(),
		msgJackpot->GetJackpotType(),
		msgJackpot->GetProgressiveLevel(),
		msg->GetDWordData(), 
		msgJackpot->IsKeyToCreditAvailable());

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	bool stored = QueueToPoller(pollerMsg, transType, uniqueId, transBuffer, (WORD)transLength);

	if (stored)
	{
		CInternalMessage *msg1 = new CInternalMessage(INT_MSG_JACKPOT_STORED);
		SendMessageToInternals(msg1, QUEUE_PRIORITY_NORMAL);
	}

	if(bSendAdditional116)
	{
		uniqueId = GetUniqueId();

		CPollerJackpotTransactionMessage *pollerMsg2 = new CPollerJackpotTransactionMessage(msgMeters,
			PROGRESSIVE_JP_SCANNER_CODE,
			uniqueId,
			msg->GetBytesData(size),
			msgJackpot->GetJackpotAmount(),
			msgJackpot->GetCoinsPlayed(),
			msgJackpot->GetJackpotType(),
			msgJackpot->GetProgressiveLevel(),
			msg->GetDWordData(),
			msgJackpot->IsKeyToCreditAvailable());

		SetMessageHeader(pollerMsg2);
		pollerMsg2->SetReadWriteBuffer();
		byte *transBuffer;
		DWORD transLength;
		transBuffer = pollerMsg2->GetReadWriteBuffer(transLength);

		QueueToPoller(pollerMsg2, transType, uniqueId, transBuffer, (WORD)transLength);

	}
}

void CPollerProtocol::SendFailureCodeToPoller( CInternalMessage * msg )
{
	LogString(POLLER_LOG, "PollerProtocol: Failure Code Transaction.");

	byte transType = FAILURE_CODE;
	DWORD uniqueId;
	size_t size;

	CMeters *msgMeters = msg->GetTransactionMetersData();

	DWORD dwMachineNumber = msg->GetDWordData();
	__int64 nFailureCode = msg->GetInt64Data();

	uniqueId = GetUniqueId();

	CPollerFailureCodeMessage *pollerMsg = new CPollerFailureCodeMessage(msgMeters, transType, uniqueId, msg->GetBytesData(size), dwMachineNumber, nFailureCode );

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transType, uniqueId, transBuffer, (WORD)transLength);
}

void  CPollerProtocol::SendMaintenanceToPoller( CInternalMessage * msg )
{
	LogString(POLLER_LOG, "PollerProtocol: Mechanic Maintenance Transaction.");

	byte transType = MAINTENANCE_CODE;
	DWORD uniqueId;
	size_t size;

	CMeters *msgMeters = msg->GetTransactionMetersData();

	DWORD dwMachineNumber = msg->GetDWordData();
	DWORD dwMaintenanceCode = (DWORD) msg->GetInt64Data();

	uniqueId = GetUniqueId();

	CPollerMechMaintTransactionMessage *pollerMsg = new CPollerMechMaintTransactionMessage(msgMeters, transType, uniqueId, msg->GetBytesData(size), dwMachineNumber, dwMaintenanceCode );

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transType, uniqueId, transBuffer, (WORD)transLength);
}

void CPollerProtocol::SendCommStatusToPoller(  CInternalMessage * msg )
{
	byte transType;

	switch(msg->GetMessageType())
	{
	case INT_MSG_TRANSACTION_SERIAL_COMM_DOWN:
		LogString(POLLER_LOG, "PollerProtocol: Game Comm Down Transaction.");
		transType = GAME_COMM_DOWN_CODE;
		break;
	case INT_MSG_TRANSACTION_SERIAL_COMM_UP:
		LogString(POLLER_LOG, "PollerProtocol: Game Comm Up Transaction.");
		transType = GAME_COMM_UP_CODE;
		break;
	default:
		LogString(POLLER_LOG, "PollerProtocol: INVALID COMM STATUS ");
		transType = NONE_SPECIFIED;
	}

	DWORD uniqueId;
	size_t size;

	CMeters *msgMeters = msg->GetTransactionMetersData();

	uniqueId = GetUniqueId();

	CPollerSerialCommDownTransactionMessage *pollerMsg = new CPollerSerialCommDownTransactionMessage(msgMeters, transType, uniqueId, msg->GetBytesData(size), (byte)msg->GetWordData() );

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transType, uniqueId, transBuffer, (WORD)transLength);
}


void CPollerProtocol::SendTicketTransactionToPoller(CInternalMessage * msg )
{
	LogString(POLLER_LOG, "PollerProtocol: Sending Ticket Transaction.");

	byte transType = TICKET_CODE;
	DWORD uniqueId;
	size_t size;
	CMeters *msgMeters = msg->GetTransactionMetersData();
	CTicket *msgTicket = msg->GetTicketData();

	uniqueId = GetUniqueId();

	if (!m_extensibleFlags.TicketInResend() && msgTicket && (RequestVendAuthorization == msgTicket->ActionCode() || VoucherRejectedByGame == msgTicket->ActionCode()))
	{
		LogString(TICKETING_LOG, "m_listOfUniqueIdsNotToResend insert=%u", uniqueId);
		m_listOfUniqueIdsNotToResend.insert(uniqueId);
	}

	CPollerTicketTransactionMessage *pollerMsg = new CPollerTicketTransactionMessage(msgMeters, msgTicket, transType, uniqueId, msg->GetBytesData(size));
	DWORD status = msg->GetDWordData(true);
	// adjust status, show transaction backlog
	// TBD: how to know backlog exists.
	pollerMsg->SetStatus(status & 0xFFF7FFFF); // clear bit, for now.

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	//Pass Play Session ID
	DWORD psid = msg->GetDWordData(false);
	LogString(POLLER_LOG, "%s: PollerProtocol: Play Session ID received from internal: %u", __FUNCTION__, psid);
	pollerMsg->SetPlaySessionID(psid);
	bool stored = QueueToPoller(pollerMsg, transType, uniqueId, transBuffer, (WORD)transLength, msgTicket && !msgTicket->IsTicketIn());

	//tell internals if we have it stored in NVRAM.
	if (stored && msgTicket)
	{
		CTicket * confirmTicket = new CTicket(*msgTicket);
		CInternalMessage * confirmMsg = new CInternalMessage(INT_MSG_TICKET_STORED, confirmTicket);
		SendMessageToInternals(confirmMsg, QUEUE_PRIORITY_ABOVE_NORMAL);

		if( !msgTicket->IsTicketIn() )
		{
			lock_guard<recursive_mutex> lock(m_criticalSection);
			{
				try
				{
					//tell internals how many pending outs we have
					m_ticketsBufferedCount++;
					CInternalMessage * msgCount = new CInternalMessage(INT_MSG_TICKET_OUT_PENDING_COUNT, (WORD)m_ticketsBufferedCount);
					SendMessageToInternals(msgCount, QUEUE_PRIORITY_NORMAL);
				}
				catch( char * str )
				{
					LogString(ERROR_LOG, "Failed To Send Ticket Pending Size - %s", str);
				}
			}
		}
	}
}

void CPollerProtocol::SendExternalBonusToPoller(CInternalMessage *msg)
{
	LogString(POLLER_LOG, "PollerProtocol: Sending External Bonus Transaction.");

	byte transCode = EXTERNAL_BONUS_CODE;
	DWORD uniqueId;
	size_t size;
	CMeters *msgMeters = msg->GetTransactionMetersData();
	CEGMAward * msgAward = msg->GetEGMAward();
	uniqueId = GetUniqueId();

	CPollerExternalBonusTransaction *pollerMsg = new CPollerExternalBonusTransaction(msgMeters,
		transCode,
		uniqueId,
		msg->GetBytesData(size),
		msgAward->GetAwardAmountPaid());

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transCode, uniqueId, transBuffer, (WORD)transLength);
}

void CPollerProtocol::SetNvramStatus(CInternalMessage *msg)
{
	m_nvramMsgReceived = true;
	m_nvramValid = msg->GetWordData() ? 1 : 0; 
	m_transactionResendList = new CTransactionResendList(m_nvramValid);
	m_uniqueId = m_transactionResendList->GetHighestUniqueId();
	m_ticketsBufferedCount = m_transactionResendList->GetInitialTicketOutCount();
	CInternalMessage * msgCount = new CInternalMessage(INT_MSG_TICKET_OUT_PENDING_COUNT, (WORD)m_ticketsBufferedCount);
	SendMessageToInternals(msgCount, QUEUE_PRIORITY_NORMAL);
}

bool CPollerProtocol::QueueToPoller(
	CPollerMessage*pollerMsg,
	byte msgType,
	DWORD uniqueId,
	byte *transPtr,
	DWORD transLength,									
	bool isTicketOut)
{

	bool retVal = false;

	bool addToResendList(true);

	if (msgType == PERSONAL_BANKER_CODE)
	{
		addToResendList = ((CPollerPBTransactionMessage*)pollerMsg)->IsEligibleToResend();
	}

	if (m_transactionResendList && addToResendList)
	{
		// Add it to the resend queue which will only cause the transaction to resend 30 seconds later
		retVal = m_transactionResendList->AddTransaction(msgType, uniqueId, transPtr, (WORD)transLength, true, isTicketOut);
	}

	// Always add to the poller queue otherwise it will only get sent 30 seconds later
	AddToPollerQueue(pollerMsg, QUEUE_PRIORITY_NORMAL );

	CheckResendUse();

	return retVal;
}

void CPollerProtocol::CheckResendUse()
{
	//check to see if the resend list is filling up

	if(m_transactionResendList != NULL)
	{
		int percentage = m_transactionResendList->GetPercentageFull();
		if ( percentage > RESEND_LIST_FULL_THRESHOLD )
		{
			if (m_resendState != RESEND_FULL)
			{
				m_resendState = RESEND_FULL;
				CInternalMessage *msg = new CInternalMessage(INT_MSG_POLLER_LIST_FULL);
				SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);
			}
		}
		else if( percentage > RESEND_LIST_FILLING_THESHOLD )
		{
			if (m_resendState != RESEND_FILLING)
			{
				m_resendState = RESEND_FILLING;
				CInternalMessage *msg = new CInternalMessage(INT_MSG_POLLER_LIST_WARNING);
				SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);
			}
		}
		else if ( (percentage < RESEND_LIST_FILLING_THESHOLD) && (m_resendState != RESEND_OK) )
		{
			m_resendState = RESEND_OK;
			CInternalMessage *msg = new CInternalMessage(INT_MSG_POLLER_LIST_OK);
			SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);
		}
	}
}

void CPollerProtocol::SetOneLinkProgressivesSenOptionSupport(WORD oldPacketVersion, WORD packetVersion)
{
	bool supported = packetVersion >= PACKET_VERSION_ONE_LINK_PROGRESIVES_SEN_OPTION;
	if (m_oneLinkProgressivesSenOptionSupported != supported ||
		oldPacketVersion != packetVersion)
	{
		m_oneLinkProgressivesSenOptionSupported = supported;
		SendOneLinkProgressivesSenOptionSupportToInternals();
	}
}

void CPollerProtocol::SendOneLinkProgressivesSenOptionSupportToInternals()
{
	LogString(POLLER_LOG, "SendOneLinkProgressivesSenOptionSupportToInternals");

	CInternalMessage *msg(
		new CInternalMessage(INT_MSG_SET_ONE_LINK_PROGRESSIVES_SEN_OPTION_SUPPORT, (WORD)m_oneLinkProgressivesSenOptionSupported));
	msg->SetData((DWORD)m_pollerPacketVersion);
	SendToInternals(msg, QUEUE_PRIORITY_HIGHEST);
}

// Don't send a tilt request to Internals for a problem processing a tilt transation, to avoid repetiveness,
// in case there is an issue processing tilt transactions.
// Don't send tltUnknown. For this method, tltUnknown is used to indicate no tilt.
void CPollerProtocol::SendPacketTiltToInternals(const BYTE *packet, unsigned long packetLength, TiltCode tiltCode)
{
	if (tiltCode != tltUnknown)
	{
		bool packetIsTiltTransaction(false);
		if (packetLength > TRANSACTION_CODE_OFFSET)
		{
			WORD messageId;
			memcpy(&messageId, packet + MESSAGE_ID_OFFSET, sizeof(messageId));
			BYTE transactionCode;
			memcpy(&transactionCode, packet + TRANSACTION_CODE_OFFSET, sizeof(transactionCode));
			if (POLLER_MSG_TRANSACTION == messageId)
			{
				if (COMPOSITE_TILT == transactionCode)
				{
					packetIsTiltTransaction = true;
				}

				if (packetLength > UNIQUE_ID_OFFSET)
				{
					DWORD uniqueId(*(DWORD *)(packet + UNIQUE_ID_OFFSET));
					m_transactionResendList->SetSuspicious(uniqueId);
				}
			}
		}

		if (!packetIsTiltTransaction)
		{
			// If the bad packet was not a tilt transaction then send a request to Internals to generate a tilt transaction.
			SendToInternals(new CInternalMessage(INT_MSG_SEND_TILT, (WORD)tiltCode), QUEUE_PRIORITY_NORMAL);
		}
	}
}

void CPollerProtocol::ProcessPlayerBonusEventsFromPoller(CPollerPlayerBonusEventsMessage *pollerMsg)
{
	// get bonus events data, send to internals
	CBonusEvents *events = new CBonusEvents(pollerMsg->GetBonusEvents());

	CInternalMessage *msg = new CInternalMessage(INT_MSG_PLAYER_BONUS_EVENTS, events);

	SendToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::ProcessPlayerUpdateEventsFromPoller(CPollerPlayerUpdateEventsMessage *pollerMsg)
{
	// Send INT_MSG_PLAYER_UPDATE_TIME for each time in pollerMsg
	byte eventCount = pollerMsg->GetEventCount();
	time_t *eventTime;

	for (int i = 0; i < eventCount; i++)
	{
		eventTime = new time_t(pollerMsg->GetEventTime(i));

		CInternalMessage *msg = new CInternalMessage(INT_MSG_PLAYER_UPDATE_TIME, eventTime);

		SendToInternals(msg, QUEUE_PRIORITY_NORMAL);
	}
}

bool CPollerProtocol::IsPmtTransactionsUp()
{
	return m_pmtTransactionsUp;
}

void CPollerProtocol::SendAuthorizePinToPoller(CInternalMessage *msg)
{

	byte transCode = AUTHORIZE_PIN;
	DWORD uniqueId;
	size_t size;
	CMeters *msgMeters = msg->GetTransactionMetersData();
	uniqueId = GetUniqueId();
	WORD currentPBPin = msg->GetWordData();
	LogString(PBT_LOG, "PollerProtocol: Sending Authorize Pin Transaction.");

	CPollerAuthorizePINTransactionMessage *pollerMsg = new CPollerAuthorizePINTransactionMessage(msgMeters,
		transCode,
		uniqueId,
		msg->GetBytesData(size),
		currentPBPin);

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transCode, uniqueId, transBuffer, (WORD)transLength);

}

void CPollerProtocol::SendUJPResult(CInternalMessage *msg)
{
	LogString(POLLER_LOG, "PollerProtocol: Sending Unattended jackpot result.");

	byte transCode = UNATTENDED_JACKPOT_RESULT;
	DWORD uniqueId;
	size_t size;
	CMeters *msgMeters = msg->GetTransactionMetersData();
	uniqueId = GetUniqueId();

	CJackpot *jackpot = msg->GetJackpotData();
	byte jackpotStatus = (byte)msg->GetWordData();

	CPollerUJPResultTransactionMessage *pollerMsg = new CPollerUJPResultTransactionMessage(msgMeters,
		transCode,
		uniqueId,
		msg->GetBytesData(size),
		jackpot->GetJackpotID(),
		(UnattendedJackpotStatus::Type) jackpotStatus,
		jackpot->GetJackpotAmount());

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength(0);
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);
	
	QueueToPoller(pollerMsg, transCode, uniqueId, transBuffer, (WORD)transLength);
}

void CPollerProtocol::LowerTimeoutsForShutdown()
{
	m_pollerSocketReadTimeout = POLLER_CHEKIN_TIME_DURING_SHUTDOWN;
}

bool CPollerProtocol::VoucherSeparationEnabled(void)
{
    return m_voucherSeparationEnabled;
}

void CPollerProtocol::SetVoucherSeparationEnabled( bool voucherSeparationEnabled )
{
    m_voucherSeparationEnabled = voucherSeparationEnabled;
}

bool CPollerProtocol::MGMDEnabled(void)
{
	return m_MGMDfeatureFlag;
}

void CPollerProtocol::SetMGMDEnabled(bool MGMDEnabled)
{
	m_MGMDfeatureFlag = MGMDEnabled;
}

void CPollerProtocol::SetChunkedGamesListEnabled(bool isChunkedGamesListEnabled)
{
	CPollerMGMDData::Instance().SetChunkedGamesListFlag(isChunkedGamesListEnabled);
}

bool CPollerProtocol::IsTranstoSendToPollarIfPMTDown(byte * resendItem)
{
     bool bTranSend = (VoucherSeparationEnabled() && !(CTransactionItem::IsMessagePMTDependent(resendItem)));
	 return (bTranSend);
}

bool  CPollerProtocol::IsTranstoSendToPollarIfPMTDown(CPollerMessage *pollerMsg)
{
     bool bTranSend = (VoucherSeparationEnabled() && !(pollerMsg->IsPMTDependent()));
	 return (bTranSend);
}

bool  CPollerProtocol::ProcessFeatureFlag(CInternalMessage *intMag)
{
	DWORD m_FeatureFlagRaw = intMag->GetDWordData();
	m_voucherSeparationEnabled = (m_FeatureFlagRaw & FF_VOUCHER_SEPARATION) == FF_VOUCHER_SEPARATION;
	if (m_voucherSeparationEnabled)
		LogString(POLLER_LOG, "Voucher separation flag enabled");
	m_MGMDfeatureFlag = (m_FeatureFlagRaw & FF_MULTI_GAME_MULTI_DENOM) == FF_MULTI_GAME_MULTI_DENOM;
	if (m_MGMDfeatureFlag)
		LogString(POLLER_LOG, "MGMD flag enabled");
	return m_voucherSeparationEnabled | m_MGMDfeatureFlag;
}

static void SendResendItems(CPollerProtocol *pollerProtocol, bool forceSend)
{
	byte *resendItem;
	WORD itemLength = 0;
	DWORD uniqueId = 0;
	DWORD lastSent;
	bool alreadySuspicious(false);

	resendItem = pollerProtocol->GetFirstResendItem(itemLength, uniqueId, lastSent, alreadySuspicious);
	while( resendItem != NULL && !pollerProtocol->IsTimeToClose())
	{
		//Transactions will sent to pollar if PMT is up or in case PMT is down, then only ticket transactions are sent to pollar if 
		//voucher separation featur enabled on pollar client	
		if (pollerProtocol->IsPmtTransactionsUp() || pollerProtocol->IsTranstoSendToPollarIfPMTDown(resendItem))
		{
			//Only send to the poller if we havent sent it in the last 30 seconds
			//or if we are forcing them to be sent again (when the poller is back up)
			if (forceSend || ((abs( int(CUtilities::GetTickCount() - lastSent ))) > (int)(RESEND_TRANSACTION_INTERVAL) && itemLength > 0) )
			{
				if (pollerProtocol->ValidateBadTransactionResendCount(uniqueId))
				{
					if (!pollerProtocol->RemoveResendListItemIfNecessary(uniqueId))
					{
						LogData(POLLERDATA_LOG, DATA_WRITE | DATA_ETHERNET, resendItem, itemLength);

						pollerProtocol->SetLastSent(uniqueId);
						pollerProtocol->SendToPoller(resendItem, itemLength, uniqueId, alreadySuspicious);
					}
				}
				else
				{
					LogString(POLLER_LOG, "[uniqueId = %d] Bad packet was removed after %d send attempts.", uniqueId, MAX_BAD_TX_RESEND_COUNT + 1);
				}
			}
		}

		CUtilities::Sleep(0);

		// Get the next resend item, if any.
		resendItem = pollerProtocol->GetNextResendItem(itemLength, uniqueId, lastSent, alreadySuspicious);
		CWatchDog::Instance().Checkin();

	}
}

static void SendToPollerThreadInner(CPollerProtocol *pollerProtocol, CPollerMessage *msg)
{
    try
	{
		pollerProtocol->SendToPoller( msg );
	}
    catch (...) 
    {
		CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_SEND_TO_POLLER);
    }
}

static UINT AFX_CDECL SendToPollerThread(PVOID threadParam)
{
	CPollerProtocol *pollerProtocol = (CPollerProtocol *)threadParam;

	CUtilities::SetThreadName(CUtilities::GetCurrentThreadId(), THREAD_SEND_TO_POLLER);

	// Get the queue of messages from Internals
	CQueueList *queueList = pollerProtocol->GetPollerQueue();

	// Get the handle to the event that gets called when an item is put in the queue
	binsem &itemAddedEventHandle = queueList->GetAddedItemEventHandle();
	bool previousOnlineState = false;
	bool goneOnline = false;

	while ( !pollerProtocol->IsTimeToClose() )
	{
		// Wait for an item to be added to the queue or resend check time expired.
		itemAddedEventHandle.try_wait_for(RESEND_CHECK_INTERVAL );

		if (pollerProtocol->IsTimeToClose())
			break;
				
		CWatchDog::Instance().Checkin();

		// This will try to connect if we are not connected.
		//This could take up to 35 seconds
		pollerProtocol->CheckForConnect();

		CWatchDog::Instance().Checkin();

		// did we just connect?
		goneOnline = (previousOnlineState != pollerProtocol->IsConnected()) && pollerProtocol->IsConnected();
		previousOnlineState = pollerProtocol->IsConnected();

		if (goneOnline)
		{
			// If we just reconnected then we need to send everything in the resend list now
			// rather than after it times out, this so that transactions do not get out of order
			SendResendItems(pollerProtocol, true);
		}

		// Send any transactions in the resend list that have been sitting for more than 30 seconds
		SendResendItems(pollerProtocol);

		// Next get all of the queue items and send them.
		CPollerMessage *msg = (CPollerMessage *)queueList->Remove(); 

		//SS Tracker #25783
		while ( msg != NULL && !pollerProtocol->IsTimeToClose() )
		{
			LogString(POLLER_LOG, "PollerProtocol::SendToPollerThread");
			// Give this message to internals to process
			
			SendToPollerThreadInner(pollerProtocol, msg);

			// Just in case this loops for a long time, let other threads have a go.
			CUtilities::Sleep(0);

			// Get the next message (if any)
			msg = (CPollerMessage *)queueList->Remove(); 
			CWatchDog::Instance().Checkin();
		}

		CUtilities::Sleep(0);
		CWatchDog::Instance().Checkin();
	}

	CWatchDog::Instance().DeregisterThread();

	return 0;
}

static UINT AFX_CDECL ReadFromPollerThread( PVOID threadParam )
{
	CPollerProtocol *pollerProtocol = (CPollerProtocol *)threadParam;

	CUtilities::SetThreadName(CUtilities::GetCurrentThreadId(), THREAD_READ_FROM_POLLER);

	while ( !pollerProtocol->IsTimeToClose() )
	{
		try
		{
			pollerProtocol->ReadAndProcessFromPoller();
		}
        catch (...) 
        {
			CRestartManager::ReportThreadExceptionAndRequestAReboot(THREAD_READ_FROM_POLLER);
        }

		CUtilities::Sleep(0);
	}


	CWatchDog::Instance().DeregisterThread();

	return 0;
}

void CPollerProtocol::SendSubGameSelection(CInternalMessage *msg)
{
	CPlayer *player = msg->GetPlayerData();
	SubgameSessionParameters *params = msg->GetSubgameSessionParameters();

	DWORD playerID = 0;
	bool cardedPlayer = false;
	const CCard *card = nullptr;
	const byte *cardID = nullptr;
	int cardLen = 0;
	if (player != nullptr)
	{
		playerID = player->GetPlayerInfo()->GetPlayerId();
		card = player->GetCard();
		if (card != nullptr)
		{
			cardID = card->GetCardID(cardLen);
		}
		cardedPlayer = true;
	}
	else
	{
		playerID = 0;

	}

	bool mgmdAvailable = (bool)msg->GetWordData();

	// Send message to poller
	CPollerSubgameSelectionMessage *pollerMsg = new CPollerSubgameSelectionMessage(params->m_gameSessionSequenceNumber, cardedPlayer, cardID, cardLen, playerID, params->m_playSessionID, params->m_gameNumber, params->m_denom, mgmdAvailable);
	SetMessageHeader(pollerMsg);
	AddToPollerQueue( pollerMsg, QUEUE_PRIORITY_NORMAL );
}

void CPollerProtocol::ProcessSubgameSelectionResponseFromPoller(CPollerSubgameSelectionResponseMessage *pollerMsg)
{
	CInternalMessage *msg = new CInternalMessage(INT_MSG_SUBGAME_SELECTION, pollerMsg->GetSubgameSelectionResponse());
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::ProcessRebootRequestFromPoller(CPollerRebootRequestMessage *pollerMsg)
{
	CInternalMessage *msg = new CInternalMessage(INT_MSG_REMOTE_REBOOT, (WORD)pollerMsg->GetRebootRequestType());
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::SendGameListToPoller()
{
	LogString(POLLER_LOG, "%s: %d", __FUNCTION__, __LINE__);
	
	CParseGameList* parseGameList = m_pollergameListInfo->GetParseList();
	WORD gamesInCurrentMessage	= m_pollergameListInfo->GetGamesInCurrentMessage();

	if (MGMDEnabled() && (parseGameList != nullptr) && (gamesInCurrentMessage > 0))
	{
		WORD gameListChunkSize		= CPollerMGMDData::Instance().GetGameListChunkSizeOption(); 
		WORD totalSubgames			= m_pollergameListInfo->GetTotalSubgames();
		WORD numberOfChunks 		= m_pollergameListInfo->GetNumberOfChunks();
		WORD gameListChunkoffset	= m_pollergameListInfo->GetSubgamesSent(); //denotes how many games already sent
		time_t currentTime			= m_pollergameListInfo->GetCurrentTime();
		WORD lastSubgameIndexInCurrentChunk	= m_pollergameListInfo->GetLastSubgameIndexInCurrentChunk();
		WORD nextChunkIndex			= m_pollergameListInfo->GetNextGameListChunkIndex();
		WORD chunksToSendNow		= 1; //Assuming there will be only one chunk to be send. But it could be more as calculated below.
	
		//It might be that there could be more subgames than the chunksize
		if (gamesInCurrentMessage > gameListChunkSize)
			chunksToSendNow = ceil((float)gamesInCurrentMessage / gameListChunkSize);
		
		LogString(POLLER_LOG, "%s: %d chunksToSendNow - %u gameListChunkoffset - %u currentTime - %s lastSubgameIndexInCurrentChunk - %u gamesInCurrentMessage - %u",
			__FUNCTION__, __LINE__, chunksToSendNow, gameListChunkoffset, CUtilities::FormatDateTimeString( currentTime, "%m-%d-%Y-%H-%M-%S" ).c_str(), lastSubgameIndexInCurrentChunk, gamesInCurrentMessage);

		WORD oldGameListChunkoffset = gameListChunkoffset;
		for (WORD index = 1; index < chunksToSendNow; index++)
		{
			PrepareAndSendGameListChunksMessage(parseGameList, currentTime, nextChunkIndex, gameListChunkoffset, numberOfChunks, gameListChunkSize, totalSubgames);
			gameListChunkoffset = oldGameListChunkoffset + (index * gameListChunkSize); //denotes how many games already sent
			nextChunkIndex++;
			LogString(POLLER_LOG, "%s: %d gameListChunkoffset: %u", __FUNCTION__, __LINE__, gameListChunkoffset);
			CUtilities::Sleep(DELAY_GAME_LIST_SEND_CHUNKS);
			CWatchDog::Instance().Checkin();
		}

		if (gameListChunkoffset < lastSubgameIndexInCurrentChunk)
		{		
			WORD gamesInLastChunk = lastSubgameIndexInCurrentChunk - gameListChunkoffset;
			PrepareAndSendGameListChunksMessage(parseGameList, currentTime, nextChunkIndex, gameListChunkoffset, numberOfChunks, gamesInLastChunk, totalSubgames);
			nextChunkIndex++;
		}
		
		m_pollergameListInfo->SetNextGameListChunkIndex(nextChunkIndex);
		m_pollergameListInfo->UpdateSubgamesSent(gamesInCurrentMessage);

		if (m_pollergameListInfo->GetSubgamesSent() == m_pollergameListInfo->GetTotalSubgames())
			m_pollergameListInfo->ClearData();
	}
}


void CPollerProtocol::PrepareAndSendGameListChunksMessage(CParseGameList* parseGameList, time_t& currentTime, WORD chunkIndex, WORD gameListChunkoffset, WORD numberOfChunks, WORD gamesInChunk, WORD totalSubgames )
{	
	if(parseGameList)
	{
		DWORD transLength = 0;
		LogString(POLLER_LOG, "%s: %d: Sending Game list Chunk message (322) to Poller (%u of %u chunks with %u subgames)", 
			__FUNCTION__, __LINE__,  chunkIndex, numberOfChunks, gamesInChunk);

		CPollerGamesListMessage *pollerGameList = new CPollerGamesListMessage(parseGameList, currentTime, m_pollerPacketVersion, totalSubgames, chunkIndex, gameListChunkoffset, numberOfChunks, gamesInChunk);
		SetMessageHeader(pollerGameList);
		pollerGameList->GetReadWriteBuffer(transLength);
		AddToPollerQueue(pollerGameList, QUEUE_PRIORITY_NORMAL);
		
		//Send to internals to update on diagnostic web page		
		CInternalMessage *msg(new CInternalMessage(INT_MSG_GAME_LIST_CHUNKS_SENT, chunkIndex));
		msg->SetData((DWORD)numberOfChunks);
		SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
	}
	else{
		LogString(POLLER_LOG, "%s: %d: Invalid parseGameList", __FUNCTION__, __LINE__);
	}
}

void CPollerProtocol::SendPBResetPinTransactionMessageToPoller( CInternalMessage * msg )
{
	byte transCode = PBT_RESET_PIN;
	DWORD uniqueId;
	size_t size;
	CMeters *msgMeters = msg->GetTransactionMetersData();
	uniqueId = GetUniqueId();
	WORD newPin = msg->GetWordData();
	DWORD currentPBPin = msg->GetDWordData();
	
	LogString(PBT_LOG, "%s: %d", __FUNCTION__, __LINE__);

	CPollerPBResetPinTransactionMessage *pollerMsg = new CPollerPBResetPinTransactionMessage(msgMeters,
		transCode,
		uniqueId,
		msg->GetBytesData(size),
		currentPBPin,
		newPin);

	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transCode, uniqueId, transBuffer, (WORD)transLength);

}

void CPollerProtocol::ProcessPBPinResetResponseFromPoller(CPollerPBPinResetResponseMessage *pollerMsg)
{
	CInternalMessage *msg = new CInternalMessage(INT_MSG_PB_PIN_RESET_RESULT, (WORD)pollerMsg->GetPinResetResult());
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::ProcessProximityBeaconScanListResponseFromPoller(CPollerProximityBeaconScanListResponseMessage *pollerMsg)
{
	vector<string>* UUIDList = new vector<string>(pollerMsg->GetUUIDList());
	CInternalMessage *msg = new CInternalMessage(INT_MSG_PROXIMITY_BEACON_SCAN_LIST_RESPONSE, UUIDList);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::ProcessMCResponseFromPoller(CPollerMobileConnectResponseMessage *pollerMsg)
{
	// Call MobileProtocol Factory functions here
	string &transBuffer = pollerMsg->GetJson();
	LogString(POLLER_LOG, "PollerProtocol:: ProcessMCResponseFromPoller '%s'.", CUtilities::MaskPin(transBuffer).c_str());

	std::unique_lock<std::recursive_mutex> omCS(m_criticalSection);
	try
	{
		rapidjson::Document jsonDoc;
		if (jsonDoc.ParseInsitu((char*)transBuffer.c_str()).HasParseError())
		{
			LogString(POLLER_LOG, "ERROR: ProcessMCResponseFromPoller parse error: Error: %s",
				JsonException::GetParseErrorString(jsonDoc.GetParseError()));
		}
		else
		{
			if (jsonDoc.HasMember(pFKeyMessageType))
			{
				string messageType = jsonDoc[pFKeyMessageType].GetString();
				LogString(POLLER_LOG, "ProcessMCResponseFromPoller: Type: %s", messageType.c_str());
				FromPlayerDeviceService::Type type = FromPlayerDeviceService::emap[messageType.c_str()];

				switch (type)
				{
				case FromPlayerDeviceService::EventPairingSettings:
					ProcessMCEventPairingSettings(jsonDoc);
					break;
				case FromPlayerDeviceService::EventPairing:
					ProcessMCEventPairing(jsonDoc);
					break;
				case FromPlayerDeviceService::RequestCardChange:
					ProcessMCRequestCardChange(jsonDoc);
					break;
				case FromPlayerDeviceService::RequestFundsTransferBalance:
					ProcessMCRequestBalance(jsonDoc);
					break;
				case FromPlayerDeviceService::RequestFundsTransfer:
					ProcessMCRequestFundsTransfer(jsonDoc);
					break;
				case FromPlayerDeviceService::RequestEndPBTSession:
					ProcessMCEndPBTSession();
					break;
				// Proximity related messages
				case FromPlayerDeviceService::EventProximitySettings:
					ProcessEventProximitySettings(jsonDoc);
					break;
				default:
					LogString(POLLER_LOG, "ProcessMCResponseFromPoller - N/A: Type: %s", messageType.c_str());
					break;
				}
			}
			else
			{
				LogString(ERROR_LOG, "ERROR: ProcessMCResponseFromPoller JSON missing: %s", pFKeyMessageType);
			}
		}
	}
	catch (JsonException& e)
	{
		LogString(ERROR_LOG, "ERROR: onMessage JsonException: %s: '%s'", e.codestring(), e.what());
		LogString(POLLER_LOG, "ERROR: onMessage JsonException: %s: '%s'", e.codestring(), e.what());
	}
	catch (EnumMap<FromPlayerDeviceService::Type>::Exception& e)
	{
		LogString(ERROR_LOG, "ERROR: onMessage EnumMap<FromOneLink::Type>::Exception (unhandled message type): '%s'", e.what());
		LogString(POLLER_LOG, "ERROR: onMessage EnumMap<FromOneLink::Type>::Exception (unhandled message type): '%s'", e.what());
	}
	catch (...)
	{
		LogString(POLLER_LOG, "ERROR: onMessage unhandled exception");
	}
}

void CPollerProtocol::ProcessMCEventPairingSettings(const rapidjson::Document& jsonDoc)
{
    LogString(POLLER_LOG, "%s %d - Creating new internal message INT_MSG_PAIRING_SETTINGS_EVENT", __FUNCTION__, __LINE__);
	CPairingSettingsEvent *pairingSettingsEventData = new CPairingSettingsEvent(jsonDoc);
    CInternalMessage *msg = new CInternalMessage(INT_MSG_PAIRING_SETTINGS_EVENT, pairingSettingsEventData);
    SendToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::ProcessMCEventPairing(const rapidjson::Document &jsonDoc)
{
    LogString(POLLER_LOG, "%s %d - Creating new internal message INT_MSG_PAIRING_EVENT", __FUNCTION__, __LINE__);
	CPairingEvent *pairingEventData = new CPairingEvent(jsonDoc);
    CInternalMessage *msg = new CInternalMessage(INT_MSG_PAIRING_EVENT, pairingEventData);
    SendToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::ProcessMCRequestCardChange(const rapidjson::Document &jsonDoc)
{
    LogString(POLLER_LOG, "%s %d - Creating new internal message INT_MSG_CARD_CHANGE_REQUEST", __FUNCTION__, __LINE__);
    CCardChangeRequest *pCardChangeRequest = new CCardChangeRequest(jsonDoc);
    CInternalMessage *msg = new CInternalMessage(INT_MSG_CARD_CHANGE_REQUEST, pCardChangeRequest);
    SendToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::ProcessMCRequestBalance(const rapidjson::Document &jsonDoc)
{
	LogString(POLLER_LOG, "%s %d - Creating new internal message INT_MSG_CREDIT_TOTALS_REQUEST", __FUNCTION__, __LINE__);
    CFundsTransferBalanceRequest *pFundsTransferBalanceRequest = new CFundsTransferBalanceRequest(jsonDoc);
    CInternalMessage *msg = new CInternalMessage(INT_MSG_CREDIT_TOTALS_REQUEST, pFundsTransferBalanceRequest);
    SendToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::ProcessMCRequestFundsTransfer(const rapidjson::Document &jsonDoc)
{
	LogString(POLLER_LOG, "%s %d - Creating new internal message INT_MSG_FUNDS_TRANSFER_REQUEST", __FUNCTION__, __LINE__);
    CFundsTransferRequest *pFundsTransferRequest = new CFundsTransferRequest(jsonDoc);
    CInternalMessage *msg = new CInternalMessage(INT_MSG_FUNDS_TRANSFER_REQUEST, pFundsTransferRequest);
    SendToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::ProcessMCEndPBTSession()
{
	LogString(POLLER_LOG, "%s %d - Creating new internal message INT_MSG_PB_CANCEL_SESSION", __FUNCTION__, __LINE__);
	CInternalMessage *msg = new CInternalMessage(INT_MSG_PB_CANCEL_SESSION);
	SendToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::ProcessEventProximitySettings(const rapidjson::Document &jsonDoc)
{
	m_proximitySettingsResponseReceived = true;

	LogString(POLLER_LOG, "%s %d - Creating new internal message INT_MSG_PROXIMITY_SETTINGS_EVENT", __FUNCTION__, __LINE__);
	CProximitySettingsEvent *proximitySettingsEventData = new CProximitySettingsEvent(jsonDoc);
    CInternalMessage *msg = new CInternalMessage(INT_MSG_PROXIMITY_SETTINGS_EVENT, proximitySettingsEventData);
    SendToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CPollerProtocol::SendGenericTransactionToPoller(CInternalMessage *msg)
{
	byte transCode((byte)msg->GetWordData());
	int64_t sessionPoints(msg->GetInt64Data()), residualValue(msg->GetInt64Data(true)), bonusPoints(0);
	LogString(POLLER_LOG, _T("PollerProtocol: SendGenericTransactionToPoller transCode=%d. sessionPoints=%lld residualValue=%lld bonusPoints=%lld"),
		transCode, sessionPoints, residualValue, bonusPoints);
	size_t size;
	CMeters *msgMeters = msg->GetTransactionMetersData();
	DWORD uniqueId(GetUniqueId());
	CPollerGenericTransactionMessage* pollerMsg(new CPollerGenericTransactionMessage(msgMeters, transCode, uniqueId, msg->GetBytesData(size), sessionPoints, residualValue, bonusPoints));
	SetMessageHeader(pollerMsg);
	pollerMsg->SetReadWriteBuffer();
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);
	QueueToPoller(pollerMsg, transCode, uniqueId, transBuffer, (WORD)transLength);
}

void CPollerProtocol::ProcessPollerIPFileIfExist()
{
	// path to poller IP file
	std::string strPollerIPPath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + "/PollerIP.txt";

	if (CUtilities::FileExists(strPollerIPPath))
	{
		LogString(FLASHING, "Found PollerIP file, path = %s", strPollerIPPath.c_str());
		ifstream file(strPollerIPPath);
		if (file.good())
		{
			LogString(FLASHING, "File is good");
			// read line
			std::string strLine;
			std::getline(file, strLine);

			// trim unwanted spaces
			strLine.erase(0, strLine.find_first_not_of(" \n\r\t"));
			strLine.erase(strLine.find_last_not_of(" \n\r\t") + 1);

			if (strLine.size())
			{
				// compare the new IP wth current one.
				if (m_connection->GetHost() != strLine)
				{
					LogString(FLASHING, "New Poller IP = %s", strLine.c_str());

					// send new poller IP to internals to persist it.
					CInternalMessage *msg = new CInternalMessage(INT_MSG_SET_POLLER_IP, strLine);
					SendMessageToInternals(msg, QUEUE_PRIORITY_HIGHEST);

					// update the poller IP for reconnection.
					m_NewPollerIP = strLine;
				}
				else
				{
					LogString(FLASHING, "New Poller IP is same as current Poller IP (%s)", strLine.c_str());
				}
			}
		}
		file.close();

		LogString(FLASHING, "Deleting file %s", strPollerIPPath.c_str());
		CUtilities::DeleteFile(strPollerIPPath.c_str());
	}
}

void CPollerProtocol::SendCardlessTransactionToPoller(CInternalMessage *msg)
{
	byte transCode = NONE_SPECIFIED;
	switch (msg->GetMessageType())
	{
	case INT_MSG_TRANSACTION_CARDLESS_SESSION_START:
		LogString(POLLER_LOG, "PollerProtocol: Sending Cardless Session Start.");
		transCode = CARDLESS_SESSION_START_CODE;
		break;

	case INT_MSG_TRANSACTION_CARDLESS_SESSION_END:
		LogString(POLLER_LOG, "PollerProtocol: Sending Cardless Session End.");
		transCode = CARDLESS_SESSION_END_CODE;
		break;

	default:
		// We are not expected to get here
		LogString(POLLER_LOG, "PollerProtocol: INVALID CARDLESS TRANSACTION %d.",
			msg->GetMessageType());
		LogString(ERROR_LOG, "PollerProtocol: INVALID CARDLESS TRANSACTION. %d.",
			msg->GetMessageType());
		break;
	}

	DWORD uniqueId = GetUniqueId();
	DWORD ptsn = (DWORD)msg->GetInt64Data();
	DWORD psid = (DWORD)msg->GetInt64Data(true);
	int gameNumber = msg->GetIntData();
	int gameDenom = msg->GetIntData(true);
	bool mgmdGlobal = (bool)msg->GetWordData();
	bool mgmdAvailable = (bool)msg->GetWordData(true);

	CMeters *msgMeters = msg->GetTransactionMetersData();

	CPollerCardlessSessionTransactionMessage *pollerMsg = new 
		CPollerCardlessSessionTransactionMessage(
			msgMeters, transCode, uniqueId,
			psid, ptsn, gameNumber,	gameDenom,
			mgmdGlobal,	mgmdAvailable);

	DWORD status = msg->GetDWordData(true);
	// adjust status, show transaction backlog
	pollerMsg->SetStatus(status & 0xFFF7FFFF); // clear bit, for now.

	SetMessageHeader(pollerMsg);
	byte *transBuffer;
	DWORD transLength;
	transBuffer = pollerMsg->GetReadWriteBuffer(transLength);

	QueueToPoller(pollerMsg, transCode, uniqueId, transBuffer, (WORD)transLength);
}

bool CPollerProtocol::IsNFDPoller()
{
	return (m_pollerPacketVersion == 0) ? m_defaultIsPollerNFD :
		(m_pollerPacketVersion >= PACKET_VERSION_NEW_FLOOR_DESIGN);
}

const CPairingInformation& CPollerProtocol::GetMobilePairingInformation() const
{
	return m_mobilePairingInfo;
}
