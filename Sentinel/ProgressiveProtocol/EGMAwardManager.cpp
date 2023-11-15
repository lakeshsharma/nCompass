#include "stdafx.h"
#include "EGMAwardManager.h"
#include "metersdef.h"
#include "PlayerSession.h"
#include "SASProtocol/SASProtocol.h"
#include "Logging/LogString.h"

using namespace std;

const DWORD CEGMAwardManager::LOG_TICK_COUNT_EXPIRED = 60 * MILLISECONDS_PER_SECOND;

// List of possible SAS BCD meter rollover values for 8-digit to 18-digit meters.
const __int64 CEGMAwardManager::SAS_ROLLOVER_VALUES[] =
{
	100000000LL,
	10000000000LL,
	1000000000000LL,
	100000000000000LL,
	10000000000000000LL,
	1000000000000000000LL
};

CEGMAwardManager::CEGMAwardManager(const ProgressiveLevelManager& progressiveLevelManager, MEMORY_TYPE memoryType, bool buildFromNvram) :
	m_egmAward(memoryType, buildFromNvram, NVRAM_EGM_AWARD_CHECKBYTE_OFFSET),
	m_jackpotExternalBonus(false),
	m_unlockInProgress(false),
	m_prizeClearedByPbt(false),
	m_logTickCountStart(0),m_progressiveLevelManager(progressiveLevelManager)
{
	//This is created here, but there is a function SetProgressiveWins which can also set the ProgWins obj.
	m_standardProgressiveWins = shared_ptr<IStandardProgressiveWins>(new StandardProgressiveWins(true));

	//This is created here, but there is a function SetCumulativeProgressiveManager which can also set the cumulative manager
	m_cumulativeProgressiveManager = shared_ptr<ICumulativeProgressiveManager>(new CumulativeProgressiveManager(memoryType));

	if (m_egmAward.GetAwardType() == AwardType::Prize || m_egmAward.IsFromPromoGame())
	{
		m_egmAward.Init();
	}
}

void CEGMAwardManager::Start(DWORD tickCount)
{
	if (m_egmAward.GetAwardState() == SentToGame)
	{
		if (m_egmAward.GetAwardTransferType() == EGMAwardTransferType_SASLegacyBonusing)
		{
			// For SAS legacy bonusing, if we are in this state,
			// it is unknown what was actually sent to the EGM.
			// So, wait for a meter change instead of sending again.
			m_egmAward.SetAwardState(Signaled);
			m_egmAward.SetStatusInfo(CEGMAward::TEXT_RESTART_OCCURRED_WITH_LEGACY_BONUSING_TRANSFER);
			m_egmAward.StoreYourself();
			// It is not necessary to send this state/status change to the progressive protocol.
		}
	}
	else if (m_egmAward.GetAwardState() == AftLockPending)
	{
		m_egmAward.SetAwardState(SentToGame);
		m_egmAward.StoreYourself();
		// It is not necessary to send this state/status change to the progressive protocol.
	}

	if (m_egmAward.InProgress())
	{
		// Restart the award expiration each time we restart.
		m_state.m_awardTickCountStart = tickCount;
		m_state.m_awardExpired = false;
		LogString(PROGRESSIVE, _T("AWARD: awardTickCountStart=%u"), (unsigned)tickCount);
	}
}

CEGMAward* CEGMAwardManager::GetEGMAwardCopy() const
{
	CEGMAward* egmAward(new CEGMAward(m_egmAward));
	return egmAward;
}

CEGMAwardManagerState CEGMAwardManager::GetEGMAwardManagerState() const
{
	return m_state;
}

bool CEGMAwardManager::IsEGMAwardInProgress() const
{
	bool inProgress(m_state.m_waitingForGCF);

	if (!inProgress)
	{
		inProgress = m_egmAward.InProgress();
	}

	return inProgress;
}

void CEGMAwardManager::ProcessMessage(DWORD tickCount,
	CGames& games,
	bool progressiveDown,
	CInternalMessage* msg,
	MessageQueueId source,
	CInternalMessage*& targetMsg,
	MessageQueueId& target,
	std::vector<MsgTargetPair*> *returnMessages,
	bool gameOnline)
{
	targetMsg = NULL;
	target = mqNone;

	// Check the timers.
	if (!m_state.m_awardExpired)
	{
		DWORD tickCountElapsed = tickCount - m_state.m_awardTickCountStart;
		if (m_egmAward.GetAwardState() == Signaled)
		{
		m_state.m_awardExpired = tickCountElapsed >= CEGMAwardManagerState::AWARD_TICK_COUNT_EXPIRED;
		}
		else
		{
			m_state.m_awardExpired = tickCountElapsed >= CEGMAwardManagerState::SIGNAL_TICK_COUNT_EXPIRED;
		}
		if (m_state.m_awardExpired)
		{
			LogString(PROGRESSIVE, _T("AWARD: awardTimerExpired start=%u now=%u delta=%u"),
				(unsigned)m_state.m_gcfDelayTickCountStart, (unsigned)tickCount, (unsigned)tickCountElapsed);
		}
	}

	if (!m_state.m_gcfDelayExpired && gameOnline)
	{
		DWORD tickCountElapsed = tickCount - m_state.m_gcfDelayTickCountStart;
		if (m_egmAward.GetAwardState() == AftLockPending)
		{
			m_state.m_gcfDelayExpired = tickCountElapsed >= CEGMAwardManagerState::AFT_LOCK_PENDING_TICK_COUNT_EXPIRED;
		}
		else
		{
			m_state.m_gcfDelayExpired = tickCountElapsed >= CEGMAwardManagerState::GCF_DELAY_TICK_COUNT_EXPIRED;
		}

		if (m_state.m_gcfDelayExpired)
		{
			LogString(PROGRESSIVE, _T("AWARD: sasDelayTimerExpired start=%u now=%u delta=%u awardState=\"%s\""),
				(unsigned)m_state.m_gcfDelayTickCountStart,
				(unsigned)tickCount,
				(unsigned)tickCountElapsed,
				m_egmAward.GetAwardStateString().c_str());
		}
	}

	switch (source)
	{
	case	mqProgressive:
		ProcessMessageFromProgressive(tickCount, games, progressiveDown, msg, targetMsg, target, returnMessages, gameOnline);
		break;

	case	mqGCF:
		ProcessMessageFromGCF(tickCount, games, msg, targetMsg, target, returnMessages, progressiveDown);
		break;

	case mqNone:
	default:
		PerformTimedEvents(tickCount, games, targetMsg, target, gameOnline);
		m_standardProgressiveWins->Pulse(tickCount, progressiveDown, returnMessages);
		break;
	}
}

void CEGMAwardManager::ProcessMessageFromProgressive(DWORD tickCount,
	CGames& games,
	bool progressiveDown,
	CInternalMessage* msg,
	CInternalMessage*& targetMsg,
	MessageQueueId& target,
	std::vector<MsgTargetPair*> *returnMessages,
	bool gameOnline)
{
	if (msg != NULL)
	{
		switch (msg->GetMessageType())
		{
			case INT_MSG_EGM_AWARD:
			{
				CEGMAward* egmAwardHit(msg->GetEGMAward());
				if (egmAwardHit != NULL)
				{
					ProcessEGMAwardFromProgressive(tickCount, games, progressiveDown, *egmAwardHit, targetMsg, target, returnMessages, gameOnline);
				}
				break;
			}

			case INT_MSG_SET_PROGRESSIVE_HIT_SENT:
			{
				shared_ptr<CEGMAward> award(msg->GetEGMAward());
				msg->SetData((CEGMAward*) nullptr); // Keep the instance for ourselves.
				if (award != NULL)
				{
					if (NULL != m_standardProgressiveWins)
					{
						LogString(PROGRESSIVE, _T(
							"CEGMAwardManager::ProcessMessageFromProgressive  type INT_MSG_SET_PROGRESSIVE_HIT_SENT marking as SentToSystem"));

						m_standardProgressiveWins->MarkState(award, SentToSystem);	//Tell it, it has been received by one link
					}
				}
				break;
			}
        case INT_MSG_SET_PROGRESSIVE_PAID_SENT:
            {
            
				shared_ptr<CEGMAward> award(msg->GetEGMAward());
				msg->SetData((CEGMAward*) nullptr); // Keep the instance for ourselves.
				if (award != NULL)
				{
					if (NULL != m_standardProgressiveWins)
					{
						LogString(PROGRESSIVE, _T(
							"CEGMAwardManager::ProcessMessageFromProgressive  type INT_MSG_SET_PROGRESSIVE_PAID_SENT "));

						m_standardProgressiveWins->ProgressivePaidSentReceived(award);
					}
				}
				break;
            }

            default:
                break;
		}
	}
}

void CEGMAwardManager::ProcessEGMAwardFromProgressive(DWORD tickCount,
	CGames& games,
	bool progressiveDown,
	CEGMAward& egmAwardHit,
	CInternalMessage*& targetMsg,
	MessageQueueId& target,
	std::vector<MsgTargetPair*> *returnMessages,
	bool gameOnline)
{
	if (egmAwardHit.IsStandardProgressive())
	{
		m_standardProgressiveWins->ProcessEGMAwardStandardFromProgressive(tickCount, games, progressiveDown, egmAwardHit, returnMessages );
	}
	else
	{
		ProcessEGMAwardBonusFromProgressive(tickCount, games, progressiveDown, egmAwardHit, targetMsg, target, gameOnline);
	}
}

void CEGMAwardManager::ProcessEGMAwardBonusFromProgressive(DWORD tickCount,
	CGames& games,
	bool progressiveDown,
	CEGMAward& egmAwardHit,
	CInternalMessage*& targetMsg,
	MessageQueueId& target,
	bool gameOnline)
{
	if (!egmAwardHit.IsValidJackpotHitInitalization())
	{
		// It shouldn't happen, but this award was not created or sent correctly. Don't try to process this award.
		egmAwardHit.LogInfo(_T(">> Internals received invalid jackpot hit from progressive protocol"));

		CEGMAward *egmAward = new CEGMAward(egmAwardHit);
		egmAward->SetAwardState(AwardToGameIneligible);
		egmAward->LogInfo(_T("[Internals From Progressive]"));
		targetMsg = new CInternalMessage(INT_MSG_EGM_AWARD, egmAward);
		target = mqProgressive;
	}
	else if (m_egmAward.IsSameHit(egmAwardHit) && m_egmAward.GetAwardState() != AwardToGameIneligible)
	{
		// The jackpot IDs match or current award, so send the current state/status to the progressive protocol.
		// NOTE: Allow a match to go through if ineligible because One Link could resend these to try again.
		m_egmAward.SetDuplicateHitCount(m_egmAward.GetDuplicateHitCount() + 1);
		m_egmAward.StoreYourself();
		egmAwardHit.LogInfo(_T(">> Internals receiving duplicate jackpot hit from System"));
		m_egmAward.LogInfo(_T("== Internals received duplicate jackpot hit from System"));
		targetMsg = GetMessage();
		target = mqProgressive;
	}
	else if (progressiveDown)
	{
		CEGMAward* egmAward = new CEGMAward(egmAwardHit);
		egmAward->SetAwardState(AwardToGameIneligible);
		egmAward->SetStatusInfo(CEGMAward::TEXT_INTERNALS_RECEIVED_NEW_JACKPOT_HIT_FROM_SYSTEM_WHILE_PROGRESSIVE_DOWN);
		egmAward->LogInfo(_T("[Internals From Progressive]"));
		targetMsg = new CInternalMessage(INT_MSG_EGM_AWARD, egmAward);
		target = mqProgressive;
	}
	else if (!IsEGMAwardInProgress())
	{
		if (egmAwardHit.GetAwardType() == AwardType::Type::Prize)
		{
			m_prizeClearedByPbt = false;
			m_egmAward.LogInfo(_T("== Internals accepted new jackpot Prize from System"));
			egmAwardHit.SetAwardState(SentToGame);
			m_egmAward = egmAwardHit;
			m_state.m_lastPrizeGameLock = CUtilities::GetTickCount();

			// Pass to EGM
			CEGMAward *egmAward = new CEGMAward(egmAwardHit);
			targetMsg = new CInternalMessage(INT_MSG_EGM_AWARD);
			targetMsg->SetData(egmAward);
			target = mqGCF;
		}
		else if (games.GetAftBonusEnabled() || games.GetLegacyBonusEnabled())
		{
			egmAwardHit.SetFromPromoGame(m_pendingPromoJackpotIds.end() != m_pendingPromoJackpotIds.find(egmAwardHit.GetJackpotId()));

			m_egmAward = egmAwardHit;
			m_egmAward.SetAwardState(WaitForMetersInitial);

			// For web diagnostics and debug logs, set amount paid to the amount hit value until the outcome is known.
			// If award sent to game causes aft lockup, will need amount paid for jackpot detection
			m_egmAward.SetAwardAmountPaid(m_egmAward.GetAwardAmountHit());
			m_egmAward.StoreYourself();
			m_egmAward.LogInfo(_T("== Internals accepted new jackpot hit from System"));
			m_state.m_awardTickCountStart = tickCount;
			m_state.m_awardExpired = false;
			m_state.m_gcfDelayExpired = true;
			LogString(PROGRESSIVE, _T("AWARD: awardTickCountStart=%u"), (unsigned)tickCount);
			PerformTimedEvents(tickCount, games, targetMsg, target, gameOnline);
		}
		else
		{
			CEGMAward* egmAward = new CEGMAward(egmAwardHit);
			egmAward->SetAwardState(AwardToGameIneligible);
			egmAward->SetStatusInfo(CEGMAward::TEXT_INTERNALS_DECLINED_NEW_JACKPOT_HIT_FROM_SYSTEM__NO_TRANSFER_CAPABILITY);
			egmAward->LogInfo(_T("[Internals From Progressive]"));
			targetMsg = new CInternalMessage(INT_MSG_EGM_AWARD, egmAward);
			target = mqProgressive;
		}
	}
	else // We already have an award in progress.
	{
		if (egmAwardHit.GetJackpotId() == m_egmAward.GetJackpotId())
		{
			LogString(PROGRESSIVE, _T("Received same jackpot id %lld already in progress."), m_egmAward.GetJackpotId());
		}
		else
		{
			m_egmAward.SetOtherHitCount(m_egmAward.GetOtherHitCount() + 1);
			m_egmAward.StoreYourself();
			CEGMAward* egmAward = new CEGMAward(egmAwardHit);
			string logStr(CEGMAward::TEXT_INTERNALS_DECLINED_NEW_JACKPOT_HIT_FROM_SYSTEM__AWARD_IN_PROGRESS);
			if (m_pendingPromoJackpotIds.end() == m_pendingPromoJackpotIds.find(egmAward->GetJackpotId()))
			{
				egmAward->SetAwardState(AwardToGameIneligible);
			}
			else
			{
				logStr = CEGMAward::TEXT_INTERNALS_FAILED_NEW_PROMO_AWARD_FROM_SYSTEM_AWARD_IN_PROGRESS;
				egmAward->SetFromPromoGame(true);
				egmAward->SetAwardState(AwardToGameError);
			}
			m_egmAward.LogInfo(logStr);
			egmAward->SetStatusInfo(logStr);
			egmAward->LogInfo(_T("[Internals From Progressive]"));
			targetMsg = new CInternalMessage(INT_MSG_EGM_AWARD, egmAward);
			target = mqProgressive;
		}
	}
}

CInternalMessage* CEGMAwardManager::GetMessage()
{
	CEGMAward* egmAward = new CEGMAward(m_egmAward);
	CInternalMessage* msg = new CInternalMessage(INT_MSG_EGM_AWARD, egmAward);
	return msg;
}

void CEGMAwardManager::ProcessMessageFromGCF(DWORD tickCount,
	CGames& games,
	CInternalMessage* msg,
	CInternalMessage*& targetMsg,
	MessageQueueId& target,
	std::vector<MsgTargetPair*> *returnMessages,
	bool progressiveDown)
{
	if (msg != NULL)
	{
		LogString(PROGRESSIVE, _T("EGMAward Manager ProcessMessageFromGCF message type %s"), msg->GetMessageTypeStr().c_str());
		switch (msg->GetMessageType())
		{
		case INT_MSG_EGM_AWARD:
			{
				CEGMAward* egmAwardUpdate(msg->GetEGMAward());
				if (egmAwardUpdate != NULL)
				{
					if (!m_egmAward.IsSameHit(*egmAwardUpdate))
					{
						egmAwardUpdate->LogInfo(_T("<< Internals ignoring EGM award info from GCF for a different hit"));
					}
					else
					{
						ProcessEGMAwardFromGCF(tickCount, games, *egmAwardUpdate, targetMsg, target);
					}
				}
			}
			break;

		case INT_MSG_SET_METERS:
			ProcessSetMetersMessageFromGCF(tickCount, games, msg, targetMsg, target, returnMessages);
			break;
		case INT_MSG_JACKPOT_INFO:
			ProcessJackpotInfoMessageFromGCF(tickCount, games, msg, targetMsg, target, returnMessages);
			break;
		case INT_MSG_PROGRESSIVE_WIN_HIT:
			if (false == progressiveDown)
			{
				ProcessProgressiveWinHit(msg, returnMessages);
			}
			break;
		case INT_MSG_PROGRESSIVE_WIN:
			ProcessProgressiveWin(msg, returnMessages);
			break;
		case INT_MSG_NOTIFY_GAME_START:
		{
			ProcessGameStartFromGCF(tickCount, games, targetMsg, target);
			bool processHandPayResetAfterGameStart(msg->GetWordData());
			if (processHandPayResetAfterGameStart)
			{
				LogString(PROGRESSIVE, "ProcessHandPayResetAfterGameStart");
				ProcessHandPayReset(returnMessages, true);
			}
			break;
		}
		case INT_MSG_CUMULATIVE_PROGRESSIVE_WIN:
			ProcessCumulativeProgressiveWinFromGCF(msg, returnMessages);
			break;
		case INT_MSG_HANDPAY_RESET:
			ProcessHandPayReset(returnMessages);
			break;
		case INT_MSG_PROGRESSIVE_HOPPER_PAID_HIT:
				ProcessProgressiveWinHitPaidToEGM(msg, returnMessages);
			break;
		default:
			break;
		}
	}
}

__int64 CEGMAwardManager::GetSasMeterDelta(__int64 currentMeterValue, __int64 previousMeterValue)
{
	__int64 deltaMeterValue = currentMeterValue - previousMeterValue;

	// Check to see if we had a rollover. If so, recalculate our deltaMeterValue
	if (deltaMeterValue < 0)
	{
		// Use the smallest rollover threshold that provides a non-negative delta.
		for (int index = 0; index < (int)_countof(SAS_ROLLOVER_VALUES) && deltaMeterValue < 0; ++index)
		{
			__int64 newDeltaMeterValue = deltaMeterValue + SAS_ROLLOVER_VALUES[index];
			if (newDeltaMeterValue >= 0)
			{
				deltaMeterValue = newDeltaMeterValue;
			}
		}
	}

	// If the delta is still negative then return 0.
	if (deltaMeterValue < 0)
	{
		deltaMeterValue = 0;
	}
	return deltaMeterValue;
}

void CEGMAwardManager::ProcessProgressiveWinHit(CInternalMessage *msg, std::vector<MsgTargetPair*> *returnMessages)
{
	LogString(PROGRESSIVE, _T("EGMAward Manager ProcessProgressiveWinHit message type %s"), msg->GetMessageTypeStr().c_str());
	if (NULL != m_standardProgressiveWins && false == m_standardProgressiveWins->HasUnsentProgressiveWins())		//If there are no unsent messages
	{
		LogString(PROGRESSIVE, _T("EGMAward Manager ProcessProgressiveWinHit sending to GCF INT_MSG_GET_PROGRESSIVE_WIN_INFO"));

		//This returns to process for sending back to GCF
		MsgTargetPair *newMesgPair = new MsgTargetPair;
		newMesgPair->target = mqGCF;
		newMesgPair->msg = new CInternalMessage(INT_MSG_GET_PROGRESSIVE_WIN_INFO);
		returnMessages->push_back(newMesgPair);
	}
}

void CEGMAwardManager::ProcessProgressiveWinHitPaidToEGM(CInternalMessage *msg, std::vector<MsgTargetPair*> *returnMessages)
{
	LogString(PROGRESSIVE, _T("EGMAward Manager ProcessProgressiveWinHitPaidToEGM message type %d"), msg->GetMessageType());

	//This returns to internals for sending back to GCF
	MsgTargetPair *newMesgPair = new MsgTargetPair;
	newMesgPair->target = mqGCF;
	newMesgPair->msg = new CInternalMessage(INT_MSG_GET_PROGRESSIVE_HOPPER_PAID_INFO);
	returnMessages->push_back(newMesgPair);
}

void CEGMAwardManager::ProcessHandPayReset(std::vector<MsgTargetPair*> *returnMessages, bool fromGameStart)
{
	m_standardProgressiveWins->SetHandPayReset(returnMessages, fromGameStart);
}

void CEGMAwardManager::ProcessProgressiveWin(CInternalMessage *msg, std::vector<MsgTargetPair*> *returnMessages)
{
	CEGMAward *award = msg->GetEGMAward();
	if (nullptr != award)
	{
		int64_t levelId = m_progressiveLevelManager.GetLevelIdFromSASLevel(award->GetSASLevelId());

		if (levelId != InvalidProgressiveLevelId)
		{
			m_standardProgressiveWins->SetProgressiveWin(msg, returnMessages, levelId);
		}
		else
		{
			LogString(PROGRESSIVE, _T("EGMAward Manager ProcessProgressiveWin could not find a level with the Sas Level - %d"),
				award->GetSASLevelId());
		}
	}
	else
	{
		LogString(PROGRESSIVE, _T("EGMAward Manager ProcessProgressiveWin was sent with no Award."));
	}
}

void CEGMAwardManager::ProcessCumulativeProgressiveWinFromGCF(CInternalMessage *msg, std::vector<MsgTargetPair*> *returnMessages)
{
	__int64 amount = (__int64)msg->GetInt64Data();						//Get the amount sent in the message
	__int64 delta = m_cumulativeProgressiveManager->GetCumulativeDelta(amount);

	if (delta > 0)	//make sure the value incremented
	{
		MsgTargetPair *newMesgPair = new MsgTargetPair;
		newMesgPair->target = mqGCF;
		newMesgPair->msg = new CInternalMessage(INT_MSG_GET_PROGRESSIVE_HOPPER_PAID_INFO);	//tell the SAS stack to do an 84
		returnMessages->push_back(newMesgPair);
		LogString(PROGRESSIVE, _T("EGMAwardManager::ProcessCumulativeProgressiveWinFromGCF -  Pushing message on list of return message for SAS to do a LP84."));
	}
	else
	{
		LogString(PROGRESSIVE, _T("EGMAwardManager::ProcessCumulativeProgressiveWinFromGCF -  Progressive win value did not increment."));
	}
}

void CEGMAwardManager::ProcessSetMetersMessageFromGCF(DWORD tickCount,
	CGames& games,
	CInternalMessage* msg,
	CInternalMessage*& targetMsg,
	MessageQueueId& target,
	std::vector<MsgTargetPair*> *returnMessages)
{
	// "games.GetAccountingDenom()" returns zero until we get the denomination from the EGM.
	const int accountingDenom(games.GetAccountingDenom());

	if ((m_egmAward.GetAwardState() == Signaled || m_egmAward.GetAwardState() == HandpayPending) && accountingDenom > 0)
	{
		WORD gameNumber;
		CMeters* meters(msg->GetMetersData(gameNumber));
		if (meters != NULL && gameNumber == EGM_GAME_NUMBER)
		{
			__int64 machineBonusMeterValue(INVALID_METER_VALUE);
			__int64 attendantBonusingMeterValue(INVALID_METER_VALUE);
			__int64 legacyBonusMeterValue(INVALID_METER_VALUE);

			CMeter* bonusMachMeter = meters->GetMeterByNumber(mBonusMach);
			if (bonusMachMeter != NULL)
			{
				machineBonusMeterValue = bonusMachMeter->GetValue();
			}

			const CMeter* bonusAttMeter = meters->GetMeterByNumber(mBonusAtt);
			if (bonusAttMeter != NULL)
			{
				attendantBonusingMeterValue = bonusAttMeter->GetValue();
			}

			CMeter* bonusLegacyMeter = meters->GetMeterByNumber(gBW);
			if (bonusLegacyMeter != NULL)
			{
				legacyBonusMeterValue = bonusLegacyMeter->GetValue();
			}

			__int64 machineAmount(0);
			__int64 attAmount(0);

			// Require both meters at the same time, to handle partial hand pay and machine pay.
			if (machineBonusMeterValue != INVALID_METER_VALUE && attendantBonusingMeterValue != INVALID_METER_VALUE)
			{
				machineAmount = GetSasMeterDelta(machineBonusMeterValue, m_egmAward.GetOriginalMachineBonusMeterValue());
				attAmount = GetSasMeterDelta(attendantBonusingMeterValue, m_egmAward.GetOriginalAttendantBonusingMeterValue());
			}

			// If the external bonus meters are not available, use the legacy meter values.
			if (machineAmount == 0 && attAmount == 0 && legacyBonusMeterValue != INVALID_METER_VALUE)
			{
				__int64 legacyBonusAmount = GetSasMeterDelta(legacyBonusMeterValue, m_egmAward.GetOriginalLegacyBonusMeterValue());
				if (m_egmAward.GetAwardState() == HandpayPending)
				{
					attAmount = legacyBonusAmount;
				}
				else
				{
					machineAmount = legacyBonusAmount;
				}
			}

			LogString(PROGRESSIVE, _T("EGMAwardManager::ProcessSetMetersMessageFromGCF Meters : "
				"Machine [old: %lld new: %lld] Attendant [old: %lld new: %lld] "
				"Legacy [old: %lld new: %lld] machineAmt: %lld attAmount: %lld"),
				machineBonusMeterValue, m_egmAward.GetOriginalMachineBonusMeterValue(),
				attendantBonusingMeterValue, m_egmAward.GetOriginalAttendantBonusingMeterValue(),
				legacyBonusMeterValue, m_egmAward.GetOriginalLegacyBonusMeterValue(),
				machineAmount, attAmount);

			if (machineAmount > 0 || attAmount > 0)
			{
				if (m_egmAward.GetAwardType() == AwardType::Prize)
				{
					LogString(PROGRESSIVE, _T("Internals did not create egmAward from bonus meter change because it is prize."));
				}
				else
				{
					__int64 amount = machineAmount + attAmount;

					CEGMAward egmAward(m_egmAward);
					egmAward.SetAwardAmountPaid(amount * accountingDenom);
					egmAward.SetAwardState(AwardToGameSuccess);
					egmAward.SetStatusInfo(CEGMAward::TEXT_INTERNALS_CREATED_EGM_AWARD_FROM_BONUS_METER_CHANGE);
					egmAward.LogInfo(_T("[Internals From GCF]"));

					// Set paid type
					if (machineAmount > 0)
					{
						egmAward.SetAwardPaidType(EGMAwardPaidtype_Machine);
					}
					else if (attAmount > 0)
					{
						egmAward.SetAwardPaidType(EGMAwardPaidType_Attendant);
					}

					ProcessEGMAwardFromGCF(tickCount, games, egmAward, targetMsg, target);

					// Message to go to FloorLogix
					// Currently, only 1 type of message goes to FloorLogix
					MsgTargetPair *newMesgPair = new MsgTargetPair;
					newMesgPair->msg = nullptr;
					newMesgPair->target = mqPoller;
					returnMessages->push_back(newMesgPair);
				}
			}
		}
	}
}

void CEGMAwardManager::ProcessJackpotInfoMessageFromGCF(DWORD tickCount, CGames& games, CInternalMessage* msg, CInternalMessage*& targetMsg, 
	MessageQueueId& target, std::vector<MsgTargetPair*> *returnMessages)
{
	CJackpot *jackpot = msg->GetJackpotData();
	if (jackpot != NULL)
	{
		if (jackpot->GetJackpotType() == JT_HANDPAY && m_egmAward.IsJackpotMatch(jackpot->GetJackpotAmount()))
		{
			CEGMAward egmAward(m_egmAward);

			egmAward.SetAwardState(HandpayPending);
			egmAward.SetStatusInfo(CEGMAward::TEXT_SETTING_AWARD_AS_HANDPAY_PENDING_AFTER_A_JACKPOT_IS_SET);
			egmAward.LogInfo(_T("[Internals from GCF]"));
			ProcessEGMAwardFromGCF(tickCount, games, egmAward, targetMsg, target);

			// Matches pending jackpot, set type to JT_EXTERNALBONUS
			jackpot->SetJackpotType(JT_EXTERNALBONUS);
		}
		else
		{
			const DWORD LowestProgLevelID = 0x01;
			const DWORD HighestProgLevelID = 0x20;
			if (jackpot->GetJackpotType() == JT_NONE && jackpot->GetProgressiveLevel() >= LowestProgLevelID && jackpot->GetProgressiveLevel() <= HighestProgLevelID) //Yes its a standard progressive
			{
				m_standardProgressiveWins->SetHandPayPending(jackpot->GetJackpotAmount(), jackpot->GetProgressiveLevel(), returnMessages);
			}
		}
	}
}

EGMAwardTransferType CEGMAwardManager::GetNextTransferType(CGames& games)
{
	EGMAwardTransferType awardTransferType(EGMAwardTransferType_None);

	// Check if we have the communications necessary to perform a transfer.
	if (games.GetAccountingDenom() > 0)
	{
		if (games.GetAftBonusEnabled())
		{
			awardTransferType = EGMAwardTransferType_SASAFTBonusingCash;
		}
		else if (games.GetLegacyBonusEnabled())
		{
			awardTransferType = EGMAwardTransferType_SASLegacyBonusing;
		}
	}

	return awardTransferType;
}

void CEGMAwardManager::ProcessEGMAwardFromGCF(DWORD tickCount,
	CGames& games,
	const CEGMAward& egmAwardUpdate,
	CInternalMessage*& targetMsg,
	MessageQueueId& target)
{
	egmAwardUpdate.LogInfo(_T("<< Internals received EGM award info from GCF"));
	if (m_egmAward.GetSentToGameCount() < egmAwardUpdate.GetSentToGameCount())
	{
		m_egmAward.SetSentToGameCount(egmAwardUpdate.GetSentToGameCount());
		m_egmAward.SetSentToGameTime(egmAwardUpdate.GetSentToGameTime());
	}

	// By default, start a new delay timer (for retries) each time we receive something from GCF.
	if (egmAwardUpdate.GetAwardState() != AftLockPending)
	{
		// Don't send another request to GCF while AftLockPending, because if game comm just went down
		// then we could mark a record as AwardToGameIneligible while in an unknown state.
		m_state.m_waitingForGCF = false;
	}
	m_state.m_gcfDelayTickCountStart = tickCount;
	m_state.m_gcfDelayExpired = false;
	LogString(PROGRESSIVE, _T("AWARD: gcfDelayTickCountStart=%u"), (unsigned)tickCount);

	switch (m_egmAward.GetAwardState())
	{
	case WaitForMetersInitial:
		switch (egmAwardUpdate.GetAwardState())
		{
		case WaitForMetersInitial:
			if ((egmAwardUpdate.GetOriginalMachineBonusMeterValue() != INVALID_METER_VALUE &&
				egmAwardUpdate.GetOriginalAttendantBonusingMeterValue() != INVALID_METER_VALUE) ||
				egmAwardUpdate.GetOriginalLegacyBonusMeterValue() != INVALID_METER_VALUE)
			{
				// For SAS legacy bonusing, when we send SentToGame to the EGM protocol,
				// it is unknown what was actually sent to the EGM, until we get an update from the GCF.
				EGMAwardTransferType awardTransferType(GetNextTransferType(games));
				if (awardTransferType != EGMAwardTransferType_None)
				{
					m_egmAward.SetOriginalMachineBonusMeterValue(egmAwardUpdate.GetOriginalMachineBonusMeterValue());
					m_egmAward.SetOriginalAttendantBonusingMeterValue(egmAwardUpdate.GetOriginalAttendantBonusingMeterValue());
					m_egmAward.SetOriginalLegacyBonusMeterValue(egmAwardUpdate.GetOriginalLegacyBonusMeterValue());
					m_egmAward.SetAwardTransferType(awardTransferType);
					m_egmAward.SetAwardState(SentToGame);
					m_egmAward.StoreYourself();
					m_state.m_gcfDelayExpired = true;
					targetMsg = GetMessage();
					target = mqProgressive;
				}
			}
			break;
		case AwardToGameSuccess:
		case AwardToGameError:
		case AwardToGameIneligible:
			{
				EGMAwardState newAwardState = egmAwardUpdate.GetAwardState();
				if (newAwardState == AwardToGameError)
				{
					newAwardState = AwardToGameIneligible;
				}
				m_egmAward.SetAwardState(newAwardState);
				m_egmAward.SetStatusInfo(egmAwardUpdate.GetStatusInfo());
			}
			m_egmAward.SetAwardAmountPaid(egmAwardUpdate.GetAwardState() == AwardToGameSuccess ? egmAwardUpdate.GetAwardAmountPaid() : 0);
			m_egmAward.StoreYourself();
			m_state.m_gcfDelayExpired = true;
			targetMsg = GetMessage();
			target = mqProgressive;
			break;
		default:
			break;
		}
		break;

	case SentToGame:
	case GameBusy:
	case AftLockPending:
		switch (egmAwardUpdate.GetAwardState())
		{
		case Signaled:
		case HandpayPending:
			// if you are going TO a signaled state, roll back the expired flag
			if (m_state.m_awardExpired)
			{
				LogString(PROGRESSIVE, _T("AWARD: rolling back the award expired flag since we just received signaled or handpayPending"));
				m_state.m_awardExpired = false;
			}
			m_egmAward.SetAwardState(egmAwardUpdate.GetAwardState());
			m_egmAward.SetStatusInfo(egmAwardUpdate.GetStatusInfo());
			m_egmAward.StoreYourself();
			m_state.m_gcfDelayExpired = true;
			LogString(PROGRESSIVE, _T("AWARD: sasDelayTimerExpired on state change"));
			targetMsg = GetMessage();
			target = mqProgressive;
			break;
		case GameBusy:
		case AftLockPending:
			if (m_egmAward.GetAwardState() != egmAwardUpdate.GetAwardState())
			{
				m_egmAward.SetAwardTransferType(EGMAwardTransferType_None);
				m_egmAward.SetAwardState(egmAwardUpdate.GetAwardState());
				m_egmAward.SetStatusInfo(egmAwardUpdate.GetStatusInfo());
				m_egmAward.StoreYourself();
				// A state change to GameBusy or AftLockPending is a change that does not set the retry delay to expired.
				targetMsg = GetMessage();
				target = mqProgressive;
			}
			break;
		case AwardToGameSuccess:
		case AwardToGameError:
		case AwardToGameIneligible:
			{
				EGMAwardState newAwardState = egmAwardUpdate.GetAwardState();
				if (newAwardState == AwardToGameError)
				{
					if (egmAwardUpdate.IsFromPromoGame())
					{
						LogString(PROGRESSIVE, _T("AWARD: jackpotId=%lld from promo game failed"), egmAwardUpdate.GetJackpotId());
						m_pendingPromoJackpotIds.erase(egmAwardUpdate.GetJackpotId());
					}
					else
					{
						newAwardState = AwardToGameIneligible;
					}
				}				
				m_egmAward.SetAwardState(newAwardState);
				m_egmAward.SetStatusInfo(egmAwardUpdate.GetStatusInfo());
				m_egmAward.SetAwardAmountPaid(egmAwardUpdate.GetAwardState() == AwardToGameSuccess ? egmAwardUpdate.GetAwardAmountPaid() : 0);
				m_egmAward.StoreYourself();
				m_state.m_gcfDelayExpired = true;
				LogString(PROGRESSIVE, _T("AWARD: sasDelayTimerExpired on state change"));
				targetMsg = GetMessage();
				target = mqProgressive;
			}
			break;
		default:
			break;
		}
		break;

	case Signaled:
	case HandpayPending:
		switch (egmAwardUpdate.GetAwardState())
		{
		case HandpayPending:
			if (m_egmAward.GetAwardState() == Signaled)
			{
				// if you are going TO a handpay state, roll back the expired flag
				if (m_state.m_awardExpired)
				{
					LogString(PROGRESSIVE, _T("AWARD: rolling back the award expired flag since we just received signaled or handpayPending"));
					m_state.m_awardExpired = false;
				}
				m_egmAward.SetAwardState(HandpayPending);
				m_egmAward.SetStatusInfo(egmAwardUpdate.GetStatusInfo());
				m_egmAward.StoreYourself();
				m_state.m_gcfDelayExpired = true;
				LogString(PROGRESSIVE, _T("AWARD: sasDelayTimerExpired on state change"));
				targetMsg = GetMessage();
				target = mqProgressive;
			}
			break;
		case AwardToGameSuccess:
		case AwardToGameError:
		case AwardToGameIneligible:
			{
				// Set an ineligible state to an error state.
				// We cannot go from a signaled/pending state to an ineligible state.
				EGMAwardState newAwardState = egmAwardUpdate.GetAwardState();
				if (newAwardState == AwardToGameIneligible)
				{
					newAwardState = AwardToGameError;
				}
				m_egmAward.SetAwardState(newAwardState);
				m_egmAward.SetStatusInfo(egmAwardUpdate.GetStatusInfo());
				m_egmAward.SetAwardAmountPaid(
					egmAwardUpdate.GetAwardState() == AwardToGameSuccess ? egmAwardUpdate.GetAwardAmountPaid() : 0);
				m_egmAward.SetAwardPaidType(egmAwardUpdate.GetAwardPaidType());
				m_egmAward.StoreYourself();
				m_state.m_gcfDelayExpired = true;
				LogString(PROGRESSIVE, _T("AWARD: sasDelayTimerExpired on state change"));
				targetMsg = GetMessage();
				target = mqProgressive;
				if (m_egmAward.GetAwardType() == AwardType::Type::Prize)
				{
					if (egmAwardUpdate.GetAwardState() == AwardToGameSuccess)
					{
						LogString(PROGRESSIVE, _T("ProcessEGMAwardFromGCF: Prize AwardToGameSuccess"));
						targetMsg->SetMessageType(INT_MSG_EGM_STATE);
						targetMsg->SetData((DWORD)true);
					}
					else
					{
						LogString(PROGRESSIVE, _T("ProcessEGMAwardFromGCF: Prize failed %d"), egmAwardUpdate.GetAwardState());
					}
				}
			}
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}

    ostringstream SStream;
	std::string description;
    SStream << "== Internals updated EGM award info from GCF. New msg target=" << (int)target;
    description =  SStream.str();
	m_egmAward.LogInfo(description);
}

void CEGMAwardManager::PerformTimedEvents(DWORD tickCount,
	CGames& games,
	CInternalMessage*& targetMsg,
	MessageQueueId& target,
	bool gameOnline)
{
	if (tickCount - m_logTickCountStart >= LOG_TICK_COUNT_EXPIRED)
	{
		m_logTickCountStart = tickCount;
		m_egmAward.LogInfo(_T("== PerformTimedEvents"));
	}

	// For simplicity, wait for GCF even if failure is the only option.
	if (!m_state.m_waitingForGCF)
	{
		PerformTimedEventsNotWaiting(tickCount, games, targetMsg, target, gameOnline);
		if (target != mqNone)
		{
            std::string  description;
            stringstream sStream;
            sStream << "== Internals send EGM award info target=" << (int)target;
            description = sStream.str();
			m_egmAward.LogInfo(description);
		}
	}
}

void CEGMAwardManager::PerformTimedEventsNotWaiting(DWORD tickCount,
	CGames& games,
	CInternalMessage*& targetMsg,
	MessageQueueId& target,
	bool gameOnline)
{
	switch (m_egmAward.GetAwardState())
	{
	case WaitForMetersInitial:
	case SentToGame:
	case GameBusy:
	case Signaled:
	case AftLockPending:
		// If the award has expired, set it to error or ineligible and sent it to GCF so it gets cleared out in GCF.
		// GCF will then send it back to internals.
		// NOTE: Pending awards do not get expired.
		if (m_state.m_awardExpired)
		{
			if (AwardType::Type::Prize == m_egmAward.GetAwardType())
			{
				CheckIfPrizeAwardLockTimeNeedsToBeRefreshed(targetMsg, target);
			}
			else
			{
				CEGMAward* egmAward = new CEGMAward(m_egmAward);
				egmAward->SetAwardState(m_egmAward.GetAwardState() == Signaled ? AwardToGameError : AwardToGameIneligible);
				egmAward->SetStatusInfo(CEGMAward::TEXT_INTERNALS_EXPIRED_EGM_AWARD__SENDING_TO_GCF_TO_BE_CLEARED_THERE_FIRST);
				egmAward->LogInfo(_T("[Internals to GCF]"));
				targetMsg = new CInternalMessage(INT_MSG_EGM_AWARD, egmAward);
				target = mqGCF;
			}
		}
		else if (m_state.m_gcfDelayExpired && gameOnline)
		{
			// Don't send another request to GCF while AftLockPending  If game comm just went down
			// then we could mark a record as AwardToGameIneligible while in an unknown state.
			if (m_egmAward.GetAwardState() == GameBusy)
			{
				EGMAwardTransferType awardTransferType(GetNextTransferType(games));
				if (awardTransferType != EGMAwardTransferType_None)
				{
					m_egmAward.SetAwardTransferType(awardTransferType);
					m_egmAward.SetAwardState(SentToGame);
					m_egmAward.StoreYourself();
					m_egmAward.LogInfo(_T("== Internals retry after game busy"));
				}
			}

			if (m_egmAward.GetAwardState() != GameBusy && m_egmAward.GetAwardState() != AftLockPending)
			{
				m_state.m_waitingForGCF = true;
				m_state.m_gcfDelayTickCountStart = tickCount;
				m_state.m_gcfDelayExpired = false;
				LogString(PROGRESSIVE, _T("AWARD: gcfDelayTickCountStart=%u"), (unsigned)tickCount);
				targetMsg = GetMessage();
				target = mqGCF;
			}
		}
		else
		{
			CheckIfPrizeAwardLockTimeNeedsToBeRefreshed(targetMsg, target);
		}
		break;

	default:
		break;
	}
}

void CEGMAwardManager::SetJackpotExternalBonusFeature(bool enabled)
{
	m_jackpotExternalBonus = enabled;
}

bool CEGMAwardManager::UnlockInProgress() const
{
	return m_unlockInProgress;
}

void CEGMAwardManager::SetUnlockInProgress(bool enabled)
{
	m_unlockInProgress = enabled;
}

bool CEGMAwardManager::PrizeClearedByPbt() const
{
	return m_prizeClearedByPbt;
}

void CEGMAwardManager::SetPrizeClearedByPbt(bool value)
{
	m_prizeClearedByPbt = value;
}

void CEGMAwardManager::SetProgressiveWins(shared_ptr<IStandardProgressiveWins> progressiveWins)
{
	m_standardProgressiveWins = progressiveWins;
}

void CEGMAwardManager::GetStandardWins(std::shared_ptr<CEGMAward> &lastStandardWin,
	std::vector<std::shared_ptr<CEGMAward>> &standardWins)
{
	m_standardProgressiveWins->GetLastStandardWin(lastStandardWin);
	m_standardProgressiveWins->GetStandardWins(standardWins);

}

void CEGMAwardManager::SetCumulativeProgressiveManager(shared_ptr<ICumulativeProgressiveManager> cumulativeProgressiveManager)
{
	m_cumulativeProgressiveManager = cumulativeProgressiveManager;
}

void CEGMAwardManager::ProcessGameStartFromGCF(DWORD tickCount,
	CGames& games,
	CInternalMessage*& targetMsg,
	MessageQueueId& target)
{
	if (m_egmAward.GetAwardState() == HandpayPending)
	{
		// Set award as paid since waiting for handpay and a new game started
		CEGMAward egmAward(m_egmAward);
		
		// Keep previously set amount paid
		egmAward.SetAwardState(AwardToGameSuccess);
		egmAward.SetStatusInfo(CEGMAward::TEXT_HAND_PAY_PENDING_RESOLVED_BY_GAME_START);
		egmAward.SetAwardAmountPaid(egmAward.GetAwardAmountHit());
		egmAward.LogInfo(_T("<< Internals created egmAward from NOTIFY_GAME_START msg"));
		ProcessEGMAwardFromGCF(tickCount, games, egmAward, targetMsg, target);
	}
}

void CEGMAwardManager::CheckIfPrizeAwardLockTimeNeedsToBeRefreshed(CInternalMessage*& targetMsg, MessageQueueId& target)
{
	if (m_egmAward.GetAwardType() == AwardType::Type::Prize && (m_egmAward.GetAwardState() == Signaled || m_egmAward.GetAwardState() == SentToGame))
	{
		if (CUtilities::GetTickCount() - m_state.m_lastPrizeGameLock > CEGMAwardManagerState::PRIZE_AWARD_LOCK_REFRESH && !m_unlockInProgress && !m_prizeClearedByPbt)
		{
			LogString(PROGRESSIVE, "PRIZE_AWARD_LOCK_REFRESH");
			m_state.m_lastPrizeGameLock = CUtilities::GetTickCount();
			targetMsg = GetMessage();
			target = mqGCF;
		}
	}
}

bool CEGMAwardManager::IsPrizeAwardActive(int64_t& jackpotId) const
{
	bool isPrizeAwardActive(m_egmAward.GetAwardType() == AwardType::Type::Prize && (m_egmAward.GetAwardState() == Signaled || m_egmAward.GetAwardState() == SentToGame));
	jackpotId = isPrizeAwardActive ? m_egmAward.GetJackpotId() : 0;
	return isPrizeAwardActive;
}

void CEGMAwardManager::AddPendingPromoJackpotId(int64_t jackpotId)
{
	m_pendingPromoJackpotIds.insert(jackpotId);
}