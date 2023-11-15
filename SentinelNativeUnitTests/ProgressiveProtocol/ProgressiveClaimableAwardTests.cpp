#if 000//TODO-PORT
#include "stdafx.h"
#include "ProgressiveLevelTestHelpers.h"
#include "ProgressiveProtocol\ProgressiveClaimableAward.h"
#include "TestUtil.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SentinelNativeUnitTests
{
	using namespace ProgressiveLevelTestHelpers;

	static const char* pJson =
		"{"
			"\"Amount\":1.23,"
			"\"SessionId\":45,"
			"\"ClaimableAcceptanceUrl\":\"URL\","
			"\"ClaimableExpirationMillis\":67,"
			"\"ClaimableSamePlayer\":true"
		"}";

	static const char* pJson2 =
		"{"
			"\"Amount\":1.23,"
			"\"ClaimableAcceptanceUrl\":\"URL\","
			"\"ClaimableExpirationMillis\":67,"
			"\"ClaimableSamePlayer\":true"
		"}";

	static const char* pJson3 =
		"{"
			"\"Amount\":1.23,"
			"\"ClaimableAcceptanceUrl\":\"URL\","
			"\"ClaimableSamePlayer\":true"
		"}";

	TEST_CLASS(ProgressiveClaimableAwardTests)
	{
	public:
		TEST_METHOD(ProgressiveClaimableAward_Constructor)
		{
			OutputDebugStringA("### pJson ###\n");
			OutputDebugStringA(pJson);
			OutputDebugStringA("\n### pJson ###\n");

			try
			{
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(pJson).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, L"Parse");

				CProgressiveClaimableAward award(1000, jsonDoc);

				Assert::AreEqual(1000LU		, award.m_receivedTickCount		, L"m_receivedTickCount");
				Assert::AreEqual(L"1.23"	, award.m_amount				, L"m_amount");
				Assert::IsTrue(award.m_sessionId.isValid					, L"m_sessionId.isValid");
				Assert::AreEqual(45			, award.m_sessionId.val			, L"m_sessionId.val");
				Assert::AreEqual(_T("URL")	, award.m_acceptanceUrl			, L"m_acceptanceUrl");
				Assert::AreEqual(67			, (int) award.m_expirationMilliseconds,L"m_expirationMilliseconds");
				Assert::AreEqual(true		, award.m_isSamePlayer			, L"m_isSamePlayer");
			}
			catch (JsonException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename()));
			}
			catch (SentinelException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what()));
			}
		}

		TEST_METHOD(ProgressiveClaimableAward_Constructor2)
		{
			OutputDebugStringA("### pJson2 ###\n");
			OutputDebugStringA(pJson2);
			OutputDebugStringA("\n### pJson2 ###\n");

			try
			{
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(pJson2).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, L"Parse");

				CProgressiveClaimableAward award(1000, jsonDoc);

				Assert::AreEqual(1000LU		, award.m_receivedTickCount		, L"m_receivedTickCount");
				Assert::AreEqual(L"1.23"	, award.m_amount				, L"m_amount");
				Assert::IsFalse(award.m_sessionId.isValid					, L"m_sessionId.isValid");
				Assert::AreEqual(_T("URL")	, award.m_acceptanceUrl			, L"m_acceptanceUrl");
				Assert::AreEqual(67			, (int) award.m_expirationMilliseconds,L"m_expirationMilliseconds");
				Assert::AreEqual(true		, award.m_isSamePlayer			, L"m_isSamePlayer");
			}
			catch (JsonException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename()));
			}
			catch (SentinelException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what()));
			}
		}

		TEST_METHOD(ProgressiveClaimableAward_Constructor3)
		{
			OutputDebugStringA("### pJson3 ###\n");
			OutputDebugStringA(pJson3);
			OutputDebugStringA("\n### pJson3 ###\n");

			try
			{
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(pJson3).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, L"Parse");

				CProgressiveClaimableAward award(1000, jsonDoc);

				Assert::AreEqual(1000LU		, award.m_receivedTickCount		, L"m_receivedTickCount");
				Assert::AreEqual(L"1.23"	, award.m_amount				, L"m_amount");
				Assert::IsFalse(award.m_sessionId.isValid					, L"m_sessionId.isValid");
				Assert::AreEqual(_T("URL")	, award.m_acceptanceUrl			, L"m_acceptanceUrl");
				Assert::AreEqual(0			, (int) award.m_expirationMilliseconds,L"m_expirationMilliseconds");
				Assert::AreEqual(true		, award.m_isSamePlayer			, L"m_isSamePlayer");
			}
			catch (JsonException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename()));
			}
			catch (SentinelException& e)
			{
				Assert::Fail(AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what()));
			}
		}
	};
}
#endif //TODO-PORT