#include "stdafx.h"
#include "PollerProtocol/PollerAuthorizationMessage.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "UnitTestingUtils.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class PollerAuthorizationMessage : public ::testing::Test
	{
	public:
		void AssertEqual(CPollerAuthorizationMessage &obj,
			const std::string txIdExpected,
			const byte authorizationGrantedExpected,
			int testIndex)
		{
			ASSERT_EQ(obj.GetTransactionID(), txIdExpected) << "obj.GetTransactionID() for testIndex = " << testIndex;
			ASSERT_EQ(obj.IsAuthorized(), authorizationGrantedExpected) << "obj.IsAuthorized() for testIndex = " << testIndex;
		}
	};

	/// <summary>
	/// Functional test for:
	///     CPollerAuthorizationMessage::CPollerAuthorizationMessage()
	///     CPollerAuthorizationMessage::IsAuthorized()
	///     CPollerAuthorizationMessage::GetTransactionID()
	/// Code coverage for:
	///     CPollerAuthorizationMessage::~CPollerAuthorizationMessage()
	TEST_F(PollerAuthorizationMessage, CPollerAuthorizationMessageTest)
	{
		typedef struct
		{
			// Input(s).
			const BYTE* const m_response;

			// Expected result(s).
			const std::string m_txIdExpected;
			const byte m_isAuthorizedExpected;
		} TestDataType;

		const BYTE response1[] = {
			5, // transaction id length
			't', 'x', '_', '0', '1', // transaction id
			1, // authorization granted
		};

		const BYTE response2[] = {
			6, // transaction id length
			't', 'x', '_', '0', '0', '2', // transaction id
			0, // authorization granted
		};

		const BYTE *response3 = nullptr;

		const TestDataType TEST_DATA[] =
		{
			{ response1, "tx_01", 1 },
			{ response2, "tx_002", 0 },
			{ response3, "", 0 },
		};

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Perform operation(s) to be tested.
			CPollerAuthorizationMessage varObj(TEST_DATA[testIndex].m_response);

			AssertEqual(varObj,
				TEST_DATA[testIndex].m_txIdExpected,
				TEST_DATA[testIndex].m_isAuthorizedExpected,
				testIndex);
		}
	}
}
