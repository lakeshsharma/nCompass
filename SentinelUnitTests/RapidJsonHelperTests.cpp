#include "stdafx.h"
#include "TestUtil.h"
#include "RapidJsonHelper.h"

#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;

namespace SentinelUnitTests
{
	static const char json[] = "{"
		"\"String\": \"1\","
		"\"Int\":     -2,"
		"\"UInt\":     3,"
		"\"Int64\":   -4,"
		"\"UInt64\":   5,"
		"\"Double\":   6.7,"
		"\"Double2\":  6,"
		"\"Bool\":     true"
		"}";

	static const char jsonInvalid[] = "{"
		"\"string\":   1,"
		"\"int\":   \"-2\","
		"\"uInt\":   \"3\","
		"\"int64\": \"-4\","
		"\"uInt64\": \"5\","
		"\"double\": \"6.7\","
		"\"bool\":     7.8"
		"}";

	static const char json2[] = "{\"version\":1,\"name\":\"P1\",\"contentHttpUrl\":\"<TEMP>/\",\"items\":["
		"{\"filename\":\"File2\",\"MD5\":\"MD52\",\"duration\":22,\"isImage\":false},"
		"{\"filename\":\"File3\",\"MD5\":\"MD53\",\"duration\":26,\"isImage\":false},"
		"{\"filename\":\"File4\",\"MD5\":\"MD54\",\"duration\":28,\"isImage\":true}"
		"]}";

		TEST(RapidJsonHelperTests, JsonParseExceptionConstructor1)
		{
			const char* pMsg = "version";
			const rapidjson::ParseErrorCode errorCode = rapidjson::kParseErrorValueInvalid;

			JsonException e(errorCode, pMsg);

			ASSERT_STREQ((char*)pMsg, e.what()) << L"what()";
			ASSERT_EQ((intptr_t) errorCode, e.code())<<  L"parseErrorCode";
			ASSERT_STREQ("", e.filename()) << L"filename";
		}

		TEST(RapidJsonHelperTests, JsonParseExceptionConstructor1a)
		{
			const char* pMsg = "version";
			const rapidjson::ParseErrorCode errorCode = rapidjson::kParseErrorValueInvalid;
			const char* pFile = "File";

			JsonException e(errorCode, pMsg, pFile);

			ASSERT_STREQ((char*)pMsg, e.what()) << L"what()";
			ASSERT_EQ((intptr_t) errorCode, e.code()) << L"parseErrorCode";
			ASSERT_STREQ((char*)pFile, e.filename()) << L"filename";
		}

		TEST(RapidJsonHelperTests, JsonParseExceptionConstructor2)
		{
			const char* pMsg = "version";
			const JsonException::JsonErrorCode errorCode = JsonException::MemberNotFound;

			JsonException e(errorCode, pMsg);

			ASSERT_STREQ((char*)pMsg, e.what()) << "what()";
			ASSERT_EQ((intptr_t) errorCode, e.code()) << "jsonErrorCode";
			ASSERT_STREQ("", e.filename()) << "filename";
		}

		TEST(RapidJsonHelperTests, JsonParseExceptionConstructor2a)
		{
			const char* pMsg = "version";
			const JsonException::JsonErrorCode errorCode = JsonException::MemberNotFound;
			const char* pFile = "File";

			JsonException e(errorCode, pMsg, pFile);

			ASSERT_STREQ((char*)pMsg, e.what()) << "what()";
			ASSERT_EQ((intptr_t) errorCode, e.code()) << "jsonErrorCode";
			ASSERT_STREQ(pFile, e.filename()) << "filename";
		}

		TEST(RapidJsonHelperTests, JsonParseExceptionConstructor3)
		{
			const char* pMsg = "version";
			const intptr_t errorCode = -1;

			JsonException e(errorCode, pMsg);

			ASSERT_STREQ((char*)pMsg, e.what()) << "what()";
			ASSERT_EQ((intptr_t) errorCode, e.code()) << "parseErrorCode";
			ASSERT_STREQ("", e.filename()) << "filename";
		}

		TEST(RapidJsonHelperTests, JsonParseExceptionConstructor3a)
		{
			const char* pMsg = "version";
			const intptr_t errorCode = -1;
			const  char* pFile = "File";

			JsonException e(errorCode, pMsg, pFile);

			ASSERT_STREQ((char*)pMsg, e.what()) << "what()";
			ASSERT_EQ((intptr_t) errorCode, e.code()) << "parseErrorCode";
			ASSERT_STREQ(pFile, e.filename()) << "filename";
		}

		TEST(RapidJsonHelperTests, JsonFindMemberTest)
		{
			rapidjson::Document document;
			bool err = document.Parse(json).HasParseError();
			ASSERT_FALSE(err) << "Parse";
			const char* pStreamName = "Stream";

			try
			{
				JsonFindMemberEx(document, "String", pStreamName);
			}
			catch (JsonException&)
			{
				FAIL();
			}
		}

		TEST(RapidJsonHelperTests, JsonFindMemberTestFail)
		{
			rapidjson::Document document;
			bool err = document.Parse(json).HasParseError();
			ASSERT_FALSE(err) << "Parse";

			const char* pName = "missing";
			const char* pStreamName = "Stream";

			try
			{
				JsonFindMemberEx(document, "String", pStreamName);
			}
			catch (JsonException& e)
			{
				ASSERT_STREQ(pName, e.what()) << "what()";
				ASSERT_EQ((intptr_t) JsonException::MemberNotFound, e.code()) << "code()";
				ASSERT_STREQ(pStreamName, e.filename()) << "filename()";
			}
		}

		TEST(RapidJsonHelperTests, JsonGetTests_Good)
		{
			char buffer[sizeof(json)];
			memcpy(buffer, json, sizeof(json));

			rapidjson::Document document;
			bool err = document.ParseInsitu(buffer).HasParseError();
			ASSERT_FALSE(err) << "ParseInsitu";

			try
			{
				{
					string val = JsonGetStringEx(document, "String");
					ASSERT_STREQ("1", val.c_str()) << "String";
				}
				{
					int val = JsonGetIntEx(document, "Int");
					ASSERT_EQ((int) -2, val) << "Int";
				}
				{
					unsigned val = JsonGetUintEx(document, "UInt");
					ASSERT_EQ((unsigned) 3, val) << "UInt";
				}
				{
					int64_t val = JsonGetInt64Ex(document, "Int64");
					ASSERT_EQ((int) -4, (int) val) << "Int64";
				}
				{
					uint64_t val = JsonGetUint64Ex(document, "UInt64");
					ASSERT_EQ((uint64_t) 5, val) << "UInt64";
				}
				{
					double val = JsonGetDoubleEx(document, "Double", true);
					ASSERT_EQ((double) 6.7, val) << "Double";
				}
				{
					double val = JsonGetDoubleEx(document, "Double2", false);
					ASSERT_EQ((double) 6.0, val) << "Double2";
				}
				{
					bool val = JsonGetBoolEx(document, "Bool");
					ASSERT_EQ(true, val) << "Bool";
				}
			}
			catch (JsonException& e)
			{
				FAIL() << AssertMsg::FormatOnly(L"name: %S: ParseErrorCode: %d",	e.what(), e.code());
			}
		}

		TEST(RapidJsonHelperTests, JsonGetTests_Missing)
		{
			char buffer[sizeof(json)];
			memcpy(buffer, json, sizeof(json));

			rapidjson::Document document;
			bool err = document.ParseInsitu(buffer).HasParseError();
			ASSERT_FALSE(err) << "ParseInsitu";

			try
			{
				JsonGetStringEx(document, "string");
			}
			catch (JsonException& e)
			{
				ASSERT_STREQ("string", e.what()) << "what()";
				ASSERT_EQ((intptr_t) JsonException::MemberNotFound, e.code()) << "code()";
				ASSERT_STREQ("", e.filename()) << "filename()";
			}

			try
			{
				JsonGetIntEx(document, "int");
			}
			catch (JsonException& e)
			{
				ASSERT_STREQ("int", e.what());
				ASSERT_EQ((intptr_t) JsonException::MemberNotFound, e.code()) << "code()";
				ASSERT_STREQ("", e.filename()) << "filename()";
			}

			try
			{
				JsonGetUintEx(document, "uInt");
			}
			catch (JsonException& e)
			{
				ASSERT_STREQ("uInt", e.what());
				ASSERT_EQ((intptr_t) JsonException::MemberNotFound, e.code()) << "code()";
				ASSERT_STREQ("", e.filename()) << "filename()";
			}

			try
			{
				JsonGetInt64Ex(document, "int64");
			}
			catch (JsonException& e)
			{
				ASSERT_STREQ("int64", e.what());
				ASSERT_EQ((intptr_t) JsonException::MemberNotFound, e.code()) << "code()";
				ASSERT_STREQ("", e.filename()) << "filename()";
			}

			try
			{
				JsonGetUint64Ex(document, "uInt64");
			}
			catch (JsonException& e)
			{
				ASSERT_STREQ("uInt64", e.what());
				ASSERT_EQ((intptr_t) JsonException::MemberNotFound, e.code()) << "code()";
				ASSERT_STREQ("", e.filename()) << "filename()";
			}

			try
			{
				JsonGetDoubleEx(document, "double");
			}
			catch (JsonException& e)
			{
				ASSERT_STREQ("double", e.what());
				ASSERT_EQ((intptr_t) JsonException::MemberNotFound, e.code()) << "code()";
				ASSERT_STREQ("", e.filename()) << "filename()";
			}

			try
			{
				JsonGetBoolEx(document, "bool");
			}
			catch (JsonException& e)
			{
				ASSERT_STREQ("bool", e.what());
				ASSERT_EQ((intptr_t) JsonException::MemberNotFound, e.code()) << "code()";
				ASSERT_STREQ("", e.filename()) << "filename()";
			}
		}

		TEST(RapidJsonHelperTests, JsonGetTests_Invalid)
		{
			char buffer[sizeof(jsonInvalid)];
			memcpy(buffer, jsonInvalid, sizeof(jsonInvalid));

			rapidjson::Document document;
			bool err = document.ParseInsitu(buffer).HasParseError();
			ASSERT_FALSE(err) << "ParseInsitu";

			try
			{
				JsonGetStringEx(document, "string");
			}
			catch (JsonException& e)
			{
				ASSERT_STREQ("string", e.what());
				ASSERT_EQ((intptr_t) JsonException::ValueIsNotString, e.code()) << "code()";
				ASSERT_STREQ("", e.filename()) << "filename()";
			}

			try
			{
				JsonGetIntEx(document, "int");
			}
			catch (JsonException& e)
			{
				ASSERT_STREQ("int", e.what());
				ASSERT_EQ((intptr_t) JsonException::ValueIsNotInt, e.code()) << "code()";
				ASSERT_STREQ("", e.filename()) << "filename()";
			}

			try
			{
				JsonGetUintEx(document, "uInt");
			}
			catch (JsonException& e)
			{
				ASSERT_STREQ("uInt", e.what());
				ASSERT_EQ((intptr_t) JsonException::ValueIsNotUint, e.code()) << "code()";
				ASSERT_STREQ("", e.filename()) << "filename()";
			}

			try
			{
				JsonGetInt64Ex(document, "int64");
			}
			catch (JsonException& e)
			{
				ASSERT_STREQ("int64", e.what());
				ASSERT_EQ((intptr_t) JsonException::ValueIsNotInt64, e.code()) << "code()";
				ASSERT_STREQ("", e.filename()) << "filename()";
			}

			try
			{
				JsonGetUint64Ex(document, "uInt64");
			}
			catch (JsonException& e)
			{
				ASSERT_STREQ("uInt64", e.what());
				ASSERT_EQ((intptr_t) JsonException::ValueIsNotUint64, e.code()) << "code()";
				ASSERT_STREQ("", e.filename()) << "filename()";
			}

			try
			{
				JsonGetDoubleEx(document, "double");
			}
			catch (JsonException& e)
			{
				ASSERT_STREQ("double", e.what());
				ASSERT_EQ((intptr_t) JsonException::ValueIsNotNumber, e.code()) << "code()";
				ASSERT_STREQ("", e.filename()) << "filename()";
			}

			try
			{
				JsonGetBoolEx(document, "bool");
			}
			catch (JsonException& e)
			{
				ASSERT_STREQ("bool", e.what());
				ASSERT_EQ((intptr_t) JsonException::ValueIsNotBool, e.code()) << "code()";
				ASSERT_STREQ("", e.filename()) << "filename()";
			}
		}

		TEST(RapidJsonHelperTests, JsonReadStreamStringTest)
		{
			try
			{
				JsonReadStreamString stream(json2);
				rapidjson::Document document;
				if (document.ParseStream(stream).HasParseError())
					FAIL() << AssertMsg::FormatOnly(L"Json ParseStream error: %S", rapidjson::GetParseError_En(document.GetParseError()));
			}
			catch (JsonException&)
			{
			}
		}

		TEST(RapidJsonHelperTests, JsonReadStreamString_Take)
		{
			char buffer[8];

			struct
			{
				int line;
				char* pBuffer;
				size_t bufferLen;
				size_t expectedReturn;
				const char* pExpectedResult;
				char expectedNextChar;
			}
			data[] = 
			{
				{ __LINE__, buffer,  0, 0, ".......", '1' },
				{ __LINE__, buffer,  1, 1, "1......", '2' },
				{ __LINE__, buffer,  2, 2, "12.....", '3' },
				{ __LINE__, buffer,  3, 3, "123....", '4' },
				{ __LINE__, buffer,  4, 4, "1234...",  0  },
				{ __LINE__, buffer,  5, 4, "1234...",  0  },
				{ __LINE__, nullptr, 0, 0, ".......", '1' },
				{ __LINE__, nullptr, 1, 1, ".......", '2' },
				{ __LINE__, nullptr, 2, 2, ".......", '3' },
				{ __LINE__, nullptr, 3, 3, ".......", '4' },
				{ __LINE__, nullptr, 4, 4, ".......",  0  },
				{ __LINE__, nullptr, 5, 4, ".......",  0  },
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				memcpy(buffer, ".......", 8);
				JsonReadStreamString stringRead("1234");
				size_t result = stringRead.Take(data[i].pBuffer, data[i].bufferLen);

				ASSERT_EQ((int)data[i].expectedReturn, (int)result) << AssertMsg::Format(data[i].line) << "Return";
				ASSERT_STREQ(data[i].pExpectedResult, buffer) << AssertMsg::Format(data[i].line) << "Buffer";

				char ch = stringRead.Take();
				ASSERT_EQ((int)data[i].expectedNextChar, (int)ch) << AssertMsg::Format(data[i].line) << "NextChar";
			}
		}

		TEST(RapidJsonHelperTests, GetJsonErrorCodeString_tests)
		{
			ASSERT_STREQ("JSON File not found"			, JsonException::GetJsonErrorCodeString(JsonException::JsonFileNotFound )) << "JsonFileNotFound" ;
			ASSERT_STREQ("DuplicateFilename"			, JsonException::GetJsonErrorCodeString(JsonException::DuplicateFilename)) << "DuplicateFilename";
			ASSERT_STREQ("Member not found"				, JsonException::GetJsonErrorCodeString(JsonException::MemberNotFound   )) << "MemberNotFound"   ;
			ASSERT_STREQ("String value is null or empty", JsonException::GetJsonErrorCodeString(JsonException::StringValueEmpty )) << "StringValueEmpty" ;
			ASSERT_STREQ("Value is not a string"		, JsonException::GetJsonErrorCodeString(JsonException::ValueIsNotString )) << "ValueIsNotString" ;
			ASSERT_STREQ("Value is not an Int"			, JsonException::GetJsonErrorCodeString(JsonException::ValueIsNotInt    )) << "ValueIsNotInt"    ;
			ASSERT_STREQ("Value is not a UInt"			, JsonException::GetJsonErrorCodeString(JsonException::ValueIsNotUint   )) << "ValueIsNotUint"   ;
			ASSERT_STREQ("Value is not an Int64"		, JsonException::GetJsonErrorCodeString(JsonException::ValueIsNotInt64  )) << "ValueIsNotInt64"  ;
			ASSERT_STREQ("Value is not a UInt64"		, JsonException::GetJsonErrorCodeString(JsonException::ValueIsNotUint64 )) << "ValueIsNotUint64" ;
			ASSERT_STREQ("Value is not a double"		, JsonException::GetJsonErrorCodeString(JsonException::ValueIsNotDouble )) << "ValueIsNotDouble" ;
			ASSERT_STREQ("Value is not a number"		, JsonException::GetJsonErrorCodeString(JsonException::ValueIsNotNumber )) << "ValueIsNotNumber" ;
			ASSERT_STREQ("Value is not a bool"			, JsonException::GetJsonErrorCodeString(JsonException::ValueIsNotBool   )) << "ValueIsNotBool"   ;
			ASSERT_STREQ("Value is not an array"		, JsonException::GetJsonErrorCodeString(JsonException::ValueIsNotArray  )) << "ValueIsNotArray"  ;
			ASSERT_STREQ("Value is not an object"		, JsonException::GetJsonErrorCodeString(JsonException::ValueIsNotObject )) << "ValueIsNotObject" ;

			ASSERT_STREQ("Unknown error"				, JsonException::GetJsonErrorCodeString(JsonException::__last__JsonErrorCode__)) << "__last__JsonErrorCode__";
			ASSERT_STREQ("Unknown error"				, JsonException::GetJsonErrorCodeString((JsonException::JsonErrorCode) INT_MAX)) << "INT_MAX";
		}

		TEST(RapidJsonHelperTests, JsonWriter_Write_tests)
		{
			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);

			writer.StartObject();

			writer.Key("1"); writer.String(L"One");
			writer.Key("2"); writer.String(L"Twoo", 3);
			writer.Key("3"); writer.String(std::wstring(L"Three"));
			writer.Key("4"); writer.String("Four");
			writer.Key("5"); writer.String("Fivee", 4);
			writer.Key("6"); writer.String("Six");
			writer.Key("7"); writer.String(std::string("seven"));
			writer.Key("8"); writer.String("eight");
			writer.Key("pi"); writer.Currency(314);

			writer.EndObject();

			rapidjson::Document document;
			bool err = document.Parse(writeStream).HasParseError();
			ASSERT_FALSE(err) << "Parse";

			try
			{
				const char*
				pVal = document["1"].GetString(); writer.String(L"One");
				ASSERT_STREQ("One", pVal) << "One";

				pVal = document["2"].GetString(); writer.String(L"Twoo", 3);
				ASSERT_STREQ("Two", pVal) << "Two";

				pVal = document["3"].GetString(); writer.String(std::wstring(L"Three"));
				ASSERT_STREQ("Three", pVal) << "Three";

				pVal = document["4"].GetString(); writer.String("Four");
				ASSERT_STREQ("Four", pVal) << "Four";

				pVal = document["5"].GetString(); writer.String("Fivee", 4);
				ASSERT_STREQ("Five", pVal) << "Five";

				pVal = document["6"].GetString(); writer.String("Six");
				ASSERT_STREQ("Six", pVal) << "Six";

				pVal = document["7"].GetString(); writer.String(std::string("seven"));
				ASSERT_STREQ("seven", pVal) << "seven";

				pVal = document["8"].GetString(); writer.String("eight");
				ASSERT_STREQ("eight", pVal) << "eight";

				double val = document["pi"].GetDouble(); writer.Currency(314);
				ASSERT_EQ(3.14, val) << "pi";
			}
			catch (JsonException& e)
			{
				FAIL() << AssertMsg::FormatOnly(L"name: %S: ParseErrorCode: %d",	e.what(), e.code());
			}
		}

		TEST(RapidJsonHelperTests, JsonConditionalObject_empty_Test)
		{
			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);

			writer.StartObject();

			JsonConditionalObject conditionalObject(writer, "Object");
			conditionalObject.End();

			writer.EndObject();

			ASSERT_STREQ("{}", writeStream.m_str.c_str());
		}

		TEST(RapidJsonHelperTests, JsonConditionalObject_not_empty_Test1)
		{
			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);

			writer.StartObject();
			{
				JsonConditionalObject conditionalObject(writer, "Object");
				writer.Key("K"); writer.Int(1);
				conditionalObject.Start();
			}
			writer.EndObject();

			ASSERT_STREQ("{\"K\":1,\"Object\":{}}", writeStream.m_str.c_str());
		}

		TEST(RapidJsonHelperTests, JsonConditionalObject_not_empty_Test2)
		{
			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);

			writer.StartObject();

			JsonConditionalObject conditionalObject(writer, "Object");
			conditionalObject.Start();
			conditionalObject.Start();
			conditionalObject.End();
			writer.Key("K"); writer.Int(1);
			conditionalObject.End();

			writer.EndObject();

			ASSERT_STREQ("{\"Object\":{},\"K\":1}", writeStream.m_str.c_str());
		}

		TEST(RapidJsonHelperTests, JsonConditionalArray_empty_Test)
		{
			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);

			writer.StartObject();

			JsonConditionalArray conditionalArray(writer, "Array");
			conditionalArray.End();

			writer.EndObject();

			ASSERT_STREQ("{}", writeStream.m_str.c_str());
		}

		TEST(RapidJsonHelperTests, JsonConditionalArray_not_empty_Test1)
		{
			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);

			writer.StartObject();
			{
				JsonConditionalArray conditionalArray(writer, "Array");
				writer.Key("K"); writer.Int(1);
				conditionalArray.Start();
			}
			writer.EndObject();

			ASSERT_STREQ("{\"K\":1,\"Array\":[]}", writeStream.m_str.c_str());
		}

		TEST(RapidJsonHelperTests, JsonConditionalArray_not_empty_Test2)
		{
			JsonWriteStreamString writeStream;
			JsonWriter writer(writeStream);

			writer.StartObject();

			JsonConditionalArray conditionalArray(writer, "Array");
			conditionalArray.Start();
			conditionalArray.Start();
			conditionalArray.End();
			writer.Key("K"); writer.Int(1);
			conditionalArray.End();

			writer.EndObject();

			ASSERT_STREQ("{\"Array\":[],\"K\":1}", writeStream.m_str.c_str());
		}

// #pragma region JsonGetArrayEx
		TEST(RapidJsonHelperTests, JsonGetArrayEx_Present)
		{
			static const char* pJson =
				"{"
					"\"SessionId\":470,"
					"\"PatronInformation\":{"
						"\"CardNumber\":\"4BDC04FD39\","
						"\"PlayerId\":0,"
						"\"Tier\":\"Default\","
						"\"Name\":\"joe\""
					"},"
					"\"EligibilityDetermined\":true,"
					"\"EligibleLevels\":["
						"{"
							"\"LevelId\":24037,"
							"\"Name\":\"La Montaña\","
							"\"TriggerType\":\"CoinInTriggerOnly\","
							"\"Eligible\":true,"
							"\"Minors\":["
								"{"
									"\"Sequence\":1,"
									"\"Eligible\":true,"
									"\"Name\":\"1\","
									"\"Min\":0.00,"
									"\"Max\":0.00"
								"}"
							"]"
						"}"
					"],"
					"\"SourceDeviceId\":1,"
					"\"SourceDeviceConfigVersion\":0,"
					"\"SourceAsset\":\"\","
					"\"Timestamp\":1433466405284"
				"}";

			//OutputDebugStringA("### pJson ###\n" + stringA(pJson) + "\n### pJson ###\n");

			try
			{
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(pJson).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

				const rapidjson::Value &eligibleLevels = JsonGetArrayEx(jsonDoc, "EligibleLevels");
				ASSERT_EQ((__int64)24037, JsonGetInt64Ex(*eligibleLevels.Begin(), "LevelId"));
			}
			catch (JsonException& e)
			{
				FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename());
			}
			catch (SentinelException& e)
			{
				FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what());
			}
		}

		TEST(RapidJsonHelperTests, JsonGetArrayEx_NotPresent)
		{
			static const char* pJson =
				"{"
					"\"SessionId\":470,"
					"\"PatronInformation\":{"
						"\"CardNumber\":\"4BDC04FD39\","
						"\"PlayerId\":0,"
						"\"Tier\":\"Default\","
						"\"Name\":\"joe\""
					"},"
					"\"EligibilityDetermined\":true,"
					"\"SourceDeviceId\":1,"
					"\"SourceDeviceConfigVersion\":0,"
					"\"SourceAsset\":\"\","
					"\"Timestamp\":1433466405284"
				"}";

			//OutputDebugStringA("### pJson ###\n" + stringA(pJson) + "\n### pJson ###\n");

			try
			{
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(pJson).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

				try
				{
					const rapidjson::Value &eligibleLevels = JsonGetArrayEx(jsonDoc, "EligibleLevels");
					FAIL() << L"Exception not thrown";
				}
				catch (JsonException& e)
				{
					ASSERT_STREQ("EligibleLevels", e.what()) << "what()";
					ASSERT_EQ((intptr_t)JsonException::MemberNotFound, e.code()) << "code()";
					ASSERT_STREQ("", e.filename()) << "filename()";
				}
			}
			catch (JsonException& e)
			{
				FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename());
			}
			catch (SentinelException& e)
			{
				FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what());
			}
		}
// #pragma endregion JsonGetArrayEx

// #pragma region JsonGetOptionalArrayEx
		TEST(RapidJsonHelperTests, JsonGetOptionalArrayEx_Present)
		{
			static const char* pJson =
				"{"
					"\"SessionId\":470,"
					"\"PatronInformation\":{"
						"\"CardNumber\":\"4BDC04FD39\","
						"\"PlayerId\":0,"
						"\"Tier\":\"Default\","
						"\"Name\":\"joe\""
					"},"
					"\"EligibilityDetermined\":true,"
					"\"EligibleLevels\":["
						"{"
							"\"LevelId\":24037,"
							"\"Name\":\"La Montaña\","
							"\"TriggerType\":\"CoinInTriggerOnly\","
							"\"Eligible\":true,"
							"\"Minors\":["
								"{"
									"\"Sequence\":1,"
									"\"Eligible\":true,"
									"\"Name\":\"1\","
									"\"Min\":0.00,"
									"\"Max\":0.00"
								"}"
							"]"
						"}"
					"],"
					"\"SourceDeviceId\":1,"
					"\"SourceDeviceConfigVersion\":0,"
					"\"SourceAsset\":\"\","
					"\"Timestamp\":1433466405284"
				"}";

			//OutputDebugStringA("### pJson ###\n" + stringA(pJson) + "\n### pJson ###\n");

			try
			{
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(pJson).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

				const rapidjson::Value *eligibleLevels = JsonGetOptionalArrayEx(jsonDoc, "EligibleLevels");
				ASSERT_TRUE(eligibleLevels != NULL) << "eligibleLevels";
				ASSERT_EQ((__int64)24037, JsonGetInt64Ex(*eligibleLevels->Begin(), "LevelId"));
			}
			catch (JsonException& e)
			{
				FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename());
			}
			catch (SentinelException& e)
			{
				FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what());
			}
		}

		TEST(RapidJsonHelperTests, JsonGetOptionalArrayEx_NotPresent)
		{
			static const char* pJson =
				"{"
					"\"SessionId\":470,"
					"\"PatronInformation\":{"
						"\"CardNumber\":\"4BDC04FD39\","
						"\"PlayerId\":0,"
						"\"Tier\":\"Default\","
						"\"Name\":\"joe\""
					"},"
					"\"EligibilityDetermined\":true,"
					"\"SourceDeviceId\":1,"
					"\"SourceDeviceConfigVersion\":0,"
					"\"SourceAsset\":\"\","
					"\"Timestamp\":1433466405284"
				"}";

			//OutputDebugStringA("### pJson ###\n" + stringA(pJson) + "\n### pJson ###\n");

			try
			{
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(pJson).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

				const rapidjson::Value *eligibleLevels = JsonGetOptionalArrayEx(jsonDoc, "EligibleLevels");
				ASSERT_TRUE(eligibleLevels == NULL) << "eligibleLevels";
			}
			catch (JsonException& e)
			{
				FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename());
			}
			catch (SentinelException& e)
			{
				FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what());
			}
		}
// #pragma endregion JsonGetOptionalArrayEx
}
