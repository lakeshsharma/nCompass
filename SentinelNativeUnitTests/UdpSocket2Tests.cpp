#if 000//TODO-PORT
#include "stdafx.h"
#include "TestUtil.h"
#include "UdpSocket2.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(UdpSocket2Tests)
	{
	public:
		TEST_CLASS_INITIALIZE(_Initialize)
		{
			WSAData data;
			WSAStartup(MAKEWORD(2, 2), &data);
		}

		TEST_CLASS_CLEANUP(_Cleanup)
		{
			WSACleanup();
		}

		TEST_METHOD(IsMulticastAddress)
		{
			struct
			{
				int			line;
				const char* pAddress;
				bool		expectedResult;
			}
			static tests[] =
			{
				{ __LINE__, "0.0.0.0",			false },
				{ __LINE__, "127.0.0.1",		false },
				{ __LINE__, "255.0.0.0",		false },
				{ __LINE__, "223.255.255.255",	false },
				{ __LINE__, "224.0.0.0",		true  },
				{ __LINE__, "226.5.127.3",		true  },
				{ __LINE__, "239.255.255.255",	true  },
				{ __LINE__, "240.0.0.0",		false }
			};

			for (size_t test = 0; test < _countof(tests); test++)
			{
				in_addr addr;
				addr.S_un.S_addr = inet_addr(tests[test].pAddress);

				bool result = UdpSocket2::IsAddrMulticast(addr);
				Assert::AreEqual(tests[test].expectedResult, result, AssertMsg::Format(tests[test].line, nullptr, L"%S", tests[test].pAddress));
			}
		}

		TEST_METHOD(Constructor)
		{
			{
				UdpSocket2 socket;
				Assert::AreEqual((size_t) INVALID_HANDLE_VALUE, (size_t) socket.m_socket);
			}
			{
				UdpSocket2 socket(true);
				Assert::AreNotEqual((size_t) INVALID_HANDLE_VALUE, (size_t) socket.m_socket);
			}
		}

		TEST_METHOD(SimplePacket)
		{
			UdpSocket2 receiver;
			int result = receiver.Create("127.0.0.1", 64400);
			Assert::AreEqual(0, result, L"receiver.Create");

			int bytesRead = 0;
			char buffer[64];
			result = receiver.Read(buffer, _countof(buffer), bytesRead, 10);
			Assert::AreEqual((int) UdpSocket2::ErrReadTimeout, result, L"receiver.Read with timeout");

			const CStringA testStr("hello");

			const CStringA senderIp("127.0.0.2");
			const uint16_t senderPort = 64001;

			UdpSocket2 sender;
			result = sender.Create(senderIp, senderPort);
			sender.SendTo("127.0.0.1", 64400, (const char*) testStr, testStr.GetLength() + 1);

			union
			{
				sockaddr	from;
				sockaddr_in from_in;
			};

			result = receiver.Read(buffer, _countof(buffer), bytesRead, 10, &from);
			Assert::AreEqual((int) UdpSocket2::Success, result, L"receiver.Read with timeout");
			Assert::AreEqual((const char*) testStr, buffer, false, L"received data");
			Assert::AreEqual((int) AF_INET, (int) from_in.sin_family, L"sockaddr_in.sin_family");
			Assert::AreEqual((unsigned) senderPort, (unsigned) htons(from_in.sin_port), L"sockaddr_in.sin_port");

			_snprintf_s(buffer, _countof(buffer), _countof(buffer), "%u.%u.%u.%u", 
				from_in.sin_addr.S_un.S_un_b.s_b1,
				from_in.sin_addr.S_un.S_un_b.s_b2,
				from_in.sin_addr.S_un.S_un_b.s_b3,
				from_in.sin_addr.S_un.S_un_b.s_b4);

			Assert::AreEqual((const char*) senderIp, buffer, L"sockaddr_in.sin_addr");
		}
	};
}
#endif //TODO-PORT