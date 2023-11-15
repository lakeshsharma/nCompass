#pragma once
#include "stdafx.h"
#include "InternalMsgContent.h"
#include "ProgressiveKeys.h"

// Declare friend class(es) for unit test access.
namespace SentinelUnitTests { class PromoGameObjectsTests; }

class CPromoGameRequest : public CInternalMsgContent<INT_MSG_CONTENT_PROMOGAME_REQUEST>
{
public:
	/// <summary>
	/// CPromoGameRequest json deserialize constructor.
	/// </summary>
	/// <param name="jsonDoc">The json document to use for deserialization.</param>
	CPromoGameRequest(const rapidjson::Document& jsonDoc);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CPromoGameRequest"/> class.
	/// </summary>
	virtual ~CPromoGameRequest();

	/// <summary>
	/// Gets the level id
	/// </summary>
	/// <returns>level id</returns>
	int64_t GetLevelId() const;

	/// <summary>
	/// Gets the minor award id
	/// </summary>
	/// <returns>minor award id</returns>
	std::string GetMinorAwardId() const;

	/// <summary>
	/// Gets the jackpot id
	/// </summary>
	/// <returns>jackpot id</returns>
	int64_t GetJackpotId() const;

	/// <summary>
	/// Gets the amount
	/// </summary>
	/// <returns>amount</returns>
	int64_t GetAmount() const;

	/// <summary>
	/// Gets the paytable item type
	/// </summary>
	/// <returns>paytable item type</returns>
	std::string GetPaytableItemType() const;

	/// <summary>
	/// Gets the paytable item value
	/// </summary>
	/// <returns>paytable item value</returns>
	int64_t GetPaytableItemValue() const;

	/// <summary>
	/// Gets the jackpot trigger type
	/// </summary>
	/// <returns>jackpot trigger type</returns>
	JackpotTrigger::Type GetJackpotTriggerType() const;

	/// <summary>
	/// Checks to see if Paytable Item tag exists
	/// </summary>
	/// <returns>true if paytable item tag exists, false otherwise</returns>
	bool PaytableItemTagExists() const;

private:
	int64_t m_levelId;
	std::string m_minorAwardId;
	int64_t m_jackpotId;
	int64_t m_amount;
	int m_paytableItemTag;
	std::string m_paytableItemType;
	int64_t m_paytableItemValue;
	JackpotTrigger::Type m_jackpotTriggerType;

	// Grant "friend" access for testing.
	friend class SentinelUnitTests::PromoGameObjectsTests;
	FRIEND_TEST(PromoGameObjectsTests, PromoGameRequestTest);
	FRIEND_TEST(PromoGameObjectsTests, PromoGameAcceptanceStatusTest);
	FRIEND_TEST(PromoGameObjectsTests, PromoGameRequestPaytableItemTagTest);
};
