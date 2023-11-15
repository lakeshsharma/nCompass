#include "stdafx.h"

#include "PollerProtocol/PollerPBPinResetResponseMessage.h"

// Unit testing includes.
#include "gtest/gtest.h"

namespace SentinelNativeUnitTests
{
	TEST(PollerPBPinResetResponseMessage, PollerPBPinResetResponseMessagePinSuccess)
	{
		//Data
		const byte data[] = {0x00};

		//SETUP
		CPollerPBPinResetResponseMessage pollerMessage(data);

		//TEST
		ASSERT_TRUE(PinResetResult::PinResetSuccess == pollerMessage.GetPinResetResult());
	}
	
	TEST(PollerPBPinResetResponseMessage, PollerPBPinResetResponseMessagePinFailures)
	{
		//Data
		const byte data[] = {0x01};

		//SETUP
		CPollerPBPinResetResponseMessage pollerMessage(data);

		//TEST
		ASSERT_TRUE(PinResetResult::PinResetErrorSequence == pollerMessage.GetPinResetResult());
	}
}
