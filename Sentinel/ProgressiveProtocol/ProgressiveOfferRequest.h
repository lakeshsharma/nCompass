#pragma once
#include "stdafx.h"
#include "InternalMsgContent.h"
#include "ProgressiveKeys.h"

// Declare friend class(es) for unit test access.
namespace SentinelUnitTests { class PromoGameObjectsTests; }

class CProgressiveOfferRequest : public CInternalMsgContent<INT_MSG_CONTENT_PROMOGAME_REQUEST>
{
public:
	/// <summary>
	/// CProgressiveOfferRequest json deserialize constructor.
	/// </summary>
	/// <param name="jsonDoc">The json document to use for deserialization.</param>
	CProgressiveOfferRequest(const rapidjson::Document& jsonDoc);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CProgressiveOfferRequest"/> class.
	/// </summary>
	virtual ~CProgressiveOfferRequest();

	/// <summary>
	/// Gets the jackpot id
	/// </summary>
	/// <returns>jackpot id</returns>
	int64_t GetJackpotId() const;

	/// <summary>
	/// Gets the player id
	/// </summary>
	/// <returns>player id</returns>
	int64_t GetPlayerId() const;

	/// <summary>
	/// Gets the offer id
	/// </summary>
	/// <returns>offer id</returns>
	std::string GetOfferId() const;

	/// <summary>
	/// Gets the Transaction Id
	/// </summary>
	/// <returns>Transaction Id</returns>
	std::string GetTransactionId() const;

private:
	int64_t m_jackpotId;
	int64_t m_playerId;
	std::string m_offerId;
	std::string m_transactionId;
};
