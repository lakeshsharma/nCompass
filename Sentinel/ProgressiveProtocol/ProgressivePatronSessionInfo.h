#pragma once

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"

namespace SentinelNativeUnitTests { class ProgressivePatronInfoTests; }

class ProgressivePatronSessionInfo : public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_PATRON_SESSION_INFO>
{

private:
	int m_sessionId;
	OptionalVal<int64_t> m_playerId;
	OptionalVal<int> m_sessionPoints;
	std::string	m_name;
	std::string m_cardNumber;
	std::string	m_tier;

public:
	/// <summary>
	/// ProgressivePatronSessionInfo Constructor.
	/// </summary>
	/// <param name="sessionId">The session identifier.</param>
	///	<param name="jsonDoc">The JSON document containing the BonusingAvailable message.</param>
	ProgressivePatronSessionInfo(int sessionId, const rapidjson::Document& jsonDoc);

	/// <summary>
	/// Returns the session identifier.
	/// </summary>
	/// <returns>The session identifier.</returns>
	int GetSessionId() const;

	///	<summary>
	///	Returns the patron's player ID.
	///	</summary>
	///	<param name="isValid">A reference which will be set to <b>true</b> if the
	///		returned value is valid.</param>
	///	<returns>The patron's player ID.</returns>
	///	<remarks>
	/// The "PlayerId" JSON element is optional.
	///	</remarks>
	int64_t GetPlayerId(bool& isValid) const;

	///	<summary>
	///	Returns the patron's session points.
	///	</summary>
	///	<param name="isValid">A reference which will be set to <b>true</b> if the
	///		returned value is valid.</param>
	///	<returns>The patron's session points.</returns>
	///	<remarks>
	/// The "SessionPts" JSON element is optional.
	///	</remarks>
	int GetSessionPoints(bool& isValid) const;

	///	<summary>
	///	Returns the patron's name.
	///	</summary>
	///	<returns>The patron's name.</returns>
	///	<remarks>
	/// The "Name" JSON element is optional.
	///	</remarks>
	LPCTSTR	GetName() const;

	///	<summary>
	///	Returns the patron's card number.
	///	</summary>
	///	<returns>The patron's card number.</returns>
	///	<remarks>
	/// The "CardNumber" JSON element is optional.
	///	</remarks>
	LPCTSTR GetCardNumber() const;

	///	<summary>
	///	Returns the patron's tier.
	///	</summary>
	///	<returns>The patron's tier.</returns>
	///	<remarks>
	/// The "Tier" JSON element is optional.
	///	</remarks>
	LPCTSTR	GetTier() const;

private:
	ProgressivePatronSessionInfo(const ProgressivePatronSessionInfo&) = delete;
	void operator=(const ProgressivePatronSessionInfo&) = delete;

};
