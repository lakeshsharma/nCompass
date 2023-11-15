#pragma once

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"
#include "ProgressiveEligibleLevelInfo.h"
#include "ProgressivePatronSessionInfo.h"

class ProgressiveLevelManager;

namespace SentinelNativeUnitTests
{
	class ProgressiveBonusingAvailableTests;
	class ProgressiveLevelManagerTests;
}

class CProgressiveBonusingAvailable : public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_BONUSING_AVAILABLE>
{
public:
	/// <summary>
	/// CProgressiveBonusingAvailable Constructor.
	/// </summary>
	/// <param name="sessionId">The session identifier.</param>
	///	<param name="jsonDoc">The JSON document containing the BonusingAvailable message.</param>
	CProgressiveBonusingAvailable(int sessionId, const rapidjson::Document& jsonDoc);

	/// <summary>
	/// Gets the session identifier.
	/// </summary>
	/// <returns>The session identifier.</returns>
	int GetSessionId() const;

	/// <summary>
	/// Determines whether eligibility could be determined for this session
	/// </summary>
	/// <returns>true if eligibility could be determined for this session</returns>
	bool IsEligibilityDetermined() const;

	/// <summary>
	/// Gets the time the bonusing available was received.
	/// </summary>
	/// <returns>the time the bonusing available was received</returns>
	SYSTEMTIME GetReceivedTime() const;

private:
	CProgressiveBonusingAvailable(const CProgressiveBonusingAvailable& other) = delete;
	void operator=(const CProgressiveBonusingAvailable&) = delete;
	std::string m_jsonString;
	SYSTEMTIME m_receivedTime;
	DWORD m_receivedTickCount;
	int m_sessionId;
	bool m_eligibilityDetermined;

	friend SentinelNativeUnitTests::ProgressiveBonusingAvailableTests;
};
