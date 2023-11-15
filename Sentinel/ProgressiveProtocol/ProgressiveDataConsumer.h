#pragma once

#include "EGMAward.h"
#include "ProgressiveBonusingAvailable.h"
#include "ProgressiveClaimableAward.h"
#include "ProgressiveConnectionInfo.h"
#include "ProgressiveHost.h"
#include "ProgressiveAnnouncementInfo.h"
#include "ProgressiveLevelsInfo.h"
#include "ProgressiveWinnerPick.h"
#include "ProgressiveWinnerPicked.h"
#include "PromoGameRequest.h"
#include "FirmwarePackage.h"
#include "ConfigFilePackage.h"
#include "CampaignPackage.h"
#include "QueueList.h"

class CInternalMessage;
class ProgressiveQueueToOneLink;

/// <summary>
/// abstract class.
/// Interface for a progressive data consumer.
/// Class must inherit this interface to be a receiver of incoming progressive data.
/// </summary>
class CProgressiveDataConsumer
{
public:
	/// <summary>
	/// Send message to internals.
	/// </summary>
	/// <param name="pMsg">The message.</param>
	/// <param name="priority">The priority to use when sending the message.</param>
	virtual void SendMsgToInternals(CInternalMessage* pMsg, int priority = QUEUE_PRIORITY_NORMAL) = 0;

	/// <summary>
	/// Queue a message to be sent to OneLink.
	/// </summary>
	/// <param name="pMsg">The message.</param>
	/// <param name="priority">The priority to use when sending the message.</param>
	virtual void QueueToOneLink(ProgressiveQueueToOneLink* pMsg, int priority = QUEUE_PRIORITY_HIGH) = 0;

	/// <summary>
	/// This function receives updates to the progressive connection info.
	/// </summary>
	/// <param name="connectionInfo">IN - The current connection information.</param>
	virtual void ConnectionUpdate(const CProgressiveConnectionInfo &connectionInfo) = 0;

	/// <summary>
	/// Used to send the status update of the hit back to internals.
	/// </summary>
	/// <param name="award">The award.</param>
	/// <param name="wasSent">if set to <c>true</c> [was sent].</param>
	virtual void ProgressiveHitSentStatusUpdate(const CEGMAward &award, bool wasSent) = 0;

	/// <summary>
	/// Sends a progressive Jackpot Paid Status Update to the internals for EGM award manager.
	/// </summary>
	/// <param name="award">The award.</param>
	/// <param name="wasSent">whether or not the code was able to send the Paid Status to onelink.</param>
	virtual void SendJackpotPaidRequestStatusUpdate(const CEGMAward &award, bool wasSent) = 0;

	/// <summary>
	/// This function receives updates and changes to the current progressive host.
	/// </summary>
	/// <param name="connectionInfo">IN - The current host information. NULL indicates there is no current host.</param>
	virtual void HostUpdate(const CProgressiveHost* host) = 0;

	/// <summary>
	/// This function receives requests for progressive game info.
	/// </summary>
	virtual void GetProgressiveGameInfo() = 0;

	/// <summary>
	/// This function receives jackpot hit events.
	/// </summary>
	/// <param name="egmAward">IN - The jackpot hit information.</param>
	/// <remarks>
	/// This method is responsible for consuming the *egmAward instance.
	/// The caller MUST NOT access the instance after calling this method.
	/// </remarks>
	virtual void JackpotHit(CEGMAward *egmAward) = 0;

	/// <summary>
	/// This function sends remote reboot command to internals.
	/// </summary>
	virtual void SendRemoteReboot() = 0;

	/// <summary>
	/// This function sends promote game request to internals
	/// </summary>
	virtual void SendPromoGameRequest(CPromoGameRequest* promoGameRequest) = 0;

	/// <summary>
	/// This function sends firmware download request to internals
	/// </summary>
	virtual void SendFirmwareDownload(CFirmwarePackage* firmwarePackage) = 0;

	/// <summary>
	/// This function receives jackpot announcement messages and forwards them to internals.
	/// </summary>
	/// <param name="pAnnouncement">IN - The announcement information.</param>
	virtual void ProgressiveJackpotAnnouncement(CProgressiveJackpotAnnouncement* pAnnouncement) = 0;

	/// <summary>
	/// This function receives totalizer announcement messages and forwards them to internals.
	/// </summary>
	/// <param name="pAnnouncement">IN - The announcement information.</param>
	virtual void ProgressiveTotalizerAnnouncement(CProgressiveTotalizerAnnouncement* pAnnouncement) = 0;

    /// <summary>
	/// This function sends config file download request to internals
	/// </summary>
	virtual void SendConfigFileDownload(CConfigFilePackage* configFilePackage) = 0;

	/// <summary>
	/// This function sends campaign download request to internals
	/// </summary>
	virtual void SendCampaignDownload(CCampaignPackage* campaignPackage) = 0;

	/// <summary>
	/// This function sends skin download request to internals
	/// </summary>
	virtual void SendSkinDownload(CSkinPackage* skinPackage) = 0;

	/// <summary>
	/// This function sends skin download request to internals
	/// </summary>
	/// <param name="levelId">IN - level Id that ended.</param>
	/// <param name="levelName">IN - level name that ended.</param>
	virtual void SendScheduleEnd(int64_t levelId, const std::string& levelName) = 0;

	/// <summary>
	/// This function receives claimable award messages and forwards them to internals.
	/// </summary>
	/// <param name="pClaimableAward">IN - The claimable award information.</param>
	/// <remarks>
	/// This method is responsible for consuming the *pClaimableAward instance.
	/// The caller MUST NOT access the instance after calling this method.
	/// </remarks>
	virtual void ClaimableAward(CProgressiveClaimableAward* pClaimableAward) = 0;

	/// <summary>
	/// This function receives level broadcasts and forwards them to internals.
	/// </summary>
	/// <param name="pLevelBroadcast">IN - The level broadcast information.</param>
	/// <remarks>
	/// This method is responsible for consuming the *pLevelBroadcast instance.
	/// The caller MUST NOT access the instance after calling this method.
	/// </remarks>
	virtual void ProgressiveLevelBroadcast(CProgressiveLevelBroadcast* pLevelBroadcast) = 0;

	/// <summary>
	/// This function receives level and announcement information and forwards them to internals.
	/// </summary>
	/// <param name="pProgressiveLevelsInfo">IN - The level information.</param>
	/// <param name="pAnnouncmentsConfiguration">IN - The announcements information.</param>
	/// <param name="pFirmwarePackage">IN - The Firmware configuration and files.</param>
	/// <param name="pCampaignConfig">IN - The Campaign configuration.</param>
	/// <param name="pSkinConfig">IN - The Skin configuration.</param>
	/// <remarks>
	/// This method is responsible for consuming the *pProgressiveLevelConfiguration instance.
	/// The caller MUST NOT access the instance after calling this method.
	/// </remarks>
	virtual void SendProgressiveConfiguration(ProgressiveLevelsInfo* pProgressiveLevelsInfo, CProgressiveAnnouncementsInfo* pAnnouncmentsConfiguration, CFirmwarePackage* pFirmwarePackage, CCampaignConfig* pCampaignConfig, CSkinConfig* pSkinConfig) = 0;

	/// <summary>
	/// This function receives progressive bonusing available info.
	/// </summary>
	/// <param name="progresiveBonusingAvailable">IN - The progressive bonusing available info.</param>
	/// <remarks>
	/// This method is responsible for consuming the *progresiveBonusingAvailable instance.
	/// The caller MUST NOT access the instance after calling this method.
	/// </remarks>
	virtual void ProgressiveBonusingAvailable(CProgressiveBonusingAvailable *progresiveBonusingAvailable, CProgressiveEligibleLevelsInfo *progressiveEligibleLevelsInfo, ProgressivePatronSessionInfo *progressivePatronSessionInfo) = 0;

	/// <summary>
	/// Receives winner pick messages and forwards them to internals.
	/// </summary>
	/// <param name="pWinnerPick">Pointer to CProgressiveWinnerPick instance.</param>
	/// <remarks>
	/// This method is responsible for consuming the *pWinnerPick instance.
	/// The caller MUST NOT access the instance after calling this method.
	/// </remarks>
	virtual void ProgressiveWinnerPick(CProgressiveWinnerPick* pWinnerPick) = 0;

	/// <summary>
	/// Receives winner picked messages and forwards them to internals.
	/// </summary>
	/// <param name="pWinnerPicked">Pointer to CProgressiveWinnerPicked instance.</param>
	/// <remarks>
	/// This method is responsible for consuming the *pWinnerPicked instance.
	/// The caller MUST NOT access the instance after calling this method.
	/// </remarks>
	virtual void ProgressiveWinnerPicked(CProgressiveWinnerPicked* pWinnerPicked) = 0;

	/// <summary> Request to enable/disable game. 
	/// </summary>
	/// <param name="egmStateRequest">Requested EGM state</param> 
	/// <param name="jackpotId">Jackpot ID</param>
	/// <remarks>
	/// This method is responsible for consuming the egm state 
	/// request. Currently only "true" will be processed. 
	/// </remarks>
	virtual void EgmStateRequest(bool egmStateRequest, int64_t jackpotId) = 0;
};
