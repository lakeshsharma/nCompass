#include "stdafx.h"
#include "PollerProtocol/PollerEmployeeResponseMessage.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "UnitTestingUtils.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class PollerEmployeeResponseMessage : public ::testing::Test
	{
	public:
		void AssertEqual(CPollerEmployeeResponseMessage &obj,
			const byte cardIdExpected[CARD_ID_LEN],
			const DWORD pinExpected,
			int testIndex,
			LPCTSTR functionName,
			int lineNumber,
			const int* extraIndex = NULL)
		{
			CUnitTestingUtils::AssertEqual(obj.GetCardID(), CARD_ID_LEN, cardIdExpected, CARD_ID_LEN,
				"obj.GetCardID()", &testIndex, functionName, lineNumber, extraIndex);

			ASSERT_EQ(obj.GetPIN(), pinExpected) << "obj.GetPIN() for testIndex = " << testIndex;
		}
	};

	/// <summary>
	/// Functional test for:
	///     CPollerEmployeeResponseMessage::CPollerEmployeeResponseMessage()
	///     CPollerEmployeeResponseMessage::GetPIN()
	///     CPollerEmployeeResponseMessage::GetCardID()
	/// Code coverage for:
	///     CPollerEmployeeResponseMessage::~CPollerEmployeeResponseMessage()
	TEST_F(PollerEmployeeResponseMessage, CPollerEmployeeResponseMessageTest)
	{
		typedef struct
		{
			// Input(s).
			const BYTE* const m_response;

			// Expected result(s).
			const byte m_cardIdExpected[CARD_ID_LEN];
			const DWORD m_pinExpected;
		} TestDataType;

		const BYTE response1[] = {
			0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
			1, 2, 3, 4, // pin.
		};

		const BYTE response2[] = {
			0x12, 0x34, 0x56, 0x78, 0x90, // card_id.
			0, 0, 0, 0, // pin.
		};

		const BYTE *response3 = nullptr;

		const TestDataType TEST_DATA[] =
		{
			{ response1, { 0x12, 0x34, 0x56, 0x78, 0x90 }, 8208},
			{ response2, { 0x12, 0x34, 0x56, 0x78, 0x90 }, 5797},
			{ response3, { 0x00, 0x00, 0x00, 0x00, 0x00 }, 5797},
		};

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Perform operation(s) to be tested.
			CPollerEmployeeResponseMessage varObj(TEST_DATA[testIndex].m_response);

			AssertEqual(varObj,
				TEST_DATA[testIndex].m_cardIdExpected,
				TEST_DATA[testIndex].m_pinExpected,
				testIndex, __TFUNCTION__, __LINE__, NULL);
		}
	}
}
