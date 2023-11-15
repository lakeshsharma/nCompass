#include "stdafx.h"
#include "StandardProgressiveWins.h"
#include "ProgressiveLevel.h"

using namespace std;

const DWORD StandardProgressiveWins::QUEUE_RETRY_EXPIRED = 15 * MILLISECONDS_PER_SECOND; //seconds
const DWORD StandardProgressiveWins::QUEUE_HIT_PUNT_EXPIRED = 10 * MILLISECONDS_PER_MINUTE; //minutes

StandardProgressiveWins::StandardProgressiveWins(bool buildFromNvram)
{
	BuildYourself(buildFromNvram);
	m_queueRetryTickCountStart = 0;
}

StandardProgressiveWins::~StandardProgressiveWins()
{
}

void StandardProgressiveWins::SetInactiveLocation()
{
	for (DWORD index(0); index < NVRAM_EGM_AWARD_STANDARD_DATA_COUNT; ++index)
	{
		DWORD nvramActiveOffset(NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET + index * NVRAM_EGM_AWARD_STANDARD_ITEM_SIZE);
		if (!CMemoryStorage::SetInactiveLocation(nvramActiveOffset, (BYTE)~NVRAM_EGM_AWARD_CHECKBYTE_VALUE))
		{
			LogString(ERROR_LOG, _T("Could not write Standard Award inactive check byte to NVRAM"));
		}
	}

	CMemoryStorage::SetInactiveLocation(NVRAM_PREV_AWARD_ACTIVE_OFFSET, (BYTE)~NVRAM_PREV_AWARD_ACTIVE_VALUE);
}

void StandardProgressiveWins::BuildYourself(bool buildFromNvram)
{
	LogString(PROGRESSIVE, _T("StandardProgressiveWins::BuildYourself(buildFromNvram=%d)"), (int)buildFromNvram);
	for (DWORD index(0); index < NVRAM_EGM_AWARD_STANDARD_DATA_COUNT; ++index)
	{
		bool offsetIsAvailable(true);
		DWORD nvramActiveOffset(NVRAM_EGM_AWARD_STANDARD_CHECKBYTE_OFFSET + index * NVRAM_EGM_AWARD_STANDARD_ITEM_SIZE);

		if (buildFromNvram)
		{
			auto egmAward = make_shared<CEGMAward>(MEMORY_NVRAM, true, nvramActiveOffset);
			if (NoAward != egmAward->GetAwardState())
			{
				egmAward->LogInfo(_T("StandardProgressiveWins -- Restart occurred"));
				m_Awards.push_back(egmAward);
				offsetIsAvailable = false; // NVRAM offset is not available -- it is in use by a progressive in progress.
			}
		}

		if (offsetIsAvailable)
		{
			// Add offset to container for available offsets.
			m_availableNvramActiveOffsets.push(nvramActiveOffset);
		}
	}

	BuildLastAwardProcessed(buildFromNvram);
}

void StandardProgressiveWins::BuildLastAwardProcessed(bool buildFromNvram)
{
	if (buildFromNvram)
	{
		m_lastAwardProcessed.reset(new CEGMAward);
		m_lastAwardProcessed->SetMemoryStorageParameters(MEMORY_NVRAM,
			NVRAM_PREV_AWARD_DATA_OFFSET,
			NVRAM_PREV_AWARD_DATA_SIZE,
			NVRAM_PREV_AWARD_ACTIVE_OFFSET,
			NVRAM_PREV_AWARD_ACTIVE_VALUE);
		m_lastAwardProcessed->BuildYourself(true);
		m_lastAwardProcessed->LogInfo(_T("StandardProgressiveWins -- m_lastAwardProcessed->BuildYourself(true)"));
		if (NoAward == m_lastAwardProcessed->GetAwardState())
		{
			m_lastAwardProcessed.reset();
		}
	}
}

bool StandardProgressiveWins::HasUnsentProgressiveWins()
{
	auto it = find_if(m_Awards.cbegin(), m_Awards.cend(), [&](const shared_ptr<CEGMAward>& award) -> bool
	{
		if (award->isHitReceived() == false)
		{
			return true;
		}
		return false;
	});

	if (it != m_Awards.cend())
	{
		return true;
	}
	return false;
}

DWORD StandardProgressiveWins::GetNextNvramActiveOffset()
{
	DWORD nvramActiveOffset(CMemoryStorage::NVRAM_DEFAULT_OFFSET);
	if (!m_availableNvramActiveOffsets.empty())
	{
		nvramActiveOffset = m_availableNvramActiveOffsets.front();
		m_availableNvramActiveOffsets.pop();
	}

	return nvramActiveOffset;
}

void StandardProgressiveWins::RecoverNvramActiveOffset(shared_ptr<CEGMAward> egmAward)
{
	if (egmAward.get() != nullptr)
	{
		DWORD nvramActiveOffset(egmAward->GetNvramActiveOffset());
		if (nvramActiveOffset != CMemoryStorage::NVRAM_DEFAULT_OFFSET)
		{
			egmAward->SetActiveLocation(false);
			m_availableNvramActiveOffsets.push(nvramActiveOffset);
		}
	}
}

bool StandardProgressiveWins::AddProgressiveWin(shared_ptr<CEGMAward> award)
{
	if (NULL == award)
	{
		return false;
	}

	award->SetMemoryStorage(GetNextNvramActiveOffset());
	m_Awards.push_back(award);
	award->LogInfo(_T("StandardProgressiveWins::AddProgressiveWin"));

	LogString(
		PROGRESSIVE,
		_T("StandardProgressiveWins::AddProgressiveWin Award - JackpotID:%lld - levelid:%d - amount:%lld"),
		award->GetJackpotId(),
		award->GetLevelId(),
		award->GetAwardAmountHit());

	return true;
}

bool StandardProgressiveWins::MarkState(shared_ptr<CEGMAward> award, EGMAwardState state)
{
	LogString(PROGRESSIVE, _T("StandardProgressiveWins MarkState - new state:%s - amount:%lld - Level:%d - SAS Level:%d"),
		award->GetAwardStateString().c_str(),
		award->GetAwardAmountHit(),
		award->GetLevelId(),
		award->GetSASLevelId());

	for (auto awardIterator = m_Awards.cbegin(); awardIterator != m_Awards.cend(); ++awardIterator)
	{
		if (((*awardIterator)->GetAwardAmountHit() == award->GetAwardAmountHit()) &&
			((*awardIterator)->GetLevelId() == award->GetLevelId())) //if the amount and level match
		{
			LogString(PROGRESSIVE, _T("StandardProgressiveWins MarkState Found award, marking - old state:%s - new state:%s - amount:%lld - Level:%d"),
				(*awardIterator)->GetAwardStateString().c_str(),
				award->GetAwardStateString().c_str(),
				award->GetAwardAmountHit(),
				award->GetLevelId());

			(*awardIterator)->SetAwardState(state); //store in the list
			(*awardIterator)->StoreYourself();
			return true;
		}
	}
	return false;
}

void StandardProgressiveWins::ProgressivePaidSentReceived(shared_ptr<CEGMAward> egmAwardHit)
{
	shared_ptr<CEGMAward> pendingAward(FindAward(egmAwardHit->GetAwardAmountHit(), egmAwardHit->GetLevelId()));
	if (nullptr != pendingAward)
	{
		RemoveAward(egmAwardHit->GetAwardAmountHit(), egmAwardHit->GetLevelId());
	}
}

void StandardProgressiveWins::GetLastStandardWin(std::shared_ptr<CEGMAward> &lastStandardWin)
{
	if (m_lastAwardProcessed.get() != nullptr)
	{
		lastStandardWin.reset(new CEGMAward(*m_lastAwardProcessed));
	}
	else
	{
		lastStandardWin.reset();
	}
}

void StandardProgressiveWins::GetStandardWins(std::vector<std::shared_ptr<CEGMAward>> &standardWins)
{
	standardWins.clear();

	for (auto awardIterator = m_Awards.cbegin(); awardIterator != m_Awards.cend(); ++awardIterator)
	{
		if (*awardIterator != nullptr && (*awardIterator).get() != nullptr)
		{
			shared_ptr<CEGMAward> award(new CEGMAward(**awardIterator));
			standardWins.push_back(award);
		}
	}
}

void StandardProgressiveWins::ProcessEGMAwardStandardFromProgressive(DWORD tickCount,
	CGames& games,
	bool progressiveDown,
	const CEGMAward& egmAwardHit,
	std::vector<MsgTargetPair*> *returnMessages)
{
    UNREFERENCED_PARAMETER(&tickCount); 
    UNREFERENCED_PARAMETER(&games); 
    UNREFERENCED_PARAMETER(&progressiveDown); 
	LogString(
		PROGRESSIVE,
		_T("StandardProgressiveWins::ProcessEGMAwardStandardFromProgressive Award - JackpotID:%lld - levelid:%d - amount:%lld"),
		egmAwardHit.GetJackpotId(),
		egmAwardHit.GetLevelId(),
		egmAwardHit.GetAwardAmountHit());

	shared_ptr<CEGMAward> pendingAward(FindAward(egmAwardHit.GetAwardAmountHit(), egmAwardHit.GetLevelId()));
	if (nullptr != pendingAward)
	{
		LogString(
			PROGRESSIVE,
			_T("StandardProgressiveWins::ProcessEGMAwardStandardFromProgressive Found Pending Award - state:%s -JackpotID:%lld - levelid:%d - amount:%lld"),
			pendingAward->GetAwardStateString().c_str(),
			pendingAward->GetJackpotId(),
			pendingAward->GetLevelId(),
			pendingAward->GetAwardAmountHit());

		if (pendingAward->GetJackpotId() == 0 || pendingAward->GetJackpotId() == -1)		//to get in here we want jackpotID==0 or -1(not Set), matching level and amount.
		{
			pendingAward->SetJackpotId(egmAwardHit.GetJackpotId());
			pendingAward->SetRemoteJackpotId(egmAwardHit.GetRemoteJackpotId());
			pendingAward->SetAwardAmountPaid(egmAwardHit.GetAwardAmountHit());
			if (pendingAward->isEgmPaidSignaled())
			{
				ProcessEGMAwardWithEGMPaidSignaled(pendingAward, egmAwardHit, returnMessages);
			}
			else
			{
				ProcessEGMAwardWithNoEGMPaidSignaled(pendingAward, egmAwardHit, returnMessages);
			}
		}
		else
		{
			LogString(
				PROGRESSIVE,
				_T("StandardProgressiveWins ProcessEGMAwardStandardFromProgressive received Jackpot from progressive that already had an ID - JackpotID:%lld - levelid:%d - amount:%lld"),
				pendingAward->GetJackpotId(),
				pendingAward->GetLevelId(),
				pendingAward->GetAwardAmountHit());
		}

		pendingAward->LogInfo(_T("-- Exit StandardProgressiveWins ProcessEGMAwardStandardFromProgressive"));
	}
}

void StandardProgressiveWins::ProcessEGMAwardWithNoEGMPaidSignaled(shared_ptr<CEGMAward> pendingAward, const CEGMAward &egmAwardHit, std::vector<MsgTargetPair*> *returnMessages)
{
	pendingAward->SetHitReceived(true);
	pendingAward->StoreYourself();
	if (pendingAward->isHandpayPendingSignaled())
	{
		CEGMAward* egmAward = new CEGMAward(egmAwardHit);
		egmAward->SetAwardState(HandpayPending);	//Tell one link hand pay pending
        egmAward->SetAwardAmountPaid(egmAward->GetAwardAmountHit());

		MsgTargetPair *newMesgPair = new MsgTargetPair;
		newMesgPair->target = mqProgressive;
		newMesgPair->msg = new CInternalMessage(INT_MSG_EGM_AWARD, egmAward);
		returnMessages->push_back(newMesgPair);

		LogString(
			PROGRESSIVE,
			_T("StandardProgressiveWins ProcessEGMAwardStandardFromProgressive Sending Hand Pay Pending to one link - JackpotID:%lld - levelid:%d - amount:%lld"),
			egmAwardHit.GetJackpotId(),
			egmAwardHit.GetLevelId(),
			egmAwardHit.GetAwardAmountHit());
	}
	else
	{
		LogString(
			PROGRESSIVE,
			_T("StandardProgressiveWins::ProcessEGMAwardWithNoEGMPaidSignaled - No Handpay pending signaled"));
	}
}

void StandardProgressiveWins::ProcessEGMAwardWithEGMPaidSignaled(shared_ptr<CEGMAward> pendingAward, const CEGMAward &egmAwardHit, std::vector<MsgTargetPair*> *returnMessages)
{
	pendingAward->SetHitReceived(true);
	pendingAward->StoreYourself();
	CEGMAward* egmAward = new CEGMAward(egmAwardHit);
	egmAward->SetAwardState(AwardToGameSuccess);	//this is the equivalent of "Paid"

	MsgTargetPair *newMesgPair = new MsgTargetPair;
	newMesgPair->target = mqProgressive;
	newMesgPair->msg = new CInternalMessage(INT_MSG_EGM_AWARD, egmAward);
	returnMessages->push_back(newMesgPair);

	LogString(
		PROGRESSIVE,
		_T("StandardProgressiveWins::ProcessEGMAwardWithEGMPaidSignaled Sending Paid back to progressives"));

}

void StandardProgressiveWins::SetHandPayPending(__int64 awardAmount, int sasLevelId, std::vector<MsgTargetPair*> *returnMessages)
{
	LogString(
		PROGRESSIVE,
		_T("StandardProgressiveWins::SetHandPayPending SetHandPayPending - levelid:%d - amount:%lld"),
		sasLevelId,
		awardAmount);

	//loop through each award 
	for (auto awardIterator = m_Awards.cbegin(); awardIterator != m_Awards.cend(); awardIterator++)
	{
		if (!(*awardIterator)->isHandpayPendingSignaled() && (*awardIterator)->GetAwardState() != HandpayPending)
		{
			(*awardIterator)->setHandpayPendingSignaled(true);
			(*awardIterator)->SetAwardState(HandpayPending);
			(*awardIterator)->StoreYourself();

			if ((*awardIterator)->isHitReceived())
			{
				CEGMAward* egmAward = new CEGMAward(*(*awardIterator));
				MsgTargetPair *newMesgPair = new MsgTargetPair;
				newMesgPair->target = mqProgressive;
				newMesgPair->msg = new CInternalMessage(INT_MSG_EGM_AWARD, egmAward);
				returnMessages->push_back(newMesgPair);

				LogString(
					PROGRESSIVE,
					_T("StandardProgressiveWins::SetHandPayPending Sending Hand Pay Pending to one link - JackpotID:%lld - levelid:%d - amount:%lld"),
					(*awardIterator)->GetJackpotId(),
					(*awardIterator)->GetLevelId(),
					(*awardIterator)->GetAwardAmountHit());
			}
		}
		else
		{
			LogString(
				PROGRESSIVE,
				_T("StandardProgressiveWins::SetHandPayPending handpay already pending - JackpotID:%lld - levelid:%d - amount:%lld"),
				(*awardIterator)->GetJackpotId(),
				(*awardIterator)->GetLevelId(),
				(*awardIterator)->GetAwardAmountHit());
		}
	}
}

void StandardProgressiveWins::SetHandPayReset(std::vector<MsgTargetPair*> *returnMessages, bool fromGameStart)
{
	std::string fromGameStartString(_T("- fromGameStart:0"));
	if (fromGameStart)
	{
		fromGameStartString = _T(" - fromGameStart:1");
		if (m_lastAwardProcessed.get() && m_lastAwardProcessed->IsActive())
		{
			m_lastAwardProcessed->ClearActive();
			m_lastAwardProcessed->StoreYourself();
		}
	}

	for (auto awardIterator = m_Awards.cbegin(); awardIterator != m_Awards.cend(); awardIterator++)
	{
		if (nullptr != (*awardIterator))
		{
			LogString(
				PROGRESSIVE,
				_T("StandardProgressiveWins::SetHandPayReset Setting paid signaled - JackpotID:%lld - levelid:%d - amount:%lld - fromGameStart:%d"),
				(*awardIterator)->GetJackpotId(),
				(*awardIterator)->GetLevelId(),
				(*awardIterator)->GetAwardAmountHit(),
				(int)fromGameStart);

			(*awardIterator)->setEgmPaidSignaled(true);
			(*awardIterator)->SetAwardState(AwardToGameSuccess);	//Tell one link paid
			(*awardIterator)->StoreYourself();
			(*awardIterator)->LogInfo(_T("StandardProgressiveWins::SetHandPayReset Setting paid signaled") + fromGameStartString);
			if (
				(true == fromGameStart || (*awardIterator)->isHitReceived()) //if this came from a game start OR the hit was received
				&& (*awardIterator)->isHandpayPendingSignaled())			 // AND it is has received a hand pay pending.
			{
				CEGMAward* egmAward = new CEGMAward(*(*awardIterator));

				MsgTargetPair *newMesgPair = new MsgTargetPair;
				newMesgPair->target = mqProgressive;
				newMesgPair->msg = new CInternalMessage(INT_MSG_EGM_AWARD, egmAward);
				returnMessages->push_back(newMesgPair);

				(*awardIterator)->LogInfo(_T("StandardProgressiveWins::SetHandPayReset Sending Paid for") + fromGameStartString);
				LogString(
					PROGRESSIVE,
					_T("StandardProgressiveWins::SetHandPayReset Sending Paid for - JackpotID:%lld - levelid:%d - SAS LevelId:%d - amount:%lld - fromGameStart:%d"),
					egmAward->GetJackpotId(),
					egmAward->GetLevelId(),
					egmAward->GetSASLevelId(),
					egmAward->GetAwardAmountHit(),
					(int)fromGameStart);
			}
			else
			{
				(*awardIterator)->LogInfo(_T("StandardProgressiveWins::SetHandPayReset NOT Sending Paid for") + fromGameStartString);
			}
		}
		else
		{
			LogString(
				PROGRESSIVE,
				_T("StandardProgressiveWins::SetHandPayReset No award data available"));
		}
	}
}

void StandardProgressiveWins::SetProgressivePaidByEGM(shared_ptr<CEGMAward> award, std::vector<MsgTargetPair*> *returnMessages)
{
	if(m_Awards.size() > 0)
	{
		shared_ptr<CEGMAward> foundAward = FindAwardBySasLevelID(award->GetAwardAmountHit(), award->GetSASLevelId());
		if (nullptr != foundAward)
		{
			foundAward->setEgmPaidSignaled(true);
			foundAward->StoreYourself();
			if (foundAward->isHitReceived())
			{
				CEGMAward* egmAward = new CEGMAward(*foundAward);
				egmAward->SetAwardState(AwardToGameSuccess);	//Tell one link paid

				MsgTargetPair *newMesgPair = new MsgTargetPair;
				newMesgPair->target = mqProgressive;
				newMesgPair->msg = new CInternalMessage(INT_MSG_EGM_AWARD, egmAward);
				returnMessages->push_back(newMesgPair);

				LogString(
					PROGRESSIVE,
					_T("StandardProgressiveWins SetProgressivePaidByEGM Sending Paid for - JackpotID:%lld - levelid:%d - SAS LevelId:%d - amount:%lld"),
					egmAward->GetJackpotId(),
					egmAward->GetLevelId(),
					egmAward->GetSASLevelId(),
					egmAward->GetAwardAmountHit());
			}
			else
			{
				LogString(
					PROGRESSIVE,
					_T("StandardProgressiveWins SetProgressivePaidByEGM award has not received the Hit Received back."));
			}
		}
		else
		{
			LogString(
				PROGRESSIVE,
				_T("StandardProgressiveWins SetProgressivePaidByEGM No awards in list"));
		}
	}
	else	//Looks like we got a 0x54(Progressive win(cash out device/credit paid)) before anything else.
	{
		// The following is dead code that I left in. I am not sure what purpose it served but for Accounting only on some EGMs it was causing an 
		// endless loop of doing a LPx84. The code commented out instead of removing so when this is refactored we can determine 
		// what's its purpose was. RJR 10/28/15
		//LogString(
		//		PROGRESSIVE,
		//		_T("StandardProgressiveWins SetProgressivePaidByEGM with no data pending."));

		//MsgTargetPair *newMesgPair = new MsgTargetPair;
		//newMesgPair->target = mqGCF;
		//newMesgPair->msg = new CInternalMessage(INT_MSG_GET_PROGRESSIVE_HOPPER_PAID_INFO);	//tell the SAS stack to do an 84
		//returnMessages->push_back(newMesgPair);
	}
}

void StandardProgressiveWins::SetProgressiveWin(CInternalMessage *msg, std::vector<MsgTargetPair*> *returnMessages, int64_t progressiveLevelId)
{
	//This is from either an 0x83, 0x84 or a 0x85 poll in the SAS stack.
	CEGMAward *award = msg->GetEGMAward();
	if (nullptr != award)
	{
		if (!IsThisADuplicate(award->GetAwardAmountHit(), award->GetSASLevelId()))		//Check the dupe
		{
			msg->SetData((CEGMAward*) nullptr);			// Keep the instance for ourselves.
			shared_ptr<CEGMAward> awardCpy = shared_ptr<CEGMAward>(award);

			LogString(PROGRESSIVE, _T("StandardProgressiveWins::SetProgressiveWin message type %s - amount:%lld - Level:%d"),
				msg->GetMessageTypeStr().c_str(),
				award->GetAwardAmountHit(),
				award->GetSASLevelId());

			if (progressiveLevelId != InvalidProgressiveLevelId)
			{
				awardCpy->SetLevelId((int) progressiveLevelId);	//set the id to the actual level id and not the SAS id when storing for later processing
				if (true == AddProgressiveWin(awardCpy))
				{
					CEGMAward *awardCpyForProgressive = new CEGMAward();
					*awardCpyForProgressive = *awardCpy;	//Do a full copy because this one is going to progressive

					//This returns to process for sending to progressive
					MsgTargetPair *newMesgPair = new MsgTargetPair;
					newMesgPair->target = mqProgressive;
					newMesgPair->msg = new CInternalMessage(INT_MSG_PROGRESSIVE_WIN, awardCpyForProgressive);	//This sends the Hit to One Link
					returnMessages->push_back(newMesgPair);

					LogString(PROGRESSIVE,
						_T("StandardProgressiveWins::SetProgressiveWin Find a level with SAS successful - Sas Level:%d - OneLink Level:%d"),
						awardCpyForProgressive->GetSASLevelId(),
						awardCpyForProgressive->GetLevelId());
				}
			}
			else
			{
				LogString(PROGRESSIVE, _T("StandardProgressiveWins::SetProgressiveWin could not find a level with the Sas Level - %d"),
					award->GetSASLevelId());
			}
		}
		else
		{
			if (award->isEgmPaidSignaled())	//the duplication of an 83 vs an 84 vs an 85 when a hit is advertised via a SAS GPE 
			{
				msg->SetData((CEGMAward*) nullptr);			// Keep the instance for ourselves.
				shared_ptr<CEGMAward> awardCpy = shared_ptr<CEGMAward>(award);

				SetProgressivePaidByEGM(awardCpy, returnMessages);
			}
			else
			{
				LogString(PROGRESSIVE,
					_T("StandardProgressiveWins::SetProgressiveWin - This is a duplicate in the list"));
			}
		}
	}
	else //This should never really be called unless something wrong has happened.
	{
		LogString(PROGRESSIVE, _T("StandardProgressiveWins::SetProgressiveWin message type %s did not send EGMAward"),
			msg->GetMessageTypeStr().c_str());
	}
}


void StandardProgressiveWins::Pulse(DWORD tickCount, bool progressiveDown, std::vector<MsgTargetPair*> *returnMessages)
{
	if (false == progressiveDown && m_Awards.size() > 0) //Don't do anything unless the progressive is up and we have stored awards.
	{
		if (tickCount - GetRetryTickCountStart() >= QUEUE_RETRY_EXPIRED || GetRetryTickCountStart() == 0)
		{
			SetRetryTickCountStart(tickCount);
			BuildAwardResendList(returnMessages);
		}
	}
}

void StandardProgressiveWins::BuildAwardResendList(std::vector<MsgTargetPair*> * returnMessages)
{
	for (auto awardIterator = m_Awards.cbegin(); awardIterator != m_Awards.cend();)
	{
		if (nullptr != (*awardIterator))
		{
			if (true == (*awardIterator)->isHitReceived()) //if we have received the hit from the system.
			{
				LogString(
					PROGRESSIVE,
					_T("StandardProgressiveWins::BuildAwardResendList resending - JackpotID:%lld - levelid:%d - amount:%lld"),
					(*awardIterator)->GetJackpotId(),
					(*awardIterator)->GetLevelId(),
					(*awardIterator)->GetAwardAmountHit());

				CEGMAward* egmAward = new CEGMAward(*(*awardIterator)); //copy the award and send it
				MsgTargetPair *newMesgPair = new MsgTargetPair;
				newMesgPair->target = mqProgressive;
				newMesgPair->msg = new CInternalMessage(INT_MSG_EGM_AWARD, egmAward);
				returnMessages->push_back(newMesgPair);
				awardIterator++;
			}
			else // If we have not received the hit from the system
			{
				__int64 elapsedMilliseconds = CUtilities::GetMillisecondsSinceTimestamp(
					(*awardIterator)->GetReceivedFromEGMTime());
					
				if (elapsedMilliseconds >= QUEUE_HIT_PUNT_EXPIRED && ((*awardIterator)->GetAwardState() == SentToSystem)) //This is to Punt a Jackpot hit that never was returned in a long time.
				{
					LogString(
						PROGRESSIVE,
						_T("StandardProgressiveWins::BuildAwardResendList Sending a hit that has elapsed 10 minutes with level 0 value 0 - JackpotID:%lld - levelid:%d - amount:%lld"),
						(*awardIterator)->GetJackpotId(),
						(*awardIterator)->GetLevelId(),
						(*awardIterator)->GetAwardAmountHit());

					CEGMAward* egmAward = new CEGMAward(*(*awardIterator)); //copy the award and send it
					egmAward->SetLevelId(0);
					egmAward->SetAwardAmountHit(0);
					MsgTargetPair *newMesgPair = new MsgTargetPair;
					newMesgPair->target = mqProgressive;
					newMesgPair->msg = new CInternalMessage(INT_MSG_EGM_AWARD, egmAward);
					returnMessages->push_back(newMesgPair);

					awardIterator = RemoveAward(awardIterator);
				}
				else if (elapsedMilliseconds >= QUEUE_RETRY_EXPIRED && ((*awardIterator)->GetAwardState() == ReceivedFromGame)) //This is to retry a HIT that never made it.
				{
					//This is for a Hit that never made it to the system. We will retry it until we get the ack.
					CEGMAward *awardCpyForProgressive = new CEGMAward(*(*awardIterator)); //Do a full copy
					//This returns to process for sending to progressive
					MsgTargetPair *newMesgPair = new MsgTargetPair;
					newMesgPair->target = mqProgressive;
					newMesgPair->msg = new CInternalMessage(INT_MSG_PROGRESSIVE_WIN, awardCpyForProgressive);	//This sends the Hit to One Link
					returnMessages->push_back(newMesgPair);

					awardIterator++;
				}
				else
				{
					awardIterator++;
				}
			}
		}
		else
		{
			awardIterator++;
		}
	}
}

DWORD StandardProgressiveWins::GetRetryTickCountStart()
{
	return m_queueRetryTickCountStart;
}

void StandardProgressiveWins::SetRetryTickCountStart(DWORD retryCountStart)
{
	m_queueRetryTickCountStart = retryCountStart;
}

bool StandardProgressiveWins::IsThisADuplicate(__int64 awardAmount, int sasLevelId)
{
	if (nullptr != m_lastAwardProcessed &&
		m_lastAwardProcessed->IsActive() &&
		m_lastAwardProcessed->GetAwardAmountHit() == awardAmount &&
		m_lastAwardProcessed->GetSASLevelId() == sasLevelId)
	{
		return true;
	}

	auto it = find_if(m_Awards.cbegin(), m_Awards.cend(), [&](const shared_ptr<CEGMAward>& award) -> bool
	{
		if (award->GetAwardAmountHit() == awardAmount && award->GetSASLevelId() == sasLevelId)
		{
			return true;
		}
		return false;
	});

	if (it != m_Awards.cend())
	{
		return true;
	}
	return false;
}

void StandardProgressiveWins::RemoveAward(__int64 awardAmount, int levelId)
{
	auto it = find_if(m_Awards.cbegin(), m_Awards.cend(), [&](const shared_ptr<CEGMAward>& award) -> bool
	{
		if (award->GetAwardAmountHit() == awardAmount && award->GetLevelId() == levelId)
		{
			return true;
		}
		return false;
	});

	RemoveAward(it);
}

std::vector<shared_ptr<CEGMAward>>::const_iterator StandardProgressiveWins::RemoveAward(
	std::vector<shared_ptr<CEGMAward>>::const_iterator it)
{
	auto returnIterator(it);

	if (it != m_Awards.cend())
	{
		RecoverNvramActiveOffset(*it);
		m_lastAwardProcessed = *it;

		m_lastAwardProcessed->SetMemoryStorageParameters(MEMORY_NVRAM,
			NVRAM_PREV_AWARD_DATA_OFFSET,
			NVRAM_PREV_AWARD_DATA_SIZE,
			NVRAM_PREV_AWARD_ACTIVE_OFFSET,
			NVRAM_PREV_AWARD_ACTIVE_VALUE);
		m_lastAwardProcessed->StoreYourself();
		m_lastAwardProcessed->LogInfo(_T("StandardProgressiveWins::RemoveAward -- m_lastAwardProcessed"));

		returnIterator = m_Awards.erase(it);
	}

	return returnIterator;
}

shared_ptr<CEGMAward> StandardProgressiveWins::FindAward(__int64 awardAmount, int levelId) 
{
	auto it = find_if(m_Awards.cbegin(), m_Awards.cend(), [&](const shared_ptr<CEGMAward>& award) -> bool
	{
		if (award->GetAwardAmountHit() == awardAmount && award->GetLevelId() == levelId)
		{
			return true;
		}
		return false;
	});

	if (it != m_Awards.cend())
	{
		return *it;
	}
	return nullptr;
}

shared_ptr<CEGMAward> StandardProgressiveWins::FindAwardBySasLevelID(__int64 awardAmount, int sasLevelId)
{
	auto it = find_if(m_Awards.cbegin(), m_Awards.cend(), [&](const shared_ptr<CEGMAward>& award) -> bool
	{
		if (award->GetAwardAmountHit() == awardAmount && award->GetSASLevelId() == sasLevelId)
		{
			return true;
		}
		return false;
	});

	if (it != m_Awards.cend())
	{
		return *it;
	}
	return nullptr;
}
