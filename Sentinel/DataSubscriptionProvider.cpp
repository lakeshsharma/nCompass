#include "stdafx.h"
#include "DataSubscriptionProvider.h"
#include "Logging/LogString.h"
#include "Utilities.h"

using namespace std;

CDataSubscriptionProvider::CDataSubscriptionProvider(const string& topic)
{
	if (!topic.empty())
	{
		AddTopic(topic);
	}	
}

CDataSubscriptionProvider::CDataSubscriptionProvider()
{
}

CDataSubscriptionProvider::~CDataSubscriptionProvider()
{
}

void CDataSubscriptionProvider::AddTopic(const std::string& topic)
{
	auto topicIter = std::find(m_topics.begin(), m_topics.end(), topic);
	if (topicIter == m_topics.end())
	{
		m_topics.push_back(topic);
	}
}

bool CDataSubscriptionProvider::IsTopicSupported(const std::string& topic) const
{
	return m_topics.end() != std::find(m_topics.begin(), m_topics.end(), topic);
}

bool CDataSubscriptionProvider::Subscribe(const CSubscription& subscription)
{
	bool added(false);
	if (IsTopicSupported(subscription.GetTopic()))
	{
		AddSubscriptionIfNeeded(subscription);
		added = true;
	}

	return added;
}

void CDataSubscriptionProvider::SetSubscriptionDataResponse(const string& topic, const string& whereId)
{
	string subscriptionDataResponse(GetSubscriptionDataResponse(topic, whereId));

	if (!subscriptionDataResponse.empty())
	{
		SetSubscriptionData(topic, whereId, subscriptionDataResponse);
	}
}

void CDataSubscriptionProvider::AddSubscriptionIfNeeded(const CSubscription& subscription)
{
	auto subscriptionIter = std::find(m_subscriptions.begin(), m_subscriptions.end(), subscription);
	if (subscriptionIter == m_subscriptions.end())
	{
		LogString(SUBSCRIPTIONS_LOG, "Subscription is added id=%s whereid=%s jsonrpcversion=%s topic=%s",
			subscription.GetId().c_str(),
			subscription.GetWhereId().c_str(),
			subscription.GetJsonRpcVersion().c_str(),
			subscription.GetTopic().c_str());
		m_subscriptions.push_back(subscription);
	}
	else
	{
		auto itr = find_if(m_subscriptions.begin(), m_subscriptions.end(), [&](const CSubscription& s) {
			return s.Comparator(subscription);
		});
		if (itr != m_subscriptions.end())
		{
			itr->ClearData();
		}
	}
}

void CDataSubscriptionProvider::AppendToSubscriptionData(std::vector<CSubscription>& subscriptionData)
{
	for (CSubscription& subscription : m_subscriptions)
	{
		SetSubscriptionDataResponse(subscription.GetTopic(), subscription.GetWhereId());
		if (subscription.NeedToSendUpdate())
		{
			subscriptionData.push_back(subscription);
			subscription.UpdateIsSent();
		}
	}
}

void CDataSubscriptionProvider::Unsubscribe(const CSubscription& subscription)
{
	auto subscriptionIter = std::find(m_subscriptions.begin(), m_subscriptions.end(), subscription);
	if (subscriptionIter != m_subscriptions.end())
	{
		LogString(SUBSCRIPTIONS_LOG, "Unsubscribe id=%s whereid=%s jsonrpcversion=%s topic=%s",
			subscription.GetId().c_str(),
			subscription.GetWhereId().c_str(),
			subscription.GetJsonRpcVersion().c_str(),
			subscription.GetTopic().c_str());
		m_subscriptions.erase(subscriptionIter);
	}
}

void CDataSubscriptionProvider::SetSubscriptionData(const string& topic, const string& whereId, const string& data)
{
	for (CSubscription& subscription : m_subscriptions)
	{
		subscription.SetSubscriptionData(topic, whereId, data);
	}
}
