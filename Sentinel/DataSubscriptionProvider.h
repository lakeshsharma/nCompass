#pragma once

#include "Subscription.h"

class CDataSubscriptionProvider
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CDataSubscriptionProvider"/> class.
	/// </summary>
	CDataSubscriptionProvider(const std::string& topic);

	/// <summary>
	/// Finalizes an instance of the <see cref="CDataSubscriptionProvider"/> class.
	/// </summary>
	~CDataSubscriptionProvider();

	/// <summary>
    /// Default constructor <see cref="CDataSubscriptionProvider"/> 
    /// class. 
	/// </summary>
    CDataSubscriptionProvider();

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId) = 0;

	/// <summary>
	/// Subscribes to a provider.
	/// </summary>
	/// <param name="subscription">The subscription.</param>
	/// <returns>true if subscription is added to the list, false otherwise.</returns>
	virtual bool Subscribe(const CSubscription& subscription);

	/// <summary>
	/// Sets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	virtual void SetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

	/// <summary>
	/// Adds the subscription if it doesn't exist yet.
	/// </summary>
	/// <param name="subscription">The subscription.</param>
	void AddSubscriptionIfNeeded(const CSubscription& subscription);

	/// <summary>
	/// Gets the subscription data.
	/// </summary>
	/// <param name="subscriptions">The subscription updates.</param>
	void AppendToSubscriptionData(std::vector<CSubscription>& subscriptions);

	/// <summary>
	/// Unsubscribes the specified subscription.
	/// </summary>
	/// <param name="subscription">The subscription to unsubscribe.</param>
	void Unsubscribe(const CSubscription& subscription);

protected:
	std::vector<CSubscription> m_subscriptions;
	std::vector<std::string> m_topics;

	void AddTopic(const std::string& topic);
	bool IsTopicSupported(const std::string& topic) const;
	void SetSubscriptionData(const std::string& topic, const std::string& whereId, const std::string& data);
};

