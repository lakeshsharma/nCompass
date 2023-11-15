#include "stdafx.h"
#include "Utilities.h"
#include "TestUtil.h"
#include "RapidJsonHelper.h"

#include "gtest/gtest.h"
#include "AssertMsg.h"
using namespace std;
namespace SentinelUnitTests
{
	/* The JSON contents from below repeated here for non-escaped clarity
		{
			"baseStr": "string",
			"item": {
				"baseInt" : 123,
				"baseBool" : false,
			}
			"baseArray": [
				{ "value1": "v1.0", "value2" : 2.0 },
				{ "value1": "v1.1", "value2" : 2.1 }
			]
		}

		{"baseStr":"string","item":{"baseInt":123,"baseBool":false,}"baseArray":[{"value1":"v1.0","value2":2.0},{"value1":"v1.1","value2":2.1}]}
	*/

	struct JsonItem
	{
		int baseInt;
		bool baseBool;

		JsonItem(int baseInt = 0, bool baseBool = false)
			: baseInt(baseInt)
			, baseBool(baseBool)
		{
		}

		void Deserialize(const rapidjson::Value& item)
		{
			baseInt = JsonGetIntEx(item, "baseInt");
			baseBool = JsonGetBoolEx(item, "baseBool");
		}

		template <typename Writer>
		void Serialize(Writer& writer) const
		{
			writer.StartObject();

			writer.Key("baseInt");	 writer.Int(baseInt);
			writer.Key("baseBool");	 writer.Bool(baseBool);

			writer.EndObject();
		}
	};

	struct JsonValue
	{
		string value1;
		double  value2;

		JsonValue(const char* pValue1, double value2)
			: value1(pValue1)
			, value2(value2)
		{
		}

		JsonValue(rapidjson::Value::ConstValueIterator valueIt)
		{
			value1 = JsonGetStringEx(*valueIt, "value1");
			value2 = JsonGetDoubleEx(*valueIt, "value2");
		}

		template <typename Writer>
		void Serialize(Writer& writer) const
		{
			writer.StartObject(); 

           //writer.Key("value1"); writer.String(CUtilities::ConvertWideCharsToUtf8String((LPCWSTR)value1.c_str()));
            writer.Key("value1"); writer.String(value1.c_str());
			writer.Key("value2"); writer.Double(value2);

			writer.EndObject();
		}
	};

	struct JsonBase
	{
		string baseStr;
		JsonItem jsonItem;
		vector<unique_ptr<JsonValue>> jsonValues;

		JsonBase(const char* pBaseStr, int baseInt, bool baseBool)
			: baseStr(pBaseStr)
			, jsonItem(baseInt, baseBool)
		{
		}
        void exceptionOccured(rapidjson::Document& document)
        {
            	FAIL() << AssertMsg::FormatOnly(L"Json Parse error: %S", rapidjson::GetParseError_En(document.GetParseError()));
				//AfxThrowFileException(CFileException::genericException);
                throw;
        }
        void fun_push_back(rapidjson::Document& document)
        {
            try
			{
				baseStr = JsonGetIntEx(document, "baseStr");

				const rapidjson::Value& item = document["item"];
				if (!item.IsObject())
					throw JsonException(rapidjson::kParseErrorValueInvalid, "item");

				jsonItem.Deserialize(item);

				const rapidjson::Value& values = document["baseArray"];
				if (!values.IsArray())
					throw JsonException(rapidjson::kParseErrorValueInvalid, "baseArray");

				for (auto itr = values.Begin(); itr != values.End(); ++itr)
				{
					jsonValues.push_back(unique_ptr<JsonValue>(new JsonValue(itr)));
				}
			}
			catch (JsonException& e)
			{
				FAIL() << AssertMsg::FormatOnly(L"JsonBase Parse error: %S: %S", e.what(), rapidjson::GetParseError_En((rapidjson::ParseErrorCode) e.code()));
                throw;
			}
        }
		JsonBase(const char* pStr, size_t len)
		{
			rapidjson::MemoryStream memStream(pStr, len);
			rapidjson::Document document;
			if (document.ParseStream(memStream).HasParseError())
			{
                exceptionOccured(document);
			}

            fun_push_back(document);
		}

		JsonBase(rapidjson::Document& document)
		{
			try
			{
				baseStr = JsonGetStringEx(document, "baseStr");

				const rapidjson::Value& item = document["item"];
				if (!item.IsObject())
					throw JsonException(rapidjson::kParseErrorValueInvalid, "item");

				jsonItem.Deserialize(item);

				const rapidjson::Value& values = document["baseArray"];
				if (!values.IsArray())
					throw JsonException(rapidjson::kParseErrorValueInvalid, "baseArray");

				for (auto itr = values.Begin(); itr != values.End(); ++itr)
				{
					jsonValues.push_back(unique_ptr<JsonValue>(new JsonValue(itr)));
				}
			}
			catch (JsonException& e)
			{
                exceptionOccured(document);
			}
		}

		template <typename Writer>
		void Serialize(Writer& writer) const
		{
			writer.StartObject();

			writer.Key("baseStr"); writer.String(baseStr.c_str());

			writer.Key("item");
			jsonItem.Serialize(writer);

			writer.Key("baseArray");
			writer.StartArray();
			for_each(jsonValues.cbegin(), jsonValues.cend(), [&writer](const unique_ptr<JsonValue>& jsonValue)
			{
				jsonValue->Serialize(writer);
			});
			writer.EndArray();

			writer.EndObject();
		}

		void Serialize2(JsonWriter& writer) const
		{
			writer.StartObject();

			writer.Key("baseStr"); writer.String(baseStr.c_str());

			writer.Key("item");
			jsonItem.Serialize(writer);

			writer.Key("baseArray");
			writer.StartArray();
			for_each(jsonValues.cbegin(), jsonValues.cend(), [&writer](const unique_ptr<JsonValue>& jsonValue)
			{
				jsonValue->Serialize(writer);
			});
			writer.EndArray();

			writer.EndObject();
		}
	};

	static const char json[] =
		"{\n"
		"    \"baseStr\": \"string\",\n"
		"    \"item\": {\n"
		"        \"baseInt\" : 123,\n"
		"        \"baseBool\" : false\n"
		"    },\n"
		"    \"baseArray\": [\n"
		"        { \"value1\": \"v1.0\", \"value2\" : 2.0 },\n"
		"        { \"value1\": \"v1.1\", \"value2\" : 2.1 }\n"
		"    ]\n"
		"}";

	static const char jsonCompact[] = "{\"baseStr\":\"String\",\"item\":{\"baseInt\":123,\"baseBool\":false},\"baseArray\":[{\"value1\":\"v1.0\",\"value2\":2.0},{\"value1\":\"v1.1\",\"value2\":2.1}]}";

    class RapidJsonTests : public ::testing::Test
    {
	  protected:
		void Verify(rapidjson::Document& document)
		{
			ASSERT_TRUE(document.IsObject()) << "document.IsObject()";

			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			document.Accept(writer);

			ASSERT_TRUE(strcmp(jsonCompact, buffer.GetString())) << " buffer.GetString()";
		}

	 public:

         void ParseInsitu()
         {
             char buffer[sizeof(json)];
             memcpy(buffer, json, sizeof(json));

             rapidjson::Document document;
             bool err = document.ParseInsitu(buffer).HasParseError();
             ASSERT_FALSE(err) << "ParseInsitu";

             Verify(document);
         }
         void Parse()
         {
			char buffer[sizeof(json)];
			memcpy(buffer, json, sizeof(json));

			rapidjson::Document document;
			bool err = document.Parse(buffer).HasParseError();
			ASSERT_FALSE(err) << "Parse";

			Verify(document);
         }

         void Serialize()
		 {
                JsonBase jsonBase("String", 123, false);
    			jsonBase.jsonValues.push_back(move(unique_ptr<JsonValue>(new JsonValue("v1.0", 2.0))));
    			jsonBase.jsonValues.push_back(move(unique_ptr<JsonValue>(new JsonValue("v1.1", 2.1))));


                rapidjson::StringBuffer stringBuffer;

                rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);

                jsonBase.Serialize(writer);

    			string pJson = stringBuffer.GetString();

                //OutputDebugStringA("\n*** Expected ***\n");
    			//OutputDebugStringA(jsonCompact);
    			//OutputDebugStringA("\n*** Actual ***\n");
    			//OutputDebugStringA(pJson);

    			ASSERT_STREQ(jsonCompact, pJson.c_str());
		 }

         void SerializeHelperWriteStreamString()
         {
                JsonBase jsonBase("String", 123, false);
    			jsonBase.jsonValues.push_back(move(unique_ptr<JsonValue>(new JsonValue("v1.0", 2.0))));
    			jsonBase.jsonValues.push_back(move(unique_ptr<JsonValue>(new JsonValue("v1.1", 2.1))));

    			JsonWriteStreamString stringWrite;
    			rapidjson::Writer<JsonWriteStreamString> writer(stringWrite);
    			jsonBase.Serialize(writer);

    			string pJson = stringWrite;

                //OutputDebugStringA("\n*** Expected ***\n");
    			//OutputDebugStringA(jsonCompact);
    			//OutputDebugStringA("\n*** Actual ***\n");
    			//OutputDebugStringA(pJson);

    			ASSERT_STREQ(jsonCompact, pJson.c_str());
         }

  };

  TEST_F(RapidJsonTests, ParseInsitu)
  {
      ParseInsitu();
  }

  TEST_F(RapidJsonTests, Parse)
  {
      Parse();
  }

  TEST_F(RapidJsonTests, Serialize)
  {
      Serialize();
  }

  TEST_F(RapidJsonTests, SerializeHelperWriteStreamString)
  {
      SerializeHelperWriteStreamString();
  }
 

}

