#pragma once

#include "InternalMsgContent.h"
#include "OptionalVal.h"
#include "ProgressiveKeys.h"

namespace SentinelNativeUnitTests { class ProgressiveWinnerPickedTests; }

class CProgressiveWinnerPicked : public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_WINNER_PICKED>
{
public:

	static const MethodOfPayment::Type DEFAULT_METHOD_OF_PAYMENT = MethodOfPayment::Invalid;
	static const AwardType::Type DEFAULT_AWARD_TYPE = AwardType::Invalid;

public:
	/// <summary>
	/// Constructor.
	/// </summary>
	///	<param name="receivedTickCount">The received tick count.</param>
	///	<param name="jsonDoc">The json document.</param>
	CProgressiveWinnerPicked(DWORD receivedTickCount, const rapidjson::Document& jsonDoc);

	/// <summary>
	/// Initializes a new instance of the <see cref="CProgressiveWinnerPicked"/> class.
	/// </summary>
	/// <param name="other">The other.</param>
	CProgressiveWinnerPicked(const CProgressiveWinnerPicked& other);

	/// <summary>
	/// Operator=s the specified other.
	/// </summary>
	/// <param name="other">The other.</param>
	void operator=(const CProgressiveWinnerPicked& other);

	/// <summary>
	/// Methods the name of the of payment.
	/// </summary>
	/// <returns>Name of method of payment</returns>
	std::string MethodOfPaymentName();

	/// <summary>
	/// Awards the name of the type.
	/// </summary>
	/// <returns>Name of award type</returns>
	std::string AwardTypeName();

	/// <summary>
	/// Gets the received tick count.
	/// </summary>
	/// <returns>received tick count</returns>
	DWORD GetReceivedTickCount() const;

	/// <summary>
	/// Gets the level identifier.
	/// </summary>
	/// <returns>level id</returns>
	int GetLevelId() const;

	/// <summary>
	/// Gets the level name.
	/// </summary>
	/// <returns>level name</returns>
	std::string GetLevelName() const;

	/// <summary>
	/// Gets the win amount.
	/// </summary>
	/// <returns>win amount</returns>
	std::string	GetWinAmount() const;

	/// <summary>
	/// Gets the win amount.
	/// </summary>
	/// <returns>win amount</returns>
	int64_t GetWinAmountInt64() const;

	/// <summary>
	/// Gets the winner venue.
	/// </summary>
	/// <returns>winner venue</returns>
	std::string GetWinnerVenue() const;

	/// <summary>
	/// Gets the winner location.
	/// </summary>
	/// <returns>winner location</returns>
	std::string GetWinnerLocation() const;

	/// <summary>
	/// Gets the winner asset number.
	/// </summary>
	/// <returns>winner asset number</returns>
	std::string GetWinnerAssetNumber() const;

	/// <summary>
	/// Gets the winning hand.
	/// </summary>
	/// <returns>winning hand</returns>
	std::string GetWinningHand() const;

	/// <summary>
	/// Gets the winner device identifier.
	/// </summary>
	/// <returns>winner device id</returns>
	int64_t GetWinnerDeviceId() const;

	/// <summary>
	/// Gets the jackpot identifier.
	/// </summary>
	/// <returns>jackpot identifier</returns>
	int64_t GetJackpotId() const;

	/// <summary>
	/// Gets the media minimum seconds.
	/// </summary>
	/// <returns>media minimum seconds</returns>
	int GetMediaMinSeconds() const;

	/// <summary>
	/// Sets the media minimum seconds.
	/// </summary>
	/// <param name="mediaMinSeconds">The media minimum seconds.</param>
	void SetMediaMinSeconds(int mediaMinSeconds);

	/// <summary>
	/// Gets the media maximum seconds.
	/// </summary>
	/// <returns>media maximum seconds</returns>
	int GetMediaMaxSeconds() const;

	/// <summary>
	/// Sets the media maximum seconds.
	/// </summary>
	/// <param name="mediaMaxSeconds">The media maximum seconds.</param>
	void SetMediaMaxSeconds(int mediaMaxSeconds);

	/// <summary>
	/// Gets the type of the jackpot trigger.
	/// </summary>
	/// <returns>type of the jackpot trigger</returns>
	JackpotTrigger::Type GetJackpotTriggerType() const;

	/// <summary>
	/// Determines whether this instance is pte.
	/// </summary>
	/// <returns>tur if pte false otherwise</returns>
	bool IsPte() const;

	/// <summary>
	/// Gets the method of payment.
	/// </summary>
	/// <returns>method of payment</returns>
	MethodOfPayment::Type GetMethodOfPayment() const;

	/// <summary>
	/// Gets the type of the award.
	/// </summary>
	/// <returns> type of award</returns>
	AwardType::Type GetAwardType() const;

	/// <summary>
	/// Gets the minor award id.
	/// </summary>
	/// <returns>minor award id</returns>
	std::string GetMinorAwardId() const;

	/// <summary>
	/// Gets external award name.
	/// </summary>
	/// <returns>external award name</returns>
	std::string GetExternalAwardName() const;

	/// <summary>
	/// Gets external award value.
	/// </summary>
	/// <returns>external award value</returns>
	std::string GetExternalAwardValue() const;

	/// <summary>
	/// Gets external award currency.
	/// </summary>
	/// <returns>external award currency</returns>
	std::string GetExternalAwardCurrency() const;

	/// <summary>
	/// Gets external award ID
	/// </summary>
	/// <returns>external award ID</returns>
	std::string GetExternalAwardId() const;

	/// <summary>
	/// Checks to see if winner paid message needs to be displayed to the player.
	/// </summary>
	/// <returns>true if winner paid needs to be displayed, false otherwise.</returns>
	bool ForwardToUI() const;

private:

	friend SentinelNativeUnitTests::ProgressiveWinnerPickedTests;

	DWORD	m_receivedTickCount;
	int		m_levelId;
	std::string m_levelName;
	std::string	m_winAmount;
	std::string m_winnerVenue;
	std::string m_winnerLocation;
	std::string m_winnerAssetNumber;
	std::string m_winningHand;
	int64_t m_winnerDeviceId;
	int64_t m_jackpotId;
	int		m_mediaMinSeconds;
	int		m_mediaMaxSeconds;
    OptionalVal<int> m_sessionId;
	JackpotTrigger::Type m_jackpotTriggerType;
	bool	m_pte;
	MethodOfPayment::Type m_methodOfPayment;
	AwardType::Type m_awardType;
	std::string m_minorAwardId;
	std::string m_externalAwardName;
	std::string m_externalAwardValue;
	std::string m_externalAwardCurrency;
	std::string m_externalAwardId;
};
