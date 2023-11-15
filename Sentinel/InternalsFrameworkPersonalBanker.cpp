#include "stdafx.h"
#include "InternalsFramework.h"
#include "Sentinel.h"
#include "UI/PersonalBankerLog.h"
#include "Diagnostics.h"
#include "Logging/LogString.h"

using namespace std;

/// <summary>
/// Perform PersonalBanker actions.
/// </summary>
/// <remarks>
/// Possibly, perform multiple interdependent accesses and updates to m_personalBanker.
/// Get exclusive access to m_personalBanker to:
/// ensure consistent results from this thread,
/// prevent unwanted state changes by other threads,
/// and ensure that state values reported to other threads are consistent.
/// <para/>
/// Note that this function can be called recursively via ReleasePersonalBankerSession().
/// </remarks>
void CInternalsFramework::PerformPersonalBankerActions()
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	PersonalBankerAction personalBankerAction = m_personalBanker->GetAction();

	if (PBActionNone != personalBankerAction)
	{
		LogString(PBT_LOG, "%s %s", __FUNCTION__, m_personalBanker->GetActionString(personalBankerAction));
	}

	switch (personalBankerAction)
	{
	case PBActionNone:
		break;

	case PBActionGetEGMBalances:
	case PBActionGetEGMBalancesWithMessage:
		if (PBActionGetEGMBalancesWithMessage == personalBankerAction)
		{
			PerformPBActionSendCardSessionInProgressToUI();
		}
		TellGCFToGetBalances();

		break;

	case PBActionSendTransferTransaction:
		SendPersonalBankerTransaction();
		break;

	case PBActionReleaseSession:
		ReleasePersonalBankerSession();
		break;

	case PBActionSendCardSessionInProgressToUI:
		PerformPBActionSendCardSessionInProgressToUI();
		break;

	case PBActionStartTransferOnly:
		PerformPBActionStartTransfer();
		break;

	case PBActionSendPersonalBankerNcepOutFail:
		PerformPBActionSendPersonalBankerNcepOutFail();
		break;

	case PBActionSendDisplayPBAnimationToUI:
	case PBActionSendDisplayPBAnimationToUIAndTransferToEGM:
		PerformPBActionSendDisplayPBAnimationToUI(personalBankerAction);
		break;

	case PBActionSendTaxDeductionRequest:
		LogString(JACKPOT, "BeginPersonalBankerTransaction TotalWithholdingAmountInPennies=%u", m_jackpot->GetTotalWithholdingInPennies());
		BeginPersonalBankerTransaction(CashFromGame, m_jackpot->GetTotalWithholdingInPennies());
		break;

	case PBActionSendPersonalBankerInfoToUI:
		PerformPBActionSendPersonalBankerInfoToUI(m_personalBanker->GetInfoCopy());
		break;

	case PBActionProcessRequestPlayerResume:
	case PBActionCancelAutoUploadDueToCashErrorSessionRelease:
	case PBActionProcessRequestPlayerResumeDueToCashInactive:
	case PBActionProcessRequestPlayerResumeDueToGuestPlayer:
		ProcessRequestPlayerResume(personalBankerAction);
		break;

	case PBActionSendAutoTransferCancelledToUI:
		PerformPBActionSendAutoTransferCancelledToUI();
		break;

	case PBActionSendCompleteToUI:
		PerformPBActionSendCompleteToUI(personalBankerAction);
		break;

	case PBActionSendAutoUploadCancelledMessage:
		PerformPBActionSendAutoUploadCancelledMessage();
		break;

	case PBActionProcessAuthorizationCanceled:
		PerformPBActionProcessAuthorizationCanceled();
		//Need to send message to UI for pb complete
		PerformPBActionSendCompleteToUI(PBActionSendCompleteToUI);
		break;

	case PBActionSendCancelCashoutToHostToEGM:
		{
			WORD cashoutErrorFlags = m_personalBanker->GetCashoutErrorFlags();
			if (cashoutErrorFlags)
			{
				SetMessagePersistenceAndSendMessageToUI(new CInternalMessage(INT_MSG_DISPLAY_CASHOUT_PBT_ERROR, cashoutErrorFlags));
				SendMCPlayerSessionInfoToPollerProtocol(SESSION_CHANGE_EVENT_CASHOUT_ERROR, cashoutErrorFlags);
			}
			PerformPBActionSendCancelCashoutToHostToEGM();
			m_sentinelState.IncrementCashoutToHostRequestsCanceled();
			// Send Tx90 ReleaseSession and delete transfer object
			m_personalBanker->SetToReleaseTransferSession();
			SendPersonalBankerTransaction();
		}
		break;

	default:
		break;
	}

	m_personalBanker->ClearPersonalBankerAction();
}

void CInternalsFramework::PerformPBActionSendCardSessionInProgressToUI()
{
	SetMessagePersistenceAndSendMessageToUI(new CInternalMessage(INT_MSG_CHECK_UPLOADABLE_CREDITS));
	m_personalBanker->SetLastCardedSessionInProgressToCurrent();
}

void CInternalsFramework::PerformPBActionStartTransfer()
{
	LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);

	// Start the transfer
	StartPersonalBankerTransfer();
}

void CInternalsFramework::PerformPBActionSendPersonalBankerNcepOutFail()
{
	CPersonalBankerTransfer *pbTransfer = m_personalBanker->GetTransferCopy();
	if (pbTransfer != nullptr)
	{
		CInternalMessage *msg = new CInternalMessage(INT_MSG_PB_NCEPOUT_FAIL, pbTransfer, m_personalBanker->GetState());
		//	LogString("GENERAL", "**** Sending PB complete to UI ****" );
		int priority = QUEUE_PRIORITY_NORMAL;
		msg->SetMessagePersist(m_msgPriority.PrioritizeMessage(msg, priority));
		SendPersonalBankerMessageToUI(msg, true);
	}

	LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);

	//Send transaction to poller
	SendPersonalBankerTransaction();

	//Delete the transfer object because we are done with it
	m_personalBanker->DoneWithTransfer();

	//SPS Tracker 26423 2010-06-04
	// Send new transaction 46 so that we can get the user who canceled the transfer.
	CInternalMessage *transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_AUTO_UPLOAD_CANCELED, IsPlayerVirtual());
	SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::PerformPBActionSendDisplayPBAnimationToUI(PersonalBankerAction personalBankerAction)
{
	if (personalBankerAction == PBActionSendDisplayPBAnimationToUIAndTransferToEGM)
	{
		CPersonalBankerTransfer *pbTransfer = m_personalBanker->GetTransferCopy();
		if (pbTransfer)
		{
			// Send message to GCF to get balances on game for NCEPOut stuff
			CInternalMessage *msg = new CInternalMessage(INT_MSG_PB_TRANSFER, pbTransfer, m_personalBanker->GetState());
			SendMessageToGCF(msg, QUEUE_PRIORITY_NORMAL);

			// if cashout to PBT or auto-upload, send command ListenToPBTProgress to Angular UI.
			if (pbTransfer->IsCashoutToHostTransfer() || m_personalBanker->IsAutoInProcess())
			{
				SendMessageToUI(new CInternalMessage(INT_MSG_LISTEN_TO_PBT_PROGRESS_FROM_GAME), QUEUE_PRIORITY_NORMAL);
			}
		}
	}
}

void CInternalsFramework::PerformPBActionSendPersonalBankerInfoToUI(CPersonalBankerInfo *pbInfo)
{
	if (nullptr != pbInfo)
	{
		InternalMsgType msgType(NewPinRequired == pbInfo->GetErrorCode() ? INT_MSG_NEW_PIN_REQUIRED : INT_MSG_PB_INFO);
		CInternalMessage *msg(new CInternalMessage(msgType, pbInfo));
		msg->SetData((WORD)(m_pairingInformation.IsMobileCardedSession() && m_mobilePBTInfo.IsMobileFundsTransferBalanceRequest()));
		if (pbInfo->HasError())
		{
			int priority(QUEUE_PRIORITY_NORMAL);
			CMessagePersist *mp = m_msgPriority.PrioritizeMessage(msg, priority);
			msg->SetMessagePersist(mp);
		}
	
		ProcessPersonalBankerUIActivity();

		//Send the message to the UI
		// UI_REWRITE_CLEANUP: Sending INT_MSG_PB_INFO for case like 'InvalidPin' here will stop PBT starvation
		// and starve this messsage instead for 4 secs and then unstarve the PBT session. Looks like the current
		// function is not used at-all and only relevant thing remaining here worth calling is 
		// ProcessPersonalBankerUIActivity(). Check and cleanup the code accordingly.
		// SendPersonalBankerMessageToUI(msg);
	}
}

void CInternalsFramework::PerformPBActionSendAutoTransferCancelledToUI()
{
	// cause PBT transfering message or animation to clear.
	CInternalMessage *newMsg = new CInternalMessage(INT_MSG_AUTO_TRANSFER_CANCELLED);
	int priority = QUEUE_PRIORITY_NORMAL;
	newMsg->SetMessagePersist(m_msgPriority.PrioritizeMessage(newMsg, priority));
	SendMessageToUI(newMsg, priority);
}

void CInternalsFramework::PerformPBActionSendCancelCashoutToHostToEGM()
{
	CInternalMessage *newMsg = new CInternalMessage(INT_MSG_CASHOUT_TO_HOST);
	newMsg->SetData((WORD)0); // tell SAS to cancel cashout to host request
	SendMessageToGCF(newMsg, QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::PerformPBActionSendCompleteToUI(PersonalBankerAction action)
{
	// Tell the UI we are done with PB.
	CPersonalBankerTransfer *pbTransfer = m_personalBanker->GetTransferCopy();
	if (pbTransfer != nullptr)
	{
		if (PBActionSendCompleteToUI == action && pbTransfer->IsProcessTaxDeduction())
		{
			CQueueList* scfQueue(GetSCFQueue());
			if (scfQueue)
			{
				//We're not doing the processing here directly because we can't acquire the jackpot lock
				//because it will cause a deadlock because player lock has been acquired prior to this.
				CInternalMessage* msg(new CInternalMessage(INT_MSG_PROCESS_TAX_DEDUCTION_COMPLETE,
					(WORD)m_personalBanker->IsSessionReleasedDueToTimeout(), (WORD)(CanceledCashUpload == pbTransfer->GetPersonalBankerType())));
				msg->SetData(pbTransfer);
				scfQueue->Add(msg, QUEUE_PRIORITY_HIGHEST);
			}
		}
		else
		{
			m_egmAwardManager.SetPrizeClearedByPbt(true);

			LogString(PBT_LOG, "%s %d CashInactive=%d action=%d", __FUNCTION__, __LINE__, m_personalBanker->IsCashInactive(), action);
			
			//UI_REWRITE_CLEANUP - will be referred for auto upload 
			/* int priority = QUEUE_PRIORITY_NORMAL;
			CInternalMessage *msg = new CInternalMessage((PBActionSendCompleteToUICashInactive == action) ? INT_MSG_PB_COMPLETE_SKIP_AUTO_UPLOAD : INT_MSG_PB_COMPLETE,
				pbTransfer, m_personalBanker->GetPbStateForUi(),
				m_personalBanker->IsSessionReleasedDueToTimeout());
			msg->SetMessagePersist(m_msgPriority.PrioritizeMessage(msg, priority));
			SendPersonalBankerMessageToUI(msg, false, priority);*/

			// Sending cashout error to Poller Protocol
			if (pbTransfer->IsCashoutToHostTransfer())
			{
				std::string sasErrorText;

				if (m_personalBanker->GetPbStateForUi() == PBStateTransferComplete &&
					pbTransfer->IsTransferRequestCanceled())
				{
					std::string text1 = ""; // pbTransfer->GetCancelledText1(); UI_REWRITE_CLEANUP
					std::string text2 = ""; // pbTransfer->GetCancelledText2(); UI_REWRITE_CLEANUP

					sasErrorText = text1;
					if (!text2.empty())
					{
						sasErrorText = sasErrorText + " " + text2;
					}

					pbTransfer->AppendCashoutErrorFlags(CashoutError::CASHOUT_ERROR_SAS_ERROR);
				}

				if (pbTransfer->GetCashoutErrorFlags())
				{
					LogString(PBT_LOG, "%s %d SendMCPlayerSessionInfoToPollerProtocol -> SESSION_CHANGE_EVENT_CASHOUT_ERROR", __FUNCTION__, __LINE__);
					SendMCPlayerSessionInfoToPollerProtocol(SESSION_CHANGE_EVENT_CASHOUT_ERROR, pbTransfer->GetCashoutErrorFlags(), sasErrorText);
				}
			}

			// Send PBTransfer subscription to Angular UI
			SendSubscriptionUpdateIfNecessary();

			// update mobile about success transfer
			if (m_mobilePBTInfo.IsMobileFundTransferRequest())
			{
				if (m_personalBanker->IsTransferSuccess())
				{
					// successful transfer
					if (!m_mobilePBTInfo.GetMobileTransferDirection().compare(FROM_GAME))
					{
#ifdef _DEBUG
						LogString(PBT_LOG, "[%s %d] IsTransferSuccess, amount: %lld, FROM game", __FUNCTION__, __LINE__, pbTransfer->GetAmount());
#endif
						SendMCFundsTransferToPollerProtocol(true, SUCCESS, false, pbTransfer->GetAmount(), pbTransfer->GetNonrestrictedAmount(), pbTransfer->GetRestrictedAmount());
					}
					else
					{
#ifdef _DEBUG
						LogString(PBT_LOG, "[%s %d] IsTransferSuccess, amount: %lld, TO game", __FUNCTION__, __LINE__, pbTransfer->GetAmount());
#endif
						switch (m_mobilePBTInfo.GetMobileTransferType())
						{
						case CashToGame:
							SendMCFundsTransferToPollerProtocol(true, SUCCESS, false, pbTransfer->GetAmount(), 0, 0);
							break;
						case PointsToGame:
							SendMCFundsTransferToPollerProtocol(true, SUCCESS, false, 0, pbTransfer->GetAmount(), 0);
							break;
						case PromoDollarsToGame:
							SendMCFundsTransferToPollerProtocol(true, SUCCESS, false, 0, 0, pbTransfer->GetAmount());
							break;
						default:
							break;
						}

					}
				}
				else if (m_personalBanker->IsSessionReleasedDueToTimeout())
				{
					SendMCFundsTransferToPollerProtocol(false, SESSION_TIMEOUT, true);
				}
				else
				{
					// do not send error to UI here as it will already be available to UI via PBTransfer subscription.
					SendMCFundsTransferToPollerProtocol(false, ERROR);
				}
			}

			delete pbTransfer;
		}
	}
}

void CInternalsFramework::PerformPBActionSendAutoUploadCancelledMessage()
{
	CPersonalBankerTransfer *pbTransfer = m_personalBanker->GetTransferCopy();
	if (pbTransfer != nullptr)
	{
		CInternalMessage *msg = new CInternalMessage(INT_MSG_PB_NCEPOUT_FAIL, pbTransfer, m_personalBanker->GetState());
		int priority = QUEUE_PRIORITY_NORMAL;
		msg->SetMessagePersist(m_msgPriority.PrioritizeMessage(msg, priority));
		SendPersonalBankerMessageToUI(msg, true);

		//SPS Tracker 26423 2010-06-04
		// Send new transaction 46 so that we can get the user who canceled the transfer.
		CInternalMessage *transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_AUTO_UPLOAD_CANCELED, IsPlayerVirtual());
		SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);
	}
}

void CInternalsFramework::PerformPBActionProcessAuthorizationCanceled()
{
	CPersonalBankerTransfer *pbTransfer = m_personalBanker->GetTransferCopy();
	pbTransfer->ChangePersonalBankerTypeToCanceled();
	pbTransfer->SetCancelledType(CanceledBySystem);
	ProcessPersonalBankerTransferComplete(pbTransfer);
}

/// <summary>
/// Perform PersonalBanker timed events.
/// </summary>
/// <remarks>
/// Possibly, perform multiple interdependent accesses and updates to m_personalBanker.
/// Get exclusive access to m_personalBanker to:
/// ensure consistent results from this thread,
/// prevent unwanted state changes by other threads,
/// and ensure that state values reported to other threads are consistent.
/// </remarks>
void CInternalsFramework::PerformPersonalBankerTimedEvents()
{
	//We delay the personal banker timed events so that Auto upload does
	//start without the UI because money is taken out from the game
	//without messaging to the customer.
	if (m_restartManager.GetHeartbeatReceivedFromUI())
	{
		lock_guard<recursive_mutex> lock(m_playerCriticalSection); // Added by gph fo TR#: 031617
		bool delayAutoNcepOutWhenNotDefaultedPlayer = !m_protocolManager->InboundQueueForProtocolIsReady(IProtocolManager::EGMProtocol);
		m_personalBanker->PerformTimedEvents(delayAutoNcepOutWhenNotDefaultedPlayer,
			m_employee1 != nullptr,
			m_config,
			CUtilities::GetTickCount(),
			m_mobilePBTInfo.IsMobilePBSession() ? CPersonalBanker::PB_SESSION_TIMEOUT_MOBILE :
												  CPersonalBanker::PB_SESSION_TIMEOUT );

		// If action is not PBActionNone then process action and check again for more timed events.
		if (PBActionNone != m_personalBanker->GetAction())
		{
			PerformPersonalBankerActions();

			m_personalBanker->PerformTimedEvents(delayAutoNcepOutWhenNotDefaultedPlayer,
				m_employee1 != nullptr,
				m_config,
				CUtilities::GetTickCount(),
				m_mobilePBTInfo.IsMobilePBSession() ? CPersonalBanker::PB_SESSION_TIMEOUT_MOBILE :
													  CPersonalBanker::PB_SESSION_TIMEOUT );
		}

		PerformPersonalBankerActions();
	}
}

/// <summary>
/// End active player session for  PersonalBanker.
/// If we have a PB transfer object but the transfer has not started, release the transfer.
/// </summary>
/// <remarks>
/// Possibly, perform multiple interdependent accesses and updates to m_personalBanker.
/// Get exclusive access to m_personalBanker to:
/// ensure consistent results from this thread,
/// prevent unwanted state changes by other threads,
/// and ensure that state values reported to other threads are consistent.
/// </remarks>
void CInternalsFramework::EndPersonalBankerActivePlayerSession()
{
	// If we have a PB transfer object but the transfer has not started, release the transfer.
	// lock this section to prevent potential conflicting events
	lock_guard<recursive_mutex> lock(m_playerCriticalSection); // Added by gph fo TR#: 031617
	m_personalBanker->EndActivePlayerSession(m_config);
	PerformPersonalBankerActions();
}

/// <summary>
/// Process PersonalBankerInfo.
/// </summary>
/// <param name="pbInfo">IN/OUT - PersonalBanker info.</param>
void CInternalsFramework::ProcessPersonalBankerInfo(CPersonalBankerInfo *pbInfo)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	if (pbInfo == nullptr)
	{
		LogString(ERROR_LOG, "pbInfo == nullptr in %s", static_cast<LPCTSTR>(__TFUNCTION__));
	}
	else
	{
		string personalBankerId;
		if (m_personalBanker->ProcessInvalidTransIdIfNecessary(pbInfo, m_config.GetSlotMastID(), personalBankerId))
		{
			TiltCode tilt = tltInvalidPbIdBef;
			CDiagnostics::ReportPBTTicketDiagnosticToFile(CDirectoryManager::PBTTicketDiagnosticsFilePath(), IDS_PBT_INVALID_ID_FLOORLOGIX, personalBankerId, tilt);
			GenerateTilt(tilt);
			LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
			ReleasePersonalBankerSession(true);
		}
		else
		{
			m_personalBanker->ProcessPersonalBankerCreditTransfer(pbInfo);

			PerformPersonalBankerActions();

			m_personalBanker->ProcessPersonalBankerInfo(pbInfo,
				m_config,
				m_sentinelState.IsGameOnline(),
				m_bIsEFT);

		}
		PerformPersonalBankerActions();
	}
}

// CInternalsFramework::GetPbtInProgress
//   RETURN - true if a PBT transfer is pending or is in progress or if displaying PBT information is in progress.
//      - returns false otherwise.
bool CInternalsFramework::GetPbtInProgress()
{
	return m_personalBanker->IsPbtInProgress();
}

void CInternalsFramework::ProcessInitialPersonalBanker(void)
{
	m_protocolManager->WaitForProtocol(IProtocolManager::UIProtocol);
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	m_personalBanker->ProcessInitialPersonalBanker(m_config);
	if (m_personalBanker->HasTransfer())
	{
		PersonalBankerState pbState = m_personalBanker->GetState();
		LogString(PBT_LOG, "%s %d pbState=%s", __FUNCTION__, __LINE__, CPersonalBankerTransfer::GetStateString(pbState));
		switch (pbState)
		{
		case PBStateUnknown:
			m_personalBanker->RemoveSession();
			break;
		case PBStateInquireSent:
		case PBStateInfoReceived:
			ReleasePersonalBankerSession();
			break;
		case PBStateTransferingFromToGame:
			RequestPersonalBankerStatus();
			SendCardSessionInProgressToUI(true);
			break;
		case PBStateTransferComplete:
		case PBStateReleaseSent:
			SendPersonalBankerTransaction();
			break;
		case PBStateWaitingForUploadToStart:
			m_personalBanker->MakeAutoUploadPending();
			ReleasePersonalBankerSession();
			SendCardSessionInProgressToUI(true);
			break;
		default:
			break;
		}
	}
}

void CInternalsFramework::BeginPersonalBankerSession(CInternalMessage *msg)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	if (nullptr != msg && m_player != nullptr)
	{
		string pin; 
		bool isPinValidLength = false;
		if (m_pairingInformation.IsMobileCardedSession() && m_mobilePBTInfo.IsMobileFundsTransferBalanceRequest())
		{
			CFundsTransferBalanceRequest *pFundsTransferBalanceRequest(msg->GetFundsTransferBalanceRequestData());
			if (pFundsTransferBalanceRequest != nullptr) 
			{
				pin = pFundsTransferBalanceRequest->GetPin();
				// Unencrypt PIN and put it into String pin
				DWORD dwPin = atoi(pFundsTransferBalanceRequest->GetPin().c_str());
				dwPin = CUtilities::UnencryptPBTPin(dwPin);
				stringstream ss;
				ss << dwPin;
				pin = ss.str();

				isPinValidLength = true;//assuming mobile will have length check
#ifdef _DEBUG
				LogString(PBT_LOG, "%s %d - unencrypted PIN  (mobile carded) is '%s'", __FUNCTION__, __LINE__, pin.c_str());
#endif
			}
		}
		else
		{
			pin = msg->GetStringData();
			isPinValidLength = msg->GetWordData();
		}
		
		PBError pbErrorCode(InvalidPin);
		if (CUtilities::IsBannedPin(pin))
		{
			pbErrorCode = BannedPinEntered;
			isPinValidLength = false;
		}
		
		if (isPinValidLength)
		{
			if (m_personalBanker->BeginPersonalBankerSession(m_player, pin, m_config))
			{
				LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
				SendPersonalBankerTransaction();
			}
		}
		else
		{
			if (m_pairingInformation.IsMobileCardedSession() && m_mobilePBTInfo.IsMobileFundsTransferBalanceRequest())
			{
				m_mobilePBTInfo.SetError(INVALID_PIN_LENGTH);
				SendMCFundsTransferBalanceToPollerProtocol();
				LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
				ReleasePersonalBankerSession();

				if (pbErrorCode == BannedPinEntered)
				{
					CPersonalBankerInfo* personalBankerInfo(new CPersonalBankerInfo());
					personalBankerInfo->SetErrorCode(pbErrorCode);
					PerformPBActionSendPersonalBankerInfoToUI(personalBankerInfo);
				}
			}
			else
			{// update UI only for non-mobile session
				CPersonalBankerInfo* personalBankerInfo(new CPersonalBankerInfo());
				personalBankerInfo->SetErrorCode(pbErrorCode);
				PerformPBActionSendPersonalBankerInfoToUI(personalBankerInfo);
			}
		}
	}
}

/// <summary>
/// Begin a PersonalBanker auto session.
/// </summary>
/// <param name="sendToPoller">IN - true if PB Transcation needs to be sent to Poller, false otherwise.</param>
void CInternalsFramework::BeginPersonalBankerAutoSession(bool sendToPoller)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	if (m_personalBanker->BeginPersonalBankerAutoSession(sendToPoller, m_config, PBT_AUTO_UPLOAD_PIN, CUtilities::GetTickCount()))
	{
		LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);

		// Start the transfer
		StartPersonalBankerTransfer();
	}
}

/// <summary>
/// Try to start a PersonalBanker transfer.
/// </summary>
void CInternalsFramework::StartPersonalBankerTransfer()
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	CPersonalBankerTransfer *pbTransfer = m_personalBanker->GetTransferCopy();
	if (pbTransfer == nullptr)
	{
		LogString(PBT_LOG, "%s %d pbTransfer is nullptr.", __FUNCTION__, __LINE__);
		LogString(ERROR_LOG, _T("pbTransfer == nullptr in StartPersonalBankerTransfer"));
	}
	else
	{
		LogString(PBT_LOG, "%s %d pbTransfer is nullptr.", __FUNCTION__, __LINE__);

		// Start the transfer
		CInternalMessage *msg = PrepareTransactionMessage(INT_MSG_TRANSACTION_PB, m_player == nullptr ? false : m_player->IsVirtual(), nullptr);
		msg->SetData(pbTransfer);
		SendMessageToSCF(msg, QUEUE_PRIORITY_NORMAL);

		//Added by Spencer to ensure that the auto upload is not interrupted by Third party content.
		m_promotionMediaInPlay = false;
	}
}

void CInternalsFramework::BeginPersonalBankerTransaction(PersonalBankerTransferType type, DWORD amountInPennies)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	if (!m_personalBanker->IsAuthorisationInProcess())
	{
		if (m_personalBanker->CheckIfAuthorizationIsNeeded(type, amountInPennies, m_config.AtomicRedemptions()))
		{
			LogString("PBT", "BeginPersonalBankerTransaction(authorization needed) type = %d amount = %u",
				type, (unsigned)amountInPennies);
			CPersonalBankerTransfer* pbTransfer(m_personalBanker->GetTransferCopy());
			if (pbTransfer != nullptr)
			{
				LogString("PBT", "Sending PBT authorization to poller...");
				CInternalMessage *msg = PrepareTransactionMessage(INT_MSG_TRANSACTION_PB_AUTHORIZE, m_player == nullptr ? false : m_player->IsVirtual(), nullptr);
				msg->SetData(pbTransfer);
				SendTransactionMessageToSCF(msg, QUEUE_PRIORITY_NORMAL);
			}
		}
		else
		{
			LogString("PBT", "BeginPersonalBankerTransaction(authorization not needed) type = %d amount = %u",
				type, (unsigned)amountInPennies);
			BeginPersonalBankerTransfer(type, amountInPennies);
		}
	}
	else
	{
		LogString("PBT", "BeginPersonalBankerTransaction ignored as authorization is already in progress."); // root cause for double charge scenario
	}
}

void CInternalsFramework::BeginPersonalBankerTransfer(PersonalBankerTransferType type, DWORD amountInPennies)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	LogString(PBT_LOG, "%s :  PersonalBankerType : %d, amountInPennies= %d", __FUNCTION__, type, amountInPennies);
	if (m_personalBanker->BeginPersonalBankerTransfer(m_games, type, amountInPennies, m_pbEFTNumber))
	{
		CheckIfEgmNeedsToBeLockedOrUnlocked();

		// Start the transfer
		CPersonalBankerTransfer* pbTransfer(m_personalBanker->GetTransferCopy());
		if (pbTransfer != nullptr)
		{
			LogString(PBT_LOG, "%s :Send INT_MSG_PB_TRANSFER to internal  m_personalBanker->GetState()= %d", __FUNCTION__, m_personalBanker->GetState());
			CInternalMessage *msg = new CInternalMessage(INT_MSG_PB_TRANSFER, pbTransfer, m_personalBanker->GetState());
			SendMessageToGCF(msg, QUEUE_PRIORITY_NORMAL);
		}
	}
	else
	{
		LogString(PBT_LOG, "%s : pbTransfer null", __FUNCTION__);
		CPersonalBankerTransfer* pbTransfer(m_personalBanker->GetTransferCopy());
		if (pbTransfer != nullptr)
		{
			ProcessPersonalBankerTransferComplete(pbTransfer);
			delete pbTransfer;
		}
	}
}

/// <summary>
/// This gets called to process personal banker transaction stored message from the poller.
/// </summary>
void CInternalsFramework::ProcessPersonalBankerTransactionStored(void)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	m_personalBanker->LogPBT("ProcessPersonalBankerTransactionStored()");
	switch (m_personalBanker->GetState())
	{
	case PBStateTransferComplete:
		if (!m_personalBanker->IsCreditTransfer())
		{
			LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
			ReleasePersonalBankerSession();
		}
		break;
	case PBStateReleaseSent:
	{
		m_personalBanker->RemoveSession();
	}
	break;
	default:
		break;
	}
}

/// <summary>
/// This gets called to process personal banker transaction acknowledged message from the poller.
/// </summary>
void CInternalsFramework::ProcessPersonalBankerTransactionAcked(WORD sequenceNumber)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	m_personalBanker->ProcessPersonalBankerTransactionAcked(sequenceNumber);
}

void CInternalsFramework::ProcessPersonalBankerAuthorization(const string& transactionID, bool granted)
{
	LogString("PBT", "ProcessPersonalBankerAuthorization() : transactionID = %s, granted = %d, m_state = %s",
		transactionID.c_str(), granted, CPersonalBankerTransfer::GetStateString(m_personalBanker->GetState()));
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);

	string currentID(m_personalBanker->GetInfoPersonalBankerID());

	// The conversion from char* to cstring leaves characters at the end. We fix it by truncating
	currentID = currentID.substr(0,transactionID.length());
	
	// Verify that the transactionID that the poller told us was authorized is actually the same ID that we are waiting for
	// If authorisation is received, we should in state to accept it.
	if ((transactionID == currentID) && (m_personalBanker->GetState() == PBStateInfoReceived))
	{
		if (granted)
		{
			LogString("PBT", "Personal Banker Transaction Authorization granted");
			PersonalBankerTransferType type = m_personalBanker->GetPersonalBankerTransferType();
			__int64 amount = m_personalBanker->GetTransferAmount();// In pennies
			BeginPersonalBankerTransfer(type, (DWORD)amount);
		}
		else
		{
			LogString("PBT", "Personal Banker Transaction Authorization denied");
			m_personalBanker->SetState(PBStateAuthDenied);
			ReleasePersonalBankerSession();
			SendMCFundsTransferToPollerProtocol(false, AUTHORIZATION_DENIED, true);
		}
	}
	else
	{
		LogString("PBT", "Unknown Personal Banker Transaction Authorization ignored, expected %s received %s", currentID.c_str(), transactionID.c_str());
	}
}

/// <summary>
/// This gets called to process cancel personal banker session.
/// </summary>
void CInternalsFramework::ProcessCancelPersonalBankerSession(CInternalMessage *msg)
{
	if (nullptr != msg)
	{
		{
			lock_guard<recursive_mutex> lock(m_playerCriticalSection);

			if (m_personalBanker->IsAutoInProcess() || m_personalBanker->IsAutoUploadPending())
			{
					SetMessagePersistenceAndSendMessageToUI(new CInternalMessage(INT_MSG_CLEAR_PBT));
			}

			if (m_pairingInformation.IsMobileCardedSession())
			{
				m_personalBanker->SetSessionForceExit();
			}

			if (m_personalBanker->ProcessCancelPersonalBankerSession(msg->GetPBTransferData(), msg->GetWordData()))
			{
				LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
				ReleasePersonalBankerSession(true);
			}
		}
	}
}

void CInternalsFramework::ProcessRestorePersonalBankerSession()
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	m_personalBanker->ProcessRestorePersonalBankerSession();
}

/// <summary>
/// Process Personal Banker Transfer Complete message.
/// </summary>
/// <param name="pbTransfer">IN - Pointer to the Personal Banker Transfer object.</param>
void CInternalsFramework::ProcessPersonalBankerTransferComplete(CPersonalBankerTransfer *pbTransfer)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	if (pbTransfer != nullptr && m_personalBanker != nullptr)
	{
		bool isDuplicate(false);
		byte meterNumberToAddTo(mNone);
		string errorMessage;
		string error(SAS_ERROR);
		
		// Check transfer against expected amounts
		TiltCode tilt = m_personalBanker->VerifyTransferedAmounts(pbTransfer);
		switch (tilt)
		{
		case tltInvalidPbAmount:
		{
			errorMessage = IDS_UNEXPTECTED_PBT_AMOUNT_FROM_GAME;
			break;
		}
		case tltTransferOvrLmtAftr:
		{
			errorMessage = IDS_PBT_AMOUNT_OVER_LIMIT;
			error = TRANSFER_AMOUNT_OVER_LIMIT;
			break;
		}
		default:
			break;
		}
		
		if (tltUnknown != tilt)
			SendMCFundsTransferToPollerProtocol( false, error, true);

		if (!errorMessage.empty())
		{
			// Log data
			CDiagnostics::ReportPBTTicketDiagnosticToFile(CDirectoryManager::PBTTicketDiagnosticsFilePath(), 
				errorMessage, pbTransfer->ToStringForLog(), tilt);

			// Send tilt and log pbTransfer
			GenerateTilt(tilt);
		}

		m_personalBanker->ProcessPersonalBankerTransferComplete(pbTransfer,
			m_games,
			meterNumberToAddTo,
			isDuplicate,
			m_bIsEFT);

		if (meterNumberToAddTo != mNone)
		{
			CGame *game = m_games.GetEGM();
			if (game != nullptr)
			{
				game->AddToMeter(meterNumberToAddTo, m_personalBanker->GetTransferAmount());
				__int64 newMeterValue = game->GetMeter(meterNumberToAddTo);

				if (!IsNFDPoller())
				{
					// Send this meter update to the SCF
					LogString(GAMES, "[%s]: Sending Incremental 304 EGM meters [Meter: %d, MeterValue: %lld] "
						"at PBTComplete event.", __FUNCTION__, meterNumberToAddTo, newMeterValue);

					CGames *scfGames = new CGames();
					CGame *scfGame = new CGame(*game);
					scfGames->AddGame(scfGame);
					CMeters *scfMeters = scfGame->GetMeters();
					scfMeters->ClearAllMeters();
					scfMeters->SetMeter(meterNumberToAddTo, newMeterValue, true, false);
					CInternalMessage *scfMsg = new CInternalMessage(INT_MSG_SET_METERS, scfGames);
					SendMessageToSCF(scfMsg, QUEUE_PRIORITY_NORMAL);
				}
			}
		}

		// Get the current time to be used for the transaction and the log.
		time_t currentTime = CUtilities::GetCurrentTime();
		time_t *transactionTime = new time_t(currentTime);

		m_personalBanker->SetState(PBStateTransferComplete);

		if (isDuplicate)
		{
			m_personalBanker->LogPBT("ProcessPersonalBankerTransferComplete() ReleasePersonalBankerSession");
			ReleasePersonalBankerSession();
		}
		else
		{
			// Send the transfer object to the poller.
			// Send to poller even if no amount and not cancelled
			if (!m_personalBanker->TransferHasAmount() &&
				!m_personalBanker->IsTransferRequestCanceled() &&
				!m_personalBanker->IsCreditTransfer())
			{
				m_personalBanker->SetPersonalBankerType(UnknownError);
			}

			m_personalBanker->LogPBT("ProcessPersonalBankerTransferComplete() SendPersonalBankerTransaction");
			LogString(PBT_LOG, "%s %d", __FUNCTION__, __LINE__);
			SendPersonalBankerTransaction(transactionTime);

			m_personalBanker->LogTransfer(m_player, m_config, currentTime);
		}

		CheckIfEgmNeedsToBeLockedOrUnlocked();
	}
}

// Send transaction to poller
void CInternalsFramework::SendPersonalBankerTransaction(time_t *transactionTime)
{
	m_personalBanker->PreparePersonalBankerTransfer(m_pbSequence);

	// Make a copy of the transfer object
	CPersonalBankerTransfer *pbTransfer(m_personalBanker->GetTransferCopy());

	if (pbTransfer != nullptr)
	{
		SendPersonalBankerTransfer(pbTransfer, transactionTime);
	}
}

/// <summary>
/// This gets called to request the Personal Banker status.
/// </summary>
void CInternalsFramework::RequestPersonalBankerStatus(void)
{
	lock_guard<recursive_mutex> lock(m_playerCriticalSection);
	CPersonalBankerTransfer *pbTransfer(m_personalBanker->GetTransferCopy());
	if (pbTransfer != nullptr)
	{
		CInternalMessage *msg = new CInternalMessage(INT_MSG_PB_STATUS, pbTransfer, m_personalBanker->GetState());
		SendMessageToGCF(msg, QUEUE_PRIORITY_NORMAL);
	}
}

/// <summary>
/// This gets called to release personal banker session.
/// </summary>
void CInternalsFramework::ReleasePersonalBankerSession(bool canceledFromUI)
{
	if (!CWatchDog::Instance().IsTimeToShutdown())
	{
		LogString(PBT_LOG, "%s %d BEGIN", __FUNCTION__, __LINE__);
		if (m_personalBanker->IsSessionReleasedDueToTimeout()
			|| m_personalBanker->IsTransferRequestCanceled()
			|| m_personalBanker->IsInvalidTransactionIDReceived())
		{
			// Send timeout update to PBInfo subscription
			SendSubscriptionUpdateIfNecessary();
		}

		// Lock to ensure that PersonalBanker transactions to SCF are queued in order.  I.e.,
		// ensure that the release transaction for this PersonalBanker session gets generated to SCF BEFORE
		// another transaction for another PersonalBanker session can be generated.
		lock_guard<recursive_mutex> lock(m_playerCriticalSection);

		// Send GCF message to reset/cancel pbt probe
		LogString(PBT_LOG, "Sending PBT Probe Reset to GCF");
		CInternalMessage* msg = new CInternalMessage(INT_MSG_CLEAR_PBT_PROBE);
		SendMessageToGCF(msg, QUEUE_PRIORITY_NORMAL);

		m_personalBanker->ReleasePersonalBankerSession(m_config, canceledFromUI);

		// Inform PDS & UI about session release when it is due to timeout
		if (m_mobilePBTInfo.IsMobilePBSession() && m_personalBanker->IsSessionReleasedDueToTimeout()
			&& m_config.MobileConnectCommunicationEnabled())
		{
			CInternalMessage *pollerMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_MOBILE_CONNECT_PROXIMITY, IsPlayerVirtual());
			pollerMsg->SetData((WORD) ToPlayerDeviceService::EventEndPBTSession);
			SendTransactionMessageToSCF( pollerMsg, QUEUE_PRIORITY_NORMAL );
		}
		m_mobilePBTInfo.SetMobilePBSession(false);

		PerformPersonalBankerActions();

		m_personalBanker->SetToReleaseTransferSession();
		LogString(PBT_LOG, "%s %d END", __FUNCTION__, __LINE__);
		SendPersonalBankerTransaction();
	}
}

void CInternalsFramework::SendMCFundsTransferToPollerProtocol(bool success, std::string status, bool sendErrorToUI, DWORD cashable, DWORD cashablePromo, DWORD nonCashablePromo)
{
	LogString(POLLER_LOG, "%s %d : success = %d, status = %s", __FUNCTION__, __LINE__, success, status.c_str());

	// just to ensure response for request is sent only once
	if (m_mobilePBTInfo.IsMobileFundTransferRequest() && m_config.MobileConnectCommunicationEnabled())
	{
		CFundsTransferEvent *pFundsTransferEvent = new CFundsTransferEvent(m_mobilePBTInfo.GetMobilePin(), m_mobilePBTInfo.GetMobileFundType(), m_mobilePBTInfo.GetMobileTransferDirection(), 
																			success, status, cashable, cashablePromo, nonCashablePromo, m_pairingInformation.GetCorrelationId(RequestFundsTransfer));
		CInternalMessage *pollerMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_MOBILE_CONNECT_PROXIMITY, IsPlayerVirtual());
		pollerMsg->SetData((WORD) ToPlayerDeviceService::EventFundsTransfer);
		pollerMsg->SetData(pFundsTransferEvent);

		SendTransactionMessageToSCF( pollerMsg, QUEUE_PRIORITY_NORMAL );

		if (!success && sendErrorToUI)
		{
			SendMessageToUI(new CInternalMessage(INT_MSG_DISPLAY_PB_TRANSFER_ERROR, status), QUEUE_PRIORITY_NORMAL);
		}

		m_mobilePBTInfo.SetMobileDenomMismatch(false);
		m_mobilePBTInfo.SetMobileSASError(false);
		m_mobilePBTInfo.SetMobileFundTransferRequest(false);
	}
}

bool CInternalsFramework::IsPBTProbeRequiredMobile(CPersonalBankerInfo *pbInfo)
{
	return (m_pairingInformation.IsMobileCardedSession() && 
				m_mobilePBTInfo.IsMobileFundTransferRequest() && 
				m_personalBanker->GetState() == PBStateInfoReceived && 
				!m_personalBanker->IsAuthorisationInProcess() &&
				!pbInfo->HasError());
}
