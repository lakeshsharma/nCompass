#pragma once

#include "gmock/gmock.h"
#include "ProgressiveProtocol/ProgressiveConnection.h"
#include "ProgressiveProtocol/ProgressiveDataConsumer.h"
#include "ProgressiveProtocol/ProgressiveLevelsInfo.h"
#include "ProgressiveProtocol/ProgressiveLevelBroadcast.h"
#include "ProgressiveProtocol/ProgressiveWinnerPick.h"
#include "ProgressiveProtocol/ProgressiveWinnerPicked.h"

using namespace std;

class CProgressiveDataConsumerMock : public CProgressiveDataConsumer
{
public:
	MOCK_METHOD2(SendMsgToInternals, void(CInternalMessage*, int));
	MOCK_METHOD2(QueueToOneLink, void(ProgressiveQueueToOneLink*, int));

	MOCK_METHOD0(GetProgressiveGameInfo, void());
	MOCK_METHOD1(ConnectionUpdate, void(const CProgressiveConnectionInfo &connectionInfo));
	MOCK_METHOD2(ProgressiveHitSentStatusUpdate, void(const CEGMAward &award, bool wasSent));
	MOCK_METHOD2(SendJackpotPaidRequestStatusUpdate, void(const CEGMAward &award, bool wasSent));
	MOCK_METHOD1(HostUpdate, void(const CProgressiveHost* host));
	MOCK_METHOD1(JackpotHit, void(CEGMAward *egmAward));
	MOCK_METHOD1(SendProgressiveLevelsInfo, void(ProgressiveLevelsInfo* pProgressiveLevelInfo));
	MOCK_METHOD3(ProgressiveBonusingAvailable, void(CProgressiveBonusingAvailable *progressiveBonusingAvailable,
		CProgressiveEligibleLevelsInfo *progressiveEligibleLevelsInfo,
		ProgressivePatronSessionInfo *progressivePatronSessionInfo));
	MOCK_METHOD1(ClaimableAward, void(CProgressiveClaimableAward *pProgressiveClaimableAward));
	MOCK_METHOD1(ProgressiveLevelBroadcast, void(CProgressiveLevelBroadcast* pLevelBroadcast));
	MOCK_METHOD1(ProgressivePlaylist, void(CProgressivePlaylist* pPlaylist));
	MOCK_METHOD1(ProgressiveWinnerPick, void(CProgressiveWinnerPick* pWinnerPick));
	MOCK_METHOD1(ProgressiveWinnerPicked, void(CProgressiveWinnerPicked* pWinnerPicked));
};
