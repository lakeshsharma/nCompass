#include "stdafx.h"
#include "TestUtil.h"
#include "ProgressiveProtocol/ProgressiveMulticastChannel.h"
#include "ProgressiveProtocol/ProgressiveMulticastFrame.h"
#include "Stomp/StompHelper.h"
#include "AssertMsg.h"
#include <stdlib.h>

#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	static const char* pValidSystemId = "a35ef42d-8995-4060-befe-f09e75210a56";
	static const char* pInvalidSystemId = "a35ef42d-8995-4060-befe-f09e75210a57";

	static const char* pContentTypeTest = "application/vnd.onelink.v1+test";
	static const char* pContentTypeBad = "application/vnd.onelink.v1+tesT";

	static const char* pFormatValTest = "test";
	static const char* ppFormatValBad = "tesT";
	class ProgressiveMulticastChannelTests : public ::testing::Test
	{
	public:
		ProgressiveMulticastChannelTests(void)
		{
		}

		~ProgressiveMulticastChannelTests(void)
		{
		}

		void f_ParseUrl(void)
		{
			struct TestData
			{
				int			line;
				const char*	pStr;
				bool		expectedResult;
				ULONG		addr;
				WORD		port;
			}
			static testData[] =
			{
				{ __LINE__, "1.2.3.4:4607",			true, 0x04030201U, 4607 },

				{ __LINE__, "1.1.1.1",				false, 0x04030201U, 4607 },
				{ __LINE__, "1.2.3.4:4607",			true, 0x04030201U, 4607 },
				{ __LINE__, "255.255.255.255:1234",	true, 0xffffffffU, 1234 },
				{ __LINE__, "192.168.1.1:5678",		true, 0x0101a8c0U, 5678 },
				{ __LINE__, "10.10.1.1:65534",		true, 0x01010a0aU, 65534 },
				{ __LINE__, "132.254.111.10:1",		true, 0x0a6ffe84U, 1 },
				{ __LINE__, "26.10.2.10:80",		true, 0x0a020a1aU, 80 },
				{ __LINE__, "127.0.0.1:65535",		true, 0x0100007fU, 65535 },
				{ __LINE__, "10.1..2:32768",		true, 0x0200010aU, 32768 },
				{ __LINE__, "",					false,  0xffffffffU, 1234 },
				{ __LINE__, nullptr,			false,  0xffffffffU, 1234 },
				{ __LINE__, "10.10.10:1",		false,  0xffffffffU, 1234 },
				{ __LINE__, "10.10.10.10.:1",	false, 0xffffffffU, 1234 },
				{ __LINE__, "10.10.10.10;1",	false,  0xffffffffU, 1234 },
				{ __LINE__, "10.10",			false,  0xffffffffU, 1234 },
				{ __LINE__, "10",				false,  0xffffffffU, 1234 },
				{ __LINE__, "a.a.a.a",			false,  0xffffffffU, 1234 },
				{ __LINE__, "10.0.0.a",			false,  0xffffffffU, 1234 },
				{ __LINE__, "10.10.10.256:1",	false,  0xffffffffU, 1234 },
				{ __LINE__, "222.222.2.999:1",	false,  0xffffffffU, 1234 },
				{ __LINE__, "999.10.10.20:1",	false,  0xffffffffU, 1234 },
				{ __LINE__, "2222.22.22.22:1",	false,  0xffffffffU, 1234 },
				{ __LINE__, "22.2222.22.2:1",	false,  0xffffffffU, 1234 },
				{ __LINE__, "?22.2222.22.2:1",	false,  0xffffffffU, 1234 },

				{ __LINE__, "1.1.1.1:/",		false, 0xffffffffU, 1234 },
				{ __LINE__, "1.1.1.1:000000/",	false, 0xffffffffU, 1234 },
				{ __LINE__, "1.1.1.1:00000x/",	false, 0xffffffffU, 1234 },
				{ __LINE__, "1.1.1.1:0x1234/",	false, 0xffffffffU, 1234 }
			};

			for (size_t test = 0; test < _countof(testData); test++)
			{
				TestData& data = testData[test];

				struct sockaddr_in addr;
				WORD    port;

				bool result = ProgressiveMulticastChannelSocket::ParseUrl(data.pStr, addr.sin_addr, port);
				ASSERT_EQ(data.expectedResult, result) << AssertMsg::Format(data.line, L"Result");

				if (result)
				{
					ASSERT_EQ(data.addr, addr.sin_addr.s_addr) << AssertMsg::Format(data.line, L"addr");
					ASSERT_EQ((DWORD)data.port, (DWORD)port) << AssertMsg::Format(data.line, L"port");
				}
			}

		}

		void f_BadConstructor(void)
		{
			try
			{
				unique_ptr<ProgressiveMulticastChannelSocket> socket(new ProgressiveMulticastChannelSocket("1"));
			}
			catch (SentinelExceptionT<ProgressiveChannelError>& e)
			{
				ASSERT_EQ((intptr_t)InvalidProtocol, e.code());
			}

			try
			{
				unique_ptr<ProgressiveMulticastChannelSocket> socket(new ProgressiveMulticastChannelSocket(""));
			}
			catch (SentinelExceptionT<ProgressiveChannelError>& e)
			{
				ASSERT_EQ((intptr_t)InvalidParameter, e.code());
			}

			try
			{
				unique_ptr<ProgressiveMulticastChannelSocket> socket(new ProgressiveMulticastChannelSocket("udpstomp://127.0.0.1:0"));
			}
			catch (SentinelExceptionT<ProgressiveChannelError>& e)
			{
				ASSERT_EQ((intptr_t)InvalidPort, e.code());
			}

			try
			{
				unique_ptr<ProgressiveMulticastChannelSocket> socket(new ProgressiveMulticastChannelSocket("badprotocol://127.0.0.1:12345"));
			}
			catch (SentinelExceptionT<ProgressiveChannelError>& e)
			{
				ASSERT_EQ((intptr_t)InvalidProtocol, e.code());
			}

			try
			{
				unique_ptr<ProgressiveMulticastChannelSocket> socket(new ProgressiveMulticastChannelSocket("stomp://127.0.0.1:12345"));
			}
			catch (SentinelExceptionT<ProgressiveChannelError>& e)
			{
				ASSERT_EQ((intptr_t)InvalidMulticastAddress, e.code());
			}

			try
			{
				unique_ptr<ProgressiveMulticastChannelSocket> socket(new ProgressiveMulticastChannelSocket("stomp://127.0.0.1:12345"));
			}
			catch (SentinelExceptionT<ProgressiveChannelError>& e)
			{
				ASSERT_EQ((intptr_t)InvalidMulticastAddress, e.code());
			}
		}

		static string CreateFrameV(const char* pCommand, const char* pContentType, const char* pType, const char* pBody, size_t bodyLen, size_t maxHeaders, va_list args)
		{
			if (pContentType && !*pContentType)
				pContentType = nullptr;

			if (pType && !*pType)
				pType = nullptr;

			string frame;
			frame.reserve(24 + (pContentType ? 48 : 0) + (bodyLen ? 24 : 0) + (pType ? 24 : 0) + bodyLen);

			frame += pCommand;
			frame += '\n';
			if (pContentType)
			{
				frame += Stomp::Header::pContentType;
				frame += ':';
				frame += pContentType;
				frame += '\n';
			}

			if (bodyLen)
			{
				frame += Stomp::Header::pContentLength;
				frame += ':';
				string buffer;
				buffer = to_string((int)bodyLen);
				frame += buffer;
				frame += '\n';
			}

			if (pType)
			{
				frame += OneLinkHeader::pType;
				frame += ':';
				frame += pType;
				frame += '\n';
			}

			for (size_t header = 0; header < maxHeaders; header++)
			{
				const char* pKey = va_arg(args, const char*);
				if (pKey == nullptr)
					break;

				const char* pVal = va_arg(args, const char*);
				if (pVal == nullptr)
					break;

				if (strcmp(pKey, "content-type") != 0 &&
					strcmp(pKey, "content-length") != 0)
				{
					frame += pKey;
					frame += ':';
					frame += pVal;
					frame += '\n';
				}
			}

			// end headers
			frame += '\n';

			if (pBody)
				frame += string(pBody, bodyLen);

			frame.push_back('\0');

			return frame;
		}

		string CreateClientFrame(Stomp::RecvCommand::Type command, const char* pContentType, const char* pType, const char* pBody, size_t bodyLen, size_t maxHeaders, ...)
		{
			string frame;

			va_list args;
			va_start(args, maxHeaders);

			switch (command)
			{
			case Stomp::RecvCommand::Connected:
				frame = CreateFrameV("CONNECTED", nullptr, nullptr, nullptr, 0, maxHeaders, args);
				break;

			case Stomp::RecvCommand::Receipt:
				frame = CreateFrameV("RECEIPT", nullptr, nullptr, nullptr, 0, maxHeaders, args);
				break;

			case Stomp::RecvCommand::Message:
				frame = CreateFrameV("MESSAGE", pContentType, pType, pBody, pBody ? (bodyLen ? bodyLen : strlen(pBody)) : 0, maxHeaders, args);
				break;

			case Stomp::RecvCommand::Error:
				frame = CreateFrameV("ERROR", pContentType, pType, pBody, pBody ? (bodyLen ? bodyLen : strlen(pBody)) : 0, maxHeaders, args);
				break;
			default:

				break;
			}

			va_end(args);
			return frame;
		}
	};
	TEST_F(ProgressiveMulticastChannelTests, ParseUrl)
	{
		f_ParseUrl();
	}

	TEST_F(ProgressiveMulticastChannelTests, BadConstructor)
	{
		f_BadConstructor();

	}
}

