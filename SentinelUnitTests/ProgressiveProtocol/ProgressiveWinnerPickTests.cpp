#include "stdafx.h"
#include "TestUtil.h"
#include "ProgressiveLevelTestHelpers.h"
#include "ProgressiveProtocol/ProgressiveWinnerPick.h"
#include "AssertMsg.h"

#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	using namespace ProgressiveLevelTestHelpers;

	static const char* pJson =
		"{"
			"\"LevelId\":1,"
			"\"LevelName\":\"José\""
		"}";

		TEST(ProgressiveWinnerPickTests, ProgressiveWinnerPick_Constructor)
		{

			try
			{
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(pJson).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

				CProgressiveWinnerPick pick(1000, jsonDoc);
				ASSERT_EQ(1, pick.m_levelId) << L"LevelId";
				ASSERT_STREQ("José", (LPCTSTR) pick.m_levelName.c_str()) <<  L"LevelId";
			}
			catch (JsonException& e)
			{
				ASSERT_TRUE(false) << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename());
			}
			catch (SentinelException& e)
			{
				ASSERT_TRUE(false) << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what());
			}
		}
}
