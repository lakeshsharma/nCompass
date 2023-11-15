#include "stdafx.h"
#include "DataSubscriptionManager.h"
#include "Logging/LogString.h"
#include "RapidJsonHelper.h"

using namespace std;

CDataSubscriptionManager::CDataSubscriptionManager()
{
}

CDataSubscriptionManager::~CDataSubscriptionManager()
{
}

void CDataSubscriptionManager::RegisterProvider(CDataSubscriptionProvider* dataSubscriptionProvider)
{
	std::list<CDataSubscriptionProvider*>::iterator findIter = std::find(m_subscriptionProviders.begin(), m_subscriptionProviders.end(), dataSubscriptionProvider);

	if (findIter == m_subscriptionProviders.end())
	{
		m_subscriptionProviders.push_back(dataSubscriptionProvider);
	}
}

void CDataSubscriptionManager::UnregisterProvider(CDataSubscriptionProvider* dataSubscriptionProvider)
{
	std::list<CDataSubscriptionProvider*>::iterator findIter = std::find(m_subscriptionProviders.begin(), m_subscriptionProviders.end(), dataSubscriptionProvider);

	if (findIter != m_subscriptionProviders.end())
	{
		m_subscriptionProviders.erase(findIter);
	}
}

void CDataSubscriptionManager::Subscribe(CSubscription* subscription)
{
	if (subscription != nullptr)
	{
		for (CDataSubscriptionProvider* dataSubscriptionProvider : m_subscriptionProviders)
		{
			dataSubscriptionProvider->Unsubscribe(*subscription);
			dataSubscriptionProvider->Subscribe(*subscription);
		}
	}
}

void CDataSubscriptionManager::Unsubscribe(CSubscription* subscription)
{
	if (subscription)
	{
		for (auto subcriptionProvider : m_subscriptionProviders)
		{
			subcriptionProvider->Unsubscribe(*subscription);
		}
	}
}

void CDataSubscriptionManager::GetSubscriptionData(std::vector<CSubscription>& subscriptionData)
{
	for (CDataSubscriptionProvider* dataSubscriptionProvider : m_subscriptionProviders)
	{
		dataSubscriptionProvider->AppendToSubscriptionData(subscriptionData);
	}
}
