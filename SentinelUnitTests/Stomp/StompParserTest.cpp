#include "stdafx.h"
#include "Stomp/StompParser.h"
#include "TestUtil.h"
#include "Stomp/StompFrame.h"
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;
using namespace Stomp;


namespace Stomp{

    class StompParserTests : public ::testing::Test, private Parser
    {
    	public:
            virtual void SetUp()
            {

            }

            virtual void TearDown()
            {
            }
            static RecvCommand::Type DetermineCommands(char*& pStr, Version version)
            {
                return Parser::DetermineCommand(pStr, version);
            }
            static int GetEolLength(const char* pStr, Version version)
            {
                return Parser::GetEolLen(pStr, version);
            }

            void StompParser_ParseHeader_cstor_V1_0()
            {
                 Parser::ParseHeader parseHeader(V1_0);

                 ASSERT_EQ((int) V1_0, (int) parseHeader.version) << L"version";
            }

            void StompParser_ParseHeader_cstor_V1_1()
            {
                 Parser::ParseHeader parseHeader(V1_1);

                 ASSERT_EQ((int) V1_1, (int) parseHeader.version) << L"version";
            }

            void StompParser_ParseHeader_cstor_V1_2()
            {
                 Parser::ParseHeader parseHeader(V1_2);

                 ASSERT_EQ((int) V1_2, (int) parseHeader.version) << L"version";
            }

            void Stomp_ParserResetTest()
            {
                	Parser::ParseHeader parseHeader(V1_0);

                	char frame[32];
                	//strcpy_s(frame, "h1");
                    strcpy(frame, "h1");
                	parseHeader.Reset(frame);

                	ASSERT_STREQ(frame, parseHeader.pKey) << L"pKey";
                	ASSERT_EQ((size_t) 0, parseHeader.keyLen) << L"keyLen";

                	ASSERT_STREQ("", parseHeader.pVal) << L"pVal";
                	ASSERT_EQ((size_t) 0, parseHeader.valLen ) << L"valLen";

                	ASSERT_EQ((int) V1_0, (int) parseHeader.version) << L"version";
            }
            void StompParser_ParseHeaderConnected_Parse_V1_0A()
            {
                char frame[32];
            	//strcpy_s(frame, "h1:v01\n\n");
                strcpy(frame, "h1:v01\n\n");

            	Parser::ParseHeaderConnected parseHeader(V1_0);

            	char* pFrame = frame;
            	Err result = parseHeader.Parse(pFrame);
            	ASSERT_EQ((int) Success, (int) result) << L"Return";

            	ASSERT_EQ(memcmp(frame, "h1\0v01\0\n", 8), 0) << L"Frame not modified correctly";

            	ASSERT_STREQ("h1", parseHeader.pKey) << L"pKey";
            	ASSERT_EQ((size_t) 2, parseHeader.keyLen) << L"keyLen";

            	ASSERT_STREQ("v01", parseHeader.pVal) << L"pVal";
            	ASSERT_EQ((size_t) 3, parseHeader.valLen) << L"valLen";
            }
            void StompParser_ParseHeaderConnected_Parse_V1_0B()
            {
            	char frame[32];
            	//strcpy_s(frame, "h1 : v01\n\n");
            	strcpy(frame, "h1 : v01\n\n");

            	Parser::ParseHeaderConnected parseHeader(V1_0);

            	char* pFrame = frame;
            	Err result = parseHeader.Parse(pFrame);
            	ASSERT_EQ((int) Success, (int) result) << L"Return";

            	ASSERT_EQ(memcmp(frame, "h1\0: v01\0\n", 10), 0) << L"Frame not modified correctly";

            	ASSERT_STREQ("h1", parseHeader.pKey) << L"pKey";
            	ASSERT_EQ((size_t) 2, parseHeader.keyLen) << L"keyLen";

            	ASSERT_STREQ("v01", parseHeader.pVal) << L"pVal";
            	ASSERT_EQ((size_t) 3, parseHeader.valLen ) << L"valLen";
            }
            void StompParser_ParseHeaderConnected_Parse_V1_0C()
            {
                char frame[32];
            	//strcpy_s(frame, "h\\:\\v01\r\n\r\n");
                strcpy(frame, "h\\:\\v01\r\n\r\n");

            	Parser::ParseHeaderConnected parseHeader(V1_0);

            	char* pFrame = frame;
            	Err result = parseHeader.Parse(pFrame);
            	ASSERT_EQ((int) Success, (int) result) << L"Return";

            	ASSERT_EQ(memcmp(frame, "h\\\0\\v01\0\n", 9), 0) << L"Frame not modified correctly";

            	ASSERT_STREQ("h\\", parseHeader.pKey) << L"pKey";
            	ASSERT_EQ((size_t) 2, parseHeader.keyLen) << L"keyLen";

            	ASSERT_STREQ("\\v01", parseHeader.pVal) << L"pVal";
            	ASSERT_EQ((size_t) 4, parseHeader.valLen) << L"valLen";
            }
            void StompParser_ParseHeaderConnected_Parse_V1_2C()
            {
            	char frame[32];
            	//strcpy_s(frame, "h\\:\\v01\r\n\r\n");
                strcpy(frame, "h\\:\\v01\r\n\r\n");
            	Parser::ParseHeaderConnected parseHeader(V1_2);

            	char* pFrame = frame;
            	Err result = parseHeader.Parse(pFrame);
            	ASSERT_EQ((int) Success, (int) result) << L"Return";

            	ASSERT_EQ(memcmp(frame, "h\\\0\\v01\0\n", 9), 0) << L"Frame not modified correctly";

            	ASSERT_STREQ("h\\", parseHeader.pKey) << L"pKey";
            	ASSERT_EQ((size_t) 2, parseHeader.keyLen) << L"keyLen";

            	ASSERT_STREQ("\\v01", parseHeader.pVal) << L"pVal";
            	ASSERT_EQ((size_t) 4, parseHeader.valLen) << L"valLen";
            }
            void ParserHeader_Parse()
            {

                struct
                {
                    Version	version;
                    char*			pFrame;
                    const char*		pKey;
                    const char*		pVal;
                    Err		rval;
                    int				line;
                }
                static data[] =
                {
                    { V1_0, (char*)"key:value\n",    	 	 "key",   "value", 		Success,				__LINE__ },
                    { V1_0, (char*)"key : value\n",  	 	 "key",   "value", 		Success,				__LINE__ },
                    { V1_0, (char*)"key\t:\tvalue\n",	 	 "key",   "value", 		Success,				__LINE__ },
                    { V1_0, (char*)"key: \n",        	 	 "key",   "",      		Success,				__LINE__ },
                    { V1_0, (char*)" key : value \n",	 	 " key",  "value ",		Success,				__LINE__ },
                    { V1_0, (char*)" key value \n",  	 	 nullptr, nullptr, 		InvalidHeaderKey,	__LINE__ },

                    { V1_1, (char*)"key:value\n",    	 	 "key",   "value", 		Success,				__LINE__ },
                    { V1_1, (char*)"key : value\n",  	 	 "key",   "value", 		Success,				__LINE__ },
                    { V1_1, (char*)"key\t:\tvalue\n",	 	 "key",   "value", 		Success,				__LINE__ },
                    { V1_1, (char*)"key: \n",        	 	 "key",   "",      		Success,				__LINE__ },
                    { V1_1, (char*)" key : value \n",	 	 " key",  "value ",		Success,				__LINE__ },
                    { V1_1, (char*)" key value \n",  	 	 nullptr, nullptr, 		InvalidHeaderKey,	__LINE__ },

                    { V1_2, (char*)"key:value\n",    	 	 "key",   "value",		Success,				__LINE__ },
                    { V1_2, (char*)"key : value\n",  	 	 "key ",  " value",		Success,				__LINE__ },
                    { V1_2, (char*)"key\t:\tvalue\n",	 	 "key\t", "\tvalue",	Success,				__LINE__ },
                    { V1_2, (char*)"key: \n",        	 	 "key",   " ",			Success,				__LINE__ },
                    { V1_2, (char*)" key : value \n",	 	 " key ", " value ",	Success,				__LINE__ },
                    { V1_2, (char*)" key value \n",  	 	 nullptr, nullptr,		InvalidHeaderKey,	__LINE__ },

                    { V1_2, (char*)"key:value\r\n",    		"key",   "value",		Success,				__LINE__ },
                    { V1_2, (char*)"key : value\r\n",  		"key ",  " value",		Success,				__LINE__ },
                    { V1_2, (char*)"key: \r\n",        		"key",   " ",			Success,				__LINE__ },
                    { V1_2, (char*)" key : value \r\n",		" key ", " value ",		Success,				__LINE__ },
                    { V1_2, (char*)" key value \r\n",		 nullptr, nullptr,		InvalidHeaderKey,	__LINE__ },

                    { V1_0, (char*)"key\\c:v\\\\al\\nue\n", "key:",  "v\\al\nue",	Success,				__LINE__ },
                    { V1_1, (char*)"key\\c:v\\\\al\\nue\n", "key:",  "v\\al\nue",	Success,				__LINE__ },
                    { V1_2, (char*)"key\\c:v\\\\al\\nue\n", "key:",  "v\\al\nue",	Success,				__LINE__ },

                    { V1_0, (char*)"key\\c:v\\\\al\\rue\n", nullptr, nullptr,		InvalidHeaderValue,	__LINE__ },
                    { V1_1, (char*)"key\\c:v\\\\al\\rue\n", nullptr, nullptr,		InvalidHeaderValue,	__LINE__ },
                    { V1_2, (char*)"key\\c:v\\\\al\\rue\n", "key:",  "v\\al\rue",	Success,				__LINE__ },

                    { V1_0, (char*)"\\ckey:\\cval\\r\n",	nullptr, nullptr,		InvalidHeaderValue,	__LINE__ },
                    { V1_1, (char*)"\\ckey:\\cval\\r\n",	nullptr, nullptr,		InvalidHeaderValue,	__LINE__ },
                    { V1_2, (char*)"\\ckey:\\cval\\r\n",	":key",  ":val\r",		Success,				__LINE__ },

                    { V1_0, (char*)"key:val\\t\n",			nullptr, nullptr,		InvalidHeaderValue,	__LINE__ },
                    { V1_1, (char*)"key:val\\t\n",			nullptr, nullptr,		InvalidHeaderValue,	__LINE__ },
                    { V1_2, (char*)"key:val\\t\n",			nullptr, nullptr,		InvalidHeaderValue,	__LINE__ }
                };

                for (int test = 0; test < _countof(data); test++)
                {
                    char frame[64];
                    //strcpy_s(frame, data[test].pFrame);
                    strcpy(frame, data[test].pFrame);

                    Parser::ParseHeader parseHeader(data[test].version);

                    char* pFrame = frame;
                    Err result = parseHeader.Parse(pFrame);

                    ASSERT_EQ((int) data[test].rval, (int) result) << AssertMsg::Format(data[test].line, L"Return");
                    if (result == Success)
                    {
                        ASSERT_STREQ(data[test].pKey,			parseHeader.pKey) << /*true AssertMsg::Format(data[test].line,*/ L"pKey";
                        ASSERT_EQ(strlen(data[test].pKey),	parseHeader.keyLen) <<		/*AssertMsg::Format(data[test].line,*/ L"keyLen";
                        ASSERT_STREQ(data[test].pVal,			parseHeader.pVal) << /*true AssertMsg::Format(data[test].line,*/ L"pVal";
                        ASSERT_EQ(strlen(data[test].pVal),	parseHeader.valLen) << /*		AssertMsg::Format(data[test].line,*/ L"valLen";
                    }
                }
            }
            void Parse_BadFrames()
            {
                Parser parser;

            	Err result = parser.Parse(nullptr, 100, V1_0);
            	ASSERT_EQ((int) NullFrame, (int) result) << L"frame.Parse(nullptr, 100)";

            	char data[2] = { 0, 0 };

            	result = parser.Parse(data, 1, V1_0);
            	ASSERT_EQ((int) ZeroLength, (int) result) << L"frame.Parse(data[0]=0, 1)";

            	data[0] = 1;
            	result = parser.Parse(data, 0, V1_0);
            	ASSERT_EQ((int) ZeroLength, (int) result) << L"frame.Parse(data[0]=1, 0)";

            	result = parser.Parse(data, MaxFrameLen + 1, V1_0);
            	ASSERT_EQ((int) InvalidLength, (int) result) << L"frame.Parse(data[0]=1, MaxFrameLen + 1)";

            	data[1] = 1;
            	result = parser.Parse(data, 2, V1_0);
            	ASSERT_EQ((int) InvalidFrame, (int) result) << L"frame.Parse(data[0]=1/data[1]=1, 2)";
            }

            void Parse_BadCommands()
            {
                char frameData[] = { "CONNECT\nkey:val\n\nbody" };
            	size_t frameDataLen = _countof(frameData);

            	Parser parser;

            	Err err = parser.Parse(frameData, frameDataLen, V1_0);
            	ASSERT_EQ((int) InvalidCommand, (int) err);
            }

            void Parse_SimpleFrame()
            {
                struct
            	{
            		Version	version;
            		const char*		pFrame;
            		Err		expectedResult;
            		int				line;
            	}
            	static data[] =
            	{
            		{ V1_0, "CONNECTED",		InvalidCommand,	__LINE__ },
            		{ V1_0, "CONNECTED\n\n", Success,			__LINE__ }
            	};

            	// Intentionally left outside loop to make sure resets are working
            	Parser parser;

            	for (int test = 0; test < _countof(data); test++)
            	{
            		char buffer[256];
            		//strncpy_s(buffer, _countof(buffer), data[test].pFrame, _TRUNCATE);
                    strncpy(buffer, data[test].pFrame,  _countof(buffer));
            		size_t len = strlen(buffer) + 1;

            		Err result = parser.Parse(buffer, len, data[test].version);
            		ASSERT_EQ((int) data[test].expectedResult, (int) result) /*<< AssertMsg::Format(data[test].line, L"Result", L"%s", GetVersionStrW(data[test].version)*/;
            	}
            }
            void Parse_OnCommand_Fail()
            {
                class VirtualTest : public Parser
            	{
            	protected:
            		virtual Err OnCommand(RecvCommand::Type command)							{ return (Err) -1; }
            		//virtual Err OnHeader(char* pKey, size_t keylen, char* pValue, size_t valueLen)	{ return Success; }
            		//virtual Err OnBody(char* pBody, size_t bodyLen)								{ return Success; }

            	public:
            		Err Parse(char* pFrame, size_t frameLen, Version version)
            		{
            			return Parser::Parse(pFrame, frameLen, version);
            		}
            	};

            	char frameData[] = { "ERROR\nkey:val\n\nbody" };
            	size_t frameLen = _countof(frameData);

            	VirtualTest test;

            	int err = (int) test.Parse(frameData, frameLen, V1_0);
            	ASSERT_EQ(-1, err);
            }
            void Parse_OnHeader_fail()
            {
            	class VirtualTest : public Parser
            	{
            	protected:
            		//virtual Err OnCommand(RecvCommand::Type command)						{ return (Err) -1; }
            		virtual Err OnHeader(char* pKey, size_t keylen, char* pValue, size_t valueLen)	{ return (Err) -1; }
            		//virtual Err OnBody(char* pBody, size_t bodyLen)								{ return Success; }

            	public:
            		Err Parse(char* pFrame, size_t frameLen, Version version)
            		{
            			return Parser::Parse(pFrame, frameLen, version);
            		}
            	};

            	char frameData[] = { "ERROR\nkey:val\n\nbody" };
            	size_t frameLen = _countof(frameData);

            	VirtualTest test;

            	int err = (int) test.Parse(frameData, frameLen, V1_0);
            	ASSERT_EQ(-1, err);
            }
            void Parse_OnBody_fail()
            {
                class VirtualTest : public Parser
            	{
            	protected:
            		//virtual Err OnCommand(RecvCommand::Type command)									{ return (Err) -1; }
            		//virtual Err OnHeader(char* pKey, size_t keylen, char* pValue, size_t valueLen)	{ return (Err) -; }
            		virtual Err OnBody(char* pBody, size_t bodyLen)									{ return  (Err) -1; }

            	public:
            		Err Parse(char* pFrame, size_t frameLen, Version version)
            		{
            			return Parser::Parse(pFrame, frameLen, version);
            		}
            	};

            	char frameData[] = { "ERROR\nkey:val\n\nbody" };
            	size_t frameLen = _countof(frameData);

            	VirtualTest test;

            	int err = (int) test.Parse(frameData, frameLen, V1_0);
            	ASSERT_EQ(-1, err);
            }
            void Parse_Coverage()
            {
                class VirtualTest : public Parser
            	{
            	protected:
            		virtual Err OnCommand(RecvCommand::Type command)							{ return Parser::OnCommand(command); }
            		virtual Err OnHeader(char* pKey, size_t keylen, char* pValue, size_t valueLen)	{ return Parser::OnHeader(pKey, keylen, pValue, valueLen); }
            		virtual Err OnBody(char* pBody, size_t bodyLen)									{ return Parser::OnBody(pBody, bodyLen); }

            	public:
            		Err Parse(char* pFrame, size_t frameLen, Version version)
            		{
            			return Parser::Parse(pFrame, frameLen, version);
            		}
            	};

            	char frameData[] = { "ERROR\nkey:val\n\nbody" };
            	size_t frameLen = _countof(frameData);

            	VirtualTest test;

            	int err = (int) test.Parse(frameData, frameLen, V1_0);
            	ASSERT_EQ(0, err);
            }
            void Parse_InvalidCharAfterCommand()
            {
                class VirtualTest : public Parser
            	{
            	protected:
            		virtual Err OnCommand(RecvCommand::Type command)									{ return Parser::OnCommand(command); }

            	public:
            		Err Parse(char* pFrame, size_t frameLen, Version version)
            		{
            			return Parser::Parse(pFrame, frameLen, version);
            		}
            	};

            	char frameData[] = { "ERROR\n""\0""key:val\n\nbody" };
            	size_t frameLen = _countof(frameData);

            	VirtualTest test;

            	Err err = test.Parse(frameData, frameLen, V1_0);
            	ASSERT_EQ((int) InvalidHeader, (int) err);
            }
            void Parse_InvalidCharAfterHeaders()
            {
                class VirtualTest : public Parser
            	{
            	protected:
            		virtual Err OnCommand(RecvCommand::Type command)							{ return Parser::OnCommand(command); }
            		virtual Err OnHeader(char* pKey, size_t keylen, char* pValue, size_t valueLen)	{ return Parser::OnHeader(pKey, keylen, pValue, valueLen); }

            	public:
            		Err Parse(char* pFrame, size_t frameLen, Version version)
            		{
            			return Parser::Parse(pFrame, frameLen, version);
            		}
            	};

            	char frameData[] = { "ERROR\nkey:val\n""\0""\nbody" };
            	size_t frameLen = _countof(frameData);

            	VirtualTest test;

            	Err err = test.Parse(frameData, frameLen, V1_0);
            	ASSERT_EQ((int) InvalidFrame, (int) err);
            }
            void StompParser_Parse_escapechars_V1_0()
            {
            	char frameData[] = { "ERROR\nk\\\\\\r\\n\\cey:val\n\nbody" };
            	size_t frameLen = _countof(frameData);

            	Parser parser;

            	Err err = parser.Parse(frameData, frameLen, V1_0);
            	ASSERT_EQ((int) InvalidHeaderKey, (int) err);
            }

            void StompParser_Parse_badescapechars_V1_0()
            {
            	char frameData[] = { "ERROR\nk\\ty:val\n\nbody" };
            	size_t frameLen = _countof(frameData);

            	Parser parser;

            	Err err = parser.Parse(frameData, frameLen, V1_0);
            	ASSERT_EQ((int) InvalidHeaderKey, (int) err);
            }

            void StompParser_Parse_escapechars_V1_2()
            {
            	char frameData[] = { "ERROR\nk\\\\\\r\\n\\cey:val\n\nbody" };
            	size_t frameLen = _countof(frameData);

            	Parser parser;

            	Err err = parser.Parse(frameData, frameLen, V1_2);
            	ASSERT_EQ((int) Success, (int) err);
            }

            void StompParser_Parse_badreturninheader_V1_2()
            {
            	char frameData[] = { "ERROR\nkey\\c\n""\0""\nbody" };
            	size_t frameLen = _countof(frameData);

            	Parser parser;

            	Err err = parser.Parse(frameData, frameLen, V1_2);
            	ASSERT_EQ((int) InvalidHeaderKey, (int) err);
            }

            void StompParser_Parse_emptykey()
            {
            	char frameData[] = { "ERROR\n:val\n\nbody" };
            	size_t frameLen = _countof(frameData);

            	Parser parser;

            	Err err = parser.Parse(frameData, frameLen, V1_2);
            	ASSERT_EQ((int) InvalidHeaderKey, (int) err);
            }

            void StompParser_Parse_headeronly()
            {
            	char frameData[] = { "ERROR\ncontent-length:4\n\n" };
            	size_t frameLen = _countof(frameData) - 1;

            	Parser parser;

            	Err err = parser.Parse(frameData, frameLen, V1_2, true);
            	ASSERT_EQ((int) Success, (int) err);
            }

           void StompParser_Parse_headeronly2()
            {
            	char frameData[] = { "ERROR\ncontent-length:4\n\n" };
            	size_t frameLen = _countof(frameData) - 1;

            	Parser parser;

            	Err err = parser.Parse(frameData, frameLen, V1_2, true);
            	ASSERT_EQ((int) Success, (int) err);
            }

            void StompParser_Parse_headeronlybad_V1_0()
            {
            	char frameData[] = { "ERROR\ncontent-length:4\n\r\n" };
            	size_t frameLen = _countof(frameData) - 1;

            	Parser parser;

            	Err err = parser.Parse(frameData, frameLen, V1_0, true);
            	ASSERT_EQ((int) InvalidHeader, (int) err);
            }
            void StompParser_Parse_headeronlybad_V1_1()
            {
            	char frameData[] = { "ERROR\ncontent-length:4\n\r" };
            	size_t frameLen = _countof(frameData) - 1;

            	Parser parser;

            	Err err = parser.Parse(frameData, frameLen, V1_1, true);
            	ASSERT_EQ((int) InvalidHeader, (int) err);
            }

            void StompParser_Parse_headeronlybad_V1_2()
            {
            	char frameData[] = { "ERROR\ncontent-length:4\nx" };
            	size_t frameLen = _countof(frameData) - 1;

            	Parser parser;

            	Err err = parser.Parse(frameData, frameLen, V1_2, true);
            	ASSERT_EQ((int) InvalidHeader, (int) err);
            }

            void StompParser_Parse_contentlength()
            {
            	char frameData[] = { "ERROR\ncontent-length:4\n\n" };
            	size_t frameLen = _countof(frameData) - 1;

            	Parser parser;

            	int contentLen;
            	Err err = parser.Parse(frameData, frameLen, V1_2, true, &contentLen);
            	ASSERT_EQ((int) Success, (int) err);
            	ASSERT_EQ((int) 4, (int) contentLen);
            }

            void StompParser_Parse_contentlengthinvalid1()
            {
            	char frameData[] = { "ERROR\ncontent-length:-4\n\n" };
            	size_t frameLen = _countof(frameData) - 1;

            	Parser parser;

            	int contentLen;
            	Err err = parser.Parse(frameData, frameLen, V1_2, true, &contentLen);
            	ASSERT_EQ((int) Success, (int) err);
            	ASSERT_EQ(ContentLengthInvalid, contentLen);
            }
            void StompParser_Parse_contentlengthinvalid2()
            {
            	char frameData[] = { "ERROR\ncontent-length:4i\n\n" };
            	size_t frameLen = _countof(frameData) - 1;

            	Parser parser;

            	int contentLen;
            	Err err = parser.Parse(frameData, frameLen, V1_2, true, &contentLen);
            	ASSERT_EQ((int) Success, (int) err);
            	ASSERT_EQ(ContentLengthInvalid, contentLen);
            }

            void StompParser_Parse_contentlengthlarge()
            {
            	char frameData[] = { "ERROR\ncontent-length:12345678\n\n" };
            	size_t frameLen = _countof(frameData) - 1;

            	Parser parser;

            	int contentLen;
            	Err err = parser.Parse(frameData, frameLen, V1_2, true, &contentLen);
            	ASSERT_EQ((int) Success, (int) err);
            	ASSERT_EQ(ContentLengthInvalid, contentLen);
            }
            void StompParser_Parse_contentlengthmissing()
            {
            	char frameData[] = { "ERROR\n\n\n" };
            	size_t frameLen = _countof(frameData) - 1;

            	Parser parser;

            	int contentLen;
            	Err err = parser.Parse(frameData, frameLen, V1_2, true, &contentLen);
            	ASSERT_EQ((int) Success, (int) err);
            	ASSERT_EQ(ContentLengthMissing, contentLen);
            }
            void StompParser_Parse_headeronly_invalidheaderlen()
            {
            	char frameData[] = { "E\n" };
            	size_t frameLen = _countof(frameData) - 1;

            	Parser parser;

            	int contentLen;
            	Err err = parser.Parse(frameData, frameLen, V1_2, true, &contentLen);
            	ASSERT_EQ((int) InvalidHeader, (int) err);
            }
            void StompParser_VerifyEndOfHeader()
            {
            	struct
            	{
            		int line;
            		const char* pStr;
            		Version version;
            		bool expectedResult;
            	}
            	static data[] =
            	{
            		{ __LINE__, "------\n\n",	V1_0, true  },
            		{ __LINE__, "------\n\n",	V1_1, true  },
            		{ __LINE__, "------\n\n",	V1_2, true  },
            		{ __LINE__, "--\r\n\r\n",	V1_0, false },
            		{ __LINE__, "--\r\n\r\n",	V1_1, false },
            		{ __LINE__, "--\r\n\r\n",	V1_2, true  },
            		{ __LINE__, "----\r\n\n",	V1_0, true  },
            		{ __LINE__, "----\r\n\n",	V1_1, true  },
            		{ __LINE__, "----\r\n\n",	V1_2, true  },
            		{ __LINE__, "----\r\r\n",	V1_0, false },
            		{ __LINE__, "----\r\r\n",	V1_1, false },
            		{ __LINE__, "----\r\r\n",	V1_2, false },
            		{ __LINE__, "----\n\r\n",	V1_0, false },
            		{ __LINE__, "----\n\r\n",	V1_1, false },
            		{ __LINE__, "----\n\r\n",	V1_2, true  }
            	};

            	for (size_t i = 0; i < _countof(data); i++)
            	{
            		const char* pEnd = &data[i].pStr[strlen(data[i].pStr) - 1];
            		bool result = Parser::VerifyEndOfHeader(pEnd, data[i].version);
            		ASSERT_EQ(data[i].expectedResult, result) << AssertMsg::Format(data[i].line, L"ExpectedResult");
            	}
            }

            void StompParser_GetContentLengthVal()
            {
            	struct
            	{
            		int line;
            		const char* pStr;
            		int expectedResult;
            	}
            	static data[] =
            	{
            		{ __LINE__, "0",		0   },
            		{ __LINE__, "4",		4   },
            		{ __LINE__, "123",		123 },

            		{ __LINE__, "",		    ContentLengthInvalid },
            		{ __LINE__, "  ",	    ContentLengthInvalid },
            		{ __LINE__, "-4",		ContentLengthInvalid },
            		{ __LINE__, "4-",		ContentLengthInvalid },
            		{ __LINE__, "12345678",	ContentLengthInvalid },
            	};

            	for (size_t i = 0; i < _countof(data); i++)
            	{
            		int result = Parser::GetContentLengthVal(data[i].pStr);
            		ASSERT_EQ(data[i].expectedResult, result) << AssertMsg::Format(data[i].line, L"ExpectedResult");
            	}
            }

            void StompParser_GetContentLengthValMax()
            {
            	char buffer[16];
            	//_snprintf_s(buffer, _countof(buffer), _countof(buffer), "%d", MaxBodyLen);
                snprintf(buffer, _countof(buffer)*sizeof(char), "%d", (int)MaxBodyLen);

            	int result = Parser::GetContentLengthVal(buffer);
            	ASSERT_EQ((int) MaxBodyLen, result) << L"ExpectedResult";
            }

            void StompParser_GetContentLengthVal_maxplusone()
            {
            	char buffer[16];
            	//_snprintf_s(buffer, _countof(buffer), _countof(buffer), "%d", ((int) MaxBodyLen) + 1);
                snprintf(buffer, _countof(buffer)*sizeof(char), "%d", (int)MaxBodyLen);
            	int result = Parser::GetContentLengthVal(buffer);
            	ASSERT_EQ((int) MaxBodyLen, result) << L"ExpectedResult";
            }

    };
}

TEST_F(StompParserTests, StompParser_DetermineCommand)
{
	struct
	{
		const char*		pStr;
		RecvCommand::Type	expectedResult;
		int				line;
	}
	static data[] =
	{
		{ "",			  RecvCommand::Invalid,	__LINE__ },

		{ "CONNECTED\n",  RecvCommand::Connected,__LINE__ },
		{ "MESSAGE\n",	  RecvCommand::Message,	__LINE__ },
		{ "RECEIPT\n",	  RecvCommand::Receipt,	__LINE__ },
		{ "ERROR\n",	  RecvCommand::Error,	__LINE__ },

		{ "CONNECTED",	  RecvCommand::Invalid,	__LINE__ },
		{ "MESSAGE",	  RecvCommand::Invalid,	__LINE__ },
		{ "RECEIPT",	  RecvCommand::Invalid,	__LINE__ },
		{ "ERROR",		  RecvCommand::Invalid,	__LINE__ },

		{ "CONNECTEd\n",  RecvCommand::Invalid,	__LINE__ },
		{ "MESSAGe\n",	  RecvCommand::Invalid,	__LINE__ },
		{ "RECEIPt\n",	  RecvCommand::Invalid,	__LINE__ },
		{ "ERROr\n",	  RecvCommand::Invalid,	__LINE__ },

		{ "CONNECTED \n", RecvCommand::Invalid,	__LINE__ },
		{ "MESSAGE \n",	  RecvCommand::Invalid,	__LINE__ },
		{ "RECEIPT \n",	  RecvCommand::Invalid,	__LINE__ },
		{ "ERROR \n",	  RecvCommand::Invalid,	__LINE__ },

		{ " CONNECTED\n", RecvCommand::Invalid,	__LINE__ },
		{ " MESSAGE\n",	  RecvCommand::Invalid,	__LINE__ },
		{ " RECEIPT\n",	  RecvCommand::Invalid,	__LINE__ },
		{ " ERROR\n",	  RecvCommand::Invalid,	__LINE__ },

		{ "connected",	  RecvCommand::Invalid,	__LINE__ },
		{ "message",	  RecvCommand::Invalid,	__LINE__ },
		{ "receipt",	  RecvCommand::Invalid,	__LINE__ },
		{ "error",		  RecvCommand::Invalid,	__LINE__ },

		{ "",			  RecvCommand::Invalid,	__LINE__ },

		{ "CONNECT",	  RecvCommand::Invalid,	__LINE__ },
		{ "SEND",		  RecvCommand::Invalid,	__LINE__ },
		{ "SUBSCRIBE",	  RecvCommand::Invalid,	__LINE__ },
		{ "UNSUBSCRIBE",  RecvCommand::Invalid,	__LINE__ },
		{ "ACK",		  RecvCommand::Invalid,	__LINE__ },
		{ "NACK",		  RecvCommand::Invalid,	__LINE__ },
		{ "BEGIN",		  RecvCommand::Invalid,	__LINE__ },
		{ "COMMIT",		  RecvCommand::Invalid,	__LINE__ },
		{ "ABORT",		  RecvCommand::Invalid,	__LINE__ },
		{ "DISCONNECT",	  RecvCommand::Invalid,	__LINE__ },
		{ "SPAM",		  RecvCommand::Invalid,	__LINE__ }
	};

	for (int test = 0; test < _countof(data); test++)
	{
		char frame[64];
		//strncpy_s(frame, _countof(frame), data[test].pStr, _countof(frame));
        strncpy(frame, data[test].pStr, _countof(frame));
   
		char* pFrame = frame;
		RecvCommand::Type result = StompParserTests::DetermineCommands(pFrame, V1_0);

		ASSERT_EQ((int) data[test].expectedResult, (int) result) << AssertMsg::Format(data[test].line, L"Return", GetVersionStrW(V1_0));

		//strncpy_s(frame, _countof(frame), data[test].pStr, _countof(frame));
        strncpy(frame, data[test].pStr, _countof(frame));

		pFrame = frame;
		result = StompParserTests::DetermineCommands(pFrame, V1_1);
		ASSERT_EQ((int) data[test].expectedResult, (int) result) << AssertMsg::Format(data[test].line, L"Return", GetVersionStrW(V1_1));

		//strncpy_s(frame, _countof(frame), data[test].pStr, _countof(frame));
        strncpy(frame, data[test].pStr, _countof(frame));

		pFrame = frame;
		result = StompParserTests::DetermineCommands(pFrame, V1_2);
		ASSERT_EQ((int) data[test].expectedResult, (int) result) << AssertMsg::Format(data[test].line, L"Return", GetVersionStrW(V1_2));
	}
}


TEST_F(StompParserTests,StompParser_DetermineCommand2_V1_0)
{
	char frame[32];
	//strcpy_s(frame, "CONNECTED\nX");
	strcpy(frame, "CONNECTED\nX");

	char* pFrame = frame;
	RecvCommand::Type command = StompParserTests::DetermineCommands(pFrame, V1_2);

	ASSERT_EQ((int) RecvCommand::Connected, (int) command);
	ASSERT_STREQ("CONNECTED", frame);
	ASSERT_EQ('X', *pFrame);
}

TEST_F(StompParserTests,StompParser_DetermineCommand2_V1_1)
{
	char frame[32];
	//strcpy_s(frame, "CONNECTED\nX");
    strcpy(frame, "CONNECTED\nX");

	char* pFrame = frame;
	RecvCommand::Type command = StompParserTests::DetermineCommands(pFrame, V1_2);

	ASSERT_EQ((int) RecvCommand::Connected, (int) command);
	ASSERT_STREQ("CONNECTED", frame);
	ASSERT_EQ('X', *pFrame);
}

TEST_F(StompParserTests,StompParser_DetermineCommand2_V1_2)
{
	char frame[32];
    //strcpy_s(frame, "CONNECTED\nX");
    strcpy(frame, "CONNECTED\nX");

	char* pFrame = frame;
	RecvCommand::Type command = StompParserTests::DetermineCommands(pFrame, V1_2);

	ASSERT_EQ((int) RecvCommand::Connected, (int) command);
	ASSERT_STREQ("CONNECTED", frame);
	ASSERT_EQ('X', *pFrame);
}

TEST_F(StompParserTests,StompParser_GetEolLen)
{
	struct
	{
		const char*		pFrame;
		Version	version;
		int				expectedResult;
		int				line;
	}
	static data[] =
	{
		{ "\n",    V1_0,  1, __LINE__ },
		{ "\n",    V1_1,  1, __LINE__ },
		{ "\n",    V1_2,  1, __LINE__ },

		{ "\r\n",  V1_0, -1, __LINE__ },
		{ "\r\n",  V1_1, -1, __LINE__ },
		{ "\r\n",  V1_2,  2, __LINE__ },

    	{ "\r1",   V1_0, -1, __LINE__ },
		{ "\r1",   V1_1, -1, __LINE__ },
		{ "\r1",   V1_2, -1, __LINE__ },

		{ "0\n",   V1_0,  0, __LINE__ },
		{ "0\n",   V1_1,  0, __LINE__ },
		{ "0\n",   V1_2,  0, __LINE__ },

    	{ "0\r\n", V1_0,  0, __LINE__ },
		{ "0\r\n", V1_1,  0, __LINE__ },
		{ "0\r\n", V1_2,  0, __LINE__ },

		{ "0\rA",  V1_0,  0, __LINE__ },
		{ "0\rA",  V1_1,  0, __LINE__ },
		{ "0\rA",  V1_2,  0, __LINE__ },

		{ "",	   V1_0, -1, __LINE__ },
		{ "",	   V1_1, -1, __LINE__ },
		{ "",	   V1_2, -1, __LINE__ }
	};

	for (int test = 0; test < _countof(data); test++)
	{
		int result = StompParserTests::GetEolLength(data[test].pFrame, data[test].version);
		ASSERT_EQ(data[test].expectedResult, result) << AssertMsg::Format(data[test].line, L"Result", L"%s", GetVersionStr(data[test].version));
	}
}

TEST_F(StompParserTests,StompParser_ParseHeader_ctor_V1_0)
{
	//Parser::ParseHeader parseHeader(V1_0);

	//ASSERT_EQ((int) V1_0, (int) parseHeader.version) << , L"version";
    StompParser_ParseHeader_cstor_V1_0();
}

TEST_F(StompParserTests,StompParser_ParseHeader_ctor_V1_1)
{
	//Parser::ParseHeader parseHeader(V1_1);

	//ASSERT_EQ((int) V1_1, (int) parseHeader.version) <<  L"version";
    StompParser_ParseHeader_cstor_V1_1();
}

TEST_F(StompParserTests,StompParser_ParseHeader_ctor_V1_2)
{
	//Parser::ParseHeader parseHeader(V1_2);

	//ASSERT_EQ((int) V1_2, (int) parseHeader.version) << L"version";
    StompParser_ParseHeader_cstor_V1_2();
}

// All versions have the same behavior so there is no reason to check
// them separately.
TEST_F(StompParserTests,StompParser_Reset)
{
    Stomp_ParserResetTest();
}

TEST_F(StompParserTests,StompParser_ParseHeaderConnected_Parse_V1_0a)
{

    StompParser_ParseHeaderConnected_Parse_V1_0A();
}

TEST_F(StompParserTests,StompParser_ParseHeaderConnected_Parse_V1_0b)
{
    StompParser_ParseHeaderConnected_Parse_V1_0B();
}

TEST_F(StompParserTests,StompParser_ParseHeaderConnected_Parse_V1_0c)
{
    StompParser_ParseHeaderConnected_Parse_V1_0C();
}

TEST_F(StompParserTests, StompParser_ParseHeaderConnected_Parse_V1_2c)
{
    StompParser_ParseHeaderConnected_Parse_V1_2C();
}


TEST_F(StompParserTests,StompParser_ParseHeader_Parse)
{
    ParserHeader_Parse();
}

TEST_F(StompParserTests,StompParser_Parse_badframes)
{
	Parse_BadFrames();
}

TEST_F(StompParserTests,StompParser_Parse_badcommand)
{
    Parse_BadCommands();
}

TEST_F(StompParserTests,StompParser_Parse_simpleframes)
{
    Parse_SimpleFrame();
}

TEST_F(StompParserTests,StompParser_Parse_OnCommand_fail)
{
    Parse_OnCommand_Fail();
}

TEST_F(StompParserTests,StompParser_Parse_OnHeader_fail)
{
    Parse_OnHeader_fail();
}

TEST_F(StompParserTests,StompParser_Parse_OnBody_fail)
{
    Parse_OnBody_fail();
}

TEST_F(StompParserTests,StompParser_Parse_coverage)
{
	Parse_Coverage();
}

TEST_F(StompParserTests,StompParser_Parse_InvalidCharAfterCommand)
{
    Parse_InvalidCharAfterCommand();
}

TEST_F(StompParserTests,StompParser_Parse_InvalidCharAfterHeaders)
{
    Parse_InvalidCharAfterHeaders();
}


TEST_F(StompParserTests,StompParser_Parse_escapechars_V1_0)
{
    StompParser_Parse_escapechars_V1_0();
}

TEST_F(StompParserTests,StompParser_Parse_badescapechars_V1_0)
{
    StompParser_Parse_badescapechars_V1_0();
}

TEST_F(StompParserTests,StompParser_Parse_escapechars_V1_2)
{
    StompParser_Parse_escapechars_V1_2();
}

TEST_F(StompParserTests,StompParser_Parse_badreturninheader_V1_2)
{
    StompParser_Parse_badreturninheader_V1_2();
}

TEST_F(StompParserTests,StompParser_Parse_emptykey)
{
    StompParser_Parse_emptykey();
}

TEST_F(StompParserTests,StompParser_Parse_headeronly)
{
    StompParser_Parse_headeronly();
}

TEST_F(StompParserTests,StompParser_Parse_headeronly2)
{
    StompParser_Parse_headeronly2();
}

TEST_F(StompParserTests,StompParser_Parse_headeronlybad_V1_0)
{
    StompParser_Parse_headeronlybad_V1_0();
}

TEST_F(StompParserTests,StompParser_Parse_headeronlybad_V1_1)
{
	StompParser_Parse_headeronlybad_V1_1();
}

TEST_F(StompParserTests,StompParser_Parse_headeronlybad_V1_2)
{
	StompParser_Parse_headeronlybad_V1_2();
}

TEST_F(StompParserTests,StompParser_Parse_contentlength)
{
	StompParser_Parse_contentlength();
}

TEST_F(StompParserTests,StompParser_Parse_contentlengthinvalid1)
{
	StompParser_Parse_contentlengthinvalid1();
}

TEST_F(StompParserTests,StompParser_Parse_contentlengthinvalid2)
{
    StompParser_Parse_contentlengthinvalid2();
}

TEST_F(StompParserTests,StompParser_Parse_contentlengthlarge)
{
	StompParser_Parse_contentlengthlarge();
}

TEST_F(StompParserTests,StompParser_Parse_contentlengthmissing)
{
	StompParser_Parse_contentlengthmissing();
}
TEST_F(StompParserTests,StompParser_Parse_headeronly_invalidheaderlen)
{
	StompParser_Parse_headeronly_invalidheaderlen();
}

TEST_F(StompParserTests,StompParser_VerifyEndOfHeader)
{
	StompParser_VerifyEndOfHeader();
}

TEST_F(StompParserTests,StompParser_GetContentLengthVal)
{
	StompParser_GetContentLengthVal();
}

TEST_F(StompParserTests,StompParser_GetContentLengthValMax)
{
	StompParser_GetContentLengthValMax();
}

TEST_F(StompParserTests,StompParser_GetContentLengthVal_maxplusone)
{
	StompParser_GetContentLengthVal_maxplusone();
}


