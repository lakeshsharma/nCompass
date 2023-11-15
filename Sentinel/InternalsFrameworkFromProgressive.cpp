#include "stdafx.h"
#include "InternalsFramework.h"

// Sentinel includes.
#include "Hardware/WatchDog.h"
#include "Sentinel.h"
#include "Logging/LogString.h"
#include "ProgressiveProtocol/ProgressiveHost.h"
#include "UI/MassMarketingCommand.h"
#include "UI/WinnerPickAndPickedCommand.h"
#include "UI/PromoGameCommand.h"
#include "UI/TotalizerAnnouncementCommand.h"
#include "UI/JackpotAnnouncementCommand.h"
#include "UI/ScheduleEndCommand.h"
#include "MsgTargetPair.h"
#include "Diagnostics.h"
#include "ProgressiveProtocol/EGMAwardManager.h"
#include "ProgressiveProtocol/ProgressiveLevelsInfo.h"
#include "ProgressiveProtocol/ProgressiveWinnerPick.h"
#include "ProgressiveProtocol/ProgressiveWinnerPicked.h"
#include "ProgressiveProtocol/ProgressiveKeys.h"
#include "ProgressiveProtocol/DashboardIdentifiers.h"
#include "Payout.h"
#include "FileDownloader.h"
#include "Platform.h"

using namespace std;

const DWORD DEFAULT_WINNER_PICKED_DURATION_IN_MS(30000);
const DWORD DEFAULT_ANNOUNCEMENT_DURATION_IN_MS(30000);
const DWORD DEFAULT_SCHEDULE_END_DURATION_IN_MS(10000);
const DWORD MAX_CUSTOM_CONTENT_DURATION_IN_MS(90000);
const DWORD MAX_CUSTOM_CONTENT_DURATION_SCHD_END_IN_MS(20000);

void CInternalsFramework::ProcessMessageFromProgressive(CInternalMessage *msg)
{
	// Most of the message we will delete below.
	bool bDeleteMsg = true;
	switch (msg->GetMessageType())
	{
	case INT_MSG_GET_PROGRESSIVE_GAMES_INFO:

		ProcessGetProgressiveGamesInfo(msg);
		break;
	case INT_MSG_SET_PROGRESSIVE_CONFIG:
		ProcessSetProgressiveConfig(msg);
		break;
	case INT_MSG_SET_PROGRESSIVE_CONNECTION_INFO:
		ProcessSetProgressiveConnectionInfo(msg);
		break;
	case INT_MSG_SET_PROGRESSIVE_HOST:
		ProcessSetProgressiveHost(msg);
		break;
	case INT_MSG_EGM_AWARD:
		ProcessEgmAwardMessage(msg, mqProgressive);
		break;
	case INT_MSG_EGM_AWARD_STATUS:
		ProcessEgmAwardStatus(msg);
		break;
	case INT_MSG_PROGRESSIVE_CONFIGURATION:
		ProcessProgressiveConfiguration(msg);
		break;
	case INT_MSG_PROGRESSIVE_BONUSING_AVAILABLE:
		ProcessProgressiveBonusingAvailable(msg);
		break;
	case INT_MSG_PROGRESSIVE_LEVEL_BROADCAST:
		ProcessProgressiveLevelBroadcast(msg);
		break;
	case INT_MSG_PROGRESSIVE_EXTERNAL_COMMUNICATIONS_STATUS:
		ProcessSystemChangeForProgressive();
		break;
	case INT_MSG_PROGRESSIVE_WINNER_PICK:
		ProcessProgressiveWinnerPick(msg);
		bDeleteMsg = false;
		break;
	case INT_MSG_PROGRESSIVE_WINNER_PICKED:
		ProcessProgressiveWinnerPicked(msg);
		bDeleteMsg = false;
		break;
	case INT_MSG_PROGRESSIVE_JACKPOT_ANNOUNCEMENT:
	case INT_MSG_PROGRESSIVE_TOTALIZER_ANNOUNCEMENT:
		ProcessProgressiveAnnouncement(msg);
		bDeleteMsg = false;
		break;
	case INT_MSG_PROGRESSIVE_CLAIMABLE_AWARD:
		ProcessProgressiveClaimableAward(msg);
		break;
	case INT_MSG_SET_PROGRESSIVE_PAID_SENT:
		ProcessEgmAwardMessage(msg, mqProgressive);
		break;
	case INT_MSG_SET_PROGRESSIVE_HIT_SENT:
		ProcessEgmAwardMessage(msg, mqProgressive);
		break;
	case INT_MSG_SET_PROGRESSIVE_DOWN:
		ProcessSetProgressiveDown(msg);
		break;
	case INT_MSG_REMOTE_REBOOT:
		ProcessRemoteReboot(msg);
		break;
	case INT_MSG_EGM_STATE:
		ProcessEgmStateRequest(msg);
		break;
	case INT_MSG_PROMOGAME_REQUEST:
		ProcessPromoGameRequest(msg);
		break;
	case INT_MSG_SEND_TILT:
		ProcessTilt(msg);;
		break;
	case INT_MSG_FIRMWARE_PACKAGE_AVAILABLE:
		ProcessFirmwarePackageAvailable(msg);
		break;
	case INT_MSG_CONFIG_FILE_AVAILABLE:
		ProcessConfigFileAvailable(msg);
		break;
	case INT_MSG_CAMPAIGN_AVAILABLE:
		ProcessCampaignAvailable(msg);
		break;
	case INT_MSG_SKIN_AVAILABLE:
		ProcessSkinAvailable(msg);
		break;
	case INT_MSG_PROGRESSIVE_SCHEDULE_END:
		ProcessScheduleEnd(msg);
		bDeleteMsg = false;
		break;
	default:
		//ASSERT(false); // We need to add this message to the switch.
		LogString(ERROR_LOG, _T("Unexpected message from Progressive to Internals: %d"), (int)msg->GetMessageType());
		break;
	}

	// If we are sending the message on to somewhere else, we don't want to delete the message.
	if (bDeleteMsg)
		delete msg;

	CWatchDog::Instance().Checkin();
}

void CInternalsFramework::ProcessGetProgressiveGamesInfo(CInternalMessage *msg)
{
	UNREFERENCED_PARAMETER(msg);
	CProgressiveConnectionInfo *info = m_sentinelState.GetProgressiveConnectionInfoCopy();

	m_progressiveWaitingForSubGames = true;
	if (m_finishedFirstPassForSubGames && info->GetState() == InDiscovery)
	{
		// This is a re-request for games, send it to GCF
		m_finishedFirstPassForSubGames = false;
		CInternalMessage *uiMsg = new CInternalMessage(INT_MSG_GET_PROGRESSIVE_GAMES_INFO);
		SendMessageToGCF(uiMsg, QUEUE_PRIORITY_NORMAL);
	}
	else
	{
		std::lock_guard< std::recursive_mutex > lock(m_progressiveCriticalSection);
		CheckProgressiveWaitingForSubGames();
	}

	delete info;
}

void CInternalsFramework::CheckProgressiveWaitingForSubGames()
{
	// If finished first pass and
	// If Progressive protocol is waiting for sub game info, send it now.
	if (m_finishedFirstPassForSubGames && m_progressiveWaitingForSubGames)
	{
		m_games.CalcProgressiveGroup();

		// if coMeters was supplied, use its totCoinOutCredits value for new games collection
		CGames* games(m_sentinelState.GetProgGames());
		if (games)
		{
			games->UpdateSubGameData(&m_games);
			WORD gameNumber = EGM_GAME_NUMBER;
			CGame *egm = games->GetGame(gameNumber);
			if (egm != NULL)
			{
				__int64 totCoinOut = games->GetLastTotalCoinOutMeterValue();
				if (totCoinOut != INVALID_METER_VALUE)
				{
					games->GetGame(EGM_GAME_NUMBER)->SetMeter(gCO, totCoinOut);
				}
			}

			CInternalMessage *msg = new CInternalMessage(INT_MSG_SET_PROGRESSIVE_GAMES_INFO, new CGames(*games));
			msg->SetData(GetGameData(0, nullptr));
			SendMessageToProgressive(msg, QUEUE_PRIORITY_NORMAL);
		}

		// If progressive needs game info (again), then it might need connection data (again).
		if (m_sentinelState.m_timeOfLastFloorLogixConfigAvailable)
		{
			// Send the connection data to the progressive once we have a config.
			CInternalMessage *newMsg = CreateConnectionDataMessage();
			SendMessageToProgressive(newMsg, QUEUE_PRIORITY_NORMAL);
		}

		m_progressiveWaitingForSubGames = false;
	}
}

void CInternalsFramework::ProcessSetProgressiveConfig(CInternalMessage *msg)
{
	if (msg != NULL)
	{
		CProgressiveConfig* progressiveConfig(msg->GetProgressiveConfig());
		if (progressiveConfig != NULL)
		{
			m_sentinelState.SetProgressiveConfig(*progressiveConfig);

			// Set the progressive critical text object with the current progressive status
			if (m_progressiveCriticalText.ProcessProgressiveConfig(progressiveConfig))
			{
				// There was something changed for the critical text, so send it to the UI
				SendProgressiveCriticalTextToUI();
			}
		}
	}
}

void CInternalsFramework::ProcessProgressiveAnnouncement(CInternalMessage* msg)
{
	if (NULL != msg)
	{
		switch (msg->GetMessageType())
		{
		case INT_MSG_PROGRESSIVE_JACKPOT_ANNOUNCEMENT:
		{
			CProgressiveJackpotAnnouncement* announcement(msg->GetProgressiveJackpotAnnouncement());
			LogString(PROGRESSIVE, "CInternalsFramework::ProcessProgressiveAnnouncement: jackpot announcement %s", announcement->m_Name.c_str());

			std::unique_lock <std::recursive_mutex>  csLock(m_playerCriticalSection);
			string dynamicContentPath(m_mediaPackageManager->GetDynamicContentPath(announcement->m_Id, "", SlotsContentType::JackpotAnnouncement, pKeyLcdContent));
			CJackpotAnnouncementCommand announcementCommand(DEFAULT_ANNOUNCEMENT_DURATION_IN_MS, *announcement, dynamicContentPath);
			auto announcementJSON = announcementCommand.getJSON();
			LogString(PROGRESSIVEDATA, "CInternalsFramework::ProcessProgressiveAnnouncement: jackpot announcement JSON : %s ",announcementJSON.c_str());
			msg->SetData(announcementJSON);

			int priority(0);
			msg->SetMessagePersist(m_msgPriority.PrioritizeMessage(msg, priority));
			SendMessageToUI(msg, priority);
			break;
		}
		case INT_MSG_PROGRESSIVE_TOTALIZER_ANNOUNCEMENT:
		{
			CProgressiveTotalizerAnnouncement* announcement(msg->GetProgressiveTotalizerAnnouncement());
			LogString(PROGRESSIVE, "CInternalsFramework::ProcessProgressiveAnnouncement: totalizer announcement: %s", announcement->m_Name.c_str());

			std::unique_lock <std::recursive_mutex>  csLock(m_playerCriticalSection);
			string dynamicContentPath(m_mediaPackageManager->GetDynamicContentPath(announcement->m_Id, "", SlotsContentType::TotalizerAnnouncement, pKeyLcdContent));
			CTotalizerAnnouncementCommand announcementCommand(DEFAULT_ANNOUNCEMENT_DURATION_IN_MS, *announcement, dynamicContentPath);
			auto announcementJSON = announcementCommand.getJSON();
			LogString(PROGRESSIVE, "CInternalsFramework::ProcessProgressiveAnnouncement: totalizer announcement JSON to UI: %s ", announcementJSON.c_str());
			msg->SetData(announcementJSON);

			int priority(0);
			msg->SetMessagePersist(m_msgPriority.PrioritizeMessage(msg, priority));
			SendMessageToUI(msg, priority);
			break;
		}
		default:
		{
			LogString(PROGRESSIVE, "CInternalsFramework::ProcessProgressiveAnnouncement: invalid announcement");
			break;
		}
		}
	}
}


void CInternalsFramework::ProcessProgressiveClaimableAward(CInternalMessage *msg)
{
	if (NULL != msg)
	{
		CProgressiveClaimableAward* claimableAward(msg->GetProgressiveClaimableAward());
		if (NULL != claimableAward)
		{
			if (claimableAward->m_sessionId.IsInvalidOrEqual(m_playerSession.GetSessionId()))
			{
				std::unique_lock <std::recursive_mutex>  csLock(m_playerCriticalSection);
				LogString(PROGRESSIVE, "Received forwarded ClaimableAward");

				CMassMarketingCommand command(claimableAward, MAXIMUM_DISPLAY_DURATION);
				CInternalMessage* uiMsg = new CInternalMessage(INT_MSG_PROGRESSIVE_CLAIMABLE_AWARD, command.getJSON());
				int priority(QUEUE_PRIORITY_NORMAL);
				uiMsg->SetMessagePersist(m_msgPriority.PrioritizeMessage(uiMsg, priority));
				SendMessageToUI(uiMsg, priority);
			}
		}
	}
}

void CInternalsFramework::ProcessProgressiveWinnerPick(CInternalMessage *msg)
{
	if (NULL != msg)
	{
		CProgressiveWinnerPick* winnerPick(msg->GetProgressiveWinnerPick());
		if (NULL != winnerPick)
		{
			// Get the m_egmAwardCriticalSection critical section BEFORE the m_playerCriticalSection to avoid deadlocks.
			// Hold the lock(s) after acquiring values, so values are queued in the same order that they are acquired.
			std::unique_lock<std::recursive_mutex> csLockEgmAward(m_egmAwardCriticalSection);
			bool isValid = m_progressiveLevelManager.IsWinnerPickValid(winnerPick->m_levelId);
			if (isValid)
			{
				LogString(PROGRESSIVE, "CInternalsFramework::ProcessProgressiveWinnerPick: valid");
				std::unique_lock<std::recursive_mutex> csLock(m_playerCriticalSection);
				string dynamicContentPath(m_mediaPackageManager->GetDynamicContentPath(winnerPick->m_levelId, "", SlotsContentType::Anticipation, pKeyLcdContent));
				CWinnerPickAndPickedCommand command(MAXIMUM_DISPLAY_DURATION, FromOneLinkUdp::pWinnerPick, dynamicContentPath, winnerPick->m_levelName);
				msg->SetData(command.getJSON());

				int priority(0);
				msg->SetMessagePersist(m_msgPriority.PrioritizeMessage(msg, priority, (__int64)winnerPick->m_levelId));
				SendMessageToUI(msg, priority);
			}
			else
			{
				LogString(PROGRESSIVE, "CInternalsFramework::ProcessProgressiveWinnerPick: invalid");

				delete msg;
			}
		}
		else
		{
			delete msg;
		}
	}
}

void CInternalsFramework::ProcessProgressiveWinnerPicked(CInternalMessage *msg)
{
	if (NULL != msg)
	{
		CProgressiveWinnerPicked* winnerPicked(msg->GetProgressiveWinnerPicked());
		if (NULL != winnerPicked)
		{
			SendOneLinkAwardStatus(winnerPicked->GetAwardType(), AwardToGameSuccess, winnerPicked->GetMethodOfPayment(), winnerPicked->GetJackpotId(), winnerPicked->GetWinAmountInt64(), true, false);
			std::unique_lock<std::recursive_mutex> csLock(m_playerCriticalSection);
			LogString(PROGRESSIVE, "Received forwarded WinnerPicked");
			CInternalMessage *pteMsg = m_progressiveLevelManager.GetPteAwardDisplayItem(winnerPicked);
			const DWORD defaultDisplayDuration(AwardType::Prize == winnerPicked->GetAwardType() ? INFINITE_DURATION : DEFAULT_WINNER_PICKED_DURATION_IN_MS);
			DWORD displayDuration(winnerPicked->GetMediaMaxSeconds() > 0 ? (winnerPicked->GetMediaMaxSeconds() * TICK_COUNTS_PER_SECOND) : defaultDisplayDuration);
			string prizeDescription(m_progressiveLevelManager.GetPrizeDescription(winnerPicked->GetLevelId()));
			string awardName = winnerPicked->GetExternalAwardName();

			if (pteMsg != nullptr)
			{
				string dynamicContentPath(m_mediaPackageManager->GetDynamicContentPath(pteMsg->GetDWordData(), winnerPicked->GetMinorAwardId(), SlotsContentType::Celebration, pKeyLcdContent));
				if (!dynamicContentPath.empty())
				{
					displayDuration = MAX_CUSTOM_CONTENT_DURATION_IN_MS;
				}
				CWinnerPickAndPickedCommand command(displayDuration,
					FromOneLinkUdp::pWinnerPicked,
					dynamicContentPath,
					winnerPicked->GetLevelName(),
					winnerPicked->GetWinAmount(),
					prizeDescription,
					winnerPicked->MethodOfPaymentName(),
					winnerPicked->AwardTypeName(),
					awardName,
					winnerPicked->GetExternalAwardValue(),
					winnerPicked->GetExternalAwardCurrency());


				// prioritize message
				int priority(0);
				pteMsg->SetMessagePersist(m_msgPriority.PrioritizeWinnerPickedMessage(pteMsg, priority, winnerPicked));
				pteMsg->SetData(command.getJSON());
				SendMessageToUI(pteMsg, priority);
				LogString(PROGRESSIVE, "PTE WinnerPicked SendMessageToUI priority: %d", priority);

				// This code path doesn't reuse msg, delete it
				delete msg;
			}
			else
			{
				LogString(PROGRESSIVE, "Received forwarded WinnerPicked winAmount=%s", winnerPicked->GetWinAmount().c_str());
				if (AwardType::Prize == winnerPicked->GetAwardType() ||
					AwardType::Offer == winnerPicked->GetAwardType())
				{
					msg->SetData((WORD)(AwardType::Prize == winnerPicked->GetAwardType() ? screenProgressivePrize : screenProgressiveOffer));
					msg->SetMessageType(INT_MSG_PROGRESSIVE_PRIZE);
				}

				if (winnerPicked->ForwardToUI())
				{
					string dynamicContentPath(m_mediaPackageManager->GetDynamicContentPath(winnerPicked->GetLevelId(), winnerPicked->GetMinorAwardId(),
						winnerPicked->GetJackpotTriggerType() == JackpotTrigger::PokerFrenzyMinor ?
						SlotsContentType::PokerMinorCelebration : SlotsContentType::Celebration, pKeyLcdContent));
					if (!dynamicContentPath.empty())
					{
						displayDuration = MAX_CUSTOM_CONTENT_DURATION_IN_MS;
					}

					int priority(0);
					CMessagePersist* mpPrize(nullptr);
					if (AwardType::Prize == winnerPicked->GetAwardType())
					{
						mpPrize = m_msgPriority.PrioritizeMessage(msg, priority);
						msg->SetMessagePersist(m_msgPriority.PrioritizeMessage(msg, priority));
					}
					else
					{
						msg->SetMessagePersist(m_msgPriority.PrioritizeWinnerPickedMessage(msg, priority, winnerPicked));
					}

					if (winnerPicked->GetAwardType() == AwardType::Type::ExtendedBucket && m_player != nullptr)
					{
						DWORD id = strtoul(winnerPicked->GetExternalAwardId().c_str(), nullptr,10);
						m_player->GetBucketDisplayName(id, awardName);
					}
					CWinnerPickAndPickedCommand command(displayDuration,
						FromOneLinkUdp::pWinnerPicked,
						dynamicContentPath,
						winnerPicked->GetLevelName(),
						winnerPicked->GetWinAmount(),
						prizeDescription,
						winnerPicked->MethodOfPaymentName(),
						winnerPicked->AwardTypeName(),
						awardName,
						winnerPicked->GetExternalAwardValue(),
						winnerPicked->GetExternalAwardCurrency());
					msg->SetData(command.getJSON());
					msg->SetMessagePersist(m_msgPriority.PrioritizeMessage(msg, priority));
					LogString(PROGRESSIVE, _T("SendMessageToUI %s priority: %d"), msg->GetMessageTypeStr().c_str(), priority);
					SendMessageToUI(msg, priority);
				}
				else
				{
					LogString(PROGRESSIVE, _T("ProcessProgressiveWinnerPicked  winnerPicked->ForwardToUI()=falsed"));
					delete msg;
				}
			}
		}
		else
		{
			delete msg;
		}
	}
}

void CInternalsFramework::ProcessSetProgressiveConnectionInfo(CInternalMessage *msg)
{
	if (msg != NULL)
	{
		// Get the progressive connection info object from the message
		CProgressiveConnectionInfo* progressiveConnectionInfo(msg->GetProgressiveConnectionInfo());

		if (progressiveConnectionInfo != NULL)
		{
			// Set the progressive info for the diagnostic pages.
			m_sentinelState.SetProgressiveConnectionInfo(*progressiveConnectionInfo);

			// Set the progressive critical text object with the current progressive status
			if (m_progressiveCriticalText.ProcessConnectionInfo(progressiveConnectionInfo))
			{
				// There was something changed for the critical text, so send it to the UI
				SendProgressiveCriticalTextToUI();
			}
		}
	}
}

void CInternalsFramework::ProcessSasStartupQueueEmptyUpdate()
{
	// Set the current SAS startup queue empty status in the progressive critical text object
	if (m_progressiveCriticalText.ProcessSasStartupQueueEmpty(m_playerSession.IsSasStartupQueueEmpty()))
	{
		// There was something changed for the critical text, so send it to the UI
		SendProgressiveCriticalTextToUI();
	}
}

void CInternalsFramework::ProcessSystemChangeForProgressive()
{
	SendOneLinkProgressivesSupportToProgressive();

	{
		// Get a lock, so values are queued in the same order that they are acquired.
		std::unique_lock<std::recursive_mutex> csLock(m_playerCriticalSection);
		SendMessageToProgressive(
			new CInternalMessage(INT_MSG_PROGRESSIVE_EXTERNAL_COMMUNICATIONS_STATUS,
			(WORD)(m_playerSession.IsSasStartupQueueEmpty()),
				(WORD) m_progressiveLevelManager.GetMulticastActive()),
			QUEUE_PRIORITY_NORMAL);
	}

	SendGameDataToProgressive(INT_MSG_GAME_DATA, 0, NULL);
	SendCurrentDenomToProgressive();
	DistributePlayerDataToProgressiveProtocol();
	SendDashboardStatusToProgressive();
}

void CInternalsFramework::SendOneLinkProgressivesSupportToProgressive()
{
	// Get a lock, so values are queued in the same order that they are acquired.
	lock_guard<recursive_mutex> lock(m_egmAwardCriticalSection);
	SendMessageToProgressive(new CInternalMessage(INT_MSG_SET_ONE_LINK_PROGRESSIVES_SUPPORT,
		(WORD)m_config.OneLinkProgressivesEnabled(), (WORD)m_EGMConfig.GetProgressivesSupportedByEGM()),
		QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::ProcessSetProgressiveHost(CInternalMessage *msg)
{
	if (msg != NULL)
	{
		CProgressiveHost* progressiveHost(msg->GetProgressiveHost());
		if (progressiveHost != NULL)
		{
			m_sentinelState.SetProgressiveHost(*progressiveHost);
		}
	}
}

// Process anything related to m_egmAwardManager.
// Includes INT_MSG_EGM_AWARD, INT_MSG_SET_METERS, and msg == NULL (for timed events).
void CInternalsFramework::ProcessEgmAwardMessage(CInternalMessage* msg, MessageQueueId source)
{
	std::lock_guard<std::recursive_mutex> lock(m_egmAwardCriticalSection);
	{
		if (mqProgressive == source && INT_MSG_EGM_AWARD == msg->GetMessageType())
		{
			CEGMAward* egmAwardHit(msg->GetEGMAward());
			CInternalMessage *transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_ONELINK, IsPlayerVirtual());
			egmAwardHit->SetFieldsForOneLinkAwardHit(transMsg);
			SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);
		}

		// Hold lock until outgoing messages are queued, so outgoing messages are queued in order.
		CInternalMessage* targetMsg(NULL);
		MessageQueueId target(mqNone);
		SYSTEMTIME systemTime;
		std::vector<MsgTargetPair*> returnMessages;
		bool progressiveIsDown = m_progressiveCriticalText.IsDown();
		m_progressiveLevelManager.SetProgressiveDown(progressiveIsDown);

		time_t currentUtcTime = CUtilities::GetUTCTime();
		systemTime = CUtilities::GetUTCSystemTime(currentUtcTime);
		m_egmAwardManager.ProcessMessage(CUtilities::GetTickCount(),
			m_games,
			progressiveIsDown,
			msg,
			source,
			targetMsg,
			target,
			&returnMessages,
			m_playerSession.IsSasStartupQueueEmpty());

		switch (target)
		{
		case mqProgressive:
			SendMessageToProgressive(targetMsg, QUEUE_PRIORITY_NORMAL);
			break;

		case mqGCF:
			SendMessageToGCF(targetMsg, QUEUE_PRIORITY_NORMAL);
			break;

		case mqInternals:
			if (targetMsg != nullptr)
			{
				// Handle message immediately without queuing it.
				if (INT_MSG_SEND_PROGRESSIVE_LEVEL_FORWARD == targetMsg->GetMessageType())
				{
					m_progressiveCacheEventHandle.signal(false);
				}
				delete targetMsg;
			}
			break;

		default:
			if (targetMsg != NULL)
				delete targetMsg;
			break;
		}

		ProcessReturnedMessages(&returnMessages);
	}
}

bool CInternalsFramework::ProcessEgmAwardStatus(CInternalMessage* msg)
{
	bool forwardToUI(false);
	const CEGMAward* egmAward(msg->GetEGMAward());
	if (egmAward)
	{
		if (AwardToGameError == egmAward->GetAwardState() 
			|| AwardToGameIneligible == egmAward->GetAwardState()
			|| AwardToGameSuccess == egmAward->GetAwardState())
		{
			SendOneLinkAwardStatus(egmAward->GetAwardType(), egmAward->GetAwardState(), egmAward->GetMethodOfPayment(),
				egmAward->GetJackpotId(), egmAward->GetAwardAmountHit(), false, false);

			forwardToUI = egmAward->IsFromPromoGame() && AwardToGameError == egmAward->GetAwardState();
			LogString(PROGRESSIVE, "egmAward->IsFromPromoGame() = %d, AwardToGameError == egmAward->GetAwardState() = %d",
				egmAward->IsFromPromoGame(), AwardToGameError == egmAward->GetAwardState());
			LogString(PROGRESSIVE, "forwardToUI = %d", forwardToUI);
			if (forwardToUI)
			{
				LogString(PROGRESSIVE, _T("Forward EgmAwardStatus to UI to display award failed message"));
				int priority(QUEUE_PRIORITY_HIGHEST);
				msg->SetData(egmAward->GetAwardAmountHit());
				msg->SetMessagePersist(m_msgPriority.PrioritizeMessage(msg, priority));
				SendMessageToUI(msg, priority);
			}
		}
	}
	return !forwardToUI;
}

void CInternalsFramework::ProcessProgressiveConfiguration(CInternalMessage* msg)
{

	//m_egmAwardCriticalSection.Lock();
	std::lock_guard<std::recursive_mutex> lock(m_egmAwardCriticalSection);

	m_progressiveLevelManager.SetProgressiveDown(m_progressiveCriticalText.IsDown());
	const ProgressiveLevelsInfo* pLevelsInfo(msg->GetProgressiveLevelsInfo());
	m_progressiveLevelManager.UpdateLevels(pLevelsInfo);
	m_mediaPackageManager->SetOneLinkContentFiles(pLevelsInfo, msg->GetProgressiveAnnouncementsInfo());

	if (!CUtilities::DoesFileExist("/sdboot/DISABLE_OFFLINE_DOWNLOADS"))
	{
		ProcessFirmwarePackageAvailable(msg);

		//If there is campaign config then process it
		CCampaignConfig* campaignConfig(msg->GetCampaignConfig());
		if (nullptr != campaignConfig)
		{
			if (m_sentinelState.GetCampaignId() != campaignConfig->GetId() ||
				m_sentinelState.GetCampaignRevision() != campaignConfig->GetRevision())
			{ 
				LogString(PROGRESSIVE, _T("Sending INT_MSG_CAMPAIGN_DOWNLOAD_REQUEST to Progressive from Internals CampaignId=%u ConfigCampaignId=%u CampaignRevision=%u ConfigCampaignRevision=%u"),
					m_sentinelState.GetCampaignId(), campaignConfig->GetId(), m_sentinelState.GetCampaignRevision(), campaignConfig->GetRevision());

				CInternalMessage* msg = new CInternalMessage(INT_MSG_CAMPAIGN_DOWNLOAD_REQUEST, campaignConfig->GetId());
				SendMessageToProgressive(msg, QUEUE_PRIORITY_NORMAL);
			}
		}

		//If there is skin config then process it
		CSkinConfig* skinConfig(msg->GetSkinConfig());
		if (nullptr != skinConfig)
		{
			if (m_sentinelState.GetSkinId() != skinConfig->GetId() ||
				m_sentinelState.GetSkinRevision() != skinConfig->GetRevision())
			{
				LogString(PROGRESSIVE, _T("Sending INT_MSG_SKIN_DOWNLOAD_REQUEST to Progressive from Internals SkinId=%u ConfigSkinId=%u SkinRevision=%u ConfigSkinRevision=%u"),
					m_sentinelState.GetSkinId(), skinConfig->GetId(), m_sentinelState.GetSkinRevision(), skinConfig->GetRevision());
				CInternalMessage* msg = new CInternalMessage(INT_MSG_SKIN_DOWNLOAD_REQUEST, skinConfig->GetId());
				SendMessageToProgressive(msg, QUEUE_PRIORITY_NORMAL);
			}
		}
	}

	SendMessageToGCF(new CInternalMessage(INT_MSG_QUERY_POKER_HAND,
		(WORD) m_progressiveLevelManager.QueryPokerCards()), QUEUE_PRIORITY_NORMAL);

	SendMessageToGCF(new CInternalMessage(INT_MSG_PROGRESSIVE_ACCOUNTING_ONLY,
		(WORD) m_progressiveLevelManager.HasLevelType(ProgressiveLevelType::AccountingOnly)), QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::ProcessProgressiveBonusingAvailable(CInternalMessage *msg)
{
	// Get the m_egmAwardCriticalSection critical section BEFORE the m_playerCriticalSection to avoid deadlocks.
	// Hold the lock(s) after acquiring values, so values are queued in the same order that they are acquired.
	std::lock_guard<std::recursive_mutex> lock(m_egmAwardCriticalSection);
	m_progressiveLevelManager.SetProgressiveDown(m_progressiveCriticalText.IsDown());

	CProgressiveBonusingAvailable *progressiveBonusingAvailable = msg->GetProgressiveBonusingAvailable();
	CProgressiveEligibleLevelsInfo *eligibleLevels = msg->GetProgressiveEligibleLevelsInfo();

	if (progressiveBonusingAvailable != nullptr)
	{
		std::lock_guard<std::recursive_mutex> lock(m_playerCriticalSection);
		if (progressiveBonusingAvailable->GetSessionId() == m_playerSession.GetSessionId())
		{
			m_progressiveLevelManager.ProcessBonusingAvailable(progressiveBonusingAvailable, eligibleLevels);

			bool criticalTextChanged = m_progressiveCriticalText.ProcessEligibilityDetermined(m_progressiveLevelManager.GetEligibilityDetermined());

			if (m_playerSession.ProcessProgressiveBonusingAvailable(*progressiveBonusingAvailable))
			{
				if (m_progressiveCriticalText.ProcessSessionIdMatchStatus(m_playerSession.GetSessionIdMatchStatus()))
				{
					criticalTextChanged = true;
				}
			}

			if (criticalTextChanged)
			{
				// There was something changed for the critical text, so send it to the UI
				SendProgressiveCriticalTextToUI();
			}
		}
	}
}

void CInternalsFramework::ProcessProgressiveLevelBroadcast(CInternalMessage* msg)
{
	std::lock_guard<std::recursive_mutex> lock(m_egmAwardCriticalSection);
	m_progressiveLevelManager.SetProgressiveDown(m_progressiveCriticalText.IsDown());
	m_progressiveLevelManager.ProcessLevelBroadcast(msg->GetProgressiveLevelBroadcast());
}


void CInternalsFramework::ProcessCumulativeProgressiveWin(CInternalMessage* msg, MessageQueueId source)
{
	ProcessEgmAwardMessage(msg, source);	//Tell the Award manager about the cumulative meter.
}

void CInternalsFramework::PerformProgressiveTimedEvents()
{
	// Don't perform these timed events until the queues are ready and accounting denom is greater than zero.
	// 	We want m_games.GetAftBonusEnabled() and m_games.GetLegacyBonusEnabled() to be accurate.
	if (this->GetProgressiveQueue() != NULL && GetGCFQueue() != NULL && m_games.GetAccountingDenom() > 0)
	{
		// And perform these timed events only when we have communications to the progressive system and the EGM.
		// So it is more likely that (result) messages will be delivered (in a timely manner).
		const CProgressiveConnectionInfo* progInfo(m_sentinelState.GetProgressiveConnectionInfoCopy());
		if (progInfo->GetState() == Connected)
		{
			ProcessEgmAwardMessage(NULL, mqNone);
		}
		delete progInfo;
	}
}

void CInternalsFramework::SendProgressiveCriticalTextToUI()
{
	// Set progressive critical alert in notification box.
	WORD alert = m_progressiveCriticalText.GetAlert();

	// Clear progressive alerts.
	m_notificationBox.RemoveAlert(Alerts::ALERT_PROGRESSIVE_DOWN);
	m_notificationBox.RemoveAlert(Alerts::ALERT_PROGRESSIVE_INELIGIBILITY);

	if (alert)
	{
		m_notificationBox.AddAlert((Alerts)alert);
	}
	SendSubscriptionUpdateIfNecessary();
}

void CInternalsFramework::InitializeProgressive(void)
{
	// Loop until the outgoing message queue to the Progressive protocol is resolved.
	m_protocolManager->WaitForProtocol(IProtocolManager::ProgressiveProtocol);
	SendOneLinkProgressivesSupportToProgressive();
}

void CInternalsFramework::UninitializeProgressive(void)
{
	m_protocolManager->RemoveProtocol(IProtocolManager::ProgressiveProtocol);
}

void CInternalsFramework::ProgressiveCacheThreadInner(CInternalsFramework *internals)
{
	CUtilities::SetThreadName(CUtilities::GetCurrentThreadId(), THREAD_PROGRESSIVE_CACHE_INTERNALS);
	CUtilities::ShowThreadId(_T(THREAD_PROGRESSIVE_CACHE_INTERNALS));
	internals->ProgressiveCacheLoop();
}


void CInternalsFramework::ProgressiveCacheLoop()
{
	while (!m_shutdownProgressiveCacheThread)
	{
		CWatchDog::Instance().Checkin();
		bool mulitcastActiveChanged;
		CProgressiveLevelForward *pLevelForward = GetProgressiveLevelForward(mulitcastActiveChanged);

		CWatchDog::Instance().Checkin();
		if (pLevelForward != nullptr)
		{
			// Callee will make a copy of pLevelForward, if needed.
			SendProgressiveLevelForwardToGCF(*pLevelForward);
			CWatchDog::Instance().Checkin();

			// Callee is responsible for deleting this instance of pLevelForward, which means this call must be last.
			m_progressiveLevelManager.UpdateLevelCounters(pLevelForward);
			SendSubscriptionUpdateIfNecessary();
			CWatchDog::Instance().Checkin();
		}

		if (mulitcastActiveChanged)
		{
			ProcessSystemChangeForProgressive();

			CWatchDog::Instance().Checkin();
		}

		m_progressiveCacheEventHandle.try_wait_for(PROGRESSIVE_CACHE_EVENT_INTERVAL);
	}
}

/// <summary>
/// Returns a pointer to the progressive levels to forward.
/// </summary>
/// <param name="mulitcastActiveChanged">OUT - true if GetMulticastActive() status changed, false otherwise.</param>
/// <returns>A pointer to the progressive levels to forward, if available, else nullptr.</returns>
/// <remarks>
/// The caller is responsible for deleting the returned instance.
/// </remarks>
CProgressiveLevelForward *CInternalsFramework::GetProgressiveLevelForward(bool &mulitcastActiveChanged)
{
	std::unique_lock<std::recursive_mutex> csLockEgmAward(m_egmAwardCriticalSection);
	return m_progressiveLevelManager.GetLevelForward(
		CUtilities::GetTickCount(),
		m_progressiveCriticalText.IsProcessingDown(),
		mulitcastActiveChanged);
}

void CInternalsFramework::SendProgressiveLevelForwardToGCF(const CProgressiveLevelForward &levels)
{
	if (m_playerSession.IsSasStartupQueueEmpty())
	{
		SendMessageToGCF(
			new CInternalMessage(INT_MSG_PROGRESSIVE_LEVEL_FORWARD, new CProgressiveLevelForward(levels)),
			QUEUE_PRIORITY_HIGHEST);
	}
}

void CInternalsFramework::ProcessPromoGameRequest(CInternalMessage *msg)
{
	if (NULL != msg)
	{
		CPromoGameRequest* promoGameRequest(msg->GetPromoGameRequest());
		if (NULL != promoGameRequest)
		{
			lock_guard<recursive_mutex> csLockEgmAward(m_egmAwardCriticalSection);
			lock_guard<recursive_mutex> csLockPlayer(m_playerCriticalSection);
			LogString(PROGRESSIVE, _T("Received forwarded PromoGameRequest levelId=%lld minorAwardId=%s jackpotId=%lld"), 
				promoGameRequest->GetLevelId(), promoGameRequest->GetMinorAwardId().c_str(), promoGameRequest->GetJackpotId());

			CInternalMessage *promoMsg = new CInternalMessage(INT_MSG_PROGRESSIVE_PROMO_GAME);
			int priority = 0;
			promoMsg->SetMessagePersist(m_msgPriority.PrioritizeMessage(promoMsg, priority));

			string dynamicContentPath(m_mediaPackageManager->GetDynamicContentPath(promoGameRequest->GetLevelId(), promoGameRequest->GetMinorAwardId(), SlotsContentType::PromoGame, pKeyLcdContent));
			CPromoGameCommand promoGameCommand(promoGameRequest->GetPaytableItemType(),
				promoGameRequest->GetPaytableItemValue(),
				promoGameRequest->GetJackpotId(),
				promoGameRequest->GetAmount(),
				dynamicContentPath);
			promoMsg->SetData(promoGameCommand.getJSON());
			SendMessageToUI(promoMsg, priority);
			LogString(PROGRESSIVE, "PromoGameRequest SendMessageToUI priority: %d", priority);
		}
	}
}

void CInternalsFramework::ProcessEgmStateRequest(CInternalMessage* msg)
{
	// Should only be received to re-enable game disabled for prize award
	if (msg != nullptr)
	{
		bool enable = msg->GetWordData();
		if (enable)
		{
			int64_t jackpotId(msg->GetInt64Data());
			int64_t clearedByOneLink(msg->GetInt64Data(true));
			LogString(PROGRESSIVE, _T("ProcessEgmStateRequest jackpotId=%lld clearedByOneLink=%lld"), jackpotId, clearedByOneLink);
			// Send to unlock to GCF
			lock_guard<recursive_mutex> lock(m_egmAwardCriticalSection);
			m_egmAwardManager.SetUnlockInProgress(true);
			SendMessageToGCF(new CInternalMessage(INT_MSG_PRIZE_LOCK_CLEAR, jackpotId, clearedByOneLink), QUEUE_PRIORITY_NORMAL);
		}
	}
}

void CInternalsFramework::ProcessPrizeLockClear(int64_t jackpotId, bool clearedByOneLink)
{
	lock_guard<recursive_mutex> lock(m_egmAwardCriticalSection);
	m_egmAwardManager.SetUnlockInProgress(false);
	LogString(PROGRESSIVE, _T("INT_MSG_PRIZE_LOCK_CLEAR-Game unlocked jackpotId=%lld clearedByOneLink=%d"), jackpotId, (int)clearedByOneLink);
	m_protocolManager->ClearInboundQueueStarvation(IProtocolManager::UIProtocol, PERSIST_CONTENT_WINDOW);
	SendOneLinkAwardStatus(AwardType::Prize, AwardToGameSuccess, MethodOfPayment::Claimable, jackpotId, 0, false, clearedByOneLink);
}

void CInternalsFramework::SendOneLinkAwardStatus(AwardType::Type awardType, EGMAwardState awardState, MethodOfPayment::Type methodPayment, int64_t jackpotId, int64_t amount, bool fromWinnerPicked, bool clearedByOneLink)
{
	CInternalMessage *msg(PrepareTransactionMessage(INT_MSG_TRANSACTION_ONELINK, IsPlayerVirtual()));
	if (CEGMAward::SetFieldsForOneLinkAwardStatus(msg, awardType, awardState, methodPayment, jackpotId, amount, fromWinnerPicked, clearedByOneLink))
	{
		SendTransactionMessageToSCF(msg, QUEUE_PRIORITY_NORMAL);
	}
	else
	{
		delete msg;
	}
}

void CInternalsFramework::ProcessSetProgressiveDown(CInternalMessage* msg)
{
	LogString(PROGRESSIVE, "Progressive Level Manager - Set progressive down");
	CProgressiveConnectionInfo* connectionInfo = msg->GetProgressiveConnectionInfo();
	m_progressiveLevelManager.ProgressiveDownClearLevels();
	m_progressiveLevelManager.SetProgressiveDown(true);
	m_progressiveCriticalText.ProcessConnectionInfo(connectionInfo);
	SendProgressiveCriticalTextToUI();
}


void CInternalsFramework::SendPayoutToOneLink(CJackpot* jackpot)
{
	CPayout* payout = new CPayout(jackpot);

	SendMessageToProgressive(new CInternalMessage(INT_MSG_PAYOUT, payout), QUEUE_PRIORITY_NORMAL);
}

/// <summary>
/// This handles non jackpot awards like OneLink bonuses
/// </summary>
/// <param name="award">The bonus award</param>
void CInternalsFramework::SendPayoutToOneLink(CEGMAward* award)
{
	CPayout* payout;
	if (award->IsAwardStateFinalOrPending())
	{
		payout = new CPayout(award->GetAwardAmount(), AllJackpots);
		SendMessageToProgressive(new CInternalMessage(INT_MSG_PAYOUT, payout), QUEUE_PRIORITY_NORMAL);
	}
}

void CInternalsFramework::ProcessFirmwarePackageAvailable(CInternalMessage* msg)
{
	CWatchDog::Instance().Checkin();
	if (nullptr != msg)
	{
		CFirmwarePackage* firmwarePackage(msg->GetFirmwarePackage());
		if (nullptr != firmwarePackage)
		{
			for(auto &firmwareFile : firmwarePackage->GetFirmwareFiles())
			{
				if (firmwareFile)
				{
					// validate the current platform
					if (CPlatform::Instance().GetPlatformName() == firmwareFile->m_platform)
					{
						std::string currentVersion;
						CFileDownloader::Filetype filetype;
						bool isIncompatibleFile = false;

						if (firmwareFile->m_filename == FIRMWARE_BUNDLE_OS)
						{
							LogString(FLASHING, "[%s] OS available to download, filename=%s, version=%s, checksum=%s",
								__FUNCTION__, firmwareFile->m_filename.c_str(), firmwareFile->m_version.c_str(), firmwareFile->m_checksum.c_str());
							currentVersion = CPlatform::Instance().GetOSImageVersionNumber();
							filetype = CFileDownloader::Filetype::FILE_TYPE_OS;
						}
						else if (firmwareFile->m_filename == FIRMWARE_BUNDLE_EXE)
						{
							LogString(FLASHING, "[%s] EXE available to download, filename=%s, version=%s, checksum=%s",
								__FUNCTION__, firmwareFile->m_filename.c_str(), firmwareFile->m_version.c_str(), firmwareFile->m_checksum.c_str());
							currentVersion = CUtilities::GetEXEVersion();
							filetype = CFileDownloader::Filetype::FILE_TYPE_EXE;
						}
						else if (firmwareFile->m_filename == FIRMWARE_BUNDLE_FPGA)
						{
							LogString(FLASHING, "[%s] FPGA available to download, filename=%s, version=%s, checksum=%s",
								__FUNCTION__, firmwareFile->m_filename.c_str(), firmwareFile->m_version.c_str(), firmwareFile->m_checksum.c_str());
							currentVersion = m_config.GetFirmwareVersion();
							filetype = CFileDownloader::Filetype::FILE_TYPE_FPGA;
						}
						else
						{
							isIncompatibleFile = true;
							LogString(FLASHING, "[%s] Incompatible file type to download, filename=%s, ignoring it.",
								__FUNCTION__, firmwareFile->m_filename.c_str());
							LogString(ERROR_LOG, "[%s] Incompatible file type to download, filename=%s, ignoring it.",
								__FUNCTION__, firmwareFile->m_filename.c_str());

							// Trim the checksum for UI, keep last 8 characters only.
							const size_t LAST_8_CHECKSUM_CHARS(8);
							const std::string checksumForUI = firmwareFile->m_checksum.substr(firmwareFile->m_checksum.size() - LAST_8_CHECKSUM_CHARS);

							// Create CFlashStatus instance to pass download status to UI.
							CFlashStatus status(FlashType::FLASH_SYSTEM, 0, checksumForUI,
								CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), DATE_TIME_MASK),
								firmwareFile->m_filename, firmwareFile->m_downloadURL);
							status.SetStatus(DOWNLOAD_STATUS_INCOMPATIBLE_FILE_TYPE);
							UpdateFirmareDownloadStatus(status);
						}

						if (!isIncompatibleFile)
						{
							if (currentVersion != firmwareFile->m_version)
							{
								LogString(FLASHING, "[%s] Adding file to download queue.", __FUNCTION__);
								CFileDownloader::Instance().AddFileToDownload(firmwareFile->m_filename,
									firmwareFile->m_downloadURL, filetype, firmwareFile->m_checksum);
							}
							else
							{
								LogString(FLASHING, "[%s] Ignoring this file due to duplicate version.", __FUNCTION__);
							}
						}
					}
				}
			}
		}
	}
}

void CInternalsFramework::ProcessConfigFileAvailable(CInternalMessage* msg)
{
	CWatchDog::Instance().Checkin();
	if (nullptr != msg)
	{
		CConfigFilePackage* configFilePackage(msg->GetConfigFilePackage());
		if (nullptr != configFilePackage)
		{
			LogString(FLASHING, "[%s] Config file available to download, filename=%s, checksum=%s",
				__FUNCTION__, configFilePackage->GetFilename().c_str(), configFilePackage->GetChecksum().c_str());
			LogString(FLASHING, "[%s] Adding file to download queue.", __FUNCTION__);
			CFileDownloader::Instance().AddFileToDownload(configFilePackage->GetFilename(), configFilePackage->GetUrl(),
				CFileDownloader::Filetype::FILE_TYPE_CONFIG, configFilePackage->GetChecksum(), configFilePackage->GetMessageId());
		}
	}
}

void CInternalsFramework::ProcessCampaignAvailable(CInternalMessage* msg)
{
	// make sure custom dynamic content path exist
	std::string path = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_CUSTOM_DYNAMIC_CONTENT);
	if (!CUtilities::DoesDirExist(path.c_str()))
	{
		const std::string mkdirCmd(FormatString("mkdir -p %s", path.c_str()));
		system(mkdirCmd.c_str());
	}

	CWatchDog::Instance().Checkin();
	if (nullptr != msg)
	{
		CCampaignPackage* campaignPackage(msg->GetCampaignPackage());
		if (nullptr != campaignPackage)
		{
			LogString(FLASHING, "[%s] Campaign available, campaignName=%s, revision=%d",
				__FUNCTION__, campaignPackage->GetName().c_str(), campaignPackage->GetRevision());

			// Get the resolution of current device.
			DWORD width = 0;
			DWORD height = 0;
			CUtilities::GetResolution(width, height);

			// Calculate media count to download
			DWORD mediaCount = 0;
			for (auto &campMediaPkg : campaignPackage->GetCampaignMediaPackages())
			{
				if (campMediaPkg->width == width && campMediaPkg->height == height)
				{
					LogString(FLASHING, "[%s] Campaign available for %u x %u resolution, name=%s",
						__FUNCTION__, width, height, campMediaPkg->name.c_str());

					for (auto &campEvent : campMediaPkg->events)
					{
						for (auto &item : campEvent->playlist.items)
						{
							if (item->isEnabled)
							{
								mediaCount++;
							}
						}
					}
				}
			}
			LogString(FLASHING, "[%s] Campaign media count to download = %u", __FUNCTION__, mediaCount);

			if (mediaCount > 0)
			{
				CMediaPackageManager::Instance().SetPendingCampaignInfo(
					campaignPackage->GetId(), campaignPackage->GetName(), campaignPackage->GetRevision(), mediaCount);

				// Cleanup pending campaign directory
				CUtilities::DeleteDirectory(PENDING_CAMPAIGN_PATH);
				CUtilities::CreateDirectory(PENDING_CAMPAIGN_PATH);

				// purge campaign media if any in FileDownloader queue.
				CFileDownloader::Instance().PurgeFilesToDownload(CFileDownloader::Filetype::FILE_TYPE_CAMPAIGN_MEDIA);

				for (auto &campMediaPkg : campaignPackage->GetCampaignMediaPackages())
				{
					if (campMediaPkg->width == width && campMediaPkg->height == height)
					{
						for (auto &campEvent : campMediaPkg->events)
						{
							LogString(FLASHING, "[%s] %s event available", __FUNCTION__, campEvent->name.c_str());
							ProcessCampaignEventInfo(*campEvent);
						}
					}
				}
			}
		}
	}
}

void CInternalsFramework::ProcessCampaignEventInfo(CampaignEvent &campEvent) const
{
	const std::string targetPath = PENDING_CAMPAIGN_PATH + "/" + campEvent.name;
	CUtilities::CreateDirectory(targetPath);

	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	bool enabledItemFoundInPlaylist = false;

	writer.StartObject();
	writer.Key("attractorSequenceItems");
	{
		writer.StartArray();
		for (auto &playlistItem : campEvent.playlist.items)
		{
			if (playlistItem->isEnabled)
			{
				enabledItemFoundInPlaylist = true;
				writer.StartObject();
				writer.Key("sequenceId");		writer.Int64(playlistItem->sequenceId);
				writer.Key("url");				writer.String(playlistItem->filename);
				writer.Key("duration");			writer.Int64(playlistItem->duration);
				writer.Key("activeDays");		writer.Int64(playlistItem->activeDays);
				writer.Key("mute");				writer.Bool(playlistItem->isMute);
				writer.Key("beginDate");		writer.String(playlistItem->beginDate);
				writer.Key("beginTime");		writer.String(playlistItem->beginTime);
				writer.Key("endDate");			writer.String(playlistItem->endDate);
				writer.Key("endTime");			writer.String(playlistItem->endTime);

				writer.EndObject();

				// download the media
				LogString(FLASHING, "[%s] Campaign media to download, filename=%s, checksum=%s",
					__FUNCTION__, playlistItem->filename.c_str(), playlistItem->checksum.c_str());
				LogString(FLASHING, "[%s] Adding file to download queue.", __FUNCTION__);
				CFileDownloader::Instance().AddFileToDownload(playlistItem->filename, playlistItem->url,
					CFileDownloader::Filetype::FILE_TYPE_CAMPAIGN_MEDIA, playlistItem->checksum, targetPath);
			}
		}

		writer.EndArray();
	}

	writer.Key("textOverlayItems");
	{
		writer.StartArray();
		writer.EndArray();
	}
	writer.EndObject();

	if (enabledItemFoundInPlaylist)
	{
		// write JSON to file
		const std::string filename = targetPath + "/Spec.json";
		ofstream file(filename);
		file << writeStream.m_str;
		file.close();
	}
}

void CInternalsFramework::PendingCampaignMediaDownloaded()
{
	if (CMediaPackageManager::Instance().PendingCampaignMediaDownloaded())
	{
		// inform UI for campaign available
		LogString(FLASHING, "[%s] Sending INT_MSG_CAMPAIGN_AVAILABLE to UI.", __FUNCTION__);
		SendMessageToUI(new CInternalMessage(INT_MSG_CAMPAIGN_AVAILABLE), QUEUE_PRIORITY_NORMAL);

		CPackageInfo info = CMediaPackageManager::Instance().GetActiveCampaignInfo();
		m_sentinelState.SetCampaignInfo(info.GetId(), info.GetName(), info.GetRevision());
		SendDashboardStatusToProgressive();
		SendSubscriptionUpdateIfNecessary();
		LaunchCampaignMemoryLogIfReqd();
		LaunchCampaignCpuLogIfReqd();
	}
}

void CInternalsFramework::UpdateFirmareDownloadStatus(CFlashStatus status)
{
	m_sentinelState.SetDownloadStatus(status);
	SendSubscriptionUpdateIfNecessary();
}

void CInternalsFramework::ProcessSkinAvailable(CInternalMessage* msg)
{
	CWatchDog::Instance().Checkin();
	if (nullptr != msg)
	{
		CSkinPackage* skinPackage(msg->GetSkinPackage());
		if (nullptr != skinPackage)
		{
			LogString(FLASHING, "[%s] Skin available, skinName=%s, version=%s",
				__FUNCTION__, skinPackage->GetName().c_str(), skinPackage->GetVersion().c_str());

			// Calculate media count to download (skin.json, language files, media files)
			DWORD mediaCount = 1 + skinPackage->GetLanguagePacks().size() + skinPackage->GetFiles().size();
			CMediaPackageManager::Instance().SetPendingSkinInfo(
				skinPackage->GetId(), skinPackage->GetName(), skinPackage->GetVersion(), skinPackage->GetRevision(), mediaCount);

			const std::string assetsPath(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_ASSETS) + "/");
			const std::string customAssetsPath(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_CUSTOM_ASSETS) + "/");
			if (!CUtilities::DoesDirExist(customAssetsPath.c_str()))
			{
				CUtilities::CreateDirectory(customAssetsPath);
			}

			// Cleanup pending skin directory
			CUtilities::DeleteDirectory(PENDING_SKIN_PATH);
			CUtilities::CreateDirectory(PENDING_SKIN_PATH);

			// purge skin media if any in FileDownloader queue.
			CFileDownloader::Instance().PurgeFilesToDownload(CFileDownloader::Filetype::FILE_TYPE_SKIN);

			// compare the checksum with existing skin file, and if it matches then don't download, just copy it.
			const std::string skinName("Skin.json");
			const std::string existingSkinFile(assetsPath + skinName);
			if (CUtilities::DoesFileExist(existingSkinFile) &&
				skinPackage->GetSkinChecksum() == CFileDownloader::Instance().GetChecksum(CFileDownloader::Filetype::FILE_TYPE_SKIN, existingSkinFile))
			{
				// copy the file
				LogString(FLASHING, "[%s] Duplicate skin file found, copying it from existing assets, filename=%s",
					__FUNCTION__, skinName.c_str());
				CUtilities::CopyFile(existingSkinFile, PENDING_SKIN_PATH);

				// making call to decrement the media count.
				PendingSkinMediaDownloaded();
			}
			else
			{
				// add file to download queue.
				LogString(FLASHING, "[%s] Adding skin file to download queue, filename=%s", __FUNCTION__, skinName.c_str());
				CFileDownloader::Instance().AddFileToDownload(skinName, skinPackage->GetSkinUrl(),
					CFileDownloader::Filetype::FILE_TYPE_SKIN, skinPackage->GetSkinChecksum(), PENDING_SKIN_PATH);
			}

			const std::string languagePath(PENDING_SKIN_PATH + "i18n/");
			CUtilities::CreateDirectory(languagePath);

			for (auto &languagePack : skinPackage->GetLanguagePacks())
			{
				const std::string languageFilename(languagePack->code + ".json");

				// compare the checksum with existing language file, and if it matches then don't download, just copy it.
				const std::string existingLanguageFile(assetsPath + "i18n/" + languageFilename);
				if (CUtilities::DoesFileExist(existingLanguageFile) &&
					languagePack->checksum == CFileDownloader::Instance().GetChecksum(CFileDownloader::Filetype::FILE_TYPE_SKIN, existingLanguageFile))
				{
					// copy the file
					LogString(FLASHING, "[%s] Duplicate language file found, copying it from existing assets, filename=%s",
						__FUNCTION__, languageFilename.c_str());
					CUtilities::CopyFile(existingLanguageFile, PENDING_SKIN_PATH);

					// making call to decrement the media count.
					PendingSkinMediaDownloaded();
				}
				else
				{
					// add file to download queue.
					LogString(FLASHING, "[%s] Adding language file to download queue, filename=%s", __FUNCTION__, languageFilename.c_str());
					CFileDownloader::Instance().AddFileToDownload(languageFilename, languagePack->url,
						CFileDownloader::Filetype::FILE_TYPE_SKIN, languagePack->checksum, languagePath);
				}
				CWatchDog::Instance().Checkin();
			}

			for (auto &file : skinPackage->GetFiles())
			{
				// compare the checksum with existing media file, and if it matches then don't download, just copy it.
				const std::string existingMediaFile(assetsPath + file->filename);
				if (CUtilities::DoesFileExist(existingMediaFile) &&
					file->checksum == CFileDownloader::Instance().GetChecksum(CFileDownloader::Filetype::FILE_TYPE_SKIN, existingMediaFile))
				{
					// check for NCD file
					const std::string ncdStr(".ncd");
					const std::string endStr(file->filename.substr(file->filename.size() - ncdStr.size(), ncdStr.size()));
					if (endStr == ncdStr)
					{
						// skip the copy of NCD file.
						LogString(FLASHING, "[%s] Duplicate NCD media found, skipping the copy from existing assets, filename=%s",
							__FUNCTION__, file->filename.c_str());
					}
					else
					{
						// copy the file
						LogString(FLASHING, "[%s] Duplicate media found, copying it from existing assets, filename=%s",
							__FUNCTION__, file->filename.c_str());
						CUtilities::CopyFile(existingMediaFile, PENDING_SKIN_PATH);
					}

					// making call to decrement the media count.
					PendingSkinMediaDownloaded();
				}
				else
				{
					// add file to download queue.
					LogString(FLASHING, "[%s] Adding file to download queue, filename=%s", __FUNCTION__, file->filename.c_str());
					CFileDownloader::Instance().AddFileToDownload(file->filename, file->url,
						CFileDownloader::Filetype::FILE_TYPE_SKIN, file->checksum, PENDING_SKIN_PATH);
				}
				CWatchDog::Instance().Checkin();
			}
		}
	}
}

void CInternalsFramework::ProcessScheduleEnd(CInternalMessage* msg)
{
	if (NULL != msg)
	{
		lock_guard<recursive_mutex> csLockEgmAward(m_egmAwardCriticalSection);
		lock_guard<recursive_mutex> csLockPlayer(m_playerCriticalSection);
		int64_t levelId(msg->GetInt64Data());
		string levelName(msg->GetStringData());
		LogString(PROGRESSIVE, "Received ScheduleEnd levelId=%lld levelName=%s", levelId, levelName.c_str());
		DWORD displayDuration(DEFAULT_SCHEDULE_END_DURATION_IN_MS);
		string dynamicContentPath(m_mediaPackageManager->GetDynamicContentPath(levelId, "", SlotsContentType::ScheduleEnd, pKeyLcdContent));
		if (!dynamicContentPath.empty())
		{
			displayDuration = MAX_CUSTOM_CONTENT_DURATION_SCHD_END_IN_MS;
		}
		CScheduleEndCommand command(displayDuration, dynamicContentPath, levelName);
		msg->SetData(command.getJSON());
		int priority(0);
		msg->SetMessagePersist(m_msgPriority.PrioritizeMessage(msg, priority));
		SendMessageToUI(msg, priority);
	}
}

void CInternalsFramework::PendingSkinMediaDownloaded()
{
	if (CMediaPackageManager::Instance().PendingSkinMediaDownloaded())
	{
		// All pending skin and related assets are downloaded. Set RefreshUI as pending action.
		// We will exchange pending skin with active skin just before sending RefreshUI command
		// to the UI when conditions are suitable.
		m_sentinelState.SetPendingAction(RefreshUI);
		SendDashboardStatusToProgressive();
		m_restartManager.SetPerformUiRefreshAsapFlag(true);
		SendSubscriptionUpdateIfNecessary();
	}
}
