#include "stdafx.h"
#include "PollerProtocol/PollerAuthorizePINResponseMessage.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "UnitTestingUtils.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class PollerAuthorizePINResponseMessage : public ::testing::Test
	{
	public:
		void AssertEqual(CPollerAuthorizePINResponseMessage &obj, 
			const byte cardIdExpected[CARD_ID_LEN],
			const byte authorizeResultExpected,
			int testIndex,
			LPCTSTR functionName,
			int lineNumber,
			const int* extraIndex = NULL)
		{
			CUnitTestingUtils::AssertEqual(obj.GetCardID(), CARD_ID_LEN, cardIdExpected, CARD_ID_LEN,
				"obj.GetCardID()", &testIndex, functionName, lineNumber, extraIndex);

			ASSERT_EQ(obj.GetAuthorizeResult(), authorizeResultExpected) << "obj.GetAuthorizeResult() for testIndex = " << testIndex;
		}
	};

	/// <summary>
	/// Functional test for:
	///     CPollerAuthorizePINResponseMessage::CPollerAuthorizePINResponseMessage()
	///     CPollerAuthorizePINResponseMessage::GetCardID()
	///     CPollerAuthorizePINResponseMessage::GetAuthorizeResult()
	/// Code coverage for:
	///     CPollerAuthorizePINResponseMessage::~CPollerAuthorizePINResponseMessage()
	TEST_F(PollerAuthorizePINResponseMessage, CPollerAuthorizePINResponseMessageTest)
	{
		typedef struct
		{
			// Input(s).
			const BYTE* const m_response;

			// Expected result(s).
			const byte m_cardIdExpected[CARD_ID_LEN];
			const byte m_resultExpected;
		} TestDataType;

		const BYTE response1[] = {
			0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
			1, // authorize_result.
		};

		const BYTE response2[] = {
			0x12, 0x34, 0x56, 0x78, 0x77, // card_id.
			5, // authorize_result.
		};

		const BYTE *response3 = nullptr;

		const TestDataType TEST_DATA[] =
		{
			{ response1, { 0x12, 0x34, 0x56, 0x78, 0x90 }, 1 },
			{ response2, { 0x12, 0x34, 0x56, 0x78, 0x77 }, 5 },
			{ response3, { 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
		};

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Perform operation(s) to be tested.
			CPollerAuthorizePINResponseMessage varObj(TEST_DATA[testIndex].m_response);

			AssertEqual(varObj,
				TEST_DATA[testIndex].m_cardIdExpected,
				TEST_DATA[testIndex].m_resultExpected,
				testIndex, __TFUNCTION__, __LINE__, NULL);
		}
	}
}
