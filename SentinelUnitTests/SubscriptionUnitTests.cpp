#include "stdafx.h"
#include "Subscription.h"
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;

namespace SentinelUnitTests
{

    TEST(SubscriptionUnitTests, SubscriptionConstructorUnitTest)
    {
        std::string subscriptionData("{\"jsonrpc\":\"2.0\", \"params\":{\"type\":\"SampleType\", \"where\":{\"id\":3147483647}}, \"method\":\"subscribe\", \"id\":\"69\"}");
        string sampleJsonData = "{\"stringdata\":\"stringvalue\",\"intvalue\":987,\"doubleValue\":2.67}";
        string responseData = "{\"jsonrpc\":\"2.0\",\"type\":\"SampleType\",\"id\":\"69\",\"result\":{\"stringdata\":\"stringvalue\",\"intvalue\":987,\"doubleValue\":2.67}}";

        rapidjson::Document jsonDoc;
        if (!jsonDoc.Parse(subscriptionData.c_str()).HasParseError())
        {
            CSubscription subscription(jsonDoc);
            ASSERT_FALSE(subscription.NeedToSendUpdate());
            subscription.SetSubscriptionData("SampleType", "3147483647", sampleJsonData);
            ASSERT_TRUE(subscription.NeedToSendUpdate());
            subscription.UpdateIsSent();
            ASSERT_FALSE(subscription.NeedToSendUpdate());
            subscription.SetSubscriptionData("SampleType", "3147483647", sampleJsonData);
            ASSERT_FALSE(subscription.NeedToSendUpdate());
            string temp = subscription.GetSubscriptionData();
            ASSERT_TRUE(responseData == subscription.GetSubscriptionData());
            ASSERT_TRUE(subscription.GetJsonRpcVersion() == "2.0");
            ASSERT_TRUE(subscription.GetId() == "69");
            ASSERT_TRUE(subscription.GetTopic() == "SampleType");
            ASSERT_TRUE(subscription.GetWhereId() == "3147483647");
        }
    }

    TEST(SubscriptionUnitTests, SubscriptionEqualOperatorUnitTest)
    {
        std::string subscriptionData1("{\"jsonrpc\":\"2.0\", \"params\":{\"type\":\"SampleType\", \"where\":{\"id\":\"3147483647\"}}, \"method\":\"subscribe\", \"id\":\"69\"}");

        std::string subscriptionData2("{\"jsonrpc\":\"2.0\", \"params\":{\"type\":\"SampleType\", \"where\":{\"id\":3147483647}}, \"method\":\"subscribe\", \"id\":\"69\"}");

        rapidjson::Document jsonDoc1;
        rapidjson::Document jsonDoc2;
        if (!jsonDoc1.Parse(subscriptionData1.c_str()).HasParseError() &&
            !jsonDoc2.Parse(subscriptionData2.c_str()).HasParseError())
        {
            CSubscription subscription1(jsonDoc1);
            CSubscription subscription2(jsonDoc2);
            ASSERT_TRUE(subscription1 == subscription2);
        }
    }

    TEST(SubscriptionUnitTests, SubscriptionConstructorWithInvalidWhereIdUnitTest)
    {
        std::string subscriptionData("{\"jsonrpc\":\"2.0\", \"params\":{\"type\":\"SampleType\", \"where\":{\"id\":\"3147483647\"}}, \"method\":\"subscribe\", \"id\":-42949672958}");
        string sampleJsonData = "{\"stringdata\":\"stringvalue\",\"intvalue\":987,\"doubleValue\":2.67}";
        string responseData = "{\"jsonrpc\":\"2.0\",\"type\":\"SampleType\",\"id\":\"-42949672958\",\"result\":{\"stringdata\":\"stringvalue\",\"intvalue\":987,\"doubleValue\":2.67}}";

        rapidjson::Document jsonDoc;
        if (!jsonDoc.Parse(subscriptionData.c_str()).HasParseError())
        {
            CSubscription subscription(jsonDoc);
            ASSERT_FALSE(subscription.NeedToSendUpdate());
            subscription.SetSubscriptionData("SampleType", "3147483647", sampleJsonData);
            ASSERT_TRUE(subscription.NeedToSendUpdate());
            string temp = subscription.GetSubscriptionData();
			ASSERT_TRUE(responseData == subscription.GetSubscriptionData());
            ASSERT_TRUE(subscription.GetJsonRpcVersion() == "2.0");
            ASSERT_TRUE(subscription.GetId() == "-42949672958");
            ASSERT_TRUE(subscription.GetTopic() == "SampleType");
            ASSERT_TRUE(subscription.GetWhereId() == "3147483647");
        }
    }

}
