#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveQueueToOneLink.h"
#include "TestUtil.h"

using namespace std;
using namespace TestUtil;

#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{
		TEST(ProgressiveQueueToOneLinkTests, ProgressiveQueueToOneLink_ctor)
		{
			ProgressiveQueueToOneLink msg(ToOneLink::DeviceAcknowledgement, string("Body"));

			ASSERT_EQ((int) ToOneLink::DeviceAcknowledgement, (int) msg.GetMessageType()) << "GetMessageType()";
			ASSERT_EQ(string("Body"), msg.GetMessageBody()) << "GetMessageBody()";
		}
}
