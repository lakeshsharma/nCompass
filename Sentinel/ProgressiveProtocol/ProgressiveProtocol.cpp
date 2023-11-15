#include "stdafx.h"
#include "ProgressiveProtocol.h"
#include "ProgressiveConfigFile.h"
#include "Logging/LogString.h"
#include "Sentinel.h"
#include <Payout.h>

using namespace std;

const LPCSTR PALTRONICS_CERTIFICATE = "/Paltronics.crt";
const int JACKPOTHITSTARVETICKS = 10000;

CProgressiveProtocol::CProgressiveProtocol(CQueueList &scfToInternalsQueue,
	const std::string &firmwareVersion,
	IOperatingSystem &operatingSystem)
	:
	CSystemProtocol(scfToInternalsQueue,
		_T(THREAD_PROGRESSIVE_INTERNALS_QUEUE),
		INTPROG_MessageQueueLogIdentifier,
		TIMED_EVENTS_TIMEOUT,
		operatingSystem),
	m_configFileActive(false),
	m_oneLinkProgressivesSenOptionEnabled(false),
	m_oneLinkProgressivesSupportedByEGM(false),
	m_firmwareVersion(firmwareVersion)
{
	m_progressiveConnection = NULL;
}

CProgressiveProtocol::~CProgressiveProtocol()
{
	Stop();

	if (m_progressiveConnection != NULL)
	{	
		delete m_progressiveConnection;
	}
}

void CProgressiveProtocol::Init()
{
	CSystemProtocol::Init();
	RefreshConnection();
}

void CProgressiveProtocol::SendMsgToInternals(CInternalMessage* pMsg, int priority)
{
	SendMessageToInternals(pMsg, priority);
}

void CProgressiveProtocol::QueueToOneLink(ProgressiveQueueToOneLink* pMsg, int priority)
{
	GetInternalsQueue()->Add(pMsg, priority);
}

void CProgressiveProtocol::ProcessMessageFromInternals(CInternalMessage *msg)
{
	if (msg != nullptr)
	{
		LogString(PROGRESSIVE, "ProcessMessageFromInternals: %s", InternalMsgTypeStr[msg->GetMessageType()]);
		switch (msg->GetMessageType())
		{
			case INT_MSG_SET_PROGRESSIVE_GAMES_INFO:
				ProcessSetGamesInfo(msg);
				break;
			case INT_MSG_PROGRESSIVE_EXTERNAL_COMMUNICATIONS_STATUS:
				ProcessExternalCommunicationsStatus(msg);
				break;
			case INT_MSG_SET_CONNECTION_DATA:
				ProcessSetConnectionFromInternals(msg);
				break;
			case INT_MSG_SET_ONE_LINK_PROGRESSIVES_SUPPORT:
				ProcessSetOneLinkProgressivesSupportFromInternals(msg);
				break;
			case INT_MSG_SET_PROGRESSIVE_CONFIG:
				ProcessSetProgressiveConfigFromInternals(msg);
				break;
			case INT_MSG_DEFAULT_DATA_DIRECTORY_UPDATED:
				ProcessDefaultDataDirectoryUpdated(msg);
				break;
			case INT_MSG_NOTIFY_GAME_START:
				ProcessGameStartNotification(msg);
				break;
			case INT_MSG_NOTIFY_GAME_END:
				ProcessGameEndNotification(msg);
				break;
			case INT_MSG_GAME_DATA:
				ProcessGameData(msg);
				break;
			case INT_MSG_PLAYER_DATA:
				ProcessPlayerData(msg);
				break;
			case INT_MSG_EGM_AWARD:
				ProcessProgressiveJackpotPaid(msg);
				break;
			case INT_MSG_GENERAL_POLL_EXCEPTION:
				ProcessGeneralPollException(msg);
				break;
			case INT_MSG_OFFLINE_COIN_IN:
				ProcessOfflineCoinIn(msg);
				break;
			case INT_MSG_PROGRESSIVE_WIN:
				ProcessProgressiveWin(msg);
				break;
			case INT_MSG_SET_DENOM:
				ProcessSetCurrentDenom(msg);
				break;
			case INT_DMSG_PROGRESSIVE_QUEUE_TO_ONELINK:
				ProcessQueueToOneLink(msg);
				break;
			case INT_MSG_EGM_STATE:
				ProcessEgmStateResponse(msg);
				break;
			case INT_MSG_PROMOGAME_COMPLETE:
				ProcessPromoGameComplete(msg);
				break;
			case INT_MSG_PAYOUT:
				ProcessPayout(msg);
				break;
			case INT_MSG_CAMPAIGN_DOWNLOAD_REQUEST:
				ProcessCampaignDownloadRequest(msg);
				break;
			case INT_MSG_SKIN_DOWNLOAD_REQUEST:
				ProcessSkinDownloadRequest(msg);
				break;
			case INT_MSG_PROGRESSIVE_DASHBOARD_STATUS:
				ProcessProgressiveDashboardStatus(msg);
				break;
			default:
				assert(false); // We need to add this message to the switch.
				LogString(ERROR_LOG, _T("Unexpected message from Internals to Progressive: %d"), (int) msg->GetMessageType());
				break;
		}
	}

	delete msg;
}

CProgressiveConnection *CProgressiveProtocol::GetProgressiveConnection(CProgressiveDataConsumer &dataConsumer,
	CProgressiveConfig *progConfig,
	const std::string &paltronicsCertificatePathname,
	IOperatingSystem &operatingSystem)
{
	return new CProgressiveConnection(dataConsumer, progConfig, paltronicsCertificatePathname, m_firmwareVersion, operatingSystem);
}

void CProgressiveProtocol::RefreshConnection()
{
	unique_ptr<CProgressiveConfig> progConfig(LoadConfigFile());

	if (progConfig.get() != NULL && progConfig->GetActive())
	{
		// Start the progressive connection
		if (m_progressiveConnection == NULL)
		{
			std::string certificatePathname = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_DEFAULT_CERTIFICATE_STORE);
			certificatePathname += PALTRONICS_CERTIFICATE;
			m_progressiveConnection = GetProgressiveConnection(*this, progConfig.get(), certificatePathname, m_operatingSystem);
		}
		// Reload the config.
		else
		{
			m_progressiveConnection->LoadProgressiveConfig(progConfig.get());
		}

		// Request an INT_MSG_PROGRESSIVE_EXTERNAL_COMMUNICATIONS_STATUS message from Internals.
		CInternalMessage* msg = new CInternalMessage(INT_MSG_PROGRESSIVE_EXTERNAL_COMMUNICATIONS_STATUS);
		SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
	}

	// The progressive is not active
	else
	{
		// Remove the connection if it is there
		if (m_progressiveConnection != NULL)
		{
			delete m_progressiveConnection;
			m_progressiveConnection = NULL;
		}
	}
}

CProgressiveConfig* CProgressiveProtocol::LoadConfigFile()
{
	CProgressiveConfigFile progConfigFile(m_operatingSystem);
	progConfigFile.Start();
	CProgressiveConfig* progConfig(progConfigFile.GetProgressiveConfigCopy());
	if (progConfig != nullptr)
	{
		m_configFileActive = progConfig->GetActive();
		progConfig->SetActive(IsActive());

	// Send the current progressive configuration to Internals.
	CProgressiveConfig* progressiveConfig(new CProgressiveConfig(*progConfig));
	CInternalMessage* msg = new CInternalMessage(INT_MSG_SET_PROGRESSIVE_CONFIG, progressiveConfig);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
	}

	return progConfig;
}

void CProgressiveProtocol::ProcessDefaultDataDirectoryUpdated(CInternalMessage *msg)
{
	std::string msgRefId = msg->GetStringData();
	std::string filename = msg->GetStringData(true);
	if (m_progressiveConnection != nullptr && !msgRefId.empty() && !filename.empty())
	{
		// Send ConfigFileDownloadAcknowledgement to OneLink.
		m_progressiveConnection->SendConfigFileDownloadAck(msgRefId, filename);
	}

	RefreshConnection();
}

bool CProgressiveProtocol::IsActive()
{
	return m_oneLinkProgressivesSenOptionEnabled && m_oneLinkProgressivesSupportedByEGM;
}
				
void CProgressiveProtocol::ProcessSetOneLinkProgressivesSupportFromInternals(CInternalMessage* msg)
{
	if (msg != NULL)
	{
		bool oldActive(IsActive());
		m_oneLinkProgressivesSenOptionEnabled = msg->GetWordData() != 0;
		m_oneLinkProgressivesSupportedByEGM = msg->GetWordData(true) != 0;
		bool newActive(IsActive());

		if (oldActive != newActive)
		{
			RefreshConnection();
		}
	}
}

void CProgressiveProtocol::ProcessSetProgressiveConfigFromInternals(CInternalMessage *msg)
{
	CProgressiveConfig *progressiveConfig = msg->GetProgressiveConfig();

	if (progressiveConfig != NULL)
	{
		CProgressiveConfigFile progressiveConfigFile;
		progressiveConfigFile.SetProgressiveConfig(*progressiveConfig); // Writes the configuration to disk.
		RefreshConnection(); // Reads the configuration from disk.
	}
}

void CProgressiveProtocol::ConnectionUpdate(const CProgressiveConnectionInfo &connectionInfo)
{
	// Send the updated progressive connection info to Internals.
	CProgressiveConnectionInfo* progressiveConnectionInfo(new CProgressiveConnectionInfo(connectionInfo));
	CInternalMessage* msg = new CInternalMessage(INT_MSG_SET_PROGRESSIVE_CONNECTION_INFO, progressiveConnectionInfo);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::ProgressiveHitSentStatusUpdate(const CEGMAward &award, bool wasSent)
{
	if (wasSent)
	{
		CEGMAward *awardToSend = new CEGMAward(award);
		CInternalMessage* msg = new CInternalMessage(INT_MSG_SET_PROGRESSIVE_HIT_SENT, awardToSend);
		SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
	}
}

void CProgressiveProtocol::SendJackpotPaidRequestStatusUpdate(const CEGMAward &award, bool wasSent)
{
	if (wasSent)
	{
		CEGMAward *awardToSend = new CEGMAward(award);
		CInternalMessage* msg = new CInternalMessage(INT_MSG_SET_PROGRESSIVE_PAID_SENT, awardToSend);
		SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
	}
}

void CProgressiveProtocol::HostUpdate(const CProgressiveHost *host)
{
	// Send the updated progressive host information to Internals.
	CProgressiveHost* progressivehost(NULL);
	if (host == NULL)
	{
		progressivehost = new CProgressiveHost(_T(""), 0);
	}
	else
	{
		progressivehost = new CProgressiveHost(*host);
	}

	CInternalMessage* msg = new CInternalMessage(INT_MSG_SET_PROGRESSIVE_HOST, progressivehost);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::GetProgressiveGameInfo()
{
	// Ask internals to send game info.
	CInternalMessage *msg = new CInternalMessage(INT_MSG_GET_PROGRESSIVE_GAMES_INFO);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::JackpotHit(CEGMAward *egmAward)
{
	// Send the jackpot hit event information to Internals.
	CInternalMessage* msg = new CInternalMessage(INT_MSG_EGM_AWARD, egmAward);
	if (!egmAward->IsStandardProgressive())
	{
		// Use PERSIST_JACKPOTHIT to queue message
		CMessagePersist *mp = new CMessagePersist(false, JACKPOTHITSTARVETICKS, QUEUE_PRIORITY_NORMAL, PERSIST_JACKPOTHIT);
		msg->SetMessagePersist(mp);
	}

	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::ProgressiveJackpotAnnouncement(CProgressiveJackpotAnnouncement* pAnnouncement)
{
	CInternalMessage* msg = new CInternalMessage(INT_MSG_PROGRESSIVE_JACKPOT_ANNOUNCEMENT, pAnnouncement);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::ProgressiveTotalizerAnnouncement(CProgressiveTotalizerAnnouncement* pAnnouncement)
{
	CInternalMessage* msg = new CInternalMessage(INT_MSG_PROGRESSIVE_TOTALIZER_ANNOUNCEMENT, pAnnouncement);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::ClaimableAward(CProgressiveClaimableAward* pClaimableAward)
{
	CInternalMessage* msg = new CInternalMessage(INT_MSG_PROGRESSIVE_CLAIMABLE_AWARD, pClaimableAward);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::SendProgressiveConfiguration(ProgressiveLevelsInfo* pProgressiveLevelsInfo, CProgressiveAnnouncementsInfo* pProgressiveAnnouncementsInfo, CFirmwarePackage* pFirmwarePackage, CCampaignConfig* pCampaignConfig, CSkinConfig* pSkinConfig)
{
	// Send the Progressive configuration data to Internals.
	SendMessageToInternals(new CInternalMessage(INT_MSG_PROGRESSIVE_CONFIGURATION, pProgressiveLevelsInfo, pProgressiveAnnouncementsInfo, pFirmwarePackage, pCampaignConfig, pSkinConfig), QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::ProgressiveBonusingAvailable(CProgressiveBonusingAvailable *progresiveBonusingAvailable, 
	CProgressiveEligibleLevelsInfo *progressiveEligibleLevelsInfo, 
	ProgressivePatronSessionInfo *progressivePatronSessionInfo)
{
	// Send the information to Internals.
	CInternalMessage *msg = new CInternalMessage(INT_MSG_PROGRESSIVE_BONUSING_AVAILABLE, progresiveBonusingAvailable);
	msg->SetData(progressiveEligibleLevelsInfo);
	msg->SetData(progressivePatronSessionInfo);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::SendRemoteReboot()
{
	CInternalMessage *msg = new CInternalMessage(INT_MSG_REMOTE_REBOOT, (WORD)REBOOT_NO_ACTIVE_SESSION);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::SendPromoGameRequest(CPromoGameRequest* promoGameRequest)
{
	SendMessageToInternals(new CInternalMessage(INT_MSG_PROMOGAME_REQUEST, promoGameRequest), QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::SendFirmwareDownload(CFirmwarePackage* firmwarePackage)
{
	SendMessageToInternals(new CInternalMessage(INT_MSG_FIRMWARE_PACKAGE_AVAILABLE, firmwarePackage), QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::SendConfigFileDownload(CConfigFilePackage* configFilePackage)
{
	SendMessageToInternals(new CInternalMessage(INT_MSG_CONFIG_FILE_AVAILABLE, configFilePackage), QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::SendCampaignDownload(CCampaignPackage* campaignPackage)
{
	SendMessageToInternals(new CInternalMessage(INT_MSG_CAMPAIGN_AVAILABLE, campaignPackage), QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::SendSkinDownload(CSkinPackage* skinPackage)
{
	SendMessageToInternals(new CInternalMessage(INT_MSG_SKIN_AVAILABLE, skinPackage), QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::SendScheduleEnd(int64_t levelId, const string& levelName)
{
	CInternalMessage* msg(new CInternalMessage(INT_MSG_PROGRESSIVE_SCHEDULE_END, levelId));
	msg->SetData(levelName);
	LogString(PROGRESSIVE, "SendScheduleEnd for level id=%lld name=%s to Internals.", levelId, levelName.c_str());
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::ProgressiveLevelBroadcast(CProgressiveLevelBroadcast* pLevelBroadcast)
{
	// Send the level broadcast data to Internals.
	CInternalMessage* msg = new CInternalMessage(INT_MSG_PROGRESSIVE_LEVEL_BROADCAST, pLevelBroadcast);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::ProgressiveWinnerPick(CProgressiveWinnerPick* pWinnerPick)
{
	// Send the winner pick data to Internals.
	CInternalMessage* msg = new CInternalMessage(INT_MSG_PROGRESSIVE_WINNER_PICK, pWinnerPick);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::ProgressiveWinnerPicked(CProgressiveWinnerPicked* pWinnerPicked)
{
	// Send the winner picked data to Internals.
	CInternalMessage* msg = new CInternalMessage(INT_MSG_PROGRESSIVE_WINNER_PICKED, pWinnerPicked);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::PerformTimedEvents()
	{
		if (m_progressiveConnection != nullptr)
		{
			m_progressiveConnection->TimedEvents();
		}
}

void CProgressiveProtocol::ProcessSetGamesInfo(CInternalMessage *msg)
{
	if (m_progressiveConnection != nullptr)
	{
		LogString(GAMES, "%s %d", __FUNCTION__, __LINE__);
		CGames *games = msg->GetGamesData();
		if (games != nullptr)
		{
			LogString(GAMES, "%s %d", __FUNCTION__, __LINE__);
			m_progressiveConnection->SetGames(games, msg->GetGameEndData());
		}
	}
}

void CProgressiveProtocol::ProcessSetConnectionFromInternals(CInternalMessage *msg)
{
	if (m_progressiveConnection != nullptr)
	{
		CConnection *connection = msg->GetConnectionData();
		if (connection != nullptr)
		{
			m_progressiveConnection->LoadDeviceConnectionData(connection);
		}
	}
}


void CProgressiveProtocol::ProcessExternalCommunicationsStatus(CInternalMessage *msg)
{
	if (m_progressiveConnection != nullptr)
	{
		m_progressiveConnection->SetDependentCommunicationStatus((bool)msg->GetWordData(), (bool)msg->GetWordData(true));
	}
}

void CProgressiveProtocol::ProcessGameStartNotification(CInternalMessage *msg)
{
	if (m_progressiveConnection != nullptr)
	{
		CGameEndData *gameEndData = msg->GetGameEndData();
		m_progressiveConnection->SendCoinIn(gameEndData);
	}
}

void CProgressiveProtocol::ProcessGameEndNotification(CInternalMessage *msg)
{
	if (m_progressiveConnection != nullptr)
	{
		CGameEndData *gameEndData = msg->GetGameEndData();
		m_progressiveConnection->SendGameEnd(gameEndData);
	}	
}

void CProgressiveProtocol::ProcessPayout(CInternalMessage* msg)
{
	if (m_progressiveConnection != nullptr)
	{
		CPayout* payout = msg->GetPayout();
		m_progressiveConnection->SendPayout(payout);
	}
}

void CProgressiveProtocol::ProcessCampaignDownloadRequest(CInternalMessage* msg)
{
	if (m_progressiveConnection != nullptr)
	{
		auto campaignId = msg->GetDWordData();
		m_progressiveConnection->SendCampaignDownloadRequest(campaignId);
	}
}

void CProgressiveProtocol::ProcessSkinDownloadRequest(CInternalMessage* msg)
{
	if (m_progressiveConnection != nullptr)
	{
		auto skinId = msg->GetDWordData();
		m_progressiveConnection->SendSkinDownloadRequest(skinId);
	}
}

void CProgressiveProtocol::ProcessProgressiveDashboardStatus(CInternalMessage* msg)
{
	if (m_progressiveConnection != nullptr)
	{
		m_progressiveConnection->SetProgressiveDashboardStatus(
			msg->GetDWordData(), msg->GetIntData(),				// campaign id and campaign revision
			msg->GetDWordData(true), msg->GetIntData(true),		// skin id and skin revision
			msg->GetWordData(), msg->GetWordData(true));		// current activity and pending action
	}
}

void CProgressiveProtocol::ProcessGameData(CInternalMessage *msg)
{
	if (m_progressiveConnection != nullptr)
	{
		CGameEndData *gameData = msg->GetGameEndData();
		m_progressiveConnection->ProcessGameData(gameData);
	}
}

void CProgressiveProtocol::ProcessPlayerData(CInternalMessage *msg)
{
	if (m_progressiveConnection != nullptr)
	{
		bool sessionActive(msg->GetWordData());
		int sessionId((int)msg->GetDWordData());
		m_progressiveConnection->SetPlayer(msg->GetPlayerData(), sessionActive, sessionId);
	}
}

void CProgressiveProtocol::ProcessProgressiveJackpotPaid(CInternalMessage* msg)
{
	if (m_progressiveConnection != nullptr)
	{
		CEGMAward* egmAward(msg->GetEGMAward());
		if (egmAward != nullptr)
		{
			if (!egmAward->IsStandardProgressive() && egmAward->IsAwardStateFinalOrPending())
			{
				// Clear starvation caused by jackpot hit message
				m_scfToInternalsQueue.ClearQueueStarvation(PERSIST_JACKPOTHIT);
			}
			m_progressiveConnection->SendJackpotPaidRequest(*egmAward);
			if (AwardType::Prize != egmAward->GetAwardType())
			{
				LogString(PROGRESSIVE, "Sending INT_MSG_EGM_AWARD_STATUS to internals");
				SendMessageToInternals(new CInternalMessage(INT_MSG_EGM_AWARD_STATUS, new CEGMAward(*egmAward)), QUEUE_PRIORITY_HIGHEST);
			}
		}
	}
}

void CProgressiveProtocol::ProcessGeneralPollException(CInternalMessage* msg)
{
	if (m_progressiveConnection != nullptr)
	{
		GeneralPollException generalPollException((GeneralPollException)msg->GetWordData());
		m_progressiveConnection->SendICardEvent(generalPollException);
	}
}

void CProgressiveProtocol::ProcessOfflineCoinIn(CInternalMessage *msg)
{
	if (m_progressiveConnection != nullptr)
	{
		__int64 coinIn(msg->GetInt64Data());
		m_progressiveConnection->SendOfflineCoinInEvent(coinIn);
	}
}

void CProgressiveProtocol::ProcessProgressiveWin(CInternalMessage *msg)
{
	if (m_progressiveConnection != nullptr)
	{
		LogString(PROGRESSIVE, _T("CProgressiveProtocol::ProcessProgressiveWin message: %s"), msg->GetMessageTypeStr().c_str());
		const CEGMAward *egmAward = msg->GetEGMAward();
		if (nullptr != egmAward)
		{
			LogString(PROGRESSIVE, _T(
				"CProgressiveProtocol::ProcessProgressiveWin message: Hit Amount: %llu - Hit Level: %d"),
				egmAward->GetAwardAmountHit(), egmAward->GetLevelId());

			m_progressiveConnection->SendProgressiveLevelHit(*egmAward);
		}
	}
}

void CProgressiveProtocol::ProcessSetCurrentDenom(CInternalMessage *msg)
{
	int currentDenom = msg->GetDWordData();
	if (m_progressiveConnection != nullptr)
	{
		m_progressiveConnection->SetCurrentDenom(currentDenom);
	}
}

void CProgressiveProtocol::ProcessQueueToOneLink(CInternalMessage* pMsg)
{
	if (m_progressiveConnection != nullptr)
	{
		m_progressiveConnection->SendMessageToOneLink((const ProgressiveQueueToOneLink*) pMsg);
	}
}

void CProgressiveProtocol::EgmStateRequest(bool EgmStateRequest, int64_t jackpotId)
{
	// Send the EGM state request to internals
	int64_t clearedByOneLink(1);
	CInternalMessage* msg(new CInternalMessage(INT_MSG_EGM_STATE, jackpotId, clearedByOneLink));
	msg->SetData((WORD)EgmStateRequest);
	SendMessageToInternals(msg, QUEUE_PRIORITY_NORMAL);
}

void CProgressiveProtocol::ProcessEgmStateResponse(CInternalMessage* msg)
{
	if (msg != nullptr)
	{
		CEGMAward* egmAward(msg->GetEGMAward());
		if (egmAward && AwardType::Prize == egmAward->GetAwardType() && AwardToGameSuccess == egmAward->GetAwardState())
		{
			egmAward->SetAwardState(HandpayPending);
			ProcessProgressiveJackpotPaid(msg);
			egmAward->SetAwardState(AwardToGameSuccess);
		}
		ProcessProgressiveJackpotPaid(msg);

		bool enabled = msg->GetDWordData();
		m_progressiveConnection->SendEgmStateResponse(enabled);
	}
}

void CProgressiveProtocol::ProcessPromoGameComplete(CInternalMessage* msg)
{
	if (m_progressiveConnection != nullptr)
	{
		m_progressiveConnection->SendPromoGameAcceptanceStatus(msg->GetPromoGameAcceptanceStatus());
	}
}

