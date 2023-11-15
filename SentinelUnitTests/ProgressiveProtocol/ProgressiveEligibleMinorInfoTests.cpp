#include "stdafx.h"
#include "TestUtil.h"
#include "ProgressiveLevelTestHelpers.h"
#include "ProgressiveProtocol/ProgressiveEligibleMinorInfo.h"
#include "RapidJsonHelper.h"
#include "AssertMsg.h"

#include "gtest/gtest.h"

using namespace std;
class ProgressiveEligibleMinorInfo;
namespace SentinelNativeUnitTests
{
	class ProgressiveEligibleMinorInfoTests : public ::testing::Test
	{
	public:
		void f_ProgressiveEligibleMinorInfo_ctor(void)
		{
			static const char* pMinor =
				"{"
				"\"Minors\":["
				"{"
				"\"Sequence\":1,"
				"\"Eligible\":true,"
				"\"PercentEligible\": 7.89,"
				"\"Name\":\"name\","
				"\"Min\":1.23,"
				"\"Max\":4.56"
				"}"
				"]"
				"}";

			try
			{
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(pMinor).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

				const rapidjson::Value& minors = JsonGetArrayEx(jsonDoc, "Minors");
				auto it = minors.Begin();
				ASSERT_TRUE(it != minors.End()) << "it != minors.End()";

				ProgressiveEligibleMinorInfo objMinor(*it);
				objMinor.IsEligible();
				ASSERT_TRUE(objMinor.IsEligible()) << "IsEligible()";
				ASSERT_EQ(1, objMinor.GetSequence()) << "GetSequence()";
				ASSERT_EQ((int)123, (int)objMinor.GetMin()) << "GetMin()";
				ASSERT_EQ((int)456, (int)objMinor.GetMax()) << "GetMax()";
				ASSERT_STREQ("name", objMinor.GetName().c_str()) << L"GetName()";
				ASSERT_EQ(7.89, objMinor.GetPercentEligible()) << "GetPercentEligible()";

				string expectedJSON = 
					"\"Minors\"[{\"Sequence\":1,\"Eligible\":true,\"PercentEligible\":7.89,"					
					"\"Name\":\"name\",\"Min\":1.23,\"Max\":4.56,\"AwardType\":\"\"}]";
				string JSONstring = objMinor.GetJsonString();
				ASSERT_EQ(expectedJSON, JSONstring) << "GetJsonString()";


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

	};

	TEST_F(ProgressiveEligibleMinorInfoTests, ProgressiveEligibleMinorInfo_ctor)
	{
		f_ProgressiveEligibleMinorInfo_ctor();
	}
}

