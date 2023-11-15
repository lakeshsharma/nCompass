#include "stdafx.h"
#include "IPItem.h"

#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;

const string ip = "10.138.128.96";

class IPItemUnitTests : public ::testing::Test
{
	public:
        virtual void SetUp() 
        {
        }

        virtual void TearDown() 
        {
        }
};
TEST_F(IPItemUnitTests, IPItemConstructors)
{
	CIPItem *item = new CIPItem();

	ASSERT_TRUE(item->GetIP() == "");
	delete item;

	item = new CIPItem(ip);
	ASSERT_TRUE(item->GetIP() == ip);
	delete item;

	item = new CIPItem(ip, 4);
	ASSERT_TRUE(item->GetIP() == ip);
	ASSERT_TRUE(item->GetFailCount() == 4);
	delete item;
}

TEST_F(IPItemUnitTests, IPItemResetFailCount)
{
	CIPItem *item = new CIPItem(ip);
	ASSERT_TRUE(item->GetFailCount() == 0);
	delete item;

	item = new CIPItem(ip, 4);
	ASSERT_TRUE(item->GetIP() == ip);
	ASSERT_TRUE(item->GetFailCount() == 4);

	item->ResetFailCount();
	ASSERT_TRUE(item->GetFailCount() == 0);

	delete item;

}

TEST_F(IPItemUnitTests, PItemIncFailCount)
{
	CIPItem *item = new CIPItem(ip);
	ASSERT_TRUE(item->GetFailCount() == 0);
	item->IncrementFailCount();
	ASSERT_TRUE(item->GetFailCount() == 1);
	item->IncrementFailCount();
	ASSERT_TRUE(item->GetFailCount() == 2);

	delete item;
}



