#if 000//TODO-PORT
#include "stdafx.h"
#include "TestUtil.h"
#include "ProgressiveProtocol\ProgressiveLevelBroadcast.h"
#include "ProgressiveLevelTestHelpers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SentinelNativeUnitTests
{
	using namespace ProgressiveLevelTestHelpers;

	static const char* pJson =
	"{"
		"\"LevelValues\":["
		"{"
			"\"LevelId\":5667,"
			"\"Value\":20.00,"
			"\"Name\":\"a\""
		"},"
		"{"
			"\"LevelId\":61234,"
			"\"Value\":12.34,"
			"\"Name\":\"b\""
		"},"
		"{"
			"\"LevelId\":5,"
			"\"Value\":655.36,"
			"\"Name\":\"c\""
		"}]"
	"}";

	struct
	{
		int64_t id;
		int64_t value;
		LPCWSTR pName;
	}
	static data[] =
	{
		{ 5667,  2000,  L"a", },
		{ 61234, 1234,  L"b", },
		{ 5,	 65536, L"c", }
	};

	TEST_CLASS(ProgressiveLevelBroadcastTests)
	{
	public:
		TEST_METHOD(ProgressiveLevelBroadcast_Constructor)
		{
			OutputDebugStringA("### pJson ###\n");
			OutputDebugStringA(pJson);
			OutputDebugStringA("\n### pJson ###\n");

			rapidjson::Document jsonDoc;
			bool result = jsonDoc.Parse(pJson).HasParseError();
			Assert::IsFalse(result, AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError())));

			const DWORD receivedTickCount = 20000;
			CProgressiveLevelBroadcast levelBroadcast(receivedTickCount, jsonDoc);

			Assert::AreEqual((long) receivedTickCount, (long) levelBroadcast.m_receivedTickCount, L"levelBroadcast.m_receivedTickCount");
			Assert::AreEqual(_countof(data), levelBroadcast.m_levels.size(), L"levelBroadcast.m_levels.size()");

			for (size_t i = 0; i < 3; i++)
			{
				Assert::AreEqual((long) data[i].id,	   (long) levelBroadcast.m_levels[i].id,	AssertMsg::FormatOnly(L"m_levels[%d].m_id",    i));
				Assert::AreEqual((long) data[i].value, (long) levelBroadcast.m_levels[i].value, AssertMsg::FormatOnly(L"m_levels[%d].m_value", i));
				Assert::AreEqual(       data[i].pName, levelBroadcast.m_levels[i].name.c_str(), AssertMsg::FormatOnly(L"m_levels[%d].m_name",  i));
			}
		}

		TEST_METHOD(ProgressiveLevelBroadcast_Level_CopyConstructor)
		{
			const __int64 id = 1234;
			const __int64 value = 5678;
			const wchar_t* pName = L"9";

			CProgressiveLevelBroadcast::Level levelOrg(id, value, pName);
			CProgressiveLevelBroadcast::Level level(levelOrg);

			Assert::AreEqual((long) id,	   (long) level.id,	 AssertMsg::FormatOnly(L"level.m_id"));
			Assert::AreEqual((long) value, (long) level.value, AssertMsg::FormatOnly(L"level.m_value"));
			Assert::AreEqual(       pName, level.name.c_str(), AssertMsg::FormatOnly(L"level.m_name"));
		}
	};
}
#endif //TODO-PORT