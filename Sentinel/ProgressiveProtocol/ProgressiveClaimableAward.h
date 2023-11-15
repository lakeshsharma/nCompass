#pragma once

#include "InternalMsgContent.h"
#include "OptionalVal.h"

namespace SentinelNativeUnitTests { class ProgressiveClaimableAwardTests; }

class CProgressiveClaimableAward : public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_CLAIMABLE_AWARD>
{
public:
	int m_levelId;
	std::string m_minorAwardId;
	DWORD m_receivedTickCount;
	std::string	m_amount;
	OptionalVal<int> m_sessionId;
	std::string m_acceptanceUrl;
	int64_t m_expirationMilliseconds;
	bool m_isSamePlayer;

public:
	/// <summary>
	/// Constructor.
	/// </summary>
	///	<param name="receivedTickCount">The received tick count.</param>
	///	<param name="jsonDoc">The json document.</param>
	CProgressiveClaimableAward(DWORD receivedTickCount, const rapidjson::Document& jsonDoc);

private:
	CProgressiveClaimableAward(const CProgressiveClaimableAward& other);
	void operator=(const CProgressiveClaimableAward& other);

	friend SentinelNativeUnitTests::ProgressiveClaimableAwardTests;
};
