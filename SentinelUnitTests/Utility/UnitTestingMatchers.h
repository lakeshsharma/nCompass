#pragma once
// Include "gmock.h" before including this file.

using ::testing::Args;

// Put custom gmock matchers in a header file to avoid linking inconsistent definitions.
namespace SentinelNativeUnitTests
{
	// It would be better to use a multi-argument matcher and check the length before comparing buffer contents.
	MATCHER_P2(CheckBuffer, expectedBuffer, expectedSize, "")
	{
		bool matches(0 == memcmp(arg, expectedBuffer, expectedSize));
		return matches;
	}

	MATCHER_P2(CheckBufferAndSize, buffer, bufferSize, "")
	{
		return std::get<1>(arg) == bufferSize && memcmp(std::get<0>(arg), buffer, bufferSize) == 0;
	}

	MATCHER_P2(CheckMessage, messageTypeExpected, dwordValueExpected, "")
	{
		bool matches(messageTypeExpected == arg->GetMessageType() && dwordValueExpected == arg->GetDWordData());
		return matches;
	}

	MATCHER_P2(CheckPollValue, first, second, "")
	{
		return arg[0] == first && arg[1] == second;
	}

	// Simple CSASPollMessage matcher that only requires poll value for a match.
	MATCHER_P(MatchSASPollMessage, pollValue, "") {
		return arg->GetPollValue() == pollValue;
	}

	// Return true if the address of the reference argument matches the expected address, return false otherwise.
	MATCHER_P(MatchRef, expectedAddress, "")
	{
		bool matches(&arg == expectedAddress);
		return matches;
	}
}
