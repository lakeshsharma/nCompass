#pragma once
#include "PollerProtocol/PollerProtocol.h"
#include "ProgressiveProtocol/ProgressiveConnectionInfo.h"
#include "ProgressiveProtocol/ProgressiveHost.h"
#include "TiltsDef.h"
#include "DataSubscriptionProvider.h"
#include "metersdef.h"
#include "Card.h"
#include "FlashStatus.h"
#include "Games.h"
#include "CurrentActivity.h"
#include "PendingAction.h"
#include "DirectoryManager.h"


struct tDiskSpace
{
	std::string m_fileSystem;
	std::string m_size;
	std::string m_available;
	std::string m_used;

	tDiskSpace(const std::string& fileSystem, const std::string& size, const std::string& available, const std::string& used)
    : m_fileSystem(fileSystem)
      , m_size(size)
      , m_available(available)
      , m_used(used)
	{
	}
};

// Forward declare class to declare friend for unit test access.
namespace SentinelNativeUnitTests { class SentinelState; }


/// <summary>
/// Sentinel state and status information.
/// </summary>
class CSentinelState : public CDataSubscriptionProvider
{
public:
	/// <summary>
	/// Constructor. Initializes a new instance of the <see cref="CSentinelState"/> class.
	/// </summary>
	/// <param name="startupTime">IN - The local time that the Service Window UI was last restarted.</param>
	/// <param name="sentinelVersion">IN - The application version number string.</param>
	CSentinelState(const time_t &startTime, const std::string& sentinelVersion);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CSentinelState"/> class.
	/// </summary>
	virtual ~CSentinelState();

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

	/// <summary>
	/// Sets the progressive configuration.
	/// </summary>
	/// <param name="progressiveConfig">IN - The progressive configuration.</param>
	void SetProgressiveConfig(const CProgressiveConfig &progressiveConfig);

	/// <summary>
	/// Gets a copy of the progressive configuration.
	/// </summary>
	/// <returns>A pointer to a new CProgressiveConfig instance.  MUST NOT return NULL.</returns>
	/// <remarks>
	/// The caller is responsible for deleting the returned instance.
	/// </remarks>
	CProgressiveConfig* GetProgressiveConfigCopy() const;

	/// <summary>
	/// Sets the progressive connection information.
	/// </summary>
	/// <param name="progressiveConfig">IN - The progressive connection information.</param>
	void SetProgressiveConnectionInfo(const CProgressiveConnectionInfo &progressiveConnectionInfo);

	/// <summary>
	/// Gets a copy of the progressive connection info.
	/// </summary>
	/// <returns>A pointer to a new CProgressiveConnectionInfo instance.  MUST NOT return NULL.</returns>
	/// <remarks>
	/// The caller is responsible for deleting the returned instance.
	/// </remarks>
	CProgressiveConnectionInfo* GetProgressiveConnectionInfoCopy() const;

	/// <summary>
	/// Sets the progressive host information.
	/// </summary>
	/// <param name="progressiveHost">IN - The progressive host information.</param>
	void SetProgressiveHost(const CProgressiveHost &progressiveHost);

	/// <summary>
	/// Gets a copy of the progressive host information.
	/// </summary>
	/// <returns>A pointer to a new CProgressiveHost instance.  MUST NOT return NULL.</returns>
	/// <remarks>
	/// The caller is responsible for deleting the returned instance.
	/// </remarks>
	CProgressiveHost* GetProgressiveHostCopy() const;

	/// <summary>
	/// Sets the pending action.
	/// </summary>
	/// <param name="pendingAction">The pending action.</param>
	void SetPendingAction(PendingAction pendingAction);

	/// <summary>
	/// Gets the pending action.
	/// </summary>
	PendingAction GetPendingAction();

	 /// <summary>
	 /// Gets the pending action.
	 /// </summary>
	 /// <returns>returns the pending action text representative.</returns>
	 std::string GetPendingActionAsString() const;

	 /// <summary>
	 /// Clears the pending action.
	 /// </summary>
	 /// <param name="pendingAction">IN - The pending action.</param>
	 void ClearPendingAction(PendingAction pendingAction);

	 /// <summary>
	 /// Sets the current activity.
	 /// </summary>
	 /// <param name="currentActivity">The current activity.</param>
	 void SetCurrentActivity(CurrentActivity currentActivity);

	 /// <summary>
	 /// Gets the current activity.
	 /// </summary>
	 CurrentActivity GetCurrentActivity();

	 /// <summary>
	 /// Clears the current activity.
	 /// </summary>
	 /// <param name="currentActivity">IN - The current activity.</param>
	 void ClearCurrentActivity(CurrentActivity currentActivity);

	 /// <summary>
	 /// Sets the door open flag.
	 /// </summary>
	 /// <param name="value">if set to <c>true</c> [value].</param>
	 /// <summary>
	 void SetDoorOpen(bool value);

	 /// <summary>
	/// Sets the last coin played value.
	/// </summary>
	/// <param name="lastCoinsPlayed">last coin played value</param>
	/// <summary>
	void SetLastCoinsPlayed(int lastCoinsPlayed);

	/// <summary>
	/// Sets the reported EGM protocol version..
	/// </summary>
	/// <param name="reportedEgmProtocolVersion">IN - The reported EGM protocol version.</param>
	void SetReportedEgmProtocolVersion(const std::string& reportedEgmProtocolVersion);

	/// <summary>
	/// Gets the reported EGM protocol version..
	/// </summary>
	/// <returns>The reported EGM protocol version.</returns>
	std::string GetReportedEgmProtocolVersion() const;

	/// <summary>
	/// Sets EGM cashout to host status.
	/// </summary>
	/// <param name="value">cashout to host status</param>
	void SetEGMCashoutToHostStatus(byte value);

	/// <summary>
	/// Gets EGM cashout to host status.
	/// </summary>
	/// <returns></returns>
	byte GetEGMCashoutToHostStatus() const;

	/// <summary>
	/// Increment counter to track cashout to host requests accepted
	/// </summary>
	/// <param name="value">cashout to host status</param>
	void IncrementCashoutToHostRequestsAccepted();

	/// <summary>
	/// Gets counter to track cashout to host requests accepted
	/// </summary>
	/// <returns></returns>
	DWORD GetCashoutToHostRequestsAccepted() const;

	/// <summary>
	/// Increment counter to track cashout to host requests canceled
	/// </summary>
	/// <param name="value">cashout to host status</param>
	void IncrementCashoutToHostRequestsCanceled();

	/// <summary>
	/// Gets counter to track cashout to host requests canceled
	/// </summary>
	/// <returns></returns>
	DWORD GetCashoutToHostRequestsCanceled() const;

	/// <summary>
	/// Sets whether the EGM supports long poll 7B information is available.
	/// </summary>
	/// <param name="egmSupportsLP7B_Available">IN - Sets whether the EGM supports long poll 7B information is available.</param>
	void SetEgmSupportsLP7B_Available(bool egmSupportsLP7B_Available);

	/// <summary>
	/// Gets whether the EGM supports long poll 7B information is available.
	/// </summary>
	/// <returns>true if the EGM supports long poll 7B information is available, false otherwise.</returns>
	bool GetEgmSupportsLP7B_Available() const;

	/// <summary>
	/// Sets whether the EGM supports long poll 7B.
	/// </summary>
	/// <param name="egmSupportsLP7B">IN - Sets whether the EGM supports long poll 7B.</param>
	void SetEgmSupportsLP7B(bool egmSupportsLP7B);

	/// <summary>
	/// Gets whether the EGM supports long poll 7B.
	/// </summary>
	/// <returns>true if the EGM supports long poll 7B, false otherwise.</returns>
	/// <remarks>
	/// Note that the return value is not valid if GetEgmSupportsLP7B_Available() returns false.
	/// </remarks>
	bool GetEgmSupportsLP7B() const;

	/// <summary>
	/// Determines whether meter capture is in progress.
	/// </summary>
	/// <returns>true if meter capture is in progress, false otherwise.</returns>
	bool IsMeterCaptureInProgress() const;

	/// <summary>
	/// Gets the type of the meter capture.
	/// </summary>
	/// <returns>meter capture type</returns>
	SnapshotType GetMeterCaptureType() const;

	/// <summary>
	/// Sets the meter capture start time.
	/// </summary>
	/// <param name="type">meter capture snapshot type.</param>
	void SetMeterCaptureStartTime(SnapshotType type);

	/// <summary>
	/// Gets the meter capture start time.
	/// </summary>
	/// <returns>meter capture start time</returns>
	time_t GetMeterCaptureStartTime() const;

	/// <summary>
	/// Sets the meter capture state.
	/// </summary>
	/// <param name="type">meter capture snapshot typ</param>
	/// <param name="keepMeterCaptureDisplayUpTilFinished">keeps the meter capture in progress display up if true</param>
	void BeginMeterCapture(SnapshotType type, bool keepMeterCaptureDisplayUpTilFinished);

	/// <summary>
	/// Sets the meter capture percentage.
	/// </summary>
	/// <param name="gameNum">The game number.</param>
	/// <param name="totalMeters">The total meters.</param>
	/// <param name="finishedMeters">The finished meters.</param>
	void SetMeterCapturePercentage(DWORD gameNum, WORD totalMeters, WORD finishedMeters);

	/// <summary>
	/// Sets the meter capture to the end state.
	/// </summary>
	/// <param name="captureStatus">The capture status.</param>
	/// <param name="gameCount">The game count.</param>
	void SetMeterCaptureEnd(MeterCaptureStatus captureStatus, WORD gameCount);

	void SetGameOnline(bool value);
	bool IsGameOnline() const;
	void SetEgmErrorCount(WORD value);
	WORD GetEgmErrorCount() const;
	void SetPollerOnline(bool value);
	bool IsPollerOnline() const;
	bool IsSystemOnline() const;

	/// <summary>
	/// Checks whether poller is ready to accept subgame list(322) message from nCompass. 
	/// This is different from IsSystemOnline() method in respect that it doesn't take 
	/// into account voucher service as that is not needed while sending GameList (322) 
	/// message to poller.
	/// </summary>
	/// <returns> whether poller is ready to accept GameList (322) message or not</returns>
	bool IsPollerReadyForSubgameList() const;

	/// <summary>
	/// Gets the TCP connected.
	/// </summary>
	/// <returns></returns>
	bool GetTcpConnected();

	/// <summary>
	/// Sets the connectivity status flags.
	/// </summary>
	/// <param name="value">contains the tcp, poller, PMT, MMT and Voucher services flags.</param>
	void SetConnectivityStatus(WORD value);

	/// <summary>
	/// Gets the PMT up.
	/// </summary>
	/// <returns></returns>
	bool GetPmtUp();

	/// <summary>
	/// Sets the employee goodbye enabled.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetEmployeeGoodbyeEnabled(bool value);

	/// <summary>
	/// Transactions the fill status okay.
	/// </summary>
	/// <returns>true fill status is okay, false otherwise</returns>
	bool TransactionFillStatusOkay() const;

	/// <summary>
	/// Sets the transaction fill status.
	/// </summary>
	/// <param name="fillStatus">The fill status.</param>
	void SetTransactionFillStatus(TransactionsFillStatus fillStatus);

	/// <summary>
	/// Gets the transaction fill status.
	/// </summary>
	/// <returns>Transactions Fill Status</returns>
	TransactionsFillStatus GetTransactionFillStatus() const;

	/// <summary>
	/// Check if meter capture timed out
	/// </summary>
	/// <returns>true if meter capture timed out, false otherwise.</returns>
	bool MeterCaptureTimedOut() const;

	/// <summary>
	/// Determines whether background snapshot is in progress.
	/// </summary>
	/// <param name="meterCapturePercentage">OUT - meter capture percentage.</param>
	/// <returns>true if background snapshot is in progress, false otherwise.</returns>
	bool IsBackgroundSnapshotInProgress(int& meterCapturePercentage) const;
	
	/// <summary>
	/// Sets the type of the meter change request.
	/// </summary>
	/// <param name="type">The meter change request type.</param>
	void SetMeterChangeRequestType(MeterChangeRequestType type);

	/// <summary>
	/// Determines whether meter change is in progress.
	/// </summary>
	/// <returns>true if meter change is in progress, false otherwise.</returns>
	bool IsMeterChangeInProgress() const;
	
	/// <summary>
	/// Determines whether a meter change is major.
	/// </summary>
	/// <returns>true if meter change is major, false otherwise.</returns>
	bool IsMeterChangeMajor() const;

	/// <summary>
	/// Checks if card insertion is to be ignored.
	// If we are in a snapshot, card insertion is ignored,
	// but allow the card insertion if we are performing a backgroud (drop door and cash door) meter snapshot.
	/// </summary>
	/// <param name="card">The card.</param>
	void CheckIfCardInsertionIsToBeIgnored(CCard*& card) const;

	/// <summary>
	/// Sets the virtual player flag.
	/// </summary>
	/// <param name="value">virtual player flag</param>
	void SetVirtualPlayerFlag(bool value);

	/// <summary>
	/// Sets the skin information.
	/// </summary>
	/// <param name="skinId">skin Id</param>
	/// <param name="skinName">skin name</param>
	/// <param name="skinVersion">skin version</param>
	/// <param name="skinRevision">skin Revision</param>
	void SetSkinInfo(const DWORD skinId, const std::string& skinName, const std::string& skinVersion, const DWORD skinRevision);

	/// <summary>
	/// Gets the skin Id.
	/// </summary>
	DWORD GetSkinId() const;

	/// <summary>
	/// Gets the skin name.
	/// </summary>
	std::string GetSkinName() const;

	/// <summary>
	/// Gets the skin version.
	/// </summary>
	std::string GetSkinVersion() const;

	/// <summary>
	/// Gets the skin Revision.
	/// </summary>
	DWORD GetSkinRevision() const;

	/// <summary>
	/// Sets the campaign info.
	/// </summary>
	/// <param name="campaignId">campaign Id</param>
	/// <param name="campaignName">campaign name</param>
	/// <param name="campaignRevision">campaign revision</param>
	void SetCampaignInfo(const DWORD campaignId, const std::string& campaignName, const DWORD campaignRevision);

	/// <summary>
	/// Gets the campaign Id.
	/// </summary>
	DWORD GetCampaignId() const;

	/// <summary>
	/// Gets the campaign name.
	/// </summary>
	std::string GetCampaignName() const;

	/// <summary>
	/// Gets the campaign revision.
	/// </summary>
	DWORD GetCampaignRevision() const;

	void SetProgGames(CGames *games);
	CGames *GetProgGames();
	
	void MemoryClearSuccessful();

	void SetDownloadStatus(const CFlashStatus &status);

	/// <summary>
	/// Gets the USB path string if any USB is attched to the Sentinel.
	/// </summary>
	static std::string GetUsbFilePath();

	/// <summary>
	/// Sets if UI Restart is in progress.
	/// </summary>
	void SetUIRestartInProgress(bool value)
	{
		m_buiRestartInProgress = value;
	}

	/// <summary>
	/// Returns true if Calibration is in progress.
	/// </summary>
	bool IsUIRestartInProgress() const
	{
		return m_buiRestartInProgress;
	}
	
	static const std::string SENTINEL_STATE_TOPIC;
	static const std::string STATUS_TOPIC;
	static const std::string ONELINK_CONNECTION_TOPIC;
	static const std::string DISK_SPACE_TOPIC;
	static const std::string MEMORY_CLEAR_TOPIC;
	static const std::string DOWNLOAD_STATE_TOPIC;
	static const std::string METER_CAPTURE_TOPIC;
	static const std::string GRAPHIC_CARD_CONTENT_TOPIC;	

	const time_t m_startupTime;
	const std::string m_sentinelVersion;
	DWORD m_availablePhysicalMemory;

	bool m_tickCountOfLastFloorLogixTimeUpdateAvailable;
	DWORD m_tickCountOfLastFloorLogixTimeUpdate;
	bool m_timeOfLastFloorLogixConfigAvailable;
	time_t m_timeOfLastFloorLogixConfig;

	time_t m_lastUIRestartTime;
	DWORD m_uiRestartCount; // Restarts.  Does not include first start.

	time_t m_lastUIRefreshTime;
	DWORD m_uiRefreshCount;

	TiltCode m_rebootTilt;
	DWORD m_pollerPacketSentVerifyErrors;
	WORD m_pollerPacketVersion;
	bool m_oneLinkProgressivesSenOptionSupported;
	bool m_oneLinkProgressivesEnabled;
	bool m_mobileConnectCommEnabled;

	PendingAction m_pendingAction;	

	CurrentActivity m_currentActivity;

	/// <summary>
	/// Gets name of last file received.
	/// </summary>
	/// <returns>name of last file received</returns>
	std::string GetLastReceivedFile() const;

	/// <summary>
	/// Gets time of last file received.
	/// </summary>
	/// <returns>time of last file received</returns>
	std::string GetLastReceivedTime() const;

	/// <summary>
	/// Gets Game Chunk Index Sent to Poller.
	/// </summary>
	/// <returns>Returns Game Chunk Index Sent to Poller</returns>
	WORD GetGameChunkIndexSent() const;

	/// <summary>
	/// Sets Game Chunk Index Sent to Poller.
	/// </summary>
	/// <param name="gameChunkIndexSent">Game Chunk Index Sent to Poller</param>
	/// <returns>None</returns>
	void SetGameChunkIndexSent(WORD gameChunkIndexSent);

	/// <summary>
	/// Gets Total Number Of Game Chunks to be sent to Poller.
	/// </summary>
	/// <returns>Returns Total Number Of Game Chunks to be sent to Poller</returns>
	WORD GetTotalNumberOfGameChunks() const;

	/// <summary>
	/// Sets Total Number Of Game Chunks to be sent to Poller.
	/// </summary>
	/// <param name="numberOfGameChunks">Total Number Of Game Chunks to be sent to Poller</param>
	/// <returns>None</returns>
	void SetTotalNumberOfGameChunks(WORD numberOfGameChunks);

	/// <summary>
	/// Sets meter rollover configuration values.
	/// </summary>
	void SetMeterRolloverConfig(bool meterRolloverSyncEnabled,
		BYTE meterRolloverTimeHour,
		BYTE meterRolloverTimeMinute,
		BYTE meterRolloverTimeSecond);
	
	/// <summary>
	/// Checks if rollover meter snapshot need to be done.
	/// </summary>
	/// /// <returns>true if rollover meter snapshot needs to be done, false otherwise.</returns>
	bool PerformRolloverMeterCheck();

	/// <summary>
	/// Gets the download status.
	/// </summary>
	/// <returns>the download status.</returns>
	CFlashStatus GetDownloadStatus() const;

private:
	// Disable default constructor, default copy constructor, and default assignment operator.
	CSentinelState() = delete;
	CSentinelState(const CSentinelState &rhs) = delete;
	CSentinelState& operator=(const CSentinelState &rhs) = delete;

	std::string ResponseForSentinelState() const;
	std::string ResponseForStatus() const;
	std::string ResponseForOneLinkConnection() const;
	std::string ResponseForDiskSpace();
	std::string ResponseForMemoryClear() const;
	std::string ResponseForDownloadState() const;
	std::string ResponseForMeterCapture();
	std::string ResponseForGraphicCardContent() const;

	void AddDiskSpaceInfo(std::vector<tDiskSpace>& vdiskSpace, const std::string& fileSytemName, const std::string& fileSytemPath);

	static const DWORD AVAILABLE_PHYSICAL_MEMORY_NOT_AVAILABLE = (DWORD)-1; // Default value at startup.

	mutable std::recursive_mutex m_criticalSection;

	bool m_doorOpen;
	int m_lastCoinsPlayed;

	bool m_virtualPlayer;

	// Lock the m_criticalSection while accessing the following progressive members.
	CProgressiveConfig m_progressiveConfig;
	CProgressiveConnectionInfo m_progressiveConnectionInfo;
	CProgressiveHost m_progressiveHost;
	
	std::string m_reportedEgmProtocolVersion;
	bool m_egmSupportsLP7B_Available; // Set true if m_egmSupportsLP7B is available, set false otherwise.
	bool m_egmSupportsLP7B;

	std::string m_meterCaptureMessage;
	int m_meterCapturePercentage;
	MeterCaptureStatus m_meterCaptureStatus;
	bool m_meterCaptureInProgress;
	bool m_keepMeterCaptureUp;
	SnapshotType m_snapShotType;
	time_t m_snapshotStartTime;
	time_t m_rolloverStartTime;
	DWORD m_snapshotStatusTickCount;// This is to keep track of meter capture time out.
	MeterChangeRequestType m_meterChangeInProgress;
	TransactionsFillStatus m_transactionFillStatus;
	SlotmastIDResponse m_slotIdResponse;

	bool m_gameOnline;
	WORD m_egmErrorCount;
	u_ConnectivityStatus m_connectivityStatus;
	bool m_employeeGoodbyeEnabled;
	bool m_memoryClearSuccessful;
    CFlashStatus m_downloadStatus;
	CGames *m_progGames;

	std::string m_skinVersion;
	std::string m_skinName;
	DWORD m_skinId;
	DWORD m_skinRevision;

	std::string m_campaignName;
	DWORD m_campaignRevision;
	long m_campaignId;

	bool m_buiRestartInProgress;
	bool m_bCalibrationInProgress;

	WORD m_gameChunkIndexSent;
	WORD m_numberOfGameChunks;

	byte m_cashoutToHostStatusOnEGM;
	DWORD m_cashoutToHostRequestsAccepted;
	DWORD m_cashoutToHostRequestsCanceled;

	bool m_canPerformMeterRollover;
	bool m_meterRolloverSyncEnabled;
	BYTE m_meterRolloverTimeHour;
	BYTE m_meterRolloverTimeMinute;
	BYTE m_meterRolloverTimeSecond;

	SYSTEMTIME m_lastSetEgmErroCountTimeStamp;

	//friend class declarations for unit test access.
	friend class SentinelNativeUnitTests::SentinelState;
};
