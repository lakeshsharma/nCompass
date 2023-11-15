#include "stdafx.h"
#include "Stomp/StompHelper.h"
#include "TestUtil.h"

#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;

class StompHelperTests : public ::testing::Test
{
	public:
        virtual void SetUp()
        {

        }

        virtual void TearDown()
        {
        }
};

TEST(StompHelperTests, StompHelper_GetVersionStr)
{
	const char*
	pStr = GetVersionStr(Stomp::V1_0);
	ASSERT_STREQ("1.0", pStr);

	pStr = GetVersionStr(Stomp::V1_1);
	ASSERT_STREQ("1.1", pStr);

	pStr = GetVersionStr(Stomp::V1_2);
	ASSERT_STREQ("1.2", pStr);

	pStr = GetVersionStr(Stomp::Invalid_Version);
	ASSERT_STREQ("", pStr);

	pStr = GetVersionStr((Stomp::Version) -1);
	ASSERT_STREQ("", pStr);

	pStr = GetVersionStr((Stomp::Version) INT_MAX);
	ASSERT_STREQ("", pStr);
}

TEST(StompHelperTests, StompHelper_GetVersionStrW)
{
	const wchar_t*
	pStr = GetVersionStrW(Stomp::V1_0);
	ASSERT_STREQ(L"1.0", pStr);

	pStr = GetVersionStrW(Stomp::V1_1);
	ASSERT_STREQ(L"1.1", pStr);

	pStr = GetVersionStrW(Stomp::V1_2);
	ASSERT_STREQ(L"1.2", pStr);

	pStr = GetVersionStrW(Stomp::Invalid_Version);
	ASSERT_STREQ(L"", pStr);

	pStr = GetVersionStrW((Stomp::Version) -1);
	ASSERT_STREQ(L"", pStr);

	pStr = GetVersionStrW((Stomp::Version) INT_MAX);
	ASSERT_STREQ(L"", pStr);
}

TEST(StompHelperTests, StompHelper_GetVersion)
{
	Stomp::Version
	version = Stomp::GetVersion("1.0");
	ASSERT_EQ((size_t) Stomp::V1_0, (size_t) version);

	version = Stomp::GetVersion("1.1");
	ASSERT_EQ((size_t) Stomp::V1_1, (size_t) version);

	version = Stomp::GetVersion("1.2");
	ASSERT_EQ((size_t) Stomp::V1_2, (size_t) version);

	version = Stomp::GetVersion("1.3");
	ASSERT_EQ((size_t) Stomp::V1_0, (size_t) version)<< "1.3";

	version = Stomp::GetVersion("1.");
	ASSERT_EQ((size_t) Stomp::V1_0, (size_t) version)<< "1.";

	version = Stomp::GetVersion("1.0,1.1,1.2");
	ASSERT_EQ((size_t) Stomp::V1_2, (size_t) version)<< "1.0,1.1,1.2";

	version = Stomp::GetVersion("1.2,1.1");
	ASSERT_EQ((size_t) Stomp::V1_2, (size_t) version)<< "1.2,1.1";

	version = Stomp::GetVersion("1.1,1");
	ASSERT_EQ((size_t) Stomp::V1_1, (size_t) version)<< "1.1,1";

	version = Stomp::GetVersion(" 1.1");
	ASSERT_EQ((size_t) Stomp::V1_1, (size_t) version)<< " 1.1";

	version = Stomp::GetVersion(nullptr);
	ASSERT_EQ((size_t) Stomp::V1_0, (size_t) version)<<"nullptr";

	version = Stomp::GetVersion("");
	ASSERT_EQ((size_t) Stomp::V1_0, (size_t) version)<<"";
}

TEST(StompHelperTests, StompHelper_RecvCommand_Str)
{
	const char*
	pStr = Stomp::RecvCommand::emap.At(Stomp::RecvCommand::Invalid);
	ASSERT_STREQ("", pStr);

	pStr = Stomp::RecvCommand::emap.At(Stomp::RecvCommand::Connected);
	ASSERT_STREQ("CONNECTED", pStr);

	pStr = Stomp::RecvCommand::emap.At(Stomp::RecvCommand::Message);
	ASSERT_STREQ("MESSAGE", pStr);

	pStr = Stomp::RecvCommand::emap.At(Stomp::RecvCommand::Receipt);
	ASSERT_STREQ("RECEIPT", pStr);

	pStr = Stomp::RecvCommand::emap.At(Stomp::RecvCommand::Error);
	ASSERT_STREQ("ERROR", pStr);

	pStr = Stomp::RecvCommand::emap.At(Stomp::RecvCommand::Invalid);
	ASSERT_STREQ("", pStr);

	pStr = Stomp::RecvCommand::emap.At((Stomp::RecvCommand::Type) -1);
	ASSERT_STREQ("", pStr);

	pStr = Stomp::RecvCommand::emap.At((Stomp::RecvCommand::Type) INT_MAX);
	ASSERT_STREQ("", pStr);
}

TEST(StompHelperTests, StompHelper_RecvCommand_Enum)
{
	Stomp::RecvCommand::Type
	command = Stomp::RecvCommand::emap.At("CONNECTED");
	ASSERT_EQ((size_t) Stomp::RecvCommand::Connected, (size_t) command);

	command = Stomp::RecvCommand::emap.At("CONNECTEd");
	ASSERT_EQ((size_t) Stomp::RecvCommand::Invalid, (size_t) command)<<L"CONNECTEd";

	command = Stomp::RecvCommand::emap.At("C");
	ASSERT_EQ((size_t) Stomp::RecvCommand::Invalid, (size_t) command)<<L"C";

	command = Stomp::RecvCommand::emap.At("CONNECT");
	ASSERT_EQ((size_t) Stomp::RecvCommand::Invalid, (size_t) command)<<L"CONNECT";

	command = Stomp::RecvCommand::emap.At("RECEIPT");
	ASSERT_EQ((size_t) Stomp::RecvCommand::Receipt, (size_t) command);

	command = Stomp::RecvCommand::emap.At("MESSAGE");
	ASSERT_EQ((size_t) Stomp::RecvCommand::Message, (size_t) command);

	command = Stomp::RecvCommand::emap.At("ERROR");
	ASSERT_EQ((size_t) Stomp::RecvCommand::Error, (size_t) command);

	command = Stomp::RecvCommand::emap.At(nullptr);
	ASSERT_EQ((size_t) Stomp::RecvCommand::Invalid, (size_t) command)<<L"nullptr";

	command = Stomp::RecvCommand::emap.At("");
	ASSERT_EQ((size_t) Stomp::RecvCommand::Invalid, (size_t) command)<<L"\"\"";
}

TEST(StompHelperTests, StompHelper_SendCommand_At)
{
	struct
	{
		int line;
		Stomp::SendCommand::Type command;
		const char* pExpectedResult;
	}
	static data[] =
	{
		{ __LINE__, Stomp::SendCommand::Connect,	 "CONNECT"		},
		{ __LINE__, Stomp::SendCommand::Disconnect,  "DISCONNECT"	},
		{ __LINE__, Stomp::SendCommand::Subscribe,	 "SUBSCRIBE"	},
		{ __LINE__, Stomp::SendCommand::Unsubscribe, "UNSUBSCRIBE"	},
		{ __LINE__, Stomp::SendCommand::Send,		 "SEND"			},
		{ __LINE__, Stomp::SendCommand::Ack,		 "ACK"			},
		{ __LINE__, (Stomp::SendCommand::Type) -1,	 nullptr		}
	};

	for (size_t i = 0; i < _countof(data); i++)
	{
		const char* pResult = Stomp::SendCommand::At(data[i].command, false);
		ASSERT_STREQ(data[i].pExpectedResult, pResult)<<AssertMsg::Format(data[i].line, nullptr);
	}
}

TEST(StompHelperTests, StompHelper_SendCommand_AtEx)
{
	try
	{
		Stomp::SendCommand::AtEx((Stomp::SendCommand::Type) - 1);
		//Assert::Fail(L"Expected Exception");
	}
	catch (Stomp::StompException& e)
	{
		ASSERT_EQ((int) Stomp::InvalidCommand, (int) e.code());
	}
}

	
