#pragma once

#include "gmock/gmock.h"
#include "ProgressiveProtocol/IStandardProgressiveWins.h"
#include "MsgTargetPair.h"

using namespace std;

class ProgressiveWinsMock : public IStandardProgressiveWins
{
private:
public:
	MOCK_METHOD1(BuildAwardResendList, void(std::vector<MsgTargetPair*> *returnMessages));
	MOCK_METHOD1(SetRetryTickCountStart, void(DWORD tickCount));
	MOCK_METHOD0(HasUnsentProgressiveWins, bool());
	MOCK_METHOD1(AddProgressiveWin, bool(std::shared_ptr<CEGMAward>award));
	MOCK_METHOD2(MarkState, bool(std::shared_ptr<CEGMAward>award, EGMAwardState state));
	MOCK_METHOD5(ProcessEGMAwardStandardFromProgressive, void(DWORD tickCount,
		CGames& games,
		bool progressiveDown,
		const CEGMAward& egmAwardHit,
		std::vector<MsgTargetPair*> *returnMessages));
	MOCK_METHOD3(SetHandPayPending, void(__int64 awardAmount, int sasLevelId, std::vector<MsgTargetPair*> *returnMessages));
	MOCK_METHOD2(IsThisADuplicate, bool(__int64 awardAmount, int sasLevelId));
	MOCK_METHOD2(SetHandPayReset, void(std::vector<MsgTargetPair*> *returnMessages, bool fromGameStart));
	MOCK_METHOD2(SetProgressivePaidByEGM, void(std::shared_ptr<CEGMAward> award, std::vector<MsgTargetPair*> *returnMessages));
	MOCK_METHOD3(SetProgressiveWin, void(CInternalMessage *msg, std::vector<MsgTargetPair*> *returnMessages, int64_t levelId));
	MOCK_METHOD3(Pulse, void(DWORD tickCount, bool progressiveDown, std::vector<MsgTargetPair*> *returnMessages));
	MOCK_METHOD1(ProgressivePaidSentReceived, void(std::shared_ptr<CEGMAward> egmAwardHit));
	MOCK_METHOD1(GetLastStandardWin, void(std::shared_ptr<CEGMAward> &lastStandardWin));
	MOCK_METHOD1(GetStandardWins, void(std::vector<std::shared_ptr<CEGMAward>> &standardWins));
};

