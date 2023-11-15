#include "stdafx.h"
#include "Subscription.h"
#include "RapidJsonHelper.h"
#include "Logging/LogString.h"

using namespace std;

const string SUBSCRIBER_ID = "subscriberid";
const string ROUTING1_ID = "routing1id";
const string ROUTING2_ID = "routing2id";
const string ROUTING3_ID = "routing3id";
const string SERVICE_WINDOW_SHELL = "ServiceWindowShell";

const char* pJsonrpc = "jsonrpc";
const char* pId = "id";
const char* pParams = "params";
const char* pType = "type";
const char* pWhere = "where";
const char* pResult = "result";
const char* pData = "data";

const string SUBSCRIPTION_DATA_PLACEHOLDER = "insert_data_here";
const string SUBSCRIPTION_DATA_PLACEHOLDER_IN_QUOTES = "\"insert_data_here\"";

CSubscription::CSubscription(const rapidjson::Document& jsonDoc) : 
m_updateNeedsToBeSent(false)
{	
	const rapidjson::Value& paramsJsonValue = JsonGetObjectEx(jsonDoc, pParams);
	m_jsonrpc = JsonGetStringEx(jsonDoc, pJsonrpc);
	m_id = JsonGetIdEx(jsonDoc, pId);
	m_topic = JsonGetStringEx(paramsJsonValue, pType);
	if (JsonMemberExists(paramsJsonValue, pWhere))
	{
		const rapidjson::Value& whereJsonValue = JsonGetObjectEx(paramsJsonValue, pWhere);
		m_whereId = JsonGetIdStringEx(whereJsonValue, pId);
	}
}

CSubscription::~CSubscription()
{
}

bool CSubscription::operator==(const CSubscription &other) const
{
	//we are omitting method, data and updated on purpose so
	//we can use the std::find method in the std::list 
	//to erase a subscription
	return (m_id == other.m_id &&
			m_whereId == other.m_whereId &&
			m_jsonrpc == other.m_jsonrpc &&
			m_topic == other.m_topic);
}

bool CSubscription::NeedToSendUpdate() const
{
	return m_updateNeedsToBeSent;
}

void CSubscription::UpdateIsSent()
{
	m_updateNeedsToBeSent = false;
}

void CSubscription::SetSubscriptionData(const string& topic, const string& whereId, const std::string& data)
{
	if (topic == m_topic && whereId == m_whereId)
	{
		if (m_data != data)
		{
			m_updateNeedsToBeSent = true;
			m_data = data;
		}
	}
}

string CSubscription::GetId() const
{
	return m_id;
}

string CSubscription::GetWhereId() const
{
	return m_whereId;
}

string CSubscription::GetJsonRpcVersion() const
{
	return m_jsonrpc;
}

string CSubscription::GetTopic() const
{
	return m_topic;
}

string CSubscription::GetSubscriptionData() const
{
	string subscriptionData = CreateSubscriptionDataTemplate();
	
	subscriptionData.replace(subscriptionData.find(SUBSCRIPTION_DATA_PLACEHOLDER_IN_QUOTES), SUBSCRIPTION_DATA_PLACEHOLDER_IN_QUOTES.length(), m_data);

	return subscriptionData;
}

string CSubscription::CreateSubscriptionDataTemplate() const
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();

	writer.Key(pJsonrpc);		writer.String(m_jsonrpc);
	writer.Key(pType);			writer.String(m_topic);
	writer.Key(pId);			writer.String(m_id);
	writer.Key(pResult);		writer.String(SUBSCRIPTION_DATA_PLACEHOLDER);

	writer.EndObject();

	return writeStream.m_str;
}

bool CSubscription::Comparator(const CSubscription& s1)const
{
	return(this->GetTopic() == s1.GetTopic());
}

void  CSubscription::ClearData(void)
{
	this->m_data.clear();
}

