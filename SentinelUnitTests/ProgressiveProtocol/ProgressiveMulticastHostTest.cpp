
#include "stdafx.h"
#include "TestUtil.h"
#include "ProgressiveProtocol/ProgressiveMulticastFrame.h"
#include "ProgressiveProtocol/ProgressiveMulticastHost.h"
#include "RapidJsonHelper.h"
#include "ThreadSafeQueue.h"
#include "Utility/AssertMsg.h"
#include <chrono>
#include <thread>

using namespace std;
#include "gtest/gtest.h"

namespace SentinelNativeUnitTests 
{
	static const char* pKeyThreadNum = "ThreadNum";
	static const char* pKeyMsgNum = "MsgNum";

	static const char* pValidSystemId = "a35ef42d-8995-4060-befe-f09e75210a56";
	static const char* pInvalidSystemId = "a35ef42d-8995-4060-befe-f09e75210a57";

	static const char* pValidContentType = "application/vnd.onelink.v1+json";
	static const char* pInvalidContentType = "application/vnd.onelink.v1+JsOn";

     class ProgressiveMulticastHostTests : public ::testing::Test {
		std::string CreateTestFrame(const char* pSystemId, const char* pContentType, unsigned threadNum, unsigned msgNum)
		{
			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);

			writer.StartObject();
				writer.Key(pKeyThreadNum);					writer.Uint(threadNum);
				writer.Key(pKeyMsgNum);						writer.Uint(msgNum);
				writer.Key(pKeySourceDeviceId);				writer.Int(1);
				writer.Key(pKeySourceDeviceConfigVersion);	writer.Int(0);
				writer.Key(pKeyLevelValues);
				writer.StartArray();
					writer.StartObject();
						writer.Key(pKeyLevelId);	writer.Int(49);
						writer.Key(pKeyValue);		writer.Double(25.00);
						writer.Key(pKeyName);		writer.String("Date range");
                    writer.EndObject();
					writer.StartObject();
						writer.Key(pKeyLevelId);	writer.Int(19);
						writer.Key(pKeyValue);		writer.Double(4125.78);
						writer.Key(pKeyName);		writer.String("StdProg4000");
					writer.EndObject();
					writer.StartObject();
						writer.Key(pKeyLevelId);	writer.Int(23);
						writer.Key(pKeyValue);		writer.Double(1498.21);
						writer.Key(pKeyName);		writer.String("StdProg1000");
					writer.EndObject();
					writer.StartObject();
						writer.Key(pKeyLevelId);	writer.Int(21);
						writer.Key(pKeyValue);		writer.Double(2290.75);
						writer.Key(pKeyName);		writer.String("StdProg2000");
					writer.EndObject();
				writer.EndArray();
			writer.EndObject();

			string header("MESSAGE\n");

			header += OneLinkHeader::pOneLinkSystemId; header += ':'; header += pSystemId; header += '\n';
			header += "content-type:"; header += pContentType; header += '\n';
			header += "type:"; header += FromOneLinkUdp::pLevelBroadcast; header += '\n';
			header += "multicast-address:@239.1.1.1:61613\n";
			header += "destination:239.1.1.1:61613\n\n";

			header += writeStream.m_str;
			header.push_back(0);

			return header;
		}

        class ProgressiveMulticastHostTest : public ProgressiveMulticastHost
        {
        protected:
            ThreadSafeQueue<string>& m_hostQueue;
            virtual void OnMulticastMessage(ProgressiveMulticastFrame& message)
            {
                m_hostQueue.Push(message.GetBodyAsString());
            }

        public:
            ProgressiveMulticastHostTest(ThreadSafeQueue<string>& hostQueue)
                : m_hostQueue(hostQueue)
            {}

        private:
            friend SentinelNativeUnitTests::ProgressiveMulticastHostTests;
        };

        bool FindChannel(const ProgressiveMulticastHost* pHost, const char* pUrl)
        {
            for (auto it = pHost->m_channels.begin(); it != pHost->m_channels.end(); ++it)
            {
                if (((*it)->GetUrl() == pUrl))
                    return true;
            }

            return false;
        }

     public:
        void f_ProgressiveMulticastHost_simpleTest()
        {
            ThreadSafeQueue<string> hostQueue(8);
            UdpSocket2 sendSocket(true);

            unique_ptr<ProgressiveMulticastHostTest> pHost(new ProgressiveMulticastHostTest(hostQueue));
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            vector<string> updatedChannels;
            updatedChannels.push_back("udpstomp://127.0.0.1:60000");
            bool configResult = pHost->UpdateMulticastConfiguration(pValidSystemId, updatedChannels);
            ASSERT_TRUE(configResult) << "UpdateMulticastConfiguration";

            std::this_thread::sleep_for(std::chrono::milliseconds(15));
            std::string frame = CreateTestFrame(pValidSystemId, pValidContentType, 0, 1);
            int result = sendSocket.SendTo("127.0.0.1", 60000, frame.c_str(), (int) frame.length());
            ASSERT_EQ(0, result) << "SendTo 127.0.0.1:6000{0} (1)";
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            string jsonStr;
            ThreadSafeQueue<string>::Error err =  hostQueue.Pop(jsonStr, 1009999);
            ASSERT_EQ((int) ThreadSafeQueue<string>::Success, (int) err) <<  " hostQueue.Pop(msg, 100)";

            string OutputDebugString("### jsonStr ####\n");
			OutputDebugString += jsonStr;
			OutputDebugString+="\n### jsonStr ####\n";

			rapidjson::Document jsonDoc;
			bool  parseResult = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
			ASSERT_FALSE(parseResult) << AssertMsg::Format(0, L"Parse", L"%s", JsonException::GetParseErrorString(jsonDoc.GetParseError()));

			int threadNum = jsonDoc["pKeyThreadNum"].GetInt();
			ASSERT_EQ(0, threadNum) <<  "ThreadNum";

			int msgNum = jsonDoc["pKeyMsgNum"].GetInt();
			ASSERT_EQ(0, msgNum) << "NsgNum";

        }


        void f_ProgressiveMulticastHost_simpleUpdateTest( void )
        {
            ThreadSafeQueue<string> hostQueue(8);

            unique_ptr<ProgressiveMulticastHostTest> pHost(new ProgressiveMulticastHostTest(hostQueue));

            std::this_thread::sleep_for(std::chrono::milliseconds(15));

            vector<string> updatedChannels;

            updatedChannels.push_back("udpstomp://127.0.0.1:60001");
            updatedChannels.push_back("udpstomp://127.0.0.1:60002");
            updatedChannels.push_back("udpstomp://127.0.0.1:60003");

            bool configResult = pHost->UpdateMulticastConfiguration(pValidSystemId, updatedChannels);
            ASSERT_TRUE(configResult) << "UpdateMulticastConfiguration";
            ASSERT_EQ((size_t) 3, pHost->m_channels.size()) << "UpdateMulticastConfiguration (pHost->m_channels.size())";

            ASSERT_TRUE(FindChannel(pHost.get(), "udpstomp://127.0.0.1:60001")) <<  "Could not find: udpstomp://127.0.0.1:60001";
            ASSERT_TRUE(FindChannel(pHost.get(), "udpstomp://127.0.0.1:60002")) << "Could not find: udpstomp://127.0.0.1:60002";
            ASSERT_TRUE(FindChannel(pHost.get(), "udpstomp://127.0.0.1:60002")) << "Could not find: udpstomp://127.0.0.1:60003";

            updatedChannels.clear();

            updatedChannels.push_back("udpstomp://127.0.0.1:60002");
            updatedChannels.push_back("udpstomp://127.0.0.1:60004");
            updatedChannels.push_back("udpstomp://127.0.0.1:60005");

            configResult = pHost->UpdateMulticastConfiguration(pValidSystemId, updatedChannels);
            ASSERT_TRUE(configResult) << "UpdateMulticastConfiguration";
            ASSERT_EQ((size_t) 3, pHost->m_channels.size()) << "UpdateMulticastConfiguration (pHost->m_pChannels->size())";

            ASSERT_FALSE(FindChannel(pHost.get(), "udpstomp://127.0.0.1:60001")) <<  "Found: udpstomp://127.0.0.1:60001";
            ASSERT_FALSE(FindChannel(pHost.get(), "udpstomp://127.0.0.1:60003")) << "Found: udpstomp://127.0.0.1:60003";

            ASSERT_TRUE(FindChannel(pHost.get(), "udpstomp://127.0.0.1:60002")) << "Could not find: udpstomp://127.0.0.1:60002";
            ASSERT_TRUE(FindChannel(pHost.get(), "udpstomp://127.0.0.1:60004")) << "Could not find: udpstomp://127.0.0.1:60004";
            ASSERT_TRUE(FindChannel(pHost.get(), "udpstomp://127.0.0.1:60005")) << "Could not find: udpstomp://127.0.0.1:60005";
        }
     };

       TEST_F(ProgressiveMulticastHostTests, ProgressiveMulticastHost_simpleTest)
        {
            f_ProgressiveMulticastHost_simpleTest();
        }

        TEST_F(ProgressiveMulticastHostTests, ProgressiveMulticastHost_simpleUpdateTest)
        {
            f_ProgressiveMulticastHost_simpleUpdateTest();
        }
   };
