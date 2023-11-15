#include "stdafx.h"
#include "TestUtil.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{

TEST_CLASS(UnitTestingUtils_AssertMsg_Tests)
{
public:
	TEST_METHOD(FormatPlain)
	{
		const wchar_t* pResult = AssertMsg::FormatOnly(L"%d", 123);
		Assert::AreEqual(L"123", pResult, true);
	}

	TEST_METHOD(FormatLongInternal)
	{
		const wchar_t*
		pResult = AssertMsg::Format(1, L"pFieldName", L"S=%s N=%d", L"s", 12345);
		Assert::AreEqual(L"Line 1: pFieldName: S=s N=12345", pResult, true);

		pResult = AssertMsg::Format(-1, L"pFieldName", L"%s=%d", L"s", 12345);
		Assert::AreEqual(L"pFieldName: s=12345", pResult, true);

		pResult = AssertMsg::Format(2, nullptr, L"%d", 2468135);
		Assert::AreEqual(L"Line 2: 2468135", pResult, true);

		pResult = AssertMsg::Format(3, L"FieldName");
		Assert::AreEqual(L"Line 3: FieldName", pResult, true);

		pResult = AssertMsg::Format(4, nullptr);
		Assert::AreEqual(L"Line 4", pResult, true);

		pResult = AssertMsg::Format(-1, nullptr);
		Assert::AreEqual(L"", pResult, true);

		pResult = AssertMsg::Format(-1, L"nullptr");
		Assert::AreEqual(L"nullptr", pResult, true);
	}

	TEST_METHOD(FormatLongExternal)
	{
		wchar_t buffer[64];
		const wchar_t*
		pResult = AssertMsg::Format(buffer, _countof(buffer), 1, L"pFieldName", L"S=%s N=%d", L"s", 12345);
		Assert::AreEqual(L"Line 1: pFieldName: S=s N=12345", pResult, true);

		pResult = AssertMsg::Format(buffer, _countof(buffer), -1, L"pFieldName", L"%s=%d", L"s", 12345);
		Assert::AreEqual(L"pFieldName: s=12345", pResult, true);

		pResult = AssertMsg::Format(buffer, _countof(buffer), 2, nullptr, L"%d", 2468135);
		Assert::AreEqual(L"Line 2: 2468135", pResult, true);

		pResult = AssertMsg::Format(buffer, _countof(buffer), 3, L"FieldName");
		Assert::AreEqual(L"Line 3: FieldName", pResult, true);

		pResult = AssertMsg::Format(buffer, _countof(buffer), 4, nullptr);
		Assert::AreEqual(L"Line 4", pResult, true);

		pResult = AssertMsg::Format(buffer, _countof(buffer), -1, nullptr);
		Assert::AreEqual(L"", pResult, true);

		pResult = AssertMsg::Format(buffer, _countof(buffer), -1, L"nullptr");
		Assert::AreEqual(L"nullptr", pResult, true);

		pResult = AssertMsg::Format(buffer, _countof(buffer), -1, nullptr, L"pFormat");
		Assert::AreEqual(L"pFormat", pResult, true);

		pResult = AssertMsg::Format(buffer, _countof(buffer), -1, nullptr, nullptr);
		Assert::AreEqual(L"", pResult, true);

		pResult = AssertMsg::Format(buffer, _countof(buffer), -1, L"x", L"pFormat");
		Assert::AreEqual(L"x: pFormat", pResult, true);

		pResult = AssertMsg::Format(buffer, _countof(buffer), -1, L"xyz", L"%s%c", L"zy", L'x');
		Assert::AreEqual(L"xyz: zyx", pResult, true);
	}

	TEST_METHOD(FormatLongError)
	{
		wchar_t buffer[64];

		const wchar_t*
		pResult = AssertMsg::Format(nullptr, _countof(buffer), 1, L"pFieldName", L"S=%s N=%d", L"s", 12345);
		Assert::AreEqual(L"", pResult, true);

		pResult = AssertMsg::Format(buffer,  0, 3, L"9", L"%s", L"123");
		Assert::AreEqual(L"", pResult, true);

		pResult = AssertMsg::Format(buffer,  1, 3, L"9", L"%s", L"123");
		Assert::AreEqual(L"", pResult, true);

		pResult = AssertMsg::Format(buffer,  7, 3, L"9", L"%s", L"123");
		Assert::AreEqual(L"", pResult, true);

		pResult = AssertMsg::Format(buffer,  8, 3, L"9", L"%s", L"123");
		Assert::AreEqual(L"Line 3:", pResult, true);

		pResult = AssertMsg::Format(buffer,  9, 3, L"9", L"%s", L"123");
		Assert::AreEqual(L"Line 3: ", pResult, true);

		pResult = AssertMsg::Format(buffer, 10, 3, L"9", L"%s", L"123");
		Assert::AreEqual(L"Line 3: 9", pResult, true);

		pResult = AssertMsg::Format(buffer, 11, 3, L"9", L"%s", L"123");
		Assert::AreEqual(L"Line 3: 9", pResult, true);

		pResult = AssertMsg::Format(buffer, 12, 3, L"9", L"%s", L"123");
		Assert::AreEqual(L"Line 3: 9", pResult, true);

		pResult = AssertMsg::Format(buffer, 13, 3, L"9", L"%s", L"123");
		Assert::AreEqual(L"Line 3: 9: 1", pResult, true);

		pResult = AssertMsg::Format(buffer, 14, 3, L"9", L"%s", L"123");
		Assert::AreEqual(L"Line 3: 9: 12", pResult, true);

		pResult = AssertMsg::Format(buffer, 15, 3, L"9", L"%s", L"123");
		Assert::AreEqual(L"Line 3: 9: 123", pResult, true);

		pResult = AssertMsg::Format(buffer, 16, 3, L"9", L"%s", L"123");
		Assert::AreEqual(L"Line 3: 9: 123", pResult, true);
	}
};
}