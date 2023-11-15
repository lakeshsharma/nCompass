#pragma once
#include "../SystemProtocol.h"
#include "ProgressiveAnnouncementsInfo.h"
#include "ProgressiveDataConsumer.h"
#include "ProgressiveConnection.h"
#include "ProgressiveWinnerPick.h"
#include "ProgressiveWinnerPicked.h"
#include "ProgressiveLevelsInfo.h"
#include "ProgressiveJackpotAnnouncement.h"
#include "ProgressiveTotalizerAnnouncement.h"
#include "CampaignConfig.h"
#include "SkinConfig.h"
#include <stdafx.h>

namespace SentinelNativeUnitTests { class ProgressiveProtocol; }
namespace SentinelNativeUnitTests { class ProgressiveProtocol_Connection; }

/// <summary>
/// The Progressive protocol class that inherits from SystemProtocol.
/// This is the progressive module for nCompasss that interfaces with InternalsFramework.
/// </summary>
class CProgressiveProtocol :
	public CSystemProtocol, protected CProgressiveDataConsumer
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CProgressiveProtocol"/> class.
	/// </summary>
	/// <param name="scfToInternalsQueue">The SCF to internals queue.</param>
	/// <param name="firmwareVersion">IN - The firmware version.</param>
	/// <param name="operatingSystem">IN/OUT - The operating system.</param>
	CProgressiveProtocol(CQueueList &scfToInternalsQueue,
		const std::string &firmwareVersion = "",
		IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CProgressiveProtocol"/> class.
	/// </summary>
	virtual ~CProgressiveProtocol();

	/// <summary>
	/// Initializer.  Intended to be called immediately after the constructor.
	/// </summary>
	virtual void Init();

	/// <summary>
	/// Processes the message from internals.
	/// </summary>
	/// <param name="msg">The message to be processed</param>
	virtual void ProcessMessageFromInternals(CInternalMessage *msg);

	/// <summary>
	/// Called to perform timed event processing.
	/// </summary>
	virtual void PerformTimedEvents();

// #pragma region CProgressiveDataConsumer
	/// <summary>
	/// Send message to internals.
	/// </summary>
	/// <param name="pMsg">The message.</param>
	/// <param name="priority">The priority to use when sending the message.</param>
	void SendMsgToInternals(CInternalMessage* pMsg, int priority = QUEUE_PRIORITY_NORMAL);
	
	/// <summary>
	/// Queue a message to be sent to OneLink.
	/// </summary>
	/// <param name="pMsg">The message.</param>
	/// <param name="priority">The priority to use when sending the message.</param>
	void QueueToOneLink(ProgressiveQueueToOneLink* pMsg, int priority = QUEUE_PRIORITY_HIGH);

	/// <summary>
	/// Receives the connection updates.
	/// </summary>
	/// <param name="connectionInfo">IN - The current connection information</param>
	virtual void ConnectionUpdate(const CProgressiveConnectionInfo &connectionInfo);

	/// <summary>
	/// Sends a progressive Hit Status Update to the internals for EGM award manager.
	/// </summary>
	/// <param name="award">The award.</param>
	/// <param name="wasSent">whether or not the code was able to send the Hit to onelink.</param>
	void ProgressiveHitSentStatusUpdate(const CEGMAward &award, bool wasSent);

	/// <summary>
	/// Sends a progressive Jackpot Paid Status Update to the internals for EGM award manager.
	/// </summary>
	/// <param name="award">The award.</param>
	/// <param name="wasSent">whether or not the code was able to send the Paid Status to onelink.</param>
	void SendJackpotPaidRequestStatusUpdate(const CEGMAward &award, bool wasSent);

	/// <summary>
	/// This function receives updates and changes to the current progressive host.
	/// </summary>
	/// <param name="connectionInfo">IN - The current host information. NULL indicates there is no current host.</param>
	virtual void HostUpdate(const CProgressiveHost* host);

	/// <summary>
	/// This function receives requests for progressive game info.
	/// </summary>
	virtual void GetProgressiveGameInfo();

	/// <summary>
	/// This function receives jackpot hit events and forwards them to internals.
	/// </summary>
	/// <param name="egmAward">IN - The jackpot hit information.</param>
	/// <remarks>
	/// This method is responsible for consuming the *egmAward instance.
	/// The caller MUST NOT access the instance after calling this method.
	/// </remarks>
	virtual void JackpotHit(CEGMAward *egmAward);

	/// <summary>
	/// This function receives Progressive Jackpot Announcement events and forwards them to internals.
	/// </summary>
	/// <param name="egmAward">IN - The progressive announcement information.</param>
	virtual void ProgressiveJackpotAnnouncement(CProgressiveJackpotAnnouncement* pAnnouncement);

	/// <summary>
	/// This function receives Progressive Totalizer Announcement events and forwards them to internals.
	/// </summary>
	/// <param name="egmAward">IN - The progressive announcement information.</param>
	virtual void ProgressiveTotalizerAnnouncement(CProgressiveTotalizerAnnouncement* pAnnouncement);

	/// <summary>
	/// This function receives claimable award messages and forwards them to internals.
	/// </summary>
	/// <param name="pClaimableAward">IN - The claimable award information.</param>
	/// <remarks>
	/// This method is responsible for consuming the *pClaimableAward instance.
	/// The caller MUST NOT access the instance after calling this method.
	/// </remarks>
	void ClaimableAward(CProgressiveClaimableAward* pClaimableAward);

	/// <summary>
	/// This function receives level broadcasts and forwards them to internals.
	/// </summary>
	/// <param name="pLevelBroadcast">IN - The level broadcast information.</param>
	/// <remarks>
	/// This method is responsible for consuming the *pLevelBroadcast instance.
	/// The caller MUST NOT access the instance after calling this method.
	/// </remarks>
	virtual void ProgressiveLevelBroadcast(CProgressiveLevelBroadcast* pLevelBroadcast);

	/// <summary>
	/// This function receives level and announcement information and forwards them to internals.
	/// </summary>
	/// <param name="pProgressiveLevelsInfo">IN - The level information.</param>
	/// <param name="pProgressiveAnnouncementsInfo">IN - The announcements information.</param>
	/// <param name="pFirmwarePackage">IN - The Firmware configuration and files.</param>
	/// <param name="pCampaignConfig">IN - The Campaign configuration.</param>
	/// <param name="pSkinConfig">IN - The Skin configuration.</param>
	/// <remarks>
	/// This method is responsible for consuming the *pProgressiveLevelConfiguration instance.
	/// The caller MUST NOT access the instance after calling this method.
	/// </remarks>
	virtual void SendProgressiveConfiguration(ProgressiveLevelsInfo* pProgressiveLevelsInfo, CProgressiveAnnouncementsInfo* pProgressiveAnnouncementsInfo, CFirmwarePackage* pFirmwarePackage, CCampaignConfig* pCampaignConfig, CSkinConfig* pSkinConfig);

	/// <summary>
	/// This function send remote reboot command to internals
	/// </summary>
	virtual void SendRemoteReboot();

	/// <summary>
	/// This function sends promote game request to internals
	/// </summary>
	virtual void SendPromoGameRequest(CPromoGameRequest* promoGameRequest);

	/// <summary>
	/// This function sends firmware download request to internals
	/// </summary>
	virtual void SendFirmwareDownload(CFirmwarePackage* firmwarePackage);

	/// <summary>
	/// This function sends config file download request to internals
	/// </summary>
	virtual void SendConfigFileDownload(CConfigFilePackage* configFilePackage);

	/// <summary>
	/// This function sends campaign download request to internals
	/// </summary>
	virtual void SendCampaignDownload(CCampaignPackage* campaignPackage);

	/// <summary>
	/// This function sends skin download request to internals
	/// </summary>
	virtual void SendSkinDownload(CSkinPackage* skinPackage);

	/// <summary>
	/// This function sends skin download request to internals
	/// </summary>
	/// <param name="levelId">IN - level Id that ended.</param>
	/// <param name="levelName">IN - level name that ended.</param>
	virtual void SendScheduleEnd(int64_t levelId, const std::string& levelName);

	/// <summary>
	/// This function receives progressive bonusing available info.
	/// </summary>
	/// <param name="progresiveBonusingAvailable">IN - The progressive bonusing available info.</param>
	/// <param name="progressiveEligibleLevelsInfo">IN - The progressive eligible levels info.</param>
	/// <param name="progressivePatronSessionInfo">IN - The progressive patron session info.</param>
	/// <remarks>
	/// This method is responsible for consuming the *progresiveBonusingAvailable instance.
	/// The caller MUST NOT access the instance after calling this method.
	/// </remarks>
	virtual void ProgressiveBonusingAvailable(CProgressiveBonusingAvailable *progresiveBonusingAvailable, 
		CProgressiveEligibleLevelsInfo *progressiveEligibleLevelsInfo, 
		ProgressivePatronSessionInfo *progressivePatronSessionInfo);

	/// <summary>
	/// Receives winner pick messages and forwards them to internals.
	/// </summary>
	/// <param name="pWinnerPick">IN - The winner pick information.</param>
	/// <remarks>
	/// This method is responsible for consuming the *pWinnerPick instance.
	/// The caller MUST NOT access the instance after calling this method.
	/// </remarks>
	virtual void ProgressiveWinnerPick(CProgressiveWinnerPick* pWinnerPick);

	/// <summary>
	/// Receives winner picked messages and forwards them to internals.
	/// </summary>
	/// <param name="pWinnerPicked">IN - The winner picked information.</param>
	/// <remarks>
	/// This method is responsible for consuming the *pWinnerPicked instance.
	/// The caller MUST NOT access the instance after calling this method.
	/// </remarks>
	virtual void ProgressiveWinnerPicked(CProgressiveWinnerPicked* pWinnerPicked);
// #pragma endregion CProgressiveDataConsumer

protected:
	/// <summary>
	/// Get a new progressive connection instance (to One Link).
	/// </summary>
	/// <param name="dataConsumer">IN/OUT - The data consumer.</param>
	/// <param name="progConfig">IN/OUT - The CProgressiveConfig.</param>
	/// <param name="paltronicsCertificatePath">IN - The Paltronics certificate path.</param>
	/// <param name="operatingSystem">IN/OUT - An interface to operating system functionality.</param>
	/// <remarks>
	/// May be overridden for testing.
	/// </remarks>
	    virtual CProgressiveConnection *GetProgressiveConnection(CProgressiveDataConsumer &dataConsumer,
		CProgressiveConfig *progConfig,
		const std::string &paltronicsCertificatePathname,
		IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

private:
	static const DWORD TIMED_EVENTS_TIMEOUT = 100; // In milliseconds.

	virtual void RefreshConnection();
	virtual CProgressiveConfig* LoadConfigFile();
	void ProcessDefaultDataDirectoryUpdated(CInternalMessage *msg);
	void ProcessSetGamesInfo(CInternalMessage *msg);
	void ProcessSetConnectionFromInternals(CInternalMessage *msg);
	void ProcessSetOneLinkProgressivesSupportFromInternals(CInternalMessage *msg);
	void ProcessSetProgressiveConfigFromInternals(CInternalMessage *msg);
	void ProcessExternalCommunicationsStatus(CInternalMessage *msg);
	void ProcessGameStartNotification(CInternalMessage *msg);
	void ProcessGameEndNotification(CInternalMessage *msg);
	void ProcessGameData(CInternalMessage *msg);
	void ProcessPlayerData(CInternalMessage *msg);
	void ProcessProgressiveJackpotPaid(CInternalMessage* msg);
	void ProcessGeneralPollException(CInternalMessage* msg);
	void ProcessOfflineCoinIn(CInternalMessage *msg);
	void ProcessSetCurrentDenom(CInternalMessage *msg);
	void ProcessQueueToOneLink(CInternalMessage* pMsg);
	void EgmStateRequest(bool egmStateRequest, int64_t jackpotId);
	void ProcessEgmStateResponse(CInternalMessage* msg);
	void ProcessPromoGameComplete(CInternalMessage* pMsg);
	void ProcessPayout(CInternalMessage* msg);
	void ProcessCampaignDownloadRequest(CInternalMessage* msg);
	void ProcessSkinDownloadRequest(CInternalMessage* msg);
	void ProcessProgressiveDashboardStatus(CInternalMessage* msg);
    
	bool IsActive();

	/// <summary>
	/// Processes the progressive win message from Internals.
	/// </summary>
	/// <param name="msg">IN/OUT - The MSG.</param>
	/// <remarks>
	/// This method is responsible for sending the progressive win to onelink
	/// </remarks>
	void ProcessProgressiveWin(CInternalMessage *msg);

	CProgressiveConnection *m_progressiveConnection;

	bool m_configFileActive;
	bool m_oneLinkProgressivesSenOptionEnabled;
	bool m_oneLinkProgressivesSupportedByEGM;

	std::string m_firmwareVersion;

	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::ProgressiveProtocol;
	friend class SentinelNativeUnitTests::ProgressiveProtocol_Connection;
};


