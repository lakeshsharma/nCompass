#include "stdafx.h"
#include "SpeedMediaConfig.h"
#include "Subscription.h"

#include <iostream>
#include <map>
#include <string>

#include "gtest/gtest.h"

using namespace std;

const string SPEEDMEDIA_CONFIGURATION_TOPIC("SpeedMediaConfiguration");

namespace SentinelNativeUnitTests
{
    TEST(SpeedMediaConfigUnitTests, SpeedMediaConfigSuccessfulSubcriptionToAllUnitTest)
    {
        string subscriptionData("{\"jsonrpc\":\"2.0\", \"params\":{\"type\":\"SpeedMediaConfiguration\", \"where\":{\"id\":\"ALL\"}}, \"method\":\"subscribe\", \"id\":\"999\", \"routing1id\":\"ATI_Attractor_Main\", \"subscriberid\":\"ServiceWindowShell\"}");
        string subscriptionResponse("{\"jsonrpc\":\"2.0\",\"type\":\"SpeedMediaConfiguration\",\"id\":\"999\",\"result\":[{\"Key\":\"key1\",\"Value\":\"value1\"},{\"Key\":\"key2\",\"Value\":\"value2\"},{\"Key\":\"key3\",\"Value\":\"value3\"},{\"Key\":\"key4\",\"Value\":\"value4\"}]}");

        rapidjson::Document jsonDoc;
        if (!jsonDoc.Parse(subscriptionData.c_str()).HasParseError())
        {
            CSubscription subscription(jsonDoc);
            CSpeedMediaConfig speedMediaConfig;
            map<string, string> keyValuePairs;
            keyValuePairs["key1"] = "value1";
            keyValuePairs["key2"] = "value2";
            keyValuePairs["key3"] = "value3";
            keyValuePairs["key4"] = "value4";
            speedMediaConfig.SetConfigurationKeyValuePairs(keyValuePairs);
            ASSERT_TRUE(speedMediaConfig.Subscribe(subscription));
            speedMediaConfig.SetSubscriptionDataResponse(SPEEDMEDIA_CONFIGURATION_TOPIC, "ALL");
            vector<CSubscription> subscriptions;
            speedMediaConfig.AppendToSubscriptionData(subscriptions);
            ASSERT_TRUE(subscriptions[0].GetSubscriptionData() == subscriptionResponse);
        }
    }

    TEST(SpeedMediaConfigUnitTests, SpeedMediaConfigSuccessfulSubcriptionToSpecificKeyUnitTest)
    {
        string subscriptionData("{\"jsonrpc\":\"2.0\", \"params\":{\"type\":\"SpeedMediaConfiguration\", \"where\":{\"id\":\"key2\"}}, \"method\":\"subscribe\", \"id\":\"999\", \"routing1id\":\"ATI_Attractor_Main\", \"subscriberid\":\"ServiceWindowShell\"}");
        string subscriptionResponse("{\"jsonrpc\":\"2.0\",\"id\":\"999\",\"result\":[{\"Key\":\"key2\",\"Value\":\"value2\"}]}");

        rapidjson::Document jsonDoc;
        if (!jsonDoc.Parse(subscriptionData.c_str()).HasParseError())
        {
            CSubscription subscription(jsonDoc);
            CSpeedMediaConfig speedMediaConfig;
            map<string, string> keyValuePairs;
            keyValuePairs["key1"] = "value1";
            keyValuePairs["key2"] = "value2";
            keyValuePairs["key3"] = "value3";
            keyValuePairs["key4"] = "value4";
            speedMediaConfig.SetConfigurationKeyValuePairs(keyValuePairs);
            ASSERT_TRUE(speedMediaConfig.Subscribe(subscription));
            speedMediaConfig.SetSubscriptionDataResponse(SPEEDMEDIA_CONFIGURATION_TOPIC, "key2");
            vector<CSubscription> subscriptions;
            speedMediaConfig.AppendToSubscriptionData(subscriptions);
            //ASSERT_TRUE(subscriptions[0].GetSubscriptionData() == subscriptionResponse);
        }
    }

    TEST(SpeedMediaConfigUnitTests, SpeedMediaConfigFailedSubcriptionUnitTest)
    {
        string subscriptionData("{\"jsonrpc\":\"2.0\", \"params\":{\"type\":\"Player\", \"where\":{\"id\":\"key2\"}}, \"method\":\"subscribe\", \"id\":\"999\", \"routing1id\":\"ATI_Attractor_Main\", \"subscriberid\":\"ServiceWindowShell\"}");

        rapidjson::Document jsonDoc;
        if (!jsonDoc.Parse(subscriptionData.c_str()).HasParseError())
        {
            CSubscription subscription(jsonDoc);
            CSpeedMediaConfig speedMediaConfig;
            map<string, string> keyValuePairs;
            keyValuePairs["key1"] = "value1";
            keyValuePairs["key2"] = "value2";
            keyValuePairs["key3"] = "value3";
            keyValuePairs["key4"] = "value4";
            speedMediaConfig.SetConfigurationKeyValuePairs(keyValuePairs);
            ASSERT_FALSE(speedMediaConfig.Subscribe(subscription));
        }
    }
};

