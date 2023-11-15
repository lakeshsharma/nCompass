#pragma once

#include "ScreenDefs.h"
#include "../Config/EGMConfig.h"
#include "../Connection.h"
#include "../resource.h"
#include "../InternalMessageQueueList.h"
#include "../InternalMessage.h"
#include "../FileList.h"
#include "../VideoMixerUpgrade.h"
#include "../CarrierUpgrade.h"
#include "../Config/MediaWindowConfig.h"
#include "SentinelDlgState.h"
#include "BonusCounterState.h"
#include "ProgressiveProtocol/ProgressiveConfig.h"
#include "ProgressiveProtocol/ProgressiveConnectionInfo.h"
#include "ProgressiveProtocol/ProgressiveHost.h"
#include "ServiceWindowManager.h"

const int MAX_METERS_DISPLAYED = 15;
const int MAX_LOGS_DISPLAYED = 5;
const int MAX_TOUCH_SCREEN_PROTOCOLS_DISPLAYED = 5;
const int MAX_VIDEO_MODES_DISPLAYED = 5;
const DWORD STARVE_INDEFINITELY = 0;
// Sound played when the card is not read correctly

class CUIProtocol
{
public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CUIProtocol"/> class.
	/// </summary>
	/// <param name="uiToInternalsQueue">The UI to internals queue.</param>
	/// <param name="pParent">A pointer to the parent window.</param>
	CUIProtocol(CQueueList *uiToInternalsQueue);

	/// <summary>
	/// Destructor.
	/// </summary>
	virtual ~CUIProtocol();

	CInternalMessageQueueList * GetInternalsQueue(void)
	{
		return m_internalsToUIQueue;
	}

	void ProcessMessageFromInternals( CInternalMessage *msg );

	void CheckTimedEvents( void );
	
	bool  IsTimeToClose();
	bool IsTimeToRestartMWThread();
	void SetReadThreadClosed(); //only to be called by the read thread.
	
	CServiceWindowManager *GetServiceWindowManager(void)
	{
		return m_serviceWindow;
	}

	/// <summary>
	/// Process JSON message coming from angular UI
	/// </summary>
	/// <param name="message">Message to process</param>
	void ProcessAngularUIMessage(const std::string& message);

	void SendMessageToInternals( CInternalMessage *msg, int priorityLevel );
	void SendMessageToUI( CInternalMessage *msg, int priorityLevel );

	/// <summary>
	/// Performs shutdown immediately.
	/// </summary>
	void PerformShutdown(CInternalMessage *msg);

	CSentinelDlgState* GetUiStateObject() const { return m_uiState; }
	
	void RestartServiceWindow(bool calibrationError, bool startup);

// Implementation
private:
	static const DWORD MINIMUM_PB_POINTS_PER_DOLLAR = 1; // Minimum for UI operation to avoid division issues.
	static const int MAXIMUM_PENDING_PB_LOG_ITEMS = 3;
	static const int MAX_HOST_LIST_VALUES_DISPLAYED = 15;
	static const int MAX_HOST_ADDRESSES_VALUES_DISPLAYED = 15;

	bool Initialize();
	
	bool ProcessServiceWindowManagerDependentMessages(CInternalMessage *msg);
	
	bool PushMessageToOurQueue( CInternalMessage *msg, int priorityLevel );

	//Process internals messages to UI
	void ProcessDisplayEmployeeScreen(CInternalMessage* msg);
	void ProcessEmployeePinResult(CInternalMessage* msg);
	void SendEmployeeCardEventSubscriptionData();

	void ProcessRestartServiceWindow(CInternalMessage *msg);
	void ProcessTerminateBrowser();
	void ProcessSubscriptionUpdates(CInternalMessage* msg);
	void ProcessUIConfig(CInternalMessage *msg);
	void ProcessDisplayAnniversary(CInternalMessage *msg);
	void ProcessDisplayBirthday(CInternalMessage *msg);
	void ProcessDisplayBirthdayAndAnniversary(CInternalMessage *msg);
	void ProcessDisplayCongrats(CInternalMessage* msg);
	void ProcessDisplayBadCardRead(CInternalMessage* msg);
	void ProcessDisplayBannedPlayerMessage(CInternalMessage* msg);
	void ProcessDisplayCashoutPBTError(CInternalMessage *msg);
	void ProcessDisplayAutoUploadError(CInternalMessage* msg);
	void ProcessListenToPBTProgressFromGame(CInternalMessage *msg);
	void ProcessListenToPBTProgressToGame(CInternalMessage *msg);
	void ProcessCheckUploadableCredits(CInternalMessage *msg);
	void ProcessClearPBT(CInternalMessage *msg);
	void ProcessDisplayAutoTransferCancelled(CInternalMessage *msg);
	void ProcessMonitorMobilePBSession(CInternalMessage *msg);
	void ProcessDisplayPBTransferError(CInternalMessage *msg);

#if 0 //UI_REWRITE_CLEANUP, need to revisit with new design.
	void ProcessPBTProbeStart();
#endif

	void ProcessEmployeeCardIn(CInternalMessage *msg);
	void ProcessCampaignAvailable(CInternalMessage *msg);
	void ProcessRefreshUI(CInternalMessage *msg);
	void ProcessEgmAwardFailed(CInternalMessage *msg);
	void ShutdownServiceWindow();
	std::unique_ptr<std::thread> m_uiReadThread;

	// The queue for messages coming from Internals
	CInternalMessageQueueList *m_internalsToUIQueue;

	// The external queue for messages going to the internals
	CQueueList *m_uiToInternalsQueue;

	std::recursive_mutex m_criticalSection;

	time_t m_t1;
	
	CServiceWindowManager *m_serviceWindow;

	bool m_restartingMW;

	// Facelift members.
	CUIConfig m_uiConfig;
	CSentinelDlgState *m_uiState;
};
