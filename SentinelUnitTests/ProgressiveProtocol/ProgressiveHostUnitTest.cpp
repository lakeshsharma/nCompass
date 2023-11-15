
#include "stdafx.h"
#include "gtest/gtest.h"
#include "../ProgressiveProtocol/ProgressiveHostManager.h"

using namespace std;


class ProgressiveHostUnitTests : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

};

TEST_F(ProgressiveHostUnitTests, ProgressiveHostConstructorAddressUnitTest)
{
	CIPAddressArray addressArray;
	CIPAddress address;
	address.SetIP("1.1.1.1");
	address.SetPort(1234);
	addressArray.Add(address);

	CProgressiveHost host(addressArray);
	EXPECT_EQ(host.GetHostPort(), (DWORD)0);
	EXPECT_TRUE(host.HasAddress());
	EXPECT_TRUE(address == host.GetIPAddress());
	string url = host.GetURL();
	string expectedUrl("ssl://1.1.1.1:1234?wireFormat=stomp");
	EXPECT_EQ(expectedUrl, url);
}

TEST_F(ProgressiveHostUnitTests, ProgressiveHostConstructorHostNameUnitTest)
{
	CProgressiveHost host("www.google.com", 1234);
	EXPECT_TRUE(host.HasAddress());
	EXPECT_EQ(host.GetHostPort(), (DWORD)1234);

	CProgressiveHost host2("bogusadssressbogusaddressbogusaddress", 1234);
	EXPECT_FALSE(host2.HasAddress());
}

TEST_F(ProgressiveHostUnitTests, ProgressiveHostRandomizeUnitTest)
{
	CIPAddressArray addressArray;
	CIPAddress address;
	address.SetIP("1.1.1.1");
	addressArray.Add(address);
	address.SetIP("2.2.2.2");
	addressArray.Add(address);
	address.SetIP("3.3.3.3");
	addressArray.Add(address);
	address.SetIP("4.4.4.4");
	addressArray.Add(address);
	address.SetIP("5.5.5.5");
	addressArray.Add(address);
	address.SetIP("6.6.6.6");
	addressArray.Add(address);
	address.SetIP("7.7.7.7");
	addressArray.Add(address);
	address.SetIP("8.8.8.8");
	addressArray.Add(address);
	address.SetIP("9.9.9.9");
	addressArray.Add(address);
	address.SetIP("10.10.10.10");
	addressArray.Add(address);
	address.SetIP("11.11.11.11");
	addressArray.Add(address);
	address.SetIP("12.12.12.12");
	addressArray.Add(address);
	address.SetIP("13.13.13.13");
	addressArray.Add(address);
	address.SetIP("14.14.14.14");
	addressArray.Add(address);
	address.SetIP("15.15.15.15");
	addressArray.Add(address);

	int repeatCount = 0;
	int compareCount = 0;
	CIPAddress prevTopAddress;
	for (int newIndex = 0; newIndex < 100; newIndex++)
	{
		// Everytime this constructor is called there should be a different randomization of the list
		CProgressiveHost *host = new CProgressiveHost(addressArray);
		if (newIndex > 0)
		{
			compareCount++;	// This is how many times we caompared the top address
			if (host->GetIPAddress() == prevTopAddress)
			{
				repeatCount++;	// This is the count of how manytimes the top item was repeated
			}
		}
		prevTopAddress = host->GetIPAddress();
		delete host;
	}

	// The chances that the same IP was at the top of the list are 1 in 15^99
	ASSERT_TRUE(repeatCount != compareCount);
}

TEST_F(ProgressiveHostUnitTests, ProgressiveTwoHostsRandomizeUnitTest)
{
	int address1Count = 0;
	int address2Count = 0;
	for (int newIndex = 0; newIndex < 3000; newIndex++)
	{
		CIPAddressArray addressArray;
		CIPAddress address1;
		address1.SetIP("1.1.1.1");
		addressArray.Add(address1);
		CIPAddress address2;
		address2.SetIP("2.2.2.2");
		addressArray.Add(address2);

		// Everytime this constructor is called there should be a different randomization of the list
		CProgressiveHost *host = new CProgressiveHost(addressArray);

		if (host->GetIPAddress() == address1)
		{
			address1Count++;	// This is the count of how many times the first address was repeated
		}
		else if (host->GetIPAddress() == address2)
		{
			address2Count++;	// This is the count of how manytimes the second address was repeated
		}
		delete host;
	}

	// The two address should be pretty evenly distributed amongst 3000 calls
	if (address1Count < 1 || address2Count < 1)
	{		
		string msg;
		msg = FormatString("address 1 set %d times. address 2 set %d times", address1Count, address2Count);
		FAIL()<<msg.c_str();
	}
	
}


TEST_F(ProgressiveHostUnitTests, ProgressiveHostSetToNextUnitTest)
{
	CIPAddressArray addressArray;
	CIPAddress address;
	address.SetIP("1.1.1.1");
	addressArray.Add(address);
	address.SetIP("2.2.2.2");
	addressArray.Add(address);
	address.SetIP("3.3.3.3");
	addressArray.Add(address);
	address.SetIP("4.4.4.4");
	addressArray.Add(address);
	int addressCount = addressArray.GetCount();

	CProgressiveHost host(addressArray);

	int hasAddressCount = 0;
	int setTextSuccessCount = 0;
	for (int addressIndex = 0; addressIndex < addressCount; addressIndex++)
	{
		if (host.HasAddress())
		{
			hasAddressCount++;
		}
		if (host.SetToNextAddress())
		{
			setTextSuccessCount++;
		}
	}
	EXPECT_FALSE(host.HasAddress());
	EXPECT_EQ(addressCount, hasAddressCount);
	EXPECT_EQ(setTextSuccessCount, addressCount - 1);

	CIPAddress uninitAddress;
	EXPECT_TRUE(uninitAddress == host.GetIPAddress());
	EXPECT_TRUE(host.GetURL().empty());

	host.UpdateAddress();
	EXPECT_TRUE(host.HasAddress());
}

TEST_F(ProgressiveHostUnitTests, ProgressiveHostManagerDNSSetupUnitTest)
{
	std::vector<std::string> hostNameArray;
	hostNameArray.push_back("bogusadssressbogusaddressbogusaddress:1234");
	hostNameArray.push_back("www.google.com : 5678");

	CProgressiveHostManager hostManager(hostNameArray, true);

	CProgressiveHost *host1 = hostManager.GetCurrentHost();

	EXPECT_TRUE(host1 != nullptr);

	if (host1 != NULL)
	{
		EXPECT_EQ(host1->GetHostPort(), (DWORD)1234);
		EXPECT_FALSE(host1->HasAddress());
	}

	// When we get the url it should have moved to the second host since the first host had no addresses.
	string url = hostManager.GetUrl();

	CProgressiveHost *host2 = hostManager.GetCurrentHost();
	EXPECT_FALSE(host1 == host2);
	
}

TEST_F(ProgressiveHostUnitTests, ProgressiveHostManagerAddressSetupUnitTest)
{
	std::vector<std::string> addressStringArray;
	addressStringArray.push_back("1.1.1.1:1234");
	addressStringArray.push_back("2.2.2.2:2345");
	addressStringArray.push_back("3.3.3.3:3456");

	CProgressiveHostManager hostManager(addressStringArray, false);

	CProgressiveHost *host = hostManager.GetCurrentHost();

	EXPECT_TRUE(host != nullptr);

	if (host != NULL)
	{
		EXPECT_TRUE(host->HasAddress());
	}

	string url1 = hostManager.GetUrl();
	EXPECT_TRUE(
		url1 == "ssl://1.1.1.1:1234?wireFormat=stomp" || url1 == "ssl://2.2.2.2:2345?wireFormat=stomp" || url1 == "ssl://3.3.3.3:3456?wireFormat=stomp");

	hostManager.ConnectionFailure();
	string url2 = hostManager.GetUrl();
	EXPECT_TRUE(
		url2 == "ssl://1.1.1.1:1234?wireFormat=stomp" || url2 == "ssl://2.2.2.2:2345?wireFormat=stomp" || url2 == "ssl://3.3.3.3:3456?wireFormat=stomp");
	EXPECT_TRUE(url1 != url2);

	hostManager.ConnectionFailure();
	string url3 = hostManager.GetUrl();
	EXPECT_TRUE(
		url3 == "ssl://1.1.1.1:1234?wireFormat=stomp" || url3 == "ssl://2.2.2.2:2345?wireFormat=stomp" || url3 == "ssl://3.3.3.3:3456?wireFormat=stomp");

	EXPECT_TRUE(url1 != url3);
	EXPECT_TRUE(url2 != url3);

	hostManager.ConnectionFailure();
	string urlLast = hostManager.GetUrl();
	EXPECT_TRUE(urlLast.empty());

	hostManager.AddressCheck();
	string urlReset = hostManager.GetUrl();
	EXPECT_EQ(url1, urlReset);
}