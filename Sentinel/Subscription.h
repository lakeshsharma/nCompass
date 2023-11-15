#pragma once
#include "InternalMsgContent.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class CDMManager; }

class CSubscription : public CInternalMsgContent<INT_MSG_CONTENT_SUBSCRIPTION>
{
	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::CDMManager;
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CSubscription"/> class.
	/// </summary>
	/// <param name="jsonDoc">The json document.</param>
	CSubscription(const rapidjson::Document& jsonDoc);

	/// <summary>
	/// Finalizes an instance of the <see cref="CSubscription"/> class.
	/// </summary>
	~CSubscription();

	/// <summary>
	/// The equal operator.
	/// </summary>
	/// <param name="other">The other object.</param>
	/// <returns>true if object is equal, false otherwise</returns>
	bool operator==(const CSubscription &other) const;

	/// <summary>
	/// Sets the data.
	/// </summary>
	/// <param name="topic">Topic name.</param>
	/// <param name="subscriptionData">The subscription data.</param>
	void SetSubscriptionData(const std::string& topic, const std::string& whereId, const std::string& subscriptionData);

	/// <summary>
	/// Needs to send update.
	/// </summary>
	/// <returns>true if update needs to be sent, false otherwise.</returns>
	bool NeedToSendUpdate() const;

	/// <summary>
	/// Updates the is sent.
	/// </summary>
	void UpdateIsSent();

	/// <summary>
	/// Gets the identifier.
	/// </summary>
	/// <returns>returns the id of the request</returns>
	std::string GetId() const;

	/// <summary>
	/// Gets the where identifier.
	/// </summary>
	/// <returns>id of the where_clause</returns>
	std::string GetWhereId() const;

	/// <summary>
	/// Gets the json RPC version.
	/// </summary>
	/// <returns>json RPC version</returns>
	std::string GetJsonRpcVersion() const;

	/// <summary>
	/// Gets the topic.
	/// </summary>
	/// <returns>subscription topic</returns>
	std::string GetTopic() const;

	/// <summary>
	/// Gets the subscription data.
	/// </summary>
	/// <returns>subscription data</returns>
	std::string GetSubscriptionData() const;

	/// <summary>
	/// Compares the subscriptions on the basis of topic
	/// </summary>
	/// <returns>true if topics of subscriptions are same</returns>
	bool Comparator(const CSubscription& s1)const;

	/// <summary>
	/// Clears the data in the subscription
	/// </summary>
	void ClearData(void);

private:
	std::string m_id;
	std::string m_whereId;
	std::string m_jsonrpc;
	std::string m_topic;

	std::string m_data;
	bool m_updateNeedsToBeSent;

	std::string CreateSubscriptionDataTemplate() const;
};

