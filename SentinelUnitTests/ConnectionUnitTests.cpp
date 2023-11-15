
#include "stdafx.h"
#include "Connection.h"
#include "windef.h"


#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{

	TEST(ConnectionUnitTests, Constructor)
		{
			CIPAddress ip;
			ip.SetIP("0.0.0.0");

			CConnection conn;

			ASSERT_TRUE(conn.GetActualIP() == ip);
			ASSERT_TRUE(conn.GetAlternateDNS() == ip);
			ASSERT_TRUE(conn.GetDefaultGateway() == ip);
			ASSERT_TRUE(conn.GetPreferredDNS() == ip);
			ASSERT_TRUE(conn.GetSubnetMask() == ip);
			ASSERT_TRUE(conn.AutoDHCP());
			ASSERT_TRUE(conn.GetPort() == HOST_PORT_DEFAULT);
		}

		TEST(ConnectionUnitTests, SlotNumber)
		{
			CConnection conn;

			const long slotNumber = 56;
			conn.SetSlotNumber(slotNumber);
			long result = conn.GetSlotNumber();

			ASSERT_EQ(slotNumber, result);
		}

		TEST(ConnectionUnitTests, SlotmastID)
		{
			CConnection conn;

			const long slotmastID = INT_MAX;
			conn.SetSlotmastID(slotmastID);
			long result = conn.GetSlotmastID();

			ASSERT_EQ(slotmastID, result);
		}

		TEST(ConnectionUnitTests, Host)
		{
			CConnection conn;

			const char* pHost1 = "Host1";

			// scoped to ensure tmpHost is deleted and that copy on write doesn't thwart the test.
			{
				string tmpHost(pHost1);
				conn.SetHost(tmpHost);
			}

			string result = conn.GetHost();

			ASSERT_EQ(string(pHost1), result);

			// combined to ensure the set replaces old
			const char* pHost2 = "Host2";
			char host2[8];
			strncpy(host2, pHost2, _countof(host2));
			conn.SetHost(host2);
			string resultA(conn.GetHost());

			ASSERT_EQ(string(pHost2), resultA);
		}

		TEST(ConnectionUnitTests, Host2)
		{
			CConnection conn;

			const char* pHost1 = "";

			// scoped to ensure tmpHost is deleted and that copy on write doesn't thwart the test.
			{
				string tmpHost(pHost1);
				conn.SetHost(tmpHost);
			}

			string result = conn.GetHost();

			ASSERT_EQ(string(pHost1),  result);

			// combined to ensure the set replaces old
			conn.SetHost(nullptr);
			result = conn.GetHost();

			ASSERT_EQ(string(pHost1), result);
		}

		TEST(ConnectionUnitTests, Port)
		{
			CConnection conn;

			const long port1 = 1234;
			const long port2 = USHRT_MAX + 1;
			const long port3 = 1;

			// scoped to ensure tmpPort is deleted and that copy on write doesn't thwart the test.
			{
				string tmpPort;
				tmpPort = FormatString("%ld", port1);
				conn.SetPort(tmpPort);
			}

			long result = conn.GetPort();
			ASSERT_EQ(port1, result);

			{
				string tmpPort;
				tmpPort = FormatString("%ld", port2);
				conn.SetPort(tmpPort);
			}

			result = conn.GetPort();
			ASSERT_EQ(port2, result);

			// combined to ensure the set replaces old
			conn.SetPort(port3);
			result = conn.GetPort();

			ASSERT_EQ(port3, result);
		}

		TEST(ConnectionUnitTests, MyIP)
		{
			CConnection conn;

			const char* pIP = "1.2.3.4";

			{
				string ipStr(pIP);
				CIPAddress ip;
				ip.SetIP(ipStr);
				conn.SetMyIP(ip);
			}

			CIPAddress ip = conn.GetMyIP();
			char address[32];
			ip.GetIP(address, _countof(address));

			ASSERT_STREQ(pIP, address);
		}

		TEST(ConnectionUnitTests, ConnectionSetActualIP)
		{
			CConnection conn;
			CIPAddress address;
			address.SetIP("127.0.0.1");
			conn.SetActualIP(address);

			ASSERT_TRUE(conn.GetActualIP() == address);
		}

		TEST(ConnectionUnitTests, ConnectionSetDHCP)
		{
			CConnection conn;
			bool bDhcp = true;
			conn.SetDHCP(bDhcp);
			ASSERT_TRUE(conn.AutoDHCP() == bDhcp);

			bDhcp = false;
			conn.SetDHCP(bDhcp);
			ASSERT_TRUE(conn.AutoDHCP() == bDhcp);
		}

		TEST(ConnectionUnitTests, ConnectionSetHoststring)
		{
			CConnection conn;
			string hostname = "thehostname";
			conn.SetHost(hostname);

			ASSERT_TRUE(conn.GetHost() == hostname);
		}

};
			

