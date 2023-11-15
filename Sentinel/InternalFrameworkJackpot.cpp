#include "stdafx.h"
#include "InternalsFramework.h"
#include "UI/AttendantPaidPayoutLog.h"
#include "Logging/LogString.h"

using namespace std;

void CInternalsFramework::InitializeJackpot(void)
{
	lock_guard<recursive_mutex> lock(m_jackpotCriticalSection);

	if (CJackpot::DoesJackpotExistInNVRAM())
	{
		LogString(JACKPOT, _T("Jackpot restored from NVRAM at startup"));
		m_jackpot = new CJackpot(MEMORY_NVRAM, true);
		m_jackpot->SetTaxWithholdingConfig(m_config.UJPTaxWithholding(), m_config.GetFederalTaxWithholdingPercent(), m_config.GetStateTaxWithholdingPercent());
		if (!m_jackpot->IsTransactionSent())
		{
			LogString(JACKPOT, _T("Unattended jackpot current state = %s"), m_jackpot->GetUnattendedStateStr());
			SendJackpotTransactionToSCF(m_jackpot);
		}

		// If UJP is in process, cancel it
		if (m_jackpot->IsUnattendedJackpotInProgress())
		{
			// Send cancel to poller
			LogString(JACKPOT, "%s %d", __FUNCTION__, __LINE__);
			m_jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledReboot);
			SendUJPToPoller(new CJackpot(*m_jackpot));
		}
		CJackpot::LogInfo(m_jackpot, _T("InitializeJackpot from NVRAM"));
	}

}

void CInternalsFramework::UninitializeJackpot(void)
{
	lock_guard<recursive_mutex> lock(m_jackpotCriticalSection);

	if (m_jackpot != NULL)
	{
		delete m_jackpot;
		m_jackpot = nullptr;
		CJackpot::LogInfo(m_jackpot, _T("UninitializeJackpot"));
	}
}

void CInternalsFramework::SendJackpotTransactionToSCF(CJackpot *jackpotToSend)
{
	if (jackpotToSend)
	{
		lock_guard<recursive_mutex> lock(m_jackpotCriticalSection);

		LogString(JACKPOT, _T("Sending jackpot to poller, amount = %lld"), jackpotToSend->GetJackpotAmount());
		CInternalMessage *transMsg = PrepareTransactionMessage(INT_MSG_TRANSACTION_JACKPOT, IsPlayerVirtual());

		SetPlayerCardIDToUJPTransaction(transMsg);

		CJackpot *jackpot = new CJackpot(*jackpotToSend);

		// Add the jackpot object to the transaction
		transMsg->SetData(jackpot);

		// Add the machine number to the transaction
		transMsg->SetData((DWORD)m_config.GetSlotNumber());

		if (jackpot->GetJackpotTransactionType() == JTRANS_COMPOSITE)
		{
			m_attnPaidLog->Add(new CAttendantPaidPayoutLog(jackpot->GetJackpotAmount(), jackpot->GetJackpotTransactionType()));
		}

		JackpotTransactionType jpType =jackpot->GetJackpotTransactionType();
		if (jpType == JTRANS_PROGRESSIVE_HOPPER_PAID || (jpType == JTRANS_COMPOSITE && jackpot->GetJackpotType() == JT_NONE))
		{
			// Poller protocol is going add on a 116 transaction, so add it to the transaction log here
			m_transLog->Add(new CTransactionLog(PROGRESSIVE_JP_SCANNER_CODE, jackpot->GetJackpotAmount(), *transMsg->GetTimeData()));
		}

		if (jackpot->GetJackpotType() != JT_CANCEL_CREDIT)
		{
			m_excessiveVoucherOut.ProcessCoinOut(jackpot->GetJackpotAmount());
		}

		CJackpot::LogInfo(jackpotToSend, _T("SendTransactionMessageToSCF"));
		SendTransactionMessageToSCF(transMsg, QUEUE_PRIORITY_NORMAL);
	}
}

/// <summary>
/// Handle the Jackpot Hit
/// </summary>
/// <param name="msg">The internal Message.</param>
void CInternalsFramework::JackpotHit(CInternalMessage *msg)
{
	if (INT_MSG_JACKPOT_HIT == msg->GetMessageType())
	{
		m_playerSession.SetJackpotPending(true);
	}	

	std::unique_lock<std::recursive_mutex> lock(m_jackpotCriticalSection);
	CJackpot::LogInfo(m_jackpot, _T("JackpotHit"));
	LogString(JACKPOT, _T("Jackpot Hit"));
	if (m_jackpot != nullptr)
	{
		LogString(JACKPOT, "%s %d current state = %s", __FUNCTION__, __LINE__, m_jackpot->GetUnattendedStateStr());
	}
	lock.unlock();

	switch (msg->GetMessageType())
	{
	case INT_MSG_PROGRESSIVE_HOPPER_PAID_HIT:
	case INT_MSG_PROGRESSIVE_WIN:
	case INT_MSG_PROGRESSIVE_WIN_HIT:
		ProcessProgressiveWinHit(msg);							//Process the win
		break;
	default:
		lock.lock();
		// Wait if we did not send the jackpot to Poller. SAS will send again.
		if (m_jackpot == NULL || m_jackpot->IsJackpotComplete())
		{
			//Tell GCF to get the Jackpot Info
			LogString(JACKPOT, _T("Getting jackpot info."));
			SendMessageToGCF(new CInternalMessage(INT_MSG_GET_JACKPOT_INFO), QUEUE_PRIORITY_NORMAL);
		}
		lock.unlock();
		break;
	}
}


/// <summary>
/// Processes the progressive win Hit.
/// </summary>
/// <param name="msg">The MSG.</param>
void CInternalsFramework::ProcessProgressiveWinHit(CInternalMessage *msg)
{
	unique_ptr<CProgressiveConfig> config(m_sentinelState.GetProgressiveConfigCopy());
	if (NULL != config && true == config->GetActive())                      //Is the progressive active
	{
		LogString(PROGRESSIVE, _T("Processing Progressive Win Info with Progressive Active"));
		ProcessEgmAwardMessage(msg, mqGCF);
	}
    else if (msg->GetMessageType() == INT_MSG_PROGRESSIVE_HOPPER_PAID_HIT)
	{
		LogString(PROGRESSIVE, _T("Processing INT_MSG_PROGRESSIVE_HOPPER_PAID_HIT"));
		SendMessageToGCF(new CInternalMessage(
			INT_MSG_GET_PROGRESSIVE_HOPPER_PAID_INFO), QUEUE_PRIORITY_NORMAL);	//Tell GCF to get Hopper Paid Info
	}
	else if (msg->GetMessageType() != INT_MSG_PROGRESSIVE_WIN)  // Prevent infinite SAS LP85 / INT_MSG_PROGRESSIVE_WIN loop.
	{
		LogString(JACKPOT, _T("Processing Progressive Win Info with Progressive NOT Active"));
		SendMessageToGCF(new CInternalMessage(
								  INT_MSG_GET_PROGRESSIVE_WIN_INFO), QUEUE_PRIORITY_NORMAL);           //Tell GCF to get prog win info
	}
}

void CInternalsFramework::ProcessHandPayReset(CInternalMessage *msg)
{
	{
		lock_guard<recursive_mutex> lock(m_jackpotCriticalSection);
		CJackpot::LogInfo(m_jackpot, _T("ProcessHandPayReset check"));

		m_playerSession.SetJackpotPending(false);

		//If it is hand paid jackpot then set the reset true, not for jackpot credited to account
        if (m_jackpot)
        {
			ProcessVoucherOut((DWORD)m_jackpot->GetJackpotAmount());
			m_jackpot->SetHandPayReset(true);
            LogString(JACKPOT, _T("Hand Pay reset done"));

			PlayerSessionChangeEventType evtType = (m_jackpot->GetJackpotType() != JT_CANCEL_CREDIT) ?
				SESSION_CHANGE_EVENT_JACKPOT_RESET : SESSION_CHANGE_EVENT_GAMEPLAY;

			// Jackpot handpay reset done either manually or via UTJ, send updated  
			// player session information to Poller/PDS which includes jackpot state
			SendMCPlayerSessionInfoToPollerProtocol(evtType);
        }

		// Look for UJP to cancel
		if (m_jackpot != nullptr && m_jackpot->GetUnattendedState() == UnattendedJackpotState::UJPPinEntry)
		{
			LogString(JACKPOT, "%s %d", __FUNCTION__, __LINE__);
			m_jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledHandpayReset);
			SendUJPToPoller(new CJackpot(*m_jackpot));
	
			// Clear UI queue starvation to clear pinpad
			m_protocolManager->ClearInboundQueueStarvation(IProtocolManager::UIProtocol, PERSIST_CONTENT_WINDOW);
			CJackpot::LogInfo(m_jackpot, _T("ProcessHandPayReset updated"));
		}
	}

	unique_ptr<CProgressiveConfig> config(m_sentinelState.GetProgressiveConfigCopy());
	if (NULL != config && true == config->GetActive())								//Is the progressive active
	{
		LogString(PROGRESSIVE, _T("Processing Hand Pay Reset Info with Progressive Active"));
		ProcessEgmAwardMessage(msg, mqGCF);
	}
}

void CInternalsFramework::ProcessJackpot(CJackpot *jackpot)
{
	if (JTRANS_COMPOSITE == jackpot->GetJackpotTransactionType())
	{
		lock_guard<recursive_mutex> lock(m_jackpotCriticalSection);
		CJackpot::LogInfo(m_jackpot, _T("ProcessJackpot info (prior)"));

		if (NULL == m_jackpot || m_jackpot->IsJackpotComplete())
		{
			if (NULL != m_jackpot)
			{
				m_dataSubscriptionManager.UnregisterProvider(m_jackpot);
				delete m_jackpot;
				m_jackpot = NULL;
			}

			m_jackpot = new CJackpot(MEMORY_NVRAM);
			*m_jackpot = *jackpot;
			m_jackpot->SetTaxWithholdingConfig(m_config.UJPTaxWithholding(), m_config.GetFederalTaxWithholdingPercent(), m_config.GetStateTaxWithholdingPercent());
			m_dataSubscriptionManager.RegisterProvider(m_jackpot);

			const byte *cardID(nullptr);
			int cardLength(0);
			{
				lock_guard<recursive_mutex> playerLock(m_playerCriticalSection);
				if (m_player != nullptr)
				{					
					cardID = m_player->GetCard()->GetCardID(cardLength);
				}
			}
			m_jackpot->SetJackpotData(cardID, cardLength, m_config.GetUJPEnabled() && m_jackpot->IsKeyToCreditAvailable(), m_config.GetLastCoinsPlayed());

			// Adjust key to credit available based on config
			LogString(JACKPOT, "%s %d config=%u keytocredit=%u currentState=%s TaxWithholdingEnabled=%u FederalTaxWithholdingPercent=%f StateTaxWithholdingPercent=%f",
				__FUNCTION__, __LINE__, m_config.GetUJPEnabled(), m_jackpot->IsKeyToCreditAvailable(), m_jackpot->GetUnattendedStateStr(), m_jackpot->IsTaxWithholdingEnabled(), m_jackpot->GetFederalTaxWithholdingPercent(), m_jackpot->GetStateTaxWithholdingPercent());
			SendJackpotTransactionToSCF(m_jackpot);

			if (m_jackpot->GetJackpotTransactionType() == JTRANS_COMPOSITE)
			{
				PlayerSessionChangeEventType evtType = (m_jackpot->GetJackpotType() != JT_CANCEL_CREDIT) ?
					SESSION_CHANGE_EVENT_JACKPOT_LOCKUP : SESSION_CHANGE_EVENT_GAMEPLAY;

				// Handpay triggerred for standard and progressive jackpots
				// Send updated player session information to Poller which includes jackpot state
				SendMCPlayerSessionInfoToPollerProtocol(evtType);
			}

			CJackpot::LogInfo(m_jackpot, _T("Processing jackpot info updated"));
		}
		else
		{
			LogString(JACKPOT, "Jackpot info cannot be processed if one is already in process");
			CJackpot::LogInfo(m_jackpot, _T("Jackpot info cannot be processed if one is already in process"));
		}
	}
	else if(JTRANS_PROGRESSIVE_HOPPER_PAID == jackpot->GetJackpotTransactionType())
	{
		LogString(JACKPOT, "%s %d", __FUNCTION__, __LINE__);
		SendJackpotTransactionToSCF(jackpot);
	}
	else
	{
		LogString(JACKPOT, "%s %d", __FUNCTION__, __LINE__);
	}
}


void CInternalsFramework::ProcessJackpotTransactionStored(void)
{
	lock_guard<recursive_mutex> lock(m_jackpotCriticalSection);

	if (m_jackpot != NULL)
	{
		m_jackpot->SetTransactionSent();
	}

	CJackpot::LogInfo(m_jackpot, _T("ProcessJackpotTransactionStored"));
}

void CInternalsFramework::CheckIfUJPNeedsToBeCancelled(int buttonId)
{
	lock_guard<recursive_mutex> lock(m_jackpotCriticalSection);

	if(m_jackpot != nullptr && m_jackpot->GetUnattendedState() == UnattendedJackpotState::UJPPinEntry)
	{
		LogString(JACKPOT, "%s %d buttonId=%d", __FUNCTION__, __LINE__, buttonId);
		m_jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledTaskbarButtonPress);
		SendUJPToPoller(new CJackpot(*m_jackpot));
	}
}

TransactionType CInternalsFramework::DetermineJackpotTransType(CJackpot *jackpot)
{
	TransactionType transType = COMPOSITE_JACKPOT_CODE;
	if (jackpot != nullptr)
	{
		LogString(GENERAL, _T("Logging Jackpot, type = %d"), jackpot->GetJackpotTransactionType());

		switch (jackpot->GetJackpotTransactionType())
		{
		case JTRANS_PROGRESSIVE_HOPPER_PAID:
			transType = PROGRESSIVE_JP_HOPPER_CODE;
			break;
		case JTRANS_PROGRESSIVE_SCANNER_ONLY:
			transType = PROGRESSIVE_JP_SCANNER_CODE;
			break;
		default:
			break;
		}
	}

	return transType;
}

void CInternalsFramework::PerformUnattendedJackpotCheck()
{
	lock_guard<recursive_mutex> lock(m_jackpotCriticalSection);
	
	if (m_jackpot != nullptr && m_jackpot->IsExpiredUnattendedJackpot(m_msgPriority.GetQueuingConfig().GetUjpTimeout()))
	{
		// Jackpot was canceled, send notifications
		LogString(JACKPOT, "%s %d", __FUNCTION__, __LINE__);
		m_jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledTimedOut);
		SendSubscriptionUpdateIfNecessary();
		SendUJPToPoller(new CJackpot(*m_jackpot));
	}	
}

void CInternalsFramework::SendUJPToPoller(CJackpot *jackpot)
{
	CJackpot::LogInfo(m_jackpot, FormatString(_T("SendUJPToPoller %s"), jackpot->GetJackpotStatusStr()).c_str());
	CInternalMessage *pollerMsg = PrepareTransactionMessage(INT_MSG_UNATTENDED_JACKPOT, IsPlayerVirtual());
	pollerMsg->SetData(jackpot);
	SetPlayerCardIDToUJPTransaction(pollerMsg);
	pollerMsg->SetData((WORD)jackpot->GetJackpotStatus());
	SendTransactionMessageToSCF(pollerMsg, QUEUE_PRIORITY_NORMAL);
}

void CInternalsFramework::SetPlayerCardIDToUJPTransaction(CInternalMessage *pollerMsg)
{
	if (m_player)
	{	
        lock_guard<recursive_mutex> lock(m_playerCriticalSection);
		if (m_player)
		{
			int cardIDLen = 0;
			const CCard* card = m_player->GetCard();
			byte * cardID = new byte[CARD_ID_LEN];
			memcpy(cardID, card->GetCardID(cardIDLen), CARD_ID_LEN);
			pollerMsg->SetData(cardID, CARD_ID_LEN);
		}
	}

}

void CInternalsFramework::ProcessTaxDeductionComplete(CInternalMessage *msg)
{
	if (msg)
	{
		CPersonalBankerTransfer* pbTransfer(msg->GetPBTransferData());		
		lock_guard<recursive_mutex> lock(m_jackpotCriticalSection);
		if (m_jackpot && pbTransfer)
		{

			bool isSessionReleasedDueToTimeout(msg->GetWordData() == 1);
			bool canceledCashUpload(msg->GetWordData(true) == 1);
			LogString(JACKPOT, "UJPScreen::Complete with Tax Deduction PBType=%u JackpotAmount=$%llu isSessionReleasedDueToTimeout=%u",
				pbTransfer->GetPersonalBankerType(), m_jackpot->GetJackpotAmount() / ONE_DOLLAR_IN_CENTS, isSessionReleasedDueToTimeout);
			if (isSessionReleasedDueToTimeout || canceledCashUpload)
			{
				LogString(JACKPOT, "Withholding taxes failed");
				GenerateTilt(tltTaxDeductionFailed);
				m_jackpot->SetTaxWithholding(0, 0, false, false);
			}
			SendSubscriptionUpdateIfNecessary();
		}
	}	
}
