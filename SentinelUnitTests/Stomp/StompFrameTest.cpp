#include "stdafx.h"
#include "Stomp/StompFrame.h"
#include "TestUtil.h"

#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;
using namespace Stomp;



namespace Stomp
{

class StompFrameTests: public ::testing::Test
{
	public:
                                                                                                   

        virtual void SetUp()
        {

        }

        virtual void TearDown()
        {
        }

        void StompFrameAddGetBodyAsString()
        {
            Frame frame;                                                                                                  
            bool result = frame.Add('1');                                 
            ASSERT_FALSE(result) << L"frame.Add('1')";                    
                                                                                      
            result = frame.Add('2');                                      
            ASSERT_FALSE(result) << L"frame.Add('2')";                    
                                                                                      
            result = frame.Add(0);                                        
            ASSERT_TRUE(result) << L"frame.Add(0)";                       
                                                                                      
                                                                                      
            frame.m_pBody = &frame.m_frame[0];                            
            frame.m_bodyLen = 2;                                          
                                                                                      
                                                                                      
            string str = frame.GetBodyAsString();                         
            ASSERT_STREQ("12", str.c_str()) << L"frame.GetBodyAsString()";
        }

        struct Stomp_FrameData 
        {
            void Run(Stomp::Frame& frame, int test)
            {
                size_t len = strlen(pFrame) + 1;
                char* pData = frame.GetBuffer(len);
                //strncpy_s(pData, len, pFrame, _TRUNCATE);
                strncpy(pData, pFrame, len);

                Stomp::Err result = frame.Parse(version);
                ASSERT_EQ((int) expectedResult, (int) result)<< AssertMsg::Format(line, L"Parse Result", L": %s", GetVersionStr(version));

                Stomp::RecvCommand::Type command = frame.GetCommand();
                ASSERT_EQ((int) expectedCommand, (int) command) << AssertMsg::Format(line, L"GetCommand", L": %s", GetVersionStr(version));

                size_t numHeaders = frame.GetNumHeaders();
                ASSERT_EQ(expectedNumHeaders, numHeaders) << AssertMsg::Format(line, L"NumHeaders", L": %s", GetVersionStr(version));

                for (size_t i = 0; i < numHeaders; i++)
                {
                    const char* pValue = frame.GetHeaderValue(headers[i].pKey);
                    //Assert::IsNotNull(pValue, AssertMsg::Format(line, L"GetHeaderValue (IsNotNull)", L": %s", GetVersionStr(version)));
                    ASSERT_STRNE(pValue, (const char*)NULL) << AssertMsg::Format(line, L"GetHeaderValue (IsNotNull)", L": %s", GetVersionStr(version));
                    ASSERT_STREQ(headers[i].pVal, pValue) << AssertMsg::Format(line, L"GetHeaderValue", L": %s", GetVersionStr(version));
                }

                size_t bodyLen = frame.GetBodyLen();
                ASSERT_EQ(expectedBodyLen, bodyLen) << AssertMsg::Format(line, L"BodyLength", L": %s", GetVersionStr(version));
                ASSERT_EQ(memcmp(pExpectedBody, frame.GetBody(), expectedBodyLen), 0) << AssertMsg::Format(line, L"GetBody (memcmp)", L": %s", GetVersionStr(version));
            }

            Stomp::Version	version;
            Stomp::Err		expectedResult;
            Stomp::RecvCommand::Type expectedCommand;
            int				line;
            const char*		pFrame;
            size_t			expectedNumHeaders;
            struct
            {
                const char* pKey;
                const char* pVal;
           }	headers[5];
           size_t			expectedBodyLen;
           const char*		pExpectedBody;
        };

};

}

TEST_F(StompFrameTests, StompFrame_ctor) 
{
    Stomp::Frame frame;

    Stomp::RecvCommand::Type command = frame.GetCommand();
    ASSERT_EQ((int)Stomp::RecvCommand::Invalid, (int)command) << L"GetCommand";

    size_t numHeaders = frame.GetNumHeaders();
    ASSERT_EQ((size_t)0, numHeaders) << "GetNumHeaders";

    const char *pBody = frame.GetBody();
    //Assert::IsNull(pBody, L"GetBody");
    ASSERT_STREQ(pBody, (const char *)NULL);

    size_t bodyLen = frame.GetBodyLen();
    ASSERT_EQ((size_t)0, bodyLen) << "GetBodyLen";

    const char *pHeaderVal = frame.GetHeaderValue("abc");
    //Assert::IsNull(pHeaderVal, L"GetHeaderValue('abc', nullptr)");
    ASSERT_STREQ(pHeaderVal, (const char *)NULL) << "GetHeaderValue('abc', nullptr)";

    const char *pDefaultStr = "123";
    pHeaderVal = frame.GetHeaderValue("abc", pDefaultStr);
    ASSERT_EQ(pDefaultStr, pHeaderVal) << "GetHeaderValue('abc', '123')";
}


TEST_F(StompFrameTests, StompFrame_Parse_connected_V1_0) 
{
    static Stomp_FrameData testData[] =
	{
				{
					Stomp::V1_0, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\n\n",
					0, {},
					0, ""
				},
				{
					Stomp::V1_0, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\n version : 1.0 \n\n",
					1, { { " version", "1.0 " } },
					0, ""
				},
				{
					Stomp::V1_0,Stomp:: InvalidHeaderKey, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\n:\n\n",
					0, {},
					0, ""
				},
				{
					Stomp::V1_0, Stomp::InvalidHeaderKey, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\n :\n\n",
					0, {},
					0, ""
				},
				{
					Stomp::V1_0, Stomp::InvalidHeaderKey, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\n: \n\n",
					0, {},
					0, ""
				},
				{
					Stomp::V1_0, Stomp::InvalidHeaderKey, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\n : \n\n",
					0, {},
					0, ""
				},
				{
					Stomp::V1_0, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nversion:1.0\n\n",
					1, { { "version", "1.0" } },
					0, ""
				},
				{
					Stomp::V1_0, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nversion: 1.0\n\n",
					1, { { "version", "1.0" } },
					0, ""
				},
				{
					Stomp::V1_0, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\n version : 1.0 \n\n",
					1, { { " version", "1.0 " } },
					0, ""
				},
				{
					Stomp::V1_0, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nversion:1.0\nversion: 1.9\n\n",
					1, { { "version", "1.0" } },
					0, ""
				},
				{
					Stomp::V1_0, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nversion:1.0\nversion: 1.9\n\n",
					1, { { "version", "1.0" } },
					0, ""
				},
				{
					Stomp::V1_0, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nE:\n\n",
					1, { { "E", "" } },
					0, ""
				},
				{
					Stomp::V1_0, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nF: \n\n",
					1, { { "F", "" } },
					0, ""
				},
				{
					Stomp::V1_0, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nversion: 1.0\nserver: Apache/1.3.9\nsession : 5\nE:\nF: \n\n",
					5, { { "version", "1.0" }, { "server", "Apache/1.3.9" }, { "session", "5" }, { "E", "" }, { "F", "" } },
					0, ""
				}
			};

			Stomp::Frame frame;

			for (int test = 0; test < _countof(testData); test++)
			{
				testData[test].Run(frame, test);
			}
}

TEST_F(StompFrameTests, StompFrame_Parse_connected_V1_2) 
{
    static Stomp_FrameData testData[] =
	{
				{
					Stomp::V1_2, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\n\n",
					0, {},
					0, ""
				},
				{
					Stomp::V1_2, Stomp::InvalidHeaderKey, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\n:\n\n",
					0, {},
					0, ""
				},
				{
					Stomp::V1_2, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\n :\n\n",
					1, { " ", "" },
					0, ""
				},
				{
					Stomp::V1_2, Stomp::InvalidHeaderKey, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\n: \n\n",
					0, {},
					0, ""
				},
				{
					Stomp::V1_2, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\n : \n\n",
					1, { " ", " " },
					0, ""
				},
				{
					Stomp::V1_2, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nversion:1.0\n\n",
					1, { { "version", "1.0" } },
					0, ""
				},
				{
					Stomp::V1_2, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nversion: 1.0\n\n",
					1, { { "version", " 1.0" } },
					0, ""
				},
				{
					Stomp::V1_2, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\n version : 1.0 \n\n",
					1, { { " version ", " 1.0 " } },
					0, ""
				},
				{
					Stomp::V1_2, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nversion:1.0\n\n",
					1, { { "version", "1.0" } },
					0, ""
				},
				{
					Stomp::V1_2, Stomp::InvalidHeaderKey, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nversion\n\nBody",
					0, {},
					0, ""
				},
				{
					Stomp::V1_2, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nversion:1.0\nversion: 1.9\n\n",
					1, { { "version", "1.0" } },
					0, ""
				},
				{
					Stomp::V1_2, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nE:\n\n",
					1, { { "E", "" } },
					0, ""
				},
				{
					Stomp::V1_2, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nF: \n\n",
					1, { { "F", " " } },
					0, ""
				},
				{
					Stomp::V1_2, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nsession:4\n\n",
					1, { { "session", "4" } },
					0, ""
				},
				{
					Stomp::V1_2, Stomp::Success, Stomp::RecvCommand::Connected, __LINE__,
					"CONNECTED\nversion: 1.0\r\nserver: Apache/1.3.9\nsession : 5\nE:\nF: \n\r\n",
					5, { { "version", " 1.0" }, { "server", " Apache/1.3.9" }, { "session ", " 5" }, { "E", "" }, { "F", " " } },
					0, ""
				}
			};

			Stomp::Frame frame;

			for (int test = 0; test < _countof(testData); test++)
			{
				testData[test].Run(frame, test);
			}
}

TEST_F(StompFrameTests, StompFrame_Parse_body) {
    static Stomp_FrameData testData[] =
    {
        {
            Stomp::V1_0, Stomp::Success, Stomp::RecvCommand::Message, __LINE__,
            "MESSAGE\ncontent-length:4\n\nBody",
            1, { { "content-length", "4" } },
            4, "Body"
        },
        {
            Stomp::V1_0, Stomp::Success, Stomp::RecvCommand::Message, __LINE__,
            "MESSAGE\n\nBody",
            0, { },
            4, "Body"
        },
        {
            Stomp::V1_1, Stomp::Success, Stomp::RecvCommand::Message, __LINE__,
            "MESSAGE\n\nBody",
            0, { },
            4, "Body"
        },
        {
            Stomp::V1_2, Stomp::Success, Stomp::RecvCommand::Message, __LINE__,
            "MESSAGE\n\nBody",
            0, { },
            4, "Body"
        },
        {
            Stomp::V1_0, Stomp::Success, Stomp::RecvCommand::Message, __LINE__,
            "MESSAGE\ncontent-length:4\n\nBody",
            1, { { "content-length", "4" } },
            4, "Body"
        },
        {
            Stomp::V1_1, Stomp::Success, Stomp::RecvCommand::Message, __LINE__,
            "MESSAGE\ncontent-length:4\n\nBody",
            1, { { "content-length", "4" } },
            4, "Body"
        },
        {
            Stomp::V1_2, Stomp::Success, Stomp::RecvCommand::Message, __LINE__,
            "MESSAGE\ncontent-length:4\n\nBody",
            1, { { "content-length", "4" } },
            4, "Body"
        }
    };

    Stomp::Frame frame;
    for (int test = 0; test < _countof(testData); test++) {
        testData[test].Run(frame, test);
    }
}

TEST_F(StompFrameTests, StompFrame_Parse_complexbody) {
    // Intentionally not const or static
    const char frameData[] = { "MESSAGE\ncontent-type:bin\ncontent-length:8\nval:-1\nnoval:\n\nBody""\0""Two" };
    size_t frameDataLen = _countof(frameData);
    Stomp::Frame frame;
    memcpy(frame.GetBuffer(frameDataLen), frameData, frameDataLen);

    Stomp::Err err = frame.Parse();

    ASSERT_EQ((int)Stomp::Success, (int)err) <<  L"Return";
    ASSERT_EQ((int)Stomp::RecvCommand::Message, (int)frame.GetCommand()) << L"GetCommand()";
    ASSERT_EQ((int)4, (int)frame.GetNumHeaders()) <<  L"GetNumHeaders()";

    const char *pVal = frame.GetHeaderValue("content-type");
    ASSERT_STREQ(pVal, "bin") << L"GetHeaderValue(content-type)";
    ASSERT_EQ((int)4, (int)frame.GetNumHeaders()) << L"GetNumHeaders()";

    pVal = frame.GetHeaderValue("content-length");
    ASSERT_STREQ(pVal, "8") << L"GetHeaderValue(content-length)";

    ASSERT_EQ((size_t)8, frame.GetBodyLen()) << L"GetBodyLen";

    const char *pBody = frame.GetBody();
    ASSERT_EQ(memcmp("Body""\0""Two", pBody, 8), 0) <<  L"GetBody()";
}

TEST_F(StompFrameTests, StompFrame_empty) 
{
    const char frameData[] = { "MESSAGE\ncontent-type:bin\ncontent-length:8\n\nBody""\0""Two" };
    size_t frameDataLen = _countof(frameData);
    Stomp::Frame frame;
    memcpy(frame.GetBuffer(frameDataLen), frameData, frameDataLen);

    Stomp::Err err = frame.Parse();

    ASSERT_EQ((int)Stomp::Success, (int)err) << L"Return";

    frame.Empty();
    ASSERT_EQ((int)Stomp::RecvCommand::Invalid, (int)frame.GetCommand()) <<  L"GetCommand";

    ASSERT_EQ((size_t)0, frame.GetBufferLen()) << L"GetBufferLen";
    ASSERT_EQ((size_t)0, frame.GetBodyLen()) << L"GetBodyLen";
    ASSERT_EQ((size_t)0, frame.GetNumHeaders()) << L"GetNumHeaders";

    ASSERT_STREQ(frame.GetBody(), (const char *)NULL) << L"GetBody";
}

TEST_F(StompFrameTests, StompFrame_GetBuffer_big) {
    Stomp::Frame frame;
    char *pBuffer = frame.GetBuffer(Stomp::MaxFrameLen + 1);
    ASSERT_STREQ(pBuffer, (const char *)NULL) << L"GetBuffer";

    size_t len = frame.GetBufferLen();
    ASSERT_EQ((size_t)0, len) << L"GetBufferLen";

    size_t capacity = frame.GetBufferCapacity();
    ASSERT_EQ((size_t)0, capacity) << L"GetBufferCapacity";
    Stomp::Err err = frame.ReleaseBuffer(1);
    ASSERT_EQ((int)Stomp::InvalidLength, (int)err) << L"ReleaseBuffer";
}

TEST_F(StompFrameTests, StompFrame_GetBuffer_small) 
{
    Stomp::Frame frame;
    char *pBuffer = frame.GetBuffer(0);
    ASSERT_STREQ(pBuffer, (const char *)NULL) << L"GetBuffer";

    size_t len = frame.GetBufferLen();
    ASSERT_EQ((size_t)0, len) << L"GetBufferLen";

    size_t capacity = frame.GetBufferCapacity();
    ASSERT_EQ((size_t)0, capacity) << L"GetBufferCapacity";

    Stomp::Err err = frame.ReleaseBuffer(1);
    ASSERT_EQ((int)Stomp::InvalidLength, (int)err) << L"ReleaseBuffer";
}

TEST_F(StompFrameTests, StompFrame_ReleaseBuffer) 
{
    Stomp::Frame frame;
    char *pBuffer = frame.GetBuffer(16);
    ASSERT_STRNE(pBuffer, (const char *)NULL) << L"GetBuffer";

    size_t len = frame.GetBufferLen();
    ASSERT_EQ((size_t)16, len) << L"GetBufferLen";

    Stomp::Err err = frame.ReleaseBuffer(0);
    ASSERT_EQ((int)Stomp::ZeroLength, (int)err) << L"ReleaseBuffer(0)";

    err = frame.ReleaseBuffer(32);
    ASSERT_EQ((int)Stomp::InvalidLength, (int)err) << L"ReleaseBuffer(32)";

    err = frame.ReleaseBuffer(8);
    ASSERT_EQ((int)Stomp::InvalidFrame, (int)err) << L"ReleaseBuffer(8)";

    //strncpy_s(pBuffer, len, "1234567", _TRUNCATE);
    strncpy(pBuffer, "1234567", len);

    err = frame.ReleaseBuffer(8);
    ASSERT_EQ((int)Stomp::Success, (int)err) << L"ReleaseBuffer(8)";
    len = frame.GetBufferLen();
    ASSERT_EQ((size_t)8, len) << L"GetBufferLen";
}

TEST_F(StompFrameTests, StompFrame_Parse_fail) {
    Stomp::Frame frame;
    Stomp::Err err = frame.Parse();
    ASSERT_EQ((int)Stomp::ZeroLength, (int)err) << L"Parse with nothing";
}


TEST_F(StompFrameTests, StompFrame_GetBodyAsString_empty) {
    Stomp::Frame frame;
    string result = frame.GetBodyAsString();
    ASSERT_STREQ("", result.c_str());
}

TEST_F(StompFrameTests, StompFrame_Add_GetBodyAsString)
{
    StompFrameAddGetBodyAsString();
}

TEST_F(StompFrameTests, StompFrame_SetHeaderValue)
{
	Stomp::Frame frame;
	Stomp::Err err = frame.SetHeaderValue("type", "T", true);
	ASSERT_EQ((int) Stomp::Success, (int) err) << L"SetHeaderValue('type')";

	err = frame.SetHeaderValue("content-length", "5", true);
	ASSERT_EQ((int) Stomp::Success, (int) err) << L"SetHeaderValue('5')";

	const char* pVal = frame.GetHeaderValue("type");
	ASSERT_STREQ("T", pVal) << L"GetHeaderValue('type')";

	pVal = frame.GetHeaderValue("content-length");
	ASSERT_EQ("5", pVal) << L"GetHeaderValue('content-len')";

	size_t val = frame.GetContentLen();
	ASSERT_EQ((unsigned) 5, val) << L"GetContentLen";
}

TEST_F(StompFrameTests, StompFrame_SetHeaderValue_contentlen)
{
	struct
	{
		int line;
		const char* pStr;
		Stomp::Err expectedResult;
		size_t expectedLen;
	}
	static data[] = 
	{
		{ __LINE__, "0",		  Stomp::Success, 0    },
		{ __LINE__, "10",		  Stomp::Success, 10   },
		{ __LINE__, "8000",		  Stomp::Success, 8000 },
		{ __LINE__, "2147483647", Stomp::Invalid_content_length, 0 },
		{ __LINE__, "-5",		  Stomp::Invalid_content_length, 0 },
	};

	for (size_t i = 0; i < _countof(data); i++)
	{
		Stomp::Frame frame;
		Stomp::Err err = frame.SetHeaderValue("content-length", data[i].pStr, false);
		ASSERT_EQ((int) data[i].expectedResult, (int) err) << AssertMsg::Format(data[i].line, L"SetHeaderValue", L"%S", data[i].pStr);

		size_t len = frame.GetContentLen();
        ASSERT_EQ(data[i].expectedLen, len) <<  AssertMsg::Format(data[i].line, L"GetContentLen", L"%S", data[i].pStr);
	}
}


