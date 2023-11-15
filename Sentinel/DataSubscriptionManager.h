#pragma once

#include "DataSubscriptionProvider.h"

class CDataSubscriptionManager
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CDataSubscriptionManager"/> class.
	/// </summary>
	CDataSubscriptionManager();

	/// <summary>
	/// Finalizes an instance of the <see cref="CDataSubscriptionManager"/> class.
	/// </summary>
	~CDataSubscriptionManager();

	/// <summary>
	/// Registers the provider.
	/// </summary>
	/// <param name="dataSubscriptionProvider">The data subscription provider.</param>
	void RegisterProvider(CDataSubscriptionProvider* dataSubscriptionProvider);

	/// <summary>
	/// Unregisters the provider.
	/// </summary>
	/// <param name="dataSubscriptionProvider">The data subscription provider.</param>
	void UnregisterProvider(CDataSubscriptionProvider* dataSubscriptionProvider);

	/// <summary>
	/// Subscribes the specified subscription.
	/// </summary>
	/// <param name="subscription">The subscription.</param>
	void Subscribe(CSubscription* subscription);

	/// <summary>
	/// Unsubscribes the specified subscriptions.
	/// </summary>
	/// <param name="subscription">The subscriptions to unsubscribe.</param>
	void Unsubscribe(CSubscription* subscriptions);

	/// <summary>
	/// Gets the subscription data.
	/// </summary>
	/// <param name="subscriptionData">The subscriptions.</param>
	void GetSubscriptionData(std::vector<CSubscription>& subscriptionData);

private:
	std::list<CDataSubscriptionProvider*> m_subscriptionProviders;
};


